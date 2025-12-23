/*
 * SubC Compiler - Limitation Tests
 * 
 * Tests for compiler limits and AArch64 code generation.
 * Compile: ./scc0 -T darwin-aarch64 -I ../runtime/include -S test.c
 */

#include <stdio.h>

/* TEST 1: Function with 32 parameters */
int fn32(
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

/* TEST 2: Function with 33 parameters */
int fn33(
    int a1,  int a2,  int a3,  int a4,  int a5,  int a6,  int a7,  int a8,
    int a9,  int a10, int a11, int a12, int a13, int a14, int a15, int a16,
    int a17, int a18, int a19, int a20, int a21, int a22, int a23, int a24,
    int a25, int a26, int a27, int a28, int a29, int a30, int a31, int a32,
    int a33
) {
    return a1 + a33;
}

/* TEST 3: Call 32 params */
void tcall32(void) {
    int r;
    r = fn32(
        1,  2,  3,  4,  5,  6,  7,  8,
        9,  10, 11, 12, 13, 14, 15, 16,
        17, 18, 19, 20, 21, 22, 23, 24,
        25, 26, 27, 28, 29, 30, 31, 32
    );
    printf("Sum 1..32 = %d (exp 528)\n", r);
}

/* TEST 4: Call 33 params */
void tcall33(void) {
    int r;
    r = fn33(
        1,  2,  3,  4,  5,  6,  7,  8,
        9,  10, 11, 12, 13, 14, 15, 16,
        17, 18, 19, 20, 21, 22, 23, 24,
        25, 26, 27, 28, 29, 30, 31, 32,
        33
    );
    printf("fn33 = %d (exp 34)\n", r);
}

/* TEST 5: 16 nested loops */
int tloop16(void) {
    int i1, i2, i3, i4, i5, i6, i7, i8;
    int i9, i10, i11, i12, i13, i14, i15, i16;
    int cnt;

    cnt = 0;
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
        cnt++;

    return cnt;
}

/* TEST 6: Switch with 32 cases */
int tswitch(int x) {
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

/* TEST 7: 32 local initializers */
int tinit32(void) {
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

/* TEST 8: 33 local initializers */
int tinit33(void) {
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

/* TEST 9: Pointer indirection */
void tptr(void) {
    int x;
    int *p1;
    int **p2;

    x = 42;
    p1 = &x;
    p2 = &p1;

    printf("x=%d *p1=%d **p2=%d\n", x, *p1, **p2);
}

/* TEST 10: Complex expression */
int texpr(int a, int b, int c, int d) {
    return ((a + b) * (c - d)) + ((a - b) * (c + d)) +
           ((a * b) + (c * d)) - ((a / b) + (c / d)) +
           ((a + c) * (b - d)) + ((a - c) * (b + d)) +
           ((a * c) + (b * d)) - ((a / c) + (b / d)) +
           ((a + d) * (b - c)) + ((a - d) * (b + c)) +
           ((a * d) + (b * c)) - ((a / d) + (b / c));
}

/* TEST 11: Identifier length */
int shortname;
int name16chars_ok;

void tidlen(void) {
    shortname = 1;
    name16chars_ok = 2;
    
    printf("shortname = %d\n", shortname);
    printf("name16 = %d\n", name16chars_ok);
}

/* MAIN */
int main(void) {
    int r;

    printf("=== SubC Tests ===\n\n");

    printf("TEST 1: 32 params\n");
    tcall32();

    printf("\nTEST 2: 33 params\n");
    tcall33();

    printf("\nTEST 3: 16 nested loops\n");
    r = tloop16();
    printf("cnt = %d (exp 65536)\n", r);

    printf("\nTEST 4: switch 32 cases\n");
    r = tswitch(15);
    printf("sw(15) = %d (exp 15)\n", r);

    printf("\nTEST 5: 32 inits\n");
    r = tinit32();
    printf("sum = %d (exp 528)\n", r);

    printf("\nTEST 6: 33 inits\n");
    r = tinit33();
    printf("1+33 = %d (exp 34)\n", r);

    printf("\nTEST 7: pointers\n");
    tptr();

    printf("\nTEST 8: expr\n");
    r = texpr(10, 5, 8, 2);
    printf("expr = %d\n", r);

    printf("\nTEST 9: id len\n");
    tidlen();

    printf("\n=== Done ===\n");
    return 0;
}
