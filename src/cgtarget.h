/*
 * SubC Compiler - Cross-Compiler Target Architecture Framework
 * 
 * This file defines the target architecture abstraction layer that enables
 * SubC to function as a cross-compiler. Instead of compile-time target
 * selection via symlinks, targets are now selected at runtime through
 * a vtable-based dispatch mechanism.
 *
 * Architecture:
 *   The target system is split into three composable parts:
 *
 *   1. struct cg_vtable - Code generation function pointers
 *      Shared by all targets of the same CPU architecture.
 *      Example: All i386 targets (Linux, FreeBSD, Windows) share cg_386_vtable.
 *
 *   2. struct cg_arch - Architecture properties
 *      CPU-specific properties like word size, endianness, registers.
 *      Example: All i386 targets share cg_386_arch.
 *
 *   3. struct cg_os_config - Operating system configuration
 *      OS-specific properties like assembler commands, object format.
 *      Example: All Linux targets (386, x86-64) share cg_os_linux.
 *
 *   4. struct cg_target - Final target descriptor
 *      Combines the above three via pointers, plus target-specific overrides.
 *
 * Adding a new target:
 *   For a new OS on existing architecture:
 *     1. Create OS config in cg_os_configs.c (or reuse existing)
 *     2. Create target file combining arch + os + vtable
 *
 *   For a new architecture:
 *     1. Create cg_ARCH.c with code generation functions
 *     2. Create cg_ARCH_arch and cg_ARCH_vtable
 *     3. Create target files for each supported OS
 *
 * Copyright (c) 2011-2025 - Public Domain (CC0)
 */

#ifndef CGTARGET_H
#define CGTARGET_H

#include <stddef.h>

/*
 * ============================================================================
 * SECTION: Enumerations for Target Properties
 * ============================================================================
 */

/* Endianness - Byte order for multi-byte values */
enum cg_endian {
    ENDIAN_LITTLE = 0,
    ENDIAN_BIG    = 1
};

/* Stack Growth Direction */
enum cg_stack_dir {
    STACK_DOWN = 0,
    STACK_UP   = 1
};

/* Assembler Syntax Style */
enum cg_asm_syntax {
    ASM_GAS = 0,
    ASM_INTEL,
    ASM_TASM,
    ASM_HLASM,
    ASM_ARM
};

/* Calling Convention */
enum cg_call_conv {
    CC_CDECL = 0,
    CC_STDCALL,
    CC_FASTCALL,
    CC_SYSV_AMD64,
    CC_WIN64,
    CC_AAPCS,
    CC_AAPCS64,
    CC_HLASM
};

/*
 * Floating Point Format
 *
 * Different architectures use different floating-point representations:
 * - IEEE 754: Standard for most modern CPUs (x86, ARM, RISC-V, etc.)
 * - IBM Hexadecimal: Used by IBM mainframes (System/360, z/Architecture)
 * - VAX: Used by older DEC VAX systems
 * - None: No floating-point support (8086 without FPU, some embedded)
 */
enum cg_float_format {
    FLOAT_NONE = 0,       /* No floating-point support */
    FLOAT_IEEE754,        /* IEEE 754 (most modern CPUs) */
    FLOAT_IBM_HEX,        /* IBM Hexadecimal Floating Point (mainframes) */
    FLOAT_IBM_DFP,        /* IBM Decimal Floating Point (z9+) */
    FLOAT_VAX             /* VAX floating-point format */
};

/*
 * Floating Point Unit Type
 *
 * Specifies how floating-point operations are implemented:
 * - FPU_NONE: No FP support (error on FP operations)
 * - FPU_X87: x87 FPU coprocessor (8086/80287, i386/80387, etc.)
 * - FPU_SSE: SSE/SSE2 instructions (modern x86/x86-64)
 * - FPU_VFP: ARM VFP/NEON
 * - FPU_EMULATED: Software emulation via runtime library calls
 */
enum cg_fpu_type {
    FPU_NONE = 0,         /* No FP support */
    FPU_X87,              /* x87 coprocessor (FILD, FADD, FSTP, etc.) */
    FPU_SSE,              /* SSE/SSE2 (ADDSD, MULSD, etc.) */
    FPU_VFP,              /* ARM VFP/NEON */
    FPU_EMULATED          /* Software emulation via function calls */
};

