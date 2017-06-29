//      mpmas_coupling.c
//
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//
#include "mpmas_coupling.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//Begin of Hong
#include <string.h> // for memcpy

//End of Hong

G_DEFINE_TYPE(mpmas_coupling, mpmas_coupling, EXPERTN_MODUL_BASE_TYPE);
static void mpmas_coupling_class_init(mpmas_couplingClass *klass) {}
static void mpmas_coupling_init(mpmas_coupling *self)
{
    gchar *S;
// hier könnnen die Standartwerte der Variablen gesetzt werden oder Speicher reserviert werden!
// Vorsicht: Die Daten der Expert N Basis Klasse sind hier noch nicht gesetzt!
// Werden diese beim Initialisieren benoetigt, so muss eine eigene Init dafuer registriert werden!
// init example var:
//self->Plant_Mass = 20.0;
// print Output Message if Modul is used from Expert N
//S  = g_strdup_printf("%s\t%d\tInit MODUL MPMAS_COUPLING!",__FILE__,__LINE__);
//fprintf(stdout,"%s\n",S);
//fflush(stdout);
//g_free(S);
}
// Our Modul Functions:
int mpmas_coupling_Load(mpmas_coupling *self)
{
    expertn_modul_base *xpn = &(self->parent);
	
//Begin of Hong: for turn-off module of not-currentGrid  
    expertnclass** xpn_class=(expertnclass **)xpn->pXSys->xpn_classes;  	
	int p, id, Module_len;
    id = xpn->pXSys->id;
    Module_len= (int)xpn_class[id]->XPN_Moduls_len;
//End of Hong

    self->mpmas_to_xn = (STRUCT_mpmas_to_xn*)xpn_register_var_get_pointer(xpn->pXSys->var_list,"p_grid_mpmas_to_xn");
    self->xn_to_mpmas = (STRUCT_xn_to_mpmas*)xpn_register_var_get_pointer(xpn->pXSys->var_list,"p_grid_xn_to_mpmas");
    self->xn_to_mpmas2 = (STRUCT_xn_to_mpmas2*)xpn_register_var_get_pointer(xpn->pXSys->var_list,"p_grid_xn_to_mpmas2");
        
//Begin of Hong: for turn-off module
    self->previousGrid=self->mpmas_to_xn->currentGrid; //to ensure the update of currentGrid after harvest
    
	//get the original module list of each xpn: 
    self->XPN_Moduls_full= g_malloc0(sizeof(struct_module*)*Module_len);
    
    for (p=0;p<Module_len;p++)
        {
            self->XPN_Moduls_full[p]=g_malloc0(sizeof(struct_module));
            memcpy(self->XPN_Moduls_full[p],xpn_class[id]->XPN_Moduls[p],sizeof(struct_module));
        }
//End of Hong   
     
    //self->WRITE_mpmas_xn =(STRUCT_WRITE_mpmas_xn*)xpn_register_var_get_pointer(xpn->pXSys->var_list,"pWRITE_mpmas_xn");
	
	self->new_plant = 0;	
    self->simulation_days = 0;
    self->count = 0;
    self->restart = 1;
	
	//Begin of Hong: to determine the date of data transfer (after harvest and tillage of )
	self->dataTransferDate.day= 0;
	self->dataTransferDate.month= 0;
	self->dataTransferDate.year= 0;
	
	self->harvest_done=0;
	self->tillageAfterHarvest_done=0;
	self->checkSwitchDate_done=0;
		
    if ((self->mpmas_to_xn==NULL) || (self->xn_to_mpmas==NULL))
        {
            PRINT_ERROR("No pmpmas_to_xn");
        }   
    return RET_SUCCESS;
}
int mpmas_coupling_Austausch(mpmas_coupling *self)
{
    expertn_modul_base *xpn = &(self->parent);
	
    //Begin of Hong: turn-off module of not-currentGrid
    expertnclass** xpn_class=(expertnclass **)xpn->pXSys->xpn_classes;
    int p, id, Module_len;
    id = xpn->pXSys->id;
    Module_len= (int)xpn_class[id]->XPN_Moduls_len;
    //End of Hong
	
    PPLANT pPl = xpn->pPl;
    PGENOTYPE pGe = pPl->pGenotype;
    PTIME pTi = xpn->pTi;
    PSIMTIME pST = pTi->pSimTime;
    PMSOWINFO   pSI = xpn->pMa->pSowInfo;
    PMANAGEMENT  pMa = xpn->pMa;
    PSPROFILE pSo = xpn->pSo;
    PCHEMISTRY pCh = xpn->pCh;
    PNFERTILIZER fertil_first,fertil,fertil_act;
    PMIRRIGATION irr_first, irr, irr_act;
	PTILLAGE till_first, till, till_act; // Hong: for tillage
    
// Begin Hong: for multigrid
    PSLAYER  pSL;
    PHLAYER  pHL; 
    PCLAYER  pCL;
    PWLAYER  pWL;
    PSWATER  pSW;
    PLAYERROOT pLR;
    PCPARAM pPA;
    PCPROFILE pCP;
    
    PSPROFILE pSo2;
    PSLAYER  pSL2;
    PHLAYER  pHL2; 
    PCLAYER  pCL2;
    PWLAYER  pWL2;
    PSWATER  pSW2;
    PLAYERROOT pLR2;
    PCPARAM pPA2;
    PCPROFILE pCP2;
// End of Hong

    char *S, *S2;
    char *read_filename, *read_filename2;

    int i, j, start_i;
    int fertilizer_count, irrigation_count;
    int year, month, day;
    int fertil_start_year,fertil_start_mon,fertil_start_day;

    GDate* date;
    
    if ((self->mpmas_to_xn==NULL) || (self->xn_to_mpmas==NULL))
        {
            return RET_SUCCESS;
        }

    if(self->restart == 1)
        {
            self->xn_to_mpmas2->startDay = (int)pTi->pSimTime->fTimeY;
            self->restart = 0;
        }

    get_daily_air_and_soil_temperatures(self);
    
// Begin of Hong: ######################## turn-on / turn-off modules and data transfer ######################################### 
    	
	//memset(stopDate, 0, sizeof (xnmpmasDate));
	if ((self->mpmas_to_xn->updateManagement==1))
    {
	
	  //1. For the first year, let the not-currentGrid turn-off	
      if (pTi->pSimTime->bFirstRound==1) // only for the first year
	  {
		  if ((self->mpmas_to_xn->own_grid_number!=self->mpmas_to_xn->currentGrid)) // search for not-currentGrid
            {  
              // turn-off module
               for (p=0;p<Module_len;p++)
                   { 
					 if ((strcmp(xpn_class[id]->XPN_Moduls[p]->SubModul,"database")!=0)&&(strcmp(xpn_class[xpn->pXSys->id]->XPN_Moduls[p]->SubModul,"mpmas")!=0))// solange SubModul != ""database"&&"mpmas", soll ausgeschaltet werden
                        {
                           xpn_class[id]->XPN_Moduls[p]->run=NULL;
                           xpn_class[id]->XPN_Moduls[p]->global_run=NULL;
                           xpn_class[id]->XPN_Moduls[p]->mosaic_run=NULL;
                          
                        }
                    }
				PRINT_MESSAGE(xpn,1,"irrelevant module turned off\n");// for debug	
				
				// skip reading of new plant and management:
				self->new_plant = 1;
				self->new_management = 1;
				self->mpmas_to_xn->updateManagement=0;
				pPl->pModelParam->HarvestDay=0; // to forbid the plant growth at not-currentGrid
                pPl->pModelParam->HarvestMonth=0;
                pPl->pModelParam->HarvestYear=0;
				
	        }
	  }//End of first year
      
	  //2. For the rest of years     		
	 // 2.1 In case of switching from not-currentGrid to currentGrid: 	
	 else if ((self->mpmas_to_xn->own_grid_number!=self->previousGrid)&&(self->mpmas_to_xn->own_grid_number==self->mpmas_to_xn->currentGrid)) // ownGrid != previousGrid but ==currentGrid
         {
		  // because of skipping the plant growth last year, self->new_plant won't be reset to 0 at harvest:
		  self->new_plant = 0;
		   self->new_management = 0;
		  
		  //2.1.(1)turn-on module by copy of XPN_Moduls_full
          for (p=0;p<Module_len;p++)
                {           
                  memcpy(xpn_class[id]->XPN_Moduls[p],self->XPN_Moduls_full[p],sizeof(struct_module));
            
                 } 
		  PRINT_MESSAGE(xpn,1,"module turned on");// for debug	
			 
		  //2.1.(2) reset updateManagement and save the currentGrid in the previousGrid before next update on stopDate		 
          self->mpmas_to_xn->updateManagement=0;
          self->previousGrid=self->mpmas_to_xn->currentGrid;
		  
         }// End of ownGrid != previousGrid but ==currentGrid
		 
	  
	 // 2.2 In case of switching from currentGrid to not-currentGrid:   
       else if ((self->mpmas_to_xn->own_grid_number==self->previousGrid)&&(self->mpmas_to_xn->own_grid_number!=self->mpmas_to_xn->currentGrid)) // ownGrid == previousGrid but !=currentGrid
         {  
			 
		  if ((self->tillageAfterHarvest_done ==1)&&(xpn_time_compare_date(pTi->pSimTime->iyear,pTi->pSimTime->mon,pTi->pSimTime->mday,self->stopDate.year,self->stopDate.month,self->stopDate.day)>=1))
		  
			  {
		    
		  //2.2.(1) Begin of data transfer, pass data from previousGrid to updated currentGrid            
            int numberOfGrids, currentGrid, own_grid_number,iLayer=0;
            
            numberOfGrids= self->mpmas_to_xn->number_of_grids;
            own_grid_number = self->mpmas_to_xn->own_grid_number;
            currentGrid=self->previousGrid; // if harvest date is later than 1.st. Aug., self->mpmas_to_xn->currentGrid has been updated for the coming year   
  
            for (i=0;i<numberOfGrids;i++)
            {
                if((i== self->mpmas_to_xn->currentGrid)) // search for the updated currentGrid of xpn_other_grids  
                {
                    //1. do the data transfer
                    
 // e.g.                 for(pCL=pCh->pCLayer,pSL=pSo->pSLayer,pWL=pWa->pWLayer,pSW=pSo->pSWater,iLayer=0;
 // e.g.                           iLayer<pSo->iLayers;
 // e.g.                           iLayer++, pSL= pSL->pNext,pCL=pCL->pNext,pWL=pWL->pNext,pSW=pSW->pNext)
 // e.g.                           {
// e.g.                                
 // e.g.                           }
                   
                  pSo2= self->mpmas_to_xn->xpn_other_grids[currentGrid]->pSo; //from [own_grid_number] to [i]

                  // soil water 
                  for(pWL=self->mpmas_to_xn->xpn_other_grids[i]->pWa->pWLayer,pWL2=self->mpmas_to_xn->xpn_other_grids[currentGrid]->pWa->pWLayer,iLayer=0;
                            iLayer<pSo2->iLayers;
                            iLayer++, pWL=pWL->pNext,pWL2=pWL2->pNext)
                            {
  
                                memcpy(pWL,pWL2,sizeof(WLAYER)-2*sizeof(PWLAYER));
                                pWL->fContInit = pWL2->fContInit;
                                //memcpy(&(pWL->fContInit),&(pWL2->fContInit),&(pWL2->fLatFlowDay)-&(pWL2->fContInit)+sizeof(double));
                                //memcpy(&(pWL->fContInit),&(pWL2->fContInit),sizeof(WLAYER)-2*sizeof(PWLAYER));
                                
                            }
                                             
                    
                 // soil temp
                 for(pHL=self->mpmas_to_xn->xpn_other_grids[i]->pHe->pHLayer,pHL2=self->mpmas_to_xn->xpn_other_grids[currentGrid]->pHe->pHLayer,iLayer=0;
                            iLayer<pSo2->iLayers;
                            iLayer++, pHL=pHL->pNext,pHL2=pHL2->pNext)
                            {
                                memcpy(pHL,pHL2,sizeof(HLAYER)-2*sizeof(PHLAYER));
                                
                            }
                            
                // soil nitrogen and carbon in layers
                 for(pCL=self->mpmas_to_xn->xpn_other_grids[i]->pCh->pCLayer,pCL2=self->mpmas_to_xn->xpn_other_grids[currentGrid]->pCh->pCLayer,iLayer=0;
                            iLayer<pSo2->iLayers;
                            iLayer++, pCL=pCL->pNext,pCL2=pCL2->pNext)
                            {
                                memcpy(pCL,pCL2,sizeof(CLAYER)-2*sizeof(PCLAYER));
                            }
                            
                                                        
                 // soil humus in layers
                for(pSL=self->mpmas_to_xn->xpn_other_grids[i]->pSo->pSLayer,pSL2=self->mpmas_to_xn->xpn_other_grids[currentGrid]->pSo->pSLayer,iLayer=0;
                            iLayer<pSo2->iLayers;
                            iLayer++, pSL=pSL->pNext,pSL2=pSL2->pNext)
                            {
                                memcpy(&(pSL->fCHumus),&(pSL2->fCHumus),&(pSL2->fHumusCN)-&(pSL2->fCHumus)+sizeof(double));
                            }
                            
                // soil nitrogen and carbon in profile
                {     
                   pCP=self->mpmas_to_xn->xpn_other_grids[i]->pCh->pCProfile;
                   pCP2=self->mpmas_to_xn->xpn_other_grids[currentGrid]->pCh->pCProfile; 
                                 
                   memcpy(pCP,pCP2,sizeof(CPROFILE));
                                   
                }
                  
                // root density // 
                for(pLR=self->mpmas_to_xn->xpn_other_grids[i]->pPl->pRoot->pLayerRoot,pLR2=self->mpmas_to_xn->xpn_other_grids[currentGrid]->pPl->pRoot->pLayerRoot,iLayer=0;
                            iLayer<pSo2->iLayers;
                            iLayer++, pLR=pLR->pNext,pLR2=pLR2->pNext)
                            {
                                memcpy(pLR,pLR2,sizeof(STLAYERROOT)-2*sizeof(PLAYERROOT));
                            }
                /*
                 // nitrogen model param
                  for(pPA=self->mpmas_to_xn->xpn_other_grids[i]->pCh->pCParam,pPA2=self->mpmas_to_xn->xpn_other_grids[currentGrid]->pCh->pCParam,iLayer=0;
                            iLayer<pSo2->iLayers;
                            iLayer++, pPA=pPA->pNext,pPA2=pPA2->pNext)
                            {
                                memcpy(pPA,pPA2,sizeof(CPARAM)-2*sizeof(PCPARAM));
                            }
                */
		
                // for debug
                 PRINT_MESSAGE(xpn,1,"data transferred from previousGrid to currentGrid");// for debug               
                 // End of debug
				 
				} //End of i=currentGrid
				
            }//End of data transfer
		   
	 		  
          //2.2.(2) turn-off module
          for (p=0;p<Module_len;p++)
          { 
              if ((strcmp(xpn_class[id]->XPN_Moduls[p]->SubModul,"database")!=0)&&(strcmp(xpn_class[xpn->pXSys->id]->XPN_Moduls[p]->SubModul,"mpmas")!=0))// solange SubModul != ""database"&&"mpmas", soll ausgeschaltet werden
                        {
                           xpn_class[id]->XPN_Moduls[p]->run=NULL;
                           xpn_class[id]->XPN_Moduls[p]->global_run=NULL;
                           xpn_class[id]->XPN_Moduls[p]->mosaic_run=NULL;
                          
                        }
          }
		  PRINT_MESSAGE(xpn,1,"module turned off");// for debug

		  //2.2.(3) reset updateManagement and save the currentGrid in the previousGrid before next update on stopDate
          self->mpmas_to_xn->updateManagement=0;
          self->previousGrid=self->mpmas_to_xn->currentGrid;
		  
		  self->new_plant = 1;
		  self->new_management = 1;
		  self->harvest_done=0;
		  pPl->pModelParam->HarvestDay=0; // to forbid the plant growth at not-currentGrid
		  pPl->pModelParam->HarvestMonth=0;
		  pPl->pModelParam->HarvestYear=0;
		  		  			 
          } //End of harvest_done==1
         }// End of ownGrid == previousGrid but !=currentGrid  	 

	 

		// 2.3 In case of no switching
        else // ownGrid == previousGrid and ==currentGrid or ownGrid != previousGrid and !=currentGrid, stay unchanged
         {
		  	 
           self->mpmas_to_xn->updateManagement=0;
           self->previousGrid=self->mpmas_to_xn->currentGrid;
         }
	    				
      } //End of self->mpmas_to_xn->updateManagement=1      
           
// End of Hong ############################ turn-on or turn-off module & data transfer ##########################################

//Begin of Hong: cover crop, tillage ##############################################################
if (NewDay(pTi))
	{	
	   //1. read plant info:	
	   if ((self->new_plant == 0)&&(self->mpmas_to_xn->own_grid_number==self->mpmas_to_xn->currentGrid))
        {
			// memset(self->xn_to_mpmas, 0, sizeof (STRUCT_xn_to_mpmas)); // not the right place here
			        self->harvestAdaptive=0;// Hong: default no adaptive!
			
			      //1.1 read cover crop info if any
					if ((self->mpmas_to_xn->coverCropCode[0] != '\0')&&(xpn_time_compare_date(pTi->pSimTime->iyear,pTi->pSimTime->mon,pTi->pSimTime->mday,self->mpmas_to_xn->coverCropSowDate.year,self->mpmas_to_xn->coverCropSowDate.month,self->mpmas_to_xn->coverCropSowDate.day)<=0))         
					{
                    PRINT_MESSAGE(xpn,1,"read new cover crop info:");
					
					self->new_plant = 1;
					self->daysSinceBBCH1 = 0;
					self->daysSinceBBCH2 = 0;
					
					self->harvest_done=0;//Hong
					self->tillageAfterHarvest_done=0;
					self->checkSwitchDate_done=0;
					
					pPl->pGenotype->acCropCode = self->mpmas_to_xn->coverCropCode;
                    pPl->pGenotype->acCropName = self->mpmas_to_xn->coverCropName;
                    pSI->Day = self->mpmas_to_xn->coverCropSowDate.day;
                    pSI->Month = self->mpmas_to_xn->coverCropSowDate.month;
                    pSI->Year = self->mpmas_to_xn->coverCropSowDate.year;
                    pPl->pGenotype->acVarietyName = self->mpmas_to_xn->coverCropVariety;
                    pSI->fPlantDens = self->mpmas_to_xn->coverCropSowDens;
                    pSI->fRowWidth = self->mpmas_to_xn->coverCropRowDist;
                    pSI->fSowDepth = self->mpmas_to_xn->coverCropSowDepth;
                    pPl->pModelParam->HarvestDay = self->mpmas_to_xn->coverCropPloughUnderDate.day;
                    pPl->pModelParam->HarvestMonth = self->mpmas_to_xn->coverCropPloughUnderDate.month;
                    pPl->pModelParam->HarvestYear = self->mpmas_to_xn->coverCropPloughUnderDate.year;
										
					//self->harvestAdaptive = self->mpmas_to_xn->harvestAdaptive;
					//self->harvestBBCH1 = self->mpmas_to_xn->harvestBBCH1;	
					//self->harvestBBCH1ExtraDays = self->mpmas_to_xn->harvestBBCH1ExtraDays;
					//self->harvestBBCH2 = self->mpmas_to_xn->harvestBBCH2;
					//self->harvestBBCH2ExtraDays = self->mpmas_to_xn->harvestBBCH2ExtraDays;
					
					pPl->pModelParam->cResidueCarryOff = 0; //residuals of coverCrop liegen lassen

 				// read plant ini file:
	                S  = g_strdup_printf("Config.ceres.%s", pPl->pGenotype->acCropName);
                    read_filename = xpn_register_var_get_pointer(xpn->pXSys->var_list,S);
                    g_free(S);
                    if (read_filename==NULL)
                        {
                            S  = g_strdup_printf("Global_Config.options.%s", pPl->pGenotype->acCropName);
                            read_filename = xpn_register_var_get_pointer(xpn->pXSys->var_list,S);
                            g_free(S);
                            if (read_filename==NULL)
                                {
                                    S  = g_strdup_printf("Entry 'ceres.%s' or 'global.%s' is missing in your options!",pPl->pGenotype->acCropName,pPl->pGenotype->acCropName);
                                    PRINT_ERROR(S);
                                    g_free(S);
                                }
                        }
                    if (read_filename!=NULL)
                        {
                            //Read plant parameters
                            S2 = expertn_modul_base_replace_std_templates(xpn,read_filename);
                            if (S2!=NULL)
                                {
                                    read_filename = get_fullpath_from_relative(xpn->pXSys->base_path, S2);
                                    if (expertn_modul_base_GenotypeRead(xpn,pPl,read_filename)!=0)
                                        {
                                            S  = g_strdup_printf("Error Read '%s.ini'-file, check 'crop_rotation.ini' and your model options", pPl->pGenotype->acCropName);
                                            PRINT_ERROR(S);
                                            g_free(S);
                                        }
                                    read_filename2 = g_strdup_printf("%s",read_filename);
                                    free(read_filename);
                                    g_free(S2);
                                }
                        }

				    expertn_modul_base_PlantVariableInitiation(pPl, pSo, pSI);
										
				   } //end new configuration necessary for coverCrop
			
			// 1.2 read crop info (sometimes after tillage of cover crop residues)
              else if (xpn_time_compare_date(pTi->pSimTime->iyear,pTi->pSimTime->mon,pTi->pSimTime->mday,self->mpmas_to_xn->sowDate.year,self->mpmas_to_xn->sowDate.month,self->mpmas_to_xn->sowDate.day)<=0)
                {			
					PRINT_MESSAGE(xpn,1,"read crop management");
					
					self->new_plant = 1;
					self->daysSinceBBCH1 = 0;
					self->daysSinceBBCH2 = 0;
					
					self->harvest_done=0;//Hong
					self->tillageAfterHarvest_done=0;
					self->checkSwitchDate_done=0;
		
					pPl->pGenotype->acCropCode = self->mpmas_to_xn->CropCode;
                    pPl->pGenotype->acCropName = self->mpmas_to_xn->CropName;
                    pSI->Day = self->mpmas_to_xn->sowDate.day;
                    pSI->Month = self->mpmas_to_xn->sowDate.month;
                    pSI->Year = self->mpmas_to_xn->sowDate.year;
                    pPl->pGenotype->acVarietyName = self->mpmas_to_xn->variety;
                    pSI->fPlantDens = self->mpmas_to_xn->sowDens;
                    pSI->fRowWidth = self->mpmas_to_xn->rowDist;
                    pSI->fSowDepth = self->mpmas_to_xn->sowDepth;
                    pPl->pModelParam->HarvestDay = self->mpmas_to_xn->harvestDate.day;
                    pPl->pModelParam->HarvestMonth = self->mpmas_to_xn->harvestDate.month;
                    pPl->pModelParam->HarvestYear = self->mpmas_to_xn->harvestDate.year;
					self->harvestAdaptive = self->mpmas_to_xn->harvestAdaptive;
					self->harvestBBCH1 = self->mpmas_to_xn->harvestBBCH1;	
					self->harvestBBCH1ExtraDays = self->mpmas_to_xn->harvestBBCH1ExtraDays;
					self->harvestBBCH2 = self->mpmas_to_xn->harvestBBCH2;
					self->harvestBBCH2ExtraDays = self->mpmas_to_xn->harvestBBCH2ExtraDays;
					
					 pPl->pModelParam->cResidueCarryOff = self->mpmas_to_xn->biom_remove;
                    // maybe needs to read the plant model
		
					// read plant ini file:
	                S  = g_strdup_printf("Config.ceres.%s", pPl->pGenotype->acCropName);
                    read_filename = xpn_register_var_get_pointer(xpn->pXSys->var_list,S);
                    g_free(S);
                    if (read_filename==NULL)
                        {
                            S  = g_strdup_printf("Global_Config.options.%s", pPl->pGenotype->acCropName);
                            read_filename = xpn_register_var_get_pointer(xpn->pXSys->var_list,S);
                            g_free(S);
                            if (read_filename==NULL)
                                {
                                    S  = g_strdup_printf("Entry 'ceres.%s' or 'global.%s' is missing in your options!",pPl->pGenotype->acCropName,pPl->pGenotype->acCropName);
                                    PRINT_ERROR(S);
                                    g_free(S);
                                }
                        }
                    if (read_filename!=NULL)
                        {
                            //Read plant parameters
                            S2 = expertn_modul_base_replace_std_templates(xpn,read_filename);
                            if (S2!=NULL)
                                {
                                    read_filename = get_fullpath_from_relative(xpn->pXSys->base_path, S2);
                                    if (expertn_modul_base_GenotypeRead(xpn,pPl,read_filename)!=0)
                                        {
                                            S  = g_strdup_printf("Error Read '%s.ini'-file, check 'crop_rotation.ini' and your model options", pPl->pGenotype->acCropName);
                                            PRINT_ERROR(S);
                                            g_free(S);
                                        }
                                    read_filename2 = g_strdup_printf("%s",read_filename);
                                    free(read_filename);
                                    g_free(S2);
                                }
                        }
		
					expertn_modul_base_PlantVariableInitiation(pPl, pSo, pSI);
									
				}//end new configuration necessary of crop	
																			
		} //end self->new_plant = 0;
			
		// 2. read management info:
	  if (self->new_management==0)
		{
			        self->new_management=1;
					
					self->nextMinFertAction = 0;
					self->nextOrgFertAction = 0;
					self->nextIrrigation = 0; 
				    self->nextTillage = 0;	
				
					
					self->numIrrig = self->mpmas_to_xn->numIrrig;
					self->numMinFert = self->mpmas_to_xn->numMinFert;
					self->numOrgFert = self->mpmas_to_xn->numOrgFert;
					self->numTill = self->mpmas_to_xn->numTill;
					
					int i;
					for (i = 0; i < self->mpmas_to_xn->numMinFert; ++i)
						self->mineralFertilization[i] = self->mpmas_to_xn->mineralFertilization[i];
						
					for ( i = 0; i < self->mpmas_to_xn->numOrgFert; ++i)	
						self->organicFertilization[i] = self->mpmas_to_xn->organicFertilization[i];
						
					for ( i = 0; i < self->mpmas_to_xn->numIrrig; ++i)
						self->irrigation[i] = self->mpmas_to_xn->irrigation[i];
						
					for ( i = 0; i < self->mpmas_to_xn->numTill; ++i)
						self->tillage[i] = self->mpmas_to_xn->tillage[i];	
															
		  }	//End of new_mamagement==0	
             
} // end NewDay		
		
//End of Hong 

    // XN to MPMAS
    //if ((xpn_time_compare_date(pTi->pSimTime->iyear,pTi->pSimTime->mon,pTi->pSimTime->mday,pPl->pModelParam->HarvestYear,pPl->pModelParam->HarvestMonth,pPl->pModelParam->HarvestDay)==0)&&(NewDay(pTi)))
    
    if (NewDay(pTi))
    {
        if (checkIfMineralFertilization(self) == 1) 
        {
            fertil = g_malloc0_n(1,sizeof(STNFERTILIZER));
            fertil_first = g_malloc0_n(1,sizeof(STNFERTILIZER));
            fertil->acName = g_strdup_printf("Mineral fertilizer");
            fertil->Day = pTi->pSimTime->mday;
            fertil->Month = pTi->pSimTime->mon;
            fertil->Year = pTi->pSimTime->iyear;
            fertil->fNO3N = self->mineralFertilization[self->nextMinFertAction].no3n;
            fertil->fNH4N = self->mineralFertilization[self->nextMinFertAction].nh4n;
            fertil->fUreaN = self->mineralFertilization[self->nextMinFertAction].urea;
            fertil->fTotalN = fertil->fNO3N + fertil->fNH4N + fertil->fUreaN;
            fertil->acCode =  self->mineralFertilization[self->nextMinFertAction].code;
            fertil->pNext = fertil_first;
            fertil_first->pBack = fertil;                                           
            pMa->pNFertilizer = NULL;
            pMa->pNFertilizer = fertil;
            
            //Begin of Hong: actualMinFertDate
            self->xn_to_mpmas->actualMinFertDate[self->nextMinFertAction].day=pTi->pSimTime->mday;
            self->xn_to_mpmas->actualMinFertDate[self->nextMinFertAction].month=pTi->pSimTime->mon;
            self->xn_to_mpmas->actualMinFertDate[self->nextMinFertAction].year=pTi->pSimTime->iyear;
            //End of Hong
            
            self->nextMinFertAction++;
                        
        }

        if (checkIfOrganicFertilization(self) == 1) 
        {
            fertil = g_malloc0_n(1,sizeof(STNFERTILIZER));
            fertil_first = g_malloc0_n(1,sizeof(STNFERTILIZER));
            fertil->acName = g_strdup_printf("Organic fertilizer");
            fertil->Day = pTi->pSimTime->mday;
            fertil->Month = pTi->pSimTime->mon;
            fertil->Year = pTi->pSimTime->iyear;
            fertil->fDryMatter = self->organicFertilization[self->nextOrgFertAction].drymatter;
            fertil->fNH4N = self->organicFertilization[self->nextOrgFertAction].nh4n;
            fertil->fNorgManure = self->organicFertilization[self->nextOrgFertAction].org_subst;
            fertil->fTotalN = fertil->fNO3N + fertil->fNH4N + fertil->fUreaN;
            fertil->acCode =  self->organicFertilization[self->nextOrgFertAction].code;
            fertil->pNext = fertil_first;
            fertil_first->pBack = fertil;                                           
            pMa->pNFertilizer = NULL;
            pMa->pNFertilizer = fertil;
            self->nextOrgFertAction++;
        }


        if ((1 == checkIfHarvest(self)) && (1 == self->new_plant))
            {
				
                PSPROFILE   pSo = xpn->pSo;
                PSLAYER     pSL;
                PCLAYER     pCL;

                double nmin0_30, nmin30_60, nmin60_90;
                double depth0_30, depth30_60, depth60_90;
                double actdepth;
                double EPSILON;

                EPSILON = 1e-9;

                nmin0_30 = 0.0;
                nmin30_60 = 0.0;
                nmin60_90 = 0.0;

                actdepth = 0.0;

                depth0_30 = 0.0;
                depth30_60 = 0.0;
                depth60_90 = 0.0;
                
						
                for (pCL = xpn->pCh->pCLayer->pNext, pSL = pSo->pSLayer->pNext;  //start
                (pSL->pNext != NULL);                     //end
                pSL = pSL->pNext,pCL=pCL->pNext)          //step
                {
                actdepth += pSL->fThickness; //mm
                if (actdepth <= 300.0)
                    {
                    depth0_30 += pSL->fThickness;
                    nmin0_30 += (pCL->fNO3N + pCL->fNH4N) * pSL->fThickness;
                    }
                if ((actdepth > 300.0) && (actdepth <= 600.0))
                    {
                    depth30_60 += pSL->fThickness;
                    nmin30_60 += (pCL->fNO3N + pCL->fNH4N) * pSL->fThickness;
                    }
                if ((actdepth > 600.0) && (actdepth <= 900.0))
                    {
                    depth60_90 += pSL->fThickness;
                    nmin60_90 += (pCL->fNO3N + pCL->fNH4N) * pSL->fThickness;
                    }
                if (actdepth > 900.0)
                    break;
                }

            self->xn_to_mpmas->Nmin0_30 = nmin0_30 / (depth0_30+EPSILON);
            self->xn_to_mpmas->Nmin30_60 = nmin30_60 / (depth30_60+EPSILON);
            self->xn_to_mpmas->Nmin60_90 = nmin60_90 / (depth60_90+EPSILON);
                
            self->new_plant = 0;
            self->xn_to_mpmas->fruitDryWeight = pPl->pBiomass->fFruitWeight;
            self->xn_to_mpmas->stemLeafDryWeight = pPl->pBiomass->fLeafWeight + pPl->pBiomass->fStemWeight;
            //self->xn_to_mpmas->Nmin = pCh->pCBalance->fNO3NProfile;
            pPl->pModelParam->HarvestDay=pTi->pSimTime->mday;
            pPl->pModelParam->HarvestMonth=pTi->pSimTime->mon;
            pPl->pModelParam->HarvestYear=pTi->pSimTime->iyear;
            
            //Begin of Hong: actualHarvestDate
            self->xn_to_mpmas->actualHarvestDate.day=pTi->pSimTime->mday;
            self->xn_to_mpmas->actualHarvestDate.month=pTi->pSimTime->mon;
            self->xn_to_mpmas->actualHarvestDate.year=pTi->pSimTime->iyear;
			
			pPl->pModelParam->HarvestDay=pTi->pSimTime->mday; // to forbid the plant growth at not-currentGrid
            pPl->pModelParam->HarvestMonth=pTi->pSimTime->mon;
            pPl->pModelParam->HarvestYear=pTi->pSimTime->iyear;
						
			self->harvest_done=1;
			PRINT_MESSAGE(xpn,1,"harvest");// for debug
            //End of Hong
            
            }
			
	//Begin of Hong: tillage and irrigation 2017/01/11#########################################################
		if (checkIfIrrigation(self) == 1) 
        {
            printf("irrigation: \n");              
  
            irr = g_malloc0_n(1,sizeof(STIRRIGATION));
            irr_first = g_malloc0_n(1,sizeof(STIRRIGATION));
			till->acName = g_strdup_printf("irrigation");
			irr->Day = pTi->pSimTime->mday+1;
			irr->Month = pTi->pSimTime->mon;
            irr->Year = pTi->pSimTime->iyear;
            irr->fAmount = self->irrigation[self->nextIrrigation].quantity;
         

            irr->pNext = irr_first;
            irr_first->pBack = irr;                                           
            pMa->pIrrigation = NULL;
            pMa->pIrrigation = irr;

            self->nextIrrigation++;


		} //End of checkIfIrrigation
		
		if (checkIfTillage(self) == 1) 
        {
            till = g_malloc0_n(1,sizeof(STTILLAGE));
            till_first = g_malloc0_n(1,sizeof(STTILLAGE));
            till->acName = g_strdup_printf("tillage");
            till->Day = pTi->pSimTime->mday+1;
            till->Month = pTi->pSimTime->mon;
            till->Year = pTi->pSimTime->iyear;
            till->fDepth = self->tillage[self->nextTillage].fDepth;
			//till->fEffMix = self->tillage[self->nextTillage].fEffMix;
			//till->fEffLoose = self->tillage[self->nextTillage].fEffLoose;
			//till->fEffIncorp = self->tillage[self->nextTillage].fEffIncorp;
			
            
            till->acCode =  self->tillage[self->nextTillage].code;
            till->pNext = till_first;
            till_first->pBack = till;                                           
            pMa->pTillage = NULL;
            pMa->pTillage = till;
					
            self->nextTillage++;
				
        } // End of checkIfTillage
		
			
		if(checkIfGridSwitch(self) == 1)
		{ 	  	 
		 self->tillageAfterHarvest_done =1;		 
	     //PRINT_MESSAGE(xpn,1,"time for switch");  //for debug	 		 
		}
							  
    } //End of Newday
		
    if ((xpn_time_compare_date(pTi->pSimTime->iyear,pTi->pSimTime->mon,pTi->pSimTime->mday,self->mpmas_to_xn->stopDate.year,self->mpmas_to_xn->stopDate.month,self->mpmas_to_xn->stopDate.day)==0)&&(NewDay(pTi)))
        {   

			self->stopDate.year= pTi->pSimTime->iyear;
			self->stopDate.month=pTi->pSimTime->mon;
			self->stopDate.day=pTi->pSimTime->mday;
			
			self->xn_to_mpmas2->stopDay = (int)pTi->pSimTime->fTimeY;
            
            self->restart = 1;
			
            return 2;
        }
        
        
        
    //if ((xpn_time_compare_date(pTi->pSimTime->iyear,pTi->pSimTime->mon,pTi->pSimTime->mday,pPl->pModelParam->HarvestYear,pPl->pModelParam->HarvestMonth,pPl->pModelParam->HarvestDay)>0)&&(NewDay(pTi)))
    //      printf("plant growth %d maturity %d \n", pPl->pDevelop->bPlantGrowth, pPl->pDevelop->bMaturity);
    //self->WRITE_mpmas_xn->mass=self->Plant_Mass;
    return RET_SUCCESS;
}

