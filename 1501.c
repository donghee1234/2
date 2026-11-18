#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif

#define SIZE_COUNT 6
#define MAX_N 20000
#define MAX_OPS (MAX_N / 5)
#ifndef REPS
#define REPS 20                 /* 실행시간은 같은 과정을 REPS번 반복한 평균 */
#endif
#define DEFAULT_RANGE 1000001   /* 0 ~ 1,000,000 */

/* ---------- 측정용 전역 변수 ---------- */
static long long g_cmp;         /* 데이터 값 비교 횟수 */
static long long g_mov;         /* 데이터 값 복사/대입 횟수 */
static long long g_nodeCount, g_peakNodes;

/* 재귀 호출이 사용한 스택 공간(Byte)을 실제 주소 차이로 측정한다. */
static uintptr_t g_stackBase;
static size_t g_stackPeak;
#define TRACK_STACK() do { \
        char marker_; \
        uintptr_t addr_ = (uintptr_t)&marker_; \
        size_t used_ = (size_t)(g_stackBase > addr_ ? g_stackBase - addr_ : addr_ - g_stackBase); \
        if (used_ > g_stackPeak) g_stackPeak = used_; \
    } while (0)

/* ---------- 시간 측정 (단위: ms) ---------- */
#ifdef _WIN32
double nowMs(void) {
    LARGE_INTEGER freq, count;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&count);
    return (double)count.QuadPart * 1000.0 / (double)freq.QuadPart;
}
#else
double nowMs(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1e6;
}
#endif

/* ---------- 난수 (같은 시드면 어느 환경에서나 같은 데이터가 나온다) ---------- */
static unsigned long long rngState;

unsigned int nextRand(void) {
    rngState = rngState * 6364136223846793005ULL + 1442695040888963407ULL;
    return (unsigned int)(rngState >> 33);
}

int randInt(int n) {
    return (int)(nextRand() % (unsigned int)n);
}

/* =====================================================================
 *  1. 퀵 정렬 (과제-12의 Median-of-Three 피벗 + Lomuto 분할)
 * ===================================================================== */
/* 두 데이터를 임시 변수로 교환: 이동 3회 (같은 위치끼리는 교환하지 않는다) */
void swapInt(int* x, int* y) {
    if (x == y) return;
    int temp = *x;  g_mov++;
    *x = *y;        g_mov++;
    *y = temp;      g_mov++;
}

/* 첫 번째, 가운데, 마지막 원소의 중앙값 위치. 중앙값을 찾기 위한 값 비교도 비교 횟수에 포함한다. */
int medianOfThreeIndex(const int a[], int lo, int hi) {
    int mid = lo + (hi - lo) / 2;

    g_cmp++;
    if (a[lo] < a[mid]) {
        g_cmp++;
        if (a[mid] < a[hi]) return mid;
        g_cmp++;
        return (a[lo] < a[hi]) ? hi : lo;
    } else {
        g_cmp++;
        if (a[lo] < a[hi]) return lo;
        g_cmp++;
        return (a[mid] < a[hi]) ? hi : mid;
    }
}

int partition(int a[], int lo, int hi, int pivotIndex) {
    swapInt(&a[pivotIndex], &a[hi]);      /* 피벗을 맨 뒤로. 비교는 a[hi]를 직접 읽으므로 피벗 값 복사는 없다. */
    int i = lo - 1;
    for (int j = lo; j < hi; j++) {
        g_cmp++;
        if (a[j] < a[hi]) {
            i++;
            swapInt(&a[i], &a[j]);
        }
    }
    swapInt(&a[i + 1], &a[hi]);
    return i + 1;
}

void quickSort(int a[], int lo, int hi) {
    TRACK_STACK();
    if (lo >= hi) return;
    int p = (hi - lo + 1 < 3) ? lo : medianOfThreeIndex(a, lo, hi);
    int m = partition(a, lo, hi, p);
    quickSort(a, lo, m - 1);
    quickSort(a, m + 1, hi);
}

