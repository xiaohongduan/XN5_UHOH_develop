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
//  File:       CatchmentSector.h (formerly sector9.h)
//
//  Contents:   Should develop into a new water user association class that
//              provides irrigation infrastructure to its member farm agents,
//              assigns water quotas and distributes irrigation water
//
//------------------------------------------------------------------------------


#ifndef _CatchmentSector_h
#define _CatchmentSector_h

// Libraries C++
#include <iostream>
using namespace std; // fuer C++-Compiler, damit cin nicht als std::cin eingelesen werden muss
#include <fstream>  
#include <string>

#include "BasicEnums.h"

#include "CatchmentHandler.h"
//#include "GridCellHandler.h"
#include "MpmasLandscape.h"

#include "RandomDataHandler.h"
#include "Raster2D.h"

class CropPrecipitation;
class CropWater;
class IrrigationMethodSector;
class TimeHandler;

class sector
{	
	protected: 
	int* zuflussID;       //Nummern der Sektorzufluesse
		
	protected:
	string secName; //Name der Gitternetzdatei (Input)
	int sc_nr; //Nr. der Subcuenca
	int GISID;//Value in arcGIS-map for sector
	// This ID is now relevant to identify a sector within a Subcatchment map.
	
	int sectorSeed;
	
	scuenca* sub; //Zeiger auf die Subcuenca
	int sectorIndex;//sector ID starts with "0", index within catchment
	// sub[sc_nr]->
	// sek[sc_nr][sectorIndex]->
	// BUT:
	// sub[sc_nr]->getRoutingData(positionInRegionFile);
	//
	int positionInRegionFile;// Position that this sector had in input file;
	// corresponds to row and column in BUCKET-model files
	
	double irrigableHa_j;	//irrigable area of sector
	double nonIrrigableHa_j;//non-irrigable area
	int iniAgents; //Initial number of agents
	int numInflows; //Number of water inflows

	// Used for exporting sector maps to ASCII grid
	double xCoord;
	double yCoord;

	// Troost 20130219 Improve ExistingRentalsContracts
	// Troost **********begin block insert**********
	int xShiftToCatchMap;
	int yShiftToCatchMap;
	// Troost **********end block insert**********

	int* plotsPerType; //Anzahl der Parzellen je Bodentyp

	double* totalSectorShares; //total shares of all users within sector, formerly <watershares>
	double* maxQuantInflow; //Maximum amount of inflows into sector

	//Pointer to crop-water module;
	CropWater* p_cropWat;
	
	//Matrix of actual rainfall, potential ETr and flood factors in all months
	CropPrecipitation* p_cropActualPrecipitation;
	CropPotentialETr* p_cropPotentialEvapotranspiration;
	CropFloodFactor* p_cropFloodFactor;

	//Matrix of irrigation water demands in all months
	MatrixDouble irrigationWaterDemands;//(numWaterActivites * MONATE)
	
	//Matrix of crop effective precipitation
	MatrixDouble effectivePrecipitation;//(numWaterActivites * MONATE)

	//Matrix of crop potential evapotranspiration
	MatrixDouble potentialEvapotranspiration;//(numWaterActivites * MONATE)

	//Vector of flood factors
	MatrixDouble floodFactor;//(1 * MONATE)

	//Matrix with all different irrigation methods, for irrigated area (ha) 
	//under certain irrigation method, at current time step
	MatrixDouble methoden_j;//(1 * irrigMethodEND)

	//share of irrigation methods in irrigated area at sector level
	MatrixDouble areaShareOfIrrigationMethods;//(1 * irrigMethodEND)

	// Inherited class from Catchment data IrrigationMethod
	// Corrected: Chgannel efficiency
	IrrigationMethodSector sectorparamsIrrigationMethod;

	// Efficiency of channel system of sector, as read from file.
   // ... Actual realization will be determinated according to CANAL_MODEL()
	double canalEfficiency;
   // ... additional variables for canal efficiency, depending on model
   
	//yearly vector, contains monthly entries
	MatrixDouble N_j; // monthly net inflow from water sources (1 X MONATE)
	
	//monthly
	double Ext_j; //Inflows from other sectors ("Sektor-Ext-Water")
	double U_j; //sectoral Re-Use-coefficient

