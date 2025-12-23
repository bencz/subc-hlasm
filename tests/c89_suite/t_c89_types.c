/*
 * ============================================================================
 * C89 DATA TYPES COMPLETE REFERENCE
 * ============================================================================
 *
 * This file demonstrates ALL data types defined in ANSI X3.159-1989 (C89)
 * and ISO/IEC 9899:1990 (C90).
 *
 * USAGE:
 *   gcc -std=c89 -pedantic -Wall -Wextra c89_types.c -o c89_types
 *   gcc -ansi -pedantic -Wall -Wextra c89_types.c -o c89_types
 *
 * ============================================================================
 * C89 TYPE CATEGORIES
 * ============================================================================
 *
 * 1. BASIC TYPES (Section 6.1.2.5):
 *
 *    1.1 Character Types:
 *        [INCLUDED] char
 *        [INCLUDED] signed char
 *        [INCLUDED] unsigned char
 *
 *    1.2 Integer Types (signed):
 *        [INCLUDED] short int (short)
 *        [INCLUDED] int
 *        [INCLUDED] long int (long)
 *
 *    1.3 Integer Types (unsigned):
 *        [INCLUDED] unsigned short int (unsigned short)
 *        [INCLUDED] unsigned int (unsigned)
 *        [INCLUDED] unsigned long int (unsigned long)
 *
 *    1.4 Floating-Point Types:
 *        [INCLUDED] float
 *        [INCLUDED] double
 *        [INCLUDED] long double
 *
 * 2. VOID TYPE (Section 6.1.2.5):
 *        [INCLUDED] void
 *
 * 3. DERIVED TYPES (Section 6.1.2.5):
 *        [INCLUDED] Arrays
 *        [INCLUDED] Structures (struct)
 *        [INCLUDED] Unions (union)
 *        [INCLUDED] Functions
 *        [INCLUDED] Pointers
 *
 * 4. ENUMERATION TYPE (Section 6.1.2.5):
 *        [INCLUDED] enum
 *
 * 5. TYPE QUALIFIERS (Section 6.5.3):
 *        [INCLUDED] const
 *        [INCLUDED] volatile
 *
 * 6. STORAGE CLASS SPECIFIERS (Section 6.5.1):
 *        [INCLUDED] auto
 *        [INCLUDED] register
 *        [INCLUDED] static
 *        [INCLUDED] extern
 *        [INCLUDED] typedef
 *
 * 7. STANDARD LIBRARY TYPES (from standard headers):
 *        [INCLUDED] size_t (<stddef.h>)
 *        [INCLUDED] ptrdiff_t (<stddef.h>)
 *        [INCLUDED] wchar_t (<stddef.h>)
 *        [INCLUDED] NULL (<stddef.h>)
 *        [INCLUDED] offsetof (<stddef.h>)
 *        [INCLUDED] FILE (<stdio.h>)
 *        [INCLUDED] fpos_t (<stdio.h>)
 *        [INCLUDED] va_list (<stdarg.h>)
 *        [INCLUDED] jmp_buf (<setjmp.h>)
 *        [INCLUDED] sig_atomic_t (<signal.h>)
 *        [INCLUDED] time_t (<time.h>)
 *        [INCLUDED] clock_t (<time.h>)
 *        [INCLUDED] struct tm (<time.h>)
 *        [INCLUDED] div_t (<stdlib.h>)
 *        [INCLUDED] ldiv_t (<stdlib.h>)
 *        [INCLUDED] struct lconv (<locale.h>)
 *
 * ============================================================================
 * TYPES NOT IN C89 (added in later standards)
 * ============================================================================
 *
 * C99 additions:
 *   - long long int, unsigned long long int
 *   - _Bool (bool with <stdbool.h>)
 *   - _Complex, _Imaginary (complex numbers)
 *   - intN_t, uintN_t (exact-width integers from <stdint.h>)
 *   - intptr_t, uintptr_t
 *   - intmax_t, uintmax_t
 *   - int_leastN_t, uint_leastN_t
 *   - int_fastN_t, uint_fastN_t
 *
 * C11 additions:
 *   - _Atomic
 *   - _Alignas, _Alignof
 *   - _Generic
 *   - _Noreturn
 *   - _Static_assert
 *   - _Thread_local
 *   - char16_t, char32_t
 *
 * C23 additions:
 *   - _BitInt(N)
 *   - nullptr_t
 *   - typeof, typeof_unqual
 *
 * ============================================================================
 * IMPLEMENTATION-DEFINED ASPECTS (Section 5.2.4.2)
 * ============================================================================
 *
 * The following are implementation-defined in C89:
 *   - Whether char is signed or unsigned by default
 *   - The number of bits in a byte (CHAR_BIT, minimum 8)
 *   - The sizes of integer types (only minimum ranges guaranteed)
 *   - The representation of floating-point types
 *   - The result of converting to a signed type when value cannot be represented
 *
 * Minimum guaranteed ranges (from <limits.h>):
 *   - char: at least 8 bits
 *   - short: at least 16 bits (-32767 to +32767)
 *   - int: at least 16 bits (-32767 to +32767)
 *   - long: at least 32 bits (-2147483647 to +2147483647)
 *
 * ============================================================================
 */

