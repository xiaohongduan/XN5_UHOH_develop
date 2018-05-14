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
//  File:       PopulationCluster.h (formerly cluster.h)
//
//  Contents:   Population cluster class, containing the information for
//              Monte Carlo assignment of assets
//
//
//------------------------------------------------------------------------------



#ifndef _PopulationCluster_h
#define _PopulationCluster_h

#include <map>

#include "AgentFarmHousehold.h"
#include "PopulationNetworkSegment.h"
#include "MatrixDouble.h"


typedef struct
{  int career; //career path (i.e. object ID)
   int sex;
   int ageLow;
   int ageUpp;
// Troost 20121208 - Advanced demography model
// Troost **********begin block insert**********
	int married;
	int hhead;
// Troost **********end block insert**********
	int quantity;
} sexAgeGroup;

struct assetOwnershipInfo
{
		int objID;
		int age;
		double quantity;
};

struct agentCharacteristic
{
		int type;
		double value;
};

#define COLUMN_OF_FSTID 5 //index needed for "assetsFromFile"

class cluster
{  protected:
   int popID;        //Population ID
   int cluID;        //Cluster ID
   int cluNumAg;     //Number of agents in cluster (expected from cluster input file)
   int actNumAg;     //Number of agents in cluster (actually generated from maps)

   int numGroups;		//number of sex age groups in cluster
   sexAgeGroup* SAgroups;	//pointer to sex and age groups
   MatrixDouble SAlotto;   //Matrix for random assignment of household members
   MatrixDouble lotto;     //info for random assignments of investment goods
   int startC;					//first column containing lottery information

   MatrixDouble fCosts;	//Matrix of size-specific fix costs ("Gemeinkosten")

   MatrixDouble distrib;//Matrix containing results of random data generation
   int addInfoR;     //additional rows in 'distrib'
   int addInfoC;     //additional rows in 'distrib'
   int lastRow;      //last row entry in 'distrib'

	MatrixDouble assetsFromFile;//Matrix containing assets for each farm agent
	std::map<int,vector<assetOwnershipInfo> > assetsFromFileWithAge;
	std::map<int,vector<sexAgeGroup> > membersFromFileWithAge;
	std::map<int,vector<agentCharacteristic> > characteristicsFromFile;


   char filename[MXLENGTH];//Inputfile
   FILE* stream;     //Datenstrom

   void returnFscan(int retVal, char* strVar);

   double getCharacteristic(int objType, agentF* betr, string description, int& lr , int agentRowInAssetMatrix = -1);

	public:
	cluster(int, int, char*);//Parameter: Pop, Cluster ID, Name of inputfile

 	virtual ~cluster()
	{  lotto.deallocate();//previously freed by c_liste::lotto_deallokieren()
		distrib.deallocate();
		assetsFromFile.deallocate();
      SAlotto.deallocate();
      fCosts.deallocate();
      delete [] SAgroups;
	}

   cluster* nextCluster;
   virtual void setNextPtr (cluster*);
   virtual cluster* getNext();

   virtual void readClusterInput();
   virtual void writeClusterInput(char*);

   virtual void readAssetsMembersAgesFromFile();
	virtual void readAssetsFromFile();
	virtual void writeAssetsToFile(char*);

   virtual int getCluNum();
   virtual int getNumAgents();
   virtual int getActAgents();
   virtual void incrNumAgents();
   virtual void makeConsistent();

   //returns farm fix costs
   virtual double getFarmFixCosts(double totalHa, double operatedHa);

   //member functions being called from "betrieb_ausstatten()"
   virtual int getRowInLotto(int);
	virtual int getRowInAssetMatrix(int fstID);

   //Parameters: lotto MatrixDouble, row index, column index, number of stages
  	virtual double makeLottery(const MatrixDouble& lotto, int row, int = 0, int = STAGES);

	virtual void adjustAssetStructure(agentF*);//"Vermoegensstruktur"

	//Original:
	//virtual void adjustVintage(agentF*, segment*, int, int, double, int, double, double);
	//121031 C. Troost changed for interest rates on assets at start:
	//void adjustVintage(agentF*, segment*, int, int, double, int, double);
	// 121206 Troost correct price for old assets:
	void adjustVintage(agentF*, segment*, int, int, double, int, double, bool);


   virtual void adjustAsynchr(agentF*, segment*, double);

   virtual void deallocateLotto();
	virtual void printToScreen();
   virtual void writeLottoInFile(char*);
	virtual void writeDistribInFile(char*);
};


#endif

