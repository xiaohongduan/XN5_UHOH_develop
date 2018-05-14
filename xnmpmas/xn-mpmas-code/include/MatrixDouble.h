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
//  File:       MatrixDouble.h (formerly matrix9.h)
//
//  Contents:   Matrix class, implemented for floating points. Features include: 
//		          - initialization
// 	          - information on matrix
// 	          - handling of matrix size, 
// 	          - overloading operators (simple calculation)
// 	          - reading and writing of matrix from or to file
// 	          - printing matrix or matrix components to screen, in various formats
// 	          - accessing the content of the matrix (element, row, column, ...)
// 	          - send and receive matrix through TDT
// 	          - perform specific tasks (sort, histogram, ...)
//			
//------------------------------------------------------------------------------

#ifndef _MatrixDouble_h
#define _MatrixDouble_h

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <string>
using namespace std; // fuer C++-Compiler, damit cin nicht als std::cin eingelesen werden muss
#include "BasicEnums.h"
#include "BasicDefines.h"

#ifndef EPS
#define EPS 1.0e-6   //precision, OSL seems to work with 1.0e-7
// (see "ekk_c_api.h")
#endif
#ifdef ODBSTORE
	#include <vector>
	#include <string>
#endif
#ifdef NOMAS
	#include <fstream>
	#include "nomas_utilities.h"
#endif

#ifdef DO_TDT
	extern "C" {
		#include "tdt.h"
	}
#endif


class MatrixDouble
{	protected:     //public

//ODB: make data array transient, because they need to transformed, respectively old
	double** ptr;

	int z_anzahl; //rows
	int s_anzahl; // columns



public:
	//CONSTRUCTOR without memory allocation
	// --> Everything set to 'NULL' / NOTDEFINED
	MatrixDouble (void)
	{	setEmpty();
	}

	MatrixDouble ( const MatrixDouble& other );

	//CONSTRUCTOR with memory allocation
	// --> memory allocated,
	MatrixDouble (int r, int c)
	{	setEmpty();
		allocate(r,c);
	}

	//DESTRUCTOR
	// calls 'deallocate()'
	virtual ~MatrixDouble();

	// --> pointer set to 'NULL' everything else to NOTDEFINED
	// z_anzahl, s_anzahl are ZERO
	void setEmpty(void);
	// Free memory of **ptr;  setEmpty
	virtual void deallocate(void);
	virtual void deallokieren(void); // calls 'deallocate'   


	////////////////////////////////////////////////////////////////
	// FUNcTIONs TO MODIFY THE DIMENSIONS OF MATRIX
	//allocate(z,s);
	//setValueExp(r, c, val)  --> expands MatrixDouble automaticall,
	// 	if r, c not within MatrixDouble!
	//addRow()
	//addRows(4)
	//addCol()
	//addCols(2)

	// if dimensions are [0][0] - or loose all values
	virtual void allokieren (int z, int s);
	virtual void allocate(int row, int col);
	// Vector
	virtual void allocate(int elems);
	//Immediately initialize with val
	virtual void allocate (int z, int s, double val);
	virtual void allocate (MatrixDouble& m2); // Allocates same dimensions as m2

	// RESIZING MATRIX
	//Function expands MatrixDouble by "zus_z" rows
	virtual void matrix_vergroessern_z(int zus_z);

	//Function expands MatrixDouble by "zus_s" columns
	virtual void matrix_vergroessern_s(int zus_s);

	// increase MatrixDouble size by adding more than one row
	virtual void addRows(int newR);

	// increase MatrixDouble size by adding  more than one column
	virtual void addCols(int newC);
	// increase MatrixDouble size by adding  one row
	virtual void addRow(void);
	// increase MatrixDouble size by adding  one column
	virtual void addCol(void);

	// deletes row with number 'row' (any, from within)
	virtual void deleteRow(int row);
	// deletes one col (any, from within)
	//   virtual void deleteCol(int col);
	// deletes first row
	virtual void deleteFirstRow(void);
	// deletes first row
	virtual void deleteLastRow(void);

	// insertRow
	// Searches column 'col' until val is larger then first entry (to maintain order!)
	// Then, one row is added into that position
	virtual int insertRow(int col, double val);


