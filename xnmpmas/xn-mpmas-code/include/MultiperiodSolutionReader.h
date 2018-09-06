/*
 * MultiperiodSolutionReader.h
 *
 *  Created on: Jun 1, 2015
 *      Author: troost
 */

#ifndef MULTIPERIODSOLUTIONREADER_H_
#define MULTIPERIODSOLUTIONREADER_H_

#include <map>
#include <vector>
#include <sstream>
#include <stdexcept>

#include "MpmasCustomContainers.h"

enum enumTypeMultiperiodSolutionReader
{
	typeMultiperiodSolutionReaderBasic,
	typeMultiperiodSolutionReaderWithMultiplier,
	typeMultiperiodSolutionReaderByArgs
};

using namespace std;


class MultiperiodSolutionReader
{
	protected:
		vector<int> colsSolutionOnly;
		vector<int> colsObjectiveValue;
	public:
		virtual enumTypeMultiperiodSolutionReader type() const { return typeMultiperiodSolutionReaderBasic;}
		virtual size_t numArgs() const {return 0;}
		bool isDefined() const
		{
			if (colsSolutionOnly.empty() && colsObjectiveValue.empty())
				return false;

			return true;
		}

		virtual double calculateValue(double * sol, double * zrow) const
		{
			double final = 0.0;

			for (size_t i = 0; i < colsSolutionOnly.size(); i++)
				final += sol[ colsSolutionOnly.at(i) ];

			for (size_t i = 0; i < colsObjectiveValue.size(); i++)
				final += sol[ colsObjectiveValue.at(i) ] * zrow[ colsObjectiveValue.at(i) ];

			return final;
		}
		void addColSolOnly(int col_)
		{
			colsSolutionOnly.push_back(col_);
		}
		void addColObjValue(int col_)
		{
			colsObjectiveValue.push_back(col_);
		}

		//virtual destructor (though no virtual methods yet) to allow dynamic_cast
		virtual ~MultiperiodSolutionReader(){}
};
class MultiperiodSolutionReaderWithMultiplier: public MultiperiodSolutionReader {
	protected:
		vector<double> multiplierSolutionOnly;
		vector<double> multiplierObjectiveValue;
	public:
		virtual enumTypeMultiperiodSolutionReader type() const { return typeMultiperiodSolutionReaderWithMultiplier;}
		virtual size_t numArgs() const {return 1;}
		void addColSolOnly(int col_, double coefficient)
		{
			colsSolutionOnly.push_back(col_);
			multiplierSolutionOnly.push_back(coefficient);
		}
		void addColObjValue(int col_, double coefficient)
		{
			colsObjectiveValue.push_back(col_);
			multiplierObjectiveValue.push_back(coefficient);
		}
		virtual double calculateValue (double * sol, double * zrow) const
		{
			double final = 0.0;

			for (size_t i = 0; i < colsSolutionOnly.size(); i++)
				final += sol[ colsSolutionOnly.at(i) ] * multiplierSolutionOnly.at(i);

			for (size_t i = 0; i < colsObjectiveValue.size(); i++)
				final += sol[ colsObjectiveValue.at(i) ] * zrow[ colsObjectiveValue.at(i) ] * multiplierObjectiveValue.at(i);

			return final;
		}

		virtual ~MultiperiodSolutionReaderWithMultiplier(){};

};

class MultiperiodSolutionReaderByArgs: public MultiperiodSolutionReader
{
	protected:
		size_t nArgs;
		vector<int> arglimitsUp;
		vector<int> arglimitsLow;
		map<vector<int>, pair<vector<int>,vector<int> > > colsByArgs; //key: .first: vector of int arguments, value: second.first: colsSolOnly, second.second. colsWithObjective
		vector<pair<vector<int>,vector<int> > > fastAccessArray;
	public:
		MultiperiodSolutionReaderByArgs(size_t n_) : nArgs (n_),  arglimitsUp(), arglimitsLow(),colsByArgs(),fastAccessArray() {}

