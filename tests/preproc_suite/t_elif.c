/*
 * Test: #elif functionality
 */

#define VALUE 2

/* Basic #elif chain */
#if VALUE == 1
int result1 = 1;
#elif VALUE == 2
int result1 = 2;
#elif VALUE == 3
int result1 = 3;
#else
int result1 = 0;
#endif

/* First condition true */
#if 1
int result2 = 1;
#elif 1
int result2 = 2;
#else
int result2 = 0;
#endif

/* Second condition true */
#if 0
int result3 = 1;
#elif 1
int result3 = 2;
#else
int result3 = 0;
#endif

/* No condition true */
#if 0
int result4 = 1;
#elif 0
int result4 = 2;
#else
int result4 = 3;
#endif

/* Multiple #elif */
#if 0
int result5 = 1;
#elif 0
int result5 = 2;
#elif 0
int result5 = 3;
#elif 1
int result5 = 4;
#else
int result5 = 0;
#endif

int main(void) {
    if (result1 != 2) return 1;
    if (result2 != 1) return 2;
    if (result3 != 2) return 3;
    if (result4 != 3) return 4;
    if (result5 != 4) return 5;
    return 0;
}
