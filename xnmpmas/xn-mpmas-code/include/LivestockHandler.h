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
//  File:       LivestockHandler.h (formerly lsfarm.h)
//
//  Contents:   Livestock data class, manages general livestock data such as
//              life tables and computes livestock related coefficients such as
//              expected remaining milk production per head
//
//------------------------------------------------------------------------------

#ifndef _LivestockHandler_h
#define _LivestockHandler_h

#include "MatrixDouble.h"

typedef struct
{  int lsID;      //livestock ID
   double purchP; //purchase price
   int purchA;    //purchase age
	int lfspan;    //lifespan

	int* LProws;   //row in TempLP
   int* LPcols;   //columns in TempLP

   double* weight;//live weight per period
	int* offspr;   //offspring per period

   MatrixDouble annPrds;//"annual" tradable products (i.e., offspring, milk, wool)
   MatrixDouble intPrds;//internal products and factors (i.e. manure, forage energy)
   MatrixDouble cashDmd;//cash demand
   MatrixDouble landDmd;//land demand
   MatrixDouble labDmd; //labor demand

	//Troost 200206 -
	//for maximum instead of annualized in livestock
	//start addition
		MatrixDouble maxDmd; //constraint demands to be considered with maximum (not annualized) in investment LP
	//end addition -Troost 200206

}  lstck;


typedef struct
{  int lsID;      //livestock ID
   int row0;      //row index of first vintage in MILP
   int col0;      //column index of first vintage in MILP
   int colP;		//column index of selling price
}  idxLP;


class lsfarm
{  protected:
	int numTypes;  //number of livestock types
   idxLP* lstckMilp;
   lstck* lstckData;

   int lsProds;
   int lsInPrd;

	//Troost 200206 -
	//for maximum instead of annualized in livestock
	//start addition
		int lsMaxDem;
	//end addition -Troost 200206

	void returnFscan(int retVal, char* strVar);

	public:
	lsfarm(void)
	{  numTypes = 0;
      lstckMilp = NULL;
      lstckData = NULL;
	}

   virtual ~lsfarm()
	{	deallocateMem();
	}

   virtual void readInputFromFile(const char*);
   virtual void writeInputIntoFile(char*);

   virtual int getLsProds();
   virtual int getLsInPrd();

	//Troost 200206 -
	//for maximum instead of annualized in livestock
	//start addition
		virtual int getLsMaxDem();
	//end addition -Troost 200206

   virtual int getNumTypes();
   virtual int getTypeID(int i);
   virtual int getRow0LP(int lsID);
   virtual int getCol0LP(int lsID);
   virtual int getColPLP(int lsID);
   virtual int checkIfLivestock(int lsID);
   virtual double getPurchasePrice(int lsID);
   virtual int getPurchaseAge(int lsID);
   virtual int getLifespan(int lsID);
   virtual int isCulled(int lsID, int age);

   virtual double getActWeight(int lsID,  int age);
   virtual int getNumOffspr(int lsID, int age);

   virtual int getWgtRowLP (int lsID);
   virtual int getWgtColLP (int lsID);
   virtual int getOffRowLP (int lsID);
   virtual int getOffColLP (int lsID);
   virtual int getPrdRowLP (int lsID, int row);
   virtual int getPrdColLP (int lsID, int row);
   virtual int getIntRowLP (int lsID, int row);
   virtual int getIntColLP (int lsID, int row);
   virtual int getCashRowLP(int lsID, int row);
   virtual int getCashColLP(int lsID, int row);
   virtual int getLandRowLP(int lsID, int row);
   virtual int getLandColLP(int lsID, int row);
   virtual int getLabRowLP (int lsID, int row);
   virtual int getLabColLP (int lsID, int row);


	//Troost 200206 -
	//for maximum instead of annualized in livestock
	//start addition
	virtual int getMaxDmdRowLP (int lsID, int row);
	virtual int getMaxDmdColLP (int lsID, int row);
	//end addition -Troost 200206



   //functions return error flag
	virtual int addActOffs(MatrixDouble& mtx, int lsID, int age);
   virtual int addExpOffs(MatrixDouble& mtx, int lsID, int age, double=0.0);
	virtual int addActPrds(MatrixDouble& mtx, int lsID, int age);
   virtual int addExpPrds(MatrixDouble& mtx, int lsID, int age, double=0.0);
	virtual int addActIntPrds(MatrixDouble& mtx, int lsID, int age);
   virtual int addExpIntPrds(MatrixDouble& mtx, int lsID, int age, double=0.0);
	virtual int addActCashDmd(MatrixDouble& mtx, int lsID, int age);
   virtual int addExpCashDmd(MatrixDouble& mtx, int lsID, int age, double=0.0);
	virtual int addActLandDmd(MatrixDouble& mtx, int lsID, int age);
   virtual int addExpLandDmd(MatrixDouble& mtx, int lsID, int age, double=0.0);
	virtual int addActLabDmd(MatrixDouble& mtx, int lsID, int age);
   virtual int addExpLabDmd(MatrixDouble& mtx, int lsID, int age, double=0.0);
	virtual int addActLivWgt(MatrixDouble& mtx, int lsID, int age);
   virtual int addExpLivWgt(MatrixDouble& mtx, int lsID, int age, double=0.0);

	//Troost 200206 -
	//for maximum instead of annualized in livestock
	//start addition
	virtual int addActMaxDmds(MatrixDouble& mtx, int lsID, int age);
	virtual int addExpMaxDmds(MatrixDouble& mtx, int lsID, int age, double=0.0);
	//end addition -Troost 200206

	virtual void printToScreen(void);
	virtual void deallocateMem(void);
};


#endif

