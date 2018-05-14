#ifndef XN_MPMAS_INTERFACE_H_
#define XN_MPMAS_INTERFACE_H_

#define XNCROPCODELENGTH 2
#define XNFERTCODELENGTH 5
#define XNORGFERTCODELENGTH 19
#define XNNAMELENGTH 63
#define XNMPMASMINFERTSLOTS 4
#define XNMPMASORGFERTSLOTS 2
#define XNMPMASIRRIGATIONSLOTS 10

#define XNMPMASDAYSOFYEAR 366

//XNMPMASVERSION 9


typedef struct {
	int day;
	int month;
	int year;
} xnmpmasDate;

typedef struct {
	xnmpmasDate fertDate;
	char code[XNFERTCODELENGTH + 1];
	double no3n; //kg N
	double nh4n;//kg N
	double urea;//kg N
	int adaptive;
	int bbch;
} xnmpmasMinFert;

typedef struct {
	xnmpmasDate orgfertDate;
	char code [XNORGFERTCODELENGTH + 1];
	double drymatter; //kg
	double nh4n; // kg N
	double org_subst;//kg 
	int adaptive;
	int bbch;
} xnmpmasOrgFert;

typedef struct {
	xnmpmasDate irrDate;
	double quantity; //mm
} xnmpmasIrrigation;

typedef struct  {
//Simulation end
	xnmpmasDate stopDate;
//Crop rotation
	char CropCode[XNCROPCODELENGTH + 1];
	char CropName[XNNAMELENGTH +1 ];
	xnmpmasDate sowDate;
	char variety[XNNAMELENGTH + 1];
	double sowDens;
	double rowDist; 
	//double rootDepth;//?? better define according to soil in default_crop_rotation.ini file
	double sowDepth;
	xnmpmasDate harvestDate; //record yield data here

	int harvestAdaptive;
	int harvestBBCH1;
//NEW:	
	int harvestBBCH1ExtraDays;
//END NEW	
	int harvestBBCH2;
	int harvestBBCH2ExtraDays;

	int biom_remove;
//Fertilization
	int numMinFert;
	xnmpmasMinFert mineralFertilization[XNMPMASMINFERTSLOTS];
	int numOrgFert;
	xnmpmasOrgFert organicFertilization[XNMPMASORGFERTSLOTS];
	int numIrrig;
	xnmpmasIrrigation irrigation[XNMPMASIRRIGATIONSLOTS];
} STRUCT_mpmas_to_xn;


typedef struct {
	double fruitDryWeight; // kg/ha
	double stemLeafDryWeight;
	double Nmin0_30;
	double Nmin30_60;
	double Nmin60_90;
	//Bodenfeuchte (klären welche Schichten)
} STRUCT_xn_to_mpmas;

//NEW:
//not sure, just a suggestion for access to records
typedef struct {
  double airTemp[XNMPMASDAYSOFYEAR];
  int startDay;
  double topsoilTemp[XNMPMASDAYSOFYEAR]; 
  int stopDay;
} STRUCT_xn_to_mpmas2; 	
//

#endif
