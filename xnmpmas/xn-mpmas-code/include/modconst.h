/*   $Revision: 18 $ 
     $Date: 15.10.06 16:15 $  */

#ifndef MODCONSTH
#define MODCONSTH

// TEST FLAGS (Arnold / ARNOLD )
#define _testRouting_ 0
#define _testSaveRouting_  0
#define _requestReturns_  0
#define _testRoutingOutput_  0
#define _testIrrig_ 0
#define _testIrrig_2_ 0
#define _testIrrig_3_ 0
//#define _testIrrig_4_ 0
#define _test_abfluss_ 0
//#define _test_abfluss_storage_ 0
#define _test_routing_control_ 1
#define _test_writeGis_ 0
#define _test_Riverflow_ 0
#define _checkReturn_ 0

#define _testReadStorage_ 0
//#define _testUnsatzCont_ 0

#define _referenceSurface_Code_             "MAS"
#define _referenceSurface_lengthOfCode_             	   3
#define _MPMAS_lengthActivityID_           	   3
#define _test_ETPmod_ 						   1
#define _test_ETPmod_detail_ 				   1



#undef  _ENGLISH_
#undef  _16_bit_
#define _ENGLISH_
//#define _16_bit_

#if defined _16_bit_
#define buffersize      (long)65535
#else
#define buffersize      (long)262143 /* maximum size of the internal output buffer */
#endif

#define _stringsize_      1200        /* alle Dateinamen und andere reservierte Strings haben diesen Speicherplatz zur Verfuegung */
#define _meteosatzlaenge_ 8000        /* maximale Laenge des Puffers, in den ein Satz aus meteorologischen Dateien im ganzen eingelesen werden kann. Sind die Saetze laenger, wird nachgelesen */
#define _Dicke_Schicht_1_old_ 0.05         /* Dicke der ersten Bodenschicht in m */
#define gridmaxanzahl     500         /* maximale Anzahl von im SPeicher zu haltenden Grids */
#define maxzonenzahl      16000        /* 8000 Zonen koennen im Zonengrid codiert sein */
#define maxzonenwert      (long)32767  /* maximale Groesse der Codes fuer die Zonen */
#define _max_regionen_zahl_ 30        /* anzahl an Regionen, die fuer die regression-interpolation genutzt werden koennen */

#define true              1
#define false             0
#define maxsymbolanzahl   1000         /* fuer Steuerdatei */
#ifdef ASGI
# define dirzeichen       '/'         /* ASGI wird auf der UNIX Platform gefahren */
#else
# define dirzeichen       '\\'        /* DOS: \\ Unix: / VAX: . */
#endif
#define maxteilgebiete    8000        /* fuer Abflussbildungsmodell -> Bodenmodellklasse
                                       pro Teilgebiet werden im Bodenmodell 168 Byte belegt (zonale Statistik u.a.)
                                       bei 500 Teilgebieten also ca. 82 Kbyte */
#define Pi              (double)3.14159265359

#define _maxResults_            100  /* maximum number of entries in the lastresult-array */

#define NO_VALID_INSTANCE_INDEX 102
#define UNKNOWN_EXCEPTION       103

#define OTHERDATACOUNT 16 /* how many data columns will be writen to the control points "otherdata" file */

/* nun einige Konstanten, die fuer eine rationelle Speicherreservierung noetig sind,
   besonders, wenn mit GNU-Compilern gearbeitet wird, die keine eigene Speicherverwaltung ala
   Borland haben. Damit bei einer Matrix-reservierung nicht immer maximal eine halbe 2er-Potenz
   Speicher verschenkt wird, wird der Speicher in optimalen Bloecken angefordert, die moeglichst
   wenig Rest lassen, aber auch, moeglichst wenig Verwaltungsaufwand erfordern sollen, weshalb
   ein maximaler overhead angegeben werden kann, der die Blance zwischen Speicherverschwendung und
   minimalem Verwaltungsaufwand angibt. Der Wert sollte <= 0.05 sein.
   Je nach Betriebssystem kann der maximal linear zu adressierende Speicher in _maxlinmem_ angegeben werden.
   16-Bit hat 2^16-24 = 65520, 32-bit hat 2^32 Byte direkt zu adressieren.  */

