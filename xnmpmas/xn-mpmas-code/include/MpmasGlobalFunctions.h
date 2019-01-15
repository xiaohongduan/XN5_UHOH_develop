#ifndef _MPMAS_GLOBAL_FUNCTIONS
#define _MPMAS_GLOBAL_FUNCTIONS

#include "BasicDefines.h"
#ifdef ODB
//////----------------------------------------------------------------------------
///	This is a global wrapper class around the mpmasControlSettings class
///	in order to make the basic configuration parameters available to any subclass
///	without having to pass the mpmasControlSettings pointer around.
///	(and especially to save us from the work of recoding all the functions in
///	the original files
//////-----------------------------------------------------------------------------

#ifdef AIX
   #include <unistd.h>
	#include <limits.h>
#else
	#include <dos.h>
#endif /* AIX */
#include <time.h>
#include "MpmasControlSettings.h"





namespace mpmasGlobal {

	//global pointers to the mpmas class and the settings member of that class
	//this are set and initialized in the mpmas constructor, resp. mpmas initializeControl
	extern	mpmasControlSettings* globalMpmasSettings;

	//can be used only after mpmas has been constructed!

//General parameters, formerly defines
//Maximum number of catchments (formerly subcatchments)
	inline int NUM_CATCHMENTS() 			{	return globalMpmasSettings->NUM_CATCHMENTS();   }
	//void	setNUM_CATCHMENTS(int numC) {	globalMpmasSettings->setNUM_CATCHMENTS(numC);   };

	//Maximum number of catchment sectors
	inline int NUM_SECTORS() 				{	return globalMpmasSettings->NUM_SECTORS();   }
	//void	setNUM_SECTORS(int numS);

	//Maximum number of river inflows
	inline int NUMINFLOWS(void)				{	return globalMpmasSettings->NUMINFLOWS();   }
	inline void setNUMINFLOWS(int numInflows) {globalMpmasSettings->setNUMINFLOWS(numInflows);}
	inline int IRRIGATION(){	return globalMpmasSettings->IRRIGATION();   }


	inline int MONATE()						{	return globalMpmasSettings->MONATE();   }          //Maximum number of irrigation months
	inline int NW_ANZAHL()					{	return globalMpmasSettings->NW_ANZAHL();   }        //Maximum number of innovation networks
	inline int SEG_ANZAHL()					{	return globalMpmasSettings->SEG_ANZAHL();   }       //Maximum number of segments per network
	inline int POP_ANZAHL(){	return globalMpmasSettings->POP_ANZAHL();   }       //Maximum number of agent populations
	inline int CLU_ANZAHL(){	return globalMpmasSettings->CLU_ANZAHL();   }       //Maximum number of clusters per population
	inline int PRODUCER_ORGANIZATION(){	return globalMpmasSettings->PRODUCER_ORGANIZATION();   }//Number of producer organization
	inline int NUM_MARKETS(){	return globalMpmasSettings->NUM_MARKETS();   }		//Number of regional markets
	inline int MAXLOOP(){	return globalMpmasSettings->MAXLOOP();   }          //Maximum number of loops before lottery is stopped
	inline double SHARE_DEMAND_MET() {	return globalMpmasSettings->SHARE_DEMAND_MET();   }	//Minimum share of water demand to be met in lottery

	inline void setSHARE_DEMAND_MET(double share) {	globalMpmasSettings->setSHARE_DEMAND_MET(share);   }

	//void NODATAVAL_def(int);// Defines No-Data-Value within program
	inline int NODATAVAL(){	return globalMpmasSettings->NODATAVAL();   }

	//void set_MONATE(int maxM){	globalMpmasSettings->set_MONATE(maxM);   }

	//void set_contStrSoilMap (string _n);// Troost 150331

	//flag for water rights assignment
	inline void setWATER_RIGHTS(int flag) {	globalMpmasSettings->setWATER_RIGHTS(flag);   }
	inline int WATER_RIGHTS(){	return globalMpmasSettings->WATER_RIGHTS();   }

