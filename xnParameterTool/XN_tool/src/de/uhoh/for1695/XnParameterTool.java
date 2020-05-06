package de.uhoh.for1695;

import javax.swing.JFrame;
import javax.swing.JDialog;
import javax.swing.JFileChooser;
import javax.swing.JOptionPane;
import javax.swing.JSeparator;

import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JMenu;

import javax.swing.*;

import javax.swing.border.*;

import javax.swing.table.TableModel;
import javax.swing.table.TableColumn;
import javax.swing.table.TableCellRenderer;


//import com.sun.crypto.provider.RSACipher;

import java.awt.Toolkit;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.event.*;
import java.awt.Font;

import de.uhoh.for1695.XnDatabaseConnection;
import de.uhoh.for1695.ImportXn3Dialog.xn3ImportType;

import java.io.*;
import java.rmi.activation.ActivateFailedException;
import java.sql.*;
import javax.sql.rowset.RowSetProvider;
import javax.sql.rowset.CachedRowSet;
import javax.sql.RowSetListener;
import javax.sql.RowSetEvent;
import javax.swing.event.TableModelListener;

import java.util.*;


public class XnParameterTool {

	String outdir = "./";
	//String mysqlconfig = "./config/mysql_config.txt";

	static XnParameterToolConfig myConfig;
	
	
	public static void main(String[] args) {
	
		/*try {
			UIManager.setLookAndFeel(
                UIManager.getSystemLookAndFeelClassName());
		}
		catch(Exception e)
		{
			e.printStackTrace();
		}*/
		initConfig();

		XnDatabaseConnection myConnection = new XnDatabaseConnection(myConfig.dbHost, myConfig.dbSchema, myConfig.dbUser, myConfig.dbPwd);

		while (!myConnection.isValid())
		{
			myConnection.getLastError().printStackTrace();
			int rc = JOptionPane.showOptionDialog(null, myConnection.getLastError().getMessage(),"Connection failed!",
	                JOptionPane.YES_NO_OPTION,
	                JOptionPane.ERROR_MESSAGE, null, 
	                new String[]{"Change connection parameters", "Cancel"}, "Change connection parameters");
			if (rc == JOptionPane.NO_OPTION)
			{
				System.exit(1);
			}
			else if (rc == JOptionPane.YES_OPTION)
			{
				myConfig.askForConfigEntries();
				myConnection = new XnDatabaseConnection(myConfig.dbHost, myConfig.dbSchema, myConfig.dbUser, myConfig.dbPwd);
			}
		}
		
		
		XnParameterToolMainWindow mainFrame = new XnParameterToolMainWindow("FOR1695 XnParameterTool", myConfig, myConnection);
		mainFrame.pack();
        mainFrame.setVisible(true);
		
		//myConnection.loop_importDaisyPoolsFromXnm();
	}
   public static void initConfig() {
	   
	   myConfig = new XnParameterToolConfig();
	   String configFile = "XnParameterTool.ini";
		File configFH = new File(configFile);
		if ( configFH.isFile() )
		{
			myConfig.readConfig(configFile);
		}

    	myConfig.askForConfigEntries();
   }
}	 
class XnParameterToolMainWindow extends JFrame implements ActionListener {
	
	//Menu
	private JMenuItem menuItemOpenProject;
	private JMenuItem menuItemNewProject;
	private JMenuItem menuItemCopyProject;
	private JMenuItem menuItemLoadConfig;
	private JMenuItem menuItemSaveConfig;
	private JMenuItem menuItemClose;
	
	private JMenuItem menuItemImportGecrosXnm;
	private JMenuItem menuItemImportGecrosIni;
	private JMenuItem menuItemImportDaisyXnm;
	private JMenuItem menuItemImportManagementXnd;
	private JMenuItem menuItemImportSoilStartXndXnm;
	private JMenuItem menuItemImportSoilParamXndXnm;

	private JMenuItem menuItemImportSomPoolsXnm;
	private JMenuItem menuItemImportSoilDataAll;
	private JMenuItem menuItemImportFull;
	private JMenuItem menuItemImportManECloop;
	private JMenuItem menuItemExportXn5Input;
	private JMenuItem menuItemExportGecrosIni;
	private JMenuItem menuItemExportCellCfg;
	private JMenuItem menuItemExportCellManagement;
	private JMenuItem menuItemExportWeatherFiles;
	
	
	public simProject currentProject;
	
	private JLabel labelDescription;
	private JLabel labelCode;
	private JLabel labelAuthorCode;
	private JLabel labelProjType;
	
	//Tabs
	//private XnParameterProjectPanel panelCurrentProject;
	private XnParameterGecrosPanel panelGecros;
	public XnParameterProjectPanel panelProjectGeneral;
	private XnParameterSoilPanel panelSoils;
	private XnParameterManagementPanel panelManagements;
	private XnParameterXpnPanel panelXpn;
	private XnParameterXpiPanel panelXpi;
	
	private XnParameterContentEditorPanel panelOutputVarlist;
	private XnParameterContentEditorPanel panelSpecialOutputDef;
	private XnParameterCouplingPanel panelBEMSCouplingIni;
	
	//Background storage
	public XnParameterToolConfig myConfig;
	public XnDatabaseConnection myConnection;
	
	
	
	
	public XnParameterToolMainWindow(String title, XnParameterToolConfig config, XnDatabaseConnection conn ) {
		super(title);
		
		this.setDefaultCloseOperation(EXIT_ON_CLOSE);
		myConfig = config;
		myConnection = conn;
		
		Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
		// 
		int height = screenSize.height * 4/5;
		int width = screenSize.width * 4/5;

		this.setSize(new Dimension(width, height));
		this.setLocation(screenSize.width/2-this.getSize().width/2, screenSize.height/2-this.getSize().height/2);
		
		
	//MENU
        JMenuBar bar = new JMenuBar();
        
        //Options
        JMenu menuConfig = new JMenu("Project");
 
        menuItemOpenProject = new JMenuItem("Open simulation project ...");
        menuItemOpenProject.addActionListener(this);
        menuConfig.add(menuItemOpenProject);
        
        menuItemNewProject = new JMenuItem("New simulation project ...");
        menuItemNewProject.addActionListener(this);
        menuConfig.add(menuItemNewProject);
        
        menuItemCopyProject = new JMenuItem("Copy current simulation project ...");
        menuItemCopyProject.addActionListener(this);
        menuConfig.add(menuItemCopyProject);
        
        menuConfig.add(new JSeparator());
        
        menuItemLoadConfig = new JMenuItem("Load configuration ...");
        menuItemLoadConfig.addActionListener(this);
        menuConfig.add(menuItemLoadConfig);
        
        menuItemSaveConfig = new JMenuItem("Save configuration ...");
        menuItemSaveConfig.addActionListener(this);
        menuConfig.add(menuItemSaveConfig);
        
        menuConfig.add(new JSeparator());

        menuItemClose = new JMenuItem("Close");
        menuItemClose.addActionListener(this);
        menuConfig.add(menuItemClose);

        //Import
        JMenu menuImport = new JMenu("Import to database");   
        
        menuItemImportGecrosXnm = new JMenuItem("Import GECROS from XN3 .xnm ...");
        menuItemImportGecrosXnm.addActionListener(this);
        menuImport.add(menuItemImportGecrosXnm);
        
        menuItemImportGecrosIni = new JMenuItem("Import GECROS from XN5 .ini ...");
        menuItemImportGecrosIni.addActionListener(this);
        menuImport.add(menuItemImportGecrosIni);
        

        menuImport.add(new JSeparator());

        menuItemImportSoilParamXndXnm = new JMenuItem("Import soil physical parameters from .xnm and .xnd...");
        menuItemImportSoilParamXndXnm.addActionListener(this);
        menuImport.add(menuItemImportSoilParamXndXnm);
        
        menuItemImportSoilStartXndXnm = new JMenuItem("Import soil start values from .xnm and .xnd ...");
        menuItemImportSoilStartXndXnm.addActionListener(this);
        menuImport.add(menuItemImportSoilStartXndXnm);

        menuItemImportDaisyXnm = new JMenuItem("Import DAISY turnover parameters from .xnm ...");
        menuItemImportDaisyXnm.addActionListener(this);
        menuImport.add(menuItemImportDaisyXnm);
        
        menuItemImportSomPoolsXnm = new JMenuItem("Import DAISY SOM pools initialization from .xnm ...");
        menuItemImportSomPoolsXnm.addActionListener(this);
        menuImport.add(menuItemImportSomPoolsXnm);

        menuImport.add(new JSeparator());

        menuItemImportSoilDataAll = new JMenuItem("Import all soil related parameters from .xnm and .xnd ...");
        menuItemImportSoilDataAll.addActionListener(this);
        menuImport.add(menuItemImportSoilDataAll);

        menuImport.add(new JSeparator());

        menuItemImportManagementXnd = new JMenuItem("Import crop management from .xnd ...");
        menuItemImportManagementXnd.addActionListener(this);
        menuImport.add(menuItemImportManagementXnd);
        
        menuImport.add(new JSeparator());

        menuItemImportFull = new JMenuItem("Import complete .xnd/.xnm (except GECROS) ...");
        menuItemImportFull.addActionListener(this);
        menuImport.add(menuItemImportFull);

        menuImport.add(new JSeparator());

        menuItemImportManECloop = new JMenuItem("Import all EC .xnds) ...");
        menuItemImportManECloop.addActionListener(this);
        menuImport.add(menuItemImportManECloop);
        
        //Export
        JMenu menuExport = new JMenu("Export");
        
        menuItemExportXn5Input = new JMenuItem("Generate XN5 or XN5-MPMAS dataset ...");
        menuItemExportXn5Input.addActionListener(this);
        menuExport.add(menuItemExportXn5Input);
        
        
        menuExport.add(new JSeparator());
        
        menuItemExportGecrosIni = new JMenuItem("Export XN5 gecros.ini ...");
        menuItemExportGecrosIni.addActionListener(this);
        menuExport.add(menuItemExportGecrosIni);

        menuItemExportCellCfg	= new JMenuItem("Export XN5 cell configurations ...");
        menuItemExportCellCfg.addActionListener(this);
        menuExport.add(menuItemExportCellCfg);
        

        menuItemExportCellManagement = new JMenuItem("Export XN5 cell plant management ...");
        menuItemExportCellManagement.addActionListener(this);
        menuExport.add(menuItemExportCellManagement);
        
        menuExport.add(new JSeparator());
        menuItemExportWeatherFiles = new JMenuItem("Export weather files ...");
        menuItemExportWeatherFiles.addActionListener(this);
        menuExport.add(menuItemExportWeatherFiles);

        
        
        //put everything together
        bar.add(menuConfig);
        bar.add(menuImport);
        bar.add(menuExport);
        
        this.setJMenuBar(bar);
        
        this.setLayout( new javax.swing.BoxLayout(
        		getContentPane(), javax.swing.BoxLayout.Y_AXIS ) );//Important: getContentPane() not this for JFrame()
        //this.setBorder(new EmptyBorder(10, 10, 10, 10));
        
        //PROJECT INFO
        if (myConfig.currentProjectId == -1)
		{
			currentProject = null;
		}
		else {
			ResultSet rs =  myConnection.getSimulationProjectInfo(myConfig.currentProjectId);
			try {
				rs.next();
				currentProject = new simProject(rs);
				rs.close();
			}
			catch(Exception e) {
				e.printStackTrace();
			}
		}
        
        
        JPanel top = new JPanel();
        top.setLayout( new java.awt.GridLayout(4,3 )  );
        top.setBorder(new EmptyBorder(5, 50, 5, 50));
        
		JLabel header = new JLabel("Simulation project: ");
		JLabel descHeader = new JLabel("Description: ");
		JLabel authorHeader = new JLabel("Author: ");
		JLabel projTypeHeader = new JLabel("Project type: ");
		labelCode = new JLabel();
		labelDescription = new JLabel();
		labelAuthorCode = new JLabel();
		labelProjType = new JLabel();
		
        Font fontCode = new Font("Courier", Font.BOLD,12);
        //set font for JLabel
        labelCode.setFont(fontCode);

        Font fontDescription = new Font("Courier", Font.ITALIC,12);
        labelDescription.setFont(fontDescription);
        labelAuthorCode.setFont(fontDescription);
		
		updateCurrentProjectInfo();
		
		top.add(header);
		top.add(labelCode);
		top.add(new JLabel());
		top.add(descHeader);
		top.add(labelDescription);
		top.add(new JLabel());
		top.add(authorHeader);
		top.add(labelAuthorCode);
		top.add(new JLabel());
		top.add(projTypeHeader);
		top.add(labelProjType);
		top.add(new JLabel());

		this.add(top);
		this.add(new JSeparator());
   //TABS     
        JTabbedPane tabpane = new JTabbedPane
                (JTabbedPane.TOP,JTabbedPane.SCROLL_TAB_LAYOUT );

        //Project View Tab
        //panelCurrentProject = new XnParameterProjectPanel(this);
        //tabpane.add("SimulationProject", panelCurrentProject);

        panelProjectGeneral = new XnParameterProjectPanel(this);
        tabpane.add("General", panelProjectGeneral);

        panelGecros = new XnParameterGecrosPanel(this);
        tabpane.add("GECROS", panelGecros);

        panelSoils = new XnParameterSoilPanel(this);
        tabpane.add("Soils", panelSoils);
        
        panelManagements = new XnParameterManagementPanel (this);
        tabpane.add("Crop management", panelManagements);
        
        panelXpn = new XnParameterXpnPanel(this);
        tabpane.add("XPN", panelXpn);

        panelXpi = new XnParameterXpiPanel(this);
        tabpane.add("XPI", panelXpi);
        
        panelOutputVarlist = new XnParameterContentEditorPanel(this, "_varlist.var", "simulation_projects_output_varlist", "varlistText");
        tabpane.add("VARLIST", panelOutputVarlist);
        
        panelSpecialOutputDef = new XnParameterContentEditorPanel(this, "_special_output_def.ini", "simulation_projects_special_output", "specialOutputText");
        tabpane.add("SPECIAL_OUTPUT", panelSpecialOutputDef);
        
        panelBEMSCouplingIni = new XnParameterCouplingPanel(this);
        tabpane.add("XnMpmasCoupling.ini", panelBEMSCouplingIni);
        
        
        this.add(tabpane);
	}


	
    public void actionPerformed(ActionEvent e){
    	
    	if (e.getSource() == menuItemClose)
    	{
    		this.dispose();
    	}
    	else if (e.getSource() == menuItemSaveConfig)
    	{
    		saveConfig();
    	}
    	else if (e.getSource() == menuItemLoadConfig)
    	{
    		myConfig.loadConfig();
    	}
    	if (e.getSource() == menuItemNewProject)
    	{
    		createNewProjectDialog dlg = new createNewProjectDialog();
    		simProject sp = dlg.showDialog(myConfig, myConnection);
			switchProject(sp);
    	}
    	else if (e.getSource() == menuItemCopyProject)
    	{
    		simProject temp = currentProject;
    		
    		createNewProjectDialog dlg = new createNewProjectDialog();
    		simProject newP = dlg.showDialog(myConfig, myConnection);
    		
    		boolean rc = myConnection.copyProjectSettings(temp.simulation_project_id, newP.simulation_project_id);
    		
			if (!rc) {
				JOptionPane.showMessageDialog(null, "Error: Copying project information failed.");
			}
			//done here to make sure alternative cells and management tables are updated after project settings have been copied
			ResultSet rs =  myConnection.getSimulationProjectInfo(newP.simulation_project_id);
			try {
				rs.next();
				newP.bems_cells_management_table = rs.getString("bems_cells_management_table");
				newP.xn5_cells_table = rs.getString("xn5_cells_table");
				rs.close();
			}
			catch(Exception exc) {
				exc.printStackTrace();
			}
			
			switchProject(newP);
    	}	    	
    	
    	else if (e.getSource() == menuItemOpenProject)
    	{
    		openProjectDialog dlg = new openProjectDialog();
    		simProject sp = dlg.showDialog(myConfig, myConnection);
			switchProject(sp);
    	}
    	
    	
    	
    	else if (e.getSource() == menuItemExportXn5Input)
    	{
    		String dir = askForOutputDirectory();
    		if (dir.equals("")) {
    			return;
    		}
    		String prefix = dir + "/"+ getCurrentProjectName();
    		
    		if (currentProject.project_type >= 1) { 
    			createXnmpmasCouplingIni(prefix,getCurrentProjectName(), currentProject.project_type, dir); 
    			// includes check for consistency of XN and coupling timing
    		}
    		
    		createGecrosIni(prefix);
    		createSoilCells(prefix);
    		createManagementFiles(prefix);
    		createXpn(prefix,getCurrentProjectName());
    		createXpis(prefix,getCurrentProjectName());
    		createOutputVarlist(prefix,getCurrentProjectName());
    		createSpecialOutputDef(prefix,getCurrentProjectName());

    		createWeatherFilesForProject(dir, getCurrentProjectName(), currentProject.project_type );
    		System.out.println("Finished.");

    	}  
    	else if (e.getSource() == menuItemExportGecrosIni)
    	{
    		String prefix = askForOutputDirectory();
    		if (prefix.equals("")) {
    			return;
    		}
    		prefix += "/"+ getCurrentProjectName();
    		createGecrosIni(prefix);
    	}  
    	else if (e.getSource() == menuItemExportCellCfg)
    	{
    		String prefix = askForOutputDirectory();
    		if (prefix.equals("")) {
    			return;
    		}
    		prefix += "/"+ getCurrentProjectName();
    		createSoilCells(prefix);
    	} 
    	else if (e.getSource() == menuItemExportCellManagement)
    	{
    		String prefix = askForOutputDirectory();
    		if (prefix.equals("")) {
    			return;
    		}
    		prefix += "/"+ getCurrentProjectName();
    		createManagementFiles(prefix);
    	} 
    	else if (e.getSource() == menuItemExportWeatherFiles)
    	{
        	ExportWeatherDialog dlg = new ExportWeatherDialog(this);
        	int rc = dlg.showDialog();
        	if (rc < 0 )
        	{
        		return;
        	}
    	} 
    	else if (e.getSource() == menuItemImportGecrosXnm)
    	{
    		importGecrosXnm();
    	}
    	else if (e.getSource() == menuItemImportGecrosIni)
    	{
    		importGecrosIni();
    	}
    	else if (e.getSource() == menuItemImportDaisyXnm)
    	{
    		importDaisyXnm();
    	}
    	else if (e.getSource() == menuItemImportSomPoolsXnm)
    	{
    		importSomPoolsXnm();
    	}
    	else if (e.getSource() == menuItemImportSoilStartXndXnm)
    	{
    		importSoilStartXndXnm();
    	} 
    	else if (e.getSource() == menuItemImportSoilParamXndXnm)
    	{
    		importSoilParamXndXnm();
    	} 
    	else if (e.getSource() == menuItemImportSoilDataAll)
    	{
    		importSoilDataAllXndXnm();
    	} 
    	else if (e.getSource() == menuItemImportManagementXnd)
    	{
    		importManagementXnd();
    	} 
    	else if (e.getSource() == menuItemImportFull)
    	{
    		importFullXnmXnd();
    	} 
    	else if (e.getSource() == menuItemImportManECloop)
    	{
    		myConnection.loop_importCropManagementFromECs();
    	}

    }
    public void switchProject(simProject sp) {
    	currentProject = sp;
    	myConfig.currentProjectId = sp.simulation_project_id;
    	refreshAll();
    }
    public void refreshAll() {
    	updateCurrentProjectInfo();
    	panelProjectGeneral.refresh();
    	panelGecros.refresh();
    	panelSoils.refresh();
    	panelManagements.refresh();
    	panelXpn.refresh();
    	panelXpi.refresh();
    	panelSpecialOutputDef.refresh();
    	panelOutputVarlist.refresh();
    	panelBEMSCouplingIni.refresh();
    	if(currentProject != null  && currentProject.project_type >= 1) {
    		panelBEMSCouplingIni.setEnabled(true);
    	}
    	else if(currentProject != null  || currentProject.project_type == 0) {
    		panelBEMSCouplingIni.setEnabled(false);
    	}
    		
    }
    public void updateCurrentProjectInfo() {
    	
    	
		if (currentProject == null )
		 {		labelDescription.setText("");
		 		labelCode.setText("No project selected");
		 		labelAuthorCode.setText("");
		 		labelProjType.setText("");
		 }
		else {
				labelDescription.setText(currentProject.simulation_project_description);
			 	labelCode.setText(currentProject.simulation_project_code);
			 	labelAuthorCode.setText(currentProject.author_id);		
			 	labelProjType.setText(String.valueOf(currentProject.project_type));		

		}
    }
    
    private void saveConfig () {
		JFileChooser chooser = new JFileChooser();
        int rc = chooser.showSaveDialog(null);
        
        /* Abfrage, ob auf "Öffnen" geklickt wurde */
        if(rc == JFileChooser.APPROVE_OPTION)
        {
        	String configFile =   chooser.getSelectedFile().getAbsolutePath();
    		myConfig.saveConfig(configFile);
        }
    }
    
    private void importGecrosXnm () {
    	ImportGecrosDialog dlg = new ImportGecrosDialog(this);
    	int rc = dlg.showDialog();
    	if (rc < 0 )
    	{
    		return;
    	}
    }
    private void importGecrosIni () {
    	ImportGecrosDialogIni dlg = new ImportGecrosDialogIni(this);
    	int rc = dlg.showDialog();
    	if (rc < 0 )
    	{
    		return;
    	}
    }
    private void importDaisyXnm () {
    	ImportDaisyDialog dlg = new ImportDaisyDialog(this);
    	int rc = dlg.showDialog();
    	if (rc < 0 )
    	{
    		return;
    	}
    }
    private void importSomPoolsXnm () {
    	ImportXn3Dialog dlg = new ImportXn3Dialog(this, xn3ImportType.daisyPools);
    	int rc = dlg.showDialog();
    	if (rc < 0 )
    	{
    		return;
    	}
    }
    private void importSoilParamXndXnm () {
    	ImportXn3Dialog dlg = new ImportXn3Dialog(this, xn3ImportType.soilParam);
    	int rc = dlg.showDialog();
    	if (rc < 0 )
    	{
    		return;
    	}
    }
    private void importSoilStartXndXnm () {
    	ImportXn3Dialog dlg = new ImportXn3Dialog(this, xn3ImportType.soilStart);
    	int rc = dlg.showDialog();
    	if (rc < 0 )
    	{
    		return;
    	}
    }
    private void importSoilDataAllXndXnm () {
    	ImportXn3Dialog dlg = new ImportXn3Dialog(this, xn3ImportType.soilAll);
    	int rc = dlg.showDialog();
    	if (rc < 0 )
    	{
    		return;
    	}
    }
    private void importManagementXnd () {
    	ImportXn3Dialog dlg = new ImportXn3Dialog(this, xn3ImportType.Management);
    	int rc = dlg.showDialog();
    	if (rc < 0 )
    	{
    		return;
    	}
    }
    private void importFullXnmXnd () {
    	ImportXn3Dialog dlg = new ImportXn3Dialog(this, xn3ImportType.fullXnmXnd);
    	int rc = dlg.showDialog();
    	if (rc < 0 )
    	{
    		return;
    	}
    }

