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
//  File:       CatchmentSectorAuxiliary.h (formerly part of sector.h)
//
//  Contents:   Class for irrigation methods and canal efficiency (EDIC)
//
//
//
//------------------------------------------------------------------------------


#ifndef _Catchment_DynamicRiverFlows_h
#define _Catchment_DynamicRiverFlows_h

#include "BasicDefines.h"
#ifdef ODB
	#include "MpmasGlobalFunctions.h"
#else
	#include "BasicData.h"
#endif
#include"MatrixDouble.h"


//----------------------------------------------------
/// Functions for handling irrigation information
//----------------------------------------------------



class DynamicRiverFlows
{
   public:
 	//----------------------------------------------------------------------------------
	///@name Read - Write functions
	//@{
      virtual void read_DynamicRiverFlows(string fn);
      virtual void writeToFile(string fn);
	//@}
	//----------------------------------------------------------------------------------
	///@name Access functions
	//@{
      double getFlowForRiverAndYearAndMonth(int riverID_, int y_, int m_, bool isNorthern_);

      virtual void setFlowForRiverAndYearAndMonth(int riverID, int yearID, int modelMonth, int valFlowM3);
      virtual void setRiver_fullYear(int riverID, int yearID, MatrixDouble& vectorThisYearFlows);
      virtual void getRiver_fullYear(int yr_, MatrixDouble& N_forecast, MatrixDouble& vec_InflowID_);

   // nothing in base class!
//

   //@}
	//----------------------------------------------------------------------------------
	///@name Print functions
	//@{
//


	//@}
	//----------------------------------------------------------------------------------
	///@name Constructor etc
	//@{

      DynamicRiverFlows();
      DynamicRiverFlows(int minYear_, int numYears_, int numFlows_, bool isNorthern_)
      {  allocate(minYear_, numYears_, numFlows_, isNorthern_);
      }

      DynamicRiverFlows(DynamicRiverFlows& dynamicRiverFlows_)
      {  copy(dynamicRiverFlows_);
      }

      virtual ~DynamicRiverFlows();
      
      virtual void copy(DynamicRiverFlows& dynamicRiverFlows_);
      virtual void copy_N_full(DynamicRiverFlows& dynamicRiverFlows_);

      virtual void allocate();
      virtual void allocate(int minYear_, int numYears_, int numFlows_, bool isNorthern_);
      virtual void deallocate();
      virtual void deallocate_N_full();

	//@}
	//----------------------------------------------------------------------------------

      MatrixDouble* N_full;
      MatrixDouble  yearsVector, riverIdsVector;
      int minYear ; // First year with data. Last year is (minYear + numYears) 
      int numYears;
      int numFlows;
      bool isNorthern;
};

#endif
