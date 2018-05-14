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
//  File:       GridCellHandler.h (formerly flaeche9.h)
//
//  Contents:   Auxiliary landscape class, containing the grid cells
//
//
//
//------------------------------------------------------------------------------


#ifndef _GridCellHandler_h
#define _GridCellHandler_h
#include <iostream>
using namespace std; // fuer C++-Compiler, damit cin nicht als std::cin eingelesen werden muss
#include <fstream>   // new 2005
#include <string>    // new 2005

#include "CropSoil.h"
#include "AgentParcel.h"
#include "MainAuxiliaryFunctions.h"
#include "MatrixDouble.h"
class Raster3D;
class Raster2D;


// coordinates
typedef struct {
	int x; // = column
	int y; // = row
} coord;

// gridcell
class cell
{
   protected:
   bool isInitializedWithData;
   
	int cellFarmID;    //farmstead ID, must be identical with agent plot ID (cellAgentID = cellFarmID)
	int cellAgentID;   //agent plot ID, must be identical with farmstead ID (cellAgentID = cellFarmID)
	double cellDistanceHome;//Euklidean distance of plot from farmstead
	int cellFlagIdentifiedByOwner;   //flag: plot identified by owner
	int cellSoilType;  //soil type
   int cellUser;      //cellUser ID (operator, not necessarily owner)
   int colr;          //color of cellUser's plots

   int cellPopID;     //population ID of owner
   int cellCluID;     //cluster ID of owner
   int cellNetwID;    //network ID of owner
   int cellSectorID;  // used when migrating cells to catchment

/* Tbe 071120 apparently not needed
   // This pointer is initialized if the grid is actually owned by some agent, and
   // if it then attributed spatially. Otherwise, it remains as initialized to NULL
   plot* parcel;
*/
   // Pointer to soilC
   // Changed pointer soilC* sCon. Since there were problems with memory allocation,
   // now I defined a variable soilC sConDat, which is still referenced with a
   // pointer soilC* sCon. Thus, the code does not have to be changed anywhere.
   // Only memory allocation changes, sCon->METHODE() still works!
   soilC* sCon;   //soil conditions
   //soilC sConDat;

   //Spatial allcoation of cropping activities
   //cropAct links to milp->getHydro_XXXXX(int crA)
   int cropAct;     // Cropping activity
   MatrixDouble monthlyIrrigationQuantity;
                     // Quantity of irrigation water applied
                    // ARRAY, since it encodes for each REAL month
                    // setIrrigQ(mTot) called with real month, has Dims of RELEVANT months

                    // (returns zero if not relevant)

   public:
   cell();
   virtual ~cell();
   virtual void deallocate(void);
   virtual void setEmptyAll();
   // calls:
   virtual void setEmptyBasic(void);
   virtual void setEmptyIrrig(void);
   virtual void setEmptyTSPC(void);



   // ----------------------------------------
   /// Public functions
   // ----------------------------------------
   virtual void printToScreen(void);
   virtual void printToScreenBasic(void);
   virtual void printOnScreenCropped(void);
   virtual void printOnScreenIrrig(void);
   virtual void printOnScreenTSPC(void);

   bool isInitialized (void)
   { return isInitializedWithData;};

   // -----------------
   // get - set functions
   // -----------------

   virtual void setByContent(Content cont, int val);
   virtual void setByContent(Content cont, double val);

   virtual double getByContent(Content cont);

   // ---------------------------------------------------
   // SHOULD ALL BE PRIVATE
   // ---------------------------------------------------

   // Receives ACTUAL (real) months, but internally saves as reduced ("relevant") months.
   // Internal conversion.
   virtual void setMonthlyIrrigationQuantity(int mTot, double monthlyIrrigVal);
   virtual double getMonthlyIrrigationQuantity(int mTot);

   // General, original version
   virtual int getFaID(void);
   virtual int getAGID(void);

   virtual double getDistanceFromHome(void);
   virtual int getFlagOwnerOK(void);
   virtual int getType(void);
   virtual int getUser(void);
//   virtual int getColr(void);

   virtual int getPpID(void);
   virtual int getClustID(void);
   virtual int getNetwID(void);
   virtual int getSectorID(void);

   virtual void setSectorID(double in);
   virtual void setSectorID(int in);

   virtual void setFaID(int cellFarmID);
   virtual void set_aID(int cellAgentID);

   virtual void setDistanceFromHome(double cellDistanceHome);
   virtual void setFlagOwnerOK(int cellFlagIdentifiedByOwner);
   virtual void setSoilType(int cellSoilType);
   virtual void setUser(int usID);
   virtual void setColr(int colr);

   virtual void setPpID(int ppID);
   virtual void setClusterID(int cellCluID);
   virtual void setNetworkID(int nwID);

   // TSPC
   virtual soilC* getSCon(void);
   virtual void setSCon(soilC* sCon);

   virtual double getSoilN(void);
   virtual double getSoilP(void);
   virtual double getSoilK(void);
   virtual double getSoilA(void);
   virtual double getSoilC(void);
   virtual double getSoilSl(void);
   virtual double getSoilSe(void);

