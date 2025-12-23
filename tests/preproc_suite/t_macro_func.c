/*
 * Test: Function-like macros (parameterized macros)
 */

#define DOUBLE(x) ((x) + (x))
#define ADD(a, b) ((a) + (b))
#define MUL(a, b) ((a) * (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define SQUARE(x) ((x) * (x))

int main(void) {
    int x;
    int y;
    int z;
    
    x = DOUBLE(5);
    if (x != 10) return 1;
    
    y = ADD(3, 4);
    if (y != 7) return 2;
    
    z = MUL(3, 4);
    if (z != 12) return 3;
    
    x = MAX(10, 20);
    if (x != 20) return 4;
    
    y = MAX(30, 15);
    if (y != 30) return 5;
    
    z = SQUARE(5);
    if (z != 25) return 6;
    
    return 0;
}
