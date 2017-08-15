#ifndef DBUTIL_H
#define DBUTIL_H

#include <map>
#include <vector>
#include <string>
#include "my_global.h"
#include "mysql.h"
#include "GraphElement.h"
#include "Node.h"
#include "Edge.h"

void check_error(MYSQL*);
void create_db(MYSQL*, char*);
void select_db(MYSQL*, char*);
void connect(MYSQL*);
MYSQL* init_mysql();
int execute_query(MYSQL*, char*);
void create_node_table(MYSQL*);
void create_edge_table(MYSQL*);
void create_node_properties_table(MYSQL*);
void create_edge_properties_table(MYSQL*);
std::vector<Node> get_all_nodes(MYSQL*);
std::vector<Edge> get_all_edges(MYSQL*);
int create_node(MYSQL*, char*);
int create_edge(MYSQL*, Node const&, Node const&, char*);
Node get_edge_source(MYSQL*, Edge const&);
void set_edge_source(MYSQL*, Edge const&, Node const&);
Node get_edge_target(MYSQL*, Edge const&);
void set_edge_target(MYSQL*, Edge const&, Node const&);
Node get_node_by_id(MYSQL*, int);
Edge get_edge_by_id(MYSQL*, int);
std::vector<Edge> get_incoming_edges(MYSQL*, Node const&);
std::vector<Edge> get_outgoing_edges(MYSQL*, Node const&);
std::vector<Edge> get_connected_edges(MYSQL*, Node const&);
std::string get_edge_property(MYSQL*, Edge const&, char*);
std::string get_node_property(MYSQL*, Node const&, char*);
void set_edge_property(MYSQL*, Edge const&, char*, char*);
void set_node_property(MYSQL*, Node const&, char*, char*);
void delete_edge_property(MYSQL*, Edge const&, char*);
void delete_node_property(MYSQL*, Node const&, char*);
void delete_edge_properties(MYSQL*, Edge const&);
void delete_node_properties(MYSQL*, Node const&);
void delete_database(MYSQL*);
void clear_database(MYSQL*);
void remove_node(MYSQL*, Node const&);
void remove_edge(MYSQL*, Edge const&);
std::map<std::string, std::string> get_node_properties(MYSQL*, Node const&);
std::map<std::string, std::string> get_edge_properties(MYSQL*, Edge const&);
std::vector<Node> get_typed_nodes(MYSQL*, char*);
std::vector<Edge> get_typed_edges(MYSQL*, char*);
std::vector<Node> get_property_nodes(MYSQL*, char*);
std::vector<Edge> get_property_edges(MYSQL*, char*);
std::vector<Node> get_property_nodes(MYSQL*, char*, char*);
std::vector<Edge> get_property_edges(MYSQL*, char*, char*);
#endif