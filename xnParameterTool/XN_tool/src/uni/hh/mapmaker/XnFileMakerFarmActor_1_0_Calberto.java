package uni.hh.mapmaker;

import general.GenericTools;
import general.TextFile;

import java.io.*;

import java.sql.ResultSet;
//import java.sql.ResultSetMetaData;
import java.sql.SQLException;
import java.text.DecimalFormat;
import java.text.DecimalFormatSymbols;
import java.text.NumberFormat;
import java.text.SimpleDateFormat;

//import java.sql.Statement;
//import java.util.ArrayList;

//import java.util.HashMap;
import java.util.ArrayList;
import java.util.Calendar;
import java.sql.Date;
import java.util.GregorianCalendar;
import java.util.Locale;
import java.util.StringTokenizer;
import java.util.TimeZone;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import cropmod.UpdateGenetics;
import cropmod.XnSimObj;
import dataclasses.Crop;
import dataclasses.Action;
import dataclasses.Modus;
import dataclasses.SoilValues;
import dataclasses.WeatherDB;
import dataclasses.WeatherDD;
import dataclasses.XnConfig;

//import uni.hh.mysql_db.*;
/**
 * FileMaker Class by German Calberto is used to write data from a MySQL database into files 
 * that are input files for the model Expert-N
 * @author German Calberto
 * Adaptations to the model of J. Aurbacher
 */
public class FileMakerNew {
	final TimeZone tz = TimeZone.getTimeZone("UTC")  ;
	final double maxp = 30D; //Maximum precipitation per day that XN can cope with, rest will be carried over to the next day.
	boolean useLEACHN; //default DAISY
	boolean useStaticSoilvalues; //no use of read results of soilvalues
	boolean useStaticWater;       //no use of read results of soil water contents
	boolean useChngGenetics; //default = Static genetics
	CropData cd = new CropData();
	private String configFile = "";
	private GetFromDB gfdb = new GetFromDB();
	private SimpleDateFormat xndateform = new SimpleDateFormat("ddMMyy");
	Locale loc = Locale.UK;
	DecimalFormat dblform;
	


	
	//Here all the generic scripting variables are initialised
	//String newLine = System.getProperty("line.separator"); //System independent hard return
	String newLine = "\r\n"; //CT 100408 to make windows line breaks although being in Linux
	String space = " ";
	String var_txt_general_MCK001_xnd_line ="=============================================================================";
	String var_txt_general_MCK001_xnd_line_two ="===================================================================================================================================";
	String var_txt_general_MCK001_xnd_slash	="/";
	String var_txt_general_MCK001_xnd_line_three ="-----------------"; 
	String var_txt_general_MCK001_xnd_line_four ="==================================================================================================================";	
	String var_txt_general_MCK001_xnd_line_five ="-----------------------------------------------------------------------------------------------";	
	String var_txt_general_MCK001_xnd_line_six ="------------------------------------------------------------------------------------------------------------------";
	String tab = "\t";
	String var_txt_MCK001_xnd_title	="** Test-File for Simulation Object";
	String var_txt_general_MCK001_xnd_block1_title ="*Simulation data";
	String var_txt_MCK001_xnd_block1_number ="10000";
	String var_txt_general_MCK001_xnd_block1_txt1 ="*Start Date (ddmmyy)" + tab + "End Date (ddmmyy)";
	String var_txt_general_MCK001_xnd_block2_title ="*Farm Data";
	String var_txt_MCK001_xnd_block2_number ="10001";
	String var_txt_general_MCK001_xnd_block2_txt1 ="*FarmID	No.Plots" + tab + "Latitude" + tab + "Longitude" + tab + "Altitude";
	String var_txt_general_MCK001_xnd_block2_txt2 ="*LastName" + tab + "FirstName" + tab + "Street";
	String var_txt_general_MCK001_xnd_block2_txt3 ="*PLZ" + tab + "City" + tab + "Tel." + tab +	"Fax."; 
	String var_txt_general_MCK001_xnd_block3_title ="*Plot Data";
	String var_txt_MCK001_xnd_block3_number ="10002";
	String var_txt_general_MCK001_xnd_block3_txt1 ="*PlotID	Name" + tab + "Size" + tab + "Num.Subplots";
	String var_txt_general_MCK001_xnd_block4_title ="*Subplot data";
	String var_txt_MCK001_xnd_block4_number ="10003";
	String var_txt_general_MCK001_xnd_block4_txt1 ="*ID	Name" + tab + "Princ.Crop"  + tab + "Weatherfile" + tab + "Parameterfile"  + tab + "ConfigFile";
	String var_txt_general_MCK001_xnd_block4_txt2 ="*AreaNum." + tab + "Size" + tab + "Slope" + tab + "Exposition";
	String var_txt_general_MCK001_xnd_block5_title ="*Plant Data";
	String var_txt_general_MCK001_xnd_block5_txt1 ="*Harvest Residues of preceding crop (before simulation starts)";
	String var_txt_general_MCK001_xnd_block5_txt2 ="Fruchtart" + tab + "Ertrag" + tab + "Ernte-" + tab + "Ernte" + tab + tab + "C/N Ernte" + tab + "Wurzel-" + tab + tab + "C/N Wurzel";
	String var_txt_general_MCK001_xnd_block5_txt3 ="termin" + tab +	"rückstände" + tab + "rückstände" + tab + "rückstände" + tab + "rückstände";
	String var_txt_general_MCK001_xnd_block5_txt4 ="kg/ha" + tab + "Datum" + tab + "kg/ha" + tab + tab + "1/1" + tab + tab + "kg/ha" + tab + tab + "1/1"; 
	String var_txt_MCK001_xnd_block5_number = "10004";
	String var_txt_general_MCK001_xnd_block6_title ="*Cultivated Crop";
	String var_txt_general_MCK001_xnd_block6_txt1 ="Nr.	Bez." + tab + "Variety" + tab + tab + "Sowing" + tab + "Row-" + tab + "Seed-" + tab + "Plant" + tab + "Emerge" + tab + "Harvest	Date" + tab + "max.Root." + tab + "max. Root." + tab + "Removal";
	String var_txt_general_MCK001_xnd_block6_txt2 = tab + "Art" + tab + "name" + tab + tab + "depth" + tab + "space" + tab + "density" + tab + "Sowing" + tab + "Date" + tab + "Date" + tab + "max." + tab + "Date" + tab + tab + "Depth" + tab + tab + "Yes=1";
	String var_txt_general_MCK001_xnd_block6_txt3 = tab + tab + tab + tab + tab + tab + tab + tab + tab + tab + "Biom." + tab + tab + tab + tab + tab +	"No=0";
	String var_txt_general_MCK001_xnd_block6_txt4 = tab + "Code" + tab + tab + tab + "m" + tab + "m" + tab + "seed/m2" + tab + "Datum" + tab + "Datum" + tab + "Datum" + tab + "Datum" + tab + "Datum" + tab + tab + "m";
	String var_txt_MCK001_xnd_block6_number ="10005";
	String var_txt_general_MCK001_xnd_block6_txt5 =":Number of crops";
	String var_txt_MCK001_xnd_title_two	="MANAGEMENT DATA";
	String var_txt_general_MCK001_xnd_block7_title = "*Mineral Fertilization (Pro Massnahme)";
	String var_txt_general_MCK001_xnd_block7_txt1 = "Date" + tab + "Fertilizer" + tab + "Fertilizer" + tab + "N" + tab + "NO3-N" + tab + "NH4-N" + tab + "Amid-N";
	String var_txt_general_MCK001_xnd_block7_txt2 = "Datum" + tab + "Name" + tab + tab + "Code" + tab + tab + "kg/ha" + tab + "kg/ha" + tab + "kg/ha" + tab + "kg/ha";
	String var_txt_MCK001_xnd_block7_number = "10006";
	String var_txt_general_MCK001_xnd_block7_txt3 = ":Number of MINERAL Fertilizer Applications";
	String var_txt_general_MCK001_xnd_block8_title = "*Organic Fertilization (Pro Massnahme)";
	String var_txt_general_MCK001_xnd_block8_txt1 = "Date" + tab + "Org_fertilizer" + tab + "Org_fertilizer" + tab + "DM" + tab + "N" + tab + "NH4-N" + tab + "Org.Sub.";
	String var_txt_general_MCK001_xnd_block8_txt2 = "Datum" + tab + "Name" + tab + tab + "Code" + tab + tab + "kg/ha" + tab + "kg/ha" + tab + "kg/ha" + tab + "kg/ha";	
	String var_txt_MCK001_xnd_block8_number = "10007";
	String var_txt_general_MCK001_xnd_block8_txt3 = ":Number of ORGANIC Fertilizer Applications";
	String var_txt_general_MCK001_xnd_block9_title ="*Soil Management";
	String var_txt_general_MCK001_xnd_block9_txt1 ="Date" + tab + "Depth" + tab + "Equip." + tab + "Equip.";
	String var_txt_general_MCK001_xnd_block9_txt2 ="Datum" + tab + "m" + tab + "Name" + tab + "Code";
	String var_txt_MCK001_xnd_block9_number ="10008";
	String var_txt_general_MCK001_xnd_block9_txt3 = ":Number of soil cultivation activities"; 
	String var_txt_general_MCK001_xnd_block10_title ="*Irrigation (Pro MaÃŸnahme)";
	String var_txt_general_MCK001_xnd_block10_txt1 ="Date" + tab + "Amount" + tab + "Irrig";
	String var_txt_general_MCK001_xnd_block10_txt2 ="Datum" + tab + "mm" + tab + "Code";
	String var_txt_MCK001_xnd_block10_number ="10009";
	String var_txt_general_MCK001_xnd_block10_txt3 =":Number of Irrigation applications";
	String var_txt_general_MCK001_xnd_block11_txt1 ="*SIMULATIONS LAYERS (equidistant)";
	String var_txt_general_MCK001_xnd_block11_txt2 = tab + "Num.layers" + tab + "Num.Horizons";
	String var_txt_general_MCK001_xnd_block11_txt3 = tab + "(10,100)" + tab + "(1 cm, 50 cm)";
	String var_txt_general_MCK001_xnd_block11_txt4 ="Layer" + tab + "Num.Layers" + tab + "Clay" + tab + "Silt" + tab + "Sand" + tab + "Organ." + tab + "Bulk.Density" + tab + "Stone.cont" + tab + "pH";
	String var_txt_general_MCK001_xnd_block11_txt5 ="number" + tab + "per.horizon" + tab + "wgt.%" + tab + "wgt.%" + tab + "wgt.%" + tab + "wgt.%" + tab + "kg/dm3" + tab + tab + "Vol.%" + tab + tab + "1";
	String var_txt_MCK001_xnd_block11_number ="10010" ;
	String var_txt_general_MCK001_xnd_block12_txt1 ="*START VALUES";
	String var_txt_general_MCK001_xnd_block12_txt2 =tab + "DateSampling";
	String var_txt_general_MCK001_xnd_block12_txt3 ="Layer" + tab + "Num.Layers" + tab + "WaterCont" + tab + "MatrixPot" + tab + "Soiltemp" + tab + "NH4" + tab + "NO3" + tab + "Root.Density";
	String var_txt_general_MCK001_xnd_block12_txt4 ="number	per.horizon" + tab + "Vol.%" + tab + tab + "kPa" + tab + tab + "°C" + tab + tab + "kg/ha" + tab + "kg/ha" + tab + "kg/dm3";
	String var_txt_MCK001_xnd_block12_number ="10011";
	String var_txt_general_MCK001_xnd_block12_txt5 =":Number of soil layers";
	String var_txt_general_MCK001_xnd_block13_txt1 ="*WATER READINGS";
	String var_txt_general_MCK001_xnd_block13_txt2 ="Number of Events" + tab + "Number of measurement depths (max. 5)";
	String var_txt_general_MCK001_xnd_block13_txt3 ="Measuring depths (cm)";
	String var_txt_general_MCK001_xnd_block13_txt4 ="Date" + tab + "Water.cont" + tab + "Matrix.Pot";
	String var_txt_general_MCK001_xnd_block13_txt5 ="Datum" + tab + "Vol.%" + tab + tab + "kPa";
	String var_txt_MCK001_xnd_block13_number ="10012";
	String var_txt_general_MCK001_xnd_block14_title ="*NITROGEN MEASUREMENTS";
	String var_txt_general_MCK001_xnd_block14_txt1 ="Number of Events" + tab + "Number of measurement depths (max. 5)";
	String var_txt_general_MCK001_xnd_block14_txt2 ="Measuring depths (cm)";
	String var_txt_general_MCK001_xnd_block14_txt3 ="Date" + tab + "Nitrat-N" + tab + "Ammonium-N";
	String var_txt_general_MCK001_xnd_block14_txt4 ="Datum" + tab + "kg/ha" + tab + tab + "kg/ha";
	String var_txt_MCK001_xnd_block14_number ="10013";
	String var_txt_general_MCK001_xnd_block15_title ="*Plant Measurements I";
	String var_txt_general_MCK001_xnd_block15_txt1 ="Termin/" + tab + tab + "Entw." + tab + "BFI" + tab + "Boden" + tab + "Blatt" + tab + "Trieb" + tab + "Pflanzen" + tab + "Wurzel" + tab + "Verd." + tab + "Kum. N";
	String var_txt_general_MCK001_xnd_block15_txt2 ="Datum" + tab + tab + "Stad." + tab + tab + "bedeck." + tab + "anz." + tab + "zahl" + tab + "hoehe" + tab + tab + "tiefe" + tab + "faktor" + tab + "Aufnahme";
	String var_txt_general_MCK001_xnd_block15_txt3 ="DD.MM.YY" + tab + "EC 1/1" + tab + "1/1" + tab + "1/1" + tab + "1/1" + tab + "1/m2" + tab + "cm" + tab + tab + "cm" + tab + "1/1" + tab + "kg/ha";
	String var_txt_MCK001_xnd_block15_number ="10014";
	String var_txt_general_MCK001_xnd_block15_txt4 =":Number of measurement dates";
	String var_txt_general_MCK001_xnd_block16_title ="*Plant Measurements II";
	String var_txt_general_MCK001_xnd_block16_txt1 ="-----------------Dry matter-----------------/------N-content in % the dry matter------";
	String var_txt_general_MCK001_xnd_block16_txt2 ="Termin" + tab + tab + "Blätter" + tab + "Halme" + tab + "Frucht" + tab + "Wurzel" + tab + "Obere" + tab + "BlÃ¤tter" + tab + "Halme/" + tab + "Frucht" + tab + "Wurzel" + tab + "Obere";
	String var_txt_general_MCK001_xnd_block16_txt3 ="Datum" + tab + tab + tab + "Stengel"	+ tab + tab + tab + "Biom." + tab + "N" + tab + "Stengel" + tab + "N" + tab + "N" + tab + "Biom.N";
	String var_txt_general_MCK001_xnd_block16_txt4 ="dd.mm.yy" + tab + "kg/ha" + tab + "kg/ha" + tab + "kg/ha" + tab + "kg/ha" + tab + "kg/ha" + tab + "%" + tab + "%" + tab + "%" + tab + "%" + tab + "%";
	String var_txt_MCK001_xnd_block16_number ="10015";
	String var_txt_general_MCK001_xnd_block16_txt5 =":Number of measurement dates";
	String var_txt_general_MCK001_xnd_block17_title ="***" + tab + "Expert-N" + tab + "Ende Eingabedatei" + tab + "***";
	String var_txt__xnw_title ="*** Expert-N | Wetterdaten ***";
	String var_txt_general_xnw_block1_txt1 ="Wetter-" + tab + "|Breiten|Laengen|Hoehe" + tab + "|Temperatur-" + tab + "|Temp.Amplitude" + tab + "|Hoehe d.Temp." + tab + "|Hoehe d.Wind-";
	String var_txt_general_xnw_block1_txt2 ="Station" + tab + "|grad" + tab + "|grad" + tab + "|NN" + tab + "|mittel(Jahr)" + tab + "|Monatsmittel" + tab + "|messung" + tab + "|messung";
	String var_txt_general_xnw_block1_txt3 ="" + tab + "|" + tab + "|" + tab + "|m" + tab + "|°C" + tab + tab + "|°C" + tab + tab + "|m" + tab + tab + "|m";
	String var_txt_xnw_block1_number ="20000";
	String var_txt_general_xnw_block2_txt1 ="1" + tab + "2" + tab + "3" + tab + "4" + tab + "5" + tab + "6" + tab + "7" + tab + "8" + tab + "9" + tab + "10" + tab + "11" + tab + "12" + tab + "13" + tab + "14" + tab + "15" + tab + "16" + tab + "17" + tab + "18" + tab + "19";
	String var_txt_xnw_block2_number ="20001";
	String var_txt_general_xnw_block2_txt2 =":Anzahl der Simulationstage - Number of simulation days";
	String var_txt_xnw_block3_text1 ="*** Expert-N | Ende Wetterdaten ***";
	String var_txt_xnw_block3_text2 ="1: Tag,Monat";
	String var_txt_xnw_block3_text3 ="2: Jahr";
	String var_txt_xnw_block3_text4 ="3: Tag im Jahr";
	String var_txt_xnw_block3_text5 ="4: Globalstrahlung in MJ/m2";
	String var_txt_xnw_block3_text6 ="5: maximale Lufttemperatur in °C";
	String var_txt_xnw_block3_text7 ="6: minimale Lufttemperatur in °C";
	String var_txt_xnw_block3_text8 ="7: Niederschlag in mm";
	String var_txt_xnw_block3_text9 ="8: Taupunkttemperatur in °C";
	String var_txt_xnw_block3_text10 ="9: Windgeschwindigkeit in m/s";
	String var_txt_xnw_block3_text11 ="10: Photosynthetisch aktive Strahlung in MJ/m2";
	String var_txt_xnw_block3_text12 ="11: Sonnenscheindauer in Stunden";
	String var_txt_xnw_block3_text13 ="12: mittlere Lufttemperatur in °C";
	String var_txt_xnw_block3_text14 ="13: Relative Luftfeuchtigkeit in %";
	String var_txt_xnw_block3_text15 ="14: Saettigungsdefizit in %";
	String var_txt_xnw_block3_text16 ="15: Kesselverdunstung in mm";
	String var_txt_xnw_block3_text17 ="16: Schneehoehe in mm";
	String var_txt_xnw_block3_text18 ="17: Bodentemp. in 5 cm Tiefe in °C";
	String var_txt_xnw_block3_text19 ="18: Bodentemp. in 10 cm Tiefe in °C";
	String var_txt_xnw_block3_text20 ="19: Bodentemp. in 20 cm Tiefe in °C";
	String var_txt_xnm_title ="---> Parameter Datei für Rasterpunkt 130180 (DP)";
	String var_txt_xnm_block1_txt1 ="---> Simulationsstart:";
	String var_txt_xnm_block1_txt2 ="CHumus,NHumus nach Bodendatenbank,";
	String var_txt_xnm_block1_txt3 ="CLitter und NLitter:  Wurzeln der Vorfrucht nach Faustzahlen.";
	String var_txt_xnm_block1_txt4 ="LitterSurf und StandCropRes (85 % und 15 % der Strohreste)";
	String var_txt_xnm_block1_txt5 ="DAISY: SOMSlow (58,8%) ,SOMFast (39,2 %) ,MicBiomSlow (1.9 %), MicBiomFast (0.1%)";
	String var_txt_xnm_block1_txt6 ="NCSOIL: SOMSlow (20 %), SOMFast (6 %), MicBiomSlow (1 %), MicBiomFast (1 %), FOMSlow (1 %)";
	String var_txt_xnm_block1_txt7 =tab + tab + "FOMFast (1 %), HumusStable (70 %)";
	String var_txt_xnm_block1_txt8 ="DNDC: Standardinitialisierung (20008 und 20009 auskommentieren)";
	String var_txt_xnm_block2_title ="SOIL PHYSICAL PARAMETERS (Bodenbibliothek der Datenerfassung)";
	String var_txt_xnm_block2_txt1 ="Layer" + tab +	"|Soil" + tab +	"|Wilting" + tab + "|Field" + tab + tab + "|Total Pore" + tab + "|Satur. Hydr.";
	String var_txt_xnm_block2_txt2 =tab + "|type" + tab + "|Point" + tab + tab + "|Capacity" + tab + "|Volume" + tab + tab + "|Conductivity";
	String var_txt_xnm_block2_txt3 =tab + "|ADV" + tab + "|Vol.%" + tab + tab + "|Vol.%" + tab + tab + "|Vol.%" + tab + tab + "|mm/d";
	String var_txt_xnm_block2_number ="10020";
	String var_txt_xnm_block3_title ="SOIL HYDRAULIC CURVE PARAMETERS";
	String var_txt_xnm_block3_txt1 ="Layer" + tab +	"|Water" + tab + "|Campbell" + tab + tab + "|van Genuchten";
	String var_txt_xnm_block3_txt2 = tab +	"|Res." + tab + "|Sat." + tab +	"|A" + tab + "|B"+ tab + "|alpha" + tab + "|n" + tab + "|m";
	String var_txt_xnm_block3_txt3 = tab +	"|Vol.%"+ tab + "|Vol%" + tab +	"|kPa" + tab + "|1/1" + tab + "|1/cm" + tab + "|1/1" + tab + "|1/1";	
	String var_txt_xnm_block3_number ="10021";
	String var_txt_xnm_block4_title ="MINERALIZATION MODEL PARAMETER";
	String var_txt_xnm_block4_txt1 ="Layer" + tab + "|distribution factors" + tab +  "|temperature" + tab + "|moisture" + tab + tab + tab + "|nitrification";
	String var_txt_xnm_block4_txt2 = tab + "|MinEff" + tab + "fHum"+ tab + "MinCN" + tab + "|Temp0" + tab + "Q10" + tab + "|Theta0" + tab + "Theta1" + tab + "SatActiv"	+ tab + "|NO3:NH4";
	String var_txt_xnm_block4_txt3 = tab + "|1/1" + tab + "1/1" + tab + "1/1" + tab + "|°C" + tab + "1/1" + tab + "|1/1" + tab + "1/1" + tab + "1/1"+ tab + tab + "|1/1";
	String var_txt_xnm_block4_number ="20001";
	//here to do???
	String var_txt_xnm_block5_title ="RATE CONSTANT (LEACHN, NITS, OMI)";
	String var_txt_xnm_block5_txt1 ="Layer" + tab + "UreaHy :" + tab + "Nitrif. :" + tab + "Denitrif. :" + tab + "MinerLit :" + tab + "MinerMan :" + tab + "MinerHum";
	String var_txt_xnm_block5_txt2 =tab + "1/t" + tab + tab + "1/t" + tab + tab + "1/t" + tab + tab + "1/t" + tab + tab + "1/t" + tab + tab + "1/t"; 
	String var_txt_xnm_block5_number = "20002";
	String var_txt_xnm_block6_title ="START SOIL VALUES C-, N-POOLS LEACHN, NITS, OMI, CERES, DAISY, NCSOIL)";
	String var_txt_xnm_block6_txt1 ="Layer" + tab + "|Litter C" + tab + "Litter N" + tab + "|Manure C" + tab + "Manure N" + tab + "|Humus C" + tab + "Humus N";
	String var_txt_xnm_block6_txt2 = tab + "|kg C/ha" + tab + "kg N/ha" + tab + tab + "|kg C/ha" + tab + "kg N/ha" + tab + tab + "|kg C/ha" + tab +	"kg N/ha";
	String var_txt_xnm_block6_number ="20003";
	String var_txt_xnm_block7_title = "RATE CONSTANT (DAISY,DNDC,NCSOIL)";
	String var_txt_xnm_block7_txt1 = "Layer" + tab + "FOMSlow :" + tab + "FOMFast :" + tab + "FOMVeryFast :" + tab + "MicBiomSlow :" + tab + "MicBiomFast :" + tab + "HumusSlow :" + tab + "HumusFast"; 
	String var_txt_xnm_block7_txt2 = tab + "1/t" + tab + tab + "1/t" + tab + tab + "1/t" + tab + tab + "1/t" + tab + tab + "1/t" + tab + tab + "1/t" + tab + tab + "1/t";
	String var_txt_xnm_block7_number = "20007";
	String var_txt_xnm_block8_title ="START SOIL VALUES C-POOLS (DAISY)";
	String var_txt_xnm_block8_txt1 = "Layer" + tab + "|AOM1" + tab + "|AOM2" + tab + "|AOM3" + tab + "|BOM1" + tab + "|BOM2" + tab + "|BOMD" + tab + "|SOM0" + tab + "|SOM1" + tab + "|SOM2" + tab + "|Csol";
	String var_txt_xnm_block8_txt2 = tab + "|kg C/ha";
	String var_txt_xnm_block8_number ="20008";
	String var_txt_xnm_block9_title ="START SOIL VALUES N-POOLS (DAISY)";
	String var_txt_xnm_block9_txt1 = "Layer" + tab + "|AOM1" + tab + "|AOM2" + tab + "|AOM3" + tab + "|BOM1" + tab + "|BOM2" + tab + "|BOMD" + tab + "|SOM0" + tab + "|SOM1" + tab + "|SOM2";
	String var_txt_xnm_block9_txt2 = tab + "|kg N/ha";
	String var_txt_xnm_block9_txt3 = "End of Simfile";
	String var_txt_xnm_block9_number = "20009";
	
	
	public FileMaker(String cfg, Modus mod){
		TimeZone.setDefault(tz);
		configFile = cfg;
		this.useLEACHN = mod.isUse_leachn();
		this.useStaticSoilvalues = mod.isUse_staticsoil();
		this.useStaticWater = mod.isUse_staticwater();
		this.useChngGenetics = mod.isUse_changing_genetics();
		
		NumberFormat df = DecimalFormat.getInstance(loc);
		((DecimalFormat)df).setMinimumFractionDigits(0);
		((DecimalFormat)df).setMaximumFractionDigits(4);
		((DecimalFormat)df).setGroupingUsed(false);
		DecimalFormatSymbols dfs = new DecimalFormatSymbols(loc);
		((DecimalFormat)df).setDecimalFormatSymbols(dfs);
		dblform=(DecimalFormat)df;
	}
	
//******************************FROM HERE STARTS THE XNM FILE**************************************************
	public void asciiFileMaker_XNM_API (XnSimObj xnsimob){
		
		String the_file = xnsimob.getWorkingfolder()+"/param/soilparam.xnm"; 
		
			try{
			
			PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(the_file)));
			
