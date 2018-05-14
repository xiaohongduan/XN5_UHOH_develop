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
//  File:       AgentParcelList.h (formerly pliste.h)
//
//  Contents:   Manages linked list of all parcels held by one agent
//
//
//
//------------------------------------------------------------------------------


#ifndef _AgentParcelList_h
#define _AgentParcelList_h



//#include "AgentParcel.h"
#include "MarketNonTradablesBid.h"
//#include "GridCellHandler.h"

#include "MpmasPlotCell.h"

// Troost 20140527 Generalize dynamic NRUs
// Troost **********begin block insert**********
#include <map>
#include <set>
#include <vector>
// Troost **********end block insert**********


class crpMixClass;
class agentF;
class agent;
struct assetRequiredForCrop;

struct  parcelGroupIdSorter
{
		bool operator() ( const parcelGroup* lhs,  const parcelGroup* rhs);

};

class p_liste
{	protected:

	agent* ownerPtr;

	int panzahl;
//	int letzte_p_nr;
	int numberPlotsRentedOut;

	int*    MainTypeInNRU;  
	int**   NumPlotsInNRU;  
	double* avgSlNRU;       
	double* avgSeNRU;


// Troost 20140527 Generalize dynamic NRUs
// Troost **********begin block insert**********
	multimap<int, mpmasPlotCell*> cellsInNRU;
	multimap<int, int> soilTypesInNRU;
// Troost **********end block insert**********

	vector<vector<double> > plotsInNRUsOverTime;//dim 1 next years, dim 2 NRUs

	mpmasPlotCell* firstPlot;
	mpmasPlotCell* lastPlot;
	mpmasPlotCell* rememberPlot;

	virtual void initialize(void);

	set<parcelGroup*,parcelGroupIdSorter> parcelGroupList;

	public:
	//p_liste(int, int, int, int);
	//p_liste();
	p_liste(agent*);

	virtual ~p_liste()
	{  liste_loeschen();
	}

	//int sk_nr;  //Sektor-Nummer //Now all taken from ownerPtr
	//int sc_nr;	//Subcatchment-Nummer
	//int b_nr;	//Betriebsnummer
	//int fstID; //farmsteadID

	int getAgentId();
	int getAgentMapId();
	int getSectorId();
	int getCatchmentId();

	//Uebergabe: x, y, Bodentyp, Betriebsnr., Sc-, Sek-nr., Zustand, Entf.
	//virtual void create_plot(int,int,int,int,int,int, int = 1,double = 0.0, parcelGroup* pg = NULL);
	virtual void insertPlotAsNewOwnerAndUser(mpmasPlotCell* pp);
	virtual void insertPlotRentedIn(	mpmasPlotCell* pp );
	virtual void insertPlotReturned(	mpmasPlotCell* pp );
	virtual void insertPlotIntoPlotList(mpmasPlotCell*);
	//virtual void deallokieren(mpmasPlotCell*);//deallokiert eine Parzelle//Troost 160111: obsolete after merging plots and cells, plot inventory now kept by landscape

	virtual int checkNumberOfAllUsablePlots();//liefert Anzahl aller Parzellen, renamed from <get_azahl>
	virtual int checkSoilTypeOfFarmstead();
	virtual int checkNumberOfPlotsRentedIn();
	virtual int checkNumberOfPlotsRentedIn(int soilType);
	virtual int checkNumberOfPlotsWithPermanentCrops(int soilType);
	virtual int checkNumberOfPlotsRentedOut();
	virtual int getNumPlotsInSoilType(int soilType, int withPlotsRentedIn);//refactored from <get_anz_typ>
	//virtual int get_nummer();//liefert zuletzt vergebene Parzellennummer
	virtual int get_hofx();//x-Koordinate der Hofstelle
	virtual int get_hofy();//y-Koordinate der Hofstelle

