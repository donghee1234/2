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

int count_nodes(Node* root) {
    if (root == NULL) return 0;
    int count = 1;
    for (int i = 0; i < root->child_count; i++) {
        count += count_nodes(root->children[i]);
    }
    return count;
}

int count_leaves(Node* root) {
    if (root == NULL) return 0;
    if (root->child_count == 0) return 1;
    int count = 0;
    for (int i = 0; i < root->child_count; i++) {
        count += count_leaves(root->children[i]);
    }
    return count;
}

int get_height(Node* root) {
    if (root == NULL) return 0;
    if (root->child_count == 0) return 1;
    int max_child_height = 0;
    for (int i = 0; i < root->child_count; i++) {
        int h = get_height(root->children[i]);
        if (h > max_child_height) {
            max_child_height = h;
        }
    }
    return max_child_height + 1;
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

    printf("입력: ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = '\0';

    const char* cursor = input;
    Node* root = parse_tree(&cursor);

    printf("루트: %c\n", root->name);
    printf("노드 개수: %d\n", count_nodes(root));
    printf("단말 노드 개수: %d\n", count_leaves(root));
    printf("트리 높이: %d\n", get_height(root));
    printf("전위 순회: ");
    print_preorder(root);
    printf("\n");

    free_tree(root);

    return 0;
}
