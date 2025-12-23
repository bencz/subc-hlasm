/*
 * Test: // comments in macros are stripped
 *
 * SubC extension: // comments should be stripped from macro definitions.
 */

#define VALUE 42 // This comment should be stripped

int main(void) {
    int x;
    x = VALUE;
    
    if (x != 42) return 1;
    
    return 0;
}
