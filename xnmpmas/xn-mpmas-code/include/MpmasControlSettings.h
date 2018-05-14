/*
 * MpmasControlSettings.h
 *
 *  Created on: Oct 23, 2015
 *      Author: troost
 */

#ifndef MPMASCONTROLSETTINGS_H_
#define MPMASCONTROLSETTINGS_H_

#include "BasicDefines.h" //here because ODB is set within BasicDefines

#ifdef ODB

#include <vector>
#include <string>
#include <cstring>
#include <cmath>
using namespace std;

#include "BasicEnums.h"
#include "BasicStreams.h"
#include "MpmasUtilityFunctions.h"

#ifdef RNGSTREAM
	#include "RngStream.h"
#endif

#ifdef ODBSTORE
	#include <odb/core.hxx>
#endif

using namespace std;

#define DEFINED_MODES 7
#pragma db value
struct MpmasOslModeSetting
{
private:
	   string logfile;
#pragma db transient
	   FILE * logstrm;
	   clock_t time_started;

#ifdef ODBSTORE
	   friend class odb::access;
#endif

public:
	//specific adaptations to global settings, note only half-way implemented
   bool tuning;
   bool log;
   int max_nodes;
   int max_solutions;
   double max_time;
   int max_iter;
   //fine tuning settings for mode 4
   int max_attempts;
   int lp_prepostsolve_first;
   int lp_prepostsolve_on_fail;
   int scale_first;
   int scale_on_fail;
   int crash_first;
   int crash_on_fail;
   int crash_mode;
   int integer_presolve_first;
   int integer_presolve_on_fail;
   int osl_integer_strategy;
   int osl_heuristic_passes;
   int maxint_first_attempts;
   int maxnodes_first_attempts;
   int maxsols_first_attempts;
   double max_diff_to_accept_first;
   double max_reldiff_to_accept_first;
   int continue_first_fail;
   int continue_mode3;
   int addrows;

   //constructor to set initial values -- add initialization if you add a member
   MpmasOslModeSetting();
   //destructor to close log file, if logging has been done
   ~MpmasOslModeSetting();
   //method to print to screen
   void print_SettingsToScreen();
   bool initialize_log ();
   bool logging();
   void write_to_log(int fstID, int milpId, int attempt, char * Msg, int rtcod, int status,
			int NumIter, int NumNodes, int NumSol, double BestFound, double BestEstm, double BestPoss);
   void flush_log();
   void start_clock();
   double elapsed();
   void stop_clock();
} ;


#pragma db object
class mpmasControlSettings {


public:
//Constructors and Destructors
	mpmasControlSettings ();
	~mpmasControlSettings (){};

//Initialization
	void inputArgumentHandling(int ac, char **av, int afterRestore = false);
	void readBasicData();
	void writeBasicData(char* filename);
	int NUM_VARIABLES() 					{			return num_variables; 	}
	void setNUM_VARIABLES(int numV) 	{			num_variables = numV;	}
	void returnFscan(int retVal, char* strVar);
#ifdef ODBSTORE
	void postRestoreRoutines();
#endif
//Helpful screen output
	static void printToScreen_InputOptions(); //Note: Class static function
	static void printToScreen_TestFunOptions(); //Note: Class static function




//Additional output settings
	bool TestFun(int testNr);
	// This function is identical to above, but does not give warnings, for use in destructors (After TestFun was destructed, while closing the program)
	void TestFun_setFalse(int testNr);
	void TestFun_setTrue(int testNr);
	int TestFun_getMax();

//Spatial allocation and export
	bool SPATIAL_ON()	{		return spatial_on;	}
	void set_SPATIAL_ON(bool b)	{		spatial_on = b;	}
	bool SPATIALLY_ALLOCATE_ON()	{		return spatially_allocate_on;	}
	void set_SPATIALLY_ALLOCATE_ON(bool b) {		spatially_allocate_on = b;	}
	int SPATIAL_ALLOCATION_CORRECTED(void)	{		return spatial_allocation_corrected;	}
	void set_SPATIAL_ALLOCATION_CORRECTED(int b)	{		spatial_allocation_corrected = b;	}

