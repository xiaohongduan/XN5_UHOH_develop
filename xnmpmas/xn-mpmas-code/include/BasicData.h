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
//  File:       BasicData.h (formerly allgem9.h)
//
//  Contents:   Basic model data, such as model configuration switches
//
//
//
//------------------------------------------------------------------------------

#ifndef _BasicData_h
#define _BasicData_h

#include "BasicDefines.h"
#ifndef ODB


// The following (low-level) header should be included always
// Contains enums

// LINUX / WINDOWS specific definitions
#ifdef AIX
   #include <unistd.h>
	#include <limits.h>
#else
	#include <dos.h>
#endif /* AIX */

#include "BasicStrings.h"

#include "BasicEnums.h"
// Troost 20120312 - advanced MilpCheck tuning
// Troost **********begin block insert**********
#include <time.h>

// Troost **********end block insert**********
// Troost 120322 ****** begin block changed ***********
/*#ifdef AIX
   long getTimeDiffInSecAIX(time_t t2, time_t t1);
#else

#ifndef _MSC_VER
	void zeitdifferenz(struct time* t2, struct time* t1); //Tbe Feb23
   double getTimeDiffInSecWIN(struct time* t2, struct time* t1);//Tbe Feb23
#endif

#endif*/
#if (defined AIX || defined _MSC_VER || defined __MINGW32__)
   long getTimeDiffInSec(time_t t2, time_t t1);
#else
	void zeitdifferenz(struct time* t2, struct time* t1); //Tbe Feb23
   double getTimeDiffInSecWIN(struct time* t2, struct time* t1);//Tbe Feb23
#endif

// Troost 120322 ****** end block changed ***********

// This function needs to be declared in MAIN
//void exitfun();

//General parameters, formerly defines
//Maximum number of catchments (formerly subcatchments)
int NUM_CATCHMENTS();
void	setNUM_CATCHMENTS(int numC);

//Maximum number of catchment sectors
int NUM_SECTORS();
void	setNUM_SECTORS(int numS);

//Maximum number of river inflows
int NUMINFLOWS(void);
void setNUMINFLOWS(int numInflows);
int IRRIGATION();


int MONATE();           //Maximum number of irrigation months
int NW_ANZAHL();        //Maximum number of innovation networks
int SEG_ANZAHL();       //Maximum number of segments per network
int POP_ANZAHL();       //Maximum number of agent populations
int CLU_ANZAHL();       //Maximum number of clusters per population
int PRODUCER_ORGANIZATION();//Number of producer organization
int NUM_MARKETS();		//Number of regional markets
int MAXLOOP();          //Maximum number of loops before lottery is stopped
double SHARE_DEMAND_MET();	//Minimum share of water demand to be met in lottery

void setSHARE_DEMAND_MET(double share);

void NODATAVAL_def(int);// Defines No-Data-Value within program
int NODATAVAL();        

void set_MONATE(int maxM);

void set_contStrSoilMap (string _n);// Troost 150331

//flag for water rights assignment
void setWATER_RIGHTS(int flag);
int WATER_RIGHTS();

//Parameters for innovation decision
int MAXINVEST();        //Tentative number of investments per farm
int HOECHSTALTER();     //Maximum age of assets in period 0
int MINDESTALTER();     //Minimum age of assets in period 0
int KOMM();             //Type of communication process (Default = 0)
int KUMM();             //Cumulative adoption  ("classical" diffusion curve)

double GetInitialLambda();

double MARGE();         //Safe rounding factor for indivisible assets (Default = 1.0)
double MINUMFANG();     //Minimum investment in permanent crops [ha]
double OVERLAP();       //Overlap of network thresholds (Default = 1.0)
double RESERVE();       //Liquidity reserve factor (Default = 0.0)
int SYM_ANZAHL();       //Number of "symbolic" investment objects
double WAEHRUNG();      //Currency units

/*//Policy parameters
int BONUSALTER();       //Maximum age of subsidized permanent crops (Default = 0)
int BONUSDAUER();       //Duration of policy measure (Default = 0)
int IRRIGATIONPROJECTS();//initial number of irrigation infrastructure projects
*/
//Switches for various modules (= components)
int CONS_MDL();         //Type of consumption module
int CROPG_MDL();        //Type of crop growth module
void setCROPG_MDL(int _crop_mdl);

int HYDRO_MDL();        //Type of hydrology module
void setHYDRO_MDL(int _hydro_mdl);

