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
//  File:       MarketTradablesHandler.h (formerly markt9.h)
//
//  Contents:   Market class for tradables, containing time series for input
//              and output prices plus imputed prices (for in-kind consumption)
//
//
//------------------------------------------------------------------------------


#ifndef _MarketTradablesHandler_h
#define _MarketTradablesHandler_h

#include "MatrixDouble.h"
#include "BasicDefines.h"
#ifdef LPPACKED
	#include "AgentMipProblem.h"
	class mpPrbl;
#endif

#ifdef ODBSTORE
	#include <odb/core.hxx>
#endif

#pragma db object
class markt
{	
#pragma db id
	int mId;
#ifdef ODBSTORE
			//void	odbCallback (odb::callback_event, odb::database&);
			//void	odbCallback (odb::callback_event, odb::database&) const;
	friend class odb::access;
	public:
#endif
	public:
	///@name Public member functions
	//@{
	///general get functions
	virtual int getNumberOfSimulationPeriods();
	virtual int getNumberOfCommodities();
	virtual int getNumberOfPayments();//that is, number of Lp columns considered for income statment
	virtual int getNumberOfFuturePrices();
	virtual int getNumberOfProductionCosts();
	virtual double getPriceForTradable(int period, int prd);
	virtual double getInitialPriceForTradable(int prd);// ## Tbe 111223 was previously used for stable income
	virtual int getNumberMarketTradablePrices();
	virtual int checkIfEndogenousPrices();
	
	///time preference penalties for market sales
	virtual int getNumberOfTimePreferencePenalties();
	virtual int getPenaltyLpRow(int idx);
	virtual int getPenaltyLpCol(int idx);
	virtual int getPenaltyActivity(int idx);
	virtual double getPenaltyTimeDelay(int idx);

	///reading of and writing to file
	virtual void readInputFromFile(const char* filename);
	virtual void writeInputIntoFile(char* filename);

	///functions for basic consumption module
	virtual double getExtCons();
	virtual double getMinCons();
	virtual double getForCons();
	virtual double calcConsumption(double income, int hhSize);

	///functions for 3-stage consumption module
	virtual int getTypeOfPayment(int lpCol);
	virtual int getIncR(void);
	virtual void calcSvgsPart(double hhEgy, MatrixDouble& scolV);
	virtual double compZeroSvgs(double hhEgy, double logitRandom);
	virtual int getsColU(void);
	virtual int getsRowB(void);
	virtual int getsColB(void);
	virtual int getsRowS(void);
	virtual int getsColS(void);
	virtual int getsNumS(void);

	virtual double calcFNoFPart(double hhEgy, MatrixDouble& fcolV);
	virtual int getfNumS(void);
	virtual int getfRowS(void);
	virtual int getfColS(void);
	virtual int getfRowH(void);
	virtual int getfColH(void);

	virtual void calcItemPart(int period, double hhEgy,
		MatrixDouble& irowH, MatrixDouble& irowP, MatrixDouble& imatS);
	virtual int getiNumF(void);
	virtual int getiNumS(void);
	virtual int getiRowS(void);
	virtual int getiColS(void);
	virtual int getiRowP(void);
	virtual int getiColP(void);
	virtual int getiRowH(void);
	virtual int getiColH(void);
	virtual int getNumNtr(void);

	///special function for logit savings model
	virtual int checkLogitSavingsModel();
	virtual int getlRowB();
	virtual int getlColB();
	virtual double getMinExpenditurePerMJ();

