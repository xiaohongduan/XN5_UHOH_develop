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
//  File:       RegionHandler.h (formerly region.h)
//
//  Contents:   Handles all input files needed to initialize the cellular and
//              agent-based component
//
//
//------------------------------------------------------------------------------


#ifndef _RegionHandler_h
#define _RegionHandler_h


#include <iostream>
#include <sstream>

// Troost 20140527 Generalize dynamic NRUs
// Troost **********begin block insert**********
#include <map>
// Troost **********end block insert**********

#include "CatchmentSectorSpecific.h"
#include "AgentPopulationHandler.h"
#include "PopulationNetworkSegmentList.h"
#include "MarketTradablesHandler.h"
#include "PopulationClusterList.h"

class ausgabe;
struct eumtr2003;

// Auxiliary class, used to initialize sectors from region input file
class classSectorReadInfoLine
{  public:
	string sectorName;
	bool active;
	bool hasSubsector;
	int CatchmentID;
	int positionInRegionFile; // is set to number of file that was read
	int strahlerOrder; // currently not read from file
	int routingOrder; // not read, as curently corresponds to order in file
	int GISID;
	int sectorSeed;
	//int NumberOfParcels;// ## Tbe 110608 
	bool hasHydroData;

	// EDIC matters
	double ReUseCoefficient;
	double WaterSystemEfficiency;
	double canalEfficiency;
	double mergeB;
	double mergeG;
	MatrixDouble canalCapacity;
	MatrixDouble waterRightsByInflow;

	int numLotteryValues;

	classSectorReadInfoLine();
	virtual ~classSectorReadInfoLine()
	{	deallocateAll();
	};


	//virtual bool readLine(ifstream& in, int HydroModel, int numInflows, int valuesPerInflow);

	virtual bool readLine(FILE* in, int irrigation, int numInflows, int valuesPerInflow);

	virtual void printToScreen();

	virtual void writeToFile_Line(ofstream& outStream);
	virtual void deallocateAll();

};

// Troost 20160725 Distance consideration in NRUs
// Troost **********begin block insert**********
struct nruDistanceSizeDef
{
		double upperDistanceBound;
		int upperSizeBound;
		int nru;
		nruDistanceSizeDef() : upperDistanceBound(0.0), upperSizeBound(-1), nru(-1){};
		nruDistanceSizeDef(double dist_, int s_, int nru_) : upperDistanceBound(dist_), upperSizeBound(s_), nru(nru_){};

};

int nruDistanceSizeDefLess(const nruDistanceSizeDef& first, const nruDistanceSizeDef& second);
// Troost **********end block insert**********


struct nruDistanceDirectionUDef
{
		double upperDistanceBound;

		int upperDirectionBound;
		vector<double> udefBounds;

		int nru;
		nruDistanceDirectionUDef() : upperDistanceBound(0.0),  upperDirectionBound(360),udefBounds(0), nru(-1){};
		nruDistanceDirectionUDef(double dist_,int dirup_, vector<double> udefs_, int nru_) :
					upperDistanceBound(dist_), upperDirectionBound(dirup_),udefBounds(udefs_), nru(nru_){};
};

int nruDistanceDirectionUDefLess(const nruDistanceDirectionUDef& first, const nruDistanceDirectionUDef& second);


class region
{
	public:
	
	///@name Constructor, destructor and related functions
	//@{	
	region();

	virtual ~region()
	{  deallocateRegion();
		deallocOtherObjects();
		if (investmentsStrm != NULL)
			fclose(investmentsStrm);
 	}

	virtual void initGlobalPointers();
	virtual void deallocateRegion();
	virtual void deallocOtherObjects();

	//@}

//------------------------------------------------------------------------------------

	/**
	* Functions for creating landscape and agents
	*/	
	
	/// Initialization
	virtual void allocateCatchmentsAndSectors(void);
	virtual void initializeRegionFromFiles(TimeHandler& timeHandle_const);
	virtual void initializeRegionStandalone();

	/// Reads inflows before first period, used by agents
	virtual void initializeCatchmentEdicAndInflows(void);
	virtual void checkConsistencyRegion();
	//virtual void analyseAllSectorsAndExportAgentInfo(string fn); // Tbe 110920 not in use
	virtual void readDatFiles();
	virtual void readLandscapeFromFiles();
	virtual void readAllCatchmentsFromRasterFiles();
	
