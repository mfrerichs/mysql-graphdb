#pragma once
#include <map>
#include "my_global.h"
#include "mysql.h"

/**
 * \brief Oberklasse der Klassen Node und Edge. Implementiert bzw. definiert also generelle Operationen, die von beiden Unterklassen unterstuetzt werden sollen. Repraesentiert letztlich ein Element (Knoten/Kante) im Graph.
 */
class GraphElement
{
protected: 
	char *type;
	int id;
	MYSQL *my;
public:
	GraphElement(char*);
	virtual std::string getProperty(char*) const = 0;
	virtual void setProperty(char*, char*) = 0;
	virtual void deleteProperty(char*) = 0;
	virtual void clearProperties() = 0;
	virtual void remove() = 0;
	virtual std::map<std::string, std::string> getProperties() const = 0;
	
	char* getType() const;
	int getId() const;
	void setId(int);
	void setDbHandler(MYSQL*);
	MYSQL* getDbHandler() const;
};