	///functions used to update LP data structure
	virtual void insActPrInObjFunc(double*& objF, int period);
#ifdef LPPACKED
	virtual void insFutPrInObjFunc(mpPrbl* origM, int period);
	virtual void insPriceIndicesNR(mpPrbl* origM, int period);
#else
	virtual void insFutPrInObjFunc(double**& ap, int period);
	virtual void insPriceIndicesNR(double**& ap, int period);
#endif
	virtual int getFoodCategoryConstraintForBuyFoodPriceLpCol(int col );
	virtual int getLpColOfFoodBuyingAndConsumptionActivity(int i);
	virtual int getCategoryRowOfFoodBuyingAndConsumptionActivity(int i);
	virtual int getNumberFoodBuyingAndConsumptionActivities();
	virtual void insertNumberAndFirstLpColumnOfBuyFoodPrices(int& number, int& startLp);// ## Tbe 121210
	virtual void insertActualBuyFoodPrices(MatrixDouble& buyFoodCoefficients, int period);// ## Tbe 121210
#ifdef LPPACKED
	virtual void makeBackUpOfCostsFromLpMatrix(int averageCashRow, mpPrbl* origM);
#else
	virtual void makeBackUpOfCostsFromLpMatrix(int averageCashRow, double**& ap);
#endif
	virtual int getInternalIndexForLpActivity(int lpActivity);

	virtual void retrieveAbsoluteCosts(MatrixDouble& lpIndicesAndAbsoluteCosts, int period);
	virtual double getAbsoluteProductionCost(int lpActivity, int period);
	virtual double getAbsolutePreHarvestCost(int lpActivity, int period);
	virtual double getAbsoluteShortTermCreditLimit(int lpActivity, int period);

	virtual void retrieveRelativeProductionCosts(MatrixDouble& lpIndicesAndRelativeCosts, int period);
	virtual double getRelativeProductionCost(int lpActivity, int period);

	///compute gross margins for cropping activity
	virtual double computeGrossMarginForCropActivity(int priceLpColumn, double amountSold, int productionLpColumn, double areaGrown, int period);

	///compute equilibrium prices for tradable
	virtual void prepareFarmProducerSurvey(MatrixDouble& questionnaire, int& firstLpColumn);
	virtual void computeMarketEquilibriumPrices(int numberOfRespondents, MatrixDouble& questionnaire);

	///inserts member prices of producer organization
	virtual void insertPriceOfProducerOrganization(int colLpAgentF, double price);
	//@}

	///@name Constructor and Destructor
	//@{
	markt(int);
	virtual ~markt()
	{	pricePaths.deallokieren();
		
		reservationPrices.deallocate();
		priceFlexibilities.deallocate();
		//delete [] priceGroup;
		priceWeights.deallocate();
		referencePriceID.deallocate();
		minimumPrices.deallocate();
		priceCalibrationFactors.deallocate();
		quantitiesProducedOrUsed.deallocate();

		//delete [] activityIndexInLp;
		//delete [] typeOfProductionCost;
		//delete [] productionCostInBaseYear;
//		delete [] preHarvestCostInBaseYear;
//		delete [] shortTermCreditInBaseYear;
		relativeChangeInCosts.deallocate();
		
//		delete [] typePayment;
//		delete [] coLP;
		futP.deallokieren();

		sVecS.deallokieren();
		fBeta.deallokieren();
		iAlfa.deallokieren();
		iBeta.deallokieren();
		iGamm.deallokieren();
		iDelt.deallokieren();
		pMatP.deallokieren();

		penaltyLpRows.deallocate();
		penaltyLpCols.deallocate();
		penaltyActivities.deallocate();
		penaltyTimeDelays.deallocate();

//		delete [] pRows;
//		delete [] pCols;
	}

	///@name Protected member data
	//@{
protected:
#pragma db transient
	FILE* strm;

	///number of simulation rounds and goods/commodities
	int numRounds;
	int numCommod;
	int numberPayments;

	///data structure for exogenous market prices over simulation horizon
	MatrixDouble pricePaths;//renamed from <preispfade>

	///data structure for endogenous market prices
	int endogenousPrices;
	int LpColumnFirstTradablePrice;
	int LpColumnFirstFoodCategoryPrice;
	int LpColumnFirstOwnConsumptionPrice;

	int numberMarketTradablePrices;
	MatrixDouble reservationPrices;
	MatrixDouble priceFlexibilities;
	vector<int> priceGroup;
	int numberFoodCategories;
	int numberFoodLuxuries;
	MatrixDouble priceWeights;
	int numberOwnConsumptionPrices;
	MatrixDouble referencePriceID;
	MatrixDouble minimumPrices;
	MatrixDouble priceCalibrationFactors;
	MatrixDouble quantitiesProducedOrUsed;

