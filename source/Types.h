/** @file Types.h
 *  @brief Bietet Raum fuer jegliche Art von sonstigen Definitionen.
 */

#pragma once
#include "GraphElement.h"

/** Definiert eine Enumeration fuer die Kantenrichtung */
enum DIRECTION { /**- nur eingehende Kanten*/ INCOMING, /**- nur ausgehende Kanten*/ OUTGOING, /**- alle Kanten*/ ALL };