	/// Function to read region input file and to initialize sectors
	virtual bool readRegionDatFileAndInitSectors(string filename);
	
// Troost 20140527 Generalize dynamic NRUs
// Troost **********begin block insert**********
	// Function to read info for user-defined assignment of soils to NRUs
	virtual void readDatFileSoilToNRU(string filename);
// Troost **********end block insert**********

	/// Function to initialize sector and user association
	virtual void initializeSectorFromAuxiliaryClass(classSectorReadInfoLine& lineRead);
	virtual void initializeUserAssociation(int agentID_, int catchmentID_, int sectorID_, int numLotteryValues);
	
	/// Function to initialize producer organization
	virtual void initializeProducerOrganization(int agentID, int catchmentID);

	/// Creation of agents
	virtual void createAgents();
	virtual void assignResourcesToAgents();
	virtual int numInfrastructureProjects();
	virtual void computeStartValuesForAgentWaterExpectations(TimeHandler& timeHandle_const);// ## H2O

	//------------------------------------------------------------------------------------

	/**
	* Functions handling agent planning and decisionmaking
	*/	
	
	/// Agent planning at start and end of period
	virtual int agentsDoExpectationFormation(TimeHandler& th_const);//refactored from <agentsDoExpectationAndPlanning>
	virtual void agentsDoInvestmentAndProductionPlanning(TimeHandler& th_const);//refactored from <agentsDoExpectationAndPlanning>
#ifdef MULTIPERIOD
	void agentsDoStartOfPeriodDecision(doInvestModes doProduce, string decisionStageName);
	string multiperiod_outputAgentDecisionResults(string decisionStage, bool aggregateOnly = false, bool noAggYears = false);
	void agentsCalculateIncomes();
	void updateSoilTypes();
#endif
	virtual void agentsExitingDecision(int periode);

	/// Edic routing model
	virtual void wasserPeriodeRegion(TimeHandler& timeHandle_const);
	virtual void writeSectoralWaterInfoToFile(TimeHandler& timeHandle_const, string fn_append);
	virtual void resetEdicAnnualData(TimeHandler& timeHandle_const);

	/// Spatial functions, copying between "cell" and "parcel"
	virtual void joinRegionMap(Content cont, Raster2D& rasterRegion, int userContentId = -1);
	//virtual void joinRegionMap(Content cont, Raster2D& rasterRegion, int m); replaced by default value below
	virtual void joinRegionMap(Content cont, Raster2D& rasterRegion, int m, bool flag_setFilename, bool flag_forceCell, int userContentId );

	//virtual void translateFromParcelCallToRaster(Content cont, Raster2D& test);// ## Tbe 110920 not in use

	/// Export maps
	virtual void exportMapsMonthly(TimeHandler& timeHandle_const);
	virtual void exportCropMapsAnnual(TimeHandler& timeHandle_const);
	virtual void exportMapsAnnualAsRequested(TimeHandler& timeHandle_const);
	virtual void exportMapsGeneric(bool flagIsAnnual, TimeHandler& timeHandle_const);

	/// Export aggregated land-use
	void exportAggregatedLandUse(FILE* openStream = NULL, int numCropActs = 0, int* cropActID = NULL, double* cropArea = NULL);
	vector<string> exportUserDefinedMaps();
#ifdef MULTIPERIOD
	void multiperiod_exportLandUseSpatially( Raster2D& outputRaster);
	void multiperiod_exportAggregatedLandUse(FILE* openStream, int numCropActs, int* cropActID, double* cropArea);
	void multiperiod_recordLandUseForExtYieldsNoMaps();
	void multiperiod_importYieldsAggregatedLandUse( double* cropYield, double* stoverYield, double** extraAttrs);
	void multiperiod_importYieldMaps(Raster2D& rasterCropYields, Raster2D& rasterStoverYields, vector<Raster2D>& extraMaps, bool harvestAfterNewLandUseDecision);
	void multiperiod_agentsImportExtYieldsNoMaps();
	int getNumberExtraCropActAttributes();
#ifdef LIVSIM_COUPLING
	void multiperiod_agentsImplementHerdManagement(LivSimHerdTable& herds_table, LivSimGrazingTable& grazing_table, LivSimFeedingTable& feeding_table);
	void multiperiod_agentsUpdateAssetsFromUpdatedHerd();

#endif
#endif//MULTIPERIOD
	void updateUserdefinedAttributesInMaps(Raster2D& grasslandStatusMap, int udefLayerID, int updateNRUs);


