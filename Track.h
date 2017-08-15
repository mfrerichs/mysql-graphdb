#pragma once
#include <vector>
#include <iostream>
#include "GraphElement.h"
#include "Edge.h"
#include "Node.h"

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

