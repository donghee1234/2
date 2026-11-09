#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// 전역 비교 횟수 카운터
long long comp_count = 0;

// 배열 요소 교환 함수
void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// -------------------------------------------------------------------------
// [1] 피벗 선택 함수들
// -------------------------------------------------------------------------
// 1. 첫 번째 원소 선택
int choose_pivot_first(int low, int high) {
    return low;
}

// 2. 무작위 원소 선택
int choose_pivot_random(int low, int high) {
    return low + rand() % (high - low + 1);
}

// 3. 세 원소의 중앙값 선택 (First, Middle, Last)
int choose_pivot_median_of_three(int arr[], int low, int high) {
    int mid = low + (high - low) / 2;
    
    // 세 값 비교 (비교 횟수에 포함)
    comp_count++;
    if (arr[low] > arr[mid]) {
        comp_count++;
        if (arr[mid] > arr[high]) return mid;
        else {
            comp_count++;
            return (arr[low] > arr[high]) ? high : low;
        }
    } else {
        comp_count++;
        if (arr[low] > arr[high]) return low;
        else {
            comp_count++;
            return (arr[mid] > arr[high]) ? high : mid;
        }
    }
}

// -------------------------------------------------------------------------
// [2] 파티션(Partition) 함수 (Lomuto 또는 Hoare 방식 중 Lomuto 파티션 사용 예시)
// -------------------------------------------------------------------------
int partition(int arr[], int low, int high, int mode) {
    int pivot_index;
    
    // 피벗 선택 방식에 따른 분기
    if (mode == 1) {
        pivot_index = choose_pivot_first(low, high);
    } else if (mode == 2) {
        pivot_index = choose_pivot_random(low, high);
    } else {
        pivot_index = choose_pivot_median_of_three(arr, low, high);
    }
    
    // 피벗을 맨 끝으로 이동
    swap(&arr[pivot_index], &arr[high]);
    int pivot = arr[high];
    
    int i = (low - 1);
    for (int j = low; j <= high - 1; j++) {
        comp_count++; // 데이터 간 비교 횟수 증가
        if (arr[j] <= pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

// -------------------------------------------------------------------------
// [3] 재귀적 퀵 정렬
// -------------------------------------------------------------------------
void quick_sort_recursive(int arr[], int low, int high, int mode) {
    if (low < high) {
        int pi = partition(arr, low, high, mode);
        quick_sort_recursive(arr, low, pi - 1, mode);
        quick_sort_recursive(arr, pi + 1, high, mode);
    }
}

// -------------------------------------------------------------------------
// [4] 보너스: 반복적 퀵 정렬 (스택 구조체 활용)
// -------------------------------------------------------------------------
typedef struct {
    int low;
    int high;
} StackFrame;

void quick_sort_iterative(int arr[], int low, int high, int mode) {
    int size = high - low + 1;
    StackFrame* stack = (StackFrame*)malloc(sizeof(StackFrame) * (size + 1));
    int top = -1;

    stack[++top] = (StackFrame){low, high};

    while (top >= 0) {
        StackFrame frame = stack[top--];
        int l = frame.low;
        int h = frame.high;

        if (l < h) {
            int pi = partition(arr, l, h, mode);

            if (pi - 1 > l) {
                stack[++top] = (StackFrame){l, pi - 1};
            }
            if (pi + 1 < h) {
                stack[++top] = (StackFrame){pi + 1, h};
            }
        }
    }
    free(stack);
}

// 배열 복사 함수
void copy_array(int src[], int dest[], int n) {
    for (int i = 0; i < n; i++) {
        dest[i] = src[i];
    }
}

int main() {
    srand((unsigned int)time(NULL));
    int sizes[] = {1000, 2000, 4000, 8000, 16000, 20000};
    int num_sizes = 6;

    printf("=========================================\n");
    printf(" 1~4번 과제: 피벗 선택에 따른 비교 횟수 비교\n");
    printf("=========================================\n");

    for (int s = 0; s < num_sizes; s++) {
        int n = sizes[s];
        int* original = (int*)malloc(sizeof(int) * n);
        int* temp = (int*)malloc(sizeof(int) * n);

        // 동일한 무작위 정수 데이터 생성 (0 ~ 100000 범위)
        for (int i = 0; i < n; i++) {
            original[i] = rand() % 100000;
        }

        printf("\nData Size : %d\n", n);

        // 1. 첫 번째 원소 (First Element)
        copy_array(original, temp, n);
        comp_count = 0;
        quick_sort_recursive(temp, 0, n - 1, 1);
        printf("Quick Sort (First)   : %lld comparisons\n", comp_count);

        // 2. 무작위 원소 (Random Pivot)
        copy_array(original, temp, n);
        comp_count = 0;
        quick_sort_recursive(temp, 0, n - 1, 2);
        printf("Quick Sort (Random)  : %lld comparisons\n", comp_count);

        // 3. 세 원소의 중앙값 (Median-of-Three)
        copy_array(original, temp, n);
        comp_count = 0;
        quick_sort_recursive(temp, 0, n - 1, 3);
        printf("Quick Sort (Median-3): %lld comparisons\n", comp_count);

        free(original);
        free(temp);
    }

    // -------------------------------------------------------------------------
    // 보너스 과제: 재귀적 vs 반복적 퀵 정렬 실행 시간 비교 (Data Size : 10000)
    // -------------------------------------------------------------------------
    printf("\n=========================================\n");
    printf(" [보너스] 재귀적 vs 반복적 퀵 정렬 성능 비교\n");
    printf("=========================================\n");
    
    int bonus_n = 10000;
    int* bonus_orig = (int*)malloc(sizeof(int) * bonus_n);
    int* bonus_temp = (int*)malloc(sizeof(int) * bonus_n);

    for (int i = 0; i < bonus_n; i++) {
        bonus_orig[i] = rand() % 100000;
    }

    printf("Data Size : %d (Pivot : Median-of-Three)\n", bonus_n);

    // 재귀적 실행 시간 측정
    copy_array(bonus_orig, bonus_temp, bonus_n);
    clock_t start = clock();
    quick_sort_recursive(bonus_temp, 0, bonus_n - 1, 3);
    clock_t end = clock();
    double recursive_time = (double)(end - start) / CLOCKS_PER_SEC * 1000.0; // ms 단위

    // 반복적 실행 시간 측정
    copy_array(bonus_orig, bonus_temp, bonus_n);
    start = clock();
    quick_sort_iterative(bonus_temp, 0, bonus_n - 1, 3);
    end = clock();
    double iterative_time = (double)(end - start) / CLOCKS_PER_SEC * 1000.0; // ms 단위

    printf("Recursive Quick Sort : %.3f ms\n", recursive_time);
    printf("Iterative Quick Sort : %.3f ms\n", iterative_time);

    free(bonus_orig);
    free(bonus_temp);

    return 0;
}