	bool SpatialExport(int v_);
	void SpatialExport_setFalse(int v_);
	void SpatialExport_setTrue(int v_);
	int SpatialExport_getMaxCont();
	void SpatialExport_printAllActive();

	// Description of spatial info / content type
	bool SpatialExportIsAtSectorLevel(Content cont);
	bool SpatialExportIsMonthly(Content cont);
	bool SpatialExportRequiresAgentToLandscape(Content cont);
	bool SpatialExportIsPossible(Content cont); //whether spatial export of this content is possible given the current model configuration
	bool SpatialExportIsAvailable(Content cont); //whether spatial export of this content is generally available
	void SpatialExportPrintToScreen();
	void SpatialExportPrintToScreen(Content cont);

//File names and directories
	void set_OUTDIR(string outStr);
	string OUTDIR();
	void set_SIMNAME(string outStr);
	string SIMNAME();
	void set_XMLDIR(string outStr);
	string XMLDIR();
	void set_TEMPDIR(string outStr);
	string TEMPDIR();
	void set_INDIR(string outStr);
	string INDIR();
	void set_MASINPUTDIR(string out);
	string MASINPUTDIR();
	void set_MASOUTPUTDIR(string out);
	string MASOUTPUTDIR();
	string OUTGIS() {		return outgis;	}
	void set_OUTGIS(string outStr)	{		outgis = outStr;	}



	string SECTOR_MAP_NAME()	{		return sectorMapName;	}
	void set_SECTOR_MAP_NAME(string strg)	{		sectorMapName = strg;	}
	string GIS_FOLDER_NAME()	{		return gisFolderName;	}
	void set_GIS_FOLDER_NAME(string strg)	{		gisFolderName = strg;	}
	string PATH_ACT_PRECIPITATION()	{		return pathActPrecip;	}
	void set_PATH_ACT_PRECIPITATION(string strg)	{		pathActPrecip = strg;	}
	string PATH_POT_EVAPOTRANSPIRATION()	{		return pathPotEtr;	}
	void set_PATH_POT_EVAPOTRANSPIRATION(string strg)	{		pathPotEtr = strg;	}
	string PATH_ACT_PRICES()	{		return pathActPrices;	}
	void set_PATH_ACT_PRICES(string strg)	{		pathActPrices = strg;	}
	string ADD_OUTPUT_NAME()	{		return addOutputName;	}
	void set_ADD_OUTPUT_NAME(string strg)	{		addOutputName = strg;	}


	string getTestFilename(string s, int CatchmentID);
	string getFullMpmasFilename(
	      EnumFileType fileTypeEnum_,
	      Content cont,
	      int CatchmentID = -1, int year = -1, int month = -1, int agType = -1, int marketID = -1, int userContentID = -1);
#ifdef ODBSTORE
	//for ODB snapshot storage
	void set_snapshotFilestub(string outstring);
	string get_snapshotFilestub();
#endif
	string getMpmasVersion();

	//For debugging maps when using TDT
	int GET_mapsForExternalModel()	{		return write_MapsForExternalCropModel;	}
	void SET_mapsForExternalModel(int flag)	{		write_MapsForExternalCropModel = flag;	}

	// Request return (in Test-Files)
	void getReturnIf99(int i = -1);

	// Content
	string getCONTSTR(Content contIn);
	ExportType getExpTpFromContent(Content cont);
	string getEXPSTRING(ExportType expTP);
	Content getContFromExpTP(ExportType expTP);