//NEW:
//----------------------------------------------------
//Determine if crop is to be harvested today
//function to be called once a day
//return codes: 0 - no, 1- yes 
//requires a counter 'daysSinceBBCH2' to be maintained within caller
int checkIfHarvest(mpmas_coupling *self) 
    {
    expertn_modul_base *xpn = &(self->parent);
    
    PPLANT pPl = xpn->pPl;
    PTIME pTi = xpn->pTi;
    
    int currentBBCH;
    int currentDay, currentMonth, currentYear;
    
    currentBBCH = pPl->pDevelop->fDevStage;
    
    currentYear = pTi->pSimTime->iyear;
    currentMonth = pTi->pSimTime->mon;
    currentDay = pTi->pSimTime->mday;
    
	//Begin of Hong: only the current grid needs to checkIfHarvest
  if (self->mpmas_to_xn->own_grid_number==self->previousGrid)
	{	
	
    //If the decision is not adaptive, no harvest at this day
    if ( ! (self->harvestAdaptive) ) 
    {   
            //first check fixed harvest date and return 1 if date has been reached
            //If the decision is not marked as adaptive, it is the only criterion
            //If the decision is adaptive it serves as a backstop and should be set to the latest possible harvest
        if (currentDay == pPl->pModelParam->HarvestDay
        && currentMonth == pPl->pModelParam->HarvestMonth
        && currentYear == pPl->pModelParam->HarvestYear ) 
            {
				//Hong for debug:
				//printf("defined harvest date reached\n");
            return 1;
            }
        return 0;
    }
    //If the decision is adaptive ...
    else {
        //check whether maturity (BBCH1) has been reached
        if (currentBBCH >= self->harvestBBCH1 ) {
            if (self->daysSinceBBCH1 >= self->harvestBBCH1ExtraDays) {
				
				//Hong for debug:
				//printf("defined harvest BBCH1= %d reached\n",self->harvestBBCH1);
				//printf("defined harvestBBCH1ExtraDays= %d reached\n",self->daysSinceBBCH1);
                return 1;
            }
            //if we are > BBCH1, but not over the limit, increase the counter of days
            else {
                self->daysSinceBBCH1 += 1;
            }
        }
        //if maturity has not been reached, but we are close (> BBCH2)
        // check the number of days for which we have been > BBCH2
        // and harvest in case we are over the limit given for that stage 
        //(this is because in some cases the actual maturity will not be reached at all)
        else if (currentBBCH >= self->harvestBBCH2 ) {
            if (self->daysSinceBBCH2 >= self->harvestBBCH2ExtraDays) {
				
				//Hong for debug:
				//printf("defined harvest BBCH2= %d reached\n",self->harvestBBCH2);
				//printf("defined harvestBBCH2ExtraDays= %d reached\n",self->daysSinceBBCH2);
				
                return 1;
            }
            //if we are > BBCH2, but not over the limit, increase the counter of days
            else {
                self->daysSinceBBCH2 += 1;
                return 0;
            }
        }
        else if (currentDay == pPl->pModelParam->HarvestDay
        && currentMonth == pPl->pModelParam->HarvestMonth
        && currentYear == pPl->pModelParam->HarvestYear ) 
            {
				//Hong for debug:
				printf("defined harvest date do have reached\n");
				
            return 1;
            }
    }
	
	} //End of ownGrid==previousGrid
		
    return 0;
}

