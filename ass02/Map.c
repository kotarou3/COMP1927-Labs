#include <stdbool.h>
#include <stdlib.h>
#include <strings.h>

#include "Map.h"

// The map graph is completely static, but it's too much effort to write a
// source generator to generate the map at compile time, so this just
// initialises the map the first time getEdgesOf() is called

static struct MapNode {
    MapEdge *edges;
    size_t edgesCount;
    size_t edgesAllocated;
} nodes[NUM_MAP_LOCATIONS];

static MapEdge emptyEdge = {.next = NOWHERE, .method = NONE};

static void addEdge(LocationID from, LocationID to, TransportID method) {
    struct MapNode *node = &nodes[from];

    if (!node->edges) {
        node->edgesAllocated = 2;
        node->edges = malloc(node->edgesAllocated * sizeof(MapEdge));
    } else if (node->edgesCount + 1 == node->edgesAllocated) {
        node->edgesAllocated *= 2;
        node->edges = realloc(node->edges, node->edgesAllocated * sizeof(MapEdge));
    }

    node->edges[node->edgesCount++] = (MapEdge){.next = to, .method = method};
    node->edges[node->edgesCount] = emptyEdge;
}

static void addConnection(LocationID from, LocationID to, TransportID method) {
    addEdge(from, to, method);
    addEdge(to, from, method);
}

static void deinitMap() {
    for (size_t n = 0; n < NUM_MAP_LOCATIONS; ++n)
        free(nodes[n].edges);
}

