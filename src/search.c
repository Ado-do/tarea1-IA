#include "search.h"

#include "maze.h"

#include <assert.h>
#include <raylib.h>
#include <stdlib.h>
#include <string.h>

int GetNodeIndex(const SearchData *sd, int x, int y)
{
    assert(x >= 0 && x < sd->maze->cols);
    assert(y >= 0 && y < sd->maze->rows);
    int index = y * sd->maze->cols + x;
    return index;
}

static Node *CreateNode(int x, int y, int cost)
{
    Node *n = malloc(sizeof(Node));
    if (n == NULL)
        TraceLog(LOG_FATAL, "CreateNode: couldn't allocate node");

    n->x = x;
    n->y = y;
    n->next = NULL;
    n->parent = NULL;
    n->cost = cost;
    n->priority = cost;
    return n;
}

SearchData *CreateSearchData(Maze *m)
{
    SearchData *sd = malloc(sizeof(SearchData));
    if (sd == NULL) {
        TraceLog(LOG_FATAL, "CreateSearchData: couldn't allocate sd");
    }
    *sd = (SearchData){
        .maze = m,
        .type = DFS,
        .state = NOT_SEARCHING,
        .totalNodes = m->rows * m->cols,
        .checked = calloc(m->rows * m->cols, sizeof(bool)),
        .visited = calloc(m->rows * m->cols, sizeof(bool)),
        .adjLists = calloc(m->rows * m->cols, sizeof(Node *)),
        .containerList = CreateNode(m->start.x, m->start.y, 0),
        .processedList = NULL,
        .currentNode = CreateNode(m->start.x, m->start.y, 0),
        .inFinalPath = calloc(m->rows * m->cols, sizeof(bool)),
        .finalPathLength = 0,
        .stepCount = 0,
        .timer = 0.0f,
        .delay = DEFAULT_DELAY,
        .maxJumpValue = 0,
    };

    sd->checked[GetNodeIndex(sd, m->start.x, m->start.y)] = true;
    sd->visited[GetNodeIndex(sd, m->start.x, m->start.y)] = true;

    if (!sd->checked || !sd->adjLists || !sd->containerList || !sd->inFinalPath) {
        TraceLog(LOG_FATAL, "CreateSearchData error");
    }

    for (unsigned k = 0; k < sd->totalNodes; k++) {
        int row = k / m->cols;
        int col = k % m->cols;
        int jumpLength = m->grid[row][col];

        if (jumpLength > sd->maxJumpValue)
            sd->maxJumpValue = jumpLength;

        int jumps[4][2] = {
            {col, row - jumpLength}, // up
            {col, row + jumpLength}, // down
            {col + jumpLength, row}, // right
            {col - jumpLength, row}, // left
        };

        for (int i = 0; i < 4; i++) {
            int x = jumps[i][0], y = jumps[i][1];
            if (IsValidPosition(m, x, y)) {
                Node *new = CreateNode(x, y, 0);
                new->next = sd->adjLists[k];
                sd->adjLists[k] = new;
            }
        }
    }
    return sd;
}

static void DeleteNodeList(Node *list)
{
    if (list == NULL)
        return;

    Node *current = list;
    Node *next;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
}

void DeleteSearchData(SearchData *sd)
{
    if (!sd)
        return;

    free(sd->checked);
    free(sd->visited);

    for (unsigned i = 0; i < sd->totalNodes; i++) {
        DeleteNodeList(sd->adjLists[i]);
    }
    free(sd->adjLists);

    DeleteNodeList(sd->containerList);

    DeleteNodeList(sd->processedList);

    if (sd->currentNode) {
        free(sd->currentNode);
    }

    free(sd->inFinalPath);

    free(sd);
}

