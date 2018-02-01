//      balance.c
//
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//


#include "balance.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

G_DEFINE_TYPE(balance, balance, EXPERTN_MODUL_BASE_TYPE);


static void balance_class_init(balanceClass *klass) {}


static void balance_init(balance *self)
{
	self->fCumRain=0.0;
	self->fPotDay=0.0;
	self->fPotCumEvap=0.0;
	self->fRunOffDay=0.0;
	self->fRunOffCum=0.0;
	self->fLeaching=0.0;
	self->fCumLeaching=0.0;
	self->fBalance=0.0;
	self->fCumBalance=0.0;	
	self->fWaterStorage = 0.0;
	self->fWaterStoragedt=0.0;
	self->fWaterInput = 0.0;
	self->fWaterOutput = 0.0;
	self->fActETCum = 0.0;
	self->fProfil = 0.0;
	self->fProfilStart = 0.0;
	
	self->cum_dust1=0.0;
	self->cum_dust2=0.0;
	self->cum_dust3=0.0;
	self->cum_dust4=0.0;
}


int balance_load(balance *self)
{	
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->fCumRain,"output.Water.Water Balance.Cum Rain [mm]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->fCumLeaching,"output.Water.Water Balance.Cum Leaching [mm]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->fActETCum,"output.Water.Water Balance.Cum Evapotranspiration [mm]",0.0);	
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->fBalance,"output.Water.Water Balance.Balance [mm]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->fCumBalance,"output.Water.Water Balance.Cum_Balance [mm]",0.0);
	
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->parent.pCh->pCBalance->dNProfile,"output.Nitrogen.N Balance.Mineral N in Profile [kg N ha-1]",0.0);			
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->parent.pCh->pCBalance->dNInputCum,"output.Nitrogen.N Balance.Mineral N Input [kg N ha-1]",0.0);			
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->parent.pCh->pCProfile->dNUptakeCum,"output.Nitrogen.N Balance.Mineral N Uptake [kg N ha-1]",0.0);			
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->parent.pCh->pCProfile->dN2OEmisCum,"output.Nitrogen.N Balance.Gaseous N Emission [kg N ha-1]",0.0);			
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->parent.pCh->pCProfile->dNTotalLeachCum,"output.Nitrogen.N Balance.N Leaching [kg N ha-1]",0.0);			
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->parent.pCh->pCBalance->dNBalance,"output.Nitrogen.N Balance.Mineral N Balance [kg N ha-1]",0.0);		
	
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->fWaterStorage,"output.Water.Temporal Changes.Storage [mm]",0.0);
	
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->cum_dust1,"output.Atmos.Balance.Dust 1 [ug (kg dryair)-1]",0.0);		
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->cum_dust2,"output.Atmos.Balance.Dust 2 [ug (kg dryair)-1]",0.0);		
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->cum_dust3,"output.Atmos.Balance.Dust 3 [ug (kg dryair)-1]",0.0);		
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->cum_dust4,"output.Atmos.Balance.Dust 4 [ug (kg dryair)-1]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->cum_dust5,"output.Atmos.Balance.Dust 5 [ug (kg dryair)-1]",0.0);
	
	xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Heat.Energy Balance.Residual [W m-2]",&(self->parent.pHe->pHEBalance->fRes),-1,TRUE,TRUE);
	self->fWaterStoragedt = 0.0;
	self->waterstorage_init = 0.0;	
	return RET_SUCCESS;
}

