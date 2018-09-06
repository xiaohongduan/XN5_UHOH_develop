//------------------------------------------------------------------------------
//
//  Thomas Berger (main author), Pepijn Schreinemachers, Thorsten Arnold, Christian Troost
//
//
//  Hohenheim University (490d)
//  Professorship for Land Use Economics in the Tropics and Subtropics
//
//  May not be distributed without permission by main author
//
//  File:       AgentMipProblem.h (rewrite of lp9.h)
//
//  Contents:   Interface between agents and solvers
//
//
//
//------------------------------------------------------------------------------

#ifndef LP9
#ifndef _AgentMipProblem_h
#define _AgentMipProblem_h

#include "MatrixDouble.h"
#include "FinancialMathFunctions.h"
#include "simplx9.h"
#include "BasicFunctions.h"
#include "BasicDefines.h"

#ifdef USE_OSL
	#include "ekk_c_api.h"
#endif
#if defined(USE_CPLEX) && !defined(LPPACKED)
 #error USE_CPLEX only with LPPACKED
#endif
#if defined(USE_SYM) || defined(USE_CBC) || defined(LPPACKED)
	#include "CoinPackedMatrix.hpp"
	#include <fstream>
	#include <sstream>
	#include <string>
	#include <vector>
#endif
#ifdef USE_SYM
	#include "OsiSymSolverInterface.hpp"
	#include "symphony.h" //for now, for infinity
   #include "SymConfig.h" //for symphony version number

#endif
#ifdef USE_CBC
	#include "CbcModel.hpp"
	#include "OsiClpSolverInterface.hpp"
	#include "CbcSolver.hpp"
   #include "CbcConfig.h" //for Cbc version number
	#define CBCCALL2
#endif
#ifdef USE_CPLEX
	#include "OsiCpxSolverInterface.hpp"
#endif
#ifdef MULTIPERIOD
	#include "MultiperiodMipVariableCoefficient.h"
	#include "MultiperiodSolutionReader.h"
	#include "CropSoil.h"
#endif
#ifdef ODBSTORE
	#include <odb/core.hxx>
	#include <odb/callback.hxx>
#endif

//Note: MPMAS_INFINITY is the value used in the input files
//It has to be translated to the solver-specific value of infinity
//This is done in AgentMipProblem::copyModel
#ifdef USE_OSL
	#define MPMAS_INFINITY OSL_INFINITY //for now
#else
	#define MPMAS_INFINITY 1.0e31 //same as OSL_INFINITY
#endif //USE_OSL
#ifndef USE_SYM
	#define SYM_INFINITY 1.0e20
#endif
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

	double get_LHS(int irow);
	double get_RHS(int irow);
	double get_ColSol(int icol);

	int NUM_COUNTER();
	void setNUM_COUNTER(int numC);
	int NUM_LP();
	void setNUM_LP(int numL);

//#define EPS 1.0e-6 //OSL seems to work with 1.0e-7 (see "ekk_c_api.h")
#define NR 2

class agentF; //forward decl

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

   void setIntInfo(int c, int p, double pd, double pu) ;// shorthand populator for integer sets with on member

   virtual void readInfoFromFile(FILE*);
   virtual void writeInfoToFile(FILE*);
   virtual int getType();
   virtual int getPriority();
   virtual int getNumber();
   virtual int* getColumns();
   virtual double* getPCost();
   virtual double* getRRow();
   virtual int getColumn(int m);
   virtual double getPCost(int m);
   virtual double getRRow(int m);
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
   virtual void readTableauFromFile(FILE* strm, MatrixDouble& zrx, MatrixDouble& mtx, int nrows, int ncols, string fn);
   virtual void writeTableauIntoFile(FILE* strm, const MatrixDouble& zrx, const MatrixDouble& mtx);

   virtual void allocateBlock(int, int, int);
   virtual void readBlockFromFile(int, FILE*, string);
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
#ifdef USE_SYM
struct symphonyOption
{
		string name;
		bool isDouble;
		union
		{	int as_int;
			double as_double;
		} value;
		symphonyOption(string s_, bool i_, int v_ )
		{
			name = s_;
			isDouble = i_;
			value.as_int = v_;
		};
		symphonyOption(string s_, bool i_, double v_ )
		{
			name = s_;
			isDouble = i_;
			value.as_double = v_;
		};
};
#endif //USE_SYM
#ifdef USE_CBC
#ifdef CBCCALL2
struct cbcArgumentArray {
		bool useIntegerPriorities;
		int numArgs;
		char** args;
		cbcArgumentArray(string commandLine );
		~cbcArgumentArray()
		{
			for (int i = 0; i< numArgs; i++)
				delete[] args[i];

			delete[] args;
			args = NULL;
		};

};
#endif
#endif

#ifdef USE_CPLEX
struct cplexOption
{
		int refNumber;
		string name;
		bool isDouble;
		union
		{	int as_int;
			double as_double;
		} value;
		cplexOption(string s_, int refNum_, bool i_, int v_ )
		{
			refNumber = refNum_;
			name = s_;
			isDouble = i_;
			value.as_int = v_;
		};
		cplexOption(string s_, int refNum_, bool i_, double v_ )
		{
			name = s_;
			refNumber = refNum_;
			isDouble = i_;
			value.as_double = v_;
		};
};
#endif //USE_CPLEX


#if (defined(LP9) || defined(USE_OSL))
#include "ekk_c_api.h"
#endif

// some auxiliary data structures used by both Lp modules
//MILP column indices for performance indicators
#pragma db value
typedef struct
{	int numA; //number of activities
	int col0; //first column index
} infoPerf;

//MILP fine-tuning parameters
#pragma db value
typedef struct
{	int    rowLP;  //row (=constraint) index
	int    colLP;  //column (=activity) index
	double origV;  //original fine-tuned parameter
	double consV;	//parameter for consumption mode
} infoFine;

// two auxiliary data structures for TSPC
#pragma db value
typedef struct
{  //data per cropping activity
	int colLP;  //activity column in LP
	int colOp;  //output column in LP ("Preisertragsspalte")
	int typCr;  //type of crop (seasonal=0; annual=1; permanent=2)
	int roYLD;  //grain yield row in LP
	int roYSM;  //stover energy row in LP
} infoTSPC;

#pragma db value
typedef struct
{  int colStv;    //'stover fed to livestock' column in LP
} infoResi;

#ifdef ODBSTORE
#pragma db value
struct matrixTriplet
{		//for storing the matrix file
		int row;
		int col;
		double value;
		matrixTriplet() : row(-1), col(-1), value(0.0) {};
		matrixTriplet(int r_, int c_, double v_) : row(r_), col(c_), value(v_) {};
} ;
#pragma db value
struct intInfo
{		//for storing integer sets information
		int col;
		int priority;
		double pseudo_down;
		double pseudo_up;
		intInfo() : col(-1), priority(1000), pseudo_down(0.0001), pseudo_up(0.0001) { }
		intInfo(int c, int p, double pd, double pu) : col(c), priority(p), pseudo_down(pd), pseudo_up(pu) { }
};
/*struct sosInfo
{		//for storing sos information
	   int id;
	   int numm;        //number of members in set
	   int type;        //type (-1 in case of integer set)
	   int priority;        //priority
	   sosInfo(int i, int n, int t, int p) : id(i), numm(n), type(t),priority(p) { }
};
struct sosMemberInfo
{		//for storing sos member information
		int sosId;		 // Id of set
	   int col_p;      //column indices
	   double dpco_p;  //down pseudocosts
	   double rrow_p;  //reference row  (or up-pseudocosts in case of integer set)
	   sosMemberInfo(int i, int c, double pd, double rr) :sosId(i), col_p(c), dpco_p(pd), rrow_p(rr) {}
};*/
#endif

#pragma db object
class mpPrbl
{
#ifdef ODBSTORE
	private:
		friend class odb::access;
#endif
	protected:
   int	agType;
#pragma db transient
	FILE* datStrm;//lpdatei;	   //Zeiger auf einen Datenstrom
#pragma db transient
   char  datName[MXLENGTH];
	
