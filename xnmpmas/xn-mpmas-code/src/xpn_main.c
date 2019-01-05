/*
 * xpn_main.c
 *
 * Copyright 2014 chrikle <christian.klein@helmholtz-muenchen.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 */

#include "xpn_main.h"

static char* create_string_from_source(char* src)
{
	char *S;
	if (src==NULL) return NULL;
	S = (char*) malloc(strlen(src)+1);
	strcpy(S,src);
	return S;

}

static void xpn_main_finalize (GObject *object);

G_DEFINE_TYPE (xpn_main, xpn_main, G_TYPE_OBJECT)


static void xpn_main_class_init (xpn_mainClass *klass)
{
	GObjectClass *g_object_class;

	g_object_class = G_OBJECT_CLASS (klass);

	g_object_class->finalize = xpn_main_finalize;

}


static void xpn_main_finalize (GObject *object)
{
	g_return_if_fail (IS_XPN_MAIN (object));
	G_OBJECT_CLASS (xpn_main_parent_class)->finalize (object);
}


static void xpn_main_init (xpn_main *self) {}


xpn_main *xpn_main_new (int rank,int numtasks)
{
	xpn_main *self;
	// glib Klassen initiatisieren
	g_type_init();

	// Speicher Initialisieren:
	self = g_object_new (TYPE_XPN_MAIN, NULL);
	self->netf = NULL;
	self->MPI_act = 0;
	self->OPENMP_act = 0;
	self->numtasks=numtasks;
	self->rank=rank;

#ifdef USE_MPI
	self->MPI_act=1;
#endif

#ifdef USE_OPENMP
	self->OPENMP_act=1;
#endif

	self->cfg_file=NULL;
	self->base_dir=NULL;

	self->back = 0;

	self->loop_min=0;
	self->loop_max=0;

	self->domain_list=NULL;
	self->domain_list_size=0;

	self->cfg=NULL;
	self->xpn_class=NULL;
	self->drv=NULL;
	self->modul_list=NULL;
	return self;
}

int xpn_main_parse_command_line(xpn_main *self, int argc, char **argv)
{
	// Variablen für command line parser:
	GError *error = NULL;
	GOptionContext *context;
	//options var:
	gboolean xpn_version = FALSE;
	gboolean xpn_modules = FALSE;
	gchar* cfg_file = NULL;
	gchar* base_dir = NULL;
	gboolean console_modus = FALSE;
	//options:
	GOptionEntry entries[] =
	{
		{ "version", 'v', 0, G_OPTION_ARG_NONE, &xpn_version, "Expert - N Version", NULL },
		{ "show-modules", 'm', 0, G_OPTION_ARG_NONE, &xpn_modules, "Used Modules", NULL },
		{ "config-file",'c',0,G_OPTION_ARG_STRING,&cfg_file, "Config - File",NULL},
		{ "base-dir",'d',0,G_OPTION_ARG_STRING,&base_dir, "Base - Directory",NULL},
		{ "console_modus",'n',0,G_OPTION_ARG_NONE,&console_modus, "Console - Modus",NULL},
		{ NULL }
	};

	// Command Line Parser:
	context = g_option_context_new ("- Expert-N (Stand Alone Version)");
	g_option_context_add_main_entries (context, entries, NULL); // für unterschiedliche Sprachen: GETTEXT_PACKAGE

	if (!g_option_context_parse (context, &argc, &argv, &error))
		{
			g_print ("option parsing failed: %s\n", error->message);
			g_option_context_free(context);
#ifdef USE_MPI
			MPI_Finalize();
#endif
			return 1;
		}

	if (xpn_version)
		{
			g_option_context_free(context);
#ifdef XPN_VERSION
			printf("Expert - N Version: %s\n",XPN_VERSION);
#else
			printf("Expert - N Version: 5.0 Rev 0\n");
#endif
#ifdef USE_MPI
			MPI_Finalize();
#endif
			return 1;
		}

	if (cfg_file==NULL)
		{
			g_option_context_free(context);
			printf("No config file defined, use '--help' to get more information!\n");
#ifdef USE_MPI
			MPI_Finalize();
#endif
			return 1;
		}

	if (base_dir==NULL)
		{
			char *S,*S2;
			S = g_get_current_dir();
			S2=delete_last_part_of_path(S);
			base_dir = g_strdup_printf("%s",S2);
			printf("No base path defined, '%s' is used!\n",base_dir);
			g_free(S);
			//free(S2);
		}

	self->cfg_file=cfg_file;
	self->base_dir=base_dir;

	return RET_SUCCESS;
}