static void initMap() {
    static bool isInited = false;
    if (isInited)
        return;

    // Road connections
    addConnection(ALICANTE, GRANADA, ROAD);
    addConnection(ALICANTE, MADRID, ROAD);
    addConnection(ALICANTE, SARAGOSSA, ROAD);
    addConnection(AMSTERDAM, BRUSSELS, ROAD);
    addConnection(AMSTERDAM, COLOGNE, ROAD);
    addConnection(ATHENS, VALONA, ROAD);
    addConnection(BARCELONA, SARAGOSSA, ROAD);
    addConnection(BARCELONA, TOULOUSE, ROAD);
    addConnection(BARI, NAPLES, ROAD);
    addConnection(BARI, ROME, ROAD);
    addConnection(BELGRADE, BUCHAREST, ROAD);
    addConnection(BELGRADE, KLAUSENBURG, ROAD);
    addConnection(BELGRADE, SARAJEVO, ROAD);
    addConnection(BELGRADE, SOFIA, ROAD);
    addConnection(BELGRADE, ST_JOSEPH_AND_ST_MARYS, ROAD);
    addConnection(BELGRADE, SZEGED, ROAD);
    addConnection(BERLIN, HAMBURG, ROAD);
    addConnection(BERLIN, LEIPZIG, ROAD);
    addConnection(BERLIN, PRAGUE, ROAD);
    addConnection(BORDEAUX, CLERMONT_FERRAND, ROAD);
    addConnection(BORDEAUX, NANTES, ROAD);
    addConnection(BORDEAUX, SARAGOSSA, ROAD);
    addConnection(BORDEAUX, TOULOUSE, ROAD);
    addConnection(BRUSSELS, COLOGNE, ROAD);
    addConnection(BRUSSELS, LE_HAVRE, ROAD);
    addConnection(BRUSSELS, PARIS, ROAD);
    addConnection(BRUSSELS, STRASBOURG, ROAD);
    addConnection(BUCHAREST, CONSTANTA, ROAD);
    addConnection(BUCHAREST, GALATZ, ROAD);
    addConnection(BUCHAREST, KLAUSENBURG, ROAD);
    addConnection(BUCHAREST, SOFIA, ROAD);
    addConnection(BUDAPEST, KLAUSENBURG, ROAD);
    addConnection(BUDAPEST, SZEGED, ROAD);
    addConnection(BUDAPEST, VIENNA, ROAD);
    addConnection(BUDAPEST, ZAGREB, ROAD);
    addConnection(CADIZ, GRANADA, ROAD);
    addConnection(CADIZ, LISBON, ROAD);
    addConnection(CADIZ, MADRID, ROAD);
    addConnection(CASTLE_DRACULA, GALATZ, ROAD);
    addConnection(CASTLE_DRACULA, KLAUSENBURG, ROAD);
    addConnection(CLERMONT_FERRAND, GENEVA, ROAD);
    addConnection(CLERMONT_FERRAND, MARSEILLES, ROAD);
    addConnection(CLERMONT_FERRAND, NANTES, ROAD);
    addConnection(CLERMONT_FERRAND, PARIS, ROAD);
    addConnection(CLERMONT_FERRAND, TOULOUSE, ROAD);
    addConnection(COLOGNE, FRANKFURT, ROAD);
    addConnection(COLOGNE, HAMBURG, ROAD);
    addConnection(COLOGNE, LEIPZIG, ROAD);
    addConnection(COLOGNE, STRASBOURG, ROAD);
    addConnection(CONSTANTA, GALATZ, ROAD);
    addConnection(CONSTANTA, VARNA, ROAD);
    addConnection(DUBLIN, GALWAY, ROAD);
    addConnection(EDINBURGH, MANCHESTER, ROAD);
    addConnection(FLORENCE, GENOA, ROAD);
    addConnection(FLORENCE, ROME, ROAD);
    addConnection(FLORENCE, VENICE, ROAD);
    addConnection(FRANKFURT, LEIPZIG, ROAD);
    addConnection(FRANKFURT, NUREMBURG, ROAD);
    addConnection(FRANKFURT, STRASBOURG, ROAD);
    addConnection(GALATZ, KLAUSENBURG, ROAD);
    addConnection(GENEVA, MARSEILLES, ROAD);
    addConnection(GENEVA, PARIS, ROAD);
    addConnection(GENEVA, STRASBOURG, ROAD);
    addConnection(GENEVA, ZURICH, ROAD);
    addConnection(GENOA, MARSEILLES, ROAD);
    addConnection(GENOA, MILAN, ROAD);
    addConnection(GENOA, VENICE, ROAD);
    addConnection(GRANADA, MADRID, ROAD);
    addConnection(HAMBURG, LEIPZIG, ROAD);
    addConnection(KLAUSENBURG, SZEGED, ROAD);
    addConnection(LEIPZIG, NUREMBURG, ROAD);
    addConnection(LE_HAVRE, NANTES, ROAD);
    addConnection(LE_HAVRE, PARIS, ROAD);
    addConnection(LISBON, MADRID, ROAD);
    addConnection(LISBON, SANTANDER, ROAD);
    addConnection(LIVERPOOL, MANCHESTER, ROAD);
    addConnection(LIVERPOOL, SWANSEA, ROAD);
    addConnection(LONDON, MANCHESTER, ROAD);
    addConnection(LONDON, PLYMOUTH, ROAD);
    addConnection(LONDON, SWANSEA, ROAD);
    addConnection(MADRID, SANTANDER, ROAD);
    addConnection(MADRID, SARAGOSSA, ROAD);
    addConnection(MARSEILLES, MILAN, ROAD);
    addConnection(MARSEILLES, TOULOUSE, ROAD);
    addConnection(MARSEILLES, ZURICH, ROAD);
    addConnection(MILAN, MUNICH, ROAD);
    addConnection(MILAN, VENICE, ROAD);
    addConnection(MILAN, ZURICH, ROAD);
    addConnection(MUNICH, NUREMBURG, ROAD);
    addConnection(MUNICH, STRASBOURG, ROAD);
    addConnection(MUNICH, VENICE, ROAD);
    addConnection(MUNICH, VIENNA, ROAD);
    addConnection(MUNICH, ZAGREB, ROAD);
    addConnection(MUNICH, ZURICH, ROAD);
    addConnection(NANTES, PARIS, ROAD);
    addConnection(NAPLES, ROME, ROAD);
    addConnection(NUREMBURG, PRAGUE, ROAD);
    addConnection(NUREMBURG, STRASBOURG, ROAD);
    addConnection(PARIS, STRASBOURG, ROAD);
    addConnection(PRAGUE, VIENNA, ROAD);
    addConnection(SALONICA, SOFIA, ROAD);
    addConnection(SALONICA, VALONA, ROAD);
    addConnection(SANTANDER, SARAGOSSA, ROAD);
    addConnection(SARAGOSSA, TOULOUSE, ROAD);
    addConnection(SARAJEVO, SOFIA, ROAD);
    addConnection(SARAJEVO, ST_JOSEPH_AND_ST_MARYS, ROAD);
    addConnection(SARAJEVO, VALONA, ROAD);
    addConnection(SARAJEVO, ZAGREB, ROAD);
    addConnection(SOFIA, VALONA, ROAD);
    addConnection(SOFIA, VARNA, ROAD);
    addConnection(STRASBOURG, ZURICH, ROAD);
    addConnection(ST_JOSEPH_AND_ST_MARYS, SZEGED, ROAD);
    addConnection(ST_JOSEPH_AND_ST_MARYS, ZAGREB, ROAD);
    addConnection(SZEGED, ZAGREB, ROAD);
    addConnection(VIENNA, ZAGREB, ROAD);

    // Rail connections
    addConnection(ALICANTE, BARCELONA, RAIL);
    addConnection(ALICANTE, MADRID, RAIL);
    addConnection(BARCELONA, SARAGOSSA, RAIL);
    addConnection(BARI, NAPLES, RAIL);
    addConnection(BELGRADE, SOFIA, RAIL);
    addConnection(BELGRADE, SZEGED, RAIL);
    addConnection(BERLIN, HAMBURG, RAIL);
    addConnection(BERLIN, LEIPZIG, RAIL);
    addConnection(BERLIN, PRAGUE, RAIL);
    addConnection(BORDEAUX, PARIS, RAIL);
    addConnection(BORDEAUX, SARAGOSSA, RAIL);
    addConnection(BRUSSELS, COLOGNE, RAIL);
    addConnection(BRUSSELS, PARIS, RAIL);
    addConnection(BUCHAREST, CONSTANTA, RAIL);
    addConnection(BUCHAREST, GALATZ, RAIL);
    addConnection(BUCHAREST, SZEGED, RAIL);
    addConnection(BUDAPEST, SZEGED, RAIL);
    addConnection(BUDAPEST, VIENNA, RAIL);
    addConnection(COLOGNE, FRANKFURT, RAIL);
    addConnection(EDINBURGH, MANCHESTER, RAIL);
    addConnection(FLORENCE, MILAN, RAIL);
    addConnection(FLORENCE, ROME, RAIL);
    addConnection(FRANKFURT, LEIPZIG, RAIL);
    addConnection(FRANKFURT, STRASBOURG, RAIL);
    addConnection(GENEVA, MILAN, RAIL);
    addConnection(GENOA, MILAN, RAIL);
    addConnection(LEIPZIG, NUREMBURG, RAIL);
    addConnection(LE_HAVRE, PARIS, RAIL);
    addConnection(LISBON, MADRID, RAIL);
    addConnection(LIVERPOOL, MANCHESTER, RAIL);
    addConnection(LONDON, MANCHESTER, RAIL);
    addConnection(LONDON, SWANSEA, RAIL);
    addConnection(MADRID, SANTANDER, RAIL);
    addConnection(MADRID, SARAGOSSA, RAIL);
    addConnection(MARSEILLES, PARIS, RAIL);
    addConnection(MILAN, ZURICH, RAIL);
    addConnection(MUNICH, NUREMBURG, RAIL);
    addConnection(NAPLES, ROME, RAIL);
    addConnection(PRAGUE, VIENNA, RAIL);
    addConnection(SALONICA, SOFIA, RAIL);
    addConnection(SOFIA, VARNA, RAIL);
    addConnection(STRASBOURG, ZURICH, RAIL);
    addConnection(VENICE, VIENNA, RAIL);

    // Boat connections
    addConnection(ADRIATIC_SEA, BARI, BOAT);
    addConnection(ADRIATIC_SEA, IONIAN_SEA, BOAT);
    addConnection(ADRIATIC_SEA, VENICE, BOAT);
    addConnection(ALICANTE, MEDITERRANEAN_SEA, BOAT);
    addConnection(AMSTERDAM, NORTH_SEA, BOAT);
    addConnection(ATHENS, IONIAN_SEA, BOAT);
    addConnection(ATLANTIC_OCEAN, BAY_OF_BISCAY, BOAT);
    addConnection(ATLANTIC_OCEAN, CADIZ, BOAT);
    addConnection(ATLANTIC_OCEAN, ENGLISH_CHANNEL, BOAT);
    addConnection(ATLANTIC_OCEAN, GALWAY, BOAT);
    addConnection(ATLANTIC_OCEAN, IRISH_SEA, BOAT);
    addConnection(ATLANTIC_OCEAN, LISBON, BOAT);
    addConnection(ATLANTIC_OCEAN, MEDITERRANEAN_SEA, BOAT);
    addConnection(ATLANTIC_OCEAN, NORTH_SEA, BOAT);
    addConnection(BARCELONA, MEDITERRANEAN_SEA, BOAT);
    addConnection(BAY_OF_BISCAY, BORDEAUX, BOAT);
    addConnection(BAY_OF_BISCAY, NANTES, BOAT);
    addConnection(BAY_OF_BISCAY, SANTANDER, BOAT);
    addConnection(BLACK_SEA, CONSTANTA, BOAT);
    addConnection(BLACK_SEA, IONIAN_SEA, BOAT);
    addConnection(BLACK_SEA, VARNA, BOAT);
    addConnection(CAGLIARI, MEDITERRANEAN_SEA, BOAT);
    addConnection(CAGLIARI, TYRRHENIAN_SEA, BOAT);
    addConnection(DUBLIN, IRISH_SEA, BOAT);
    addConnection(EDINBURGH, NORTH_SEA, BOAT);
    addConnection(ENGLISH_CHANNEL, LE_HAVRE, BOAT);
    addConnection(ENGLISH_CHANNEL, LONDON, BOAT);
    addConnection(ENGLISH_CHANNEL, NORTH_SEA, BOAT);
    addConnection(ENGLISH_CHANNEL, PLYMOUTH, BOAT);
    addConnection(GENOA, TYRRHENIAN_SEA, BOAT);
    addConnection(HAMBURG, NORTH_SEA, BOAT);
    addConnection(IONIAN_SEA, SALONICA, BOAT);
    addConnection(IONIAN_SEA, TYRRHENIAN_SEA, BOAT);
    addConnection(IONIAN_SEA, VALONA, BOAT);
    addConnection(IRISH_SEA, LIVERPOOL, BOAT);
    addConnection(IRISH_SEA, SWANSEA, BOAT);
    addConnection(MARSEILLES, MEDITERRANEAN_SEA, BOAT);
    addConnection(MEDITERRANEAN_SEA, TYRRHENIAN_SEA, BOAT);
    addConnection(NAPLES, TYRRHENIAN_SEA, BOAT);
    addConnection(ROME, TYRRHENIAN_SEA, BOAT);

    isInited = true;
    atexit(deinitMap);
}

const MapEdge *getEdgesOf(LocationID from) {
    initMap();

    MapEdge *result = nodes[from].edges;
    if (!result)
        return &emptyEdge;
    return result;
}
