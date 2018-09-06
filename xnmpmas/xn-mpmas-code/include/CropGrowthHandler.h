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
//  File:       CropGrowthHandler.h (formerly part of soils.h)
//
//  Contents:   Plant growth class, computes soil nutrient, water stress
//              and actual plant yields
//
//
//------------------------------------------------------------------------------

#ifndef _CropGrowthHandler_h
#define _CropGrowthHandler_h

#include "CropSoil.h"
#include "CropWater.h"
#include "CropLucia.h"
#include "CropActualPrecipitation.h"


#include <algorithm>
#include <string>
#include <iostream>
#include "tree.hh"
#include "tree_util.hh"

using namespace std;


///Class CropGrowthHandler

/** 
 * This crop-related class handles the crop data input, allocates memory for the 
 * various crop growth modules and provide the interface between agents and crops
 * (this last functionality will be extended in the longer run)
 */

class CropGrowthHandler
{
	public:

		///@name Data query functions
		//@{
		///Return pointers to crop growth modules
		virtual CropSoil* const getPointerToCropSoilModule() const;
		virtual CropWater* const getPointerToCropWaterModule() const;
		virtual CropLucia* const getPointerToCropLuciaModule() const;
		virtual CropPrecipitation* const getPointerToCropPrecipitationModule() const;
		virtual CropPotentialETr* const getPointerToCropPotentialETrModule() const;
		virtual CropFloodFactor* const getPointerToCropFloodFactorModule() const;

		///Basic info
		virtual int getNumberOfCropActivitiesForSimilarity();
		virtual int getLevelsOfSimilarity();
		virtual int getPriceLpColumn(int cropActID);
		virtual int getLpColumn(int cropActID);

		virtual int getNumberOfCropActivities();
		virtual int getNumberCommunicatedYieldMapsPerPeriod();

#ifdef MULTIPERIOD
		map<int,int>* getMappingLandUseActivityIdToCropIndices();
		map<pair<int,int>,int>* getMappingCropManagementIdSoilTypeToArrayIndex();
		void getCopyOfArrayOfLandUseActivityIDs(int* arrayToCopyInto);
		int getYldArrayIndexForCropActivityID(int cropActID);
		int getCropManagementIdForYldArrayIndex(int YldIndex);
		int getNumberExtraCropActAttributes();
		const vector<int>* getPointerToLandUseForUnownedLandVector();

#endif //MULTIPERIOD

		///prepares questionnaire for farm survey
		virtual void prepareFarmProducerSurvey(MatrixDouble& questionnaire, int& firstLpColumn);
		virtual void insertCropActivityLpColumnsAndActIDs(MatrixDouble& tableLpColsAndActIDs);
		virtual const MatrixDouble* getTableLpColsAndActIDs();
		//@}

		//---------------------------------------------------------------------------

		///@name Data input functions
		//@{
		///Crop growth module reads its input data or gets its yields
		virtual void readInputFromFile();
		virtual void getCropYieldsFromExternalModel(FILE* openStream = NULL, const int numCropActs = 0, const int* cropActID = NULL, const double* cropYield = NULL, const double* stoverYield = NULL);

		//@}

		//---------------------------------------------------------------------------


		///@name Function related to agent knowledge base
		//@{
		///read and write data for initialization of knowledge base
		virtual void readDataKnowledgeBase();
		virtual void writeDataKnowledgeBase(char* filename);
		virtual void print_tree(const tree<string>& tr, tree<string>::pre_order_iterator it, tree<string>::pre_order_iterator end);

		///estimate yields for crops that were not grown by agent
		virtual void estimateYieldsForMissingCrops8(int agentID, const MatrixDouble& expectedYields, //## Tbe 121007, substitute
				MatrixDouble& actualYields);
		virtual void estimateYieldsForMissingCrops(int agentID, const MatrixDouble& expectedYields,
				MatrixDouble& actualYields);
		//@}

		//---------------------------------------------------------------------------
		///@name Constructors and destructors
		//@{
		///Constructor
		CropGrowthHandler();

