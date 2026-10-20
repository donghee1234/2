#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define V 50
#define SPARSE_E 100
#define DENSE_E 600
#define MAX_W 100
#define REPEAT 10000
#define INF 1000000000

typedef struct { int u, v, w; } Edge;

/* ---------- 인접 리스트 ---------- */
typedef struct Node {
    int vertex;
    int weight;
    struct Node *next;
} Node;

typedef struct {
    int n;
    int edgeCount;
    Node *head[V];
} ListGraph;

/* ---------- 인접 행렬 (0 = 간선 없음, 가중치는 양수) ---------- */
typedef struct {
    int n;
    int matrix[V][V];
} MatrixGraph;

/* ---------- Union-Find ---------- */
typedef struct {
    int parent[V];
    int rank[V];
} UnionFind;

typedef struct {
    Edge mst[V - 1];
    int mstCount;
    long cost;
    size_t memory;
    double timeUs;
} Result;

/* ===== 난수 (플랫폼이 달라도 같은 시드면 같은 그래프가 나오도록 직접 구현) ===== */
static unsigned long long rngState;

unsigned int nextRand(void) {
    rngState = rngState * 6364136223846793005ULL + 1442695040888963407ULL;
    return (unsigned int)(rngState >> 33);
}

int randInt(int n) {
    return (int)(nextRand() % (unsigned int)n);
}

/* ===== 그래프 생성 ===== */
/* 무작위 신장 트리(V-1개 간선)로 연결을 보장한 뒤, 나머지 간선을 중복 없이 추가한다. */
void generateEdges(Edge edges[], int edgeCount) {
    static char used[V][V];
    int perm[V];
    int k = 0;

    memset(used, 0, sizeof(used));
    for (int i = 0; i < V; i++) perm[i] = i;
    for (int i = V - 1; i > 0; i--) {
        int j = randInt(i + 1);
        int tmp = perm[i];
        perm[i] = perm[j];
        perm[j] = tmp;
    }

    for (int i = 1; i < V; i++) {
        int u = perm[i];
        int v = perm[randInt(i)];
        used[u][v] = used[v][u] = 1;
        edges[k].u = u;
        edges[k].v = v;
        edges[k].w = 1 + randInt(MAX_W);
        k++;
    }

    while (k < edgeCount) {
        int u = randInt(V);
        int v = randInt(V);
        if (u == v || used[u][v]) continue;
        used[u][v] = used[v][u] = 1;
        edges[k].u = u;
        edges[k].v = v;
        edges[k].w = 1 + randInt(MAX_W);
        k++;
    }
}

ListGraph* buildListGraph(Edge edges[], int edgeCount) {
    ListGraph* g = (ListGraph*)calloc(1, sizeof(ListGraph));
    g->n = V;
    g->edgeCount = edgeCount;
    for (int i = 0; i < edgeCount; i++) {
        Node* a = (Node*)malloc(sizeof(Node));
        a->vertex = edges[i].v;
        a->weight = edges[i].w;
        a->next = g->head[edges[i].u];
        g->head[edges[i].u] = a;

        Node* b = (Node*)malloc(sizeof(Node));
        b->vertex = edges[i].u;
        b->weight = edges[i].w;
        b->next = g->head[edges[i].v];
        g->head[edges[i].v] = b;
    }
    return g;
}

MatrixGraph* buildMatrixGraph(Edge edges[], int edgeCount) {
    MatrixGraph* g = (MatrixGraph*)calloc(1, sizeof(MatrixGraph));
    g->n = V;
    for (int i = 0; i < edgeCount; i++) {
        g->matrix[edges[i].u][edges[i].v] = edges[i].w;
        g->matrix[edges[i].v][edges[i].u] = edges[i].w;
    }
    return g;
}

void freeListGraph(ListGraph* g) {
    for (int u = 0; u < V; u++) {
        Node* p = g->head[u];
        while (p != NULL) {
            Node* next = p->next;
            free(p);
            p = next;
        }
    }
    free(g);
}

