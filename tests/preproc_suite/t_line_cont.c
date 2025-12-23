/*
 * Test: Line continuation with backslash
 */

#define MULTILINE_MACRO(x, y) \
    ((x) + \
     (y))

#define LONG_MACRO \
    1 + 2 + 3 + \
    4 + 5 + 6

int main(void) {
    int a;
    int b;
    
    a = MULTILINE_MACRO(10, 20);
    b = LONG_MACRO;
    
    if (a != 30) return 1;
    if (b != 21) return 2;
    
    return 0;
}
