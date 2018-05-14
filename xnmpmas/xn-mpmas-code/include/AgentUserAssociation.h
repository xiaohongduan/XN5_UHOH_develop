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
//  File:       AgentUserAssociation.h (formerly part of sector9.h)
//
//  Contents:   Agent user association manages the access to land resources, 
//              provides irrigation infrastructure to its member farm agents,
//              assigns water quotas and distributes irrigation water
//
//------------------------------------------------------------------------------


#ifndef _AgentUserAssociation_h
#define _AgentUserAssociation_h

#include "Agent.h"
//#include "AgentParcel.h"
#include "MpmasPlotCell.h"
#include "MatrixDouble.h"
#include "BasicDefines.h"
#include "FinancialMathFunctions.h"
#include "MarketNonTradablesHandler.h"
#include "PopulationNetworkSegment.h"


//forward decs
class agentF;
class agentL;

///Auxiliary data structure for water rights registry
class AgentWaterRight
{  //auxiliary class, everthing public
   public:

	///@name Data per agent
	//@{
	///Agent farmstead ID
   int AgentFstdID;	

	///Shares to various inflows
   MatrixDouble shares; 	
   MatrixDouble inflows;
 	//@}

	///Constructor
   AgentWaterRight(int numInflows)
	{	AgentFstdID = -1;        
		shares.allocate(numInflows, 1);
		inflows.allocate(numInflows, 1);      
	}

	///Destructor
   virtual ~AgentWaterRight()
	{	shares.deallocate();
		inflows.deallocate();
	}
   
	//virtual  void deallocate();
};


///Auxiliary data structure for registry of infrastructure projects
class InfrastructureProject
{  //auxiliary class, everthing public
   public:

	///@name Data per infrastructure project
	//@{
	///year of implementation
   int yearImplemented;

	///investment object ID (in network file)
	int investObjectID;

	///size in ha
	double sizeInHa;

	///total cost of project
	double totalCost;

	///share of subsidies
	double shareSubsidies;

	///number of participants (1 = individual on-farm project, > 1 = collective off-farm project)
	int numParticipants;

	///Agent farmstead ID of each participant
   MatrixDouble AgentFstdIDs;	
 	//@}

	///Constructor
   InfrastructureProject()
	{	//empty
	}
   
	///Destructor
   virtual ~InfrastructureProject()
	{	AgentFstdIDs.deallocate();
	}
};


///Auxiliary data structure for existing rental contracts
class LandRentalContract
{  //auxiliary class, everthing public
   public:

	///@name Data per rental contract
	//@{
	///agent map ID of user
	int userMapID;

	///map row index of parcel
	int mapRowIdx;

	///map column index of parcel
	int mapColIdx;

	///rental payment
	double payment;

	///duration of contract
	int duration;

	///Water volumns and water rights
	w_jahr* vol;
	w_rechte* w;
	//@}

	///Constructor
   LandRentalContract()
	{	//set variables to default
		userMapID = -1;
		mapRowIdx = -1;
		mapColIdx = -1;
		payment = 0.0;
		duration = -1;
		
		//set pointers to Null
		vol = NULL;
		w = NULL;
	}
   
	///Destructor
   virtual ~LandRentalContract()
	{	//free memory
		delete vol;
		delete w;
	}
};
struct applicationForLandRedistribution
{
		int agentFstID;
		vector<int> landBySoilType; //one column per soil type plus last column = any soil
		vector<double> priorityByCategory; //one column for each category used in prioritizing applications
		bool doApply;
		applicationForLandRedistribution();
		applicationForLandRedistribution(int fstID);
	    bool operator < (const applicationForLandRedistribution& app2) const
	    {
	        for (size_t i = 0; i < priorityByCategory.size(); ++i)
	        {//loop through priority categories while no difference
	      	  double comp = fabs((double) (app2.priorityByCategory[i] - priorityByCategory[i]));
	      	  if ( comp > EPS)
	      			  return true;
	      	  else if (comp < EPS)
	      		  	  return false;
	      	  //else check next category
	        }
	        return false;// all are equal
	    }
	    void printToScreen() {
	   	 printf("Land application by agent %d\n",agentFstID );
	   	 for (size_t i = 0; i < landBySoilType.size() - 1; ++i)
			 {
	   		      printf("Soil: %lu\t%d\n", i, landBySoilType[i] );
			 }
	   	 printf("Any soil\t%d\n",  landBySoilType.back() );
	   	 printf("Priority code: ");
	   	 for (size_t i = 0; i < priorityByCategory.size(); ++i)
			 {
	   		 printf("%.4f ",  priorityByCategory[i] );
			 }
	   	 printf("\n");
	    }
};

