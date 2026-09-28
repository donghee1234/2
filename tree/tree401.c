#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX 2048

typedef struct Node {
    char data;
    struct Node *left;
    struct Node *right;
} Node;

typedef struct {
    Node *node;
    int commas;
} Frame;

Node* createNode(char data) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->data = data;
    node->left = NULL;
    node->right = NULL;
    return node;
}

void freeTree(Node* root) {
    if (root == NULL) return;
    Node* stack[MAX];
    int top = 0;
    stack[top++] = root;
    while (top > 0) {
        Node* cur = stack[--top];
        if (cur->left != NULL) stack[top++] = cur->left;
        if (cur->right != NULL) stack[top++] = cur->right;
        free(cur);
    }
}

int buildTree(char* s, Node** outRoot) {
    Frame frames[MAX];
    int ft = 0;
    Node* root = NULL;
    int ok = 1;
    int i = 0;

    while (s[i] != '\0' && ok) {
        char c = s[i];

        if (isspace((unsigned char)c)) {
            i++;
        } else if (c == '(') {
            i++;
            if (!isalnum((unsigned char)s[i])) { ok = 0; break; }
            if (ft == 0 && root != NULL) { ok = 0; break; }
            if (ft >= MAX) { ok = 0; break; }

            Node* node = createNode(s[i]);
            if (ft == 0) {
                root = node;
            } else {
                Frame* top = &frames[ft - 1];
                if (top->commas == 1 && top->node->left == NULL) {
                    top->node->left = node;
                } else if (top->commas == 2 && top->node->right == NULL) {
                    top->node->right = node;
                } else {
                    free(node);
                    ok = 0;
                    break;
                }
            }
            frames[ft].node = node;
            frames[ft].commas = 0;
            ft++;
            i++;
        } else if (c == ',') {
            if (ft == 0) { ok = 0; break; }
            frames[ft - 1].commas++;
            if (frames[ft - 1].commas > 2) { ok = 0; break; }
            i++;
        } else if (c == ')') {
            if (ft == 0 || frames[ft - 1].commas != 2) { ok = 0; break; }
            ft--;
            i++;
        } else {
            ok = 0;
        }
    }

    if (ok && (ft != 0 || root == NULL)) ok = 0;

    if (!ok) {
        freeTree(root);
        *outRoot = NULL;
        return 0;
    }
    *outRoot = root;
    return 1;
}

void printStructure(Node* tree) {
    Node* stack[MAX];
    int depths[MAX];
    int top = 0;
    Node* cur = tree;
    int depth = 0;

    while (cur != NULL || top > 0) {
        while (cur != NULL) {
            stack[top] = cur;
            depths[top] = depth;
            top++;
            cur = cur->right;
            depth++;
        }
        top--;
        cur = stack[top];
        depth = depths[top];

        for (int i = 0; i < depth; i++) printf("    ");
        printf("%c\n", cur->data);

        cur = cur->left;
        depth++;
    }
}

void preorder(Node* tree) {
    if (tree == NULL) return;
    Node* stack[MAX];
    int top = 0;
    stack[top++] = tree;

    while (top > 0) {
        Node* cur = stack[--top];
        printf("%c ", cur->data);
        if (cur->right != NULL) stack[top++] = cur->right;
        if (cur->left != NULL) stack[top++] = cur->left;
    }
}

void inorder(Node* tree) {
    Node* stack[MAX];
    int top = 0;
    Node* cur = tree;

    while (cur != NULL || top > 0) {
        while (cur != NULL) {
            stack[top++] = cur;
            cur = cur->left;
        }
        cur = stack[--top];
        printf("%c ", cur->data);
        cur = cur->right;
    }
}

void postorder(Node* tree) {
    Node* stack[MAX];
    int top = 0;
    Node* cur = tree;
    Node* last = NULL;

    while (cur != NULL || top > 0) {
        if (cur != NULL) {
            stack[top++] = cur;
            cur = cur->left;
        } else {
            Node* peek = stack[top - 1];
            if (peek->right != NULL && last != peek->right) {
                cur = peek->right;
            } else {
                printf("%c ", peek->data);
                last = peek;
                top--;
            }
        }
    }
}

int main(void) {
    char input[MAX];

    printf("괄호 표기법으로 이진트리를 입력하세요.\n");
    printf("예) (A,(B,(D,,),(E,,)),(C,,(F,,)))\n> ");

    if (fgets(input, sizeof(input), stdin) == NULL) return 0;
    input[strcspn(input, "\r\n")] = '\0';

    Node* tree = NULL;
    if (!buildTree(input, &tree)) {
        printf("오류: 괄호 표기법이 올바르지 않습니다.\n");
        return 0;
    }

    printf("\n[입력된 이진트리의 구조]\n");
    printStructure(tree);

    printf("\nPreorder  : ");
    preorder(tree);
    printf("\nInorder   : ");
    inorder(tree);
    printf("\nPostorder : ");
    postorder(tree);
    printf("\n");

    freeTree(tree);
    return 0;
}
