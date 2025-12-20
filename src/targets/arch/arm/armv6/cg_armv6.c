/*
 * SubC Compiler - ARMv6 (32-bit ARM) Target
 *
 * This file implements the code generator for the ARMv6 architecture.
 * It generates GNU Assembler (GAS) syntax assembly code for ARM.
 *
 * Based on the original cgarmv6.c by NMH.
 *
 * Architecture characteristics:
 *   - 32-bit registers and pointers
 *   - Little-endian byte order (can be big-endian, but we assume LE)
 *   - Stack grows downward (toward lower addresses)
 *   - PC-relative addressing for constants and addresses
 *
 * Register usage:
 *   r0  - Primary accumulator, return value
 *   r1  - Auxiliary register, second operand
 *   r2  - Tertiary register, pointer for stores
 *   r3  - Scratch register
 *   r11 - Frame pointer
 *   sp  - Stack pointer
 *   lr  - Link register (return address)
 *   pc  - Program counter
 *
 * Copyright (c) 2011-2025 - Public Domain (CC0)
 */

#include "../../../../defs.h"
#include "../../../../data.h"
#include "../../../../decl.h"
#include "../../cgtarget.h"

/*
 * ============================================================================
 * SECTION: Forward Declarations
 * ============================================================================
 */

void cgarm_data(void);
void cgarm_text(void);
void cgarm_prelude(void);
void cgarm_postlude(void);
void cgarm_public(char *s);
void cgarm_align(void);

void cgarm_lit(int v);
void cgarm_clear(void);
void cgarm_clear2(void);

void cgarm_ldgb(char *s);
void cgarm_ldgw(char *s);
void cgarm_ldlb(int n);
void cgarm_ldlw(int n);
void cgarm_ldsb(int n);
void cgarm_ldsw(int n);
void cgarm_ldla(int n);
void cgarm_ldsa(int n);
void cgarm_ldga(char *s);
void cgarm_indb(void);
void cgarm_indw(void);
void cgarm_ldlab(int id);

void cgarm_push(void);
void cgarm_pushlit(int n);
void cgarm_pop2(void);
void cgarm_swap(void);
void cgarm_popptr(void);

void cgarm_add(void);
void cgarm_sub(void);
void cgarm_mul(void);
void cgarm_div(void);
void cgarm_mod(void);
void cgarm_neg(void);

void cgarm_and(void);
void cgarm_ior(void);
void cgarm_xor(void);
void cgarm_not(void);
void cgarm_shl(void);
void cgarm_shr(void);

void cgarm_eq(void);
void cgarm_ne(void);
void cgarm_lt(void);
void cgarm_gt(void);
void cgarm_le(void);
void cgarm_ge(void);
void cgarm_ult(void);
void cgarm_ugt(void);
void cgarm_ule(void);
void cgarm_uge(void);

void cgarm_breq(int n);
void cgarm_brne(int n);
void cgarm_brlt(int n);
void cgarm_brgt(int n);
void cgarm_brle(int n);
void cgarm_brge(int n);
void cgarm_brult(int n);
void cgarm_brugt(int n);
void cgarm_brule(int n);
void cgarm_bruge(int n);

void cgarm_lognot(void);
void cgarm_bool(void);

void cgarm_scale(void);
void cgarm_scale2(void);
void cgarm_unscale(void);
void cgarm_scaleby(int v);
void cgarm_scale2by(int v);
void cgarm_unscaleby(int v);

void cgarm_ldinc(void);
void cgarm_inc1pi(int v);
void cgarm_dec1pi(int v);
void cgarm_inc2pi(int v);
void cgarm_dec2pi(int v);

void cgarm_incpl(int a, int v);
void cgarm_decpl(int a, int v);
void cgarm_inclw(int a);
void cgarm_declw(int a);
void cgarm_inclb(int a);
void cgarm_declb(int a);

void cgarm_incps(int a, int v);
void cgarm_decps(int a, int v);
void cgarm_incsw(int a);
void cgarm_decsw(int a);
void cgarm_incsb(int a);
void cgarm_decsb(int a);

