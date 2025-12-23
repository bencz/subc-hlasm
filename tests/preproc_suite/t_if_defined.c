/*
 * Test: #if with defined() operator
 */

#define MACRO1 1
#define MACRO2

/* defined with parentheses */
#if defined(MACRO1)
int def1 = 1;
#else
int def1 = 0;
#endif

#if defined(MACRO2)
int def2 = 1;
#else
int def2 = 0;
#endif

#if defined(UNDEFINED_MACRO)
int def3 = 1;
#else
int def3 = 0;
#endif

/* defined without parentheses */
#if defined MACRO1
int def4 = 1;
#else
int def4 = 0;
#endif

/* Combining defined with logical operators */
#if defined(MACRO1) && defined(MACRO2)
int def5 = 1;
#else
int def5 = 0;
#endif

#if defined(MACRO1) || defined(UNDEFINED_MACRO)
int def6 = 1;
#else
int def6 = 0;
#endif

#if !defined(UNDEFINED_MACRO)
int def7 = 1;
#else
int def7 = 0;
#endif

int main(void) {
    if (!def1) return 1;
    if (!def2) return 2;
    if (def3) return 3;
    if (!def4) return 4;
    if (!def5) return 5;
    if (!def6) return 6;
    if (!def7) return 7;
    return 0;
}