	// Increase MatrixDouble size by adding  row vector to MatrixDouble (complying with row number)
	virtual void addRow(MatrixDouble& m);
	// Increase MatrixDouble size by adding column vector to MatrixDouble (complying with row number)
	virtual void addCol(MatrixDouble& m);


	//////////////////////////////////////////
	// INFORMATION ON MATRIX SIZE
	// access size of MatrixDouble
	virtual int zeilen(void);
	virtual int spalten(void);
	virtual int rows() const;
	virtual int cols() const;
	// <length()> returns maximum  of (rows()) and (cols())
	virtual int length() const;
	// <width()> returns minimum of (rows()) and (cols())
	virtual int width() const;

	// returns <true> if width() == 1
	virtual bool isvector();
	virtual bool isVector(); // rename of above

	//////////////////////////////////////////
	// INFORMATION ON MATRIX: KEYS
	// --> get information on values
	//
	// Functions to transform MatrixDouble into PositionMatrix (Sparse Matrix notaiton)
	// MatrixDouble 'posMat' contains numValues rows, three columns
	//  row   col  getValue(row, col)
	//
	//Then,the number of entries is returned
	//
	//Writes all values equal to 'val' into posMAT
	virtual int findAllEntriesEquallingValue(MatrixDouble& posMat, double value);

	//Writes all values not equal to 'noVal' into posMAT
	virtual int getAllValPos(MatrixDouble& posMat, double noVal);

	virtual void shiftPosMat(
			int rowShift,   // shift of rows
			int colShift    // shift of cols
	);

	virtual void makeTableHeader(string fn);
	virtual void makeTableHeader(char* d);

	// Count entries that are not NODATA
	virtual int countNOT_NODATA(double NODATA);
	virtual int countDATA(double DATA);

	//Function deletes on row from MatrixDouble and shrinks MatrixDouble
	virtual void cutMatrixRow(int crow);//neu 2004
	//Function deletes on column from MatrixDouble and shrinks MatrixDouble
	virtual void cutMatrixCol(int ccol);//neu 2004
	// Matrix copies all elements from matrix2 into mother MatrixDouble.
	//Both matrixes need same dimension
	virtual void copy(MatrixDouble& matrix2);
	//Arnold2006_200
	virtual void copyInto(MatrixDouble& matrixTarg);

	// Operators
	virtual MatrixDouble& operator+= (MatrixDouble& matrix2);
	virtual MatrixDouble& operator-= (MatrixDouble& matrix2);
	// Multiply MatrixDouble piecewise
	virtual MatrixDouble& operator*= (MatrixDouble& matrix2); 
	// Multiply MatrixDouble with scalar
	virtual MatrixDouble& operator*= (double fact);
#ifdef AIX
	virtual MatrixDouble& operator= (MatrixDouble& matrix2);
#endif   
	virtual MatrixDouble& operator= (const MatrixDouble& matrix2);

	// READING & WRITING TO FILE -----------------------------//
	// Matrix is read from closed file, to be passed name of file
	//virtual void readInputFromFile(char* filename);
	// Matrix is read from open file (C-Notation)
	virtual void readInputFromFile(char* fn, int z, int s);
	virtual void readInputFromFile(string fn, int z, int s);

	//reads part of matrix only, error if not upper left corner
	virtual int readTopMatrixCorner(FILE*, int ,int);

	// Vector function ...
	//virtual void readInputFromFile(FILE*, int);
	virtual void readInputFromFile(char* fn, int pos);
	virtual void readMatrix_FILE(string filename);
	virtual void readMatrix_FILE_INT(string filename);
	virtual void readInputFromFile_INT(FILE* datei);
	// Function reads line and dissects it char by char...

	// Matrix is read from open file (C-Notation)
	//virtual void readInputFromFile(ifstream&); // Matrix has to be allocated already!

	virtual void readInputFromFile(FILE*); // Matrix has to be allocated already!
	virtual void readInputFromFile(FILE*, string description ); //For easier debugging. Troost 140807.
	//virtual void readInputFromFile(ifstream&, int ,int);
	//virtual void readInputFromFile(ifstream&, int);

	// Function reads line and dissects it char by char...
	// Carefull: Delimits tabs '\t', space and semicolon.
	virtual void readDelimNums(ifstream& streamData, int numVals);

	//virtual void appendRowFromFile(ifstream& in, int numValues);
	// Assumes full row
	//virtual void appendRowFromFile(ifstream& in);

