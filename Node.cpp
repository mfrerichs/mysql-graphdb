#include "Node.h"
#include "DBUtil.h"
#include "GraphDB.h"

Node::Node(char* type) : GraphElement(type)
{
}

Node::Node(GraphDB& gdb, char* type) : GraphElement(type)
{
	this->my = gdb.getDbHandler();
	this->id = create_node(gdb.getDbHandler(), type);
}

Node::~Node(void)
{
}

std::vector<Edge> Node::getEdges(int direction) const {

	if(direction == INCOMING) {
		return get_incoming_edges(my, *this);
	}

	if(direction == OUTGOING) {
		return get_outgoing_edges(my, *this);
	}

	return get_connected_edges(my, *this);

}

std::string Node::getProperty(char* key) const {
	return get_node_property(my, *this, key);
}

void Node::setProperty(char* key, char* value) {
	set_node_property(my, *this, key, value);
}

void Node::deleteProperty(char* key) {
	delete_node_property(my, *this, key);
}

void Node::clearProperties() {
	delete_node_properties(my, *this);
}

void Node::remove() {
	clearProperties();
	remove_node(my, *this);
}

std::map<std::string, std::string> Node::getProperties() const {
	return get_node_properties(my, *this);
}