#if defined _16_bit_
#define _maxlinmem_      (unsigned long)65520
#else
#define _maxlinmem_      (unsigned long)2147480000
#endif

#define _1_ln2_          (double)1.442695041111   /* 1/ln2 */
#define _maxmemoverhead_ (double)0.05    /* max. ca. 5% Speicherverschwendung sollen im Interesse kleinen Verwaltungsaufwandes moeglich sein */

/* hier werden die im Modell genutzten und z.T. auch in der Steuerdatei im Klartext angegebenen
   Bezeichnungen einzelner Groessen, Teilmodelle und anderer Bezeichnungen als Makros definiert,
   so kann spaeter das Umschreiben der Meldungen und der Steuerdatei in eine andere Sprache
   schneller erfolgen. Ausserdem hat das den Vorteil, dass sich Aenderungen in der Benennung eines
   Abschnittes schnell im Modell realisieren lassen, da nicht mehr jedes einzelne AUftreten
   des Textes gesucht werden muss, sondern die Aenderung in dieser Datei ueberall wirksam ist. */

#if !defined(_ENGLISH_)

/* Abschnitte und Bezeichnungen aus der Steuerdatei */
#define _Programmausgabe_             "[Programmausgabe]"
#define _output_list_                 "[Ausgabeliste]"
#define _Koordinaten_                 "[Koordinaten]"
#define _region_transition_distance_  "[region_transition_distance]"
#define _Hoehenmodell_                "[Hoehenmodell]"
#define _Zonengrid_                   "[Zonengrid]"
#define _Standardgrids_               "[Standardgrids]"
#define _variable_Grids_              "[variable_Grids]"
#define _Modellzeit_                  "[Modellzeit]"
#define _anzahl_                      "[Anzahl]"
#define _meteonamen_                  "[Meteonamen]"
#define _Niederschlagskorrektur_      "Niederschlagskorrektur"
#define _Expositionskorrektur_        "Expositionskorrektur"
#define _RelCellArea_                 "RelCellArea"
#define _CellSizeX_                   "CellSizeX"
#define _CellSizeY_                   "CellSizeY"
#define _CoordY_                      "CoordY"
#define _CoordX_                      "CoordX"
#define _Bodenarten_                  "Bodenarten"
#define _Landnutzung_                 "Landnutzung"
#define _ice_firn_                    "ice_firn"
#define _Nutzbare_Feldkapazitaet_     "Nutzbare_Feldkapazitaet"
#define _Gefaelle_                    "Gefaelle"
#define _Exposition_                  "Exposition"
#define _Topographischer_Faktor_      "Topographischer_Faktor"
#define _Zonengrid_Bodenmodell_       "Zonengrid_Bodenmodell"
#define _Hydraulische_Leitfaehigkeit_ "Hydraulische_Leitfaehigkeit"
#define _SkyViewFactor_               "SkyViewFactor"
#define _Fliesszeiten_                "Fliesszeiten"
#define _Albedo_                      "Albedo"
#define _Bodenspeicher_               "Bodenspeicher"
#define _max_ponding_storage_         "max_ponding_storage"
#define _clay_depth_                  "clay_depth"
#define _irrigation_codes_            "Bewaesserungscodes"
#define _river_depth_                 "Gewaessertiefe"
#define _river_width_                 "Gewaesserbreite"
#define _river_links_                 "Routingstrecken"
#define _kolmation_                   "Kolmation"

/* Speichergrids fuer Zustaende/Fluesse */

