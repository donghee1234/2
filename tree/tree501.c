#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DATA_COUNT 100
#define SEARCH_COUNT 50
#define MAX_VALUE 1000

typedef struct Node {
    int data;
    struct Node *left;
    struct Node *right;
} Node;

Node* createNode(int value) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->data = value;
    node->left = NULL;
    node->right = NULL;
    return node;
}

/* BST에 value를 삽입하고, 삽입 과정에서 발생한 비교 횟수를 반환한다. */
int insertBST(Node** root, int value) {
    int comparisons = 0;

    if (*root == NULL) {
        *root = createNode(value);
        return comparisons;
    }

    Node* current = *root;
    while (1) {
        comparisons++;
        if (value < current->data) {
            if (current->left == NULL) {
                current->left = createNode(value);
                break;
            }
            current = current->left;
        } else {
            if (current->right == NULL) {
                current->right = createNode(value);
                break;
            }
            current = current->right;
        }
    }
    return comparisons;
}

/* 배열에서 순차 탐색. found에 성공 여부, 반환값은 비교 횟수 */
int sequentialSearch(int arr[], int n, int key, int* found) {
    int comparisons = 0;
    *found = 0;

    for (int i = 0; i < n; i++) {
        comparisons++;
        if (arr[i] == key) {
            *found = 1;
            break;
        }
    }
    return comparisons;
}

/* BST에서 탐색. found에 성공 여부, 반환값은 비교 횟수 */
int bstSearch(Node* root, int key, int* found) {
    int comparisons = 0;
    *found = 0;
    Node* current = root;

    while (current != NULL) {
        comparisons++;
        if (key == current->data) {
            *found = 1;
            break;
        } else if (key < current->data) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    return comparisons;
}

int isDuplicate(int arr[], int count, int value) {
    for (int i = 0; i < count; i++) {
        if (arr[i] == value) return 1;
    }
    return 0;
}

void freeBST(Node* root) {
    if (root == NULL) return;
    freeBST(root->left);
    freeBST(root->right);
    free(root);
}

int main(void) {
    srand((unsigned int)time(NULL));

    /* 1. 0~1000 사이 서로 다른 정수 100개 생성 */
    int data[DATA_COUNT];
    int count = 0;
    while (count < DATA_COUNT) {
        int value = rand() % (MAX_VALUE + 1);
        if (!isDuplicate(data, count, value)) {
            data[count] = value;
            count++;
        }
    }

    printf("[생성된 정수 100개 (발생 순서)]\n");
    for (int i = 0; i < DATA_COUNT; i++) {
        printf("%4d", data[i]);
        if ((i + 1) % 10 == 0) printf("\n"); else printf(" ");
    }
    printf("\n");

    /* 2. 동일한 순서로 BST 생성, 생성 비용(비교 횟수) 측정 */
    Node* root = NULL;
    int bstBuildComparisons = 0;
    for (int i = 0; i < DATA_COUNT; i++) {
        bstBuildComparisons += insertBST(&root, data[i]);
    }

    printf("\nBST 생성 과정에서 발생한 총 비교 횟수 : %d\n", bstBuildComparisons);

    /* 3. 탐색 대상 50개 생성 (배열/트리에 있을 수도, 없을 수도 있음) */
    int searchKeys[SEARCH_COUNT];
    for (int i = 0; i < SEARCH_COUNT; i++) {
        searchKeys[i] = rand() % (MAX_VALUE + 1);
    }

    /* 4. 50개 각각에 대해 순차 탐색 / BST 탐색 수행 및 비교 횟수 측정 */
    long seqTotal = 0, bstTotal = 0;

    printf("\n[탐색 결과]\n");
    for (int i = 0; i < SEARCH_COUNT; i++) {
        int key = searchKeys[i];
        int seqFound, bstFound;

        int seqComparisons = sequentialSearch(data, DATA_COUNT, key, &seqFound);
        int bstComparisons = bstSearch(root, key, &bstFound);

        seqTotal += seqComparisons;
        bstTotal += bstComparisons;

        printf("Search Key : %d\n", key);
        printf("  Sequential Search  Result : %-8s Comparisons : %d\n",
               seqFound ? "Found" : "Not Found", seqComparisons);
        printf("  BST Search         Result : %-8s Comparisons : %d\n",
               bstFound ? "Found" : "Not Found", bstComparisons);
    }

    double seqAvg = (double)seqTotal / SEARCH_COUNT;
    double bstAvg = (double)bstTotal / SEARCH_COUNT;

    printf("\n[탐색 50회 종합 결과]\n");
    printf("Number of searches : %d\n\n", SEARCH_COUNT);
    printf("Sequential Search\n");
    printf("  Total comparisons   : %ld\n", seqTotal);
    printf("  Average comparisons : %.2f\n\n", seqAvg);
    printf("BST Search\n");
    printf("  Total comparisons   : %ld\n", bstTotal);
    printf("  Average comparisons : %.2f\n", bstAvg);

    printf("\n[BST 생성 비용 포함 총 비교 횟수]\n");
    printf("BST 생성 비용 + BST 탐색 50회 : %d + %ld = %ld\n",
           bstBuildComparisons, bstTotal, (long)bstBuildComparisons + bstTotal);

    freeBST(root);
    return 0;
}
