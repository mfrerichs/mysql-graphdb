#include "Track.h"


Track::Track(void)
{
}


Track::~Track(void)
{
}

/** Alle Kanten die zum Pfad gehoeren werden in einen Vector abgelegt
*	@param e 	Kante die zum Pfad gehoert
*/
void Track::add(Edge e) {
	edges.push_back(e);
}

/** Alle Knoten die zum Pfad gehoeren werden in einen Vector abgelegt
*	@param n 	Knoten der zum Pfad gehoert
*/
void Track::add(Node n) {
	nodes.push_back(n);
}

/** Erzeugt einen Pfad als Folge aus Knoten und Kanten
*	@return Gibt einen Vector zurueck, der die Folge aus Knoten und Kanten enthaelt, die den Pfad repraesentieren
*/
std::vector<GraphElement*> Track::getPath() {
	std::vector<GraphElement*> gev;
	std::vector<Node>::iterator nodeIt;
	std::vector<Edge>::iterator edgeIt;
	
	for(nodeIt = nodes.begin(), edgeIt = edges.begin(); nodeIt != nodes.end(); nodeIt++, edgeIt++) {
		gev.push_back(&(*nodeIt));
		// std::cout << nodeIt->getId() << " >> Type: " << nodeIt->getType() << std::endl;
		if(edgeIt != edges.end()) {
			// std::cout << edgeIt->getId() << " >> Type: " << edgeIt->getType() << std::endl;
			gev.push_back(&(*edgeIt));
		}
	}

	return gev;
}

void Track::deleteLastNode() {
	nodes.pop_back();
}

void Track::deleteLastEdge() {
	edges.pop_back();
}