//---------------------------------------

int checkIfTillage(mpmas_coupling *self)
{

    expertn_modul_base *xpn = &(self->parent);
    
    PPLANT pPl = xpn->pPl;
    PTIME pTi = xpn->pTi;
    
    int daysBeforeAfter;
    int currentDay, currentMonth, currentYear;
	
	xnmpmasDate adaptiveTillageDate;
	
	int adaptiveDays;
    
    
    currentYear = pTi->pSimTime->iyear;
    currentMonth = pTi->pSimTime->mon;
    currentDay = pTi->pSimTime->mday;
    
    //security check to make sure the next tillage is a valid action
    if (self->nextTillage >= XNMPMASTILLAGESLOTS || self->nextTillage >= self->numTill ) {
        return 0;
    }
	
	
	//1. if timing of tillage is fixed, check whether indicated date has been reached
    if (self->tillage[self->nextTillage].typeAdaptiveTillage==adaptiveTillageNotAdaptive) 
	{

	//one day before the indicated day (else too late for calling teh function)
      if (xpn_time_compare_date(self->tillage[self->nextTillage].tillDate.year,self->tillage[self->nextTillage].tillDate.month,self->tillage[self->nextTillage].tillDate.day,currentYear,currentMonth,currentDay+1)==0)
	   {
		// PRINT_MESSAGE(xpn,1,"tomorrow is default TILLAGE date");  //for debug
		 return 1;
		}	
	}	


	//2. if timing of tillage is adaptive
	//2.1 tillage before sowing of crop

	if (self->tillage[self->nextTillage].typeAdaptiveTillage==adaptiveTillageBeforeSowing)
	  {
			    
		daysBeforeAfter = self->tillage[self->nextTillage].daysBeforeAfter;		
		adaptiveTillageDate.year = self->mpmas_to_xn->sowDate.year;
		adaptiveTillageDate.month = self->mpmas_to_xn->sowDate.month;
		adaptiveTillageDate.day = self->mpmas_to_xn->sowDate.day;
		xpn_time_date_add_dt(&adaptiveTillageDate.year,&adaptiveTillageDate.month, &adaptiveTillageDate.day, -daysBeforeAfter);
		
		//PRINT_MESSAGE(xpn,1,"should tillage before sowDate:");
		//printf("daybeforeafter: %d\n", daysBeforeAfter);
		//printf("%d-%d-%d \n", adaptiveTillageDate.year,adaptiveTillageDate.month,adaptiveTillageDate.day );
            
		
		if (xpn_time_compare_date(currentYear,currentMonth,currentDay+1,adaptiveTillageDate.year,adaptiveTillageDate.month,adaptiveTillageDate.day)==0)
	  {
		//PRINT_MESSAGE(xpn,1,"tillage");   
		//printf("on %d days before sowing\n", daysBeforeAfter); //for debug
		return 1;
	  }
		}
	

	
	// 2.2 tillage before sowing of cover crop
	if (self->tillage[self->nextTillage].typeAdaptiveTillage==adaptiveTillageBeforeCoverCrop)
	  {
		
		daysBeforeAfter = self->tillage[self->nextTillage].daysBeforeAfter;
		adaptiveTillageDate.year = self->mpmas_to_xn->coverCropSowDate.year;
		adaptiveTillageDate.month = self->mpmas_to_xn->coverCropSowDate.month;
		adaptiveTillageDate.day = self->mpmas_to_xn->coverCropSowDate.day;
		xpn_time_date_add_dt(&adaptiveTillageDate.year,&adaptiveTillageDate.month, &adaptiveTillageDate.day, -daysBeforeAfter);
		
	
		if (xpn_time_compare_date(currentYear,currentMonth,currentDay+1,adaptiveTillageDate.year,adaptiveTillageDate.month,adaptiveTillageDate.day)==0)
	  {
		//PRINT_MESSAGE(xpn,1,"tillage");   //for debug
		//printf("on %d days before sowing of cover crop\n", daysBeforeAfter); //for debug
		return 1;
	  }
		}
		
	
	 //2.3 tillage after harvest
	if (self->harvest_done==1)
	{
	 
	if (self->tillage[self->nextTillage].typeAdaptiveTillage==adaptiveTillageAfterHarvest)
	  {
		
		daysBeforeAfter = self->tillage[self->nextTillage].daysBeforeAfter;
				
		//PRINT_MESSAGE(xpn,1,"time for tillage after harvest on:"); //for debug	
		adaptiveTillageDate.year = pPl->pModelParam->HarvestYear;
		adaptiveTillageDate.month = pPl->pModelParam->HarvestMonth;
		adaptiveTillageDate.day = pPl->pModelParam->HarvestDay;
		xpn_time_date_add_dt(&adaptiveTillageDate.year,&adaptiveTillageDate.month, &adaptiveTillageDate.day, daysBeforeAfter);
				
	  if (xpn_time_compare_date(currentYear,currentMonth,currentDay+1,adaptiveTillageDate.year,adaptiveTillageDate.month,adaptiveTillageDate.day)==0)
	  {
		//self->tillageAfterHarvest_done=1;  
		self->dataTransferDate.year= adaptiveTillageDate.year;
        self->dataTransferDate.month= adaptiveTillageDate.month;
		self->dataTransferDate.day= adaptiveTillageDate.day;
		xpn_time_date_add_dt(&self->dataTransferDate.year,&self->dataTransferDate.month, &self->dataTransferDate.day, 1);
		self->checkSwitchDate_done=1;

		return 1;
	  }
	  
	  }
	   
	  }

		 
    return 0; //actually never reached
}





