#ifndef _postprocessing_h

#define _postprocessing_h

#include <string>
#include "Raster3D.h"

using namespace std;  // std::stream  --> stream (simpler)

enum VarType { dailyVar, statVar, monthlyVar, noneVar};


int ROWS();
int COLS();
void setROWS(int r);
void setCOLS(int c);

string getCELLINFO();
void setCELLINFO(string str);

//string getEXT(int i);
// Extension
string getEXT();
void setEXT(string str, int i);
void setEXT(string str);

string getVAR(int i);
void setVAR(int i, VarType  type, string str, string ext);



   void POST_printToScreen_InputOptions();
void POST_inputArgumentHandling(int ac, char **av);

void deletePOST();

int makeTableHeaderWithDates(MatrixDouble& posMat, string fn, double yshift, double xshift, double cellsize, int numSTARTCOLS);
int makeTableHeaderWithDates(MatrixDouble& posMat, char* d, double yshift, double xshift, double cellsize, int numSTARTCOLS);

int makeHeaderAll(MatrixDouble& posMat, Raster2D& gisIRR);

void makeGlobalFilenames(int varID);
	   
#endif



