#include "xn_mpmas_translator.h"
#include <cstdio>
#include <cstring>
#include <exception>
#include <stdexcept>
#include "yaml-cpp/yaml.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include <iomanip>
#include <map>
#include "Raster2D.h"




using namespace xnmpmas::coupling;
using namespace xnmpmas::adaptation;
using namespace std;

namespace xnmpmas
{ 	namespace coupling
	{
		int daysTillMonth[2][12] =
		{
			{ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 },
			{ 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 },
		};
		
		bool isLeapYear( int year )
		{
			return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
		};
		
		void extractDateFromString(const string input, xnmpmasDate& output, char delimiter = '-')
		{
			stringstream dateS (input);
			unsigned int day, month, year;			
			dateS >> day;
			dateS.ignore(numeric_limits<streamsize>::max(), delimiter);
			dateS >> month;
			dateS.ignore(numeric_limits<streamsize>::max(), delimiter);
			dateS >> year;
			
			output.day = day;
			output.month = month;
			output.year = year;
		}
		xnmpmasDate convertDayOfYearToDate (int doy, int year)
		{
			xnmpmasDate date;
			date.year = year;
			bool leap = isLeapYear( date.year );
			int i = 1;
			while (  i < 12 && doy > daysTillMonth[leap][i] )
				++i;
			date.month = i;
			date.day = doy - daysTillMonth[leap][i-1];
			return date;
		}
		int convertDateToDayOfYear (xnmpmasDate date)
		{
			if (date.month <= 0 || date.day <= 0)
				return 0;
			return daysTillMonth[ isLeapYear( date.year ) ][date.month - 1 ] + date.day;
		}
		
		template <typename T> T getValueFromYamlNode( const YAML::Node _node, std::string filename, std::string description, int position = -99)
		{	T value;
			try
			{	value = _node.as<T>();
			}
			catch (exception &e)
			{	stringstream errortext;
				errortext << e.what() << "\n" << "Error in " <<filename << " when trying to read " << description ;
				if(position != -99)
					errortext << position;
				errortext <<  endl;
				throw runtime_error(errortext.str());
			}
			return value;
		}
		
	}//end namespace coupling	
}//end namespace xnmpmas

void xn_mpmas_translator::checkFscanfSuccess(int rtcod, int toBeRead, const char* fn, char* messageOnError)
{
	if (rtcod != toBeRead)
	{
		std::stringstream errmsg ;
		errmsg << "Error when reading "<< fn << "\n" << messageOnError <<"\n"  ;
		throw runtime_error(errmsg.str());
	}
};

