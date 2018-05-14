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
//  File:       RandomDataHandler.h (formerly lotterie.h)
//
//  Contents:   Random-data generator class for stepwise distribution functions
//
//
//
//------------------------------------------------------------------------------


#ifndef _RandomDataHandler_h
#define _RandomDataHandler_h
#include "BasicDefines.h"
class MatrixDouble;

double rndDataGenerator(int stages, double* upps, double* vals);
double ran1(long idum);

#endif
