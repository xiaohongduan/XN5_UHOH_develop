#ifndef XN_MPMAS_TRANSLATOR_H_
#define XN_MPMAS_TRANSLATOR_H_

#include <cstdio>
#include <string>
#include <map>
#include <vector>
#include <deque>
#include <sstream>

#include "xn_mpmas_interface.h"
#include "xn_mpmas_options.h"
#include "xn_mpmas_adaptation.h"

#include "Raster2D.h"

//Begin of Hong
#undef max     // double defined in expertn
#undef min
//End of Hong

using namespace std;

namespace xnmpmas
{ 	namespace coupling 
	{
		typedef struct {
			double yieldFactor;
			double stoverYieldFactor;
			double stemYieldFactor;
		} luaInfo;
	}
}

class xn_mpmas_translator 
{
private:
	xnmpmasCouplingTypes couplingType;	
		
	map<int, STRUCT_mpmas_to_xn> LuaXnParameters;
	map<int, xnmpmas::coupling::luaInfo> LuaCouplingParameters;
	map<int, xnmpmas::adaptation::cropAdaptationParameters> LuaAdaptationParameters;
	
	vector<multimap< int, int > > CellForLua; //map: key lua, value cell, vector index: period for which map holds
	multimap<int, int> CurrentCellsForLua;
	multimap<int, int> PreviousCellsForLua;
	map<string,int> GridForCropCode;
	vector<int> currentXnCropGrid;
	vector<int> previousXnCropGrid;
	
	int xnGridYdim;
	int xnGridZdim;
	int xnGridXdim;
	vector<deque<xnmpmas::adaptation::weatherRecord> > xnGridWeatherHistory;
	vector<double> historyWeighting;
	
	int xnGridSize;
	int mpmasArraySize;
	int* cropActIdsArray;
	double* cropAreasArray;

	xnmpmasDate startDate;

protected:
	//initialization
	int readLuaProps(const char* fnLuaProps );
	int readLuaCellAssociation(const char* fnCells );
	int readCropGridAssociation(const char* fnCropGrids );
	void checkFscanfSuccess(int rtcod, int toBeRead, const char* fn, char* messageOnError);
	void swapAndCleanGrids();
public:
	//constructor/initialization
	xn_mpmas_translator(xnmpmasCouplingTypes type_, const char* fnLuaProps_, const char* fnCropGrids_, const char* fnCells_, xnmpmasDate startDate_);

	void setXnGridDimensions (int xdim, int ydim, int zdim);	
	void readAdaptationParametersAndWeatherHistory(	std::string fnParams, 
													std::string fnHistory, 
													int cellSpecificWeather,
													std::string fnMap);

	//getters
	xnmpmasCouplingTypes getCouplingType()	{		return couplingType;	};
	
	int getStartDoy(int currentyear);
	
	
	//translation
	void defineLuaforCellsFromArray(int year, int mpmasArraySize_,  int* cropActIds_,  double* cropAreas_);
	void defineLuaForCellsFromMap(const Raster2D* luaMap);

	STRUCT_mpmas_to_xn getManagementForCell(int cell, int mpmasYear);
	
	void calcYieldsToMaps(const STRUCT_xn_to_mpmas* grid_xn_to_mpmas, Raster2D* yieldMap1, Raster2D* yieldMap2, vector<Raster2D>& cropExtraAttrRasters, int overlapping, string fnAggXnOutput);
	void calcYieldsToArray(const STRUCT_xn_to_mpmas* grid_xn_to_mpmas,  double* yieldArray, double* stoverYieldArray, double** extraAttrsX, int numExtra, const int overlapping);
	void updateWeatherHistory(const STRUCT_xn_to_mpmas2* grid_xn_to_mpmas2,  int currentyear,  xnmpmasDate curDate);
	void clearArrayRefs();
	
	//screen and file output
	void writeWeatherHistoryToFile(string filename, int currentyear);
};



#endif
