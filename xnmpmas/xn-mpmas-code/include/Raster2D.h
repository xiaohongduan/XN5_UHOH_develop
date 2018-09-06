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
//  File:       Raster2D.h (formerly Raster2D.h)
//
//  Contents:   Auxiliary class, handles 2-dimensional raster
//
//
//
//------------------------------------------------------------------------------


#ifndef _Raster2D_h
#define _Raster2D_h


// Libraries C++
#include <stdio.h>
#include <iostream>
using namespace std;
#include <fstream>
#include <string.h>
#include "MatrixDouble.h"
#include "BasicEnums.h"

////////////////////////////////////////////////////////////////////////////////
// INCLUDE TDT FUNTIONS INTO FILE - ONLY POSSIBLE UNDER LINUX
////////////////////////////////////////////////////////////////////////////////
#ifdef DO_TDT
   extern "C" {
   #include "tdt.h"
   }
#endif

class sector;


enum modeRaster {
	modeIJ, modeXY
};

// -------------------------------
// CLASS DEFINITION
// -------------------------------
// Function  contains all infrmation normally written in a binary ascii GIS file,
// as it is exported by Geographical Information Systems (GIS), including ArcInfo, GRASS,
// It is also a standard for many models in hydrology and land use.
// Thus, this class is
//
// Outputs can be translated (or written) as ASCII-Tables, as
//
class Raster2D
{

private:

	struct FertilizerRate
	{
		int intJulianDayManure;
		double dblManureN;
		double dblManureP;
		double dblManureK;
		double dblManureC;
		int intJulianDay1;
		double dblFertilizerN1;
		double dblFertilizerP1;
		double dblFertilizerK1;
		int intJulianDay2;
		double dblFertilizerN2;
		double dblFertilizerP2;
		double dblFertilizerK2;
		int intJulianDay3;
		double dblFertilizerN3;
		double dblFertilizerP3;
		double dblFertilizerK3;
	} ;

protected:
   int numCols;
   int numRows;
   double xcoord;
   double ycoord;
   double cellsize;
   int noData;

   double** dbPtr;

   Content gisCont;

   //GISrel2abs
   int Xshift;
   int Yshift;

   long double statanz,statmini,statmaxi,sx,sx2;
   float stat_nodata;

   // Matrix of class "matrix9.cpp"
   MatrixDouble mat;

   string filenameGis; //maximal 50 Zeichen

   virtual void copyHeader(Raster2D& gis);

   virtual void copyHeader(const Raster2D& gis);

   // Sum the absolute values of (gisB[r][c] .- gisMother[r][c])
   virtual double gisStruct_sumDiff(Raster2D& gisB);



public:
   Raster2D(void);			//Uebergabe: Output-Dateikrzel
   Raster2D(int cols, int rows);			//Uebergabe: Output-Dateikrzel
   Raster2D(int cols, int rows, int xcoord, int ycoord, double cellS, int nodata);
   Raster2D(int cols, int rows, double xcoord, double ycoord, double cellS, int nodata);
   // Opens GIS-ASCII file, then initializes with header-information, and reads data
   Raster2D(string fn);
   // Expects open GIS-ASCII file, initializes with header-information, and reads data
   Raster2D(FILE* stream);

   //Troost added: To be able to pass in both string(streams) and text files (Note: istream, not ifstream)
   Raster2D(istream&  stream);

   // Allcoate memory
   virtual void initializeEmpty();
   // NOT SUPPORTED ANY MORE
   //virtual void initializeFile(ifstream&  stream);

   virtual void initializeFile(FILE* stream);

   // Opens file <fn> and then calls <initializeFromFile(ifstream&  streamData)>
   virtual void initializeFromFile(string fn);
   // Reads data from GIS-ASCII-file: First reads header, initializes memory, then reads data.
   //virtual void initializeFromFile(ifstream&  streamData);

   virtual void initializeFromFile(FILE* streamData, string fnForError = "?");

   //Troost added: To be able to pass in both string(streams) and text files (Note: istream, not ifstream)
   void initializeFromStream(istream& );

   // Copies only MatrixDouble, without any checks. Assuming same size!
   virtual void copyMatFromThis(Raster2D& target);
   virtual void initCopy(const Raster2D& RHS);
   virtual void copyHeaderFromThis(Raster2D& target);

   virtual void allocate(int rows, int cols);
   virtual void allocate(int rows, int cols, double val);
   virtual void allocate(int cols, int rows, int xc, int yc, double cellS, int noD); // Cast to int
   virtual void allocate(int rows, int cols, double xc, double yc, double cellS, int noD);
   virtual void allocate(void);