		MultiperiodSolutionReaderByArgs(size_t n_, int* lowlimits_, int* uplimits_) : nArgs (n_),colsByArgs(),fastAccessArray()
		{
			arglimitsUp = vector<int>( uplimits_, uplimits_ + nArgs);
			arglimitsLow = vector<int>(lowlimits_, lowlimits_ + nArgs);
		}
		//MultiperiodSolutionReaderByArgs()  {}
		virtual ~MultiperiodSolutionReaderByArgs() {}
		virtual enumTypeMultiperiodSolutionReader type() const  { return typeMultiperiodSolutionReaderByArgs;}
		virtual size_t numArgs() const {return nArgs;}
		void addColSolOnly(int col_, vector<int>  args) throw(logic_error)
		{
			if (args.size() != nArgs)
			{
				stringstream errmsg;
				errmsg << "Error: Incorrect number of arguments in <MultiperiodSolutionReaderByArgs::addColSolOnly> " <<
						"for column " << col_ << "\n";
				throw(logic_error(errmsg.str()));
			}

			if(! arglimitsUp.empty())
			{  for (size_t i = 0; i < nArgs; ++i)
				{
					if(arglimitsUp[i] > -1   && args[i] > arglimitsUp[i] )
					{
						stringstream errmsg;
						errmsg << "Error: Invalid argument (pos " <<i <<" in <MultiperiodSolutionReaderByArgs::addColSolOnly> " <<
								"for column " << col_ << "\n";
						throw(logic_error(errmsg.str()));
					}
				}
			}
			if(! arglimitsLow.empty())
			{  for (size_t i = 0; i < nArgs; ++i)
				{
					if(arglimitsLow[i] > -2   && args[i] < arglimitsLow[i] )
					{
						stringstream errmsg;
						errmsg << "Error: Invalid argument (pos " <<i <<" in <MultiperiodSolutionReaderByArgs::addColSolOnly> " <<
								"for column " << col_ << "\n";
						throw(logic_error(errmsg.str()));
					}
				}
			}
			vector<int> tempColSol;
			vector<int> tempColObj;
			//try to insert empty vector, fails if already exists, but will return iterator to newly created or already existing key/value pair
			//in first part of return value
			pair<map<vector<int>, pair<vector<int>,vector<int> > >::iterator, bool>  iterBool = colsByArgs.insert(pair<vector<int>, pair<vector<int>,vector<int> > >(args, pair<vector<int>, vector<int> >(tempColSol, tempColObj )) );

			//add column to appropriate vector (here:second.first = no objective )
			map<vector<int>, pair<vector<int>,vector<int> > >::iterator it = iterBool.first;
			it->second.first.push_back(col_);

		}
		void addColObjValue(int col_, vector<int>  args) throw(logic_error)
		{
			if (args.size() != nArgs)
			{
				stringstream errmsg;
				errmsg << "Error: Incorrect number of arguments in <MultiperiodSolutionReaderByArgs::addColObjValue> "
						"for column " << col_ << "\n";
				throw(logic_error(errmsg.str()));
			}

			vector<int> tempColSol;
			vector<int> tempColObj;
			//try to insert empty vector, fails if already exists, but will return iterator to newly created or already existing key/value pair
			//in first part of return value
			pair<map<vector<int>, pair<vector<int>,vector<int> > >::iterator, bool>  iterBool = colsByArgs.insert(pair<vector<int>, pair<vector<int>,vector<int> > >(args, pair<vector<int>, vector<int> >(tempColSol, tempColObj )) );

			//add column to appropriate vector (here:second.second = with objective )
			map<vector<int>, pair<vector<int>,vector<int> > >::iterator it = iterBool.first;
			it->second.second.push_back(col_);
		}
		double getResultForArgs(double* sol, double* zrow, vector<int> where, int &nothingFoundFlag) const
		{
			double result = 0.0;;
			map<vector<int>, pair<vector<int>,vector<int>  > >::const_iterator it = colsByArgs.find(where);
			if (it != colsByArgs.end() )
			{
				vector<int>colsOnlyList = it->second.first;

				for (size_t j = 0; j < colsOnlyList.size(); j++)
					result += sol[ colsOnlyList.at(j) ];

				vector<int>colsObjList = it->second.second;
				for (size_t i = 0; i < colsObjList.size(); i++)
					result += sol[ colsObjList.at(i) ] * zrow[ colsObjList.at(i) ];
				nothingFoundFlag = false;
			}
			else {
				nothingFoundFlag = true;
			}
			return result;
		}