/* Target Operating System */
enum cg_os_type {
    OS_NONE = 0,
    OS_LINUX,
    OS_FREEBSD,
    OS_NETBSD,
    OS_OPENBSD,
    OS_DARWIN,
    OS_WINDOWS,
    OS_DOS,
    OS_MVS
};

/* Object File Format */
enum cg_obj_format {
    OBJ_NONE = 0,
    OBJ_ELF,
    OBJ_MACHO,
    OBJ_PE,
    OBJ_COFF,
    OBJ_OMF,
    OBJ_GOFF
};

/*
 * ============================================================================
 * SECTION: Code Generation Vtable
 * ============================================================================
 *
 * This structure contains all code generation function pointers.
 * It is shared by all targets of the same CPU architecture.
 * The interface matches the existing cgen.h declarations.
 */
struct cg_vtable {
    /* Section Control */
    void (*cgdata)(void);
    void (*cgtext)(void);
    void (*cgprelude)(void);
    void (*cgpostlude)(void);
    void (*cgpublic)(char *s);
    
    /* Synthesizer Support */
    void (*cgsynth)(char *op);
    int  (*cgload2)(void);
    
    /* Literal and Clear */
    void (*cglit)(int v);
    void (*cgclear)(void);
    void (*cgclear2)(void);
    
    /* Load Operations - Global */
    void (*cgldgb)(char *s);
    void (*cgldgw)(char *s);
    
    /* Load Operations - Local */
    void (*cgldlb)(int n);
    void (*cgldlw)(int n);
    
    /* Load Operations - Static */
    void (*cgldsb)(int n);
    void (*cgldsw)(int n);
    
    /* Load Address Operations */
    void (*cgldla)(int n);
    void (*cgldsa)(int n);
    void (*cgldga)(char *s);
    
    /* Indirect Load Operations */
    void (*cgindb)(void);
    void (*cgindw)(void);
    
    /* Load Label */
    void (*cgldlab)(int id);
    
    /* Stack Operations */
    void (*cgpush)(void);
    void (*cgpushlit)(int n);
    void (*cgpop2)(void);
    void (*cgswap)(void);
    void (*cgpopptr)(void);
    
    /* Arithmetic Operations */
    void (*cgand)(void);
    void (*cgior)(void);
    void (*cgxor)(void);
    void (*cgadd)(void);
    void (*cgsub)(void);
    void (*cgmul)(void);
    void (*cgdiv)(void);
    void (*cgmod)(void);
    void (*cgshl)(void);
    void (*cgshr)(void);
    
    /* Unary Operations */
    void (*cgneg)(void);
    void (*cgnot)(void);
    void (*cglognot)(void);
    
    /* Scaling Operations */
    void (*cgscale)(void);
    void (*cgscale2)(void);
    void (*cgunscale)(void);
    void (*cgscaleby)(int v);
    void (*cgscale2by)(int v);
    void (*cgunscaleby)(int v);
    
    /* Comparison Operations */
    void (*cgeq)(void);
    void (*cgne)(void);
    void (*cglt)(void);
    void (*cggt)(void);
    void (*cgle)(void);
    void (*cgge)(void);
    void (*cgult)(void);
    void (*cgugt)(void);
    void (*cgule)(void);
    void (*cguge)(void);
    
    /* Conditional Branch Operations */
    void (*cgbreq)(int n);
    void (*cgbrne)(int n);
    void (*cgbrlt)(int n);
    void (*cgbrgt)(int n);
    void (*cgbrle)(int n);
    void (*cgbrge)(int n);
    void (*cgbrult)(int n);
    void (*cgbrugt)(int n);
    void (*cgbrule)(int n);
    void (*cgbruge)(int n);
    
    /* Branch Operations */
    void (*cgbrtrue)(int n);
    void (*cgbrfalse)(int n);
    void (*cgjump)(int n);
    void (*cgldswtch)(int n);
    void (*cgcalswtch)(void);
    void (*cgcase)(int v, int l);
    
    /* Boolean Operations */
    void (*cgbool)(void);
    
