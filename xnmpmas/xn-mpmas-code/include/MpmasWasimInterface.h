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
//  File:       MpmasWasimInterface.h (formerly translation.h)
//
//  Contents:   Handles the interface between MP-MAS and WASIM-ETH
//
//
//
//------------------------------------------------------------------------------

#ifdef COUPLWASIM

//------------------------------------------------------------------------------
#ifndef _MpmasWasimInterface_h
#define _MpmasWasimInterface_h
//for class FileType

#include <string>

#include <stdio.h>
#include <iostream>
#include <fstream>
using namespace std;  // std::stream  --> stream (simpler)

#define _CUTOFF_IRRIG_  1E-4

class ausgabe;
// LINUX / WINDOWS specific definitions
#ifdef AIX
   #include <unistd.h>
#else
	#include <dos.h>
#endif /* AIX */

class MatrixDouble;
class Raster2D;
class Raster3D;
class classInputTable;


enum enumActivityKeyNames
{typeAKeyActivityID, typeAKeyLPCOL, typeAKeyPRICECOL,   typeAKeyLPROW, typeAKeySOILCLASS, typeAKeyIPG,
typeAKeyKRTYPE, typeAKeyKY, typeAKeyCROPTYPE, typeAKeyIRRMETHOD, typeAKeyYIELDMAX,
typeAKeyIrrigEfficiency, typeAKeyLanduseMpmas, typeAKeyLanduseWasim, typeAKeyCropID,
typeAKeyEND,
matrixTypeAKeyMonthlyDemand, matrixTypeAKeyExpectations,
finalAKey};

enum enumTypeAggreg
{typeAggreg_mean, typeAggreg_mostFrequent, typeAggreg_positiveMean
};

////////////////////////////////////////////////////////////////////////
// --> TRANSLATION FUNCTIONS USED BY ALL COMPONENTS
// =====================================================================

void readActivityKey(ifstream& stream, string*& colNamesActivityKey, classInputTable& activityKey, MatrixDouble& matrixMonthlyWaterDemand, MatrixDouble& matrixMonthlyWaterExpectations);
void readActivityKey(string fn, string*&  colNamesActivityKey, classInputTable& activityKey, MatrixDouble& matrixMonthlyWaterDemand, MatrixDouble& matrixMonthlyWaterExpectations);
void getKeyfileColumnNames(string*& colNamesActivityKey);

int readTable_ActivityKey_Lookup(MatrixDouble& lookup_CroppingActivity2WasimLanduse, string fnActivityKey);
int readTable_CropKey_Lookup( MatrixDouble& lookupMatrix_LuMpmas_2_LuWASIM, string fnCropKey);

void addToLookup_CropKey(MatrixDouble& lookup_CroppingActivity2WasimLanduse, MatrixDouble& lookupMatrix_LuMpmas_2_LuWASIM);


void translateIrrigQuant2WASIM(Raster2D& irrigIDs, Raster2D& gisCropAct,
		// in: Irrig quantity in mm, translation MatrixDouble,  number of "bins" per activity
 		Raster2D& gisIrrigQuant, MatrixDouble& MAT_IRRIGID, int maxExpansion);




////////////////////////////////////////////////////////////////////////
// --> DEFINITION OF RESCALING FUNCTIONS
// =====================================================================
//Arnold2006_12_31_c
// returns ratio of both cells
// link-MatrixDouble linkss g-> G, thus to recover f, we still need shift_f2g_row, col
//

// Shift C->G is recalculated during rescaling!
int rescale_CREATEMAP(
               Raster2D& C,      // original data
               Raster2D& f,       // original data
               Raster2D& gisLink_g2G,  // reference
               Raster2D& g,           // reference
               Raster2D& G,           // reference
               int& shift_f2g_row,   // reference
               int& shift_f2g_col    // reference
               );

// assuming that gisOrig is already initialized and contains NODATA everywhere
void rescale_G2f_old(
               Raster2D& G,           // data
               Raster2D& gisLink_g2G,  // data
               int shift_f2g_row,   // data
               int shift_f2g_col,    // data
               int ratio,
               Raster2D& gisTarget     // reference to final data struct
               );

// NEW FUNCTION TO TRANSLATE GIS FROM TWO RESOLUTIONS
//
// Example executable: SEE allgem9.cpp
//
// Function creates direct link MatrixDouble that connects C to some data MatrixDouble.
// Since link MatrixDouble does not refer to g any more, the link numbers are directly
// linking f to G.
// If a function receives a data source of dimension C instead of G, the shift C2G
// has to be considered!
// returns ratio of both cells
int rescale_CREATEMAP_f2G(
               Raster2D& C,      // original data
               Raster2D& f,       // original data
	       			 Raster2D& G, 		 // return large MatrixDouble
               Raster2D& gis_Link_f2G,  // reference
               MatrixDouble& mSHIFT   // reference
               );

