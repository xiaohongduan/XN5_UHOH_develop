//------------------------------------------------------------------------------
//
//  Thomas Berger (main author), Christian Troost, Pepijn Schreinemachers, and Thorsten Arnold
//
//
//  Hohenheim University (490d)
//  Professorship for Land Use Economics in the Tropics and Subtropics
//
//  May not be distributed without permission by main author
//
//  File:      Landscape.h
//
//  Contents:   Grid cell raster
//
//
//
//------------------------------------------------------------------------------


#ifndef MpmasLandscape_h
#define MpmasLandscape_h



#include <vector>
#ifndef ODB
	#include "MainAuxiliaryFunctions.h"
#endif
#include "MpmasPlotCell.h"
#include "BasicMath.h"

using namespace std;

class Raster2D;
class parcelGroup;


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
	
	//Troost 160111: Replaced matrix double of farmstead IDs (referenced by agent IDs)
	//by vector of pairs containing the location (Note: x,y, not r, c!, ) of the farmsteads in sector map (array index)
	//all other info needed for agent initialization in createAgents can be easily accessed knowing the farmstead location
	//once all maps have been
	//much more efficient than running through the whole map again and again
	//int  numAg;
	/* TODO 1 -oArnold -cImplement : Discard this vector - it does not belong into this class */
	//MatrixDouble agentIDs;
	vector<coord>  farmsteadLocations;


	//list of parcel groups in the sector
	//Note: main list of parcel groups, groups that or not in this list should not exist
	list<parcelGroup*> parcelGroupList;

	public:
	//pointer to cells in grid
	/* TODO 2 -oBerger -cImplement : grid should become protected element of ldscape */
	mpmasPlotCell*** grid;

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
#ifdef MULTIPERIOD
	double getExtraCropActAttributeInCell(int r, int c, int userContentID);
#endif

	virtual void setValueInCell_monthly(int r, int c, int m, Content cont, double value);
	virtual double getValueInCell_monthly(int r, int c, int m, Content cont);
	virtual void getValueInCell_monthly(MatrixDouble& mat, int m,  Content cont);

// Troost 20160331 Soil type transitions
// Troost **********begin block insert**********
#ifdef MULTIPERIOD
	void updateSoilTypes();
#endif
// Troost **********end block insert**********


	virtual void registerParcelGroup(parcelGroup* pg);
	virtual void deleteParcelGroup(list<parcelGroup*>::iterator it);
	virtual void assignCellToParcelGroupAndOwner(int r, int c, parcelGroup* pg, agent* ag );
	virtual parcelGroup* assignCellToNewParcelGroupAndOwner(int r, int c, agent* ag);
	virtual parcelGroup* getParcelGroupForCell(int r, int c);
	virtual void updateParcelGroups();
	virtual void initializeParcelGroupsAndAssignToOwners();
	virtual void findNeighboringCellsAndAttachToParcelGroup(int r, int c, agent* ag, int first = false);
	virtual vector<int>findContentAroundCell(int sr, int sc , int distance, Content cont);

	// --------------------------------------
	/// Should be cleaned up.... much of this is quick and dirty
	// --------------------------------------
	// Functions that do something
	//virtual void setFoundAndPlotDistances(int fr, int fc);
	/*virtual void makeAgentVector(int nAg);
	virtual void setAgentID(int i, int val);
	*/

	void reserveSpaceFarmsteadLocationsVector(size_t n) { farmsteadLocations.reserve(n); };
	void appendToFarmsteadLocationVectorRC(int r, int c)	 { farmsteadLocations.push_back(coord(c, r)); }
	void appendToFarmsteadLocationVectorXY(int x, int y)	 { farmsteadLocations.push_back(coord(x, y)); }
	coord getFarmsteadSectorXYByAgentId(int i) {	return farmsteadLocations.at(i); }
	std::pair<int,int> getFarmsteadSectorRCByAgentId(int i) {	return std::pair<int,int>(farmsteadLocations.at(i).y, farmsteadLocations[i].x);	}
	void setStatusFarmstead(int r, int c) 							{	grid[r][c]->setStatusFarmstead();	}

	int getAgentMapIdByAgentId(int i);
	int getContentAtFarmsteadLocationOfAgentId(int i, Content cont);


	size_t getNumAgents() {return farmsteadLocations.size() ; }


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
	// folowing functions have benn replaced by  ...ByAgentId versions
	/* virtual int getAgentMapID(int i); // replaced by ...ByAgentId
	virtual int getPopID(int i);
	virtual int getCluID(int i);
	virtual int getNetID(int i);
	virtual int getNumPl(int i); */
	//virtual int getColor(int i); // even previously unused

	mpmasPlotCell* getCell(int r, int c);

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

