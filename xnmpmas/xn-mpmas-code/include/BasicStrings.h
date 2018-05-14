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
//  File:       BasicStrings.h (formerly init_strings.h)
//
//  Contents:   Auxiliary class, specific string handling
//
//
//
//------------------------------------------------------------------------------


#ifndef _BasicStrings_h
#define _BasicStrings_h


#include "BasicDefines.h"
#ifndef ODB

#include <string.h>
#include <string>
using namespace std;  // std::stream  --> stream (simpler)

#include "BasicEnums.h"
// Contains string headers for
//       file_exists(string filename),
//       chk_dir(string dirName)


#include "BasicStreams.h"


// Contains #define


void set_GISOUTDIR(string out);
string GISOUTDIR();

void set_MASINPUTDIR(string out);
string MASINPUTDIR();

void set_MASOUTPUTDIR(string out);
string MASOUTPUTDIR();


//Handling of filenames
//
//All filenames are handled by functions
//	- void initFILENAMES()
//	- void set_FILENAMES(KEYTYPE key, string s)
//	- string FILENAMES(KEYTYPE key)
//Access to FILENAMES is done versus en enum-type "KEYTYPE", in "enum_def.h"
//
//To access a filename as a char, use a copy command, z.B.
//	char d[200];
//	sprintf(d, "%s", FILENAME(keyDATAINI).c_str())
//
//vThere should be no filenames in the code any more!
//
//To add a filename, simply define new KEYTYPE ("key####") and add to initFILENAMES
// ALL THESE ARE INITIALIZED FOR WASIM COUPLING (InflowsKey.txt)
// -->
void set_ABSTRFILE_fn(string s);
string ABSTRFILE_fn();

// Name of directory for WASIM calibration mode
void set_WASIM_CNTRL(string s);
string WASIM_CNTRL();


////////////////////////////////////////////////////////////////////////
// --> Content type functions
// =====================================================================




// Indicates which data should be passed via TDT
ExportType getExpTpFromContent(Content cont);

string getEXPSTRING(ExportType expTP);

// renaming from above, no new functions
ExportType cont2exptp(Content cont);
Content exptp2cont(ExportType expTP);
Content getContFromExpTP(ExportType expTP);


// Directory of KEY files (iniFiles)
string KEYDIR();
void set_KEYDIR(string out);

// Destination for saving GIS files passed by controller
string OUTGIS();
void set_OUTGIS(string outg);

void set_OUTDIR(string outStr);
string OUTDIR();

void set_SIMNAME(string outStr);
string SIMNAME();

void set_XMLDIR(string outStr);
string XMLDIR();


void set_TEMPDIR(string outStr);
string TEMPDIR();

void set_INDIR(string outStr);
string INDIR();


string ISIRR(void);
bool DOIRR(void);
void set_ISIRR(string out);
void set_ISIRR(int out);

string SECTOR_MAP_NAME();
void set_SECTOR_MAP_NAME(string strg);

string GIS_FOLDER_NAME();
void set_GIS_FOLDER_NAME(string strg);

string PATH_ACT_PRECIPITATION(); // Tbe 120212
void set_PATH_ACT_PRECIPITATION(string strg);

string PATH_POT_EVAPOTRANSPIRATION(); // Tbe 120212
void set_PATH_POT_EVAPOTRANSPIRATION(string strg);

string PATH_ACT_PRICES(); // Tbe 120212
void set_PATH_ACT_PRICES(string strg);

string ADD_OUTPUT_NAME(); // Tbe 120220
void set_ADD_OUTPUT_NAME(string strg);

string StringToLower(string strToConvert);
string StringToUpper(string strToConvert);

#endif//ODB
#endif
