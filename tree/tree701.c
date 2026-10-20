#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define N 100
#define SPARSE_EDGES 100
#define DENSE_EDGES 4000
#define OP_COUNT 20
#define CONN_COUNT 50
#define ADJ_COUNT 10

typedef struct { int u, v; } Pair;

/* ---------- 인접 행렬 ---------- */
typedef struct {
    int n;
    int matrix[N][N];
} MatrixGraph;

/* ---------- 인접 리스트 ---------- */
typedef struct Node {
    int vertex;
    struct Node *next;
} Node;

typedef struct {
    int n;
    Node *head[N];
} ListGraph;

typedef struct {
    long memory;
    long connectivity;
    long adjacent;
    long insertion;
    long deletion;
} Result;

/* ===== 인접 행렬 연산 ===== */
MatrixGraph* createMatrixGraph(void) {
    MatrixGraph* g = (MatrixGraph*)calloc(1, sizeof(MatrixGraph));
    g->n = N;
    return g;
}

void matrixBuildEdge(MatrixGraph* g, int u, int v) {
    g->matrix[u][v] = 1;
    g->matrix[v][u] = 1;
}

/* 존재 여부 확인: matrix[u][v] 1회 확인 */
long matrixCheckEdge(MatrixGraph* g, int u, int v, int* found) {
    *found = (g->matrix[u][v] == 1);
    return 1;
}

long matrixInsertEdge(MatrixGraph* g, int u, int v) {
    long comparisons = 1;
    if (g->matrix[u][v] == 0) {
        g->matrix[u][v] = 1;
        g->matrix[v][u] = 1;
    }
    return comparisons;
}

long matrixDeleteEdge(MatrixGraph* g, int u, int v) {
    long comparisons = 1;
    if (g->matrix[u][v] == 1) {
        g->matrix[u][v] = 0;
        g->matrix[v][u] = 0;
    }
    return comparisons;
}

/* u의 행 전체(matrix[u][0] ~ matrix[u][N-1])를 확인 */
long matrixAdjacent(MatrixGraph* g, int u, int out[], int* count) {
    long comparisons = 0;
    *count = 0;
    for (int j = 0; j < N; j++) {
        comparisons++;
        if (g->matrix[u][j] == 1) out[(*count)++] = j;
    }
    return comparisons;
}

/* ===== 인접 리스트 연산 ===== */
ListGraph* createListGraph(void) {
    ListGraph* g = (ListGraph*)calloc(1, sizeof(ListGraph));
    g->n = N;
    return g;
}

void listPushFront(ListGraph* g, int u, int v) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->vertex = v;
    node->next = g->head[u];
    g->head[u] = node;
}

void listBuildEdge(ListGraph* g, int u, int v) {
    listPushFront(g, u, v);
    listPushFront(g, v, u);
}

long listCountNodes(ListGraph* g) {
    long count = 0;
    for (int u = 0; u < N; u++) {
        for (Node* p = g->head[u]; p != NULL; p = p->next) count++;
    }
    return count;
}

/* u의 리스트에서 v를 찾을 때까지 확인한 노드 수 (못 찾으면 전체 노드 수) */
long listCheckEdge(ListGraph* g, int u, int v, int* found) {
    long comparisons = 0;
    *found = 0;
    for (Node* p = g->head[u]; p != NULL; p = p->next) {
        comparisons++;
        if (p->vertex == v) {
            *found = 1;
            break;
        }
    }
    return comparisons;
}

/* 삽입 위치(맨 앞)는 비교가 필요 없고, 중복 여부 확인을 위해 u의 리스트를 확인 */
long listInsertEdge(ListGraph* g, int u, int v) {
    int found;
    long comparisons = listCheckEdge(g, u, v, &found);
    if (!found) listBuildEdge(g, u, v);
    return comparisons;
}

/* u의 리스트에서 v 노드를 찾아 제거하고, 확인한 노드 수를 반환 */
long listRemove(ListGraph* g, int u, int v) {
    long comparisons = 0;
    Node* prev = NULL;
    Node* cur = g->head[u];
    while (cur != NULL) {
        comparisons++;
        if (cur->vertex == v) {
            if (prev == NULL) g->head[u] = cur->next;
            else prev->next = cur->next;
            free(cur);
            break;
        }
        prev = cur;
        cur = cur->next;
    }
    return comparisons;
}

