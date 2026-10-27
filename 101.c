#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_DATA 10000
#define TRIALS 100

// 데이터 구조체 정의 (Key 1, Key 2, 원래 입력 순서 index)
typedef struct {
    int key1;
    int key2;
    int original_index;
} Element;

// 측정 결과 저장을 위한 구조체
typedef struct {
    long long comparisons;
    long long movements;
} Result;

// 함수 선언
void generate_random_data(Element data[], int n);
void copy_data(Element dest[], Element src[], int n);
void check_sorted(Element data[], int n, const char* sort_name);

// 정렬 알고리즘 함수 (삽입 정렬은 비교 연산자 선택을 위해 flag 사용: 1이면 '>', 0이면 '>=')
void selection_sort(Element data[], int n, Result* res);
void bubble_sort(Element data[], int n, Result* res);
void bubble_sort_early(Element data[], int n, Result* res);
void insertion_sort(Element data[], int n, Result* res, int strict_greater);

// 초기 상태별 생성 함수
void make_sorted_data(Element dest[], Element src[], int n);
void make_reverse_sorted_data(Element dest[], Element src[], int n);
void make_partially_sorted_data(Element dest[], Element src[], int n);

// 안정성 검증 함수
int verify_stability(Element sorted[], int n);

int main(void) {
    srand((unsigned int)time(NULL));

    Element original[MAX_DATA];
    Element working[MAX_DATA];
    Result res;

    printf("Data Count : %d\n\n", MAX_DATA);

    // ==========================================
    // 4. 무작위 데이터에 대한 100회 반복 실험
    // ==========================================
    printf("Random Data\n");
    printf("Number of Trials : %d\n\n", TRIALS);

    long long sel_comp_total = 0, sel_move_total = 0;
    long long bub_comp_total = 0, bub_move_total = 0;
    long long bub_e_comp_total = 0, bub_e_move_total = 0;
    long long ins_comp_total = 0, ins_move_total = 0;

    for (int t = 0; t < TRIALS; t++) {
        generate_random_data(original, MAX_DATA);

        // Selection Sort
        copy_data(working, original, MAX_DATA);
        res.comparisons = 0; res.movements = 0;
        selection_sort(working, MAX_DATA, &res);
        sel_comp_total += res.comparisons; sel_move_total += res.movements;
        if (t == 0) check_sorted(working, MAX_DATA, "Selection Sort");

        // Bubble Sort
        copy_data(working, original, MAX_DATA);
        res.comparisons = 0; res.movements = 0;
        bubble_sort(working, MAX_DATA, &res);
        bub_comp_total += res.comparisons; bub_move_total += res.movements;

        // Bubble Sort (Early Termination)
        copy_data(working, original, MAX_DATA);
        res.comparisons = 0; res.movements = 0;
        bubble_sort_early(working, MAX_DATA, &res);
        bub_e_comp_total += res.comparisons; bub_e_move_total += res.movements;

        // Insertion Sort
        copy_data(working, original, MAX_DATA);
        res.comparisons = 0; res.movements = 0;
        insertion_sort(working, MAX_DATA, &res, 1);
        ins_comp_total += res.comparisons; ins_move_total += res.movements;
    }

    // 출력 형식 맞추기
    printf("Selection Sort\n");
    printf("Total Comparisons   : %lld\n", sel_comp_total);
    printf("Average Comparisons : %.2f\n", (double)sel_comp_total / TRIALS);
    printf("Average Movements   : %.2f\n\n", (double)sel_move_total / TRIALS);

    printf("Bubble Sort\n");
    printf("Total Comparisons   : %lld\n", bub_comp_total);
    printf("Average Comparisons : %.2f\n", (double)bub_comp_total / TRIALS);
    printf("Average Movements   : %.2f\n\n", (double)bub_move_total / TRIALS);

    printf("Bubble Sort (Early Termination)\n");
    printf("Total Comparisons   : %lld\n", bub_e_comp_total);
    printf("Average Comparisons : %.2f\n", (double)bub_e_comp_total / TRIALS);
    printf("Average Movements   : %.2f\n\n", (double)bub_e_move_total / TRIALS);

    printf("Insertion Sort\n");
    printf("Total Comparisons   : %lld\n", ins_comp_total);
    printf("Average Comparisons : %.2f\n", (double)ins_comp_total / TRIALS);
    printf("Average Movements   : %.2f\n\n", (double)ins_move_total / TRIALS);

    printf("----------------------------------------\n\n");

    // ==========================================
    // 7. 입력 데이터의 정렬 상태에 따른 성능 비교
    // ==========================================
    Element base_data[MAX_DATA];
    generate_random_data(base_data, MAX_DATA);

    // 원본 데이터를 기준으로 각 상태 데이터 생성
    Element sorted_data[MAX_DATA], rev_data[MAX_DATA], part_data[MAX_DATA];
    copy_data(sorted_data, base_data, MAX_DATA);
    // 미리 정렬 수행해두기
    Result dummy = {0, 0};
    selection_sort(sorted_data, MAX_DATA, &dummy);

    make_reverse_sorted_data(rev_data, sorted_data, MAX_DATA);
    make_partially_sorted_data(part_data, sorted_data, MAX_DATA);

    // 상태별 테스트 수행 함수 포인터 대신 반복문 처리용 구조나 개별 출력
    // 여기서는 요구된 출력 형식에 맞게 각각 출력합니다.

    // 1) Sorted Data
    printf("Sorted Data\n\n");
    // Selection
    copy_data(working, sorted_data, MAX_DATA); res.comparisons = 0; res.movements = 0;
    selection_sort(working, MAX_DATA, &res);
    printf("Selection Sort\nComparisons : %lld\nMovements   : %lld\n\n", res.comparisons, res.movements);
    // Bubble
    copy_data(working, sorted_data, MAX_DATA); res.comparisons = 0; res.movements = 0;
    bubble_sort(working, MAX_DATA, &res);
    printf("Bubble Sort\nComparisons : %lld\nMovements   : %lld\n\n", res.comparisons, res.movements);
    // Bubble Early
    copy_data(working, sorted_data, MAX_DATA); res.comparisons = 0; res.movements = 0;
    bubble_sort_early(working, MAX_DATA, &res);
    printf("Bubble Sort (Early Termination)\nComparisons : %lld\nMovements   : %lld\n\n", res.comparisons, res.movements);
    // Insertion
    copy_data(working, sorted_data, MAX_DATA); res.comparisons = 0; res.movements = 0;
    insertion_sort(working, MAX_DATA, &res, 1);
    printf("Insertion Sort\nComparisons : %lld\nMovements   : %lld\n\n", res.comparisons, res.movements);

    printf("----------------------------------------\n\n");

    // 2) Reverse-Sorted Data
    printf("Reverse-Sorted Data\n\n");
    copy_data(working, rev_data, MAX_DATA); res.comparisons = 0; res.movements = 0;
    selection_sort(working, MAX_DATA, &res);
    printf("Selection Sort\nComparisons : %lld\nMovements   : %lld\n\n", res.comparisons, res.movements);

    copy_data(working, rev_data, MAX_DATA); res.comparisons = 0; res.movements = 0;
    bubble_sort(working, MAX_DATA, &res);
    printf("Bubble Sort\nComparisons : %lld\nMovements   : %lld\n\n", res.comparisons, res.movements);

    copy_data(working, rev_data, MAX_DATA); res.comparisons = 0; res.movements = 0;
    bubble_sort_early(working, MAX_DATA, &res);
    printf("Bubble Sort (Early Termination)\nComparisons : %lld\nMovements   : %lld\n\n", res.comparisons, res.movements);

    copy_data(working, rev_data, MAX_DATA); res.comparisons = 0; res.movements = 0;
    insertion_sort(working, MAX_DATA, &res, 1);
    printf("Insertion Sort\nComparisons : %lld\nMovements   : %lld\n\n", res.comparisons, res.movements);

    printf("----------------------------------------\n\n");

    // 3) Partially Sorted Data
    printf("Partially Sorted Data\n\n");
    copy_data(working, part_data, MAX_DATA); res.comparisons = 0; res.movements = 0;
    selection_sort(working, MAX_DATA, &res);
    printf("Selection Sort\nComparisons : %lld\nMovements   : %lld\n\n", res.comparisons, res.movements);

    copy_data(working, part_data, MAX_DATA); res.comparisons = 0; res.movements = 0;
    bubble_sort(working, MAX_DATA, &res);
    printf("Bubble Sort\nComparisons : %lld\nMovements   : %lld\n\n", res.comparisons, res.movements);

    copy_data(working, part_data, MAX_DATA); res.comparisons = 0; res.movements = 0;
    bubble_sort_early(working, MAX_DATA, &res);
    printf("Bubble Sort (Early Termination)\nComparisons : %lld\nMovements   : %lld\n\n", res.comparisons, res.movements);

    copy_data(working, part_data, MAX_DATA); res.comparisons = 0; res.movements = 0;
    insertion_sort(working, MAX_DATA, &res, 1);
    printf("Insertion Sort\nComparisons : %lld\nMovements   : %lld\n\n", res.comparisons, res.movements);

    printf("----------------------------------------\n\n");

    // ==========================================
    // 6. 정렬 알고리즘의 안정성(Stability) 검증
    // ==========================================
    printf("Stability Verification\n\n");

    // 임의의 테스트 데이터로 안정성 검증용 배열 생성 (Key 1이 같은 데이터 포함)
    Element stab_data[4] = {
        {527, 100, 0},
        {527, 200, 1},
        {125, 300, 2},
        {892, 400, 3}
    };

    Element temp_stab[4];

    // Selection Sort 안정성 검증
    copy_data(temp_stab, stab_data, 4);
    selection_sort(temp_stab, 4, &dummy);
    printf("Selection Sort        : %s\n", verify_stability(temp_stab, 4) ? "Stable" : "Unstable");

    // Bubble Sort 안정성 검증
    copy_data(temp_stab, stab_data, 4);
    bubble_sort(temp_stab, 4, &dummy);
    printf("Bubble Sort           : %s\n", verify_stability(temp_stab, 4) ? "Stable" : "Unstable");

    // Insertion Sort (Case A: >) 안정성 검증
    copy_data(temp_stab, stab_data, 4);
    insertion_sort(temp_stab, 4, &dummy, 1);
    printf("Insertion Sort (>)    : %s\n", verify_stability(temp_stab, 4) ? "Stable" : "Unstable");

    // Insertion Sort (Case B: >=) 안정성 검증
    copy_data(temp_stab, stab_data, 4);
    insertion_sort(temp_stab, 4, &dummy, 0);
    printf("Insertion Sort (>=)   : %s\n", verify_stability(temp_stab, 4) ? "Stable" : "Unstable");

    return 0;
}

