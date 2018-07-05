package de.uhoh.for1695;

import java.sql.*;

class MySQLExample 
{
	public static void main(String[] args) 
	{
		//Default initialization
		String host = "localhost";
		String dbName = "test";
		String userName = "";
		//Processing of command line arguments
		for (int i = 0; i < args.length; i++) {
			
			switch (args[i].charAt(1) ) {
				case 104: host= args[i].substring(2);break;//-h host name
				case 100: dbName= args[i].substring(2);break;//-d database name
				case 117: userName= args[i].substring(2);break;//-u database name
			}
		}
		String password = "";
		try 
		{
			System.out.println("Password:");
			password = new jline.ConsoleReader().readLine(new Character('*'));
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
		MySQLConnection c = new MySQLConnection (host, dbName, userName, password);
		
		ResultSet rs = c.query("SELECT product, year, price FROM tbl_prices");
		try
		{
			while (rs.next())
			{
				System.out.println(rs.getString("product") +"\t"+  rs.getInt("year") + "\t" + rs.getDouble("price") );
			}
		}
		catch (Exception e) 
		{
			e.printStackTrace();
		}
	}
}