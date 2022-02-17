//      evapotranspiration.c
//
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//




// Reference: THE ASCE STANDARDIZED REFERENCE EVAPOTRANSPIRATION EQUATION of the Environmental and Water Resources Institute January, 2005 Final Report

#include "evapotranspiration.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "evapotranspiration_reg.h"

G_DEFINE_TYPE(evapotranspiration, evapotranspiration, EXPERTN_MODUL_BASE_TYPE);


static void evapotranspiration_class_init(evapotranspirationClass *klass) {}


static void evapotranspiration_init(evapotranspiration *self)
{
	self->__ERROR=RET_SUCCESS;
}




// Our Modul Functions:

int evapotranspiration_load(evapotranspiration *self)
{
	/* Declarations/Abbreviations */
	expertn_modul_base *xpn = &(self->parent);
	xpn_register_var *var_list;
	var_list = xpn->pXSys->var_list;

	self->ETpot_Day = 0.0;
	self->ETpot_Day_=0.0;
	self->ETpot_Year = 0.0;

	self->Epot_Day = 0.0;
	self->Epot_Year = 0.0;

	self->stomata_res_day=0.0;

	EVAPOTRANSPIRATION_REG


	//self->KC_Factor = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.Penman Monteith.kc",1.0);
	xpn->pSo->fSoilCover = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.Penman Monteith.soil cover",0.0);
	//self->Time_Zone = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.Penman Monteith.time zone",0.0);

    //AS: added information about weather data resolution
    char *S;
	S = xpn_register_var_get_pointer(xpn->pXSys->var_list,"Config.Expert N Standard Read INI.use high resolution climate data");
	if ((S==NULL) || (atoi(S)==0))
		{self->high_res_weather=0;} 
    else 
        {self->high_res_weather=1;}

	return RET_SUCCESS;
}

//  CONVERT EVAP TERMS FROM KINEMATIC (KG M-2 S-1) TO ENERGY UNITS (W M-2)
#define LVH2O 2.501E+6


// calculates latant heat of Vaporization:
// input T: Temperature in °C
// result: lambda in MJ / kg
// 2.45 MJ/kg für 20°C
// 2.5 MJ/kg für 0°C
static double calc_lh_of_vap(double T)
{
	double T2,T3,L;
	// Cubic fit to Table 2.1,p.16, Textbook: R.R.Rogers & M.K. Yau, A Short Course in Cloud Physics, 3e,(1989), Pergamon press
	// The latent heat of condensation of water in the temperature range from −40 °C to 40 °C is approximated by the following empirical cubic function:
	T2 = T*T;
	T3 = T2*T;
	L = (0.0000614342*T3 + 0.00158927*T2 - 2.36418*T + 2500.79)*1e-3; // für MJ /m2 ; AS: this is probably in MJ/kg or MJ/L
	return L;
}


int penman_monteith_lh_run(evapotranspiration *self)
{
	expertn_modul_base *xpn = &(self->parent);
	double L_evap;
	double LE;
	L_evap = calc_lh_of_vap(xpn->pCl->pWeather->fTempAir) * 1.0e6; // MJ --> J
	LE = (L_evap * xpn->pWa->fActETR) /(24.0*60.0*60.0);
	xpn->pHe->pHEBalance->fHeatLatent=LE;
	return RET_SUCCESS;
}

int penman_monteith_lh_pot_run(evapotranspiration *self)
{
	expertn_modul_base *xpn = &(self->parent);
	double L_evap;
	double LE;
	L_evap = calc_lh_of_vap(xpn->pCl->pWeather->fTempAir) * 1.0e6; // MJ --> J
	LE = (L_evap * xpn->pWa->fPotETR) /(24.0*60.0*60.0);
	xpn->pHe->pHEBalance->fHeatLatent=LE;
	return RET_SUCCESS;
}

int penman_monteith_sh_run(evapotranspiration *self)
{
	expertn_modul_base *xpn = &(self->parent);
	double Hs,Rn,LE,G;
	Rn = xpn->pHe->pHEBalance->fNetRad;
	LE = xpn->pHe->pHEBalance->fHeatLatent;
	G = xpn->pHe->pHEBalance->fGroundHeat;
	Hs = Rn - LE + G;
	xpn->pHe->pHEBalance->fHeatSens=Hs;
	return RET_SUCCESS;
}

int penman_monteith_sh_run2(evapotranspiration *self)
{
	expertn_modul_base *xpn = &(self->parent);
	xpn->pHe->pHEBalance->fHeatSens = self->pot_sens_heat * 1.0e6/(24.0*60.0*60.0); // MJ --> J
	return RET_SUCCESS;
}

int penman_monteith_gh_run(evapotranspiration *self)
{
	expertn_modul_base *xpn = &(self->parent);
	double Rs,Rn,G;

	Rs = xpn->pCl->pWeather->fSolRad/24.0;
	Rn = xpn->pHe->pHEBalance->fNetRad;


	//soil heat flux density G
	if(Rs>1e-2)   //daytime as defined in ASCE page 5
		{
			G=0.1*Rn; //eqn 65a
            //G=0.2*Rn; //eqn 65a
		}
	else  	//nighttime
		{
			G=0.5*Rn; //eqn 65b
		}


	xpn->pHe->pHEBalance->fGroundHeat = -G; //convert to flux defined positive if oriented towards soil surface
	return RET_SUCCESS;
}


int penman_monteith_gh_run_plus(evapotranspiration *self)
{
	expertn_modul_base *xpn = &(self->parent);
    double L_evap;	
	L_evap = calc_lh_of_vap(xpn->pCl->pWeather->fTempAir) * 1.0e6; // MJ --> J
    penman_monteith_gh_run(self);    
	xpn->pHe->pHEBalance->fGroundHeat -= (L_evap * (xpn->pWa->fPotETR-xpn->pWa->fActETR)) /(24.0*60.0*60.0);
	return RET_SUCCESS;
}


int penman_monteith_gh_run_with_LAI(evapotranspiration *self)
{
	expertn_modul_base *xpn = &(self->parent);
	double Rs,Rn,G;

	Rs = xpn->pCl->pWeather->fSolRad/24.0;
	Rn = xpn->pHe->pHEBalance->fNetRad;

	//soil heat flux density G : (Choudhury et al. 1987, Choudhury et al. 1989)
	double KG;
	if(Rs>0.0)   //daytime as defined in ASCE page 5
		{
			KG = 0.4;
		}
	else  	//nighttime
		{
			KG = 2.0;
		}
	G = KG * exp(-0.5*xpn->pPl->pCanopy->fLAI)*Rn;

	xpn->pHe->pHEBalance->fGroundHeat = -G; //convert to flux defined positive if oriented towards soil surface

	return RET_SUCCESS;
}

