/*
 * Test: Binary search and sorting algorithms
 * Tests: binary search, quicksort, merge operations
 */
#include <stdio.h>

/* Binary search - returns index or -1 */
int binary_search(int *arr, int n, int target) {
    int left, right, mid;
    left = 0;
    right = n - 1;
    while (left <= right) {
        mid = left + (right - left) / 2;
        if (arr[mid] == target) {
            return mid;
        }
        if (arr[mid] < target) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return -1;
}

/* Recursive binary search */
int binary_search_rec(int *arr, int left, int right, int target) {
    int mid;
    if (left > right) {
        return -1;
    }
    mid = left + (right - left) / 2;
    if (arr[mid] == target) {
        return mid;
    }
    if (arr[mid] < target) {
        return binary_search_rec(arr, mid + 1, right, target);
    }
    return binary_search_rec(arr, left, mid - 1, target);
}

/* Swap helper */
void swap(int *a, int *b) {
    int tmp;
    tmp = *a;
    *a = *b;
    *b = tmp;
}

/* Partition for quicksort */
int partition(int *arr, int low, int high) {
    int pivot, i, j;
    pivot = arr[high];
    i = low - 1;
    for (j = low; j < high; j++) {
        if (arr[j] <= pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return i + 1;
}

/* Quicksort */
void quicksort(int *arr, int low, int high) {
    int pi;
    if (low < high) {
        pi = partition(arr, low, high);
        quicksort(arr, low, pi - 1);
        quicksort(arr, pi + 1, high);
    }
}

/* Insertion sort */
void insertion_sort(int *arr, int n) {
    int i, j, key;
    for (i = 1; i < n; i++) {
        key = arr[i];
        j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

/* Selection sort */
void selection_sort(int *arr, int n) {
    int i, j, min_idx;
    for (i = 0; i < n - 1; i++) {
        min_idx = i;
        for (j = i + 1; j < n; j++) {
            if (arr[j] < arr[min_idx]) {
                min_idx = j;
            }
        }
        if (min_idx != i) {
            swap(&arr[i], &arr[min_idx]);
        }
    }
}

/* Check if array is sorted */
int is_sorted(int *arr, int n) {
    int i;
    for (i = 0; i < n - 1; i++) {
        if (arr[i] > arr[i + 1]) {
            return 0;
        }
    }
    return 1;
}

/* Find lower bound (first element >= target) */
int lower_bound(int *arr, int n, int target) {
    int left, right, mid;
    left = 0;
    right = n;
    while (left < right) {
        mid = left + (right - left) / 2;
        if (arr[mid] < target) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    return left;
}

/* Find upper bound (first element > target) */
int upper_bound(int *arr, int n, int target) {
    int left, right, mid;
    left = 0;
    right = n;
    while (left < right) {
        mid = left + (right - left) / 2;
        if (arr[mid] <= target) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    return left;
}

/* Count occurrences using binary search */
int count_occurrences(int *arr, int n, int target) {
    int lb, ub;
    lb = lower_bound(arr, n, target);
    ub = upper_bound(arr, n, target);
    return ub - lb;
}

int main(void) {
    int arr[20];
    int arr2[10];
    int i, result;
    int passed;

    passed = 1;

    /* Test binary search on sorted array */
    for (i = 0; i < 10; i++) {
        arr[i] = i * 2;  /* 0, 2, 4, 6, 8, 10, 12, 14, 16, 18 */
    }

    result = binary_search(arr, 10, 8);
    if (result != 4) {
        printf("FAIL: binary_search(8) = %d (expected 4)\n", result);
        passed = 0;
    }

    result = binary_search(arr, 10, 0);
    if (result != 0) {
        printf("FAIL: binary_search(0) = %d (expected 0)\n", result);
        passed = 0;
    }

    result = binary_search(arr, 10, 18);
    if (result != 9) {
        printf("FAIL: binary_search(18) = %d (expected 9)\n", result);
        passed = 0;
    }

    result = binary_search(arr, 10, 7);
    if (result != -1) {
        printf("FAIL: binary_search(7) = %d (expected -1)\n", result);
        passed = 0;
    }

    /* Test recursive binary search */
    result = binary_search_rec(arr, 0, 9, 12);
    if (result != 6) {
        printf("FAIL: binary_search_rec(12) = %d (expected 6)\n", result);
        passed = 0;
    }

    /* Test quicksort */
    arr2[0] = 64;
    arr2[1] = 34;
    arr2[2] = 25;
    arr2[3] = 12;
    arr2[4] = 22;
    arr2[5] = 11;
    arr2[6] = 90;
    arr2[7] = 45;
    arr2[8] = 33;
    arr2[9] = 77;

    quicksort(arr2, 0, 9);
    if (!is_sorted(arr2, 10)) {
        printf("FAIL: quicksort did not sort correctly\n");
        passed = 0;
    }
    if (arr2[0] != 11 || arr2[9] != 90) {
        printf("FAIL: quicksort: arr2[0]=%d, arr2[9]=%d\n", arr2[0], arr2[9]);
        passed = 0;
    }

    /* Test insertion sort */
    arr2[0] = 5;
    arr2[1] = 2;
    arr2[2] = 9;
    arr2[3] = 1;
    arr2[4] = 7;

    insertion_sort(arr2, 5);
    if (!is_sorted(arr2, 5)) {
        printf("FAIL: insertion_sort did not sort correctly\n");
        passed = 0;
    }
    if (arr2[0] != 1 || arr2[4] != 9) {
        printf("FAIL: insertion_sort: arr2[0]=%d, arr2[4]=%d\n", arr2[0], arr2[4]);
        passed = 0;
    }

    /* Test selection sort */
    arr2[0] = 8;
    arr2[1] = 3;
    arr2[2] = 6;
    arr2[3] = 2;
    arr2[4] = 4;

    selection_sort(arr2, 5);
    if (!is_sorted(arr2, 5)) {
        printf("FAIL: selection_sort did not sort correctly\n");
        passed = 0;
    }

    /* Test lower_bound and upper_bound */
    /* arr = {1, 2, 2, 2, 3, 4, 5} */
    arr[0] = 1;
    arr[1] = 2;
    arr[2] = 2;
    arr[3] = 2;
    arr[4] = 3;
    arr[5] = 4;
    arr[6] = 5;

    result = lower_bound(arr, 7, 2);
    if (result != 1) {
        printf("FAIL: lower_bound(2) = %d (expected 1)\n", result);
        passed = 0;
    }

    result = upper_bound(arr, 7, 2);
    if (result != 4) {
        printf("FAIL: upper_bound(2) = %d (expected 4)\n", result);
        passed = 0;
    }

    /* Test count_occurrences */
    result = count_occurrences(arr, 7, 2);
    if (result != 3) {
        printf("FAIL: count_occurrences(2) = %d (expected 3)\n", result);
        passed = 0;
    }

    result = count_occurrences(arr, 7, 1);
    if (result != 1) {
        printf("FAIL: count_occurrences(1) = %d (expected 1)\n", result);
        passed = 0;
    }

    result = count_occurrences(arr, 7, 99);
    if (result != 0) {
        printf("FAIL: count_occurrences(99) = %d (expected 0)\n", result);
        passed = 0;
    }

    if (passed) {
        printf("PASS: t_binary_search\n");
    }
    return passed ? 0 : 1;
}