	///Regional market access
	int marketID;

#pragma db id
	int mipID;

   //basic LP info
   int ncols;      //number of columns
   int nrows;      //number of rows

   int nncols;     //number of nonzero columns
   int nnrows;     //number of nonzero rows
   double pfill;   //percentage of MatrixDouble filled with nonzero values
   

   int capLab;     //row index labor capacity
   int capLiq;     //row index liquidity capacity
   vector<int> capInv;    //row indices for investments in different soil classes

	int capCashFlowCoef; //row index cash flow (Country Switch Germany) for coefficients
	int capCashFlowRHS; //row index cash flow (Country Switch Germany) for RHS (fix costs)
	int capCashFlowRHSminCons; //row index cash flow (Country Switch Germany) for RHS (fix costs)

   int capLnd;	    //Zeilennr. der ersten Bodenkapazitaet
   int capWat;	    //Zeilennr. der ersten Wasserkapazitaet
   int capCsh;	    //Zeilennr. durchs. Kapitalbedarf
   int capAvl;     //Zeilennr. durchs. Arbeitsbedarf
   int capMac;	    //Zeilennr. durchs. Maschinenbedarf der Dauerkulturen
   int actCsh;     //remaining cash activity (LP column)
   
	int anzahlK;	 //Anzahl der betriebsunabhaengigen LP-Kapazitaeten


#ifdef MULTIPERIOD
#ifdef ODBSTORE
	#error "ODBSTORE not yet implemented for MULTIPERIOD"
#endif
   int numPeriodsInMIP;
   vector<int> startColsPeriodInMIP;
   vector<int> startRowsPeriodInMIP;

   vector<MultiperiodMipVariableValue*> remainingVariableCoefficients;
   vector<MultiperiodMipVariableValue*> marketPricesToBeUpdated;
   vector<MultiperiodMipVariableValue*> cashReservesToBeUpdated;
   vector<MultiperiodMipVariableValue*> labCapsToBeUpdated;
   vector<MultiperiodMipVariableValue*> plotAreasToBeUpdated;
   vector<MultiperiodMipVariableValue*> interestRatesToBeUpdated;
   vector<MultiperiodMipVariableValue*> valuesFromPastSolutionToBeUpdated;
   vector<MultiperiodMipVariableValue*> agentsPriceExpectationsToBeUpdated;
 /*  vector<MultiperiodMipVariableValue*> debtServiceToBeUpdated;*/
   vector<MultiperiodMipVariableValue*> depreciationToBeUpdated;
   vector<MultiperiodMipVariableValue*> principalToBeUpdated;
   vector<MultiperiodMipVariableValue*> interestOnLoansToBeUpdated;
   vector<MultiperiodMipVariableValue*> rentalPaymentsToBeUpdated;
   vector<MultiperiodMipVariableValue*> minConsToBeUpdated;
   vector<MultiperiodMipVariableValue*> agentsYieldExpectationsToBeUpdated;
   vector<MultiperiodMipVariableValue*> agentsYieldsToBeUpdated;
   vector<MultiperiodMipVariableValue*> userDefinedHouseholdAttributes;
   vector<MultiperiodMipVariableValue*> agentsLivSimMilkYieldToBeUpdated;

   vector<MultiperiodMipVariableValue*>cyclicDataToBeUpdated;
   vector<MultiperiodMipVariableValue*>changingDataToBeUpdated;
   vector<MultiperiodMipVariableValue*>stageSpecificDataToBeUpdated;

   vector<MultiperiodMipVariableValue*> agentCharacteristicsToBeUpdated;
   vector<MultiperiodMipVariableValue*> nruAttributesToBeUpdated;
   vector<MultiperiodMipVariableValue*> nruAttributesRangeTestToBeUpdated;
   vector<MultiperiodMipVariableValue*> nruDistanceToBeUpdated;
   vector<MultiperiodMipVariableValue*> nruDistanceRangeTestToBeUpdated;

   vector<MultiperiodMipVariableValue*> cropActAttributesToBeUpdated;
   vector<MultiperiodMipVariableValue*> cropActAttributesRangeTestToBeUpdated;
   vector<MultiperiodMipVariableValue*> cropActAttributesExpectationsToBeUpdated;
   vector<MultiperiodMipVariableValue*> cropActAttributesExpectationsRangeTestToBeUpdated;


   vector<vector<vector<MultiperiodMipVariableValue*> > > assetCapacitiesToBeUpdated;
   vector<vector<vector<MultiperiodMipVariableValue*> > > assetVintagesToBeUpdated;

   MultiperiodSolutionReader readerIncome;
   MultiperiodSolutionReader readerIncomeOffFarm;
   MultiperiodSolutionReader readerIncomeTaxes;
   MultiperiodSolutionReader readerIncomeInKind;
   MultiperiodSolutionReader readerCashCarryOver;
   MultiperiodSolutionReaderWithMultiplier readerLaborOffFarm;
   MultiperiodSolutionReaderWithMultiplier readerLaborOnFarm;
   MultiperiodSolutionReader readerCapitalOffFarm;
   MultiperiodSolutionReader readerCapitalBorrowed;
   MultiperiodSolutionReader readerCashConsumption;
   MultiperiodSolutionReader readerShortTermInterestPaid;

   MultiperiodSolutionReaderByArgs readerCropNRU;

   MultiperiodSolutionReaderByArgs readerAssetInvestments;
   MultiperiodSolutionReaderByArgs readerAssetDisinvestments;
   MultiperiodSolutionReaderByArgs readerAssetKeep;
   MultiperiodSolutionReaderByArgs readerAssetTransform;
   MultiperiodSolutionReaderByArgs readerInnovationArea;
   MultiperiodSolutionReaderByArgs readerResultOutput;
   MultiperiodSolutionReaderByArgs readerResultToExternalScript;

   MultiperiodSolutionReaderByArgs readerLivestockAssetsToLivSimHerdGroups;
   MultiperiodSolutionReaderByArgs readerLivSimHerdGroupsToNRU;
   MultiperiodSolutionReaderByArgs readerLivSimHerdGroupFeeding;

   MultiperiodSolutionReaderByArgs readerAgentCharacteristicSet;
   MultiperiodSolutionReaderByArgs readerAgentCharacteristicAdd;

#endif //MULTIPERIOD

#ifdef LPPACKED
#pragma db transient
	CoinPackedMatrix genericCoeffMatrix;
	vector<double> genericRHS;
	vector<double> genericObjFunc;
#ifdef ODBSTORE

#pragma db table("genericCoeffMatrix")
	mutable vector<matrixTriplet> coeffMatrixForStorage;
#endif

#pragma db transient
	CoinPackedMatrix specificCoeffMatrix;
#pragma db transient
	vector<double> specificObjFunc;
#pragma db transient
	vector<double> specificRHS;
#else
#ifdef ODBSTORE
	#error "ODBSTORE not possible without LPPACKED"
#endif
	double** ap;	 //NR-Ausgangsmatrix
   double** bp;	 //NR-Betriebsmatrix
#endif


   int tripletFormat;
#pragma db transient
   tableauLp9 tabMILP;//MILP-Tableau

#pragma db transient
   MatrixDouble tmp_LP;  //hier werden die Informationen aus LP.DAT reingeschrieben
   // Dimensions: (nrows, ncols);
#pragma db transient
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

 //  int  disBinC;     //column index of binary disinvestment activity
 //  int  disBinR;     //row index of first asset in binary disinvestment activity
	int  ntrDmdR;		//row index of first food nutrient demand
 //  int  numFixVCons;	//number of variables to be fixed in consumption mode
	vector<int> fixColsCons;	//column indices of fixed variables in consumption mode
	//int numFixVMark;	//number of variables to be fixed in marketing mode
	vector<int> fixColsMark;	//column indices of fixed variables in marketing mode