   // Allocate to new the same dimensions as to passed Raster2D;
   // Initialize also with gis.getNodata()
   virtual void allocate(Raster2D& gis);
   virtual void setEmptyVal(double val);
   virtual void setEmptyVal(int val); // cast to double
   // Identical to setEmptyVal: Overwrites complete MatrixDouble
   virtual void setAllValues(double val);

   // Fill all with nodata
   virtual void setEmpty();

   //Write  value 'val' into every MatrixDouble position which is 'noData'
   // Also modify NodataVal
   virtual void setNodataValuesToVal(int val);

   //virtual bool checkIfEqualHeaders(Raster2D& rhs);
   virtual double sumAll();


   // Operator "="
   const Raster2D& operator = (const Raster2D& );

   // a) Copy Constructur for Raster2D
   Raster2D(const Raster2D& gisOrig) ;

// Destructor
	virtual ~Raster2D();
   virtual void deallocate(void);

   // -------------------------------------- //
   // ACCESS FUNCTIONS
   // returns number of columns
   int getCols()      { return numCols; }
   int cols() const   { return numCols; }  //For consistency with MatrixDouble-class
   int cols()         { return numCols; }
   // returns number of rows
   int getRows()      { return numRows; }
   int rows() const   { return numRows; }
   int rows()         { return numRows; }

   // returns x-coordinate;
   // often internally used as "shift" in cells
   // from origin of larger Raster2D

   double getXcoord()         { return xcoord; }
   double getYcoord()         { return ycoord; }
   double getXcoord() const   { return xcoord; }
   double getYcoord() const   { return ycoord; }

   // returns length of square grid
   double getCellsize()       { return cellsize; }
   double getCellsize() const { return cellsize; }
   // returns value for "empty" or "Nodata"
   int getNodata()            { return noData; }
   int getNodata() const      { return noData; }
   int getNoData()            { return noData; }
   int getNoData() const      { return noData; }
   Content getContent()       { return gisCont; }
   Content getContent() const { return gisCont; }
   void setContent(Content gisC){ gisCont = gisC; }

   string getFilename() const { return filenameGis;}
   void  setFilename(string s){ filenameGis = s;}

   // Retrieving values from GIS STRUCT
   // (Function always returns constant double!)
   virtual double getValue(int r, int c) const;
         // This function calls for a constant. It is simply
         // passed on to the non-constant version!

   // Function for a call with enum-Type
   //    (position in irrigation MatrixDouble; internal use only...)
   double getValue(enumIrrigationPosition r, int c) const
		{return getValue((int) r, c);}


   // Copy complete file to Raster2D "target"
   virtual void copyFromThis(Raster2D& target);

   // get maximum of complete MatrixDouble
   virtual double getMax(void);
   // get maximum of specified row
   virtual double getMax_row(int row);
   // get maximum of specified col
   virtual double getMax_col(int col);

   // sets number of rows (int numR), DANGEROUS! does not re-size MatrixDouble
   void setCols(int c)        { numCols = c;}
   void setRows(int r)        {  numRows = r;   }

   //sets double xcoord. Used as "origin" of small gis within larger gis,
   // when copying one MatrixDouble into another
   void setXcoord(int xc)     { xcoord = (double)xc; } //casted to double
   void setYcoord(int yc)     { ycoord = (double)yc; }
   void setXcoord(double xc)  { xcoord = xc; }
   void setYcoord(double yc)  { ycoord = yc; }

   void setCellsize(double cellS) {cellsize = cellS;}
   void setNodata(int nodata) {noData = nodata;}
   // Change all noData values to new nodata value
   void changeNodata(int);

   // Passing values TO gisstruct FROM extern
   virtual void setValue(int r, int c, int val); // Cast to double
   virtual void setValue(int r, int c, double val);

   void setValue(enumIrrigationPosition r, int c, double val)
   {	setValue((int) r, c, val);
   }

   // makes histogram of Raster2D
   virtual void makeHist(MatrixDouble& mat); // Takes by reference: Empty MatrixDouble (not yet allocated!)
   virtual void makeHistNoNodata(MatrixDouble& histMat);
      	// Takes by reference: Empty MatrixDouble (not yet allocated!)

   virtual void uniqueNoNodata(MatrixDouble& unMat); // Takes by reference: Empty MatrixDouble (not yet allocated!)

// MatrixDouble matHist;
// Raster2D gis(filename);
// gis.makeHist(mat);
// mat.printToScreenInt();

