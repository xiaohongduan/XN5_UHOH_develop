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

#ifdef OLDCELLS
#ifndef _GridCellHandler_h
#define _GridCellHandler_h
#include <iostream>
using namespace std; // fuer C++-Compiler, damit cin nicht als std::cin eingelesen werden muss
#include <fstream>   // new 2005
#include <string>    // new 2005
#include <vector>

#include "CropSoil.h"
#include "AgentParcel.h"
#ifndef ODB
	#include "MainAuxiliaryFunctions.h"
#endif
#include "MatrixDouble.h"
#include "BasicMath.h"

class Raster3D;
class Raster2D;
class parcelGroup;





// gridcell
class cell
{ 
	protected:
	bool isInitializedWithData;
	
	int cellFarmID;	//farmstead ID, must be identical with agent plot ID (cellAgentID = cellFarmID)//## Tbe 110423 substitute
	int farmsteadID;	//farmstead ID in map
	
	//int cellAgentID;	//agent plot ID, must be identical with farmstead ID (cellAgentID = cellFarmID)//## Tbe 110423 substitute
	int agentMapID;	//owner ID of plots in map

	double cellDistanceHome;//Euklidean distance of plot from farmstead
	int cellFlagIdentifiedByOwner;   //flag: plot identified by owner
	int cellSoilType;  //soil type
	int cellUserMapID; //user map ID (operator, not necessarily owner)
	int color;         //color of cell user's plots

	int cellPopID;     //population ID of owner
	int cellCluID;     //cluster ID of owner
	int cellNetwID;    //network ID of owner
	int cellSectorID;  //used when migrating cells to catchment

	double cellPrecip; //cell precipitation

// Troost 20140527 Generalize dynamic NRUs
// Troost **********begin block insert**********
	double * cellUserDefined; //user defined landscape parameters
// Troost **********end block insert**********

	/*
	// This pointer is initialized if the grid is actually owned by some agent, and
	// if it then attributed spatially. Otherwise, it remains as initialized to NULL
	plot* parcel;
*/
	parcelGroup* parcelGroupPtr;
	// Pointer to soilC
	// Changed pointer soilC* sCon. Since there were problems with memory allocation,
	// now I defined a variable soilC sConDat, which is still referenced with a
	// pointer soilC* sCon. Thus, the code does not have to be changed anywhere.
	// Only memory allocation changes, sCon->METHODE() still works!
	soilC* sCon;   //soil conditions
	//soilC sConDat;

	//Spatial allcoation of cropping activities
	//cropAct links to milp->getHydro_XXXXX(int crA)
	int cropAct;     // Crop activity
	double cropYield;

	/// FOR COUPLING
	// ... dimension: ( 1 X (contENDCOUPL - contENDIRRIG + 1) )
	// contains information for 
	// ... contSoilMoist, contSoilMoist05, contSoilMoist1, contETpot, contETreal, 
	// ... contPrecip,	contSurfaceRunoff, contPercol, contBaseFlow, contGroundwaterRecharge,  
	MatrixDouble couplVector;

	// (returns zero if not relevant)
	// ... automatically allocated if called first time, automatically destructed.
	// ... Dimensions: contENDMONTHLY-contENDCOUPLTABLE X 12
	MatrixDouble monthlyCoupl; 
	

	// ---------------------------------------------------
	// SHOULD ALL BE PRIVATE
	// ---------------------------------------------------
	

	// Friend!
	friend class ldscape;

private:

	// General, original version
	//virtual int getFaID(void);//## substitute
	virtual int getFarmsteadID();
	//virtual int getAGID(void);//## substitute
	virtual int getAgentMapID();

	virtual double getDistanceFromHome(void);
	virtual int getFlagOwnerOK(void);
	virtual int getType(void);
	virtual int getUserMapID(void);
	virtual int getColor(void);

	virtual int getPpID(void);
	virtual int getClustID(void);
	virtual int getNetwID(void);
	virtual int getSectorID(void);

	virtual void setSectorID(double in);
	virtual void setSectorID(int in);

	//virtual void setFaID(int cellFarmID);// ## substitute
	virtual void setFarmsteadID(int f);

	//virtual void set_aID(int cellAgentID);// ## substitute
	virtual void setAgentMapID(int a);

	virtual void setDistanceFromHome(double cellDistanceHome);
	virtual void setFlagOwnerOK(int cellFlagIdentifiedByOwner);
	virtual void setSoilType(int cellSoilType);
	virtual void setUserMapID(int usID);
	virtual void setColor(int color);

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
	virtual double getCropYield(void);
	virtual void setCropYield(double value);
	virtual double getPrecip();
	virtual void setPrecip(double val);


	// CELL
	// Receives ACTUAL (real) months, but internally saves as reduced ("relevant") months.
	// Internal conversion.

	// For wasim coupling: Access to vector with exchanged data
	virtual double getCouplingValue(Content cont);// CELL
	virtual void setCouplingValue(Content cont, double val);// CELL

public: // for landscape

//   public:
	cell();
	virtual ~cell();
	virtual void deallocate(void);
	virtual void setEmptyAll();
	// calls:
	virtual void setEmptyBasic(void);
	virtual void setEmptyIrrig(void);
	virtual void setEmptyTSPC(void);

