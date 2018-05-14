#ifndef XN_MPMAS_OPTIONS_H
#define XN_MPMAS_OPTIONS_H

#include <unistd.h>
#include <vector>
#include <string>
#include "xn_mpmas_interface.h"
#include "xn_mpmas_adaptation.h"

//Begin of Hong
#undef max     // double defined in expertn
#undef min
//End of Hong

using namespace std;

enum xnmpmasCouplingTypes 
{	xnmpmasCouplingFixedMaps = 0,
	xnmpmasCouplingVirtualSlots,
	xnmpmasCouplingOneToOne,
	xnmpmasCouplingTypeLast
};

class xn_mpmas_options {

	xnmpmasCouplingTypes  couplingType;
	std::string fnCellAllocation;
	std::string fnLuaMap;
	std::string fnLuaParameters;
	std::string fnCropGrids;
	
	std::string fnAdaptationParameters;
	
	int weatherRecordsCellSpecific;
	std::string fnCellWeatherLinkMap;
	std::string fnWeatherRecords;

	vector<string> argsXPN;
	vector<string> argsMPMAS;

	int firstXpnYear;
	xnmpmasDate firstSowDateEachSeason;
	xnmpmasDate lastHarvestDateEachSeason;
	int cleanSeason;
	

	int simYears;
	string scenarioName;
	
public:
	xn_mpmas_options(int ac, char **av);
	~xn_mpmas_options();

	//getters
	xnmpmasCouplingTypes getCouplingType()			{ return couplingType;		}
	string getFnCellAllocation ()					{ return fnCellAllocation;	}
	string getFnLuaParameters ()					{ return fnLuaParameters;	}
	string getFnCropGrids ()						{ return fnCropGrids;	}
	string getFnLuaMap ()							{ return fnLuaMap;	}

	string getScenarioName() 						{ return scenarioName; }
	void setScenarioName(string s_)					{ scenarioName  = s_ ;}
	int getNumberOfYearsToSimulate() 				{ return simYears; }
	void setNumberOfYearsToSimulate(int n_) 		{ simYears = n_;}

	string getFnWeatherRecords ()					{ return fnWeatherRecords;	}
	string getFnCellWeatherLinkMap ()				{ return fnCellWeatherLinkMap;	}
	bool areWeatherRecordsCellSpecific()			{ return weatherRecordsCellSpecific; }
	string getFnAdaptationParameters ()				{ return fnAdaptationParameters;}
	vector<string> getMpmasOptions() 				{ return argsMPMAS; 		} 
	vector<string> getXpnOptions()					{ return argsXPN ;  		}		
	int getFirstXpnYear() 							{ return firstXpnYear;		}
	xnmpmasDate getfirstSowingDateEachSeason()		{ return firstSowDateEachSeason;		}
	xnmpmasDate getlastHarvestDateEachSeason()		{ return lastHarvestDateEachSeason; 	}
		
	xnmpmasDate getfirstSowingDateThisSeason(int curyear )		{ 
			xnmpmasDate date = firstSowDateEachSeason;	
			date.year += curyear -1;
			return date;
	}
	xnmpmasDate getlastHarvestDateThisSeason(int curyear)	{ 
			xnmpmasDate date = lastHarvestDateEachSeason;	
			date.year += curyear -1;
			return date;
	}
	xnmpmasDate getfirstSowingDateNextSeason(int curyear )		{ 
			xnmpmasDate date = firstSowDateEachSeason;	
			date.year += curyear ;
			return date;
	}
	int isCleanSeason() {return cleanSeason;}
	
	xnmpmasDate getStartDate()						{ return getfirstSowingDateThisSeason(getFirstXpnYear());}
	
	bool skipMpmasDecisions() { return (couplingType == xnmpmasCouplingFixedMaps);}
};
#endif // XN_MPMAS_OPTIONS_H