/* ===== 그래프 검증 ===== */
int isConnected(MatrixGraph* g) {
    int visited[V] = {0};
    int queue[V];
    int front = 0, rear = 0, count = 0;

    visited[0] = 1;
    queue[rear++] = 0;
    while (front < rear) {
        int u = queue[front++];
        count++;
        for (int v = 0; v < V; v++) {
            if (g->matrix[u][v] > 0 && !visited[v]) {
                visited[v] = 1;
                queue[rear++] = v;
            }
        }
    }
    return count == V;
}

int verifySameGraph(ListGraph* l, MatrixGraph* m) {
    for (int u = 0; u < V; u++) {
        int rowCount = 0;
        for (int v = 0; v < V; v++) {
            if (m->matrix[u][v] > 0) rowCount++;
        }

        int mark[V] = {0};
        int listCount = 0;
        for (Node* p = l->head[u]; p != NULL; p = p->next) {
            if (mark[p->vertex] || m->matrix[u][p->vertex] != p->weight) return 0;
            mark[p->vertex] = 1;
            listCount++;
        }
        if (rowCount != listCount) return 0;
    }
    return 1;
}

/* ===== Union-Find ===== */
void ufInit(UnionFind* uf) {
    for (int i = 0; i < V; i++) {
        uf->parent[i] = i;
        uf->rank[i] = 0;
    }
}

int ufFind(UnionFind* uf, int x) {
    while (uf->parent[x] != x) {
        uf->parent[x] = uf->parent[uf->parent[x]];
        x = uf->parent[x];
    }
    return x;
}

/* 합쳐졌으면 1, 이미 같은 집합(사이클 발생)이면 0 */
int ufUnion(UnionFind* uf, int a, int b) {
    int ra = ufFind(uf, a);
    int rb = ufFind(uf, b);
    if (ra == rb) return 0;
    if (uf->rank[ra] < uf->rank[rb]) {
        uf->parent[ra] = rb;
    } else if (uf->rank[ra] > uf->rank[rb]) {
        uf->parent[rb] = ra;
    } else {
        uf->parent[rb] = ra;
        uf->rank[ra]++;
    }
    return 1;
}

/* ===== Kruskal (인접 리스트) ===== */
/* 가중치 오름차순, 같으면 (u, v) 순으로 비교해서 플랫폼과 상관없이 결과가 같게 한다. */
int cmpEdge(const void* a, const void* b) {
    const Edge* x = (const Edge*)a;
    const Edge* y = (const Edge*)b;
    if (x->w != y->w) return x->w - y->w;
    if (x->u != y->u) return x->u - y->u;
    return x->v - y->v;
}

void kruskal(ListGraph* g, Edge mst[], int* mstCount, long* cost) {
    /* 1. 인접 리스트에서 간선을 한 번씩만 꺼내 배열에 모은다. (u < v 인 경우만) */
    Edge* edges = (Edge*)malloc(g->edgeCount * sizeof(Edge));
    int m = 0;
    for (int u = 0; u < g->n; u++) {
        for (Node* p = g->head[u]; p != NULL; p = p->next) {
            if (u < p->vertex) {
                edges[m].u = u;
                edges[m].v = p->vertex;
                edges[m].w = p->weight;
                m++;
            }
        }
    }

    /* 2. 가중치 순으로 정렬 */
    qsort(edges, m, sizeof(Edge), cmpEdge);

    /* 3. 작은 간선부터 사이클이 생기지 않으면 MST에 추가 */
    UnionFind uf;
    ufInit(&uf);
    *mstCount = 0;
    *cost = 0;
    for (int i = 0; i < m && *mstCount < V - 1; i++) {
        if (ufUnion(&uf, edges[i].u, edges[i].v)) {
            mst[*mstCount] = edges[i];
            (*mstCount)++;
            *cost += edges[i].w;
        }
    }
    free(edges);
}