// ----------------------------------------------------
// 유틸리티 함수 구현
// ----------------------------------------------------
void generate_random_data(Element data[], int n) {
    for (int i = 0; i < n; i++) {
        data[i].key1 = rand() % 1000001; // 0 ~ 1,000,000
        data[i].key2 = rand() % 1000001;
        data[i].original_index = i;      // 안정성 검증용 원래 순서 기록
    }
}

void copy_data(Element dest[], Element src[], int n) {
    for (int i = 0; i < n; i++) {
        dest[i] = src[i];
    }
}

void check_sorted(Element data[], int n, const char* sort_name) {
    for (int i = 0; i < n - 1; i++) {
        if (data[i].key1 > data[i+1].key1) {
            printf("[Error] %s failed to sort correctly!\n", sort_name);
            return;
        }
    }
}

// ----------------------------------------------------
// 정렬 알고리즘 구현 (비교 및 이동 횟수 측정 포함)
// ----------------------------------------------------

// 1. 선택 정렬 (Selection Sort)
void selection_sort(Element data[], int n, Result* res) {
    for (int i = 0; i < n - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < n; j++) {
            res->comparisons++;
            if (data[j].key1 < data[min_idx].key1) {
                min_idx = j;
            }
        }
        if (min_idx != i) {
            // 교환 연산: 임시 변수 사용 (이동 3회)
            Element temp = data[i]; res->movements++;
            data[i] = data[min_idx]; res->movements++;
            data[min_idx] = temp; res->movements++;
        }
    }
}