int penman_monteith_gh_run_FAO(evapotranspiration *self)
{
	expertn_modul_base *xpn = &(self->parent);
	double G = 0;
    double dt = xpn->pTi->pTimeStep->fAct;

    if(NewDay(self->parent.pTi)) {
        // In case of daily weather data use T=(Tmin+Tmax)/2, in case of high-res weather data use actual daily mean temperature:
        self->weather.fTempAve = (xpn->pCl->pWeather->fTempMax + xpn->pCl->pWeather->fTempMin)/2.0;
        // For daily weather data start calculating G at the second day, for high-res weather data start at the third day
        // as there is no yesterday before (--> G is shifted by one day for high-res weather data!)
        if( ((SimStart(self->parent.pTi) == FALSE) && (self->high_res_weather == 0)) || ((xpn->pTi->pSimTime->year+xpn->pTi->pSimTime->fTimeY_old >= xpn->pTi->pSimTime->iStart_year+xpn->pTi->pSimTime->fStart_TimeY+2) && (self->high_res_weather == 1)) ){
            G = 0.38*(self->weather.fTempAve - self->fTempYesterday)*1.0e6/(24*3600); //[MJ m-2 d-1] -> [W m-2]
        }
        self->fTempYesterday = self->weather.fTempAve;
        xpn->pHe->pHEBalance->fGroundHeat = -G; //convert to flux defined positive if oriented towards soil surface
    }
    // Get daily average of high-res weather data
    if(self->high_res_weather == 1) {
        self->__weather.fTempAve += xpn->pCl->pWeather->fTempAir_daily_models*dt;
    }
	return RET_SUCCESS;
}

int penman_monteith_nr_load(evapotranspiration *self)
{
	self->night_rad = 0.0;
	self->night_rad2 = 0.0;
	self->night_count = 0;
    self->night_count_2 = 0;
	self->night_time = 0.0;

	self->cloud_cover_factor=0.5;
	return RET_SUCCESS;
}

int penman_monteith_nr_run(evapotranspiration *self)
{
	expertn_modul_base *xpn = &(self->parent);
	double Rn,Rns,Rnl,Ra; //AS: added extraterrestrial radiation Ra

	Rns = 0.0;
	Rnl = 0.0;
    Ra = 0.0;

	Rn = evapotranspiration_get_Rn(self,&Rns,&Rnl,&Ra,0);

	xpn->pHe->pHEBalance->fNetRad=Rn;
	xpn->pHe->pHEBalance->fShortNetRad=Rns;
	xpn->pHe->pHEBalance->fLongNetRad=Rnl;

	xpn->pCl->pWeather->fSolNet = xpn->pHe->pHEBalance->fNetRad/86400.0*1.0e-6; // W/m2 --> MJ/m2/day


	return RET_SUCCESS;
}

int penman_monteith_nr_run_FAO(evapotranspiration *self)
{
	expertn_modul_base *xpn = &(self->parent);
	double Rn,Rns,Rnl,Ra;
    
    Rn = 0.0;
	Rns = 0.0;
	Rnl = 0.0;
    Ra = 0.0;
    
    // calculate net radiation at the start of a new day using daily averaged weather data (of the day before -> Rn is shifted by one day!)
    if(NewDay(xpn->pTi)) {
        if(self->high_res_weather == 1) {
            self->weather.fTempMax = self->__weather.fTempMax;
            self->weather.fTempMin = self->__weather.fTempMin;
            self->weather.fDailyHumidity = self->__weather.fDailyHumidity;
            self->weather.fDaylySolRad = self->__weather.fDaylySolRad;
            self->__weather.fTempMax = -99;
            self->__weather.fTempMin = 99;
            self->__weather.fDailyHumidity = 0;
            self->__weather.fDaylySolRad = 0;
        }
        else {
            self->weather.fDailyHumidity = self->__weather.fDailyHumidity;
            self->weather.fDaylySolRad = self->__weather.fDaylySolRad;
            self->__weather.fDailyHumidity = 0;
            self->__weather.fDaylySolRad = 0;
        }
        if(SimStart(xpn->pTi)==FALSE) {
            Rn = evapotranspiration_get_Rn(self,&Rns,&Rnl,&Ra,1);
//            xpn->pHe->pHEBalance->fNetRadDay=Rn;
//            xpn->pHe->pHEBalance->fShortNetRadDay=Rns;
//            xpn->pHe->pHEBalance->fLongNetRadDay=Rnl;
//            xpn->pHe->pHEBalance->fExtRadDay=Ra;
            // add net radiation also to variables used in the sub daily models
            xpn->pHe->pHEBalance->fNetRad=Rn;
            xpn->pHe->pHEBalance->fShortNetRad=Rns;
            xpn->pHe->pHEBalance->fLongNetRad=Rnl;
            //xpn->pHe->pHEBalance->fExtRad=Ra;
        }
    }
   // Get daily averages of weather data:
    self->__weather.fDailyHumidity += xpn->pCl->pWeather->fHumidity_daily_models * xpn->pTi->pTimeStep->fAct;
    self->__weather.fDaylySolRad += xpn->pCl->pWeather->fSolRad * xpn->pTi->pTimeStep->fAct;
    if(self->high_res_weather == 1) { // for high res weather data Tmin and Tmax have to be updated every timestep:
        if(self->__weather.fTempMax < xpn->pCl->pWeather->fTempAir) {
            self->__weather.fTempMax = xpn->pCl->pWeather->fTempAir;
        }
        if(self->__weather.fTempMin > xpn->pCl->pWeather->fTempAir) {
            self->__weather.fTempMin = xpn->pCl->pWeather->fTempAir;
        }
    }
    else{ // for daily weather data Tmin and Tmax can be extracted from the dataset:
        int LastStep = 24*(1-xpn->pTi->pTimeStep->fAct);
        if(xpn->pTi->pSimTime->hour >= LastStep){ //at the last timestep of a day get daily min and max Temp for Rn calculation
            self->weather.fTempMax = xpn->pCl->pWeather->fTempMax;
            self->weather.fTempMin = xpn->pCl->pWeather->fTempMin;
        }
    }
	return RET_SUCCESS;
}

static double calc_long_wave_net_rad(evapotranspiration *self)
{
	expertn_modul_base *xpn = &(self->parent);
	// Stefan Boltzmann Konst (MJ K-4 m-2 h-1):
#define sigma 2.042*1.0e-10
	double f_up;
	double lwdn;
	double net_lwr;
	//f_up = xpn->pHe->fEmissi * sigma * pow(xpn->pHe->pHLayer->pNext->fSoilTemp+273.15,4.0);
	f_up = sigma * pow(xpn->pCl->pWeather->fTempAir+273.15,4.0);
	lwdn = xpn->pHe->fEmissi * sigma * pow(xpn->pCl->pWeather->fTempAir+273.15,4.0);


	// oder:
	//Rnl = 2.042*1e-10*fcd*(0.34-0.14*sqrt(ea))*pow(T+273.16,4.0); //eqn 44

	net_lwr = - lwdn + f_up;
	return net_lwr;
}