	virtual void appendRowFromFile(FILE* in, int numValues);
	virtual void appendRowFromFile(FILE* in);

	virtual void appendColToFileAsRow(int col, string fn);
	virtual void appendColToFileAsRow(int col, char* fn);

	virtual void appendRowToFile(int row, string fn);
	virtual void appendRowToFile(int row, char* fn);
	virtual void appendRowToOpenStream(ofstream& outStream, int row);
	virtual void appendRowToOpenStream(FILE* strm, int row);

	virtual void appendVecToFile(char* fn);
	virtual void appendVecToFile(string fn);
	virtual void appendVecToFile(ofstream& out);

	virtual void appendColToFileAsRow(int col, ofstream& out);

	// Writing into file, using "filename"
	virtual void writeToFile(char* filename);//renamed from <datei_schreiben>

	/// Functions that write matrix to open streams
	virtual void writeToFile(FILE* stream);//renamed from <in_datei_schreiben>
	virtual void writeToFile_ScientificFormat(FILE* stream);//renamed from <in_datei_schreibenE>
	virtual void writeToFile_Transposed(FILE* stream);//renamed from <in_datei_schreibenT>
	virtual void writeToFile_NonZeroValues(FILE* stream);//renamed from <writeToFileNonZeroValuesE>

	virtual void writeRowToFile_NoLineEnd(FILE* stream, int row);
	virtual void writeRowToFile_WithLineEnd(FILE* stream, int row);
	virtual void writeRowToFile_NoLineEnd(ofstream& stream, int row);
	virtual void writeRowToFile_WithLineEnd(ofstream& stream, int row);

	virtual void writeColToOpenFileAsRow_WithLineEnd(ofstream& stream, int row);
	virtual void writeColToOpenFileAsRow_NoLineEnd(FILE* stream, int col);
	virtual void writeColToOpenFileAsRow_WithLineEnd(FILE* stream, int col);

	// Functions that write to open streams, using C++ stream
	virtual void writeToFile(ofstream& out);
	//virtual void writeToFile(char* filen); ## Tbe 120329
	virtual void writeToFile(string filename);
	virtual void writeToFileTransposed(string filename);
	virtual void writeToFileTransposed(FILE* stream);


	// First: Filename
	// 	(default:
	// 			- FILENAMES(keyLOG) for DOLONG==false,
	// 			- FILENAMES(keyLOG2)  for DOLONG==true )
	// Name of TDT configuration file
	// type of communication: 1 SEND, 0 RECEIVE
	// DOLONG: Also write some content of MatrixDouble
	virtual void writeLogfileTDT(string fn, char*  configFile, int type, bool dolong);
	virtual void writeLogfileRW(string , char* , int type);


	virtual void writeToFileT(ofstream& out);
	// Into first row, writes two entries: number of rows,
	// and number of columns
	// Leaves empty row in second line
	virtual void readFromFileWithDims(string fn);
	virtual void readFromFileWithDims(char* fn);
	//virtual void readFromFileWithDims(ifstream& in);

	virtual void readFromFileWithDims(FILE* in);

	virtual int readVectorDyn(string fn);
	virtual int readVectorDyn(char* fn);
	//virtual int readVectorDyn(ifstream& in);

	virtual int readVectorDyn(FILE* in);

	// This function does not yet know anything about MatrixDouble size.
	// - The size is determined according to an analysis of the first line
	// - Empty lines (during and in the end) are ignored
	// - Check: Line should have appropiate number of entries!
	virtual int readMatDyn(string fn);
	virtual int readMatDyn(char* fn);
	//virtual int readMatDyn(ifstream& in);

	virtual int readMatDyn(FILE* in);

	virtual void readFromFileDimsAndHeader(string fn);
	virtual void readFromFileDimsAndHeader(char* fn);
	//virtual void readFromFileDimsAndHeader(ifstream& in);

	virtual void readFromFileDimsAndHeader(FILE* in);

	virtual void writeToFileWithDims(string fn);
	virtual void writeToFileWithDims(char* fn);
	virtual void writeToFileWithDims(ofstream& out);