#define _Auffuellbare_Porositaet_     "Auffuellbare_Porositaet"
#define _Saettigungsdefizit_          "Saettigungsdefizit"
#define _Bestandesniederschlag_       "Bestandesniederschlag"
#define _Schneeausfluss_              "Schneeausfluss"
#define _Snow_out_                    "Schneeschmelze"
#define _Schneetage_                  "Schneetage"
#define _Schneealter_                 "Schneealter"
#define _Schneeanteil_                "Schneeanteil"
#define _Regenanteil_                 "Regenanteil"
#define _melt_from_firn_              "melt_from_firn"
#define _melt_from_ice_               "melt_from_ice"

/* #define _Wasseraequivalent_           "Wasseraequivalent" */

#define _Unsaturated_Zone_Storage_    "Unsaturated_Zone_Storage"
#define _Interflow_Storage_           "Interflow_Storage"
#define _Saturated_Soil_Partition_    "Saturated_Soil_Partition"
#define __sbalt_grid__                "$$sbalt_grid$$"
#define _Bodenspeicher_Alt_           "Bodenspeicher_Alt"
#define _Basisabfluss_                "Basisabfluss"
#define _Direktabfluss_               "Direktabfluss"
#define _Interflow_                   "Interflow"
#define _Peakflow_                    "Peakflow"
#define _Nacht_Temperatur_            "Nacht_Temperatur"
#define _Korrigierte_Temperatur_      "Korrigierte_Temperatur"
#define _Potentielle_Verdunstung_     "Potentielle_Verdunstung"
#define _Reale_Verdunstung_           "Reale_Verdunstung"
#define _Nadelwald_                   "Nadelwald"
#define _Schatten_                    "Schatten"
#define __Nacht_Temperatur__          "$$Nacht_Temperatur$$"
#define _Infiltrationsexzess_         "Infiltrationsexzess"
#define _Infiltrationssaettigung_     "Infiltrationssaettigung"
#define _ungueltiger_Name_            "ungueltiger_Name"
#define _Interzeptionsspeicher_       "Interzeptionsspeicher"
#define _Interzeptionsverdunstung_    "Interzeptionsverdunstung"
#define _Schneespeicher_Fest_         "Schneespeicher_Fest"
#define _Schneespeicher_Fluessig_     "Schneespeicher_Fluessig"
#define _Wurzeltiefe_                 "Wurzeltiefe"
#define _Vegetationsbedeckungsgrad_   "Vegetationsbedeckungsgrad"
#define _Blattflaechenindex_          "Blattflaechenindex"
#define _inaktive_Bodenfeuchte_       "inaktive_Bodenfeuchte"
#define _inaktive_Feldkapazitaet_     "inaktive_Feldkapazitaet"

/* einige Definitionen fuer die Abflussoptimierung mit flow_opt */

#define _Abflussoptimierung_          "Abflussoptimierung"
#define _Mh_                          "Mh"
#define _Bh_                          "Bh"
#define _kh_                          "kh"
#define _Th_                          "Th"
#define _Mv_                          "Mv"
#define _Bv_                          "Bv"
#define _kv_                          "kv"

/* fuer Routingclass */
#define _TG_                          "TG"
#define _OL_                          "OL"
#define _SUMTRIB_                     "SUMTRIB"
#define _ZL_                          "ZL"
#define _AL_                          "AL"
#define _SP_                          "SP"
#define _constant_                    "constant"
#define _Schwelle_                    "Schwelle"
#define _extern_                      "extern"
#define _intern_                      "intern"


