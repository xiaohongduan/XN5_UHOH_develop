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
//  File:       MarketNonTradablesHandler.h (formerly bwmarkt.h)
//
//  Contents:   Water and land market class, manages auctions for land and
//              water; currently implemented at irrigation section
//
//
//------------------------------------------------------------------------------



//Header fuer die Boden-Watermarkt-Klasse

#ifndef _MarketNonTradablesHandler_h
#define _MarketNonTradablesHandler_h


#include "MarketNonTradablesBid.h"
#include "MarketNonTradablesBiddingList.h"
#include <deque>

class bwmarkt
{  protected:
   int sek_nr; 	        //Num_Sector
   int sc_nr;  	        //SubcatchmentNum

   //int trans[TYPEN];     //Anzahl der Transaktionen
   //double maxEntf[TYPEN];//Radius des "Thuenen'schen Rings" der Pachtnachfrager
   int* trans;             
   double* maxEntf;
// Troost 20130930 Additional output land market
// Troost **********begin block insert**********
	int* numOffers; //Anzahl der angebotenen Zellen
// Troost **********end block insert**********

	p_daten* minPreis;      //Mindest- und Maximalpachtpreise
	p_daten* maxPreis;
	p_daten* menge;         //Anteil des Transfers an Gesamt

	a_liste verpaechter;    //Liste der Verpachtgebote
   a_liste paechter;       //Liste der Zupachtgebote
   a_liste transaktionen;  //Liste der vermittelten Gebote

	public:
	bwmarkt(int sk, int sc);
	virtual ~bwmarkt()
	{	delete [] trans;
// Troost 20130930 Additional output land market
// Troost **********begin block insert**********
		delete [] numOffers;
// Troost **********end block insert**********
      delete [] maxEntf;
      delete minPreis;
      delete maxPreis;
      delete menge;
	}


	//Elementfunktionen liefern geschuetzte Informationen zurueck
	virtual int get_trans(int);//Uebergabe: Bodentyp
// Troost 20130930 Additional output land market
// Troost **********begin block insert**********
	virtual int get_numOffers(int); //Uebergabe: Bodentyp
// Troost **********end block insert**********

	virtual double get_min_preisB(int);//Uebergabe: Bodentyp
	virtual double get_min_preisW(int);//Uebergabe: Monat
	virtual double get_max_preisB(int);//Uebergabe: Bodentyp
	virtual double get_max_preisW(int);//Uebergabe: Monat
	virtual double get_mengeB(int);//Uebergabe: Bodentyp
	virtual double get_mengeW(int);//Uebergabe: Monat

	//stellt neue Gebote sortiert in die entsprechenden Listen
	virtual int get_zufallszahl(int);
	virtual void verpacht_aufnehmen(gebot*);
	virtual void zupacht_aufnehmen(gebot*);

   //die folgenden Elementfunktionen ermitteln den Radius sowie den noerdlichen
   //und suedlichen Rand des "Thuenen'schen Rings"
   virtual double radius_ermitteln(int);//Uebergabe: Bodentyp
   virtual int nordrand_ermitteln(int, int);//Uebergabe: Bodentyp, y-Koord.
   virtual int suedrand_ermitteln(int, int);//Uebergabe: Bodentyp, y-Koord.

   //die folgenden Elementfunktionen wickeln den Pachtmarkt ab.
   virtual int teilnehmer_vorhanden(int typ);//ja/nein?
   virtual void makeAuctionForThisSoil(int typ, int milpMode);//renamed from <auktionsrunde_durchfuehren>
   virtual void findAgentOfRentInBid(gebot*, int milpMode);//renamed from <paechter_ermitteln>
   virtual void unvermittelbare_loeschen();
   virtual int renewBidsForThisSoil(int typ, int milpMode);//renamed from <gebote_erneuern>
   virtual void makeUpfrontRentalPayments(int milpMode);//renamed from <pacht_abschliessen>
   virtual void wassertransfers_pruefen(int);//korrigiert unerfuellbare Transfers
   virtual void alles_zuruecksetzen();//Pachtmarktdaten auf Null setzen

   virtual void listen_printOnScreen();//Auktionslisten
   virtual void listen_in_datei_schreiben(FILE*);
	virtual void printToScreen();//Marktdaten
};

class parcelGroupMarket
{
		int sc_nr; //CatchmentID
		int sk_nr;// SectorID
		deque<parcelGroupOffer>offers;
		bool offersSorted;
	public:
		parcelGroupMarket(int _sc_nr, int _sk_nr);
		virtual ~parcelGroupMarket();

		virtual void recordParcelGroupOffer(parcelGroup* pg, int b_nr, double rprice);
		virtual deque<parcelGroupOffer>::iterator getFirstOffer();
		virtual deque<parcelGroupOffer>::iterator getAfterLastOffer();
		virtual size_t countOffers();
		virtual void recordBidForParcelGroupOffer(deque<parcelGroupOffer>::iterator itpgo, int agentID, double bid);
		virtual void recordBidForParcelGroupOffer(parcelGroupOffer* pgo, int agentID, double bid);
#ifdef PAR2
		virtual void synchronizeBidsForParcelGroupOfferMPI(deque<parcelGroupOffer>::iterator itpgo);
#endif
		vector<bidForParcelGroupOffer > listBidsForParcelGroupOffer(deque<parcelGroupOffer>::iterator itpgo);
		virtual pair <int,double> resolveBidsForParcelGroupOffer(parcelGroupOffer*, double markup_second_bid);
		virtual pair <int,double> resolveBidsForParcelGroupOffer(deque<parcelGroupOffer>::iterator itpgo, double markup_second_bid);

		virtual void sortParcelGroupOffers();
		virtual void resetStatistics();
		virtual deque<parcelGroupOffer>::iterator removeOffer(deque<parcelGroupOffer>::iterator itpgo);
		virtual void removeOffer();
		virtual void clearOffers();
};
#endif

