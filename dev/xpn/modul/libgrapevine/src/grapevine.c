//      grapevine.c
//      
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//      


#include "grapevine.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

G_DEFINE_TYPE(grapevine, grapevine, EXPERTN_MODUL_BASE_TYPE);


static void grapevine_class_init(grapevineClass *klass){}


static void grapevine_init(grapevine *self)
{
//gchar *S;
// hier könnnen die Standartwerte der Variablen gesetzt werden oder Speicher reserviert werden!
// Vorsicht: Die Daten der Expert N Basis Klasse sind hier noch nicht gesetzt!
// Werden diese beim Initialisieren benoetigt, so muss eine eigene Init dafuer registriert werden!

// init example var:
//self->Plant_Mass = 20.0;
// print Output Message if Modul is used from Expert N
//S  = g_strdup_printf("%s\t%d\tInit MODUL GRAPEVINE!",__FILE__,__LINE__);
//fprintf(stdout,"%s\n",S);
//fflush(stdout);
//g_free(S);

self->__LOAD_DONE=0;
self->__DONE_DONE=0;

}



// Our Modul Functions:

int grapevine_load(grapevine *self)
  {
		if (self->__LOAD_DONE!=0) 
		{
			return RET_SUCCESS;
		}
		self->__LOAD_DONE=1;
		
		expertn_modul_base *xpn = EXPERTN_MODUL_BASE(self);
		PPLANT pPl = xpn->pPl;
		PSIMTIME pST = xpn->pTi->pSimTime;
		PSTAGEPARAM		pSt = pPl->pGenotype->pStageParam;
		
		int i;
		
		pPl->pDevelop->fCumVernUnit = 0.0;
		pPl->pGenotype->fTempBase = 10.0; //later: read from input file or from GUI
		pPl->pGenotype->fTempMax = 25.0; //later: read from input file or from GUI
		pPl->pDevelop->iStageCERES = 9; //later: read from input file or from GUI
		
		self->fQ10c = 2.17; //later: read from input file or from GUI
		
		pPl->pGenotype->fOptVernDays = 30.0;
		self->iDOYDormCalcBegin = 32;
		
		self->iDOYDormCalcLatest = 60;
		self->iDOYHarvest = 100;
		self->iDOYDormBreakLatest = 240;
	
		double ThermalTime[]= {350.0,    420.0,    -99.9,  -99.9,  -99.9,   400.0,   600.0,   200.0,   200.0,   200.0};
		pSt->afThermalTime = g_malloc0(sizeof(double)*10);
		for (i=0; i<10; i++)
		{
			pSt->afThermalTime[i]=(double)ThermalTime[i];
		}
		
		self->fTempSumDormancy = 350.0;
		self->fTempSumVeraison = 420;
		
		// Start of dormancy break calculation -> dormancy break start: start of budburst calculation -> budburst: start of plant growth calculation
		// 1. Dormancy break calculation: bMaturity = TRUE and bPlantGrowth = TRUE
		// 2. Budburst calculation: bMaturity = TRUE and bPlantGrowth = FALSE -> plant is dormant
		// 3. Plant growth calculation: bMaturity = FALSE and bPlantGrowth = TRUE
		// first decision via typical date
		pPl->pDevelop->bMaturity = FALSE; // TRUE from start of dormancy break calculation until budburst
		pPl->pDevelop->bPlantGrowth = FALSE; // TRUE until dormancy break start and from budburst
		self->bDormancyThisYear  = TRUE;
		
		if ((pST->fTimeY >= (double)self->iDOYDormBreakLatest) || (pST->fTimeY <= (double)self->iDOYHarvest))
		{
			pPl->pDevelop->bPlantGrowth = TRUE;
		}
		
		if ((pST->fTimeY <= (double)self->iDOYDormBreakLatest) && (pST->fTimeY >= (double)self->iDOYDormCalcLatest))
		{
			pPl->pDevelop->bMaturity = TRUE;
		}
		
		return RET_SUCCESS;
  }
  
  int grapevine_done(grapevine *self)
  {
		if (self->__DONE_DONE!=0) 
		{
			return RET_SUCCESS;
		}
		self->__DONE_DONE=1;
		
		expertn_modul_base *xpn = EXPERTN_MODUL_BASE(self);
		PPLANT pPl = xpn->pPl;
		PSTAGEPARAM		pSt = pPl->pGenotype->pStageParam;	
		
		g_free(pSt->afThermalTime);
		
		return RET_SUCCESS;
  }
  
  int grapevine_phenological_development(grapevine *self)
  {
	  // Adaptations from
      // Garcia de Cortazar-Atauri et al. 2009: Performance of several models for predicting budburst date of grapevine (Vitis vinifera L.)
	  // Andreoli et al. 2019: Description and Preliminary Simulations with the Italian Vineyard Integrated Numerical Model for Estimating Physiological Values (IVINE)
	  
	   expertn_modul_base *xpn = EXPERTN_MODUL_BASE(self);
       PPLANT pPl = xpn->pPl;
	   PCLIMATE pCl = xpn->pCl;
	   PTIME pTi = xpn->pTi;
	   PSIMTIME pST = pTi->pSimTime;
	   PTIMESTEP	pTS	=pTi->pTimeStep;
	  
	  pPl->pDevelop->fVernUnit = 0.0;
	  pPl->pDevelop->fDTT = 0.0;
	  
	  // Calculation of dormancy start
	  //if ((pPl->pDevelop->bPlantGrowth == TRUE) && (pPl->pDevelop->bMaturity == TRUE))
	  if (pPl->pDevelop->iStageCERES >= 7)
		{
		pPl->pDevelop->fVernUnit = pow(self->fQ10c,-pCl->pWeather->fTempAir/10.0);
		pPl->pDevelop->fCumVernUnit += pPl->pDevelop->fVernUnit * pTS->fAct;
		}
	  
	  //Phenology
	  if (pPl->pDevelop->iStageCERES < 9)
	  {
		if(pCl->pWeather->fTempAir < pPl->pGenotype->fTempBase)
		{
		  pPl->pDevelop->fDTT = 0.0;
		}
		else if (pCl->pWeather->fTempAir >= pPl->pGenotype->fTempMax)
		{
		  pPl->pDevelop->fDTT = pPl->pGenotype->fTempMax - pPl->pGenotype->fTempBase;
		}
		else
		{
		  pPl->pDevelop->fDTT = pCl->pWeather->fTempAir - pPl->pGenotype->fTempBase;
		}
		pPl->pDevelop->fCumDTT += pPl->pDevelop->fDTT * pTS->fAct;
	  }
	  
	  // Set value(s) to 0 on harvest day
	  if ((pST->fTimeY >= (double)self->iDOYHarvest) && (pPl->pDevelop->iStageCERES < 9) && (pPl->pDevelop->iStageCERES > 0) )
	  {
		  printf("Harvest %d\n", pPl->pDevelop->iStageCERES);
		  pPl->pDevelop->bPlantGrowth = FALSE;
		  pPl->pDevelop->fCumDTT = 0.0;
		  pPl->pDevelop->iStageCERES = 9;
	  }
	  
	  // Start of Dormancy, reset the cumulative values
	  if((pPl->pDevelop->iStageCERES >= 9) && ((pPl->pDevelop->fCumVernUnit > pPl->pGenotype->fOptVernDays) || (pST->fTimeY >= (double)self->iDOYDormBreakLatest)))
	  {
		  //printf("%f \n", pST->fTimeY);
		  printf("Start of Dormancy %d\n", pPl->pDevelop->iStageCERES);
		  pPl->pDevelop->bPlantGrowth = TRUE;
		  pPl->pDevelop->bMaturity = FALSE;
		  pPl->pDevelop->fCumVernUnit = 0.0;
		  pPl->pDevelop->iStageCERES = -1;
	  }
	  
	  // Budburst: when a certain temperature sum is exceeded from start of dormancy onwards
	  if ((pPl->pDevelop->iStageCERES == -1) && (pPl->pDevelop->fCumDTT >= self->fTempSumDormancy))
	  {
		  printf("Budburst %d\n", pPl->pDevelop->iStageCERES);
		pPl->pDevelop->iStageCERES = 0;
	  }
	  
	  // Dormancy calculation (also add earliest and latest date) begins when a certain temperature sum is exceeded
	  if(((pPl->pDevelop->iStageCERES == 0) && (self->bDormancyThisYear == FALSE)) &&
	  (((pPl->pDevelop->fCumDTT >= self->fTempSumDormancy + self->fTempSumVeraison) && (pST->fTimeY >= (double)self->iDOYDormCalcBegin))
	  ||(pST->fTimeY >= (double)self->iDOYDormCalcLatest)))
	  {
		  printf("Start of Dormancy Calculation %d\n", pPl->pDevelop->iStageCERES);
		  pPl->pDevelop->bMaturity = TRUE;
		  self->bDormancyThisYear = TRUE;
		  pPl->pDevelop->iStageCERES = 7;
	  }
	  
	  //
	  if (((int)pST->fTimeY == 0) && (self->bDormancyThisYear == TRUE))
		{
		  self->bDormancyThisYear = FALSE;
		}
	  
	  return RET_SUCCESS;
  }