xn_mpmas_translator::xn_mpmas_translator(xnmpmasCouplingTypes type_, const char* fnLuaProps_, const char* fnCropGrids_, const char* fnCells_, xnmpmasDate startDate_)
{
	int rtcod = 0;

	startDate.day = startDate_.day;
	startDate.month = startDate_.month;
	startDate.year = startDate_.year;
	
	//set coupling type
	if (type_ >= xnmpmasCouplingTypeLast || type_ < 0)
	{
		fprintf(stderr, "Error: Invalid coupling type\nUsing default: 0\n");
		type_ = xnmpmasCouplingVirtualSlots;
	}	
	couplingType = type_;
	
	//read associations of crop codes with XN grids (= plant submodels)
	if (strcmp(fnCropGrids_, "")  )
	{	rtcod = readCropGridAssociation(fnCropGrids_ );
		if(1 == rtcod )
		{	std::stringstream errmsg ;
			errmsg << "Error when reading "<< fnCropGrids_<< "\n"   ;
			throw runtime_error(errmsg.str());
		};
	}
	
	
	
	//read/initialize Expert-N parameters for Land Use Activities (readCropGridAssociation should be done first if necessary)
	rtcod = readLuaProps(fnLuaProps_ );
	if(1 == rtcod )
	{		std::stringstream errmsg ;
			errmsg << "Error when reading "<< fnLuaProps_<< "\n"   ;
			throw runtime_error(errmsg.str());
	}	
	// read coupling type-specific data
	switch (couplingType) {
		
		case xnmpmasCouplingVirtualSlots:
			rtcod = readLuaCellAssociation(fnCells_ );
			if(1 == rtcod )
			{	std::stringstream errmsg ;
				errmsg << "Error when reading "<< fnCells_<< "\n"   ;
				throw runtime_error(errmsg.str());
			};
			
			break;
		case xnmpmasCouplingFixedMaps:
		case xnmpmasCouplingOneToOne:
			break;
		default:
			std::string errmsg ("Error: Unknown coupling type!\n");
			throw runtime_error(errmsg);		
	}
	

	
	//zero-initialize other variables
	clearArrayRefs();
}
void xn_mpmas_translator::clearArrayRefs() {
	mpmasArraySize = 0;
	//xnGridSize = 0;
	cropActIdsArray = NULL;
	cropAreasArray = NULL;
}
//new function using YAML format
int xn_mpmas_translator::readLuaProps(const char* fnLuaProps )
{	YAML::Node luaProps;
	try 
	{	luaProps = YAML::LoadFile(fnLuaProps);
	}
	catch (exception &e)
	{
		stringstream newE; 
		newE << e.what() << "\nError when reading " << fnLuaProps << endl ;
		throw runtime_error(newE.str());
	}
	
	if(! luaProps.IsSequence())
	{
		std::stringstream errmsg ;
		errmsg << "Error: Root node of "<< fnLuaProps<< " is not a sequence!\n"   ;
		throw runtime_error(errmsg.str());
	}
	for (std::size_t i = 0; i < luaProps.size(); ++i) 
	{
		//temporary constructs later copied to the corresponding containers
		STRUCT_mpmas_to_xn temp ={};//all parameters that are communicated to Expert-N, zero initialized
		cropAdaptationParameters tempCAP = {};//all parameters for adaptation that is not done within Expert-N, zero initialized
		luaInfo tempInfo = {};//Parameters for conversion of biomass to yields, zero initialized
		
		int luaID = getValueFromYamlNode<int>( luaProps[i]["id"], fnLuaProps, "id of element ", i)  ;
		
		string tempstring = getValueFromYamlNode<string>(luaProps[i]["crop-code"], fnLuaProps, "crop code of luaID ", luaID );
		if(tempstring.length() > XNCROPCODELENGTH)
		{
			std::stringstream errmsg ;
			errmsg << "Error in " << fnLuaProps<< "Crop code '" << tempstring.c_str() << "' of ID " << luaID << " too long.\n";
			throw runtime_error(errmsg.str());
		}
		strncpy(temp.CropCode, tempstring.c_str(), XNCROPCODELENGTH +1 );
		
		if (GridForCropCode.size() > 0)
		{
			if (GridForCropCode.count(tempstring) == 0)
			{
				std::stringstream errmsg ;
				errmsg << "Error : No entry for Crop code '" << tempstring.c_str() << "' of luaID " << luaID << " in XnCropCodeToGrid file.\n";
				throw runtime_error(errmsg.str());
			}
			temp.currentGrid = GridForCropCode[tempstring];
		}
		else 
		{
			temp.currentGrid = 0;
		}
		
		
		tempstring = getValueFromYamlNode<string>(luaProps[i]["crop-name"], fnLuaProps,"crop-name of luaID ", luaID);
		if(tempstring.length() > XNNAMELENGTH)
		{
			std::stringstream errmsg ;
			errmsg << "Error in " << fnLuaProps<< "Crop name '" << tempstring.c_str() << "' of ID " << luaID << " too long.\n";
			throw runtime_error(errmsg.str());
		}
		strncpy(temp.CropName, tempstring.c_str(), XNNAMELENGTH +1);
		
		tempstring = getValueFromYamlNode<string>(luaProps[i]["variety"], fnLuaProps,"variety of luaID ", luaID);
		if(tempstring.length() > XNNAMELENGTH)
		{
			std::stringstream errmsg ;
			errmsg << "Error in " << fnLuaProps<< "Variety '" << tempstring.c_str() << "' of ID " << luaID << " too long.\n";
			throw runtime_error(errmsg.str());
		}
		strncpy(temp.variety, tempstring.c_str(), XNNAMELENGTH +1);
		
		if (luaProps[i]["sowing"]["date"])
		{
			tempstring = getValueFromYamlNode<string>(luaProps[i]["sowing"]["date"], fnLuaProps,"sowing->date of luaID ", luaID);
			extractDateFromString(tempstring, temp.sowDate);			
		}
		else 
		{
			temp.sowDate.year = -1;
			temp.sowDate.month = -1;
			temp.sowDate.day = -1;
		}
		temp.sowDens = getValueFromYamlNode<double>(luaProps[i]["sowing"]["density"], fnLuaProps,"sowing->density of luaID ", luaID);
		temp.rowDist = getValueFromYamlNode<double>(luaProps[i]["sowing"]["row-spacing"], fnLuaProps,"sowing->row-spacing of luaID ", luaID);;
		temp.sowDepth = getValueFromYamlNode<double>(luaProps[i]["sowing"]["depth"], fnLuaProps,"sowing->depth of luaID ", luaID);
		if (luaProps[i]["sowing"]["max-days-delay"])
		{
			temp.maxSowDelay = getValueFromYamlNode<double>(luaProps[i]["sowing"]["max-days-delay"], fnLuaProps,"sowing->max-days-delay of luaID ", luaID);
		}
		else {
			temp.maxSowDelay = 0;
		}
		
		
		if (luaProps[i]["sowing"]["adaptive"])
		{
			int formulaType = getValueFromYamlNode<int>(luaProps[i]["sowing"]["adaptive"]["type"], fnLuaProps,"sowing->adaptive->type of luaID ", luaID);
			
			if (formulaType < notAdaptive || formulaType >= undefSowingDayFormula)
			{
				std::stringstream errmsg ;
				errmsg << "Error in " << fnLuaProps<< ",ID " << luaID << ": Invalid sowing->adaptive->type: " << formulaType <<".\n" ;
				throw runtime_error(errmsg.str());
			}
			
			tempCAP.sowingDayFormula = (sowingDayFormulaType) formulaType;
			tempCAP.basetemp = getValueFromYamlNode<double>(luaProps[i]["sowing"]["adaptive"]["basetemp"], fnLuaProps,"sowing->adaptive->basetemp of luaID ", luaID);
			tempCAP.seed_cons = getValueFromYamlNode<double>(luaProps[i]["sowing"]["adaptive"]["const"], fnLuaProps,"sowing->adaptive->const of luaID ", luaID);
			if(	tempCAP.sowingDayFormula == springSoilGdd
				|| tempCAP.sowingDayFormula == springSoil
			  )
			{
				tempCAP.seed_tgzthr = getValueFromYamlNode<double>(luaProps[i]["sowing"]["adaptive"]["tgzthr"], fnLuaProps,"sowing->adaptive->tgzthr of luaID ", luaID);
				tempCAP.seed_coef_tgz = getValueFromYamlNode<double>(luaProps[i]["sowing"]["adaptive"]["coef-tgz"], fnLuaProps,"sowing->adaptive->coef-tgz of luaID ", luaID);								
			}
			if(	tempCAP.sowingDayFormula == springSoilGdd
				|| tempCAP.sowingDayFormula == springGdd
				|| tempCAP.sowingDayFormula == winterGdd
			  )
			{
				tempCAP.seed_gddthr = getValueFromYamlNode<double>(luaProps[i]["sowing"]["adaptive"]["gddthr"], fnLuaProps,"sowing->adaptive->gddthr of luaID ", luaID);
				tempCAP.seed_coef_gdd = getValueFromYamlNode<double>(luaProps[i]["sowing"]["adaptive"]["coef-gdd"], fnLuaProps,"sowing->adaptive->coef-gdd of luaID ", luaID);
			}
		}
		else
		{
			tempCAP.sowingDayFormula = notAdaptive;
		}
		//either fixed date or formula for calculation of sowing date need to be specified
		if(tempCAP.sowingDayFormula == notAdaptive && (temp.sowDate.day == -1 || temp.sowDate.month == -1 || temp.sowDate.year == -1) )
		{
			std::stringstream errmsg ;
			errmsg << "Error in " << fnLuaProps<< ",ID " << luaID << ": Neither sowing date given, nor parameters for endogenous generation of sowing date.\n" ;
			throw runtime_error(errmsg.str());
		}
		//harvest
		if (luaProps[i]["harvest"]["date"])
		{
			tempstring = getValueFromYamlNode<string>(luaProps[i]["harvest"]["date"], fnLuaProps,"harvest->date of luaID ", luaID);
			extractDateFromString(tempstring, temp.harvestDate);			
		}
		else 
		{
			temp.harvestDate.year = -1;
			temp.harvestDate.month = -1;
			temp.harvestDate.day = -1;
		}
		if (luaProps[i]["harvest"]["adaptive"])
		{
			temp.harvestAdaptive = 1;
			temp.harvestBBCH1 = getValueFromYamlNode<int>(luaProps[i]["harvest"]["adaptive"]["bbch1"], fnLuaProps,"harvest->adaptive->bbch1 of luaID ", luaID);
			temp.harvestBBCH2 = getValueFromYamlNode<int>(luaProps[i]["harvest"]["adaptive"]["bbch2"], fnLuaProps,"harvest->adaptive->bbch2 of luaID ", luaID);
			temp.harvestBBCH1ExtraDays = getValueFromYamlNode<int>(luaProps[i]["harvest"]["adaptive"]["bbch1-extra-days"], fnLuaProps,"harvest->adaptive->bbch1-extra-days of luaID ", luaID);
			temp.harvestBBCH2ExtraDays = getValueFromYamlNode<int>(luaProps[i]["harvest"]["adaptive"]["bbch2-extra-days"], fnLuaProps,"harvest->adaptive->bbch2-extra-days of luaID ", luaID);
		}
		else
		{
			temp.harvestAdaptive = 0;
		}
		//either fixed date or formula for calculation of harvest date needs to be specified
		if(!temp.harvestAdaptive && (temp.harvestDate.day == -1  || temp.harvestDate.month == -1  || temp.harvestDate.year == -1  )  )
		{
			std::stringstream errmsg ;
			errmsg << "Error in " << fnLuaProps<< ",ID " << luaID << ": Neither harvest date given, nor parameters for endogenous generation of harvest date.\n" ;
			throw runtime_error(errmsg.str());
		}
		
		temp.biom_remove = getValueFromYamlNode<bool>(luaProps[i]["harvest"]["biomass-removed"], fnLuaProps,"harvest->biomass-removed of luaID ", luaID);
		tempInfo.yieldFactor = getValueFromYamlNode<double>(luaProps[i]["harvest"]["yield-factor"], fnLuaProps,"harvest->yield-factor of luaID ", luaID);
		tempInfo.stoverYieldFactor = getValueFromYamlNode<double>(luaProps[i]["harvest"]["stover-factor"], fnLuaProps,"harvest->stover-factor of luaID ", luaID);

		if (luaProps[i]["harvest"]["stem-factor"])
			tempInfo.stemYieldFactor = getValueFromYamlNode<double>(luaProps[i]["harvest"]["stem-factor"], fnLuaProps,"harvest->stem-factor of luaID ", luaID);
		else
			tempInfo.stemYieldFactor = 0;

//NEW CT 170107: 		
		//tillage
		if(luaProps[i]["tillage"])
		{	if(! luaProps[i]["tillage"].IsSequence() )
			{
				std::stringstream errmsg ;
				errmsg << "Error in " << fnLuaProps<< ",ID " << luaID << ": Wrong format for 'tillage'\n";
				throw runtime_error(errmsg.str());
			}
			if(luaProps[i]["tillage"].size() > XNMPMASTILLAGESLOTS )
			{
				std::stringstream errmsg ;
				errmsg << "Error in " << fnLuaProps<< ",ID " << luaID << ": Too many instances for 'tillage',  max: " << XNMPMASTILLAGESLOTS << " .\n" ;
				throw runtime_error(errmsg.str());
			}
			
			temp.numTill = luaProps[i]["tillage"].size();
			
			for (size_t j = 0; j < luaProps[i]["tillage"].size(); ++j)
			{	
				tempstring = getValueFromYamlNode<string>(luaProps[i]["tillage"][j]["date"], fnLuaProps,"tillage->?->date of luaID ", luaID);
				extractDateFromString(tempstring, temp.tillage[j].tillDate);
				
				tempstring = getValueFromYamlNode<string>(luaProps[i]["tillage"][j]["code"], fnLuaProps,"tillage->?->code of luaID ", luaID);
				if(tempstring.length() > XNTILLCODELENGTH)
				{
					std::stringstream errmsg ;
					errmsg << "Error in " << fnLuaProps<< ",ID " << luaID << ": Tillage  code '"<<tempstring.c_str()<<"' too long.\n .\n" ;
					throw runtime_error(errmsg.str());
				}
				strncpy(temp.tillage[j].code, tempstring.c_str(),XNTILLCODELENGTH +1);
				
				temp.tillage[j].fDepth =getValueFromYamlNode<double>(luaProps[i]["tillage"][j]["depth"], fnLuaProps,"tillage->?->depth of luaID ", luaID);
				
				if (luaProps[i]["tillage"][j]["adaptive"])
				{
					tempstring = getValueFromYamlNode<string>(luaProps[i]["tillage"][j]["adaptive"]["type"], fnLuaProps,"tillage->adaptive->type of luaID ", luaID);
					if (tempstring == "before-sowing") {
						temp.tillage[j].typeAdaptiveTillage = adaptiveTillageBeforeSowing;
					}
					else if (tempstring == "before-cover") {
						temp.tillage[j].typeAdaptiveTillage = adaptiveTillageBeforeCoverCrop;
					}
					else if (tempstring == "after-harvest") {
						temp.tillage[j].typeAdaptiveTillage = adaptiveTillageAfterHarvest;
					}
					else {
						std::stringstream errmsg ;
						errmsg << "Error in " << fnLuaProps<< ",ID " << luaID << ": Tillage invalid type of adaptive tillage date: '"<<tempstring.c_str()<<"' too long.\n .\n" ;
						throw runtime_error(errmsg.str());
					}
					temp.tillage[j].daysBeforeAfter = getValueFromYamlNode<int>(luaProps[i]["tillage"][j]["adaptive"]["days"], fnLuaProps,"mineral-fertilization->?->adaptive->bbch of luaID ", luaID);
				}
				else
				{
					temp.tillage[j].typeAdaptiveTillage = adaptiveTillageNotAdaptive;
					temp.tillage[j].daysBeforeAfter = -1;
				}
			}
		} //endif tillage exists
		else 
		{  temp.numTill = 0;
		} //endif tillage does not exist
				
		
		
		//mineral fertilization
		if(luaProps[i]["mineral-fertilization"])
		{	if(! luaProps[i]["mineral-fertilization"].IsSequence() )
			{
				std::stringstream errmsg ;
				errmsg << "Error in " << fnLuaProps<< ",ID " << luaID << ": Wrong format for 'mineral-fertilization'\n";
				throw runtime_error(errmsg.str());
			}
			if(luaProps[i]["mineral-fertilization"].size() > XNMPMASMINFERTSLOTS )
			{
				std::stringstream errmsg ;
				errmsg << "Error in " << fnLuaProps<< ",ID " << luaID << ": Too many instances for 'mineral-fertilization',  max: " << XNMPMASMINFERTSLOTS << " .\n" ;
				throw runtime_error(errmsg.str());
			}
			
			temp.numMinFert = luaProps[i]["mineral-fertilization"].size();
			
			for (size_t j = 0; j < luaProps[i]["mineral-fertilization"].size(); ++j)
			{	
				tempstring = getValueFromYamlNode<string>(luaProps[i]["mineral-fertilization"][j]["date"], fnLuaProps,"mineral-fertilization->?->date of luaID ", luaID);
				extractDateFromString(tempstring, temp.mineralFertilization[j].fertDate);
				
				tempstring = getValueFromYamlNode<string>(luaProps[i]["mineral-fertilization"][j]["code"], fnLuaProps,"mineral-fertilization->?->code of luaID ", luaID);
				if(tempstring.length() > XNFERTCODELENGTH)
				{
					std::stringstream errmsg ;
					errmsg << "Error in " << fnLuaProps<< ",ID " << luaID << ": Fertilizer code '"<<tempstring.c_str()<<"' too long.\n .\n" ;
					throw runtime_error(errmsg.str());
				}
				strncpy(temp.mineralFertilization[j].code, tempstring.c_str(),XNFERTCODELENGTH +1);
				
				temp.mineralFertilization[j].no3n =getValueFromYamlNode<double>(luaProps[i]["mineral-fertilization"][j]["no3n"], fnLuaProps,"mineral-fertilization->?->no3n of luaID ", luaID);
				temp.mineralFertilization[j].nh4n = getValueFromYamlNode<double>(luaProps[i]["mineral-fertilization"][j]["nh4n"], fnLuaProps,"mineral-fertilization->?->nh4n of luaID ", luaID);
				temp.mineralFertilization[j].urea = getValueFromYamlNode<double>(luaProps[i]["mineral-fertilization"][j]["urea"], fnLuaProps,"mineral-fertilization->?->urea of luaID ", luaID);
				
				if (luaProps[i]["mineral-fertilization"][j]["adaptive"])
				{
					temp.mineralFertilization[j].adaptive = 1;
					if ( luaProps[i]["mineral-fertilization"][j]["adaptive"]["bbch"]) 
					{
						temp.mineralFertilization[j].bbch = getValueFromYamlNode<int>(luaProps[i]["mineral-fertilization"][j]["adaptive"]["bbch"], fnLuaProps,"mineral-fertilization->?->adaptive->bbch of luaID ", luaID);
					}
					else 
					{
						temp.mineralFertilization[j].bbch = -1;
					}
					
					if (luaProps[i]["mineral-fertilization"][j]["adaptive"]["nmin-factor"] )
					{
					
						temp.mineralFertilization[j].nminAdapt_factor = getValueFromYamlNode<double>(luaProps[i]["mineral-fertilization"][j]["adaptive"]["nmin-factor"], fnLuaProps,"mineral-fertilization->?->adaptive->nmin-factor of luaID ", luaID);
						temp.mineralFertilization[j].nminAdapt_ref = getValueFromYamlNode<double>(luaProps[i]["mineral-fertilization"][j]["adaptive"]["nmin-reference"], fnLuaProps,"mineral-fertilization->?->adaptive->nmin-base of luaID ", luaID);
						temp.mineralFertilization[j].nminAdapt_depth = getValueFromYamlNode<int>(luaProps[i]["mineral-fertilization"][j]["adaptive"]["nmin-depth"], fnLuaProps,"mineral-fertilization->?->adaptive->nmin-depth of luaID ", luaID);					
					}
					else 
					{
						temp.mineralFertilization[j].nminAdapt_factor = -1.0;
						temp.mineralFertilization[j].nminAdapt_ref = 0;
						temp.mineralFertilization[j].nminAdapt_depth = 90;					
					}
				}
				else
				{
					temp.mineralFertilization[j].adaptive = 0;
					temp.mineralFertilization[j].bbch = -1;
				}

			}
		} //endif mineral fertilization exists
		else 
		{  temp.numMinFert = 0;
		} //endif mineral fertilization does not exist
		
		//organic fertilization
		if(luaProps[i]["organic-fertilization"] )
		{	if (! luaProps[i]["organic-fertilization"].IsSequence() )
			{
				std::stringstream errmsg ;
				errmsg << "Error in " << fnLuaProps<< ",ID " << luaID << ": Wrong format for 'organic-fertilization'.\n" ;
				throw runtime_error(errmsg.str());
			}
		
			if(luaProps[i]["organic-fertilization"].size() > XNMPMASORGFERTSLOTS )
			{
				std::stringstream errmsg ;
				errmsg << "Error in " << fnLuaProps<< ",ID " << luaID << ": Too many instances for 'organic-fertilization',  max: " << XNMPMASORGFERTSLOTS << " .\n" ;
				throw runtime_error(errmsg.str());
			}
			
			temp.numOrgFert = luaProps[i]["organic-fertilization"].size();
			
			for (size_t j = 0; j < luaProps[i]["organic-fertilization"].size(); ++j)
			{
				tempstring = getValueFromYamlNode<string>(luaProps[i]["organic-fertilization"][j]["date"], fnLuaProps,"organic-fertilization->?->date of luaID ", luaID);
				extractDateFromString(tempstring, temp.organicFertilization[j].orgfertDate);
				
				tempstring = getValueFromYamlNode<string>(luaProps[i]["organic-fertilization"][j]["code"], fnLuaProps,"organic-fertilization->?->code of luaID ", luaID);
				if(tempstring.length() > XNORGFERTCODELENGTH)
				{
					std::stringstream errmsg ;
					errmsg << "Error in " << fnLuaProps<< ",ID " << luaID << ": Fertilizer code '"<<tempstring.c_str()<<"' too long.\n .\n" ;
					throw runtime_error(errmsg.str());
				}
				strncpy(temp.organicFertilization[j].code, tempstring.c_str(),XNORGFERTCODELENGTH +1);
				
				temp.organicFertilization[j].drymatter = getValueFromYamlNode<double>(luaProps[i]["organic-fertilization"][j]["dry-matter"], fnLuaProps,"organic-fertilization->?->dry-matter of luaID ", luaID);
				temp.organicFertilization[j].nh4n = getValueFromYamlNode<double>(luaProps[i]["organic-fertilization"][j]["nh4n"], fnLuaProps,"organic-fertilization->?->nh4n of luaID ", luaID);
				temp.organicFertilization[j].org_subst = getValueFromYamlNode<double>(luaProps[i]["organic-fertilization"][j]["organic-substance"], fnLuaProps,"organic-fertilization->?->organic-substance of luaID ", luaID);
				
				if (luaProps[i]["organic-fertilization"][j]["adaptive"])
				{
					temp.organicFertilization[j].adaptive = 1;
					temp.organicFertilization[j].bbch = getValueFromYamlNode<int>(luaProps[i]["organic-fertilization"][j]["adaptive"]["bbch"], fnLuaProps,"organic-fertilization->?->adaptive->bbch of luaID ", luaID);
				}
				else
				{
					temp.organicFertilization[j].adaptive = 0;
					temp.organicFertilization[j].bbch = -1;
				}
			}
		}//endif organic-fertilization exists
		else 
		{	temp.numOrgFert = 0;
		} //endif organic-fertilization does not exist
		
		//irrigation
		if(luaProps[i]["irrigation"] )
		{	if (! luaProps[i]["irrigation"].IsSequence() )
			{
				std::stringstream errmsg ;
				errmsg << "Error in " << fnLuaProps<< ",ID " << luaID << ": Wrong format for 'irrigation'.\n" ;
				throw runtime_error(errmsg.str());
			}
		
			if(luaProps[i]["irrigation"].size() > XNMPMASIRRIGATIONSLOTS )
			{
				std::stringstream errmsg ;
				errmsg << "Error in " << fnLuaProps<< ",ID " << luaID << ": Too many instances for 'irrigation',  max: " << XNMPMASIRRIGATIONSLOTS << " .\n" ;
				throw runtime_error(errmsg.str());
			}
			
			temp.numIrrig = luaProps[i]["irrigation"].size();
			
			for (size_t j = 0; j < luaProps[i]["irrigation"].size(); ++j)
			{
				tempstring = getValueFromYamlNode<string>(luaProps[i]["irrigation"][j]["date"], fnLuaProps,"irigation->?->date of luaID ", luaID);
				extractDateFromString(tempstring, temp.irrigation[j].irrDate);
				temp.irrigation[j].quantity = getValueFromYamlNode<double>(luaProps[i]["irrigation"][j]["quantity"], fnLuaProps,"irigation->?->quantity of luaID ", luaID);
			}
		}//endif irrigation exists
		else 
		{	temp.numIrrig = 0;
		} //endif irrigation does not exist
		
	
//NEW CT 170107: 		
		//Cover crop
		if (luaProps[i]["cover-crop"]) {
		
			tempstring = getValueFromYamlNode<string>(luaProps[i]["cover-crop"]["cover-code"], fnLuaProps, "cover-crop->cover-code of luaID ", luaID );
			if(tempstring.length() > XNCROPCODELENGTH)
			{
				std::stringstream errmsg ;
				errmsg << "Error in " << fnLuaProps<< "Cover-crop cover-code '" << tempstring.c_str() << "' of ID " << luaID << " too long.\n";
				throw runtime_error(errmsg.str());
			}
			strncpy(temp.coverCropCode, tempstring.c_str(), XNCROPCODELENGTH +1 );
			
			tempstring = getValueFromYamlNode<string>(luaProps[i]["cover-crop"]["cover-name"], fnLuaProps,"cover-crop->cover-name of luaID ", luaID);
			if(tempstring.length() > XNNAMELENGTH)
			{
				std::stringstream errmsg ;
				errmsg << "Error in " << fnLuaProps<< "Cover-crop cover-name '" << tempstring.c_str() << "' of ID " << luaID << " too long.\n";
				throw runtime_error(errmsg.str());
			}
			strncpy(temp.coverCropName, tempstring.c_str(), XNNAMELENGTH +1);
			
			tempstring = getValueFromYamlNode<string>(luaProps[i]["cover-crop"]["cover-variety"], fnLuaProps,"cover-crop variety of luaID ", luaID);
			if(tempstring.length() > XNNAMELENGTH)
			{
				std::stringstream errmsg ;
				errmsg << "Error in " << fnLuaProps<< "Cover-crop cover-variety '" << tempstring.c_str() << "' of ID " << luaID << " too long.\n";
				throw runtime_error(errmsg.str());
			}
			strncpy(temp.coverCropVariety, tempstring.c_str(), XNNAMELENGTH +1);
			
			if (luaProps[i]["cover-crop"]["cover-sowing"]["date"])
			{
				tempstring = getValueFromYamlNode<string>(luaProps[i]["cover-crop"]["cover-sowing"]["date"], fnLuaProps,"cover-crop->sowing->date of luaID ", luaID);
				extractDateFromString(tempstring, temp.coverCropSowDate);			
			}
			else 
			{
				temp.coverCropSowDate.year = -1;
				temp.coverCropSowDate.month = -1;
				temp.coverCropSowDate.day = -1;
			}
			
			if (luaProps[i]["cover-crop"]["cover-sowing"]["max-days-delay"])
			{
				temp.coverCropMaxSowDelay = getValueFromYamlNode<double>(luaProps[i]["cover-crop"]["cover-sowing"]["max-days-delay"], fnLuaProps,"cover-crop->cover-sowing->max-days-delay of luaID ", luaID);
			}
			else {
				temp.coverCropMaxSowDelay = 0;
			}
			
			temp.coverCropSowDens = getValueFromYamlNode<double>(luaProps[i]["cover-crop"]["cover-sowing"]["density"], fnLuaProps,"cover-crop sowing->density of luaID ", luaID);
			temp.coverCropRowDist = getValueFromYamlNode<double>(luaProps[i]["cover-crop"]["cover-sowing"]["row-spacing"], fnLuaProps,"cover-crop sowing->row-spacing of luaID ", luaID);;
			temp.coverCropSowDepth = getValueFromYamlNode<double>(luaProps[i]["cover-crop"]["cover-sowing"]["depth"], fnLuaProps,"cover-crop sowing->depth of luaID ", luaID);
			
			if (luaProps[i]["cover-crop"]["plough-under"]["date"])
			{
				tempstring = getValueFromYamlNode<string>(luaProps[i]["cover-crop"]["plough-under"]["date"], fnLuaProps,"cover-crop->plough-under->date of luaID ", luaID);
				extractDateFromString(tempstring, temp.coverCropPloughUnderDate);			
			}
			else 
			{
				temp.coverCropPloughUnderDate.year = -1;
				temp.coverCropPloughUnderDate.month = -1;
				temp.coverCropPloughUnderDate.day = -1;
			}
		
		}//endif exists cover-crop
		else {
			strcpy(temp.coverCropCode, "\0\0\0");
			strcpy(temp.coverCropName, "\0");
			strcpy(temp.coverCropVariety, "\0");
			temp.coverCropSowDate.year = -1;
			temp.coverCropSowDate.month = -1;
			temp.coverCropSowDate.day = -1;;
			temp.coverCropSowDens= 0;
			temp.coverCropRowDist = 0;
			temp.coverCropSowDepth= 0;  
			temp.coverCropPloughUnderDate.year = -1;
			temp.coverCropPloughUnderDate.month = -1;
			temp.coverCropPloughUnderDate.day = -1;
		}
		
	//*****//
		//insert temporary structs into corresponding map containers
		LuaXnParameters.insert(pair<int,STRUCT_mpmas_to_xn>(luaID, temp));
		LuaCouplingParameters.insert(pair<int,luaInfo>(luaID, tempInfo));
		LuaAdaptationParameters.insert(pair<int,cropAdaptationParameters>(luaID, tempCAP));
	} //end loop over land use types
	
	//add dummy fallow management to be used for unused cells if not provided by input file (-1)
	if (LuaXnParameters.find(-1) == LuaXnParameters.end()) 
	{
		STRUCT_mpmas_to_xn temp = {};
		cropAdaptationParameters tempCAP = {};
		luaInfo tempInfo = {};
		
		strcpy(temp.CropCode, "MZ"); //???
		strcpy(temp.CropName, "Maize");
		strcpy(temp.variety, "Fallow");
		temp.sowDate.day = 10;
		temp.sowDate.month = 10;
		temp.sowDate.year = 2;
		temp.harvestDate.day = 10;
		temp.harvestDate.month = 1;
		temp.harvestDate.year = 2;
		temp.biom_remove = 0;
		temp.numIrrig = 0;
		temp.numMinFert = 0;
		temp.numOrgFert = 0;
		temp.rowDist = 1;
		temp.sowDens = 0.0001;

		tempCAP.sowingDayFormula = notAdaptive;
		//...
		
		LuaXnParameters.insert(pair<int,STRUCT_mpmas_to_xn>(-1, temp));
		LuaCouplingParameters.insert(pair<int,luaInfo>(-1, tempInfo));
		LuaAdaptationParameters.insert(pair<int,cropAdaptationParameters>(-1, tempCAP));
		
	}
	return 0;
}
/* old version with numbers only input file format
int xn_mpmas_translator::readLuaProps(const char* fnLuaProps )
{
	FILE* infile = fopen(fnLuaProps, "r");
	if(NULL == infile )
	{
		fprintf(stderr, "Error when trying to open: %s\n", fnLuaProps);
		return 1;
	}
	while(!feof(infile) )
	{
		int lua;
		int rtcod = fscanf(infile, "%d", &lua);
		if(rtcod != 1 && feof(infile))
		{
			break;
		}
		checkFscanfSuccess(rtcod, 1, fnLuaProps, "Land use activity");
		STRUCT_mpmas_to_xn temp ={};
		
		//temp buffer for reading strings
		char rc[250];
		
		// read crop code
		rtcod = fscanf(infile, "%249s", &rc);
		char message[250];
		sprintf(message," crop code for land use activity %d", lua);
		checkFscanfSuccess(rtcod, 1, fnLuaProps, message);
		if (strlen(rc) != XNCROPCODELENGTH)
		{
			fprintf(stderr, "Error when reading: %s\nInvalid crop code for LUA %d\n", fnLuaProps, lua);
			fclose(infile);
			return 1;
		}	
		strncpy(temp.CropCode, rc, XNCROPCODELENGTH + 1);
		
		//crop name
		rtcod = fscanf(infile, "%249s", &rc);
		sprintf(message," crop name for land use activity %d", lua);
		checkFscanfSuccess(rtcod, 1, fnLuaProps, message);
		if (strlen(rc) > XNNAMELENGTH)
		{
			fprintf(stderr, "Error when reading: %s\nInvalid crop name for LUA %d\n", fnLuaProps, lua);
			fclose(infile);
			return 1;
		}
		strncpy(temp.CropName, rc, XNNAMELENGTH + 1);
		
		//crop variety
		rtcod = fscanf(infile, "%249s", &rc);
		sprintf(message," variety for land use activity %d", lua);
		checkFscanfSuccess(rtcod, 1, fnLuaProps, message);
		if (strlen(rc) > XNNAMELENGTH)
		{
			fprintf(stderr, "Error when reading: %s\nInvalid variety for LUA %d\n", fnLuaProps, lua);
			fclose(infile);
			return 1;
		}
		strncpy(temp.variety, rc, XNNAMELENGTH + 1);
				
		rtcod = fscanf(infile, "%d", &temp.sowDate.day);
		sprintf(message," sowing day for land use activity %d", lua);
		checkFscanfSuccess(rtcod, 1, fnLuaProps, message);

		rtcod = fscanf(infile, "%d", &temp.sowDate.month);
		sprintf(message," sowing month for land use activity %d", lua);
		checkFscanfSuccess(rtcod, 1, fnLuaProps, message);

		rtcod = fscanf(infile, "%d", &temp.sowDate.year);
		sprintf(message," sowing year for land use activity %d", lua);
		checkFscanfSuccess(rtcod, 1, fnLuaProps, message);

		rtcod = fscanf(infile, "%lf", &temp.sowDens);
		sprintf(message," sowing density for land use activity %d", lua);
		checkFscanfSuccess(rtcod, 1, fnLuaProps, message);
		
		rtcod = fscanf(infile, "%lf", &temp.rowDist);
		sprintf(message," row distance for land use activity %d", lua);
		checkFscanfSuccess(rtcod, 1, fnLuaProps, message);

		rtcod = fscanf(infile, "%lf", &temp.sowDepth);
		sprintf(message," sowing depth for land use activity %d", lua);
		checkFscanfSuccess(rtcod, 1, fnLuaProps, message);

		int num;
		rtcod = fscanf(infile, "%d", &num);
		sprintf(message," number of mineral fertilizations for land use activity %d", lua);
		checkFscanfSuccess(rtcod, 1, fnLuaProps, message);

		if (num > XNMPMASMINFERTSLOTS|| num < 0)
		{
			fprintf(stderr, "Error when reading: %s\nInvalid number of mineral fertilizer applications %d (max. %d) for LUA %d\n", fnLuaProps, num, XNMPMASMINFERTSLOTS, lua);
			fclose(infile);
			return 1;
		}
		temp.numMinFert = num;
		for(int i = 0; i < num; i++)
		{
			rtcod = fscanf(infile, "%d %d %d %249s %lf %lf %lf", 
							&temp.mineralFertilization[i].fertDate.day,
							&temp.mineralFertilization[i].fertDate.month,
							&temp.mineralFertilization[i].fertDate.year,
							&rc,
							&temp.mineralFertilization[i].no3n,
							&temp.mineralFertilization[i].nh4n,
							&temp.mineralFertilization[i].urea
							);
			sprintf(message," mineral fertilizer application %d for land use activity %d", i, lua);
			checkFscanfSuccess(rtcod, 7, fnLuaProps, message);
			if(strlen(rc) > XNFERTCODELENGTH)
			{
				fprintf(stderr, "Error when reading: %s\nInvalid fertilizer code for LUA %d\n", fnLuaProps, lua);
				fclose(infile);
				return 1;
			}
			strncpy(temp.mineralFertilization[i].code, rc, XNFERTCODELENGTH + 1);
		}

		rtcod = fscanf(infile, "%d", &num);
		sprintf(message," number of organic fertilizations for land use activity %d", lua);
		checkFscanfSuccess(rtcod, 1, fnLuaProps, message);

		if (num > XNMPMASORGFERTSLOTS|| num < 0)
		{
			fprintf(stderr, "Error when reading: %s\nInvalid number of organic fertilizer applications %d (max. %d) for LUA %d\n", fnLuaProps, num, XNMPMASORGFERTSLOTS, lua);
			fclose(infile);
			return 1;
		}
		temp.numOrgFert = num;
		for(int i = 0; i < num; i++)
		{
			rtcod = fscanf(infile, "%d %d %d %249s %lf %lf %lf", 
							&temp.organicFertilization[i].orgfertDate.day,
							&temp.organicFertilization[i].orgfertDate.month,
							&temp.organicFertilization[i].orgfertDate.year,
							&rc,
							&temp.organicFertilization[i].drymatter,
							&temp.organicFertilization[i].nh4n,
							&temp.organicFertilization[i].org_subst
							);
			sprintf(message," organic fertilizer application %d for land use activity %d", i, lua);
			checkFscanfSuccess(rtcod, 7, fnLuaProps, message);
			if(strlen(rc) > XNORGFERTCODELENGTH)
			{
				fprintf(stderr, "Error when reading: %s\nInvalid organic fertilizer code for LUA %d\n", fnLuaProps, lua);
				fclose(infile);
				return 1;
			}
			strncpy(temp.organicFertilization[i].code, rc, XNORGFERTCODELENGTH +1 );
		}

		rtcod = fscanf(infile, "%d", &num);
		sprintf(message," number of irrigation applications for land use activity %d", lua);
		checkFscanfSuccess(rtcod, 1, fnLuaProps, message);

		if (num > XNMPMASIRRIGATIONSLOTS|| num < 0)
		{
			fprintf(stderr, "Error when reading: %s\nInvalid number of irrigation water applications %d (max. %d) for LUA %d\n", fnLuaProps, num, XNMPMASIRRIGATIONSLOTS, lua);
			fclose(infile);
			return 1;
		}
		temp.numIrrig = num;
		for(int i = 0; i < num; i++)
		{
			rtcod = fscanf(infile, "%d %d %d %lf", 
							&temp.irrigation[i].irrDate.day,
							&temp.irrigation[i].irrDate.month,
							&temp.irrigation[i].irrDate.year,
							&temp.irrigation[i].quantity
							);
			sprintf(message," irrigation water application %d for land use activity %d", i, lua);
			checkFscanfSuccess(rtcod, 4, fnLuaProps, message);
		}

		rtcod = fscanf(infile, "%d", &temp.harvestDate.day);
		sprintf(message," harvest day for land use activity %d", lua);
		checkFscanfSuccess(rtcod, 1, fnLuaProps, message);

		rtcod = fscanf(infile, "%d", &temp.harvestDate.month);
		sprintf(message," harvest month for land use activity %d", lua);
		checkFscanfSuccess(rtcod, 1, fnLuaProps, message);

		rtcod = fscanf(infile, "%d", &temp.harvestDate.year);
		sprintf(message," harvest year for land use activity %d", lua);
		checkFscanfSuccess(rtcod, 1, fnLuaProps, message);

		rtcod = fscanf(infile, "%d", &temp.biom_remove);
		sprintf(message," biomass removal for land use activity %d", lua);
		checkFscanfSuccess(rtcod, 1, fnLuaProps, message);		
		
		LuaXnParameters.insert(pair<int,STRUCT_mpmas_to_xn>(lua, temp));
		
		luaInfo tempInfo = {};//zero initialized
		rtcod = fscanf(infile, "%lf", &tempInfo.yieldFactor);
		sprintf(message," yield conversion factor for land use activity %d", lua);
		checkFscanfSuccess(rtcod, 1, fnLuaProps, message);	
	
		rtcod = fscanf(infile, "%lf", &tempInfo.stoverYieldFactor);
		sprintf(message," stover yield conversion factor for land use activity %d", lua);
		checkFscanfSuccess(rtcod, 1, fnLuaProps, message);	
		
		LuaCouplingParameters.insert(pair<int,luaInfo>(lua, tempInfo));
		
	}
	fclose(infile);
	
	//add dummy fallow management to be used for unused cells if not provided by input file (-1)
	if (LuaXnParameters.find(-1) == LuaXnParameters.end()) 
	{
		STRUCT_mpmas_to_xn temp = {};
		strcpy(temp.CropCode, "MZ"); //???
		strcpy(temp.CropName, "Maize");
		strcpy(temp.variety, "Fallow");
		temp.sowDate.day = 10;
		temp.sowDate.month = 10;
		temp.sowDate.year = 2;
		temp.harvestDate.day = 10;
		temp.harvestDate.month = 1;
		temp.harvestDate.year = 2;
		temp.biom_remove = 0;
		temp.numIrrig = 0;
		temp.numMinFert = 0;
		temp.numOrgFert = 0;
		temp.rowDist = 1;
		temp.sowDens = 0.0001;
		//...
		LuaXnParameters.insert(pair<int,STRUCT_mpmas_to_xn>(-1, temp));
	}
	return 0;
}
 */
