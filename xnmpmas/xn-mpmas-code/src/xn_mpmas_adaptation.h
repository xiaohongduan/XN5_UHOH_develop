#ifndef XN_MPMAS_ADAPTATION_H_
#define XN_MPMAS_ADAPTATION_H_

#include "xn_mpmas_interface.h"

//Begin of Hong
#undef max // double defined in expertn
#undef min
//End of Hong

#include <deque>
#include <vector>

namespace xnmpmas
{ namespace adaptation 
{
//1) Typedefs
	enum sowingDayFormulaType 
	{
		notAdaptive = -1,	
		springSoilGdd = 0,
		springSoil = 1,
		springGdd = 2,
		winterGdd = 3,
		undefSowingDayFormula
	};

	struct cropAdaptationParameters { 
		//Sowing parameters
		sowingDayFormulaType sowingDayFormula;
		double basetemp; //base temperature for degree day calculation
		double seed_tgzthr; //threshold soil temperature
		double seed_gddthr;	//threshold degree days
		double seed_cons; //regression constant
		double seed_coef_tgz; //coefficient soil temp
		double seed_coef_gdd; //coefficient degree days

		//Fertilization parameters
		double fert_leach;
		double fert_Ncontent;
		//double fertNsh[XNMPMASMINFERTSLOTS];

	//Harvest parameters
	/* included in STRUCT_mpmas_to_xn in "xn_mpmas_interface.h"
	 int harvestBBCH1; // BBCH stage at which harvest should take place
	 int harvestBBCH1ExtraDays; //days to wait with harvest after BBCH has been reached
	 int harvestBBCH2; //// BBCH stage at which harvest may take place, in case BBCH1 is never reached
	 int harvestBBCH2ExtraDays; ////days to wait with harvest after BBCH2 has been reached 
	*/

	};

	//Saving and using past weather
	struct weatherRecord {
		 int year; //for which year the record was recorded
		 int numDays; //Highest index with valid data, e.g 365 or 366 days for full year, or less if year has not yet been fully recorded
		 double airTemp [XNMPMASDAYSOFYEAR]; // air temperature 2 m for 366 days of year
		 double topsoilTemp [XNMPMASDAYSOFYEAR]; //soil temperature 5cm for 366 days of year
		 /* Important:  Indices start at first of January here!  */ 
		 
		 //constructor
		 weatherRecord() : year(-1),  numDays(0) {} ; //initializes indicator for days with meaningful data to zero
		 weatherRecord(int y) : year(y), numDays(0) {} ; //initializes indicator for days with meaningful data to zero
	};

//2) Functions
	//Determine sowing date before season starts based on weather history and weighting schema
	//Allows different types of formulae to be used.
	int calculatePlantingDate(	const cropAdaptationParameters* params, 
								const std::deque<weatherRecord> cellHistory, 
								const std::vector<double> historyWeighting );

	void calculateFertilizerQuantity (	const cropAdaptationParameters* params, 
										const double expectedYield,  
										STRUCT_mpmas_to_xn& adaptedManagement);

	int isSpringCrop ( const sowingDayFormulaType type );

} //end namespace adaptation
} //end namespace xnmpmas
#endif