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


typedef struct xnmpmasDate {
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
	double Nmin;
	//Bodenfeuchte (klären welche Schichten)
} STRUCT_xn_to_mpmas;

//NEW:
//not sure, just a suggestion for access to records
typedef struct {
  double airTemp[XNMPMASDAYSOFYEAR];
  int numDaysAirTemp;
  double topsoilTemp[XNMPMASDAYSOFYEAR]; 
  int numDaystopSoilTemp;
} STRUCT_xn_to_mpmas2; 	
//



/*//NEW:
//----------------------------------------------------
//Determine if crop is to be harvested today
//function to be called once a day
//return codes: 0 - no, 1- yes 
//requires a counter 'daysSinceBBCH2' to be maintained within caller
int checkIfHarvest(STRUCT_mpmas_to_xn* management, int currentBBCH, /* NEW:*/ /* int* daysSinceBBCH1, /*END NEW*/ /*int* daysSinceBBCH2, xnmpmasDate currentDate) {

	//first check fixed harvest date and return 1 if date has been reached
	//If the decision is not marked as adaptive, it is the only criterion
	//If the decision is adaptive it serves as a backstop and should be set to the latest possible harvest
	if (currentDate.day == management->harvestDate.day 
		&& currentDate.month == management->harvestDate.month 
		&& currentDate.year == management->harvestDate.year ) {
		return 1;
	}
	//If the decision is not adaptive, no harvest at this day
	if ( ! (management->harvestAdaptive) ) {	
		return 0;
	}
	//If the decision is adaptive ...
	else {
		//check whether maturity (BBCH1) has been reached
		if (currentBBCH >= management->harvestBBCH1 ) {
//NEW:
			if (daysSinceBBCH1 >= harvestBBCH1ExtraDays) {
				return 1;
			}
			//if we are > BBCH1, but not over the limit, increase the counter of days
			else {
				daysSinceBBCH1 += 1:
			}
			//deleted: return 0
//END NEW
		}
		//if maturity has not been reached, but we are close (> BBCH2)
		// check the number of days for which we have been > BBCH2
		// and harvest in case we are over the limit given for that stage 
		//(this is because in some cases the actual maturity will not be reached at all)
		else if (currentBBCH >= management->harvestBBCH2 ) {
			if (daysSinceBBCH2 >= harvestBBCH2ExtraDays) {
				return 1;
			}
			//if we are > BBCH2, but not over the limit, increase the counter of days
			else {
				daysSinceBBCH2 += 1:
				return 0;
			}
		}
	}
	return 0;
}
//---------------------------------------
//Determine if next mineral fertilizing event is to take place today
//function to be called once a day
//return codes: 0 - no, 1- yes
//assumes that the counter which fertilization action is next 'nextMinFertAction' is maintained by the caller
int checkIfMineralFertilization(STRUCT_mpmas_to_xn* management, int nextMinFertAction, int currentBBCH, xnmpmasDate currentDate) {

	//security check to make sure the next fertilization action is a valid action
	if (nextMinFertAction >= XNMPMASMINFERTSLOTS || nextMinFertAction >= management->numMinFert ) {
		return 0;
	}
	//if timing of fertilization is adaptive, check whether indicated BBCH stage has been reached
	if ( management->mineralFertilization[nextMinFertAction].adaptive ) {	
		if (currentBBCH >= management->mineralFertilization[nextMinFertAction].bbch)
			return 1;
		else
			return 0;
	}
	//else if timing of fertilization is fixed, check whether indicated date has been reached
	else {
		if (currentDate.day == management->harvestDate.day 
			&& currentDate.month == management->harvestDate.month 
			&& currentDate.year == management->harvestDate.year ) {
			return 1;
		}

		return 0;
	}
	return 0; //actually never reached
}*/
//END NEW*/
#endif
