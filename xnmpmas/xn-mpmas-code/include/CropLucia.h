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
//  File:       CropLucia.h
//
//  Contents:   Header of the crop growth model LUCIA
//
//
//
//------------------------------------------------------------------------------

#ifndef _CropLucia_h
#define _CropLucia_h


#ifdef ODB
	//#include "MpmasGlobalFunctions.h"
#else
	#include "BasicData.h"
#endif
#include "MatrixDouble.h"
#include <vector>
#include <map>


using namespace std;


struct assetRequiredForCrop {
		int objId;
		int minAge;
		int maxAge;
		assetRequiredForCrop() : objId(-1), minAge(-1), maxAge(1000) {}
		assetRequiredForCrop(int _id, int _min, int _max) : objId(_id), minAge(_min), maxAge(_max) {}
};
//---------------------------------------------------------------------------------------
///Auxiliary class
//---------------------------------------------------------------------------------------
class InfoLuciaCrops{  
public:
	///@name Data query functions
	//@{
	int getCropActivityID()const;
#ifdef MULTIPERIOD
	int getCropManagementID() const { return cropManagementID;}
// Troost 20160331 Soil type transitions
// Troost **********begin block insert**********
	int getNewSoilTypeAfterHarvest() const	{		return newSoilTypeAfterHarvest;	}
// Troost **********end block insert**********
#endif
	int getSoilTypeID()const;
	int getCropType()const;
	int getPriority()const;
	int getOutputID()const;
	double getInitialCropYield()const;
	double getInitialStoverYield()const;
	//@}
	
// Troost 20130813 Exogenous Yield Expectations (Country Model Germany)
// Troost **********begin block insert**********
	double getExogenousCropYieldExpectation();
	double getExogenousStoverYieldExpectation();
// Troost **********end block insert**********

	int isLandUseForUnownedLand();
#ifdef MULTIPERIOD
	 double getExtraAttribute(size_t index) const;
	void setExtraAttribute(size_t index, double value);
#endif

	///@name Read, write, and copy functions
	//@{	
	void copyRhsToThis(const InfoLuciaCrops& rhs);
	void printToScreen();
	void writeToFile(FILE* strm);
	void readFromFile(FILE* strm, size_t numExtraChars = 0);

	///reads and writes only crop and stover yields
	void writeYieldsToFile(FILE* strm);
	void readYieldsFromFile(FILE* strm);

	///set crop and stover yields
	void setCropYield(double yield);
	void setStoverYield(double yield);
	//@}

// Troost 20130813 Exogenous Yield Expectations (Country Model Germany)
// Troost **********begin block insert**********
	void readExogenousYieldExpectationsFromFile(FILE* strm, size_t numExtra = 0);
// Troost **********end block insert**********

#ifdef MULTIPERIOD
	void addRequiredAsset(int tempId, int tempMinAge, int tempMaxAge) { requiredAssets.push_back(assetRequiredForCrop(tempId, tempMinAge, tempMaxAge)) ;}
	vector<assetRequiredForCrop> getRequiredAssets() { return requiredAssets ;}
#endif

	///@name Constructors and destructor
	//@{		
	InfoLuciaCrops();
	InfoLuciaCrops(const InfoLuciaCrops&);
	InfoLuciaCrops& operator = (const InfoLuciaCrops&);
	~InfoLuciaCrops();
	//@}

private:
	///crop activity ID
	int cropActivityID;

#ifdef MULTIPERIOD
	int cropManagementID;
// Troost 20160331 Soil type transitions
// Troost **********begin block insert**********
	int newSoilTypeAfterHarvest;
	vector<assetRequiredForCrop> requiredAssets;

// Troost **********end block insert**********
#endif

	///soil type
	int soilTypeID;

	///crop type
	int cropType;
	
	///priority for spatial allocation (reflects closeness to farmstead)
	int priority;

	///output ID
	int outputID;

	///initial crop yield
	double initialCropYield;
	
	///initial stover yield
	double initialStoverYield;

// Troost 20130813 Exogenous Yield Expectations (Country Model Germany)
// Troost **********begin block insert**********
	double exogenousCropYieldExpectation;
	double exogenousStoverYieldExpectation;
// Troost **********end block insert**********
#ifdef MULTIPERIOD
	int thisIsLandUseForUnownedLand;