	/// Import yield maps from external model
	virtual void importYieldMapsFromExternalModel();//Tbe 090303

	/// Import crop yields directly from external look-up table
	virtual void importCropYieldsFromExternalModel(FILE* openStream = NULL, const int numCropActs = 0, const int* cropActID = NULL, const double* cropYield = NULL, const double* stoverYield = NULL);//Tbe 130305

	/// Export aggregate and indivudal farm income and crop gross margins
	virtual void exportAggregatedIncomeAndGrossMargins(double& farmIncome, const int numCropActs = 0, const int* cropActID = NULL, double* grossMarginsCropActs = NULL, FILE* openStream = NULL);

#ifdef MONICA
	virtual void exportSelectedIncomeAndGrossMargins(int numAgents, int* agentID, double* farmIncome, const int numCropActs = 0, const int* cropActID = NULL, double** grossMarginsCropActs = NULL, FILE* openStream = NULL);

#endif

	/// Land-use allocation functions
	virtual void agentsImplementLandusePlans();// renamed from <agentsAllocateSpatiallyLanduse>
	virtual void agentsIrrigationWater2Landscape(int m);

	/// Generic functions between "Landscape" and "CropMixClass"
	virtual void agentsExportContentTypeToLandscape(Content cont, int m);
	virtual void agentsImportLandscapeToCropMixClass(Content cont, int m);

	// ... Use data in <CropMixClass>
	virtual void agentsInterpretTempInCropMixClass(Content cont, int m);

	///Special functions for dynamic inflows 
	// Copy inflows for current month to catchments
	virtual void updateDynamicInflows_preknown(int catchID, TimeHandler& timeHandle_const);

//------------------------------------------------------------------------------------

	/**
	* Special functions for handling of inconsistent input maps
	*/	
	
	/// Delete all lands without agents and agents that own area below factor_HRB * cutoffsize
	//virtual void checkConsistencyOfMaps(double factor_HRB, double cutoffsize); ## Tbe 110422 eliminate?
	//virtual void checkMapAndExportConsistentMaps(double factor, double cutoffsize); ## Tbe 110422 eliminate?
	//virtual void analyseAllSectorsByContentAndExportHistogram(Content cont);## Tbe 110422 eliminate?

	/// Special function for Ghana model
	//virtual void rectifySectorBoundariesFromInflows(); ## Tbe 110422 eliminate?

#ifdef MONICA
	virtual void allocateMemoryForMonica(int numCropActs);
	virtual void deallocateMemoryForMonica();

	virtual void makeLandUseBackup(int numCropActs, int* cropActID, double* cropArea);
	virtual void makeCropYieldBackup(const int numCropActs, const int* cropActID, const double* cropYield, const double* stoverYield);

	virtual int disableCropActivities(int numDisabled, const int* cropActID = NULL);
	virtual int disableAgentsInSectors(int catchmentID, int numDisabledSectors, const int* sectorID = NULL);
#endif

	///@name Print and write functions
	//@{	
	virtual void printListOfAllAgents();
	virtual void print_AllSubcatchs();
	virtual void printAllAgentsProductions();

	virtual void writeLandMarketsToFile(ausgabe& outfiles);
	virtual void outputLanduseByUserAssociation(int type);
	virtual void outputAgentsWritePercentageByIpg();

// Troost 20130930 Additional output land market
// Troost **********begin block insert**********
	//open UserAssociation Streams
	virtual void openStreamsRentalMarketTransactions();
	virtual void closeStreamsRentalMarketTransactions();
// Troost **********end block insert**********

	/// small function for writing header in file
	virtual void writeToFile_RegionTableFrontMatter(ofstream& out, int NumSectors, int irrigation, int numInflows, int valuesPerInflow);
	virtual void writeMapsOnScreen();
	virtual void writeSubcatchmentMatrixDimsOnScreen();   
	//@}

// Troost 20140527 Generalize dynamic NRUs
// Troost **********begin block insert**********


