#include "DBUtil.h"

/**
@mainpage
@htmlonly
<embed src="graphdb.pdf" width="100%" height="400" alt="pdf" pluginspage="http://www.adobe.com/products/acrobat/readstep2.html">
@endhtmlonly
*/

/** 
* Check for error-Funktion. Sollte nach jeder SQL-Anfrage aufgerufen werden.
* @param my MYSQL-Handler
*/
void check_error(MYSQL *my)  {
   if (mysql_errno(my) != 0) {
      fprintf(stderr, "Fehler: %s\n", mysql_error(my));
      exit(EXIT_FAILURE);
   }
}

/** 
* Datenbank erzeugen.
* @param my MYSQL-Handler
* @param db_name Name der anzulegenden Datenbank
*/
void create_db(MYSQL *my, char *db_name) {
	char *query = "CREATE DATABASE IF NOT EXISTS ";
	char *myQuery;

	myQuery = (char*)malloc(strlen(query)+strlen(db_name)+1);
	strcpy(myQuery, query);
	strcat(myQuery, db_name);

	mysql_real_query(my, myQuery, strlen(myQuery)); 
	check_error(my);

	free(myQuery);
}

/** 
* Datenbank selektieren.
* @param my MYSQL-Handler
* @param db Name der Datenbank
*/
void select_db(MYSQL *my, char *db) {
   mysql_select_db(my, db);
   check_error(my);
}

/** 
* Mit Datenbank verbinden.
* @param my MYSQL-Handler
*/
void connect(MYSQL *my) {
		if(mysql_real_connect(
		my,				/* Zeiger auf MYSQL-Handler */
        "localhost",	/* Host-Name */
        "root",			/* User-Name */
        "algomath",		/* Passwort für user_name */
        "graphdb",		/* Name der Datenbank */
        3306,			/* Port (default=0) */
        NULL,			/* Socket (default=NULL) */
        0				/* keine Flags */) == NULL) {
		check_error(my);
	} else {
		printf("Erfolgreich verbunden\n");
	}
}

/**
* MYSQL-Handler erzeugen.
* @return MYSQL-Handler
*/
MYSQL* init_mysql() {
	MYSQL* mysql = mysql_init(NULL);
	if(mysql == NULL) {
		fprintf(stderr, " Initialisierung fehlgeschlagen\n");
		exit(EXIT_FAILURE);
	}

	return mysql;
}

/**
* SQL-Statement ausfuehren.
* @param my MYSQL-Handler
* @param statement SQL-Ausdruck
* @return status Status (OK == 0; Fehler == 1)
*/
int execute_query(MYSQL *my, char *statement) {
	int status = mysql_real_query(my, statement, strlen(statement)); 
	check_error(my);

	return status;
}

/**
* NODE-Tabelle in SQL-Datenbank erzeugen.
* @param my MYSQL-Handler
*/
void create_node_table(MYSQL *my) {
	printf("CREATE NODE TABLE: %s\n", (execute_query(my, "CREATE TABLE IF NOT EXISTS NODE (ID INT NOT NULL AUTO_INCREMENT PRIMARY KEY, TYPE VARCHAR(255))") == 0) ? "OK" : "ERROR");
}

/**
* NODE_PROPERTIES-Tabelle in SQL-Datenbank erzeugen.
* @param my MYSQL-Handler
*/
void create_node_properties_table(MYSQL *my) {
	printf("CREATE NODE PROPERTIES TABLE: %s\n", (execute_query(my, "CREATE TABLE IF NOT EXISTS NODE_PROPERTIES (ID INT NOT NULL, THE_KEY VARCHAR(255) NOT NULL, VALUE VARCHAR(255), DATATYPE VARCHAR(32) NOT NULL, PRIMARY KEY (ID, THE_KEY), KEY (THE_KEY))") == 0) ? "OK" : "ERROR");
}

/**
* EDGE-Tabelle in SQL-Datenbank erzeugen.
* @param my MYSQL-Handler
*/
void create_edge_table(MYSQL *my) {
	printf("CREATE EDGE TABLE: %s\n", (execute_query(my, "CREATE TABLE IF NOT EXISTS EDGE (ID INT NOT NULL AUTO_INCREMENT PRIMARY KEY, SOURCE INT NOT NULL, TARGET INT NOT NULL, TYPE VARCHAR(255))") == 0) ? "OK" : "ERROR");
}

/**
* EDGE_PROPERTIES-Tabelle in SQL-Datenbank erzeugen.
* @param my MYSQL-Handler
*/
void create_edge_properties_table(MYSQL *my) {
	printf("CREATE EDGE PROPERTIES TABLE: %s\n", (execute_query(my, "CREATE TABLE IF NOT EXISTS EDGE_PROPERTIES (ID INT NOT NULL, THE_KEY VARCHAR(255) NOT NULL, VALUE VARCHAR(255), DATATYPE VARCHAR(32) NOT NULL, PRIMARY KEY (ID, THE_KEY), KEY (THE_KEY))") == 0) ? "OK" : "ERROR");
}
	
/**
* Liefert alle Knoten in der Datenbank zurueck.
* @param my MYSQL-Handler
* @return Vector mit allen \see Node Objekten
*/
std::vector<Node> get_all_nodes(MYSQL *mysql) {
	std::vector<Node> v;

	mysql_query(mysql, "SELECT * FROM NODE");
	check_error(mysql);
	MYSQL_RES *result = mysql_store_result(mysql);

	MYSQL_ROW row;
	while ((row = mysql_fetch_row(result))) { 
		Node n(row[1]);
		n.setId(atoi(row[0]));
		n.setDbHandler(mysql);
		v.push_back(n);
	}
  
	mysql_free_result(result);

	return v;
}

/**
* Liefert alle Knoten in der Datenbank zurueck.
* @param my MYSQL-Handler
* @return Vector mit allen \see Edge Objekten
*/
std::vector<Edge> get_all_edges(MYSQL *mysql) {
	std::vector<Edge> v;

	mysql_query(mysql, "SELECT * FROM EDGE");
	check_error(mysql);
	MYSQL_RES *result = mysql_store_result(mysql);

	MYSQL_ROW row;
	while ((row = mysql_fetch_row(result))) { 
		Edge e(row[3]);
		e.setId(atoi(row[0]));
		e.setSourceId(atoi(row[1]));
		e.setTargetId(atoi(row[2]));
		e.setDbHandler(mysql);
		v.push_back(e);
	}
  
	mysql_free_result(result);

	return v;
}

