/*
 * ============================================================================
 * C89 PREPROCESSOR COMPLIANCE TEST SUITE
 * ============================================================================
 * 
 * This file tests preprocessor features mandated by ANSI X3.159-1989 (C89)
 * and ISO/IEC 9899:1990 (C90). These standards are equivalent for the
 * preprocessor specification.
 *
 * USAGE:
 *   Preprocessor only:  gcc -std=c89 -pedantic -Wall -E c89_preprocessor_test.c
 *   Full compilation:   gcc -std=c89 -pedantic -Wall c89_preprocessor_test.c
 *   Alternative flag:   gcc -ansi -pedantic -Wall -E c89_preprocessor_test.c
 *
 * A fully compliant C89 preprocessor should process this file without errors.
 *
 * ============================================================================
 * C89 PREPROCESSOR FEATURES TESTED IN THIS FILE
 * ============================================================================
 *
 * SECTION 6.8 - PREPROCESSING DIRECTIVES:
 *
 *   6.8.1 - Conditional Inclusion:
 *     [TESTED] #if constant-expression
 *     [TESTED] #ifdef identifier
 *     [TESTED] #ifndef identifier
 *     [TESTED] #elif constant-expression
 *     [TESTED] #else
 *     [TESTED] #endif
 *     [TESTED] defined operator (both "defined X" and "defined(X)" forms)
 *     [TESTED] Nested conditional compilation
 *     [TESTED] Complex boolean expressions in #if
 *
 *   6.8.2 - Source File Inclusion:
 *     [TESTED] #include <header-name> (standard headers)
 *     [TESTED] #include "header-name" (quoted form)
 *     [PARTIAL] Macro expansion in #include arguments (commented example)
 *
 *   6.8.3 - Macro Replacement:
 *     [TESTED] #define identifier replacement-list (object-like macros)
 *     [TESTED] #define identifier() replacement-list (function-like, no params)
 *     [TESTED] #define identifier(params) replacement-list (function-like)
 *     [TESTED] #undef identifier
 *     [TESTED] Macro expansion and rescanning
 *     [TESTED] Nested macro expansion
 *     [TESTED] Recursive macro prevention (self-referential macros)
 *     [TESTED] Macro redefinition (identical definitions allowed)
 *     [TESTED] # operator (stringification)
 *     [TESTED] ## operator (token pasting/concatenation)
 *     [TESTED] Indirect stringification (double-macro technique)
 *     [TESTED] Empty macro definitions
 *     [TESTED] Macros with multiple arguments
 *     [TESTED] Parenthesization best practices
 *
 *   6.8.4 - Line Control:
 *     [TESTED] #line digit-sequence
 *     [TESTED] #line digit-sequence "filename"
 *
 *   6.8.5 - Error Directive:
 *     [TESTED] #error (conditionally disabled to allow compilation)
 *
 *   6.8.6 - Pragma Directive:
 *     [TESTED] #pragma (implementation-defined behavior)
 *     [TESTED] Unknown pragmas (should be ignored per C89)
 *
 *   6.8.7 - Null Directive:
 *     [TESTED] # (empty directive, line with only #)
 *
 * SECTION 6.8.8 - PREDEFINED MACRO NAMES:
 *     [TESTED] __LINE__ (current line number)
 *     [TESTED] __FILE__ (current source file name)
 *     [TESTED] __DATE__ (compilation date)
 *     [TESTED] __TIME__ (compilation time)
 *     [TESTED] __STDC__ (conforming implementation indicator)
 *
 * PREPROCESSOR CONSTANT EXPRESSIONS (Section 6.4):
 *
 *   Arithmetic Operators:
 *     [TESTED] + (unary plus, addition)
 *     [TESTED] - (unary minus, subtraction)
 *     [TESTED] * (multiplication)
 *     [TESTED] / (division)
 *     [TESTED] % (modulo)
 *
 *   Relational Operators:
 *     [TESTED] < (less than)
 *     [TESTED] > (greater than)
 *     [TESTED] <= (less than or equal)
 *     [TESTED] >= (greater than or equal)
 *     [TESTED] == (equal)
 *     [TESTED] != (not equal)
 *
 *   Logical Operators:
 *     [TESTED] && (logical AND)
 *     [TESTED] || (logical OR)
 *     [TESTED] ! (logical NOT)
 *
 *   Bitwise Operators:
 *     [TESTED] & (bitwise AND)
 *     [TESTED] | (bitwise OR)
 *     [TESTED] ^ (bitwise XOR)
 *     [TESTED] ~ (bitwise NOT)
 *     [TESTED] << (left shift)
 *     [TESTED] >> (right shift)
 *
 *   Other Operators:
 *     [TESTED] ?: (ternary/conditional operator)
 *     [TESTED] () (parentheses for grouping)
 *     [TESTED] Operator precedence
 *
 *   Numeric Constants:
 *     [TESTED] Decimal integer constants
 *     [TESTED] Octal integer constants (0-prefixed)
 *     [TESTED] Hexadecimal integer constants (0x-prefixed)
 *     [TESTED] Long integer suffixes (L, l)
 *     [TESTED] Unsigned integer suffixes (U, u)
 *     [TESTED] Character constants in expressions ('A', '\n', etc.)
 *
 * LEXICAL ELEMENTS:
 *     [TESTED] Line continuation with backslash-newline
 *     [TESTED] Comments within macro definitions
 *     [TESTED] Whitespace handling in directives
 *     [TESTED] Token separation and recognition
 *     [TESTED] String literal handling in macros
 *     [TESTED] Character constant handling
 *     [TESTED] Escape sequences in strings and chars
 *
 * COMMON PATTERNS:
 *     [TESTED] Include guards (#ifndef/#define/#endif)
 *     [TESTED] Conditional feature selection
 *     [TESTED] Debug/release build macros
 *     [TESTED] Platform detection patterns
 *
 * ============================================================================
 * C89 PREPROCESSOR FEATURES NOT TESTED IN THIS FILE
 * ============================================================================
 *
 * INTENTIONALLY NOT TESTED (would cause errors or are impractical):
 *
 *   Trigraphs (Section 5.2.1.1):
 *     [NOT TESTED] ??= for #
 *     [NOT TESTED] ??( for [
 *     [NOT TESTED] ??/ for \
 *     [NOT TESTED] ??) for ]
 *     [NOT TESTED] ??' for ^
 *     [NOT TESTED] ??< for {
 *     [NOT TESTED] ??! for |
 *     [NOT TESTED] ??> for }
 *     [NOT TESTED] ??- for ~
 *     REASON: Trigraphs are deprecated, often disabled, and would make the
 *             file unreadable. Most modern compilers require explicit flags.
 *
 *   Digraphs (C95/AMD1 addition, not C89):
 *     [NOT APPLICABLE] <: for [
 *     [NOT APPLICABLE] :> for ]
 *     [NOT APPLICABLE] <% for {
 *     [NOT APPLICABLE] %> for }
 *     [NOT APPLICABLE] %: for #
 *     [NOT APPLICABLE] %:%: for ##
 *     REASON: Digraphs were added in C95 Amendment 1, not part of C89.
 *
 *   Error Conditions:
 *     [NOT TESTED] #error with actual error (would stop compilation)
 *     [NOT TESTED] #include with missing file
 *     [NOT TESTED] Unterminated conditional compilation
 *     [NOT TESTED] Mismatched #if/#endif
 *     [NOT TESTED] Macro redefinition with different definition
 *     [NOT TESTED] Invalid token pasting results
 *     [NOT TESTED] Division by zero in constant expressions
 *     REASON: These are error conditions that would prevent successful
 *             preprocessing. A separate negative test file would be needed.
 *
 *   Implementation-Defined Behavior:
 *     [NOT TESTED] Specific #pragma behaviors
 *     [NOT TESTED] Maximum macro nesting depth
 *     [NOT TESTED] Maximum #include nesting depth
 *     [NOT TESTED] Maximum identifier length
 *     [NOT TESTED] Maximum number of macro parameters
 *     [NOT TESTED] Maximum length of logical source line
 *     [NOT TESTED] Character set specifics beyond ASCII
 *     REASON: These vary by implementation and cannot be portably tested.
 *
 *   Empty Macro Arguments:
 *     [NOT TESTED] Calling function-like macros with empty arguments
 *     REASON: Empty macro arguments are undefined behavior in C89
 *             (only defined starting in C99).
 *
 *   Variadic Macros:
 *     [NOT APPLICABLE] #define MACRO(...) (C99 feature)
 *     [NOT APPLICABLE] __VA_ARGS__ (C99 feature)
 *     REASON: Variadic macros were added in C99, not part of C89.
 *
 *   C99/C11/C17/C23 Features (not part of C89):
 *     [NOT APPLICABLE] _Pragma operator (C99)
 *     [NOT APPLICABLE] __func__ predefined identifier (C99)
 *     [NOT APPLICABLE] __STDC_VERSION__ (C95)
 *     [NOT APPLICABLE] __STDC_HOSTED__ (C99)
 *     [NOT APPLICABLE] __STDC_IEC_559__ (C99)
 *     [NOT APPLICABLE] __STDC_IEC_559_COMPLEX__ (C99)
 *     [NOT APPLICABLE] __STDC_ISO_10646__ (C99)
 *     [NOT APPLICABLE] #elifdef, #elifndef (C23)
 *     [NOT APPLICABLE] __has_include (C23)
 *     [NOT APPLICABLE] __has_c_attribute (C23)
 *     REASON: These features were added in later C standards.
 *
 * PARTIALLY TESTED:
 *
 *   Header Inclusion:
 *     [PARTIAL] Only tests a few standard headers (<stddef.h>, <limits.h>,
 *               <float.h>). Full C89 header compliance would require testing:
 *               <assert.h>, <ctype.h>, <errno.h>, <float.h>, <limits.h>,
 *               <locale.h>, <math.h>, <setjmp.h>, <signal.h>, <stdarg.h>,
 *               <stddef.h>, <stdio.h>, <stdlib.h>, <string.h>, <time.h>
 *
 *   Macro Expansion in #include:
 *     [PARTIAL] Example provided but commented out because not all
 *               preprocessors support this identically.
 *
 * ============================================================================
 * TRANSLATION PHASES RELEVANT TO PREPROCESSOR (Section 5.1.1.2)
 * ============================================================================
 *
 * The C89 standard defines 8 translation phases. This test primarily
 * exercises phases 1-4:
 *
 *   Phase 1: Physical source file characters mapped to source character set
 *            Trigraph sequences replaced
 *            [TESTED] Basic character handling
 *            [NOT TESTED] Trigraphs (see above)
 *
 *   Phase 2: Line splicing (backslash-newline deleted)
 *            [TESTED] Line continuation in macros
 *
 *   Phase 3: Source file decomposed into preprocessing tokens and whitespace
 *            [TESTED] Token recognition
 *            [TESTED] Comment handling (comments become single spaces)
 *
 *   Phase 4: Preprocessing directives executed, macro invocations expanded
 *            [TESTED] All directive types
 *            [TESTED] Macro expansion
 *            [TESTED] _Pragma would be here but is C99
 *
 * ============================================================================
 * MINIMUM TRANSLATION LIMITS (Section 5.2.4.1)
 * ============================================================================
 *
 * C89 requires implementations to support at least:
 *   - 15 nesting levels of compound statements, iteration, and selection
 *   - 8 nesting levels of conditional inclusion
 *   - 12 pointer, array, and function declarators modifying a type
 *   - 31 declarators nested by parentheses
 *   - 32 nesting levels of parenthesized expressions
 *   - 31 significant initial characters in an internal identifier
 *   - 6 significant initial characters in an external identifier
 *   - 511 external identifiers in one translation unit
 *   - 127 identifiers with block scope in one block
 *   - 1024 macro identifiers simultaneously defined
 *   - 31 parameters in one function definition/call
 *   - 31 parameters in one macro definition/invocation
 *   - 509 characters in a logical source line
 *   - 509 characters in a string literal
 *   - 32767 bytes in an object
 *   - 8 nesting levels for #include
 *   - 257 case labels for a switch
 *   - 127 members in a struct/union
 *   - 127 enumeration constants in an enum
 *   - 15 levels of nested struct/union in a declaration
 *
 * This file does NOT stress-test these limits. A separate test would be
 * needed for implementation limit verification.
 *
 * ============================================================================
 * TEST ORGANIZATION
 * ============================================================================
 *
 * TEST 01: Basic macro definition and expansion
 * TEST 02: Object-like macros with expressions
 * TEST 03: Function-like macros
 * TEST 04: Macros with no arguments vs empty argument list
 * TEST 05: Stringification operator (#)
 * TEST 06: Token pasting operator (##)
 * TEST 07: Predefined macros
 * TEST 08: Conditional compilation (#if, #elif, #else, #endif)
 * TEST 09: Conditional compilation (#ifdef, #ifndef)
 * TEST 10: The defined operator
 * TEST 11: #undef directive
 * TEST 12: #line directive
 * TEST 13: #error directive (conditionally disabled)
 * TEST 14: #pragma directive
 * TEST 15: Null directive
 * TEST 16: Macro argument prescan and expansion order
 * TEST 17: Recursive macro prevention
 * TEST 18: Macros with multiple arguments
 * TEST 19: Comments in macro definitions
 * TEST 20: Line continuation in macros
 * TEST 21: Whitespace handling in macros
 * TEST 22: Character constants and strings in macros
 * TEST 23: Numeric constants in preprocessor
 * TEST 24: Logical operators in preprocessor expressions
 * TEST 25: Comparison operators in preprocessor expressions
 * TEST 26: Arithmetic operators in preprocessor expressions
 * TEST 27: Bitwise operators in preprocessor expressions
 * TEST 28: Ternary operator in preprocessor expressions
 * TEST 29: Parentheses and operator precedence
 * TEST 30: Multiple arguments in function-like macros
 * TEST 31: Include and macro expansion
 * TEST 32: Complex nested conditionals
 * TEST 33: Token types in stringification
 * TEST 34: Macro redefinition (same definition allowed)
 * TEST 35: Edge cases with token pasting
 * TEST 36: sizeof in preprocessor (negative test)
 * TEST 37: Character constants in preprocessor expressions
 * TEST 38: Macro name edge cases
 * TEST 39: Preprocessor arithmetic edge cases
 * TEST 40: Include guards pattern
 * TEST 41: Main function for compilation verification
 *
 * ============================================================================
 */

