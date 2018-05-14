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
//  File:       PopulationNetworkSegment.h (formerly segment.h)
//
//  Contents:   Network segment class, manages innovation data and provides
//              investment related coefficients to compute, for example annuity
//              investment costs
//
//------------------------------------------------------------------------------


#ifndef _PopulationNetworkSegment_h
#define _PopulationNetworkSegment_h


#include "MatrixDouble.h"
#include "BasicDefines.h"
#include <vector>
#include <deque>

using namespace std;

///Auxiliary data structure for registry of available permanent crops
class AvailablePermanentCrops
{	//auxiliary class, everything public
public:

	double purchasePrice;			//0
	int accessible;					//1 (bool)
   double annuityYields;			//2 periodisierte Werte
   double investmentCosts;			//3 periodisierte jaehrl. Kosten (Spezialkosten + Kapitaldienst + Abschreibung)
   double cashDemandYear0;			//4 Kapitalansprueche im Jahr0 (Eigenfinanzierungsanteil)
   double cashDemandAverage;		//5 periodisierte Kapitalansprueche (durchschnittliches Jahr)
	int lpColumnInvestment;			//7 Spaltennr. in LP (Investitionsplanungsaktivitaet)
	int lpYieldRow;					//8 entsprechende Ertragszeilennr. in LP
   int soilSuitability;				//9 fuer alle Bodentypen geeignet: 0; nur BodenI/II: 1; nur IV/IVa: 2
   double minimumArea;				//10 erforderliche Mindestbodenausstattung (in ha)
   int objectID;						//11 Objektcode
	double financeDemand;
	int availableUntil;
	double cashDemandEndOfYear;	// ## Tbe 121222
	
	//average labor demands
	MatrixDouble averageLaborDemands;		//12 
	
	//average machinery demands
	MatrixDouble averageMachineryDemands;	//15 
	
	///constructor
	AvailablePermanentCrops()
	{	purchasePrice = 0.0;			
		accessible = 0;				
		annuityYields = 0.0;			
		investmentCosts = 0.0;		
		cashDemandYear0 = 0.0;		
		cashDemandAverage = 0.0;	
		lpColumnInvestment = -1;	
		lpYieldRow = -1;				
		soilSuitability = -1;		
		minimumArea = 0.0;			
		objectID = -1;	
		financeDemand = 0.0;
		availableUntil = 0;
		cashDemandEndOfYear = 0.0;
	}

	///destructor
	virtual ~AvailablePermanentCrops()
	{	averageLaborDemands.deallocate();
		averageMachineryDemands.deallocate();
	}
};


struct farmRandCollection; //forward dec
struct newInventoryObject;//forward dec

//properties of an investment object
struct investmentObject
{  char name[OBJNAME];	//Investitionsobjektname
	int innovationID;
   int code;				//Objekt-ID
	int typ;					//Dauerkultur, Maschine, Stallplaetze
   int divisible;			//teilbares Objekt (ja/nein)
   double purPrice;
	int lifeSp;
   int boden;				//Bodentyp
   double mindest;		//erforderlicher Mindestinvestitonsumfang (Anz. Aggregate)
	int activity;			//welche Aktivitaet? (Spaltennr. in temp_LP)
	int capacity;			//Wirkung auf welche Kapazitaet, z.B. Nr. 19 Milchviehplaetze
   int yieldRow;			//Ertragszeile
	double size;			//Leistung des Objekts, z.B. 100 Stallplaetze oder 283 AKh
   int seg;					//Innovationsfaehigkeit bzw. Segment
   double shareEC;		//Eigenfinanzierungsanteil
   double rateDC;			//Fremdkapitalzins
	int availableUntil;
// 121206 Troost correct price for old assets
// ********* begin insert ******************
	double avgInflationRatePast; // to take account of the fact, that investment objects available at the beginning have been bought at lower prices then those currently observed
	int minAgeAtSimulationStart;
	int maxAgeAtSimulationStart;
// ********* end insert ********************
// Troost 20140611 Disinvestment of assets
// Troost **********begin block insert**********
	double shareResaleValue;
	double shareBookValueConsidered;
	int firstDisinvestActivity;
	int numDisinvestActivities;
	int disinvestCompletely;
// Troost **********end block insert**********
#ifdef MULTIPERIOD
	int internalAccounting;
#endif //MULTIPERIOD
// Troost 20150218 MEKA more realistic
// Troost **********begin block insert**********
	double resaleTopup;
	double reducedResaleTopupPerYearOfAge;
	int	 zombieObject;
	int	 zombieObjectWhenDisinvest;
// Troost **********end block insert**********
// Troost 20141210 Land markets - type 4, assets traded with land
// Troost **********begin block insert**********
	int plotCapacitySlot;
	double plotCapacityUsed;
	int canBeAttachedToRentedPlot;
	double compensationIfPlotReturnedToOwner;
	int objType;
	double landRequirement;

// Troost **********end block insert**********
#ifdef MULTIPERIOD
	vector<int> offspringAtAge; //Note: This is the maximum number offspring at each age. Combined with offspring type probabilities
										 //that sum up to less than one, you can make the actual number of offspring random as well.
										// It will try X times to generate offspring, but probability sums < 1 may mean that for some tries no offspring is actually generated
	vector<int> offspringTypes;
	vector<double> offspringCumulativeProbabilities;

	investmentObject() ;
	bool mayHaveOffspring() const;

	void addOffspringAtAge(int age, int quantity);
	void addOffspringType(int childID, double probability);
	void generateOffspring(int age, deque<newInventoryObject> & dequeOfObjIdsAndQuantitiesToAppendTo,  farmRandCollection& farmRandomNumbers ) const;

#endif
#ifdef LIVSIM_COUPLING
	bool isLivSimLivestock;
#endif
} ; //refactoring from <investart>


