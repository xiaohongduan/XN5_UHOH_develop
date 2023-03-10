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
	// Variablen f??r command line parser:
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
	g_option_context_add_main_entries (context, entries, NULL); // f??r unterschiedliche Sprachen: GETTEXT_PACKAGE

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
	// End of Hong

	// Speicher f??r CFG:
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

	// Es werden alle Namen der Modulle und Klassen die m??glich sind geladen (werden aus der dll entnommen)
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
		
		general_config_get_grid_size_ij(self->cfg,&x1,&y1,&x2,&y2, id); //Achtung, nicht Gridgr????e, sondern maximale AKTIVE Koordinate
		
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
    //Speicher f??r ...xpn_other_grids: 
     oneGridsize = self->matrix_size_x *self->matrix_size_y;    
     for (gridId=0; gridId<self->grid_layers;gridId++)
     {
        for (cellNumber=0; cellNumber< oneGridsize;cellNumber++)
        {
            self->grid_mpmas_to_xn[gridId *oneGridsize+cellNumber].xpn_other_grids = (expertn_modul_base **)g_malloc0(sizeof(expertn_modul_base *)*self->grid_layers);
           
        }
        
     }
/*
   // initialize ID-Table
    self->IDSorting = IDSortINI(oneGridsize, self->grid_layers);// not necessary
*/
// End of Hong  

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

	//Speicher f??r Expert - N Instanzen reservieren:
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


	i3 = 0;
	printf("grid_len: %d\n", self->cfg->grid_len);
	for (i=0; i<self->cfg->grid_len; i++)
		{
			
			printf("grid[%d].instance_len: %d\n", i, self->cfg->grid[i].instance_len);
			self->drv[i].xpn_N = self->cfg->grid[i].instance_len;
			self->drv[i].xpn_finished = FALSE;
			self->drv[i].xpn = (expertnclass**) calloc(self->drv[i].xpn_N,sizeof(expertnclass*));
			for (i2=0; i2<self->cfg->grid[i].instance_len; i2++)
				{

#ifdef USE_MPI
					self->loop_min    =(int)((long)(self->rank + 0) *
					                         (long)(count)/(long)self->numtasks);
					self->loop_max    =(int)((long)(self->rank + 1) *
					                         (long)(count)/(long)self->numtasks);

					if ((i3 >= self->loop_min) && (i3 < self->loop_max))
						{
#endif

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


	// Um auf andere Instanzen zugreifen zu k??nnen, werden jetzt die entsprechenden Verkn??pfungen gemacht:

#ifdef USE_MPI
	self->loop_min    =(int)((long)(self->rank + 0) *
	                         (long)(count)/(long)self->numtasks);
	self->loop_max    =(int)((long)(self->rank + 1) *
	                         (long)(count)/(long)self->numtasks);
#endif

	i3 = 0;
	for (i=0; i<self->cfg->grid_len; i++)
		{
			double fraction;

			i5 = i3;
			fraction = 0.0;
			for (i2=0; i2<self->cfg->grid[i].instance_len; i2++)
				{
					PXSYSTEM pXSys;

#ifdef USE_MPI
					if ((i3 >= self->loop_min) && (i3 < self->loop_max))
						{
#endif
							pXSys = self->xpn_class[i3]->pXSys;

							pXSys->MPI_act = self->MPI_act;
							pXSys->OPENMP_act = self->OPENMP_act;

							pXSys->reg_str_inst = (char**)calloc(self->cfg->number_of_xpn,sizeof(char*));
							pXSys->xpn_classes = (void**)calloc(self->cfg->number_of_xpn,sizeof(void*));

//Begin of Hong  
// link to other instances referring to the same soil column
                            cellID= self->cfg->grid[i].id* oneGridsize+self->cfg->grid[i].j*self->matrix_size_y+self->cfg->grid[i].i;
                            self->grid_mpmas_to_xn[cellID].xpn_own_grid=self->xpn_class[i3]->XPN_Moduls[0]->xpn;
                                                
                            for (gridId2=0; gridId2<self->grid_layers; gridId2++)
                               {
                                 cellID2= gridId2 * oneGridsize+self->cfg->grid[i].j*self->matrix_size_y+self->cfg->grid[i].i;   
                            
                                 self->grid_mpmas_to_xn[cellID2].xpn_other_grids[self->cfg->grid[i].id] = self->xpn_class[i3]->XPN_Moduls[0]->xpn;
                               }
/*                        
// write in ID-Table
                            self->IDSorting[self->cfg->grid[i].j*self->matrix_size_y+self->cfg->grid[i].i][self->cfg->grid[i].id]= pXSys->id;  //not necessary                       
*/                                                                            
// End of Hong 

							// Das hier muss f??r MPI noch mal ??berarbeitet werden:
							//for (i4=0; i4 < cfg->number_of_xpn; i4++) {
							for (i4=self->loop_min; i4 < self->loop_max; i4++)
								{
									pXSys->reg_str_inst[i4] = create_string_from_source(self->xpn_class[i4]->pXSys->reg_str);
									pXSys->xpn_classes[i4] = (void*)self->xpn_class[i4];
								}

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
							xpn_register_var_add_none(reg,"p_grid_mpmas_to_xn",&(self->grid_mpmas_to_xn[self->cfg->grid[i].id * simpleGridSize +self->cfg->grid[i].j*self->matrix_size_y+self->cfg->grid[i].i]),-1,TRUE);
							xpn_register_var_add_none(reg,"p_grid_xn_to_mpmas",&(self->grid_xn_to_mpmas[self->cfg->grid[i].id * simpleGridSize +self->cfg->grid[i].j*self->matrix_size_y+self->cfg->grid[i].i]),-1,TRUE);							
							xpn_register_var_add_none(reg,"p_grid_xn_to_mpmas2",&(self->grid_xn_to_mpmas2[self->cfg->grid[i].id * simpleGridSize +self->cfg->grid[i].j*self->matrix_size_y+self->cfg->grid[i].i]),-1,TRUE);	
							}

#ifdef USE_MPI
						}
#endif
					i3++;
				}
		}
	self->netf = xpn_output_netcdf_new(self->xpn_class,self->cfg->number_of_xpn,self->cfg->config_values,self->cfg->config_values_len,self->cfg);
	//g_atexit(exit_NETCDF);

	// ... hier k??nnen jetzt zus??tzliche registry Eintr??ge gemacht werden:
	// ... --> xpn_register_var *expertnclass_get_varlist(expertnclass *self);



	// f??r MPI werden loop_min und loop_max auf die Prozesoren aufgeteilt:

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
	for (i=self->loop_min; i<self->loop_max; i++)
		{
			if (self->back != 2)
				expertnclass_initialize(self->xpn_class[i]);
		}


	finish = FALSE;

	while (1)
		{
			for (i=self->loop_min; i<self->loop_max; i++)
				{
					self->back=expertnclass_run_mosaic(self->xpn_class[i]);
					if (self->back != RET_SUCCESS)
						{
							finish=TRUE;
							break;
						}
				}
			if (finish==TRUE) break;




#ifdef USE_OPENMP
			#pragma omp parallel for schedule(dynamic) reduction(||: finish)
#endif
			for (i=self->loop_min; i<self->loop_max; i++)
				{

					//printf("%d,%d,%d\n",omp_get_thread_num(),omp_get_num_threads(),omp_get_max_threads());
					self->back = expertnclass_run(self->xpn_class[i]);
					if (self->back != RET_SUCCESS)
						{
							finish=TRUE;
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
	int i,i2;
	
/*	for (i2=0;i2<matrix_size_y;i2++)
		for (i=0;i<matrix_size_x;i++)
			{
				//printf("i: %d, j: %d, value: %f\n",i,i2,self->matrix_to_xpn[i2*matrix_max_y+i].mass);
			}	
	*/
}

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
					for (i=self->loop_min; i<self->loop_max; i++)
						{
							expertnclass_done(self->xpn_class[i]);
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

/* //not necessary
// Begin of Hong
//to initialize ID-Table IDSorting
int **IDSortINI(int row, int col)
   { 
     int r, c, i;
    // int j;
     int size;
     r= row;
     c = col;  
     int **arr; 
     arr = (int **)g_malloc0(r * sizeof(int *));
     for (i=0; i<r; i++)
         arr[i] = (int *)g_malloc0(c * sizeof(int));
 
   
    for (i = 0; i <  r; i++)
        {
      //for (j = 0; j < c; j++)
      //   arr[i][j] = -1;  // assign ini values to -1 instead of 0 
            size =  sizeof(arr[i]);
            memset(arr[i], -1, size);
        }
     
     return arr;
        
    }       

// End of Hong
*/