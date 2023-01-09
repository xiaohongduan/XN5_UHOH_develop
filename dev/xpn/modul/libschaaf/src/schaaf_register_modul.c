#include "xpn_register_modul.h"
#include <stdio.h>
#include <stdlib.h>

G_MODULE_EXPORT struct_register_modul_list *ExpertN_Lib_Register()
{
	return register_modul_list_new(3,  // Anzahl der Modelle
									register_modul_new("Schaaf",  							   // Bezeichnung
													   "management",			               // Modul
													   "application fertilizers",	           // Sub Modul
													    "schaaf",					           // Klasse
														"read_fertilizers",	         		   // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"schaaf_fertilizer",		           // run Funktion
														NULL),                                  // done Function
									register_modul_new("Fertilize_at_BBCH",  							   // Bezeichnung
													   "management",			               // Modul
													   "application fertilizers",	           // Sub Modul
													    "schaaf",					           // Klasse
														"read_fertilizers_BBCH",	         		   // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"schaaf_fertilizer_bbch",		           // run Funktion
														NULL),                                  // done Function
									register_modul_new("Schaaf",  							   // Bezeichnung
													   "management",			               // Modul
													   "irrigation",	           // Sub Modul
													    "schaaf",					           // Klasse
														"read_irrigation",	         		   // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"schaaf_irrigation",		           // run Funktion
														NULL)                                  // done Function
														
									
								   );
								   
}