/**
* Erzeugt einen neuen Knoten.
* @param my MYSQL-Handler
* @param type Knotentyp
* @return ID des erzeugten Knotens
*/
int create_node(MYSQL* my, char* type) {
	MYSQL_BIND bind[1];
	MYSQL_STMT *stmt = mysql_stmt_init(my);
	char* statement = "INSERT INTO NODE (TYPE) VALUES (?)";
	mysql_stmt_prepare(stmt, statement, strlen(statement));

	memset(bind,0,sizeof(bind));

	unsigned long string_len = strlen(type);
	bind[0].buffer_type = MYSQL_TYPE_STRING;
	bind[0].buffer = type;
	bind[0].is_null = 0;
	bind[0].length = &string_len;

	mysql_stmt_bind_param(stmt, bind);
	mysql_stmt_execute(stmt);

	int id = mysql_insert_id(my);

	mysql_stmt_free_result(stmt);
	mysql_stmt_close(stmt);

	return id;
}

/**
* Erzeugt eine neue Kante.
* @param my MYSQL-Handler
* @param fromNode Startknoten
* @param toNode Zielknoten
* @param type Kantentyp
* @return ID der erzeugten Kante
*/
int create_edge(MYSQL* my, Node const& fromNode, Node const& toNode, char* type) {
	MYSQL_BIND bind[3];
	MYSQL_STMT *stmt = mysql_stmt_init(my);
	char* statement = "INSERT INTO EDGE (SOURCE, TARGET, TYPE) VALUES (?,?,?)";
	mysql_stmt_prepare(stmt, statement, strlen(statement));

	memset(bind,0,sizeof(bind));

	char* myType = type;
	int sourceId = fromNode.getId();
	int targetId = toNode.getId();

	unsigned long string_len = strlen(myType);

	bind[0].buffer_type = MYSQL_TYPE_LONG;
	bind[0].buffer = (void *) &sourceId;
	bind[0].is_null = 0;

	bind[1].buffer_type = MYSQL_TYPE_LONG;
	bind[1].buffer = (void *) &targetId;
	bind[1].is_null = 0;

	bind[2].buffer_type = MYSQL_TYPE_STRING;
	bind[2].buffer = myType;
	bind[2].is_null = 0;
	bind[2].length = &string_len;

	mysql_stmt_bind_param(stmt, bind);
	mysql_stmt_execute(stmt);

	int id = mysql_insert_id(my);

	mysql_stmt_free_result(stmt);
	mysql_stmt_close(stmt);

	return id;
}

/**
* Liefert den Knoten mit der angegebenen ID zurueck.
* @param my MYSQL-Handler
* @param id ID des gewuenschten Knotens
* @return Gewuenschtes Knoten-Objekt
*/
Node get_node_by_id(MYSQL* my, int id) {
	MYSQL_BIND bind[1], result[1];
	char type[256];
	MYSQL_STMT *stmt = mysql_stmt_init(my);
	char* statement = "SELECT `TYPE` FROM NODE WHERE ID = ?";
	mysql_stmt_prepare(stmt, statement, strlen(statement));

	memset(bind,0,sizeof(bind));
	memset(result, 0, sizeof(result));

	int nodeId = id;	

	bind[0].buffer_type = MYSQL_TYPE_LONG;
	bind[0].buffer = (void *) &nodeId;
	bind[0].is_null = 0;

	result[0].buffer_type = MYSQL_TYPE_STRING;
	result[0].buffer = (void *) type;
	result[0].buffer_length = 256;

	mysql_stmt_bind_param(stmt, bind);
	mysql_stmt_bind_result(stmt, result);
	mysql_stmt_execute(stmt);
	
	if(mysql_stmt_fetch(stmt)) {
		fprintf(stderr, "NODE #%d DOES NOT EXIST!\n", id);
		exit(EXIT_FAILURE);
	}
	
	Node n(type);
	n.setId(id);
	n.setDbHandler(my);

	mysql_stmt_free_result(stmt);
	mysql_stmt_close(stmt);

	return n;
}

/**
* Liefert die Kante (Edge) mit der angegebenen ID zurueck.
* @param my MYSQL-Handler
* @param id ID der gewuenschten Kante
* @return Gewuenschtes Kanten-Objekt (Node)
*/
Edge get_edge_by_id(MYSQL* my, int id) {
	MYSQL_BIND bind[1], result[3];
	MYSQL_STMT *stmt = mysql_stmt_init(my);
	char* statement = "SELECT TYPE, SOURCE, TARGET FROM EDGE WHERE ID = ?";
	mysql_stmt_prepare(stmt, statement, strlen(statement));

	memset(bind,0,sizeof(bind));
	memset(result, 0, sizeof(result));

	int edgeId = id;
	int sourceId;
	int targetId;
	char type[256];

	bind[0].buffer_type = MYSQL_TYPE_LONG;
	bind[0].buffer = (void *) &edgeId;
	bind[0].is_null = 0;

	result[0].buffer_type = MYSQL_TYPE_STRING;
	result[0].buffer = (void *) type;
	result[0].buffer_length = 256;

	result[1].buffer_type = MYSQL_TYPE_LONG;
	result[1].buffer = (void *) &sourceId;
	result[1].length = 0;

	result[2].buffer_type = MYSQL_TYPE_LONG;
	result[2].buffer = (void *) &targetId;
	result[2].length = 0;

	mysql_stmt_bind_param(stmt, bind);
	mysql_stmt_bind_result(stmt, result);
	mysql_stmt_execute(stmt);

	if(mysql_stmt_fetch(stmt)) {
		fprintf(stderr, "EDGE #%d DOES NOT EXIST!\n", id);
		exit(EXIT_FAILURE);
	}

	Edge e(type);
	e.setId(id);
	e.setSourceId(sourceId);
	e.setTargetId(targetId);
	e.setDbHandler(my);

	mysql_stmt_free_result(stmt);
	mysql_stmt_close(stmt);

	return e;
}

