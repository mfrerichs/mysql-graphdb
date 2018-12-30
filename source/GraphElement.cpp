#include "GraphElement.h"

GraphElement::GraphElement(char* type) {
	this->type = (char *) malloc(strlen(type));
	strcpy(this->type, type);
}

char* GraphElement::getType() const {
	return type;
}

int GraphElement::getId() const {
	return id;
}

void GraphElement::setId(int id) {
	this->id = id;
}

void GraphElement::setDbHandler(MYSQL* my) {
	this->my = my;
}

MYSQL* GraphElement::getDbHandler() const {
	return my;
}