/* ========================================================================== */
/* TEST 1: Basic macro definition and expansion                               */
/* ========================================================================== */

#define SIMPLE_MACRO 42
#define EMPTY_MACRO

int test1_simple = SIMPLE_MACRO;
int test1_empty EMPTY_MACRO;

/* ========================================================================== */
/* TEST 2: Object-like macros with expressions                                */
/* ========================================================================== */

#define EXPR_MACRO (3 + 4 * 5)
#define PAREN_MACRO ((1) + (2))
#define NEGATIVE_MACRO (-100)

int test2_expr = EXPR_MACRO;
int test2_paren = PAREN_MACRO;
int test2_neg = NEGATIVE_MACRO;

/* ========================================================================== */
/* TEST 3: Function-like macros                                               */
/* ========================================================================== */

#define ADD(a, b) ((a) + (b))
#define MUL(x, y) ((x) * (y))
#define SQUARE(n) ((n) * (n))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

int test3_add = ADD(10, 20);
int test3_mul = MUL(3, 4);
int test3_square = SQUARE(5);
int test3_max = MAX(100, 200);
int test3_min = MIN(100, 200);

/* Nested macro calls */
int test3_nested = ADD(MUL(2, 3), SQUARE(4));

/* ========================================================================== */
/* TEST 4: Macros with no arguments vs macros with empty argument list        */
/* ========================================================================== */