void cgarm_incpg(char *s, int v);
void cgarm_decpg(char *s, int v);
void cgarm_incgw(char *s);
void cgarm_decgw(char *s);
void cgarm_incgb(char *s);
void cgarm_decgb(char *s);

void cgarm_inc1iw(void);
void cgarm_dec1iw(void);
void cgarm_inc2iw(void);
void cgarm_dec2iw(void);
void cgarm_inc1ib(void);
void cgarm_dec1ib(void);
void cgarm_inc2ib(void);
void cgarm_dec2ib(void);

void cgarm_brtrue(int n);
void cgarm_brfalse(int n);
void cgarm_jump(int n);

void cgarm_ldswtch(int n);
void cgarm_calswtch(void);
void cgarm_case(int v, int l);

void cgarm_storib(void);
void cgarm_storiw(void);
void cgarm_storlb(int n);
void cgarm_storlw(int n);
void cgarm_storsb(int n);
void cgarm_storsw(int n);
void cgarm_storgb(char *s);
void cgarm_storgw(char *s);

void cgarm_initlw(int v, int a);
void cgarm_call(char *s);
void cgarm_calr(void);
void cgarm_stack(int n);
void cgarm_entry(void);
void cgarm_exit(void);

void cgarm_defb(int v);
void cgarm_defw(int v);
void cgarm_defp(int v);
void cgarm_defl(int v);
void cgarm_defc(int c);
void cgarm_gbss(char *s, int z);
void cgarm_lbss(char *s, int z);

int cgarm_load2(void);

/*
 * ============================================================================
 * SECTION: PC-Relative Addressing Helper Functions
 * ============================================================================
 *
 * ARM uses PC-relative addressing for loading constants and addresses.
 * For values that don't fit in immediate fields, we generate:
 *   ldr rN, Lx      ; load from literal pool
 *   b   Ly          ; skip over the data
 * Lx: .long value   ; the actual value
 * Ly:               ; continue here
 */

/*
 * cgarm_lit2 - Load literal value into register
 *
 * @v: Value to load
 * @aux: Which register (0=r0, 1=r1, 2=r2)
 *
 * Small values use mov/mvn, large values use PC-relative load.
 */
void cgarm_lit2(int v, int aux) {
    int l, skip;
    
    if (0 <= v && v <= 127) {
        /* Small positive: use mov */
        if (aux == 2)
            ngen("%s\tr2,#%d", "mov", v);
        else if (aux == 1)
            ngen("%s\tr1,#%d", "mov", v);
        else
            ngen("%s\tr0,#%d", "mov", v);
    }
    else if (-128 <= v && v < 0) {
        /* Small negative: use mvn (move not) */
        v = -v - 1;
        if (aux == 2)
            ngen("%s\tr2,#%d", "mvn", v);
        else if (aux == 1)
            ngen("%s\tr1,#%d", "mvn", v);
        else
            ngen("%s\tr0,#%d", "mvn", v);
    }
    else {
        /* Large value: use PC-relative load */
        l = label();
        if (aux == 2)
            lgen("%s\tr2,%c%d", "ldr", l);
        else if (aux == 1)
            lgen("%s\tr1,%c%d", "ldr", l);
        else
            lgen("%s\tr0,%c%d", "ldr", l);
        skip = label();
        lgen("%s\t%c%d", "b", skip);
        genlab(l);
        cgarm_defw(v);
        genlab(skip);
    }
}

/*
 * cgarm_locladdr - Load local variable address
 *
 * @n: Stack offset from frame pointer (r11)
 * @aux: Which register (0=r0, 1=r1)
 */
void cgarm_locladdr(int n, int aux) {
    if (n < -255 || n > 255) {
        /* Offset too large for immediate, use register */
        cgarm_lit2(n, 2);
        if (aux)
            gen("add\tr1,r11,r2");
        else
            gen("add\tr0,r11,r2");
    }
    else {
        if (aux)
            ngen("%s\tr1,r11,#%d", "add", n);
        else
            ngen("%s\tr0,r11,#%d", "add", n);
    }
}

