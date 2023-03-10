package uni.hh.mapmaker;

import java.io.*;

import java.sql.ResultSet;
//import java.sql.ResultSetMetaData;
import java.sql.SQLException;

//import java.sql.Statement;
//import java.util.ArrayList;

//import java.util.HashMap;
import java.util.StringTokenizer;

//import uni.hh.mysql_db.*;

import java.lang.StringBuilder;

class xn3_subplotInfo {
	int ID;
	String subplotName;
	String princCrop;	
	String weatherFn;
	String parameterFn;
	String xncFn;
	int areaNum;
	double size;
	double slope;
	String exposition;
	
	String newLine = "\r\n"; //CT 100408 to make windows line breaks although being in Linux

	
	public void writeWithHeader (PrintWriter out) {
		out.write("*Subplot data" + newLine);
		writeNoHeader(out);
	}
	
	public void writeNoHeader (PrintWriter out) {
		out.write("10003" + newLine);
		out.write("*ID\tName\tPrinc.Crop\tWeatherfile\tParameterfile\tConfigFile" + newLine);
		out.write(ID + "\t" + subplotName + "\t" + princCrop + "\t" + weatherFn + "\t" + parameterFn + "\t" + xncFn + newLine);
		out.write("*AreaNum.\tSize\tSlope\tExposition" + newLine);
		out.write(areaNum +"\t" + size + "\t" + slope + "\t" + exposition + newLine);
	}
	
}
class xn3_soilStartValuesTop {
	String date;
	int numLayers;
}
class xn3_simTime {
	String startDate;
	String endDate;
	String newLine = "\r\n";
	
	public void writeWithHeader (PrintWriter out) {
		out.write("*Simulation data" + newLine);
		writeNoHeader(out);
	}
	
	public void writeNoHeader (PrintWriter out) {
		out.write("10000" + newLine);
		out.write("*Start Date (ddmmyy)\tEnd Date (ddmmyy)" + newLine);
		out.write(startDate + "\t" + endDate + newLine);
	}
}
public class FileMaker {
	
	private GetFromDB gfdb;

	
	
	
	//Here all the generic scripting variables are initialized
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
	String var_txt_general_MCK001_xnd_block7_title = "*Mineral Fertilization (Pro Maßnahme)";
	String var_txt_general_MCK001_xnd_block7_txt1 = "Date" + tab + "Fertilizer" + tab + "Fertilizer" + tab + "N" + tab + "NO3-N" + tab + "NH4-N" + tab + "Amid-N";
	String var_txt_general_MCK001_xnd_block7_txt2 = "Datum" + tab + "Name" + tab + tab + "Code" + tab + tab + "kg/ha" + tab + "kg/ha" + tab + "kg/ha" + tab + "kg/ha";
	String var_txt_MCK001_xnd_block7_number = "10006";
	String var_txt_general_MCK001_xnd_block7_txt3 = ":Number of MINERAL Fertilizer Applications";
	String var_txt_general_MCK001_xnd_block8_title = "*Organic Fertilization (Pro Maßnahme)";
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
	String var_txt_xnm_block4_number =";20001"; //TODO: activate
	String var_txt_xnm_block5_title ="RATE CONSTANT (LEACHN, NITS, OMI)";
	String var_txt_xnm_block5_txt1 ="Layer" + tab + "UreaHy :" + tab + "Nitrif. :" + tab + "Denitrif. :" + tab + "MinerLit :" + tab + "MinerMan :" + tab + "MinerHum";
	String var_txt_xnm_block5_txt2 =tab + "1/t" + tab + tab + "1/t" + tab + tab + "1/t" + tab + tab + "1/t" + tab + tab + "1/t" + tab + tab + "1/t"; 
	String var_txt_xnm_block5_number = ";20002";
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
	String var_txt_xnm_titlea ="GECROS INPUT PARAMETERS (default parameter values)";
	String var_txt_xnm_block1a_txt1 ="Marker for crop type using default values to parameterize GECROS (iGECROS = 0)";
	String var_txt_xnm_block1a_txt2 =";80000 iGECROS";
	String var_txt_xnm_block2a_txt1 ="TypeName" + tab + "Variety" + tab + "Ecotype" + tab + "DaylengthType" + tab + "Leaf Angle Type";
	String var_txt_xnm_block2a_txt2 ="(Code)" + tab + tab + tab +"(Code)" + tab + "(Code)" + tab + tab + "(Code)";
	String var_txt_xnm_block2a_number =";80000";
	String var_txt_xnm_block2a_number_two ="0";
	String var_txt_xnm_block3a_txt1 ="GECROS INPUT PARAMETERS (user specified parameter values)";
	String var_txt_xnm_block4a_txt1 ="Markers for using user specified values to parameterize GECROS";
	String var_txt_xnm_block4a_txt2 =";80001 crop type specification";
	String var_txt_xnm_block4a_txt3 =";80002 - 80004 crop type specific parameters";
	String var_txt_xnm_block4a_txt4 =";80005 - 80006 genotype specific parameters";
	String var_txt_xnm_block4a_txt5 =";80007 - 80008 default and sensitivity parameters";
	String var_txt_xnm_block4a_txt6 =";80009 - 80010 soil parameters";
	String var_txt_xnm_block5a_txt1 ="Crop Type";
	String var_txt_xnm_block5a_txt2 ="legume" + tab + "C3/C4" + tab + "determinate" + tab + "short-/long-day"  + tab + tab + "lodging" + tab + tab + "vernalisation";
	String var_txt_xnm_block5a_txt3 ="+1/-1" + tab + "+1/-1" + tab + "+1/-1" + tab + tab + "+1/-1" + tab + tab + tab + "+1/-1";
	String var_txt_xnm_block5a_number ="80001";
	String var_txt_xnm_block6a_title ="Vegetative and Generative Biomass";
	String var_txt_xnm_block6a_txt1 ="veg.biom." + tab + "veg.biom." + tab + "germin."+ tab + "seed" + tab + "biomass" + tab + "composition" + tab + "fractions";
	String var_txt_xnm_block6a_txt2 ="growth effic." + tab +"carbon frac." + tab + "effic."+ tab + "lipids" + tab + "lignin" + tab + "org.acids" + tab + "minerals";
	String var_txt_xnm_block6a_txt3 ="[g C g-1 C]"+ tab +"[g C g-1]"+ tab +	"[-]"+ tab +"[-]"+ tab + "[-]"+ tab + "[-]" + tab + tab + "[-]";
	String var_txt_xnm_block6a_number ="80002";
	String var_txt_xnm_block7a_title ="Morphology and Phenology";
	String var_txt_xnm_block7a_txt1 ="leaf"+ tab +"stem-dry"+ tab +"max.root"+ tab +"temperature response curve for phenology";
	String var_txt_xnm_block7a_txt2 ="width"+ tab +"weight"+ tab + tab + "depth"+ tab + tab + "base"+ tab +"optimum"+ tab +"ceiling"+ tab + "curvature";
	String var_txt_xnm_block7a_txt3 ="[m]"+ tab +"[g m-2 m-1]"+ tab + "[cm]"+ tab + tab +"[°C]"+ tab +"[°C]"+ tab +"[°C]"+ tab +"[-]";
	String var_txt_xnm_block7a_number ="80003";
	String var_txt_xnm_block8a_title ="Nitrogen";
	String var_txt_xnm_block8a_txt1 ="maximal"+ tab + tab +"minimal"+ tab + tab +"minimal"+ tab + tab +"minimal"+ tab + tab +"initial"+ tab + tab +"specific"+ tab +"N fixation";
	String var_txt_xnm_block8a_txt2 ="N-uptake"+ tab +"root N"+ tab + tab +"stem N"+ tab + tab +"leaf N"+ tab + tab +"leaf N"+ tab + tab + "leaf area"+ tab +"cost";
	String var_txt_xnm_block8a_txt3 ="[g N m-2 d-1]"+ tab +"[g N g-1]"+ tab +"[g N g-1]"+ tab +"[g N m-2]"+ tab +"[g N g-1]"+ tab +"[m2 g-1]"+ tab +"[g C g-1 N] ";
	String var_txt_xnm_block8a_number ="80004";
	String var_txt_xnm_block9a_title ="Photoperiod and Photosynthesis";
	String var_txt_xnm_block9a_txt1 ="Inclination of"+ tab +"Dev.Stage"+ tab +"Dev.Stage"+ tab +"activation"+ tab +"slope"+ tab +"slope"+ tab +tab +"convex.for";
	String var_txt_xnm_block9a_txt2 ="sun angle for"+ tab +"at start of"+ tab +"at end of"+ tab +"energy of"+ tab +"VCMAX"+ tab +"VJMAX"+ tab +tab +"light resp.";
	String var_txt_xnm_block9a_txt3 ="photoperiodic"+ tab +"photosens."+ tab +"photosens."+ tab +"JMAX"+ tab + tab +"vs."+ tab +"vs."+ tab +tab +"electron";
	String var_txt_xnm_block9a_txt4 ="daylength"+ tab +"phase"+ tab + tab +"phase"+ tab + tab +tab +tab +"leaf N"+ tab +"leaf N"+ tab +tab +"transport";
	String var_txt_xnm_block9a_txt5 ="[degree]"+ tab +"[-]"+ tab+tab+"[-]"+ tab + tab +"[J mol-1]"+ tab +"[umol s-1 g-1 N]"+ tab +"[-]";
	String var_txt_xnm_block9a_number ="80005";
	String var_txt_xnm_block10a_title ="Genotype Parameters";
	String var_txt_xnm_block10a_txt1 ="Seed"+ tab + tab +"Seed N"+ tab +tab +"Leaf angle"+ tab +"Max.crop"+ tab +"Min. thermal days of"+ tab +"Photoperiod";
	String var_txt_xnm_block10a_txt2 ="weight"+ tab +tab +"conc."+ tab +tab +"frm.horizontal"+ tab +"height"+ tab + tab +"veg. & reprod. phase"+ tab +"sensitivity";
	String var_txt_xnm_block10a_txt3 ="[g seed-1]"+ tab + "[g N g-1]"+ tab +"[degree]"+ tab +"[m]"+ tab + tab +"[d]"+ tab +"[d]"+ tab + tab +"[h-1]";
	String var_txt_xnm_block10a_number ="80006";
	String var_txt_xnm_block11a_title ="Default Parameters Values of rather high uncertainty";
	String var_txt_xnm_block11a_txt1 ="Fraction of sigmoid"+ tab + tab +"Dev.Stage of seed number"+ tab +"Critical root";
	String var_txt_xnm_block11a_txt2 ="curve inflexion in entire"+ tab +"determining period for"+ tab + tab +"weight density";
	String var_txt_xnm_block11a_txt3 ="height and seed growth period"+ tab +"indeterminate crops";
	String var_txt_xnm_block11a_txt4 ="[-]"+ tab + tab +"[-]"+ tab + tab +"[-]"+ tab + tab + tab + tab +"[g m-2 cm-1 depth]";
	String var_txt_xnm_block11a_number ="80007";
	String var_txt_xnm_block12a_title ="Parameters for sensitivity-analysis options";
	String var_txt_xnm_block12a_txt1 ="Ambient" + tab + tab + tab + "Factors for change in" + tab + tab + tab + "Initial fraction in shoot";
	String var_txt_xnm_block12a_txt2 ="CO2 conc." + tab + "radiation" + tab + "temperature" + tab + "vapour press." + tab + tab + "C" + tab + tab + "N";
	String var_txt_xnm_block12a_txt3 ="[umol mol-1]" + tab + "[-]" + tab + tab + "[°C]" + tab + tab + "[-]" + tab + tab + "[g C g-1 C]" + tab + "[g N g-1 N]"; 
	String var_txt_xnm_block12a_txt4 ="Seed N from" + tab + "Factor N conc. of seed fill" + tab + "Dev.Stage of fastest transition";
	String var_txt_xnm_block12a_txt5 ="non-struct. N" + tab + "initial" + tab + tab + "final" + tab + tab +  "from initial to final factor";
	String var_txt_xnm_block12a_txt6 ="[-]" + tab + tab + "[-]" + tab + tab + "[-]" + tab + tab + tab + "[-]";
	String var_txt_xnm_block12a_number ="80008";
	String var_txt_xnm_block13a_title ="Soil parameters I";
	String var_txt_xnm_block13a_txt1 ="Soil" + tab + "Min." + tab + "Plant" + tab + tab + "Soil" + tab + "upper" + tab + "time constant for" + tab + "Decomp." + tab + tab + "Decomp.";
	String var_txt_xnm_block13a_txt2 ="Clay" + tab + "Water" + tab + "Water" + tab + tab + "profile" + tab + "soil" + tab + "dynamics of soil" + tab + "rate" + tab + tab + "rate";
	String var_txt_xnm_block13a_txt3 ="frac." + tab + "Content" + tab + "Capacity" + tab + "depth" + tab + "depth" + tab + "temp." + tab + "transf." + tab + tab + "microbes" + tab + "humus"; 
	String var_txt_xnm_block13a_txt4 ="[%]" + tab + "[-]" + tab + "[mm]" + tab + tab + "[cm]" + tab + "[cm]" + tab + "[d]" + tab + "[d]" + tab + tab + "[yr-1]" + tab + tab + "[yr-1]";
	String var_txt_xnm_block13a_number =";80009";
	String var_txt_xnm_block14a_title ="Soil parameters II";
	String var_txt_xnm_block14a_txt1 ="Fraction" + tab + "Ratio decomp." + tab + "Decomposition rates" + tab + tab + "Soil total" + tab + "Initial" + tab + tab + "Frac. of";
	String var_txt_xnm_block14a_txt2 ="leaf to" + tab + tab + "to resistant" + tab + "of plant materials" + tab + tab + "organic C" + tab + "value" + tab + tab + "BIO in ";
	String var_txt_xnm_block14a_txt3 ="litter" + tab + tab + "plant mat." + tab + "decomposable  resistant" + tab + tab + "TOC" + tab + tab + "BIO+HUM" + tab + tab + "initial TOC"; 
	String var_txt_xnm_block14a_txt4 ="[-]" + tab + tab + "[-]" + tab + tab + "[g C m-2 d-1][g C m-2 d-1]" + tab + "[g C m-2]" + tab + "[g C m-2]" + tab + "[-]";
	String var_txt_xnm_block14a_number =";80010";
	
