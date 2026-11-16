#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define FRAME_OVERHEAD 16

long long comparisons;
int depth, max_depth;
int pivot_type;

void swap(int* a, int* b) {
    int t = *a;
    *a = *b;
    *b = t;
}

double now_ms() {
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1000000.0;
}

/* ---------- 힙 정렬 ---------- */
void sift_down(int* arr, int start, int end) {
    int root = start;
    while (root * 2 + 1 <= end) {
        int child = root * 2 + 1;
        if (child + 1 <= end) {
            comparisons++;
            if (arr[child] < arr[child + 1]) child++;
        }
        comparisons++;
        if (arr[root] < arr[child]) {
            swap(&arr[root], &arr[child]);
            root = child;
        } else {
            return;
        }
    }
}

void heap_sort(int* arr, int n) {
    for (int start = (n - 2) / 2; start >= 0; start--) {
        sift_down(arr, start, n - 1);
    }
    for (int end = n - 1; end > 0; end--) {
        swap(&arr[0], &arr[end]);
        sift_down(arr, 0, end - 1);
    }
}

/* ---------- 병합 정렬 ---------- */
void merge(int* arr, int* tmp, int left, int mid, int right) {
    int i = left, j = mid + 1, k = left;
    while (i <= mid && j <= right) {
        comparisons++;
        if (arr[i] <= arr[j]) tmp[k++] = arr[i++];
        else tmp[k++] = arr[j++];
    }
    while (i <= mid) tmp[k++] = arr[i++];
    while (j <= right) tmp[k++] = arr[j++];
    for (i = left; i <= right; i++) arr[i] = tmp[i];
}

void merge_sort(int* arr, int* tmp, int left, int right) {
    depth++;
    if (depth > max_depth) max_depth = depth;
    if (left < right) {
        int mid = (left + right) / 2;
        merge_sort(arr, tmp, left, mid);
        merge_sort(arr, tmp, mid + 1, right);
        merge(arr, tmp, left, mid, right);
    }
    depth--;
}

void merge_sort_main(int* arr, int n) {
    int* tmp = (int*)malloc(n * sizeof(int));
    merge_sort(arr, tmp, 0, n - 1);
    free(tmp);
}

/* ---------- 퀵 정렬 ---------- */
void choose_pivot(int* arr, int low, int high) {
    if (pivot_type == 1) {
        int r = low + rand() % (high - low + 1);
        swap(&arr[low], &arr[r]);
    } else if (pivot_type == 2) {
        int mid = (low + high) / 2;
        comparisons++;
        if (arr[low] > arr[mid]) swap(&arr[low], &arr[mid]);
        comparisons++;
        if (arr[low] > arr[high]) swap(&arr[low], &arr[high]);
        comparisons++;
        if (arr[mid] > arr[high]) swap(&arr[mid], &arr[high]);
        swap(&arr[low], &arr[mid]);
    }
}

int partition(int* arr, int low, int high) {
    int pivot = arr[low];
    int i = low;
    for (int j = low + 1; j <= high; j++) {
        comparisons++;
        if (arr[j] < pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[low], &arr[i]);
    return i;
}

void quick_sort(int* arr, int low, int high) {
    depth++;
    if (depth > max_depth) max_depth = depth;
    if (low < high) {
        choose_pivot(arr, low, high);
        int p = partition(arr, low, high);
        quick_sort(arr, low, p - 1);
        quick_sort(arr, p + 1, high);
    }
    depth--;
}

/* ---------- 측정 ---------- */
int is_sorted(int* arr, int n) {
    for (int i = 0; i + 1 < n; i++) {
        if (arr[i] > arr[i + 1]) return 0;
    }
    return 1;
}

void run(const char* name, int type, int* orig, int n) {
    int* arr = (int*)malloc(n * sizeof(int));
    memcpy(arr, orig, n * sizeof(int));

    comparisons = 0;
    depth = 0;
    max_depth = 0;
    srand(777);

    size_t swap_frame = 2 * sizeof(int*) + sizeof(int) + FRAME_OVERHEAD;
    size_t memory = 0;

    double start = now_ms();
    if (type == 0) {
        heap_sort(arr, n);
    } else if (type == 1) {
        merge_sort_main(arr, n);
    } else {
        pivot_type = type - 2;
        quick_sort(arr, 0, n - 1);
    }
    double end = now_ms();

    if (type == 0) {
        size_t heap_sort_frame = sizeof(int*) + 2 * sizeof(int) + FRAME_OVERHEAD;
        size_t sift_frame = sizeof(int*) + 4 * sizeof(int) + FRAME_OVERHEAD;
        memory = heap_sort_frame + sift_frame + swap_frame;
    } else if (type == 1) {
        size_t ms_frame = 2 * sizeof(int*) + 3 * sizeof(int) + FRAME_OVERHEAD;
        size_t merge_frame = 2 * sizeof(int*) + 8 * sizeof(int) + FRAME_OVERHEAD;
        memory = (size_t)n * sizeof(int) + (size_t)max_depth * ms_frame + merge_frame;
    } else {
        size_t qs_frame = sizeof(int*) + 3 * sizeof(int) + FRAME_OVERHEAD;
        size_t part_frame = sizeof(int*) + 5 * sizeof(int) + FRAME_OVERHEAD;
        memory = (size_t)max_depth * qs_frame + part_frame + swap_frame;
    }

    printf("%-16s%-15lld%-12.3f%zu", name, comparisons, end - start, memory);
    if (!is_sorted(arr, n)) printf("   (정렬 오류!)");
    printf("\n");

    free(arr);
}

int main() {
    int sizes[] = {1000, 2000, 4000, 8000, 16000, 20000};
    int count = sizeof(sizes) / sizeof(sizes[0]);

    srand(12345);

    for (int s = 0; s < count; s++) {
        int n = sizes[s];
        int* orig = (int*)malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) {
            orig[i] = rand() % 100000;
        }

        printf("Data Size : %d\n\n", n);
        printf("%-16s%-15s%-12s%s\n", "Algorithm", "Comparisons", "Time(ms)", "Memory(Bytes)");
        printf("------------------------------------------------------\n");
        run("Heap Sort", 0, orig, n);
        run("Merge Sort", 1, orig, n);
        run("Quick (First)", 2, orig, n);
        run("Quick (Random)", 3, orig, n);
        run("Quick (Median3)", 4, orig, n);
        printf("\n");

        free(orig);
    }

    return 0;
}