/* 무방향이므로 u의 리스트와 v의 리스트에서 각각 제거 */
long listDeleteEdge(ListGraph* g, int u, int v) {
    long comparisons = listRemove(g, u, v);
    comparisons += listRemove(g, v, u);
    return comparisons;
}

long listAdjacent(ListGraph* g, int u, int out[], int* count) {
    long comparisons = 0;
    *count = 0;
    for (Node* p = g->head[u]; p != NULL; p = p->next) {
        comparisons++;
        out[(*count)++] = p->vertex;
    }
    return comparisons;
}

void freeListGraph(ListGraph* g) {
    for (int u = 0; u < N; u++) {
        Node* p = g->head[u];
        while (p != NULL) {
            Node* next = p->next;
            free(p);
            p = next;
        }
    }
    free(g);
}

/* ===== 두 표현이 같은 그래프인지 검증 ===== */
int verifySame(MatrixGraph* m, ListGraph* l) {
    for (int u = 0; u < N; u++) {
        int rowCount = 0;
        for (int v = 0; v < N; v++) rowCount += m->matrix[u][v];

        int mark[N] = {0};
        int listCount = 0;
        for (Node* p = l->head[u]; p != NULL; p = p->next) {
            if (mark[p->vertex] || m->matrix[u][p->vertex] != 1) return 0;
            mark[p->vertex] = 1;
            listCount++;
        }
        if (rowCount != listCount) return 0;
    }
    return 1;
}

/* ===== 그래프 / 측정 대상 생성 ===== */
void generateEdges(int edges[][2], int count, char used[N][N]) {
    memset(used, 0, N * N);
    int k = 0;
    while (k < count) {
        int u = rand() % N;
        int v = rand() % N;
        if (u == v || used[u][v]) continue;
        used[u][v] = used[v][u] = 1;
        edges[k][0] = u;
        edges[k][1] = v;
        k++;
    }
}

/* 현재 그래프에 없는 정점 쌍 OP_COUNT개 */
void pickInsertPairs(Pair pairs[], char used[N][N]) {
    char picked[N][N];
    memset(picked, 0, sizeof(picked));
    int k = 0;
    while (k < OP_COUNT) {
        int u = rand() % N;
        int v = rand() % N;
        if (u == v || used[u][v] || picked[u][v]) continue;
        picked[u][v] = picked[v][u] = 1;
        pairs[k].u = u;
        pairs[k].v = v;
        k++;
    }
}

/* 현재 그래프에 존재하는 간선 OP_COUNT개 */
void pickDeletePairs(Pair pairs[], int edges[][2], int edgeCount) {
    char chosen[DENSE_EDGES] = {0};
    int k = 0;
    while (k < OP_COUNT) {
        int idx = rand() % edgeCount;
        if (chosen[idx]) continue;
        chosen[idx] = 1;
        if (rand() % 2) {
            pairs[k].u = edges[idx][0];
            pairs[k].v = edges[idx][1];
        } else {
            pairs[k].u = edges[idx][1];
            pairs[k].v = edges[idx][0];
        }
        k++;
    }
}

/* ===== 측정 ===== */
Result measureMatrix(MatrixGraph* g, Pair ins[], Pair del[], Pair conn[], int adjV[]) {
    Result r = {0};
    int found, out[N], count;

    r.memory = sizeof(MatrixGraph);
    for (int i = 0; i < CONN_COUNT; i++) r.connectivity += matrixCheckEdge(g, conn[i].u, conn[i].v, &found);
    for (int i = 0; i < ADJ_COUNT; i++) r.adjacent += matrixAdjacent(g, adjV[i], out, &count);
    for (int i = 0; i < OP_COUNT; i++) r.insertion += matrixInsertEdge(g, ins[i].u, ins[i].v);
    for (int i = 0; i < OP_COUNT; i++) r.deletion += matrixDeleteEdge(g, del[i].u, del[i].v);
    return r;
}