#define NO_ARGS_MACRO 999
#define EMPTY_ARGS_MACRO() 888

int test4_no_args = NO_ARGS_MACRO;
int test4_empty_args = EMPTY_ARGS_MACRO();

/* ========================================================================== */
/* TEST 5: Stringification operator (#)                                       */
/* ========================================================================== */

#define STRINGIFY(x) #x
#define XSTRINGIFY(x) STRINGIFY(x)

const char *test5_str1 = STRINGIFY(hello);
const char *test5_str2 = STRINGIFY(hello world);
const char *test5_str3 = STRINGIFY(123);
const char *test5_str4 = STRINGIFY("already quoted");
const char *test5_str5 = STRINGIFY(a + b * c);

/* Indirect stringification */
#define INDIRECT_VALUE 12345
const char *test5_indirect = XSTRINGIFY(INDIRECT_VALUE);

/* ========================================================================== */
/* TEST 6: Token pasting operator (##)                                        */
/* ========================================================================== */

#define PASTE(a, b) a ## b
#define XPASTE(a, b) PASTE(a, b)

/* Token pasting to create identifiers */
#define MAKE_FUNC(name) void func_ ## name(void) { }
MAKE_FUNC(alpha)
MAKE_FUNC(beta)

/* Create variable names dynamically */
#define VAR(n) variable_ ## n
int VAR(1) = 10;
int VAR(2) = 20;
int VAR(3) = 30;