int LIVST_MDL();        //Livestock module
int PERMC_MDL();        //Permanent crops module
int LANDM_MDL();        //Land markets module
int INPUT_LT2();        //Type of lottery input
int SPECL_MDL();        //Country-specific module
int YIELD_MDL();			//Solving LP with actual yields (needed for internal products and advanced consumption model)
int SUBVR_MDL();			//Subversion of biophysical modules
unsigned int POLICY_MDL();			//Type of policy intervention (default = none = 0),
								//Not mutually exclusive, using bitflags as defined in the next lines
// Troost 20141210 Land markets - type 4, assets traded with land
// Troost **********begin block insert**********
//Bitflags for policy models
#define POLICY_PERENNIAL_BONUS 1
#define POLICY_INFRAPROJ 2
#define POLICY_EUMTR2003 4
// Troost **********end block insert**********


int DEMOGRAPHY_MDL();	//Type of demography model
int DEMOGRAPHY_UDEF();	//user-defined household member characteristics?

// Troost 20130813 Exogenous Yield Expectations (Country Model Germany)
// Troost **********begin block insert**********
int EXO_YIELDEXP();
// Troost **********end block insert**********

//Soil types and soil classes
double PLOTSIZE();      //Size of 1 gridcell (=plot) [ha]
void	setPLOTSIZE(double _plotsize);

double	CELLLENGTH();  // Converts PLOTTSIZE to length of one cell
double DIST_COSTS();    //Unit transport costs [ZT$]
int TYPEN();            //Number of soil types
int NUMBER_NRU();       //Number of nutrient response units
int TYPE_NRU_ASSIGNMENT();//Soil types identical with NRUs (yes/no)

// Troost 20140527 Generalize dynamic NRUs
// Troost **********begin block insert**********
int NUMBER_UDEF_LSCAPE_PARAMETERS();
// Troost **********end block insert**********

///Switches for debugging purposes
int DBG_INFOS();        //debug information (yes/no)
int DYNAMICS_OFF_ALL();				//switches off all dynamics (aging, liquidity, soil processes)
int DYNAMICS_OFF_EQUITY();			//switches off changes in liquidity and equity
int DYNAMICS_OFF_HOUSEHOLDS();	//switches off aging of household members
int DYNAMICS_OFF_LIVESTOCK();		//switches off aging of livestock
int DYNAMICS_OFF_ASSETS();			//switches off aging of assets
int DYNAMICS_OFF_INVESTMENT();	//switches off investment
int DELAY_OFF_INVESTMENT();		//delays activation of switch for investment-off
int DYNAMICS_OFF_SOIL_PROCESSES();//switches off updating of soil processes (only TSPC)

void setDYNAMICS_OFF_INVESTMENT(bool);
void setDELAY_OFF_INVESTMENT(int years);

// File handling and output for dynamic switches
void readDynamicOffSwitches(FILE* bdata) ;
void readDynamicOffSwitches(string fn) ;
void writeDynamicOffSwitchesToFile(string fn)  ;
string getDynamicSwitchesAsString();
void setDynamicSwitchesFilename(string fn);
string getDynamicSwitchesFilename();

// SWITCH:
// read input flows from separate file at monthly timesteps,
// or from Catchment Fuile (EDIC version) ?
void setDYNAMIC_OFF_INFLOWS(bool);
int DYNAMIC_OFF_INFLOWS();

bool SUPPRESS_COUPLING();
void SUPPRESS_COUPLING_setTrue();
void SUPPRESS_COUPLING_setFalse();

int get_NUMBER_SPIN_UP_ROUNDS();
void set_NUMBER_SPIN_UP_ROUNDS(int numSpinUpRounds);
void set_DYNAMICS_OFF_ALL(int flag);

//Special debug info for Milp
double MIN_OBJV();      //minimum objective value
int SAVE_MILP(int IdOfMilp);//returns 'true' if MILP has to be saved
int SAVE_AGENT(int farmsteadID);//returns 'true' if agent data have to be saved

//Back- and whitelists of farm agents
void SET_BLACK_WHITE_LIST(int readLists);//true = read lists
int NUM_BLACKLIST_AGENTF();//returns number of blacklisted farm agents
int BLACKLISTED_AGENTF(int farmsteadID);//returns true if farm agent is blacklisted

int NUM_WHITELIST_AGENTF();//returns number of whitelisted farm agents
int WHITELISTED_AGENTF(int farmsteadID);//returns true if farm agent is whitelisted

//Fine-tuning of OSL Solver
int OSL_TUNE();         //fine tuning (yes/no)
double MAX_TIME();      //maximum time allowed for solving
void SET_MAX_TIME(double max_time_);//set maximum time
int MAX_ITER();         //maximum iterations
int MAX_NODE();         //maximum nodes
// Troost 20120312 - advanced MilpCheck tuning
// Troost **********begin block insert **********
//int SLV2();					//start solving with OLS level 2 (skip level 1)
int SLV_SEQUENCE();
void set_SLV_SEQUENCE(int flag);
// Troost **********end block insert **********
void SET_OSL_TUNE(bool osl_tune_);
void SET_MAX_ITER(int max_iter_);
void SET_MAX_NODE(int max_node_);