#include <stddef.h>   /* size_t, ptrdiff_t, wchar_t, NULL, offsetof */
#include <limits.h>   /* integer limits */
#include <float.h>    /* floating-point limits */
#include <stdio.h>    /* FILE, fpos_t */
#include <stdarg.h>   /* va_list */
#include <setjmp.h>   /* jmp_buf */
#include <signal.h>   /* sig_atomic_t */
#include <time.h>     /* time_t, clock_t, struct tm */
#include <stdlib.h>   /* div_t, ldiv_t */
#include <locale.h>   /* struct lconv */

/* ========================================================================== */
/* SECTION 1: BASIC TYPES - CHARACTER TYPES                                   */
/* ========================================================================== */

/*
 * char: Basic character type. Whether it is signed or unsigned is
 * implementation-defined. Used for character data and strings.
 * Guaranteed to be at least 8 bits (CHAR_BIT >= 8).
 */
char char_var = 'A';
char char_array[] = "Hello, C89!";

/*
 * signed char: Explicitly signed character type.
 * Range: SCHAR_MIN (-127) to SCHAR_MAX (+127) minimum.
 * Can be used for small signed integers.
 */
signed char signed_char_var = -100;
signed char signed_char_max = 127;
signed char signed_char_min = -127;

/*
 * unsigned char: Explicitly unsigned character type.
 * Range: 0 to UCHAR_MAX (255 minimum).
 * Often used for raw byte manipulation.
 */
unsigned char unsigned_char_var = 200;
unsigned char unsigned_char_max = 255;
unsigned char unsigned_char_min = 0;

/* ========================================================================== */
/* SECTION 2: BASIC TYPES - SIGNED INTEGER TYPES                              */
/* ========================================================================== */

/*
 * short int (or just "short"): Small signed integer.
 * Range: SHRT_MIN (-32767) to SHRT_MAX (+32767) minimum.
 * Guaranteed to be at least 16 bits.
 */
short short_var = 1000;
short int short_int_var = -1000;
signed short signed_short_var = 2000;
signed short int signed_short_int_var = -2000;

/*
 * int: Default signed integer type.
 * Range: INT_MIN (-32767) to INT_MAX (+32767) minimum.
 * Guaranteed to be at least 16 bits.
 * Most commonly 32 bits on modern systems.
 */
int int_var = 100000;
signed signed_var = -100000;
signed int signed_int_var = 50000;

/*
 * long int (or just "long"): Large signed integer.
 * Range: LONG_MIN (-2147483647) to LONG_MAX (+2147483647) minimum.
 * Guaranteed to be at least 32 bits.
 */
long long_var = 1000000L;
long int long_int_var = -1000000L;
signed long signed_long_var = 2000000L;
signed long int signed_long_int_var = -2000000L;

