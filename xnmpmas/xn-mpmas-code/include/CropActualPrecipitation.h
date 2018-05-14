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
//  File:       CropActualPrecipitation.h
//
//  Contents:   Auxiliary class for actual precipiation and potential ETr
//              
//
//
//------------------------------------------------------------------------------


#ifndef _CropActualPrecipitation_h
#define _CropActualPrecipitation_h

#ifdef ODB
//	#include "MpmasGlobalFunctions.h"
#else
	#include "BasicData.h"
#endif
#include "MatrixDouble.h"


///First auxiliary class
enum enumPrecipType
{	precipTypeActual, precipTypeAverage
};

enum typeInputDataActualPrecipitation
{	//inputTypeByYear, inputTypeRandom // ## Tbe 111201
	inputTypePrecipByYear, inputTypePrecipRandom
};

class CropPrecipitation
{

public:
	///@name Data query functions
	//@{
	virtual double getActualPrecipitationByYear(int year_, int month_, int catchment_, int sector_);
	virtual double getActualPrecipitationByIndex(int year_, int month_, int catchment_, int sector_);
	virtual int getIndexForAbsoluteYear(int year);
	virtual void printValue(int year_, int month_, int catchment_, int sector_);
	virtual void printValueByIndex(int yearIndex, int month_, int catchment_, int sector_);
	virtual double getAveragePrecipitationInMonth(int month_, int catchment_, int sector_);
	//@}

	///@name Print, write and read functions
	//@{
	virtual void printToScreen();
	virtual void writeToFile(string fn, int catchID);//with <ifstream>
	virtual void writeToFile(char* filename, int catchID);//with <fscanf>
	virtual void readHeader(FILE* strm, int catchID);
	virtual void readPrecipitationDataIntoMatrix(FILE* strm, MatrixDouble& matrixPassedByRef, int numYR_, int secID, enumPrecipType precipType, int catchID);
	virtual void readFromFile(char* filename, int catchID);//with <fscanf>
	//@}

	///@name Constructors and destructor
	//@{
	CropPrecipitation()
	{	allocate();
	}

	CropPrecipitation(int numCatchments_, int numSectors_, int numberOfYears_, int numTimeSteps_)
	{	allocate(numCatchments_, numSectors_, numberOfYears_, numTimeSteps_);
	}

	CropPrecipitation(CropPrecipitation& orig)
	{	allocate(orig);
	}

	virtual CropPrecipitation& operator=(CropPrecipitation&);
	virtual ~CropPrecipitation();

	//@}

protected:

	int numberOfYears;
	int numTimeSteps;
	int numCatchments;
	int numSectors;
	typeInputDataActualPrecipitation inputType;

	virtual int getNumberOfYears()
	{	return numberOfYears;
	}

	virtual int getNumTimeStepsPerYear()
	{	return numTimeSteps;
	}

	virtual int getNumCatchments()
	{	return numCatchments;
	}

	virtual int getNumSectors()
	{	return numSectors;
	}

	virtual int getRealYear(int yInd)
	{  return (int)translationMatrix_YearToIndex.getValue(yInd);
	}

	virtual typeInputDataActualPrecipitation getInputType()
	{	return inputType;
	}

	virtual bool  getIsDataPerSector()
	{	return isDataPerSector;
	}

	bool isDataPerSector;
	
	///Used to convert specific year to index
	MatrixDouble translationMatrix_YearToIndex;

	///Pointer to actual precipitation (data structure at regional level)
	MatrixDouble** actualPrecipitation;
	// Dimension of matrix: (years X timeSteps ):
	// Length of array: (catchments X sectors)

	///Pointer to average precipitation (data structure at regional level)
	MatrixDouble** averagePrecipitation;
	// Dimension of matrix: (1 X timeSteps ):
	// Length of array: (catchments X sectors)


	// Functions:
	virtual void allocate();
	virtual void allocate(int numCatchments_, int numSectors_, int numberOfYears_, int numTimeSteps_);
	virtual void allocate(CropPrecipitation& orig);
	virtual void deallocate();
	virtual void copy(CropPrecipitation& orig);

	void returnFscan(int retVal, char* strVar, int catchID);

};


///Second auxiliary class
enum enumPotETrType
{	potETrTypeActual, potETrTypeAverage
};

enum typeInputDataPotentialETr
{	inputTypePotEtrByYear, inputTypePotEtrRandom
};

class CropPotentialETr
{

public:
	///@name Data query functions
	//@{
	virtual double getPotentialETrByYear(int year_, int month_, int catchment_, int sector_);
	virtual double getPotentialETrByIndex(int year_, int month_, int catchment_, int sector_);
	virtual int getIndexForAbsoluteYear(int year);
	virtual void printValue(int year_, int month_, int catchment_, int sector_);
	virtual void printValueByIndex(int yearIndex, int month_, int catchment_, int sector_);
	virtual double getAveragePotentialETrInMonth(int month_, int catchment_, int sector_);
	//@}

	///@name Print, write and read functions
	//@{
	virtual void printToScreen();
	virtual void writeToFile(char* filename, int catchID);
	virtual void readHeader(FILE* strm, int catchID);
	virtual void readPotentialETrDataIntoMatrix(FILE* strm, MatrixDouble& matrixPassedByRef, int numYR_, int secID, enumPotETrType _potETrType, int catchID);
	virtual void readFromFile(char* filename, int catchID);//with <fscanf>
	//@}

	///@name Constructors and destructor
	//@{
	CropPotentialETr()
	{	allocate();
	}

