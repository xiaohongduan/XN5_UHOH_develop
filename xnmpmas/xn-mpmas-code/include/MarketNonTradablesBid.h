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
//  File:       MarketNonTradablesBid.h (formerly gebot.h)
//
//  Contents:   Bid submitted by one agent, contains data such as ownership,
//              minimum asking price or maximum offered price
//
//
//------------------------------------------------------------------------------



//Header fuer die Pachtgebot-Klasse
#ifndef _MarketNonTradablesBid_h
#define _MarketNonTradablesBid_h


#include <string>
#include <vector>
#include <stdio.h>
#include <iostream>
#include <fstream>

#include "FinancialMathFunctions.h"
//#include "AgentParcel.h"

using namespace std;

class parcelGroup;
//Klasse der Pachtgebote, die Pachtanbieter und Pachtnachfrager aufstellen. Je
//nach dem, ob es sich um einen Anbieter (Nachfrager) handelt, sind einige der
//Datenelemente entsprechend zu interpretieren.
class gebot
{  protected:
	int x;//x-Koordinate der angebotenen Parzelle (der Hofstelle des Nachfragers)
   int y;//y-Koordinate der angebotenen Parzelle (der Hofstelle des Nachfragers)
   int typ;	   //Bodentyp des Pachtgebots
   int b_nr;	//Betriebsnr. des Pachtanbieters (Pachtnachfragers)
   int b_sc;	//Subcatchment ID des Pachtanbieters (Pachtnachfragers)
   w_jahr* vol;//damit verbundenes monatliches Wasservolumen //neu 2003
   double entf;//Maximalentfernung einer in Betracht kommenden Parzelle
   double pacht;//ausgehandelte Pachtzahlung
	
	w_rechte* waterRights;//Water shares

   int p_nr;	//Betriebsnr. eines moeglichen Pachtpartners
   int p_sc;	//Subcatchment ID eines moeglichen Pachtpartners
   int flag;	//zur Kennzeichnung, ob bereits ausgefuehrte Transaktion

public:
	//Uebergabe: x-Koord., y-Koord., Bodentyp, Betriebsnr., SubcatchmentNum, Volumina, maximale Entfernung und Rechte
   gebot(int xk, int yk, int t, int b, int sc, w_jahr* v, double e, w_rechte* w);
   virtual ~gebot()
   {	delete vol;
		delete waterRights;	
   }

   //fuer die Auktionsgebotsliste
   gebot* nextAuktion;
   void setNextPtr_A (gebot*);
   gebot* getNext_A();

	//Elementfunktionen liefern geschuetzte Informationen zurueck
   virtual int get_flag();
	virtual int get_x();
	virtual int get_y();
	virtual int get_typ();
	virtual int get_bnr();
	virtual int get_bsc();
	int getCatchmentOfOwner(){	return b_sc;}
   virtual w_jahr* get_vol();
   virtual double get_m_vol(int);
   virtual double get_entf();
	virtual int get_pnr();
	virtual int get_psc();
   virtual double get_pacht();
	virtual w_rechte* getWaterRights();

	//Elementfunktionen zur Veraenderung der Eigenschaften
   virtual void set_flag(int);//Uebergabe: true/false
	virtual void partner_eintragen(int, int);//Uebergabe: Betr.-, Subcatchment-nr
   virtual void pacht_eintragen(double);

   virtual void printToScreen();
	virtual void writeToFile(FILE*);   
};
struct bidForParcelGroupOffer {
		int agId;
		double bidValue;
		bidForParcelGroupOffer();
		bidForParcelGroupOffer(int a,double b);
};
class parcelGroupOffer
{
		parcelGroup* pg;
		int owner_b_nr;
		double reservePrice;
		bool bidsSorted;
		vector< bidForParcelGroupOffer > bidList;
		static int maxId;
		int id;
	public:
		parcelGroupOffer(parcelGroup* _pg, int b_nr, double rprice = 0.0);
		virtual size_t getSize() const;
		virtual ~parcelGroupOffer();
		virtual parcelGroup* getParcelGroup() const;
		virtual int getOwnerBNr() const;
		virtual double getReservePrice();
		virtual void recordBid(int agentID, double bid);
		vector<bidForParcelGroupOffer > listBids();
		bidForParcelGroupOffer getHighestBid();
		bidForParcelGroupOffer getSecondHighestBid();
	   bool operator==(const parcelGroupOffer& b){
	     return id == b.id;
	   }
#ifdef PAR2
	   virtual void synchronizeBidsMPI();
	   static void initMpiBidType();
#endif
};

struct  parcelGroupOfferSorter
{
		bool operator() ( const parcelGroupOffer& lhs,  const parcelGroupOffer& rhs);

};
struct  parcelGroupBidSorter
{
		bool operator() ( const bidForParcelGroupOffer& lhs, const bidForParcelGroupOffer& rhs);

};

#endif

