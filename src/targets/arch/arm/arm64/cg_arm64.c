/*
 * SubC Compiler - ARM64 (AArch64) Code Generator
 *
 * This file implements the code generator for the ARM64 architecture.
 * It generates GNU Assembler (GAS) syntax assembly code for AArch64.
 *
 * Architecture characteristics:
 *   - 64-bit registers and pointers
 *   - Little-endian byte order (default)
 *   - Stack grows downward (toward lower addresses)
 *   - 16-byte stack alignment required
 *   - PC-relative addressing with ADRP/ADD for globals
 *
 * Register usage (AAPCS64):
 *   x0-x7   - Arguments and return value (x0 = primary accumulator)
 *   x8      - Indirect result location register
 *   x9-x15  - Temporary registers (caller-saved)
 *   x16-x17 - Intra-procedure-call scratch registers (IP0, IP1)
 *   x18     - Platform register (reserved on some OSes)
 *   x19-x28 - Callee-saved registers
 *   x29     - Frame pointer (FP)
 *   x30     - Link register (LR)
 *   sp      - Stack pointer
 *
 * For SubC we use:
 *   x0  - Primary accumulator, return value
 *   x1  - Secondary accumulator, second operand
 *   x2  - Tertiary register, pointer for stores
 *   x3  - Scratch register
 *   x9  - Additional scratch for address calculations
 *   x29 - Frame pointer
 *   x30 - Link register
 *   sp  - Stack pointer
 *
 * Note: ARM64 uses 32-bit (w0-w31) or 64-bit (x0-x30) register names.
 * For SubC's 64-bit int/ptr model, we use x registers for pointers
 * and can use w registers for 32-bit operations if needed.
 *
 * Copyright (c) 2025 - Public Domain (CC0)
 */

#include <stdio.h>
#include <string.h>
#include "defs.h"
#include "data.h"
#include "decl.h"
#include "cgtarget.h"
#include "cg_arm64_funcs.h"

/* Track if we're generating for Darwin (different symbol conventions) */
#define IS_DARWIN (CG->os->target_os == OS_DARWIN)

/* Buffer for generating assembly lines */
static char buf[256];

/*
 * ============================================================================
 * SECTION: Helper Functions for Address Loading
 * ============================================================================
 *
 * ARM64 uses PC-relative addressing. For large offsets or global symbols,
 * we use ADRP (Address of 4KB Page) + ADD for the page offset.
 *
 * Darwin uses different syntax: symbol@PAGE and symbol@PAGEOFF
 * Linux uses :pg_hi21: and :lo12: relocations
 */

/*
 * cga64_lit2 - Load literal value into register
 *
 * @v: Value to load
 * @aux: Which register (0=x0, 1=x1, 2=x2)
 *
 * Small values use mov, large values use movz/movk sequence.
 */
void cga64_lit2(int v, int aux) {
    char *reg;
    unsigned int uv;
    
    if (aux == 2)
        reg = "x2";
    else if (aux == 1)
        reg = "x1";
    else
        reg = "x0";
    
    /* For values that fit in 16 bits, use simple mov */
    if (v >= 0 && v <= 65535) {
        sprintf(buf, "mov\t%s, #%d", reg, v);
        gen(buf);
    }
    else if (v >= -65536 && v < 0) {
        /* Negative values: use movn (move wide with NOT) */
        sprintf(buf, "mov\t%s, #%d", reg, v);
        gen(buf);
    }
    else {
        /* Larger values: use movz + movk sequence */
        uv = (unsigned int)v;
        sprintf(buf, "mov\t%s, #%d", reg, (int)(uv & 0xFFFF));
        gen(buf);
        if ((uv >> 16) & 0xFFFF) {
            sprintf(buf, "movk\t%s, #%d, lsl #16", reg, (int)((uv >> 16) & 0xFFFF));
            gen(buf);
        }
    }
}

/*
 * cga64_locladdr - Load local variable address
 *
 * @n: Stack offset from frame pointer (x29)
 * @aux: Which register (0=x0, 1=x1)
 */
