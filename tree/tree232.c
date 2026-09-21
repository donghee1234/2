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

int findIndex(char target) {
    for (int i = 1; i < MAX; i++) {
        if (tree[i] == target) return i;
    }
    return -1;
}

void printNode(const char* label, int idx) {
    if (idx > 0 && idx < MAX && tree[idx] != 0) {
        printf("%s : %c\n", label, tree[idx]);
    } else {
        printf("%s : 없음\n", label);
    }
}

int main(void) {
    printf("괄호 표기법으로 이진트리를 입력하세요.\n> ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;

    pos = 0;
    if (input[0] == '(') parse(1);

    char target;
    printf("정보를 조회할 노드를 입력하세요 (예: B) : ");
    scanf(" %c", &target);

    int idx = findIndex(target);
    if (idx == -1) {
        printf("트리에 존재하지 않는 노드입니다.\n");
        return 0;
    }

    printf("\n[%c 노드 정보]\n", target);
    printf("-----------------------------\n");

    printNode("부모 노드  ", idx == 1 ? -1 : idx / 2);
    printNode("왼쪽 자식  ", idx * 2);
    printNode("오른쪽 자식", idx * 2 + 1);

    if (idx == 1) {
        printf("형제 노드   : 없음 (루트)\n");
    } else if (idx % 2 == 0) {
        printNode("형제 노드  ", idx + 1);
    } else {
        printNode("형제 노드  ", idx - 1);
    }

    return 0;
}