void ResetSearchData(SearchData *sd)
{
    Position start = sd->maze->start;

    memset(sd->checked, false, sd->totalNodes * sizeof(bool));
    sd->checked[GetNodeIndex(sd, start.x, start.y)] = true;

    memset(sd->visited, false, sd->totalNodes * sizeof(bool));
    sd->visited[GetNodeIndex(sd, start.x, start.y)] = true;

    memset(sd->inFinalPath, false, sd->totalNodes * sizeof(bool));

    DeleteNodeList(sd->containerList);
    sd->containerList = CreateNode(start.x, start.y, sd->maze->grid[start.y][start.x]);

    DeleteNodeList(sd->processedList);
    sd->processedList = NULL;

    if (sd->currentNode != NULL) {
        free(sd->currentNode);
    }
    sd->currentNode = CreateNode(start.x, start.y, 0);

    sd->finalPathLength = sd->stepCount = 0;

    sd->timer = 0.0f;

    sd->state = NOT_SEARCHING;
}

static void PushToStack(Node **container, Node *node)
{
    node->next = *container;
    *container = node;
}

static void PushToQueue(SearchData *sd, Node *node)
{
    Node **container = &sd->containerList;
    node->next = NULL;

    if (*container == NULL) {
        *container = node;
        return;
    }

    Node *current = *container;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = node;
}

static void PushToPriorityQueue(SearchData *sd, Node *node)
{
    Node **container = &sd->containerList;

    if (sd->type == A_START) {
        if (*container == NULL || node->priority <= (*container)->priority) {
            node->next = *container;
            *container = node;
            return;
        }
    } else {
        if (*container == NULL || node->cost <= (*container)->cost) {
            node->next = *container;
            *container = node;
            return;
        }
    }

    Node *current = *container;
    if (sd->type == A_START) {
        while (current->next != NULL && current->next->priority < node->priority) {
            current = current->next;
        }
    } else {
        while (current->next != NULL && current->next->cost < node->cost) {
            current = current->next;
        }
    }

    node->next = current->next;
    current->next = node;
}

static Node *RemoveFromContainer(SearchData *sd)
{
    Node **container = &sd->containerList;
    if (*container == NULL)
        return NULL;

    Node *node = *container;
    *container = node->next;
    node->next = NULL;
    return node;
}

static bool CommonSearchStep(SearchData *sd, Node *current)
{
    // Store current node for visualization
    if (sd->currentNode != NULL)
        free(sd->currentNode);
    sd->currentNode = CreateNode(current->x, current->y, 0);
    sd->currentNode->parent = current->parent;
    sd->currentNode->cost = current->cost;

    // Mark current like visited
    int i_current = GetNodeIndex(sd, current->x, current->y);
    sd->visited[i_current] = true;
    sd->stepCount++;

    // Check if is goal
    if (current->x == sd->maze->goal.x && current->y == sd->maze->goal.y) {
        sd->state = FOUND;
        SetupFinalPath(sd);

        current->next = sd->processedList;
        sd->processedList = current;

        TraceLog(LOG_DEBUG, "Goal found (%d, %d), finalPathLength = %u", current->x, current->y, sd->finalPathLength);

        return true;
    }

    return false;
}

bool ManualStep(SearchData *sd, Direction dir)
{
    if (sd->state == FOUND || sd->state == NOT_FOUND) {
        return false;
    }

    Position current_pos = {sd->currentNode->x, sd->currentNode->y};
    int jump_length = sd->maze->grid[current_pos.y][current_pos.x];
    Position newPos = current_pos;

    switch (dir) {
        case DIR_UP: newPos.y -= jump_length; break;
        case DIR_DOWN: newPos.y += jump_length; break;
        case DIR_RIGHT: newPos.x += jump_length; break;
        case DIR_LEFT: newPos.x -= jump_length; break;
        default: return false;
    }

    if (!IsValidPosition(sd->maze, newPos.x, newPos.y)) {
        return false;
    }

    sd->currentNode->x = newPos.x;
    sd->currentNode->y = newPos.y;
    sd->finalPathLength++;
    sd->stepCount++;

    unsigned index = GetNodeIndex(sd, newPos.x, newPos.y);
    sd->checked[index] = true;
    sd->visited[index] = true;

    TraceLog(LOG_DEBUG, "ManualStep: current node (%d, %d) in step %d", newPos.x, newPos.y, sd->finalPathLength);

    if (newPos.x == sd->maze->goal.x && newPos.y == sd->maze->goal.y) {
        TraceLog(LOG_DEBUG, "Goal found (%d, %d), finalPathLength = %u", newPos.x, newPos.y, sd->finalPathLength);
        sd->state = FOUND;
        return true;
    }

    return true;
}

