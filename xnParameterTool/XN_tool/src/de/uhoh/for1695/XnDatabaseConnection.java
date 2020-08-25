package de.uhoh.for1695;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.DataInputStream;
import java.io.FileInputStream;
import java.io.FileWriter;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.Arrays;
import java.util.List;
import java.util.StringTokenizer;
import java.util.ArrayList;
import java.util.HashMap;
import java.io.File;
import java.lang.Math;

import javax.swing.JOptionPane;

import de.uhoh.for1695.MySQLConnection;

class	XnDatabaseConnection {
	
	String newLine = System.getProperty("line.separator"); //System independent hard return
	//String newLine = "\r\n"; //CT 100408 to make windows line breaks although being in Linux
	String space = " ";
	String var_txt_general_MCK001_xnd_line_six ="------------------------------------------------------------------------------------------------------------------";
	String tab = "\t";
	private MySQLConnection myConnection;
	
	
	
	
	public XnDatabaseConnection ( String host, String dbName , String userName, String password) {
		
		myConnection = new MySQLConnection(host, dbName, userName, password);
		if ( myConnection.isValid())
			myConnection.updateDb("SET NAMES utf8");
	}
	public boolean isValid() {
		
		return myConnection.isValid();
	}
	public Exception getLastError() {
		
		return myConnection.getLastError();
	}
	
	
	public boolean checkAuthorName (String currentAuthor) {
		boolean ok = false;
		try {
			String s = "SELECT author_surname FROM info_authors WHERE author_id = '" + currentAuthor +"'";
			ResultSet rs = myConnection.query(s);
			
			if (rs.first())
			{
				ok = true;
			}
			rs.close();
		}
		catch (Exception e) 
		{
			  e.printStackTrace();
		}
		return ok;
	}
	
	//IMPORTING FROM XN3
	
	public boolean importCropManagementFromXndFile (String xndFileName,  
			int cropManagementId, boolean overwrite ) {
		boolean ok = false;
		try {
		
			BufferedReader xndIn = new BufferedReader(new InputStreamReader(new DataInputStream(new FileInputStream(xndFileName))));

			//Create an entry in the table, to be updated later on
			cropInfo ret = parse_and_upload_xnd_planting_section (xndIn, cropManagementId, overwrite) ; 
			if (ret == null){
				xndIn.close();
				return false;
			}
			String crop_code = ret.crop_code;
			String variety = ret.variety;
			int sowYear = ret.sowYear;
			
			List<String> fieldList; 
			fieldList = Arrays.asList("date", "fertilizer", "code", "n_tot_min", "no3n", "nh4n", "urea");					
			if(!parse_and_upload_xnd_management_section( xndIn,  crop_code,  variety,  cropManagementId, 
					 "10006", "crop_management_mineral_fertilization", fieldList,sowYear ) ){
				xndIn.close();
				return false;
			}
			
			fieldList = Arrays.asList("date", "fertilizer", "code", "dry_matter", "n_tot_org", "nh4n", "org_subst");					
			if(!parse_and_upload_xnd_management_section( xndIn,  crop_code,  variety,  cropManagementId, 
					 "10007", "crop_management_organic_fertilization", fieldList,sowYear ) ){
				xndIn.close();
				return false;
			}
			fieldList = Arrays.asList("date", "depth", "equipment", "code");					
			if(!parse_and_upload_xnd_management_section( xndIn,  crop_code,  variety,  cropManagementId, 
					 "10008", "crop_management_tillage", fieldList,sowYear ) ){
				xndIn.close();
				return false;
			}
			fieldList = Arrays.asList("date", "amount", "code");					
			if(!parse_and_upload_xnd_management_section( xndIn,  crop_code,  variety,  cropManagementId, 
					 "10009", "crop_management_irrigation", fieldList,sowYear ) ){
				xndIn.close();
				return false;
			}
			
			xndIn.close();
			ok = true;
		}
		catch (Exception e) 
		{
			  e.printStackTrace();
			  return false;
		}
		return ok;
	}
	
	
	
	
	public boolean importSoilParameterizationsFromXnmXndFiles (String xnmFileName, String xndFileName,  
			int soilParamId, String profileID, boolean overwrite ) {
		boolean ok = false;
		try {
			
			BufferedReader xnmIn = new BufferedReader(new InputStreamReader(new DataInputStream(new FileInputStream(xnmFileName))));
			
			//Create an entry in the table, to be updated later on
			if (overwrite)
			{
				String sqlstring = "DELETE FROM  soil_parameterization  " 
						+ " WHERE soil_param_id = " + soilParamId
						+ " AND profileID =  '" + profileID + "'"
						+";";;
				//System.out.println( sqlstring);
				myConnection.updateDb(sqlstring);
			}

			
			List<String> fieldList;
			fieldList = Arrays.asList("soil_type", "wilting_point", "field_capacity", "porosity", "cond_sat"
					);
			if(! parse_and_upload_soil_param_xnm_table_section (xnmIn, profileID, soilParamId,  "10020", fieldList, true) ) {
				xnmIn.close();
				return false;
			}

			fieldList = Arrays.asList("res_water_cont", "cont_sat", "camp_a", "camp_b", "van_gen_a", "van_gen_n", "van_gen_m"
					);
			if(! parse_and_upload_soil_param_xnm_table_section (xnmIn, profileID, soilParamId,  "10021", fieldList, false)){
				xnmIn.close();
				return false;
			}

			xnmIn.close();
			
			BufferedReader xndIn = new BufferedReader(new InputStreamReader(new DataInputStream(new FileInputStream(xndFileName))));

			fieldList = Arrays.asList("clay", "silt", "sand", "organic_matter", "bulk_density", "rock_fraction", "ph"
					);
			if( ! parse_and_upload_soil_param_xnd_table_section (xndIn, profileID, soilParamId,  fieldList, false) ) {
				xndIn.close();
				return false;
			}
			xndIn.close();
			ok = true;
		}
		catch (Exception e) 
		{
			  e.printStackTrace();
			  return false;
		}
		return ok;
	}
	public boolean importSoilInitializationsFromXnmXndFiles (String xnmFileName, String xndFileName,  
			int soilInitId, String profileID, boolean overwrite ) {
		boolean ok = false;
		try {
			
			BufferedReader xnmIn = new BufferedReader(new InputStreamReader(new DataInputStream(new FileInputStream(xnmFileName))));


			//Create an entry in the table, to be updated later on
			if (overwrite)
			{
				String sqlstring = "DELETE FROM  soil_initialization  " 
						+ " WHERE soilinit_param_id = " + soilInitId
						+ " AND profileID =  '" + profileID + "'"
						+";";;
				//System.out.println( sqlstring);
				myConnection.updateDb(sqlstring);
			}

			
			List<String> fieldList;
			fieldList = Arrays.asList("c_litter", "n_litter", "c_manure", "n_manure", "c_humus", "n_humus"
					);
			if (!parse_and_upload_soil_init_xnm_table_section (xnmIn, profileID, soilInitId,  "20003", fieldList, true)) {
					xnmIn.close();
					return false;
			}
			xnmIn.close();
		
			BufferedReader xndIn = new BufferedReader(new InputStreamReader(new DataInputStream(new FileInputStream(xndFileName))));

			fieldList = Arrays.asList("water_content", "matrix_potential", "soil_temperature", "nh4_content", "no3_content", "root_density"
					);
			if(! parse_and_upload_soil_init_xnd_table_section (xndIn, profileID, soilInitId,  "10011", fieldList, false)) {
				xndIn.close();
				return false;
			}
			xndIn.close();
			ok = true;
		}
		catch (Exception e) 
		{
			  e.printStackTrace();
			  return false;
		}
		return ok;
	}
	public boolean importDaisyParametersFromXnmFile (String xnmFileName,  
			int daisyParamId, String profileID, boolean overwrite ) {
		boolean ok = false;
		try {
		
			BufferedReader xnmIn = new BufferedReader(new InputStreamReader(new DataInputStream(new FileInputStream(xnmFileName))));

			//Create an entry in the table, to be updated later on
			if (overwrite)
			{
				String sqlstring = "DELETE FROM  daisy_parameterization  " 
						+ " WHERE daisy_param_id = " + daisyParamId
						+ " AND profileID =  '" + profileID + "'"
						+";";;
				//System.out.println( sqlstring);
				if (!myConnection.updateDb(sqlstring)){
						xnmIn.close();
						return false;
				}
				sqlstring = "DELETE FROM  daisy_parameterization_layers  "
						+ " WHERE daisy_param_id = " + daisyParamId
						+ " AND profileID =  '" + profileID + "'"
						+";";;
				//System.out.println( sqlstring);
				if (!myConnection.updateDb(sqlstring)){
						xnmIn.close();
						return false;
				}

			}
			String sqlstring = "INSERT INTO daisy_parameterization  " 
					+ " (daisy_param_id, profileID) VALUES (" + daisyParamId + ", '" +  profileID + "')"
					+";";
			//System.out.println( sqlstring);
			if (!myConnection.updateDb(sqlstring)){
				xnmIn.close();
				return false;
			}

			List<String> fieldList;
			fieldList = Arrays.asList("layer","effic","humf","min_cn", "temp0", "miner_q10", "theta0", "theta1", "MinerSatActiv", "NitrifNO3NH4Ratio"
					);
			if (! parse_and_upload_xnm_daisy_section (xnmIn, profileID, daisyParamId,  "20001", fieldList) ) {
				xnmIn.close();
				return false;
			}
			

			fieldList = Arrays.asList("fom_slow", "fom_fast", "fom_veryfast", "micbioms_slow", "micbioms_fast", "humus_slow", "humus_fast"
					);
			if (! parse_and_upload_xnm_daisy_table_section (xnmIn, profileID, daisyParamId,  "20007", fieldList)) { 
				xnmIn.close();
				return false;
			}
			
			
			fieldList = Arrays.asList("iTempCorr", "AOM1Q10","AOM2Q10","BOM1Q10","BOM2Q10","SOM1Q10"
					,"SOM2Q10", "DBOM1Q10","DBOM2Q10",  "MBOM1Q10", "MBOM2Q10"
					);
			if (! parse_and_upload_xnm_daisy_section (xnmIn, profileID, daisyParamId,  "20021", fieldList, 1) ) { 
				xnmIn.close();
				return false;
			}
			
			fieldList = Arrays.asList("fBOM1",	    "fSOM1",	   "fSOM2",	   "fEFF"
					);
			if (! parse_and_upload_xnm_daisy_section (xnmIn, profileID, daisyParamId,  "20022", fieldList) ) {
				xnmIn.close();
				return false;
			}
			
			
			fieldList = Arrays.asList("fEFFAOM1","fEFFAOM2","fEFFBOM1",	"fEFFBOM2",	"fEFFSOM1",	"fEFFSOM2"
					);
			if (! parse_and_upload_xnm_daisy_section (xnmIn, profileID, daisyParamId,  "20023", fieldList) ) {
				xnmIn.close();
				return false;
			}
			

			
			xnmIn.close();
			ok = true;
		}
		catch (Exception e) 
		{
			  e.printStackTrace();
			  return false;
		}
		return ok;
	}
	public boolean importDaisyPoolsFromXnmFile (String xnmFileName,  int sompoolsParamId, String profileID, boolean overwrite ) {
		boolean ok = false;
		try {
		
			BufferedReader xnmIn = new BufferedReader(new InputStreamReader(new DataInputStream(new FileInputStream(xnmFileName))));

			//Create an entry in the table, to be updated later on
			if (overwrite)
			{
				String sqlstring = "DELETE FROM  sompools_initialization " 
						+ " WHERE sompools_param_id = " + sompoolsParamId
						+ " AND profileID =  '" + profileID + "'"
						+";";;
				//System.out.println( sqlstring);
				if (!myConnection.updateDb(sqlstring)){
						xnmIn.close();
						return false;
				}
				

			}


			List<String> fieldList;
			fieldList = Arrays.asList("AOM1_C", "AOM2_C", "AOM3_C",  "BOM1_C",
					"BOM2_C", "BOMD_C", "SOM0_C", "SOM1_C", "SOM2_C", "Csol"
					);
			if (! parse_and_upload_xnm_sompools_table_section (xnmIn, profileID, sompoolsParamId,  "20008", fieldList, true)) { 
				xnmIn.close();
				return false;
			}

			fieldList = Arrays.asList(
					"AOM1_N", "AOM2_N", "AOM3_N", "BOM1_N", "BOM2_N", "BOMD_N","SOM0_N", 
					"SOM1_N", "SOM2_N"
					);
			if (! parse_and_upload_xnm_sompools_table_section (xnmIn, profileID, sompoolsParamId,  "20009", fieldList, false)) { 
				xnmIn.close();
				return false;
			}
		
			xnmIn.close();
			ok = true;
		}
		catch (Exception e) 
		{
			  e.printStackTrace();
			  return false;
		}
		return ok;
	}
	
	//IMPORT LOOPS:
	
	public void loop_importDaisyPoolsFromXnm () {
		String letter = "S";
		String dir = "SA";
		String codeStub = "RXN40MSA";
		int sompool_param_id = 3;
		int daisy_param_id = 3;

		
		String infolder = "/home/troost/Dropbox/SFAL/RegionalXN/daisy_regional_inputs/"+dir+"_regional_xnm/";
		
		for (int s = 1; s<= 40; ++s) {
			
				
				String xnmFileName = infolder + letter + String.valueOf(s) + ".xnm";
				File f = new File(xnmFileName);
				if (f.exists() ) {
					
					importDaisyPoolsFromXnmFile ( xnmFileName,  sompool_param_id, codeStub+ String.format("%02d", s), true ); 
					importDaisyParametersFromXnmFile ( xnmFileName,  daisy_param_id, codeStub+ String.format("%02d", s), true ); 
				}
				
				
			
		}
	}
	public void loop_importCropManagementFromECs () {
		
		String infolder = "/home/troost/Dropbox/SFAL/RegionalXN/xnd/";
		
		for (int s = 1; s<= 6; ++s) {
			for (int y = 2009; y <= 2016; ++y) {
				
				String xndFileName = infolder + "EC" + String.valueOf(s) + "_"+ String.valueOf(y) + ".xnd";
				File f = new File(xndFileName);
				if (f.exists() ) {
					int cropManagementId = addIdCode(idTypes.management, "EC" + String.valueOf(s) + "_"+ String.valueOf(y), "", "poyda");
					importCropManagementFromXndFile ( xndFileName,  cropManagementId, true ); 
				}
				
				xndFileName = infolder + "EC" + String.valueOf(s) + "_"+ String.valueOf(y) + "a.xnd";
				f = new File(xndFileName);
				if (f.exists() ) {
					int cropManagementId = addIdCode(idTypes.management, "EC" + String.valueOf(s) + "_"+ String.valueOf(y)+"a", "", "poyda");
					importCropManagementFromXndFile ( xndFileName,  cropManagementId, true ); 
				}
			}
		}
	}
	
	public cropInfo	parse_and_upload_xnd_planting_section (BufferedReader xndIn, int cropManagementId, boolean overwrite)  {
			String marker = "10005";
			List<String> fieldList = Arrays.asList("sow_depth", "row_dist", "sow_dens", "sow_date", "emerg_date", "harvest_date", "max_biom_date","max_ro_date", "max_root_depth", "biom_remove");
			cropInfo ret = null;
			int sowYear = 0;
			try {
				String inLine;
				//find header
				boolean found = false; 
				while ((inLine = xndIn.readLine()) != null) {
					//System.out.println(inLine);
					if (inLine.matches("^\\s*"+marker+"\\b.*") ) {
						//System.out.println("Found results 10011");
						//parsedString.append(rfsLine + newLine);
						found = true;
						break;					 
					}
				}
				if(!found)
					return ret;
				
				int i = 0;

				
				StringTokenizer st = new StringTokenizer(inLine, " \t");
				st.nextToken();//skip marker
				
				int numcrops = Integer.valueOf(st.nextToken());
				if(numcrops > 1) 	{						
					System.out.println("Error: Crop management importing not defined from xnd files containing more than one crop");
					return ret;
				}
			
				if ((inLine = xndIn.readLine()) == null) {
					return ret;
				}
				
				
				st = new StringTokenizer(inLine, " \t");
				st.nextToken();//skip crop id
				String crop_code = st.nextToken();
				/*if(!file_crop_code.equals(crop_code)) 	{						
					System.out.println("Error in crop management importing: Crop code '"+file_crop_code+"' in file 'does not match specified crop code:" +crop_code);
					return false;
				}*/
				String variety = st.nextToken(); //discard variety, no check here, since variety change could be o.k (e.g. to reflect KRG and MSA differences)
				
				
				
				if (overwrite)
				{
					String sqlstring = "DELETE FROM  crop_management_planting  " 
							+ " WHERE crop_management_id = " + cropManagementId
							+ " AND crop_code =  '" + crop_code + "'"
							+ " AND variety =  '" + variety + "'"
							+";";;
					//System.out.println( sqlstring);
					if (!myConnection.updateDb(sqlstring)){
						xndIn.close();
							return ret;
					}
					sqlstring = "DELETE FROM  crop_management_irrigation  " 
							+ " WHERE crop_management_id = " + cropManagementId
							+ " AND crop_code =  '" + crop_code + "'"
							+ " AND variety =  '" + variety + "'"
							+";";;
					//System.out.println( sqlstring);
					if (!myConnection.updateDb(sqlstring)){
						xndIn.close();
							return ret;
					}
					sqlstring = "DELETE FROM  crop_management_tillage  " 
							+ " WHERE crop_management_id = " + cropManagementId
							+ " AND crop_code =  '" + crop_code + "'"
							+ " AND variety =  '" + variety + "'"
							+";";;
					//System.out.println( sqlstring);
					if (!myConnection.updateDb(sqlstring)){
						xndIn.close();
							return ret;
					}
					sqlstring = "DELETE FROM  crop_management_mineral_fertilization  " 
							+ " WHERE crop_management_id = " + cropManagementId
							+ " AND crop_code =  '" + crop_code + "'"
							+ " AND variety =  '" + variety + "'"
							+";";;
					//System.out.println( sqlstring);
					if (!myConnection.updateDb(sqlstring)){
						xndIn.close();
							return ret;
					}
					sqlstring = "DELETE FROM  crop_management_organic_fertilization  " 
							+ " WHERE crop_management_id = " + cropManagementId
							+ " AND crop_code =  '" + crop_code + "'"
							+ " AND variety =  '" + variety + "'"
							+";";;
					//System.out.println( sqlstring);
					if (!myConnection.updateDb(sqlstring)){
							xndIn.close();
							return ret;
					}

				}
				String sqlstring = "INSERT INTO crop_management_planting  " 
						+ " (crop_management_id, crop_code, variety) VALUES (" + cropManagementId + ", '" +  crop_code + "', '" +  variety +"')"
						+";";
				//System.out.println( sqlstring);
				if (!myConnection.updateDb(sqlstring)){
					xndIn.close();
					return ret;
				}
				sqlstring = "UPDATE crop_management_planting  " 
						+ " SET " ;				
				
				
					for (; i < fieldList.size(); ++i )
					{
						try {
							String temp = st.nextToken();
							if (i > 0)
							{	
								sqlstring	+= ",";
							}
							if (fieldList.get(i).equals("sow_date") && ! temp.equals("-99")) {
								String day = temp.substring(0,2);
								String month = temp.substring(2,4);
								String yearRead = temp.substring(4,6);
								sowYear = Integer.valueOf(yearRead);
								
								sqlstring += " sow_date_year = 0,";
								sqlstring += " sow_date =  '" + month+"-"+day+"'";
							}
							else if (fieldList.get(i).matches(".*date.*") && ! temp.equals("-99")) {
								String day = temp.substring(0,2);
								String month = temp.substring(2,4);
								String yearRead = temp.substring(4,6);
								int relYear = Integer.valueOf(yearRead) - sowYear;

								
								sqlstring += " " + fieldList.get(i) + " = '" + month+"-"+day+"',";
								sqlstring += " " + fieldList.get(i) + "_year = "+relYear;
							}
							else if ((fieldList.get(i).matches(".*depth.*") || fieldList.get(i).matches(".*row_dist.*") ) && ! temp.equals("-99")) {
								double depth = Double.valueOf(temp) * 100;
								sqlstring += " " + fieldList.get(i) + " = " + String.valueOf(depth);									
							}
							else {
								sqlstring += " " + fieldList.get(i) + " = " + temp;
							}
						}
						catch (Exception ex)
						{
							System.out.println("Error in crop management importing: Missing entry for " + fieldList.get(i));
							return ret;
						}
					}

					sqlstring += " WHERE crop_management_id = " + cropManagementId
						+ " AND crop_code =  '" + crop_code + "'"
						+ " AND variety =  '" + variety + "'"
						+";";
					System.out.println( sqlstring);
					if (!myConnection.updateDb(sqlstring)){
						return ret;
					}



				ret = new cropInfo(crop_code, variety, sowYear);
			}
			catch (Exception e) 
			{
				System.out.println("Error in parse_and_upload_xnd_planting_section:" + marker);
				e.printStackTrace();
				
			}
			return ret;
	}
	
