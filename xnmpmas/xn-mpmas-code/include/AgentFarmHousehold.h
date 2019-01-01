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
//  File:       AgentFarmHousehold.h (formerly agentF.h)
//
//  Contents:   Header of farm-household class that is derived
//              from the basic class "agent"
//
//
//------------------------------------------------------------------------------


#ifndef _AgentFarmHousehold_h
#define _AgentFarmHousehold_h

#include "BasicDefines.h"
#include "BasicFunctions.h"
#include "Agent.h"
#include "BasicMath.h"
#include "MatrixDouble.h"
//#include "AgentParcel.h"
#include "AgentParcelList.h"
#include "AgentHouseholdList.h"
#include "AgentLivestockList.h"
#include "PopulationNetworkSegment.h"
#include "AgentParcelGridCellInterface.h"
#include "TimeHandlerClass.h"
#include "AgentMipProblem.h"

#ifdef LIVSIM_COUPLING
#include "MpmasLuciaLivSimCoupling.h"
#endif
//header files of STL for "sort"
#include <vector>
#include <map>
#include <deque>
#include <algorithm>
#include <iostream>

class IrrigationMethodSector;
class CropWater;

#ifdef PAR2
	#include "mpi.h"

	void initMpiResultStruct(int ncol, int nrow, int snrow);

	struct ResultstructMPI{
		double obj;						// Objective value
		int mp;							// Number of loaded problems
		double* colsol;					// Column solution
		double* LHS;					// Left hand side
		double* RHS;					// Right-hand side hand side
		double* shadowP;					// ShadowPrices
	};
#endif
#ifdef RNGSTREAM
	typedef std::bitset<24> farmBitCode;
	struct farmRandCollection
	{		int fstID;
			farmBitCode farmCode;

			unsigned long farmStartSeed[6];
			unsigned long nextSeed[6]; //for general randomized asset decisions
			unsigned long nextLivestockSeed[6]; // for livestock/offspring decisions

			double randInitialAge;

			double getRandAgeAtInit() { return randInitialAge; }

			double getRandForNextOffspringDecision();

			farmRandCollection(int fstID);

			void writeNextSeedToStream(FILE* stream);
			void writeToStream(std::ostream& out);
	};
#endif
// Troost 20130813 Cash Flow CountryModel Germany
// Troost **********begin block insert**********
struct CoefByColumn
{
		int Col;
		double Value;
		CoefByColumn(int, double);
};
// Troost **********end block insert**********

// Troost 201600506 Assets for multiperiod
// Troost **********begin block insert**********
struct newInventoryObject
{
		int objID;
		double quantity;
		int age;
		double value;
		newInventoryObject(int id, double q, int _age, double val) :
				objID(id), quantity(q), age(_age), value(val)
		{
		}
};
struct objAccountingInfo
{
		double quantity, acqcost, eqshare, interestLoan;
		objAccountingInfo(double q, double pp, double ssf, double iLC) : quantity(q), acqcost(pp), eqshare(ssf), interestLoan(iLC){};
};
// Troost **********end block insert**********

#ifdef MULTIPERIOD
class yieldHistory
{
		typedef vector<vector<deque<double> > > vvdDouble;

		vvdDouble yield;
		vvdDouble stover;
		vector<vvdDouble> extraAttrs ;

public:
		yieldHistory(int numCA_, int numNRU_, int numExtra_, caYld** start);
		yieldHistory();

		~yieldHistory();

		void appendYear(size_t numCA_, size_t numNRU_, size_t numExtra_,  caYld** newRecord );

		void writeToStream(ostream& out, int fstId);

		double getIndividualYield(size_t ca, size_t nru, size_t year);
		double getIndividualStoverYield(size_t ca, size_t nru, size_t year);
		double getIndividualExtraAttr(size_t ca, size_t nru, size_t extra, size_t year);
		deque<double>* getYieldSeriesPointer(size_t ca, size_t nru);
		deque<double>* getStoverYieldSeriesPointer(size_t ca, size_t nru);
		deque<double>* getExtraAttrSeriesPointer(size_t ca, size_t nru, size_t extraAttrId);

};
struct expectationParameters {

		int methodPrices;
		int methodYields;
		vector<double> parametersYields;

		expectationParameters() : methodPrices(-1), methodYields(-1), parametersYields(){};
};
#endif




class agentF : public agent
{	public:
	
//------------------------------------------------------------------------------------
	///@name Get and set functions
	//@{
	virtual void	setPtrToInvestmentObjects(const investmentObject* investmentObjectsInNetwork);
// Troost 20141210 Land markets - type 4, assets traded with land
// Troost **********begin block insert**********
	virtual const investmentObject* getPtrToInvestmentObject(int code);
// Troost **********end block insert**********
	virtual void	setSegPtr(segment*);
	virtual segment* getSegPtr();

	void setPNSPointers();

	virtual int		getNumberOfPlots();
	virtual void	setNumberOfPlots(int);
	virtual void	addToNumberOfPlots(int);
	virtual int		getNumberOfIrrigablePlots();//special treatment for Ghana
	virtual int		get_segment();
	virtual void	set_segment(int);
	virtual int		get_innov();
	virtual int		get_erwart();
	virtual void	set_erwart(int);
	virtual int		getCluNum();
	virtual int		getPopNum();
	virtual int		getFstID();
	virtual int		getAgentMapId();
	virtual int		getMapColor();
	virtual void	setMapColor(int clr56);
	virtual void	setLogitRandom(double rnd);
	virtual void	setTimePreference(double timePref);
	virtual void	setMarketAccess(int market);

	virtual int getFirstInflowID();

	virtual double get_b_wert();//Betriebswert
	virtual int		getSolvedLP();
	virtual double get_lastTgtPrd();
	virtual void set_lastTgtPrd(double);
	virtual double get_lastTgtInv();
	virtual void set_lastTgtInv(double);
	virtual double get_lastTgtInvLin();
	virtual void set_lastTgtInvLin(double);
	virtual double get_lastTgtMkt();
	virtual void set_lastTgtMkt(double);
	virtual double get_lastTgtCons();
	virtual void set_lastTgtCons(double);



	virtual int    get_FamAK();
	virtual void   set_FamAK(int);
	virtual int    get_FrAK();
	virtual void   set_FrAK(int);
	virtual double get_EK();
	virtual void   set_EK(double);
	virtual double get_liq();
	virtual void   set_liq(double);
	virtual double get_akt();
	virtual void   set_akt(double);
	virtual double get_FK();
	virtual void   set_FK(double);
	virtual double get_entfk();
	virtual void   set_entfk(double);
	virtual double get_werts();
	virtual double get_durchsDB();
	virtual int    get_migr();//migration decision
	virtual void set_migr(int);

	double getInterestRateLongTermCredit() { return *rateLC_ptr;}
	double getInterestRateShortTermCredit() { return *rateSC_ptr;}
	double getInterestRateShortTermDeposits() { return *rateSD_ptr;}
#ifdef MULTIPERIOD
	double getDiscountRate() { return *rateSD_ptr;}
#endif

	virtual int    checkNumberOfAllUsablePlots();//including plots rented-in
	virtual int		getNumberOfUsablePlotsInSoilType(int soilType);//renamed from <getNumPlotsInSoilType>
	virtual int    get_plotsPerType(int typ);
// Troost 20130910 Make land markets work with external crop growth model
// Troost **********begin block insert**********
	virtual void   first_set_plotsPerType(int typ, int n);
	virtual void 	  updateNRUs();
// Troost **********end block insert**********
	virtual void   set_plotsPerType(int typ, int n, mpmasPlotCell* p = NULL, bool plotIsTakenOut = false);
	virtual int getNumberOfSoilRowsInLP();

