// GameView.c ... GameView ADT implementation

#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include "Globals.h"
#include "Game.h"
#include "GameView.h"
#include "Map.h"

struct gameView {
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    int hello;
};


// Creates a new GameView to summarise the current state of the game
GameView newGameView(char *pastPlays, PlayerMessage messages[])
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    GameView gameView = malloc(sizeof(struct gameView));
    gameView->hello = 42;
    return gameView;
}


// Frees all memory previously allocated for the GameView toBeDeleted
void disposeGameView(GameView toBeDeleted)
{
    //COMPLETE THIS IMPLEMENTATION
    free( toBeDeleted );
}


//// Functions to return simple information about the current state of the game

// Get the current round
Round getRound(GameView currentView)
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    return 0;
}

// Get the id of current player - ie whose turn is it?
PlayerID getCurrentPlayer(GameView currentView)
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    return 0;
}

// Get the current score
int getScore(GameView currentView)
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    return 0;
}

// Get the current health points for a given player
int getHealth(GameView currentView, PlayerID player)
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    return 0;
}

// Get the current location id of a given player
LocationID getLocation(GameView currentView, PlayerID player)
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    return 0;
}

//// Functions that return information about the history of the game

// Fills the trail array with the location ids of the last 6 turns
void getHistory(GameView currentView, PlayerID player,
                            LocationID trail[TRAIL_SIZE])
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
}

//// Functions that query the map to find information about connectivity

static void fillConnectedLocations(bool *results, LocationID from, bool road, int rail, bool sea) {
    const MapEdge *edges = getEdgesOf(from);
    for (size_t e = 0; edges[e].next != NOWHERE; ++e) {
        if ((road && edges[e].method == ROAD) ||
            (rail && edges[e].method == RAIL) ||
            (sea && edges[e].method == BOAT)) {
            results[edges[e].next] = true;
        }

        if (rail && edges[e].method == RAIL)
            fillConnectedLocations(results, edges[e].next, false, rail - 1, false);
    }
}

// Returns an array of LocationIDs for all directly connected locations
LocationID *connectedLocations(GameView currentView, int *numLocations,
                               LocationID from, PlayerID player, Round round,
                               int road, int rail, int sea)
{
    (void)currentView; // Suppress compiler warnings

    if (rail) {
        if (player == PLAYER_DRACULA)
            rail = 0;
        else
            rail = (player + round) % 4;
    }

    bool connectedLocations[NUM_MAP_LOCATIONS] = {false};
    fillConnectedLocations(connectedLocations, from, road, rail, sea);

    *numLocations = 0;
    for (size_t l = 0; l < NUM_MAP_LOCATIONS; ++l)
        *numLocations += connectedLocations[l];

    LocationID *result = malloc(*numLocations * sizeof(LocationID));
    for (size_t l = 0, r = 0; l < NUM_MAP_LOCATIONS; ++l)
        if (connectedLocations[l])
            result[r++] = l;

    return result;
}