#define _Theta_                         "theta"
#define _Hydraulische_Hoehe_            "Hydraulische_Hoehe"
#define _Geodaetische_Hoehe_            "Geodaetische_Hoehe"
#define _Q_unten_                       "Q_unten"
#define _Concentrations_                "concentrations"
#define _Grundwasserstand_              "Grundwasserstand"
#define _Theta_in_GWLEVEL_              "Theta_in_GWLEVEL"
#define _Grundwasserneubildung_         "Grundwasserneubildung"
#define _Grundwasserlevel_              "Grundwasserlevel"
#define _Drainageabfluss_               "Drainageabfluss"
#define _Drainagedistance_              "Drainageabstand"
#define _Drainagehoehen_                "Drainagetiefen"
#define _Drainagelevel_                 "Drainagelevel"
#define _Tangens_Beta_                  "Tangens_Beta"
#define _ponding_storage_               "ponding_storage"
#define _Max_Pumping_                   "Max_Pumping"
#define _SB_1_grid_                     "SB_1_grid"
#define _SB_2_grid_                     "SB_2_grid"
#define _ROOTgrid_                      "ROOTgrid"
#define _SBLOW_grid_                    "SBLOW_grid"
#define _SBHIGH_grid_                   "SBHIGH_grid"
#define _THETA_LIMIT_FOR_ETP_REDUCTION_ "_THETA_LIMIT_FOR_ETP_REDUCTION_"
#define _river_infiltration_            "river_infiltration"
#define _river_exfiltration_            "river_exfiltration"
#define _groundwater_balance_           "groundwater_balance"

#define _gw_k_x_                        "gw_k_x_"
#define _gw_k_y_                        "gw_k_y_"
#define _gw_head_old_                   "gw_head_old_"
#define _gw_head_                       "gw_head_"
#define _gw_storage_coeff_              "gw_storage_coeff_"
#define _gw_boundary_fix_h_             "gw_boundary_fix_h_"
#define _gw_boundary_fix_q_             "gw_boundary_fix_q_"
#define _gw_transmissivity_x_           "gw_transmissivity_x_"
#define _gw_transmissivity_y_           "gw_transmissivity_y_"
#define _aquifer_thickness_             "Aquifer_Maechtigkeit_"
#define _gw_flow_x_                     "gw_flow_x_"
#define _gw_flow_y_                     "gw_flow_y_"
#define _gw_kolmation_                  "gw_kolmation_"
#define _gw_leakage_                    "gw_leakage_"
#define _aquifer_upper_limit_           "aquifer_upper_limit"

/* Program print outs */

#define _actual_parameters_           "aktuelle Parameter"
#define _Meteodatenzeit_              "(Zeit der Meteodaten)"
#define _Schneeverdunstung_           "Schneeverdunstung"
#define _ETP_                         "pot. Verdunstung"
#define _ETR_                         "reale Verdunstung"
#define _Scenarios_                   "mit Szenarien!"
#define _suchradius_                  "keine Station innerhalb des angegebenen Suchradius!"

#define _tracer_ini_                  "tracer_ini"
#define _substance_transport_         "Stofftransport"
#define _radioactive_                 "radioaktiv"
#define _non_radioactive_             "nicht_radioaktiv"
#define _half_time_                   "Halbwertszeit"
#define _days_                        "Tage"
#define _mgl_                         "mg/l"
#define _kgkg_                        "kg/kg"
#define _gl_                          "g/l"
#define _evaporating_                 "verdunstend"
#define _non_evaporating_             "nicht_verdunstend"
#define _min_conc_                    "min_conc"
#define _max_conc_                    "max_conc"
#define _soilini_                     "soilini"
#define _gwini_                       "gwini"


/* end of german definitions, now english definitions */

#else

