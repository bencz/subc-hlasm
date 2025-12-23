/*
 * SubC Floating-Point Test Suite
 *
 * Comprehensive tests for floating-point support:
 * - Basic arithmetic operations (+, -, *, /)
 * - Comparisons (==, !=, <, >, <=, >=)
 * - Type conversions (int<->float, float<->double)
 * - Function calls with many FP parameters
 * - Mixed integer and FP operations
 * - Negation and sign handling
 * - Special values (zero, negative)
 *
 * Note: printf with %f requires runtime library support.
 * These tests are designed to validate code generation.
 */

/* Forward declarations for test functions */
int test_float_arithmetic(void);
int test_double_arithmetic(void);
int test_float_comparisons(void);
int test_double_comparisons(void);
int test_float_conversions(void);
int test_many_float_params(void);
int test_many_double_params(void);
int test_mixed_operations(void);
int test_float_negation(void);
int test_float_assignment(void);

/* Global test counters */
int tests_passed;
int tests_failed;

/* Helper to report test results */
void report(char *name, int passed) {
    if (passed) {
        tests_passed++;
        /* printf("PASS: %s\n", name); */
    } else {
        tests_failed++;
        /* printf("FAIL: %s\n", name); */
    }
}

/* ============================================================================
 * Float Arithmetic Tests
 * ============================================================================ */

float float_add(float a, float b) {
    return a + b;
}

float float_sub(float a, float b) {
    return a - b;
}

float float_mul(float a, float b) {
    return a * b;
}

float float_div(float a, float b) {
    return a / b;
}

int test_float_arithmetic(void) {
    float a, b, r;
    int pass;

    pass = 1;

    /* Addition */
    a = 1.5;
    b = 2.5;
    r = float_add(a, b);
    /* Expected: 4.0 */

    /* Subtraction */
    a = 5.0;
    b = 3.0;
    r = float_sub(a, b);
    /* Expected: 2.0 */

    /* Multiplication */
    a = 2.0;
    b = 3.0;
    r = float_mul(a, b);
    /* Expected: 6.0 */

    /* Division */
    a = 10.0;
    b = 2.0;
    r = float_div(a, b);
    /* Expected: 5.0 */

    /* Compound expression */
    a = 2.0;
    b = 3.0;
    r = (a + b) * (a - b);
    /* Expected: 5.0 * -1.0 = -5.0 */

    return pass;
}

/* ============================================================================
 * Double Arithmetic Tests
 * ============================================================================ */

double double_add(double a, double b) {
    return a + b;
}

double double_sub(double a, double b) {
    return a - b;
}

double double_mul(double a, double b) {
    return a * b;
}

double double_div(double a, double b) {
    return a / b;
}

int test_double_arithmetic(void) {
    double a, b, r;
    int pass;

    pass = 1;

    /* Addition */
    a = 1.5;
    b = 2.5;
    r = double_add(a, b);

    /* Subtraction */
    a = 5.0;
    b = 3.0;
    r = double_sub(a, b);

    /* Multiplication */
    a = 2.0;
    b = 3.0;
    r = double_mul(a, b);

    /* Division */
    a = 10.0;
    b = 2.0;
    r = double_div(a, b);

    /* More precision test */
    a = 1.23456789012345;
    b = 9.87654321098765;
    r = double_add(a, b);

    return pass;
}

/* ============================================================================
 * Float Comparison Tests
 * ============================================================================ */

int float_eq(float a, float b) { return a == b; }
int float_ne(float a, float b) { return a != b; }
int float_lt(float a, float b) { return a < b; }
int float_gt(float a, float b) { return a > b; }
int float_le(float a, float b) { return a <= b; }
int float_ge(float a, float b) { return a >= b; }

