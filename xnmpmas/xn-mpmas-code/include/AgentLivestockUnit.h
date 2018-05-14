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
//  File:       AgentLivestockUnit.h (formerly livestck.h)
//
//  Contents:   Livestock class, containing individual information of each
//              livestock unit, e.g. age, sex.
//
//
//------------------------------------------------------------------------------

#ifndef _AgentLivestockUnit_h
#define _AgentLivestockUnit_h

#include <iostream>
#include <stdio.h>

class livestck
{
protected:
	int hhID;//Household ID (i.e. "Betriebsnummer)
	int scID;//Watershed ID (i.e. "SubcatchmentNum")
	int pID; //personal ID
	int age;
	int type;

	//int** health; //health status

public:
	livestck(int hID, int scID, int pID, int type, int age);
	virtual ~livestck()
	{
	}

	//Pointers for list of herd members
	livestck* nextLivestck;
	void setNextPtr_M (livestck*);
	livestck* getNext_M();

	livestck* prevLivestck;
	void setPrevPtr_M (livestck*);
	livestck* getPrev_M();

	//Elementfunctions
	virtual int getHhID();
	virtual int getScID();
	virtual int getPID();
	virtual int getAge();
	virtual void increaseAge();
	virtual int getType();

	virtual void printToScreen();

	virtual void writeToFile(FILE* stream);

};

#endif