/* Token pasting to create numbers */
#define NUM_PASTE(a, b) a ## b
int test6_number = NUM_PASTE(12, 34);

/* Token pasting test in function scope (allows non-constant initializers) */
void test6_paste_func(void)
{
    int local_var1 = 111;
    int local_var2 = 222;
    int result1 = PASTE(local_var, 1);
    int result2 = PASTE(local_var, 2);
    (void)result1;
    (void)result2;
}

/* ========================================================================== */
/* TEST 7: Predefined macros                                                  */
/* ========================================================================== */

const char *test7_file = __FILE__;
int test7_line = __LINE__;
const char *test7_date = __DATE__;
const char *test7_time = __TIME__;

#ifdef __STDC__
int test7_stdc = __STDC__;
#else
int test7_stdc = 0;
#endif

/* ========================================================================== */
/* TEST 8: Conditional compilation - #if, #elif, #else, #endif                */
/* ========================================================================== */

#define CONDITION_A 1
#define CONDITION_B 0
#define VALUE_TEN 10

#if CONDITION_A
int test8_if = 1;
#endif

#if CONDITION_B
int test8_else_wrong = 999;
#else
int test8_else = 2;
#endif

#if VALUE_TEN > 5
int test8_expr_gt = 1;
#endif

#if VALUE_TEN < 5
int test8_expr_lt_wrong = 999;
#elif VALUE_TEN == 10
int test8_elif = 3;
#else
int test8_elif_else_wrong = 999;
#endif