	vector<double> extraAttributes;
#endif
};

//---------------------------------------------------------------------------------------
///Auxiliary class
//---------------------------------------------------------------------------------------
class InfoLuciaNRUs{  
public:
	///@name Data query functions
	//@{
	int getColumnInLP()const;
	int getCropYieldRowInLP()const;
	int getStoverYieldRowInLP()const;
	//@}
	
	///@name Read, write and copy functions
	//@{	
	void copyRhsToThis(const InfoLuciaNRUs& rhs);
	void printToScreen();
	void writeToFile(FILE* strm);
	void readFromFile(FILE* strm);
	//@}

	///@name Constructors and destructor
	//@{		
	InfoLuciaNRUs();
	InfoLuciaNRUs(const InfoLuciaNRUs&);
	InfoLuciaNRUs& operator = (const InfoLuciaNRUs&);
	 ~InfoLuciaNRUs();
	//@}

private:

	///activity column in LP
	int columnInLP;	
	///index of crop yield constraint
	int cropYieldRowInLP;
	///index of stover yield constraint
	int stoverYieldRowInLP;


};



//---------------------------------------------------------------------------------------
///Class CropLucia
//---------------------------------------------------------------------------------------

/** 
 * This crop-related class implements the LUCIA model and computes
 * crop yields based on external yield maps.
 */


class CropLucia
{  
	public:

	///@name Function returns crop yields that were read from external file
	///get crop yields
   virtual double getCropYield(int arrayIndex);
	virtual double getStoverYield(int arrayIndex);
	//@}

// Troost 20130813 Exogenous Yield Expectations (Country Model Germany)
// Troost **********begin block insert**********
	virtual double getExogenousCropYieldExpectation(int arrayIndex);
	virtual double getExogenousStoverYieldExpectation(int arrayIndex);
// Troost **********end block insert**********
	//---------------------------------------------------------------------------
	
	///@name Data query functions
	//@{	
	///get info on cropping activities
	virtual int getMaxNumberCropsPerSoilType();
   virtual int getTotalNumberCropActivities();
   virtual int getNumberCommunicatedYieldMapsPerPeriod();

	///searches in InfoLuciaCrop
	virtual int getLuciaArrayIndexForCropActivityID(int cropActID);//cropActID = unique ID in crop input file
	virtual int getCropActivityIdForLuciaArrayIndex(int arrayIndex);//arrayIndex = index in data structure
#ifndef MULTIPERIOD
	virtual int getLuciaCropActivityIdForCropAndSoilType(int actIdx, int soilType);//index within soil type
	virtual int getLuciaArrayIndexForCropAndSoilType(int actIdx, int soilType);//index within soil type
#endif //ndef MULTIPERIOD
#ifdef MULTIPERIOD
	int getCropManagementIdForCropActivityID(int cropActId);
	int getCropManagementIdForLuciaArrayIndex(int arrayIndex);
	int getSoilTypeIdForLuciaArrayIndex(int arrayIndex);
	int getNewSoilTypeIdAfterHarvestForLuciaArrayIndex(int arrayIndex);
	vector<assetRequiredForCrop> getAssetsRequiredForLuciaArrayIndex(int arrayIndex);
	int getCropManagementIdForYldArrayIndex(int YldIndex);
	int getLuciaArrayIndexForCropManagementIdAndSoilType(int managId, int soilType);//index within soil type
	int getLuciaArrayIndexForYieldArrayIndexAndSoilType(int yldArrayIdx, int soilType);
#endif

	virtual InfoLuciaCrops getInfoOnCropActivity(int arrayIndex);
	const InfoLuciaCrops* getPointerToInfoOnCropActivity(int arrayIndex);
	InfoLuciaCrops* getMutablePointerToInfoOnCropActivity(int arrayIndex);

#ifndef MULTIPERIOD
	///searches in InfoLuciaNRUs
	virtual int getLuciaNruIndexForLpColumn(int colLP, int& crA, int& nru);//returns if found

	virtual InfoLuciaNRUs getInfoOnNruActivity(int nruIdx, int nru);
	const InfoLuciaNRUs* getPointerToInfoOnNruActivity(int nruIdx, int nru);

	///Note: array index in Lucia soil and Lucia NRUs must match
	virtual int getLuciaInternalIndexForZeroLaborActivity(int soil);
	virtual int getLuciaArrayIndexForZeroLaborActivity(int soil);


