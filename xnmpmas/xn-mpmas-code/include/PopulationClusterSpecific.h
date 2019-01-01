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
//  File:       PopulationClusterSpecific.h (formerly ccluster.h)
//
//  Contents:   Contains application-specific implementation of Monte Carlo
//              assignment of farm household agents
//
//
//------------------------------------------------------------------------------



#ifndef _PopulationClusterSpecific_h
#define _PopulationClusterSpecific_h


#include "PopulationCluster.h"


class ccluster : public cluster
{	public:

   //Parameter: Pop, Cluster ID, Name of inputfile
	ccluster(int pID, int cID, char* inf) : cluster(pID, cID, inf)
   {
   }

	virtual ~ccluster()
	{
	}

   //contry-specific functions for random assignments
   virtual void assignFamMembers(agentF*);
   virtual void assignFamMembersWithAge(agentF*);

	virtual void assignAssetsToAgent(agentF*);
	virtual void assignAssetsToAgentWithAge(agentF*);

   virtual int assignCharactToAgent(agentF*);
   //virtual void writeDistribInFile(char*);## Tbe 110108 moved this function to cluster level
   virtual void printOnScreenCC();

};


#endif

