#include "xpn_register_modul.h"
#include <stdio.h>
#include <stdlib.h>

G_MODULE_EXPORT struct_register_modul_list *ExpertN_Lib_Register()
{
	return register_modul_list_new(10,  // Anzahl der Modelle
	                               register_modul_new("LEACHN",  // Bezeichnung
	                                       "nitrogen",						                   // Modul
	                                       "nitrogen transport",				           // Sub Modul
	                                       "transpn",					           // Klasse
	                                       "transpn_flow_init",						           // load/init Function
	                                       NULL,						           // global run Function
	                                       NULL,						           // mosaic run Function
	                                       "transpn_flow",	           // run Funktion
	                                       NULL),                                                       // done Function

	                               register_modul_new("LEACHN",  // Bezeichnung
	                                       "water",						                   // Modul
	                                       "flow module",				           // Sub Modul
	                                       "water_flow_hutson",					           // Klasse
	                                       "water_flow_hutson_load",						           // load/init Function
	                                       NULL,						           // global run Function
	                                       NULL,						           // mosaic run Function
	                                       "water_flow_hutson_run",	           // run Funktion
	                                       "water_flow_hutson_done") ,                                                      // done Function
	                               register_modul_new("LEACHN",  // Bezeichnung
	                                       "plant",						                   // Modul
	                                       "actual transpiration",				           // Sub Modul
	                                       "leachn_plant",					           // Klasse
	                                       "leachn_plant_load",						           // load/init Function
	                                       NULL,						           // global run Function
	                                       NULL,						           // mosaic run Function
	                                       "leachn_plant_act_transpiration",	           // run Funktion
	                                       NULL),
	                               register_modul_new("LEACHN",  // Bezeichnung
	                                       "plant",						                   // Modul
	                                       "biomass growth",				           // Sub Modul
	                                       "leachn_plant",					           // Klasse
	                                       "leachn_plant_load",						           // load/init Function
	                                       NULL,						           // global run Function
	                                       NULL,						           // mosaic run Function
	                                       "leachn_plant_act_biomass_growth",	           // run Funktion
	                                       NULL),
	                               register_modul_new("LEACHN",  // Bezeichnung
	                                       "plant",						                   // Modul
	                                       "root length growth",				           // Sub Modul
	                                       "leachn_plant",					           // Klasse
	                                       "leachn_plant_load",						           // load/init Function
	                                       NULL,						           // global run Function
	                                       NULL,						           // mosaic run Function
	                                       "leachn_plant_RootMaizeLeach",	           // run Funktion
	                                       NULL),	                              
	                               register_modul_new("LEACHN",  // Bezeichnung
	                                       "plant",						                   // Modul
	                                       "nitrogen uptake",				           // Sub Modul
	                                       "leachn_plant",					           // Klasse
	                                       "leachn_plant_load",						           // load/init Function
	                                       NULL,						           // global run Function
	                                       NULL,						           // mosaic run Function
	                                       "leachn_plant_NUptake",	           // run Funktion
	                                       NULL),
	                               register_modul_new("LEACHN",	   // Bezeichnung
	                                       "nitrogen",						       // Modul
	                                       "nitrification",				           // Sub Modul
	                                       "stickstoff",						   // Klasse
	                                       "stickstoff_load",					   // load/init Function
	                                       NULL,						           // global run Function
	                                       NULL,						           // mosaic run Function
	                                       "stickstoff_run",		               // run Funktion
	                                       "stickstoff_done"),					   // done Function
	                               register_modul_new("LEACHN",     // Bezeichnung
	                                       "nitrogen",						        // Modul
	                                       "denitrification",	           			// Sub Modul
	                                       "stickstoff",					   		// Klasse
	                                       "leachn_denit_load",		            // load/init Function
	                                       NULL,						            // global run Function
	                                       NULL,						            // mosaic run Function
	                                       "leachn_denit_run_call",	            // run Funktion
	                                       "leachn_denit_done"),                // done Function


	                               register_modul_new("LEACHN",     // Bezeichnung
	                                       "nitrogen",						        // Modul
	                                       "mineralisation",	           			// Sub Modul
	                                       "stickstoff",					   		// Klasse
	                                       "leachn_miner_load",		            // load/init Function
	                                       NULL,						            // global run Function
	                                       NULL,						            // mosaic run Function
	                                       "leachn_miner_run",	         		   // run Funktion
	                                       "leachn_miner_done"),              	  // done Function


	                               register_modul_new("LEACHN",     // Bezeichnung
	                                       "nitrogen",						        // Modul
	                                       "urea hydrolysis",	           			    // Sub Modul
	                                       "stickstoff",					   		// Klasse
	                                       "leachn_hydro_load",		            // load/init Function
	                                       NULL,						            // global run Function
	                                       NULL,						            // mosaic run Function
	                                       "leachn_hydro_run",	         		   // run Funktion
	                                       "leachn_hydro_done")              	  // done Function
	                              );

}