// 2. 버블 정렬 (Bubble Sort)
void bubble_sort(Element data[], int n, Result* res) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - 1 - i; j++) {
            res->comparisons++;
            if (data[j].key1 > data[j+1].key1) {
                Element temp = data[j]; res->movements++;
                data[j] = data[j+1]; res->movements++;
                data[j+1] = temp; res->movements++;
            }
        }
    }
}

// 2-1. 조기 종료 조건이 추가된 버블 정렬 (Bubble Sort with Early Termination)
void bubble_sort_early(Element data[], int n, Result* res) {
    for (int i = 0; i < n - 1; i++) {
        int swapped = 0;
        for (int j = 0; j < n - 1 - i; j++) {
            res->comparisons++;
            if (data[j].key1 > data[j+1].key1) {
                Element temp = data[j]; res->movements++;
                data[j] = data[j+1]; res->movements++;
                data[j+1] = temp; res->movements++;
                swapped = 1;
            }
        }
        if (!swapped) break; // 교환이 발생하지 않았다면 종료
    }
}

// 3. 삽입 정렬 (Insertion Sort)
// strict_greater가 1이면 '>' (안정 정렬), 0이면 '>=' (불안정 정렬 유도)
void insertion_sort(Element data[], int n, Result* res, int strict_greater) {
    for (int i = 1; i < n; i++) {
        Element key = data[i];
        res->movements++; // key 복사
        int j = i - 1;

        if (strict_greater) {
            while (j >= 0) {
                res->comparisons++;
                if (data[j].key1 > key.key1) {
                    data[j+1] = data[j];
                    res->movements++;
                    j--;
                } else {
                    break;
                }
            }
        } else {
            while (j >= 0) {
                res->comparisons++;
                if (data[j].key1 >= key.key1) { // 크거나 같으면 이동하므로 안정성 깨짐
                    data[j+1] = data[j];
                    res->movements++;
                    j--;
                } else {
                    break;
                }
            }
        }
        data[j+1] = key;
        res->movements++; // 제자리에 삽입
    }
}

