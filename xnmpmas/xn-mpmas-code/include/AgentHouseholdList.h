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
//  File:       AgentHouseholdList.h (formerly hhlist.h)
//
//  Contents:   Manages linked list of all household members of one agent
//
//
//
//------------------------------------------------------------------------------


#ifndef _AgentHouseholdList_h
#define _AgentHouseholdList_h


#include "AgentHouseholdMember.h"
#include "MatrixDouble.h"
#include <map>

// Troost 20121208 - Advanced demography model
// Troost **********begin block insert**********
//forward declaration here
class agentF;
// Troost **********end block insert**********

// Troost 20160121 Household member-specific random streams
// Troost **********begin block insert**********
#ifdef RNGSTREAM
#include "RngStream.h"
namespace mpmasGlobal
{
	inline int getRngSubstreamExponent_HouseholdBase()
	{	return 24;} // 2^24 * 2^76 * fstID is the place in the random stream where the random number stream for a household starts

	inline int getRngSubstreamExponent_GenerationBase()
	{
		return 0; //2^24 * 2^76 * fstID + 2^76 * descendanceCode  is where the random number stream of the members of a household starts
/*		if (g < 0 || g > 3)
		return -1;
		if (g == 0)
		return 89; //2^94 * fstID + 2^89 * mID  is where the random number stream of the max. 32 initial members of a household starts
		else
		return 77 + (3 -g ) * 4;//for new members the substream is determined*/
	}
}
#endif
// Troost **********end block insert**********
class hh_list
{	protected:
   //int scID;	//SubcatchmentNum
	int hhID;// corresponds to Farmstead ID

   int hhSize;
   int nAdults;
   int nChilds;
   int nSchool;
   int lastPID;

   int lSpan;
   int lGrps;

   MatrixDouble actLabCapHH;
   MatrixDouble expLabCapHH;
   MatrixDouble actNtrReqHH;
   MatrixDouble expNtrReqHH;

#ifdef MULTIPERIOD
   vector<vector<double> > multiperiodLabCapHH; //dim1 time, dim2 labgrp
   vector<vector<double> > multiperiodUserDefHH;
   vector<double> multiperiodMinConsHH;
#endif //MULTIPERIOD
   hhmember* firstHHmember;
   hhmember* lastHHmember;
   hhmember* markedHHmember;

// Troost 20121208 - Advanced demography model
// Troost **********begin block insert**********
   //pointer to household head
	hhmember* HHead;
	//current minimum consumption of household
	double minConsHH;
	int expectedRemainingLifetimeHH;
// Troost **********end block insert**********

// Troost 20160121 Household member-specific random streams
// Troost **********begin block insert**********
#ifdef RNGSTREAM
	unsigned long householdStartSeed[6];
#endif
// Troost **********end block insert**********

   public:
   //hh_list(int, int);
	hh_list(int);
   virtual ~hh_list()
   {  deleteList();
   }
#ifdef RNGSTREAM
	virtual hhmember* makeHHmember(int minAge, int maxAge, int sex, int head, int career, memberBitCode code);
	virtual hhmember* makeHHmemberAtInitialization(int minAge, int maxAge, int sex, int head, int career);
	virtual hhmember* makeHHmemberAtInitialization(int minAge, int maxAge, int sex, int head, int c, int marriedOrChild);
	virtual hhmember* makeHHmemberJoiningAsPartnerOf(int age, int sex, int head, int career, hhmember* partner, int popID);
	virtual hhmember* makeHHmemberJoiningAsChildOf(int age, int sex, int head, int career, hhmember* parent, int popID);
#else
	virtual hhmember* makeHHmember(int age, int sex, int head, int career);
	virtual hhmember* makeHHmember(int age, int sex, int head, int c, int probablyMarried);
#endif //RNGSTREAM
// Troost 20121208 - Advanced demography model
// Troost **********begin block insert**********


	virtual void matchMarried();
#ifdef RNGSTREAM
	virtual void matchMarriedAndChildren();
#endif
	virtual bool checkHHsize();

	virtual hhmember* identifyNewHHead(int popID, int flag);
	virtual hhmember* identifyNewHHeadExpectedInFuture(int popID, int flag);
	virtual int checkForNewHHeadExpectedInFuture(int popID, int flag);
	virtual void setHHead(hhmember*);
	virtual hhmember* getHHead();
	virtual int getHHeadAge();

	virtual void makeCareerSwitchesHouseholdHead(hhmember* newHHead, int popID);
	virtual void takeOutOtherMembersLeavingWithMember(hhmember* hhp, int popID, int formNewFarmHousehold, vector<hhmember*> & leavingMembers, hhmember*& nhp, int& hheadGone, FILE* outStream);
	virtual std::map<int, std::vector<hhmember*> > identifyHouseholdMembersLeavingForNewHouseholdsFromLP(agentF* betr, FILE* outStream);
// Troost **********end block insert**********
// Troost 20140505 Age HHead in LP
// Troost **********begin block insert**********
	virtual int getPotentialSuccessorAge(int popID);
// Troost **********end block insert**********

   virtual void makeHeadOfHH(int popID, int sex, int mID); //not for advanced demography model
	virtual void deallocHHmember(hhmember*);
	virtual void insertHHmember(hhmember*);

   virtual int getNumLabSpan();
   virtual int getNumLabGrps();

