/*
 * Test: Printf formatting
 * Tests: %d, %c, %s, %x, %o, %%
 */
#include <stdio.h>
#include <string.h>

int main(void) {
    char buf[100];
    int passed;

    passed = 1;

    /* %d - decimal */
    sprintf(buf, "%d", 42);
    if (strcmp(buf, "42") != 0) {
        printf("FAIL: %%d: \"%s\" (expected \"42\")\n", buf);
        passed = 0;
    }

    /* %d - negative */
    sprintf(buf, "%d", -123);
    if (strcmp(buf, "-123") != 0) {
        printf("FAIL: %%d negative: \"%s\"\n", buf);
        passed = 0;
    }

    /* %c - character */
    sprintf(buf, "%c", 'A');
    if (strcmp(buf, "A") != 0) {
        printf("FAIL: %%c: \"%s\"\n", buf);
        passed = 0;
    }

    /* %s - string */
    sprintf(buf, "%s", "Hello");
    if (strcmp(buf, "Hello") != 0) {
        printf("FAIL: %%s: \"%s\"\n", buf);
        passed = 0;
    }

    /* %x - hex */
    sprintf(buf, "%x", 255);
    if (strcmp(buf, "ff") != 0) {
        printf("FAIL: %%x: \"%s\" (expected \"ff\")\n", buf);
        passed = 0;
    }

    /* %o - octal */
    sprintf(buf, "%o", 8);
    if (strcmp(buf, "10") != 0) {
        printf("FAIL: %%o: \"%s\" (expected \"10\")\n", buf);
        passed = 0;
    }

    /* %% - literal percent */
    sprintf(buf, "100%%");
    if (strcmp(buf, "100%") != 0) {
        printf("FAIL: %%%%: \"%s\"\n", buf);
        passed = 0;
    }

    /* Combined */
    sprintf(buf, "%s=%d", "x", 10);
    if (strcmp(buf, "x=10") != 0) {
        printf("FAIL: combined: \"%s\"\n", buf);
        passed = 0;
    }

    if (passed) {
        printf("PASS: t_printf\n");
    }
    return passed ? 0 : 1;
}
