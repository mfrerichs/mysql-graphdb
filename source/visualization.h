/** @file visualization.h
 *  @brief Stellt eine Funktionalitaet zur Visualisierung von Ergebnissen einer Graphtraversierung (Result) zur Verfuegung.
 * 	HINWEIS: Hierzu ist Graphviz erforderlich! -> GRAPHVIZ_PATH
 */

#ifndef VISUALIZATION_H_
#define VISUALIZATION_H_
#define GRAPHVIZ_PATH "C:/Users/Marc/Desktop/graphviz-2.36"

#include <fstream>
#include <vector>
#include <map>
#include <string>
#include "Node.h"
#include "Edge.h"
#include "Result.h"

class Result;

void draw_graph(Result);



#endif /* VISUALIZATION_H_ */
