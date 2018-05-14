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
//  File:       CropWater.h (formerly part of agentF.h)
//
//  Contents:   Header of the water-stress crop growth model CROPWAT
//
//
//
//------------------------------------------------------------------------------

#ifndef _CropWater_h
#define _CropWater_h

#ifdef ODB
//	#include "MpmasGlobalFunctions.h"
#else
	#include "BasicData.h"
#endif
#include "MatrixDouble.h"

#ifndef LP9
	#include "AgentMipProblem.h"
#else
	#include "lp9.h"
#endif


///Auxiliary class
class InfoHydro{  
public:
	///@name Data query functions
	//@{
	int getColumnInLP()const;
	int getPriceColumnInLP()const;
	int getYieldRowInLP()const;
	int getSoilTypeID()const;
	int getIrrigationPriorityGroup()const;
	int getCropIrrigationType()const;
	int getIrrigationMethod()const;
	int getFormOfStressCoefficientKR()const;
	double getWaterStressCoefficientKY()const;
	double getCropYieldPotential()const;
	double getCropYieldStartValue()const;
	double getMonthlyKcValue(int m)const;
	//double getPlantWaterDemandsGrossInLiterPerSecond(int m)const; ## Tbe 111202
	//double getWaterDemandsGrossInOriginalUnit(int m)const; ## Tbe 111202
	//@}
	
	///@name Read, write and copy functions
	//@{	
	void copyRhsToThis(const InfoHydro& rhs);
	void printToScreen();
	void writeToFile(ofstream& outStream);
	bool readFromFile(FILE* fileStreamCrop, int numIrrigationPeriods);

	void allocate();
	void initialize(double initVal);
	void write(FILE* strm);
	//@}

	///@name Constructors and destructor
	//@{		
	InfoHydro();
	InfoHydro(double initVal);
	InfoHydro(const InfoHydro&);
	InfoHydro& operator=(const InfoHydro&);
	~InfoHydro();
	//@}

private:
	 ///activity column in LP
	int columnInLP;
	///index of selling activity
	int priceColumnInLP;
	///index of yield constraint
	int yieldRowInLP;
	///soil type
	int soilTypeID;

	///irrigation-related info
	int irrigationPriorityGroup;
	int cropIrrigationType;	//formerly "LP activity type"
	int irrigationMethod;
	int formOfStressCoefficientKR;
	double waterStressCoefficientKY;
	
	///crop yields
	double cropYieldPotential;
	double cropYieldStartValue;
	
	///Kc-values per crop-water activity
	MatrixDouble monthlyKcValues;

	///Water demands per crop-water activity (without effective rainfall)
	//MatrixDouble waterDemandsGross; ## Tbe 111202
};


///Class CropWater

/** 
 * This crop-related class implements the FAO CropWat model and computes
 * crop yields under water stress.
 */

class IrrigationMethodSector;
 
class CropWater
{
public:

	///@name Water-related crop growth functions
	///compute effective precipitation based on USDA/SCA method (see EDIC-CEDEC)
	virtual double computeCropEffectivePrecipitation(double actualRainfall, double monthlyWaterDemand);

	///compute monthly crop water demands in sector("effective demands")
	virtual void computeCropPotentialEvapotranspirationInMonth //must be called first
			 (int m,  double potentialETrInCurrentMonth,
			 MatrixDouble& vectorOfMonthlyCropPotentialEtr);
	virtual void computeCropEffectivePrecipitationInMonth //must be called second
			 (int m, double actualRainfall, 
			 const MatrixDouble& potentialEvapotranspiration,
			 MatrixDouble& vectorOfMonthlyCropEffectivePrecipitation);
	virtual void computeCropIrrigationWaterDemandsInMonth //must be called third
			(int m,   int secID_,
			const IrrigationMethodSector& sectorparamsIrrigationMethod, 
			const MatrixDouble& effectivePrecipitation,
			const MatrixDouble& potentialEvapotranspiration,
			MatrixDouble& vectorOfMonthlyCropIrrigationWaterDemands);

	///compute irrigated area per irrigation method and return irrigation demand
	virtual double computeIrrigatedAreaPerMethodAndTotalIrrigationWaterDemand(int m,
			const MatrixDouble& cropWaterData,
			const MatrixDouble*& irrigationWaterDemands,
			MatrixDouble& vectorOfIrrigatedAreaByMethod);

