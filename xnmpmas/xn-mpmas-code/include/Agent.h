//------------------------------------------------------------------------------
//     8
//  Thomas Berger (main author), Pepijn Schreinemachers, and Thorsten Arnold
//
//
//  Hohenheim University (490d)
//  Professorship for Land Use Economics in the Tropics and Subtropics
//
//  May not be distributed without permission by main author
//
//  File:       Agent.h (formerly agent.h)
//
//  Contents:   Header for the basic class of agents
//
//
//
//------------------------------------------------------------------------------


#ifndef _Agent_h
#define _Agent_h
#include "MainAuxiliaryFunctions.h"
#include "PopulationNetworkSegment.h"

class agent
{  protected:
   int agType;    //agent type  (basic = -1)
	int bnummer;   //agent ID
	int sc_nr;     //ID of subcatchment
	int sek_nr;    //ID of microcatchment ("sektor")
	int isActive;  //status of agent during simulation
	int ageOfAgent;//age of agent

	int curMPIRank;	//MPI rank that solved last decision for agent

   public:
	agent* nextAgent;

	agent(int agid, int scid, int skid);
	virtual ~agent()
	{
	}

	//member functions
   virtual void printToScreen();
	virtual void setNextPtr(agent*);
	virtual agent* getNext();

   virtual int getAgType();
   int getAgentID()     {return bnummer; };
   int getSectorID()    {return sek_nr; };
   int getCatchmentID() {return sc_nr; };

	virtual int getActivityStatus();//true = is active
	virtual void setActivityStatus(int agentIsActive);//true = is active
   virtual int get_alter();
   virtual void set_alter(int yr);
   virtual int getCurMpiRank() const;
   virtual void setCurMpiRank(int curMpiRank);

//"Abstract" functions to be implemented for derived classes
   virtual int getAgentMapId();
   virtual void synchronizeDecisionResultsMPI(milpModeTypes mode);

// Troost 20170516 Advanced Demography Model 2
// Troost **********begin block insert**********
	virtual void submitApplicationForLandRedistribution();
// Troost **********end block insert**********


   // Troost 20141210 Land markets - type 4, assets traded with land
   // Troost **********begin block insert**********
   	///Functions related to investment goods
	virtual int get_binv_size() ;
	virtual int get_binv_anz_obj() ;
	virtual char* get_binv_acc(int i) ;
	virtual void set_binv_acc(int i, char* chr) ;
	virtual int get_binv_code(int i);	//Parameter Row
	virtual double get_binv_ask(int i);	//Parameter Row
	virtual void set_binv_ask(int i, double price); //Parameter Row
	virtual double get_binv_anz(int i); //Parameter Row
	virtual int get_binv_age(int i); //Parameter Row
	virtual void set_binv_age(int i, int age); //Parameters Row, age
	virtual double get_binv_shareEC(int i); //Parameter Row
	virtual void set_binv_shareEC(int i, double shr); //Parameters Row, share
	virtual double get_binv_rateDC(int i); //Parameter Row
	virtual void set_binv_rateDC(int i, double rat); //Parameters Row, rate
	virtual investdef get_binv(int i);
	virtual int inventarisieren(char* acc, int oID, double pPr, double siz, double ssf, double iDC, int age, int plotX =
			-1, int plotY = -1, int newlyAttach = false);
	virtual void deinventarisieren(int, string remark, int detach = 1, int disinvest = 1); // Uebergabe: Inventarnr.
	virtual void binvest_ausschreiben(int i = -1);
	virtual void binvest_writeToStream(FILE* strm);
};


#endif

