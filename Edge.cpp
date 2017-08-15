#include "Edge.h"
#include "DBUtil.h"
#include "GraphDB.h"

Edge::Edge(char* type) : GraphElement(type)
{
}

Edge::Edge(GraphDB& gdb, Node& fromNode, Node& toNode, char* type) : GraphElement(type)
{
	this->my = gdb.getDbHandler();
	this->id = create_edge(gdb.getDbHandler(), fromNode, toNode, type);
	this->source = fromNode.getId();
	this->target = toNode.getId();
}

Edge::~Edge(void) 
{
}

int Edge::getSourceId() const {
	return source;
}

int Edge::getTargetId() const {
	return target;
}

void Edge::setSourceId(int source) {
	this->source = source;
}

void Edge::setTargetId(int target) {
	this->target = target;
}

Node Edge::getSource() const {
	return get_node_by_id(my, source);
}

Node Edge::getTarget() const {
	return get_node_by_id(my, target);
}

void Edge::setSource(Node const& n) {
	set_edge_source(my, *this, n);
	this->source = n.getId();
}

void Edge::setTarget(Node const& n) {
	set_edge_target(my, *this, n);
	this->target = n.getId();
}

std::string Edge::getProperty(char* key) const {
	return get_edge_property(my, *this, key);
}

void Edge::setProperty(char* key, char* value) {
	set_edge_property(my, *this, key, value);
}

void Edge::deleteProperty(char* key) {
	delete_edge_property(my, *this, key);
}

void Edge::clearProperties() {
	delete_edge_properties(my, *this);
}

void Edge::remove() {
	clearProperties();
	remove_edge(my, *this);
}

std::map<std::string, std::string> Edge::getProperties() const {
	return get_edge_properties(my, *this);
}