void cga64_locladdr(int n, int aux) {
    char *reg = aux ? "x1" : "x0";
    
    if (n >= -256 && n <= 255) {
        /* Small offset: use add/sub with immediate */
        if (n >= 0) {
            sprintf(buf, "add\t%s, x29, #%d", reg, n);
        } else {
            sprintf(buf, "sub\t%s, x29, #%d", reg, -n);
        }
        gen(buf);
    }
    else {
        /* Large offset: load offset into scratch, then add */
        cga64_lit2(n, 2);
        sprintf(buf, "add\t%s, x29, x2", reg);
        gen(buf);
    }
}

/*
 * cga64_stataddr - Load static variable address
 *
 * @n: Label number
 * @aux: Which register (0=x0, 1=x1)
 */
void cga64_stataddr(int n, int aux) {
    char *reg = aux ? "x1" : "x0";
    
    if (IS_DARWIN) {
        /* Darwin: use @PAGE and @PAGEOFF */
        sprintf(buf, "adrp\t%s, %c%d@PAGE", reg, LPREFIX, n);
        gen(buf);
        sprintf(buf, "add\t%s, %s, %c%d@PAGEOFF", reg, reg, LPREFIX, n);
        gen(buf);
    } else {
        /* Linux: use :pg_hi21: and :lo12: */
        sprintf(buf, "adrp\t%s, :pg_hi21:%c%d", reg, LPREFIX, n);
        gen(buf);
        sprintf(buf, "add\t%s, %s, :lo12:%c%d", reg, reg, LPREFIX, n);
        gen(buf);
    }
}

/*
 * cga64_globaddr - Load global variable address
 *
 * @s: Symbol name
 * @aux: Which register (0=x0, 1=x1)
 */
void cga64_globaddr(char *s, int aux) {
    char *reg = aux ? "x1" : "x0";
    
    if (IS_DARWIN) {
        /* Darwin: symbols need underscore prefix and @PAGE/@PAGEOFF */
        sprintf(buf, "adrp\t%s, _%s@PAGE", reg, s);
        gen(buf);
        sprintf(buf, "add\t%s, %s, _%s@PAGEOFF", reg, reg, s);
        gen(buf);
    } else {
        /* Linux ELF */
        sprintf(buf, "adrp\t%s, :pg_hi21:%s", reg, s);
        gen(buf);
        sprintf(buf, "add\t%s, %s, :lo12:%s", reg, reg, s);
        gen(buf);
    }
}

/* Indirect load helpers */
static void cga64_ind2b(void) { gen("ldrb\tw1, [x1]"); }
static void cga64_ind2w(void) { gen("ldr\tx1, [x1]"); }

/* Store helpers */
static void cga64_storb(void) { gen("strb\tw0, [x1]"); }
static void cga64_storw(void) { gen("str\tx0, [x1]"); }

/*
 * ============================================================================
 * SECTION: Section Control Functions
 * ============================================================================
 */

void cga64_data(void) {
    if (IS_DARWIN) {
        gen(".data");
    } else {
        gen(".data");
    }
}

void cga64_text(void) {
    if (IS_DARWIN) {
        gen(".text");
    } else {
        gen(".text");
    }
}

void cga64_prelude(void) {
    if (IS_DARWIN) {
        gen(".section __TEXT,__text,regular,pure_instructions");
        gen(".build_version macos, 11, 0");
    } else {
        gen(".arch armv8-a");
    }
}

void cga64_postlude(void) {
    /* No postlude needed */
}

void cga64_public(char *s) {
    if (IS_DARWIN) {
        sgen("%s\t_%s", ".globl", s);
    } else {
        sgen("%s\t%s", ".globl", s);
    }
}

void cga64_name(char *s) {
    if (IS_DARWIN) {
        sgen("_%s:", s, "");
    } else {
        sgen("%s:", s, "");
    }
}

void cga64_funcname(char *s) {
    if (IS_DARWIN) {
        sgen("_%s:", s, "");
    } else {
        sgen("%s:", s, "");
    }
}

void cga64_align(void) {
    if (IS_DARWIN) {
        gen(".p2align 3");
    } else {
        gen(".align 3");
    }
}