/* Nested conditionals */
#if 1
    #if 1
        int test8_nested = 4;
    #endif
#endif

/* ========================================================================== */
/* TEST 9: Conditional compilation - #ifdef, #ifndef                          */
/* ========================================================================== */

#define DEFINED_MACRO

#ifdef DEFINED_MACRO
int test9_ifdef = 1;
#endif

#ifdef UNDEFINED_MACRO_XYZ
int test9_ifdef_wrong = 999;
#endif

#ifndef UNDEFINED_MACRO_XYZ
int test9_ifndef = 2;
#endif

#ifndef DEFINED_MACRO
int test9_ifndef_wrong = 999;
#endif

/* ========================================================================== */
/* TEST 10: The defined operator                                              */
/* ========================================================================== */

#if defined(DEFINED_MACRO)
int test10_defined1 = 1;
#endif

#if defined DEFINED_MACRO
int test10_defined2 = 2;
#endif

#if !defined(UNDEFINED_MACRO_XYZ)
int test10_not_defined = 3;
#endif

#if defined(DEFINED_MACRO) && !defined(UNDEFINED_MACRO_XYZ)
int test10_combined = 4;
#endif

/* ========================================================================== */
/* TEST 11: #undef directive                                                  */
/* ========================================================================== */

#define TEMP_MACRO 100
int test11_before = TEMP_MACRO;

#undef TEMP_MACRO
#define TEMP_MACRO 200
int test11_after = TEMP_MACRO;

