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
//  File:       AgentHouseholdMember.h (formerly hhmember.h)
//
//  Contents:   Household member class, containing individual information of
//              each household member, e.g. age, sex.
//
//
//------------------------------------------------------------------------------


#ifndef _AgentHouseholdMember_h
#define _AgentHouseholdMember_h

#include <stdio.h>
//#include <stdlib.h>
#include <vector>
#include <list>
#include "BasicDefines.h"
#include "BasicEnums.h"
using namespace std;

// Troost 20160121 Household member-specific random streams
// Troost **********begin block insert**********
#ifdef RNGSTREAM
	#include "RngStream.h"
	#include <deque>
	#include <bitset>
	#include <iostream>
	#include <set>

	typedef std::bitset<24> memberBitCode;

	struct memberBitCodeCompare {
		bool operator()(const memberBitCode& x, const memberBitCode& y)
		{
			 for (int i = 24-1; i >= 0; i--) {
				  if (x[i] ^ y[i]) return y[i];
			 }
			 return false;
		}
	};
	//This class holds preinitialised random number streams for each household member

struct hhMemberRandCollection
{		unsigned long nextSeed[6];
		memberBitCode personIdCode;
		std::bitset<24> householdCode;
		//yearly career events:
		std::deque<double> randDie;
		std::deque<double> randLeave;
		std::deque<double> randBirth;
		std::deque<double> randMarry;
		std::deque<double> randSwitchCareer;

		//yearly check, but only one value
		double randRetire;

		//individual career events, stock of values
		std::deque<double>  randChildCareer;
		std::deque<double>  randMarryCareer;
		std::deque<double>  randPartnerCareer;

		//individual career events, constant draw
		double randHeadCareer;
		double randPartnerHeadCareer;
		double randRetireCareer;
		double randAgeAtInit;
		double randLeavingWithParent;

		double getNextRandDeath();
		double getNextRandLeave();
		double getNextRandBirth();
		double getNextRandMarry();
		double getNextRandSwitchCareer();
		double getRandRetire() {return randRetire; }
		double getRandLeavingWithParent() {return randLeavingWithParent; }


		double getNextRandChildCareer();
		double getNextRandMarryCareer();
		double getNextRandPartnerCareer();
		double getRandHeadCareer() 		{ return randRetireCareer; }
		double getRandPartnerHeadCareer(){ return randPartnerHeadCareer; }
		double getRandRetirementCareer() { return randHeadCareer; }

		double getRandAgeAtInit() { return randAgeAtInit; }


		double peekNextRandDeath(int ahead);
		double peekNextRandLeave(int ahead);
		double peekNextRandBirth(int ahead);
		double peekNextRandMarry(int ahead);
		double peekNextRandSwitchCareer(int ahead);

		double peekNextRandChildCareer();
		double peekNextRandMarryCareer();
		double peekNextRandPartnerCareer();


		void populate(RngStream* rng, int years);
		hhMemberRandCollection(memberBitCode code, RngStream* rng, int years) : personIdCode(code) { populate(rng, years);}

		void writeNextSeedToStream(FILE* stream);
		void writeToStream(int hhID, int pID, std::ostream& out);
};
#endif //RNGSTREAM
// Troost **********end block insert**********

class hhmember
{  protected:
   int hhID;//Household ID (i.e. Farmstead ID)
   //int scID;//Watershed ID (i.e. "SubcatchmentNum")
   
	int pID; //personal ID



// Troost 20160121 Household member-specific random streams
// Troost **********begin block insert**********
#ifdef RNGSTREAM
	memberBitCode descendanceCode;
	int numDirectDescendants;
	int numPartners;
	hhMemberRandCollection randomNumbers;
#endif //RNGSTREAM
	// Troost **********end block insert**********
   int age;
   int sex; //female = 0, male = 1
   int head;
   int career;//occupational career

   //double* actLabCap;//'actual' labor capacity, i.e. for current year
   //double* expLabCap;//'expected' labor capacity, i.e. for average future year

   //int** health; //health status //Troost 160119 commented because unused

// Troost 20121208 - Advanced demography model
// Troost **********begin block insert**********
	hhmember* partner;
	int marriedOrChildAtInitialization;
// Troost **********end block insert**********

// Troost 20140605 Mark household head for young farmer subsidy
// Troost **********begin block insert**********
	double userMark;
// Troost **********end block insert**********

// Troost 20170516 Advanced Demography Model 2
// Troost **********begin block insert**********
	list<householdMemberSwitchFromLp> scheduledSwitches;
#ifdef RNGSTREAM
	set<memberBitCode,memberBitCodeCompare> children;
#endif //RNGSTREAM
// Troost **********end block insert**********