/*
 * ============================================================================
 * SECTION: Literal and Clear Functions
 * ============================================================================
 */

void cga64_lit(int v) {
    cga64_lit2(v, 0);
}

void cga64_clear(void) {
    gen("mov\tx0, #0");
}

void cga64_clear2(void) {
    gen("mov\tx1, #0");
}

/*
 * ============================================================================
 * SECTION: Load Operations
 * ============================================================================
 */

void cga64_ldgb(char *s) {
    cga64_globaddr(s, 0);
    cga64_indb();
}

void cga64_ldgw(char *s) {
    cga64_globaddr(s, 0);
    cga64_indw();
}

void cga64_ldlb(int n) {
    cga64_locladdr(n, 0);
    cga64_indb();
}

void cga64_ldlw(int n) {
    cga64_locladdr(n, 0);
    cga64_indw();
}

void cga64_ldsb(int n) {
    cga64_stataddr(n, 0);
    cga64_indb();
}

void cga64_ldsw(int n) {
    cga64_stataddr(n, 0);
    cga64_indw();
}

void cga64_ldla(int n) {
    cga64_locladdr(n, 0);
}

void cga64_ldsa(int n) {
    cga64_stataddr(n, 0);
}

void cga64_ldga(char *s) {
    cga64_globaddr(s, 0);
}

void cga64_indb(void) {
    gen("ldrb\tw0, [x0]");
}

void cga64_indw(void) {
    gen("ldr\tx0, [x0]");
}

void cga64_ldlab(int id) {
    cga64_stataddr(id, 0);
}

/*
 * ============================================================================
 * SECTION: Stack Operations
 * ============================================================================
 */

void cga64_push(void) {
    /* ARM64 requires 16-byte stack alignment */
    gen("str\tx0, [sp, #-16]!");
}

void cga64_pushlit(int n) {
    cga64_lit(n);
    cga64_push();
}

void cga64_pop2(void) {
    gen("ldr\tx1, [sp], #16");
}

void cga64_swap(void) {
    gen("mov\tx3, x0");
    gen("mov\tx0, x1");
    gen("mov\tx1, x3");
}

void cga64_popptr(void) {
    gen("ldr\tx2, [sp], #16");
}

/*
 * ============================================================================
 * SECTION: Arithmetic Operations
 * ============================================================================
 */

void cga64_add(void) {
    gen("add\tx0, x1, x0");
}

void cga64_sub(void) {
    /* After swap: x0=first, x1=second, want first-second */
    /* sub x0, x0, x1 = x0 - x1 = first - second */
    gen("sub\tx0, x0, x1");
}

void cga64_mul(void) {
    gen("mul\tx0, x1, x0");
}

void cga64_div(void) {
    /* After swap: x0=first (dividend), x1=second (divisor) */
    /* sdiv x0, x0, x1 = x0 / x1 = first / second */
    gen("sdiv\tx0, x0, x1");
}

void cga64_mod(void) {
    /* After swap: x0=first (dividend), x1=second (divisor) */
    /* ARM64 doesn't have mod instruction, use: a % b = a - (a/b)*b */
    gen("sdiv\tx2, x0, x1");    /* x2 = x0 / x1 = first / second */
    gen("msub\tx0, x2, x1, x0"); /* x0 = x0 - x2*x1 = first - (first/second)*second */
}

void cga64_neg(void) {
    gen("neg\tx0, x0");
}

/*
 * ============================================================================
 * SECTION: Bitwise Operations
 * ============================================================================
 */

void cga64_and(void) {
    cga64_load2();
    gen("and\tx0, x1, x0");
}

void cga64_ior(void) {
    cga64_load2();
    gen("orr\tx0, x1, x0");
}

void cga64_xor(void) {
    cga64_load2();
    gen("eor\tx0, x1, x0");
}

void cga64_not(void) {
    gen("mvn\tx0, x0");
}

void cga64_shl(void) {
    gen("lsl\tx0, x1, x0");
}

void cga64_shr(void) {
    gen("asr\tx0, x1, x0");
}

/*
 * ============================================================================
 * SECTION: Comparison Operations
 * ============================================================================
 */

