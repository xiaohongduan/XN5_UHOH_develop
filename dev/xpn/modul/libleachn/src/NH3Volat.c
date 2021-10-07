//     NH3Volat.c
//
//  
//


//#include "stickstoff.h"
//#include <glib.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <math.h>
//
//  /********************************************************************************************/
//**********************************************************************************************/
/* Procedur    :NVolat1()                                                                                 */
/*                                                                                                                         */
/* Beschreibung:   Ammoniak Verfluechtigung                                          */
/*                 Methode LEACHM 3.0 (Hutson & Wagenet, 1992)               */
/*                                                                                                                        */
/*                  ep/gsf 15.03.94                                                                          */
/*					ch/gsf 22.07.96	  Betrachte ROrgNH3 als kumul. Groesse.       */
/*								      => Verwertbar in Bilanzierung.                           */  
/*								                                                                                        */
//*********************************************************************************************/
/* veränd. Var.		pCh->pCLayer->pNext->fNH4N                                      */
/*					pCh->pCProfile->fNH3VolatR                                                 */
/*					                                                                                                     */
//**********************************************************************************************/
//* XN5: SG 26.06.2020                                                                                   */
//**********************************************************************************************/


#include "stickstoff.h"
#include "stickstoff_macros.h"

int leachn_NH3Volat1_load(stickstoff *self)
{
/*    expertn_modul_base *xpn = &(self->parent);
    stickstoff_load_ini(self);
    
    // Hydraulische Funktionen laden:
	self->WCont = xpn_register_var_get_pointer(xpn->pXSys->var_list,"hydraulic_fuctions.WCont");
	self->HCond = xpn_register_var_get_pointer(xpn->pXSys->var_list,"hydraulic_fuctions.HCond");
	self->DWCap = xpn_register_var_get_pointer(xpn->pXSys->var_list,"hydraulic_fuctions.DWCap");
	self->MPotl = xpn_register_var_get_pointer(xpn->pXSys->var_list,"hydraulic_fuctions.MPotl");
    
	if ((self->WCont==NULL) || (self->HCond==NULL) || (self->DWCap==NULL) || (self->MPotl==NULL))
		{
			PRINT_ERROR("Problem with hydraulic functions!");
		}*/
    
    expertn_modul_base *xpn = &(self->parent);
	//PCHEMISTRY pCh = xpn->pCh;
	//double NH3VolatMaxR;
//	PTIME pTi = xpn->pTi;
	
	// in Expert-N 3.0 xnm File Marker: 20013
	//Max. NH3-N Volatilisation | NH4-N Deposition | NO3-N Deposition
	// [1/d]            |   [kg N /ha /a]  |   [kg N /ha /a] 
//	----------------------------------------------------------------------
//20013
// 5.e-002				12.				6.				

	// kg N /ha / a
//	NH3VolatMaxR = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.volatilization.  ",0.1);

/*	// kg N /ha / a
	fNO3NImisRa = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.Constant Deposition.no3",0.0);

	// kg N /ha / a -> kg N /ha / d
	pCh->pCProfile->fNH4NImisR = fNH4NImisRa / 365.25;
	
	// kg N /ha / a -> kg N /ha / d
	pCh->pCProfile->fNO3NImisR = fNO3NImisRa / 365.25;*/

//	pCh->pCProfile->fNH3VolatMaxR = NH3VolatMaxR;

    
    return RET_SUCCESS;
}

int leachn_NH3Volat1_run(stickstoff *self)
{
    expertn_modul_base *xpn = &(self->parent);
    PCHEMISTRY pCh = xpn->pCh;
    PSPROFILE pSo = xpn->pSo;
    PWATER pWa = xpn->pWa;
    PTIME pTi = xpn->pTi;

/*    PHEAT pHe = xpn->pHe ;*/
 
    double f1;

/*    PSLAYER    pSL;
    PSWATER    pSW;
    PWLAYER    pWL;
    PHLAYER    pHL;
    PCLAYER    pCL;*/

/*  float f1;*/

  //fNH3VolMaxR = (float)0.1;   // orig Leach:0.4;
  							//  0.0 - 0.3  Jemison et al 1994
  
  //************** Volatilisation ***************/
   
    //  Berechnung erfolgt fuer Schicht 1

    f1       = max(EPSILON,
    				pWa->pWLayer->pNext->fContAct
             		+ pSo->pSLayer->pNext->fBulkDens * pCh->pCParam->afKd[1]);

// unnötig:    f2       = pSo->pSLayer->pNext->fThickness * (float)0.01;
    
    pCh->pCProfile->fNH3VolatR  = min(((float)0.5 * pCh->pCLayer->pNext->fNH4N / pTi->pTimeStep->fAct),
                   (pCh->pCProfile->fNH3VolatMaxR * pCh->pCLayer->pNext->fNH4N))
// betrachtet wird der gelöste Anteil des NH4:
             		* pWa->pWLayer->pNext->fContAct /f1; 
               
    pCh->pCLayer->pNext->fNH4N   -= pCh->pCProfile->fNH3VolatR * pTi->pTimeStep->fAct;

  return 0;
}


int leachn_NH3Volat1_done(stickstoff *self)
{
    G_FREE_IF_NOT_0(self->ini_filename);
    return RET_SUCCESS;

}