/*
 * cgarm_stataddr - Load static variable address
 *
 * @n: Label number
 * @aux: Which register (0=r0, 1=r1)
 */
void cgarm_stataddr(int n, int aux) {
    int l, skip;
    
    l = label();
    if (aux)
        lgen("%s\tr1,%c%d", "ldr", l);
    else
        lgen("%s\tr0,%c%d", "ldr", l);
    skip = label();
    lgen("%s\t%c%d", "b", skip);
    genlab(l);
    cgarm_defl(n);
    genlab(skip);
}

/*
 * cgarm_globaddr - Load global variable address
 *
 * @s: Symbol name
 * @aux: Which register (0=r0, 1=r1)
 */
void cgarm_globaddr(char *s, int aux) {
    int l, skip;
    
    l = label();
    if (aux)
        lgen("%s\tr1,%c%d", "ldr", l);
    else
        lgen("%s\tr0,%c%d", "ldr", l);
    skip = label();
    lgen("%s\t%c%d", "b", skip);
    genlab(l);
    sgen("%s\t%s", ".long", s);
    genlab(skip);
}

/* Indirect load helpers */
void cgarm_ind2b(void) { gen("ldrb\tr1,[r1]"); }
void cgarm_ind2w(void) { gen("ldr\tr1,[r1]"); }

/* Store helpers */
void cgarm_storb(void) { gen("strb\tr0,[r1]"); }
void cgarm_storw(void) { gen("str\tr0,[r1]"); }

/*
 * ============================================================================
 * SECTION: Section Control Functions
 * ============================================================================
 */

void cgarm_data(void) {
    gen(".data");
}

void cgarm_text(void) {
    gen(".text");
}

void cgarm_prelude(void) {
    gen(".syntax unified");
}

void cgarm_postlude(void) {
    /* No postlude needed */
}

void cgarm_public(char *s) {
    ngen(".globl\t%s", s, 0);
}

void cgarm_align(void) {
    gen(".align 2");
}

/*
 * ============================================================================
 * SECTION: Literal and Clear Functions
 * ============================================================================
 */

void cgarm_lit(int v) {
    cgarm_lit2(v, 0);
}

void cgarm_clear(void) {
    /* unused in ARM - cglit2 handles zero */
}

void cgarm_clear2(void) {
    /* unused in ARM - cglit2 handles zero */
}

/*
 * ============================================================================
 * SECTION: Load Operations
 * ============================================================================
 */

void cgarm_ldgb(char *s) {
    cgarm_globaddr(s, 0);
    cgarm_indb();
}

void cgarm_ldgw(char *s) {
    cgarm_globaddr(s, 0);
    cgarm_indw();
}

void cgarm_ldlb(int n) {
    cgarm_locladdr(n, 0);
    cgarm_indb();
}

void cgarm_ldlw(int n) {
    cgarm_locladdr(n, 0);
    cgarm_indw();
}

void cgarm_ldsb(int n) {
    cgarm_stataddr(n, 0);
    cgarm_indb();
}

void cgarm_ldsw(int n) {
    cgarm_stataddr(n, 0);
    cgarm_indw();
}

void cgarm_ldla(int n) {
    cgarm_locladdr(n, 0);
}

void cgarm_ldsa(int n) {
    cgarm_stataddr(n, 0);
}

void cgarm_ldga(char *s) {
    cgarm_globaddr(s, 0);
}

void cgarm_indb(void) {
    gen("ldrb\tr0,[r0]");
}

void cgarm_indw(void) {
    gen("ldr\tr0,[r0]");
}

void cgarm_ldlab(int id) {
    cgarm_stataddr(id, 0);
}

/*
 * ============================================================================
 * SECTION: Stack Operations
 * ============================================================================
 */

void cgarm_push(void) {
    gen("push\t{r0}");
}

void cgarm_pushlit(int n) {
    cgarm_lit(n);
    cgarm_push();
}

void cgarm_pop2(void) {
    gen("pop\t{r1}");
}

void cgarm_swap(void) {
    gen("mov\tr2,r0");
    gen("mov\tr0,r1");
    gen("mov\tr1,r2");
}

void cgarm_popptr(void) {
    gen("pop\t{r2}");
}