	public boolean	parse_and_upload_xnd_management_section( BufferedReader xndIn, String crop_code, String variety, int cropManagementId, 
			 String marker, String dbTable, List<String> fieldList , int sowYear) {
			boolean ok = false;
			try {
				String inLine;
				//find header
				boolean found = false;
				while ((inLine = xndIn.readLine()) != null) {
					if (inLine.matches("^\\s*"+marker+"\\b.*") ) {
						//System.out.println("Found results 10011");
						//parsedString.append(rfsLine + newLine);
						found = true;
						break;					 
					}
				}
				if (!found)
					return false;
				
				xndIn.mark(2000);

				String sqlstringStart = "INSERT INTO " + dbTable 
						+ " SET  crop_management_id  = " + cropManagementId + ", " +
								"crop_code = '" + crop_code + "', " +
								"variety = '" + variety + "' ";
						
				int cc = 0;
				while ((inLine = xndIn.readLine()) != null) {
					//System.out.println( rfsLine);
					
					if (inLine.matches("^\\s*[\\d+-.].*") ) {

						xndIn.mark(2000);
						StringTokenizer st = new StringTokenizer(inLine, " \t");
						cc++;	
						
						String sqlstring = sqlstringStart;
						sqlstring += ", event_id = " + cc;

						
						int i = 0;
						for (; i < fieldList.size(); ++i )
						{
							try {
								String temp = st.nextToken();
								if (fieldList.get(i).matches(".*date.*") && ! temp.equals("-99")) {
									String day = temp.substring(0,2);
									String month = temp.substring(2,4);
									String yearRead = temp.substring(4,6);
									int relYear = Integer.valueOf(yearRead) - sowYear;

									
									sqlstring += ", " + fieldList.get(i) + " = '" + month+"-"+day+"'";
									sqlstring += ", " + fieldList.get(i) + "_year = "+relYear;
								}
								else if ((fieldList.get(i).matches(".*depth.*")  ) && ! temp.equals("-99")) {
									double depth = Double.valueOf(temp) * 100;
									sqlstring += ", " + fieldList.get(i) + " = " + String.valueOf(depth);									
								}
								else if(isNumeric(temp) ) {
									sqlstring += ", " + fieldList.get(i) + " = " + temp;
								}
								else {
									sqlstring += ", " + fieldList.get(i) + " = '" + temp +"'";
								}
							}
							catch (Exception ex)
							{
								break; //exit from for list
							}
						}
						
						if (i != fieldList.size() )
						{	
							System.err.println("Error: Missing column in .xnd table " + marker);
							return false;
						}
						sqlstring += " ;";
						System.out.println( sqlstring);
						if(!myConnection.updateDb(sqlstring) ) {
							return false;
						}
						//if end of layers not reached read next line
					}
					else {
						//System.out.println("no match "+rfsLine);
						xndIn.reset();
						break;
					}

				}
				
				ok = true;
			}
			catch (Exception e) 
			{
				System.out.println("Error in parse_and_upload_xnd_management_section:" + marker);
				e.printStackTrace();
				
			}
			return ok;
	}
	
	
	public boolean	parse_and_upload_xnm_daisy_section( BufferedReader xnmIn, String profileID, int daisyParamId, 
			 String marker, List<String> fieldList  ) {
		return parse_and_upload_xnm_daisy_section(  xnmIn,  profileID,  daisyParamId, 
				  marker,  fieldList, 0 );
	}
	public boolean	parse_and_upload_xnm_daisy_section( BufferedReader xnmIn, String profileID, int daisyParamId, 
			 String marker, List<String> fieldList, int valuesOnMarkerLine ) {
			boolean ok = false;
			try {
				String inLine;
				//find header
				boolean found = false; 
				while ((inLine = xnmIn.readLine()) != null) {
					//System.out.println(inLine);
					if (inLine.matches("^\\s*"+marker+"\\b.*") ) {
						//System.out.println("Found results 10011");
						//parsedString.append(rfsLine + newLine);
						found = true;
						break;					 
					}
				}
				if(!found)
					return false;
				
				xnmIn.mark(2000);

				int i = 0;
				String sqlstring = "UPDATE daisy_parameterization  " 
						+ " SET " ;
				if (valuesOnMarkerLine > 0) {
					StringTokenizer st0 = new StringTokenizer(inLine, " \t");
					st0.nextToken();
					for (; i < valuesOnMarkerLine; ++i) {
						try {
							String temp = st0.nextToken();
							if (i > 0)
							{	
								sqlstring	+= ",";
							}
							sqlstring += " " + fieldList.get(i) + " = " + temp;
						}
						catch (Exception ex)
						{
							System.out.println("Error: Missing value on marker line!");
							return false; //exit from for list
						}
						
						
					}
				}
				while ((inLine = xnmIn.readLine()) != null) {
					//System.out.println( rfsLine);
					if (inLine.matches("^\\s*[\\d+-.].*") ) {

						xnmIn.mark(2000);
						StringTokenizer st = new StringTokenizer(inLine, " \t");

						for (; i < fieldList.size(); ++i )
						{
							try {
								String temp = st.nextToken();
								if (i > 0)
								{	
									sqlstring	+= ",";
								}
								sqlstring += " " + fieldList.get(i) + " = " + temp;
							}
							catch (Exception ex)
							{
								break; //exit from for list
							}
						}

						if (i == fieldList.size() )
						{	sqlstring += " WHERE daisy_param_id = " + daisyParamId
							+ " AND profileID =  '" + profileID + "'"
							+";";
							System.out.println( sqlstring);
							if (!myConnection.updateDb(sqlstring)){
								return false;
							}
							break;
						}
						//if end of fieldList not reached read next line
					}
					else {
						//System.out.println("no match "+rfsLine);
						xnmIn.reset();
						break;
					}

				}

				ok = true;
			}
			catch (Exception e) 
			{
				System.out.println("Error in parse_and_upload_xnm_daisy_section:" + marker);
				e.printStackTrace();
				
			}
			return ok;
	}
	public boolean	parse_and_upload_xnm_daisy_table_section( BufferedReader xnmIn, String profileID, int daisyParamId, 
			 String marker, List<String> fieldList ) {
			boolean ok = false;
			try {
				String inLine;
				//find header
				boolean found = false;
				while ((inLine = xnmIn.readLine()) != null) {
					if (inLine.matches("^\\s*"+marker+"\\b.*") ) {
						//System.out.println("Found results 10011");
						//parsedString.append(rfsLine + newLine);
						found = true;
						break;					 
					}
				}
				if (!found)
					return false;
				
				xnmIn.mark(2000);

				String sqlstringStart = "INSERT INTO daisy_parameterization_layers  " 
						+ " SET  daisy_param_id  = " + daisyParamId + ", " +
								"profileID = '" + profileID + "' ";
						
				
				while ((inLine = xnmIn.readLine()) != null) {
					//System.out.println( rfsLine);
					if (inLine.matches("^\\s*[\\d+-.].*") ) {

						xnmIn.mark(2000);
						StringTokenizer st = new StringTokenizer(inLine, " \t");
						
						String layerTemp = st.nextToken();
						int layerID = Integer.valueOf(layerTemp);
						
						String sqlstring = sqlstringStart;
						sqlstring += ", first_layer = "+ layerID
									 + ", layers = 1";

						
						int i = 0;
						for (; i < fieldList.size(); ++i )
						{
							try {
								String temp = st.nextToken();
								
								sqlstring += ", " + fieldList.get(i) + " = " + temp;
							}
							catch (Exception ex)
							{
								break; //exit from for list
							}
						}
						
						if (i != fieldList.size() )
						{	
							System.err.println("Error: Missing column in .xnm table " + marker);
							return false;
						}
						sqlstring += " ;";
						System.out.println( sqlstring);
						if(!myConnection.updateDb(sqlstring) ) {
							return false;
						}
						//if end of layers not reached read next line
					}
					else {
						//System.out.println("no match "+rfsLine);
						xnmIn.reset();
						break;
					}

				}
				
				ok = true;
			}
			catch (Exception e) 
			{
				System.out.println("Error in parse_and_upload_xnm_daisy_section:" + marker);
				e.printStackTrace();
				
			}
			return ok;
	}
	public boolean	parse_and_upload_soil_param_xnm_table_section( BufferedReader xIn, String profileID, int soilParamId, 
			 String marker, List<String> fieldList, boolean newRecord ) {
			boolean ok = false;
			try {
				String inLine;
				//find header
				boolean found = false;
				while ((inLine = xIn.readLine()) != null) {
					if (inLine.matches("^\\s*"+marker+"\\b.*") ) {
						//System.out.println("Found results 10011");
						//parsedString.append(rfsLine + newLine);
						found = true;
						break;					 
					}
				}
				if (!found)
					return false;
				
				xIn.mark(2000);

				String sqlstringStart;
				if(newRecord) {
				 sqlstringStart = "INSERT INTO soil_parameterization  " 
						+ " SET  soil_param_id  = " + soilParamId + ", " +
								"profileID = '" + profileID + "' ,";
				}
				else {
					sqlstringStart = "UPDATE soil_parameterization SET " ;
				}
				
				while ((inLine = xIn.readLine()) != null) {
					//System.out.println( rfsLine);
					if (inLine.matches("^\\s*[\\d+-.].*") ) {

						xIn.mark(2000);
						StringTokenizer st = new StringTokenizer(inLine, " \t");
						
						String layerTemp = st.nextToken();
						int layerID = Integer.valueOf(layerTemp);
						
						String sqlstring = sqlstringStart;
						if (newRecord) {
							sqlstring += " first_layer = "+ layerID
									 + ", layers = 1,";
						}
						
						int i = 0;
						for (; i < fieldList.size(); ++i )
						{
							try {
								String temp = st.nextToken();
								
								if(i != 0 )
								{
										sqlstring += ", ";
								}
								if(isNumeric(temp) ) {
									sqlstring +=  fieldList.get(i) + " = " + temp;
								}
								else {
									sqlstring +=  fieldList.get(i) + " = '" + temp +"'";
								}
								
							}
							catch (Exception ex)
							{
								break; //exit from for list
							}
						}
						
						if (i != fieldList.size() )
						{	
							System.err.println("Error: Missing column in table " + marker);
							return false;
						}
						if(newRecord) {
							sqlstring += " ;";
						}
						else {
							 sqlstring += " WHERE  soil_param_id  = " + soilParamId + " AND " +
											"profileID = '" + profileID + "'  AND first_layer = "+ layerID +";";
						}
						
						System.out.println( sqlstring);
						if(!myConnection.updateDb(sqlstring) ) {
							return false;
						}
						//if end of layers not reached read next line
					}
					else {
						//System.out.println("no match "+rfsLine);
						xIn.reset();
						break;
					}

				}
				
				ok = true;
			}
			catch (Exception e) 
			{
				System.out.println("Error in parse_and_upload_soil_param_xnm_table_section: " + marker);
				e.printStackTrace();
				
			}
			return ok;
	}
	
	
	public boolean	parse_and_upload_soil_init_xnm_table_section( BufferedReader xIn, String profileID, int soilInitId, 
			 String marker, List<String> fieldList, boolean newRecord ) {
			boolean ok = false;
			try {
				String inLine;
				//find header
				boolean found = false;
				while ((inLine = xIn.readLine()) != null) {
					if (inLine.matches("^\\s*"+marker+"\\b.*") ) {
						//System.out.println("Found results 10011");
						//parsedString.append(rfsLine + newLine);
						found = true;
						break;					 
					}
				}
				if (!found)
					return false;
				
				xIn.mark(2000);

				String sqlstringStart;
				if(newRecord) {
				 sqlstringStart = "INSERT INTO soil_initialization  " 
						+ " SET  soilinit_param_id  = " + soilInitId + ", " +
								"profileID = '" + profileID + "' ,";
				}
				else {
					sqlstringStart = "UPDATE soil_initialization SET " ;
				}
				
				while ((inLine = xIn.readLine()) != null) {
					//System.out.println( rfsLine);
					if (inLine.matches("^\\s*[\\d+-.].*") ) {

						xIn.mark(2000);
						StringTokenizer st = new StringTokenizer(inLine, " \t");
						
						String layerTemp = st.nextToken();
						int layerID = Integer.valueOf(layerTemp);
						
						String sqlstring = sqlstringStart;
						if (newRecord) {
							sqlstring += " first_layer = "+ layerID
									 + ", layers = 1,";
						}
						
						int i = 0;
						for (; i < fieldList.size(); ++i )
						{
							try {
								String temp = st.nextToken();
								
								if(i == 0 )
								{
									sqlstring +=  fieldList.get(i) + " = " + temp;
								}
								else {
									sqlstring += ", " + fieldList.get(i) + " = " + temp;
								}
								
							}
							catch (Exception ex)
							{
								break; //exit from for list
							}
						}
						
						if (i != fieldList.size() )
						{	
							System.err.println("Error: Missing column in table " + marker);
							return false;
						}
						if(newRecord) {
							sqlstring += " ;";
						}
						else {
							 sqlstring = " WHERE  soilinit_param_id  = " + soilInitId + " AND " +
											"profileID = '" + profileID + "'  AND first_layer = "+ layerID +";";
						}
						
						System.out.println( sqlstring);
						if(!myConnection.updateDb(sqlstring) ) {
							return false;
						}
						//if end of layers not reached read next line
					}
					else {
						//System.out.println("no match "+rfsLine);
						xIn.reset();
						break;
					}

				}
				
				ok = true;
			}
			catch (Exception e) 
			{
				System.out.println("Error in parse_and_upload_soil_init_xnm_table_section: " + marker);
				e.printStackTrace();
				
			}
			return ok;
	}
	public boolean	parse_and_upload_soil_init_xnd_table_section( BufferedReader xIn, String profileID, int soilInitId, 
			 String marker, List<String> fieldList, boolean newRecord ) {
			boolean ok = false;
			try {
				String inLine;
				//find header
				boolean found = false;
				while ((inLine = xIn.readLine()) != null) {
					if (inLine.matches("^\\s*"+marker+"\\b.*") ) {
						//System.out.println("Found results 10011");
						//parsedString.append(rfsLine + newLine);
						found = true;
						break;					 
					}
				}
				if (!found)
					return false;
				
				xIn.mark(2000);

				String sqlstringStart;
				if(newRecord) {
				 sqlstringStart = "INSERT INTO soil_initialization  " 
						+ " SET  soilinit_param_id  = " + soilInitId + ", " +
								"profileID = '" + profileID + "' ,";
				}
				else {
					sqlstringStart = "UPDATE soil_initialization SET " ;
				}
				int lastLayer = 0;
				while ((inLine = xIn.readLine()) != null) {
					//System.out.println( rfsLine);
					if (inLine.matches("^\\s*[\\d+-.].*") ) {

						xIn.mark(2000);
						StringTokenizer st = new StringTokenizer(inLine, " \t");
						
						String layerTemp = st.nextToken();
						int layerID = Integer.valueOf(layerTemp);
						layerTemp = st.nextToken();
						int numLayers = Integer.valueOf(layerTemp);
						
						String sqlstring = sqlstringStart;

						
						int i = 0;
						for (; i < fieldList.size(); ++i )
						{
							try {
								String temp = st.nextToken();
								
								if (fieldList.get(i).equals("nh4_content") 
										|| fieldList.get(i).equals("no3_content"))
								{
									temp =  String.valueOf(Double.valueOf(temp)/numLayers);
								}
								
								if(i == 0 )
								{
									sqlstring +=  fieldList.get(i) + " = " + temp;
								}
								else {
									sqlstring += ", " + fieldList.get(i) + " = " + temp;
								}
								
							}
							catch (Exception ex)
							{
								
								break; //exit from for list
							}
						}
						
						if (i != fieldList.size() )
						{	
							System.err.println("Error: Missing column in table " + marker);
							return false;
						}
						
						for (int j = lastLayer + 1 ; j <= lastLayer + numLayers; ++j) {
							String sqlstringFull = sqlstring;
							if(newRecord) {
								sqlstringFull += " first_layer = "+ j
										 + ", layers = 1 ;";
							} 
							else {
								 sqlstringFull += " WHERE  soilinit_param_id  = " + soilInitId + " AND " +
												"profileID = '" + profileID + "'  AND first_layer = "+ j +";";
							}
							System.out.println( sqlstringFull);
							if(!myConnection.updateDb(sqlstringFull) ) {
								return false;
							}
						}
						lastLayer = lastLayer + numLayers;
				
						//if end of layers not reached read next line
					}
					else {
						//System.out.println("no match "+rfsLine);
						xIn.reset();
						break;
					}

				}
				
				ok = true;
			}
			catch (Exception e) 
			{
				System.out.println("Error in parse_and_upload_soil_init_xnd_table_section: " + marker);
				e.printStackTrace();
				
			}
			return ok;
	}
	public boolean	parse_and_upload_soil_param_xnd_table_section( BufferedReader xIn, String profileID, int soilParamId, 
			  List<String> fieldList, boolean newRecord ) {
			String marker = "10010"; 
			boolean ok = false;
			try {
				String inLine;
				//find header
				boolean found = false;
				while ((inLine = xIn.readLine()) != null) {
					if (inLine.matches("^\\s*"+marker+"\\b.*") ) {
						//System.out.println("Found results 10011");
						//parsedString.append(rfsLine + newLine);
						found = true;
						break;					 
					}
				}
				if (!found)
					return false;
				
				xIn.mark(2000);

				String sqlstringStart;
				if(newRecord) {
				 sqlstringStart = "INSERT INTO soil_parameterization  " 
						+ " SET  soil_param_id  = " + soilParamId + ", " +
								"profileID = '" + profileID + "' ,";
				}
				else {
					sqlstringStart = "UPDATE soil_parameterization SET " ;
				}
				
				StringTokenizer st0 = new StringTokenizer(inLine, " \t");
				st0.nextToken(); //discard marker
				st0.nextToken(); //discard number of layers
				int depth_per_layer = Integer.valueOf(st0.nextToken());
				sqlstringStart += " depth_per_layer = " + depth_per_layer + ", ";
				
				int lastLayer = 0;
				while ((inLine = xIn.readLine()) != null) {
					//System.out.println( rfsLine);
					if (inLine.matches("^\\s*[\\d+-.].*") ) {

						xIn.mark(2000);
						StringTokenizer st = new StringTokenizer(inLine, " \t");
						
						String layerTemp = st.nextToken();
						int layerID = Integer.valueOf(layerTemp);
						layerTemp = st.nextToken();
						int numLayers = Integer.valueOf(layerTemp);
						
						String sqlstring = sqlstringStart + " horizon_id = " + layerID + ", ";

						
						int i = 0;
						for (; i < fieldList.size(); ++i )
						{
							try {
								String temp = st.nextToken();
								
								if(i != 0 )
								{
									sqlstring += ", ";
								}
								if(isNumeric(temp) ) {
									sqlstring +=  fieldList.get(i) + " = " + temp;
								}
								else {
									sqlstring +=  fieldList.get(i) + " = '" + temp +"'";
								}
								
							}
							catch (Exception ex)
							{
								break; //exit from for list
							}
						}
						
						if (i != fieldList.size() )
						{	
							System.err.println("Error: Missing column in table " + marker);
							return false;
						}
						
						for (int j = lastLayer + 1 ; j <= lastLayer + numLayers; ++j) {
							String sqlstringFull = sqlstring;
							if(newRecord) {
								sqlstringFull += " first_layer = "+ j
										 + ", layers = 1 ;";
							} 
							
							else {
								 sqlstringFull += " WHERE  soil_param_id  = " + soilParamId + " AND " +
												"profileID = '" + profileID + "'  AND first_layer = "+ j +";";
							}
							System.out.println( sqlstringFull);
							if(!myConnection.updateDb(sqlstringFull) ) {
								return false;
							}
						}
						lastLayer = lastLayer + numLayers;
				
						//if end of layers not reached read next line
					}
					else {
						//System.out.println("no match "+rfsLine);
						xIn.reset();
						break;
					}

				}
				
				ok = true;
			}
			catch (Exception e) 
			{
				System.out.println("Error in parse_and_upload_soil_param_xnd_table_section: " + marker);
				e.printStackTrace();
				
			}
			return ok;
	}
	public boolean	parse_and_upload_xnm_sompools_table_section( BufferedReader xIn, String profileID, int sompoolInitId, 
			 String marker, List<String> fieldList, boolean newRecord ) {
			boolean ok = false;
			try {
				String inLine;
				//find header
				boolean found = false;
				while ((inLine = xIn.readLine()) != null) {
					if (inLine.matches("^\\s*"+marker+"\\b.*") ) {
						//System.out.println("Found results 10011");
						//parsedString.append(rfsLine + newLine);
						found = true;
						break;					 
					}
				}
				if (!found)
					return false;
				
				xIn.mark(2000);

				String sqlstringStart;
				if(newRecord) {
				 sqlstringStart = "INSERT INTO sompools_initialization  " 
						+ " SET  sompools_param_id  = " + sompoolInitId + ", " +
								"profileID = '" + profileID + "' ,";
				}
				else {
					sqlstringStart = "UPDATE sompools_initialization SET " ;
				}
				
				while ((inLine = xIn.readLine()) != null) {
					//System.out.println( rfsLine);
					if (inLine.matches("^\\s*[\\d+-.].*") ) {

						xIn.mark(2000);
						StringTokenizer st = new StringTokenizer(inLine, " \t");
						
						String layerTemp = st.nextToken();
						int layerID = Integer.valueOf(layerTemp);
						
						String sqlstring = sqlstringStart;
						if (newRecord) {
							sqlstring += " first_layer = "+ layerID
									 + ", layers = 1,";
						}
						
						int i = 0;
						for (; i < fieldList.size(); ++i )
						{
							try {
								String temp = st.nextToken();
								
								if(i == 0 )
								{
									sqlstring +=  fieldList.get(i) + " = " + temp;
								}
								else {
									sqlstring += ", " + fieldList.get(i) + " = " + temp;
								}
								
							}
							catch (Exception ex)
							{
								break; //exit from for list
							}
						}
						
						if (i != fieldList.size() )
						{	
							System.err.println("Error: Missing column in table " + marker);
							return false;
						}
						if(newRecord) {
							sqlstring += " ;";
						}
						else {
							 sqlstring += " WHERE  sompools_param_id  = " + sompoolInitId + " AND " +
											"profileID = '" + profileID + "'  AND first_layer = "+ layerID +";";
						}
						
						System.out.println( sqlstring);
						if(!myConnection.updateDb(sqlstring) ) {
							return false;
						}
						//if end of layers not reached read next line
					}
					else {
						//System.out.println("no match "+rfsLine);
						xIn.reset();
						break;
					}

				}
				
				ok = true;
			}
			catch (Exception e) 
			{
				System.out.println("Error in parse_and_upload_xnm_sompools_table_section: " + marker);
				e.printStackTrace();
				
			}
			return ok;
	}
	
	
	public boolean importGecrosParametersFromXnmFile (String xnmFileName,  
			int plantParamId, String crop_code, String variety, String ecotype, String daylength, String leafangle, 
			 int OptVernDays, int VernCoeff, double TempOptDevVern, double TempMinDevVern , double TempMaxDevVern,
			boolean overwrite ) {
		boolean ok = false;
		try {
		
			BufferedReader xnmIn = new BufferedReader(new InputStreamReader(new DataInputStream(new FileInputStream(xnmFileName))));

			//Create an entry in the table, to be updated later on
			if (overwrite)
			{
				String sqlstring = "DELETE FROM  plant_parameterization_gecros  " 
						+ " WHERE plant_param_id = " + plantParamId
						+ " AND crop_code =  '" + crop_code + "' AND variety = '" + variety +"'"
						+";";;
				//System.out.println( sqlstring);
				myConnection.updateDb(sqlstring);
			}
			
			String sqlstring = "INSERT INTO plant_parameterization_gecros  " 
					+ " (plant_param_id, crop_code,  variety, ECOTYPE, DAYLENGTH, LEAFANGLE, OptVernDays, VernCoeff, TempOptDevVern, TempMinDevVern , TempMaxDevVern) VALUES (" 
					+ plantParamId + ", '" +  crop_code + "', '" + variety + "', '" + ecotype + "', '" + daylength + "', '" + leafangle 
					+ "', '" + OptVernDays+ "', '" + VernCoeff+ "', '" + TempOptDevVern+ "', '" + TempMinDevVern + "', '" + TempMaxDevVern + "')"
					+";";
			//System.out.println( sqlstring);
			myConnection.updateDb(sqlstring);
			
			
			List<String> fieldList = Arrays.asList("LEGUME", "C3C4", "DETER", "SLP", "LODGE", "VERN");
			parse_and_upload_xnm_gecros_section (xnmIn, crop_code, plantParamId, variety, "80001", fieldList); 

			fieldList = Arrays.asList("YGV", "CFV",	"EG", "FFAT","FLIG", "FOAC","FMIN");
			parse_and_upload_xnm_gecros_section (xnmIn, crop_code, plantParamId, variety, "80002", fieldList); 
			
			fieldList = Arrays.asList("LWIDTH",	"CDMHT", "RDMX", "TBD", "TOD", "TCD", "TSEN");
			parse_and_upload_xnm_gecros_section (xnmIn, crop_code, plantParamId, variety, "80003", fieldList);
			
			fieldList = Arrays.asList("NUPTX","RNCMIN",	"STEMNC","SLNMIN", "LNCI", "SLA0", "CCFIX");
			parse_and_upload_xnm_gecros_section (xnmIn, crop_code, plantParamId, variety, "80004", fieldList);
	
			fieldList = Arrays.asList("INSP", "SPSP", "EPSP",	"EAJMAX",	   "XVN",	    "XJN",	    "THETA");
			parse_and_upload_xnm_gecros_section (xnmIn, crop_code, plantParamId, variety, "80005", fieldList);

			fieldList = Arrays.asList("SEEDW", "SEEDNC", "BLD", "HTMX", "MTDV", "MTDR", "PSEN");
			parse_and_upload_xnm_gecros_section (xnmIn, crop_code, plantParamId, variety, "80006", fieldList);
			
			fieldList = Arrays.asList("PMEH",	   "PMES",		      "ESDI",			"WRB");
			parse_and_upload_xnm_gecros_section (xnmIn, crop_code, plantParamId, variety, "80007", fieldList);
			
			fieldList = Arrays.asList("CO2A","COEFR", "COEFT", "COEFV",	"FCRSH", "FNRSH", "PNPRE", "CB", "CX", "TM");
			parse_and_upload_xnm_gecros_section (xnmIn, crop_code, plantParamId, variety, "80008", fieldList);
			
			if (crop_code.equals("SB")) {
				fieldList = Arrays.asList("SINKBEET", "EFF", "CFS");
				parse_and_upload_xnm_gecros_section (xnmIn, crop_code, plantParamId, variety, "80011", fieldList);
			}
			
			
			xnmIn.close();
			ok = true;
		}
		catch (Exception e) 
		{
			  e.printStackTrace();
			  return false;
		}
		return ok;
	}
	
	
	
	/*public boolean	parse_and_upload_xnm_section_80001( BufferedReader xnmIn, int cropId, int plantParamId, String variety ) 
	{
		boolean ok = false;
		try {
			String inLine;
			//find header
			while ((inLine = xnmIn.readLine()) != null) {
					if (inLine.matches("^\\s*80001") ) {
						//System.out.println("Found results 10011");
						//parsedString.append(rfsLine + newLine);  
						break;					 
					}
			}
			xnmIn.mark(2000);
			while ((inLine = xnmIn.readLine()) != null) {
				//System.out.println( rfsLine);
				if (inLine.matches("^\\s*[\\d+-].*") ) {

					xnmIn.mark(2000);
					StringTokenizer st = new StringTokenizer(inLine, "\t");
					
					String sqlstring = "UPDATE plant_parameterization_gecros  " 
							+ " SET " 
							+ " LEGUME = " + st.nextToken() + ","
							+ " C3C4 = " + st.nextToken() + ","
							+ " DETER = " + st.nextToken() + ","
							+ " SLP = " + st.nextToken() + ","
							+ " LODGE = " + st.nextToken() + ","
							+ " VERN = " + st.nextToken() 
							+ " WHERE plant_param_id = " + plantParamId
							+ " AND crop_id =  " + cropId + " AND variety = '" + variety + "'"
							+";";
					System.out.println( sqlstring);
					myConnection.updateDb(sqlstring);
					break;
				}
				else {
					//System.out.println("no match "+rfsLine);
					xnmIn.reset();
					break;
				}
				
			}
		
			
			ok = true;
		}
		catch (Exception e) 
		{
			System.out.println("Error in parse_and_upload_xnm_section_80001");
			e.printStackTrace();
			System.exit(0);
		}
		return ok;
	}
	*/
	public boolean	parse_and_upload_xnm_gecros_section( BufferedReader xnmIn, String crop_code, int plantParamId, 
													String variety, String marker, List<String> fieldList ) 
	{
		boolean ok = false;
		try {
			String inLine;
			//find header
			while ((inLine = xnmIn.readLine()) != null) {
					if (inLine.matches("^\\s*"+marker) ) {
						//System.out.println("Found results 10011");
						//parsedString.append(rfsLine + newLine);  
						break;					 
					}
			}
			xnmIn.mark(2000);
			
			int i = 0;
			String sqlstring = "UPDATE plant_parameterization_gecros  " 
					+ " SET " ;
			while ((inLine = xnmIn.readLine()) != null) {
				//System.out.println( rfsLine);
				if (inLine.matches("^\\s*[\\d+-.].*") ) {

					xnmIn.mark(2000);
					StringTokenizer st = new StringTokenizer(inLine, " \t");
					

					
					for (; i < fieldList.size(); ++i )
					{
						try {
							String temp = st.nextToken();
							if (i > 0)
							{	
								sqlstring	+= ",";
							}
							sqlstring += " " + fieldList.get(i) + " = " + temp;
				
						}
						catch (Exception ex)
						{
							break; //exit from for list
						}
					}
					
					if (i == fieldList.size() )
					{	sqlstring += " WHERE plant_param_id = " + plantParamId
						+ " AND crop_code =  '" + crop_code + "' AND variety = '" + variety + "'"
						+";";
						System.out.println( sqlstring);
						myConnection.updateDb(sqlstring);
						break;
					}
					//if end of fieldList not reach read next line
				}
				else {
					//System.out.println("no match "+rfsLine);
					xnmIn.reset();
					break;
				}
				
			}
		
			
			ok = true;
		}
		catch (Exception e) 
		{
			System.out.println("Error in parse_and_upload_xnm_gecros_section:" + marker);
			e.printStackTrace();
			System.exit(0);
		}
		return ok;
	}
	
	String getXn3ContentUntilMarker (BufferedReader inR, String marker) {
		
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
	String getXn3ContentUntilEOF(BufferedReader inR) {
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
	public String firstLetterToLowerCase(String input) {
		char c[] = input.toCharArray();
		c[0] = Character.toLowerCase(c[0]);
		return new String(c);
	}
	
	public boolean importGecrosParametersFromIniFile (String iniFileName,  
			int plantParamId, String crop_code, boolean overwrite ) {
		boolean ok = false;
		try {
		
			BufferedReader xnmIn = new BufferedReader(new InputStreamReader(new DataInputStream(new FileInputStream(iniFileName))));

			//Create an entry in the table, to be updated later on
/*			if (overwrite)
			{
				String sqlstring = "DELETE FROM  plant_parameterization_gecros  " 
						+ " WHERE plant_param_id = " + plantParamId
						+ " AND crop_code =  '" + crop_code + "' AND variety = '" + variety +"'"
						+";";;
				//System.out.println( sqlstring);
				myConnection.updateDb(sqlstring);
			}
*/			
			
			
			
			List<String> fieldList = Arrays.asList(
					
													"variety", "ECOTYPE", "DAYLENGTH", "LEAFANGLE",
													"LEGUME", "C3C4", "DETER", "SLP", "LODGE", "VERN",
													"YGV", "CFV",	"EG", "FFAT","FLIG", "FOAC","FMIN",
													"LWIDTH",	"CDMHT", "RDMX", "TBD", "TOD", "TCD", "TSEN",
													"NUPTX","RNCMIN",	"STEMNC","SLNMIN", "LNCI", "SLA0", "CCFIX",
													"INSP", "SPSP", "EPSP",	"EAJMAX",	   "XVN",	    "XJN",	    "THETA",
													"SEEDW", "SEEDNC", "BLD", "HTMX", "MTDV", "MTDR", "PSEN",
													"PMEH",	   "PMES",		      "ESDI",			"WRB",
													"CO2A","COEFR", "COEFT", "COEFV",	"FCRSH", "FNRSH", "PNPRE", "CB", "CX", "TM",
													"OptVernDays", "VernCoeff", "TempOptDevVern", "TempMinDevVern", "TempMaxDevVern"
													
									);
			if (crop_code.equals("SB")) {
				fieldList.add("SINKBEET");
				fieldList.add("EFF");
				fieldList.add("CFS");
			}
			
			//"ResidueToAMO1_frac","C_fromDeadleaf_frac","N_fromDeadleaf_frac"
			
			HashMap<String,String> map = new HashMap<String, String>();
			
			String inLine;
			while ((inLine = xnmIn.readLine()) != null) {
				//System.out.println( rfsLine);
				if (inLine.matches("^\\s*#") ) {
					continue;
				}
				if (inLine.matches("^\\s*$") ) {
					continue;
				}
				if (inLine.matches("^\\s*\\[") ) {
					continue;
				}
				
				if (inLine.matches("^\\s*[\\w].*") ) {
					
					StringTokenizer st = new StringTokenizer(inLine, "=;");
		
					try {
						String field = st.nextToken();
						String value = st.nextToken();
						//field = field.trim();
						//value = value.trim();
						//System.out.println("Field:\t'"+field+"'");
						//System.out.println("Value:\t'"+value+"'");
						
						map.put(field.trim(), value.trim());
					}
					catch (Exception ex)
					{
						break; //exit from for list
					}
				}
			}
			
			String sqlstring;
			String sqlstring2 = ") VALUES ("+ plantParamId + ", '" +  crop_code + "'";
			
			if (overwrite) {
				sqlstring = "REPLACE INTO plant_parameterization_gecros  (plant_param_id, crop_code";
			}
			else {
				sqlstring = "INSERT INTO plant_parameterization_gecros  (plant_param_id, crop_code";

			}
			for (int i = 0; i < fieldList.size(); ++i) {
			
				String value = map.get( fieldList.get(i) );
				if (value == null){
					JOptionPane.showMessageDialog(null, "Missing entry " +  fieldList.get(i) +" in import of GECROS parameters from .ini file.");
				}
				else {
					sqlstring += ", " + fieldList.get(i);
					sqlstring2 +=", '" + value+"'";
				}
			}	
			
			sqlstring2 +=");";
			
			//System.out.println( sqlstring);
			//System.out.println(sqlstring+sqlstring2);
			myConnection.updateDb(sqlstring+sqlstring2);
			
			
			
			xnmIn.close();
			ok = true;
		}
		catch (Exception e) 
		{
			  e.printStackTrace();
			  return false;
		}
		return ok;
	}
	
	
		
	
	
	
	public boolean writeXn5GenericContentFile (String xn5FilePrefix, int projectId, String projectName, String table, String contentColumn, String title) {
		boolean ok = false;
		try {
			
			ResultSet freeText = getContentForProject( table, contentColumn, projectId);
			
			if (! freeText.first()) {
				throw new Exception("Error: Missing "+title+" content for simulation project.");
			}
			
			String xn5FileName = xn5FilePrefix + title;		
			PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(xn5FileName)));
			
			out.println(freeText.getString(contentColumn));
			
			out.close();
			ok = true;
			freeText.close();
		}
		catch (Exception e) 
		{
			  e.printStackTrace();
		}
		return ok;
	}
	public boolean writeXnMpmasCouplingIni(String xn5FilePrefix, int projectId, String projectName, int projectType, String projectDir) {
		boolean ok = false;
		try {
			String s0 = "SELECT startYear, endYear, startMonth, startDay, endMonth, endDay, plotSize " 
					+ " FROM simulation_projects_general "
					+ " WHERE simulation_project_id = "+ projectId + ";";
			ResultSet projectInfo = myConnection.query(s0);	
	
			if (! projectInfo.first()) {
				throw new Exception("Error: Missing general information on simulation project.");
			}
		
			ResultSet couplingInfo = getCouplingInfoForProject(projectId);
			
			if (! couplingInfo.first()) {
				throw new Exception("Error: Missing xnmpmas coupling content for simulation project.");
			}
			String[] temp = projectDir.split("/", 0);
			String projectDirectory = temp[temp.length -1];
			
			int startYear = projectInfo.getInt("startYear");
			int startMonth = projectInfo.getInt("startMonth");
			int startDay = projectInfo.getInt("startDay");
			int endYear = projectInfo.getInt("endYear");
			int endMonth = projectInfo.getInt("endMonth");
			int endDay = projectInfo.getInt("endDay");
			
			int simYears = endYear - startYear;
			
			if (couplingInfo.getInt("firstSowingMonth") < projectInfo.getInt("startMonth") 
					|| (couplingInfo.getInt("firstSowingMonth") == projectInfo.getInt("startMonth") 
						&& 	couplingInfo.getInt("firstSowingDay") < projectInfo.getInt("startDay")
							)
					) {// if first sowing not in first calendar year deduct one BEMS cycle
				simYears -= 1;
			} 
			
			if (couplingInfo.getInt("firstSowingMonth") > projectInfo.getInt("endMonth") 
					|| (couplingInfo.getInt("firstSowingMonth") == projectInfo.getInt("endMonth") 
						&& 	couplingInfo.getInt("firstSowingDay") > projectInfo.getInt("endDay")
							)
					) {// if sowing is not reached in final calendar year deduct one BEMS cycle (
				simYears -= 1;
			} 

			if (couplingInfo.getInt("lastHarvestMonth") > projectInfo.getInt("endMonth") 
					|| (couplingInfo.getInt("lastHarvestMonth") == projectInfo.getInt("endMonth") 
						&& 	couplingInfo.getInt("lastHarvestDay") > projectInfo.getInt("endDay")
							)
					) {// if last harvest not in last calendar year, deduct one BEMS cycle as it cannot be completed (last cycle ends at last harvest)
				simYears -= 1;
			}  

			
			String xn5FileNameCn = xn5FilePrefix + "_XnMpmasCoupling.ini" ;		
			PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(xn5FileNameCn)));
			
			out.println("[Common]");	
			out.println("scenarioName = " + projectName);
			out.println("");
			out.println("luaParametersFile = " + projectName + "_MpmasLuaToXnParameters.yaml");
			out.println("");
			out.println("firstSowingDay = "+  couplingInfo.getString("firstSowingDay"));
			out.println("firstSowingMonth = "+  couplingInfo.getString("firstSowingMonth"));
			out.println("firstSowingRelativeYear = "+  couplingInfo.getString("firstSowingRelativeYear"));
			out.println("lastHarvestDay = "+  couplingInfo.getString("lastHarvestDay"));
			out.println("lastHarvestMonth = "+  couplingInfo.getString("lastHarvestMonth"));
			out.println("lastHarvestRelativeYear = "+  couplingInfo.getString("lastHarvestRelativeYear"));		
			out.println("simYears = "+ simYears );
			out.println("");		
			out.println("[Coupling]");			
			
