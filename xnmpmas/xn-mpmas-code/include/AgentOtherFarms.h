//------------------------------------------------------------------------------
//
//  Thomas Berger (main author), Pepijn Schreinemachers, and Thorsten Arnold
//
//
//  Hohenheim University (490d)            
//  Professorship for Land Use Economics in the Tropics and Subtropics
//
//  May not be distributed without permission by main author
//
//  File:       AgentOtherFarms.h
//
//  Contents:   Header of remaining farms class that is derived
//              from the basic class "agent"
//
//
//------------------------------------------------------------------------------


#ifndef _AgentOtherFarms_h
#define _AgentOtherFarms_h

#include "Agent.h"
#include "MatrixDouble.h"
#include "AgentParcelList.h"
#include "AgentHouseholdList.h"
#include "PopulationNetworkSegment.h"
#include "AgentParcelGridCellInterface.h"
#include "TimeHandlerClass.h"

class IrrigationMethodSector;

#ifdef PAR2
	#include "mpi.h"

	void initSdataO(int ncol, int nrow);

	struct ResultstructOMPI{
		double obj;						// Objective value
		double* colsol;					// Column solution
		double* LHS;					// Left hand side
		double* RHS;					// Right hand side
		int mp;							// Number of loaded problems
	};
#endif


class agentO : public agent
{	public:
	
//------------------------------------------------------------------------------------
	///@name Get and set functions
	//@{
	virtual int		getNumberOfPlots();
	virtual void	setNumberOfPlots(int);
	virtual int		getNumberOfIrrigablePlots();//special treatment for Ghana
	virtual int		get_erwart();
	virtual void	set_erwart(int);
	virtual int		getAgentMapId();
	virtual int		getMapColor();

	virtual int getFirstInflowID();

	virtual double get_b_wert();//Betriebswert
	virtual int		getSolvedLP();
	virtual double get_lastTgtPrd();
	virtual void set_lastTgtPrd(double);
	virtual double calculateLinearPartOfObjVal();

	virtual double get_liq();
	virtual void set_liq(double pmt);
	
	virtual int    checkNumberOfAllUsablePlots();
	virtual int		getNumberOfUsablePlotsInSoilType(int soilType);//renamed from <getNumPlotsInSoilType>
	virtual int    get_plotsPerType(int);//Parameter Typ
	virtual void   set_plotsPerType(int, int);//Parameter Typ, Anzahl
	//virtual void   make_bflaech(int c, int r, int typ, int fst, double dst, parcelGroup* pg = NULL);
	virtual void   make_bflaech(mpmasPlotCell* pp);
	virtual void   bflaechen_printOnScreen();
	//virtual void   bflaech_einfuegen(plot*);
	virtual void receivePlotAsNewOwnerAndUser(mpmasPlotCell* p, w_rechte* w, w_jahr* volN);
	virtual void receivePlotRentedOutReturned(mpmasPlotCell* p, w_rechte* w);

	virtual double getEtaOfAgent(){return etaOfAgent;}
	virtual double getTotalIrrigationWaterApplied() {return totalIrrigationWaterApplied;};
	
//------------------------------------------------------------------------------------

	/**
	* Functions called at beginning of period
	*/	

	///Functions for building expectations
	virtual void makePriceExpectation();  //DONE_twoTimeHorizons
	virtual void makeWaterSupplyExpectationYearly();
	virtual void makeWaterSupplyExpectationInMonth(int m);
	virtual void makeWaterDemandExpectationYearly();
	virtual void makeWaterDemandExpectationInMonth(int m);
	virtual void makeYieldExpectationsForCropWat();
	virtual void initializeYieldExpectationsForCropWat();

	//virtual void estimateEffectivePrecipitationForFullYear(TimeHandler& th_notChanged,    MatrixDouble& vectorPrecipitation,    MatrixDouble& agentsEffectivePrecipitation );// ## Tbe 111201 not in use
	//virtual void copyEffectivePrecipitationInMonth(int m, const MatrixDouble& vectorOfMonthlyCropEffectivePrecipitation, MatrixDouble& agentsEffectivePrecipitation);

	virtual void makeYieldExpectationsForTspc();//renamed from <makeYieldExpect>
	virtual void initializeYieldExpectationsForTspc();//renamed from <initYieldExpect>
	virtual void makeYieldExpectationsForLucia();
	virtual void initializeYieldExpectationsForLucia();
	virtual void initPlotsInNRUs();
// Troost 20140527 Generalize dynamic NRUs
// Troost **********begin block insert**********
	virtual void makeCellsInSoilTypes();
// Troost **********end block insert**********
	
//------------------------------------------------------------------------------------

	/**
	* Decision-making functions using MP
	*/	
#ifdef PAR2
	virtual void synchronizeDecisionResultsMPI(milpModeTypes mode);
#endif
	///Functions for production
	virtual double producPlan();
	