	void printContString(Content cont)	{		cout << getCONTSTR(cont);	}
	string returnContString(Content cont)	{		return getCONTSTR(cont);	}
	string cont2string(Content cont)	{		return returnContString(cont);	}
	ExportType cont2exptp(Content cont){		return getExpTpFromContent(cont);}
	Content exptp2cont(ExportType expTP)	{		return getContFromExpTP(expTP);	}
	string exptp2string(ExportType tp) {	return cont2string(exptp2cont(tp)); }


//MIP solving
//Debugging:
	void set_WRITE_MPX(bool b_);
	bool WRITE_MPX();



	void set_SAVE_ALL_MTX(bool b_)		{		flagSaveAllMtx = b_;	}
	bool SAVE_ALL_MTX()	{		return flagSaveAllMtx;	}

//OSL options:
	MpmasOslModeSetting * get_SettingsForMode(int mode);
	void set_ModeMaxNodes (int mode, int max_nodes_) ;
	void set_ModeMaxSolutions (int mode, int max_solutions_);
	bool specific_SettingsForMode (int mode) ;


//Mpmas configuration options

	int NUM_CATCHMENTS()					{		return num_subcatch;	}
	void setNUM_CATCHMENTS(int numC)	{		num_subcatch = numC;	}
	int NUM_SECTORS()						{		return num_sectors;	}
	void setNUM_SECTORS(int numS_)	{		num_sectors = numS_;	}
	int NUMINFLOWS(void)					{		return numberOfInflows;	}
	void setNUMINFLOWS(int numInflows) {	numberOfInflows = numInflows;	}
	int IRRIGATION()						{		return withIrrigation;	}
	int MONATE();
	void set_MONATE(int maxM);

	//Type of water rights assignment

	void setWATER_RIGHTS(int flag)	{		water_rights = flag;	}
	int WATER_RIGHTS()	{		return water_rights;	}

	int NW_ANZAHL()	{		return nw_anzahl;	}
	int SEG_ANZAHL()	{		return seg_anzahl;	}
	int POP_ANZAHL()	{		return pop_anzahl;	}
	int CLU_ANZAHL()	{		return clu_anzahl;	}
	int PRODUCER_ORGANIZATION()	{		return producer_organization;	}
	int NUM_MARKETS()	{		return num_markets;	}
	int MAXLOOP()	{		return maxloop;	}
	double SHARE_DEMAND_MET()	{		return share_demand_met;	}
	void setSHARE_DEMAND_MET(double share)	{		share_demand_met = share;	}

	//Parameters for innovation decisions
	int MAXINVEST()	{		return maxinvest;	}
	int HOECHSTALTER()	{		return hoechstalter;	}
	int MINDESTALTER()	{		return mindestalter;	}
	int KOMM()	{		return komm;	}
	int KUMM()	{		return kumm;	}
	double MARGE()	{		return marge;	}
	double MINUMFANG()	{		return minumfang;	}
	double OVERLAP()	{		return overlap;	}
	double RESERVE()	{		return reserve;	}
	int SYM_ANZAHL()	{		return sym_anzahl;	}
	double WAEHRUNG()	{		return waehrung;	}

	double GetInitialLambda()	{		return initialLambda;	}


	//Switches for various modules (= components)
	int CONS_MDL()	{		return cons_mdl;	}
	int CROPG_MDL()	{		return cropg_mdl;	}
	void setCROPG_MDL(int _crop_mdl)	{		cropg_mdl = _crop_mdl;	}
	void setHYDRO_MDL(int _hydro_mdl)	{		hydro_mdl = _hydro_mdl;	}
	int HYDRO_MDL()	{		return hydro_mdl;	}
	int LIVST_MDL()	{		return livst_mdl;	}
	int PERMC_MDL()	{		return permc_mdl;	}
	int LANDM_MDL()	{		return landm_mdl;	}
	int INPUT_LT2()	{		return input_lt2;	}
	int SPECL_MDL()	{		return specl_mdl;	}
	int YIELD_MDL()	{		return yield_mdl;	}
	int SUBVR_MDL()	{		return subvr_mdl;	}
	unsigned int POLICY_MDL() 	{		return policy_mdl;	}
	int DEMOGRAPHY_MDL()	{		return demography_mdl;	}
	int DEMOGRAPHY_UDEF()	{		return demography_udef;	}
	int NUMBER_UDEF_LSCAPE_PARAMETERS()	{		return numUserDefinedLandscapeParameters;	}
	int CHANGING_SOIL_TYPES() {return changingSoilTypes;}
	int EXO_YIELDEXP()	{		return exo_yieldexp;	}