int xpn_main_load_cfg_and_reserve_memory(xpn_main *self)
{
	int count;
	char *S2;
	char *project_name,*reg_str,*project_path;
	int i,i2,i3,i4,i5;
	int *nest_ids;
	int x1,x2,y1,y2;
	int id;
	xpn_register_var *reg;
	
	//Begin of Hong	
	int gridId, oneGridsize, cellNumber;
	int cellID, gridId2, cellID2;
	int group;// ~ count // for MPI
	int k,g; // for MPI
	int r,l,n, z, last_n; // for MPI
    int *XPNforRanks, *recordRank; // for MPI

	// End of Hong

	// Speicher für CFG:
	self->cfg = general_config_new();

	// Config lesen
	if (general_config_load_config_file(self->cfg,self->cfg_file)!=RET_SUCCESS)
		{
#ifdef USE_MPI
			MPI_Finalize();
#endif
			exit(1);
		}

	self->domain_list = general_config_get_grid_id_list(self->cfg,&(self->domain_list_size));

	// Es werden alle Namen der Modulle und Klassen die möglich sind geladen (werden aus der dll entnommen)
	self->modul_list = register_modul_load_modul_list(self->cfg->expertn_lib_list,self->cfg->expertn_lib_list_len,self->base_dir);

	count = self->cfg->number_of_xpn;
	
	printf("Info: Number of xpn instances according to configfile: %d\n", count);
	
	nest_ids = general_config_get_grid_id_list(self->cfg,&(self->grid_layers));
	
	printf("Info: Number of xpn grid layers: %d\n", self->grid_layers );
	
	if (self->grid_layers<=0)
		{
			PRINT_ERROR("grid_layers y<=0");
			return 1;
		}
	
	for (i=0;i<self->grid_layers;i++) 
	{
		id = nest_ids[i] ;
		printf("Info: Processing nestid: %d (#%d)\n", id , i );
		
		if (id != i)
			{	 char* S;
				 S = g_strdup_printf("inconsistent grid numbering %d", id);
				 PRINT_ERROR(S);
				 g_free(S);	
				return 1;
			}
		
		general_config_get_grid_size_ij(self->cfg,&x1,&y1,&x2,&y2, id); //Achtung, nicht Gridgröße, sondern maximale AKTIVE Koordinate
		
		if ( (x1 != 0) || (y1 != 0) )
			{
				PRINT_ERROR("invalid numbering of lowest grid coordinate");
				return 1;			
			}
		if (id == 0)
			{
				self->matrix_size_x = x2 + 1;
				self->matrix_size_y = y2 + 1;
			}
		else 
			{
				if ( (self->matrix_size_x != x2 + 1)   || (self->matrix_size_y != y2 + 1) )
				{
						 char *S;
						 S = g_strdup_printf("inconsistent grid extents for grid %d", id);
						 PRINT_ERROR(S);
					     g_free(S);					
						return 1;	
				}
			}
	}
	g_free(nest_ids);
	
	self->grid_mpmas_to_xn = g_malloc0(sizeof(STRUCT_mpmas_to_xn)*self->grid_layers * self->matrix_size_y* self->matrix_size_x);
	self->grid_xn_to_mpmas = g_malloc0(sizeof(STRUCT_xn_to_mpmas)*self->grid_layers * self->matrix_size_y* self->matrix_size_x);
	self->grid_xn_to_mpmas2 = g_malloc0(sizeof(STRUCT_xn_to_mpmas2)*self->grid_layers * self->matrix_size_y* self->matrix_size_x);

// Begin of Hong: 
    //Speicher für ...xpn_other_grids: 
     oneGridsize = self->matrix_size_x *self->matrix_size_y;    
     for (gridId=0; gridId<self->grid_layers;gridId++)
     {
        for (cellNumber=0; cellNumber< oneGridsize;cellNumber++)
        {
            self->grid_mpmas_to_xn[gridId *oneGridsize+cellNumber].xpn_other_grids = (expertn_modul_base **)g_malloc0(sizeof(expertn_modul_base *)*self->grid_layers);
           
        }
        
     }

//MPI Hong
    self->XPN_ID_tab=array_INI(oneGridsize*self->grid_layers);	
	self->sorted_XPN= array_INI(self->cfg->number_of_xpn+1);
    self->XPNforThisRank= array_INI(self->cfg->number_of_xpn+1); 
// End of MPI Hong 

#ifdef USE_MPI
	if (self->numtasks>count)
		{
			if (self->rank>count-1)
				{
					char *S;
					S = g_strdup_printf("Only %d processors are needed. Process %d is killed!\n",count,self->rank);
					PRINT_ERROR(S);
					g_free(S);
					MPI_Finalize();
					return RET_SUCCESS;
				}
			self->numtasks=count;
		}
#endif
	self->loop_min=0;
	self->loop_max=count;

	//Speicher für Expert - N Instanzen reservieren:
	self->xpn_class = (expertnclass **)calloc(count,sizeof(expertnclass *));

	self->drv = (struct_drive_xpn*) calloc(self->cfg->grid_len,sizeof(struct_drive_xpn));

#ifdef USE_OPENMP
	if ((self->cfg->num_proc<=0) || self->cfg->num_proc > omp_get_num_procs())
		{
			self->cfg->num_proc=count;
			if (count > omp_get_num_procs())
				{
					self->cfg->num_proc = omp_get_num_procs();
				}
			else if (count <= 0)
				{
					self->cfg->num_proc = 1;
				}
			else
				{
					self->cfg->num_proc = count;
				}

		}

	omp_set_num_threads(self->cfg->num_proc);
#else
	self->cfg->num_proc=1;
#endif


#ifdef USE_OPENMP
	{
		char S[128];
		sprintf(S,"%d processors are used by OpenMP!\n",self->cfg->num_proc);
		PRINT_M(self->cfg->DEBUG_LEVEL,5,S);
	}
#endif


	// Expert-N Classen initialiesieren:

	S2 = get_filename_without_path(self->cfg_file);
	project_name = get_filename_without_extension(S2);
	free(S2);

	project_path = get_path_from_filename(self->cfg_file);


//Hong MPI
// 1. arrange the XPN instances according to the cell number and save in the self->XPN_ID_tab 
k = 0;
for (i=0; i<self->cfg->grid_len; i++)
{
for (i2=0; i2<self->cfg->grid[i].instance_len; i2++)
	{
		self->XPN_ID_tab[(self->cfg->grid[i].i*self->matrix_size_y+self->cfg->grid[i].j)+(self->cfg->grid[i].id)*oneGridsize]=k;
		k++;
	}
}

// 2. read the sorted XPN instance number and save in "self->sorted_XPN" 
k=0;
while(k<self->cfg->number_of_xpn){
for(i=0;i<oneGridsize*self->grid_layers; i++)	
	{
		if((self->XPN_ID_tab[i]!=(-1)))
		{
		   self->sorted_XPN[k]=self->XPN_ID_tab[i];
		   if (k<self->cfg->number_of_xpn)
		   {
		   k++;	
		   }						
		 }		 		
	}
}	


// 3. to estimate the XPN instances that should be assigned to the given rank, then save in "XPNforThisRank" 	
	group= self->cfg->number_of_xpn/self->grid_layers;

	//simple grid (not considering grid layers) bounds for this rank
	self->loop_min_xn_to_mpmas_grid = (int)((long)(self->rank+0)*(long)(group)/(long)(self->numtasks));
	self->loop_max_xn_to_mpmas_grid = (int)((long)(self->rank+1)*(long)(group)/(long)(self->numtasks));

    k=0;
	
	for (gridId=0; gridId< self->grid_layers;gridId++)
		
		{
#ifdef USE_MPI			
			self->loop_min_grid=(int)((long)(self->rank+0)*(long)(group)/(long)(self->numtasks))+group*gridId;
			self->loop_max_grid=(int)((long)(self->rank+1)*(long)(group)/(long)(self->numtasks))+group*gridId;
#endif			
			for (g=0;g<self->cfg->number_of_xpn; g++)
				
				{
#ifdef USE_MPI					
					if ((g>=self->loop_min_grid)&&(g<self->loop_max_grid))
#endif
					{					
					self->XPNforThisRank[k]=self->sorted_XPN[g];
					k++;
					}					
				}							
			}
		
//End of Hong MPI

	i3 = 0;
	//printf("grid_len: %d\n", self->cfg->grid_len);
	for (i=0; i<self->cfg->grid_len; i++)
		{
			
			//printf("grid[%d].instance_len: %d\n", i, self->cfg->grid[i].instance_len);
			self->drv[i].xpn_N = self->cfg->grid[i].instance_len;
			self->drv[i].xpn_finished = FALSE;
			self->drv[i].xpn = (expertnclass**) calloc(self->drv[i].xpn_N,sizeof(expertnclass*));
			for (i2=0; i2<self->cfg->grid[i].instance_len; i2++)
				{
//Hong MPI
/* 
 #ifdef USE_MPI
					self->loop_min    =(int)((long)(self->rank + 0) *
					                         (long)(count)/(long)self->numtasks);
					self->loop_max    =(int)((long)(self->rank + 1) *
					                         (long)(count)/(long)self->numtasks);

					if ((i3 >= self->loop_min) && (i3 < self->loop_max))
						{
#endif
*/
//replace if ((i3 >= self->loop_min) && (i3 < self->loop_max)): ////////////////////////////
#ifdef USE_MPI

					if (isXPNforThisRank(i3, self->XPNforThisRank))
					{ 
#endif						
////////////////////////////////////////////////////////////////////////////////////////////
//End of Hong MPI	
							reg_str = g_strdup_printf("%i_%i_%i_%i",self->cfg->grid[i].id,self->cfg->grid[i].i,self->cfg->grid[i].j,i2);
							self->xpn_class[i3] = expertnclass_new(self->cfg->DEBUG_LEVEL, self->modul_list,self->cfg->grid[i].instance[i2].config_file,self->base_dir,project_name,project_path,reg_str,self->cfg->grid[i].instance[i2].xpn_id_number,self->cfg->number_of_xpn,self->cfg->config_values_len,self->cfg->config_values);
							self->drv[i].xpn[i2] = self->xpn_class[i3];
							
							
							
							g_free(reg_str);
#ifdef USE_MPI
						}
					else
						{
							self->xpn_class[i3] = NULL;
						}
#endif
					i3++;
				}

		}

	free(project_name);
	free(project_path);


	// Um auf andere Instanzen zugreifen zu können, werden jetzt die entsprechenden Verknüpfungen gemacht:
//Hong MPI
/*
#ifdef USE_MPI
	self->loop_min    =(int)((long)(self->rank + 0) *
	                         (long)(count)/(long)self->numtasks);
	self->loop_max    =(int)((long)(self->rank + 1) *
	                         (long)(count)/(long)self->numtasks);
#endif
*/
//End of Hong MPI
	i3 = 0;
	for (i=0; i<self->cfg->grid_len; i++)
		{
			double fraction;

			i5 = i3;
			fraction = 0.0;
			for (i2=0; i2<self->cfg->grid[i].instance_len; i2++)
				{
					PXSYSTEM pXSys;
//Hong MPI
/*
#ifdef USE_MPI
					if ((i3 >= self->loop_min) && (i3 < self->loop_max))
						{
#endif
*/
//replace if ((i3 >= self->loop_min) && (i3 < self->loop_max)): ////////////////////////////
#ifdef USE_MPI

					if (isXPNforThisRank(i3, self->XPNforThisRank))
					{ 
#endif						
////////////////////////////////////////////////////////////////////////////////////////////
//End of Hong MPI
							pXSys = self->xpn_class[i3]->pXSys;

							pXSys->MPI_act = self->MPI_act;
							pXSys->OPENMP_act = self->OPENMP_act;

							pXSys->reg_str_inst = (char**)calloc(self->cfg->number_of_xpn,sizeof(char*));
							pXSys->xpn_classes = (void**)calloc(self->cfg->number_of_xpn,sizeof(void*));

//Begin of Hong  
// link to other instances referring to the same soil column
                            cellID= self->cfg->grid[i].id* oneGridsize+self->cfg->grid[i].i*self->matrix_size_y+self->cfg->grid[i].j;
                            self->grid_mpmas_to_xn[cellID].xpn_own_grid=self->xpn_class[i3]->XPN_Moduls[0]->xpn;
                                                
                            for (gridId2=0; gridId2<self->grid_layers; gridId2++)
                               {
                                 cellID2= gridId2 * oneGridsize+self->cfg->grid[i].i*self->matrix_size_y+self->cfg->grid[i].j;   
                            
                                 self->grid_mpmas_to_xn[cellID2].xpn_other_grids[self->cfg->grid[i].id] = self->xpn_class[i3]->XPN_Moduls[0]->xpn;
                               }
                                                                        
// End of Hong 

							// Das hier muss für MPI noch mal überarbeitet werden:
							//for (i4=0; i4 < cfg->number_of_xpn; i4++) {
//Hong MPI
							//for (i4=self->loop_min; i4 < self->loop_max; i4++)
							k=0;
 							while (self->XPNforThisRank[k]!=(-1))		
								{
									i4=self->XPNforThisRank[k];
									pXSys->reg_str_inst[i4] = create_string_from_source(self->xpn_class[i4]->pXSys->reg_str);
									pXSys->xpn_classes[i4] = (void*)self->xpn_class[i4];
									k++;
								}
//End of Hong MPI
							if (self->cfg->grid[i].instance_len==1)
								{
									pXSys->landuse_cover_fraction = 1.0;
								}
							else
								{
									pXSys->landuse_cover_fraction = self->cfg->grid[i].instance[i2].cover/100.0;
								}
							fraction+=pXSys->landuse_cover_fraction;
							pXSys->id_M = self->cfg->grid[i].instance_len;


							pXSys->id_Mosaic = (int*)calloc(self->cfg->grid[i].instance_len,sizeof(int));
							for (i4=0; i4<self->cfg->grid[i].instance_len; i4++)
								{
									pXSys->id_Mosaic[i4] = i5+i4;
								}
								
							{
							//get_full_grid_info(int *_grid_id,int *_grid_i,int *_grid_j,int *_grid_k, char* reg_str);							
							// Registry holen
							reg = expertnclass_get_varlist(self->drv[i].xpn[i2]);
							
							int simpleGridSize = self->matrix_size_x * self->matrix_size_y;
							xpn_register_var_add_none(reg,"p_grid_mpmas_to_xn",&(self->grid_mpmas_to_xn[self->cfg->grid[i].id * simpleGridSize +self->cfg->grid[i].i*self->matrix_size_y+self->cfg->grid[i].j]),-1,TRUE);
							xpn_register_var_add_none(reg,"p_grid_xn_to_mpmas",&(self->grid_xn_to_mpmas[self->cfg->grid[i].id * simpleGridSize +self->cfg->grid[i].i*self->matrix_size_y+self->cfg->grid[i].j]),-1,TRUE);							
							xpn_register_var_add_none(reg,"p_grid_xn_to_mpmas2",&(self->grid_xn_to_mpmas2[self->cfg->grid[i].id * simpleGridSize +self->cfg->grid[i].i*self->matrix_size_y+self->cfg->grid[i].j]),-1,TRUE);	
							}

#ifdef USE_MPI
						}
#endif
					i3++;
				}
		}
	self->netf = xpn_output_netcdf_new(self->xpn_class,self->cfg->number_of_xpn,self->cfg->config_values,self->cfg->config_values_len,self->cfg);
	//g_atexit(exit_NETCDF);

	// ... hier können jetzt zusätzliche registry Einträge gemacht werden:
	// ... --> xpn_register_var *expertnclass_get_varlist(expertnclass *self);



	// für MPI werden loop_min und loop_max auf die Prozesoren aufgeteilt:

//Hong MPI	
#ifdef USE_MPI
    if (self->rank==0)
		{
			{
				char S[128];
				sprintf(S,"%d processors are used by MPI!\n",self->numtasks);
				PRINT_M(self->cfg->DEBUG_LEVEL,3,S);
			}
		}
     
	 // Rank der anderen
	
    XPNforRanks=array_INI(self->cfg->number_of_xpn+1);
	
	recordRank=array_INI(self->cfg->number_of_xpn+1);
	

	
	
	k=0;
    last_n=0; 
	for (r=0; r<self->numtasks; r++)
		
		{   
			//1. find the XPNforThisRank while r=0...n, and save in XPNforRanks 
			n=0;
	        for (gridId=0; gridId< self->grid_layers;gridId++)		
		    {
			self->loop_min_grid=(int)((long)(r+0)*(long)(group)/(long)(self->numtasks))+group*gridId;
			self->loop_max_grid=(int)((long)(r+1)*(long)(group)/(long)(self->numtasks))+group*gridId;
						    
			
			  for (g=0;g<self->cfg->number_of_xpn; g++)
				
				{
					if ((g>=self->loop_min_grid)&&(g<self->loop_max_grid))
					{
					XPNforRanks[k]=self->sorted_XPN[g];
					k++;
					n++;
					}	
                    				
				}							
			}
			
			//2. loop XPNforRanks[l] to record its rank
			
            for (l=last_n; l<n+last_n; l++)	
			{					
				z= XPNforRanks[l];
				recordRank[z]=r;
			}     
		     
			last_n=n; 
			
			
		}
		

		for(i=0;i<self->cfg->number_of_xpn; i++)
		{
			if (isXPNforThisRank(i, self->XPNforThisRank))
			{
			  
				  self->xpn_class[i]->pXSys->MPI_rank=self->rank;
				  self->xpn_class[i]->pXSys->MPI_size=self->numtasks;
				  self->xpn_class[i]->pXSys->MPI_ranks=(int*)g_malloc0(sizeof(int)*self->cfg->number_of_xpn);// ?				  
		          //memcpy(self->xpn_class[i]->pXSys->MPI_ranks, recordRank, sizeof(self->xpn_class[i]->pXSys->MPI_ranks));
		          memcpy(self->xpn_class[i]->pXSys->MPI_ranks, recordRank, sizeof(int)*self->cfg->number_of_xpn); //test to correct warning
		          
			}
		}
	 
#endif	
//End of Hong MPI	
/*
#ifdef USE_MPI
	self->loop_min    =(int)((long)(self->rank + 0) *
	                         (long)(count)/(long)self->numtasks);
	self->loop_max    =(int)((long)(self->rank + 1) *
	                         (long)(count)/(long)self->numtasks);


	//printf("count: %d, numtasks: %d\n",count,numtasks);
	//printf("Prozess ID: %d, loop_min: %d, loop_max %d\n",rank,loop_min,loop_max);

	if (self->rank==0)
		{
			{
				char S[128];
				sprintf(S,"%d processors are used by MPI!\n",self->numtasks);
				PRINT_M(self->cfg->DEBUG_LEVEL,3,S);
			}
		}


	{
		// Rank der anderen
		for (i2=self->loop_min; i2<self->loop_max; i2++)
			{
				self->xpn_class[i2]->pXSys->MPI_rank=i;
				self->xpn_class[i2]->pXSys->MPI_size=self->numtasks;
				self->xpn_class[i2]->pXSys->MPI_ranks=(int*)g_malloc0(sizeof(int)*count);
				for (i=0; i<count; i++)
					{
						int k;
						k= (int)((double)i/(double)self->numtasks * (double)count);
						self->xpn_class[i2]->pXSys->MPI_ranks[i]=k;
					}
			}

	}


#endif
*/
	return RET_SUCCESS;
}

