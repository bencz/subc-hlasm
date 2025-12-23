/* Test parameter passing */
#include <stdio.h>

int sum2(int a, int b) {
    return a + b;
}

int sum4(int a, int b, int c, int d) {
    return a + b + c + d;
}

int sum8(int a, int b, int c, int d, int e, int f, int g, int h) {
    return a + b + c + d + e + f + g + h;
}

int sum9(int a, int b, int c, int d, int e, int f, int g, int h, int i) {
    return a + b + c + d + e + f + g + h + i;
}

int sum10(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j) {
    return a + b + c + d + e + f + g + h + i + j;
}

int main(void) {
    int r;
    
    r = sum2(1, 2);
    printf("sum2(1,2) = %d (exp 3)\n", r);
    
    r = sum4(1, 2, 3, 4);
    printf("sum4(1,2,3,4) = %d (exp 10)\n", r);
    
    r = sum8(1, 2, 3, 4, 5, 6, 7, 8);
    printf("sum8(1..8) = %d (exp 36)\n", r);
    
    r = sum9(1, 2, 3, 4, 5, 6, 7, 8, 9);
    printf("sum9(1..9) = %d (exp 45)\n", r);
    
    r = sum10(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
    printf("sum10(1..10) = %d (exp 55)\n", r);
    
    return 0;
}
