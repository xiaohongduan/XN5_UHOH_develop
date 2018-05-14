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
//  File:       ???.h (formerly lp9.h)
//
//  Contents:   "Old" LP-Solver class, use compiler flag LP9
//
//
//
//------------------------------------------------------------------------------

#ifdef LP9

#ifndef _lp9_h
#define _lp9_h
#include "MatrixDouble.h"
#include "FinancialMathFunctions.h"
   #include "simplx9.h"
#include "BasicFunctions.h"

#include "ekk_c_api.h"
// Troost 20120312 - advanced MilpCheck tuning
// Troost **********begin block insert**********
#include <exception>
// Troost **********end block insert**********
// Troost 20130118 Catch segmentation faults
// Troost **********begin block insert**********
#ifdef AIX
#include <signal.h>
#endif
// Troost **********end block insert**********

	struct Matrixstruct{
		int ncols;						//number of columns
		int nrows;						//number of rows
	};

	struct Resultstruct{
		double obj;						// Objective value
		double* colsol;					// Column solution
		double* LHS;					// Left hand side
		double* RHS;					// Right hand side
		int mp;							// Number of loaded problems
	};

	double get_LHS(int irow);
	double get_RHS(int irow);
	double get_ColSol(int icol);

	int NUM_COUNTER();
	void setNUM_COUNTER(int numC);
	int NUM_LP();
	void setNUM_LP(int numL);

//#define EPS 1.0e-6 //OSL seems to work with 1.0e-7 (see "ekk_c_api.h")
#define NR 2

#if (defined(LP9) || defined(USE_OSL))
#include "ekk_c_api.h"
#endif

// some auxiliary data structures used by both Lp modules
//MILP column indices for performance indicators
typedef struct
{	int numA; //number of activities
	int col0; //first column index
} infoPerf;

//MILP fine-tuning parameters
typedef struct
{	int    rowLP;  //row (=constraint) index
	int    colLP;  //column (=activity) index
	double origV;  //original fine-tuned parameter
	double consV;	//parameter for consumption mode
} infoFine;

// two auxiliary data structures for TSPC
typedef struct
{  //data per cropping activity
	int colLP;  //activity column in LP
	int colOp;  //output column in LP ("Preisertragsspalte")
	int typCr;  //type of crop (seasonal=0; annual=1; permanent=2)
	int roYLD;  //grain yield row in LP
	int roYSM;  //stover energy row in LP
} infoTSPC;

typedef struct
{  int colStv;    //'stover fed to livestock' column in LP
} infoResi;



//auxiliary class for specially ordered set
class sos
{  protected:
   FILE* file;
   int numm;        //number of members in set
   int type;        //type (-1 in case of integer set)
   int prio;        //priority
   int* col_p;      //column indices
   double* dpco_p;  //down pseudocosts
   double* rrow_p;  //reference row  (or up-pseudocosts in case of integer set)

public:
   sos();
   virtual ~sos()
   {  delete [] col_p;
      delete [] dpco_p;
      delete [] rrow_p;
   }

   virtual void readInfoFromFile(FILE*);
   virtual void writeInfoToFile(FILE*);
   virtual int getType();
   virtual int getPriority();
   virtual int getNumber();
   virtual int* getColumns();
   virtual double* getPCost();
   virtual double* getRRow();
};



//Auxiliary structure and class for LP tableau
typedef struct
{  int brows;     //start rows of block
   int bcols;     //start column of block
   MatrixDouble bzrow;  //block z-row
   MatrixDouble bmat;   //block MatrixDouble
} matblockLp9;



class tableauLp9
{  protected:
   int numblx;					//Number of matrix blocks
   int trans;					//transposed (yes/no)
   matblockLp9** matblx;	//Pointer to matrix block pointers
   MatrixDouble infoblx;   //Matrix containing info for each block

   public:
   tableauLp9();
   virtual ~tableauLp9()
   {  deallocateTableau();
   }