int xn_mpmas_translator::readCropGridAssociation(const char* fnCropGrids )
{
	FILE* infile = fopen(fnCropGrids, "r");
	if(NULL == infile )
	{
		fprintf(stderr, "Error when trying to open: %s\n", fnCropGrids);
		return 1;
	}

#define STRINGIFY(x) STRINGIFY2(x)
#define STRINGIFY2(x) #x
	
	while(!feof(infile) )
	{
		char cropcode[XNCROPCODELENGTH +1];
		int grid;
		int rtcod = 0;
		rtcod = fscanf(infile, "%" STRINGIFY(XNCROPCODELENGTH) "s %d", cropcode, &grid);
		if (rtcod != 2)
		{
			if (feof(infile))
				break;
			else
			{	fprintf(stderr, "Error when reading: %s\n", fnCropGrids);
				return 1;
			}
		}
		cropcode[XNCROPCODELENGTH] = '\0';
	
		GridForCropCode[std::string(cropcode)] = grid;
	}	
	fclose(infile);
	return 0;
}

void xn_mpmas_translator::defineLuaForCellsFromMap(const Raster2D* luaMap )
{
	swapAndCleanGrids();	
	
	for (int row = 0; row < luaMap->rows(); ++row)
	{	for (int col = 0; col < luaMap->cols(); ++col)	
		{
			int ci = xnGridYdim * col + row;
			int lua = (int) luaMap->getValue(row, col);
			try {
				currentXnCropGrid.at(ci) = lua;
			}
			catch (exception& e)
			{	std::stringstream errmsg;
				errmsg << "Error in <defineLuaForCellsFromMap>:\n"
				<< "currentXnCropGrid : Cell index " << ci <<"(" << col <<"," << row << ")"<< " out of bounds ("<< currentXnCropGrid.size()<<")\n:" << e.what() <<"\n";
				throw runtime_error(errmsg.str());
			}		
		}
	}

} 
int xn_mpmas_translator::readLuaCellAssociation(const char* fnCells )
{
	vector<map< int, int > > LuaForCell; //just for error checking
	FILE* infile = fopen(fnCells, "r");
	if(NULL == infile )
	{
		fprintf(stderr, "Error when trying to open: %s\n", fnCells);
		return 1;
	}
	
	while(!feof(infile) )
	{
		int lua, year, x, rtcod;
		rtcod = fscanf(infile, "%d %d %d ", &year, &lua, &x);
		if (rtcod != 3)
		{
			if (feof(infile))
				break;
			else
			{	fprintf(stderr, "Error when reading: %s\n", fnCells);
				return 1;
			}
		}
		if(year >= (signed) LuaForCell.size()) 
		{ 	for (int i = LuaForCell.size(); i <= year ; i++)
			{		LuaForCell.push_back(map<int,int>());
					CellForLua.push_back(multimap<int,int>());
			}
		}
		if(x < 0)
		{
			try {
				CellForLua.at(year).insert(pair<int,int>(lua, x));
			}
			catch (exception& e)
			{	std::stringstream errmsg;
				errmsg << "Error in <readLuaCellAssociation>:\n"
				<< "CellForLua: Year index out of bounds\n:" << e.what() <<"\n";
				throw runtime_error(errmsg.str());
			}	
		}
		else 
		{
			try {
				map<int, int >::iterator checkIt = LuaForCell.at(year).find(x);
				if (checkIt == LuaForCell.at(year).end())
				{
					LuaForCell.at(year).insert(pair<int,int>(x, lua));
					CellForLua.at(year).insert(pair<int,int>(lua, x));
				}

				else 
				{
					fprintf(stderr, "Error when reading: %s\nExpert-N cell %d has been assigned to two land use activities for year %d\n", fnCells, x, year);
					fclose(infile);
					return 1;
				}
			}				
			catch (exception& e)
			{	std::stringstream errmsg;
				errmsg << "Error in <readLuaCellAssociation>:\n"
				<< "LuaForCell or CellForLua : Year index out of bounds\n:" << e.what() <<"\n";
				throw runtime_error(errmsg.str());
			}	
			
		}
	}
	fclose(infile);
	return 0;
}
void xn_mpmas_translator::swapAndCleanGrids()  
{
	
	swap(currentXnCropGrid, previousXnCropGrid);
	currentXnCropGrid.clear();
	currentXnCropGrid.resize(xnGridSize, -1);
	
	
	swap(PreviousCellsForLua, CurrentCellsForLua);
	CurrentCellsForLua.clear();

}
void xn_mpmas_translator::defineLuaforCellsFromArray(int year, int mpmasArraySize_,  int* cropActIds_, double* cropAreas_)
{
	//for coulingType = xnmpmasCouplingVirtualSlots
	#define XNMPMASDBG2  0

	mpmasArraySize = mpmasArraySize_;
	cropActIdsArray = cropActIds_;
	cropAreasArray = cropAreas_;

	swapAndCleanGrids();
	//transform mpmas model periods
	//if period greater than number of periods for which cell assignments have been recorded 
	//start over again from first
	int p = 0;
	if(year < 0)
	{
		std::string errmsg ("Error: Coupling currently not implemented for spin up periods\n") ;
		throw runtime_error(errmsg);
	}	
	
	if (year >= 0 && year < (signed) CellForLua.size())
		p = year;
	else
		p = year % CellForLua.size();
		
	if(xnmpmasCouplingVirtualSlots == couplingType)
	{
		for (int l = 0; l < mpmasArraySize; l++)
		{
			int lua = cropActIdsArray[l];
			//if(cropAreasArray[l] > 0 && cropActIdsArray[l] > -1)
			if(lua > -1)
			{
				multimap<int,int>::iterator it, itlow, itup;
				
				itlow = CellForLua[p].lower_bound(lua);
				itup = CellForLua[p].upper_bound(lua);
				int actualCellChosen = -1;
				for (it = itlow; it != itup; it++)
				{
					int c = it->second;
					
					if (c < 0)
					{
						if(actualCellChosen == 1)
						{
							std::stringstream errmsg;
							errmsg << "Error in xn_mpmas_translator::defineLUAforCells: You requested to derive the yield for " 
								   << "LUA " <<l <<" from LUA " <<(-1*c) <<", but you already assigned a cell to LUA " << l<< "\n";
							throw runtime_error(errmsg.str());
						}
						CurrentCellsForLua.insert(pair<int,int>(lua,c));
						actualCellChosen = 0;
						//debug:
						if (XNMPMASDBG2)
							printf("Lua: %d from lua %d\n", lua, c);
					}
					else 
					{
						if(actualCellChosen == 0)
						{							
							std::stringstream errmsg;
							errmsg << "Error in xn_mpmas_translator::defineLUAforCells: You are assigning a cell to  " 
								   << "LUA " <<l <<", but you already ordered to derive the yield for this LUA from another land use activity\n";
							throw runtime_error(errmsg.str());
						}
						try {
							currentXnCropGrid.at(c) = lua;
						}
						catch (exception& e)
						{	std::stringstream errmsg;
							errmsg << "Error in <defineLUAforCells>:\n"
							<< "currentXnCropGrid : Cell index out of bounds\n:" << e.what() <<"\n";
							throw runtime_error(errmsg.str());
						}	
						CurrentCellsForLua.insert(pair<int,int>(lua,c));
						actualCellChosen = 1;
						if (XNMPMASDBG2)
							printf("Lua: %d on cell %d\n", lua, c);
						
					}
				}
			}
			
		}
	}
#define XNMPMASDBG2  0
}


