/*
 * Test: Macro redefinition with same value (should be OK)
 *
 * C89 allows redefinition if the replacement list is identical.
 */

#define SAME_VALUE 100
#define SAME_VALUE 100

int main(void) {
    int x;
    x = SAME_VALUE;
    
    if (x != 100) return 1;
    
    return 0;
}