	virtual int    get_hof_x();
	virtual void   set_hof_x(int);
	virtual int    get_hof_y();
	virtual void   set_hof_y(int);
	virtual void	 transferPlotAsRentedOut(int r, int c, w_rechte* w, int years, double pmt, int newAgID, int newScID, int typ);//refactored from <set_bflaech_verpachtet>
	virtual void 	 receivePlotRentedIn(mpmasPlotCell* p, w_rechte* w);
	virtual void 	 receivePlotRentedOutReturned(mpmasPlotCell* p, w_rechte* w);
	//virtual void   make_bflaech(int c, int r, int typ, int fst, double dst, parcelGroup* pg = NULL);
	virtual void   make_bflaech(mpmasPlotCell* pp);
	virtual void   bflaechen_printOnScreen();
	//virtual void   bflaech_einfuegen(mpmasPlotCell*);
	virtual void  receivePlotAsNewOwnerAndUser (mpmasPlotCell* p, w_rechte* w);


	virtual double getEtaOfAgent(){return etaOfAgent;}
	virtual double getTotalIrrigationWaterApplied() {return totalIrrigationWaterApplied;};
	
//------------------------------------------------------------------------------------

	/**
	* Functions handling farm assets
	*/	
	
	///Functions related to "symbolic" objects
	virtual int get_sym_obj(int);//Parameter Nr
	//virtual void set_sym_obj(int, int);//Parameter Nr, Flag ## Tbe 120402 not in use anymore
	virtual void makeSymObj(int oID);
	virtual int lookSymObj(int oID);
	virtual void copyCoeffSymObjMILP();
	virtual void printSymObjsOnScreen();

// Troost 20140611 Disinvestment of assets
// Troost **********begin block insert**********
	virtual void prepareDisinvestments();
	virtual void copyCoeffDisinvestmentMILP(vector<CoefByColumn>*);
	virtual int getIndexInvObjForCode(int code);
// Troost **********end block insert**********

	///Functions related to investment goods
	virtual int get_binv_size();
	virtual int get_binv_anz_obj();
	virtual char* get_binv_acc(int i);
	virtual void set_binv_acc(int i, char* chr);
	virtual int get_binv_code(int i);//Parameter Row
	virtual double get_binv_ask(int i);//Parameter Row
// 121206 Troost correct price for old assets
// ********* begin insert ******************
	virtual void set_binv_ask(int i, double price); //Parameter Row
// ********* end insert ********************
	virtual double get_binv_anz(int i);//Parameter Row
	virtual int get_binv_age(int i);//Parameter Row
	virtual void set_binv_age(int i, int age);//Parameters Row, age
	virtual double get_binv_shareEC(int i);//Parameter Row
	virtual void set_binv_shareEC(int i, double shr);//Parameters Row, share
	virtual double get_binv_rateDC(int i);//Parameter Row
	virtual void set_binv_rateDC(int i, double rat);//Parameters Row, rate
// Troost 20141210 Land markets - type 4, assets traded with land
// Troost **********begin block insert**********
	virtual investdef get_binv(int);
// Troost **********end block insert**********
	virtual void binvest_vergroessern();//erhoeht die Speicherplatzgroesse
	virtual int inventarisieren(char* acc, int oID, double pPr, double siz, double ssf, double iDC, int age, string remark, int plotX = -1, int plotY = -1, int newlyAttach = false, int plotCapSlot = -1, double plotCapUsed = 0.0);
	virtual int inventarisieren(investdef asset , string remark,  int newlyAttach = false,  int plotX = -1, int plotY = -1, int plotCapSlot = -1, double plotCapUsed = 0.0);
	virtual void deinventarisieren(int, string remark, int detach = 1, int disinvest = 1);// Uebergabe: Inventarnr.
	virtual void binvest_ausschreiben(int which = -1);
	virtual void binvest_writeToStream(FILE* strm);
	virtual int getObjRowForCol(int cLP);//gibt Obj.nr in Invest_matrix
	virtual int getObjCodeForCol(int cLP);//gibt Objektcode zurueck
	virtual int getObjRowForCode(int code);//
	virtual int getCapRowForCode(int code);//liefert Kapazitaetsnr.
	virtual double getSelfFinanceShareForCode(int code);
	virtual double getCreditInterestRateForCode(int code);
	virtual int get_n_dauer(int code);//ermittelt Nutz.dauer
	virtual int getRegRowForCode(int code);//Nr. im Dauerkulturenregister ermitteln
	virtual int getRegRowForLpColumn(int lpCol);

	///Specific functions for infrastructure projects
	virtual void checkIfAgentHasInfrastructureProjects();
	
	///Soil type requirement of investment object
	virtual int getSoilType(int objID);

	virtual double get_lohn();//ermittelt ausserlandwirtschaftliches Einkommen
	///returns user information on investment good
	virtual nutz get_nutz_info(int);

	///Functions handling permanent crops
	virtual void updatePerennialsRegistry();
	virtual int makeEntryInPerennialsRegistry(int objID, int colLP);
	virtual void computeIndividualObjectCoefficients(int objectID, double sizeHa, int age, int idxPermCrops, double interest);

	virtual void dauerkultur_austragen(int soilType, double sizeHa);//## soil rename function
	virtual double get_fruehe_produktionskosten(int);
	virtual double getHectaresOfPermanentCropsOnSoilType(int soilType, int);

	// ... Additional string passed for error messages   
	virtual void blockPlotsWithPermanentCrops(int soilType, double existingHa, double deltaHa, string outputStr);

//------------------------------------------------------------------------------------

	/**
	* Functions called at beginning of period
	*/	

	///Functions for building expectations
	virtual void makePriceExpectation();
	virtual void makeWaterSupplyExpectationYearly();
	virtual void makeWaterSupplyExpectationInMonth(int m);
	virtual void makeWaterDemandExpectationYearly();
	virtual void makeWaterDemandExpectationInMonth(int m);
#ifndef MULTIPERIOD
	virtual void makeYieldExpectationsForCropWat();
	virtual void initializeYieldExpectationsForCropWat();


	virtual void makeYieldExpectationsForTspc();//renamed from <makeYieldExpect>
	virtual void initializeYieldExpectationsForTspc();//renamed from <initYieldExpect>
#endif //ndef MULTIPERIOD

	virtual void makeYieldExpectationsForLucia();
	virtual void initializeYieldExpectationsForLucia();
	virtual void initPlotsInNRUs();
// Troost 20130910 Make land markets work with external crop growth model
// Troost **********begin block insert**********
	virtual void updatePlotsInNRUs();
// Troost **********end block insert**********
	
// Troost 20140527 Generalize dynamic NRUs
// Troost **********begin block insert**********
	virtual void makeCellsInSoilTypes();
// Troost **********end block insert**********

	///Functions for rental market activities
	virtual int pacht_ueberlegen();	//Ueberlegungen ueber Pacht/Zupacht
	virtual double hole_q_B(int);//liefert Boden-Schattenpreis
	virtual double hole_q_W(int);//liefert Wasser-Schattenpreis
	virtual double hole_volB(int);//liefert erwartetetes Brutto-Water
	virtual void forderung_speichern(int, p_gebot* f);
	virtual void forderung_holen(int typ, p_gebot* f);
	virtual void forderung_loeschen(int typ);//loescht eine gespeicherte Forderung
	virtual void matrix_forderung_loeschen();
	virtual double get_zus_TK();
	virtual void set_zus_TK(double);
	virtual double getRentalPayments();
	virtual void setRentalPayments(double pmt);
// Troost 20141210 Land markets - type 4, assets traded with land
// Troost **********begin block insert**********
	virtual void addToAssetSales(double pmt);
// Troost **********end block insert**********

// Troost 20140630 Voluntary exit decision in Investment LP
// Troost **********begin block insert**********
	virtual void addToRentalPayments(double pmt);
	virtual double getRentalPaymentsReceived();
	virtual void setRentalPaymentsReceived(double pmt);
	virtual void addToRentalPaymentsReceived(double pmt);
#ifndef MULTIPERIOD
	virtual double decideOnBidForParcelGroup(parcelGroup* pg, int milpMode);
#endif //ndef MULTIPERIOD
#ifdef MIPSTART
	virtual double decideOnBidForParcelGroup_ColumnBasedValuation(parcelGroup* pg, int milpMode);
#endif