	virtual mpmasPlotCell* search_plot(int z, int s);
	virtual mpmasPlotCell* erste_suchen(int);//uebergeben wird der Bodentyp
	virtual mpmasPlotCell* naechste_suchen(int);//uebergeben wird der Bodentyp
	virtual mpmasPlotCell* letzte_suchen(int);//uebergeben wird der Bodentyp
	virtual mpmasPlotCell* vorherige_suchen(int);//uebergeben wird der Bodentyp
	//virtual mpmasPlotCell* fernste_belegt(int);//uebergeben wird Bodenklasse ## Tbe 110921 not in use
	virtual mpmasPlotCell* getFirst();//liefert Listenerste
	virtual mpmasPlotCell* getLast();//liefert Listenletzte

	virtual mpmasPlotCell* findLastPlotWithPermanentCrops(int soilType);
	virtual mpmasPlotCell* findFirstPlotWithPermanentCropsAndRemember(int soilType);
	virtual mpmasPlotCell* findNextFirstPlotWithPermanentCropsAndRemember(int soilType);

	int getNRUofFarmstead();

	//die folgenden Funktionen belegen bzw. geben Parzellen mit Dauerkulturen frei. 
	//Uebergabe: jeweilige Gesamt-Dauerkulturflaeche, zu veraendernde Dauerkulturflaeche
	virtual int blockHectaresInPlotsForPermanentCrops(int soilType, double existingHa, double deltaHa, string outputStr);
	virtual double blockPlots(int soilType, double sizeHa);
	virtual int freeHectaresInPlotsForPermanentCrops(int soilType, double totalHa, double reductionHa);//not in use anymore

	//uebergeben wird bei den folgenden Elementfunktionen der Bodentyp
	virtual mpmasPlotCell* verpachtbare_suchen(int typ);//sucht die hoffernste Parzelle
	virtual double computeTransportCostsOfRemotestFreePlot(int typ);//refactored from <get_maxTK>

	virtual mpmasPlotCell* remove_plot_from_list(mpmasPlotCell*);
	
	///selling parcels to landowner agent
	virtual double transferThisPlotToAgentL(mpmasPlotCell* p, w_rechte* w, double zins);
	virtual double sell_free_plot(int, w_rechte*, double);
	virtual double sell_blocked_plot(int, w_rechte*, double); //## soil
	virtual double sell_farmstead_plot(w_rechte*, double);

	///transferring parcels to other farm agent
	virtual double transferThisPlotToAgentO(mpmasPlotCell* p, w_rechte* w, double zins);// ## Tbe 130722
	virtual double transferEmptyPlotToAgentO(int, w_rechte*, double);
	virtual double transferBlockedPlotToAgentO(int, w_rechte*, double); //## soil
	virtual double transferFarmsteadToAgentO(w_rechte*, double);



	//bildet ein Pachtgebot und leitet es an den Sektor-Pachtmarkt weiter
	//Uebergabe: Sektornr, Typ, zu transferierende monatliche Wasservolumina bzw.
	//Uebergabe: Sektornr, Typ, Maximalentfernung
	virtual void verpachten( int typ, w_jahr* vol, w_rechte* w);
	virtual void zupachten( int typ, double entf, w_jahr* vol, w_rechte* w);
	virtual void alles_verpachten( w_jahr* vol, w_rechte* w);//verpachtet alle Parzellen
	virtual void transferPlotAsRentedOut(int mapRow, int mapCol, w_rechte* w, int years, double pmt, int newAgID, int newScID);//refactored from <verpachtet_belegen>
	virtual void transferPlotRentedInBackToOwner(mpmasPlotCell* p, w_rechte* w);
	virtual double arrangePaymentsOrReturnPlotsRentedIn(double availableCash, w_rechte* w);//refactored from <verpachtungen_aufloesen>
	virtual void decreaseCounterOfPlotsRentedOut();
	virtual double entfernungskosten_aktualisieren();

