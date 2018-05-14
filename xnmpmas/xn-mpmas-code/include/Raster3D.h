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
//  File:       Raster3D.h (formerly Raster3D.h)
//
//  Contents:   Auxiliary class, handles stack of 2-dimensional raster
//
//
//
//------------------------------------------------------------------------------


#ifndef _Raster3D_h
#define _Raster3D_h


#include <stdio.h>
#include <iostream>
using namespace std;
#include <fstream>
#include <string.h>
#include "BasicEnums.h"
#include "Raster2D.h"

////////////////////////////////////////////////////////////////////////////////
// INCLUDE TDT FUNTIONS INTO FILE - ONLY POSSIBLE UNDER LINUX
////////////////////////////////////////////////////////////////////////////////
#ifdef DO_TDT
   extern "C" {
   #include "tdt.h"
   }
#endif

class Raster3D
{
   protected:
      Raster2D* raster3D;
      int _layers;
      string* fnames;
      int month;
      int year;

      virtual void setLayers(int z);
      virtual void initEmpty();


   public:
      // Allocate as empty
      Raster3D(void);

      Raster3D(int z);
      // initializes z layers with same info as gisType,
      // and sets all values to 'gisType.getNoData()'
      Raster3D(int z, Raster2D& gisType);

      virtual ~Raster3D(void);

      // Operator "="
      const Raster3D& operator = (const Raster3D& );

      // a) Copy Constructur for Raster2D
      Raster3D(const Raster3D& gisOrig) ;
      virtual void initCopy(const Raster3D& RHS);
      
//      virtual void initCopy(Raster2D& gis);

      virtual void deallocate();
       virtual void allocate();
      virtual void allocate(int z);
      virtual void allocate(int z, Raster2D& gisType);
      virtual void allocate(const Raster3D& RHS);



      virtual double getValue(int z, int r, int c);
      // writes gisZ into new gis which was passed
      // Returns number of layers
      virtual int getLayers();
      virtual int getLayers() const;
      // Returns number of layers
      virtual int  getMonth();
      virtual int getYear();
      //virtual int getLayerFromCont(Content cont);

      virtual string getFilename(int z);
      virtual void getCopyRaster_2D(int z, Raster2D& gis);
      virtual void getCopyRaster_2D(int z, Raster2D& gis) const;
      virtual Raster2D& getPointertoLayer(int z);

      // -------------------------------------- //
      // ACCESS FUNCTIONS
      // returns number of columns  of FIRST raster3D
      virtual int getCols();
      virtual int cols() const;  //For consistency with MatrixDouble-class
      virtual int cols() ;
      // returns number of rows
      virtual int getRows();
      virtual int rows() const;
      virtual int rows() ;

      // Returns content of layer
      virtual Content getContent(int z);
      // Returns layer with content
      virtual int checkForContent(Content cont);

      virtual void setValue(int z, int r, int c, double val);
      virtual void setFilename(int z, string fn);
      // Expects string-array of same size as Raster2D
      virtual void setFilenameAll(string* fn);
      virtual void resetLayers();// sets layers to zero
      virtual void setContent(int z, Content cont);



      virtual void setMonth(int m);
      virtual void setYear(int y);

      // Sets all values of layer z to value 'val'
      virtual void setAllVal(int z, double val);

      //
      virtual void setGislayer(int z, Raster2D& gisIn);

      ////////////////////////////////////////////////////////////77
      //  OUTPUT TO SCREEN

      virtual void printHeader();
		virtual void printAllHeaders();
      virtual void printToScreen();
      virtual void printToScreen(int z);
      virtual void printToScreen_compact();
      virtual void printToScreen_compact(int z);

      ////////////////////////////////////////////////////////////77
      //  FILES:  WRITING AND READING
      virtual void writeToFiles(string fn, string DIR);
      virtual void writeDescriptionFile(string fn);

      virtual void writeToFiles(string fn, string DIR, int m, int y);
      virtual void writeDescriptionFile(string fn, int m, int y);

      virtual void readFromFiles(string fn, string DIR);
      virtual bool readDescriptionFile(string fn, MatrixDouble& mat);

      ////////////////////////////////////////////////////////////77
      //  MATH: SIMPLE ARITHMETICS
      virtual void add(Raster3D& g3D, Raster3D& g3D2);
      virtual void subtract(Raster3D& g3D, Raster3D& g3D2);
      virtual void mult(Raster3D& g3D, Raster3D& g3D2);
      virtual void divide(Raster3D& g3D, Raster3D& g3D2);
      
      virtual double log_2(double x);

      ////////////////////////////////////////////////////////////77
      //  EXTRACTING POSITION MATRIOX TO TABLE

      virtual void table_getFullTable(Raster2D& gis_L, MatrixDouble& tab);

      virtual void table_getFullTable(MatrixDouble& posMat, MatrixDouble& tab);

      virtual void table_getOneLayerPos(int z, MatrixDouble& posMat, MatrixDouble& tabRow);

      virtual void table_appendFilteredToFile(int z, MatrixDouble& posMat, string fn);
      virtual void table_appendFilteredToFile(int z, MatrixDouble& posMat, char* fn);




      /// Also passes a MatrixDouble with indices to the layers that should be exported.
      virtual void sendViaTDT_extract(string configfile, MatrixDouble& extractVec);
      virtual void sendViaTDT_extract(char* configfile, MatrixDouble& extract);

      virtual void sendViaTDT(string configfile);
      virtual void sendViaTDT(char* configfile);
		
      virtual void receiveViaTDT(char* configfile);
		virtual void receiveViaTDT(string configfile);

      virtual void rescale_C2f(
               Raster2D& gis_L,        // data
               int Y_shift_C2G,   // data
               int X_shift_C2G,    // data
               Raster3D& gis3DTarget     // reference to final data struct
               );

      virtual void rescale_C2f_extract(
               MatrixDouble& extract,
               Raster2D& gis_L,        // data
               int Y_shift_C2G,   // data
               int X_shift_C2G,    // data
               Raster3D& gis3DTarget     // reference to final data struct
               );
               
     ///  Functions to treat binary formats

	  void writeerror(char *dateiname,int fehler);
     int readstackfromfile(char *dateiname);
     void printstackfile(char *dateiname,float ***stack,int levels);
     
     int readFromBinary(char *dateiname);
     void writeStackAsTableCpp(string filename);
     void writeStackAsTable(string filename);
          

	void free_drei_d_matrix(float ***m,int  nrl,int nrh,int ncl,int  nch,int levels);
	float ***drei_d_matrix(int nrl,int nrh,int ncl,int nch,int levels);
	float **matrix(int nrl,int nrh,int ncl,int nch);
	void free_d_matrix(double **m,int nrl,int nrh,int ncl,int nch);
	void free_matrix(float **m,int nrl,int nrh,int ncl,int nch);



     virtual void extractTable_eachOneFile(MatrixDouble& extract, MatrixDouble& posMat, string trunk);
     virtual void extractTable_OneFile(MatrixDouble& extract, MatrixDouble& posMat, string trunk);
	  
     virtual void writeLogfileTDT(string fn, char*  configFile, int type, bool dolong);

     virtual void writeDescriptionFile_open(ofstream& streamData);
	  
	  
};


#endif