	// Write in sparse MatrixDouble notation (often simpler for reading)
	// If MatrixDouble only: Start with dimensions (ROW, COL)
	//    header (normal)
	//    row  col  val
	virtual void writeToFile_sparse(string fn, int nodata);
	virtual void writeToFile_sparse(char* fn, int nodata);
	virtual void writeToFile_sparse(ofstream& out, int nodata);
	// Only for internal use, does not write dimension of MatrixDouble
	virtual void writeToFile_sparse_values(ofstream& out, int nodata);

	//virtual void loadFromFile_sparse_values(ifstream& streamData, int nodata);

	virtual void loadFromFile_sparse_values(FILE* streamData, int nodata);

	virtual void loadFromFile_sparse(string fn, int nodata);
	virtual void loadFromFile_sparse(char* fn, int nodata);
	//virtual void loadFromFile_sparse(ifstream& streamData, int nodata);

	virtual void loadFromFile_sparse(FILE* streamData, int nodata);

	///////////////////////////////////////////////////////////////
	// WRITING TO SCREEN
	// Write complete MatrixDouble  to screen
	virtual void printToScreen(void);
	// Print only subset of MatrixDouble
	virtual void printToScreen(int rlu, int clu, int rdr, int cdr);


	virtual void ausschreiben (void);   //(german)

	//Write all rows, as integers
	//   (using pTS_row_compact(int z); )
	//    as integer value if whole values
	//    as 3.2  if double values (cuts of larger numbers...)
	virtual void printToScreen_compact(void);
	virtual void printToScreen_compact(int numDigits);

	// Print only subset of MatrixDouble
	virtual void printToScreen_compact(int rlu, int clu, int rdr, int cdr); // assumes 2 digits
	virtual void printToScreen_compact(int rlu, int clu, int rdr, int cdr, int numDigits);






	//	virtual void printToScreen_compactT(void);
	virtual void printToScreenInt(void);
	//Write column as integer value
	virtual void printToScreen_col_compact(int c);

	//Write row or part of row
	virtual void printToScreen_row(int z);
	virtual void printToScreen_row(int z, int startC, int endC);
	//Write row in compact form

	//    as integer value if whole values
	//    as 3.2  if double values (cuts of larger numbers...)
	virtual void printToScreen_row_compact (void);     // assumes to print last row / vector, 2 digits, full row
	virtual void printToScreen_row_compact (int z); // assuming 2 digites, full row
	virtual void printToScreen_row_compact (int z, int numDigits);
	virtual void printToScreen_row_compact (int z, int startC, int endC, int numDigits);
	virtual void printToScreen_row_compact (int z, int startC, int endC); // assuming 2 digites

	virtual string printToString_row_compact(int z, int startC, int endC, int numDigits);
	virtual string printToString_row_compact (int z);

	// Print size of MatrixDouble
	virtual void printSize();
	// Print only "active" rows
	// --> ignores all entries equal to "noval", uses sparse MatrixDouble notation
	//    r   c   val
	//    3   5   0.234
	virtual void printToScreen_col_valuesOnly(int c, double noval);

	virtual void printSparse(double NODATA);

	// Prints a row if the col "c" does NOT have value "noval"
	virtual void printToScreen_ifColActive(int c, double noval);

	//wichtig: die Nummerierung der Zeilen und Spalten beginnt mit 0
	virtual void wert_ausschreiben(int, int); //(write column, German)
	virtual void zeile_ausschreiben(int);     //(write row, German)
	virtual void spalte_ausschreiben(int);

	// -----------------------------------------------------
	// ACCESSING VALUES
	// -----------------------------------------------------

	/////////////////////////////////////////////////////
	// SETTING VALUES
	//
	// Access within existing MatrixDouble only, with value
	// both equal (historical)
	virtual void wert_eintragen (int, int, double wert);
	virtual void setValue(int row, int col, double value);
	// Vector function
	// (only possible if one-dimensional,
	//	 menaing rows() or cols() is 1
	virtual void setValue(int pos, double val);
	// INITIALIZE MATRIX WITH OTHER MATRIX
	//   allocate same dimensions if necessary
	virtual void setValue(MatrixDouble& m1);

	// If position of value is out of range of the MatrixDouble,
	// it is expanded with additional rows and  columns
	// (carefull: difficult to debugg function afterwards!)
	virtual void setValueExp(int r, int c, double val);

	// Vector function
	// Set complete column to one value
	// --> set all values in column to val
	virtual void setCol(int c, double v);
	// Set complete row to one value
	virtual void setRow(int row, double val);