	//Parameters for innovation decision
	inline int MAXINVEST(){	return globalMpmasSettings->MAXINVEST();   }       //Tentative number of investments per farm
	inline int HOECHSTALTER(){	return globalMpmasSettings->HOECHSTALTER();   }     //Maximum age of assets in period 0
	inline int MINDESTALTER(){	return globalMpmasSettings->MINDESTALTER();   }    //Minimum age of assets in period 0
	inline int KOMM(){	return globalMpmasSettings->KOMM();   }            //Type of communication process (Default = 0)
	inline int KUMM(){	return globalMpmasSettings->KUMM();   }             //Cumulative adoption  ("classical" diffusion curve)

	inline double GetInitialLambda() {	return globalMpmasSettings->GetInitialLambda();   }

	inline double MARGE(){	return globalMpmasSettings->MARGE();   }         //Safe rounding factor for indivisible assets (Default = 1.0)
	inline double MINUMFANG(){	return globalMpmasSettings->MINUMFANG();   }    //Minimum investment in permanent crops [ha]
	inline double OVERLAP(){	return globalMpmasSettings->OVERLAP();   }      //Overlap of network thresholds (Default = 1.0)
	inline double RESERVE(){	return globalMpmasSettings->RESERVE();   }       //Liquidity reserve factor (Default = 0.0)
	inline int SYM_ANZAHL(){	return globalMpmasSettings->SYM_ANZAHL();   }       //Number of "symbolic" investment objects
	inline double WAEHRUNG(){	return globalMpmasSettings->WAEHRUNG();   }      //Currency units


	//Switches for various modules (= components)
	inline int CONS_MDL(){	return globalMpmasSettings->CONS_MDL();   }          //Type of consumption module
	inline int CROPG_MDL(){	return globalMpmasSettings->CROPG_MDL();   }         //Type of crop growth module
	inline void setCROPG_MDL(int _crop_mdl){	 globalMpmasSettings->setCROPG_MDL(_crop_mdl);   }

	inline int HYDRO_MDL(){	return globalMpmasSettings->HYDRO_MDL();   }        //Type of hydrology module
	//void setHYDRO_MDL(int _hydro_mdl);

	inline int LIVST_MDL(){	return globalMpmasSettings->LIVST_MDL();   }         //Livestock module
	inline int PERMC_MDL(){	return globalMpmasSettings->PERMC_MDL();   }         //Permanent crops module
	inline int LANDM_MDL(){	return globalMpmasSettings->LANDM_MDL();   }         //Land markets module
	inline int INPUT_LT2(){	return globalMpmasSettings->INPUT_LT2();   }         //Type of lottery input
	inline int SPECL_MDL(){	return globalMpmasSettings->SPECL_MDL();   }         //Country-specific module
	inline int YIELD_MDL(){	return globalMpmasSettings->YIELD_MDL();   } 			//Solving LP with actual yields (needed for internal products and advanced consumption model)
	inline int SUBVR_MDL(){	return globalMpmasSettings->SUBVR_MDL();   } 			//Subversion of biophysical modules

	//Type of policy intervention (default = none = 0),
	//Not mutually exclusive, using bitflags as defined in the next lines
	inline unsigned int POLICY_MDL(){	return globalMpmasSettings->POLICY_MDL();   }
	//Bitflags for policy models
	#define POLICY_PERENNIAL_BONUS 1
	#define POLICY_INFRAPROJ 2
	#define POLICY_EUMTR2003 4
   #define POLICY_PREINVESTMENT_EXTERNAL_SCRIPT 8
	#define POLICY_INYEAR_EXTERNAL_SCRIPT 16
	#define POLICY_COMMON_PROPERTY 32


	inline int ADDITIONAL_DECISION_STAGES() { return globalMpmasSettings->ADDITIONAL_DECISION_STAGES();}

	inline int DEMOGRAPHY_MDL(){	return globalMpmasSettings->DEMOGRAPHY_MDL();   }	//Type of demography model
	inline int DEMOGRAPHY_UDEF(){	return globalMpmasSettings->DEMOGRAPHY_UDEF();   }	//user-defined household member characteristics?