/* ========================================================================== */
/* SECTION 3: BASIC TYPES - UNSIGNED INTEGER TYPES                            */
/* ========================================================================== */

/*
 * unsigned short int (or "unsigned short"): Small unsigned integer.
 * Range: 0 to USHRT_MAX (65535 minimum).
 */
unsigned short unsigned_short_var = 60000U;
unsigned short int unsigned_short_int_var = 65000U;

/*
 * unsigned int (or just "unsigned"): Default unsigned integer.
 * Range: 0 to UINT_MAX (65535 minimum).
 */
unsigned unsigned_var = 50000U;
unsigned int unsigned_int_var = 100000U;

/*
 * unsigned long int (or "unsigned long"): Large unsigned integer.
 * Range: 0 to ULONG_MAX (4294967295 minimum).
 */
unsigned long unsigned_long_var = 3000000000UL;
unsigned long int unsigned_long_int_var = 4000000000UL;

/* ========================================================================== */
/* SECTION 4: BASIC TYPES - FLOATING-POINT TYPES                              */
/* ========================================================================== */

/*
 * float: Single-precision floating-point.
 * Typically 32 bits (IEEE 754 single precision on most systems).
 * Minimum: 6 significant decimal digits (FLT_DIG >= 6).
 */
float float_var = 3.14159F;
float float_scientific = 1.23e10F;
float float_negative = -2.5F;

/*
 * double: Double-precision floating-point.
 * Typically 64 bits (IEEE 754 double precision on most systems).
 * Minimum: 10 significant decimal digits (DBL_DIG >= 10).
 */
double double_var = 3.141592653589793;
double double_scientific = 1.23e100;
double double_negative = -2.718281828;

/*
 * long double: Extended-precision floating-point.
 * Size varies by implementation (80-bit, 128-bit, or same as double).
 * Minimum: 10 significant decimal digits (LDBL_DIG >= 10).
 */
long double long_double_var = 3.14159265358979323846L;
long double long_double_scientific = 1.23e1000L;
long double long_double_negative = -1.41421356237L;

/* ========================================================================== */
/* SECTION 5: VOID TYPE                                                       */
/* ========================================================================== */

/*
 * void: Represents the absence of type.
 * Three uses in C89:
 * 1. Function returning no value
 * 2. Function taking no parameters
 * 3. Generic pointer (void *)
 */

/* Function returning void (no return value) */
void void_function(void)
{
    /* Does nothing, returns nothing */
}

/* Function taking void (no parameters) - C89 style */
int function_no_params(void)
{
    return 42;
}

/* void pointer - generic pointer type */
void *void_pointer;
void *generic_ptr = NULL;

/* ========================================================================== */
/* SECTION 6: DERIVED TYPES - ARRAYS                                          */
/* ========================================================================== */

/*
 * Arrays: Contiguous sequences of objects of the same type.
 * In C89, array size must be a constant expression (no VLAs).
 */

/* One-dimensional arrays */
int int_array[10];
char char_array_sized[100];
double double_array[5] = {1.0, 2.0, 3.0, 4.0, 5.0};

/* Multi-dimensional arrays */
int matrix_2d[3][4];
int matrix_3d[2][3][4];
char string_array[3][20] = {"Hello", "World", "C89"};