STRUCT_mpmas_to_xn xn_mpmas_translator::getManagementForCell(int cell, int mpmasYear, Raster2D* cropMapILMS , Raster2D* sowMapILMSearly, Raster2D* sowMapILMSlate, vector<int>* sowDateRepresentatives)
{	int ws = 1; // change to zero if not overlapping years
	STRUCT_mpmas_to_xn management;
	if (cell < 0 || cell >= xnGridSize)
	{
		std::stringstream errmsg;
		errmsg << "Error in xn_mpmas_translator::getManagementForCell: Cell " 
			   << cell <<" out of bounds\n";
		throw runtime_error(errmsg.str());
	}
	
	//check land use activity assigned to grid cell
	int lua = currentXnCropGrid[cell];

	//search for management info for land use activity
	map<int, STRUCT_mpmas_to_xn>::iterator managIt = LuaXnParameters.find(lua); //Note: LuaXnParameters must contain something for empty cells under lua -1
	if (managIt == LuaXnParameters.end())
	{
		std::stringstream errmsg;
		errmsg << "Error in xn_mpmas_translator::getManagementForCell:  No Expert-N management defined for land use activity " 
			   << lua <<"\n";
		throw runtime_error(errmsg.str());
	}
	management = (*managIt).second;


	if (cropMapILMS != NULL) 
	{
		if (lua != 170 && strncmp( management.CropCode, "MZ",2) && strncmp( management.CropCode, "SB",2)   ) //Maize and sugar beet are not early removed crops
		{
			int col = cell / xnGridYdim; //integer division
			int row = cell % xnGridYdim;
			cropMapILMS->setValue(row, col, 1);
		}
		else //currently everything else is set to NOAH_MP cropland 
		{
			int col = cell / xnGridYdim; //integer division
			int row = cell % xnGridYdim;
			cropMapILMS->setValue(row, col, 0);
		}
		
	}
	//debugging
	//printf("In <xn_mpmas_translator::getManagementForCell>: Cell: %d, LUA: %d, CropCode: %s\n",cell, lua,management.CropCode);
	
	//adapt sowing date if adaptive
	map<int, cropAdaptationParameters>::iterator adaptIt = LuaAdaptationParameters.find(lua);
	if (adaptIt == LuaAdaptationParameters.end())
	{
		std::stringstream errmsg;
		errmsg << "Error in xn_mpmas_translator::getManagementForCell:  No Adaptation parameters defined for land use activity " 
			   << lua <<"\n";
		throw runtime_error(errmsg.str());
	}
	if ( adaptIt->second.sowingDayFormula != notAdaptive ) 
	{
			int sowDoy = xnmpmas::adaptation::calculatePlantingDate(&(adaptIt->second), xnGridWeatherHistory[cell], historyWeighting, convertDateToDayOfYear(startDate) - 1);

			int springCrop = 0;
			if(ws)
				springCrop = xnmpmas::adaptation::isSpringCrop(adaptIt->second.sowingDayFormula);
				
			management.sowDate = convertDayOfYearToDate( sowDoy, mpmasYear + springCrop);
			
/*			if (cropMapILMS !=  NULL && sowMapILMS != NULL) 
			{
				int col = cell / xnGridYdim; //integer division
				int row = cell % xnGridYdim;
			
				if (cropMapILMS->getValue(row, col) > 0 )
				{
					sowMapILMS->setValue(row, col, sowDoy);
				}
			}*/
	}
	else 
	{
			management.sowDate.year = management.sowDate.year + mpmasYear - 1; // -1 because in management information years are stored as 01 and 02, respectively
			
	/*		if (cropMapILMS !=  NULL && sowMapILMS != NULL) 
			{
				int col = cell / xnGridYdim; //integer division
				int row = cell % xnGridYdim;
			
				if (cropMapILMS->getValue(row, col) > 0 )
				{
					sowMapILMS->setValue(row, col, convertDateToDayOfYear( management.sowDate) );
				}
			}*/
	}
	
	//sowing date for ILMS
	//currently not the sow date of the actual crop is used, but the sow date for that cell of a representative growing activity for
	//early resp. late crops
	if (sowMapILMSearly != NULL  ) 
	{

		
		if (sowDateRepresentatives != NULL && (*sowDateRepresentatives)[0]  > -1)
		{
			int col = cell / xnGridYdim; //integer division
			int row = cell % xnGridYdim;
			int repEarlyLua = (*sowDateRepresentatives)[0];
			
			map<int, STRUCT_mpmas_to_xn>::iterator repEarlyManagIt = LuaXnParameters.find(repEarlyLua); //Note: LuaXnParameters must contain something for empty cells under lua -1
			if (repEarlyManagIt == LuaXnParameters.end())
			{
				std::stringstream errmsg;
				errmsg << "Error in xn_mpmas_translator::getManagementForCell:  No Expert-N management defined for land use activity " 
					   << repEarlyLua <<" representing early-removed land use in ILMS\n";
					   
				throw runtime_error(errmsg.str());
			}
			STRUCT_mpmas_to_xn repEarlyManagement = (*repEarlyManagIt).second;
			
			int sowDoy = -1;
			
			map<int, cropAdaptationParameters>::iterator repEarlyAdaptIt = LuaAdaptationParameters.find(repEarlyLua);
			if (repEarlyAdaptIt == LuaAdaptationParameters.end())
			{
				std::stringstream errmsg;
				errmsg << "Error in xn_mpmas_translator::getManagementForCell:  No Adaptation parameters defined for land use activity " 
					   << repEarlyLua <<" representing early-removed land use in ILMS\n";
				throw runtime_error(errmsg.str());
			}
			if ( repEarlyAdaptIt->second.sowingDayFormula != notAdaptive ) 
			{
					sowDoy = xnmpmas::adaptation::calculatePlantingDate(&(repEarlyAdaptIt->second), xnGridWeatherHistory[cell], historyWeighting, convertDateToDayOfYear(startDate) - 1 );
			}
			else 
			{
					sowDoy = convertDateToDayOfYear( repEarlyManagement.sowDate);
			}		
			sowMapILMSearly->setValue(row, col, sowDoy );
		}
		
	}
	if (sowMapILMSlate != NULL  ) 
	{
		if (sowDateRepresentatives != NULL && (*sowDateRepresentatives)[1]  > -1)
		{
			int col = cell / xnGridYdim; //integer division
			int row = cell % xnGridYdim;
		
			
			int repLateLua = (*sowDateRepresentatives)[1];
			
			map<int, STRUCT_mpmas_to_xn>::iterator repLateManagIt = LuaXnParameters.find(repLateLua); //Note: LuaXnParameters must contain something for empty cells under lua -1
			if (repLateManagIt == LuaXnParameters.end())
			{
				std::stringstream errmsg;
				errmsg << "Error in xn_mpmas_translator::getManagementForCell:  No Expert-N management defined for land use activity " 
					   << repLateLua <<" representing late-covering land use in ILMS\n";
				throw runtime_error(errmsg.str());
			}
			STRUCT_mpmas_to_xn repLateManagement = (*repLateManagIt).second;
			
			int sowDoy = -1;
			
			map<int, cropAdaptationParameters>::iterator repLateAdaptIt = LuaAdaptationParameters.find(repLateLua);
			if (repLateAdaptIt == LuaAdaptationParameters.end())
			{
				std::stringstream errmsg;
				errmsg << "Error in xn_mpmas_translator::getManagementForCell:  No Adaptation parameters defined for land use activity " 
					   << repLateLua <<" representing late-covering land use in ILMS\n";
				throw runtime_error(errmsg.str());
			}
			if ( repLateAdaptIt->second.sowingDayFormula != notAdaptive ) 
			{
					sowDoy = xnmpmas::adaptation::calculatePlantingDate(&(repLateAdaptIt->second), xnGridWeatherHistory[cell], historyWeighting, convertDateToDayOfYear(startDate) - 1);
			}
			else 
			{
					sowDoy = convertDateToDayOfYear( repLateManagement.sowDate);
			}		
			sowMapILMSlate->setValue(row, col, sowDoy );
		}
		
	}
	
	//Adapt dates from relative to current year
	//Note: input file should have year = 1 for everything done in the year when sowing of first crop takes place (i.e. sowing of winter crops in Germany)
	// and year = 2 for everything done a year later (e.g. harvest of winter crops in Germany)

	management.harvestDate.year = management.harvestDate.year + mpmasYear -1 ;
	for (int i = 0; i < management.numMinFert; i++)
	{
		management.mineralFertilization[i].fertDate.year =  management.mineralFertilization[i].fertDate.year + mpmasYear -1;
	}
	for (int i = 0; i < management.numOrgFert; i++)
	{
		management.organicFertilization[i].orgfertDate.year =  management.organicFertilization[i].orgfertDate.year + mpmasYear -1;
	}
	for (int i = 0; i < management.numIrrig; i++)
	{
		management.irrigation[i].irrDate.year =  management.irrigation[i].irrDate.year + mpmasYear -1;
	}
	for (int i = 0; i < management.numTill; i++)
	{
		management.tillage[i].tillDate.year =  management.tillage[i].tillDate.year + mpmasYear -1;
	}
	management.coverCropSowDate.year = management.coverCropSowDate.year + mpmasYear -1;
	management.coverCropPloughUnderDate.year = management.coverCropPloughUnderDate.year + mpmasYear -1;
	//management.stopDate = 
	return management;
}
void xn_mpmas_translator::calcYieldsToMaps(const STRUCT_xn_to_mpmas* grid_xn_to_mpmas, Raster2D* yieldMap1, Raster2D* yieldMap2, vector<Raster2D>& cropExtraAttrRasters, int overlapping, string fnAggXnOutput)
{
	yieldMap1->setAllValues(-1.0);
	yieldMap2->setAllValues(-1.0);
	
	size_t numExtra = cropExtraAttrRasters.size();
	
	for (size_t j = 0; j < numExtra; ++j)
	{
		cropExtraAttrRasters[j].setAllValues(-1.0);
	}
	
	vector<int>* relevantXnGrid = &currentXnCropGrid;
	if (overlapping)
		relevantXnGrid = &previousXnCropGrid;
	
	
	FILE* dbgXnActualDates = fopen(fnAggXnOutput.c_str(), "w" );
	fprintf(dbgXnActualDates, "x\ty\tgrid\tLUA\tCropCode\tVariety\tYieldMPMAS\t"
							  "FruitDM\tStem+LeaveDM\tStemDM\tharvest_date\t"
							  "minfert_date0\tminfert_N0\tminfert_date1\tminfert_N1\tminfert_date2\tminfert_N2\tminfert_date3\tminfert_N3\tNmin0_30\tNmin30_60\tNmin60_90\t"
							  "sow_date\tcovercrop_sow_date"
							  "\n"
	);
	
	
	
	for (int row = 0; row < xnGridYdim; ++row)
	{ 	for (int col = 0; col < xnGridXdim; ++col)
		{
			int ci = xnGridYdim * col + row;
			int lua = (*relevantXnGrid)[ci];				
			int gridId = LuaXnParameters[lua].currentGrid;
			
			map<int,luaInfo>::iterator it2 = LuaCouplingParameters.find(lua);
			
			
			map<int, STRUCT_mpmas_to_xn>::iterator managIt = LuaXnParameters.find(lua); //Note: LuaXnParameters must contain something for empty cells under lua -1
			

			double mpmasYield = grid_xn_to_mpmas[gridId * xnGridSize + ci].fruitDryWeight * it2->second.yieldFactor
							+ grid_xn_to_mpmas[gridId * xnGridSize + ci].stemLeafDryWeight * it2->second.stoverYieldFactor 
							+ grid_xn_to_mpmas[gridId * xnGridSize + ci].stemOnlyDryWeight * it2->second.stemYieldFactor ;


			yieldMap1->setValue(row, col, mpmasYield);
			
			yieldMap2->setValue(row, col, 0.0 );


			if (numExtra > 0)
				cropExtraAttrRasters[0].setValue(row, col,  convertDateToDayOfYear(grid_xn_to_mpmas[gridId * xnGridSize +ci].actualSowDate) );

			if (numExtra > 1)
				cropExtraAttrRasters[1].setValue(row, col,  convertDateToDayOfYear(grid_xn_to_mpmas[gridId * xnGridSize +ci].actualHarvestDate) );

			if (numExtra > 2)
				cropExtraAttrRasters[2].setValue(row, col,  convertDateToDayOfYear(grid_xn_to_mpmas[gridId * xnGridSize +ci].actualCoverCropSowDate) );

			if (numExtra > 3)
				cropExtraAttrRasters[3].setValue(row, col,  convertDateToDayOfYear(grid_xn_to_mpmas[gridId * xnGridSize +ci].actualMinFertDate[0]) );

			if (numExtra > 4)
				cropExtraAttrRasters[4].setValue(row, col,  grid_xn_to_mpmas[gridId * xnGridSize +ci].actualTotalFertN[0] );

			if (numExtra > 5)
				cropExtraAttrRasters[5].setValue(row, col,  convertDateToDayOfYear(grid_xn_to_mpmas[gridId * xnGridSize +ci].actualMinFertDate[1]) );

			if (numExtra > 6)
				cropExtraAttrRasters[6].setValue(row, col,  grid_xn_to_mpmas[gridId * xnGridSize +ci].actualTotalFertN[1] );

			if (numExtra > 7)
				cropExtraAttrRasters[7].setValue(row, col,  convertDateToDayOfYear(grid_xn_to_mpmas[gridId * xnGridSize +ci].actualMinFertDate[2]) );

			if (numExtra > 8)
				cropExtraAttrRasters[8].setValue(row, col,  grid_xn_to_mpmas[gridId * xnGridSize +ci].actualTotalFertN[2] );

			if (numExtra > 9)
				cropExtraAttrRasters[9].setValue(row, col,  convertDateToDayOfYear(grid_xn_to_mpmas[gridId * xnGridSize +ci].actualMinFertDate[3]) );

			if (numExtra > 10)
				cropExtraAttrRasters[10].setValue(row, col,  grid_xn_to_mpmas[gridId * xnGridSize +ci].actualTotalFertN[3] );

			if (numExtra > 11)
				cropExtraAttrRasters[11].setValue(row, col,  grid_xn_to_mpmas[gridId * xnGridSize +ci].Nmin0_30 
															+ grid_xn_to_mpmas[gridId * xnGridSize +ci].Nmin30_60 
															+ grid_xn_to_mpmas[gridId * xnGridSize +ci].Nmin60_90 );




			fprintf(dbgXnActualDates, "%d\t%d\t%d\t%d\t%s\t%s"
							"\t%01.2f\t%01.2f\t%01.2f\t%01.2f"
							"\t%02d-%02d-%02d\t"
							"%02d-%02d-%02d\t%01.2f\t"
							"%02d-%02d-%02d\t%01.2f\t"
							"%02d-%02d-%02d\t%01.2f\t"
							"%02d-%02d-%02d\t%01.2f"
							"\t%01.2f\t%01.2f\t%01.2f"
							"\t%02d-%02d-%02d\t%02d-%02d-%02d"
							"\n",
							col, row, gridId, lua, managIt->second.CropCode, managIt->second.variety,
							
							mpmasYield,
							grid_xn_to_mpmas[gridId * xnGridSize + ci].fruitDryWeight,
							grid_xn_to_mpmas[gridId * xnGridSize + ci].stemLeafDryWeight,
							grid_xn_to_mpmas[gridId * xnGridSize + ci].stemOnlyDryWeight,
							
							grid_xn_to_mpmas[gridId * xnGridSize +ci].actualHarvestDate.day,
							grid_xn_to_mpmas[gridId * xnGridSize +ci].actualHarvestDate.month,
							grid_xn_to_mpmas[gridId * xnGridSize +ci].actualHarvestDate.year,
							
							grid_xn_to_mpmas[gridId * xnGridSize +ci].actualMinFertDate[0].day,
							grid_xn_to_mpmas[gridId * xnGridSize +ci].actualMinFertDate[0].month,
							grid_xn_to_mpmas[gridId * xnGridSize +ci].actualMinFertDate[0].year,
							grid_xn_to_mpmas[gridId * xnGridSize +ci].actualTotalFertN[0],
							
							
							grid_xn_to_mpmas[gridId * xnGridSize +ci].actualMinFertDate[1].day,
							grid_xn_to_mpmas[gridId * xnGridSize +ci].actualMinFertDate[1].month,
							grid_xn_to_mpmas[gridId * xnGridSize +ci].actualMinFertDate[1].year,
							grid_xn_to_mpmas[gridId * xnGridSize +ci].actualTotalFertN[1],
							grid_xn_to_mpmas[gridId * xnGridSize +ci].actualMinFertDate[2].day,
							grid_xn_to_mpmas[gridId * xnGridSize +ci].actualMinFertDate[2].month,
							grid_xn_to_mpmas[gridId * xnGridSize +ci].actualMinFertDate[2].year,
							grid_xn_to_mpmas[gridId * xnGridSize +ci].actualTotalFertN[2],
							grid_xn_to_mpmas[gridId * xnGridSize +ci].actualMinFertDate[3].day,
							grid_xn_to_mpmas[gridId * xnGridSize +ci].actualMinFertDate[3].month,
							grid_xn_to_mpmas[gridId * xnGridSize +ci].actualMinFertDate[3].year,
							grid_xn_to_mpmas[gridId * xnGridSize +ci].actualTotalFertN[3],
							
							grid_xn_to_mpmas[gridId * xnGridSize +ci].Nmin0_30,
							grid_xn_to_mpmas[gridId * xnGridSize +ci].Nmin30_60,
							grid_xn_to_mpmas[gridId * xnGridSize +ci].Nmin60_90,
							
							grid_xn_to_mpmas[gridId * xnGridSize +ci].actualSowDate.day,
							grid_xn_to_mpmas[gridId * xnGridSize +ci].actualSowDate.month,
							grid_xn_to_mpmas[gridId * xnGridSize +ci].actualSowDate.year,
							
							grid_xn_to_mpmas[gridId * xnGridSize +ci].actualCoverCropSowDate.day,
							grid_xn_to_mpmas[gridId * xnGridSize +ci].actualCoverCropSowDate.month,
							grid_xn_to_mpmas[gridId * xnGridSize +ci].actualCoverCropSowDate.year
						);


		}
	}
	
}

