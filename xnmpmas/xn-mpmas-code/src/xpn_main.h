/*
 * xpn_main.h
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


#ifndef XPN_MAIN_H_
#define XPN_MAIN_H_

#include <glib-object.h>
#ifdef WIN32
#include <windows.h>
#endif

#ifdef USE_OPENMP
#include <omp.h>
#endif

#ifdef USE_MPI
#include <mpi.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <expertnclass.h>
#include <xpn_output_netcdf.h>
#include <general_config.h>

#include "xn_mpmas_interface.h"

G_BEGIN_DECLS

#if USE_OPENMP && USE_MPI
#error "MPI and OPENMP can not be used at the same time!"
#endif

#define EPSILON 1.0e-34
#define WORKAROUND_GRIDID 1

typedef struct {
  expertnclass** xpn; // Expert-N Instance
  int xpn_N;
  int xpn_finished; // 1 or 0
} struct_drive_xpn;


#define TYPE_XPN_MAIN             (xpn_main_get_type ())
#define XPN_MAIN(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_XPN_MAIN, xpn_main))
#define XPN_MAIN_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_XPN_MAIN, xpn_mainClass))
#define IS_XPN_MAIN(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_XPN_MAIN))
#define IS_XPN_MAIN_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_XPN_MAIN))
#define XPN_MAIN_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_XPN_MAIN, xpn_mainClass))

typedef struct xpn_main_         xpn_main;
typedef struct xpn_mainClass_    xpn_mainClass;
typedef struct xpn_mainPrivate_  xpn_mainPrivate;

struct xpn_main_
{
  GObject parent;
  /* add your public declarations here */
  xpn_output_netcdf *netf;
  
  // Par:
  int MPI_act; // 0..1: Es wird mit MPI parallelisiert
  int OPENMP_act; // 0..1: Es wird mit OpenMP parallelisiert
  int numtasks; // MPI Const
  int rank; // MPI Const
  int loop_min;
  int loop_max;
  int back;
  
  int matrix_size_y;
  int matrix_size_x;
  int grid_layers;
  
  int *domain_list;
  int domain_list_size;
  
  
  // Cfg Files:
  gchar* cfg_file;
  gchar* base_dir;
  
  general_config *cfg;
  expertnclass **xpn_class;
  struct_drive_xpn *drv;
  struct_register_modul_list *modul_list;
  
  struct_variable ***save_registry;
  
  // Data Exchange:
  STRUCT_mpmas_to_xn* grid_mpmas_to_xn;
  STRUCT_xn_to_mpmas* grid_xn_to_mpmas;
  STRUCT_xn_to_mpmas2* grid_xn_to_mpmas2;
 
//Hong MPI
  // 2-D Array for IDs (ID-Table): rows refer to soil profiles; columns refer to crop models/ gridLayers  
  int gridSize;
  
  int *XPN_ID_tab;
  int *sorted_XPN;
  
  int *XPNforThisRank;
  
  int loop_min_grid, loop_max_grid; //Note (Troost): these are temporarily used in initialization ? and change relative to the grid currently being treated?
  
  int loop_min_xn_to_mpmas_grid; //These are the bounds of the actual coordinates in the single grid (not considering the grid layer/grid_id)
  int loop_max_xn_to_mpmas_grid;
  
  
  //End of Hong MPI
};

struct xpn_mainClass_
{
  GObjectClass parent_class;
};


GType xpn_main_get_type (void);


// Init the xpn_main class
// Std: rank=0, numtasks=1 (e.g. if there is no MPI)
// return pointer to structure
xpn_main *xpn_main_new (int rank,int numtasks);

// Parses Parameter and setup config_file
// if config_file is not set return 1 else return 0
// setup cfg_file and base_dir
int       xpn_main_parse_command_line(xpn_main *self, int argc, char **argv);

int     xpn_main_load_cfg_and_reserve_memory(xpn_main *self);

int     xpn_main_save_current_state_of_registry(xpn_main *self);

int     xpn_main_run(xpn_main *self);

void    xpn_main_write_vars_to_matrices(xpn_main *self);

void 	xpn_main_mpi_share_results_on_all_nodes (xpn_main *self, int weatherOnly);


xpn_main *xpn_main_done(xpn_main *self);

//Hong MPI
int *array_INI(int size);
int isXPNforThisRank(int val, int *arr);
//End of Hong MPI

G_END_DECLS

#endif /* XPN_MAIN_H_ */