	///Functions for solving MP problems
	virtual int getNNrowsInternal();
	virtual int getNNcolsInternal();
	virtual int getNumColsInternal();
	virtual int getNumRowsInternal();
	virtual double getActualTotalGrossMargin();

	virtual double getLpSolution(int lpCol);
	virtual void setVarRHSInternal(int lpRow, double value);
	virtual double getVarRHSInternal(int lpRow);

	virtual double get_kapazitaeten(int);
	virtual void set_kapazitaeten(int, double);

	virtual void updateAgentZRow();//formerly 'kapaz9()'
	virtual void updateStartPeriodRHS();//formerly 'kapaz9()'
	virtual void updateEndOfPeriodRHS();//formerly 'kapaz9()'
	virtual void checkForNegativeValuesInRHS();

	virtual int copyLpDataInProductionMode(int isIntegerProblem);
	
	virtual void copyExpectedWaterDemandsIntoMILP();
	virtual void copyExpectedYieldsOfAnnualCropsIntoMILP();//refactoring of <copyYldExpMILP>

	virtual void modifyProductionSolution(int num, int*& cols, double*& values);
	virtual void setProductionSolutionColumn(double va, int colLP);
	
#ifdef LP9
	virtual double solveLinPrbOSL(int savePrim, int saveDual);
	virtual double solveIntPrbOSL(int savePrim, int saveDual, int& forSol);
#endif //LP9
	virtual void makeNewPrb();
	virtual void freePrb();

//------------------------------------------------------------------------------------

	/**
	* Functions related to TSPC crop growth module
	*/ 

	///Functions for crop yields
	virtual void computeCropTspcYields(FILE* beforeYieldsStrm, FILE* afterYieldsStrm);




//------------------------------------------------------------------------------------

	/**
	* Functions related to crop-water and irrigation module
	*/ 

	///Functions for irrigation farming
	virtual double computeAverageOnFieldIrrigationEfficiency(IrrigationMethodSector* sectorPointerToIrrigationMethods);
	virtual double irrigateCropsByPriorityGroups(int month);
	virtual void computeCropWaterYields(FILE* beforeYieldsStrm, FILE* afterYieldsStrm);
	virtual void setOutputOfAllCropsToZero();
	virtual void applyYieldReductionFactorsAndUpdateLpSolution();

	///Functions for water rights
	virtual double get_N_i(int);
	virtual double get_WPa_i(int);
	virtual void set_WPa_i(int, double);//Parameters Month, Volumn
	virtual void waterrights_eintragen(int, double);//Zufluss-Nr. und Anteil
	virtual void hole_durchs_waterrights(w_rechte*);//... je Parzelle 
	virtual void waterrights_austragen(w_rechte*);//traegt WaterRights aus
	
	///Functions related to irrigation
	virtual void createDataForCropWaterModule();
	virtual double aggregateIrrigationWaterDemandsByIrrigationMethod(int month);
	virtual double receiveNetInflows(int month, int = true);//Flag (save sector values yes/no)
	virtual double computeIrrigWaterSupplyAndPlantWaterDemandSatisfied(int month, double Ueb_j_tmp);


//------------------------------------------------------------------------------------

	/**
	* Functions related to external crop growth module (Lucia, Expert-N or simple look-up table in file)
	*/ 

	///Functions for crop yields
	virtual void computeCropLuciaYields(FILE* beforeYieldsStrm, FILE* afterYieldsStrm);

//------------------------------------------------------------------------------------

	/**
	* Functions handling farm surveys
	*/	

	///Collect farm production of tradables
	virtual int replyToFarmProductionSurvey(MatrixDouble& questionnaire, int& firstLpColumn);


//------------------------------------------------------------------------------------


	/**
	* Functions called at end of period
	*/
	
	///Function for crops yields	
	virtual void makePlantYields();//formerly 'ertraege_berechnen()'

	
//------------------------------------------------------------------------------------

	/**
	 * Functions that save or write outputs 
	 */ 

	///Write land and and water-related data
	//virtual void writeAgtLandData(FILE* stream0);
	virtual void writeAgtWaterData(int m);

	///Write expected and actual values over all periods (file in append mode)
	virtual void writeExpectedMarketPriceData(FILE* strm);
	virtual void writeExpectedWaterSupplyData(FILE* strm);
	virtual void writeExpectedCropWaterData(FILE* strm);
	virtual void writeExpectedCropYieldData(FILE* strm);

	///Write agent solution from production problem to file
	virtual void writeAgentProductionSolutionToFile(char* filename);