	double B_j; //Coef. de derrames
	double G_j; //Coef. de escurrimientos
	double L_j; //Coef. de escurrimientos
	double eta_j; //Coef. de escurrimientos

	double b_j; //Coef. de aprovechamiento
	double g_j; //Coef. de aprovechamiento
	double loss_j; //Coef. de aprovechamiento
	double zeta_j;

	//monthly
	double D_sat_j; //sectorale D'
	double Derr_j; // superficial discharges ("Derrames")
	double Esc_j; // Topsoil discharges (Escurrimientos)
	double Ueb_j; // Sektor-Ueberschusswasser

   double Canal_j; // Additional Sektor-water from inefficient canals, which is
                   // re-distributed to all agents proportional to land ownership
   double S_j;     // Total water available to agents ... even if not used.
                   // NOT USABLE FOR BALANCES, because re-distributed!
   double D_j;     // Total water demand

	//Extra currently not used. Initialized with 0.0 so it does not produce any errors!
	//double Extra;// sum of excess water of an agent
	double weff;   //increase of water-use efficiency (1.0 = no increase)

	//Gitternetz des Sektors
	ldscape fl;

   //Vector with values for each content type. Filled during coupling.
   // ... call with "getContentFromSectorLevel( Content cont)"
   MatrixDouble couplVector;



public:
	// --------------------------------------------------------------------------
	///         Constructor - Destructor
	// --------------------------------------------------------------------------

	//Uebergabe: Files, Nr. und Zeiger der Subcuenca, Sektornr., Sektorflaeche.
	//Initialisierungsliste: Sektor- und SubcatchmentNum wird gleich an den
	//Pachtmarkt-Konstruktor weitergereicht
	sector (const char* in, const char* _sectorName, int sc, scuenca* s, int sk, 
		CropWater* p_cropWat_,
		CropPrecipitation* p_cropPrecipitation_, 
		CropPotentialETr* p_cropPotentialETr_,
		CropFloodFactor* p_cropFloodFactor_) ;
	virtual ~sector();

	virtual void sectorbetriebe_loeschen();//loescht Sektorbetriebe aus Liste
	virtual void speicher_freigeben(void);


	// --------------------------------------------------------------------------
	///         Initializing all those things...
	// --------------------------------------------------------------------------

	// READ IFNORMATION ABOUT SECTOR:
	//      Inflow,              --> ZUF*
	//      Lottery data,        -->
	//      Nr of agents         --> iniAgents
	//      size of grid         --> not read here, but in GIS-maps
	// assign it to sector-variables
	virtual void initializeInflowsToSector(MatrixDouble& INFL, MatrixDouble& MAXINFL);
	virtual void assignInflowIDs(const MatrixDouble& inflows);
	virtual void initLandscape_SectorRaster(Content cont, Raster2D& rasterCont);
// Troost 20140527 Generalize dynamic NRUs
// Troost **********begin block changed**********
	//virtual void fillLandscapeDataContent(Content cont, Raster2D& rasterCont);
	virtual void fillLandscapeDataContent(Content cont, Raster2D& rasterCont, int userContentId = -1);
// Troost **********end block changed**********

	void setActualPrecipitationPointer(CropPrecipitation* p_cropPrecipitation_)
	{	p_cropActualPrecipitation = p_cropPrecipitation_;
	}
	
	void setPotentialEvapotranspirationPointer(CropPotentialETr* p_cropPotentialETr_)
	{	p_cropPotentialEvapotranspiration = p_cropPotentialETr_;
	}

	void setFloodFactorPointer(CropFloodFactor* p_cropFloodFactor_)
	{	p_cropFloodFactor = p_cropFloodFactor_;
	}

	void setCropWatPointer( CropWater* p_cropWat_)
	{	p_cropWat = p_cropWat_;
	}
   
	CropWater* getCropWatPointer()
	{	return p_cropWat;
	}

	//virtual void makeAgentVector(); ## Tbe 110423 eliminate?
	//virtual int checkAndInitializeAgentIds(Raster2D& rasterCont);
	virtual void checkPropertiesAndInitializeToSector(Raster2D& rasterCont);
	//virtual void analyseAgentsInSectorAndExport(ofstream& streamData);// ## Tbe 110920 not in use

