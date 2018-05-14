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
//  File:       AgentProducerOrganization.h
//
//  Contents:   Agent producer organization manages the collective buying 
//              and selling activities of its member farm agents
//
//
//------------------------------------------------------------------------------


#ifndef _AgentProducerOrganzation_h
#define _AgentProducerOrganzation_h
#include "BasicDefines.h"
#include "Agent.h"
#include "MatrixDouble.h"

#include "FinancialMathFunctions.h"
#include <vector>
#include <algorithm>
#include <iostream>


///Auxiliary data structure for individual member contributions
class AgentContribution
{  //auxiliary class, everthing public
   public:

	///@name Data per agent
	//@{
	///Agent farmstead ID (= member ID)
   int AgentFstdID;	

	///buying/selling amounts of this member
   MatrixDouble serviceAmounts; 	
  	//@}

	///Constructor
   AgentContribution(int numServices)
	{	AgentFstdID = -1;        
		//serviceAmounts.allocate(numServices, 2); ## Tbe 120117
		serviceAmounts.allocate(numServices);
	}

	///Destructor
   virtual ~AgentContribution()
	{	serviceAmounts.deallocate();
	}
   
	//virtual  void deallocate();
};



///Auxiliary data structure for PO services to members
class serviceActivity
{  //auxiliary class, everthing public
   public:

	///@name Data per member service
	//@{
	///ID of service
   int serviceID;

	///LP indices for service amount
   int serviceQuantityRowLpProdOrg;
	int serviceQuantityColLpAgentF;

	///LP indices for service "price"
   int serviceProfitColLpProdOrg;
	int serviceQuantityColProdOrg;
  	//@}

	///Constructor
   serviceActivity()
	{
	}

	///Destructor
   virtual ~serviceActivity()
	{
	}
};




///Class AgentProducerOrganization, derived from base class Agent

/** 
 * This agent-related class manages manages the collective buying 
 * and selling activities of its member farm agents
 *  */ 

class AgentProducerOrganization : public agent
{
public:

//------------------------------------------------------------------------------------

	/**
	* Data query, input, output functions
	*/	

	///@name Data query functions
	//@{	
   ///Request basic information
	virtual int getNumMembers(void);
	virtual int checkMemberAccess(int symbolicObjectID);
	//@}
	
	//----------------------------------------------------------------------------------

	///@name File handling
	//@{	
   ///reading and writing files
	virtual void readInputFileProducerOrganization(const char* filename);
	virtual void writeInputFileProducerOrganization(const char* filename);
	virtual void writeMemberDirectoryToFile(const char* filename);
	virtual void writeMemberInventoryToFile(const char* filename);
	virtual void writeBusinessResultsToFile(const char* filename);
	//@}

//------------------------------------------------------------------------------------

	/**
	* Decision-making functions using MP
	*/	

	///@name Business activities
	//@{	
   ///request membership for certain membership type
	virtual int requestMembership(int fstID, int symbolicObjectID);
	virtual void terminateAllMemberships();
	virtual void checkMembershipTypes();
		
	///collect contributions from members
	virtual void collectContributionsFromMembers();
	virtual void createMemberInventory();
	virtual void deleteMemberInventory();

	///compute service prices for members
	virtual void computeFarmGatePricesForMembers();
	virtual void copyPricesForMembersIntoPricePaths();
	virtual void closingBusinessEndOfYear();
	
	//@}

	///Functions for production planning
	virtual double producPlan();

	virtual int getNNrowsInternal();
	virtual int getNNcolsInternal();
	virtual int getNumRowsInternal();
	virtual int getNumColsInternal();
	virtual double getActualTotalGrossMargin();

	virtual void updateAgentZRow();//formerly 'kapaz9()'
	virtual void updateStartPeriodRHS();//formerly 'kapaz9()'
	virtual void updateEndOfPeriodRHS();//formerly 'kapaz9()'
	virtual void checkForNegativeValuesInRHS();

	virtual int copyLpDataInProductionMode(int isIntegerProblem);
	
#ifdef LP9
	virtual double solveLinPrbOSL(int savePrim, int saveDual);
	virtual double solveIntPrbOSL(int savePrim, int saveDual, int& forSol);
#endif //LP9
	virtual void makeNewPrb();
	virtual void freePrb();




	// --------------------------------------------------------------------------
   ///         Constructor / destructor
   // --------------------------------------------------------------------------

	///@name Constructors and destructors
	//@{
	///Constructor that also allocates required memory
	AgentProducerOrganization(int agentID_, int catchmentID_);

	///Copy constructor
	//AgentProducerOrganization(const AgentProducerOrganization& association_);

	///Assignment operator
	//AgentProducerOrganization& operator=(const AgentProducerOrganization& association_);

	///Destructor
	virtual ~AgentProducerOrganization();
	//@}
	
	
protected:
	///@name Protected member data
	//@{
	///Catchment ID
	int catchmentID;

	///Agent organization ID
	int agentOrganizationID;

	///Regional marekt access
	int marketID;

	///Type of agent (here: producer organization)
	int agType;

	///Number of members
	int numMembers;
	
	///Member directory
	vector <int> memberDirectory;
	vector <int>::iterator iterMemberDir;

	///Number of membership types
	int numMembershipTypes;	
	
	///object IDs of membership types
	int* objectIdMembershipTypes;

	///Number of PO services to members
	int numServices;

	///LP-related indices for PO services
	serviceActivity* lpRelatedIndicesOfServices;

	///Inventory of member contributions
	AgentContribution** memberInventory;

	///Farm-gate prices for members
	MatrixDouble farmGatePricesForMembers;

	/// Target value of production LP
	double targetValueProduction;

	int agentMipCount;
	///reading membership data from open file
	void returnFscanMembersOfProducerOrganization(int retVal, char* strVar);

//------------------------------------------------------------------------------------

	/**
	 * Data structures for LP module
	*/
	//double* erw_preise;			//expected prices, used to initialize objective fucntion coefficients
	double* varsRHS;				//the agent's resources (Right Hand Side)
	double* varsLHS;				//used capacities of resources (Left Hand Side)
	double* schattenpreise;		//shadow prices

	int solvedLP;					//forced solution (default 0, inv 1, prd 2, con 3)	
	
	///Information for "forced solution" (case that LP does not solve)
	MatrixDouble mEntries;		//matrix with information for "forced" solution

	l_vektor bprod;//Loesungsvektor (Umfaenge, Zielfunktionskoeff., Zielwert)


	//@}
};




#endif /* _AgentProducerOrganzation_h */