/**
* Liefert den Start-/Eingangsknoten der angegebenen Kante.
* @param my MYSQL-Handler
* @param e Kantenobjekt (Edge)
* @return Gewuenschtes Knoten-Objekt (Node)
*/
Node get_edge_source(MYSQL* my, Edge const& e) {
	MYSQL_BIND bind[1], result[1];
	MYSQL_STMT *stmt = mysql_stmt_init(my);
	char* statement = "SELECT SOURCE FROM EDGE WHERE ID = ?";
	mysql_stmt_prepare(stmt, statement, strlen(statement));

	memset(bind,0,sizeof(bind));
	memset(result, 0, sizeof(result));

	int edgeId = e.getId();
	int sourceId;

	bind[0].buffer_type = MYSQL_TYPE_LONG;
	bind[0].buffer = (void *) &edgeId;
	bind[0].is_null = 0;

	result[0].buffer_type = MYSQL_TYPE_LONG;
	result[0].buffer = (void *) &sourceId;
	result[0].length = 0;

	mysql_stmt_bind_param(stmt, bind);
	mysql_stmt_bind_result(stmt, result);
	mysql_stmt_execute(stmt);

	mysql_stmt_fetch(stmt);

	mysql_stmt_free_result(stmt);
	mysql_stmt_close(stmt);

	return get_node_by_id(my, sourceId);
}

/**
* Liefert den End-/Ausgangsknoten der angegebenen Kante.
* @param my MYSQL-Handler
* @param e Kantenobjekt (Edge)
* @return Gewuenschtes Knoten-Objekt (Node)
*/
Node get_edge_target(MYSQL* my, Edge const& e) {
	MYSQL_BIND bind[1], result[1];
	MYSQL_STMT *stmt = mysql_stmt_init(my);
	char* statement = "SELECT TARGET FROM EDGE WHERE ID = ?";
	mysql_stmt_prepare(stmt, statement, strlen(statement));

	memset(bind,0,sizeof(bind));
	memset(result, 0, sizeof(result));

	int edgeId = e.getId();
	int targetId;

	bind[0].buffer_type = MYSQL_TYPE_LONG;
	bind[0].buffer = (void *) &edgeId;
	bind[0].is_null = 0;

	result[0].buffer_type = MYSQL_TYPE_LONG;
	result[0].buffer = (void *) &targetId;
	result[0].length = 0;

	mysql_stmt_bind_param(stmt, bind);
	mysql_stmt_bind_result(stmt, result);
	mysql_stmt_execute(stmt);

	mysql_stmt_fetch(stmt);

	mysql_stmt_free_result(stmt);
	mysql_stmt_close(stmt);

	return get_node_by_id(my, targetId);
}

/**
* Setzt den Start-/Eingangsknoten der angegebenen Kante.
* @param my MYSQL-Handler
* @param e Kantenobjekt (Edge)
* @param n Knotenobjekt (Node)
*/
void set_edge_source(MYSQL* my, Edge const& e, Node const& n) {
	MYSQL_BIND bind[2];
	MYSQL_STMT *stmt = mysql_stmt_init(my);
	char* statement = "UPDATE EDGE SET SOURCE = ? WHERE ID = ?";
	mysql_stmt_prepare(stmt, statement, strlen(statement));

	memset(bind,0,sizeof(bind));

	int edgeId = e.getId();
	int sourceId = n.getId();

	bind[0].buffer_type = MYSQL_TYPE_LONG;
	bind[0].buffer = (void *) &sourceId;
	bind[0].is_null = 0;

	bind[1].buffer_type = MYSQL_TYPE_LONG;
	bind[1].buffer = (void *) &edgeId;
	bind[1].is_null = 0;

	mysql_stmt_bind_param(stmt, bind);
	mysql_stmt_execute(stmt);
	mysql_stmt_free_result(stmt);
	mysql_stmt_close(stmt);
}

/**
* Setzt den End-/Ausgangsknoten der angegebenen Kante.
* @param my MYSQL-Handler
* @param e Kantenobjekt (Edge)
* @param n Knotenobjekt (Node)
*/
void set_edge_target(MYSQL* my, Edge const& e, Node const& n) {
	MYSQL_BIND bind[2];
	MYSQL_STMT *stmt = mysql_stmt_init(my);
	char* statement = "UPDATE EDGE SET TARGET = ? WHERE ID = ?";
	mysql_stmt_prepare(stmt, statement, strlen(statement));

	memset(bind,0,sizeof(bind));

	int edgeId = e.getId();
	int targetId = n.getId();

	bind[0].buffer_type = MYSQL_TYPE_LONG;
	bind[0].buffer = (void *) &targetId;
	bind[0].is_null = 0;

	bind[1].buffer_type = MYSQL_TYPE_LONG;
	bind[1].buffer = (void *) &edgeId;
	bind[1].is_null = 0;

	mysql_stmt_bind_param(stmt, bind);
	mysql_stmt_execute(stmt);
	mysql_stmt_free_result(stmt);
	mysql_stmt_close(stmt);
}

/**
* Liefert alle eingehenden Kanten des angegebenen Knotens.
* @param my MYSQL-Handler
* @param n Knotenobjekt (Node)
* @return Vector mit eingehenden Kanten (Edge)
*/
std::vector<Edge> get_incoming_edges(MYSQL* my, Node const& n) {
	MYSQL_BIND bind[1], result[3];
	MYSQL_STMT *stmt = mysql_stmt_init(my);
	char* statement = "SELECT ID, SOURCE, TYPE FROM EDGE WHERE TARGET = ?";
	mysql_stmt_prepare(stmt, statement, strlen(statement));

	memset(bind,0,sizeof(bind));
	memset(result, 0, sizeof(result));

	std::vector<Edge> v;
	int nodeId = n.getId();
	int edgeId;
	int sourceId;
	char type[256];

	bind[0].buffer_type = MYSQL_TYPE_LONG;
	bind[0].buffer = (void *) &nodeId;
	bind[0].is_null = 0;

	result[0].buffer_type = MYSQL_TYPE_LONG;
	result[0].buffer = (void *) &edgeId;
	result[0].length = 0;

	result[1].buffer_type = MYSQL_TYPE_LONG;
	result[1].buffer = (void *) &sourceId;
	result[1].length = 0;

	result[2].buffer_type = MYSQL_TYPE_STRING;
	result[2].buffer = (void *) type;
	result[2].buffer_length = 256;

	mysql_stmt_bind_param(stmt, bind);
	mysql_stmt_bind_result(stmt, result);
	mysql_stmt_execute(stmt);

	while(mysql_stmt_fetch(stmt) == 0) {
		Edge e(type);
		e.setId(edgeId);
		e.setSourceId(sourceId);
		e.setTargetId(nodeId);
		e.setDbHandler(my);
		v.push_back(e);
	}

	mysql_stmt_free_result(stmt);
	mysql_stmt_close(stmt);

	return v;
}