int xpn_main_save_current_state_of_registry(xpn_main *self)
{

/*int i, i2, i3, i4;
int j;	
	
//self->save_registry = (struct_variable***)malloc(self->save_registry,sizeof(struct_variable**)*self->xpn_class[0]->pXSys->id_N);	

#ifdef USE_MPI
	self->loop_min    =(int)((long)(self->rank + 0) *
	                         (long)(count)/(long)self->numtasks);
	self->loop_max    =(int)((long)(self->rank + 1) *
	                         (long)(count)/(long)self->numtasks);
#endif
	i3 = 0;
	for (i=0; i<self->cfg->grid_len; i++)
		{
			for (i2=0; i2<self->cfg->grid[i].instance_len; i2++)
				{
					PXSYSTEM pXSys;
#ifdef USE_MPI
					if ((i3 >= self->loop_min) && (i3 < self->loop_max))
						{
#endif
							pXSys = self->xpn_class[i3]->pXSys;
							j = 0;
							for (i4=self->loop_min; i4 < self->loop_max; i4++)
								{
									self->save_registry[j] = pXSys->var_list[i4].vars;
									j++;
								}
#ifdef USE_MPI
						}
#endif
					i3++;
				}
		}*/
		
return RET_SUCCESS;
}


int xpn_main_run(xpn_main *self)
{
	int i;
	gboolean finish;
	// Alle geladenen Module sollen Initiatisiert werden:

//Hong MPI
/*	
	for (i=self->loop_min; i<self->loop_max; i++)
		{
*/
    for(i=0;i<self->cfg->number_of_xpn; i++)
		{
			if (isXPNforThisRank(i, self->XPNforThisRank))
			{		
//End of Hong MPI	
			if (self->back != 2)
				expertnclass_initialize(self->xpn_class[i]);
		}
		}

	finish = FALSE;

	while (1)
		{
//Hong MPI
/*	
	for (i=self->loop_min; i<self->loop_max; i++)
		{
*/
    for(i=0;i<self->cfg->number_of_xpn; i++)
		{
			if (isXPNforThisRank(i, self->XPNforThisRank))
			{		
//End of Hong MPI	
					self->back=expertnclass_run_mosaic(self->xpn_class[i]);
					if (self->back != RET_SUCCESS)
						{
							finish=TRUE;
							break;
						}
				}
		}
			if (finish==TRUE) break;




#ifdef USE_OPENMP
			#pragma omp parallel for schedule(dynamic) reduction(||: finish)
#endif
//Hong MPI
/*	
	for (i=self->loop_min; i<self->loop_max; i++)
		{
*/
    for(i=0;i<self->cfg->number_of_xpn; i++)
		{
			if (isXPNforThisRank(i, self->XPNforThisRank))
			{		
//End of Hong MPI

					//printf("%d,%d,%d\n",omp_get_thread_num(),omp_get_num_threads(),omp_get_max_threads());
					self->back = expertnclass_run(self->xpn_class[i]);
					if (self->back != RET_SUCCESS)
						{
							finish=TRUE;
						}

				}
		}

			//xpn_output_netcdf_write_data(netf);

			for (i=0; i<self->domain_list_size; i++)
				{
					xpn_output_netcdf_write_data_matrix(self->netf,self->domain_list[i]);
				}

			if(self->back == 2)
				{
				finish = FALSE;
				return RET_SUCCESS;
				}
				
			if (finish==TRUE) break;
		}
	return RET_SUCCESS;
}