   virtual void printToScreen();
   virtual void allocateTableau(int, int);
   virtual void readTableauFromFile(FILE* strm, MatrixDouble& zrx, MatrixDouble& mtx, int nrows, int ncols);
   virtual void writeTableauIntoFile(FILE* strm, const MatrixDouble& zrx, const MatrixDouble& mtx);

   virtual void allocateBlock(int, int, int);
   virtual void readBlockFromFile(int, FILE*);
   virtual void writeBlockIntoFile(int, FILE*);
	virtual void writeBlockIntoFileOnlyNonZeroValues(int, FILE*);

   virtual int getNumBlocks();
   virtual int getIfTrans();
   virtual void writeInfoblxInFile(FILE* strm);

   virtual void copyBlockIntoZRow(int bn, MatrixDouble& zrx, int zrxc);
   virtual void copyBlockIntoMatrix(int bn, MatrixDouble& mtx, int mtxr, int mtxc);

   virtual void copyZRowIntoBlock(int bn, const MatrixDouble& zrx, int zrxc);
   virtual void copyMatrixIntoBlock(int bn, const MatrixDouble& mtx, int mtxr, int mtxc);

   virtual void deallocateTableau();
   virtual void deallocateBlock(int);

	//to be used only when input is read from MPS file
	virtual void setInfoBlxExternally();
};


class mpPrbl
{  protected:
   int	agType;
	FILE* datStrm;//lpdatei;	   //Zeiger auf einen Datenstrom
   char  datName[MXLENGTH];
	
	///Regional market access
	int marketID;

   //basic LP info
   int ncols;      //number of columns
   int nrows;      //number of rows
   int nncols;     //number of nonzero columns
   int nnrows;     //number of nonzero rows
   double pfill;   //percentage of MatrixDouble filled with nonzero values
   
   int capLab;     //row index labor capacity
   int capLiq;     //row index liquidity capacity
   int* capInv;    //row indices for investments in different soil classes

// Troost 20130813 Cash Flow CountryModel Germany
// Troost **********begin block insert**********
	int capCashFlowCoef; //row index cash flow (Country Switch Germany) for coefficients
	int capCashFlowRHS; //row index cash flow (Country Switch Germany) for RHS (fix costs)
	int capCashFlowRHSminCons; //row index cash flow (Country Switch Germany) for RHS (fix costs)
// Troost **********end block insert**********

   int capLnd;	    //Zeilennr. der ersten Bodenkapazitaet
   int capWat;	    //Zeilennr. der ersten Wasserkapazitaet
   int capCsh;	    //Zeilennr. durchs. Kapitalbedarf
   int capAvl;     //Zeilennr. durchs. Arbeitsbedarf
   int capMac;	    //Zeilennr. durchs. Maschinenbedarf der Dauerkulturen
   int actCsh;     //remaining cash activity (LP column)
   
	int anzahlK;	 //Anzahl der betriebsunabhaengigen LP-Kapazitaeten

	double** ap;	 //NR-Ausgangsmatrix
   double** bp;	 //NR-Betriebsmatrix
   tableauLp9 tabMILP;//MILP-Tableau
   MatrixDouble tmp_LP;  //hier werden die Informationen aus LP.DAT reingeschrieben
   // Dimensions: (nrows, ncols);

   MatrixDouble erste_zeile;// Vector with obective function coefficients (OFC)
   //(die Produktpreise werden spaeter durch die
	//Preiserwartungen ersetzt, die restlichen Zielfunktionswerte (i.d.R.
	//Spezialkosten) bleiben unveraendert)
   // dimensions:   (1, ncols)

   int cshSpans;     //number of cash spans
   int labSpans;     //number of labor spans

	int  fixLabor;		//fix binary labor (yes/no)
   int  actFemaY;    //activity index for female adult labor (yes)
   int  actFemaN;    //activity index for female adult labor (no)
   int  actMaleY;    //activity index for male adult labor (yes)
   int  actMaleN;    //activity index for male adult labor (no)