/**
* Liefert alle ausgehenden Kanten des angegebenen Knotens.
* @param my MYSQL-Handler
* @param n Knotenobjekt (Node)
* @return Vector mit ausgehenden Kanten (Edge)
*/
std::vector<Edge> get_outgoing_edges(MYSQL* my, Node const& n) {
	MYSQL_BIND bind[1], result[3];
	MYSQL_STMT *stmt = mysql_stmt_init(my);
	char* statement = "SELECT ID, TARGET, TYPE FROM EDGE WHERE SOURCE = ?";
	mysql_stmt_prepare(stmt, statement, strlen(statement));

	memset(bind,0,sizeof(bind));
	memset(result, 0, sizeof(result));

	std::vector<Edge> v;
	int nodeId = n.getId();
	int edgeId;
	int targetId;
	char type[256];

	bind[0].buffer_type = MYSQL_TYPE_LONG;
	bind[0].buffer = (void *) &nodeId;
	bind[0].is_null = 0;

	result[0].buffer_type = MYSQL_TYPE_LONG;
	result[0].buffer = (void *) &edgeId;
	result[0].length = 0;

	result[1].buffer_type = MYSQL_TYPE_LONG;
	result[1].buffer = (void *) &targetId;
	result[1].length = 0;

	result[2].buffer_type = MYSQL_TYPE_STRING;
	result[2].buffer = (void *) type;
	result[2].buffer_length = 256;

	mysql_stmt_bind_param(stmt, bind);
	mysql_stmt_bind_result(stmt, result);
	mysql_stmt_execute(stmt);

	while(mysql_stmt_fetch(stmt) == 0) {
		Edge e(type);
		e.setId(edgeId);
		e.setSourceId(nodeId);
		e.setTargetId(targetId);
		e.setDbHandler(my);
		v.push_back(e);
	}

	mysql_stmt_free_result(stmt);
	mysql_stmt_close(stmt);

	return v;
}

/**
* Liefert ALLE angebundenen (eingehende wie ausgehende) Kanten des angegebenen Knotens.
* @param my MYSQL-Handler
* @param n Knotenobjekt (Node)
* @return Vector mit Kanten (Edge)
*/
std::vector<Edge> get_connected_edges(MYSQL* my, Node const& n) {
	std::vector<Edge> v1 = get_incoming_edges(my, n);
	std::vector<Edge> v2 = get_outgoing_edges(my, n);
	
	v1.insert(v1.end(), v2.begin(), v2.end());

	return v1;
}

/**
* Liefert eine Kanten-Eigenschaft unter Angabe des Schluessels/Eigenschaftentyps.
* @param my MYSQL-Handler
* @param e Kantenobjekt (Edge)
* @param key Schluessel
* @return Kanten-Eigenschaft
*/
std::string get_edge_property(MYSQL* my, Edge const& e, char* key) {
	MYSQL_BIND bind[2], result[1];
	char value[256];
	MYSQL_STMT *stmt = mysql_stmt_init(my);
	char* statement = "SELECT VALUE FROM EDGE_PROPERTIES WHERE ID = ? AND THE_KEY = ?";
	mysql_stmt_prepare(stmt, statement, strlen(statement));

	memset(bind,0,sizeof(bind));
	memset(result,0,sizeof(result));
	memset(value,0,sizeof(value));

	int edgeId = e.getId();
	bind[0].buffer_type = MYSQL_TYPE_LONG;
	bind[0].buffer = (void *) &edgeId;
	bind[0].is_null = 0;

	unsigned long string_len = strlen(key);
	bind[1].buffer_type = MYSQL_TYPE_STRING;
	bind[1].buffer = key;
	bind[1].is_null = 0;
	bind[1].length = &string_len;

	result[0].buffer_type = MYSQL_TYPE_STRING;
	result[0].buffer = (void *) value;
	result[0].buffer_length = 256;

	mysql_stmt_bind_param(stmt, bind);
	mysql_stmt_bind_result(stmt, result);
	mysql_stmt_execute(stmt);
	mysql_stmt_fetch(stmt);
	mysql_stmt_free_result(stmt);
	mysql_stmt_close(stmt);

	return std::string(value);
}

/**
* Liefert eine Knoten-Eigenschaft unter Angabe des Schluessels/Eigenschaftentyps.
* @param my MYSQL-Handler
* @param n Knotenobjekt (Node)
* @param key Schluessel
* @return Knoten-Eigenschaft
*/
std::string get_node_property(MYSQL* my, Node const& n, char* key) {
	MYSQL_BIND bind[2], result[1];
	char value[256];
	MYSQL_STMT *stmt = mysql_stmt_init(my);
	char* statement = "SELECT VALUE FROM NODE_PROPERTIES WHERE ID = ? AND THE_KEY = ?";
	mysql_stmt_prepare(stmt, statement, strlen(statement));

	memset(bind,0,sizeof(bind));
	memset(result,0,sizeof(result));
	memset(value,0,sizeof(value));

	int nodeId = n.getId();
	bind[0].buffer_type = MYSQL_TYPE_LONG;
	bind[0].buffer = (void *) &nodeId;
	bind[0].is_null = 0;

	unsigned long string_len = strlen(key);
	bind[1].buffer_type = MYSQL_TYPE_STRING;
	bind[1].buffer = key;
	bind[1].is_null = 0;
	bind[1].length = &string_len;

	result[0].buffer_type = MYSQL_TYPE_STRING;
	result[0].buffer = (void *) value;
	result[0].buffer_length = 256;

	mysql_stmt_bind_param(stmt, bind);
	mysql_stmt_bind_result(stmt, result);
	mysql_stmt_execute(stmt);
	mysql_stmt_fetch(stmt);
	mysql_stmt_free_result(stmt);
	mysql_stmt_close(stmt);

	return std::string(value);
}

