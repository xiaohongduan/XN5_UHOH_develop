#include "xpn_register_modul.h"
#include <stdio.h>
#include <stdlib.h>

G_MODULE_EXPORT struct_register_modul_list *ExpertN_Lib_Register()
{
	return register_modul_list_new(10,  // Anzahl der Modelle
	                                 register_modul_new("GRAPEVINE",  // Bezeichnung
	                                       "plant",						                   // Modul
	                                       "potential transpiration",				           // Sub Modul
	                                       "grapevine",					           // Klasse
	                                       "grapevine_load",						           // load/init Function
	                                       NULL,						           // global run Function
	                                       NULL,						           // mosaic run Function
	                                       "grapevine_potential_transpiration",	           // run Funktion
	                                       "grapevine_done"),                           // done Function								   
								    register_modul_new("GRAPEVINE",  // Bezeichnung
	                                       "plant",						                   // Modul
	                                       "actual transpiration",				           // Sub Modul
	                                       "grapevine",					           // Klasse
	                                       "grapevine_load",						           // load/init Function
	                                       NULL,						           // global run Function
	                                       NULL,						           // mosaic run Function
	                                       "grapevine_actual_transpiration",	           // run Funktion
	                                       "grapevine_done"),                           // done Function
									register_modul_new("GRAPEVINE",  // Bezeichnung
	                                       "plant",						                   // Modul
	                                       "phenological development",				           // Sub Modul
	                                       "grapevine",					           // Klasse
	                                       "grapevine_load",						           // load/init Function
	                                       NULL,						           // global run Function
	                                       NULL,						           // mosaic run Function
	                                       "grapevine_phenological_development",	           // run Funktion
	                                       "grapevine_done"),                           // done Function
									register_modul_new("GRAPEVINE",  // Bezeichnung
	                                       "plant",						                   // Modul
	                                       "canopy gross photosynthesis",				           // Sub Modul
	                                       "grapevine",					           // Klasse
	                                       "grapevine_load",						           // load/init Function
	                                       NULL,						           // global run Function
	                                       NULL,						           // mosaic run Function
	                                       "grapevine_photosynthesis",	           // run Funktion
	                                       "grapevine_done"),                           // done Function
									register_modul_new("GRAPEVINE",  // Bezeichnung
	                                       "plant",						                   // Modul
	                                       "biomass growth",				           // Sub Modul
	                                       "grapevine",					           // Klasse
	                                       "grapevine_load",						           // load/init Function
	                                       NULL,						           // global run Function
	                                       NULL,						           // mosaic run Function
	                                       "grapevine_biomass_growth",	           // run Funktion
	                                       "grapevine_done"),                           // done Function
									register_modul_new("GRAPEVINE",  // Bezeichnung
	                                       "plant",						                   // Modul
	                                       "canopy formation",				           // Sub Modul
	                                       "grapevine",					           // Klasse
	                                       "grapevine_load",						           // load/init Function
	                                       NULL,						           // global run Function
	                                       NULL,						           // mosaic run Function
	                                       "grapevine_canopy_formation",	           // run Funktion
	                                       "grapevine_done"),                           // done Function
									register_modul_new("GRAPEVINE",  // Bezeichnung
	                                       "plant",						                   // Modul
	                                       "root length growth",				           // Sub Modul
	                                       "grapevine",					           // Klasse
	                                       "grapevine_load",						           // load/init Function
	                                       NULL,						           // global run Function
	                                       NULL,						           // mosaic run Function
	                                       "grapevine_root_length_growth",	           // run Funktion
	                                       "grapevine_done"),                           // done Function
									register_modul_new("GRAPEVINE",  // Bezeichnung
	                                       "plant",						                   // Modul
	                                       "nitrogen demand",				           // Sub Modul
	                                       "grapevine",					           // Klasse
	                                       "grapevine_load",						           // load/init Function
	                                       NULL,						           // global run Function
	                                       NULL,						           // mosaic run Function
	                                       "grapevine_nitrogen_demand",	           // run Funktion
	                                       "grapevine_done"),                           // done Function
									register_modul_new("GRAPEVINE",  // Bezeichnung
	                                       "plant",						                   // Modul
	                                       "nitrogen uptake",				           // Sub Modul
	                                       "grapevine",					           // Klasse
	                                       "grapevine_load",						           // load/init Function
	                                       NULL,						           // global run Function
	                                       NULL,						           // mosaic run Function
	                                       "grapevine_nitrogen_uptake",	           // run Funktion
	                                       "grapevine_done"),                           // done Function
									register_modul_new("GRAPEVINE",  // Bezeichnung
	                                       "plant",						                   // Modul
	                                       "crop senescence",				           // Sub Modul
	                                       "grapevine",					           // Klasse
	                                       "grapevine_load",						           // load/init Function
	                                       NULL,						           // global run Function
	                                       NULL,						           // mosaic run Function
	                                       "grapevine_crop_senescence",	           // run Funktion
	                                       "grapevine_done")                           // done Function
	                              );

}
