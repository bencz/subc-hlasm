/*
 * Test: Nested #if directives
 */

#define OUTER 1
#define INNER 1

#if OUTER
int level0 = 1;
    #if INNER
    int level1 = 1;
    #else
    int level1 = 0;
    #endif
#else
int level0 = 0;
int level1 = 0;
#endif

/* Nested with #elif */
#if 0
int nest1 = 0;
#elif 1
    #if 1
    int nest1 = 1;
    #else
    int nest1 = 2;
    #endif
#else
int nest1 = 3;
#endif

/* Deep nesting */
#if 1
    #if 1
        #if 1
        int deep = 1;
        #endif
    #endif
#endif

int main(void) {
    if (level0 != 1) return 1;
    if (level1 != 1) return 2;
    if (nest1 != 1) return 3;
    if (deep != 1) return 4;
    return 0;
}
