#pragma once

#include "maze.h"
#include "player.h"

#include <stdbool.h>

#define DEFAULT_DELAY 0.25f

typedef enum { DFS = 0, BFS, UCS, A_START, MANUAL } SearchType;
typedef enum { NOT_SEARCHING, SEARCHING, FOUND, NOT_FOUND } SearchState;

typedef struct Node {
    int x, y;
    int cost;
    int priority;
    struct Node *next;
    struct Node *parent;
} Node;

typedef struct {
    const Maze *maze;
    SearchType type;
    SearchState state;
    unsigned totalNodes;
    Node **adjLists;
    bool *checked;
    bool *visited;
    Node *containerList;
    Node *processedList;
    Node *currentNode;
    float timer;
    float delay;
    bool *inFinalPath;
    int stepCount;
    int finalPathLength;
    int maxJumpValue;
} SearchData;

SearchData *CreateSearchData(Maze *m);
void DeleteSearchData(SearchData *sd);
void ResetSearchData(SearchData *sd);

bool SearchStep(SearchData *sd, float vel);
bool BFSStep(SearchData *sd);
bool ManualStep(SearchData *sd, Direction dir);

void SetupFinalPath(SearchData *sd);
bool IsCurrentNode(const SearchData *sd, int x, int y);
bool IsNodeVisited(const SearchData *sd, int x, int y);
bool IsNodeInFinalPath(const SearchData *sd, int x, int y);

float VelocityToDelay(float velocity);

const char *GetSearchTypeText(SearchType type);
const char *GetSearchStateText(SearchState state);
