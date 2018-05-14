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
//  File:       OutputDataHandler.h (formerly ausgabe.h)
//
//  Contents:   Collects information from model components and writes to files
//
//
//
//------------------------------------------------------------------------------


#ifndef _OutputDataHandler_h
#define _OutputDataHandler_h

#include <cstdio>
#include "BasicDefines.h"


class ausgabe
{	protected:
#ifndef MULTIPERIOD
	FILE* aStrm_xp;
	FILE* aStrm_xu;
	FILE* aStrm_xk;
#endif
	FILE* aStrm_y;				
	
	FILE* aStrm_w;     // Yearly Water File
   FILE* aStrm_wAg;   // Yearly Water Agent File
   FILE* aStrm_Lrm;   // Yearly Land rental file


   public:
	ausgabe();			
	virtual ~ausgabe();
   //virtual void deallocate(); ## Tbe 130103 not needed

	virtual void data_period0();

	virtual void output_periode();
	virtual void outputFinal();	

	virtual void aufgabedaten();  
	virtual void writeDiffusion();

   virtual void openWater();
   virtual void closeWater();
      
  	virtual void openPachtLand();  
   virtual void closePachtLand(); 
	
	virtual void writeAgLndDataInStream();
#ifndef MULTIPERIOD
   virtual void writeAgPfmDataInStream();

	virtual void prepareStreamsExitData();
	virtual void closeStreamsExitData();
#endif
	virtual void prepareStreamYieldData();
	virtual void closeStreamYieldData();
#ifndef MULTIPERIOD
	virtual FILE* get_stream_xp();        
	virtual FILE* get_stream_xu();
	virtual FILE* get_stream_xk();
#endif
   virtual FILE* get_stream_y();

   virtual FILE* get_stream_w();       
   virtual FILE* get_stream_wAg();     
   virtual FILE* get_stream_Lrm();     
};

#endif