int balance_run(balance *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PCHEMISTRY pCh = xpn->pCh;
	PCLIMATE pCl = xpn->pCl;
	//PHEAT pHe = xpn->pHe ;
	//PLOCATION pLo = xpn->pLo;
	//PMANAGEMENT pMa = xpn->pMa;
	PPLANT pPl = xpn->pPl;
	PSPROFILE pSo = xpn->pSo;
	PTIME pTi = xpn->pTi;
	PWATER pWa = xpn->pWa;
	PCLAYER		pCL;
	PCPROFILE	pCP;
	PCBALANCE	pCB;
	PSLAYER		pSL;
	PWLAYER		pWL;
	PSWATER  pSW;
	
	double dt;
	int iLayer;
	double fProfile;
	double fProfileIce;
	double fFluxDay;
	double fSimDepth;
//	int i;
	/*********************************************************************************/
	/*                                                                               */
	/*                                Wasserbilanz                                   */
	/*                                                                               */
	/*********************************************************************************/

	/********************************* Kumulation ************************************/

//float bal_old_h2o, f1;

// pEV = pWa->pEvap;
// pWB = pWa->pWBalance;

	/* Weather: */
//if (NewDay(pTi)&& pCl->pWeather->fRainAmount > EPSILON)

	dt= pTi->pTimeStep->fAct;

	self->fCumRain += pCl->pWeather->fPreciRate*dt;
	
	//Water Output
	//pWa->fLeaching    += pWa->fPercolR * DeltaT;
	self->fCumLeaching += pWa->fPercolR * dt;
	self->fActETCum += pWa->fActETR*dt;
	self->fWaterOutput = self->fActETCum + self->fCumLeaching;	
	
	//Water Input
	if (pWa->fInfiltR > (double)0.0)
		self->fWaterInput += pWa->fInfiltR*dt;
	
	//Water Storage
	self->fWaterStorage = 0.0;
	self->fProfil = 0.0;
	for (pSL = pSo->pSLayer->pNext,pSW = pSo->pSWater->pNext,pWL = pWa->pWLayer->pNext,iLayer = 1;((pSL->pNext != NULL)&&(pSW->pNext != NULL)&&(pWL->pNext != NULL));pSL = pSL->pNext,pSW = pSW->pNext,pWL = pWL->pNext,iLayer++)
{
		self->fWaterStorage += (pWL->fContAct-pWL->fContOld) * pSL->fThickness;
		self->fProfil += pWL->fContAct * pSL->fThickness;
}
	if (pTi->pSimTime->bFirstRound==1)
		{
			self->waterstorage_init = self->fWaterStorage;
			self->fProfilStart = self->fProfil;
		}

	self->fWaterStoragedt = self->fWaterStorage*dt;

	self->fBalance = self->fWaterStorage - self->waterstorage_init  + pWa->fPercolR - pCl->pWeather->fPreciRate;

	self->fCumBalance = self->fWaterOutput-self->fWaterInput+self->fProfil-self->fProfilStart;

	fProfile = 0.0;
	fProfileIce = 0.0;
	fFluxDay = 0.0;
	for (pSL = pSo->pSLayer->pNext,pSW = pSo->pSWater->pNext,pWL = pWa->pWLayer->pNext,iLayer = 1;((pSL->pNext != NULL)&&(pSW->pNext != NULL)&&(pWL->pNext != NULL));pSL = pSL->pNext,pSW = pSW->pNext,pWL = pWL->pNext,iLayer++)
	{
	fProfile    += pWL->fContAct * pSL->fThickness;
	fProfileIce += pWL->fIce * pSL->fThickness / 1.1;
	fFluxDay += pWL->fFluxDens  * dt;
	}


	/*********************************************************************************/
	/*                                                                               */
	/*                                Stickstoffbilanz                               */
	/*                                                                               */
	/*********************************************************************************/

	pCL = pCh->pCLayer;
	pCP = pCh->pCProfile;
	pCB = pCh->pCBalance;	

	fSimDepth=(double)0.0;

	for(pCL = pCh->pCLayer->pNext;pCL->pNext!=NULL;pCL=pCL->pNext)
	{
		
	  fSimDepth+=pSL->fThickness;

	  /***************************** Mineralisierung ***********************************/

//Added by Hong on 20180124, for consistence in daisy_miner.c
      //pCP->dNMinerCum  += (pCL->fLitterMinerR + pCL->fManureMinerR + pCL->fHumusMinerR) * dt;     
     pCP->dNMinerCum  += (pCL->fMinerR) * dt;   
//End of Hong    

      pCP->dNImmobCum  += pCL->fNImmobR * dt;
	     
	  if(fSimDepth <= (float)300) 
		  //pCP->dNetNMinerCum30 += (pCL->fLitterMinerR + pCL->fManureMinerR + pCL->fHumusMinerR - pCL->fNLitterImmobR) * dt;
		  pCP->dNetNMinerCum30 += (pCL->fMinerR - pCL->fNLitterImmobR) * dt; //Added by Hong on 20180124, for consistence in daisy_miner.c
	}

/******************************* Volatisation ************************************/
      pCP->dNH3VolatCum += pCP->fNH3VolatR * dt;  

/******************************* N2O-Emission ************************************/
	if (pCP->fN2OEmisR > 0)	   
     {
      pCP->dN2OEmisCum += pCP->fN2OEmisR  * (float)24e-5 * dt;
	 }

	pCP->dNTotalLeachCum = pCP->dUreaLeachCum + pCP->dNH4LeachCum + pCP->dNO3LeachCum + pCP->dN2ODrainCum;      
//  fN2ODrainCum wird im Transport-Modell berechnet.

/******************************* Pflanzen-N-Entzug  ************************************/

	if (pPl != NULL)
	{
	pCP->dNUptakeCum += pPl->pPltNitrogen->fActNUptR * dt;
	}

	/****************************** Bilanzierung *************************************/
  
	// Profilsumme Null setzen
	pCB->fNO3NProfile  =  (double)0.0;
	pCB->fNH4NProfile  = (double)0.0;
	pCB->fN2ONProfile  = (double)0.0;
	pCB->fN2NProfile   = (double)0.0;
	pCB->fNONProfile = (double)0.0;
	pCB->dNProfile  = (double)0.0;
	pCB->fUreaNProfile = (double)0.0;

	pCL = pCh->pCLayer; //eingefügt, da sonst Programm nicht läuft (warum?)
	
	for (SOIL_LAYERS1(pCL,pCL->pNext))
	{
      pCB->fNO3NProfile 	+= pCL->fNO3N;
      pCB->fNH4NProfile 	+= pCL->fNH4N;
      pCB->fN2ONProfile 	+= pCL->fN2ON;
      pCB->fN2NProfile  	+= pCL->fN2N;
	  pCB->fNONProfile  	+= pCL->fNON; 
      pCB->fUreaNProfile    += pCL->fUreaN;
     }


	//Gesamtstickstoff im Profil
	pCB->dNProfile = pCB->fNO3NProfile + pCB->fNH4NProfile + pCB->fN2ONProfile + pCB->fN2NProfile + pCB->fNONProfile + pCB->fUreaNProfile;

	//if (pCB->dNProfile>1000.0)
	//printf("%f %f %f %f %f %f \n", pCB->fNO3NProfile,pCB->fNH4NProfile,pCB->fN2ONProfile,pCB->fN2NProfile,pCB->fNONProfile,pCB->fUreaNProfile);

	if (pTi->pSimTime->bFirstRound==1)
		{
			pCB->fNProfileStart = pCB->dNProfile;
		}

	pCB->dNOutputCum = pCP->dN2OEmisCum + pCP->dNH3VolatCum + pCP->dNUptakeCum + pCP->dNImmobCum - pCP->dNMinerCum + pCP->dNTotalLeachCum;

	pCB->dNBalCorrect = -pCP->dN2OEmisCum;

	pCB->dNBalance = pCB->dNProfile + pCB->dNOutputCum - pCB->fNProfileStart - pCB->dNInputCum - pCB->dNBalCorrect;// Was ist das? - pCB->dNBalCorrect;
	
	// Atmosphere:
	self->cum_dust1+=xpn->pCh->pAtmos->dustr1*dt;
	self->cum_dust2+=xpn->pCh->pAtmos->dustr2*dt;
	self->cum_dust3+=xpn->pCh->pAtmos->dustr3*dt;	
	self->cum_dust4+=xpn->pCh->pAtmos->dustr4*dt;
	self->cum_dust5+=xpn->pCh->pAtmos->dustr5*dt;
	
	// Energy Balance:
	xpn->pHe->pHEBalance->fRes = xpn->pHe->pHEBalance->fNetRad - xpn->pHe->pHEBalance->fHeatLatent + xpn->pHe->pHEBalance->fGroundHeat - xpn->pHe->pHEBalance->fHeatSens;	
	
	return RET_SUCCESS;
}
