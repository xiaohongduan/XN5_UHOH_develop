//      hydraulic_functions.c
//      
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//      


#include "hydraulic_functions.h"
#include "VanGenuchten.h"
#include "Hutson_and_Cass.h"
#include "Bimodal_van_Genuchten.h"
#include "Brutsaert_and_Gardner.h"
#include "Brooks_and_Corey.h"
#include "Brunswick_model.h"
#include "shp_look_up_table.h"

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


G_DEFINE_TYPE(hydraulic_functions, hydraulic_functions, EXPERTN_MODUL_BASE_TYPE);


static void hydraulic_functions_class_init(hydraulic_functionsClass *klass){}

static void hydraulic_functions_init(hydraulic_functions *self)
{
//gchar *S;
// hier könnnen die Standartwerte der Variablen gesetzt werden oder Speicher reserviert werden!
// Vorsicht: Die Daten der Expert N Basis Klasse sind hier noch nicht gesetzt!
// Werden diese beim Initialisieren benoetigt, so muss eine eigene Init dafuer registriert werden!

// print Output Message if Modul is used from Expert N

self->hydraulic_function_choice=1; // Standard, falls aus irgeneinem Grund keine Wahl getroffen wird

}


int hydraulic_functions_set(hydraulic_functions *self,void *WCont,void *HCond, void *DWCap, void *MPotl)
  {
	  xpn_register_var_add_none(self->parent.pXSys->var_list,"hydraulic_fuctions.WCont",WCont,-1,TRUE);
	  xpn_register_var_add_none(self->parent.pXSys->var_list,"hydraulic_fuctions.HCond",HCond,-1,TRUE);
	  xpn_register_var_add_none(self->parent.pXSys->var_list,"hydraulic_fuctions.DWCap",DWCap,-1,TRUE);
	  xpn_register_var_add_none(self->parent.pXSys->var_list,"hydraulic_fuctions.MPotl",MPotl,-1,TRUE);
	  return RET_SUCCESS;
  }


int hydraulic_functions_set_Hutson_and_Cass(hydraulic_functions *self)
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

	  PRINT_MESSAGE(xpn,5,"Hydraulic Function set to Hutson and Cass") ;
	  hydraulic_functions_set(self,&HC_WCont,&HC_HCond,&HC_DWCap,&HC_MPotl); 
        
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
        
    /* Initialisieren der Potentialgroessen des Bodens fuer die
       Loeser der Richardsgleichung.  */
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

    /* Auf die virtuelle erste Simulationsschicht werden
       Werte der zweiten Simulationsschicht geschrieben  */

    pWa->pWLayer->fMatPotOld = pWa->pWLayer->pNext->fMatPotOld;
    pWa->pWLayer->fMatPotAct = pWa->pWLayer->pNext->fMatPotAct;


    /* Auf die virtuelle letzte Simulationsschicht werden
       Werte der vorletzten Simulationsschicht geschrieben  */

    pWL->fMatPotOld = pWL->pBack->fMatPotOld;
    pWL->fMatPotAct = pWL->pBack->fMatPotAct;
	
	  return RET_SUCCESS;
  }

int hydraulic_functions_set_van_Genuchten_and_Mualem(hydraulic_functions *self)
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

	   PRINT_MESSAGE(xpn,5,"Hydraulic Function set to van Genuchten and Mualem") ;
	   hydraulic_functions_set(self,&VG_WCont,&VG_HCond,&VG_DWCap,&VG_MPotl);
        
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
        
    /* Initialisieren der Potentialgroessen des Bodens fuer die
       Loeser der Richardsgleichung.  */
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

    /* Auf die virtuelle erste Simulationsschicht werden
       Werte der zweiten Simulationsschicht geschrieben  */

    pWa->pWLayer->fMatPotOld = pWa->pWLayer->pNext->fMatPotOld;
    pWa->pWLayer->fMatPotAct = pWa->pWLayer->pNext->fMatPotAct;


    /* Auf die virtuelle letzte Simulationsschicht werden
       Werte der vorletzten Simulationsschicht geschrieben  */

    pWL->fMatPotOld = pWL->pBack->fMatPotOld;
    pWL->fMatPotAct = pWL->pBack->fMatPotAct;
	

	   return RET_SUCCESS;
  }

