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
//  File:       CatchmentHandler.h (formerly subcatch.h, scuenca.h)
//
//  Contents:   Handles the hydrology-related aspects of the cellular component
//
//
//
//------------------------------------------------------------------------------


//dies ist die header-Datei fuer ein Untereinzugsgebiet "subcuenca"

#ifndef _CatchmentHandler_h
#define _CatchmentHandler_h

//#include "CatchmentSector.h"
#include "CatchmentSectorAuxiliary.h"
#include "MatrixDouble.h"
#include "Raster2D.h"
#include "BasicEnums.h"

//New, for dynamic inflows
// ( see: Landscape/src/master/testDynamicRiverFlows.cpp )
#include"Catchment_DynamicRiverFlows.h"



class CropPrecipitation;
class CropPotentialETr;
class CropFloodFactor;
class CropWater;

class TimeHandler;

class scuenca
{
	public:
   
   
   //---------------------------------------------------------------------------
   ///@name Data query functions
	//@{
   ///         get - set functions
   // --------------------------------------------------------------------------
   virtual void initializeNetInflowsInYear(const MatrixDouble& inflows);
   virtual void rememberTypicalNetInflowsInYear(const MatrixDouble& inflows);
   virtual void reinitWithTypicalNetInflows();
   // see explaination in *.cpp
   virtual void makeForecastForNetInflows(int yr_, double forecastError , string type);   
  	virtual bool setOneInflow(int mTot, int inflowID, double flowVal_m3PerSec);

   virtual string getName();//           {  return name;}
   virtual int getCatchID()           {  return sc_nummer;}
   virtual int getCatchmentID()           {  return sc_nummer;}
   virtual int getSubcatchID() const  {  return sc_nummer;};//returns subcatchment ID
   virtual int get_sectorsInCuenca(void) {return numberOfActualSectors;};


	/// EDIC related query functions
//	virtual double get_b_j(int);//ermittelt b_j fuer einen Sektor
//	virtual double get_g_j(int);//ermittelt g_j fuer einen Sektor
	virtual double get_Ext(int);//ermittelt ext fuer einen Sektor
   virtual double get_monatl_N(int, int);//ermittelt monatl. Wasser fuer Zufluss
   
   virtual void addTo_totalMonthlyIrr(int month_, int secID_, double irrAmountSector_);//ermittelt monatl. Wasser fuer Zufluss
   virtual double get_totalMonthlyIrr(int month_, int secID_);//ermittelt monatl. Wasser fuer Zufluss
   virtual double get_totalMonthlyIrr_sum(int month_);//ermittelt monatl. Wasser fuer Zufluss
   
   // Collects total water demand in all sectors belonging to catchment
   virtual double getGlobalWaterDemand();

	//@}
   //---------------------------------------------------------------------------
   ///@name Initialization functions
	//@{
   void addOneToSectorsInSubcatchment(); // when constructing sectors
   virtual void initRoutingEmpty(void);

   virtual void readSectorMapFromFile(string fn);      



   // Creates raster files in ASCII-Format, readable by GIS
   virtual void readAndExtractSectorMapsFromCatchMaps();      

	// More general, used for coupling. 
   // Here, a switch is used that determines if data is 
   // (a) copied only to sector-level, as mean value; or 
   // (b) copied to cell, with "copyRaster2Cell"
   virtual void copyRaster2Landscape(Raster2D& catchmentMap, Content cont);
   // ... copies to sector, where it is determined if content is stored at cell or at sector level
   virtual void copyRaster2Landscape(Raster2D& catchmentMap, Content cont, bool flag_fixCopyToCell, int udefLayer = -1);


   virtual void makeSectorwiseHistogram(MatrixDouble& histogramBySector, Content cont, int userContentId = -1);
	//@}
	//---------------------------------------------------------------------------

	///@name Other EDIC related functions
	//@{
   // --------------------------------------------------------------------------
   ///         EDIC - initializing
   // --------------------------------------------------------------------------
   void initializeEmpty(void);
   virtual void EDIC_readSurfaceAndSubsurfaceRunoffMatrix();
   virtual void readparamsIrrigationMethodAndAssignToSectors();
   //virtual void initializeCropPointersInSectors(CropPrecipitation* p_cropPrecipitation, CropWater* p_cropWat_); ## Tbe 110608 never used

   // --------------------------------------------------------------------------
   ///         Functions for EDIC model
   // --------------------------------------------------------------------------
	//diese Funktionen machen Eintraege in den jeweiligen Matrizen fuer einzelne
	//Sektoren (Uebergabeparameter: Sektornr., Wert)
   virtual void make_derr(int, double);
	virtual void make_esc(int, double);

   // Resets Edic routing model
   // - Surface return flows
   // - Lateral return flows
   // - irrigation water accounting
   virtual void resetEdicMonthlyReturnFlowData();
   virtual void resetEdicAnnualData();

	double return_delta(int sectorIndex);
	double return_e(int sectorIndex);

   // If dynamic inflows, then these functions pass information through to class DynamicRiverFlows DynamicFlows;
   bool IS_DYNAMIC_INFLOWS() { return isDynamicRiverFlow;};