	// Overwrite one row  with a MatrixDouble (dimensions need to match)
	virtual void setRow(int row, MatrixDouble& vec);//<overwriteRowInMatrix>
	// Overwrite one column with a MatrixDouble (dimensions need to match)
	virtual void setCol(int col, MatrixDouble& vec);//<overwriteColumnInMatrix>

	// Set all values of initilaized MatrixDouble to 'val'
	virtual void setAllValues(double val);

	virtual void extractSubmatrixColKey(MatrixDouble& target, MatrixDouble& colKey);
	virtual void extract(MatrixDouble& empt,
			int row_left_up, int col_left_up,
			int dimR, int dimC);
	virtual void extract(MatrixDouble& empt, MatrixDouble& minmax);


	// Set all elements of MatrixDouble with one value
	virtual void initialisieren (double startwert); // (same in German)
	virtual void initialisieren (int startwert); // cast to double


	/////////////////////////////////////////////////////
	// ACCESS CONTENT OF MATRIX
	/////////////////////////////////////////////////////
	// returns content of element[r][c]
	virtual double getValue(int r, int c) const;
	virtual double wert_holen(int z, int s) const; //(german, historical)

	// Vector function
	// This function returns col OR row,
	// if MatrixDouble is a vector otherwise: returns error
	virtual double getValue(int pos) const;

	// Function requests empty matrixes as argument (passed by reference)
	// Then, values are written into them.
	virtual void getCol(int col, MatrixDouble& mCol );//<insertColumnIntoMatrix>
	virtual void getRow(int row, MatrixDouble& mRow );//<insertRowIntoMatrix>

	// -------------------------------------------------------
	// Other access function - Calculations
	// renamed from wert_hinzuaddieren
	virtual void addValue(int, int, double);
	virtual void addValue(int, double);

	virtual void divideValue(int, int, double);
	virtual void divideValue(int, double);

	// Increases specified value by one
	virtual void increment(int r, int c); // add one

	// Add the column 'pCol' of 'matPassed' to column 'thisCol' of *THIS
	virtual void addColValues(int pCol, int thisCol, MatrixDouble& matPassed);

	// Multiply one element in matrix
	virtual void wert_multiplizieren(int, int, double);
	// Scalar multiplication of complete matrix
	virtual void skalar_multiplizieren(double);
	// Divide one element in matrix
	virtual void wert_dividieren(int, int, double);

	// Component-wise multiplication
	virtual void mult(MatrixDouble& m1, MatrixDouble& m2);

	// Calculate element-wise
	// All assume same matrix dimensions, and return NODATAVALUE.
	// If one of the matrixes contains "NODATAVALUE", then THIS will be NODATAVALUE
	// Operator performed on m1 - / + / * / div m2
	virtual double subtractMatrix(MatrixDouble& m1, MatrixDouble& m2, int noData1 , int noData2);
	virtual double addMatrix(MatrixDouble& m1, MatrixDouble& m2, int noData1, int noData2);
	// Division by zero --> NODATAVAL
	virtual double divideMatrix(MatrixDouble& m1, MatrixDouble& m2, int noData1, int noData2);
	virtual double multMatrix(MatrixDouble& m1, MatrixDouble& m2, int noData1, int noData2);


	// Returns sum of one row
	virtual double sumRow(int);
	// Returns sum of one column
	virtual double sumCol(int);

	virtual double spaltensumme(int); // not used
	virtual double zeilensumme(int);  // not used

	//Arnold2006_84
	// Copies one row to another row
	virtual void copyRow(int originRow, int targetRow);

	// Copy originRow from THIS to target MatrixDouble (targetRow)
	virtual void copyRow(int originRow, MatrixDouble& mTarg, int targetRow);

	// Copy transposed: column <originCol> from THIS to row target MatrixDouble (targetRow)
	virtual void copyCol2Row(int originCol, MatrixDouble& mTarg, int targetRow);
	virtual void copyCol2Col(int originCol, MatrixDouble& mTarg, int targetCol);

	//Arnold2006_86
	// Functions return maximum value from
	// --- complete MatrixDouble
	virtual double getMax(void);
	// --- MatrixDouble  row specified
	virtual double getMax_row(int row);
	// --- MatrixDouble  column specified
	virtual double getMax_col(int col);