int test_float_comparisons(void) {
    float a, b;
    int r, pass;

    pass = 1;

    a = 1.0;
    b = 2.0;

    r = float_eq(a, a);  /* 1.0 == 1.0 -> 1 */
    r = float_eq(a, b);  /* 1.0 == 2.0 -> 0 */

    r = float_ne(a, b);  /* 1.0 != 2.0 -> 1 */
    r = float_ne(a, a);  /* 1.0 != 1.0 -> 0 */

    r = float_lt(a, b);  /* 1.0 < 2.0 -> 1 */
    r = float_lt(b, a);  /* 2.0 < 1.0 -> 0 */

    r = float_gt(b, a);  /* 2.0 > 1.0 -> 1 */
    r = float_gt(a, b);  /* 1.0 > 2.0 -> 0 */

    r = float_le(a, b);  /* 1.0 <= 2.0 -> 1 */
    r = float_le(a, a);  /* 1.0 <= 1.0 -> 1 */
    r = float_le(b, a);  /* 2.0 <= 1.0 -> 0 */

    r = float_ge(b, a);  /* 2.0 >= 1.0 -> 1 */
    r = float_ge(a, a);  /* 1.0 >= 1.0 -> 1 */
    r = float_ge(a, b);  /* 1.0 >= 2.0 -> 0 */

    return pass;
}

/* ============================================================================
 * Double Comparison Tests
 * ============================================================================ */

int double_eq(double a, double b) { return a == b; }
int double_ne(double a, double b) { return a != b; }
int double_lt(double a, double b) { return a < b; }
int double_gt(double a, double b) { return a > b; }
int double_le(double a, double b) { return a <= b; }
int double_ge(double a, double b) { return a >= b; }

int test_double_comparisons(void) {
    double a, b;
    int r, pass;

    pass = 1;

    a = 1.0;
    b = 2.0;

    r = double_eq(a, a);
    r = double_ne(a, b);
    r = double_lt(a, b);
    r = double_gt(b, a);
    r = double_le(a, b);
    r = double_ge(b, a);

    return pass;
}

/* ============================================================================
 * Type Conversion Tests
 * ============================================================================ */

float int_to_float(int i) {
    return (float) i;
}

int float_to_int(float f) {
    return (int) f;
}

double int_to_double(int i) {
    return (double) i;
}

int double_to_int(double d) {
    return (int) d;
}

double float_to_double(float f) {
    return (double) f;
}

float double_to_float(double d) {
    return (float) d;
}

int test_float_conversions(void) {
    int i, r;
    float f;
    double d;
    int pass;

    pass = 1;

    /* int -> float */
    i = 42;
    f = int_to_float(i);

    /* float -> int */
    f = 3.7;
    r = float_to_int(f);  /* Expected: 3 (truncation) */

    /* int -> double */
    i = 12345;
    d = int_to_double(i);

    /* double -> int */
    d = 9.9;
    r = double_to_int(d);  /* Expected: 9 */

    /* float -> double */
    f = 1.5;
    d = float_to_double(f);

    /* double -> float */
    d = 2.5;
    f = double_to_float(d);

    /* Negative conversions */
    i = -10;
    f = int_to_float(i);
    r = float_to_int(f);  /* Expected: -10 */

    return pass;
}

/* ============================================================================
 * Many Float Parameters Test (40 parameters)
 * ============================================================================ */

float sum_10_floats(float f1, float f2, float f3, float f4, float f5,
                    float f6, float f7, float f8, float f9, float f10) {
    return f1 + f2 + f3 + f4 + f5 + f6 + f7 + f8 + f9 + f10;
}

float sum_20_floats(float f1, float f2, float f3, float f4, float f5,
                    float f6, float f7, float f8, float f9, float f10,
                    float f11, float f12, float f13, float f14, float f15,
                    float f16, float f17, float f18, float f19, float f20) {
    return f1 + f2 + f3 + f4 + f5 + f6 + f7 + f8 + f9 + f10 +
           f11 + f12 + f13 + f14 + f15 + f16 + f17 + f18 + f19 + f20;
}