		//returns results
		MapOfVectorIndexedDoubles calculateResultMap(double* sol, double* zrow, bool includeNonzero = false) const
		{  //returns a map, whose first element is a vector of int arguments, and the second is the corresponding solution from the MIP
			MapOfVectorIndexedDoubles resultMap;
			for (map<vector<int>, pair<vector<int>,vector<int>  > >::const_iterator it = colsByArgs.begin(); it != colsByArgs.end(); ++it)
			{
				double val = 0.0;
				vector<int>colsOnlyList = it->second.first;

				for (size_t j = 0; j < colsOnlyList.size(); j++)
					val += sol[ colsOnlyList.at(j) ];

				vector<int>colsObjList = it->second.second;
				for (size_t i = 0; i < colsObjList.size(); i++)
					val += sol[ colsObjList.at(i) ] * zrow[ colsObjList.at(i) ];

				if (includeNonzero || val != 0.0)
					(void) resultMap.insert(vectorIndexedDouble(it->first, val) ); //not checking whether key already exists as construction of colsByArgs should make sure it is not
			}
			return resultMap;
		}
		MapOfSubindexedVectorIndexedDoubles calculateSubindexedResultMap(double* sol, double* zrow, size_t sizeIndex1 , bool includeNonzero = false) const
		{  //returns a map, whose first element is a vector of int arguments,
			//and the second is the corresponding solution from the MIP
			MapOfSubindexedVectorIndexedDoubles resultMap;
			for (map<vector<int>, pair<vector<int>,vector<int>  > >::const_iterator it = colsByArgs.begin(); it != colsByArgs.end(); ++it)
			{
				double val = 0.0;
				vector<int> fullIndex = it->first;

				vector<int> colsOnlyList = it->second.first;

				for (size_t j = 0; j < colsOnlyList.size(); j++)
					val += sol[ colsOnlyList.at(j) ];

				vector<int>colsObjList = it->second.second;
				for (size_t i = 0; i < colsObjList.size(); i++)
					val += sol[ colsObjList.at(i) ] * zrow[ colsObjList.at(i) ];

				//split the index as requested
				vector<int> index1 (fullIndex.begin(), fullIndex.begin() +  sizeIndex1 );
				vector<int> index2 (fullIndex.begin() +  sizeIndex1, fullIndex.end() );
				if (includeNonzero || val != 0.0)
					(void) resultMap.insert(SubindexedVectorIndexedDouble(index1, vectorIndexedDouble(index2, val)) ); //not checking whether key already exists as construction of colsByArgs should make sure it is not
			}
			return resultMap;
		}



