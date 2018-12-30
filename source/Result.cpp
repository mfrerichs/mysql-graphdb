#include "Result.h"


/**
 * @param my Datenbank-Pointer
 */
Result::Result(MYSQL* my) {
	this->my = my;
}

Result::~Result(void) {
}

/**
 * @param e Kante
 */
void Result::addEdge(Edge& e) {
	this->edgeIds.push_back(e.getId());
}

/**
 * @param n Knoten
 */
void Result::addNode(Node& n) {
	this->nodeIds.push_back(n.getId());
}

/**
 * @return Knoten-Vektor
 */
std::vector<Node> Result::getNodes() {
	std::vector < Node > nodes;

	for (std::vector<int>::iterator it = this->nodeIds.begin();
			it != this->nodeIds.end(); it++) {
		nodes.push_back(get_node_by_id(my, *it));
	}

	return nodes;
}


/**
 * @return Kanten-Vektor
 */
std::vector<Edge> Result::getEdges() {
	std::vector < Edge > edges;

	for (std::vector<int>::iterator it = this->edgeIds.begin();
			it != this->edgeIds.end(); it++) {
		edges.push_back(get_edge_by_id(my, *it));
	}

	return edges;
}


/**
 * @return Kanten-Vektor
 */
std::vector<Edge> Result::getUniqueEdges() {
	std::vector < Edge > edges;
	std::set<int> s;

	unsigned size = edgeIds.size();
	for (unsigned i = 0; i < size; ++i) {
		s.insert(edgeIds[i]);
	}

	for (std::set<int>::iterator it = s.begin(); it != s.end(); it++) {
		edges.push_back(get_edge_by_id(my, *it));
	}

	return edges;
}


/**
 * @param n Knoten
 */
void Result::deleteNode(Node& n) {
	nodeIds.erase(std::remove(nodeIds.begin(), nodeIds.end(), n.getId()),
			nodeIds.end());
}

/**
 * @param e Kanten
 */
void Result::deleteEdge(Edge& e) {
	edgeIds.erase(std::remove(edgeIds.begin(), edgeIds.end(), e.getId()),
			edgeIds.end());
}

void Result::print() {
	draw_graph(*this);
	char* rest =
			"/release/bin/dot -Tpng visualization.dot -o C:/Users/Marc/Desktop/visualization.png";
	char* graphviz = (char*) malloc(strlen(GRAPHVIZ_PATH) + strlen(rest) + 1);
	strcpy(graphviz, GRAPHVIZ_PATH);
	strcat(graphviz, rest);
	system(graphviz);
	system(
			"rundll32.exe \"%ProgramFiles%\\Windows Photo Viewer\\PhotoViewer.dll\",ImageView_Fullscreen C:\\Users\\Marc\\Desktop\\visualization.png");
	free(graphviz);
}

