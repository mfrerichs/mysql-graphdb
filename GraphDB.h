#ifndef GRAPHDB_H
#define GRAPHDB_H

#include "DBUtil.h"

class GraphDB
{
private:
	MYSQL *mysql;
public:
	GraphDB(char*);
	~GraphDB(void);
	std::vector<Node> getAllNodes() const;
	std::vector<Edge> getAllEdges() const;
	Node getNodeById(int) const;
	Edge getEdgeById(int) const;
	MYSQL* getDbHandler() const;
	void deleteDB();
	void clearDB();
	std::vector<Node> getTypedNodes(char*) const;
	std::vector<Edge> getTypedEdges(char*) const;
	std::vector<Node> getPropertyNodes(char*) const;
	std::vector<Edge> getPropertyEdges(char*) const;
	std::vector<Node> getPropertyNodes(char*, char*) const;
	std::vector<Edge> getPropertyEdges(char*, char*) const;
};

#endif
