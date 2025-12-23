/*
 * Test: Complex array operations
 * Tests: array of pointers, pointer to array, array arithmetic
 */
#include <stdio.h>
#include <string.h>

int g_matrix[3][3];

/* Function to sum array elements */
int sum_array(int *arr, int n) {
    int i, sum;
    sum = 0;
    for (i = 0; i < n; i++) {
        sum += arr[i];
    }
    return sum;
}

/* Function with array of pointers */
int sum_ptr_array(int **ptrs, int n) {
    int i, sum;
    sum = 0;
    for (i = 0; i < n; i++) {
        sum += *ptrs[i];
    }
    return sum;
}

/* Reverse array in place */
void reverse_array(int *arr, int n) {
    int i, j, tmp;
    i = 0;
    j = n - 1;
    while (i < j) {
        tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
        i++;
        j--;
    }
}

/* Find max in array */
int find_max(int *arr, int n) {
    int i, max;
    max = arr[0];
    for (i = 1; i < n; i++) {
        if (arr[i] > max) {
            max = arr[i];
        }
    }
    return max;
}

int main(void) {
    int arr[10];
    int *ptrs[5];
    int vals[5];
    int i, j, sum, result;
    int passed;
    char *strings[4];
    int matrix[3][4];

    passed = 1;

    /* Initialize array with pattern */
    for (i = 0; i < 10; i++) {
        arr[i] = (i + 1) * 10;
    }
    /* arr = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100} */

    /* Sum array */
    sum = sum_array(arr, 10);
    if (sum != 550) {
        printf("FAIL: sum_array = %d (expected 550)\n", sum);
        passed = 0;
    }

    /* Find max */
    result = find_max(arr, 10);
    if (result != 100) {
        printf("FAIL: find_max = %d (expected 100)\n", result);
        passed = 0;
    }

    /* Reverse array */
    reverse_array(arr, 10);
    if (arr[0] != 100 || arr[9] != 10) {
        printf("FAIL: reverse: arr[0]=%d, arr[9]=%d\n", arr[0], arr[9]);
        passed = 0;
    }

    /* Array of pointers */
    for (i = 0; i < 5; i++) {
        vals[i] = (i + 1) * 100;
        ptrs[i] = &vals[i];
    }

    sum = sum_ptr_array(ptrs, 5);
    /* 100 + 200 + 300 + 400 + 500 = 1500 */
    if (sum != 1500) {
        printf("FAIL: sum_ptr_array = %d (expected 1500)\n", sum);
        passed = 0;
    }

    /* Modify through pointer array */
    *ptrs[2] = 999;
    if (vals[2] != 999) {
        printf("FAIL: vals[2] = %d after *ptrs[2]=999\n", vals[2]);
        passed = 0;
    }

    /* Array of strings */
    strings[0] = "apple";
    strings[1] = "banana";
    strings[2] = "cherry";
    strings[3] = "date";

    if (strcmp(strings[1], "banana") != 0) {
        printf("FAIL: strings[1] = %s\n", strings[1]);
        passed = 0;
    }

    /* Find string by iteration */
    result = -1;
    for (i = 0; i < 4; i++) {
        if (strcmp(strings[i], "cherry") == 0) {
            result = i;
            break;
        }
    }
    if (result != 2) {
        printf("FAIL: find 'cherry' = %d (expected 2)\n", result);
        passed = 0;
    }

    /* 2D array (simulated with 1D) */
    /* matrix[i][j] stored as matrix[i * cols + j] */
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 4; j++) {
            matrix[i][j] = i * 4 + j;
        }
    }

    /* Verify 2D access */
    if (matrix[0][0] != 0 || matrix[1][2] != 6 || matrix[2][3] != 11) {
        printf("FAIL: matrix[1][2]=%d, matrix[2][3]=%d\n", 
               matrix[1][2], matrix[2][3]);
        passed = 0;
    }

    /* Sum 2D array */
    sum = 0;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 4; j++) {
            sum += matrix[i][j];
        }
    }
    /* 0+1+2+...+11 = 66 */
    if (sum != 66) {
        printf("FAIL: 2D sum = %d (expected 66)\n", sum);
        passed = 0;
    }

    /* Pointer arithmetic on array */
    for (i = 0; i < 10; i++) {
        arr[i] = i;
    }
    
    /* *(arr + 5) should equal arr[5] */
    if (*(arr + 5) != arr[5]) {
        printf("FAIL: *(arr+5) != arr[5]\n");
        passed = 0;
    }

    /* arr + 3 should point to arr[3] */
    {
        int *p;
        p = arr + 3;
        if (*p != 3) {
            printf("FAIL: *(arr+3) = %d (expected 3)\n", *p);
            passed = 0;
        }
        
        /* Increment pointer */
        p++;
        if (*p != 4) {
            printf("FAIL: after p++, *p = %d (expected 4)\n", *p);
            passed = 0;
        }
    }

    /* Negative index via pointer */
    {
        int *p;
        p = arr + 5;
        if (p[-2] != 3) {
            printf("FAIL: p[-2] = %d (expected 3)\n", p[-2]);
            passed = 0;
        }
    }

    if (passed) {
        printf("PASS: t_array_complex\n");
    }
    return passed ? 0 : 1;
}