static void cga64_cmp(char *cond) {
    gen("mov\tx3, x0");
    gen("mov\tx0, #0");
    if (empty == Q_type) {
        cga64_pop2();
        gen("cmp\tx1, x3");
    }
    else {
        cga64_load2();
        gen("cmp\tx3, x1");
    }
    sprintf(buf, "csinc\tx0, x0, xzr, %s", cond);
    gen(buf);
}

void cga64_eq(void)  { cga64_cmp("ne"); }
void cga64_ne(void)  { cga64_cmp("eq"); }
void cga64_lt(void)  { cga64_cmp("ge"); }
void cga64_gt(void)  { cga64_cmp("le"); }
void cga64_le(void)  { cga64_cmp("gt"); }
void cga64_ge(void)  { cga64_cmp("lt"); }
void cga64_ult(void) { cga64_cmp("hs"); }
void cga64_ugt(void) { cga64_cmp("ls"); }
void cga64_ule(void) { cga64_cmp("hi"); }
void cga64_uge(void) { cga64_cmp("lo"); }

/*
 * ============================================================================
 * SECTION: Conditional Branch Operations
 * ============================================================================
 */

static void cga64_brcond(char *cond, int n) {
    int lab;
    
    lab = label();
    if (empty == Q_type) {
        cga64_pop2();
        gen("cmp\tx1, x0");
    }
    else {
        cga64_load2();
        gen("cmp\tx0, x1");
    }
    lgen("%s\t%c%d", cond, lab);
    lgen("%s\t%c%d", "b", n);
    genlab(lab);
}

void cga64_breq(int n)  { cga64_brcond("b.eq", n); }
void cga64_brne(int n)  { cga64_brcond("b.ne", n); }
void cga64_brlt(int n)  { cga64_brcond("b.lt", n); }
void cga64_brgt(int n)  { cga64_brcond("b.gt", n); }
void cga64_brle(int n)  { cga64_brcond("b.le", n); }
void cga64_brge(int n)  { cga64_brcond("b.ge", n); }
void cga64_brult(int n) { cga64_brcond("b.lo", n); }
void cga64_brugt(int n) { cga64_brcond("b.hi", n); }
void cga64_brule(int n) { cga64_brcond("b.ls", n); }
void cga64_bruge(int n) { cga64_brcond("b.hs", n); }

/*
 * ============================================================================
 * SECTION: Boolean and Logic Operations
 * ============================================================================
 */

void cga64_lognot(void) {
    gen("cmp\tx0, #0");
    gen("cset\tx0, eq");
}

void cga64_bool(void) {
    gen("cmp\tx0, #0");
    gen("cset\tx0, ne");
}

/*
 * ============================================================================
 * SECTION: Pointer Scaling Operations
 * ============================================================================
 */

void cga64_scale(void) {
    gen("lsl\tx0, x0, #3");
}

void cga64_scale2(void) {
    gen("lsl\tx1, x1, #3");
}

void cga64_unscale(void) {
    gen("lsr\tx0, x0, #3");
}

void cga64_scaleby(int v) {
    cga64_lit2(v, 1);
    gen("mul\tx0, x0, x1");
}

void cga64_scale2by(int v) {
    cga64_lit2(v, 2);
    gen("mul\tx1, x1, x2");
}

void cga64_unscaleby(int v) {
    cga64_lit2(v, 1);
    gen("udiv\tx0, x0, x1");
}

/*
 * ============================================================================
 * SECTION: Increment/Decrement Operations
 * ============================================================================
 */

void cga64_ldinc(void) {
    gen("mov\tx2, x0");
}

void cga64_inc1pi(int v) {
    cga64_lit2(v, 1);
    gen("ldr\tx3, [x0]");
    gen("add\tx3, x3, x1");
    gen("str\tx3, [x0]");
}

void cga64_dec1pi(int v) {
    cga64_lit2(v, 1);
    gen("ldr\tx3, [x0]");
    gen("sub\tx3, x3, x1");
    gen("str\tx3, [x0]");
}

