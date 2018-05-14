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
//  File:       AgentLivestockList.h (formerly lslist.h)
//
//  Contents:   Manages linked list of all livestock units held by one agent
//
//
//
//------------------------------------------------------------------------------

#ifndef _AgentLivestockList_h
#define _AgentLivestockList_h

#include "AgentLivestockUnit.h"
#include "FinancialMathFunctions.h"

//151123 ct: added to avoid MatrixDouble use for livestock counts
#include <vector>
struct livestockPack {
		int type;
		int age;
		int number;

		livestockPack(int _t, int _a, int _n): type(_t), age(_a) , number(_n) {};

		void printToScreen() {
			printf("%d\t%d\t%d\n", type, age, number);
		}
};


class ls_list
{
protected:
	int scID;	//SubcatchmentNum
	int hhID;	//Betriebsnummer

	int hSize;  //herd size
	int lastPID;

	livestck* firstLivestck;
	livestck* lastLivestck;
	livestck* markedLivestck;

public:
	ls_list(int, int);
	virtual ~ls_list()
	{
		deleteList();
	}

	virtual void makeLivestck(int lsID, int age);
	virtual void deallocLivestck(livestck*);
	virtual void insertLivestck(livestck*);
	virtual int getNumMembers(void);
	virtual int getNumLs(int lsID);
	virtual int getNumLsAgeGroup(int lsID, int ageLow, int ageUpp);
	virtual void checkLsInventory(int oid, MatrixDouble& ageLs);
	virtual void makeLsEntriesRHS(MatrixDouble& lsTyp, MatrixDouble& lsRHS);
	virtual void makeActLPcoeff(int cshSpans, int labSpans,
			int, MatrixDouble&, MatrixDouble&, MatrixDouble&, MatrixDouble&, MatrixDouble&, MatrixDouble&);
	virtual void makeExpLPcoeff(int cshSpans, int labSpans,
			int, double, MatrixDouble&, MatrixDouble&, MatrixDouble&, MatrixDouble&, MatrixDouble&, MatrixDouble&, MatrixDouble&);
	virtual void makeInvLPcoeff(int, double,
                        MatrixDouble&, MatrixDouble&, MatrixDouble&, MatrixDouble&, MatrixDouble&, MatrixDouble&);
	virtual int getLastID(void);

	virtual livestck* searchForLivestck(int pID);
	virtual livestck* searchFirstType(int lsID);//Parameter: type of livestock
	virtual livestck* searchNextType(int lsID);
	virtual livestck* searchLastType(int lsID);
	virtual livestck* searchPreviousType(int lsID);
	virtual livestck* getFirst();//return first in list
	virtual livestck* getLast();//return last in list

	virtual void putLvstInSolution(MatrixDouble& mEntries);

	//151123 ct changed to avoid using MatrixDouble for livestock numbers
	virtual int checkSoldLivestockAtStartPeriod(std::vector<livestockPack>& lsSoldStart, l_vektor& bprod);
 	virtual int checkSoldLivestockAtEndOfPeriod(std::vector<livestockPack>& lsSoldEnd, l_vektor& bprod);

 	//151123 ct changed to avoid using MatrixDouble for livestock numbers
	virtual void updateExitUnits(std::vector<livestockPack>& lsExit);

	virtual void updateOffspInHerd(MatrixDouble& offsp);
	virtual void updateAgeInHerd();
	virtual livestck* takeOutLivestck(livestck*);
// Troost 20170516 Advanced Demography Model 2
// Troost **********begin block insert**********
	virtual livestck* takeOutLivestock(int type, int age);
// Troost **********end block insert**********


	virtual void printOnScreenLivestck(livestck*);
	virtual void printOnScreen_list(void);
	virtual void deleteList(void);

	virtual void writeListToFile(int fstID); //##Tbe 090820

};

#endif