    /* Increment/Decrement via Pointer */
    void (*cgldinc)(void);
    void (*cginc1pi)(int v);
    void (*cgdec1pi)(int v);
    void (*cginc2pi)(int v);
    void (*cgdec2pi)(int v);
    
    /* Increment/Decrement Pointer - Local/Static/Global */
    void (*cgincpl)(int a, int v);
    void (*cgdecpl)(int a, int v);
    void (*cgincps)(int a, int v);
    void (*cgdecps)(int a, int v);
    void (*cgincpg)(char *s, int v);
    void (*cgdecpg)(char *s, int v);
    
    /* Increment/Decrement Indirect Word */
    void (*cginc1iw)(void);
    void (*cgdec1iw)(void);
    void (*cginc2iw)(void);
    void (*cgdec2iw)(void);
    
    /* Increment/Decrement Local Word */
    void (*cginclw)(int a);
    void (*cgdeclw)(int a);
    
    /* Increment/Decrement Static Word */
    void (*cgincsw)(int a);
    void (*cgdecsw)(int a);
    
    /* Increment/Decrement Global Word */
    void (*cgincgw)(char *s);
    void (*cgdecgw)(char *s);
    
    /* Increment/Decrement Indirect Byte */
    void (*cginc1ib)(void);
    void (*cgdec1ib)(void);
    void (*cginc2ib)(void);
    void (*cgdec2ib)(void);
    
    /* Increment/Decrement Local Byte */
    void (*cginclb)(int a);
    void (*cgdeclb)(int a);
    
    /* Increment/Decrement Static Byte */
    void (*cgincsb)(int a);
    void (*cgdecsb)(int a);
    
    /* Increment/Decrement Global Byte */
    void (*cgincgb)(char *s);
    void (*cgdecgb)(char *s);
    
    /* Store Operations */
    void (*cgstorib)(void);
    void (*cgstoriw)(void);
    void (*cgstorlb)(int n);
    void (*cgstorlw)(int n);
    void (*cgstorsb)(int n);
    void (*cgstorsw)(int n);
    void (*cgstorgb)(char *s);
    void (*cgstorgw)(char *s);
    
    /* Function Operations */
    void (*cginitlw)(int v, int a);
    void (*cgcall)(char *s);
    void (*cgcalr)(void);
    void (*cgstack)(int n);
    void (*cgentry)(void);
    void (*cgexit)(void);
    
    /*
     * ABI-Compliant Calling Convention Support
     * 
     * These functions implement proper argument passing according to the
     * target ABI. For register-based ABIs (x86-64, ARM), arguments are
     * passed in registers first, then on the stack.
     *
     * cgpusharg: Move accumulator to argument position (register or stack)
     *   - argnum: 0-based argument number (0 = first arg)
     *   - For x86-64: args 0-5 go to rdi,rsi,rdx,rcx,r8,r9; rest to stack
     *   - For ARM: args 0-3 go to r0-r3; rest to stack
     *   - For i386/8086: all args go to stack (same as cgpush)
     *
     * cgcallprep: Prepare for function call (called before pushing args)
     *   - nargs: total number of arguments
     *   - May need to align stack for some ABIs
     *
     * cgcallend: Clean up after function call
     *   - nargs: total number of arguments
     *   - Adjusts stack pointer for stack-passed arguments only
     *
     * cgfnentry: Function entry with parameter info (replaces cgentry)
     *   - nparams: number of declared parameters (negative if variadic)
     *   - For register ABIs: saves register args to stack for access
     */
    void (*cgpusharg)(int argnum);
    void (*cgcallprep)(int nargs);
    void (*cgcallend)(int nargs);
    void (*cgfnentry)(int nparams);
    
    /* Data Definition */
    void (*cgdefb)(int v);      /* Define byte (1 byte) */
    void (*cgdefh)(int v);      /* Define half-word (2 bytes) - for short */
    void (*cgdefw)(int v);      /* Define word (native int size) */
    void (*cgdefd)(int v);      /* Define double-word (4 bytes) - for long on 16-bit */
    void (*cgdefp)(int v);      /* Define pointer */
    void (*cgdefl)(int v);      /* Define label reference */
    void (*cgdefc)(int c);      /* Define character */
    void (*cgdefq)(int v);      /* Define quad-word (8 bytes) - for double/long long */
    void (*cggbss)(char *s, int z);
    void (*cglbss)(char *s, int z);
    