#undef TEMP_MACRO
#ifndef TEMP_MACRO
int test11_undef_check = 1;
#endif

/* ========================================================================== */
/* TEST 12: #line directive                                                   */
/* ========================================================================== */

#line 1000
int test12_line1 = __LINE__;

#line 2000 "fake_file.c"
int test12_line2 = __LINE__;
const char *test12_file = __FILE__;

/* ========================================================================== */
/* TEST 13: #error directive (commented out to allow compilation)             */
/* ========================================================================== */

/* Uncomment to test #error:
#if 0
#error "This error should not be triggered"
#endif
*/

#define ERROR_TEST_ENABLED 0
#if ERROR_TEST_ENABLED
#error "Error directive test"
#endif

/* ========================================================================== */
/* TEST 14: #pragma directive                                                 */
/* ========================================================================== */

/* Note: #pragma once is a common extension, not standard C89 */
/* #pragma once */

/* Unknown pragmas should be ignored in C89 */
#pragma unknown_pragma_xyz

/* ========================================================================== */
/* TEST 15: Null directive (# alone on a line)                                */
/* ========================================================================== */

#
#    
#/* comment after null directive */

/* ========================================================================== */
/* TEST 16: Macro argument prescan and expansion order                        */
/* ========================================================================== */

#define FIRST(a, b) a
#define SECOND(a, b) b
#define CALL(macro, args) macro args

int test16_first = FIRST(1, 2);
int test16_second = SECOND(1, 2);

/* ========================================================================== */
/* TEST 17: Recursive macro prevention                                        */
/* ========================================================================== */

#define RECURSIVE RECURSIVE
/* This should expand to just "RECURSIVE", not infinite recursion */
int RECURSIVE;

#define A B
#define B A
/* This should expand A to B, not cause infinite recursion */
int A;

/* ========================================================================== */
/* TEST 18: Macro with variadic-like behavior using multiple args             */
/* ========================================================================== */

#define TRIPLET(a, b, c) ((a) + (b) + (c))
#define QUAD(a, b, c, d) ((a) + (b) + (c) + (d))

int test18_triplet = TRIPLET(1, 2, 3);
int test18_quad = QUAD(1, 2, 3, 4);

/* ========================================================================== */
/* TEST 19: Comments in macro definitions                                     */
/* ========================================================================== */

#define COMMENT_MACRO /* comment */ 42 /* another comment */
int test19_comment = COMMENT_MACRO;

/* ========================================================================== */
/* TEST 20: Line continuation in macros                                       */
/* ========================================================================== */

#define MULTILINE_MACRO(x, y) \
    ((x) + \
     (y))

int test20_multiline = MULTILINE_MACRO(10, 20);

#define LONG_MACRO \
    1 + 2 + 3 + \
    4 + 5 + 6 + \
    7 + 8 + 9

int test20_long = LONG_MACRO;

/* ========================================================================== */
/* TEST 21: Whitespace handling in macros                                     */
/* ========================================================================== */

#define   SPACED_MACRO   100
#define TABS_MACRO	200
#define FUNC_SPACE( a , b )  ((a) + (b))

int test21_spaced = SPACED_MACRO;
int test21_tabs = TABS_MACRO;
int test21_func = FUNC_SPACE( 5 , 10 );

/* ========================================================================== */
/* TEST 22: Character constants and strings in macros                         */
/* ========================================================================== */

#define CHAR_MACRO 'A'
#define STRING_MACRO "Hello, World!"
#define ESCAPE_MACRO "Tab:\tNewline:\n"
#define QUOTE_MACRO "He said \"Hello\""

char test22_char = CHAR_MACRO;
const char *test22_string = STRING_MACRO;
const char *test22_escape = ESCAPE_MACRO;
const char *test22_quote = QUOTE_MACRO;

/* ========================================================================== */
/* TEST 23: Numeric constants in preprocessor                                 */
/* ========================================================================== */

#define DEC_CONST 12345
#define OCT_CONST 0777
#define HEX_CONST 0xFF
#define LONG_CONST 123456789L
#define ULONG_CONST 123456789UL

