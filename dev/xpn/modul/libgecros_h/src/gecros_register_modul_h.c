#include "xpn_register_modul.h"
#include <stdio.h>
#include <stdlib.h>

G_MODULE_EXPORT struct_register_modul_list *ExpertN_Lib_Register()
{
	return register_modul_list_new(9,  // Anzahl der Modelle
									register_modul_new("GECROS_h Development",  // Bezeichnung
													   "plant",						           // Modul
													   "phenological development",			   // Sub Modul
													    "gecros_h",					           // Klasse
														"gecros_h_Init_and_AllocMemory",         // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"gecros_h_Development",	               // run Funktion
														"gecros_h_Done_and_FreeMemory"),         // done Function//
														
									register_modul_new("GECROS_h BiomassGrowth",  // Bezeichnung
													   "plant",						           // Modul
													   "biomass growth",				       // Sub Modul
													    "gecros_h",					           // Klasse
														"gecros_h_Init_and_AllocMemory",         // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"gecros_h_BiomassGrowth",	               // run Funktion
														"gecros_h_Done_and_FreeMemory"),                                 // done Function
														
									register_modul_new("GECROS_h Gross Photosynthesis",  // Bezeichnung
													   "plant",						           // Modul
													   "canopy gross photosynthesis",	       // Sub Modul
													    "gecros_h",					           // Klasse
														"gecros_h_Init_and_AllocMemory",         // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"gecros_h_Photosynthesis",	           // run Funktion
														"gecros_h_Done_and_FreeMemory"),                                 // done Function															
									
								    register_modul_new("GECROS_h Canopy Formation",  // Bezeichnung
													   "plant",						           // Modul
													   "canopy formation",			   // Sub Modul
													    "gecros_h",					           // Klasse
														"gecros_h_Init_and_AllocMemory",         // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"gecros_h_CanopyFormation",	               // run Funktion
														"gecros_h_Done_and_FreeMemory"),                                 // done Function

									register_modul_new("GECROS_h Root System Formation",  // Bezeichnung
													   "plant",						           // Modul
													   "root length growth",				           // Sub Modul
													    "gecros_h",					           // Klasse
														"gecros_h_Init_and_AllocMemory",         // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"gecros_h_RootSytem",	                   // run Funktion
														"gecros_h_Done_and_FreeMemory"),                                 // done Function
														
									register_modul_new("GECROS_h Crop Senescence",  // Bezeichnung
													   "plant",						           // Modul
													   "crop senescence",	       // Sub Modul
													    "gecros_h",					           // Klasse
														"gecros_h_Init_and_AllocMemory",         // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"gecros_h_CropSenescence",	           // run Funktion
														"gecros_h_Done_and_FreeMemory"),                                 // done Function

								    register_modul_new("GECROS_h Nitrogen Demand",  // Bezeichnung
													   "plant",						           // Modul
													   "nitrogen demand",			   // Sub Modul
													    "gecros_h",					           // Klasse
														"gecros_h_Init_and_AllocMemory",         // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"gecros_h_NitrogenDemand",	           // run Funktion
														"gecros_h_Done_and_FreeMemory"),                                 // done Function

									register_modul_new("GECROS_h Nitrogen Uptake",  // Bezeichnung
													   "plant",						           // Modul
													   "nitrogen uptake",				       // Sub Modul
													    "gecros_h",					           // Klasse
														"gecros_h_Init_and_AllocMemory",         // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"gecros_h_NitrogenUptake",	           // run Funktion
														"gecros_h_Done_and_FreeMemory"),                                 // done Function
														
									register_modul_new("GECROS_h Actual Transpiration",  // Bezeichnung
													   "plant",						           // Modul
													   "actual transpiration",	       // Sub Modul
													    "gecros_h",					           // Klasse
														"gecros_h_Init_and_AllocMemory",         // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"gecros_h_ActualTranspiration",	       // run Funktion
														"gecros_h_Done_and_FreeMemory")                                 // done Function															
								   );
								   
}

