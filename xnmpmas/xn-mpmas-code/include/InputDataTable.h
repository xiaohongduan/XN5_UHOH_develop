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
//  File:       InputDataTable.h (formerly inputTable.h)
//
//  Contents:   Auxiliary class for reading input files
//
//
//
//------------------------------------------------------------------------------


#ifndef _InputDataTable_h
#define _InputDataTable_h

#include <stdio.h>
//#include <stdlib.h>

//Arnold2006_10
// Libraries C++
#include <iostream>

#include <fstream>   // new 2005
#include <string>    // new 2005
#include "MatrixDouble.h"

#define _NUMDATATYPES_ 4
enum DataType {typeEMPTY, typeDouble, typeString, typeInteger, typeValue, typeMAX};
string getStringDataType(DataType tp);
void printDataTypeToScreen();
bool checkDataType(DataType tp, DataType typeExpected, string nameOfFunction);

#define _NUMCOLS_ 200  // maximum number of columns
#define _MAXROWS_ 2000  // maximum number of columns
#define _NUM_COLINFO_ 2
enum enumTableCol{colType, colCol};



/** The auxiliary class <classInputTable> is for reading tables from a formated, but generic
 *  input table. ASCII Files have the following look:
 *  n   > Number of header lines
 *  h1  > Header line 1
 *  h2  > Header line 2
 *  ...
 *  hn-1> Header line n-1, contains variable names
 *  h_n > Header line n, contains data types
 *  d1  > Data row 1, matching data type column n
 *  d2  > Data row 1, matching data type column n
 *  ....
 *  EXAMPLE (the first column containing 1m h1, .. is just for explaination!):
 *  n   >  3
 *  h1  > bla bla blabla bla (does not matter!)
 *  h2  > Name     Val1 Val2  Address
 *  h3  > s         d    i     s
 *  d1  > name1     3.1  5    street1
 *  d2  > name2     5.1  6    street2
 *  ....
 *  (Data types can be strings, doubles, or integers)
 *  This class reads such a table of mixed input data, performs strict checks on data
 *  types. Data can either be accessed
 *     - via the (capitalized) NAME of the variable,
 *     - via the absolute column in the original MatrixDouble (dangerous! C++ - notation)
 *
 */


class classInputTable
{

public:

	///@name Data query functions
	//@{	
	///Return pointers to crop growth modules

   // READ functions
   //virtual void readFromFile(ifstream& s);

   virtual void readFromFile(FILE* s);
   virtual void readFromFile(char* s);
   virtual void readFromFile(string s);

   //---------------------------------------------------------------------------

   ///@name Information Functions about size and content
	//@{	   
   /// Some functions to access information
   //  info on data types, columns, etc
   int rows() {return hasNumberOfRows;}
   int cols() {return hasNumberOfCols;}

   void checkDimension(int r, int cAbs, string nameOfFunction);

   
   int getStringCols(){return hasNumberOfStringCols;};
   int strings(){return getStringCols();};
   int getNumCols(DataType dt);
   
   // Returns data type by column in original data table
   DataType getTypeFromAbsCol(int cAbs)
      {return (DataType)(int)KEY.getValue(cAbs, colType);}

   // Returns (INTERNAL) column in storage class by column in original data table
   // (not to be used, as the use of internall columns might become confusing)
   int getInternalColFromAbsCol(int cAbs)
      {return (int)KEY.getValue(cAbs, colCol);}

   // Returns column in original data table
   virtual int findAbsColumnByName(string varname);
   virtual DataType getTypeByVarname(string varname);
   virtual string getVariableName(int cAbs);
	//@}
   //---------------------------------------------------------------------------

   ///@name ACCESS functions: GET
	//@{	   
   //  All functions require the column in the original table
   virtual int getInt(int r, int colAbs);
   virtual double getDouble(int r, int colAbs);
   virtual string getString(int r, int colAbs);

   virtual int getInt(int r, string varname);
   virtual double getDouble(int r, string varname);
   virtual string getString(int r, string varname);
	//@}
   //---------------------------------------------------------------------------

   ///@name ACCESS functions: SET
	//@{	   
   //  All functions require the column in the original table
   virtual void setValue(int r, int c, double value);
   virtual void setString(int r, int c, string  str);
	//@}
   //---------------------------------------------------------------------------

   ///@name WRITE functions
	//@{	
   virtual void writeToFile(ofstream&);
   virtual void writeToFile(char* s);
   virtual void writeToFile(string s);
   void writeToFile_RowsWhereColUnequals(string fn, string nm, double valCol);

	//@}
   //---------------------------------------------------------------------------

   ///@name Return complete column (by reference)
	//@{	
      
   /// Get complete columns
   //  info on data types, columns, etc
   virtual DataType getCol(string varname, MatrixDouble& matrixTarget);
   virtual DataType copyColumnIntoVector(string varname, MatrixDouble& matrixTarget);