   int  disBinC;     //column index of binary disinvestment activity
   int  disBinR;     //row index of first asset in binary disinvestment activity
	int  ntrDmdR;		//row index of first food nutrient demand
   int  numFixVCons;	//number of variables to be fixed in consumption mode
	int* fixColsCons;	//column indices of fixed variables in consumption mode
	int numFixVMark;	//number of variables to be fixed in marketing mode
	int* fixColsMark;	//column indices of fixed variables in marketing mode

	infoPerf cashOnf;//info on cash on-farm (=short-term credit)
	infoPerf cashOff;//info on cash off-farm (=short-term deposit)
	infoPerf tLabOnf;//info on hiring in temporary labor
	infoPerf tLabOff;//info on hiring out temporary labor
	infoPerf pLabOnf;//info on hiring in permanent labor
	infoPerf pLabOff;//info on hiring out permanent labor

   int  numFine;     //number of fine-tuning parameters
   infoFine* actFine;//pointer to MILP fine-tuning information

   infoTSPC** actTSPC;//pointer to crop information used in TSPC model
   // dimensions: (soilsp->getNumCropA() X soilsp->getNumCropA())
      
   infoResi* actResi;//  "     "  stover      "      "      "
   int       colManu;//manure accounting column in LP

	//additonal information on bounds of columns
   MatrixDouble uppColA;
   // dimensions:              (1, ncols)
   MatrixDouble lowColA;
   // dimensions:  (1, ncols)
   MatrixDouble uppColB;
   // dimensions: (1, ncols)
   MatrixDouble lowColB;
   // dimensions: (1, ncols)

   //Pointers to OSL context and model. Environment is created in
   //zeiger_initialisieren() and deleted in gesamten_speicher_freigeben(). Model
   //is created and deleted in betrieb::solveLpOSL()
   EKKContext* env;
   EKKModel* model;

   double* dels_p;// Matrix elements
   //int tnel: number of MatrixDouble elements (mps-Style)
   //dimensions: (1 X tnel + OFFSET

   int* mrow_p   ;// Row indices
   //dimensions: (1 X  tnel + OFFSET
   int* mcol_p   ;// Column indices
   //dimensions: (1 X  tnel + OFFSET
   double* drlo_p;// Lower bounds on row activities
    //dimensions: (1 X   [nrows + OFFSET]
   double* drup_p;// Upper bounds on row activities
   //dimensions: (1 X nrows + OFFSET
   double* dclo_p;// Lower bounds on columns
   //dimensions: (1 X  [ncols + OFFSET]
   double* dcup_p;// Upper bounds on columns
   //dimensions: (1 X   [ncols + OFFSET]
   double* dobj_p;// LINEAR objective function coefficients
    //dimensions: (1 X   [ncols + OFFSET]

	double* negative_dobj_p; //negative linear objective function coefficients for quadratic problems

	double* dqdg_p;// QUADRATIC objective function coefficients
	double* negative_dqdg_p; //negative quadratic objective function coefficients for quadratic problems

	int* qrow_p;// Row indices of qudratic components
	int* qcol_p;// Column indices of qudratic components

	int tnel;		// Total number of MatrixDouble elements
   int nzel;      // Number of non-zero MatrixDouble elements
   int* vtyp_p   ;// Type of decision variable: 0 = continuous 1 = integer 2 = quadratic continuous 3 = quadratic integer
   //dimensions: (1 X [ncols + OFFSET])
   int* etyp_p   ;// Type of equation (LE = 1, GE = 2, EE = 3)
   //dimensions: (1 X [nrows + OFFSET]
   double* rng_p ;// Range values on the RHS
   //dimensions: (1 X [nrows + OFFSET]

