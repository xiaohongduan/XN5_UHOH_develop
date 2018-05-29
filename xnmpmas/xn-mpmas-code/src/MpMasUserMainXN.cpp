//------------------------------------------------------------------------------
//
//  Christian Troost, Christian Klein, Florian Heinlein, Thomas Berger
//
//
//  Hohenheim University (490d)
//  Professorship for Land Use Economics in the Tropics and Subtropics
//
//  May not be distributed without permission by main author
//
//
//  Contents:   User main
//
//
//
//------------------------------------------------------------------------------
#include <cstdio>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include "MpmasMaster.h"
#include "Raster2D.h"
#include <execinfo.h>


#if defined(PAR2)
	#include "mpi.h"
#endif //PAR2

#include "xpn_main.h"
#include "xn_mpmas_translator.h"

namespace xnmpmas {
	void abort_simulation(std::string reason) 
	{
		cerr << reason << endl;
		cout << "ABORTING ..." << endl;

		//int* makeSegfault;
		//printf("%d", *makeSegfault);

		#if defined(PAR2)
			MPI_Abort(MPI_COMM_WORLD,1);
		#endif

		  void *array[50];
		  size_t size;

		  // get void*'s for all entries on the stack
		  size = backtrace(array, 10);

		  // print out all the frames to stderr
		  
		  backtrace_symbols_fd(array, size, STDERR_FILENO);
		  exit(1);


		exit(1);
	}
}


namespace mpmasScope
{	// just for now, so that this simple exit function can still be used
	#ifdef ODB
		extern mpmas* mpmasPtr;
		extern void exitfun();
	#else
		mpmas* mpmasPtr;
		void exitfun();
		region* wshed;
	#endif //ODB
};

using namespace mpmasScope;

#define XNMPMASDBG 1


