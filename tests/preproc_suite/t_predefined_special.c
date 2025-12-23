/*
 * Test: Special predefined macros (__FILE__, __LINE__, __DATE__, __TIME__)
 * These are C89 mandatory predefined macros.
 */

int main(void) {
    char *file;
    char *date;
    char *time_str;
    int line1;
    int line2;
    
    file = __FILE__;
    line1 = __LINE__;
    line2 = __LINE__;
    date = __DATE__;
    time_str = __TIME__;
    
    /* __LINE__ should give different values on different lines */
    if (line1 == line2) return 1;
    
    /* If this compiles, all special macros work */
    return 0;
}
