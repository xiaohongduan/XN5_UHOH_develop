#ifndef _MPMAS_LUCIA_LIVSIM_COUPLING_H_
#define _MPMAS_LUCIA_LIVSIM_COUPLING_H_


#include "MpmasTcpConnector.h"

#include <string>
#include <limits>
#include <list>
#include <map>
#include <exception>
#include <stdexcept>
#include <algorithm>


//forward declarations
class s_liste;

using namespace std;

/*enum LivSimFeedGroups
{
	livSimFeedGroupDefault,
	livSimFeedGroupCalves,
	livSimFeedGroupLactating
};*/

//#define MPMAS_DEBUG_LIVSIM_DESERIALIZATION
#define LIVSIM_SUBHERD_MAX 100
inline int asLivSimHerdId(int fstID, int herdGroupId) {return fstID * LIVSIM_SUBHERD_MAX +  herdGroupId;}
inline int fstIdFromLivSimHerdId(int herd_id) {return (int) ( herd_id / LIVSIM_SUBHERD_MAX) ; }//integer division !
inline int herdgroupIdFromLivSimHerdId(int herd_id) {return (int) ( herd_id % LIVSIM_SUBHERD_MAX) ; }//integer division !

#define MACROLIST_LivSimHerdManagementTable_COLUMNS \
	   X(int, target_herd,  0) \
		X(int, replacement_age_bulls,  1) \
	   X(int, max_open,  2) \
	   X(int, max_bulls,  3) \
	   X(int, max_age_cows, 4) \
	   Y(int, max_lact,  5)


// Note: column zero in table is herd_id, but that is not part of member attributes
#define MACROLIST_LivSimHerdMemberTable_ATTRIBUTES \
		X(int, id,  1) \
		X(string, breed,  2) \
	   X(int, sex,  3) \
	   X(double, age,  4) \
	   X(double, bw, 5) \
	   X(int, gestating, 6) \
	   X(int, lactating, 7) \
	   X(double, pregind, 8) \
	   X(double, calvind, 9) \
	   X(double, lactind, 10) \
	   X(int, damid, 11) \
	   X(int, weeks_in_gestation, 12) \
	   X(int, removed, 13) \
	   Y(double, milk, 14)


#define MACROLIST_LivSimGrazingTable_ATTRIBUTES \
		X(int, herd_id,  0) \
		X(int, season,  1) \
	   X(int, pixel_x,  2) \
	   X(int, pixel_y,  3) \
	   Y(int, priority,  4)



#define MACROLIST_LivSimFeedingTable_ATTRIBUTES \
		X(int, herd_id,  0) \
	   X(int, feed_type,  1) \
	   X(int, feed_id,  2) \
	   X(double, jan,   3) \
	   X(double, feb,   4) \
	   X(double, mar,   5) \
	   X(double, apr,   6) \
	   X(double, may,  7) \
	   X(double, jun,  8) \
	   X(double, jul,  9) \
	   X(double, aug,  10) \
	   X(double, sep,  11) \
	   X(double, oct,  12) \
	   X(double, nov,  13) \
	   Y(double, dec,  14)

//X(int, feeding_group_id,  1)
//defining macros for X-macros idiom
namespace MpmasLuciaLivSim	 {
	inline void stream_column_error_check(istream& is, string entityname, string columnname ) throw()
	{
		if (is.eof())
		{
			stringstream errmsg;
			errmsg << "Error when receiving " << entityname
					 << ": Not enough columns.\nReached EOF at column " << columnname << "\n";
			throw runtime_error(errmsg.str());
		}
		if (is.fail() || is.bad())
		{
			stringstream errmsg;
			errmsg << "Error when receiving " << entityname
					 << ": Error when trying to read column " << columnname << "\n";
			throw runtime_error(errmsg.str());
		}
	}
}


class LivSimHerdManagementTable : public MpmasTcpConnectionSendableTable<LivSimHerdManagementTable>
{ //X macro pattern
	private:
//defining the columns in the macrolist as class members
#define X(a,b,c) a b;
#define Y(a,b,c) a b;
		MACROLIST_LivSimHerdManagementTable_COLUMNS
#undef X
#undef Y

	public:
		//Default constructor - just taking all arguments in the order as listed in the macro and initialize them with 0
		LivSimHerdManagementTable( )
		: //initializer list
#define X(a,b,c) b(0),
#define Y(a,b,c) b(0)
		MACROLIST_LivSimHerdManagementTable_COLUMNS
#undef X
#undef Y
		{}

		//Standard constructor - just taking all arguments in the order as listed in the macro and initialize them corresponding attributes
		LivSimHerdManagementTable( //constructor arguments
#define X(a,b,c) a b##_,
#define Y(a,b,c) a b##_
		MACROLIST_LivSimHerdManagementTable_COLUMNS
#undef X
#undef Y
			) : //initializer list
#define X(a,b,c) b(b##_),
#define Y(a,b,c) b(b##_)
		MACROLIST_LivSimHerdManagementTable_COLUMNS
#undef X
#undef Y
		{}

		//Copy constructor - just taking all arguments in the order as listed in the macro and initialize them with 0
		LivSimHerdManagementTable( const LivSimHerdManagementTable & in )
		{
#define X(a,b,c) b = in.b ;
#define Y(a,b,c) b = in.b ;
		MACROLIST_LivSimHerdManagementTable_COLUMNS
#undef X
#undef Y
		}