// gisTarget will have same dimensions as gisLink
void rescale_C2f(
               Raster2D& C,           // data
               Raster2D& gisLink_f2G,  // data
               int Y_shift_C2G,   // data
               int X_shift_C2G,    // data
               Raster2D& gisTarget     // reference to final data struct
               );

void rescale_f2G(Raster2D& gis_f, Raster2D& gis_L, Raster2D& gis_Gorig, Raster2D& gis_G, enumTypeAggreg tp);
// assume mean
void rescale_f2G(Raster2D& gis_f, Raster2D& gis_L, Raster2D& gis_Gorig, Raster2D& gis_G);


void rescale_saveAll(string dirOut,
      Raster2D& gis_L,    // Link MatrixDouble, will be saved to "gis_L"
      Raster2D& gis_G,    // Coarse extracted MatrixDouble around gis_L, will be saved to "gis_G"
      Raster2D& gis_C,    // Coarse full MatrixDouble, only passed for x/y-coordinates
      int Y_shift_C2G,     // Shift of row: full MatrixDouble C to G
      int X_shift_C2G,     // Shift of col: full MatrixDouble C to G
      int ratio            // Ratio of cellsize gis_L to gis_C
      );

void rescale_loadAll(string dirOut,   // only input, rest loaded
      Raster2D& gis_L,    // Link MatrixDouble,  loaded from "gis_L"
      Raster2D& gis_G,    // Coarse extracted MatrixDouble around gis_L, loaded from "gis_G"
      int C_ycoord,    // Coarse full MatrixDouble,  x/y-coordinates
      int C_xcoord,    // Coarse full MatrixDouble, x/y-coordinates
      int Y_shift_C2G,     // Shift of row: full MatrixDouble C to G
      int X_shift_C2G,     // Shift of col: full MatrixDouble C to G
      int ratio            // Ratio of cellsize gis_L to gis_C
      );

int ratio_getAndCheck(double nom, double div);
int ratio_getAndCheck(int nom, int div);


// ------------------------------------------------------------
//    Functions around COUPLING
// --> start
// ------------------------------------------------------------

void Mpmas_initCoupling();

int DOWASIM_loadKeyFile(string fn, MatrixDouble& keyMat);


void initTDTvariables();



// Both Functions call COMMUNICATE_ALL_DATA, to ensure mirror order of data exchange
// flag_SENDorRECEIVE == true  : SEND
// flag_SENDorRECEIVE == false  : RECEIVE

void receiveRaster3D_couplingData(Raster3D& raster3D,  bool RECEIVE_TDT,  bool doprocessing, bool isIrrig);
void sendViaTDT_Raster3D_couplingData( bool RECEIVE_TDT, bool doprocessing, bool isIrrig);


int receiveIntFromController(void);
void sendIntToController(int val);


//void extractColumns_initializeFromFile(string fn, MatrixDouble& transKey);
//int extractColumns_initializeFromFile(ifstream& stream, MatrixDouble& transKey);
string updateraster3D_filenames(Raster3D& raster3D, string irrigYN);

////////////////////////////////////////////////////////////////////////
// --> Functions used in coupling and also in MPMAS internally
// =====================================================================
//Arnold2006_83
//Master function, calls next three functions
// Filenames specified here!
//void  readMAS2WASIM(int expandFact, MatrixDouble& MAT_IRRIGID);

// Read cropping activity table
//int readTable_cropAct(MatrixDouble& MAS2WASIM, char* keyname);
//int readTable_LU_mas2wasim(MatrixDouble& LUmas2Wasim, char* filename);

void  readMAS2WASIM_byKey(int expandFact,  MatrixDouble& MAT_IRRIGID);

void readActivityKey(ifstream& stream,  classInputTable& activityKey,
      MatrixDouble& matrixMonthlyWaterDemand,
      MatrixDouble& indexMonthlyWaterDemand,
      MatrixDouble& matrixMonthlyWaterExpectations,
      MatrixDouble& indexMonthlyWaterExpectations);
void readActivityKey(string fn,  classInputTable& activityKey,
      MatrixDouble& matrixMonthlyWaterDemand,
      MatrixDouble& indexMonthlyWaterDemand,
      MatrixDouble& matrixMonthlyWaterExpectations,
      MatrixDouble& indexMonthlyWaterExpectations);

string getKeyfileColumnNames(enumActivityKeyNames type);
int readTable_ActivityKey_MAS2WASIM(MatrixDouble& MAS2WASIM, string fnActivityKey);
int readTable_CropID2WasimLU(MatrixDouble& LUmas2Wasim, string fnCropKey);


void addToMas2Wasim_LU_mas2wasim(MatrixDouble& MAS2WASIM, MatrixDouble& LUmas2Wasim);
// ------------------------------------------------------------
//    Functions around COUPLING
// <-- end
// ------------------------------------------------------------
void initTDTvariables();
// Function returns number of irriagion types
void  sendIrrigationQuantities_Raster(Raster2D& rasterIrrigationQuantity_mm, int numIrrigationActivities, int m);

int receiveIntFromController(void);
void sendIntToController(int val);


#endif

#endif