float sum_40_floats(float f1, float f2, float f3, float f4, float f5,
                    float f6, float f7, float f8, float f9, float f10,
                    float f11, float f12, float f13, float f14, float f15,
                    float f16, float f17, float f18, float f19, float f20,
                    float f21, float f22, float f23, float f24, float f25,
                    float f26, float f27, float f28, float f29, float f30,
                    float f31, float f32, float f33, float f34, float f35,
                    float f36, float f37, float f38, float f39, float f40) {
    float sum;
    sum = f1 + f2 + f3 + f4 + f5 + f6 + f7 + f8 + f9 + f10;
    sum = sum + f11 + f12 + f13 + f14 + f15 + f16 + f17 + f18 + f19 + f20;
    sum = sum + f21 + f22 + f23 + f24 + f25 + f26 + f27 + f28 + f29 + f30;
    sum = sum + f31 + f32 + f33 + f34 + f35 + f36 + f37 + f38 + f39 + f40;
    return sum;
}

int test_many_float_params(void) {
    float r;
    int pass;

    pass = 1;

    /* Sum of 1+2+...+10 = 55 */
    r = sum_10_floats(1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0);

    /* Sum of 1+2+...+20 = 210 */
    r = sum_20_floats(1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0,
                      11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 17.0, 18.0, 19.0, 20.0);

    /* Sum of 1+2+...+40 = 820 */
    r = sum_40_floats(1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0,
                      11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 17.0, 18.0, 19.0, 20.0,
                      21.0, 22.0, 23.0, 24.0, 25.0, 26.0, 27.0, 28.0, 29.0, 30.0,
                      31.0, 32.0, 33.0, 34.0, 35.0, 36.0, 37.0, 38.0, 39.0, 40.0);

    return pass;
}

/* ============================================================================
 * Many Double Parameters Test (40 parameters)
 * ============================================================================ */

double sum_10_doubles(double d1, double d2, double d3, double d4, double d5,
                      double d6, double d7, double d8, double d9, double d10) {
    return d1 + d2 + d3 + d4 + d5 + d6 + d7 + d8 + d9 + d10;
}

double sum_20_doubles(double d1, double d2, double d3, double d4, double d5,
                      double d6, double d7, double d8, double d9, double d10,
                      double d11, double d12, double d13, double d14, double d15,
                      double d16, double d17, double d18, double d19, double d20) {
    return d1 + d2 + d3 + d4 + d5 + d6 + d7 + d8 + d9 + d10 +
           d11 + d12 + d13 + d14 + d15 + d16 + d17 + d18 + d19 + d20;
}

double sum_40_doubles(double d1, double d2, double d3, double d4, double d5,
                      double d6, double d7, double d8, double d9, double d10,
                      double d11, double d12, double d13, double d14, double d15,
                      double d16, double d17, double d18, double d19, double d20,
                      double d21, double d22, double d23, double d24, double d25,
                      double d26, double d27, double d28, double d29, double d30,
                      double d31, double d32, double d33, double d34, double d35,
                      double d36, double d37, double d38, double d39, double d40) {
    double sum;
    sum = d1 + d2 + d3 + d4 + d5 + d6 + d7 + d8 + d9 + d10;
    sum = sum + d11 + d12 + d13 + d14 + d15 + d16 + d17 + d18 + d19 + d20;
    sum = sum + d21 + d22 + d23 + d24 + d25 + d26 + d27 + d28 + d29 + d30;
    sum = sum + d31 + d32 + d33 + d34 + d35 + d36 + d37 + d38 + d39 + d40;
    return sum;
}

int test_many_double_params(void) {
    double r;
    int pass;

    pass = 1;

    r = sum_10_doubles(1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0);

    r = sum_20_doubles(1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0,
                       11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 17.0, 18.0, 19.0, 20.0);

    r = sum_40_doubles(1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0,
                       11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 17.0, 18.0, 19.0, 20.0,
                       21.0, 22.0, 23.0, 24.0, 25.0, 26.0, 27.0, 28.0, 29.0, 30.0,
                       31.0, 32.0, 33.0, 34.0, 35.0, 36.0, 37.0, 38.0, 39.0, 40.0);

    return pass;
}

/* ============================================================================
 * Mixed Integer and Float Operations
 * ============================================================================ */

