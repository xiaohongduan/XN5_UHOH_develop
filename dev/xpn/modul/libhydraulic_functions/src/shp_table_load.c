//#include "stickstoff.h"
#include "shp_look_up_table.h"
#include "hydraulic_functions.h"


int shp_table_load_ini(hydraulic_functions *self)
//int shp_table_load_ini(expertn_standard_ini *self)
{
	expertn_modul_base *xpn = &(self->parent);
	char *ini_filename;
	
	if (self->ini_filename==NULL)
		{
			// Read from INI File:
			ini_filename = xpn_register_var_get_pointer(self->parent.pXSys->var_list,"Config.shp_table.filename");

			if (ini_filename==NULL)
				{
					PRINT_ERROR("Missing entry 'shp_table.ini_filename' in your configuration!");
				}
			else
				{

					char *S2;
					S2 = expertn_modul_base_replace_std_templates(xpn,ini_filename);
					if (S2!=NULL)
						{
							ini_filename = get_fullpath_from_relative(self->parent.pXSys->base_path, S2);
							self->ini_filename = g_strdup_printf("%s",ini_filename);
//							shp_table_load_config(self);
							free(ini_filename);
							free(S2);
						}

				}

		}

	return RET_SUCCESS;
}


int shp_table_load_config(hydraulic_functions *self)
//int shp_table_load_config(expertn_standard_ini *self)
{
	GError *error = NULL;
	expertn_modul_base *xpn = &(self->parent);
	//PCPROFILE pCProfile = xpn->pCh->pCProfile;	//Added by hong on 20190926
	//PCLAYER		pCL;  //SG20190730
	//PSLAYER		pSL;
    
     PSLAYER pSLayer;
     PSLAYER pSL;
	 PSWATER pSWater;
     PWLAYER pWL;
	 PSWATER      pSW;
     PWATER pWa = xpn->pWa;
     PSPROFILE pSo = xpn->pSo;
     pWL= xpn->pWa->pWLayer; 
	 pSW = xpn->pSo->pSWater;
	
     int iLayer, iHorizon; 


    int  i, i4; //SG 20180921


	GKeyFile *keyfile;
	GKeyFileFlags flags;
	char *filename;
	int layers_len;
	int *layers;
	double *var;
	int var_len;
    
    char *S;
	
	var = NULL;
	var_len=0;
	layers_len=0;
	layers=NULL;


	filename=g_strdup_printf("%s",self->ini_filename);

	/* Create a new GKeyFile object and a bitwise list of flags. */
	keyfile = g_key_file_new ();

	flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

	/* Load the GKeyFile from keyfile.conf or return. */
	if (!g_key_file_load_from_file (keyfile, filename, flags, &error))
		{
			PRINT_ERROR(error->message);
			return -1;
		}

    //SG 20200326: read Look-up table
    
    GET_INI_INT_ARRAY(layers,layers_len,"soil hydraulic properties table","layers");
    
  /* iLayersCum = 0; 
    for(i=0,i<layers_len,i++)
    {
        iLayersCum += layers[i-1];
        iEnd=layers[i-1];
    }
    */
    
    pSLayer=xpn->pSo->pSLayer;
    pSWater=xpn->pSo->pSWater;
    
    for (iHorizon=0;iHorizon<layers_len;iHorizon++) //layers_len: number of horizons defined in project_shp.ini
    {
        //printf("READING soil hydraulic properties tables for horizon %i \n", iHorizon+1);
        S  = g_strdup_printf("READING soil hydraulic properties tables for horizon %i ", iHorizon+1);
        PRINT_MESSAGE(xpn,2,S);
        g_free(S);

       GET_INI_DOUBLE_ARRAY(self->shp_table_mat_pot,self->shp_table_mat_pot_len,"soil hydraulic properties table","MatPot");
       GET_INI_DOUBLE_ARRAY(self->shp_table_wat_cont,self->shp_table_wat_cont_len,"soil hydraulic properties table","WatCont");
       GET_INI_DOUBLE_ARRAY(self->shp_table_h_cond,self->shp_table_h_cond_len,"soil hydraulic properties table","HCond");
       GET_INI_DOUBLE_ARRAY(self->shp_table_dif_wc,self->shp_table_dif_wc_len,"soil hydraulic properties table","DifWC");
        
       // Array Groessen muessen alle gleich lang sein, sonst: Fehlermeldung und abbrechen
       CHECK_LEN(self->shp_table_mat_pot_len,self->shp_table_wat_cont_len);
       CHECK_LEN(self->shp_table_mat_pot_len,self->shp_table_h_cond_len);
       CHECK_LEN(self->shp_table_mat_pot_len,self->shp_table_dif_wc_len);
       
       
        for (iLayer=1; iLayer<=layers[iHorizon],pSLayer!=NULL; pSLayer=pSLayer->pNext,pSWater=pSWater->pNext,iLayer++)
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
        }
   
   
    }
   
    G_FREE_IF_NOT_0(self->shp_table_mat_pot);
    G_FREE_IF_NOT_0(self->shp_table_wat_cont);
    G_FREE_IF_NOT_0(self->shp_table_h_cond);
    G_FREE_IF_NOT_0(self->shp_table_dif_wc);
               
	G_FREE_IF_NOT_0(layers);
	layers_len=0;

	g_key_file_free(keyfile);
	g_free(filename);

	return RET_SUCCESS;
}
