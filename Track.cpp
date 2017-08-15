#include "Track.h"


Track::Track(void)
{
}


Track::~Track(void)
{
}

void Track::add(Edge e) {
	edges.push_back(e);
}

void Track::add(Node n) {
	nodes.push_back(n);
}

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