	infoPerf cashOnf;//info on cash on-farm (=short-term credit)
	infoPerf cashOff;//info on cash off-farm (=short-term deposit)
	infoPerf tLabOnf;//info on hiring in temporary labor
	infoPerf tLabOff;//info on hiring out temporary labor
	infoPerf pLabOnf;//info on hiring in permanent labor
	infoPerf pLabOff;//info on hiring out permanent labor

   int  numFine;     //number of fine-tuning parameters
   vector<infoFine> actFine;//pointer to MILP fine-tuning information

   //ODBSTORE missing: TSPC
#pragma db transient
   infoTSPC** actTSPC;//pointer to crop information used in TSPC model
   // dimensions: (soilsp->getNumCropA() X soilsp->getNumCropA())

   vector<infoResi> actResi;//  "     "  stover      "      "      "
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

#if !defined(LPPACKED) || defined(USE_OSL)
   double* dels_p;// Matrix elements
   //int tnel: number of MatrixDouble elements (mps-Style)
   //dimensions: (1 X tnel + OFFSET
   int* mrow_p   ;// Row indices
   //dimensions: (1 X  tnel + OFFSET
   int* mcol_p   ;// Column indices
   //dimensions: (1 X  tnel + OFFSET
#endif
#pragma db transient
   double* drlo_p;// Lower bounds on row activities
    //dimensions: (1 X   [nrows + OFFSET]
#pragma db transient
   double* drup_p;// Upper bounds on row activities
   //dimensions: (1 X nrows + OFFSET
#pragma db transient
   double* dclo_p;// Lower bounds on columns
   //dimensions: (1 X  [ncols + OFFSET]
#pragma db transient
   double* dcup_p;// Upper bounds on columns
   //dimensions: (1 X   [ncols + OFFSET]
#pragma db transient
   double* dobj_p;// LINEAR objective function coefficients
    //dimensions: (1 X   [ncols + OFFSET]
#pragma db transient //TODO: make quadratic possible
	double* negative_dobj_p; //negative linear objective function coefficients for quadratic problems
#pragma db transient //TODO: make quadratic possible
	double* dqdg_p;// QUADRATIC objective function coefficients
#pragma db transient //TODO: make quadratic possible
	double* negative_dqdg_p; //negative quadratic objective function coefficients for quadratic problems
#pragma db transient //TODO: make quadratic possible
	int* qrow_p;// Row indices of qudratic components
#pragma db transient //TODO: make quadratic possible
	int* qcol_p;// Column indices of qudratic components
#if  !defined(LPPACKED) ||  defined(USE_OSL)
	int tnel;		// Total number of MatrixDouble elements
   int nzel;      // Number of non-zero MatrixDouble elements
#endif
   vector<int> vtyp_p   ;// Type of decision variable: 0 = continuous 1 = integer 2 = quadratic continuous 3 = quadratic integer
   //dimensions: (1 X [ncols + OFFSET])
   vector<int> etyp_p   ;// Type of equation (LE = 1, GE = 2, EE = 3)
   //dimensions: (1 X [nrows + OFFSET]
   vector<double> rng_p ;// Range values on the RHS
   //dimensions: (1 X [nrows + OFFSET]

   int imark;     // Flag if ekk_markAsInteger() must be called
   int nint;      // Number of integer sets
#pragma db transient
   sos* is_p;     // Pointer to integer sets
   int nsos;      // Number of specially ordered sets
#pragma db transient
   sos* sos_p;    // Pointer to specially ordered sets
#ifdef ODBSTORE
#pragma db table ("integer_sets")
   mutable  vector<intInfo>is_p_forStorage;
#endif


   int QuadraticBarrierIsPreferred;
	int numQuadraticElms;//number of non-zero quadratic elements in Q matrix
	int numQuadraticVars;//number of quadratic decision variables
	MatrixDouble qIndices;//Lp columns indices of quadratic variables
	MatrixDouble qMatrix;//Q matrix for standard from

	int numberShadowPrices;
	int firstShadowPrice;
	int shadowPriceLpMode;

	int nudR;      // Number of empty rows to be dropped from model
#pragma db transient
   int* dropR;    // Array of row indices
      //dimensions: (1 X nrows + OFFSET

	int nudC;      // Number of empty columns to be dropped from model
#pragma db transient
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

// Troost 20141210 Land markets - type 4, assets traded with land
// Troost **********begin block insert**********
	int numberPlotCapsToLP;
	MatrixDouble plotCapacityConstraints;
// Troost **********end block insert**********

// Troost 20150218 MEKA more realistic
// Troost **********begin block insert**********
	int numberRemDurationConstraints;
	MatrixDouble remDurationConstraints;
// Troost **********end block insert**********

// Troost 20170516 Advanced Demography Model 2
// Troost **********begin block insert**********
	int numberHouseholdMembersDecisionsFromLP;
	MatrixDouble householdMembersDecisionsFromLP;


	int numberLandAndAssetSplittingActivities;
	MatrixDouble householdLandAndAssetSplitting;

	int numberHouseholdMembersToLP;
	MatrixDouble householdMembersToLP;

	int numberFarmsteadNRUinfo;
	MatrixDouble farmsteadNRUinfo;

	int numberLPColsLandDistribution;
	int maxID_LandDistribution;
	MatrixDouble applicationForLandDistributionCols;
// Troost **********end block insert**********

// Troost 20180418 External script calls
// Troost **********begin block insert**********
	int numberSolutionsToExternalScriptPreInvest;
	MatrixDouble solutionsToExternalScriptPreInvest;
	int numberUboundsPreInvest;
	MatrixDouble uboundsPreInvest;
	vector<string> solutionFileToExternalScriptPreInvestColumnNames;


// Troost **********end block insert**********

// Troost 20170310 Cplex Mip starts
// Troost **********begin block insert**********
#ifdef MIPSTART
	int numberLandMarketPlotOfferColumns;
	int firstLandMarketPlotOfferColumn;
#endif
// Troost **********end block insert**********
	string curAgentMipCode;
	SolverType solverLastSolved;
	static SolverType preferredSolver;

#ifdef USE_OSL
   //Pointers to OSL context and model.
#pragma db transient
   EKKContext* env;
#pragma db transient
   EKKModel* model;
	int oslContextInitialized;
	int oslModelLoaded;
#endif

#ifdef USE_SYM
#pragma db transient
	OsiSymSolverInterface* symIntf;
#ifndef LPPACKED
	CoinPackedMatrix* loadedMatrixSym;
#endif
	int symContextInitialized;
	int symModelLoaded;
	bool symMax;
#pragma db transient
	static bool symphonyStopOnFirstFailed;
#pragma db transient
	static bool symphonyStopOnSecondFailed;
#pragma db transient
	static bool symphonyStopOnLimitReached;
#pragma db transient
	static string symphonyOptionsFile;
#pragma db transient
	static vector<symphonyOption> symphonyStartOptions;
#pragma db transient
	static vector<symphonyOption> symphonyFailedFirstOptions;
#pragma db transient
	static vector<symphonyOption> symphonyFailedSecondOptions;
#pragma db transient
	static vector<symphonyOption> symphonyLimitReachedOptions;
#endif
#ifdef USE_CBC
#pragma db transient
	CbcModel* cbcIntf;
#pragma db transient
	OsiSolverInterface* clpIntf;
#ifndef LPPACKED
	CoinPackedMatrix* loadedMatrixCbc;
#endif
#pragma db transient
	int* priorities;
	//int cbcContextInitialized;
#pragma db transient
	int cbcModelLoaded;
#pragma db transient
	static string cbcOptionsFile;
#ifdef CBCCALL2
#pragma db transient
	static vector<cbcArgumentArray* > cbcCommandlines;
#pragma db transient
	static int instanceCounter;
#else
	static vector<string>cbcCommandlines;
#endif//CBCCALL2
#endif
#ifdef USE_CPLEX
#pragma db transient
	OsiCpxSolverInterface* cpxIntf;
	CoinMessageHandler* coinMsg;
	int cpxContextInitialized;
	int cpxModelLoaded;
	bool cpxMax;
#pragma db transient
	static bool cplexStopOnFirstFailed;
#pragma db transient
	static bool cplexStopOnSecondFailed;
#pragma db transient
	static bool cplexStopOnLimitReached;
#pragma db transient
	static string cplexOptionsFile;
#pragma db transient
	static vector<cplexOption> cplexStartOptions;
#pragma db transient
	static vector<cplexOption> cplexFailedFirstOptions;
#pragma db transient
	static vector<cplexOption> cplexFailedSecondOptions;
#pragma db transient
	static vector<cplexOption> cplexLimitReachedOptions;
#pragma db transient
	int* priorityIntegers;
#pragma db transient
	int* priorityIndices;

#endif//USE_CPLEX
#if defined(LPPACKED) && defined(USE_OSL)
	bool usePacked; //only if LPPACKED is used is CoinPackedMatrix used directly (Cbc, Sym) or do we use dels_p (for OSL)
#endif
	void returnFscan(int retVal, char* strVar);

#ifdef MIPSTART
	double* mipstartSolution1;
	double* mipstartSolution2;
	int* mipStartIndices;
	int mipstartEffortLevel[2];
	char* mipstartNames[2];
#endif //MIPSTART


#ifdef MULTIPERIOD
	vector<pair<string, int> > resultOutputAggregates; // contains the name of the variable
																		// and its index in the resultArray communicated from the reader function
	int resultAggregatePeriod;
	string resultAggregateScenario;
	string resultAggregateDecisionStage;