   int imark;     // Flag if ekk_markAsInteger() must be called
   int nint;      // Number of integer sets
   sos* is_p;     // Pointer to integer sets
   int nsos;      // Number of specially ordered sets
   sos* sos_p;    // Pointer to specially ordered sets

   int QuadraticBarrierIsPreferred;
	int numQuadraticElms;//number of non-zero quadratic elements in Q matrix
	int numQuadraticVars;//number of quadratic decision variables
	MatrixDouble qIndices;//Lp columns indices of quadratic variables
	MatrixDouble qMatrix;//Q matrix for standard from

	int numberShadowPrices;
	int firstShadowPrice;
	int shadowPriceLpMode;

	int nudR;      // Number of empty rows to be dropped from model
   int* dropR;    // Array of row indices
      //dimensions: (1 X nrows + OFFSET

	int nudC;      // Number of empty columns to be dropped from model
   int* dropC;    // Array of column indices
      //dimensions: (1 X ncols + OFFSET

	//epsilons for EE contraints (optional feature for large MIP)
	int numberConversionEE;
	MatrixDouble epsilonForEE;

	//LP coefficients to be changed during simulation
	int numberChangingLpCoefficients;
	MatrixDouble changingLpCoefficients;

// Troost 20140505 Age HHead in LP
// Troost **********begin block insert**********
	//Agent attributes to LP
	int numberAgentAttributesToLP;
	MatrixDouble agentAttributesToLP;
// Troost **********end block insert**********

// Troost 20140527 Generalize dynamic NRUs
// Troost **********begin block insert**********
	int numberLandscapeAttributesToLP;
	MatrixDouble landscapeAttributesToLP;
// Troost **********end block insert**********

	string curAgentMipCode;

	void returnFscan(int retVal, char* strVar);

   public:
   mpPrbl(int lpType = agentFarmHousehold, int mID = 0);
   virtual ~mpPrbl()
   {  LP_deallokieren9();
   }

   //function returning basic LP info
   virtual int getNCOLS();   //number of columns
   virtual int getNROWS();   //number of rows
   virtual int getNNCOLS();  //number of nonzero columns
   virtual int getNNROWS();  //number of nonzero rows
   virtual double getPerFil();//percentage of matrix filled with nonzero values

   virtual int getCapLab();    //row index labor capacity
   virtual int getCapLiq();    //row index liquidity capacity
   virtual int getCapInv(int soil);//row indices for investments in different soils
   virtual int getCapLnd();	 //row index first land capacity
   virtual int getCapWat();	 //row index first water capacity
   virtual int getCapCsh();	 //row index average cash demand
   virtual int getCapAvl();    //row index average labor demand
   virtual int getCapMac();	 //row index average demand for fruit equipment M4
   virtual int getActCsh();	 //remaining cash activity (LP column)

   // Troost 20130813 Cash Flow CountryModel Germany
   // Troost **********begin block insert**********
   virtual int getCapCashFlowCoef (); //row index cash flow (Country Switch Germany) for coefficients
   virtual int getCapCashFlowRHS (); //row index cash flow (Country Switch Germany) for RHS (fix costs)
   virtual int getCapCashFlowRHSminCons (); //row index cash flow (Country Switch Germany) for RHS (min consumption)
   // Troost **********end block insert**********

   //reading and writing
	virtual void LP_readInputFromFile(char* filename, int agentType = agentFarmHousehold);
   virtual void LP_writeInputIntoFile(char* filename, int agentType = agentFarmHousehold);
   virtual void LP_makeLinearMatrix();//renamed from <LP_matrizen_anlegen>
	virtual void LP_makeQuadraticMatrix();
	virtual void sendCostDataToMarketHandler();

 	//info on performance indicators
   virtual infoPerf getInfocashOnf();
   virtual infoPerf getInfocashOff();
   virtual infoPerf getInfotLabOnf();
   virtual infoPerf getInfotLabOff();
   virtual infoPerf getInfopLabOnf();
   virtual infoPerf getInfopLabOff();

