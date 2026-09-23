#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Node {
    char data;
    struct Node *left;
    struct Node *right;
} Node;

Node* root = NULL;

Node* createNode(char data) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->data = data;
    node->left = NULL;
    node->right = NULL;
    return node;
}

/* path 문자열을 따라가서 해당 노드와 그 부모를 찾는다. */
void findNodeAndParent(char* path, Node** outNode, Node** outParent) {
    *outNode = NULL;
    *outParent = NULL;
    if (root == NULL) return;

    char buf[200];
    strcpy(buf, path);

    char* token = strtok(buf, "/");
    if (token == NULL || strlen(token) != 1 || token[0] != root->data) return;

    Node* current = root;
    Node* parent = NULL;

    token = strtok(NULL, "/");
    while (token != NULL) {
        if (strlen(token) != 1) return;
        char c = token[0];

        Node* next = NULL;
        if (current->left != NULL && current->left->data == c) next = current->left;
        else if (current->right != NULL && current->right->data == c) next = current->right;

        if (next == NULL) return;

        parent = current;
        current = next;
        token = strtok(NULL, "/");
    }

    *outNode = current;
    *outParent = parent;
}

int isValidData(char* s) {
    return strlen(s) == 1 && isupper((unsigned char)s[0]);
}

void cmdInsertRoot(char* path, char* value) {
    if (strcmp(path, "/") != 0) {
        printf("오류: 루트 생성 경로 형식이 올바르지 않습니다.\n");
        return;
    }
    if (!isValidData(value)) {
        printf("오류: 데이터는 영문 대문자 한 글자여야 합니다.\n");
        return;
    }
    if (root != NULL) {
        printf("오류: 이미 루트 노드가 존재합니다.\n");
        return;
    }
    root = createNode(value[0]);
}

void cmdInsertChild(char* path, char* child, char* value) {
    if (strcmp(child, "L") != 0 && strcmp(child, "R") != 0) {
        printf("오류: child는 L 또는 R이어야 합니다.\n");
        return;
    }
    if (!isValidData(value)) {
        printf("오류: 데이터는 영문 대문자 한 글자여야 합니다.\n");
        return;
    }

    Node* parent;
    Node* unused;
    findNodeAndParent(path, &parent, &unused);

    if (parent == NULL) {
        printf("오류: 부모 노드가 존재하지 않습니다.\n");
        return;
    }
    if (parent->left != NULL || parent->right != NULL) {
        printf("오류: 부모 노드가 단말 노드가 아닙니다.\n");
        return;
    }

    Node* sibling = (strcmp(child, "L") == 0) ? parent->right : parent->left;
    if (sibling != NULL && sibling->data == value[0]) {
        printf("오류: 같은 부모의 다른 자식과 데이터가 같습니다.\n");
        return;
    }

    Node* newNode = createNode(value[0]);
    if (strcmp(child, "L") == 0) parent->left = newNode;
    else parent->right = newNode;
}

void cmdDelete(char* path) {
    Node* node;
    Node* parent;
    findNodeAndParent(path, &node, &parent);

    if (node == NULL) {
        printf("오류: 해당 노드가 존재하지 않습니다.\n");
        return;
    }
    if (node->left != NULL || node->right != NULL) {
        printf("오류: 단말 노드가 아닙니다.\n");
        return;
    }

    if (parent == NULL) {
        root = NULL;
    } else if (parent->left == node) {
        parent->left = NULL;
    } else {
        parent->right = NULL;
    }
    free(node);
}

void cmdUpdate(char* path, char* value) {
    if (!isValidData(value)) {
        printf("오류: 데이터는 영문 대문자 한 글자여야 합니다.\n");
        return;
    }

    Node* node;
    Node* parent;
    findNodeAndParent(path, &node, &parent);

    if (node == NULL) {
        printf("오류: 해당 노드가 존재하지 않습니다.\n");
        return;
    }

    if (parent != NULL) {
        Node* sibling = (parent->left == node) ? parent->right : parent->left;
        if (sibling != NULL && sibling->data == value[0]) {
            printf("오류: 같은 부모의 다른 자식과 데이터가 같습니다.\n");
            return;
        }
    }

    node->data = value[0];
}

void cmdRead(char* path) {
    Node* node;
    Node* unused;
    findNodeAndParent(path, &node, &unused);

    if (node == NULL) {
        printf("오류: 해당 노드가 존재하지 않습니다.\n");
        return;
    }

    if (node->left == NULL && node->right == NULL) {
        printf("자식 노드가 없습니다.\n");
        return;
    }

    if (node->left != NULL && node->right != NULL) {
        printf("%c(L), %c(R)\n", node->left->data, node->right->data);
    } else if (node->left != NULL) {
        printf("%c(L)\n", node->left->data);
    } else {
        printf("%c(R)\n", node->right->data);
    }
}

void printTree(Node* node, int depth) {
    if (node == NULL) return;

    if (depth == 0) {
        printf("%c\n", node->data);
    } else {
        for (int i = 0; i < depth - 1; i++) printf("    ");
        printf("+---%c\n", node->data);
    }

    printTree(node->left, depth + 1);
    printTree(node->right, depth + 1);
}

void cmdPrint() {
    if (root == NULL) {
        printf("트리가 비어 있습니다.\n");
        return;
    }
    printTree(root, 0);
}

int isCmd(char* token, char* full, char* shortForm) {
    return strcmp(token, full) == 0 || strcmp(token, shortForm) == 0;
}

int main(void) {
    char line[500];

    while (fgets(line, sizeof(line), stdin) != NULL) {
        char* tokens[10];
        int tokenCount = 0;

        char* tok = strtok(line, " \t\r\n");
        while (tok != NULL && tokenCount < 10) {
            tokens[tokenCount++] = tok;
            tok = strtok(NULL, " \t\r\n");
        }

        if (tokenCount == 0) continue;

        char* cmd = tokens[0];

        if (isCmd(cmd, "Insert", "I")) {
            if (tokenCount == 3) {
                cmdInsertRoot(tokens[1], tokens[2]);
            } else if (tokenCount == 4) {
                cmdInsertChild(tokens[1], tokens[2], tokens[3]);
            } else {
                printf("오류: Insert 명령의 인자 개수가 올바르지 않습니다.\n");
            }
        } else if (isCmd(cmd, "Delete", "D")) {
            if (tokenCount == 2) {
                cmdDelete(tokens[1]);
            } else {
                printf("오류: Delete 명령의 인자 개수가 올바르지 않습니다.\n");
            }
        } else if (isCmd(cmd, "Update", "U")) {
            if (tokenCount == 3) {
                cmdUpdate(tokens[1], tokens[2]);
            } else {
                printf("오류: Update 명령의 인자 개수가 올바르지 않습니다.\n");
            }
        } else if (isCmd(cmd, "Read", "R")) {
            if (tokenCount == 2) {
                cmdRead(tokens[1]);
            } else {
                printf("오류: Read 명령의 인자 개수가 올바르지 않습니다.\n");
            }
        } else if (isCmd(cmd, "Print", "P")) {
            if (tokenCount == 1) {
                cmdPrint();
            } else {
                printf("오류: Print 명령은 인자를 받지 않습니다.\n");
            }
        } else {
            printf("오류: 잘못된 명령어입니다.\n");
        }
    }

    return 0;
}