void cga64_inc2pi(int v) {
    cga64_lit2(v, 1);
    gen("ldr\tx3, [x2]");
    gen("add\tx3, x3, x1");
    gen("str\tx3, [x2]");
}

void cga64_dec2pi(int v) {
    cga64_lit2(v, 1);
    gen("ldr\tx3, [x2]");
    gen("sub\tx3, x3, x1");
    gen("str\tx3, [x2]");
}

void cga64_incpl(int a, int v) {
    cga64_lit2(v, 2);
    cga64_locladdr(a, 1);
    gen("ldr\tx3, [x1]");
    gen("add\tx3, x3, x2");
    gen("str\tx3, [x1]");
}

void cga64_decpl(int a, int v) {
    cga64_lit2(v, 2);
    cga64_locladdr(a, 1);
    gen("ldr\tx3, [x1]");
    gen("sub\tx3, x3, x2");
    gen("str\tx3, [x1]");
}

void cga64_incps(int a, int v) {
    cga64_lit2(v, 2);
    cga64_stataddr(a, 1);
    gen("ldr\tx3, [x1]");
    gen("add\tx3, x3, x2");
    gen("str\tx3, [x1]");
}

void cga64_decps(int a, int v) {
    cga64_lit2(v, 2);
    cga64_stataddr(a, 1);
    gen("ldr\tx3, [x1]");
    gen("sub\tx3, x3, x2");
    gen("str\tx3, [x1]");
}

void cga64_incpg(char *s, int v) {
    cga64_lit2(v, 2);
    cga64_globaddr(s, 1);
    gen("ldr\tx3, [x1]");
    gen("add\tx3, x3, x2");
    gen("str\tx3, [x1]");
}

void cga64_decpg(char *s, int v) {
    cga64_lit2(v, 2);
    cga64_globaddr(s, 1);
    gen("ldr\tx3, [x1]");
    gen("sub\tx3, x3, x2");
    gen("str\tx3, [x1]");
}

void cga64_inc1iw(void) {
    gen("ldr\tx1, [x0]");
    gen("add\tx1, x1, #1");
    gen("str\tx1, [x0]");
}

void cga64_dec1iw(void) {
    gen("ldr\tx1, [x0]");
    gen("sub\tx1, x1, #1");
    gen("str\tx1, [x0]");
}

void cga64_inc2iw(void) {
    gen("ldr\tx1, [x2]");
    gen("add\tx1, x1, #1");
    gen("str\tx1, [x2]");
}

void cga64_dec2iw(void) {
    gen("ldr\tx1, [x2]");
    gen("sub\tx1, x1, #1");
    gen("str\tx1, [x2]");
}

void cga64_inclw(int a) {
    cga64_locladdr(a, 1);
    gen("ldr\tx2, [x1]");
    gen("add\tx2, x2, #1");
    gen("str\tx2, [x1]");
}

void cga64_declw(int a) {
    cga64_locladdr(a, 1);
    gen("ldr\tx2, [x1]");
    gen("sub\tx2, x2, #1");
    gen("str\tx2, [x1]");
}

void cga64_incsw(int a) {
    cga64_stataddr(a, 1);
    gen("ldr\tx2, [x1]");
    gen("add\tx2, x2, #1");
    gen("str\tx2, [x1]");
}

void cga64_decsw(int a) {
    cga64_stataddr(a, 1);
    gen("ldr\tx2, [x1]");
    gen("sub\tx2, x2, #1");
    gen("str\tx2, [x1]");
}

void cga64_incgw(char *s) {
    cga64_globaddr(s, 1);
    gen("ldr\tx2, [x1]");
    gen("add\tx2, x2, #1");
    gen("str\tx2, [x1]");
}

void cga64_decgw(char *s) {
    cga64_globaddr(s, 1);
    gen("ldr\tx2, [x1]");
    gen("sub\tx2, x2, #1");
    gen("str\tx2, [x1]");
}

void cga64_inc1ib(void) {
    gen("ldrb\tw1, [x0]");
    gen("add\tw1, w1, #1");
    gen("strb\tw1, [x0]");
}