// T = Temperature in Grad C
// return vapor pressure of water [hPa]
static double get_e(double T)
{
	double a0,a1,a2,a3,a4,a5,a6;
	a0 = 6.10779961;
	a1 = 4.436518521e-1;
	a2 = 1.428945805e-2;
	a3 = 2.650648471e-4;
	a4 = 3.031240396e-6;
	a5 = 2.034080948e-8;
	a6 = 6.136820929e-11;
	return a0 + T * (a1 + T*(a2 + T*(a3 + T*(a4 + T * (a5 + T * a6)))));
}

int evapotranspiration_emissivity(evapotranspiration *self)
{
	expertn_modul_base *xpn = &(self->parent);
	double epsilon_CL = 0.976;
	double b = 0.433;
	double epsilon_CS;
	double cloud_cover; // 0..1
	double ea;
	double emissivity;
	cloud_cover = self->cloud_cover_factor;

	ea = get_e(xpn->pCl->pWeather->fTempAir);
	epsilon_CS = 0.23 + b * pow(ea / (xpn->pCl->pWeather->fTempAir+273.15) ,1.0/8.0);
	emissivity = epsilon_CS* (1.0-(cloud_cover*cloud_cover))+epsilon_CL*(cloud_cover*cloud_cover);

	xpn->pHe->fEmissi = emissivity;
	return RET_SUCCESS;
}

int evapotranspiration_run(evapotranspiration *self)
{
	/* Abbreviation */
	expertn_modul_base *xpn = &(self->parent);
	PWEATHER pWe = xpn->pCl->pWeather;

	/* Variables */
	double Rs;
	double Cn, Cd;
	double T, RH;	// T [°C], RH [%]

	double dT;		// time step [day]
	double iJulianDay;

	double Delta, es, ea;
	double Rn;
	double G;
	double measurementheight, u2, plant_height;
	double wind;
	double P, gamma;
	double ETpot;
	double L_evap;

	/* Get Climate and Location Values */

	T	= pWe->fTempAir;
	RH	= pWe->fHumidity;
	wind = pWe->fWindSpeed;	// wind speed [m/s]
	Rs = pWe->fSolRad/24.0;
	//Rs = pWe->fPAR/24.0;

	//C_DEBUG(pWe->fPAR/24.0);

	measurementheight = xpn->pCl->fWindMeasHeight;
	if(measurementheight == 0.0)
		{
			measurementheight = 2.0;
		}
    //AS: set measurementheight always at least 10cm above the crop height
    plant_height = xpn->pPl->pCanopy->fPlantHeight;
    measurementheight = max(measurementheight,plant_height+0.1);

	/* Get time */
	dT = xpn->pTi->pTimeStep->fAct;
	iJulianDay	= (int)xpn->pTi->pSimTime->fTimeY;	// actual julian day




	if(Rs > 0.01)  	// table 1, hourly volumes converted depending on the actual timestep
		{
			Cn=37.0;	// [K*mm*s3/Mg*h]
			Cd=0.24;	// [K*mm*s3/Mg*h]
		}
	else  	//nighttime
		{
			Cn=37.0;	//
			Cd=0.96;	//
		}

	//Slope of the Saturation Vapor Pressure-Temperature Curve (Δ)
	Delta = 2503.0*exp( 17.27*T/(T+237.3) )/ pow(T+237.3,2.0); //eqn. 36, [kPa/°C]


	//Saturation Vapor Pressure (es)
	es = 0.6108*exp(17.27*T/(T+237.3)); //eqn. 37, [kPa/°C]

	//Actual Vapor Pressure (ea)
	//Method nr. 3 in table 4, eqn 37, 41, [kPa/°C]
	//using rel. humidity and temperature at the hour
	ea = RH/100.0 * es;




	Rn = xpn->pHe->pHEBalance->fNetRad*(3600.0*1.0e-6); // W / m2 -> MJ / (m2 h)


	G = -xpn->pHe->pHEBalance->fGroundHeat*(3600.0*1.0e-6); // W / m2 -> MJ / (m2 h)


	//wind speed at 2m
	u2 = wind*4.87/log(67.8*measurementheight-5.42);	//eqn 67


	// calculation of the Psychrometric constant
	//P = 101.3*pow((293.0-0.0065*elevation)/293.0, 5.26);	//eqn 34, atmospheric pressure
	P = xpn->pCl->pWeather->fAtmPressure*1.0e-3;
	gamma = 0.000665*P;	//eqn 35

	C_DEBUG(P);

	C_DEBUG(Cd*u2)
	L_evap = calc_lh_of_vap(xpn->pCl->pWeather->fTempAir);
	//ETpot = ((0.408*Delta*(Rn-G)+gamma*Cn/(T+273.0)*u2*(es-ea))/(Delta+gamma*(1.0+Cd*u2)))*24.0;//*24.0; //eqn 1, [mm/h]*24.0 -> [mm/d]
	ETpot = (( 1.0/L_evap *Delta*(Rn-G)+gamma*Cn/(T+273.0)*u2*(es-ea))/(Delta+gamma*(1.0+Cd*u2)))*24.0;//*24.0; //eqn 1, [mm/h]*24.0 -> [mm/d]
	//ETpot = (( 1.0/L_evap *Delta*(Rn-G)+gamma*Cn/(T+273.0)*u2*(1.1))/(Delta+gamma*(1.0+Cd*u2)))*24.0;//*24.0; //eqn 1, [mm/h]*24.0 -> [mm/d]

	self->pot_sens_heat = L_evap*((( 1.0/L_evap *gamma*(1.0+Cd*u2)*(Rn-G)-gamma*Cn/(T+273.0)*u2*(es-ea))/(Delta+gamma*(1.0+Cd*u2)))*24.0);

	// introduce the Kc factor
	//ATTN: hard coded Kc for Spruce!
	// Add Dew fall to rain
	if (ETpot < 0.0)
		{
			xpn->pCl->pWeather->fPreciRate-=self->ETpot;
			xpn->pCl->pWeather->fLiquPreciRate-=self->ETpot;

		}
	self->ETpot = MAX(0.0, ETpot)*xpn->pWa->kc_factor;		// [mm/d]
	//self->ETpot = ETpot*self->KC_Factor;
	
	self->ETpot_dt = self->ETpot*dT;	// [mm/dt]

	/* Values To XN Structure */
	xpn->pWa->fPotETR = self->ETpot;
	xpn->pWa->fPotETdt = self->ETpot_dt;

	if (xpn->DEBUG_LEVEL>3)
		{
			CHECK_VALID(Rn);
			CHECK_VALID(xpn->pWa->fPotETR);
		}

	/***DAILY VARIABLES********************************************************************/
	if(NewDay(self->parent.pTi))
		{
			self->Rs_Day = 0.0;
			self->ETpot_Day = self->ETpot_Day_;
			self->ETpot_Day_=0.0;
			self->stomata_res_day=0.0;

		}

	self->stomata_res_day+=xpn->pPl->pCanopy->fsto_res*dT;
	C_DEBUG(self->stomata_res_day)

	if(iJulianDay == 1)
		{
			self->ETpot_Year = 0.0;
		}
	self->Rs_Day += pWe->fSolRad*dT;
	self->ETpot_Day_ += xpn->pWa->fPotETdt;
	self->ETpot_Year += self->ETpot_dt;
	self->ETpot_Cum += self->ETpot_dt;


	// Rn [MJ / (m2h)] --> W / (m2s):  3600/1.0e6

	C_DEBUG(Rn/3600.0*1.0e6);

	//Rn = Rns + calc_long_wave_net_rad(self);


	return self->__ERROR;
}

