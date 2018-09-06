/*
 * MpmasTcpConnector.h
 *
 *  Created on: Dec 16, 2015
 *      Author: Christian Troost
 *      			 Universität Hohenheim
 */

#ifndef MPMASTCPCONNECTOR_H_
#define MPMASTCPCONNECTOR_H_

#include <string>
#include <sstream>
#include <iostream>
#include <exception>
#include <stdexcept>

#include "Raster2D.h"

//#define MPMAS_TCP_DEBUG

#define LUCIA_COM_WITH_BOOST

#ifdef LUCIA_COM_WITH_BOOST
	#include <boost/asio.hpp>

	using boost::asio::ip::tcp;
#endif


using namespace std;

/*inline std::istream& safeGetline(std::istream& is, std::string& t)
{
    t.clear();

    // The characters in the stream are read one-by-one using a std::streambuf.
    // That is faster than reading them one-by-one using the std::istream.
    // Code that uses streambuf this way must be guarded by a sentry object.
    // The sentry object performs various tasks,
    // such as thread synchronization and updating the stream state.

    std::istream::sentry se(is, true);
    std::streambuf* sb = is.rdbuf();

    for(;;) {
        int c = sb->sbumpc();
        switch (c) {
        case '\n':
            return is;
        case '\r':
            if(sb->sgetc() == '\n')
                sb->sbumpc();
            return is;
        case EOF:
            // Also handle the case when the last line has no line ending
            if(t.empty())
                is.setstate(std::ios::eofbit);
            return is;
        default:
            t += (char)c;
        }
    }
}*/
namespace MpmasTcpConnectionAuxiliary {
	inline std::istream& getUntilDelimiterAndConvert(std::istream& is, int& out, char fieldsep )
	{
		is >> out;
		is.ignore( numeric_limits<streamsize>::max(), fieldsep);
		return is;
	}
	inline std::istream& getUntilDelimiterAndConvert(std::istream& is, double& out, char fieldsep )
	{
		is >> out;
		is.ignore( numeric_limits<streamsize>::max(), fieldsep);
		return is;
	}
	inline std::istream& getUntilDelimiterAndConvert(std::istream& is, string& out, char fieldsep )
	{
		getline(is, out, fieldsep);
		return is;
	}
}


template <class Derived> class MpmasTcpConnectionSendableTable
{  // abstract class to be inherited from if class should be send via sendTable/receiveTable
	// uses static polymorphism using the Curiously Recurring Template Pattern (CRTP)
   /* member functions to be implemented by derived class
			getColName(col)
			getTableName()
			getNumberOfColumns()
			serializeFields(ostream)
			deserializeFields(istream)
	*/
	public:

		void serializeTableToStream (ostream& os, char fieldsep, char linesep)
		{
			int cols = static_cast<Derived*>(this)->getNumberOfColumns();
			for (int i = 0; i < cols - 1; ++i)
			{
				os << static_cast<Derived*>(this)->getColName(i) << fieldsep;
			}
			os << static_cast<Derived*>(this)->getColName(cols -1) << linesep;

			static_cast<Derived*>(this)->serializeFields(os, fieldsep, linesep);
		}
		void deserializeTableFromStream (istream& is, char fieldsep, char linesep)
		{
			//is.exceptions ( std::ifstream::failbit | std::ifstream::badbit );
			int cols = static_cast<Derived*>(this)->getNumberOfColumns();

			for (int i = 0; i < cols; ++i)
			{
				string temp;
				if (i == cols -1)
					getline(is, temp, linesep);
				else
					getline(is, temp, fieldsep);

				if (is.eof())
				{
					stringstream errmsg;
					errmsg << "Error when receiving table " << static_cast<Derived*>(this)->getTableName()
							 << ": Not enough columns in header.\nReached EOF at column " << i << "[C counting]\n";
					throw runtime_error(errmsg.str());
				}

				if (is.fail() || is.bad())
				{
					stringstream errmsg;
					errmsg << "Error when receiving table " << static_cast<Derived*>(this)->getTableName()
							 << ": Error when trying to read column name for col " << i << "[C counting]\n";
					throw runtime_error(errmsg.str());
				}

				if( temp != static_cast<Derived*>(this)->getColName(i))
				{
					stringstream errmsg;
					errmsg << "Error when receiving table " << static_cast<Derived*>(this)->getTableName()
							 << ": wrong column name.\nExpected: '" << static_cast<Derived*>(this)->getColName(i)
							 <<"'\nReceived: '" << temp << "'\n";
					throw runtime_error(errmsg.str());
				}
				//cout << "Read colname :" << temp << endl;
			}

			static_cast<Derived*>(this)->deserializeFields(is, fieldsep, linesep);

		}



		void serializeTableToStream (ostream& os) { serializeTableToStream(os, ';', '\n' );}
		void deserializeTableFromStream (istream& is) {deserializeTableFromStream(is, ';', '\n' );}

		void serializeTableToStream (ostream& os, char fieldsep) { serializeTableToStream(os, fieldsep, '\n' );}
		void deserializeTableFromStream (istream& is, char fieldsep) {deserializeTableFromStream(is, fieldsep, '\n' );}

		void printToScreen() { serializeTableToStream(cout);}
		void writeToFile(string filename) {
				ofstream s (filename.c_str());
				if (!(s.is_open()))
				{
					stringstream errmsg;
					errmsg << "Error when opening " << filename <<
							  "\n";
					throw runtime_error(errmsg.str());
				}
				serializeTableToStream(s);
				s.close();
		}

		virtual ~MpmasTcpConnectionSendableTable(){}
};


