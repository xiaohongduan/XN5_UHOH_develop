#include "xn_mpmas_options.h"
#include <exception>
#include <stdexcept>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <iostream>
#include <cstring>

using namespace std;

vector<string>tokenize_string (const string str) 
{
	vector<string>tokens;
	string newToken = string();
	
	int counter = 0;
	for( size_t i=0; i<str.length(); i++){

		char c = str[i];
		if( c == ' ' )
		{	if(counter == 0) 
			{	tokens.push_back(newToken);
				newToken = string();
				cout << endl;
			}
			++counter;
		}
		else if(c == '\"' )
		{	counter = 0;
			i++;
			while( str[i] != '\"' ){ 
				 newToken.append(1, str[i]); 
				i++; 
			}
		}
		else
		{	counter = 0;
			cout << c;
			newToken.append(1, c);
		}
	}
	if (newToken.length() > 0)
		tokens.push_back(newToken);
	
	cout << endl;
		
	return tokens;
}


xn_mpmas_options::xn_mpmas_options(int ac, char **av): sowDateRepresentative(vector<int>(2,-1))
{
	if (ac < 2)
	{	std::stringstream errmsg;
		errmsg << "Error in program call:\nUsage "<< av[0] <<" <configuration.ini>\n";
		throw runtime_error(errmsg.str());
	}

	int i = 1;
	string couplingIniName = "";
	int overriddenSimYears = -1;
	int overridenStopBeforeXnFinal = false;
	produceILMScouplingMaps = false;
	
	while (i < ac)
	{
		if (strncmp("-", av[i], 1) ) // does not start with -, not an option but coupling file (arguments to options are consumed by the option itself
		{ 
			couplingIniName = av[i];
		}
		else 
		{
			if ( strlen(av[i]) < 2) 
			{
				std::stringstream errmsg;
				errmsg << "Error invalid command line argument:" <<  av[i] << "\n";
				throw runtime_error(errmsg.str());
			}	
			switch (av[i][1])
			{
				case 'Y': 
					if (i +1 >= ac)
					{
						std::stringstream errmsg;
						errmsg << "Error in command line argument: -Y  without following number\n";
						throw runtime_error(errmsg.str());
					} 
					overriddenSimYears =  atoi(av[i+1]);
					++i; //advance counter one further to skip number used as argument to Y
					break;
				case 'D': 
					overridenStopBeforeXnFinal = true;
					break;
				case 'C': 
					produceILMScouplingMaps = true;
					break;
					
				default:
				{ std::stringstream errmsg;
				  errmsg << "Error invalid command line argument:" <<  av[i] << "\n";
				 throw runtime_error(errmsg.str());
				}
				break;
			}
		}
		
		++i;
	}



	// use boost::property_tree to read ini file and split into section.key nodes
	boost::property_tree::ptree ini_entries;
	boost::property_tree::ini_parser::read_ini(couplingIniName, ini_entries);
 	int temp1 = ini_entries.get<int>("Coupling.couplingType");
	if (temp1 < 0 || temp1 >= xnmpmasCouplingTypeLast)
	{	
		std::stringstream errmsg;
		errmsg << "Error in  "<< av[1]<< ": Invalid couplingType\n";
		throw runtime_error(errmsg.str());
	}	
	


	//transfer section.keys nodes into values
	couplingType = (xnmpmasCouplingTypes) temp1;

	if (couplingType == xnmpmasCouplingVirtualSlots) 
		fnCellAllocation = ini_entries.get<string>("Coupling.cellAllocationFile");
	else
		fnCellAllocation = "";
	if 	(couplingType == xnmpmasCouplingFixedMaps) 
		fnLuaMap = ini_entries.get<string>("Coupling.luaMapFileStub");
	else
		fnLuaMap = "";
		
	firstXpnYear = ini_entries.get<int>("XPN.startYear");
	
	fnLuaParameters = ini_entries.get<string>("Common.luaParametersFile");
	firstSowDateEachSeason.day = ini_entries.get<int>("Common.firstSowingDay");
	firstSowDateEachSeason.month = ini_entries.get<int>("Common.firstSowingMonth");
	firstSowDateEachSeason.year = ini_entries.get<int>("Common.firstSowingRelativeYear");
	
	lastHarvestDateEachSeason.day = ini_entries.get<int>("Common.lastHarvestDay");
	lastHarvestDateEachSeason.month = ini_entries.get<int>("Common.lastHarvestMonth");
	lastHarvestDateEachSeason.year = ini_entries.get<int>("Common.lastHarvestRelativeYear");	

	
	fnCropGrids = ini_entries.get("XPN.cropGridFile", "");

	
	//if we have a clean season, no crops for next season will have to be planted while crops of this season have not yet been harvested
	int longdateNextSeasonSowing = (firstSowDateEachSeason.year +1) * 10000 + firstSowDateEachSeason.month * 100 + firstSowDateEachSeason.day;
	int longdateThisSeasonHarvest = lastHarvestDateEachSeason.year * 10000 + lastHarvestDateEachSeason.month * 100 + lastHarvestDateEachSeason.day;
	if (longdateThisSeasonHarvest <= longdateNextSeasonSowing)
		cleanSeason = true;	
	else
		cleanSeason = false;
		
	//tokenize and transform command lines into string vectors	
	argsMPMAS.clear();
	argsMPMAS.push_back(av[0]);
	vector<string> tokenizedArgumentString = tokenize_string(ini_entries.get<string>("MPMAS.commandline"));
	argsMPMAS.insert(argsMPMAS.begin() + 1, tokenizedArgumentString.begin(), tokenizedArgumentString.end());

	argsXPN.clear();
	argsXPN.push_back(av[0]);
	vector<string> tokenizedArgumentString2 = tokenize_string(ini_entries.get<string>("XPN.commandline"));
	argsXPN.insert( argsXPN.begin() + 1, tokenizedArgumentString2.begin(), tokenizedArgumentString2.end());
	
	weatherRecordsCellSpecific = ini_entries.get<int>("Adaptation.weatherRecordsCellSpecific");
	if(!weatherRecordsCellSpecific)
	{
		fnCellWeatherLinkMap = ini_entries.get<string>("Adaptation.cellWeatherRecordsLinkMap");		
	}
	fnWeatherRecords = ini_entries.get<string>("Adaptation.weatherRecordsFile");
	fnAdaptationParameters = ini_entries.get<string>("Adaptation.generalParametersFile");	

	if (overriddenSimYears > -1)
		simYears = overriddenSimYears;
	else
		simYears = ini_entries.get("Common.simYears",1);
		
	if(overridenStopBeforeXnFinal)
			stopBeforeXNinFinalPeriod = true;
	else
			stopBeforeXNinFinalPeriod = ini_entries.get("Common.stopBeforeXNinFinalPeriod",0);
			
	if ( !produceILMScouplingMaps	)
		produceILMScouplingMaps = ini_entries.get("Coupling.produceILMScouplingMaps",0);
			
				
	scenarioName = ini_entries.get("Common.scenarioName","testxnmas_");

	
	sowDateRepresentative[0] = ini_entries.get("CouplingILMS.luaID_sowing_date_early",  -1 );
	sowDateRepresentative[1] = ini_entries.get("CouplingILMS.luaID_sowing_date_late",  -1 );

}

xn_mpmas_options::~xn_mpmas_options()
{

}