	virtual void unregisterParcelGroup(parcelGroup* pg);
	virtual void registerParcelGroup(parcelGroup* pg);
// Troost **********end block insert**********
// Troost 20141210 Land markets - type 4, assets traded with land
// Troost **********begin block insert**********
	virtual void attachAssetToPlotAndInventarize(char acc[MXLENGTH], int code, double pp, double umfS, double ssf, double iLC, int age, int slot, double capUsed, int toRented, int requiredSoilType, string remark );
	virtual void deregisterRentedAttachedAssets(mpmasPlotCell* p);
// Troost **********end block insert**********

	virtual double computeDistanceFromFarmstead(int xCoord, int yCoord);
	virtual int computeDirectionFromFarmstead(int xCoord, int yCoord);
	virtual double computeTransportCostFromFarmstead(int xCoord, int yCoord);
	virtual void addWaterRightsRentedIn(int mapRow, int mapCol, w_rechte* w);
	virtual void printWaterRentedInOnScreen();
	virtual void arrangePaymentsOrReturnPlotsRentedIn();
	virtual void updateAgentPlotArrays();

// Troost 20170516 Advanced Demography Model 2
// Troost **********begin block insert**********
#ifndef MULTIPERIOD
	virtual void checkApplicationForLandRedistribution(milpModeTypes milpmode);
	virtual void submitApplicationForLandRedistribution();
#endif
// Troost **********end block insert**********

//------------------------------------------------------------------------------------

	/**
	* Farm decision-making functions using MP
	*/	
#ifndef MULTIPERIOD
	///Functions for investment, production, marketing, and consumption
	virtual double investPlan(int saveSol, double* solutionToBeSavedForWarmStart = NULL); //## substitute Psi 131129
	virtual double makeInvestmentPlan(int saveSol, bool saveForWartstart = false, double* solutionToBeSavedForWarmStart = NULL);
	virtual double producPlan(int saveSol, double* solutionToBeSavedForWarmStart = NULL); //## substitute Psi 131129
	virtual double makeProductionPlan(int saveSol, bool saveForWartstart = false, double* solutionToBeSavedForWarmStart = NULL);
	virtual double marketPlan(int doMarketing, double* solutionToBeSavedForWarmStart = NULL); //## substitute Psi 131129
	virtual double makeMarketingPlan(int doMarketing, bool saveForWartstart = false, double* solutionToBeSavedForWarmStart = NULL);
	virtual double consumPlan(int doConsume, double* solutionToBeSavedForWarmStart = NULL); //## substitute Psi 131129
	virtual double makeConsumptionPlan(int doConsume, bool saveForWartstart = false, double* solutionToBeSavedForWarmStart = NULL);
#endif //ndef MULTIPERIOD

#ifdef MIPSTART
	vector<double> landRentalValuation(map<int,parcelGroupOffer*>::iterator firstOffer,map<int,parcelGroupOffer*>::iterator afterLastOffer , double* solutionToBeSavedForWarmStart = NULL);
#endif //MIPSTART

#ifdef MULTIPERIOD
	double makeMultiPeriodPlanStartOfYear(doInvestModes, string);
	double multiperiod_startOfYearPlan(doInvestModes, string);
	void resetLiquidityAccounts();
#endif
	virtual void implement_investments();

#ifdef MULTIPERIOD
	virtual void multiperiod_implementInvestments();
	virtual int multiperiod_tryToDisinvest(int code, int age, double quantity, int adaptCapital, vector<objAccountingInfo>& accountInfo, string remark, int dieOnError = true);
	virtual void multiperiod_updateAssetsOffspring();
#endif

	vector<investdef> externalExportAssetRegistry();
	void externalImportAsset_fromStream(istream& in);
	void externalImportAsset(int pos, investdef asset);
#ifndef MULTIPERIOD
	void checkInvestmentDecisionBeforeExternalScriptCall(milpModeTypes milpMode);
#endif //ndef MULTIPERIOD
	void reportSolutionForExternalScriptCall(map<int,vector<double> > & outputMapWithSolutions);
	void setSpecificUboundsForDecisionExternalScripts();

#ifndef MULTIPERIOD
#ifdef DISTEST
	virtual void implement_investmentsNew(); //intermediate while coding, rename later
#else
	virtual void implement_investmentsOld(); //intermediate while coding, rename later
#endif
	virtual void implement_investmentsOSL();
#endif //ndef MULTIPERIOD

	virtual void inventorizeDeferredObjects();

#ifdef MULTIPERIOD
	virtual void multiperiod_implementSingleInvestment(int rowInObjects, double umf, int age);
#endif
	virtual double implementSingleInvestment(int rowInObjects, double umf, MatrixDouble& newHectaresPermanentCrops, int& permCrops);
	virtual void updateCapitalAccounts(double own, double foreign);
	virtual void updateBlockedAreasPermanentCropsAfterInvestment(MatrixDouble& newHectaresPermanentCrops);
	virtual void updateRegistryAndZRowPermanentCropsAfterInvestment();
// Troost 20140611 Disinvestment of assets
// Troost **********begin block insert**********
	virtual double implement_disinvestments();
// Troost **********end block insert**********

// Troost 20140630 Voluntary exit decision in Investment LP
// Troost **********begin block insert**********
	virtual bool checkVoluntaryExitAfterInvestmentDecision();
// Troost **********end block insert**********
	virtual void updateAgentAttributesInZRow();
	virtual void updateAgentAttributesInMatrix();
	virtual double getAgentAttributeForMatrix(int attribute, int i, int attribute2 = -1);
#ifdef RNGSTREAM
	virtual void updateHouseholdMembersInZRow();
	virtual void updateHouseholdMembersInMatrix();
#endif
	virtual void updateFarmsteadNRUInZRow();
	virtual void updateFarmsteadNRUInMatrix();

// Troost 20140527 Generalize dynamic NRUs
// Troost **********begin block insert**********
	virtual void updateLandscapeAttributesInZRow();
	virtual void updateLandscapeAttributesInMatrix();
	virtual double getAggregatedUserDefinedLandscapeParameterForNRU(int paramID, int agg, int nru);
// Troost **********end block insert**********

#ifdef MULTIPERIOD
	virtual double getAggregatedUserDefinedLandscapeParameterForSharedNRU(int paramID, int agg, int nru);
	int getNumberPlotsOfCommonPropertyOfSoilType(int nru);
	vector<int> getListOfCommonPropertyGroupMembership();
	void addToListOfCommonPropertyGroupMembership(int);
#endif