		void getResultsUsingFastAccessArray(double* sol, double* zrow, double* resultArray, int sizeResultArray) const
		{	// requires that makeFastAccessArrayFor2Dims() has been called to create fastAccessArray
			if ( (int) fastAccessArray.size() > sizeResultArray )
			{
				stringstream errmsg;
				errmsg << "Error in <MultiperiodSolutionReaderByArgs::getResultsUsingFastAccessArray(): Array sizes inconsistent."
						     << " Size of result array: " << sizeResultArray
						     << "; Size of fastAccessArray: " << fastAccessArray.size() << "\n";
				throw(runtime_error(errmsg.str()));
			}
			if (fastAccessArray.empty() )
			{
				stringstream errmsg;
				errmsg << "Error in <MultiperiodSolutionReaderByArgs::getResultsUsingFastAccessArray(): No fastAccessArray defined!\n";
				throw(runtime_error(errmsg.str()));

			}
			for (int i = 0; i < (int) fastAccessArray.size(); i++)
			{
				resultArray[i] = 0.0;
				vector<int>colsOnlyList = fastAccessArray[i].first;

				for (size_t j = 0; j < colsOnlyList.size(); j++)
					resultArray[i] += sol[ colsOnlyList.at(j) ];

				vector<int>colsObjList = fastAccessArray[i].second;
				for (size_t i = 0; i < colsObjList.size(); i++)
					resultArray[i] += sol[ colsObjList.at(i) ] * zrow[ colsObjList.at(i) ];
			}
		}
		void addToResultsUsingFastAccessArray(double* sol, double* zrow, double* resultArray, int sizeResultArray) const
		{	// requires that makeFastAccessArrayFor2Dims() has been called to create fastAccessArray
			if ((int) fastAccessArray.size() > sizeResultArray )
			{
				stringstream errmsg;
				errmsg << "Error in <MultiperiodSolutionReader::addToResultsUsingFastAccessArray(): Array sizes inconsistent."
						     << " Size of result array: " << sizeResultArray
						     << "; Size of fastAccessArray: " << fastAccessArray.size() << "\n";
				throw(runtime_error(errmsg.str()));
			}
			if (fastAccessArray.empty() )
			{
				stringstream errmsg;
				errmsg << "Error in <MultiperiodSolutionReader::addToResultsUsingFastAccessArray(): No fastAccessArray defined!\n";
				throw(runtime_error(errmsg.str()));

			}
			for (int i = 0; i < (int) fastAccessArray.size(); i++)
			{
				//contrary to getResultsUsingFastAccessArray resultArrayEntry is not set to zero here!
				vector<int>colsOnlyList = fastAccessArray[i].first;

				for (size_t j = 0; j < colsOnlyList.size(); j++)
					resultArray[i] += sol[ colsOnlyList.at(j) ];

				vector<int>colsObjList = fastAccessArray[i].second;
				for (size_t j = 0; j < colsObjList.size(); j++)
					resultArray[j] += sol[ colsObjList.at(j) ] * zrow[ colsObjList.at(j) ];
			}
		}
		void addToResultsAndRecordInVectorUsingFastAccessArray(double* sol, double* zrow, double* resultArray, int sizeResultArray, vector<pair<int,double> >* myResult) const
		{	// requires that makeFastAccessArrayFor2Dims() has been called to create fastAccessArray
			if ((int) fastAccessArray.size() > sizeResultArray && sizeResultArray > -1 )
			{
				stringstream errmsg;
				errmsg << "Error in <MultiperiodSolutionReader::addToResultsUsingFastAccessArray(): Array sizes inconsistent."
						     << " Size of result array: " << sizeResultArray
						     << "; Size of fastAccessArray: " << fastAccessArray.size() << "\n";
				throw(runtime_error(errmsg.str()));
			}
			if (fastAccessArray.empty() )
			{
				stringstream errmsg;
				errmsg << "Error in <MultiperiodSolutionReader::addToResultsUsingFastAccessArray(): No fastAccessArray defined!\n";
				throw(runtime_error(errmsg.str()));

			}
			for (int i = 0; i < (int) fastAccessArray.size(); i++)
			{
				double myVal = 0.0;

				//contrary to getResultsUsingFastAccessArray resultArrayEntry is not set to zero here!
				vector<int>colsOnlyList = fastAccessArray[i].first;

				for (size_t j = 0; j < colsOnlyList.size(); j++)
					myVal += sol[ colsOnlyList.at(j) ];

				vector<int>colsObjList = fastAccessArray[i].second;
				for (size_t j = 0; j < colsObjList.size(); j++)
					myVal += sol[ colsObjList.at(j) ] * zrow[ colsObjList.at(j) ];

				if (myVal != 0.0)
				{	if(sizeResultArray > -1)
						resultArray[i] += myVal;
					myResult->push_back(pair<int,double>(i, myVal));
				}
			}
		}