/**
* Setzt eine Kanten-Eigenschaft unter Angabe des Schluessels/Eigenschaftentyps sowie des zugehoerigen Werts.
* @param my MYSQL-Handler
* @param e Kantenobjekt (Edge)
* @param key Schluessel
* @param key Wert
*/
void set_edge_property(MYSQL* my, Edge const& e, char* key, char* value) {
	MYSQL_BIND bind[5];
	MYSQL_STMT *stmt = mysql_stmt_init(my);
	char* statement = "INSERT INTO EDGE_PROPERTIES (`ID`, `THE_KEY`, `VALUE`, `DATATYPE`) VALUES (?, ?, ?, ?) ON DUPLICATE KEY UPDATE `VALUE` = ?";
	mysql_stmt_prepare(stmt, statement, strlen(statement));

	memset(bind,0,sizeof(bind));

	int edgeId = e.getId();
	bind[0].buffer_type = MYSQL_TYPE_LONG;
	bind[0].buffer = (void *) &edgeId;
	bind[0].is_null = 0;

	unsigned long key_len = strlen(key);
	bind[1].buffer_type = MYSQL_TYPE_STRING;
	bind[1].buffer = key;
	bind[1].is_null = 0;
	bind[1].length = &key_len;

	unsigned long val_len = strlen(value);
	bind[2].buffer_type = MYSQL_TYPE_STRING;
	bind[2].buffer = value;
	bind[2].is_null = 0;
	bind[2].length = &val_len;

	char* datatype = "STRING";
	unsigned long datatype_len = strlen(datatype);
	bind[3].buffer_type = MYSQL_TYPE_STRING;
	bind[3].buffer = datatype;
	bind[3].is_null = 0;
	bind[3].length = &datatype_len;

	bind[4] = bind[2];

	mysql_stmt_bind_param(stmt, bind);
	mysql_stmt_execute(stmt);
	mysql_stmt_free_result(stmt);
	mysql_stmt_close(stmt);
}

/**
* Setzt eine Knoten-Eigenschaft unter Angabe des Schluessels/Eigenschaftentyps sowie des zugehoerigen Werts.
* @param my MYSQL-Handler
* @param n Knotenobjekt (Node)
* @param key Schluessel
* @param key Wert
*/
void set_node_property(MYSQL* my, Node const& n, char* key, char* value) {
	MYSQL_BIND bind[5];
	MYSQL_STMT *stmt = mysql_stmt_init(my);
	char* statement = "INSERT INTO NODE_PROPERTIES (`ID`, `THE_KEY`, `VALUE`, `DATATYPE`) VALUES (?, ?, ?, ?) ON DUPLICATE KEY UPDATE `VALUE` = ?";
	mysql_stmt_prepare(stmt, statement, strlen(statement));

	memset(bind,0,sizeof(bind));

	int nodeId = n.getId();
	bind[0].buffer_type = MYSQL_TYPE_LONG;
	bind[0].buffer = (void *) &nodeId;
	bind[0].is_null = 0;

	unsigned long key_len = strlen(key);
	bind[1].buffer_type = MYSQL_TYPE_STRING;
	bind[1].buffer = key;
	bind[1].is_null = 0;
	bind[1].length = &key_len;

	unsigned long val_len = strlen(value);
	bind[2].buffer_type = MYSQL_TYPE_STRING;
	bind[2].buffer = value;
	bind[2].is_null = 0;
	bind[2].length = &val_len;

	char* datatype = "STRING";
	unsigned long datatype_len = strlen(datatype);
	bind[3].buffer_type = MYSQL_TYPE_STRING;
	bind[3].buffer = datatype;
	bind[3].is_null = 0;
	bind[3].length = &datatype_len;

	bind[4] = bind[2];

	mysql_stmt_bind_param(stmt, bind);
	mysql_stmt_execute(stmt);
	mysql_stmt_free_result(stmt);
	mysql_stmt_close(stmt);
}

/**
* Loescht eine Kanten-Eigenschaft unter Angabe des Schluessels/Eigenschaftentyps.
* @param my MYSQL-Handler
* @param e Kantenobjekt (Edge)
* @param key Schluessel
*/
void delete_edge_property(MYSQL* my, Edge const& e, char* key) {
	MYSQL_BIND bind[2];
	MYSQL_STMT *stmt = mysql_stmt_init(my);
	char* statement = "DELETE FROM EDGE_PROPERTIES WHERE ID = ? AND THE_KEY = ?";
	mysql_stmt_prepare(stmt, statement, strlen(statement));

	memset(bind,0,sizeof(bind));

	int edgeId = e.getId();
	bind[0].buffer_type = MYSQL_TYPE_LONG;
	bind[0].buffer = (void *) &edgeId;
	bind[0].is_null = 0;

	unsigned long key_len = strlen(key);
	bind[1].buffer_type = MYSQL_TYPE_STRING;
	bind[1].buffer = key;
	bind[1].is_null = 0;
	bind[1].length = &key_len;

	mysql_stmt_bind_param(stmt, bind);
	mysql_stmt_execute(stmt);
	mysql_stmt_free_result(stmt);
	mysql_stmt_close(stmt);
}

/**
* Loescht eine Knoten-Eigenschaft unter Angabe des Schluessels/Eigenschaftentyps.
* @param my MYSQL-Handler
* @param n Knotenobjekt (Node)
* @param key Schluessel
*/
void delete_node_property(MYSQL* my, Node const& n, char* key) {
	MYSQL_BIND bind[2];
	MYSQL_STMT *stmt = mysql_stmt_init(my);
	char* statement = "DELETE FROM NODE_PROPERTIES WHERE ID = ? AND THE_KEY = ?";
	mysql_stmt_prepare(stmt, statement, strlen(statement));

	memset(bind,0,sizeof(bind));

	int nodeId = n.getId();
	bind[0].buffer_type = MYSQL_TYPE_LONG;
	bind[0].buffer = (void *) &nodeId;
	bind[0].is_null = 0;

	unsigned long key_len = strlen(key);
	bind[1].buffer_type = MYSQL_TYPE_STRING;
	bind[1].buffer = key;
	bind[1].is_null = 0;
	bind[1].length = &key_len;

	mysql_stmt_bind_param(stmt, bind);
	mysql_stmt_execute(stmt);
	mysql_stmt_free_result(stmt);
	mysql_stmt_close(stmt);
}

/**
* Loescht alle Kanten-Eigenschaften.
* @param my MYSQL-Handler
* @param e Kantenobjekt (Edge)
*/
void delete_edge_properties(MYSQL* my, Edge const& e) {
	MYSQL_BIND bind[1];
	MYSQL_STMT *stmt = mysql_stmt_init(my);
	char* statement = "DELETE FROM EDGE_PROPERTIES WHERE ID = ?";
	mysql_stmt_prepare(stmt, statement, strlen(statement));

	memset(bind,0,sizeof(bind));

	int edgeId = e.getId();
	bind[0].buffer_type = MYSQL_TYPE_LONG;
	bind[0].buffer = (void *) &edgeId;
	bind[0].is_null = 0;

	mysql_stmt_bind_param(stmt, bind);
	mysql_stmt_execute(stmt);
	mysql_stmt_free_result(stmt);
	mysql_stmt_close(stmt);
}