static bool DFSStep(SearchData *sd)
{
    // Alias (pointer) for RemoveFromContainer in DFS
    Node *(*const PopStack)(SearchData *sd) = RemoveFromContainer;

    if (sd->state == FOUND || sd->state == NOT_FOUND) {
        TraceLog(LOG_DEBUG, "DFSStep: returning true, FOUND or NOT_FOUND checked");
        return true;
    }

    Node *current = PopStack(sd);
    if (current == NULL) {
        TraceLog(LOG_DEBUG, "DFSStep: returning true, current is NULL");
        sd->state = NOT_FOUND;
        return true;
    }

    TraceLog(LOG_DEBUG, "DFSStep: current node (%d, %d) in step %d", current->x, current->y, sd->stepCount);

    if (CommonSearchStep(sd, current)) {
        return true;
    }

    // Add adjacent nodes to container (stack)
    int i_current = GetNodeIndex(sd, current->x, current->y);
    Node *adjList = sd->adjLists[i_current];

    while (adjList != NULL) {
        int i_adjNode = GetNodeIndex(sd, adjList->x, adjList->y);
        if (!sd->checked[i_adjNode]) {
            sd->checked[i_adjNode] = true;
            Node *nextNode = CreateNode(adjList->x, adjList->y, 0);
            nextNode->parent = current;
            PushToStack(&sd->containerList, nextNode);
        }
        adjList = adjList->next;
    }

    // Track processed nodes
    current->next = sd->processedList;
    sd->processedList = current;

    TraceLog(LOG_DEBUG, "DFSStep: returning false, DFS in progress");
    return false;
}

bool BFSStep(SearchData *sd)
{
    // Alias (pointer) for RemoveFromContainer in BFS
    Node *(*PopQueue)(SearchData *sd) = RemoveFromContainer;

    if (sd->state == FOUND || sd->state == NOT_FOUND) {
        TraceLog(LOG_DEBUG, "BFSStep: returning true, FOUND or NOT_FOUND checked");
        return true;
    }

    Node *current = PopQueue(sd);
    if (current == NULL) {
        TraceLog(LOG_DEBUG, "BFSStep: returning true, current is NULL");
        sd->state = NOT_FOUND;
        return true;
    }

    TraceLog(LOG_DEBUG, "BFSStep: current node (%d, %d) in step %d", current->x, current->y, sd->stepCount);

    if (CommonSearchStep(sd, current)) {
        return true;
    }

    int i_current = GetNodeIndex(sd, current->x, current->y);
    Node *adjList = sd->adjLists[i_current];

    while (adjList != NULL) {
        int i_adjNode = GetNodeIndex(sd, adjList->x, adjList->y);
        if (!sd->checked[i_adjNode]) {
            sd->checked[i_adjNode] = true;
            Node *nextNode = CreateNode(adjList->x, adjList->y, 0);
            nextNode->parent = current;
            PushToQueue(sd, nextNode);
        }
        adjList = adjList->next;
    }

    current->next = sd->processedList;
    sd->processedList = current;

    TraceLog(LOG_DEBUG, "BFSStep: returning false, BFS in progress");
    return false;
}

static bool UCSStep(SearchData *sd)
{
    // Alias (pointer) for RemoveFromContainer in UCS
    Node *(*DequeuePriorityQueue)(SearchData *sd) = RemoveFromContainer;

    if (sd->state == FOUND || sd->state == NOT_FOUND) {
        TraceLog(LOG_DEBUG, "UCStep: returning true, FOUND or NOT_FOUND checked");
        return true;
    }

    Node *current = DequeuePriorityQueue(sd);
    if (current == NULL) {
        sd->state = NOT_FOUND;
        return true;
    }

    TraceLog(LOG_DEBUG, "UCSStep: current node (%d, %d) with cost %d in step %d", current->x, current->y, current->cost, sd->stepCount);

    // Check
    if (CommonSearchStep(sd, current)) {
        return true;
    }

    // Add adjacent nodes to container (cost priority queue)
    int i_current = GetNodeIndex(sd, current->x, current->y);
    Node *adjList = sd->adjLists[i_current];

    while (adjList != NULL) {
        int i_adjNode = GetNodeIndex(sd, adjList->x, adjList->y);
        if (!sd->checked[i_adjNode]) {
            sd->checked[i_adjNode] = true;
            Node *nextNode = CreateNode(adjList->x, adjList->y, current->cost + 1);
            nextNode->parent = current;
            PushToPriorityQueue(sd, nextNode);
        }
        adjList = adjList->next;
    }

    current->next = sd->processedList;
    sd->processedList = current;

    return false;
}

