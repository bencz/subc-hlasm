# SubC Compiler - Limitations Analysis

## Version: 2025-12-23 (Updated)

This document lists the limitations and known issues in the SubC compiler,
discovered through the C89 conformance test suite (`tests/c89_suite/`).

---

## 1. Critical Bugs

### 1.1 Infinite Loop on Valid C89 Code

The compiler enters an infinite loop (hangs) when compiling certain valid C89
constructs. This requires killing the process. Affected patterns include:

| Test File | Pattern |
|-----------|---------|
| `t_array_complex.c` | Complex array operations with pointer arrays |
| `t_const_expr.c` | Constant expressions in array sizes |
| `t_global.c` | Global array initialization with values |
| `t_state_machine.c` | Enum with function pointer arrays |

**Severity**: Critical - compiler must be killed
**Status**: Unresolved

---

## 1.2 Functions with Many Parameters (32+)

Functions with more than 8 parameters (stack-passed parameters) may produce
incorrect results on AArch64. The stack parameter offset calculation needs
review.

**Severity**: Medium - affects only functions with many parameters
**Status**: Under investigation

---

## 2. C89 Features Not Supported

These are valid C89 features that SubC does not support:

### 2.1 Block-Scoped Declarations

C89 allows variable declarations at the beginning of any compound statement
(block), not just at function level. SubC only supports declarations at the
start of functions.

```c
/* Valid C89, fails in SubC */
if (condition) {
    int x;  /* SubC: "syntax error at: int" */
    x = 5;
}
```

**Affected tests**: `t_for.c`, `t_pointer.c`, `t_pointer2.c`, `t_scope.c`,
`t_recursion.c`, `t_linked_list.c`, `t_struct_complex.c`, `t_typedef_complex.c`

### 2.2 Enum Variable Declarations

SubC has issues with enum type variables:

```c
enum color { RED, GREEN, BLUE };
enum color c;  /* SubC: "'{' expected" */
c = GREEN;     /* SubC: "lvalue expected" */
```

**Affected tests**: `t_enum.c`, `t_state_machine.c`

### 2.3 Pointer Cast in Expressions

Certain pointer casts in expressions fail:

```c
pc = (char *)pi;  /* SubC: "invalid operands to binary operator" */
```

**Affected tests**: `t_cast.c`

### 2.4 Complex Typedef with Function Pointers

Advanced typedef patterns with function pointers in structs:

```c
typedef int (*BinaryOp)(int, int);
typedef struct {
    BinaryOp operation;
} NamedOp;
```

**Affected tests**: `t_callback.c`, `t_typedef_complex.c`, `t_vtable.c`

### 2.5 String Library Functions with Pointers

Some string operations with pointer returns:

**Affected tests**: `t_string.c`

---

## 3. Design Limitations (Documented)

These are intentional SubC limitations, not bugs:

| Limitation | Description |
|------------|-------------|
| Max 2 levels of indirection | `int **` valid, `int ***` invalid |
| 1D arrays only | `int a[10]` valid, `int a[10][20]` invalid |
| No goto | `goto` keyword not recognized |
| Struct/union by value | Use pointers: `void fn(struct x *p)` |
| No parameterized macros | `#define FOO(x)` not supported |

---

## 4. Alignment Fields (Future)

| Field | Status |
|-------|--------|
| `align_stack` | Defined, future use for strict alignment architectures |
| `align_data` | Defined, future use for data alignment |
| `align_func` | Defined, future use for function alignment |

**Priority**: Low - current architectures (x86, ARM) work without this.
Required for IBM S/370 / z/Architecture.

---

## 5. Bootstrap Limitations

### 5.1 System Includes

SubC cannot compile files that include system headers.
For bootstrap, use SubC's own headers in `runtime/include/`.

### 5.2 Function Pointer Return Types

SubC treats all function pointers as returning `int` internally.

---

## 6. Test Suite Results Summary

From `tests/c89_suite/` with 49 tests:

| Result | Count | Percentage |
|--------|-------|------------|
| Passed | 32 | 65% |
| Failed (compile) | 13 | 27% |
| Failed (timeout) | 4 | 8% |

### Passing Tests (32)
`t_arithmetic`, `t_array`, `t_array_ptr`, `t_assignment`, `t_binary_search`,
`t_bitfield_sim`, `t_bitwise`, `t_char`, `t_comma`, `t_comparison`, `t_ctype`,
`t_do_while`, `t_fnptr`, `t_function`, `t_if_else`, `t_increment`, `t_literals`,
`t_logical`, `t_memory`, `t_precedence`, `t_preproc`, `t_printf`, `t_sizeof`,
`t_static`, `t_stdlib`, `t_struct`, `t_switch`, `t_ternary`, `t_typedef`,
`t_union`, `t_void`, `t_while`

### Failed Tests (17)
- **Timeout (4)**: `t_array_complex`, `t_const_expr`, `t_global`, `t_state_machine`
- **Block declarations (8)**: `t_for`, `t_pointer`, `t_pointer2`, `t_scope`,
  `t_recursion`, `t_linked_list`, `t_struct_complex`, `t_typedef_complex`
- **Enum issues (1)**: `t_enum`
- **Cast issues (1)**: `t_cast`
- **Callback/vtable (2)**: `t_callback`, `t_vtable`
- **String ops (1)**: `t_string`

---

## 7. Recently Fixed Issues

### 7.1 Variadic Functions on Darwin/AArch64 (FIXED 2025-12-23)

**Problem**: `printf()` and other variadic functions produced garbage output on
Darwin/AArch64 (Apple Silicon).

**Cause**: Darwin AArch64 ABI requires variadic arguments to be passed on the
stack, not in registers. SubC was passing all arguments in registers (x0-x7).

**Solution**: Added `cgpusharg_vararg` to the vtable. On Darwin/AArch64, variadic
arguments are now pushed to the stack. Other architectures use the default
behavior (registers where applicable).

**Files changed**: `cgtarget.h`, `cgen_compat.h`, `tree.c`, `cg_aarch64.c`,
and NULL entries added to other architecture vtables.

---

## 8. Priority for Fixes

| Issue | Severity | Priority |
|-------|----------|----------|
| Infinite loop bug | Critical | High |
| Block-scoped declarations | High | Medium |
| Enum variables | Medium | Medium |
| Pointer casts | Medium | Low |
| Complex typedefs | Low | Low |