	//Function writes the first lines of the agent - which is needed for every output file to identify agents
	///Production and LP capacity data
	virtual void writeAgtIdEntry(FILE* stream);
	//virtual void writePfmPrdData(FILE* stream0, FILE* stream1);
	virtual void writeCapRHSData(FILE* stream);
	
	//virtual void printOnScreenHH();
	virtual void printOnScreenWaterRights();
	virtual void bnummer_anpassen(int);

	///Print all crops from CrpMixClass
	virtual void printOnScreen_crpMixClass();
	
	///Print only one activity 'cnt' from CrpMixClass
	virtual void printOnScreen_crpMixClass(int cnt);
	virtual void printOnScreen_crpMixWater();
	virtual void printOnScreen_crpMixWater(int cnt);

	virtual void printOnScreenProduction();
	virtual void writeToFileProduction(ofstream& out);
	virtual void printOnScreen_AllInfo(int soilC, int cnt);
	virtual void printOnScreen_bflaech(void);
	
	virtual void printOnScreenWater();
	virtual void printOnScreenCropMatrix();

	virtual void writeAllCropWaterDataToFile();
	virtual void writePercentageByIpg(FILE* out);
	virtual void appendCropWaterYieldsToFile();
	virtual void appendWaterUseAndEconomicReturnsToFile();

	virtual void writeAllPlotsToFile(ofstream& out);
	virtual void writeWaterInfoToFile(ofstream& out);

//------------------------------------------------------------------------------------

	/**
	 * Functions needed for spatial allocation and TDT coupling
	 */

	///Crop mix implementation and spatial allocation of crops
	virtual void createCropMixFromSolutionVector();// renamed from <createDataForSpatialAllocationOfCrops>
	virtual void allocateCropActivitiesSpatially(void);
	
	virtual void fixRoundingErrorsInProductionSolution(int soil, int cropIdx, double ha);
	virtual void addFallowToProductionSolution(int soil, int cropIdx, double ha);
	virtual void fixEmptyPlotErrors();
	virtual void addFallowToEmptyPlots(int soil, int cropIdx, int actID);

	//get number of crop mixes and crop activities, according to crop growth model
	virtual int getNumberOfCropMixes();
	virtual int getNumberOfCropActivities();
	virtual int getCropMixIndexForSoilType(int soilType);
	virtual void printAgentCropMixToFile(char* filename);

	//get info for zero labor activity (=fallow), according to soil type or NRU
	virtual int getLpColumnOfZeroLaborActivity(int soil);
	virtual int getActivityIdOfZeroLaborActivity(int soil);
	virtual int getPriorityOfZeroLaborActivity(int soil);

	// For coupling and spatially active runs
	/* TODO 2 -oArnold -cImplement : Using content type (here a switch must still be implemented) information is retrieved and then assigned to the content type, into a temporary variable */
	virtual void assignToCropMixClassTemp_ByContentType(Content cont, int m);
	
	// Note: make sure that even if no irrigation exists, crop mix class is somehow initialized with content
	virtual void touchCropMixClassTemp_WithEmpty();
		
	// Switch only, externalized for simplicity
	virtual double getValueForCropMix_ByTempType_SWITCH(Content cont, int crA, int g, int m, CropWater* cropWat);

	// ... does both of the above
	virtual void copy_CropMixClass2LandscapeFromTemp_monthly(Content cont, int m);//Tbe 090303 fixed bug
	virtual void copy_Landscape2CropMixClassTemp_monthly(Content cont, int m);//Tbe 090303 fixed bug
	virtual void copy_Landscape2CropMixClassTemp(Content cont);//Tbe 090303

	// Interpret data in crop mix class
	virtual void interpretTempInCropMixClass(Content cont, int m);//## substitute
	virtual void interpretDataRetrieved(Content cont, MatrixDouble& vector_data);//## substitute
	virtual void processReceivedDataInCropMixes(Content cont);//Tbe 090303

	//virtual void copyContent_Landscape2Parcels(Content cont);// ## Tbe 110920 not in use
	//virtual void copyContent_Landscape2Parcels(Content cont, bool flag_fixCopyToCell);// ## Tbe 110920 not in use
	//virtual void copyContent_Landscape2Parcels_monthly(Content cont, int m);// ## eliminate
	virtual void copyContent_PlotToLandscape(Content cont);
	//virtual void copyContent_ParcelToLandscape_monthly(Content cont, int month);// ## Tbe 110920 not in use
	
	// ... for testing
	virtual double getMeanOfAllPlots(Content cont);

	//virtual void resetAllParcels_toNotUsed(void); //## Tbe 100616
	virtual void resetAllPlotsAndCropMixes();//cleans parcel list and crop mixes after use

	// check status of parcels
	virtual void checkIfTrue(bool isTrue);//checks status of parcels in crop mixes 
	virtual void checkIfAllPlotsHaveCrops();//checks status of parcels in landscape
	