float Heuristic(SearchData *sd, int x, int y)
{
    Position goal = sd->maze->goal;
    return (abs(x - goal.x) + abs(y - goal.y)) / (float)sd->maxJumpValue;
}

static bool AStarStep(SearchData *sd)
{
    Node *(*DequeuePriorityQueue)(SearchData *sd) = RemoveFromContainer;

    if (sd->state == FOUND || sd->state == NOT_FOUND) {
        return true;
    }

    Node *current = DequeuePriorityQueue(sd);
    if (current == NULL) {
        sd->state = NOT_FOUND;
        return true;
    }

    if (CommonSearchStep(sd, current)) {
        return true;
    }

    int i_current = GetNodeIndex(sd, current->x, current->y);
    Node *adjList = sd->adjLists[i_current];

    while (adjList != NULL) {
        int i_adjNode = GetNodeIndex(sd, adjList->x, adjList->y);
        if (!sd->checked[i_adjNode]) {
            sd->checked[i_adjNode] = true;
            int new_cost = current->cost + 1;
            float h = Heuristic(sd, adjList->x, adjList->y);
            float priority = new_cost + h;
            Node *nextNode = CreateNode(adjList->x, adjList->y, new_cost);
            nextNode->priority = priority;
            nextNode->parent = current;

            PushToPriorityQueue(sd, nextNode);
        }
        adjList = adjList->next;
    }

    current->next = sd->processedList;
    sd->processedList = current;

    return false;
}

bool SearchStep(SearchData *sd, float vel)
{
    sd->timer += GetFrameTime();
    if (sd->timer < sd->delay / vel)
        return true;

    sd->timer = 0;

    switch (sd->type) {
        case MANUAL: return false;
        case DFS: return DFSStep(sd);
        case BFS: return BFSStep(sd);
        case UCS: return UCSStep(sd);
        case A_START: return AStarStep(sd);
    }

    return false;
}

void SetupFinalPath(SearchData *sd)
{
    if (sd->state != FOUND || sd->currentNode == NULL) {
        TraceLog(LOG_WARNING, "GetFinalPath: bad function call");
        return;
    }

    memset(sd->inFinalPath, false, sd->totalNodes * sizeof(bool));
    sd->finalPathLength = 0;

    Node *current = sd->currentNode;
    while (current != NULL) {
        unsigned index = GetNodeIndex(sd, current->x, current->y);
        assert(index < sd->totalNodes);

        sd->inFinalPath[index] = true;
        sd->finalPathLength++;
        current = current->parent;
    }
    sd->finalPathLength--; // don't count the start
}

bool IsCurrentNode(const SearchData *sd, int x, int y)
{
    if (sd->currentNode == NULL)
        return false;
    return (sd->currentNode->x == x && sd->currentNode->y == y);
}

bool IsNodeVisited(const SearchData *sd, int x, int y)
{
    int index = GetNodeIndex(sd, x, y);
    return sd->visited[index];
}

bool IsNodeInFinalPath(const SearchData *sd, int x, int y)
{
    int index = GetNodeIndex(sd, x, y);
    return sd->inFinalPath[index];
}

const char *GetSearchTypeText(SearchType type)
{
    switch (type) {
        case MANUAL: return "Manual";
        case DFS: return "DFS";
        case BFS: return "BFS";
        case UCS: return "UCS";
        case A_START: return "A*";
        default: return "UNKNOWN";
    }
}

const char *GetSearchStateText(SearchState state)
{
    switch (state) {
        case NOT_SEARCHING: return "Not Searching";
        case SEARCHING: return "Searching";
        case FOUND: return "Found";
        case NOT_FOUND: return "Not Found";
        default: return "UNKNOWN";
    }
}
