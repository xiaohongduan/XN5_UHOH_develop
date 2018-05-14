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
//  File:       MpmasWasimCouplingFunctions.h
//
//  Contents:   Contains some functions outsourced from MAIN that are only relevant
//              for WASIM couplng, which are NOT used outside of WASIM
//
//             This file can contain MPMAS-specific classes
//------------------------------------------------------------------------------

#ifdef COUPLWASIM

//------------------------------------------------------------------------------

//#include"MpmasWasimCouplingFunctions.h"
#ifndef _MpmasWasimCouplingFunctions_h
#define _MpmasWasimCouplingFunctions_h

#include <string>
using namespace std; // fuer C++-Compiler, damit cin nicht als std::cin eingelesen werden muss

class Raster2D;
class Raster3D;
class MatrixDouble;
class ausgabe;

// For testing
//void testUpdatingToParcelsAndGrids();

// ------------------------------------------------------------
//    Functions around COUPLING
// --> start
// ------------------------------------------------------------

void Mpmas_initCoupling();
void MpmasMain_couplingPrepareFile();

// =====================================================================
////////////////////////////////////
// Link WASIM area to MPMAS area
//
// Read in Raster2D from filename,
// then allocate all non-NODATA values, then determine MatrixDouble:
//
//  minmax  [ minRow,  maxRow ] with first and last row with values
//          [ minCol,  maxCol ] with first and last COL with values
//
// -->

void setCOUPLINGMINMAX(string fn);
void setCOUPLINGMINMAX(MatrixDouble& minmax);
// Copies global MatrixDouble to local copy 'minmax'
void getCOUPLINGMINMAX(MatrixDouble& minmax);
MatrixDouble& getCOUPLINGMINMAX(void);

// Returns first row
int COUPLING_minRow();
// Returns first col
int COUPLING_minCol();
// <--



void DOWASIMCONTROLL();
void DOWASIM_createIrrigMap(int mTot, Raster2D& gisLU, Raster2D& gisQuant, MatrixDouble& keyMAT, int posIRR);
int DOWASIM_loadKeyFile(string fn, MatrixDouble& keyMat);




void receiveInflowMat_orRead(MatrixDouble& mat_InflowMedian);
void writeInflowToFile(MatrixDouble& infl1, char*  filename);

// Both Functions call COMMUNICATE_ALL_DATA, to ensure mirror order of data exchange
// flag_SENDorRECEIVE == true  : SEND
// flag_SENDorRECEIVE == false  : RECEIVE

void receiveRaster3D_couplingData(Raster3D& raster3D,  bool RECEIVE_TDT, bool doprocessing, bool isIrrig);
//void sendViaTDT_Raster3D_couplingData( bool RECEIVE_TDT, bool doprocessing, bool isIrrig);

int receiveIntFromController(void);
void sendIntToController(int val);


string updateFilenamesWithContType(Raster3D& raster3D, string irrigYN);

// ------------------------------------------------------------
//    Functions around COUPLING
// <-- end
// ------------------------------------------------------------


#endif

 
#endif