/* Abschnitte und Bezeichnungen aus der Steuerdatei */
#define _Programmausgabe_             "[output_interval]"
#define _output_list_                 "[output_list]"
#define _Koordinaten_                 "[coordinates]"
#define _region_transition_distance_  "[region_transition_distance]"
#define _Hoehenmodell_                "[elevation_model]"
#define _Zonengrid_                   "[zone_grid]"
#define _Standardgrids_               "[standard_grids]"
#define _variable_Grids_              "[variable_grids]"
#define _Modellzeit_                  "[model_time]"
#define _anzahl_                      "[meteo_data_count]"
#define _meteonamen_                  "[meteo_names]"
#define _referenceCrop_				  "[reference_crop]"		
#define _Niederschlagskorrektur_      "precipitation_correction"
#define _Expositionskorrektur_        "radiation_correction"
#define _Verdunstungsmodell_          "evapotranspiration"
#define _Schneemodell_                "snow_model"
#define _Interzeptionsmodell_         "interception_model"
#define _Infiltrationsmodell_         "infiltration_model"
#define _Bodenmodell_                 "soil_model"
#define _Unsatzonmodell_              "unsatzon_model"
#define _gwflow_                      "groundwater_flow"
#define _irrigation_amounts_          "irrigation_amounts"
#define _irrigation_parameters_       "irrigation"
#define _Abflussmodell_               "routing_model"
#define _Defaults_                    "defaults"
#define _MultiLayerLUSETable_         "[multilayer_landuse]"
#define _Landnutzungstabelle_         "[landuse_table]"
#define _Bodenartentabelle_           "[soil_table]"
#define _irrigation_table_            "[irrigation_table]"
/* Meteogroessen */

#define _Globalstrahlung_             "global_radiation"
#define _Nettostrahlung_              "net_radiation"
#define _Sonnenscheindauer_           "sunshine_duration"
#define _Temperatur_                  "temperature"
#define _Temperatur_14_               "temperature_14"
#define _Windgeschwindigkeit_         "wind_speed"
#define _Dampfdruck_                  "vapor_pressure"
#define _Dampfdruck_14_               "vapor_pressure_14"
#define _Luftfeuchte_                 "air_humidity"
#define _Luftfeuchte_14_              "air_humidity_14"
#define _Luftdruck_                   "air_pressure"
#define _Niederschlag_                "precipitation"

/* Standardgrids und veraenderliche Standardgrids */

#define _regression_regions_1_        "regression_regions"
#define _regression_regions_2_        "regression_regions2"
#define _regression_regions_3_        "regression_regions3"
#define _regression_weights_1_        "regression_weights1"
#define _regression_weights_2_        "regression_weights2"
#define _regression_weights_3_        "regression_weights3"
#define _RelCellArea_                 "RelCellArea"
#define _CellSizeX_                   "CellSizeX"
#define _CellSizeY_                   "CellSizeY"
#define _CoordY_                      "CoordY"
#define _CoordX_                      "CoordX"
#define _Bodenarten_                  "soil_types"
#define _Landnutzung_                 "landuse"
#define _ice_firn_                    "ice_firn"
#define _Nutzbare_Feldkapazitaet_     "available_soil_moisture"
#define _Gefaelle_                    "slope_angle"
#define _Exposition_                  "slope_aspect"
#define _Topographischer_Faktor_      "topographic_faktor"
#define _Zonengrid_Bodenmodell_       "zonegrid_soilmodel"
#define _Hydraulische_Leitfaehigkeit_ "hydraulic_conductivity"
#define _SkyViewFactor_               "sky_view_factor"
#define _Fliesszeiten_                "flow_times"
#define _Albedo_                      "albedo"
#define _Bodenspeicher_               "soil_storage"
#define _max_ponding_storage_         "max_ponding_storage"
#define _clay_depth_                  "clay_depth"
#define _irrigation_codes_            "irrigation_codes"
#define _river_depth_                 "river_depth"
#define _river_width_                 "river_width"
#define _river_links_                 "river_links"
#define _kolmation_                   "kolmation"

/* Speichergrids fuer Zustaende/Fluesse */

#define _Auffuellbare_Porositaet_     "fillable_porosity"
#define _Saettigungsdefizit_          "saturation_deficite"
#define _Bestandesniederschlag_       "throughfall"
#define _Schneeausfluss_              "snowcover_outflow"
#define _Snow_out_                    "melt_from_snowcover"
#define _Schneetage_                  "days_snow"
#define _Schneealter_                 "snow_age"
#define _Schneeanteil_                "snow_rate"
#define _Regenanteil_                 "rain_rate"
#define _melt_from_firn_              "melt_from_firn"
#define _melt_from_ice_               "melt_from_ice"