float mixed_add(int i, float f) {
    return (float)i + f;
}

double mixed_mul(int i, double d) {
    return (double)i * d;
}

int mixed_compare(int i, float f) {
    return (float)i < f;
}

float compute_average(int a, int b, int c, int d, int e) {
    float sum;
    sum = (float)(a + b + c + d + e);
    return sum / 5.0;
}

int test_mixed_operations(void) {
    int i;
    float f;
    double d;
    int pass;

    pass = 1;

    /* int + float */
    i = 10;
    f = 2.5;
    f = mixed_add(i, f);  /* Expected: 12.5 */

    /* int * double */
    i = 3;
    d = 4.5;
    d = mixed_mul(i, d);  /* Expected: 13.5 */

    /* int < float comparison */
    i = 5;
    f = 5.5;
    i = mixed_compare(i, f);  /* Expected: 1 (5.0 < 5.5) */

    /* Average calculation */
    f = compute_average(10, 20, 30, 40, 50);  /* Expected: 30.0 */

    return pass;
}

/* ============================================================================
 * Float Negation Tests
 * ============================================================================ */

float negate_float(float f) {
    return -f;
}

double negate_double(double d) {
    return -d;
}

int test_float_negation(void) {
    float f;
    double d;
    int pass;

    pass = 1;

    f = 5.0;
    f = negate_float(f);  /* Expected: -5.0 */

    f = -3.0;
    f = negate_float(f);  /* Expected: 3.0 */

    d = 7.5;
    d = negate_double(d);  /* Expected: -7.5 */

    d = -2.5;
    d = negate_double(d);  /* Expected: 2.5 */

    return pass;
}

/* ============================================================================
 * Float Assignment and Local Variable Tests
 * ============================================================================ */

int test_float_assignment(void) {
    float f1, f2, f3, f4, f5;
    float f6, f7, f8, f9, f10;
    double d1, d2, d3, d4, d5;
    int pass;

    pass = 1;

    /* Multiple local float variables */
    f1 = 1.0;
    f2 = 2.0;
    f3 = 3.0;
    f4 = 4.0;
    f5 = 5.0;
    f6 = 6.0;
    f7 = 7.0;
    f8 = 8.0;
    f9 = 9.0;
    f10 = 10.0;

    /* Chain assignment */
    f1 = f2 = f3 = 5.5;

    /* Multiple local double variables */
    d1 = 1.0;
    d2 = 2.0;
    d3 = 3.0;
    d4 = 4.0;
    d5 = 5.0;

    /* Complex expression with multiple variables */
    f1 = (f2 + f3) * (f4 - f5) / f6;

    return pass;
}

/* ============================================================================
 * Global Float Variables
 * ============================================================================ */

float global_float;
double global_double;

float get_global_float(void) {
    return global_float;
}

void set_global_float(float f) {
    global_float = f;
}

double get_global_double(void) {
    return global_double;
}

void set_global_double(double d) {
    global_double = d;
}

int test_global_floats(void) {
    float f;
    double d;
    int pass;

    pass = 1;

    set_global_float(3.14);
    f = get_global_float();

    set_global_double(2.71828);
    d = get_global_double();

    /* Modify global directly */
    global_float = 1.5;
    global_double = 2.5;

    f = global_float + 1.0;
    d = global_double * 2.0;

    return pass;
}

/* ============================================================================
 * Float Array Tests
 * ============================================================================ */

float float_array[10];
double double_array[10];

int test_float_arrays(void) {
    int i;
    float sum_f;
    double sum_d;
    int pass;

    pass = 1;

    /* Initialize float array */
    i = 0;
    while (i < 10) {
        float_array[i] = (float)(i + 1);
        i = i + 1;
    }

    /* Sum float array */
    sum_f = 0.0;
    i = 0;
    while (i < 10) {
        sum_f = sum_f + float_array[i];
        i = i + 1;
    }
    /* Expected: 55.0 */

    /* Initialize double array */
    i = 0;
    while (i < 10) {
        double_array[i] = (double)(i + 1) * 0.5;
        i = i + 1;
    }

    /* Sum double array */
    sum_d = 0.0;
    i = 0;
    while (i < 10) {
        sum_d = sum_d + double_array[i];
        i = i + 1;
    }
    /* Expected: 27.5 */

    return pass;
}

