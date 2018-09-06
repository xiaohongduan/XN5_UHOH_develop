//------------------------------------------------------------------------------
//
//  Thomas Berger (main author), Pepijn Schreinemachers, Thorsten Arnold,
//  Prakit Siripalangkanont, and Christian Troost
//
//
//  Hohenheim University (490d)
//  Professorship for Land Use Economics in the Tropics and Subtropics
//
//  May not be distributed without permission by main author
//
//  File:       CropSoil.h (formerly soils.h)
//
//  Contents:   Header of the soil nutrient model TSPC
//
//
//
//------------------------------------------------------------------------------

#ifndef _CropSoil_h
#define _CropSoil_h

#ifdef ODB
//	#include "MpmasGlobalFunctions.h"
#else
	#include "BasicData.h"

#endif
	#include "MainAuxiliaryFunctions.h"
//
#include <string>
#include <cstring>
#include <cstdio>
#include <vector>

//general parameters
typedef struct
{  double dc;
   double bd;
   double rf;
   double ef;
   double so;
}  gnrlP;

//mineral fertilizer parameters
typedef struct
{  double mf_n;
   double mf_p;
   double mf_k;
   double ph_f;
   double li_f;
}  mferP;


//organic fertilizer parameters
typedef struct
{  double of_n;
   double of_p;
   double of_k;
   double of_c;
}  oferP;


//crop yield parameters
typedef struct
{  double yp;
   double m1;
   double m2;
   double n1;
   double n2;
   double p1;
   double p2;
   double k1;
   double k2;
   double c1;
   double c2;
   double c3;
   double a1;
   double a2;
   double fg;
   double fs;
}  yildF;

//crop physical parameters
typedef struct
{  double s1;
   double h1;
   double gn;
   double gp;
   double gk;
   double sn;
   double sp;
   double sk;
   double sc;
   double me;
}  cropP;

//crop erosion parameters
typedef struct
{  double e1;
   double e2;
}  erosP;

//-----------------------------------------------------------------------------

//data structures for cropping activities
typedef struct caYld
{  double yld;    //grain yield
   double ysm;    //stover energy yield

#ifdef MULTIPERIOD
   std::vector<double>extraAttributes;
#endif
}  caYld;


class inCrA
{  //auxiliary class, everthing public
   public:

   //input data per cropping activity
	int		actID;	//crop activity ID (wheat with certain crop management)
   int      crpID;	//crop ID (groups activities of wheat, beans, etc.)
   double   inLab;	//labor input
   double*  inFer;	//input of mineral fertilizers

   inCrA(int numF);
   virtual ~inCrA();
};

//-----------------------------------------------------------------------------

//auxiliary class soil conditions (held by grid object "flaeche9")
class soilC
{  public:

   double N;
   double P;
   double K;
   double A;
   double C;
   double sl;
   double se;

   virtual ~soilC()
   {
   }

};

//soil process variables (held by parcel object "plot")
class sProc
{  //auxiliary class, everthing public
   public:

   double SON1;
   double RTN1;
   double PCO1;
   double CER1;
   double KIC1;
   double NER1;
   double RTS1;
   double KGC1;
   double YST1;
   double SOC1;
   double APH1;

   double* FER1;

   sProc(int numF)
   {  if((FER1 = new double[numF]) == NULL)
      {	sprintf(fehlertxt, "Error: Not enough memory for 'sProc'\n");
      	fehlerabbruch();
      }
   }

   virtual ~sProc()
   {  delete [] FER1;
   }
};

//land use history of plot (held by parcel object "plot")
typedef struct
{  double STN1;
   double STN2;
   double STN3;
   double RES1;
   double RES2;
   double RES3;
   double MAN1;
   double MAN2;
   double MAN3;
}  lUseH;


//manure management
typedef struct
{  int manureApplied;
   double distributionWeight;
}  manure;

//crop mix per NRU (held by parcel object "plot")
//IMPORTANT: all values in crop mix are per ha values
class crpMixNRU
{  //auxiliary class, everthing public
   public:

   //data per cropping activity
   int nCr;    //number of cropping activities
   int*    crA;//cropping activity ID
   int*    cID;//crop IDs
   int*    cLP;//LP columns
   int*    yLP;//yield price column in LP
   double* aHa;//hectar of cropping activities per plot
   double* LAB;//labor input per ha
   double* YLD;//grain yield per ha
   double* YSM;//stover energy yield per ha
   double** FER;//mineral fertilizers applied per ha

   //average data of nutrient response unit
   int nru;		 //ID of NRU
   int soilID;  //soil type ID of NRU
   double tSize;//size of NRU in ha
   double tStvL;//expected stover demand of livestock from NRU in MJ
   double avgSl;//average slope length in NRU
   double avgSe;//average slope erodibility in NRU

   double STN;  //average N in stover per ha
   double RES;  //average percentage of stover left on plot
   double MAN;  //average manure left on plot per ha

   sProc* soilP;//soil process variables of previous year
   lUseH landU;//land use history


