package uni.hh.mapmaker;

public class XN_tool_UI {

	public static void main(String[] args) {
		
		int mode = 1; // Default mode creating xnd files
		//String outfile = "./output/";
		String outfile = "./";
		String mysqlconfig = "./Xnwrapper/xnfilemaker/config/mysql_config.txt";
		String projectdir = "C:\\Program Files\\Expert-N 3.0";
		int lu_id = -1;
		int soil_id = -1;
		int weather_station = -1;
		int weather_year = -1;
		int start_year = -1;
		int end_year = -1;

		
		int weatherfileID = -99;
		String resultID = "Test";
		int xn_config = 1;
		String table = "tbl_xndfiles";
		int cellGroup = -1;
		int crop_id = -1;
		int simulationPeriod = -1;
		String file_link_cell_rfs = "-1";
		String cropGrowthModel = "CERES";
		String cellGroupScenario = "eq";
		String RUN = "TEST";
		boolean exact_dates = false;
		int xnVersion = 3;
		String mpmasDB = "for1695_mpmas";
		String cropDB = "pak_crops";
		//boolean is_subsequent_year = false;
		boolean noDB = false;
		
		String xnmOutfile = "";
		String xnmInfile  = "";
		String rfsInfile  = "";
		String xndInfile  = "";
		String weatherFn  = "";
		String startDate  = "";
		String endDate    = "";
		String gecrosInfile = "";
		
		int i = 0;
		while ( i < args.length) {
			//System.out.println(args[i]);
			if (args[i].charAt(0) == 45) { //check if hyphen at beginning
				//System.out.println("-");
				switch (args[i].charAt(1) ) {
					case 65: cropGrowthModel = args[i].substring(2);break;//A
					case 66: crop_id = Integer.parseInt(args[i].substring(2));break;//B
					case 67: mysqlconfig = args[i].substring(2);break;//C
					case 68: projectdir = args[i].substring(2);break;//D
					case 70: file_link_cell_rfs = args[i].substring(2);break;//F
					case 71: cellGroup = Integer.parseInt(args[i].substring(2));break;//G
					case 80: simulationPeriod = Integer.parseInt(args[i].substring(2));break;//P
					case 76: lu_id = Integer.parseInt(args[i].substring(2));break;//L
					case 77: mode = Integer.parseInt(args[i].substring(2));break;//M
					case 79: outfile = args[i].substring(2);break;//O		
					case 82: resultID = args[i].substring(2);break;//R
					case 83: soil_id = Integer.parseInt(args[i].substring(2));break;//S
					case 84: table = args[i].substring(2);break;//T
					case 85: RUN = args[i].substring(2);break;//U
					case 86: xnVersion = Integer.parseInt(args[i].substring(2));break;//V
					case 87: weatherfileID = Integer.parseInt(args[i].substring(2));break;//W
					case 88: xn_config = Integer.parseInt(args[i].substring(2));break;//X
					case 89: weather_year =Integer.parseInt(args[i].substring(2));break;//Y
					case 90: cellGroupScenario = args[i].substring(2);break;//Z
					case 101: end_year= Integer.parseInt(args[i].substring(2));break;//e
					case 115: start_year= Integer.parseInt(args[i].substring(2));break;//s
					case 119: weather_station = Integer.parseInt( args[i].substring(2));break;//w				
					case 120: exact_dates = true;//x
					//TODO: activate (flag 1=true, 0=false indicating whether it is subsequent year
					//case 71: is_subsequent_year = Boolean.parseBoolean(args[i].substring(2));break;//F 
				}
				if (args[i].charAt(1) == 45) { //check if second hyphen at beginning
					//System.out.println("--");
					if (args[i].substring(2).equals("rfs-in")) {
						rfsInfile = args[++i]; 
						System.out.println("rfsInfile: " + rfsInfile);
					}
					else if (args[i].substring(2).equals("xnd-in")) {
						xndInfile = args[++i];
						System.out.println("xndInfile: " + xndInfile);
					}
					else if (args[i].substring(2).equals("xnm-in")) {
						xnmInfile = args[++i]; 
						System.out.println("xnmInfile: " + xnmInfile);
					}
					else if (args[i].substring(2).equals("gecros-in")) {
						gecrosInfile = args[++i]; 
						System.out.println("gecrosInfile: " + gecrosInfile);
					}
					else if (args[i].substring(2).equals("xnd-out")) {
						outfile = args[++i]; 
						System.out.println("xndOutfile: " + outfile);
					}
					else if (args[i].substring(2).equals("xnm-out")) {
						xnmOutfile = args[++i]; 
						System.out.println("xnmOutfile: " + xnmOutfile);
					}
					else if (args[i].substring(2).equals("weatherFn")) {
						weatherFn = args[++i]; 
						System.out.println("Change weather to: " + weatherFn );
					}
					else if (args[i].substring(2).equals("start")) {
						startDate = args[++i];
						System.out.println("Change start date to: " + startDate );
					}	
					else if (args[i].substring(2).equals("end")) {
						endDate = args[++i];
						System.out.println("Change end date to: " + endDate );
					}	
				}
			}
			else {
				//System.out.println("no-");
				if (args[i].equalsIgnoreCase("updatefromrfs")) {
					mode = 11; noDB = true;
					System.out.println("Updating xnm and xnd from rfs");
				}
			}
			i++;
		}
		//System.out.println("Group " + cellGroup + "\n");
		/*TODO: activate		
		FileMaker fm;
		if(is_subsequent_year){
			fm = new FileMaker(mysqlconfig,true);
		}
		else{
			fm = new FileMaker(mysqlconfig);
		}
		
		//TODO: deactivate 
		*/
		
		FileMaker fm;
		if (noDB) {
			fm = new FileMaker();
		}
		else {
			fm = new FileMaker(mysqlconfig, cropDB, mpmasDB);
		}
		
		
		if (xnVersion == 3) {
			switch (mode) {
				case 0: fm.asciiFileMaker_XND(outfile, resultID, lu_id, soil_id, weatherfileID, xn_config, cellGroup, simulationPeriod, cropGrowthModel, RUN, exact_dates);
						fm.asciiFileMaker_XNP(outfile, outfile);
						fm.asciiFileMaker_XNI(projectdir, outfile);
				break;
				case 1: fm.asciiFileMaker_XND(outfile, resultID, lu_id, soil_id, weatherfileID, xn_config, cellGroup, simulationPeriod, cropGrowthModel, RUN, exact_dates);break;
				case 2: fm.asciiFileMaker_XNW(outfile, weather_station, weather_year); break;
				case 3: fm.asciiFileMaker_XNM(outfile, soil_id, cropGrowthModel, simulationPeriod, cellGroup, "-1", crop_id, RUN, -1);break;
				case 4: fm.asciiFileMaker_XND_loop(outfile, table, cropGrowthModel, exact_dates) ;break;
				case 5: fm.direct_yield_query(outfile, lu_id, soil_id, weatherfileID);break;
				case 6: fm.asciiFileMaker_XNM_loop(outfile,  simulationPeriod, file_link_cell_rfs, cellGroupScenario, RUN) ;break;
				case 7: fm.asciiFileMaker_XNW_loop(outfile, weather_station, start_year, end_year);break;
				case 8: fm.asciiFileMaker_XNP(outfile, outfile);break;
				case 9: fm.asciiFileMaker_XNI(projectdir, outfile);break;

				case 10: fm.asciiFileMaker_XNW(outfile, weatherfileID); break;
				case 11: fm.cmd_updateFromRFS(outfile, xnmOutfile, xndInfile, xnmInfile, rfsInfile, weatherFn, startDate, endDate ); break;
			}
		}
		else if (xnVersion == 5) {
			switch (mode) {
				case 1: fm.asciiFileMakerXN5Management(outfile, resultID, lu_id, soil_id);break;
			}
		}

		else {
			System.err.println("Error: Invalid xnVersion, wrong argument to -V option");
			System.exit(1);
		}
		
//		fm.asciiFileMaker_XND(	"C:\\workspace\\XN_tool\\output\\");

//		fm.asciiFileMaker_XND("C:\\Program Files\\Expert-N 3.0\\test.xnd");
		
//		fm.asciiFileMaker_XNW("C:\\workspace\\XN_tool\\output\\test_xn_mas5.xnw", 2, 1996);
		
//		fm.asciiFileMaker_XNM("C:\\workspace\\XN_tool\\output\\crop.xnm");
		
//		fm.asciiFileMaker_Gecros_File("C:\\workspace\\XN_tool\\output\\gecros.xnm");
	}
	
}
