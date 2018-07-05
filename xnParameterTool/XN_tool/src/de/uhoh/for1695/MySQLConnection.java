package de.uhoh.for1695;

import java.sql.*;



public class MySQLConnection {
	
	Connection conn;
	boolean valid = false;
	Exception lastError;
	
	public MySQLConnection ( String host, String dbName , String userName, String password)
	{
		  conn = null;
		  String url = "jdbc:mysql://" + host;
		  String driver = "com.mysql.jdbc.Driver";

		  try 
		  {
			  Class.forName(driver).newInstance();
			  System.out.println("Trying to connect to " + url + "/" + dbName + " on " + host + " as " + userName );
			  conn = DriverManager.getConnection(url+ "/"+dbName,userName,password);
			  System.out.println("Connected to " + dbName + " on " + host );
			  valid = true;
		  } 
		  catch (Exception e) 
		  {
			  lastError = e;
			  valid = false;
			  
			  
		  }
		  
	}
	public boolean isValid() {
		return valid;
	}
	public Exception getLastError() {
		return lastError;
	}
	
	protected void finalize () {
		  try 
		  {
				conn.close();
		  }
		  catch (Exception e) 
		  {
			  e.printStackTrace();
		  }
		  System.out.println("Disconnected from database");

	}
	public ResultSet query (String SQLString)
	{
	// runs the query passed to the function and returns the ResultSet
	// (generic function)
	  try {
		Statement stmt = conn.createStatement();
		ResultSet rs = stmt.executeQuery(SQLString);
		return rs;
	  } 
	  catch (Exception e) 
	  {
		  e.printStackTrace();
		  return null;
	  }
	}
	public boolean updateDb (String SQLString)
	{
	// runs the query passed to the function and returns the ResultSet
	// (generic function)
	  try {
		Statement stmt = conn.createStatement();
		stmt.executeUpdate(SQLString);
		return true;
	  } 
	  catch (Exception e) 
	  {
		  e.printStackTrace();
		  return false;
	  }
	}
	public String getStringFromDB (String SQLString)
	{
	  // runs the query passed to the function and returns the string found in the first column of the first row of the resultset
	  // intended to retrieve single string values
	  try {
		Statement stmt = conn.createStatement();
		ResultSet rs = stmt.executeQuery(SQLString);
		rs.first();
		return rs.getString(1);
	  } 
	  catch (Exception e) 
	  {
		  e.printStackTrace();
		  return null;
	  }
	}
	public double getDoubleFromDB (String SQLString)
	{
	  // runs the query passed to the function and returns the string found in the first column of the first row of the resultset
	  // intended to retrieve single double values
	  try {
		Statement stmt = conn.createStatement();
		ResultSet rs = stmt.executeQuery(SQLString);
		rs.first();
		return rs.getDouble(1);
	  } 
	  catch (Exception e) 
	  {
		  e.printStackTrace();
		  return 0;
	  }
	}
	public int getIntFromDB (String SQLString)
	{
	  // runs the query passed to the function and returns the string found in the first column of the first row of the resultset
	  // intended to retrieve single integer values
	  try {
		Statement stmt = conn.createStatement();
		ResultSet rs = stmt.executeQuery(SQLString);
		rs.first();
		return rs.getInt(1);
	  } 
	  catch (Exception e) 
	  {
		  e.printStackTrace();
		  return 0;
	  }
	}
	public Connection getConnectionObject() {
		return conn;
	}
}