	public:
	//hhmember(int hID, int scID, int pID, int age, int sex, int head, int car);//## Tbe 110428
#ifdef RNGSTREAM
	hhmember(int hID, int pID, int minAge, int maxAge, int sex, int head, int car, memberBitCode descendanceCode, RngStream* rng);
#else
	hhmember(int hID, int pID, int age, int sex, int head, int car);
#endif //RNGSTREAM
	virtual ~hhmember()
	{//	printf("\nInfo: HHmember deallokiert\n");
	}

	//Pointers for list of household members
   hhmember* nextHHmember;
   void setNextPtr_M (hhmember*);
   hhmember* getNext_M();

	hhmember* prevHHmember;
   void setPrevPtr_M (hhmember*);
   hhmember* getPrev_M();


	//Elementfunctions
   virtual int getHhID();
   virtual int getPID();
	virtual int getAge();
   virtual void increaseAge();
	virtual int getSex();
	virtual int getHead();
   virtual void setHead(int);
   virtual int getCareer();
   virtual void setCareer(int);


   void changeIDs(int newHhId, int newPID) { hhID = newHhId; pID =  newPID; }

   void addScheduledDecisionFromLP(householdMemberSwitchFromLp what ) { scheduledSwitches.push_back(what);}
   void clearScheduledDecisionsFromLP() { scheduledSwitches.clear();}
   bool checkAndPopScheduledDecisionFromLP(householdMemberSwitchFromLp what ) ;
   int checkAndPopScheduledCareerSwitchFromLP();


#ifdef RNGSTREAM
   memberBitCode getDescendanceCode() {return descendanceCode;}
   void setDescendanceCode(memberBitCode code) {descendanceCode = code ; }
   int getNumDescendants() {	return numDirectDescendants;}
   int getNumPartners() {	return numPartners;}
   void increaseNumDescendants(int i = 1) {numDirectDescendants += i;}
   void increaseNumPartners(int i = 1) {numPartners += i;}

   void addChild(memberBitCode code);
   const set<memberBitCode,memberBitCodeCompare> getChildren();

   //void drawNewYearlyRands();

	double getRandDeath() {return randomNumbers.getNextRandDeath();}
	double getRandLeave() {return randomNumbers.getNextRandLeave();}
	double getRandBirth() {return randomNumbers.getNextRandBirth();}
	double getRandMarriage() {return randomNumbers.getNextRandMarry();}
	double getRandRetire() {return randomNumbers.getRandRetire();}
	double getRandSwitch() {return randomNumbers.getNextRandSwitchCareer();}
	double getRandLeaveWithParent() {return randomNumbers.getRandLeavingWithParent();}


	double getRandChildCareer() {return randomNumbers.getNextRandChildCareer();}
	double getRandMarriageCareer() {return randomNumbers.getNextRandMarryCareer();}
	double getRandPartnerCareer() {return randomNumbers.getNextRandPartnerCareer();}
	double getRandHHeadCareer() {return randomNumbers.getRandHeadCareer();}
	double getRandPartnerHHeadCareer() {return randomNumbers.getRandPartnerHeadCareer();}
	double getRandRetireCareer() {return randomNumbers.getRandRetirementCareer();}

	double peekRandDeath(int ahead) {return randomNumbers.peekNextRandDeath(ahead);}
	double peekRandLeave(int ahead) {return randomNumbers.peekNextRandLeave(ahead);}
	double peekRandBirth(int ahead) {return randomNumbers.peekNextRandBirth(ahead);}
	double peekRandMarriage(int ahead) {return randomNumbers.peekNextRandMarry(ahead);}
	double peekRandSwitch(int ahead) {return randomNumbers.peekNextRandMarry(ahead);}
	double peekRandChildCareer() {return randomNumbers.peekNextRandChildCareer();}
	double peekRandMarriageCareer() {return randomNumbers.peekNextRandMarryCareer();}
	double peekRandPartnerCareer() {return randomNumbers.peekNextRandPartnerCareer();}


#endif

// Troost 20121208 - Advanced demography model
// Troost **********begin block insert**********
	virtual hhmember* getPartner();
	virtual void setPartner(hhmember*);
	virtual bool hasPartner();

	virtual int statusMarriedOrChildAtInitialization();
	virtual void setStatusMarriedOrChildAtInitialization(int);
// Troost **********end block insert**********

// Troost 20140605 Mark household head for young farmer subsidy
// Troost **********begin block insert**********
	virtual void set_userMark(double);
	virtual double get_userMark();
// Troost **********end block insert**********

   virtual void printToScreen();
	virtual void writeToStream(FILE* stream);
#ifdef RNGSTREAM
	void writeRandomNumbersToStream(std::ostream& stream) {randomNumbers.writeToStream(hhID, pID, stream);}
#endif
};


#endif


