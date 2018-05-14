//----------------------------------------------------------------------------
//
//  Thomas Berger (main author), Pepijn Schreinemachers, and Thorsten Arnold
//
//
//  Hohenheim University (490d)
//  Professorship for Land Use Economics in the Tropics and Subtropics
//
//  May not be distributed without permission by main author
//
//  File:       AgentPopulationHandler.h (formerly bliste.h)
//                                                                        
//  Contents:   Manages linked list of all agents
//
//
//
//------------------------------------------------------------------------------

#ifndef _AgentPopulationHandler_h
#define _AgentPopulationHandler_h

#include <string>
#include "BasicEnums.h"
#include "FinancialMathFunctions.h"
#include "Agent.h"
#include "AgentFarmHousehold.h"
#include "AgentOtherFarms.h"
#include "MainAuxiliaryFunctions.h"

class IrrigationMethodSector;
class TimeHandler;

// Sektor-ID eines Betriebes in der Liste
typedef struct {
	int sek;//Sector ID
	int sc; //Subcatchment ID
} s_id;


class b_liste
{	private:

	protected:
	int bl_nr;
	int banzahl;
	int letzte_b_nr;
	int* banzahl_NW;

	agent* firstBetrieb;
	agent* merkeBetrieb;//wird fuer suche_naechsten() gebraucht

	public:
	b_liste(int scID);//Uebergabe Betriebslisten bzw. Subcatchment-Nummer
	virtual ~b_liste();

	//create agents
	virtual void makeAgentL(int scid, int skid/*, int numP*/);
	virtual agentF* makeAgentF(int scid, int skid, int ppid, int clid, int nwid, int fstd, coord fstCoord/*, int numP*/);
	virtual void makeAgentO(int scid, int skid, int mapId/*, int numP*/);
	virtual void deallokieren(agent*);//deallokiert einen Betrieb
	virtual void betriebssortiert_einfuegen(agent*);//fuegt einen Betrieb ein

	virtual agent* ersten_suchen(int);//uebergeben wird sek_nr
	virtual agent* imaginaeren_suchen(int s);//uebergeben wird sek_nr
	virtual agent* searchForAgentO(int s);//uebergeben wird sek_nr
	virtual agent* naechsten_suchen(int);//uebergeben wird sek_nr
	virtual agent* betrieb_suchen(int);//uebergeben wird b_nummer;
	virtual agent* betrieb_herausnehmen(int);//  "        "
	virtual s_id betrieb_SID(int);//uebergeben wird b_nummer
	virtual int farbe_ermitteln(int);//uebergeben wird b_nummer
	virtual agentF* searchFarmAgent(int _fstID);// ## Tbe 090820
	virtual agent* searchAgent(int _agentMapID, int _catchmID, int _sectorID);// ## Tbe 110423

	//liefert die Nutzerinformationen der Anwender eines Inv.objektes innerhalb des Netzwerks
	virtual nutz ersten_nutzer_suchen(int, int);//substitute ##
	virtual nutz naechsten_nutzer_suchen(int, int);//substitute ##
   virtual nutz findFirstUserOfInnovation(int nw, int idxInnovation);
   virtual nutz findNextUserOfInnovation(int nw, int idxInnovation);

	///makes a survey among all agents in population
	virtual int makeSurveyAmongAllAgents(int typeOfSurvey, MatrixDouble& questionnaire, int& firstLpColumn);

#ifdef MONICA
	virtual int getIndividualPerformanceData(int numAgents, int* agentID, double* farmIncome, MatrixDouble& agentCropAreas, const int numCropActs = 0, const int* lpCols = NULL);

#endif

#ifdef MULTIPERIOD
	void addToTotalCropProduction(double* cropArea = NULL, int numCropActs = -1);
	void agentsImportYieldsAggregatedLandUse(  double* cropYield, double* stoverYield);
	void multiperiod_importActualYieldsFromCells(bool harvestAfterNewLandUseDecision);
	void multiperiod_agentsImportExtYieldsNoMaps(vector<int>& cropActivityID,  double* cropYield, double* stoverYield );

#ifdef LIVSIM_COUPLING
	void multiperiod_exportLivSimHerdManagement(LivSimHerdTable& herds_table, LivSimGrazingTable& grazing_table, LivSimFeedingTable& feeding_table);
	void multiperiod_updateAssetsFromUpdatedHerd();
#endif//LIVSIM_COUPLING


#endif //MULTIPERIOD

	virtual int get_bl_nr(void);//liefert Nr. der Betriebsliste zurueck
   virtual void set_bl_nr(int bl_nr_);
	virtual int get_anzahl_NW(int);//liefert Anzahl der Betriebe eines Netzwerks
	virtual int get_anzahl(void);//liefert Gesamtzahl der Betriebe der Subcatchment
	virtual int get_nummer(void);//liefert die zuletzt vergebene Betriebsnummer
	virtual int getCatchID();