    public String getCurrentProjectName() {
    	return currentProject.simulation_project_code;
    }
    

    
    private void createGecrosIni (String xn5FilePrefixGecros) {

    	boolean createGridAssoc = false;
    	if (currentProject.project_type >= 1){
    		createGridAssoc = true;
    	}

    	ResultSet rs = myConnection.getGecrosParameterizationSelectionOnly(myConfig.currentProjectId);
    	try {
    		
    		
    		
    		rs.beforeFirst();
	    	while(rs.next())
	    	{
	    		
	    		int param_id = rs.getInt("plant_param_id");
	    		String crop_code = rs.getString("crop_code");
	    		String variety = rs.getString("variety");
	    		String crop_name = rs.getString("crop_name");
	    		System.out.println("Writing " + param_id + " " + crop_code + " " + variety + "...");
	    		myConnection.writeXn5GecrosParametersFile(xn5FilePrefixGecros, param_id, crop_code, variety,crop_name);	
	    	}
    		if (createGridAssoc) {
    			String bemsFileNameGridForCrops = xn5FilePrefixGecros + "_XnCropCodeToGrid.dat";
        		PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(bemsFileNameGridForCrops)));
        		rs.beforeFirst();
    	    	while(rs.next())
    	    	{
    	    		out.println(rs.getString("crop_code") + "\t" + "0");
    	    	}
    			out.close();
    		}
    		rs.close();
    	}
    	catch(Exception e)
    	{
    		JOptionPane.showMessageDialog(null, "Error: " + e.getMessage());
    		return;
    	}
    }
    private void createSoilCells (String xn5FilePrefix) {
    	myConnection.writeXn5CellCfgFiles(xn5FilePrefix, myConfig.currentProjectId, currentProject.xn5_cells_table);
    }
    private void createManagementFiles (String xn5FilePrefix) {
    	if (currentProject.project_type == 0) {
    		myConnection.writeXn5CropManagementFiles(xn5FilePrefix, myConfig.currentProjectId);
    	}
    	else if (currentProject.project_type > 0){
    		int adaptive = 0;
    		try {
    			ResultSet rs = myConnection.getGeneralInfoForProject(myConfig.currentProjectId);
    			if ( rs.first())
    				adaptive = rs.getInt("adaptive");
    			rs.close();
    		}
    		catch (Exception e) {
    			e.printStackTrace();
    		}
    		
        	myConnection.writeBemsCropManagementInfo(xn5FilePrefix, myConfig.currentProjectId, adaptive);
        	myConnection.writeXn5CropManagementFileDummies(xn5FilePrefix, myConfig.currentProjectId, currentProject.xn5_cells_table);
        	
        	if (currentProject.project_type == 1) {
        		myConnection.writeBEMSCropManagementMaps(xn5FilePrefix, myConfig.currentProjectId, currentProject.xn5_cells_table, currentProject.bems_cells_management_table);
        	}
    	}
    }
    private void createXpn (String xn5FilePrefix, String projectName) {
    	myConnection.writeXn5XpnFile(xn5FilePrefix, myConfig.currentProjectId, projectName, currentProject.xn5_cells_table);
    }
    private void createXpis (String xn5FilePrefix, String projectName) {
    	myConnection.writeXn5XpiFiles(xn5FilePrefix, myConfig.currentProjectId, projectName, currentProject.project_type, currentProject.xn5_cells_table);
    }
    private void createOutputVarlist (String xn5FilePrefix, String projectName) {
    	myConnection.writeXn5GenericContentFile(xn5FilePrefix, myConfig.currentProjectId, projectName, panelOutputVarlist.table, panelOutputVarlist.contentColumn, panelOutputVarlist.title);
    }
    private void createSpecialOutputDef (String xn5FilePrefix, String projectName) {
    	myConnection.writeXn5GenericContentFile(xn5FilePrefix, myConfig.currentProjectId, projectName, panelSpecialOutputDef.table, panelSpecialOutputDef.contentColumn, panelSpecialOutputDef.title);
    }
    private void createXnmpmasCouplingIni (String xn5FilePrefix, String projectName, int projectType, String projectDir) {
    	myConnection.writeXnMpmasCouplingIni(xn5FilePrefix, myConfig.currentProjectId, projectName, projectType, projectDir);
    }
    private void createWeatherFilesForProject(String directory,  String projectName, int projectType) {
    	myConnection.writeWeatherFilesForProject(directory, myConfig.currentProjectId, projectName, projectType, currentProject.xn5_cells_table);
    }
	/*private List<cropToExport> selectCropsToExport() {
		List<cropToExport> list = ;
		return list;
	}*/
    public String askForOutputDirectory () {
    	JFileChooser chooser = new JFileChooser(); 
        chooser.setCurrentDirectory(new java.io.File(myConfig.lastOutputDirectory));
        chooser.setDialogTitle("Choose directory");
        chooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
        chooser.setAcceptAllFileFilterUsed(false); // disable the "All files" option.
           
        if (chooser.showSaveDialog(this) == JFileChooser.APPROVE_OPTION) { 
         /* System.out.println("getCurrentDirectory(): " 
             +  chooser.getCurrentDirectory());
          System.out.println("getSelectedFile() : " 
             +  chooser.getSelectedFile());*/
          	myConfig.lastOutputDirectory = chooser.getSelectedFile().getAbsolutePath();
          	return myConfig.lastOutputDirectory;
          }
        else {
            return "";
        }
    }
    public static void initColumnSizes(JTable table) {
    	TableModel model = (TableModel)table.getModel();
        TableColumn column = null;
        Component comp = null;
        int headerWidth = 0;
        int cellWidth = 0;
        
        TableCellRenderer headerRenderer =
            table.getTableHeader().getDefaultRenderer();

        for (int i = 0; i < model.getColumnCount(); i++) {
            column = table.getColumnModel().getColumn(i);
            column.sizeWidthToFit();
        }
    }

}
//Tabs
class obsolete_XnParameterProjectPanel extends JPanel implements ActionListener {

	private XnParameterToolMainWindow myMainWindow ;
	
	private JList simProjectListView;
	private simProjectListModel simProjectList;
	private JButton buttonSwitchProject;
	private JButton buttonNewProject;
	private JButton buttonCopyProject;
	private simProject currentProject;
	
	private JLabel labelDescription;
	private JLabel labelCode;
	private JLabel labelAuthorCode;
	
	public obsolete_XnParameterProjectPanel(XnParameterToolMainWindow mainWindow) {
		super();
		myMainWindow = mainWindow;
		
		simProjectList = new simProjectListModel();
		labelDescription = new JLabel();
		labelCode = new JLabel();
		labelAuthorCode = new JLabel();
		updateProjectListView();
		

		
		this.setLayout( new javax.swing.BoxLayout(
                this, javax.swing.BoxLayout.Y_AXIS ) );
        this.setBorder(new EmptyBorder(10, 10, 10, 10));
		
        JPanel top = new JPanel();
        top.setLayout( new java.awt.GridLayout(3,3 )  );
        top.setBorder(new EmptyBorder(5, 50, 5, 50));
        
		JLabel header = new JLabel("Simulation project: ");
		JLabel descHeader = new JLabel("Description: ");
		JLabel authorHeader = new JLabel("Author: ");
		top.add(header);
		top.add(labelCode);
		buttonCopyProject = new JButton("Copy project...");
		buttonCopyProject.addActionListener(this);
		top.add(buttonCopyProject);
		top.add(descHeader);
		top.add(labelDescription);
		top.add(new JLabel());
		top.add(authorHeader);
		top.add(labelAuthorCode);
		top.add(new JLabel());

		this.add(top);
		this.add(new JSeparator());
		
		
        JPanel mid = new JPanel();
        mid.setLayout( new javax.swing.BoxLayout(
                mid, javax.swing.BoxLayout.Y_AXIS ) );
        mid.setBorder(new EmptyBorder(10, 10, 10, 10));
		
		buttonNewProject = new JButton("New project...");
		buttonNewProject.addActionListener(this);
		buttonSwitchProject = new JButton("Switch project");
		buttonSwitchProject.addActionListener(this);

		simProjectListView = new JList(simProjectList);
		simProjectListView.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
		//simProjectListView.setSize(200,200);
		
		JScrollPane plistScroll = new JScrollPane(simProjectListView, ScrollPaneLayout.VERTICAL_SCROLLBAR_ALWAYS,ScrollPaneLayout.HORIZONTAL_SCROLLBAR_AS_NEEDED);
		mid.add(plistScroll);
		this.add(mid);
		
	    JPanel bottom = new JPanel();
	    bottom.setLayout( new javax.swing.BoxLayout(
			   bottom, javax.swing.BoxLayout.X_AXIS ) );
	    bottom.setBorder(new EmptyBorder(10, 10, 10, 10));
		bottom.add(buttonSwitchProject);
		bottom.add(buttonNewProject);
		this.add(bottom);
		
	}

	 public void actionPerformed(ActionEvent e){
	    	
	    	if (e.getSource() == buttonNewProject)
	    	{
	    		createNewProject();
	    	}
	    	else if (e.getSource() == buttonCopyProject)
	    	{
	    		simProject temp = currentProject;
	    		createNewProject();
	    		
	    		boolean rc = myMainWindow.myConnection.copyProjectSettings(temp.simulation_project_id, currentProject.simulation_project_id);
	    		
    			if (!rc) {
    				JOptionPane.showMessageDialog(null, "Error: Copying project information failed.");
    			}
    			//done here to make sure alternative cells and management tables are updated after project settings have been copied
    			ResultSet rs =  myMainWindow.myConnection.getSimulationProjectInfo(currentProject.simulation_project_id);
    			try {
    				rs.next();
    				currentProject.bems_cells_management_table = rs.getString("bems_cells_management_table");
    				currentProject.xn5_cells_table = rs.getString("xn5_cells_table");
    				rs.close();
    			}
    			catch(Exception exc) {
    				exc.printStackTrace();
    			}
    			
    			myMainWindow.switchProject(currentProject);
	    	}	    	
	    	
	    	else if (e.getSource() == buttonSwitchProject)
	    	{
	    		int index = simProjectListView.getSelectedIndex();
	    		if (index > -1)
	    		{	currentProject = simProjectList.getProjectAt(index);
	    			myMainWindow.switchProject(currentProject);
	    		}	    		
	    		updateProjectListView();
	    	}
	 }
    
	
	public void updateProjectListView () {
		
		try {
			simProjectList.clear();
		
			ResultSet projectList = myMainWindow.myConnection.getListOfSimulationProjects();
			
			while(projectList.next()) 
			{
				//System.out.println(projectList.getString("simulation_project_code"));

				simProjectList.addElement(new simProject(projectList));
			}
			projectList.beforeFirst();
			projectList.close();
			
			if (myMainWindow.myConfig.currentProjectId == -1)
			{
				currentProject = null;
			}
			else {
				currentProject = simProjectList.getProjectAt(simProjectList.getListIndexOfProject(myMainWindow.myConfig.currentProjectId ));
			}
			
			if (currentProject == null )
			 {	labelDescription.setText("");
			 	labelCode.setText("No project selected");
			 	labelAuthorCode.setText("");
			 }
			else {
					labelDescription.setText(currentProject.simulation_project_description);
				 	labelCode.setText(currentProject.simulation_project_code);
				 	labelAuthorCode.setText(currentProject.author_id);
				 
			}

		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}
	private void 	createNewProject() {
		createNewProjectDialog dlg = new createNewProjectDialog();
	
		simProject sp = dlg.showDialog(myMainWindow.myConfig, myMainWindow.myConnection);
		currentProject = sp;
		myMainWindow.switchProject(currentProject);
			    		
		updateProjectListView();
	}
	public String getCurrentProjectName()
	{
		return currentProject.simulation_project_code;
	}
}


class simProjectListModel extends DefaultListModel {

	
	public Object getElementAt(int index) {
		simProject sp = (simProject) super.getElementAt(index);
		try {
			return  sp.simulation_project_code;
		}
		catch(Exception e)
		{
			e.printStackTrace();
			return null;
		}
    }
	public simProject getProjectAt(int index) {
		simProject sp = (simProject) super.getElementAt(index);
		try {
			return  sp;
		}
		catch(Exception e)
		{
			e.printStackTrace();
			return null;
		}
    }
	public int getListIndexOfProject(int spId) {
		for (int index = 0; index < super.getSize(); ++index)
		{ simProject t = (simProject) super.getElementAt(index);
			if (t.simulation_project_id == spId )
				return index;
		}
		return -1;
    }
	
}
class columnListModel extends DefaultListModel {

	
	public Object getElementAt(int index) {
		String sp = (String) super.getElementAt(index);
		try {
			return  sp;
		}
		catch(Exception e)
		{
			e.printStackTrace();
			return null;
		}
    }
	public String getColumnAt(int index) {
		String sp = (String) super.getElementAt(index);
		try {
			return  sp;
		}
		catch(Exception e)
		{
			e.printStackTrace();
			return null;
		}
    }
	public int getListIndexOfColumn(String column) {
		for (int index = 0; index < super.getSize(); ++index)
		{ String t = (String) super.getElementAt(index);
			if (t.equals(column) )
				return index;
		}
		return -1;
    }
	
}


class XnParameterProjectPanel extends JPanel implements ActionListener, RowSetListener {
	private XnParameterToolMainWindow myMainWindow ;
	
	private JTable projectGeneralTable;
	private CachedRowSet projectGeneral;
	private ProjectGeneralTableModel projectGeneralTM;
	
	public XnParameterProjectPanel(XnParameterToolMainWindow mainWindow) {
		super();
		
		myMainWindow = mainWindow;
	    
		JPanel panel = new JPanel();
	
	    try {
	    	projectGeneral =  RowSetProvider.newFactory().createCachedRowSet();
	    	
	    	projectGeneralTable = new JTable();
	    	refreshTableInfo();
	    }
	    catch(Exception e) {
	    	e.printStackTrace();
	    }


	    JScrollPane tableViewScrollPaneLeft = new JScrollPane (projectGeneralTable, 
	            ScrollPaneConstants.VERTICAL_SCROLLBAR_AS_NEEDED,
	            ScrollPaneConstants.HORIZONTAL_SCROLLBAR_ALWAYS);
		Dimension preferredSize = new Dimension(800,400);
		tableViewScrollPaneLeft.setPreferredSize(preferredSize);
	    panel.add(tableViewScrollPaneLeft, java.awt.BorderLayout.CENTER);

	    this.add(panel);
	    
	}
	private void refreshTableInfo() {
	    
	    try {
	    	if (myMainWindow.myConfig.currentProjectId > -1) { 
	    	
	    		projectGeneral.close();
		    	ResultSet rs2 = myMainWindow.myConnection.getGeneralInfoForProject(myMainWindow.myConfig.currentProjectId);
		    	projectGeneral.populate(rs2);
	    		
	
		    	projectGeneralTM =
		                  new ProjectGeneralTableModel (projectGeneral,  myMainWindow.myConnection, myMainWindow.myConfig );
		    	projectGeneralTM.addEventHandlersToRowSet(this);
		    	projectGeneralTable.setModel(projectGeneralTM);
				
		    	projectGeneralTable.setAutoResizeMode(JTable.AUTO_RESIZE_OFF);
		    	projectGeneralTable.setPreferredScrollableViewportSize(new Dimension(700,600));
		    	//projectGeneralTable.setPreferredSize(new Dimension(600,600));

		    	XnParameterToolMainWindow.initColumnSizes(projectGeneralTable);
	    	}
	    	
	    }
		catch (Exception e) {
	    	  e.printStackTrace();
	    }
	}
	public void refresh() {
		try {
			/*gecrosSelection.beforeFirst();
			while (gecrosSelection.next())
			{	gecrosSelection.refreshRow();
			}*/
			refreshTableInfo();
			projectGeneralTable.setModel(projectGeneralTM);
		}
		catch (Exception e) {
	    	  e.printStackTrace();
	      }
	}
    public void actionPerformed(ActionEvent e){
    	
    	if (e.getSource() == myMainWindow)
    	{
    		//this.dispose();
    	}
    }

    public void rowSetChanged(RowSetEvent event) {  
    	
        CachedRowSet currentRowSet = projectGeneralTM.myRowSet;

        try {
          currentRowSet.moveToCurrentRow();
          projectGeneralTM =
            new ProjectGeneralTableModel(projectGeneralTM.getRowSet(), myMainWindow.myConnection, myMainWindow.myConfig);
          projectGeneralTable.setModel(projectGeneralTM);

        } 
        catch (Exception e) {
      	  e.printStackTrace();
        }
    }

    public void rowChanged(RowSetEvent event) {

      CachedRowSet currentRowSet = projectGeneralTM.myRowSet;

      try {
        currentRowSet.moveToCurrentRow();
        projectGeneralTM =
          new ProjectGeneralTableModel(projectGeneralTM.getRowSet(), myMainWindow.myConnection, myMainWindow.myConfig);
        projectGeneralTable.setModel(projectGeneralTM);

      } 
      catch (Exception e) {
    	  e.printStackTrace();
      }
    }

    public void cursorMoved(RowSetEvent event) {  }
    
}

class ProjectGeneralTableModel implements TableModel {
	
	  XnDatabaseConnection myConnection;
	  XnParameterToolConfig myConfig;
	  CachedRowSet myRowSet; 
	  ResultSetMetaData metadata;
	  int numcols, numrows;

	  public CachedRowSet getRowSet() {
	    return myRowSet;
	  }
	  public ProjectGeneralTableModel() throws SQLException {

		    myRowSet = null;
		    metadata = null;
		    numcols = 0;
		    numrows = 0;
		    
		  }


	  public ProjectGeneralTableModel(CachedRowSet _rowSet, XnDatabaseConnection conn, XnParameterToolConfig config) throws SQLException {

	    myRowSet = _rowSet;
	    metadata = myRowSet.getMetaData();
	    numcols = metadata.getColumnCount();
	    myConnection = conn;
	    myConfig = config;
	    
	    
	    // Retrieve the number of rows.
	    myRowSet.beforeFirst();
	    numrows = 0;
	    while (myRowSet.next()) {
	     numrows++;
	    }
	    myRowSet.beforeFirst();
	  }

	  public void addEventHandlersToRowSet(RowSetListener listener) {
		  myRowSet.addRowSetListener(listener);
	  }

	  public void close() {
	    try {
	      myRowSet.getStatement().close();
	    } 
	    catch (SQLException e) {
	      e.printStackTrace();
	    }
	  }

	  protected void finalize() {
	    close();
	  }

	  public int getColumnCount() {
	    return numcols;
	  }

	  public int getRowCount() {
	    return numrows;
	  }

	  public String getColumnName(int column) {
	    try {
	      return metadata.getColumnLabel(column + 1);
	    } 
	    catch (SQLException e) {
	      return e.toString();
	    }
	  }

	  public Class getColumnClass(int column) {
		  
		switch (column) {	
			case 6:
			case 8:
					return Double.class;
			case 9:
			case 10:
			case 13:
			case 14:
					return String.class;
			default:
					return Integer.class;
		}
	    
	  }



	  public Object getValueAt(int rowIndex, int columnIndex) {

	    try {
	      myRowSet.absolute(rowIndex + 1);
	      
	  	  if ((columnIndex == 6) || (columnIndex == 8))
	  	  {
	  		   Double objD = myRowSet.getDouble(columnIndex + 1);
	  		   return objD;
	  		   
	  	  }
	  	  else if ((columnIndex == 9) || (columnIndex == 10) || (columnIndex == 13)  || (columnIndex == 14))
	  	  {
	  		   String objS = myRowSet.getString(columnIndex + 1);
	  		   return objS;
	  		   
	  	  }
	  	  else
	  	  {	  Integer objI = myRowSet.getInt(columnIndex + 1);	  
	  		  return objI;
	  	  }  
	    } 
	    catch (SQLException e) {
	       e.toString();
	       return null;
	    }
	  }

	  public boolean isCellEditable(int rowIndex, int columnIndex) {
					return true;
	  }
	
	  public void setValueAt(Object value, int row, int col) {
		  	  
			try {
	
					myRowSet.absolute(row + 1);
			  
					String oldV = (String) myRowSet.getString(col+1);
					
					String newV = String.valueOf(value);
					myRowSet.updateString(col +1, newV);
					myRowSet.updateRow();
					myRowSet.absolute(row + 1);

					if (! myConnection.updateGeneralInfoForProject(myConfig.currentProjectId,
							myRowSet.getString("startYear"), myRowSet.getString("endYear"), 
							myRowSet.getString("startMonth"), myRowSet.getString("startDay"), 
							myRowSet.getString("endMonth"), myRowSet.getString("endDay"),
							myRowSet.getString("plotSize"), myRowSet.getString("adaptive"), myRowSet.getString("max_daily_precip"),
							myRowSet.getString("xn5_cells_table"), myRowSet.getString("bems_cells_management_table"),
							myRowSet.getString("elevationCorrectionType"),	myRowSet.getString("elevationCorrectionClassSize"),
							myRowSet.getString("elevationInfoTableWeatherCells"),myRowSet.getString("co2_table"), myRowSet.getString("kc_param_id")
							
							)) {
	
		    			JOptionPane.showMessageDialog(null, "Error: Changing general simulation project info failed.");
		    			//rollback changes in view if database failed
						myRowSet.absolute(row + 1);
						myRowSet.updateString(col +1, oldV);
						myRowSet.updateRow();
	
		    			
		    			return;
					}
			}							
			catch(Exception e) {			
				e.printStackTrace();
			}
	  }

	  public void addTableModelListener(TableModelListener l) {
	  }

	  public void removeTableModelListener(TableModelListener l) {
	  }

	
}



class XnParameterGecrosPanel extends JPanel implements ActionListener, RowSetListener {
	private XnParameterToolMainWindow myMainWindow ;

	//private JTable gecrosTable;
	//private CachedRowSet gecrosData;
	//private GecrosTableModel gecrosTM;
	
	private JTable gecrosSelectionTable;	
	private CachedRowSet gecrosSelection;
	private GecrosSelectionTableModel gecrosSelectionTM;
	
	//Database View Tab
	public XnParameterGecrosPanel(XnParameterToolMainWindow mainWindow) {
		super();
		
		myMainWindow = mainWindow;
		   
		JPanel panel = new JPanel();
		panel.setLayout( new java.awt.BorderLayout() );
		
	    //JTable tableOverview = new JTable();
	    
	    try {
	    	gecrosSelection =  RowSetProvider.newFactory().createCachedRowSet();
	    	
	    	gecrosSelectionTable = new JTable();
	    	refreshSelection();
		    
		    
	    }
	    catch(Exception e) {
	    	e.printStackTrace();
	    }

	    JScrollPane tableViewScrollPaneLeft = new JScrollPane (gecrosSelectionTable, 
	            ScrollPaneConstants.VERTICAL_SCROLLBAR_AS_NEEDED,
	            ScrollPaneConstants.HORIZONTAL_SCROLLBAR_AS_NEEDED);
	    
		Dimension preferredSize = new Dimension(800,400);
		tableViewScrollPaneLeft.setPreferredSize(preferredSize);

	    panel.add(tableViewScrollPaneLeft, java.awt.BorderLayout.CENTER);

	    this.add(panel);
	    
	}
	private void refreshSelection() {
	    
	    try {
	    	if (myMainWindow.myConfig.currentProjectId > -1) { 
	    	
	    		gecrosSelection.close();
		    	ResultSet rs2 = myMainWindow.myConnection.getGecrosParameterizationSelection(myMainWindow.myConfig.currentProjectId);
		    	gecrosSelection.populate(rs2);
	    		
	    		//String qu = myMainWindow.myConnection.getGecrosParameterizationSelectionString(myMainWindow.myConfig.currentProjectId);
	    		//gecrosSelection.setCommand(qu);
	    		//gecrosSelection.execute(myMainWindow.myConnection.getConnectionObject());
	    				
		        gecrosSelectionTM =
		                  new GecrosSelectionTableModel(gecrosSelection,  myMainWindow.myConnection, myMainWindow.myConfig );
			    gecrosSelectionTM.addEventHandlersToRowSet(this);
				gecrosSelectionTable.setModel(gecrosSelectionTM);
				
				gecrosSelectionTable.setAutoResizeMode(JTable.AUTO_RESIZE_OFF);
				gecrosSelectionTable.setPreferredScrollableViewportSize(new Dimension(600,600));
				//gecrosSelectionTable.setPreferredSize(new Dimension(600,600));

				XnParameterToolMainWindow.initColumnSizes(gecrosSelectionTable);
	    	}
	    	
	    }
		catch (Exception e) {
	    	  e.printStackTrace();
	    }
	}
	public void refresh() {
		try {
			/*gecrosSelection.beforeFirst();
			while (gecrosSelection.next())
			{	gecrosSelection.refreshRow();
			}*/
			refreshSelection();
			gecrosSelectionTable.setModel(gecrosSelectionTM);
		}
		catch (Exception e) {
	    	  e.printStackTrace();
	      }
	}
    public void actionPerformed(ActionEvent e){
    	
    	if (e.getSource() == myMainWindow)
    	{
    		//this.dispose();
    	}
    }