void xpn_main_write_vars_to_matrices(xpn_main *self)
{
	//int i,i2;
	
/*	for (i2=0;i2<matrix_size_x;i2++)
		for (i=0;i<matrix_size_y;i++)
			{
				//printf("i: %d, j: %d, value: %f\n",i,i2,self->matrix_to_xpn[i2*matrix_max_y+i].mass);
			}	
	*/
}
//start added 180528 Troost
void xpn_main_mpi_share_results_on_all_nodes (xpn_main *self, int weatherOnly)
{
	int i,i2,k, entry_len;
	int *n_for_procs, *starts_for_procs;
	int *actual_n_for_procs, *actual_starts_for_procs;
	double *sendbuf_double, *recvbuf_double;
	int *sendbuf_int, *recvbuf_int;


	
	int oneGridSize = self->matrix_size_x *self->matrix_size_y;    
	int n_own=  self->loop_max_xn_to_mpmas_grid -  self->loop_min_xn_to_mpmas_grid; 	// calc number of cells managed by this rank

	
	//Send and obtain data amounts send by each processer
	n_for_procs = (int *) g_malloc0(sizeof(int) * self->numtasks);
	actual_n_for_procs = (int *) g_malloc0(sizeof(int) * self->numtasks);
	starts_for_procs = (int *) g_malloc0(sizeof(int) * self->numtasks);
	actual_starts_for_procs = (int *) g_malloc0(sizeof(int) * self->numtasks);
	
	
	n_for_procs[self->rank] = n_own;
	//printf("Data size at: %d  n= %d, %d\n", self->rank, n_own, n_for_procs[self->rank]); 

	
	MPI_Allgather(&n_own, 1, MPI_INT, n_for_procs, 1, MPI_INT, MPI_COMM_WORLD);
	
	for ( i = 1; i <  self->numtasks; ++i)
	{
		starts_for_procs[i] = starts_for_procs[i-1] +  n_for_procs[i-1];
		
	}
	
	//Send actual data
	//Note: a MPI_type_struct would be nicer, but I am doing it the explicit way here
	
	if (weatherOnly == 0) 
	{
		//xn_to_mpmas DOUBLES
		entry_len = 10;
		sendbuf_double = (double *) g_malloc0(sizeof(double) * n_own * entry_len);
		recvbuf_double = (double *) g_malloc0(sizeof(double) * oneGridSize * entry_len);
		
		for ( i = 0; i <  self->numtasks; ++i)
		{
			actual_n_for_procs[i] = n_for_procs[i] * entry_len;
			actual_starts_for_procs[i] = starts_for_procs[i] * entry_len;
		}
		
		
		
		for (i =  self->loop_min_xn_to_mpmas_grid,k=0; i < self->loop_max_xn_to_mpmas_grid; ++i) 
		{
			sendbuf_double[k * entry_len] = self->grid_xn_to_mpmas[i].fruitDryWeight;
			sendbuf_double[k * entry_len +1] = self->grid_xn_to_mpmas[i].stemLeafDryWeight;
			sendbuf_double[k * entry_len +2] = self->grid_xn_to_mpmas[i].stemOnlyDryWeight;
			sendbuf_double[k * entry_len +3] = self->grid_xn_to_mpmas[i].Nmin0_30;
			sendbuf_double[k * entry_len +4] = self->grid_xn_to_mpmas[i].Nmin30_60;
			sendbuf_double[k * entry_len +5] = self->grid_xn_to_mpmas[i].Nmin60_90;
			for (i2 = 0; i2 < XNMPMASMINFERTSLOTS; ++i2) 
			{
				sendbuf_double[k * entry_len + 6 + i2] = self->grid_xn_to_mpmas[i].actualTotalFertN[i2];
			}
			++k;
		}

		//dbg
		/* for (k=0; k < n_own; ++k) {
			printf("FruitDryWeight to be sent: %d [%d] : %f\n", self->rank, k, sendbuf_double[k*entry_len]); 

		}
		for (k=0; k < self->numtasks; ++k) {
			printf("Rankinfo at: %d [%d] : n= %d, start = %d\n", self->rank, k, n_for_procs[k], starts_for_procs[k]); 

		} */

		MPI_Allgatherv(sendbuf_double, n_own * entry_len, MPI_DOUBLE, recvbuf_double, actual_n_for_procs, actual_starts_for_procs, MPI_DOUBLE, MPI_COMM_WORLD);

		for (i = 0; i < oneGridSize; ++i) 
		{
			if (i >=  self->loop_min_xn_to_mpmas_grid && i < self->loop_max_xn_to_mpmas_grid) //for debugging don't overwrite own values
				continue;
			
			self->grid_xn_to_mpmas[i].fruitDryWeight = recvbuf_double[i * entry_len];
			self->grid_xn_to_mpmas[i].stemLeafDryWeight = recvbuf_double[i * entry_len +1];
			self->grid_xn_to_mpmas[i].stemOnlyDryWeight = recvbuf_double[i * entry_len +2];
			self->grid_xn_to_mpmas[i].Nmin0_30 = recvbuf_double[i * entry_len +3];
			self->grid_xn_to_mpmas[i].Nmin30_60 = recvbuf_double[i * entry_len +4];
			self->grid_xn_to_mpmas[i].Nmin60_90 = recvbuf_double[i * entry_len +5];
			for (i2 = 0; i2 < XNMPMASMINFERTSLOTS; ++i2) 
			{
				self->grid_xn_to_mpmas[i].actualTotalFertN[i2] = recvbuf_double[i * entry_len +6 +i2];
			}
			
		}
		//free memory
		g_free(sendbuf_double);
		g_free(recvbuf_double);
		
		//xn_to_mpmas INTS
		entry_len = 4 + 3* XNMPMASMINFERTSLOTS + 6;
		sendbuf_int = (int *) g_malloc0(sizeof(int) * n_own * entry_len);
		recvbuf_int = (int *) g_malloc0(sizeof(int) * oneGridSize * entry_len);
		
		for ( i = 0; i <  self->numtasks; ++i)
		{
			actual_n_for_procs[i] = n_for_procs[i] * entry_len;
			actual_starts_for_procs[i] = starts_for_procs[i] * entry_len;
		}
		
		
		
		for (i =  self->loop_min_xn_to_mpmas_grid,k=0; i < self->loop_max_xn_to_mpmas_grid; ++i) 
		{
			sendbuf_int[k * entry_len] = self->grid_xn_to_mpmas[i].cellID;
			sendbuf_int[k * entry_len +1] = self->grid_xn_to_mpmas[i].actualHarvestDate.year;
			sendbuf_int[k * entry_len +2] = self->grid_xn_to_mpmas[i].actualHarvestDate.month;
			sendbuf_int[k * entry_len +3] = self->grid_xn_to_mpmas[i].actualHarvestDate.day;
			for (i2 = 0; i2 < XNMPMASMINFERTSLOTS; ++i2) 
			{
				sendbuf_int[k * entry_len + 4 + 3 * i2 + 0] = self->grid_xn_to_mpmas[i].actualMinFertDate[i2].year;
				sendbuf_int[k * entry_len + 4 + 3 * i2 + 1] = self->grid_xn_to_mpmas[i].actualMinFertDate[i2].month;
				sendbuf_int[k * entry_len + 4 + 3 * i2 + 2] = self->grid_xn_to_mpmas[i].actualMinFertDate[i2].day;
			}
			
			sendbuf_int[k * entry_len + 4 + 3 * XNMPMASMINFERTSLOTS + 0] = self->grid_xn_to_mpmas[i].actualSowDate.year;
			sendbuf_int[k * entry_len + 4 + 3 * XNMPMASMINFERTSLOTS + 1] = self->grid_xn_to_mpmas[i].actualSowDate.month;
			sendbuf_int[k * entry_len + 4 + 3 * XNMPMASMINFERTSLOTS + 2] = self->grid_xn_to_mpmas[i].actualSowDate.day;
			sendbuf_int[k * entry_len + 4 + 3 * XNMPMASMINFERTSLOTS + 3] = self->grid_xn_to_mpmas[i].actualCoverCropSowDate.year;
			sendbuf_int[k * entry_len + 4 + 3 * XNMPMASMINFERTSLOTS + 4] = self->grid_xn_to_mpmas[i].actualCoverCropSowDate.month;
			sendbuf_int[k * entry_len + 4 + 3 * XNMPMASMINFERTSLOTS + 5] = self->grid_xn_to_mpmas[i].actualCoverCropSowDate.day;		
			
			++k;
		}

		MPI_Allgatherv(sendbuf_int, n_own * entry_len, MPI_INT, recvbuf_int, actual_n_for_procs, actual_starts_for_procs, MPI_INT, MPI_COMM_WORLD);

		for (i = 0; i < oneGridSize; ++i) 
		{
			if (i >=  self->loop_min_xn_to_mpmas_grid && i < self->loop_max_xn_to_mpmas_grid) //for debugging don't overwrite own values
				continue;
			
			self->grid_xn_to_mpmas[i].cellID = recvbuf_int[i * entry_len];
			self->grid_xn_to_mpmas[i].actualHarvestDate.year = recvbuf_int[i * entry_len +1];
			self->grid_xn_to_mpmas[i].actualHarvestDate.month = recvbuf_int[i * entry_len +2];
			self->grid_xn_to_mpmas[i].actualHarvestDate.day = recvbuf_int[i * entry_len +3];

			for (i2 = 0; i2 < XNMPMASMINFERTSLOTS; ++i2) 
			{
				self->grid_xn_to_mpmas[i].actualMinFertDate[i2].year = recvbuf_int[i * entry_len + 4 + 3 * i2 + 0];
				self->grid_xn_to_mpmas[i].actualMinFertDate[i2].month = recvbuf_int[i * entry_len + 4 + 3 * i2 + 1] ;
				self->grid_xn_to_mpmas[i].actualMinFertDate[i2].day = recvbuf_int[i * entry_len + 4 + 3 * i2 + 2];
			}
			self->grid_xn_to_mpmas[i].actualSowDate.year = recvbuf_int[i 	* entry_len + 4 + 3 * XNMPMASMINFERTSLOTS + 0];
			self->grid_xn_to_mpmas[i].actualSowDate.month = recvbuf_int[i 	* entry_len + 4 + 3 * XNMPMASMINFERTSLOTS + 1];
			self->grid_xn_to_mpmas[i].actualSowDate.day = recvbuf_int[i 	* entry_len + 4 + 3 * XNMPMASMINFERTSLOTS + 2];
			self->grid_xn_to_mpmas[i].actualCoverCropSowDate.year = recvbuf_int[i 	* entry_len + 4 + 3 * XNMPMASMINFERTSLOTS + 3];
			self->grid_xn_to_mpmas[i].actualCoverCropSowDate.month = recvbuf_int[i 	* entry_len + 4 + 3 * XNMPMASMINFERTSLOTS + 4];
			self->grid_xn_to_mpmas[i].actualCoverCropSowDate.day = recvbuf_int[i 	* entry_len + 4 + 3 * XNMPMASMINFERTSLOTS + 5];
			
			
		}
		//free memory
		g_free(sendbuf_int);
		g_free(recvbuf_int);
	}
	
	//xn_to_mpmas2 DOUBLES
	entry_len = 2 * XNMPMASDAYSOFYEAR;
	sendbuf_double = (double *) g_malloc0(sizeof(double) * n_own * entry_len);
	recvbuf_double = (double *) g_malloc0(sizeof(double) * oneGridSize * entry_len);
	
	for ( i = 0; i <  self->numtasks; ++i)
	{
		actual_n_for_procs[i] = n_for_procs[i] * entry_len;
		actual_starts_for_procs[i] = starts_for_procs[i] * entry_len;
	}
	
	
	
	for (i =  self->loop_min_xn_to_mpmas_grid,k=0; i < self->loop_max_xn_to_mpmas_grid; ++i) 
	{	for (i2 = 0; i2 < XNMPMASDAYSOFYEAR; ++i2) 
		{
			sendbuf_double[k * entry_len + i2 ] = self->grid_xn_to_mpmas2[i].airTemp[i2];
			sendbuf_double[k * entry_len + XNMPMASDAYSOFYEAR + i2 ] = self->grid_xn_to_mpmas2[i].topsoilTemp[i2];
		}

		++k;
	}



	MPI_Allgatherv(sendbuf_double, n_own * entry_len, MPI_DOUBLE, recvbuf_double, actual_n_for_procs, actual_starts_for_procs, MPI_DOUBLE, MPI_COMM_WORLD);

	for (i = 0; i < oneGridSize; ++i) 
	{
		if (i >=  self->loop_min_xn_to_mpmas_grid && i < self->loop_max_xn_to_mpmas_grid) //for debugging don't overwrite own values
			continue;
			
		for (i2 = 0; i2 < XNMPMASDAYSOFYEAR; ++i2) 
		{
			self->grid_xn_to_mpmas2[i].airTemp[i2] = recvbuf_double[i * entry_len + i2 ];
			self->grid_xn_to_mpmas2[i].topsoilTemp[i2] = recvbuf_double[i * entry_len + XNMPMASDAYSOFYEAR + i2 ];
		}
		
	}
	//free memory
	g_free(sendbuf_double);
	g_free(recvbuf_double);
	
	//xn_to_mpmas2 INTS
	entry_len = 2;
	sendbuf_int = (int *) g_malloc0(sizeof(int) * n_own * entry_len);
	recvbuf_int = (int *) g_malloc0(sizeof(int) * oneGridSize * entry_len);
	
	for ( i = 0; i <  self->numtasks; ++i)
	{
		actual_n_for_procs[i] = n_for_procs[i] * entry_len;
		actual_starts_for_procs[i] = starts_for_procs[i] * entry_len;
	}
	
	
	
	for (i =  self->loop_min_xn_to_mpmas_grid,k=0; i < self->loop_max_xn_to_mpmas_grid; ++i) 
	{
		sendbuf_int[k * entry_len] = self->grid_xn_to_mpmas2[i].startDay;
		sendbuf_int[k * entry_len +1] = self->grid_xn_to_mpmas2[i].stopDay;
		++k;
	}

	MPI_Allgatherv(sendbuf_int, n_own * entry_len, MPI_INT, recvbuf_int, actual_n_for_procs, actual_starts_for_procs, MPI_INT, MPI_COMM_WORLD);

	for (i = 0; i < oneGridSize; ++i) 
	{
		if (i >=  self->loop_min_xn_to_mpmas_grid && i < self->loop_max_xn_to_mpmas_grid) //for debugging don't overwrite own values
			continue;
		
		self->grid_xn_to_mpmas2[i].startDay = recvbuf_int[i * entry_len];
		self->grid_xn_to_mpmas2[i].stopDay  = recvbuf_int[i * entry_len +1];
	}
	//free memory
	g_free(sendbuf_int);
	g_free(recvbuf_int);
	
	
	//free memory generals
	g_free(n_for_procs);
	g_free(starts_for_procs);
	g_free(actual_n_for_procs);
	g_free(actual_starts_for_procs);
	
}
//end added 180528 Troost