class MpmasTcpConnection
{
	public:
		void waitForConnection();
		void closeConnection();

		void sendMessage(string message, bool null_terminated = 1);
		string waitForMessage();
		string waitForMessageOld();

		void expectMessage (string expectedMessage);


		void sendAsciiMap(Raster2D raster);
		Raster2D receiveAsciiMap();


		//Note: T must implement serializeTableToStream and deserializeTableFromStream,
		//possibly by inheritance from MpmasTcpConnectionSendableTable
		template <class T> void sendTable( T& tab );
		template <class T> void receiveTable( T& tab );




		MpmasTcpConnection(string host_, int port_, int serv_ = 0);
		virtual ~MpmasTcpConnection();

	protected:
		string hostString;
		int port;
		int server;
		int socketIn;
		int connectionId;

	   boost::asio::streambuf accumulatedResponse;


		void setupTcpServer();
		void shutDownServer();
		void connectToServer();
		//string waitForMessageWithKnownEnd(string delimiter);



#ifdef LUCIA_COM_WITH_BOOST
	   boost::asio::io_service obligatory_io_service;
		tcp::socket mySocket;
	   tcp::acceptor acceptorObject;

#endif

};

template <class T> void MpmasTcpConnection::sendTable( T& tab )
{

	sendMessage(string("TABLESTART"));
	expectMessage(string("TABLESTART OK"));
	ostringstream os;
	tab.serializeTableToStream( os, ';');
#ifdef MPMAS_TCP_DEBUG
	cout << "Sending table:" << os.str() << endl;
#endif
	sendMessage(os.str());
	expectMessage(string("TABLEEND OK"));
}
template <class T> void MpmasTcpConnection::receiveTable(T& tab)
{

	expectMessage(string("TABLESTART"));
	sendMessage(string("TABLESTART OK"));
	string content = waitForMessage();

#ifdef MPMAS_TCP_DEBUG
	cout << "Table reception: Raw message received:\n" << content <<"\n\n";
#endif
	stringstream is (content);

	try {
		tab.deserializeTableFromStream(is, ';'); //if this fails it should throw an exception

		sendMessage(string("TABLEEND OK"));
	}
	catch (exception& e)
	{
		sendMessage(string("TABLE FAILED"));
		throw runtime_error(e.what());
	}

}


#endif /* MPMASTCPCONNECTOR_H_ */