	// Functions position of maximum value from
	// --- complete MatrixDouble
	virtual int findMax_row(int row);
	// --- MatrixDouble  column specified
	virtual int findMax_col(int col);
	virtual int findMax_col_nonempty(int col, double emptVal, int emptCol);
	// Functions return maximum value from
	// --- complete MatrixDouble



	virtual double getMin(void);
	virtual double getMinNotNodata(int nodata);
	// --- MatrixDouble  row specified
	virtual double getMin_row(int row);
	virtual double getMin_row(int row, int nodata);
	// --- MatrixDouble  column specified
	virtual double getMin_col(int col);


	// Swaps the content of two pointers
	virtual void swap (double *pa,double *pb);

	// Function returns smaller MatrixDouble MEXTRACT with all rows
	// specified in ACTIVE
	virtual void mextract(MatrixDouble& ACTIVE, MatrixDouble& MEXTRACT);

	/////////////////////////////////////////////////////
	//    OTHER MATRIX OPERATIONS
	/////////////////////////////////////////////////////
	// findPos searches the MatrixDouble for "val" and writes the position
	// into FOUNDPOS (passed by reference)
	//
	// FINDPOS then has two columns: One for the row, one for the column of val.
	// Each row corresponds to one value.
	//
	// Function returns number of values (FOUNDPOS.rows())  or -1 (not found
	//
	// Example call:
	//    MatrixDouble m, pos;
	//    [m = ...]
	//    double val = -1;
	//    m.findPos(-1, pos);
	virtual int findPos(double val, MatrixDouble& FOUNDPOS);
	virtual int findPos_col(int col, double val, MatrixDouble& FOUNDPOS);

	// Finds all values that are not equal to 'noData', and writes those into FOUND
	// FOUND has Sparse Matrix notation
	virtual int findIsValue(double noData, MatrixDouble& FOUND);

	// Matrix returns position vector to all data that are NOT equal to the value 'val',
	// and also different to noData'
	virtual int findDifferent(double val, double noData, MatrixDouble& FOUNDPOS);



	// Function returns first row in which the value of col (getValue(?,col)==val
	// Then, returns row or -1 (not found)
	virtual int findfirstCol(int colSearch, double val); // (old)
	virtual int findfirstMatchInCol(int colSearch, double val); // (new)
	virtual int findfirstMatchInRow(int rowSearch, double val);



	// Some simple computations: median, mean, variance etc.
	// Calculates values for all columns
	virtual void medianAllCols(MatrixDouble& medM);//assigns result to matrix passed by reference

	virtual double varianceOfCol(int col, double mean);
	virtual double standardDeviationOfCol(int _col, double mean);
	virtual double giniOfCol(int _col);

	virtual double getVarianceIfNotNodata(double nodata_, double mean_);
	virtual double getStandardDeviationIfNotNodata(double nodata_, double mean_);
	virtual double getMeanIfNotNodata(double nodata_);

	virtual double medianCol(int col);//computation just for one column as indicated
	virtual double arithmeticMeanOfColumn(int col, double noData_);


	// Sorts rows in ascending order
	// Sorts ALL columns in ascending order
	virtual void sortColumns(void);
	// (historical, not supported any more)
	virtual void spalten_aufsteigend_sortieren (int z, int s);

	// Sorts one column in ascending order
	// and connects all other columns according to this column 'col'
	virtual void sortAfterOneColumn(int col);


	// Sorts one column in ascending order
	// and connects all other columns according to this column 'col'
	// The order of the columns is retuzrned in the MatrixDouble 'matPos'
	virtual void sortAfterOneColumn(int col, MatrixDouble& matPos);
	//Arnold2006_200
	virtual void sortAfterOneColumn_B(int col, MatrixDouble& POSITION);


	//Arnold - send complete MatrixDouble (both equal, renamed only)
	virtual void sendMatrixViaTDT(char* configFile);
	virtual void sendViaTDT(char* configFile);
	virtual void sendViaTDT(string configFile);
	//
	virtual void receiveViaTDT(char* configFile);
	virtual void receiveViaTDT(string configFile);

#ifdef DO_TDT
	virtual void sendMatrixViaTDT_open(TDTConfig& tc, TDTState& ts);
	virtual void receiveViaTDT_open(TDTConfig& tc, TDTState& ts);

#endif

