# SubC Compiler - Source Code Organization

## Directory Structure

```
src/
├── main.c                      # Entry point, CLI, driver
├── defs.h                      # Global definitions (constants, includes)
├── data.h                      # Global variables (legacy)
├── decl.h                      # Function declarations (legacy)
│
├── frontend/                   # Lexical and syntactic analysis
│   ├── lexer/
│   │   ├── scan.c              # Scanner (lexical analyzer)
│   │   ├── scan.h              # Scanner declarations
│   │   └── tokens.h            # Token definitions
│   │
│   ├── preprocessor/
│   │   ├── prep.c              # Preprocessor (#define, #include, etc.)
│   │   └── prep.h              # Preprocessor declarations
│   │
│   └── parser/
│       ├── decl.c              # Declaration parser
│       ├── decl.h              # Declaration parser declarations
│       ├── stmt.c              # Statement parser
│       ├── stmt.h              # Statement parser declarations
│       ├── expr.c              # Expression parser
│       ├── expr.h              # Expression parser declarations
│       └── prec.h              # Operator precedence table
│
├── ast/                        # Abstract Syntax Tree
│   ├── tree.c                  # AST node construction and emission
│   ├── tree.h                  # AST declarations
│   └── ops.h                   # AST operator definitions
│
├── optimizer/                  # Code optimization
│   ├── opt.c                   # Constant folding, reordering
│   └── opt.h                   # Optimizer declarations
│
├── codegen/                    # Code generation interface
│   ├── gen.c                   # High-level code generation
│   └── gen.h                   # Code generator declarations
│
├── symbols/                    # Symbol table management
│   ├── sym.c                   # Symbol table operations
│   ├── sym.h                   # Symbol table declarations
│   └── types.h                 # Type and storage class definitions
│
├── common/                     # Shared utilities
│   ├── error.c                 # Error handling
│   ├── error.h                 # Error declarations
│   ├── misc.c                  # Miscellaneous utilities
│   ├── misc.h                  # Misc declarations
│   └── data.h                  # Global variables
│
├── targets/                    # Cross-compiler target architecture
│   ├── arch/                   # Architecture-specific code generators
│   │   ├── cgtarget.h/c        # Target framework (canonical location)
│   │   ├── cgen_compat.h       # Backward compatibility macros (canonical)
│   │   ├── cg_os_configs.h/c   # OS configurations
│   │   ├── x86/
│   │   │   ├── i386/           # Intel 386 targets
│   │   │   ├── x86_64/         # AMD64/x86-64 targets
│   │   │   └── i8086/          # 8086 (DOS) targets
│   │   ├── arm/
│   │   │   ├── armv6/          # ARMv6 targets (32-bit ARM)
│   │   │   └── arm64/          # ARM64/AArch64 targets (64-bit ARM)
│   │   │       ├── cg_arm64.c          # ARM64 code generation
│   │   │       ├── cg_arm64_arch.c/h   # ARM64 architecture (64-bit)
│   │   │       ├── cg_arm64_linux.c    # ARM64 Linux target
│   │   │       └── cg_arm64_darwin.c   # ARM64 Darwin/macOS target
│   │   └── s370/               # IBM System/370 targets
│   │       ├── cg_s370.c           # S/370 code generation (HLASM)
│   │       ├── cg_s370_arch.c/h    # S/370 architecture (24-bit)
│   │       ├── cg_s370_mvs.c       # S/370 MVS target
│   │       ├── cg_s370xa_arch.c    # S/370-XA architecture (31-bit)
│   │       └── cg_s370xa_mvs.c     # S/370-XA MVS target
│   ├── os/                     # OS-specific files (crt0, sys headers)
│   ├── include/                # Target-specific headers
│   └── lib/                    # Target-specific library files
│
├── include/                    # Standard C headers
└── lib/                        # Runtime library sources
```

## Compilation Flow

```
Source Code (.c)
      │
      ▼
┌─────────────────┐
│  Preprocessor   │  frontend/preprocessor/prep.c
│  (#include,     │
│   #define, etc) │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│     Lexer       │  frontend/lexer/scan.c
│  (Tokenization) │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│     Parser      │  frontend/parser/{decl,stmt,expr}.c
│  (Syntax        │
│   Analysis)     │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│      AST        │  ast/tree.c
│  (Abstract      │
│   Syntax Tree)  │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│   Optimizer     │  optimizer/opt.c
│  (Constant      │
│   Folding)      │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  Code Generator │  codegen/gen.c
│  (Interface)    │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│ Target Backend  │  targets/arch/{arch}/{target}.c
│ (x86, ARM, etc) │
└────────┬────────┘
         │
         ▼
   Assembly (.s)
```

## Building

The Makefile supports both the legacy flat structure and the new modular structure:

```bash
# Build using legacy flat structure (default)
make scc0

# The modular sources are defined in SRC_MODULAR for future use
```

## Adding New Targets

See `targets/arch/README` for detailed instructions on adding new CPU architectures or OS targets.

## Command Line Options

Key options handled in `main.c`:

| Option | Description |
|--------|-------------|
| `-c` | Compile only, do not link |
| `-o file` | Write output to FILE |
| `-S` | Compile to assembly language |
| `-T target` | Select target architecture (default: 386) |
| `-L` | List available targets |
| `-R` | Use system runtime (libc) instead of SubC runtime |
| `-I dir` | Add DIR to include search path |
| `-D m=v` | Define macro M with optional value V |
| `-N` | Do not use stdio |
| `-v` | Verbose mode |

## Include Search Path

The preprocessor (`frontend/preprocessor/prep.c`) searches for include files in this order:

**For `#include "file"`:**
1. Current directory
2. Directories specified with `-I` (in order)
3. `SCCDIR/include`

**For `#include <file>`:**
1. Directories specified with `-I` (in order)
2. `SCCDIR/include`

## System Runtime Mode (-R)

When `-R` is active, the compiler generates code compatible with the system's C library:

- Symbol names are not prefixed with 'C'
- Uses platform ABI for function calls (e.g., AAPCS64 for ARM64)
- Links with system libc instead of SubC runtime

For ARM64, full AAPCS64 support is implemented:
- Caller side: arguments in x0-x7, overflow on stack
- Callee side: saves register arguments to stack for SubC access
- No limit on number of function parameters

## C89 Compliance

All code is C89 compliant. No C99/C11 features are used.
