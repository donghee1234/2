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
int maxIndex;
int totalNodes;

void parseArray(int idx) {
    pos++;
    if (idx > maxIndex) maxIndex = idx;

    if (input[pos] != ',' && input[pos] != ')') {
        pos++;
        totalNodes++;
    }

    if (input[pos] == ',') {
        pos++;
        if (input[pos] == '(') parseArray(idx * 2);
    }
    if (input[pos] == ',') {
        pos++;
        if (input[pos] == '(') parseArray(idx * 2 + 1);
    }
    if (input[pos] == ')') pos++;
}

Node* parsePointer() {
    pos++;
    char data = 0;
    if (input[pos] != ',' && input[pos] != ')') {
        data = input[pos];
        pos++;
    }
    Node* node = (Node*)malloc(sizeof(Node));
    node->data = data;
    node->left = NULL;
    node->right = NULL;

    if (input[pos] == ',') {
        pos++;
        if (input[pos] == '(') node->left = parsePointer();
    }
    if (input[pos] == ',') {
        pos++;
        if (input[pos] == '(') node->right = parsePointer();
    }
    if (input[pos] == ')') pos++;

    return node;
}

void measure(const char* label, const char* treeStr) {
    strcpy(input, treeStr);

    pos = 0; maxIndex = 0; totalNodes = 0;
    parseArray(1);
    long arrayBytes = (long)(maxIndex + 1) * sizeof(char);

    pos = 0;
    Node* root = parsePointer();
    (void)root;
    long pointerBytes = (long)totalNodes * sizeof(Node);

    printf("%-20s | 노드 수: %2d | 배열 방식: %5ld byte (배열 크기 %d) | 연결 방식: %5ld byte\n",
           label, totalNodes, arrayBytes, maxIndex + 1, pointerBytes);
}

int main(void) {
    printf("sizeof(Node) = %zu byte  (char data + left포인터 + right포인터)\n\n", sizeof(Node));

    measure("일반 이진트리",     "(A,(B,(D,,),(E,,)),(C,,(F,(G,,),)))");
    measure("완전(포화) 이진트리", "(A,(B,(D,,),(E,,)),(C,(F,,),(G,,)))");
    measure("편향 이진트리",     "(A,(B,(C,(D,(E,(F,(G,,),),),),),),)");

    return 0;
}
