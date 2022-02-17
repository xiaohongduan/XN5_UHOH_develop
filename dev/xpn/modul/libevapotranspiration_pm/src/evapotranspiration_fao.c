//      evapotranspiration_fao.c
//      
//      Copyright 2013 Florian Heinlein <florian.heinlein@inbox.lt>
//  


#include "evapotranspiration.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "evapotranspiration_reg.h"

int evapotranspiration_FAO_load(evapotranspiration *self)
{
    expertn_modul_base *xpn = &(self->parent);
    
    self->counter = 0; //AS: used to check how many timesteps are used for daily averaging
	self->ETpot_Day = 0.0;
	self->ETpot_Day_=0.0;
	self->ETpot_Year = 0.0;

	self->Epot_Day = 0.0;
	self->Epot_Year = 0.0;	
	self->fcumtrad = 0.0;
	self->fcumaero = 0.0;
    
    //AS: added information about weather data resolution
    char *S;
	S = xpn_register_var_get_pointer(xpn->pXSys->var_list,"Config.Expert N Standard Read INI.use high resolution climate data");
	if ((S==NULL) || (atoi(S)==0))
		{self->high_res_weather=0;} 
    else 
        {self->high_res_weather=1;}

	
	return RET_SUCCESS;
}


int evapotranspiration_FAO_run(evapotranspiration *self)
{
	expertn_modul_base *xpn = &(self->parent);

	PWEATHER  		pWE = xpn->pCl->pWeather;
	PWATER			pWa = xpn->pWa;
	PEVAP     		pEV = xpn->pWa->pEvap;
	PTIME			pTi = xpn->pTi;
	PPLANT			pPl = xpn->pPl;

	double	fP, flamda, fgamma, fra, frc, fgammastern;
	double	fea_Tmax, fea_Tmin, fea, fed, fea_Tave;
	double   fDelta, fJ, fdr, fdelta, fphi, ftemp, fomegas;
	double	fRa, fN, fn, fRns, fRnl, fRn, fG;
	double   fETaero, fETrad;
	double   fas=(double)0.25, fbs=(double)0.5;
	double	EvTeiler;
	double DeltaT;

	DeltaT = pTi->pTimeStep->fAct;

	//SG20220217: Unterscheidung Wetter-Tageswerte vs. hochaufgelöst
    if(self->high_res_weather==1)
        evapotranspiration_integrate_small_time_step_vars(self);
    else
    {
  //  if(...){ //sollte immer gemacht werden, wenn Wetter-Tageswerte verwendet werden! -- wie abfragen???
  //     if(pWE->fGlobalStrahlung > 0.001)
  //      { 
 
       self->weather.fWindSpeed = pWE->fWindSpeed;
        self->weather.fTempMax = pWE->fTempMax;
        self->weather.fTempMin = pWE->fTempMin;
     //self->weather.fTempAve = pWE->fTempAve;
        self->weather.fTempAve = (pWE->fTempMax+pWE->fTempMin)/2.0;//SG20220207: für Vergleich mit XN3 
        self->weather.fDaylySolRad = pWE->fGlobalStrahlung;
        self->weather.fDailyHumidity =  pWE->fHumidity;
        }

	if(SimStart(pTi))
		self->fTempYesterday = (double)-99;

/*	//AS: added check to skip calculation at SimStart as the model needs to average the weather data over the day
	// and hence has no sensible weather values to calculate with at stim start
	if((NewDay(pTi)) && (SimStart(pTi)==FALSE)) { */
	if(NewDay(pTi)) {
		fP     = (double)101.3;										//equation (6) const.
		flamda = (double)2.45;										//equation (2)
		fgamma = (double)0.00163*fP/flamda;							//equation (4)

		fra = (double)208/(self->weather.fWindSpeed+ (double)1e-8);           //equation (40) grass
		frc = (double)200/(double)2.88;                               //equation (35) grass
		fgammastern = fgamma*((double)1+frc/fra);					//equation (41)

		if (pPl == NULL) {
			//frc=(double)200/((double)24*(double)0.05);
			//fgammastern = fgamma*((double)1+frc/fra);					//equation (41)
			fgammastern = fgamma;
		} else {
			if PlantIsGrowing { //ep 260406
			//frc = (double)200/(pPl->pCanopy->fLAI + (double)1e-8);		//equation (32)
			//frc = min(frc, (double)200/((double)24*(double)0.05));
			//fgammastern = fgamma*((double)1+frc/fra);					//equation (41)
			fgammastern = fgamma*((double)1+(double)0.34*self->weather.fWindSpeed);//equation (42) grass
			} else {
				//frc=(double)200/((double)24*(double)0.05);
				//fgammastern = fgamma*((double)1+frc/fra);					//equation (41)
				fgammastern = fgamma;
			}// Plant is not growing
		}//pPl != NULL

		fea_Tmax = (double)0.611*(double)exp((double)17.27*self->weather.fTempMax / (self->weather.fTempMax+(double)237.3));
		fea_Tmin = (double)0.611*(double)exp((double)17.27*self->weather.fTempMin / (self->weather.fTempMin+(double)237.3));
		//equation (10)
	
		fea = (double)0.5*(fea_Tmax + fea_Tmin); //equation (17) (saturation vapor pressure)
		fed = (double)2.0*self->weather.fDailyHumidity*(double)0.01/( (double)1/fea_Tmin + (double)1/fea_Tmax ); // actual vapor pressure
		//equation (14)
		fea_Tave = (double)0.611*(double)exp((double)17.27*self->weather.fTempAve / (self->weather.fTempAve+(double)237.3));
		fDelta = (double)4098*fea_Tave/SQR(self->weather.fTempAve+(double)237.3); //equation (3
		pWE->fSatDef = fea - fed;
		//-------------------------------------------------------------------------------------
		fETaero =      fgamma / (fDelta+fgammastern)
		               * (double)900 / (self->weather.fTempAve+(double)273) * self->weather.fWindSpeed
		               * (fea - fed);									          //equation (48)
		//-------------------------------------------------------------------------------------

		fJ = (double)pTi->pSimTime->fTimeY;
	
		fdr = (double)1 + (double)0.033*(double)cos((double)0.0172*fJ);	       //equation (21)

		fdelta = (double)0.409*(double)sin((double)0.0172*fJ - (double)1.39);  //equation (22)
		fphi   = xpn->pLo->pFarm->fLatitude*(double)PI/(double)180;

		ftemp = -(double)tan(fphi)*(double)tan(fdelta);					   //equation (20)
		if(     ftemp <= (double)-1)	fomegas = (double)PI;
		else if(ftemp >= (double)1 )	fomegas = (double)0;
		else						fomegas = (double)acos(ftemp);

		fRa = (double)37.6*fdr*(fomegas*(double)sin(fphi)*(double)sin(fdelta) //equation (19)
		                        + (double)cos(fphi)*(double)cos(fdelta)*(double)sin(fomegas));
		fN  = (double)24/(double)PI * fomegas;							   //equation (25)

        /*AS: commented out for correction of the Rn calculation
		if(self->weather.fDaylySolRad >= (double)0) {
			// Berechnung der aktuellen Sonnenscheindauer //equation (52)
			if (fRa != (double)0) fn  = self->weather.fDaylySolRad/fRa;
			else fn = (double)0;
			fn -= fas;// a_s=0.25
			fn *= fN;
			fn /= fbs;// b_s=0.50
			fn = min(fN,fn);
			fn = max((double)0, fn);
			//fn = min((double)16,fn);
			if (self->fSunDuration < (double)0)
				self->fSunDuration = fn; //keine Expert-N Variable mehr
		} else {
			if (self->fSunDuration >= (double)0) {
				// Berechnung der aktuellen Globalstrahlung //equation (52)
				fn = self->fSunDuration;
				self->weather.fDaylySolRad = fRa*(fn/fN * fbs + fas);
			}
		}//fSolRad>=0

		if (fN != (double)0) {
			fRns = (double)0.77*(fas + fbs * fn/fN)*fRa; //equation (55) grass

			fRnl =  (double)2.45e-9 * ((double)0.9*fn/fN + (double)0.1)
			       * ((double)0.34 - (double)0.14*(double)sqrt(fed))
			       * ( (double)pow(273.0+self->weather.fTempMax, 4.0)
			           +(double)pow(273.0+self->weather.fTempMin, 4.0)); //equation (63)
		} else {
			fRns = (double)0;
			fRnl = (double)0;
		}

		fRn = fRns - fRnl;	//equation (50) [MJ m-2 -d]
        pWE->fcd = (double)0.9*fn/fN + (double)0.1;
        */
        
        //AS: correction of the net radiation part:
        // net solar radiation:
        double Rs0, Rs_Rs0, fcd;
        fRns = 0.77*self->weather.fDaylySolRad;
        //Clear-sky solar radiation Rs0
        Rs0 = (0.75+2.0e-5*xpn->pLo->pFarm->fAltitude)*fRa;
        Rs_Rs0 = self->weather.fDaylySolRad/Rs0;
        if(Rs_Rs0 < 0.3)
            {
                Rs_Rs0 = 0.3;
            }
        else if(Rs_Rs0 > 1.0)
            {
                Rs_Rs0 = 1.0;
            }
        // cloudiness function:
        fcd= 1.35*Rs_Rs0 -0.35;
        fcd = MAX(fcd,0.05);
        fcd = MIN(fcd,1.0);
//        xpn->pCl->pWeather->fcd = fcd;
        // long wave net radiation:
		fRnl =  2.45e-9*fcd *(0.34 - 0.14*sqrt(fed))*(pow(273.0+self->weather.fTempMax, 4.0)+pow(273.0+self->weather.fTempMin, 4.0));
        // net radiation:
        fRn = fRns - fRnl; // [MJ m-2 -d]
        xpn->pHe->pHEBalance->fNetRad = fRn*1.0e6/(24*3600); // [MJ m-2 -d -> W m-2]
        xpn->pHe->pHEBalance->fLongNetRad = fRnl*1.0e6/(24*3600);
        xpn->pHe->pHEBalance->fShortNetRad = fRns*1.0e6/(24*3600);
//        xpn->pHe->pHEBalance->fExtRad = fRa*1.0e6/(24*3600);
        // End of correction by AS
		
		fG = (double)0;
		if(self->fTempYesterday != (double)-99)
			fG = (double)0.38*(self->weather.fTempAve - self->fTempYesterday);	//equation (65)

		self->fTempYesterday = self->weather.fTempAve;	//da alle Berechnungen nur tageweise
        xpn->pHe->pHEBalance->fGroundHeat = -fG*1.0e6/(24*3600); // AS: added to xpn structure for comparison to other gh models; [MJ m-2 d-1 -> W m-2]
		//ist kein if(EndDay) nötig

		//if (fRn > fG)
			fETrad = (double)0.408*fDelta*(fRn - fG)/(fDelta + fgammastern);//equation (49)
		//else
		//	fETrad = 0.0;
			
		//if (fETrad < 0.0)
			
		
		pWE->fPanEvap = fETaero + fETrad;			//equation (31)

		self->fcumaero += fETaero; //Cumulative aerodynamic term
		self->fcumtrad += fETrad; //Cumulative radiation term

		pWa->fPotETDay = pWE->fPanEvap;
	
	}//NewDay


	EvTeiler = (DeltaT < (double)0.11)?
	           SolPartTime(pTi)
	           : (double)1.0;


//	xpn->pWa->fPotETdt = EvTeiler * pWE->fPanEvap;
//SG20210726: bei negativemfPotETdt treten Wasserbilanzfehler auf. Warum? --> erstmal abgefangen!	
    xpn->pWa->fPotETdt = EvTeiler * max(0.0,pWE->fPanEvap);
	xpn->pWa->fPotETR = xpn->pWa->fPotETdt *pWa->kc_factor / DeltaT;

	/***DAILY VARIABLES********************************************************************/
	if(NewDay(self->parent.pTi)) {
		self->Rs_Day = 0.0;
		self->ETpot_Day = self->ETpot_Day_;
		self->ETpot_Day_=0.0;
	}

	if((int)pTi->pSimTime->fTimeY == 1) {
		self->ETpot_Year = 0.0;
	}
	self->Rs_Day += self->weather.fDaylySolRad*DeltaT;
	self->ETpot_Day_ += xpn->pWa->fPotETdt;
	self->ETpot_Year += self->ETpot_dt;
	self->ETpot_Cum += self->ETpot_dt;
	return  RET_SUCCESS;
}

