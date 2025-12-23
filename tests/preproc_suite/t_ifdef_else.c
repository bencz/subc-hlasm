/*
 * Test: #ifdef with #else
 */

#define HAVE_FEATURE 1

int result1;
int result2;

int main(void) {
#ifdef HAVE_FEATURE
    result1 = 100;
#else
    result1 = 200;
#endif

#ifdef NO_FEATURE
    result2 = 100;
#else
    result2 = 200;
#endif

    if (result1 != 100) return 1;
    if (result2 != 200) return 2;
    
    return 0;
}