		string getTableName() { return string("LivSimHerdManagementTable"); };

		string getColName(int i)
		{
			switch(i) {
				#define X(a,b,c) case c : return string(#b) ; break;
				#define Y(a,b,c) case c : return string(#b) ; break;
						MACROLIST_LivSimHerdManagementTable_COLUMNS
				#undef X
				#undef Y
				default :
					throw runtime_error("Invalid column name request in LivSimHerdManagementTable");

			}
			return string();
		}

		int getNumberOfColumns()
		{		//number of columns is supposed to be the index of the last element + 1
				#define X(a,b,c)
				#define Y(a,b,c) return c + 1;
						MACROLIST_LivSimHerdManagementTable_COLUMNS
				#undef X
				#undef Y
		}
		void serializeFields(ostream & os, char fieldsep, char linesep)
		{
			#define X(a,b,c) os << b << fieldsep;
			#define Y(a,b,c) os << b << linesep;
					MACROLIST_LivSimHerdManagementTable_COLUMNS
			#undef X
			#undef Y
		}
		void deserializeFields(istream & is, char fieldsep, char linesep)
		{

			#define X(a,b,c) MpmasTcpConnectionAuxiliary::getUntilDelimiterAndConvert(is, b, fieldsep);\
					MpmasLuciaLivSim::stream_column_error_check(is, "herd management info", string(#b));
			#define Y(a,b,c) MpmasTcpConnectionAuxiliary::getUntilDelimiterAndConvert(is, b, linesep);			\
					MpmasLuciaLivSim::stream_column_error_check(is, "herd management info", string(#b));

					MACROLIST_LivSimHerdManagementTable_COLUMNS
			#undef X
			#undef Y
		}
};




class LivSimAnimalInfo
{
	private:
//defining the columns in the macrolist as class members
#define X(a,b,c) a b;
#define Y(a,b,c) a b;
		MACROLIST_LivSimHerdMemberTable_ATTRIBUTES
#undef X
#undef Y


public:
		//Default constructor - just taking all arguments in the order as listed in the macro and initialize them corresponding attributes
		LivSimAnimalInfo( ) : //initializer list
#define X(a,b,c) b(),
#define Y(a,b,c) b()
			MACROLIST_LivSimHerdMemberTable_ATTRIBUTES
#undef X
#undef Y
	{ id = -1;}


		//Standard constructor - just taking all arguments in the order as listed in the macro and initialize them corresponding attributes
		LivSimAnimalInfo( //constructor arguments
#define X(a,b,c) a b##_,
#define Y(a,b,c) a b##_
				MACROLIST_LivSimHerdMemberTable_ATTRIBUTES
#undef X
#undef Y
		) : //initializer list
#define X(a,b,c) b(b##_),
#define Y(a,b,c) b(b##_)
			MACROLIST_LivSimHerdMemberTable_ATTRIBUTES
#undef X
#undef Y
	{}

		//getters and setters
#define X(a,b,c)  a get_ ## b () const {return b;}  \
						  void set_ ## b (a b##_) {b = b##_;}
#define Y(a,b,c) a get_ ## b () const {return b;}  \
						  void set_ ## b (a b##_) {b = b##_;}
				MACROLIST_LivSimHerdMemberTable_ATTRIBUTES
#undef X
#undef Y


				void serializeFields(ostream & os, char fieldsep, char linesep)
				{
					#define X(a,b,c) os << b << fieldsep;
					#define Y(a,b,c) os << b << linesep;
						MACROLIST_LivSimHerdMemberTable_ATTRIBUTES
					#undef X
					#undef Y
				}
				void deserializeFields(istream & is, char fieldsep, char linesep)
				{
					#define X(a,b,c) \
						MpmasTcpConnectionAuxiliary::getUntilDelimiterAndConvert(is, b, fieldsep);  \
						MpmasLuciaLivSim::stream_column_error_check(is, "herd member info", string(#b));
					#define Y(a,b,c) MpmasTcpConnectionAuxiliary::getUntilDelimiterAndConvert(is, b, linesep); ;\
						MpmasLuciaLivSim::stream_column_error_check(is, "herd member info", string(#b));

					//use macros for list
						MACROLIST_LivSimHerdMemberTable_ATTRIBUTES

					//undefine macros
					#undef X
					#undef Y
				}

};
class matchAnimalID
{
    int id;

public:
    matchAnimalID(const int &_ID) : id(_ID) {}

    bool operator()(const LivSimAnimalInfo &animal) const
    {
        return animal.get_id() == id;
    }
};
class LivSimHerdIterator;
class LivSimHerd
{
	protected:
		int herdId;
	   list<LivSimAnimalInfo> animals;
	   friend class LivSimHerdIterator;

	public :

	   LivSimHerd() : herdId(-1), animals() {}

	   LivSimHerd(int _id) : herdId(_id), animals() {}
	   //
		void addAnimal(LivSimAnimalInfo newAnimal) { animals.push_back(newAnimal);}
		//void removeAnimal(LivSimAnimalInfo animal) { animals.remove(animal);}
		void setAnimalRemovedById(int id) {
			list<LivSimAnimalInfo>::iterator it = std::find_if(animals.begin(), animals.end(), matchAnimalID(id));
			if (it != animals.end())
			{
				it->set_removed(true);
				//animals.erase(it);
			}
			else
			{	stringstream errmsg;
				errmsg << "Error can't remove (set removed) animal " << id << " from herd " << herdId << ". Animal not found in herd\n";
				throw runtime_error(errmsg.str());
			}
		}
		LivSimAnimalInfo takeOutAnimalById(int id) {
			list<LivSimAnimalInfo>::iterator it = std::find_if(animals.begin(), animals.end(), matchAnimalID(id));
			if (it != animals.end())
			{
				//it->set_removed(true);
				LivSimAnimalInfo copyForReturn = *it;
				animals.erase(it);
				return copyForReturn;
			}
			else
			{	stringstream errmsg;
				errmsg << "Error can't remove (take out) animal " << id << " from herd " << herdId << ". Animal not found in herd\n";
				throw runtime_error(errmsg.str());
			}
		}

		const LivSimAnimalInfo getAnimalById(int id)
		{
			list<LivSimAnimalInfo>::iterator it = std::find_if(animals.begin(), animals.end(), matchAnimalID(id));
			if (it != animals.end())
			{
				return (*it);
			}
			return LivSimAnimalInfo();

		}
		LivSimAnimalInfo* getPtrToAnimalById(int id)
		{
			list<LivSimAnimalInfo>::iterator it = std::find_if(animals.begin(), animals.end(), matchAnimalID(id));
			if (it != animals.end())
			{
				return &(*it);
			}
			return NULL;

		}
		void clear() { animals.clear();}
		int getHerdId() { return herdId;}
		size_t getHerdSize() { return animals.size(); }

	   //serialization for communication
	  void serializeMembers (ostream & os, char fieldsep, char linesep)
	  {
			for (list<LivSimAnimalInfo>::iterator it = animals.begin(); it != animals.end(); ++it)
			{
				os << herdId << fieldsep;
				(it)->serializeFields(os, fieldsep, linesep);
			}
	  }
	  int deserializeMembers (istream & is, char fieldsep, char linesep)
	  {
		  clear();
		  int currentHerdId = herdId; //Note: HerdId of first line was already read by LivSimHerdTable in order to get here
		  while(currentHerdId == herdId) //while herdId is my herd id and not EOF, keep reading animals from stream
		  {
			  LivSimAnimalInfo individual;
			  individual.deserializeFields(is, fieldsep, linesep);
			  addAnimal(individual);
#ifdef MPMAS_DEBUG_LIVSIM_DESERIALIZATION
			   cout << "Read animal: ";
			   individual.serializeFields(cout,':',';');
				cout << endl;
#endif
			  if ( !MpmasTcpConnectionAuxiliary::getUntilDelimiterAndConvert(is, currentHerdId, fieldsep)  ) // if this fails, either error or EOF reached
			  {
				  return -1;
			  }
		  }
		  return currentHerdId;
	  }

		void increaseAgeAndBw(double years, double perc) //just for testing
		{
			for (list<LivSimAnimalInfo>::iterator it = animals.begin(); it != animals.end(); ++it)
			{
				if (!it->get_removed() )
				{	(it)->set_age((it)->get_age() + years) ;
					(it)->set_bw((it)->get_bw() * (1+perc/100)) ;
				}
			}
		}

		void adoptAnimalsFromHerd (LivSimHerd* oldHerd);


};

class LivSimHerdIterator
{
    LivSimHerd* herd_;
    list<LivSimAnimalInfo>::iterator internalIt   ;

public:
    LivSimHerdIterator(LivSimHerd& herd)
    : herd_(&herd), internalIt( herd.animals.begin() ) {}

    const LivSimAnimalInfo& getAnimal()
    {
        return *internalIt;
    }

    LivSimHerdIterator& operator ++ ()
    {
   	 if (internalIt != herd_->animals.end())
   		 ++internalIt;
       return *this;
    }

    bool operator != (const LivSimHerdIterator& i) const
    {
        assert(herd_ == i.herd_);
        return internalIt != i.internalIt;
    }
    bool reachedEnd ()
    {
   	 return (internalIt == herd_->animals.end() );
    }
};



class LivSimHerdTable : public MpmasTcpConnectionSendableTable<LivSimHerdTable>
{
	protected:
		typedef map<int,LivSimHerd*> typeOfListOfHerds;
		typeOfListOfHerds listOfHerds;
		bool ownHerdPointers;


	public:
		//Default constructor - just taking all arguments in the order as listed in the macro and initialize them with 0
		LivSimHerdTable( ) 	: listOfHerds(), ownHerdPointers(false)	{}
		LivSimHerdTable(bool ownHerds ) 	: listOfHerds(), ownHerdPointers(ownHerds)	{}

		//Copy without owning
		LivSimHerdTable(const LivSimHerdTable& tableToCopyFrom ) : listOfHerds(), ownHerdPointers(false)
		{
			listOfHerds.clear();
			for ( typeOfListOfHerds::const_iterator it = tableToCopyFrom.listOfHerds.begin(); it != tableToCopyFrom.listOfHerds.end(); ++it)
			{
				addHerd(it->second);
			}
		}


		virtual ~LivSimHerdTable(){ clear();}
		void addHerd(LivSimHerd* newHerd) { listOfHerds[newHerd->getHerdId()] = newHerd;}
		LivSimHerd* getHerdPointer(int id)
		{ 	typeOfListOfHerds::iterator it = listOfHerds.find(id);
			if (it == listOfHerds.end())
				return NULL;
			else
				return it->second;
		}

		LivSimHerd* getFirstHerdPointer()
		{ 	typeOfListOfHerds::iterator it = listOfHerds.begin();
			if (it == listOfHerds.end())
				return NULL;
			else
				return it->second;
		}

		void removeHerd(LivSimHerd* herd) { listOfHerds.erase(herd->getHerdId());}
		void removeHerd(int id) { listOfHerds.erase(id);}



		virtual void clear()
		{
			if (ownHerdPointers)
			{
				for (typeOfListOfHerds::iterator it = listOfHerds.begin(); it != listOfHerds.end(); ++it)
				{
					delete (it->second);
					it->second = NULL;
				}
				listOfHerds.clear();
			}
			else
			{	listOfHerds.clear();
			}
		}

		public:


			void increaseAgeAndBw(double years, double perc)
			{
				for (typeOfListOfHerds::iterator herdIt = listOfHerds.begin(); herdIt != listOfHerds.end(); ++herdIt)
				{
					(herdIt->second)->increaseAgeAndBw(years, perc);
				}
			}

		//functions for serialization with  MpmasTcpConnectionSendableTable
		string getTableName() { return string("LivSimHerdTable"); };

		string getColName(int i)
		{
			switch(i) {
				case 0 : return "herd_id"; break;
				#define X(a,b,c) case c : return string(#b) ; break;
				#define Y(a,b,c) case c : return string(#b) ; break;
					MACROLIST_LivSimHerdMemberTable_ATTRIBUTES
				#undef X
				#undef Y
				default :
					throw runtime_error("Invalid column name request in LivSimHerdTable\n");

			}
			return string();
		}

		int getNumberOfColumns()
		{		//number of columns is supposed to be the index of the last element + 1
				#define X(a,b,c)
				#define Y(a,b,c) return c + 1;
					MACROLIST_LivSimHerdMemberTable_ATTRIBUTES
				#undef X
				#undef Y
		}
		void serializeFields(ostream & os, char fieldsep, char linesep)
		{
			for (typeOfListOfHerds::iterator herdIt = listOfHerds.begin(); herdIt != listOfHerds.end(); ++herdIt)
			{
				(herdIt->second)->serializeMembers(os, fieldsep, linesep);
			}
		}
		void deserializeFields(istream & is, char fieldsep, char linesep)
		{
				if (ownHerdPointers)
						clear();

				int currentHerdId;
				if ( ! MpmasTcpConnectionAuxiliary::getUntilDelimiterAndConvert(is, currentHerdId, fieldsep)  )
				{
					//empty table
					return;
				}
				try {
					while (currentHerdId != -1)
					{
#ifdef MPMAS_DEBUG_LIVSIM_DESERIALIZATION
							cout << "Deserializing herd " << currentHerdId  << ":" << endl;
#endif
						if (ownHerdPointers)
						{
							LivSimHerd* newHerd = new LivSimHerd(currentHerdId);
							addHerd(newHerd);
							currentHerdId = newHerd->deserializeMembers(is, fieldsep, linesep);
						}
						else
						{ currentHerdId = listOfHerds.at(currentHerdId)->deserializeMembers(is, fieldsep, linesep);
						}
					}
				}
				catch(out_of_range& e)
				{
					stringstream errmsg;
					errmsg << "Error on MPMAS side, function LivSimHerdTable->deserializeFields. Herd with id " << currentHerdId << " unknown to MPMAS\n";
					throw runtime_error(errmsg.str());
				}

			}

};

class MpmasLuciaGrazingOpportunity {

		//X macro pattern
			private:
		//defining the columns in the macrolist as class members
		#define X(a,b,c) a b;
		#define Y(a,b,c) a b;
			MACROLIST_LivSimGrazingTable_ATTRIBUTES
		#undef X
		#undef Y

			public:
				//Default constructor - just taking all arguments in the order as listed in the macro and initialize them with 0
				MpmasLuciaGrazingOpportunity( )
				: //initializer list
		#define X(a,b,c) b(0),
		#define Y(a,b,c) b(0)
					MACROLIST_LivSimGrazingTable_ATTRIBUTES
		#undef X
		#undef Y
				{}

		//Standard constructor - just taking all arguments in the order as listed in the macro and initialize them corresponding attributes
				MpmasLuciaGrazingOpportunity( //constructor arguments
		#define X(a,b,c) a b##_,
		#define Y(a,b,c) a b##_
						MACROLIST_LivSimGrazingTable_ATTRIBUTES
		#undef X
		#undef Y
					) : //initializer list
		#define X(a,b,c) b(b##_),
		#define Y(a,b,c) b(b##_)
						MACROLIST_LivSimGrazingTable_ATTRIBUTES
		#undef X
		#undef Y
				{}
		//Stream constructor
		MpmasLuciaGrazingOpportunity(istream& is, char fieldsep, char linesep) {
				deserializeFields(is, fieldsep, linesep);
		}
				//getters and setters
		#define X(a,b,c)  a get_ ## b () {return b;} \
								  void set_ ## b (a b##_) {b = b##_;}
		#define Y(a,b,c) a get_ ## b () {return b;} \
								  void set_ ## b (a b##_) {b = b##_;}
		MACROLIST_LivSimGrazingTable_ATTRIBUTES
		#undef X
		#undef Y

		void serializeFields(ostream & os, char fieldsep, char linesep)
		{
			#define X(a,b,c) os << b << fieldsep;
			#define Y(a,b,c) os << b << linesep;
					MACROLIST_LivSimGrazingTable_ATTRIBUTES
			#undef X
			#undef Y
		}
		void deserializeFields(istream & is, char fieldsep, char linesep)
		{


			#define X(a,b,c) \
				MpmasTcpConnectionAuxiliary::getUntilDelimiterAndConvert(is, b, fieldsep);  \
				MpmasLuciaLivSim::stream_column_error_check(is, "grazing opportunities", string(#b));
			#define Y(a,b,c) MpmasTcpConnectionAuxiliary::getUntilDelimiterAndConvert(is, b, linesep); \
				MpmasLuciaLivSim::stream_column_error_check(is, "grazing opportunities", string(#b));

			//use macros for list
			MACROLIST_LivSimGrazingTable_ATTRIBUTES

			//undefine macros
			#undef X
			#undef Y
		}

};

class LivSimGrazingTable : public MpmasTcpConnectionSendableTable<LivSimGrazingTable>
{
	protected:
		typedef vector<MpmasLuciaGrazingOpportunity> typeOfGrazingList;
		typeOfGrazingList GrazingList;

	public:

		LivSimGrazingTable( ) 	: GrazingList()	{ }
		virtual ~LivSimGrazingTable(){ }

		void addGrazingOpportunity(MpmasLuciaGrazingOpportunity newOpp) { GrazingList.push_back(newOpp);}

		virtual void clear()
		{
				GrazingList.clear();
		}

		public:

		//functions for serialization with  MpmasTcpConnectionSendableTable
		string getTableName() { return string("LivSimGrazingTable"); };

		string getColName(int i)
		{
			switch(i) {
				#define X(a,b,c) case c : return string(#b) ; break;
				#define Y(a,b,c) case c : return string(#b) ; break;
				MACROLIST_LivSimGrazingTable_ATTRIBUTES
				#undef X
				#undef Y
				default :
					throw runtime_error("Invalid column name request in LivSimGrazingTable\n");

			}
			return string();
		}

		int getNumberOfColumns()
		{		//number of columns is supposed to be the index of the last element + 1
				#define X(a,b,c)
				#define Y(a,b,c) return c + 1;
					MACROLIST_LivSimGrazingTable_ATTRIBUTES
				#undef X
				#undef Y
		}
		void serializeFields(ostream & os, char fieldsep, char linesep)
		{
			for (typeOfGrazingList::iterator it = GrazingList.begin(); it != GrazingList.end(); ++it)
			{
				it->serializeFields(os, fieldsep, linesep);
			}
		}
		void deserializeFields(istream & is, char fieldsep, char linesep)
		{
			clear();
			while (is.peek() != EOF)
			{
				GrazingList.push_back(MpmasLuciaGrazingOpportunity(is, fieldsep, linesep));
			}
		}

		//accessors
		size_t getListSize() { return GrazingList.size(); }
		MpmasLuciaGrazingOpportunity getElement(int index) { return GrazingList.at(index); }
};

class MpmasLivSimFeedingPlan {

		//X macro pattern
			private:
		//defining the columns in the macrolist as class members
		#define X(a,b,c) a b;
		#define Y(a,b,c) a b;
			MACROLIST_LivSimFeedingTable_ATTRIBUTES
		#undef X
		#undef Y

			public:
				//Default constructor - just taking all arguments in the order as listed in the macro and initialize them with 0
			MpmasLivSimFeedingPlan( )
				: //initializer list
		#define X(a,b,c) b(0),
		#define Y(a,b,c) b(0)
					MACROLIST_LivSimFeedingTable_ATTRIBUTES
		#undef X
		#undef Y
				{}

		//Standard constructor - just taking all arguments in the order as listed in the macro and initialize them corresponding attributes
			MpmasLivSimFeedingPlan( //constructor arguments
		#define X(a,b,c) a b##_,
		#define Y(a,b,c) a b##_
					MACROLIST_LivSimFeedingTable_ATTRIBUTES
		#undef X
		#undef Y
					) : //initializer list
		#define X(a,b,c) b(b##_),
		#define Y(a,b,c) b(b##_)
					MACROLIST_LivSimFeedingTable_ATTRIBUTES
		#undef X
		#undef Y
				{}
		//vector constructor
			MpmasLivSimFeedingPlan(
							int herd_id_,
							//int feeding_group_id_,
						   int feed_type_,
						   int feed_id_,
						   vector<double> quantities
					) : herd_id(herd_id_), /*feeding_group_id(feeding_group_id_),*/ feed_type(feed_type_), feed_id(feed_id_)
	{
				if (quantities.size() != 12)
					throw runtime_error ("Error: invalid number of months in MpmasLivSimFeedingPlan constructor\n");

				jan = quantities[0];
				feb = quantities[1];
				mar = quantities[2];
				apr = quantities[3];
				may = quantities[4];
				jun = quantities[5];
				jul = quantities[6];
				aug = quantities[7];
				sep = quantities[8];
				oct = quantities[9];
				nov = quantities[10];
				dec = quantities[11];
	}
		//Stream constructor
			MpmasLivSimFeedingPlan(istream& is, char fieldsep, char linesep) {
				deserializeFields(is, fieldsep, linesep);
		}
				//getters and setters
		#define X(a,b,c)  a get_ ## b () {return b;} \
								  void set_ ## b (a b##_) {b = b##_;}
		#define Y(a,b,c) a get_ ## b () {return b;} \
								  void set_ ## b (a b##_) {b = b##_;}
						MACROLIST_LivSimFeedingTable_ATTRIBUTES
		#undef X
		#undef Y

		void serializeFields(ostream & os, char fieldsep, char linesep)
		{
			#define X(a,b,c) os << b << fieldsep;
			#define Y(a,b,c) os << b << linesep;
							MACROLIST_LivSimFeedingTable_ATTRIBUTES
			#undef X
			#undef Y
		}
		void deserializeFields(istream & is, char fieldsep, char linesep)
		{


			#define X(a,b,c) \
				MpmasTcpConnectionAuxiliary::getUntilDelimiterAndConvert(is, b, fieldsep);  \
				MpmasLuciaLivSim::stream_column_error_check(is, "feeding plan", string(#b));
			#define Y(a,b,c) MpmasTcpConnectionAuxiliary::getUntilDelimiterAndConvert(is, b, linesep); ;\
				MpmasLuciaLivSim::stream_column_error_check(is, "feeding plan", string(#b));

			//use macros for list
			MACROLIST_LivSimFeedingTable_ATTRIBUTES

			//undefine macros
			#undef X
			#undef Y
		}

};
class LivSimFeedingTable : public MpmasTcpConnectionSendableTable<LivSimFeedingTable>
{
	protected:
		typedef vector<MpmasLivSimFeedingPlan> typeOfFeedingList;
		typeOfFeedingList FeedingList;

	public:

		LivSimFeedingTable( ) 	: FeedingList()	{ }
		virtual ~LivSimFeedingTable(){ }

		void addFeedingPlan(MpmasLivSimFeedingPlan newPlan) { FeedingList.push_back(newPlan);}

		virtual void clear()
		{
			FeedingList.clear();
		}

		//functions for serialization with  MpmasTcpConnectionSendableTable
		string getTableName() { return string("LivSimFeedingTable"); };

		string getColName(int i)
		{
			switch(i) {
				#define X(a,b,c) case c : return string(#b) ; break;
				#define Y(a,b,c) case c : return string(#b) ; break;
				MACROLIST_LivSimFeedingTable_ATTRIBUTES
				#undef X
				#undef Y
				default :
					throw runtime_error("Invalid column name request in LivSimFeedingTable\n");

			}
			return string();
		}

		int getNumberOfColumns()
		{		//number of columns is supposed to be the index of the last element + 1
				#define X(a,b,c)
				#define Y(a,b,c) return c + 1;
					MACROLIST_LivSimFeedingTable_ATTRIBUTES
				#undef X
				#undef Y
		}
		void serializeFields(ostream & os, char fieldsep, char linesep)
		{
			for (typeOfFeedingList::iterator it = FeedingList.begin(); it != FeedingList.end(); ++it)
			{
				it->serializeFields(os, fieldsep, linesep);
			}
		}
		void deserializeFields(istream & is, char fieldsep, char linesep)
		{
			clear();
			while (is.peek() != EOF)
			{
				FeedingList.push_back(MpmasLivSimFeedingPlan(is, fieldsep, linesep));
			}
		}

};


struct LivSimAssetMatchingClass {
		string  breed;
	   int sex;
	   int lower_age;
	   int upper_age;
	   int lower_bw;
	   int upper_bw;
	   int gestating;
	   int lactating;



	   LivSimAssetMatchingClass()
	   	:	breed(""), sex(-1),lower_age(-1), upper_age(-1), lower_bw(-1), upper_bw(-1), gestating(-1), lactating(-1)
	   {}

	   LivSimAssetMatchingClass(string breed_, int sex_, int ageL, int ageU, int bwL, int bwU, int g_, int l_)
	   	:	breed(breed_), sex(sex_),lower_age(ageL), upper_age(ageU), lower_bw(bwL), upper_bw(bwU), gestating(g_), lactating(l_)
	   {}
	   void serializeToStream(ostream& out, char sep = '\t')
	   {
	   	out << breed << sep << sex << sep << lower_age << sep << upper_age << sep << lower_bw << sep << upper_bw
	   			<< sep << gestating << sep << lactating << "\n";
	   }

};

struct LivSimAssetMatchingClassComparator {
		bool operator() (const LivSimAssetMatchingClass& lhs, const LivSimAssetMatchingClass& rhs )
		{
			if (lhs.breed < rhs.breed)
				return true;
			if (lhs.breed > rhs.breed)
				return false;

			//if breed is equal:
			if (lhs.sex < rhs.sex)
					return true;
			if (lhs.sex > rhs.sex)
					return false;


			// if overlap, then they are considered equal
		   if (lhs.upper_age < rhs.lower_age)
		   {
		   	return true;
		   }
		   if (lhs.lower_age > rhs.upper_age)
			{
				return false;
			}

			// if overlap, then they are considered equal
		   if (lhs.upper_bw < rhs.lower_bw)
		   	return true;
		   if (lhs.lower_bw > rhs.upper_bw)
				return false;

		   if (lhs.gestating != -1 && rhs.gestating != -1)
		   {
		   	if (lhs.gestating < rhs.gestating)
					return true;
				if (lhs.gestating > rhs.gestating)
					return false;
		   }

		   if (lhs.lactating != -1 && rhs.lactating != -1)
		   {
		   	if (lhs.lactating < rhs.lactating)
					return true;
				if (lhs.lactating > rhs.lactating)
					return false;
		   }
		   return false;

		}
};

typedef map<LivSimAssetMatchingClass,int,LivSimAssetMatchingClassComparator> LivSimToMpmasMap;
typedef map<int,vector<LivSimAssetMatchingClass> > MpmasToLivSimMap;
typedef map<int,vector<int> > MpmasLuciaPastureIdList;


class MpmasLivSimTransformationInfo
{
	private:
		LivSimHerdManagementTable herdManagementTable;
		MpmasToLivSimMap matchAssetToLivSimAnimal;
		LivSimToMpmasMap matchLivSimAnimalToAsset;
		MpmasLuciaPastureIdList pastureAllowedCropManagementIDs;
		s_liste* parentMpmasNetwork;
		double age_age_factor;
		int precision_age;
		int precision_bw;

		int maxSeason;





	public:
	MpmasLivSimTransformationInfo(): herdManagementTable(), matchAssetToLivSimAnimal(),matchLivSimAnimalToAsset(), pastureAllowedCropManagementIDs(), parentMpmasNetwork(NULL), age_age_factor(1), precision_age(10), precision_bw(10), maxSeason(0)
	{};
	MpmasLivSimTransformationInfo(s_liste* parentNetwork): herdManagementTable(),matchAssetToLivSimAnimal(),matchLivSimAnimalToAsset(), pastureAllowedCropManagementIDs(),  parentMpmasNetwork(parentNetwork), age_age_factor(1), precision_age(10), precision_bw(10),maxSeason(0)
	{};
	~MpmasLivSimTransformationInfo(){};

	void setParentMpmasNetwork(s_liste* parentNetwork) { parentMpmasNetwork = parentNetwork; };

	void readInputFromFile(string filename);

	LivSimAssetMatchingClass getLivSimMatchForAsset(int assetObjId, int age, bool allowEmptyReturn = false);
	pair<int,int> getAssetMatchForLivSimAnimal(string breed_, int sex_, double age, double bwU, int gestating_, int lactating_);
	int ageConversionLivSimToMpmas( double livsimAge){ return int ((double) livsimAge * age_age_factor  + EPS);	}
	double ageConversionMpmasToLivSim( int mpmasAge) { return  mpmasAge / age_age_factor;	}

	int ageConversionLivSimToMatchClass( double livsimAge) {  return int( livsimAge * (double) precision_age + 0.5 + EPS);}
	double ageConversionMatchClassToLivSim( int matchClassAge) {  return  (double) matchClassAge / (double) precision_age ;}
	int bwConversionLivSimToMatchClass( double livsimBw) {  return int( livsimBw * (double) precision_bw + 0.5 + EPS);}
	double bwConversionMatchClassToLivSim( int matchClassBw) {  return  (double) matchClassBw / (double) precision_bw ;}

	vector<int> getPastureIdListForSeason(int season) { return  pastureAllowedCropManagementIDs[season]; }
	MpmasLuciaPastureIdList getPastureIdLists() { return  pastureAllowedCropManagementIDs; }

	int getMaxSeason(){return maxSeason;}

	LivSimHerdManagementTable getHerdManagementTable() {return herdManagementTable;}
};





class LivSimHerdCollection
{
	protected:
		int fstId;
		typedef vector<LivSimHerd*> typeOfHerdList;
		typeOfHerdList herds;
		typedef map<int,int> typeOfAnimalHerdMap;
		typeOfAnimalHerdMap herdOfAnimal;

	   friend class LivSimHerdCollectionIterator;

	public :

	   LivSimHerdCollection() : fstId(-1), herds(LIVSIM_SUBHERD_MAX) /*NULL initialization by default constructor, writing NULL explicitly gives 32bit errors though*/, herdOfAnimal() {herds[0] = new LivSimHerd(asLivSimHerdId(fstId,0));}

	   LivSimHerdCollection(int _id) : fstId(_id),  herds(LIVSIM_SUBHERD_MAX)/*NULL initialization by default constructor, writing NULL explicitly gives 32bit errors though*/, herdOfAnimal() { herds[0] = new LivSimHerd(asLivSimHerdId(fstId,0));}
	   ~LivSimHerdCollection()
	   {	for (int i = 0; i < LIVSIM_SUBHERD_MAX; ++i)
	   	{
	   		if (herds[i] != NULL)
	   			delete herds[i];

	   		herds[i] = NULL;
	   	}
 	 	 }
	   //
		void addAnimal(LivSimAnimalInfo newAnimal, int whichHerd) {
			if (whichHerd >= LIVSIM_SUBHERD_MAX)
			{
				stringstream errmsg;
				errmsg << "Error can't add animal " << newAnimal.get_id() << " to herd " << whichHerd << ". Herd index too high\n";
				throw runtime_error(errmsg.str());
			}
			if (herds[whichHerd] == NULL )
			{
				herds[whichHerd] = new LivSimHerd(asLivSimHerdId(fstId,whichHerd));
			}
			herds[whichHerd]->addAnimal(newAnimal);
			pair<typeOfAnimalHerdMap::iterator,bool> ret =  herdOfAnimal.insert(pair<int,int>(newAnimal.get_id(), whichHerd));
			if(ret.second == false)
			{
				stringstream errmsg;
				errmsg << "Error trying to add animal " << newAnimal.get_id() << " which already exists in herd\n";
				throw runtime_error(errmsg.str());
			}

		}

		void setAnimalRemovedById(int id) {
			typeOfAnimalHerdMap::iterator it = herdOfAnimal.find(id);
			if (it != herdOfAnimal.end())
			{
				herds.at(it->second)->setAnimalRemovedById(id);
				//herdOfAnimal.erase(id);
			}
			else
			{	stringstream errmsg;
				errmsg << "Error can't remove animal " << id << " of agent " << fstId << ". Animal not found in agent's herd\n";
				throw runtime_error(errmsg.str());
			}
		}
		LivSimAnimalInfo getAnimalById(int id)
		{
			typeOfAnimalHerdMap::iterator it = herdOfAnimal.find(id);
			if (it != herdOfAnimal.end())
			{
				return herds.at(it->second)->getAnimalById(id);
			}
			return LivSimAnimalInfo();
		}
		void clear() {
			for (int i = 0; i < LIVSIM_SUBHERD_MAX; ++i)
			{
				if (herds[i] != NULL)
					delete herds[i];

				herds[i] = NULL;
			}
			herds[0] = new LivSimHerd(asLivSimHerdId(fstId,0));
			//herds.clear();
		}
		int getFstId() { return fstId ;}

		void resetHerdGroups()
		{ //move all herd groups back to herd zero
			for (int i = 1; i < LIVSIM_SUBHERD_MAX; ++i)
			{
				if (herds[i] != NULL)
					herds[0]->adoptAnimalsFromHerd(herds[i]);
			}
			for (typeOfAnimalHerdMap::iterator it = herdOfAnimal.begin(); it != herdOfAnimal.end(); ++it)
			{
				it->second = 0;
			}
		}
		void moveAnimalToHerdGroup(int animalId, int whichHerd)
		{
			LivSimAnimalInfo newAnimal;
			typeOfAnimalHerdMap::iterator it = herdOfAnimal.find(animalId);
			if (it != herdOfAnimal.end())
			{
				newAnimal = herds.at(it->second)->takeOutAnimalById(animalId);

			}
			else
			{	stringstream errmsg;
				errmsg << "Error can't move animal " << animalId << " of agent " << fstId << " to (sub)herd  "<< whichHerd<< ". Animal not found in agent's whole herd\n";
				throw runtime_error(errmsg.str());
			}


			if (whichHerd >= LIVSIM_SUBHERD_MAX)
			{
				stringstream errmsg;
				errmsg << "Error can't add animal " << newAnimal.get_id() << " to herd " << whichHerd << ". Herd index too high\n";
				throw runtime_error(errmsg.str());
			}
			if (herds[whichHerd] == NULL )
			{
				herds[whichHerd] = new LivSimHerd(asLivSimHerdId(fstId,whichHerd));
			}
			herds[whichHerd]->addAnimal(newAnimal);

			herdOfAnimal[animalId] = whichHerd;

		}
		void addHerdsToTable(LivSimHerdTable& herds_table, int skip_zero)
		{

			for (int i = skip_zero ? 1 : 0; i< LIVSIM_SUBHERD_MAX; ++i)
			{
				if (herds[i] != NULL)
				{
					herds_table.addHerd( herds[i] );
				}
			}
		}
		size_t getNumberAnimalsInHerd(int whichHerd)
		{
			if (whichHerd >= LIVSIM_SUBHERD_MAX)
			{
				stringstream errmsg;
				errmsg << "Error can't get number of animals in herd " << whichHerd << ". Herd index too high\n";
				throw runtime_error(errmsg.str());
			}
			if (herds[whichHerd] == NULL )
			{
				return 0;
			}
			return herds[whichHerd]->getHerdSize();
		}
		size_t getNumberNonRemovedAnimalsInHerd(int whichHerd);
};

class LivSimHerdCollectionIterator
{
    LivSimHerdCollection* herdCollection_;
    vector<LivSimHerd*>::iterator internalIt   ;

public:
    LivSimHerdCollectionIterator(LivSimHerdCollection& herdC)
    : herdCollection_(&herdC), internalIt( herdCollection_->herds.begin() ) {}

    LivSimHerd* getHerd()
    {
        return *internalIt;
    }

    LivSimHerdCollectionIterator& operator ++ ()
    {
   	 if(internalIt == herdCollection_->herds.end())
   			 return *this;

   	 	 ++internalIt; //increase at least once

   	 while(internalIt != herdCollection_->herds.end()
   			 && *internalIt == NULL)
   		 ++internalIt; //increase while zero ()) LP is not guaranteed to set only the first herds)

   	 return *this;
    }

    bool operator != (const LivSimHerdCollectionIterator& i) const
    {
        assert(herdCollection_ == i.herdCollection_);
        return internalIt != i.internalIt;
    }
    bool reachedEnd ()
    {
   	 return (internalIt == herdCollection_->herds.end() );
    }
};

#endif // _MPMAS_LUCIA_LIVSIM_COUPLING_H_