	///allocate irrigation water to crops according to priority groups
	virtual double allocateIrrigationWaterToCropsInMonth
			(int m,
			double*& D_sat,
			const  MatrixDouble*& irrigationWaterDemands,
			const MatrixDouble& cropWaterData,
			MatrixDouble& irrigationWaterDemandsPerPriorityGroup,
			MatrixDouble& percentageIrrigatedPerPriorityGroup,
			MatrixDouble& irrigationWaterSuppliedToCrops);

	///compute crop yields under water stress
	virtual void computeWaterDeficitsOfAllCrops
			(const MatrixDouble& cropWaterData,
			const MatrixDouble*& effectivePrecipitation,
			const MatrixDouble*& potentialEvapotranspiration,
			const MatrixDouble*& floodFactor,
			const MatrixDouble& irrigationWaterSuppliedToCrops,
			MatrixDouble& waterDeficitsOfCropsAsMonthlyKR);
	virtual void computeYieldReductionUnderWaterStress 
			(MatrixDouble& cropWaterData,
			MatrixDouble& waterDeficitsOfCropsAsMonthlyKR);
		//@}


	//---------------------------------------------------------------------------

	///@name Data query functions
	//@{	
	///get info on water-related cropping activities
	virtual int getNumWaterActivities();
	virtual int getNumIrrigationPriorityGroups();
	virtual int getNumWaterMonths();
	
	virtual int getLpColumnForActivityIdx(int arrayIndex);
	virtual int getIrrigationTypeForActivityIdx(int arrayIndex);
	virtual int getZeroLaborActivity(int soilType);
	
	virtual int getActivityIdxForLpColumn(int _colLP);
	virtual int getIrrigationTypeForLpColumn(int _colLP);

   //virtual double getPlantWaterDemandsGrossInLiterPerSecond(int arrayIndex, int m_);// ## Tbe 111202
	virtual int getSoilTypeID(int arrayIndex);
	
	virtual InfoHydro getInfoOnActivityCropHydrology(int arrayIndex);
	const InfoHydro* getPointerToInfoOnActivityCropHydrology(int arrayIndex);

	///Functions related to farm surveys
	virtual int getLpColumnOfFirstCropActivity();
	virtual int getLpColumnOfLastCropActivity();
	virtual int checkSequenceOfCropActivitiesInLp();
	//@}
	
	//---------------------------------------------------------------------------
	
	///@name Data input functions
	//@{	
	///Function passes input filename to crop growth module
	virtual void readInputFromFile(string fn);
	virtual void writeInputIntoFile();

	
	//@}

	//---------------------------------------------------------------------------

	///@name Constructors and destructors
	//@{
	///Constructor 
	CropWater();

	///Copy constructor
	//CropWater(const CropWater& cropWater_);

	///Assignment operator
	//CropWater& operator=(const CropWater& cropWater_);

	///Destructor
	virtual ~CropWater()
	{	delete [] waterActivities;
		waterActivities = NULL;
		waterActivityLookup.deallocate();
		zeroLaborActivities.deallocate();
	}
	//@}
	
	
protected:
	///@name Protected member data
	//@{
	///Number of water-related cropping activities
	int numWaterActivities;
	
	///Number irrigation priority groups
	int numIrrigationPriorityGroups;

	///Number of months with possible water stress
	int numWaterMonths;

	///Type of equation used for effective rainfall
	int equationTypeEffectiveRainfall;

	///Parameters of effective rainfall function (Type 0)
	double effRainCoeffConstant;		// 1.2995160
	double effRainCoeffDemand;			//-0.0348437
	double effRainCoeffDemandSquared;// 0.0001663
	double effRainCoeffRain;			// 0.6439529
	double effRainCoeffRainSquared;	//-0.0004772
	double effRainCoeffCross;			// 0.0010454	
	
	///Water-related information for each cropping activity
	InfoHydro* waterActivities;//formerly 'actHydro' or 'actHydroVector'

	///vector over all LP columns, containing the index of corresponding water activity
	MatrixDouble waterActivityLookup;

	///vector of LP activity indices for zero-labor crops
	MatrixDouble zeroLaborActivities;

	//@}

	///@name Protected member functions
	//@{
	///Compute crop growth reduction factor
	//double growthRedFunction(double ky, double kr);//substitute
	double computeCropWaterYieldReductionFactor(double ky, double kr);//Tbe 080220
	
	//@}
};


#endif
