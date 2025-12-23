/*
 * SubC Compiler - Extensive Limitation Tests
 * 
 * This file contains test cases to verify and expose the limitations
 * of the SubC compiler, particularly around:
 * - Function parameters (MAXFNARGS = 32)
 * - va_args implementation
 * - Calling conventions
 * - Various other limits
 *
 * Compile with: ./scc0 -S test_limitations.c
 * Then examine the generated assembly
 */

#include <stdio.h>

/* ============================================================================
 * TEST 1: Function with exactly 32 parameters (at the limit)
 * ============================================================================
 */
int func_32_params(
    int a1,  int a2,  int a3,  int a4,  int a5,  int a6,  int a7,  int a8,
    int a9,  int a10, int a11, int a12, int a13, int a14, int a15, int a16,
    int a17, int a18, int a19, int a20, int a21, int a22, int a23, int a24,
    int a25, int a26, int a27, int a28, int a29, int a30, int a31, int a32
) {
    return a1 + a2 + a3 + a4 + a5 + a6 + a7 + a8 +
           a9 + a10 + a11 + a12 + a13 + a14 + a15 + a16 +
           a17 + a18 + a19 + a20 + a21 + a22 + a23 + a24 +
           a25 + a26 + a27 + a28 + a29 + a30 + a31 + a32;
}

/* ============================================================================
 * TEST 2: Function with 33 parameters (exceeds MAXFNARGS)
 * This should trigger "too many function parameters" error
 * ============================================================================
 */
int func_33_params(
    int a1,  int a2,  int a3,  int a4,  int a5,  int a6,  int a7,  int a8,
    int a9,  int a10, int a11, int a12, int a13, int a14, int a15, int a16,
    int a17, int a18, int a19, int a20, int a21, int a22, int a23, int a24,
    int a25, int a26, int a27, int a28, int a29, int a30, int a31, int a32,
    int a33
) {
    return a1 + a33;
}

/* ============================================================================
 * TEST 3: Function with 64 parameters (double the limit)
 * ============================================================================
 */
int func_64_params(
    int a1,  int a2,  int a3,  int a4,  int a5,  int a6,  int a7,  int a8,
    int a9,  int a10, int a11, int a12, int a13, int a14, int a15, int a16,
    int a17, int a18, int a19, int a20, int a21, int a22, int a23, int a24,
    int a25, int a26, int a27, int a28, int a29, int a30, int a31, int a32,
    int a33, int a34, int a35, int a36, int a37, int a38, int a39, int a40,
    int a41, int a42, int a43, int a44, int a45, int a46, int a47, int a48,
    int a49, int a50, int a51, int a52, int a53, int a54, int a55, int a56,
    int a57, int a58, int a59, int a60, int a61, int a62, int a63, int a64
) {
    return a1 + a64;
}

/* ============================================================================
 * TEST 4: Test calling functions with many arguments
 * ============================================================================
 */
void test_call_32_params(void) {
    int result;
    result = func_32_params(
        1,  2,  3,  4,  5,  6,  7,  8,
        9,  10, 11, 12, 13, 14, 15, 16,
        17, 18, 19, 20, 21, 22, 23, 24,
        25, 26, 27, 28, 29, 30, 31, 32
    );
    printf("Sum of 1..32 = %d (expected 528)\n", result);
}

void test_call_33_params(void) {
    int result;
    result = func_33_params(
        1,  2,  3,  4,  5,  6,  7,  8,
        9,  10, 11, 12, 13, 14, 15, 16,
        17, 18, 19, 20, 21, 22, 23, 24,
        25, 26, 27, 28, 29, 30, 31, 32,
        33
    );
    printf("func_33_params result = %d (expected 34)\n", result);
}

/* ============================================================================
 * TEST 5: Deeply nested loops (testing MAXBREAK = 16)
 * ============================================================================
 */
