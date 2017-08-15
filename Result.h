#pragma once
#include <set>
#include <algorithm>
#include "visualization.h"
#include "DBUtil.h"


class Node;
class Edge;

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

