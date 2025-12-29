/*
 * Test: Comments after preprocessor directives
 *
 * Tests that comments are allowed after preprocessor directives:
 * - #include with inline comments
 * - #define with inline comments
 * - #ifdef/#ifndef with comments
 */

/* Test 1: Comment after #include with angle brackets */
#include <stdio.h> /* standard I/O header */

/* Test 2: Comment after #include with quotes */
#include <stdlib.h> /* also works with stdlib */

/* Test 3: // comment after #include */
#include <string.h> // C++ style comment after include

/* Test 5: Comment after #define */
#define VALUE1 100 /* a constant value */

/* Test 6: // comment after #define */
#define VALUE2 200 // another constant

/* Test 7: Comment after #define with expression */
#define SUM (VALUE1 + VALUE2) /* sum of values */

/* Test 8: Comment after #ifdef */
#ifdef VALUE1 /* check if VALUE1 is defined */
#define HAS_VALUE1 1
#endif /* end VALUE1 check */

/* Test 9: Comment after #ifndef */
#ifndef UNDEFINED_MACRO /* this should not be defined */
#define NOT_DEFINED_CHECK 1
#endif /* end UNDEFINED_MACRO check */

/* Test 10: Comment after #else */
#ifdef NONEXISTENT
#define BRANCH_A 1
#else /* take the else branch */
#define BRANCH_B 1
#endif /* end NONEXISTENT check */

/* Test 11: Comment after #undef */
#define TEMP_MACRO 1
#undef TEMP_MACRO /* remove the macro */

/* Test 12: Multiple comments on same directive */
#define MULTI 42 /* first comment */ /* second comment */

/* Test 13: Tab before comment */
#define TABBED 99	/* comment after tab */

/* Test 14: Lots of spaces before comment */
#define SPACED 88                    /* far away comment */

int main(void) {
    int passed;
    passed = 1;

    /* Verify the defines worked correctly */
    if (VALUE1 != 100) {
        printf("FAIL: VALUE1 should be 100, got %d\n", VALUE1);
        passed = 0;
    }

    if (VALUE2 != 200) {
        printf("FAIL: VALUE2 should be 200, got %d\n", VALUE2);
        passed = 0;
    }

    if (SUM != 300) {
        printf("FAIL: SUM should be 300, got %d\n", SUM);
        passed = 0;
    }

#ifdef HAS_VALUE1
    /* OK - VALUE1 was defined so HAS_VALUE1 should be set */
#else
    printf("FAIL: HAS_VALUE1 should be defined\n");
    passed = 0;
#endif

#ifdef NOT_DEFINED_CHECK
    /* OK - UNDEFINED_MACRO was not defined */
#else
    printf("FAIL: NOT_DEFINED_CHECK should be defined\n");
    passed = 0;
#endif

#ifdef BRANCH_B
    /* OK - NONEXISTENT was not defined, so else branch taken */
#else
    printf("FAIL: BRANCH_B should be defined\n");
    passed = 0;
#endif

#ifdef TEMP_MACRO
    printf("FAIL: TEMP_MACRO should have been undefined\n");
    passed = 0;
#endif

    if (MULTI != 42) {
        printf("FAIL: MULTI should be 42, got %d\n", MULTI);
        passed = 0;
    }

    if (TABBED != 99) {
        printf("FAIL: TABBED should be 99, got %d\n", TABBED);
        passed = 0;
    }

    if (SPACED != 88) {
        printf("FAIL: SPACED should be 88, got %d\n", SPACED);
        passed = 0;
    }

    if (passed) {
        printf("PASS: t_comments_after_directive\n");
    }

    return passed ? 0 : 1;
}