/**
* Loescht alle Knoten-Eigenschaften.
* @param my MYSQL-Handler
* @param n Knotenobjekt (Node)
*/
void delete_node_properties(MYSQL* my, Node const& n) {
	MYSQL_BIND bind[1];
	MYSQL_STMT *stmt = mysql_stmt_init(my);
	char* statement = "DELETE FROM NODE_PROPERTIES WHERE ID = ?";
	mysql_stmt_prepare(stmt, statement, strlen(statement));

	memset(bind,0,sizeof(bind));

	int nodeId = n.getId();
	bind[0].buffer_type = MYSQL_TYPE_LONG;
	bind[0].buffer = (void *) &nodeId;
	bind[0].is_null = 0;

	mysql_stmt_bind_param(stmt, bind);
	mysql_stmt_execute(stmt);
	mysql_stmt_free_result(stmt);
	mysql_stmt_close(stmt);
}

/**
* Loescht die gesamte Datenbank.
* @param my MYSQL-Handler
*/
void delete_database(MYSQL* my) {
	execute_query(my, "DROP TABLE NODE");
	execute_query(my, "DROP TABLE NODE_PROPERTIES");
	execute_query(my, "DROP TABLE EDGE");
	execute_query(my, "DROP TABLE EDGE_PROPERTIES");
	char *query = "DROP DATABASE ";
	char *whole_query = (char *) malloc(strlen(query)+strlen(my->db)+1);
	strcpy(whole_query, query);
	strcat(whole_query, my->db);
	execute_query(my, whole_query);
	free(whole_query);
}

/**
* Loescht alle Eintraege aus der Datenbank.
* @param my MYSQL-Handler
*/
void clear_database(MYSQL* my) {
	execute_query(my, "DELETE FROM NODE");
	execute_query(my, "DELETE FROM NODE_PROPERTIES");
	execute_query(my, "DELETE FROM EDGE");
	execute_query(my, "DELETE FROM EDGE_PROPERTIES");
}

/**
* Loescht einen Knoten aus der Datenbank.
* @param my MYSQL-Handler
* @param n Knotenobjekt (Node)
*/
void remove_node(MYSQL* my, Node const& n) {
	MYSQL_BIND bind[1];
	MYSQL_STMT *stmt = mysql_stmt_init(my);
	char* statement = "DELETE FROM NODE WHERE ID = ?";
	mysql_stmt_prepare(stmt, statement, strlen(statement));

	memset(bind,0,sizeof(bind));

	int nodeId = n.getId();
	bind[0].buffer_type = MYSQL_TYPE_LONG;
	bind[0].buffer = (void *) &nodeId;
	bind[0].is_null = 0;

	mysql_stmt_bind_param(stmt, bind);
	mysql_stmt_execute(stmt);
	mysql_stmt_free_result(stmt);
	mysql_stmt_close(stmt);

	std::vector<Edge> ve = n.getEdges(ALL);

	for(std::vector<Edge>::iterator it = ve.begin(); it != ve.end(); it++) {
		it->remove();
	}
}

/**
* Loescht eine Kante aus der Datenbank.
* @param my MYSQL-Handler
* @param e Kantenobjekt (Edge)
*/
void remove_edge(MYSQL* my, Edge const& e) {
	MYSQL_BIND bind[1];
	MYSQL_STMT *stmt = mysql_stmt_init(my);
	char* statement = "DELETE FROM EDGE WHERE ID = ?";
	mysql_stmt_prepare(stmt, statement, strlen(statement));

	memset(bind,0,sizeof(bind));

	int edgeId = e.getId();
	bind[0].buffer_type = MYSQL_TYPE_LONG;
	bind[0].buffer = (void *) &edgeId;
	bind[0].is_null = 0;

	mysql_stmt_bind_param(stmt, bind);
	mysql_stmt_execute(stmt);
	mysql_stmt_free_result(stmt);
	mysql_stmt_close(stmt);
}

/**
* Liefert alle Eigenschaften eines Knotens.
* @param my MYSQL-Handler
* @param n Knotenobjekt (Node)
* @return Map mit Schluessel/Wert-Paaren
*/
std::map<std::string, std::string> get_node_properties(MYSQL* my, Node const& n) {
	std::map<std::string, std::string> properties;
	MYSQL_BIND bind[1], result[2];
	char value[256], key[256];
	MYSQL_STMT *stmt = mysql_stmt_init(my);
	char* statement = "SELECT THE_KEY, VALUE FROM NODE_PROPERTIES WHERE ID = ?";
	mysql_stmt_prepare(stmt, statement, strlen(statement));

	memset(bind,0,sizeof(bind));
	memset(result,0,sizeof(result));
	memset(value,0,sizeof(value));
	memset(value,0,sizeof(key));

	int nodeId = n.getId();
	bind[0].buffer_type = MYSQL_TYPE_LONG;
	bind[0].buffer = (void *) &nodeId;
	bind[0].is_null = 0;

	result[0].buffer_type = MYSQL_TYPE_STRING;
	result[0].buffer = (void *) key;
	result[0].buffer_length = 256;

	result[1].buffer_type = MYSQL_TYPE_STRING;
	result[1].buffer = (void *) value;
	result[1].buffer_length = 256;

	mysql_stmt_bind_param(stmt, bind);
	mysql_stmt_bind_result(stmt, result);
	mysql_stmt_execute(stmt);

	while(mysql_stmt_fetch(stmt) == 0) {
		properties.insert(std::pair<std::string,std::string>(std::string(key), std::string(value)));
	}

	mysql_stmt_free_result(stmt);
	mysql_stmt_close(stmt);

	return properties;
}

