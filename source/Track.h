#pragma once
#include <vector>
#include <iostream>
#include "GraphElement.h"
#include "Edge.h"
#include "Node.h"

/**
 * \brief Repraesentiert einen Pfad zwischen zwei Knoten. Wird i.d.R. durch eine Pfadsuche mittels Traverser erzeugt.
 */
class Track
{
private:
	std::vector<Node> nodes;
	std::vector<Edge> edges;
public:
	Track(void);
	~Track(void);
	void add(Edge);
	void add(Node);
	void deleteLastNode();
	void deleteLastEdge();
	std::vector<GraphElement*> getPath();
};

