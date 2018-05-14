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
//  File:       PermanentCropsHandler.h (formerly cultivo9.h)
//
//  Contents:   Manages perennial crop data such as life tables and computes
//              related coefficients such as expected remaining fruit production
//              per hectare
//
//------------------------------------------------------------------------------


//Klasse fuer Dauerkulturen

#ifndef _PermanentCropsHandler_h
#define _PermanentCropsHandler_h

#include "BasicEnums.h"
#include "MatrixDouble.h"
#ifdef ODBSTORE
	#include "odb/core.hxx"
#endif


///data structure for permanent crops
typedef struct
{  int code;//object ID
   double purPrice;//purchase price
	int lifeSp;//lifespan
	MatrixDouble anbaudaten;//yearly factor demands
	MatrixDouble originalData;//original input data
} dauerkultur;

//---------------------------------------------------------------

///auxiliary class for management groups
class PermCropCultivationPractices
{	public:

	///@name Functions
	//@{
	///get number of management groups
	virtual int getNumManagementGroups();

	///put permanent crop into management group
	virtual void putPermCropIntoManagementGroup(int objID, int groupIdx);

	///query gives group index for a particular object
	virtual int getGroupIdxForObject(int objID);

	///query gives object IDs of other members and returns number of IDs
   virtual int getObjectIDsOfOthersInGroup(int objID, MatrixDouble& otherObjIDs);

	///query gives Lp activity for switching between objects
	virtual int getLpColumnForSwitchingObjects(int objID, int switchObjID);

	///read content from open stream
	virtual void readFromOpenStream(FILE* strm);

	///write content to open stream
	virtual void writeToOpenStream(FILE* strm);

	///free memory
   virtual void deallocate();	
 	//@}
	
	///Constructor
   PermCropCultivationPractices(int numGroups);
	
	///Destructor
   virtual ~PermCropCultivationPractices()
	{	deallocate();
	}

	protected:
	///@name Data
	//@{
	///number of management groups
   int numManagementGroups;	

	///object IDs per management group
   MatrixDouble* objectIDsInGroup; 	

	///Lp activity indices per switching option
	MatrixDouble* lpColumnsInGroup;
 	//@}
};

//---------------------------------------------------------------
#pragma db object
class cultivo
{  protected:
#pragma db id
	int id;
#pragma db transient
	FILE* datenstrom;
	int cul_anzahl;//Anzahl der Dauerkulturen
	int numLaborConstraints;
	int numMachineryConstraints;

#pragma db transient
	//auxiliary data structure for switching of cultivation practices
	PermCropCultivationPractices* switchManagementInfo;//TODO: ODBSTORE implement perennials

	void returnFscan(int retVal, char* strVar);

#ifdef ODBSTORE
	friend class odb::access;
#endif
	public:
	cultivo(void)
	{  datenstrom = NULL;
		cul_anzahl = 0;
		numLaborConstraints = 0;
		numMachineryConstraints = 0;
		switchManagementInfo = NULL;
		id = 1;
	}

   virtual ~cultivo()
	{	speicher_freigeben();
	}
#pragma db transient
//TODO: ODBSTORE implement perennials
   dauerkultur* d_kultur; //dynamisch allokiertes Array von Dauerkulturen
   virtual void readInputFromFile(const char* fileName);
   virtual void writeInputInFile(char* fileName);

	virtual void updateProductionCosts(MatrixDouble& lpIndicesAndRelativeCosts);
	virtual int getSwitchingCultivationOtherIDs(int objID, MatrixDouble& otherObjIDs);
	virtual int getSwitchingCultivationLpColumn(int objID, int switchObjID);
	
	virtual int getPermanentCropID(int objectID);
   virtual int checkIfObjectIsPermamentCrop(int objectID);
	virtual int getNumLaborConstraints();
	virtual int getNumMachineryConstraints();

	///Note: functions return values for permanent crop ID, not for object ID
	virtual double getFutureCropYields(int permCropID, int age, double rate);
	virtual double getFutureVariableCosts(int permCropID, int age, double rate);
	virtual double getFuturePreHarvestCosts(int permCropID, int age, double rate);
	virtual double getFutureHarvestCosts(int permCropID, int age, double rate);
	virtual double getFutureLaborDemand(int permCropID, int age, double rate, int level);
	virtual double getFutureMachineryDemand(int permCropID, int age, double rate, int level);

	virtual double getCropYieldInYear(int permCropID, int age);
	virtual double getVariableCostsInYear(int permCropID, int age);
	virtual double getPreHarvestCostsInYear(int permCropID, int age);
	virtual double getHarvestCostsInYear(int permCropID, int age);
	virtual double getLaborDemandInLevel(int permCropID, int age, int level);
	virtual double getMachineryDemandInLevel(int permCropID, int age, int level);

	virtual double getMaximumYield(int permCropID);//relative yield for TSPC and CropWat
	virtual double getMaxCashDeficitInGestationPeriod(int permCropID, double expPrice, double maxYield);

	virtual void printToScreen(void);
	virtual void speicher_freigeben(void);
};


#endif