int test_nested_loops_16(void) {
    int i1, i2, i3, i4, i5, i6, i7, i8;
    int i9, i10, i11, i12, i13, i14, i15, i16;
    int count;

    count = 0;
    for (i1 = 0; i1 < 2; i1++)
    for (i2 = 0; i2 < 2; i2++)
    for (i3 = 0; i3 < 2; i3++)
    for (i4 = 0; i4 < 2; i4++)
    for (i5 = 0; i5 < 2; i5++)
    for (i6 = 0; i6 < 2; i6++)
    for (i7 = 0; i7 < 2; i7++)
    for (i8 = 0; i8 < 2; i8++)
    for (i9 = 0; i9 < 2; i9++)
    for (i10 = 0; i10 < 2; i10++)
    for (i11 = 0; i11 < 2; i11++)
    for (i12 = 0; i12 < 2; i12++)
    for (i13 = 0; i13 < 2; i13++)
    for (i14 = 0; i14 < 2; i14++)
    for (i15 = 0; i15 < 2; i15++)
    for (i16 = 0; i16 < 2; i16++)
        count++;

    return count;
}

/* ============================================================================
 * TEST 6: 17 nested loops (exceeds MAXBREAK)
 * This should trigger "too many nested loops/switches" fatal error
 * ============================================================================
 */
/*
int test_nested_loops_17(void) {
    int i1, i2, i3, i4, i5, i6, i7, i8;
    int i9, i10, i11, i12, i13, i14, i15, i16, i17;
    int count;

    count = 0;
    for (i1 = 0; i1 < 2; i1++)
    for (i2 = 0; i2 < 2; i2++)
    for (i3 = 0; i3 < 2; i3++)
    for (i4 = 0; i4 < 2; i4++)
    for (i5 = 0; i5 < 2; i5++)
    for (i6 = 0; i6 < 2; i6++)
    for (i7 = 0; i7 < 2; i7++)
    for (i8 = 0; i8 < 2; i8++)
    for (i9 = 0; i9 < 2; i9++)
    for (i10 = 0; i10 < 2; i10++)
    for (i11 = 0; i11 < 2; i11++)
    for (i12 = 0; i12 < 2; i12++)
    for (i13 = 0; i13 < 2; i13++)
    for (i14 = 0; i14 < 2; i14++)
    for (i15 = 0; i15 < 2; i15++)
    for (i16 = 0; i16 < 2; i16++)
    for (i17 = 0; i17 < 2; i17++)
        count++;

    return count;
}
*/

/* ============================================================================
 * TEST 7: Switch with many cases (testing MAXCASE = 256)
 * ============================================================================
 */
int test_switch_256_cases(int x) {
    switch (x) {
    case 0: return 0;
    case 1: return 1;
    case 2: return 2;
    case 3: return 3;
    case 4: return 4;
    case 5: return 5;
    case 6: return 6;
    case 7: return 7;
    case 8: return 8;
    case 9: return 9;
    case 10: return 10;
    case 11: return 11;
    case 12: return 12;
    case 13: return 13;
    case 14: return 14;
    case 15: return 15;
    case 16: return 16;
    case 17: return 17;
    case 18: return 18;
    case 19: return 19;
    case 20: return 20;
    case 21: return 21;
    case 22: return 22;
    case 23: return 23;
    case 24: return 24;
    case 25: return 25;
    case 26: return 26;
    case 27: return 27;
    case 28: return 28;
    case 29: return 29;
    case 30: return 30;
    case 31: return 31;
    default: return -1;
    }
}

/* ============================================================================
 * TEST 8: Local initializers (testing MAXLOCINIT = 32)
 * ============================================================================
 */
int test_local_init_32(void) {
    int a1 = 1,  a2 = 2,  a3 = 3,  a4 = 4;
    int a5 = 5,  a6 = 6,  a7 = 7,  a8 = 8;
    int a9 = 9,  a10 = 10, a11 = 11, a12 = 12;
    int a13 = 13, a14 = 14, a15 = 15, a16 = 16;
    int a17 = 17, a18 = 18, a19 = 19, a20 = 20;
    int a21 = 21, a22 = 22, a23 = 23, a24 = 24;
    int a25 = 25, a26 = 26, a27 = 27, a28 = 28;
    int a29 = 29, a30 = 30, a31 = 31, a32 = 32;

    return a1 + a2 + a3 + a4 + a5 + a6 + a7 + a8 +
           a9 + a10 + a11 + a12 + a13 + a14 + a15 + a16 +
           a17 + a18 + a19 + a20 + a21 + a22 + a23 + a24 +
           a25 + a26 + a27 + a28 + a29 + a30 + a31 + a32;
}

