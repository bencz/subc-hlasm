/*
 * Test: String operations (using string.h)
 * Tests: strlen, strcpy, strcmp, strcat
 */
#include <stdio.h>
#include <string.h>

int main(void) {
    char buf[100];
    char *s;
    int len;
    int passed;

    passed = 1;

    /* strlen */
    len = strlen("Hello");
    if (len != 5) {
        printf("FAIL: strlen(\"Hello\") = %d (expected 5)\n", len);
        passed = 0;
    }

    /* strlen empty */
    len = strlen("");
    if (len != 0) {
        printf("FAIL: strlen(\"\") = %d (expected 0)\n", len);
        passed = 0;
    }

    /* strcpy */
    strcpy(buf, "Test");
    if (strcmp(buf, "Test") != 0) {
        printf("FAIL: strcpy failed\n");
        passed = 0;
    }

    /* strcmp equal */
    if (strcmp("abc", "abc") != 0) {
        printf("FAIL: strcmp(\"abc\",\"abc\") != 0\n");
        passed = 0;
    }

    /* strcmp less */
    if (strcmp("abc", "abd") >= 0) {
        printf("FAIL: strcmp(\"abc\",\"abd\") should be < 0\n");
        passed = 0;
    }

    /* strcmp greater */
    if (strcmp("abd", "abc") <= 0) {
        printf("FAIL: strcmp(\"abd\",\"abc\") should be > 0\n");
        passed = 0;
    }

    /* strcat */
    strcpy(buf, "Hello");
    strcat(buf, " World");
    if (strcmp(buf, "Hello World") != 0) {
        printf("FAIL: strcat: buf = \"%s\"\n", buf);
        passed = 0;
    }

    /* strchr */
    s = strchr("Hello", 'l');
    if (s == 0 || *s != 'l') {
        printf("FAIL: strchr failed\n");
        passed = 0;
    }

    /* strchr not found */
    s = strchr("Hello", 'x');
    if (s != 0) {
        printf("FAIL: strchr should return NULL\n");
        passed = 0;
    }

    if (passed) {
        printf("PASS: t_string\n");
    }
    return passed ? 0 : 1;
}