/* #define _Wasseraequivalent_           "Wasseraequivalent" */

#define _Unsaturated_Zone_Storage_    "unsaturated_zone_storage"
#define _Interflow_Storage_           "interflow_itorage"
#define _Saturated_Soil_Partition_    "saturated_soil_partition"
#define __sbalt_grid__                "$$sbold_grid$$"
#define _Bodenspeicher_Alt_           "soil_storage_old"
#define _Basisabfluss_                "baseflow"
#define _Direktabfluss_               "directflow"
#define _Interflow_                   "interflow"
#define _Peakflow_                    "peakflow"
#define _Nacht_Temperatur_            "temperature_night"
#define _Korrigierte_Temperatur_      "corr_temperature"
#define _Potentielle_Verdunstung_     "pot_transpiration"
#define _Reale_Verdunstung_           "real_transpiration"
#define _Potential_Evaporation_       "potential_evaporation"
#define _Reale_Evaporation_           "real_evaporation"
#define _Snow_Evaporation_            "snow_evaporation"
#define _Potential_InterceptionEvap_  "potential_InterceptionEvap"
#define _Reale_InterceptionEvap_      "real_InterceptionEvap"
#define _Nadelwald_                   "pineforest"
#define _Schatten_                    "Shadow"
#define __Nacht_Temperatur__          "$$temperature_night$$"
#define _Infiltrationsexzess_         "infiltration_excess"
#define _Infiltrationssaettigung_     "infiltration_saturation"
#define _ungueltiger_Name_            "ungueltiger_Name"
#define _Interzeptionsspeicher_       "interception_storage"
#define _Interzeptionsverdunstung_    "interception_evaporation"
#define _Schneespeicher_Fest_         "snowstorage_solid"
#define _Schneespeicher_Fluessig_     "snowstorage_liquid"
#define _Wurzeltiefe_                 "root_depth"
#define _Vegetationsbedeckungsgrad_   "vegetation_coverage_degree"
#define _Blattflaechenindex_          "leaf_area_index"
#define _inaktive_Bodenfeuchte_       "inaktive_soil_moisture"
#define _inaktive_Feldkapazitaet_     "inaktive_field_capacity"

/* einige Definitionen fuer die Abflussoptimierung mit flow_opt */

#define _Abflussoptimierung_          "discharge_optimizing"
#define _Mh_                          "Mh"
#define _Bh_                          "Bh"
#define _kh_                          "kh"
#define _Th_                          "Th"
#define _Mv_                          "Mv"
#define _Bv_                          "Bv"
#define _kv_                          "kv"

/* fuer Routingclass */
#define _TG_                          "TG"
#define _OL_                          "OL"
#define _SUMTRIB_                     "SUMTRIB"
#define _ZL_                          "ZL"
#define _AL_                          "AL"
#define _SP_                          "SP"
#define _constant_                    "constant"
#define _Schwelle_                    "Threshold"
#define _extern_                      "extern"
#define _intern_                      "intern"