    public void rowSetChanged(RowSetEvent event) {  
    	
        CachedRowSet currentRowSet = gecrosSelectionTM.myRowSet;

        try {
          currentRowSet.moveToCurrentRow();
          gecrosSelectionTM =
            new GecrosSelectionTableModel(gecrosSelectionTM.getRowSet(), myMainWindow.myConnection, myMainWindow.myConfig);
          gecrosSelectionTable.setModel(gecrosSelectionTM);

        } 
        catch (Exception e) {
      	  e.printStackTrace();
        }
    }

    	
    

    public void rowChanged(RowSetEvent event) {

      CachedRowSet currentRowSet = gecrosSelectionTM.myRowSet;

      try {
        currentRowSet.moveToCurrentRow();
        gecrosSelectionTM =
          new GecrosSelectionTableModel(gecrosSelectionTM.getRowSet(), myMainWindow.myConnection, myMainWindow.myConfig);
        gecrosSelectionTable.setModel(gecrosSelectionTM);

      } 
      catch (Exception e) {
    	  e.printStackTrace();
      }
    }

    public void cursorMoved(RowSetEvent event) {  }
    
    /*
     * This method picks good column sizes.
     * If all column heads are wider than the column's cells'
     * contents, then you can just use column.sizeWidthToFit().
     */

    }
class GecrosTableModel implements TableModel {
	
	  CachedRowSet myRowSet; 
	  ResultSetMetaData metadata;
	  int numcols, numrows;

	  public CachedRowSet getRowSet() {
	    return myRowSet;
	  }


	  public GecrosTableModel(CachedRowSet _rowSet) throws SQLException {

	    myRowSet = _rowSet;
	    metadata = myRowSet.getMetaData();
	    numcols = metadata.getColumnCount();

	    // Retrieve the number of rows.
	    myRowSet.beforeFirst();
	    numrows = 0;
	    while (myRowSet.next()) {
	     numrows++;
	    }
	    myRowSet.beforeFirst();
	  }

	  public void addEventHandlersToRowSet(RowSetListener listener) {
		  myRowSet.addRowSetListener(listener);
	  }



	  public void close() {
	    try {
	      myRowSet.getStatement().close();
	    } 
	    catch (SQLException e) {
	      e.printStackTrace();
	    }
	  }

	  protected void finalize() {
	    close();
	  }

	  public int getColumnCount() {
	    return numcols;
	  }

	  public int getRowCount() {
	    return numrows;
	  }


	  public String getColumnName(int column) {
	    try {
	      return metadata.getColumnLabel(column + 1);
	    } 
	    catch (SQLException e) {
	      return e.toString();
	    }
	  }

	  public Class getColumnClass(int column) {
	    return String.class;
	  }



	  public Object getValueAt(int rowIndex, int columnIndex) {
		    try {
			      myRowSet.absolute(rowIndex + 1);
			      Object obj = this.myRowSet.getObject(columnIndex + 1);
			      if (obj == null)
			        return null;
			      else
			        return obj.toString();
			    } 
			    catch (SQLException e) {
			      return e.toString();
			    }

	  }

	  public boolean isCellEditable(int rowIndex, int columnIndex) {
	    return false;
	  }
	
	  public void setValueAt(Object value, int row, int column) {
		  }

	  public void addTableModelListener(TableModelListener l) {
	  }

	  public void removeTableModelListener(TableModelListener l) {
	  }

	
}
class GecrosSelectionTableModel implements TableModel {
	
	  XnDatabaseConnection myConnection;
	  XnParameterToolConfig myConfig;
	  CachedRowSet myRowSet; 
	  ResultSetMetaData metadata;
	  int numcols, numrows;

	  public CachedRowSet getRowSet() {
	    return myRowSet;
	  }
	  public GecrosSelectionTableModel() throws SQLException {

		    myRowSet = null;
		    metadata = null;
		    numcols = 0;
		    numrows = 0;
		    
		  }


	  public GecrosSelectionTableModel(CachedRowSet _rowSet, XnDatabaseConnection conn, XnParameterToolConfig config) throws SQLException {

	    myRowSet = _rowSet;
	    metadata = myRowSet.getMetaData();
	    numcols = metadata.getColumnCount();
	    myConnection = conn;
	    myConfig = config;
	    
	    
	    // Retrieve the number of rows.
	    myRowSet.beforeFirst();
	    numrows = 0;
	    while (myRowSet.next()) {
	     numrows++;
	    }
	    myRowSet.beforeFirst();
	  }

	  public void addEventHandlersToRowSet(RowSetListener listener) {
		  myRowSet.addRowSetListener(listener);
	  }



	  public void close() {
	    try {
	      myRowSet.getStatement().close();
	    } 
	    catch (SQLException e) {
	      e.printStackTrace();
	    }
	  }

	  protected void finalize() {
	    close();
	  }

	  public int getColumnCount() {
	    return numcols;
	  }

	  public int getRowCount() {
	    return numrows;
	  }


	  public String getColumnName(int column) {
	    try {
	      return metadata.getColumnLabel(column + 1);
	    } 
	    catch (SQLException e) {
	      return e.toString();
	    }
	  }

	  public Class getColumnClass(int column) {
		  
		switch (column) {
			case 0:
					return Boolean.class;
					
			default:
					return String.class;
		}
	    
	  }



	  public Object getValueAt(int rowIndex, int columnIndex) {

	    try {
	      myRowSet.absolute(rowIndex + 1);
	      
    	  if (columnIndex == 0)
    	  {
    		  return myRowSet.getBoolean(columnIndex + 1);
    	  }
    	  else
    	  {	  Object obj = myRowSet.getObject(columnIndex + 1);	  
    		  return obj;
    	  }  
	    } 
	    catch (SQLException e) {
	       e.toString();
	       return null;
	    }
	  }

	  public boolean isCellEditable(int rowIndex, int columnIndex) {
			switch (columnIndex) {
				case 0:
					return true;
				default:
					return false;
			}

	  }
	
	  public void setValueAt(Object value, int row, int col) {
		  
		  
			try {
				if(col == 0)
				{
					myRowSet.absolute(row + 1);
			  
					int old = myRowSet.getInt("selected");
					int newV =  (Boolean) value ? 1: 0;
					
					if (myConnection.updateSelectedGecrosPlantsForProject(myConfig.currentProjectId,
							myRowSet.getInt("plant_param_id"), myRowSet.getString("crop_code"), 
							myRowSet.getString("variety"), newV )) {
					
						//updated table view only if change in database succeeded
						myRowSet.updateInt(col +1, newV);
						myRowSet.updateRow();
					}
					else {
		    			JOptionPane.showMessageDialog(null, "Error: Changing GECROS crop selection failed.");
		    			
		    			return;
					}
					
					
				}
			  
			}
			catch(Exception e)
			{
				e.printStackTrace();
			}
		  }

	  public void addTableModelListener(TableModelListener l) {
	  }

	  public void removeTableModelListener(TableModelListener l) {
	  }

	
}


class XnParameterSoilPanel extends JPanel implements ActionListener, RowSetListener {
	private XnParameterToolMainWindow myMainWindow ;
	
	private JTable soilTable;
	private CachedRowSet soils;
	private SoilsTableModel soilsTM;
	
	private JButton buttonAddRow;
	private JButton buttonDeleteRow;
	private JTextField  tNewX;
	private JTextField  tNewY;
	
	private JButton buttonUpdate;
	private JTextArea updateSETstring;
	private JTextArea updateWHEREstring;
	
	
	
	public XnParameterSoilPanel(XnParameterToolMainWindow mainWindow) {
		super();
		
		myMainWindow = mainWindow;
	    
		JPanel panel = new JPanel();
		panel.setLayout( new java.awt.BorderLayout() );

		soilTable = new JTable();
		



	    JScrollPane tableViewScrollPaneLeft = new JScrollPane (soilTable, 
	            ScrollPaneConstants.VERTICAL_SCROLLBAR_ALWAYS,
	            ScrollPaneConstants.HORIZONTAL_SCROLLBAR_AS_NEEDED);

		Dimension preferredSize = new Dimension(800,400);
		tableViewScrollPaneLeft.setPreferredSize(preferredSize);
	    
	    panel.add(tableViewScrollPaneLeft, java.awt.BorderLayout.CENTER);
	    
	    JPanel lpanel = new JPanel();
	    lpanel.setLayout( new BoxLayout(lpanel, BoxLayout.PAGE_AXIS) );
	    
	    lpanel.add(new JLabel("New Cell:"));
	    
	    JPanel xPanel = new JPanel();
	    lpanel.add(xPanel);
	    xPanel.add(new JLabel("x:"));
	    tNewX = new JTextField("",5);
	    tNewX.setMaximumSize(  tNewX.getPreferredSize() );
	    xPanel.add(tNewX);
	    xPanel.setMaximumSize(new Dimension(Integer.MAX_VALUE, tNewX.getPreferredSize().height+5));
	    
	    JPanel yPanel = new JPanel();
	    lpanel.add(yPanel);
	    yPanel.add(new JLabel("y:"));
	    tNewY = new JTextField("",5);
	    tNewY.setMaximumSize( tNewY.getPreferredSize() );
	    yPanel.add(tNewY);
	    yPanel.setMaximumSize(new Dimension(Integer.MAX_VALUE, tNewY.getPreferredSize().height +5 ));

	    
	    buttonAddRow = new JButton("Add cell");
	    buttonAddRow.addActionListener(this);
	    lpanel.add(buttonAddRow);
	    
	    buttonDeleteRow = new JButton("Delete selected cell");
	    buttonDeleteRow.addActionListener(this);
	    lpanel.add(buttonDeleteRow);
	    
	    lpanel.add(new JLabel());
	    lpanel.add(new JSeparator(SwingConstants.HORIZONTAL));
	    lpanel.add(new JLabel("UPDATE ... SET "));
	    updateSETstring = new JTextArea();
	    JScrollPane tableViewScrollPaneLeft2 = new JScrollPane (updateSETstring, 
	            ScrollPaneConstants.VERTICAL_SCROLLBAR_ALWAYS,
	            ScrollPaneConstants.HORIZONTAL_SCROLLBAR_ALWAYS);
	    Dimension preferredSize2 = new Dimension(100,200);
		tableViewScrollPaneLeft2.setPreferredSize(preferredSize2);
		updateSETstring.setText("");
		updateSETstring.setEditable(true);
		lpanel.add(tableViewScrollPaneLeft2);

	    lpanel.add(new JLabel("WHERE "));
	    updateWHEREstring = new JTextArea();
	    JScrollPane tableViewScrollPaneLeft3 = new JScrollPane (updateWHEREstring, 
	            ScrollPaneConstants.VERTICAL_SCROLLBAR_ALWAYS,
	            ScrollPaneConstants.HORIZONTAL_SCROLLBAR_ALWAYS);
	    Dimension preferredSize3 = new Dimension(100,200);
		tableViewScrollPaneLeft3.setPreferredSize(preferredSize3);
		updateWHEREstring.setText("");
		updateWHEREstring.setEditable(true);
		lpanel.add(tableViewScrollPaneLeft3);
		
	    
	    buttonUpdate = new JButton("Update");
	    buttonUpdate.addActionListener(this);
	    lpanel.add(buttonUpdate);
	    
	    panel.add(lpanel, java.awt.BorderLayout.WEST);
		
	    
	    
	    try {
	    	soils =  RowSetProvider.newFactory().createCachedRowSet();
	    	
	    	
	    	refreshTableInfo();
	    }
	    catch(Exception e) {
	    	e.printStackTrace();
	    }
	    
	    
	    this.add(panel);
	    
	}
	private void refreshTableInfo() {
	    
	    try {
	    	if (myMainWindow.myConfig.currentProjectId > -1) { 
	    	
	    		soils.close();
	    		
	    		boolean editable = true;
	    	
	    		String cellsTableName = myMainWindow.currentProject.xn5_cells_table;
	    		if (  cellsTableName.equals("simulation_projects_xn5_cells"))  {
	    			buttonAddRow.setEnabled(true);
	    			buttonDeleteRow.setEnabled(true);
	    			buttonUpdate.setEnabled(true);

	    			editable = true;

	    		}
	    		else if( myMainWindow.myConnection.checkTableIsNotView("for1695_expertN", cellsTableName)) {
	    			editable = true;
	    			buttonAddRow.setEnabled(false);
	    			buttonDeleteRow.setEnabled(false);
	    			buttonUpdate.setEnabled(true);

	    		}
	    		else {
	    			editable = false;
	    			buttonAddRow.setEnabled(false);
	    			buttonDeleteRow.setEnabled(false);
	    			buttonUpdate.setEnabled(false);

	    		}
	    		
		    	ResultSet rs2 = myMainWindow.myConnection.getSoilInfoForProject(myMainWindow.myConfig.currentProjectId, cellsTableName);
		    
		    	
		    	soils.populate(rs2);
	    		
	
		    	soilsTM = new SoilsTableModel (soils,  myMainWindow.myConnection, myMainWindow.myConfig, editable );
		    	soilsTM.addEventHandlersToRowSet(this);
		    	soilTable.setModel(soilsTM);
				
		    	soilTable.setAutoResizeMode(JTable.AUTO_RESIZE_OFF);
		    	soilTable.setPreferredScrollableViewportSize(new Dimension(1000,600));
		    	//soilTable.setPreferredSize(new Dimension(1000,600));

		    	XnParameterToolMainWindow.initColumnSizes(soilTable);
	    	}
	    	
	    }
		catch (Exception e) {
	    	  e.printStackTrace();
	    }
	}
	public void refresh() {
		try {
			/*gecrosSelection.beforeFirst();
			while (gecrosSelection.next())
			{	gecrosSelection.refreshRow();
			}*/
			refreshTableInfo();
			soilTable.setModel(soilsTM);
			
 
		}
		catch (Exception e) {
	    	  e.printStackTrace();
	      }
	}
    public void actionPerformed(ActionEvent e){
      	if (e.getSource() == buttonAddRow) {
      		if (soilsTM.getRowCount() > 0) {
      			myMainWindow.myConnection.addCell(myMainWindow.myConfig.currentProjectId,Integer.valueOf(tNewX.getText()),Integer.valueOf(tNewY.getText()));
      		}
      		else {
      			myMainWindow.myConnection.addCellForProject(myMainWindow.myConfig.currentProjectId,Integer.valueOf(tNewX.getText()),Integer.valueOf(tNewY.getText())
      					,"", -1,-1,-1,-1,0,0,0,"S",0,0.0,0.0,1,"$<$PROJECT_PATH/$PROJECT_NAME_$REG_STR_climate.csv$>", "", -1);
      		}
      		refresh();
      	}
      	else if (e.getSource() == buttonDeleteRow) {
      		try {
	      		soilsTM.myRowSet.moveToCurrentRow();
	      		int x = soilsTM.myRowSet.getInt("xn5_cell_x");
	      		int y = soilsTM.myRowSet.getInt("xn5_cell_y");
	      		myMainWindow.myConnection.deleteCell(myMainWindow.myConfig.currentProjectId,x,y);
      		}
      		catch(Exception ex) {
      			ex.printStackTrace();
      		}
      		
      		myMainWindow.refreshAll();
      	}
      	else if (e.getSource() == buttonUpdate) {
      		try {
	      		myMainWindow.myConnection.userUpdateSoilsTable(myMainWindow.currentProject.xn5_cells_table, myMainWindow.currentProject.simulation_project_id,updateSETstring.getText(),updateWHEREstring.getText() );
      		}
      		catch(Exception ex) {
      			ex.printStackTrace();
      		}
      		
      		myMainWindow.refreshAll();
      	}
    }

    public void rowSetChanged(RowSetEvent event) {  
    	
        CachedRowSet currentRowSet = soilsTM.myRowSet;

        try {
          currentRowSet.moveToCurrentRow();
          soilsTM =
            new SoilsTableModel(soilsTM.getRowSet(), myMainWindow.myConnection, myMainWindow.myConfig, true);
          soilTable.setModel(soilsTM);

        } 
        catch (Exception e) {
      	  e.printStackTrace();
        }
    }

    public void rowChanged(RowSetEvent event) {

      CachedRowSet currentRowSet = soilsTM.myRowSet;

      try {
        currentRowSet.moveToCurrentRow();
        soilsTM =
          new SoilsTableModel(soilsTM.getRowSet(), myMainWindow.myConnection, myMainWindow.myConfig, true);
        soilTable.setModel(soilsTM);

      } 
      catch (Exception e) {
    	  e.printStackTrace();
      }
    }

    public void cursorMoved(RowSetEvent event) {  }
    
}

class SoilsTableModel implements TableModel {
	
	  XnDatabaseConnection myConnection;
	  XnParameterToolConfig myConfig;
	  CachedRowSet myRowSet; 
	  ResultSetMetaData metadata;
	  int numcols, numrows;
	  boolean editable;

	  public CachedRowSet getRowSet() {
	    return myRowSet;
	  }
	  public SoilsTableModel() throws SQLException {

		    myRowSet = null;
		    metadata = null;
		    numcols = 0;
		    numrows = 0;
		    editable = true;
		  }


	  public SoilsTableModel(CachedRowSet _rowSet, XnDatabaseConnection conn, XnParameterToolConfig config, boolean editable_) throws SQLException {

	    myRowSet = _rowSet;
	    metadata = myRowSet.getMetaData();
	    numcols = metadata.getColumnCount();
	    myConnection = conn;
	    myConfig = config;
	    editable = editable_;
	    
	    // Retrieve the number of rows.
	    myRowSet.beforeFirst();
	    numrows = 0;
	    while (myRowSet.next()) {
	     numrows++;
	    }
	    myRowSet.beforeFirst();
	  }

	  public void addEventHandlersToRowSet(RowSetListener listener) {
		  myRowSet.addRowSetListener(listener);
	  }

	  public void close() {
	    try {
	      myRowSet.getStatement().close();
	    } 
	    catch (SQLException e) {
	      e.printStackTrace();
	    }
	  }

	  protected void finalize() {
	    close();
	  }

	  public int getColumnCount() {
	    return numcols;
	  }

	  public int getRowCount() {
	    return numrows;
	  }

	  public String getColumnName(int column) {
	    try {
	      return metadata.getColumnLabel(column + 1);
	    } 
	    catch (SQLException e) {
	      return e.toString();
	    }
	  }

	  public Class getColumnClass(int column) {
		  
		switch (column) {
			case 2:
			case 10:
			case 15:
			case 16:
					return String.class;
			case 7:
			case 8:
			case 9:
			case 11:
			case 12:
			case 13:
					return Double.class;
			default:
					return Integer.class;
		}
	    
	  }

	  public Object getValueAt(int rowIndex, int columnIndex) {

	    try {
	      myRowSet.absolute(rowIndex + 1);
	      
			switch (columnIndex) {
				case 2:
				case 10:
				case 15:
				case 16:
			  		  return myRowSet.getString(columnIndex + 1);
				case 7:
				case 8:
				case 9:
				case 11:
				case 12:
				case 13:
			  		  return myRowSet.getDouble(columnIndex + 1);
				default:
			  		  return myRowSet.getInt(columnIndex + 1);
			}
		} 
	    catch (SQLException e) {
	       e.toString();
	       return null;
	    }
	  }

	  public boolean isCellEditable(int rowIndex, int columnIndex) {
		  
		  if (editable) {
		  		if (columnIndex < 2)
		  			return false;
		  		else
					return true;
		  }
		  else
		  {
			  return false;
		  }
	  }
	
	  public void setValueAt(Object value, int row, int col) {
		  	  
			try {
	
					myRowSet.absolute(row + 1);
			  
					String oldV = (String) myRowSet.getString(col+1);
					
					String newV = String.valueOf(value);
					myRowSet.updateString(col +1, newV);
					myRowSet.updateRow();
					myRowSet.absolute(row + 1);

					if (! myConnection.updateCellForProject( myConfig.currentProjectId,
							myRowSet.getInt("xn5_cell_x"), myRowSet.getInt("xn5_cell_y"), myRowSet.getString("profileID"), 
							myRowSet.getInt("soil_param_id"), myRowSet.getInt("daisy_param_id"), 
							myRowSet.getInt("sompools_param_id"), myRowSet.getInt("soilinit_param_id"),
							myRowSet.getDouble("lat"), myRowSet.getDouble("lon"),
							myRowSet.getDouble("alt"), myRowSet.getString("exposition")
							, myRowSet.getDouble("inclination"), myRowSet.getDouble("AveYearTemp"), myRowSet.getDouble("MonthTempAmp")
							, myRowSet.getInt("leachn_param_id"),myRowSet.getString("climate_file"),
							myRowSet.getString("weather_table_name"),
							myRowSet.getInt("weather_station_id")
							)) {
	
		    			JOptionPane.showMessageDialog(null, "Error: Changing cell failed.");
		    			//rollback changes in view if database failed
						myRowSet.absolute(row + 1);
						myRowSet.updateString(col +1, oldV);
						myRowSet.updateRow();
		    			
		    			return;
					}
			}							
			catch(Exception e) {			
				e.printStackTrace();
			}
	  }
	  public void addTableModelListener(TableModelListener l) {
	  }

	  public void removeTableModelListener(TableModelListener l) {
	  }

	
}



class XnParameterCouplingPanel extends JPanel implements ActionListener, RowSetListener {
	private XnParameterToolMainWindow myMainWindow ;
	
	private JTable projectCouplingTable;
	private CachedRowSet projectCoupling;
	private ProjectCouplingTableModel projectCouplingTM;
	
	public XnParameterCouplingPanel(XnParameterToolMainWindow mainWindow) {
		super();
		
		myMainWindow = mainWindow;
	    
		JPanel panel = new JPanel();
	
	    try {
	    	projectCoupling =  RowSetProvider.newFactory().createCachedRowSet();
	    	
	    	projectCouplingTable = new JTable();
	    	refreshTableInfo();
	    }
	    catch(Exception e) {
	    	e.printStackTrace();
	    }


	    JScrollPane tableViewScrollPaneLeft = new JScrollPane (projectCouplingTable, 
	            ScrollPaneConstants.VERTICAL_SCROLLBAR_AS_NEEDED,
	            ScrollPaneConstants.HORIZONTAL_SCROLLBAR_ALWAYS);

	    panel.add(tableViewScrollPaneLeft, java.awt.BorderLayout.CENTER);

	    this.add(panel);
	    
	}
	private void refreshTableInfo() {
	    
	    try {
	    	if (myMainWindow.myConfig.currentProjectId > -1) { 
	    	
	    		projectCoupling.close();
		    	ResultSet rs2 = myMainWindow.myConnection.getCouplingInfoForProject(myMainWindow.myConfig.currentProjectId);
		    	projectCoupling.populate(rs2);
	    		
	
		    	projectCouplingTM =
		                  new ProjectCouplingTableModel (projectCoupling,  myMainWindow.myConnection, myMainWindow.myConfig );
		    	projectCouplingTM.addEventHandlersToRowSet(this);
		    	projectCouplingTable.setModel(projectCouplingTM);
				
		    	projectCouplingTable.setAutoResizeMode(JTable.AUTO_RESIZE_OFF);
		    	projectCouplingTable.setPreferredScrollableViewportSize(new Dimension(1000,600));
		    	//projectGeneralTable.setPreferredSize(new Dimension(600,600));

		    	XnParameterToolMainWindow.initColumnSizes(projectCouplingTable);
	    	}
	    	
	    }
		catch (Exception e) {
	    	  e.printStackTrace();
	    }
	}
	public void refresh() {
		try {
			/*gecrosSelection.beforeFirst();
			while (gecrosSelection.next())
			{	gecrosSelection.refreshRow();
			}*/
			refreshTableInfo();
			projectCouplingTable.setModel(projectCouplingTM);
		}
		catch (Exception e) {
	    	  e.printStackTrace();
	      }
	}
    public void actionPerformed(ActionEvent e){
    	
    	if (e.getSource() == myMainWindow)
    	{
    		//this.dispose();
    	}
    }

    public void rowSetChanged(RowSetEvent event) {  
    	
        CachedRowSet currentRowSet = projectCouplingTM.myRowSet;

        try {
          currentRowSet.moveToCurrentRow();
          projectCouplingTM =
            new ProjectCouplingTableModel(projectCouplingTM.getRowSet(), myMainWindow.myConnection, myMainWindow.myConfig);
          projectCouplingTable.setModel(projectCouplingTM);

        } 
        catch (Exception e) {
      	  e.printStackTrace();
        }
    }