	// Troost 20170516 Advanced Demography Model 2
	// Troost **********begin block insert**********
	virtual double transferThisPlotToAgentF(mpmasPlotCell* p, w_rechte* w, double zins, agentF* newOwner);
	// Troost **********end block insert**********

// Troost 20140707 Land markets - type 4
// Troost **********begin block insert**********
	virtual void offerAllParcelGroups();
	virtual void unregisterParcelGroup(parcelGroup*);
	virtual void registerParcelGroup(parcelGroup* pg);
	virtual double arrangePaymentsOrReturnParcelGroupsRentedIn(double availableCash, w_rechte* w);
	virtual void returnParcelGroupIfRentedIn(parcelGroup* pg, w_rechte* w);
	virtual double sellParcelGroupsToAgentL( double target, double zins);
	virtual double transferAllParcelGroupsToAgentL( double zins, int sold);
	virtual double transferParcelGroupToAgentL(parcelGroup* pg,  double zins, int sold);

	virtual double getFreePlotCapacityInSlot(int slot, int toRented);
	virtual pair<double,mpmasPlotCell*> findPlotWithCapacityForAsset(double size, int slot, double capPerUnit, int toRented, int requiredSoilType = -1);
	virtual void assignAssetToPlot( int ownerUserId, int ownerInventoryId, int plotX, int plotY, int code, int plotCapacitySlot, double totCapacityUsed);
	virtual void partlyDetachAssetFromPlot( int userInventoryId, int plotX, int plotY, double reductionCapacityUsed);
	virtual void detachAssetFromPlot( int ownerInventoryId, int plotX, int plotY);
	virtual void changeUserInventoryIdOfAttachedAsset(int plotX, int plotY, int ownerAgentId, int ownerInventoryId, int newUserInventoryId);
	virtual void changeOwnerInventoryIdOfAttachedAsset(int plotX, int plotY, int oldOwnerInventoryId, int ownerInventoryId, int newOwnerInventoryId);

	// Troost **********end block insert**********

	int identifyPlotsForRedistribution();

	virtual double getAreaOfNRUWithRemainingDurationOfAtLeast(int soilType, int minDur);
	virtual double getValueForPlot(mpmasPlotCell* p, int paramID);
#ifdef MULTIPERIOD
	void updateMatrixAreaOfNRUbyPeriods(int numPeriods);
	vector<vector<double> >* getMatrixAreaOfNRUbyPeriods ();
	map<pair<int, int>, pair<double, double> > getYieldsByNRUs(bool harvestAfterNewLandUseDecision); //cropYield,stoverYield by YldIndex, NRU
#endif
	//TSPC crop-soil model
	virtual void makeConvVector(void);
	virtual void cleanConvVector(void);
	virtual void makeConvMatrix(void);
	virtual void cleanConvMatrix(void);
	virtual void makeAvSlVector(void);
	virtual void makeAvSeVector(void);
	virtual void makeMTypesInNRUs(void);
	virtual int getMTypeForNRU(int nru);//returns main soil type in NRU
	virtual int getNumPlotsInNRU(int nru);
	virtual int getNRUForType(int typ);
	virtual int getFreeNRU(void);
	virtual double getAvSlNRU(int nru);
	virtual double getAvSeNRU(int nru);
// Troost 20140527 Generalize dynamic NRUs
// Troost **********begin block insert**********
	virtual double calcAggregatedUserDefinedLandscapeParameterForNRU(int paramID, int agg, int nru);
	virtual void makeCellsInSoilTypes();
	virtual int getNumPlotsOfSoilInNRU(int nru, int t);
	virtual multimap<int,int>::iterator getItLboundSoilTypesInNRU(int nru);
	virtual multimap<int,int>::iterator getItUboundSoilTypesInNRU(int nru);
	virtual	size_t getItCountSoilTypesInNRU(int nru);
// Troost **********end block insert**********

// Troost 20160725 Distance consideration in NRUs
// Troost **********begin block insert**********
	vector<int> getNRUsOfParcelGroup(parcelGroup* pg);
// Troost **********end block insert**********

	virtual void assignPlotsToNRUs(int*& sTypes);
// Troost 20130910 Make land markets work with external crop growth model
// Troost **********begin block insert**********
	virtual void updatePlotsInNRUs(int*& sTypes, int changedType = -1, int newTotal = -1, mpmasPlotCell* switchedPlot = NULL, bool plotIsTakenOut = false);
// Troost **********end block insert**********