/* ---------- 정렬된 배열에서의 삽입/삭제 (선형 탐색 + 이동) ----------
 * 비교 횟수는 "확인한 원소 수"이다. (값 하나를 한 번 비교해서 <, ==, > 를 모두 판단) */
void arrayInsert(int a[], int* size, int value) {
    int pos = 0;
    while (pos < *size) {
        g_cmp++;
        if (a[pos] > value) break;            /* 처음으로 value보다 큰 원소 앞에 삽입 */
        pos++;
    }
    for (int k = *size; k > pos; k--) {       /* 뒤로 한 칸씩 이동 */
        a[k] = a[k - 1];
        g_mov++;
    }
    a[pos] = value;
    g_mov++;
    (*size)++;
}

int arrayDelete(int a[], int* size, int value) {
    int pos = 0;
    int found = 0;
    while (pos < *size) {
        g_cmp++;
        if (a[pos] >= value) {                /* 정렬되어 있으므로 여기서 멈춰도 된다 */
            found = (a[pos] == value);
            break;
        }
        pos++;
    }
    if (!found) return 0;

    for (int k = pos; k < *size - 1; k++) {   /* 앞으로 한 칸씩 이동 */
        a[k] = a[k + 1];
        g_mov++;
    }
    (*size)--;
    return 1;
}

/* =====================================================================
 *  2. AVL 트리 (중복 값은 노드의 count에 저장)
 * ===================================================================== */
typedef struct AVLNode {
    int data;
    int count;                 /* 같은 값의 개수 */
    struct AVLNode *left;
    struct AVLNode *right;
    int height;                /* 노드 수 기준 높이 (단말 = 1) */
} AVLNode;

int heightOf(const AVLNode* n) { return n ? n->height : 0; }
int maxInt(int a, int b) { return a > b ? a : b; }

void updateHeight(AVLNode* n) {
    n->height = 1 + maxInt(heightOf(n->left), heightOf(n->right));
}

AVLNode* rotateRight(AVLNode* y) {
    AVLNode* x = y->left;
    y->left = x->right;
    x->right = y;
    updateHeight(y);
    updateHeight(x);
    return x;
}

AVLNode* rotateLeft(AVLNode* x) {
    AVLNode* y = x->right;
    x->right = y->left;
    y->left = x;
    updateHeight(x);
    updateHeight(y);
    return y;
}

/* 높이 갱신 후 균형이 깨졌으면 회전 (균형 인수 비교는 데이터 값 비교가 아니므로 세지 않는다) */
AVLNode* rebalance(AVLNode* n) {
    updateHeight(n);
    int balance = heightOf(n->left) - heightOf(n->right);

    if (balance > 1) {
        if (heightOf(n->left->left) < heightOf(n->left->right)) n->left = rotateLeft(n->left);   /* LR */
        return rotateRight(n);                                                                   /* LL */
    }
    if (balance < -1) {
        if (heightOf(n->right->right) < heightOf(n->right->left)) n->right = rotateRight(n->right); /* RL */
        return rotateLeft(n);                                                                    /* RR */
    }
    return n;
}

AVLNode* avlInsert(AVLNode* node, int value) {
    TRACK_STACK();
    if (node == NULL) {
        AVLNode* created = (AVLNode*)malloc(sizeof(AVLNode));
        created->data = value;  g_mov++;           /* 노드에 데이터 값을 저장 */
        created->count = 1;
        created->left = created->right = NULL;
        created->height = 1;
        g_nodeCount++;
        if (g_nodeCount > g_peakNodes) g_peakNodes = g_nodeCount;
        return created;
    }

    g_cmp++;
    if (value == node->data) {
        node->count++;                              /* 중복: 개수만 증가 (구조 변화 없음) */
        return node;
    }
    if (value < node->data) node->left = avlInsert(node->left, value);
    else node->right = avlInsert(node->right, value);
    return rebalance(node);
}

/* 서브트리에서 가장 작은 노드를 떼어낸다. (왼쪽 포인터만 따라가므로 값 비교는 없다) */
AVLNode* avlRemoveMin(AVLNode* node, int* minData, int* minCount) {
    TRACK_STACK();
    if (node->left == NULL) {
        AVLNode* right = node->right;
        *minData = node->data;
        *minCount = node->count;
        free(node);
        g_nodeCount--;
        return right;
    }
    node->left = avlRemoveMin(node->left, minData, minCount);
    return rebalance(node);
}