//Determine if next mineral fertilizing event is to take place today
//function to be called once a day
//return codes: 0 - no, 1- yes
//assumes that the counter which fertilization action is next 'nextMinFertAction' is maintained by the caller
int checkIfMineralFertilization(mpmas_coupling *self)
{

    expertn_modul_base *xpn = &(self->parent);
    
    PPLANT pPl = xpn->pPl;
    PTIME pTi = xpn->pTi;
    
    int currentBBCH;
    int currentDay, currentMonth, currentYear;
    
    currentBBCH = pPl->pDevelop->fDevStage;
    
    currentYear = pTi->pSimTime->iyear;
    currentMonth = pTi->pSimTime->mon;
    currentDay = pTi->pSimTime->mday;
    
    //security check to make sure the next fertilization action is a valid action
    if (self->nextMinFertAction >= XNMPMASMINFERTSLOTS || self->nextMinFertAction >= self->numMinFert ) {
        return 0;
    }
    //if timing of fertilization is adaptive, check whether indicated BBCH stage has been reached
    if ( self->mineralFertilization[self->nextMinFertAction].adaptive ) {   
        if (currentBBCH >= self->mineralFertilization[self->nextMinFertAction].bbch)
            return 1;
        else
            return 0;
    }
    //else if timing of fertilization is fixed, check whether indicated date has been reached
    else {
        if (currentDay == self->mineralFertilization[self->nextMinFertAction].fertDate.day 
            && currentMonth == self->mineralFertilization[self->nextMinFertAction].fertDate.month 
            && currentYear == self->mineralFertilization[self->nextMinFertAction].fertDate.year ) {
            return 1;
        }

        return 0;
    }
    return 0; //actually never reached
}