	virtual void loeschen (int);//loescht einen Betrieb, dessen Nr. uebergeben wird
	virtual void betriebe_loeschen(int);//loescht alle Betriebe eines Sektors
	virtual agent* getFirst();//liefert Listenersten
	virtual agent* getSecond();//liefert Listenzweiten

	virtual void betriebe_ausstatten();//Betriebe mit Arbeit/Kapital ausstatten
   virtual void writeInitialEndowmentToFile();

	virtual void agentsMakeNewExpectations();//refactored from <uebergang_periode>
	virtual void agentsMakeNewPlanningDecisions();//refactored from <uebergang_periode>
#ifdef MULTIPERIOD
	void agentsMakeStartOfPeriodDecision();
	void implementInvestmentsAndOffspring();
	void agentsResetAccountingVariables();

#endif


	virtual void agentsWritePercentageByIpg();
	virtual void agentsEngageOnLandRentalMarket(int milpMode);//renamed from <pacht_periode>
	virtual void getBidForRentIn //renamed from <hole_gebot>
		(p_gebot* g, int bnr, int typ, int z, int s, w_jahr* vol, double pacht, int milpMode);

	virtual void getBidForRentOut //renamed from <hole_forderung>
		(p_gebot* f, int bnr, int typ, int z, int s, w_jahr* vol, int milpMode);

	virtual void deleteMinimumBidsForSoilType(int bnr, int typ);//renamed from <kopie_loeschen> ##not in use
	virtual void deleteAllMinimumBids(int bnr);//renamed from <alle_kopien_loeschen>
	virtual int makeAgentUpdateItsRHS(int bnr, int typ, int z, int s, w_jahr* vol, double pacht, int milpMode, int pnr, int psc, w_rechte* w);//refactored from <kapaz_aktualisieren>
	virtual int makeAgentImplementExistingContracts(int userMapID, int catchmID, int sectorID, int mapRowIdx, int mapColIdx, double payment,  int duration, w_jahr* vol = NULL, w_rechte* w = NULL);//refactored from <makeAgentUpdateItsRHS>
	virtual void makeAgentRestoreShadowPrices(int);//renamed from <alte_schattenpreise>
	virtual void makeUpdatedPlanningDecision(int bnr, int milpMode);//renamed from <neu_planen>
	virtual double adjustContractsForRentingOut(int bnr, double volB, int m);//renamed from <verpachtung_korrigieren>
	virtual void adjustContractsForRentingIn(int bnr, double volN, int m);//renamed from <zupachtung_korrigieren>

	virtual void agentsArrangePaymentsOnLandRentalMarket();
	virtual void makeAgentsUpdateTheirPlotLists();

// Troost 20140707 Land markets - type 4
// Troost **********begin block insert**********
	virtual void agentsEngageOnLandRentalMarket_ParcelGroupVersion(int milpMode);
	virtual void transferParcelGroupRentedOut (int oid, int uid, double payment, parcelGroup* pg );
// Troost **********end block insert**********

	void agentsApplyForLandRedistribution(milpModeTypes milpMode);

   // ======================================================
   /// Functions for irrigation and EDIC Model
   // ------------------------------------------------------
		
	//assigns monthly water supply to agents and saves sectoral data on water and rental markets
	virtual void wasser_periode_refactured( TimeHandler& th_const );

   // Function called by <wasser_periode>
   // For all agents, compute irrigation water demand and report demand to sector, aggregated by method
   virtual agent* agentsComputeIrrigDemandAndReportToSector(agent* anker, int sk, int thisMonth);
   virtual void agentsReceiveNetInflowsFromCatchment(agent* betr, int sk, int thisMonth);

   // Function called by <wasser_periode>
   virtual double agentsReceivePlantWaterDemandsSatisfied (agent* anker, int sk, int sc, int thisMonth, IrrigationMethodSector* sectorPointerToIrrigationMethods, double Ueb_j_tmp);

   // New function that incorporate and distribute water
   virtual void agentsEstimatePlantWaterDemandsAndReportRest(agent* anker, int sk, int sc, int thisMonth, IrrigationMethodSector* sectorPointerToIrrigationMethods);
   virtual void agentsDistributeRestWater(agent* anker, int sk, int sc, int thisMonth, IrrigationMethodSector* sectorPointerToIrrigationMethods);
   virtual void agentsUpdatePlantWaterSupply(agent* anker, int sk, int sc, int thisMonth, IrrigationMethodSector* sectorPointerToIrrigationMethods);