	int decisionOutputLongFormat;

	ofstream resultAggregateStream;
	ofstream resultDecisionsStream;
	double* resultAggregateArray; //filled by reader function

	vector<vector<double> > seasonalValueDatabase;
	vector<vector<double> > decisionStageDatabase;
#endif

public:

	mpPrbl(int lpType = agentFarmHousehold, int mID = 0);

   virtual ~mpPrbl()
   {  LP_deallokieren9();
#ifdef USE_CPLEX
   	delete coinMsg;
   	coinMsg = NULL;
#endif
   }

//BASIC INFO
#ifdef MULTIPERIOD
   int getNumPeriodsInMIP();
   int getColsOfZeroPeriodInMIP();
#endif

#ifdef ODBSTORE
   void odbCallback(odb::callback_event event, odb::database& db);
   void odbCallback(odb::callback_event event, odb::database& db) const;
#endif

  //function returning basic LP info
   virtual int getNCOLS();   //number of columns
   virtual int getNROWS();   //number of rows
   virtual int getNNCOLS();  //number of nonzero columns
   virtual int getNNROWS();  //number of nonzero rows
   virtual double getPerFil();//percentage of matrix filled with nonzero values
   virtual int getNumQuadraticVars();//Original problem in input file with quadratic variables?
	virtual int hasPrblInts();//Original problem in input file with marked integer variables?
   virtual int hasPrblSets();//Original problem in input file with integer or special ordered sets?

   //functions for getting and setting solver options
 	//static void setDefaultSolver(SolverType s); ## Tbe 150116 unsafe function
 	static void setDefaultSolver(int i);
 	static SolverType getDefaultSolver();
	virtual int preferQuadraticBarrier(); //check whether QuadraticBarrier Algorithm is to be preferred over Simplex for continuous quadratic problems

//READING AND WRITING
	//writing the model to file or screen
	virtual void LP_ausschreiben9(); // print to screen
	virtual void writeLpMatrixToFileAP(char* filename);
	virtual void writeLpMatrixToFileBP(char* filename);//renamed from <LP_datei_schreiben>

#ifndef MCHECK
   //reading and writing MPMAS input
	virtual void LP_readInputFromFile(char* filename, int agentType = agentFarmHousehold);
   virtual void LP_writeInputIntoFile(char* filename, int agentType = agentFarmHousehold);
#ifdef MULTIPERIOD
   void readMpmasInsertValuesFromStream( FILE* strm, string filename);
   void readMpmasReadValues( FILE* strm, string filename);
   void readMpmasReadResultAggregates( FILE* strm, string filename);
   void readMpmasSeasonalValueDatabase( FILE* strm, string filename);
   void readMpmasDecisionStageDatabase( FILE* strm, string filename);
#endif
   //?	virtual void LP_readLpNR(char* filename);
#else
	//read LP model to file in Excel style (MilpCheck)
   virtual void LP_readMilpExl(char* filename);
   virtual void LP_readMilpText(char* filename);
#endif

	//write LP model to file in Excel style (MilpCheck)
	virtual void writeModelInFile(char* filename, int withText);
	virtual void writeModelIn256Cols(char* filename);

   //writing MPS from solver
   virtual void saveMilpMPS(char* filename);
   virtual void LP_readMilpMPSDef(const char* filename, int quadratic = false);
   virtual void LP_readMilpMPS(SolverType whichSolver, const char* filename, int quadratic = false);

   //write solutions
	// ... formatType == true : default format (XLA)
	// ... formatType == false:   block format (tsv in row)
	virtual void writePrimsInFile(char* filename);
	virtual void writePrimsIntoStream( FILE*  datei);
	virtual void writePrimsIntoStream( FILE*  datei, bool formatType);
	virtual void writeDualsInFile(char* filename);
	virtual void writeDualsIntoStream( FILE*  datei);
	virtual void writeDualsIntoStream( FILE*  datei, bool formatType);
	virtual void writeObjInFile(char* filename);
	virtual void writeObjIntoStream( FILE*  datei);
	virtual void writeObjIntoStream( FILE*  datei, bool formatType);

   //prepare internal vars
   virtual void LP_makeLinearMatrix();//renamed from <LP_matrizen_anlegen>
   void LP_allocateAuxiliaryArrays();
   void LP_deallocateAuxiliaryArrays();
	virtual void LP_makeQuadraticMatrix();

//Meaning of columns and rows for the AGENT
	virtual int getCapLab();    //row index labor capacity
	virtual int getCapLiq();    //row index liquidity capacity
	virtual int getCapInv(int soil);//row indices for investments in different soils
	virtual int getCapLnd();	 //row index first land capacity
	virtual int getCapWat();	 //row index first water capacity
	virtual int getCapCsh();	 //row index average cash demand
	virtual int getCapAvl();    //row index average labor demand
	virtual int getCapMac();	 //row index average demand for fruit equipment M4
	virtual int getActCsh();	 //remaining cash activity (LP column)
	virtual int getCapCashFlowCoef (); //row index cash flow (Country Switch Germany) for coefficients
	virtual int getCapCashFlowRHS (); //row index cash flow (Country Switch Germany) for RHS (fix costs)
	virtual int getCapCashFlowRHSminCons (); //row index cash flow (Country Switch Germany) for RHS (min consumption)

   //labor and cash and labor spans
   virtual int getCshSpans();
   virtual int getLabSpans();

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

   //fixing binary activities for labor force
   virtual int fixAdultBin(); //fix adult labor binaries (yes(no)
   virtual int getActFemaY();//acitivity index for female adult labor (yes)
   virtual int getActFemaN();//acitivity index for female adult labor (no)
   virtual int getActMaleY();//acitivity index for male adult labor (yes)
   virtual int getActMaleN();//acitivity index for male adult labor (no)

   //consumption model
	virtual int getNtrDmdR();