   virtual void makeActLabCapHH(int popID);
   virtual void makeExpLabCapHH(int popID);
   virtual double getActLabAvail(int span, int lGrp);
   virtual double getExpLabAvail(int span, int lGrp);

#ifdef MULTIPERIOD
   vector<vector<double> >* getMultiperiodLabCapHH();
   vector<vector<double> >* getMultiperiodUserDefinedAttributes();
   vector<double>* getMultiperiodMinConsHH();
   void makeMultiperiodHouseholdMatrices(int popID, int numPeriods);
#endif

   virtual double getActTotLabForce(int popID); //total available labor force
   virtual double getActAduLabForce(int popID); //only adult labor force
   virtual double getActAduFemForce(int popID); //only adult female labor
   virtual double getActAduMalForce(int popID); //only adult male labor
   virtual double getExpTotLabForce(int popID); //total available labor force
   virtual double getExpAduLabForce(int popID); //only adult labor force
   virtual double getExpAduFemForce(int popID); //only adult female labor
   virtual double getExpAduMalForce(int popID); //only adult male labor

   virtual void makeActNtrReqHH(int popID);
   virtual void makeExpNtrReqHH(int popID);
   virtual double getActNtrReqs(int ntr);
   virtual double getExpNtrReqs(int ntr);

// Troost 20121208 - Advanced demography model
// Troost **********begin block insert**********
	virtual void makeActMinConsHH(int popID);
	virtual double getActMinConsHH();
	virtual void makeExpectedRemainingLifetimeOfHoushold (int popID);
	virtual int getExpectedRemainingLifetimeOfHoushold();
// Troost **********end block insert**********

// Troost 20140630 Voluntary exit decision in Investment LP
// Troost **********begin block insert**********
	virtual double calculateOppWageOfHouseholdMembers(int popID);
// Troost **********end block insert**********


	virtual void set_userMark_HouseholdHead(double value);
	virtual double get_userMark_HouseholdHead();
	virtual void set_userMark_PotentialSuccessor(int popID, double value);
	virtual double get_userMark_PotentialSuccessor(int popID);

	virtual double get_userDefinedCharacteristic(int popID, int attribute2);
#ifdef RNGSTREAM
	double get_householdMemberInfoForLP(int popID, int career, int ageMin, int ageMax, int which, int what, int ahead, int whoelse);
#endif
	double get_memberInfoFromCareer(int popID, int what, hhmember* hhp, int ahead);
 	virtual int getNumMembers(void);
   virtual int getNumAdults(void);
   virtual int getNumChilds(void);
   virtual int getNumSchoolAge(void);
   virtual int getNumSexAgeGroup(int sex, int ageLow, int ageUpp);
   virtual int getLastID(void);

   virtual hhmember* searchForHHmember(int pID);

	virtual hhmember* searchFirstSex(int);//Parameter: sex
   virtual hhmember* searchNextSex(int); //Parameter: sex
	virtual hhmember* searchLastSex(int); //Parameter: sex
   virtual hhmember* searchPreviousSex(int);//Parameter: sex
   virtual hhmember* getFirst();//return first in list
   virtual hhmember* getLast();//return last in list

// Troost 20121208 - Advanced demography model
// Troost **********begin block changed**********
//	virtual void updateAgeInHH(int popID);
	virtual int updateAgeInHH(int popID, agentF* betr, FILE* outStream);
// Troost **********end block changed**********

// Troost 20170516 Advanced Demography Model 2
// Troost **********begin block insert**********
	void setHouseholdMemberDecisionsFromLP(agentF* betr);
#ifdef RNGSTREAM
	bool peekStochasticDecisionLeave(hhmember* hhp, int popID, int ahead);
	bool peekStochasticDecisionMarriage(hhmember* hhp, int popID, int ahead);
	bool peekStochasticDecisionRetire(hhmember* hhp, int popID, int ahead);
	int peekStochasticDecisionSwitch(hhmember* hhp, int popID, int ahead);
	int peekStochasticDecisionMarriageCareer(hhmember* hhp, int popID);
	int peekStochasticDecisionPartnerCareer(hhmember* hhp, int popID);
	bool peekStochasticDecisionLeaveWithMother(hhmember* hhp, int popID,  int ahead);
	bool peekStochasticDecisionLeaveWithFather(hhmember* hhp, int popID,  int ahead);

	virtual hhmember* getHouseholdMemberByMemberBitCode(memberBitCode code);
#endif //RNGSTREAM
// Troost **********end block insert**********


// Troost 20121208 - Advanced demography model
// Troost **********begin block insert**********
	virtual void allocateStatistics(int popID, int mID);
// Troost **********end block insert**********
   virtual void updateStatistics(int popID, int mID);
	virtual hhmember* takeOutHHmember(hhmember* hhp);

	virtual void printOnScreenHHmember(hhmember* hhp);
   virtual void printOnScreen_list(void);

	virtual void writeHHmemberToStream(FILE* stream, hhmember* hhp);
	virtual void writeListOfHHmembersToStream(FILE* stream);
#ifdef RNGSTREAM
	void writeRandomNumbersOfHHmembersToStream(ostream& stream);
#endif
   virtual void deleteList(void);

#ifdef RNGSTREAM


   void updateLastPID();
   void includeNewHouseholdMembers(vector<hhmember*>& memberList, int popID, int marketID);
 private:
   memberBitCode  calcDescendanceCodePartner(const memberBitCode oldMemberCode, int memberPartnerID );
   memberBitCode  calcDescendanceCodeDescendant(const memberBitCode parentCode, int memberDescID );
   void calculateHouseholdStartSeed();
   RngStream*  forwardRngStreamToHouseholdMemberStart(const memberBitCode code);



#endif
};

#endif //  _AgentHouseholdList_h
