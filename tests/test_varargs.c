/*
 * SubC Compiler - Extensive va_args Tests
 * 
 * This file tests the va_args implementation in SubC.
 * The implementation has known issues on x86-64 due to ABI incompatibility.
 *
 * KNOWN PROBLEMS:
 * 1. x86-64 System V ABI passes first 6 args in registers, but SubC
 *    always pushes to stack
 * 2. _va_arg() has incorrect pointer arithmetic
 * 3. No type-aware argument extraction
 *
 * Compile with: ./scc0 -S test_varargs.c
 */

#include <stdio.h>
#include <varargs.h>

/* ============================================================================
 * TEST 1: Simple varargs function - sum of integers
 * ============================================================================
 */
int sum_ints(int count, ...) {
    void *ap;
    int i, total, val;

    ap = _va_start(&count);
    total = 0;
    for (i = 0; i < count; i++) {
        val = (int) _va_arg(&ap);
        total = total + val;
    }
    _va_end(&ap);

    return total;
}

/* ============================================================================
 * TEST 2: Varargs with different first argument types
 * ============================================================================
 */
int sum_after_char(char marker, int count, ...) {
    void *ap;
    int i, total, val;

    ap = _va_start(&count);
    total = 0;
    for (i = 0; i < count; i++) {
        val = (int) _va_arg(&ap);
        total = total + val;
    }
    _va_end(&ap);

    return total;
}

/* ============================================================================
 * TEST 3: Varargs with pointer argument before ellipsis
 * ============================================================================
 */
int sum_with_ptr(int *result, int count, ...) {
    void *ap;
    int i, total, val;

    ap = _va_start(&count);
    total = 0;
    for (i = 0; i < count; i++) {
        val = (int) _va_arg(&ap);
        total = total + val;
    }
    _va_end(&ap);

    *result = total;
    return total;
}

/* ============================================================================
 * TEST 4: Multiple varargs calls in sequence
 * ============================================================================
 */
void test_multiple_calls(void) {
    int r1, r2, r3;

    r1 = sum_ints(3, 10, 20, 30);
    r2 = sum_ints(5, 1, 2, 3, 4, 5);
    r3 = sum_ints(1, 100);

    printf("sum_ints(3, 10, 20, 30) = %d (expected 60)\n", r1);
    printf("sum_ints(5, 1, 2, 3, 4, 5) = %d (expected 15)\n", r2);
    printf("sum_ints(1, 100) = %d (expected 100)\n", r3);
}

/* ============================================================================
 * TEST 5: Varargs with many arguments (stress test)
 * ============================================================================
 */
int sum_many(int count, ...) {
    void *ap;
    int i, total, val;

    ap = _va_start(&count);
    total = 0;
    for (i = 0; i < count; i++) {
        val = (int) _va_arg(&ap);
        total = total + val;
    }
    _va_end(&ap);

    return total;
}