//properties of an innovation
typedef struct
{	int innovationID;
	int startAvailable;	//period of introduction (immediately available: 0)
   int accessible;		//accessible to agents in segment (yes/no)
   double exposure;
} innovation; //refactoring from <investart>



// Def. eines Investitionsobjektes ueber seinen Anschaffungspreis
typedef struct investdef
{  char acc[3];	//not yet accounted for 'x', outdated 'o', free slot in
						//inventory '-', otherwise ' '
						//141212 Troost: 'r' - rented in, 'l' - rented out (Brit: "to let")
	int code;      //Objektcode
   double anz;		//Anzahl der Aggregate (z.B. 32.5 ha Obstbau)
	double ask;		//Anschaffungspreis bzw. -kosten (je ha Obstbau)
	int alter;		//Alter
	double shareEC;//share equity capital
   double rateDC; //interest rate debt capital
// Troost 20141210 Land markets - type 4, assets traded with land
// Troost **********begin block insert**********
	int plotX;
	int plotY;
// Troost **********end block insert**********

	int livsimId;

} investdef;



class segment
{  protected:
	int sgID;			//segment ID
	int nwID;         //network ID
	double schw_wert;	//Schwellenwert
	double* invBounds;//upper bounds for investments
	double* invShadowPrices;//proxy for shadow prices

	const investmentObject* investmentObjectsInNetwork;//substitute for <invest_matrix> ##
	int numInvestmentObjects;
	innovation* innovationsInSegment;//substitute for <invest_matrix> ##
	int numberOfInnovations;

	AvailablePermanentCrops* registryOfAvailablePermanentCrops;//substitute for <verfuegb_dkulturen>
	int numberOfAvailablePermanentCrops;

	char filename[MXLENGTH];//Inputfile
	FILE* datenstrom;	//Datenstrom

	void returnFscan(int retVal, char* strVar);

	public:
	segment(int sg, int nw, char* in);
	virtual ~segment()
	{  //verfuegb_dkulturen.deallokieren(); ##
   	delete [] registryOfAvailablePermanentCrops;
	
		//delete [] invest_matrix; ##
		delete [] innovationsInSegment;

      delete [] invBounds;
		delete [] invShadowPrices;
	}

   segment* nextSegment;
   virtual void setNextPtr (segment*);
   virtual segment* getNext();

	double rateLC;	//Zinsfuss fuer langfristige Kredite
   double rateSC;	//Zinsfuss fuer kurzfristge Kredite
   double rateSD;	//Zinsfuss fuer einjaehrige Geldanlagen
	double shareEC;//Eigenfinanzierungsanteil
   double ikalk;	//Kalkulationszins
	double bonus;	//direkte Einkommenstransfers
#ifndef NEWFORMAT14
	double sunkcost;//Anteil der wiedergewinnbaren Kosten
   double sog;		//"Sogfaktor" zur Pruefung der freiwilligen Betriebsaufgabe
   double opplohn;//Opportunity wage of household head
#else

#endif
	virtual double get_sw();//liefert Schwellenwert zurueck
   virtual int get_sgID();
   virtual int get_nwID();
   virtual int get_objnummer(int objID);//liefert Nr. eines Inv.objekts
	virtual int get_n_dauer(int objID);//liefert Nutz.dauer eines Inv.objekts
	virtual int getObjectIdForLpActivity(int lpActivity);//returns -1 if no object found
   virtual int getIDforOutput(double leistg);//Uebergabe Leistung
	virtual int getIDforPurchasePrice(double pp);
	virtual int getIDforObjectType(int objType);
   virtual double getPPforID(int objID);//liefert ASK, Uebergabe ObjCode
   virtual double getSFforID(int objID);//liefert Eigenfinanzierungsanteil
   virtual double getILCforID(int objID);//liefert Fremdkapitalzins
   virtual int getLPRowforID(int objID);
   virtual const char* getNameForID(int objID);//liefert Name, Uebergabe ObjCode
	virtual double getInvBound(int soilType);//upper bound for investments
	virtual double getInvShadowPrices(int soilType);//proxy shadow price for investment
	
	virtual int getNumberOfAvailablePermanentCrops();
	virtual int getIndexOfInnovation(int innovationID);
	virtual int getAccessibilityOfInnovation(int innovationID);
   virtual void updateAccessibilityOfInnovation //refactored from <wert_updaten>
      (int period, double sw, int idxInnovation, int typeCommunication);

   virtual void readSegmentInput();//baut Investmatrix + Lotterie mit Inputdaten auf
   virtual void writeSegmentInput(char*);

	virtual void createRegistryOfAvailablePermanentCrops();//substitute for <dkulturen_anlegen>
	virtual void updateProductionCostsInRegistryOfAvailablePermanentCrops();//changing production costs

	virtual void writeRegistryOfAvailablePermanentCropsToFile(char*);
	virtual void writeRegistryOfAvailablePermanentCropsToStream(FILE* stream);
	virtual void updateProductionCostsOfAvailablePermanentCrops();
	virtual AvailablePermanentCrops* getPointerToRegistryOfAvailablePermanentCrops();
	
	virtual void updateAccessToPermanentCrop(int idxInnovation);//substitute for <dkulturen_updaten>

   virtual void writeAccessibityStatusToFile(FILE* strm, int period);
	virtual void printToScreen();
   //virtual void lotto_datei_schreiben(char* filename);//neu 2004 in 'cluster'
};


#endif