	void incrementPlotsPerType(int type);
	// --------------------------------------------------------------------------
	///         Map functions (in landscape)
	// --------------------------------------------------------------------------
	///Update land owners and land users
	virtual void besitzer_eintragen(int z, int s, int nr);
	virtual void updateCellUserMapID(int z, int s, int userID);
	virtual void updateCellMapColor(int z, int s, int mapColor);
	virtual void updateUserInMap(int z, int s, int userID, int mapColor, double dist); //141002 Troost, to make sure everything is updated
	virtual void alle_nutzer_zuruecksetzen();

	virtual void updateParcelGroups();
	virtual vector<int>findContentAroundCell(int sr, int sc , int distance, Content cont );
	// --------------------------------------------------------------------------
	///         PRINT - WRITE functions (partly markets, partly landscape, partly TSPC)
	// --------------------------------------------------------------------------
	virtual void printOnScreen_MatrixSize(void); // Info about function
	virtual void printToScreen(void);
	virtual void printToScreenOld(void);
	virtual void printIrrgationMethods();

   virtual void writeIrrigationMethodToFile(string fn, bool isFirst);

	/// Writes monthly water information into two streams 
	virtual void writeMonthlyWaterToStream(int m); 
	virtual void writeMonthlyWaterToStream(int m, double Ueb_j_tmp);

	virtual void bodenkarte_printOnScreen(void);
	virtual void savePlotUserMap(char*);

	// --------------------------------------------------------------------------
	///         GET / SET
	// --------------------------------------------------------------------------

	// set functions
	/* TODO 1 -oArnold -cImplement : propbably discard these functions, or migrate them to other class?*/
	virtual void set_iniAgents(int numA)  			{  iniAgents = numA; }
	virtual void setIrrigableHa_j(double haSz)	{  irrigableHa_j = haSz;  }
	virtual void setNonIrrigableHa_j(double haSz){  nonIrrigableHa_j = haSz;  }

	virtual void setSectorSeed(int _sectorSeed); // Function copied from BasicData::setSEED()
	virtual int getSectorSeed() {  return sectorSeed;  }   
	
	// Coordinates of sector
	virtual double getXcoord() { return xCoord;}
	virtual double getYcoord() { return yCoord;}
	virtual void setXcoord(double xc);
	virtual void setYcoord(double yc);

	virtual int getLscapeRows();
	virtual int getLscapeCols();

// Troost 20130219 Improve ExistingRentalsContracts
// Troost **********begin block insert**********
	virtual void setYShiftToCatchMap(int yShift);
	virtual void setXShiftToCatchMap(int xShift);
	virtual int transformXCoordCatchToSector(int x);
	virtual int transformYCoordCatchToSector(int y);
	virtual int transformXCoordSectorToCatch(int x);
	virtual int transformYCoordSectorToCatch(int y);
// Troost **********end block insert**********

	// get functions
	virtual int getSectorID(void)		{return sectorIndex;}
	virtual int getCatchmentID(void)	{return sc_nr;}

	virtual int getGISID(void)			{return GISID;}
	virtual int getGisId(void)			{return GISID;}
	
	virtual int getIniAgents()			{return iniAgents;}
	virtual double getIrrigableHa(void){return irrigableHa_j;}
	virtual double getNonIrrigableHa(void){return nonIrrigableHa_j;}

	virtual int getInflowIdForIndex(int index)//index refers to row index in EdicRiverFlowsX.dat
	{	return zuflussID[index];
	}
	virtual double getWaterShares(int index)//index refers to row index in EdicRiverFlowsX.dat
	{	return totalSectorShares[index];
	}
	
	virtual string getInflowStatusAsString();
	
	virtual IrrigationMethodSector* const getPointerToIrrigationMethodSector()
	{	return &sectorparamsIrrigationMethod;
	}

	virtual const MatrixDouble* getPtrToIrrigationWaterDemands() {return &irrigationWaterDemands;}
	virtual const MatrixDouble* getPtrToEffectivePrecipitation(){return &effectivePrecipitation;}
	virtual const MatrixDouble* getPtrToPotentialEvapotranspiration(){return &potentialEvapotranspiration;}
	virtual const MatrixDouble* getPtrToFloodFactor(){return &floodFactor;}
	