void xn_mpmas_translator::calcYieldsToArray(const STRUCT_xn_to_mpmas* grid_xn_to_mpmas,  double* yieldArray, double* stoverYieldArray, double** extraAttrsX,  int numExtra,  int overlapping)
{
	multimap<int, int>* CellAssociationMapToUse = &CurrentCellsForLua;
	if (overlapping)
		CellAssociationMapToUse = &PreviousCellsForLua;
	
	vector<int> resultsToCopy;
	for (int l = 0; l < mpmasArraySize; l++)
	{
		//if(cropAreasArray[l] > 0 && cropActIdsArray[l] > -1)
		if( cropActIdsArray[l] > -1)
		{
			multimap<int,int>::iterator it, itlow, itup;
			int lua = cropActIdsArray[l];	
			int gridId = LuaXnParameters[lua].currentGrid;
			
			itlow = CellAssociationMapToUse->lower_bound(lua);
			itup = CellAssociationMapToUse->upper_bound(lua);
			int count = 0;
			double harvest = 0.0;
			double stoverHarvest = 0.0;
			double stemHarvest = 0.0;
			for (int i = 0; i < numExtra; ++i)
				extraAttrsX[l][i] = 0.0;
			
			for (it = itlow; it != itup; it++)
			{
					int c = it->second;
					
					if (c < 0 )
					{
						resultsToCopy.push_back(l);
						count = 0;
						break;
					}	
					
					else
					{
						count++;
						harvest += grid_xn_to_mpmas[gridId * xnGridSize + c].fruitDryWeight;
						stoverHarvest += grid_xn_to_mpmas[gridId * xnGridSize + c].stemLeafDryWeight;
						stemHarvest   += grid_xn_to_mpmas[gridId * xnGridSize + c].stemOnlyDryWeight;
						if (numExtra > 0)
							extraAttrsX[l][0] += convertDateToDayOfYear(grid_xn_to_mpmas[gridId * xnGridSize +c].actualSowDate);
						if (numExtra > 1)
							extraAttrsX[l][1] += convertDateToDayOfYear(grid_xn_to_mpmas[gridId * xnGridSize +c].actualHarvestDate);
						if (numExtra > 2)
							extraAttrsX[l][2] += convertDateToDayOfYear(grid_xn_to_mpmas[gridId * xnGridSize +c].actualCoverCropSowDate);	
						if (numExtra > 3)
							extraAttrsX[l][3] += convertDateToDayOfYear(grid_xn_to_mpmas[gridId * xnGridSize +c].actualMinFertDate[0]);								
						if (numExtra > 4)
							extraAttrsX[l][4] += grid_xn_to_mpmas[gridId * xnGridSize +c].actualTotalFertN[0];								
						if (numExtra > 5)
							extraAttrsX[l][5] += convertDateToDayOfYear(grid_xn_to_mpmas[gridId * xnGridSize +c].actualMinFertDate[1]);								
						if (numExtra > 6)
							extraAttrsX[l][6] += grid_xn_to_mpmas[gridId * xnGridSize +c].actualTotalFertN[1];								
						if (numExtra > 7)
							extraAttrsX[l][7] += convertDateToDayOfYear(grid_xn_to_mpmas[gridId * xnGridSize +c].actualMinFertDate[2]);								
						if (numExtra > 8)
							extraAttrsX[l][8] += grid_xn_to_mpmas[gridId * xnGridSize +c].actualTotalFertN[2];								
						if (numExtra > 9)
							extraAttrsX[l][9] += convertDateToDayOfYear(grid_xn_to_mpmas[gridId * xnGridSize +c].actualMinFertDate[3]);								
						if (numExtra > 10)
							extraAttrsX[l][10] += grid_xn_to_mpmas[gridId * xnGridSize +c].actualTotalFertN[3];								
						if (numExtra > 11)
							extraAttrsX[l][11] += grid_xn_to_mpmas[gridId * xnGridSize +c].Nmin0_30 
															+ grid_xn_to_mpmas[gridId * xnGridSize +c].Nmin30_60 
															+ grid_xn_to_mpmas[gridId * xnGridSize +c].Nmin60_90;								


					}
			}
			if (count > 0)
			{	map<int,luaInfo>::iterator it2 = LuaCouplingParameters.find(lua);
				yieldArray[l] = (harvest / count) * it2->second.yieldFactor +  (stoverHarvest / count) * it2->second.stoverYieldFactor  + (stemHarvest /count) * it2->second.stemYieldFactor ;
				stoverYieldArray[l] = 0.0;
				
				for (int i = 0; i < numExtra; ++i) 
				{
						extraAttrsX[l][i] /= count;
				}
			}
			else 
			{
				yieldArray[l] = 0.0;
				stoverYieldArray[l] = 0.0;
				
				for (int i = 0; i < numExtra; ++i) 
				{
						extraAttrsX[l][i] = 0.0;
				}
			}
		}
		else
		{
			yieldArray[l] = 0.0;
			stoverYieldArray[l] = 0.0;
			for (int i = 0; i < numExtra; ++i) 
			{
				extraAttrsX[l][i] = 0.0;
			}
		}
	}
	for (vector<int>::iterator it3 = resultsToCopy.begin(); it3 != resultsToCopy.end(); it3++)
	{
		int l2 = *it3;
		int lua = cropActIdsArray[l2];
		multimap<int,int>::iterator itlow2 = CellAssociationMapToUse->lower_bound(lua);
		int luaToCopy = -1 * (*itlow2).second;
		int l;
		for ( l= 0; l < mpmasArraySize; l++)
		{
			if (cropActIdsArray[l] == luaToCopy)
				break;
		}
		if (l == mpmasArraySize )
		{
			std::stringstream errmsg;
			errmsg << "Error in xn_mpmas_translator::calcYieldsToArray:  LUA " << luaToCopy 
					<< " (required to provide results for LUA "<<lua <<") does not exist\n";	  
			throw runtime_error(errmsg.str());
		}	
		yieldArray[l2] = yieldArray[l];
		stoverYieldArray[l2] = stoverYieldArray[l];
	}
}
void xn_mpmas_translator::setXnGridDimensions (int xdim, int ydim, int zdim)
{
	xnGridYdim = ydim;
	xnGridXdim = xdim;
	xnGridZdim = zdim;
	xnGridSize = ydim * xdim;
}
void xn_mpmas_translator::readAdaptationParametersAndWeatherHistory(std::string fnParams, std::string fnHistory, int cellSpecificWeather, std::string fnMap)
{
	
	unsigned historySize;
	int ws = 1; //set to zero if years do not overlap
	
	//read adaptation parameters file first to know the necessary size of history
	{	YAML::Node node;
		try {
			node = YAML::LoadFile(fnParams);	
		}
		catch (exception &e)
		{	stringstream newE; 
			newE << e.what() << "\nError when reading " << fnParams << endl ;
			throw runtime_error(newE.str());
		}
		if(! node["history-size"])
		{
			std::stringstream errmsg;
			errmsg << "Error in " << fnParams << ": "
				   << "Missing entry 'history-size'\n";
			throw runtime_error(errmsg.str());
		}
		historySize = getValueFromYamlNode<unsigned>(node["history-size"], fnParams,"history-size ");
		
		if(! node["history-weights"])
		{
			std::stringstream errmsg;
			errmsg << "Error in " << fnParams << ": "
				   << "Missing entry 'history-weights'\n";
			throw runtime_error(errmsg.str());
		}
		if(! node["history-weights"].IsSequence())
		{
			std::stringstream errmsg;
			errmsg << "Error in " << fnParams << ": "
				   << "Wrong format for 'history-weights'. Expected a sequence\n";
			throw runtime_error(errmsg.str());
		}
		if(  node["history-weights"].size() !=  historySize )
		{
			std::stringstream errmsg;
			errmsg << "Error in " << fnParams << ": "
				   << "Size of 'history-weights' != 'history-size'\n";
			throw runtime_error(errmsg.str());
		}	
		for (unsigned i = 0; i < node["history-weights"].size(); ++i)
			historyWeighting.push_back(getValueFromYamlNode<double>(node["history-weights"][i], fnParams,"history-weights, position ", i));
	}//end block, node goes out of scope here

	//Create and zero-initialize weather records for all cells
	xnGridWeatherHistory.resize(xnGridSize);
	for (size_t i = 0; i < xnGridWeatherHistory.size(); ++i)
	{
		xnGridWeatherHistory[i].resize(historySize + ws);
		for (size_t j = 0; j < historySize + ws; ++j)
		{	xnGridWeatherHistory[i][j].numDays = 365;
			xnGridWeatherHistory[i][j].year = startDate.year  - j;
		}
	}
	
	if(cellSpecificWeather)
	{
		ifstream infile(fnHistory.c_str());
		
		//First line should be file header, check if looks correct and discard
		string line;
		getline(infile, line);
		if ( line != "year\tdayofyear\tx\ty\tairtemp\tsoiltemp")
		{	cerr << "Warning: unexpected header in " << fnHistory << ":\n"
				   << "Found: " <<line<<"\nExpected: year\tdayofyear\tx\ty\tairtemp\tsoiltemp\n";
		}
		//loop over remaining lines	
		while (	getline(infile, line) )
		{
			stringstream lineS(line); //make stream from extracted line 
			int year, dayofyear, x, y;
			double airtemp, soiltemp;
			
			lineS >> year;
			//ignore years that we do not need
			if (year > startDate.year || startDate.year  - year > (int) historySize ) 
				continue;
			
			lineS >> dayofyear >> x >> y >> airtemp >> soiltemp;
			if (dayofyear < 1 || dayofyear > 366 || x < 0 || y < 0 || x >= xnGridXdim || y >= xnGridYdim )
			{
				std::stringstream errmsg;
				errmsg << "Error in " << fnHistory << ":\n"
				   << "Invalid input" << year <<" "<< dayofyear<<" "<< x <<" "<< y <<" "<< airtemp <<" "<< soiltemp <<  "\n";
				throw runtime_error(errmsg.str());
			}
			try 
			{	xnGridWeatherHistory.at(x * xnGridYdim + y).at(startDate.year  - year).airTemp[dayofyear - 1] = airtemp;
				xnGridWeatherHistory.at(x * xnGridYdim + y).at(startDate.year  - year).topsoilTemp[dayofyear - 1] = soiltemp;
				if(dayofyear == 366)
					xnGridWeatherHistory.at(x * xnGridYdim + y).at(startDate.year  - year).numDays = 366;
			}
			catch (exception& e)
			{	std::stringstream errmsg;
				errmsg << "Error in <readAdaptationParametersAndWeatherHistory>, file " << fnHistory << ":\n"
				<< "Cell or year index out of bounds\n:" << e.what() <<"\n";
				throw runtime_error(errmsg.str());
			}	
		}
	}
	else // weather not cell specific 
	{ 	//first load raster map indicating weather station for each cell
		
		//Note not using Raster2D here, as we may need to cope with long integer IDs and Raster2D is double-based.
		
		//Raster2D cellToStationMap = Raster2D(fnMap.c_str());
		
		
		FILE* cellToStationMapStream = fopen(fnMap.c_str(), "r");
		if(NULL == cellToStationMapStream )
		{
			stringstream errmsg;
			errmsg  <<  "Error when trying to open:" << fnMap <<  "\n";
			throw runtime_error(errmsg.str());
		}
		int cellToStationMap_rows, cellToStationMap_cols; 
		double xllC, yllC, noData,cellSize;

		char varString [254];
		
		int rtcod = fscanf(cellToStationMapStream, "%253s %d", varString, &cellToStationMap_cols);
		if (rtcod != 2)
		{
				stringstream errmsg;
				errmsg  <<  "Error when reading NCOLS in " << fnMap <<  "\n";
				throw runtime_error(errmsg.str());
			
		}
		
		rtcod = fscanf(cellToStationMapStream, "%253s %d", varString, &cellToStationMap_rows);
		if (rtcod != 2)
		{
			stringstream errmsg;
				errmsg  <<  "Error when reading NROWS in " << fnMap <<  "\n";
				throw runtime_error(errmsg.str());
		}
		
		
		
		if ( xnGridXdim != cellToStationMap_cols || xnGridYdim != cellToStationMap_rows  )
		{	stringstream errmsg;
			errmsg  << "Error: Dimensions ("<<cellToStationMap_cols << ","<< cellToStationMap_rows 
					<<")  of cellWeatherRecordsLinkMap inconsistent with dimensions (" 
					<< xnGridXdim << "," << xnGridYdim <<")  of XnCellRaster\n";
			throw runtime_error(errmsg.str() );
		}
		
		rtcod = fscanf(cellToStationMapStream, "%253s %lf", varString, &xllC);
		if (rtcod != 2)
		{
			stringstream errmsg;
				errmsg  <<  "Error when reading XLLCORNER in " << fnMap <<  "\n";
				throw runtime_error(errmsg.str());
		}
		rtcod = fscanf(cellToStationMapStream, "%253s %lf", varString, &yllC);
		if (rtcod != 2)
		{
			stringstream errmsg;
				errmsg  <<  "Error when reading YLLCORNER in " << fnMap <<  "\n";
				throw runtime_error(errmsg.str());
		}
		rtcod = fscanf(cellToStationMapStream, "%253s %lf", varString, &cellSize);
		if (rtcod != 2)
		{
			stringstream errmsg;
				errmsg  <<  "Error when reading CELLSIZE in " << fnMap <<  "\n";
				throw runtime_error(errmsg.str());
		}
		rtcod = fscanf(cellToStationMapStream, "%253s %lf", varString, &noData);
		if (rtcod != 2)
		{
			stringstream errmsg;
				errmsg  <<  "Error when reading NODATA_VALUE in " << fnMap <<  "\n";
				throw runtime_error(errmsg.str());
		}
		
		multimap<long long,int> cellsByStation;//key = station, value = cells associated to that station
		for (int y = xnGridYdim -1; y >= 0 ; --y)	
		{	for (int x = 0; x < xnGridXdim; ++x)
			{	
				long long s;
				rtcod = fscanf(cellToStationMapStream, "%lld", &s);
				if (rtcod != 1)
				{
					stringstream errmsg;
					errmsg  <<  "Error when reading station id for cell " << x << ", " << y << " in " << fnMap <<  "\n";
					throw runtime_error(errmsg.str());
				}				
				cellsByStation.insert(pair<long long,int>(s, x * xnGridYdim + y));
			}
		}
		fclose(cellToStationMapStream);
		
		cout << "cellsByStation:\n";
		for (multimap<long long,int>::iterator it =  cellsByStation.begin(); it != cellsByStation.end(); it++)
		{
			cout << it->first << "\t" << it->second << "\n";
		}
		cout.flush();

		
		ifstream infile(fnHistory.c_str());			
		if (infile.fail())
		{		std::stringstream errmsg;
				errmsg << "Error when trying to open " << fnHistory << "\n";
				throw runtime_error(errmsg.str());
		}
		
		
		//First line should be file header, check if looks correct and discard
		string line;
		getline(infile, line);
		if ( line != "year\tdayofyear\tstation\tairtemp\tsoiltemp")
		{	
			cerr << "Warning: unexpected header in " << fnHistory << ":\n"
				   << "Found: " <<line<<"\nExpected: year\tdayofyear\tx\ty\tairtemp\tsoiltemp\n";
		}
		//save iterators to avoid searching everytime
		// initialize to first station's values in map
		multimap<long long,int>::iterator lastItLow = cellsByStation.begin();
		multimap<long long,int>::iterator lastItUpp = cellsByStation.upper_bound(lastItLow->first);

		//loop over remaining lines	
		while (	getline(infile, line) )
		{
			
			stringstream lineS(line); //make stream from extracted line 
			int year, dayofyear; long long station;
			double airtemp, soiltemp;
			
			lineS >> year;
			if (lineS.fail() )
			{
				std::stringstream errmsg;
				errmsg << "Error in " << fnHistory << " when reading "<< lineS.str() <<"\n";
				throw runtime_error(errmsg.str());
			}
			//ignore years that we do not need
			if (year > startDate.year  || (startDate.year  - year) > (int) historySize ) 
				continue;
			
			lineS >> dayofyear >> station >> airtemp >> soiltemp;
			if (lineS.fail() )
			{
				std::stringstream errmsg;
				errmsg << "Error in " << fnHistory << " when reading "<< year<<"\t"<< lineS.str() <<"\n";
				throw runtime_error(errmsg.str());
			}
			if (dayofyear < 1 || dayofyear > 366)
			{
				fprintf(stderr, "Error in %s: Invalid input %d %d %lld %f %f\n", fnHistory.c_str(), year, dayofyear, station, airtemp, soiltemp );
				std::stringstream errmsg;
				errmsg << "Error in " << fnHistory << ":\n"
				<< "Invalid input" << year <<" "<< dayofyear<<" "<< station <<" "<< airtemp <<" "<< soiltemp <<  "\n";
				throw runtime_error(errmsg.str());
			}
			
			multimap<long long,int>::iterator itLow, itUpp;
			if (lastItLow->first  == station )
			{	itLow =  lastItLow;
				itUpp = lastItUpp;
			}
			else
			{
				lastItLow = itLow =  cellsByStation.lower_bound(station);
				lastItUpp = itUpp = cellsByStation.upper_bound(station);
			}
			
			//loop over all cells associated to that station and insert the value into the corresponding record
			for ( multimap<long long,int>::iterator itStationCells = itLow;
						itStationCells != itUpp;
						itStationCells++)
			{
				int cell = itStationCells->second;
				try {
					xnGridWeatherHistory.at(cell).at(startDate.year  - year).airTemp[dayofyear - 1] = airtemp;
					xnGridWeatherHistory.at(cell).at(startDate.year  - year).topsoilTemp[dayofyear - 1] = soiltemp;
					if(dayofyear == 366)
						xnGridWeatherHistory.at(cell).at(startDate.year  - year).numDays = 366;
						
				}
				catch (exception& e)
				{	std::stringstream errmsg;
					errmsg << "Error in <readAdaptationParametersAndWeatherHistory>, file " << fnHistory << ":\n"
					<< "Cell or year index out of bounds\n:" << e.what() <<"\n";
					throw runtime_error(errmsg.str());
				}	
			}
		}
	
	}
	lastWeatherUpDate = startDate;
	//TODO: check completenes of weatherHistory (years should be historySize + 1 to account for the calendar year/season overlap
}
int xn_mpmas_translator::getStartDoy(int currentyear)
{
	xnmpmasDate thisStartDate = startDate;
	thisStartDate.year = currentyear;
	return convertDateToDayOfYear(thisStartDate);
}