    public void rowChanged(RowSetEvent event) {

      CachedRowSet currentRowSet = projectCouplingTM.myRowSet;

      try {
        currentRowSet.moveToCurrentRow();
        projectCouplingTM =
          new ProjectCouplingTableModel(projectCouplingTM.getRowSet(), myMainWindow.myConnection, myMainWindow.myConfig);
        projectCouplingTable.setModel(projectCouplingTM);

      } 
      catch (Exception e) {
    	  e.printStackTrace();
      }
    }

    public void cursorMoved(RowSetEvent event) {  }
    
}

class ProjectCouplingTableModel implements TableModel {
	
	  XnDatabaseConnection myConnection;
	  XnParameterToolConfig myConfig;
	  CachedRowSet myRowSet; 
	  ResultSetMetaData metadata;
	  int numcols, numrows;

	  public CachedRowSet getRowSet() {
	    return myRowSet;
	  }
	  public ProjectCouplingTableModel() throws SQLException {

		    myRowSet = null;
		    metadata = null;
		    numcols = 0;
		    numrows = 0;
		    
		  }


	  public ProjectCouplingTableModel(CachedRowSet _rowSet, XnDatabaseConnection conn, XnParameterToolConfig config) throws SQLException {

	    myRowSet = _rowSet;
	    metadata = myRowSet.getMetaData();
	    numcols = metadata.getColumnCount();
	    myConnection = conn;
	    myConfig = config;
	    
	    
	    // Retrieve the number of rows.
	    myRowSet.beforeFirst();
	    numrows = 0;
	    while (myRowSet.next()) {
	     numrows++;
	    }
	    myRowSet.beforeFirst();
	  }

	  public void addEventHandlersToRowSet(RowSetListener listener) {
		  myRowSet.addRowSetListener(listener);
	  }

	  public void close() {
	    try {
	      myRowSet.getStatement().close();
	    } 
	    catch (SQLException e) {
	      e.printStackTrace();
	    }
	  }

	  protected void finalize() {
	    close();
	  }

	  public int getColumnCount() {
	    return numcols;
	  }

	  public int getRowCount() {
	    return numrows;
	  }

	  public String getColumnName(int column) {
	    try {
	      return metadata.getColumnLabel(column + 1);
	    } 
	    catch (SQLException e) {
	      return e.toString();
	    }
	  }

	  public Class getColumnClass(int column) {
		  
		switch (column) {
			case 6:
			case 7:
			case 8:
					return String.class;
			case 9:
			case 10:
			case 11:
					return Double.class;
			default:
					return Integer.class;
		}
	    
	  }



	  public Object getValueAt(int rowIndex, int columnIndex) {

	    try {
	      myRowSet.absolute(rowIndex + 1);
	      
	  	  if (columnIndex >= 6 &&  columnIndex <= 8 )
	  	  {
	  		  return myRowSet.getString(columnIndex + 1);
	  	  }
	  	  else if (columnIndex >= 9 &&  columnIndex <= 11 )
	  	  {
	  		  return myRowSet.getDouble(columnIndex + 1);
	  	  }
	  	  else
	  	  {	  Integer objI = myRowSet.getInt(columnIndex + 1);	  
	  		  return objI;
	  	  }  
	    } 
	    catch (SQLException e) {
	       e.toString();
	       return null;
	    }
	  }

	  public boolean isCellEditable(int rowIndex, int columnIndex) {
					return true;
	  }
	
	  public void setValueAt(Object value, int row, int col) {
		  	  
			try {
	
					myRowSet.absolute(row + 1);
			  
					String oldV = (String) myRowSet.getString(col+1);
					
					String newV = String.valueOf(value);
					myRowSet.updateString(col +1, newV);
					myRowSet.updateRow();
					myRowSet.absolute(row + 1);

					if (! myConnection.updateCouplingInfoForProject(myConfig.currentProjectId,
							myRowSet.getString("firstSowingDay"), myRowSet.getString("firstSowingMonth"), 
							myRowSet.getString("firstSowingRelativeYear"), myRowSet.getString("lastHarvestDay"), 
							myRowSet.getString("lastHarvestMonth"), myRowSet.getString("lastHarvestRelativeYear"),
							myRowSet.getString("MPMAS_commandline"), myRowSet.getString("XN_basedir"), 
							myRowSet.getString("simulation_basedir"), myRowSet.getDouble("history_link_map_xllcorner"),
							myRowSet.getDouble("history_link_map_yllcorner"), myRowSet.getDouble("history_link_map_cellsize")
							)) {
	
		    			JOptionPane.showMessageDialog(null, "Error: Changing Coupling info failed.");
		    			//rollback changes in view if database failed
						myRowSet.absolute(row + 1);
						myRowSet.updateString(col +1, oldV);
						myRowSet.updateRow();
	
		    			
		    			return;
					}
			}							
			catch(Exception e) {			
				e.printStackTrace();
			}
	  }

	  public void addTableModelListener(TableModelListener l) {
	  }

	  public void removeTableModelListener(TableModelListener l) {
	  }

	
}






class XnParameterManagementPanel extends JPanel implements ActionListener, RowSetListener {
	private XnParameterToolMainWindow myMainWindow ;
	
	private JTable managementTable;
	private CachedRowSet managements;
	private ManagementsTableModel managementsTM;
	private JButton buttonAddRow;
	private JButton buttonDeleteRow;
	private JTextField tNewX;
	private JTextField tNewY;
	private JTextField tNewYear;
	private JTextField tNewPosition;
	private JLabel l3rdFieldLabel;
	private JLabel l4thFieldLabel;
	private JScrollPane tableViewScrollPaneLeft;
	
	public JCheckBox cbAdaptiveManagements;
	
	public XnParameterManagementPanel(XnParameterToolMainWindow mainWindow) {
		super();
		
		myMainWindow = mainWindow;
	    
		JPanel panel = new JPanel();
		panel.setLayout( new java.awt.BorderLayout() );

		//Note: the following fields are initialized at this point
		// because they are activated or deactivated in refreshTableInfo()
	    l3rdFieldLabel = new JLabel("Year");
	    l4thFieldLabel = new JLabel("Position");
	    tNewYear = new JTextField("",5);
	    tNewPosition = new JTextField("",5);
		
	    try {
	    	managements =  RowSetProvider.newFactory().createCachedRowSet();
	    	
	    	managementTable = new JTable();
	    	refreshTableInfo();
	    }
	    catch(Exception e) {
	    	e.printStackTrace();
	    }

    	tableViewScrollPaneLeft = new JScrollPane (managementTable, 
 	            ScrollPaneConstants.VERTICAL_SCROLLBAR_ALWAYS,
 	            ScrollPaneConstants.HORIZONTAL_SCROLLBAR_AS_NEEDED);
 	    
 	    

 		Dimension preferredSize = new Dimension(800,600);
 		tableViewScrollPaneLeft.setPreferredSize(preferredSize);
	    
	    panel.add(tableViewScrollPaneLeft, java.awt.BorderLayout.CENTER);

	    JPanel lpanel = new JPanel();
	    lpanel.setLayout( new BoxLayout(lpanel, BoxLayout.PAGE_AXIS) );
	    
	    lpanel.add(new JLabel("New Cell:"));
	    
	    JPanel xPanel = new JPanel();
	    lpanel.add(xPanel);
	    xPanel.add(new JLabel("x:"));
	    tNewX = new JTextField("",5);
	    tNewX.setMaximumSize(  tNewX.getPreferredSize() );
	    xPanel.add(tNewX);
	    xPanel.setMaximumSize(new Dimension(Integer.MAX_VALUE, tNewX.getPreferredSize().height+5));
	    
	    JPanel yPanel = new JPanel();
	    lpanel.add(yPanel);
	    yPanel.add(new JLabel("y:"));
	    tNewY = new JTextField("",5);
	    tNewY.setMaximumSize( tNewY.getPreferredSize() );
	    yPanel.add(tNewY);
	    yPanel.setMaximumSize(new Dimension(Integer.MAX_VALUE, tNewY.getPreferredSize().height +5 ));

	    JPanel yearPanel = new JPanel();
	    lpanel.add(yearPanel);
	    yearPanel.add(l3rdFieldLabel);
	    tNewYear.setMaximumSize( tNewYear.getPreferredSize() );
	    yearPanel.add(tNewYear);
	    yearPanel.setMaximumSize(new Dimension(Integer.MAX_VALUE, tNewYear.getPreferredSize().height +5 ));

	    JPanel pPanel = new JPanel();
	    lpanel.add(pPanel);
	    pPanel.add(l4thFieldLabel);
	    tNewPosition.setMaximumSize( tNewPosition.getPreferredSize() );
	    pPanel.add(tNewPosition);
	    pPanel.setMaximumSize(new Dimension(Integer.MAX_VALUE, tNewPosition.getPreferredSize().height +5 ));
    
	    buttonAddRow = new JButton("Add row");
	    buttonAddRow.addActionListener(this);
	    lpanel.add(buttonAddRow);
	    
	    buttonDeleteRow = new JButton("Delete selected row");
	    buttonDeleteRow.addActionListener(this);
	    lpanel.add(buttonDeleteRow);
	    panel.add(lpanel, java.awt.BorderLayout.WEST);
	    
	    

	    
	    this.add(panel);
	    
	}
	private void refreshTableInfo() {
	    
	    try {

	    	if (myMainWindow.myConfig.currentProjectId > -1) { 
	    		
	    		
		    		managements.close();
		    		ResultSet rs2;
		    		boolean editable = true;
		    		if (myMainWindow.currentProject.project_type == 1) {
		    			rs2 = myMainWindow.myConnection.getManagementInfoForProjectBEMS(myMainWindow.myConfig.currentProjectId, myMainWindow.currentProject.bems_cells_management_table);
		    			l3rdFieldLabel.setEnabled(false);
		    			l4thFieldLabel.setEnabled(false);
		    			
		    			tNewYear.setEnabled(false);
		    			tNewPosition.setEnabled(false);
		    			if (! myMainWindow.currentProject.bems_cells_management_table.equals("simulation_projects_bems_cells_management"))
		    			{
		    				buttonAddRow.setEnabled(false);
		    				buttonDeleteRow.setEnabled(false);
			    			tNewX.setEnabled(false);
			    			tNewY.setEnabled(false);
		    				editable = false;
		    			}
		    			else {
		    				buttonAddRow.setEnabled(true);
		    				buttonDeleteRow.setEnabled(true);
		    				tNewX.setEnabled(true);
			    			tNewY.setEnabled(true);
		    			}
		    			
		    		}
		    		else {
		    			rs2 = myMainWindow.myConnection.getManagementInfoForProject(myMainWindow.myConfig.currentProjectId);
		    			l3rdFieldLabel.setEnabled(true);
		    			l4thFieldLabel.setEnabled(true);
		    			tNewYear.setEnabled(true);
		    			tNewPosition.setEnabled(true);
		    		}  		
		    		
			    	managements.populate(rs2);
		    		
			    	managementsTM =
			                  new ManagementsTableModel (managements,  myMainWindow.myConnection, myMainWindow.myConfig , myMainWindow.currentProject.project_type, editable);
			    	managementsTM.addEventHandlersToRowSet(this);
			    	managementTable.setModel(managementsTM);
					
			    	managementTable.setAutoResizeMode(JTable.AUTO_RESIZE_OFF);
			    	managementTable.setPreferredScrollableViewportSize(new Dimension(1000,600));
			    	//managementTable.setPreferredSize(new Dimension(1000,600));
	
			    	XnParameterToolMainWindow.initColumnSizes(managementTable);
	    		
			    	 
	    	}

	    	
	    }
		catch (Exception e) {
	    	  e.printStackTrace();
	    }
	}
	public void refresh() {
		try {
			/*gecrosSelection.beforeFirst();
			while (gecrosSelection.next())
			{	gecrosSelection.refreshRow();
			}*/

			refreshTableInfo();
			managementTable.setModel(managementsTM);
		}
		catch (Exception e) {
	    	  e.printStackTrace();
	      }
	}
    public void actionPerformed(ActionEvent e){
    	
      	if (e.getSource() == buttonAddRow) {
      		if (myMainWindow.currentProject.project_type == 1) {
	      		if (managementsTM.getRowCount() > 0) {
	      			myMainWindow.myConnection.addManagementRowBEMS(myMainWindow.myConfig.currentProjectId,Integer.valueOf(tNewX.getText()),Integer.valueOf(tNewY.getText()));
	      		}
	      		else {
	      			myMainWindow.myConnection.updateManagementForProjectBEMS(myMainWindow.myConfig.currentProjectId,Integer.valueOf(tNewX.getText()),Integer.valueOf(tNewY.getText())
	      					,-1, -1	);
	
	      		}
      		}
      		else {
	      		if (managementsTM.getRowCount() > 0) {
	      			myMainWindow.myConnection.addManagementRow(myMainWindow.myConfig.currentProjectId,Integer.valueOf(tNewX.getText()),Integer.valueOf(tNewY.getText()),Integer.valueOf(tNewYear.getText()), Integer.valueOf(tNewPosition.getText()) );
	      		}
	      		else {
	      			myMainWindow.myConnection.addManagementForProject(myMainWindow.myConfig.currentProjectId,Integer.valueOf(tNewX.getText()),Integer.valueOf(tNewY.getText()),Integer.valueOf(tNewYear.getText()), Integer.valueOf(tNewPosition.getText()) 
	      					,-1, "", ""	);
	
	      		}
      		}
      		refresh();
      	}
      	else if (e.getSource() == buttonDeleteRow) {
      		try {
      			managementsTM.myRowSet.moveToCurrentRow();
	      		int x = managementsTM.myRowSet.getInt("xn5_cell_x");
	      		int y = managementsTM.myRowSet.getInt("xn5_cell_y");
	      		
	      		if (myMainWindow.currentProject.project_type == 1) {
	      			myMainWindow.myConnection.deleteManagementBEMS(myMainWindow.myConfig.currentProjectId,x,y);
	      		}
	      		else {
	      			int year = managementsTM.myRowSet.getInt("year");
		      		int position = managementsTM.myRowSet.getInt("position");
	      			myMainWindow.myConnection.deleteManagementRow(myMainWindow.myConfig.currentProjectId,x,y, year, position);
	      		}
      		}
      		catch(Exception ex) {
      			ex.printStackTrace();
      		}
      		
      		refresh();
      	}
    }

    public void rowSetChanged(RowSetEvent event) {  
    	
        CachedRowSet currentRowSet = managementsTM.myRowSet;

        try {
          currentRowSet.moveToCurrentRow();
          managementsTM =
            new ManagementsTableModel(managementsTM.getRowSet(), myMainWindow.myConnection, myMainWindow.myConfig, managementsTM.getProjectType(),true);
          managementTable.setModel(managementsTM);

        } 
        catch (Exception e) {
      	  e.printStackTrace();
        }
    }

    public void rowChanged(RowSetEvent event) {

      CachedRowSet currentRowSet = managementsTM.myRowSet;

      try {
        currentRowSet.moveToCurrentRow();
        managementsTM =
          new ManagementsTableModel(managementsTM.getRowSet(), myMainWindow.myConnection, myMainWindow.myConfig, managementsTM.getProjectType(), true);
        managementTable.setModel(managementsTM);

      } 
      catch (Exception e) {
    	  e.printStackTrace();
      }
    }

    public void cursorMoved(RowSetEvent event) {  }
    
}

class ManagementsTableModel implements TableModel {
	
	  XnDatabaseConnection myConnection;
	  XnParameterToolConfig myConfig;
	  CachedRowSet myRowSet; 
	  ResultSetMetaData metadata;
	  int numcols, numrows, projectType;
	  boolean editable;

	  public CachedRowSet getRowSet() {
		  	return myRowSet;
	  }
	  public int getProjectType() {
		    return projectType;
	  }
	  public ManagementsTableModel() throws SQLException {

		    myRowSet = null;
		    metadata = null;
		    numcols = 0;
		    numrows = 0;
		    projectType = 0;
		    editable = true;
		    
	  }


	  public ManagementsTableModel(CachedRowSet _rowSet, XnDatabaseConnection conn, XnParameterToolConfig config, int project_type_, boolean editable_) throws SQLException {

	    myRowSet = _rowSet;
	    metadata = myRowSet.getMetaData();
	    numcols = metadata.getColumnCount();
	    myConnection = conn;
	    myConfig = config;
	    projectType = project_type_;
	    editable = editable_;
	    
	    // Retrieve the number of rows.
	    myRowSet.beforeFirst();
	    numrows = 0;
	    while (myRowSet.next()) {
	     numrows++;
	    }
	    myRowSet.beforeFirst();
	  }

	  public void addEventHandlersToRowSet(RowSetListener listener) {
		  myRowSet.addRowSetListener(listener);
	  }

	  public void close() {
	    try {
	      myRowSet.getStatement().close();
	    } 
	    catch (SQLException e) {
	      e.printStackTrace();
	    }
	  }

	  protected void finalize() {
	    close();
	  }

	  public int getColumnCount() {
	    return numcols;
	  }

	  public int getRowCount() {
	    return numrows;
	  }

	  public String getColumnName(int column) {
	    try {
	      return metadata.getColumnLabel(column + 1);
	    } 
	    catch (SQLException e) {
	      return e.toString();
	    }
	  }

	  public Class getColumnClass(int column) {
		  
		switch (column) {
			case 5:
			case 6:
					return String.class;	//Note: do not exist if projectType == 1			
			default:
					return Integer.class;
		}
	    
	  }

	  public Object getValueAt(int rowIndex, int columnIndex) {

	    try {
	      myRowSet.absolute(rowIndex + 1);
	      
			switch (columnIndex) {
				case 5:
				case 6:
			  		  return myRowSet.getString(columnIndex + 1);//Note: do not exist if projectType == 1
				default:
			  		  return myRowSet.getInt(columnIndex + 1);
			}
		} 
	    catch (SQLException e) {
	       e.toString();
	       return null;
	    }
	  }

	  public boolean isCellEditable(int rowIndex, int columnIndex) {
		  	if (editable) {
		  		if ( columnIndex < 2)
		  			return false;
		  		else
					return true;
		  	}
		  	else {
		  		return false;
		  	}
	  }
	
	  public void setValueAt(Object value, int row, int col) {
		  	  
			try {
	
					myRowSet.absolute(row + 1);
			  
					String oldV = (String) myRowSet.getString(col+1);
					
					String newV = String.valueOf(value);
					myRowSet.updateString(col +1, newV);
					myRowSet.updateRow();
					myRowSet.absolute(row + 1);

					boolean rtcod = false;
					if (projectType == 1) {
					
						rtcod =  myConnection.updateManagementForProjectBEMS(myConfig.currentProjectId,
								myRowSet.getInt("xn5_cell_x"), myRowSet.getInt("xn5_cell_y"),  
								myRowSet.getInt("crop_sequence_id"), 
								myRowSet.getInt("start_position") );
					}		
					else if (projectType == 0) {
						rtcod =  myConnection.updateManagementForProject(myConfig.currentProjectId,
							myRowSet.getInt("xn5_cell_x"), myRowSet.getInt("xn5_cell_y"),  
							myRowSet.getInt("year"), myRowSet.getInt("position"), 
							myRowSet.getInt("crop_management_id"), myRowSet.getString("crop_code"),
							myRowSet.getString("variety")		);
					}
					
					if (!rtcod) {
		    			JOptionPane.showMessageDialog(null, "Error: Changing cell failed.");
		    			//rollback changes in view if database failed
						myRowSet.absolute(row + 1);
						myRowSet.updateString(col +1, oldV);
						myRowSet.updateRow();

		    			return;
					}
			}							
			catch(Exception e) {			
				e.printStackTrace();
			}
	  }

	  public void addTableModelListener(TableModelListener l) {
	  }

	  public void removeTableModelListener(TableModelListener l) {
	  }

	
}


class XnParameterXpnPanel extends JPanel implements ActionListener {
	private XnParameterToolMainWindow myMainWindow ;
	
	private JTextArea xpnEditor;
	
	private JButton buttonSave;
	private JButton buttonRevert;
	private JButton buttonEdit;
	private JButton buttonCopy;
	
	public XnParameterXpnPanel(XnParameterToolMainWindow mainWindow) {
		super();
		
		myMainWindow = mainWindow;
	    
		JPanel panel = new JPanel();
		panel.setLayout( new java.awt.BorderLayout() );

		JLabel header = new JLabel("[model]");
		JLabel header2 = new JLabel("Note: The [model] section that specifies the model grid will be generated automatically from your entries in the SOILS tab.");
		JLabel header3 = new JLabel("Note: The  \"simulation start\" and \"simulation end \" entries will be generated automatically from your entries in the General tab.");
		JPanel tPanel = new JPanel();
		tPanel.setLayout( new BoxLayout(tPanel, BoxLayout.PAGE_AXIS) );
		tPanel.add(header);
		tPanel.add(header2);
		panel.add(tPanel, java.awt.BorderLayout.NORTH);
		xpnEditor = new JTextArea();
		reloadTextArea();
		xpnEditor.setEditable(false);


	    JScrollPane tableViewScrollPaneLeft = new JScrollPane (xpnEditor, 
	            ScrollPaneConstants.VERTICAL_SCROLLBAR_ALWAYS,
	            ScrollPaneConstants.HORIZONTAL_SCROLLBAR_ALWAYS);
	    Dimension preferredSize = new Dimension(800,400);
		tableViewScrollPaneLeft.setPreferredSize(preferredSize);

	    panel.add(tableViewScrollPaneLeft, java.awt.BorderLayout.CENTER);
	    
	    buttonSave = new JButton("Save");
	    buttonSave.addActionListener(this);
	    buttonSave.setEnabled(false);
	    
	    buttonRevert = new JButton("Revert");
	    buttonRevert.addActionListener(this);
	    buttonRevert.setEnabled(false);
	    
	    buttonEdit = new JButton("Edit");
	    buttonEdit.addActionListener(this);
	    
	    buttonCopy = new JButton("Copy from ...");
	    buttonCopy.addActionListener(this);
	    
	    JPanel lPanel = new JPanel();
	    lPanel.setLayout( new BoxLayout(lPanel, BoxLayout.PAGE_AXIS) );
	    lPanel.add(buttonEdit);
	    lPanel.add(buttonCopy);
	    lPanel.add(buttonSave);
	    lPanel.add(buttonRevert);
	    panel.add(lPanel, java.awt.BorderLayout.WEST);

	    this.add(panel);
	    
	}
	private void reloadTextArea() {
	    
	    try {
	    	if (myMainWindow.myConfig.currentProjectId > -1) { 
	    	
		    	ResultSet rs2 = myMainWindow.myConnection.getXpnContentForProject(myMainWindow.myConfig.currentProjectId);
		    	if(rs2.first()) {
		    		xpnEditor.setText(rs2.getString("xpnText"));
		    	}
		    	else {
		    		xpnEditor.setText("");
		    	}
		    	rs2.close();
	    	}
	    	
	    }
		catch (Exception e) {
	    	  e.printStackTrace();
	    }
	}
	public void refresh() {
		try {
			reloadTextArea();
		}
		catch (Exception e) {
	    	  e.printStackTrace();
	      }
	}
    public void actionPerformed(ActionEvent e){
    	if (e.getSource() == buttonEdit) {
    		buttonRevert.setEnabled(true);
    		buttonSave.setEnabled(true);
    		xpnEditor.setEditable(true);
    		buttonEdit.setEnabled(false);
    		buttonCopy.setEnabled(false);
    	}
    	else if (e.getSource() == buttonRevert) {
    		buttonRevert.setEnabled(false);
    		buttonSave.setEnabled(false);
    		xpnEditor.setEditable(false);
    		buttonEdit.setEnabled(true);
    		buttonCopy.setEnabled(true);
    		reloadTextArea();
    	}
    	else if (e.getSource() == buttonSave) {
    		myMainWindow.myConnection.setXpnContentForProject(myMainWindow.myConfig.currentProjectId, xpnEditor.getText());
    		buttonRevert.setEnabled(false);
    		buttonSave.setEnabled(false);
    		xpnEditor.setEditable(false);
    		buttonEdit.setEnabled(true);
    		buttonCopy.setEnabled(true);
    	}
    	else if (e.getSource() == buttonCopy) {
    		
    		openProjectDialog dlg = new openProjectDialog();
    		simProject sp = dlg.showDialog(myMainWindow.myConfig, myMainWindow.myConnection);
    		boolean success = myMainWindow.myConnection.copyXpnContentForProjectFromProject(myMainWindow.myConfig.currentProjectId, sp.simulation_project_id);
    		if (success) {
    			reloadTextArea();
    		}
    		else {
    			JOptionPane.showMessageDialog(null, "Error: Could not copy XPN config.");
    		}
    	}
    }

    
}
class XnParameterXpiPanel extends JPanel implements ActionListener {
	private XnParameterToolMainWindow myMainWindow ;
	
