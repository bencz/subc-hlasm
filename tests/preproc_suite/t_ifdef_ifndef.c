/*
 * Test: #ifdef and #ifndef
 */

#define DEFINED_MACRO 1

int test1;
int test2;
int test3;
int test4;

int main(void) {
    test1 = 0;
    test2 = 0;
    test3 = 0;
    test4 = 0;

#ifdef DEFINED_MACRO
    test1 = 1;  /* Should execute */
#endif

#ifdef UNDEFINED_MACRO
    test2 = 1;  /* Should NOT execute */
#endif

#ifndef UNDEFINED_MACRO
    test3 = 1;  /* Should execute */
#endif

#ifndef DEFINED_MACRO
    test4 = 1;  /* Should NOT execute */
#endif

    if (test1 != 1) return 1;
    if (test2 != 0) return 2;
    if (test3 != 1) return 3;
    if (test4 != 0) return 4;
    
    return 0;
}