#define _Theta_                         "theta"
#define _Hydraulische_Hoehe_            "hydraulic_heads"
#define _Geodaetische_Hoehe_            "geodetic_heads"
#define _Q_unten_                       "q_down"
#define _Concentrations_                "concentrations"
#define _Grundwasserstand_              "groundwater_distance"
#define _Theta_in_GWLEVEL_              "Theta_in_GWLEVEL"
#define _Grundwasserneubildung_         "groundwater_recharge"
#define _Grundwasserlevel_              "groundwater_level"
#define _Drainageabfluss_               "drainage_flow"
#define _Drainagedistance_              "drainage_distance"
#define _Drainagehoehen_                "drainage_depth"
#define _Drainagelevel_                 "drainage_level"
#define _Tangens_Beta_                  "tangens_beta"
#define _ponding_storage_               "ponding_storage"
#define _Max_Pumping_                   "Max_Pumping"
#define _SB_1_grid_                     "SB_1_grid"
#define _SB_2_grid_                     "SB_2_grid"
#define _ROOTgrid_                      "ROOTgrid"
#define _SBLOW_grid_                    "SBLOW_grid"
#define _SBHIGH_grid_                   "SBHIGH_grid"
#define _THETA_LIMIT_FOR_ETP_REDUCTION_ "_THETA_LIMIT_FOR_ETP_REDUCTION_"
#define _river_infiltration_            "river_infiltration"
#define _river_exfiltration_            "river_exfiltration"
#define _groundwater_balance_           "groundwater_balance"
#define _macropore_infiltration_        "macropore_infiltration"

#define _gw_k_x_                        "gw_k_x_"
#define _gw_k_y_                        "gw_k_y_"
#define _gw_head_old_                   "gw_head_old_"
#define _gw_head_                       "gw_head_"
#define _gw_storage_coeff_              "gw_storage_coeff_"
#define _gw_boundary_fix_h_             "gw_boundary_fix_h_" 
#define _gw_boundary_fix_q_             "gw_boundary_fix_q_"
#define _gw_transmissivity_x_           "gw_transmissivity_x_"
#define _gw_transmissivity_y_           "gw_transmissivity_y_"
#define _aquifer_thickness_             "aquifer_thickness_"
#define _gw_flow_x_                     "gw_flow_x_"
#define _gw_flow_y_                     "gw_flow_y_"
#define _gw_kolmation_                  "gw_kolmation_"
#define _gw_leakage_                    "gw_leakage_"
#define _aquifer_upper_limit_           "aquifer_upper_limit"

#define _actual_parameters_           "actual parameters"
#define _Meteodatenzeit_              "(time of meteo data)"
#define _Schneeverdunstung_           "snow_evaporation"
#define _ETP_                         "pot. evaptranspir."
#define _ETR_                         "real evapotranspir."
#define _Scenarios_                   "using scenarios!"
#define _suchradius_                  "no station within the specified search radius!"

/* transport algorithms */

#define _tracer_ini_                  "tracer_ini"
#define _substance_transport_         "substance_transport"
#define _radioactive_                 "radioactive"
#define _non_radioactive_             "non_radioactive"
#define _half_time_                   "half_time"
#define _days_                        "days"
#define _mgl_                         "mg/l"
#define _kgkg_                        "kg/kg"
#define _gl_                          "g/l"
#define _evaporating_                 "evaporating"
#define _non_evaporating_             "non_evaporating"
#define _min_conc_                    "min_conc"
#define _max_conc_                    "max_conc"
#define _soilini_                     "soilini"
#define _gwini_                       "gwini"

#endif

// definitions for new WaSiM-ETH 7 and higher doesn't use german definitions any longer 

#define _ExternalCoupling_             "[ExternalCoupling]"
#define _GWTableExtern_                "GWTableExtern"
#define _RenameMode_Delete_            0
#define _RenameMode_Rename_            1
#define _BALANCE_                      "Balance"
#define _SurfaceCanopyResistance_      "SurfaceCanopyResistance"
#define _SurfaceInterzepResistance_    "SurfaceInterzepResistance"
#define _SurfaceEvaporationResistance_ "SurfaceEvaporationResistance"
#define _RoughnessLength_              "RoughnessLength"
#define _Capillar_Uprise_              "Capillar_Uprise"
#define _Percolation_                  "Percolation"
#define _Schneespeicher_Total_         "snow_storage_total"
#define _AspectCorrectedGlobalRadiation_ "AspectCorrectedGlobalRadiation"
#endif