	// Troost 20130813 Exogenous Yield Expectations (Country Model Germany)
	// Troost **********begin block insert**********
	inline int EXO_YIELDEXP(){	return globalMpmasSettings->EXO_YIELDEXP();   }
	// Troost **********end block insert**********

	//Soil types and soil classes
	inline double PLOTSIZE(){	return globalMpmasSettings->PLOTSIZE();   }      //Size of 1 gridcell (=plot) [ha]
	//void	setPLOTSIZE(double _plotsize);

	inline double CELLLENGTH(){	return globalMpmasSettings->CELLLENGTH();   }   // Converts PLOTTSIZE to length of one cell
	inline double DIST_COSTS(){	return globalMpmasSettings->DIST_COSTS();   }     //Unit transport costs [ZT$]
	inline int TYPEN(){	return globalMpmasSettings->TYPEN();   }             //Number of soil types in maps
	inline int NUMBER_NRU(){	return globalMpmasSettings->NUMBER_NRU();   }        //Number of nutrient response units (soil classes) in LP
	inline int TYPE_NRU_ASSIGNMENT(){	return globalMpmasSettings->TYPE_NRU_ASSIGNMENT();   } //Soil types identical with NRUs (yes/no)
	inline typeofUserdefinedNRUassignment TYPE_OF_USERDEFINED_NRU_ASSIGNMENT() { return globalMpmasSettings->TYPE_OF_USERDEFINED_NRU_ASSIGNMENT();   }
	// Troost 20140527 Generalize dynamic NRUs
	// Troost **********begin block insert**********
	inline int NUMBER_UDEF_LSCAPE_PARAMETERS(){	return globalMpmasSettings->NUMBER_UDEF_LSCAPE_PARAMETERS();   }
	inline int NUMBER_UDEF_LSCAPE_PARAMETERS_FOR_CATEG(){	return globalMpmasSettings->NUMBER_UDEF_LSCAPE_PARAMETERS_FOR_CATEG();   }
	inline void set_NUMBER_UDEF_LSCAPE_PARAMETERS_FOR_CATEG(int i){	return globalMpmasSettings->set_NUMBER_UDEF_LSCAPE_PARAMETERS_FOR_CATEG(i) ;   }
	// Troost **********end block insert**********

	inline int CHANGING_SOIL_TYPES(){	return globalMpmasSettings->CHANGING_SOIL_TYPES();   }

	///Switches for debugging purposes
	inline int DBG_INFOS(){	return globalMpmasSettings->DBG_INFOS();   }        //debug information (yes/no)
	inline int DYNAMICS_OFF_ALL(){	return globalMpmasSettings->DYNAMICS_OFF_ALL();   }				//switches off all dynamics (aging, liquidity, soil processes)
	inline int DYNAMICS_OFF_EQUITY(){	return globalMpmasSettings->DYNAMICS_OFF_EQUITY();   }			//switches off changes in liquidity and equity
	inline int DYNAMICS_OFF_HOUSEHOLDS(){	return globalMpmasSettings->DYNAMICS_OFF_HOUSEHOLDS();   }	//switches off aging of household members
	inline int DYNAMICS_OFF_LIVESTOCK(){	return globalMpmasSettings->DYNAMICS_OFF_LIVESTOCK();   }		//switches off aging of livestock
	inline int DYNAMICS_OFF_ASSETS(){	return globalMpmasSettings->DYNAMICS_OFF_ASSETS();   }			//switches off aging of assets
	inline int DYNAMICS_OFF_INVESTMENT(){	return globalMpmasSettings->DYNAMICS_OFF_INVESTMENT();   }	//switches off investment
	inline int DELAY_OFF_INVESTMENT(){	return globalMpmasSettings->DELAY_OFF_INVESTMENT();   }		//delays activation of switch for investment-off
	inline int DYNAMICS_OFF_SOIL_PROCESSES(){	return globalMpmasSettings->DYNAMICS_OFF_SOIL_PROCESSES();   }//switches off updating of soil processes (only TSPC)

