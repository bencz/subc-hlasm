/*
 * Test: Virtual table pattern (vtable)
 * Tests: struct with function pointers, polymorphism simulation
 */
#include <stdio.h>

/* Forward declarations */
struct shape;

/* Virtual table structure */
struct shape_vtable {
    int (*area)(struct shape *s);
    int (*perimeter)(struct shape *s);
    void (*describe)(struct shape *s);
};

/* Base shape structure */
struct shape {
    struct shape_vtable *vt;
    int type;
};

/* Rectangle "class" */
struct rectangle {
    struct shape_vtable *vt;
    int type;
    int width;
    int height;
};

/* Square "class" */
struct square {
    struct shape_vtable *vt;
    int type;
    int side;
};

/* Rectangle methods */
int rect_area(struct shape *s) {
    struct rectangle *r;
    r = (struct rectangle *)s;
    return r->width * r->height;
}

int rect_perimeter(struct shape *s) {
    struct rectangle *r;
    r = (struct rectangle *)s;
    return 2 * (r->width + r->height);
}

void rect_describe(struct shape *s) {
    struct rectangle *r;
    r = (struct rectangle *)s;
    printf("Rectangle: %dx%d\n", r->width, r->height);
}

/* Square methods */
int square_area(struct shape *s) {
    struct square *sq;
    sq = (struct square *)s;
    return sq->side * sq->side;
}

int square_perimeter(struct shape *s) {
    struct square *sq;
    sq = (struct square *)s;
    return 4 * sq->side;
}

void square_describe(struct shape *s) {
    struct square *sq;
    sq = (struct square *)s;
    printf("Square: %d\n", sq->side);
}

/* Vtables */
struct shape_vtable rect_vt = {
    rect_area,
    rect_perimeter,
    rect_describe
};

struct shape_vtable square_vt = {
    square_area,
    square_perimeter,
    square_describe
};

/* Polymorphic function */
int total_area(struct shape **shapes, int count) {
    int i, total;
    total = 0;
    for (i = 0; i < count; i++) {
        total += shapes[i]->vt->area(shapes[i]);
    }
    return total;
}

int main(void) {
    struct rectangle r1, r2;
    struct square s1;
    struct shape *shapes[3];
    int area, perim, total;
    int passed;

    passed = 1;

    /* Initialize rectangle */
    r1.vt = &rect_vt;
    r1.type = 1;
    r1.width = 10;
    r1.height = 5;

    /* Test rectangle vtable calls */
    area = r1.vt->area((struct shape *)&r1);
    if (area != 50) {
        printf("FAIL: rect area = %d (expected 50)\n", area);
        passed = 0;
    }

    perim = r1.vt->perimeter((struct shape *)&r1);
    if (perim != 30) {
        printf("FAIL: rect perimeter = %d (expected 30)\n", perim);
        passed = 0;
    }

    /* Initialize square */
    s1.vt = &square_vt;
    s1.type = 2;
    s1.side = 7;

    /* Test square vtable calls */
    area = s1.vt->area((struct shape *)&s1);
    if (area != 49) {
        printf("FAIL: square area = %d (expected 49)\n", area);
        passed = 0;
    }

    perim = s1.vt->perimeter((struct shape *)&s1);
    if (perim != 28) {
        printf("FAIL: square perimeter = %d (expected 28)\n", perim);
        passed = 0;
    }

    /* Second rectangle */
    r2.vt = &rect_vt;
    r2.type = 1;
    r2.width = 3;
    r2.height = 4;

    /* Polymorphic array */
    shapes[0] = (struct shape *)&r1;
    shapes[1] = (struct shape *)&s1;
    shapes[2] = (struct shape *)&r2;

    /* Test polymorphic function */
    total = total_area(shapes, 3);
    /* 50 + 49 + 12 = 111 */
    if (total != 111) {
        printf("FAIL: total_area = %d (expected 111)\n", total);
        passed = 0;
    }

    /* Call through array */
    area = shapes[0]->vt->area(shapes[0]);
    if (area != 50) {
        printf("FAIL: shapes[0] area = %d (expected 50)\n", area);
        passed = 0;
    }

    area = shapes[1]->vt->area(shapes[1]);
    if (area != 49) {
        printf("FAIL: shapes[1] area = %d (expected 49)\n", area);
        passed = 0;
    }

    if (passed) {
        printf("PASS: t_vtable\n");
    }
    return passed ? 0 : 1;
}