// Peneman Monteith Method 1965,81
int evapotranspiration_run65_0(evapotranspiration *self)
{
	return evapotranspiration_run65(self,0); //PM_ASCE
}

int evapotranspiration_run65_1(evapotranspiration *self)
{
	return evapotranspiration_run65(self,1); //PM_ASCE (MO 1)
}

int evapotranspiration_run65_2(evapotranspiration *self)
{
	return evapotranspiration_run65(self,2); //PM_ASCE crop
}

int evapotranspiration_run65(evapotranspiration *self, int opt_sfc)
{
	/* Abbreviation */
	expertn_modul_base *xpn = &(self->parent);
	PWEATHER pWe = xpn->pCl->pWeather;

	/* Variables */
	double T, RH;	// T [°C], RH [%]

	double dT;		// time step [day]
	double iJulianDay;

	double Delta, es, ea;
	double Rn;
	double G;
	double measurementheight;
	double wind;
	double P, gamma;
	double ETpot;
	double L_evap;
	double rho_a; // mean air density at constant pressure [kg m-3]
	double c_p; // specific heat of the air [MJ kg-1 °C-1]
	double rs; // (bulk) surface resistance [s m-1]
	double ra; // (areodynamic resistance [s m-1]
	double plant_height; // m

	c_p = 1.013e-3; // [MJ kg-1 °C-1]

	/* Get Climate and Location Values */

	T	= pWe->fTempAir;
	RH	= pWe->fHumidity;
	wind = pWe->fWindSpeed;	// wind speed [m/s]
	plant_height = xpn->pPl->pCanopy->fPlantHeight;
	//Rs = pWe->fPAR/24.0;

	//C_DEBUG(pWe->fPAR/24.0);



	measurementheight = xpn->pCl->fWindMeasHeight;
	if(measurementheight == 0.0)
		{
			measurementheight = 2.0;
		}
    //AS: set measurementheight always at least 10cm above the crop height
    measurementheight = max(measurementheight,plant_height+0.1);

	/* Get time */
	dT = xpn->pTi->pTimeStep->fAct;
	iJulianDay	= (int)xpn->pTi->pSimTime->fTimeY;	// actual julian day


	//Slope of the Saturation Vapor Pressure-Temperature Curve (Δ)
	Delta = 2503.0*exp( 17.27*T/(T+237.3) )/ pow(T+237.3,2.0); //eqn. 36, [kPa/°C]


	//Saturation Vapor Pressure (es)
	es = 0.6108*exp(17.27*T/(T+237.3)); //eqn. 37, [kPa/°C]

	//Actual Vapor Pressure (ea)
	//Method nr. 3 in table 4, eqn 37, 41, [kPa/°C]
	//using rel. humidity and temperature at the hour
	ea = RH/100.0 * es;




	Rn = xpn->pHe->pHEBalance->fNetRad*(3600.0*1.0e-6); // W / m2 -> MJ / (m2 h)


	G = -xpn->pHe->pHEBalance->fGroundHeat*(3600.0*1.0e-6); // W / m2 -> MJ / (m2 h)


	// calculation of the Psychrometric constant
	//P = 101.3*pow((293.0-0.0065*elevation)/293.0, 5.26);	//eqn 34, atmospheric pressure
	P = xpn->pCl->pWeather->fAtmPressure*1.0e-3;
	gamma = 0.000665*P;	//eqn 35

	C_DEBUG(P);

	// Atmospheric density EQ: B.10
	{
		double TKv; // virtual Temperature [K]
		double TK; // absolute Temperature [K]
		TK = 273.16+T;
		TKv = TK * 1.0/(1.0-0.378*ea/P);
		rho_a = 3.486 * P / TKv;

	}


	L_evap = calc_lh_of_vap(xpn->pCl->pWeather->fTempAir);



	switch (opt_sfc)
		{
		case 0: //Penman Monteith ASCE81
			{
			// eq: B.2
			double d;
			double zom,zoh;
			double psi_h,psi_m;
			double ch;
			d=    0.67 * plant_height;
			zom = 0.123 * plant_height;
			zoh = 0.0123 * plant_height;
			
			psi_h = 0.0;
			psi_m = 0.0;
			ra = log((measurementheight-d)/zom-psi_m)*log((measurementheight-d)/zoh-psi_h)/(0.41*0.41*wind);
			ch = 1.0/(log((measurementheight-d)/zom-psi_m)*log((measurementheight-d)/zoh-psi_h)/(0.41*0.41));
			C_DEBUG(ch);
			}
			break;
		case 1: //Penman Monteith ASCE81 (MO)
			{
			double ch;
			ch = surface_layer_get_ch(xpn);
			ra = 1.0/(ch*wind);
			}
			break;
		case 2: //Penman Monteith ASCE81 (crop)
			{	
			// eq: B.2
			double d;
			double zom,zoh;
			double psi_h,psi_m;
			//double ch;
			d=    max(0.10,0.67 * plant_height);
			zom = max(0.05,0.123 * plant_height);
			zoh = max(0.005,0.0123 * plant_height);
			psi_h = 0.0;
			psi_m = 0.0;
			ra = log((measurementheight-d)/zom-psi_m)*log((measurementheight-d)/zoh-psi_h)/(0.41*0.41*wind);
			//ch = 1.0/(log((measurementheight-d)/zom-psi_m)*log((measurementheight-d)/zoh-psi_h)/(0.41*0.41));
			break;
			}
		}

	// eq: B.3
    double rss,rsc,rl,LAI_active;
    //rl = 100.0; // [s m-1] stomatal resistiance
		rl = xpn->pPl->pCanopy->fsto_res;
    //AS: fsto_res somehow is 0 for every plant in crop rotation after the first one. Set to back to standard value 100:
    if (xpn->pPl->pCanopy->fsto_res == 0) {
        xpn->pPl->pCanopy->fsto_res = 100;
    }
    LAI_active = xpn->pPl->pCanopy->fLAI*0.5;
    //AS: changed to include soil surface resistance rss in parallel to crop surface resistance rsc
    // rs = rl / LAI_active;  // AS: changed from rl*LAI_active to rl/LAI_active according to FAO56
    if (LAI_active > 0.05) {
        rsc = rl / LAI_active; // crop surface resistance (see FAO56)
        rss = self->rss; // soil surface resistance
        rs = rss*rsc/(rss+rsc);
    }
    else { // if there is no crop yet, use soil surface resistance only
        rs = self->rss;
    }
    //ad resistances to output:
    xpn->pWa->rss = self->rss;
    xpn->pWa->rsc = rsc;
    xpn->pWa->rs = rs;
    xpn->pWa->ra = ra;
	C_DEBUG(rs/ra)
    //AS: debugging:
    xpn->pWa->rsra = rs/ra;
    xpn->pWa->PM_numerator = Delta+gamma*(1.0+rs/ra);

	//es=ea;
	ETpot = ((Delta*(Rn-G)+3600.0*rho_a*c_p*(es-ea)/ra)/(Delta+gamma*(1.0+rs/ra)))/L_evap*24.0;
	//ETpot 			            = (( 1.0/L_evap *Delta*(Rn-G)+gamma*Cn/(T+273.0)*u2*(es-ea))/(Delta+gamma*(1.0+Cd*u2)))*24.0;//*24.0; //eqn 1, [mm/h]*24.0 -> [mm/d]
	//self->pot_sens_heat = L_evap*((( 1.0/L_evap *gamma*(1.0+Cd*u2)*(Rn-G)-gamma*Cn/(T+273.0)*u2*(es-ea))/(Delta+gamma*(1.0+Cd*u2)))*24.0);
	self->pot_sens_heat = ((gamma*(1.0+rs/ra)*(Rn-G)-3600.0*rho_a*c_p*(es-ea)/ra)/(Delta+gamma*(1.0+rs/ra)))*24.0;


	// introduce the Kc factor
	//ATTN: hard coded Kc for Spruce!
	// Add Dew fall to rain
	if (ETpot < 0.0)
		{
			xpn->pCl->pWeather->fPreciRate-=self->ETpot;
			xpn->pCl->pWeather->fLiquPreciRate-=self->ETpot;
			
		}
	self->ETpot = MAX(0.0, ETpot)*xpn->pWa->kc_factor;		// [mm/d]
	//self->ETpot = ETpot*self->KC_Factor;
	self->ETpot_dt = self->ETpot*dT;	// [mm/dt]

	/* Values To XN Structure */
	xpn->pWa->fPotETR = self->ETpot;
	xpn->pWa->fPotETdt = self->ETpot_dt;

	if (xpn->DEBUG_LEVEL>3)
		{
			CHECK_VALID(Rn);
			CHECK_VALID(xpn->pWa->fPotETR);
		}

	/***DAILY VARIABLES********************************************************************/
	if(NewDay(self->parent.pTi))
		{
			self->Rs_Day = 0.0;
			self->ETpot_Day = self->ETpot_Day_;
			self->ETpot_Day_=0.0;
			self->stomata_res_day=0.0;

		}

	self->stomata_res_day+=xpn->pPl->pCanopy->fsto_res*dT;
	C_DEBUG(self->stomata_res_day)

	if(iJulianDay == 1)
		{
			self->ETpot_Year = 0.0;
		}
	self->Rs_Day += pWe->fSolRad*dT;
	self->ETpot_Day_ += xpn->pWa->fPotETdt;
	self->ETpot_Year += self->ETpot_dt;
	self->ETpot_Cum += self->ETpot_dt;


	// Rn [MJ / (m2h)] --> W / (m2s):  3600/1.0e6

	C_DEBUG(Rn/3600.0*1.0e6);

	//Rn = Rns + calc_long_wave_net_rad(self);


	return self->__ERROR;
}


