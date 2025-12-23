/*
 * Test: Character classification (ctype.h)
 * Tests: isalpha, isdigit, isspace, toupper, tolower
 */
#include <stdio.h>
#include <ctype.h>

int main(void) {
    int passed;

    passed = 1;

    /* isalpha */
    if (!isalpha('A') || !isalpha('z')) {
        printf("FAIL: isalpha\n");
        passed = 0;
    }
    if (isalpha('5') || isalpha(' ')) {
        printf("FAIL: isalpha should be false for non-alpha\n");
        passed = 0;
    }

    /* isdigit */
    if (!isdigit('0') || !isdigit('9')) {
        printf("FAIL: isdigit\n");
        passed = 0;
    }
    if (isdigit('A') || isdigit(' ')) {
        printf("FAIL: isdigit should be false for non-digit\n");
        passed = 0;
    }

    /* isspace */
    if (!isspace(' ') || !isspace('\t') || !isspace('\n')) {
        printf("FAIL: isspace\n");
        passed = 0;
    }
    if (isspace('A') || isspace('5')) {
        printf("FAIL: isspace should be false for non-space\n");
        passed = 0;
    }

    /* isalnum */
    if (!isalnum('A') || !isalnum('5')) {
        printf("FAIL: isalnum\n");
        passed = 0;
    }
    if (isalnum(' ') || isalnum('!')) {
        printf("FAIL: isalnum should be false\n");
        passed = 0;
    }

    /* isupper/islower */
    if (!isupper('A') || isupper('a')) {
        printf("FAIL: isupper\n");
        passed = 0;
    }
    if (!islower('a') || islower('A')) {
        printf("FAIL: islower\n");
        passed = 0;
    }

    /* toupper/tolower */
    if (toupper('a') != 'A') {
        printf("FAIL: toupper('a') = %c\n", toupper('a'));
        passed = 0;
    }
    if (tolower('A') != 'a') {
        printf("FAIL: tolower('A') = %c\n", tolower('A'));
        passed = 0;
    }

    if (passed) {
        printf("PASS: t_ctype\n");
    }
    return passed ? 0 : 1;
}