	inline void setDYNAMICS_OFF_INVESTMENT(bool y){	globalMpmasSettings->setDYNAMICS_OFF_INVESTMENT(y);   }
	inline void setDELAY_OFF_INVESTMENT(int years){	globalMpmasSettings->setDELAY_OFF_INVESTMENT(years);   }

	// File handling and output for dynamic switches
	/*void readDynamicOffSwitches(FILE* bdata) ;
	void readDynamicOffSwitches(string fn) ;
	void writeDynamicOffSwitchesToFile(string fn)  ;
	string getDynamicSwitchesAsString();
	void setDynamicSwitchesFilename(string fn);
	string getDynamicSwitchesFilename();*/

	// SWITCH:
	// read input flows from separate file at monthly timesteps,
	// or from Catchment Fuile (EDIC version) ?
	inline void setDYNAMIC_OFF_INFLOWS(bool y){	globalMpmasSettings->setDYNAMIC_OFF_INFLOWS(y);   }
	inline int DYNAMIC_OFF_INFLOWS() { return globalMpmasSettings->DYNAMIC_OFF_INFLOWS();}

	inline bool SUPPRESS_COUPLING() {	return globalMpmasSettings->SUPPRESS_COUPLING();}
	inline void SUPPRESS_COUPLING_setTrue(){	globalMpmasSettings->SUPPRESS_COUPLING_setTrue();   }
	inline void SUPPRESS_COUPLING_setFalse(){	globalMpmasSettings->SUPPRESS_COUPLING_setFalse();   }

	inline int get_NUMBER_SPIN_UP_ROUNDS(){ return globalMpmasSettings->get_NUMBER_SPIN_UP_ROUNDS();}
	inline void set_NUMBER_SPIN_UP_ROUNDS(int numSpinUpRounds){	globalMpmasSettings->set_NUMBER_SPIN_UP_ROUNDS(numSpinUpRounds);   }
	inline void set_DYNAMICS_OFF_ALL(int flag){	globalMpmasSettings->set_DYNAMICS_OFF_ALL(flag);   }

	//Special debug info for Milp
	inline double MIN_OBJV(){	return globalMpmasSettings->MIN_OBJV();}      //minimum objective value
	inline int SAVE_MILP(int IdOfMilp){	return globalMpmasSettings->SAVE_MILP(IdOfMilp);}//returns 'true' if MILP has to be saved
	inline int SAVE_AGENT(int farmsteadID){	return globalMpmasSettings->SAVE_AGENT(farmsteadID);}//returns 'true' if agent data have to be saved

	//write only non-zero values to file
	inline bool WRITE_MPX() {	return globalMpmasSettings->WRITE_MPX() ;}
	//save all agent MIPs to file
	inline bool SAVE_ALL_MTX() {	return globalMpmasSettings->SAVE_ALL_MTX() ;}


	//Back- and whitelists of farm agents
	inline void SET_BLACK_WHITE_LIST(int readLists){	globalMpmasSettings->SET_BLACK_WHITE_LIST(readLists) ;}//true = read lists
	inline int NUM_BLACKLIST_AGENTF(){	return globalMpmasSettings->NUM_BLACKLIST_AGENTF() ;}//returns number of blacklisted farm agents
	inline int BLACKLISTED_AGENTF(int farmsteadID){	return globalMpmasSettings->BLACKLISTED_AGENTF(farmsteadID) ;}//returns true if farm agent is blacklisted

	inline int NUM_WHITELIST_AGENTF(){	return globalMpmasSettings->NUM_WHITELIST_AGENTF() ;}//returns number of whitelisted farm agents
	inline int WHITELISTED_AGENTF(int farmsteadID){	return globalMpmasSettings->WHITELISTED_AGENTF(farmsteadID) ;}//returns true if farm agent is whitelisted

