/*
 * Test: #if with ternary operator
 */

#if (1 ? 5 : 10) == 5
int tern1 = 1;
#else
int tern1 = 0;
#endif

#if (0 ? 5 : 10) == 10
int tern2 = 1;
#else
int tern2 = 0;
#endif

/* Nested ternary */
#if (1 ? (0 ? 1 : 2) : 3) == 2
int tern3 = 1;
#else
int tern3 = 0;
#endif

int main(void) {
    if (!tern1) return 1;
    if (!tern2) return 2;
    if (!tern3) return 3;
    return 0;
}
