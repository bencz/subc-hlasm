/*
 * Test: Float/double global initialization
 * Tests: float/double variable initialization, long double, array initialization
 */
#include <stdio.h>

/* Global float initialization */
float f1 = 3.14F;
float f2 = -2.5F;
float f3 = 0.0F;

/* Global double initialization */
double d1 = 3.141592653589793;
double d2 = -1.0;
double d3 = 0.0;

/* Long double (treated as double in SubC) */
long double ld1 = 2.718281828;

/* Float array with auto-size */
float farr[] = {1.0F, 2.0F, 3.0F};

/* Double array with fixed size */
double darr[3] = {10.0, 20.0, 30.0};

/* Float array with fixed size */
float farr2[2] = {-1.5F, 2.5F};

int main(void) {
    int passed;

    passed = 1;

    /* Test float initialization - compare as integers to avoid FP comparison issues */
    /* We just verify they compile and have reasonable values */
    if (f1 < 3.0F || f1 > 3.2F) {
        printf("FAIL: f1 = 3.14F not in expected range\n");
        passed = 0;
    }

    if (f2 > -2.4F || f2 < -2.6F) {
        printf("FAIL: f2 = -2.5F not in expected range\n");
        passed = 0;
    }

    /* Test double initialization */
    if (d1 < 3.14 || d1 > 3.15) {
        printf("FAIL: d1 = 3.14159... not in expected range\n");
        passed = 0;
    }

    if (d2 > -0.9 || d2 < -1.1) {
        printf("FAIL: d2 = -1.0 not in expected range\n");
        passed = 0;
    }

    /* Test long double (treated as double) */
    if (ld1 < 2.7 || ld1 > 2.8) {
        printf("FAIL: ld1 = 2.718... not in expected range\n");
        passed = 0;
    }

    /* Test float array */
    if (farr[0] < 0.9F || farr[0] > 1.1F) {
        printf("FAIL: farr[0] = 1.0F not in expected range\n");
        passed = 0;
    }
    if (farr[1] < 1.9F || farr[1] > 2.1F) {
        printf("FAIL: farr[1] = 2.0F not in expected range\n");
        passed = 0;
    }
    if (farr[2] < 2.9F || farr[2] > 3.1F) {
        printf("FAIL: farr[2] = 3.0F not in expected range\n");
        passed = 0;
    }

    /* Test double array */
    if (darr[0] < 9.9 || darr[0] > 10.1) {
        printf("FAIL: darr[0] = 10.0 not in expected range\n");
        passed = 0;
    }
    if (darr[1] < 19.9 || darr[1] > 20.1) {
        printf("FAIL: darr[1] = 20.0 not in expected range\n");
        passed = 0;
    }
    if (darr[2] < 29.9 || darr[2] > 30.1) {
        printf("FAIL: darr[2] = 30.0 not in expected range\n");
        passed = 0;
    }

    /* Test fixed-size float array */
    if (farr2[0] > -1.4F || farr2[0] < -1.6F) {
        printf("FAIL: farr2[0] = -1.5F not in expected range\n");
        passed = 0;
    }
    if (farr2[1] < 2.4F || farr2[1] > 2.6F) {
        printf("FAIL: farr2[1] = 2.5F not in expected range\n");
        passed = 0;
    }

    if (passed) {
        printf("PASS: t_float_init\n");
    }
    return passed ? 0 : 1;
}