/**
* Liefert alle Eigenschaften einer Kante.
* @param my MYSQL-Handler
* @param e Kantenobjekt (Edge)
* @return Map mit Schluessel/Wert-Paaren
*/
std::map<std::string, std::string> get_edge_properties(MYSQL* my, Edge const& e) {
	std::map<std::string, std::string> properties;
	MYSQL_BIND bind[1], result[2];
	char value[256], key[256];
	MYSQL_STMT *stmt = mysql_stmt_init(my);
	char* statement = "SELECT THE_KEY, VALUE FROM EDGE_PROPERTIES WHERE ID = ?";
	mysql_stmt_prepare(stmt, statement, strlen(statement));

	memset(bind,0,sizeof(bind));
	memset(result,0,sizeof(result));
	memset(value,0,sizeof(value));
	memset(value,0,sizeof(key));

	int edgeId = e.getId();
	bind[0].buffer_type = MYSQL_TYPE_LONG;
	bind[0].buffer = (void *) &edgeId;
	bind[0].is_null = 0;

	result[0].buffer_type = MYSQL_TYPE_STRING;
	result[0].buffer = (void *) key;
	result[0].buffer_length = 256;

	result[1].buffer_type = MYSQL_TYPE_STRING;
	result[1].buffer = (void *) value;
	result[1].buffer_length = 256;

	mysql_stmt_bind_param(stmt, bind);
	mysql_stmt_bind_result(stmt, result);
	mysql_stmt_execute(stmt);

	while(mysql_stmt_fetch(stmt) == 0) {
		properties.insert(std::pair<std::string,std::string>(std::string(key), std::string(value)));
	}

	mysql_stmt_free_result(stmt);
	mysql_stmt_close(stmt);

	return properties;
}

/**
* Liefert alle Knoten, die einem bestimmten Typ entsprechen.
* @param my MYSQL-Handler
* @param type Knotentyp
* @return Vector mit gesuchten Knoten
*/
std::vector<Node> get_typed_nodes(MYSQL* my, char* type) {
	std::vector<Node> nodes;
	MYSQL_BIND bind[1], result[1];
	MYSQL_STMT *stmt = mysql_stmt_init(my);
	char* statement = "SELECT ID FROM NODE WHERE TYPE = ?";
	mysql_stmt_prepare(stmt, statement, strlen(statement));

	memset(bind,0,sizeof(bind));
	memset(result,0,sizeof(result));

	int nodeId;

	unsigned long string_len = strlen(type);
	bind[0].buffer_type = MYSQL_TYPE_STRING;
	bind[0].buffer = type;
	bind[0].is_null = 0;
	bind[0].length = &string_len;

	result[0].buffer_type = MYSQL_TYPE_LONG;
	result[0].buffer = (void *) &nodeId;
	result[0].length = 0;

	mysql_stmt_bind_param(stmt, bind);
	mysql_stmt_bind_result(stmt, result);
	mysql_stmt_execute(stmt);

	while(mysql_stmt_fetch(stmt) == 0) {
		Node n(type);
		n.setId(nodeId);
		n.setDbHandler(my);
		nodes.push_back(n);
	}

	mysql_stmt_free_result(stmt);
	mysql_stmt_close(stmt);

	return nodes;
}

/**
* Liefert alle Kanten, die einem bestimmten Typ entsprechen.
* @param my MYSQL-Handler
* @param type Kantentyp
* @return Vector mit gesuchten Kanten
*/
std::vector<Edge> get_typed_edges(MYSQL* my, char* type) {
	std::vector<Edge> edges;
	MYSQL_BIND bind[1], result[3];
	MYSQL_STMT *stmt = mysql_stmt_init(my);
	char* statement = "SELECT ID, SOURCE, TARGET FROM EDGE WHERE TYPE = ?";
	mysql_stmt_prepare(stmt, statement, strlen(statement));

	memset(bind,0,sizeof(bind));
	memset(result,0,sizeof(result));

	int edgeId, sourceId, targetId;

	unsigned long string_len = strlen(type);
	bind[0].buffer_type = MYSQL_TYPE_STRING;
	bind[0].buffer = type;
	bind[0].is_null = 0;
	bind[0].length = &string_len;

	result[0].buffer_type = MYSQL_TYPE_LONG;
	result[0].buffer = (void *) &edgeId;
	result[0].length = 0;

	result[1].buffer_type = MYSQL_TYPE_LONG;
	result[1].buffer = (void *) &sourceId;
	result[1].length = 0;

	result[2].buffer_type = MYSQL_TYPE_LONG;
	result[2].buffer = (void *) &targetId;
	result[2].length = 0;

	mysql_stmt_bind_param(stmt, bind);
	mysql_stmt_bind_result(stmt, result);
	mysql_stmt_execute(stmt);

	while(mysql_stmt_fetch(stmt) == 0) {
		Edge e(type);
		e.setId(edgeId);
		e.setSourceId(sourceId);
		e.setTargetId(targetId);
		e.setDbHandler(my);
		edges.push_back(e);
	}

	mysql_stmt_free_result(stmt);
	mysql_stmt_close(stmt);

	return edges;
}

/**
* Liefert alle Knoten, die eine bestimmte Eigenschaft besitzen.
* @param my MYSQL-Handler
* @param key Eigenschaft
* @return Vector mit gesuchten Knoten
*/
std::vector<Node> get_property_nodes(MYSQL* my, char* key) {
	std::vector<Node> nodes;
	MYSQL_BIND bind[1], result[2];
	char type[256];
	MYSQL_STMT *stmt = mysql_stmt_init(my);
	char* statement = "SELECT ID, TYPE FROM NODE WHERE ID IN (SELECT ID FROM NODE_PROPERTIES WHERE THE_KEY = ?)";
	mysql_stmt_prepare(stmt, statement, strlen(statement));

	memset(bind,0,sizeof(bind));
	memset(result,0,sizeof(result));
	memset(type,0,sizeof(type));

	int nodeId;

	unsigned long string_len = strlen(key);
	bind[0].buffer_type = MYSQL_TYPE_STRING;
	bind[0].buffer = key;
	bind[0].is_null = 0;
	bind[0].length = &string_len;

	result[0].buffer_type = MYSQL_TYPE_LONG;
	result[0].buffer = (void *) &nodeId;
	result[0].length = 0;

	result[1].buffer_type = MYSQL_TYPE_STRING;
	result[1].buffer = (void *) type;
	result[1].buffer_length = 256;

	mysql_stmt_bind_param(stmt, bind);
	mysql_stmt_bind_result(stmt, result);
	mysql_stmt_execute(stmt);

	while(mysql_stmt_fetch(stmt) == 0) {
		Node n(type);
		n.setId(nodeId);
		n.setDbHandler(my);
		nodes.push_back(n);
	}

	mysql_stmt_free_result(stmt);
	mysql_stmt_close(stmt);

	return nodes;
}

