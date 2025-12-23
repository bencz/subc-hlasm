/*
 * Test: Preprocessor
 * Tests: #define, #ifdef, #ifndef, #else, #endif
 */
#include <stdio.h>

#define VALUE 42
#define ENABLED

int main(void) {
    int passed;
    int x;

    passed = 1;

    /* Simple define */
    x = VALUE;
    if (x != 42) {
        printf("FAIL: VALUE = %d (expected 42)\n", x);
        passed = 0;
    }

    /* ifdef - defined */
#ifdef ENABLED
    x = 1;
#else
    x = 0;
#endif
    if (x != 1) {
        printf("FAIL: #ifdef ENABLED\n");
        passed = 0;
    }

    /* ifdef - not defined */
#ifdef NOT_DEFINED
    x = 0;
#else
    x = 1;
#endif
    if (x != 1) {
        printf("FAIL: #ifdef NOT_DEFINED\n");
        passed = 0;
    }

    /* ifndef - not defined */
#ifndef NOT_DEFINED
    x = 1;
#else
    x = 0;
#endif
    if (x != 1) {
        printf("FAIL: #ifndef NOT_DEFINED\n");
        passed = 0;
    }

    /* ifndef - defined */
#ifndef ENABLED
    x = 0;
#else
    x = 1;
#endif
    if (x != 1) {
        printf("FAIL: #ifndef ENABLED\n");
        passed = 0;
    }

    /* Nested ifdef */
#ifdef ENABLED
#ifdef VALUE
    x = 100;
#else
    x = 0;
#endif
#else
    x = 0;
#endif
    if (x != 100) {
        printf("FAIL: nested #ifdef\n");
        passed = 0;
    }

    if (passed) {
        printf("PASS: t_preproc\n");
    }
    return passed ? 0 : 1;
}