	virtual double get_N_j(int m){ return N_j.wert_holen(0, m); }

	virtual int get_plotsPerType(int typ);
	
	// set functions
	virtual void set_GISID(int GISID_){  GISID = GISID_;};
	virtual void set_GisId(int GISID_){  GISID = GISID_;};

	// not yet used...
	virtual void set_positionInRegionFile(int posInRegionFile){ positionInRegionFile  =posInRegionFile;} 
	


	// --------------------------------------------------------------------------
	///         CREATE AGENTS
	// --------------------------------------------------------------------------
	//teilt einzelnen Betrieben den Standort der Hofstelle und der Flaechen zu
	virtual void createAgentsInSector();
	//virtual void assignGridCells();
	void assignGridCellsAndInitializeParcelGroups();
	
	// --------------------------------------------------------------------------
	///         EDIC calculations
	// --------------------------------------------------------------------------

	// No local time handler needed. Internally, global time is used
	virtual double getAveragePrecipitation_Global();// ## Tbe 111202 not in use

	// Ongoing ("current") time from time handler that was passed is used
	virtual double getActualPrecipitation(TimeHandler& th_notChanged, string s);
	virtual double getPotentialEvapotranspiration(TimeHandler& th_notChanged, string s);
	virtual double getFloodFactor(TimeHandler& th_notChanged, string s);

   virtual double getAveragePrecipitation(TimeHandler& th_notChanged);
	virtual double getAveragePotentialEvapotranspiration(TimeHandler& th_notChanged);
	virtual double getAverageFloodFactor(TimeHandler& th_notChanged);

	/// book-keeping of area under various irrigation methods
	virtual void countInflows(void);
	virtual void addToIrrigationMethod_AreaShare(typeIrrigationMethod type, double val);
	virtual void computeAreaShareOfIrrigationMethods();

	///compute irrigation water supply, return demand actually met
	virtual double computeAgentsIrrigationWaterSupplyPerMonth (int m, double irrHa, 
      double* D_i, double* N_i, double*& Ext_i, double*& S_i, double* WPa_i, double Ueb_j_tmp, double*& L_i);
	
	virtual void addAgentValueToN_j(int m, double val); //summiert den Wert "val" ZU den monatliche WaterRights
	virtual void setN_jToZero();
	
	virtual void make_N_j(int m);//summiert alle monatliches Netto-Water ## Tbe 110621 not in use

	//	virtual void addToExtra(double);  //summiert nicht genutztes Wasser, Renamed from <make_extra>
	virtual void add_Ueb_j(double);	 //summiert nicht genutztes Wasser
   virtual double get_Ueb_j();
   virtual void reduce_Ueb_j(double);
   virtual void reset_Ueb_j();
   
	virtual void addToSectorsTotalSatisfiedDemand_D_sat(double);  //summiert betriebliche D'


	//Elementfunktionen zur Ermittlung der entsprechenden Sektorwerte
	virtual void setEta(double eta_);
	virtual double getEta() {return eta_j;}
	
	virtual void compute_B_G();
	virtual double compute_delta();   // sum of all outflow coefficients
	virtual double compute_e();       // sum of all outflow coefficients
	virtual double compute_fieldLoss(); // after (!) B, G was calculated
	virtual double compute_sectorLoss();
	virtual double compute_e_jj();       // sum of all outflow coefficients	

	// Returning value for output
	virtual double computeInternalReturnFlowCoefficients();// renamed from <U_j_berechnen>
	virtual double receiveExternalReturnFlowsFromUpstreamSectors();// renamed from <compute_returnFlowsEdic>
	virtual void computeSurfaceRunoff();//renamed from <Derr_j_berechnen>
	virtual void computeLateralFlows();//renamed from <Esc_j_berechnen>
	virtual void sendReturnFlowsToDownstreamSectors();//renamed from <Abf_j_berechnen>

	// compute start values based on average rainfall
	virtual void initializeStartValuesForEffectivePrecipitation(TimeHandler& th_notChanged);
	virtual void initializeStartValuesForPotentialEvapotranspiration(TimeHandler& th_notChanged);
	virtual void initializeEffectivePrecipitation(TimeHandler& th_notChanged);
	virtual void initializePotentialEvapotranspiration(TimeHandler& th_notChanged);