/* ============================================================================
 * TEST 9: 33 local initializers (exceeds MAXLOCINIT)
 * This should trigger "too many local initializers" error
 * ============================================================================
 */
int test_local_init_33(void) {
    int a1 = 1,  a2 = 2,  a3 = 3,  a4 = 4;
    int a5 = 5,  a6 = 6,  a7 = 7,  a8 = 8;
    int a9 = 9,  a10 = 10, a11 = 11, a12 = 12;
    int a13 = 13, a14 = 14, a15 = 15, a16 = 16;
    int a17 = 17, a18 = 18, a19 = 19, a20 = 20;
    int a21 = 21, a22 = 22, a23 = 23, a24 = 24;
    int a25 = 25, a26 = 26, a27 = 27, a28 = 28;
    int a29 = 29, a30 = 30, a31 = 31, a32 = 32;
    int a33 = 33;

    return a1 + a33;
}

/* ============================================================================
 * TEST 10: Pointer indirection levels
 * ============================================================================
 */
void test_pointer_levels(void) {
    int x;
    int *p1;
    int **p2;
    /* int ***p3; */  /* This would trigger "too many levels of indirection" */

    x = 42;
    p1 = &x;
    p2 = &p1;

    printf("x = %d, *p1 = %d, **p2 = %d\n", x, *p1, **p2);
}

/* ============================================================================
 * TEST 11: Complex expression (testing NODEPOOLSZ)
 * ============================================================================
 */
int test_complex_expr(int a, int b, int c, int d) {
    return ((a + b) * (c - d)) + ((a - b) * (c + d)) +
           ((a * b) + (c * d)) - ((a / b) + (c / d)) +
           ((a + c) * (b - d)) + ((a - c) * (b + d)) +
           ((a * c) + (b * d)) - ((a / c) + (b / d)) +
           ((a + d) * (b - c)) + ((a - d) * (b + c)) +
           ((a * d) + (b * c)) - ((a / d) + (b / c));
}

/* ============================================================================
 * TEST 12: Identifier length (NAMELEN = 16)
 * ============================================================================
 */
int short_name;
int exactly_16_chars;  /* 16 characters - at limit */
int this_is_a_very_long_identifier_name;  /* Will be truncated to 16 chars */

void test_identifier_length(void) {
    short_name = 1;
    exactly_16_chars = 2;
    this_is_a_very_long_identifier_name = 3;
    
    printf("short_name = %d\n", short_name);
    printf("exactly_16_chars = %d\n", exactly_16_chars);
    printf("long_name = %d\n", this_is_a_very_long_identifier_name);
}

/* ============================================================================
 * MAIN - Run all tests
 * ============================================================================
 */
int main(void) {
    int result;

    printf("=== SubC Limitation Tests ===\n\n");

    printf("TEST 1: 32 parameters function\n");
    test_call_32_params();

    printf("\nTEST 2: 33 parameters function\n");
    test_call_33_params();

    printf("\nTEST 3: 16 nested loops\n");
    result = test_nested_loops_16();
    printf("Nested loop count = %d (expected 65536)\n", result);

    printf("\nTEST 4: Switch with 32 cases\n");
    result = test_switch_256_cases(15);
    printf("switch(15) = %d (expected 15)\n", result);

    printf("\nTEST 5: 32 local initializers\n");
    result = test_local_init_32();
    printf("Sum of 1..32 = %d (expected 528)\n", result);

    printf("\nTEST 6: 33 local initializers\n");
    result = test_local_init_33();
    printf("1 + 33 = %d (expected 34)\n", result);

    printf("\nTEST 7: Pointer indirection\n");
    test_pointer_levels();

    printf("\nTEST 8: Complex expression\n");
    result = test_complex_expr(10, 5, 8, 2);
    printf("Complex expr result = %d\n", result);

    printf("\nTEST 9: Identifier length\n");
    test_identifier_length();

    printf("\n=== Tests Complete ===\n");
    return 0;
}
