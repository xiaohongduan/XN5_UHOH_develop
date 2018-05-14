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
//  File:       PopulationClusterList.h (formerly cliste.h)
//
//  Contents:   Manages linked list of agent clusters
//
//
//
//------------------------------------------------------------------------------



#ifndef _PopulationClusterList_h
#define _PopulationClusterList_h

#include "BasicDefines.h"
#include "PopulationCluster.h"
#include "CropSoil.h"



typedef struct
{  int carID;     //career ID
   int lfSpan;    //lifespan
	MatrixDouble labGrp; //labor group ID
   MatrixDouble labCap; //labor capacity over lifespan
   MatrixDouble probM;  //probability of dying at end of year
   MatrixDouble probF;  //probability of giving birth at end of year
   MatrixDouble ntrReq; //nutrient requirements
   MatrixDouble userDef;// user-defined age-specific charateristics

// Troost 20121208 - Advanced demography model
// Troost **********begin block insert**********
   int sex;
   int AgeOfCertainDeath;
   int AgeOfObligatoryRetirement;
   //additional age-specific values
	MatrixDouble probMarriage; //probability of marrying
	MatrixDouble probLeaving; //probability of leaving the household
	MatrixDouble probRetiring; //probability of retiring
	MatrixDouble priorityHHead; //priority of becoming household head
	MatrixDouble minCons; //minimum consumption
	// Troost 20140630 Voluntary exit decision in Investment LP
	// Troost **********begin block insert**********
	MatrixDouble oppWage;
	// Troost **********end block insert**********
	//probabilities for careers of descendants and career switching
	MatrixDouble probCareerDescendants;
	MatrixDouble probCareerMarriage;
	MatrixDouble probCareerPartner;
	MatrixDouble probCareerHHead;
	MatrixDouble probCareerPartnerHHead;
	MatrixDouble probCareerRetirement;
// Troost **********end block insert**********


// Troost 20170516 Advanced Demography Model 2
// Troost **********begin block insert**********
	MatrixDouble probCareerGivingBirth;
	MatrixDouble probCareerSwitch;
	int numCareersToSwitchTo;
	MatrixDouble careersSwitchedTo;

	int leaveReqBoth;
	int marryReqBoth;
	int retireReqBoth;
	int giveBirthReqBoth;
	int switchReqBoth;
	int dieReqBoth;

	int leavingWithPartner;
	MatrixDouble leavingWithParent;

	//int formNewHouseholdWhenLeaving;
	int switchHeadToPartnerIfMarried;

// Troost **********end block insert**********

}  career;

// Troost 20121208 - Advanced demography model
// Troost **********begin block insert**********
struct hhLifetimeConstraint
{
		int lpRow;
		int lBound;
		double rhsValue;
};

#define NUMREASONSSUCCESSION 3
struct successionContr
{
		//control variables for household head succession defined, which are common to the three cases
		//that may trigger succession are declared a arrays.
		//Column	Trigger
		//0 - Death of old household head
		//1 - voluntary (optional) retirement of household head
		//2 - obligatory retirement of household head
		//see BasicEnums.h

		double relMinCons[NUMREASONSSUCCESSION]; // relative
		double minInc[NUMREASONSSUCCESSION];
		int lpColControlActivity[NUMREASONSSUCCESSION];
		double minRental;

		double probThresholdHouseholdLifetime;
		int invHorizonModel;
		int invHorizonWhoIsIn;
		int invHorizonAffectsInvestmentLifetime;
		int numHHLifetimeConstraints;
		hhLifetimeConstraint* hhLifetimeConstraints;

// Troost 20130109 - Succession model part 2
// Troost **********begin block insert**********
		int fixCostR;
		int incBalR;
		int minConsR;
		int extraIncRow;
// Troost **********end block insert**********

// Troost 20140630 Voluntary exit decision in Investment LP
// Troost **********begin block insert**********
		int actVoluntaryExit;
		int firstRowOwnedPlots;
		int firstActOwnedPlots;



// Troost **********end block insert**********

		//constructor
		successionContr();
		//destructor
		~successionContr();
};

// Troost **********end block insert**********

// Troost 20140605 Mark household head for young farmer subsidy
// Troost **********begin block insert**********
struct userMarkHHmembers
{
		int whomToMark;
		int lpColMark;
		int lpColUnmark;
};
// Troost **********end block insert**********