int grapevine_biomass_growth(grapevine *self)
  {
       expertn_modul_base *xpn = EXPERTN_MODUL_BASE(self);
       PPLANT pPl = xpn->pPl;
 
       self->Plant_Mass = 100.0; // Change Plant Mass
       pPl->pBiomass->fFruitWeight = self->Plant_Mass;
       

       return RET_SUCCESS;
  }

int grapevine_canopy_formation(grapevine *self)
  {
      // ...
      // do anything
      // ....
      return RET_SUCCESS;
  }

int grapevine_nitrogen_demand(grapevine *self)
  {
      // ...
      // do anything
      // ....
      return RET_SUCCESS;
  }
  
  int grapevine_nitrogen_uptake(grapevine *self)
  {
      // ...
      // do anything
      // ....
      return RET_SUCCESS;
  }
  
  int grapevine_potential_transpiration(grapevine *self)
  {
      // ...
      // do anything
      // ....
      return RET_SUCCESS;
  }
  
  int grapevine_actual_transpiration(grapevine *self)
  {
      // ...
      // do anything
      // ....
      return RET_SUCCESS;
  }
  
  int grapevine_photosynthesis(grapevine *self)
  {
      // ...
      // do anything
      // ....
      return RET_SUCCESS;
  }
  
  int grapevine_root_length_growth(grapevine *self)
  {
      // ...
      // do anything
      // ....
      return RET_SUCCESS;
  }
  
  int grapevine_crop_senescence(grapevine *self)
  {
      // ...
      // do anything
      // ....
      return RET_SUCCESS;
  }