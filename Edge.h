#pragma once
#include "GraphElement.h"

class Node;
class GraphDB;

class Edge : public GraphElement
{
private:
	int source;
	int target;

public:
	Edge(char*);
	Edge(GraphDB&, Node&, Node&, char*);
	~Edge(void);

	int getSourceId() const;
	int getTargetId() const;
	void setSourceId(int);
	void setTargetId(int);
	Node getSource() const;
	void setSource(Node const&);
	Node getTarget() const;
	void setTarget(Node const&);
	std::string getProperty(char*) const;
	void setProperty(char*, char*);
	void deleteProperty(char*);
	void clearProperties();
	std::map<std::string, std::string> getProperties() const;
	void remove();
};

