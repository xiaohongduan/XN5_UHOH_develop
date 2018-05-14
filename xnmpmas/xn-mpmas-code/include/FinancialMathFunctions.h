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
//  File:       FinancialMathFunctions.h (formerly part of funktio9.h)
//
//  Contents:   Financial mathematics functions as used by agents
//
//
//
//------------------------------------------------------------------------------



#ifndef _FinancialMathFunctions_h
#define _FinancialMathFunctions_h
class MatrixDouble;



double wigefa(double i,int t);
double durchverz(double i,int t);
double KW(double, MatrixDouble&);//Zins (z.B. 0.05), Nettozahlungsstrom (Spaltenvektor)
double zeitwert(double ask, int n_dauer, int alter, double i);
double zeitwert2(double ask, int n_dauer, int alter, double ekzins,
double fkzins, double v);


//neue Datentypen, um die Uebergabe von Parametern zu vereinfachen
typedef struct
{	double w1;
	double w2;
} double2;


typedef struct
{	int w1;
	int w2;
} int2;


//Auxiliary class with dynamic array
class w_jahr
{  public:
   double* monat;	//public pointer to monthly values

   w_jahr(int m);

    ~w_jahr();
};


//Datentyp "Pachtgebote", bestehend aus 2 doubles und monatevielen doubles
class p_gebot
{  public:
   double q1; //Schattenpreis Parzelle
	double q2; //Schattenpreis WaterRights
   double* m; //Monatliche Schattenpreise fuer je 1 Wassereinheit

   p_gebot();

	p_gebot(int mo);

   p_gebot(const p_gebot& w);

   virtual ~p_gebot();
};


//Datentyp "Pachtdaten", bestehend aus typenvielen und monatevielen doubles
class p_daten
{  public:
   double* B;  //Preise je Typ und Hektar
   double* W;  //Monatliche Preise fuer je 1 Wassereinheit

   p_daten();
   p_daten(int ty, int mo);
   virtual ~p_daten();
};


//Datentyp "WaterRights", bestehend aus 2 Arrays mit zuflussvielen Elementen
//Auxiliary class with dynamic array
class w_rechte
{  public:
	int* zuf;				//Nummern der Sektorzufluesse
   double* watershares; //Sektorale Water-Anteile an diesen Zufluessen

   w_rechte();
	w_rechte(int zanz);
   w_rechte(const w_rechte& w);
   virtual ~w_rechte();
};

class waterLinkedToPlots
{	public:
	int mapRow;
	int mapCol;
	w_rechte* waterRights;

	waterLinkedToPlots();
	waterLinkedToPlots(int rowInMap, int colInMap, w_rechte* w);
	waterLinkedToPlots(const waterLinkedToPlots& w);
	virtual ~waterLinkedToPlots();

	virtual void printOnScreen();
};

//Dynamic version of typedef struct l_vector
typedef struct
{	double* umf;	 //Pointer to 'Produktionsumfaenge'
	double* zrow;   //Pointer to 'Zielfunktionskoeffizienten'
	double ziel;    //Zielwert (Gesamt-Deckungsbeitrag)
} l_vektor;


struct mat // Charakteristika der LP-Matrix
{	public:
	int lpanzahl;
   int lpfehler;
   int lpkritisch;
	int m1;//Anzahl der Restriktionen LE
	int m2;//Anzahl der Restriktionen GE
	int m3;//Anzahl der Restriktionen E
};

//Datentyp "Nutzerinformation"
typedef struct
{	int nr;	  //Anzahl der Nutzer (bzw. -1, wenn kein Nutzer)
	double ha; //genutzte Flaeche
   double db; //Deckungsbeitrag
   double ws; //Wertschoepfung
} nutz;



#endif