	///Functions for solving MP problems
	virtual int getNNrowsInternal();
	virtual int getNNcolsInternal();
	virtual int getNumRowsInternal();
	virtual int getNumColsInternal();
	virtual int getActCshInternal();
	virtual int getCapLiqInternal();
	virtual int getCapLabInternal();
	virtual int getCapLndInternal();
	virtual int getCapWatInternal();
	virtual int getCapCshInternal();
	virtual int getCapCashFlowEndOfYearInternal();
	virtual int getCapShortTermCreditBalanceInternal();
	virtual int getCapShortTermCreditDefaultInternal();
	virtual int getCapAvlInternal();
	virtual int getCapMacInternal();
	virtual int getCapInvInternal(int soil);
	virtual int getCshSpansInternal();
	virtual int getLabSpansInternal();
	virtual int getfixAdultBinInternal();
	virtual int getNutrientDemandRowInternal();

// Troost 20130813 Cash Flow CountryModel Germany
// Troost **********begin block insert**********
	virtual int getCapCashFlowRHSInternal();
	virtual int getCapCashFlowRHSminConsInternal();
	virtual int getCapCashFlowCoefInternal();
// Troost **********end block insert**********

	virtual infoPerf getInfocashOnfInternal();
	virtual infoPerf getInfocashOffInternal();
	virtual infoPerf getInfotLabOnfInternal();
	virtual infoPerf getInfotLabOffInternal();
	virtual infoPerf getInfopLabOnfInternal();
	virtual infoPerf getInfopLabOffInternal();

	virtual double getOriginalMtxCoeffInternal(int lpRow, int lpCol);
	virtual void setMtxCoeffInternal(int lpRow, int lpCol, double value);
	virtual void setObjCoeffInternal(int lpCol, double value);
	virtual double getObjCoeffInternal(int lpCol);
	virtual void setVarRHSInternal(int lpRow, double value);
	virtual double getVarRHSInternal(int lpRow);

	virtual void fixLaborBinaryInternal(double femLab, double malLab);
	virtual void fixProductionVariablesInMarketingModeInternal(l_vektor& bprod);
	virtual void fixProductionVariablesInConsumptionModeInternal(l_vektor& bprod);
	virtual void changeElementsInMatrixInternal(int num, int*& rows, int*& cols, double*& elms);
	virtual void changeObjectiveFunctionInternal(double*& objF, int oSta, int oEnd);
	virtual void setColumnUpperInternal(int lpCol, double value);
	virtual void setToEEConstraintInternal(int rLP, double value);
	virtual void switchOffFineTuneInternal();

	virtual int hasPrblSetsInternal();
	virtual double getActualTotalGrossMargin();
	virtual double getBaselineTotalGrossMargin();
	virtual void setBaselineTotalGrossMargin(double tgt);

// Troost 20121208 - Advanced demography model
// Troost **********begin block insert**********
	virtual double getSolutionForActivity(int idx);
	mpPrbl* getPointerToMIP();
// Troost **********end block insert**********

	///Functions for TSPC-related LP info
	virtual int identCrAcForColInternal(int colLP, int& crA, int& soil);
	virtual int getColLPInternal(int crA, int soil);
	virtual int getTypCrInternal(int crA, int soil);
	virtual int getRoYldInternal(int crA, int soil);
	virtual int getRoYsmInternal(int crA, int soil);
	virtual int getColOpInternal(int crA, int soil);
	virtual int getColStvInternal(int soil);
	virtual int getColManInternal();


	virtual void inv_leistungen();	//traegt die Leistungen der Investitionsobjekte in die Kapazitaeten ein
#ifdef MULTIPERIOD
	void makeAssetCapacityMatrix();
#endif
	virtual void rhs_Runden();

	virtual double get_kapazitaeten(int);
	virtual void set_kapazitaeten(int, double);
	virtual double get_schattenpreise(int);
	virtual void schattenpreise_speichern();
	virtual void schattenpreise_wiederherstellen();

#ifndef MULTIPERIOD
	virtual void computeShadowPricesForLand(int milpMode);
	virtual void computeShadowPricesForWater(int milpMode);
	virtual void computeShadowPricesForAdjacentContraints(int milpMode, int firstRow, int numberRows);


	virtual void updateAgentZRow();
	virtual void copyZeroColumnsToAgentZRow();
	virtual void updateStartPeriodRHS(int reload=false);
	virtual void updateEndOfPeriodRHS();
	virtual void copyInvPCropsRHS();
#endif //ndef MULTIPERIOD

	virtual void checkForNegativeValuesInRHS();
#ifdef MULTIPERIOD
	void multiperiod_updateAgentVariablesAtStartOfPeriod();
	void multiperiod_prepareMIP();
	void addCropProductionAreasToResultArrayAndAgentCropAreaVector(double* resultArray, int sizeResultArray);
	void makeNRUCropProductionAreas();
	void multiperiod_allocateIntegerCropAreasSpatially();
	void multiperiod_allocateIntegerCropAreasSpatially_NRUs();
	void importYieldsAggregatedLandUse( int numberCroppingActivities, int* CropManagementYldArrayIndex, int* soiTypeID, double* cropYield, double* stoverYield, double** extraAttrs);
	void multiperiod_importYieldsFromCells(bool harvestAfterNewLandUseDecision);

#else //ndef MULTIPERIOD
	virtual int copyLPdataInvMode(int numIntegSets, int doInvest); //
	virtual int copyLpDataInInvestmentMode(int isIntegerProblem);
	
	virtual void copyAvailablePermanentCropsInInvestmentMode(int availPCropsIdx);
	virtual void copyExistingPermanentCropsInInvestmentMode(int agentRegistryIdx, int lpCol, int lpRow);//dkult_reg
	virtual void copyAllOtherAvailableObjectsInInvestmentMode(int invObjIdx);
	virtual void copyNewLivestockInInvestmentMode(int invObjIdx);
	virtual void copyExistingLivestockInInvestmentMode(int objectID);

	virtual int copyLPdataPrdMode(int);//substitute
	virtual int copyLpDataInProductionMode(int isIntegerProblem);
	
	virtual void copyExistingPermanentCropsInProductionMode(int agentRegistryIdx, int lpCol, int lpRow);//dkult_reg
	virtual void copyExistingLivestockInProductionMode(int objectID);

	virtual void copyExpectedWaterDemandsIntoMILP();
	virtual void copyExpectedYieldsOfAnnualCropsIntoMILP();//refactoring of <copyYldExpMILP>
	
	virtual int copyLPdataMarkMode(int ipr, int doMarketing);
	virtual int copyLPdataConsMode(int ipr, int doConsume);
	virtual void copyConsCoeff(double hhEgy);

// Troost 20130109 - Succession model part 2
// Troost **********begin block insert**********
	virtual void copyMinIncCoeff();
	virtual void updateRHSMinConsAndFixCost();
// Troost **********end block insert**********


	virtual void updateSoilRelatedRHS();

// Troost 20140630 Voluntary exit decision in Investment LP
// Troost **********begin block insert**********
	virtual void updateRHSOwnedPlots();
	virtual void clearRHSOwnedPlots();
	virtual void copyCoeffActRentOutOwnedPlots();
	virtual void copyCoeffVoluntaryExit();
// Troost **********end block insert**********
// Troost 20170516 Advanced Demography Model 2
// Troost **********begin block insert**********
	void toggleUboundsAssetTransferToNewHousehold(int turnOn);
// Troost **********end block insert**********
// Troost 20130813 Cash Flow CountryModel Germany
// Troost **********begin block insert**********
	virtual void clearRHSCashFlow();
	virtual void enterRHSCashFlow();
	virtual void enterCoefCashFlow(vector<CoefByColumn>*);
// Troost **********end block insert**********

// Troost 20141210 Land markets - type 4, assets traded with land
// Troost **********begin block insert**********
	virtual void updateRHSPlotCapacities();
// Troost **********end block insert**********
	virtual void updateRHSRemDurationConstraints();

#endif //ndef MULTIPERIOD
	virtual void modifyProductionSolution(int num, int*& cols, double*& values);
	virtual void setProductionSolutionColumn(double va, int colLP);