	virtual set<int> getNRUsInCompatibilityGroup(int compGroup);
	virtual int getCompatibilityGroupForSoil(int soilType);

	virtual multimap<int,int>::iterator getItBeginCompatibilityGroupToNRU();
	virtual multimap<int,int>::iterator getItEndCompatibilityGroupToNRU();
	virtual multimap<int,int>::iterator getItLboundCompatibilityGroupToNRU(int compGroup);
	virtual multimap<int,int>::iterator getItUboundCompatibilityGroupToNRU(int compGroup);
	virtual size_t getNumNRUsinGroup(int compGroup);



	virtual int getNRUForSoilDistanceSize(int soilType, double distance, int size);
	virtual int getNRUForSoilDistanceDirectionUdef(int soilType, double distance, int direction, vector<double> udefs);
	virtual int getNumSoilNRUGroupLevels();
	virtual int getSoilSimilarityGroupForSoilAtLevel(int soil, int level);
// Troost **********end block insert**********

	virtual int getNumberOfCroppingActivities();
	int getNumberCommunicatedYieldMapsPerPeriod();
	virtual eumtr2003* getEuCap2003Info();

	int* getArraySoilTypeForNRU();

	void closeStreamInvestments();
	void recordInvestment(int farmsteadId, int period, int objectID, double quantity, int age, string remark);


	map<int,vector<investdef> > agentsExportAssets();
	/*void externalUpdateAgentAssets(map<int,vector<investdef> > );*/
	void externalUpdateAgentAssets_fromStream(istream & in, bool updateRhs = false);
#ifdef MULTIPERIOD
#ifdef LIVSIM_COUPLING
	void initializeLivSimHerdsForCoupling();
#endif //LIVSIM_COUPLING
#endif//MULTIPERIOD


protected:
	// If spatial, writes yearly land use map here.
	bool landuseWasAllocated;
	Raster2D rasterLanduse;

	Raster2D rasterLookupTable;

	Raster2D regionRaster_IrrigWaterHeight_Monthly;
	
	CropGrowthHandler* p_cropGrowthHandler;

// Troost 20140527 Generalize dynamic NRUs
// Troost **********begin block insert**********
	multimap<int, int> compatibilityGroupToNRU;
	map<int, int> compatibilitySoilToGroup;

	int numSoilNRUGroupLevels;
	MatrixDouble SoilNRUGroups;
// Troost **********end block insert**********
	vector<vector<nruDistanceSizeDef> > soilDistanceSizeToNRUmap;
	vector<vector<nruDistanceDirectionUDef> > soilDistanceDirectionUdefToNRUmap;

	int* soilTypeForNRU;
	FILE* investmentsStrm;
	void openStreamInvestments();

// Troost 20141210 Land markets - type 4, assets traded with land
// Troost **********begin block insert**********
	int irrigationprojects;
	eumtr2003* eumtr2003info;
// Troost **********end block insert**********

// Troost 20171106 LivSim/LUCIA Coupling
// Troost **********begin block insert**********
	//LivSimHerdManagementTable herd_management_table;
	//LivSimHerdTable herds_table;
	//LivSimGrazingTable grazing_table;
	//LivSimFeedingTable feeding_table;
// Troost **********end block insert**********


#ifdef MONICA
	//backup of aggregate data used for Monica coupling
	int numCropActsBAK;
	int* cropActIDBAK;
	int* lpColsBAK;

	//land uses
	double* cropAreaBAK;

	//crop yields
	double* cropYieldBAK;
	double* stoverYieldBAK;

#endif
};

//For dynamic simulations of German agriculture over 2003-2005
//Distribution of entitlements for direct payments
//Struct for parameters
struct eumtr2003
{
		int year;
		double basevalArable;
		double basevalGrassland;
		double shareSetaside;
		double maxAreaNoSetaside;

		int lpColArable;
		int lpColGrassland;
		int lpColTopup;

		int objIdSetaside;

		vector<pair<double, int> > objIdsArable;
		vector<pair<double, int> > objIdsGrassland;

		map<int, pair<int,double> >   recordedSubsidies;

		eumtr2003(char* filename);

		void recordSubsidy(int, double);
		double getAverageOfRecordedSubsidy(int);

} ;


// auxiliary function
void printInflowToScreen(Raster2D& mat_InflowMedian, Raster2D& inflow);




#endif