/* ===== Prim (인접 행렬) ===== */
void prim(MatrixGraph* g, Edge mst[], int* mstCount, long* cost) {
    int visited[V];
    int key[V];      /* MST까지 연결하는 최소 비용 */
    int parent[V];   /* 그 간선의 반대쪽 정점 */

    for (int i = 0; i < V; i++) {
        visited[i] = 0;
        key[i] = INF;
        parent[i] = -1;
    }
    key[0] = 0;
    *mstCount = 0;
    *cost = 0;

    for (int iter = 0; iter < V; iter++) {
        int u = -1;
        for (int i = 0; i < V; i++) {
            if (!visited[i] && (u == -1 || key[i] < key[u])) u = i;
        }
        visited[u] = 1;

        if (parent[u] != -1) {
            mst[*mstCount].u = parent[u];
            mst[*mstCount].v = u;
            mst[*mstCount].w = key[u];
            (*mstCount)++;
            *cost += key[u];
        }

        for (int v = 0; v < V; v++) {
            if (g->matrix[u][v] > 0 && !visited[v] && g->matrix[u][v] < key[v]) {
                key[v] = g->matrix[u][v];
                parent[v] = u;
            }
        }
    }
}

/* ===== 메모리 계산 (sizeof 사용) ===== */
size_t listGraphBytes(ListGraph* g) {
    return sizeof(ListGraph) + (size_t)(2 * g->edgeCount) * sizeof(Node);
}

size_t matrixGraphBytes(void) {
    return sizeof(MatrixGraph);
}

/* Kruskal 보조 공간: 간선 배열 + Union-Find + MST 결과 간선 */
size_t kruskalAuxBytes(int edgeCount) {
    return (size_t)edgeCount * sizeof(Edge) + sizeof(UnionFind) + (V - 1) * sizeof(Edge);
}

/* Prim 보조 공간: visited, key, parent 배열 + MST 결과 간선 */
size_t primAuxBytes(void) {
    return 3 * V * sizeof(int) + (V - 1) * sizeof(Edge);
}

/* ===== 측정 ===== */
/* 그래프 생성이 끝난 뒤 MST 계산 시작부터 끝까지의 시간만 측정한다.
 * 실행 시간이 너무 짧아 한 번으로는 측정이 어려우므로 REPEAT번 반복한 평균을 사용한다. */
Result runKruskal(ListGraph* g) {
    Result r;
    memset(&r, 0, sizeof(r));

    clock_t start = clock();
    for (int i = 0; i < REPEAT; i++) {
        kruskal(g, r.mst, &r.mstCount, &r.cost);
    }
    clock_t end = clock();

    r.timeUs = (double)(end - start) * 1e6 / CLOCKS_PER_SEC / REPEAT;
    r.memory = listGraphBytes(g) + kruskalAuxBytes(g->edgeCount);
    return r;
}

Result runPrim(MatrixGraph* g) {
    Result r;
    memset(&r, 0, sizeof(r));

    clock_t start = clock();
    for (int i = 0; i < REPEAT; i++) {
        prim(g, r.mst, &r.mstCount, &r.cost);
    }
    clock_t end = clock();

    r.timeUs = (double)(end - start) * 1e6 / CLOCKS_PER_SEC / REPEAT;
    r.memory = matrixGraphBytes() + primAuxBytes();
    return r;
}

/* MST 검증: 간선 49개, 실제 그래프의 간선과 가중치 일치, 사이클 없음, 비용 합 일치 */
int verifyMST(MatrixGraph* g, Result* r) {
    if (r->mstCount != V - 1) return 0;

    UnionFind uf;
    ufInit(&uf);
    long sum = 0;
    for (int i = 0; i < r->mstCount; i++) {
        Edge e = r->mst[i];
        if (g->matrix[e.u][e.v] != e.w) return 0;
        if (!ufUnion(&uf, e.u, e.v)) return 0;
        sum += e.w;
    }
    return sum == r->cost;
}

