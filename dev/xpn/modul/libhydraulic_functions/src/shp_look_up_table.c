#include "shp_look_up_table.h"
#include "hydraulic_functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "math_functions.h"


//static  PSPROFILE       pSo;
//extern PSPROFILE GetSoilPoi(void);

int hydraulic_functions_shp_table_load(hydraulic_functions *self)
//int hydraulic_functions_shp_table_load(expertn_standard_ini *self)
{
	 expertn_modul_base *xpn = &(self->parent);
	 PRINT_MESSAGE(xpn,5,"Hydraulic Function set to Look-up Table") ;
	 hydraulic_functions_set(self,&SHP_LT_WCont,&SHP_LT_HCond,&SHP_LT_DWCap,&SHP_LT_MPotl); 
	 
	shp_table_load_ini(self);
	shp_table_load_config(self);
    
   shp_table_set_soil(self); 
		
	return RET_SUCCESS;
}


int shp_table_set_soil(hydraulic_functions *self)
//int shp_table_set_soil(expertn_standard_ini *self)
{
	expertn_modul_base *xpn = &(self->parent);
/*    PSLAYER pSLayer;
    PSWATER pSWater;
  */  
    PSLAYER pSLayer;
     PSLAYER pSL;
	 PSWATER pSWater;
     PWLAYER pWL;
	 PSWATER      pSW;
     PWATER pWa = xpn->pWa;
     PSPROFILE pSo = xpn->pSo;
     pWL= xpn->pWa->pWLayer; 
	 pSW = xpn->pSo->pSWater;
	
     int iLayer; 
     int  i, i4,var_len; //SG 20180921
  
/*	for (pSLayer=xpn->pSo->pSLayer,pSWater=xpn->pSo->pSWater,i=0; pSLayer!=NULL; pSLayer=pSLayer->pNext,pSWater=pSWater->pNext,i++)
    {

             ///////////////////////////////////////////////////////////////////////////////////////////////////
            //SG 20180920:  creating response-tables for  soil hydraulic properties
            //so far only one table for all soil layers!
            var_len = array_length(self->shp_table_mat_pot_len,self->shp_table_wat_cont_len);
            pSWater->MatPot_2_WatCont = (RESPONSE*)g_malloc0_n(var_len,sizeof(RESPONSE));
            pSWater->MatPot_2_WatCont[0].iInLen = self->shp_table_mat_pot_len;
            pSWater->MatPot_2_WatCont[0].iOutLen = self->shp_table_wat_cont_len;
            for(i4=0; i4<var_len; i4++)
            {
                pSWater->MatPot_2_WatCont[i4].fInput = self->shp_table_mat_pot[var_len-i4-1];
                pSWater->MatPot_2_WatCont[i4].fOutput = self->shp_table_wat_cont[var_len-i4-1];
            }
             
            var_len = array_length(self->shp_table_wat_cont_len,self->shp_table_mat_pot_len);
            pSWater->WatCont_2_MatPot = (RESPONSE*)g_malloc0_n(var_len,sizeof(RESPONSE));
            pSWater->WatCont_2_MatPot[0].iInLen = self->shp_table_wat_cont_len;
            pSWater->WatCont_2_MatPot[0].iOutLen = self->shp_table_mat_pot_len;
            for(i4=0; i4<var_len; i4++)
            {
                pSWater->WatCont_2_MatPot[i4].fInput = self->shp_table_wat_cont[var_len-i4-1];
                pSWater->WatCont_2_MatPot[i4].fOutput = self->shp_table_mat_pot[var_len-i4-1];
            }
            
            var_len = array_length(self->shp_table_mat_pot_len,self->shp_table_h_cond_len);
            pSWater->MatPot_2_HCond = (RESPONSE*)g_malloc0_n(var_len,sizeof(RESPONSE));
            pSWater->MatPot_2_HCond[0].iInLen = self->shp_table_mat_pot_len;
            pSWater->MatPot_2_HCond[0].iOutLen = self->shp_table_h_cond_len;
            for(i4=0; i4<var_len; i4++)
            {
                pSWater->MatPot_2_HCond[i4].fInput = self->shp_table_mat_pot[var_len-i4-1];
                pSWater->MatPot_2_HCond[i4].fOutput = self->shp_table_h_cond[var_len-i4-1];
            }
            
            var_len = array_length(self->shp_table_mat_pot_len,self->shp_table_dif_wc_len);
            pSWater->MatPot_2_DifWC = (RESPONSE*)g_malloc0_n(var_len,sizeof(RESPONSE));
            pSWater->MatPot_2_DifWC[0].iInLen = self->shp_table_mat_pot_len;
            pSWater->MatPot_2_DifWC[0].iOutLen = self->shp_table_dif_wc_len;
            for(i4=0; i4<var_len; i4++)
            {
                pSWater->MatPot_2_DifWC[i4].fInput = self->shp_table_mat_pot[var_len-i4-1];
                pSWater->MatPot_2_DifWC[i4].fOutput = self->shp_table_dif_wc[var_len-i4-1];
            }
            ///////////////////////////////////////////////////////////////////////////////////////////////////
    }*/
    
        // Hydraulische Funktionen laden:
        self->WCont = xpn_register_var_get_pointer(xpn->pXSys->var_list,"hydraulic_fuctions.WCont");
        self->HCond = xpn_register_var_get_pointer(xpn->pXSys->var_list,"hydraulic_fuctions.HCond");
        self->DWCap = xpn_register_var_get_pointer(xpn->pXSys->var_list,"hydraulic_fuctions.DWCap");
        self->MPotl = xpn_register_var_get_pointer(xpn->pXSys->var_list,"hydraulic_fuctions.MPotl");
        if ((self->WCont==NULL) || (self->HCond==NULL) || (self->DWCap==NULL) || (self->MPotl==NULL))
            {
                PRINT_ERROR("Problem with hydraulic functions!");
            }
 
           for (pSLayer=xpn->pSo->pSLayer,pSWater=xpn->pSo->pSWater,i=0; pSLayer!=NULL; pSLayer=pSLayer->pNext,pSWater=pSWater->pNext,i++)
            {
                int f1, f2;
                f1= (double)-150000;
                f2=(double)-3300;
                pSWater->fContPWP = WATER_CONTENT(f1);
                pSWater->fContFK = WATER_CONTENT(f2);
            }
 
      // Initialisieren der Potentialgroessen des Bodens fuer die
        //   Loeser der Richardsgleichung.  
        // initSoilPotential(exp_p);  // newly calculation of soil water potential
	
        for(pSL = pSo->pSLayer->pNext,
                    pSW = pSo->pSWater->pNext,
                    pWL = pWa->pWLayer->pNext,
                    iLayer = 1; ((pSL->pNext != NULL)&&
                                 (pSW->pNext != NULL)&&
                                 (pWL->pNext != NULL)); pSL = pSL->pNext,
                    pSW = pSW->pNext,
                    pWL = pWL->pNext,
                    iLayer++)
                {
                    pWL->fMatPotAct = MATRIX_POTENTIAL(pWL->fContAct);
                    pWL->fMatPotOld = pWL->fMatPotAct;
                }

            // Auf die virtuelle erste Simulationsschicht werden
            //   Werte der zweiten Simulationsschicht geschrieben  

            pWa->pWLayer->fMatPotOld = pWa->pWLayer->pNext->fMatPotOld;
            pWa->pWLayer->fMatPotAct = pWa->pWLayer->pNext->fMatPotAct;


           // Auf die virtuelle letzte Simulationsschicht werden
           //    Werte der vorletzten Simulationsschicht geschrieben  

            pWL->fMatPotOld = pWL->pBack->fMatPotOld;
            pWL->fMatPotAct = pWL->pBack->fMatPotAct;
	
		
	return RET_SUCCESS;
}



