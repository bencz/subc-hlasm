/*
 * Test: #define with string values
 */

#define MESSAGE "Hello"
#define EMPTY ""

char *msg;
char *empty;

int main(void) {
    msg = MESSAGE;
    empty = EMPTY;
    
    /* Just check it compiles and runs */
    if (msg[0] != 'H') return 1;
    if (empty[0] != '\0') return 2;
    
    return 0;
}