	virtual void copyCoefficientsTimePreferenceOfMarketSales(int penaltyNumber);
	virtual void setAllCoefficientsInInvestmentActivitiesToZero();




// Troost 20130915 Additional output quadratic programming
// Troost **********begin block insert**********
	virtual double calculateLinearPartOfObjVal();
// Troost **********end block insert**********

#ifdef PAR2
	virtual void synchronizeDecisionResultsMPI(milpModeTypes mode);
#endif

#ifdef LP9
	virtual double solveLinPrbOSL(int savePrim, int saveDual);//forced solution not yet fully tested
	virtual double solveQuaPrbOSL(int savePrim, int saveDual, int solverMode);//forced solution not yet fully tested
	virtual double solveIntPrbOSL(int savePrim, int saveDual, int& forSol);
#endif //LP9
	virtual void makeNewPrb(const char* typeOfMip, double* mipstartSolution1 = NULL, double* mipstartSolution2 = NULL);
	virtual void freePrb();
	virtual void setAgentMipCount(int);
	virtual int getAgentMipCount() const;


//------------------------------------------------------------------------------------

	/**
	* Functions related to TSPC crop growth module
	*/ 

	///Functions for crop yields
#ifndef MULTIPERIOD
	virtual void computeCropTspcYields(FILE* beforeYieldsStrm, FILE* afterYieldsStrm);
#endif



//------------------------------------------------------------------------------------

	/**
	* Functions related to crop-water and irrigation module
	*/ 

	///Functions for irrigation farming
	virtual double computeAverageOnFieldIrrigationEfficiency(IrrigationMethodSector* sectorPointerToIrrigationMethods);
	virtual double irrigateCropsByPriorityGroups(int month);
#ifndef MULTIPERIOD
	virtual void computeCropWaterYields(FILE* beforeYieldsStrm, FILE* afterYieldsStrm);
#endif
	virtual void setOutputOfAllCropsToZero();
	virtual void applyYieldReductionFactorsAndUpdateLpSolution();

	///Functions for water rights
	virtual double get_N_i(int);
	virtual double get_WPa_i(int);
	virtual void set_WPa_i(int, double);//Parameters Month, Volumn
	virtual void waterrights_eintragen(int, double);//Zufluss-Nr. und Anteil
	virtual void hole_durchs_waterrights(w_rechte*);//... je Plot
	virtual void waterrights_austragen(w_rechte*);//traegt WaterRights aus
	
	///Functions related to irrigation
	virtual void createDataForCropWaterModule();
	virtual double aggregateIrrigationWaterDemandsByIrrigationMethod(int month);
	virtual double receiveNetInflows(int month, int = true);//Flag (save sector values yes/no)
	virtual double computeIrrigWaterSupplyAndPlantWaterDemandSatisfied(int month, double Ueb_j_tmp);


//------------------------------------------------------------------------------------

	/**
	* Functions related to external crop growth module (Lucia, Expert-N or simple look-up table in file)
	*/ 

	///Functions for crop yields
#ifndef MULTIPERIOD
	virtual void computeCropLuciaYields(FILE* beforeYieldsStrm, FILE* afterYieldsStrm);
#endif
//------------------------------------------------------------------------------------

	/**
	* Functions handling farm surveys
	*/	

	///Collect farm production of tradables
	virtual int replyToFarmProductionSurvey(MatrixDouble& questionnaire, int& firstLpColumn);
	virtual int replyToFarmIncomeSurvey(MatrixDouble& questionnaire);

	virtual double getLpSolution(int lpCol);


//------------------------------------------------------------------------------------


	/**
	* Functions called at end of period
	*/
#ifdef MULTIPERIOD
	
	void multiperiod_estimateYieldsOfMissingCropsBySimilarity();
#else
	///Function for crops yields
		virtual void resetActualYieldsToDefault();
		virtual void makePlantYields();//formerly 'ertraege_berechnen()'
	virtual void estimateYieldsOfMissingCrops();
#endif

	///Functions related to income statement
	virtual double estimateValueOfOwnLandAndWater();//renamed from <bodenvermoegen_bestimmen>
	/*virtual void einkommen_berechnen();//not used anymore "*/
	
	virtual void computeIncome();//refactored from "einkommen_berechnen()"
#ifdef MULTIPERIOD
	virtual void multiperiod_AccountingAndIncomeStatement();
	virtual void saveAgentIncomeDataInFile(double gemeinkosten, double transportkosten);
#endif
	virtual void computeTotalGrossMarginAndCashFlow(int saveAgent, FILE* incomeDataStream);
	virtual double computeCashConsumption();
	virtual double redeployAssets(w_rechte* w);//renamed from <vermoegen_umschichten>
	virtual int checkFarmCloseDownDecision(double valueOfLand);//renamed from <betriebsaufgabe_pruefen>
	virtual double abandonAllWaterAndLandResources(w_rechte* w);// ## substitute
	virtual double redeployAllAssets(w_rechte* w, int sold);
	virtual double redeployAssetsParcelGroupVersion(double target, w_rechte* w);
	virtual double computeDepreciationAndReturnPrincipal(double& lfzinsen, int updateAge);//renamed <from abs_tilg_berechnen>
	virtual double computeTransportCosts();
	virtual double computeFarmFixCosts();
// Troost 20141210 Land markets - type 4, assets traded with land
// Troost **********begin block insert**********
	virtual void receiveEuCap2003Entitlements();
	virtual void recordPastSubsidiesForEuCap2003Entitlements();

// Troost **********end block insert**********


// Troost 20121208 - Advanced demography model
// Troost **********begin block insert**********
	virtual double getIncome();
// Troost **********end block insert**********

	//Functions concerning household members
#ifdef RNGSTREAM
	virtual void addHHmember(int minAge, int maxAge, int sex, int head, int career);
	virtual void addHHmember(int minAge, int maxAge, int sex, int head, int career, int probablyMarried);
#else
	virtual void addHHmember(int age, int sex, int head, int career);
	virtual void addHHmember(int age, int sex, int head, int career, int probablyMarried);
#endif

	virtual void initializeHHmemberStatus();

	virtual void makeHeadOfHH(int sex);

// Troost 20121208 - Advanced demography model
// Troost **********begin block changed**********
//	virtual void updateAgeOfMembers();
	virtual int updateAgeOfMembers(FILE* outStream);
// Troost **********end block changed**********
		
// Troost 20170516 Advanced Demography Model 2
// Troost **********begin block insert**********
#ifndef MULTIPERIOD
	agentF* transferAssetsAndLandToNewHousehold(int whichNew, vector<hhmember*> memberList);
	void transferAssetsAndLandToLandOwnerAgent();
#endif
	int getNextNewFstID();
	coord determineLocationOfNewFarmstead( const set<mpmasPlotCell*, plotSetSorter> & plotsToTransfer);
   void includeNewHouseholdMembers(vector<hhmember*>& memberList);
   void receiveAssetsAsNewOwner(vector<investdef>& assetList, string , bool permanentCropsAlreadyBlocked = true);
#ifdef MULTIPERIOD
   void initializeExpectationsFromOtherAgent(caYldExp** otherExpYld,		caYld** otherActYld, double* otherPriceExp  );
#else
   void initializeExpectationsFromOtherAgent(caYld** otherExpYld, caYld** otherActYld, double* otherPriceExp  );
   void checkIfNewFarmingHouseholdsAreGeneratedAndReleaseAssetsAndLand(vector<agentF*>& newFarms, FILE* outStream);
#endif
// Troost **********end block insert**********

	///Functions for carry-over of resources, especially livestock

	virtual void addUnitLstck(int type, int age);
	virtual void updateOffspOfLstck();
	virtual void updateAgeOfLstck();