// old crop cover function (approx):
double evapotranspiration_calc_crop_cover_frac1(double LAI)
{
	double SoilCoverFrac;
	if((LAI/3.0) < 1.0)
		{
			SoilCoverFrac = LAI/3.0;	// LAI converted to SoilCoverFrac
		}
	else
		{
			SoilCoverFrac = 1.0;
		}
	return SoilCoverFrac;
}


// Raes, D., Van Aelst, P., Wyseure, G., & ETref, E. (1986).
// ETsplit and deficit, a computer package for calculating crop
// water requirements: reference manual. Leuven: Laboratory of Soil and Water Engineering.
// Seite 47
double evapotranspiration_calc_crop_cover_frac2(double LAI,double kcan)
{
	double f;
	double c;
	f = 1.0;
	//c = 0.6;
	// f and c are regression coefficionts

	c = kcan;

	return 1.0-f*exp(-c*LAI);
}

int evaporation_run1(evapotranspiration *self)
{
	return evaporation_run(self,1);
}

int evaporation_run2(evapotranspiration *self)
{
	return evaporation_run(self,2);
}

int evaporation_run3(evapotranspiration *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PEVAP pE = xpn->pWa->pEvap;
	double dt = xpn->pTi->pTimeStep->fAct;					// actual time step
	int iJulianDay = (int)xpn->pTi->pSimTime->fTimeY;	// actual julian day
	pE->fPotR=0.0;
	if (xpn->DEBUG_LEVEL>3)
		{
			CHECK_VALID(pE->fPotR);
		}

	pE->fPot = pE->fPotR*dt;


	if(NewDay(self->parent.pTi))
		{
			self->Epot_Day = 0.0;
		}
	if(iJulianDay == 1)
		{
			self->Epot_Year = 0.0;
		}
	self->Epot_Day += pE->fPot;
	self->Epot_Year += pE->fPot;
	self->Epot_Cum += pE->fPot;
	return RET_SUCCESS;
}

