/*
 * Test: #undef functionality
 */

#define MACRO1 1
#define MACRO2 2

#undef MACRO1

int main(void) {
#ifdef MACRO1
    /* MACRO1 should be undefined */
    return 1;
#endif

#ifdef MACRO2
    /* MACRO2 should still be defined */
    return 0;
#else
    return 2;
#endif
}