	virtual void readWasimStatisticFile(string fn, int numGages, int numHeaderRows, bool withSum);
	virtual int getNumGages(string fn, int numHeaderRows);
	virtual int findDateRow(MatrixDouble& DateRow, int row);

	int getRowToDateInWasimFile(int _year, int _month, int _day, int _hour);
	void addColValuesFromWasimStat_SameDates(int sourceCol, int thisCol, MatrixDouble& wasimStatFile);



	//Mast2006_05
	//virtual void readFromTranslationMatrix(int z, int s, ifstream& in);
	//virtual void readFromTranslationMatrix(ifstream& in);

	virtual void readFromTranslationMatrix(int z, int s, FILE* in);
	virtual void readFromTranslationMatrix(FILE* in);

	virtual void readFromTranslationMatrix(char* fn);

	virtual void printToScreenTranslationMatrix(int z, int s);
	virtual void printToScreenTranslationMatrix();

	///////////////////////////////////////////////////////////////////
	// SEARCH FUNCTIONS
	///////////////////////////////////////////////////////////////////
	// returns position in column

	virtual int getPositionInColumn(double val, int col);
	virtual int getPositionInColumn(int val, int col);

	// ASSUME THAT MATRIX IS A VECTOR
	virtual int getPositionInColumn(int val);     // cast to double
	virtual int getPositionInColumn(double val);

	// CREATE HISTOGRAM
	// An empty MatrixDouble is passed (or, if not empty, it is re-initialized)
	// Then, all values that occur are written into the first column
	// The quantity of these values are written to the second column
	//
	// EXAMPLE CALL:
	//    MatrixDouble matHist;
	//    // initialize with any values
	//    MatrixDouble gisMat; gisMat.readFromFile(filename);
	//    gisMat.makeHist(matHist);
	//    matHist.printToScreenInt();

	virtual void makeHist(MatrixDouble& histMat);
	virtual void makeHist(MatrixDouble& histMat, double first);
	// writes vector into the MatrixDouble uniqueM (passed by reference)
	virtual void unique(MatrixDouble& uniqueM);
	virtual void unique(int col, MatrixDouble& uniqueM);

	// Transposes THIS into m
	virtual void transpose(MatrixDouble& m);

	// Function to extract a number of columns from matOrig,
	// then reorder and write to matTarget.
	//
	// The vector transKey has dimensions
	//         transKey.length()==matTarget.cols();
	//         The maximum integer entry in transKey.max() <= matOrig.cols();
	//
	virtual void extractColumns(MatrixDouble& matTarget, MatrixDouble& transKey);

	// Function determines minimum row / col and maximum row / col
	// and writes it into MatrixDouble minmax
	//
	//  minmax  [ minRow,  maxRow ]
	//          [ minCol,  maxCol ]
	virtual void findMinMax(MatrixDouble& minmax, double nodata);


	// Filter function.
	// Of all values of vector <isTrue> that are FALSE,
	// find maximum value
	//
	// argument <isTrue> is vector of bools (MatrixDouble)
	//
	// Returns row index into THIS with maximum value that is FALSE in isTrue
	//
	virtual int findMaxNotTrue(int col, MatrixDouble& isTrue);

	// Returns MatrixDouble as string
	virtual string mat2string();
	virtual void string2mat(string s);


	// Developed from "TestGlobalSensitivity.cpp"
	virtual void copyToLargerMatrix(int row, int startCol, MatrixDouble& fullVector);
	virtual void copyToLargerMatrix(int startCol, MatrixDouble& fullVector);
	virtual void copyOneRowToLargerMatrix(int row, int startCol, MatrixDouble& targetMatrix);//Tbe 091128
	virtual void copyOneColToLargerMatrix(int col, int startRow, MatrixDouble& targetMatrix);//Tbe 091128
	virtual double sumVector();
	virtual void roundToInteger();

	virtual void writeErrorToLogfile(string fn, char* fehlertext, int type);


};


int receiveIntegerViaTDT(TypeDataExchangeChannel canal);
void sendIntegerViaTDT(int val, TypeDataExchangeChannel canal);
void sendStringViaTDT(string s, char* configFile);
string receiveStringViaTDT(char* configFile);


#ifdef DO_TDT

void sendStringViaTDT_open(string s, TDTConfig& tc, TDTState& ts);
string receiveStringViaTDT_open(TDTConfig& tc, TDTState& ts);

#endif

#endif





