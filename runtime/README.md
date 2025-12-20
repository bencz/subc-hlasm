# SubC Runtime Library

This directory contains the runtime components needed to compile and run programs with the SubC compiler.

## Directory Structure

```
runtime/
├── include/    # Standard C header files
│   ├── stdio.h
│   ├── stdlib.h
│   ├── string.h
│   └── ...
│
└── lib/        # Runtime library source files
    ├── crt0.s      # C runtime startup code
    ├── libscc.a    # Compiled runtime library (generated)
    ├── malloc.c    # Memory allocation
    ├── printf.c    # Formatted output
    ├── scanf.c     # Formatted input
    └── ...
```

## Building the Runtime Library

From the `src/` directory:

```bash
make lib        # Build libscc.a
make crt0       # Build crt0.o (startup code)
make ncrt0      # Build ncrt0.o (no-init startup code)
```

## Installation

When installing SubC, these files are copied to the installation directory:

- `include/*` → `$(SCCDIR)/include/`
- `lib/libscc.a` → `$(SCCDIR)/lib/`
- `lib/crt0.o` → `$(SCCDIR)/lib/`
- `lib/ncrt0.o` → `$(SCCDIR)/lib/`

## Header Files

The `include/` directory contains SubC's implementation of standard C headers:

| Header | Description |
|--------|-------------|
| `stdio.h` | Standard I/O (printf, scanf, fopen, etc.) |
| `stdlib.h` | General utilities (malloc, atoi, exit, etc.) |
| `string.h` | String handling (strcpy, strlen, memcpy, etc.) |
| `ctype.h` | Character classification (isalpha, isdigit, etc.) |
| `time.h` | Date and time functions |
| `limits.h` | Implementation limits |
| `errno.h` | Error codes |
| `signal.h` | Signal handling |
| `setjmp.h` | Non-local jumps |
| `fcntl.h` | File control options |
| `unistd.h` | POSIX API (Unix only) |
| `varargs.h` | Variable arguments (pre-C89 style) |
| `windows.h` | Windows API (Windows only) |

## Library Functions

The `lib/` directory contains the source code for the C runtime library functions. These are compiled into `libscc.a` which is linked with user programs.

Key components:
- **Memory management**: `malloc.c`, `free.c`, `calloc.c`, `realloc.c`
- **I/O**: `fopen.c`, `fclose.c`, `fread.c`, `fwrite.c`, `printf.c`, `scanf.c`
- **String handling**: `strcpy.c`, `strlen.c`, `strcmp.c`, `memcpy.c`
- **Formatting**: `vformat.c`, `vscan.c`
- **Utilities**: `atoi.c`, `qsort.c`, `bsearch.c`, `rand.c`

## Notes

- This is a minimal C runtime, not a full libc implementation
- Some functions may have limitations compared to standard libc
- The runtime is designed to be self-contained and portable
