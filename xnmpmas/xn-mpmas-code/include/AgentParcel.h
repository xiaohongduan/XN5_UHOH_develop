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
//  File:       AgentParcel.h (formerly plot.h)
//
//  Contents:   Parcel class containing coordinates, ownership and user data,
//              for example in case of renting-in and renting-out
//
//
//------------------------------------------------------------------------------


#ifndef _AgentParcel_h
#define _AgentParcel_h

#include <set>
#include <list>
#include <map>
#include <vector>
#include "CropSoil.h"
#include "GridCellHandler.h"

class parcelGroup; // Forward declaration because plot und parcelGroup are mutually dependent
class ldscape;
class agentF;

struct attachedAsset {
		int userAgentId;
		int ownerAgentId;
		int userInventoryId;
		int ownerInventoryId;
		int plotCapacitySlot;
		double plotCapacityUsed;
		int code;
		attachedAsset(int _agentId, int _agentInventoryId, int _code, int _plotCapacitySlot, double _plotCapacityUsed)
		{
			ownerAgentId = _agentId;
			userAgentId = _agentId;
			ownerInventoryId = _agentInventoryId;
			userInventoryId = _agentInventoryId;
			plotCapacitySlot = _plotCapacitySlot;
			plotCapacityUsed = _plotCapacityUsed;
			code = _code;
		}
} ;

class plot
{  protected:
	int x;      //x-coordinate (column index in sector map)	
   int y;      //y-coordinate (row index) 
   int typ;		//soil type
   int b_nr; 	//agentID of owner
	int p_sc;	//subcatchment ID, renamed from <b_sc>
   int p_sk;	//sector ID of parcel 
	double entf;//distance to farmstead
	int plotStatus;//moegliche Zustaende: 0 brach, 1 Hofstelle, 2 Dauerkultur, 3 einjaehrige Kultur, 4 verpachtet

	int durationRentalContract;
	double rentalPayment;
	int userAgentID;//i.e. agent renting-in: -1 indicates parcel not rented-out, -2 indicates parcel returned to owner

   //TSPC crop-soil module
   int nruID;  //ID nutrient response unit
   lUseH luHis;//land use history
   sProc* slPro;//soil processes

   bool hasCrop;      // parcel was allocated to a specific cropping activity
   int  cropA;        // ID for specific cropping activity
   int crop_ind;      // index into CropMixClass
   
   double precip;
   double soilMoisture;

// Troost 20140527 Generalize dynamic NRUs
// Troost **********begin block insert**********
	//double * userDefinedParams;
// Troost **********end block insert**********
// Troost 20140707 Land markets - type 4
// Troost **********begin block insert**********
	parcelGroup* parcelGroupPtr;
// Troost **********end block insert**********
// Troost 20141210 Land markets - type 4, assets traded with land
// Troost **********begin block insert**********
	list<attachedAsset> attachedAssets;
	double* capacityForAssets;
	static int plot_capacity_slots;
// Troost **********end block insert**********

	public:
	//Uebergabe: x-Koord., y-Koord., Bodentyp, Betriebsnr., SubcatchmentNum, Sektornr., Zustand, Entfernung
	plot(int, int, int, int, int, int, int = 1, double = 0.0, parcelGroup* pg = NULL);
	virtual ~plot();

	//fuer die Betriebsparzellenliste
   plot* nextPlot;
	plot* prevPlot;

   void setNextPtr_P (plot*);
   plot* getNext_P();

   void setPrevPtr_P (plot*);
   plot* getPrev_P();

	//Elementfunktionen liefern geschuetzte Informationen zurueck
	virtual int get_x();
	virtual int get_y();
   
	virtual int get_col();
	virtual int get_row();
	virtual int get_typ();
	virtual int get_bnr();
	virtual int get_psc();
	virtual int get_psk();
	virtual int getPlotStatus();// renamed from <get_zustand>
   virtual double get_entf();

	virtual int getCatchmentOfOwner() {	return p_sc;}//identical with <get_psc()>
	virtual int getDurationRentalContract();
	virtual double getRentalPayment();
	virtual int getAgentUserID();
	virtual int getAgentOwnerID();
	virtual void setAgentUserID(int userID);

   //Elementfunktionen zur Veraenderung der Eigenschaften
   virtual void set_bnr(int);
   virtual void set_typ(int);
	virtual void set_entf(double);
	virtual void setDurationRentalContract(int years);
	virtual void setRentalPayment(double pmt);
	virtual void setStatusFree();//Plot auf Brache setzen <raeumen>
	virtual void setStatusFarmstead();//Plot mit Hofstelle besetzen <hofstelle>
	virtual void setStatusPermanentCrop();//Plot mit Dauerkultur belegen <belegen>
	virtual void setStatusRented(int years);//Plot als verpachtet markieren <verpachten>

// Troost 20141210 Land markets - type 4, assets traded with land
// Troost **********begin block insert**********
	static int plotCapacitySlots();
	static void setPlotCapacitySlots(int);