	virtual void mergeSoilTypes(int& numTypes, int*& sTypes, int*& typToTyp);
	virtual void assignNRUsToPlots(int update = false);
	virtual void assignNRUsDynamically( int*& sTypes);
	virtual void assignNRUsUserDefined( int*& sTypes);
	virtual void initValuesNRUs();
	virtual mpmasPlotCell* searchFirstNRU(int nru);
	virtual mpmasPlotCell* searchNextNRU(int nru);
	virtual mpmasPlotCell* searchLastNRU(int nru);
		virtual mpmasPlotCell* searchPrevNRU(int nru);
	virtual sProc* getSProcNRU(int nru);
	virtual lUseH getLUseHNRU(int nru);
	virtual void calcYields(int kID, int fID, int nru, crpMixNRU*& cmNRU);

	// update plots -> cells
   //virtual void copyContent_Parcels2Landscape(Content cont);// ## Tbe 110920 not in use
   //virtual void copyContent_Parcels2Landscape_monthly(Content cont, int m);// ## Tbe 110920 not in use

	// update cells -> plots
	//virtual void copyContent_Landscape2Parcels(Content cont); // ## Tbe 110920 not in use
	//virtual void copyContent_Landscape2Parcels(Content cont, bool flag_fixCopyToCell); // ## Tbe 110920 not in use
   //virtual void copyContent_Landscape2Parcels_monthly(Content cont, int m);// ## eliminate

   // updates crpMixClass - value directly to cell.    
   // Required: AllocateCroppingActivitiesSpatially, uses parcel.crop_ind as index into <cropMixClass>
	virtual void copy_cropMixTemp2Landscape_monthly(crpMixClass* cmClass, Content cont, int m);
   
	// Transfers values from landscape to crop mix
   virtual void copy_Landscape2CropMixClassTemp_monthly(crpMixClass* cmClass, Content cont, int m);//Tbe 090303 fixed bug
	virtual void copy_Landscape2CropMixClassTemp(crpMixClass* cropMix, Content cont);//Tbe 090303

   //currently, only for testing
   virtual double getMeanOfAllPlots(Content cont);
	
	//erases all parcels in list
	virtual void liste_loeschen(void);

	//Print and write
	virtual void printToScreen(mpmasPlotCell*);
	virtual void printOnScreen_list(void);
	virtual void writePlotDataInStrm(FILE* stream0,
		int agID,
		int scID,
		int skID,
		int fsID,
		int poID,
		int clID,
		int nwID,
		int sgID,
		int expT,
		int colr,
		int numP);
	
	virtual void writeAllPlotsToFile(ofstream& out);
	virtual void writeAllPlotsToFile(FILE* strm);
	virtual void writeAllPlotsToFileBackwards(ofstream& out);

	virtual mpmasPlotCell* allocateOneHaToActivityPP(int crA, int crIndex);//##substitute 090222
	virtual mpmasPlotCell* assignCropActivityToNextPlot(int actID, int soil, int crIndex);
	virtual void assignCropActivityToPlot(int actID, mpmasPlotCell* pl, int crIndex = -1);

// Troost 20160331 Soil type transitions
// Troost **********begin block insert**********
	void setAllPlotsWithoutCrops();
// Troost **********end block insert**********
#ifdef LIVSIM_COUPLING
	void setAllPlotsWithoutGrazingPressure();
#endif
	//leaves plots, only advisable if just including pointers!
	//virtual void clearList(void);// ## Tbe 110920 not in use

	//find position of a parcel "searchParcel" in list p_liste, accordiong to row/col position
	//virtual int getPosInList(mpmasPlotCell* searchParcel);// ## Tbe 110920 not in use

	//append one parcel to end of list, returns new length of list
	//virtual int appendParcelToEndOfList(mpmasPlotCell* newParcel);// ## Tbe 110920 not in use

	//returns length of list in memory
	//virtual int get_len(void);// ## Tbe 110920 not in use
#ifdef LIVSIM_COUPLING
	vector<mpmasPlotCell*>getPlotsWithNRUAndLandUse(int nru, vector<int> landUses);
#endif


};


#endif