///Class AgentUserAssociation, derived from base class Agent

/** 
 * This agent-related class manages the access to land resources, 
 * provides irrigation infrastructure to its member farm agents,
 * assigns water quotas and distributes irrigation water
 *  */ 

class AgentUserAssociation : public agent
{
public:

	// --------------------------------------------------------------------------
   ///         Assignment of individual water rights
   // --------------------------------------------------------------------------

	///@name Assignment of individual water rights
	//@{
	///Number of lottery values
	virtual int getNumLotteryValues();
	
	///... by random data generator
	virtual void assignWaterRightsPerRandomData(FILE* rndWaterRightsStrm, FILE* dbgWaterRightsStrm);
	virtual void readLotteryDataFromStream(FILE* stream);
	virtual void writeLotteryDataToStream(FILE* stream);
	virtual void initializeWaterLottery(int numLotteryValues_);
	virtual double drawRandomWaterRight(int inflow_);

	///... by data from file
	virtual void assignWaterRightsPerActualData(FILE* actWaterRightsStrm, FILE* dbgWaterRightsStrm, int& catchmID_, int& sectorID_);
	virtual void readWaterRightsFromStream(FILE* stream, int& catchmID_, int& sectorID_);
	virtual void writeWaterRightsToStream(FILE* stream);
	
	virtual void assignNumberOfMembers(int numMembers_);
	virtual void createWaterRightsRegistry();
	virtual double getWaterRightForAgent(int agFstdID_, int inflowID_);
	virtual void setWaterRightForAgent(int agFstdID_, int inflowID_, double share_);
	//@}


	// --------------------------------------------------------------------------
   ///         Irrigation infrastructure projects
   // --------------------------------------------------------------------------

	///@name Assignment of infrastructure projects to agents
	//@{
	///... by reading data from file
	virtual void assignInfrastructureProjectsPerActualData(FILE* infrastructureProjectsStrm, int& catchmID_, int& sectorID_, int _numInfrastructureProjects);
	virtual void readInfrastructureProjectsFromStream(FILE* stream, int& catchmID_, int& sectorID_);
	virtual void writeInfrastructureProjectsToStream(FILE* stream);
	
	virtual void createInfrastructureProjectRegistry(int _numInfrastructureProjects);
	virtual int checkIfAgentHasProjects(int agFstdID, int year,
		MatrixDouble& objectIDs, MatrixDouble& sizes, MatrixDouble& purchasePrices, MatrixDouble& sharesEC, MatrixDouble& ratesDC);
	virtual void reportImplementationResult(int agFstdID, int year, int objectID, double size, double purchasePrice, int implemented);
	
	//@}


	// --------------------------------------------------------------------------
   ///         Land and water markets
   // --------------------------------------------------------------------------
	
	//die naechsten Funktionen liefern die Sektor-Schattenpreise je Ha. bzw. je
   //l/s/Monat bzw. die Parzellenzahl und das gesamte monatliche Netto-Water
   //zurueck

	///@name Data query functions
	//@{	
   ///Request basic market information
	virtual double get_Q_B(int stype);   
   virtual double get_Q_W(int month);   
   virtual double get_volB(int month); 
	virtual int getNumAgents(void);
	virtual int getNumPlots(void);
	//@}

	///@name Implementation of of existing rental contracts
	//@{
	///... by reading data from file
	virtual void assignExistingRentalContractsPerActualData(FILE* rentalContractsStrm, int& catchmID_, int& sectorID_);
	virtual void readExistingRentalContractsFromStream(FILE* stream, int& catchmID_, int& sectorID_);
	virtual void writeExistingRentalContractsToStream(FILE* stream);

	virtual void createExistingRentalContractRegistry();
	//@}
	
	//----------------------------------------------------------------------------------
	