/*
 * ============================================================================
 * SECTION: Arithmetic Operations
 * ============================================================================
 */

void cgarm_add(void) {
    gen("add\tr0,r0,r1");
}

void cgarm_sub(void) {
    gen("sub\tr0,r0,r1");
}

void cgarm_mul(void) {
    gen("mul\tr0,r0,r1");
}

void cgarm_div(void) {
    gen("bl\tsdiv");
}

void cgarm_mod(void) {
    gen("bl\tsrem");
}

void cgarm_neg(void) {
    gen("neg\tr0,r0");
}

/*
 * ============================================================================
 * SECTION: Bitwise Operations
 * ============================================================================
 */

void cgarm_and(void) {
    cgarm_load2();
    gen("and\tr0,r0,r1");
}

void cgarm_ior(void) {
    cgarm_load2();
    gen("orr\tr0,r0,r1");
}

void cgarm_xor(void) {
    cgarm_load2();
    gen("eor\tr0,r0,r1");
}

void cgarm_not(void) {
    gen("mvn\tr0,r0");
}

void cgarm_shl(void) {
    gen("lsl\tr0,r0,r1");
}

void cgarm_shr(void) {
    gen("asr\tr0,r0,r1");
}

/*
 * ============================================================================
 * SECTION: Comparison Operations
 * ============================================================================
 */

void cgarm_cmp(char *inst) {
    gen("mov\tr3,r0");
    gen("mov\tr0,#0");
    if (empty == Q_type) {
        cgarm_pop2();
        gen("cmp\tr1,r3");
    }
    else {
        cgarm_load2();
        gen("cmp\tr3,r1");
    }
    ngen("%s\tr0,#%d", inst, 1);
}

void cgarm_eq(void)  { cgarm_cmp("moveq"); }
void cgarm_ne(void)  { cgarm_cmp("movne"); }
void cgarm_lt(void)  { cgarm_cmp("movlt"); }
void cgarm_gt(void)  { cgarm_cmp("movgt"); }
void cgarm_le(void)  { cgarm_cmp("movle"); }
void cgarm_ge(void)  { cgarm_cmp("movge"); }
void cgarm_ult(void) { cgarm_cmp("movlo"); }
void cgarm_ugt(void) { cgarm_cmp("movhi"); }
void cgarm_ule(void) { cgarm_cmp("movls"); }
void cgarm_uge(void) { cgarm_cmp("movhs"); }

/*
 * ============================================================================
 * SECTION: Conditional Branch Operations
 * ============================================================================
 */

void cgarm_brcond(char *i, int n) {
    int lab;
    
    lab = label();
    if (empty == Q_type) {
        cgarm_pop2();
        gen("cmp\tr1,r0");
    }
    else {
        cgarm_load2();
        gen("cmp\tr0,r1");
    }
    lgen("%s\t%c%d", i, lab);
    lgen("%s\t%c%d", "b", n);
    genlab(lab);
}

void cgarm_breq(int n)  { cgarm_brcond("beq", n); }
void cgarm_brne(int n)  { cgarm_brcond("bne", n); }
void cgarm_brlt(int n)  { cgarm_brcond("blt", n); }
void cgarm_brgt(int n)  { cgarm_brcond("bgt", n); }
void cgarm_brle(int n)  { cgarm_brcond("ble", n); }
void cgarm_brge(int n)  { cgarm_brcond("bge", n); }
void cgarm_brult(int n) { cgarm_brcond("blo", n); }
void cgarm_brugt(int n) { cgarm_brcond("bhi", n); }
void cgarm_brule(int n) { cgarm_brcond("bls", n); }
void cgarm_bruge(int n) { cgarm_brcond("bhs", n); }

/*
 * ============================================================================
 * SECTION: Boolean and Logic Operations
 * ============================================================================
 */

void cgarm_lognot(void) {
    gen("cmp\tr0,#0");
    gen("mov\tr0,#0");
    gen("moveq\tr0,#1");
}

void cgarm_bool(void) {
    gen("cmp\tr0,#0");
    gen("mov\tr0,#0");
    gen("movne\tr0,#1");
}

