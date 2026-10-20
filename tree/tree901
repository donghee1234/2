#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define V 10
#define E 20
#define MAX_W 100
#define INF 1000000000

typedef struct { int u, v, w; } Edge;

Edge edges[E];
int adj[V][V];   /* 인접 행렬 (0 = 간선 없음, 가중치는 양수) */

/* ---------- 그래프 생성 ---------- */
void addEdge(int k, int a, int b) {
    int w = 1 + rand() % MAX_W;
    edges[k].u = a < b ? a : b;     /* 항상 u < v 로 저장 */
    edges[k].v = a < b ? b : a;
    edges[k].w = w;
    adj[a][b] = w;
    adj[b][a] = w;
}

int cmpEdge(const void* x, const void* y) {
    const Edge* a = (const Edge*)x;
    const Edge* b = (const Edge*)y;
    if (a->u != b->u) return a->u - b->u;
    return a->v - b->v;
}

/* 무작위 신장 트리(9개 간선)로 연결을 보장한 뒤, 나머지 11개 간선을 중복 없이 추가한다. */
void generateGraph(void) {
    int perm[V];
    int k = 0;

    for (int i = 0; i < V; i++) perm[i] = i;
    for (int i = V - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = perm[i];
        perm[i] = perm[j];
        perm[j] = tmp;
    }

    for (int i = 1; i < V; i++) {
        addEdge(k++, perm[i], perm[rand() % i]);
    }

    while (k < E) {
        int a = rand() % V;
        int b = rand() % V;
        if (a == b || adj[a][b] > 0) continue;
        addEdge(k++, a, b);
    }

    qsort(edges, E, sizeof(Edge), cmpEdge);
}

/* ---------- Dijkstra: src에서 모든 정점까지의 최단거리와 이전 정점 ---------- */
void dijkstra(int src, int dist[], int prev[]) {
    int visited[V] = {0};

    for (int i = 0; i < V; i++) {
        dist[i] = INF;
        prev[i] = -1;
    }
    dist[src] = 0;

    for (int iter = 0; iter < V; iter++) {
        int u = -1;
        for (int i = 0; i < V; i++) {
            if (!visited[i] && (u == -1 || dist[i] < dist[u])) u = i;
        }
        visited[u] = 1;

        for (int v = 0; v < V; v++) {
            if (adj[u][v] > 0 && !visited[v] && dist[u] + adj[u][v] < dist[v]) {
                dist[v] = dist[u] + adj[u][v];
                prev[v] = u;
            }
        }
    }
}

/* prev 배열을 거꾸로 따라가서 s -> t 경로를 복원한다. 경로의 정점 수를 반환 */
int buildPath(int s, int t, int prev[V][V], int path[]) {
    int rev[V];
    int len = 0;
    for (int x = t; x != -1; x = prev[s][x]) {
        rev[len++] = x;
    }
    for (int i = 0; i < len; i++) {
        path[i] = rev[len - 1 - i];
    }
    return len;
}

/* ---------- 검증용 함수 ---------- */
int isConnected(void) {
    int visited[V] = {0};
    int stack[V];
    int top = 0, count = 0;

    visited[0] = 1;
    stack[top++] = 0;
    while (top > 0) {
        int u = stack[--top];
        count++;
        for (int v = 0; v < V; v++) {
            if (adj[u][v] > 0 && !visited[v]) {
                visited[v] = 1;
                stack[top++] = v;
            }
        }
    }
    return count == V;
}

int hasNoSelfLoopOrDuplicate(void) {
    for (int i = 0; i < E; i++) {
        if (edges[i].u == edges[i].v) return 0;
        for (int j = i + 1; j < E; j++) {
            if (edges[i].u == edges[j].u && edges[i].v == edges[j].v) return 0;
        }
    }
    for (int i = 0; i < V; i++) {
        if (adj[i][i] != 0) return 0;
    }
    return 1;
}

int weightsAreValid(void) {
    for (int i = 0; i < E; i++) {
        if (edges[i].w < 1 || edges[i].w > MAX_W) return 0;
        if (adj[edges[i].u][edges[i].v] != edges[i].w) return 0;
        if (adj[edges[i].v][edges[i].u] != edges[i].w) return 0;
    }
    return 1;
}