int evapotranspiration_integrate_small_time_step_vars(evapotranspiration *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PTIME pTi = xpn->pTi;
	PCLIMATE pCl = xpn->pCl;
	PWEATHER pWe = pCl->pWeather;
	PWATER pWa = xpn->pWa;
	double dt;

	if ((pTi->pSimTime->fTimeY==self->weather.fTimeY_save) && (pTi->pSimTime->iyear == self->weather.iyear_save)) {
		return RET_SUCCESS;
	} else {
		self->weather.fTimeY_save = pTi->pSimTime->fTimeY;
		self->weather.iyear_save = pTi->pSimTime->iyear;
	}

	dt = pTi->pTimeStep->fAct;



	if ((int)NewDay(pTi)) {
		self->weather.fTempMax = self->__weather.fTempMax;
		self->weather.fTempMin = self->__weather.fTempMin;
		//self->weather.fTempAve = 0.5*(self->weather.fTempMax + self->weather.fTempMin);	
		self->weather.fTempAve = self->__weather.fTempAve;
		self->weather.fWindSpeed = self->__weather.fWindSpeed;
		self->weather.fDaylySolRad = self->__weather.fDaylySolRad;
		self->weather.fDailyHumidity = self->__weather.fDailyHumidity;

		self->__weather.fTempMax=pWe->fTempAir_daily_models;
		self->__weather.fTempMin=pWe->fTempAir_daily_models;
		self->__weather.fTempMax=-99.0;
		self->__weather.fTempMin= 99.0;		
		self->__weather.fTempAve=0.0;
		self->__weather.fDaylySolRad=0.0;
		self->__weather.fWindSpeed = 0.0;
		self->__weather.fDailyHumidity = 0.0;
        self->counter = 0;
	} else {
        self->counter++; // counts number of timesteps used for daily averaging
		if ((pWe->fTempAir_daily_models > self->__weather.fTempMax)&&(pTi->pSimTime->fTimeDay > 0.5)) {
			self->__weather.fTempMax = pWe->fTempAir_daily_models;
		}
		if ((pWe->fTempAir_daily_models < self->__weather.fTempMin)&&(pTi->pSimTime->fTimeDay < 0.5)) {
			self->__weather.fTempMin = pWe->fTempAir_daily_models;
		}

		self->__weather.fWindSpeed += pWe->fWindSpeed_daily_models * dt;
		self->__weather.fTempAve += pWe->fTempAir_daily_models * dt;
		self->__weather.fDaylySolRad += pWe->fSolRad_daily_models * dt;
		self->__weather.fDailyHumidity += pWe->fHumidity_daily_models * dt;

	}

	/*self->weather.fTempAve = *(self->TairMean);
	self->weather.fTempMin = *(self->TairMin);
	self->weather.fTempMax = *(self->TairMax);
	self->weather.fDaylySolRad = *(self->fSolRad);*/
	return RET_SUCCESS;
}

double SolPartTime(PTIME pTi)
{
	double T1, T2, y;
	double DayTime     = pTi->pSimTime->fTimeDay;
	double DeltaT;

	DeltaT = pTi->pTimeStep->fAct;

 
	//if ((DayTime <= (double)0.3)||(DayTime > (double)0.8)) {
    if ((DayTime<=(double)0.3)||((DayTime>(double)0.8)&&((DayTime-DeltaT)>=(double)0.8))){ //SG20220209, as in XN3!
		T1 = (double)0.0;
		T2 = (double)0.0;
	} else {
		T1 = max((double)0.0,(DayTime - DeltaT - (double)0.3));
		T2 = min((double)0.5,(DayTime - (double)0.3));
	} /* else DayTime  */


	y = (double)(cos((double)(2.0*PI*T1)) - cos((double)(2.0*PI*T2))) / (double)2.0;

	return y;
}
