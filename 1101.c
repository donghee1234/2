#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_VALUE 1000000
#define SIZE_COUNT 6

/* 비교 횟수 (정렬 함수 안에서 데이터 값끼리 비교할 때만 증가시킨다) */
static long long g_cmp;

/* ---------- 난수 (같은 시드면 어느 환경에서나 같은 데이터가 나온다) ---------- */
static unsigned long long rngState;

unsigned int nextRand(void) {
    rngState = rngState * 6364136223846793005ULL + 1442695040888963407ULL;
    return (unsigned int)(rngState >> 33);
}

int randInt(int n) {
    return (int)(nextRand() % (unsigned int)n);
}

/* ---------- 삽입 정렬 ---------- */
void insertionSort(int a[], int n) {
    for (int i = 1; i < n; i++) {
        int current = a[i];
        int j = i - 1;
        while (j >= 0) {
            g_cmp++;                          /* a[j] 와 current 비교 */
            if (a[j] > current) {
                a[j + 1] = a[j];
                j--;
            } else {
                break;
            }
        }
        a[j + 1] = current;
    }
}

/* ---------- 쉘 정렬의 한 단계: 간격 h 로 떨어진 원소끼리 삽입 정렬 ---------- */
void gappedInsertionSort(int a[], int n, int h) {
    for (int i = h; i < n; i++) {
        int current = a[i];
        int j = i - h;
        while (j >= 0) {
            g_cmp++;
            if (a[j] > current) {
                a[j + h] = a[j];
                j -= h;
            } else {
                break;
            }
        }
        a[j + h] = current;
    }
}

/* 기본 간격: n/2, n/4, n/8, ..., 1 */
void shellSortOriginal(int a[], int n) {
    for (int h = n / 2; h >= 1; h /= 2) {
        gappedInsertionSort(a, n, h);
    }
}

/* Knuth 간격: h(1) = 1, h(k+1) = 3h(k) + 1  (1, 4, 13, 40, 121, ...)
 * 데이터 크기보다 작은 가장 큰 간격부터 시작해서 역순으로 적용한다. */
void shellSortKnuth(int a[], int n) {
    int h = 1;
    while (3 * h + 1 < n) h = 3 * h + 1;
    for (; h >= 1; h /= 3) {              /* 3k+1 꼴이라 h/3 이 바로 이전 간격이 된다 */
        gappedInsertionSort(a, n, h);
    }
}

/* ---------- 간격 수열 출력용 (정렬과 무관) ---------- */
int countOriginalSteps(int n) {
    int count = 0;
    for (int h = n / 2; h >= 1; h /= 2) count++;
    return count;
}

int countKnuthSteps(int n) {
    int h = 1, count = 0;
    while (3 * h + 1 < n) h = 3 * h + 1;
    for (; h >= 1; h /= 3) count++;
    return count;
}

int isSortedAscending(const int a[], int n) {
    for (int i = 0; i + 1 < n; i++) {
        if (a[i] > a[i + 1]) return 0;
    }
    return 1;
}

int main(int argc, char* argv[]) {
    unsigned int seed = (argc > 1) ? (unsigned int)atoi(argv[1]) : (unsigned int)time(NULL);
    rngState = seed;

    int sizes[SIZE_COUNT] = { 1000, 2000, 4000, 8000, 16000, 20000 };
    long long result[SIZE_COUNT][3];      /* [크기][0=삽입, 1=쉘 기본, 2=쉘 Knuth] */

    static int original[20000], work[20000], first[20000];

    printf("Seed : %u\n\n", seed);

    for (int s = 0; s < SIZE_COUNT; s++) {
        int n = sizes[s];

        /* 원본 데이터 생성 (비교 횟수에 포함하지 않음) */
        for (int i = 0; i < n; i++) original[i] = randInt(MAX_VALUE + 1);

        for (int k = 0; k < 3; k++) {
            memcpy(work, original, sizeof(int) * n);   /* 모든 알고리즘이 같은 원본의 복사본 사용 */
            g_cmp = 0;
            if (k == 0) insertionSort(work, n);
            else if (k == 1) shellSortOriginal(work, n);
            else shellSortKnuth(work, n);
            result[s][k] = g_cmp;

            /* 결과 검증: 오름차순이고, 세 방법의 결과가 서로 같아야 한다 */
            if (!isSortedAscending(work, n)) {
                printf("ERROR: result is not sorted (n=%d, method=%d)\n", n, k);
                return 1;
            }
            if (k == 0) memcpy(first, work, sizeof(int) * n);
            else if (memcmp(first, work, sizeof(int) * n) != 0) {
                printf("ERROR: results differ between methods (n=%d)\n", n);
                return 1;
            }
        }

        printf("Data Size : %d\n\n", n);
        printf("%-21s: %lld comparisons\n", "Insertion Sort", result[s][0]);
        printf("%-21s: %lld comparisons\n", "Shell Sort (Original)", result[s][1]);
        printf("%-21s: %lld comparisons\n\n", "Shell Sort (Knuth)", result[s][2]);
    }

    /* ---------- 추가 정보: 분석에 사용한 요약 ---------- */
    printf("All results are sorted in ascending order: OK\n\n");

    printf("[Summary]\n");
    printf("%-8s %-14s %-14s %-14s %-8s %-8s\n",
           "n", "Insertion", "Shell(Orig)", "Shell(Knuth)", "Steps(O)", "Steps(K)");
    for (int s = 0; s < SIZE_COUNT; s++) {
        printf("%-8d %-14lld %-14lld %-14lld %-8d %-8d\n", sizes[s],
               result[s][0], result[s][1], result[s][2],
               countOriginalSteps(sizes[s]), countKnuthSteps(sizes[s]));
    }

    printf("\n[Growth ratio when n doubles (n -> 2n)]\n");
    printf("%-12s %-12s %-14s %-14s\n", "n -> 2n", "Insertion", "Shell(Orig)", "Shell(Knuth)");
    for (int s = 0; s + 1 < SIZE_COUNT - 1; s++) {
        printf("%-5d->%-6d %-12.2f %-14.2f %-14.2f\n", sizes[s], sizes[s + 1],
               (double)result[s + 1][0] / result[s][0],
               (double)result[s + 1][1] / result[s][1],
               (double)result[s + 1][2] / result[s][2]);
    }

    printf("\n[Comparisons ratio: Insertion / Shell]\n");
    printf("%-8s %-16s %-16s\n", "n", "Insertion/Orig", "Insertion/Knuth");
    for (int s = 0; s < SIZE_COUNT; s++) {
        printf("%-8d %-16.1f %-16.1f\n", sizes[s],
               (double)result[s][0] / result[s][1], (double)result[s][0] / result[s][2]);
    }

    return 0;
}