// ----------------------------------------------------
// 입력 상태 생성 함수
// ----------------------------------------------------
void make_reverse_sorted_data(Element dest[], Element src[], int n) {
    for (int i = 0; i < n; i++) {
        dest[i] = src[n - 1 - i];
    }
}

void make_partially_sorted_data(Element dest[], Element src[], int n) {
    copy_data(dest, src, n);
    // 전체 데이터 중 일부(예: 10% 정도)의 원소 위치를 무작위로 교환하여 일부만 정렬된 상태로 만듦
    int swaps = n / 10;
    for (int i = 0; i < swaps; i++) {
        int idx1 = rand() % n;
        int idx2 = rand() % n;
        Element temp = dest[idx1];
        dest[idx1] = dest[idx2];
        dest[idx2] = temp;
    }
}

// ----------------------------------------------------
// 안정성 검증 함수
// ----------------------------------------------------
// 첫 번째 키가 같은 항목들끼리 원래 입력 순서(original_index)가 유지되었는지 확인
int verify_stability(Element sorted[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (sorted[i].key1 == sorted[j].key1) {
                if (sorted[i].original_index > sorted[j].original_index) {
                    return 0; // 순서가 역전되었으므로 불안정(Unstable)
                }
            }
        }
    }
    return 1; // 안정(Stable)
}