int main(int ac, char **av)
{
	printf("XN-MPMAS version 29\n");
	try {
		//Input argument handling
		xn_mpmas_options configuration (ac,av);
		xn_mpmas_translator translator(configuration.getCouplingType(), configuration.getFnLuaParameters().c_str(), configuration.getFnCropGrids().c_str(), configuration.getFnCellAllocation().c_str(), configuration.getStartDate() );

		// XPN Class
		xpn_main *xpn;

		int world_size = 1; //default values also necessary if no MPI is used (Expert-N 5.0 req)
		int my_rank = 0;
	
#if defined(PAR2)
		MPI_Init(&ac, &av);
		
		double start, finish, time;	

		MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
		start = MPI_Wtime();

		
		MPI_Comm_size(MPI_COMM_WORLD, &world_size);
		if (my_rank == 0)
		{
			printf("MPI info: using %d MPI processes\n", world_size);
		}
		{//BEGIN ARTIFICAL ENCLOSING BLOCK TO MAKE SURE mpmas IS destroyed BEFORE MPI_Finalize() is called;
#endif //PAR2	

/*
 * Expert-N initialization 
 */

			printf("Processor %d: Starting Expert-N initialization ...\n", my_rank);
			int xnRetcode;
			// create instance of XPN 5.0:
			xpn = xpn_main_new (my_rank,world_size);
			
			vector<string> xpnOptions = configuration.getXpnOptions();
			int xpn_argc = xpnOptions.size();
			char** xpn_argv = new char* [xpn_argc];
			for (int i = 0; i < xpn_argc; i++)
			{	xpn_argv[i] = (char*) xpnOptions[i].c_str();
			}
			xnRetcode = xpn_main_parse_command_line(xpn,xpn_argc,(char**)xpn_argv);
			if ( xnRetcode !=RET_SUCCESS)
			{
				xpn = xpn_main_done(xpn);
				return RET_SUCCESS;
			}
			xnRetcode =  xpn_main_load_cfg_and_reserve_memory(xpn);
			if ( xnRetcode != RET_SUCCESS)
			{
				xpn = xpn_main_done(xpn);
				return RET_SUCCESS;
			}
			printf("Processor %d: Finished Expert-N initialization ...\n", my_rank);
			int matrix_size_x = xpn->matrix_size_x;
			int matrix_size_y = xpn->matrix_size_y;
			int grid_layers = xpn->grid_layers;
			int xnGridSize = matrix_size_y * matrix_size_x;
			//xpn_main_save_current_state_of_registry(xpn);
			translator.setXnGridDimensions(matrix_size_x, matrix_size_y, grid_layers);
/*
 * MPMAS initialization 
 */			
			//create instance of Mpmas and set pointer for exit function


			mpmas* mpmasInstance = NULL;
			//if (!configuration.skipMpmasDecisions())
			//{
				printf("Processor %d: Starting Mpmas initialization ...\n", my_rank);
				vector<string> mpmasOptions = configuration.getMpmasOptions();
				int mpmas_argc = mpmasOptions.size();
				char** mpmas_argv = new char* [mpmas_argc];
				for (int i = 0; i < mpmas_argc; i++)
				{	mpmas_argv[i] = (char*) mpmasOptions[i].c_str();
				}
				mpmasInstance = new mpmas( mpmas_argc, (char**)mpmas_argv) ;				
				printf("Processor %d: Mpmas initialization finished...\n", my_rank);
			//}
			
			#ifndef ODB
				mpmasPtr = mpmas;
			#endif //ODB

/*
 * Management timing initialization 
 */				
			
			//initialize weighting scheme and weather records for previous years from file; 
			//important: do after mpmas initilaization as Raster2D in function uses TestFun
			printf("Processor %d: Initializing field work adaptation parameters...\n", my_rank);

			translator.readAdaptationParametersAndWeatherHistory(configuration.getFnAdaptationParameters(), configuration.getFnWeatherRecords(), configuration.areWeatherRecordsCellSpecific(), configuration.getFnCellWeatherLinkMap());
			printf("Processor %d: Finished initializing field work adaptation parameters and weather history...\n", my_rank);


/*
 * 		Coupling scheme preparations
 */
			//just for testing, assign everything static
			int firstyear = configuration.getFirstXpnYear();
			
			//get length of simulation horizon and number of spin-up rounds
			int numYears;
			int numSpinUp;
			
			if(configuration.skipMpmasDecisions())
			{
					numYears = configuration.getNumberOfYearsToSimulate();
					numSpinUp = 0;
			}		
			else
			{		numYears = mpmasInstance->getNumberOfYearsToSimulate();
					numSpinUp = mpmasInstance->getNumberOfSpinUpRounds();
					configuration.setNumberOfYearsToSimulate(numYears);
					configuration.setScenarioName(mpmasInstance->getScenarioName());
					
					
			}
			// create arrays for communication
			int mpmasGridSize = 0; 
			switch(configuration.getCouplingType() )
			{
				case xnmpmasCouplingFixedMaps:
					mpmasGridSize = xnGridSize;
					break;		
				
				case xnmpmasCouplingVirtualSlots:
					//TODO:
					//check consistency of Mpmas settings: 
					if(  mpmasGlobal::CROPG_MDL() != typeCropLucia || mpmasGlobal::SUBVR_MDL() != subversionLookUpTables) 
					{
						sprintf(fehlertxt, "Error (Processor %d): Coupling type %d incompatible with MPMAS input:\n"
											"CROP_MODEL: %d, BIOVERSION, %d\n"
											, my_rank, xnmpmasCouplingVirtualSlots, mpmasGlobal::CROPG_MDL(),mpmasGlobal::SUBVR_MDL() );
						fehlerabbruch();
					}
				
					mpmasGridSize = mpmasInstance->getNumberOfCroppingActivities();
					break;
				case xnmpmasCouplingOneToOne:
					if(  mpmasGlobal::CROPG_MDL() != typeCropLucia || mpmasGlobal::SUBVR_MDL() != subversionExtYieldsMaps) 
					{
						sprintf(fehlertxt, "Error (Processor %d): Coupling type %d incompatible with MPMAS input:\n"
											"CROP_MODEL: %d, BIOVERSION, %d\n"
											, my_rank, xnmpmasCouplingOneToOne, mpmasGlobal::CROPG_MDL(),mpmasGlobal::SUBVR_MDL() );
						fehlerabbruch();
					}
					break;
				default:
					sprintf(fehlertxt, "Error: Unknown coupling type! (Processor %d)\n", my_rank);
					fehlerabbruch();		
						
			}
			int* cropActIDX = new int [mpmasGridSize];
			double* cropAreaX = new double [mpmasGridSize];
			double* cropYieldX = new double [mpmasGridSize];
			double* stoverYieldX = new double [mpmasGridSize];
			Raster2D*  luaMap = NULL;
			Raster2D*  yield1Map = NULL;
			Raster2D*  yield2Map = NULL;
/*
 * 	START COUPLED SIMULATIONS
 */
			//loop over simulation horizon, starting with end of period 0
			//in case of spin-up, start counting with negative value
			printf("Processor %d: Starting to loop over simulation years...\n", my_rank);


			for (int year = -numSpinUp; year <= numYears; year++) //Note: In last year is stopped after last harvest!
			//for (int year = -numSpinUp; year < numYears; year++)

			{
				//agents make final decision for last year, plan the coming year and ahead
				//update their accounts and income calculation
				//decide whether to continue or to exit
				//and implement investments
				
				int rtcode = 0;
				try {
					if (!configuration.skipMpmasDecisions())
					{	printf("Processor %d: MPMAS: Agents make start of year decision...\n", my_rank);
						rtcode = mpmasInstance->agentsUpdateFarmAssetsAndPlanningAtPeriodSwitch(year);						
					}
				}
				catch (exception & e)
				{
					stringstream errmsg;
					errmsg << "Processor "<< my_rank << ": Error in <agentsUpdateFarmAssetsAndPlanningAtPeriodSwitch>:" << e.what() <<"\n";
					throw runtime_error(errmsg.str());
				}
				
				//stop prematurely if needed
				if(0 < rtcode)
				{	break;
				}
				//get land use from MPMAS
				//formatted according to chosen crop growth model
				

				switch(configuration.getCouplingType() )
				{
					case xnmpmasCouplingFixedMaps:
					{	//transfer from map
						printf("\nProcessor %d: Reading land use from maps:\n", my_rank);					
						stringstream fnLuaMapThisYear;
						fnLuaMapThisYear << configuration.getFnLuaMap()
						<< setw(2)<< setfill('0')  << year << ".txt";
						delete luaMap;
						luaMap = new Raster2D(fnLuaMapThisYear.str().c_str());

						translator.defineLuaForCellsFromMap(luaMap);
						
					}
						break;
					case xnmpmasCouplingVirtualSlots:
						//export to table
						printf("\nProcessor %d:MPMAS communicates land use decisions:\n", my_rank);					
						mpmasInstance->agentsImplementLandUse_LookUpTable(mpmasGridSize, cropActIDX, cropAreaX);
						translator.defineLuaforCellsFromArray(year, mpmasGridSize, cropActIDX, cropAreaX);
						break;
					case xnmpmasCouplingOneToOne:
						{	//transfer from map
							printf("\nProcessor %d: MPMAS produces land use maps from agent decisions::\n", my_rank);
							delete luaMap;
							luaMap = new Raster2D();
							mpmasInstance->agentsImplementLandUse_Maps(*luaMap);					
							
					 	 	stringstream fnLuMap;
					 	 	fnLuMap << mpmasInstance->getOutputDirectory()<<"/out/" << mpmasInstance->getScenarioName() << "LanduseMap"
					 			 	 	 << setw(2) <<setfill('0') << year << ".txt";
					 		luaMap->writeToFile(fnLuMap.str());
					 	 
							translator.defineLuaForCellsFromMap(luaMap);
							
						}
						break;	
						
					default:
						sprintf(fehlertxt, "Error: Unknown coupling type! (Processor %d)\n", my_rank);
						fehlerabbruch();		
						
				}
				
				//translate land use activity into variables for XN
				xnmpmasDate nextStop;
				nextStop = configuration.getlastHarvestDateThisSeason(firstyear + year - 1);
				
				printf("\nProcessor %d : ...set management on fields:\n", my_rank);	

//Begin of Hong:for gridId-loop 
				for (int gridId=0; gridId<grid_layers; gridId++)     
				{  
//End of Hong				
					for (int i=0; i< xnGridSize; i++)
					{	STRUCT_mpmas_to_xn tempStruct = translator.getManagementForCell(i, firstyear + year);		
						
						//Begin of Hong
						expertn_modul_base  *xpn_own_grid=xpn->grid_mpmas_to_xn[ gridId * xnGridSize +  i].xpn_own_grid;
						expertn_modul_base **xpn_other_grids=xpn->grid_mpmas_to_xn[ gridId * xnGridSize +  i].xpn_other_grids;
						//End of Hong
						
						tempStruct.stopDate = nextStop;
						xpn->grid_mpmas_to_xn[ gridId * xnGridSize +  i] = tempStruct;
					
						//Begin of Hong
						xpn->grid_mpmas_to_xn[ gridId * xnGridSize +  i].xpn_own_grid=xpn_own_grid;
						xpn->grid_mpmas_to_xn[ gridId * xnGridSize +  i].xpn_other_grids= xpn_other_grids;
						
						xpn->grid_mpmas_to_xn[ gridId * xnGridSize +  i].number_of_grids= xpn->grid_layers;
						xpn->grid_mpmas_to_xn[ gridId * xnGridSize +  i].own_grid_number= gridId;
						xpn->grid_mpmas_to_xn[ gridId * xnGridSize +  i].cellID= gridId * xnGridSize +  i;
						
	//Todo: soll von translator.getManagementForCell kommen:
						//xpn->grid_mpmas_to_xn[ gridId * xnGridSize +  i].currentGrid= 0; //does not need to be set here, because is already set in getManagementForCell
						xpn->grid_mpmas_to_xn[ gridId * xnGridSize +  i].nextGrid= 1; //currently ignored
						xpn->grid_mpmas_to_xn[ gridId * xnGridSize +  i].updateManagement= 1;
					}
					
				 
					if(XNMPMASDBG) 
					{ //pre-xpn-run debugging output
					
						if(my_rank == 0 )
						{	cout << "Cell\tCrop\tSowDate\tHarvestDate\n";
							for (int i=0; i<  xnGridSize; i++)
							{	
								cout << (i / matrix_size_y) << ", " << (i % matrix_size_y)  << "["<<gridId<<"]"<< "\t"; 
								cout << xpn->grid_mpmas_to_xn[gridId * xnGridSize +i].CropCode << "\t";
								cout << xpn->grid_mpmas_to_xn[gridId * xnGridSize +i].sowDate.day 
														<< "-" << xpn->grid_mpmas_to_xn[gridId * xnGridSize +i].sowDate.month
														<< "-" << xpn->grid_mpmas_to_xn[gridId * xnGridSize +i].sowDate.year << "\t";
								cout << xpn->grid_mpmas_to_xn[gridId * xnGridSize +i].harvestDate.day
														<< "-" << xpn->grid_mpmas_to_xn[gridId * xnGridSize +i].harvestDate.month
														<< "-" << xpn->grid_mpmas_to_xn[gridId * xnGridSize +i].harvestDate.year << "\t";
								cout << ((gridId == xpn->grid_mpmas_to_xn[gridId * xnGridSize +i].currentGrid) ? "X" : "-");

								cout << endl;						
							}	
						}
					}
//Begin of Hong				
				} //End of gridId-loop			  			  
// End of Hong!
				cout << "\nProcessor " << my_rank << ": Run XPN until LAST HARVEST STOP: "  << nextStop.day << "-" << nextStop.month << "-" << nextStop.year << endl	;
				xpn_main_run(xpn); //Run until LAST HARVEST STOP, after last harvest date of season to collect yields from last year
				printf("\nProcessor %d: LAST HARVEST STOP:\n", my_rank);
				xnmpmasDate curDate = nextStop; // copy before change
				
				// Synchornization of yield results from XN to all nodes
				MPI_Barrier(MPI_COMM_WORLD);
				xpn_main_mpi_share_results_on_all_nodes(xpn);
				MPI_Barrier(MPI_COMM_WORLD);
				
				if(XNMPMASDBG) 
				{//post-xpn-run debugging output
					
					if(my_rank == 0 )
					{
						
						stringstream fnDbgXnActualDates;
						fnDbgXnActualDates << "dbg_XnCommunicatedActualDates_" << setw(2) << setfill('0') << year << "b.txt";
						FILE* dbgXnActualDates = fopen(fnDbgXnActualDates.str().c_str(), "w" );
					
						
						printf("\n...yields of fields:\n");	



						cout << "Stored yield previous season's crop:\n";
						cout << "Cell\tFruitDM\tStem+LeaveDM\tNmin0_30\tNmin30_90\tNmin60_90\n";
		//Begin of Hong:for gridId-loop					
						for (int gridId=0; gridId<grid_layers; gridId++)     
						{  
		//End of Hong
							for (int i2=0;i2<matrix_size_x;i2++)
							{	for (int i=0;i<matrix_size_y;i++)
								{							
									//printf("i: %d, j: %d, cropCode: %s\n",i,i2,xpn->grid_mpmas_to_xn[i2*matrix_size_y+i].CropCode);
									if (configuration.isCleanSeason() || year != -numSpinUp) {

										//Begin of Hong
										//cout << (i / matrix_size_y) << ", " << (i % matrix_size_y) << "["<<gridId<<"]"<< "\t"; 
										cout << (i2) << ", " << (i) << "["<<gridId<<"]"<< "\t"; 
										//End of Hong
										cout << xpn->grid_xn_to_mpmas[gridId * xnGridSize +i2*matrix_size_y+i].fruitDryWeight <<"\t";
										cout << xpn->grid_xn_to_mpmas[gridId * xnGridSize +i2*matrix_size_y+i].stemLeafDryWeight <<"\t";
										cout << xpn->grid_xn_to_mpmas[gridId * xnGridSize +i2*matrix_size_y+i].Nmin0_30 <<"\t";
										cout << xpn->grid_xn_to_mpmas[gridId * xnGridSize +i2*matrix_size_y+i].Nmin30_60 <<"\t";
										cout << xpn->grid_xn_to_mpmas[gridId * xnGridSize +i2*matrix_size_y+i].Nmin60_90 <<"\t";
										//cout << ((gridId == xpn->grid_xn_to_mpmas[gridId * xnGridSize +i2*matrix_size_y+i].currentGrid) ? "X" : "-");
										cout << "\n";
									

										fprintf(dbgXnActualDates, "%d\t%d\t%d\t%02d-%02d-%02d\t%02d-%02d-%02d\t%02d-%02d-%02d\t%02d-%02d-%02d\t%02d-%02d-%02d\n",
																		i2, i, gridId,
																		xpn->grid_xn_to_mpmas[gridId * xnGridSize +i2*matrix_size_y+i].actualHarvestDate.day,
																		xpn->grid_xn_to_mpmas[gridId * xnGridSize +i2*matrix_size_y+i].actualHarvestDate.month,
																		xpn->grid_xn_to_mpmas[gridId * xnGridSize +i2*matrix_size_y+i].actualHarvestDate.year,
																		
																			xpn->grid_xn_to_mpmas[gridId * xnGridSize +i2*matrix_size_y+i].actualMinFertDate[0].day,
																			xpn->grid_xn_to_mpmas[gridId * xnGridSize +i2*matrix_size_y+i].actualMinFertDate[0].month,
																			xpn->grid_xn_to_mpmas[gridId * xnGridSize +i2*matrix_size_y+i].actualMinFertDate[0].year,
																			xpn->grid_xn_to_mpmas[gridId * xnGridSize +i2*matrix_size_y+i].actualMinFertDate[1].day,
																			xpn->grid_xn_to_mpmas[gridId * xnGridSize +i2*matrix_size_y+i].actualMinFertDate[1].month,
																			xpn->grid_xn_to_mpmas[gridId * xnGridSize +i2*matrix_size_y+i].actualMinFertDate[1].year,
																			xpn->grid_xn_to_mpmas[gridId * xnGridSize +i2*matrix_size_y+i].actualMinFertDate[2].day,
																			xpn->grid_xn_to_mpmas[gridId * xnGridSize +i2*matrix_size_y+i].actualMinFertDate[2].month,
																			xpn->grid_xn_to_mpmas[gridId * xnGridSize +i2*matrix_size_y+i].actualMinFertDate[2].year,
																			xpn->grid_xn_to_mpmas[gridId * xnGridSize +i2*matrix_size_y+i].actualMinFertDate[3].day,
																			xpn->grid_xn_to_mpmas[gridId * xnGridSize +i2*matrix_size_y+i].actualMinFertDate[3].month,
																			xpn->grid_xn_to_mpmas[gridId * xnGridSize +i2*matrix_size_y+i].actualMinFertDate[3].year
																	);
										
									/*
									// Begin of Hong
									printf("Id: %d, i: %d, j: %d, check_gridToChange: %f\n",gridId,i,i2,xpn->grid_xn_to_mpmas[gridId * xnGridSize +i2*matrix_size_y+i].check_gridToChange);
									printf("Id: %d, i: %d, j: %d, check_currentGrid: %f\n",gridId,i,i2,xpn->grid_xn_to_mpmas[gridId * xnGridSize +i2*matrix_size_y+i].check_currentGrid);
									printf("Id: %d, i: %d, j: %d, check_changed: %f\n",gridId,i,i2,xpn->grid_xn_to_mpmas[gridId * xnGridSize +i2*matrix_size_y+i].check_changed);
									//End of Hong
									 */
									 
									}
								
								/*	// Debugging output air and soil temperatures
									printf("\n...writing dbg_XnCommunicatedWeather_ file a\n");					

									stringstream fnDbgXnWeatherCom;
									fnDbgXnWeatherCom << "dbg_XnCommunicatedWeather_" << setw(2) << setfill('0') << year << "a.txt";
									FILE* dbgXnWeatherCom = fopen(fnDbgXnWeatherCom.str().c_str(), "w" );
									
									for(int i3=0;i3<XNMPMASDAYSOFYEAR;i3++)
									{   int isSet = 0;
										if (xpn->grid_xn_to_mpmas2[i2*matrix_size_y+i].startDay  <= i3 && xpn->grid_xn_to_mpmas2[i2*matrix_size_y+i].stopDay >= i3 )
											isSet = 1;
										fprintf(dbgXnWeatherCom, "i: %d, j: %d, SimulationDay: %d/%d, AirTemp: %f, SoilTemp: %f\n",i,i2,i3,isSet,xpn->grid_xn_to_mpmas2[i2*matrix_size_y+i].airTemp[i3],xpn->grid_xn_to_mpmas2[i2*matrix_size_y+i].topsoilTemp[i3]);	
									}
									fclose(dbgXnWeatherCom);*/
									
									
									
								}
							}
		//Begin of Hong 
						} //End of gridId-loop			  			  
		// End of Hong!
						fclose(dbgXnActualDates);
					}//endif my_rank == 0
				}//XNMPMASDBG
				
			
															
				if (configuration.isCleanSeason() || year != -numSpinUp) //in first year with overlapping seasons, no yields available yet
				{	printf("Processor %d: ...read yields\n", my_rank);
					switch(configuration.getCouplingType() )
					{   //TODO: currently done also in first year though though arrays are empty
						case xnmpmasCouplingFixedMaps:  //same as OneToOne
						case xnmpmasCouplingOneToOne:
							delete yield1Map;
							delete yield2Map;
							yield1Map = new Raster2D(*luaMap);
							yield2Map = new Raster2D(*luaMap);
							translator.calcYieldsToMaps(xpn->grid_xn_to_mpmas, yield1Map, yield2Map, !configuration.isCleanSeason());
							break;
						case xnmpmasCouplingVirtualSlots:
							translator.calcYieldsToArray(xpn->grid_xn_to_mpmas, cropYieldX, stoverYieldX, !configuration.isCleanSeason());
							break;
						default:
							sprintf(fehlertxt, "Error: Unknown coupling type! (Processor %d)\n", my_rank);
							fehlerabbruch();
					}
					
					if(mpmasGlobal::TestFun(19) && my_rank == 0)
					{
						char fnDbg [250];
						sprintf(fnDbg,"YieldArrayDebugging%02d.txt", year);
						printf("...writing YieldArrayDebugging file\n");			
						FILE* dbgFH = fopen(fnDbg,"w");
						if (dbgFH != NULL)
						{
							for(int i = 0; i < mpmasGridSize; i++)
							{
								fprintf(dbgFH, "%d\t%.2f\t%.2f\t%.2f\n",cropActIDX[i], cropAreaX[i], cropYieldX[i],stoverYieldX[i]);
							}
						}
						fclose(dbgFH);
					}
				
				}
				//process yields (communicate to MPMAS or write out
				
				switch(configuration.getCouplingType() )
				{	case xnmpmasCouplingFixedMaps:
						if (my_rank == 0)
						{ printf("...write yields to Maps\n");	
							if (yield1Map != NULL)
							{	stringstream fnYields1,fnYields2;
								fnYields1 << mpmasInstance->getOutputDirectory() <<"/out/" << configuration.getScenarioName() << "_yields1_"  <<  (firstyear + year) << ".txt";
								fnYields2 << mpmasInstance->getOutputDirectory() <<"/out/" << configuration.getScenarioName() << "_yields2_"  <<  (firstyear + year) << ".txt";
								yield1Map->writeToFile(fnYields1.str());
								yield2Map->writeToFile(fnYields2.str());							
							}
						}
#if XNMPMASDBG
						else  
						{ 
							if (yield1Map != NULL)
							{	stringstream fnYields1,fnYields2;
								fnYields1 << mpmasInstance->getOutputDirectory() <<"/out/" <<configuration.getScenarioName() << "_yields1_"  <<  (firstyear + year) << "_proc_" << my_rank << ".txt";
								fnYields2 << mpmasInstance->getOutputDirectory() <<"/out/" << configuration.getScenarioName() << "_yields2_"  <<  (firstyear + year)  << "_proc_" << my_rank << ".txt";
								yield1Map->writeToFile(fnYields1.str());
								yield2Map->writeToFile(fnYields2.str());							
							}
						}
#endif //XNMPMASDBG						
						break;
					case xnmpmasCouplingVirtualSlots:
						 //TODO: currently done also in first year though though arrays are empty
						printf("...communicate yields to MPMAS\n");	
						mpmasInstance->agentsReceiveYields_LookUpTable(cropYieldX, stoverYieldX );
						break;
					case xnmpmasCouplingOneToOne:
						if (my_rank == 0)
						{   printf("...write yields to maps for debugging\n");	
							if (yield1Map != NULL)
							{	stringstream fnYields1,fnYields2;
								fnYields1 << mpmasInstance->getOutputDirectory()<<"/out/" << mpmasInstance->getScenarioName() << "CropYields_"  <<  (firstyear + year) << ".txt";
								fnYields2 << mpmasInstance->getOutputDirectory()<<"/out/" << mpmasInstance->getScenarioName() << "StoverYields_"  <<  (firstyear + year) << ".txt";
								yield1Map->writeToFile(fnYields1.str());
								yield2Map->writeToFile(fnYields2.str());							
							}
						}
						mpmasInstance->agentsReceiveYields_Maps(*yield1Map, *yield2Map, ! configuration.isCleanSeason());

						break;	
						
					default:
						sprintf(fehlertxt, "Error: Unknown coupling type! (Processor %d)\n", my_rank);
						fehlerabbruch();		
				}
				
				
				
				printf("Processor %d: ...updating weather history (a)\n", my_rank);				
				translator.updateWeatherHistory(xpn->grid_xn_to_mpmas2, firstyear + year, curDate);
								
				//in last year stop here:
				if (year == numYears)
				{
					break;
				}
				
				
				nextStop = configuration.getfirstSowingDateNextSeason(firstyear + year);
				for (int i=0; i< grid_layers * xnGridSize; i++)
				{	xpn->grid_mpmas_to_xn[i].stopDate = nextStop;
				}
				
			
				cout << "\nProcessor "<< my_rank << ": Run XPN  until SOWING DECISION STOP: "  << nextStop.day << "-" << nextStop.month << "-" << nextStop.year << endl	;			
				xpn_main_run(xpn); //Run until SOWING DECISION STOP, before first sowing date of next season.
				printf("\nProcessor %d: SOWING DECISION STOP:\n", my_rank);
				curDate = nextStop; // copy before changed
				
				
				if(XNMPMASDBG) 
				{//post-xpn-run debugging output
					printf("...writing dbg_XnCommunicatedWeather_ file\n");	
					// Comment for-loop if you do not want to print air and soil temperatures
					stringstream fnDbgXnWeatherCom;
					fnDbgXnWeatherCom << "dbg_XnCommunicatedWeather_" << setw(2) << setfill('0') << year << "b.txt";
					FILE* dbgXnWeatherCom = fopen(fnDbgXnWeatherCom.str().c_str(), "w" );
					

					
					
//Begin of Hong:for gridId-loop 
		            for (int gridId=0; gridId<grid_layers; gridId++)     
			        {  
//End of Hong	
					  for (int i2=0;i2<matrix_size_x; i2++)
					  {	for (int i=0;i<matrix_size_y; i++)
						{/*
							printf("...current state of fields\n");							

							printf("Id: %d, i: %d, j: %d, cropCode: %s\n",gridId,i,i2,xpn->grid_mpmas_to_xn[gridId * xnGridSize +i2*matrix_size_y+i].CropCode);
							printf("Id: %d, i: %d, j: %d, Yield: %f\n",gridId,i,i2,xpn->grid_xn_to_mpmas[gridId * xnGridSize +i2*matrix_size_y+i].fruitDryWeight);
							printf("Id: %d, i: %d, j: %d, Stem + Leave: %f\n",gridId,i,i2,xpn->grid_xn_to_mpmas[gridId * xnGridSize +i2*matrix_size_y+i].stemLeafDryWeight);
							printf("Id: %d, i: %d, j: %d, Nmin0_30: %f\n",gridId,i,i2,xpn->grid_xn_to_mpmas[gridId * xnGridSize +i2*matrix_size_y+i].Nmin0_30);
							printf("Id: %d, i: %d, j: %d, Nmin30_60: %f\n",gridId,i,i2,xpn->grid_xn_to_mpmas[gridId * xnGridSize +i2*matrix_size_y+i].Nmin30_60);
							printf("Id: %d, i: %d, j: %d, Nmin60_90: %f\n",gridId,i,i2,xpn->grid_xn_to_mpmas[gridId * xnGridSize +i2*matrix_size_y+i].Nmin60_90);
							
							// Begin of Hong
							printf("Id: %d, i: %d, j: %d, check_gridToChange: %f\n",gridId,i,i2,xpn->grid_xn_to_mpmas[gridId * xnGridSize +i2*matrix_size_y+i].check_gridToChange);
							printf("Id: %d, i: %d, j: %d, check_currentGrid: %f\n",gridId,i,i2,xpn->grid_xn_to_mpmas[gridId * xnGridSize +i2*matrix_size_y+i].check_currentGrid);
							printf("Id: %d, i: %d, j: %d, check_changed: %f\n",gridId,i,i2,xpn->grid_xn_to_mpmas[gridId * xnGridSize +i2*matrix_size_y+i].check_changed);
							//End of Hong
							*/
							if (my_rank == 0)
							{ 	
								int startDoy = translator.getStartDoy(firstyear + year);
								for(int i3=0;i3<XNMPMASDAYSOFYEAR;i3++)
								{   int isYear = firstyear + year;
									if (i3+1 <  startDoy)
										isYear += 1; 
									fprintf(dbgXnWeatherCom, "g: %d i: %d, j: %d, SimulationDay: %d/%d, AirTemp: %f, SoilTemp: %f\n",gridId, i,i2,i3,isYear,xpn->grid_xn_to_mpmas2[gridId * xnGridSize +i2*matrix_size_y+i].airTemp[i3],xpn->grid_xn_to_mpmas2[gridId * xnGridSize +i2*matrix_size_y+i].topsoilTemp[i3]);	
								}
							}
						}
					  }
//Begin of Hong
					} //End of gridId-loop			  			  
// End of Hong!

					fclose(dbgXnWeatherCom);
				}
				
				printf("Processor %d: ...updating weather history (b)\n", my_rank);				
				translator.updateWeatherHistory(xpn->grid_xn_to_mpmas2, firstyear + year, curDate);
				
				//Output for debugging:
				if (my_rank == 0)
				{ 	
					printf("Processor %d: ...writing internalWeatherHistory_ file\n", my_rank);		
					stringstream fnHist;
					fnHist << configuration.getScenarioName() << "internalWeatherHistory_" << (firstyear + year) << ".txt"; 
					translator.writeWeatherHistoryToFile (fnHist.str(), firstyear + year);
				}
				translator.clearArrayRefs();
				
				
			}//end of loop over simulation years
			
			//deallocate arrays
			delete [] cropActIDX;
			cropActIDX = NULL;
			delete [] cropAreaX;
			cropAreaX = NULL;
			delete [] cropYieldX;
			cropYieldX = NULL;
			delete [] stoverYieldX;
			stoverYieldX = NULL;
			delete luaMap;
			luaMap = NULL;
			delete yield1Map;
			yield1Map = NULL;
			delete yield2Map;
			yield2Map = NULL;
			
			xpn = xpn_main_done(xpn);
			delete mpmasInstance;
			mpmasInstance = NULL;
			
#if defined(PAR2)
		}//END ARTIFICIAL ENCLOSING BLOCK TO MAKE SURE mpmas IS destroyed BEFORE MPI_Finalize() is called;
		finish = MPI_Wtime();
		time = finish - start;
		printf("MPI info: Simulation time %f seconds of process %d.\n", time, my_rank);
		MPI_Finalize();
#endif //PAR2
	}
	catch (exception& e)
	{
		xnmpmas::abort_simulation(e.what());
	}
	//all done
	return 0;
}



#ifndef ODB

void mpmasScope::exitfun()
{
	cout << "EXITING ..." << endl;
	
	//create segfault to get backtrace when debugging
	/*int* test = NULL;
	printf("%d", *test);
		*/
	//deallocateAll();
	delete mpmasPtr;
	mpmasPtr = NULL;
/*
	cout << "-------------------------------------------------------------------------" << endl 
	<< "SimName:  "		<< SIMNAME().c_str() << endl;
	cout << "INDIR():  " << INDIR().c_str() << " true: " << strcmp(INDIR().c_str(), ".") << endl;
	cout << "OUTDIR(): " << OUTDIR().c_str() << " true: " << strcmp(OUTDIR().c_str(), ".") << endl
			<< "-------------------------------------------------------------------------" << endl;
			* */
#if defined(PAR2)
	MPI_Abort(MPI_COMM_WORLD,1);
#endif

	exit(1);
}
#endif //ODB