		unsigned makeFastAccessArrayFor2Dims(map<pair<int,int>, int>* transformationMap)
		{	// create an aligned array for faster copying based on transformationMap
			// multikey index is transformed into single int array index for later result arrays, allows use of getResultsUsingFastAccessArray()
			for (map<vector<int>, pair<vector<int>,vector<int> > >::iterator it = colsByArgs.begin(); it != colsByArgs.end(); ++it)
			{	 //get arrayIndex for first two arguments of key vector of colsByArgs from transformationMap
				 map<pair<int,int>, int>::iterator mi = transformationMap->find(pair<int,int>(it->first.at(0),it->first.at(1)));
				 if (mi == transformationMap->end())
				 {
						stringstream errmsg;
						errmsg << "Error in <MultiperiodSolutionReader::makeFastAccessArrayFor2Dims(): Value "<< it->first.at(0) <<", " << it->first.at(1) << " not found in transformation map!\n";
						throw(runtime_error(errmsg.str()));
				 }
				 int arrayIndex = mi->second;
				 //increase size of fastAccessArray if necessary
				 if (arrayIndex >= (int) fastAccessArray.size())
					 fastAccessArray.resize(arrayIndex +1);

				 //copy the pair of colsOnlyList and colsObjList vectors from the original map to the fastAccessArray
				 fastAccessArray[arrayIndex] = it->second;
			}
			return colsByArgs.size();
		}
		void getResultsAsArrayForFirstArgument(double* sol, double* zrow, double* resultArray, int sizeResultArray) const
		{	// only for one argument arrays

			for (map<vector<int>, pair<vector<int>,vector<int>  > >::const_iterator it = colsByArgs.begin(); it != colsByArgs.end(); ++it)
			{
				int arrayIndex = it->first[0];
				if (arrayIndex >= sizeResultArray)
				{
					stringstream errmsg;
					errmsg << "Error in <MultiperiodSolutionReader::getResultsAsArrayForFirstArgument(): Index larger than array size\n";
					throw(runtime_error(errmsg.str()));
				}

				resultArray[arrayIndex] = 0.0;

				vector<int>colsOnlyList = it->second.first;

				for (size_t j = 0; j < colsOnlyList.size(); j++)
					resultArray[arrayIndex] += sol[ colsOnlyList.at(j) ];

				vector<int>colsObjList = it->second.second;
				for (size_t i = 0; i < colsObjList.size(); i++)
					resultArray[arrayIndex] += sol[ colsObjList.at(i) ] * zrow[ colsObjList.at(i) ];
			}
		}
};






/*class MultiperiodSolutionReaderByArgs: public MultiperiodSolutionReader
{
	protected:
		int nArgs;
		vector<vector<double> > colsSolutionOnlyArgs;
		vector<vector<double> > colsObjectiveValueArgs;
	public:
		MultiperiodSolutionReaderByArgs(int n_) {nArgs = n_;};
		void addColSolOnly(int col_, vector<double>*  args)
		{
			if (args->size() != nArgs)
			{
				sprintf(fehlertxt, "Error: Incorrect number of arguments in <MultiperiodSolutionReaderByArgs::addColSolOnly> for column %d\n", col_);
				fehlerabbruch();
			}
			colsSolutionOnly.push_back(col_);
			colsSolutionOnlyArgs.push_back(*args);
		}
		void addColObjValue(int col_, vector<double>*  args)
		{
			if (args->size() != nArgs)
			{
				sprintf(fehlertxt, "Error: Incorrect number of arguments in <MultiperiodSolutionReaderByArgs::addColObjValue> for column %d\n", col_);
				fehlerabbruch();
			}

			colsObjectiveValue.push_back(col_);
			colsObjectiveValueArgs.push_back(*args);
		}
		void calculate2DimResultMatrix(double* sol, double* zrow, vector<vector<double> >* resultMatrix)
		{

			for (int i = 0; i < colsSolutionOnly.size(); i++)
				resultMatrix->at((int) colsSolutionOnlyArgs.at(0)).at( (int) colsSolutionOnlyArgs.at(1)) += sol[ colsSolutionOnly.at(i) ];

			for (int i = 0; i < colsObjectiveValue.size(); i++)
				resultMatrix->at((int) colsObjectiveValueArgs.at(0)).at( (int) colsObjectiveValueArgs.at(1)) += sol[ colsObjectiveValue.at(i) ] * zrow[ colsObjectiveValue.at(i) ];
		}
};*/


#endif /* MULTIPERIODSOLUTIONREADER_H_ */