			out.write(var_txt_general_MCK001_xnd_line);
			out.write(newLine);
			out.write(gfdb.getXnm_file_name((int)xnsimob.getSoiltypeid())+ ".mod");
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line);
			out.write(newLine);
			out.write(var_txt_xnm_title);
			out.write(newLine);
			out.write(var_txt_xnm_block1_txt1);
			out.write(newLine);
			out.write(newLine);
			out.write(var_txt_xnm_block1_txt2);
			out.write(newLine);
			out.write(var_txt_xnm_block1_txt3);
			out.write(newLine);
			out.write(var_txt_xnm_block1_txt4);
			out.write(newLine);
			out.write(newLine);
			out.write(var_txt_xnm_block1_txt5);
			out.write(newLine);
			out.write(newLine);
			out.write(var_txt_xnm_block1_txt6);
			out.write(newLine);
			out.write(var_txt_xnm_block1_txt7);
			out.write(newLine);
			out.write(var_txt_xnm_block1_txt8);
			out.write(newLine);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line);
			out.write(newLine);
			out.write(var_txt_xnm_block2_title);
			out.write(newLine);
			out.write(var_txt_xnm_block2_txt1);
			out.write(newLine);
			out.write(var_txt_xnm_block2_txt2);
			out.write(newLine);
			out.write(var_txt_xnm_block2_txt3);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line);
			out.write(newLine);
			out.write(var_txt_xnm_block2_number);
			out.write(newLine);
			this.writeSoilPhysicalParam_API(out,(int)xnsimob.getSoiltypeid());
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line);
			out.write(newLine);
			out.write(var_txt_xnm_block3_title);
			out.write(newLine);
			out.write(var_txt_xnm_block3_txt1);
			out.write(newLine);
			out.write(var_txt_xnm_block3_txt2);
			out.write(newLine);
			out.write(var_txt_xnm_block3_txt3);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line);
			out.write(newLine);
			out.write(var_txt_xnm_block3_number);
			out.write(newLine);
			this.writeSoilHydraulicCurveParam_API(out,(int)xnsimob.getSoiltypeid());
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line);
			out.write(newLine);
			out.write(var_txt_xnm_block4_title);
			out.write(newLine);
			out.write(newLine);
			out.write(var_txt_xnm_block4_txt1);
			out.write(newLine);
			out.write(var_txt_xnm_block4_txt2);
			out.write(newLine);
			out.write(var_txt_xnm_block4_txt3);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line_five);
			out.write(newLine);
			out.write(var_txt_xnm_block4_number);
			out.write(newLine);
			this.writeMineral_mod_param_API(out, (int)xnsimob.getSoiltypeid());
			out.write(var_txt_general_MCK001_xnd_line);
			out.write(newLine);
			out.write(newLine);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line);
			out.write(newLine);
			out.write(var_txt_xnm_block5_title);
			out.write(newLine);
			out.write(var_txt_xnm_block5_txt1);
			out.write(newLine);
			out.write(var_txt_xnm_block5_txt2);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line_five);
			out.write(newLine);
			out.write(var_txt_xnm_block5_number);
			out.write(newLine);
			this.writeRate_constant_leachN(out, (int)xnsimob.getSoiltypeid());
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line);
			out.write(newLine);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line);
			out.write(newLine);
			out.write(var_txt_xnm_block6_title);
			out.write(newLine);
			out.write(newLine);
			out.write(var_txt_xnm_block6_txt1);
			out.write(newLine);
			out.write(var_txt_xnm_block6_txt2);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line_five);
			out.write(newLine);
			out.write(var_txt_xnm_block6_number);
			out.write(newLine);	
			this.writeStart_soil_values_C_API(out, xnsimob);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line);
			out.write(newLine);
			out.write(var_txt_xnm_block7_title);
			out.write(newLine);
			out.write(newLine);
			out.write(var_txt_xnm_block7_txt1);
			out.write(newLine);
			out.write(var_txt_xnm_block7_txt2);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line_six);
			out.write(newLine);
			out.write(var_txt_xnm_block7_number);
			out.write(newLine);
			this.writeRate_constant_API(out,(int)xnsimob.getSoiltypeid());
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line);
			out.write(newLine);
			out.write(var_txt_xnm_block8_title);
			out.write(newLine);
			out.write(newLine);
			out.write(var_txt_xnm_block8_txt1);
			out.write(newLine);
			out.write(var_txt_xnm_block8_txt2);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line_five);
			out.write(newLine);
			out.write(var_txt_xnm_block8_number);
			out.write(newLine);
			this.writeCarbonPoolSizes_API(out,xnsimob);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line_five);
			out.write(newLine);
			out.write(var_txt_xnm_block9_title);
			out.write(newLine);
			out.write(newLine);
			out.write(var_txt_xnm_block9_txt1);
			out.write(newLine);
			out.write(var_txt_xnm_block9_txt2);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line_five);
			out.write(newLine);
			out.write(var_txt_xnm_block9_number);			
			out.write(newLine);
			this.writeNitrogenPoolSizes_API(out, xnsimob);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line_four);
			out.write(newLine);
			
			if ((xnsimob.getMainCrop()!=null && GenericTools.containsNum(Crop.cm_gecros,xnsimob.getMainCrop().getActID()))  || xnsimob.getMainCrop()==null) {
				buildBlock_GecrosParams(out, xnsimob);
			}
			
			out.write(newLine);
			out.write(var_txt_xnm_block9_txt3);		
			out.close();		
		}
	
		catch (Exception ex2) {
			System.err.println("Error in asciiFileMaker_XNM");
			ex2.printStackTrace();
		}

	}
	

	public void asciiFileMaker_XNM (String filename){
	
		String the_file = filename; 
		
			try{
			
			PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(the_file)));
			
			out.write(var_txt_general_MCK001_xnd_line);
			out.write(newLine);
			out.write(gfdb.getXnm_file_name()+ ".mod");
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line);
			out.write(newLine);
			out.write(var_txt_xnm_title);
			out.write(newLine);
			out.write(var_txt_xnm_block1_txt1);
			out.write(newLine);
			out.write(newLine);
			out.write(var_txt_xnm_block1_txt2);
			out.write(newLine);
			out.write(var_txt_xnm_block1_txt3);
			out.write(newLine);
			out.write(var_txt_xnm_block1_txt4);
			out.write(newLine);
			out.write(newLine);
			out.write(var_txt_xnm_block1_txt5);
			out.write(newLine);
			out.write(newLine);
			out.write(var_txt_xnm_block1_txt6);
			out.write(newLine);
			out.write(var_txt_xnm_block1_txt7);
			out.write(newLine);
			out.write(var_txt_xnm_block1_txt8);
			out.write(newLine);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line);
			out.write(newLine);
			out.write(var_txt_xnm_block2_title);
			out.write(newLine);
			out.write(var_txt_xnm_block2_txt1);
			out.write(newLine);
			out.write(var_txt_xnm_block2_txt2);
			out.write(newLine);
			out.write(var_txt_xnm_block2_txt3);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line);
			out.write(newLine);
			out.write(var_txt_xnm_block2_number);
			out.write(newLine);
			this.writeSoilPhysicalParam(out);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line);
			out.write(newLine);
			out.write(var_txt_xnm_block3_title);
			out.write(newLine);
			out.write(var_txt_xnm_block3_txt1);
			out.write(newLine);
			out.write(var_txt_xnm_block3_txt2);
			out.write(newLine);
			out.write(var_txt_xnm_block3_txt3);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line);
			out.write(newLine);
			out.write(var_txt_xnm_block3_number);
			out.write(newLine);
			this.writeSoilHydraulicCurveParam(out);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line);
			out.write(newLine);
			out.write(var_txt_xnm_block4_title);
			out.write(newLine);
			out.write(newLine);
			out.write(var_txt_xnm_block4_txt1);
			out.write(newLine);
			out.write(var_txt_xnm_block4_txt2);
			out.write(newLine);
			out.write(var_txt_xnm_block4_txt3);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line_five);
			out.write(newLine);
			out.write(var_txt_xnm_block4_number);
			out.write(newLine);
			
			out.write(var_txt_general_MCK001_xnd_line);
			out.write(newLine);
			out.write(newLine);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line);
			out.write(newLine);
			out.write(var_txt_xnm_block5_title);
			out.write(newLine);
			out.write(var_txt_xnm_block5_txt1);
			out.write(newLine);
			out.write(var_txt_xnm_block5_txt2);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line_five);
			out.write(newLine);
			out.write(var_txt_xnm_block5_number);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line);
			out.write(newLine);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line);
			out.write(newLine);
			out.write(var_txt_xnm_block6_title);
			out.write(newLine);
			out.write(newLine);
			out.write(var_txt_xnm_block6_txt1);
			out.write(newLine);
			out.write(var_txt_xnm_block6_txt2);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line_five);
			out.write(newLine);
			out.write(var_txt_xnm_block6_number);
			out.write(newLine);	
			this.writeStart_soil_values_C(out);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line);
			out.write(newLine);
			out.write(var_txt_xnm_block7_title);
			out.write(newLine);
			out.write(newLine);
			out.write(var_txt_xnm_block7_txt1);
			out.write(newLine);
			out.write(var_txt_xnm_block7_txt2);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line_six);
			out.write(newLine);
			out.write(var_txt_xnm_block7_number);
			out.write(newLine);
			this.writeRate_constant(out);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line);
			out.write(newLine);
			out.write(var_txt_xnm_block8_title);
			out.write(newLine);
			out.write(newLine);
			out.write(var_txt_xnm_block8_txt1);
			out.write(newLine);
			out.write(var_txt_xnm_block8_txt2);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line_five);
			out.write(newLine);
			out.write(var_txt_xnm_block8_number);
			out.write(newLine);
			this.writeCarbonPoolSizes(out);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line_five);
			out.write(newLine);
			out.write(var_txt_xnm_block9_title);
			out.write(newLine);
			out.write(newLine);
			out.write(var_txt_xnm_block9_txt1);
			out.write(newLine);
			out.write(var_txt_xnm_block9_txt2);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line_five);
			out.write(newLine);
			out.write(var_txt_xnm_block9_number);
			out.write(newLine);
			this.writeNitrogenPoolSizes(out);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line_four);
			out.write(newLine);
			out.write(var_txt_xnm_block9_txt3);
			
			out.close();
			
		}
	
		catch (Exception ex2) {
			System.err.println("Error in asciiFileMaker_XNM");
			ex2.printStackTrace();
		}

	}
	
	/**
	 * Section 10020
	 * @param out
	 * @param soiltypeid
	 */
	private void writeSoilPhysicalParam_API(PrintWriter out, int soiltypeid){
		
		try {
			ResultSet rs = gfdb.getSoilPhysicalResultSet(soiltypeid);
			int lastLayerOfPreviousHorizon = 0;
			while(rs.next()){		
				int numLayers = rs.getInt("number_simulation_layers");
				for (int cnt = lastLayerOfPreviousHorizon + 1; cnt <= lastLayerOfPreviousHorizon + numLayers; cnt++) {  
					//Here the Soil physical parameters
					out.write(Integer.toString( cnt));
					out.write(tab);
					out.write(String.valueOf( rs.getString("soil_type")));
					out.write(tab);
					out.write(Double.toString( rs.getDouble("theta_pF42")));
					out.write(tab);
					out.write(Double.toString( rs.getDouble("field_capacity")));
					out.write(tab);
					out.write(Double.toString( rs.getDouble("total_pore_volume")));
					out.write(tab);
					out.write(Double.toString( rs.getDouble("saturated_conductivity_kf")));
					out.write(newLine);
					
				}
				lastLayerOfPreviousHorizon += numLayers; 
			
			}
		} catch (SQLException e) {
			e.printStackTrace();
		}
	}
	
	/**
	 * Section 10020
	 * @param out
	 */
	private void writeSoilPhysicalParam(PrintWriter out){	
		try {
			ResultSet rs = gfdb.getSoilPhysicalResultSet(2);
			int lastLayerOfPreviousHorizon = 0;
			while(rs.next()){		
				int numLayers = rs.getInt("number_simulation_layers");
				for (int cnt = lastLayerOfPreviousHorizon + 1; cnt <= lastLayerOfPreviousHorizon + numLayers; cnt++) {  
					//Here the Soil physical parameters
					out.write(Integer.toString( cnt));
					out.write(tab);
					out.write(String.valueOf( rs.getString("soil_type")));
					out.write(tab);
					out.write(Double.toString( rs.getDouble("theta_pF42")));
					out.write(tab);
					out.write(Double.toString( rs.getDouble("field_capacity")));
					out.write(tab);
					out.write(Double.toString( rs.getDouble("total_pore_volume")));
					out.write(tab);
					out.write(Double.toString( rs.getDouble("saturated_conductivity_kf")));
					out.write(newLine);
					
				}
				lastLayerOfPreviousHorizon += numLayers; 
			
			}
		} catch (SQLException e) {
			e.printStackTrace();
		}
	}
	
	/**
	 * Section 10021
	 * @param out
	 * @param soiltypeid
	 */
	private void writeSoilHydraulicCurveParam_API(PrintWriter out, int soiltypeid){
		
		try {
			ResultSet rs = gfdb.getSoilHydraulicResultSet(soiltypeid);
			int lastLayerOfPreviousHorizon = 0;
			while(rs.next()){
				int numLayers = rs.getInt("number_simulation_layers");
				for (int cnt = lastLayerOfPreviousHorizon + 1; cnt <= lastLayerOfPreviousHorizon + numLayers; cnt++) {
				
				out.write(Integer.toString( cnt));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("theta_R")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("theta_S")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("campbell_a")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("campbell_b")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("van_genuchten_alpha")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("van_genuchten_n")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("van_genuchten_m")));
				out.write(newLine);

				}
				lastLayerOfPreviousHorizon += numLayers;

			}
		} catch (SQLException e) {
			e.printStackTrace();
		}
	}

	/**
	 * Section 10021
	 * @param out
	 */
	private void writeSoilHydraulicCurveParam(PrintWriter out){
		try {
			ResultSet rs = gfdb.getSoilHydraulicResultSet(2);
			int lastLayerOfPreviousHorizon = 0;
			while(rs.next()){
				int numLayers = rs.getInt("number_simulation_layers");
				for (int cnt = lastLayerOfPreviousHorizon + 1; cnt <= lastLayerOfPreviousHorizon + numLayers; cnt++) {

					out.write(Integer.toString( cnt));
					out.write(tab);
					out.write(Double.toString( rs.getDouble("theta_R")));
					out.write(tab);
					out.write(Double.toString( rs.getDouble("theta_S")));
					out.write(tab);
					out.write(Double.toString( rs.getDouble("campbell_a")));
					out.write(tab);
					out.write(Double.toString( rs.getDouble("campbell_b")));
					out.write(tab);
					out.write(Double.toString( rs.getDouble("van_genuchten_alpha")));
					out.write(tab);
					out.write(Double.toString( rs.getDouble("van_genuchten_n")));
					out.write(tab);
					out.write(Double.toString( rs.getDouble("van_genuchten_m")));
					out.write(newLine);
				}
				lastLayerOfPreviousHorizon += numLayers;
			}
		} catch (SQLException e) {
			e.printStackTrace();
		}
	}
	
	/**
	 * Section 20001
	 * Writes the mineral model parameters to the xnm-file
	 * This is preliminarily independent of soiltype!
	 * @param out handle on xnm file
	 * @param soiltype_id 
	 */
	private void writeMineral_mod_param_API(PrintWriter out, int soiltype_id) {
		if (soiltype_id == 0 || soiltype_id==1|| soiltype_id == 2|| soiltype_id==3|| soiltype_id==4 || soiltype_id==5|| soiltype_id==6|| soiltype_id==7|| soiltype_id==8 || soiltype_id==9  || soiltype_id==10  || soiltype_id==11   || soiltype_id==101 || soiltype_id==110  || soiltype_id==201 ) {
		//if (soiltype_id >= 0) {
			out.write("1");
			out.write(tab);
			out.write("0.45");
			out.write(tab);
			out.write("0.2");
			out.write(tab);
			out.write("10.0");
			out.write(tab);
			out.write("20.0");
			out.write(tab);
			out.write("2.0");
			out.write(tab);
			out.write("0.08");
			out.write(tab);
			out.write("0.40");
			out.write(tab);
			out.write("0.6");
			out.write(tab);
			out.write("25");
			out.write(newLine);
		} else {
			System.err.println("No mineralization model parameters for soiltype "+soiltype_id+"!\nInserted no values!");
		}
	}
	
   /**
    * Section 20002
    * implemented in 2012_03 by German to account for LEACHN
    * @param out
    * @param soilId
    */
	public void writeRate_constant_leachN(PrintWriter out, int soilId){
		
		try {
			ResultSet rs = gfdb.getrate_constant_leachn(soilId);
			int lastLayerOfPreviousHorizon = 0;
			while(rs.next()){
				int numLayers = rs.getInt("number_simulation_layers");
				for (int cnt = lastLayerOfPreviousHorizon + 1; cnt <= lastLayerOfPreviousHorizon + numLayers; cnt++) {
				
				out.write(Integer.toString( cnt));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("UreaHy")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("Nitrif")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("Denitrif")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("MinerLit")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("MinerMan")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("MinerHum")));
				out.write(newLine);
				
			}
			lastLayerOfPreviousHorizon += numLayers;	
			}
			
		} catch (SQLException e) {
			e.printStackTrace();
		}
	}
	
	/**
	 * Section 20003
	 * This method writes the start values for soil C and N contents into the xnm file using the values from simob
	 * It uses a geometric distribution for litter carbon and nitrogen and adds the surface C and N content to the first layer
	 * For manure C and N a geometric quotient of 0.5 is used, for litter 0.79 is used
	 * Humus C and N are distributed according to an empirical distribution (ca. 60% is put into the first 30 cm), if the soil has 14 layers. If not, a uniform distribution is used as fallback.
	 * @param out handle of the xnm file
	 * @param simob simulation object with all the data
	 */
	@SuppressWarnings("unused")
	private void writeStart_soil_values_C_API(PrintWriter out, XnSimObj simob){
		Date startdate = simob.getStartdate();
		SoilValues soilval = simob.getSoilValuesbyDate(startdate);
		if (soilval!=null) {System.out.println("Soilval vor run: littersoil: "+soilval.getC_litter_soil()+" n-litter-soil: "+soilval.getN_litter_soil()+ " no3+nh4: "+ (soilval.getNo3_total()+soilval.getNh4_total())+ " waterct: "+soilval.getWater_30()  );}
		try {
			if (soilval == null || useStaticSoilvalues) {
				System.out.println("Soilval von Database!");
				ResultSet rs = gfdb.getStart_soil_values_C((int)simob.getSoiltypeid());
				int lastLayerOfPreviousHorizon = 0;
				while(rs.next()){
					int numLayers = rs.getInt("number_simulation_layers");
					for (int cnt = lastLayerOfPreviousHorizon + 1; cnt <= lastLayerOfPreviousHorizon + numLayers; cnt++) {
						out.write(Integer.toString( cnt));
						out.write(tab);
						out.write(Double.toString( rs.getDouble("litter_c")));
						out.write(tab);
						out.write(Double.toString( rs.getDouble("litter_n")));
						out.write(tab);
						out.write(Double.toString( rs.getDouble("manure_c")));
						out.write(tab);
						out.write(Double.toString( rs.getDouble("manure_n")));
						out.write(tab);
						out.write(Double.toString( Math.max(1, rs.getDouble("humus_c"))));
						out.write(tab);
						out.write(Double.toString( Math.max(1, rs.getDouble("humus_n"))));
						out.write(newLine);
						System.out.print(" humC:"+rs.getDouble("humus_c"));
					}
					lastLayerOfPreviousHorizon += numLayers;
				}
				System.out.println("");
			} else { //here dynamic input of output values!
				int numlayers = gfdb.getsum_number_simulationlayers_sample((int)simob.getSoiltypeid());
				Double[] litter_c = distribLayersGeom(numlayers, getLitterCdistribCoef(simob.getSoiltypeid()), soilval.getC_litter_soil());
				Double[] litter_n = distribLayersGeom(numlayers, getLitterNdistribCoef(simob.getSoiltypeid()), soilval.getN_litter_soil());
				Double[] manure_c = distribLayersGeom(numlayers, 0.5, soilval.getC_manure_soil());
				Double[] manure_n = distribLayersGeom(numlayers, 0.5, soilval.getN_manure_soil());
				Double[] humdist = this.getHumusDist(simob.getSoiltypeid());
				if (humdist.length == 0) humdist = new Double[numlayers];
				Double[] humus_c = distribLayersCustom(humdist, soilval.getC_humus_soil());
				Double[] humus_n = distribLayersCustom(humdist, soilval.getN_humus_soil());
				litter_c[0] += soilval.getC_litter_surf();
				litter_n[0] += soilval.getN_litter_surf();
				for (int i=0 ; i< numlayers; i++) {
					out.write(Integer.toString( i+1));
					out.write(tab);
					out.write(dblform.format(litter_c[i]));
					out.write(tab);
					out.write(dblform.format(litter_n[i]));
					out.write(tab);
					out.write(dblform.format(manure_c[i]));
					out.write(tab);
					out.write(dblform.format(manure_n[i]));
					out.write(tab);
					out.write(dblform.format(Math.max(1,humus_c[i])));
					out.write(tab);
					out.write(dblform.format(Math.max(1, humus_n[i])));
					out.write(newLine);
				}			
			}
		} catch (SQLException e) {
			e.printStackTrace();
		}
	}
	
	/**
	 * Section 20003 non-API
	 * @param out
	 */
	private void writeStart_soil_values_C(PrintWriter out){

		try {
			ResultSet rs = gfdb.getStart_soil_values_C(2);
			int lastLayerOfPreviousHorizon = 0;
			while(rs.next()){
				int numLayers = rs.getInt("number_simulation_layers");
				for (int cnt = lastLayerOfPreviousHorizon + 1; cnt <= lastLayerOfPreviousHorizon + numLayers; cnt++) {

					out.write(Integer.toString( cnt));
					out.write(tab);
					out.write(Double.toString( rs.getDouble("litter_c")));
					out.write(tab);
					out.write(Double.toString( rs.getDouble("litter_n")));
					out.write(tab);
					out.write(Double.toString( rs.getDouble("manure_c")));
					out.write(tab);
					out.write(Double.toString( rs.getDouble("manure_n")));
					out.write(tab);
					out.write(Double.toString( rs.getDouble("humus_c")));
					out.write(tab);
					out.write(Double.toString( rs.getDouble("humus_n")));
					out.write(newLine);

				}
				lastLayerOfPreviousHorizon += numLayers;
			}

		} catch (SQLException e) {
			e.printStackTrace();
		}
	}

	/**
	 * Section 20007 API
	 * @param out
	 * @param soiltypeid
	 */
	private void writeRate_constant_API(PrintWriter out, int soiltypeid){	
		try {
			ResultSet rs = gfdb.getrate_constant(soiltypeid);
			int lastLayerOfPreviousHorizon = 0;
			while(rs.next()){
				int numLayers = rs.getInt("number_simulation_layers");
				for (int cnt = lastLayerOfPreviousHorizon + 1; cnt <= lastLayerOfPreviousHorizon + numLayers; cnt++) {
				
				out.write(Integer.toString( cnt));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("FOMSlow")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("FOMFast")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("FOMVeryFast")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("MicBiomSlow")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("MicBiomFast")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("HumusSlow")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("HumusFast")));
				out.write(newLine);
				
			}
			lastLayerOfPreviousHorizon += numLayers;	
			}
			
		} catch (SQLException e) {
			e.printStackTrace();
		}
	}	
	/**
	 * Section 20007 non-API
	 * @param out
	 */
	private void writeRate_constant(PrintWriter out){	
		try {
			ResultSet rs = gfdb.getrate_constant(2);
			int lastLayerOfPreviousHorizon = 0;
			while(rs.next()){
				int numLayers = rs.getInt("number_simulation_layers");
				for (int cnt = lastLayerOfPreviousHorizon + 1; cnt <= lastLayerOfPreviousHorizon + numLayers; cnt++) {
				
				out.write(Integer.toString( cnt));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("FOMSlow")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("FOMFast")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("FOMVeryFast")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("MicBiomSlow")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("MicBiomFast")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("HumusSlow")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("HumusFast")));
				out.write(newLine);		
			}
			lastLayerOfPreviousHorizon += numLayers;	
			}		
		} catch (SQLException e) {
			e.printStackTrace();
		}
	}
	
	
	/**
	 * 20008 API-Version
	 * This has to be adapted to dynamic modelling!!!
	 * @param out
	 * @param soiltypeid
	 */
	@SuppressWarnings("unused")
	private void writeCarbonPoolSizes_API(PrintWriter out, XnSimObj simob) {
		Date startdate = simob.getStartdate();
		SoilValues soilval = simob.getSoilValuesbyDate(startdate);
		int soiltypeid = (int)simob.getSoiltypeid();
		try {
			double AOM1sh = gfdb.getPoolShares("AOM1", 0);
			double AOM2sh = gfdb.getPoolShares("AOM2", 0);
			double AOM3sh = gfdb.getPoolShares("AOM3", 0);
			double BOM1sh = gfdb.getPoolShares("BOM1", 0);
			double BOM2sh = gfdb.getPoolShares("BOM2", 0);
			double BOMDsh = gfdb.getPoolShares("BOMD", 0);
			double SOM0sh = gfdb.getPoolShares("SOM0", 0);
			double SOM1sh = gfdb.getPoolShares("SOM1", 0);
			double SOM2sh = gfdb.getPoolShares("SOM2", 0);
			double Csolsh = gfdb.getPoolShares("Csol", 0);
	
			if (soilval==null || useStaticSoilvalues) {
				ResultSet rs = gfdb.getOrgCarbon(soiltypeid);

				int lastLayerOfPreviousHorizon = 0;
				while(rs.next()){
					double totalCarbonPerLayer = ((rs.getDouble("total_org_carbon") / 100 ) 
							* rs.getDouble("bulk_density") * 1000 * rs.getDouble("Layer_thickness") * 100);
					int numLayers = rs.getInt("number_simulation_layers");
					for (int cnt = lastLayerOfPreviousHorizon + 1; cnt <= lastLayerOfPreviousHorizon + numLayers; cnt++) {  

						double  AOM1 = AOM1sh * totalCarbonPerLayer;
						double  AOM2 = AOM2sh * totalCarbonPerLayer;
						double  AOM3 = AOM3sh * totalCarbonPerLayer;
						double  BOM1 = BOM1sh * totalCarbonPerLayer;
						double  BOM2 = BOM2sh * totalCarbonPerLayer;
						double  BOMD = BOMDsh * totalCarbonPerLayer;
						double  SOM0 = SOM0sh * totalCarbonPerLayer;
						double  SOM1 = SOM1sh * totalCarbonPerLayer;
						double  SOM2 = SOM2sh * totalCarbonPerLayer;
						double  Csol = Csolsh * totalCarbonPerLayer;

						out.write(Integer.toString( cnt));
						out.write(tab);
						out.write(dblform.format(AOM1));
						out.write(tab);
						out.write(dblform.format(AOM2));
						out.write(tab);
						out.write(dblform.format(AOM3));
						out.write(tab);
						out.write(dblform.format(BOM1));
						out.write(tab);
						out.write(dblform.format(BOM2));
						out.write(tab);
						out.write(dblform.format(BOMD));
						out.write(tab);
						out.write(dblform.format(SOM0));
						out.write(tab);
						out.write(dblform.format(SOM1));
						out.write(tab);
						out.write(dblform.format(SOM2));
						out.write(tab);
						out.write(dblform.format(Csol));
						out.write(newLine);

					}
					lastLayerOfPreviousHorizon += numLayers; 
				}
			} else {
				// Berechnen aus den Shares, der Tiefenverteilung und dem Gesamtgehalt der letzten Simulation
				double totorgC = soilval.getC_humus_soil() + soilval.getC_microb();  //German laut P3
				System.out.println("Distributing: C_humus_soil: "+soilval.getC_humus_soil() + " C_microb: " + soilval.getC_microb());
				int numLayers = gfdb.getsum_number_simulationlayers_sample((int)simob.getSoiltypeid());
				
				//Double[] manure_c = distribLayersGeom(numlayers, 0.5, soilval.getC_manure_soil());		
				Double[] soil_c_distrib = getHumusDist(simob.getSoiltypeid());
				if (soil_c_distrib.length == 0 ) soil_c_distrib = new Double[numLayers];
				Double[] soil_c = distribLayersCustom(soil_c_distrib, totorgC);
				
				for (int l = 0; l < numLayers ; l++) {
					double  AOM1 = AOM1sh * soil_c[l];
					double  AOM2 = AOM2sh * soil_c[l];
					double  AOM3 = AOM3sh * soil_c[l];
					double  BOM1 = BOM1sh * soil_c[l];
					double  BOM2 = BOM2sh * soil_c[l];
					double  BOMD = BOMDsh * soil_c[l];
					double  SOM0 = SOM0sh * soil_c[l];
					double  SOM1 = SOM1sh * soil_c[l];
					double  SOM2 = SOM2sh * soil_c[l];
					double  Csol = Csolsh * soil_c[l];

					out.write(Integer.toString(l+1));
					out.write(tab);
					out.write(dblform.format(AOM1));
					out.write(tab);
					out.write(dblform.format(AOM2));
					out.write(tab);
					out.write(dblform.format(AOM3));
					out.write(tab);
					out.write(dblform.format(BOM1));
					out.write(tab);
					out.write(dblform.format(BOM2));
					out.write(tab);
					out.write(dblform.format(BOMD));
					out.write(tab);
					out.write(dblform.format(SOM0));
					out.write(tab);
					out.write(dblform.format(SOM1));
					out.write(tab);
					out.write(dblform.format(SOM2));
					out.write(tab);
					out.write(dblform.format(Csol));
					out.write(newLine);
				}			
			}		
			
		} catch (SQLException e) {
			e.printStackTrace();
		}
	}	
	/**
	 * 20008 non-API
	 * @param out
	 */
	private void writeCarbonPoolSizes (PrintWriter out) {
		try {
			ResultSet rs = gfdb.getOrgCarbon(2);
			double AOM1sh = gfdb.getPoolShares("AOM1", 0);
			double AOM2sh = gfdb.getPoolShares("AOM2", 0);
			double AOM3sh = gfdb.getPoolShares("AOM3", 0);
			double SOM0sh = gfdb.getPoolShares("SOM0", 0);
			double SOM1sh = gfdb.getPoolShares("SOM1", 0);
			double SOM2sh = gfdb.getPoolShares("SOM2", 0);
			double BOM1sh = gfdb.getPoolShares("BOM1", 0);
			double BOM2sh = gfdb.getPoolShares("BOM2", 0);
			double BOMDsh = gfdb.getPoolShares("BOMD", 0);
			double Csolsh = gfdb.getPoolShares("Csol", 0);
			
			int lastLayerOfPreviousHorizon = 0;
			while(rs.next()){
			   double totalCarbonPerLayer = ((rs.getDouble("total_org_carbon") / 100 ) 
					   * rs.getDouble("bulk_density") * 1000 *rs.getDouble("Layer_thickness") * 100);
			   int numLayers = rs.getInt("number_simulation_layers");
			   for (int cnt = lastLayerOfPreviousHorizon + 1; cnt <= 
				   lastLayerOfPreviousHorizon + numLayers; cnt++) {  
				
				   double  AOM1 = AOM1sh * totalCarbonPerLayer;
				   double  AOM2 = AOM2sh * totalCarbonPerLayer;
				   double  AOM3 = AOM3sh * totalCarbonPerLayer;
				   double  BOM1 = BOM1sh * totalCarbonPerLayer;
				   double  BOM2 = BOM2sh * totalCarbonPerLayer;
				   double  BOMD = BOMDsh * totalCarbonPerLayer;
				   double  SOM0 = SOM0sh * totalCarbonPerLayer;
				   double  SOM1 = SOM1sh * totalCarbonPerLayer;
				   double  SOM2 = SOM2sh * totalCarbonPerLayer;
				   double  Csol = Csolsh * totalCarbonPerLayer;
				  
					out.write(Integer.toString( cnt));
					out.write(tab);
					out.write(String.valueOf( AOM1));
					out.write(tab);
					out.write(String.valueOf( AOM2));
					out.write(tab);
					out.write(String.valueOf( AOM3));
					out.write(tab);
					out.write(String.valueOf(Math.round(BOM1*Math.pow(10,1))/Math.pow(10,1)));
					out.write(tab);
					out.write(String.valueOf(Math.round(BOM2*Math.pow(10,1))/Math.pow(10,1)));
					out.write(tab);
					out.write(String.valueOf( BOMD));
					out.write(tab);
					out.write(String.valueOf(Math.round(SOM0*Math.pow(10,1))/Math.pow(10,1)));
					out.write(tab);
					out.write(String.valueOf(Math.round(SOM1*Math.pow(10,1))/Math.pow(10,1)));
					out.write(tab);
					out.write(String.valueOf( SOM2));
					out.write(tab);
					out.write(String.valueOf( Csol));
					out.write(newLine);
					
				}
				lastLayerOfPreviousHorizon += numLayers; 
				   
			}
		} catch (SQLException e) {
			e.printStackTrace();
		}
	}
	
	/**
	 * 20009 API
	 * @param out
	 * @param soiltypeid
	 * toDo: dynamischer Input bei N
	 */
	@SuppressWarnings("unused")
	private void writeNitrogenPoolSizes_API(PrintWriter out, XnSimObj simob) {
		Date startdate = simob.getStartdate();
		SoilValues soilval = simob.getSoilValuesbyDate(startdate);
		int soiltypeid = (int)simob.getSoiltypeid();
		double AOM1sh = gfdb.getPoolShares("AOM1", 0);
		double AOM2sh = gfdb.getPoolShares("AOM2", 0);
		double AOM3sh = gfdb.getPoolShares("AOM3", 0);
		double SOM0sh = gfdb.getPoolShares("SOM0", 0);
		double SOM1sh = gfdb.getPoolShares("SOM1", 0);
		double SOM2sh = gfdb.getPoolShares("SOM2", 0);
		double BOM1sh = gfdb.getPoolShares("BOM1", 0);
		double BOM2sh = gfdb.getPoolShares("BOM2", 0);
		double BOMDsh = gfdb.getPoolShares("BOMD", 0);
		
		try {				
			if (soilval == null || useStaticSoilvalues) {
				ResultSet rs = gfdb.getOrgCarbonPlusC_N(soiltypeid);
				int lastLayerOfPreviousHorizon = 0;
				while(rs.next()){
					double totalCarbonPerLayer = ((rs.getDouble("total_org_carbon") / 100 ) 
							* rs.getDouble("bulk_density") * 1000 *rs.getDouble("Layer_thickness") * 100);
					double cn_ratio = rs.getDouble("C_N_ratio");
					int numLayers = rs.getInt("number_simulation_layers");
					for (int cnt = lastLayerOfPreviousHorizon + 1; cnt <= 
						lastLayerOfPreviousHorizon + numLayers; cnt++) {  

						double  AOM1 = AOM1sh * totalCarbonPerLayer * cn_ratio;
						double  AOM2 = AOM2sh * totalCarbonPerLayer * cn_ratio;
						double  AOM3 = AOM3sh * totalCarbonPerLayer * cn_ratio;
						double  BOM1 = BOM1sh * totalCarbonPerLayer * cn_ratio;
						double  BOM2 = BOM2sh * totalCarbonPerLayer * cn_ratio;
						double  BOMD = BOMDsh * totalCarbonPerLayer * cn_ratio;
						double  SOM0 = SOM0sh * totalCarbonPerLayer * cn_ratio;
						double  SOM1 = SOM1sh * totalCarbonPerLayer * cn_ratio;
						double  SOM2 = SOM2sh * totalCarbonPerLayer * cn_ratio;

						out.write(Integer.toString( cnt));
						out.write(tab);
						out.write(dblform.format(AOM1));
						out.write(tab);
						out.write(dblform.format(AOM2));
						out.write(tab);
						out.write(dblform.format(AOM3));
						out.write(tab);
						out.write(dblform.format(BOM1));
						out.write(tab);
						out.write(dblform.format(BOM2));
						out.write(tab);
						out.write(dblform.format(BOMD));
						out.write(tab);
						out.write(dblform.format(SOM0));
						out.write(tab);
						out.write(dblform.format(SOM1));
						out.write(tab);
						out.write(dblform.format(SOM2));
						out.write(newLine);

					}
					lastLayerOfPreviousHorizon += numLayers; 	   
				}
			} else { //dynamic using soilval
				double tot_org_n = soilval.getN_humus_soil() + soilval.getN_microb();
				int numLayers = gfdb.getsum_number_simulationlayers_sample((int)simob.getSoiltypeid());		
				Double[] soil_n_distrib = this.getHumusDist(simob.getSoiltypeid());
				if (soil_n_distrib.length==0) soil_n_distrib = new Double[numLayers];
				Double[] soil_n = this.distribLayersCustom(soil_n_distrib, tot_org_n);
				for (int ly = 0; ly < numLayers; ly++) {
					double  AOM1 = AOM1sh * soil_n[ly];
					double  AOM2 = AOM2sh * soil_n[ly];
					double  AOM3 = AOM3sh * soil_n[ly];
					double  BOM1 = BOM1sh * soil_n[ly];
					double  BOM2 = BOM2sh * soil_n[ly];
					double  BOMD = BOMDsh * soil_n[ly];
					double  SOM0 = SOM0sh * soil_n[ly];
					double  SOM1 = SOM1sh * soil_n[ly];
					double  SOM2 = SOM2sh * soil_n[ly];

					out.write(Integer.toString(ly+1));
					out.write(tab);
					out.write(dblform.format(AOM1));
					out.write(tab);
					out.write(dblform.format(AOM2));
					out.write(tab);
					out.write(dblform.format(AOM3));
					out.write(tab);
					out.write(dblform.format(BOM1));
					out.write(tab);
					out.write(dblform.format(BOM2));
					out.write(tab);
					out.write(dblform.format(BOMD));
					out.write(tab);
					out.write(dblform.format(SOM0));
					out.write(tab);
					out.write(dblform.format(SOM1));
					out.write(tab);
					out.write(dblform.format(SOM2));
					out.write(newLine);
				}			
			}
		} catch (SQLException e) {
			e.printStackTrace();
		}
	}
	
	/**
	 * 20009- non-API
	 * @param out
	 */
	private void writeNitrogenPoolSizes (PrintWriter out) {
		try {
			
			ResultSet rs = gfdb.getOrgCarbonPlusC_N(2);
			double AOM1sh = gfdb.getPoolShares("AOM1", 0);
			double AOM2sh = gfdb.getPoolShares("AOM2", 0);
			double AOM3sh = gfdb.getPoolShares("AOM3", 0);
			double SOM0sh = gfdb.getPoolShares("SOM0", 0);
			double SOM1sh = gfdb.getPoolShares("SOM1", 0);
			double SOM2sh = gfdb.getPoolShares("SOM2", 0);
			double BOM1sh = gfdb.getPoolShares("BOM1", 0);
			double BOM2sh = gfdb.getPoolShares("BOM2", 0);
			double BOMDsh = gfdb.getPoolShares("BOMD", 0);
			
			
			int lastLayerOfPreviousHorizon = 0;
			while(rs.next()){
			   double totalCarbonPerLayer = ((rs.getDouble("total_org_carbon") / 100 ) 
					   * rs.getDouble("bulk_density") * 1000 *rs.getDouble("Layer_thickness") * 100);
			   
			   double cn_ratio = rs.getDouble("C_N_ratio");
			   
			   int numLayers = rs.getInt("number_simulation_layers");
			   for (int cnt = lastLayerOfPreviousHorizon + 1; cnt <= 
				   lastLayerOfPreviousHorizon + numLayers; cnt++) {  
				
				   double  AOM1 = AOM1sh * totalCarbonPerLayer * cn_ratio;
				   double  AOM2 = AOM2sh * totalCarbonPerLayer * cn_ratio;
				   double  AOM3 = AOM3sh * totalCarbonPerLayer * cn_ratio;
				   double  BOM1 = BOM1sh * totalCarbonPerLayer * cn_ratio;
				   double  BOM2 = BOM2sh * totalCarbonPerLayer * cn_ratio;
				   double  BOMD = BOMDsh * totalCarbonPerLayer * cn_ratio;
				   double  SOM0 = SOM0sh * totalCarbonPerLayer * cn_ratio;
				   double  SOM1 = SOM1sh * totalCarbonPerLayer * cn_ratio;
				   double  SOM2 = SOM2sh * totalCarbonPerLayer * cn_ratio;
			   
					out.write(Integer.toString( cnt));
					out.write(tab);
					out.write(String.valueOf( AOM1));
					out.write(tab);
					out.write(String.valueOf( AOM2));
					out.write(tab);
					out.write(String.valueOf( AOM3));
					out.write(tab);
					out.write(String.valueOf(Math.round(BOM1*Math.pow(10,1))/Math.pow(10,1)));
					out.write(tab);
					out.write(String.valueOf(Math.round(BOM2*Math.pow(10,1))/Math.pow(10,1)));
					out.write(tab);
					out.write(String.valueOf( BOMD));
					out.write(tab);
					out.write(String.valueOf(Math.round(SOM0*Math.pow(10,1))/Math.pow(10,1)));
					out.write(tab);
					out.write(String.valueOf(Math.round(SOM1*Math.pow(10,1))/Math.pow(10,1)));
					out.write(tab);
					out.write(String.valueOf( SOM2));
					out.write(newLine);
			   
			   }
			   lastLayerOfPreviousHorizon += numLayers; 	   
			}
			
		} catch (SQLException e) {
			e.printStackTrace();
		}
	}
		
	/**
	 * This method inserts the text block with crop parameters for the GECROS model into the xnm-file. It uses the template from the data_cm folder. It switches hard coded between "xnmgec_de1.txt" for Soil 1 and "xnmgec_ec6.txt" for the rest.
	 * @param out print stream to be written to
	 * @param xnsimob Simulation object containing among others the soil type
	 */
	
	private void buildBlock_GecrosParams(PrintWriter out, XnSimObj xnsimob) {	  

		Class<? extends FileMaker> c = this.getClass();
		TextFile blockfile;
		String fname;
		ClassLoader cl = this.getClass().getClassLoader();
		int cropid;
		if (xnsimob.getMainCrop()!=null) {
			cropid = xnsimob.getMainCrop().getActID();
			if (xnsimob.getMainCrop().getActID() == 1) { 
				fname = "xnmgec-h_ww.txt";	
			}
			else if (xnsimob.getMainCrop().getActID() == 2) { 
				fname = "xnmgec_wb.txt";	
			}
			else if (xnsimob.getMainCrop().getActID() == 3) { 
				fname = "xnmgec-h_sb.txt";	
			}
			else if (xnsimob.getMainCrop().getActID() == 4) { 
				fname = "xnmgec-h_wr.txt";	// "xnmgec_Rapeseed.txt",  "xnmgec-h_wr.txt"
			}
			else if (xnsimob.getMainCrop().getActID() == 5){
				fname = "xnmgec-h_mz.txt";
			}
			else if (xnsimob.getMainCrop().getActID() == 6){
				fname = "xnmgec_Grain_Maize.txt";
			}
			else if (xnsimob.getMainCrop().getActID() == 7)
				fname = "xnmgec_Beet.txt";

			else if (xnsimob.getMainCrop().getActID() == 8){
				fname = "xnmgec_Potato.txt";
			}

			else {
				fname = "xnmgec_ec6.txt";

			}
		}
		else {
			fname = "xnmgec-h_ww.txt"; //Weizen
			cropid = 1;   //Weizen
		
		}
		
		String rawpath = c.getResource(c.getSimpleName()+".class").toString();
		
		if  (rawpath.contains(".jar")) {
			blockfile = new TextFile(cl.getResource("data_cm/"+fname).getFile());
		} else {
			Pattern pat = Pattern.compile(".*file:(.*/).*bin.*");
			Matcher m = pat.matcher(rawpath);
		    m.matches();
		    String path = m.group(1);
			blockfile = new TextFile(path+"data_cm/"+fname);
			
		}			
		String gecblock = blockfile.readString();
		//System.out.println("Growing: "+xnsimob.getMainCrop().getActID()+", but using for GECROS: "+fname);
		if (this.useChngGenetics) {
			System.out.println("changing Gen");
			UpdateGenetics genupdater = new UpdateGenetics();
			gecblock = genupdater.updateAllGenXNM(gecblock, xnsimob, cropid);
		}
		
		out.write(gecblock);
	}
	
	
	
	
	