	//Fine-tuning of OSL Solver
	inline int OSL_TUNE() {	return globalMpmasSettings->OSL_TUNE() ;}         //fine tuning (yes/no)
	inline double MAX_TIME() {	return globalMpmasSettings->MAX_TIME() ;}     //maximum time allowed for solving
	inline void SET_MAX_TIME(double max_time_) {	globalMpmasSettings->SET_MAX_TIME(max_time_) ;}//set maximum time
	inline int MAX_ITER() {	return globalMpmasSettings->MAX_ITER() ;}         //maximum iterations
	inline int MAX_NODE() {	return globalMpmasSettings->MAX_NODE() ;}         //maximum nodes
	inline int SLV_SEQUENCE() {	return globalMpmasSettings->SLV_SEQUENCE() ;}
	inline void set_SLV_SEQUENCE(int flag) {	 globalMpmasSettings->set_SLV_SEQUENCE(flag) ;}
	inline void SET_OSL_TUNE(bool osl_tune_) {	 globalMpmasSettings->SET_OSL_TUNE(osl_tune_) ;}
	inline void SET_MAX_ITER(int max_iter_) {	 globalMpmasSettings->SET_MAX_ITER(max_iter_) ;}
	inline void SET_MAX_NODE(int max_node_) {	 globalMpmasSettings->SET_MAX_NODE(max_node_) ;}

	inline MpmasOslModeSetting * get_SettingsForMode(int mode) 	{	return globalMpmasSettings->get_SettingsForMode(mode) ;}
	inline void set_ModeMaxNodes (int mode, int max_nodes_) 		{	 globalMpmasSettings->set_ModeMaxNodes(mode, max_nodes_) ;}
	inline void set_ModeMaxSolutions (int mode, int max_solutions_){	 globalMpmasSettings->set_ModeMaxSolutions(mode, max_solutions_) ;}
	inline bool specific_SettingsForMode (int mode) {	return globalMpmasSettings->specific_SettingsForMode(mode) ;}
	// Troost **********end block insert**********

	// ... can be overwritten by values passed as input arguments
	inline double MAX_FABS()						 {	return globalMpmasSettings->MAX_FABS() ;} 		//maximum absolute floating point
	inline void SET_MAX_FABS(double mfabs)	 {	globalMpmasSettings->SET_MAX_FABS(mfabs) ;} 	//set maximum absolute floating point
	inline int DIGITS_ROUND1()					 {	return globalMpmasSettings->DIGITS_ROUND1() ;} 		//number of digits used for round1 "NK1"
	inline void SET_DIGITS_ROUND1(int digits){	 globalMpmasSettings->SET_DIGITS_ROUND1(digits) ;}

	inline void set_EPSILON_FIXCOLS(double d){	 globalMpmasSettings->set_EPSILON_FIXCOLS(d) ;}
	inline double EPSILON_FIXCOLS()			{	return globalMpmasSettings->EPSILON_FIXCOLS() ;}
	///////////////////////////////////////////////////////////////
	// GENERAL FUNCTIONS
	// ===========================================================

	inline int NUM_VARIABLES(){	return globalMpmasSettings->NUM_VARIABLES() ;}
	inline void setNUM_VARIABLES(int numV){	 globalMpmasSettings->setNUM_VARIABLES(numV) ;}
	inline void returnFscan(int retVal, char* strVar) {	 globalMpmasSettings->returnFscan(retVal, strVar) ;}

	inline enumCoupling getCoupling(){	return globalMpmasSettings->getCoupling() ;}
	inline void setCoupling(int intCommand){	 globalMpmasSettings->setCoupling(intCommand) ;}

	// Keep grid in RAM
	inline bool SPATIAL_ON(){	return globalMpmasSettings->SPATIAL_ON() ;}
	inline void set_SPATIAL_ON(bool b){	 globalMpmasSettings->set_SPATIAL_ON(b) ;}

	// Also allocate land use to plots
	inline bool SPATIALLY_ALLOCATE_ON(){	return globalMpmasSettings->SPATIALLY_ALLOCATE_ON() ;}
	inline void set_SPATIALLY_ALLOCATE_ON(bool b){	 globalMpmasSettings->set_SPATIALLY_ALLOCATE_ON(b) ;}

