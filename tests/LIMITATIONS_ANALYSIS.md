# SubC Compiler - Limitations

## Version: 2025-12-23

**IMPORTANT**: This document contains ONLY the current limitations of SubC.
Do NOT add resolved features here. When a limitation is fixed, REMOVE it from
this file. For changelog of resolved issues, see the `Changes` file.

These are **intentional design limitations** - deliberate simplifications that
make SubC a small, fast, and self-compiling C compiler.

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

### 1.5 Global Initializers Limited to Constants

Global variables can only be initialized with constant expressions.
Ternary operator (`?:`) and function calls are not allowed in global initializers.

```c
int x = 10;                      /* OK - constant */
int y = 10 + 20;                 /* OK - constant expression */
int z = (10 > 5) ? 10 : 5;       /* NOT supported - ternary in global */
int w = MAX(10, 20);             /* NOT supported - macro with ternary */
```

**Workaround**: Initialize in `main()` or use simpler expressions.

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

- No `#warning` directive (not part of C89, but common extension)
- Macros with empty argument list `MACRO()` may not work correctly
- `#line` directive with filename argument not fully supported
- Comments not allowed on `#include` lines: `#include <file.h> /* comment */` fails

---

## 4. Type Qualifier Limitations

### `const` Keyword

The `const` keyword is recognized but **not enforced** by SubC.
You can declare `const` variables, but the compiler won't prevent modifications.

```c
const char *str = "hello";  /* OK - compiles */
const int value = 42;       /* OK - compiles */
```

### `volatile` Keyword

The `volatile` keyword is recognized but ignored by SubC.

---

## 5. Bootstrap Limitations

### 5.1 System Headers

SubC cannot compile files that include system headers directly.
For self-compilation, use SubC's own headers in `runtime/include/`.

### 5.2 Runtime Library

The SubC runtime library (`runtime/lib/`) must be compiled separately
for each target platform. Currently only DOS has a complete runtime.
