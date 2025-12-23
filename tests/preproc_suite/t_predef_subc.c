/*
 * Test: Predefined __SUBC__ macro
 * 
 * The __SUBC__ macro should always be defined when compiling with SubC.
 */

#ifdef __SUBC__
int subc_defined = 1;
#else
int subc_defined = 0;
#endif

int main(void) {
    return subc_defined ? 0 : 1;
}

/* EXPECT_ASM: subc_defined */