	// Round crop activities in solution vector
	inline int SPATIAL_ALLOCATION_CORRECTED(){	return globalMpmasSettings->SPATIAL_ALLOCATION_CORRECTED() ;}
	inline void set_SPATIAL_ALLOCATION_CORRECTED(int i){	 globalMpmasSettings->set_SPATIAL_ALLOCATION_CORRECTED(i) ;}


	//
	inline bool TestFun(int testNr){	return globalMpmasSettings->TestFun(testNr) ;}
	inline bool TestFunDestr(int testNr){	return globalMpmasSettings->TestFun(testNr) ;}
	inline int TestFun_getMax(){	return globalMpmasSettings->TestFun_getMax() ;}
	inline void TestFun_setFalse(int testNr){	globalMpmasSettings->TestFun_setFalse(testNr) ;}
	inline void TestFun_setTrue(int testNr){	globalMpmasSettings->TestFun_setTrue(testNr) ;}


	inline bool SpatialExport(int v_){	return globalMpmasSettings->SpatialExport(v_) ;}
	inline int SpatialExport_getMaxCont(){	return globalMpmasSettings->SpatialExport_getMaxCont() ;}
	inline void SpatialExport_setFalse(int v_){	globalMpmasSettings->SpatialExport_setFalse(v_) ;}
	inline void SpatialExport_setTrue(int v_){	globalMpmasSettings->SpatialExport_setTrue(v_) ;}
	inline void SpatialExport_printAllActive(){	 globalMpmasSettings->SpatialExport_printAllActive() ;}

	// Description of spatial info / content type
	inline bool SpatialExportIsAtSectorLevel(Content cont){	return globalMpmasSettings->SpatialExportIsAtSectorLevel(cont) ;}
	inline bool SpatialExportIsMonthly(Content cont){	return globalMpmasSettings->SpatialExportIsMonthly(cont) ;}
	inline bool SpatialExportRequiresAgentToLandscape(Content cont){	return globalMpmasSettings->SpatialExportRequiresAgentToLandscape(cont) ;}
	inline bool SpatialExportIsPossible(Content cont){	return globalMpmasSettings->SpatialExportIsPossible(cont) ;} //whether spatial export of this content is possible given the current model configuration
	inline bool SpatialExportIsAvailable(Content cont){	return globalMpmasSettings->SpatialExportIsAvailable(cont) ;} //whether spatial export of this content is generally available
	inline void SpatialExportPrintToScreen(){	globalMpmasSettings->SpatialExportPrintToScreen() ;}
	inline void SpatialExportPrintToScreen(Content cont){	globalMpmasSettings->SpatialExportPrintToScreen(cont) ;}


	//void set_OUTDIR(string outStr);
	inline string OUTDIR() 			{	return globalMpmasSettings->OUTDIR();}
	inline string SIMNAME() 			{	return globalMpmasSettings->SIMNAME();}
	inline string XMLDIR()			{	return globalMpmasSettings->XMLDIR();}
	inline string TEMPDIR()			{	return globalMpmasSettings->TEMPDIR();}
	inline string INDIR()				{	return globalMpmasSettings->INDIR();}
	inline string MASINPUTDIR() 	{	return globalMpmasSettings->MASINPUTDIR();}
	inline string MASOUTPUTDIR()	{	return globalMpmasSettings->MASOUTPUTDIR();}
	inline string OUTGIS() 			{	return globalMpmasSettings->OUTGIS();}

	inline void set_SIMNAME(string sn) 			{	globalMpmasSettings->set_SIMNAME(sn);}

	inline string SECTOR_MAP_NAME()			{	return globalMpmasSettings->SECTOR_MAP_NAME();}
	inline string GIS_FOLDER_NAME()			{	return globalMpmasSettings->GIS_FOLDER_NAME();}
	inline string PATH_ACT_PRECIPITATION()	{	return globalMpmasSettings->PATH_ACT_PRECIPITATION();}
	inline string PATH_POT_EVAPOTRANSPIRATION()	{	return globalMpmasSettings->PATH_POT_EVAPOTRANSPIRATION();}
	inline string PATH_ACT_PRICES()			{	return globalMpmasSettings->PATH_ACT_PRICES();}
	inline string ADD_OUTPUT_NAME()			{	return globalMpmasSettings->ADD_OUTPUT_NAME();}