void cga64_dec1ib(void) {
    gen("ldrb\tw1, [x0]");
    gen("sub\tw1, w1, #1");
    gen("strb\tw1, [x0]");
}

void cga64_inc2ib(void) {
    gen("ldrb\tw1, [x2]");
    gen("add\tw1, w1, #1");
    gen("strb\tw1, [x2]");
}

void cga64_dec2ib(void) {
    gen("ldrb\tw1, [x2]");
    gen("sub\tw1, w1, #1");
    gen("strb\tw1, [x2]");
}

void cga64_inclb(int a) {
    cga64_locladdr(a, 1);
    gen("ldrb\tw2, [x1]");
    gen("add\tw2, w2, #1");
    gen("strb\tw2, [x1]");
}

void cga64_declb(int a) {
    cga64_locladdr(a, 1);
    gen("ldrb\tw2, [x1]");
    gen("sub\tw2, w2, #1");
    gen("strb\tw2, [x1]");
}

void cga64_incsb(int a) {
    cga64_stataddr(a, 1);
    gen("ldrb\tw2, [x1]");
    gen("add\tw2, w2, #1");
    gen("strb\tw2, [x1]");
}

void cga64_decsb(int a) {
    cga64_stataddr(a, 1);
    gen("ldrb\tw2, [x1]");
    gen("sub\tw2, w2, #1");
    gen("strb\tw2, [x1]");
}

void cga64_incgb(char *s) {
    cga64_globaddr(s, 1);
    gen("ldrb\tw2, [x1]");
    gen("add\tw2, w2, #1");
    gen("strb\tw2, [x1]");
}

void cga64_decgb(char *s) {
    cga64_globaddr(s, 1);
    gen("ldrb\tw2, [x1]");
    gen("sub\tw2, w2, #1");
    gen("strb\tw2, [x1]");
}

/*
 * ============================================================================
 * SECTION: Unconditional Branch Operations
 * ============================================================================
 */

static void cga64_br(char *cond, int n) {
    int lab;
    
    lab = label();
    gen("cmp\tx0, #0");
    lgen("%s\t%c%d", cond, lab);
    lgen("%s\t%c%d", "b", n);
    genlab(lab);
}

void cga64_brtrue(int n) {
    cga64_br("b.eq", n);
}

void cga64_brfalse(int n) {
    cga64_br("b.ne", n);
}

void cga64_jump(int n) {
    lgen("%s\t%c%d", "b", n);
}

/*
 * ============================================================================
 * SECTION: Switch Statement Support
 * ============================================================================
 */

void cga64_ldswtch(int n) {
    cga64_stataddr(n, 1);
}

void cga64_calswtch(void) {
    if (IS_DARWIN) {
        gen("bl\t_switch");
    } else {
        gen("bl\tswitch");
    }
}

void cga64_case(int v, int l) {
    lgen2(".quad\t%d, %c%d", v, l);
}

/*
 * ============================================================================
 * SECTION: Store Operations
 * ============================================================================
 */

void cga64_storib(void) {
    gen("strb\tw0, [x2]");
}

void cga64_storiw(void) {
    gen("str\tx0, [x2]");
}

void cga64_storlb(int n) {
    cga64_locladdr(n, 1);
    cga64_storb();
}

void cga64_storlw(int n) {
    cga64_locladdr(n, 1);
    cga64_storw();
}

void cga64_storsb(int n) {
    cga64_stataddr(n, 1);
    cga64_storb();
}

void cga64_storsw(int n) {
    cga64_stataddr(n, 1);
    cga64_storw();
}

void cga64_storgb(char *s) {
    cga64_globaddr(s, 1);
    cga64_storb();
}

void cga64_storgw(char *s) {
    cga64_globaddr(s, 1);
    cga64_storw();
}

/*
 * ============================================================================
 * SECTION: Function Call Operations
 * ============================================================================
 */

void cga64_initlw(int v, int a) {
    cga64_lit(v);
    cga64_locladdr(a, 1);
    cga64_storw();
}

void cga64_call(char *s) {
    if (IS_DARWIN) {
        sgen("%s\t_%s", "bl", s);
    } else {
        sgen("%s\t%s", "bl", s);
    }
}

