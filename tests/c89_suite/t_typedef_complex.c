/*
 * Test: Complex typedef usage
 * Tests: typedef for structs, pointers, function pointers, arrays
 */
#include <stdio.h>
#include <string.h>

/* Basic typedefs */
typedef int Int32;
typedef char Byte;
typedef char *String;

/* Typedef for struct */
struct point_s {
    int x;
    int y;
};
typedef struct point_s Point;

/* Typedef for pointer to struct */
typedef struct point_s *PointPtr;

/* Typedef for struct with typedef inside */
typedef struct {
    Point start;
    Point end;
} Line;

/* Typedef for function pointer */
typedef int (*BinaryOp)(int, int);
typedef void (*Callback)(int);

/* Typedef for array */
typedef int IntArray10[10];
typedef char Buffer[64];

/* Complex: typedef for struct containing function pointer */
typedef struct {
    char name[16];
    BinaryOp operation;
} NamedOp;

/* Functions for testing */
int add_fn(int a, int b) { return a + b; }
int sub_fn(int a, int b) { return a - b; }
int mul_fn(int a, int b) { return a * b; }

int g_callback_value;
void my_callback(int v) { g_callback_value = v; }

/* Function using typedef types */
Int32 distance_squared(PointPtr p1, PointPtr p2) {
    Int32 dx, dy;
    dx = p2->x - p1->x;
    dy = p2->y - p1->y;
    return dx * dx + dy * dy;
}

int apply_op(BinaryOp op, int a, int b) {
    return op(a, b);
}

int main(void) {
    Int32 i;
    Byte b;
    String s;
    Point p1, p2;
    PointPtr pp;
    Line line;
    BinaryOp op;
    Callback cb;
    IntArray10 arr;
    Buffer buf;
    NamedOp ops[3];
    int result, sum;
    int passed;

    passed = 1;

    /* Basic typedef */
    i = 42;
    if (i != 42) {
        printf("FAIL: Int32 i = %d\n", i);
        passed = 0;
    }

    b = 'X';
    if (b != 'X') {
        printf("FAIL: Byte b = %c\n", b);
        passed = 0;
    }

    /* String typedef */
    s = "Hello";
    if (strcmp(s, "Hello") != 0) {
        printf("FAIL: String s = %s\n", s);
        passed = 0;
    }

    /* Point typedef */
    p1.x = 0;
    p1.y = 0;
    p2.x = 3;
    p2.y = 4;

    /* PointPtr typedef */
    pp = &p1;
    pp->x = 10;
    if (p1.x != 10) {
        printf("FAIL: PointPtr: p1.x = %d\n", p1.x);
        passed = 0;
    }

    /* Function using typedef params */
    p1.x = 0;
    p1.y = 0;
    result = distance_squared(&p1, &p2);
    /* 3^2 + 4^2 = 25 */
    if (result != 25) {
        printf("FAIL: distance_squared = %d (expected 25)\n", result);
        passed = 0;
    }

    /* Line typedef (nested Point) */
    line.start.x = 0;
    line.start.y = 0;
    line.end.x = 100;
    line.end.y = 100;
    if (line.end.x != 100) {
        printf("FAIL: Line: line.end.x = %d\n", line.end.x);
        passed = 0;
    }

    /* Function pointer typedef */
    op = add_fn;
    result = op(5, 3);
    if (result != 8) {
        printf("FAIL: BinaryOp add = %d (expected 8)\n", result);
        passed = 0;
    }

    op = mul_fn;
    result = op(5, 3);
    if (result != 15) {
        printf("FAIL: BinaryOp mul = %d (expected 15)\n", result);
        passed = 0;
    }

    /* Function pointer through apply_op */
    result = apply_op(sub_fn, 10, 3);
    if (result != 7) {
        printf("FAIL: apply_op(sub,10,3) = %d (expected 7)\n", result);
        passed = 0;
    }

    /* Callback typedef */
    cb = my_callback;
    g_callback_value = 0;
    cb(123);
    if (g_callback_value != 123) {
        printf("FAIL: Callback: g_callback_value = %d\n", g_callback_value);
        passed = 0;
    }

    /* Array typedef */
    for (i = 0; i < 10; i++) {
        arr[i] = i * i;
    }
    if (arr[5] != 25) {
        printf("FAIL: IntArray10: arr[5] = %d (expected 25)\n", arr[5]);
        passed = 0;
    }

    /* Buffer typedef */
    strcpy(buf, "Test buffer");
    if (strcmp(buf, "Test buffer") != 0) {
        printf("FAIL: Buffer: buf = %s\n", buf);
        passed = 0;
    }

    /* NamedOp - struct with function pointer */
    strcpy(ops[0].name, "add");
    ops[0].operation = add_fn;
    strcpy(ops[1].name, "sub");
    ops[1].operation = sub_fn;
    strcpy(ops[2].name, "mul");
    ops[2].operation = mul_fn;

    /* Call through NamedOp array */
    result = ops[0].operation(10, 5);
    if (result != 15) {
        printf("FAIL: ops[0].operation = %d (expected 15)\n", result);
        passed = 0;
    }

    result = ops[2].operation(4, 5);
    if (result != 20) {
        printf("FAIL: ops[2].operation = %d (expected 20)\n", result);
        passed = 0;
    }

    /* Sum using all ops */
    sum = 0;
    for (i = 0; i < 3; i++) {
        sum += ops[i].operation(10, 5);
    }
    /* add(10,5)=15 + sub(10,5)=5 + mul(10,5)=50 = 70 */
    if (sum != 70) {
        printf("FAIL: sum of ops = %d (expected 70)\n", sum);
        passed = 0;
    }

    if (passed) {
        printf("PASS: t_typedef_complex\n");
    }
    return passed ? 0 : 1;
}
