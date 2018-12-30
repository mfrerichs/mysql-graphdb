#pragma once
#include <set>
#include <algorithm>
#include "visualization.h"
#include "DBUtil.h"


class Node;
class Edge;

/**
 * \brief Repraesentiert ein Ergebnis einer Graphtraversierung mittels Traverser. Ein Result bietet Zugriff auf die gefundenen Knoten und Kanten einer Graphtraversierung.
 */
class Result
{
private:
	MYSQL *my;
	std::vector<int> nodeIds;
	std::vector<int> edgeIds;
public:
	Result(MYSQL*);
	~Result(void);
	void print();
	void addNode(Node&);
	void addEdge(Edge&);
	std::vector<Node> getNodes();
	std::vector<Edge> getEdges();
	std::vector<Edge> getUniqueEdges();
	void deleteNode(Node&);
	void deleteEdge(Edge&);
};