		///Copy constructor
		//CropGrowthHandler(const CropGrowthHandler& cropHandler_);

		///Assignment operator
		//CropGrowthHandler& operator=(const CropGrowthHandler& cropHandler_);

		///Destructor
		virtual ~CropGrowthHandler()
		{	//free memory
			delete cropSoilModule;
			cropSoilModule = NULL;
			delete cropWaterModule;
			cropWaterModule = NULL;
			delete cropLuciaModule;
			cropLuciaModule = NULL;
			delete cropPrecipitationModule;
			cropPrecipitationModule = NULL;
			delete cropPotentialETrModule;
			cropPotentialETrModule = NULL;
			delete cropFloodFactorModule;
			cropFloodFactorModule = NULL;

			inputCropSimilarities.deallocate();
			numberOfClustersPerLevel.deallocate();
			clusterIndices.deallocate();
			tableLpColsAndActIDs.deallocate();

			delete [] iterLevels;
			for(int lvl = 0; lvl < levelsOfSimilarity; lvl++)
			{	delete [] iterClusters[lvl];
			}
			iterLevels = NULL;
			delete [] iterClusters;
			iterClusters = NULL;
		}
		//@}


	protected:
		///@name Protected member data
		//@{
		///Pointer to the various crop growth modules
		CropSoil* cropSoilModule;
		CropWater* cropWaterModule;
		CropLucia* cropLuciaModule;
		CropPrecipitation* cropPrecipitationModule;
		CropPotentialETr* cropPotentialETrModule;
		CropFloodFactor* cropFloodFactorModule;
		//@}

		//@{
		///Agent knowledge base
		MatrixDouble inputCropSimilarities;//contains information directly taken from input file: cropping activities in rows
		int numberOfCropActivitiesForSimilarity;//corresponds to rows in 'inputCropSimilarities'
		int levelsOfSimilarity;//corresponds to columns in 'inputCropSimilarities'
		
		MatrixDouble numberOfClustersPerLevel;//vector, needed to create tree
		MatrixDouble clusterIndices;//matrix contains cluster indices for all cropping activities (rows) and levels (cols)

		int checkedSequenceInLp;
		MatrixDouble tableLpColsAndActIDs;//LpCol in col0, ActID in col1

	   ///Crop similarities organized in n-ary tree
		tree<string> tr;
		tree<string>::iterator top, loc; //default tree iterators
		tree<string>::iterator* iterLevels; //iterators, one per level of similarity
		tree<string>::iterator** iterClusters;//iterators, one per cluster and level
		//@}

		///@name Protected member functions
		//@{
		virtual void countClustersPerLevel();
		virtual void createCropSimilarityTree();
		virtual void createClusterIndices();

		virtual int getClusterIndex(int cropIdx, int levelIdx);//returns '-1' when nothing found
		virtual int retrieveActivityIDsInCluster(int levelIdx, int clusterIdx, MatrixDouble& activityIDs);//returns '0' when OK

		virtual double estimateActualYield8(int idx, int lvl, const MatrixDouble& expectedYields, // ## Tbe 121007
				const MatrixDouble& actualYields);
		virtual double estimateActualYield(int idx, int lvl, 
			MatrixDouble*& expectedAverageYieldsInCLuster, MatrixDouble*& actualAverageYieldsInCLuster,
			const MatrixDouble& expectedYields,	const MatrixDouble& actualYields);

		virtual int getAverageYieldsOfSimilarCrops(int idx, int lvl, //return: -1 error, 0 yields available, 1 yields computed, 2 computed but no crop grown, 3 available no crop
			double& expectedYieldOfSimilarCrops, double& actualYieldOfSimilarCrops,
			MatrixDouble*& expectedAverageYieldsInCLuster, MatrixDouble*& actualAverageYieldsInCLuster,
			const MatrixDouble& expectedYields,	const MatrixDouble& actualYields);

		virtual void returnFscan(int retVal, char* strVar);
		//@}

};

#endif