int checkIfOrganicFertilization(mpmas_coupling *self)
{

    expertn_modul_base *xpn = &(self->parent);
    
    PPLANT pPl = xpn->pPl;
    PTIME pTi = xpn->pTi;
    
    int currentBBCH;
    int currentDay, currentMonth, currentYear;
    
    currentBBCH = pPl->pDevelop->fDevStage;
    
    currentYear = pTi->pSimTime->iyear;
    currentMonth = pTi->pSimTime->mon;
    currentDay = pTi->pSimTime->mday;
    
    //security check to make sure the next fertilization action is a valid action
    if (self->nextOrgFertAction >= XNMPMASORGFERTSLOTS || self->nextOrgFertAction >= self->numOrgFert ) {
        return 0;
    }
    //if timing of fertilization is adaptive, check whether indicated BBCH stage has been reached
    if ( self->organicFertilization[self->nextOrgFertAction].adaptive ) {   
        if (currentBBCH >= self->organicFertilization[self->nextOrgFertAction].bbch)
            return 1;
        else
            return 0;
    }
    //else if timing of fertilization is fixed, check whether indicated date has been reached
    else {
        if (currentDay == self->organicFertilization[self->nextOrgFertAction].orgfertDate.day 
            && currentMonth == self->organicFertilization[self->nextOrgFertAction].orgfertDate.month 
            && currentYear == self->organicFertilization[self->nextOrgFertAction].orgfertDate.year ) {
            return 1;
        }

        return 0;
    }
    return 0; //actually never reached
}