	private JTextArea xpiEditor;
	
	private JButton buttonSave;
	private JButton buttonRevert;
	private JButton buttonEdit;
	private JButton buttonCopy;
	int gridId = 0;
	
	public XnParameterXpiPanel(XnParameterToolMainWindow mainWindow) {
		super();
		
		myMainWindow = mainWindow;
	    
		JPanel panel = new JPanel();
		panel.setLayout( new java.awt.BorderLayout() );

		JLabel header = new JLabel("[model]");
		JLabel header2 = new JLabel("Note: The entries in the [gecros], [GECROS ... ], [CERES], [ceres], and [daisy_miner] sections as well as the \"climate file\" entry will be automatically generated");
		JLabel header3 = new JLabel("Note: The  \"simulation start\" and \"simulation end \" entries will be generated automatically from your entries in the General tab.");
		JPanel tPanel = new JPanel();
		tPanel.setLayout( new BoxLayout(tPanel, BoxLayout.PAGE_AXIS) );
		tPanel.add(header);
		tPanel.add(header2);
		panel.add(tPanel, java.awt.BorderLayout.NORTH);
		xpiEditor = new JTextArea();
		reloadTextArea();
		xpiEditor.setEditable(false);

	    JScrollPane tableViewScrollPaneLeft = new JScrollPane (xpiEditor, 
	            ScrollPaneConstants.VERTICAL_SCROLLBAR_ALWAYS,
	            ScrollPaneConstants.HORIZONTAL_SCROLLBAR_ALWAYS);
	    
		Dimension preferredSize = new Dimension(800,400);
		tableViewScrollPaneLeft.setPreferredSize(preferredSize);

	    
	    panel.add(tableViewScrollPaneLeft, java.awt.BorderLayout.CENTER);
	    
	    buttonSave = new JButton("Save");
	    buttonSave.addActionListener(this);
	    buttonSave.setEnabled(false);
	    
	    buttonRevert = new JButton("Revert");
	    buttonRevert.addActionListener(this);
	    buttonRevert.setEnabled(false);
	    
	    buttonEdit = new JButton("Edit");
	    buttonEdit.addActionListener(this);
	    
	    buttonCopy = new JButton("Copy from ...");
	    buttonCopy.addActionListener(this);
	    
	    JPanel lPanel = new JPanel();
	    lPanel.setLayout( new BoxLayout(lPanel, BoxLayout.PAGE_AXIS) );
	    lPanel.add(buttonEdit);
	    lPanel.add(buttonCopy);
	    lPanel.add(buttonSave);
	    lPanel.add(buttonRevert);
	    panel.add(lPanel, java.awt.BorderLayout.WEST);

	    this.add(panel);
	    
	}
	private void reloadTextArea() {
	    
	    try {
	    	if (myMainWindow.myConfig.currentProjectId > -1) { 
		    	ResultSet rs2 = myMainWindow.myConnection.getXpiContentForProject(myMainWindow.myConfig.currentProjectId, gridId);
		    	if(rs2.first()) {
		    		xpiEditor.setText(rs2.getString("xpiText"));
		    	}
		    	else {
		    		xpiEditor.setText("");
		    	}
		    	rs2.close();
	    	}
	    	
	    }
		catch (Exception e) {
	    	  e.printStackTrace();
	    }
	}
	public void refresh() {
		try {
			reloadTextArea();
		}
		catch (Exception e) {
	    	  e.printStackTrace();
	      }
	}
    public void actionPerformed(ActionEvent e){
    	if (e.getSource() == buttonEdit) {
    		buttonRevert.setEnabled(true);
    		buttonSave.setEnabled(true);
    		xpiEditor.setEditable(true);
    		buttonEdit.setEnabled(false);
    		buttonCopy.setEnabled(false);
    	}
    	else if (e.getSource() == buttonRevert) {
    		buttonRevert.setEnabled(false);
    		buttonSave.setEnabled(false);
    		xpiEditor.setEditable(false);
    		buttonEdit.setEnabled(true);
    		buttonCopy.setEnabled(true);
    		reloadTextArea();
    	}
    	else if (e.getSource() == buttonSave) {

    		myMainWindow.myConnection.setXpiContentForProject(myMainWindow.myConfig.currentProjectId, gridId, xpiEditor.getText());
    		buttonRevert.setEnabled(false);
    		buttonSave.setEnabled(false);
    		xpiEditor.setEditable(false);
    		buttonEdit.setEnabled(true);
    		buttonCopy.setEnabled(true);
    	}
    	else if (e.getSource() == buttonCopy) {
    		
    		openProjectDialog dlg = new openProjectDialog();
    		simProject sp = dlg.showDialog(myMainWindow.myConfig, myMainWindow.myConnection);
    		boolean success = myMainWindow.myConnection.copyXpiContentForProjectFromProject(myMainWindow.myConfig.currentProjectId, sp.simulation_project_id, gridId);
    		if (success) {
    			reloadTextArea();
    		}
    		else {
    			JOptionPane.showMessageDialog(null, "Error: Could not copy XPI config.");
    		}
    	}
    }

    
}







	
class XnParameterContentEditorPanel extends JPanel implements ActionListener {
	private XnParameterToolMainWindow myMainWindow ;
	
	private JTextArea contentEditor;
	
	private JButton buttonSave;
	private JButton buttonRevert;
	private JButton buttonEdit;
	private JButton buttonCopy;
	public String title;
	public String table;
	public String contentColumn;
	
	public XnParameterContentEditorPanel(XnParameterToolMainWindow mainWindow, String title_, String table_, String contentColumn_) {
		super();
		
		myMainWindow = mainWindow;
		title = title_;
		table = table_;
		contentColumn = contentColumn_;
	    
		JPanel panel = new JPanel();
		panel.setLayout( new java.awt.BorderLayout() );

		JLabel header = new JLabel(title);
		JPanel tPanel = new JPanel();
		tPanel.setLayout( new BoxLayout(tPanel, BoxLayout.PAGE_AXIS) );
		tPanel.add(header);
		
		panel.add(tPanel, java.awt.BorderLayout.NORTH);
		contentEditor = new JTextArea();
		reloadTextArea();
		contentEditor.setEditable(false);


	    JScrollPane tableViewScrollPaneLeft = new JScrollPane (contentEditor, 
	            ScrollPaneConstants.VERTICAL_SCROLLBAR_ALWAYS,
	            ScrollPaneConstants.HORIZONTAL_SCROLLBAR_ALWAYS);
	    Dimension preferredSize = new Dimension(800,400);
		tableViewScrollPaneLeft.setPreferredSize(preferredSize);

	    panel.add(tableViewScrollPaneLeft, java.awt.BorderLayout.CENTER);
	    
	    buttonSave = new JButton("Save");
	    buttonSave.addActionListener(this);
	    buttonSave.setEnabled(false);
	    
	    buttonRevert = new JButton("Revert");
	    buttonRevert.addActionListener(this);
	    buttonRevert.setEnabled(false);
	    
	    buttonEdit = new JButton("Edit");
	    buttonEdit.addActionListener(this);
	    
	    buttonCopy = new JButton("Copy from ...");
	    buttonCopy.addActionListener(this);
	    
	    JPanel lPanel = new JPanel();
	    lPanel.setLayout( new BoxLayout(lPanel, BoxLayout.PAGE_AXIS) );
	    lPanel.add(buttonEdit);
	    lPanel.add(buttonCopy);
	    lPanel.add(buttonSave);
	    lPanel.add(buttonRevert);
	    panel.add(lPanel, java.awt.BorderLayout.WEST);

	    this.add(panel);
	    
	}
	private void reloadTextArea() {
	    
	    try {
	    	if (myMainWindow.myConfig.currentProjectId > -1) { 
	    	
		    	ResultSet rs2 = myMainWindow.myConnection.getContentForProject(table, contentColumn, myMainWindow.myConfig.currentProjectId);
		    	if(rs2.first()) {
		    		contentEditor.setText(rs2.getString(contentColumn));
		    	}
		    	else {
		    		contentEditor.setText("");
		    	}
		    	rs2.close();
	    	}
	    	
	    }
		catch (Exception e) {
	    	  e.printStackTrace();
	    }
	}
	public void refresh() {
		try {
			reloadTextArea();
		}
		catch (Exception e) {
	    	  e.printStackTrace();
	      }
	}
    public void actionPerformed(ActionEvent e){
    	if (e.getSource() == buttonEdit) {
    		buttonRevert.setEnabled(true);
    		buttonSave.setEnabled(true);
    		contentEditor.setEditable(true);
    		buttonEdit.setEnabled(false);
    		buttonCopy.setEnabled(false);
    	}
    	else if (e.getSource() == buttonRevert) {
    		buttonRevert.setEnabled(false);
    		buttonSave.setEnabled(false);
    		contentEditor.setEditable(false);
    		buttonEdit.setEnabled(true);
    		buttonCopy.setEnabled(true);
    		reloadTextArea();
    	}
    	else if (e.getSource() == buttonSave) {
    		myMainWindow.myConnection.setContentForProject(table, contentColumn,myMainWindow.myConfig.currentProjectId, contentEditor.getText());
    		buttonRevert.setEnabled(false);
    		buttonSave.setEnabled(false);
    		contentEditor.setEditable(false);
    		buttonEdit.setEnabled(true);
    		buttonCopy.setEnabled(true);
    	}
    	else if (e.getSource() == buttonCopy) {
    		
    		openProjectDialog dlg = new openProjectDialog();
    		simProject sp = dlg.showDialog(myMainWindow.myConfig, myMainWindow.myConnection);
    		boolean success = myMainWindow.myConnection.copyContentForProjectFromProject(table, contentColumn, myMainWindow.myConfig.currentProjectId, sp.simulation_project_id);
    		if (success) {
    			reloadTextArea();
    		}
    		else {
    			JOptionPane.showMessageDialog(null, "Error: Could not copy "+title+" content.");
    		}
    	}
    }

    
}



//DIALOGS
class openProjectDialog  extends JDialog  implements ActionListener {
	   
	private simProject result;
	private boolean finished;
	private XnDatabaseConnection myConnection;
	private XnParameterToolConfig myConfig;
	
	private JButton buttonSwitchProject;
	private JButton buttonCancel;
	
	private JList simProjectListView;
	private simProjectListModel simProjectList;
	
	public simProject showDialog(XnParameterToolConfig conf, XnDatabaseConnection conn) {
	
		myConfig = conf;
		myConnection = conn;
		finished = false;
		
		this.setSize(400,400);
		this.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);

		
		JPanel panel = new JPanel();
        panel.setLayout( new javax.swing.BoxLayout(
                panel, javax.swing.BoxLayout.Y_AXIS ) );
        panel.setBorder(new EmptyBorder(10, 10, 10, 10));
		 
		JLabel header = new JLabel("Choose simulation project: ");
		this.add(header);
		
		simProjectList = new simProjectListModel();
		
		try {
			simProjectList.clear();
		
			ResultSet projectList = myConnection.getListOfSimulationProjects();
			
			while(projectList.next()) 
			{
				//System.out.println(projectList.getString("simulation_project_code"));

				simProjectList.addElement(new simProject(projectList));
			}
			projectList.beforeFirst();
			projectList.close();
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
		
		simProjectListView = new JList(simProjectList);
		simProjectListView.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
		//simProjectListView.setSize(200,200);
		
		JScrollPane plistScroll = new JScrollPane(simProjectListView, ScrollPaneLayout.VERTICAL_SCROLLBAR_ALWAYS,ScrollPaneLayout.HORIZONTAL_SCROLLBAR_AS_NEEDED);
		panel.add(plistScroll);
		
        JPanel bottom = new JPanel();
        bottom.setLayout( new java.awt.GridLayout(1,2 )  );
        bottom.setBorder(new EmptyBorder(5, 50, 5, 50));

		buttonSwitchProject = new JButton("Open");
		buttonSwitchProject.addActionListener(this);

		buttonCancel = new JButton("Cancel");
		buttonCancel.addActionListener(this);	
		bottom.add(buttonSwitchProject);
		bottom.add(buttonCancel);
		panel.add(bottom);
		
    	this.add(panel);
    	this.setModal(true);
    	while(!finished)
    	{
    		this.setVisible(true);
    	}
    	
    	return result;
	}
	public void actionPerformed(ActionEvent e){
    	
    	if (e.getSource() == buttonSwitchProject) 	{
    		int index = simProjectListView.getSelectedIndex();
    		if (index > -1)
    		{	result = simProjectList.getProjectAt(index);
    			finished = true;
    	    	dispose();
    		}	    		
    		return;
    	}
    	if (e.getSource() == buttonCancel) {
    		finished = true;
    		result = null;
        	dispose();
    		return;
    	}
 }
}

class  createNewProjectDialog extends JDialog  implements ActionListener {
	   
	private simProject result; 
	private JButton buttonDialogNewProjectCancel;
	private JButton buttonDialogNewProjectOk;
	private JButton buttonDialogNewProjectAddAuthor;
	private JTextField tfCode;
	private JTextArea tfDesc;
	private JList lfAuthor;
	private JList lfType;
	private DefaultListModel lfAuthorModel;
	private DefaultListModel lfTypeModel;
	private XnDatabaseConnection myConnection;
	private XnParameterToolConfig myConfig;
	private boolean finished;
	
	public simProject showDialog(XnParameterToolConfig conf, XnDatabaseConnection conn) {
	
		
		myConfig = conf;
		myConnection = conn;
		finished = false;
		
		this.setSize(400,400);
		this.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);

	    JPanel panel = new JPanel();
	    panel.setLayout( new java.awt.GridLayout(5, 3, 1,40) );
	    panel.setBorder(new EmptyBorder(10, 10, 10, 10));
    
	    JLabel codeLabel = new JLabel("Simulation project code");
	    panel.add(codeLabel);
	    tfCode = new JTextField("", 30);
	    panel.add(tfCode);
	    panel.add(new JLabel());
	    
	    JLabel descLabel = new JLabel("Description:");
	    panel.add(descLabel);
	    tfDesc = new JTextArea("", 30,5);
	    panel.add(tfDesc);
	    panel.add(new JLabel());
	    
	    JLabel schemaLabel = new JLabel("Author:");
	    panel.add(schemaLabel);
	    lfAuthorModel = new DefaultListModel();
        ResultSet rs =  myConnection.getListOfAuthors();
        
        try {
	        while (rs.next())
	        {
	        	lfAuthorModel.addElement(rs.getString("author_id"));
	        }
	        rs.close();
        }
        catch(Exception e)
        {
        	e.printStackTrace();
        }

	    lfAuthor = new JList(lfAuthorModel);
	    lfAuthor.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
	    
        JScrollPane alistScroll = new JScrollPane(lfAuthor, ScrollPaneLayout.VERTICAL_SCROLLBAR_ALWAYS,ScrollPaneLayout.HORIZONTAL_SCROLLBAR_AS_NEEDED);
        panel.add(alistScroll);
    
        buttonDialogNewProjectAddAuthor = new JButton("Add Author...");
        buttonDialogNewProjectAddAuthor.addActionListener(this);
       	panel.add(buttonDialogNewProjectAddAuthor);
    
       	
       	JLabel typeLabel = new JLabel("Project Type:");
	    panel.add(typeLabel);
	    lfTypeModel = new DefaultListModel<String>();
    
	    lfTypeModel.addElement("XN5 standalone");
	    lfTypeModel.addElement("BEMS with fixed maps (no MPMAS)");
	    lfTypeModel.addElement("BEMS  (with MPMAS)");
	    lfTypeModel.addElement("BEMS  (within ILMS)");
	
       	
       	lfType = new JList(lfTypeModel);
       	lfType.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
	    
        JScrollPane tlistScroll = new JScrollPane(lfType, ScrollPaneLayout.VERTICAL_SCROLLBAR_ALWAYS,ScrollPaneLayout.HORIZONTAL_SCROLLBAR_AS_NEEDED);
        panel.add(tlistScroll);
	    panel.add(new JLabel());

       	
        
    	buttonDialogNewProjectOk = new JButton("OK"); 
    	buttonDialogNewProjectOk.addActionListener(this);
    	buttonDialogNewProjectOk.setSelected(true);
    	panel.add(buttonDialogNewProjectOk);
    
    
    	buttonDialogNewProjectCancel = new JButton("Cancel");
    	buttonDialogNewProjectCancel.addActionListener(this);
    	panel.add(buttonDialogNewProjectCancel);

    
    	this.add(panel);
    	this.setModal(true);
    	while(!finished)
    	{
    		this.setVisible(true);
    	}
    	return result;
	}
	
	public void actionPerformed(ActionEvent e){
    	
    	if (e.getSource() == buttonDialogNewProjectOk)
    	{
    		String authorToCheck = (String) lfAuthor.getSelectedValue();
    		String code = tfCode.getText();
    		int typeOfProject = (int) lfType.getSelectedIndex();
    		String description = tfDesc.getText();

    		
    		if (! myConnection.checkAuthorName(authorToCheck))
    		{
    			JOptionPane.showMessageDialog(null, "Error: Invalid author.");
    			dispose();
    			return;
    		}
    		
    		int rc = myConnection.addSimulationProject(code, description, authorToCheck, typeOfProject);
    		if (rc == -2)
    		{
    			JOptionPane.showMessageDialog(null, "Error: Project code already used.");
    			dispose();
    			return;
    		}
    		else if (rc == -1)
    		{
    			JOptionPane.showMessageDialog(null, "Error: Project creation failed.");
    			dispose();
    			return;
    		}
    		else 
    		{
    			JOptionPane.showMessageDialog(null, "New project '" + code +  "' has been created.");
    		}
    		
    		simProject sp = new simProject(rc, code , description, authorToCheck, typeOfProject);
    		result = sp;
    		finished = true;
    		dispose();
    	}
    	if (e.getSource() == buttonDialogNewProjectCancel)
    	{
    		result = null;
    		finished = true;
    		dispose();
    	}
       	if (e.getSource() == buttonDialogNewProjectAddAuthor)
    	{
       		createNewAuthorDialog dlg = new createNewAuthorDialog();
       		author na  = dlg.showDialog(myConfig, myConnection);
       		lfAuthorModel.addElement(na.author_id);
       		lfAuthor.setSelectedValue(na.author_id, true);

       		finished = false;
       		
       		dispose();
    	}
    	

	}
		
}


class ImportGecrosDialog extends JDialog implements ActionListener {
	
	private JTextField tfFilename;
	private JTextField tfPlant;
	private JTextField tfVariety;
	private JComboBox tfEcotype;
	private JComboBox tfDaylength;
	private JComboBox tfLeafangle;
	private JList  lfPlantParam;
	private DefaultListModel lfPlantParamModel;
	
	private JTextField tfOptVernDays;
	private JTextField tfVernCoeff;
	private JTextField tfTempMinDevVern;
	private JTextField tfTempOptDevVern;
	private JTextField tfTempMaxDevVern;
	
	
	
	
	private JButton buttonImportGecrosOk;
	private JButton buttonImportGecrosCancel;
	private JButton buttonImportGecrosChooseFile;
	private JButton buttonImportGecrosNewParamId;

	private boolean finished = false;
	private int rc;
	
	private XnParameterToolMainWindow myMainWindow;
	
	public ImportGecrosDialog (XnParameterToolMainWindow mainWindow){
		
		rc = -1;
		myMainWindow = mainWindow;
		
		setTitle("Import GECROS parameters from .xnm file ...");
		setSize(500,900);
		this.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);

		
        JPanel panel = new JPanel();
        panel.setLayout( new java.awt.GridLayout(13, 3, 1,40) );
        panel.setBorder(new EmptyBorder(10, 10, 10, 10));
        
        JLabel fileLabel = new JLabel("File:");
        panel.add(fileLabel);
        tfFilename = new JTextField("", 20);
        panel.add(tfFilename);
        buttonImportGecrosChooseFile = new JButton("Browse...");
        buttonImportGecrosChooseFile.addActionListener(this);
        panel.add(buttonImportGecrosChooseFile);
        
        
        panel.add(new JLabel("Crop code:"));
        tfPlant = new JTextField("", 2);
        panel.add(tfPlant);
        panel.add(new JLabel());
        
        panel.add(new JLabel("Variety:"));
        tfVariety = new JTextField("", 20);
        panel.add(tfVariety);
        panel.add(new JLabel());
        
        String [] ecoList = {"COOL"};
        panel.add(new JLabel("Ecotype:"));
        tfEcotype = new JComboBox(ecoList);
        tfEcotype.setEditable(true);
        panel.add(tfEcotype);
        panel.add(new JLabel());
        
        String [] dlgthList = {"LDPs", "DNPs", "SDPs"};
        panel.add(new JLabel("Day length: "));
        tfDaylength = new JComboBox(dlgthList);
        tfDaylength.setEditable(true);
        panel.add(tfDaylength);
        panel.add(new JLabel());
        
        String [] leafanglelist = {"SPH"};
        panel.add(new JLabel("Leaf angle: "));
        tfLeafangle = new JComboBox(leafanglelist);
        tfLeafangle.setEditable(true);
        panel.add(tfLeafangle);
        panel.add(new JLabel());
        
        
        panel.add(new JLabel("OptVernDays:"));
        tfOptVernDays = new JTextField("", 20);
        panel.add(tfOptVernDays); 
        panel.add(new JLabel());
        
        panel.add(new JLabel("VernCoeff:"));
        tfVernCoeff = new JTextField("", 20);
        panel.add(tfVernCoeff);
        panel.add(new JLabel());
        
        panel.add(new JLabel("TempMinDevVern:"));
        tfTempMinDevVern = new JTextField("", 20);
        panel.add(tfTempMinDevVern);
        
        panel.add(new JLabel());
        panel.add(new JLabel("TempOptDevVern:"));
        tfTempOptDevVern = new JTextField("", 20);
        panel.add(tfTempOptDevVern);
        panel.add(new JLabel());
        
        panel.add(new JLabel("TempMaxDevVern:"));
        tfTempMaxDevVern = new JTextField("", 20);
        panel.add(tfTempMaxDevVern);
        panel.add(new JLabel());
        
       
        panel.add(new JLabel("Parameterization Name: "));
        lfPlantParamModel = new DefaultListModel();
        ResultSet rs = myMainWindow.myConnection.getListOfPlantParameterizations();
        
        try {
	        while (rs.next())
	        {
	        	lfPlantParamModel.addElement(rs.getString("plant_param_name"));
	        }
	        rs.close();
        }
        catch(Exception e)
        {
        	e.printStackTrace();
        }
        lfPlantParam = new JList(lfPlantParamModel);
        lfPlantParam.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
        
        JScrollPane plistScroll = new JScrollPane(lfPlantParam, ScrollPaneLayout.VERTICAL_SCROLLBAR_ALWAYS,ScrollPaneLayout.HORIZONTAL_SCROLLBAR_AS_NEEDED);
        panel.add(plistScroll);

        
        buttonImportGecrosNewParamId = new JButton("Add Parameterization...");
        buttonImportGecrosNewParamId.addActionListener(this);
        panel.add(buttonImportGecrosNewParamId);
       