int evaporation_run(evapotranspiration *self,int crop_cover_calc)
{
	/*Abbreviations*/
	expertn_modul_base *xpn = &(self->parent);
	PCANOPY pCan = xpn->pPl->pCanopy;
	PEVAP pE = xpn->pWa->pEvap;

	/*Variables*/
	double SoilCoverFrac = 0.0;
	double fSCEff = 0.35;
	double EPSILON = 0.000001;
	double iJulianDay;
	double dt;
    double dayafterem;
    double LAI;

	/* Functions */

	/****************************************************************************************************************/

	dt = xpn->pTi->pTimeStep->fAct;					// actual time step
	iJulianDay	= xpn->pTi->pSimTime->fTimeY;	// actual julian day

	pE->fPotR_old = pE->fPotR;
    
    dayafterem = xpn->pPl->pDevelop->iDayAftEmerg;
    C_DEBUG(dayafterem);
    C_DEBUG(xpn->pPl->pDevelop->bPlantGrowth);
    C_DEBUG(xpn->pPl->pDevelop->bMaturity);


	if(xpn->pPl->pDevelop->iDayAftEmerg == 0)
		{
			pE->fPotR = (1.0-xpn->pSo->fSoilCover*fSCEff)*xpn->pWa->fPotETR;	// evaporation rate [mm/d]
		}
	else
		{
			if ((pCan->fCropCoverFrac <= 0.0)&&(pCan->fLAI > EPSILON))
				{
					// Möglichkeit 1:
					if (crop_cover_calc==1)
						{
							SoilCoverFrac = evapotranspiration_calc_crop_cover_frac1(pCan->fLAI);
						}
					else
						{
							// Möglichkeit 2:
							SoilCoverFrac = evapotranspiration_calc_crop_cover_frac2(pCan->fLAI,pCan->fkcan);
						}
				}
			else
				{
					SoilCoverFrac = max(0.0,pCan->fCropCoverFrac);
				}

			SoilCoverFrac = SoilCoverFrac + (1.0-SoilCoverFrac)*xpn->pSo->fSoilCover*fSCEff;

			if(SoilCoverFrac < 1.0)
				{
					pE->fPotR = (1-SoilCoverFrac)*xpn->pWa->fPotETR;
				}
			else
				{
					pE->fPotR = 0.0;	//only transpiration takes place
				}
		}
        
    C_DEBUG(SoilCoverFrac);
	//printf("Soil Cover Fraction %f ETpot %f Epot %f \n", SoilCoverFrac,xpn->pWa->fPotETR,pE->fPotR);
    LAI= pCan->fLAI;
    C_DEBUG(LAI);
    C_DEBUG(pCan->fkcan);

	if (xpn->DEBUG_LEVEL>3)
		{
			CHECK_VALID(pE->fPotR);
		}

	pE->fPot = pE->fPotR*dt;


	if(NewDay(self->parent.pTi))
		{
			self->Epot_Day = 0.0;
		}
	if(iJulianDay == 1)
		{
			self->Epot_Year = 0.0;
		}
	self->Epot_Day += pE->fPot;
	self->Epot_Year += pE->fPot;
	self->Epot_Cum += pE->fPot;



	return self->__ERROR;
}

int act_evaporation_run(evapotranspiration *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PEVAP pE = xpn->pWa->pEvap;





	/* Actual Evaporation is the minimum of potential and maximal evaporation */

	if (pE->fMaxR > pE->fPotR)
		{
			pE->fActR = pE->fPotR;
		}
	else
		{
			pE->fActR = pE->fMaxR;
		}

	C_DEBUG(pE->fActR);

	return RET_SUCCESS;
}

int pot_transpiration_run(evapotranspiration *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PPLTWATER pPLW = xpn->pPl->pPltWater;
	PEVAP pE = xpn->pWa->pEvap;


	pPLW->fPotTranspR = xpn->pWa->fPotETR - pE->fPotR - pPLW->fInterceptR; // [mm/d]

	if(pPLW->fPotTranspR < 0.0)
		{
			pPLW->fPotTranspR = 0.0;
		}

	pPLW->fPotTranspdt = pPLW->fPotTranspR*xpn->pTi->pTimeStep->fAct;

	return RET_SUCCESS;
}

