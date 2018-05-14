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
//  File:       MainAuxiliaryFunctions.h (formerly main_fkt.h)
//
//  Contents:   Auxiliary class, contains some outsourced functions of main()
//
//
//
//------------------------------------------------------------------------------

#ifndef _MainAuxiliaryFunctions_h
#define _MainAuxiliaryFunctions_h
#include "BasicDefines.h"
#include "BasicEnums.h"
#include <string>
using namespace std;  // std::stream  --> stream (simpler)

#ifndef ODB
//write only non-zero values to file
void set_WRITE_MPX(bool b_);
bool WRITE_MPX();          

//save all agent MIPs to file
void set_SAVE_ALL_MTX(bool b_);
bool SAVE_ALL_MTX();          

// Some initialization functions
void printToScreen_InputOptions();
void printToScreen_TestFunOptions();
void MpmasMain_inputArgumentHandling(int ac, char **av);

void MpmasMain_InitializeHydrologicalModel(); //ODB TODO
void MpmasMain_InitializeExternalModel();
#endif //ODB
// ... calls global time handler
int GETPERIODE();

int getNumberOfNoneZeroRows();
int getNumberOfNoneZeroCols();

// ------------------------------------------------------------
//    Functions MEMORY / DEALLOCATION
// -->
// ------------------------------------------------------------

void fehlerabbruch();      //writes error message and exits program
void writeToErrorFile(char* fehlertxt_); // Writes semething into err file
extern char fehlertxt[];
#ifndef ODB
	void FREE_MEMO();          //free memory
#endif
//some warning/error handling
void writeWarningMsgFile();//writes warning message into file (WIN32)


// ------------------------------------------------------------
//    LEARNING module
// ------------------------------------------------------------
#ifndef ODB
void LAMBDA_init(double initialLambda);
double LAMBDA(int type);//Parameter for expectations (Brandes et al., 1997: 382)
void SET_LAMBDA(int type, double value);

// For varying lambda: read values from file
string LAMBDA_filename();
void set_LAMBDA_filename(string s);

// Lambda i/o functions
void writeLambdaToFile(string s);
string getLambdaAsSting();
void readLambdaFromFile();

//Special file to deactivate sectors
string getInactiveSectorsFilename();
void setInactiveSectorsFilename(string fn);

void expectFileDelimiter(FILE* strm, string delim, string filename, string extraInfo);
#endif // ODB
#endif