        buttonImportGecrosOk = new JButton("Import");
        buttonImportGecrosOk.addActionListener(this);
        buttonImportGecrosOk.setSelected(true);
        panel.add(buttonImportGecrosOk);
        
        
        buttonImportGecrosCancel = new JButton("Cancel");
        buttonImportGecrosCancel.addActionListener(this);
        panel.add(buttonImportGecrosCancel);
        panel.add(new JLabel());

        
        this.add(panel);

		
	}
	public int showDialog () {         
		this.setModal(true);
	
		while(!finished) {
			this.setVisible(true);
		}
		return rc;
    }
	

	//Event handler
    public void actionPerformed(ActionEvent ae) {
    	 if(ae.getSource() == buttonImportGecrosCancel){
    		 finished = true;
    		 rc = -1;
    		 dispose();
    	 }
    	 else if(ae.getSource() == buttonImportGecrosChooseFile){
			JFileChooser chooser = new JFileChooser();
	        int rc = chooser.showOpenDialog(null);
	        
	        /* Abfrage, ob auf "Öffnen" geklickt wurde */
	        if(rc == JFileChooser.APPROVE_OPTION)
	        {
	        	tfFilename.setText(chooser.getSelectedFile().getPath());
	    		finished = false;
	        }
    	 }
    	 else if(ae.getSource() == buttonImportGecrosNewParamId){
        		createNewPlantParamDialog dlg = new createNewPlantParamDialog();
           		plantParam pp  = dlg.showDialog(myMainWindow.myConfig, myMainWindow.myConnection);
           		lfPlantParamModel.addElement(pp.plant_param_code);
           		lfPlantParam.setSelectedValue(pp.plant_param_code, true);
           		finished = false;
    	 }
    	 else if(ae.getSource() == buttonImportGecrosOk){
    		 
    			
    		 String xnmFileName = tfFilename.getText();
    		 String crop_code = tfPlant.getText();
    		 String variety = tfVariety.getText();
    		 String ecotype = (String) tfEcotype.getSelectedItem();
    		 String daylength = (String) tfDaylength.getSelectedItem();
    		 String leafangle = (String) tfLeafangle.getSelectedItem();
    		 String param_name = (String) lfPlantParam.getSelectedValue();
    		 
    		 int OptVernDays = 0; 
    		 int VernCoeff = 0; 
    		 double TempOptDevVern = 2;
    		 double TempMinDevVern = -1; 
    		 double TempMaxDevVern = 15;
    		 
    		 
    		 try {
	    		 OptVernDays = Integer.parseInt(tfOptVernDays.getText()); 
	    		 VernCoeff = Integer.parseInt(tfVernCoeff.getText()); 
	    		 TempOptDevVern = Double.parseDouble(tfTempOptDevVern.getText());
	    		 TempMinDevVern = Double.parseDouble(tfTempMinDevVern.getText()); 
	    		 TempMaxDevVern = Double.parseDouble(tfTempMaxDevVern.getText());
    		 }
    		 catch( Exception e ) {
    			 JOptionPane.showMessageDialog(null, "Error: Invalid import for one of the vernalization parameters (integers: OptVernDays, VernCoeff; can have decimals: TempOptDevVern, TempMinDevVern , TempMaxDevVern.");
    			 finished = false;
        		 rc = -1; 
    		  } 
    		 
    		 int param_id = myMainWindow.myConnection.getPlantParamIdForPlantParamName(param_name);
    		 
    		 //Import GECROS parameters from XN3 to database
    		 boolean ret = myMainWindow.myConnection.importGecrosParametersFromXnmFile(xnmFileName, 
    				 		param_id, crop_code, variety, ecotype, daylength, leafangle,OptVernDays, VernCoeff, TempOptDevVern, TempMinDevVern , TempMaxDevVern, true);
    			
    		 if(ret)
    		 {	rc = 0;
    		 	finished = true;
    		 	dispose();
    		 }
    		 else {
    			 JOptionPane.showMessageDialog(null, "Error: Import of GECROS parameters failed.");
    			 finished = false;
        		 rc = -1;

    		 }
    	 }
    	 
    }
}


class ImportGecrosDialogIni extends JDialog implements ActionListener {
	
	private JTextField tfFilename;
	private JTextField tfPlant;
	private JTextField tfVariety;
	private JComboBox tfEcotype;
	private JComboBox tfDaylength;
	private JComboBox tfLeafangle;
	private JList  lfPlantParam;
	private DefaultListModel lfPlantParamModel;
	
	private JTextField tfOptVernDays;
	private JTextField tfVernCoeff;
	private JTextField tfTempMinDevVern;
	private JTextField tfTempOptDevVern;
	private JTextField tfTempMaxDevVern;
	
	
	
	
	private JButton buttonImportGecrosOk;
	private JButton buttonImportGecrosCancel;
	private JButton buttonImportGecrosChooseFile;
	private JButton buttonImportGecrosNewParamId;

	private boolean finished = false;
	private int rc;
	
	private XnParameterToolMainWindow myMainWindow;
	
	public ImportGecrosDialogIni (XnParameterToolMainWindow mainWindow){
		
		rc = -1;
		myMainWindow = mainWindow;
		
		setTitle("Import GECROS parameters from .ini file ...");
		setSize(500,500);
		this.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);

		
        JPanel panel = new JPanel();
        panel.setLayout( new java.awt.GridLayout(4, 3, 1,40) );
        panel.setBorder(new EmptyBorder(10, 10, 10, 10));
        
        JLabel fileLabel = new JLabel("File:");
        panel.add(fileLabel);
        tfFilename = new JTextField("", 20);
        panel.add(tfFilename);
        buttonImportGecrosChooseFile = new JButton("Browse...");
        buttonImportGecrosChooseFile.addActionListener(this);
        panel.add(buttonImportGecrosChooseFile);
        
        
        panel.add(new JLabel("Crop code:"));
        tfPlant = new JTextField("", 2);
        panel.add(tfPlant);
        panel.add(new JLabel());
        
 
       
        panel.add(new JLabel("Parameterization Name: "));
        lfPlantParamModel = new DefaultListModel();
        ResultSet rs = myMainWindow.myConnection.getListOfPlantParameterizations();
        
        try {
	        while (rs.next())
	        {
	        	lfPlantParamModel.addElement(rs.getString("plant_param_name"));
	        }
	        rs.close();
        }
        catch(Exception e)
        {
        	e.printStackTrace();
        }
        lfPlantParam = new JList(lfPlantParamModel);
        lfPlantParam.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
        
        JScrollPane plistScroll = new JScrollPane(lfPlantParam, ScrollPaneLayout.VERTICAL_SCROLLBAR_ALWAYS,
        														ScrollPaneLayout.HORIZONTAL_SCROLLBAR_AS_NEEDED);
        JScrollBar bar = plistScroll.getVerticalScrollBar();
        bar.setPreferredSize(new Dimension(20, 60));
        panel.add(plistScroll);

        
        buttonImportGecrosNewParamId = new JButton("Add Parameterization...");
        buttonImportGecrosNewParamId.addActionListener(this);
        panel.add(buttonImportGecrosNewParamId);
       
        buttonImportGecrosOk = new JButton("Import");
        buttonImportGecrosOk.addActionListener(this);
        buttonImportGecrosOk.setSelected(true);
        panel.add(buttonImportGecrosOk);
        
        
        buttonImportGecrosCancel = new JButton("Cancel");
        buttonImportGecrosCancel.addActionListener(this);
        panel.add(buttonImportGecrosCancel);
        panel.add(new JLabel());

        
        this.add(panel);

		
	}
	public int showDialog () {         
		this.setModal(true);
	
		while(!finished) {
			this.setVisible(true);
		}
		return rc;
    }
	

	//Event handler
    public void actionPerformed(ActionEvent ae) {
    	 if(ae.getSource() == buttonImportGecrosCancel){
    		 finished = true;
    		 rc = -1;
    		 dispose();
    	 }
    	 else if(ae.getSource() == buttonImportGecrosChooseFile){
			JFileChooser chooser = new JFileChooser();
	        int rc = chooser.showOpenDialog(null);
	        
	        /* Abfrage, ob auf "Öffnen" geklickt wurde */
	        if(rc == JFileChooser.APPROVE_OPTION)
	        {
	        	tfFilename.setText(chooser.getSelectedFile().getPath());
	    		finished = false;
	        }
    	 }
    	 else if(ae.getSource() == buttonImportGecrosNewParamId){
        		createNewPlantParamDialog dlg = new createNewPlantParamDialog();
           		plantParam pp  = dlg.showDialog(myMainWindow.myConfig, myMainWindow.myConnection);
           		lfPlantParamModel.addElement(pp.plant_param_code);
           		lfPlantParam.setSelectedValue(pp.plant_param_code, true);
           		finished = false;
    	 }
    	 else if(ae.getSource() == buttonImportGecrosOk){
    		 
    			
    		 String iniFileName = tfFilename.getText();
    		 String crop_code = tfPlant.getText();

    		 String param_name = (String) lfPlantParam.getSelectedValue();
    		 
 
    		 
    		 int param_id = myMainWindow.myConnection.getPlantParamIdForPlantParamName(param_name);
    		 
    		 //Import GECROS parameters from XN3 to database
    		 boolean ret = myMainWindow.myConnection.importGecrosParametersFromIniFile(iniFileName, 
    				 		param_id, crop_code, true);
    			
    		 if(ret)
    		 {	rc = 0;
    		 	finished = true;
    		 	dispose();
    		 }
    		 else {
    			 JOptionPane.showMessageDialog(null, "Error: Import of GECROS parameters from .ini file failed.");
    			 finished = false;
        		 rc = -1;

    		 }
    	 }
    	 
    }
}

class ImportDaisyDialog extends JDialog implements ActionListener {
	
	private JTextField tfFilename;
	private JTextField tfProfileId;
	private JList  lfDaisyParam;
	private DefaultListModel lfDaisyParamModel;
	private JButton buttonImportDaisyOk;
	private JButton buttonImportDaisyCancel;
	private JButton buttonImportDaisyChooseFile;
	private JButton buttonImportDaisyNewParamId;

	private boolean finished = false;
	private int rc;
	
	private XnParameterToolMainWindow myMainWindow;
	
	public ImportDaisyDialog (XnParameterToolMainWindow mainWindow){
		
		rc = -1;
		myMainWindow = mainWindow;
		
		setTitle("Import DAISY parameters from .xnm file ...");
		setSize(600,300);
		this.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);

        JPanel panel = new JPanel();
        panel.setLayout( new java.awt.GridLayout(4, 3, 1,30) );
        panel.setBorder(new EmptyBorder(10, 10, 10, 10));
        
        JLabel fileLabel = new JLabel("File:");
        panel.add(fileLabel);
        tfFilename = new JTextField("", 20);
        panel.add(tfFilename);
        buttonImportDaisyChooseFile = new JButton("Browse...");
        buttonImportDaisyChooseFile.addActionListener(this);
        panel.add(buttonImportDaisyChooseFile);
        
        
        panel.add(new JLabel("Soil profile code:"));
        tfProfileId = new JTextField("", 2);
        panel.add(tfProfileId);
        panel.add(new JLabel());
                
        panel.add(new JLabel("Parameterization Name: "));
        lfDaisyParamModel = new DefaultListModel();
        ResultSet rs = myMainWindow.myConnection.getListOfDaisyParameterizations();
        
        try {
	        while (rs.next())
	        {
	        	lfDaisyParamModel.addElement(rs.getString("daisy_param_name"));
	        }
	        rs.close();
        }
        catch(Exception e)
        {
        	e.printStackTrace();
        }
        lfDaisyParam = new JList(lfDaisyParamModel);
        lfDaisyParam.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
        
        JScrollPane plistScroll = new JScrollPane(lfDaisyParam, ScrollPaneLayout.VERTICAL_SCROLLBAR_ALWAYS,ScrollPaneLayout.HORIZONTAL_SCROLLBAR_AS_NEEDED);
        panel.add(plistScroll);
        
        
        buttonImportDaisyNewParamId = new JButton("Add Parameterization...");
        buttonImportDaisyNewParamId.addActionListener(this);
        panel.add(buttonImportDaisyNewParamId);
       
        buttonImportDaisyOk = new JButton("Import");
        buttonImportDaisyOk.addActionListener(this);
        buttonImportDaisyOk.setSelected(true);
        panel.add(buttonImportDaisyOk);
        
        
        buttonImportDaisyCancel = new JButton("Cancel");
        buttonImportDaisyCancel.addActionListener(this);
        panel.add(buttonImportDaisyCancel);
        panel.add(new JLabel());
        
        this.add(panel);

		
	}
	public int showDialog () {         
		this.setModal(true);
	
		while(!finished) {
			this.setVisible(true);
		}
		return rc;
    }
	

	//Event handler
    public void actionPerformed(ActionEvent ae) {
    	 if(ae.getSource() == buttonImportDaisyCancel){
    		 finished = true;
    		 rc = -1;
    		 dispose();
    	 }
    	 else if(ae.getSource() == buttonImportDaisyChooseFile){
			JFileChooser chooser = new JFileChooser();
	        chooser.setCurrentDirectory(new java.io.File(myMainWindow.myConfig.lastXnmInDirectory));
	        chooser.setDialogTitle("Choose .xnm file to import ...");

	        int rc = chooser.showOpenDialog(null);
	        
	        /* Abfrage, ob auf "Öffnen" geklickt wurde */
	        if(rc == JFileChooser.APPROVE_OPTION)
	        {
	        	tfFilename.setText(chooser.getSelectedFile().getPath());	        	
	        	myMainWindow.myConfig.lastXnmInDirectory = chooser.getCurrentDirectory().getPath();
	        	if (myMainWindow.myConfig.lastXndInDirectory == ".")
	        	{
	        		myMainWindow.myConfig.lastXndInDirectory = chooser.getCurrentDirectory().getPath();	
	        	}
	    		finished = false;
	        }
    	 }
    	 else if(ae.getSource() == buttonImportDaisyNewParamId){
        		createNewDaisyParamDialog dlg = new createNewDaisyParamDialog();
           		daisyParam pp  = dlg.showDialog(myMainWindow.myConfig, myMainWindow.myConnection);
           		lfDaisyParamModel.addElement(pp.daisy_param_code);
           		lfDaisyParam.setSelectedValue(pp.daisy_param_code, true);
           		finished = false;
    	 }
    	 else if(ae.getSource() == buttonImportDaisyOk){
    		 
    			
    		 String xnmFileName = tfFilename.getText();
    		 String profileID = tfProfileId.getText();

    		 String param_name = (String) lfDaisyParam.getSelectedValue();
    		 
    		 int param_id = myMainWindow.myConnection.getDaisyParamIdForDaisyParamName(param_name);
    		 
    		 //Import DAISY parameters from XN3 to database
    		 boolean ret = myMainWindow.myConnection.importDaisyParametersFromXnmFile(xnmFileName, 
    				 		param_id, profileID, true);
    			
    		 if(ret)
    		 {	rc = 0;
    		 	finished = true;
    		 	dispose();
    		 }
    		 else {
    			 JOptionPane.showMessageDialog(null, "Error: Import of DAISY parameters failed.");
    			 finished = false;
        		 rc = -1;

    		 }
    	 }
    	 
    }
}

class ExportWeatherDialog extends JDialog implements ActionListener {
	private JTextField tfTableName;
	private JTextField tfFilename;
	private JTextField tfStationId;

	private JTextField tfFirstYear;
	private JTextField tfLastYear;
	private JTextField tfMaxDailyPrecip;
	
	private JCheckBox cbCO2;
	private JTextField tfCO2Table;
	
	private JCheckBox cbInits;
	private JCheckBox cbElevationCorr;
	private JTextField tfTableWithElevationOrigHeight;
	private JTextField tfElevationNewHeight;

	

	
	
	private JButton buttonExportOk;
	private JButton buttonExportCancel;
	
	private XnParameterToolMainWindow myMainWindow;
	
	private int rc = 0;
	private boolean finished = false;
	
	public ExportWeatherDialog (XnParameterToolMainWindow mainWindow){
		
		rc = -1;
		myMainWindow = mainWindow;
		
		setTitle("Export weather data to file ...");
		setSize(400,600);
		this.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);

        JPanel panel = new JPanel();
        panel.setLayout( new java.awt.GridLayout(13, 2, 1,30) );
        panel.setBorder(new EmptyBorder(10, 10, 10, 10));
        
        JLabel fileLabel = new JLabel("Filename:");
        panel.add(fileLabel);
        tfFilename = new JTextField("", 20);
        panel.add(tfFilename);
                
        
        panel.add(new JLabel("Table in for1695_weather:"));
        tfTableName = new JTextField("", 20);
        panel.add(tfTableName);
                
        panel.add(new JLabel("Station ID: "));
        tfStationId = new JTextField("", 5);
        panel.add(tfStationId);
        
        panel.add(new JLabel("First year: "));
        tfFirstYear = new JTextField("", 4);
        panel.add(tfFirstYear);
        panel.add(new JLabel("Last year: "));
        tfLastYear = new JTextField("", 4);
        panel.add(tfLastYear);

        
        panel.add(new JLabel("Max. limit daily precip. : "));
        tfMaxDailyPrecip = new JTextField("30", 4);
        panel.add(tfMaxDailyPrecip); 
        
        
        panel.add(new JLabel("Correct for elevation?"));
        cbElevationCorr = new JCheckBox();
        panel.add(cbElevationCorr);
        panel.add(new JLabel("Original altitude (enter the value or a table name): "));
        tfTableWithElevationOrigHeight = new JTextField("", 30);
        panel.add(tfTableWithElevationOrigHeight);
        panel.add(new JLabel("New altitude: "));
        tfElevationNewHeight = new JTextField("", 4);
        panel.add(tfElevationNewHeight);
        
        
        panel.add(new JLabel("Add CO2 concentration?"));
        cbCO2 = new JCheckBox();
        panel.add(cbCO2);
        
        panel.add(new JLabel("Table with CO2 concentration time series: "));
        tfCO2Table = new JTextField("for1695_weather.", 50);
        panel.add(tfCO2Table);
        
        
        
        
        panel.add(new JLabel("Generate BEMS initial history?"));
        cbInits = new JCheckBox();
        panel.add(cbInits);
        
        
        buttonExportOk = new JButton("OK ...");
        buttonExportOk.addActionListener(this);
        buttonExportOk.setSelected(true);
        panel.add(buttonExportOk);
       
        buttonExportCancel = new JButton("Cancel");
        buttonExportCancel.addActionListener(this);
        panel.add(buttonExportCancel);
           

        
        this.add(panel);

		
	}
	public int showDialog () {         
		this.setModal(true);
	
		while(!finished) {
			this.setVisible(true);
		}
		return rc;
    }
	//Event handler
    public void actionPerformed(ActionEvent ae) {
    	 if(ae.getSource() == buttonExportCancel){
    		 finished = true;
    		 rc = -1;
    		 dispose();
    	 }
    	
    	 else if(ae.getSource() == buttonExportOk){
    		 
    			
    		 String filename = tfFilename.getText();
    		 String tablename = tfTableName.getText();
    		 String stationid = tfStationId.getText();
    		 String first_year = tfFirstYear.getText();
    		 String last_year = tfLastYear.getText();
    		 Double maxDailyPrecip = Double.valueOf(tfMaxDailyPrecip.getText());
    		 
    		 String co2Table = tfCO2Table.getText();
    		 
    		 String prefix = myMainWindow.askForOutputDirectory();
    		 
    		 String origAltitudeInfo = tfTableWithElevationOrigHeight.getText();
    		 int newAltitude = 0;
    		 if (cbElevationCorr.isSelected()) { 
    			 newAltitude = Integer.valueOf(tfElevationNewHeight.getText());
    		 }
    		 

     		 prefix += "/"+ filename;
     		 boolean ret = myMainWindow.myConnection.writeWeatherData(prefix, tablename, Integer.valueOf(stationid), Integer.valueOf(first_year), Integer.valueOf(last_year), cbInits.isSelected() ? 1 : 0, false, "", maxDailyPrecip, cbElevationCorr.isSelected() ? 1:0, newAltitude, origAltitudeInfo
     				, cbCO2.isSelected(), co2Table );
    		     			
    		 if(ret)
    		 {	rc = 0;
    		 	finished = true;
    		 	dispose();
    		 }
    		 else {
    			 JOptionPane.showMessageDialog(null, "Error: Export of weather file failed.");
    			 finished = false;
        		 rc = -1;

    		 }
    	 }
    	 
    }
}


class ImportXn3Dialog extends JDialog implements ActionListener {
	
	private JTextField tfXnmFilename;
	private JTextField tfXndFilename;
	private JTextField tfProfileId;
	private JList  lfDaisyParam;
	private DefaultListModel lfDaisyParamModel;
	private JList  lfSoilParam;
	private DefaultListModel lfSoilParamModel;
	private JList  lfSoilInit;
	private DefaultListModel lfSoilInitModel;
	private JList  lfSomPools;
	private DefaultListModel lfSomPoolsModel;
	private JList  lfManagements;
	private DefaultListModel lfManagementsModel;

	//private JTextField tfPlant;
	//private JTextField tfVariety;
	
	private JButton buttonImportOk;
	private JButton buttonImportCancel;
	private JButton buttonImportChooseXnmFile;
	private JButton buttonImportChooseXndFile;
	private JButton buttonImportNewDaisyParamId;
	private JButton buttonImportNewSomPoolId;
	private JButton buttonImportNewSoilInitParamId;
	private JButton buttonImportNewSoilParamId;
	private JButton buttonImportNewManagementId;
	
	private boolean finished = false;
	private int rc;
	
	private XnParameterToolMainWindow myMainWindow;
	
	public enum xn3ImportType {
		daisyParam,
		daisyPools,
		soilParam,
		soilStart,
		soilAll,
		Management,
		fullXnmXnd
		
	}
	xn3ImportType inType;
	
	public ImportXn3Dialog (XnParameterToolMainWindow mainWindow, xn3ImportType _inType){
		
		inType = _inType;
		rc = -1;
		myMainWindow = mainWindow;
		
		switch(inType) {
		
			case daisyParam:
				setTitle("Import DAISY parameters from .xnm file ...");
				break;
			case daisyPools:
				setTitle("Import DAISY SOM pool initialization from .xnm file ...");
				break;
			case soilParam:
				setTitle("Import soil physical parameters from .xnm/.xnd file ...");
				break;
			case soilStart:
				setTitle("Import soil start values from .xnm/.xnd file ...");
				break;
			case soilAll:
				setTitle("Import soil data from .xnm/.xnd file ...");
				break;
			case Management:
				setTitle("Import crop management from .xnd file ...");
				break;
			case fullXnmXnd:
				setTitle("Import full .xnm/.xnd file ...");
				break;
			
				
		}
		setSize(900,800);
		this.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);

        JPanel panel = new JPanel();
        panel.setLayout( new java.awt.GridLayout(9, 3, 1,30) );//11, 3, 1,30
        panel.setBorder(new EmptyBorder(10, 10, 10, 10));

        switch (inType) {  
			case daisyParam:
			case daisyPools:
			case soilParam:
			case soilStart:
			case soilAll:
			case fullXnmXnd:
		        JLabel fileLabel = new JLabel("XNM File:");
		        panel.add(fileLabel);
		        tfXnmFilename = new JTextField("", 20);
		        panel.add(tfXnmFilename);
		        buttonImportChooseXnmFile = new JButton("Browse...");
		        buttonImportChooseXnmFile.addActionListener(this);
		        panel.add(buttonImportChooseXnmFile);
		        break;
		     default:
			    panel.add(new JLabel());
			    panel.add(new JLabel());
			    panel.add(new JLabel());
        }
        
