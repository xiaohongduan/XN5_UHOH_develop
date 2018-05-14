#ifndef _MPMAS_UTILITY_FUNCTIONS
#define _MPMAS_UTILITY_FUNCTIONS

#include "BasicDefines.h"
// LINUX / WINDOWS specific definitions
#ifdef AIX
   #include <unistd.h>
	#include <limits.h>

#else
	#include <dos.h>
#endif /* AIX */
#include <time.h>
#include <sstream>
#include "MainAuxiliaryFunctions.h"
#include <cstdio>
#include <cstring>
#include <string>

inline char* const2char(const char* exp)
{
	char* dummy;
	dummy = new char[50];
	int cnt = 0;
	while (*exp)
	{
		dummy[cnt] = *exp;
		++exp;
		cnt++;
	}
	dummy[cnt] = '\0';
	return dummy;
}

inline void const2char(const char* exp, char* out)
{
	int cnt = 0;
	while (*exp)
	{
		out[cnt] = *exp;
		++exp;
		cnt++;
	}
	out[cnt] = '\0';
}

string getOneToken(string& fn, string& rest);
string getOneToken(string& fn);
string getOnlyNums(string& fn);

#if (defined AIX || defined _MSC_VER << defined __MINGW32__)
	long getTimeDiffInSec(time_t t2, time_t t1);
#else
	double getTimeDiffInSecWIN(struct time* t2, struct time* t1); //Tbe Feb23
	void zeitdifferenz(struct time* t2, struct time* t1); //Tbe Feb23
#endif//AIX

inline void printIfOptimalSpeed()
{
#ifdef OPTIMALSPEED
	printf("  Compiler flag OPTIMALSPEED\n"
			"  (not checking matrix dimensions, may lead to segmentation error)\n\n");
#endif
}

inline void printIfTDT()
{
#ifdef DO_TDT
	printf("  Compiler flag DO_TDT \n"
		"\tnot checking matrix dimensions, may lead to segmentation error)\n");
#else
	printf("\t(Typed Transfer Library TDT inactive)\n");
#endif
}
void expectFileDelimiter(FILE* strm, string delim, string filename, string extraInfo);
string StringToUpper(string strToConvert);
string StringToLower(string strToConvert);
#endif
