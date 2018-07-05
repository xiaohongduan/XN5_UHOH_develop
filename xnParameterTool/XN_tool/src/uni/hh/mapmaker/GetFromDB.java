package uni.hh.mapmaker;

import java.sql.Date;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.io.File;
//import java.util.Calendar;

import uni.hh.mysql_db.DbCommands;
import uni.hh.mysql_db.DbConn;

public class GetFromDB {
	DbConn dbconn;
	DbCommands cmd;
	String mpmasDb;
	String cropsDb;
	
	public GetFromDB (String configfile, String _cropsDb, String _mpmasDb){
		try {
			File f = new File(configfile);
			if(! f.isFile()) {
		    	System.err.println("Error: " + configfile + " does not exist!");
				System.exit(1);
			}
			dbconn = new DbConn(
				configfile, 
				_cropsDb, 
				"get data from database");
			cmd = new DbCommands(dbconn);
			mpmasDb = _mpmasDb;
			cropsDb = _cropsDb;
		}
		catch (Exception e) {
	    	System.err.println("Error: " + e.getMessage());
			System.exit(1);
		}
		
	}
//	DbConn dbconn = new DbConn(dbconfigfile, dbname, " making a map");
	
//******************************FROM HERE STARTS THE XND FILE**************************************************	
	

public String getStartDate3(int crop_id, int seed_choice){
	//CT 110615 made dependent on crop
	try {

		String sql = "SELECT SimStartDate FROM `" + cropsDb + "`.`tbl_crop_characteristics` " +
				" WHERE crop_id = " + crop_id
				+ " AND seed_choice = " + seed_choice;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
	
}

public String getEndDate3(int crop_id, int seed_choice){
	//CT 110615 made dependent on crop
	try {

		String sql = "SELECT SimEndDate FROM `" + cropsDb + "`.`tbl_crop_characteristics` " +
				"WHERE crop_id = " + crop_id
				+ " AND seed_choice = " + seed_choice;
		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
	
public String getFarmId(){
	try {

		String sql = "SELECT t.`farm_id`" 
			+ "FROM " + cropsDb + ".tbl_farm t WHERE t.`farm_id`=1";

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}

public String getNoPlots(){
	try {
		String sql = "SELECT t.`num_plots`" 
			+ "FROM " + cropsDb + ".tbl_farm t where t.`farm_id`=1";

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}

public String getLatitude(){
	try {
		String sql = "SELECT t.`latitude`" 
			+ "FROM " + cropsDb + ".tbl_farm t where t.`farm_id`=1";

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}

public String getLongitude(){
	try {
		String sql = "SELECT t.`longitude`" 
			+ "FROM " + cropsDb + ".tbl_farm t where t.`farm_id`=1";

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getAltitude(){
	try {
		String sql = "SELECT t.`altitude`" 
			+ "FROM " + cropsDb + ".tbl_farm t where t.`farm_id`=1";

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getlast_name(){
	try {
		String sql = "SELECT t.`last_name`" 
			+ "FROM " + cropsDb + ".tbl_farm t where t.`farm_id`=1";

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getFirst_name(){
	try {
		String sql = "SELECT t.`first_name`" 
			+ "FROM " + cropsDb + ".tbl_farm t where t.`farm_id`=1";

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getAddress(){
	try {
		String sql = "SELECT t.`address`" 
			+ "FROM " + cropsDb + ".tbl_farm t where t.`farm_id`=1";

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getPlz(){
	try {
		String sql = "SELECT t.`plz`" 
			+ "FROM " + cropsDb + ".tbl_farm t where t.`farm_id`=1";

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getcity(){
	try {
		String sql = "SELECT t.`city`" 
			+ "FROM " + cropsDb + ".tbl_farm t where t.`farm_id`=1";

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getTel(){
	try {
		String sql = "SELECT t.`tel`" 
			+ "FROM " + cropsDb + ".tbl_farm t where t.`farm_id`=1";

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getFax(){
	try {
		String sql = "SELECT t.`fax`" 
			+ "FROM " + cropsDb + ".tbl_farm t where t.`farm_id`=1";

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}


public String getPlotID(){
	try {
		String sql = "SELECT t.`plot_id`" 
			+ "FROM " + cropsDb + ".tbl_plot t where t.`plot_id`=1";

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getPlot_name(){
	try {
		String sql = "SELECT t.`plot_name`" 
			+ "FROM " + cropsDb + ".tbl_plot t where t.`plot_id`=1";

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getPlot_size(){
	try {
		String sql = "SELECT t.`plot_size`" 
			+ "FROM " + cropsDb + ".tbl_plot t where t.`plot_id`=1";

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getnum_subplots(){
	try {
		String sql = "SELECT t.`num_subplots`" 
			+ "FROM " + cropsDb + ".tbl_plot t where t.`plot_id`=1";

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}


public String getSubPlotID(int id){
	return "1";
	/*try {
		String sql = "SELECT t.`plot_id`" 
			+ "FROM pak_crops.tbl_subplot t where t.`id`=" + id ;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}*/
}


public int getPak_crops_crop_id_by_ktblmilp_process_id(int process_id){
	try {
		String sql = "SELECT t2.`pak_crops_crop_id`" 
			+ "FROM " +  mpmasDb + ".tbl_xp_process t1 JOIN " + cropsDb + ".tbl_crop_linkup_ktblmilp t2 ON t1.crop_id = t2.ktblmilp_goods_id" 
			+ " WHERE t1.`process_id`= " + process_id;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getInt(1);
		}
		return -99;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return -99;
	}
}
public int getPak_crops_seed_choice_by_ktblmilp_process_id (int process_id){
	try {
		String sql = "SELECT t2.`pak_crops_seed_choice`" 
			+ "FROM " +  mpmasDb + ".tbl_xp_process t1 JOIN " + cropsDb + ".tbl_crop_linkup_ktblmilp t2 ON t1.crop_id = t2.ktblmilp_goods_id" 
			+ " WHERE t1.`process_id`= " + process_id;
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getInt(1);
		}
		return -99;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return -99;
	}
}

public String getPrinc_Crop(int crop_id){
	try {
		String sql = "SELECT t3.`crop_name_german`" 
			+ "FROM  " + cropsDb + ".tbl_crop t3 WHERE t3.`crop_id`=" + crop_id;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getWeatherfile(int id){
	try {
		String sql = "SELECT t.`filename`" 
			+ "FROM " + cropsDb + ".tbl_weatherfiles t WHERE t.`weather_id`= "+ id;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public int getStationForWeatherfile(int id){
	try {
		String sql = "SELECT t.`station_id`" 
			+ "FROM " + cropsDb + ".tbl_weatherfiles t WHERE t.`weather_id`= "+ id;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getInt(1);
		}
		return -1;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return -1;
	}
}
public int getYearForWeatherfile(int id){
	try {
		String sql = "SELECT t.`year`" 
			+ "FROM " + cropsDb + ".tbl_weatherfiles t WHERE t.`weather_id`= "+ id;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getInt(1);
		}
		return -1;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return -1;
	}
}
public String getParameterfile(int soil_id){
	try {
		String sql = "SELECT parameterfile FROM " + cropsDb + ".tbl_paksoils t WHERE t.`pak_soiltype_id`=" + soil_id;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getParameterfile(int soil_id, int crop_id){
	try {
		String sql = "SELECT xnm_file FROM " + cropsDb + ".tbl_gecros_xn_config t WHERE t.`pak_soiltype_id`=" 
			+ soil_id + " AND crop_id = " + crop_id;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getParameterfile(int soil_id, int crop_id, int xn_config){
	try {
		String sql = "SELECT xnm_file FROM " + cropsDb + ".tbl_xnm_by_xn_config t WHERE t.`pak_soiltype_id`=" 
			+ soil_id + " AND crop_id = " + crop_id + " AND xn_config = " + xn_config;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getXnConfigfile(int xn_config){
	try {
		String sql = "SELECT config_xnc FROM " + cropsDb + ".tbl_xn_configurations t WHERE t.`config_id`= " + xn_config;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public ResultSet get_xnd_files_to_create(String table) {
	try{
		String sql = "SELECT filename, resultname as resultID, process_id as lu_id, soil_id, weather_id, config_id " 
			+ " FROM " + cropsDb + "." + table;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		return rs;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}

public String getConfigFile(int id){
	try {
		String sql = "SELECT t.`configfile`" 
			+ "FROM " + cropsDb + ".tbl_subplot t WHERE t.`id`=" + id;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getAreaNum(int id){
	try {
		String sql = "SELECT t.`area_number`" 
			+ "FROM " + cropsDb + ".tbl_subplot t WHERE t.`id`=" + id;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getsubplotsize(int id){
	try {
		String sql = "SELECT t.`size`" 
			+ "FROM " + cropsDb + ".tbl_subplot t WHERE t.`id`=" + id;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getsubplotslope(int id){
	try {
		String sql = "SELECT t.`slope`" 
			+ "FROM " + cropsDb + ".tbl_subplot t WHERE t.`id`=" + id;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getsubplotExpo(int id){
	try {
		String sql = "SELECT t.`exposition_id`" 
			+ "FROM " + cropsDb + ".tbl_subplot t WHERE t.`id`=" + id;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}

public String getcrop_preceding(int sim_id){
	try {
		String sql = "SELECT t.`crop_name`" 
			+ "FROM " + cropsDb + ".tbl_preceding_crop t WHERE t.`sim_id`=" + sim_id;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			
			
			String s = rs.getString(1);
			if(s==null){
				return "-99"; 
			}
			return s;
		}
		return "";
		
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getyield_preceding(int sim_id){
	try {
		String sql = "SELECT t.`yield_preceding_crop`" 
			+ "FROM " + cropsDb + ".tbl_preceding_crop t WHERE t.`sim_id`=" + sim_id;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			String s= rs.getString(1);
			if(s==null){
				return "-99"; 
			}
			return s;
		
		}
		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getharvestDate_preceding(int sim_id){
	try {
		String sql = "SELECT t.`harvest_date`" + 
		"FROM " + cropsDb + ".tbl_preceding_crop t WHERE t.`sim_id`=" + sim_id;
		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			
			Date d = rs.getDate(1);
			//System.out.println(d);
						if(d==null){
							return "-99";
						}
			
			
			//System.out.println(d);
			int dy = d.getDate();
			int m = d.getMonth() + 1;
			//System.out.println(dy);
			
			String day="", month=""; 
			if (dy<10){
				 day = "0"+Integer.toString(dy);
			}
			else {
				 day = Integer.toString(dy);
			}
			
			

			if (m<10){
				 month = "0"+Integer.toString(m);
			}
			else {
				 month = Integer.toString(m);
			}

			
			int yr = d.getYear();
			String startdate = day+month+ (Integer.toString(yr)).substring(1,3);
			return startdate;
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getamount_aboveground_residues_preceding(int sim_id){
	try {
		String sql = "SELECT t.`amount_aboveground_residues`" 
			+ "FROM " + cropsDb + ".tbl_preceding_crop t WHERE t.`sim_id`=" + sim_id;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			
			String s= rs.getString(1);
			if(s==null){
				return "-99"; 
			}
				return s;
		
		}
		
		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String get_c_n_aboveground_residues_preceding(int sim_id){
	try {
		String sql = "SELECT t.`c_n_aboveground_residues`" 
			+ "FROM " + cropsDb + ".tbl_preceding_crop t WHERE t.`sim_id`=" + sim_id;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			
			String s= rs.getString(1);
			if(s==null){
				return "-99"; 
			}

				return s;

		}
		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getroot_residues_weight_preceding(int sim_id){
	try {
		String sql = "SELECT t.`root_residues_weight`" 
			+ "FROM " + cropsDb + ".tbl_preceding_crop t WHERE t.`sim_id`=" + sim_id;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			String s= rs.getString(1);
			if(s==null){
				return "-99"; 
			}

				return s;

		}
		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String get_c_n_root_residues_preceding(int sim_id){
	try {
		String sql = "SELECT t.`c_n_root_residues`" 
			+ "FROM " + cropsDb + ".tbl_preceding_crop t WHERE t.`sim_id`=" + sim_id;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			String s= rs.getString(1);
			if(s==null){
				return "-99"; 
			}
				return s;
		}
		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}

public int getnumber_cultivated_crops(int id){
	try {
		String sql = "SELECT count(*) "  
			+ "FROM " + cropsDb + ".tbl_cultivated_crop t WHERE t.`id`=" + id;
			
		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getInt(1);
		}
		return -99;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return -99;
	}
}

public ResultSet get_intermediate_crop(int process_id) {
	try {
		String sql = "SELECT t2.tillharvest_phase_id, goods_id, quantity, xn_day, xn_month, xn_year  FROM " +  mpmasDb + ".tbl_xp_process t1 join " +  mpmasDb + ".tbl_xp_crop_tillharvest t2 on t1.crop_id = t2.crop_id and t1.tillharvest_id = t2.tillharvest_id where process_id = "+ process_id +" and t2.tillharvest_phase_id = 10";
			

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		return rs;
	}
	catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
	return null;
}
}
public ResultSet get_intermediate_crop_harvest(int process_id) {
	try {
		String sql = "SELECT t2.tillharvest_phase_id, goods_id, quantity, xn_day, xn_month, xn_year  " +
				"FROM " +  mpmasDb + ".tbl_xp_process t1 " +
				"JOIN " +  mpmasDb + ".tbl_xp_crop_tillharvest t2 " +
				"ON t1.crop_id = t2.crop_id AND t1.tillharvest_id = t2.tillharvest_id where process_id = "+ process_id +" " +
				"AND t2.tillharvest_phase_id in (3,4)";
			

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		return rs;
	}
	catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
	return null;
}
}


public ResultSet get_intermediate_crop_characteristics_by_goods_id (int goods_id) {
	try {
		String sql = "SELECT xn_code, variety, seeding_depth, row_spacing, seed_density, max_root_development_depth, removal_above_ground, crop_name_english  " +
				"FROM " + cropsDb + ".tbl_crop_linkup_ktblmilp t1 " +
				"JOIN " + cropsDb + ".tbl_crop t2 ON t1.pak_crops_crop_id = t2.crop_id " +
				"JOIN " + cropsDb + ".tbl_crop_characteristics t3 ON t2.crop_id = t3.crop_id AND t1.pak_crops_seed_choice = t3.seed_choice"  +
				"WHERE ktblmilp_goods_id = " + goods_id;
			

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		return rs;
	}
	catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
	return null;
}
}
public ResultSet get_cultivated_crop_characteristics_by_crop_id (int crop_id, int seed_choice) {
	try {
		String sql = "SELECT xn_code, variety, seeding_depth, row_spacing, seed_density, max_root_development_depth, removal_above_ground, crop_name_english  "
					+ "FROM  " + cropsDb + ".tbl_crop t2 JOIN " + cropsDb + ".tbl_crop_characteristics t3 "
					+ "ON t2.crop_id = t3.crop_id "
					+ "WHERE t2.crop_id  = " + crop_id
					+ " AND t3.seed_choice = " + seed_choice;
			

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		return rs;
	}
	catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
	return null;
}
}
public ResultSet get_cultivated_crop_harvestdate(int process_id) {
	try {
		String sql = "SELECT  xn_day, xn_month, xn_year  " +
				"FROM " +  mpmasDb + ".tbl_xp_process t1 " +
				"JOIN " +  mpmasDb + ".tbl_xp_crop_tillharvest t2 on t1.crop_id = t2.crop_id " +
				"AND t1.tillharvest_id = t2.tillharvest_id " +
				"WHERE process_id = "+ process_id +" AND t2.tillharvest_phase_id = 7 ";
			

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		return rs;
	}
	catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
	return null;
}
}
public ResultSet get_cultivated_crop_seeddate(int process_id) {
	try {
		String sql = "SELECT  xn_day, xn_month, xn_year  " +
				"FROM " +  mpmasDb + ".tbl_xp_process t1 JOIN " +  mpmasDb + ".tbl_xp_crop_tillharvest t2 " +
				"ON t1.crop_id = t2.crop_id AND t1.tillharvest_id = t2.tillharvest_id " +
				"WHERE process_id = "+ process_id +" AND t2.tillharvest_phase_id = 6 ";
			

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		return rs;
	}
	catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
	return null;
}
}

public String getCropcode_cultivated(int id){
	try {
		String sql = "SELECT t.`xn_code`" 
			+ "FROM " + cropsDb + ".tbl_cultivated_crop t WHERE t.`id`=" + id;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getGenotype_cultivated(int id){
	try {
		String sql = "SELECT t.`genotype`" 
			+ "FROM " + cropsDb + ".tbl_cultivated_crop t WHERE t.`id`=" + id;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public int getGenotypeForCrop(int crop_id){
	try {
		String sql = "SELECT t.`pak_crops_genotype_id`" 
			+ "FROM " + cropsDb + ".tbl_crop_linkup_ktblmilp t WHERE t.`pak_crops_crop_id`=" + crop_id;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getInt(1);
		}
		return -1;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return -1;
	}
}
public String getSeeding_depth(int id){
	try {
		String sql = "SELECT t.`seeding_depth`" 
			+ "FROM " + cropsDb + ".tbl_cultivated_crop t WHERE t.`id`=" + id;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getRow_space(int id){
	try {
		String sql = "SELECT t.`row_spacing`" 
			+ "FROM " + cropsDb + ".tbl_cultivated_crop t WHERE t.`id`=" + id;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getSeed_density(int id){
	try {
		String sql = "SELECT t.`seed_density`" 
			+ "FROM " + cropsDb + ".tbl_cultivated_crop t WHERE t.`id`=" + id;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getplanting_date_cultivated(int id){
	try {
		String sql = "SELECT t.`planting_date`" + 
		"FROM " + cropsDb + ".tbl_cultivated_crop t WHERE t.`id`=" + id;
		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			Date d = rs.getDate(1);
			//System.out.println(d);
			int dy = d.getDate();
			int m = d.getMonth() + 1;
			//System.out.println(dy);
			
			String day="", month=""; 
			if (dy<10){
				 day = "0"+Integer.toString(dy);
			}
			else {
				 day = Integer.toString(dy);
			}
			
			

			if (m<10){
				 month = "0"+Integer.toString(m);
			}
			else {
				 month = Integer.toString(m);
			}

			
			int yr = d.getYear();
			String startdate = day+month+ (Integer.toString(yr)).substring(1,3);
			return startdate;
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getEmergence_date_cultivated(int id){
	try {
		String sql = "SELECT t.`emergence_date`" + 
		"FROM " + cropsDb + ".tbl_cultivated_crop t WHERE t.`id`=" + id;
		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			
			Date d = rs.getDate(1);
			if (d == null){
				return "-99";
			}
			
			//System.out.println(d);
			int dy = d.getDate();
			int m = d.getMonth() + 1;
			//System.out.println(dy);
			
			String day="", month=""; 
			if (dy<10){
				 day = "0"+Integer.toString(dy);
			}
			else {
				 day = Integer.toString(dy);
			}
			
			

			if (m<10){
				 month = "0"+Integer.toString(m);
			}
			else {
				 month = Integer.toString(m);
			}

			
			int yr = d.getYear();
			String startdate = day+month+ (Integer.toString(yr)).substring(1,3);
			return startdate;
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getHarvest_date_cultivated(int id){
	try {
		String sql = "SELECT t.`harvest_date`" + 
		"FROM " + cropsDb + ".tbl_cultivated_crop t WHERE t.`id`=" + id;
		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			Date d = rs.getDate(1);
			
			if (d == null){
				return "-99";
			}
			//System.out.println(d);
			int dy = d.getDate();
			int m = d.getMonth() + 1;
			//System.out.println(dy);
			
			String day="", month=""; 
			if (dy<10){
				 day = "0"+Integer.toString(dy);
			}
			else {
				 day = Integer.toString(dy);
			}
			
			

			if (m<10){
				 month = "0"+Integer.toString(m);
			}
			else {
				 month = Integer.toString(m);
			}

			
			int yr = d.getYear();
			String startdate = day+month+ (Integer.toString(yr)).substring(1,3);
			return startdate;
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getMax_Bio_date_cultivated(int id){
	try {
		String sql = "SELECT t.`max_biomass_date`" + 
		"FROM " + cropsDb + ".tbl_cultivated_crop t WHERE t.`id`=" + id;
		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			
						Date d = rs.getDate(1);
			//System.out.println(d);
						if(d==null){
							return "-99";
						}
							

						int dy = d.getDate();
			int m = d.getMonth() + 1;
			//System.out.println(dy);
			
			String day="", month=""; 
			if (dy<10){
				 day = "0"+Integer.toString(dy);
			}
			else {
				 day = Integer.toString(dy);
			}
			
			

			if (m<10){
				 month = "0"+Integer.toString(m);
			}
			else {
				 month = Integer.toString(m);
			}

			
			int yr = d.getYear();
			String startdate = day+month+ (Integer.toString(yr)).substring(1,3);
			return startdate;
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getMax_root_date_cultivated(int id){
	try {
		String sql = "SELECT t.`max_root_develop_date`" + 
		"FROM " + cropsDb + ".tbl_cultivated_crop t WHERE t.`id`=" + id;
		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			
			Date d = rs.getDate(1);
			if (d == null){
				return "-99";
			}
			//System.out.println(d);
			int dy = d.getDate();
			int m = d.getMonth() + 1;
			//System.out.println(dy);
			
			String day="", month=""; 
			if (dy<10){
				 day = "0"+Integer.toString(dy);
			}
			else {
				 day = Integer.toString(dy);
			}
			
			

			if (m<10){
				 month = "0"+Integer.toString(m);
			}
			else {
				 month = Integer.toString(m);
			}

			
			int yr = d.getYear();
			String startdate = day+month+ (Integer.toString(yr)).substring(1,3);
			return startdate;
		}//else
			return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getMax_root_depth_cultivated(int id){
	try {
		String sql = "SELECT t.`max_root_develop_depth`" 
			+ "FROM " + cropsDb + ".tbl_cultivated_crop t WHERE t.`id`=" + id;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getMax_removal_cultivated(int id){
	try {
		String sql = "SELECT t.`removal_above_ground`" 
			+ "FROM " + cropsDb + ".tbl_cultivated_crop t WHERE t.`id`=" + id;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}



public ResultSet get_min_fertilizations(int process_id) {
	try {
		String sql = "SELECT `process_id`, t3.fertilizer_mineral_id, t4.fertilizer_mineral_name, t4.fertilizer_mineral_code, round(t4.total_n_content * t2.quantity) as total_n, round(t4.nitrate_n_content *t2.quantity) as nitrate_n, round(t4.ammonium_n_content*t2.quantity) as ammonium_n, round(t4.urea_n_content*t2.quantity) as urea_n,`xn_day`,`xn_month`,`xn_year`"
		       + " FROM " +  mpmasDb + ".tbl_xp_process t1 left join " +  mpmasDb + ".tbl_xp_crop_fertilization t2 on t1.crop_id = t2.crop_id	and t1.fertilization_id=t2.fertilization_practice_id join " + cropsDb + ".tbl_fertilizer_linkup_ktblmilp t3 on t2.fertilizer_id = t3.ktblmilp_goods_id join " + cropsDb + ".tbl_fertilizer_mineral t4 on t3.fertilizer_mineral_id = t4.fertilizer_mineral_id"
		       + " where t3.fertilizer_mineral_id > 0 and process_id = " + process_id +" order by xn_year, xn_month, xn_day";
		
		ResultSet rs = cmd.runQuery(sql);
		return rs;
		
	}catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
	
	
}

public int getnumber_min_fertilizations(int processid){// Number of Mineral Fertilizer Applications.
	try {
		String sql = "SELECT * "  
			+ "FROM " + cropsDb + ".tbl_fertilizer_min_per_subplot t WHERE t.`process_id` =" + processid;
		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getInt(1);
		}
		return -99;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return -99;
	}
}
public String getMineral_fert_date(int processid, int entry){ //MINERAL FERTILIZATION DATE
	try {
		int entry1; 
		int entry2;
		if (entry == 1) {
			entry1 = 0;
			entry2 = 1;
		}
		else {
			entry1 = entry -1;
			entry2 = entry -1;
			
		}
		String sql = "SELECT t.`date_min_fertilization`" + 
		" FROM " + cropsDb + ".tbl_fertilizer_min_per_subplot t WHERE t.`process_id` =" + processid + " Order by t.`date_min_fertilization` Limit " + entry1 + "," + entry2 ;  

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			Date d = rs.getDate(1);
			//System.out.println(d);
			int dy = d.getDate();
			int m = d.getMonth() + 1;
			//System.out.println(dy);
			
			String day="", month=""; 
			if (dy<10){
				 day = "0"+Integer.toString(dy);
			}
			else {
				 day = Integer.toString(dy);
			}
			
			

			if (m<10){
				 month = "0"+Integer.toString(m);
			}
			else {
				 month = Integer.toString(m);
			}

			
			int yr = d.getYear();
			String startdate = day+month+ (Integer.toString(yr)).substring(1,3);
			return startdate;
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getMineral_fert_name(int processid){
	try {
		String sql = "SELECT t1.`fertilizer_mineral_name`" + //FERTILIZER NAME
		" FROM " + cropsDb + ".tbl_fertilizer_mineral t1 left join " + cropsDb + ".tbl_fertilizer_min_per_subplot t2"
		+ " on t1.fertilizer_mineral_id= t2.fertilizer_mineral_id where t2.process_id=" + processid ;
		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getMineral_fert_code(int processid){
	try {
		String sql = "SELECT t1.`fertilizer_mineral_code`" + //FERTILIZER CODE
		" FROM " + cropsDb + ".tbl_fertilizer_mineral t1 left join " + cropsDb + ".tbl_fertilizer_min_per_subplot t2"
		+ " on t1.fertilizer_mineral_id= t2.fertilizer_mineral_id where t2.process_id=" + processid;
		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getMineral_fert_N_content(int processid){//NITROGEN FERTILIZER CONTENT
	try {
		String sql = "SELECT ROUND(t.`fertilizer_amount`* t.`total_n_content`) " 
			+ "FROM " + cropsDb + ".tbl_fertilizer_min_per_subplot t WHERE t.`process_id`=" + processid;
		
		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getMineral_fert_nitrate_n_content(int processid){
	try {
		String sql = "select ROUND(t.`fertilizer_amount`* t.`nitrat_n_content`)" //NITRATE FERTILIZER CONTENT
			+ "FROM " + cropsDb + ".tbl_fertilizer_min_per_subplot t WHERE t.`process_id`=" + processid;
		
		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getMineral_fert_ammonium_n_content(int processid){
	try {
		String sql = "SELECT ROUND(t.`fertilizer_amount`* t.`ammonium_n_content`)" //AMMONIUM FERTILIZER CONTENT
			+ "FROM " + cropsDb + ".tbl_fertilizer_min_per_subplot t WHERE t.`process_id`=" + processid;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getMineral_fert_urea_n_content(int processid){
	try {
		String sql = "SELECT ROUND(t.`fertilizer_amount`*t.`urea_n_content`)" //UREA FERTILIZER CONTENT
			+ "FROM " + cropsDb + ".tbl_fertilizer_min_per_subplot t WHERE t.`process_id`=" + processid;;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}

public ResultSet getnumber_organic_fertilizations(int process_id){// Number of Organic Fertilizer Applications.
	try {
		String sql = "SELECT `process_id`, t3.fertilizer_organic_id, t4.fertilizer_organic_german_name, t2.quantity as amount, t4.fertilizer_organic_code, round(t4.dry_matter_content * t2.quantity * 10) as TS, round(t4.total_n_content *t2.quantity *10) as total_n, round(t4.ammonium_n_content*t2.quantity *10) as ammonium_n, round(t4.amount_org_substance*t2.quantity) as org_sub,`xn_day`,`xn_month`,`xn_year`" 
			+ " FROM " +  mpmasDb + ".tbl_xp_process t1 left join " +  mpmasDb + ".tbl_xp_crop_fertilization t2 on t1.crop_id = t2.crop_id  and t1.fertilization_id=t2.fertilization_practice_id join " + cropsDb + ".tbl_fertilizer_linkup_ktblmilp t3 on t2.fertilizer_id = t3.ktblmilp_goods_id join " + cropsDb + ".tbl_fertilizer_organic t4 on t3.fertilizer_organic_id = t4.fertilizer_organic_id"
			+ " where t3.fertilizer_organic_id > 0 and process_id = " + process_id +" order by xn_year, xn_month, xn_day";
		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
			return rs;
		
		
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}  

public ResultSet getnumber_soilmanagement_activities(int process_id){ // Soil Management Activities
	try {
		String sql = "SELECT process_id, xn_day, xn_month, xn_year, soil_management_equipment_code, soil_management_equipment_german, soil_management_equipment_depth" 
		+ " FROM " +  mpmasDb + ".tbl_xp_process t1 join " +  mpmasDb + ".tbl_xp_crop_tillharvest t2 on t1.crop_id = t2.crop_id and t1.tillharvest_id = t2.tillharvest_id" 
		+ " join " + cropsDb + ".tbl_soil_management_linkup_ktblmilp t3 on t2.worktype_id = t3.ktblmilp_worktype_id" 
		+ " join " + cropsDb + ".tbl_soil_management_equipment t4 on t3.soil_management_equipment_id = t4.soil_management_equipment_id"
		+ " WHERE process_id = " + process_id
		+ " order by xn_year, xn_month, xn_day;";
		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
			return rs;

	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}

public int getIrrigation_activities(int subplot){// Number of irrigation activities.
	
		try {
		
		String sql = "SELECT count(*) "  
			+ "FROM " + cropsDb + ".tbl_irrigation_per_subplot t WHERE t.`subplot_id`=" + subplot;
		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getInt(1);
		}
		return -9999;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return -9999;
	}
}


public String getIrrigation_date(int farmid, int plotid, int subplotid, int year){ //IRRIGATION DATE
	try {
		String sql = "SELECT t.`irrigation_date`" 
			+ "FROM " + cropsDb + ".tbl_irrigation_per_subplot t WHERE t.`scenario_id`=1000";
		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			Date d = rs.getDate(1);
			//System.out.println(d);
			int dy = d.getDate();
			int m = d.getMonth() + 1;
			//System.out.println(dy);
			
			String day="", month=""; 
			if (dy<10){
				 day = "0"+Integer.toString(dy);
			}
			else {
				 day = Integer.toString(dy);
			}
			
			

			if (m<10){
				 month = "0"+Integer.toString(m);
			}
			else {
				 month = Integer.toString(m);
			}

			
			int yr = d.getYear();
			String startdate = day+month+ (Integer.toString(yr)).substring(1,3);
			return startdate;
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getIrrigation_amount(){
	try {
		String sql = "Select t.`irrigation_amount`" //IRRIGATION AMOUNT
			+ "FROM " + cropsDb + ".tbl_irrigation_per_subplot t WHERE t.`scenario_id`=1000";

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getIrrigation_code(){
	try {
		String sql = "Select t.`irrigation_equipment_code`" //IRRIGATION AMOUNT
			+ "FROM " + cropsDb + ".tbl_irrigation_per_subplot t WHERE t.`scenario_id`=1000";

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}


public int getnumber_layer_per_horizon(int soilprofile){
	try {
		String sql = "SELECT count(*) "  
			+ "FROM " + cropsDb + ".tbl_soilprofile t WHERE t.`soilprofile_id`=" + soilprofile;
		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getInt(1);
		}
		return -9999;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return -9999;
	}
}
public int getsum_number_simulationlayers(int soilprofile){
	try {
		String sql = "SELECT sum(number_simulation_layers) "  
			+ "FROM " + cropsDb + ".tbl_soilprofile t WHERE t.`soilprofile_id`=" + soilprofile;
		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getInt(1);
		}
		return -9999;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return -9999;
	}
}
public String getprofile_layer_number(int soilprofile){
	try {
		String sql = "SELECT t.`profile_layer_number`"  
			+ "FROM " + cropsDb + ".tbl_soilprofile t WHERE t.`soilprofile_id`=" + soilprofile;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getprofile_layer_thickness(int soilprofile){
	try {
		String sql = "SELECT ROUND(t.`layer_thickness`)"  
			+ "FROM " + cropsDb + ".tbl_soilprofile t WHERE t.`soilprofile_id`=" + soilprofile;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getnumber_simulation_layers_per_profile_layer(int soilprofile, int layer){
	try {
		String sql = "SELECT t.`number_simulation_layers`"  
			+ " FROM " + cropsDb + ".tbl_soilprofile t " +
			"WHERE t.`soilprofile_id`=" + soilprofile +
			" AND profile_layer_number = " + layer;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getClay_content(int soilprofile, int layer){
	try {
		String sql = "SELECT t.`clay_content`"  
			+ " FROM " + cropsDb + ".tbl_soilprofile t " +
			"WHERE t.`soilprofile_id`=" + soilprofile +
			" AND profile_layer_number = " + layer;
		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){

			String s = rs.getString(1);
			if(s==null){
				return "-99"; 
			}
				return s;
		}
		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getSilt_content(int soilprofile, int layer){
	try {
		String sql = "SELECT t.`silt_content`"  
			+ " FROM " + cropsDb + ".tbl_soilprofile t " +
			"WHERE t.`soilprofile_id`=" + soilprofile +
			" AND profile_layer_number = " + layer;
		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){

			String s = rs.getString(1);
			if(s==null){
				return "-99"; 
			}
			return s;
		}
		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getSand_content(int soilprofile, int layer){
	try {
		String sql = "SELECT t.`sand_content`"  
			+ " FROM " + cropsDb + ".tbl_soilprofile t " +
			"WHERE t.`soilprofile_id`=" + soilprofile +
			" AND profile_layer_number = " + layer;
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){

			String s = rs.getString(1);
			if(s==null){
				return "-99"; 
			}
			return s;
		}
		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getOrgan_content(int soilprofile, int layer){
	try {
		String sql = "SELECT t.`org_content_substance`"  
			+ " FROM " + cropsDb + ".tbl_soilprofile t " +
			"WHERE t.`soilprofile_id`=" + soilprofile +
			" AND profile_layer_number = " + layer;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){

			String s = rs.getString(1);
			if(s==null){
				return "-99"; 
			}
			return s;
		}
		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getBulk_Density(int soilprofile, int layer){
	try {
		String sql = "SELECT t.`bulk_density`"  
			+ " FROM " + cropsDb + ".tbl_soilprofile t " +
			"WHERE t.`soilprofile_id`=" + soilprofile +
			" AND profile_layer_number = " + layer;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){

			String s = rs.getString(1);
			if(s==null){
				return "-99"; 
			}
				return s;
		}
		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getStone_cont(int soilprofile, int layer){
	try {
		String sql = "SELECT t.`stone_content`"  
			+ " FROM " + cropsDb + ".tbl_soilprofile t " +
			"WHERE t.`soilprofile_id`=" + soilprofile +
			" AND profile_layer_number = " + layer;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			
			String s = rs.getString(1);
			if(s==null){
				return "-99"; 
			}
			return s;
		}
		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getph_value(int soilprofile, int layer){
	try {
		String sql = "SELECT t.`ph_value`"  
			+ " FROM " + cropsDb + ".tbl_soilprofile t " + 
			"WHERE t.`soilprofile_id`=" + soilprofile +
			" AND profile_layer_number = " + layer;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			
			String s = rs.getString(1);
			if(s==null){
				return "-99"; 
			}
			return s;
		}
		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getsoil_sample_date_static(int soilprofile_id){
	//CT 100408 adapted to unified start date of simulation 
	return "020801";

}
public String getsoil_sample_date(int soilprofile_id){
	try {
		String sql = "SELECT t.`date_sampling`" 
			+ " FROM " + cropsDb + ".tbl_start_values t"
			+ " left join " + cropsDb + ".tbl_soilprofile t1"
			+ " on t.soilprofile_id = t1.soilprofile_id" 
			+ " and t.profile_layer = t1.profile_layer_number" 
			+ " WHERE t.`soilprofile_id`=" + soilprofile_id;
		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			
			Date d = rs.getDate(1);
			if (d == null){
				return "-99";
			}
			//System.out.println(d);
			int dy = d.getDate();
			int m = d.getMonth() + 1;
			//System.out.println(dy);
			
			String day="", month=""; 
			if (dy<10){
				 day = "0"+Integer.toString(dy);
			}
			else {
				 day = Integer.toString(dy);
			}
			
			

			if (m<10){
				 month = "0"+Integer.toString(m);
			}
			else {
				 month = Integer.toString(m);
			}

			
			int yr = d.getYear();
			String startdate = day+month+ (Integer.toString(yr)).substring(1,3);
			return startdate;
		}//else
			return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public int getnumber_layer_per_horizon_sample(int soilprofile_id){
	try {
		String sql = "SELECT count(*) "  
			+ " FROM " + cropsDb + ".tbl_start_values t"
			+ " left join " + cropsDb + ".tbl_soilprofile t1"
			+ " on t.soilprofile_id = t1.soilprofile_id" 
			+ " and t.profile_layer = t1.profile_layer_number" 
			+ " WHERE t.`soilprofile_id`=" + soilprofile_id;
		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getInt(1);
		}
		return -9999;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return -9999;
	}
}
public int getsum_number_simulationlayers_sample(int soilprofile_id){
	try {
		String sql = "SELECT sum(t.number_simulation_layers)"  
			+ " FROM " + cropsDb + ".tbl_start_values t"
			+ " left join " + cropsDb + ".tbl_soilprofile t1"
			+ " on t.soilprofile_id = t1.soilprofile_id" 
			+ " and t.profile_layer = t1.profile_layer_number" 
			+ " WHERE t.`soilprofile_id`=" + soilprofile_id;
		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getInt(1);
		}
		return -9999;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return -9999;
	}
}
public String getprofile_layer_number_sample(int soilprofile_id){
	try {
		String sql = "SELECT t.`profile_layer`"  
			+ " FROM " + cropsDb + ".tbl_start_values t"
			+ " left join " + cropsDb + ".tbl_soilprofile t1"
			+ " on t.soilprofile_id = t1.soilprofile_id" 
			+ " and t.profile_layer = t1.profile_layer_number" 
			+ " WHERE t.`soilprofile_id`=" + soilprofile_id;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getnumber_simulation_layers_per_profile_layer_sample(int soilprofile_id, int layer){
	try {
		String sql = "SELECT t.`number_simulation_layers`"  
			+ " FROM " + cropsDb + ".tbl_start_values t " 
			+ " WHERE t.`soilprofile_id`=" + soilprofile_id 
			+ " AND profile_layer = " + layer;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
		}
		return null;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getwater_cont(int soilprofile_id, int layer){
	try {
		String sql = "SELECT t.`water_cont`"  
			+ " FROM " + cropsDb + ".tbl_start_values t " +
			"WHERE t.`soilprofile_id`=" + soilprofile_id +
			" AND profile_layer = " + layer;
		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			
			String s = rs.getString(1);
			if(s==null){
				return "-99"; 
			}
			return s;
		}
		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getmatrix_pot(int soilprofile_id, int layer){
	try {
		String sql = "SELECT t.`matrix_pot`"  
			+ " FROM " + cropsDb + ".tbl_start_values t " 
			+ " WHERE t.`soilprofile_id`=" + soilprofile_id 
			+ " AND profile_layer = " + layer;
		
		//System.out.println(sql);
		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			
			String s = rs.getString(1);
			if(s==null){
				return "-99"; 
			}
			return s;
		}
		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getsoil_temp(int soilprofile_id, int layer){
	try {
		String sql = "SELECT t.`soil_temp`"  
			+ " FROM " + cropsDb + ".tbl_start_values t " +
			"WHERE t.`soilprofile_id`=" + soilprofile_id +
			" AND profile_layer = " + layer;
		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			String s = rs.getString(1);
			if(s==null){
				return "-99"; 
			}
			return s;
		}
		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getNH4(int soilprofile_id, int layer){
	try {
		String sql = "SELECT t.`NH4`"  
			+ " FROM " + cropsDb + ".tbl_start_values t " +
			"WHERE t.`soilprofile_id`=" + soilprofile_id +
			" AND profile_layer = " + layer;
		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			String s = rs.getString(1);
			if(s==null){
				return "-99"; 
			}
			return s;
		}
		return "-99";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getNO3(int soilprofile_id, int layer){
	try {
		String sql = "SELECT t.`NO3`"  
			+ " FROM " + cropsDb + ".tbl_start_values t "
			+ " WHERE t.`soilprofile_id`=" + soilprofile_id 
			+ " AND profile_layer = " + layer;
		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			String s = rs.getString(1);
			if(s==null){
				return "-99"; 
			}
			return s;
		}
		return "-99";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getroot_density(int soilprofile_id, int layer){
	try {
		String sql = "SELECT t.`root_density`"  
			+ " FROM " + cropsDb + ".tbl_start_values t " 
			+ " WHERE t.`soilprofile_id`=" + soilprofile_id 
			+ " AND profile_layer = " + layer;
		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			String s = rs.getString(1);
			if(s==null){
				return "-99"; 
			}
			return s;
		}
		return "-99";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}


public int getsum_number_water_reading_depth(int scenarioid){
	try {
		String sql = "SELECT sum(reading_depth) "  
			+ "FROM " + cropsDb + ".tbl_water_readings t WHERE t.`scenario_id`=" + scenarioid;
		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getInt(1);
		}
		return -9999;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return -9999;
	}
}
public int getnumber_water_readings(int scenarioid){//****WATER READINGS****
	try {
		String sql = "SELECT count(*) "  
			+ "FROM " + cropsDb + ".tbl_water_readings t WHERE t.`scenario_id`=" + scenarioid;
		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getInt(1);
		}
		return -9999;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return -9999;
	}
}
public String getwater_reading_date(int no_reading){ //***WATER READING DATE***
	try {
		String sql = "SELECT t.`date_reading`" 
			+ "FROM " + cropsDb + ".tbl_water_readings t";
		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			Date d = rs.getDate(1);
			//System.out.println(d);
			if(d==null){
				return "";
			}
			
			int dy = d.getDate();
			int m = d.getMonth() + 1;
			//System.out.println(dy);
			
			String day="", month=""; 
			if (dy<10){
				 day = "0"+Integer.toString(dy);
			}
			else {
				 day = Integer.toString(dy);
			}
			
			

			if (m<10){
				 month = "0"+Integer.toString(m);
			}
			else {
				 month = Integer.toString(m);
			}

			
			int yr = d.getYear();
			String startdate = day+month+ (Integer.toString(yr)).substring(1,3);
			return startdate;
		}
		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getwater_content(){
	try {
		String sql = "Select t.`water_content`" //WATER READING - WATER CONTENT
			+ "FROM " + cropsDb + ".tbl_water_readings t where `no_reading`=1";

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			
			String s = rs.getString(1);
			if(s==null){
				return "-99"; 
			}
			return s;
		}
			
		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getmatrix_potential(){
	try {
		String sql = "Select t.`matrix_potential`" //WATER READING - MATRIX POTENTIAL
			+ "FROM " + cropsDb + ".tbl_water_readings t where `no_reading`=1";

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			
			String s = rs.getString(1);
			if(s==null){
				return "-99"; 
			}
			return s;
		}

		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}


public int getsum_number_nitrogen_reading_depth(int scenarioid){//NITROGEN READINGS//
	try {
		String sql = "SELECT sum(reading_depth) "  
			+ "FROM " + cropsDb + ".tbl_nitrogen_readings t WHERE t.`scenario_id`=" + scenarioid;
		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getInt(1);
		}
		return -9999;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return -9999;
	}
}
public int getnumber_nitrogen_readings(int scenarioid){//***NITROGEN READINGS NUMBER***
	try {
		String sql = "SELECT count(*) "  
			+ "FROM " + cropsDb + ".tbl_nitrogen_readings t WHERE t.`scenario_id`=" + scenarioid;
		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getInt(1);
		}
		return -9999;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return -9999;
	}
}
public String getnitrogen_reading_date(int no_reading){ //***NITROGEN READING DATE***
	try {
		String sql = "SELECT t.`date_reading`" 
			+ "FROM " + cropsDb + ".tbl_nitrogen_readings t";
		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			Date d = rs.getDate(1);
			//System.out.println(d);
			if(d==null){
				return "";
			}
			
			int dy = d.getDate();
			int m = d.getMonth() + 1;
			//System.out.println(dy);
			
			String day="", month=""; 
			if (dy<10){
				 day = "0"+Integer.toString(dy);
			}
			else {
				 day = Integer.toString(dy);
			}
			
			

			if (m<10){
				 month = "0"+Integer.toString(m);
			}
			else {
				 month = Integer.toString(m);
			}

			
			int yr = d.getYear();
			String startdate = day+month+ (Integer.toString(yr)).substring(1,3);
			return startdate;
		}
		return "";//Change here the value
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getnitrat_content(){
	try {
		String sql = "Select t.`nitrat_content`" //NITROGEN READING - NITRAT CONTENT
			+ "FROM " + cropsDb + ".tbl_nitrogen_readings t where `no_reading`=1";

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			
			String s = rs.getString(1);
			if(s==null){
				return "-99"; 
			}
			return s;
		}

		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getammonium_content(){
	try {
		String sql = "Select t.`ammonium_content`" //NITROGEN READING - AMMONIUM CONTENT
			+ "FROM " + cropsDb + ".tbl_nitrogen_readings t where `no_reading`=1";

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			
			String s = rs.getString(1);
			if(s==null){
				return "-99"; 
			}
				return s;
		}

		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}

public int getnumber_plant_measurements_I(int scenarioid){//***PLANT MEASUREMENTS I-NUMBER OF MEASUREMENTS***
	try {
		String sql = "SELECT count(*) "  
			+ "FROM " + cropsDb + ".tbl_plant_measurementsI t WHERE t.`scenario_id`=" + scenarioid;
		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getInt(1);
		}
		return -9999;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return -9999;
	}
}
public String getplant_measurements_I_date(int scenarioid){ //***PLANT MEASUREMENTS I DATE***
	try {
		String sql = "SELECT t.`measurement_date`" 
			+ "FROM " + cropsDb + ".tbl_plant_measurementsI t";
		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			Date d = rs.getDate(1);
			//System.out.println(d);
			if(d==null){
				return "";
			}
			
			int dy = d.getDate();
			int m = d.getMonth() + 1;
			//System.out.println(dy);
			
			String day="", month=""; 
			if (dy<10){
				 day = "0"+Integer.toString(dy);
			}
			else {
				 day = Integer.toString(dy);
			}
			
			

			if (m<10){
				 month = "0"+Integer.toString(m);
			}
			else {
				 month = Integer.toString(m);
			}

			
			int yr = d.getYear();
			String startdate = day+month+ (Integer.toString(yr)).substring(1,3);
			return startdate;
		}
		return "";//Change here the value
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getplant_stage_EC_measurementI(){
	try {
		String sql = "Select t.`stage_EC`" //PLANT MEASUREMENT_I-STAGE
			+ "FROM " + cropsDb + ".tbl_plant_measurementsI t where `farm_id`=1";

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			
			String s = rs.getString(1);
			if(s==null){
				return "-99"; 
			}
			return s;
		}

		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getplant_BFI_measurementI(){
	try {
		String sql = "Select t.`BFI`" //PLANT MEASUREMENT_I-BFI
			+ "FROM " + cropsDb + ".tbl_plant_measurementsI t where `farm_id`=1";

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			
			String s = rs.getString(1);
			if(s==null){
				return "-99"; 
			}
			return s;
		}

		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getplant_soil_bedeck_measurementI(){
	try {
		String sql = "Select t.`soil_bedeck`" //PLANT MEASUREMENT_I-SOIL BEDECK
			+ "FROM " + cropsDb + ".tbl_plant_measurementsI t where `farm_id`=1";

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			
			String s = rs.getString(1);
			if(s==null){
				return "-99"; 
			}

				return s;

		}

		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getplant_leaf_number_measurementI(){
	try {
		String sql = "Select t.`leaf_number`" //PLANT MEASUREMENT_I-LEAF NUMBER
			+ "FROM " + cropsDb + ".tbl_plant_measurementsI t where `farm_id`=1";

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			
			String s = rs.getString(1);
			if(s==null){
				return "-99"; 
			}

				return s;

		}

		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getplant_shoot_number_measurementI(){
	try {
		String sql = "Select t.`shoot_number`" //PLANT MEASUREMENT_I-SHOOT NUMBER
			+ "FROM " + cropsDb + ".tbl_plant_measurementsI t where `farm_id`=1";

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			
			String s = rs.getString(1);
			if(s==null){
				return "-99"; 
			}
				return s;

		}

		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getplant_height_measurementI(){
	try {
		String sql = "Select t.`plant_height`" //PLANT MEASUREMENT_I- PLANT HEIGHT
			+ "FROM " + cropsDb + ".tbl_plant_measurementsI t where `farm_id`=1";

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			
			String s = rs.getString(1);
			if(s==null){
				return "-99"; 
			}
				return s;

		}

		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getplant_root_depth_measurementI(){
	try {
		String sql = "Select t.`root_depth`" //PLANT MEASUREMENT_I- ROOT DEPTH
			+ "FROM " + cropsDb + ".tbl_plant_measurementsI t where `farm_id`=1";

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			
			String s = rs.getString(1);
			if(s==null){
				return "-99"; 
			}

				return s;

		}

		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getplant_evap_factor_measurementI(){
	try {
		String sql = "Select t.`evap_factor`" //PLANT MEASUREMENT_I- EVAPORATION FACTOR 
			+ "FROM " + cropsDb + ".tbl_plant_measurementsI t where `farm_id`=1";

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			
			String s = rs.getString(1);
			if(s==null){
				return "-99"; 
			}

				return s;

		}

		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getplant_kum_nitrogen_uptake_measurementI(){
	try {
		String sql = "Select t.`kum_nitrogen_uptake`" //PLANT MEASUREMENT_I- ACCUMULATE NITROGEN UPTAKE 
			+ "FROM " + cropsDb + ".tbl_plant_measurementsI t where `farm_id`=1";

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			
			String s = rs.getString(1);
			if(s==null){
				return "-99"; 
			}

				return s;

		}

		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}

public int getnumber_plant_measurements_II(int scenarioid){//***PLANT MEASUREMENTS II-NUMBER OF MEASUREMENTS***
	try {
		String sql = "SELECT count(*) "  
			+ "FROM " + cropsDb + ".tbl_plant_measurementsII t WHERE t.`scenario_id`=" + scenarioid;
		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getInt(1);
		}
		return -9999;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return -9999;
	}
}
public String getplant_measurements_II_date(int scenarioid){ //***PLANT MEASUREMENTS II DATE***
	try {
		String sql = "SELECT t.`measurement_date`" 
			+ "FROM " + cropsDb + ".tbl_plant_measurementsII t";
		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			Date d = rs.getDate(1);
			//System.out.println(d);
			if(d==null){
				return "";
			}
			
			int dy = d.getDate();
			int m = d.getMonth() + 1;
			//System.out.println(dy);
			
			String day="", month=""; 
			if (dy<10){
				 day = "0"+Integer.toString(dy);
			}
			else {
				 day = Integer.toString(dy);
			}
			
			

			if (m<10){
				 month = "0"+Integer.toString(m);
			}
			else {
				 month = Integer.toString(m);
			}

			
			int yr = d.getYear();
			String startdate = day+month+ (Integer.toString(yr)).substring(1,3);
			return startdate;
		}
		return "";//Change here the value
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getplant_leaves_dm_measurementII(){
	try {
		String sql = "Select t.`leaves_dm`" //PLANT MEASUREMENT_II- DM IN LEAVES
			+ "FROM " + cropsDb + ".tbl_plant_measurementsII t where `farm_id`=1";

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			
			String s = rs.getString(1);
			if(s==null){
				return "-99"; 
			}

				return s;

		}

		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getplant_stems_dm_measurementII(){
	try {
		String sql = "Select t.`stems_dm`" //PLANT MEASUREMENT_II- DM IN STEMS
			+ "FROM " + cropsDb + ".tbl_plant_measurementsII t where `farm_id`=1";

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			
			String s = rs.getString(1);
			if(s==null){
				return "-99"; 
			}
			return s;
		}

		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getplant_fruit_dm_measurementII(){
	try {
		String sql = "Select t.`fruit_dm`" //PLANT MEASUREMENT_II- DM IN FRUITS
			+ "FROM " + cropsDb + ".tbl_plant_measurementsII t where `farm_id`=1";

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			
			String s = rs.getString(1);
			if(s==null){
				return "-99"; 
			}
			return s;
		}

		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getplant_root_dm_measurementII(){
	try {
		String sql = "Select t.`root_dm`" //PLANT MEASUREMENT_II- DM IN ROOTS
			+ "FROM " + cropsDb + ".tbl_plant_measurementsII t where `farm_id`=1";

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			
			String s = rs.getString(1);
			if(s==null){
				return "-99"; 
			}
			return s;
		}

		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getplant_above_biomass_dm_measurementII(){
	try {
		String sql = "Select t.`root_dm`" //PLANT MEASUREMENT_II- DM IN ABOVE BIOMASS
			+ "FROM " + cropsDb + ".tbl_plant_measurementsII t where `farm_id`=1";

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			
			String s = rs.getString(1);
			if(s==null){
				return "-99"; 
			}
			return s;
		}

		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getplant_leaves_nitrogen_measurementII(){
	try {
		String sql = "Select t.`leaves_nitrogen`" //PLANT MEASUREMENT_II- NITROGEN IN LEAVES (in % the dry matter)
			+ "FROM " + cropsDb + ".tbl_plant_measurementsII t where `farm_id`=1";

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			
			String s = rs.getString(1);
			if(s==null){
				return "-99"; 
			}
			return s;
		}

		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getplant_stems_nitrogen_measurementII(){
	try {
		String sql = "Select t.`stems_nitrogen`" //PLANT MEASUREMENT_II- NITROGEN IN STEMS (in % the dry matter)
			+ "FROM " + cropsDb + ".tbl_plant_measurementsII t where `farm_id`=1";

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			
			String s = rs.getString(1);
			if(s==null){
				return "-99"; 
			}
			return s;
		}

		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getplant_fruit_nitrogen_measurementII(){
	try {
		String sql = "Select t.`fruit_nitrogen`" //PLANT MEASUREMENT_II- NITROGEN IN FRUITS (in % the dry matter)
			+ "FROM " + cropsDb + ".tbl_plant_measurementsII t where `farm_id`=1";

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			
			String s = rs.getString(1);
			if(s==null){
				return "-99"; 
			}
			return s;
		}

		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getplant_root_nitrogen_measurementII(){
	try {
		String sql = "Select t.`root_nitrogen`" //PLANT MEASUREMENT_II- NITROGEN IN ROOTS (in % the dry matter)
			+ "FROM " + cropsDb + ".tbl_plant_measurementsII t where `farm_id`=1";

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			
			String s = rs.getString(1);
			if(s==null){
				return "-99"; 
			}
			return s;
		}

		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}
public String getplant_above_bio_nitrogen_measurementII(){
	try {
		String sql = "Select t.`root_nitrogen`" //PLANT MEASUREMENT_II- NITROGEN IN ABOVE BIOMASS NITROGEN (in % the dry matter)
			+ "FROM " + cropsDb + ".tbl_plant_measurementsII t where `farm_id`=1";

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			
			String s = rs.getString(1);
			if(s==null){
				return "-99"; 
			}
			return s;
		}

		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}

//******************************FROM HERE STARTS THE WEATHER FILE**************************************************

public String getStation_name(int station_id){
	try {
		String sql = "Select t.`station_name`" //LOCATION OF WEATHER STATION
			+ "FROM " + cropsDb + ".tbl_weatherstation t where `station_id`=" + station_id;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			
			String s = rs.getString(1);
			if(s==null){
				return "-99"; 
			}
			return s;
		}

		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}

public String getStation_latitude(int station_id){
	try {
		String sql = "Select t.`latitude`" //LATITUDE
			+ "FROM " + cropsDb + ".tbl_weatherstation t where `station_id`=" + station_id;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			
			String s = rs.getString(1);
			if(s==null){
				return "-99"; 
			}
			return s;
		}

		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}

public String getStation_longitude(int station_id){
	try {
		String sql = "Select t.`longitude`" //LONGITUDE
			+ "FROM " + cropsDb + ".tbl_weatherstation t where `station_id`=" + station_id;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			
			String s = rs.getString(1);
			if(s==null){
				return "-99"; 
			}
			return s;
		}

		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}

public String getStation_altitude(int station_id){
	try {
		String sql = "Select t.`altitude`" //ALTITUDE
			+ "FROM " + cropsDb + ".tbl_weatherstation t where `station_id`=" + station_id;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			
			String s = rs.getString(1);
			if(s==null){
				return "-99"; 
			}
			return s;
		}

		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}

public String getStation_avg_temperature(int station_id){
	try {
		String sql = "Select t.`avg_temp_year`" //AVERAGE TEMPERATURE
			+ "FROM " + cropsDb + ".tbl_weatherstation t where `station_id`=" + station_id;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			
			String s = rs.getString(1);
			if(s==null){
				return "-99"; 
			}
			return s;
		}

		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}

public String getStation_temp_amplitude(int station_id){
	try {
		String sql = "Select t.`temp_amplitude_monthly_avg`" //TEMP. AMPLITUDE
			+ "FROM " + cropsDb + ".tbl_weatherstation t where `station_id`=" + station_id;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			
			String s = rs.getString(1);
			if(s==null){
				return "-99"; 
			}
			return s;
		}

		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}

public String getStation_temp_measurement(int station_id){
	try {
		String sql = "Select t.`height_temp_measure_device`" //TEMP. MEASUREMENT
			+ "FROM " + cropsDb + ".tbl_weatherstation t where `station_id`=" + station_id;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			
			String s = rs.getString(1);
			if(s==null){
				return "-99"; 
			}
			return s;
		}

		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}

public String getStation_wind_measurement(int station_id){
	try {
		String sql = "Select t.`height_wind_measure_device`" //WIND MEASUREMENT
			+ "FROM " + cropsDb + ".tbl_weatherstation t where `station_id`=" + station_id;

		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			
			String s = rs.getString(1);
			if(s==null){
				return "-99"; 
			}
			return s;
		}

		return "";
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	}
}

public int getnumber_simulation_days(int station, int second_year){
	// CT 100409 adapted 
	try {
		int first_year = second_year -1;
		String sql = "SELECT count(*) "  
			+ " FROM " + cropsDb + ".tbl_weatherdata_per_station t "
			+ " WHERE t.`year` IN ("+ second_year+", "+first_year + ") and t.`station_id`=" + station;
		
		//call method run query, get ResultSet back
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getInt(1);
		}
		return -99;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return -99;
	}
}

public String getsimulation_day(int station, int simulation_day){
	try {
	String sql = "SELECT t.`day`"  
		+ " FROM " + cropsDb + ".tbl_weatherdata_per_station t " +
		"WHERE t.`station_id`=" + station;
		

	//call method run query, get ResultSet back
	ResultSet rs = cmd.runQuery(sql);
	while (rs.next()){
		return rs.getString(1);
		
		}
		return null;
	} catch (SQLException e) {
	// TODO Auto-generated catch block
	e.printStackTrace();
	return null;
	
	}
}

public ResultSet getWeatherResultSet_per_station(int station , int second_year){

	try { //put here all the columns that need for one row in right order"
	int first_year = second_year -1;	
	
	String sql = "SELECT  day, month, year, day_year, global_radiation, maximum_temperature, minimum_temperature, precipitation, dewpoint, avg_wind_speed, photosynthetic_activity, sunshine_duration, avg_temp, relative_humidity, saturation_deficit_air, pan_evaporation, snow_depth, soil_temp_5_cm, soil_temp_10_cm, soil_temp_20_cm"
		+ " FROM " + cropsDb + ".tbl_weatherdata_per_station t "
		+ "WHERE (t.`year` IN ("+ second_year +", "+ first_year + ")) AND t.`station_id` = " + station 
		+ " ORDER BY year, month, day ASC ";
	
		ResultSet rs = cmd.runQuery(sql);
		
		return rs;
	} catch (SQLException e) {
	// TODO Auto-generated catch block
	e.printStackTrace();
	return null;
	
	}
}

//******************************FROM HERE STARTS THE XNM FILE********************************************************

public String getXnm_file_name() {
	try {
		
		String sql = "SELECT t.`field_id` "
				+ "FROM " + cropsDb + ".tbl_soilprofile t WHERE t.`soilprofile_id` = 6"; 
		ResultSet rs = cmd.runQuery(sql);
		while (rs.next()){
			return rs.getString(1);
			
			}
			return null;
		} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
		
		}
	}

public int getSoilIdForCellGroup (int cellGroup, String cellgroupScenario) {
	try {
		String sql = "SELECT soilprofile_id FROM " + cropsDb + ".tbl_link_cellgroup_soil " +
				"WHERE cellgroup = " + cellGroup + " AND scen = '" + cellgroupScenario +"'";
		ResultSet rs = cmd.runQuery(sql);
		rs.first();
		return rs.getInt("soilprofile_id");
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return -1;
	
	}
}

public ResultSet getSoilPhysicalResultSet(int soilprofile){

	try { //put here all the columns that need for one row in right order"
		//TODO: CT problem: van_genuchten_theta_pF42 is not known in pak_crops! we have theta_pF42, assumed it's that, is taht correct?
	/*Old Version: String sql = "SELECT  Profile_layer_number, number_simulation_layers, soil_type, (van_genuchten_theta_pF42)*100 van_genuchten_theta_pF42, field_capacity, total_pore_volume, (saturated_conductivity_kf)*10 saturated_conductivity_kf " 
		+ " FROM " + cropsDb + ".tbl_soilprofile t "
		+ "WHERE t.`soilprofile_id`=" + soilprofile
		+ " order by Profile_layer_number ASC ";*/
		String sql = "SELECT  Profile_layer_number, number_simulation_layers, soil_type, (theta_pF42)*100 van_genuchten_theta_pF42, field_capacity, total_pore_volume, (saturated_conductivity_kf)*10 saturated_conductivity_kf " 
			+ " FROM " + cropsDb + ".tbl_soilprofile t "
			+ "WHERE t.`soilprofile_id`=" + soilprofile
			+ " order by Profile_layer_number ASC ";
		ResultSet rs = cmd.runQuery(sql);
		
		return rs;
	} catch (SQLException e) {
	// TODO Auto-generated catch block
	e.printStackTrace();
	return null;
	
	}
}
public ResultSet getMineralizationParametersResultSet(int soilprofile){

	try { //put here all the columns that need for one row in right order"
	
		String sql = "SELECT *" //TODO: enter values to the database and complete query here 
			+ " FROM " + cropsDb + ".tbl_soilprofile t "
			+ "WHERE t.`soilprofile_id`=" + soilprofile
			+ " order by Profile_layer_number ASC ";
		ResultSet rs = cmd.runQuery(sql);
		
		return rs;
	} catch (SQLException e) {
	// TODO Auto-generated catch block
	e.printStackTrace();
	return null;
	
	}
}
public ResultSet getSoilHydraulicResultSet(int soilprofile){

	try { //put here all the columns that need for one row in right order"
	String sql = "SELECT  Profile_layer_number, number_simulation_layers, (theta_R)*100 theta_R, (theta_S)*100 theta_S, campbell_a, campbell_b, van_genuchten_alpha, van_genuchten_n, van_genuchten_m" 
		+ " FROM " + cropsDb + ".tbl_soilprofile t "
		+ "WHERE t.`soilprofile_id`=" + soilprofile
		+ " order by Profile_layer_number ASC ";

		ResultSet rs = cmd.runQuery(sql);
		
		return rs;
	} catch (SQLException e) {
	// TODO Auto-generated catch block
	e.printStackTrace();
	return null;
	
	}
}

public ResultSet getrate_constant_leachn(int soilprofile){

	try { //put here all the columns that need for one row in right order"
	String sql = "SELECT profile_layer_number, number_simulation_layers, UreaHy, Nitrif, Denitrif, MinerLit, MinerMan, MinerHum" 
		+ " FROM " + cropsDb + ".tbl_soilprofile t "
		+ "WHERE t.`soilprofile_id`=" + soilprofile
		+ " order by Profile_layer_number ASC ";

		ResultSet rs = cmd.runQuery(sql);
		
		return rs;
	} catch (SQLException e) {
	// TODO Auto-generated catch block
	e.printStackTrace();
	return null;
	
	}
}

public ResultSet getStart_soil_values_C(int soilprofile){

	try { //put here all the columns that need for one row in right order"
	String sql = "SELECT profile_layer_number, number_simulation_layers, litter_c, litter_n, manure_c, manure_n, humus_c, humus_n" 
		+ " FROM " + cropsDb + ".tbl_soilprofile t "
		+ "WHERE t.`soilprofile_id`=" + soilprofile
		+ " order by Profile_layer_number ASC ";

		ResultSet rs = cmd.runQuery(sql);
		
		return rs;
	} catch (SQLException e) {
	// TODO Auto-generated catch block
	e.printStackTrace();
	return null;
	
	}
}

public ResultSet getrate_constant(int soilprofile){

	try { //put here all the columns that need for one row in right order"
	String sql = "SELECT profile_layer_number, number_simulation_layers, FOMSlow, FOMFast, FOMVeryFast, MicBiomSlow, MicBiomFast, HumusSlow, HumusFast" 
		+ " FROM " + cropsDb + ".tbl_soilprofile t "
		+ "WHERE t.`soilprofile_id`=" + soilprofile
		+ " order by Profile_layer_number ASC ";

		ResultSet rs = cmd.runQuery(sql);
		
		return rs;
	} catch (SQLException e) {
	// TODO Auto-generated catch block
	e.printStackTrace();
	return null;
	
	}
}

public ResultSet getOrgCarbon (int soilprofile) {
	try { //put here all the columns that need for one row in right order"
		String sql = "Select number_simulation_layers, total_org_carbon, bulk_density, layer_thickness"
			+ " FROM " + cropsDb + ".tbl_soilprofile t "
			+ "WHERE t.`soilprofile_id`=" + soilprofile
			+ " order by Profile_layer_number ASC ";
		
		ResultSet rs = cmd.runQuery(sql);
		return rs;
	} catch (SQLException e) {
		e.printStackTrace();
		return null;
	}
 }

public ResultSet getOrgCarbonPlusC_N (int soilprofile) {
	try { //put here all the columns that need for one row in right order"
		String sql = "Select number_simulation_layers, total_org_carbon, bulk_density, layer_thickness, C_N_ratio "
			+ " FROM " + cropsDb + ".tbl_soilprofile t "
			+ "WHERE t.`soilprofile_id`=" + soilprofile
			+ " order by Profile_layer_number ASC ";
		
		ResultSet rs = cmd.runQuery(sql);
		return rs;
	} catch (SQLException e) {
		e.printStackTrace();
		return null;
	}
 }

public double getPoolShares (String pool, int scenario) {
	try { //put here all the columns that need for one row in right order"
		String sql = "SELECT * FROM " + cropsDb + ".tbl_pool_shares" 
			+ " WHERE pool_name = '" + pool 
			+ "' AND scenario = " + scenario;
		ResultSet rs = cmd.runQuery(sql);
		rs.first();
		double share = rs.getDouble("share");
		return share;
	} catch (SQLException e) {
		e.printStackTrace();
		return 0;
	}
 }

//******************************FROM HERE STARTS THE GECROS INPUT FILE********************************************************

public ResultSet getCrop_Gecros_parameters(int crop_id, int genotype_id){

	try {
		
	
	String sql = "SELECT t1.`xn_code`, t1.`crop_name_english`, t2.`genotype_txt`, t2.`ecotype`, t2.`daylengthtype`, t2.`leafangletype` "
		+ "FROM " + cropsDb + ".tbl_crop t1 join " + cropsDb + ".tbl_genotype t2 on t1.crop_id = t2.crop_id "
		+ "WHERE t1.`crop_id` = '" + crop_id
		+ "' AND t2.`genotype_id` = " + genotype_id;

		
		ResultSet rs = cmd.runQuery(sql);
		
		return rs;
	} catch (SQLException e) {
	// TODO Auto-generated catch block
	e.printStackTrace();
	return null;
	
	}
}

public ResultSet getCrop_Type_parameters(int crop_id){

	try {
		
	
	String sql = "Select t.`legume`, t.`photosynthesistype_txt`, t.`determinate`, t.`short_day`, t.`with_lodging`, t.`vernalisation`"
		+ "FROM " + cropsDb + ".tbl_crop t"
		+ " WHERE t.`crop_id` = " + crop_id;
		

		
		ResultSet rs = cmd.runQuery(sql);
		
		return rs;
	} catch (SQLException e) {
	// TODO Auto-generated catch block
	e.printStackTrace();
	return null;
	
	}
}

public ResultSet getVegetative_Generative_Biomass_data(int crop_id){

	try {		
	
	String sql = "SELECT  t.`veg_biom_growth_effic`, t.`veg_biom_carbon_frac`, t.`germin_effic`, t.`seed_lipids`, t.`seed_lignin`, t.`composition_org_acids`, t.`fractions_minerals`"
		+ " FROM " + cropsDb + ".tbl_crop t "
		+ "WHERE t.`crop_id` = " + crop_id;
	
		ResultSet rs = cmd.runQuery(sql);
		
		return rs;
	} catch (SQLException e) {
	// TODO Auto-generated catch block
	e.printStackTrace();
	return null;
	
	}
}

public ResultSet getMorphology_phenology_data(int crop_id){

	try { //put here all the columns that need for one row in right order"
		
	
	String sql = "SELECT  leaf_width, stem_dry_weight, max_root_depth, temp_respon_base,temp_respon_optimum, temp_respon_ceiling,temp_phenology_curve"
		+ " FROM " + cropsDb + ".tbl_crop t "
		+ "WHERE t.`crop_id` = " + crop_id;
	
		ResultSet rs = cmd.runQuery(sql);
		
		return rs;
	} catch (SQLException e) {
	// TODO Auto-generated catch block
	e.printStackTrace();
	return null;
	
	}
}

public ResultSet getNitrogen_Gecros(int crop_id){

	try { //put here all the columns that need for one row in right order"
		
	
	String sql = "SELECT  max_n_uptake, min_root_n, min_steam_n, min_leaf_n,initial_leaf_n, specif_leaf_area,n_fix_cost"
		+ " FROM " + cropsDb + ".tbl_crop t "
		+ "WHERE t.`crop_id` = " + crop_id;
	
		ResultSet rs = cmd.runQuery(sql);
		
		return rs;
	} catch (SQLException e) {
	// TODO Auto-generated catch block
	e.printStackTrace();
	return null;
	
	}
}

public ResultSet getPhotoperiod_photosyn(int crop_id){

	try { //put here all the columns that need for one row in right order"
		
	
	String sql = "SELECT  inclination_sun_angle_photoperiodic_daylength, dev_stage_start_photo_phase, dev_stage_end_photo_phase, activation_energy_jmax, slopevcmax_leaf_n,slopevjmax_leaf_n,convex_light_resp_electron_transport"
		+ " FROM " + cropsDb + ".tbl_crop t "
		+ "WHERE t.`crop_id` = " + crop_id;
	
		ResultSet rs = cmd.runQuery(sql);
		
		return rs;
	} catch (SQLException e) {
	// TODO Auto-generated catch block
	e.printStackTrace();
	return null;
	
	}
}

public ResultSet getGenotype_parameters(int crop_id, int geno_id){

	try { //put here all the columns that need for one row in right order"
		
	
	String sql = "SELECT seed_weight, seed_n_concen,leaf_angle_frm_horizontal, max_height, min_thermal_days_veg,min_thermal_days_reprod,photoper_sensitivity"
		+ " FROM " + cropsDb + ".tbl_genotype t "
		+ "WHERE t.`crop_id` = " + crop_id
		+ " AND t.`genotype_id` = " + geno_id;
	
		ResultSet rs = cmd.runQuery(sql);
		
		return rs;
	} catch (SQLException e) {
	// TODO Auto-generated catch block
	e.printStackTrace();
	return null;
	
	}
}

public ResultSet getDefault_parameters(int crop_id, int genotype_id){

	try { //put here all the columns that need for one row in right order"
		
	
	String sql = "SELECT sig_curve_inflex_height, sig_curve_inflex_seed_growth, dev_stage_seed_num_indeter_crops, crit_root_weight_den"
		+ " FROM " + cropsDb + ".tbl_genotype t "
		+ "WHERE t.`crop_id` = " + crop_id
		+ " And t.`genotype_id`= " + genotype_id;
		
		ResultSet rs = cmd.runQuery(sql);
		
		return rs;
	} catch (SQLException e) {
	// TODO Auto-generated catch block
	e.printStackTrace();
	return null;
	
	}
}

public ResultSet getSensitivity_analysis_param(int crop_id, int genotype_id){

	try { //put here all the columns that need for one row in right order"
		
	
	String sql = "SELECT t.`CO2A`, t.`COEFR`, t.`COEFT`, t.`COEFV`, t.`FCRSH`, t.`FNRSH`, t.`PNPRE`, t.`CB`, t.`CX`, t.`TM`"
		+ "FROM " + cropsDb + ".tbl_genotype t "
		+ "WHERE t.`crop_id` = " + crop_id
		+ " And t.`genotype_id`= " + genotype_id;
		
		
		ResultSet rs = cmd.runQuery(sql);
		
		return rs;
	} catch (SQLException e) {
	// TODO Auto-generated catch block
	e.printStackTrace();
	return null;
	
	}
}

public ResultSet getSoil_param_I(int soilprofile, int layer){
	try {
		String sql = "SELECT t.`clay_frac`, t.`min_water_content`, t.`plant_water_capacity`, t.`soil_profile_depth`, t.`upper_soil_depth`, t.`time_dyn_temp`, t.`time_dyn_transf`, t.`descom_rate_micro`, t.`descom_rate_humus`"  
			+ " FROM " + cropsDb + ".tbl_soilprofile t " +
			"WHERE t.`soilprofile_id`=" + soilprofile +
			" AND profile_layer_number = " + layer;
		
ResultSet rs = cmd.runQuery(sql);
		
		return rs;
	} catch (SQLException e) {
	// TODO Auto-generated catch block
	e.printStackTrace();
	return null;
	
	}
}

public ResultSet getSoil_param_II(int soilprofile, int layer){
	try {
		String sql = "SELECT t.`frac_leaf_litter`, t.`ratio_decomp_resistant_plant_mat`, t.`decomp_plant_mat_decomp`, t.`decomp_plant_mat_resistant`, t.`total_org_carbon`, t.`bio_humus`, t.`frac_bio_ini_toc`"  
			+ " FROM " + cropsDb + ".tbl_soilprofile t " +
			"WHERE t.`soilprofile_id`=" + soilprofile +
			" AND profile_layer_number = " + layer;
		
ResultSet rs = cmd.runQuery(sql);
		
		return rs;
	} catch (SQLException e) {
	// TODO Auto-generated catch block
	e.printStackTrace();
	return null;
	
	}
}
public ResultSet get_static_yield(int lu_id, int soil_id, int weatherfileID) {
	try {
		String sql = "SELECT yield FROM " + cropsDb + ".vw_static_yields WHERE process_id = "+ lu_id + " and soiltype_id = "+ soil_id + " and weather_id = "+ weatherfileID;
		ResultSet rs = cmd.runQuery(sql);
		return rs;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	
	}
}

public boolean run_query(String sqlstring) {
	try {
		cmd.addToBatch(sqlstring);
		cmd.execBatch();
		return true;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return false;
	
	}
}
public int getnumber_of_layers(String RUN, int cellGroup, int simulationPeriod) {
	try {
		String sql = "SELECT count(*) FROM tbl_IMR_start_values WHERE "
			+ "model_run = '" + RUN + "' and cellgroup = " + cellGroup + " and period = "+ simulationPeriod;  
		ResultSet rs = cmd.runQuery(sql);
		rs.first();
		return rs.getInt(1);
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return -1;
	
	}
}

public ResultSet get_start_values(String RUN, int cellGroup, int simulationPeriod, int profile_layer) {
	try {
		String sql = "SELECT number_simulation_layers, water_cont, matrix_pot, soil_temp, NH4, NO3, root_density FROM tbl_IMR_start_values WHERE "
			+ "model_run = '" + RUN + "' and cellgroup = " + cellGroup + " and period = "+ simulationPeriod + " and profile_layer = " + profile_layer;  
		ResultSet rs = cmd.runQuery(sql);
		return rs;
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return null;
	
	}
	
	}
public boolean soil_information_has_been_updated(String RUN, int cellGroup, int simulationPeriod){
	
	try {
		String sql = "SELECT updated FROM tbl_IMR_cellGroup_updated WHERE "
			+ "model_run = '" + RUN + "' and cellgroup = " + cellGroup + " and period = "+ simulationPeriod;  
		ResultSet rs = cmd.runQuery(sql);
		
		
		if (rs.next() && rs.getInt("updated") == 1) {
			return true;
		}
		else {
			return false;
		}
	} catch (SQLException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		return false;
	
	}
	
	
	

}
}
