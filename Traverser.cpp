#include "Traverser.h"

Traverser::Traverser(Node& n)
{
	this->startNode = &n;
	this->direction = ALL;
	this->keepGoing = true;
	this->my = n.getDbHandler();
}


Traverser::~Traverser(void)
{
}

void Traverser::setDirection(int direction) {
	this->direction = direction;
}

void Traverser::addEdgeType(char* type) {
	this->edgeTypes.push_back(type);
}

void Traverser::addNodeType(char* type) {
	this->nodeTypes.push_back(type);
}

void Traverser::addEdgeProperty(char* prop) {
	this->edgeProps.insert(std::pair<std::string,std::vector<std::string> >(std::string(prop), std::vector<std::string>()));
}

void Traverser::addNodeProperty(char* prop) {
	this->nodeProps.insert(std::pair<std::string,std::vector<std::string> >(std::string(prop), std::vector<std::string>()));
}

void Traverser::addEdgeProperty(char* key, char* val) {
	if(!edgeProps.count(key)) {
		edgeProps.insert(std::pair<std::string,std::vector<std::string> >(std::string(key), std::vector<std::string>()));
	} 
	edgeProps.at(key).push_back(val);
}

void Traverser::addNodeProperty(char* key, char* val) {
	if(!nodeProps.count(key)) {
		nodeProps.insert(std::pair<std::string,std::vector<std::string> >(std::string(key), std::vector<std::string>()));
	} 
	nodeProps.at(key).push_back(val);
}

MYSQL* Traverser::getDbHandler() const {
	return my;
}

std::vector<Track> Traverser::paths(Node& source, Node& target) {
	std::vector<Track> tracks;
	std::vector<Edge> visited;
	Track track;

	pathDepthFirstRecursive(track, tracks, visited, source, target);

	return tracks;
}

Result Traverser::depthFirst() {
	Result r(this->getDbHandler());

	if(!keepGoing && !nodeTypes.empty() && !isTypeOf(nodeTypes, startNode->getType())) {
		return r;
	}

	if(!keepGoing && !nodeProps.empty() && !isPropertyOf(nodeProps, startNode->getProperties())) {
		return r;
	}

	depthFirstRecursive(r, *startNode);

	std::vector<Node> nodes;
	if(!nodeTypes.empty()) {
		nodes = r.getNodes();
		for(std::vector<Node>::iterator it = nodes.begin(); it != nodes.end(); it++) {
			if(!isTypeOf(nodeTypes, it->getType())) {
				r.deleteNode(*it);

				std::vector<Edge> ve = it->getEdges(ALL);
				for(std::vector<Edge>::iterator edgeIt = ve.begin(); edgeIt != ve.end(); edgeIt++) {
					r.deleteEdge(*edgeIt);
				}
			}
		}
	}

	if(!nodeProps.empty()) {
		nodes = r.getNodes();
		for(std::vector<Node>::iterator it = nodes.begin(); it != nodes.end(); it++) {
			if(!isPropertyOf(nodeProps, it->getProperties())) {
				r.deleteNode(*it);

				std::vector<Edge> ve = it->getEdges(ALL);
				for(std::vector<Edge>::iterator edgeIt = ve.begin(); edgeIt != ve.end(); edgeIt++) {
					r.deleteEdge(*edgeIt);
				}
			}
		}
	}

	std::vector<Edge> edges;
	if(!edgeTypes.empty()) {
		edges = r.getEdges();
		for(std::vector<Edge>::iterator it = edges.begin(); it != edges.end(); it++) {
			if(!isTypeOf(edgeTypes, it->getType())) {
				r.deleteEdge(*it);

				Edge e = *it;
				Node src = e.getSource();
				std::vector<Edge> srcEdges = src.getEdges(ALL);
				bool deleteNode = true;
				for(std::vector<Edge>::iterator edgeIt = srcEdges.begin(); (edgeIt != srcEdges.end()) && deleteNode; edgeIt++) {
					for(std::vector<char*>::iterator edgeTypesIt = edgeTypes.begin(); edgeTypesIt != edgeTypes.end(); edgeTypesIt++) {
						if(!strcmp(edgeIt->getType(), *edgeTypesIt)) {
							deleteNode = false;
							break;
						}
					}
				}

				if(deleteNode) {
					r.deleteNode(src);
				}

				Node target = e.getTarget();
				std::vector<Edge> targetEdges = target.getEdges(ALL);
				deleteNode = true;
				for(std::vector<Edge>::iterator edgeIt = targetEdges.begin(); (edgeIt != targetEdges.end()) && deleteNode; edgeIt++) {
					for(std::vector<char*>::iterator edgeTypesIt = edgeTypes.begin(); edgeTypesIt != edgeTypes.end(); edgeTypesIt++) {
						if(!strcmp(edgeIt->getType(), *edgeTypesIt)) {
							deleteNode = false;
							break;
						}
					}
				}

				if(deleteNode) {
					r.deleteNode(target);
				}
			}
		}
	}
	
	if(!edgeProps.empty()) {
		edges = r.getEdges();
		for(std::vector<Edge>::iterator it = edges.begin(); it != edges.end(); it++) {
			if(!isPropertyOf(edgeProps, it->getProperties())) {
				r.deleteEdge(*it);

				Edge e = *it;
				Node src = e.getSource();
				std::vector<Edge> srcEdges = src.getEdges(ALL);
				bool deleteNode = true;
				for(std::vector<Edge>::iterator edgeIt = srcEdges.begin(); (edgeIt != srcEdges.end()) && deleteNode; edgeIt++) {
					for(std::map<std::string, std::vector<std::string> >::iterator edgePropsIt = edgeProps.begin(); edgePropsIt != edgeProps.end(); edgePropsIt++) {
						if(isPropertyOf(edgeProps, edgeIt->getProperties())) {
							deleteNode = false;
							break;
						}
					}
				}

				if(deleteNode) {
					r.deleteNode(src);
				}

				Node target = e.getTarget();
				std::vector<Edge> targetEdges = target.getEdges(ALL);
				deleteNode = true;
				for(std::vector<Edge>::iterator edgeIt = targetEdges.begin(); (edgeIt != targetEdges.end()) && deleteNode; edgeIt++) {
					for(std::map<std::string, std::vector<std::string> >::iterator edgePropsIt = edgeProps.begin(); edgePropsIt != edgeProps.end(); edgePropsIt++) {
						if(isPropertyOf(edgeProps, edgeIt->getProperties())) {
							deleteNode = false;
							break;
						}
					}
				}

				if(deleteNode) {
					r.deleteNode(target);
				}
			}
		}
	}
	

	return r;
}