   // Access
   // Edic:
   double getFlowForRiverAndYearAndMonth_preknown(int riverID_, int y_, int m_, bool isNorthern_);
   // Wasim:
   double getFlowForRiverAndYearAndMonth_exchange(int riverID_, int y_, int m_, bool isNorthern_);


   virtual void setFlowForRiverAndYearAndMonth(int riverID, int yearID, int modelMonth, int valFlowM3);
   virtual void setRiver_fullYear(int riverID, int yearID, MatrixDouble& vectorThisYearFlows);

   // Update catchment-N by copying monthly inflows from catchment-DynamicRiverFlows
   // ... This is done in the beginning of each month,
   // ... if there is true == sub[sc]->IS_DYNAMIC_INFLOWS()
   virtual void updateAllInflowsForCurrentMonth_preknown(TimeHandler& timeHandle_const);
   virtual void updateAllInflowsForCurrentMonth_exchange(TimeHandler& timeHandle_const, MatrixDouble& mat_InflowMedian);
   // passes ID to region
   virtual void set_N_IDs(MatrixDouble& inflows);

   // read and write
   virtual void read_DynamicRiverFlows_preknown(string fn);
   virtual void writeDynamicInflowsToFile_preknown(string fn);

	virtual void writeSectoralWaterInfoToFile(TimeHandler& timeHandle_const, string fn_append);
	virtual void openFile(string fn, bool isFirst, ofstream& out);

	void checkDimensionConsistencyOfFiles(Raster2D& testRaster);

	//@}

	//---------------------------------------------------------------------------


	///@name Data output functions
	//@{
      virtual void joinCatchmentMap(Content cont, Raster2D& gisFull, int userContentId = -1);
      virtual void joinCatchmentMap(Content cont, Raster2D& gisFull, int monthIndex, bool flag_forceToCell, int userContentId = -1);

      virtual void writeToFile_SektorMaps(Content cont, int userContentId = -1);
      virtual void writeToFile_SektorMaps_monthly(Content cont, int m);

      virtual void writeInputToFile(char*);

      ///         PRINT - WRITE functions
		virtual void printToScreen(void);
      virtual void printEffectiveRouting();
      virtual void writeToFileEffectiveRouting();
      virtual void printToScreen_allSectors();
      virtual void printToScreen_Edic();
		virtual void printOnScreen_MatrixSize(void);
      virtual void printInflows();
	//@}

	//---------------------------------------------------------------------------
	///@name Constructors and destructors
	//@{
	///Constructor
  	scuenca(void);
   scuenca(int sc);

	///Destructor
	virtual ~scuenca ();

	//@}

//---------------------------------------------------------------------------
protected:
//---------------------------------------------------------------------------

// Moved from global to local. 
   // Contains map of all sectors; used to dissect maps and pass them to sector->landscape 
   Raster2D gisKey;
   
   
	int sc_nummer;		//Nummer des Untereinzugsgebiets


	//int NUMINFLOWS;	//Anzahl der Wasservorkommen , noch global
   double b_faktor;	//Faktoren zur Aggregation von Sektoren
   double g_faktor;

   // Now instead of NUM_SECTORS()
	int numberOfActualSectors;    //Anzahl der dazugehoerenden Sektoren
                                 // used to be called sek_anzahl

	//enum {MAXELEMS = 1};//Maximum moeglicher Elemente
	unsigned num;		//Anzahl der bereits eingetragenen Elemente

	MatrixDouble b_j;       //diese Daten werden in folgende Matrizen
	MatrixDouble g_j;       //geschrieben
	MatrixDouble d_jk;
	MatrixDouble e_jk;

   // For simpler information handling
   MatrixDouble totalSectorIrrig;   // dim (sectors X months)
   MatrixDouble sumByMonth_Irrig;  // dim (1 X months)

   // Auxiliary Class with irrigation technology parameters
   IrrigationMethod paramsIrrigationMethod;


   //Matrix aller Wasservorkommen
   // ... in liter/sec
	MatrixDouble N; //Dimension: ( inflows  X  (months+1) )
   // ... in m3/sec, same as raw data!
   MatrixDouble N_typical_m3Sec; //Dimension: ( inflows  X  (months+1) )

   /* TODO 2 -oArnold -cImplement : Inflows should be defined by catchment, not globally */
   // ------------------------------
   //	   ABOUT INFLOWS
   // ------------------------------
   //	int NUMINFLOWS;//Anzahl der Wasservorkommen
   //
   // Now: each inflow also has a name, and an ID.
   // These values are stored in BasicData.cpp, and accessible via
   //    string INFLOWNAME_val(int val);
   //    string INFLOWNAME_pos(int pos);
   //    int INFLOWVAL_pos(int pos);
   //    void setNUMINFLOWS(int numInflows);
   //    int NUMINFLOWS(void);
   //    readInflowKey( ABSTRFILE_fn() );
   //    ABSTRFILE_fn() is read from file: "AllDefaults.txt"

	MatrixDouble Derr;      //Matrizen werden sukzessive mit Informationen
	MatrixDouble Esc;			//aus den Sektoren aufgefuellt

      //
   bool isDynamicRiverFlow;
   DynamicRiverFlows DynamicFlows_preknown;


   virtual void speicher_freigeben(void);

};


#endif