   // -------------------------------------- //
   // FILE ACCESS FUNCTIONS
   // writes gis-Struct into open C++ file stream, can be read from ArcGIS
   virtual void writeToGisFile_header(ostream&  streamData);

   virtual void writeToGisFile(ostream&  stream);
   virtual void writeToFile(string str);

	// First: Filename
	// 	(default:
	// 			- FILENAMES(keyLOG) for DOLONG==false,
	// 			- FILENAMES(keyLOG2)  for DOLONG==true )
	// Name of TDT configuration file
	// type of communication: 1 SEND, 0 RECEIVE
	// DOLONG: Also write some content of MatrixDouble
	virtual void  writeLogfileTDT(string fn, char*  configFile, int type, bool dolong);
   virtual void writeLogfileRW(string fn, char* matFN, int type);
   virtual void writeLogfileRW(string fn, string matFN, int type);

   //Troost added: To be able to pass in both string(streams) and text files (Note: istream, not ifstream)
   virtual void readHeaderAllocate(istream&  stream);

   virtual void readHeaderAllocate(FILE* stream);

   virtual void readHeaderAllocate(string fn);
   //writes Raster2D as binary file, Routine taken from WASIM-ETH
   virtual void writeToBinFile(string  filenameOut);
   virtual void readFromBinFile(string  filenameIn);
   virtual void initializeSizeFromBinFile(string  fn);

   virtual void readFromFile(string filenameIn);



   // Sparse Matrix:
   //    header (normal)
   //    row  col  val

   virtual void writeToFile_sparse(string str);
   virtual void writeToFile_sparse(char* fn);
   virtual void writeToFile_sparse(ofstream& out);

   //virtual void loadFromFile_sparse(ifstream& streamData);

   virtual void loadFromFile_sparse(FILE* streamData);

   virtual void loadFromFile_sparse(string fn);
   virtual void loadFromFile_sparse(char* fn);

   //////////////////////////////////////////////////////////////////
   // OUTPUT FUNCTIONS
   //////////////////////////////////////////////////////////////////
   virtual void printHeader()   ;

   // Print complete MatrixDouble and header, as doubles
   virtual void printToScreen(void);
   virtual void printToScreen(MatrixDouble& minmax);
   virtual void printToScreen(int rlu, int clu, int rdr, int cdr);

   // shorter form: Print int's as int's, and only up to two digits
   virtual void printToScreen_compact(void);
   virtual void printToScreen_compact_maporder(void);

   // takes MatrixDouble minmax (passed back from function <Raster2D::findMinMax(..)>
   // Prints in compact form a rectangle from within the complete area
   //
   //  minmax  [ minRow,  maxRow ]
   //          [ minCol,  maxCol ]
   virtual void printToScreen_compact(MatrixDouble& minmax);
   virtual void printToScreen_compact(int rlu, int clu, int rdr, int cdr);



   //////////////////////////////////////////////////////////////////
   //    JOINING AND EXTRACTING
   //////////////////////////////////////////////////////////////////


   // Shifts coordinates of Land Use to absolute values, assuming that WASIMluse-coordinates
   // are the origin and <THIS>-coordinates contain the shift as number of grid cells
   //
   //  Also, remembers relative values of masLU for later re-shift
   virtual void shift_coordinats_rel2abs(Raster2D& C);

   // Only calculates shift from ORIGIN from relative coordinates, which are then returned
   // as reference
   // m.getValue(0): row-shift dY
   // m.getValue(1): col-shift dX
   virtual void coord_rel2abs(MatrixDouble& m);


   // Extracts a number of values
   // MatrixDouble minmax is passed back from MatrixDouble::findMinMax(MatrixDouble& minmax);
   //
   //  minmax  [ minRow,  maxRow ]
   //          [ minCol,  maxCol ]
   // Returns Raster2D with same dimensions, where the row/col-shift is
   // still stored in Yshift/ Xshift
   virtual void extractNew(Raster2D& empt, MatrixDouble& minmax);


   // Extracts a number of values
   virtual void extractNew(
         Raster2D& empt,
         int row_left_low, int col_left_low,
         int numRows, int numCols); // default; modeXY

   virtual void extractNew(
         Raster2D& empt,
         int row_left_low, int col_left_low,
         int numRows, int numCols, modeRaster mode);


   // Extracts map that contains values around
   virtual void extractTight(Raster2D& empt);