	//For debugging maps when using TDT
	inline int GET_mapsForExternalModel() {return globalMpmasSettings->GET_mapsForExternalModel();}
	//void SET_mapsForExternalModel(int flag);

	// Request return (in Test-Files)
	inline void getReturnIf99(){globalMpmasSettings->getReturnIf99();}
	inline void getReturnIf99(int i){globalMpmasSettings->getReturnIf99(i);}

	// Content
	inline string getCONTSTR(Content contIn){return globalMpmasSettings->getCONTSTR(contIn);}
	inline void printContString(Content cont){ globalMpmasSettings->printContString(cont);}
	inline string returnContString(Content cont){return globalMpmasSettings->returnContString(cont);}

	// ONLY RENAMED FROM ABOVE
	inline string cont2string(Content cont){return globalMpmasSettings->cont2string(cont);}
	inline string exptp2string(ExportType tp){return globalMpmasSettings->exptp2string(tp);}

	inline string getFullMpmasFilename(	EnumFileType fileTypeEnum_,Content cont,
			int CatchmentID = -1, int year = -1, int month = -1, int agType = -1, int marketID = -1, int userContentID = -1)
	{
			return globalMpmasSettings->getFullMpmasFilename(fileTypeEnum_, cont,CatchmentID, year, month, agType, marketID, userContentID);
	}

	inline string getTestFilename(string s, int CatchmentID){	return globalMpmasSettings->getTestFilename(s, CatchmentID);}

	inline int SEED()								{	return globalMpmasSettings->SEED() ;	}
	inline void setSEED(int seed_)				{	globalMpmasSettings->setSEED(seed_);	}

	// For controlling number of model runs with flag Y
	inline int STOPYEAR()							{	return globalMpmasSettings->STOPYEAR() ;		}
	inline void setSTOPYEAR(int stopyear_)	{	globalMpmasSettings->setSTOPYEAR(stopyear_);	}

	inline long IDUM()								{	return globalMpmasSettings->IDUM() ;		}
	inline void setIDUM(long idum_)			{	globalMpmasSettings->setIDUM(idum_);	}

	inline int MAXPERIODIN()								{	return globalMpmasSettings->MAXPERIODIN() ;		}
	inline void setMAXPERIODIN(int maxPeriodIn_)	{	globalMpmasSettings->setMAXPERIODIN(maxPeriodIn_);	}

	inline string getTDTConfigFilename(TypeDataExchangeChannel c,  TypeDataExchangeType t) {	return globalMpmasSettings->getTDTConfigFilename(c, t) ;		}
	inline string filenameConfigFile(TypeRoleInCoupling roleInCoupling)							{	return globalMpmasSettings->filenameConfigFile(roleInCoupling) ;		}

	inline void printAllIrrigationMethods() 								{	globalMpmasSettings->printAllIrrigationMethods();	}
	inline string getStringIrrigationMethods(typeIrrigationMethod t)	{	return globalMpmasSettings->getStringIrrigationMethods(t) ;		}

	// --------------------------------------------
	// ... Canal model
	inline void setCANAL_MODEL(int _canalModelType)		{	globalMpmasSettings->setCANAL_MODEL(_canalModelType);	}
	inline int CANAL_MODEL() 									{	return globalMpmasSettings->CANAL_MODEL() ;		}


