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
//  File:       CatchmentSectorSpecific.h (formerly ssector9.h)
//
//  Contents:   Special class of EDIC sub-sector
//              
//
//
//------------------------------------------------------------------------------


//dies ist die header-Datei fuer einen abgeleiteten Bewaesserungssector

#ifndef _CatchmentSectorSpecific_h
#define _CatchmentSectorSpecific_h

#include "CropGrowthHandler.h"
#include "CatchmentSector.h"
class raster3D;


class ssector : public sector
{	protected:

	double A_2; //Flaechenanteil von Subsystemen nach Gleichung 2
	double A_5; //Flaechenanteil von Subsystemen nach Gleichung 5
	double S_5; //Summenwert von Subsystemen nach Gleichung 5
	double S_7; //Summenwert  von Subsystemen nach Gleichung 7


	public:
	ssector (const char* i, const char* o,
            int sc, scuenca* sub,
            int sk, 
            double a2, 
				double a5,
            double s5, 
				double s7,
            CropWater* p_cropWat_,
				CropPrecipitation* p_cropPrecipitation_,
				CropPotentialETr* p_cropPotentialETr_,
				CropFloodFactor* p_cropFloodFactor_) :
		 sector (i, o, sc, sub, sk, p_cropWat_, p_cropPrecipitation_, p_cropPotentialETr_, p_cropFloodFactor_)
	{	A_2 = a2;
		A_5 = a5;
		S_5 = s5;
		S_7 = s7;
	}

	virtual ~ssector(void)
	{
	}

	virtual double computeInternalReturnFlowCoefficients(void);

	//Ausgabe
	virtual void printToScreen(void);

   virtual double get_a2(void);
   virtual double get_a5(void);
   virtual double get_s5(void);
   virtual double get_s7(void);
};



#endif


