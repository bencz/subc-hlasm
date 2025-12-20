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

/*
 * Endianness - Byte order for multi-byte values
 */
enum cg_endian {
    ENDIAN_LITTLE = 0,
    ENDIAN_BIG    = 1
};

/*
 * Stack Growth Direction
 */
enum cg_stack_dir {
    STACK_DOWN = 0,
    STACK_UP   = 1
};

/*
 * Assembler Syntax Style
 */
enum cg_asm_syntax {
    ASM_GAS = 0,
    ASM_INTEL,
    ASM_TASM,
    ASM_HLASM,
    ASM_ARM
};

/*
 * Calling Convention
 */
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
 * Target Operating System
 */
enum cg_os {
    OS_NONE = 0,
    OS_LINUX,
    OS_FREEBSD,
    OS_NETBSD,
    OS_OPENBSD,
    OS_DARWIN,
    OS_WINDOWS,
    OS_DOS,
    OS_MVS,
    OS_ZOS
};

/*
 * Object File Format
 */
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
 */
struct cg_vtable {
    /* Section Control */
    void (*cgdata)(void);
    void (*cgtext)(void);
    void (*cgprelude)(void);
    void (*cgpostlude)(void);
    void (*cgpublic)(char *s);
    void (*cgname)(char *s);
    void (*cgfuncname)(char *s);
    void (*cgalign)(void);
    
    /* Literal and Clear */
    void (*cglit)(int v);
    void (*cgclear)(void);
    void (*cgclear2)(void);
    
    /* Load Operations */
    void (*cgldgb)(char *s);
    void (*cgldgw)(char *s);
    void (*cgldlb)(int n);
    void (*cgldlw)(int n);
    void (*cgldsb)(int n);
    void (*cgldsw)(int n);
    void (*cgldla)(int n);
    void (*cgldsa)(int n);
    void (*cgldga)(char *s);
    void (*cgindb)(void);
    void (*cgindw)(void);
    void (*cgldlab)(int id);
    
    /* Stack Operations */
    void (*cgpush)(void);
    void (*cgpushlit)(int n);
    void (*cgpop2)(void);
    void (*cgswap)(void);
    void (*cgpopptr)(void);
    
    /* Arithmetic Operations */
    void (*cgadd)(void);
    void (*cgsub)(void);
    void (*cgmul)(void);
    void (*cgdiv)(void);
    void (*cgmod)(void);
    void (*cgneg)(void);
    
    /* Bitwise Operations */
    void (*cgand)(void);
    void (*cgior)(void);
    void (*cgxor)(void);
    void (*cgnot)(void);
    void (*cgshl)(void);
    void (*cgshr)(void);
    
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
    
    /* Boolean and Logic Operations */
    void (*cglognot)(void);
    void (*cgbool)(void);
    
    /* Pointer Scaling Operations */
    void (*cgscale)(void);
    void (*cgscale2)(void);
    void (*cgunscale)(void);
    void (*cgscaleby)(int v);
    void (*cgscale2by)(int v);
    void (*cgunscaleby)(int v);
    
    /* Increment/Decrement via Pointer */
    void (*cgldinc)(void);
    void (*cginc1pi)(int v);
    void (*cgdec1pi)(int v);
    void (*cginc2pi)(int v);
    void (*cgdec2pi)(int v);
    
    /* Increment/Decrement Local Variables */
    void (*cgincpl)(int a, int v);
    void (*cgdecpl)(int a, int v);
    void (*cginclw)(int a);
    void (*cgdeclw)(int a);
    void (*cginclb)(int a);
    void (*cgdeclb)(int a);
    
    /* Increment/Decrement Static Variables */
    void (*cgincps)(int a, int v);
    void (*cgdecps)(int a, int v);
    void (*cgincsw)(int a);
    void (*cgdecsw)(int a);
    void (*cgincsb)(int a);
    void (*cgdecsb)(int a);
    
    /* Increment/Decrement Global Variables */
    void (*cgincpg)(char *s, int v);
    void (*cgdecpg)(char *s, int v);
    void (*cgincgw)(char *s);
    void (*cgdecgw)(char *s);
    void (*cgincgb)(char *s);
    void (*cgdecgb)(char *s);
    
    /* Increment/Decrement via Indirect */
    void (*cginc1iw)(void);
    void (*cgdec1iw)(void);
    void (*cginc2iw)(void);
    void (*cgdec2iw)(void);
    void (*cginc1ib)(void);
    void (*cgdec1ib)(void);
    void (*cginc2ib)(void);
    void (*cgdec2ib)(void);
    
