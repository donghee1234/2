#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    char data;
    struct Node *left;
    struct Node *right;
} Node;

char input[1024];
int pos;

Node* createNode(char data) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->data = data;
    node->left = NULL;
    node->right = NULL;
    return node;
}

Node* parse() {
    pos++;

    char data = 0;
    if (input[pos] != ',' && input[pos] != ')') {
        data = input[pos];
        pos++;
    }

    Node* node = createNode(data);

    if (input[pos] == ',') {
        pos++;
        if (input[pos] == '(') {
            node->left = parse();
        }
    }

    if (input[pos] == ',') {
        pos++;
        if (input[pos] == '(') {
            node->right = parse();
        }
    }

    if (input[pos] == ')') {
        pos++;
    }

    return node;
}

void printTree(Node* node, int depth) {
    if (node == NULL) return;

    printTree(node->right, depth + 1);

    for (int i = 0; i < depth; i++) printf("    ");
    printf("%c\n", node->data);

    printTree(node->left, depth + 1);
}

int countTotalNodes(Node* node) {
    if (node == NULL) return 0;
    return 1 + countTotalNodes(node->left) + countTotalNodes(node->right);
}

int countLeafNodes(Node* node) {
    if (node == NULL) return 0;
    if (node->left == NULL && node->right == NULL) return 1;
    return countLeafNodes(node->left) + countLeafNodes(node->right);
}

int getHeight(Node* node) {
    if (node == NULL) return 0;
    int left = getHeight(node->left);
    int right = getHeight(node->right);
    if (left > right) return 1 + left;
    return 1 + right;
}

int maxDegree = 0;

void checkDegree(Node* node) {
    if (node == NULL) return;

    int degree = 0;
    if (node->left != NULL) degree++;
    if (node->right != NULL) degree++;

    if (degree > maxDegree) maxDegree = degree;

    checkDegree(node->left);
    checkDegree(node->right);
}

int isFullBinaryTree(int totalNodes, int height) {
    return totalNodes == (1 << height) - 1;
}

#define QSIZE 2000
Node* queue[QSIZE];
int front, rear;

void enqueue(Node* n) { queue[rear] = n; rear++; }
Node* dequeue() { Node* n = queue[front]; front++; return n; }
int isQueueEmpty() { return front == rear; }

int isCompleteBinaryTree(Node* root) {
    if (root == NULL) return 0;

    front = 0;
    rear = 0;
    enqueue(root);
    int seenEmpty = 0;

    while (!isQueueEmpty()) {
        Node* cur = dequeue();

        if (cur == NULL) {
            seenEmpty = 1;
        } else {
            if (seenEmpty == 1) {
                return 0;
            }
            enqueue(cur->left);
            enqueue(cur->right);
        }
    }
    return 1;
}

int isLeftSkewed, isRightSkewed;

void checkSkewed(Node* node) {
    if (node == NULL) return;

    int hasLeft = (node->left != NULL);
    int hasRight = (node->right != NULL);

    if (hasLeft && hasRight) {
        isLeftSkewed = 0;
        isRightSkewed = 0;
    } else if (hasLeft && !hasRight) {
        isRightSkewed = 0;
    } else if (hasRight && !hasLeft) {
        isLeftSkewed = 0;
    }

    checkSkewed(node->left);
    checkSkewed(node->right);
}

int isSkewedBinaryTree(Node* root, int totalNodes) {
    if (totalNodes <= 1) return 0;

    isLeftSkewed = 1;
    isRightSkewed = 1;
    checkSkewed(root);

    return isLeftSkewed || isRightSkewed;
}

int main(void) {
    printf("괄호 표기법으로 이진트리를 입력하세요.\n");
    printf("예) (A,(B,(D,,),(E,,)),(C,,(F,,)))\n> ");

    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;

    Node* root = NULL;
    pos = 0;

    if (input[0] == '(') {
        root = parse();
    } else {
        printf("빈 트리입니다.\n");
        return 0;
    }

    int totalNodes = countTotalNodes(root);
    int leafNodes = countLeafNodes(root);
    int nonLeafNodes = totalNodes - leafNodes;
    int height = getHeight(root);

    maxDegree = 0;
    checkDegree(root);

    printf("\n[1] 이진트리 출력\n");
    printf("-----------------------------\n");
    printTree(root, 0);

    printf("\n[2] 트리 정보 출력\n");
    printf("-----------------------------\n");
    printf("전체 노드의 수     : %d\n", totalNodes);
    printf("단말 노드의 수     : %d\n", leafNodes);
    printf("비단말 노드의 수   : %d\n", nonLeafNodes);
    printf("트리의 높이        : %d\n", height);
    printf("트리의 차수        : %d\n", maxDegree);

    printf("\n[3] 이진트리의 형태 판별\n");
    printf("-----------------------------\n");
    printf("완전 이진트리 여부 : %s\n", isCompleteBinaryTree(root) ? "예" : "아니오");
    printf("포화 이진트리 여부 : %s\n", isFullBinaryTree(totalNodes, height) ? "예" : "아니오");
    printf("편향 이진트리 여부 : %s\n", isSkewedBinaryTree(root, totalNodes) ? "예" : "아니오");

    return 0;
}