Result measureList(ListGraph* g, Pair ins[], Pair del[], Pair conn[], int adjV[]) {
    Result r = {0};
    int found, out[N], count;

    r.memory = sizeof(ListGraph) + listCountNodes(g) * (long)sizeof(Node);
    for (int i = 0; i < CONN_COUNT; i++) r.connectivity += listCheckEdge(g, conn[i].u, conn[i].v, &found);
    for (int i = 0; i < ADJ_COUNT; i++) r.adjacent += listAdjacent(g, adjV[i], out, &count);
    for (int i = 0; i < OP_COUNT; i++) r.insertion += listInsertEdge(g, ins[i].u, ins[i].v);
    for (int i = 0; i < OP_COUNT; i++) r.deletion += listDeleteEdge(g, del[i].u, del[i].v);
    return r;
}

void printCase(int no, const char* graphName, const char* repName, Result r) {
    printf("Case %d : %s - %s\n", no, graphName, repName);
    printf("%-30s : %ld Bytes\n", "Memory", r.memory);
    printf("%-30s : %ld (avg %.2f)\n", "Edge insertion comparisons", r.insertion, (double)r.insertion / OP_COUNT);
    printf("%-30s : %ld (avg %.2f)\n", "Edge deletion comparisons", r.deletion, (double)r.deletion / OP_COUNT);
    printf("%-30s : %ld (avg %.2f)\n", "Connectivity check comparisons", r.connectivity, (double)r.connectivity / CONN_COUNT);
    printf("%-30s : %ld (avg %.2f)\n\n", "Adjacent vertices comparisons", r.adjacent, (double)r.adjacent / ADJ_COUNT);
}

void runGraph(const char* graphName, int edgeCount, int firstCase, Pair conn[], int adjV[]) {
    static int edges[DENSE_EDGES][2];
    static char used[N][N];
    Pair ins[OP_COUNT], del[OP_COUNT];

    generateEdges(edges, edgeCount, used);
    pickInsertPairs(ins, used);
    pickDeletePairs(del, edges, edgeCount);

    /* 같은 간선 목록으로 두 표현을 만든다 */
    MatrixGraph* m = createMatrixGraph();
    ListGraph* l = createListGraph();
    for (int i = 0; i < edgeCount; i++) {
        matrixBuildEdge(m, edges[i][0], edges[i][1]);
        listBuildEdge(l, edges[i][0], edges[i][1]);
    }

    printf("===== %s (Vertices : %d, Edges : %d) =====\n", graphName, N, edgeCount);
    printf("Same graph check (matrix vs list) : %s\n", verifySame(m, l) ? "OK" : "FAIL");

    int out[N], count;
    matrixAdjacent(m, adjV[0], out, &count);
    printf("Adjacent vertices of vertex %d (%d) :", adjV[0], count);
    for (int i = 0; i < count; i++) printf(" %d", out[i]);
    printf("\n\n");

    Result rm = measureMatrix(m, ins, del, conn, adjV);
    Result rl = measureList(l, ins, del, conn, adjV);

    printCase(firstCase, graphName, "Adjacency Matrix", rm);
    printCase(firstCase + 1, graphName, "Adjacency List", rl);

    printf("Same graph check after insert/delete : %s\n\n", verifySame(m, l) ? "OK" : "FAIL");

    free(m);
    freeListGraph(l);
}

int main(int argc, char* argv[]) {
    unsigned int seed = (argc > 1) ? (unsigned int)atoi(argv[1]) : (unsigned int)time(NULL);
    srand(seed);

    printf("Seed : %u\n", seed);
    printf("Operations : insertion %d, deletion %d, connectivity checks %d, adjacency queries %d\n\n",
           OP_COUNT, OP_COUNT, CONN_COUNT, ADJ_COUNT);

    /* 연결 여부 확인용 정점 쌍과 인접 정점 조회용 정점: 모든 경우에 동일하게 사용 */
    Pair conn[CONN_COUNT];
    for (int i = 0; i < CONN_COUNT; i++) {
        do {
            conn[i].u = rand() % N;
            conn[i].v = rand() % N;
        } while (conn[i].u == conn[i].v);
    }

    int adjV[ADJ_COUNT];
    char taken[N] = {0};
    for (int i = 0; i < ADJ_COUNT; ) {
        int v = rand() % N;
        if (taken[v]) continue;
        taken[v] = 1;
        adjV[i++] = v;
    }

    runGraph("Sparse Graph", SPARSE_EDGES, 1, conn, adjV);
    runGraph("Dense Graph", DENSE_EDGES, 3, conn, adjV);

    return 0;
}
