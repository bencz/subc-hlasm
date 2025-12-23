/*
 * Test: Nested #ifdef directives
 */

#define OUTER 1
#define INNER 1

int level0;
int level1;
int level2;

int main(void) {
    level0 = 0;
    level1 = 0;
    level2 = 0;

#ifdef OUTER
    level0 = 1;
    
    #ifdef INNER
        level1 = 1;
        
        #ifdef DEEPEST
            level2 = 1;  /* Should NOT execute */
        #endif
    #endif
#endif

    if (level0 != 1) return 1;
    if (level1 != 1) return 2;
    if (level2 != 0) return 3;
    
    return 0;
}
