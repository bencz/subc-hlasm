# SubC Compiler - ARM64/AArch64 Target

## Overview

This directory contains the ARM64 (AArch64) code generator for the SubC compiler.
It generates GNU Assembler (GAS) syntax assembly code for 64-bit ARM processors.

## Supported Targets

| Target | Description | OS | Object Format |
|--------|-------------|-----|---------------|
| `arm64` | Default ARM64 target | Linux | ELF |
| `arm64-linux` | ARM64 Linux | Linux | ELF |
| `aarch64-linux` | Alias for arm64-linux | Linux | ELF |
| `arm64-darwin` | ARM64 macOS (Apple Silicon) | Darwin | Mach-O |
| `aarch64-darwin` | Alias for arm64-darwin | Darwin | Mach-O |
| `arm64-macos` | Alias for arm64-darwin | Darwin | Mach-O |

## Architecture Characteristics

- **Bit width**: 64-bit
- **Endianness**: Little-endian (default)
- **Stack direction**: Grows downward (toward lower addresses)
- **Stack alignment**: 16-byte alignment required
- **Floating point**: IEEE 754 (via NEON/VFP)
- **Calling convention**: AAPCS64 (ARM Architecture Procedure Call Standard)

## Register Usage

| Register | Purpose |
|----------|---------|
| `x0` | Primary accumulator, return value, first argument |
| `x1` | Secondary accumulator, second argument |
| `x2` | Tertiary register, pointer for stores |
| `x3` | Scratch register |
| `x9-x15` | Additional scratch (caller-saved) |
| `x19-x28` | Callee-saved registers |
| `x29` | Frame pointer (FP) |
| `x30` | Link register (LR) |
| `sp` | Stack pointer |

Note: `w0-w30` are the 32-bit versions of the same registers.

## Files

| File | Description |
|------|-------------|
| `cg_arm64.c` | Code generation functions (~1000 lines) |
| `cg_arm64_funcs.h` | Function prototypes for all ~90 codegen functions |
| `cg_arm64_arch.h` | Architecture and vtable declarations |
| `cg_arm64_arch.c` | Architecture properties and vtable definitions |
| `cg_arm64_linux.c` | Linux target descriptor |
| `cg_arm64_darwin.c` | Darwin/macOS target descriptor |

## Usage

```bash
# Compile for ARM64 Linux
scc -T arm64-linux -S program.c

# Compile for ARM64 macOS (Apple Silicon)
scc -T arm64-darwin -S program.c

# List all available targets
scc -L
```

## Linux vs Darwin Differences

### Symbol Naming
- **Linux**: Symbols use their C names directly (`main`, `printf`)
- **Darwin**: Symbols require underscore prefix (`_main`, `_printf`)

### PC-Relative Addressing
- **Linux**: Uses `:pg_hi21:` and `:lo12:` relocations
  ```asm
  adrp    x0, :pg_hi21:symbol
  add     x0, x0, :lo12:symbol
  ```
- **Darwin**: Uses `@PAGE` and `@PAGEOFF`
  ```asm
  adrp    x0, _symbol@PAGE
  add     x0, x0, _symbol@PAGEOFF
  ```

### Section Directives
- **Linux**: Standard ELF sections
  ```asm
  .arch armv8-a
  .text
  .data
  ```
- **Darwin**: Mach-O specific sections
  ```asm
  .section __TEXT,__text,regular,pure_instructions
  .build_version macos, 11, 0
  ```

### Alignment
- **Linux**: `.align 3` (2^3 = 8 bytes)
- **Darwin**: `.p2align 3` (power-of-2 alignment)

## Example Output

### Linux
```asm
        .arch armv8-a
        .text
        .globl  Cmain
        .align 3
Cmain:
        stp     x29, x30, [sp, #-16]!
        mov     x29, sp
        mov     x0, #42
        b       L1
L1:
        ldp     x29, x30, [sp], #16
        ret
```

### Darwin (macOS)
```asm
        .section __TEXT,__text,regular,pure_instructions
        .build_version macos, 11, 0
        .text
        .globl  _Cmain
        .p2align 3
_Cmain:
        stp     x29, x30, [sp, #-16]!
        mov     x29, sp
        mov     x0, #42
        b       L1
L1:
        ldp     x29, x30, [sp], #16
        ret
```

## Implementation Notes

### Stack Frame
The ARM64 backend uses a simple stack frame:
- `stp x29, x30, [sp, #-16]!` - Save FP and LR, allocate 16 bytes
- `mov x29, sp` - Set up frame pointer
- Local variables are accessed via `[x29, #offset]`
- `ldp x29, x30, [sp], #16` - Restore FP and LR, deallocate

### Literal Loading
- Small values (0-65535): `mov xN, #imm`
- Negative values: `mov xN, #imm` (assembler handles encoding)
- Large values: `mov` + `movk` sequence for 32-bit values

### Comparisons
ARM64 uses conditional select instructions:
```asm
cmp     x1, x0
csinc   x0, x0, xzr, ne    ; x0 = (x1 == x0) ? 1 : 0
```

### Division and Modulo
ARM64 has hardware division:
- `sdiv x0, x1, x0` - Signed division
- `msub x0, x3, x0, x1` - Modulo via multiply-subtract

## Testing

```bash
# Generate assembly and inspect
echo 'int main() { return 42; }' > test.c
./scc0 -T arm64-linux -S test.c
cat test.s

# On an ARM64 Linux system, you can assemble and run:
as -o test.o test.s
ld -o test test.o
./test
echo $?  # Should print 42
```

## Copyright

Public Domain (CC0) - 2025