	virtual void updateSalesOfLivestock();
	virtual void updateAges(FILE* outStream);
#ifdef MULTIPERIOD
	void updateAssetAges();
#endif
	virtual double updateIdleLsInBinvest(MatrixDouble& lsIdle);
	//virtual double updateSoldLsInBinvest(MatrixDouble& lsSold);
	virtual double updateSoldLsInBinvest(vector<livestockPack>&lsSold);

	virtual void incrLiqStartPeriod(double amount);
	virtual void incrLiqEndOfPeriod(double amount);
	
//------------------------------------------------------------------------------------

	/**
	 * Functions that save or write outputs 
	 */ 

	///Write land and and water-related data
	virtual void writeAgtLandData(FILE* stream0);
	virtual void writeAgtWaterData(int m);

	///Write expected and actual values over all periods (file in append mode)
	virtual void writeExpectedMarketPriceData(FILE* strm);
	virtual void writeExpectedWaterSupplyData(FILE* strm);
	virtual void writeExpectedCropWaterData(FILE* strm);
	virtual void writeExpectedCropYieldData(FILE* strm);

	///Write agent solution from production problem to file
	virtual void writeAgentProductionSolutionToFile(char* filename);

	//Function writes the first lines of the agent - which is needed for every output file to identify agents
	///Production and LP capacity data
	virtual void writeAgtIdEntry(FILE* stream);
	virtual void writePfmPrdData(FILE* stream0, FILE* stream1);
	virtual void writeCapRHSData(FILE* stream);
	virtual void writeHhListToStream(FILE* stream);
#ifdef MULTIPERIOD
	virtual void multiperiod_writeOutDecisionResults(bool aggregateOnly);
#endif

#ifdef RNGSTREAM
	void writeHHListRandomNumbersToStream(ostream& stream);
#endif
	///Print on screen
	virtual void printOnScreenHH();
	virtual void printOnScreenWaterRights();
	virtual void bnummer_anpassen(int);

	///Print all crops from CrpMixClass
	virtual void printOnScreen_crpMixClass();
	
	///Print only one activity 'cnt' from CrpMixClass
	virtual void printOnScreen_crpMixClass(int cnt);
	virtual void printOnScreen_crpMixWater();
	virtual void printOnScreen_crpMixWater(int cnt);

	virtual void printOnScreenProduction();
	virtual void writeToFileProduction(ofstream& out);
	virtual void printOnScreen_AllInfo(int soilC, int cnt);
	virtual void printOnScreen_bflaech(void);
	virtual void printSymbolicObjects();

	virtual void printOnScreenLv();
	virtual void printOnScreen2();
	virtual void printOnScreenWater();
	virtual void printOnScreenCropMatrix();

	virtual void writeAllCropWaterDataToFile();
	virtual void writePercentageByIpg(FILE* out);
	virtual void appendCropWaterYieldsToFile();
	virtual void appendWaterUseAndEconomicReturnsToFile();

	virtual void writeAllPlotsToFile(ofstream& out);
	virtual void writeWaterInfoToFile(ofstream& out);

//------------------------------------------------------------------------------------

	/**
	 * Functions needed for spatial allocation and TDT coupling
	 */
#ifndef MULTIPERIOD
	///Crop mix implementation and spatial allocation of crops
	virtual void createCropMixFromSolutionVector();// renamed from <createDataForSpatialAllocationOfCrops>
	virtual void allocateCropActivitiesSpatially(void);

	virtual void fixRoundingErrorsInProductionSolution(int soil, int cropIdx, double ha);
	virtual void addFallowToProductionSolution(int soil, int cropIdx, double ha);
	virtual void fixEmptyPlotErrors();
	virtual void addFallowToEmptyPlots(int soil, int cropIdx, int actID);
#endif //ndef MULTIPERIOD
	//get number of crop mixes and crop activities, according to crop growth model
	virtual int getNumberOfCropMixes();
	virtual int getNumberOfCropActivities();
#ifndef MULTIPERIOD
	virtual int getCropMixIndexForSoilType(int soilType);
	virtual void printAgentCropMixToFile(char* filename);
#endif //ndef MULTIPERIOD
	virtual void printAgentPlotListArrayToFile(char* filename);
#ifndef MULTIPERIOD
	//get info for zero labor activity (=fallow), according to soil type or NRU
	virtual int getLpColumnOfZeroLaborActivity(int nru);
	virtual int getActivityIdOfZeroLaborActivity(int nru);
	virtual int getPriorityOfZeroLaborActivity(int nru);
#endif //ndef MULTIPERIO
	// For coupling and spatially active runs
	/* TODO 2 -oArnold -cImplement : Using content type (here a switch must still be implemented) information is retrieved and then assigned to the content type, into a temporary variable */
	virtual void assignToCropMixClassTemp_ByContentType(Content cont, int m);
	
	// Note: make sure that even if no irrigation exists, crop mix class is somehow initialized with content
	virtual void touchCropMixClassTemp_WithEmpty();
		
	// Switch only, externalized for simplicity
	virtual double getValueForCropMix_ByTempType_SWITCH(Content cont, int crA, int g, int m, CropWater* cropWat);

	// ... does both of the above
	virtual void copy_CropMixClass2LandscapeFromTemp_monthly(Content cont, int m);//Tbe 090303 fixed bug
	virtual void copy_Landscape2CropMixClassTemp_monthly(Content cont, int m);//Tbe 090303 fixed bug
	virtual void copy_Landscape2CropMixClassTemp(Content cont);//Tbe 090303

#ifndef MULTIPERIOD
	// Interpret data in crop mix class
	virtual void interpretTempInCropMixClass(Content cont, int m);//## substitute
	virtual void interpretDataRetrieved(Content cont, MatrixDouble& vector_data);//## substitute
	virtual void processReceivedDataInCropMixes(Content cont);//Tbe 090303
#endif //ndef MULTIPERIOD
	//virtual void copyContent_Landscape2Parcels(Content cont);// ## Tbe 110920 not in use
	//virtual void copyContent_Landscape2Parcels(Content cont, bool flag_fixCopyToCell);// ## Tbe 110920 not in use
	//virtual void copyContent_Landscape2Parcels_monthly(Content cont, int m); // ## eliminate
	virtual void copyContent_PlotToLandscape(Content cont);
	//virtual void copyContent_ParcelToLandscape_monthly(Content cont, int month);// ## Tbe 110920 not in use
	
	// ... for testing
	virtual double getMeanOfAllPlots(Content cont);

	//virtual void resetAllParcels_toNotUsed(void); //## Tbe 100616
	virtual void resetAllPlotsAndCropMixes();//cleans parcel list and crop mixes after use

	// check status of parcels
	virtual void checkIfTrue(bool isTrue);//checks status of parcels in crop mixes 
	virtual void checkIfAllPlotsHaveCrops();//checks status of parcels in landscape
	
	virtual void addLanduseToColumnOfMatrix(MatrixDouble& matrixOfCatchmentLandUses, int uaID);
	virtual void checkActivityIDconsistent(int a, string functionName);
	virtual double getActivityArea(int a);

//------------------------------------------------------------------------------------   	

	/**
	* Functions related to membership in producer organizations
	*/

	///function called during lottery
	virtual int checkIfProducerOrganizationDuringLottery(int symbolicObjectID);
	
	///request access to producer organization
	virtual int applyForMembershipInProducerOrganization(int symbolicObjectID);
	
	///screens production solution vector for contributions to producer organization
	virtual void collectContributionForProducerOrganization(MatrixDouble& contribution);





//------------------------------------------------------------------------------------   	

	/**
	* Functions for saving and restoring (partial) images ##Tbe 090820
	*/