	// sector-specific flood factor
	virtual void initializeFloodFactor(TimeHandler& th_notChanged);

	// Compute per-hectare irrigation demands for all irrigation activities
   //  ... Renamed from "initializeIrrigationWaterDemands()" with call to global time handle.
   //  ... Now calls local time function, using global time handler
	virtual void initializeIrrigationWaterDemands(TimeHandler& th_fixed);

	// Reset monthly sector values
	virtual void initializeEdicMonth();
   // ... resets edic storages   Ueb_j, D_sat_j, Canal_j, Dj
   // ... (also called from <initializeEdicMonth>)
   virtual void resetEdicSectorStorages();

	// copy values from monthly vector to yearly matrix
	virtual void copyEffectivePrecipitationInMonth(int m, const MatrixDouble& vectorOfMonthlyCropEffectivePrecipitation);
	virtual void copyPotentialEvapotranspirationInMonth(int m, const MatrixDouble& vectorOfMonthlyCropPotentialEtr);
	virtual void copyIrrigationWaterDemandsInMonth(int m, const MatrixDouble& vectorOfMonthlyCropIrrigationDemands);

   // for output functions only 
   virtual double getEffectivePrecipitation(int waterActiID_, int month_);
   virtual double getPotentialEvapotranspiration(int waterActiID_, int month_);
   virtual double getPlantIrrigationDemand(int waterActiID_, int month_);

	// ---------------------------------
	///         EDIC get / set / output
	// ---------------------------------
   
	virtual int    get_numInflows(void);  //returns number of inflows actually initialized
	virtual double getU_j();
	virtual double getWeff();
	virtual string getName();	
	virtual double getNetInflowFromSector(int inflowID, int month, double share);
	virtual double getCanalEfficiency() {return canalEfficiency;}
	virtual void setCanalEfficiency (double canalEfficiency_);

	virtual void set_b_j(double _b);
	virtual void set_g_j(double _g);
	virtual double getZeta_j(){return zeta_j;};
   
	virtual void setEsc(double val);
	virtual void setDerr(double val);
	void set_U_j(double U_j_)		{  U_j = U_j_;}
	void set_weff(double weff_)	{ weff = weff_; }
	
	//used if no external inflows are modelled (WASIM)
	virtual void set_Ext_j(double ext_j_);
   virtual double get_Ext_j(void);

	//computes agent return flows, share depending on agent's irrigation area
	virtual double computeReturnFlowsForAgent_Ext_i(int m, double agentArea);
   virtual double computeReturnFlowsForAgent_Ext_i(int m, double agentArea, double sectorLeftOverWater, double& agentLeftOverWater);

	double get_D_sat_j() {return D_sat_j;};

	void setparamsIrrigationMethod(IrrigationMethod paramsIrrigationMethod);
	virtual double getMethodPlantWaterAvailability(int methodID);
	
	virtual void writeToDescriptiveFile(ofstream& out);

   // Canal model
   virtual void addToCanal_j(double CC_J_);	 //summiert nicht genutztes Wasser
   virtual double get_Canal_j();
   virtual double reduce_Canal_j(double CC_J_);
   virtual void reset_Canal_j();

   // Total water supplied (not usable for balances, because later re-distributed. Double accounting!)
   virtual void addToS_j(double S_J_);
   virtual double get_S_j();
   virtual void reduce_S_j(double S_J_);
   virtual void reset_S_j();

   // And initial water demand
   virtual void addToD_j(double D_J_);
   virtual double get_D_j();
   virtual void reduce_D_j(double D_J_);
   virtual void reset_D_j();

	// --------------------------------------------------------------------------
	///         TSPC
	// --------------------------------------------------------------------------

	//nTSPC model
	virtual double getSlopeLen(int r, int c);
	virtual double getSlopeEro(int r, int c);
	virtual void updateSoilC(int r, int c, soilC*& slc);

	// --------------------------------------------------------------------------
	///         LANDSCAPE HANDLING
	// --------------------------------------------------------------------------

	// --------------------------------
	///         modify content of landscape
	// --------------------------------
	// And for monthly values (REAL month as input expected. Only "cell" does conversion)
	// (here, specific link only in ldscape::updateCellMonthly(int r, int c, int mTot, Content cont, double newVal)) //Arnold2008_0812
	// --------------------------------
	///         landscape - PRINT
	// --------------------------------
	virtual void printOnScreenLdscapeCells(void);
	virtual void printOnScreenAllCells(void);
	virtual void printCell(int row, int col);

