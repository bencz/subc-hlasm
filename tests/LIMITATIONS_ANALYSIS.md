# SubC Compiler - Limitations

## Version: 2025-12-23

This document describes the **intentional design limitations** of the SubC
compiler. These are not bugs - they are deliberate simplifications that make
SubC a small, fast, and self-compiling C compiler.

For bugs, see the `BUGS` file. For test results, run `tests/c89_suite/run_tests.sh`.

---

## 1. Language Limitations

### 1.1 Maximum Two Levels of Indirection

SubC supports at most two levels of pointer indirection.

```c
int *p;      /* OK */
int **pp;    /* OK */
int ***ppp;  /* NOT supported */
```

### 1.2 One-Dimensional Arrays Only

SubC only supports one-dimensional arrays.

```c
int a[10];       /* OK */
int b[10][20];   /* NOT supported */
```

**Workaround**: Use pointer arithmetic or flatten to 1D.

### 1.3 No goto Statement

The `goto` keyword is not recognized.

### 1.4 No Struct/Union by Value

Structures and unions cannot be passed or returned by value.

```c
struct point { int x, y; };

struct point make_point(int x, int y);  /* NOT supported */
void use_point(struct point p);         /* NOT supported */

struct point *make_point(int x, int y); /* OK - return pointer */
void use_point(struct point *p);        /* OK - pass pointer */
```

### 1.5 No Parameterized Macros

The preprocessor does not support function-like macros.

```c
#define MAX(a,b) ((a)>(b)?(a):(b))  /* NOT supported */
#define PI 3.14159                   /* OK - object-like macro */
```

### 1.6 Function-Level Declarations Only

Variable declarations are only allowed at the start of functions,
not inside blocks.

```c
void foo(void) {
    int x;           /* OK - function start */
    x = 1;
    if (x) {
        int y;       /* NOT supported - block scope */
        y = 2;
    }
}
```

### 1.7 Enum Variable Declarations

Enum types cannot be used directly in variable declarations.

```c
enum color { RED, GREEN, BLUE };
enum color c;   /* NOT supported */
int c;          /* OK - use int instead */
c = GREEN;      /* OK */
```

---

## 2. Type Limitations

### 2.1 Function Pointer Return Types

Internally, all function pointers are treated as returning `int`.

### 2.2 Limited Pointer Casts

Some pointer cast expressions may not work:

```c
int *pi;
char *pc;
pc = (char *)pi;  /* May fail in some contexts */
```

### 2.3 Complex Typedef Patterns

Advanced typedef patterns with function pointers in structs may not work:

```c
typedef int (*BinaryOp)(int, int);
typedef struct {
    BinaryOp operation;  /* May not work */
} NamedOp;
```

---

## 3. Preprocessor Limitations

- No token pasting (`##`) or stringification (`#`)
- No line continuation with `\`
- No `#warning`

---

## 4. Bootstrap Limitations

### 4.1 System Headers

SubC cannot compile files that include system headers directly.
For self-compilation, use SubC's own headers in `runtime/include/`.

### 4.2 Runtime Library

The SubC runtime library (`runtime/lib/`) must be compiled separately
for each target platform. Currently only DOS has a complete runtime.