/*
 * ============================================================================
 * SECTION: Pointer Scaling Operations
 * ============================================================================
 */

void cgarm_scale(void) {
    gen("lsl\tr0,#2");
}

void cgarm_scale2(void) {
    gen("lsl\tr1,#2");
}

void cgarm_unscale(void) {
    gen("lsr\tr0,#2");
}

void cgarm_scaleby(int v) {
    cgarm_lit2(v, 1);
    gen("mul\tr0,r0,r1");
}

void cgarm_scale2by(int v) {
    cgarm_lit2(v, 2);
    gen("mul\tr1,r1,r2");
}

void cgarm_unscaleby(int v) {
    cgarm_lit2(v, 1);
    gen("bl\tudiv");
}

/*
 * ============================================================================
 * SECTION: Increment/Decrement Operations
 * ============================================================================
 */

void cgarm_ldinc(void) {
    gen("mov\tr2,r0");
}

void cgarm_inc1pi(int v) {
    cgarm_lit2(v, 1);
    gen("ldr\tr3,[r0]");
    gen("add\tr3,r3,r1");
    gen("str\tr3,[r0]");
}

void cgarm_dec1pi(int v) {
    cgarm_lit2(v, 1);
    gen("ldr\tr3,[r0]");
    gen("sub\tr3,r3,r1");
    gen("str\tr3,[r0]");
}

void cgarm_inc2pi(int v) {
    cgarm_lit2(v, 1);
    gen("ldr\tr3,[r2]");
    gen("add\tr3,r3,r1");
    gen("str\tr3,[r2]");
}

void cgarm_dec2pi(int v) {
    cgarm_lit2(v, 1);
    gen("ldr\tr3,[r2]");
    gen("sub\tr3,r3,r1");
    gen("str\tr3,[r2]");
}

void cgarm_incpl(int a, int v) {
    cgarm_lit2(v, 2);
    cgarm_locladdr(a, 1);
    gen("ldr\tr3,[r1]");
    gen("add\tr3,r3,r2");
    gen("str\tr3,[r1]");
}

void cgarm_decpl(int a, int v) {
    cgarm_lit2(v, 2);
    cgarm_locladdr(a, 1);
    gen("ldr\tr3,[r1]");
    gen("sub\tr3,r3,r2");
    gen("str\tr3,[r1]");
}

void cgarm_incps(int a, int v) {
    cgarm_lit2(v, 2);
    cgarm_stataddr(a, 1);
    gen("ldr\tr3,[r1]");
    gen("add\tr3,r3,r2");
    gen("str\tr3,[r1]");
}

void cgarm_decps(int a, int v) {
    cgarm_lit2(v, 2);
    cgarm_stataddr(a, 1);
    gen("ldr\tr3,[r1]");
    gen("sub\tr3,r3,r2");
    gen("str\tr3,[r1]");
}

void cgarm_incpg(char *s, int v) {
    cgarm_lit2(v, 2);
    cgarm_globaddr(s, 1);
    gen("ldr\tr3,[r1]");
    gen("add\tr3,r3,r2");
    gen("str\tr3,[r1]");
}

void cgarm_decpg(char *s, int v) {
    cgarm_lit2(v, 2);
    cgarm_globaddr(s, 1);
    gen("ldr\tr3,[r1]");
    gen("sub\tr3,r3,r2");
    gen("str\tr3,[r1]");
}

void cgarm_inc1iw(void) {
    gen("ldr\tr1,[r0]");
    gen("add\tr1,r1,#1");
    gen("str\tr1,[r0]");
}

void cgarm_dec1iw(void) {
    gen("ldr\tr1,[r0]");
    gen("sub\tr1,r1,#1");
    gen("str\tr1,[r0]");
}

void cgarm_inc2iw(void) {
    gen("ldr\tr1,[r2]");
    gen("add\tr1,r1,#1");
    gen("str\tr1,[r2]");
}

void cgarm_dec2iw(void) {
    gen("ldr\tr1,[r2]");
    gen("sub\tr1,r1,#1");
    gen("str\tr1,[r2]");
}

