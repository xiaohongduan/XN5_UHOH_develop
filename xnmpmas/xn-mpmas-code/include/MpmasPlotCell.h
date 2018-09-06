/*
 * MpmasPlotCell.h
 *
 *  Created on: Jan 8, 2016
 *      Author: troost
 */

#ifndef MPMASPLOTCELL_H_
#define MPMASPLOTCELL_H_

#include <set>
#include <list>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include "BasicMath.h"
#include "MatrixDouble.h"
#include "CropSoil.h"

//forward declarations
class parcelGroup;
class ldscape;
class agent;

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

// merged class: former cell and plot
class mpmasPlotCell
{
	protected:
	bool isInitializedWithData;
	int cellFlagIdentifiedByOwner;   //flag: plot identified by owner

	int x;      //x-coordinate (column index in sector map)
   int y;      //y-coordinate (row index)

	//int cellFarmID;	//farmstead ID, must be identical with agent plot ID (cellAgentID = cellFarmID)//## Tbe 110423 substitute
	int farmsteadID;	//farmstead ID in map

	//int cellAgentID;	//agent plot ID, must be identical with farmstead ID (cellAgentID = cellFarmID)//## Tbe 110423 substitute
	int ownerMapID;	//owner ID of plots in map (fstID)
	int b_nr; 	//agentID of owner (agID)

	int cellUserMapID; //user map ID (operator, not necessarily owner, can also be renter) //TODO: check here!!!
	int renterID;//i.e. agent renting-in: -1 indicates parcel not rented-out, -2 indicates parcel returned to owner

	double cellDistanceHome;//Euklidean distance of plot from farmstead
	int cellDirectionHome;//Angle (degrees) from farmstead (0 = 360 = North; 90 = East)
	//double entf;//distance to farmstead

	int cellSoilType;  //soil type,    int typ;		//soil type
	int nruID;  //ID nutrient response unit

// Troost 20160331 Soil type transitions
// Troost **********begin block insert**********
#ifdef MULTIPERIOD
	int cellSoilTypeNextSeason;
#endif
// Troost **********end block insert**********

	int color;         //color of cell user's plots

	int cellPopID;     //population ID of owner
	int cellCluID;     //cluster ID of owner
	int cellNetwID;    //network ID of owner
	int cellSectorID;  //used when migrating cells to catchment    //int p_sk;	//sector ID of parcel
	int cellCatchID;	//int p_sc;	//subcatchment ID, renamed from <b_sc>

	double * cellUserDefined; //user defined landscape parameters
	parcelGroup* parcelGroupPtr;

	int plotStatus;//moegliche Zustaende: 0 brach, 1 Hofstelle, 2 Dauerkultur, 3 einjaehrige Kultur, 4 verpachtet

	int durationRentalContract;
	double rentalPayment;

	list<attachedAsset> attachedAssets;
	double* capacityForAssets;
	static int plot_capacity_slots;

	int plotMarkedForRedistribution;

   //Spatial allocation of cropping activities
   bool hasCrop;      // parcel was allocated to a specific cropping activity
   int  cropAct;        // ID for specific cropping activity //	int cropAct;     // Crop activity

   int  previousCropAct;        // ID for specific cropping activity of previous year (for overlapping seasons)
   int  previousNRU;        // ID of NRU of previous year (for overlapping seasons with dynamic soil types)

   int crop_ind;      // index into CropMixClass
	double cropYield;
	double stoverYield;
#ifdef MULTIPERIOD
	vector<double> extraCropActAttributes;
#endif


#ifdef LIVSIM_COUPLING
	//double feedState;
	double cumulatedGrazingPressure;
#endif

   //TSPC crop-soil module
	// Pointer to soilC
	soilC* sCon;   //soil conditions
   lUseH luHis;//land use history
   sProc* slPro;//soil processes

	double cellPrecip; //cell precipitation
  // double precip;
 //  double soilMoisture;