	///production costs
	int numberProductionCosts;
	vector<int> activityIndexInLp;
	vector<int> typeOfProductionCost;
	vector<double> productionCostInBaseYear;//= pre-harvest plus harvest costs
	vector<double> preHarvestCostInBaseYear;
	vector<double> shortTermCreditInBaseYear;
	MatrixDouble relativeChangeInCosts;//for all periods, in relation to base year
	
	///variables of basic consumption module -------------------------------------
	double extCons;//renamed from KONSUMFAKTOR
	double minCons;//renamed from MINENTNAHMEN
	double forCons;//renamed from VERZICHT

	///variables of 3-stage consumption module --------------------------------------

	vector<int> typePayment;//flags indicating type of payment
	int nApp;//number of future prices for permanent crops ("appreciation")
	vector<int> coLP;//column indices in MILP
	MatrixDouble futP;//expected future prices
	int incR;//income transfer row
	
	int numberPurchasedFoodItems;// ## Tbe 121210
	int firstLpColPurchasedFoodItem;

	///savings part
	int sColU;//col in LP of upper bound on segment0 (=zero-savings)
	int sRowB;//row in LP of binary constraint for segment0 (=zero-savings)
	int sColB;//col in LP of binary constraint for segment0 (=zero-savings)
	int sRowS;//row in LP of savings segments
	int sColS;//col in LP of segment1

	int sNumS;//number of other segments in savings model (excl. segment0)
	MatrixDouble sVecS;//vector of segmen t sizes (size: 1 * sNumS)

	double sAlfa;//income coefficient
	double sBeta;//squared income coefficient
	double sGamm;//household size coefficient
	double sDelt;//constant term

	///food/non-food part
	int fNumS;//number of expenditure segments
	int fRowS;//row in LP of expenditure segments
	int fColS;//col in LP of expenditure segment1
	int fRowH;//row in LP of household size coefficient
	int fColH;//col in LP of household size coefficient

	double fAlfa;//expenditure coefficient
	MatrixDouble fBeta;//vector of income coefficients (size: 1 * fNumS)
	double fGamm;//household size coefficient

	///food items part
	int iNumF;//number of food items
	int iNumS;//number of expenditure segments
	int iRowH;//row in LP of household size coefficient1
	int iColH;//col in LP of household size coefficients
	int iRowP;//row in LP of price coefficient1
	int iColP;//col in LP of price coefficients
	int iRowS;//row in LP of expenditure segment1
	int iColS;//col in LP of expenditure segment1

	MatrixDouble iAlfa;//vector of expenditure coefficients (size: 1* iNumF)
	MatrixDouble iBeta;//MatrixDouble of period-specific expenditures (numRounds * iNumS)
	MatrixDouble iGamm;//MatrixDouble of price coefficients (size: numRounds * iNumF)
	MatrixDouble iDelt;//vector of household size coefficients (size: 1 * iNumF)

	///price indices for consumption model
	int  pNumG;//number of goods

	vector<int> pRows;//row indices of prices of consumption goods in LP
	vector<int>  pCols;//column indices of prices of consumption goods in LP
	MatrixDouble pMatP;//matrix of price coefficients (size: numRounds * pNumG)

	///human nutrient requirements
	int numNtr;

	///Logit model for zero savings (yes/no)
	int logit;
	double minExpenditurePerMJ;//minimum expenditure per megajoule (.00 Ush)
	int lRowB;//row in LP of minimum income constraint for logit model	
	int lColB;//col in LP of binary constraint for logit model

	double pAlfa;//additional regression coefficient for income (Ush)
 	double pGamma;//additional regression coefficient for household size (BJoule)
 	double pDelta;//additional regression coefficient constant (Ush)

	///time preference penalties
	int penaltyNumber;//number of penalty activities
	MatrixDouble penaltyLpRows;//vector of Lp row indices
	MatrixDouble penaltyLpCols;//vector of Lp column indices
	MatrixDouble penaltyActivities;//vector of Lp activity indices
	MatrixDouble penaltyTimeDelays;//vector containing temporal delay in payments
	//@}

	void returnFscan(int retVal, char* strVar);

};


#endif