/* int hydraulic_functions_shp_table_run(hydraulic_functions *self)
{
	 expertn_modul_base *xpn = &(self->parent);
   
 PSLAYER pSLayer;
     PSLAYER pSL;
	 PSWATER pSWater;
     PWLAYER pWL;
	 PSWATER      pSW;
     PWATER pWa = xpn->pWa;
     PSPROFILE pSo = xpn->pSo;
     pWL= xpn->pWa->pWLayer; 
	 pSW = xpn->pSo->pSWater;
	
     int i, iLayer; 

     
     if((xpn->pSo->pSWater->pNext!=NULL)&&(self->iCheckPointer ==0))
     {
        //Added by Hong
        // Hydraulische Funktionen laden:
        self->WCont = xpn_register_var_get_pointer(xpn->pXSys->var_list,"hydraulic_fuctions.WCont");
        self->HCond = xpn_register_var_get_pointer(xpn->pXSys->var_list,"hydraulic_fuctions.HCond");
        self->DWCap = xpn_register_var_get_pointer(xpn->pXSys->var_list,"hydraulic_fuctions.DWCap");
        self->MPotl = xpn_register_var_get_pointer(xpn->pXSys->var_list,"hydraulic_fuctions.MPotl");
        if ((self->WCont==NULL) || (self->HCond==NULL) || (self->DWCap==NULL) || (self->MPotl==NULL))
            {
                PRINT_ERROR("Problem with hydraulic functions!");
            }
        //End of Hong
 
     
         if((xpn->pSo->pSWater->pNext!=NULL)&&(self->iCheckPointer ==0))
         {
            shp_table_set_soil(self); 
            
            for (pSLayer=xpn->pSo->pSLayer,pSWater=xpn->pSo->pSWater,i=0; pSLayer!=NULL; pSLayer=pSLayer->pNext,pSWater=pSWater->pNext,i++)
            {
                int f1, f2;
                f1= (double)-150000;
                f2=(double)-3300;
                pSWater->fContPWP = WATER_CONTENT(f1);
                pSWater->fContFK = WATER_CONTENT(f2);
            }
 
      // Initialisieren der Potentialgroessen des Bodens fuer die
        //   Loeser der Richardsgleichung.  
        // initSoilPotential(exp_p);  // newly calculation of soil water potential
	
        for(pSL = pSo->pSLayer->pNext,
                    pSW = pSo->pSWater->pNext,
                    pWL = pWa->pWLayer->pNext,
                    iLayer = 1; ((pSL->pNext != NULL)&&
                                 (pSW->pNext != NULL)&&
                                 (pWL->pNext != NULL)); pSL = pSL->pNext,
                    pSW = pSW->pNext,
                    pWL = pWL->pNext,
                    iLayer++)
                {
                    pWL->fMatPotAct = MATRIX_POTENTIAL(pWL->fContAct);
                    pWL->fMatPotOld = pWL->fMatPotAct;
                }

            // Auf die virtuelle erste Simulationsschicht werden
            //   Werte der zweiten Simulationsschicht geschrieben  

            pWa->pWLayer->fMatPotOld = pWa->pWLayer->pNext->fMatPotOld;
            pWa->pWLayer->fMatPotAct = pWa->pWLayer->pNext->fMatPotAct;


           // Auf die virtuelle letzte Simulationsschicht werden
           //    Werte der vorletzten Simulationsschicht geschrieben  

            pWL->fMatPotOld = pWL->pBack->fMatPotOld;
            pWL->fMatPotAct = pWL->pBack->fMatPotAct;
	
            self->iCheckPointer = 1;
         }

     }
     
	return RET_SUCCESS;
}*/