	//shadow prices
   virtual int getNumberShadowPrices();
	virtual int getFirstShadowPrice();
	virtual int getShadowPriceLpMode();

// Troost 20140505 Age HHead in LP
// Troost **********begin block insert**********
	virtual int getNumberAgentAttributesToLP();
	virtual int getAgentAttributeToLP(int);
	virtual int getAgentSubAttributeToLP(int);
	virtual int getColumnForAgentAttributeToLP(int);
	virtual int getRowForAgentAttributeToLP(int);
// Troost **********end block insert**********

// Troost 20170516 Advanced Demography Model 2
// Troost **********begin block insert**********
	int getNumberHouseholdMembersToLP();
	int getHouseholdMembersToLP_Career(int i);
	int getHouseholdMembersToLP_ageMin(int i);
	int getHouseholdMembersToLP_ageMax(int i);
	int getHouseholdMembersToLP_whichMember(int i);
	int getHouseholdMembersToLP_whatInfo(int i);
	int getHouseholdMembersToLP_whoElse(int i);
	int getHouseholdMembersToLP_yearsAhead(int i);
	int getHouseholdMembersToLP_col(int i);
	int getHouseholdMembersToLP_row(int i);

	int getNumberHouseholdDecisionsFromLP();
	int getHouseholdDecisionsFromLP_Career(int i);
	int getHouseholdDecisionsFromLP_ageMin(int i);
	int getHouseholdDecisionsFromLP_ageMax(int i);
	int getHouseholdDecisionsFromLP_whichMember(int i);
	int getHouseholdDecisionsFromLP_whatInfo(int i);
	int getHouseholdDecisionsFromLP_whereTo(int i);
	int getHouseholdDecisionsFromLP_col(int i);

	int getNumberHouseholdSplittingActivities();
	int getHouseholdSplittingFromLP_What(int i);
	int getHouseholdSplittingFromLP_Which(int i);
	int getHouseholdSplittingFromLP_Age(int i);
	int getHouseholdSplittingFromLP_Whereto (int i);
	int getHouseholdSplittingFromLP_Col(int i);

	int getNumberFarmsteadNRUtoLP();
	int getFarmsteadNRUtoLP_nru(int i);
	int getFarmsteadNRUtoLP_col(int i);
	int getFarmsteadNRUtoLP_row(int i);

	int getNumberLPColsLandDistribution();
	int getApplicationForLandDistribution_appNo(int i);
	int getApplicationForLandDistribution_typeOfCol(int i);
	int getApplicationForLandDistribution_which(int i);
	int getApplicationForLandDistribution_col(int i);
	int getMaxID_ApplicationForLandDistribution();

// Troost **********end block insert**********


// Troost 20141210 Land markets - type 4, assets traded with land
// Troost **********begin block insert**********
	virtual int getNumberPlotCapacitiesToLP();
	virtual int getCapacitySlotForPlotCapacityConstraint(int i);
	virtual int getRowForPlotCapacityConstraint(int i);
	virtual int getWithRentedForPlotCapacityConstraint(int i);
// Troost **********end block insert**********
// Troost 20150218 MEKA more realistic
// Troost **********begin block insert**********
	virtual int getNumberRemDurationConstraints();
	virtual int getRowForRemDurationConstraint(int i);
	virtual int getNRUForRemDurationConstraint(int i);
	virtual int getMinDurationForRemDurationConstraint(int i);
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


// Troost 20180418 External script calls
// Troost **********begin block insert**********
	int getNumberSolutionToExternalScriptPreInvest();
	int getNumberUboundsPreInvest();
	int getLpColumnToExternalScriptPreInvest(int i);
	int getLpColumnUboundsPreInvest(int i);
	int getFileColumnToExternalScriptPreInvest(int i);
	int getUboundPreInvest(int i);
	string getColumnNameFileToExternalScriptPreInvest(int i);
	size_t getNumVariableColumnsInFileToExternalScriptPreInvest();
// Troost **********end block insert**********


// Troost 20170310 Cplex Mip starts
// Troost **********begin block insert**********
#ifdef MIPSTART
	int getNumberOfRentalValuationColumns() { return numberLandMarketPlotOfferColumns;}
	int getIndexOfFirstRentalValuationColumn() { return firstLandMarketPlotOfferColumn;}
#endif
// Troost **********end block insert**********


//ADAPTING THE MILP
	//general MILP handling
   virtual double getZRowEntry(int);//refers to original matrix ap
	virtual double getVarRHS(int r);
   virtual void setVarRHS(int r, double w);

	virtual double getOgMtxCoeff(int r, int c);//refers to original matrix ap
	virtual void setOgMtxCoeff(int r, int c, double w);//refers to original matrix ap
   virtual double getOgObjCoeff(int c);//refers original matrix ap
   virtual void setOgObjCoeff(int c, double w);//refers original matrix ap
   virtual void copyOgObjRowIntoArray(double* arrayPtr);

	virtual double getOgRhsValue(int r);//refers to original matrix ap
	virtual void setOgRhsValue(int r, double w);//refers to original matrix ap
   virtual void copyOgRhsIntoArray(double* arrayPtr);

	virtual double getMtxCoeff(int r, int c);//refers agent matrix bp
   virtual void setMtxCoeff(int r, int c, double w);//refers agent matrix bp
   virtual double getObjCoeff(int c);//refers agent matrix bp
   virtual void setObjCoeff(int c, double w);//refers agent matrix bp

   virtual void copyObjRowIntoArray(double* arrayPtr);
   virtual void copyRhsIntoArray(double* arrayPtr);

	virtual void setColUpp(int c, double w);
	virtual void setOrigColUpp(int c, double w);

	virtual void setColLow(int c, double w);
	virtual void setOrigColLow(int c, double w);

	virtual void setValueInOrigMatrix(int r, int c, double w);
	virtual void setValueInMatrix(int r, int c, double w);

   virtual void updateOrigMatrix();
	virtual void updateOrigMatrixWithActualPrices();
	virtual void sendCostDataToMarketHandler();
#ifdef MULTIPERIOD
	void updateGenericMatrixForDecisionStage(int decisionStage);

	void multiperiod_updatePricesInOrigMatrix();
	void updateMipDecisionStage(int decisionStage);
	void updateAgentExpectationsPrices(double* priceExpectations, int arraySize);
	void updateAgentExpectationsPrices(double* priceExpectations, int arraySize, double* varsRHS, double* zrow);
	void updateAgentFromPreviousSolution(double* oldSolution, int arraySize);
	void updateAgentFromPreviousSolution(double* oldSolution, int arraySize , double* varsRHS, double* zrow);
	void updateAgentCashReserves(double liqMeans);
	void updateAgentCashReserves(double liqMeans, double* varsRHS, double* zrow);
	void updateAgentInterestRates(double rateLC, double rateSC, double rateSD);
	void updateAgentInterestRates(double rateLC, double rateSC, double rateSD, double* varsRHS, double* zrow);
	/*void updateAgentDebtService(vector<double>* debtServiceByPeriods);
	void updateAgentDebtService(vector<double>* debtServiceByPeriods, double* varsRHS, double* zrow);*/
	void updateAgentDebtServiceAndDepreciation(vector<double>* depreciationByPeriods, vector<double>* principalByPeriods, vector<double>* interestByPeriods, double* varsRHS, double* zrow);
	void updateAgentDebtServiceAndDepreciation(vector<double>* depreciationByPeriods, vector<double>* principalByPeriods, vector<double>* interestByPeriods);
	void updateAgentHouseholdMinimumConsuption(vector<double>* minConsByPeriods);
	void updateAgentHouseholdMinimumConsuption(vector<double>* minConsByPeriods, double* varsRHS, double* zrow);
   void updateAgentUserDefinedHouseholdAttributes(vector<vector<double> >*   arrayOfValuesByAttributeIndex  , double* varsRHS, double* zrow);
   void updateAgentUserDefinedHouseholdAttributes(vector<vector<double> >*   arrayOfValuesByAttributeIndex );
#ifdef LIVSIM_COUPLING
   void updateAgentLivSimMilkProduction(double milkProduction);
   void updateAgentLivSimMilkProduction(double milkProduction,double* varsRHS, double* zrow);
#endif