        switch (inType) {  
			case soilParam:
			case soilStart:
			case soilAll:
			case fullXnmXnd:
			case Management:
		        JLabel fileLabel = new JLabel("XND File:");
		        panel.add(fileLabel);
		        tfXndFilename = new JTextField("", 20);
		        panel.add(tfXndFilename);
		        buttonImportChooseXndFile = new JButton("Browse...");
		        buttonImportChooseXndFile.addActionListener(this);
		        panel.add(buttonImportChooseXndFile);
		        break;
		     default:
			    panel.add(new JLabel());
			    panel.add(new JLabel());
			    panel.add(new JLabel());
        }
        switch (inType) {
        	case daisyParam:
        	case daisyPools:
			case soilParam:
			case soilStart:
			case soilAll:
			case fullXnmXnd:
		        panel.add(new JLabel("Soil profile code:"));
		        tfProfileId = new JTextField("", 2);
		        panel.add(tfProfileId);
		        panel.add(new JLabel());
		        break;
		     default:
			    panel.add(new JLabel());
			    panel.add(new JLabel());
			    panel.add(new JLabel());
        }    
        
        
        switch (inType) {
	    	case daisyParam:
			case soilAll:
			case fullXnmXnd:
				panel.add(new JLabel("DAISY parameterization set: "));
		        lfDaisyParamModel = new DefaultListModel();
		        ResultSet rs = myMainWindow.myConnection.getListOfIdCodes(XnDatabaseConnection.idTypes.daisyParam);
		        
		        try {
			        while (rs.next())
			        {
			        	lfDaisyParamModel.addElement(rs.getString(1));
			        }
			        rs.close();
		        }
		        catch(Exception e)
		        {
		        	e.printStackTrace();
		        }
		        lfDaisyParam = new JList(lfDaisyParamModel);
		        lfDaisyParam.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
		        
		        JScrollPane plistScroll = new JScrollPane(lfDaisyParam, ScrollPaneLayout.VERTICAL_SCROLLBAR_ALWAYS,ScrollPaneLayout.HORIZONTAL_SCROLLBAR_AS_NEEDED);
		        panel.add(plistScroll);
		        
		        buttonImportNewDaisyParamId = new JButton("Add DAISY parameterization set...");
		        buttonImportNewDaisyParamId.addActionListener(this);
		        panel.add(buttonImportNewDaisyParamId);
		        break;
		     default:
			    panel.add(new JLabel());
			    panel.add(new JLabel());
			    panel.add(new JLabel());
	    }  
        
        switch (inType) {
	    	case daisyPools:
			case soilAll:
			case fullXnmXnd:
				panel.add(new JLabel("SOM pool initialization set: "));
				lfSomPoolsModel = new DefaultListModel();
		        ResultSet rs = myMainWindow.myConnection.getListOfIdCodes(XnDatabaseConnection.idTypes.daisyPools);;
		        
		        try {
			        while (rs.next())
			        {
			        	lfSomPoolsModel.addElement(rs.getString(1));
			        }
			        rs.close();
		        }
		        catch(Exception e)
		        {
		        	e.printStackTrace();
		        }
		        lfSomPools = new JList(lfSomPoolsModel);
		        lfSomPools.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
		        
		        JScrollPane plistScroll2 = new JScrollPane(lfSomPools, ScrollPaneLayout.VERTICAL_SCROLLBAR_ALWAYS,ScrollPaneLayout.HORIZONTAL_SCROLLBAR_AS_NEEDED);
		        panel.add(plistScroll2);
		        
		        buttonImportNewSomPoolId = new JButton("Add SOM pool initialization set...");
		        buttonImportNewSomPoolId.addActionListener(this);
		        panel.add(buttonImportNewSomPoolId);
		        break;
		     default:
			    panel.add(new JLabel());
			    panel.add(new JLabel());
			    panel.add(new JLabel());
	    }  
        
        switch (inType) {
	    	case soilParam:
			case soilAll:
			case fullXnmXnd:
				panel.add(new JLabel("Soil physical parameterization set: "));
				lfSoilParamModel = new DefaultListModel();
		        ResultSet rs = myMainWindow.myConnection.getListOfIdCodes(XnDatabaseConnection.idTypes.soilParam);;
		        
		        try {
			        while (rs.next())
			        {
			        	lfSoilParamModel.addElement(rs.getString(1));
			        }
			        rs.close();
		        }
		        catch(Exception e)
		        {
		        	e.printStackTrace();
		        }
		        lfSoilParam = new JList(lfSoilParamModel);
		        lfSoilParam.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
		        
		        JScrollPane plistScroll3 = new JScrollPane(lfSoilParam, ScrollPaneLayout.VERTICAL_SCROLLBAR_ALWAYS,ScrollPaneLayout.HORIZONTAL_SCROLLBAR_AS_NEEDED);
		        panel.add(plistScroll3);
		        
		        buttonImportNewSoilParamId = new JButton("Add soil physical parameterization set...");
		        buttonImportNewSoilParamId.addActionListener(this);
		        panel.add(buttonImportNewSoilParamId);
		        break;
		     default:
			    panel.add(new JLabel());
			    panel.add(new JLabel());
			    panel.add(new JLabel());
	    }          
        
        switch (inType) {
	    	case soilStart:
			case soilAll:
			case fullXnmXnd:
				panel.add(new JLabel(" Soil initialization set: "));
				lfSoilInitModel = new DefaultListModel();
		        ResultSet rs = myMainWindow.myConnection.getListOfIdCodes(XnDatabaseConnection.idTypes.soilInit);;
		        
		        try {
			        while (rs.next())
			        {
			        	lfSoilInitModel.addElement(rs.getString(1));
			        }
			        rs.close();
		        }
		        catch(Exception e)
		        {
		        	e.printStackTrace();
		        }
		        lfSoilInit = new JList(lfSoilInitModel);
		        lfSoilInit.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
		        
		        JScrollPane plistScroll4 = new JScrollPane(lfSoilInit, ScrollPaneLayout.VERTICAL_SCROLLBAR_ALWAYS,ScrollPaneLayout.HORIZONTAL_SCROLLBAR_AS_NEEDED);
		        panel.add(plistScroll4);
		        
		        buttonImportNewSoilInitParamId = new JButton("Add soil initialization set...");
		        buttonImportNewSoilInitParamId.addActionListener(this);
		        panel.add(buttonImportNewSoilInitParamId);
		        break;
		     default:
			    panel.add(new JLabel());
			    panel.add(new JLabel());
			    panel.add(new JLabel());
	    }   
        
        switch (inType) {
	    	case Management:
			case fullXnmXnd:
				panel.add(new JLabel("Crop management set: "));
				lfManagementsModel = new DefaultListModel();
		        ResultSet rs = myMainWindow.myConnection.getListOfIdCodes(XnDatabaseConnection.idTypes.management);;
		        
		        try {
			        while (rs.next())
			        {
			        	lfManagementsModel.addElement(rs.getString(1));
			        }
			        rs.close();
		        }
		        catch(Exception e)
		        {
		        	e.printStackTrace();
		        }
		        lfManagements = new JList(lfManagementsModel);
		        lfManagements.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
		        
		        JScrollPane plistScroll4 = new JScrollPane(lfManagements, ScrollPaneLayout.VERTICAL_SCROLLBAR_ALWAYS,ScrollPaneLayout.HORIZONTAL_SCROLLBAR_AS_NEEDED);
		        panel.add(plistScroll4);
		        
		        buttonImportNewManagementId = new JButton("Add crop management set...");
		        buttonImportNewManagementId.addActionListener(this);
		        panel.add(buttonImportNewManagementId);
		        
/*		        panel.add(new JLabel("Crop code:"));
		        tfPlant = new JTextField("", 2);
		        panel.add(tfPlant);
		        panel.add(new JLabel());
		        
		        panel.add(new JLabel("Variety:"));
		        tfVariety = new JTextField("", 20);
		        panel.add(tfVariety);
		        panel.add(new JLabel());*/
		        
		        break;
		     default:
			    panel.add(new JLabel());
			    panel.add(new JLabel());
			    panel.add(new JLabel());
			   /* panel.add(new JLabel());
			    panel.add(new JLabel());
			    panel.add(new JLabel());
			    panel.add(new JLabel());
			    panel.add(new JLabel());
			    panel.add(new JLabel());*/
	    }           
       
        buttonImportOk = new JButton("Import");
        buttonImportOk.addActionListener(this);
        buttonImportOk.setSelected(true);
        panel.add(buttonImportOk);
        
        
        buttonImportCancel = new JButton("Cancel");
        buttonImportCancel.addActionListener(this);
        panel.add(buttonImportCancel);
        panel.add(new JLabel());
        
        this.add(panel);

		
	}
	
	
	public int showDialog () {         
		this.setModal(true);
	
		while(!finished) {
			this.setVisible(true);
		}
		return rc;
    }
	

	//Event handler
    public void actionPerformed(ActionEvent ae) {
    	 if(ae.getSource() == buttonImportCancel){
    		 finished = true;
    		 rc = -1;
    		 dispose();
    	 }
    	 else if(ae.getSource() == buttonImportChooseXnmFile){
			JFileChooser chooser = new JFileChooser();
	        chooser.setCurrentDirectory(new java.io.File(myMainWindow.myConfig.lastXnmInDirectory));
	        chooser.setDialogTitle("Choose .xnm file to import ...");

	        int rc = chooser.showOpenDialog(null);
	        
	        /* Abfrage, ob auf "Öffnen" geklickt wurde */
	        if(rc == JFileChooser.APPROVE_OPTION)
	        {
	        	tfXnmFilename.setText(chooser.getSelectedFile().getPath());	        	
	        	myMainWindow.myConfig.lastXnmInDirectory = chooser.getCurrentDirectory().getPath();
	        	if (myMainWindow.myConfig.lastXndInDirectory == ".")
	        	{
	        		myMainWindow.myConfig.lastXndInDirectory = chooser.getCurrentDirectory().getPath();	
	        	}
	    		finished = false;
	        }
    	 }
    	 else if(ae.getSource() == buttonImportChooseXndFile){
			JFileChooser chooser = new JFileChooser();
	        chooser.setCurrentDirectory(new java.io.File(myMainWindow.myConfig.lastXndInDirectory));
	        chooser.setDialogTitle("Choose .xnd file to import ...");

	        int rc = chooser.showOpenDialog(null);
	        
	        /* Abfrage, ob auf "Öffnen" geklickt wurde */
	        if(rc == JFileChooser.APPROVE_OPTION)
	        {
	        	tfXndFilename.setText(chooser.getSelectedFile().getPath());	        	
	        	myMainWindow.myConfig.lastXndInDirectory = chooser.getCurrentDirectory().getPath();
	        	if (myMainWindow.myConfig.lastXnmInDirectory == ".")
	        	{
	        		myMainWindow.myConfig.lastXnmInDirectory = chooser.getCurrentDirectory().getPath();	
	        	}
	    		finished = false;
	        }
    	 }
    	 else if(ae.getSource() == buttonImportNewDaisyParamId){
        		createNewDaisyParamDialog dlg = new createNewDaisyParamDialog();
           		daisyParam pp  = dlg.showDialog(myMainWindow.myConfig, myMainWindow.myConnection);
           		System.out.println("DAISY:"+pp.daisy_param_code);
           		lfDaisyParamModel.addElement(pp.daisy_param_code);
           		lfDaisyParam.setSelectedValue(pp.daisy_param_code, true);
           		finished = false;
    	 }
    	 /*else if(ae.getSource() == buttonImportNewDaisyParamId){
    		 createNewIdCodeDialog dlg = new createNewIdCodeDialog();
        		idCode pp  = dlg.showDialog( myMainWindow.myConfig, myMainWindow.myConnection, XnDatabaseConnection.idTypes.daisyParam);
        		lfDaisyParamModel.addElement(pp.code);
        		lfDaisyParam.setSelectedValue(pp.code, true);
        		finished = false;
    	 }*/
    	 else if(ae.getSource() == buttonImportNewSomPoolId){
    		 createNewIdCodeDialog dlg = new createNewIdCodeDialog();
        		idCode pp  = dlg.showDialog( myMainWindow.myConfig, myMainWindow.myConnection, XnDatabaseConnection.idTypes.daisyPools);
        		System.out.println("SOM:"+pp.code);
        		lfSomPoolsModel.addElement(pp.code);
        		lfSomPools.setSelectedValue(pp.code, true);
        		finished = false;
    	 }
    	 else if(ae.getSource() == buttonImportNewSoilParamId){
    		 	createNewIdCodeDialog dlg = new createNewIdCodeDialog();
        		idCode pp  = dlg.showDialog( myMainWindow.myConfig, myMainWindow.myConnection, XnDatabaseConnection.idTypes.soilParam);
        		lfSoilParamModel.addElement(pp.code);
        		lfSoilParam.setSelectedValue(pp.code, true);
        		finished = false;
    	 }
    	 else if(ae.getSource() == buttonImportNewSoilInitParamId){
 		 	createNewIdCodeDialog dlg = new createNewIdCodeDialog();
     		idCode pp  = dlg.showDialog( myMainWindow.myConfig, myMainWindow.myConnection, XnDatabaseConnection.idTypes.soilInit);
     		lfSoilInitModel.addElement(pp.code);
     		lfSoilInit.setSelectedValue(pp.code, true);
     		finished = false;
    	 }
    	 else if(ae.getSource() == buttonImportNewManagementId){
 		 	createNewIdCodeDialog dlg = new createNewIdCodeDialog();
     		idCode pp  = dlg.showDialog( myMainWindow.myConfig, myMainWindow.myConnection, XnDatabaseConnection.idTypes.management);
     		lfManagementsModel.addElement(pp.code);
     		lfManagements.setSelectedValue(pp.code, true);
     		finished = false;
    	 }
    	 
    	 
    	 else if(ae.getSource() == buttonImportOk){
    		 
    		//Check validity of all required input fields
     		if ( isAnyOf(inType, Arrays.asList(xn3ImportType.daisyParam, xn3ImportType.daisyPools, xn3ImportType.soilParam, xn3ImportType.soilStart, xn3ImportType.soilAll, xn3ImportType.fullXnmXnd   ))) {
    		
	     		 if (tfXnmFilename.getText().matches("^\\s*$") ) {
	     			 JOptionPane.showMessageDialog(null, "Error: Please specify an XNM file.");
	     			 finished = false;
	         		 rc = -1;
	         		 return;
	     		 }
     		}
     		if ( isAnyOf(inType, Arrays.asList(xn3ImportType.Management, xn3ImportType.soilParam, xn3ImportType.soilStart, xn3ImportType.soilAll, xn3ImportType.fullXnmXnd   ))) {
	     		 if (tfXndFilename.getText().matches("^\\s*$") ) {
	     			 JOptionPane.showMessageDialog(null, "Error: Please specify an XND file.");
	     			 finished = false;
	         		 rc = -1;
	         		 return;
	     		 }
     		}
     		if ( isAnyOf(inType, Arrays.asList(xn3ImportType.daisyParam, xn3ImportType.daisyPools, xn3ImportType.soilParam, xn3ImportType.soilStart, xn3ImportType.soilAll, xn3ImportType.fullXnmXnd   ))) {
	     		 if (tfProfileId.getText().matches("^\\s*$") ) {
	     			 JOptionPane.showMessageDialog(null, "Error: Please specify a valid profileID file.");
	     			 finished = false;
	         		 rc = -1;
	         		 return;
	     		 }
     		}    		 
    		 
    		 
    		if ( isAnyOf(inType, Arrays.asList(xn3ImportType.daisyParam, xn3ImportType.soilAll, xn3ImportType.fullXnmXnd   ))) {
	     		 if (  lfDaisyParam.isSelectionEmpty()  ) {
	     			 JOptionPane.showMessageDialog(null, "Error: Please indicate the DAISY parameterization set.");
	     			 finished = false;
	         		 rc = -1;
	         		 return;
	     		 }    			
    		}
    		if ( isAnyOf(inType, Arrays.asList(xn3ImportType.daisyPools, xn3ImportType.soilAll, xn3ImportType.fullXnmXnd   ))) {
	     		 if ( lfSomPools.isSelectionEmpty()  ) {
	     			 JOptionPane.showMessageDialog(null, "Error: Please indicate the SOM pool initialization set.");
	     			 finished = false;
	         		 rc = -1;
	         		 return;
	     		 } 
    		}
    		if ( isAnyOf(inType, Arrays.asList(xn3ImportType.soilParam, xn3ImportType.soilAll, xn3ImportType.fullXnmXnd   ))) {
	     		 if (  lfSoilParam.isSelectionEmpty()  ) {
	     			 JOptionPane.showMessageDialog(null, "Error: Please indicate the Soil physical parameterization set.");
	     			 finished = false;
	         		 rc = -1;
	         		 return;
	     		 } 
    		} 

    		if ( isAnyOf(inType, Arrays.asList(xn3ImportType.soilStart, xn3ImportType.soilAll, xn3ImportType.fullXnmXnd   ))) {
	     		 if (  lfSoilInit.isSelectionEmpty()  ) {
	     			 JOptionPane.showMessageDialog(null, "Error: Please indicate the Soil initialization set.");
	     			 finished = false;
	         		 rc = -1;
	         		 return;
	     		 } 
    		} 
    		if ( isAnyOf(inType, Arrays.asList(xn3ImportType.Management,  xn3ImportType.fullXnmXnd   ))) {
	     		 if (  lfManagements.isSelectionEmpty()  ) {
	     			 JOptionPane.showMessageDialog(null, "Error: Please indicate the crop management set.");
	     			 finished = false;
	         		 rc = -1;
	         		 return;
	     		 } 
    		} 
    		/*if ( isAnyOf(inType, Arrays.asList(xn3ImportType.Management,  xn3ImportType.fullXnmXnd   ))) {
	     		 if (tfPlant.getText().matches("^\\s*$") ) {
	     			 JOptionPane.showMessageDialog(null, "Error: Please specify a crop code.");
	     			 finished = false;
	         		 rc = -1;
	         		 return;
	     		 }
	     		 if (tfPlant.getText().length() > 2 ) {
	     			 JOptionPane.showMessageDialog(null, "Error: Please specify a valid crop code (max 2 letters).");
	     			 finished = false;
	         		 rc = -1;
	         		 return;
	     		 }
	     		 if (tfVariety.getText().matches("^\\s*$") ) {
	     			 JOptionPane.showMessageDialog(null, "Error: Please specify a variety.");
	     			 finished = false;
	         		 rc = -1;
	         		 return;
	     		 }
    		}*/
    		//Process required steps	
    	    switch (inType) {
    	    	case daisyParam:
    			case soilAll:
    			case fullXnmXnd:
    			{	 boolean ret = myMainWindow.myConnection.importDaisyParametersFromXnmFile(
    	    				 tfXnmFilename.getText(), 
    	    				 myMainWindow.myConnection.getDaisyParamIdForDaisyParamName((String) lfDaisyParam.getSelectedValue()), 
    	    				 tfProfileId.getText(), 
    	    				 true);
     			
		     		 if(ret)
		     		 {	rc = 0;
		     		 	finished = true;
		     		 }
		     		 else {
		     			 JOptionPane.showMessageDialog(null, "Error: Import of DAISY parameters failed.");
		     			 finished = false;
		         		 rc = -1;
		         		 return;
		     		 }
    			}
    			break;
    			default:
    	    } 
    	    switch (inType) {
		    	case daisyPools:
				case soilAll:
				case fullXnmXnd:
				{	 boolean ret = myMainWindow.myConnection.importDaisyPoolsFromXnmFile(
		    				 tfXnmFilename.getText(), 
		    				 myMainWindow.myConnection.getNumericIdForIdCode(XnDatabaseConnection.idTypes.daisyPools, (String) lfSomPools.getSelectedValue()), 
		    				 tfProfileId.getText(), 
		    				 true);
		     		 if(ret)
		     		 {	rc = 0;
		     		 	finished = true;
 		     		 }
		     		 else {
		     			 JOptionPane.showMessageDialog(null, "Error: Import of DAISY SOM pool initialization failed.");
		     			 finished = false;
		         		 rc = -1;
		         		 return;
		     		 }
				}
				break;
				default:
		    }  
    	    switch (inType) {
		    	case soilParam:
				case soilAll:
				case fullXnmXnd:
				{	 boolean ret = myMainWindow.myConnection.importSoilParameterizationsFromXnmXndFiles(
		    				 tfXnmFilename.getText(),
		    				 tfXndFilename.getText(), 
		    				 myMainWindow.myConnection.getNumericIdForIdCode(XnDatabaseConnection.idTypes.soilParam, (String) lfSoilParam.getSelectedValue()), 
		    				 tfProfileId.getText(), 
		    				 true);
		     		 if(ret)
		     		 {	rc = 0;
		     		 	finished = true;
		     		 	
		     		 }
		     		 else {
		     			 JOptionPane.showMessageDialog(null, "Error: Import of soil physical parameterization failed.");
		     			 finished = false;
		         		 rc = -1;
		         		 return;
		     		 }
				}
				break;
				default:
		    }  
    	    switch (inType) {
		    	case soilStart:
				case soilAll:
				case fullXnmXnd:
				{	 boolean ret = myMainWindow.myConnection.importSoilInitializationsFromXnmXndFiles( 
		    				 tfXnmFilename.getText(),
		    				 tfXndFilename.getText(), 
		    				 myMainWindow.myConnection.getNumericIdForIdCode(XnDatabaseConnection.idTypes.soilInit, (String) lfSoilInit.getSelectedValue()), 
		    				 tfProfileId.getText(), 
		    				 true);
		     		 if(ret)
		     		 {	rc = 0;
		     		 	finished = true;
		     		 	
		     		 }
		     		 else {
		     			 JOptionPane.showMessageDialog(null, "Error: Import of soil initialization failed.");
		     			 finished = false;
		         		 rc = -1;
		         		 return;
		     		 }
				}
				break;
				default:
		    }     		 
    	    switch (inType) {
		    	case Management:
				case fullXnmXnd:
				{	 boolean ret = myMainWindow.myConnection.importCropManagementFromXndFile( 
		    				 tfXndFilename.getText(), 
		    				 myMainWindow.myConnection.getNumericIdForIdCode(XnDatabaseConnection.idTypes.management, (String) lfManagements.getSelectedValue()) 
		    				 ,true);
		     		 if(ret)
		     		 {	rc = 0;
		     		 	finished = true;
		     		 	
		     		 }
		     		 else {
		     			 JOptionPane.showMessageDialog(null, "Error: Import of crop management failed.");
		     			 finished = false;
		         		 rc = -1;
		         		 return;
		     		 }
				}
				break;
				default:
		    }     		 


    	 }
    	 //dispose();
    }
    public static boolean isAnyOf(xn3ImportType inType, List<xn3ImportType> inTypeList) {
    	for (int i = 0; i< inTypeList.size(); ++i) {
    		if (inType.equals(inTypeList.get(i))) 
    			return true;
    	}
    	return false;
    }
}



//Tool configuration with dialogs
class XnParameterToolConfig implements ActionListener {
	public String dbUser = "";
	public String dbHost = "144.41.15.33";
	public String dbSchema = "for1695_expertN";
	public String dbPwd;
	public String currentAuthor = "???";
	public int currentProjectId = -1;
	public String lastOutputDirectory = ".";
	
	public String lastXnmInDirectory = ".";
	public String lastXndInDirectory = ".";
	
	//Interface: class variables to allow event handling
	JDialog enterConfig ;     
	JTextField tfUser;
	JTextField tfHost;
	JTextField tfSchema;
	JPasswordField tfPwd;
	JButton	buttonSetConnInfo;
	JButton	buttonLoadConfig;
	
	
	public boolean readConfig (String filename) {
		
		try {
			Properties props = new Properties();
			props.load(new FileInputStream(filename));
			dbUser = props.getProperty("dbUser");
			dbHost = props.getProperty("dbHost");
			dbSchema = props.getProperty("dbSchema");
			currentAuthor = props.getProperty("currentAuthor");	
			currentProjectId = Integer.valueOf(props.getProperty("currentProjectId"));
			lastOutputDirectory = props.getProperty("outputDirectory");
			lastXnmInDirectory = props.getProperty("xnmInDirectory");
			lastXndInDirectory = props.getProperty("xndInDirectory");
			tfUser.setText(dbUser);
			tfHost.setText(dbHost);
			tfSchema.setText(dbSchema);
			tfPwd.setText("");
			
		}
		catch (Exception e) 
		{
			e.printStackTrace();
			return false;
		}
		return true;
	}
	public void saveConfig (String filename) {
		try {
			PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(filename)));
			
			out.println("dbUser\t=\t" + dbUser );
			out.println("dbHost\t=\t" + dbHost );
			out.println("dbSchema\t=\t" + dbSchema );
			out.println("currentAuthor\t=\t" + currentAuthor );
			out.println("currentProjectId\t=\t" + currentProjectId );
			out.println("outputDirectory\t=\t" + lastOutputDirectory );
			out.println("xnmInDirectory\t=\t" + lastXnmInDirectory );
			out.println("xndInDirectory\t=\t" + lastXndInDirectory );