//******************************FROM HERE STARTS THE WEATHER FILE**********************************************
	
	
	
	public void asciiFileMaker_XNW_API(XnSimObj xnsimob, WeatherDB wdb){
	
		long numdays = (xnsimob.getEnddate().getTime()-xnsimob.getStartdate().getTime())/(1000*60*60*24)+1;
		//System.out.println("Numdays: "+numdays);
		
		
		try{
			
			PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(xnsimob.getWorkingfolder()+"/weather.xnw")));

			out.write(var_txt_general_MCK001_xnd_line);
			out.write(newLine);
			out.write(var_txt__xnw_title);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line);
			out.write(newLine);
			out.write(var_txt_general_xnw_block1_txt1);
			out.write(newLine);
			out.write(var_txt_general_xnw_block1_txt2);
			out.write(newLine);
			out.write(var_txt_general_xnw_block1_txt3);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line_five);
			out.write(newLine);
			out.write(var_txt_xnw_block1_number);
			out.write(newLine);
			out.write(Long.toString(xnsimob.getId()));
			out.write(tab);
			out.write("48.412"); //Lattitude
			out.write(tab);
			out.write("9.497"); //Longitude
			out.write(tab);
			out.write("750"); //Altitude
			out.write(tab);
			out.write("-99"); //Avg Temp Station
			out.write(tab);
			out.write("-99"); //Temp Amplitude
			out.write(tab);
			out.write("-99"); //getStation_temp_measurement(2)
			out.write(tab);
			out.write("-99"); //getStation_wind_measurement(2)
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line_five);
			out.write(newLine);
			out.write(var_txt_general_xnw_block2_txt1);
			out.write(newLine);
			out.write(newLine);
			out.write (var_txt_xnw_block2_number);
			out.write(tab);
			
			//int no_simulation_days_per_station= gfdb.getnumber_simulation_days(2, second_year);
			out.write(String.valueOf(numdays));
			out.write(tab);
			out.write(var_txt_general_xnw_block2_txt2);
			out.write(newLine);
			this.writeWeatherDataAPI(out, xnsimob, wdb);
			
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line_two);
			out.write(newLine);
			out.write(var_txt_xnw_block3_text1);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line_two);
			out.write(newLine);
			out.write(var_txt_xnw_block3_text2);
			out.write(newLine);
			out.write(var_txt_xnw_block3_text3);
			out.write(newLine);
			out.write(var_txt_xnw_block3_text4);
			out.write(newLine);
			out.write(var_txt_xnw_block3_text5);
			out.write(newLine);
			out.write(var_txt_xnw_block3_text6);
			out.write(newLine);
			out.write(var_txt_xnw_block3_text7);
			out.write(newLine);
			out.write(var_txt_xnw_block3_text8);
			out.write(newLine);
			out.write(var_txt_xnw_block3_text9);
			out.write(newLine);
			out.write(var_txt_xnw_block3_text10);
			out.write(newLine);
			out.write(var_txt_xnw_block3_text11);
			out.write(newLine);
			out.write(var_txt_xnw_block3_text12);
			out.write(newLine);
			out.write(var_txt_xnw_block3_text13);
			out.write(newLine);
			out.write(var_txt_xnw_block3_text14);
			out.write(newLine);
			out.write(var_txt_xnw_block3_text15);
			out.write(newLine);
			out.write(var_txt_xnw_block3_text16);
			out.write(newLine);
			out.write(var_txt_xnw_block3_text17);
			out.write(newLine);
			out.write(var_txt_xnw_block3_text18);
			out.write(newLine);
			out.write(var_txt_xnw_block3_text19);
			out.write(newLine);
			out.write(var_txt_xnw_block3_text20);
			out.close();
				
		}
	
		catch (Exception ex2) {
			System.err.println("Error in asciiFileMaker_XNW_API");
			ex2.printStackTrace();
			System.exit(1);
		}

	}
	
	private void writeWeatherDataAPI(PrintWriter out, XnSimObj xnsim, WeatherDB weather){
		// CT 100409 adapted, made flexible
		GregorianCalendar cal = null;
		try {
			//ResultSet rs = gfdb.getWeatherResultSet_per_station(station, second_year);
			GregorianCalendar calptr = new GregorianCalendar();
			GregorianCalendar endcal = new GregorianCalendar();
			calptr.setTime(xnsim.getStartdate());
			cal=calptr;
			endcal.setTime(xnsim.getEnddate());
			double precoverfl = 0D;
			double precval;
			while(!calptr.after(endcal)) {
				//System.out.println(calptr.getTime());
				WeatherDD weatherd = weather.getWDaybyDate(calptr.getTime()).getAggregWeatherbyStationlist(xnsim.getStationlist());
				
				int day = calptr.get(Calendar.DAY_OF_MONTH);
				int mnth = calptr.get(Calendar.MONTH)+1;
				String m = Integer.toString(mnth);
				if (mnth<10){ m = "0"+m;}
				String d = Integer.toString(day);
				if (day<10) {d="0"+d; }
				String dm = d + m;
				
				//int yr = rs.getInt("year");
				//String y = Integer.toString(yr);//year
				//String y2 = y.substring(y.length()-2, y.length()); // year format
				
				//int yr = calptr.get(Calendar.YEAR);
				//String y2 = "01";
				//if (yr == second_year) {y2 = "02";}
				
				int d_y= calptr.get(Calendar.YEAR);
				String yr = Integer.toString(d_y);//year 
				yr = yr.substring(2);
				int dyr = calptr.get(Calendar.DAY_OF_YEAR);
				
				out.write(dm); //day_month
				out.write(tab);
				out.write(yr); //year
				out.write(tab);
				out.write(Integer.toString(dyr)); //Julian days
				out.write(tab);
				out.write(dblform.format(weatherd.getGlobalradiation()));
				out.write(tab);
				out.write(dblform.format( weatherd.getMaxairtemp() ));
				out.write(tab);
				out.write(dblform.format( weatherd.getMinairtemp()));
				out.write(tab);	
				precval = Math.min(weatherd.getPrecip()+precoverfl,maxp);
				out.write(dblform.format(precval));
				precoverfl = weatherd.getPrecip() + precoverfl - precval;
				out.write(tab);
				out.write(dblform.format( weatherd.getDewpoint())); //dewpoint
				out.write(tab);
				out.write(dblform.format(weatherd.getAvgwindspeed())); //avg wind speed
				out.write(tab);
				out.write(dblform.format(weatherd.getPhotosyntact())); //photosynthetic_activity
				out.write(tab);
				out.write(dblform.format(weatherd.getSunshineduration())); //sunshine duration
				out.write(tab);
				out.write(dblform.format( weatherd.getMeanairtemp()));
				out.write(tab);
				out.write(dblform.format( weatherd.getRelativehum())); 
				out.write(tab);
				out.write(dblform.format(weatherd.getSaturdeficitair())); //saturation_deficit_air
				out.write(tab);
				out.write(dblform.format(weatherd.getPanevaporation())); //pan evaporation
				out.write(tab);
				out.write(dblform.format(weatherd.getSnowdepth())); //snow depth
				out.write(tab);
				out.write(dblform.format(weatherd.getSoiltemp5cm())); //soil temp 5 cm
				out.write(tab);
				out.write(dblform.format(weatherd.getSoiltemp10cm())); //soil temp 10 cm
				out.write(tab);
				out.write(dblform.format(weatherd.getSoiltemp20cm())); //soil temp 20 cm
				out.write(newLine);
				calptr.add(Calendar.DAY_OF_YEAR, 1);
			}
		} catch (NullPointerException np) {
			np.printStackTrace();
			System.err.println("Write-Date: " + cal.getTime());
			System.err.println("WeatherDB-start:" +weather.getStartdate());
			System.err.println("WeatherDB-ende:" +weather.getEnddate());
			System.exit(1);
		} catch (Exception e) {
			e.printStackTrace();
		} 
	}
	
	
	
	
	
	public void asciiFileMaker_XNW(String filename, int station, int second_year){
//CT 100809 adapted
		//Here the config file entries are initialised
//		String folder = this.getDestinationFolder();
//		String the_file = folder + "/" + filename; 
		
		String the_file = filename; 
		
		//file specific text elements initialised
		
		
		try{
			
			PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(the_file)));


			out.write(var_txt_general_MCK001_xnd_line);
			out.write(newLine);
			out.write(var_txt__xnw_title);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line);
			out.write(newLine);
			out.write(var_txt_general_xnw_block1_txt1);
			out.write(newLine);
			out.write(var_txt_general_xnw_block1_txt2);
			out.write(newLine);
			out.write(var_txt_general_xnw_block1_txt3);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line_five);
			out.write(newLine);
			out.write(var_txt_xnw_block1_number);
			out.write(newLine);
			out.write(gfdb.getStation_name(2));
			out.write(tab);
			out.write(gfdb.getStation_latitude(2));
			out.write(tab);
			out.write(gfdb.getStation_longitude(2));
			out.write(tab);
			out.write(gfdb.getStation_altitude(2));
			out.write(tab);
			out.write(gfdb.getStation_avg_temperature(2));
			out.write(tab);
			out.write(gfdb.getStation_temp_amplitude(2));
			out.write(tab);
			out.write(gfdb.getStation_temp_measurement(2));
			out.write(tab);
			out.write(gfdb.getStation_wind_measurement(2));
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line_five);
			out.write(newLine);
			out.write(var_txt_general_xnw_block2_txt1);
			out.write(newLine);
			out.write(newLine);
			out.write (var_txt_xnw_block2_number);
			out.write(tab);
			
			int no_simulation_days_per_station= gfdb.getnumber_simulation_days(2, second_year);
			
			out.write(String.valueOf(no_simulation_days_per_station));
			out.write(tab);
			out.write(var_txt_general_xnw_block2_txt2);
			out.write(newLine);
			this.writeWeatherData(out, station, second_year);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line_two);
			out.write(newLine);
			out.write(var_txt_xnw_block3_text1);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line_two);
			out.write(newLine);
			out.write(var_txt_xnw_block3_text2);
			out.write(newLine);
			out.write(var_txt_xnw_block3_text3);
			out.write(newLine);
			out.write(var_txt_xnw_block3_text4);
			out.write(newLine);
			out.write(var_txt_xnw_block3_text5);
			out.write(newLine);
			out.write(var_txt_xnw_block3_text6);
			out.write(newLine);
			out.write(var_txt_xnw_block3_text7);
			out.write(newLine);
			out.write(var_txt_xnw_block3_text8);
			out.write(newLine);
			out.write(var_txt_xnw_block3_text9);
			out.write(newLine);
			out.write(var_txt_xnw_block3_text10);
			out.write(newLine);
			out.write(var_txt_xnw_block3_text11);
			out.write(newLine);
			out.write(var_txt_xnw_block3_text12);
			out.write(newLine);
			out.write(var_txt_xnw_block3_text13);
			out.write(newLine);
			out.write(var_txt_xnw_block3_text14);
			out.write(newLine);
			out.write(var_txt_xnw_block3_text15);
			out.write(newLine);
			out.write(var_txt_xnw_block3_text16);
			out.write(newLine);
			out.write(var_txt_xnw_block3_text17);
			out.write(newLine);
			out.write(var_txt_xnw_block3_text18);
			out.write(newLine);
			out.write(var_txt_xnw_block3_text19);
			out.write(newLine);
			out.write(var_txt_xnw_block3_text20);
			out.close();
				
		}
	
		catch (Exception ex2) {
			System.out.println("Error in asciiFileMaker_XNW");
			ex2.printStackTrace();
		}

	}
	
	private void writeWeatherData(PrintWriter out, int station, int second_year){
		// CT 100409 adapted, made flexible
		try {
			ResultSet rs = gfdb.getWeatherResultSet_per_station(station, second_year);
			
			while(rs.next()){
				int day = rs.getInt("day");
				int mnth = rs.getInt("month");
				String m = Integer.toString(mnth);
				if (mnth<10){ m = "0"+m;}
				String dm = Integer.toString(day) + m;
				
				//int yr = rs.getInt("year");
				//String y = Integer.toString(yr);//year
				//String y2 = y.substring(y.length()-2, y.length()); // year format
				
				int yr = rs.getInt("year");
				String y2 = "01";
				if (yr == second_year) {y2 = "02";}
				
				int d_y= rs.getInt("day_year");
				String dy = Integer.toString(d_y);//year 
				
				out.write(dm); //day_month
				out.write(tab);
				out.write(y2); //year
				out.write(tab);
				out.write(dy); //Julian days
				out.write(tab);
				out.write(Double.toString( rs.getDouble("global_radiation")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("maximum_temperature")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("minimum_temperature")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("precipitation")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("dewpoint")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("avg_wind_speed")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("photosynthetic_activity")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("sunshine_duration")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("avg_temp")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("relative_humidity"))); 
				out.write(tab);
				out.write(Double.toString( rs.getDouble("saturation_deficit_air")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("pan_evaporation")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("snow_depth")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("soil_temp_5_cm")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("soil_temp_10_cm")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("soil_temp_20_cm")));
				out.write(newLine);
				
			}
		} catch (SQLException e) {
			e.printStackTrace();
		}
	}
		
