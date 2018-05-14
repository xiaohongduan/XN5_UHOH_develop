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
//  File:       PopulationNetworkSegmentList.h (formerly sliste.h)
//
//  Contents:   Manages linked list of all agent communication networks
//
//
//
//------------------------------------------------------------------------------


#ifndef _PopulationNetworkSegmentList_h
#define _PopulationNetworkSegmentList_h

#include "FinancialMathFunctions.h"
#include "PopulationNetworkSegment.h"

#ifdef LIVSIM_COUPLING
#include "MpmasLuciaLivSimCoupling.h"
#endif


#include <map>

class s_liste
{	protected:
	int nw_nr;				//Netzwerknummer
   int sanzahl;         //Anzahl Segmente
   int letzte_s_nr;		//zuletzt vergebene Segmentsnummer

	//Gesamtwerte fuer Netzwerk
   int n_ges;				//Nutzeranzahl
   double ha_ges;			//Flaechen
   double db_ges;			//Deckungsbeitraege
   double ws_ges;			//Wertschoepfung

	//Matrizen zu den Innovationen in den Segmenten (d.h. Investitionsobjekte,
   //die nicht schon in der 1. Periode vorhanden sind)
   MatrixDouble innovN;			//Nutzeranzahl
   MatrixDouble innovHA;		//Flaechen
   MatrixDouble innovDB;		//Deckungsbeitraege
   MatrixDouble innovWS;		//Wertschoepfung

   MatrixDouble erzw_aufgabe;	//Matrizen der erzwungenen Betriebsaufgaben je Segment
   MatrixDouble frei_aufgabe;	//Matrizen der freiwilligen Betriebsaufgaben je Segment
	MatrixDouble plan_fehler;	//Matrix der moeglichen Planungsfehler je Segment

	segment* firstSegment;
   segment* merkeSegment;//wird fuer suche_naechsten() gebraucht

	///complete list of investment objects (both innovations and well-established practices)
	investmentObject* investmentObjectsInNetwork;
	int numInvestmentObjects;

// Troost 201600506 Assets for multiperiod
// Troost **********begin block insert**********
	//should make searching more efficient
	//replace by unordered map when switching to C++11
	std::map<int, int> mapObjectIdToInvestmentObjectsArrayIndex;
	typedef std::map<int, int> mapObjIDs;
// Troost **********end block insert**********

	MatrixDouble* objectIdsPerInnovation;

// Troost 20140611 Disinvestment of assets
// Troost **********begin block insert**********
	int activatedDisinvestment;
// Troost **********end block insert**********

// Troost 20141210 Land markets - type 4, assets traded with land
// Troost **********begin block insert**********
	int bonusalter;
	int bonusdauer;
// Troost **********end block insert**********

#ifdef LIVSIM_COUPLING
	MpmasLivSimTransformationInfo mpmasLivSimTransformator;
#endif


	bool reportFileRenewed;//to make sure old files are overwritten at simulation start

	void returnFscan(int retVal, char* strVar);


   public:
   s_liste(int);
   virtual ~s_liste()
   {  innovN.deallokieren();
		innovHA.deallokieren();
		innovDB.deallokieren();
		innovWS.deallokieren();      
      erzw_aufgabe.deallokieren();
		frei_aufgabe.deallokieren();
      plan_fehler.deallokieren();
   	liste_loeschen();

		delete [] investmentObjectsInNetwork;
		delete [] objectIdsPerInnovation;
   }

	///initialisation from files
	virtual void readNetworkInput(char* filename);
	virtual void writeNetworkInput(char* filename);
// Troost 20141210 Land markets - type 4, assets traded with land
// Troost **********begin block insert**********
	virtual void readInvestmentsAttachedToLand();
	virtual void readPerennialBonusFile(char* filename);
// Troost **********end block insert**********

	//virtual void innov_anlegen();//## substitute
	virtual void checkNumberOfInnovations();

   virtual void aufgabe_anlegen();//legt die Matrizen zur Betriebsaufgabe an
	virtual void aufgabe_eintragen(int, int); //Betriebsaufgabe eintragen
   virtual void planfehler_eintragen(int);//Planungsfehler vermerken

   //legt Segment an: Uebergabe Name der Input-Datei
	virtual void segment_anlegen(char*);
	virtual void deallokieren(segment*);//deallokiert ein Segment
	virtual void einfuegen(segment*);//fuegt ein Segment ein
 	virtual int get_anzahl(void);//liefert Anzahl der Segmente
   virtual int get_nummer(void);//liefert die zuletzt vergebene Segmentsnummer
   virtual segment* segment_suchen(int);//uebergeben wird s_nummer;
   virtual segment* getFirst();//liefert Listenerstes
	virtual void updateProductionCostsOfAvailablePermanentCrops();

