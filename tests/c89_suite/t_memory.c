/*
 * Test: Memory functions (string.h)
 * Tests: memset, memcpy, memcmp
 */
#include <stdio.h>
#include <string.h>

int main(void) {
    char buf1[20];
    char buf2[20];
    int passed;
    int i;

    passed = 1;

    /* memset */
    memset(buf1, 'A', 10);
    buf1[10] = '\0';
    for (i = 0; i < 10; i++) {
        if (buf1[i] != 'A') {
            printf("FAIL: memset buf1[%d] = %c\n", i, buf1[i]);
            passed = 0;
            break;
        }
    }

    /* memset with zero */
    memset(buf1, 0, 10);
    for (i = 0; i < 10; i++) {
        if (buf1[i] != 0) {
            printf("FAIL: memset zero buf1[%d] = %d\n", i, buf1[i]);
            passed = 0;
            break;
        }
    }

    /* memcpy */
    buf1[0] = 'H';
    buf1[1] = 'e';
    buf1[2] = 'l';
    buf1[3] = 'l';
    buf1[4] = 'o';
    buf1[5] = '\0';
    memcpy(buf2, buf1, 6);
    if (strcmp(buf2, "Hello") != 0) {
        printf("FAIL: memcpy: buf2 = \"%s\"\n", buf2);
        passed = 0;
    }

    /* memcmp equal */
    buf1[0] = 'A';
    buf1[1] = 'B';
    buf1[2] = 'C';
    buf2[0] = 'A';
    buf2[1] = 'B';
    buf2[2] = 'C';
    if (memcmp(buf1, buf2, 3) != 0) {
        printf("FAIL: memcmp equal\n");
        passed = 0;
    }

    /* memcmp different */
    buf2[1] = 'X';
    if (memcmp(buf1, buf2, 3) == 0) {
        printf("FAIL: memcmp should be different\n");
        passed = 0;
    }

    if (passed) {
        printf("PASS: t_memory\n");
    }
    return passed ? 0 : 1;
}