	//Soil types and soil classes
	double PLOTSIZE()	{		return plotsize;	}
	void setPLOTSIZE(double _plotsize)	{		plotsize = _plotsize;	}
	double CELLLENGTH()	{		return sqrt((double) PLOTSIZE() * 10000);	}
	double DIST_COSTS()	{		return dist_costs;	}
	int TYPEN()	{		return typen;	}
	int NUMBER_NRU()	{		return number_nru;	}
	int TYPE_NRU_ASSIGNMENT()	{		return type_nru_assignment;	}
	typeofUserdefinedNRUassignment TYPE_OF_USERDEFINED_NRU_ASSIGNMENT() { return typeUserDefinedNRUassignment;}
	//------------------------------------------------------

	//Switches for debugging purposes
	int DBG_INFOS()	{		return dbg_infos;	}
	//switches off change in all resources
	int DYNAMICS_OFF_ALL() 	{		return dynamics_off_all;	}
	//switches off change in equity
	int DYNAMICS_OFF_EQUITY()	{		return dynamics_off_equity;	}
	//switches off aging of household members
	int DYNAMICS_OFF_HOUSEHOLDS()	{		return dynamics_off_households;	}
	//switches off aging of livestock
	int DYNAMICS_OFF_LIVESTOCK()	{		return dynamics_off_livestock;	}
	//switches off aging of assets
	int DYNAMICS_OFF_ASSETS()	{		return dynamics_off_assets;	}
	//switches off investments
	int DYNAMICS_OFF_INVESTMENT()	{		return dynamics_off_investment;	}
	void setDYNAMICS_OFF_INVESTMENT(bool flag)	{		dynamics_off_investment = (int) flag;	}
	//delays activation of switch by certain number of years
	int DELAY_OFF_INVESTMENT()	{		return delay_off_investment;	}
	void setDELAY_OFF_INVESTMENT(int years)	{		delay_off_investment = years;	}

	//switches off updating of soil processes
	int DYNAMICS_OFF_SOIL_PROCESSES()	{		return dynamics_off_soil_processes;	}

	//switches dynamic inflows off
	void setDYNAMIC_OFF_INFLOWS(bool flag)	{		dynamics_off_inflows = (int) flag;	}

	int DYNAMIC_OFF_INFLOWS()	{		return dynamics_off_inflows;	}

	//number of spin-up rounds
	int get_NUMBER_SPIN_UP_ROUNDS()	{		return number_spin_up_rounds;	}

	//set number of spin-up rounds
	void set_NUMBER_SPIN_UP_ROUNDS(int numSpinUpRounds)	{		number_spin_up_rounds = numSpinUpRounds;	}

	//switch flag
	void set_DYNAMICS_OFF_ALL(int flag)	{		dynamics_off_all = flag;	}

	//------------------------------------------------------

	//Special debug info for Milp
	double MIN_OBJV()	{		return min_objv;	}

	int SAVE_MILP(int nmp); //return 'true' if MILP has to be saved
	int SAVE_AGENT(int farmsteadID);
	//------------------------------------------------------

	void SET_BLACK_WHITE_LIST(int readLists)	{		readBlackWhitelistedAgents = readLists;	}
	//return number of blacklisted farm agents
	int NUM_BLACKLIST_AGENTF()	{		return numberBlacklistedAgentsF;	}
	//check if farm agent is blacklisted
	int BLACKLISTED_AGENTF(int farmsteadID);
	//return number of whitelisted farm agents
	int NUM_WHITELIST_AGENTF()	{		return numberWhitelistedAgentsF;	}
	//check if farm agent is whitelisted
	int WHITELISTED_AGENTF(int farmsteadID);
	//------------------------------------------------------