   // Function called by <wasser_periode>
   // ... afp->irrigateCropsByPriorityGroups
   // ... afp->computeAverageOnFieldIrrigationEfficiency
   virtual double agentsIrrigateCropsByPriorityGroups(agent* anker, int sk, int sc, int thisMonth, IrrigationMethodSector* sectorPointerToIrrigationMethods);
   virtual agent* computeTotIrrigAppliedInSectorAndEfficiency(agent* anker, int sk, int sc, int thisMonth, double irrigAmountSector, double& etaOfSectorAgents );

	// ------------------------------------------------------

	virtual void agentsHarvestCropsAndLivestock();//refactored from <b_liste::produktion_periode>
	virtual void agentsMakeMarketingPlans();
	virtual void agentsMakeConsumptionPlans();//refactored from <b_liste::produktion_periode>

	virtual void calculateAgentIncomes();//Betriebseinkommen und andere Kennziffern

	// Write to file 
	virtual void writeAgentIDsToStream(FILE* openStream);
	virtual void writeAgentLndData(FILE* stream0);
	virtual void writeAgentPfmData(FILE* stream0, FILE* stream1);
	virtual void kapazitaetsdaten_periode(FILE*);//in Outputfile schreiben

	// Debug Functions
	virtual void implementBlackAndWhiteLists();
	virtual void planungsfehler_melden(int nw, int seg);
	virtual void printToScreen(agent*);
	virtual void printOnScreen_list(void);
#ifdef MULTIPERIOD
	virtual void multiperiod_writeAgentDecisionsToFiles();
	virtual void multiperiod_removeExitingAgents();
#else
	virtual void betriebsaufgabe_periode();//loescht aufgeloeste Betriebe
#endif
	virtual void liste_loeschen(void);

   //virtual void writeAllParcelsOfAgentFToFile(void);// ## Tbe 110927 not in use, not sure if this function works at all
   virtual void writeAllCropWaterDataToFile(void);//## Tbe 080220

	///assigns crop activities to crop mixes per soil type, and allocates them spatially if needed
	virtual void createCropMixAndAssignSpatiallyIfNeeded();//renamed from <allocateSpatiallyExplicit_AllCrops>

	//Function updates the value "cont" from all parcells of agent to cell grid
	//This function is used for time-dependent, monthly content, see 
	//switch-statement(cont) in "agentF::copyContent_PlotToLandscape"
	//virtual void copyContent_ParcelToLandscape_monthly(Content cont, int mTot); // ## Tbe 110920 not in use
	virtual void copyContent_PlotToLandscape(Content cont);

   //virtual void copyContent_Landscape2Parcels(Content cont);// ## Tbe 110920 not in use
   //virtual void copyContent_Landscape2Parcels(Content cont, bool flag_fixCopyToCell);// ## Tbe 110920 not in use

	virtual void agentsExportVariables2Landscape(Content cont, int m);
   virtual void copy_Landscape2CropMixClassTemp_monthly(int m,Content cont);
   virtual void agentsInterpretTempInCropMixClass(Content cont, int m);

   // for testing ... 
   virtual void testMeanPerAgent(Content cont);

	// checks if all parcels in crpMixClass are set to "hasCrop"
	virtual void checkIfTrue(bool isTrue);
	virtual void printAllAgentsProductions();
	
	virtual void aggregateAndOutputLanduseBySector(int numberOfCroppingActivities, MatrixDouble& vectorOfIDs, string*& rowNames, string fn);
	virtual void aggregateLanduseBySector(MatrixDouble& matrixOfCatchmentLandUses);
	virtual void outputLanduseBySector(MatrixDouble& matrixOfCatchmentLandUses, MatrixDouble& vectorOfIDs, string*& rowNames, string fn);
	virtual void writeToFile_AllAgentsProduction();
	virtual void writeEdicHeaderToFile(ofstream& out);

   // For Ghana map correction
   virtual int  getFirstInflowForFarmsteadID(int _fstdID);

	/// Saving and restoring (partial) images of the agents ##Tbe 090820
	virtual int saveLivestockListOfAgent(int _fstID);//returns 0 when OK
	virtual int deleteLivestockListOfAgent(int _fstID);//returns 0 when OK
	virtual int restoreLivestockListOfAgent(int _fstID);//returns 0 when OK


#ifdef MULTIPERIOD
	map<int,vector<investdef> > agentsExportAssets();
	void externalUpdateAgentAssets(map<int,vector<investdef> > updatedAssets );
	void externalUpdateAgentAssets_fromStream(istream & in );

#ifdef LIVSIM_COUPLING
	void initializeLivSimHerdsForCoupling();
#endif //LIVSIM_COUPLING

#endif //MULTIPERIOD

};

#endif