void cga64_calr(void) {
    gen("blr\tx0");
}

void cga64_stack(int n) {
    /* Adjust stack pointer - must maintain 16-byte alignment */
    if (n > 0) {
        n = (n + 15) & ~15;  /* Align to 16 bytes */
        if (n <= 4095) {
            ngen("%s\tsp, sp, #%d", "add", n);
        } else {
            cga64_lit2(n, 1);
            gen("add\tsp, sp, x1");
        }
    } else if (n < 0) {
        n = ((-n) + 15) & ~15;  /* Align to 16 bytes */
        if (n <= 4095) {
            ngen("%s\tsp, sp, #%d", "sub", n);
        } else {
            cga64_lit2(n, 1);
            gen("sub\tsp, sp, x1");
        }
    }
}

void cga64_entry(int lsize) {
    (void)lsize;  /* Stack allocation handled by genstack() */
    /* Save frame pointer and link register */
    gen("stp\tx29, x30, [sp, #-16]!");
    gen("mov\tx29, sp");
}

void cga64_exit(void) {
    /* Restore stack pointer to frame pointer (deallocates locals) */
    gen("mov\tsp, x29");
    /* Restore frame pointer and link register, return */
    gen("ldp\tx29, x30, [sp], #16");
    gen("ret");
}

/*
 * cga64_movearg - Move accumulator (x0) to argument register n
 *
 * For AAPCS64 calling convention, arguments go in x0-x7.
 * This function moves x0 to the appropriate register for argument n.
 * Used when -R (system runtime) flag is active.
 *
 * @n: Argument number (0-7)
 */
void cga64_movearg(int n) {
    if (n < 0 || n > 7) {
        /* Too many arguments for register passing */
        return;
    }
    if (n == 0) {
        /* Already in x0, nothing to do */
        return;
    }
    sprintf(buf, "mov\tx%d, x0", n);
    gen(buf);
}

/*
 * cga64_emitargs - Emit function arguments using AAPCS64 calling convention
 *
 * This function handles argument emission for system runtime mode (-R flag).
 * On ARM64 AAPCS64, arguments go in registers x0-x7. If there are more than
 * 8 arguments, the rest go on the stack (not implemented yet).
 *
 * The argument tree is structured as a linked list via 'left' pointers,
 * with each argument value in 'right'. The list is in reverse order:
 * the last argument is at the root, first argument is deepest.
 *
 * We process from last to first, so that x0 ends up with the first argument.
 *
 * @emitter: callback to emit a single argument node (leaves result in x0)
 * @args: argument list node (opaque, uses accessor functions)
 * @nargs: total number of arguments
 * @return: number of stack slots used (for cleanup after call)
 */

/* Accessor functions defined in tree.c */
extern void *node_get_left(void *n);
extern void *node_get_right(void *n);

/* Frontend function to clear accumulator state (prevents spill on next commit) */
extern void clear(int q);

/*
 * Emit variadic arguments to stack in correct order.
 * On ARM64 Darwin, variadic args go on stack at [sp], [sp+8], [sp+16], etc.
 * We need to emit them in forward order (first variadic at [sp]).
 */
static int emit_variadic_args(void (*emitter)(void*), void *a, int nfixed, int idx) {
    void *left, *right;
    int count = 0;
    
    if (a == NULL) return 0;
    if (idx < nfixed) return 0;  /* Not a variadic arg */
    
    left = node_get_left(a);
    right = node_get_right(a);
    
    /* First emit earlier variadic args (recursive) */
    count = emit_variadic_args(emitter, left, nfixed, idx - 1);
    
    /* Then emit this variadic arg */
    if (right != NULL) {
        emitter(right);
    }
    
    /* Store to stack at correct offset */
    /* sp points to variadic area, store at [sp + count*8] */
    sprintf(buf, "str\tx0, [sp, #%d]", count * 8);
    gen(buf);
    
    clear(1);
    return count + 1;
}

/*
 * Emit fixed arguments to registers (last to first so x0 gets first arg).
 */