	void updateAgentAvailableLabor(vector<vector<double> >* inputTimeCategoryVector);
	void updateAgentAvailableLabor(vector<vector<double> >* inputTimeCategoryVector, double* varsRHS, double* zrow);
	void updateAgentAvailablePlots(vector<vector<double> >* inputTimeCategoryVector);
	void updateAgentAvailablePlots(vector<vector<double> >* inputTimeCategoryVector, double* varsRHS, double* zrow);
	void updateAgentAssetCapacities(map<int, vector<double> >* inputMap);
	void updateAgentAssetCapacities(map<int, vector<double> >* inputMap, double* varsRHS, double* zrow);
	void updateAgentAssetVintageCapacities(map<int, vector<double> >* inputMap);
	void updateAgentAssetVintageCapacities(map<int, vector<double> >* inputMap, double* varsRHS, double* zrow);

	void updateAgentAttributes(vector<double>* attributeValues);
	void updateAgentAttributes(vector<double>* attributeValues, double* varsRHS, double* zrow);


	void updateAgentYields(caYld** inputArray, int inputArraySize0, int inputArraySize1, double* varsRHS, double* zrow);
	void updateAgentYields(caYld** inputArray, int inputArraySize0, int inputArraySize1);
	void updateAgentYieldExpectations(caYld** inputArray, int inputArraySize0, int inputArraySize1, double* varsRHS, double* zrow);
	void updateAgentYieldExpectations(caYld** inputArray, int inputArraySize0, int inputArraySize1);

	void updateAgentCropActAttributeExpectations(caYld** inputArray, int inputArraySize0, int inputArraySize1 );
	void updateAgentCropActAttribute(caYld** inputArray, int inputArraySize0, int inputArraySize1 );
	void updateAgentCropActAttributeRangeExpectations(caYld** inputArray, int inputArraySize0, int inputArraySize1 );
	void updateAgentCropActAttributeRange( caYld** inputArray, int inputArraySize0, int inputArraySize1 );



	void updateAgentSeasonals( int curCyclicPeriod, int cycleLength);

   void updatePlotAttributesAggregatedByNRU(agentF* afp, double* varsRHS, double* zrow);
   void updatePlotAttributesAggregatedByNRU(agentF* afp);



	void applyDiscountRateToObjCoeff(double dRate);
//gutsOf updating functions
	void updateAgentTemplate2Dims(vector<MultiperiodMipVariableValue*>* listToChange, vector<vector<double> >* inputTimeCategoryVector, string where);
	void updateAgentTemplate2DimsFromArray(vector<MultiperiodMipVariableValue*>* listToChange, double** inputArray, int inputArraySize0, int inputArraySize1);
	void updateAgentTemplate1Dim(vector<MultiperiodMipVariableValue*>* listToChange, vector<double>* inputTimeCategoryVector);
	void updateAgentTemplate1DimFromArray(vector<MultiperiodMipVariableValue*>* listToChange, double* inputArray, int inputArraySize);
	void updateAgentTemplateSingleVal(vector<MultiperiodMipVariableValue*>* listToChange, double val_);
	void updateAgentTemplate2DimsMapped(vector<vector<vector<MultiperiodMipVariableValue*> > >* toChange, map<int, vector<double> >* inputMap);


	void updateAgentTemplate2Dims(vector<MultiperiodMipVariableValue*>* listToChange, vector<vector<double> >* inputTimeCategoryVector, double* varsRHS, double* zrow, string where);
	void updateAgentTemplate2DimsFromArray(vector<MultiperiodMipVariableValue*>* listToChange, double** inputArray, int inputArraySize0, int inputArraySize1, double* varsRHS, double* zrow);
	void updateAgentTemplate1Dim(vector<MultiperiodMipVariableValue*>* listToChange, vector<double>* inputTimeCategoryVector, double* varsRHS, double* zrow);
	void updateAgentTemplate1DimFromArray(vector<MultiperiodMipVariableValue*>* listToChange, double* inputArray, int inputArraySize, double* varsRHS, double* zrow);
	void updateAgentTemplateSingleVal(vector<MultiperiodMipVariableValue*>* listToChange, double val_, double* varsRHS, double* zrow);
	void updateAgentTemplate2DimsMapped(vector<vector<vector<MultiperiodMipVariableValue*> > >* toChange, map<int, vector<double> >* inputMap, double* varsRHS, double* zrow);





	//memory management
	void deallocateMultiperiodMipVariableValues(vector<MultiperiodMipVariableValue*>* vec);

//lookup structure preparation
	void updateCropManagementIdToCgmArrayIndex(map<int,int>* transformationMap);
	unsigned updateCropManagementIdSoilTypeToCgmArrayIndex(map<pair<int,int>,int>* transformationMap);


//values to be read from the solution
	bool isInLp_incomeBeforeTaxes()	{	return readerIncome.isDefined();	}
	bool isInLp_incomeTaxes()			{	return readerIncomeTaxes.isDefined();	}
	bool isInLp_InKindIncome()			{  return readerIncomeInKind.isDefined();}
	double getIncomeBeforeTaxes(double* sol, double* zrow){ return readerIncome.calculateValue(sol, zrow);				}
	double getIncomeTaxes(double* sol, double* zrow)		{ return readerIncomeTaxes.calculateValue(sol, zrow);				}

	double getInKindIncome(double* sol, double* zrow)		{ return readerIncomeInKind.calculateValue(sol, zrow);				}
	double getOffFarmIncome(double* sol, double* zrow)		{ return readerIncomeOffFarm.calculateValue(sol, zrow);				}
	double getOffFarmLabor(double* sol, double* zrow) 	 	{ return readerLaborOffFarm.calculateValue(sol, zrow);		}
	double getOffFarmCapital(double* sol, double* zrow) 	{ return readerCapitalOffFarm.calculateValue(sol, zrow);		}
	double getOnFarmLabor(double* sol, double* zrow) 	 	{ return readerLaborOnFarm.calculateValue(sol, zrow);			}
	double getBorrowedCapital(double* sol, double* zrow)	{ return readerCapitalBorrowed.calculateValue(sol, zrow);	}
	double getCashCarryOver (double* sol, double* zrow)	{ return readerCashCarryOver.calculateValue(sol, zrow);	}
	double getCashConsumption(double* sol, double* zrow)	{ return readerCashConsumption.calculateValue(sol, zrow);	}
	double getShortTermInterestPaid(double* sol, double* zrow)	{ return readerShortTermInterestPaid.calculateValue(sol, zrow);	}

	void addCropProductionAreasToResultArray (double* sol, double* zrow, double* resultArray, int sizeResultArray, vector<pair<int,double> >* agentCropAreas);
	MapOfSubindexedVectorIndexedDoubles  getNRUCropProductionAreasMap(double* sol, double* zrow)  { return readerCropNRU.calculateSubindexedResultMap(sol, zrow, 1 );}

	MapOfVectorIndexedDoubles  getInvestments(double* sol, double* zrow)  { return readerAssetInvestments.calculateResultMap(sol, zrow);}
	MapOfVectorIndexedDoubles  getDisinvestments(double* sol, double* zrow)  { return readerAssetDisinvestments.calculateResultMap(sol, zrow);}
	MapOfVectorIndexedDoubles  getAssetsKept(double* sol, double* zrow)  { return readerAssetKeep.calculateResultMap(sol, zrow);}
	MapOfSubindexedVectorIndexedDoubles getAssetsTransformed(double* sol, double* zrow)  { return readerAssetTransform.calculateSubindexedResultMap(sol, zrow, 2);}

	MapOfVectorIndexedDoubles  getAgentAttributeChanges(double* sol, double* zrow)  { return readerAgentCharacteristicSet.calculateResultMap(sol, zrow);}
	MapOfVectorIndexedDoubles  getAgentAttributeAdditions(double* sol, double* zrow)  { return readerAgentCharacteristicAdd.calculateResultMap(sol, zrow);}



#ifdef LIVSIM_COUPLING
	MapOfVectorIndexedDoubles  getHerdGroupPlacementsOnNRUs(double* sol, double* zrow)  { return readerLivSimHerdGroupsToNRU.calculateResultMap(sol, zrow);}
	MapOfSubindexedVectorIndexedDoubles  getLivsimFeedingStrategies(double* sol, double* zrow)  { return readerLivSimHerdGroupFeeding.calculateSubindexedResultMap(sol, zrow,3);}
	MapOfSubindexedVectorIndexedDoubles  getFeedGroupMembers(double* sol, double* zrow)  { return readerLivestockAssetsToLivSimHerdGroups.calculateSubindexedResultMap(sol, zrow,2);}

#endif