int hydraulic_functions_set_Brooks_and_Corey(hydraulic_functions *self)
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

	  PRINT_MESSAGE(xpn,5,"Hydraulic Function set to van Brooks and Corey") ;
	  hydraulic_functions_set(self,&BC_WCont,&BC_HCond,&BC_DWCap,&BC_MPotl); 
        
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
        
    /* Initialisieren der Potentialgroessen des Bodens fuer die
       Loeser der Richardsgleichung.  */
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

    /* Auf die virtuelle erste Simulationsschicht werden
       Werte der zweiten Simulationsschicht geschrieben  */

    pWa->pWLayer->fMatPotOld = pWa->pWLayer->pNext->fMatPotOld;
    pWa->pWLayer->fMatPotAct = pWa->pWLayer->pNext->fMatPotAct;


    /* Auf die virtuelle letzte Simulationsschicht werden
       Werte der vorletzten Simulationsschicht geschrieben  */

    pWL->fMatPotOld = pWL->pBack->fMatPotOld;
    pWL->fMatPotAct = pWL->pBack->fMatPotAct;
	
	  return RET_SUCCESS;
  }
  
int hydraulic_functions_set_Brutsaert_and_Gardner(hydraulic_functions *self)
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

	 PRINT_MESSAGE(xpn,5,"Hydraulic Function set to Brutsaert and Gardner") ;
	 hydraulic_functions_set(self,&M2_WCont,&M2_HCond,&M2_DWCap,&M2_MPotl); 
        
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
        
    /* Initialisieren der Potentialgroessen des Bodens fuer die
       Loeser der Richardsgleichung.  */
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

    /* Auf die virtuelle erste Simulationsschicht werden
       Werte der zweiten Simulationsschicht geschrieben  */

    pWa->pWLayer->fMatPotOld = pWa->pWLayer->pNext->fMatPotOld;
    pWa->pWLayer->fMatPotAct = pWa->pWLayer->pNext->fMatPotAct;


    /* Auf die virtuelle letzte Simulationsschicht werden
       Werte der vorletzten Simulationsschicht geschrieben  */

    pWL->fMatPotOld = pWL->pBack->fMatPotOld;
    pWL->fMatPotAct = pWL->pBack->fMatPotAct;
	
	 return RET_SUCCESS;
  }
  
int hydraulic_functions_set_Bimodal_van_Genuchten(hydraulic_functions *self)
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

	 PRINT_MESSAGE(xpn,5,"Hydraulic Function set to Bimodal van Genuchten") ;
	 hydraulic_functions_set(self,&C2_WCont,&C2_HCond,&C2_DWCap,&C2_MPotl); 
        
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
        
    /* Initialisieren der Potentialgroessen des Bodens fuer die
       Loeser der Richardsgleichung.  */
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

    /* Auf die virtuelle erste Simulationsschicht werden
       Werte der zweiten Simulationsschicht geschrieben  */

    pWa->pWLayer->fMatPotOld = pWa->pWLayer->pNext->fMatPotOld;
    pWa->pWLayer->fMatPotAct = pWa->pWLayer->pNext->fMatPotAct;


    /* Auf die virtuelle letzte Simulationsschicht werden
       Werte der vorletzten Simulationsschicht geschrieben  */

    pWL->fMatPotOld = pWL->pBack->fMatPotOld;
    pWL->fMatPotAct = pWL->pBack->fMatPotAct;
	
	 return RET_SUCCESS;	 
  }
  
int hydraulic_functions_set_Brunswick_model(hydraulic_functions *self)
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

	 PRINT_MESSAGE(xpn,5,"Hydraulic Function set to Brunswick_model") ;
	 hydraulic_functions_set(self,&BRUNSWICK_WCont,&BRUNSWICK_HCond,&BRUNSWICK_DWCap,&BRUNSWICK_MPotl); 
        
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
        
    /* Initialisieren der Potentialgroessen des Bodens fuer die
       Loeser der Richardsgleichung.  */
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

    /* Auf die virtuelle erste Simulationsschicht werden
       Werte der zweiten Simulationsschicht geschrieben  */

    pWa->pWLayer->fMatPotOld = pWa->pWLayer->pNext->fMatPotOld;
    pWa->pWLayer->fMatPotAct = pWa->pWLayer->pNext->fMatPotAct;


    /* Auf die virtuelle letzte Simulationsschicht werden
       Werte der vorletzten Simulationsschicht geschrieben  */

    pWL->fMatPotOld = pWL->pBack->fMatPotOld;
    pWL->fMatPotAct = pWL->pBack->fMatPotAct;
	
	 return RET_SUCCESS;	 
  }
 
/*int hydraulic_functions_set_SHP_LT(hydraulic_functions *self)
  {
	 expertn_modul_base *xpn = &(self->parent);
	 PRINT_MESSAGE(xpn,5,"Hydraulic Function set to Look-up Table") ;
	 hydraulic_functions_set(self,&SHP_LT_WCont,&SHP_LT_HCond,&SHP_LT_DWCap,&SHP_LT_MPotl); 
	 return RET_SUCCESS;	 
  }
 */