/*Calculate Net Radiation from Solar Radiation [W/m2]; AS: added extraterrestrial radiation for debugging and FAO version*/
double evapotranspiration_get_Rn(evapotranspiration *self,double *Rns,double *Rnl, double *Ra, int version)
{
	expertn_modul_base *xpn = &(self->parent);
	/* Abbreviations */

	/* Variables */
	double latitude;
	double soldec;
	int iJulianDay;
	double elevation;
	double t, b, Sc;
	double h, T, RH;
	double w, w1, w2, ws;
	double dr;
	double Rs, Rs0;
	double ea, es, es_Tmax, es_Tmin;
	double Rn;
	double fcd, N;
	double ftime_day;
	double measurementheight;
	double dT;
	double Lm;
	// double Ra; AS: Ra now already is defined in the input
	double mid_of_day;
	double dusk_time;
	double dawn_time;
	double time_for_night_rad1;
	double time_for_night_rad2;
	double Rs_Rs0;
	double Lz,Time_Zone;
    double fDelta, fJ, fdr, fdelta, fphi, ftemp, fomegas; //AS: for calculation of daily Ra as in "Penman Monteith (FAO)"
    double sol_elev1, sol_elev2; //AS: solar elevation angle (used to decide, if fcd should be calculated)

	Time_Zone = xpn->pTi->pSimTime->fTimeZone;

	T	= xpn->pCl->pWeather->fTempAir;
	RH	= xpn->pCl->pWeather->fHumidity;
    //AS: use current solar radiation for sub daily model (case 0) and daily average for daily model (case 1)
    // Rs = xpn->pCl->pWeather->fSolRad/24.0;
    switch(version) {
        case 0: {Rs = xpn->pCl->pWeather->fSolRad/24.0; break;}
        case 1: {Rs = self->weather.fDaylySolRad/24.0; break;} 
    }


	latitude = xpn->pLo->pFarm->fLatitude * M_PI/180.0; //rad eqn 49
	elevation = xpn->pLo->pFarm->fAltitude;
	measurementheight = xpn->pCl->fWindMeasHeight;
	if(measurementheight == 0.0)
		{
			measurementheight = 2.0;
		}

	/* Get time */
	dT = xpn->pTi->pTimeStep->fAct;
	iJulianDay	= (int)xpn->pTi->pSimTime->fTimeY;	// actual julian day
	ftime_day = xpn->pTi->pSimTime->fTimeY;		// [s]->[day]
	h = (double)(((double)((double)ftime_day - (int)ftime_day))*24.0);


	if (Time_Zone==0.0)
		{
			Lz=0.0;
		}
	else if (Time_Zone<0.0)
		{
			Lz = -(Time_Zone)/24.0 * 360.0;
		}
	else
		{
			Lz = 360.0 - Time_Zone/24.0 * 360.0;
		}


    // AS: added calculation of actual vapor pressure according to "Penman Monteith (FAO)" ETpot model
	/*
	//Saturation Vapor Pressure (es)
	es = 0.6108*exp(17.27*T/(T+237.3)); //eqn. 37, [kPa/°C]

	//Actual Vapor Pressure (ea)
	//Method nr. 3 in table 4, eqn 37, 41, [kPa/°C]
	//using rel. humidity and temperature at the hour
	ea = RH/100.0 * es;
    xpn->pCl->pWeather->ea = ea;
    */
    switch(version) {
        case 0: { // original version as above, suitable for sub-daily resolution
            es = 0.6108*exp(17.27*T/(T+237.3)); //eqn. 37, [kPa/°C]
            //Actual Vapor Pressure (ea)
            //Method nr. 3 in table 4, eqn 37, 41, [kPa/°C]
            //using rel. humidity and temperature at the hour
            ea = RH/100.0 * es;
            //xpn->pCl->pWeather->ea = ea;
            break;
        }
        case 1: { // as in "Penman Monteith (FAO)", sutiable for daily resolution
            // saturaion vapor pressure:
            es_Tmax = 0.611*exp(17.27*self->weather.fTempMax / (self->weather.fTempMax+237.3));
            es_Tmin = 0.611*exp(17.27*self->weather.fTempMin / (self->weather.fTempMin+237.3));   
            // actual vapor pressure:
            ea = 2.0*self->weather.fDailyHumidity*0.01/(1.0/es_Tmin + 1.0/es_Tmax);
            //xpn->pCl->pWeather->ea = ea;
            break;
        }
    }

	//Extraterrestrial radiation (Ra)
	soldec = 0.409*sin(2*M_PI/365.0*iJulianDay-1.39); //solar declination [radians]
	t = h;
	b = 2.0*M_PI*((double)iJulianDay-81.0)/364.0;						//eqn 58
	Sc = 0.1645*sin(2.0*b)-0.1255*cos(b)-0.025*sin(b);	//eqn 57, seasonal correction factor
	Lm = -xpn->pLo->pFarm->fLongitude;
	w  = M_PI/12.0 *((t+0.06667*(Lz-Lm)+Sc)-12.0);								//eqn 55, solar time angle at midpoint of period

	// solar time angle at beginning and end of each period
	w1 = w - M_PI*dT; //eqn 53
	w2 = w + M_PI*dT; //eqn 54
	ws = acos(-tan(soldec)*tan(latitude));
	//if(w1 < -ws) w1 = -ws;  //conditions eqn 56
	//if(w2 < -ws) w2 = -ws;
	//if(w1 > ws) w1 = ws;
	//if(w2 > ws) w2 = ws;
	//if(w1 > w2) w1 = w2;

	dr = (1.0+0.033*cos(2.0*M_PI/365.0*(double)iJulianDay)); //eqn 50, inverse relative distant factor (squared) for the earth-sun [-]

	//if( dT < 1.0)
	{

		// Ra: MJ/(m2 h)
		//AS: exchanged for pointer
		//Ra = 12.0/M_PI * 4.92 *dr*((w2-w1)*sin(latitude)*sin(soldec)+cos(latitude)*cos(soldec)*(sin(w2)-sin(w1)));		// [MJ/m2*dt]		//self->Ra_h = 12.0/M_PI * 4.92 *dr*((w2-w1)*sin(latitude)*sin(soldec)+cos(latitude)*cos(soldec)*(sin(w2)-sin(w1)));		// [MJ/m2*h], orig eq. 48
		//Ra = *Ra/(dT*24.0);
		*Ra = 12.0/M_PI * 4.92 *dr*((w2-w1)*sin(latitude)*sin(soldec)+cos(latitude)*cos(soldec)*(sin(w2)-sin(w1)));		// [MJ/m2*dt]		//self->Ra_h = 12.0/M_PI * 4.92 *dr*((w2-w1)*sin(latitude)*sin(soldec)+cos(latitude)*cos(soldec)*(sin(w2)-sin(w1)));		// [MJ/m2*h], orig eq. 48
		*Ra = *Ra/(dT*24.0);
	}
	/*else
	{
		Ra = 24.0/M_PI * 4.92 *dr*(w*sin(latitude)*sin(soldec)+cos(latitude)*cos(soldec)*sin(w));	// [MJ/m2*day]
	}*/

	//if (Ra < 0.0) AS: exchanged for pointer
	if (*Ra < 0.0)
		{
			//*Ra = 0.0; AS: exchanged for pointer
			*Ra = 0.0;
		}

    // AS: calculation of daily Ra as done in "Penman Monteith (FAO)"
    switch(version){
        case 0: {/* Ra is used as calculated above*/ break;}
        case 1: {/*daily Ra is calculated as in "Penman Monteith (FAO)"*/
            fJ = (double)xpn->pTi->pSimTime->fTimeY;
            fdr = (double)1 + (double)0.033*(double)cos((double)0.0172*fJ);	       //equation (21
            fdelta = (double)0.409*(double)sin((double)0.0172*fJ - (double)1.39);  //equation (22)
            fphi   = xpn->pLo->pFarm->fLatitude*(double)PI/(double)180;

            ftemp = -(double)tan(fphi)*(double)tan(fdelta);					   //equation (20)
            if(     ftemp <= (double)-1)	fomegas = (double)PI;
            else if(ftemp >= (double)1 )	fomegas = (double)0;
            else						fomegas = (double)acos(ftemp);
            // Ra in [MJ/m2/d]
            *Ra = (double)37.6*fdr*(fomegas*(double)sin(fphi)*(double)sin(fdelta)+(double)cos(fphi)*(double)cos(fdelta)*(double)sin(fomegas)); //equation (19)
            *Ra = *Ra/24; // [MJ/m2/d] -> [MJ/m2/h]
            break;
        }
    }
    // end of AS


	// day light hours:
	N = 24.0/M_PI * ws;

	// middle of day:
	mid_of_day = 24.0/2.0 - (Lz-Lm)/360.0*24.0;

	// dusk time:
	dusk_time = mid_of_day + N / 2.0;
	dawn_time = mid_of_day - N / 2.0;
	if (dusk_time >= 24.0)
		{
			dusk_time-=24.0;
		}
	if (dawn_time < 0.0)
		{
			dawn_time+=24.0;
		}
    //AS: Solar elevation angle
    sol_elev1 = asin( sin(latitude)*sin(soldec)+cos(latitude)*cos(soldec)*cos(w1) ); // at the beginning of the timestep
    sol_elev2 = asin( sin(latitude)*sin(soldec)+cos(latitude)*cos(soldec)*cos(w2) ); // at the end of the timestep
	/*time_for_night_rad1 = dusk_time-3.5;
	time_for_night_rad2 = dusk_time-2.5;*/
	time_for_night_rad1 = dawn_time;
	time_for_night_rad2 = dusk_time;
	if (time_for_night_rad1 < 0.0)
		{
			time_for_night_rad1+=24.0;
		}
	if (time_for_night_rad2 < 0.0)
		{
			time_for_night_rad2+=24.0;
		}

	if ((h >= time_for_night_rad1) && (h <= time_for_night_rad2))
		{
			/*if (self->night_count==0)
			{
				self->night_rad=0.0;
			}*/
			self->night_rad+=Rs*dT;
			self->night_time+=dT;
			self->night_count++;
		}
	else
		{
			if (self->night_count>0)
				{
					//self->night_rad/=(double)self->night_count;
					self->night_rad/=self->night_time;
					if  (self->night_rad!=0.0)
						{
							self->night_rad2 = self->night_rad;
						}
					self->night_count=0;
					self->night_time=0.0;
					self->night_rad=0.0;
				}
			else
				{

				}
		}



	//Clear-sky solar radiation Rs0
	// Rs0 = (0.75+2.0e-5*elevation)*Ra; //eqn 47 //AS: exchanged Ra for pointer *Ra
	Rs0 = (0.75+2.0e-5*elevation)**Ra; //eqn 47


	C_DEBUG(Rs0)
	C_DEBUG(Rs)


	Rs_Rs0 = Rs/Rs0;
	if(Rs_Rs0 < 0.3)
		{
			Rs_Rs0 = 0.3;
		}
	else if(Rs_Rs0 > 1.0)
		{
			Rs_Rs0 = 1.0;
		}



	//cloudiness factor
	//if(Rs0>0.0)
	if((sol_elev1>0.3) && (sol_elev2>0.3)) //AS: changed to procedure described in ASCE(2005)	
		{
			fcd= 1.35*Rs_Rs0 -0.35; //eqn 45
			self->night_count_2 = 0; //AS: used to determine first timestep of nighttime
		}
	else
		{
			double Ra_noon;
			double Rs0_noon;

			//Rs = self->night_rad2;

			//During Nighttime Rs0 is zero => eqn 45 is undefined.
			//therefore fcd is used from another period (midday) having sufficient sun angle
			//t = 12.5;								// noon
			t = (time_for_night_rad2 - time_for_night_rad1)/2.0+time_for_night_rad1;
			w  = M_PI/12.0 *(t +Sc-12.0);
			w1 = w - M_PI*dT;						// /24.0; //eqn 53
			w2 = w + M_PI*dT;						// /24.0; //eqn 54
			ws = acos(-tan(soldec)*tan(latitude));

			/*if(w1 < -ws) w1 = -ws;  				//conditions eqn 56
			if(w2 < -ws) w2 = -ws;
			if(w1 > ws) w1 = ws;
			if(w2 > ws) w2 = ws;
			if(w1 > w2) w1 = w2;*/

			//b = 2.0*M_PI*(iJulianDay-81.0)/364.0;
			//Sc = 0.1645*sin(2.0*b)-0.1255*cos(b)-0.025*sin(b);//seasonal correction factor
			//dr = (1.0+0.033*cos(2.0*M_PI/365.0*iJulianDay)); //eqn 50
			Ra_noon = 12.0/M_PI * 4.92 *dr*((w2-w1)*sin(latitude)*sin(soldec)+cos(latitude)*cos(soldec)*(sin(w2)-sin(w1)));		// [MJ/m2*dt]
			Ra_noon = Ra_noon/(dT*24.0);
			Rs0_noon = (0.75+2.0e-5*elevation)*Ra_noon;
			fcd = 1.35*self->night_rad2/Rs0_noon-0.35;

	        // AS: Trying to fix fcd for the night, which is too low, when calculated as above.
	        if(self->night_count_2 == 0) { //At first timestep of nighttime get the last fcd calculated at daytime
	            self->fcd_last = self->cloud_cover_factor;
	        }
	        fcd = self->fcd_last; //Use last fcd calculated at daytime as current fcd
	        self->night_count_2++; //Don't reload last fcd calculated for the next timestep at nighttime
    }

	fcd = MAX(fcd,0.05);
	fcd = MIN(fcd,1.0);

	self->cloud_cover_factor = fcd;



	//Net Solar or Net Short-Wave Radiation (Rns )
	//Rns = Rs - 0.23 *Rs; //eqn 43, albedo
	*Rns = Rs - xpn->pSo->fAlbedo *Rs; //eqn 43, albedo

	//Net Outgoing Long-Wave Radiation (Rnl )
	*Rnl = 2.042*1e-10*fcd*(0.34-0.14*sqrt(ea))*pow(T+273.16,4.0); //eqn 44

	//Net Radiation (Rn )

	*Rns = *Rns / 3600.0*1.0e6; // MJ m-2 -h -> W m-2
	*Rnl = *Rnl / 3600.0*1.0e6; // MJ m-2 -h -> W m-2

	Rn = *Rns - *Rnl;

    *Ra = *Ra/3600.0*1.0e6; // MJ m-2 -h -> W m-2
	return Rn;
}