void xn_mpmas_translator::updateWeatherHistory(const STRUCT_xn_to_mpmas2* grid_xn_to_mpmas2, int thisSeasonsStartyear, xnmpmasDate curDate)
{   //copy information on cell-specific weather data from XN/MPMAS shared array into weatherHistory
	
	int startDoy, endDoy, year1, year2;
	
	startDoy = convertDateToDayOfYear(lastWeatherUpDate);
	endDoy = convertDateToDayOfYear(curDate) -1;
	
	year1 = lastWeatherUpDate.year;
	year2 = curDate.year;
	
	cout << "Updating weather " << startDoy << "/" << year1 << " to " << endDoy << "/" << year2 << "\n";
	
	int daysYear1 = isLeapYear(year1) ? 366 : 365;
	//int daysYear2 = isLeapYear(year2) ? 366 : 365;
	if (endDoy <= 0) {
		year2 = year1;
		endDoy =  daysYear1;
		//daysYear2 = daysYear2;
	}
	
			
//	int startDoy1 = getStartDoy(thisSeasonsStartyear);
//	int startDoy2 = getStartDoy(thisSeasonsStartyear + 1);
	
	for (int i = 0; i < xnGridSize; ++i)
	{	
		int lua = currentXnCropGrid.at(i);//TODO: switch correctly !!!
		int gridId = LuaXnParameters.at(lua).currentGrid;
		
		if (year1 == year2 && startDoy <= endDoy) 
		{	
			if (xnGridWeatherHistory[i][0].year == year1)	
			{
				for (int j = startDoy-1; j < endDoy; ++j)
				{	// -1 to convert from doy (>=1) to array position (>=0)
					xnGridWeatherHistory[i][0].airTemp[j] = grid_xn_to_mpmas2[gridId * xnGridSize + i].airTemp[j];
					xnGridWeatherHistory[i][0].topsoilTemp[j] = grid_xn_to_mpmas2[gridId * xnGridSize + i].topsoilTemp[j];
				}
				xnGridWeatherHistory[i][0].numDays = endDoy ;

			}
			else if (xnGridWeatherHistory[i][0].year == year1 - 1 )	
			{
				weatherRecord temp = weatherRecord(year1);
				for (int j = startDoy-1; j < endDoy; ++j)
				{
					temp.airTemp[j] = grid_xn_to_mpmas2[gridId * xnGridSize + i].airTemp[j];
					temp.topsoilTemp[ j]  = grid_xn_to_mpmas2[gridId * xnGridSize + i].topsoilTemp[j];
				}
				temp.numDays = endDoy ;
				
				//remove last record ??
				xnGridWeatherHistory[i].pop_back();
				//add new record at beginning
				xnGridWeatherHistory[i].push_front(temp);				
			}
			else 
			{
				std::stringstream errmsg;
					errmsg << "Error in <updateWeatherHistory>:\n"
					<< "Gap in weather history: New year " << year1 << ", last existing year " << xnGridWeatherHistory[i][0].year <<"\n";
					throw runtime_error(errmsg.str());
			}
		}
		else if (year1 == year2 - 1 ) 
		{	
			if (xnGridWeatherHistory[i][0].year == year1)	
			{
				for (int j = startDoy-1; j < daysYear1; ++j)
				{	// -1 to convert from doy (>=1) to array position (>=0)
					xnGridWeatherHistory[i][0].airTemp[j] = grid_xn_to_mpmas2[gridId * xnGridSize + i].airTemp[j];
					xnGridWeatherHistory[i][0].topsoilTemp[j] = grid_xn_to_mpmas2[gridId * xnGridSize + i].topsoilTemp[j];
				}
				xnGridWeatherHistory[i][0].numDays = daysYear1 ;
			}
			else if (xnGridWeatherHistory[i][0].year == year1 - 1 )	
			{
				weatherRecord temp = weatherRecord(year1);
				for (int j = startDoy -1 ; j < daysYear1; ++j)
				{
					temp.airTemp[j] = grid_xn_to_mpmas2[gridId * xnGridSize + i].airTemp[j];
					temp.topsoilTemp[ j]  = grid_xn_to_mpmas2[gridId * xnGridSize + i].topsoilTemp[j];
				}
				temp.numDays = daysYear1 ;
				
				//remove last record ??
				xnGridWeatherHistory[i].pop_back();
				//add new record at beginning
				xnGridWeatherHistory[i].push_front(temp);				
			}
			else 
			{
				std::stringstream errmsg;
					errmsg << "Error in <updateWeatherHistory>:\n"
					<< "Gap in weather history: New year " << year1 << ", last existing year " << xnGridWeatherHistory[i][0].year <<"\n";
					throw runtime_error(errmsg.str());
			}
		
		
			if (xnGridWeatherHistory[i][0].year == year2)	
			{
					std::stringstream errmsg;
					errmsg << "Error in <updateWeatherHistory>:\n"
					<< "Unexpected case: New year (second in recevied info) " << year2 << "already exists, last existing year " << xnGridWeatherHistory[i][0].year <<"\n";
					throw runtime_error(errmsg.str());
			}
			else if (xnGridWeatherHistory[i][0].year == year2 - 1 )	
			{
				weatherRecord temp = weatherRecord(year2);
				for (int j = 0 ; j < endDoy; ++j)
				{
					temp.airTemp[j] = grid_xn_to_mpmas2[gridId * xnGridSize + i].airTemp[j];
					temp.topsoilTemp[ j]  = grid_xn_to_mpmas2[gridId * xnGridSize + i].topsoilTemp[j];
				}
				temp.numDays = endDoy ;
				
				//remove last record ??
				xnGridWeatherHistory[i].pop_back();
				//add new record at beginning
				xnGridWeatherHistory[i].push_front(temp);				
			}
			else 
			{
				std::stringstream errmsg;
					errmsg << "Error in <updateWeatherHistory>:\n"
					<< "Gap in weather history: New year " << year2 << ", last existing year " << xnGridWeatherHistory[i][0].year <<"\n";
					throw runtime_error(errmsg.str());
			}		
		}
	}
	lastWeatherUpDate = curDate;
}