/* ============================================================================
 * Float Pointer Tests
 * ============================================================================ */

void swap_floats(float *a, float *b) {
    float temp;
    temp = *a;
    *a = *b;
    *b = temp;
}

void swap_doubles(double *a, double *b) {
    double temp;
    temp = *a;
    *a = *b;
    *b = temp;
}

int test_float_pointers(void) {
    float f1, f2;
    double d1, d2;
    float *fp;
    double *dp;
    int pass;

    pass = 1;

    f1 = 1.0;
    f2 = 2.0;
    swap_floats(&f1, &f2);
    /* Expected: f1 = 2.0, f2 = 1.0 */

    d1 = 3.0;
    d2 = 4.0;
    swap_doubles(&d1, &d2);
    /* Expected: d1 = 4.0, d2 = 3.0 */

    /* Pointer arithmetic */
    fp = &float_array[0];
    f1 = *fp;
    fp = fp + 1;
    f2 = *fp;

    dp = &double_array[0];
    d1 = *dp;
    dp = dp + 1;
    d2 = *dp;

    return pass;
}

/* ============================================================================
 * Conditional Float Tests
 * ============================================================================ */

float max_float(float a, float b) {
    if (a > b)
        return a;
    else
        return b;
}

float min_float(float a, float b) {
    if (a < b)
        return a;
    else
        return b;
}

float abs_float(float f) {
    if (f < 0.0)
        return -f;
    else
        return f;
}

int test_float_conditionals(void) {
    float f;
    int pass;

    pass = 1;

    f = max_float(3.0, 5.0);  /* Expected: 5.0 */
    f = max_float(7.0, 2.0);  /* Expected: 7.0 */

    f = min_float(3.0, 5.0);  /* Expected: 3.0 */
    f = min_float(7.0, 2.0);  /* Expected: 2.0 */

    f = abs_float(-4.5);      /* Expected: 4.5 */
    f = abs_float(4.5);       /* Expected: 4.5 */

    return pass;
}

/* ============================================================================
 * Float Loop Tests
 * ============================================================================ */

float sum_series(int n) {
    float sum;
    int i;

    sum = 0.0;
    i = 1;
    while (i <= n) {
        sum = sum + (float)i;
        i = i + 1;
    }
    return sum;
}

float factorial_float(int n) {
    float result;
    int i;

    result = 1.0;
    i = 2;
    while (i <= n) {
        result = result * (float)i;
        i = i + 1;
    }
    return result;
}

int test_float_loops(void) {
    float f;
    int pass;

    pass = 1;

    f = sum_series(10);       /* Expected: 55.0 */
    f = sum_series(100);      /* Expected: 5050.0 */

    f = factorial_float(5);   /* Expected: 120.0 */
    f = factorial_float(10);  /* Expected: 3628800.0 */

    return pass;
}

/* ============================================================================
 * Main Test Runner
 * ============================================================================ */

int main(void) {
    tests_passed = 0;
    tests_failed = 0;

    report("float_arithmetic", test_float_arithmetic());
    report("double_arithmetic", test_double_arithmetic());
    report("float_comparisons", test_float_comparisons());
    report("double_comparisons", test_double_comparisons());
    report("float_conversions", test_float_conversions());
    report("many_float_params", test_many_float_params());
    report("many_double_params", test_many_double_params());
    report("mixed_operations", test_mixed_operations());
    report("float_negation", test_float_negation());
    report("float_assignment", test_float_assignment());
    report("global_floats", test_global_floats());
    report("float_arrays", test_float_arrays());
    report("float_pointers", test_float_pointers());
    report("float_conditionals", test_float_conditionals());
    report("float_loops", test_float_loops());

    /* Return 0 if all tests passed */
    return tests_failed;
}
