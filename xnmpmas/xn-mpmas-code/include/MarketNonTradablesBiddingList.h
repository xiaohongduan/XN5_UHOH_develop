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
//  File:       MarketNonTradablesBiddingList.h (formerly aliste.h)
//
//  Contents:   Auxiliary class, manages linked list of bids submitted
//              by agents
//
//
//------------------------------------------------------------------------------

#ifndef _MarketNonTradablesBiddingList_h
#define _MarketNonTradablesBiddingList_h


#include "MarketNonTradablesBid.h"

/*
//Position im Gitternetz #bisher noch nicht gebraucht
typedef struct
{	int z;
	int s;
} position;
*/

//Liste der Anbieter bzw. Nachfrager von Parzellen
class a_liste
{	protected:
   int sc_nr;	//SubcatchmentNum
   int sek_nr;	//Num_Sector

   int panzahl;
   int letzte_p_nr;

	gebot* firstBid;
   gebot* merkeBid;//wird fuer die Sucherei gebraucht

   public:
   a_liste(int, int);
   virtual ~a_liste()
   {	liste_loeschen();
   }

	virtual void deallokieren(gebot*);//deallokiert eine Gebot
	virtual void einfuegen_typ_z_s(gebot*);//fuegt ein Gebot sortiert ein
	virtual void einfuegen_typ_zufall(gebot*);//fuegt ein Gebot sortiert ein
 	virtual int get_anzahl(void);//liefert Anzahl der Gebote
   virtual int get_nummer(void);//liefert die zuletzt vergebene Gebotsnummer
   virtual gebot* getFirst();//liefert Listenerstes

   virtual gebot* erste_suchen(int);//uebergeben wird der Bodentyp;
   virtual gebot* naechste_suchen(int);//uebergeben wird der Bodentyp;
	virtual gebot* bieter_gebot_suchen(int, int);//Uebergabe: Betriebsnr, Bodentyp
	virtual double get_maxEntf(int);//Uebergabe: Bodentyp
	virtual void paechter_neu_beruecksichtigen(int);//Uebergabe: Bodentyp

	//Uebergabe: Bodentyp, y-, x-Koord., noerdl. Rand, suedl. Rand, Radius
   virtual gebot* max_bieter_suchen(int, int, int, int, int, double);
   virtual void gebote_zurueckziehen(int bnr, int bsc);
	virtual void liste_aufraeumen(int);//Uebergabe: Flag
	virtual gebot* gebot_herausnehmen(gebot*);//Gebot nur aus Liste streichen
   virtual void gebot_loeschen(gebot*);//Gebot aus Liste streichen und loeschen

	virtual void printToScreen(gebot*);
   virtual void printOnScreen_list(void);
	virtual void writeToFile(FILE*, gebot*);
	virtual void liste_in_datei_schreiben(FILE*);
   virtual void liste_loeschen(void);
};

#endif