	///@name Data input functions
	//@{	
   ///Add to market level and calculate averages
   virtual void addQ_B(int stype, double q_b);
   virtual void addQ_W(int month, double q_w);
   virtual void addVolB(int month, double v_B);
	virtual void addNumAgents(int numberOfAgentsInMarket_);
	virtual void addNumPlots(int numberOfParcelsInMarket_);
   virtual void calcSectorAverages(void);
// Troost 20140707 Land markets - type 4
// Troost **********begin block insert**********
	virtual void calcSectorAverages_ParcelGroupVersion(int use_standard_if_no_obs = false);
	virtual void addnP(int stype, double n);
	virtual void addQtot_B(int stype, double q_b);
// Troost **********end block insert**********
	//@}

	//----------------------------------------------------------------------------------

	///@name Rental market functions
	//@{	
   ///Setting up rental markets for water and land
	virtual void makeBidForRentingOut(gebot* bid);
	virtual void makeBidForRentingIn(gebot* bid);

// Troost 20140707 Land markets - type 4
// Troost **********begin block insert**********
	virtual void recordParcelGroupOffer(parcelGroup* pg, int b_nr, double rprice);
	virtual void doParcelGroupMarket(int milpMode);
#ifdef MIPSTART
	virtual void doParcelGroupMarket2();
#endif
	virtual void implementNewContract(pair<int,double>newContract, parcelGroup* pg);
#ifdef PAR2
	virtual void synchronizeMipCounts(vector<int>& newMipCounts);
#endif //PAR2
// Troost **********end block insert**********

	void doLandRedistribution(agentL* alp);
	void clearPlotsForRedistribution();
	void considerPlotForRedistribution(mpmasPlotCell* p);
	void recordApplicationForLandRedistribution(applicationForLandRedistribution app);

// Troost 20160228 Improvements land market
// Troost **********begin block insert**********
	void recordContract(parcelGroup* pg, double rent);
	void removeOldestYearFromContractList();
// Troost **********end block insert**********


	virtual void facilitateWaterLandMarkets(int milpMode);//renamed from <boden_wasser_transferieren>
   virtual int getStillOpenBids(int*);//renamed from <get_interesse>
   virtual void transferRentalWater(int month);//renamed from <receiveNetInflows>
   virtual void resetRentalMarket();//renamed from <pachtmarkt_zuruecksetzen>
	virtual void resetAverageShadowPrices();//renamed from <schattenpreise_zuruecksetzen>

	//Rental market parameters
	static int getBidDuration(); //Duration of rental contracts
	static void setBidDuration(int bidDuration);
	static int getBidMaxdistance();//Maximum distance to consider renting a plot
	static void setBidMaxdistance(int bidMaxdistance);
	static double getBidPriceCoeff();//Parcel-wise land market: factor applied to shadow price to get to bid
	static void setBidPriceCoeff(double bidPriceCoeff);
	static double getBidProbOthers();//Parcel-wise land market: probability to find a similar parcel at average rental price
	static void setBidProbOthers(double bidProbOthers);
	static int getExistingContracts();//Get grand total of existing contracts for all sectors
	static void setExistingContracts( int existingContracts);
	static int getLmNorentout();//Whether farm household agents rent out land
	static void setLmNorentout( int lmNorentout);
	static int getLmSellforcash();//Whether farm households sell land to obtain liquidity and avoid bankruptcy
	static void setLmSellforcash( int lmSellforcash);
	static int getLoopsRentin();//Plot-wise land market: maximum trials to rent a plot
	static void setLoopsRentin( int loopsRentin);
	static int getLoopsRentout();// Maximum number of trys to rent out land
	static void setLoopsRentout( int loopsRentout);
	static double getMarkdownRentin(); //Plot-wise land market: Markdown for renting-in decisions (< 1.0)
	static void setMarkdownRentin( double markdownRentin);
	static double getMarkupRentout();//Plot-wise land market: Markup for renting-out decisions (> 1.0)
	static void setMarkupRentout( double markupRentout);
/* These seem to be unused:
 	static double getMaxbidAbsolute();//Plot-wise land market: Maximum bid (absolute value)
	static void setMaxbidAbsolute( double maxbidAbsolute);
	static double getMaxbidRelative();//Plot-wise land market: Maximum bid (relative to shadow price)
	static void setMaxbidRelative( double maxbidRelative);
	static double getMinbidAbsolute();
	static void setMinbidAbsolute( double minbidAbsolute);//Plot-wise land market: Minimum bid (absolute value)
*/
	static int getMaxdistanceToPlot();//Parcel-wise land market: Calculate maximum distance from nearest plot, not from farmstead
	static void setMaxdistanceToPlot( int maxdistanceToPlot);
	static double getMinbidRelative();
	static void setMinbidRelative( double minbidRelative);//Plot-wise land market: Minimum bid (relative to shadow price)
	static int getRentalActivity();//Activity on rental markets: 0 = without trade, 1 = with trade
	static void setRentalActivity( int rentalActivity);
	static double getReservePriceCoeff();//Parcel-wise land market: Minimum share of average rental price expected by landlord
	static void setReservePriceCoeff( double reservePriceCoeff);
	static int getAssetsWithLand();//Are there assets that can be attached to land?
	static void readRentalMarketParametersFromFile();
	static void writeLandmarketParameters(char* filename);
	static int getIfLandDistribution();
	static int isPartialLandAllocationAllowed();
	static int getLandDistributionInfo_includePlotsReliquishedInLP();
	static int getLandDistributionInfo_includePlotsFreeAtStart();
	static int getLandDistributionInfo_includePlotsAbandoned();
	static int getLandDistributionInfo_includePlotsSoldInEmergency();

