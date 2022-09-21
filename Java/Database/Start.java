import java.sql.*;

public class Start {
	static final String JDBC_DRIVER = "org.postgresql.Driver"; 
	static final String DB_URL = "jdbc:postgresql://localhost:5432/trains_system";
	static final String USER = "postgres";
	static final String PASS = "passwd1234";
	
	public static void insert(String[] args) {
                Connection conn = null; 
                if(args.length != 0){
                	String query = args[0];
                	
                	try{
                		Class.forName("org.postgresql.Driver");
                		conn = DriverManager.getConnection(DB_URL, USER, PASS);
                		
                		System.out.println("Connection succes!");
                		
                		conn.setAutoCommit(false);
                		//String query = "SELECT * FROM trains LIMIT 2";
                		//PreparedStatement stmt = conn.prepareStatement(query);
                		//stmt.executeQuery();
                		
                		Statement stmt = conn.createStatement();
                        stmt.executeUpdate(query);
                        
                        
                		//ResultSet rs = stmt.executeQuery();
                		/*
                		while ( rs.next() ) {
                			int id = rs.getInt("id_train");
                			String  type = rs.getString("type");
                			String  train_name = rs.getString("train_name");
                			short num_cars = rs.getShort("number_of_cars");
                        
                			System.out.println( "ID train = " + id );
                			System.out.println( "TYPE = " + type );
                			System.out.println( "train name = " + train_name );
                			System.out.println( "# of cars = " + num_cars );
                			System.out.println();
                		}
                		*/
                		//rs.close();
                		
                        stmt.close();
                        conn.commit();
                		conn.close();
                    
                    
                	}catch(Exception e){
                		e.printStackTrace(); 
                		System.exit(1);
                	}
                	System.out.println("Goodbye!");
                }
                else System.out.println("No query were passed");
                
                
                
                
	}
}