int hydraulic_functions_shp_table_done(hydraulic_functions *self)
{
	 expertn_modul_base *xpn = &(self->parent);
     
/*     PSWATER      pSWater = xpn->pSo->pSWater;
     
     int iLayer, i;
     int	var_len=0;*/
    
     if (self->ini_filename  !=NULL)
        free(self->ini_filename);
	 self->ini_filename=NULL;
    
/*       for (iLayer=1; pSWater!=NULL; pSWater=pSWater->pNext,iLayer++)
        {
               var_len = pSWater->MatPot_2_WatCont[0].iInLen;
              
                for(i=0; i<var_len; i++)
               {
               free(&pSWater->MatPot_2_WatCont[i]);
               }
                   
        }*/

		
	return RET_SUCCESS;
}



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : SHP_LT_HCond()                                            */
/*                                                                   */
/*  Function : Calculation of hydraulic unsaturated conductivity     */
/*             --> Look-up Table      */
/*                                                */
/*  Author   : sg, 13.09.2018                                           */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

double SHP_LT_HCond(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2,   
                double tau, double Tsat_c, double Tmin_c, double Ksat_c, double Ksat_nc, PSWATER pSW)
{
    extern PSPROFILE GetSoilPoi(void); //wie kann ich Funktion aus  expertn_module_base hier aufrufen?

//    PSPROFILE       pSo = GetSoilPoi();
    
    double FK;
    
   //Hydraulic conductivity
   FK = SHP_AFGENERATOR(Hakt, pSW->MatPot_2_HCond);
 return FK;

}  /* end Look-up Table conductivity */
/*------------------------------------------------*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : SHP_LT_WCont()                                            */
/*                                                                   */
/*  Function : Calculation of volumetric water content               */
/*             --> Look-up Table         */
/*                                                                   */
/*  Author   : sg, 13.09.2018                                          */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
               