	/// FOR COUPLING
	// ... dimension: ( 1 X (contENDCOUPL - contENDIRRIG + 1) )
	// contains information for
	// ... contSoilMoist, contSoilMoist05, contSoilMoist1, contETpot, contETreal,
	// ... contPrecip,	contSurfaceRunoff, contPercol, contBaseFlow, contGroundwaterRecharge,
	MatrixDouble couplVector;

	// (returns zero if not relevant)
	// ... automatically allocated if called first time, automatically destructed.
	// ... Dimensions: contENDMONTHLY-contENDCOUPLTABLE X 12
	MatrixDouble monthlyCoupl;

	// Friend!
	friend class ldscape;

private:
	virtual void setParcelGroup(parcelGroup* pg);

	friend class parcelGroup;

public:
	//Location
	virtual int get_x() const;//plot
	virtual int get_y() const;//plot

	virtual int get_col();//plot
	virtual int get_row();//plot

	virtual int getSectorID(void);//cell
	virtual void setSectorID(double in);//cell
	virtual void setSectorID(int in);//cell
	virtual int get_psk();//plot

	virtual int get_psc();//plot
	virtual int getCatchmentOfOwner() {	return cellCatchID;}//identical with <get_psc()>//plot



	bool isInitialized (void)	{ return isInitializedWithData;};//cell

	virtual parcelGroup* getParcelGroup();
	//generic content functions

	virtual double getByContent(Content cont);//cell
	virtual void setByContent(Content cont,  int val);//cell
	virtual void setByContent(Content cont,  double val);//cell

	virtual double getByContent_monthly(Content cont, int m);//cell
	virtual void setByContent_monthly(Content cont, int m, int val);//cell
	virtual void setByContent_monthly(Content cont, int m, double val);//cell


	//Soil type and user defined characteristics
	virtual int get_typ();//plot
	virtual void set_typ(int);//plot
	virtual int getType(void);//cell
	virtual void setSoilType(int cellSoilType);//cell
// Troost 20160331 Soil type transitions
// Troost **********begin block insert**********
#ifdef MULTIPERIOD
	virtual void setSoilTypeNextSeason(int s);
	virtual void doSoilTypeTransition();
#endif
// Troost **********end block insert**********
	virtual double getUserDefinedContent(int userContentId);//cell
	virtual void setUserDefinedContent(int userContentId, double val);//cell

	virtual int getNruID();//plot
	virtual void setNruID(int);//plot

	//Characteristics related to user and owner

	//if plot harbors farmstead
	virtual int getFarmsteadID();//cell
	virtual int getPpID(void);//cell
	virtual int getClustID(void);//cell
	virtual int getNetwID(void);//cell

	virtual void setFarmsteadID(int f);//cell
	virtual void setPpID(int ppID);//cell
	virtual void setClusterID(int cellCluID);//cell
	virtual void setNetworkID(int nwID);//cell

	//Ownership, user, rental
	virtual int getOwnerMapID();//cell
	virtual void setOwnerMapID(int a);//cell

	virtual int getAgentOwnerID();//plot
	virtual int get_bnr();//plot
	virtual void set_bnr(int);//plot

	virtual int getUserMapID(void);//cell
	virtual void setUserMapID(int usID);//cell

	virtual int getRenterID();//plot
	virtual void setRenterID(int userID);//plot

	virtual double getDistanceFromHome(void);//cell
	virtual void setDistanceFromHome(double cellDistanceHome);//cell
	virtual double get_entf();//plot
	virtual void set_entf(double);//plot

	virtual double getDirectionFromHome(void);//cell
	virtual void setDirectionFromHome(int);//cell

	virtual int getFlagOwnerOK(void);//cell
	virtual void setFlagOwnerOK(int cellFlagIdentifiedByOwner);//cell

	virtual int getDurationRentalContract();//plot
	virtual double getRentalPayment();//plot
	virtual void setDurationRentalContract(int years);//plot
	virtual void setRentalPayment(double pmt);//plot

