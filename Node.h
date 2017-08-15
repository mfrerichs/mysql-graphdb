#pragma once
#include "GraphElement.h"
#include <vector>
#include "Types.h"

class Edge;
class GraphDB;

class Node : public GraphElement
{
public:
	Node(char*);
	Node(GraphDB&, char*);
	~Node(void);
	std::vector<Edge> getEdges(int) const;
	std::string getProperty(char*) const;
	void setProperty(char*, char*);
	void deleteProperty(char*);
	void clearProperties();
	std::map<std::string, std::string> getProperties() const;
	void remove();
};

