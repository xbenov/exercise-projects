import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;

import javafx.application.*;
import javafx.beans.property.SimpleStringProperty;
import javafx.beans.value.ObservableValue;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.scene.*;
import javafx.scene.control.*;
import javafx.scene.control.TableColumn.CellDataFeatures;
import javafx.scene.layout.*;
import javafx.stage.*;
import javafx.util.Callback;



public class StartGUI extends Application{
	static final String JDBC_DRIVER = "org.postgresql.Driver"; 
	static final String DB_URL = "jdbc:postgresql://localhost:5432/trains_system";
	static final String USER = "postgres";
	static final String PASS = "passwd1234";
	int offset = 20;
	int count = 1;
	
	private Button scenar1 = new Button("1. scenár");
	private Button scenar2 = new Button("2. scenár");
	private Button scenar3 = new Button("3. scenár");
	private Button scenar4 = new Button("4. scenár");
	private Button scenar5 = new Button("5. scenár");
	private Label label1 = new Label(" Pridanie listka ");
	private Label label2 = new Label(" Prehlad atributov jednotlivych voznov ");
	private Label label3 = new Label(" Prehlad miesteniek na danej trase ");
	private Label label4 = new Label(" Zoznam zakaznikov ");
	private Label label5 = new Label(" Vratenie miestenky ");
	
	private Button getBack = new Button("Naspat");
	private Label scenar1_label1 = new Label(" ID cesty: ");
	private Label scenar1_label2 = new Label(" ID zakaznika: ");
	private Label scenar1_label3 = new Label(" Typ listka: ");
	private TextField idRoute = new TextField();
	private TextField idCustomer = new TextField();
	private Button insertNovyListok = new Button("Pridaj listok");
	ObservableList<String> typListka;
	ComboBox<String> vyberTypuListka;
	
	private Button getBack2 = new Button("Naspat");
	private Label scenar2_label1 = new Label(" Trieda: ");
	@SuppressWarnings("rawtypes")
	private ObservableList<ObservableList> pasCars;
	@SuppressWarnings("rawtypes")
	private TableView tabulka = new TableView();
	ObservableList<String> trieda;
	ComboBox<String> vyberTriedy;
	private CheckBox wifi = new CheckBox("Wi-Fi");
	private CheckBox restCar = new CheckBox("Restauracny vozen");
	private CheckBox bikeTransport = new CheckBox("Preprava bicyklov");
	private CheckBox airCondition = new CheckBox("Klimatizacia");
	private Button filter2 = new Button("Zobraz vysledky");
	private Button dalsiaStrana = new Button("Dalsia strana");
	private Button predchStrana = new Button("Predchadzajuca strana");
	