    /* Alignment */
    void (*cgalign)(void);
    
    /*
     * ========================================================================
     * Floating-Point Operations
     * ========================================================================
     *
     * These functions implement floating-point arithmetic. The implementation
     * depends on the target's fpu_type:
     *   - FPU_X87: Use x87 FPU instructions (fld, fadd, fstp, etc.)
     *   - FPU_SSE: Use SSE/SSE2 instructions (movsd, addsd, etc.)
     *   - FPU_VFP: Use ARM VFP instructions
     *   - FPU_EMULATED: Generate calls to runtime library functions
     *
     * For x87, the FPU uses a stack-based model:
     *   - cgfpush: Push value from memory/integer reg to FPU stack (ST0)
     *   - cgfpop: Pop ST0 to memory
     *   - Operations work on ST0 and ST1, result in ST0
     *
     * For SSE/emulated, values are in general-purpose registers or memory.
     *
     * Float vs Double:
     *   - Functions ending in 'f' operate on float (32-bit)
     *   - Functions ending in 'd' operate on double (64-bit)
     *   - Some architectures may use the same implementation for both
     */
    
    /* Load/Store floating-point values */
    void (*cgfloads)(int n);        /* Load float from stack offset to FP reg */
    void (*cgfloadd)(int n);        /* Load double from stack offset to FP reg */
    void (*cgfloadgs)(char *s);     /* Load float from global symbol */
    void (*cgfloadgd)(char *s);     /* Load double from global symbol */
    void (*cgfstores)(int n);       /* Store float to stack offset */
    void (*cgfstored)(int n);       /* Store double to stack offset */
    void (*cgfstoregs)(char *s);    /* Store float to global symbol */
    void (*cgfstoregsd)(char *s);   /* Store double to global symbol */
    
    /* Floating-point literals */
    void (*cgflits)(int lab);       /* Load float literal (label reference) */
    void (*cgflitd)(int lab);       /* Load double literal (label reference) */
    
    /* Floating-point arithmetic */
    void (*cgfadds)(void);          /* float addition: ST0 = ST1 + ST0 */
    void (*cgfaddd)(void);          /* double addition */
    void (*cgfsubs)(void);          /* float subtraction: ST0 = ST1 - ST0 */
    void (*cgfsubd)(void);          /* double subtraction */
    void (*cgfmuls)(void);          /* float multiplication */
    void (*cgfmuld)(void);          /* double multiplication */
    void (*cgfdivs)(void);          /* float division: ST0 = ST1 / ST0 */
    void (*cgfdivd)(void);          /* double division */
    void (*cgfnegs)(void);          /* float negation: ST0 = -ST0 */
    void (*cgfnegd)(void);          /* double negation */
    
    /* Floating-point comparisons (result in integer register) */
    void (*cgfcmps)(void);          /* Compare floats, set flags */
    void (*cgfcmpd)(void);          /* Compare doubles, set flags */
    void (*cgfeqs)(void);           /* float == : result 0 or 1 in acc */
    void (*cgfeqd)(void);           /* double == */
    void (*cgfnes)(void);           /* float != */
    void (*cgfned)(void);           /* double != */
    void (*cgflts)(void);           /* float < */
    void (*cgfltd)(void);           /* double < */
    void (*cgfgts)(void);           /* float > */
    void (*cgfgtd)(void);           /* double > */
    void (*cgfles)(void);           /* float <= */
    void (*cgfled)(void);           /* double <= */
    void (*cgfges)(void);           /* float >= */
    void (*cgfged)(void);           /* double >= */
    
    /* Type conversions */
    void (*cgitofs)(void);          /* int to float: convert acc to FP */
    void (*cgitofd)(void);          /* int to double */
    void (*cgftois)(void);          /* float to int: convert FP to acc */
    void (*cgftoid)(void);          /* double to int */
    void (*cgstod)(void);           /* float to double (promote) */
    void (*cgdtos)(void);           /* double to float (demote) */
    