void test_many_varargs(void) {
    int result;

    /* Test with 10 arguments */
    result = sum_many(10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
    printf("sum_many(10, 1..10) = %d (expected 55)\n", result);

    /* Test with 20 arguments */
    result = sum_many(20, 
        1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
        11, 12, 13, 14, 15, 16, 17, 18, 19, 20);
    printf("sum_many(20, 1..20) = %d (expected 210)\n", result);

    /* Test with 30 arguments */
    result = sum_many(30,
        1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
        11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
        21, 22, 23, 24, 25, 26, 27, 28, 29, 30);
    printf("sum_many(30, 1..30) = %d (expected 465)\n", result);
}

/* ============================================================================
 * TEST 6: Varargs with zero variable arguments
 * ============================================================================
 */
void test_zero_varargs(void) {
    int result;

    result = sum_ints(0);
    printf("sum_ints(0) = %d (expected 0)\n", result);
}

/* ============================================================================
 * TEST 7: Nested varargs calls
 * ============================================================================
 */
int nested_sum(int a, int b, ...) {
    void *ap;
    int c, d;

    ap = _va_start(&b);
    c = (int) _va_arg(&ap);
    d = (int) _va_arg(&ap);
    _va_end(&ap);

    /* Call another varargs function from within */
    return sum_ints(4, a, b, c, d);
}

void test_nested_varargs(void) {
    int result;

    result = nested_sum(10, 20, 30, 40);
    printf("nested_sum(10, 20, 30, 40) = %d (expected 100)\n", result);
}

/* ============================================================================
 * TEST 8: Varargs with pointer arguments in variable part
 * ============================================================================
 */
int sum_ptr_values(int count, ...) {
    void *ap;
    int i, total;
    int *ptr;

    ap = _va_start(&count);
    total = 0;
    for (i = 0; i < count; i++) {
        ptr = (int *) _va_arg(&ap);
        total = total + *ptr;
    }
    _va_end(&ap);

    return total;
}

void test_ptr_varargs(void) {
    int a, b, c;
    int result;

    a = 100;
    b = 200;
    c = 300;

    result = sum_ptr_values(3, &a, &b, &c);
    printf("sum_ptr_values(3, &100, &200, &300) = %d (expected 600)\n", result);
}

/* ============================================================================
 * TEST 9: Custom printf-like function
 * ============================================================================
 */
void my_print(char *fmt, ...) {
    void *ap;
    char *p;
    int ival;
    char *sval;

    ap = _va_start(&fmt);

    for (p = fmt; *p; p++) {
        if (*p != '%') {
            putchar(*p);
            continue;
        }
        p++;
        switch (*p) {
        case 'd':
            ival = (int) _va_arg(&ap);
            printf("%d", ival);
            break;
        case 's':
            sval = (char *) _va_arg(&ap);
            printf("%s", sval);
            break;
        case '%':
            putchar('%');
            break;
        }
    }

    _va_end(&ap);
}

void test_custom_printf(void) {
    printf("Testing custom printf:\n");
    my_print("Hello %s, you have %d messages\n", "User", 42);
    my_print("Values: %d, %d, %d\n", 10, 20, 30);
}

/* ============================================================================
 * TEST 10: Varargs return value preservation
 * ============================================================================
 */
int first_arg(int dummy, ...) {
    void *ap;
    int first;

    ap = _va_start(&dummy);
    first = (int) _va_arg(&ap);
    _va_end(&ap);

    return first;
}

int last_of_n(int n, ...) {
    void *ap;
    int i, val;

    ap = _va_start(&n);
    val = 0;
    for (i = 0; i < n; i++) {
        val = (int) _va_arg(&ap);
    }
    _va_end(&ap);

    return val;
}

void test_return_values(void) {
    int r1, r2, r3;

    r1 = first_arg(0, 42, 99, 100);
    r2 = last_of_n(3, 10, 20, 30);
    r3 = last_of_n(5, 1, 2, 3, 4, 5);

    printf("first_arg(0, 42, 99, 100) = %d (expected 42)\n", r1);
    printf("last_of_n(3, 10, 20, 30) = %d (expected 30)\n", r2);
    printf("last_of_n(5, 1, 2, 3, 4, 5) = %d (expected 5)\n", r3);
}

/* ============================================================================
 * TEST 11: Varargs with expressions as arguments
 * ============================================================================
 */
void test_expr_args(void) {
    int a, b, result;

    a = 5;
    b = 10;

    result = sum_ints(4, a + b, a * b, a - b, b / a);
    printf("sum_ints(4, 5+10, 5*10, 5-10, 10/5) = %d (expected 15+50-5+2=62)\n", result);

    result = sum_ints(3, a * 2, b * 2, (a + b) * 2);
    printf("sum_ints(3, 10, 20, 30) = %d (expected 60)\n", result);
}

/* ============================================================================
 * TEST 12: Varargs with function call as argument
 * ============================================================================
 */
int double_val(int x) {
    return x * 2;
}

void test_funcall_args(void) {
    int result;

    result = sum_ints(3, double_val(5), double_val(10), double_val(15));
    printf("sum_ints(3, double(5), double(10), double(15)) = %d (expected 60)\n", result);
}

/* ============================================================================
 * TEST 13: Verify argument order (right-to-left vs left-to-right)
 * ============================================================================
 */
int check_order(int first, ...) {
    void *ap;
    int second, third;

    ap = _va_start(&first);
    second = (int) _va_arg(&ap);
    third = (int) _va_arg(&ap);
    _va_end(&ap);

    printf("  first=%d, second=%d, third=%d\n", first, second, third);
    return (first == 1 && second == 2 && third == 3);
}

void test_arg_order(void) {
    int ok;

    printf("Testing argument order:\n");
    ok = check_order(1, 2, 3);
    printf("Order correct: %s\n", ok ? "YES" : "NO");
}

/* ============================================================================
 * MAIN - Run all va_args tests
 * ============================================================================
 */
int main(void) {
    printf("=== SubC va_args Tests ===\n\n");

    printf("TEST 1: Simple sum\n");
    printf("sum_ints(3, 1, 2, 3) = %d (expected 6)\n", sum_ints(3, 1, 2, 3));

    printf("\nTEST 2: Multiple calls\n");
    test_multiple_calls();

    printf("\nTEST 3: Many varargs\n");
    test_many_varargs();

    printf("\nTEST 4: Zero varargs\n");
    test_zero_varargs();

    printf("\nTEST 5: Nested varargs\n");
    test_nested_varargs();

    printf("\nTEST 6: Pointer varargs\n");
    test_ptr_varargs();

    printf("\nTEST 7: Custom printf\n");
    test_custom_printf();

    printf("\nTEST 8: Return values\n");
    test_return_values();

    printf("\nTEST 9: Expression arguments\n");
    test_expr_args();

    printf("\nTEST 10: Function call arguments\n");
    test_funcall_args();

    printf("\nTEST 11: Argument order\n");
    test_arg_order();

    printf("\n=== va_args Tests Complete ===\n");
    return 0;
}
