/*
 * MpmasCustomContainers.h
 *
 *  Created on: May 6, 2016
 *      Author: troost
 */

#ifndef MPMASCUSTOMCONTAINERS_H_
#define MPMASCUSTOMCONTAINERS_H_

//Use for exchange of information between MIP solution
//and other classes

#include <vector>
#include <map>


typedef pair<vector<int>, double> vectorIndexedDouble;
typedef pair<vector<int>, vectorIndexedDouble> SubindexedVectorIndexedDouble;
typedef multimap<vector<int>, vectorIndexedDouble > MapOfSubindexedVectorIndexedDoubles;
typedef map<vector<int>, double > MapOfVectorIndexedDoubles;
typedef multimap<vector<int>, vector<double> > MapOfVectorIndexedDoubleVectors;
/*
class MapOfVectorIndexedDoubles
{
public:
		find();
		void insert(vectorIndexedDouble input) { return internalMap.insert(input); };
private:
		map<vector<int>, double > internalMap;

};

*/

#endif /* MPMASCUSTOMCONTAINERS_H_ */
