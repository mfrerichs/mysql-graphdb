
#include "GraphDB.h"
#include "DBUtil.h"

/**
 * @param Name der Datenbank
 */
GraphDB::GraphDB(char *database_name) {
	mysql = init_mysql();
	connect (mysql);

	create_db(mysql, database_name);
	select_db(mysql, database_name);

	create_node_table(mysql);
	create_edge_table(mysql);
	create_node_properties_table(mysql);
	create_edge_properties_table(mysql);
}

GraphDB::~GraphDB(void) {
	mysql_close (mysql);
}

/**
 * Alle Knoten aus der Datenbank laden.
 * @return Knoten-Vektor
 */
std::vector<Node> GraphDB::getAllNodes() const {
	return get_all_nodes(mysql);
}

/**
 * Alle Kanten aus der Datenbank laden.
 * @return Kanten-Vektor
 */
std::vector<Edge> GraphDB::getAllEdges() const {
	return get_all_edges(mysql);
}

/**
 * Einen Knoten anhand einer ID ermitteln und aus der Datenbank laden.
 * @param id Identifikation
 * @return Knoten
 */
Node GraphDB::getNodeById(int id) const {
	return get_node_by_id(mysql, id);
}

/**
 * Eine Kante anhand einer ID ermitteln und aus der Datenbank laden.
 * @param id Identifikation
 * @return Kante
 */
Edge GraphDB::getEdgeById(int id) const {
	return get_edge_by_id(mysql, id);
}

MYSQL* GraphDB::getDbHandler() const {
	return mysql;
}

/**
 * Datenbank loeschen.
 */
void GraphDB::deleteDB() {
	delete_database (mysql);
}

/**
 * Alle Eintraege in der Datenbank loeschen.
 */
void GraphDB::clearDB() {
	clear_database (mysql);
}

/**
 * Alle Knoten eines bestimmten Typs aus der Datenbank laden.
 * @param type Typ-Bezeichnung
 * @return Knoten-Vektor
 */
std::vector<Node> GraphDB::getTypedNodes(char* type) const {
	return get_typed_nodes(mysql, type);
}

/**
 * Alle Kanten eines bestimmten Typs aus der Datenbank laden.
 * @param type Typ-Bezeichnung
 * @return Kanten-Vektor
 */
std::vector<Edge> GraphDB::getTypedEdges(char* type) const {
	return get_typed_edges(mysql, type);
}

/**
 * Alle Knoten, die eine bestimmte Eigenschaft besitzen, aus der Datenbank laden.
 * @param key Attribut-Bezeichnung
 * @return Knoten-Vektor
 */
std::vector<Node> GraphDB::getPropertyNodes(char* key) const {
	return get_property_nodes(mysql, key);
}

/**
 * Alle Kanten, die eine bestimmte Eigenschaft besitzen, aus der Datenbank laden.
 * @param key Attribut-Bezeichnung
 * @return Kanten-Vektor
 */
std::vector<Edge> GraphDB::getPropertyEdges(char* key) const {
	return get_property_edges(mysql, key);
}

/**
 * Alle Knoten, die eine bestimmte Eigenschaft besitzen, die einen bestimmten Wert aufweist, aus der Datenbank laden.
 * @param key Attribut-Bezeichnung
 * @param val Wert
 * @return Knoten-Vektor
 */
std::vector<Node> GraphDB::getPropertyNodes(char* key, char* val) const {
	return get_property_nodes(mysql, key, val);
}

/**
 * Alle Kanten, die eine bestimmte Eigenschaft besitzen, die einen bestimmten Wert aufweist, aus der Datenbank laden.
 * @param key Attribut-Bezeichnung
 * @param val Wert
 * @return Kanten-Vektor
 */
std::vector<Edge> GraphDB::getPropertyEdges(char* key, char* val) const {
	return get_property_edges(mysql, key, val);
}