			switch (projectType){
				case 1: 
					out.println("couplingType = 0 " );
					out.println("luaMapFileStub = " +  projectName + "_fixedMapsLanduse");
					break;
				case 2: 
					out.println("couplingType = 2 " );
					break;
				case 3: 
					out.println("couplingType = 2 " );
					break;
			
			}
					
			out.println("");		
			out.println("[MPMAS]");
			out.println("commandline = " + couplingInfo.getString("MPMAS_commandline")); 
			out.println("");
			out.println("[XPN]");
			out.println("startYear = "+ startYear );
			out.println("config-name = " +  projectName + ".xpn"  );
			out.println("commandline = --base-dir="+couplingInfo.getString("XN_basedir") +" --config-file="
					+couplingInfo.getString("simulation_basedir")+"/"+projectDirectory +"/"+projectName  + ".xpn"  ); 
			

			out.println("cropGridFile = " +  projectName + "_XnCropCodeToGrid.dat");

//			commandline = 
			
			out.println("");		
			out.println("[Adaptation]");
			out.println("generalParametersFile = " + projectName + "_AdaptationParameters.yaml");
			out.println("cellWeatherRecordsLinkMap = " + projectName + "_xnCellStationMap.txt");
			out.println("weatherRecordsFile = " + projectName + "_init_weather_history.txt");
			out.println("weatherRecordsCellSpecific = 0");					
	
			out.close();
			
			
			String xn5FileNameA = xn5FilePrefix + "_AdaptationParameters.yaml" ;		
			PrintWriter out2 = new PrintWriter(new BufferedWriter(new FileWriter(xn5FileNameA)));
			
			out2.println("---");
			out2.println("history-size    : 6");
			out2.println("history-weights : [ 0.29385965, 0.24232456, 0.20942982, 0.14144737, 0.08004386, 0.03289474 ]");
			out2.println("...");
			
