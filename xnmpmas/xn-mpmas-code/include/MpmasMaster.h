//------------------------------------------------------------------------------
//
//  Thomas Berger (main author), Pepijn Schreinemachers, Thorsten Arnold,
//  Prakit Siripalangkanont, and Christian Troost
//
//
//  Hohenheim University (490d)
//  Professorship for Land Use Economics in the Tropics and Subtropics
//
//  May not be distributed without permission by main author
//
//  File:       MpmasMaster.h
//
//  Contents:   Header file for Mpmas implementation
//
//
//
//------------------------------------------------------------------------------


#ifndef _MpmasMaster_h
#define _MpmasMaster_h

#include "BasicDefines.h"
#ifdef ODB
	#include "MpmasControlSettings.h"
	#include "MpmasUtilityFunctions.h"

#else
	#include "BasicData.h"
#endif
	#include "OutputDataHandler.h"
	#include "RegionHandler.h"
	#include "TimeHandlerClass.h"
#ifdef LIVSIM_COUPLING
	#include "MpmasLuciaLivSimCoupling.h"
#endif

#if defined(PAR2)
	#include "mpi.h"
#endif

#ifdef ODBSTORE
	using namespace odb::core;
#endif
#ifdef ODB
	namespace mpmasScope {
		void exitfun();
	}
#endif



class mpmas
{
#ifdef LIVSIM_COUPLING
		LivSimHerdTable herds_table;
#endif

	public:
#ifndef MCHECK
	///@name Get and set functions
	//@{
	int getNumberOfPeriodsToSimulate();
	int getNumberOfSpinUpRounds();
	string getInputDirectory() ;
	string getOutputDirectory();
	string getScenarioName();
	int getNumberOfCroppingActivities ();
	int getNumberCommunicatedYieldMapsPerPeriod();

	region* getPointerToRegion();

#ifdef IP_COUPLING
	string getHostForCoupling() 	{ return settings.getHostForCoupling();}
	int getPortForCoupling() 		{ return settings.getPortForCoupling(); }
#endif
#ifdef LIVSIM_COUPLING
	LivSimHerdManagementTable getHerdManagementTable();
#endif
	//}@

	///@name running the simulation
	//@{
#ifndef MULTIPERIOD
	///Run complete simulation with internal crop growth modules
	virtual int simulateOnePeriodComplete(int period);
	
	///Export land uses and import yields for coupling with external crop growth models
	virtual int simulateOnePeriodExportingLandUse(int period, int numCropActs = 0, int* cropActID = NULL, double* cropArea = NULL);
	virtual int simulateOnePeriodImportingYields(int period, const int numCropActs = 0, const int* cropActID = NULL, const double* cropYield = NULL, const double* stoverYield = NULL);
#endif // ndef MULTIPERIOD
#ifdef MULTIPERIOD
	virtual int agentsUpdateFarmAssetsAndPlanningAtPeriodSwitch(int simPeriod);
	void agentsImplementLandUse_LookUpTable(int numCropActs = 0, int* cropActID = NULL, double* cropArea = NULL);
	void agentsImplementLandUse_Maps(Raster2D& outputRaster);
	void agentsImplementLandUse_ExtYieldsNoMaps();
	void agentsReceiveYields_LookUpTable(double* cropYield, double* stoverYield, double** extrAttrs);
	void agentsReceiveYields_Maps(Raster2D& yield1map, Raster2D& yield2map,vector<Raster2D>& extraMaps, bool harvestAfterNewLandUseDecision);
	void agentsImportExtYieldsNoMaps();
	int getNumberExtraCropActAttributes();
#ifdef LIVSIM_COUPLING
	void agentsImplementHerdManagement(LivSimHerdTable*& herds_tablePtr, LivSimGrazingTable& grazing_table, LivSimFeedingTable& feeding_table);
	void importHerdChangesFromStream(istream & herdChangeFile);
	void agentsUpdateAssetsFromUpdatedHerd();

#endif
#endif