static void emit_fixed_args(void (*emitter)(void*), void *a, int nfixed, int idx) {
    void *left, *right;
    
    if (a == NULL) return;
    if (nfixed >= 0 && idx >= nfixed) {
        /* Skip variadic args */
        emit_fixed_args(emitter, node_get_left(a), nfixed, idx - 1);
        return;
    }
    
    left = node_get_left(a);
    right = node_get_right(a);
    
    /* Process this fixed argument */
    if (right != NULL) {
        emitter(right);
    }
    
    /* Move to appropriate register */
    if (idx < 8) {
        cga64_movearg(idx);
    }
    
    clear(1);
    
    /* Process earlier fixed arguments */
    emit_fixed_args(emitter, left, nfixed, idx - 1);
}

int cga64_emitargs(void (*emitter)(void*), void *args, int nargs, int nfixed) {
    int nvarargs = 0;
    int stack_slots = 0;
    
    if (nfixed >= 0 && nfixed < nargs) {
        /* Has variadic arguments */
        nvarargs = nargs - nfixed;
        
        /* Ensure we're in text section before emitting code */
        cga64_text();
        
        /* Allocate stack space for variadic args (16-byte aligned) */
        stack_slots = (nvarargs + 1) & ~1;  /* Round up to even for 16-byte alignment */
        sprintf(buf, "sub\tsp, sp, #%d", stack_slots * 8);
        gen(buf);
        
        /* Emit variadic args to stack */
        emit_variadic_args(emitter, args, nfixed, nargs - 1);
        
        /* Emit fixed args to registers */
        emit_fixed_args(emitter, args, nfixed, nargs - 1);
    } else {
        /* No variadic arguments - all go to registers */
        emit_fixed_args(emitter, args, -1, nargs - 1);
    }
    
    return stack_slots;
}

/*
 * ============================================================================
 * SECTION: Data Definition
 * ============================================================================
 */

void cga64_defb(int v) {
    ngen("%s\t%d", ".byte", v);
}

void cga64_defw(int v) {
    ngen("%s\t%d", ".quad", v);
}

void cga64_defp(int v) {
    ngen("%s\t%d", ".quad", v);
}

void cga64_defl(int v) {
    lgen("%s\t%c%d", ".quad", v);
}

void cga64_defc(int c) {
    ngen("%s\t'%c'", ".byte", c);
}

void cga64_gbss(char *s, int z) {
    if (IS_DARWIN) {
        ngen(".comm\t_%s, %d", s, z);
    } else {
        ngen(".comm\t%s, %d", s, z);
    }
}

void cga64_lbss(char *s, int z) {
    if (IS_DARWIN) {
        ngen(".lcomm\t_%s, %d", s, z);
    } else {
        ngen(".lcomm\t%s, %d", s, z);
    }
}

/*
 * ============================================================================
 * SECTION: Synthesizer Support
 * ============================================================================
 */

int cga64_load2(void) {
    int   n, q;
    char *s;

    n = Q_val;
    s = gsym(Q_name);
    
    switch (Q_type) {
    case addr_auto:
        cga64_locladdr(n, 1);
        break;
    case addr_static:
        cga64_stataddr(n, 1);
        break;
    case addr_globl:
        cga64_globaddr(s, 1);
        break;
    case addr_label:
        cga64_stataddr(n, 1);
        break;
    case literal:
        cga64_lit2(n, 1);
        break;
    case auto_byte:
        cga64_locladdr(n, 1);
        cga64_ind2b();
        break;
    case auto_word:
        cga64_locladdr(n, 1);
        cga64_ind2w();
        break;
    case static_byte:
        cga64_stataddr(n, 1);
        cga64_ind2b();
        break;
    case static_word:
        cga64_stataddr(n, 1);
        cga64_ind2w();
        break;
    case globl_byte:
        cga64_globaddr(s, 1);
        cga64_ind2b();
        break;
    case globl_word:
        cga64_globaddr(s, 1);
        cga64_ind2w();
        break;
    case empty:
        cga64_pop2();
        break;
    default:
        fatal("internal: bad type in cga64_load2()");
    }
    
    q = Q_type;
    Q_type = empty;
    return empty == q;
}
