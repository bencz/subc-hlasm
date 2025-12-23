/*
 * Test: Callback patterns
 * Tests: function pointers as callbacks, event handlers, comparators
 */
#include <stdio.h>
#include <string.h>

/* Callback type definitions */
typedef int (*Comparator)(void *, void *);
typedef void (*Visitor)(int);
typedef int (*Transformer)(int);
typedef int (*Predicate)(int);

/* Global for tracking callback invocations */
int g_visit_sum;
int g_visit_count;

/* Visitor callback */
void sum_visitor(int value) {
    g_visit_sum += value;
    g_visit_count++;
}

void print_visitor(int value) {
    printf("  visited: %d\n", value);
}

/* Transformer callbacks */
int double_it(int x) { return x * 2; }
int square_it(int x) { return x * x; }
int negate_it(int x) { return -x; }

/* Predicate callbacks */
int is_positive(int x) { return x > 0; }
int is_even_pred(int x) { return (x % 2) == 0; }
int is_greater_than_5(int x) { return x > 5; }

/* Comparator callbacks */
int int_compare_asc(void *a, void *b) {
    int ia, ib;
    ia = *(int *)a;
    ib = *(int *)b;
    if (ia < ib) return -1;
    if (ia > ib) return 1;
    return 0;
}

int int_compare_desc(void *a, void *b) {
    int ia, ib;
    ia = *(int *)a;
    ib = *(int *)b;
    if (ia > ib) return -1;
    if (ia < ib) return 1;
    return 0;
}

/* Higher-order functions */
void foreach(int *arr, int n, Visitor visit) {
    int i;
    for (i = 0; i < n; i++) {
        visit(arr[i]);
    }
}

void map_inplace(int *arr, int n, Transformer transform) {
    int i;
    for (i = 0; i < n; i++) {
        arr[i] = transform(arr[i]);
    }
}

int filter_count(int *arr, int n, Predicate pred) {
    int i, count;
    count = 0;
    for (i = 0; i < n; i++) {
        if (pred(arr[i])) {
            count++;
        }
    }
    return count;
}

int reduce(int *arr, int n, int initial, int (*reducer)(int, int)) {
    int i, acc;
    acc = initial;
    for (i = 0; i < n; i++) {
        acc = reducer(acc, arr[i]);
    }
    return acc;
}

int add_reducer(int acc, int val) { return acc + val; }
int mul_reducer(int acc, int val) { return acc * val; }
int max_reducer(int acc, int val) { return (val > acc) ? val : acc; }

/* Simple bubble sort with comparator */
void bubble_sort(int *arr, int n, Comparator cmp) {
    int i, j, tmp;
    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - i - 1; j++) {
            if (cmp(&arr[j], &arr[j + 1]) > 0) {
                tmp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = tmp;
            }
        }
    }
}

/* Find with predicate */
int find_first(int *arr, int n, Predicate pred) {
    int i;
    for (i = 0; i < n; i++) {
        if (pred(arr[i])) {
            return i;
        }
    }
    return -1;
}

int main(void) {
    int arr[10];
    int arr2[5];
    int i, result;
    int passed;

    passed = 1;

    /* Initialize array */
    for (i = 0; i < 10; i++) {
        arr[i] = i + 1;
    }
    /* arr = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10} */

    /* Test foreach with visitor */
    g_visit_sum = 0;
    g_visit_count = 0;
    foreach(arr, 10, sum_visitor);
    if (g_visit_sum != 55 || g_visit_count != 10) {
        printf("FAIL: foreach sum=%d, count=%d\n", g_visit_sum, g_visit_count);
        passed = 0;
    }

    /* Test map with transformer */
    arr2[0] = 1;
    arr2[1] = 2;
    arr2[2] = 3;
    arr2[3] = 4;
    arr2[4] = 5;
    
    map_inplace(arr2, 5, double_it);
    /* arr2 = {2, 4, 6, 8, 10} */
    if (arr2[0] != 2 || arr2[4] != 10) {
        printf("FAIL: map double: arr2[0]=%d, arr2[4]=%d\n", arr2[0], arr2[4]);
        passed = 0;
    }

    map_inplace(arr2, 5, square_it);
    /* arr2 = {4, 16, 36, 64, 100} */
    if (arr2[2] != 36) {
        printf("FAIL: map square: arr2[2]=%d (expected 36)\n", arr2[2]);
        passed = 0;
    }

    /* Test filter_count with predicate */
    for (i = 0; i < 10; i++) {
        arr[i] = i + 1;
    }
    
    result = filter_count(arr, 10, is_even_pred);
    if (result != 5) {
        printf("FAIL: filter_count even = %d (expected 5)\n", result);
        passed = 0;
    }

    result = filter_count(arr, 10, is_greater_than_5);
    if (result != 5) {
        printf("FAIL: filter_count >5 = %d (expected 5)\n", result);
        passed = 0;
    }

    /* Test reduce */
    result = reduce(arr, 10, 0, add_reducer);
    if (result != 55) {
        printf("FAIL: reduce add = %d (expected 55)\n", result);
        passed = 0;
    }

    arr2[0] = 1;
    arr2[1] = 2;
    arr2[2] = 3;
    arr2[3] = 4;
    arr2[4] = 5;
    result = reduce(arr2, 5, 1, mul_reducer);
    /* 1*2*3*4*5 = 120 */
    if (result != 120) {
        printf("FAIL: reduce mul = %d (expected 120)\n", result);
        passed = 0;
    }

    result = reduce(arr, 10, 0, max_reducer);
    if (result != 10) {
        printf("FAIL: reduce max = %d (expected 10)\n", result);
        passed = 0;
    }

    /* Test sort with comparator */
    arr2[0] = 5;
    arr2[1] = 2;
    arr2[2] = 8;
    arr2[3] = 1;
    arr2[4] = 9;

    bubble_sort(arr2, 5, int_compare_asc);
    if (arr2[0] != 1 || arr2[4] != 9) {
        printf("FAIL: sort asc: arr2[0]=%d, arr2[4]=%d\n", arr2[0], arr2[4]);
        passed = 0;
    }

    bubble_sort(arr2, 5, int_compare_desc);
    if (arr2[0] != 9 || arr2[4] != 1) {
        printf("FAIL: sort desc: arr2[0]=%d, arr2[4]=%d\n", arr2[0], arr2[4]);
        passed = 0;
    }

    /* Test find_first */
    for (i = 0; i < 10; i++) {
        arr[i] = i - 3;
    }
    /* arr = {-3, -2, -1, 0, 1, 2, 3, 4, 5, 6} */

    result = find_first(arr, 10, is_positive);
    if (result != 4) {
        printf("FAIL: find_first positive = %d (expected 4)\n", result);
        passed = 0;
    }

    result = find_first(arr, 10, is_greater_than_5);
    if (result != 9) {
        printf("FAIL: find_first >5 = %d (expected 9)\n", result);
        passed = 0;
    }

    if (passed) {
        printf("PASS: t_callback\n");
    }
    return passed ? 0 : 1;
}
