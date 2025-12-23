/*
 * Test: Token pasting operator (##)
 */

#define PASTE(a, b) a ## b
#define MAKE_VAR(n) var_ ## n

int PASTE(my, var) = 100;
int MAKE_VAR(1) = 10;
int MAKE_VAR(2) = 20;

int main(void) {
    int result;
    
    result = myvar + var_1 + var_2;
    
    if (result != 130) return 1;
    
    return 0;
}