	static void updateNumberPriorizationCategories(int _i);
	static int getNumberPriorizationCategories();

	///Sales to the landowner agent
	virtual double transferPlotToAgentL(plot* p, w_rechte* w, double zins, agentF* afp = NULL);//renamed from <eigentum_uebertragen>
   virtual double computeSellingPrice(int typ, w_jahr* vol, double zins);//renamed from <kaufpreis_berechnen>
   //virtual double computeSellingPrice(int typ, w_jahr* vol, double zins,  segment* segPtr);

	///Transfers to the "other" farm agent
	virtual double transferPlotToAgentO(plot*, w_rechte*, double);

	///Rental transfer, only implemented for farm household agents
	virtual void transferPlotRentedToUser(int agID, int scID, plot* p, w_rechte* w);
	virtual void transferPlotRentedToOwner(int agID, int scID, plot* p, w_rechte* w);
	virtual void transferPaymentToAgent(int agID, int scID, double pmt, transferTypeAgentAgentPayment pmtType = agentagentPaymentLandRent);

// Troost 20170516 Advanced Demography Model 2
// Troost **********begin block insert**********
	virtual double transferPlotToAgentF(plot* p, w_rechte* w, double zins, agent* oldAgent, agentF* newAgentF);
// Troost **********end block insert**********


	///Write market outcomes to files
   virtual void writeRentalMarketStatisticsForLand();		// needs open stream "aStrm_Lrm"
   virtual void writeRentalMarketStatisticsForWater();// needs open stream "aStrm_Lrm"
   virtual void P_in_datei_schreiben();  //outdated, writes with headers in German

	//prepare streams to record each transaction with flag 22
   virtual void openStreamRentalMarketTransactions();
   virtual void closeStreamRentalMarketTransactions();
   //virtual void recordRentalMarketTransaction(int plotX, int plotY, int plotStatus, int soilType, int ownerAgID, int newOwnerAgID, int fromAgID, int toAgID , transactionTypeRentalMarket transactionType, double payment, double newEntf, int duration, int renterID);
   //record transaction in open stream. Note: Should be called after transaction is complete. All new(after transaction) info is read from plot object, while all old(before transaction) info has to be passed to the function.
   virtual void recordRentalMarketTransaction(mpmasPlotCell* p, int oldOwnerAgID, int fromAgID, transactionTypeRentalMarket transactionType, double payment, int oldDuration);


// Troost 20140630 Voluntary exit decision in Investment LP
// Troost **********begin block insert**********
	//virtual double getAvgRentalPrice(int soilType, segment* segPtr);
   virtual double getAvgRentalPrice(int soilType);
// Troost **********end block insert**********
	//@}

	// --------------------------------------------------------------------------
   ///         Constructor / destructor
   // --------------------------------------------------------------------------