/*
void xn_mpmas_translator::updateWeatherHistory(const STRUCT_xn_to_mpmas2* grid_xn_to_mpmas2, int thisSeasonsStartyear, xnmpmasDate curDate)
{   //copy information on cell-specific weather data from XN/MPMAS shared array into weatherHistory
	
	int startDoy1 = getStartDoy(thisSeasonsStartyear);
	int startDoy2 = getStartDoy(thisSeasonsStartyear + 1);

	int daysYear1 = isLeapYear(thisSeasonsStartyear) ? 366 : 365;
	//int daysYear2 = isLeapYear(thisSeasonsStartyear + 1) ? 366 : 365;
	
	//int curDoy = convertDateToDayOfYear(curDate);
	
	for (int i = 0; i < xnGridSize; ++i)
	{	
		int lua = currentXnCropGrid.at(i);//TODO: switch correctly !!!
		int gridId = LuaXnParameters.at(lua).currentGrid;
		
		//copy everything before Jan 1st to existing record
		for (int j = startDoy1; j < daysYear1; ++j)
		{	// -1 to convert from doy (>=1) to array position (>=0)
			xnGridWeatherHistory[i][0].airTemp[j] = grid_xn_to_mpmas2[gridId * xnGridSize + i].airTemp[j];
			xnGridWeatherHistory[i][0].topsoilTemp[j] = grid_xn_to_mpmas2[gridId * xnGridSize + i].topsoilTemp[j];
		}
		xnGridWeatherHistory[i][0].numDays = daysYear1;
		
		//create and fill temporary array to be added for second year
		weatherRecord temp = weatherRecord(thisSeasonsStartyear + 1);
		for (int j = 0; j < startDoy2 + 1; ++j)
		{
			temp.airTemp[j] = grid_xn_to_mpmas2[gridId * xnGridSize + i].airTemp[j];
			temp.topsoilTemp[ j]  = grid_xn_to_mpmas2[gridId * xnGridSize + i].topsoilTemp[j];
		}
		temp.numDays = startDoy2 ;
		
		//remove last record
		xnGridWeatherHistory[i].pop_back();
		//add new record at beginning
		xnGridWeatherHistory[i].push_front(temp);

	}
}

*/