/* value 한 개를 삭제한다. 없으면 아무것도 하지 않고 *removed = 0 */
AVLNode* avlDelete(AVLNode* node, int value, int* removed) {
    TRACK_STACK();
    if (node == NULL) {
        *removed = 0;
        return NULL;
    }

    g_cmp++;
    if (value < node->data) {
        node->left = avlDelete(node->left, value, removed);
    } else if (value > node->data) {
        node->right = avlDelete(node->right, value, removed);
    } else {
        *removed = 1;
        if (node->count > 1) {                      /* 중복이면 한 개만 제거 */
            node->count--;
            return node;
        }
        if (node->left == NULL || node->right == NULL) {
            AVLNode* child = node->left ? node->left : node->right;
            free(node);
            g_nodeCount--;
            return child;
        }
        /* 자식이 둘: 오른쪽 서브트리의 최솟값 노드의 데이터를 복사해 오고, 그 노드를 제거 */
        int minData, minCount;
        node->right = avlRemoveMin(node->right, &minData, &minCount);
        node->data = minData;  g_mov++;
        node->count = minCount;
    }
    return rebalance(node);
}

/* ---------- 검증용 (측정에 포함하지 않음) ---------- */
void inorderCollect(const AVLNode* n, int out[], int* idx) {
    if (n == NULL) return;
    inorderCollect(n->left, out, idx);
    for (int c = 0; c < n->count; c++) out[(*idx)++] = n->data;
    inorderCollect(n->right, out, idx);
}

/* BST 성질, 높이, 균형(|균형 인수| <= 1)을 확인하고 높이를 반환. 문제가 있으면 -1 */
int verifyAVL(const AVLNode* n, int lo, int hi) {
    if (n == NULL) return 0;
    if (n->data < lo || n->data > hi || n->count < 1) return -1;
    int hl = verifyAVL(n->left, lo, n->data - 1);
    int hr = verifyAVL(n->right, n->data + 1, hi);
    if (hl < 0 || hr < 0) return -1;
    if (hl - hr > 1 || hr - hl > 1) return -1;
    if (n->height != 1 + maxInt(hl, hr)) return -1;
    return 1 + maxInt(hl, hr);
}

void freeTree(AVLNode* n) {
    if (n == NULL) return;
    freeTree(n->left);
    freeTree(n->right);
    free(n);
}

/* =====================================================================
 *  3. 전체 과정(정렬 + 삽입/삭제) 측정
 * ===================================================================== */
typedef struct { int isInsert; int value; } Op;

typedef struct {
    double timeMs;
    long long cmp, mov;
    size_t memory;
    int finalCount;
    double sortMs;              /* 정렬(트리 구성) 단계만의 시간/비교/이동 (분석용) */
    long long sortCmp, sortMov;
} Metrics;

static int quickFinal[MAX_N + MAX_OPS + 8];
static int avlFinal[MAX_N + MAX_OPS + 8];
static int avlValid;

Metrics runQuickProcess(const int original[], int n, const Op ops[], int opCount) {
    static int work[MAX_N + MAX_OPS + 8];
    Metrics m;
    memset(&m, 0, sizeof(m));
    double totalMs = 0, sortMsTotal = 0;
    int size = 0;

    for (int rep = 0; rep < REPS; rep++) {
        memcpy(work, original, sizeof(int) * n);         /* 배열 복사는 측정 제외 */
        size = n;
        g_cmp = 0;
        g_mov = 0;
        g_stackPeak = 0;
        char base;
        g_stackBase = (uintptr_t)&base;

        double t0 = nowMs();
        quickSort(work, 0, n - 1);
        double tSort = nowMs();
        m.sortCmp = g_cmp;
        m.sortMov = g_mov;
        for (int i = 0; i < opCount; i++) {
            if (ops[i].isInsert) arrayInsert(work, &size, ops[i].value);
            else arrayDelete(work, &size, ops[i].value);
        }
        double t1 = nowMs();
        totalMs += t1 - t0;
        sortMsTotal += tSort - t0;
    }

    m.timeMs = totalMs / REPS;
    m.sortMs = sortMsTotal / REPS;
    m.cmp = g_cmp;
    m.mov = g_mov;
    /* 배열 칸 수는 (처음 n + 삽입 가능한 최대 횟수)만큼 미리 확보해야 하므로 그 크기로 계산 */
    m.memory = (size_t)(n + opCount) * sizeof(int) + g_stackPeak;
    m.finalCount = size;
    memcpy(quickFinal, work, sizeof(int) * size);
    return m;
}