    /* Floating-point stack operations (for x87) */
    void (*cgfpush)(void);          /* Push FP value to FP stack */
    void (*cgfpop)(void);           /* Pop FP stack */
    void (*cgfxch)(void);           /* Exchange ST0 and ST1 */
    
    /* Define floating-point constants in data section */
    void (*cgdeffloat)(int lab, unsigned int bits);   /* Define 32-bit float */
    void (*cgdefdouble)(int lab, unsigned int hi, unsigned int lo); /* Define 64-bit double */
};

/*
 * ============================================================================
 * SECTION: Architecture Properties
 * ============================================================================
 *
 * CPU-specific properties shared by all targets of the same architecture.
 */
struct cg_arch {
    /* Architecture name */
    char *name;
    
    /* Data type sizes (in bytes) */
    int  bits;              /* Architecture bits (16, 32, 64) */
    int  char_size;         /* sizeof(char) - always 1 */
    int  short_size;        /* sizeof(short) */
    int  int_size;          /* sizeof(int) */
    int  long_size;         /* sizeof(long) */
    int  ptr_size;          /* sizeof(void*) */
    int  float_size;        /* sizeof(float) */
    int  double_size;       /* sizeof(double) */
    int  bpw;               /* Bytes per word */
    
    /* Architecture characteristics */
    int  endian;            /* enum cg_endian */
    int  stack_dir;         /* enum cg_stack_dir */
    int  asm_syntax;        /* enum cg_asm_syntax */
    int  call_conv;         /* enum cg_call_conv */
    int  float_format;      /* enum cg_float_format */
    int  fpu_type;          /* enum cg_fpu_type */
    
    /* Alignment requirements */
    int  align_stack;
    int  align_data;
    int  align_func;
    
    /* Hardware capabilities */
    int  has_mul;
    int  has_div;
    int  has_mod;
    int  has_byte_ops;
    int  needs_alignment;
    
    /*
     * Stack Frame Layout Configuration
     * These fields allow different architectures to specify how parameters
     * and local variables are laid out relative to the frame pointer.
     *
     * For STACK_DOWN architectures (x86, ARM):
     *   - Parameters are at positive offsets from FP (above the frame)
     *   - Locals are at negative offsets from FP (below the frame)
     *   - param_offset_dir = 1 (positive direction)
     *   - local_offset_dir = -1 (negative direction)
     *
     * For STACK_UP architectures (S/370, PA-RISC):
     *   - Parameters may be at negative offsets or passed via registers
     *   - Locals are at positive offsets from FP
     *   - param_offset_dir = -1 or via register area
     *   - local_offset_dir = 1 (positive direction)
     */
    int  param_offset_base;   /* Initial offset for first parameter from FP */
    int  param_offset_dir;    /* Direction for subsequent params: 1 or -1 */
    int  local_offset_base;   /* Initial offset for local variables from FP */
    int  local_offset_dir;    /* Direction for subsequent locals: 1 or -1 */
    
    /*
     * Register-based Calling Convention Support
     * 
     * For ABIs that pass arguments in registers (x86-64, ARM, RISC-V):
     *   - num_arg_regs: Number of registers used for integer arguments
     *   - For x86-64 System V: 6 (rdi, rsi, rdx, rcx, r8, r9)
     *   - For ARM AAPCS: 4 (r0, r1, r2, r3)
     *   - For i386 cdecl: 0 (all on stack)
     *
     * When num_arg_regs > 0, the code generator must implement:
     *   - cgpusharg(int argnum): Push/move argument to correct location
     *   - cgargstack(int nargs): Adjust stack after call (only for stack args)
     */
    int  num_arg_regs;        /* Number of registers for integer arguments */
    
    /*
     * Symbol Transformation Callback
     * 
     * Custom function to transform C symbol names to assembler symbol names.
     * If NULL, the default transformation is used (PREFIX + name).
     * 
     * Use cases:
     *   - HLASM: Convert to uppercase, limit to 8 chars, hash long names
     *   - Darwin: Add underscore prefix
     *   - Custom naming conventions
     * 
     * The function receives the original C symbol name and returns a pointer
     * to a static buffer containing the transformed name.
     */
    char *(*symbol_transform)(char *name);
};