/* Array with initializer determining size */
int auto_sized_array[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

/* ========================================================================== */
/* SECTION 7: DERIVED TYPES - STRUCTURES                                      */
/* ========================================================================== */

/*
 * struct: User-defined type containing named members.
 * Members can be of different types.
 */

/* Basic structure */
struct Point {
    int x;
    int y;
};

/* Structure with various member types */
struct Person {
    char name[50];
    int age;
    float height;
    double weight;
};

/* Nested structures */
struct Rectangle {
    struct Point top_left;
    struct Point bottom_right;
};

/* Structure with bit-fields */
struct Flags {
    unsigned int flag1 : 1;
    unsigned int flag2 : 1;
    unsigned int flag3 : 1;
    unsigned int value : 5;
    unsigned int : 0;          /* Zero-width bit-field forces alignment */
    unsigned int next_field : 4;
};

/* Signed bit-fields */
struct SignedBits {
    signed int signed_field : 4;   /* Can hold -8 to 7 (or -7 to 7) */
    int plain_field : 4;           /* Signedness implementation-defined */
};

/* Self-referential structure (for linked data structures) */
struct Node {
    int data;
    struct Node *next;
};

/* Structure variable declarations */
struct Point point_var = {10, 20};
struct Person person_var = {"John Doe", 30, 1.75F, 70.5};
struct Rectangle rect_var = {{0, 0}, {100, 100}};

/* Anonymous structure instance */
struct {
    int a;
    int b;
} anonymous_struct = {1, 2};

/* ========================================================================== */
/* SECTION 8: DERIVED TYPES - UNIONS                                          */
/* ========================================================================== */

/*
 * union: User-defined type where all members share the same memory.
 * Size of union is the size of its largest member.
 * Only one member can be active at a time.
 */

/* Basic union */
union Data {
    int i;
    float f;
    char str[20];
};

/* Union for type punning (implementation-defined behavior) */
union IntBytes {
    int value;
    unsigned char bytes[sizeof(int)];
};

/* Union with structures */
union Mixed {
    struct {
        short low;
        short high;
    } parts;
    int whole;
};

/* Union variable declarations */
union Data data_var;
union IntBytes int_bytes_var;

/* ========================================================================== */
/* SECTION 9: DERIVED TYPES - POINTERS                                        */
/* ========================================================================== */

/*
 * Pointers: Variables that hold memory addresses.
 * Every type T has a corresponding pointer type "pointer to T".
 */

/* Pointers to basic types */
char *char_ptr;
int *int_ptr;
float *float_ptr;
double *double_ptr;
long *long_ptr;

/* Pointer to pointer (multiple indirection) */
int **ptr_to_ptr;
int ***ptr_to_ptr_to_ptr;

/* Pointer to array */
int (*ptr_to_array)[10];

/* Array of pointers */
int *array_of_ptrs[10];

/* Pointer to structure */
struct Point *point_ptr;

/* Pointer to union */
union Data *data_ptr;

/* Pointer to function */
int (*func_ptr)(int, int);

/* Void pointer (generic pointer) */
void *generic_pointer;

/* Null pointer constant */
int *null_ptr = NULL;
void *null_void_ptr = (void *)0;

/* const and pointers */
const int *ptr_to_const_int;       /* Pointer to const int */
int *const const_ptr_to_int = NULL; /* Const pointer to int (must init) */
const int *const const_ptr_to_const = NULL; /* Both const */

/* volatile pointer */
volatile int *ptr_to_volatile;
int *volatile volatile_ptr;

/* ========================================================================== */
/* SECTION 10: DERIVED TYPES - FUNCTIONS                                      */
/* ========================================================================== */

/*
 * Functions: Executable code blocks with parameters and return type.
 * Function types are defined by return type and parameter types.
 */

/* Function declarations (prototypes) - C89 style */
int add(int a, int b);
double multiply(double x, double y);
void print_message(const char *msg);
char *get_string(void);

/* Function with variable arguments (requires <stdarg.h>) */
int sum_all(int count, ...);

/* Function returning pointer */
int *get_int_ptr(void);

/* Function returning pointer to array */
int (*get_array_ptr(void))[10];

/* Function taking pointer to function as parameter */
void apply_func(int (*f)(int));

/* Function returning pointer to function */
int (*get_func_ptr(void))(int, int);

/* ========================================================================== */
/* SECTION 11: ENUMERATION TYPE                                               */
/* ========================================================================== */

/*
 * enum: User-defined type consisting of named integer constants.
 * Enumeration constants are of type int.
 */

/* Basic enumeration */
enum Color {
    RED,        /* 0 */
    GREEN,      /* 1 */
    BLUE        /* 2 */
};

/* Enumeration with explicit values */
enum Status {
    STATUS_OK = 0,
    STATUS_ERROR = -1,
    STATUS_PENDING = 1,
    STATUS_TIMEOUT = 2
};

/* Enumeration with mixed explicit/implicit values */
enum Mixed_Values {
    VAL_A = 10,
    VAL_B,          /* 11 */
    VAL_C,          /* 12 */
    VAL_D = 100,
    VAL_E           /* 101 */
};

/* Boolean-like enumeration (C89 has no bool type) */
enum Boolean {
    FALSE = 0,
    TRUE = 1
};

/* Enumeration variables */
enum Color color_var = GREEN;
enum Status status_var = STATUS_OK;
enum Boolean bool_var = TRUE;

/* ========================================================================== */
/* SECTION 12: TYPE QUALIFIERS                                                */
/* ========================================================================== */

/*
 * const: Object cannot be modified after initialization.
 * volatile: Object may be modified by external factors (hardware, other threads).
 * Both qualifiers can be combined.
 */

/* const qualifier */
const int const_int = 100;
const char const_char = 'X';
const double const_double = 3.14159;
const char *const_string = "Immutable string";

/* const array */
const int const_array[5] = {1, 2, 3, 4, 5};

/* const structure */
const struct Point const_point = {50, 50};

/* volatile qualifier */
volatile int volatile_int;
volatile unsigned long volatile_counter;

/* volatile pointer to volatile data */
volatile int *volatile volatile_ptr_to_volatile;

/* const volatile (read-only but may change externally, e.g., hardware register) */
const volatile int const_volatile_int = 0;

/* ========================================================================== */
/* SECTION 13: STORAGE CLASS SPECIFIERS                                       */
/* ========================================================================== */

/*
 * Storage class specifiers determine:
 * - Storage duration (lifetime)
 * - Linkage (visibility across files)
 * - Memory location hints
 */

/* extern: External linkage, defined elsewhere */
extern int extern_var;
extern void extern_function(void);

/* static at file scope: Internal linkage */
static int static_file_var = 0;
static void static_file_function(void) { }

/* Example function showing local storage classes */
void storage_class_demo(void)
{
    /* auto: Automatic storage duration (default for local variables) */
    auto int auto_var = 10;
    int implicit_auto = 20;  /* Same as auto */
    
    /* register: Hint to store in CPU register */
    register int register_var = 100;
    register int loop_counter;
    
    /* static at block scope: Static storage duration, no linkage */
    static int static_local = 0;
    static_local++;  /* Retains value between calls */
    
    /* Note: Cannot take address of register variable */
    /* int *ptr = &register_var;  <-- This would be an error */
    
    (void)auto_var;
    (void)implicit_auto;
    (void)register_var;
    (void)loop_counter;
    (void)static_file_var;
    static_file_function();
}

/* ========================================================================== */
/* SECTION 14: TYPEDEF                                                        */
/* ========================================================================== */

/*
 * typedef: Creates an alias (synonym) for an existing type.
 * Does not create a new type, just a new name.
 */

/* Simple typedefs */
typedef int Integer;
typedef unsigned int UInteger;
typedef unsigned long ULong;
typedef char *String;

/* Typedef for arrays */
typedef int IntArray10[10];
typedef char Buffer[256];

/* Typedef for pointers */
typedef int *IntPtr;
typedef const char *ConstString;
typedef void *GenericPtr;

/* Typedef for structures */
typedef struct Point Point_t;
typedef struct {
    double real;
    double imag;
} Complex;

/* Typedef for unions */
typedef union Data Data_t;

/* Typedef for enums */
typedef enum Color Color_t;
typedef enum { OFF, ON } Switch;

/* Typedef for function pointers */
typedef int (*BinaryOp)(int, int);
typedef void (*VoidFunc)(void);
typedef int (*Comparator)(const void *, const void *);

/* Using typedefs */
Integer typedef_int = 42;
String typedef_string = "Hello";
IntArray10 typedef_array;
Complex typedef_complex = {1.0, 2.0};
BinaryOp typedef_func_ptr;

/* ========================================================================== */
/* SECTION 15: STANDARD LIBRARY TYPES                                         */
/* ========================================================================== */

/*
 * Types defined in C89 standard library headers.
 */

/* From <stddef.h> */
size_t size_var;                    /* Unsigned type for sizeof result */
ptrdiff_t ptrdiff_var;              /* Signed type for pointer difference */
wchar_t wchar_var = L'A';           /* Wide character type */

/* NULL is a null pointer constant, typically ((void *)0) or 0 */
void *null_example = NULL;

/* offsetof macro - gets byte offset of structure member */
size_t point_y_offset = offsetof(struct Point, y);

/* From <stdio.h> */
FILE *file_ptr;                     /* File stream type */
fpos_t file_position;               /* File position type */

/* From <stdarg.h> */
va_list arg_list;                   /* Variable argument list type */

/* From <setjmp.h> */
jmp_buf jump_buffer;                /* Non-local jump buffer */

/* From <signal.h> */
sig_atomic_t atomic_flag;           /* Atomically accessible integer type */

/* From <time.h> */
time_t time_var;                    /* Calendar time type */
clock_t clock_var;                  /* Processor time type */
struct tm time_struct;              /* Broken-down time structure */

/* From <stdlib.h> */
div_t div_result;                   /* Result of div() function */
ldiv_t ldiv_result;                 /* Result of ldiv() function */

/* From <locale.h> */
struct lconv *locale_info;          /* Locale numeric formatting info */

/* ========================================================================== */
/* SECTION 16: COMPLEX TYPE DECLARATIONS                                      */
/* ========================================================================== */

/*
 * Complex declarations combining multiple type concepts.
 * Using the "clockwise/spiral rule" or "right-left rule" to read.
 */

/* Array of pointers to functions returning int */
int (*func_ptr_array[10])(void);

/* Pointer to array of 10 integers */
int (*ptr_to_int_array)[10];

/* Function returning pointer to array of 10 integers */
int (*func_returning_array_ptr(void))[10];

/* Pointer to function returning pointer to int */
int *(*ptr_to_func_returning_ptr)(void);

/* Array of pointers to functions taking int and returning pointer to char */
char *(*complex_array[5])(int);

/* Pointer to function taking pointer to function as argument */
void (*ptr_to_func_taking_func)(void (*)(void));

/* ========================================================================== */
/* SECTION 17: INTEGER CONSTANTS REPRESENTATION                               */
/* ========================================================================== */

/* Demonstrating constant suffixes and bases */
int decimal_const = 12345;
int octal_const = 0777;              /* Octal: prefix 0 */
int hex_const = 0xFF;                /* Hexadecimal: prefix 0x or 0X */
unsigned int unsigned_const = 12345U;
long int long_const = 123456789L;
unsigned long ulong_const = 123456789UL;

/* Character constants */
char char_const = 'A';
char escape_newline = '\n';
char escape_tab = '\t';
char escape_backslash = '\\';
char escape_quote = '\'';
char escape_dquote = '\"';
char escape_null = '\0';
char escape_bell = '\a';
char escape_backspace = '\b';
char escape_formfeed = '\f';
char escape_carriage = '\r';
char escape_vtab = '\v';
char escape_octal = '\101';          /* Octal: 'A' */
char escape_hex = '\x41';            /* Hexadecimal: 'A' */

/* Wide character constant */
wchar_t wide_char = L'A';

/* ========================================================================== */
/* SECTION 18: FLOATING-POINT CONSTANTS REPRESENTATION                        */
/* ========================================================================== */

/* Various floating-point constant formats */
double fp_decimal = 123.456;
double fp_no_integer = .456;
double fp_no_fraction = 123.;
double fp_exponent = 1.23e10;
double fp_exponent_neg = 1.23e-10;
double fp_exponent_pos = 1.23e+10;
double fp_exponent_cap = 1.23E10;

float fp_float = 123.456F;
float fp_float_lower = 123.456f;

long double fp_long = 123.456L;
long double fp_long_lower = 123.456l;

/* ========================================================================== */
/* SECTION 19: STRING LITERALS                                                */
/* ========================================================================== */

/* String literals (array of char with null terminator) */
char *simple_string = "Hello, World!";
char *escape_string = "Line1\nLine2\tTabbed";
char *quote_string = "He said \"Hello\"";
char *concatenated = "Hello, " "World!";  /* Adjacent strings concatenate */

/* Wide string literal */
wchar_t *wide_string = L"Wide string";

/* ========================================================================== */
/* SECTION 20: LIMITS AND RANGES                                              */
/* ========================================================================== */

/*
 * Compile-time constants for type limits.
 * From <limits.h> for integers, <float.h> for floating-point.
 */

/* Character limits */
int char_bit = CHAR_BIT;
int char_max = CHAR_MAX;
int char_min = CHAR_MIN;
int schar_max = SCHAR_MAX;
int schar_min = SCHAR_MIN;
int uchar_max = UCHAR_MAX;

/* Short limits */
int shrt_max = SHRT_MAX;
int shrt_min = SHRT_MIN;
unsigned int ushrt_max = USHRT_MAX;

/* Int limits */
int int_max = INT_MAX;
int int_min = INT_MIN;
unsigned int uint_max = UINT_MAX;

/* Long limits */
long long_max = LONG_MAX;
long long_min = LONG_MIN;
unsigned long ulong_max = ULONG_MAX;

/* Floating-point limits (from <float.h>) */
int flt_dig = FLT_DIG;
int dbl_dig = DBL_DIG;
int ldbl_dig = LDBL_DIG;

float flt_max = FLT_MAX;
float flt_min = FLT_MIN;
float flt_epsilon = FLT_EPSILON;

double dbl_max = DBL_MAX;
double dbl_min = DBL_MIN;
double dbl_epsilon = DBL_EPSILON;

/* ========================================================================== */
/* FUNCTION IMPLEMENTATIONS                                                   */
/* ========================================================================== */

int add(int a, int b)
{
    return a + b;
}

double multiply(double x, double y)
{
    return x * y;
}

void print_message(const char *msg)
{
    (void)msg;  /* Suppress unused parameter warning */
}

char *get_string(void)
{
    static char buffer[] = "Static string";
    return buffer;
}

int sum_all(int count, ...)
{
    va_list args;
    int sum = 0;
    int i;
    
    va_start(args, count);
    for (i = 0; i < count; i++) {
        sum += va_arg(args, int);
    }
    va_end(args);
    
    return sum;
}

int *get_int_ptr(void)
{
    static int value = 42;
    return &value;
}

void apply_func(int (*f)(int))
{
    (void)f;
}

/* ========================================================================== */
/* MAIN FUNCTION                                                              */
/* ========================================================================== */

int main(void)
{
    /* Basic type usage verification */
    int result = 0;
    
    result += (int)char_var;
    result += (int)signed_char_var;
    result += (int)unsigned_char_var;
    result += short_var;
    result += int_var;
    result += (int)long_var;
    result += (int)float_var;
    result += (int)double_var;
    result += (int)long_double_var;
    
    /* Structure usage */
    result += point_var.x + point_var.y;
    
    /* Enum usage */
    result += color_var;
    
    /* Function calls */
    result += add(10, 20);
    result += (int)multiply(2.0, 3.0);
    result += sum_all(3, 1, 2, 3);
    
    /* Pointer usage */
    int_ptr = &result;
    generic_pointer = int_ptr;
    
    /* Storage class demo */
    storage_class_demo();
    
    return result > 0 ? 0 : 1;
}

/* ========================================================================== */
/* END OF C89 DATA TYPES REFERENCE                                            */
/* ========================================================================== */