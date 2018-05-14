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
//  File:       AgentLandOwner.h (formerly agentL.h)
//
//  Contents:   Header of non-agricultural landowner class that is derived
//              from the basic class "agent"
//
//
//------------------------------------------------------------------------------


#ifndef _AgentLandOwner_h
#define _AgentLandOwner_h


#include "Agent.h"
#include "AgentParcelList.h"
// Troost 20141210 Land markets - type 4, assets traded with land
// Troost **********begin block insert**********
#include "PopulationNetworkSegment.h"
// Troost **********end block insert**********
#include <string>

class agentL : public agent
{  protected:

	int mapColor;	      //color of plots in maps (max. 56 colors)

	int fl_anzahl;	      //total number of parcels
   int* plotsPerType;      //pointer to parcels of different soil types

	p_liste bflaech;     //list of own parcels
   MatrixDouble wr;     //water rights

	double* erw_N_i;     // expected net water delivery
	double* N_i;         // real net water delivery
	double* erw_Ext_i;   // expected additional water delivery
	double* Ext_i;       // real additional water delivery
   double* WPa_i;       // monthly net water rented

   double liqMeans;  //here: rental payments received

// Troost 20141210 Land markets - type 4, assets traded with land
// Troost **********begin block insert**********
	vector<investdef> binvest;	//Zeiger auf das Inventar-Array
	int binv_anz;
// Troost **********end block insert**********

   public:

   //Constructor
   agentL(int agid, int scid, int skid/*, int numP*/);

   //Destructor
	virtual ~agentL();

   //Member functions
   virtual int getMapColor();
   virtual void setMapColor(int clr56);

   virtual int getAgentMapId();

   virtual int getNumberOfPlots();
   virtual void setNumberOfPlots(int numP);
   virtual int checkNumberOfAllUsablePlots();
   virtual int get_plotsPerType(int sType);
   virtual void set_plotsPerType(int sType, int numP);

   virtual void transferPlotAsRentedOut(int r, int c, w_rechte* w, int years, double pmt, int newAgID, int newScID, int typ);//refactored from <set_bflaech_verpachtet>
   //virtual void make_bflaech(int c, int r, int typ, int fst, double dst, parcelGroup* pg = NULL);
   virtual void make_bflaech(mpmasPlotCell* pp);
   virtual void bflaechen_printOnScreen();
   //virtual void bflaech_einfuegen(mpmasPlotCell*);
   virtual void receivePlotAsNewOwnerAndUser(mpmasPlotCell* p, w_rechte* w, w_jahr* volN);
   virtual void receivePlotRentedOutReturned(mpmasPlotCell* p, w_rechte* w);

   virtual void transferThisPlotToNewOwner(mpmasPlotCell* p, w_rechte* w, agentF* newAgent);
   virtual void transferThisPlotToNewOwner(mpmasPlotCell* p, w_rechte* w, int newAgentFstID);


   virtual double get_liq();
	virtual void set_liq(double pmt);
	
	virtual double get_N_i(int month);
   virtual double get_WPa_i(int month);
   virtual void set_WPa_i(int month, double vol);
	virtual void waterrights_eintragen(int srcID, double share);
   virtual void hole_durchs_waterrights(w_rechte*);
	virtual void waterrights_austragen(w_rechte*);
	virtual void printOnScreenWaterRights();

 	virtual double receiveNetInflows(int month, int = true);//Flag (save sector values yes/no) //renamed from <N_berechnen>
   virtual double receivePlantWaterDemandsSatisfied(int month);

   virtual void makeVirtualWaterExpectation();
   virtual void ausserldw_angebote();

// Troost 20140707 Land markets - type 4
// Troost **********begin block insert**********
	virtual void offerAllParcelGroups();
	virtual void unregisterParcelGroup(parcelGroup* pg);
	virtual void registerParcelGroup(parcelGroup* pg);
// Troost **********end block insert**********

	virtual int identifyPlotsForRedistribution();

// Troost 20141210 Land markets - type 4, assets traded with land
// Troost **********begin block insert**********
	///Functions related to investment goods
	virtual int get_binv_size();
	virtual int get_binv_anz_obj();
	virtual char* get_binv_acc(int i);
	virtual void set_binv_acc(int i, char* chr);
	virtual int get_binv_code(int i);	//Parameter Row
	virtual double get_binv_ask(int i);	//Parameter Row
	virtual void set_binv_ask(int i, double price); //Parameter Row
	virtual double get_binv_anz(int i); //Parameter Row
	virtual int get_binv_age(int i); //Parameter Row
	virtual void set_binv_age(int i, int age); //Parameters Row, age
	virtual double get_binv_shareEC(int i); //Parameter Row
	virtual void set_binv_shareEC(int i, double shr); //Parameters Row, share
	virtual double get_binv_rateDC(int i); //Parameter Row
	virtual void set_binv_rateDC(int i, double rat); //Parameters Row, rate
	virtual investdef get_binv(int i);
	virtual int inventarisieren(char* acc, int oID, double pPr, double siz, double ssf, double iDC, int age, string remark = "default", int plotX =
			-1, int plotY = -1, int newlyAttach = false);
	virtual void deinventarisieren(int, string remark ="default", int detach = 1, int disinvest = 1); // Uebergabe: Inventarnr.
	virtual void binvest_ausschreiben(int which = -1);
	virtual void binvest_writeToStream(FILE* strm);
	virtual void deregisterRentedAttachedAssets(plot* p);

	virtual void updateAssetAges();
	virtual int getLifeSpanOfInvestmentObject(int code);
// Troost **********end block insert**********

   virtual void writeAgtIdEntry(FILE* stream);
   virtual void writeAgtWaterData(int month);
};


#endif