struct MpmasOslModeSetting
{
private:
	   bool log;
	   char * logfile;
	   FILE * logstrm;
	   clock_t time_started;
public:
	//specific adaptations to global settings, note only half-way implemented
   bool tuning;
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
MpmasOslModeSetting * get_SettingsForMode(int mode);
void set_ModeMaxNodes (int mode, int max_nodes_) ;
void set_ModeMaxSolutions (int mode, int max_solutions_);
bool specific_SettingsForMode (int mode) ;
// Troost **********end block insert**********

// ... can be overwritten by values passed as input arguments
double MAX_FABS();		//maximum absolute floating point
void SET_MAX_FABS(double mfabs);	//set maximum absolute floating point
int DIGITS_ROUND1();		//number of digits used for round1 "NK1"
void SET_DIGITS_ROUND1(int digits);

void set_EPSILON_FIXCOLS(double);
double EPSILON_FIXCOLS();
///////////////////////////////////////////////////////////////
// GENERAL FUNCTIONS
// ===========================================================

int NUM_VARIABLES();
void setNUM_VARIABLES(int numV);
void returnFscan(int retVal, char* strVar);

enumCoupling getCoupling();
void setCoupling(int intCommand);


//Arnold2006_93
//EDIC_RELEVANT
int   totMonth2actMonth(int mTot);

typedef struct
{
   char* sectorName;
   int lenDoubles;
   int lenInts;
   double* doubleVals;
   int* integerVals;
} EDICtable;

// Keep grid in RAM
bool SPATIAL_ON();
void set_SPATIAL_ON(bool b);

// Also allocate land use to plots
bool SPATIALLY_ALLOCATE_ON();
void set_SPATIALLY_ALLOCATE_ON(bool b);

// Round crop activities in solution vector
int SPATIAL_ALLOCATION_CORRECTED();
void set_SPATIAL_ALLOCATION_CORRECTED(int);
//reads/writes basic constants them from/in a file and returns their values
void readBasicData();
void writeBasicData(char* filename);

////////////////////////////////////////////////////////////////////////
// --> TESTFUN functions
// =====================================================================

//-------------------------------------

// This function returns TRUE if a certain output should be made.
// Internally, it uses a vector (of class MatrixDouble) which contains only zeros and ones (false / true)
// USE:
// 1) The vector is first initialized at the beginning of the program, by defining the dimension with
//        TestFunInit(int max);
//       By default, all entries are FALSE

// 2) Then, any test output level can be chosen and set to TRUE with
//          TestFun_setTrue(int testNr);
//
// 3) The optional function <getReturnIf99();> checks if TestFun(99) == true,
//    interrupts the program execution and promps for an input by the user
//    By convention, it is only used WITHIN other error outputs so the program runs through if only 99 is set TRUE!
//
// 4) Error outputs are optional now:
//      int val = 4;
//      if (TestFun(val))
//      {	cout << " blablabla" << endl;
// 		   getReturnIf99();
//          exit(1);
//      }
//
bool TestFun(int testNr);
// This function is identical to above, but does not give warnings, for use in destructors (After TestFun was destructed, while closing the program)
bool TestFunDestr(int testNr);

void TestFunInit(int max);
int TestFun_getMax();
void TestFun_setFalse(int testNr);
void TestFun_setTrue(int testNr);
void TestFun_printAllActive();

// Function for spatial export. Clone of TestFun!
bool SpatialExport(int v_);
void SpatialExportDealloc();

void SpatialExportInit();
int SpatialExport_getMaxCont();
void SpatialExport_setFalse(int v_);
void SpatialExport_setTrue(int v_);
void SpatialExport_printAllActive();

// Description of spatial info / content type
void readSpatialExportInfo();
void initializeSpatialExportInfo(); // Troost 141001
bool SpatialExportIsAtSectorLevel(Content cont);
bool SpatialExportIsMonthly(Content cont);
bool SpatialExportRequiresAgentToLandscape(Content cont);
bool SpatialExportIsPossible(Content cont); //whether spatial export of this content is possible given the current model configuration
bool SpatialExportIsAvailable(Content cont); //whether spatial export of this content is generally available
void SpatialExportPrintToScreen();
void SpatialExportPrintToScreen(Content cont);


//For debugging maps when using TDT
int GET_mapsForExternalModel();
void SET_mapsForExternalModel(int flag);

// Request return (in Test-Files)
void getReturnIf99();
void getReturnIf99(int);

// Content
string getCONTSTR(Content contIn);
void printContString(Content cont);
string returnContString(Content cont);

// ONLY RENAMED FROM ABOVE
string cont2string(Content cont);
string exptp2string(ExportType tp);

////////////////////////////////////////////////////////////////////////
// Filter Function
/* removed because unused - Troost 151026
bool FilterFun(int filterNr);
void FilterFunInit(int max);
void FilterFunDealloc();
int FilterFun_getMax();
void FilterFun_setFalse(int filterNr);
void FilterFun_setTrue(int filterNr);
void FilterFun_printAllActive();
*/
////////////////////////////////////////////////////////////////////////
// --> STRING, FILE  AND DIRECTORY TREATMENT
// =====================================================================
char* const2char(const char* exp);
void const2char(const char* exp, char* out);

string getOnlyNums(string& fn);
string getOneToken(string& fn, string& rest);
string getOneToken(string& fn);

//Why here? Is defined in BasicStreams.cpp not BasicData.cpp !!!
void makeCharFrom3Int(char* irrigIDstr, int crA, int irrTech, int clusterNum);


/////////////////////////////////////////////////////////////////////////
// Factor used to convert irrigation quanitities into quantities used by
// ======================================================================
///////////////////////////////////////////////////////
// IRRIGATION AND INFLOWS (globally) UNUSED if not COUPLWASIM
///////////////////////////////////////////////////////
string FILENAME_InflowKey();
void setFILENAME_InflowKey(string s);



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

// <--

////////////////////////////////////////////////////////////////////////
// --> MEMORY ALLOCATION FUNCTIONS
// =====================================================================

// Quit program after deallocating
void deallocateUtilities();

//Misc.
void TestFunDealloc();

// ###############################
// ## allgHigher.h HIGHER
// ###############################

// Only destruction, to avoid inclusion within deallocateUtilities()

void ExportTable_readFromFile(string fn);

void setINFLOWINFO(int pos, int val, string name);

// Read from ABSTRFILE_fn();
int readInflowKey(string fn);

//////////////////////////////////
// Access to content of INFLOW KEY
//////////////////////////////////
// To a value of the inflow (as defined in InputKey.txt), the name
// is returned
string INFLOW_val2name(int val);

// To the index of the inflow (order of InputKey.txt), the name
// is returned
string INFLOW_pos2name(int pos);

// To the index of the inflow (order of InputKey.txt), value returned
int INFLOW_pos2val(int pos);

// To the value of an inflow (as defined in InputKey.txt or in
// MEDIAN_FLOW passed by data manager), the position in RegionFile
// (InputKey.txt or Region.dat) is returned
// If not found: _NODATADEF_
int INFLOW_val2pos(int pos);
void printINFLOWS(void);

// Functions --> Header
string getFullMpmasFilename(
      EnumFileType fileTypeEnum_,
      Content cont,
      int CatchmentID = -1, int year = -1, int month = -1, int agType = -1, int marketID = -1, int userContentID = -1);
string getTestFilename(string s, int CatchmentID);

int SEED();
void setSEED(int seed_);

// For controlling number of model runs with flag Y
void setSTOPYEAR(int stopyear_);
int STOPYEAR();

void setIDUM(long idum_);
long IDUM();

int MAXPERIODIN();
void setMAXPERIODIN(int maxPeriodIn_);

string getTDTConfigFilename(TypeDataExchangeChannel,  TypeDataExchangeType);
string filenameConfigFile(TypeRoleInCoupling roleInCoupling);

void printAllIrrigationMethods();
string getStringIrrigationMethods(typeIrrigationMethod);

// --------------------------------------------
// ... Canal model
void setCANAL_MODEL(int _canalModelType);
int CANAL_MODEL();

// Used in Mode 0, 3, 4
void setCANAL_INTERNAL_REUSE(int _canalModelType);
int getCANAL_INTERNAL_REUSE();
// Used for mode 0,3,4
void setCANAL_BETA_SHARE(double _betaShare);
double getCANAL_BETA_SHARE();
// Used in Mode 1, 3, 4
void setCANAL_LOSSFACTOR(double _canalLosses);
double getCANAL_LOSSFACTOR();
// Used in Mode 1, 3, 4
void setCANAL_LAMBDA(double _canalLambda);
double getCANAL_LAMBDA();

void initializeCanalDefault();
void readCanalFile(string fn);
string getCanalContentAsString();

void setCanalFilename(string fn);
string getCanalFilename();

void init_DataManagerTest();
int getTEST(int i);
void setTEST(int val);

void printIfOptimalSpeed();
void printIfTDT();

double SURPLUS_FACTOR();
void setSURPLUS_FACTOR(double val);

#endif//ndef ODB
#endif



