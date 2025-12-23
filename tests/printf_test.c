/* Test with printf */
#include <stdio.h>

int sum2(int a, int b) {
    return a + b;
}

int main(void) {
    int r;
    r = sum2(10, 20);
    printf("r = %d\n", r);
    return r;
}