	public Connection getConnection() {
        Connection conn = null; 
        	try{
        		Class.forName("org.postgresql.Driver");
        		conn = DriverManager.getConnection(DB_URL, USER, PASS);
        		
        		System.out.println("Connection succes!");
        		
        		conn.setAutoCommit(false);            
            
        	}catch(Exception e){
        		e.printStackTrace(); 
        		System.exit(1);
        	}
        	System.out.println("Goodbye!");
        	
        	return conn;
        }
	
	
	@SuppressWarnings({ "unchecked", "rawtypes" })
	public void start(Stage hlavneOkno){
		hlavneOkno.setTitle("DBS - Beňo, Hoferica");
		FlowPane pane1 = new FlowPane();
		FlowPane pane2 = new FlowPane();
		FlowPane pane3 = new FlowPane();
		FlowPane pane4 = new FlowPane();
		FlowPane pane5 = new FlowPane();
		FlowPane prvyScenar = new FlowPane();
		FlowPane druhyScenar = new FlowPane();
		
		VBox menuButtons = new VBox(5);
		Scene mainScene = new Scene(menuButtons, 860, 500);
		Scene firstScene = new Scene(prvyScenar, 860, 500);
		Scene secondScene = new Scene(druhyScenar, 860, 500);
		//nachystane dalsie scenare
		/*
		Scene thirdScene = new Scene(menuButtons, 860, 500);
		Scene fourthScene = new Scene(menuButtons, 860, 500);
		Scene fifthScene = new Scene(menuButtons, 860, 500);
		*/
		
		pane1.setHgap(10);
		pane1.getChildren().addAll(scenar1, label1);
		pane2.setHgap(10);
		pane2.getChildren().addAll(scenar2, label2);
		pane3.setHgap(10);
		pane3.getChildren().addAll(scenar3, label3);
		pane4.setHgap(10);
		pane4.getChildren().addAll(scenar4, label4);
		pane5.setHgap(10);
		pane5.getChildren().addAll(scenar5, label5);
		
		typListka = FXCollections.observableArrayList("child","student","adult","senior");
		vyberTypuListka = new ComboBox<String>(typListka);
		vyberTypuListka.getSelectionModel().select(2);
		prvyScenar.getChildren().addAll(getBack, scenar1_label1, idRoute, scenar1_label2, idCustomer, 
				scenar1_label3, vyberTypuListka, insertNovyListok);
		
		
		trieda = FXCollections.observableArrayList("1.","2.");
		vyberTriedy = new ComboBox<String>(trieda);
		vyberTriedy.getSelectionModel().select(1);
		druhyScenar.setHgap(10);
		druhyScenar.setVgap(10);
		druhyScenar.getChildren().addAll(getBack2, scenar2_label1, vyberTriedy, wifi, 
				restCar, bikeTransport, airCondition, filter2, tabulka, predchStrana, dalsiaStrana);
		
		
		
		insertNovyListok.setOnAction(e-> {
			String temp_route = idRoute.getText();
			String temp_cust = idCustomer.getText();
			String temp_type = vyberTypuListka.getValue();
			if(temp_cust.equals("")){
				temp_cust = "NULL";
			}
			
			String query = "INSERT INTO tickets (id_ticket, id_customer, id_route, type) "
					+ "VALUES ((SELECT MAX(id_ticket)+1 FROM tickets)," + 
					temp_cust + "," + temp_route + "," + "'" + temp_type + "')" ;
			Start.insert( new String[]{query});
			idRoute.clear();
			idCustomer.clear();
			
			
		});
		
		filter2.setOnAction(e-> {
			count = 1;
			
			if(count == 1)
				predchStrana.setDisable(true);
			
			tabulka.getItems().clear();
			String query = "SELECT * FROM (SELECT * FROM (SELECT * FROM "
					+ "passenger_cars WHERE class = "	+ vyberTriedy.getValue().charAt(0);
			if(wifi.isSelected())
				query = query + " and wifi";
			if(bikeTransport.isSelected())
				query = query + " and bicycle_transport";
			if(restCar.isSelected())
				query = query + " and restaurant_car";
			if(airCondition.isSelected())
				query = query + " and air_conditioning";
			
			query = query + " ORDER BY 1 ASC LIMIT " + Integer.toString(offset) + ") AS q1 "
					+ "ORDER BY 1 DESC LIMIT 20) AS q2 ORDER BY 1";
			
			pasCars = FXCollections.observableArrayList();
			try{
				Connection conn = getConnection();
        		PreparedStatement stmt = conn.prepareStatement(query);
        		ResultSet rs = stmt.executeQuery();
                

                while(rs.next()){
                    ObservableList<String> row = FXCollections.observableArrayList();
                    for(int i=1 ; i<=rs.getMetaData().getColumnCount(); i++){
                        row.add(rs.getString(i));
                    }
                    pasCars.add(row);

                }

                tabulka.setItems(pasCars);                
        		
        		rs.close();
        		stmt.close();
                conn.commit();
        		conn.close();
        		
        	}catch(Exception e1){
        		e1.printStackTrace(); 
        		System.exit(1);
        	}
			
		});
		
		dalsiaStrana.setOnAction(e-> {
			count++;
			
			if(count > 1)
				predchStrana.setDisable(false);
			
			tabulka.getItems().clear();
			String query = "SELECT * FROM (SELECT * FROM (SELECT * FROM "
					+ "passenger_cars WHERE class = "	+ vyberTriedy.getValue().charAt(0);
			if(wifi.isSelected())
				query = query + " and wifi";
			if(bikeTransport.isSelected())
				query = query + " and bicycle_transport";
			if(restCar.isSelected())
				query = query + " and restaurant_car";
			if(airCondition.isSelected())
				query = query + " and air_conditioning";
			
			query = query + " ORDER BY 1 ASC LIMIT " + Integer.toString(count*offset) + ") AS q1 "
					+ "ORDER BY 1 DESC LIMIT 20) AS q2 ORDER BY 1";
			
			pasCars = FXCollections.observableArrayList();
			try{
				Connection conn = getConnection();
        		PreparedStatement stmt = conn.prepareStatement(query);
        		ResultSet rs = stmt.executeQuery();
                

                while(rs.next()){
                    ObservableList<String> row = FXCollections.observableArrayList();
                    for(int i=1 ; i<=rs.getMetaData().getColumnCount(); i++){
                        row.add(rs.getString(i));
                    }
                    pasCars.add(row);

                }

                tabulka.setItems(pasCars);                
        		
        		rs.close();
        		stmt.close();
                conn.commit();
        		conn.close();
        		
        	}catch(Exception e1){
        		e1.printStackTrace(); 
        		System.exit(1);
        	}
		});
		
		predchStrana.setOnAction(e-> {
			count--;
			if(count == 1)
				predchStrana.setDisable(true);
			
			tabulka.getItems().clear();
			String query = "SELECT * FROM (SELECT * FROM (SELECT * FROM "
					+ "passenger_cars WHERE class = "	+ vyberTriedy.getValue().charAt(0);
			if(wifi.isSelected())
				query = query + " and wifi";
			if(bikeTransport.isSelected())
				query = query + " and bicycle_transport";
			if(restCar.isSelected())
				query = query + " and restaurant_car";
			if(airCondition.isSelected())
				query = query + " and air_conditioning";
			
			query = query + " ORDER BY 1 ASC LIMIT " + Integer.toString(count*offset) + ") AS q1 "
					+ "ORDER BY 1 DESC LIMIT 20) AS q2 ORDER BY 1";
			
			pasCars = FXCollections.observableArrayList();
			try{
				Connection conn = getConnection();
        		PreparedStatement stmt = conn.prepareStatement(query);
        		ResultSet rs = stmt.executeQuery();
                

                while(rs.next()){
                    ObservableList<String> row = FXCollections.observableArrayList();
                    for(int i=1 ; i<=rs.getMetaData().getColumnCount(); i++){
                        row.add(rs.getString(i));
                    }
                    pasCars.add(row);

                }

                tabulka.setItems(pasCars);                
        		
        		rs.close();
        		stmt.close();
                conn.commit();
        		conn.close();
        		
        	}catch(Exception e1){
        		e1.printStackTrace(); 
        		System.exit(1);
        	}
		});
		
		getBack.setOnAction(e-> {
			hlavneOkno.setScene(mainScene);
		});
		
		getBack2.setOnAction(e-> {
			hlavneOkno.setScene(mainScene);
		});
		
		scenar1.setOnAction(e-> {
			hlavneOkno.setScene(firstScene);
		});
		
		scenar2.setOnAction(e-> {
			hlavneOkno.setScene(secondScene);
			if(count == 1)
				predchStrana.setDisable(true);
			tabulka.getColumns().clear();
			
			ObservableList<String> nazvyStlpcov = FXCollections.observableArrayList("ID vagona",
            		"Trieda","Pocet miest na sedenie","ID lokomotivy","Wi-Fi","Restauracny vozen",
            		"Preprava bicyklov","Klimatizacia");
            
    		for(int i=0 ; i<8; i++){
                final int j = i;                
                TableColumn col = new TableColumn(nazvyStlpcov.get(i));
                col.setMinWidth(100);
                col.setCellValueFactory(new Callback<CellDataFeatures<ObservableList,String>,ObservableValue<String>>(){                    
                    public ObservableValue<String> call(CellDataFeatures<ObservableList, String> param) {                                                                                              
                        return new SimpleStringProperty(param.getValue().get(j).toString());                        
                    }                    
                });

                tabulka.getColumns().addAll(col); 
            }
		});
		
		hlavneOkno.setScene(mainScene);
		menuButtons.getChildren().addAll(pane1,pane2,pane3,pane4,pane5);
		hlavneOkno.show();
		
	}
	
	public static void main(String[] args) {
		launch(args);
	}
	
}
