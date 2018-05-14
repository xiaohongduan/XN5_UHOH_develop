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


#ifndef _CatchmentSectorAuxiliary_h
#define _CatchmentSectorAuxiliary_h

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


typedef struct
{
   string methodName;
   double methodSurfaceRunoffsNight;
   double methodPlantWaterAvailability;
   double methodSurfaceRunoff;
   double methodLateralFlows;
   double methodDeepPercolation;
   double sum;
}  irrigationMethodData;

class IrrigationMethod
{
   public:
 	//----------------------------------------------------------------------------------
	///@name Read - Write functions
	//@{	
      virtual int readAllIrrigationMethods(int catchmentID_);
      virtual void writeIrrigationMethods(string filename);
      virtual void writeIrrigationMethods(ofstream& out);
      virtual void writeHeader(ofstream& out);
      virtual void writeLineToFile(int i, ofstream& out);
      
	//@}
	//----------------------------------------------------------------------------------
	///@name Access functions
	//@{	
      virtual irrigationMethodData getIrrigationMethodData(int numMethod);
      virtual double getFlowShareByType(int numMethod, TypeEdicFlow typeFlow);
      virtual double getPlantShare(int numMethod);
      virtual double getPlantShare(int numMethod) const;
      virtual void setFlowShareByType(int numMethod, TypeEdicFlow typeFlow, double val);
      virtual string getMethodName(int numMethod);

      // nothing in base class!
      virtual void setRestOfValues(int numMethod, TypeEdicFlow typeFlow, double val);
      virtual double getRestOfFlows(int numMethod, int typeFlow) ;
      
      int getNumMethods(){return numMethods;};
      int getCatchmentID(){return  catchmentID;};
      virtual int getMaxType();
   //@}
	//----------------------------------------------------------------------------------
	///@name Print functions
	//@{	
      virtual void printToScreen();
      virtual void printHeader();   
      virtual void printLineToScreen(int i);

            
	//@}
	//----------------------------------------------------------------------------------

      IrrigationMethod();      
      IrrigationMethod(int catchmentID_, int numMethods);
      IrrigationMethod(IrrigationMethod& copyClass);

      virtual ~IrrigationMethod();

      void initialize(IrrigationMethod& copyClass);
      void allocate();
      void allocateSpecific(int catchmentID_, int numMethods_);
      virtual void deallocate();

   protected:
      irrigationMethodData* IMdata;
   private:
      int catchmentID;
      int numMethods;

};

class IrrigationMethodSector : public IrrigationMethod
{
 public:

 	//----------------------------------------------------------------------------------	
	///@name Print, read and write functions
	//@{
      virtual void printToScreen_Sector();

      virtual void appendToFile_Sector(string fn);
      virtual void writeToFile_Sector(string fn);
      virtual void writeIntoStream_Sector(ofstream& out);
	//@}
 	//----------------------------------------------------------------------------------
	///@name Access functions
	//@{
      virtual double getB(typeIrrigationMethod);
      virtual double getG(typeIrrigationMethod);
      virtual double getL(typeIrrigationMethod);
      virtual double getCanalLosses(int numMethod);
		//Access plant water availability through base class, function:
		//virtual double getPlantShare(int numMethod);
	//@}
 	//----------------------------------------------------------------------------------

	///@name Computations
	//@{
         virtual void  computeShrinkedProportionalFlows(double canalEfficiency);
         virtual void  computeCoefficientsBGL();
	//@}

	//----------------------------------------------------------------------------------
      virtual void setRestOfValues(int numMethod, TypeEdicFlow typeFlow, double val);
      virtual double getRestOfFlows(int numMethod, int typeFlow); // nothing in base class!
      virtual int getMaxType();

      IrrigationMethodSector();
      IrrigationMethodSector(IrrigationMethod& IMcatchmentLevel, double canalEfficiency_, double beta_share);
      virtual ~IrrigationMethodSector();

      // Pendant to constructor
      void initializeInherited(IrrigationMethod& IMcatchmentLevel, double canalEfficiency_, double beta_share);
      void deallocate();

 private:
       // Actual construction of inherited class (both constructor and inherited)
      void updateDuringInitialize(double canalEfficiency_, double beta_share);
      void allocateInherited();

      double canalSurfaceShare;
      double canalEfficiency;
      double canalLateralLoss;
      double canalDeepPercolation;
      MatrixDouble coeffB_surface;
      MatrixDouble coeffG_lateral;
      MatrixDouble coeffL_losses;
};           

void testIrrigationMethodAll(IrrigationMethod IM, double channelEfficiency, double betaShare, string fn);

#endif
