
#include "visualization.h"


using namespace std;

void draw_graph( Result res ) {

	 fstream f;
	 f.open("visualization.dot", ios::out);
	 f << "digraph visualization {" << endl;
	 f << "nodesep=5.0" << endl;

		vector<Node> nodes = res.getNodes();
		vector<Edge> edges = res.getUniqueEdges();

	 for(vector<Node>::iterator it = nodes.begin(); it != nodes.end(); it++) {
		 f << it->getId() << "[shape=none, margin=0, label =<" << endl;
		 f << "<TABLE BORDER=\"1\" CELLBORDER=\"0\" CELLPADDING=\"5\" CELLSPACING=\"5\">" << endl; // CELLSPACING=\"0\" CELLPADDING=\"4\"
		 f << "<TR><TD bgcolor=\"lightblue\">" << it->getType() << "</TD></TR>" << endl;

		 map<std::string, std::string> nodeprops = it->getProperties();
		 for(map<std::string, std::string>::iterator propsIt = nodeprops.begin(); propsIt != nodeprops.end(); propsIt++) {
			 f << "<TR><TD>" << propsIt->first << ": ";
			 f << propsIt->second << "</TD></TR>" << endl;
		 }
		 f << "</TABLE>>]" << endl;
	 }

	 for(vector<Edge>::iterator it = edges.begin(); it != edges.end(); it++) {
		 f << it->getSource().getId();
	 	 f << " -> ";
	 	 f << it->getTarget().getId();
	 	 f << "[label=<" << endl;
	 	f << "<TABLE BORDER=\"0\" CELLBORDER=\"0\" CELLPADDING=\"0\" CELLSPACING=\"5\">" << endl;
	 	 //f << "<u>" << it->getType() << "</u><br/>" << endl;
		f << "<TR><TD align=\"left\"><FONT POINT-SIZE=\"16.0\" COLOR=\"#00008B\">" << it->getType() << "</FONT></TD></TR>" << endl;
	 	 map<std::string, std::string> edgeprops = it->getProperties();
	 	for(map<std::string, std::string>::iterator propsIt = edgeprops.begin(); propsIt != edgeprops.end(); propsIt++) {
	 		//f << propsIt->first << ": " << propsIt->second << "<br/>" << endl;
			 f << "<TR><TD align=\"left\">" << propsIt->first << ": ";
			 f << propsIt->second << "</TD></TR>" << endl;
	 	}
	 	f << "</TABLE>>]" << endl;
	 	//f << ">]" << endl;
	 }

	f << "}" << endl;
	f.close();

}