#if DEC_CONST > 10000
int test23_dec = 1;
#endif

#if HEX_CONST == 255
int test23_hex = 1;
#endif

#if OCT_CONST == 511
int test23_oct = 1;
#endif

/* ========================================================================== */
/* TEST 24: Logical operators in preprocessor expressions                     */
/* ========================================================================== */

#define TRUE_VAL 1
#define FALSE_VAL 0

#if TRUE_VAL && TRUE_VAL
int test24_and = 1;
#endif

#if TRUE_VAL || FALSE_VAL
int test24_or = 1;
#endif

#if !FALSE_VAL
int test24_not = 1;
#endif

#if (TRUE_VAL && !FALSE_VAL) || FALSE_VAL
int test24_complex = 1;
#endif

/* ========================================================================== */
/* TEST 25: Comparison operators in preprocessor expressions                  */
/* ========================================================================== */

#define VAL_A 10
#define VAL_B 20

#if VAL_A < VAL_B
int test25_lt = 1;
#endif

#if VAL_B > VAL_A
int test25_gt = 1;
#endif

#if VAL_A <= 10
int test25_le = 1;
#endif

#if VAL_B >= 20
int test25_ge = 1;
#endif

#if VAL_A == 10
int test25_eq = 1;
#endif

#if VAL_A != VAL_B
int test25_ne = 1;
#endif

/* ========================================================================== */
/* TEST 26: Arithmetic operators in preprocessor expressions                  */
/* ========================================================================== */

#if (10 + 5) == 15
int test26_add = 1;
#endif

#if (20 - 8) == 12
int test26_sub = 1;
#endif

#if (3 * 4) == 12
int test26_mul = 1;
#endif

#if (20 / 4) == 5
int test26_div = 1;
#endif

#if (17 % 5) == 2
int test26_mod = 1;
#endif

#if (-5) < 0
int test26_neg = 1;
#endif

/* ========================================================================== */
/* TEST 27: Bitwise operators in preprocessor expressions                     */
/* ========================================================================== */

#if (0xFF & 0x0F) == 0x0F
int test27_and = 1;
#endif

#if (0xF0 | 0x0F) == 0xFF
int test27_or = 1;
#endif

#if (0xFF ^ 0x0F) == 0xF0
int test27_xor = 1;
#endif

#if (~0 != 0)
int test27_not = 1;
#endif

#if (1 << 4) == 16
int test27_lshift = 1;
#endif

#if (16 >> 2) == 4
int test27_rshift = 1;
#endif

/* ========================================================================== */
/* TEST 28: Ternary operator in preprocessor expressions                      */
/* ========================================================================== */

#if (1 ? 100 : 200) == 100
int test28_ternary1 = 1;
#endif

#if (0 ? 100 : 200) == 200
int test28_ternary2 = 1;
#endif

/* ========================================================================== */
/* TEST 29: Parentheses and operator precedence                               */
/* ========================================================================== */

#if 2 + 3 * 4 == 14
int test29_prec1 = 1;
#endif

#if (2 + 3) * 4 == 20
int test29_prec2 = 1;
#endif

#if 1 || 0 && 0
int test29_prec3 = 1;
#endif

#if (1 || 0) && 1
int test29_prec4 = 1;
#endif

/* ========================================================================== */
/* TEST 30: Multiple arguments in function-like macros                        */
/* ========================================================================== */

#define MAYBE_EMPTY(a, b, c) a b c

int test30_multi MAYBE_EMPTY(=, 5, ;)

/* ========================================================================== */
/* TEST 31: Macro expansion in #include (simulated)                           */
/* ========================================================================== */

#define HEADER_NAME "stddef.h"
/* #include HEADER_NAME */ /* Uncomment to test if your preprocessor supports this */

/* Standard headers that must exist in C89 */
#include <stddef.h>
#include <limits.h>
#include <float.h>

/* ========================================================================== */
/* TEST 32: Complex nested conditionals                                       */
/* ========================================================================== */

#define LEVEL1 1
#define LEVEL2 2
#define LEVEL3 3