/* ===== 출력 ===== */
void printCase(int no, const char* graphName, const char* repName, const char* algName,
               int edgeCount, Result* r) {
    printf("Case %d : %s - %s - %s\n", no, graphName, repName, algName);
    printf("%-14s: %d\n", "Vertices", V);
    printf("%-14s: %d\n", "Edges", edgeCount);
    printf("%-14s: %d\n", "MST Edges", r->mstCount);
    printf("%-14s: %ld\n", "MST Cost", r->cost);
    printf("%-14s: %zu Bytes\n", "Memory", r->memory);
    printf("%-14s: %.3f us (avg of %d runs)\n", "Execution Time", r->timeUs, REPEAT);

    printf("MST Edge List :\n");
    for (int i = 0; i < r->mstCount; i++) {
        if (i % 7 == 0) printf("  ");
        printf("(%d-%d:%d) ", r->mst[i].u, r->mst[i].v, r->mst[i].w);
        if (i % 7 == 6 || i == r->mstCount - 1) printf("\n");
    }
    printf("\n");
}

int main(int argc, char* argv[]) {
    unsigned int seed = (argc > 1) ? (unsigned int)atoi(argv[1]) : (unsigned int)time(NULL);
    rngState = seed;

    printf("Seed : %u\n", seed);
    printf("Execution time is the average of %d runs (clock()).\n\n", REPEAT);

    /* 희소/밀집 그래프를 한 번씩만 만들고, 같은 간선 목록으로 리스트와 행렬을 만든다. */
    static Edge sparseEdges[SPARSE_E];
    static Edge denseEdges[DENSE_E];
    generateEdges(sparseEdges, SPARSE_E);
    generateEdges(denseEdges, DENSE_E);

    ListGraph* sparseList = buildListGraph(sparseEdges, SPARSE_E);
    ListGraph* denseList = buildListGraph(denseEdges, DENSE_E);
    MatrixGraph* sparseMatrix = buildMatrixGraph(sparseEdges, SPARSE_E);
    MatrixGraph* denseMatrix = buildMatrixGraph(denseEdges, DENSE_E);

    int connSparse = isConnected(sparseMatrix);
    int connDense = isConnected(denseMatrix);
    int sameSparse = verifySameGraph(sparseList, sparseMatrix);
    int sameDense = verifySameGraph(denseList, denseMatrix);

    Result c1 = runKruskal(sparseList);
    Result c2 = runKruskal(denseList);
    Result c3 = runPrim(sparseMatrix);
    Result c4 = runPrim(denseMatrix);

    printCase(1, "Sparse Graph", "Adjacency List", "Kruskal", SPARSE_E, &c1);
    printCase(2, "Dense Graph", "Adjacency List", "Kruskal", DENSE_E, &c2);
    printCase(3, "Sparse Graph", "Adjacency Matrix", "Prim", SPARSE_E, &c3);
    printCase(4, "Dense Graph", "Adjacency Matrix", "Prim", DENSE_E, &c4);

    printf("===== Verification =====\n");
    printf("Connected graph (Sparse / Dense)        : %s / %s\n",
           connSparse ? "OK" : "FAIL", connDense ? "OK" : "FAIL");
    printf("Same graph, list vs matrix (Sparse / Dense) : %s / %s\n",
           sameSparse ? "OK" : "FAIL", sameDense ? "OK" : "FAIL");
    printf("MST validity (Case 1 / 2 / 3 / 4)       : %s / %s / %s / %s\n",
           verifyMST(sparseMatrix, &c1) ? "OK" : "FAIL",
           verifyMST(denseMatrix, &c2) ? "OK" : "FAIL",
           verifyMST(sparseMatrix, &c3) ? "OK" : "FAIL",
           verifyMST(denseMatrix, &c4) ? "OK" : "FAIL");
    printf("Kruskal cost == Prim cost (Sparse / Dense)  : %s / %s\n",
           c1.cost == c3.cost ? "OK" : "FAIL", c2.cost == c4.cost ? "OK" : "FAIL");

    freeListGraph(sparseList);
    freeListGraph(denseList);
    free(sparseMatrix);
    free(denseMatrix);
    return 0;
}
