#include "GraphDB.h"
#include "DBUtil.h"

GraphDB::GraphDB(char *database_name)
{
	mysql = init_mysql();
	connect(mysql);

	create_db(mysql, database_name);
	select_db(mysql, database_name);

	create_node_table(mysql);
	create_edge_table(mysql);
	create_node_properties_table(mysql);
	create_edge_properties_table(mysql);
}

GraphDB::~GraphDB(void)
{
	mysql_close(mysql);
}

std::vector<Node> GraphDB::getAllNodes() const {
	return get_all_nodes(mysql);
}

std::vector<Edge> GraphDB::getAllEdges() const {
	return get_all_edges(mysql);
}

Node GraphDB::getNodeById(int id) const {
	return get_node_by_id(mysql, id);
}
Edge GraphDB::getEdgeById(int id) const {
	return get_edge_by_id(mysql, id);
}

MYSQL* GraphDB::getDbHandler() const {
	return mysql;
}

void GraphDB::deleteDB() {
	delete_database(mysql);
}

void GraphDB::clearDB() {
	clear_database(mysql);
}

std::vector<Node> GraphDB::getTypedNodes(char* type) const {
	return get_typed_nodes(mysql, type);
}

std::vector<Edge> GraphDB::getTypedEdges(char* type ) const {
	return get_typed_edges(mysql, type);
}

std::vector<Node> GraphDB::getPropertyNodes(char* key) const {
	return get_property_nodes(mysql, key);
}

std::vector<Edge> GraphDB::getPropertyEdges(char* key) const {
	return get_property_edges(mysql, key);
}

std::vector<Node> GraphDB::getPropertyNodes(char* key, char* val) const {
	return get_property_nodes(mysql, key, val);
}

std::vector<Edge> GraphDB::getPropertyEdges(char* key, char* val) const {
	return get_property_edges(mysql, key, val);
}