	///save image of livestock list
	virtual void saveImageOfLivestockList();

	///delete livestock list
	virtual void deleteLivestockList();
	
	///restore livestock list
	virtual void restoreLivestockList();
//------------------------------------------------------------------------------------

#ifdef LIVSIM_COUPLING
	void initializeLivSimHerdForCoupling();
	void multiperiod_exportLivSimHerdManagement(LivSimHerdTable& herds_table, LivSimGrazingTable& grazing_table, LivSimFeedingTable& feeding_table);
	void multiperiod_assignAnimalsToHerdGroups();
	void multiperiod_distributeHerdsOverMap(LivSimGrazingTable& grazing_table);
	void multiperiod_exportLivsimFeedingDecisions(LivSimFeedingTable& feeding_table);
	void multiperiod_updateAssetsFromUpdatedHerd();
#endif

#ifdef MULTIPERIOD
	void initAgentCharacteristicsVector();
	void set_extraAgentCharacteristic(int id, double value);
	double get_extraAgentCharacteristic(int id);
	void add_to_extraAgentCharacteristic(int id, double value);
	void multiperiod_updateExtraAgentCharacteristics();
#endif

	/**
		Functions related to random number streams
	*/
#ifdef RNGSTREAM
	double getRandInitialAge() {return farmRandomNumbers.getRandAgeAtInit();}
#endif


#ifdef MULTIPERIOD
	void setExpectationParameters(expectationParameters input) { advancedExpectationsParams = input; }
	bool hasExpectationsParametersInitialized() { return advancedExpectationsParams.methodPrices > -1; }
	void doubleExponentialSmoothSeries( deque<double> * series, double state_weight, double trend_weight, double& newState, double& newTrend);
#endif
//------------------------------------------------------------------------------------   	
	///@name Constructor, destructor etc.
	//@{		
	///Pointers to external classes (... will be cleaned up)
	const investmentObject* investmentObjectsInNetwork;//substitute for <investmeth>
	const innovation* innovationsInSegment;//substitute for <investmeth>
	
	double* rateLC_ptr;	//Zeiger auf den FK-Zinssatz eines Segments
	double* rateSC_ptr;	//Zeiger auf den kurzfristgen FK-Zinssatz eines Segments
	double* rateSD_ptr;	//Zeiger auf den EK-Zinssatz eines Segments
	double* shareEC_ptr;	//Zeiger auf den Eigenfinanzierungsanteil eines Segments
	double* bonus_ptr; 	//Zeiger auf direkte Einkommenstransfers fuer ein Segment
	double* sunkcost_ptr;//Zeiger auf den Anteil der wiederzugewinnenden Kosten
	double* sog_ptr;		//Zeiger auf den "Sogfaktor"
	double* opp_ptr;     //Zeiger auf "Opplohn"
//	MatrixDouble* dkulturen_ptr;//Pointer to matrix <verfuegb_dkulturen> //substitute


	///Constructor 
	agentF(int agid, int scid, int skid, int ppid, int clid, int nwid, int fstd, /*int numP,*/ int seg, int erw, coord fstXY);

	///Destructor
	virtual ~agentF();
	virtual void deallocYieldExp();	
	//@}

//------------------------------------------------------------------------------------   	

	bool isBaselineTargetUptodate() const;
	void setBaselineTargetUptodate(bool yesno);

	protected:
	///@name Protected member data
	//@{
	///Agent ID, cluster ID, innovativeness etc.	
	int seg_nr;	   //Nummer des Betriebssegments (Innovationsfaehigkeit)
	int innov;	   //Nr. des Innovationskommunikations-Netzwerks (=Betriebsliste)
	int erwart;	   //Art der Preiserwartungsbildung
	int popID;     //population ID
	int cluID;     //cluster ID
	int fstID;     //farmstead ID in maps
	int mapColor;     //color of plots in maps (max. 56 colors)
	int agentMipCount;
//------------------------------------------------------------------------------------   

	/**
	 * Data structures for household resources
	*/
	///Total number of parcels
	int fl_anzahl;
	
	///Number of parcels per soil type
	int* plotsPerType;
	
	///Coordinates of farmstead
	coord hofflaeche;
	
	///List of farm parcels
	p_liste bflaech;     
	
	///List of livestock
	ls_list herd;        
	
	///List of all household members
	hh_list household;   
	int famAK_anzahl;
	int frAK_anzahl;

	///Water rights
	MatrixDouble wr;
	//  DIMS: (numInflows X 2)
	//  col 0: zuf    ID for inflow
	//  col 1: anz    quantity

	///Investment objects
	investdef* binvest;	//Zeiger auf das Inventar-Array
	int binv_anz;		   //Anzahl der inventarisierten Objekte
	int binv_size;		   //Speicherplatzgroesse
	MatrixDouble dkult_reg; //enum columnsInPermamentCropsRegistry, Matrix (Register) der eigenen Dauerkulturen
	segment* segPtr;     //Pointer to segment investment data
	int** sym_obj;			//Pointer to symbolic objects
	
#ifdef MULTIPERIOD
	map<int,vector<double> > assetCapacityMatrix; //note keys correspond to objIDs!!, vector has capacity expected in next years
	map<int,vector<double> > assetVintageMatrix; //note keys correspond to objIDs!!, vector has capacities by age
	//vector<double> debtServiceForThisAndNextPeriods;
	vector<double> depreciationForThisAndNextPeriods;
	vector<double> interestForThisAndNextPeriods;
	vector<double> principalForThisAndNextPeriods;
	vector<pair<int,double> > agentCropProductionAreas; //first: LuciaArrayIndex for LandUseActivity, second: agentsArea, should contain only the non-zero areas, should be sorted by first, only used with static NRU assignment
	MapOfSubindexedVectorIndexedDoubles agentCropProductionNRUAreas; //first: NRU, second: cropManagementID,  third: agentsArea, should contain only the non-zero areas, should be sorted by first, only used with user-defined NRU assignment
#endif
// Troost 20140611 Disinvestment of assets
// Troost **********begin block insert**********
	map<int,int> disinvestSlots; //records which objects of the registry have been assigned to which disinvestment slots in the matrix
// Troost **********end block insert**********
	deque<newInventoryObject> deferredInventorizing; //records with investment objects need to be inventarized later (mostly after all other assets have aged.)
//------------------------------------------------------------------------------------

	/**
	 * Data structures for income analysis
	*/

	///Income statement
	double afa;
	double aktiva;
	double arbeitseinsatz;
	double bodenentlohnung;
	double cashflow;
	double durchsDB;
	double eigenkapital;
	double einkommen;
	double entfernungskosten;//Transportkosten fuer die Eigentumsflaechen
	double entnahmen;
	double fremdkapital;
	double tilgung;
	double totalIrrigatedArea;	//monthly irrigated area, formerly betr_Wert
	double kapitaleinsatz;
	double shortTermDeposits;
	double liqMeans;
	double liqMeansPreviousPeriod;
	double liqMeansUsedForInvestment;
	double grossInvestments;
	double assetSales;
	double equityCapitalPreviousPeriod;
	double debtCapitalPreviousPeriod;
	double changeOfEquity;
	double rentalPayments;//negative value indicates payments received
	double rentalPaymentsForPerformanceData; //Troost: for saving in p-file
	double rentalPaymentsReceived;//Troost: for voluntary exit decision: received payments remain also when giving up farming
	double relEntl;
	double wertschoepfung;
	double zinskosten;
	double zusaetzlicheTK;//durch Pachtaktivitaeten entstandene Transportkosten