	//Fine-tuning parameters for MILP
   virtual void switchOffFineTune();
   virtual void switchOnFineTune();

   //TSPC model
   virtual int identCrAcForCol(int colLP, int& aID, int& nru);
   virtual int getColLP(int aID, int nru);
   virtual int getColOp(int aID, int nru);
   virtual int getTypCr(int aID, int nru);
   virtual int getRoYld(int aID, int nru);
   virtual int getRoYsm(int aID, int nru);
   virtual int getColStv(int nru);
   virtual int getColMan();
   virtual void writeTSPCInfoInFile(char* filename);

	//MILP handling
   virtual double getZRowEntry(int);
	virtual double getVarRHS(int r);
   virtual void setVarRHS(int r, double w);

	virtual double getOgMtxCoeff(int r, int c);//refers to original matrix ap
	virtual void setOgMtxCoeff(int r, int c, double w);//refers to original matrix ap   
   virtual double getOgObjCoeff(int c);//refers original matrix ap  
   virtual void setOgObjCoeff(int c, double w);//refers original matrix ap
	virtual double getOgRhsValue(int r);//refers to original matrix ap
	virtual void setOgRhsValue(int r, double w);//refers to original matrix ap   

	virtual double getMtxCoeff(int r, int c);//refers agent matrix bp
   virtual void setMtxCoeff(int r, int c, double w);//refers agent matrix bp
   virtual double getObjCoeff(int c);//refers agent matrix bp
   virtual void setObjCoeff(int c, double w);//refers agent matrix bp

	virtual double getMtxCoeffNR(int r, int c);//"old" NR-style function
   virtual void setMtxCoeffNR(int r, int c, double w);//"old" NR-style function
   //virtual int callSimplxNR(double, int);//"old" NR-style function
	virtual void LP_ausschreiben9();

   virtual int getNumberShadowPrices();
	virtual int getFirstShadowPrice();
	virtual int getShadowPriceLpMode();

// Troost 20140505 Age HHead in LP
// Troost **********begin block insert**********
	virtual int getNumberAgentAttributesToLP();
	virtual int getAgentAttributeToLP(int);
	virtual int getColumnForAgentAttributeToLP(int);
	virtual int getRowForAgentAttributeToLP(int);
// Troost **********end block insert**********

// Troost 20140527 Generalize dynamic NRUs
// Troost **********begin block insert**********
	virtual int getNumberLandscapeAttributesToLP();
	virtual int getParamIDForLandscapeAttributeToLP(int i);
	virtual int getAggregationForLandscapeAttributeToLP(int i);
	virtual int getNRUForLandscapeAttributeToLP(int i);
	virtual int getColumnForLandscapeAttributeToLP(int i);
	virtual int getRowForLandscapeAttributeToLP(int i);
// Troost **********end block insert**********


	virtual void writeLpMatrixToFileAP(char* filename);
	virtual void writeLpMatrixToFileBP(char* filename);//renamed from <LP_datei_schreiben>
   //virtual void Primal_datei_schreiben(char* filename); ## NR-solver
   //virtual void Schatten_datei_schreiben(char* filename);## NR-solver
   virtual void LP_readLpNR(char* filename);
   virtual void copyProblemGenericToSpecific();//renamed from LP_kopieren9
   virtual void LP_Runden1(void);
   virtual void LP_Runden2(void);
   virtual void LP_deallokieren9(void);

	//OSL functions
   virtual void updateOrigMatrix();
	virtual void updateOrigMatrixWithActualPrices();
	virtual void setColUppOSL(int c, double w);
	virtual void setOrigColUppOSL(int c, double w);
   virtual void copyModelOSL(int aID, int sID, int fstID, string agentMipCode ="undef");
// Troost 20130117
// Troost **********begin block insert**********
	//virtual void loadModelOSL(int aID, int sID, int fstID, int max = true, int qp = false); //Default: Maximization
	virtual void loadModelOSL(int aID, int sID, int fstID,  int max = true, int qp = false, int reload = false); //Default: Maximization
// Troost **********end block insert**********
   virtual void testModelOSL(int aID, int sID, int fstID);
   virtual void freeModelOSL();
   virtual void rmEmptyOSL(void);
   virtual int checkPrimalsOSL(void);
	virtual int checkColumnsOSL(void);
   virtual void cleanMemoryOSL(void);

