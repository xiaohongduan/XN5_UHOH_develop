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
//  File:      AgentParcelGridCellInterface.h (formerly irrigation.h)
//
//  Contents:  Auxiliary class, links agent parcel list to grid cells,
//             needed for coupling with WASIM-ETH
//
//
//------------------------------------------------------------------------------


#ifndef _AgentParcelGridCellInterface_h
#define _AgentParcelGridCellInterface_h


//#include "AgentParcel.h"
#include "MpmasPlotCell.h"

#ifdef AIX
	#include <unistd.h>
#endif

#ifndef AIX
	#include <dos.h>
#endif /* AIX */



class crpMixClass;
class parPointlist;
class CropWater;


//data structure for crop mix per soil class
class parPointer
{	protected:
	parPointer* nextElem;
	parPointer* prevElem;
	plot* actualPlot;

	public:
	parPointer()   {initialize();};
	
	void initialize()
	{	nextElem=NULL;
		prevElem=NULL;
		actualPlot=NULL;
	};
	~parPointer()
	{
		initialize();
	};
	
	void setPlot(plot* par) {actualPlot = par;};
	plot* getPlot() {return actualPlot;};


	void setNextElem(parPointer* parP) {nextElem = parP;};
	parPointer* getNextElem() {return nextElem;};

	void setPrevElem(parPointer* parP) {prevElem = parP;};
	parPointer* getPrevElem() {return prevElem;};

};

//data structure for crop mix per soil type or NRU
class parPointlist
{	protected:
	int lengthList;
	parPointer* firstElement;
	parPointer* lastElement;

	public:
	parPointlist()
	{	lengthList=0;
		firstElement=NULL;
		lastElement=NULL;
	};
	virtual ~parPointlist();

	virtual void append(plot* currP);
	//Arnold2006_46
	//erasing list of pointers, but leaving parzellen untouched;
	virtual void clearList(void);

	virtual void getAllPositionsOfCell( MatrixDouble& mat);
	virtual void printOnScreen_list();
	virtual void writeToFile(FILE* strm);
	virtual void printDetailedInfoToScreen(CropWater* cropWat, plot* actParc, int m, double val);

	// find position of a parcel "searchParcel" in list p_liste, accordiong to row/col position
	virtual int getPosInList(parPointer* searchPointer);//returns position (from beginning) of parcelle in List

	//Arnold2006_54
	virtual int get_len(void);//returns length of list in memory, but re-calculates each time
	virtual int getLengthOfList(void) {return  lengthList;};

	//Arnold2006_55
	// checks if all parcels are set to "get_hasCrop() == isTrue"
	virtual bool checkIfTrue(bool isTrue);
	virtual bool checkIfTrue(bool isTrue, int cropCnt);

};

//data structure for crop mix per soil type or NRU
class crpMixClass
{	
public:

	/// Access functions
	void  set_sClass(int sClass) { scl = sClass;}
	int   get_nCr() {return nCr;};
	int   get_scl() {return scl;};

	int   get_crA(int cropCnt) {return crA[cropCnt];};
	int   get_LPcol(int cropCnt) {return LPcol[cropCnt];};
	double get_aHa(int cropCnt) {return aHa[cropCnt];};

	void  set_crA(int cropCnt, int crAnew)     {  crA[cropCnt] = (int) crAnew;  };
	void  set_aHa(int cropCnt, double xi) {  aHa[cropCnt] = xi; };
	void  set_LPcol(int cropCnt, int col) {  LPcol[cropCnt] = col; };

	// These should be the only function used!
	// All others should be erased or private
	double getValueByContentAndIndex(Content cont, int cropCnt);   
	void setValueByContentAndIndex(Content cont, int cropCnt, double val);

	// Data only stored in temporary variable, for quicker excess to more variables.
	// Careful: Call updating directly before!
	// ContentType is passed for security check
	double getValueFromTemp_withCropIndex(Content cont, int cropCnt);
	void touchEmpty();
	void setValueByContentAndIndex_ToTemp(Content cont, int cropCnt, double val);

	// Same, but without any checks!
	double get_Temp(int _cropCnt) {return tempContentMonthly[_cropCnt];}
	void set_Temp(int _cropCnt, double val, Content cont); 

	/// Functions for the spatial allocation of land use

	/// ... First some internal information on the status
	double get_aHaRest(int cropCnt) { return aHaRest[cropCnt]; };

	virtual double get_aHaRestTotal();
	virtual int get_RestMaxPos();
	virtual int get_RestMinPos();

	void set_aHaRest(int cropCnt, double xi) 		{  aHaRest[cropCnt] = xi; };
	void reduceRestByOneUnit(int cropCnt) 			;
	void restSubtract(int cropCnt, double val) 	{	aHaRest[cropCnt]-=val;};
	void restAdd(int cropCnt, double val) 			{	aHaRest[cropCnt]+=val;};

	// Check, probably not used any more
	void set_currM(int m) {currM = m;};    //RELEVANT month
	int get_currM(void) {return currM;};   //RELEVANT month

	//Number of parcels that have been allocated (and saved in list)
	void add_lenPlotList(int cropCnt) { lenPlotList[cropCnt] ++; };
	void subtr_lenPlotList(int cropCnt) { lenPlotList[cropCnt] --; };
	int get_lenPlotList(int cropCnt) { return lenPlotList[cropCnt];   };
	void reset_lenPlotList(int cropCnt) {  lenPlotList[cropCnt]=0; };

	// Parcel list functions ...
	virtual void appendPlotAllocated(int idx, plot* parcel);
	virtual void clearPlotList();

	void set_IPGvec(int cropCnt, int g) { IPGvec[cropCnt] = g; };
	int get_IPGvec(int cropCnt) { return IPGvec[cropCnt]; };

	//Add total area cropped / Rest
	virtual void addAreaCropped(void);
	virtual void addRest(void);

	/// Output functions
	virtual void printToScreen(void);
	virtual void printOnScreen_list(int cropCnt);
	virtual void printPlots(int cropCnt);
	virtual void writeToFile(FILE* strm, int mixID);
	virtual void writePlotListArrayToFile(FILE* strm, int mixID);

	// checks if all parcels in crpMixClass are set to "get_hasCrop() == true"
	virtual void checkIfTrue(int cropCnt, bool isTrue);
	virtual void getAllPositionsOfCell(int cropCnt, MatrixDouble& mat);

	/// Constructor and destructor
	crpMixClass(void);
	crpMixClass(int soil);

	virtual ~crpMixClass();   
	virtual void initEmpty(void);
	virtual void allocate(int numCropActs);
	virtual void deallocate();
	virtual void addCropActivityToMix(int crA, int lpCol, int prior, double area);

protected:

	//general data of soil type
	int scl;		//ID of soil type or NRU
	int nCr;    //number of cropping activities of agent
	int currM;  //Remembers current month (RELEVANT counting;
					// to get real month:

	//tmp, during allocation
	double areaTot;    // Total area that is cropped
	double restTot;    // Total area that is not yet allocated spatially

	//data per cropping activity
	int* crA;		//cropping activity ID
	int* LPcol;		//Index into LP
	int* IPGvec;	//Vector of (irrigation) priority group of each activity
	double* aHa;	//hectares of cropping activities
	double* aHaRest;//hectares of cropping activities that is not yet allocated

	//Arnold2006_31
	int* lenPlotList;
	parPointlist* plotListArray;  //vector of plot*

	Content contentTemp;
	double* tempContentMonthly;//hectares of cropping activities that is not yet allocated

};

#endif

