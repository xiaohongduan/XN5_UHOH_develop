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
//  File:       TimeHandlerClass.h
//
//  Contents:   Header of class used for handling of time
//              
//
//
//------------------------------------------------------------------------------


// DOCUMENTAITON AND TEST PROJECT:
// ... utilities\Win32\bcb5\TestTimeHandlerBorland.bpr
// ... utilities\src\master\testTimeHandler.cpp


#ifndef _TimeHandlerClass_
#define _TimeHandlerClass_

	#include "BasicDefines.h"
#ifdef ODB
	#include <string>
	#include <vector>

	#include "BasicEnums.h"
	#include "BasicStreams.h"
#else
	#include "BasicData.h"
#endif


#ifdef ODBSTORE
	#include <odb/core.hxx>
#endif

string integerToString(int val);
string int2String(int val);

#pragma db object
class TimeHandler
{
public:
	/// Fixed times
	int STARTYEAR()             { return absoluteStartYear;}    
	int STARTMONTH()            { return absoluteStartMonth;}
	int STARTDAY()              { return absoluteStartDay;}        
	 
	int STOPYEAR()              { return absoluteStopYear;}        
	int STOPMONTH()             { return absoluteStopMonth;}
	int STOPDAY()               { return absoluteStopDay;}

	int IRRIGATIONSTARTMONTH()  {return irrigationStartMonth;}
	int IRRIGATIONSTOPMONTH()   {return irrigationStopMonth;}
	
	bool isIrrigationMonth();  // assumes This Month
	bool isIrrigationMonth(int m); // takes any month <m> as argument   
	 
	/// Access to total model times
	// old, erase later
	virtual int MONATE();
	virtual int TOTAL_IRRIGATION_MONTHS();
	virtual int TOTALPERIODS();   
   virtual bool ISNORTHERN();
   int getNumPeriodsPerYear();

	void setMONATE(int m_);
	 
	/// Access to current time (real-time counting)
	int GETMONTH()              { return currentAbsoluteMonth;}
	int GETYEAR()               { return currentAbsoluteYear;}

	string GETMONTH_string()    { return integerToString(currentAbsoluteMonth); }
	string GETYEAR_string()     { return integerToString(currentAbsoluteYear);   }

	string getDateAsString();	 
	 
	 
	/// Access to current time (internal model counting)

#ifdef MULTIPERIOD
	int PERIODE()               { return currentPeriodCount;} // old, erase later
	int CURRENT_CYCLIC_PERIOD() { return currentCyclicPeriod;}
	int CURRENTMODELPERIOD()    { return currentPeriodCount;}
	int CURRENTMODELYEAR()    { return currentRelativeYear;}
#else
	int PERIODE()               { return currentRelativeYear;} // old, erase later
	int CURRENTMODELPERIOD()    { return currentRelativeYear;}
#endif
	int CURRENTMODELMONTH()     { return currentRelativeMonth;}


   /// Read and write
   void initializeDummy();
	void readTimeHandler(string filename);
	void readFromFile(string filename) { readTimeHandler(filename);}

	void writeToFile(string fn);
	void writeAsRead(ofstream& outstream);

   /// Set and reset time
   
   // CAREFUL with these set functions
   virtual void setSTOPYEAR(int absoluteStopYear_);

   // ... set back to start
	void resetAllDatesToStart();
	void reset() {resetAllDatesToStart();}

   // ... Reset to beginning of (model) year.
   // ... In southern hemisphere, that is July!
   virtual void resetToStartOfYear(int _year);
   virtual void resetToStartOfYear(int _year, string comment);

	/// Next time step
	void nextMonth();
	void nextPeriod();

	/// Output functions
	void printCurrentTimeStatus(std::ostream *outstream);
	void printToScreen();
	void printDaysPerMonth();

	/// Information on length of years, etc   
	bool isLeapYear(int y_year);
	int  getDaysPerMonth(int m_month, int y_year);
	int  DAYSperTHISMONTH();

	/// Constructor, destructor
	TimeHandler();
   TimeHandler(  TimeHandler& sourceHandle, string nm);
	virtual ~TimeHandler();
#ifndef ODB
   virtual void deallocate();
#endif
   // Copy constructor
   virtual void copy( TimeHandler&  sourceHandle);

   //
   virtual void setNameOfHandler(string nm_);
   virtual string getNameOfHandler(); 

   // Should be private
  	virtual void SETMONTH(int m_);
   virtual void SETYEAR(int y_);
 
private:
#ifdef ODBSTORE
	friend class odb::access;
#endif
	/// Access to current time (real-time counting)
   virtual void setSTARTYEAR(int absoluteStartYear_);
   virtual void setSTARTMONTH(int absoluteStartMonth_);
   virtual 	void setSTOPMONTH(int absoluteStopMonth_);
   virtual void setISNORTHERN(bool isNorthern_);
   



	void updateIrrigationMonths();
	void updateIrrigationMonths(bool doOutput);

	// Ongoing date within model run, in real years ( 1-1996)
   // 1-1996  is
   // ... 0-0 in north if model starts in Jan 1996
   // ... 6-0 in south if model starts in July 1995
	int currentAbsoluteMonth;
	int currentAbsoluteYear;

	// Ongoing date within model run, in real years ( 1-1996)
	int currentRelativeYear;
	int currentRelativeMonth;
	
	// Absolute, fixed times
	int absoluteStartYear;
	int absoluteStartMonth;
	int absoluteStartDay;
	
	int absoluteStopYear;
	int absoluteStopMonth;
	int absoluteStopDay;    
	
	int irrigationStartMonth;
	int irrigationStopMonth;
	
	int maximumNumberOfIrrigationMonths; // MONATE();
	int maximumNumberOfPeriodsModeled;

	bool isNorthernHemisphere;

	int periodsPerYear;
	int currentPeriodCount;
	int currentCyclicPeriod;


	/// Determining number of days in month
#ifdef ODB
	vector<int>daysPerMonth;
	vector<string> monthString;
#else
	int* daysPerMonth; //= {  0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	string* monthString;
#endif // ODB
	void initDaysPerMonths();

   // Each time handler has a name, which is set when
   //   a) read from file
   //   b) created with copy constructor
   string nameOfThisHandler;

#pragma db id
#ifdef ODB
	unsigned long timehandlerID;
	static int maxTimehandlerID;
#endif

	bool checkIfCountUpYear();
	void addYearInTimeHandler() {	currentAbsoluteYear++;}
};
#endif

