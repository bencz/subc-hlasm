/*
 * Test: Typedef
 * Tests: typedef for basic types and structs
 */
#include <stdio.h>

typedef int Integer;
typedef char Byte;
typedef int *IntPtr;

struct point {
    int x;
    int y;
};

typedef struct point Point;

int main(void) {
    Integer i;
    Byte b;
    IntPtr p;
    Point pt;
    int val;
    int passed;

    passed = 1;

    /* Typedef for int */
    i = 42;
    if (i != 42) {
        printf("FAIL: Integer i = %d (expected 42)\n", i);
        passed = 0;
    }

    /* Typedef for char */
    b = 'X';
    if (b != 'X') {
        printf("FAIL: Byte b = %c (expected X)\n", b);
        passed = 0;
    }

    /* Typedef for pointer */
    val = 100;
    p = &val;
    if (*p != 100) {
        printf("FAIL: *IntPtr = %d (expected 100)\n", *p);
        passed = 0;
    }

    /* Typedef for struct */
    pt.x = 10;
    pt.y = 20;
    if (pt.x != 10 || pt.y != 20) {
        printf("FAIL: Point pt.x=%d, pt.y=%d\n", pt.x, pt.y);
        passed = 0;
    }

    if (passed) {
        printf("PASS: t_typedef\n");
    }
    return passed ? 0 : 1;
}