//Begin of Hong: for tillage, irrigation 2017/01/11
//---------------------------------------
//Determine if next irrigation event is to take place today
//function to be called once a day
//return codes: 0 - no, 1- yes
//assumes that the counter which irrigation is next 'nextIrrigation' is maintained by the caller
int checkIfIrrigation(mpmas_coupling *self)
{

    expertn_modul_base *xpn = &(self->parent);
    
    PPLANT pPl = xpn->pPl;
    PTIME pTi = xpn->pTi;
    
    int currentBBCH;
    int currentDay, currentMonth, currentYear;
    
    currentBBCH = pPl->pDevelop->fDevStage;
    
    currentYear = pTi->pSimTime->iyear;
    currentMonth = pTi->pSimTime->mon;
    currentDay = pTi->pSimTime->mday;
    
    //security check to make sure the next irrigation is a valid action
    if (self->nextIrrigation >= XNMPMASIRRIGATIONSLOTS || self->nextIrrigation >= self->numIrrig ) {
        return 0;
    }
	
    //check whether indicated date has been reached
	if (currentDay == self->irrigation[self->nextIrrigation].irrDate.day 
            && currentMonth == self->irrigation[self->nextIrrigation].irrDate.month 
            && currentYear == self->irrigation[self->nextIrrigation].irrDate.year ) {
            return 1;
        }
        
    return 0; //actually never reached
}