   // Extraction function from TOTAL coordinates
   // Now xTarget, yTarget are the total coordinates [m]
   // lengthXdir, lengthYdir are the length of the target Raster2D in [m]
   virtual void extractTotal(Raster2D& target,
         double xTarget, double yTarget,
         double lengthXdir, double lengthYdir);

   // Extracts a number of values
   virtual void extractMat(MatrixDouble& empt,
         int row_left_up, int col_left_up,
         int numRows, int numCols);


   // Function copies all data values (not NODATA) from THIS into 'rejoin'.
   // REMEMBER: the row/col - shift is taken from xcoord (cols), ycoord (rows)
   //           of THIS
//      virtual void join(Raster2D& rejoin);
//      virtual void joinAbs(Raster2D& rejoin);

   virtual void joinTotal(Raster2D& rejoin);

   // Function takes all values of THIS amd copies them into a new Raster2D 'target'
   // It shirnks the MatrixDouble by cutting off ALL rows and cols that are both EMPTY
   // and WITHIN the Raster2D
   //
   // This function actually does NOT re-create posMAT, carefull!
   // Use 'Raster2D::getPosMatForVal' or the MatrixDouble-functions 'findAllEntriesEquallingValue'
   // or 'getAllValPos'  before, create posMat,
   //  then pass that (as reference only because of speed!)
   virtual void shrink(Raster2D& target, MatrixDouble& posMat);
   virtual void shrink(Raster2D& target, MatrixDouble& posMat, modeRaster mode);



	// Count entries that are not NODATA
	virtual int countNOT_NODATA();
	virtual int countNOT_NODATA_largerThan(double minVal);
	virtual int countDATA(double DATA);

		// Function returns (by reference) a key that is 'true' wherever
		// motherGis[r][c] == val
		// Also returns Position-Matrix
		virtual void getKeyForVal(Raster2D& keyG, double val, MatrixDouble& posMat);
		virtual void getKeyForVal(Raster2D& keyG, double val);

	//Writes all values equal to 'val' into posMAT
	virtual int findAllEntriesEquallingValue(MatrixDouble& posMat, double ISVAL);

	//Writes all values not equal to 'noVal' into posMAT
	virtual int getAllValPos(MatrixDouble& posMat);


	int getNumberOfDataValues()
	{
		MatrixDouble posMat;
		int numValues = getAllValPos(posMat);
		return numValues;
	}


	virtual void setAllInPosMatWithValue(MatrixDouble& posMat, double val);
	virtual void setAllInPosMatWithValueUnequal(MatrixDouble& posMat, double val, double badVal);

	//Gets POSITION MATRIX
	// dimension: numValues X 3
	//  row   column   value (!=nodata)
	virtual void getPosMatForVal(MatrixDouble& posMat, double val);

	// As before, but gets all positions not equal to NODATA
	virtual void getPosMatForAllData(MatrixDouble& posMat);
	// FUnction returns vector as indicated by posMat
	virtual void filterByPosMat(MatrixDouble& posMat, MatrixDouble& vec);
	virtual void updatePosMat(MatrixDouble& posMat);

	virtual void overwriteWithPosMatContent(MatrixDouble& posMat);


	// --------------------------------------------------------------------//
	// TRANSLATION FUNCTIONS
	// translate()
	// Inputs:
	//    - original gisSturct
	//    - Vector with values of original Raster2D, vOrig
	//    - Vector with target values
	// Idea:
	//    -  first finds value gisO.getValue(r,c) in vOrig, return POS
	//    -  vTarget.getVale(pos) returns target value valT
	//    -  setValue(r,c,valT);
	//
	//    Warning: ONLY FOR INTEGER VALUES! CAREFULL
	virtual void translate(Raster2D& gisO, MatrixDouble& vOrig, MatrixDouble& vTarget) ;
	// Get target value for single value
	// Does translation for above function, but only takes single value
	virtual double getTrans(int val, MatrixDouble& vOrig, MatrixDouble& vTarget);
#ifndef NOMAS
	virtual void readGisFile_Cont(Content cont, int catchID);
	virtual void readGisFile_Cont(Content cont, int catchID, int userContentId);
#endif
	//MANIPULATION FUNCTIONS

	// Subtracts GS1 - GS2 and assigns it to mother;
	// Also checks if noData is identical for both matrixes
	virtual void gisStruct_diff(Raster2D& GS1, Raster2D& GS2);
	virtual void gisStruct_diff_rel(Raster2D& GS1, Raster2D& GS2);
	virtual void translateLuse(Raster2D& masLuse, MatrixDouble& cAtoMl, MatrixDouble& MAStoWASIM);