#define tempNumIncorrect 1
/*void xn_mpmas_translator::updateWeatherHistory(STRUCT_xn_to_mpmas2* grid_xn_to_mpmas2, int currentyear, xnmpmasDate curDate)
{   //copy information on cell-specific weather data from XN/MPMAS shared array into weatherHistory
	//Note: shared array starts at start of season (startDate), while weather records start at 1st of January
	//The incoming information thus may need to be split over two history records.The first [index = 0] element of the deque 
	//must be filled with everything from the startDate of the record to before 1st January, while everything after 1st of January must go into a
	//new record. The new record is finally included in front of weatherHistory and the oldest record of weather history is discarded.
	//(This of course has to be done last, otherwise, our index = 0 would point to the wrong record.)
	//Note: currentyear is/must be the calendar year in which the seasons started for which we record the record
	
	//xnmpmasDate thisStartDate = startDate;
	//thisStartDate.year = currentyear;
	//int startDoy = convertDateToDayOfYear(thisStartDate);
	//flag while numDaysAirTemp is incorrect
	
	int curDoy = convertDateToDayOfYear(curDate);
	
	for (int i = 0; i < xnGridSize; ++i)
	{	
		if (grid_xn_to_mpmas2[i].numDaysAirTemp != grid_xn_to_mpmas2[i].numDaystopSoilTemp)
		{	fprintf(stderr, "Error in <xn_mpmas_translator::updateWeatherHistory>: grid_xn_to_mpmas2[%d].numDaysAirTemp != grid_xn_to_mpmas2[%d].numDaystopSoilTemp. [Cell: %d, %d]\n", i,i, i/ xnGridYdim, i % xnGridYdim);
			mpmasScope::exitfun();
		}
		if (grid_xn_to_mpmas2[i].numDaysAirTemp == 0) //skip cell if no information available (if cell is inactive)
			continue;	
		
		printf("- updating weather history: Cell %d\n", i);
			
		int dayDiff = curDoy - grid_xn_to_mpmas2[i].numDaysAirTemp  - tempNumIncorrect;
		
		
		if (dayDiff > 0) 
		{ //no split, everything in the same year, and this year should already exist in the database as the first record
			if (xnGridWeatherHistory[i][0].year != curDate.year) 
			{	fprintf(stderr, "Error in <xn_mpmas_translator::updateWeatherHistory>: inconsistent weather records\n"
								"Last year on record: %d\n"
								"Current date: %d-%d-%d (%d)\n"
								"Length incoming information: %d\n", xnGridWeatherHistory[i][0].year,
								curDate.day, curDate.month, curDate.year, curDoy, grid_xn_to_mpmas2[i].numDaysAirTemp + tempNumIncorrect
						);
				mpmasScope::exitfun();
			}
			int startDoy = dayDiff + 1;
			for (int j = 0; j < grid_xn_to_mpmas2[i].numDaysAirTemp + tempNumIncorrect ; ++j)
			{	// -1 to convert from doy (>=1) to array position (>=0)
				xnGridWeatherHistory[i][0].airTemp[startDoy + j - 1] = grid_xn_to_mpmas2[i].airTemp[j];
				xnGridWeatherHistory[i][0].topsoilTemp[startDoy + j -1 ] = grid_xn_to_mpmas2[i].topsoilTemp[j];
			}
			xnGridWeatherHistory[i][0].numDays = curDoy;			
		}
		else if (dayDiff == 0) //first of January is first day in received record
		{ //no split, everything in the same year, but the new record still needs to be created
			//create and fill temporary array to be added 
			weatherRecord temp = weatherRecord(curDate.year);
			for (int j = 0; j < grid_xn_to_mpmas2[i].numDaysAirTemp + tempNumIncorrect ; ++j)
			{
				temp.airTemp[j] = grid_xn_to_mpmas2[i].airTemp[j];
				temp.topsoilTemp[j] = grid_xn_to_mpmas2[i].topsoilTemp[j];
			}
			temp.numDays = grid_xn_to_mpmas2[i].numDaysAirTemp + tempNumIncorrect ; 
			
			//remove last record
			xnGridWeatherHistory[i].pop_back();
			//add new record at beginning
			xnGridWeatherHistory[i].push_front(temp);
		}
		else if (dayDiff < 0)
		{ //split into two years, new record must be created for second year
		  	int daysYear1 = 365 + isLeapYear(curDate.year -1);	
			
			//put first days to old record
			if (xnGridWeatherHistory[i][0].year != curDate.year -1 ) 
			{	fprintf(stderr, "Error in <xn_mpmas_translator::updateWeatherHistory>: inconsistent weather records\n"
								"Last year on record: %d\n"
								"Current date: %d-%d-%d (%d)\n"
								"Length incoming information: %d\n", xnGridWeatherHistory[i][0].year,
								curDate.day, curDate.month, curDate.year, curDoy, grid_xn_to_mpmas2[i].numDaysAirTemp + tempNumIncorrect
				);
				mpmasScope::exitfun();
			}
			int startDoy = daysYear1 + dayDiff + 1;
			for (int j = 0; j < -dayDiff ; ++j) 
			{	// -1 to convert from doy (>=1) to array position (>=0)
				xnGridWeatherHistory[i][0].airTemp[startDoy + j - 1] = grid_xn_to_mpmas2[i].airTemp[j];
				xnGridWeatherHistory[i][0].topsoilTemp[startDoy + j -1 ] = grid_xn_to_mpmas2[i].topsoilTemp[j];
			}
			xnGridWeatherHistory[i][0].numDays = daysYear1;

			//for second year
			//create and fill temporary array to be added 
			weatherRecord temp = weatherRecord(curDate.year);
			for (int j = -dayDiff; j < grid_xn_to_mpmas2[i].numDaysAirTemp + tempNumIncorrect ; ++j)
			{
				temp.airTemp[j + dayDiff] = grid_xn_to_mpmas2[i].airTemp[j];
				temp.topsoilTemp[j + dayDiff] = grid_xn_to_mpmas2[i].topsoilTemp[j];
			}
			temp.numDays = grid_xn_to_mpmas2[i].numDaysAirTemp + tempNumIncorrect ; 
			
			//remove last record
			xnGridWeatherHistory[i].pop_back();
			//add new record at beginning
			xnGridWeatherHistory[i].push_front(temp);
			
		}
*/
/*			
		//copy everything before Jan 1st to existing record
		for (int j = startLoop; j < daysToYear2; ++j)
		{	// -1 to convert from doy (>=1) to array position (>=0)
			xnGridWeatherHistory[i][0].airTemp[startDoy + j - 1] = grid_xn_to_mpmas2[i].airTemp[j];
			xnGridWeatherHistory[i][0].topsoilTemp[startDoy + j -1 ] = grid_xn_to_mpmas2[i].topsoilTemp[j];
		}
		xnGridWeatherHistory[i][0].numDays = daysYear1;
		
		//create and fill temporary array to be added for second year
		weatherRecord temp = weatherRecord();
		for (int j = daysToYear2; j < grid_xn_to_mpmas2[i].numDaysAirTemp; ++j)
		{
			temp.airTemp[j - daysToYear2] = grid_xn_to_mpmas2[i].airTemp[j];
			temp.topsoilTemp[ j -  daysToYear2] = grid_xn_to_mpmas2[i].topsoilTemp[j];
		}
		temp.numDays = grid_xn_to_mpmas2[i].numDaysAirTemp - daysToYear2; 
		
		//remove last record
		xnGridWeatherHistory[i].pop_back();
		//add new record at beginning
		xnGridWeatherHistory[i].push_front(temp);
		 * 
	}
	 * */
void xn_mpmas_translator::writeWeatherHistoryToFile(string filename, int currentyear)
{
	ofstream outfile(filename.c_str());
	//int ws = 1; //set to zero if years do not overlap

	outfile << "year\tdayofyear\tx\ty\tairtemp\tsoiltemp" << endl;
	for (unsigned i = 0;  i < xnGridWeatherHistory.size(); ++i)
	{	
		unsigned x = i / (unsigned) xnGridYdim; //integer division
		unsigned y = i % (unsigned) xnGridYdim;
		for (int j = (int) xnGridWeatherHistory[i].size() -1 ; j >= 0 ; --j)
		{
			int absYear =  xnGridWeatherHistory[i][j].year;//currentyear - j + ws;
			for (int k = 0; k < xnGridWeatherHistory[i][j].numDays; ++k)
			{
				int dayofyear = k + 1;
				outfile << absYear 
				<< '\t' << dayofyear 
				<< '\t' << x 
				<< '\t' << y
				<< '\t' << setprecision(4) << xnGridWeatherHistory[i][j].airTemp[k]
				<< '\t' << setprecision(4) << xnGridWeatherHistory[i][j].topsoilTemp[k]
				<< "\n";
			}
		}
	}
	outfile.close();
}