/*
 * ============================================================================
 * SECTION: Operating System Configuration
 * ============================================================================
 *
 * OS-specific properties shared by all targets on the same OS.
 */
struct cg_os_config {
    int   os_type;          /* enum cg_os_type */
    int   obj_format;       /* enum cg_obj_format */
    int   underscore_sym;   /* Prefix symbols with underscore? */
    
    char *os_name;
    char *asm_cmd;          /* Assembler command template */
    char *ld_cmd;           /* Linker command template */
    char *sys_libc;         /* System libc to link */
    char *aout_name;        /* Default output name */
};

/*
 * ============================================================================
 * SECTION: Target Descriptor
 * ============================================================================
 *
 * The final target descriptor combines architecture, OS, and vtable.
 * Each target is a small struct with pointers to shared components.
 */
struct cg_target {
    /* Identification */
    char *name;             /* Short name: "linux-386", "dos-8086" */
    char *description;      /* Human readable description */
    
    /* Composed components (pointers to shared data) */
    struct cg_arch      *arch;
    struct cg_os_config *os;
    struct cg_vtable    *vtable;
    
    /* Target-specific overrides (NULL means use os->field) */
    char *asm_cmd_override;
    char *ld_cmd_override;
};

/*
 * ============================================================================
 * SECTION: Global Target Pointer and Accessor Macros
 * ============================================================================
 */

/* Current active target */
extern struct cg_target *CG;

/*
 * Architecture property accessor macros (prefixed with CG_)
 * These provide direct access to target properties without
 * conflicting with the compatibility macros in cgen_compat.h
 */
#define CG_NAME         (CG->name)
#define CG_BITS         (CG->arch->bits)
#define CG_BPW          (CG->arch->bpw)

/* Type size accessor macros - use these instead of hardcoded values */
#define CG_CHARSIZE     (CG->arch->char_size)
#define CG_SHORTSIZE    (CG->arch->short_size)
#define CG_INTSIZE      (CG->arch->int_size)
#define CG_LONGSIZE     (CG->arch->long_size)
#define CG_PTRSIZE      (CG->arch->ptr_size)
#define CG_FLOATSIZE    (CG->arch->float_size)
#define CG_DOUBLESIZE   (CG->arch->double_size)

#define CG_ENDIAN       (CG->arch->endian)
#define CG_STACK_DIR    (CG->arch->stack_dir)
#define CG_ASM_SYNTAX   (CG->arch->asm_syntax)
#define CG_CALL_CONV    (CG->arch->call_conv)
#define CG_FPU_TYPE     (CG->arch->fpu_type)

/* Stack frame layout accessor macros */
#define CG_PARAM_OFFSET_BASE  (CG->arch->param_offset_base)
#define CG_PARAM_OFFSET_DIR   (CG->arch->param_offset_dir)
#define CG_LOCAL_OFFSET_BASE  (CG->arch->local_offset_base)
#define CG_LOCAL_OFFSET_DIR   (CG->arch->local_offset_dir)

/* Symbol transformation accessor macro */
#define CG_SYMBOL_TRANSFORM   (CG->arch->symbol_transform)

/* OS property accessor macros */
#define CG_OS_TYPE      (CG->os->os_type)
#define CG_OBJ_FORMAT   (CG->os->obj_format)
#define CG_OS_NAME      (CG->os->os_name)

/*
 * NOTE: Vtable dispatch macros (cgdata(), cgtext(), etc.) are now
 * defined in cgen_compat.h to avoid duplication and provide a
 * single source of truth for the compatibility layer.
 */

/*
 * ============================================================================
 * SECTION: Target Management Functions
 * ============================================================================
 */

/* Initialize target system */
void cg_init_targets(void);

/* Set current target by name, returns 0 on success, -1 on failure */
int cg_set_target(char *name);

/* Get current target */
struct cg_target *cg_get_target(void);

/* List all available targets to stdout */
void cg_list_targets(void);

/* Find target by name, returns NULL if not found */
struct cg_target *cg_find_target(char *name);

/* Get assembler command (checks override first) */
char *cg_get_asm_cmd(void);

/* Get linker command (checks override first) */
char *cg_get_ld_cmd(void);

#endif /* CGTARGET_H */
