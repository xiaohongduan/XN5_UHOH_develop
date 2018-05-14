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
//  File:       BasicFunctions.h (formerly part of allgem9.h)
//
//  Contents:   Contains data classes and functions such as GIS-related
//              data structures
//
//
//------------------------------------------------------------------------------


#ifndef _BasicFunctions_h
#define _BasicFunctions_h

#include "BasicDefines.h"
#ifdef ODB
//	#include "MpmasGlobalFunctions.h"
#if (defined(LP9) || defined(USE_OSL))
	#include "ekk_c_api.h"
#endif//USE_OSL
#else
	#include "BasicData.h"
#endif//ODB

#include <string>
#include <sstream>
#include <iostream>


//convert a string into a numeric type
template <class T>
bool from_string(T& t, 
                 const std::string& s, 
                 std::ios_base& (*f)(std::ios_base&))
	// the third parameter of from_string() should be 
	// one of std::hex, std::dec or std::oct
{
  std::istringstream iss(s);
  return !(iss >> f >> t).fail();
}





/*
If reason=1, function is called after a dual iteration of EKKSSLV.
If reason=3, function is called after a valid refactorization, and before a primal iteration in EKKSSLV.
If reason=4, function is called after a valid refactorization, and before a dual iteration in EKKSSLV.
If reason=5, function is called after a primal iteration of EKKBSLV.
If reason=6, function is called after a primal iteration of EKKQSLV.
If reason=8, function is called just after entering EKKQSLV.
If reason=9, function is called just after entering EKKQPAR and after each optimal solution is found by EKKQPAR.
If reason=10, function is called after an EKKSPAR iteration finds an optimal solution at an increment point.
If reason=11, function is called after an EKKSPAR iteration finds an optimal solution at a basis change between increment points.
If reason=12, function is called after a primal-dual iteration of EKKBSLV.
If reason=13, function is called after a primal iteration of EKKNSLV.
If reason=14, function is called after a dual iteration of EKKNSLV.
If reason=15, function is called after a major iteration of the first subalgorithm of EKKQSLV, i.e. after the solution of one of the successive linearization and the definition of the next. Because values have not been updated either in dspace or in or in the control variable arrays, very little useful information about the progress of the solution process is available when EKKITRU is called with reason = 15.
If reason=16, function is called after each pivot in the second subalgorithm of EKKQSLV. Because values have not been updated in either dspace or in the control variable arrays, very little useful information about the progress of the solution process is available when EKKITRU is called with reason = 16.

If userrtcd ("status") =3, the result is as if the maximum number of iterations had been performed.
If userrtcd=99, If called from EKKSSLV, a new factorization of the MatrixDouble (reinvert) is performed.
Note: If the userrtcd is unchanged from its on entry value, processing continues.

*/
#if (defined(LP9) || defined(USE_OSL))
void OSLLINKAGE_CB itruCallback(EKKModel* model, int reason, int* status);
#endif
#endif
