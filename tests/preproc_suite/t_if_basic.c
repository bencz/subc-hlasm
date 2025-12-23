/*
 * Test: Basic #if functionality
 */

#if 1
int test1 = 1;
#else
int test1 = 0;
#endif

#if 0
int test2 = 1;
#else
int test2 = 0;
#endif

#if 1 + 1
int test3 = 1;
#endif

#if 0 + 0
int test4 = 1;
#else
int test4 = 0;
#endif

int main(void) {
    if (test1 != 1) return 1;
    if (test2 != 0) return 2;
    if (test3 != 1) return 3;
    if (test4 != 0) return 4;
    return 0;
}