	void updateUserdefinedAttributesInMaps(Raster2D& grasslandStatusMap, int udefLayerID, int updateNRUs);
	//generic interface
	map<int,vector<investdef> > agentsExportAssets();
	/*void externalUpdateAgentAssets(map<int,vector<investdef> > );*/
	void externalUpdateAgentAssets_fromFile( string fileName , bool updateRhs = false);
	void printExportFile_AgentAssets(string fn);
	string printExportFile_SelectedSolutionPreInvest(string fn, map<int,vector<double> > outputMap);

	void exportMapsAnnualAsRequested();

	string get_externalScriptCall_PreInvest();
	string get_externalScriptCall_CropModel();
	string get_externalScriptCall_InYear();
	void do_externalScriptCall_InYearIfRequested(int period);

	virtual void runThroughModelMonths();
#ifdef MONICA	
	///disable agents in specific sectors after initialization
	virtual int disableAgentsInSectors(int catchmentID, int numDisabledSectors, const int* sectorID = NULL);

	///disable crop activities
	virtual int disableCropActivities(int numDisabled, const int* cropActID = NULL);

	///(de-)allocate memory in Mpmas for Monica coupling
	virtual void allocateMemoryForMonica(int numCropActs);
	virtual void deallocateMemoryForMonica();
	
	///compute aggregate agent incomes and gross margins
	virtual int simulateOnePeriodAgentIncomes(int period, double& farmIncome, const int numCropActs = 0, const int* cropActID = NULL, double* grossMarginsCropActs = NULL);
	
	///get perfomance data for selected agents
	virtual int getPerformanceDataForSelectedAgents(int numAgents, int* agentID, double* farmIncome, const int numCropActs = 0, const int* cropActID = NULL, double** grossMarginsCropActs = NULL);
#endif
	//}@


	void welcomeMessage();

	///@name Constructor, destructor etc.

	mpmas(int ac, char **av);
	virtual ~mpmas();
	//}@

#endif //end ifndef MCHECK

	// from here on functions visible for MilpCheck

	///@name MPI related wrapper functions
	//@{
	bool isReportingProcess()
	{
		bool ret = true;
	#if defined(PAR2)
		int my_rank;
		MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
		if (0 != my_rank)
			ret = false;
	#endif
		return ret;
	}

	int myProcessRank()
	{
		int myRank = 0;
	#if defined(PAR2)
		MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	#endif
		return myRank;
	}
	int numProcesses()
	{
		int mySize = 1;
	#if defined(PAR2)
		MPI_Comm_size(MPI_COMM_WORLD, &mySize);
	#endif
		return mySize;
	}
	//}@

#ifdef MCHECK
	mpmas()	{};	//dummy constructor for MilpCheck; // Dbg -- dummy
#endif
	protected:
	///@name Protected member data
	//@{
#ifndef MCHECK

#ifdef ODB
	mpmasControlSettings settings; //Note: should be first, so it is destroyed last (destruction in reverse order of construction) and global functions are still available in destructors of other members
	void initializeControl(int ac, char** av);
#endif //ODB
	/// region with raster
	///Outputs
	ausgabe outfiles;

	int Test;
	region watershed;
	Raster2D mCommand;

	/// read and update basic data, print scenario setting on screen
#ifdef ODB
	void initializeWithBasicData();
	void initializeHydrologicalModel();
	void initializeExternalModel();
#else
	virtual void initializeWithBasicData(int ac, char **av);
#endif
	virtual void updating_basicData();
	virtual void initTDTCoupling();

	/// simulate through one period
	virtual void doPeriodSwitch(int& periode, TimeHandler& timeHandle);
	void screenOutput_InitializePeriod(int numYears);
	void screenOutput_EndOfPeriod( int periode);


	virtual void updateInnovationDiffusion();
#ifndef MULTIPERIOD
	virtual void prepareLandUseForCropYieldCalculation();

	virtual void determineMarketingOutcomes();
#endif


	/// write final data to files
	virtual void completeSimulation();

	/// deallocate memory
	//virtual void deallocateMPMAS(); ## Tbe 130103 not needed
	virtual void deallocateAll();

	//}@
#endif //ifndef MCHECK

#ifdef ODBSTORE
	public:
		void storeSnapshot(string storeID);
		void restoreSnapshot(string filename);
#endif//ODBSTORE

};

#endif