			out2.close();
			ok = true;
			projectInfo.close();
			couplingInfo.close();
		}
		catch (Exception e) 
		{
			  e.printStackTrace();
		}
		return ok;
	}
	
	public boolean writeXn5XpnFile(String xn5FilePrefix, int projectId, String projectName, String xn5_cells_table_name) {
		boolean ok = false;
		try {
			String s0 = "SELECT startYear, endYear, startMonth, startDay, endMonth, endDay, plotSize, kc_param_id,type_of_project " 
					+ " FROM simulation_projects_general t1"
					+ " JOIN info_simulation_projects t2 ON t1. simulation_project_id = t2.simulation_project_id "
					+ " WHERE t1.simulation_project_id = "+ projectId + ";";
			ResultSet projectInfo = myConnection.query(s0);	
	
			if (! projectInfo.first()) {
				throw new Exception("Error: Missing general information on simulation project.");
			}
		
			ResultSet freeText = getXpnContentForProject(projectId);
			
			if (! freeText.first()) {
				throw new Exception("Error: Missing xpn content for simulation project.");
			}
			
			String s1 = "SELECT xn5_cell_x, xn5_cell_y "
						+" FROM `"+ xn5_cells_table_name +"` t1"
						+" WHERE simulation_project_id = " + projectId
						+" ORDER BY xn5_cell_x, xn5_cell_y;";
			ResultSet cellList = myConnection.query(s1);	
			
			if (! cellList.first()) {
				throw new Exception("Error: No map cells/plots defined for simulation project.");
			}	
			cellList.beforeFirst();
			
			int startYear = projectInfo.getInt("startYear");
			int startMonth = projectInfo.getInt("startMonth");
			int startDay = projectInfo.getInt("startDay");
			int endYear = projectInfo.getInt("endYear");
			int endMonth = projectInfo.getInt("endMonth");
			int endDay = projectInfo.getInt("endDay");
			
			String xn5FileNameXpn = xn5FilePrefix + ".xpn" ;		
			PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(xn5FileNameXpn)));
			
			out.println("[model]");			
			out.print("grid = ");
			while (cellList.next())
			{
				int curGrid = 0;
				int curX = cellList.getInt("xn5_cell_x");
				int curY = cellList.getInt("xn5_cell_y");
				int active = 1;
				String mosaicShare = "100.0";
				String xn5FileNameCfg = projectName + "_" + curGrid + "_" + curX + "_" + curY + "_0.xpi" ;
				
				out.print(active+","+curGrid + "," + curX + "," + curY+",$PROJECT_PATH/"+xn5FileNameCfg+","+mosaicShare+";"); 
				
			}
			out.print("\n");
			out.println("");	
			cellList.close();
			
			out.println("[options]");					
			out.println("simulation start = "+startYear+"-"+ String.format("%02d", startMonth)+"-"+String.format("%02d", startDay));
			out.println("simulation end = "+endYear+"-"+String.format("%02d", endMonth)+"-"+String.format("%02d", endDay));
			out.println("");	
			
			
	    	ResultSet gecrosCrops = getGecrosParameterizationSelectionOnly(projectId);
	    	List<String> gecrosInis = new ArrayList<String>();
			if (gecrosCrops.first()) {
				gecrosCrops.beforeFirst();
		    	while(gecrosCrops.next())
		    	{
		    		//String crop_code = gecrosCrops.getString("crop_code");
		    		String variety = gecrosCrops.getString("variety");
		    		String crop_name = gecrosCrops.getString("crop_name");
		    		String cropline;
		    		if (variety.equals("Default"))	{
		    			cropline = crop_name + "_gecros = $<$PROJECT_PATH/$PROJECT_NAME__" + crop_name.toLowerCase() + "_gecros.ini$>";
		    		}
		    		else {
		    			cropline = crop_name + "_"+ variety +"_gecros = $<$PROJECT_PATH/$PROJECT_NAME__" + crop_name.toLowerCase() + "_"+ firstLetterToLowerCase(variety) + "_gecros.ini$>";
		    		}
		    		gecrosInis.add(cropline);
		    	}
			}	
			
			for (int i = 0; i< gecrosInis.size(); ++i)
			{
				out.println(gecrosInis.get(i));
			}
			out.println("");
			gecrosCrops.close();
			
			
			out.println("");
			out.println(freeText.getString("xpnText"));
			out.close();
			freeText.close();

			
			String xn5FileNameKcDevStage = xn5FilePrefix + "_kc_dev_stage.ini" ;		
			PrintWriter outKc = new PrintWriter(new BufferedWriter(new FileWriter(xn5FileNameKcDevStage)));
			
			if (projectInfo.getInt("type_of_project") > 0 ) {
				outKc.println("[LOAD ALL]");
				outKc.println("");
			}

			String s2 = "SELECT `kc_dev_stage`.`Crop`,`kc_dev_stage`.`VarietyName`, `kc_dev_stage`.`DevStage`, `kc_dev_stage`.`kc`" +
					" , IF (`kc_dev_stage`.`VarietyName` = 'Default', 'AAAAAAAAAAAAA', `kc_dev_stage`.`VarietyName`)   AS VarietyNameForSort" +
					" FROM `for1695_expertN`.`kc_dev_stage` WHERE `kc_dev_stage`.`kc_param_id` = "+ projectInfo.getInt("kc_param_id") 
					+ " ORDER BY Crop, VarietyNameForSort, DevStage;"
					;
		    ResultSet kcData = myConnection.query(s2);	
			
		    if (kcData.first()) {
			    String curCrop = kcData.getString("Crop");
			    String curVariety = kcData.getString("VarietyName");
			    int counterDevStage = 0;
			    int counterElse = 0;
			    
			    List<String> fieldContents = new ArrayList<String>();
				fieldContents.add("VarietyName " + " = "+ String.format("%8s"," ") + String.format("%-19s",curVariety) +";" );
				fieldContents.add("DevKC       " + " = " );
				fieldContents.add("KC          " + " = " );
				fieldContents.add("else        " + " = " );
				
				outKc.println("["+curCrop+"]");
			    while (! kcData.isAfterLast())
				{
			    	if(!curCrop.equals(kcData.getString("Crop"))) {

			    		fieldContents.set(1, fieldContents.get(1) + ";"  );
			    		fieldContents.set(2, fieldContents.get(2) + ";" );
			    		fieldContents.set(3, fieldContents.get(3) + ";"  );
			    		
			    		outKc.println(fieldContents.get(0));
			    		outKc.println(fieldContents.get(1));
			    		outKc.println(fieldContents.get(2));
			    		outKc.println(fieldContents.get(3));
			    		outKc.println("");
			    		
			    		curCrop = kcData.getString("Crop");			    		
			    		fieldContents.clear();
						fieldContents.add("VarietyName " + " = " );
						fieldContents.add("DevKC       " + " = " );
						fieldContents.add("KC          " + " = " );
						fieldContents.add("else        " + " = " );
			    		
						curVariety = kcData.getString("VarietyName");
					    counterDevStage = 0;
					    counterElse = 0;

			    		fieldContents.set(0, fieldContents.get(0) +  String.format("%8s"," ")+ String.format("%-19s",curVariety)+";"  );
		
			    		outKc.println("["+curCrop+"]");
			    	}
			    	if(!curVariety.equals(kcData.getString("VarietyName"))) {
			    		curVariety = kcData.getString("VarietyName");
			    		fieldContents.set(0, fieldContents.get(0)+ String.format("%8s"," ")  +  String.format("%-19s",curVariety)+";"  );
			    		fieldContents.set(1, fieldContents.get(1) + ";" );
			    		fieldContents.set(2, fieldContents.get(2) + ";"  );
			    		fieldContents.set(3, fieldContents.get(3) + ";"  );
					    counterDevStage = 0;
					    counterElse = 0;

			    	}
			    	if(kcData.getDouble("DevStage") < 10)
			    	{
			    		if (counterDevStage == 0)
			    		{
			    			fieldContents.set(1, fieldContents.get(1)+ String.format("%8s"," ")  + String.format("%.2f",kcData.getDouble("DevStage"))  );
			    			fieldContents.set(2, fieldContents.get(2)+ String.format("%8s"," ")  + String.format("%.2f",kcData.getDouble("kc"))  );
			    		}
			    		else {
			    			fieldContents.set(1, fieldContents.get(1) + ","+ String.format("%.2f", kcData.getDouble("DevStage"))   );
			    			fieldContents.set(2, fieldContents.get(2) + ","+ String.format("%.2f",kcData.getDouble("kc"))   );			    			
			    		}
			    		counterDevStage++;
			    	}
			    	else if (kcData.getDouble("DevStage") >= 9.9)
			    	{
			    		if (counterElse == 0) {
			    			fieldContents.set(3, fieldContents.get(3)+ String.format("%18s"," ") +  String.format("%.2f",kcData.getDouble("kc"))   );
			    		}
			    		else {
			    			fieldContents.set(3, fieldContents.get(3) + ","+ String.format("%.2f",kcData.getDouble("kc"))   );
			    		}
			    		counterElse++;
			    	}
			    	
			    	if ( ! kcData.next())
			    			break;
				}
	    		fieldContents.set(1, fieldContents.get(1) + ";"  );
	    		fieldContents.set(2, fieldContents.get(2) + ";"  );
	    		fieldContents.set(3, fieldContents.get(3) + ";"  );
	    		
	    		outKc.println(fieldContents.get(0));
	    		outKc.println(fieldContents.get(1));
	    		outKc.println(fieldContents.get(2));
	    		outKc.println(fieldContents.get(3));
	    		outKc.println("");
			    
		    }
		    outKc.println("");	
			kcData.close();
			
			outKc.close();
			projectInfo.close();
			
			ok = true;
			
		}
		catch (Exception e) 
		{
			  e.printStackTrace();
		}
		return ok;
	}
	public boolean writeXn5XpiFiles(String xn5FilePrefix, int projectId, String projectName, int project_type, String xn5_cells_table_name) {
		boolean ok = false;
		try {
			int numGrids = 1;
			boolean gecros = true;
			boolean gecros_h = false;
			boolean ceres = false;

			int batchSize = 10000;
			Boolean everything_retrieved = false;
			int repcount = 0;
			
			
			String s0 = "SELECT startYear, endYear,  adaptive, max_daily_precip, elevationCorrectionType, elevationCorrectionClassSize, elevationInfoTableWeatherCells " //elevation correction class_size 
					+ " FROM simulation_projects_general "
					+ " WHERE simulation_project_id = "+ projectId + ";";
			ResultSet projectInfo = myConnection.query(s0);	
	
			if (! projectInfo.first()) {
				JOptionPane.showMessageDialog(null, "Error: Missing general information on simulation project.");

				throw new Exception("Error: Missing general information on simulation project.");
			}

			
			int elevationCorrectionType = projectInfo.getInt("elevationCorrectionType");
			int elevCorrectionClassSize = projectInfo.getInt("elevationCorrectionClassSize");
			
			
			ResultSet gridInfo = getGridsInfoForProject(projectId);	
			if (! gridInfo.first()) {
				JOptionPane.showMessageDialog(null, "Error: Missing grid information for simulation project.");

				throw new Exception("Error: Missing grid information for simulation project.");
			}
			
			while (! everything_retrieved)  { //RETRIEVING BATCH BY BATCH TO AVOID MEMORY PROBLEMS FOR LARGE MAPS
			
				String s1 = "SELECT xn5_cell_x, xn5_cell_y, climate_file, weather_table_name, weather_station_id,  ROUND(alt / " +elevCorrectionClassSize + " ) * "+ elevCorrectionClassSize+ " AS alt "
							+" FROM `"+ xn5_cells_table_name +"` t1"
							+" WHERE simulation_project_id = " + projectId
							+" ORDER BY xn5_cell_x, xn5_cell_y"
							+ " LIMIT " + (repcount * batchSize) + ",  " + batchSize+";";
				ResultSet cellList = myConnection.query(s1);	
				
				/*if (! cellList.first()) {
					throw new Exception("Error: No map cells/plots defined for simulation project.");
				}*/	
				
				if (repcount == 0 && !cellList.isBeforeFirst() ) //Note: if directly after retrieval isBeforeFirst returns false, the cell list is empty
				{
					throw new Exception("No cells defined for project!");
				}
				else if (repcount > 0 && !cellList.isBeforeFirst() )
				{
					everything_retrieved = true;
					break;
				}
				
				cellList.first();
	
				gridInfo.beforeFirst();
				
				for (int curGrid = 0; curGrid < numGrids; ++curGrid)
				{
					cellList.beforeFirst();
					gridInfo.next();
					if(gridInfo.getString("plant_model").equalsIgnoreCase("gecros")) {
						gecros_h = ceres = false;
						gecros = true;
					}
					else if(gridInfo.getString("plant_model").equalsIgnoreCase("gecros_h")) {
						gecros = ceres = false;
						gecros_h = true;
					}
					else {
						JOptionPane.showMessageDialog(null, "Error: Invalid plant_model in grid info.");

						throw new Exception("Error: Invalid plant_model in grid info.");
					}
					while (cellList.next())
					{
		
						ResultSet freeText = getXpiContentForProject(projectId, curGrid);
						
						if (! freeText.first()) {
							throw new Exception("Error: Missing xpi content for grid " + curGrid +".");
						}
						
						int curX = cellList.getInt("xn5_cell_x");
						int curY = cellList.getInt("xn5_cell_y");
						
						System.out.println("Writing xpi for cell " + curX+ ", " +curY+"...");
	
						
						String climfile; 
						if ( project_type != 3 && cellList.getInt("weather_station_id") > -1 
								&& cellList.getString("weather_table_name").compareToIgnoreCase("") != 0)  {
							
							String[] tables =cellList.getString("weather_table_name").split(";");
							climfile = "$<$PROJECT_PATH/"+ tables[0] +"_"+ cellList.getInt("weather_station_id");
							if (elevationCorrectionType > 0 && elevCorrectionClassSize > -1) {
								climfile += String.format("%04d", cellList.getInt("alt") );
							}
							climfile +=".csv$>";
						}
						else {
	
							climfile = cellList.getString("climate_file");
	
						}
						
	
		
						String xn5FileNameXpn = xn5FilePrefix + "_" + curGrid + "_" + curX + "_" + curY + "_0.xpi" ;		
						PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(xn5FileNameXpn)));
						
						out.println("");
						out.println(freeText.getString("xpiText"));
						
						out.println("");
						out.println("[Expert N Standard Read INI]");
						out.println("climate file = " + climfile);
						
						
						if (gecros) {
							out.println("");
							out.println("[plant]");
							out.println("potential transpiration = Penman Monteith");
							out.println("phenological development = GECROS Development");
							out.println("biomass growth = GECROS BiomassGrowth");
							out.println("canopy gross photosynthesis = GECROS Gross Photosynthesis");
							out.println("canopy formation = GECROS Canopy Formation");
							out.println("root length growth = GECROS Root System Formation");
							out.println("crop senescence = GECROS Crop Senescence");
							out.println("nitrogen demand = GECROS Nitrogen Demand");
							out.println("nitrogen uptake = GECROS Nitrogen Uptake");
							out.println("actual transpiration = GECROS Actual Transpiration");						
							out.println("");
							
							out.println("[GECROS Development]");
							out.println("");
	
							out.println("[gecros]");
							out.println("filename = $<$PROJECT_PATH/$PROJECT_NAME_$REG_STR_crop_rotation.ini$>");
							out.println("");
							
							out.println("[GECROS BiomassGrowth]");
							out.println("");
	
							out.println("[GECROS Nitrogen Demand]");
							out.println("");
	
							out.println("[GECROS Nitrogen Uptake]");
							out.println("");
	
							out.println("[GECROS Root System Formation]");
							out.println("");
	
						}
						else if (gecros_h) {
							out.println("");
							out.println("[plant]");
							out.println("potential transpiration = Penman Monteith");
							out.println("phenological development = GECROS_h Development");
							out.println("biomass growth = GECROS_h BiomassGrowth");
							out.println("canopy gross photosynthesis = GECROS_h Gross Photosynthesis");
							out.println("canopy formation = GECROS_h Canopy Formation");
							out.println("root length growth = GECROS_h Root System Formation");
							out.println("crop senescence = GECROS_h Crop Senescence");
							out.println("nitrogen demand = GECROS_h Nitrogen Demand");
							out.println("nitrogen uptake = GECROS_h Nitrogen Uptake");
							out.println("actual transpiration = GECROS_h Actual Transpiration");						
							out.println("");
							
							out.println("[GECROS_h Development]");
							out.println("");
	
							out.println("[gecros]");
							out.println("filename = $<$PROJECT_PATH/$PROJECT_NAME_$REG_STR_crop_rotation.ini$>");
							out.println("");
							
							out.println("[GECROS_h BiomassGrowth]");
							out.println("");
	
							out.println("[GECROS_h Nitrogen Demand]");
							out.println("");
	
							out.println("[GECROS_h Nitrogen Uptake]");
							out.println("");
	
							out.println("[GECROS_h Root System Formation]");
							out.println("");
	
						}
						else if (ceres) {
							out.println("[CERES]");
							out.println("[ceres]");
							out.println("filename = $<$PROJECT_PATH/$PROJECT_NAME_$REG_STR_crop_rotation.ini$>");
							out.println("[plant]");
							out.println("potential transpiration = CERES");
							out.println("actual transpiration = CERES");
							out.println("phenological development = CERES");
							out.println("canopy gross photosynthesis = CERES");
							out.println("biomass growth = CERES");
							out.println("canopy formation = CERES");
							out.println("root length growth = CERES");
							out.println("nitrogen demand = CERES");
							out.println("nitrogen uptake = CERES");
							out.println("crop senescence = CERES");
					}
						
	
						out.println("[Hansen et al. (DAISY_Miner)]");
	
						out.println("[daisy_miner]");
						out.println("ini_filename = $<$PROJECT_PATH/$PROJECT_NAME_$REG_STR_daisy_miner_nitrogen.ini$>");
						
						
						if (project_type > 0) {
							out.println("");
							out.println("[other]");
							out.println("mpmas = mpmas coupling");
							out.println("");
						}
						freeText.close();
						out.close();
					}
					
					ok = true;
				}
				++repcount;
				cellList.close();
			}//END LOOP BATCH
		}
		catch (Exception e) 
		{
			  e.printStackTrace();
		}
		return ok;
	}
	public boolean writeXn5GecrosParametersFile(String xn5FilePrefixGecros, int plantParamId, String crop_code, String variety, String crop_name)
	{
		boolean ok = false;
		try {
									
			String xn5FileNameGecros;			
			if (variety.equals("Default"))	{
				xn5FileNameGecros = xn5FilePrefixGecros + "__" + crop_name.toLowerCase() + "_gecros.ini";
			}
			else {
				xn5FileNameGecros = xn5FilePrefixGecros + "__" + crop_name.toLowerCase() + "_"+ firstLetterToLowerCase(variety) + "_gecros.ini";
			}
			
			String s = "SELECT *, variety as varietyName FROM plant_parameterization_gecros t1"
					+ " JOIN residue_partitioning t2 ON t1.crop_code = t2.crop_code "
					+ " WHERE plant_param_id = " + plantParamId
					+ " AND t1.crop_code =  '" + crop_code + "' AND variety = '" + variety + "'"
					+";";
			//System.out.println(s);
			ResultSet rs = myConnection.query(s);
			
			if (rs.first())
			{
				PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(xn5FileNameGecros)));
				out.println("[control]");
				out.println("option = 1");
				
				out.println("");
				out.println("[cropcode]");
				
				printSimpleIniField("variety", out, rs);
				out.println("# Oekotyp der Kulturart");

				printSimpleIniField("ECOTYPE", out, rs);
				out.println("# Typ der Tageslaenge");
				printSimpleIniField("DAYLENGTH", out, rs);
				//						DAYLENGTH = LDPs
				out.println("# Typ der Blattwinkelverteilung");
				printSimpleIniField("LEAFANGLE", out, rs);
				//LEAFANGLE = SPH
				out.println("");
				out.println("[croptype]");
				out.println("#LEGUME = +1 for leguminous crops; = -1 for non-leguminous crops");
				out.println("#C3C4 = +1 for C3 crops; = -1. for C4 crops");
				out.println("#DETER = +1 for determinate crops; = -1 for indeterminate crops");
				out.println("#SLP = +1 for short-day crops; = -1 for long-day crops");
				out.println("#LODGE = +1 for cases with lodging; = -1 for cases without lodging");
				out.println("#VERN = +1 for vernalisation; =-1 without vernalisation");

				printSimpleIniField("LEGUME", out, rs, true);
				printSimpleIniField("C3C4", out, rs, true);						
				printSimpleIniField("DETER", out, rs, true);
				printSimpleIniField("SLP", out, rs, true);
				printSimpleIniField("LODGE", out, rs, true);
				printSimpleIniField("VERN", out, rs, true);
				out.println("");
				out.println("[biomass]");
				//# 80002
				out.println("# growth efficiency of vegetative organs [g C /(g C)]");
				out.println("# carbon fraction in vegetative organs [g C /g]");
				out.println("# efficiency of germination (Par.) [-] crop  table 2, p46");
				printSimpleIniField("CFV", out, rs);
				printSimpleIniField("EG", out, rs);
				printSimpleIniField("YGV", out, rs);
				out.println("");
				out.println("# fat fraction in storage organ (Par.)   [g/g]    crop  table 2, p46");
				out.println("# lignin fraction in storage organ (Par.)   [g/g]    crop  table 2, p46");
				out.println("# organic acid fraction in storage organ (Par.)   [g/g]    crop  table 2, p46");
				out.println("# minerals fraction in storage organ (Par.)   [g/g]    crop  table 2, p46");

				printSimpleIniField("FFAT", out, rs);
				printSimpleIniField("FLIG", out, rs);
				printSimpleIniField("FOAC", out, rs);
				printSimpleIniField("FMIN", out, rs);
				out.println("");
				out.println("[morphology]");
				//# 80003
				out.println("#Leaf width [m] crop table 3, p47");
				out.println("#maximal root depth [cm] crop table 3, p47");
				out.println("#stem dry weight per unit of plant height [g /m^2 /m] crop  table 3, p47");
				
				printSimpleIniField("LWIDTH", out, rs);
				printSimpleIniField("RDMX", out, rs);
				printSimpleIniField("CDMHT", out, rs);
				out.println("");
		out.println("[phenology]");
//						# 80003
				out.println("# base temperature for phenology      [°C] crop  table 3, p47");
				out.println("# optimum temperature for phenology   [°C] crop  table 3, p47");
				out.println("# ceiling temperature for phenology   [°C] crop  table 3, p47");
				out.println("# curvature for temperature response [-] default table 4, p49");
				
				printSimpleIniField("TBD", out, rs);
				printSimpleIniField("TOD", out, rs);
				printSimpleIniField("TCD", out, rs);
				printSimpleIniField("TSEN", out, rs);
				out.println("");
				
				out.println("# Optimal vernalization units [d], marker 200030");
				printSimpleIniField("OptVernDays", out, rs);
						
				out.println("# Koeffizient der Vernalisation");
				printSimpleIniField("VernCoeff", out, rs);
				out.println("# Minimale, optimale, maximale Temperatur [deg C] fuer Vernalisation [deg C], Marker 2000033");
				printSimpleIniField("TempMinDevVern", out, rs);
				printSimpleIniField("TempOptDevVern", out, rs);
				printSimpleIniField("TempMaxDevVern", out, rs);
				
				
				out.println("[nitrogen]");
//						# 80004
				out.println("# maximum crop nitrogen uptake [g N /m^2 /d] default table 4, p49");
				out.println("# min. N conc. in roots        [g N /g]     crop  table 3, p47");
				out.println("# min. N conc. in stems        [g N /g]     crop  table 3, p47");
				out.println("# min. specific leaf N content [g N /m^2]   crop  table 3, p47");
				out.println("# initial N conc. in living leaves [g N /g] crop  table 3, p47");
				out.println("# specific leaf area constant [m^2 leaf /g] crop  table 3, p47");
				out.println("# carbon cost of symbiotic nitrogen fixation [g C /g N] const table 1, p45");
				
				printSimpleIniField("NUPTX", out, rs);
				printSimpleIniField("RNCMIN", out, rs);
				printSimpleIniField("STEMNC", out, rs);
				printSimpleIniField("SLNMIN", out, rs);
				printSimpleIniField("LNCI", out, rs);
				printSimpleIniField("SLA0", out, rs);
				printSimpleIniField("CCFIX", out, rs);
				out.println("");
				out.println("[photosynthesis]");
				//# 80005
				out.println("# activation energy of JMAX [J/mol] crop  table 3, p47");
				out.println("# slope of lin.rel. btw. VCMAX and leaf N [umol /s /(g N)] crop specific ???");
				out.println("# slope of lin.rel. btw. VJMAX and leaf N [umol /s /(g N)] crop specific ???");
				out.println("# convexity for light response electron transport (J2) in photosynthesis [-]  const table 1, p45 (photo)");
				
				printSimpleIniField("EAJMAX", out, rs);
				printSimpleIniField("XVN", out, rs);
				printSimpleIniField("XJN", out, rs);
				printSimpleIniField("THETA", out, rs);
				out.println("");
				out.println("[genotype]");
				//# 80006
				out.println("# Seed weight [g /seed]");
				out.println("# Standard seed (storage organ) N concentration [g N /g]");
				out.println("# Leaf angle from horizontal [degree]");
				out.println("# Maximal crop height [m]");
				out.println("# Minimum thermal days for vegetative phase (d)");
				out.println("# Minimum thermal days for reproductive phase (d)");
				
				printSimpleIniField("SEEDW", out, rs);
				printSimpleIniField("SEEDNC", out, rs);
				printSimpleIniField("BLD", out, rs);
				printSimpleIniField("HTMX", out, rs);
				printSimpleIniField("MTDV", out, rs);
				printSimpleIniField("MTDR", out, rs);
				
				out.println("");
				out.println("[photoperiod]");
				//#Dev.stage start of photo-sensitive phase    (-)        deflt table 4, p49
				printSimpleIniField("SPSP", out, rs);
				//#Dev.stage end of photo-sensitive phase      (-)        deflt table 4, p49
				printSimpleIniField("EPSP", out, rs);
				//#Incl. of sun angle for photoper.daylgth.  (degree)     const table 1, p45
				printSimpleIniField("INSP", out, rs);
				printSimpleIniField("PSEN", out, rs);
				
				out.println("");
				out.println("[default]");
				//# 80007
				out.println("# Fraction of sigmoid curve inflexion in entire height growth period  [-] default table 4, p49");
				out.println("# Fraction of sigmoid curve inflexion in entire seed growth period    [-] default table 4, p49");
				out.println("# Dev.Stage of seed number determining period for indeterminate crops [-] default table 4, p49");
				out.println("# Critical root weight density [g /m^2 /(cm depth)]  const table 1, p45 (others)");
				
				printSimpleIniField("PMEH", out, rs);
				printSimpleIniField("PMES", out, rs);
				printSimpleIniField("ESDI", out, rs);
				printSimpleIniField("WRB", out, rs);
				
				out.println("");
				out.println("[sensitivity]");
				//# 80008
				out.println("# Ambient CO2 concentration [umol /mol]");
				out.println("# Coefficient of radiation [-]");
				out.println("# Coefficient of vapor pressure [-]");
				out.println("# Coefficient of temperature [°C]");
				
				printSimpleIniField("CO2A", out, rs);
				printSimpleIniField("COEFR", out, rs);
				printSimpleIniField("COEFV", out, rs);
				printSimpleIniField("COEFT", out, rs);

				out.println("# Initial C fraction in shoot [g C /(g C)]");
				out.println("# Initial N fraction in shoot [g N /(g N)]");
				
				printSimpleIniField("FCRSH", out, rs);
				printSimpleIniField("FNRSH", out, rs);

				out.println("# Seed N from non-structural N [-]");
				out.println("# Initial factor of N conc. of seed filling [-]");
				out.println("# Final factor of N conc. of seed filling   [-]");
				out.println("# Dev.Stage of fastest transition from initial to final factor [-]");
				
				printSimpleIniField("PNPRE", out, rs);
				printSimpleIniField("CB", out, rs);
				printSimpleIniField("CX", out, rs);
				printSimpleIniField("TM", out, rs);

				out.println("");
				out.println("[residue partition]");
				out.println("# Dev.Stage of fastest transition from initial to final factor [-]");
				printSimpleIniFieldWithSemicolon("VarietyName", out, rs);
				printSimpleIniFieldWithSemicolon("ResidueToAMO1_frac", out, rs);
				printSimpleIniFieldWithSemicolon("C_fromDeadleaf_frac", out, rs);
				printSimpleIniFieldWithSemicolon("N_fromDeadleaf_frac", out, rs);
				
				if (rs.getString("crop_code").equals("SB") ) {
					out.println("");
					out.println("[sugarbeet specific]");
					out.println("#sink strength of beet ");
					printSimpleIniField("SINKBEET", out, rs);
					out.println("#seed efficiency ");
					printSimpleIniField("EFF", out, rs);
					out.println("#seed carbon fraction");
					printSimpleIniField("CFS", out, rs);
					out.println("");	
				}
						
						
						
				out.close();
				rs.close();
				ok = true;
			}
			else
			{
				throw new Exception("Error in writeXn5GecrosParametersFile: Query returned empty: " + s );
			}
		}
		catch (Exception e) 
		{
			  e.printStackTrace();
			System.exit(0);
		}
		return ok;
	}
	public boolean writeBemsCropManagementInfo (String xn5FilePrefix, int projectId,  int adaptive) { 
		boolean ok = false;
		try {
			
			System.out.println("Writing plant management yaml file ...");

			String s1 = "( SELECT process_id, xn_crop_code, t3.crop_name as xn_crop_name, IFNULL(xn_variety, 'Default') as xn_variety" 
						+ " FROM  for1695_mpmas.tbl_xp_process t1" 
						+ " JOIN  for1695_mpmas.link_xn_crop_code t2 ON t1.crop_id = t2.crop_id "
						+ " JOIN for1695_expertN.info_crops t3 ON t2.xn_crop_code = t3.crop_code "
						+ " JOIN for1695_expertN.simulation_projects_gecros_crops_included t4 ON t4.simulation_project_id = " + projectId
							+ " AND t3.crop_code = t4.crop_code "
						+ " WHERE i2 > 0 )"
						 + " UNION "
						 + " (SELECT 170, 'BA', 'Barley', 'Wintergerste')"
						+ " ORDER BY process_id";
			ResultSet activityList = myConnection.query(s1);
						
			String s2 = "(SELECT process_id, t2.quantity * t3.grains_per_unit /10000 as sowing_density, t2.row_spacing, t2.sowing_depth, " +
					"					xn_year, xn_month, xn_day, adapt_date_days"
						 + " FROM for1695_mpmas.tbl_xp_process t1 " 
						 + " JOIN for1695_mpmas.tbl_xp_crop_tillharvest t2 ON t1.crop_id = t2.crop_id AND t1.tillharvest_id = t2.tillharvest_id AND t2.tillharvest_phase_id = 6 "
						 + " JOIN for1695_mpmas.tbl_consumables t3 ON t2.goods_id = t3.goods_id"
						 + " WHERE i2 > 0 )"
						 + " UNION "
						 + " (SELECT 170, 0.0001,100,4, 1, 11, 20,20)"
						 + " ORDER BY process_id";	
			ResultSet sowingList = myConnection.query(s2);
			
			if(!sowingList.first() )
			{
				throw new Exception("Error: no sowing information found!");
			}
			
			ResultSet adaptSowingList = null;
			if (adaptive > 1) {
				String s2a = "SELECT    t1.process_id, `t2`.`sowingDayFormula`,`t2`.`basetemp`,`t2`.`tgzthr`, `t2`.`gddthr`, `t2`.`seed_cons`"
							+ ",`t2`.`seed_coef_tgz`,`t2`.`seed_coef_gdd`" 
							+ "  FROM for1695_mpmas.tbl_xp_process t1"
							+ "		LEFT JOIN for1695_mpmas.tbl_adaptation_sowing t2"
							+ "			ON t1.adapt_sowing_code = t2.adapt_sowing_code" 
							+ "		ORDER BY process_id;";
				adaptSowingList = myConnection.query(s2a);
				adaptSowingList.first();
			}
			
			String s3 = "SELECT process_id, t4.soil_management_equipment_code as tillage_code, soil_management_equipment_depth  as depth, xn_year, xn_month, xn_day , adapt_date_type, adapt_date_days"
						+ " FROM for1695_mpmas.tbl_xp_process t1 " 
						+ " JOIN for1695_mpmas.tbl_xp_crop_tillharvest t2" 
						+ "		ON t1.crop_id = t2.crop_id AND t1.tillharvest_id = t2.tillharvest_id" 
						+ "	JOIN for1695_mpmas.link_xn_tillage t3  ON t2.worktype_id = t3.worktype_id"
						+ " JOIN for1695_expertN.codes_tillage_implements t4 ON t3.xn_soil_management_equipment_id = t4.soil_management_equipment_id "
						+ " ORDER BY process_id, xn_year, xn_month, xn_day";
			
			ResultSet tillageList = myConnection.query(s3);
			
			if(!tillageList.first() )
			{
				throw new Exception("Error: no tillage information found!");
			}
			
			String s4 = "SELECT process_id, t4.fertilizer_mineral_code as minfert_code, "
					+ "		t4.ammonium_n_content * t2.quantity as nh4n,  t4.nitrate_n_content* t2.quantity  as no3n, t4.urea_n_content * t2.quantity as urea,"
					+ "		xn_year, xn_month, xn_day, adapt_date_bbch, adapt_nmin_factor, adapt_nmin_ref, adapt_nmin_depth "
					+ "	 FROM for1695_mpmas.tbl_xp_process t1" 
					+ "		JOIN for1695_mpmas.tbl_xp_crop_fertilization t2" 
					+ " 		ON t1.crop_id = t2.crop_id AND t1.fertilization_id = t2.fertilization_practice_id" 
					+ "		JOIN for1695_mpmas.link_xn_fertilizer t3  ON t2.fertilizer_id = t3.goods_id"
					+ "		JOIN for1695_expertN.codes_fertilizer_mineral t4 ON t3.xn_fertilizer_mineral_id = t4.fertilizer_mineral_id " +
					"			AND t4.skip != 1	"	
					+ "    ORDER BY process_id, xn_year, xn_month, xn_day";
			
			
			ResultSet minFertList = myConnection.query(s4);
			minFertList.first();
			
			String s5 = "SELECT process_id, t4.fertilizer_organic_code as orgfert_code, "
//					+ "		t4.ammonium_n_content * t2.quantity as nh4n,  t4.amount_org_substance* t2.quantity  as organic_substance, " +
					+ "		t4.ammonium_n_content * t2.quantity as nh4n,  t4.total_n_content* t2.quantity  as n_tot_org, " +
					"		t4.dry_matter_content * t2.quantity as dry_matter,"
					+ "		xn_year, xn_month, xn_day "
					+ "	 FROM for1695_mpmas.tbl_xp_process t1" 
					+ "		JOIN for1695_mpmas.tbl_xp_crop_fertilization t2" 
					+ " 		ON t1.crop_id = t2.crop_id AND t1.fertilization_id = t2.fertilization_practice_id" 
					+ "		JOIN for1695_mpmas.link_xn_fertilizer t3  ON t2.fertilizer_id = t3.goods_id"
					+ "		JOIN for1695_expertN.codes_fertilizer_organic t4 ON t3.xn_fertilizer_organic_id = t4.fertilizer_organic_id "	
					+ "    ORDER BY process_id, xn_year, xn_month, xn_day";
			
			
			ResultSet orgFertList = myConnection.query(s5);
			orgFertList.first();
			
			String s8 = "SELECT process_id, xn_year, xn_month, xn_day, yield_factor, stover_factor, stem_factor, IF(biomass_removed_yno, 'Yes','No') as biomass_removed"
						+ ",  adapt_harvest_bbch1, adapt_harvest_bbch1_extra_days, adapt_harvest_bbch2, adapt_harvest_bbch2_extra_days "
						+ ",   adapt_harvest_latest_day, adapt_harvest_latest_month, adapt_harvest_latest_year"
						+ " FROM for1695_mpmas.tbl_xp_process t1"  
						+ " JOIN for1695_mpmas.tbl_xp_crop_tillharvest t2" 
						+ "  ON t1.crop_id = t2.crop_id AND t1.tillharvest_id = t2.tillharvest_id" 
						+ " 	AND t2.tillharvest_phase_id = 7"
						+ "    JOIN for1695_mpmas.link_xn_crop_code t3 ON t1.crop_id = t3.crop_id"
						+ " 		WHERE i2 > 0"
						+ " UNION (SELECT 170,  2, 1, 12,0,0,0,'No', NULL, NULL, NULL, NULL, NULL, NULL, NULL) "
						+ " ORDER BY process_id";
			ResultSet harvestList = myConnection.query(s8);
			harvestList.first();
			
			String s9 = "SELECT process_id, t4.xn_crop_code, t5.crop_name,t4.cover_variety, t2.quantity * t3.grains_per_unit /10000 as sowing_density, t2.row_spacing, t2.sowing_depth, " +
					"	xn_year, xn_month, xn_day, adapt_date_days " 
						+ "  FROM for1695_mpmas.tbl_xp_process t1 "  
						+ "		JOIN for1695_mpmas.tbl_xp_crop_tillharvest t2" 
						+ "			ON t1.crop_id = t2.crop_id AND t1.tillharvest_id = t2.tillharvest_id" 
						+ " 			AND t2.tillharvest_phase_id = 10"
						+ "		LEFT JOIN for1695_mpmas.tbl_consumables t3 ON t2.goods_id = t3.goods_id"
						+ " 	LEFT JOIN for1695_mpmas.link_xn_crop_code t4 ON t2.goods_id = t4.crop_id"
				        + "  	LEFT JOIN for1695_expertN.info_crops t5 ON t4.xn_crop_code = t5.crop_code"

						+ "		WHERE i2 > 0 " 
						+ "		ORDER BY process_id";	
			ResultSet coverList = myConnection.query(s9);
			coverList.first();
			
			String s10 = "SELECT process_id,  t6.tillharvest_phase_id, t6.xn_year, t6.xn_month, t6.xn_day "
						 + " FROM for1695_mpmas.tbl_xp_process t1 "  
						 + " 	JOIN for1695_mpmas.tbl_xp_crop_tillharvest t2" 
						 + "		ON t1.crop_id = t2.crop_id AND t1.tillharvest_id = t2.tillharvest_id" 
						 + "			AND t2.tillharvest_phase_id = 10"
						 + "	JOIN  for1695_mpmas.tbl_xp_crop_tillharvest t6" 
						 + "			ON t1.crop_id = t6.crop_id AND t1.tillharvest_id = t6.tillharvest_id" 
						 + "				AND t6.tillharvest_phase_id IN (3,4,5,6) AND ((t6.xn_year > t2.xn_year)"
						 + "				OR (t2.xn_month < t6.xn_month) OR (t2.xn_day < t6.xn_day))"
						 + "	WHERE i2 > 0" 
						 + "	ORDER BY process_id, t6.xn_year, t6.xn_month, t6.xn_day";	
			ResultSet coverPloughUnderList = myConnection.query(s10);
			coverPloughUnderList.first();
			
			PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(xn5FilePrefix+"_MpmasLuaToXnParameters.yaml")));

			out.println("---");

			while (activityList.next())//move cursor to next value (including the first value in the first loop)
			{
				out.println("- id : " + activityList.getString("process_id"));
				out.println("  crop-code : " + activityList.getString("xn_crop_code"));
				out.println("  crop-name : " + activityList.getString("xn_crop_name"));
				out.println("  variety   : " + activityList.getString("xn_variety"));

				int curProcId = activityList.getInt("process_id");
				
				while(  sowingList.getInt("process_id") < curProcId && ! sowingList.isLast()) { 
					sowingList.next();
				}
				if (sowingList.getInt("process_id") != curProcId) {
					throw new Exception("Error: no sowing information found for process id " +String.valueOf(curProcId) + "!");
				}
				out.println("  sowing  :");
				out.println("    date        : "+String.format("%02d",sowingList.getInt("xn_day")) +"-"+String.format("%02d",sowingList.getInt("xn_month"))+"-"+String.format("%02d",sowingList.getInt("xn_year")));
				if (sowingList.getInt("adapt_date_days") > 0 ) {
					out.println("    max-days-delay : "+sowingList.getString("adapt_date_days"));
				}
				else {
					out.println("    max-days-delay : 0");
				}
				out.println("    row-spacing : "+sowingList.getString("row_spacing"));
				out.println("    depth       : "+sowingList.getString("sowing_depth"));
				out.println("    density     : "+sowingList.getString("sowing_density"));
				
				if (adaptive > 1 && adaptSowingList != null) {
					while(  adaptSowingList.getInt("process_id") < curProcId && ! adaptSowingList.isLast()) { 
						adaptSowingList.next();
					}
					if (adaptSowingList.getInt("process_id") == curProcId && adaptSowingList.getInt("sowingDayFormula") > -1 && ! adaptSowingList.wasNull() ) {
						out.println("    adaptive  :");
						out.println("      type: "+adaptSowingList.getInt("sowingDayFormula"));
						switch(adaptSowingList.getInt("sowingDayFormula")) {
							case 0: 				
								out.println("      tgzthr: "+adaptSowingList.getString("tgzthr"));
								out.println("      gddthr: "+adaptSowingList.getString("gddthr"));
								out.println("      const: "+adaptSowingList.getString("seed_cons"));
								out.println("      coef-tgz: "+adaptSowingList.getString("seed_coef_tgz"));
								out.println("      coef-gdd: "+adaptSowingList.getString("seed_coef_gdd"));
								break;
							case 1: 				
								out.println("      basetemp: "+adaptSowingList.getString("basetemp"));
								out.println("      tgzthr: "+adaptSowingList.getString("tgzthr"));
								out.println("      const: "+adaptSowingList.getString("seed_cons"));
								out.println("      coef-tgz: "+adaptSowingList.getString("seed_coef_tgz"));						
								break;
							case 2:
							case 3: 				
								out.println("      basetemp: "+adaptSowingList.getString("basetemp"));
								out.println("      gddthr: "+adaptSowingList.getString("gddthr"));
								out.println("      const: "+adaptSowingList.getString("seed_cons"));
								out.println("      coef-gdd: "+adaptSowingList.getString("seed_coef_gdd"));
							break;
						}
					}
				}
				
				
				
				
				
				while(  harvestList.getInt("process_id") < curProcId && ! harvestList.isLast()) { 
					harvestList.next();
				}
				if (harvestList.getInt("process_id") != curProcId) {
					throw new Exception("Error: no harvest information found for process id " +String.valueOf(curProcId) + "!");
				}
				out.println("  harvest  :");
				out.println("    yield-factor  : "+harvestList.getString("yield_factor"));
				out.println("    stover-factor : "+harvestList.getString("stover_factor"));
				out.println("    stem-factor : "+harvestList.getString("stem_factor"));
				out.println("    biomass-removed : "+harvestList.getString("biomass_removed"));
				if (adaptive > 0 && harvestList.getInt("adapt_harvest_bbch1") > 0) {
					out.println("    adaptive  : ");
					out.println("      bbch1      : "+harvestList.getString("adapt_harvest_bbch1"));
					out.println("      bbch1-extra-days : "+harvestList.getString("adapt_harvest_bbch1_extra_days"));
					out.println("      bbch2      : "+harvestList.getString("adapt_harvest_bbch2"));
					out.println("      bbch2-extra-days : "+harvestList.getString("adapt_harvest_bbch2_extra_days"));
					out.println("    date          : "+String.format("%02d",harvestList.getInt("adapt_harvest_latest_day")) +"-"+String.format("%02d",harvestList.getInt("adapt_harvest_latest_month"))+"-"+String.format("%02d",harvestList.getInt("adapt_harvest_latest_year")));

				}
				else {
					out.println("    date          : "+String.format("%02d",harvestList.getInt("xn_day")) +"-"+String.format("%02d",harvestList.getInt("xn_month"))+"-"+String.format("%02d",harvestList.getInt("xn_year")));
				}
				
				while( ! coverList.isAfterLast() &&  coverList.getInt("process_id") < curProcId && ! coverList.isLast()) { 
					coverList.next();
				}
				if (coverList.getInt("process_id") == curProcId) {		
					out.println("  cover-crop  :");
					out.println("    cover-code    : "+coverList.getString("xn_crop_code"));
					out.println("    cover-name    : "+coverList.getString("crop_name"));
					out.println("    cover-variety : "+coverList.getString("cover_variety"));
					out.println("    cover-sowing  : ");
					out.println("      row-spacing : "+coverList.getString("row_spacing"));
					out.println("      depth       : "+coverList.getString("sowing_depth"));
					out.println("      density     : "+coverList.getString("sowing_density"));
					out.println("      date        : "+String.format("%02d",coverList.getInt("xn_day")) +"-"+String.format("%02d",coverList.getInt("xn_month"))+"-"+String.format("%02d",coverList.getInt("xn_year")));
					if (coverList.getInt("adapt_date_days") > 0 ) {
						out.println("      max-days-delay : "+coverList.getString("adapt_date_days"));
					}
					else {
						out.println("      max-days-delay : 0");
					}
					
					out.println("    plough-under  :");
					
					while( ! coverPloughUnderList.isAfterLast() &&  coverPloughUnderList.getInt("process_id") < curProcId && ! coverPloughUnderList.isLast()) { 
						coverPloughUnderList.next();
					}
					if (coverPloughUnderList.getInt("process_id") == curProcId) {		
						out.println("      date    : "+String.format("%02d",coverPloughUnderList.getInt("xn_day")) +"-"+String.format("%02d",coverPloughUnderList.getInt("xn_month"))+"-"+String.format("%02d",coverPloughUnderList.getInt("xn_year")));
					}

				}
				
				while( ! tillageList.isAfterLast() &&  tillageList.getInt("process_id") < curProcId && ! tillageList.isLast()) { 
					tillageList.next();
				}
				//System.out.println("Now here" + curProcId);
				
				//System.out.println("At "+tillageList.getInt("process_id") + " in tillage list");
				if (! tillageList.isAfterLast() && tillageList.getInt("process_id") == curProcId) {
					out.println("  tillage  :");
				}
				
				while (! tillageList.isAfterLast() && tillageList.getInt("process_id") == curProcId) {
					
					out.println("    - date    : "+String.format("%02d",tillageList.getInt("xn_day")) +"-"+String.format("%02d",tillageList.getInt("xn_month"))+"-"+String.format("%02d",tillageList.getInt("xn_year")));
					out.println("      code    : "+tillageList.getString("tillage_code"));
					out.println("      depth   : "+tillageList.getString("depth"));
					if (adaptive > 0 && tillageList.getInt("adapt_date_type") > 0 ) {
						String typeString;
						switch(tillageList.getInt("adapt_date_type")) {
							case 1: typeString = "before-sowing"; break;
							case 2: typeString = "before-cover"; break;
							case 3: typeString = "after-harvest"; break;
							case 4: typeString = "before-sowing"; break;
							case 5: typeString = "before-cover"; break;
							default:
								throw new Exception("Error: invalid adapt_date_type");
						}
						out.println("      adaptive  : ");
						out.println("        type      : "+typeString);
						if ( tillageList.getInt("adapt_date_type") == 4 || tillageList.getInt("adapt_date_type") == 5) {
							out.println("        days      : 1");
						}
						else {
							out.println("        days      : "+tillageList.getString("adapt_date_days"));
						}
					}
					tillageList.next();
				}
				
				
				while( ! minFertList.isAfterLast() &&  minFertList.getInt("process_id") < curProcId && ! minFertList.isLast()) { 
					minFertList.next();
				}
				if (! minFertList.isAfterLast() && minFertList.getInt("process_id") == curProcId)
					out.println("  mineral-fertilization  :");

				int lastBBCH = 0;
				while (! minFertList.isAfterLast() && minFertList.getInt("process_id") == curProcId) {
					out.println("    - date    : "+String.format("%02d",minFertList.getInt("xn_day")) +"-"+String.format("%02d",minFertList.getInt("xn_month"))+"-"+String.format("%02d",minFertList.getInt("xn_year")));
					out.println("      code    : "+minFertList.getString("minfert_code"));
					out.println("      nh4n    : "+minFertList.getString("nh4n"));
					out.println("      no3n    : "+minFertList.getString("no3n"));
					out.println("      urea    : "+minFertList.getString("urea"));
					if (adaptive > 0 && ( minFertList.getInt("adapt_date_bbch") > 0 || minFertList.getDouble("adapt_nmin_factor") > 0)		) {						
						out.println("      adaptive    : ");

						if ( minFertList.getInt("adapt_date_bbch") > 0 ) {
							if ( minFertList.getInt("adapt_date_bbch") < lastBBCH) {
								throw new Exception("Error: inconsistent ordering of mineral fertilization: Higher BBCH before lower one for LUA " +String.valueOf(curProcId) + "!");
	
							}					
							out.println("        bbch      : "+minFertList.getString("adapt_date_bbch"));
							lastBBCH = minFertList.getInt("adapt_date_bbch");
						}
						
						if(minFertList.getDouble("adapt_nmin_factor") > 0) {
							out.println("        nmin-factor    : "+minFertList.getString("adapt_nmin_factor"));
							out.println("        nmin-reference : "+minFertList.getString("adapt_nmin_ref"));
							out.println("        nmin-depth     : "+minFertList.getString("adapt_nmin_depth"));
							
							
						}
					}
					
					minFertList.next();
				}
				while( ! orgFertList.isAfterLast() &&  orgFertList.getInt("process_id") < curProcId && ! orgFertList.isLast()) { 
					orgFertList.next();
				}
				
				if (! orgFertList.isAfterLast() && orgFertList.getInt("process_id") == curProcId)
					out.println("  organic-fertilization  :");

				while (! orgFertList.isAfterLast() && orgFertList.getInt("process_id") == curProcId) {
					out.println("    - date               : "+String.format("%02d",orgFertList.getInt("xn_day")) +"-"+String.format("%02d",orgFertList.getInt("xn_month"))+"-"+String.format("%02d",orgFertList.getInt("xn_year")));
					out.println("      code               : "+orgFertList.getString("orgfert_code"));
					out.println("      nh4n               : "+orgFertList.getString("nh4n"));
					out.println("      dry-matter         : "+orgFertList.getString("dry_matter"));
	//				out.println("      organic-substance  : "+orgFertList.getString("organic_substance"));
					out.println("      n-tot-org          : "+orgFertList.getString("n_tot_org"));
					orgFertList.next();
				}
				
				
				out.println("");
			}
			out.println("...");
			out.println("");

			out.close();
			
			activityList.close();
			sowingList.close();
			if (adaptSowingList != null) 
					adaptSowingList.close();
			harvestList.close();
			coverList.close();
			coverPloughUnderList.close();
			tillageList.close();
			minFertList.close();
			orgFertList.close();
			
			ok = true;
		}
		catch (Exception e) 
		{
			  e.printStackTrace();
			  //System.exit(0);
		}
		return ok;
	}
	public boolean writeBEMSCropManagementMaps(String xn5FilePrefix, int projectID, String xn5_cells_table_name, String bemsManagTableName)
	{
		boolean ok = false;
		try {
			
			
			
			String s7 = "SELECT MAX(xn5_cell_x) as maxX, MAX(xn5_cell_y) as maxY FROM `"+ xn5_cells_table_name +"` WHERE simulation_project_id = " +projectID+";";
			ResultSet gridExtend = myConnection.query(s7);
			
			if (! gridExtend.first()) {
				throw new Exception("Error: Missing basic cell information on simulation project.");
			}
			
			String s0 = "SELECT startYear, endYear, startMonth, startDay, endMonth, endDay, plotSize " 
					+ " FROM simulation_projects_general "
					+ " WHERE simulation_project_id = "+ projectID + ";";
			ResultSet projectInfo = myConnection.query(s0);	
	
			if (! projectInfo.first()) {
				throw new Exception("Error: Missing general information on simulation project.");
			}
			
			for (int year = 0; year < projectInfo.getInt("endYear") - projectInfo.getInt("startYear") + 1 ; year ++) { //Note ( why + 1?): also in the last year make a map even if not finished so that simulation can run over August and produce last yield results
			
				System.out.println("Writing crop management map for year " +year+"...");

				
				String xn5FileNameLuMaps = xn5FilePrefix + "_fixedMapsLanduse"+ String.format("%02d", year) +".txt" ;
				
				PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(xn5FileNameLuMaps)));
				
				out.println("NCOLS "+ String.valueOf(gridExtend.getInt("maxX") + 1) );
				out.println("NROWS "+  String.valueOf(gridExtend.getInt("maxY") + 1)  );
				out.println("XLLCORNER 0");
				out.println("YLLCORNER 0");
				out.println("CELLSIZE 100");
				out.println("NODATA_VALUE -1");
				
				
				String s6 = "SELECT xn5_cell_x, xn5_cell_y, for1695_mpmas_process_id  " +
						"		FROM `"+bemsManagTableName+"` t1" +
						"					JOIN (SELECT crop_sequence_id, MAX(position) + 1 as N" +
						"							FROM for1695_expertN.crop_sequence_bems" +
						"								GROUP BY crop_sequence_id) s1" +
						"							ON t1.crop_sequence_id = s1.crop_sequence_id" +
						"								AND t1.simulation_project_id = " + projectID + 
						"					JOIN for1695_expertN.crop_sequence_bems t2" +
						"						ON t1.crop_sequence_id = t2.crop_sequence_id" +
						"								AND  MOD((t1.start_position + "+year+" ) , s1.N) = t2.position" +
						"			ORDER BY xn5_cell_y DESC, xn5_cell_x";
		
				ResultSet luList = myConnection.query(s6);
				luList.first();
				for (int curY = gridExtend.getInt("maxY"); curY >= 0; curY-- ) {
					for (int curX = 0; curX <=  gridExtend.getInt("maxX"); curX++ ) {	
					
						while(!luList.isLast() && (luList.getInt("xn5_cell_x") < curX ||  luList.getInt("xn5_cell_y") > curY)   ) { //fast forward to info for this cell ( 
							luList.next();
						}
						if (!luList.isAfterLast() && luList.getInt("xn5_cell_x") == curX &&  luList.getInt("xn5_cell_y") == curY ){
							out.print(luList.getString("for1695_mpmas_process_id")+ " ");
						}
						else {
							out.print("-1 ");
						}
					}
					out.print("\n");
				}	
				luList.close();	
				out. close();
						
			}
			gridExtend.close();
			projectInfo.close();
			
			ok = true;
		} 
		catch (Exception e) 
		{
			  e.printStackTrace();
			  //System.exit(0);
		}
		return ok;

	}	
		
	
	public boolean writeXn5CropManagementFiles(String xn5FilePrefix, int projectID)
	{
		boolean ok = false;
		try {
			
			/*String s0 = "SELECT startYear, endYear, startMonth, startDay, endMonth, endDay, plotSize " 
						+ " FROM simulation_projects_general "
						+ " WHERE simulation_project_id = "+ projectID + ";";
			ResultSet projectInfo = myConnection.query(s0);	

			if (! projectInfo.first()) {
				throw new Exception("Error: Missing general information on simulation project.");
			}
			
			int startYear = projectInfo.getInt("startYear");
			int startMonth = projectInfo.getInt("startMonth");
			int startDay = projectInfo.getInt("startDay");
			int endYear = projectInfo.getInt("endYear");
			int endMonth = projectInfo.getInt("endMonth");
			int endDay = projectInfo.getInt("endDay");
			*/
			
			
			String s6 = "SELECT  xn5_cell_x, xn5_cell_y "
					+ " FROM simulation_projects_xn5_cells_management t1"
					
					+ "   WHERE simulation_project_id = "+ projectID +
					
					" GROUP BY xn5_cell_x, xn5_cell_y"
					+ " ORDER BY xn5_cell_x, xn5_cell_y";
	
			ResultSet cellList = myConnection.query(s6);
			
			
			
			String s1 = "SELECT  xn5_cell_x, xn5_cell_y, year, position, " 
							+ " t1.crop_management_id, t1.crop_code as CropCode, t1.variety,"
							+ " sow_depth, row_dist, sow_dens, sow_date,  IFNULL(emerg_date,-99) as emerg_date, harvest_date,"
							+ " IF(max_biom_date = -99 OR max_biom_date IS NULL, harvest_date, max_biom_date) AS max_biom_date, " +
							 "IF(max_ro_date = -99 OR max_ro_date IS NULL, harvest_date, max_ro_date) AS max_ro_date, " +
							 "max_root_depth AS max_root, biom_remove, t3.crop_name as CropName"
							+ ", sow_date_year, IFNULL(emerg_date_year, -99) as emerg_date_year, " +
							"harvest_date_year, IF(max_biom_date_year = -99 OR max_biom_date_year IS NULL, harvest_date_year," +
							"max_biom_date_year) AS max_biom_date_year , IF(max_ro_date_year = -99 OR max_ro_date_year IS NULL, harvest_date_year,max_ro_date_year) AS max_ro_date_year,  " +
							"HTMX * 100 as max_plant_height "
							+ " FROM simulation_projects_xn5_cells_management t1"
							+ " JOIN crop_management_planting t2"
							+ "  ON t1.crop_management_id = t2.crop_management_id"
							+ "	   AND t1.crop_code = t2.crop_code"
							+ "    AND t1.variety = t2.variety "
						    + "  JOIN info_crops t3 On t1.crop_code = t3.crop_code "
						    + " JOIN simulation_projects_gecros_crops_included t4"
						    + "   ON t1.crop_code = t4.crop_code"
						    + " AND t1.variety = t4.variety "
							+ " AND t1.simulation_project_id =t4.simulation_project_id AND t4.include = 1"
							+ " JOIN plant_parameterization_gecros t5 ON"
							+ " t1.crop_code = t5.crop_code"
						    + "  AND t1.variety = t5.variety"
						    + "   AND t4.plant_param_id = t5.plant_param_id"
							+ "   WHERE t1.simulation_project_id = "+ projectID +
							" ORDER BY xn5_cell_x, xn5_cell_y, year, position;";
			
			ResultSet managementList = myConnection.query(s1);
			
			String s2 = "SELECT  xn5_cell_x, xn5_cell_y, year, position, " 
					+ " t1.crop_management_id, t1.crop_code, t1.variety, "
					+ " event_id, date, code, fertilizer, n_tot_min, no3n, nh4n, urea, date_year"
					+ " FROM simulation_projects_xn5_cells_management t1"
					+ " JOIN crop_management_mineral_fertilization t2 "
					+ "	ON t1.crop_management_id = t2.crop_management_id "
					+ "  AND t1.crop_code = t2.crop_code "
				    + "   AND t1.variety = t2.variety "
				    + "  WHERE simulation_project_id = " 	+ projectID +
					" ORDER BY xn5_cell_x, xn5_cell_y, year, position, STR_TO_DATE(CONCAT(year + date_year, '-', date), '%Y-%m-%d');";
			
			ResultSet minFertList = myConnection.query(s2);
			
			String s3 = " SELECT  xn5_cell_x, xn5_cell_y, year, position," 
						+ " t1.crop_management_id, t1.crop_code, t1.variety,"
						+ " event_id, date, code, fertilizer, 1.0 as amount, dry_matter, org_subst, n_tot_org, nh4n, date_year"
						+ " FROM simulation_projects_xn5_cells_management t1"
						+ " JOIN crop_management_organic_fertilization t2 "
						+ "  ON t1.crop_management_id = t2.crop_management_id "
						+  "  AND t1.crop_code = t2.crop_code "
						+  "  AND t1.variety = t2.variety "
						+  "  WHERE simulation_project_id ="	+ projectID +
								" ORDER BY xn5_cell_x, xn5_cell_y, year, position, STR_TO_DATE(CONCAT(year + date_year, '-', date), '%Y-%m-%d');";
						
			ResultSet orgFertList = myConnection.query(s3);

			String s4 = "SELECT  xn5_cell_x, xn5_cell_y, year, position, " 
						+ " t1.crop_management_id, t1.crop_code, t1.variety, "
						+  " event_id, date, code, implement as fertilizer, amount, IFNULL(rate, -99) as rate, IFNULL(nh4n, -99) as nh4n, IFNULL(no3n, -99) as no3n , IFNULL(org_n, -99) as org_n, IFNULL(org_c, -99) as org_c, date_year "
						+  " FROM simulation_projects_xn5_cells_management t1 " 
						+	"	JOIN crop_management_irrigation t2 "
						+  " ON t1.crop_management_id = t2.crop_management_id "
						+  "  AND t1.crop_code = t2.crop_code "
						+  "  AND t1.variety = t2.variety "
						+  "  WHERE simulation_project_id = " +  projectID +"" +
								" ORDER BY xn5_cell_x, xn5_cell_y, year, position, STR_TO_DATE(CONCAT(year + date_year, '-', date), '%Y-%m-%d');";
			
			ResultSet irrigationList = myConnection.query(s4);

	
			String s5 = " SELECT xn5_cell_x, xn5_cell_y, year, position, " 
					    + " t1.crop_management_id, t1.crop_code, t1.variety, "
					    +  " event_id, date, depth, equipment as implement, code, date_year" 
					    + "	FROM simulation_projects_xn5_cells_management t1 "
					    + " JOIN crop_management_tillage t2 "
					    + " ON t1.crop_management_id = t2.crop_management_id "
					    +  " AND t1.crop_code = t2.crop_code "
					    +  " AND t1.variety = t2.variety "
					    +  " WHERE simulation_project_id = " +  projectID +"" +
						" ORDER BY xn5_cell_x, xn5_cell_y, year, position, STR_TO_DATE(CONCAT(year + date_year, '-', date), '%Y-%m-%d');";
			

			ResultSet tillageList = myConnection.query(s5);

			if(!managementList.first() )
			{
				throw new Exception("Error: no management for simulation project!");
			}
			
			boolean noTillage = ! tillageList.first();
			boolean noIrrigation = ! irrigationList.first();
			boolean noOrgFert = ! orgFertList.first();
			boolean noMinFert = ! minFertList.first();
			
			
			
			int curGrid = 0;
			int rn = 0;
			
			while (cellList.next())//move cursor to next value (including the first value in the first loop)
			{
				int curX = cellList.getInt("xn5_cell_x");
				int curY = cellList.getInt("xn5_cell_y");
				++rn;
				System.out.println("Writing plant management for " + curX + ","+curY);

				
				String xn5FileNameCropRotation = xn5FilePrefix + "_" + curGrid + "_" + curX + "_" + curY + "_0_crop_rotation.ini" ;
			
				PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(xn5FileNameCropRotation)));
				
				out.println("[plant_management]");					

				List<String> fieldList = Arrays.asList(		"CropCode","CropName","variety"
						 , "sow_depth", "row_dist", "sow_dens", "sow_date", "emerg_date", "harvest_date",
						  "max_biom_date", "max_ro_date", "max_root", "biom_remove", "max_plant_height"
						); /*Note: further code assumes that sow_date comes before emerg_date, if you change that, adapt later code*/

				printManagementsForCellToIniFile(fieldList, out, managementList, curX, curY);
				out.println("");
				
				
				
				out.close();


				String xn5FileNameFertilization = xn5FilePrefix + "_" + curGrid + "_" + curX + "_" + curY + "_0_fertilization.ini" ;
				
				PrintWriter outFert = new PrintWriter(new BufferedWriter(new FileWriter(xn5FileNameFertilization)));
				outFert.println("[mineral]");
				if(!noMinFert) {
					fieldList = Arrays.asList(		"date", "fertilizer", "code", "n_tot_min", "no3n", "nh4n", "urea");
					printManagementsForCellToIniFile(fieldList, outFert, minFertList, curX, curY);
				}
				outFert.println("");
				
				outFert.println("[organic]");
				if(!noOrgFert) {
					fieldList = Arrays.asList(		"date", "fertilizer", "code", "amount", "dry_matter", "nh4n", "org_subst", "n_tot_org");
					printManagementsForCellToIniFile(fieldList, outFert, orgFertList, curX, curY);
				}
				outFert.println("");
				


				outFert.close();
				
				String xn5FileNameTillage = xn5FilePrefix + "_" + curGrid + "_" + curX + "_" + curY + "_0_schaaf_manag.ini" ;
				
				PrintWriter outTillage = new PrintWriter(new BufferedWriter(new FileWriter(xn5FileNameTillage)));
				outTillage.println("[tillage]");					

				if (! noTillage) {
					fieldList = Arrays.asList(		"date", "implement", "code", "depth");
					printManagementsForCellToIniFile(fieldList, outTillage, tillageList, curX, curY);
				}
				outTillage.println("");		

				outTillage.println("[irrigation]");				
				if(!noIrrigation) {
					fieldList = Arrays.asList(		"date", "fertilizer", "code", "amount", "rate", "no3n", "nh4n", "org_c", "org_n");
					printManagementsForCellToIniFile(fieldList, outTillage, irrigationList, curX, curY);
					
				}
				outTillage.println("");
				outTillage.close();

			}
			cellList.close();
			managementList.close();
			irrigationList.close();
			minFertList.close();
			orgFertList.close();
			tillageList.close();
			
			
			ok = true;
		} 
		catch (Exception e) 
		{
			  e.printStackTrace();
			  //System.exit(0);
		}
		return ok;

	}
	
	public boolean writeXn5CropManagementFileDummies(String xn5FilePrefix, int projectID, String xn5_cells_table_name)
	{
		boolean ok = false;
		try {
			
			String s7 = "SELECT  startYear, startMonth, startDay"
					+ " FROM simulation_projects_general t1"
					
					+ "   WHERE simulation_project_id = "+ projectID + ";"
					
			;
	
			ResultSet generalInfo = myConnection.query(s7);
			generalInfo.first();
			
			
			String s6 = "SELECT  xn5_cell_x, xn5_cell_y "
					+ " FROM `"+xn5_cells_table_name+"` t1"
					
					+ "   WHERE simulation_project_id = "+ projectID +
					
					" GROUP BY xn5_cell_x, xn5_cell_y"
					+ " ORDER BY xn5_cell_x, xn5_cell_y";
	
			ResultSet cellList = myConnection.query(s6);
			
				
			
			
			int curGrid = 0;
			int rn = 0;
			
			while (cellList.next())//move cursor to next value (including the first value in the first loop)
			{
				int curX = cellList.getInt("xn5_cell_x");
				int curY = cellList.getInt("xn5_cell_y");
				++rn;
				System.out.println("Writing plant management file dummy for " + curX + ","+curY);

				
				String xn5FileNameCropRotation = xn5FilePrefix + "_" + curGrid + "_" + curX + "_" + curY + "_0_crop_rotation.ini" ;
			
				PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(xn5FileNameCropRotation)));
				
				out.println("[plant_management]");					

/*				List<String> fieldList = Arrays.asList(		"CropCode","CropName","variety"
						 , "sow_depth", "row_dist", "sow_dens", "sow_date", "emerg_date", "harvest_date",
						  "max_biom_date", "max_ro_date", "max_root", "biom_remove", "max_plant_height"
						);

				
*/				
				out.println("CropCode = WH");
				out.println("CropName = Wheat");
				out.println("variety = Winterweizen");
				out.println("sow_depth = 4");
				out.println("row_dist = 12");
				out.println("sow_dens = 400");
				out.println("sow_date =  "+ generalInfo.getInt("startYear")+ "-10-01");
				out.println("emerg_date = "+ generalInfo.getInt("startYear")+ "-10-01");
				out.println("harvest_date = "+ generalInfo.getInt("startYear")+ "-10-10");
				out.println("max_biom_date ="+ generalInfo.getInt("startYear")+ "-10-10");
				out.println("max_ro_date = "+ generalInfo.getInt("startYear")+ "-10-10");
				out.println("max_root = 2");
				out.println("biom_remove = 1");
				out.println("max_plant_height = 150");
				
				
				
				out.println("");
				
				
				
				out.close();


				String xn5FileNameFertilization = xn5FilePrefix + "_" + curGrid + "_" + curX + "_" + curY + "_0_fertilization.ini" ;
				
				PrintWriter outFert = new PrintWriter(new BufferedWriter(new FileWriter(xn5FileNameFertilization)));
				outFert.println("[mineral]");
				outFert.println("");
				
				outFert.println("[organic]");
				outFert.println("");
				outFert.close();
				
				String xn5FileNameTillage = xn5FilePrefix + "_" + curGrid + "_" + curX + "_" + curY + "_0_schaaf_manag.ini" ;
				
				PrintWriter outTillage = new PrintWriter(new BufferedWriter(new FileWriter(xn5FileNameTillage)));
				outTillage.println("[tillage]");					
				outTillage.println("");		

				outTillage.println("[irrigation]");				
				outTillage.println("");
				outTillage.close();
			}
			generalInfo.close();
			cellList.close();
			ok = true;
		} 
		catch (Exception e) 
		{
			  e.printStackTrace();
			  //System.exit(0);
		}
		return ok;

	}
	
	public boolean writeXn5CellCfgFiles(String xn5FilePrefix, int projectID, String xn5_cells_table_name)
	{
		boolean ok = false;
		try {
			
			String s0 = "SELECT startYear, endYear, startMonth, startDay, endMonth, endDay, plotSize , kc_param_id" 
						+ " FROM simulation_projects_general "
						+ " WHERE simulation_project_id = "+ projectID + ";";
			ResultSet projectInfo = myConnection.query(s0);	

			if (! projectInfo.first()) {
				throw new Exception("Error: Missing general information on simulation project.");
			}
			
			int startYear = projectInfo.getInt("startYear");
			int startMonth = projectInfo.getInt("startMonth");
			int startDay = projectInfo.getInt("startDay");
			int endYear = projectInfo.getInt("endYear");
			int endMonth = projectInfo.getInt("endMonth");
			int endDay = projectInfo.getInt("endDay");
			
			double plotSize = projectInfo.getDouble("plotSize");
			
			
			int batchSize = 10000;
			Boolean everything_retrieved = false;
			int repcount = 0;
			
			while (! everything_retrieved)  { //RETRIEVING BATCH BY BATCH TO AVOID MEMORY PROBLEMS FOR LARGE MAPS
				
				
				String s1 = "SELECT xn5_cell_x, xn5_cell_y, t1.profileID, "
							+ "lat, lon, alt, exposition, inclination, AveYearTemp, MonthTempAmp" 
								+ ", SUM(layers) as layer_count, MAX(depth_per_layer) as layer_thickness "
								+" FROM `"+xn5_cells_table_name+"` t1"
								+ " JOIN soil_parameterization t2 " 
								+ " ON t1.profileID = t2.profileID "
	                            + " AND t1.soil_param_id = t2.soil_param_id "
									+" WHERE simulation_project_id = " + projectID
									+" GROUP BY xn5_cell_x, xn5_cell_y "
									+" ORDER BY xn5_cell_x, xn5_cell_y"
									+ " LIMIT " + (repcount * batchSize) + ",  " + batchSize+";"
									;
				
				
				ResultSet cellList = myConnection.query(s1);	
				
				if (repcount == 0 && !cellList.isBeforeFirst() ) //Note: if directly after retrieval isBeforeFirst returns false, the cell list is empty
				{
					throw new Exception("No cells defined for project!");
				}
				else if (repcount > 0 && !cellList.isBeforeFirst() )
				{
					everything_retrieved = true;
					break;
				}
				
				cellList.first();
				int batchMinX = cellList.getInt("xn5_cell_x");
				int batchMinY = cellList.getInt("xn5_cell_y");
				
				cellList.last();
				int batchMaxX = cellList.getInt("xn5_cell_x");
				int batchMaxY = cellList.getInt("xn5_cell_y");				
				cellList.beforeFirst();
				
				
				
				String s2 = "SELECT xn5_cell_x, xn5_cell_y, t2.`horizon_id`,    `t2`.`horizon_name`,    `t2`.`depth_per_layer`,"
							+ " `t2`.`first_layer`,    `t2`.`layers`,    `t2`.`clay`,    `t2`.`silt`,    `t2`.`sand`,    `t2`.`organic_matter`,"
							+ " `t2`.`bulk_density`,    `t2`.`rock_fraction`,    `t2`.`ph`,    `t2`.`soil_type`,    `t2`.`wilting_point`,"
							+ " `t2`.`field_capacity`,    `t2`.`porosity`,    `t2`.`cond_sat`,    `t2`.`res_water_cont`,    `t2`.`cont_sat`,"
							+ " IFNULL(`t2`.`camp_a`,-99) as camp_a,    IFNULL(`t2`.`camp_b`,-99) as camp_b,    `t2`.`van_gen_a`,    `t2`.`van_gen_n`,    `t2`.`van_gen_m`" 
							+ "	 FROM `"+xn5_cells_table_name+"` t1	 "
							+ " JOIN soil_parameterization t2	ON t1.profileID = t2.profileID	AND t1.soil_param_id = t2.soil_param_id "
							+ "	WHERE simulation_project_id =   " + projectID
							+ " AND (  (xn5_cell_x = "+batchMinX+" AND  xn5_cell_y >= "+batchMinY +")  OR (xn5_cell_x > "+batchMinX+"  )) "
							+ " AND (  (xn5_cell_x = "+batchMaxX+" AND  xn5_cell_y <= "+batchMaxY +")  OR (xn5_cell_x < "+batchMaxX+"  ))" 
							+ " ORDER BY xn5_cell_x, xn5_cell_y, first_layer;";
				
				ResultSet soilParam = myConnection.query(s2);	
	
				String s5 = "SELECT xn5_cell_x, xn5_cell_y, " +
						" effic, humf, min_cn, temp0, miner_q10, theta0, theta1, MinerSatActiv, NitrifNO3NH4Ratio," +
						" iTempCorr, AOM1Q10 as CAOM1Q10, AOM2Q10 as CAOM2Q10, BOM1Q10 as CBOM1Q10, BOM2Q10 as CBOM2Q10, SOM1Q10 as CSOM1Q10,"
						+ "SOM2Q10 as CSOM2Q10, DBOM1Q10, DBOM2Q10, MBOM1Q10, MBOM2Q10,	"
						+ "fBOM1, fSOM1, fSOM2, fEFF, fEFFAOM1 as fEff_AOM1, fEFFAOM2 as fEff_AOM2,fEFFBOM1 as fEff_BOM1, fEFFBOM2 as fEff_BOM2, fEFFSOM1 as fEff_SOM1,	fEFFSOM2 as fEff_SOM2 "
						+ "	 FROM `"+xn5_cells_table_name+"` t1	 "
						+ " JOIN daisy_parameterization t2	ON t1.profileID = t2.profileID	" +
						"		AND t1.daisy_param_id = t2.daisy_param_id "
						+ "	WHERE simulation_project_id =   " + projectID
						+ " AND (  (xn5_cell_x = "+batchMinX+" AND  xn5_cell_y >= "+batchMinY +")  OR (xn5_cell_x > "+batchMinX+"  )) "
						+ " AND (  (xn5_cell_x = "+batchMaxX+" AND  xn5_cell_y <= "+batchMaxY +")  OR (xn5_cell_x < "+batchMaxX+"  ))" 
						+ " ORDER BY xn5_cell_x, xn5_cell_y;";
			
				ResultSet daisyParam = myConnection.query(s5);	
				
				String s6 = "SELECT xn5_cell_x, xn5_cell_y, t1.profileID, first_layer, "    
						+ " layers, fom_slow, fom_fast, fom_veryfast, micbioms_slow, micbioms_fast, humus_slow, humus_fast"
						+ " FROM `"+xn5_cells_table_name+"` t1"
	                    + "     JOIN daisy_parameterization_layers t3"
	                    + "     	ON t1.profileID = t3.profileID"
	                    + "         AND t1.daisy_param_id = t3.daisy_param_id"
						+ "	  	 WHERE simulation_project_id = " + projectID
						+ " AND (  (xn5_cell_x = "+batchMinX+" AND  xn5_cell_y >= "+batchMinY +")  OR (xn5_cell_x > "+batchMinX+"  )) "
						+ " AND (  (xn5_cell_x = "+batchMaxX+" AND  xn5_cell_y <= "+batchMaxY +")  OR (xn5_cell_x < "+batchMaxX+"  ))" 
							+ "	 ORDER BY xn5_cell_x, xn5_cell_y, first_layer;";
	
				ResultSet daisyParamLayers = myConnection.query(s6);
			
				String s3 = "SELECT xn5_cell_x, xn5_cell_y, t1.profileID, first_layer, "    
							+ " layers, AOM1_C, AOM2_C, AOM3_C,  BOM1_C,"
							+ " BOM2_C, BOMD_C, SOM0_C, SOM1_C, SOM2_C, Csol,"
							+ " AOM1_N, AOM2_N, AOM3_N, BOM1_N, BOM2_N, BOMD_N,SOM0_N, SOM1_N, SOM2_N"
							+ " FROM `"+xn5_cells_table_name+"` t1"
	                        + "     JOIN sompools_initialization t3"
	                        + "     	ON t1.profileID = t3.profileID"
	                        + "         AND t1.sompools_param_id = t3.sompools_param_id"
							+ "	  	 WHERE simulation_project_id = " + projectID
							+ " AND (  (xn5_cell_x = "+batchMinX+" AND  xn5_cell_y >= "+batchMinY +")  OR (xn5_cell_x > "+batchMinX+"  )) "
							+ " AND (  (xn5_cell_x = "+batchMaxX+" AND  xn5_cell_y <= "+batchMaxY +")  OR (xn5_cell_x < "+batchMaxX+"  ))" 
								+ "	 ORDER BY xn5_cell_x, xn5_cell_y, first_layer;";
				
				ResultSet daisyStart = myConnection.query(s3);
				
				
				String s4 = "SELECT xn5_cell_x, xn5_cell_y, t1.profileID, t2.first_layer,"    
						+ " t2.layers, "
						+ " IF(water_content >= porosity, 0.97* porosity, water_content) AS water_content, matrix_potential, soil_temperature, nh4_content, no3_content, root_density"
						+ " FROM `"+xn5_cells_table_name+"` t1"
	                    + "     JOIN soil_initialization t2"
	                    + "     	ON t1.profileID = t2.profileID"
	                    + "         AND t1.soilinit_param_id = t2.soilinit_param_id"
	                    + "     JOIN soil_parameterization t3 "
	                    +  " 		ON t1.profileID = t3.profileID	AND t1.soil_param_id = t3.soil_param_id "
	                    +  "			AND t2.first_layer = t3.first_layer"			
						+ "	  	 WHERE simulation_project_id = " + projectID
						+ " AND (  (xn5_cell_x = "+batchMinX+" AND  xn5_cell_y >= "+batchMinY +")  OR (xn5_cell_x > "+batchMinX+"  )) "
						+ " AND (  (xn5_cell_x = "+batchMaxX+" AND  xn5_cell_y <= "+batchMaxY +")  OR (xn5_cell_x < "+batchMaxX+"  ))" 
							+ "	 ORDER BY xn5_cell_x, xn5_cell_y, first_layer;";
			
				ResultSet soilStart = myConnection.query(s4);
				
				String s7 = "SELECT xn5_cell_x, xn5_cell_y, t1.profileID, first_layer,"    
						+ " layers, c_litter, n_litter, c_manure, n_manure, c_humus, n_humus "					
						+ " FROM `"+xn5_cells_table_name+"` t1"
	                    + "     JOIN soil_initialization t3"
	                    + "     	ON t1.profileID = t3.profileID"
	                    + "         AND t1.soilinit_param_id = t3.soilinit_param_id"
						+ "	  	 WHERE simulation_project_id = " + projectID
						+ " AND (  (xn5_cell_x = "+batchMinX+" AND  xn5_cell_y >= "+batchMinY +")  OR (xn5_cell_x > "+batchMinX+"  )) "
						+ " AND (  (xn5_cell_x = "+batchMaxX+" AND  xn5_cell_y <= "+batchMaxY +")  OR (xn5_cell_x < "+batchMaxX+"  ))" 
							+ "	 ORDER BY xn5_cell_x, xn5_cell_y, first_layer;";
			
				//ResultSet soilStart2 = myConnection.query(s7);
				ResultSet soilStart2_for_leachn = myConnection.query(s7);

				
				String s8 = "SELECT xn5_cell_x, xn5_cell_y, " +
						" t2.effic, t2.humf, t2.min_cn, t2.temp0, t2.miner_q10, t2.theta0, t2.MinerSatActiv, t2.NitrifNO3NH4Ratio," +
						" t3. `leachn_param_id`, t3.`fn2Fraction`, t3.`fn2oeduction` as fn2oReduction , t3.`irewet`, t3.`ino3kin` "
						+ "	 FROM `"+xn5_cells_table_name+"` t1	 "
						+ " JOIN daisy_parameterization t2	ON t1.profileID = t2.profileID	" +
						"		AND t1.daisy_param_id = t2.daisy_param_id "
						+ " JOIN leachn_parameterization t3	ON t1.leachn_param_id = t3.leachn_param_id	" 
						+ "	WHERE simulation_project_id =   " + projectID
						+ " AND (  (xn5_cell_x = "+batchMinX+" AND  xn5_cell_y >= "+batchMinY +")  OR (xn5_cell_x > "+batchMinX+"  )) "
						+ " AND (  (xn5_cell_x = "+batchMaxX+" AND  xn5_cell_y <= "+batchMaxY +")  OR (xn5_cell_x < "+batchMaxX+"  ))" 
						+ " ORDER BY xn5_cell_x, xn5_cell_y;";
			
				ResultSet leachnParam = myConnection.query(s8);	
				
				String s9 = "SELECT xn5_cell_x, xn5_cell_y, t3.leachn_param_id, t3.first_layer, " +
						"		GREATEST(0,LEAST(layers,layers_cell -cum_layers_leachn)) as layers," +
						" urea_hydrolysis , nitrification, denitrification, mineralisation_lit , mineralisation_man,  mineralisation_hum" +
						"					FROM `"+xn5_cells_table_name+"` t1" +
						"                         JOIN leachn_parameterization_layers t3" +
						"                         	ON t1.leachn_param_id = t3.leachn_param_id" +
						"						 JOIN (SELECT profileID, soil_param_id, SUM(layers) as layers_cell FROM" +
						"								soil_parameterization i2" +
						"									GROUP BY profileID, soil_param_id" +
						"                                     ) i2 ON t1.profileID = i2.profileID" +
						"									 AND t1.soil_param_id = i2.soil_param_id" +
						"                        JOIN  (SELECT i3.leachn_param_id, i3.first_layer, IFNULL(SUM(i4.layers),0) as cum_layers_leachn" +
						"									FROM leachn_parameterization_layers i3" +
						"										LEFT JOIN leachn_parameterization_layers i4" +
						"											ON i3.leachn_param_id = i4.leachn_param_id" +
						"											AND i3.first_layer > i4.first_layer" +
						"									GROUP BY i3.leachn_param_id, i3.first_layer" +
						"                                            ) u2 ON t3.leachn_param_id	= u2.leachn_param_id" +
						"											 AND t3.first_layer = u2.first_layer" +
						"						  	 WHERE simulation_project_id = "+ projectID +
						"								AND GREATEST(0,LEAST(layers,layers_cell -cum_layers_leachn)) > 0" 
						+ " AND (  (xn5_cell_x = "+batchMinX+" AND  xn5_cell_y >= "+batchMinY +")  OR (xn5_cell_x > "+batchMinX+"  )) "
						+ " AND (  (xn5_cell_x = "+batchMaxX+" AND  xn5_cell_y <= "+batchMaxY +")  OR (xn5_cell_x < "+batchMaxX+"  ))" 
						+"							 ORDER BY xn5_cell_x, xn5_cell_y, first_layer;";
	
				ResultSet leachnParamLayersTransf = myConnection.query(s9);
				
				
				String s10 = 		"SELECT xn5_cell_x, xn5_cell_y, t3.leachn_param_id, t3.first_layer, " +
						"		GREATEST(0,LEAST(layers,layers_cell -cum_layers_leachn)) as layers," +
						" nh4_to_n20" +
						"					FROM `"+xn5_cells_table_name+"` t1" +
						"                         JOIN leachn_parameterization_layers t3" +
						"                         	ON t1.leachn_param_id = t3.leachn_param_id" +
						"						 JOIN (SELECT profileID, soil_param_id, SUM(layers) as layers_cell FROM" +
						"								soil_parameterization i2" +
						"									GROUP BY profileID, soil_param_id" +
						"                                     ) i2 ON t1.profileID = i2.profileID" +
						"									 AND t1.soil_param_id = i2.soil_param_id" +
						"                        JOIN  (SELECT i3.leachn_param_id, i3.first_layer, IFNULL(SUM(i4.layers),0) as cum_layers_leachn" +
						"									FROM leachn_parameterization_layers i3" +
						"										LEFT JOIN leachn_parameterization_layers i4" +
						"											ON i3.leachn_param_id = i4.leachn_param_id" +
						"											AND i3.first_layer > i4.first_layer" +
						"									GROUP BY i3.leachn_param_id, i3.first_layer" +
						"                                            ) u2 ON t3.leachn_param_id	= u2.leachn_param_id" +
						"											 AND t3.first_layer = u2.first_layer" +
						"						  	 WHERE simulation_project_id = "+ projectID +
						"								AND GREATEST(0,LEAST(layers,layers_cell -cum_layers_leachn)) > 0" 
													+ " AND (  (xn5_cell_x = "+batchMinX+" AND  xn5_cell_y >= "+batchMinY +")  OR (xn5_cell_x > "+batchMinX+"  )) "
							+ " AND (  (xn5_cell_x = "+batchMaxX+" AND  xn5_cell_y <= "+batchMaxY +")  OR (xn5_cell_x < "+batchMaxX+"  ))" +
						"							 ORDER BY xn5_cell_x, xn5_cell_y, first_layer;";
	;
	
				ResultSet leachnParamLayersNitrif = myConnection.query(s10);
				
				String s11 = 	"SELECT xn5_cell_x, xn5_cell_y, t3.leachn_param_id, t3.first_layer, " +
						"		GREATEST(0,LEAST(layers,layers_cell -cum_layers_leachn)) as layers," +
						"			ksno3, ksc, theta0, biogrowth, bio_mc" +
						"					FROM `"+xn5_cells_table_name+"` t1" +
						"                         JOIN leachn_parameterization_layers t3" +
						"                         	ON t1.leachn_param_id = t3.leachn_param_id" +
						"						 JOIN (SELECT profileID, soil_param_id, SUM(layers) as layers_cell FROM" +
						"								soil_parameterization i2" +
						"									GROUP BY profileID, soil_param_id" +
						"                                     ) i2 ON t1.profileID = i2.profileID" +
						"									 AND t1.soil_param_id = i2.soil_param_id" +
						"                        JOIN  (SELECT i3.leachn_param_id, i3.first_layer, IFNULL(SUM(i4.layers),0) as cum_layers_leachn" +
						"									FROM leachn_parameterization_layers i3" +
						"										LEFT JOIN leachn_parameterization_layers i4" +
						"											ON i3.leachn_param_id = i4.leachn_param_id" +
						"											AND i3.first_layer > i4.first_layer" +
						"									GROUP BY i3.leachn_param_id, i3.first_layer" +
						"                                            ) u2 ON t3.leachn_param_id	= u2.leachn_param_id" +
						"											 AND t3.first_layer = u2.first_layer" +
						"						  	 WHERE simulation_project_id = "+ projectID +
						"								AND GREATEST(0,LEAST(layers,layers_cell -cum_layers_leachn)) > 0" 
													+ " AND (  (xn5_cell_x = "+batchMinX+" AND  xn5_cell_y >= "+batchMinY +")  OR (xn5_cell_x > "+batchMinX+"  )) "
							+ " AND (  (xn5_cell_x = "+batchMaxX+" AND  xn5_cell_y <= "+batchMaxY +")  OR (xn5_cell_x < "+batchMaxX+"  ))" +
						"							 ORDER BY xn5_cell_x, xn5_cell_y, first_layer;";
	
	
				ResultSet leachnParamLayersDenitrif = myConnection.query(s11);
				
				String s12 = "SELECT xn5_cell_x, xn5_cell_y, t3.leachn_param_id, t3.first_layer, " +
						"		GREATEST(0,LEAST(layers,layers_cell -cum_layers_leachn)) as layers," +
						"n2o_n2, freezing, thawing, rewet" + 
						"					FROM `"+xn5_cells_table_name+"` t1" +
						"                         JOIN leachn_parameterization_layers t3" +
						"                         	ON t1.leachn_param_id = t3.leachn_param_id" +
						"						 JOIN (SELECT profileID, soil_param_id, SUM(layers) as layers_cell FROM" +
						"								soil_parameterization i2" +
						"									GROUP BY profileID, soil_param_id" +
						"                                     ) i2 ON t1.profileID = i2.profileID" +
						"									 AND t1.soil_param_id = i2.soil_param_id" +
						"                        JOIN  (SELECT i3.leachn_param_id, i3.first_layer, IFNULL(SUM(i4.layers),0) as cum_layers_leachn" +
						"									FROM leachn_parameterization_layers i3" +
						"										LEFT JOIN leachn_parameterization_layers i4" +
						"											ON i3.leachn_param_id = i4.leachn_param_id" +
						"											AND i3.first_layer > i4.first_layer" +
						"									GROUP BY i3.leachn_param_id, i3.first_layer" +
						"                                            ) u2 ON t3.leachn_param_id	= u2.leachn_param_id" +
						"											 AND t3.first_layer = u2.first_layer" +
						"						  	 WHERE simulation_project_id = "+ projectID +
						"								AND GREATEST(0,LEAST(layers,layers_cell -cum_layers_leachn)) > 0" 
						+ " AND (  (xn5_cell_x = "+batchMinX+" AND  xn5_cell_y >= "+batchMinY +")  OR (xn5_cell_x > "+batchMinX+"  )) "
						+ " AND (  (xn5_cell_x = "+batchMaxX+" AND  xn5_cell_y <= "+batchMaxY +")  OR (xn5_cell_x < "+batchMaxX+"  ))"+ 
						"							 ORDER BY xn5_cell_x, xn5_cell_y, first_layer;";
	
	
				ResultSet leachnParamLayersDenitrifEvents = myConnection.query(s12);
		
				String s13 = "SELECT xn5_cell_x, xn5_cell_y, t1.profileID, " +
						"			IFNULL(fCLitterSurf, -99) AS fCLitterSurf, IFNULL(fNLitterSurf,-99) AS  fNLitterSurf, " +
						"			IFNULL(fCManureSurf, -99) AS fCManureSurf ,IFNULL(fNManureSurf,-99) AS  fNManureSurf, " +
						"			IFNULL(fCHumusSurf, -99) AS fCHumusSurf, IFNULL(fNHumusSurf,-99) AS fNHumusSurf" 	
						+ " FROM `"+xn5_cells_table_name+"` t1"
	                    + "    LEFT JOIN soil_initialization_surface t3"
	                    + "     	ON t1.profileID = t3.profileID"
	                    + "         AND t1.soilinit_param_id = t3.soilinit_param_id"
						+ "	  	 WHERE simulation_project_id = " + projectID
						+ " AND (  (xn5_cell_x = "+batchMinX+" AND  xn5_cell_y >= "+batchMinY +")  OR (xn5_cell_x > "+batchMinX+"  )) "
						+ " AND (  (xn5_cell_x = "+batchMaxX+" AND  xn5_cell_y <= "+batchMaxY +")  OR (xn5_cell_x < "+batchMaxX+"  ))" 
							+ "	 ORDER BY xn5_cell_x, xn5_cell_y;";
			
				
				ResultSet soilStart3_for_leachn = myConnection.query(s13);
				
				/*if (cellList)
				{
					throw new Exception("Error in writeXn5CellCfgFiles: Empty list of cells. Query: " + s1 );
				}*/
				
				
				
				int curGrid = 0;
				int rn = 0;
				
				if (! soilParam.first() ) {
					throw new Exception("Error: Missing soil parameterization for simulation project.");
				}
				if (! daisyStart.first() ) {
					throw new Exception("Error: Missing SOM pools initialization for simulation project.");
				}
				if (! soilStart.first() ) {
					throw new Exception("Error: Missing soil initialization for simulation project.");
				}
				//if (! soilStart2.first() ) {
				//	throw new Exception("Error: Missing soil initialization for simulation project.");
				//}
				if (! soilStart2_for_leachn.first() ) {
					throw new Exception("Error: Missing soil initialization for simulation project.");
				}
				if (! soilStart3_for_leachn.first() ) {
					throw new Exception("Error: Missing soil surface initialization for simulation project.");
				}
				if (! daisyParam.first() ) {
					throw new Exception("Error: Missing DAISY parameterization for simulation project.");
				}
				if (! daisyParamLayers.first() ) {
					throw new Exception("Error: Missing DAISY layered parameterization for simulation project.");
				}
				if (! leachnParam.first() ) {
					throw new Exception("Error: Missing LEACHN parameterization for simulation project.");
				}
				if (! leachnParamLayersTransf.first() ) {
					throw new Exception("Error: Missing LEACHN layered parameterization for simulation project.");
				}
				if (! leachnParamLayersNitrif.first() ) {
					throw new Exception("Error: Missing LEACHN layered parameterization for simulation project.");
				}
				if (! leachnParamLayersDenitrif.first() ) {
					throw new Exception("Error: Missing LEACHN layered parameterization for simulation project.");
				}
				if (! leachnParamLayersDenitrifEvents.first() ) {
					throw new Exception("Error: Missing LEACHN layered parameterization for simulation project.");
				}
				
				while (cellList.next())//move cursor to next value (including the first value in the first loop)
				{
					
					int curX = cellList.getInt("xn5_cell_x");
					int curY = cellList.getInt("xn5_cell_y");
					++rn;
					
					System.out.println("Writing cfg for cell " + curX+ ", " +curY+"...");
					
					String xn5FileNameCfg = xn5FilePrefix + "_" + curGrid + "_" + curX + "_" + curY + "_0_cfg.ini" ;
					String xn5FileNameDaisy = xn5FilePrefix + "_" + curGrid + "_" + curX + "_" + curY + "_0_daisy_miner_nitrogen.ini" ;
					String xn5FileNameLeachn = xn5FilePrefix + "_" + curGrid + "_" + curX + "_" + curY + "_0_leachn.ini" ;
	
					PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(xn5FileNameCfg)));
					
					
					out.println("[simulation]");					
					out.println("start="+startYear+"-"+ String.format("%02d", startMonth)+"-"+String.format("%02d", startDay));
					out.println("end="+endYear+"-"+String.format("%02d", endMonth)+"-"+String.format("%02d", endDay));
					out.println("");					
	
					out.println("[location]");					
					printNextRowInColToIniFile("lat", out, cellList);
					printNextRowInColToIniFile("lon", out, cellList);
					printNextRowInColToIniFile("alt", out, cellList);
					printNextRowInColToIniFile("exposition", out, cellList);
					printNextRowInColToIniFile("inclination", out, cellList);
					out.println("size = "+plotSize);
					out.println("plot_number = "+ rn);				
					out.println("plot_name = Cell " + curX + ", " + curY);				
					out.println("sub_plot_number = 1");				
					out.println("sub_plot_name = Subplot 1");				
					
					out.println("sub_plot_size = "+plotSize);
					out.println("area_number = 111");
					out.println("wind_measure_height=2.0");
					out.println("temp_measure_height=2.0");
					out.println("");					
	
	
					out.println("[climate]");	
					printNextRowInColToIniFile("AveYearTemp", out, cellList);
					printNextRowInColToIniFile("MonthTempAmp", out, cellList);
					out.println("");	
					
					out.println("[preceding_crop]");
					out.println("crop_name = -99");
					out.println("harvest_date = -99");
					out.println("yield = -99");
					out.println("res_removal = -99");
					out.println("above_residual = -99");
					out.println("root_residual = -99");
					out.println("above_CN_residual = -99");
					out.println("root_CN_residual = -99");
					out.println("");	
					
					out.println("[soil]");	
					printNextRowInColToIniFile("layer_count", out, cellList);
					int numLayers = cellList.getInt("layer_count");
					printNextRowInColToIniFile("layer_thickness", out, cellList);
					
					List<String> fieldList = Arrays.asList("layers", "clay", "silt", "sand", "organic_matter", "bulk_density", "rock_fraction","ph",
															"soil_type",  "wilting_point","field_capacity", "porosity","cond_sat","res_water_cont","cont_sat"
															,"camp_a","camp_b","van_gen_a","van_gen_n"		
															);
					
					printLayersForCellToIniFile(fieldList, out, soilParam, curX, curY);
					out.println("");
					out.println("[start values]");
	
					fieldList = Arrays.asList("layers",  "water_content", "matrix_potential", "soil_temperature", "nh4_content", "no3_content", "root_density"	
							);
					printLayersForCellToIniFile(fieldList, out, soilStart, curX, curY);
	
					/*N2O, N2*/
					out.println("");
	
					out.println("[farm]");
					out.println("name = BEMS model");
					out.println("number = 1");
					out.println("last_name = FOR1695");
					out.println("first_name = Universität Hohenheim");
					out.println("street = Wollgrasweg 43");
					out.println("post_code = 70599");
					out.println("city = Stuttgart");
					out.println("phone = 0711 459 23637");					
					out.println("fax = v1.0");
					out.println("mobile = 0711 459 23637");
					out.println("email = christian.troost@uni-hohenheim.de");
	
					out.close();
			//.daisy_miner file
					PrintWriter outDaisy = new PrintWriter(new BufferedWriter(new FileWriter(xn5FileNameDaisy)));
					
					outDaisy.println("[mineralisation]");
					//old 20001
					outDaisy.println("layers = " + numLayers);
					printNextRowInColToIniFile("effic", outDaisy, daisyParam);
					printNextRowInColToIniFile("humf", outDaisy, daisyParam);
					printNextRowInColToIniFile("min_cn", outDaisy, daisyParam);
					printNextRowInColToIniFile("temp0", outDaisy, daisyParam);
					printNextRowInColToIniFile("miner_q10", outDaisy, daisyParam);
					printNextRowInColToIniFile("theta0", outDaisy, daisyParam);
					printNextRowInColToIniFile("MinerSatActiv", outDaisy, daisyParam);
					printNextRowInColToIniFile("NitrifNO3NH4Ratio", outDaisy, daisyParam);
					outDaisy.println("");
	
					//old 20021
					//######## TEMPERATURE PARAMETERS OF DAISY DECOMPOSITION RATES (by Scott Demyan et al.)###################
					//iTempCorr- default: = 0
					//iTempCorr- exponential: 1 
					//iTempCorr- Arrhenius type: 2
					printNextRowInColToIniFile("iTempCorr", outDaisy, daisyParam);
					printNextRowInColToIniFile("CAOM1Q10", outDaisy, daisyParam);
					printNextRowInColToIniFile("CAOM2Q10", outDaisy, daisyParam);
					printNextRowInColToIniFile("CBOM1Q10", outDaisy, daisyParam);
					printNextRowInColToIniFile("CBOM2Q10", outDaisy, daisyParam);
					printNextRowInColToIniFile("CSOM1Q10", outDaisy, daisyParam);
					printNextRowInColToIniFile("CSOM2Q10", outDaisy, daisyParam);
					printNextRowInColToIniFile("DBOM1Q10", outDaisy, daisyParam);
					printNextRowInColToIniFile("DBOM2Q10", outDaisy, daisyParam);
					printNextRowInColToIniFile("MBOM1Q10", outDaisy, daisyParam);
					printNextRowInColToIniFile("MBOM2Q10", outDaisy, daisyParam);
					outDaisy.println("");
					//old 20022
					printNextRowInColToIniFile("fBOM1", outDaisy, daisyParam);
					printNextRowInColToIniFile("fSOM1", outDaisy, daisyParam);
					printNextRowInColToIniFile("fSOM2", outDaisy, daisyParam);
					printNextRowInColToIniFile("fEFF", outDaisy, daisyParam);
					outDaisy.println("");
					//old 20023
					printNextRowInColToIniFile("fEff_AOM1", outDaisy, daisyParam);
					printNextRowInColToIniFile("fEff_AOM2", outDaisy, daisyParam);
					printNextRowInColToIniFile("fEff_BOM1", outDaisy, daisyParam);
					printNextRowInColToIniFile("fEff_BOM2", outDaisy, daisyParam);
					printNextRowInColToIniFile("fEff_SOM1", outDaisy, daisyParam);
					printNextRowInColToIniFile("fEff_SOM2", outDaisy, daisyParam);
					outDaisy.println("");
					
					//old 20007
					outDaisy.println("[transformation daisy]");
					List<String> daisyFieldList = Arrays.asList("layers", "fom_slow", "fom_fast", "fom_veryfast", "micbioms_slow", "micbioms_fast", "humus_slow", "humus_fast"
							);
					printLayersForCellToIniFile(daisyFieldList, outDaisy, daisyParamLayers, curX, curY);
					outDaisy.println("");
					
					//old 20008/20009
					outDaisy.println("[start values daisy]");								
					daisyFieldList = Arrays.asList("layers", "AOM1_C", "AOM2_C", "AOM3_C",  "BOM1_C",
							"BOM2_C", "BOMD_C", "SOM0_C", "SOM1_C", "SOM2_C", "Csol",
									"AOM1_N", "AOM2_N", "AOM3_N", "BOM1_N", "BOM2_N", "BOMD_N","SOM0_N", 
									"SOM1_N", "SOM2_N"
							);
					printLayersForCellToIniFile(daisyFieldList, outDaisy, daisyStart, curX, curY);
	
					//old 20003
					/*outDaisy.println("[start values general]");
					daisyFieldList = Arrays.asList("layers", "c_litter", "n_litter", "c_manure", "n_manure", "c_humus", "n_humus"
							);
					printLayersForCellToIniFile(daisyFieldList, outDaisy, soilStart2, curX, curY);
					*/
					outDaisy.close();
					
					//leachn.ini
					PrintWriter outLeachn = new PrintWriter(new BufferedWriter(new FileWriter(xn5FileNameLeachn)));
					
					outLeachn.println("[mineralisation]");
					//old 20001
					outLeachn.println("layers = " + numLayers);
					outLeachn.println("");
					printNextRowInColToIniFile("effic", outLeachn, leachnParam);
					printNextRowInColToIniFile("humf", outLeachn, leachnParam);
					printNextRowInColToIniFile("min_cn", outLeachn, leachnParam);
					printNextRowInColToIniFile("temp0", outLeachn, leachnParam);
					printNextRowInColToIniFile("miner_q10", outLeachn, leachnParam);
					printNextRowInColToIniFile("theta0", outLeachn, leachnParam);
					printNextRowInColToIniFile("MinerSatActiv", outLeachn, leachnParam);
					printNextRowInColToIniFile("NitrifNO3NH4Ratio", outLeachn, leachnParam);
					outLeachn.println("");
					
					
					outLeachn.println("[transformation]");
					List<String> leachnFieldList = Arrays.asList("layers", 
							"urea_hydrolysis" , "nitrification", "denitrification",
							"mineralisation_lit" , "mineralisation_man",  "mineralisation_hum" 
							);
					printLayersForCellToIniFile(leachnFieldList, outLeachn, leachnParamLayersTransf, curX, curY);
					outLeachn.println("");
					
					outLeachn.println("[nitrification]");
					leachnFieldList = Arrays.asList("layers", "nh4_to_n20"
							);
					printLayersForCellToIniFile(leachnFieldList, outLeachn, leachnParamLayersNitrif, curX, curY);
					outLeachn.println("");
					
					outLeachn.println("[denitrification]");
					printNextRowInColToIniFile("fn2Fraction", outLeachn, leachnParam);
					printNextRowInColToIniFile("fn2oReduction", outLeachn, leachnParam);
					printNextRowInColToIniFile("irewet", outLeachn, leachnParam);
					printNextRowInColToIniFile("ino3kin", outLeachn, leachnParam);
					outLeachn.println("");
					
					leachnFieldList = Arrays.asList("layers", "ksno3", "ksc", "theta0", "biogrowth", "bio_mc"
							);
					printLayersForCellToIniFile(leachnFieldList, outLeachn, leachnParamLayersDenitrif, curX, curY);
					outLeachn.println("");
	
					outLeachn.println("[denitrification events]");
					leachnFieldList = Arrays.asList("layers", 
							"n2o_n2", "freezing", "thawing", "rewet" 
							);
					printLayersForCellToIniFile(leachnFieldList, outLeachn, leachnParamLayersDenitrifEvents, curX, curY);
					outLeachn.println("");
					
					
					outLeachn.println("[start values general]");
					leachnFieldList = Arrays.asList("layers", "c_litter", "n_litter", "c_manure", "n_manure", "c_humus", "n_humus"
							);
					printLayersForCellToIniFile(leachnFieldList, outLeachn, soilStart2_for_leachn, curX, curY);
					
					outLeachn.println("");

					outLeachn.println("[start values surface]");
					//leachnFieldList = Arrays.asList("fCLitterSurf", "fNLitterSurf", "fCManureSurf", "fNManureSurf", "fCHumusSurf", "fNHumusSurf"
					//		);
					//printLayersForCellToIniFile(leachnFieldList, outLeachn, soilStart3_for_leachn, curX, curY);
					printNextRowInColToIniFile("fCLitterSurf", outLeachn, soilStart3_for_leachn);
					printNextRowInColToIniFile("fNLitterSurf", outLeachn, soilStart3_for_leachn);
					printNextRowInColToIniFile("fCManureSurf", outLeachn, soilStart3_for_leachn);
					printNextRowInColToIniFile("fNManureSurf", outLeachn, soilStart3_for_leachn);
					printNextRowInColToIniFile("fCHumusSurf", outLeachn, soilStart3_for_leachn);
					printNextRowInColToIniFile("fNHumusSurf", outLeachn, soilStart3_for_leachn);
					
					
					outLeachn.close();
					
				} // end cellloop
				
				cellList.close();
				daisyParam.close();
				daisyParamLayers.close();
				daisyStart.close();
				leachnParam.close();
				leachnParamLayersDenitrif.close();
				leachnParamLayersDenitrifEvents.close();
				leachnParamLayersNitrif.close();
				leachnParamLayersTransf.close();
				soilStart.close();
//					soilStart2.close();
				++repcount;
				
			}//END batchloop
			projectInfo.close();
			ok = true;
		} 
		catch (Exception e) 
		{
			  e.printStackTrace();
			  //System.exit(0);
		}
		return ok;

	}
	void printSimpleIniField (String ColumnName, PrintWriter out, ResultSet rs) throws SQLException {
		String outstring = ColumnName + " = ";
		outstring += rs.getString(ColumnName) ;
		out.println(outstring);
	}
	void printSimpleIniFieldWithSemicolon (String ColumnName, PrintWriter out, ResultSet rs) throws SQLException {
		String outstring = ColumnName + " = ";
		outstring += rs.getString(ColumnName) ;
		out.println(outstring + ";");
	}
	void printSimpleIniField (String ColumnName, PrintWriter out, ResultSet rs, boolean addPlus) throws SQLException {
		String outstring = ColumnName + " = ";
		if (addPlus)
		{
			int val = rs.getInt(ColumnName);
			if (val > 0)
				outstring += "+" + rs.getString(ColumnName) ;
			else
				outstring += rs.getString(ColumnName) ;	
		}
		else {
			outstring += rs.getString(ColumnName) ;
		}
		out.println(outstring);
	}
	boolean printColumnToIniFile(String ColumnName, PrintWriter out, ResultSet rs)
	{
		return printColumnToIniFile(ColumnName, out, rs, false);
	}
	boolean printColumnToIniFile(String ColumnName, PrintWriter out, ResultSet rs, boolean addPlus)
	{
		boolean ok = false;
		try {
			String outstring = ColumnName + " = ";
			rs.beforeFirst();
			while (rs.next())
			{
				if (addPlus)
				{
					int val = rs.getInt(ColumnName);
					if (val > 0)
						outstring += "+" + rs.getString(ColumnName) + ";";
					else
						outstring += rs.getString(ColumnName) + ";";	
				}
				else
				{	
					outstring += rs.getString(ColumnName) + ";";
				}
			}
			out.println(outstring);
			ok = true;
		}
		catch (Exception e) 
		{
			  e.printStackTrace();
			//System.exit(0);
		}
		return ok;
	}
	boolean printNextRowInColToIniFile(String ColumnName, PrintWriter out, ResultSet rs)
	{
		return printNextRowInColToIniFile(ColumnName, out, rs, false);
	}
	boolean printNextRowInColToIniFile(String ColumnName, PrintWriter out, ResultSet rs, boolean addPlus)
	{
		boolean ok = false;
		try {
			String outstring = ColumnName + " = ";

			if (addPlus)
			{
				int val = rs.getInt(ColumnName);
				if (val > 0)
					outstring += "+" + rs.getString(ColumnName);
				else
					outstring += rs.getString(ColumnName) ;	
			}
			else
			{	
				outstring += rs.getString(ColumnName) ;
			}
			
			out.println(outstring);
			ok = true;
		}
		catch (Exception e) 
		{
			  e.printStackTrace();
			//System.exit(0);
		}
		return ok;
	}
	boolean printLayersForCellToIniFile(List<String> fieldList, PrintWriter out, ResultSet rs, int curX, int curY)
	{
		boolean ok = false;
		try {
			List<String> fieldContents = new ArrayList<String>();
			for (int i = 0; i < fieldList.size(); ++i)
			{
				fieldContents.add(fieldList.get(i) + " = " );
			}
			
			while(!rs.isAfterLast() && rs.getInt("xn5_cell_x") == curX && rs.getInt("xn5_cell_y") == curY ) { 
				for (int i = 0; i < fieldList.size(); ++i)
				{
					fieldContents.set(i, fieldContents.get(i)+" "+ rs.getString(fieldList.get(i)) + ";" );
				}
				if(!rs.next())
					break;
			}
			for (int i = 0; i < fieldList.size(); ++i)
			{
				out.println(fieldContents.get(i));
			}
	
			ok = true;
		}
		catch (Exception e) 
		{
			  e.printStackTrace();
			//System.exit(0);
		}
		return ok;
	}
	boolean printManagementsForCellToIniFile(List<String> fieldList, PrintWriter out, ResultSet rs, int curX, int curY)
	{
		boolean ok = false;
		try {
			List<String> fieldContents = new ArrayList<String>();
			for (int i = 0; i < fieldList.size(); ++i)
			{
				fieldContents.add(fieldList.get(i) + " = " );
			}
			int atleastone = 0;
			while(! rs.isAfterLast() &&  rs.getInt("xn5_cell_x") == curX && rs.getInt("xn5_cell_y") == curY ) { 
				for (int i = 0; i < fieldList.size(); ++i)
				{
					
					if ( fieldList.get(i).matches("emerg_date") )
					{
						if (rs.getString(fieldList.get(i)).equals("-99") || rs.getString(fieldList.get(i)).equals("-99.0") )
						{
							String sowdate= rs.getString("sow_date");
							int year= rs.getInt("sow_date_year") + rs.getInt("year");
							
							String[] sowdate_parts =  sowdate.split("-");
							int month = Integer.valueOf(sowdate_parts[0]) + 1;
							if (month > 12) {
								month = 1;
								year += 1;
							}
							fieldContents.set(i, fieldContents.get(i) + " " + year + "-" + String.format("%02d",month) + "-" + sowdate_parts[1]+ ";");
						}
						else {
							int year = rs.getInt(fieldList.get(i)+"_year") + rs.getInt("year") ;
							String temp = year +"-"+ rs.getString(fieldList.get(i));
							fieldContents.set(i, fieldContents.get(i) + " " + temp + ";" );
						}
					}
					else if ( fieldList.get(i).matches(".*date.*") )
					{
						if (rs.getString(fieldList.get(i)).equals("-99") || rs.getString(fieldList.get(i)).equals("-99.0") )
						{
							fieldContents.set(i, fieldContents.get(i) +" " + "-99" + ";" );
						}
						else {
							int year = rs.getInt(fieldList.get(i)+"_year") + rs.getInt("year") ;
							String temp = year +"-"+ rs.getString(fieldList.get(i));
							fieldContents.set(i, fieldContents.get(i) + " " + temp + ";" );
						}
					}
					else 
					{
						fieldContents.set(i,  fieldContents.get(i) + " " + rs.getString(fieldList.get(i)) + ";" );
					}
					atleastone++;
				}
				if(!rs.next())
					break;
			}
			if (atleastone > 0)
			{ for (int i = 0; i < fieldList.size(); ++i)
				{
					out.println(fieldContents.get(i));
				}
			}
			ok = true;
		}
		catch (Exception e) 
		{
			  e.printStackTrace();
			//System.exit(0);
		}
		return ok;
	}
	
	
	
	//SIMULATION PROJECTS
	public ResultSet getListOfSimulationProjects () {
		try {
			String s = "SELECT t1.simulation_project_id, simulation_project_code, simulation_project_description, t1.author_id, author_first_name, author_surname" +
					" , type_of_project, xn5_cells_table, bems_cells_management_table FROM info_simulation_projects t1 JOIN info_authors t2 on t1.author_id = t2.author_id " +
					"	JOIN simulation_projects_general t3 on t1.simulation_project_id = t3.simulation_project_id "
					+ " ORDER BY t1.simulation_project_id";
			ResultSet rs = myConnection.query(s);
			
			return rs;
		}
		catch (Exception e) 
		{
			  e.printStackTrace();
			  return null;
		}
	}
	public ResultSet getSimulationProjectInfo (int id) {
		try {
			String s = "SELECT t1.simulation_project_id, simulation_project_code, simulation_project_description, t1.author_id, author_first_name, author_surname, " +
					" type_of_project, xn5_cells_table, bems_cells_management_table FROM info_simulation_projects t1 JOIN info_authors t2 on t1.author_id = t2.author_id " +
					" JOIN simulation_projects_general t3 on t1.simulation_project_id = t3.simulation_project_id"
					+ " WHERE t1.simulation_project_id =" + id +";";
			ResultSet rs = myConnection.query(s);
			
			return rs;
		}
		catch (Exception e) 
		{
			  e.printStackTrace();
			  return null;
		}
	}
	public int addSimulationProject (String code, String description, String author, int type_of_project) {
		try {
			
			String s = "SELECT simulation_project_id " +
					" FROM info_simulation_projects t1 WHERE simulation_project_code = '" + code +"';";
			ResultSet rs = myConnection.query(s);			
			if(rs.first())
			{
				return -2; //code already exists
			}
			int id = 0;
			s ="SELECT max(simulation_project_id) as maxId " +
					" FROM info_simulation_projects t1;";
			rs = myConnection.query(s);
			
			if ( rs.first() )
			{
				id = rs.getInt("maxId") + 1;
			}
			
			if(!myConnection.updateDb("START TRANSACTION;")) {
				return -1;
			}
			
			s = "INSERT INTO info_simulation_projects VALUES ( " + id + ", '" + code 
					+ "', '" + description + "', '" + author + "', "+ type_of_project+");";
			if(! myConnection.updateDb(s) ) {
				myConnection.updateDb("ROLLBACK;");
				return -1;
			}

			s = "INSERT INTO simulation_projects_general VALUES ( " + id + ", 2009,2011,8,1,8,1,1,0,30,'simulation_projects_xn5_cells', 'simulation_projects_bems_cells_management',0,NULL,NULL,NULL, 1,1);";
			if(! myConnection.updateDb(s) ) {
				myConnection.updateDb("ROLLBACK;");
				return -1;
			}

			if(! myConnection.updateDb("COMMIT;") ) {
				myConnection.updateDb("ROLLBACK;");
				return -1;
			}
			
			return id;
		}
		catch (Exception e) 
		{
			  e.printStackTrace();
			  return -1;
		}
		
	}
	public boolean copyProjectSettings(int fromId, int toId) {
	
			String s = "REPLACE INTO simulation_projects_gecros_crops_included " +
					" SELECT " + toId + ", plant_param_id, crop_code, variety, include FROM simulation_projects_gecros_crops_included WHERE simulation_project_id = " + fromId + ";";
			if(!  myConnection.updateDb(s)) {
				return false;
			}
			s= "REPLACE INTO simulation_projects_general " +
			" SELECT " + toId + ", startYear, endYear, startMonth, startDay, endMonth, endDay, plotSize, adaptive, max_daily_precip " +
					", xn5_cells_table, bems_cells_management_table, elevationCorrectionType, elevationCorrectionClassSize, elevationInfoTableWeatherCells,  " +
					" co2_table, kc_param_id, numGrids " +
					" FROM simulation_projects_general WHERE simulation_project_id = " + fromId + ";";
			
			if(!  myConnection.updateDb(s)) {
				return false;
			}
			
			s= "SELECT xn5_cells_table, bems_cells_management_table FROM simulation_projects_general WHERE simulation_project_id = " + toId +";";
			String cellsTableName = "simulation_projects_xn5_cells";
			String managTableName = "simulation_projects_bems_cells_management";
			try {
				ResultSet temp = myConnection.query(s);
				if (!temp.first ()) {
					return false;
				}
				String cellsTableNameTest = temp.getString("xn5_cells_table");
				String managTableNameTest = temp.getString("bems_cells_management_table");
				
				if (! managTableNameTest.isEmpty() && checkTableExists("for1695_expertN", managTableNameTest) && checkTableIsNotView("for1695_expertN", managTableNameTest))	
					managTableName = managTableNameTest;
			
				if (! cellsTableNameTest.isEmpty() && checkTableExists("for1695_expertN", cellsTableNameTest) && checkTableIsNotView("for1695_expertN", cellsTableNameTest))	
					cellsTableName = cellsTableNameTest;
				
			}
			catch (Exception e) {
				return false;

			}
			
			
			
			s = "REPLACE INTO  " + cellsTableName +
					" SELECT " + toId + ", " + "`xn5_cell_x`, `xn5_cell_y`,"
					+"`profileID`, `soil_param_id`,`daisy_param_id`,`sompools_param_id`,"
					+"`soilinit_param_id`,`lat`,	`lon`,	`alt`,`exposition`, `inclination`,	`AveYearTemp`,"
					+"`MonthTempAmp`, leachn_param_id, climate_file, weather_table_name, weather_station_id " 
					+" FROM "+cellsTableName+" WHERE simulation_project_id = " + fromId + ";";
			
			if(!  myConnection.updateDb(s)) {
				return false;
			}
			
			
			s = "REPLACE INTO simulation_projects_xn5_cells_management " +
					" SELECT " + toId + ", " + "`xn5_cell_x`, `xn5_cell_y`,"
					+"`year`, `position`,`crop_management_id`,`crop_code`,"
					+"`variety`"
					+" FROM simulation_projects_xn5_cells_management " +
					"  WHERE simulation_project_id = " + fromId + ";";
			if(!  myConnection.updateDb(s)) {
				return false;
			}
			
			s = "REPLACE INTO "+managTableName +
					" SELECT " + toId + ", " + "`xn5_cell_x`, `xn5_cell_y`,"
					+"`crop_sequence_id`,`start_position`"

					+" FROM "+ managTableName +
					"  WHERE simulation_project_id = " + fromId + ";";
			if(!  myConnection.updateDb(s)) {
				return false;
			}
			
			
			
			s = "REPLACE INTO simulation_projects_xpn " +
					" SELECT " + toId + ", " + "xpnText"
					+" FROM simulation_projects_xpn " +
					"  WHERE simulation_project_id = " + fromId + ";";
			if(!  myConnection.updateDb(s)) {
				return false;
			}
			s = "REPLACE INTO simulation_projects_xpi " +
					" SELECT " + toId + ", grid, xpiText"
					+" FROM simulation_projects_xpi " +
					"  WHERE simulation_project_id = " + fromId + ";";
			if(!  myConnection.updateDb(s)) {
				return false;
			}
			
			s = "REPLACE INTO simulation_projects_special_output " +
					" SELECT " + toId + ",  specialOutputText"
					+" FROM simulation_projects_special_output " +
					"  WHERE simulation_project_id = " + fromId + ";";
			if(!  myConnection.updateDb(s)) {
				return false;
			}
			
			s = "REPLACE INTO simulation_projects_output_varlist " +
					" SELECT " + toId + ",  varlistText"
					+" FROM simulation_projects_output_varlist " +
					"  WHERE simulation_project_id = " + fromId + ";";
			if(!  myConnection.updateDb(s)) {
				return false;
			}
			
			s = "REPLACE INTO simulation_projects_xnmpmas_coupling " +
					" SELECT " + toId + ",  `firstSowingDay`, `firstSowingMonth`, `firstSowingRelativeYear`, `lastHarvestDay`, `lastHarvestMonth`, `lastHarvestRelativeYear`, `MPMAS_commandline`, XN_basedir, simulation_basedir, history_link_map_xllcorner,  history_link_map_yllcorner,  history_link_map_cellsize "
					+" FROM simulation_projects_xnmpmas_coupling " +
					"  WHERE simulation_project_id = " + fromId + ";";
			if(!  myConnection.updateDb(s)) {
				return false;
			}
			s = "REPLACE INTO simulation_projects_grids " +
					" SELECT " + toId + ",  grid_id, plant_model"
					+" FROM simulation_projects_grids " +
					"  WHERE simulation_project_id = " + fromId + ";";
			if(!  myConnection.updateDb(s)) {
				return false;
			}
			
			return true;
		
	}
	//Soil parameterizations
	public ResultSet getListOfDaisyParameterizations () {
		String s = "SELECT daisy_param_name FROM info_daisy_parameterizations;";
		return  myConnection.query(s);	
	}
	public int getDaisyParamIdForDaisyParamName (String daisy_param_name) {
		String s = "SELECT daisy_param_id FROM info_daisy_parameterizations WHERE daisy_param_name = '"+ daisy_param_name +"';";
		try {
			ResultSet rs =  myConnection.query(s);
			
			if (!rs.first() )
			{ 	rs.close();
				return -1;
			}
			else
			{	
				int r = rs.getInt("daisy_param_id");
				rs.close();
				return r;
			}
		}
		catch( Exception e)
		{
			e.printStackTrace();
			return -1;
		}
	}
	public int addDaisyParameterization (String code, String description, String author) {
		try {
			
			String s = "SELECT daisy_param_id " +
					" FROM info_daisy_parameterizations t1 WHERE daisy_param_name = '" + code +"';";
			ResultSet rs = myConnection.query(s);			
			if(rs.first())
			{	rs.close();
				return -2; //code already exists
			}
			int id = 0;
			s ="SELECT max(daisy_param_id) as maxId " +
					" FROM info_daisy_parameterizations t1;";
			rs = myConnection.query(s);
			
			if ( rs.first() )
			{
				id = rs.getInt("maxId") + 1;
			}
			
			s = "INSERT INTO info_daisy_parameterizations VALUES ( " + id + ", '" + author
					+ "', '" + code   + "', '" + description + "');";
			boolean rc = myConnection.updateDb(s);
			rs.close();
			if (!rc) 
				return -1;
			else
				return id;
		}
		catch (Exception e) 
		{
			  e.printStackTrace();
			  return -1;
		}
		
	}	
	
	public enum idTypes {
		daisyParam, daisyPools, soilInit, soilParam, management, gecros
	}
	
	public ResultSet getListOfIdCodes ( idTypes which ) {
		
		String name, table;
		switch(which) {
			case daisyParam:
				name = "daisy_param_name";
				table = "info_daisy_parameterizations";
				break;
			case daisyPools:
				name = "som_param_name";
				table = "info_sompools_initializations";
				break;		
			case soilInit: 
				name = "soilinit_param_name";
				table = "info_soil_initializations";
				break;
			case soilParam:
				name = "soil_param_name";
				table = "info_soil_parameterizations";
				break;
			case management:
				name = "crop_management_name";
				table = "info_crop_managements";
				break;
			case gecros:
				name = "plant_param_name";
				table = "info_plant_parameterizations";
				break;
			default:
				return null;
		}
		
		String s = "SELECT "+ name + " FROM "+ table +";";
		return  myConnection.query(s);	
	}
	public int getNumericIdForIdCode (idTypes which , String valueName) {
		
		String id, name, table;
		switch(which) {
			case daisyParam:
				id = "daisy_param_id";
				name = "daisy_param_name";
				table = "info_daisy_parameterizations";
				break;
			case daisyPools:
				id = "sompools_param_id";
				name = "som_param_name";
				table = "info_sompools_initializations";
				break;		
			case soilInit: 
				id = "soilinit_param_id";
				name = "soilinit_param_name";
				table = "info_soil_initializations";
				break;
			case soilParam:
				id = "soil_param_id";
				name = "soil_param_name";
				table = "info_soil_parameterizations";
				break;
			case management:
				id = "crop_management_id";
				name = "crop_management_name";
				table = "info_crop_managements";
				break;
			case gecros:
				id = "plant_param_id";
				name = "plant_param_name";
				table = "info_plant_parameterizations";
				break;
			default:
				return -1;
		}
		
		String s = "SELECT "+id+" FROM " + table+ " WHERE "+name+ " = '"+ valueName +"';";
		try {
			ResultSet rs =  myConnection.query(s);
			if (!rs.first() )
			{	rs.close();
				return -1;
			}
			else 
			{
				int r = rs.getInt(id);
				rs.close();
				return r;
			}
		}
		catch( Exception e)
		{
			e.printStackTrace();
			return -1;
		}
	}
	public int addIdCode (idTypes which, String code, String description, String author) {
		try {
			String id, name, table;
			switch(which) {
				case daisyParam:
					id = "daisy_param_id";
					name = "daisy_param_name";
					table = "info_daisy_parameterizations";
					break;
				case daisyPools:
					id = "sompools_param_id";
					name = "som_param_name";
					table = "info_sompools_initializations";
					break;		
				case soilInit: 
					id = "soilinit_param_id";
					name = "soilinit_param_name";
					table = "info_soil_initializations";
					break;
				case soilParam:
					id = "soil_param_id";
					name = "soil_param_name";
					table = "info_soil_parameterizations";
					break;
				case management:
					id = "crop_management_id";
					name = "crop_management_name";
					table = "info_crop_managements";
					break;
				case gecros:
					id = "plant_param_id";
					name = "plant_param_name";
					table = "info_plant_parameterizations";
					break;
				default:
					return -1;
			}
			String s = "SELECT "+id+ 
					" FROM "+table+ " t1 WHERE "+name+" = '" + code +"';";
			ResultSet rs = myConnection.query(s);			
			if(rs.first())
			{	rs.close();
				return -2; //code already exists
			}
			int valueId = 0;
			s ="SELECT max("+id+") as maxId " +
					" FROM "+table+" t1;";
			rs = myConnection.query(s);
			
			if ( rs.first() )
			{
				valueId = rs.getInt("maxId") + 1;
			}
			
			s = "INSERT INTO "+table+" VALUES ( " + valueId + ", '" + author
					+ "', '" + code   + "', '" + description + "');";
			boolean rc = myConnection.updateDb(s);
			rs.close();
			if (!rc) 
				return -1;
			else
				return valueId;
		}
		catch (Exception e) 
		{
			  e.printStackTrace();
			  return -1;
		}
		
	}	
	//PLANT PARAMETERIZATIONS
	public ResultSet getListOfPlantParameterizations () {
			String s = "SELECT plant_param_name FROM info_plant_parameterizations;";
			return  myConnection.query(s);	
	}
	public int getPlantParamIdForPlantParamName (String plant_param_name) {
			String s = "SELECT plant_param_id FROM info_plant_parameterizations WHERE plant_param_name = '"+ plant_param_name +"';";
			try {
				ResultSet rs =  myConnection.query(s);
				int r = -1;
				if (rs.first() )
					r = rs.getInt("plant_param_id");
				
				rs.close();
				return r;
			}
			catch( Exception e)
			{
				e.printStackTrace();
				return -1;
			}
	}
	public int addPlantParameterization (String code, String description, String author, boolean daily_timestep) {
		try {
			
			String s = "SELECT plant_param_id " +
					" FROM info_plant_parameterizations t1 WHERE plant_param_name = '" + code +"';";
			ResultSet rs = myConnection.query(s);			
			if(rs.first())
			{	rs.close();
				return -2; //code already exists
			}
			int id = 0;
			s ="SELECT max(plant_param_id) as maxId " +
					" FROM info_plant_parameterizations t1;";
			rs = myConnection.query(s);
			
			if ( rs.first() )
			{
				id = rs.getInt("maxId") + 1;
			}
			
			s = "INSERT INTO info_plant_parameterizations VALUES ( " + id + ", '" + author
					+ "', '" + code   + "', '" + description + "', "+ daily_timestep +");";
			boolean rc = myConnection.updateDb(s);
			rs.close();
			if (!rc) 
				return -1;
			else
				return id;
		}
		catch (Exception e) 
		{
			  e.printStackTrace();
			  return -1;
		}
		
	}
	
	//AUTHORS
	public int addAuthor (String author_id, String first_name, String surname) {
		try {
			
			String s = "SELECT author_id " +
					" FROM info_authors t1 WHERE author_id = '" + author_id +"';";
			ResultSet rs = myConnection.query(s);			
			if(rs.first())
			{	rs.close();
				return -2; //code already exists
			}

			
			s = "INSERT INTO info_authors VALUES ( '" + author_id + "', '" + first_name 
					+ "', '" + surname + "');";
			boolean rc = myConnection.updateDb(s);
			rs.close();
			if (!rc) 
				return -1;
			else
				return 0;
		}
		catch (Exception e) 
		{
			  e.printStackTrace();
			  return -1;
		}
		
	}
	public ResultSet getListOfAuthors () {
		String s = "SELECT author_id FROM info_authors;";
		return  myConnection.query(s);	
	}
	
	public ResultSet getGecrosParameterizations () {
		String s = "SELECT * FROM plant_parameterization_gecros" ;
		return  myConnection.query(s);	
	}
	//PROJECT GENERAL INFO
	public ResultSet getGeneralInfoForProject( int projectId ) {
		String s = "SELECT startDay, startMonth, startYear, endDay, endMonth, endYear, plotSize, adaptive, max_daily_precip, xn5_cells_table, bems_cells_management_table" 
				+ ", elevationCorrectionType, elevationCorrectionClassSize, elevationInfoTableWeatherCells , co2_table, kc_param_id, numGrids"	+
				" FROM simulation_projects_general WHERE simulation_project_id = " + projectId + ";";
				
		return myConnection.query(s);
	
	}
	public boolean updateGeneralInfoForProject( int projectId,
			String startYear, String endYear , 
			String startMonth, String startDay, 
			String endMonth, String endDay,
			String plotSize , String adaptive, String max_daily_precip, String xn5cells, String bemsManag,
			String elevationCorrectionType, String elevationCorrectionClassSize, 
			String elevationInfoTableWeatherCells, String co2Table, String kc_param_id, String numGrids) {
		    String s = "REPLACE INTO simulation_projects_general " +
				"VALUES ("+ projectId+", "+ startYear +", "+endYear+","+startMonth+","+startDay+","
				+endMonth+","+endDay+","+plotSize+","+adaptive+","+max_daily_precip+",'"+xn5cells+"','"+ bemsManag
				+"',"+ elevationCorrectionType+","+ elevationCorrectionClassSize+", '"+elevationInfoTableWeatherCells+"', '"+co2Table+"', " +
						""+kc_param_id+", " +numGrids+");";
		    //System.out.println(s);
		return myConnection.updateDb(s);
	}
	
	//PROJECT GRIDS INFO
		public ResultSet getGridsInfoForProject( int projectId ) {
			String s = "SELECT number as grid_id, IFNULL(plant_model, 'gecros') as plant_model "	+
					" FROM  simulation_projects_general t1 " +
					" JOIN aux_integers t2 " +
						" ON t1.simulation_project_id = " + projectId  +
						" AND t2.number BETWEEN 0 AND t1.numGrids -1 " +
					" LEFT JOIN simulation_projects_grids t3 "+
					"     ON t1.simulation_project_id = t3.simulation_project_id " +
					"		AND t2.number = t3.grid_id  "+		
					" ORDER BY grid_id;";
		
			return myConnection.query(s);
		
		}
		public boolean updateGridsInfoForProject( int projectId,
				String grid_id, String plant_model ) {
			    String s = "REPLACE INTO simulation_projects_grids " +
					"VALUES ("+ projectId+", "+ grid_id +", '"+plant_model+"');";
			    //System.out.println(s);
			return myConnection.updateDb(s);
		}
	//PROJECT COUPLING INFO
	public ResultSet getCouplingInfoForProject( int projectId ) {
		String s = "SELECT firstSowingDay, firstSowingMonth, firstSowingRelativeYear, lastHarvestDay, lastHarvestMonth, lastHarvestRelativeYear, MPMAS_commandline," +
				"	XN_basedir, simulation_basedir, history_link_map_xllcorner, history_link_map_yllcorner, history_link_map_cellsize" +
				" FROM simulation_projects_xnmpmas_coupling WHERE simulation_project_id = " + projectId + ";";
				
		return myConnection.query(s);
	
	}
	public boolean updateCouplingInfoForProject( int projectId,
			String firstSowingDay, String firstSowingMonth,  String firstSowingRelativeYear, 
			String lastHarvestDay, String lastHarvestMonth, String lastHarvestRelativeYear, String MPMAS_commandline,
			String XN_basedir, String simulation_basedir, double xllcorner, double yllcorner, double cellsize
			) {
				String s = "REPLACE INTO simulation_projects_xnmpmas_coupling " +
				"VALUES ("+ projectId+", "+ firstSowingDay +", "+firstSowingMonth+","+firstSowingRelativeYear+","
				+lastHarvestDay+","+lastHarvestMonth+","+lastHarvestRelativeYear+", '"+MPMAS_commandline +"'"
				+", '"+XN_basedir +"'" +	", '"+simulation_basedir +"'"
				+","+xllcorner+","+yllcorner+","+cellsize
				
				
						+");";	
		return myConnection.updateDb(s);
	}
	
	
	
	//GECROS selection
	public ResultSet getGecrosParameterizationSelection (int simprojectID) {
		String s =
			" SELECT IFNULL(t3.include,0) AS selected, t1.crop_code, t1.variety, t2.plant_param_name, t1.plant_param_id, daily_timestep  "
			+" FROM plant_parameterization_gecros t1"
			+" JOIN info_plant_parameterizations t2 ON t1.plant_param_id = t2.plant_param_id"
			+" LEFT JOIN simulation_projects_gecros_crops_included t3 "
			+" ON t1.crop_code = t3.crop_code"
			+" AND t1.variety = t3.variety"
			+" AND t1.plant_param_id = t3.plant_param_id"
			+" AND t3.simulation_project_id = " + simprojectID 
			+" ORDER BY t1.crop_code, t1.variety, t1.plant_param_id";
		return  myConnection.query(s);	
	}
	public String getGecrosParameterizationSelectionString (int simprojectID) {
		String s =
			" SELECT IFNULL(t3.include,0) AS selected, t1.crop_code, t1.variety, t2.plant_param_name, t1.plant_param_id, daily_timestep  "
			+" FROM plant_parameterization_gecros t1"
			+" JOIN info_plant_parameterizations t2 ON t1.plant_param_id = t2.plant_param_id"
			+" LEFT JOIN simulation_projects_gecros_crops_included t3 "
			+" ON t1.crop_code = t3.crop_code"
			+" AND t1.variety = t3.variety"
			+" AND t1.plant_param_id = t3.plant_param_id"
			+" AND t3.simulation_project_id = " + simprojectID 
			+" ORDER BY t1.crop_code, t1.variety, t1.plant_param_id";
		return  s;	
	}
	public ResultSet getGecrosParameterizationSelectionOnly (int simprojectID) {
		String s =
			" SELECT  t1.crop_code, t1.variety, t2.plant_param_id, t4.crop_name, daily_timestep  "
			+" FROM plant_parameterization_gecros t1"
			+" JOIN info_plant_parameterizations t2 ON t1.plant_param_id = t2.plant_param_id"
			+" JOIN simulation_projects_gecros_crops_included t3 "
			+" ON t1.crop_code = t3.crop_code AND t3.include = 1"
			+" AND t1.variety = t3.variety"
			+" AND t1.plant_param_id = t3.plant_param_id"
			+" AND t3.simulation_project_id = " + simprojectID 
			+" JOIN info_crops t4 ON t1.crop_code = t4.crop_code "
			+" ORDER BY t1.crop_code, t1.variety, t1.plant_param_id";
		return  myConnection.query(s);	
	}
	public boolean updateSelectedGecrosPlantsForProject(int project_id,  int plant_param_id, String crop_code
				, String variety, int selected) {
		String s = "REPLACE INTO simulation_projects_gecros_crops_included " +
				"  VALUES ("+project_id+", "+plant_param_id+", '"+crop_code+"', '"+ variety+"', "+selected+");";
		return myConnection.updateDb(s);
	}
	
	//CELLS
	public ResultSet getPossibleLinkedDataInfo(int id, String type) {
		
		String s = "SELECT  `xn5_cell_x`, `xn5_cell_y`, `profileID`, `soil_param_id`, `daisy_param_id`, `sompools_param_id`,"
				+ " `soilinit_param_id`, `lat`,`lon`, `alt`, `exposition`, `inclination`, `AveYearTemp`, `MonthTempAmp`, " +
				"leachn_param_id, climate_file, weather_table_name, weather_station_id"
				+ " FROM `simulation_projects_linked_data`"
				+ " WHERE simulation_project_id = "+ id 
				+ " AND active = 1"
				+ " AND simulation_projects_data_type = '"+type +"';";
		return  myConnection.query(s);
	}
	
	
	public ResultSet getSoilInfoForProject(int id, String cellsTableName) {
		String s = "SELECT  `xn5_cell_x`, `xn5_cell_y`, `profileID`, `soil_param_id`, `daisy_param_id`, `sompools_param_id`,"
					+ " `soilinit_param_id`, `lat`,`lon`, `alt`, `exposition`, `inclination`, `AveYearTemp`, `MonthTempAmp`, " +
					"leachn_param_id, climate_file, weather_table_name, weather_station_id"
					+ " FROM `"+ cellsTableName +"`"
					+ " WHERE simulation_project_id = "+ id +";";
		return  myConnection.query(s);
	}
	public boolean addCellForProject(int project_id, int xn5_cell_x, int xn5_cell_y, String profileID, int soil_param_id,
				int daisy_param_id, int sompools_param_id, int soilinit_param_id, double lat, double lon, double alt, 
				String exposition, double inclination, double AveYearTemp, double MonthTempAmp, int leachn_param_id, String climate_file ,
				String weather_table_name, int weather_station_id
				){
		String s = "INSERT INTO `simulation_projects_xn5_cells`" +
				"	   (simulation_project_id, `xn5_cell_x`, `xn5_cell_y`, `profileID`, `soil_param_id`, `daisy_param_id`, `sompools_param_id`,"
					+ " `soilinit_param_id`, `lat`,`lon`, `alt`, `exposition`, `inclination`, `AveYearTemp`, `MonthTempAmp`, leachn_param_id, " +
					"climate_file, weather_table_name, weather_station_id)"
					+ " VALUES ("+project_id+","+xn5_cell_x+","+xn5_cell_y+",'"+profileID+"',"+soil_param_id+","+daisy_param_id+","+sompools_param_id+","+
							+ soilinit_param_id+","+lat+","+lon+","+alt+",'"+exposition+"',"+inclination+","+AveYearTemp+","+MonthTempAmp+","
							+leachn_param_id+",'"+climate_file+"', '" + weather_table_name + "',"+  weather_station_id
					+");";
		return  myConnection.updateDb(s);
	}
	public boolean updateCellForProject(int project_id, int xn5_cell_x, int xn5_cell_y, String profileID, int soil_param_id,
			int daisy_param_id, int sompools_param_id, int soilinit_param_id, double lat, double lon, double alt, 
			String exposition, double inclination, double AveYearTemp, double MonthTempAmp, int leachn_param_id, String climate_file , String weather_table_name, int weather_station_id){
			String s = "REPLACE INTO `simulation_projects_xn5_cells`" +
			"	   (simulation_project_id, `xn5_cell_x`, `xn5_cell_y`, `profileID`, `soil_param_id`, `daisy_param_id`, `sompools_param_id`,"
				+ " `soilinit_param_id`, `lat`,`lon`, `alt`, `exposition`, `inclination`, `AveYearTemp`, `MonthTempAmp`, leachn_param_id, climate_file, weather_table_name, weather_station_id)"
				+ " VALUES ("+project_id+","+xn5_cell_x+","+xn5_cell_y+", '"+profileID+"',"+soil_param_id+","+daisy_param_id+","+sompools_param_id+","+
						+ soilinit_param_id+","+lat+","+lon+","+alt+",'"+exposition+"',"+inclination+","+AveYearTemp+","+MonthTempAmp+","+leachn_param_id+",'"+climate_file+"'"
						+ ", '"+ weather_table_name+"'," + weather_station_id
				+");";
		return  myConnection.updateDb(s);
	}
	public boolean addCell(int project_id, int xn5_cell_x, int xn5_cell_y) {
		String s = "INSERT INTO `simulation_projects_xn5_cells`" +
		"	   SELECT "+project_id+", "+xn5_cell_x+", "+xn5_cell_y+", `profileID`, `soil_param_id`, `daisy_param_id`, `sompools_param_id`,"
			+ " `soilinit_param_id`, `lat`,`lon`, `alt`, `exposition`, `inclination`, `AveYearTemp`, `MonthTempAmp`, leachn_param_id, climate_file" +
			"	, weather_table_name, weather_station_id"
			+ " FROM simulation_projects_xn5_cells WHERE simulation_project_id = " + project_id + " ORDER BY `xn5_cell_x` DESC, `xn5_cell_y` DESC LIMIT 1"
			+ ";";
		return  myConnection.updateDb(s);
	}
	public boolean deleteCell(int project_id, int xn5_cell_x, int xn5_cell_y) {
		String s = "DELETE FROM  `simulation_projects_xn5_cells_management`" +
		"	    WHERE simulation_project_id = " + project_id + " AND `xn5_cell_x` = " + xn5_cell_x+ " AND  `xn5_cell_y` = "+xn5_cell_y
			+ ";";
		if (!myConnection.updateDb(s)) {
			return false;
		}
		String s2 = "DELETE FROM  `simulation_projects_xn5_cells`" +
		"	    WHERE simulation_project_id = " + project_id + " AND `xn5_cell_x` = " + xn5_cell_x+ " AND  `xn5_cell_y` = "+xn5_cell_y
			+ ";";
		return  myConnection.updateDb(s2);
	}
    public boolean userUpdateTable(String table, int simulation_project_id, String updateSETstring, String updateWHEREstring ) {
  		try {
      		String sqlString = 
      				"UPDATE  " + table + " SET " 
      				+   updateSETstring
      				+ " WHERE  simulation_project_id = " + simulation_project_id 
      				+ " AND " + updateWHEREstring + ";"
      				;
      		return myConnection.updateDb(sqlString );
  		}
  		catch(Exception ex) {
  			ex.printStackTrace();
  			return false;
  		}	
    }
	
	public ResultSet getManagementInfoForProject(int id) {
		String s = "SELECT  `xn5_cell_x`, `xn5_cell_y`, `year`, `position`, `crop_management_id`, `crop_code`,"
				+ " `variety`"
				+ " FROM `simulation_projects_xn5_cells_management`"
				+ " WHERE simulation_project_id = "+ id +";";
		return  myConnection.query(s);
	}
	
	
	public boolean addManagementForProject(int project_id, int xn5_cell_x, int xn5_cell_y, int year, int position, int crop_management_id, String crop_code, String variety){
	String s = "INSERT INTO `simulation_projects_xn5_cells_management`" +
			"	   (simulation_project_id, `xn5_cell_x`, `xn5_cell_y`, `year`, `position`, `crop_management_id`, `crop_code`,"
				+ " `variety`)"
				+ " VALUES ("+project_id+","+xn5_cell_x+","+xn5_cell_y+", "+year+", "+position+", "+crop_management_id +", '"+crop_code+"', '"+variety+"'"
				+");";
		return  myConnection.updateDb(s);
	}
	public boolean updateManagementForProject(int project_id, int xn5_cell_x, int xn5_cell_y,  int year, int position, int crop_management_id, String crop_code, String variety){
		String s = "REPLACE INTO `simulation_projects_xn5_cells_management`" +
			"	   (simulation_project_id, `xn5_cell_x`, `xn5_cell_y`, `year`, `position`, `crop_management_id`, `crop_code`,"
				+ " `variety`)"
				+ " VALUES ("+project_id+","+xn5_cell_x+","+xn5_cell_y+", "+year+", "+position+", "+crop_management_id +", '"+crop_code+"', '"+variety+"'"
				+");";
		return  myConnection.updateDb(s);
	}
	public boolean addManagementRow(int project_id, int xn5_cell_x, int xn5_cell_y, int year, int position){
	String s = "INSERT INTO `simulation_projects_xn5_cells_management`" +
			"	  SELECT "+project_id+","+xn5_cell_x+","+xn5_cell_y+", "+year+", "+position+", `crop_management_id`, `crop_code`,"
				+ " `variety` " +
					" FROM  `simulation_projects_xn5_cells_management`" +
					" WHERE simulation_project_id = "+project_id+  
					"		ORDER BY xn5_cell_x DESC, xn5_cell_y DESC, year DESC, position DESC LIMIT 1"
				+";";
		return  myConnection.updateDb(s);
	}
	public boolean deleteManagementRow(int project_id, int xn5_cell_x, int xn5_cell_y, int year, int position) {
		String s = "DELETE FROM  `simulation_projects_xn5_cells_management`" +
		"	    WHERE simulation_project_id = " + project_id + " AND `xn5_cell_x` = " + xn5_cell_x+ " AND  `xn5_cell_y` = "+xn5_cell_y
				+ " AND year = "+year+ " AND position = " + position 
			+ ";";
		
		return  myConnection.updateDb(s);
	}
	
	public ResultSet getManagementInfoForProjectBEMS(int id, String bemsManagTableName) {
		String s = "SELECT  `xn5_cell_x`, `xn5_cell_y`, crop_sequence_id, start_position"
				+ " FROM `"+bemsManagTableName+"`"
				+ " WHERE simulation_project_id = "+ id +";";
		return  myConnection.query(s);
	}
	
	
	public boolean addManagementForProjectBEMS(int project_id, int xn5_cell_x, int xn5_cell_y){
	String s = "INSERT INTO `simulation_projects_bems_cells_management`" +
			"	   (simulation_project_id, `xn5_cell_x`, `xn5_cell_y`)"
				+ " VALUES ("+project_id+","+xn5_cell_x+","+xn5_cell_y
				+");";
		return  myConnection.updateDb(s);
	}
	public boolean updateManagementForProjectBEMS(int project_id, int xn5_cell_x, int xn5_cell_y,  int crop_sequence_id, int start_position){
		String s = "REPLACE INTO `simulation_projects_bems_cells_management`" +
			"	   (simulation_project_id, `xn5_cell_x`, `xn5_cell_y`, `crop_sequence_id`, `start_position`)"
				+ " VALUES ("+project_id+","+xn5_cell_x+","+xn5_cell_y+", "+crop_sequence_id+","+start_position
				+");";
		return  myConnection.updateDb(s);
	}
	public boolean addManagementRowBEMS(int project_id, int xn5_cell_x, int xn5_cell_y){
	String s = "INSERT INTO `simulation_projects_bems_cells_management`" +
			"	  SELECT "+project_id+","+xn5_cell_x+","+xn5_cell_y+", crop_sequence_id, start_position" +
					" FROM  `simulation_projects_bems_cells_management`" +
					" WHERE simulation_project_id = "+project_id+  
					"		ORDER BY xn5_cell_x DESC, xn5_cell_y DESC LIMIT 1"
				+";";
		return  myConnection.updateDb(s);
	}
	public boolean deleteManagementBEMS(int project_id, int xn5_cell_x, int xn5_cell_y) {
		String s = "DELETE FROM  `simulation_projects_bems_cells_management`" +
		"	    WHERE simulation_project_id = " + project_id + " AND `xn5_cell_x` = " + xn5_cell_x+ " AND  `xn5_cell_y` = "+xn5_cell_y
				
			+ ";";
		
		return  myConnection.updateDb(s);
	}
	
	public ResultSet getManagementsForCrop(String crop_code, String variety) {
		String s = "SELECT  `crop_management_id`, `crop_management_code`"
				+ " FROM `crop_management_planting` t1"
				+ " JOIN info_crop_managements t2 ON t1.cropManagement_id = t2.crop_management_id"
				+ " WHERE crop_code = '"+ crop_code +"' AND variety = '"+variety+"';";
		return  myConnection.query(s);
	}
	public ResultSet getVarietiesForCrop(String crop_code, int simprojectID) {
		String s = "SELECT  DISTINCT `variety`"
						+" FROM plant_parameterization_gecros t1"
						+" JOIN info_plant_parameterizations t2 ON t1.plant_param_id = t2.plant_param_id"
						+" JOIN simulation_projects_gecros_crops_included t3 "
						+" ON t1.crop_code = t3.crop_code AND t3.include = 1"
						+" AND t1.plant_param_id = t3.plant_param_id"
						+" AND t3.simulation_project_id = " + simprojectID 
						+" ;";
		return  myConnection.query(s);
	}	
	
	public ResultSet getXpnContentForProject(int simProjectId) {
		String s = "SELECT xpnText FROM " +
				"	simulation_projects_xpn "
				+ " WHERE simulation_project_id = " + simProjectId +";";
		
		return  myConnection.query(s);
	}
	public boolean setXpnContentForProject(int simProjectId, String text) {
		String s = "REPLACE INTO " +
				"	simulation_projects_xpn "
				+ " VALUES(  " + simProjectId
				+ " , '" + text + "');";
		
		return  myConnection.updateDb(s);
	}	
	public boolean copyXpnContentForProjectFromProject(int toProjectId, int fromProjectId) {
		String s = "REPLACE INTO " +
				"	simulation_projects_xpn "
				+ " ( simulation_project_id, xpnText ) " 
				+ " SELECT " + toProjectId + ",  xpnText  FROM simulation_projects_xpn WHERE simulation_project_id = " 
				+ fromProjectId + ";";
				
		return  myConnection.updateDb(s);
	}
	

	public ResultSet getXpiContentForProject(int simProjectId, int gridId) {
		String s = "SELECT xpiText FROM " +
				"	simulation_projects_xpi "
				+ " WHERE simulation_project_id = " + simProjectId 
				+ " AND grid = "+ gridId
				+";";
		
		return  myConnection.query(s);
	}
	public boolean setXpiContentForProject(int simProjectId, int gridId, String text) {
		String s = "REPLACE INTO " +
				"	simulation_projects_xpi "
				+ " VALUES(  " + simProjectId + ", " + gridId
				+ " , '" + text + "');";
		
		return  myConnection.updateDb(s);
	}	
	public boolean copyXpiContentForProjectFromProject(int toProjectId, int fromProjectId, int gridId) {
		String s = "REPLACE INTO " +
				"	simulation_projects_xpi "
				+ " ( simulation_project_id, grid, xpiText ) " 
				+ " SELECT " + toProjectId + ",  grid, xpiText  FROM simulation_projects_xpi WHERE simulation_project_id = " 
				+ fromProjectId + " AND grid = "+ gridId + ";";
				
		return  myConnection.updateDb(s);
	}
	
	
	public ResultSet getContentForProject(String table, String columnName, int simProjectId) {
		String s = "SELECT "+columnName+" FROM " +
				"	"+table+" "
				+ " WHERE simulation_project_id = " + simProjectId +";";
		
		return  myConnection.query(s);
	}
	public boolean setContentForProject(String table, String columnName, int simProjectId, String text) {
		String s = "REPLACE INTO " +
				"	"+table+" "
				+ " VALUES(  " + simProjectId
				+ " , '" + text + "');";
		
		return  myConnection.updateDb(s);
	}	
	public boolean copyContentForProjectFromProject(String table, String columnName, int toProjectId, int fromProjectId) {
		String s = "REPLACE INTO " +
				"	"+table+" "
				+ " ( simulation_project_id, "+columnName+" ) " 
				+ " SELECT " + toProjectId + ",  "+columnName+"  FROM "+table+" WHERE simulation_project_id = " 
				+ fromProjectId + ";";
				
		return  myConnection.updateDb(s);
	}
	
	public boolean writeWeatherFilesForProject(String directory, int projectId, String projectName, int projectType, String xn5_cells_table_name) {
		boolean ok = false;
		
		try {
			
			String s0 = "SELECT startYear, endYear,  adaptive, max_daily_precip, elevationCorrectionType, elevationCorrectionClassSize, elevationInfoTableWeatherCells, co2_table "  
					+ " FROM simulation_projects_general "
					+ " WHERE simulation_project_id = "+ projectId + ";";
			ResultSet projectInfo = myConnection.query(s0);	
	
			if (! projectInfo.first()) {
				JOptionPane.showMessageDialog(null, "Error: Missing general information on simulation project.");

				throw new Exception("Error: Missing general information on simulation project.");
			}

			int startYear = projectInfo.getInt("startYear");
			int endYear = projectInfo.getInt("endYear");
			double maxDailyPrecip = projectInfo.getDouble("max_daily_precip");
			
			int elevationCorrectionType = projectInfo.getInt("elevationCorrectionType");
			int elevCorrectionClassSize = projectInfo.getInt("elevationCorrectionClassSize");
			String elevationInfoTableForWeatherCells = projectInfo.getString("elevationInfoTableWeatherCells");
			String co2Table = projectInfo.getString("co2_table");

			int writeHistory = 1;
			if ( projectType == 0)
				writeHistory = 0;
			else if ( projectType == 3)
				writeHistory = 2;
			
			
			String s1 = "SELECT DISTINCT weather_table_name, weather_station_id "
						+" FROM `"+xn5_cells_table_name+"` t1"
						+" WHERE simulation_project_id = " + projectId
						+ " AND weather_station_id > -1 ;"
						;
			if (elevationCorrectionType > 0 && elevCorrectionClassSize > -1) {
				s1 = "SELECT DISTINCT weather_table_name, weather_station_id, ROUND(alt / " +elevCorrectionClassSize + " ) * "+ elevCorrectionClassSize+ " AS alt"
						+" FROM `"+xn5_cells_table_name+"` t1"
						+" WHERE simulation_project_id = " + projectId
						+ " AND weather_station_id > -1 ;"
						;
			}
			ResultSet cellList = myConnection.query(s1);	
			
			if (! cellList.first()) {
				return true;
			}
			
			boolean doCO2 = false;
			ResultSet co2Series = null;
			
			if (co2Table != null && ! co2Table.isEmpty() && !co2Table.matches("^\\s*$") )
			{
				doCO2 = true;
				
				if (! co2Table.contains(".")) {
					co2Table = "for1695_weather."+co2Table;
				}
				
				String s2 = "SELECT year, month, day, co2_ppm "
						+ " FROM " + co2Table 
						+ " 	WHERE year BETWEEN "+ startYear +"  AND "+ endYear +
						"			ORDER BY year, month, day;";
				co2Series = myConnection.query(s2);
				
				if (! co2Series.first() ) {
					JOptionPane.showMessageDialog(null, "Error: No CO2 time series found in table "+ co2Table + ".");
					System.out.println("Error:  No CO2 time series found in table "+ co2Table + ".");
					co2Series.close();
					return false;
				}
					
			}
			
			
			
			
			cellList.beforeFirst();
			boolean is_first = true;
			while (cellList.next())//move cursor to next value (including the first value in the first loop)
			{
				
				String[] tables = cellList.getString("weather_table_name").split(";");
				
				String filename = directory +"/" + tables[0] + "_" + cellList.getString("weather_station_id") ;
				if (elevationCorrectionType > 0 && elevCorrectionClassSize > -1) {
					filename += String.format("%04d", cellList.getInt("alt") );
					
				}
				boolean rc = writeWeatherData(filename, cellList.getString("weather_table_name") , cellList.getInt("weather_station_id"), 
														startYear, endYear, writeHistory, !is_first, 
															directory + "/"+ projectName + "_init_weather_history.txt", maxDailyPrecip, 
															elevationCorrectionType, 
															elevationCorrectionType > 0 && elevCorrectionClassSize > -1 ?  	cellList.getDouble("alt") : 0, 
														elevationInfoTableForWeatherCells, doCO2, co2Series);
				
				if (!rc) {
					throw new Exception("Error when writing weather data and weather history for BEMS");
				}
				is_first = false;
			}
			if (writeHistory > 0) {
				
				String sc = " SELECT weather_station_id, COUNT(*) as N " +
						"FROM " +
						"  ( SELECT DISTINCT weather_table_name, weather_station_id" +
						"		FROM `"+xn5_cells_table_name+"` t1" +
						"			WHERE simulation_project_id = " + projectId +
						"   ) u1 GROUP BY weather_station_id " +
						"	HAVING N > 1;";
				ResultSet testUniqueness = myConnection.query(sc);

				if ( testUniqueness.first()) {
					JOptionPane.showMessageDialog(null, "Error when writing weather history for BEMS: weather_station_id not unique over used datasets");
					throw new Exception("Error when writing weather history for BEMS: weather_station_id not unique over used datasets");
				}
				testUniqueness.close();
				
				
				String s3 = "SELECT xn5_cell_x, xn5_cell_y, weather_station_id "
						+" FROM `"+xn5_cells_table_name+"` t1"
						+" WHERE simulation_project_id = " + projectId
						+" AND weather_station_id < 0"
					;
				ResultSet testAssoc = myConnection.query(s3);
				
				if ( testAssoc.first()) {
    				JOptionPane.showMessageDialog(null, "Error when writing weather history for BEMS: no weather_station_id defined for cell " + testAssoc.getInt("xn5_cell_x") +","+testAssoc.getInt("xn5_cell_y"));
					throw new Exception("Error when writing weather history for BEMS: no weather_station_id defined for cell " + testAssoc.getInt("xn5_cell_x") +","+testAssoc.getInt("xn5_cell_y"));
				}
				
				testAssoc.close();
				String s4 = "SELECT MAX(xn5_cell_x) as maxX, MAX(xn5_cell_y) as maxY, MIN(xn5_cell_x) as minX, MIN(xn5_cell_y) as minY "
						+" FROM `"+xn5_cells_table_name+"` t1"
						+" WHERE simulation_project_id = " + projectId
						+"  ;"
					;
				ResultSet xnDims = myConnection.query(s4);
				
				String s2 = "SELECT xn5_cell_x, xn5_cell_y, weather_station_id  "
						+" FROM `"+xn5_cells_table_name+"` t1"
						+" WHERE simulation_project_id = " + projectId
						+" ORDER BY xn5_cell_y DESC, xn5_cell_x ;"				
						;
				
				if (elevationCorrectionType > 0 && elevCorrectionClassSize > -1) {
					s2 = "SELECT xn5_cell_x, xn5_cell_y, weather_station_id , ROUND(alt / "+ elevCorrectionClassSize+") * "+ elevCorrectionClassSize +" AS alt  "
							+" FROM `"+xn5_cells_table_name+"` t1"
							+" WHERE simulation_project_id = " + projectId
							+" ORDER BY xn5_cell_y DESC, xn5_cell_x ;"				
							;
				}
				
				
				ResultSet xn5cellList = myConnection.query(s2);
				
				ResultSet couplingInfo = getCouplingInfoForProject(projectId);
				if(! couplingInfo.first() )
				{
					JOptionPane.showMessageDialog(null, "Error when writing weather history for BEMS: no coupling ini information for project\n");
					throw new Exception("Error when writing weather history for BEMS: no coupling ini information for project\n");
				}
				
				PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(directory + "/"+ projectName + "_xnCellStationMap.txt")));

				xnDims.first();
				out.println("NCOLS " + (xnDims.getInt("maxX") + 1));
				out.println("NROWS " + (xnDims.getInt("maxY") + 1));
				out.println("XLLCORNER "+couplingInfo.getInt("history_link_map_xllcorner"));
				out.println("YLLCORNER "+couplingInfo.getInt("history_link_map_yllcorner"));
				out.println("CELLSIZE "+couplingInfo.getInt("history_link_map_cellsize"));
				out.println("NODATA_VALUE -1");
				
				int xc = 0;
				int yc = xnDims.getInt("maxY");
				
				xn5cellList.first();
				while(yc >= 0) {
				
					int x = -1; 
					int y = -1;
					
					if (!xn5cellList.isAfterLast()) 
					{
						x = xn5cellList.getInt("xn5_cell_x");
						y = xn5cellList.getInt("xn5_cell_y");
					}
					
					if (x != xc || y != yc) {
						// skipped cells may actually happen and should be allowed
						out.print(-1);
	
					} 
					else if (xn5cellList.getInt("weather_station_id") < 0 || xn5cellList.wasNull() ) {
						out.close(); 
						JOptionPane.showMessageDialog(null, "Error when writing weather history for BEMS: missing weather_station_id for cell " +  xc + ", " + yc);
						throw new Exception("Error when writing weather history for BEMS: missing weather_station_id for cell " +  xc + ", " + yc);
						
					}
					else {
						if (elevationCorrectionType > 0 && elevCorrectionClassSize > -1) {
							out.print(xn5cellList.getString("weather_station_id")  +  String.format("%04d",xn5cellList.getInt("alt")));
						}
						else {
							out.print(xn5cellList.getString("weather_station_id"));
						}
						xn5cellList.next();
					}
					++xc;
					if (xc > xnDims.getInt("maxX")) {
						xc = 0;
						--yc;
						out.print("\n");
					}
					else {
						out.print(" ");
					}
					
				}
				xn5cellList.close();
				xnDims.close();
				out.close();
			}
			projectInfo.close();
			cellList.close();
			if (doCO2)	co2Series.close();
		}	
		catch(Exception e) {
			e.printStackTrace();
			return false;
		}
		ok = true;	
		
		return ok;
	}
	public boolean writeWeatherData(String filename, String tableName, int station_id, int first_year, int last_year, 
			int write_init_history_file, boolean appendHistory, String history_filename, double maxDailyPrecip,  
			int elevationCorrectionType, double altitude, String elevationInfoTable, boolean doCO2, String co2Table) {
		
		boolean ok = false;
		
		try {
			
			ResultSet co2Series = null;
			
			if (doCO2 && co2Table != null && ! co2Table.isEmpty() && !co2Table.matches("^\\s*$") )
			{
				
				if (! co2Table.contains(".")) {
					co2Table = "for1695_weather."+co2Table;
				}
				
				String s2 = "SELECT year, month, day, co2_ppm "
						+ " FROM  " + co2Table 
						+ " 	WHERE year BETWEEN "+ first_year +"  AND "+ last_year +
						"			ORDER BY year, month, day;";
				co2Series = myConnection.query(s2);
				
				if (! co2Series.first() ) {
					JOptionPane.showMessageDialog(null, "Error: No CO2 time series found in table "+ co2Table + ".");
					System.out.println("Error:  No CO2 time series found in table "+ co2Table + ".");
					co2Series.close();
					return false;
				}
					
			}
			else {
				doCO2 = false;
			}
			writeWeatherData(filename, tableName, station_id, first_year, last_year, 
					write_init_history_file, appendHistory, history_filename, maxDailyPrecip,  
					elevationCorrectionType, altitude, elevationInfoTable,  doCO2,  co2Series);
			
		}	
		catch(Exception e) {
			e.printStackTrace();
			return false;
		}
		ok = true;	
		
		return ok;
	}
	
	
	public boolean writeWeatherData(String filename, String tableName, int station_id, int first_year, int last_year, 
					int write_init_history_file, boolean appendHistory, String history_filename, double maxDailyPrecip,  
					int elevationCorrectionType, double altitude, String elevationInfoTable, boolean doCO2, ResultSet co2Series) {

//Note: write_init_history_file 0 = no, 1 = yes, 2 = only history no normal file
		System.out.println("Writing "+filename+"...");

		
		String[] tables =tableName.split(";");

		for (int i = 0; i < tables.length; ++i) {
			if (! checkTableExists("for1695_weather", tables[i])) {
				
				JOptionPane.showMessageDialog(null, "Error: Table '"+tables[i]+"' does not exist in for1695_weather. ");
				System.out.println("Error: Table '"+tables[i]+"' does not exist in for1695_weather. ");
				return false;
				
			}
		}
		try {
			int first_data_year = first_year;
			int last_data_year = last_year;
			if (write_init_history_file > 0) {
				first_data_year = first_year - 6;
			}
			if (write_init_history_file == 2) {
				last_data_year = first_year ;
			}
			
			String s = "";
			
			for (int i = 0; i < tables.length; ++i) {
				
				if (i > 0)
					s+= " UNION ";
				
				s += "SELECT year, month, day, dayofyear(STR_TO_DATE(CONCAT(day,'-',month,'-',year), '%d-%m-%y')) as day_year, avg_temp,maximum_temperature, minimum_temperature, IFNULL(soil_temp_5_cm, -99.9) as soil_temp_5_cm, IFNULL( soil_temp_10_cm, -99.9) as soil_temp_10_cm, IFNULL( soil_temp_20_cm, -99.9) as soil_temp_20_cm," 
						+ " precipitation, avg_wind_speed, relative_humidity, IFNULL(global_radiation, 9999.9) as global_radiation, air_pressure, IFNULL(snow_depth, 9999.9) as snow_depth, IFNULL(sunshine_duration,99.9) as sunshine_duration, IFNULL(dewpoint, -99.9) as dewpoint, IFNULL(pan_evaporation, 9999.9) as pan_evaporation , IFNULL(saturation_deficit_air, 9999.9) as saturation_deficit_air, specific_humidity"
						+ " FROM for1695_weather. " + tables[i]
						+ " 	WHERE station_id = "+station_id+" AND year BETWEEN "+ first_data_year +"  AND "+ last_data_year ;
						
			 
			}
			s+="			ORDER BY year, month, day;";
			ResultSet rs = myConnection.query(s);
/*				
				int doy_correct = 0;
				
				String sx = "SELECT MIN(day_year) as doyMin"
						+ " FROM for1695_weather. " + tableName 
						+ " 	WHERE station_id = "+station_id+" AND year BETWEEN "+ first_data_year +"  AND "+ last_year +
						"			ORDER BY year, month, day;";
				ResultSet rs2 = myConnection.query(sx);
				
				rs2.first();
				if (rs2.getInt("doyMin") == 0) 
						doy_correct = 1;
			*/

			if (! rs.first() ) {
				JOptionPane.showMessageDialog(null, "Error: No weather data found. ");
				System.out.println("Error: No weather data found.");
				rs.close();// rs2.close();
				return false;
			}
			if ( write_init_history_file > 0 && (rs.getInt("year") > first_data_year)  ) {
				JOptionPane.showMessageDialog(null, "Error: Not enough weather data for initial BEMS history found. ");
				System.out.println("Error: Not enough weather data for initial BEMS history found.");
				rs.close();// rs2.close();
				return false;

			}
			rs.last();
			if (  write_init_history_file != 2 && rs.getInt("year") < last_data_year  ) {
				JOptionPane.showMessageDialog(null, "Error: Not enough weather data for simulation run time found. ");
				System.out.println("Error: Not enough weather data for simulation run time found.");
				rs.close(); //rs2.close();
				return false;
			}
			if (  write_init_history_file == 2 && rs.getInt("year") < last_data_year  ) {
				JOptionPane.showMessageDialog(null, "Error: Not enough weather data for history found. ");
				System.out.println("Error: Not enough weather data for history found (end of history too short).");
				rs.close(); //rs2.close();
				return false;
			}
			
			  
			double weatherCellElevation = 0;
			if (elevationCorrectionType > 0) {
				 try {  
					 weatherCellElevation = Integer.parseInt(elevationInfoTable);  
				  } catch(NumberFormatException nfe) {  
				      // Log exception.
				     weatherCellElevation = -9999;
				  }  
				if (weatherCellElevation ==  -9999 ) { 
					
					if (! elevationInfoTable.contains(".")) {
						elevationInfoTable = "for1695_weather."+elevationInfoTable;
					}
					String selev = "SELECT orog FROM " + elevationInfoTable + " WHERE station_id = "+station_id+";"; 
					ResultSet rsElev = myConnection.query(selev);
					
					if (rsElev == null)
					{
						JOptionPane.showMessageDialog(null, "Error: When trying to query "+elevationInfoTable+".");
						System.out.println("Error: When trying to query "+elevationInfoTable+".");
						rsElev.close();
					}
					
					if (! rsElev.first() ) {
						JOptionPane.showMessageDialog(null, "Error: No elevation information found for "+station_id+ " in "+elevationInfoTable+".");
						System.out.println("Error: No elevation information found for "+station_id+ " in "+elevationInfoTable+".");
						rsElev.close();
						return false;
					}
					
					weatherCellElevation = rsElev.getDouble("orog");
				}
			}

			String  co2string = "";
			if (	doCO2		){
				co2string = ",CO2 [ppm]";
				co2Series.beforeFirst();

			}

			
			PrintWriter out = null;
			if (write_init_history_file != 2) {
				out	= new PrintWriter(new BufferedWriter(new FileWriter(filename+".csv")));
				out.println("Date,global radiation [Mj/qm],max temp [°C],min temp [°C],precipitation [mm],sunshine duration [h],rel hum [%],wind speed [m/s],mean temp [°C],dew point [°C],Kesselverdunstung [mm],vapour saturation deficit [%],snow height [mm],par [mol/qm*d],temp soil at 2cm [°C],temp soil at 5cm [°C],temp soil at 10cm [°C],temp soil at 20cm [°C],temp soil at 50cm [°C]" + co2string);
			}
			PrintWriter out2 = null;
			if ( write_init_history_file > 0 && ! appendHistory && history_filename.compareToIgnoreCase("") != 0) {
				out2 = new PrintWriter(new BufferedWriter(new FileWriter(history_filename)));
				out2.println("year\tdayofyear\tstation\tairtemp\tsoiltemp");
			}
			else if ( write_init_history_file >0 &&  appendHistory && history_filename.compareToIgnoreCase("") != 0) {
				out2 = new PrintWriter(new BufferedWriter(new FileWriter(history_filename, true)));
				
			}
			else if ( write_init_history_file > 0 && ! appendHistory && history_filename.compareToIgnoreCase("") == 0 ) {
				out2 = new PrintWriter(new BufferedWriter(new FileWriter(filename+"_init_history.txt")));
				out2.println("year\tdayofyear\tstation\tairtemp\tsoiltemp");
			}
			else if ( write_init_history_file > 0 &&  appendHistory && history_filename.compareToIgnoreCase("") == 0 ) {
				out2 = new PrintWriter(new BufferedWriter(new FileWriter(filename+"_init_history.txt", true)));
				out2.println("year\tdayofyear\tstation\tairtemp\tsoiltemp");
			}
			
			rs.beforeFirst();
			double precipStillToWrite = 0.0;
			
			while (rs.next()) {
				if (write_init_history_file != 2 && rs.getInt("year") >= first_year) {
					out.write(String.format("%04d", rs.getInt("year"))+"-"+ String.format("%02d", rs.getInt("month"))+"-"+String.format("%02d", rs.getInt("day"))); //date
					out.write(", ");
					out.write(Double.toString( rs.getDouble("global_radiation")));
					out.write(", ");
					
					
					double tempMax = rs.getDouble("maximum_temperature");
					double tempMin = rs.getDouble("minimum_temperature");
					double relHum =  rs.getDouble("relative_humidity");
					double tempAvg = rs.getDouble("avg_temp");
					
					switch (elevationCorrectionType) {
					
					
						case 0:
							break;
						case 1: { 
							
							double obsSpecHum = rs.getDouble("specific_humidity");
							double obsPressure = rs.getDouble("air_pressure");
							if (obsSpecHum > 9998 || obsSpecHum < 0.0)
							{
								
								System.out.println("Specific humidity not given, backcalculation from given RH: ");
							
								double tempAvgK1 = tempAvg + 273.16;
								if (tempAvg >= 0) {
									obsSpecHum = relHum / (100* 0.263 *  obsPressure * Math.pow( Math.exp(17.67 * (tempAvgK1 -273.16)/ (tempAvgK1 -29.65)),-1) );
								}//Note: Multiplied by 100 because of unit of pressure  used
								else { 
									obsSpecHum = relHum/ (100* 0.263 * obsPressure * Math.pow(Math.exp( 22.46 * (tempAvgK1-273.16)/(tempAvgK1 -273.16+272.62)) ,-1.0));
								}
										
							}
							
							
							
							double tempdiff = (weatherCellElevation - altitude)/100 * 0.7;
							tempMax = tempMax + tempdiff;
							tempMin = tempMin + tempdiff;
							tempAvg = tempAvg + tempdiff;
							
							
							
							double corrPressure = obsPressure + (weatherCellElevation - altitude)/ 8 * 0.125;
							
							double tempAvgK = tempAvg + 273.16;
							if (tempAvg >= 0) {
								relHum = 100* 0.263 * obsSpecHum * corrPressure * Math.pow( Math.exp(17.67 * (tempAvgK -273.16)/ (tempAvgK -29.65)),-1);
							}//Note: Multiplied by 100 because of unit of pressure  used
							else { 
								relHum = 100* 0.263 * obsSpecHum * corrPressure * Math.pow(Math.exp( 22.46 * (tempAvgK-273.16)/(tempAvgK -273.16+272.62)) ,-1.0);
							}
							relHum = (relHum >=  100.0) ? 100.0 : relHum;   
							
							
						}
						break;
						
						default:
							break;
					}
					
					
					
					out.write(Double.toString( tempMax));
					out.write(", ");
					out.write(Double.toString( tempMin));
					out.write(", ");
					
					double precip = rs.getDouble("precipitation") + precipStillToWrite;
					if ( precip <= maxDailyPrecip) {
						precipStillToWrite = 0.0;
					}
					else {
						precipStillToWrite = precip - maxDailyPrecip;
						precip = maxDailyPrecip;
					}
					out.write(Double.toString( precip));
					out.write(", ");
					out.write(Double.toString( rs.getDouble("sunshine_duration")));
					out.write(", ");
					out.write(Double.toString(relHum)); 
					out.write(", ");
					out.write(Double.toString( rs.getDouble("avg_wind_speed")));
					out.write(", ");
					out.write(Double.toString( tempAvg));
					out.write(", ");
					out.write(Double.toString( rs.getDouble("dewpoint"))); 
					out.write(", ");
					out.write(Double.toString( rs.getDouble("pan_evaporation")));
					out.write(", ");
					out.write(Double.toString( rs.getDouble("saturation_deficit_air")));
					out.write(", ");
					out.write(Double.toString( rs.getDouble("snow_depth")));
					out.write(", ");
					out.write("99.9");//par
					out.write(", ");
					out.write("-99.9");//soil_temp_2_cm
					out.write(", ");
					out.write(Double.toString( rs.getDouble("soil_temp_5_cm")));
					out.write(", ");
					out.write(Double.toString( rs.getDouble("soil_temp_10_cm")));
					out.write(", ");
					out.write(Double.toString( rs.getDouble("soil_temp_20_cm")));
					out.write(", ");
					out.write("-99.9");//soil_temp_50_cm
					
					if(doCO2) {
						if ( !co2Series.next() ) {
							JOptionPane.showMessageDialog(null, "Error: Not enough CO2 values found for weather time series.");
							System.out.println("Error: Not enough CO2 values found for weather time series.");
							co2Series.close();
							rs.close();
							return false;
							
							
						}
						while (co2Series.getInt("year") != rs.getInt("year")
							|| co2Series.getInt("month") != rs.getInt("month")
							|| co2Series.getInt("day") != rs.getInt("day")
								
							)
						{
							if ( !co2Series.next() ) {
								JOptionPane.showMessageDialog(null, "Error: Missing CO2 value for "+rs.getString("year")+"-"+rs.getString("month")+"-"+rs.getString("day")+".");
								System.out.println("Error: Missing CO2 value for "+rs.getString("year")+"-"+rs.getString("month")+"-"+rs.getString("day")+".");
								co2Series.close();
								rs.close();
								return false;
							}
						}
						out.write(", ");
						out.write(Double.toString( co2Series.getDouble("co2_ppm")));// CO2 ppm
					}
					
					out.write("\n");
				}
				if ( (write_init_history_file > 0)  &&  (rs.getInt("year") <= first_year) ) {
					
					double tempAvg = rs.getDouble("avg_temp");

					switch (elevationCorrectionType) {
						case 0:
							break;
						case 1: { 
							double tempdiff = (weatherCellElevation - altitude)/100 * 0.7;
							tempAvg = tempAvg + tempdiff;
						}
							break;

						default:
							break;
					
					}
						
					out2.write(String.format("%04d", rs.getInt("year"))+"\t"+ String.format("%d", rs.getInt("day_year") ));
					out2.write(tab);
					
					if (elevationCorrectionType > 0 ) {
						out2.write(String.valueOf(station_id) + String.format("%04d", Math.round( altitude) ) );
					}
					else {
						out2.write(String.valueOf(station_id) );
					}
			
					
					out2.write(tab);
					out2.write(Double.toString( tempAvg));
					out2.write(tab);
					double soiltemp = rs.getDouble("soil_temp_5_cm");
					if ( soiltemp < -98.9)
						soiltemp = tempAvg;
					
					out2.write(Double.toString(soiltemp ));

					out2.write("\n");

				}
				
			}
			rs.close(); //rs2.close();
			if ( write_init_history_file != 2) {
				out.close();
			}
			if ( write_init_history_file > 0) {
				out2.close();
			}
			
			
		}
		catch(Exception e) {
			e.printStackTrace();
			return false;
		}
		return true;	

	}
	public boolean checkTableExists(String db, String tableName) {
		try {	
			String s = "SELECT * FROM information_schema.TABLES "
					  + " WHERE TABLE_SCHEMA = '"+db+"'"
					  + "	AND TABLE_NAME = '"+tableName+"';";
			ResultSet rs = myConnection.query(s);
			boolean b = rs.first();
			rs.close();
			return b;
		}
		catch (Exception e) {
			return false;
		}
	}
	public boolean checkTableIsNotView(String db, String tableName) {
		try {	
			String s = "SELECT TABLE_TYPE FROM information_schema.TABLES "
					  + " WHERE TABLE_SCHEMA = '"+db+"'"
					  + "	AND TABLE_NAME = '"+tableName+"';";
			ResultSet rs = myConnection.query(s);
			boolean b = false;
			if(rs.first() && rs.getString("TABLE_TYPE").equals("BASE TABLE") ) {
				b = true;
			}
			rs.close();
			return b;
		}
		catch (Exception e) {
			return false;
		}
	}
	
	
	
	//UTILITY FUNCTIONS
	public static boolean isNumeric(String str)
	{
	  return str.matches("-?\\d+(\\.\\d+)?");  
	}
	public Connection getConnectionObject() {
		return myConnection.getConnectionObject();
	}
}




class cropInfo {
	public String crop_code;
	public String variety;
	public int sowYear;
	
	
	public cropInfo(String _code, String _variety, int _y) {
		crop_code = _code;
		variety = _variety;
		sowYear = _y;
	}
}

