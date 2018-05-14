/*
 * nomas_utilities.h
 *
 *  Created on: Dec 17, 2015
 *      Author: troost
 */

#ifndef NOMAS_UTILITIES_H_
#define NOMAS_UTILITIES_H_

#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdio>

#ifdef NOMAS
	namespace mpmasScope
	{
		inline void exitfun() { exit(1);}
	}

//	#define EPS  1.0E-6

	void fehlerabbruch();      //writes error message and exits program
	void writeToErrorFile(char* fehlertxt_); // Writes semething into err file
	extern char fehlertxt[];
	inline void getReturnIf99(int x) {/*do nothing*/	}
	inline void getReturnIf99() {/*do nothing*/	}


	inline int TestFun(int s_){	return 0;}

	inline void setNUM_VARIABLES(int i) { }// void function

	inline void returnFscan(int retVal, char* strVar)
	{
		std::string e = "Error when reading ";
		e.append(strVar);
		e.append(": ");
		switch (retVal)
		{
			case EOF:
				e.append("EOF\n");
				throw e;
				break;
			case 0:
				e.append("unexpected content\n");
				throw e;
				break;
			default:
				break;
		}
	}

	bool file_exists(std::string filename, bool do_output);
	bool file_exists(std::string filename);


#endif



#endif /* NOMAS_UTILITIES_H_ */