   //these functions handle queries
   virtual double getInvBound(int segID, int soilType);
	virtual double getInvShadowPrices(int segID, int soilType);
	virtual double computeDebtService(int idx, double share, double interest);
	virtual int getIndexOfInvestmentObject(int objID);

	virtual int insertObjectIDsForLpColumns(MatrixDouble& lpIndicesAndRelativeCosts);

	virtual int getNumInvestmentObjects();
	virtual const investmentObject* getPointerToInvestmentObjects();
	virtual investmentObject* getPointerToInvestmentObjectsNonConst();
	virtual const MatrixDouble* getPointerToObjectIdsPerInnovation();
	virtual int getInnovationIdForInvestmentObject(int objectID);

// Troost 20140611 Disinvestment of assets
// Troost **********begin block insert**********
	virtual int disinvestmentActivated();
// Troost **********end block insert**********

   //uebergibt Beitraege zu den Gesamt-NW-Werten
   virtual void make_n_ges(int);
   virtual void make_ha_ges(double);
   virtual void make_db_ges(double);
   virtual void make_ws_ges(double);
   virtual void make_n_ha_start(int, double);

	//nicht benoetige Elementfunktion, liefert Schwellenwert des Vorsegments
	//virtual double get_schwellenwert(int); //uebergeben wird Segmentnummer

	//liefert fuer ein Untereinzugsgebiet (d.h. fuer eine Betriebsliste) die Daten
	//der Nutzer eines Inv.objekt innerhalb des Netzwerks
	//Uebergabe: SubcatchmentNum, Code des Objekts
	virtual nutz get_Nutzer(int, int);//subsitute ##
   virtual nutz getUsersOfInnovation(int catchmID, int idxInnovation);

	virtual int get_seg_banzahl(int wann, int seg);//Anzahl der Segmentsbetriebe
   virtual void matrizen_aktualisieren();//Aktualisierung aller Invest_matrizen

	virtual void printToScreen(segment*);
   virtual void printOnScreen_list(void);
	virtual void liste_loeschen(void);
   virtual void diffusionsdaten(FILE*);//Daten in geoeffnete Datei schreiben
   virtual void aufgabedaten(FILE*);
	virtual void diffusion_periodendaten(FILE*);
	virtual void aufgabe_periodendaten(FILE*);

// Troost 20141210 Land markets - type 4, assets traded with land
// Troost **********begin block insert**********
	//Policy parameters
	virtual int PerennialBonusAge();
	virtual int PerennialBonusDuration();
// Troost **********end block insert**********


#ifdef LIVSIM_COUPLING
	void readMpmasLivSimTransformationInfo();
	LivSimAssetMatchingClass getLivSimMatchForAsset(int assetObjId, int age, bool allowEmptyReturn = false);
	pair<int,int> getAssetMatchForLivSimAnimal(string breed_, int sex_, double age, double bwU, int gestating_, int lactating_);
	pair<int,int> getAssetMatchForLivSimAnimal(const LivSimAnimalInfo & animal);
	int ageConversionLivSimToMpmas( double livsimAge){		return mpmasLivSimTransformator.ageConversionLivSimToMpmas(livsimAge);}
	double ageConversionMpmasToLivSim( int mpmasAge){		return mpmasLivSimTransformator.ageConversionMpmasToLivSim( mpmasAge);}
	int ageConversionLivSimToMatchClass( double livsimAge) {		return mpmasLivSimTransformator.ageConversionLivSimToMatchClass(livsimAge);}
	double ageConversionMatchClassToLivSim( int matchClassAge) {return 	mpmasLivSimTransformator.ageConversionMatchClassToLivSim(matchClassAge);}
	int bwConversionLivSimToMatchClass( double livsimBw){			return mpmasLivSimTransformator.bwConversionLivSimToMatchClass( livsimBw);}
	double bwConversionMatchClassToLivSim( int matchClassBw){	return mpmasLivSimTransformator.bwConversionMatchClassToLivSim( matchClassBw);}
	vector<int> getPastureIdListForSeason(int season);
	const MpmasLuciaPastureIdList getPastureIdLists();
	int getMaxLivsimSeason(){return mpmasLivSimTransformator.getMaxSeason();}

	LivSimHerdManagementTable getHerdManagementTable() {return mpmasLivSimTransformator.getHerdManagementTable();}

#endif
};

#endif

