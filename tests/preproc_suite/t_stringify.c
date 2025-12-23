/*
 * Test: Stringification operator (#)
 */

#define STRINGIFY(x) #x
#define XSTRINGIFY(x) STRINGIFY(x)

#define VALUE 12345

int main(void) {
    char *s1;
    char *s2;
    char *s3;
    
    s1 = STRINGIFY(hello);
    s2 = STRINGIFY(123);
    s3 = XSTRINGIFY(VALUE);
    
    /* If this compiles, stringification works */
    return 0;
}