	//Fine-tuning of OSL
	int OSL_TUNE()								{		return osl_tune;	}
	void SET_OSL_TUNE(bool osl_tune_)	{		osl_tune = osl_tune_;	}
	double MAX_TIME()							{		return max_time;	}
	void SET_MAX_TIME(double max_time_)	{		max_time = max_time_;	}
	int MAX_ITER()								{		return max_iter;	}
	void SET_MAX_ITER(int max_iter_)		{		max_iter = max_iter_;	}
	int MAX_NODE()								{		return max_node;	}
	void SET_MAX_NODE(int max_node_)		{		max_node = max_node_;	}
	int SLV_SEQUENCE()						{		return slv_sequence;	}
	void set_SLV_SEQUENCE(int flag)		{		slv_sequence = flag;	}


	double MAX_FABS()								{		return max_fabs;	}
	void SET_MAX_FABS(double mfabs)			{		max_fabs = mfabs;	}
	int DIGITS_ROUND1()							{		return digits_round1;	}
	void SET_DIGITS_ROUND1(int digits)		{		digits_round1 = digits;	}
	void set_EPSILON_FIXCOLS(double val_)	{		epsilon_fixcols = val_;	}
	double EPSILON_FIXCOLS()					{		return epsilon_fixcols;	}

//
	int STOPYEAR()									{		return stopyear;	}
	void setSTOPYEAR(int stopyear_)			{		stopyear = stopyear_;	}
	int SEED()										{		return seed;	}
	void setSEED(int seed_);
	int MAXPERIODIN()								{		return maxPeriodIn;	}
	void setMAXPERIODIN(int maxPeriodIn_)	{		maxPeriodIn = maxPeriodIn_;	}
	void setIDUM(long idum_)					{		idum = idum_;	}
	long IDUM()										{		return idum;	}

//Coupling
	string getTDTConfigFilename(TypeDataExchangeChannel,  TypeDataExchangeType);
	string filenameConfigFile(TypeRoleInCoupling roleInCoupling);


	bool SUPPRESS_COUPLING()	{		return suppress_coupling;	}
	void SUPPRESS_COUPLING_setTrue()	{		suppress_coupling = true;	}
	void SUPPRESS_COUPLING_setFalse()	{		suppress_coupling = false;	}

// EDIC
	void printAllIrrigationMethods();
	string getStringIrrigationMethods(typeIrrigationMethod);

//EDIC: Canal model
	int CANAL_MODEL() 										{		return CanalModelType;	}
	void setCANAL_MODEL(int _canalModelType);
	void setCANAL_BETA_SHARE(double _betaShare)		{		BetaShareGlobal = _betaShare;	}
	double getCANAL_BETA_SHARE()							{		return BetaShareGlobal;	}
	void setCANAL_LOSSFACTOR(double _canalLosses)	{		CanalLossesGlobal = _canalLosses;	}
	double getCANAL_LOSSFACTOR()							{		return CanalLossesGlobal;	}
	void setCANAL_LAMBDA(double _canalLambda)			{		CanalLambdaGlobal = _canalLambda;	}
	double getCANAL_LAMBDA()								{		return CanalLambdaGlobal;	}
	void setCanalFilename(string fn)						{		filename_Canals = fn;	}
	string getCanalFilename()								{		return filename_Canals;	}

	void initializeCanalDefault();
	void readCanalFile(string fn);
	string getCanalContentAsString();

	double SURPLUS_FACTOR()									{		return surplus_factor_global;	}
	void setSURPLUS_FACTOR(double val)					{		surplus_factor_global = val;	}