	///@name Constructors and destructors
	//@{
	///Constructor that also allocates required memory
	AgentUserAssociation(int agentID_, int catchmentID_, int sectorID_);

	///Copy constructor
	//AgentUserAssociation(const AgentUserAssociation& association_);

	///Assignment operator
	//AgentUserAssociation& operator=(const AgentUserAssociation& association_);

	///Destructor
	virtual ~AgentUserAssociation();

	//@}
	
	
protected:
	///@name Protected member data
	//@{
	///Catchment and sector IDs
	int catchmentID;
	int sectorID;

	///Type of agent (here: agent association = 2)
	int agType;

	///Here: Agent association ID (not yet used)
	int agentID;
	
	///Number of members
	int numMembers;
	
	///Number of lottery values (if = 0: actual data, else: random data)
	int numLotteryValues;

	///Registry of water rights
	AgentWaterRight** waterRightsRegistry;

	///Random-data information for assignment of water rights
	MatrixDouble wlotto;

	///Instance of rental market
   bwmarkt pachtmarkt;
   parcelGroupMarket parcelGroupMkt;

	///Number of agents and parcels in land market
	int numberOfAgentsInMarket;
	int numberOfPlotsInMarket;

	///Average shadow prices for land and water 
   MatrixDouble Q_B;
   MatrixDouble Q_W; 

// Troost 20140707 Land markets - type 4
// Troost **********begin block insert**********
	MatrixDouble nP; //number of parcels traded
	MatrixDouble Qtot_B; // permanent sum of all rents for soil type
// Troost **********end block insert**********

// Troost 20160228 Improvements land market
// Troost **********begin block insert**********

	//how many past years to include in the calculation of the average rental price
	static int lengthContractMemory;

	//indicates the number of contracts recorded in a certain year
	// year 0 records the number for the most recent year
	vector<int> numContractInfosPerYear;

	//list of rental contract observations for calculating the average rental price
	//new observations are added at the end,
	//old observations are deleted from the start
	deque<vector<double> > listOfObservedContracts;

// Troost **********end block insert**********

// Troost 20141210 Land markets - type 4, assets traded with land
// Troost **********begin block insert**********
	static MatrixDouble standardRentalExp;
// Troost **********end block insert**********

	///Expected gross water availability per parcel
   MatrixDouble  volB;

	///Number of irrigation infrastructure projects
	int numInfrastructureProjects;

	///Number of existing rental contracts at start of simulation
	int numExistingContracts;

	///Registry of infrastructure projects
	InfrastructureProject* infrastructureProjectRegistry;

	///Registry of existing rental contracts at start of simulation
	LandRentalContract* landRentalContractRegistry;

	void returnFscanInfrastructure(int retVal, char* strVar);
	void returnFscanLottery(int retVal, char* strVar);
	void returnFscanWaterRights(int retVal, char* strVar);

// Troost 20130930 + 20160119 Additional output land market
// Troost **********begin block insert**********
	//Stream to write out transactions with flag 22
	FILE* transactionStrm;
	FILE* bidsStrm;
// Troost **********end block insert**********

	//Rental market parameters
	//implemented as class variables for now,
	//but could easily be made user association specific
	static double minbid_relative;
	static double minbid_absolute;
	static double maxbid_relative;
	static double maxbid_absolute;
	static double markup_rentout;
	static double markdown_rentin;
	static int maxdistance_to_plot;
	static int lm_norentout;
	static int loops_rentout;
	static int loops_rentin;
	static int bid_maxdistance;
	static int bid_duration;
	static int existing_contracts;
	static int rental_activity;
	static int lm_sellforcash;
	static int assets_with_land;
#ifdef MIPSTART
	static int column_based_valuation;
#endif
	static double reserve_price_coeff;
	static double bid_price_coeff;
	static double bid_prob_others;
	static double markup_second_bid;
	static int land_distribution_stage;
	static int allow_partial_land_alloc;
	static int numberPriorizationCategories;
	static int landDist_includeFreeAtStart;
	static int landDist_includeRelinquished;
	static int landDist_includeAbandoned;
	static int landDist_includeSoldEmergency;
	//@}

	vector<applicationForLandRedistribution> applicationsForRedistribution;
	vector<vector<mpmasPlotCell*> > plotsForRedistribution;
};




#endif /* _AgentUserAssociation_h */
