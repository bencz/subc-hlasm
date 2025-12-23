/*
 * Test: Structures
 * Tests: struct declaration, member access, pointers to struct
 */
#include <stdio.h>

struct point {
    int x;
    int y;
};

struct rect {
    struct point topleft;
    struct point botright;
};

int main(void) {
    struct point p1;
    struct point *pp;
    struct rect r;
    int passed;

    passed = 1;

    /* Basic struct */
    p1.x = 10;
    p1.y = 20;
    if (p1.x != 10 || p1.y != 20) {
        printf("FAIL: p1.x=%d, p1.y=%d\n", p1.x, p1.y);
        passed = 0;
    }

    /* Pointer to struct */
    pp = &p1;
    if (pp->x != 10 || pp->y != 20) {
        printf("FAIL: pp->x=%d, pp->y=%d\n", pp->x, pp->y);
        passed = 0;
    }

    /* Modify through pointer */
    pp->x = 100;
    pp->y = 200;
    if (p1.x != 100 || p1.y != 200) {
        printf("FAIL: after pp->: p1.x=%d, p1.y=%d\n", p1.x, p1.y);
        passed = 0;
    }

    /* Nested struct */
    r.topleft.x = 0;
    r.topleft.y = 0;
    r.botright.x = 100;
    r.botright.y = 50;
    if (r.topleft.x != 0 || r.botright.x != 100) {
        printf("FAIL: nested struct\n");
        passed = 0;
    }

    if (passed) {
        printf("PASS: t_struct\n");
    }
    return passed ? 0 : 1;
}