	void LAMBDA_init(double initialLambda);
	double LAMBDA(int type);//Parameter for expectations (Brandes et al., 1997: 382)
	void SET_LAMBDA(int type, double value);

	// For varying lambda: read values from file
	string LAMBDA_filename();
	void set_LAMBDA_filename(string s);

	// Lambda i/o functions
	void writeLambdaToFile(string s);
	string getLambdaAsSting();
	void readLambdaFromFile();

	//Special file to deactivate sectors
	string getInactiveSectorsFilename();
	void setInactiveSectorsFilename(string fn);

	// Defines Not A Value within program
	void NODATAVAL_def(int nodat)	{		nodatavalue = nodat;	}
	// Return NaValue
	int NODATAVAL()	{		return nodatavalue;	}

	void readDynamicOffSwitches(FILE* bdata) ;
	void readDynamicOffSwitches(string fn) ;
	void writeDynamicOffSwitchesToFile(string fn)  ;
	string getDynamicSwitchesAsString();
	void setDynamicSwitchesFilename(string fn);
	string getDynamicSwitchesFilename();


	enumCoupling getCoupling()	{		return couplingWith;	}
	void setCoupling(int intCommand);
	void set_contStrSoilMap (string _n)	{		contStrSoilMap = _n;	}


	// output
	void irrigationFactors_printOptions();
	void irrigationFactors_printCurrentSetting();

	// input
	void setIrrigationDefaults();
	int readIrrigationFactors(string fn);
	void setIRRIGFACTOR(int unitType, enumIrrigationFactorType conversionType);

	// access
	double IRRIGFACTOR(enumIrrigationFactorType conversionType);
	int getIrrigationConversionID_fromConversionType(enumIrrigationFactorType conversionType);
	int getIrrigUnitForWasimUnit(int conversionID); //int getIRRIGfactorValue(int unitType);
	double getFactorFromUnitType(int conversionID);

	// string
	string irrigationUnitToString(int conversionID); // irrigationFactorUnitAsStringFromType
	string getIrrigationFactorDescription(enumIrrigationFactorType);

#ifdef RNGSTREAM

	RngStream* getPtrToRandomGenerator_Households() {return &RngStreamHouseholds; }
	RngStream* getPtrToRandomGenerator_Farms() {return &RngStreamFarms; }
	int getRngHouseholdMembersFstIdShifter () { return rngHouseholdMembersFstIdShifter;}
#endif

#ifdef IP_COUPLING
	string getHostForCoupling() { return hostForCoupling;}
	int getPortForCoupling() { return portForCoupling; }
	void setHostForCoupling(string s_) { hostForCoupling = s_;}
	void setPortForCoupling(int p_) { portForCoupling = p_; }
#endif

	int EXTRAGAP() {return sparseMatrixExtraGap;}
	void set_EXTRAGAP(int i) {sparseMatrixExtraGap = i;}

private:
		#pragma db id
		unsigned long settingsID;
#ifdef ODBSTORE
		friend class odb::access;
#endif
		int num_variables;

		enumCoupling couplingWith;

		//General parameters
		int num_subcatch;
		int num_sectors;

		int nw_anzahl;
		int seg_anzahl;
		int pop_anzahl;
		int clu_anzahl;
		int maxloop;
		double share_demand_met;
		int nodatavalue;
		int producer_organization;
		int num_markets;
		int numberOfInflows;
		int water_rights ;
		#ifdef MULTIPERIOD
		 int multiperiod_yno;
		#endif



		//Parameters for innovation decisions
		int maxinvest;
		int hoechstalter;
		int mindestalter;
		int komm;
		int kumm;

		double marge;
		double minumfang;
		double overlap;
		double reserve;
		int sym_anzahl;
		double waehrung;

		double initialLambda;
		//Switches for various modules (= components)
		int cons_mdl;
		int cropg_mdl;
		int hydro_mdl;
		int livst_mdl;
		int permc_mdl;
		int landm_mdl;
		int input_lt2;
		int specl_mdl;
		int yield_mdl;
		int subvr_mdl;
		unsigned int policy_mdl;
		int withIrrigation;
		int demography_mdl;
		int demography_udef;
		int exo_yieldexp;