	virtual int getPlotStatus();//plot
	virtual void setStatusFree();//Plot auf Brache setzen <raeumen>//plot
	virtual void setStatusFarmstead();//Plot mit Hofstelle besetzen <hofstelle>//plot
	virtual void setStatusPermanentCrop();//Plot mit Dauerkultur belegen <belegen>//plot
	virtual void setStatusRented(int years);//Plot als verpachtet markieren <verpachten>//plot

	///attached assets
	static int plotCapacitySlots();//plot
	static void setPlotCapacitySlots(int);//plot

	virtual bool attachAsset( int ownerAgentId, int ownerInventoryId, int code, int plotCapacitySlot, double plotCapacityUsed);//plot
	virtual void detachAsset(int userAgentId, int userInventoryId);//plot
	virtual void partlyDetachAsset(int userAgentId, int userInventoryId, double reduction);
	virtual void detachAllAssets();//plot
	virtual void detachAllAssets(int plotCapacitySlot);//plot
	virtual list<attachedAsset> getAttachedAssets();//plot
	virtual list<attachedAsset> getAttachedAssets(int plotCapacitySlot);//plot
	virtual list< pair<int,int> > getAttachedUserAssetIds();//plot
	virtual size_t numAssetsAttached();//plot
	virtual void printAttachedUserAssets();//plot

	virtual double remainingCapacityForAsset(int plotCapacitySlot);//plot
	virtual void changeUserOfAttachedAsset(int ownerAgentId, int ownerInventoryId, int newUserAgentId, int newUserInventoryId);//plot
	virtual void changeOwnerOfAttachedAsset(int ownerAgentId, int ownerInventoryId, int newOwnerAgentId, int newOwnerInventoryId);//plot

	// ----------------------------------------
	/// Output: Printing
	// ----------------------------------------
	virtual void printToScreen(void);//cell
	virtual void printToScreenPlot(void);//cell
	virtual void printToScreenBasic(void);//cell
	virtual void printOnScreenCropped(void);//cell
	virtual void printOnScreenIrrig(void);//cell
	virtual void printOnScreenTSPC(void);//cell

	virtual void writeToFile(ofstream& out);//plot
	virtual void writeToFile(FILE* strm);//plot
	virtual void writeToBuffer(char* buffer);//plot

	virtual int getColor(void);//cell
	virtual void setColor(int color);//cell

	// TSPC
	virtual soilC* getSCon(void);//cell
	virtual void setSCon(soilC* sCon);//cell

	virtual double getSoilN(void);//cell
	virtual double getSoilP(void);//cell
	virtual double getSoilK(void);//cell
	virtual double getSoilA(void);//cell
	virtual double getSoilC(void);//cell
	virtual double getSoilSl(void);//cell
	virtual double getSoilSe(void);//cell

	virtual void setSoilN(double);//cell
	virtual void setSoilP(double);//cell
	virtual void setSoilK(double);//cell
	virtual void setSoilA(double);//cell
	virtual void setSoilC(double);//cell
	virtual void setSoilSl(double);//cell
	virtual void setSoilSe(double);//cell

	virtual void setSoilProc(sProc*& sPro);//plot
	virtual void setLandUseH(const lUseH& ldUs);//plot
	virtual sProc* getSoilProc(void);//plot
	virtual lUseH getLandUseH(void);//plot
	virtual void updateLandUseH(double STN, double RES, double MAN);//plot

	// CELL
	// Receives ACTUAL (real) months, but internally saves as reduced ("relevant") months.
	// Internal conversion.

	// For wasim coupling: Access to vector with exchanged data
	virtual double getCouplingValue(Content cont);// CELL
	virtual void setCouplingValue(Content cont, double val);// CELL

	// ----------------------------------------
	///Linked list functions
	// ----------------------------------------
	mpmasPlotCell* nextPlot;//plot
	mpmasPlotCell* prevPlot;//plot

	void setNextPtr_P (mpmasPlotCell*);//plot
	mpmasPlotCell* getNext_P();//plot