xpn_main *xpn_main_done(xpn_main *self)
{
	int i;
	if (self!=NULL)
		{
			if (self->netf!=NULL)
				{
					self->netf = xpn_output_netcdf_Done(self->netf);
				}
			if (self->domain_list!=NULL) g_free(self->domain_list);
			if (self->cfg_file!=NULL) g_free(self->cfg_file);
			if (self->base_dir!=NULL) g_free(self->base_dir);
			if (self->cfg!=NULL)
				{
					for (i=0; i<self->cfg->grid_len; i++)
						{
							if (self->drv[i].xpn!=NULL) free(self->drv[i].xpn);
						}
					general_config_delete(self->cfg);
				}
			if (self->xpn_class!=NULL)
				{
//Hong MPI
/*	
	for (i=self->loop_min; i<self->loop_max; i++)
		{
*/
    for(i=0;i<self->cfg->number_of_xpn; i++)
		{
			if (isXPNforThisRank(i, self->XPNforThisRank))
			{		
//End of Hong MPI
							expertnclass_done(self->xpn_class[i]);
						}
				}
				}
			if (self->drv!=NULL) free(self->drv);
			if (self->xpn_class!=NULL)free(self->xpn_class);
			if (self->modul_list!=NULL) register_modul_list_delete(self->modul_list);		
			g_free(self->grid_mpmas_to_xn);
			g_free(self->grid_xn_to_mpmas);
			g_free(self->grid_xn_to_mpmas2);
			g_object_unref(self);
		}	
	self = NULL;
	return self;
}

//Hong MPI
int *array_INI(int size)
   { 
     int c, i;	
	 int *arr;
     c = size;  
     arr = (int *)g_malloc0(c * sizeof(int *));
  
     for (i = 0; i <  c; i++)
        {
           arr[i]=-1;
        }
     
     return arr;        
    }  
	
int isXPNforThisRank(int val, int *arr){
    int i, number;
	i=0;
    while(arr[i]!=(-1)) 
		{
		number = arr[i];
        if (number == val)
            return 1;
        i++;
	   }

    return 0;
}	

//End of Hong MPI