	public FileMaker(String cfg, String cropsDb, String mpmasDb){//if db is needed
		gfdb = new GetFromDB(cfg, cropsDb, mpmasDb);
	}
	public FileMaker(){ //if no database connection is needed
	}
/*
	public FileMaker(String cfg, boolean is_subsequent_year){
		gfdb = new GetFromDB(cfg);
		if (is_subsequent_year){
			this.is_subsequent_yr = true;
		}
	}
	*/
	//******************************FROM HERE STARTS THE XNM LOOP OVER CELL GROUPS**********************************************
	public void asciiFileMaker_XNM_loop (String outdir, int simulationPeriod, String file_link_cell_rfs, String cellGroupScenario, String RUN) {	
		try{
			  // Open the file 
			  FileInputStream fstream = new FileInputStream(file_link_cell_rfs);
			  // Get the object of DataInputStream
			  DataInputStream in = new DataInputStream(fstream);
			  BufferedReader br = new BufferedReader(new InputStreamReader(in));
			  String strLine;
			  //Read File Line By Line
			  while ((strLine = br.readLine()) != null)   {
				  // Print the content on the console
				  StringTokenizer st = new StringTokenizer(strLine, "\t");
				  int cellGroup = Integer.parseInt(st.nextToken());
				  int lu_id	=	Integer.parseInt(st.nextToken());
				  
				    
				  //System.out.println(cellGroup);
				  int soil_id = gfdb.getSoilIdForCellGroup(cellGroup, cellGroupScenario);
				  int nextPeriod = simulationPeriod +1;
				  //System.out.println ("G " + cellGroup + "\tF " + rfsFile);
				  //File f = new File (rfsFile);
				  String rfsFile;
				  if (! st.hasMoreTokens()) {
					  System.out.println("Warning: No median rfs file for cellGroup " + cellGroup + "!");
					  gfdb.run_query("DELETE FROM tbl_IMR_cellGroup_updated WHERE model_run = '"+RUN+"' AND cellgroup = " + cellGroup + " AND period = " + nextPeriod);
					  continue;
				  }
				  else {
					  rfsFile = st.nextToken();  
				  }
				  
				  //
				  gfdb.run_query("REPLACE INTO tbl_IMR_cellGroup_updated VALUES('"+RUN+"', " + cellGroup + ", " + nextPeriod + ", 1)");
				  //we will make several versions of the file, one for use with CERES, and one for each GECROS CROP   
				  asciiFileMaker_XNM(outdir + "C"+"P" + nextPeriod + "G"+ cellGroup+".xnm", soil_id, "CERES", nextPeriod, cellGroup, rfsFile, -1, RUN, lu_id);
				  //TODO: CT activate and loop over all crop_id
				   int[] crops = new int[1];
				  	crops[0]=	36;
				   for (int i = 0; i < crops.length; i++) {
					   	int crop_id = crops[i];
						   asciiFileMaker_XNM(outdir + "G"+"C"+crop_id +"P" + nextPeriod + "G"+ cellGroup+".xnm", soil_id,"GECROS", nextPeriod, cellGroup, rfsFile, crop_id, RUN, lu_id);
			  		}
			  }
			  //Close the input stream
			  in.close();
			    }catch (Exception e){//Catch exception if any
			    	System.err.println("Error: " + e.getMessage());
			    	System.exit(0);
			    }
			  
	}	
//******************************FROM HERE STARTS THE XNM FILE**************************************************
		