//AS: soil surface resistance models

int rs_anadranistakis_run(evapotranspiration* self) { //Anadranistakis (2000)
    expertn_modul_base *xpn = &(self->parent);
    PSWATER pSW = xpn->pSo->pSWater;
    self->rss = 100*(2.5*pSW->fContFK/xpn->pWa->pWLayer->pNext->fContAct-1.5);
    return RET_SUCCESS;
}

int rs_VanDeGriend_load(evapotranspiration* self) { //Van de Griend and Owe (1994)
    expertn_modul_base *xpn = &(self->parent);
	PSWATER pSW = xpn->pSo->pSWater;
	PWLAYER pWL = xpn->pWa->pWLayer;
    self->WCont = xpn_register_var_get_pointer(xpn->pXSys->var_list,"hydraulic_fuctions.WCont");
    self->MPotl = xpn_register_var_get_pointer(xpn->pXSys->var_list,"hydraulic_fuctions.MPotl");
    //double a = xpn_register_var_get_pointer_convert_to_double(xpn->pXSys->var_list, "Config.Van de Griend and Owe (1994).a (empirical parameter 0<=a<=1)",0.7);
     self->aVanDeGriend = xpn_register_var_get_pointer_convert_to_double(xpn->pXSys->var_list, "Config.Van_de_Griend_and_Owe.aVanDeGriend",0.7);
    double pot_aev = 1/pSW->fVanGenA*1000*9.81; //water tension at air entry value
    double pot_res = -MATRIX_POTENTIAL(pSW->fContRes); //residual water tension
//    self->cont_ERRP = WATER_CONTENT(-pow(pot_res,a)*pow(pot_aev,1-a));
    self->cont_ERRP = WATER_CONTENT(-pow(pot_res,self->aVanDeGriend)*pow(pot_aev,1-self->aVanDeGriend));
 //   double pot_res = -20000; //residual water tension
 //   self->cont_ERRP = 1.;
    return RET_SUCCESS;
}

int rs_VanDeGriend_run(evapotranspiration* self) { //Van de Griend and Owe (1994)
    expertn_modul_base *xpn = &(self->parent);
    self->rss = 10*exp(0.3563*(self->cont_ERRP*100 - xpn->pWa->pWLayer->pNext->fContAct*100));
    return RET_SUCCESS;
}

int rs_CamilloGurney_run(evapotranspiration* self) { //Camillo and Gurney (1986)
    expertn_modul_base *xpn = &(self->parent);
    PSWATER pSW = xpn->pSo->pSWater;
    self->rss = -806+4140*(pSW->fContSat-xpn->pWa->pWLayer->pNext->fContAct);
    self->rss = max(0,self->rss);
    return RET_SUCCESS;
}

int rs_Sun82_run(evapotranspiration* self) { //Sun (1982)
    expertn_modul_base *xpn = &(self->parent);
    PSWATER pSW = xpn->pSo->pSWater;
    self->rss = 3.5*pow(pSW->fContSat/xpn->pWa->pWLayer->pNext->fContAct,2.3)+33.5;
    return RET_SUCCESS;
}

int rs_Sun98_run(evapotranspiration* self) { //Sun et al. (1998)
    expertn_modul_base *xpn = &(self->parent);
    PSWATER pSW = xpn->pSo->pSWater;
    self->rss = exp(8.2-4.225*xpn->pWa->pWLayer->pNext->fContAct/pSW->fContSat);
    return RET_SUCCESS;
}