			out.close();
			
		}
		catch (Exception e) 
		{
			  e.printStackTrace();
		}
	}

	public void askForConfigEntries() {
		enterConfig = new JDialog();
		enterConfig.setSize(400,400);
		enterConfig.setDefaultCloseOperation(JDialog.DO_NOTHING_ON_CLOSE);

		
		Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
		enterConfig.setLocation(screenSize.width/2-enterConfig.getSize().width/2, screenSize.height/2-enterConfig.getSize().height/2);

        JPanel panel = new JPanel();
        panel.setLayout( new java.awt.GridLayout(5, 2, 1,40) );
        panel.setBorder(new EmptyBorder(10, 10, 10, 10));
        
        JLabel userLabel = new JLabel("dbUser");
        panel.add(userLabel);
        tfUser = new JTextField(dbUser, 20);
        panel.add(tfUser);
        
        JLabel hostLabel = new JLabel("dbHost");
        panel.add(hostLabel);
        tfHost = new JTextField(dbHost, 20);
        panel.add(tfHost);
        
        JLabel schemaLabel = new JLabel("dbSchema");
        panel.add(schemaLabel);
        tfSchema = new JTextField(dbSchema, 20);
        panel.add(tfSchema);
         
        JLabel pwdLabel = new JLabel("Password:");
        panel.add(pwdLabel);
        tfPwd = new JPasswordField("", 20);
        panel.add(tfPwd);
        
        buttonSetConnInfo = new JButton("OK");
        
        //anonymous class for event action
        buttonSetConnInfo.addActionListener(this);
        buttonSetConnInfo.setSelected(true);
        panel.add(buttonSetConnInfo);
        
        
        buttonLoadConfig = new JButton("Load configuration ...");
        buttonLoadConfig.addActionListener(this);
        panel.add(buttonLoadConfig);

        
        enterConfig.add(panel);
        enterConfig.setModal(true);
		enterConfig.setVisible(true);
		
	}

	

	//Event handler
    public void actionPerformed(ActionEvent ae) {
    	 if(ae.getSource() == this.buttonSetConnInfo){
    		 dbUser = tfUser.getText();
    		 dbHost = tfHost.getText();
    		 dbSchema = tfSchema.getText();
    		 dbPwd = String.valueOf(tfPwd.getPassword());
    		 enterConfig.setModal(false);
    		 enterConfig.dispose();
    	 }
    	 if(ae.getSource() == this.buttonLoadConfig){
			loadConfig();
		}
    }
    public void loadConfig () {
		JFileChooser chooser = new JFileChooser();
        int rc = chooser.showOpenDialog(null);
        
        /* Abfrage, ob auf "Öffnen" geklickt wurde */
        if(rc == JFileChooser.APPROVE_OPTION)
        {
        	String configFile =   chooser.getSelectedFile().getAbsolutePath();
        	if (! readConfig(configFile) ) {
    			JOptionPane.showMessageDialog(null, "Error: Could not read configuration.");
    			return;
        	}
        }
    }
}


class  createNewPlantParamDialog extends JDialog  implements ActionListener {
	   
	private plantParam result; 
	private JButton buttonDialogNewPlantParamCancel;
	private JButton buttonDialogNewPlantParamOk;
	private JButton buttonDialogNewPlantParamAddAuthor;
	private JTextField tfCode;
	private JTextArea tfDesc;
	private JList lfAuthor;
	private DefaultListModel lfAuthorModel;
	private XnDatabaseConnection myConnection;
	private XnParameterToolConfig myConfig ;

	private boolean finished;
	
	public plantParam showDialog(XnParameterToolConfig conf, XnDatabaseConnection conn) {
	
		myConnection = conn;
		myConfig = conf;
		finished = false;
		
		this.setSize(400,400);
		this.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);

	    JPanel panel = new JPanel();
	    panel.setLayout( new java.awt.GridLayout(5, 3, 1,40) );
	    panel.setBorder(new EmptyBorder(10, 10, 10, 10));
    
	    JLabel codeLabel = new JLabel("Plant parameterization code");
	    panel.add(codeLabel);
	    tfCode = new JTextField("", 30);
	    panel.add(tfCode);
	    panel.add(new JLabel());
	    
	    JLabel descLabel = new JLabel("Description");
	    panel.add(descLabel);
	    tfDesc = new JTextArea("", 30,5);
	    panel.add(tfDesc);
	    panel.add(new JLabel());
	    
	    JLabel schemaLabel = new JLabel("Author:");
	    panel.add(schemaLabel);
	    
	    
	    lfAuthorModel = new DefaultListModel();
        ResultSet rs =  myConnection.getListOfAuthors();
        
        try {
	        while (rs.next())
	        {
	        	lfAuthorModel.addElement(rs.getString("author_id"));
	        }
	        rs.close();
        }
        catch(Exception e)
        {
        	e.printStackTrace();
        }

	    lfAuthor = new JList(lfAuthorModel);
	    lfAuthor.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
	    
        JScrollPane alistScroll = new JScrollPane(lfAuthor, ScrollPaneLayout.VERTICAL_SCROLLBAR_ALWAYS,ScrollPaneLayout.HORIZONTAL_SCROLLBAR_AS_NEEDED);
        panel.add(alistScroll);
	    
	 
	    
	    
	    buttonDialogNewPlantParamAddAuthor = new JButton("Add Author ...");
	    buttonDialogNewPlantParamAddAuthor.addActionListener(this);
	    panel.add(buttonDialogNewPlantParamAddAuthor);
    
	    buttonDialogNewPlantParamOk = new JButton("OK"); 
	    buttonDialogNewPlantParamOk.addActionListener(this);
	    buttonDialogNewPlantParamOk.setSelected(true);
    	panel.add(buttonDialogNewPlantParamOk);
    
    
    	buttonDialogNewPlantParamCancel = new JButton("Cancel");
    	buttonDialogNewPlantParamCancel.addActionListener(this);
    	panel.add(buttonDialogNewPlantParamCancel);

    
    	this.add(panel);
    	this.setModal(true);
    	while(!finished)
    	{
    		this.setVisible(true);
    	}
    	return result;
	}
	
	public void actionPerformed(ActionEvent e){
    	
    	if (e.getSource() == buttonDialogNewPlantParamOk)
    	{
    		String authorToCheck = (String) lfAuthor.getSelectedValue();
    		String code = tfCode.getText();

    		String description = tfDesc.getText();

    		
    		if (! myConnection.checkAuthorName(authorToCheck))
    		{
    			JOptionPane.showMessageDialog(null, "Error: Invalid author.");
    			dispose();
    			return;
    		}
    		
    		int rc = myConnection.addPlantParameterization(code, description, authorToCheck);
    		if (rc == -2)
    		{
    			JOptionPane.showMessageDialog(null, "Error: Plant parameterization name already used.");
    			dispose();
    			return;
    		}
    		else if (rc == -1)
    		{
    			JOptionPane.showMessageDialog(null, "Error: Adding plant parameterization failed.");
    			dispose();
    			return;
    		}
    		else 
    		{
    			JOptionPane.showMessageDialog(null, "New plant parameterization '" + code +  "' has been created.");
    		}
    		
    		plantParam pp = new plantParam(rc, code , description, authorToCheck);
    		result = pp;
    		finished = true;
    		dispose();
    	}
    	if (e.getSource() == buttonDialogNewPlantParamCancel)
    	{
    		result = null;
    		finished = true;
    		dispose();
    	}
       	if (e.getSource() == buttonDialogNewPlantParamAddAuthor)
    	{
       		createNewAuthorDialog dlg = new createNewAuthorDialog();
       		author na  = dlg.showDialog(myConfig, myConnection);
       		lfAuthorModel.addElement(na.author_id);
       		lfAuthor.setSelectedValue(na.author_id, true);

       		finished = false;
       		
       		dispose();
    	}
    	

	}
		
}

class  createNewDaisyParamDialog extends JDialog  implements ActionListener {
	   
	private daisyParam result; 
	private JButton buttonDialogNewParamCancel;
	private JButton buttonDialogNewParamOk;
	private JButton buttonDialogNewParamAddAuthor;
	private JTextField tfCode;
	private JTextArea tfDesc;
	private JList lfAuthor;
	private DefaultListModel lfAuthorModel;
	private XnDatabaseConnection myConnection;
	private XnParameterToolConfig myConfig ;

	private boolean finished;
	
	public daisyParam showDialog(XnParameterToolConfig conf, XnDatabaseConnection conn) {
	
		myConnection = conn;
		myConfig = conf;
		finished = false;
		
		this.setSize(400,400);
		this.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);

	    JPanel panel = new JPanel();
	    panel.setLayout( new java.awt.GridLayout(5, 3, 1,40) );
	    panel.setBorder(new EmptyBorder(10, 10, 10, 10));
    
	    JLabel codeLabel = new JLabel("Daisy parameterization code");
	    panel.add(codeLabel);
	    tfCode = new JTextField("", 30);
	    panel.add(tfCode);
	    panel.add(new JLabel());
	    
	    JLabel descLabel = new JLabel("Description");
	    panel.add(descLabel);
	    tfDesc = new JTextArea("", 30,5);
	    panel.add(tfDesc);
	    panel.add(new JLabel());
	    
	    JLabel schemaLabel = new JLabel("Author:");
	    panel.add(schemaLabel);
	    
	    
	    lfAuthorModel = new DefaultListModel();
        ResultSet rs =  myConnection.getListOfAuthors();
        
        try {
	        while (rs.next())
	        {
	        	lfAuthorModel.addElement(rs.getString("author_id"));
	        }
	        rs.close();
        }
        catch(Exception e)
        {
        	e.printStackTrace();
        }

	    lfAuthor = new JList(lfAuthorModel);
	    lfAuthor.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
	    
        JScrollPane alistScroll = new JScrollPane(lfAuthor, ScrollPaneLayout.VERTICAL_SCROLLBAR_ALWAYS,ScrollPaneLayout.HORIZONTAL_SCROLLBAR_AS_NEEDED);
        panel.add(alistScroll);
	    
	 
	    
	    
	    buttonDialogNewParamAddAuthor = new JButton("Add Author ...");
	    buttonDialogNewParamAddAuthor.addActionListener(this);
	    panel.add(buttonDialogNewParamAddAuthor);
    
	    buttonDialogNewParamOk = new JButton("OK"); 
	    buttonDialogNewParamOk.addActionListener(this);
	    buttonDialogNewParamOk.setSelected(true);
    	panel.add(buttonDialogNewParamOk);
    
    
    	buttonDialogNewParamCancel = new JButton("Cancel");
    	buttonDialogNewParamCancel.addActionListener(this);
    	panel.add(buttonDialogNewParamCancel);

    
    	this.add(panel);
    	this.setModal(true);
    	while(!finished)
    	{
    		this.setVisible(true);
    	}
    	return result;
	}
	
	public void actionPerformed(ActionEvent e){
    	
    	if (e.getSource() == buttonDialogNewParamOk)
    	{
    		String authorToCheck = (String) lfAuthor.getSelectedValue();
    		String code = tfCode.getText();

    		String description = tfDesc.getText();

    		
    		if (! myConnection.checkAuthorName(authorToCheck))
    		{
    			JOptionPane.showMessageDialog(null, "Error: Invalid author.");
    			dispose();
    			return;
    		}
    		
    		int rc = myConnection.addDaisyParameterization(code, description, authorToCheck);
    		if (rc == -2)
    		{
    			JOptionPane.showMessageDialog(null, "Error: DAISY parameterization name already used.");
    			dispose();
    			return;
    		}
    		else if (rc == -1)
    		{
    			JOptionPane.showMessageDialog(null, "Error: Adding DAISY parameterization failed.");
    			dispose();
    			return;
    		}
    		else 
    		{
    			JOptionPane.showMessageDialog(null, "New DAISY parameterization '" + code +  "' has been created.");
    		}
    		
    		daisyParam pp = new daisyParam(rc, code , description, authorToCheck);
    		result = pp;
    		finished = true;
    		dispose();
    	}
    	if (e.getSource() == buttonDialogNewParamCancel)
    	{
    		result = null;
    		finished = true;
    		dispose();
    	}
       	if (e.getSource() == buttonDialogNewParamAddAuthor)
    	{
       		createNewAuthorDialog dlg = new createNewAuthorDialog();
       		author na  = dlg.showDialog(myConfig, myConnection);
       		lfAuthorModel.addElement(na.author_id);
       		lfAuthor.setSelectedValue(na.author_id, true);

       		finished = false;
       		
       		dispose();
    	}
    	

	}
		
}

class  createNewIdCodeDialog extends JDialog  implements ActionListener {
	   
	private idCode result; 
	private JButton buttonDialogNewParamCancel;
	private JButton buttonDialogNewParamOk;
	private JButton buttonDialogNewParamAddAuthor;
	private JTextField tfCode;
	private JTextArea tfDesc;
	private JList lfAuthor;
	private DefaultListModel lfAuthorModel;
	private XnDatabaseConnection myConnection;
	private XnParameterToolConfig myConfig ;

	private boolean finished;
	
	private XnDatabaseConnection.idTypes idType;
	private String what;
	
	public idCode showDialog(XnParameterToolConfig conf, XnDatabaseConnection conn, XnDatabaseConnection.idTypes which) {
	
		myConnection = conn;
		myConfig = conf;
		
		idType = which;
		switch(idType) {
			case daisyParam:
				what = "DAISY parameterization";
				break;
			case daisyPools:
				what = "SOM pools intialization";
				break;
			case soilInit:
				what = "Soil intialization";
				break;
			case soilParam:
				what = "Soil parameterization";
				break;
			case management:
				what = "Crop management";
				break;

		}
		
		finished = false;

		this.setSize(400,400);
		setTitle("Create new " + what +"...");
		
	    JPanel panel = new JPanel();
	    panel.setLayout( new java.awt.GridLayout(5, 3, 1,40) );
	    panel.setBorder(new EmptyBorder(10, 10, 10, 10));
    
	    JLabel codeLabel = new JLabel("Code");
	    panel.add(codeLabel);
	    tfCode = new JTextField("", 30);
	    panel.add(tfCode);
	    panel.add(new JLabel());
	    
	    JLabel descLabel = new JLabel("Description");
	    panel.add(descLabel);
	    tfDesc = new JTextArea("", 30,5);
	    panel.add(tfDesc);
	    panel.add(new JLabel());
	    
	    JLabel schemaLabel = new JLabel("Author:");
	    panel.add(schemaLabel);
	    
	    
	    lfAuthorModel = new DefaultListModel();
        ResultSet rs =  myConnection.getListOfAuthors();
        
        try {
	        while (rs.next())
	        {
	        	lfAuthorModel.addElement(rs.getString("author_id"));
	        }
	        rs.close();
        }
        catch(Exception e)
        {
        	e.printStackTrace();
        }

	    lfAuthor = new JList(lfAuthorModel);
	    lfAuthor.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
	    
        JScrollPane alistScroll = new JScrollPane(lfAuthor, ScrollPaneLayout.VERTICAL_SCROLLBAR_ALWAYS,ScrollPaneLayout.HORIZONTAL_SCROLLBAR_AS_NEEDED);
        panel.add(alistScroll);
	    
	 
	    
	    
	    buttonDialogNewParamAddAuthor = new JButton("Add Author ...");
	    buttonDialogNewParamAddAuthor.addActionListener(this);
	    panel.add(buttonDialogNewParamAddAuthor);
    
	    buttonDialogNewParamOk = new JButton("OK"); 
	    buttonDialogNewParamOk.addActionListener(this);
	    buttonDialogNewParamOk.setSelected(true);
    	panel.add(buttonDialogNewParamOk);
    
    
    	buttonDialogNewParamCancel = new JButton("Cancel");
    	buttonDialogNewParamCancel.addActionListener(this);
    	panel.add(buttonDialogNewParamCancel);

    
    	this.add(panel);
    	this.setModal(true);
    	while(!finished)
    	{
    		this.setVisible(true);
    	}
    	return result;
	}
	
	public void actionPerformed(ActionEvent e){
    	
    	if (e.getSource() == buttonDialogNewParamOk)
    	{
    		String authorToCheck = (String) lfAuthor.getSelectedValue();
    		String code = tfCode.getText();

    		String description = tfDesc.getText();

    		
    		if (! myConnection.checkAuthorName(authorToCheck))
    		{
    			JOptionPane.showMessageDialog(null, "Error: Invalid author.");
    			dispose();
    			return;
    		}
    		
    		int rc = myConnection.addIdCode(idType, code, description, authorToCheck);
    		if (rc == -2)
    		{
    			JOptionPane.showMessageDialog(null, "Error: code already used.");
    			dispose();
    			return;
    		}
    		else if (rc == -1)
    		{
    			JOptionPane.showMessageDialog(null, "Error: Adding "+what+" failed.");
    			dispose();
    			return;
    		}
    		else 
    		{
    			JOptionPane.showMessageDialog(null, "New "+what+ ": '" + code +  "' has been created.");
    		}
    		
    		idCode pp = new idCode(rc, code , description, authorToCheck);
    		result = pp;
    		finished = true;
    		dispose();
    	}
    	if (e.getSource() == buttonDialogNewParamCancel)
    	{
    		result = null;
    		finished = true;
    		dispose();
    	}
       	if (e.getSource() == buttonDialogNewParamAddAuthor)
    	{
       		createNewAuthorDialog dlg = new createNewAuthorDialog();
       		author na  = dlg.showDialog(myConfig, myConnection);
       		lfAuthorModel.addElement(na.author_id);
       		lfAuthor.setSelectedValue(na.author_id, true);

       		finished = false;
       		
       		dispose();
    	}
    	

	}
		
}

class  createNewAuthorDialog extends JDialog  implements ActionListener {
	   
	private author result; 
	private JButton buttonDialogNewAuthorCancel;
	private JButton buttonDialogNewAuthorOk;
	private JTextField tfFirstName;
	private JTextField tfSurname;
	private JTextField tfAuthor;
	private XnDatabaseConnection myConnection;
	private XnParameterToolConfig myConfig;
	private boolean finished;
	
	public author showDialog(XnParameterToolConfig conf, XnDatabaseConnection conn) {
		myConfig = conf;
		myConnection = conn;
		finished = false;
		
		this.setSize(400,400);
		this.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
		
	    JPanel panel = new JPanel();
	    panel.setLayout( new java.awt.GridLayout(5, 2, 1,40) );
	    panel.setBorder(new EmptyBorder(10, 10, 10, 10));
    
	    JLabel codeLabel = new JLabel("Author code");
	    panel.add(codeLabel);
	    tfAuthor = new JTextField("", 30);
	    panel.add(tfAuthor);
	    
	    JLabel firstLabel = new JLabel("First name:");
	    panel.add(firstLabel);
	    tfFirstName = new JTextField("", 30);
	    panel.add(tfFirstName);
	    
	    JLabel surnameLabel = new JLabel("Surname");
	    panel.add(surnameLabel);
	    tfSurname = new JTextField("", 30);
	    panel.add(tfSurname);
    
	    buttonDialogNewAuthorOk = new JButton("OK"); 
	    buttonDialogNewAuthorOk.addActionListener(this);
	    buttonDialogNewAuthorOk.setSelected(true);
    	panel.add(buttonDialogNewAuthorOk);
    
    
    	buttonDialogNewAuthorCancel = new JButton("Cancel");
    	buttonDialogNewAuthorCancel.addActionListener(this);
    	panel.add(buttonDialogNewAuthorCancel);

    
    	this.add(panel);
    	this.setModal(true);
    	while(!finished)
    	{
    		this.setVisible(true);
    	}
    	return result;
	}
	
	public void actionPerformed(ActionEvent e){
    	
    	if (e.getSource() == buttonDialogNewAuthorOk)
    	{
    		String authorToCheck = tfAuthor.getText();
    		String first = tfFirstName.getText();
    		String last = tfSurname.getText();

    		int rc = myConnection.addAuthor(authorToCheck, first,last);
    		if (rc == -2)
    		{
    			JOptionPane.showMessageDialog(null, "Error: Author code already used.");
    			dispose();
    			return;
    		}
    		else if (rc == -1)
    		{
    			JOptionPane.showMessageDialog(null, "Error: Adding author failed.");
    			dispose();
    			return;
    		}
    		else 
    		{
    			JOptionPane.showMessageDialog(null, "New author '" + authorToCheck +  "' has been created.");
    		}
    		
    		author pp = new author(authorToCheck, first,last);
    		result = pp;
    		finished = true;
    		dispose();
    	}
    	if (e.getSource() == buttonDialogNewAuthorCancel)
    	{
    		result = null;
    		finished = true;
    		dispose();
    	}
    	

	}
		
}

//AUXILIARY CLASSES
class cropToExport {
	int param_id;
	String crop;
	String variety; 
	public cropToExport (int _param_id,String _crop, 	String _variety) {
		param_id = _param_id;
		crop = _crop;
		variety = _variety;
	}
}
class simProject {
	int simulation_project_id;
	String simulation_project_code;
	String simulation_project_description;
	String author_id;
	int project_type;
	
	String xn5_cells_table;
	String bems_cells_management_table;
	
	public simProject (ResultSet rs)
	{
		try {
			simulation_project_id = rs.getInt("simulation_project_id");
			simulation_project_code = rs.getString("simulation_project_code");
			simulation_project_description = rs.getString("simulation_project_description");
			author_id = rs.getString("author_id");
			project_type = rs.getInt("type_of_project");
			
			xn5_cells_table = rs.getString("xn5_cells_table");
			bems_cells_management_table  = rs.getString("bems_cells_management_table");
			
		}
		catch(Exception e) {
			e.printStackTrace();
		}
		
	}
	public simProject (int id, String code, String description, String author, int type)
	{
			simulation_project_id = id;
			simulation_project_code = code;
			simulation_project_description = description;
			author_id = author;		
			project_type = type;
			xn5_cells_table = "simulation_projects_xn5_cells";
			bems_cells_management_table = "simulation_projects_bems_cells_management";
	}
}
class plantParam {
	int plant_param_id;
	String plant_param_code;
	String plant_param_description;
	String author_id;
	public plantParam (ResultSet rs)
	{
		try {
			plant_param_id = rs.getInt("plant_param_id");
			plant_param_code = rs.getString("plant_param_code");
			plant_param_description = rs.getString("plant_param_description");
			author_id = rs.getString("author_id");
		}
		catch(Exception e) {
			e.printStackTrace();
		}
		
	}
	public plantParam (int id, String code, String description, String author)
	{
		plant_param_id = id;
		plant_param_code = code;
		plant_param_description = description;
		author_id = author;		
	}
}
class daisyParam {
	int daisy_param_id;
	String daisy_param_code;
	String daisy_param_description;
	String author_id;
	public daisyParam (ResultSet rs)
	{
		try {
			daisy_param_id = rs.getInt("daisy_param_id");
			daisy_param_code = rs.getString("daisy_param_code");
			daisy_param_description = rs.getString("daisy_param_description");
			author_id = rs.getString("author_id");
		}
		catch(Exception e) {
			e.printStackTrace();
		}
		
	}
	public daisyParam (int id, String code, String description, String author)
	{
		daisy_param_id = id;
		daisy_param_code = code;
		daisy_param_description = description;
		author_id = author;		
	}
}
class idCode {
	public int numeric_id;
	public String code;
	public String description;
	public String author_id;
	public idCode (ResultSet rs)
	{
		try {
			numeric_id = rs.getInt(1);
			code = rs.getString(2);
			description = rs.getString(3);
			author_id = rs.getString(4);
		}
		catch(Exception e) {
			e.printStackTrace();
		}
		
	}
	public idCode (int id, String _code, String _description, String author)
	{
		numeric_id = id;
		code = _code;
		description = _description;
		author_id = author;		
	}
}

class author {
	
	String author_first_name;
	String author_surname;
	String author_id;
	public author (ResultSet rs)
	{
		try {
			author_id = rs.getString("author_id");
			author_first_name = rs.getString("author_first_name");
			author_surname = rs.getString("author_surname");

		}
		catch(Exception e) {
			e.printStackTrace();
		}
		
	}
	public author (String author, String first, String last)
	{
		author_first_name = first;
		author_surname = last;
		author_id = author;		
	}
}


