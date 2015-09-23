#ifndef MAP_H
#define MAP_H

#include "Places.h"

typedef struct MapEdge {
    LocationID next;
    TransportID method;
} MapEdge;

// Terminated by .next = NOWHERE
const MapEdge *getEdgesOf(LocationID from);

#endif