Metrics runAvlProcess(const int original[], int n, const Op ops[], int opCount) {
    Metrics m;
    memset(&m, 0, sizeof(m));
    double totalMs = 0, sortMsTotal = 0;
    int total = 0;

    for (int rep = 0; rep < REPS; rep++) {
        AVLNode* root = NULL;
        g_cmp = 0;
        g_mov = 0;
        g_nodeCount = 0;
        g_peakNodes = 0;
        g_stackPeak = 0;
        char base;
        g_stackBase = (uintptr_t)&base;

        double t0 = nowMs();
        for (int i = 0; i < n; i++) root = avlInsert(root, original[i]);   /* AVL 트리 정렬 */
        double tSort = nowMs();
        m.sortCmp = g_cmp;
        m.sortMov = g_mov;
        for (int i = 0; i < opCount; i++) {
            if (ops[i].isInsert) {
                root = avlInsert(root, ops[i].value);
            } else {
                int removed;
                root = avlDelete(root, ops[i].value, &removed);
            }
        }
        double t1 = nowMs();
        totalMs += t1 - t0;
        sortMsTotal += tSort - t0;

        if (rep == REPS - 1) {                    /* 마지막 반복의 결과로 검증 (측정 제외) */
            avlValid = (verifyAVL(root, -2147483647 - 1, 2147483647) >= 0);
            total = 0;
            inorderCollect(root, avlFinal, &total);
        }
        freeTree(root);                           /* 메모리 해제도 측정 제외 */
    }

    m.timeMs = totalMs / REPS;
    m.sortMs = sortMsTotal / REPS;
    m.cmp = g_cmp;
    m.mov = g_mov;
    m.memory = (size_t)g_peakNodes * sizeof(AVLNode) + g_stackPeak;
    m.finalCount = total;
    return m;
}

int cmpIntAsc(const void* x, const void* y) {
    int a = *(const int*)x;
    int b = *(const int*)y;
    return (a > b) - (a < b);
}

void failIfBad(int ok, const char* what, int n) {
    if (!ok) {
        printf("ERROR: %s (n=%d)\n", what, n);
        exit(1);
    }
}

