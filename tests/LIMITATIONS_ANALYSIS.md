# SubC Compiler - Pending Limitations

## Version: 2025-12-23

This document lists the remaining limitations in the SubC compiler.

---

## 1. Bootstrap (Self-compilation)

### 1.1 System Includes

SubC cannot compile files that include system headers:
- `stdlib.h`, `stdio.h`, `string.h`, `ctype.h`, `stddef.h`

For bootstrap, you must use SubC's own headers in `runtime/include/`.

### 1.2 Macros with Pointer Calls

The `cgen_compat.h` uses macros like:
```c
#define cgdata()  (CG->vtable->cgdata())
```

This requires parsing complex expressions with chained `->` and function calls.

### 1.3 Function Pointer Return Types

SubC treats all function pointers as returning `int` internally.
Assignments like `char *str = vtable.fn_retstr("x");` generate type errors.

---

## 2. Language Limitations (By Design)

These are intentional SubC limitations, not bugs:

| Limitation | Description |
|------------|-------------|
| Max 2 levels of indirection | `int **` valid, `int ***` invalid |
| 1D arrays only | `int a[10]` valid, `int a[10][20]` invalid |
| No goto | `goto` keyword not recognized |
| Struct/union by value | Use pointers: `void fn(struct x *p)` |

---

## 3. Alignment Fields (Future)

| Field | Status |
|-------|--------|
| `align_stack` | Defined, future use for strict alignment architectures |
| `align_data` | Defined, future use for data alignment |
| `align_func` | Defined, future use for function alignment |

**Priority**: Low - current architectures (x86, ARM) work without this.
Required for IBM S/370 / z/Architecture.

---

## 4. Summary

| Limitation | Severity | Status |
|------------|----------|--------|
| Bootstrap (includes) | High | Pending |
| Bootstrap (vtable macros) | High | Pending |
| fn ptr return != int | Medium | Pending |
| align_* fields | Low | Future |
| 2 levels indirection | Low | Design |
| 1D arrays | Low | Design |
| No goto | Low | Design |
| Struct by value | Low | Design |