	double getAreaForInnovation(double* sol, double* zrow, int objID, int & nothingFoundFlag) { return readerInnovationArea.getResultForArgs(sol, zrow, vector<int>(1,objID), nothingFoundFlag )  ;}

	void multiperiod_writeResultAggregatesToStream(  int fstID, int agentExits, double* sol, double* zrow, int aggregateOnly = false);
	string multiperiod_openDecisionResultStreams(string decisionStage, bool aggregateOnly = false, bool noAggYears = false);
	void multiperiod_closeDecisionResultStreams(bool aggregateOnly);

#endif
	//modifies Milp tableau before loading OSL model
	virtual void changeObjFunc(double*& objF, int oSta, int oEnd);
   virtual void fixLaborBinary(double femLab, double malLab);
   virtual void fixProducVarsInConsumptionMode(l_vektor& bprod);
	virtual void fixProducVarsInMarketingMode(l_vektor& bprod);
   virtual void changeElemMtx(int num, int*& rows, int*& cols, double*& elms);
	virtual void setToEEConstr(int rLP, double value);

/*	obsolete?
   virtual double getMtxCoeffNR(int r, int c);//"old" NR-style function
   virtual void setMtxCoeffNR(int r, int c, double w);//"old" NR-style function
   //virtual int callSimplxNR(double, int);//"old" NR-style function
*/

//COPYING BETWEEN GENERAL PROBLEM AND AGENT-SPECIFIC PROBLEM AND SOLVER VARS
   virtual void copyProblemGenericToSpecific();//renamed from LP_kopieren9
   virtual void roundMatrixCoeff1(void);
   virtual void roundMatrixCoeff2(void);
   virtual void LP_deallokieren9(void);

   //copy the general RHS (read from file) to the agent-specific RHS (for MilpCheck)
   virtual void copyVarsRHS(double*& varsRHS);

   //copy the agent-specific problem to the solver arrays
 	virtual void copyModel(SolverType whichSolver, int aID, int sID, int fstID, string agentMipCode, double* mipstart1 = NULL, double* mipstart2 = NULL);
 	virtual void copyModelDef( int aID, int sID, int fstID, string agentMipCode, double* mipstart1 = NULL, double* mipstart2 = NULL);

//LOADING AND SOLVING MODELS
   virtual void loadModel(SolverType whichSolver, int aID, int sID, int fstID,  int max = true, int qp = false, int reload = false); //Default: Maximization
   virtual void loadModelDef(int aID, int sID, int fstID,  int max = true, int qp = false, int reload = false); //Default: Maximization

// Troost 20170310 Cplex Mip starts
// Troost **********begin block insert**********
#ifdef MIPSTART
virtual double solveModel(SolverType whichSolver, int agID, int scID, int fstID, int savePrim, int saveDual,
		int& forSol, l_vektor& bprod, double*& varsLHS, double*& dualRHS,
		double*& varsRHS, MatrixDouble& mEntries, double* solToSaveForMipStart, int doForcedSolution = true);
virtual double solveModelDef( int agID, int scID, int fstID, int savePrim, int saveDual,
		int& forSol, l_vektor& bprod, double*& varsLHS, double*& dualRHS,
		double*& varsRHS, MatrixDouble& mEntries, double* solToSaveForMipStart, int doForcedSolution = true);
int multipleSolveForPlotValuationDef(int agentID, int catchmentID, int fstID, vector<double>&valuations, int numPlots, double* solutionToBeSavedForWarmStart = NULL) ;
int multipleSolveForPlotValuation(SolverType whichSolver, int agentID, int catchmentID, int fstID, vector<double>&valuations, int numPlots, double* solutionToBeSavedForWarmStart = NULL) ;


#else
virtual double solveModel(SolverType whichSolver, int agID, int scID, int fstID, int savePrim, int saveDual,
		int& forSol, l_vektor& bprod, double*& varsLHS, double*& dualRHS, double*& varsRHS, MatrixDouble& mEntries,
		int doForcedSolution = true);
virtual double solveModelDef(int agID, int scID, int fstID, int savePrim, int saveDual, int& forSol, l_vektor& bprod,
		double*& varsLHS, double*& dualRHS, double*& varsRHS, MatrixDouble& mEntries, int doForcedSolution = true);
#endif //MIPSTART
// Troost **********end block insert**********
   virtual void freeModel();
   virtual double getObjValue();
   virtual double forcedSolution(int, int, int,
   		l_vektor&, double*&, double*&, double*&, MatrixDouble& mEntries);
   // exportSolverLoadedToFile()

//DEBUGGING, GENERAL
	//some functions for debugging
	virtual void cleanArraysForMilp(double dval, int ival);
#if 0 // old LP9 OSL debugging functions, unused in AgentMipProblem, will not work with LPPACKED
	virtual void cleanMatrixForMilp(double dval, int ival);
	virtual void checkArraysForMILP(double dval, int ival);
	virtual int  checkVectorForMilp(double*& array, int size, int start, double dval);
	virtual int  checkVectorForMilp(int*& array, int size, int start, int ival);
	virtual void writeArraysInFiles(char* prefix);
#endif
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
#ifdef USE_SYM
protected:
//SPECIFIC FUNCTIONS FOR THE SYMPHONY SOLVER
	void initSymphony();
	void deallocateSymphony();
   void loadModelSymphony(int aID, int sID, int fstID,  int max = true); //Default: Maximization
   int solveModelSymphony(int agID, int scID, int fstID);
   double optSolutionSymphony(int agID, int scID, int fstID, int savePrim, int saveDual,
      double prp, l_vektor& bprod, double*& varsLHS, double*& schattenpreise,
      double*& varsRHS);
   void writePrimsIntoStreamSymphony(FILE* datei, bool formatType);
   void writeDualsIntoStreamSymphony(FILE* datei, bool formatType);
   void writeObjIntoStreamSymphony(FILE* datei, bool formatType);

   void freeModelSymphony();
   void applySymphonyUserSettings(int stage = 0);
   void printSymphonyUserSettings(int curstage = 0);
   int whatIfSymphonyFailed(int stage = 0, int limit_reached = 0);
public:
   static void setSymphonyOptionsFile(string filename);
   static void readSymphonyOptionsFile();
#endif
#ifdef USE_CBC
protected:
//SPECIFIC FUNCTIONS FOR THE CBC SOLVER
	void initCbc();
	void deallocateCbc();

   void loadModelCbc(int aID, int sID, int fstID,  int max = true); //Default: Maximization
   int solveModelCbc(int agID, int scID, int fstID);
   double optSolutionCbc(int agID, int scID, int fstID, int savePrim, int saveDual,
      double prp, l_vektor& bprod, double*& varsLHS, double*& schattenpreise,
      double*& varsRHS);



   void writePrimsIntoStreamCbc(FILE* datei, bool formatType);
   void writeDualsIntoStreamCbc(FILE* datei, bool formatType);
   void writeObjIntoStreamCbc(FILE* datei, bool formatType);