double SHP_LT_WCont(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2,   
                double tau, double Tsat_c, double Tmin_c, double Ksat_c, double Ksat_nc, PSWATER pSW)
{
    double FQ;

// Water content 
FQ = SHP_AFGENERATOR(Hakt, pSW->MatPot_2_WatCont);
         
 return FQ;
}  /*  end Look-up Table water content  */

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : SHP_LT_DWCap()                                          */
/*                                                                 */
/*  Function : Calculation of differential water capacity          */
/*             --> Look-up Table       */
/*  Author   : sg, 13.09.2018                                         */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

double SHP_LT_DWCap(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2,   
                double tau, double Tsat_c, double Tmin_c, double Ksat_c, double Ksat_nc, PSWATER pSW)

{
   double FC;

 //   Differential water capacity
 FC = SHP_AFGENERATOR(Hakt, pSW->MatPot_2_DifWC);

 return FC;
}  /* end Look-up Table  DWC*/




/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : SHP_LT_MPotl()                                          */
/*  Function : Calculation of matric potential from                */
/*             a given volumetric water content                    */
/*            --> Look-up Table      */
/*                                                                 */
/*  Autor    : sg, 13.09.2018                                         */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

double SHP_LT_MPotl(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2,   
                double tau, double Tsat_c, double Tmin_c, double Ksat_c, double Ksat_nc, PSWATER pSW)

{
       double FH;

       // Matric potential
       FH=SHP_AFGENERATOR(Takt, pSW->WatCont_2_MatPot);

 return FH;
}  /* end Look-up Table Potl  */

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Function: double DLL AFGENERATOR(double fInput, RESPONSE* pfResp)
//Author:	Enli Wang
//Date:		07.11.1996
//Purpose:	This function interpolates linearly a value from a RESPONSE table using input fInput
//Inputs:	1. fInput	- the input value (-)
//			2. pfResp	- Pointer to the first element of the given RESPONSE table
//Outputs:	The interpolated value at fInput (-)
//Functions Called:
//Reference:1. Wang,Enli. xxxx.
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
double SHP_AFGENERATOR(double fInput, RESPONSE* pResp)
{

	RESPONSE *pfResp; //, *pResp1;

	double	 fSlope,fOutput;
	int	  	 nPoints,i;

	fOutput=0.0;

	pfResp = pResp;

	if (pResp[0].iInLen >= pResp[0].iOutLen)
		nPoints = pfResp[0].iOutLen;
	else
		nPoints = pfResp[0].iInLen;

	if (fInput<=pfResp[0].fInput)
		return pfResp[0].fOutput;

	if (nPoints > 0) {
		if (fInput>=pfResp[nPoints-1].fInput)
			return pfResp[nPoints-1].fOutput;


		for (i=0; i<nPoints-1; i++) {
			if (	((fInput>=pfResp[i].fInput)&&(fInput<pfResp[i+1].fInput))
			        ||  ((fInput<=pfResp[i].fInput)&&(fInput>pfResp[i+1].fInput))   )
				break;
		}

		fSlope = (pfResp[i+1].fOutput- pfResp[i].fOutput)/(pfResp[i+1].fInput-pfResp[i].fInput);
		fOutput= pfResp[i].fOutput + fSlope*(fInput-pfResp[i].fInput);
	}

	return fOutput;
}

//PSPROFILE  GetSoilPoi(void)
//{ return pSo; }

//SG 20180919  (from expertn_module_base.c)
int array_length(var1,var2)
{
    if (var1>var2)
    {
        return var2;
    }
    else if (var1<var2)
    {
        return var1;
    }
    else
    {
        return var1;
    }
}