	///Functions related to farm surveys
	virtual int getLpColumnOfFirstCropActivity();
	virtual int getLpColumnOfLastCropActivity();
	virtual int checkSequenceOfCropActivitiesInLp();
	virtual void insertCropActivityLpColumnsAndActIDs(MatrixDouble& tableLpColsAndActIDs);
	//@}
#endif //ndef MULTIPERIOD


#ifdef MULTIPERIOD
	int getCropActivityForUnownedLand(int type) { return landUseForUnownedLand[type] ;}
	const vector<int>* getPointerToLandUseForUnownedLandVector() { return &landUseForUnownedLand ;}
#endif

	//---------------------------------------------------------------------------
	
	///@name Data input functions
	//@{	
	///Function passes input filename to crop growth module  
	virtual void readInputFromFile(char* filename);
   virtual void writeInputIntoFile(char* filename);
	virtual void printToScreen(void);

	///Functions reading/writing look-up table for crop yields
	virtual void readCropYieldsFromFile(char* filename);
	virtual void readCropYieldsFromFile(FILE* openStream);
   virtual void writeCropYieldsIntoFile(char* filename);

	///Function passes crop yields directly
	virtual void setCropYields(const int numCropActs, const int* cropActID, const double* cropYield, const double* stoverYield);
		
#ifdef MULTIPERIOD
	map<int,int>* getMappingCropManagementIdToArrayIndex() {		return &mapCropManagementToArrayIndex;	}
	map<pair<int,int>,int >* getMappingCropManagementIdSoilTypeToArrayIndex() { 	return &mapCropManagementAndSoilTypeToArrayIndex;}
	void getCopyOfArrayOfLandUseActivityIDs(int* arrayToCopyInto){	for (int i = 0; i < totalNumberCropActivities; i++) 	arrayToCopyInto[i] = cropActivities[i].getCropActivityID(); 	}

	int getYldArrayIndexForCropManagementID(int cmID);
	int getYldArrayIndexForCropActivityID(int cropActID);

	size_t getNumberExtraCropActAttributes(){return numExtraCropActAttributes;}
	int cropModelWantsManagementID() { return exportManagementID;}

#endif //MULTIPERIOD
	//@}

	//---------------------------------------------------------------------------

	///@name Constructors and destructors
	//@{
	///Constructor 	
	CropLucia(void)
	{  allocateMemory();
   }

	///Destructor
   virtual ~CropLucia()
	{	deallocateMemory();
	}
	//@}

	//---------------------------------------------------------------------------

protected:
	///@name Protected member data
	//@{
	///pointer to stream
	FILE* datStrm;

	///soil types (from basic data)
   int numberSoilTypes;

	///maximum number of crop activities per soil type
   int maxNumberCropsPerSoilType;

	///total number of crop activities
   int totalNumberCropActivities;

	///number of cropping seasons per year
   int numberCommunicatedYieldMapsPerPeriod;




	///data for all crop activities
	InfoLuciaCrops* cropActivities;
#ifndef MULTIPERIOD
	///LP indices for crop activities in each nutrient response units (NRU)
	InfoLuciaNRUs** lpIndicesForNRU;
#endif

#ifdef MULTIPERIOD
	vector<int> CropManagementIDs; //map from first dimension array index of cropManagement/NRU arrays (e.g. actYlds, expYlds) to CropManagementID
	map<int,int> mapCropManagementToArrayIndex; //map from cropmanagementID to first dimension array index of cropManagement/NRU arrays  (e.g. actYlds, expYlds)
	map<pair<int,int>, int> mapCropManagementAndSoilTypeToArrayIndex;//map from cropManagementID/Map Soil Type to ArrayIndex in internal cropActivities array
	map<int,int>mapLandUseActivityIndexToCropActivityArrayIndex;//map from land use activity ID (input file, crop growth model) to ArrayIndex in internal cropActivities array

	vector<int> landUseForUnownedLand;

	size_t numExtraCropActAttributes;
	int exportManagementID;
#endif
	//@}

	///@name Protected member functions
	//@{
	///
	virtual void allocateMemory(void);
	virtual void deallocateMemory(void);

	void returnFscan(int retVal, char* strVar);

	//@}
};


#endif