   void freeModelCbc();
   void printCbcSolverSettings() const;
   static void addCbcSolvingAttempt (string curCommandline);

public:
   static void setCbcOptionsFile(string filename);
   static void readCbcOptionsFile();
#endif
#if defined(USE_CPLEX) || defined(USE_SYM)
protected:
   //can be used for all solvers included via OsiSolver Interface
   //not extra functions needed
   void loadModelOsi(SolverType whichSolver, int aID, int sID, int fstID,  int max=true);
#ifdef MIPSTART
   double optSolutionOsi(SolverType whichSolver, int agID, int scID, int fstID, int savePrim, int saveDual,
      double prp, l_vektor& bprod, double*& varsLHS, double*& schattenpreise,
      double*& varsRHS, double* solToSaveForMipstart);
#else
   double optSolutionOsi(SolverType whichSolver, int agID, int scID, int fstID, int savePrim, int saveDual,
      double prp, l_vektor& bprod, double*& varsLHS, double*& schattenpreise,
      double*& varsRHS);
#endif //MIPSTART

   void writePrimsIntoStreamOsi(SolverType whichSolver, FILE* datei, bool formatType);
   void writeDualsIntoStreamOsi(SolverType whichSolver, FILE* datei, bool formatType);
   void writeObjIntoStreamOsi(SolverType whichSolver, FILE* datei, bool formatType);
#endif

#ifdef USE_CPLEX
protected:
//SPECIFIC FUNCTIONS FOR THE CPLEX SOLVER
	void initCplex();
	void deallocateCplex();
   int solveModelCplex(int agID, int scID, int fstID);
   int multipleSolveForPlotValuationCplex(int agentID,  int catchmentID,  int fstID, vector<double>&valuations, int numPlots, double* solutionToBeSavedForWarmStart = NULL);
   void freeModelCplex();
   void applyCplexUserSettings(int stage = 0);
   void printCplexUserSettings(int curstage = 0);
   int whatIfCplexFailed(int stage = 0, int limit_reached = 0);
public:
   static void setCplexOptionsFile(string filename);
   static void readCplexOptionsFile();
#endif //USE_CPLEX
public:
   static void readSolverOptionsFile();
#ifdef USE_OSL
protected:
//SPECIFIC FUNCTIONS FOR THE OSL SOLVER
	virtual void initContextOSL();
	virtual void copyModelOSL(int aID, int sID, int fstID);
	virtual void loadModelOSL(int aID, int sID, int fstID,  int max = true, int qp = false, int reload = false); //Default: Maximization
   virtual void testModelOSL(int aID, int sID, int fstID);
   virtual void freeModelOSL();
   virtual void rmEmptyOSL(void);
   virtual int checkPrimalsOSL(void);
	virtual int checkColumnsOSL(void);
   virtual void cleanMemoryOSL(void);

   virtual int solveModelOSL(int agID, int scID, int fstID);
   /*, int savePrim, int saveDual,
         int& forSol, l_vektor& bprod, double*& varsLHS, double*& dualRHS,
         double*& varsRHS, MatrixDouble& mEntries); // new wrapper function, not in lp9.h
*/
   virtual int solveLpOSL(int agID, int scID, int fstID);
   /*, int savePrim, int saveDual,
         l_vektor& bprod, double*& varsLHS, double*& dualRHS, double*& varsRHS,
         MatrixDouble& mEntries);*/

	//attempts to solve Milp, calls solveMilpOSLx if problems show up
   virtual int solveMilpOSL(int agID, int scID, int fstID);
   /*, int savePrim, int saveDual,
         int& forSol, l_vektor& bprod, double*& varsLHS, double*& dualRHS,
         double*& varsRHS, MatrixDouble& mEntries);*/

	//with previous call of ekk_eitherSimplex, see sample code EXIMDL.CPP
   virtual int solveMilpOSL1(int agID, int scID, int fstID);
   /*, int savePrim, int saveDual,
         l_vektor& bprod, double*& varsLHS, double*& dualRHS, double*& varsRHS);*/

	//with presolving, see sample code EXMIP.CPP
   virtual int solveMilpOSL2(int agID, int scID, int fstID);
   /*, int savePrim, int saveDual,
         l_vektor& bprod, double*& varsLHS, double*& dualRHS, double*& varsRHS);*/

	//with transformation to 0/1 MIP problem, see sample code EXGMIP.CPP
   virtual int solveMilpOSL3(int agID, int scID, int fstID);
   /*, int savePrim, int saveDual,
         l_vektor& bprod, double*& varsLHS, double*& dualRHS, double*& varsRHS);*/

// Troost 20120312 - advanced MilpCheck tuning
// Troost **********begin block insert **********
   virtual int solveMilpOSL4(int agID, int scID, int fstID);
   /*, int savePrim, int saveDual,
            l_vektor& bprod, double*& varsLHS, double*& dualRHS, double*& varsRHS);*/
// Troost **********end block insert **********

	//quadratic solvers without integers
	virtual int solveQuadraticSimplexOSL(int agID, int scID, int fstID);
	/*, int savePrim, int saveDual,
         l_vektor& bprod, double*& varsLHS, double*& dualRHS, double*& varsRHS,
         MatrixDouble& mEntries);*/
	
	virtual int solveQuadraticBarrierOSL(int agID, int scID, int fstID);
	/*, int savePrim, int saveDual,// see exqbar.cpp for more elaborate version
         l_vektor& bprod, double*& varsLHS, double*& dualRHS, double*& varsRHS,
         MatrixDouble& mEntries);*/

	//quadratic solver with integers
	virtual int solveQuadraticIntegerOSL(int agID, int scID, int fstID);
	/*, int savePrim, int saveDual,
         l_vektor& bprod, double*& varsLHS, double*& dualRHS, double*& varsRHS,
         MatrixDouble& mEntries);*/

	virtual int checkIfIntegerProblemOSL();//checks the loaded OSL problem of an agent, if not available input file
	virtual int checkIfQuadraticProblemOSL();//checks the loaded OSL problem  of an agent, if not available input file

	//solver solution
   virtual double getObjValueOSL();
   virtual double getPrintCPU();
   virtual void setPrintCPU(double time);

   virtual double optSolutionOSL(int agID, int scID, int fstID, int savePrim, int saveDual,
      double prp, l_vektor& bprod, double*& varsLHS, double*& schattenpreise,
      double*& varsRHS);

	//modifies loaded OSL model using ekk_functions
   virtual int modifySolVectEKK(int num, int*& cols, double*& values);// not in use
   virtual int changeObjFuncEKK(double*& objF, int oSta, int oEnd);// not in use
   virtual int fixProducVarsEKK(l_vektor& bprod);// not in use
   virtual int changeElemMtxEKK(int num, int*& rows, int*& cols, double*& elms);// not in use
   virtual int setToEEConstrEKK(int rLP, double value);// not in use
	virtual int applyEpsilonToEEConstrEKK(int rLP, double epsilon);

   //export OSL model to file in Excel style
   virtual void exportOSLToFile(char* filename, int withText);

   //writing MPS from solver
   virtual void saveMilpMPSOSL(char* filename);
   virtual void LP_readMilpMPSOSL(const char* filename, int quadratic = false);

	virtual void newModelOSL(int aID, int sID);//refactored from <maxModelOSL>
	virtual void setModelMaximize(int aID, int sID);//refactored from <maxModelOSL>

	virtual void writeDualsIntoStreamOSL( FILE*  datei, bool formatType);
	virtual void writePrimsIntoStreamOSL( FILE*  datei, bool formatType);
	virtual void writeObjIntoStreamOSL( FILE*  datei, bool formatType);

	//for models with less than 256 columns (including RHS and LHS)
	virtual void writePrimsIn256ColsOSL(char* filename);
	virtual void writeDualsIn256ColsOSL(char* filename);
#endif //USE_OSL



};

#ifdef NEWFORMAT14
//	void expectFileDelimiter(FILE* strm, string delim, string filename, string extraInfo = ""); //no need to export
#endif
// Troost 20130118 Catch segmentation faults
// Troost **********begin block insert**********
#ifdef AIX
	void segfault_handler(int signum, siginfo_t *info, void *);
	void segfault_handler2(int signum, siginfo_t *info, void *);
#endif

// Troost **********end block insert**********
#endif  /* #ifndef _AgentMipProblem_h */
#endif /*LP9*/