		double plotsize;
		double dist_costs;
		int typen;
		int number_nru;
		int type_nru_assignment;
		typeofUserdefinedNRUassignment typeUserDefinedNRUassignment;
		int numUserDefinedLandscapeParameters;

// Troost 20160331 Soil type transitions
// Troost **********begin block insert**********
		int changingSoilTypes;
// Troost **********end block insert**********

		//Switches for debugging purposes
		int dbg_infos;
		int number_spin_up_rounds;
		int dynamics_off_all;
		int dynamics_off_equity;
		int dynamics_off_households;
		int dynamics_off_livestock;
		int dynamics_off_assets;
		int dynamics_off_investment;
		int delay_off_investment;
		int dynamics_off_soil_processes;
		int dynamics_off_inflows;

		//Special debug info for Milp
		double min_objv;
		int num_milp;

		//odb change
		vector<int> save_milp;
		bool flagSaveAllMtx;

		//blacklist of farm agents
		int readBlackWhitelistedAgents;
		int numberBlacklistedAgentsF;
		vector<int> blacklistAgentF;

		//whitelist of farm agents
		int numberWhitelistedAgentsF;
		vector<int> whitelistAgentF ;

		//Fine-tuning of OSL
		int osl_tune;
		double max_time;
		int max_iter;
		int max_node;
		double max_fabs;
		int digits_round1;
		double epsilon_fixcols;
		int slv_sequence;
		vector<MpmasOslModeSetting> mode_settings;

		int wasim_deltamonth;
		int write_MapsForExternalCropModel;

		string contStrSoilMap; //Troost 150331
		string indir;
		string outdir;
		string simname;
		string xmldir;
		string tempdir;
		string masinputdir;
		string masoutputdir;
		string snapshotFilestub;
		string mpmasVersion;
		string outgis;

		bool flagWriteMpx;
		vector<int>testFlags;

		bool spatial_on;
		bool spatially_allocate_on;
		int spatial_allocation_corrected;
		vector<int>spatialExportFlags;

		int seed;
		int stopyear;
		int maxPeriodIn;
		long idum;

		int CanalModelType;
		// Used for mode 0,3,4
		double BetaShareGlobal;
		// Used in Mode 1, 3, 4
		double CanalLossesGlobal;
		// Used in Mode 1, 3, 4
		double CanalLambdaGlobal;

		string filename_Canals;
		bool suppress_coupling; //Tbe 090428 variable should be initialized with default
		double surplus_factor_global; // Tbe 110427 initialized with default

		vector<double> lambda;
		string lambda_filename;
//		MatrixDouble forecast_meteo;
		//special function for coupling with Monica
		string inactiveSectors_Filename;
		string addOutputName; // Tbe 150220
		string pathActPrices; // Tbe 150212
		string pathPotEtr; // Tbe 150212
		string pathActPrecip; // Tbe 150212
		string gisFolderName;
		string sectorMapName;
		string dynamicSwitchesFilename;

#ifdef RNGSTREAM
		vector<unsigned long>rngStreamPackageSeed;
		int rngHouseholdMembersFstIdShifter;
#pragma db transient
		RngStream RngStreamHouseholds;
#pragma db transient
		RngStream RngStreamFarms;
#endif

#pragma db transient
		double irrigationFactorGlobal[typeIrrig_MAX];
#pragma db transient
		int irrigationUnitTypeGlobal[typeIrrig_MAX];

#ifdef IP_COUPLING
#pragma db transient
		string hostForCoupling;
#pragma db transient
		int portForCoupling;
#endif//IP_COUPLING

		int sparseMatrixExtraGap;
};


#endif //ODB
#endif /* MPMASCONTROLSETTINGS_H_ */