   virtual double solveLpMPI(int agID, int scID, int fstID, int savePrim,
			int saveDual, l_vektor& bprod, double*& varsLHS, double*& dualRHS,
			double*& varsRHS, MatrixDouble& mEntries);

   /*
   virtual double solveLpMPI1(int agID, int scID, int fstID, int savePrim, int saveDual,
            l_vektor& bprod, double*& varsLHS, double*& dualRHS, double*& varsRHS,
            MatrixDouble& mEntries, const char* filename);
   */

   virtual double solveMilpMPI(int agID, int scID, int fstID, int savePrim, int saveDual,
         int& forSol, l_vektor& bprod, double*& varsLHS, double*& dualRHS,
         double*& varsRHS, MatrixDouble& mEntries);

	//with previous call of ekk_eitherSimplex, see sample code EXIMDL.CPP
  virtual int solveMilpMPI1(int agID, int scID, int fstID, int savePrim, int saveDual,
        l_vektor& bprod, double*& varsLHS, double*& dualRHS, double*& varsRHS);

	//with presolving, see sample code EXMIP.CPP
  virtual int solveMilpMPI2(int agID, int scID, int fstID, int savePrim, int saveDual,
        l_vektor& bprod, double*& varsLHS, double*& dualRHS, double*& varsRHS);

	//with transformation to 0/1 MIP problem, see sample code EXGMIP.CPP
  virtual int solveMilpMPI3(int agID, int scID, int fstID, int savePrim, int saveDual,
        l_vektor& bprod, double*& varsLHS, double*& dualRHS, double*& varsRHS);

// Troost 20120312 - advanced MilpCheck tuning
// Troost **********begin block insert **********
  virtual int solveMilpMPI4(int agID, int scID, int fstID, int savePrim, int saveDual,
           l_vektor& bprod, double*& varsLHS, double*& dualRHS, double*& varsRHS);
// Troost **********end block insert **********

	//quadratic solvers without integers
	virtual double solveQuadraticSimplexMPI(int agID, int scID, int fstID, int savePrim, int saveDual,
       l_vektor& bprod, double*& varsLHS, double*& dualRHS, double*& varsRHS,
       MatrixDouble& mEntries);

	virtual double solveQuadraticBarrierMPI(int agID, int scID, int fstID, int savePrim, int saveDual,// see exqbar.cpp for more elaborate version
       l_vektor& bprod, double*& varsLHS, double*& dualRHS, double*& varsRHS,
       MatrixDouble& mEntries);

	//quadratic solver with integers
	virtual double solveQuadraticIntegerMPI(int agID, int scID, int fstID, int savePrim, int saveDual,
       l_vektor& bprod, double*& varsLHS, double*& dualRHS, double*& varsRHS,
       MatrixDouble& mEntries);


   virtual double solveLpOSL(int agID, int scID, int fstID, int savePrim, int saveDual,
         l_vektor& bprod, double*& varsLHS, double*& dualRHS, double*& varsRHS,
         MatrixDouble& mEntries);

	//attempts to solve Milp, calls solveMilpOSLx if problems show up
   virtual double solveMilpOSL(int agID, int scID, int fstID, int savePrim, int saveDual,
         int& forSol, l_vektor& bprod, double*& varsLHS, double*& dualRHS,
         double*& varsRHS, MatrixDouble& mEntries);