	public void asciiFileMaker_XNM (String filename, int soilType, String cropGrowthModel, int simulationPeriod, int cellGroup, String rfsFile , int crop_id,  String RUN, int old_lu_id){
	   try{
			String the_file = filename;
			
			boolean usesRFS = false;
			FileInputStream rfsFstream = null;
			DataInputStream rfsIs = null;
			BufferedReader rfsIn = null;
			if ( rfsFile != "-1" ) {
			// Open the file 
				rfsFstream = new FileInputStream(rfsFile);
			  // Get the object of DataInputStream
			  rfsIs = new DataInputStream(rfsFstream);
			  rfsIn = new BufferedReader(new InputStreamReader(rfsIs));
			  usesRFS = true;
			}
			
			if (usesRFS) {
				String rfsLine;

				while ((rfsLine = rfsIn.readLine()) != null) {
						if (rfsLine.matches("^\\s*10011") ) {
							
							//System.out.println("Found results 10011");
							break;
							 
						}
						
				}
				//delete old values for this cellgroup, simulationPeriod and RUN 
				String deletestring = "DELETE FROM tbl_IMR_start_values WHERE model_run = '" + RUN + "' AND cellgroup = " + cellGroup + " AND period = " + simulationPeriod;
				gfdb.run_query(deletestring);
				while ((rfsLine = rfsIn.readLine()) != null) {
					//System.out.println( rfsLine);
					if (rfsLine.matches("^\\s*\\d.*") ) {
						StringTokenizer st = new StringTokenizer(rfsLine, "\t");
						
						String sqlstring = "REPLACE INTO tbl_IMR_start_values VALUES ( '" + RUN + "', " + cellGroup + ", " + simulationPeriod +", " + st.nextToken()
						+ ", " + st.nextToken()+ ", " + st.nextToken()+ ", " + st.nextToken()+ ", " + st.nextToken()+ ", " + st.nextToken()
						+ ", " + st.nextToken()+ ", " + st.nextToken() +");";
						//System.out.println( sqlstring);
						gfdb.run_query(sqlstring);
					}
					else {
						//System.out.println("no match "+rfsLine);
						break;
					}
					
				}
			}
			
			
			
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
			this.writeSoilPhysicalParam(out, soilType);
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
			this.writeSoilHydraulicCurveParam(out, soilType);
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
			//TODO: this.writeMineralizationModelParameters(out, soilType); ; TO DO: Implement in Database GAC
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
			out.write("25.0");
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
			this.writeStart_soil_values_C(out, soilType, usesRFS, rfsIn);
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
			this.writeRate_constant(out, soilType);
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
			this.writeCarbonPoolSizes(out, soilType,usesRFS, rfsIn);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line_five);
			out.write(newLine);
			out.write(var_txt_xnm_block9_title);
			out.write(newLine);
			out.write(var_txt_xnm_block9_txt1);
			out.write(newLine);
			out.write(var_txt_xnm_block9_txt2);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line_five);
			out.write(newLine);
			out.write(var_txt_xnm_block9_number);
			out.write(newLine);
			this.writeNitrogenPoolSizes(out, soilType , usesRFS, rfsIn);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line_four);
			out.write(newLine);
			
			if(	 cropGrowthModel.equalsIgnoreCase("GECROS")	){
				int genotype_id = gfdb.getGenotypeForCrop(crop_id);
				asciiFileMaker_Gecros_File(out,crop_id,genotype_id);
			}
			out.write(var_txt_xnm_block9_txt3);
			out.close();
			if (usesRFS == true) {
				rfsIn.close();
				
			}
		}
	
		catch (Exception ex2) {
			System.out.println("Error in asciiFileMaker_XNM");
			ex2.printStackTrace();
			System.exit(0);
		}

	}
	
	public void writeSoilPhysicalParam(PrintWriter out, int soilId){
		
		try {
			ResultSet rs = gfdb.getSoilPhysicalResultSet(soilId);
			int lastLayerOfPreviousHorizon = 0;
			while(rs.next()){		
				int numLayers = rs.getInt("number_simulation_layers");
				for (int cnt = lastLayerOfPreviousHorizon + 1; cnt <= lastLayerOfPreviousHorizon + numLayers; cnt++) {  
					//Here the Soil physical parameters
					out.write(Integer.toString( cnt));
					out.write(tab);
					out.write(String.valueOf( rs.getString("soil_type")));
					out.write(tab);
					out.write(Double.toString( rs.getDouble("van_genuchten_theta_pF42")));
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
			// TODO Auto-generated catch block
			e.printStackTrace();
			System.exit(0);
		}
	}

	public void writeSoilHydraulicCurveParam(PrintWriter out, int soilId){
		
		try {
			ResultSet rs = gfdb.getSoilHydraulicResultSet(soilId);
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
		// TODO Auto-generated catch block
		e.printStackTrace();
		System.exit(1);
	}
}
public void writeMineralizationModelParameters(PrintWriter out, int soilId){
		
		try {
			ResultSet rs = gfdb.getMineralizationParametersResultSet(soilId);
			int lastLayerOfPreviousHorizon = 0;
			while(rs.next()){
				int numLayers = rs.getInt("number_simulation_layers");
				for (int cnt = lastLayerOfPreviousHorizon + 1; cnt <= lastLayerOfPreviousHorizon + numLayers; cnt++) {
				
				out.write(Integer.toString( cnt));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("")));
				out.write(tab);
			
				out.write(newLine);
				
			}
			lastLayerOfPreviousHorizon += numLayers;
			
			}
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		System.exit(1);
	}
}

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
		// TODO Auto-generated catch block
		e.printStackTrace();
		System.exit(1);
	}
}

	public void writeStart_soil_values_C(PrintWriter out, int soilId, boolean usesRFS, BufferedReader rfsIn){
		
		try {
			
			if (usesRFS) {
				String rfsLine;

				while ((rfsLine = rfsIn.readLine()) != null) {
						if (rfsLine.matches("^\\s*20003") ) {
							//System.out.println("Found results 20003");
							break;
						}
						
				}
				while ((rfsLine = rfsIn.readLine()) != null) {
					//System.out.println( rfsLine);
					if (rfsLine.matches("^\\s*\\d.*") ) {
						//System.out.println("layer 1" + rfsLine);
						out.write(rfsLine + newLine);	
					}
					else {
						//System.out.println("no match "+rfsLine);
						break;
					}
					
				}
			}
			else {
					ResultSet rs = gfdb.getStart_soil_values_C(soilId);
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
			}
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		System.exit(1);
	} catch (IOException ex){
		ex.printStackTrace();
		System.exit(1);
	}
}

	public void writeRate_constant(PrintWriter out, int soilId){
		
		try {
			ResultSet rs = gfdb.getrate_constant(soilId);
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
			// TODO Auto-generated catch block
			e.printStackTrace();
			System.exit(1);
		}
	}	

	public void writeCarbonPoolSizes (PrintWriter out, int soilId, boolean usesRFS, BufferedReader rfsIn) {
		try {
			if (usesRFS) {
				String rfsLine;

				while ((rfsLine = rfsIn.readLine()) != null) {
						if (rfsLine.matches("^\\s*20008") ) {
							//System.out.println("Found results 20008");
							break;
						}	
				}
				while ((rfsLine = rfsIn.readLine()) != null) {
					//System.out.println( rfsLine);
					if (rfsLine.matches("^\\s*\\d.*") ) {
						//System.out.println("layer 1" + rfsLine);
						out.write(rfsLine + newLine);	
					}
					else {
						//System.out.println("no match "+rfsLine);
						break;
					}
					
				}
			}
			else {

				ResultSet rs = gfdb.getOrgCarbon(soilId);
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
			}
		} catch (SQLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			System.exit(1);
		} catch (IOException ex){
			ex.printStackTrace();
			System.exit(1);
		}
		
	}

	public void writeNitrogenPoolSizes (PrintWriter out, int soilId, boolean usesRFS, BufferedReader rfsIn) {
		try {
			if (usesRFS) {
				String rfsLine;

				while ((rfsLine = rfsIn.readLine()) != null) {
						if (rfsLine.matches("^\\s*20009") ) {
							//System.out.println("Found results 20009");
							break;
						}
						
				}
				while ((rfsLine = rfsIn.readLine()) != null) {
					//System.out.println( rfsLine);
					if (rfsLine.matches("^\\s*\\d.*") ) {
						//System.out.println("layer 1" + rfsLine);
						out.write(rfsLine + newLine);	
					}
					else {
						//System.out.println("no match "+rfsLine);
						break;
					}
					
				}
			}
			else {			
				ResultSet rs = gfdb.getOrgCarbonPlusC_N(soilId);
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
						out.write(String.valueOf(Math.round(BOM1*Math.pow(10,2))/Math.pow(10,2)));
						out.write(tab);
						out.write(String.valueOf(Math.round(BOM2*Math.pow(10,2))/Math.pow(10,2)));
						out.write(tab);
						out.write(String.valueOf( BOMD));
						out.write(tab);
						out.write(String.valueOf(Math.round(SOM0*Math.pow(10,2))/Math.pow(10,2)));
						out.write(tab);
						out.write(String.valueOf(Math.round(SOM1*Math.pow(10,2))/Math.pow(10,2)));
						out.write(tab);
						out.write(String.valueOf( SOM2));
						out.write(newLine);
				   
				   }
				   lastLayerOfPreviousHorizon += numLayers; 	   
				}
			}	
		} catch (SQLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			System.exit(1);
		} catch (IOException ex){
			ex.printStackTrace();
			System.exit(1);
		}
	}
//******************** XNP FILE
	public void asciiFileMaker_XNP(String xnpfilename, String xndfilename) {
		String filename = xnpfilename + ".xnp";
		try{
			PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(filename)));
			out.write("------------------------------");
			out.write(newLine);
			out.write("***Automatic project definition generated by xnfilemaker");
			out.write(newLine);
			out.write("------------------------------");
			out.write(newLine);
			out.write("111	AutoXNP");
			out.write(newLine);
			out.write("1110	0");
			out.write(newLine);
			out.write("10111 " + xndfilename + ".xnd");
			out.write(newLine);
			out.write("222");
			out.write(newLine);
			out.close();
		}
		catch (IOException ex){
			ex.printStackTrace();
			System.exit(1);
		}

	}
	public void asciiFileMaker_XNI(String projectdir, String outfile) {
		try {
			PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter("ExpertN.xni")));

			String strXniText = "[ExpertN]" + newLine + newLine;
			strXniText = strXniText + ";1. Auswahl des Typs der Datenquelle" + newLine;
			strXniText = strXniText + ";-----------------------------------" + newLine;
			strXniText = strXniText + ";a) Fuer ASCII Files:" + newLine;
			strXniText = strXniText + ";   DataSource=File/ASCII" + newLine;
			strXniText = strXniText + ";b) Fuer MS Access Datenbank:" + newLine;
			strXniText = strXniText + ";   DataSource=DataBase/ODBC" + newLine;
			strXniText = strXniText + ";DataSource=DataBase/ODBC" + newLine;
			strXniText = strXniText + "DataSource=File/ASCII" + newLine + newLine;
			strXniText = strXniText + ";2. Ort der Eingabedateien sowie Ergebnisdateien" + newLine;
			strXniText = strXniText + ";-----------------------------------------------" + newLine;
			strXniText = strXniText + ";a) Fuer ASCII Files und MS Access Datenbank:" + newLine;
			strXniText = strXniText + ";   Angabe des Projekt Verzeichnisses. Dort befinden sich die ASCII Eingabedateien." + newLine;
			strXniText = strXniText + ";   Unterhalb des Projekt-Verzeichnisses werden ausserdem die Ergebnisdateien abgelegt." + newLine;
			strXniText = strXniText + ";   z.B. ProjectDir=C:\\Projekte\\ExpertN\\3.0\\ExpertN\\Example" + newLine;
			strXniText = strXniText + ";b) Nur fuer MS Access Datenbank:" + newLine;
			strXniText = strXniText + ";   Angabe des Providers" + newLine;
			strXniText = strXniText + ";   z.B. DSN=Microsoft Access-Datenbank" + newLine;
			strXniText = strXniText + ";c) Nur fuer MS Access Datenbank:" + newLine;
			strXniText = strXniText + ";   Angabe des Datenbank-Namens." + newLine;
			strXniText = strXniText + ";   z.B. DBQ=C:\\Projekte\\ExpertN\\3.0\\Database\\xn_daten.mdb" + newLine;
			strXniText = strXniText + "ProjectDir=" + projectdir + newLine;
			strXniText = strXniText + "DSN=MS Access Database" + newLine;
			strXniText = strXniText + "DBQ=C:\\Program Files\\Expert-N 3.0\\dat\\xn_daten.mdb" + newLine + newLine;
			strXniText = strXniText + ";3. Zu ladendes (zu letzt verwendetes) Projekt" + newLine;
			strXniText = strXniText + ";---------------------------------------------" + newLine;
			strXniText = strXniText + ";a) Fuer ASCII Files:" + newLine;
			strXniText = strXniText + ";   Angabe der Datei, die die Projekt-Beschreibung enthaelt. Diese Datei" + newLine;
			strXniText = strXniText + ";   muss im Projekt-Verzeichnisses liegen." + newLine;
			strXniText = strXniText + ";   z.B. LastProject=130180.xnp" + newLine;
			strXniText = strXniText + ";c) Fuer MS Access Datenbank:" + newLine;
			strXniText = strXniText + ";   Angabe des zu verwendenden Simulationsprojekts (Name innerhalb der " + newLine;
			strXniText = strXniText + ";   Tabelle \"Projekt\" in der Datenbank)." + newLine;
			strXniText = strXniText + ";   z.B. LastProject=130180" + newLine;
			strXniText = strXniText + "LastProject=" + outfile + ".xnp" + newLine;
			out.write(strXniText);
			out.close();
		}
		catch (IOException ex){
			ex.printStackTrace();
			System.exit(1);
		}
	}