   virtual void setSoilN(double);
   virtual void setSoilP(double);
   virtual void setSoilK(double);
   virtual void setSoilA(double);
   virtual void setSoilC(double);
   virtual void setSoilSl(double);
   virtual void setSoilSe(double);

   // Irrigation Spatially explicit
   virtual int  getCropActivityID(void);
   virtual void setCropActivityID(int crA);
};

//------------------------------------------------------------------------------
// --> landscape
class ldscape
{  
   protected:

	int gRows;   //Rows in grid
	int gCols;   //columns in grid
   int gridAlloc;//Flag: grid allocated (yes/no)

   double xcoord;
   double ycoord;
   double cellsize;
   int noData;

   
   int  numAg;
   /* TODO 1 -oArnold -cImplement : Discard this vector - it does not belong into this class */
   MatrixDouble agentIDs;

   public:
   //pointer to cells in grid
   /* TODO 2 -oBerger -cImplement : grid should become protected element of ldscape */
   cell*** grid;

   // --------------------------------------
   /// Constructor and Destructor
   // --------------------------------------
   ldscape();
	virtual ~ldscape();

   virtual void allocateGrid(int Rows, int Cols);
	virtual void deallocGrid();
   virtual void deallocate(int r, int c);

   virtual int getGridAlloc();

   // --------------------------------------
   /// read and write functions
   // --------------------------------------
//   virtual void writeAllMapsIntoFiles(char* prefix); // prefix not used anymore
//   virtual void writeAllMapsIntoFiles();
   virtual void writeLayerToOpenFile(FILE* file, Content cont);
     
   // --------------------------------------
   /// print functions
   // --------------------------------------
   virtual void printToScreen(int r, int c);
   virtual void printOnScreenAll(void);
   virtual void printOnScreenAllCropped(void);

   // --------------------------------------
   /// General access functions (public)
   // --------------------------------------
   virtual int getGRows();
   virtual int getGCols();
   virtual int rows();
   virtual int cols();

   void setXcoord(double xc) {xcoord = xc;};
   void setYcoord(double yc) {ycoord = yc;};
   double getXcoord() {return xcoord;}
   double getYcoord() {return ycoord;}
  
   virtual bool isInitialized(int r, int c);
   
   /// Access by Raster2D
   virtual void provideCellContentViaGisStruct( Content cont, Raster2D& gisCont);
   virtual void setCellContentViaGisStruct( Content cont, Raster2D& gisCont);

   // Access by Raster3D
   virtual void exportBasicCellContentToRaster3D(Raster3D& raster3);
   virtual void exportTSPCCellContentToRaster3D(Raster3D& raster3);
   virtual void exportIrrigCellContentToRaster3D(Raster3D& raster3);
   virtual void exportAnyCellContentToRaster3D(
           Raster3D& raster3Dcell, 
          int startContVal, int stopContVal);
   virtual double getValueInCell(int r, int c, Content cont, bool supressOutputs);
   
   // THESE SHOULD BE ONLY ACCESS FUNCTIONS at cell level
   // setValueInCell(r, c, contDistFromHome, value);
   // = getValueInCell(r, c, contAgentID);
   virtual void setValueInCell(int r, int c, Content cont, double value);
   virtual double getValueInCell(int r, int c, Content cont);

   virtual bool updateCell(int r, int c, Content cont, double value); //probably not used
    //(takes REAL month. Only "cell" does conversion)
   virtual bool updateCellMonthly(int r, int c, int mTot, Content cont, double newVal);


   // --------------------------------------
   /// Should be cleaned up.... dirty
   // --------------------------------------

   // Functions that do something
   virtual void setFoundAndPlotDistances(int fr, int fc);

   // Carefull: This function can be called INSTEAD of setFoundAndPlotDistances, 
   // implemented for standalone consistency check.   
   virtual void setFoundForPlot(int r, int c);
   virtual void makeAgentVector(int nAg);
   virtual void setAgentID(int i, int val);
   virtual int getNumAgents();

   // --------------------------------------
   /// Should all be abbolished - use setValueInCell(...) / getValueInCell(...)
   // --------------------------------------
   // row r, col c always refer to the grid within "cell*** grid"
   virtual int getFound(int r, int c);
   virtual void setFound(int r, int c, int);

   virtual soilC* getSoilCond(int r, int c);

   virtual void setSoilCond(int r, int c, soilC*&);//overwrites values
   virtual void updSoilCond(int r, int c, soilC*&);//adds values

   // These require some vector
   virtual int getFstID(int i);
   virtual int getPopID(int i);
   virtual int getCluID(int i);
   virtual int getNetID(int i);
   virtual int getNumPl(int i);

   // receives ACTUAL (real) MONTH!
   // Internal conversion within cell::getMonthlyIrrigationQuantity(m);
   virtual double getMonthlyIrrigationQuantity( int r, int c, int m);
   virtual void setMonthlyIrrigationQuantity(int r, int c, int m, double newVal);

};


#endif