   virtual DataType getCol(string varname, string*& stringArrayTarget);   

   // Column in original table
   virtual DataType getCol(int col, MatrixDouble& matrixTarget, int colOfMat);
   virtual DataType copyColumnIntoMatrix(int col, MatrixDouble& matrixTarget, int colOfMat);
   
   
   virtual DataType getCol(int col, MatrixDouble& matrixTarget);
   virtual DataType getCol(int col, string*& names);
   
   void getMatrixFromTable(int startVal, int endVal, MatrixDouble& indexValueAndColumn, MatrixDouble& monthlyWaterRequirements );
   
   /// Get full matrix with common start string
   void getMatrixWithStartString(string startStr, MatrixDouble& indexValueAndColumn, MatrixDouble& matrixReturned);
   
	//@}
   //---------------------------------------------------------------------------

   ///@name Output functions
	//@{	
   void printKeyToScreen();
   void printAllInfo();
   void printStrings();
   void printCell(int r, int colAbs);
   void printCellToStream(int r, int colAbs, ofstream& out);
   void printRowToStream(int r, ofstream& out);
   void printRow(int r);
   void printCol(int columnAbsolute);
   void printCol(string varn);
   void printVarNames();
   void printVarNamesToStream(ofstream& out);
   void printTypesToStream(ofstream& out);
   void printTypes();
	//@}
   //---------------------------------------------------------------------------

   ///@name Find-Commands
	//@{	
   virtual int findAllRowsWithVal(int colAbs, double val, MatrixDouble& foundVals);
   virtual int findAllRowsWithVal(string varNm, double val, MatrixDouble& foundVals);

   virtual int findAllRowsWithVal(int colAbs, int val, MatrixDouble& foundVals);
   virtual int findAllRowsWithVal(string varNm, int val, MatrixDouble& foundVals);

   

   virtual void shrinkKeyToActualSize();
	//@}
   //---------------------------------------------------------------------------

   ///@name Constructor and destructor functions
	//@{	

	classInputTable(void)
	{	isvirgine=true;
      initializeEmpty();
	}
   void initializeEmpty();

   classInputTable(classInputTable& tab)
   { isvirgine=true;
     copyToThis(tab);
   }

   void allocate(int rows_, int numberOfcolumns_, MatrixDouble& vectorOfColumnTypes, string*& namesOfColumns);
   
    //DESTRUCTOR
    // calls 'deallocate()'
	virtual ~classInputTable();

   void copyToThis(classInputTable& tab);

   virtual void deallocate();

   //virtual void joinIntoThis(classInputTable& tab);

   // Operators
	virtual classInputTable& operator= (classInputTable& tab);
	//@}
   //---------------------------------------------------------------------------
	
	
   
private:

 virtual void setAllZero();

protected:     //public
   /// Data
   bool isvirgine;
   // Number of rows, cols
   int hasNumberOfCols;
   int hasNumberOfRows;
   int hasNumberOfStringCols;
   int hasNumberOfDoubles;
   int hasNumberOfInts;

   // Storage of variable names
   string* varNames;    // a vector of strings

   // Storage of all string-entries 
	string** str;
   
	// Storage of value-entries in a double class "MatrixDouble"
	MatrixDouble mat;

   // KEY contains same number of columns as table.
   // Thus, each entry represents one column.
   //
   // KEY has two rows, specifying 
   //    (a) the data type of that column
   //    (b) the column in internal storage class
   // line <enumTableCol> colType : (double)DataType
   // line <enumTableCol> colCol  : (double)Column
   MatrixDouble KEY;

   // Copy constructors. Carefull: These protected functions do not clear memory,
   // thus the function <initializeEmpty> has to be called before!
   // Then, the number of cols and rows have to be set
   void copyStr(string** s);
   void copyVarNames(string* varNames);
   void setRows(int r) {hasNumberOfRows=r;}
   void setCols(int c) {hasNumberOfCols=c;}

	virtual void copyMat(MatrixDouble& m);
   virtual void copyKey(MatrixDouble k);
   virtual int getInternalColByVarname(string varname);

   // produces error output if dt is not integer or double
   virtual bool testForMatType(DataType dt); 
   virtual bool testForMatType(DataType dt, bool doOutput);    

};

string makeCapitalLetter(string s);
   // Append to closed / open file
void appendToFileBackgroundIrrig(classInputTable& tab, MatrixDouble& foundVals, string outFile);
void appendToFileBackgroundIrrig(classInputTable& tab, MatrixDouble& foundVals, ofstream& out);
void writeToFileBackgroundIrrig(classInputTable& tab, MatrixDouble& foundVals, string outFile);

string convertTypeAsString(DataType dt);

// string StringToLower(string strToConvert);
// string StringToUpper(string strToConvert);
//


#endif