   crpMixNRU(int n, int numF)
   {  nCr = n;

		if((crA = new    int[nCr]) == NULL)
		{	sprintf(fehlertxt, "Error: Not enough memory for 'crpMix'\n");
			fehlerabbruch();
		}

		if((cID = new    int[nCr]) == NULL)
		{	sprintf(fehlertxt, "Error: Not enough memory for 'crpMix'\n");
			fehlerabbruch();
		}

		if((cLP = new    int[nCr]) == NULL)
		{	sprintf(fehlertxt, "Error: Not enough memory for 'crpMix'\n");
			fehlerabbruch();
		}

		if((yLP = new    int[nCr]) == NULL)
		{	sprintf(fehlertxt, "Error: Not enough memory for 'crpMix'\n");
			fehlerabbruch();
		}

		if((aHa = new double[nCr]) == NULL)
		{	sprintf(fehlertxt, "Error: Not enough memory for 'crpMix'\n");
			fehlerabbruch();
		}

		if((LAB = new double[nCr]) == NULL)
		{	sprintf(fehlertxt, "Error: Not enough memory for 'crpMix'\n");
			fehlerabbruch();
		}

		if((YLD = new double[nCr]) == NULL)
		{	sprintf(fehlertxt, "Error: Not enough memory for 'crpMix'\n");
			fehlerabbruch();
		}

		if((YSM = new double[nCr]) == NULL)
		{	sprintf(fehlertxt, "Error: Not enough memory for 'crpMix'\n");
			fehlerabbruch();
		}

		if((FER = new double*[nCr]) == NULL)
		{	sprintf(fehlertxt, "Error: Not enough memory for 'crpMix'\n");
			fehlerabbruch();
		}

      for (int i = 0; i < nCr; i++)
      {  if((FER[i] = new double[numF]) == NULL)
			{	sprintf(fehlertxt, "Error: Not enough memory for 'crpMix'\n");
				fehlerabbruch();
			}
      }
   }

   virtual ~crpMixNRU()
   {  delete [] crA;
      delete [] cID;
      delete [] cLP;
      delete [] yLP;
      delete [] aHa;
      delete [] LAB;
      delete [] YLD;
      delete [] YSM;

      for (int i = 0; i < nCr; i++)
      {  delete [] FER[i];
      }
      delete [] FER;
   }
};

//-----------------------------------------------------------------------------

class CropSoil
{  protected:
	FILE* datStrm;
   int numTypes;//soil types (from basic data)

   int numCrops;//number of crops in TSPC model
   int numCropA;//number of cropping activities per NRU
   int numMiFer;//number mineral fertilizers
   int numOrFer;//number organic fertilizers
   int numSeasn;//number of cropping seasons per year

   gnrlP* gnrlPars; //general parameters of soil types
   mferP* mferPars; //pointer to mineral fertilizer parameters
   oferP* oferPars; //pointer to organic fertilizer parameters
   yildF* yldFuncs; //pointer to crop yield function parameters
   cropP* cropPars; //pointer to crop physical parameters
   erosP* erosPars; //pointer to crop erosion parameters

   sProc** initPro;  //initial values soil processes per soil type
   lUseH* initLuh;  //initial values land use history per soil type

   caYld** yldExp;  //start values for yield expectations
   inCrA*** craInp;  //inputs to cropping activities

	manure* manureMgt;//parameters for manure management

	void returnFscan(int retVal, char* strVar);

	public:
	CropSoil(void)
	{  numTypes = -1;

      numCrops = -1;
      numCropA = -1;
      numMiFer = -1;
      numOrFer = -1;
      numSeasn = -1;

      gnrlPars = NULL;
      mferPars = NULL;
      oferPars = NULL;
      yldFuncs = NULL;
      cropPars = NULL;
      erosPars = NULL;

	   initPro = NULL;

      yldExp  = NULL;
      craInp  = NULL;

		manureMgt = NULL;
   }

   virtual ~CropSoil()
	{	deallocateMem();
	}

   virtual void readInputFromFile(char*);
   virtual void writeInputIntoFile(char*);

   virtual int getNumCrops();//number of crops in TSPC model
   virtual int getNumCropA();//number of cropping activities per NRU
   virtual int getNumMiFer();//number mineral fertilizers
   virtual int getNumOrFer();//number organic fertilizers
   virtual int getNumSeasn();//number of cropping seasons per year

	///functions related to farm surveys
   virtual int getLpColumnOfFirstCropActivity();
	virtual int getLpColumnOfLastCropActivity();
	virtual int checkSequenceOfCropActivitiesInLp();

	virtual gnrlP& getGnrlPars(int typ);
   virtual double getLimeInFert(int fer);
   virtual sProc* getInitSoilPr(int typ);
   virtual lUseH& getInitLdUseH(int typ);
   virtual caYld  getInitYldExp(int act, int typ);
   virtual int getActID(int cra, int typ);//crop activity ID
	virtual int getCrpID(int cra, int typ);//crop ID, groups of crop activities
   virtual double getInLab(int cra, int typ);
   virtual double getInFer(int cra, int typ, int fer);
	virtual int getZeroLabAct(int typ);//returns activity index of fallow
	
	virtual int getManureApplied(int sType);
	virtual double getManureDistribution(int sType);
   
	virtual void makeYldOfCropMix(int aID, int sID, int kID, int fID,
   								crpMixNRU*& cmNRU, sProc*& sPrNRU, soilC*& sConNRU);
   virtual void writeRowInfoStrm(int aID, int sID, int kID, int fID, int nru,
   								int cID, int typ, int crA);
   virtual void writeEquatInStrm(double rslt);
   virtual void writeNewLineStrm();

	virtual void printToScreen(void);
	virtual void deallocateMem(void);
};



#endif