//******************************FROM HERE STARTS THE WEATHER FILE**********************************************
	public void asciiFileMaker_XNW_loop(String outdir, int weather_station, int start_year, int end_year) {
		for (int y = start_year; y <= end_year; y++ ) {
			System.out.println(y);
				String filename = outdir + "station_" + weather_station + "_y" + y+ ".xnw"; 
				asciiFileMaker_XNW(filename, weather_station, y);
		}
	}
	public void asciiFileMaker_XNW(String outdir, int weatherFileId) {
		int station = gfdb.getStationForWeatherfile(weatherFileId);
		int year = gfdb.getYearForWeatherfile(weatherFileId);
		String filename = gfdb.getWeatherfile(weatherFileId);
		asciiFileMaker_XNW(outdir + filename + ".xnw", station, year);
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
			out.write(gfdb.getStation_name(7));
			out.write(tab);
			out.write(gfdb.getStation_latitude(7));
			out.write(tab);
			out.write(gfdb.getStation_longitude(7));
			out.write(tab);
			out.write(gfdb.getStation_altitude(7));
			out.write(tab);
			out.write(gfdb.getStation_avg_temperature(7));
			out.write(tab);
			out.write(gfdb.getStation_temp_amplitude(7));
			out.write(tab);
			out.write(gfdb.getStation_temp_measurement(7));
			out.write(tab);
			out.write(gfdb.getStation_wind_measurement(7));
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_line_five);
			out.write(newLine);
			out.write(var_txt_general_xnw_block2_txt1);
			out.write(newLine);
			out.write(newLine);
			out.write (var_txt_xnw_block2_number);
			out.write(tab);
			
			int no_simulation_days_per_station= gfdb.getnumber_simulation_days(7, second_year);
			
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
	
	public void writeWeatherData(PrintWriter out, int station, int second_year){
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
			// TODO Auto-generated catch block
			e.printStackTrace();
			System.exit(1);
		}
	}
	
//******************************Auxiliary routine called by Xnwrapper to read a static yield from the database
	
	public void direct_yield_query(String outfile, int lu_id, int soil_id, int weatherfileID) {
		try {
			String the_file = outfile + ".yld"; 
			//file specific text elements initialized
			PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(the_file)));
			
			
			ResultSet rs = gfdb.get_static_yield(lu_id, soil_id, weatherfileID);
			while(rs.next()){
				out.write(String.format("%.3f", rs.getDouble("yield")));
				out.write(newLine);
			}
			out.close();
		}catch (Exception ex2) {
			System.out.println("Error in direct_yield_query");
			ex2.printStackTrace();
			System.exit(1);
		}
	}