void Traverser::depthFirstRecursive(Result& r, Node& startNode) {
	r.addNode(startNode);

	std::vector<Edge> edges = startNode.getEdges(direction);

	for(std::vector<Edge>::iterator it = edges.begin(); it != edges.end(); it++) {
		int neighbour;
		if((neighbour = it->getTargetId()) == startNode.getId()) {
			neighbour = it->getSourceId();
		} 

		std::vector<Node> nodes = r.getNodes();
		Node neighbourNode = get_node_by_id(my, neighbour);

		if(!keepGoing && !nodeTypes.empty() && !isTypeOf(nodeTypes, neighbourNode.getType())) {
			continue;
		}

		if(!keepGoing && !nodeProps.empty() && !isPropertyOf(nodeProps, neighbourNode.getProperties())) {
			continue;
		}

		if(!keepGoing && !edgeTypes.empty() && !isTypeOf(edgeTypes, it->getType())) {
			continue;
		}

		if(!keepGoing && !edgeProps.empty() && !isPropertyOf(edgeProps, it->getProperties())) {
			continue;
		}

		r.addEdge(*it);

		bool found = false;
		for(std::vector<Node>::iterator it = nodes.begin(); it != nodes.end(); it++) {
			if(neighbourNode.getId() == it->getId()) {
				found = true;
				break;
			}
		}

		if(!found) {
			depthFirstRecursive(r, neighbourNode);
		}
		
	}
}

bool Traverser::isTypeOf(std::vector<char*>& vec, char* type) {
	for(std::vector<char*>::iterator typeIt = vec.begin(); typeIt != vec.end(); typeIt++) {
		if(!strcmp(*typeIt, type)) {
			return true;
		}
	}

	return false;
}

bool Traverser::isPropertyOf(std::map<std::string, std::vector<std::string> >& definedProps, std::map<std::string, std::string>& nodeProps) {
	for(std::map<std::string, std::vector<std::string> >::iterator it = definedProps.begin(); it != definedProps.end(); it++) {
		for(std::map<std::string, std::string>::iterator nodePropsIt = nodeProps.begin(); nodePropsIt != nodeProps.end(); nodePropsIt++) {
			if(!it->first.compare(nodePropsIt->first)) {
				if(!it->second.empty()) {
					std::vector<std::string> vec = definedProps.at(it->first);
					for(std::vector<std::string>::iterator vecIt = vec.begin(); vecIt != vec.end(); vecIt++) {
						if(!nodePropsIt->second.compare(*vecIt)) {
							return true;
						}
					}
					return false;
				}

				return true;
			}
		}
	}

	return false;
}

void Traverser::setKeepGoing(bool keepGoing) {
	this->keepGoing = keepGoing;
}

void Traverser::pathDepthFirstRecursive(Track& track, std::vector<Track>& tracks, std::vector<Edge>& visited, Node& source, Node& target) {
	std::vector<Edge> edges = source.getEdges(direction);
	track.add(source);
	int visited_counter = 0;
	for(std::vector<Edge>::iterator it = edges.begin(); it != edges.end(); it++) {

		int neighbour;
		if((neighbour = it->getTargetId()) == source.getId()) {
			neighbour = it->getSourceId();
		} 

		Node neighbourNode = get_node_by_id(my, neighbour);

		if(!nodeTypes.empty() && !isTypeOf(nodeTypes, neighbourNode.getType())) {
			continue;
		}

		if(!nodeProps.empty() && !isPropertyOf(nodeProps, neighbourNode.getProperties())) {
			continue;
		}

		if(!edgeTypes.empty() && !isTypeOf(edgeTypes, it->getType())) {
			continue;
		}

		if(!edgeProps.empty() && !isPropertyOf(edgeProps, it->getProperties())) {
			continue;
		}

		bool found = false;
		for(std::vector<Edge>::iterator visitedIt = visited.begin(); visitedIt != visited.end(); visitedIt++) {
			if(it->getId() == visitedIt->getId()) {
				found = true;
				break;
			}
		}

		if(!found) {
			visited_counter++;
			visited.push_back(*it);
		} else {
			continue;
		}

		track.add(*it);

		if(neighbourNode.getId() != target.getId()) {
			pathDepthFirstRecursive(track, tracks, visited, neighbourNode, target);
		} else {
			track.add(neighbourNode);
			tracks.push_back(track);
			track.deleteLastNode();
			track.deleteLastEdge();
		}

	}

	for(int i = 0; i < visited_counter; i++) {
		visited.pop_back();
	}

	track.deleteLastEdge();
	track.deleteLastNode();
}