class c_liste
{	protected:
   int pop_nr;          //Populationsnummer
   int canzahl;         //Anzahl Cluster
   int letzte_c_nr;		//zuletzt vergebene Clusternummer
   int num_a;           //Anzahl aller Agenten

   int numCar;          //number of careers
   int numLab;          //number of labor groups
   int numUserDefinedCharacteristics;
	int schoolAge;
	int adultAge;

   career* careers;     //pointer to careers

   cluster* firstCluster;
   cluster* merkeCluster;//wird fuer suche_naechsten() gebraucht

   void returnFscan(int retVal, char* strVar);

// Troost 20121208 - Advanced demography model
// Troost **********begin block insert**********
	successionContr successionControl;
// Troost **********end block insert**********

// Troost 20140605 Mark household head for young farmer subsidy
// Troost **********begin block insert**********
	int numUserMarks;
	userMarkHHmembers* userMarks;
// Troost **********end block insert**********

// Troost 20170516 Advanced Demography Model 2
// Troost **********begin block insert**********
	int newFstID_shifter;
	int newFstID_multiplier;
// Troost **********end block insert**********

   public:
   c_liste(int);
   virtual ~c_liste()
   {
// Troost 20140605 Mark household head for young farmer subsidy
// Troost **********begin block insert**********
		delete [] userMarks;
		userMarks = NULL;
// Troost **********end block insert**********
   	deallocateCareers();
      liste_loeschen();
   }

   virtual void readInputFile(char* filename);
   virtual void writeInputIntoFile(char* filename);

   virtual int getLabGrps();
   virtual double getYearLabCap(int car, int age);
   virtual double getYearLabGrp(int car, int age);
   virtual void addActLabCap(MatrixDouble& mtx, int car, int age);
   virtual void addExpLabCap(MatrixDouble& mtx, int car, int age, double = 0.0);
   virtual void addActNtrReq(MatrixDouble& mtx, int car, int age);
   virtual void addExpNtrReq(MatrixDouble& mtx, int car, int age, double = 0.0);

   int getLifespanOfCareer(int car);
#ifdef RNGSTREAM
   virtual int hasDeceased(int car, int age, double draw);
   virtual int givesBirth(int car, int age, double draw);
#else
   virtual int hasDeceased(int car, int age);
   virtual int givesBirth(int car, int age);

#endif
   virtual int defCareer(int sex);//return ID of default career
	virtual int getSchoolAge();
	virtual int getAdultAge();

#ifdef MULTIPERIOD
	void multiperiod_addVars(vector<vector<double> >* mtx, vector<double>* minConsMtx,vector<vector<double> >* userDefMtx, int car, int age, int numPeriods);
#endif
   virtual void deallocateCareers();
// Troost 20121208 - Advanced demography model
// Troost **********begin block insert**********
	virtual int getArrayIndexOfCareerID(int car);
	virtual int getAgeOfObligatoryRetirement (int car);
	virtual int getAgeOfCertainDeath (int car);
	virtual double getRelMinConsSuccession ( int reason);
	virtual double getMinIncSuccession ( int reason);
	virtual double getMinRentalPaySuccession ();
	virtual int getLPActivityIndexSuccession ( int reason);

	virtual int getInvHorizonModel ();
	virtual int getInvHorizonWhoIsIn ();
	virtual int invHorizonAffectsInvestmentLifetime();
	virtual double getProbThresholdHouseholdLifetime ();
	virtual int getAgeOfHHLifetimeThresholdPassed (int car);
	virtual int numConstraintsForRemainingLifetimeOfHH ();
	virtual int getArrayIndexForRemainingLifetimeConstraint(int rLife);
	virtual int getLPRowForRemainingLifetimeConstraint(int idx);
	virtual double getRHSValueForRemainingLifetimeConstraint(int idx);
	virtual int sexOfCareer(int car);


#ifdef RNGSTREAM
	virtual int findsPartner(int car, int age, double draw);
	virtual int leavesHH(int car, int age, double draw);
	virtual int wantsToRetire(int car, int age, double draw);
	// Troost 20170516 Advanced Demography Model 2
	// Troost **********begin block insert**********
	virtual int switchesCareer(int car, int age, double draw);
		// Troost **********end block insert**********


	virtual int careerEventOccurs(int car, int age, int type, double draw);
	virtual int selectNewCareer(int car, int reason, double draw);
#else
	virtual int careerEventOccurs(int car, int age, int type);
	virtual int selectNewCareer(int car, int reason);