//******************************FROM HERE STARTS THE XND FILE**************************************************	
	
	public void asciiFileMaker_XND_API(XnSimObj xnsim, boolean succesful){
		try {
		
				//String the_file =xncfg.getXnworksp() + "/sob"+xnsim.getId().toString()  + ".xnd"; 
				String the_file =xnsim.getWorkingfolder() + "/simob.xnd";
				
				PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(the_file)));
	
				//int crop_id	=	gfdb.getPak_crops_crop_id_by_ktblmilp_process_id(rs.getInt("process_id"));
				//int crop_id=xnsim.getCropid();
				//System.out.println("CropID: " + crop_id);
				this.buildBlock_simulation_data_API(out,xnsim);
				this.buildBlock_farm_data_API(out);
				this.buildBlock_plot_data_API(out);
				//this.buildBlock_subplot_data(out, rs.getString("filename"), crop_id, rs.getInt("weather_id"), rs.getInt("soil_id"), rs.getString("configuration_name"));
				if ((xnsim.getMainCrop()!=null && GenericTools.containsNum(Crop.cm_gecros, xnsim.getMainCrop().getActID())) || xnsim.getMainCrop()==null) {
					if (useLEACHN) {
						this.buildBlock_subplot_data_API(out, "Gecros_leachn.xnc", xnsim);
					} else {
						this.buildBlock_subplot_data_API(out, "Gecros_daisy.xnc", xnsim);
					}
					if (xnsim.getNumSchedule()>1) {
						System.err.println("Warning: More than one crop to model. Don't know which Model to use, using GECROS anyway!");
					}
				} else {
					if (useLEACHN) {
						this.buildBlock_subplot_data_API(out, "Ceres_leachn.xnc", xnsim);
					} else {
						this.buildBlock_subplot_data_API(out, "Ceres_daisy.xnc", xnsim);
					}
				}
				

				//this.buildBlock_subplot_data_API(out, "Gecros_real.xnc", xnsim);
				this.buildBlock_preceding_crop_API(out, xnsim);
				this.buildBlock_cultivated_crop_API (out, xnsim, succesful);
				this.buildBlock_mineral_fert_API(out, xnsim);
				this.buildBlock_organic_fert_API(out, xnsim);
				this.buildBlock_soil_management_API(out, xnsim);
				this.buildBlock_irrigation(out);  //no Irrigation
				this.buildBlock_soilprofiles(out, xnsim.getSoiltypeid());
				this.buildBlock_sample_soilprofiles_API(out, xnsim);
				this.buildBlock_water_readings_API(out);
				this.buildBlock_nitrogen_readings_API(out);
				this.buildBlock_plant_measurementsI_API(out);
				this.buildBlock_plant_measurementsII_API(out);
				this.buildBlock_closing_block(out);
				
			
		}catch (Exception ex2) {
				System.err.println("Error in asciiFileMaker_XND_API");
				ex2.printStackTrace();
		}
		
		
	}
	
	
	public void asciiFileMaker_XND(String directoryname){

		//Here the config file entries are initialised
//		String folder = this.getDestinationFolder();
//		String the_file = folder + "/" + filename; 
		
		try {
			ResultSet rs = gfdb.get_list_of_xnd_files_to_create();
			int cnt = 0;
			while (rs.next()){
				cnt = cnt + 1;
		
				String the_file =directoryname + rs.getString("filename") + ".xnd"; 
			
			//file specific text elements initialized			
				PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(the_file)));
	
				int crop_id	=	gfdb.getPak_crops_crop_id_by_ktblmilp_process_id(rs.getInt("process_id"));
				
				
				this.buildBlock_simulation_data(out);
				this.buildBlock_farm_data(out);
				this.buildBlock_plot_data(out);
				this.buildBlock_subplot_data(out, rs.getString("filename"), crop_id, rs.getInt("weather_id"), rs.getInt("soil_id"), rs.getString("configuration_name"));
				this.buildBlock_preceding_crop(out);
				this.buildBlock_cultivated_crop (out, crop_id, rs.getInt("process_id"));
				this.buildBlock_mineral_fert(out, rs.getInt("process_id"));
				this.buildBlock_organic_fert(out, rs.getInt("process_id"));
				this.buildBlock_soil_management(out, rs.getInt("process_id"));
				this.buildBlock_irrigation(out);
				this.buildBlock_soilprofiles(out, rs.getInt("soil_id"));
				//this.buildBlock_sample_soilprofiles(out, new Date(2001,8,2), rs.getInt("soil_id"));  //hard gecodet wie German
				//missing!!
				this.buildBlock_water_readings(out);
				this.buildBlock_nitrogen_readings(out);
				this.buildBlock_plant_measurementsI(out);
				this.buildBlock_plant_measurementsII(out);
				this.buildBlock_closing_block(out);
				
			}
		}catch (Exception ex2) {
				System.out.println("Error in asciiFileMaker_XND");
				ex2.printStackTrace();
		}
		
	}
	
	/**
	 * Section 10000
	 * @param out
	 * @param xnsim
	 */
	private void buildBlock_simulation_data_API(PrintWriter out, XnSimObj xnsim){
		try {
			buildBlock_header_simulation_data (out);
			//out.write(gfdb.getStartDate3(1001));
			out.write(xndateform.format(xnsim.getStartdate()));
			//System.out.println("Startdate: "+xndateform.format(xnsim.getStartdate()));
			out.write(tab);
			//out.write(gfdb.getEndDate3(1001));
			GregorianCalendar cal = new GregorianCalendar();
			cal.setTime(xnsim.getEnddate());
			cal.add(Calendar.DATE, 1);
			out.write(xndateform.format(cal.getTime()));
			out.write(newLine);
			out.write(newLine);


		} catch (Exception e) {
			System.err.println("Error in buildBlock_simulation_data" + e);
			e.printStackTrace();
		}
	}
	
	
	
	
	private void buildBlock_simulation_data(PrintWriter out){
		try {
			buildBlock_header_simulation_data (out);
			out.write(gfdb.getStartDate3(1001));
			out.write(tab);
			out.write(gfdb.getEndDate3(1001));
			out.write(newLine);
			out.write(newLine);


		} catch (Exception e) {
			System.err.println("Error in buildBlock_simulation_data" + e);
		}
	}

	private void buildBlock_header_simulation_data(PrintWriter out){
		try {
			out.write(var_txt_general_MCK001_xnd_line);
			out.write(newLine);
			out.write(var_txt_MCK001_xnd_title);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block1_title);
			out.write(newLine);
			out.write(var_txt_MCK001_xnd_block1_number);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block1_txt1);
			out.write(newLine);
		
		} catch (Exception e) {
			System.err.println("Error in buildBlock_simulation_data" + e);
		}
	}
	
	private void buildBlock_farm_data_API(PrintWriter out){
		try {
			buildBlock_header_farm_data (out);
			out.write("1"); //FarmID
			out.write(tab);
			out.write("1"); //No Plots
			out.write(tab);
			out.write("48"); //Lattitude
			out.write(tab);
			out.write("9"); //Longitude
			out.write(tab);
			out.write("750"); //Altitude
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block2_txt2);
			out.write(newLine);
			out.write("Masterfarmer"); //Last name
			out.write(tab);
			out.write("German"); //First name
			out.write(tab);
			out.write("Mittlere Kuppenalb"); //Adress
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block2_txt3);
			out.write(newLine);
			out.write("70000"); //PLZ
			out.write(tab);
			out.write("Suppingen"); //City
			out.write(tab);
			out.write("07321"); //Tel
			out.write(tab);
			out.write("073213"); //Fax
			out.write(newLine);
		
		
	} catch (Exception e) {
		System.err.println("Error in buildBlock_farm_data_API" + e);
		e.printStackTrace();
	}
}
	
	
	private void buildBlock_farm_data(PrintWriter out){
		try {
			buildBlock_header_farm_data (out);
			out.write(gfdb.getFarmId());
			out.write(tab);
			out.write(gfdb.getNoPlots());
			out.write(tab);
			out.write(gfdb.getLatitude());
			out.write(tab);
			out.write(gfdb.getLongitude());
			out.write(tab);
			out.write(gfdb.getAltitude());
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block2_txt2);
			out.write(newLine);
			out.write(gfdb.getlast_name());
			out.write(tab);
			out.write(gfdb.getFirst_name());
			out.write(tab);
			out.write(gfdb.getAddress());
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block2_txt3);
			out.write(newLine);
			out.write(gfdb.getPlz());
			out.write(tab);
			out.write(gfdb.getcity());
			out.write(tab);
			out.write(gfdb.getTel());
			out.write(tab);
			out.write(gfdb.getFax());
			out.write(newLine);
		
		
	} catch (Exception e) {
		System.out.println("Error in buildBlock_farm_data" + e);
	}
}
	
	private void buildBlock_header_farm_data(PrintWriter out){
		try { 
			out.write(var_txt_general_MCK001_xnd_line);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block2_title);
			out.write(newLine);
			out.write(var_txt_MCK001_xnd_block2_number);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block2_txt1);
			out.write(newLine);
			
		} catch (Exception e) {
			System.out.println("Error in buildBlock_farm_data" + e);
		}
	}

	private void buildBlock_plot_data_API (PrintWriter out){
		try {
		buildBlock_header_plot_data (out);
		out.write(var_txt_MCK001_xnd_block3_number);
		out.write(newLine);
		out.write(var_txt_general_MCK001_xnd_block3_txt1);
		out.write(newLine);
		out.write("1"); //PlotID
		out.write(tab);
		out.write("Masterfeld_fuer_alles"); //Plot name
		out.write(tab);
		out.write("1"); //Plot size
		out.write(tab);
		out.write("1"); //Num Subplots
		out.write(newLine);
		out.write(var_txt_general_MCK001_xnd_line);
		out.write(newLine);
		out.write(var_txt_general_MCK001_xnd_line);
		out.write(newLine);
		} catch (Exception e) {
			System.err.println("Error in buildBlock_plot_data_API " + e);
			e.printStackTrace();
		}
	}
	
	
	private void buildBlock_plot_data (PrintWriter out){
		try {
		buildBlock_header_plot_data (out);
		out.write(var_txt_MCK001_xnd_block3_number);
		out.write(newLine);
		out.write(var_txt_general_MCK001_xnd_block3_txt1);
		out.write(newLine);
		out.write(gfdb.getPlotID());
		out.write(tab);
		out.write(gfdb.getPlot_name());
		out.write(tab);
		out.write(gfdb.getPlot_size());
		out.write(tab);
		out.write(gfdb.getnum_subplots());
		out.write(newLine);
		out.write(var_txt_general_MCK001_xnd_line);
		out.write(newLine);
		out.write(var_txt_general_MCK001_xnd_line);
		out.write(newLine);
		} catch (Exception e) {
			System.out.println("Error in buildBlock_plot_data " + e);
		}
	}
	
	private void buildBlock_header_plot_data(PrintWriter out){
		try {
			out.write(var_txt_general_MCK001_xnd_line);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block3_title);
			out.write(newLine);
		} catch (Exception e) {
			System.out.println("Error in buildBlock_plot_data " + e);
		}
	}

	
	private void buildBlock_subplot_data_API (PrintWriter out, String configname, XnSimObj xnsimob){
		try {		
			buildBlock_header_subplot_data(out);
			out.write(var_txt_MCK001_xnd_block4_number);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block4_txt1);
			out.write(newLine);
			out.write("1"); //Subplot ID
			out.write(tab);
			out.write("SP"); //Subplotname
			out.write(tab);
			out.write("Crop"); //Principal Crop german string
			out.write(tab);
			out.write("weather.xnw"); //Weather file
			out.write(tab);
			out.write("soilparam.xnm"); //Soil file
			out.write(tab);
			out.write(configname);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block4_txt2);
			out.write(newLine);
			out.write("1"); //Area Number
			out.write(tab);
			out.write("1"); //Subplot size
			out.write(tab);
			out.write("0"); //Subplot slope
			out.write(tab);
			out.write("S");  //Subplot exposition
			out.write(newLine);
		} catch (Exception e) {
			System.err.println("Error in buildBlock_subplot_data_API " + e);
			e.printStackTrace();
		}
	}
	
	private void buildBlock_subplot_data (PrintWriter out, String subpl_name, int crop_id, int weather_id, int soil_id, String configname ){
		try {		
			buildBlock_header_subplot_data(out);
			out.write(var_txt_MCK001_xnd_block4_number);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block4_txt1);
			out.write(newLine);
			out.write(gfdb.getSubPlotID(4));
			out.write(tab);
			out.write(gfdb.getSubplot_name(subpl_name));
			out.write(tab);
			out.write(gfdb.getPrinc_Crop(crop_id));
			out.write(tab);
			///out.write(gfdb.getWeatherfile(weather_id)+ ".xnw");
			out.write("weather.xnw");
			out.write(tab);
			//out.write(gfdb.getParameterfile(soil_id) + ".xnm");
			out.write("soilparam.xnm");
			out.write(tab);
			out.write(configname);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block4_txt2);
			out.write(newLine);
			out.write(gfdb.getAreaNum(4));
			out.write(tab);
			out.write(gfdb.getsubplotsize(4));
			out.write(tab);
			out.write(gfdb.getsubplotslope(4));
			out.write(tab);
			out.write(gfdb.getsubplotExpo(4));
			out.write(newLine);
		} catch (Exception e) {
			System.out.println("Error in buildBlock_subplot_data " + e);
		}
	}
	
	private void buildBlock_header_subplot_data(PrintWriter out){
		try {
			out.write(var_txt_general_MCK001_xnd_block4_title);
			out.write(newLine);
		} catch (Exception e) {
			System.out.println("Error in buildBlock_subplot_data " + e);
		}
	}

	private void buildBlock_preceding_crop_API (PrintWriter out, XnSimObj xnsim){
		try {
			buildBlock_header_preceding_crop (out);
			out.write(var_txt_MCK001_xnd_block5_number);
			out.write(newLine);
			out.write("-99");  //getcrop_preceding 1001
			out.write(tab);
			out.write("-99"); //getyield_preceding
			out.write(tab);
			out.write("-99"); // getharvestdate_preceding
			out.write(tab);
			out.write("-99"); //getaboveground_preceding
			out.write(tab);
			out.write("-99"); //get c n aboveground preceding
			out.write(tab);
			out.write("-99"); //get root residues weight preceding
			out.write(tab);
			out.write("-99"); // get cn root residues preceding
			out.write(newLine);
			out.write(newLine);
		} catch (Exception e) {
			System.err.println("Error in buildBlock_preceding_crop_API " + e);
			e.printStackTrace();
		}
	}
	
	
	private void buildBlock_preceding_crop (PrintWriter out){
		try {
			buildBlock_header_preceding_crop (out);
			out.write(var_txt_MCK001_xnd_block5_number);
			out.write(newLine);
			out.write(gfdb.getcrop_preceding(1001));
			out.write(tab);
			out.write(gfdb.getyield_preceding(1001));
			out.write(tab);
			out.write(gfdb.getharvestDate_preceding(1001));
			out.write(tab);
			out.write(gfdb.getamount_aboveground_residues_preceding(1001));
			out.write(tab);
			out.write(gfdb.get_c_n_aboveground_residues_preceding(1001));
			out.write(tab);
			out.write(gfdb.getroot_residues_weight_preceding(1001));
			out.write(tab);
			out.write(gfdb.get_c_n_root_residues_preceding(1001));
			out.write(newLine);
			out.write(newLine);
		} catch (Exception e) {
			System.out.println("Error in buildBlock_preceding_crop " + e);
		}
	}

	private void buildBlock_header_preceding_crop(PrintWriter out){
		try {
			
		
		out.write(var_txt_general_MCK001_xnd_line);
		out.write(newLine);
		out.write(var_txt_general_MCK001_xnd_line);
		out.write(newLine);
		out.write(var_txt_general_MCK001_xnd_block5_title);
		out.write(newLine);
		out.write(var_txt_general_MCK001_xnd_block5_txt1);
		out.write(newLine);
		out.write(newLine);
		out.write(var_txt_general_MCK001_xnd_block5_txt2);
		out.write(newLine);
		out.write(tab);
		out.write(tab);
		out.write(tab);
		out.write(var_txt_general_MCK001_xnd_block5_txt3);
		out.write(newLine);
		out.write(tab);
		out.write(tab);
		out.write(var_txt_general_MCK001_xnd_block5_txt4);
		out.write(newLine);
		} catch (Exception e) {
			System.out.println("Error in buildBlock_header_preceding_crop " + e);
		}
	}
	
	/**
	 * This method creates the section where the crop is defined
	 * 10005
	 * @param out
	 * @param xnsim
	 */
	private void buildBlock_cultivated_crop_API (PrintWriter out, XnSimObj xnsim, boolean successful){
		//System.out.println(crop_id);
		SingleCropData cropdata;
		Crop cropp = null;
	
		try {
			buildBlock_header_cultivated_crop(out);
			out.write(var_txt_MCK001_xnd_block6_number);
			out.write(tab);
			int no_cultivated_crops = xnsim.getCropschedule().size();
 			if (no_cultivated_crops == 0){
				out.write("1");
				out.write(tab);
				out.write(var_txt_general_MCK001_xnd_block6_txt5);
				out.write(newLine);
				GregorianCalendar cal = new GregorianCalendar();
				cal.setTime(xnsim.getEnddate());
				String seeddate = xndateform.format(cal.getTime());
				String harvestdate = xndateform.format(cal.getTime());
				
				out.write("1");
				out.write(tab);
				out.write("WH");
				out.write(tab);
				out.write("Winter");
				out.write(tab);
				out.write("0.03");
				out.write(tab);
				out.write("0.12");
				out.write(tab);
				out.write("100");
				out.write(tab);
				out.write(seeddate);
				out.write(tab);
				out.write("-99");
				out.write(tab);
				out.write(harvestdate);
				out.write(tab);
				out.write("-99");
				out.write(tab);
				out.write("-99");
				out.write(tab);
				out.write("1.5");
				out.write(tab);
				out.write("0");
				out.write(newLine);
				
			} else {															
			int idofmaincrop = 0;
			out.write(String.valueOf(no_cultivated_crops));
			out.write(tab);
			out.write(var_txt_general_MCK001_xnd_block6_txt5);
			out.write(newLine);
			GregorianCalendar endcal = new GregorianCalendar();
			endcal.setTime(xnsim.getStartdate());
			for (int i=0; i < xnsim.getCropschedule().size(); i++) {
				cropp = (Crop)xnsim.getCropschedule().get(i);
				CropData cropdatainst = new CropData();
				cropdata = cropdatainst.getCrop(cropp.getActID()); 
				
				//System.out.println("CropID "+cropp.getActID());		
				GregorianCalendar seedcal = new GregorianCalendar();
				if (cropp.getActions().getActionbyId(Action.Plant).getStatus()==Action.SUCCESFUL) {
					seedcal.setTime(cropp.getActions().getActionbyId(Action.Plant).getDate());	
				} else {
					seedcal.setTime(xnsim.getEnddate());
					seedcal.add(Calendar.DATE, 1);
				}	
				if (endcal.after(seedcal)) {
					seedcal.setTime(endcal.getTime());
				}
				String seeddate = xndateform.format(seedcal.getTime());
				//System.out.println(seeddate);

				String harvestdate;
				
				if (cropp.getActions().getActionbyId(Action.Harvest).getStatus()==Action.SUCCESFUL || cropp.getActions().getActionbyId(Action.Harvest).getStatus()==Action.FAILED) {
					endcal.setTime(cropp.getActions().getActionbyId(Action.Harvest).getDate());
					endcal.add(Calendar.DATE, 2);
				} else {
					endcal.setTime(xnsim.getEnddate());	 
					endcal.add(Calendar.DATE, 2);
				}
				
				harvestdate = xndateform.format(endcal.getTime());
				idofmaincrop++;
				out.write(String.valueOf(idofmaincrop));
				out.write(tab);
				//out.write(mcchar.getString("xn_code"));
				out.write(cropdata.xncode);
				out.write(tab);
				out.write(cropdata.varietyname);
				out.write(tab);
				out.write(Float.toString(cropdata.sowingdepth));
				out.write(tab);
				out.write(String.valueOf(cropdata.rowspace));
				out.write(tab);
				out.write(String.valueOf(cropdata.seeddensity));
				out.write(tab);
				out.write(seeddate);
				out.write(tab);
				out.write("-99"); //Emergence date
				out.write(tab);
				out.write(harvestdate);
				out.write(tab);
				out.write("-99");//Date max. Biomass
				out.write(tab);
				out.write("-99");//Date max. Root
				out.write(tab);
				out.write(String.valueOf(cropdata.rootdepth));
				out.write(tab);
				if (successful) {
					out.write(String.valueOf(cropdata.removal));
				} else {
					out.write(String.valueOf(0));
				}
				out.write(newLine);
				
			}
			}

			
			out.write(newLine);
			} catch (Exception e) {
			System.err.println("Error in buildBlock_cultivated_crop_API " + e);
			System.err.println("ActivityID: "+cropp.getActID());
			e.printStackTrace();
		}
	}
		
	
	/**
	 * 10005
	 * @param out
	 * @param crop_id
	 * @param process_id
	 */
	private void buildBlock_cultivated_crop (PrintWriter out, int crop_id, int process_id){
		//System.out.println(crop_id);
		try {
			buildBlock_header_cultivated_crop(out);
			out.write(var_txt_MCK001_xnd_block6_number);
			out.write(tab);
			int no_cultivated_crops = 1;
			int idofmaincrop = 1;
			//check if we have an intermediate crop
			ResultSet intermediate_crop = gfdb.get_intermediate_crop( process_id);
			if (intermediate_crop.next()) {
				//crop_characteristics
				ResultSet icchar = gfdb.get_intermediate_crop_characteristics_by_goods_id(intermediate_crop.getInt("goods_id"));
				icchar.first();
				//seeddate
				intermediate_crop.getInt("xn_day");
				int day = intermediate_crop.getInt("xn_day");
				String d = Integer.toString(day) ;
				if (day<10){ d = "0"+d;}
				int mnth = intermediate_crop.getInt("xn_month");
				String m = Integer.toString(mnth);
				if (mnth<10){ m = "0"+m;}
				int yr = intermediate_crop.getInt("xn_year");
				String y = Integer.toString(yr);//year
				if (yr<10){ y = "0"+y;}
				String icseeddate =  d+m+y;
				
				//iccutdate
				ResultSet iccutset = gfdb.get_intermediate_crop_harvest(process_id);
				iccutset.first();
				iccutset.getInt("xn_day");
				day = iccutset.getInt("xn_day");
				d = Integer.toString(day) ;
				if (day<10){ d = "0"+d;}
				mnth = iccutset.getInt("xn_month");
				m = Integer.toString(mnth);
				if (mnth<10){ m = "0"+m;}
				yr = iccutset.getInt("xn_year");
				y = Integer.toString(yr);//year
				if (yr<10){ y = "0"+y;}
				String iccutdate =  d+m+y;
				
				idofmaincrop = 2;
				//write number of cultivated crops
				no_cultivated_crops = 2;
				out.write(String.valueOf(no_cultivated_crops));
				out.write(tab);
				out.write(var_txt_general_MCK001_xnd_block6_txt5);
				out.write(newLine);
				
				//write out characteristics of intermediate crop
				out.write("1");
				out.write(tab);
				out.write(icchar.getString("xn_code"));
				out.write(tab);
				out.write(icchar.getString("variety"));
				out.write(tab);
				out.write(String.valueOf(icchar.getDouble("seeding_depth")));
				out.write(tab);
				out.write(String.valueOf(icchar.getDouble("row_spacing")));
				out.write(tab);
				out.write(String.valueOf(icchar.getInt("seed_density")));
				out.write(tab);
				out.write(icseeddate);
				out.write(tab);
				out.write("-99"); //Emergence date
				out.write(tab);
				out.write(iccutdate);//"-99"
				out.write(tab);
				out.write("-99");//Date max. Biomass
				out.write(tab);
				out.write("-99");//Date max. Root
				out.write(tab);
				out.write(String.valueOf(icchar.getDouble("max_root_development_depth")));
				out.write(tab);
				out.write(String.valueOf(icchar.getInt("removal_above_ground")));
				out.write(newLine);
			
				
				
				
				
			}	
			else {
				out.write(String.valueOf(no_cultivated_crops));
				out.write(tab);
				out.write(var_txt_general_MCK001_xnd_block6_txt5);
				out.write(newLine);
				
			}
			//System.out.println("CropID "+crop_id);
			ResultSet mcchar = gfdb.get_cultivated_crop_characteristics_by_crop_id(crop_id);
			ResultSet mcharvest = gfdb.get_cultivated_crop_harvestdate(process_id);
			ResultSet mcseed = gfdb.get_cultivated_crop_seeddate(process_id);
			mcchar.last();
			mcharvest.first();
			mcseed.first();
			//System.out.println("Ergebnisset: "+mcchar+ " ; "+mcchar.isFirst()+mcchar.isLast());
			//System.out.println(mcchar.getRow());
			
			//seeddate
			mcseed.getInt("xn_day");
			int day = mcseed.getInt("xn_day");
			String d = Integer.toString(day) ;
			if (day<10){ d = "0"+d;}
			int mnth = mcseed.getInt("xn_month");
			String m = Integer.toString(mnth);
			if (mnth<10){ m = "0"+m;}
			int yr = mcseed.getInt("xn_year");
			String y = Integer.toString(yr);//year
			if (yr<10){ y = "0"+y;}
			String seeddate =  d+m+y;
			
			//harvestdate
			mcharvest.getInt("xn_day");
			day = mcharvest.getInt("xn_day");
			d = Integer.toString(day) ;
			if (day<10){ d = "0"+d;}
			mnth = mcharvest.getInt("xn_month");
			m = Integer.toString(mnth);
			if (mnth<10){ m = "0"+m;}
			yr = mcharvest.getInt("xn_year");
			y = Integer.toString(yr);//year
			if (yr<10){ y = "0"+y;}
			String harvestdate =  d+m+y;
			
			
			
			out.write(String.valueOf(idofmaincrop));
			out.write(tab);
			out.write(mcchar.getString("xn_code"));
			out.write(tab);
			out.write(mcchar.getString("variety"));
			out.write(tab);
			out.write(String.valueOf(mcchar.getDouble("seeding_depth")));
			out.write(tab);
			out.write(String.valueOf(mcchar.getDouble("row_spacing")));
			out.write(tab);
			out.write(String.valueOf(mcchar.getInt("seed_density")));
			out.write(tab);
			out.write(seeddate);
			out.write(tab);
			out.write("-99"); //Emergence date
			out.write(tab);
			out.write(harvestdate);
			out.write(tab);
			out.write("-99");//Date max. Biomass
			out.write(tab);
			out.write("-99");//Date max. Root
			out.write(tab);
			out.write(String.valueOf(mcchar.getDouble("max_root_development_depth")));
			out.write(tab);
			out.write(String.valueOf(mcchar.getInt("removal_above_ground")));
			out.write(newLine);
			out.write(newLine);
			
			
			} catch (Exception e) {
			System.err.println("Error in buildBlock_cultivated_crop " + e);
			e.printStackTrace();
		}
	}
		
	private void buildBlock_header_cultivated_crop(PrintWriter out){
		try {
			
		
		out.write(var_txt_general_MCK001_xnd_line_two);
		out.write(newLine);
		out.write(var_txt_general_MCK001_xnd_block6_title);
		out.write(newLine);
		out.write(var_txt_general_MCK001_xnd_block6_txt1);
		out.write(newLine);
		out.write(var_txt_general_MCK001_xnd_block6_txt2);
		out.write(newLine);
		out.write(var_txt_general_MCK001_xnd_block6_txt3);
		out.write(newLine);
		out.write(var_txt_general_MCK001_xnd_block6_txt4);
		out.write(newLine);
		out.write(newLine);
		} catch (Exception e) {
			System.err.println("Error in buildBlock_header_cultivated_crop " + e);
		}
	}
	
	/**
	 * 10006
	 * @param out
	 * @param xnsim
	 */
	private void buildBlock_mineral_fert_API (PrintWriter out, XnSimObj xnsim){
		try {
			buildBlock_header_mineral_fert(out);
			out.write(var_txt_MCK001_xnd_block7_number);
			out.write(tab);
			int no_minfertilizer_applications=0;
			//Action.Fert_P, Action.Fert_K not included!!!
			for (int i=0; i<xnsim.getNumSchedule(); i++) {
				ArrayList<Action> fertactions = ((Crop)xnsim.getScheduleActivity(i)).getActions().getAllActionsbyId(Action.NFERTAIDS);
				for (int j =0; j<fertactions.size(); j++) {
					if (fertactions.get(j).getStatus() ==Action.SUCCESFUL) {
						no_minfertilizer_applications++;
					}
				}
			}		
			out.write(String.valueOf(no_minfertilizer_applications));
			out.write(tab);
			out.write(var_txt_general_MCK001_xnd_block7_txt3);
			out.write(newLine);
			
			//here we have several rows for the various fertilizer applications
			for (int i=0; i<xnsim.getNumSchedule(); i++) {
				ArrayList<Action> minfactions = ((Crop)xnsim.getScheduleActivity(i)).getActions().getAllActionsbyId(Action.NFERTAIDS);
				for (int j =0; j<minfactions.size(); j++) {
					Action act = minfactions.get(j);
					if (act.getStatus() == Action.SUCCESFUL) {
						//create correct date format	
						MinFertData fertd = getMinFertData(act.getActionID()); 					
						out.write(xndateform.format(act.getDate()));
						out.write(tab);
						out.write(fertd.name); //Name
						out.write(tab);
						out.write(fertd.code); //Code
						out.write(tab);
						out.write(Double.toString(act.getAmount()));  //total N
						out.write(tab);
						out.write(Double.toString(act.getAmount()*fertd.nitrate_n));  //nitrate N
						out.write(tab);
						out.write(Double.toString(act.getAmount()*fertd.ammonia_n)); //Ammonia N
						out.write(tab);
						out.write(Double.toString(act.getAmount()*fertd.urea_n)); //Urea N
						out.write(newLine);
					}
				}
			} 				
			out.write(newLine);					
			} catch (Exception e) {
			System.err.println("Error in buildBlock_mineral_fert " + e);
			e.printStackTrace();
		}
	}
	
	/**
	 * 10006 alt
	 * @param out
	 * @param process_id
	 */
	private void buildBlock_mineral_fert (PrintWriter out, int process_id){
		try {
			buildBlock_header_mineral_fert(out);
			out.write(var_txt_MCK001_xnd_block7_number);
			out.write(tab);
			ResultSet rs = gfdb.get_min_fertilizations(process_id);
			rs.last();
			int no_minfertilizer_applications=rs.getRow();
			rs.beforeFirst();

			
			out.write(String.valueOf(no_minfertilizer_applications));
			out.write(tab);
			out.write(var_txt_general_MCK001_xnd_block7_txt3);
			out.write(newLine);
			
			//here we have several rows for the various fertilizer applications
			
			while (rs.next()){
				//create correct date format	
				int day = rs.getInt("xn_day");
				String d = Integer.toString(day) ;
				if (day<10){ d = "0"+d;}
				int mnth = rs.getInt("xn_month");
				String m = Integer.toString(mnth);
				if (mnth<10){ m = "0"+m;}
				int yr = rs.getInt("xn_year");
				String y = Integer.toString(yr);//year
				if (yr<10){ y = "0"+y;}
				String fertdate =  d+m+y;
				
				out.write(fertdate);
				out.write(tab);
				out.write(rs.getString("t4.fertilizer_mineral_name"));
				out.write(tab);
				out.write(rs.getString("t4.fertilizer_mineral_code"));
				out.write(tab);
				out.write(rs.getString("total_n"));
				out.write(tab);
				out.write(rs.getString("nitrate_n"));
				out.write(tab);
				out.write(rs.getString("ammonium_n"));
				out.write(tab);
				out.write(rs.getString("urea_n"));
				out.write(newLine);
			} 
			out.write(newLine);
				

			
			} catch (Exception e) {
			System.err.println("Error in buildBlock_mineral_fert " + e);
		}
	}
		
	private void buildBlock_header_mineral_fert(PrintWriter out){
		try {	
		out.write (var_txt_general_MCK001_xnd_line_two);
		out.write(newLine);
		out.write (var_txt_MCK001_xnd_title_two);
		out.write(newLine);
		out.write (var_txt_general_MCK001_xnd_line_two);
		out.write(newLine);
		out.write(var_txt_general_MCK001_xnd_block7_title);
		out.write(newLine);
		out.write(var_txt_general_MCK001_xnd_block7_txt1);
		out.write(newLine);
		out.write(var_txt_general_MCK001_xnd_block7_txt2);
		out.write(newLine);
		out.write(newLine);
		} catch (Exception e) {
			System.err.println("Error in buildBlock_header_mineral_fert " + e);
		}
	}

	/**
	 * 10007
	 * @param out
	 * @param xnsimob
	 */
	private void buildBlock_organic_fert_API(PrintWriter out, XnSimObj xnsimob){
		try {
			buildBlock_header_organic_fert(out);
			out.write(var_txt_MCK001_xnd_block8_number);
			out.write(tab);
			int no_orgfertilizer_applications =0;
			for (int i=0; i<xnsimob.getNumSchedule(); i++) {
				ArrayList<Action> fertactions = ((Crop)xnsimob.getScheduleActivity(i)).getActions().getAllActionsbyId(Action.ORGFERTAIDS);
				for (int j =0; j<fertactions.size(); j++) {
					if (fertactions.get(j).getStatus() ==Action.SUCCESFUL) {
						no_orgfertilizer_applications++;
					}
				}
			}	
			out.write(String.valueOf(no_orgfertilizer_applications));
			out.write(tab);
			out.write(var_txt_general_MCK001_xnd_block8_txt3);
			out.write(newLine);
			//here we have several rows for the various organic fertilizer applications
			for (int i=0; i<xnsimob.getNumSchedule(); i++) {
				ArrayList<Action> minfactions = ((Crop)xnsimob.getScheduleActivity(i)).getActions().getAllActionsbyId(Action.ORGFERTAIDS);
				for (int j =0; j<minfactions.size(); j++) {
					Action act = minfactions.get(j);
					if (act.getStatus() == Action.SUCCESFUL) {
						OrgFertData fertd = getOrgFertData(act.getActionID()); 
						out.write(xndateform.format(act.getDate()));
						out.write(tab);
						out.write(fertd.name);
						out.write(tab);
						out.write(fertd.code);
						out.write(tab);
						out.write(Double.toString(act.getAmount()*fertd.ts));  //TS
						out.write(tab);
						out.write(Double.toString(act.getAmount()));  //Total N
						out.write(tab);
						out.write(Double.toString(act.getAmount()*fertd.ammonia_n));  //Ammonium N
						out.write(tab);
						out.write(Double.toString(act.getAmount()*fertd.org_sub)); //Org Substance
						out.write(newLine);
					} 
					out.write(newLine);
				}
			}
		} catch (Exception e) {
			System.err.println("Error in buildBlock_organic_fert_API " + e);
			e.printStackTrace();
		}
	}

	
	private void buildBlock_organic_fert(PrintWriter out, int process_id){
		try {
			buildBlock_header_organic_fert(out);
			out.write(var_txt_MCK001_xnd_block8_number);
			out.write(tab);
			ResultSet rs = gfdb.getnumber_organic_fertilizations(process_id);
			rs.last();
			int no_orgfertilizer_applications=rs.getRow();
			rs.beforeFirst();
			
			out.write(String.valueOf(no_orgfertilizer_applications));
			out.write(tab);
			out.write(var_txt_general_MCK001_xnd_block8_txt3);
			out.write(newLine);
			//here we have several rows for the various organic fertilizer applications
			
			while (rs.next()){
				//create correct date format	
				int day = rs.getInt("xn_day");
				String d = Integer.toString(day) ;
				if (day<10){ d = "0"+d;}
				int mnth = rs.getInt("xn_month");
				String m = Integer.toString(mnth);
				if (mnth<10){ m = "0"+m;}
				int yr = rs.getInt("xn_year");
				String y = Integer.toString(yr);//year
				if (yr<10){ y = "0"+y;}
				String orgfertdate =  d+m+y;
				
				out.write(orgfertdate);
				out.write(tab);
				out.write(rs.getString("t4.fertilizer_organic_german_name"));
				out.write(tab);
				out.write(rs.getString("t4.fertilizer_organic_code"));
				out.write(tab);
				out.write(rs.getString("TS"));
				out.write(tab);
				out.write(rs.getString("total_n"));
				out.write(tab);
				out.write(rs.getString("ammonium_n"));
				out.write(tab);
				out.write(rs.getString("org_sub"));
				out.write(newLine);
			} 
			out.write(newLine);

		} catch (Exception e) {
			System.err.println("Error in buildBlock_organic_fert " + e);
		}
	}
		
	private void buildBlock_header_organic_fert(PrintWriter out){
		try {
		
			out.write(var_txt_general_MCK001_xnd_line);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block8_title);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block8_txt1);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block8_txt2);
			out.write(newLine);
			out.write(newLine);
		} catch (Exception e) {
			System.err.println("Error in buildBlock_header_organic_fert " + e);
		}
	}
	
	private void buildBlock_soil_management_API(PrintWriter out, XnSimObj xnsim){
		try {
			buildBlock_header_soil_management(out);
			out.write(var_txt_MCK001_xnd_block9_number);
			out.write(tab);
			int no_soil_management_activities=0;
			for (int i=0; i<xnsim.getNumSchedule(); i++) {
				ArrayList<Action> soilpactions = ((Crop)xnsim.getScheduleActivity(i)).getActions().getAllActionsbyId(Action.SOILPRAIDS);
				for (int j =0; j<soilpactions.size(); j++) {
					if (soilpactions.get(j).getStatus() ==Action.SUCCESFUL) {
						no_soil_management_activities++;
					}

				}
			}
			
			out.write(String.valueOf(no_soil_management_activities)); //Number of Soil Management Activities
			out.write(tab);
			out.write(var_txt_general_MCK001_xnd_block9_txt3);
			out.write(newLine);
			//here we have several rows for the various soil activities
			Date dlastaction = new Date(-200L*365L*24L*60L*60L*1000L); // 1870 to make sure that two actions are not on the same day
			GregorianCalendar actiondate = new GregorianCalendar();
			for (int i=0; i<xnsim.getNumSchedule(); i++) {
				ArrayList<Action> soilpactions = ((Crop)xnsim.getScheduleActivity(i)).getActions().getAllActionsbyId(Action.SOILPRAIDS);
				for (int j =0; j<soilpactions.size(); j++) {
					Action act = soilpactions.get(j);
					if (act.getStatus() == Action.SUCCESFUL) {
						//System.out.println(act.getActionID()+": " + act.getStatus() +" am " + act.getDate());
						actiondate.setTime(act.getDate());
						if (dlastaction.compareTo(act.getDate())>=0) {
							actiondate.setTime(dlastaction);
							actiondate.add(Calendar.DATE, 1);
						}
						dlastaction.setTime(actiondate.getTimeInMillis());
						String actdate =  xndateform.format(actiondate.getTime());
						out.write(actdate);
						out.write(tab);
						//depth:
						out.write(Double.toString(act.getAmount()));
						out.write(tab);
						out.write(getSoilPrepName(act.getActionID()));
						out.write(tab);
						out.write(getSoilPrepCode(act.getActionID()));
						out.write(newLine);
					}

				}
			} 
			out.write(newLine);

			
		} catch (Exception e) {
			System.err.println("Error in buildBlock_soil_management_API " + e);
			e.printStackTrace();
		}
	}
	
	private void buildBlock_soil_management(PrintWriter out, int process_id){
		try {
			buildBlock_header_soil_management(out);
			out.write(var_txt_MCK001_xnd_block9_number);
			out.write(tab);
			ResultSet rs = gfdb.getnumber_soilmanagement_activities(process_id);
			rs.last();
			int no_soil_management_activities=rs.getRow();
			rs.beforeFirst();
			
			out.write(String.valueOf(no_soil_management_activities)); //Number of Soil Management Activities
			out.write(tab);
			out.write(var_txt_general_MCK001_xnd_block9_txt3);
			out.write(newLine);
			//here we have several rows for the various soil activities
			
			while (rs.next()){
				//create correct date format	
				int day = rs.getInt("xn_day");
				String d = Integer.toString(day) ;
				if (day<10){ d = "0"+d;}
				int mnth = rs.getInt("xn_month");
				String m = Integer.toString(mnth);
				if (mnth<10){ m = "0"+m;}
				int yr = rs.getInt("xn_year");
				String y = Integer.toString(yr);//year
				if (yr<10){ y = "0"+y;}
				String orgfertdate =  d+m+y;
				
				out.write(orgfertdate);
				out.write(tab);
				out.write(rs.getString("soil_management_equipment_depth"));
				out.write(tab);
				out.write(rs.getString("soil_management_equipment_german"));
				out.write(tab);
				out.write(rs.getString("soil_management_equipment_code"));
				out.write(newLine);
			} 
			out.write(newLine);
			
			
		} catch (Exception e) {
			System.err.println("Error in buildBlock_soil_management " + e);
		}
	}
	
	private void buildBlock_header_soil_management(PrintWriter out){
		try {
			
			out.write(var_txt_general_MCK001_xnd_line);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block9_title);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block9_txt1);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block9_txt2);
			out.write(newLine);
			out.write(newLine);


		} catch (Exception e) {
			System.err.println("Error in buildBlock_header_soil_management " + e);
		}
	}
		
	private void buildBlock_irrigation(PrintWriter out){
		try {
			buildBlock_header_irrigation(out);
			out.write(var_txt_MCK001_xnd_block10_number);
			out.write(tab);
			
			//int no_irrigation_activities= gfdb.getIrrigation_activities(1);
			
			//out.write(String.valueOf(no_irrigation_activities)); //About the number of Irrigation Activities
			out.write("0"); //No of irrigation activities
			out.write(tab);
			out.write(var_txt_general_MCK001_xnd_block10_txt3);
			out.write(newLine);
			//out.write(gfdb.getIrrigation_date(1,1,1,2008));
			out.write(""); //no irrigation
			out.write(tab);
			//out.write (gfdb.getIrrigation_amount());
			out.write(tab);
			//out.write (gfdb.getIrrigation_code());
			out.write(newLine);
			out.write(newLine);
		} catch (Exception e) {
			System.err.println("Error in buildBlock_irrigation " + e);
			e.printStackTrace();
		}
	}
		
	private void buildBlock_header_irrigation(PrintWriter out){
		try {
			
			out.write(var_txt_general_MCK001_xnd_line);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block10_title);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block10_txt1);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block10_txt2);
			out.write(newLine);
			out.write(newLine);


		} catch (Exception e) {
			System.out.println("Error in buildBlock_header_irrigation " + e);
		}
	}
		
	private void buildBlock_soilprofiles(PrintWriter out, long soilid){
		int soil_id = (int)soilid;
		try {
			
			int no_layers_per_horizon= gfdb.getnumber_layer_per_horizon(soil_id);
			
			this.buildBlock_header_soilprofiles(out);
			//out.write(gfdb.()); Get the Number of Horizons
			out.write(var_txt_MCK001_xnd_block11_number);
			out.write(tab);
			out.write(String.valueOf(gfdb.getsum_number_simulationlayers(soil_id)));
			out.write(tab);
			//out.write(String.valueOf(no_layers_per_horizon));
			out.write(gfdb.getprofile_layer_thickness(soil_id));
			out.write(newLine);
			for(int x=1;x<=no_layers_per_horizon;x++){
				//here we have several rows for the various soilprofiles
				//out.write (gfdb.getprofile_layer_number(130181));
				out.write(String.valueOf(x));
				out.write(tab);
				out.write (gfdb.getnumber_simulation_layers_per_profile_layer(soil_id,x));
				out.write(tab);
				out.write (gfdb.getClay_content(soil_id,x));
				out.write(tab);
				out.write (gfdb.getSilt_content(soil_id,x));
				out.write(tab);
				out.write (gfdb.getSand_content(soil_id,x));
				out.write(tab);
				out.write (gfdb.getOrgan_content(soil_id,x));
				out.write(tab);
				out.write (gfdb.getBulk_Density(soil_id,x));
				out.write(tab);
				out.write (gfdb.getStone_cont(soil_id,x));
				out.write(tab);
				out.write (gfdb.getph_value(soil_id,x));
				out.write(newLine);
			}
			

		} catch (Exception e) {
			System.err.println("Error in buildBlock_soilprofiles " + e);
			e.printStackTrace();
			System.exit(1);
		}
	}
		
	private void buildBlock_header_soilprofiles(PrintWriter out){
		try {
			
			out.write(var_txt_general_MCK001_xnd_line_four);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block11_txt1);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block11_txt2);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block11_txt3);
			out.write(newLine);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block11_txt4);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block11_txt5);
			out.write(newLine);
			out.write(newLine);


		} catch (Exception e) {
			System.out.println("Error in buildBlock_header_soilprofiles " + e);
		}
	}
	
	/**
	 * Method to include the Nitrogen- and soilmoisture values from previous runs into the start values of the XN run.
	 * Section 10011
	 * @param out handle of the xnd-file to write
	 * @param simob simulation object containing all the data
	 */
	@SuppressWarnings("unused")
	private void buildBlock_sample_soilprofiles_API(PrintWriter out, XnSimObj simob){//INITIAL VALUES
		Date startdate = simob.getStartdate();
		int soil_id = (int)simob.getSoiltypeid();
		SoilValues soilval = simob.getSoilValuesbyDate(startdate);
		System.out.println("Soilvalues: "+soilval);
		if (soilval != null && !useStaticSoilvalues) {

			try {	
				int no_horizons= gfdb.getnumber_layer_per_horizon_sample(soil_id);//START VALUES
				//z.B. 3
				Integer[] layers_p_hor = new Integer[no_horizons];
				for (int i = 0 ; i < no_horizons; i++) {
					layers_p_hor[i]= Integer.parseInt(gfdb.getnumber_simulation_layers_per_profile_layer_sample(soil_id,i+1));
				}
				Double[] distno3 = getNO3distribution(soil_id);
				Double[] distnh4 = getNH4distribution(soil_id);
				
				Double[] valno3 = distribLayersCustom(distno3, soilval.getNo3_total());
				// Information ueber 30-60-90 nicht verwendet!
				Double[] valnh4 = distribLayersCustom(distnh4, soilval.getNh4_total());

				this.buildBlock_header_sample_soilprofiles(out);
				//out.write(gfdb.()); Get the Number of Horizons	
				out.write(var_txt_MCK001_xnd_block12_number);
				out.write(tab);

				out.write(xndateform.format(startdate));
				out.write(tab);
				out.write(String.valueOf(gfdb.getsum_number_simulationlayers_sample(soil_id)));
				out.write(tab);
				out.write(var_txt_general_MCK001_xnd_block12_txt5);
				out.write(newLine);
				//System.out.println(no_sample_layers_per_horizon);
				for(int x=1;x<=no_horizons;x++){
					out.write(String.valueOf(x));
					out.write(tab);
					out.write (String.valueOf(layers_p_hor[x-1]));
					out.write(tab);
					if (x==1) { //bis 30 cm, sonst nichts
						out.write(String.valueOf(soilval.getWater_30())); //immer der gleiche Wert!
					System.out.println("Hor 1: Soilwater ct 30: "+soilval.getWater_30());
					} else if (x==2) {
						out.write(String.valueOf(Math.min(42,soilval.getWater_30())));
						System.out.println("Hor 2: Soilwater ct 30: "+soilval.getWater_30());
					} else if (x<=4) {
						out.write(String.valueOf(Math.min(42,soilval.getWater_60())));
						System.out.println("Hor 3+4: Soilwater ct 60: "+soilval.getWater_60());
					} else {
						out.write(String.valueOf(Math.min(22,soilval.getWater_60())));
						System.out.println("Hor 5+: Soilwater ct 60: "+soilval.getWater_60());
					}
					
					out.write(tab);
					out.write (gfdb.getmatrix_pot(soil_id,x));
					out.write(tab);
					out.write (gfdb.getsoil_temp(soil_id,x));
					out.write(tab);
					out.write (dblform.format(valnh4[x-1]));
					out.write(tab);
					out.write (dblform.format(valno3[x-1]));
					out.write(tab);
					out.write (gfdb.getroot_density(soil_id,x));
					out.write(newLine);
				}

			} catch (Exception e) {
				System.out.println("Error in buildBlock_sample_soilprofiles_API " + e);
				e.printStackTrace();
				System.exit(1);
			}
		} else if (soilval == null || useStaticWater){
			buildBlock_sample_soilprofiles(out, (int)simob.getSoiltypeid(), simob.getStartdate());
		} else {
			try {
				int no_sample_layers_per_horizon= gfdb.getnumber_layer_per_horizon_sample(soil_id);//START VALUES	
				this.buildBlock_header_sample_soilprofiles(out);
				//out.write(gfdb.()); Get the Number of Horizons	
				out.write(var_txt_MCK001_xnd_block12_number);
				out.write(tab);
				
				// CT 100408 replaced out.write(gfdb.getsoil_sample_date(soil_id)); by out.write(gfdb.getsoil_sample_date_static(soil_id));
				// -> fixed the sample date so it fits to the new start date 010801 for all simulations
				
				Date sampdate = gfdb.getsoil_sample_date_Date(soil_id);
				//if (sampdate.before(simstart)) {sampdate=simstart;}
				sampdate=simob.getStartdate();   //Messwerte werden immer auf Simulationsbeginn gesetzt!
				out.write(xndateform.format(sampdate));
				//out.write(gfdb.getsoil_sample_date_static(soil_id));
				//out.write(gfdb.getsoil_sample_date(soil_id));
				out.write(tab);
				out.write(String.valueOf(gfdb.getsum_number_simulationlayers_sample(soil_id)));
				out.write(tab);
				out.write(var_txt_general_MCK001_xnd_block12_txt5);
				out.write(newLine);
				//System.out.println(no_sample_layers_per_horizon);
				for(int x=1;x<=no_sample_layers_per_horizon;x++){
					//here we have several rows for the various sample_soilprofiles
					//out.write (gfdb.getprofile_layer_sample(1130180));
					out.write(String.valueOf(x));
					out.write(tab);
					out.write (gfdb.getnumber_simulation_layers_per_profile_layer_sample(soil_id,x));
					out.write(tab);
					
					if (x==1) { //bis 30 cm, sonst nichts
						out.write(String.valueOf(soilval.getWater_30())); //immer der gleiche Wert!
					System.out.println("Hor 1: Soilwater ct 30: "+soilval.getWater_30());
					} else if (x==2) {
						out.write(String.valueOf(Math.min(42,soilval.getWater_30())));
						System.out.println("Hor 2: Soilwater ct 30: "+soilval.getWater_30());
					} else if (x<=4) {
						out.write(String.valueOf(Math.min(42,soilval.getWater_60())));
						System.out.println("Hor 3+4: Soilwater ct 60: "+soilval.getWater_60());
					} else {
						out.write(String.valueOf(Math.min(22,soilval.getWater_60())));
						System.out.println("Hor 5+: Soilwater ct 60: "+soilval.getWater_60());
					}			
					
					out.write(tab);
					out.write (gfdb.getmatrix_pot(soil_id,x));
					out.write(tab);
					out.write (gfdb.getsoil_temp(soil_id,x));
					out.write(tab);
					// CT 100427 adapted
					out.write (gfdb.getNH4(soil_id,x));
					out.write(tab);
					//CT 100427 adapted
					out.write (gfdb.getNO3(soil_id,x));
					out.write(tab);
					out.write (gfdb.getroot_density(soil_id,x));
					out.write(newLine);
					
				}
		
			} catch (Exception e) {
				System.out.println("Error in buildBlock_sample_soilprofiles_API (staticSoil Dyn Water) " + e);
			}
		}
	}
	
	/**
	 * Old version of printing startvalues for nitrogen into the xnd file.
	 * Used if no endogenous values are available (at model start) 
	 * 10011
	 * @param out handle of xnd file
	 * @param soil_id Soil type id
	 * @author German Calberto
	 */
	private void buildBlock_sample_soilprofiles(PrintWriter out, int soil_id, Date simstart){//INITIAL VALUES
		try {
			int no_sample_layers_per_horizon= gfdb.getnumber_layer_per_horizon_sample(soil_id);//START VALUES	
			this.buildBlock_header_sample_soilprofiles(out);
			//out.write(gfdb.()); Get the Number of Horizons	
			out.write(var_txt_MCK001_xnd_block12_number);
			out.write(tab);
			
			// CT 100408 replaced out.write(gfdb.getsoil_sample_date(soil_id)); by out.write(gfdb.getsoil_sample_date_static(soil_id));
			// -> fixed the sample date so it fits to the new start date 010801 for all simulations
			
			Date sampdate = gfdb.getsoil_sample_date_Date(soil_id);
			//if (sampdate.before(simstart)) {sampdate=simstart;}
			sampdate=simstart;   //Messwerte werden immer auf Simulationsbeginn gesetzt!
			out.write(xndateform.format(sampdate));
			//out.write(gfdb.getsoil_sample_date_static(soil_id));
			//out.write(gfdb.getsoil_sample_date(soil_id));
			out.write(tab);
			out.write(String.valueOf(gfdb.getsum_number_simulationlayers_sample(soil_id)));
			out.write(tab);
			out.write(var_txt_general_MCK001_xnd_block12_txt5);
			out.write(newLine);
			//System.out.println(no_sample_layers_per_horizon);
			for(int x=1;x<=no_sample_layers_per_horizon;x++){
				//here we have several rows for the various sample_soilprofiles
				//out.write (gfdb.getprofile_layer_sample(1130180));
				out.write(String.valueOf(x));
				out.write(tab);
				out.write (gfdb.getnumber_simulation_layers_per_profile_layer_sample(soil_id,x));
				out.write(tab);
				out.write (gfdb.getwater_cont(soil_id,x));
				out.write(tab);
				out.write (gfdb.getmatrix_pot(soil_id,x));
				out.write(tab);
				out.write (gfdb.getsoil_temp(soil_id,x));
				out.write(tab);
				// CT 100427 adapted
				out.write (gfdb.getNH4(soil_id,x));
				out.write(tab);
				//CT 100427 adapted
				out.write (gfdb.getNO3(soil_id,x));
				out.write(tab);
				out.write (gfdb.getroot_density(soil_id,x));
				out.write(newLine);
				
			}
	
		} catch (Exception e) {
			System.out.println("Error in buildBlock_sample_soilprofiles " + e);
		}
	}
	
	
	private void buildBlock_header_sample_soilprofiles(PrintWriter out){
		try {
	
			out.write(var_txt_general_MCK001_xnd_line_four);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block12_txt1);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block12_txt2);
			out.write(newLine);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block12_txt3);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block12_txt4);
			out.write(newLine);
			out.write(newLine);
			
			
		} catch (Exception e) {
			System.out.println("Error in buildBlock_header_sample_soilprofiles " + e);
		}
	}

	private void buildBlock_water_readings_API(PrintWriter out){
		try {
			this.buildBlock_header_water_readings(out);

			//out.write(String.valueOf(gfdb.getsum_number_water_reading_depth(1)));
			out.write("0");
			out.write(tab);
			//int no_water_readings = gfdb.getnumber_water_readings(1);
			//out.write(String.valueOf(no_water_readings));
			out.write("0");
			out.write(newLine);
			out.write(newLine);
			//out.write(gfdb.getwater_reading_date(1));
			out.write("");  //Water reading date
			out.write(tab);
			//out.write(gfdb.getwater_content());  //Water content
			out.write("");
			out.write(tab);
			//out.write(gfdb.getmatrix_potential());  //matrix potential
			out.write("");
			out.write(newLine);


		} catch (Exception e) {
			System.out.println("Error in buildBlock_water_readings " + e);
		}
	}
	
	private void buildBlock_water_readings(PrintWriter out){
		try {
			this.buildBlock_header_water_readings(out);
			
			out.write(String.valueOf(gfdb.getsum_number_water_reading_depth(1)));
			out.write(tab);
			int no_water_readings = gfdb.getnumber_water_readings(1);
			out.write(String.valueOf(no_water_readings));
			out.write(newLine);
			out.write(newLine);
			out.write(gfdb.getwater_reading_date(1));
			out.write(tab);
			out.write(gfdb.getwater_content());
			out.write(tab);
			out.write(gfdb.getmatrix_potential());
			out.write(newLine);
		

	} catch (Exception e) {
		System.out.println("Error in buildBlock_water_readings " + e);
	}
}
	
	private void buildBlock_header_water_readings(PrintWriter out){
		try {
			
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line_four);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block13_txt1);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block13_txt2);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block13_txt3);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block13_txt4);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block13_txt5);
			out.write(newLine);
			out.write(newLine);
			out.write(var_txt_MCK001_xnd_block13_number);
			out.write(tab);
			
		} catch (Exception e) {
			System.out.println("Error in buildBlock_header_water_readings " + e);
		}
	}

	private void buildBlock_nitrogen_readings_API(PrintWriter out){
		try {
			
			this.buildBlock_header_nitrogen_readings(out);
			//out.write(String.valueOf(gfdb.getsum_number_nitrogen_reading_depth(1)));
			out.write("0");
			out.write(tab);
			//int no_nitrogen_readings = gfdb.getnumber_nitrogen_readings(1);
			//out.write(String.valueOf(no_nitrogen_readings));
			out.write("0");
			out.write(newLine);
			out.write(newLine);
			//out.write(gfdb.getnitrogen_reading_date(1));
			out.write("");
			out.write(tab);
			//out.write(gfdb.getnitrat_content());
			out.write("");
			out.write(tab);
			//out.write(gfdb.getammonium_content());
			out.write("");
			out.write(newLine);
			
		} catch (Exception e) {
			System.out.println("Error in buildBlock_nitrogen_readings " + e);
		}
	}
	
	private void buildBlock_nitrogen_readings(PrintWriter out){
		try {
			
			this.buildBlock_header_nitrogen_readings(out);
			out.write(String.valueOf(gfdb.getsum_number_nitrogen_reading_depth(1)));
			out.write(tab);
			int no_nitrogen_readings = gfdb.getnumber_nitrogen_readings(1);
			out.write(String.valueOf(no_nitrogen_readings));
			out.write(newLine);
			out.write(newLine);
			out.write(gfdb.getnitrogen_reading_date(1));
			out.write(tab);
			out.write(gfdb.getnitrat_content());
			out.write(tab);
			out.write(gfdb.getammonium_content());
			out.write(newLine);
			
		} catch (Exception e) {
			System.out.println("Error in buildBlock_nitrogen_readings " + e);
		}
	}
	
	private void buildBlock_header_nitrogen_readings(PrintWriter out){
		try {
			
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line_four);//HERE STARTS NITROGEN READING
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block14_title);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block14_txt1);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block14_txt2);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block14_txt3);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block14_txt4);
			out.write(newLine);
			out.write(newLine);
			out.write(var_txt_MCK001_xnd_block14_number);
			out.write(tab);
			
		} catch (Exception e) {
			System.out.println("Error in buildBlock_header_nitrogen_readings " + e);
		}
	}
	private void buildBlock_plant_measurementsI_API(PrintWriter out){
		try {
			
		this.buildBlock_header_plant_measurementsI(out);
		//int no_plant_measurements_I = gfdb.getnumber_plant_measurements_I(1);
		//out.write(String.valueOf(no_plant_measurements_I));
		out.write("0"); //""?
		out.write(tab);
		out.write(var_txt_general_MCK001_xnd_block15_txt4);
		out.write(newLine);
		out.write(newLine);
		//out.write(gfdb.getplant_measurements_I_date(1));
		out.write("");
		out.write(tab);
		//out.write(gfdb.getplant_stage_EC_measurementI());
		out.write("");
		out.write(tab);
		//out.write(gfdb.getplant_BFI_measurementI());
		out.write("");
		out.write(tab);
		//out.write(gfdb.getplant_soil_bedeck_measurementI());
		out.write("");
		out.write(tab);
		//out.write(gfdb.getplant_leaf_number_measurementI());
		out.write("");
		out.write(tab);
		//out.write(gfdb.getplant_shoot_number_measurementI());
		out.write("");
		out.write(tab);
		//out.write(gfdb.getplant_height_measurementI());
		out.write("");
		out.write(tab);
		//out.write(gfdb.getplant_root_depth_measurementI());
		out.write("");
		out.write(tab);
		//out.write(gfdb.getplant_evap_factor_measurementI());
		out.write("");
		out.write(tab);
		//out.write(gfdb.getplant_kum_nitrogen_uptake_measurementI());
		out.write("");
		out.write(newLine);
	
	
		} catch (Exception e) {
		System.err.println("Error in buildBlock_plant_measurementsI_API" + e);
		e.printStackTrace();		
		}
	}
	private void buildBlock_plant_measurementsI(PrintWriter out){
		try {
			
		this.buildBlock_header_plant_measurementsI(out);
		int no_plant_measurements_I = gfdb.getnumber_plant_measurements_I(1);
		out.write(String.valueOf(no_plant_measurements_I));
		out.write(tab);
		out.write(var_txt_general_MCK001_xnd_block15_txt4);
		out.write(newLine);
		out.write(newLine);
		out.write(gfdb.getplant_measurements_I_date(1));
		out.write(tab);
		out.write(gfdb.getplant_stage_EC_measurementI());
		out.write(tab);
		out.write(gfdb.getplant_BFI_measurementI());
		out.write(tab);
		out.write(gfdb.getplant_soil_bedeck_measurementI());
		out.write(tab);
		out.write(gfdb.getplant_leaf_number_measurementI());
		out.write(tab);
		out.write(gfdb.getplant_shoot_number_measurementI());
		out.write(tab);
		out.write(gfdb.getplant_height_measurementI());
		out.write(tab);
		out.write(gfdb.getplant_root_depth_measurementI());
		out.write(tab);
		out.write(gfdb.getplant_evap_factor_measurementI());
		out.write(tab);
		out.write(gfdb.getplant_kum_nitrogen_uptake_measurementI());
		out.write(newLine);
	
	
		} catch (Exception e) {
		System.out.println("Error in buildBlock_plant_measurementsI" + e);
		
		}
	}

	private void buildBlock_header_plant_measurementsI(PrintWriter out){
		try {
			//Here starts plant measurementsI
			out.write(var_txt_general_MCK001_xnd_line_four);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block15_title);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block15_txt1);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block15_txt2);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block15_txt3);
			out.write(newLine);
			out.write(newLine);
			out.write(var_txt_MCK001_xnd_block15_number);
			out.write(tab);	
		
		} catch (Exception e) {
			System.out.println("Error in buildBlock_header_plant_measurementsI" + e);
		}
	}
	
	private void buildBlock_plant_measurementsII_API(PrintWriter out){
		try {
			
		this.buildBlock_header_plant_measurementsII(out);
		//int no_plant_measurements_II = gfdb.getnumber_plant_measurements_II(1);
		//out.write(String.valueOf(no_plant_measurements_II));
		out.write("0");
		out.write(tab);
		out.write(var_txt_general_MCK001_xnd_block16_txt5);
		out.write(newLine);
		out.write(newLine);
		//out.write(gfdb.getplant_measurements_II_date(1));
		out.write("");
		out.write(tab);
		//out.write(gfdb.getplant_leaves_dm_measurementII());
		out.write("");
		out.write(tab);
		//out.write(gfdb.getplant_stems_dm_measurementII());
		out.write("");
		out.write(tab);
		//out.write(gfdb.getplant_fruit_dm_measurementII());
		out.write("");
		out.write(tab);
		//out.write(gfdb.getplant_root_dm_measurementII());
		out.write("");
		out.write(tab);
		//out.write(gfdb.getplant_above_biomass_dm_measurementII());
		out.write("");
		out.write(tab);
		//out.write(gfdb.getplant_leaves_nitrogen_measurementII());
		out.write("");
		out.write(tab);
		//out.write(gfdb.getplant_stems_nitrogen_measurementII());
		out.write("");
		out.write(tab);
		//out.write(gfdb.getplant_fruit_nitrogen_measurementII());
		out.write("");
		out.write(tab);
		//out.write(gfdb.getplant_root_nitrogen_measurementII());
		out.write("");
		out.write(tab);
		//out.write(gfdb.getplant_above_bio_nitrogen_measurementII());
		out.write("");
		out.write(newLine);
		out.write(newLine);
		
		} catch (Exception e) {
			System.err.println("Error in buildBlock_plant_measurementsII_API " + e);
			e.printStackTrace();
		}
	}
	
	private void buildBlock_plant_measurementsII(PrintWriter out){
		try {
			
		this.buildBlock_header_plant_measurementsII(out);
		int no_plant_measurements_II = gfdb.getnumber_plant_measurements_II(1);
		out.write(String.valueOf(no_plant_measurements_II));
		out.write(tab);
		out.write(var_txt_general_MCK001_xnd_block16_txt5);
		out.write(newLine);
		out.write(newLine);
		out.write(gfdb.getplant_measurements_II_date(1));
		out.write(tab);
		out.write(gfdb.getplant_leaves_dm_measurementII());
		out.write(tab);
		out.write(gfdb.getplant_stems_dm_measurementII());
		out.write(tab);
		out.write(gfdb.getplant_fruit_dm_measurementII());
		out.write(tab);
		out.write(gfdb.getplant_root_dm_measurementII());
		out.write(tab);
		out.write(gfdb.getplant_above_biomass_dm_measurementII());
		out.write(tab);
		out.write(gfdb.getplant_leaves_nitrogen_measurementII());
		out.write(tab);
		out.write(gfdb.getplant_stems_nitrogen_measurementII());
		out.write(tab);
		out.write(gfdb.getplant_fruit_nitrogen_measurementII());
		out.write(tab);
		out.write(gfdb.getplant_root_nitrogen_measurementII());
		out.write(tab);
		out.write(gfdb.getplant_above_bio_nitrogen_measurementII());
		out.write(newLine);
		out.write(newLine);
		
		} catch (Exception e) {
			System.out.println("Error in buildBlock_plant_measurementsII " + e);
			
			}
	}
	
	private void buildBlock_header_plant_measurementsII(PrintWriter out){
		try {
			
		
		out.write(var_txt_general_MCK001_xnd_line_four);//Here starts plant measurementsII
		out.write(newLine);
		out.write(var_txt_general_MCK001_xnd_block16_title);
		out.write(newLine);
		out.write(var_txt_general_MCK001_xnd_block16_txt1);
		out.write(newLine);
		out.write(newLine);
		out.write(var_txt_general_MCK001_xnd_block16_txt2);
		out.write(newLine);
		out.write(var_txt_general_MCK001_xnd_block16_txt3);
		out.write(newLine);
		out.write(var_txt_general_MCK001_xnd_block16_txt4);
		out.write(newLine);
		out.write(var_txt_general_MCK001_xnd_line_five);
		out.write(newLine);
		out.write(newLine);
		out.write(var_txt_MCK001_xnd_block16_number);
		out.write(tab);
		
		} catch (Exception e) {
			System.out.println("Error in buildBlock_header_plant_measurementsII" + e);
			
			}
	}
	
	private void buildBlock_closing_block (PrintWriter out){
		try {
			out.write(var_txt_general_MCK001_xnd_line_four);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block17_title);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line_four);
			out.close();	
			
		} catch (Exception e) {
			System.out.println("Error in buildBlock_closing_block" + e);
			
			}
	}
	
	String getDestinationFolder() {
		try{
			RandomAccessFile mm = new RandomAccessFile(configFile, "r");
			String m;	
			while ((m = mm.readLine()) != null) {
				// System.out.println("read: "+s);
				StringTokenizer st = new StringTokenizer(m,"=");
				if (st.countTokens()>0){
					if (st.nextToken().equalsIgnoreCase("[destinationfolder]")){
						if(st.hasMoreTokens()){
							String fs = st.nextToken();
							mm.close();
							return fs;
						}
					}
				}
			}
			mm.close();
			return null;
		}
		catch (Exception ex2) {
			System.out.println("Error in getDestinationFolder");
			ex2.printStackTrace();
			return null;
		}
	}

	private String getSoilPrepName(int id) {
		switch (id) {
		case Action.SoilPrep_DiscHarrow: return "Scheibenegge";
		case Action.SoilPrep_Grubber_1: return "Schwergrubber";
		case Action.SoilPrep_Grubber_2: return "Schwergrubber";
		case Action.SoilPrep_Plough: return "Volldrehpflug";
		case Action.SoilPrep_Seedbkomb: return "Saatbettkombination";
		case Action.SoilPrep_Rotaryharrow: return "Kreiselegge";
		}
		return "";
	}
	
	private String getSoilPrepCode(int id) {
		switch (id) {
		case Action.SoilPrep_DiscHarrow: return "TI034";
		case Action.SoilPrep_Grubber_1: return "TI038";
		case Action.SoilPrep_Grubber_2: return "TI038";
		case Action.SoilPrep_Plough: return "TI034";
		case Action.SoilPrep_Seedbkomb: return "TI032";
		case Action.SoilPrep_Rotaryharrow: return "TI024";
		}
		return "";
	}
	
	/**
	 * Method to get data of mineral fertilizers to be put into the xnd-file
	 * @param id
	 * @return
	 */
	private MinFertData getMinFertData(int id) {
		MinFertData fertd = new MinFertData();
		switch (id) {
		case Action.Fert_UR_1: 
		case Action.Fert_UR_2:
		case Action.Fert_UR_3: {
			fertd.name="Harnstoff";
			fertd.code="FE005";
			fertd.nitrate_n=0D;
			fertd.ammonia_n=0D;
			fertd.urea_n=1D;
		} break;
		case Action.Fert_AHL_1:
		case Action.Fert_AHL_2:
		case Action.Fert_AHL_3: {
			fertd.name="Ammonnitrat-Harnstoff-Lösung";
			fertd.code="FE010";
			fertd.nitrate_n=0.25D;
			fertd.ammonia_n=0.25D;
			fertd.urea_n=0.5D;
		} break;
		case Action.Fert_KAS_1:
		case Action.Fert_KAS_2:
		case Action.Fert_KAS_3: {
			fertd.name="Kalkammonsaltpeter";
			fertd.code="FE011";
			fertd.nitrate_n=0.5D;
			fertd.ammonia_n=0.5D;
			fertd.urea_n=0D;
		} break;
		case Action.Fert_ASS: {
			fertd.name="Ammonsulfatsaltpeter";
			fertd.code="FE003";
			fertd.nitrate_n=0.27D;
			fertd.ammonia_n=0.73D;
			fertd.urea_n=0D;
		} break;
	
		}
		return fertd;
	}
	

	
	
	/**
	 * This method distributes the amount "total" across "numlayers" of soil layers using a geometric sequence
	 * @param numlayers number of layers
	 * @param a quotient of the geometric sequence
	 * @param total overall amount that is to be distributed
	 * @return a vector with the distributed amounts, the sum of the vector is equal to "total"
	 */
	private Double[] distribLayersGeom(int numlayers, double a, double total) {
		double sum =0;
		Double[] result = new Double[numlayers];
		for (int i = 0; i< numlayers; i++) {
			result[i] = Math.pow(a, (double)i);
			sum += result[i];	
		}
		for (int i=0; i < numlayers; i++) {
			result[i]*=(total/sum);
		}		
		return result;
	}
	
	/**
	 * This method distributes the quantity "total" proportionally to customdist across the number of classes as there are within customdist
	 * @param customdist custom distribution (need not to be normalized)
	 * @param total quantity to be distributed
	 * @return a similar distribution as customdist with the total sum total
	 */
	private Double[] distribLayersCustom(Double[] customdist, double total) {
		Double[] result = new Double[customdist.length];
		double sum =0;
		for (int i = 0; i< customdist.length; i++) {
			sum += customdist[i];
		}
		for (int i = 0; i < customdist.length; i++) {
			if (sum==0) {
				result[i] = 1D/(double)customdist.length;
			} else {
			    result[i] = customdist[i]*total/sum;
			}
		}
		return result;
	}
	
	//If you add more soil types here, don't forget to mention the soiltype also in line 769!
	
	/**
	 * Method to get the soil specific hard coded humus distributions after harvest
	 * @param soiltypeid
	 * @return
	 */
	private Double[] getHumusDist(long soiltypeid) {
		if (soiltypeid == 0) { // 16 layers
			return new Double[] {0.25, 0.25, 0.25, 0.25, 0D, 0D, 0D, 0D, 0D, 0D, 0D, 0D, 0D, 0D, 0D, 0D};
		} else if (soiltypeid == 3) { //14 layers
			return new Double[] {0.11, 0.11,	0.11, 0.11, 0.1, 0.1, 0.05, 0.05, 0.05, 0.05, 0.05, 0.05, 0.05, 0.05};
		} else if (soiltypeid == 1) {  //16 layers
			return new Double[] {0.13, 0.13,	0.13, 0.13, 0.13, 0.13, 0.03, 0.03, 0.03, 0.03, 0.03, 0.03, 0.02, 0.02, 0.02, 0.02};
		} else if (soiltypeid == 6) { //8 layers
			return new Double[] {0.16, 0.16, 0.16, 0.16, 0.15, 0.09, 0.06, 0.06};
		} else if (soiltypeid == 9){ //18 layers
			return new Double[] {0.0667, 0.0666, 0.0666, 0.0666, 0.0666, 0.0666, 0.0685, 0.0685, 0.0685, 0.0685, 0.0685, 0.0685, 0.0315, 0.0315, 0.0315, 0.0315, 0.0315, 0.0315};
		} else if (soiltypeid == 10){ //18 layers
			return new Double[] {0.1053, 0.1051, 0.1051, 0.1051, 0.1051, 0.1051, 0.0342, 0.0342, 0.0342, 0.0342, 0.0342, 0.0342, 0.0342, 0.0342, 0.0239, 0.0239, 0.0239, 0.0239};
		} else if (soiltypeid == 11){ //18 layers
			return new Double[] {0.0930, 0.0929, 0.0929, 0.0928, 0.0928, 0.0928, 0.0928, 0.0382, 0.0382, 0.0382, 0.0294, 0.0294, 0.0294, 0.0294, 0.0294, 0.0294, 0.0294, 0.0294};
		} else if (soiltypeid == 110){ //40 layers
			return new Double[] {0.1053, 0.1051, 0.1051, 0.1051, 0.1051, 0.1051, 0.0342, 0.0342, 0.0342, 0.0342, 0.0342, 0.0342, 0.0342, 0.0342, 0.0239, 0.0239, 0.0239, 0.0239, 0.1053, 0.1051, 0.1051, 0.1051, 0.1051, 0.1051, 0.0342, 0.0342, 0.0342, 0.0342, 0.0342, 0.0342, 0.0342, 0.0342, 0.0239, 0.0239, 0.0239, 0.0239, 0.0239, 0.0239, 0.0239, 0.0239};
		} else {
			System.err.println("Attention: no Humus distribution for soiltype "+soiltypeid+"! Using even distribution instead!");
			return new Double[0];
		}
	}
	
	
	
	/**
	 * Method to get the soil specific depth distribution of Litter C
	 * @param soiltypeid
	 * @return
	 */
	private double getLitterCdistribCoef(long soiltypeid) {
		if (soiltypeid == 0) { // 16 layers
			return 0.82;
		} else if (soiltypeid == 3) { //14 layers
			return 0.79;
		} else if (soiltypeid == 1) {  //16 layers
			return 0.79;
		} else if (soiltypeid == 6) { //8 layers
			return 0.68;
		} else if (soiltypeid == 9){ //18 layers
			return 0.84;
		} else if (soiltypeid == 10){ //18 layers
			return 0.84;
		} else if (soiltypeid == 11){ //18 layers
			return 0.84;
		} else if (soiltypeid == 110){ //18 layers
			return 0.84;
		} else {
			System.err.println("Error: No distribution for Litter carbon for soiltype " +soiltypeid+ " defined!\nUsing geometric distribution with parameter 0.75");
			return 0.75;				
		}
	}
	
	/**
	 * Method to get the soil specific depth distribution of Litter N
	 * @param soiltypeid
	 * @return
	 */
	private double getLitterNdistribCoef(long soiltypeid) {
		if (soiltypeid == 0) { // 16 layers
			return 0.83;
		} else if (soiltypeid == 3) { //14 layers
			return 0.79;
		} else if (soiltypeid == 1) {  //16 layers
			return 0.79;
		} else if (soiltypeid == 6) { //8 layers
			return 0.67;
		} else if (soiltypeid == 9){ //18 layers
			return 0.85;
		} else if (soiltypeid == 10){ //18 layers
			return 0.85;
		} else if (soiltypeid == 11){ //18 layers
			return 0.85;
		} else if (soiltypeid == 110){ //18 layers
			return 0.85;
		} else {
			System.err.println("Error: No distribution for Litter nitrogen for soiltype " +soiltypeid+ " defined!\nUsing geometric distribution with parameter 0.75");
			return 0.75;				
		}
	}
	

	
	
	/**
	 * Method to get the soil specific hard coded distributions of NH4 after harvest
	 * @param soiltypeid
	 * @return
	 */
	private Double[] getNH4distribution (long soiltypeid) {
		Double[] distnh4 = null; 
		if (soiltypeid == 0) {  // 3 horizons, 16 layers
			distnh4 = new Double[3];
			distnh4[0] = 0.24;	distnh4[1] = 0.28; distnh4[2] = 0.48;
		} else if (soiltypeid == 3) {  //5 horizons
			distnh4 = new Double[5];
			distnh4[0] = 0.17;	distnh4[1] = 0.32; distnh4[2] = 0.22;
			distnh4[3] = 0.18;	distnh4[4] = 0.1;
		} else if (soiltypeid == 1) { //3 horizons
			distnh4 = new Double[3];
			distnh4[0] = 0.66;	distnh4[1] = 0.3; distnh4[2] = 0.04;
		} else if (soiltypeid==6) {  //3 horizons; Werte von Mais
			distnh4 = new Double[3];
			distnh4[0] = 0.24;	distnh4[1] = 0.28; distnh4[2] = 0.48;
		} else if (soiltypeid == 9) {
			distnh4 = new Double[3];
			distnh4[0] = 0.33;	distnh4[1] = 0.48; distnh4[2] = 0.19;
		} else if (soiltypeid == 10) {
			distnh4 = new Double[3];
			distnh4[0] = 0.47;	distnh4[1] = 0.48; distnh4[2] = 0.05;
		} else if (soiltypeid == 11) {
			distnh4 = new Double[3];
			distnh4[0] = 0.55;	distnh4[1] = 0.16; distnh4[2] = 0.29;
		} else if (soiltypeid == 110) {
			distnh4 = new Double[3];
			distnh4[0] = 0.47;	distnh4[1] = 0.48; distnh4[2] = 0.05;
		} else { System.err.println("Nitrogen NH4 distribution in soiltype "+soiltypeid+ " not defined!"); }	
		return distnh4;
	}
	
	/**
	 * Method to return the crop specific hard coded depth distribution of NO3
	 * @param soiltypeid
	 * @return
	 */
	private Double[] getNO3distribution (long soiltypeid) {
		Double[] distno3 = null;
		if (soiltypeid == 0) {  // 3 horizons, 16 layers
			distno3 = new Double[3];
			distno3[0] = 0.11;	distno3[1] = 0.14; distno3[2] = 0.75;
		} else if (soiltypeid == 3) {  //5 horizons
			distno3 = new Double[5];
			distno3[0] = 0.12;	distno3[1] = 0.16; distno3[2] = 0.35;
			distno3[3] = 0.35;	distno3[4] = 0.01;
		} else if (soiltypeid == 1) { //3 horizons
			distno3 = new Double[3];
			distno3[0] = 0.31;	distno3[1] = 0.37; distno3[2] = 0.32;
		} else if (soiltypeid==6) {  //3 horizons; Werte von Mais
			distno3 = new Double[3];
			distno3[0] = 0.17;	distno3[1] = 0.24; distno3[2] = 0.59;
		} else if (soiltypeid == 9) {
			distno3 = new Double[3];
			distno3[0] = 0.41;	distno3[1] = 0.43; distno3[2] = 0.16;
		} else if (soiltypeid == 10) {
			distno3 = new Double[3];
			distno3[0] = 0.43;	distno3[1] = 0.49; distno3[2] = 0.08;
		} else if (soiltypeid == 11) {
			distno3 = new Double[3];
			distno3[0] = 0.60;	distno3[1] = 0.22; distno3[2] = 0.18;
		} else if (soiltypeid == 110) {
			distno3 = new Double[3];
			distno3[0] = 0.43;	distno3[1] = 0.49; distno3[2] = 0.08;
		} else { System.err.println("Nitrogen NO3 distribution in soiltype "+soiltypeid+ " not defined!"); }
		return distno3;
	}
	
	
	private class MinFertData {
		String name;
		String code;
		Double nitrate_n;
		Double ammonia_n;
		Double urea_n;
	}
	
	private OrgFertData getOrgFertData(int id) {
		OrgFertData fertd = new OrgFertData();
		switch (id) {
		case Action.Fert_PigSl: {
			fertd.name="Schweineguelle";
			fertd.code="RE009";
			fertd.ammonia_n=80D/114D;
			fertd.ts=1400D/114D;
			fertd.org_sub=98D/114D;
		} break;
		case Action.Fert_CattleS: {
			fertd.name="Rinderguelle";
			fertd.code="RE031";
			fertd.ammonia_n=20D/50D;
			fertd.ts=1480D/50D;
			fertd.org_sub=120D/50D;
		} break;
		}
		return fertd;
	}
		
	private class OrgFertData {
		String name;
		String code;
		Double ammonia_n;
		Double ts;
		Double org_sub;
	}
	
	
	private class CropData {
		ArrayList<SingleCropData> datalist;
		
		CropData() {
			datalist = new ArrayList<SingleCropData>();
			
			//WW
			SingleCropData crop = new SingleCropData();
			crop.id = 1;
			crop.xncode = "WH";
			crop.varietyname = "Winterw";
			crop.sowingdepth = 0.03F;
			crop.rowspace = 0.12F;
			crop.seeddensity = 350;			//before April, 2013: 350
			crop.rootdepth = 1.5F;
			crop.removal = 0;
			
			datalist.add(crop);  
			
			//Wintergerste
			crop = new SingleCropData();
			crop.id = 2;
			crop.xncode = "BA";
			crop.varietyname = "Winterg";
			crop.sowingdepth = 0.03F;
			crop.rowspace = 0.12F;
			crop.seeddensity = 350;
			crop.rootdepth = 1.5F;
			crop.removal = 0;
			datalist.add(crop);
			
			//Sommergerste
			crop = new SingleCropData();
			crop.id = 3;
			crop.xncode = "BA";
			crop.varietyname = "Sommer";
			crop.sowingdepth = 0.03F;
			crop.rowspace = 0.12F;
			crop.seeddensity = 400;
			crop.rootdepth = 1.5F;
			crop.removal = 0;
			datalist.add(crop);
			
			//Winterraps
			crop = new SingleCropData();
			crop.id = 4;
			//crop.xncode = "RP";
			crop.xncode = "RY";
			crop.varietyname = "Default";
			crop.sowingdepth = 0.02F;
			crop.rowspace = 0.12F;
			crop.seeddensity = 60;
			crop.rootdepth = 1.5F;
			crop.removal = 0;
			datalist.add(crop);
			
			//Silomais
			crop = new SingleCropData();
			crop.id = 5;
			crop.xncode = "MZ";
			crop.varietyname = "Silage";
			crop.sowingdepth = 0.03F;
			crop.rowspace = 0.70F;
			crop.seeddensity = 11;			//before April, 2013: 10
			crop.rootdepth = 1.5F;
			crop.removal = 1;
			datalist.add(crop);
			
			//Körnermais
			crop = new SingleCropData();
			crop.id = 6;
			crop.xncode = "MZ";
			crop.varietyname = "Grain";
			crop.sowingdepth = 0.05F;
			crop.rowspace = 0.75F;
			crop.seeddensity = 10;
			crop.rootdepth = 1.5F;
			crop.removal = 0;
			datalist.add(crop);
			
			//Zuckerrueben
			crop = new SingleCropData();
			crop.id = 7;
			crop.xncode = "BS";
			crop.varietyname = "Default";
			crop.sowingdepth = 0.03F;
			crop.rowspace = 0.5F;
			crop.seeddensity = 8;
			crop.rootdepth = 1.5F;
			crop.removal = 0;
			datalist.add(crop);			
			
			//Kartoffeln
			crop = new SingleCropData();
			crop.id = 8;
			crop.xncode = "PT";
			crop.varietyname = "Default";
			crop.sowingdepth = 0.12F;
			crop.rowspace = 0.12F;
			crop.seeddensity = 5;
			crop.rootdepth = 1.5F;
			crop.removal = 0;
			datalist.add(crop);	
			
			//Ackerfutter
			crop = new SingleCropData();
			crop.id = 9;
			crop.xncode = "AP";    //Achtung: noch sinnlos!
			crop.varietyname = "Standard";   
			crop.sowingdepth = 0.01F;
			crop.rowspace = 0.12F;
			crop.seeddensity = 350;
			crop.rootdepth = 1.5F;
			crop.removal = 1;
			datalist.add(crop);	
			
			
			//Roggen
			crop = new SingleCropData();
			crop.id = 21;
			crop.xncode = "RY";
			crop.varietyname = "Visby";
			crop.sowingdepth = 0.02F;
			crop.rowspace = 0.12F;
			crop.seeddensity = 350;
			crop.rootdepth = 1.5F;
			crop.removal = 0;
			datalist.add(crop);
			
			
			
			//Zwischenfrucht
			/* Hilfsweise Raps, aber noch nicht richtig implementiert
			crop = new SingleCropData();
			crop.id = 30;
			crop.xncode = "RP";
			crop.varietyname = "Default";
			crop.sowingdepth = 0.03F;
			crop.rowspace = 0.12F;
			crop.seeddensity = 350;
			crop.rootdepth = 1.5F;
			crop.removal = 0;
			datalist.add(crop);
			*/
			
			//Hilfsweise Wintergerste
			crop = new SingleCropData();
			crop.id = 30;
			crop.xncode = "BA";
			crop.varietyname = "Winterg";
			crop.sowingdepth = 0.03F;
			crop.rowspace = 0.12F;
			crop.seeddensity = 350;
			crop.rootdepth = 1.5F;
			crop.removal = 0;
			datalist.add(crop);
			
			
		}
		
		SingleCropData getCrop(int nr) {
			for (int i = 0; i < datalist.size(); i++) {
				if (datalist.get(i).id == nr) {
					return datalist.get(i);		
				}
			}
			return null;
		}
		

		
	}
	
	private static class SingleCropData {
		int id;
		String xncode;
		String varietyname;
		float sowingdepth;
		float rowspace;
		float seeddensity;
		float rootdepth;
		int removal;

	}	
	
	
	
	
}//end of class