	//with previous call of ekk_eitherSimplex, see sample code EXIMDL.CPP
   virtual int solveMilpOSL1(int agID, int scID, int fstID, int savePrim, int saveDual,
         l_vektor& bprod, double*& varsLHS, double*& dualRHS, double*& varsRHS);

	//with presolving, see sample code EXMIP.CPP
   virtual int solveMilpOSL2(int agID, int scID, int fstID, int savePrim, int saveDual,
         l_vektor& bprod, double*& varsLHS, double*& dualRHS, double*& varsRHS);

	//with transformation to 0/1 MIP problem, see sample code EXGMIP.CPP
   virtual int solveMilpOSL3(int agID, int scID, int fstID, int savePrim, int saveDual,
         l_vektor& bprod, double*& varsLHS, double*& dualRHS, double*& varsRHS);

// Troost 20120312 - advanced MilpCheck tuning
// Troost **********begin block insert **********
   virtual int solveMilpOSL4(int agID, int scID, int fstID, int savePrim, int saveDual,
            l_vektor& bprod, double*& varsLHS, double*& dualRHS, double*& varsRHS);
// Troost **********end block insert **********

	//quadratic solvers without integers
	virtual double solveQuadraticSimplex(int agID, int scID, int fstID, int savePrim, int saveDual,
         l_vektor& bprod, double*& varsLHS, double*& dualRHS, double*& varsRHS,
         MatrixDouble& mEntries);
	
	virtual double solveQuadraticBarrier(int agID, int scID, int fstID, int savePrim, int saveDual,// see exqbar.cpp for more elaborate version
         l_vektor& bprod, double*& varsLHS, double*& dualRHS, double*& varsRHS,
         MatrixDouble& mEntries);

	//quadratic solver with integers
	virtual double solveQuadraticInteger(int agID, int scID, int fstID, int savePrim, int saveDual,
         l_vektor& bprod, double*& varsLHS, double*& dualRHS, double*& varsRHS,
         MatrixDouble& mEntries);

	//checking type of problem
	virtual int hasPrblInts();//Original problem in input file with marked integer variables?
   virtual int hasPrblSets();//Original problem in input file with integer or special ordered sets?
   virtual int getNumQuadraticVars();//Original problem in input file with quadratic variables?
	virtual int checkIfIntegerProblem();//checks the loaded OSL problem of an agent, if not available input file
	virtual int checkIfQuadraticProblem();//checks the loaded OSL problem  of an agent, if not available input file
	virtual int preferQuadraticBarrier(); //check whether QuadraticBarrier Algorithm is to be preferred over Simplex for continuous quadratic problems

	//solver solution
   virtual double getObjValue();
   virtual double getPrintCPU();
   virtual void setPrintCPU(double time);
   virtual double forSolutionOSL(int, int, int,
   		l_vektor&, double*&, double*&, double*&, MatrixDouble& mEntries);
   virtual double optSolutionOSL(int agID, int scID, int fstID, int savePrim, int saveDual,
      double prp, l_vektor& bprod, double*& varsLHS, double*& schattenpreise,
      double*& varsRHS);

   //labor and cash and labor spans
   virtual int getCshSpans();
   virtual int getLabSpans();

   //fixing binary activities for labor force
   virtual int fixAdultBin(); //fix adult labor binaries (yes(no)
   virtual int getActFemaY();//acitivity index for female adult labor (yes)
   virtual int getActFemaN();//acitivity index for female adult labor (no)
   virtual int getActMaleY();//acitivity index for male adult labor (yes)
   virtual int getActMaleN();//acitivity index for male adult labor (no)

   //consumption model
   //virtual int getDisBinC(); ## Tbe 121210 not needed anymore
   //virtual int getDisBinR(); ## Tbe 121210 not needed anymore
	virtual int getNtrDmdR();

