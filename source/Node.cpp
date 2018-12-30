#include "Node.h"
#include "DBUtil.h"
#include "GraphDB.h"

Node::Node(char* type) : GraphElement(type)
{
}

/** Erzeugt ein Knotenobjekt (Node)
*	@param gdb 		Verweis auf die verwendete Datenbank
*	@param type		Typ des Knoten
*/
Node::Node(GraphDB& gdb, char* type) : GraphElement(type)
{
	this->my = gdb.getDbHandler();
	this->id = create_node(gdb.getDbHandler(), type);
}

Node::~Node(void)
{
}

/** Gibt die anliegenden Kanten eines Knoten zurueck
*	@param 	direction	Kann als Einschraenkung genutzt werden, um nur eingehende (INCOMING) <BR> oder nur ausgehende (OUTGOING) Kanten zurueckgeben zulassen
*	@return	Je nach gewaehlter Einschraenkung erhaelt man eine Liste mit allen anliegenden <BR> Kanten für den ausfuehrenden Knoten zurueck
*/
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