void cgarm_inclw(int a) {
    cgarm_locladdr(a, 1);
    gen("ldr\tr2,[r1]");
    gen("add\tr2,r2,#1");
    gen("str\tr2,[r1]");
}

void cgarm_declw(int a) {
    cgarm_locladdr(a, 1);
    gen("ldr\tr2,[r1]");
    gen("sub\tr2,r2,#1");
    gen("str\tr2,[r1]");
}

void cgarm_incsw(int a) {
    cgarm_stataddr(a, 1);
    gen("ldr\tr2,[r1]");
    gen("add\tr2,r2,#1");
    gen("str\tr2,[r1]");
}

void cgarm_decsw(int a) {
    cgarm_stataddr(a, 1);
    gen("ldr\tr2,[r1]");
    gen("sub\tr2,r2,#1");
    gen("str\tr2,[r1]");
}

void cgarm_incgw(char *s) {
    cgarm_globaddr(s, 1);
    gen("ldr\tr2,[r1]");
    gen("add\tr2,r2,#1");
    gen("str\tr2,[r1]");
}

void cgarm_decgw(char *s) {
    cgarm_globaddr(s, 1);
    gen("ldr\tr2,[r1]");
    gen("sub\tr2,r2,#1");
    gen("str\tr2,[r1]");
}

void cgarm_inc1ib(void) {
    gen("ldrb\tr1,[r0]");
    gen("add\tr1,r1,#1");
    gen("strb\tr1,[r0]");
}

void cgarm_dec1ib(void) {
    gen("ldrb\tr1,[r0]");
    gen("sub\tr1,r1,#1");
    gen("strb\tr1,[r0]");
}

void cgarm_inc2ib(void) {
    gen("ldrb\tr1,[r2]");
    gen("add\tr1,r1,#1");
    gen("strb\tr1,[r2]");
}

void cgarm_dec2ib(void) {
    gen("ldrb\tr1,[r2]");
    gen("sub\tr1,r1,#1");
    gen("strb\tr1,[r2]");
}

void cgarm_inclb(int a) {
    cgarm_locladdr(a, 1);
    gen("ldrb\tr2,[r1]");
    gen("add\tr2,r2,#1");
    gen("strb\tr2,[r1]");
}

void cgarm_declb(int a) {
    cgarm_locladdr(a, 1);
    gen("ldrb\tr2,[r1]");
    gen("sub\tr2,r2,#1");
    gen("strb\tr2,[r1]");
}

void cgarm_incsb(int a) {
    cgarm_stataddr(a, 1);
    gen("ldrb\tr2,[r1]");
    gen("add\tr2,r2,#1");
    gen("strb\tr2,[r1]");
}

void cgarm_decsb(int a) {
    cgarm_stataddr(a, 1);
    gen("ldrb\tr2,[r1]");
    gen("sub\tr2,r2,#1");
    gen("strb\tr2,[r1]");
}

void cgarm_incgb(char *s) {
    cgarm_globaddr(s, 1);
    gen("ldrb\tr2,[r1]");
    gen("add\tr2,r2,#1");
    gen("strb\tr2,[r1]");
}

void cgarm_decgb(char *s) {
    cgarm_globaddr(s, 1);
    gen("ldrb\tr2,[r1]");
    gen("sub\tr2,r2,#1");
    gen("strb\tr2,[r1]");
}

/*
 * ============================================================================
 * SECTION: Unconditional Branch Operations
 * ============================================================================
 */

void cgarm_br(char *how, int n) {
    int lab;
    
    lab = label();
    gen("cmp\tr0,#0");
    lgen("%s\t%c%d", how, lab);
    lgen("%s\t%c%d", "b", n);
    genlab(lab);
}

void cgarm_brtrue(int n) {
    cgarm_br("beq", n);
}

void cgarm_brfalse(int n) {
    cgarm_br("bne", n);
}

void cgarm_jump(int n) {
    lgen("%s\t%c%d", "b", n);
}

/*
 * ============================================================================
 * SECTION: Switch Statement Support
 * ============================================================================
 */

