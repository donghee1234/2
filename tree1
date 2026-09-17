#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CHILDREN 10
#define MAX_INPUT 256

typedef struct Node {
    char name;
    int child_count;
    struct Node* children[MAX_CHILDREN];
} Node;

Node* create_node(char name) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    new_node->name = name;
    new_node->child_count = 0;
    for (int i = 0; i < MAX_CHILDREN; i++) {
        new_node->children[i] = NULL;
    }
    return new_node;
}

Node* parse_tree(const char** cursor) {
    if (**cursor == '\0' || **cursor == ')' || **cursor == ',') {
        return NULL;
    }

    char name = **cursor;
    Node* node = create_node(name);
    (*cursor)++;

    if (**cursor == '(') {
        (*cursor)++;

        while (**cursor != ')' && **cursor != '\0') {
            Node* child = parse_tree(cursor);
            if (child != NULL && node->child_count < MAX_CHILDREN) {
                node->children[node->child_count++] = child;
            }

            if (**cursor == ',') {
                (*cursor)++;
            }
        }

        if (**cursor == ')') {
            (*cursor)++;
        }
    }

    return node;
}

void print_preorder(Node* root) {
    if (root == NULL) return;
    printf("%c ", root->name);
    for (int i = 0; i < root->child_count; i++) {
        print_preorder(root->children[i]);
    }
}

void free_tree(Node* root) {
    if (root == NULL) return;
    for (int i = 0; i < root->child_count; i++) {
        free_tree(root->children[i]);
    }
    free(root);
}

int main() {
    char input[MAX_INPUT];

    printf("트리의 괄호 표기법을 입력하세요 (ex: A(B(E,F),C,D(G))):\n> ");
    if (fgets(input, sizeof(input), stdin) != NULL) {
        input[strcspn(input, "\n")] = '\0';
    }

    const char* cursor = input;
    Node* root = parse_tree(&cursor);

    printf("\n[결과 트리가 성공적으로 생성되었습니다 !\n");
    printf("전위 순회(Pre-order) 출력: ");
    print_preorder(root);
    printf("\n");

    free_tree(root);

    return 0;
}