	CropPotentialETr(int numCatchments_, int numSectors_, int numberOfYears_, int numTimeSteps_)
	{	allocate(numCatchments_, numSectors_, numberOfYears_, numTimeSteps_);
	}

	CropPotentialETr(CropPotentialETr& orig)
	{	allocate(orig);
	}

	virtual CropPotentialETr& operator=(CropPotentialETr&);
	virtual ~CropPotentialETr();

	//@}

protected:

	int numberOfYears;
	int numTimeSteps;
	int numCatchments;
	int numSectors;
	typeInputDataPotentialETr inputType;

	virtual int getNumberOfYears()
	{	return numberOfYears;
	}

	virtual int getNumTimeStepsPerYear()
	{	return numTimeSteps;
	}

	virtual int getNumCatchments()
	{	return numCatchments;
	}

	virtual int getNumSectors()
	{	return numSectors;
	}

	virtual int getRealYear(int yInd)
	{  return (int)translationMatrix_YearToIndex.getValue(yInd);
	}

	virtual typeInputDataPotentialETr getInputType()
	{	return inputType;
	}

	virtual bool  getIsDataPerSector()
	{	return isDataPerSector;
	}

	bool isDataPerSector;
	
	///Used to convert specific year to index
	MatrixDouble translationMatrix_YearToIndex;

	///Pointer to potential Etr (data structure at regional level)
	MatrixDouble** allYearsPotentialETr;
	// Dimension of matrix: (years X timeSteps ):
	// Length of array: (catchments X sectors)

	///Pointer to average potential Etr (data structure at regional level)
	MatrixDouble** averagePotentialEtr;
	// Dimension of matrix: (1 X timeSteps ):
	// Length of array: (catchments X sectors)


	// Functions:
	virtual void allocate();
	virtual void allocate(int numCatchments_, int numSectors_, int numberOfYears_, int numTimeSteps_);
	virtual void allocate(CropPotentialETr& orig);
	virtual void deallocate();
	virtual void copy(CropPotentialETr& orig);

	//security check
	void returnFscan(int retVal, char* strVar, int catchID);
};


///Third auxiliary class
enum enumFloodFactorType
{	floodFactorTypeActual, floodFactorTypeAverage
};

enum typeInputDataFloodFactor
{	inputTypeFloodFactorByYear, inputTypeFloodFactorRandom
};

class CropFloodFactor{
public:
	///@name Data query functions
	//@{
	virtual double getFloodFactorByYear(int year_, int month_, int catchment_, int sector_);
	virtual double getFloodFactorByIndex(int yearIndex_, int month_, int catchment_, int sector_);
	virtual int getIndexForAbsoluteYear(int year);
	virtual void printValue(int year_, int month_, int catchment_, int sector_);
	virtual void printValueByIndex(int yearIndex, int month_, int catchment_, int sector_);
	virtual double getAverageFloodFactorInMonth(int month_, int catchment_, int sector_);
	//@}

	///@name Print, write and read functions
	//@{
	virtual void printToScreen();
	virtual void writeToFile(char* filename, int catchID);
	virtual void readHeader(FILE* strm, int catchID);
	virtual void readFloodFactorDataIntoMatrix(FILE* strm, MatrixDouble& matrixPassedByRef, int numYR_, int secID, enumFloodFactorType _FloodFactorType, int catchID);
	virtual void readFromFile(char* filename, int catchID);//with <fscanf>
	//@}

	///@name Constructors and destructor
	//@{
	CropFloodFactor()
	{	allocate();
	}

	CropFloodFactor(int numCatchments_, int numSectors_, int numberOfYears_, int numTimeSteps_)
	{	allocate(numCatchments_, numSectors_, numberOfYears_, numTimeSteps_);
	}

	CropFloodFactor(CropFloodFactor& orig)
	{	allocate(orig);
	}

	virtual CropFloodFactor& operator=(CropFloodFactor&);
	virtual ~CropFloodFactor();

	//@}

protected:

	int numberOfYears;
	int numTimeSteps;
	int numCatchments;
	int numSectors;
	typeInputDataFloodFactor inputType;

	virtual int getNumberOfYears()
	{	return numberOfYears;
	}

	virtual int getNumTimeStepsPerYear()
	{	return numTimeSteps;
	}

	virtual int getNumCatchments()
	{	return numCatchments;
	}

	virtual int getNumSectors()
	{	return numSectors;
	}

	virtual int getRealYear(int yInd)
	{  return (int)translationMatrix_YearToIndex.getValue(yInd);
	}

	virtual typeInputDataFloodFactor getInputType()
	{	return inputType;
	}

	virtual bool  getIsDataPerSector()
	{	return isDataPerSector;
	}

	bool isDataPerSector;
	
	///Used to convert specific year to index
	MatrixDouble translationMatrix_YearToIndex;

	///Pointer to flood factor (data structure at regional level)
	MatrixDouble** allYearsFloodFactor;
	// Dimension of matrix: (years X timeSteps ):
	// Length of array: (catchments X sectors)

	///Pointer to average flood factors (data structure at regional level)
	MatrixDouble** averageFloodFactor;
	// Dimension of matrix: (1 X timeSteps ):
	// Length of array: (catchments X sectors)


	// Functions:
	virtual void allocate();
	virtual void allocate(int numCatchments_, int numSectors_, int numberOfYears_, int numTimeSteps_);
	virtual void allocate(CropFloodFactor& orig);
	virtual void deallocate();
	virtual void copy(CropFloodFactor& orig);

	//security check
	void returnFscan(int retVal, char* strVar, int catchID);
};



#endif

