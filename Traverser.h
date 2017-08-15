#pragma once
#include <algorithm>
#include <iostream>
#include "DBUtil.h"
#include "Types.h"
#include "Result.h"
#include "Track.h"

class Traverser
{
private:
	MYSQL *my;
	Node *startNode;
	std::vector<char*> nodeTypes;
	std::map<std::string, std::vector<std::string> > nodeProps;
	std::vector<char*> edgeTypes;
	std::map<std::string, std::vector<std::string> > edgeProps;
	int direction;
	bool keepGoing;
	void depthFirstRecursive(Result&, Node&);
	bool isTypeOf(std::vector<char*>&, char*);
	bool isPropertyOf(std::map<std::string, std::vector<std::string>>&, std::map<std::string, std::string>&);
	void deleteNode(Result&, Edge&);
public:
	Traverser(Node&);
	~Traverser(void);
	void setDirection(int);
	void setKeepGoing(bool);
	void addEdgeType(char*);
	void addNodeType(char*);
	void addEdgeProperty(char*);
	void addNodeProperty(char*);
	void addEdgeProperty(char*, char*);
	void addNodeProperty(char*, char*);
	void pathDepthFirstRecursive(Track&, std::vector<Track>&, std::vector<Edge>&, Node&, Node&);
	Result breadthFirst();
	Result depthFirst();
	std::vector<Track> paths(Node&, Node&);
	MYSQL* getDbHandler() const;
};

