//
//  Thomas Berger (main author), Pepijn Schreinemachers, and Thorsten Arnold
//
//
//  Hohenheim University (490d)
//  Professorship for Land Use Economics in the Tropics and Subtropics
//
//  May not be distributed without permission by main author
//
//  File:       BasicDefines.h (formerly const_defs.h)
//
//  Contents:   Contains defines
//
//
//
//------------------------------------------------------------------------------
#ifndef _BasicDefines_h
#define _BasicDefines_h

/* TODO -cNote : Here description of compiler flags */
/* Compiler Flags:
AIX			Functions only for UNIX
USE_OSL		includes OSL solver into build
USE_SYM		includes SYMPHONY solver into build
USE_CBC		includes CBC solver into build
USE_CPLEX	includes CPLEX solver into build

LP9			old lp9 implementation, not working anymore (incompatible with USE_... flags for solvers )

PAR2			Parallelization using MPI
DO_TDT		include TDT communication into build (note: TDT lib has to be built with compiler flag WIN32)
MONICA		coupling with MONICA model
TEACH			trial model limited to 100 agents

MULTIPERIOD multiperiod version of MPMAS
IP_COUPLING MPMAS-LUCIA coupling via TCP/IP
LIVSIM_COUPLING Coupling with MPMAS-LUCIA-LivSim

MCHECK		use to compile mpmasMipSolve (stripped down version of mpmas base class)
MPS			???

NOT_NEWFORMAT14	unsets the NEWFORMAT14 flag for new input format while not complete
NOT_LPPACKED		unsets the LPPACKED flag for the use of CoinPackedMatrix to store matrix internally

INVBOUNDS   ???, OBSOLETE???
NRSLV       Uses NR-solver (not debugged), OBSOLETE?
SENSITIVITY ???, OBSOLETE???
OPTIMALSPEED ???
 */

//implicit compiler flags for using OSL
#if defined(_MSC_VER) && ( defined(USE_OSL) || defined(LP9) )
	#define OSLMSDLL //Functions only for _WIN32, needed to invoke OSL library
#endif

//version info
#define BUILT_DATE 180808
#define MAJOR_VERSION 3
#define MINOR_VERSION 3
#define BUILD_NUMBER 435
#define REVISION 2

#ifndef NOT_ODB
	#define ODB
#endif
#ifndef NOT_LPPACKED
	#define LPPACKED
#endif
#ifndef NOT_NEWFORMAT14
	#define NEWFORMAT14
	#define RNGSTREAM //activates RNGSTREAM random number generation
#endif



#ifdef MULTIPERIOD
#ifdef LP9
#error 'Compiler flags LP9 and MULTIPERIOD are incompatible'
#endif
#endif

//for BC 4.52 und cxx
#define true 1
#define false 0

#define _MAXCATCHMENTS_ 10
#define _MAXSECTORS_ 750
//#define _NUM_IRRIGMETHODS_ 7

// used to set inflows if no files available
#define fixInflows 1.0E6

//Rounding parameters that improve performance in solving (OSL)
#ifndef NK1
#define NK1 5        //number of digits in function round1()
#endif

#ifndef NK2
#define NK2 0.00001   //precision in function round2()
#endif

//Misc.
#define MXLENGTH 200	//maximum length of filename
#define MXSTRINGFORMAT "%190s"// for ensuring the string read from file into char[MXLENGTH] does not exceed capacity // Troost 150123
#define MIN_HA 0.1	//minimum amount (ha) when investing in perm. crops
#define OBJNAME 20   //maximum length of name of investment object
#define EPS 1.0e-6   //precision, OSL seems to work with 1.0e-7 (see "ekk_c_api.h")
#define MAX_MEMBER_AGE 1000 //maximum age of household members
#define MAX_NUMBER_PRIORITIES 10 //priorities for spatial allocation (reflects closeness to farmstead)

#define NOT_RENTED_OUT_FLAG -1
#define RETURN_PLOT_FLAG -2

#define _NODATADEF_ -1   //default value "no data"
#define _SEED_DEFAULT_ -2502   //default seed value
#define STAGES 20  //default number of stages in lottery

#ifdef AIX
   #define _SLASH_ "/"
#else
    #define _SLASH_ "\\"
#endif /* AIX */


///some country-specific defines needed for lottery
//Ghana
#define GHANA_NONIRRIGABLE_PLOT	0

//Chile
#define CHILE_SOIL_TYPE_FRUITS 0 //-> PopulationClusterSpecific
#define PEAK_LABOR_FRUIT 0.75
#define PEAK_LABOR_BERRY 1.5
#define MIN_HECTARE_T2 5.0
#define UPPER_BOUND_T2 50000.0
#define CULTIVO_ENTRIES_BEFORE_LABOR 3 //->PerennialCropsHandler
//->AgentFarmHousehold
#define AGENTF_ENTRIES_BEFORE_LABOR 13 //changed from 8 to 11, quick fix ## 080812, then to 13 ## Tbe 121222
#define AGENTF_ENTRIES_BEFORE_PER_HA 5 //quick fix ## 080812
#define STATUS_ADDITIONAL 1


#endif
