#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "GraphDB.h"
#include "DBUtil.h"
#include "Traverser.h"
#include "visualization.h"

using namespace std;

int main()
{
	// GraphDB erzeugen
	GraphDB gdb("soziales_netzwerk");
	gdb.clearDB();
	system("cls");

	// Knoten erzeugen
	Node peter(gdb, "Person");
	peter.setProperty("Name", "Peter");
	peter.setProperty("Alter", "29");

	Node frieda(gdb, "Person");
	frieda.setProperty("Name", "Frieda");
	frieda.setProperty("Wohnort", "Leer");

	Node susanne(gdb, "Person");
	susanne.setProperty("Name", "Susanne");
	susanne.setProperty("Geschlecht", "w");

	Node martin(gdb, "Person");
	martin.setProperty("Name", "Martin");
	martin.setProperty("Wohnort", "Emden");

	Node hennes(gdb, "Geissbock");
	hennes.setProperty("Name", "Hennes");
	hennes.setProperty("Alter", "6");
	hennes.setProperty("Wohnort", "Koeln");

	Node heiko(gdb, "Person");
	heiko.setProperty("Name", "Heiko");

	Node peter2(gdb, "Person");
	peter2.setProperty("Name", "Peter");
	peter2.setProperty("Wohnort", "Leer");
	peter2.setProperty("Geschlecht", "m");

	// Kanten erzeugen
	Edge peter_kennt_frieda(gdb, peter, frieda, "kennt");
	peter_kennt_frieda.setProperty("seit", "gestern");

	Edge peter_kennt_martin(gdb, peter, martin, "kennt");
	peter_kennt_martin.setProperty("seit", "2010");

	Edge martin_ist_interessiert_an_susanne(gdb, martin, susanne, "ist_interessiert_an");
	martin_ist_interessiert_an_susanne.setProperty("wie", "sehr");

	Edge frieda_kennt_susanne(gdb, frieda, susanne, "kennt");
	frieda_kennt_susanne.setProperty("seit", "1999");

	Edge susanne_kennt_heiko(gdb, susanne, heiko, "kennt");

	Edge hennes_bewundert_heiko(gdb, hennes, heiko, "bewundert");
	hennes_bewundert_heiko.setProperty("seit", "2010");

	Edge heiko_kennt_peter2(gdb, heiko, peter2, "kennt");
	heiko_kennt_peter2.setProperty("wie", "wenig");
	heiko_kennt_peter2.setProperty("seit", "2010");

	Edge peter2_mag_susanne(gdb, peter2, susanne, "mag");
	peter2_mag_susanne.setProperty("wie", "sehr");

	Node irgendjemand(gdb, "Person");
	Edge irgendjemand_kennt_hennes(gdb, irgendjemand, hennes, "kennt");

	// Traversierungsobjekt erzeugen
	Traverser t(peter);
	t.addEdgeType("kennt");
	t.setKeepGoing(false);
	t.addNodeType("Person");

	// Ergebnismenge
	Result r = t.depthFirst();

	// Ergebnismenge ausgeben
	r.print();

	system("pause");
	system("cls");

	// Weg(e) von Frieda zu Heiko
	vector<Track> vt = t.paths(frieda, heiko);
	
	for(vector<Track>::iterator it = vt.begin(); it != vt.end(); it++) {
		std::vector<GraphElement*> vge = it->getPath();
		for(vector<GraphElement*>::iterator geIt = vge.begin(); geIt != vge.end(); geIt++) {
			cout << (*geIt)->getId() << ">> " << (*geIt)->getProperty("Name") << ": TYPE " << (*geIt)->getType() << endl;
		}
		cout << endl; cout << endl;
	}

	system("pause");
	system("cls");

	vector<Node> vn = r.getNodes();
	vector<Edge> ve = r.getEdges();

	cout << "NODES" << endl << "----------" << endl;
	for(std::vector<Node>::iterator it = vn.begin(); it != vn.end(); it++) {
		cout << it->getId() << ": " << it->getProperty("Name") << " (" << it->getType() << ")" << endl;
	}

	cout << endl;
	cout << "EDGES" << endl << "----------" << endl;

	for(std::vector<Edge>::iterator it = ve.begin(); it != ve.end(); it++) {
		cout << it->getId() << ": " << it->getType() << endl;
	}

	ve = r.getUniqueEdges();

	cout << endl;
	cout << "UNIQUE EDGES" << endl << "----------" << endl;

	for(std::vector<Edge>::iterator it = ve.begin(); it != ve.end(); it++) {
		cout << it->getId() << ">> SRC: " << it->getSource().getId() << " | TARGET: " << it->getTarget().getId() << " | " << it->getType() << endl;
	}
}