	void setPrevPtr_P (mpmasPlotCell*);//plot
	mpmasPlotCell* getPrev_P();//plot


	//-------------------------------------------
	/// Cropping
   //-------------------------------------------
	bool get_hasCrop(void) { return hasCrop;};//plot
	void set_hasCropTrue(void) {hasCrop = true;};//plot
	void set_hasCropFalse(void) {hasCrop = false;};//plot

	void set_cropA(int crA) {cropAct = crA;};//plot
	int get_cropA(void) {return cropAct;};//plot
	void set_crpMixClassInd(int _crop_ind) {crop_ind = _crop_ind;};//plot
	int get_crpMixClassInd(void) {return crop_ind;};//plot
	virtual int  getCropActivityID(void);//cell
	virtual void setCropActivityID(int crA);//cell

	virtual int  getPreviousCropActivityID(void);//cell
	virtual void setPreviousCropActivityID(int crA);//cell
	virtual int  getPreviousNruID(void);//cell
	virtual void setPreviousNruID(int n_);//cell

	virtual double getCropYield(void);//cell
	virtual void setCropYield(double value);//cell
	virtual double getStoverYield(void);//cell
	virtual void setStoverYield(double value);//cell

	void set_Precip(double precip_) {cellPrecip = precip_;};//plot
	double get_Precip() {return cellPrecip;};//plot


	void markThisPlotForRedistribution();
	void unmarkThisPlotForRedistribution();
	int getPlotMarkedForRedistribution();

#ifdef LIVSIM_COUPLING
	//double getFeedState(]{return feedState;}
	double getCumulatedGrazingPressure() const {return cumulatedGrazingPressure;}
	//void setFeedState(double x){feedState = x;}
	void setCumulatedGrazingPressure(double x ){cumulatedGrazingPressure = x;}
	void addToCumulatedGrazingPressure(double x ){cumulatedGrazingPressure += x;}
#endif

#ifdef MULTIPERIOD
	double getExtraCropActAttribute(int i);
	void setExtraCropActAttribute(int i, double value);
#endif


//	virtual double getPrecip();//cell
//	virtual void setPrecip(double val);//cell

//	void set_SoilMoisture(double soilMoisture_) {soilMoisture = soilMoisture_;};//plot
//	double get_SoilMoisture() {return soilMoisture;};//plot

//--------------------
///Initialization
//--------------------
	//Uebergabe: x-Koord., y-Koord., Bodentyp, Betriebsnr., SubcatchmentNum, Sektornr., Zustand, Entfernung
	//plot(int, int, int, int, int, int, int = 1, double = 0.0, parcelGroup* pg = NULL);
	mpmasPlotCell(int _x = -1, int _y = -1, int _catchID = -1, int _skID = -1);

	virtual void setEmptyAll();
	// calls:
	virtual void setEmptyBasic(void);
	virtual void setEmptyIrrig(void);
	virtual void setEmptyTSPC(void);


	void setOwner(agent* ag);
	void setOwnerAndUser(agent* ag);
	void setRenter(agent* ag);
	void setUser(agent* ag);


	//--------------------
	///Destruction
	//--------------------
	virtual ~mpmasPlotCell();
	virtual void deallocate(void);

};

typedef mpmasPlotCell plot ; //to avoid having to change all names by hand
typedef mpmasPlotCell cell ;


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
		virtual void addPlotAndInitWithOwner(mpmasPlotCell* p, agent* ag);
		virtual void addPlot(plot* p);
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

struct plotSetSorter
{
		bool operator() ( const mpmasPlotCell* lhs, const mpmasPlotCell* rhs);
};

#ifdef LIVSIM_COUPLING
	inline bool plotGrazingPressureSort ( const mpmasPlotCell* lhs, const mpmasPlotCell* rhs) { return (lhs->getCumulatedGrazingPressure() < rhs->getCumulatedGrazingPressure() ); }
#endif
#endif /* MPMASPLOTCELL_H_ */