	virtual int findsPartner(int car, int age);
	virtual int leavesHH(int car, int age);
	virtual int wantsToRetire(int car, int age);
	// Troost 20170516 Advanced Demography Model 2
	// Troost **********begin block insert**********
	virtual int switchesCareer(int car, int age);
	// Troost **********end block insert**********
#endif
	virtual double priorityForHHead(int car, int age);
	virtual double getMinConsMember( int car, int age);
// Troost 20140630 Voluntary exit decision in Investment LP
// Troost **********begin block insert**********
	virtual double getOppWageMember(int car, int age);
// Troost **********end block insert**********
	virtual double getUserDefinedCharacteristic(int which, int car, int age);
	int getNumUserDefinedCharacteristics();
	int getNutrientRequirement(int which, int car, int age);

#ifdef RNGSTREAM
	virtual int careerOfDescendant(int car, double draw);
	virtual int careerOfPartner(int car, double draw);
	virtual int careerAfterMarriage(int car, double draw);
	virtual int careerAfterRetirement(int car, double draw);
	virtual int careerIfHHead(int car, double draw);
	virtual int careerIfPartnerOfHHead(int car, double draw);
	virtual int careerAfterGivingBirth(int car, double draw);
	virtual int careerAfterSwitch(int car, double draw);

#else
	virtual int careerOfDescendant(int car);
	virtual int careerOfPartner(int car);
	virtual int careerAfterMarriage(int car);
	virtual int careerAfterRetirement(int car);
	virtual int careerIfHHead(int car);
	virtual int careerIfPartnerOfHHead(int car);
	int careerAsNewMother(int car);
#endif //RNGSTREAM
// Troost **********end block insert**********

// Troost 20170516 Advanced Demography Model 2
// Troost **********begin block insert**********
	int leavesWithPartner(int car);
	//int formNewHouseholdWhenLeaving(int car);
	double leavingWithMother(int car, int age);
	int switchHeadToPartnerIfMarried(int car);
	int getLeavingReqBoth(int car);
	int getMarryingReqBoth(int car);
	int getRetiringReqBoth(int car);
	int getGivingBirthReqBoth(int car);
	int getSwitchingReqBoth(int car);
	int getDyingReqBoth(int car);
// Troost **********end block insert**********



// Troost 20130109 - Succession model part 2
// Troost **********begin block insert**********
	virtual int getFixCostR();
	virtual int getIncBalR();
	virtual int getMinConsR();
	virtual bool useExtraIncRow();
// Troost **********end block insert**********

// Troost 20140630 Voluntary exit decision in Investment LP
// Troost **********begin block insert**********
	virtual int getActVoluntaryExit();
	virtual int getFirstRowOwnedPlots();
	virtual int getFirstActOwnedPlot();
// Troost **********end block insert**********

// Troost 20140605 Mark household head for young farmer subsidy
// Troost **********begin block insert**********
	virtual int getNumUserMarks();
	virtual int getWhomToMark(int idx);
	virtual int getLPColMark(int idx);
	virtual int getLPColUnmark(int idx);
// Troost **********end block insert**********

   //returns fix costs per farm
   virtual double getFarmFixCosts(int cluID, double totHa, double irrHa);

   virtual void cluster_anlegen(char*);//Name der Input-Datei
   virtual void ccluster_anlegen(char*);//neu 2004: Name der Input-Datei
	virtual void deallokieren(cluster*);//deallokiert ein Cluster
	virtual void einfuegen(cluster*);//fuegt ein Cluster ein
 	virtual int get_anzahl(void);//liefert Anzahl der Cluster
   virtual int get_nummer(void);//liefert die zuletzt vergebene Clusternummer
   virtual cluster* cluster_suchen(int);//uebergeben wird c_nummer;
   virtual cluster* getFirst();//liefert Listenersten
   virtual int getTotalNumAgents(void);

	virtual void lotto_deallokieren();//gibt nicht benoetigten Speicher frei
   virtual void writeRandomAssignInFiles();
	virtual void printToScreen(cluster*);
   virtual void printOnScreen_list(void);
	virtual void liste_loeschen(void);

	int getNewFstIdMultiplier() { return newFstID_multiplier;}
	int getNewFstIdShifter() { return newFstID_shifter;}

#ifdef NEWFORMAT14
	double sunkcost;//Anteil der wiedergewinnbaren Kosten
   double sog;		//"Sogfaktor" zur Pruefung der freiwilligen Betriebsaufgabe
   double opplohn;//Opportunity wage of household head
#endif
};

#endif

