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
//  File:       BasicMath.h (formerly part of funktio9.h)
//
//  Contents:   Basic mathematics functions as used by agents
//
//
//
//------------------------------------------------------------------------------


#ifndef _BasicMath_h
#define _BasicMath_h

#include <math.h>
#include "BasicDefines.h"

//truncating functions
double round1(int, double);
int round1(double&);//improved version with error flag
double round2(double, double);

//change mode of floating point unit
void set_fpu(unsigned int mode);

//returns first floating point rounded to a multiple of the second floating point
double roundToMultipleOf(double valueToClean, double baseOfMultiple);

// Folgende aus allgem9.h
int min2(int x, int y);
double min2(double x, double y);
int max2(int x, int y);
double max2(double x, double y);
double maxVector(double* x, int len);
double minVector(double* x, int len);
int maxPos(double* x, int len);
int minPos(double* x, int len);
double sumVector(double* x, int len);


// Folgende aus funktio9.h
// agentF agentL aliste bwmarkt MatrixDouble
int max(int a, int b);
// agentF agentL aliste bwmarkt MatrixDouble  sim_main allgem9
int min(int a, int b);
double max(double a, double b);
double min(double a, double b);

// -
int excel_modulo(int n, int d);

//sets absolute value of double to minimum if needed
double checkMinForFabs(double value, double minimum);

// returns sign of value, branchless, type-safe C++ version
template <typename T> int sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}

/* see http://stackoverflow.com/questions/1903954/is-there-a-standard-sign-function-signum-sgn-in-c-c
template <typename T> inline constexpr
int signum(T x, std::false_type is_signed) {
    return T(0) < x;
}

template <typename T> inline constexpr
int signum(T x, std::true_type is_signed) {
    return (T(0) < x) - (x < T(0));
}

template <typename T> inline constexpr
int signum(T x) {
    return signum(x, std::is_signed<T>());
}
*/
// coordinates
struct coord  {
	int x; // = column
	int y; // = row
	coord(int _x, int _y) : x(_x) , y(_y) {};
} ;

#endif 