//******************************FROM HERE START THE XND FILE ROUTINES**************************************************	
	public void asciiFileMaker_XND_loop (String outdir, String table, String cropGrowthModel, boolean exact_dates ){
		try {
			ResultSet rs = gfdb.get_xnd_files_to_create(table);
				
			while(rs.next()){
				asciiFileMaker_XND(outdir + rs.getString("filename"),rs.getString("resultID"), rs.getInt("lu_id"), rs.getInt("soil_id"), rs.getInt("weather_id"), rs.getInt("config_id"), -1, -1, cropGrowthModel, "TEST" , exact_dates);
		
			}
		} catch (Exception ex2) {
			System.out.println("Error in asciiFileMaker_XND_loop");
			ex2.printStackTrace();
			System.exit(1);
		}
	}
	
	public void asciiFileMaker_XND(String outfile, String resultID, int lu_id, int soil_id, int weatherfileID, int xn_config, int cellGroup, int simulationPeriod, String cropGrowthModel, String RUN, boolean exact_dates){
		
		try {
				String the_file = outfile + ".xnd"; 
				String cropGrowthModelLetter = "C";
				if (cropGrowthModel == "GECROS" || xn_config == 2) {
					cropGrowthModelLetter = "G";
				}
			//file specific text elements initialized
				PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(the_file)));
	
				int crop_id	=	gfdb.getPak_crops_crop_id_by_ktblmilp_process_id(lu_id);
				int seed_choice	=	gfdb.getPak_crops_seed_choice_by_ktblmilp_process_id(lu_id);
				this.buildBlock_simulation_data(out, crop_id, exact_dates, seed_choice);
				
				this.buildBlock_farm_data(out);
				this.buildBlock_plot_data(out);
				this.buildBlock_subplot_data(out, resultID, crop_id, weatherfileID, soil_id, xn_config, RUN, cellGroup, simulationPeriod, cropGrowthModelLetter);
				this.buildBlock_preceding_crop(out);
				this.buildBlock_cultivated_crop (out, crop_id, lu_id, seed_choice);
				this.buildBlock_mineral_fert(out, lu_id);
				this.buildBlock_organic_fert(out, lu_id);
				this.buildBlock_soil_management(out, lu_id);
				this.buildBlock_irrigation(out);
				this.buildBlock_soilprofiles(out, soil_id);
				this.buildBlock_sample_soilprofiles(out, crop_id,  soil_id, RUN, cellGroup, simulationPeriod, seed_choice);
				this.buildBlock_water_readings(out);
				this.buildBlock_nitrogen_readings(out);
				this.buildBlock_plant_measurementsI(out);
				this.buildBlock_plant_measurementsII(out);
				this.buildBlock_closing_block(out);
				
			
		}catch (Exception ex2) {
				System.out.println("Error in asciiFileMaker_XND");
				ex2.printStackTrace();
				System.exit(1);
		}
		
	}
	
	
	
	public void buildBlock_simulation_data(PrintWriter out, int crop_id, boolean exact_dates, int seed_choice){
		try {
			buildBlock_header_simulation_data (out);
			out.write(gfdb.getStartDate3(crop_id, seed_choice));
			out.write(tab);
			out.write(gfdb.getEndDate3(crop_id, seed_choice));
			out.write(newLine);
			out.write(newLine);


		} catch (Exception e) {
			// TODO Auto-generated catch block
			System.out.println("Error in buildBlock_simulation_data" + e);
			System.exit(1);
		}
	}

	public void buildBlock_header_simulation_data(PrintWriter out){
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
			System.out.println("Error in header_buildBlock_simulation_data" + e);
			System.exit(1);
		}
	}
	
	public void buildBlock_farm_data(PrintWriter out){
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
		// TODO Auto-generated catch block
		System.out.println("Error in buildBlock_farm_data" + e);
		System.exit(1);
	}
}
	
	public void buildBlock_header_farm_data(PrintWriter out){
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
			System.exit(1);
		}
	}
	
	public void buildBlock_plot_data (PrintWriter out){
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
			// TODO Auto-generated catch block
			System.out.println("Error in buildBlock_plot_data " + e);
			System.exit(1);
		}
	}
	
	public void buildBlock_header_plot_data(PrintWriter out){
		try {
			out.write(var_txt_general_MCK001_xnd_line);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block3_title);
			out.write(newLine);
		} catch (Exception e) {
			System.out.println("Error in buildBlock_plot_data " + e);
			System.exit(1);
		}
	}
	
	public void buildBlock_subplot_data (PrintWriter out, String resultID, int crop_id, int weather_id, int soil_id, int xn_config, String RUN, int cellGroup, int simulationPeriod, String cropGrowthModelLetter){
		try {

			buildBlock_header_subplot_data(out);
			out.write(var_txt_MCK001_xnd_block4_number);
			out.write(newLine);
			out.write(var_txt_general_MCK001_xnd_block4_txt1);
			out.write(newLine);
			out.write(gfdb.getSubPlotID(4));
			out.write(tab);
			out.write(resultID);
			out.write(tab);
			out.write(gfdb.getPrinc_Crop(crop_id));
			out.write(tab);
			out.write(gfdb.getWeatherfile(weather_id)+ ".xnw");
			out.write(tab);
			
			
			//
			//110707 CT/GC
			
			
			
			if (simulationPeriod > 1 & cellGroup > -1 & gfdb.soil_information_has_been_updated(RUN, cellGroup, simulationPeriod) ) {
				if (xn_config == 3) {	
					String gfilename = cropGrowthModelLetter + "P"+ simulationPeriod + "G" + cellGroup + ".xnm";
					out.write(gfilename);
				}
				else if (xn_config == 2) {
					String gfilename = cropGrowthModelLetter +  "C" + crop_id + "P"+ simulationPeriod + "G" + cellGroup + ".xnm";	
					out.write(gfilename);
				}
				
			}
			else if (xn_config == 2) { //German with GECROS
				out.write(gfdb.getParameterfile(soil_id, crop_id) + ".xnm");
			}

			else if (xn_config == 1 || xn_config == 3 ) {
				out.write(gfdb.getParameterfile(soil_id) + ".xnm");
			}
			else {
				out.write(gfdb.getParameterfile(soil_id, crop_id, xn_config) + ".xnm");
			}
			
			
			//TODO: deactivate
			//out.write(gfdb.getParameterfile(soil_id) + ".xnm");
			
			out.write(tab);
			out.write(gfdb.getXnConfigfile(xn_config)+ ".xnc");
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
			// TODO Auto-generated catch block
			System.out.println("Error in buildBlock_subplot_data " + e);
			System.exit(1);
		}
	}
	
	public void buildBlock_header_subplot_data(PrintWriter out){
		try {
			out.write(var_txt_general_MCK001_xnd_block4_title);
			out.write(newLine);
		} catch (Exception e) {
			System.out.println("Error in buildBlock_subplot_data " + e);
			System.exit(1);
		}
	}
					
	public void buildBlock_preceding_crop (PrintWriter out){
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
			// TODO Auto-generated catch block
			System.out.println("Error in buildBlock_preceding_crop " + e);
			System.exit(1);
		}
	}

	public void buildBlock_header_preceding_crop(PrintWriter out){
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
			System.exit(1);
		}
	}
	
	public void buildBlock_cultivated_crop (PrintWriter out, int crop_id, int process_id, int seed_choice){
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
			
			ResultSet mcchar = gfdb.get_cultivated_crop_characteristics_by_crop_id(crop_id, seed_choice);
			ResultSet mcharvest = gfdb.get_cultivated_crop_harvestdate(process_id);
			ResultSet mcseed = gfdb.get_cultivated_crop_seeddate(process_id);
			mcchar.first();
			mcharvest.first();
			mcseed.first();
			
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
			// TODO Auto-generated catch block
			System.out.println("Error in buildBlock_cultivated_crop " + e);
			System.exit(1);
		}
	}
		
	public void buildBlock_header_cultivated_crop(PrintWriter out){
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
			System.out.println("Error in buildBlock_header_cultivated_crop " + e);
			System.exit(1);
		}
	}
		
	public void buildBlock_mineral_fert (PrintWriter out, int process_id){
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
			// TODO Auto-generated catch block
			System.out.println("Error in buildBlock_mineral_fert " + e);
			System.exit(1);
		}
	}
		
	public void buildBlock_header_mineral_fert(PrintWriter out){
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
			System.out.println("Error in buildBlock_header_mineral_fert " + e);
			System.exit(1);
		}
	}
		
	public void buildBlock_organic_fert(PrintWriter out, int process_id){
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
			// TODO Auto-generated catch block
			System.out.println("Error in buildBlock_organic_fert " + e);
			System.exit(1);
		}
	}
		
	public void buildBlock_header_organic_fert(PrintWriter out){
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
			System.out.println("Error in buildBlock_header_organic_fert " + e);
			System.exit(1);
		}
	}
		
	public void buildBlock_soil_management(PrintWriter out, int process_id){
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
			// TODO Auto-generated catch block
			System.out.println("Error in buildBlock_soil_management " + e);
			System.exit(1);
		}
	}
	
	public void buildBlock_header_soil_management(PrintWriter out){
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
			System.out.println("Error in buildBlock_header_soil_management " + e);
			System.exit(1);
		}
	}
		
	public void buildBlock_irrigation(PrintWriter out){
		try {
			buildBlock_header_irrigation(out);
			out.write(var_txt_MCK001_xnd_block10_number);
			out.write(tab);
			
			int no_irrigation_activities = 0;
			//int no_irrigation_activities= gfdb.getIrrigation_activities(1);
			
			out.write(String.valueOf(no_irrigation_activities)); //About the number of Irrigation Activities
		/*	
			out.write(tab);
			out.write(var_txt_general_MCK001_xnd_block10_txt3);
			out.write(newLine);
			out.write(gfdb.getIrrigation_date(1,1,1,2008));
			out.write(tab);
			out.write (gfdb.getIrrigation_amount());
			out.write(tab);
			out.write (gfdb.getIrrigation_code());
			*/
			out.write(newLine);
			out.write(newLine);
			
		} catch (Exception e) {
			// TODO Auto-generated catch block
			System.out.println("Error in buildBlock_irrigation " + e);
			System.exit(1);
		}
	}
		
	public void buildBlock_header_irrigation(PrintWriter out){
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
			System.exit(1);
		}
	}
		
	public void buildBlock_soilprofiles(PrintWriter out, int soil_id){
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
			System.out.println("Error in buildBlock_soilprofiles " + e);
			System.exit(1);
		}
	}
		
	public void buildBlock_header_soilprofiles(PrintWriter out){
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
	
	public void buildBlock_sample_soilprofiles(PrintWriter out, int crop_id, int soil_id, String RUN, int cellGroup, int simulationPeriod, int seed_choice){//INITIAL VALUES
		try {
			
		
			
		
			this.buildBlock_header_sample_soilprofiles(out);
			//out.write(gfdb.()); Get the Number of Horizons	
			out.write(var_txt_MCK001_xnd_block12_number);
			out.write(tab);
			if (simulationPeriod > 1 & cellGroup > -1 &  gfdb.soil_information_has_been_updated(RUN, cellGroup, simulationPeriod) ) {
				
				int no_layers = gfdb.getnumber_of_layers(RUN, cellGroup, simulationPeriod);
				
				out.write(gfdb.getStartDate3(crop_id, seed_choice));
				out.write(tab);
				out.write(String.valueOf(no_layers));
				out.write(tab);
				out.write(var_txt_general_MCK001_xnd_block12_txt5);
				out.write(newLine);
				
				//System.out.println(no_sample_layers_per_horizon);
				for(int x=1;x<=no_layers;x++){
					ResultSet rs = gfdb.get_start_values(RUN, cellGroup, simulationPeriod, x);
					
					rs.first();
					
					out.write(String.valueOf(x));
					out.write(tab);
					out.write (String.valueOf(rs.getInt("number_simulation_layers")));
					out.write(tab);
					out.write (String.valueOf(rs.getDouble("water_cont")));
					out.write(tab);
					out.write (String.valueOf(rs.getDouble("matrix_pot")));
					out.write(tab);
					out.write (String.valueOf(rs.getDouble("soil_temp")));
					out.write(tab);
					out.write (String.valueOf(rs.getDouble("NH4")));
					//out.write ("0.00");
					out.write(tab);
					out.write (String.valueOf(rs.getDouble("NO3")));
					//out.write ("0.00");
					out.write(tab);
					out.write (String.valueOf(rs.getDouble("root_density")));
					out.write(newLine);
					
				}
			}
			else {
				
				int no_sample_layers_per_horizon= gfdb.getnumber_layer_per_horizon_sample(soil_id);//START VALUES
				// CT 100408 replaced out.write(gfdb.getsoil_sample_date(soil_id)); by out.write(gfdb.getsoil_sample_date_static(soil_id));
				// -> fixed the sample date so it fits to the new start date 010801 for all simulations
				out.write(gfdb.getStartDate3(crop_id, seed_choice));
				
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
					//out.write (gfdb.getNH4(soil_id,x));
					out.write ("0.00");
					out.write(tab);
					//CT 100427 adapted
					//out.write (gfdb.getNO3(soil_id,x));
					out.write ("0.00");
					out.write(tab);
					out.write (gfdb.getroot_density(soil_id,x));
					out.write(newLine);
				}
			}
	
		} catch (Exception e) {
			System.out.println("Error in buildBlock_sample_soilprofiles " + e);
		}
	}
	
	public void buildBlock_header_sample_soilprofiles(PrintWriter out){
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
	
	public void buildBlock_water_readings(PrintWriter out){
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
	
	public void buildBlock_header_water_readings(PrintWriter out){
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
	
	public void buildBlock_nitrogen_readings(PrintWriter out){
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
	
	public void buildBlock_header_nitrogen_readings(PrintWriter out){
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
	
	public void buildBlock_plant_measurementsI(PrintWriter out){
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

	public void buildBlock_header_plant_measurementsI(PrintWriter out){
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
	
	public void buildBlock_plant_measurementsII(PrintWriter out){
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
	
	public void buildBlock_header_plant_measurementsII(PrintWriter out){
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
	
	public void buildBlock_closing_block (PrintWriter out){
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

//******************************FROM HERE STARTS THE GECROS INPUT FILE**********************************************
	
	public void asciiFileMaker_Gecros_File (PrintWriter out, int crop_id, int genotype_id){
		
		//String the_file = filename; 
		
			try{
			
		//	PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(the_file)));
			
	out.write(var_txt_general_MCK001_xnd_line);
	out.write(newLine);
	out.write(var_txt_xnm_titlea);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line);
	out.write(newLine);
	out.write(var_txt_xnm_block1a_txt1);
	out.write(newLine);
	out.write(var_txt_xnm_block1a_txt2);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line);
	out.write(newLine);
	out.write(var_txt_xnm_block2a_txt1);
	out.write(newLine);
	out.write(var_txt_xnm_block2a_txt2);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line_five);
	out.write(newLine);
	out.write(var_txt_xnm_block2a_number);
	out.write(tab);
	out.write(var_txt_xnm_block2a_number_two);
	out.write(newLine);
	this.writeCrop_Gecros_parameters(out, crop_id, genotype_id);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line_five);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line);
	out.write(newLine);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line);
	out.write(newLine);
	out.write(var_txt_xnm_block3a_txt1);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line);
	out.write(newLine);
	out.write(var_txt_xnm_block4a_txt1);
	out.write(newLine);
	out.write(var_txt_xnm_block4a_txt2);
	out.write(newLine);
	out.write(var_txt_xnm_block4a_txt3);
	out.write(newLine);
	out.write(var_txt_xnm_block4a_txt4);
	out.write(newLine);
	out.write(var_txt_xnm_block4a_txt5);
	out.write(newLine);
	out.write(var_txt_xnm_block4a_txt6);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line);
	out.write(newLine);
	out.write(var_txt_xnm_block5a_txt1);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line_five);
	out.write(newLine);
	out.write(var_txt_xnm_block5a_txt2);
	out.write(newLine);
	out.write(var_txt_xnm_block5a_txt3);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line_five);
	out.write(newLine);
	out.write(var_txt_xnm_block5a_number);
	out.write(newLine);
	this.writeCrop_Type_parameters(out, crop_id);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line_six);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line_four);
	out.write(newLine);
	out.write(var_txt_xnm_block6a_title);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line_six);
	out.write(newLine);
	out.write(var_txt_xnm_block6a_txt1);
	out.write(newLine);
	out.write(var_txt_xnm_block6a_txt2);
	out.write(newLine);
	out.write(var_txt_xnm_block6a_txt3);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line_six);
	out.write(newLine);
	out.write(var_txt_xnm_block6a_number);
	out.write(newLine);
	this.writeVegetative_Generative_Biomass_data(out, crop_id);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line_six);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line_four);
	out.write(newLine);
	out.write(var_txt_xnm_block7a_title);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line_six);
	out.write(newLine);
	out.write(var_txt_xnm_block7a_txt1);
	out.write(newLine);
	out.write(var_txt_xnm_block7a_txt2);
	out.write(newLine);
	out.write(var_txt_xnm_block7a_txt3);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line_six);
	out.write(newLine);
	out.write(var_txt_xnm_block7a_number);
	out.write(newLine);
	this.writeMorphology_phenology(out, crop_id);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line_six);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line_four);
	out.write(newLine);
	out.write(var_txt_xnm_block8a_title);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line_six);
	out.write(newLine);
	out.write(var_txt_xnm_block8a_txt1);
	out.write(newLine);
	out.write(var_txt_xnm_block8a_txt2);
	out.write(newLine);
	out.write(var_txt_xnm_block8a_txt3);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line_six);
	out.write(newLine);
	out.write(var_txt_xnm_block8a_number);
	out.write(newLine);
	this.writeNitrogen_Gecros(out, crop_id);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line_six);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line_four);
	out.write(newLine);
	out.write(var_txt_xnm_block9a_title);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line_six);
	out.write(newLine);
	out.write(var_txt_xnm_block9a_txt1);
	out.write(newLine);
	out.write(var_txt_xnm_block9a_txt2);
	out.write(newLine);
	out.write(var_txt_xnm_block9a_txt3);
	out.write(newLine);
	out.write(var_txt_xnm_block9a_txt4);
	out.write(newLine);
	out.write(var_txt_xnm_block9a_txt5);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line_six);
	out.write(newLine);
	out.write(var_txt_xnm_block9a_number);
	out.write(newLine);
	this.writePhotoper_Pothosyn(out, crop_id);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line_six);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line_four);
	out.write(newLine);
	out.write(var_txt_xnm_block10a_title);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line_six);
	out.write(newLine);
	out.write(var_txt_xnm_block10a_txt1);
	out.write(newLine);
	out.write(var_txt_xnm_block10a_txt2);
	out.write(newLine);
	out.write(var_txt_xnm_block10a_txt3);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line_six);
	out.write(newLine);
	out.write(var_txt_xnm_block10a_number);
	out.write(newLine);
	this.writeGenotype_parameters(out, crop_id, genotype_id);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line_six);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line_four);
	out.write(newLine);
	out.write(var_txt_xnm_block11a_title);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line_six);
	out.write(newLine);
	out.write(var_txt_xnm_block11a_txt1);
	out.write(newLine);
	out.write(var_txt_xnm_block11a_txt2);
	out.write(newLine);
	out.write(var_txt_xnm_block11a_txt3);
	out.write(newLine);
	out.write(var_txt_xnm_block11a_txt4);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line_six);
	out.write(newLine);
	out.write(var_txt_xnm_block11a_number);
	out.write(newLine);
	this.writeDefault_parameters(out, crop_id, genotype_id);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line_six);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line_four);
	out.write(newLine);
	out.write(var_txt_xnm_block12a_title);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line_six);
	out.write(newLine);
	out.write(var_txt_xnm_block12a_txt1);
	out.write(newLine);
	out.write(var_txt_xnm_block12a_txt2);
	out.write(newLine);
	out.write(var_txt_xnm_block12a_txt3);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line_six);
	out.write(newLine);
	out.write(var_txt_xnm_block12a_txt4);
	out.write(newLine);
	out.write(var_txt_xnm_block12a_txt5);
	out.write(newLine);
	out.write(var_txt_xnm_block12a_txt6);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line_six);
	out.write(newLine);
	out.write(var_txt_xnm_block12a_number);
	out.write(newLine);
	this.writeSensitivity_analysis_param(out, crop_id, genotype_id);
	this.writeSensitivity_analysis_param_1(out, crop_id, genotype_id );
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line_six);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line_four);
	out.write(newLine);
	out.write(var_txt_xnm_block13a_title);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line_six);
	out.write(newLine);
	out.write(var_txt_xnm_block13a_txt1);
	out.write(newLine);
	out.write(var_txt_xnm_block13a_txt2);
	out.write(newLine);
	out.write(var_txt_xnm_block13a_txt3);
	out.write(newLine);
	out.write(var_txt_xnm_block13a_txt4);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line_six);
	out.write(newLine);
	out.write(var_txt_xnm_block13a_number);
	out.write(newLine);
	this.writeSoil_param_I(out, 9, genotype_id);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line_six);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line_four);
	out.write(newLine);
	out.write(var_txt_xnm_block14a_title);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line_six);
	out.write(newLine);
	out.write(var_txt_xnm_block14a_txt1);
	out.write(newLine);
	out.write(var_txt_xnm_block14a_txt2);
	out.write(newLine);
	out.write(var_txt_xnm_block14a_txt3);
	out.write(newLine);
	out.write(var_txt_xnm_block14a_txt4);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line_six);
	out.write(newLine);
	out.write(var_txt_xnm_block14a_number);
	out.write(newLine);
	this.writeSoil_param_II(out, 9, 1);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line_six);
	out.write(newLine);
	out.write(var_txt_general_MCK001_xnd_line_four);
	out.close();
	
			}
		
			catch (Exception ex2) {
				System.out.println("Error in asciiFileMaker_Gecros_File");
				ex2.printStackTrace();
			}

		}
	
	public void writeCrop_Gecros_parameters(PrintWriter out, int crop_id, int genotype_id){
		
		try {
				ResultSet rs = gfdb.getCrop_Gecros_parameters(crop_id, genotype_id);
				
			while(rs.next()){
				out.write(rs.getString("xn_code"));
				out.write(tab);
				out.write(rs.getString("crop_name_english"));
				out.write(tab);
				out.write(rs.getString("genotype_txt"));
				out.write(tab);
				out.write(rs.getString("ecotype"));
				out.write(tab);
				out.write(rs.getString("daylengthtype"));
				out.write(tab);
				out.write(rs.getString("leafangletype"));
				out.write(newLine);
				}
			} catch (SQLException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
	
	public void writeCrop_Type_parameters(PrintWriter out, int crop_id){
		
		try {
				ResultSet rs = gfdb.getCrop_Type_parameters(crop_id);
				
			while(rs.next()){
				out.write(rs.getString("legume"));
				out.write(tab);
				out.write(rs.getString("photosynthesistype_txt"));
				out.write(tab);
				out.write(rs.getString("determinate"));
				out.write(tab);
				out.write(rs.getString("short_day"));
				out.write(tab);
				out.write(rs.getString("with_lodging"));
				out.write(tab);
				out.write(rs.getString("vernalisation"));
				out.write(newLine);
				}
			} catch (SQLException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
	
	public void writeVegetative_Generative_Biomass_data(PrintWriter out, int crop_id){
		
		try {
				ResultSet rs = gfdb.getVegetative_Generative_Biomass_data(crop_id);
				
			while(rs.next()){
				out.write(Double.toString( rs.getDouble("veg_biom_growth_effic")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("veg_biom_carbon_frac")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("germin_effic")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("seed_lipids")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("seed_lignin")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("composition_org_acids")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("fractions_minerals")));
				out.write(newLine);
				}
			} catch (SQLException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		
	public void writeMorphology_phenology(PrintWriter out, int crop_id){
		
	try {
			ResultSet rs = gfdb.getMorphology_phenology_data(crop_id);
			
		while(rs.next()){
			out.write(Double.toString( rs.getDouble("leaf_width")));
			out.write(tab);
			out.write(Double.toString( rs.getDouble("stem_dry_weight")));
			out.write(tab);
			out.write(Double.toString( rs.getDouble("max_root_depth")));
			out.write(tab);
			out.write(Double.toString( rs.getDouble("temp_respon_base")));
			out.write(tab);
			out.write(Double.toString( rs.getDouble("temp_respon_optimum")));
			out.write(tab);
			out.write(Double.toString( rs.getDouble("temp_respon_ceiling")));
			out.write(tab);
			out.write(Double.toString( rs.getDouble("temp_phenology_curve")));
			out.write(newLine);
			}
		} catch (SQLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
			
	public void writeNitrogen_Gecros(PrintWriter out, int crop_id){
	
	try {
			ResultSet rs = gfdb.getNitrogen_Gecros(crop_id);
			
		while(rs.next()){
			out.write(Double.toString( rs.getDouble("max_n_uptake")));
			out.write(tab);
			out.write(Double.toString( rs.getDouble("min_root_n")));
			out.write(tab);
			out.write(Double.toString( rs.getDouble("min_steam_n")));
			out.write(tab);
			out.write(Double.toString( rs.getDouble("min_leaf_n")));
			out.write(tab);
			out.write(Double.toString( rs.getDouble("initial_leaf_n")));
			out.write(tab);
			out.write(Double.toString( rs.getDouble("specif_leaf_area")));
			out.write(tab);
			out.write(Double.toString( rs.getDouble("n_fix_cost")));
			out.write(newLine);	
		}
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
	}
	}
	
	public void writePhotoper_Pothosyn(PrintWriter out, int crop_id){
		
		try {
				ResultSet rs = gfdb.getPhotoperiod_photosyn(crop_id);
				
			while(rs.next()){
				out.write(Double.toString( rs.getDouble("inclination_sun_angle_photoperiodic_daylength")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("dev_stage_start_photo_phase")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("dev_stage_end_photo_phase")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("activation_energy_jmax")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("slopevcmax_leaf_n")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("slopevjmax_leaf_n")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("convex_light_resp_electron_transport")));
				out.write(newLine);	
			}
		} catch (SQLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}	
}

	public void writeGenotype_parameters(PrintWriter out, int crop_id, int genotype_id){
	
	try {
			ResultSet rs = gfdb.getGenotype_parameters(crop_id, genotype_id);
			
		while(rs.next()){
			out.write(Double.toString( rs.getDouble("seed_weight")));
			out.write(tab);
			out.write(Double.toString( rs.getDouble("seed_n_concen")));
			out.write(tab);
			out.write(Double.toString( rs.getDouble("leaf_angle_frm_horizontal")));
			out.write(tab);
			out.write(Double.toString( rs.getDouble("max_height")));
			out.write(tab);
			out.write(Double.toString( rs.getDouble("min_thermal_days_veg")));
			out.write(tab);
			out.write(Double.toString( rs.getDouble("min_thermal_days_reprod")));
			out.write(tab);
			out.write(Double.toString( rs.getDouble("photoper_sensitivity")));
			out.write(newLine);	
		}
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
	}	
}
	
	public void writeDefault_parameters(PrintWriter out, int crop_id, int genotype_id){
		
		try {
				ResultSet rs = gfdb.getDefault_parameters(crop_id, genotype_id);
				
			while(rs.next()){
				out.write(Double.toString( rs.getDouble("sig_curve_inflex_height")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("sig_curve_inflex_seed_growth")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("dev_stage_seed_num_indeter_crops")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("crit_root_weight_den")));
				out.write(newLine);	
			}
		} catch (SQLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}	
	}

	public void writeSensitivity_analysis_param(PrintWriter out, int crop_id, int genotype_id){
		
		try {
				ResultSet rs = gfdb.getSensitivity_analysis_param(crop_id, genotype_id);
				
			while(rs.next()){
				out.write(Double.toString( rs.getDouble("CO2A")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("COEFR")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("COEFT")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("COEFV")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("FCRSH")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("FNRSH")));
				out.write(newLine);	
			}
		} catch (SQLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}	
	}
	
	public void writeSensitivity_analysis_param_1(PrintWriter out, int crop_id, int genotype_id){
		
		try {
				ResultSet rs = gfdb.getSensitivity_analysis_param(crop_id, genotype_id);
				
			while(rs.next()){
				out.write(Double.toString( rs.getDouble("PNPRE")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("CB")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("CX")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("TM")));
				out.write(newLine);	
			}
		} catch (SQLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}	
	}

	public void writeSoil_param_I(PrintWriter out, int soil_id, int layer_id){
		
		try {
						
			ResultSet rs = gfdb.getSoil_param_I(soil_id, layer_id);
				
			while(rs.next()){
				
				out.write(Double.toString( rs.getDouble("clay_frac")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("min_water_content")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("plant_water_capacity")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("soil_profile_depth")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("upper_soil_depth")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("time_dyn_temp")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("time_dyn_transf")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("descom_rate_micro")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("descom_rate_humus")));
				out.write(newLine);	
			}
		} catch (SQLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}	
	}

	public void writeSoil_param_II(PrintWriter out, int soil_id, int layer_id){
		
		try {
						
			ResultSet rs = gfdb.getSoil_param_II(soil_id, layer_id);
				
			while(rs.next()){
				
				out.write(Double.toString( rs.getDouble("frac_leaf_litter")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("ratio_decomp_resistant_plant_mat")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("decomp_plant_mat_decomp")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("decomp_plant_mat_resistant")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("total_org_carbon")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("bio_humus")));
				out.write(tab);
				out.write(Double.toString( rs.getDouble("frac_bio_ini_toc")));
				out.write(newLine);	
			}
		} catch (SQLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}	
	}
	
//*******************************DESTINATION FOLDER******************************************************************	
/*	public String getDestinationFolder() {
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
*/


	//Functions for Expert-N 5.0 input files
	public void asciiFileMakerXN5Management(String outfile, String resultID, int lu_id, int soil_id) {
		
		//CROP ROTATION FILE
		
			String filestub = outfile + lu_id ;

			int crop_id	=	gfdb.getPak_crops_crop_id_by_ktblmilp_process_id(lu_id);
			int seed_choice	=	gfdb.getPak_crops_seed_choice_by_ktblmilp_process_id(lu_id);

			xn5_writeCropRotationIni(filestub, lu_id, crop_id, seed_choice);
		
	}
	public void xn5_writeFertilizationIni (String filestub, int lu_id, int crop_id, int seed_choice) {
		try {
			PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(filestub + "_fertilization.ini")));
			String minfDate = "date = ";
			String minfFertName = "fertilizer = ";
			String minfCode = "code = ";
			String minfNtot = "n_tot_min = ";
			String minfNo3n = "no3n = ";
			String minfNh4n = "nh4n = ";
			String minfUrea = "urea = ";
			
			ResultSet rs = gfdb.get_min_fertilizations(lu_id);

			rs.beforeFirst();
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
				if (yr<10){ y = "200"+y;}
				String fertdate =   y+"-"+m+"-"+d;
				
				minfDate += fertdate + "; ";
				minfFertName += rs.getString("t4.fertilizer_mineral_name") + " ; ";
				minfCode += rs.getString("t4.fertilizer_mineral_code") + " ; ";
				minfNtot += rs.getString("total_n") + "; ";
				minfNo3n += rs.getString("nitrate_n") + "; ";
				minfNh4n += rs.getString("ammonium_n") + "; ";
				minfUrea += rs.getString("urea_n") + "; ";
			} 
			out.write("[mineral]" + newLine);
			out.write(minfDate + newLine);
			out.write(minfFertName + newLine);
			out.write(minfCode + newLine);
			out.write(minfNtot + newLine);
			out.write(minfNo3n + newLine);
			out.write(minfNh4n + newLine);
			out.write(minfUrea + newLine);
			out.write(newLine);

			
			String orgfDate = "date = ";
			String orgfFertName = "fertilizer = ";
			String orgfCode = "code = ";
			String orgfAmount = "amount = ";
			String orgfUnit = "unit = ";
			String orgfDryMat = "dry_matter = ";
			String orgfSubst = "org_subst = ";
			String orgfNtot = "n_tot_org = ";
			String orgfAmmonium = "nh4n = ";
		
			rs = gfdb.getnumber_organic_fertilizations(lu_id);
			rs.beforeFirst();
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
				if (yr<10){ y = "200"+y;}
				String orgfertdate =   y+"-"+m+"-"+d;
				
				orgfDate += orgfertdate + "; ";
				orgfFertName += rs.getString("t4.fertilizer_organic_german_name") + " ; ";
				orgfCode += rs.getString("t4.fertilizer_organic_code") + " ; ";
				orgfAmount += rs.getString("amount") + " ; ";
				orgfUnit += "???" + " ; ";
				
				orgfDryMat += rs.getString("TS") + " ; ";
				orgfSubst +=  rs.getString("org_sub") + " ; ";
				orgfNtot += rs.getString("total_n") + " ; ";
				orgfAmmonium += rs.getString("ammonium_n") + "; ";

				
			} 
			out.write("[organic]" + newLine);
			out.write(orgfDate + newLine);
			out.write(orgfFertName  + newLine);
			out.write(orgfCode  + newLine);
			out.write(orgfAmount  + newLine);
			out.write(orgfUnit  + newLine);
			out.write(orgfDryMat  + newLine);
			out.write(orgfSubst  + newLine);
			out.write(orgfNtot  + newLine);
			out.write(orgfAmmonium  + newLine);
			out.write(newLine);			
			out.close();
		}
		catch (Exception ex2) {
			System.err.println("Error in xn5_writeFertilizationIni");
			System.err.println(ex2.getMessage());
			System.exit(1);
		}
	}
	public void xn5_writeCropRotationIni (String filestub, int lu_id, int crop_id, int seed_choice) {
		try {
			PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(filestub + "_crop_rotation.ini")));
			
			String CropCode = "CropCode = ";
			String CropName = "CropName = ";
			String sow_date = "sow_date = ";
			String variety =  "variety = ";
			String sow_dens = "sow_dens = ";
			String row_dist = "row_dist = ";
			String sow_depth = "sow_depth = ";
			String max_root = "max_root = ";
			String emerg_date = "emerg_date = ";
			String harvest_date = "harvest_date = "; 
			String max_biom_date = "max_biom_date = ";
			String max_ro_date = "max_ro_date = ";
			String biom_remove = "biom_remove = ";
			
			
			
			ResultSet intermediate_crop = gfdb.get_intermediate_crop( lu_id);
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
				if (yr<10){ y = "200"+y;}
				String icseeddate =  y+"-"+m+"-"+d;
				
				//iccutdate
				ResultSet iccutset = gfdb.get_intermediate_crop_harvest(lu_id);
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
				if (yr<10){ y = "200"+y;}
				String iccutdate =  y+"-"+m+"-"+d;
				

				CropCode += icchar.getString("xn_code") + "; " ;
				CropName += icchar.getString("crop_name_english") +";";
				variety += icchar.getString("variety") + "; ";
				sow_depth += String.valueOf(icchar.getDouble("seeding_depth")) + "; ";
				row_dist += String.valueOf(icchar.getDouble("row_spacing")) + "; ";
				sow_dens += String.valueOf(icchar.getInt("seed_density")) + "; ";
				sow_date += icseeddate + "; ";
				emerg_date += "9999-09-09" +"; ";
				harvest_date += iccutdate +"; ";
				max_biom_date += "9999-09-09" +"; ";
				max_ro_date += "9999-09-09" +"; ";
				max_root += String.valueOf(icchar.getDouble("max_root_development_depth")) + "; ";
				biom_remove += String.valueOf(icchar.getInt("removal_above_ground")) +"; ";
		
			}
			ResultSet mcchar = gfdb.get_cultivated_crop_characteristics_by_crop_id(crop_id, seed_choice);
			ResultSet mcharvest = gfdb.get_cultivated_crop_harvestdate(lu_id);
			ResultSet mcseed = gfdb.get_cultivated_crop_seeddate(lu_id);
			mcchar.first();
			mcharvest.first();
			mcseed.first();
			
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
			if (yr<10){ y = "200"+y;}
			String seeddate =  y+"-"+m+"-"+d;
			
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
			if (yr<10){ y = "200"+y;}
			String harvestdate = y+"-"+m+"-"+d;
			
			
			CropCode += mcchar.getString("xn_code") + "; " ;
			CropName += mcchar.getString("crop_name_english") +";";
			variety += mcchar.getString("variety") + "; ";
			sow_depth += String.valueOf(mcchar.getDouble("seeding_depth")) + "; ";
			row_dist += String.valueOf(mcchar.getDouble("row_spacing")) + "; ";
			sow_dens += String.valueOf(mcchar.getInt("seed_density")) + "; ";
			sow_date += seeddate + "; ";
			emerg_date += "9999-09-09" +"; ";
			harvest_date += harvestdate +"; ";
			max_biom_date += "9999-09-09" +"; ";
			max_ro_date += "9999-09-09" +"; ";
			max_root += String.valueOf(mcchar.getDouble("max_root_development_depth")) + "; ";
			biom_remove += String.valueOf(mcchar.getInt("removal_above_ground")) +"; ";
	
			
			out.write("[plant management]" + newLine);
			out.write(CropCode + newLine);
			out.write(CropName + newLine);
			out.write(sow_date + newLine);
			out.write(variety + newLine);
			out.write(sow_dens + newLine);
			out.write(row_dist + newLine);
			out.write(sow_depth + newLine);
			out.write(max_root + newLine);
			out.write(emerg_date + newLine);
			out.write(harvest_date + newLine);
			out.write(max_biom_date + newLine);
			out.write(max_ro_date+ newLine);
			out.write(biom_remove + newLine);
			out.close();
			
		}
		catch (Exception ex2) {
			System.err.println("Error in xn5_writeCropRotationIni");
			System.err.println(ex2.getMessage());
			System.exit(1);
		}
	}
	void cmd_updateFromRFS(String xndOutfile, String xnmOutfile, String xndInfile, String xnmInfile, String rfsInfile, String weatherFn, String startDate, String endDate) {
		
		try {
			

			//parse sections from RFS file		
			BufferedReader rfsIn = new BufferedReader(new InputStreamReader(new DataInputStream(new FileInputStream(rfsInfile))));
			String section10011 = parse_Section10011_SoilStartValues (rfsIn);
			//skip next section
			getContentUntilMarker(rfsIn, "20003");
			String sections20003_to_20006 = getContentUntilMarker(rfsIn, "20007");
			//skip next section
			getContentUntilMarker(rfsIn, "20008");
			String sections20008_to_20010 = getContentUntilMarker(rfsIn, "EVENTS\\s+MODEL\\s+PARAMETERS.*");
			rfsIn.close();
			
			//get sections from original XND file			
			BufferedReader xndIn = new BufferedReader(new InputStreamReader(new DataInputStream(new FileInputStream(xndInfile))));
			String xndTop = getContentUntilMarker(xndIn, "10000");
			xn3_simTime simTime = parse_simTime(xndIn);
			String xndTop2 = getContentUntilMarker(xndIn, "10003");
			xn3_subplotInfo subplotInfo = parse_SubplotInfo(xndIn);
			String xndMiddle = getContentUntilMarker(xndIn, "10011");
			xn3_soilStartValuesTop soilStartTop = parse_soilStartValuesHeader(xndIn);
			//skip soil start values
			String inLine;
			xndIn.mark(2000);
			while ((inLine = xndIn.readLine()) != null) {
				//System.out.println( rfsLine);
				if (!inLine.matches("^\\s*\\d+\\s+.*") ) {
					xndIn.reset();
					break;
				}
				xndIn.mark(2000);
			}
			String xndLast = getContentUntilEOF(xndIn); 
			xndIn.close();
			
			//get section from original xnm
			BufferedReader xnmIn = new BufferedReader(new InputStreamReader(new DataInputStream(new FileInputStream(xnmInfile))));
			String xnmTop = getContentUntilMarker(xnmIn, "20003");
			getContentUntilMarker(xnmIn, "20007"); //discard
			String xnmSection20007 = getContentUntilMarker(xnmIn, "20008|20009");
			
			//skip sections and values
			getContentUntilMarker(xnmIn, "20009|20010|20011");
			xnmIn.mark(2000);
			while ((inLine = xnmIn.readLine()) != null) {
				//System.out.println( rfsLine);
				if (!inLine.matches("20009|20010|20011|20012|20013|^\\s*\\d+\\s+.*") ) {
					xnmIn.reset();
					break;
				}
				xnmIn.mark(2000);
			}
			String xnmGecrosHeader = 	getContentUntilMarker(xnmIn, "TEMPERATURE\\s+PARAMETERS\\s+OF\\s+DAISY\\s+DECOMPOSITION\\s+RATES.*|GECROS\\s+INPUT\\s+PARAMETERS.*"); //discarded
			String xnmLast = getContentUntilEOF(xnmIn);
			xnmIn.close();
			
			//adapt info from command line
			//make sure only xnm filename without path is written to xnd file
			File xnmOutFileObject = new File(xnmOutfile);
			
			
			subplotInfo.parameterFn = xnmOutFileObject.getName();
			if(!weatherFn.isEmpty())
				subplotInfo.weatherFn = weatherFn;
			if(!startDate.isEmpty())
				simTime.startDate = startDate;
			if(!endDate.isEmpty())
				simTime.endDate = endDate;				
			
			//write new xnd file
			PrintWriter outXnd = new PrintWriter(new BufferedWriter(new FileWriter(xndOutfile)));
			outXnd.write(xndTop);
			simTime.writeNoHeader(outXnd);
			outXnd.write(xndTop2);
			subplotInfo.writeNoHeader(outXnd);
			outXnd.write(xndMiddle);
			outXnd.write("10011\t" + soilStartTop.date + "\t" +soilStartTop.numLayers + "\t:Number of soil layers" + newLine);
			outXnd.write(section10011);
			outXnd.write(xndLast);
			outXnd.close();
			
			//write new xnm file
			PrintWriter outXnm = new PrintWriter(new BufferedWriter(new FileWriter(xnmOutFileObject)));
			outXnm.write(xnmTop);
			outXnm.write(sections20003_to_20006);
			outXnm.write(xnmSection20007);
			outXnm.write(sections20008_to_20010);
			//outXnm.write(xnmGecrosHeader);
			outXnm.write(xnmLast);
			outXnm.close();
		}
		catch (Exception ex2) {
			System.out.println("Error in asciiFileMaker_updateFromRFS");
			ex2.printStackTrace();
			System.exit(0);
		}
	
	}
	
	xn3_simTime parse_simTime(BufferedReader xndIn){
		xn3_simTime result = new xn3_simTime();
		String inLine;
		try {
			//find header
			while ((inLine = xndIn.readLine()) != null) {
					if (inLine.matches("^\\s*10000") ) {

						break;					 
					}
			}
			//go to first line
			//int c = 0;
			while ((inLine = xndIn.readLine()) != null) {
				//System.out.println(c++ + ":" + inLine);
				if (inLine.matches("^\\s*\\d+\\s+\\d+") ) {
					break;
				}
			}
			inLine.replaceAll("^\\s+", "");
			String[] tokens = inLine.split("\\s+");
			result.startDate = tokens[0];
			result.endDate = tokens[1];
		}
		catch (Exception ex2) {
			System.out.println("Error in parse_simTime");
			ex2.printStackTrace();
			System.exit(0);
		}
		return result;
	}
	
	xn3_soilStartValuesTop parse_soilStartValuesHeader (BufferedReader xndIn) {
		xn3_soilStartValuesTop result = new xn3_soilStartValuesTop();
		String inLine;
		try {
			//find header
			while ((inLine = xndIn.readLine()) != null) {
					if (inLine.matches("^\\s*10011\\s+.*") ) {
						//System.out.println("Found results 10011");
						break;					 
					}
			}
			inLine.replaceAll("^\\s+", "");
			String[] tokens = inLine.split("\\s+");
			result.date = tokens[1];
			result.numLayers = Integer.parseInt(tokens[2]);
		}
		catch (Exception ex2) {
			System.out.println("Error in parse_soilStartValuesHeader");
			ex2.printStackTrace();
			System.exit(0);
		}
		return result;
	}
	xn3_subplotInfo parse_SubplotInfo (BufferedReader xndIn) {
		xn3_subplotInfo result = new xn3_subplotInfo();
		String inLine;
		try {
			//find header
			while ((inLine = xndIn.readLine()) != null) {
					if (inLine.matches("^\\s*10003") ) {
						//System.out.println("Found results 10011");
						break;					 
					}
			}
			//go to first line
			while ((inLine = xndIn.readLine()) != null) {
				//System.out.println( rfsLine);
				if (inLine.matches("^\\s*\\d+\\s+.*") ) {
					break;
				}
			}
			//trim leading whitespace
			inLine.replaceAll("^\\s+", "");
			String[] tokens = inLine.split("\\s+");
			result.ID = Integer.parseInt(tokens[0]);
			result.subplotName = tokens[1];
			result.princCrop = tokens[2];
			result.weatherFn = tokens[3];
			result.parameterFn = tokens[4];
			result.xncFn = tokens[5];
			//go to next line with data
			while ((inLine = xndIn.readLine()) != null) {
				//System.out.println( rfsLine);
				if (inLine.matches("^\\s*\\d+\\s+.*") ) {
					break;
				}
			}
			inLine.replaceAll("^\\s+", "");
			tokens = inLine.split("\\s+");
			result.areaNum = Integer.parseInt(tokens[0]);
			result.size = Integer.parseInt(tokens[1]);
			result.slope = Integer.parseInt(tokens[2]);
			result.exposition = tokens[3];
		}
		catch (Exception ex2) {
			System.out.println("Error in parse_SubplotInfo");
			ex2.printStackTrace();
			System.exit(0);
		}
		return result;
	}
	
	String parse_Section10011_SoilStartValues (BufferedReader rfsIn) {
		return parse_Section10011_SoilStartValues (rfsIn, false, "", -1, -1);
	}
	String parse_Section10011_SoilStartValues (BufferedReader rfsIn, boolean toDb, String RUN, int cellGroup, int simulationPeriod) {
		
		StringBuilder parsedString = new StringBuilder();
		
		try {
		
			String rfsLine;
			//find header
			while ((rfsLine = rfsIn.readLine()) != null) {
					if (rfsLine.matches("^\\s*10011") ) {
						//System.out.println("Found results 10011");
						//parsedString.append(rfsLine + newLine);  
						break;					 
					}
			}
			//delete old values for this cellgroup, simulationPeriod and RUN
			if (toDb) {
				String deletestring = "DELETE FROM tbl_IMR_start_values WHERE model_run = '" + RUN + "' AND cellgroup = " + cellGroup + " AND period = " + simulationPeriod;
				gfdb.run_query(deletestring);
			}
			rfsIn.mark(2000);
			while ((rfsLine = rfsIn.readLine()) != null) {
				//System.out.println( rfsLine);
				if (rfsLine.matches("^\\s*\\d.*") ) {
					parsedString.append(rfsLine + newLine);
					rfsIn.mark(2000);
					if(toDb) {
						StringTokenizer st = new StringTokenizer(rfsLine, "\t");
						
						String sqlstring = "REPLACE INTO tbl_IMR_start_values VALUES ( '" + RUN + "', " + cellGroup + ", " + simulationPeriod +", " + st.nextToken()
						+ ", " + st.nextToken()+ ", " + st.nextToken()+ ", " + st.nextToken()+ ", " + st.nextToken()+ ", " + st.nextToken()
						+ ", " + st.nextToken()+ ", " + st.nextToken() +");";
						//System.out.println( sqlstring);
						gfdb.run_query(sqlstring);
					}
				}
				else {
					//System.out.println("no match "+rfsLine);
					rfsIn.reset();
					break;
				}
				
			}
		
		}
		catch (Exception ex2) {
			System.out.println("Error in asciiFileMaker_updateFromRFS");
			ex2.printStackTrace();
			System.exit(0);
		}
		
		return parsedString.toString();
	}
String getContentUntilMarker (BufferedReader inR, String marker) {
		
		StringBuilder parsedString = new StringBuilder();
		
		try {

			String rfsLine;
			//find header
			inR.mark(2000);
			while ((rfsLine = inR.readLine()) != null) {
				if (rfsLine.matches("^\\s*" + marker+".*") ) {
					inR.reset();
					break;					 
				}
				parsedString.append(rfsLine + newLine);
				inR.mark(2000);
			}
		}
		catch (Exception ex2) {
			System.out.println("Error in asciiFileMaker_updateFromRFS");
			ex2.printStackTrace();
			System.exit(0);
		}
		
		return parsedString.toString();
	}
	String getContentUntilEOF(BufferedReader inR) {
		StringBuilder parsedString = new StringBuilder();
		
		try {

			String rfsLine;

			while ((rfsLine = inR.readLine()) != null) {

				parsedString.append(rfsLine + newLine);

			}
		}
		catch (Exception ex2) {
			System.out.println("Error in asciiFileMaker_updateFromRFS");
			ex2.printStackTrace();
			System.exit(0);
		}
		
		return parsedString.toString();
	}
}//end of class
