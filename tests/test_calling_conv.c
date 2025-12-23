/*
 * SubC Compiler - Calling Convention Analysis Tests
 * 
 * This file is designed to generate assembly code that can be analyzed
 * to verify the calling convention used by SubC.
 *
 * EXPECTED BEHAVIOR:
 * - i386 (cdecl): All args pushed to stack right-to-left, caller cleans
 * - x86-64 (System V): First 6 int args in rdi,rsi,rdx,rcx,r8,r9, rest on stack
 *
 * ACTUAL BEHAVIOR (SubC):
 * - All architectures: All args pushed to stack, caller cleans
 * - This is INCORRECT for x86-64!
 *
 * Compile with: ./scc0 -T linux-386 -S test_calling_conv.c
 *               ./scc0 -T linux-x86-64 -S test_calling_conv.c
 * Then compare the generated assembly.
 */

/* ============================================================================
 * TEST 1: Simple function call - observe argument passing
 * ============================================================================
 */
int add_two(int a, int b) {
    return a + b;
}

void test_simple_call(void) {
    int result;
    result = add_two(10, 20);
}

/* ============================================================================
 * TEST 2: Function with 6 arguments (x86-64 register limit)
 * ============================================================================
 */
int add_six(int a, int b, int c, int d, int e, int f) {
    return a + b + c + d + e + f;
}

void test_six_args(void) {
    int result;
    result = add_six(1, 2, 3, 4, 5, 6);
}

/* ============================================================================
 * TEST 3: Function with 7 arguments (exceeds x86-64 registers)
 * ============================================================================
 */
int add_seven(int a, int b, int c, int d, int e, int f, int g) {
    return a + b + c + d + e + f + g;
}

void test_seven_args(void) {
    int result;
    result = add_seven(1, 2, 3, 4, 5, 6, 7);
}

/* ============================================================================
 * TEST 4: Function with 8 arguments
 * ============================================================================
 */
int add_eight(int a, int b, int c, int d, int e, int f, int g, int h) {
    return a + b + c + d + e + f + g + h;
}

void test_eight_args(void) {
    int result;
    result = add_eight(1, 2, 3, 4, 5, 6, 7, 8);
}

/* ============================================================================
 * TEST 5: Accessing parameters - verify stack layout
 * ============================================================================
 */
int param_access(int a, int b, int c, int d) {
    int local1;
    int local2;

    local1 = a + b;
    local2 = c + d;

    return local1 + local2;
}

void test_param_access(void) {
    int result;
    result = param_access(100, 200, 300, 400);
}

/* ============================================================================
 * TEST 6: Mixed local variables and parameters
 * ============================================================================
 */
int mixed_access(int p1, int p2, int p3) {
    int l1, l2, l3;

    l1 = p1 * 2;
    l2 = p2 * 3;
    l3 = p3 * 4;

    return l1 + l2 + l3 + p1 + p2 + p3;
}

/* ============================================================================
 * TEST 7: Recursive function - verify stack frame
 * ============================================================================
 */
int factorial(int n) {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

void test_recursion(void) {
    int result;
    result = factorial(5);
}

/* ============================================================================
 * TEST 8: Function pointer call
 * ============================================================================
 */
int (*func_ptr)(int, int);

int multiply(int a, int b) {
    return a * b;
}

void test_func_ptr(void) {
    int result;

    func_ptr = multiply;
    result = (*func_ptr)(6, 7);
}

/* ============================================================================
 * TEST 9: Nested function calls
 * ============================================================================
 */
int inner(int x) {
    return x * 2;
}

int outer(int a, int b) {
    return inner(a) + inner(b);
}

void test_nested_calls(void) {
    int result;
    result = outer(10, 20);
}

/* ============================================================================
 * TEST 10: Function call as argument
 * ============================================================================
 */
int square(int x) {
    return x * x;
}

int sum_squares(int a, int b) {
    return a + b;
}

void test_call_as_arg(void) {
    int result;
    result = sum_squares(square(3), square(4));
}

/* ============================================================================
 * TEST 11: Return value in different contexts
 * ============================================================================
 */
int get_value(void) {
    return 42;
}

void test_return_contexts(void) {
    int a, b, c;

    /* Direct assignment */
    a = get_value();

    /* In expression */
    b = get_value() + 10;

    /* As function argument */
    c = add_two(get_value(), get_value());
}

/* ============================================================================
 * TEST 12: Large local frame
 * ============================================================================
 */
int large_frame(int p1, int p2) {
    int arr[100];
    int i, sum;

    sum = 0;
    for (i = 0; i < 100; i++) {
        arr[i] = i + p1;
    }
    for (i = 0; i < 100; i++) {
        sum = sum + arr[i];
    }

    return sum + p2;
}

void test_large_frame(void) {
    int result;
    result = large_frame(1, 2);
}

/* ============================================================================
 * TEST 13: Char parameters (promotion test)
 * ============================================================================
 */
int add_chars(char a, char b, char c) {
    return a + b + c;
}

void test_char_params(void) {
    int result;
    char x, y, z;

    x = 10;
    y = 20;
    z = 30;

    result = add_chars(x, y, z);
}

/* ============================================================================
 * TEST 14: Pointer parameters
 * ============================================================================
 */
void swap(int *a, int *b) {
    int temp;
    temp = *a;
    *a = *b;
    *b = temp;
}

void test_ptr_params(void) {
    int x, y;

    x = 100;
    y = 200;
    swap(&x, &y);
}

/* ============================================================================
 * TEST 15: Mixed types in parameters
 * ============================================================================
 */
int mixed_types(int a, char b, int *c, char *d) {
    return a + b + *c + *d;
}

void test_mixed_types(void) {
    int i, result;
    char c;

    i = 100;
    c = 50;

    result = mixed_types(10, 20, &i, &c);
}

/* ============================================================================
 * MAIN
 * ============================================================================
 */
int main(void) {
    test_simple_call();
    test_six_args();
    test_seven_args();
    test_eight_args();
    test_param_access();
    test_recursion();
    test_func_ptr();
    test_nested_calls();
    test_call_as_arg();
    test_return_contexts();
    test_large_frame();
    test_char_params();
    test_ptr_params();
    test_mixed_types();

    return 0;
}
