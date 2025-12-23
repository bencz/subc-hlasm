/*
 * Test: Basic #define functionality
 *
 * Tests simple object-like macros.
 */

#define VALUE 42
#define ZERO 0
#define NEGATIVE -1

int main(void) {
    int x;
    int y;
    int z;
    
    x = VALUE;
    y = ZERO;
    z = NEGATIVE;
    
    if (x != 42) return 1;
    if (y != 0) return 2;
    if (z != -1) return 3;
    
    return 0;
}

/* EXPECT_ASM: 42 */