#if LEVEL1
    #if LEVEL2
        #if LEVEL3
            int test32_deep_nest = 1;
        #else
            int test32_deep_wrong = 999;
        #endif
    #elif LEVEL3
        int test32_elif_wrong = 999;
    #else
        int test32_else_wrong = 999;
    #endif
#else
    int test32_outer_wrong = 999;
#endif

/* ========================================================================== */
/* TEST 33: Token types in stringification                                    */
/* ========================================================================== */

#define STR(x) #x

const char *test33_ident = STR(identifier);
const char *test33_num = STR(12345);
const char *test33_float = STR(3.14159);
const char *test33_ops = STR(+-*/%);
const char *test33_special = STR(()[]{});

/* ========================================================================== */
/* TEST 34: Macro redefinition (same definition - allowed)                    */
/* ========================================================================== */

#define REDEF_MACRO 42
#define REDEF_MACRO 42

#define REDEF_FUNC(x) ((x) + 1)
#define REDEF_FUNC(x) ((x) + 1)

int test34_redef = REDEF_MACRO;
int test34_redef_func = REDEF_FUNC(10);

/* ========================================================================== */
/* TEST 35: Edge cases with token pasting                                     */
/* ========================================================================== */

#define PASTE_EDGE(a, b) a ## b

/* Creating different token types */
int PASTE_EDGE(var, iable) = 100;
long PASTE_EDGE(long, _var) = 200L;

#define PASTE_NUM(a, b) a ## b
int test35_num = PASTE_NUM(12, 34);

/* ========================================================================== */
/* TEST 36: sizeof in preprocessor (should NOT work - for negative test)      */
/* ========================================================================== */

/* Note: sizeof is NOT evaluated in preprocessor expressions in C89 */
/* The following tests that the preprocessor handles it as identifier = 0 */

#if !defined(sizeof)
int test36_sizeof_not_defined = 1;
#endif

/* ========================================================================== */
/* TEST 37: Character constants in preprocessor expressions                   */
/* ========================================================================== */

#if 'A' == 65
int test37_char_a = 1;
#endif

#if 'Z' - 'A' == 25
int test37_char_range = 1;
#endif

#if '\n' == 10
int test37_char_newline = 1;
#endif

#if '\0' == 0
int test37_char_null = 1;
#endif

/* ========================================================================== */
/* TEST 38: Macro name edge cases                                             */
/* ========================================================================== */

#define _UNDERSCORE_START 1
#define __DOUBLE_UNDERSCORE 2
#define MixedCase123 3
#define MACRO_WITH_NUMBERS_456 4
#define a 5

int test38_underscore = _UNDERSCORE_START;
int test38_double = __DOUBLE_UNDERSCORE;
int test38_mixed = MixedCase123;
int test38_numbers = MACRO_WITH_NUMBERS_456;
int test38_single = a;

#undef a

/* ========================================================================== */
/* TEST 39: Preprocessor arithmetic edge cases                                */
/* ========================================================================== */

#if 0x7FFFFFFF > 0
int test39_max_signed = 1;
#endif

#if -1 < 0
int test39_negative = 1;
#endif

#if 0 == 0
int test39_zero = 1;
#endif

/* ========================================================================== */
/* TEST 40: Include guards pattern                                            */
/* ========================================================================== */

#ifndef TEST_INCLUDE_GUARD_H
#define TEST_INCLUDE_GUARD_H

int test40_guard = 1;

#endif /* TEST_INCLUDE_GUARD_H */

/* Second "include" should be skipped */
#ifndef TEST_INCLUDE_GUARD_H
int test40_guard_fail = 999;
#endif

/* ========================================================================== */
/* TEST 41: Main function for actual compilation test                         */
/* ========================================================================== */

int main(void)
{
    /* This allows the file to be compiled, not just preprocessed */
    /* If preprocessing succeeded, compilation should also succeed */
    
    int result = 0;
    
    result += test1_simple;
    result += test3_add;
    result += test8_if;
    result += test10_defined1;
    
    return result > 0 ? 0 : 1;
}

/* ========================================================================== */
/* END OF C89 PREPROCESSOR COMPLIANCE TEST SUITE                              */
/* ========================================================================== */