/**
* Liefert alle Kanten, die eine bestimmte Eigenschaft besitzen.
* @param my MYSQL-Handler
* @param key Eigenschaft
* @return Vector mit gesuchten Kanten
*/
std::vector<Edge> get_property_edges(MYSQL* my, char* key) {
	std::vector<Edge> edges;
	MYSQL_BIND bind[1], result[4];
	char type[256];
	MYSQL_STMT *stmt = mysql_stmt_init(my);
	char* statement = "SELECT ID, SOURCE, TARGET, TYPE FROM EDGE WHERE ID IN (SELECT ID FROM EDGE_PROPERTIES WHERE THE_KEY = ?)";
	mysql_stmt_prepare(stmt, statement, strlen(statement));

	memset(bind,0,sizeof(bind));
	memset(result,0,sizeof(result));
	memset(type,0,sizeof(type));

	int edgeId, sourceId, targetId;

	unsigned long string_len = strlen(key);
	bind[0].buffer_type = MYSQL_TYPE_STRING;
	bind[0].buffer = key;
	bind[0].is_null = 0;
	bind[0].length = &string_len;

	result[0].buffer_type = MYSQL_TYPE_LONG;
	result[0].buffer = (void *) &edgeId;
	result[0].length = 0;

	result[1].buffer_type = MYSQL_TYPE_LONG;
	result[1].buffer = (void *) &sourceId;
	result[1].length = 0;

	result[2].buffer_type = MYSQL_TYPE_LONG;
	result[2].buffer = (void *) &targetId;
	result[2].length = 0;

	result[3].buffer_type = MYSQL_TYPE_STRING;
	result[3].buffer = (void *) type;
	result[3].buffer_length = 256;

	mysql_stmt_bind_param(stmt, bind);
	mysql_stmt_bind_result(stmt, result);
	mysql_stmt_execute(stmt);

	while(mysql_stmt_fetch(stmt) == 0) {
		Edge e(type);
		e.setId(edgeId);
		e.setSourceId(sourceId);
		e.setTargetId(targetId);
		e.setDbHandler(my);
		edges.push_back(e);
	}

	mysql_stmt_free_result(stmt);
	mysql_stmt_close(stmt);

	return edges;
}

/**
* Liefert alle Knoten, die eine bestimmte Eigenschaft sowie einen bestimmten, dazugehoerigen Wert besitzen.
* @param my MYSQL-Handler
* @param key Eigenschaft
* @param value Dazugehoeriger Wert
* @return Vector mit gesuchten Knoten
*/
std::vector<Node> get_property_nodes(MYSQL* my, char* key, char* value) {
	std::vector<Node> nodes;
	MYSQL_BIND bind[2], result[2];
	char type[256];
	MYSQL_STMT *stmt = mysql_stmt_init(my);
	char* statement = "SELECT ID, TYPE FROM NODE WHERE ID IN (SELECT ID FROM NODE_PROPERTIES WHERE THE_KEY = ? AND VALUE = ?);";
	mysql_stmt_prepare(stmt, statement, strlen(statement));

	memset(bind,0,sizeof(bind));
	memset(result,0,sizeof(result));
	memset(type,0,sizeof(type));

	int nodeId;

	unsigned long key_len = strlen(key);
	bind[0].buffer_type = MYSQL_TYPE_STRING;
	bind[0].buffer = key;
	bind[0].is_null = 0;
	bind[0].length = &key_len;

	unsigned long val_len = strlen(value);
	bind[1].buffer_type = MYSQL_TYPE_STRING;
	bind[1].buffer = value;
	bind[1].is_null = 0;
	bind[1].length = &val_len;

	result[0].buffer_type = MYSQL_TYPE_LONG;
	result[0].buffer = (void *) &nodeId;
	result[0].length = 0;

	result[1].buffer_type = MYSQL_TYPE_STRING;
	result[1].buffer = (void *) type;
	result[1].buffer_length = 256;

	mysql_stmt_bind_param(stmt, bind);
	mysql_stmt_bind_result(stmt, result);
	mysql_stmt_execute(stmt);

	while(mysql_stmt_fetch(stmt) == 0) {
		Node n(type);
		n.setId(nodeId);
		n.setDbHandler(my);
		nodes.push_back(n);
	}

	mysql_stmt_free_result(stmt);
	mysql_stmt_close(stmt);

	return nodes;
}

/**
* Liefert alle Kanten, die eine bestimmte Eigenschaft sowie einen bestimmten, dazugehoerigen Wert besitzen.
* @param my MYSQL-Handler
* @param key Eigenschaft
* @param value Dazugehoeriger Wert
* @return Vector mit gesuchten Kanten
*/
std::vector<Edge> get_property_edges(MYSQL* my, char* key, char* value) {
	std::vector<Edge> edges;
	MYSQL_BIND bind[2], result[4];
	char type[256];
	MYSQL_STMT *stmt = mysql_stmt_init(my);
	char* statement = "SELECT ID, SOURCE, TARGET, TYPE FROM EDGE WHERE ID IN (SELECT ID FROM EDGE_PROPERTIES WHERE THE_KEY = ? AND VALUE = ?)";
	mysql_stmt_prepare(stmt, statement, strlen(statement));

	memset(bind,0,sizeof(bind));
	memset(result,0,sizeof(result));
	memset(type,0,sizeof(type));

	int edgeId, sourceId, targetId;

	unsigned long key_len = strlen(key);
	bind[0].buffer_type = MYSQL_TYPE_STRING;
	bind[0].buffer = key;
	bind[0].is_null = 0;
	bind[0].length = &key_len;

	unsigned long val_len = strlen(value);
	bind[1].buffer_type = MYSQL_TYPE_STRING;
	bind[1].buffer = value;
	bind[1].is_null = 0;
	bind[1].length = &val_len;

	result[0].buffer_type = MYSQL_TYPE_LONG;
	result[0].buffer = (void *) &edgeId;
	result[0].length = 0;

	result[1].buffer_type = MYSQL_TYPE_LONG;
	result[1].buffer = (void *) &sourceId;
	result[1].length = 0;

	result[2].buffer_type = MYSQL_TYPE_LONG;
	result[2].buffer = (void *) &targetId;
	result[2].length = 0;

	result[3].buffer_type = MYSQL_TYPE_STRING;
	result[3].buffer = (void *) type;
	result[3].buffer_length = 256;

	mysql_stmt_bind_param(stmt, bind);
	mysql_stmt_bind_result(stmt, result);
	mysql_stmt_execute(stmt);

	while(mysql_stmt_fetch(stmt) == 0) {
		Edge e(type);
		e.setId(edgeId);
		e.setSourceId(sourceId);
		e.setTargetId(targetId);
		e.setDbHandler(my);
		edges.push_back(e);
	}

	mysql_stmt_free_result(stmt);
	mysql_stmt_close(stmt);

	return edges;
}