int countAdjEdges(void) {
    int count = 0;
    for (int i = 0; i < V; i++) {
        for (int j = i + 1; j < V; j++) {
            if (adj[i][j] > 0) count++;
        }
    }
    return count;
}

/* Floyd-Warshall로 따로 계산한 거리 */
void floydWarshall(int d[V][V]) {
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            if (i == j) d[i][j] = 0;
            else if (adj[i][j] > 0) d[i][j] = adj[i][j];
            else d[i][j] = INF;
        }
    }
    for (int k = 0; k < V; k++) {
        for (int i = 0; i < V; i++) {
            for (int j = 0; j < V; j++) {
                if (d[i][k] + d[k][j] < d[i][j]) d[i][j] = d[i][k] + d[k][j];
            }
        }
    }
}

int main(int argc, char* argv[]) {
    unsigned int seed = (argc > 1) ? (unsigned int)atoi(argv[1]) : (unsigned int)time(NULL);
    srand(seed);
    printf("Seed : %u\n\n", seed);

    generateGraph();

    /* ----- 생성된 그래프 출력 ----- */
    printf("Graph\n");
    printf("Vertices : %d\n", V);
    printf("Edges    : %d\n\n", E);
    printf("Edges\n");
    for (int i = 0; i < E; i++) {
        printf("(%d, %d) Weight: %d\n", edges[i].u, edges[i].v, edges[i].w);
    }

    /* ----- 모든 정점을 출발점으로 Dijkstra 수행 ----- */
    int dist[V][V], prev[V][V];
    for (int s = 0; s < V; s++) {
        dijkstra(s, dist[s], prev[s]);
    }

    /* ----- 45개 정점 쌍(u < v)의 최단경로 출력 ----- */
    printf("\nShortest Paths\n");

    int pairCount = 0;
    int pathsOk = 1;       /* 경로가 출발~도착 정점으로 이어지고 실제 간선으로만 구성되는가 */
    int distanceOk = 1;    /* 최단거리 == 경로에 포함된 간선 가중치의 합 */

    for (int u = 0; u < V; u++) {
        for (int v = u + 1; v < V; v++) {
            int path[V];
            int len = buildPath(u, v, prev, path);

            printf("\n%d -> %d\n", u, v);
            printf("Path     : ");
            for (int i = 0; i < len; i++) {
                if (i > 0) printf(" -> ");
                printf("%d", path[i]);
            }
            printf("\nDistance : %d\n", dist[u][v]);
            pairCount++;

            if (path[0] != u || path[len - 1] != v) pathsOk = 0;
            int sum = 0;
            for (int i = 0; i + 1 < len; i++) {
                int w = adj[path[i]][path[i + 1]];
                if (w == 0) pathsOk = 0;
                sum += w;
            }
            if (sum != dist[u][v]) distanceOk = 0;
        }
    }
    printf("\nTotal vertex pairs : %d\n", pairCount);

    /* ----- 확인 사항 ----- */
    int fw[V][V];
    floydWarshall(fw);
    int sameAsFloyd = 1;
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            if (fw[i][j] != dist[i][j]) sameAsFloyd = 0;
        }
    }

    printf("\nVerification\n");
    printf("Vertices are exactly 10               : %s\n", V == 10 ? "OK" : "FAIL");
    printf("Edges are exactly 20                  : %s\n", (countAdjEdges() == 20) ? "OK" : "FAIL");
    printf("All weights are in 1 ~ 100            : %s\n", weightsAreValid() ? "OK" : "FAIL");
    printf("No self-loop or duplicate edge        : %s\n", hasNoSelfLoopOrDuplicate() ? "OK" : "FAIL");
    printf("Graph is connected                    : %s\n", isConnected() ? "OK" : "FAIL");
    printf("All 45 vertex pairs are printed       : %s\n", pairCount == 45 ? "OK" : "FAIL");
    printf("Actual path is printed for every pair : %s\n", pathsOk ? "OK" : "FAIL");
    printf("Distance == sum of path edge weights  : %s\n", distanceOk ? "OK" : "FAIL");
    printf("Dijkstra == Floyd-Warshall distances  : %s\n", sameAsFloyd ? "OK" : "FAIL");

    return 0;
}
