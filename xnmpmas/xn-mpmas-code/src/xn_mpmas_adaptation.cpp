#include "xn_mpmas_adaptation.h"
#include <exception>
#include <stdexcept>
#include <iostream>
#include <fstream>

using namespace std;

namespace xnmpmas
{ namespace adaptation 
{
// 1) Auxiliary functions (not shared, not declared in header)

// Berechnet den Tag im Jahr, bei dem zum ersten mal der Mittelwert der Bodentemperatur (5 cm) des Tages und 6 darauffolgender Tage einen Grenzwert überschreitet.
// BenÃ¶tigt:
// Liste mit Bodentemperaturen (°C), sortiert vom Anfang des Jahres (soiltemp[0] = 1. Januar) 
// Schwellenwert
// Rückgabe: kleinster Tag im Jahr, der die Bedingung erfüllt
// BeschrÃ¤nkung auf 180 Tage im Jahr dient der Performance und könnte entfallen
// Evtl. Einfangen von Extremjahren oder DatenlÃ¼cken erforderlich (sollte aber nicht vorkommen)

	int calculateDaySoilTgz(const double* soiltemp, const int nSoiltemp, const double threshold, const int maxDays = 180) 
	{
		if (nSoiltemp < maxDays + 6)
			throw runtime_error(string("Error in <calculateDaySoilTgz>: too few days on record.\n"));
				
		double tgz = 0;
		double curdoy = 1;
		int doy = 1;

		while (tgz < threshold && doy <= maxDays) { 
			curdoy =  doy;
			tgz = 0;
			for (int offs = 0; offs <= 6; offs++) {
				tgz += soiltemp[doy + offs - 1] / 7 ; 
			} 
			doy++;
		}
		return curdoy;
	}


	// Berechnet den Tag im Jahr, bei dem zum letzten Mal eine Temperatursumme (1. Jan bis dato) nicht Ã¼berschritten wurde.
	// BenÃ¶tigt:
	// Liste mit Lufttemperaturen(Â°C), airtemp[0] -> 1. Januar
	// Schwellenwert (Â°C * tag)
	// Basistemperatur (z.B 5Â°C)
	// Ruckgabe: Tag im Jahr, der die Bedingung erfÃ¼llt (1 -> 1. Januar)
	int calculateDayGDDreached(const double* airtemp, const int nAirtemp, const double threshold, const double basetemp, const int maxDays = 180) 
	{
		if (nAirtemp < maxDays )
			throw runtime_error(string("Error in <calculateDayGDDreached>: too few days on record.\n"));
				
		double gdd = 0;
		int doy = 0;

		while (gdd <= threshold && doy <= maxDays) {
		 gdd += ( (airtemp[doy] > basetemp) ? airtemp[doy] : basetemp) - basetemp ;
		 doy++; 
		}
		return doy;
	}


	// Berechnet den spÃ¤testen Tag im Jahr, bei dem die Temperatursumme von dato bis Jahresende einen bestimmten Wert noch erreicht.
	// BenÃ¶tigt:
	// Liste mit Lufttemperaturen(°C), airtemp[0] -> 1. Januar
	// Schwellenwert (°C * tag)
	// Basistemperatur (z.B 5°C)
	// Ruckgabe: Tag im Jahr, der die Bedingung erfüllt (1 -> 1. Januar)
	int calculateDayRemGDD(const double* airtemp, const int nAirtemp, const double threshold, const double basetemp, const int startDay = 180) 
	{
		if (nAirtemp < 365 )
				throw runtime_error(string("Error in <calculateDayRemGDD>: too few days on record.\n"));
		
		double gdd = 0;
		int doy = nAirtemp-1; 
		while (gdd <= threshold && doy > startDay) {
		 gdd += ((airtemp[doy] > basetemp) ? airtemp[doy] : basetemp) - basetemp ;
		 doy--; 
		}
		return doy+2;
	}

// 2) Main functions (declared in header)
	int calculatePlantingDate(const cropAdaptationParameters* params, const deque<weatherRecord> cellHistory, const vector<double> historyWeighting)
	{
		int sowDoy = 0;
		int day_soil_tgz = 0;
		int day_air_gdd	 = 0;
		double expectedPlantdate = 0.0;
		
		int ws = 1; //set zero if season does not span over two years
	 
		switch (params->sowingDayFormula)
		{
			case springSoilGdd:
				for (unsigned i = 0; i < historyWeighting.size(); ++i) 
				{
					day_soil_tgz += calculateDaySoilTgz(cellHistory[i].topsoilTemp, cellHistory[i].numDays, params->seed_tgzthr) 				 * historyWeighting[i]; 
					day_air_gdd  += calculateDayGDDreached(cellHistory[i].airTemp, cellHistory[i].numDays, params->seed_gddthr, params->basetemp) * historyWeighting[i]; 
				}
				expectedPlantdate = params->seed_cons  + params->seed_coef_tgz * day_soil_tgz + params->seed_coef_gdd * day_air_gdd;
			   break;

			case springSoil:
				for (unsigned i = 0; i < historyWeighting.size(); ++i) 
				{
					day_soil_tgz += calculateDaySoilTgz(cellHistory[i].topsoilTemp, cellHistory[i].numDays, params->seed_tgzthr) * historyWeighting[i]; 
				}
				expectedPlantdate = params->seed_cons  + params->seed_coef_tgz * day_soil_tgz; 
			  break;

			case springGdd:
				for (unsigned i = 0; i < historyWeighting.size(); ++i) 
				{
					day_air_gdd += calculateDayGDDreached(cellHistory[i].airTemp, cellHistory[i].numDays,params->seed_gddthr, params->basetemp) * historyWeighting[i]; 
				}
				expectedPlantdate = params->seed_cons  +  params->seed_coef_gdd * day_air_gdd;
			  break;

			case winterGdd:
				for (unsigned i = 0; i < historyWeighting.size(); ++i) 
				{	//i + ws here, because autumn values are likely not yet available in the newest record
					day_air_gdd += calculateDayRemGDD(cellHistory[i + ws].airTemp, cellHistory[i + ws].numDays, params->seed_gddthr, params->basetemp) * historyWeighting[i]; 
				}
				expectedPlantdate = params->seed_cons  + params->seed_coef_gdd * day_air_gdd;
				//cout << "WinterGdd, cons: " << params->seed_cons << ", coef: " << params->seed_coef_gdd << ", day_air_gdd: " << day_air_gdd << endl;
			   break;
			   
			case notAdaptive:
				throw invalid_argument(string ("Error: Sowing->Adaptive->Type: notAdaptive(-1) passed to function <xnmpmas::adaptation::calculatePlantingDate>\n") );
				break;
				
			default:
				throw invalid_argument(string ("Error: Sowing->Adaptive->Type: invalid type passed to function <xnmpmas::adaptation::calculatePlantingDate>\n") );
		}
		
		return sowDoy = (int) expectedPlantdate;
	}
	 
	void calculateFertilizerQuantity (const cropAdaptationParameters* params, const double expectedYield,  STRUCT_mpmas_to_xn& adaptedManagement)
	{
		double newTotal = expectedYield * params->fert_Ncontent + params->fert_leach;
				
	//preliminary here, needs to be rechecked and more sophisticated. For now just proportional scaling of
	// original applications
		
		double oldTotal = 0;
		
		for (int i = 0; i < XNMPMASMINFERTSLOTS;++i) 
		{
			oldTotal += adaptedManagement.mineralFertilization[i].no3n 
						+ adaptedManagement.mineralFertilization[i].nh4n 
						+ adaptedManagement.mineralFertilization[i].urea;
		}
		if (oldTotal <= 0 ) 
			return;
			
		double coef = newTotal / oldTotal;
		
		for (int i = 0; i < XNMPMASMINFERTSLOTS;++i) 
		{
			adaptedManagement.mineralFertilization[i].no3n *= coef;
			adaptedManagement.mineralFertilization[i].nh4n *= coef; 
			adaptedManagement.mineralFertilization[i].urea *= coef;
		}
	}
	int isSpringCrop (const sowingDayFormulaType type)
	{
		switch(type)
		{
			case springGdd:
			case springSoil:
			case springSoilGdd:
				return true;
				break;
			case winterGdd:
				return false;
				break;
			default:
				throw invalid_argument("Error: invalid argument passed to xnmpmas:.adaptation::isSpringCrop()");
		}
	}
}//end namespace adaptation
}//end namespace xnmpmas