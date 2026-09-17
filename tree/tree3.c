#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 1000
#define MAX_DEPTH 100

char input[MAX_LEN];

char name_stack[MAX_DEPTH];
int child_count_stack[MAX_DEPTH];
int top = -1;

int total_nodes = 0;
int leaf_nodes = 0;
int max_height = 0;
int max_degree = 0;

char target_node = 'C';
int target_found = 0;
char parent_of_target = 0;
char children_of_target[MAX_DEPTH];
int children_of_target_count = 0;

int is_valid = 1;

void analyze_tree() {
    int i = 0;
    int expect_name = 1;

    while (input[i] != '\0' && is_valid) {
        char c = input[i];

        if (expect_name) {
            if (!(c >= 'A' && c <= 'Z')) {
                is_valid = 0;
                break;
            }

            total_nodes++;

            if (c == target_node) {
                target_found = 1;
                if (top >= 0) {
                    parent_of_target = name_stack[top];
                }
            }

            if (top >= 0) {
                child_count_stack[top]++;
                if (name_stack[top] == target_node) {
                    children_of_target[children_of_target_count++] = c;
                }
            }

            int depth = top + 2;
            if (depth > max_height) {
                max_height = depth;
            }

            i++;
            expect_name = 0;

            if (input[i] == '(') {
                top++;
                if (top >= MAX_DEPTH) {
                    is_valid = 0;
                    break;
                }
                name_stack[top] = c;
                child_count_stack[top] = 0;
                i++;
                expect_name = 1;
            } else {
                leaf_nodes++;
            }
        } else {
            if (c == ',') {
                if (top < 0) {
                    is_valid = 0;
                    break;
                }
                i++;
                expect_name = 1;
            } else if (c == ')') {
                if (top < 0) {
                    is_valid = 0;
                    break;
                }
                if (child_count_stack[top] == 0) {
                    is_valid = 0;
                    break;
                }
                if (child_count_stack[top] > max_degree) {
                    max_degree = child_count_stack[top];
                }
                top--;
                i++;
            } else {
                is_valid = 0;
                break;
            }
        }
    }

    if (total_nodes == 0 || top != -1 || expect_name == 1) {
        is_valid = 0;
    }
}

const char* skip_node(const char* p) {
    p++;
    if (*p == '(') {
        int depth = 1;
        p++;
        while (depth > 0) {
            if (*p == '(') depth++;
            else if (*p == ')') depth--;
            p++;
        }
    }
    return p;
}

void print_tree(const char** cursor, char* prefix, int is_last, int is_root) {
    char name = **cursor;

    if (is_root) {
        printf("%c\n", name);
    } else {
        printf("%s+---%c\n", prefix, name);
    }

    char child_prefix[MAX_LEN];
    strcpy(child_prefix, prefix);
    if (!is_root) {
        strcat(child_prefix, is_last ? "    " : "|   ");
    }

    (*cursor)++;

    if (**cursor == '(') {
        (*cursor)++;
        while (**cursor != ')') {
            int child_is_last = (*skip_node(*cursor) != ',');
            print_tree(cursor, child_prefix, child_is_last, 0);
            if (**cursor == ',') {
                (*cursor)++;
            }
        }
        (*cursor)++;
    }
}

int main() {
    printf("입력: ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = '\0';

    analyze_tree();

    if (!is_valid) {
        printf("올바른 트리의 괄호 표기법이 아닙니다.\n");
        return 1;
    }

    printf("전체 노드의 수: %d\n", total_nodes);
    printf("단말 노드의 수: %d\n", leaf_nodes);
    printf("비단말 노드의 수: %d\n", total_nodes - leaf_nodes);
    printf("트리의 높이: %d\n", max_height);
    printf("트리의 차수: %d\n", max_degree);

    if (target_found) {
        if (parent_of_target != 0) {
            printf("노드 %c의 부모 노드: %c\n", target_node, parent_of_target);
        } else {
            printf("노드 %c의 부모 노드: 없음 (루트 노드)\n", target_node);
        }

        printf("노드 %c의 자식 노드: ", target_node);
        if (children_of_target_count == 0) {
            printf("없음\n");
        } else {
            for (int i = 0; i < children_of_target_count; i++) {
                printf("%c ", children_of_target[i]);
            }
            printf("\n");
        }
    } else {
        printf("트리에 노드 %c가 존재하지 않습니다.\n", target_node);
    }

    printf("\n");
    const char* cursor = input;
    print_tree(&cursor, "", 1, 1);

    return 0;
}
