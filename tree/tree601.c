#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define GEN_COUNT 100
#define SEARCH_COUNT 50
#define MAX_VALUE 1000

/* ---------- BST ---------- */
typedef struct Node {
    int data;
    struct Node *left, *right;
} Node;

Node* createNode(int value) {
    Node* n = (Node*)malloc(sizeof(Node));
    n->data = value;
    n->left = n->right = NULL;
    return n;
}

/* 반환값 1=새로 삽입, 0=중복이라 삽입 안함. comparisons는 누적된다. */
int bstInsertOrSkip(Node** root, int value, long* comparisons) {
    if (*root == NULL) {
        *root = createNode(value);
        return 1;
    }
    Node* current = *root;
    while (1) {
        (*comparisons)++;
        if (value == current->data) {
            return 0;
        } else if (value < current->data) {
            if (current->left == NULL) { current->left = createNode(value); return 1; }
            current = current->left;
        } else {
            if (current->right == NULL) { current->right = createNode(value); return 1; }
            current = current->right;
        }
    }
}

int bstSearch(Node* root, int key, long* comparisons) {
    Node* current = root;
    while (current != NULL) {
        (*comparisons)++;
        if (key == current->data) return 1;
        current = (key < current->data) ? current->left : current->right;
    }
    return 0;
}

int bstHeight(Node* node) {
    if (node == NULL) return 0;
    int l = bstHeight(node->left);
    int r = bstHeight(node->right);
    return 1 + (l > r ? l : r);
}

void freeBST(Node* root) {
    if (root == NULL) return;
    freeBST(root->left);
    freeBST(root->right);
    free(root);
}

/* ---------- AVL ---------- */
typedef struct AVLNode {
    int data;
    struct AVLNode *left, *right;
    int height;  /* 노드 수 기준 높이: 빈 트리=0, 단말=1 */
} AVLNode;

int avlH(AVLNode* n) { return n ? n->height : 0; }
int maxInt(int a, int b) { return a > b ? a : b; }

AVLNode* createAVLNode(int value) {
    AVLNode* n = (AVLNode*)malloc(sizeof(AVLNode));
    n->data = value;
    n->left = n->right = NULL;
    n->height = 1;
    return n;
}

AVLNode* rotateRight(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* t2 = x->right;
    x->right = y;
    y->left = t2;
    y->height = 1 + maxInt(avlH(y->left), avlH(y->right));
    x->height = 1 + maxInt(avlH(x->left), avlH(x->right));
    return x;
}

AVLNode* rotateLeft(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* t2 = y->left;
    y->left = x;
    x->right = t2;
    x->height = 1 + maxInt(avlH(x->left), avlH(x->right));
    y->height = 1 + maxInt(avlH(y->left), avlH(y->right));
    return y;
}

AVLNode* avlInsert(AVLNode* node, int value, long* comparisons, int* inserted) {
    if (node == NULL) {
        *inserted = 1;
        return createAVLNode(value);
    }

    (*comparisons)++;
    if (value == node->data) {
        *inserted = 0;
        return node;
    } else if (value < node->data) {
        node->left = avlInsert(node->left, value, comparisons, inserted);
    } else {
        node->right = avlInsert(node->right, value, comparisons, inserted);
    }

    node->height = 1 + maxInt(avlH(node->left), avlH(node->right));
    int balance = avlH(node->left) - avlH(node->right);

    if (balance > 1 && value < node->left->data) return rotateRight(node);
    if (balance < -1 && value > node->right->data) return rotateLeft(node);
    if (balance > 1 && value > node->left->data) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    if (balance < -1 && value < node->right->data) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }
    return node;
}

int avlSearch(AVLNode* root, int key, long* comparisons) {
    AVLNode* current = root;
    while (current != NULL) {
        (*comparisons)++;
        if (key == current->data) return 1;
        current = (key < current->data) ? current->left : current->right;
    }
    return 0;
}

void freeAVL(AVLNode* root) {
    if (root == NULL) return;
    freeAVL(root->left);
    freeAVL(root->right);
    free(root);
}

/* ---------- 배열 ---------- */
int arrayInsertOrSkip(int arr[], int* len, int value, long* comparisons) {
    for (int i = 0; i < *len; i++) {
        (*comparisons)++;
        if (arr[i] == value) return 0;
    }
    arr[*len] = value;
    (*len)++;
    return 1;
}

int sequentialSearch(int arr[], int n, int key, long* comparisons) {
    for (int i = 0; i < n; i++) {
        (*comparisons)++;
        if (arr[i] == key) return 1;
    }
    return 0;
}