	// --------------------------------
	///         retrieve Information from Landscape
	// --------------------------------

	//virtual bool checkConsistencyOfMapsFarmAndProperty();## Tbe 110422 eliminate?
	//virtual bool eraseAllAgentsBellowSize(double factor, double cutoffsize);## Tbe 110422 eliminate?

   // For Ghana case: InflowIDs become Sector!
   //virtual bool rectifySectorBoundariesFromInflows();## Tbe 110422 eliminate?


	// --------------------------------
	///         retrieve information from Landscape
	// --------------------------------
	//Sets dimensions of Raster2D, then calls ldscape::deliverGisStruct
	virtual void getRaster2DfromLandscape( Content cont, Raster2D& raster, int noData); //d
	virtual void getRaster2DfromLandscape( Content cont, Raster2D& raster); //d

   // Retrieve content from any cell[r][c]->getContent(cont) in landscape
   // Note: this function either calls a sector-level value, or a cell-level value, depending on ContentType
   virtual double getSpatialInfo_Content(int r, int c, Content cont);
   virtual double getSpatialInfo_Content_monthly( int r, int c, int m, Content cont);

   // Passes position matrix with columns: row, col. Requests: Third column with value
   virtual void getContentFromCell_monthly(MatrixDouble& mat, int m,  Content cont);//only for wasim coupling 
	virtual void getContentFromCell(MatrixDouble& coordinates, Content cont);

   // For coupling, values are either written at cell level or at sector level.
   // Sector-level value
   // ... Switch is here: 
   virtual double getContentFromSectorLevel( Content cont); //d
	virtual void  setContentToSectorLevel( Content cont, Raster2D& val);
	
   // Cell-level value
   // ... and some monthly values (for now only irrigation quantity)
   virtual double getContentFromCell( int r, int c, Content cont); //d



   virtual double getContentFromCell_monthly( int r, int c, int m,Content cont ); //d
// Troost 20140527 Generalize dynamic NRUs
// Troost **********begin block insert**********
	virtual double getUserDefinedValueinCell(int r, int c, int userContentId);
// Troost **********end block insert**********


	// --------------------------------
	///         modify content of landscape
	// --------------------------------
	// If this function is called, it is not yet determine if content is stored at sector
   // or at cell level! This function though contains a SWITCH statement which does it.
   // Then, either calls "copyContentValueIntoCell" or "copyContentToSector_Double"
   // ... later: implement dynamically, 
   // ... with lookup vector "vecLevelContentStorage[cont]" gives 0 (cell) or 1 (sector)
	virtual void setSpatialInfo_Content(Content cont, Raster2D data_); //d
   
   ///	specific functions, define how to connect "cont" to "landscape.Cell"
   virtual void copyContentRaster2Cell(Content cont, Raster2D& rasterCont); //d RTA 080724
   virtual void copyContentValueIntoCell(int r, int c, Content cont, double value); //d
	virtual void copyContentValueIntoCell_monthly(int r, int c, int month, Content cont, double value); //d

   // If contentType is stored at sector level, then this function is called
   virtual void setSpatialInfo_Content_Double(Content cont, double value); //d

   //Creates files in ASCII-Format, readable by GIS
   // from all varabiles defined within this function

	///	general functions, require that BasicData - getContentString(cont) existiert
	virtual void writeToFileRaster(Content cont);
	virtual void writeToFileRaster_monthly(Content cont, int m);

	void writeN_jToFileWithDims(ofstream& out);	
	///	specific functions, define how to connect "cont" to "landscape.Cell"
   virtual void createRasterFromCell(Content cont, Raster2D& gis);
	virtual void createRasterFromCell(Content cont, Raster2D& gis, bool flag_forceToCell);
	virtual void createRasterFromCell_monthly(Content cont, Raster2D& gis, int m);


// Troost 20160331 Soil type transitions
// Troost **********begin block insert**********
#ifdef MULTIPERIOD
	void updateSoilTypes();
#endif
// Troost **********end block insert**********


};




#endif