void cgarm_ldswtch(int n) {
    cgarm_stataddr(n, 1);
}

void cgarm_calswtch(void) {
    gen("b\tswitch");
}

void cgarm_case(int v, int l) {
    lgen2(".long\t%d,%c%d", v, l);
}

/*
 * ============================================================================
 * SECTION: Store Operations
 * ============================================================================
 */

void cgarm_storib(void) {
    gen("strb\tr0,[r2]");
}

void cgarm_storiw(void) {
    gen("str\tr0,[r2]");
}

void cgarm_storlb(int n) {
    cgarm_locladdr(n, 1);
    cgarm_storb();
}

void cgarm_storlw(int n) {
    cgarm_locladdr(n, 1);
    cgarm_storw();
}

void cgarm_storsb(int n) {
    cgarm_stataddr(n, 1);
    cgarm_storb();
}

void cgarm_storsw(int n) {
    cgarm_stataddr(n, 1);
    cgarm_storw();
}

void cgarm_storgb(char *s) {
    cgarm_globaddr(s, 1);
    cgarm_storb();
}

void cgarm_storgw(char *s) {
    cgarm_globaddr(s, 1);
    cgarm_storw();
}

/*
 * ============================================================================
 * SECTION: Function Call Operations
 * ============================================================================
 */

void cgarm_initlw(int v, int a) {
    cgarm_lit(v);
    cgarm_locladdr(a, 1);
    cgarm_storw();
}

void cgarm_call(char *s) {
    sgen("%s\t%s", "bl", s);
}

void cgarm_calr(void) {
    gen("blx\tr0");
}

void cgarm_stack(int n) {
    cgarm_lit2(n, 1);
    gen("add\tsp,sp,r1");
}

void cgarm_entry(void) {
    gen("push\t{r11,lr}");
    gen("mov\tr11,sp");
}

void cgarm_exit(void) {
    gen("pop\t{r11,pc}");
}

/*
 * ============================================================================
 * SECTION: Data Definition
 * ============================================================================
 */

void cgarm_defb(int v) {
    ngen("%s\t%d", ".byte", v);
}

void cgarm_defw(int v) {
    ngen("%s\t%d", ".long", v);
}

void cgarm_defp(int v) {
    ngen("%s\t%d", ".long", v);
}

void cgarm_defl(int v) {
    lgen("%s\t%c%d", ".long", v);
}

void cgarm_defc(int c) {
    ngen("%s\t'%c'", ".byte", c);
}

void cgarm_gbss(char *s, int z) {
    ngen(".comm\t%s,%d", s, z);
}

void cgarm_lbss(char *s, int z) {
    ngen(".lcomm\t%s,%d", s, z);
}

/*
 * ============================================================================
 * SECTION: Synthesizer Support
 * ============================================================================
 */

int cgarm_load2(void) {
    int   n, q;
    char *s;

    n = Q_val;
    s = gsym(Q_name);
    
    switch (Q_type) {
    case addr_auto:
        cgarm_locladdr(n, 1);
        break;
    case addr_static:
        cgarm_stataddr(n, 1);
        break;
    case addr_globl:
        cgarm_globaddr(s, 1);
        break;
    case addr_label:
        cgarm_stataddr(n, 1);
        break;
    case literal:
        cgarm_lit2(n, 1);
        break;
    case auto_byte:
        cgarm_locladdr(n, 1);
        cgarm_ind2b();
        break;
    case auto_word:
        cgarm_locladdr(n, 1);
        cgarm_ind2w();
        break;
    case static_byte:
        cgarm_stataddr(n, 1);
        cgarm_ind2b();
        break;
    case static_word:
        cgarm_stataddr(n, 1);
        cgarm_ind2w();
        break;
    case globl_byte:
        cgarm_globaddr(s, 1);
        cgarm_ind2b();
        break;
    case globl_word:
        cgarm_globaddr(s, 1);
        cgarm_ind2w();
        break;
    case empty:
        cgarm_pop2();
        break;
    default:
        fatal("internal: bad type in cgarm_load2()");
    }
    
    q = Q_type;
    Q_type = empty;
    return empty == q;
}