int checkIfGridSwitch(mpmas_coupling *self) 
{
	 expertn_modul_base *xpn = &(self->parent);
	 PPLANT pPl = xpn->pPl;
	 PTIME pTi = xpn->pTi;
	 
	 if ((self->checkSwitchDate_done==0)&&(self->tillage[self->nextTillage].typeAdaptiveTillage==adaptiveTillageNotAdaptive))
	  {
		self->dataTransferDate.year= pPl->pModelParam->HarvestYear;
        self->dataTransferDate.month= pPl->pModelParam->HarvestMonth;
		self->dataTransferDate.day= pPl->pModelParam->HarvestDay;
		xpn_time_date_add_dt(&self->dataTransferDate.year,&self->dataTransferDate.month, &self->dataTransferDate.day, 2);
		self->checkSwitchDate_done=1;
		
		//PRINT_MESSAGE(xpn,1,"no tillage after harvest, switchDate on:");  //for debug
		//printf("%d-%d-%d \n", self->dataTransferDate.year,self->dataTransferDate.month,self->dataTransferDate.day );
      }
	  
	  if((self->tillageAfterHarvest_done==0)&&(xpn_time_compare_date(pTi->pSimTime->iyear,pTi->pSimTime->mon,pTi->pSimTime->mday,self->dataTransferDate.year,self->dataTransferDate.month,self->dataTransferDate.day)==0))
      {
       return 1;    
	  }
	  
	  
	
	return 0;
	}