int main(int argc, char* argv[]) {
    unsigned int seed = (argc > 1) ? (unsigned int)atoi(argv[1]) : 12345u;
    int dense = (argc > 2 && strcmp(argv[2], "dense") == 0);
    rngState = seed;

    int sizes[SIZE_COUNT] = { 1000, 2000, 4000, 8000, 16000, 20000 };
    Metrics quick[SIZE_COUNT], avl[SIZE_COUNT];

    static int original[MAX_N], reference[MAX_N], test[MAX_N];
    static Op ops[MAX_OPS];

    printf("Seed : %u\n", seed);
    if (dense) printf("Mode : dense (values 0 ~ 2n-1, so deletions often find their target)\n");
    printf("Quick sort pivot : Median-of-Three\n\n");

    for (int s = 0; s < SIZE_COUNT; s++) {
        int n = sizes[s];
        int opCount = n / 5;                              /* 처음 데이터 크기의 20% */
        int range = dense ? 2 * n : DEFAULT_RANGE;

        /* 원본 데이터와 연산 목록 생성 (측정 제외). 두 방법이 똑같이 사용한다. */
        for (int i = 0; i < n; i++) original[i] = randInt(range);
        int insertCount = 0, deleteCount = 0;
        for (int i = 0; i < opCount; i++) {
            ops[i].isInsert = (randInt(range) % 2 == 0);  /* 임의의 정수가 짝수면 삽입, 홀수면 삭제 */
            ops[i].value = randInt(range);                /* 실제 삽입/삭제 값은 새로 생성 */
            if (ops[i].isInsert) insertCount++; else deleteCount++;
        }

        /* 퀵 정렬 자체 검증: 라이브러리 정렬 결과와 같은지 (측정 제외) */
        memcpy(reference, original, sizeof(int) * n);
        qsort(reference, n, sizeof(int), cmpIntAsc);
        memcpy(test, original, sizeof(int) * n);
        char base;
        g_stackBase = (uintptr_t)&base;
        quickSort(test, 0, n - 1);
        failIfBad(memcmp(test, reference, sizeof(int) * n) == 0, "quick sort result is wrong", n);

        quick[s] = runQuickProcess(original, n, ops, opCount);
        avl[s] = runAvlProcess(original, n, ops, opCount);

        /* 두 자료구조의 최종 데이터 구성이 같아야 한다 */
        failIfBad(quick[s].finalCount == avl[s].finalCount, "final counts differ", n);
        failIfBad(memcmp(quickFinal, avlFinal, sizeof(int) * quick[s].finalCount) == 0,
                  "final data differ between array and AVL tree", n);
        for (int i = 0; i + 1 < quick[s].finalCount; i++) {
            failIfBad(quickFinal[i] <= quickFinal[i + 1], "array is not sorted after operations", n);
        }
        failIfBad(avlValid, "AVL tree is not a valid balanced BST", n);

        int deleted = n + insertCount - quick[s].finalCount;

        char t1[32], t2[32], m1[32], m2[32];
        snprintf(t1, sizeof(t1), "%.3f ms", quick[s].timeMs);
        snprintf(t2, sizeof(t2), "%.3f ms", avl[s].timeMs);
        snprintf(m1, sizeof(m1), "%zu Bytes", quick[s].memory);
        snprintf(m2, sizeof(m2), "%zu Bytes", avl[s].memory);

        printf("Data Size : %d\n", n);
        printf("Operations: %d\n\n", opCount);
        printf("                %-15s %s\n", "Quick Sort", "AVL Tree");
        printf("------------------------------------------------\n");
        printf("%-15s %-15s %s\n", "Total Time", t1, t2);
        printf("%-15s %-15lld %lld\n", "Comparisons", quick[s].cmp, avl[s].cmp);
        printf("%-15s %-15lld %lld\n", "Data Movements", quick[s].mov, avl[s].mov);
        printf("%-15s %-15s %s\n\n", "Memory Usage", m1, m2);
        printf("%-17s %-15d %d\n", "Final Data Count", quick[s].finalCount, avl[s].finalCount);
        printf("(inserts %d, delete attempts %d -> actually deleted %d, skipped %d)\n",
               insertCount, deleteCount, deleted, deleteCount - deleted);
        printf("(sorting stage only) Quick Sort: %.3f ms, %lld comparisons, %lld movements\n",
               quick[s].sortMs, quick[s].sortCmp, quick[s].sortMov);
        printf("                     AVL Tree  : %.3f ms, %lld comparisons, %lld movements\n\n",
               avl[s].sortMs, avl[s].sortCmp, avl[s].sortMov);
    }

    printf("All checks passed: sorted result, identical final data, valid AVL tree\n\n");

    printf("[Ratio Quick Sort / AVL Tree]\n");
    printf("%-8s %-10s %-12s %-12s %-10s\n", "n", "Time", "Comparisons", "Movements", "Memory");
    for (int s = 0; s < SIZE_COUNT; s++) {
        printf("%-8d %-10.2f %-12.2f %-12.2f %-10.2f\n", sizes[s],
               quick[s].timeMs / avl[s].timeMs,
               (double)quick[s].cmp / avl[s].cmp,
               (double)quick[s].mov / avl[s].mov,
               (double)quick[s].memory / avl[s].memory);
    }

    return 0;
}
