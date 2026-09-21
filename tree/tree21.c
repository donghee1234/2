#include <stdio.h>
#include <string.h>

#define MAX 200

char tree[MAX];
char input[1024];
int pos;

void parse(int idx) {
    pos++;
    if (input[pos] != ',' && input[pos] != ')') {
        tree[idx] = input[pos];
        pos++;
    }

    if (input[pos] == ',') {
        pos++;
        if (input[pos] == '(') parse(idx * 2);
    }

    if (input[pos] == ',') {
        pos++;
        if (input[pos] == '(') parse(idx * 2 + 1);
    }

    if (input[pos] == ')') pos++;
}

void printTree(int idx, int depth) {
    if (idx >= MAX || tree[idx] == 0) return;

    printTree(idx * 2 + 1, depth + 1);

    for (int i = 0; i < depth; i++) printf("    ");
    printf("%c\n", tree[idx]);

    printTree(idx * 2, depth + 1);
}

int hasLeft(int idx)  { return tree[idx * 2] != 0; }
int hasRight(int idx) { return tree[idx * 2 + 1] != 0; }

int countTotalNodes() {
    int count = 0;
    for (int i = 1; i < MAX; i++) {
        if (tree[i] != 0) count++;
    }
    return count;
}

int countLeafNodes() {
    int count = 0;
    for (int i = 1; i < MAX; i++) {
        if (tree[i] != 0 && !hasLeft(i) && !hasRight(i)) count++;
    }
    return count;
}

int getHeight(int idx) {
    if (idx >= MAX || tree[idx] == 0) return 0;
    int left = getHeight(idx * 2);
    int right = getHeight(idx * 2 + 1);
    return 1 + (left > right ? left : right);
}

int getDegree() {
    int maxDegree = 0;
    for (int i = 1; i < MAX; i++) {
        if (tree[i] == 0) continue;
        int degree = hasLeft(i) + hasRight(i);
        if (degree > maxDegree) maxDegree = degree;
    }
    return maxDegree;
}

int isFullBinaryTree(int totalNodes, int height) {
    return totalNodes == (1 << height) - 1;
}

int isCompleteBinaryTree(int totalNodes) {
    for (int i = 1; i <= totalNodes; i++) {
        if (tree[i] == 0) return 0;
    }
    if (totalNodes + 1 < MAX && tree[totalNodes + 1] != 0) return 0;
    return 1;
}

int isSkewedBinaryTree(int totalNodes) {
    if (totalNodes <= 1) return 0;

    int isLeftSkewed = 1, isRightSkewed = 1;

    for (int i = 1; i < MAX; i++) {
        if (tree[i] == 0) continue;
        int L = hasLeft(i), R = hasRight(i);

        if (L && R) { isLeftSkewed = 0; isRightSkewed = 0; break; }
        if (L && !R) isRightSkewed = 0;
        if (R && !L) isLeftSkewed = 0;
    }
    return isLeftSkewed || isRightSkewed;
}

int main(void) {
    printf("괄호 표기법으로 이진트리를 입력하세요.\n");
    printf("예) (A,(B,(D,,),(E,,)),(C,,(F,,)))\n> ");

    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;

    pos = 0;
    if (input[0] == '(') {
        parse(1);
    } else {
        printf("빈 트리입니다.\n");
        return 0;
    }

    int totalNodes = countTotalNodes();
    int leafNodes = countLeafNodes();
    int nonLeafNodes = totalNodes - leafNodes;
    int height = getHeight(1);
    int degree = getDegree();

    printf("\n[1] 이진트리 출력\n");
    printf("-----------------------------\n");
    printTree(1, 0);

    printf("\n[2] 트리 정보 출력\n");
    printf("-----------------------------\n");
    printf("전체 노드의 수     : %d\n", totalNodes);
    printf("단말 노드의 수     : %d\n", leafNodes);
    printf("비단말 노드의 수   : %d\n", nonLeafNodes);
    printf("트리의 높이        : %d\n", height);
    printf("트리의 차수        : %d\n", degree);

    printf("\n[3] 이진트리의 형태 판별\n");
    printf("-----------------------------\n");
    printf("완전 이진트리 여부 : %s\n", isCompleteBinaryTree(totalNodes) ? "예" : "아니오");
    printf("포화 이진트리 여부 : %s\n", isFullBinaryTree(totalNodes, height) ? "예" : "아니오");
    printf("편향 이진트리 여부 : %s\n", isSkewedBinaryTree(totalNodes) ? "예" : "아니오");

    return 0;
}