	virtual bool attachAsset( int ownerAgentId, int ownerInventoryId, int code, int plotCapacitySlot, double plotCapacityUsed);
	virtual void detachAsset(int userAgentId, int userInventoryId);
	virtual void detachAllAssets();
	virtual void detachAllAssets(int plotCapacitySlot);
	virtual list<attachedAsset> getAttachedAssets();
	virtual list<attachedAsset> getAttachedAssets(int plotCapacitySlot);
	virtual list< pair<int,int> > getAttachedUserAssetIds();
	virtual size_t numAssetsAttached();
	virtual void printAttachedUserAssets();

	virtual double remainingCapacityForAsset(int plotCapacitySlot);
	virtual void changeUserOfAttachedAsset(int ownerAgentId, int ownerInventoryId, int newUserAgentId, int newUserInventoryId);
	virtual void changeOwnerOfAttachedAsset(int ownerAgentId, int ownerInventoryId, int newOwnerAgentId, int newOwnerInventoryId);
// Troost **********end block insert**********


   //TSPC crop-soil model
   virtual int getNruID();
   virtual void setNruID(int);
   virtual void setSoilProc(sProc*& sPro);
   virtual void setLandUseH(const lUseH& ldUs);
   virtual sProc* getSoilProc(void);
   virtual lUseH getLandUseH(void);
   virtual void updateLandUseH(double STN, double RES, double MAN);

   virtual void printToScreen();
	virtual void writeToFile(ofstream& out);
	virtual void writeToFile(FILE* strm);
	virtual void writeToBuffer(char* buffer);

   bool get_hasCrop(void) { return hasCrop;};
   void set_hasCropTrue(void) {hasCrop = true;};
   void set_hasCropFalse(void) {hasCrop = false;};

   /// Generic function to access parcel
   virtual void setByContent(Content cont, double val);
   virtual double getByContent(Content cont);
   
   //void setByContent_monthly(Content cont, int month, double val);//## Tbe eliminate ?
   //double getByContent_monthly(Content cont, int month);//## Tbe eliminate ?

   void set_cropA(int crA) {cropA = crA;};
   int get_cropA(void) {return cropA;};
   void set_crpMixClassInd(int _crop_ind) {crop_ind = _crop_ind;};
   int get_crpMixClassInd(void) {return crop_ind;};

   void set_Precip(double precip_) {precip = precip_;};
   double get_Precip() {return precip;};

   void set_SoilMoisture(double soilMoisture_) {soilMoisture = soilMoisture_;};
   double get_SoilMoisture() {return soilMoisture;};


   virtual void setParcelGroup(parcelGroup* pg);
   virtual parcelGroup* getParcelGroup();



};

class parcelGroup {
		static int numParcelGroups;
		static int maxParcelGroupSize;
		static int minParcelGroupSizeSplit;
		static int doJoinAlongDiagonal;

		set<plot*>memberPlots;
		int userAgentID;
		int ownerAgentID;
		int myID;
		int catchmentID;
		int sectorID;
		double rent;
		int contractDuration;
		double salesValue;
		int salesValueUptodate;
		double typicalRent;
		int typicalRentUptodate;
		int ynofst;
		int numBlocked;
	public:

		parcelGroup();
		parcelGroup(int oid, int uid, int catchm, int sec, int dur_ = 0, double pmt_ = 0.0);
		virtual ~parcelGroup();

		virtual void setUserAgentID(int);
		virtual void setOwnerAgentID(int);
		virtual int getUserAgentID();
		virtual int getOwnerAgentID();
		virtual void addPlot(plot*);
		virtual void removePlot(plot*);
		virtual bool contains(plot*);

		virtual void consolidate(ldscape* fl);
		virtual set<plot*>::iterator firstPlot();
		virtual set<plot*>::iterator afterLastPlot();
		virtual void printToStream(FILE*, int , int);
		virtual size_t getSize() const;
		virtual int getID() const;
		virtual pair<double,double> getCentroid() const;
		virtual vector<int>getTypes() const;
		virtual void setDurationRentalContract(int years);
		virtual void setRentalPayment(double pmt);
		virtual void setDurationFromPlots();
		virtual void setRentFromPlots();
		virtual int getDurationRentalContract() const;
		virtual double getRentalPayment()  const;
		virtual double getRentalPaymentPerPlot()  const;
		virtual void calcSalesValue(double interest);
		virtual double calcTypicalRent();
		virtual double getTypicalRent() const;
		virtual double getSalesValue() const;
		virtual bool includesFarmstead() const;
		virtual int numberBlockedPlots() const;

		static int getMaxParcelGroupSize();
		static void setMaxParcelGroupSize(int m);
		static int getMinParcelGroupSizeSplit();
		static void setMinParcelGroupSizeSplit( int m);
		static int joinAlongDiagonal();
		static void setJoinAlongDiagonal(int i);

// Troost 20141210 Land markets - type 4, assets traded with land
// Troost **********begin block insert**********
		list< pair<int, int> > getAttachedUserAssetIds();
		void printAttachedUserAssets();
		double getFreePlotCapacityForSlot(int slot, int withRented);
// Troost **********end block insert**********
};
struct  parcelGroupSizeSorter
{
		bool operator() ( const parcelGroup* lhs,  const parcelGroup* rhs);

};

struct parcelGroupSalesPriority
{
		bool operator() ( const parcelGroup* lhs, const parcelGroup* rhs);
};
#endif