	///Income statement for advanced consumption model
	double inCashEarn; //Net cash earnings from MILP
	double inKindEarn; //Net in-kind earnings from MILP
	double apprAssets; //Appreciation of durable assets
	double savingOfHH; //Savings of household
	double extraConHH; //Extra consumption

//------------------------------------------------------------------------------------   

	/**
	 * Data structures for crop-water module
	*/
	///Data structures for CropWat module
	MatrixDouble cropWaterData;// using <enumYieldInfo>

	///Water deficits for selected water activities
	MatrixDouble waterDeficitsOfCropsAsMonthlyKR;//Tbe 080220 selected crops, irrigated and rainfed

	///Monthly water demands[absolute value]
	MatrixDouble irrigationWaterDemandsPerPriorityGroup;//Tbe 080220

	///Percentage of irrigation water demand that was served
	MatrixDouble percentageIrrigatedPerPriorityGroup; //Tbe 080220

	// Actual irrigation water
	MatrixDouble irrigationWaterSuppliedToCrops;//Tbe 080220

	double etaOfAgent;
	double totalIrrigationWaterApplied; // total irrigation water in current month

	// Making code spatially explicit: LP output data structure (crop mix)
	crpMixClass* cropMixAgent;

//------------------------------------------------------------------------------------

	/**
	 * Data structures for irrigation module (monthly values)
	*/
	///irrigation water demand
	double* D_i;            
	///irrigation water demand actually met
	double* D_sat;          
	// expected net  water supply
	double* erw_N_i;     
	///actual net water supply
	double* N_i;            
	///expected water supply from return flows (additional to net)
	double* erw_Ext_i;      

	///actual water supply from return flows (additional to net)
	double* Ext_i;
	///irrigation water from rental contracts
	double* WPa_i;
	/// Maximum "initial" water supply, including share that this agent might not fully use
	double* S_i;//renamed from <Initial_i>
	/// agent share of "left-over water"
	double* L_i;

	///rented water rights
	vector <waterLinkedToPlots> waterRentedIn;
	vector <waterLinkedToPlots>::iterator iterWaterRentedIn;
	
	///yield expectations
	MatrixDouble expCropWaterYields;

	///actual yields
	MatrixDouble actCropWaterYields;

	///expected irrigation water demands
	MatrixDouble expCropWaterIrrDmds;

//------------------------------------------------------------------------------------

	/**
	 * Data structures for TSPC and Lucia crop growth modules
	*/
	///expected yields per NRU
#ifdef MULTIPERIOD
	caYldExp** expYlds;
#else
	caYld** expYlds;
#endif
	///actual yields per NRU
	caYld** actYlds;

#ifdef MULTIPERIOD
	yieldHistory agentYieldHistory;
	expectationParameters advancedExpectationsParams;
#endif
//------------------------------------------------------------------------------------

	/**
	 * Data structures for LP module
	*/
	double* erw_preise;			//expected prices, used to initialize objective fucntion coefficients
	double* varsRHS;				//the agent's resources (Right Hand Side)
	double* varsLHS;				//used capacities of resources (Left Hand Side)
	double* schattenpreise;		//shadow prices
	double* alte_spreise;		//old shadow prices
	double baselineTarget;		//Milp target value of baseline ("without")
	bool baselineTargetUptodate;
	MatrixDouble ford_bak;		//matrix to save minimal asking prices
	int lock;						//blocks more investment in case of cash shortage
	int migr;						//migration decision -1 illiquidity -2 voluntary
										//-3 deceased -4 no labor force -5 no adult labor force
										// -6 - household head left or died and no successor has been found
										// -7 - household head retired and no successor has been found
										// -8 - household head left or died and succession failed for economic reasons
										// -9 - household head retired and succession failed for economic reasons
										// -10 - household head retired, no successor was found, and household preferred renting out over continuing
										// -11 sold farmstead to regain liquidity
	int solvedLP;					//forced solution (default 0, inv 1, prd 2, con 3)	
	
	double lastTgtPrd;
	double lastTgtMkt;
	double lastTgtCons;
	double lastTgtInv;
	double lastTgtInvLin;
	///Information for "forced solution" (case that LP does not solve)
	MatrixDouble mEntries;		//matrix with information for "forced" solution

	l_vektor bprod;//Loesungsvektor (Umfaenge, Zielfunktionskoeff., Zielwert)
#ifdef MULTIPERIOD
	double* umfLastActual; //if there are several deicision stages in multiperiod the last true production solution needs to be stored, as it is not the last one solved
#endif

// Troost 20170310 Cplex Mip starts
// Troost **********begin block insert**********
#ifdef MIPSTART
	double* lastProdSolutionForWarmStart;
	double* lastInvSolutionForWarmStart;
	double* lastMarketSolutionForWarmStart;
// Troost **********end block insert**********
#endif //MIPSTART
	///Regional market access
	int marketID;

	///number of matrix elements to be changed in consumption mode
	int numChangedElems;   
	///array containing LP row indices
	int* idxRow;   
	///array containing LP column indices
	int* idxCol;   
	///array containing LP MatrixDouble elements
	double* matEls;

	///random variable for logit model
	double logitRandom;

	///time preference for market sales
	double timePreferenceSales;

	//}@

#ifdef RNGSTREAM
	farmRandCollection farmRandomNumbers;
#endif

// Troost 20170516 Advanced Demography Model 2
// Troost **********begin block insert**********
	int numberDescendentFarms;
// Troost **********end block insert**********

#ifdef LIVSIM_COUPLING
	LivSimHerdCollection livsimHerdCollection;
	map<int,int> linkLvsimIdToAssetRegistryRow;
	int animalIdCount;
	double totalLivSimMilkProduction;
#endif
#ifdef MULTIPERIOD
	double*** extraCropActivityCharacteristics;
	vector<double> extraAgentCharacteristics;

// Troost 201801207 Common property
// Troost **********begin block insert**********
	vector<int> commonPropertyGroupMemberships;
// Troost **********end block insert**********

#endif

//------------------------------------------------------------------------------------
};


/* TODO 5 -oBerger -cImplement : this is for later, with more time */
/////Auxiliary data structure for registry of agent's permanent crops
//class PermanentCropsOfAgent 
//{	//auxiliary class, everything public
//public:
//
//	double investmentObjectID;			//formerly 17 
//	double purchasePrice;				//formerly 0  
//
//	double currentCropYield;			//formerly 15
//	double currentVariableCosts;		//formerly 16
//	double currentPreHarvestCosts;	//formerly 3
//
//	double futureCropYield;				//formerly 1
//	double futureVariableCosts;		//formerly 2
//	double futurePreHarvestCosts;		//formerly 9
//
//	//current labor demands
//	MatrixDouble currentLaborDemands;	//formerly 4, 7, 8
//
//	//future (average) labor demands
//	MatrixDouble futureLaborDemands;		//formerly 10, 13, 14
//
//	//current machinery demands
//	MatrixDouble currentMachineryDemands;//formerly 5, 6
//
//	//future (average) machinery demands
//	MatrixDouble futureMachineryDemands;//formerly 11, 12
//
//	///constructor
//	PermanentCropsOfAgent()
//	{	investmentObjectID - 1;		
//		purchasePrice = 0.0;			
//
//		currentCropYield = 0.0;		
//		currentVariableCosts = 0.0;	
//		currentPreHarvestCosts = 0.0;
//
//		futureCropYield = 0.0;			
//		futureVariableCosts = 0.0;	
//		futurePreHarvestCosts = 0.0;	
//	}
//
//	///destructor
//	virtual ~PermanentCropsOfAgent()
//	{	currentLaborDemands.deallocate();
//		futureLaborDemands.deallocate();
//		currentMachineryDemands.deallocate();
//		futureMachineryDemands.deallocate();
//	}
//};
//*/

#endif