/* ---------- main ---------- */
int main(void) {
    srand((unsigned int)time(NULL));

    int generated[GEN_COUNT];
    int arr[GEN_COUNT];
    int arrLen = 0;
    Node* bstRoot = NULL;
    AVLNode* avlRoot = NULL;

    long arrBuildComparisons = 0, bstBuildComparisons = 0, avlBuildComparisons = 0;
    int skippedCount = 0;

    for (int i = 0; i < GEN_COUNT; i++) {
        int value = rand() % (MAX_VALUE + 1);
        generated[i] = value;

        int arrInserted = arrayInsertOrSkip(arr, &arrLen, value, &arrBuildComparisons);

        int bstInserted = bstInsertOrSkip(&bstRoot, value, &bstBuildComparisons);

        int avlInserted;
        avlRoot = avlInsert(avlRoot, value, &avlBuildComparisons, &avlInserted);

        if (!arrInserted) skippedCount++;

        /* 세 자료구조는 항상 동일한 집합을 가져야 함 (검증용) */
        if (arrInserted != bstInserted || arrInserted != avlInserted) {
            fprintf(stderr, "경고: 세 자료구조의 삽입/중복 판정이 서로 다릅니다 (값=%d)\n", value);
        }
    }

    printf("[생성된 정수 100개 (발생 순서)]\n");
    for (int i = 0; i < GEN_COUNT; i++) {
        printf("%4d", generated[i]);
        if ((i + 1) % 10 == 0) printf("\n"); else printf(" ");
    }

    printf("\n실제로 저장된 서로 다른 값의 수 : %d\n", arrLen);
    printf("중복으로 인해 삽입되지 않은 값의 수 : %d\n", skippedCount);

    printf("\n[자료구조 생성 비용]\n");
    printf("배열 생성 과정 총 비교 횟수 : %ld\n", arrBuildComparisons);
    printf("BST 생성 과정 총 비교 횟수  : %ld\n", bstBuildComparisons);
    printf("AVL 생성 과정 총 비교 횟수  : %ld\n", avlBuildComparisons);

    printf("\n[자료구조의 크기 및 높이]\n");
    printf("배열의 길이 : %d\n", arrLen);
    printf("BST의 높이  : %d\n", bstHeight(bstRoot));
    printf("AVL의 높이  : %d\n", avlRoot ? avlRoot->height : 0);

    /* 탐색 대상 50개 */
    int searchKeys[SEARCH_COUNT];
    for (int i = 0; i < SEARCH_COUNT; i++) {
        searchKeys[i] = rand() % (MAX_VALUE + 1);
    }

    long seqTotal = 0, bstTotal = 0, avlTotal = 0;

    printf("\n[탐색 결과]\n");
    for (int i = 0; i < SEARCH_COUNT; i++) {
        int key = searchKeys[i];
        long seqC = 0, bstC = 0, avlC = 0;

        int seqFound = sequentialSearch(arr, arrLen, key, &seqC);
        int bstFound = bstSearch(bstRoot, key, &bstC);
        int avlFound = avlSearch(avlRoot, key, &avlC);

        seqTotal += seqC;
        bstTotal += bstC;
        avlTotal += avlC;

        printf("Search Key : %d\n", key);
        printf("  Result             : %s\n", seqFound ? "Found" : "Not Found");
        printf("  Sequential Search  Comparisons : %ld\n", seqC);
        printf("  BST Search         Comparisons : %ld\n", bstC);
        printf("  AVL Search         Comparisons : %ld\n", avlC);

        if (seqFound != bstFound || seqFound != avlFound) {
            fprintf(stderr, "경고: 탐색 성공/실패 결과가 자료구조마다 다릅니다 (key=%d)\n", key);
        }
    }

    printf("\n[탐색 50회 종합 결과]\n");
    printf("Searches : %d\n\n", SEARCH_COUNT);

    printf("Sequential Search\n");
    printf("  Total comparisons   : %ld\n", seqTotal);
    printf("  Average comparisons : %.2f\n\n", (double)seqTotal / SEARCH_COUNT);

    printf("BST Search\n");
    printf("  Total comparisons   : %ld\n", bstTotal);
    printf("  Average comparisons : %.2f\n\n", (double)bstTotal / SEARCH_COUNT);

    printf("AVL Search\n");
    printf("  Total comparisons   : %ld\n", avlTotal);
    printf("  Average comparisons : %.2f\n", (double)avlTotal / SEARCH_COUNT);

    freeBST(bstRoot);
    freeAVL(avlRoot);
    return 0;
}