//End of Hong
int get_daily_air_and_soil_temperatures(mpmas_coupling *self)
{
    expertn_modul_base *xpn = &(self->parent);
    PTIME pTi = xpn->pTi;
    PCLIMATE pCl = xpn->pCl;
    PWEATHER pWe = pCl->pWeather;
    PHEAT pHe = xpn->pHe;
    PSPROFILE pSo = xpn->pSo;
    PWATER pWa = xpn->pWa;
    PSLAYER  pSL;
    PWLAYER  pWL;
    PHLAYER pHL;
    
    double dt;
    
    double depth, act_depth;
    
    int i;
    
    int dayofyear;

    if ((pTi->pSimTime->fTimeY==self->temperature.fTimeY_save) && (pTi->pSimTime->iyear == self->temperature.iyear_save)) {
        return RET_SUCCESS;
    } else {
        self->temperature.fTimeY_save = pTi->pSimTime->fTimeY;
        self->temperature.iyear_save = pTi->pSimTime->iyear;
    }

    dt = pTi->pTimeStep->fAct;


    if ((int)NewDay(pTi)) {
        
        dayofyear = ((int)pTi->pSimTime->fTimeY) - 1; //average written for previous day, when new day has already started (and index < 0)
        if (dayofyear == -1)
        {
            dayofyear = 365 + is_leap_year(pTi->pSimTime->iyear - 1) -1;
        }
        
        
        self->temperature.fTempMax = self->__temperature.fTempMax;
        self->temperature.fTempMin = self->__temperature.fTempMin;
        self->temperature.fTempAve = self->__temperature.fTempAve;
        self->temperature.fTempSoil5cm = self->__temperature.fTempSoil5cm;

        if(self->simulation_days >= 1)
            {
            self->xn_to_mpmas2->airTemp[dayofyear] = self->__temperature.fTempAve;
            self->xn_to_mpmas2->topsoilTemp[dayofyear] = self->__temperature.fTempSoil5cm;
            }
        
        //printf("%f %f %f %f \n", self->temperature.fTempMax, self->temperature.fTempMin, self->temperature.fTempAve, self->temperature.fTempSoil5cm );

        self->__temperature.fTempMax= -99.0;
        self->__temperature.fTempMin= 99.0;     
        self->__temperature.fTempAve= 0.0;
        self->__temperature.fTempSoil5cm = 0.0;
        self->simulation_days++;
        
    } else {
        if ((pWe->fTempAir > self->__temperature.fTempMax)&&(pTi->pSimTime->fTimeDay > 0.5)) {
            self->__temperature.fTempMax = pWe->fTempAir_daily_models;
        }
        if ((pWe->fTempAir < self->__temperature.fTempMin)&&(pTi->pSimTime->fTimeDay < 0.5)) {
            self->__temperature.fTempMin = pWe->fTempAir_daily_models;
        }

        self->__temperature.fTempAve += pWe->fTempAir_daily_models * dt;

        depth = 0.05; //m
        act_depth = 0.0; //m

        for(i=0,pSL=pSo->pSLayer->pNext,pHL=pHe->pHLayer->pNext, pWL=pWa->pWLayer->pNext; pSL->pNext!=NULL; pSL=pSL->pNext, pHL=pHL->pNext, pWL=pWL->pNext, i++)
            {
            act_depth +=pSL->fThickness*1.0e-3; // mm -> m
            if (act_depth >= depth)
                {
                self->__temperature.fTempSoil5cm += pHe->pHLayer->pNext->fSoilTemp * dt;
                break;
                }
            }


    }

    /*self->weather.fTempAve = *(self->TairMean);
    self->weather.fTempMin = *(self->TairMin);
    self->weather.fTempMax = *(self->TairMax);
    self->weather.fDaylySolRad = *(self->fSolRad);*/
    return RET_SUCCESS;
}

//END NEW