	virtual void setParcelGroup(parcelGroup* pg);
	virtual parcelGroup* getParcelGroup();

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
	// set
	virtual void setByContent_monthly(Content cont, int m, int val);
	virtual void setByContent_monthly(Content cont, int m, double val);

	virtual void setByContent(Content cont,  int val);
	virtual void setByContent(Content cont,  double val);

// Troost 20140527 Generalize dynamic NRUs
// Troost **********begin block insert**********
	virtual void setUserDefinedContent(int userContentId, double val);
// Troost **********end block insert**********

	// get
	virtual double getByContent_monthly(Content cont, int m);
	virtual double getByContent(Content cont);
	// Troost 20140527 Generalize dynamic NRUs
	// Troost **********begin block insert**********
	virtual double getUserDefinedContent(int userContentId);
	// Troost **********end block insert**********


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

	int myCatchmentID;
	int mySectorID;
	
	int  numAg;
	/* TODO 1 -oArnold -cImplement : Discard this vector - it does not belong into this class */
	MatrixDouble agentIDs;

	list<parcelGroup*> parcelGroupList;
	public:
	//pointer to cells in grid
	/* TODO 2 -oBerger -cImplement : grid should become protected element of ldscape */
	cell*** grid;

	// --------------------------------------
	/// Constructor and Destructor
	// --------------------------------------
	ldscape();
	ldscape(int,int);
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
	virtual void printCell(int row, int col);

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
	virtual void provideCellContentViaGisStruct(Content cont, Raster2D& rasterCont);
	//virtual void setCellContentViaGisStruct(Content cont, Raster2D& rasterCont); // ## Tbe 110422 eliminate?

	// THESE SHOULD BE ONLY ACCESS FUNCTIONS at cell level
	// setValueInCell(r, c, contDistFromHome, value);
// Troost 20140527 Generalize dynamic NRUs
// Troost **********begin block changed**********
	virtual void setValueInCell(int r, int c, Content cont, double value,  int userContentId = -1);
	//virtual void setValueInCell(int r, int c, Content cont, double value);
// Troost **********end block changed**********
	virtual double getValueInCell(int r, int c, Content cont);
	virtual double getValueInCell(int r, int c, Content cont, bool suppressErrorMessage);
	virtual void getValueInCell(MatrixDouble& mat, Content cont);

// Troost 20140527 Generalize dynamic NRUs
// Troost **********begin block insert**********
	virtual double getUserDefinedValueInCell(int r, int c, int userContentID);
// Troost **********end block insert**********



	virtual void setValueInCell_monthly(int r, int c, int m, Content cont, double value);
	virtual double getValueInCell_monthly(int r, int c, int m, Content cont);
	virtual void getValueInCell_monthly(MatrixDouble& mat, int m,  Content cont);

	virtual void registerParcelGroup(parcelGroup* pg);
	virtual void deleteParcelGroup(list<parcelGroup*>::iterator it);
	virtual void assignCellToParcelGroup(int r, int c, parcelGroup* pg);
	virtual parcelGroup* assignCellToNewParcelGroup(int r, int c, int oid = -1);
	virtual parcelGroup* getParcelGroupForCell(int r, int c);
	virtual void updateParcelGroups();
	virtual void initializeParcelGroups();
	virtual void findNeighboringCellsAndAttachToParcelGroup(int r, int c);
	virtual vector<int>findContentAroundCell(int sr, int sc , int distance, Content cont);

	// --------------------------------------
	/// Should be cleaned up.... much of this is quick and dirty
	// --------------------------------------

	// Functions that do something
	virtual void setFoundAndPlotDistances(int fr, int fc);
	virtual void makeAgentVector(int nAg);
	virtual void setAgentID(int i, int val);
	virtual int getNumAgents();

	// --------------------------------------
	/// Should all be erased - use setValueInCell(...) / getValueInCell(...)
	// --------------------------------------
	// row r, col c always refer to the grid within "cell*** grid"
	virtual int getFound(int r, int c);
	virtual void setFound(int r, int c, int);

	virtual soilC* getSoilCond(int r, int c);

	virtual void setSoilCond(int r, int c, soilC*&);//overwrites values
	virtual void updSoilCond(int r, int c, soilC*&);//adds values

	// These require some vector
	//virtual int getFstID(int i);// ## substitute
	virtual int getAgentMapID(int i);
	
	virtual int getPopID(int i);
	virtual int getCluID(int i);
	virtual int getNetID(int i);
	virtual int getNumPl(int i);
	virtual int getColor(int i);

	// receives ACTUAL (real) MONTH!
	// Internal conversion within cell::getMonthlyIrrigationQuantity(m);
	// ... to be handled by
	//virtual double getMonthlyIrrigationQuantity( int r, int c, int m);
	//virtual void setMonthlyIrrigationQuantity(int r, int c, int m, double newVal);

};
enum from {
	fromStart, fromRight, fromLeft, fromAbove, fromBelow,
	fromBelowRight, fromBelowLeft, fromAboveRight, fromAboveLeft
};
struct cellToCheck
{
		int r;
		int c;
		int from;
		cellToCheck(int _r, int _c, int _from)
		{
			r = _r; c = _c; from = _from;
		}
} ;

#endif

#endif OLDCELLS