    /* Unconditional Branch */
    void (*cgbrtrue)(int n);
    void (*cgbrfalse)(int n);
    void (*cgjump)(int n);
    
    /* Switch Statement Support */
    void (*cgldswtch)(int n);
    void (*cgcalswtch)(void);
    void (*cgcase)(int v, int l);
    
    /* Store Operations */
    void (*cgstorib)(void);
    void (*cgstoriw)(void);
    void (*cgstorlb)(int n);
    void (*cgstorlw)(int n);
    void (*cgstorsb)(int n);
    void (*cgstorsw)(int n);
    void (*cgstorgb)(char *s);
    void (*cgstorgw)(char *s);
    
    /* Function Call Operations */
    void (*cginitlw)(int v, int a);
    void (*cgcall)(char *s);
    void (*cgcalr)(void);
    void (*cgstack)(int n);
    void (*cgentry)(int lsize);
    void (*cgexit)(void);
    
    /* Data Definition */
    void (*cgdefb)(int v);
    void (*cgdefw)(int v);
    void (*cgdefp)(int v);
    void (*cgdefl)(int v);
    void (*cgdefc)(int c);
    void (*cggbss)(char *s, int z);
    void (*cglbss)(char *s, int z);
    
    /* Synthesizer Support */
    int  (*cgload2)(void);
};

/*
 * ============================================================================
 * SECTION: Architecture Properties
 * ============================================================================
 *
 * CPU-specific properties shared by all targets of the same architecture.
 */
struct cg_arch {
    /* Data type sizes (in bytes) */
    int  bits;
    int  char_size;
    int  short_size;
    int  int_size;
    int  long_size;
    int  ptr_size;
    int  bpw;
    
    /* Architecture characteristics */
    int  endian;
    int  stack_dir;
    int  asm_syntax;
    int  call_conv;
    int  float_format;
    
    /* Alignment requirements */
    int  align_stack;
    int  align_data;
    int  align_func;
    
    /* Hardware capabilities */
    int  has_mul;
    int  has_div;
    int  has_mod;
    int  has_barrel_shift;
    int  has_byte_ops;
    int  needs_alignment;
    int  needs_pic;
    int  has_frame_ptr;
};

/*
 * ============================================================================
 * SECTION: Operating System Configuration
 * ============================================================================
 *
 * OS-specific properties shared by all targets on the same OS.
 */
struct cg_os_config {
    int   target_os;
    int   obj_format;
    int   underscore_sym;
    
    char *os_name;
    char *asm_cmd;
    char *ld_cmd;
    char *sys_libc;
    char *aout_name;
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
    char *name;
    char *description;
    
    /* Composed components (pointers to shared data) */
    struct cg_arch      *arch;
    struct cg_os_config *os;
    struct cg_vtable    *vtable;
    
    /* Target-specific overrides (NULL means use os->field) */
    char *asm_cmd_override;
    char *ld_cmd_override;
    
    /* Extension data for target-specific needs */
    void *ext_data;
};

/*
 * ============================================================================
 * SECTION: Global Target Pointer and Accessor Macros
 * ============================================================================
 */

extern struct cg_target *CG;

/* Architecture properties */
#define CG_NAME         (CG->name)
#define CG_BITS         (CG->arch->bits)
#define CG_BPW          (CG->arch->bpw)
#define CG_ENDIAN       (CG->arch->endian)
#define CG_STACK_DIR    (CG->arch->stack_dir)
#define CG_ASM_SYNTAX   (CG->arch->asm_syntax)

/* OS properties */
#define CG_OS           (CG->os->target_os)
#define CG_OBJ_FORMAT   (CG->os->obj_format)

/*
 * ============================================================================
 * SECTION: Target Management Functions
 * ============================================================================
 */

void cg_init_targets(void);
int cg_set_target(char *name);
struct cg_target *cg_get_target(void);
void cg_list_targets(void);
struct cg_target *cg_find_target(char *name);

/*
 * ============================================================================
 * SECTION: Helper Functions for Accessing Target Properties
 * ============================================================================
 *
 * These functions handle overrides and provide a clean API.
 */

/* Get assembler command (checks override first) */
char *cg_get_asm_cmd(void);

/* Get linker command (checks override first) */
char *cg_get_ld_cmd(void);

#endif /* CGTARGET_H */
