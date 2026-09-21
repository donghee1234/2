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
        if (input[pos] == '(') node->left = parse();
    }
    if (input[pos] == ',') {
        pos++;
        if (input[pos] == '(') node->right = parse();
    }
    if (input[pos] == ')') pos++;

    return node;
}

Node* findParent(Node* node, char target) {
    if (node == NULL) return NULL;

    if (node->left != NULL && node->left->data == target) return node;
    if (node->right != NULL && node->right->data == target) return node;

    Node* found = findParent(node->left, target);
    if (found != NULL) return found;

    return findParent(node->right, target);
}

Node* findNode(Node* node, char target) {
    if (node == NULL) return NULL;
    if (node->data == target) return node;

    Node* found = findNode(node->left, target);
    if (found != NULL) return found;

    return findNode(node->right, target);
}

void printNode(const char* label, Node* node) {
    if (node != NULL) {
        printf("%s : %c\n", label, node->data);
    } else {
        printf("%s : 없음\n", label);
    }
}

int main(void) {
    printf("괄호 표기법으로 이진트리를 입력하세요.\n> ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;

    Node* root = NULL;
    pos = 0;
    if (input[0] == '(') root = parse();

    char target;
    printf("정보를 조회할 노드를 입력하세요 (예: B) : ");
    scanf(" %c", &target);

    Node* node = findNode(root, target);
    if (node == NULL) {
        printf("트리에 존재하지 않는 노드입니다.\n");
        return 0;
    }

    Node* parent = findParent(root, target);

    printf("\n[%c 노드 정보]\n", target);
    printf("-----------------------------\n");

    printNode("부모 노드  ", parent);
    printNode("왼쪽 자식  ", node->left);
    printNode("오른쪽 자식", node->right);

    if (parent == NULL) {
        printf("형제 노드   : 없음 (루트)\n");
    } else if (parent->left == node) {
        printNode("형제 노드  ", parent->right);
    } else {
        printNode("형제 노드  ", parent->left);
    }

    return 0;
}