	// Used for mode 0,3,4
	inline void setCANAL_BETA_SHARE(double _betaShare)			{	globalMpmasSettings->setCANAL_BETA_SHARE(_betaShare);}
	inline double getCANAL_BETA_SHARE()								{	return globalMpmasSettings->getCANAL_BETA_SHARE() ;		}
	// Used in Mode 1, 3, 4
	inline void setCANAL_LOSSFACTOR(double _canalLosses)		{	globalMpmasSettings->setCANAL_LOSSFACTOR(_canalLosses);}
	inline double getCANAL_LOSSFACTOR()								{	return globalMpmasSettings->getCANAL_LOSSFACTOR() ;		}
	// Used in Mode 1, 3, 4
	inline void setCANAL_LAMBDA(double _canalLambda)				{	globalMpmasSettings->setCANAL_LAMBDA(_canalLambda);}
	inline double getCANAL_LAMBDA()									{	return globalMpmasSettings->getCANAL_LAMBDA() ;		}

	inline void setCanalFilename(string fn)							{	globalMpmasSettings->setCanalFilename(fn);}
	inline string getCanalFilename()									{	return globalMpmasSettings->getCanalFilename() ;		}

	inline double SURPLUS_FACTOR()										{	return globalMpmasSettings->SURPLUS_FACTOR() ;		}
	inline void setSURPLUS_FACTOR(double val)						{	globalMpmasSettings->setSURPLUS_FACTOR(val);}


	inline double LAMBDA(int type)										{	return globalMpmasSettings->LAMBDA(type) ;		} //Parameter for expectations (Brandes et al., 1997: 382)
	inline void SET_LAMBDA(int type, double value)				{	globalMpmasSettings->SET_LAMBDA(type, value);}

	// For varying lambda: read values from file
	inline string LAMBDA_filename()									{	return globalMpmasSettings->LAMBDA_filename() ;		}
	inline void set_LAMBDA_filename(string s)						{	globalMpmasSettings->set_LAMBDA_filename(s);}

	//Special file to deactivate sectors
	inline string getInactiveSectorsFilename()						{	return globalMpmasSettings->getInactiveSectorsFilename() ;		}
	inline void setInactiveSectorsFilename(string fn)			{	globalMpmasSettings->setInactiveSectorsFilename(fn);}

	inline double IRRIGFACTOR(enumIrrigationFactorType e) { return globalMpmasSettings->IRRIGFACTOR(e); }
#ifdef RNGSTREAM
	inline RngStream* getPtrToRandomGenerator_Households() {return globalMpmasSettings->getPtrToRandomGenerator_Households(); }
	inline RngStream* getPtrToRandomGenerator_Farms() {return globalMpmasSettings->getPtrToRandomGenerator_Farms(); }
	inline int getRngHouseholdMembersFstIdShifter() { return globalMpmasSettings->getRngHouseholdMembersFstIdShifter();}
#endif

	inline int EXTRAGAP() {return globalMpmasSettings->EXTRAGAP();}
	inline string 	 get_externalScriptCall_PreInvest() { return globalMpmasSettings->get_externalScriptCall_PreInvest() ;}
	inline string 	 get_externalScriptCall_CropModel() { return globalMpmasSettings->get_externalScriptCall_CropModel() ;}
	inline string 	 get_externalScriptCall_InYear() { return globalMpmasSettings->get_externalScriptCall_InYear() ;}
	inline vector<int> 	 get_externalScript_InYear_ChangedMaps() { return globalMpmasSettings->get_externalScript_InYear_ChangedMaps() ;}
#ifdef MULTIPERIOD
	inline size_t get_NumberExtraAgentCharacteristics() {return globalMpmasSettings->get_NumberExtraAgentCharacteristics();}
	inline int get_extraAgentCharacteristicsIdForString(string s) {return globalMpmasSettings->get_extraAgentCharacteristicsIdForString(s) ;}
	inline string get_extraAgentCharacteristicsStringForId(size_t i){return globalMpmasSettings->get_extraAgentCharacteristicsStringForId(i);}
#endif
	inline int yearsToSkipExpectationUpdating() { return globalMpmasSettings->yearsToSkipExpectationUpdating();}
	inline void setYearsToSkipExpectationUpdating(int n) { globalMpmasSettings->setYearsToSkipExpectationUpdating(n);}

}	//end mpmasScope
#endif //ODB
#endif