	//modifies Milp tableau before loading OSL model
	virtual void changeObjFuncOSL(double*& objF, int oSta, int oEnd);
   virtual void fixLaborBinarOSL(double femLab, double malLab);
   virtual void fixProducVarsInConsumptionModeOSL(l_vektor& bprod);
	virtual void fixProducVarsInMarketingModeOSL(l_vektor& bprod);
   virtual void changeElemMtxOSL(int num, int*& rows, int*& cols, double*& elms);
	virtual void setToEEConstrOSL(int rLP, double value);
	
	//modifies loaded OSL model using ekk_functions
   virtual int modifySolVectEKK(int num, int*& cols, double*& values);// not in use
   virtual int changeObjFuncEKK(double*& objF, int oSta, int oEnd);// not in use
   virtual int fixProducVarsEKK(l_vektor& bprod);// not in use
   virtual int changeElemMtxEKK(int num, int*& rows, int*& cols, double*& elms);// not in use
   virtual int setToEEConstrEKK(int rLP, double value);// not in use
	virtual int applyEpsilonToEEConstrEKK(int rLP, double epsilon);

   //write LP model to file in Excel style
   virtual void writeModelInFile(char* filename, int withText);
   virtual void writePrimsInFile(char* filename);
   virtual void writeDualsInFile(char* filename);

   // Write into stream, with two formats
   // ... default: format (XLA)
   virtual void writeDualsIntoStream( FILE*  datei);
   // ... formatType == true : default format (XLA)
   // ... formatType == false:   block format (tsv in row)
   virtual void writeDualsIntoStream( FILE*  datei, bool formatType);

   virtual void writePrimsIntoStream( FILE*  datei);
   virtual void writePrimsIntoStream( FILE*  datei, bool formatType);

   //for models with less than 256 columns (including RHS and LHS)
   virtual void writeModelIn256Cols(char* filename);
   virtual void writePrimsIn256Cols(char* filename);
   virtual void writeDualsIn256Cols(char* filename);

   //export OSL model to file in Excel style
   virtual void exportOSLToFile(char* filename, int withText);

   //special functions only for 'MilpCheck' and 'MilpCvter' applications
   virtual void initContext();

	//virtual void maxModelOSL(int aID, int sID); ## Tbe 120602
	virtual void newModelOSL(int aID, int sID);//refactored from <maxModelOSL>
	virtual void setModelMaximize(int aID, int sID);//refactored from <maxModelOSL>

   virtual void LP_readMilpExl(char* filename);
   virtual void LP_readMilpText(char* filename);

   virtual void copyVarsRHS(double*& varsRHS);
   virtual void saveMilpMPS(char* filename);
   virtual void LP_readMilpMPS(const char* filename, int quadratic = false);

   //some functions for debugging
   virtual void cleanArraysForMilp(double dval, int ival);
	virtual void cleanMatrixForMilp(double dval, int ival);
   virtual void checkArraysForMILP(double dval, int ival);
   virtual int  checkVectorForMilp(double*& array, int size, int start, double dval);
   virtual int  checkVectorForMilp(int*& array, int size, int start, int ival);
   virtual void writeArraysInFiles(char* prefix);

   //write changes of MILP values into file
   virtual void writeUpElemsInFile(int aID, int sID);
   virtual void writeUpCoeffInFile(int r, int c, double d);
   virtual void writeUpRowupInFile(int r, double d);
   virtual void writeUpRowloInFile(int r, double d);
   virtual void writeUpColupInFile(int c, double d);
   virtual void writeUpColloInFile(int c, double d);
   virtual void writeUpObjFcInFile(int c, double d);
   virtual void writeUpRhsVcInFile(int r, double d);
   virtual void writeUpSolVcInFile(int c, double d);
   virtual void closeUpElemsStream();

};


// Troost 20130118 Catch segmentation faults
// Troost **********begin block insert**********
#ifdef AIX

void segfault_handler(int signum, siginfo_t *info, void *);
void segfault_handler2(int signum, siginfo_t *info, void *);

#endif
// Troost **********end block insert**********
#endif /* #ifndef _lp9_h */
#endif  //LP9