	virtual void addLanduseToColumnOfMatrix(MatrixDouble& matrixOfCatchmentLandUses, int uaID);
	virtual void checkActivityIDconsistent(int a, string functionName);
	virtual double getActivityArea(int a);
	

//------------------------------------------------------------------------------------   	
	///@name Constructor, destructor etc.
	//@{		
	
	///Constructor 
	agentO(int agid, int scid, int skid, int mapId,/* int numP,*/ int erw);

	///Destructor
	virtual ~agentO();
	virtual void deallocYieldExp();	
	//@}

//------------------------------------------------------------------------------------   	
	protected:
	///@name Protected member data
	//@{
	int erwart;	   //type of expectation formation

//------------------------------------------------------------------------------------   

	/**
	 * Data structures for water and land resources
	*/
	///ID for identification of plots in map ("scuencaId" + "agentType" + "sectorId")
	int agentMapId;

	///map color
	int mapColor;
	
	///Total number of parcels
	int fl_anzahl;
	
	///Number of parcels per soil type
	int* plotsPerType;
	
	///List of farm parcels
	p_liste bflaech;     
	
	///Water rights
	MatrixDouble wr;
	// DIMS: (numInflows X 2)
	//  col 0: zuf    ID for inflow
	//  col 1: anz    quantity

	int agentMipCount;
//------------------------------------------------------------------------------------

	double totalIrrigatedArea;	//monthly irrigated area, formerly betr_Wert

//------------------------------------------------------------------------------------   

	/**
	 * Data structures for crop-water module
	*/
	///Data structures for CropWat module
	MatrixDouble cropWaterData;// using <enumYieldInfo>

	///Water deficits for selected water activities
	MatrixDouble waterDeficitsOfCropsAsMonthlyKR;//Tbe 080220 selected crops, irrigated and rainfed

	///Monthly water demands[absolute value]
	MatrixDouble irrigationWaterDemandsPerPriorityGroup;//Tbe 080220

	///Percentage of irrigation water demand that was served
	MatrixDouble percentageIrrigatedPerPriorityGroup; //Tbe 080220

	// Actual irrigation water
	MatrixDouble irrigationWaterSuppliedToCrops;//Tbe 080220

	double etaOfAgent;
	double totalIrrigationWaterApplied; // total irrigation water in current month

	// Making code spatially explicit: LP output data structure (crop mix)
	// Dimensions: TYPEN or NUMBER_NRU
	crpMixClass* cropMixAgent;

//------------------------------------------------------------------------------------

	/**
	 * Data structures for irrigation module (monthly values)
	*/
	///irrigation water demand
	double* D_i;            
	///irrigation water demand actually met
	double* D_sat;          
	// expected net  water supply
	double* erw_N_i;        

	///actual net water supply
	double* N_i;            
	///expected water supply from return flows (additional to net)
	double* erw_Ext_i;      

	///actual water supply from return flows (additional to net)
	double* Ext_i;
	///irrigation water from rental contracts
	double* WPa_i;
	/// Maximum "initial" water supply, including share that this agent might not fully use
	double* S_i;//renamed from <Initial_i> 
	///Agent share of "left-over water"
	double* L_i;
	
	///yield expectations
	MatrixDouble expCropWaterYields;

	///actual yields
	MatrixDouble actCropWaterYields;

	///expected irrigation water demands
	MatrixDouble expCropWaterIrrDmds;

//------------------------------------------------------------------------------------

	/**
	 * Data structures for TSPC and Lucia crop growth modules
	*/
	///expected yields per NRU
	caYld** expYlds;        
	///actual yields per NRU
	caYld** actYlds;

//------------------------------------------------------------------------------------

	/**
	 * Data structures for LP module
	*/
	double* erw_preise;			//expected prices, used to initialize objective fucntion coefficients
	double* varsRHS;				//the agent's resources (Right Hand Side)
	double* varsLHS;				//used capacities of resources (Left Hand Side)
	double* schattenpreise;		//shadow prices


	int solvedLP;					//forced solution (default 0, inv 1, prd 2, con 3)	
	double lastTgtPrd;
	
	///Information for "forced solution" (case that LP does not solve)
	MatrixDouble mEntries;		//matrix with information for "forced" solution

	l_vektor bprod;//Loesungsvektor (Umfaenge, Zielfunktionskoeff., Zielwert)

	///Regional market access
	int marketID;

	///number of matrix elements to be changed in third LP (might be needed by YIELD_MDL)
	int numChangedElems;   
	///array containing LP row indices
	int* idxRow;   
	///array containing LP column indices
	int* idxCol;   
	///array containing LP MatrixDouble elements
	double* matEls;

	double liqMeans;

	//}@
//------------------------------------------------------------------------------------
};

#endif