	////////////////////////////////////////////////////////////////////////////////
	// INCLUDE TDT FUNTIONS INTO FILE - ONLY POSSIBLE UNDER LINUX
	////////////////////////////////////////////////////////////////////////////////

   virtual void sendViaTDT(string configFile);
   virtual void receiveViaTDT(string configFile);

   virtual void sendViaTDT(char* configFile);
   virtual void receiveViaTDT(char* configFile);

   #ifdef DO_TDT
   virtual void sendViaTDT_open(TDTConfig& tc, TDTState& ts);
   virtual void receiveViaTDT_open(TDTConfig& tc, TDTState& ts);
	#endif

   virtual void clearstat();
   virtual void statistics(float *anzahl,float *mini,float *maxi,float *summe,float *mittel,float *std);
   virtual void addstat(float x);

	// Compare header
   virtual bool gisStruct_compH(Raster2D& gisB);
   // First compares header, later complete file
   virtual bool gisStruct_compAll(Raster2D& gisB);

   // Calculate elementwise
   // All assume same MatrixDouble dimensions, and return NODATAVALUE.
   // If one of the matrixes contains "NODATAVALUE", then THIS will be NODATAVALUE
   // Operator performed on m1 - / + / * / div m2
   virtual void subtract(Raster2D& g1, Raster2D& g2);
   virtual void add(Raster2D& g1, Raster2D& g2);
   // Division by zero --> NODATAVAL
   virtual void divide(Raster2D& g1, Raster2D& g2);
   virtual void mult(Raster2D& g1, Raster2D& g2);

   virtual void addToVal(int r, int c, double val);

	virtual int setSmallvaluesToZero(double valSmall);


   // Returns position MatrixDouble of all entries that are NOT equal to 'val,
   // and also not NoData-values
   virtual int findNotEqual(double val, MatrixDouble& FOUND);
   virtual int findIsEqual(double val, MatrixDouble& FOUNDPOS);

	// Function determines minimum row / col and maximum row / col
	// and writes it into MatrixDouble minmax
	//
	//  minmax  [ minRow,  maxRow ]
	//          [ minCol,  maxCol ]
	virtual void findMinMax(MatrixDouble& minmax);

	virtual const Raster2D& mult(double val);

	// Returns "true" if there are values in both the first and last row, and first and last column
	//         "false" otherwise
	virtual bool isTight();
	virtual void setValueByPos(int Pos, double val);
	virtual double getValueByPos(int Pos);
	virtual int findMaxNotTrue(int col, MatrixDouble& isTrue);

	// sorts MatrixDouble THIS by column specified in 'col', and writes result into mSorted
	// Function lets THIS untouched, sorts all values in one column 'col',
	// and sorts all other rows according to that order
	//
	// Once too much this function copies internally complete MatrixDouble
	virtual void sortByCol(int col, MatrixDouble& mSorted);

	virtual void rescale_C2f(
					Raster2D& gis_L,        // data
					int Y_shift_C2G,   // data
					int X_shift_C2G,    // data
					Raster2D& gisTarget     // reference to final data struct
					);

	virtual double getMeanIfNotNodata();

	virtual void asci2binary(char* source, char* target);
	virtual void asci2binary(string source, string target);

	virtual double createByCoarseAndCopyDesign(
			Raster2D& rasterCoarse,
			Raster2D& rasterMASK,
			MatrixDouble& design);



	virtual void addDesignWhereMaskIsOne(
			Raster2D& rasterMASK,
			MatrixDouble& design);

	virtual double sizeUpRaster(
			Raster2D& rasterCoarseInput,
			Raster2D& rasterSize,
			MatrixDouble& design);

	virtual double sizeUpRasterAllocated(
			Raster2D& rasterCoarseInput,
			MatrixDouble& design);

	virtual double sizeUpRaster(
			Raster2D& rasterCoarse,
			string fn,
			MatrixDouble& design);
	virtual bool checkIfEqualHeaders(Raster2D& rhs);

  virtual void ReceiveCropYieldsViaTDT();
  virtual void ReceiveCommandViaTDT();
  virtual void SendLanduseViaTDT();
  void readLookupTable();
  void SendLookupTableViaTDT();
#ifdef DO_TDT
  void SendArrayViaTDT(TDTState& ts);
#endif

};

#endif


