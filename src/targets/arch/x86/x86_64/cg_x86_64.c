/*
 * SubC Compiler - x86-64 / AMD64 (64-bit) Target
 *
 * This file implements the code generator for the x86-64 architecture.
 * It generates GNU Assembler (GAS) syntax assembly code using AT&T notation.
 *
 * Architecture characteristics:
 *   - 64-bit registers and pointers
 *   - Little-endian byte order
 *   - Stack grows downward (toward lower addresses)
 *   - System V AMD64 ABI calling convention
 *
 * Register usage:
 *   %rax - Primary accumulator, return value
 *   %rcx - Auxiliary register, shift count
 *   %rdx - Used for division, multiplication overflow
 *   %rbp - Frame pointer (base pointer)
 *   %rsp - Stack pointer
 *
 * Differences from 386:
 *   - 64-bit registers (%rax vs %eax)
 *   - 64-bit instruction suffixes (movq vs movl)
 *   - Pointer size is 8 bytes instead of 4
 *   - Scale factor for pointer arithmetic is 8 instead of 4
 *
 * Copyright (c) 2011-2025 - Public Domain (CC0)
 */

#include "defs.h"
#include "data.h"
#include "decl.h"
#include "cgtarget.h"
#include "cg_x86_64_funcs.h"

/*
 * ============================================================================
 * SECTION: Helper Functions
 * ============================================================================
 */

/*
 * cgx64_synth - Synthesize operation with queued operand
 *
 * @op: The operation mnemonic (e.g., "addq", "subq")
 *
 * This function handles the "synthesizing" code generator pattern.
 * Uses 64-bit (quad) operations throughout.
 */
void cgx64_synth(char *op) {
    int   n;
    char *s;

    n = Q_val;
    s = gsym(Q_name);
    
    switch (Q_type) {
    case addr_auto:
        ngen("%s\t%d(%%rbp),%%rcx", "leaq", n);
        sgen("%s\t%s,%%rax", op, "%rcx");
        break;
        
    case addr_static:
        lgen("%s\t$%c%d,%%rax", op, n);
        break;
        
    case addr_globl:
        sgen("%s\t$%s,%%rax", op, s);
        break;
        
    case addr_label:
        lgen("%s\t$%c%d,%%rax", op, n);
        break;
        
    case literal:
        ngen("%s\t$%d,%%rax", op, n);
        break;
        
    case auto_word:
        ngen("%s\t%d(%%rbp),%%rax", op, n);
        break;
        
    case static_word:
        lgen("%s\t%c%d,%%rax", op, n);
        break;
        
    case globl_word:
        sgen("%s\t%s,%%rax", op, s);
        break;
        
    case auto_byte:
    case static_byte:
    case globl_byte:
        cgx64_load2();
        ngen("%s\t%%rcx,%%rax", op, 0);
        break;
        
    case empty:
        cgx64_pop2();
        sgen("%s\t%s,%%rax", op, "%rcx");
        break;
        
    default:
        fatal("internal: bad type in cgx64_synth()");
    }
    
    Q_type = empty;
}

/*
 * cgx64_cmp - Generate comparison and set result
 *
 * @inst: The conditional jump instruction for FALSE case
 */
void cgx64_cmp(char *inst) {
    int lab;
    
    lab = label();
    gen("xorq\t%rdx,%rdx");
    
    if (empty == Q_type) {
        cgx64_pop2();
        gen("cmpq\t%rax,%rcx");
    }
    else {
        cgx64_synth("cmpq");
    }
    
    lgen("%s\t%c%d", inst, lab);
    gen("incq\t%rdx");
    genlab(lab);
    gen("movq\t%rdx,%rax");
}

/*
 * cgx64_brcond - Generate conditional branch
 */
void cgx64_brcond(char *i, int n) {
    int lab;
    
    lab = label();
    
    if (empty == Q_type) {
        cgx64_pop2();
        gen("cmpq\t%rax,%rcx");
    }
    else {
        cgx64_synth("cmpq");
    }
    
    lgen("%s\t%c%d", i, lab);
    lgen("%s\t%c%d", "jmp", n);
    genlab(lab);
}

/*
 * cgx64_br - Generate branch on accumulator value
 */
void cgx64_br(char *how, int n) {
    int lab;
    
    lab = label();
    gen("orq\t%rax,%rax");
    lgen("%s\t%c%d", how, lab);
    lgen("%s\t%c%d", "jmp", n);
    genlab(lab);
}

/*
 * ============================================================================
 * SECTION: Section Control Functions
 * ============================================================================
 */

void cgx64_data(void) {
    gen(".data");
}

void cgx64_text(void) {
    gen(".text");
}

void cgx64_prelude(void) {
    /* No prelude needed for GAS */
}

void cgx64_postlude(void) {
    /* No postlude needed for GAS */
}

void cgx64_public(char *s) {
    ngen(".globl\t%s", s, 0);
}

void cgx64_name(char *s) {
    sgen("%s:", s, "");
}

void cgx64_funcname(char *s) {
    sgen("%s:", s, "");
}

void cgx64_align(void) {
    /* Alignment handled by assembler */
}

/*
 * ============================================================================
 * SECTION: Literal and Clear Functions
 * ============================================================================
 */

void cgx64_lit(int v) {
    ngen("%s\t$%d,%%rax", "movq", v);
}

void cgx64_clear(void) {
    gen("xorq\t%rax,%rax");
}

void cgx64_clear2(void) {
    gen("xorq\t%rcx,%rcx");
}

/*
 * ============================================================================
 * SECTION: Load Operations
 * ============================================================================
 */

void cgx64_ldgb(char *s) {
    sgen("%s\t%s,%%al", "movb", s);
}

void cgx64_ldgw(char *s) {
    sgen("%s\t%s,%%rax", "movq", s);
}

void cgx64_ldlb(int n) {
    ngen("%s\t%d(%%rbp),%%al", "movb", n);
}

void cgx64_ldlw(int n) {
    ngen("%s\t%d(%%rbp),%%rax", "movq", n);
}

void cgx64_ldsb(int n) {
    lgen("%s\t%c%d,%%al", "movb", n);
}

void cgx64_ldsw(int n) {
    lgen("%s\t%c%d,%%rax", "movq", n);
}

void cgx64_ldla(int n) {
    ngen("%s\t%d(%%rbp),%%rax", "leaq", n);
}

void cgx64_ldsa(int n) {
    lgen("%s\t$%c%d,%%rax", "movq", n);
}

void cgx64_ldga(char *s) {
    sgen("%s\t$%s,%%rax", "movq", s);
}

void cgx64_indb(void) {
    gen("movq\t%rax,%rdx");
    cgx64_clear();
    gen("movb\t(%rdx),%al");
}

void cgx64_indw(void) {
    gen("movq\t(%rax),%rax");
}

void cgx64_ldlab(int id) {
    lgen("%s\t$%c%d,%%rax", "movq", id);
}

/*
 * ============================================================================
 * SECTION: Stack Operations
 * ============================================================================
 */

void cgx64_push(void) {
    gen("pushq\t%rax");
}

void cgx64_pushlit(int n) {
    ngen("%s\t$%d", "pushq", n);
}

void cgx64_pop2(void) {
    gen("popq\t%rcx");
}

void cgx64_swap(void) {
    gen("xchgq\t%rax,%rcx");
}

void cgx64_popptr(void) {
    gen("popq\t%rdx");
}

/*
 * ============================================================================
 * SECTION: Arithmetic Operations
 * ============================================================================
 */

void cgx64_add(void) {
    gen("addq\t%rcx,%rax");
}

void cgx64_sub(void) {
    gen("subq\t%rcx,%rax");
}

void cgx64_mul(void) {
    gen("imulq\t%rcx,%rax");
}

void cgx64_div(void) {
    gen("cqo");              /* Sign-extend RAX into RDX:RAX */
    gen("idivq\t%rcx");      /* Divide RDX:RAX by RCX */
}

void cgx64_mod(void) {
    cgx64_div();
    gen("movq\t%rdx,%rax");  /* Remainder is in RDX */
}

void cgx64_neg(void) {
    gen("negq\t%rax");
}

/*
 * ============================================================================
 * SECTION: Bitwise Operations
 * ============================================================================
 */

void cgx64_and(void) {
    cgx64_synth("andq");
}

void cgx64_ior(void) {
    cgx64_synth("orq");
}

void cgx64_xor(void) {
    cgx64_synth("xorq");
}

void cgx64_not(void) {
    gen("notq\t%rax");
}

void cgx64_shl(void) {
    gen("shlq\t%cl,%rax");
}

void cgx64_shr(void) {
    gen("sarq\t%cl,%rax");
}

/*
 * ============================================================================
 * SECTION: Comparison Operations
 * ============================================================================
 */

void cgx64_eq(void)  { cgx64_cmp("jne"); }
void cgx64_ne(void)  { cgx64_cmp("je");  }
void cgx64_lt(void)  { cgx64_cmp("jge"); }
void cgx64_gt(void)  { cgx64_cmp("jle"); }
void cgx64_le(void)  { cgx64_cmp("jg");  }
void cgx64_ge(void)  { cgx64_cmp("jl");  }
void cgx64_ult(void) { cgx64_cmp("jae"); }
void cgx64_ugt(void) { cgx64_cmp("jbe"); }
void cgx64_ule(void) { cgx64_cmp("ja");  }
void cgx64_uge(void) { cgx64_cmp("jb");  }

/*
 * ============================================================================
 * SECTION: Conditional Branch Operations
 * ============================================================================
 */

void cgx64_breq(int n)  { cgx64_brcond("je",  n); }
void cgx64_brne(int n)  { cgx64_brcond("jne", n); }
void cgx64_brlt(int n)  { cgx64_brcond("jl",  n); }
void cgx64_brgt(int n)  { cgx64_brcond("jg",  n); }
void cgx64_brle(int n)  { cgx64_brcond("jle", n); }
void cgx64_brge(int n)  { cgx64_brcond("jge", n); }
void cgx64_brult(int n) { cgx64_brcond("jb",  n); }
void cgx64_brugt(int n) { cgx64_brcond("ja",  n); }
void cgx64_brule(int n) { cgx64_brcond("jbe", n); }
void cgx64_bruge(int n) { cgx64_brcond("jae", n); }

/*
 * ============================================================================
 * SECTION: Boolean and Logic Operations
 * ============================================================================
 */

void cgx64_lognot(void) {
    gen("negq\t%rax");
    gen("sbbq\t%rax,%rax");
    gen("incq\t%rax");
}

void cgx64_bool(void) {
    gen("negq\t%rax");
    gen("sbbq\t%rax,%rax");
    gen("negq\t%rax");
}

/*
 * ============================================================================
 * SECTION: Pointer Scaling Operations
 * ============================================================================
 *
 * On x86-64, sizeof(int) = 8, so we shift by 3 bits.
 */

void cgx64_scale(void) {
    gen("shlq\t$3,%rax");
}

void cgx64_scale2(void) {
    gen("shlq\t$3,%rcx");
}

void cgx64_unscale(void) {
    gen("shrq\t$3,%rax");
}

void cgx64_scaleby(int v) {
    ngen("%s\t$%d,%%rcx", "movq", v);
    gen("mulq\t%rcx");
}

void cgx64_scale2by(int v) {
    gen("pushq\t%rax");
    ngen("%s\t$%d,%%rax", "movq", v);
    gen("mulq\t%rcx");
    gen("movq\t%rax,%rcx");
    gen("popq\t%rax");
}

void cgx64_unscaleby(int v) {
    ngen("%s\t$%d,%%rcx", "movq", v);
    gen("xorq\t%rdx,%rdx");
    gen("divq\t%rcx");
}

/*
 * ============================================================================
 * SECTION: Increment/Decrement Operations
 * ============================================================================
 */

void cgx64_ldinc(void) {
    gen("movq\t%rax,%rdx");
}

void cgx64_inc1pi(int v) {
    ngen("%s\t$%d,(%%rax)", "addq", v);
}

void cgx64_dec1pi(int v) {
    ngen("%s\t$%d,(%%rax)", "subq", v);
}

void cgx64_inc2pi(int v) {
    ngen("%s\t$%d,(%%rdx)", "addq", v);
}

void cgx64_dec2pi(int v) {
    ngen("%s\t$%d,(%%rdx)", "subq", v);
}

void cgx64_incpl(int a, int v) {
    ngen2("%s\t$%d,%d(%%rbp)", "addq", v, a);
}

void cgx64_decpl(int a, int v) {
    ngen2("%s\t$%d,%d(%%rbp)", "subq", v, a);
}

void cgx64_inclw(int a) {
    ngen("%s\t%d(%%rbp)", "incq", a);
}

void cgx64_declw(int a) {
    ngen("%s\t%d(%%rbp)", "decq", a);
}

void cgx64_inclb(int a) {
    ngen("%s\t%d(%%rbp)", "incb", a);
}

void cgx64_declb(int a) {
    ngen("%s\t%d(%%rbp)", "decb", a);
}

void cgx64_incps(int a, int v) {
    lgen2("addq\t$%d,%c%d", v, a);
}

void cgx64_decps(int a, int v) {
    lgen2("subq\t$%d,%c%d", v, a);
}

void cgx64_incsw(int a) {
    lgen("%s\t%c%d", "incq", a);
}

void cgx64_decsw(int a) {
    lgen("%s\t%c%d", "decq", a);
}

void cgx64_incsb(int a) {
    lgen("%s\t%c%d", "incb", a);
}

void cgx64_decsb(int a) {
    lgen("%s\t%c%d", "decb", a);
}

void cgx64_incpg(char *s, int v) {
    sgen2("%s\t$%d,%s", "addq", v, s);
}

void cgx64_decpg(char *s, int v) {
    sgen2("%s\t$%d,%s", "subq", v, s);
}

void cgx64_incgw(char *s) {
    sgen("%s\t%s", "incq", s);
}

void cgx64_decgw(char *s) {
    sgen("%s\t%s", "decq", s);
}

void cgx64_incgb(char *s) {
    sgen("%s\t%s", "incb", s);
}

void cgx64_decgb(char *s) {
    sgen("%s\t%s", "decb", s);
}

void cgx64_inc1iw(void) {
    ngen("%s\t(%%rax)", "incq", 0);
}

void cgx64_dec1iw(void) {
    ngen("%s\t(%%rax)", "decq", 0);
}

void cgx64_inc2iw(void) {
    ngen("%s\t(%%rdx)", "incq", 0);
}

void cgx64_dec2iw(void) {
    ngen("%s\t(%%rdx)", "decq", 0);
}

void cgx64_inc1ib(void) {
    ngen("%s\t(%%rax)", "incb", 0);
}

void cgx64_dec1ib(void) {
    ngen("%s\t(%%rax)", "decb", 0);
}

void cgx64_inc2ib(void) {
    ngen("%s\t(%%rdx)", "incb", 0);
}

void cgx64_dec2ib(void) {
    ngen("%s\t(%%rdx)", "decb", 0);
}

/*
 * ============================================================================
 * SECTION: Unconditional Branch Operations
 * ============================================================================
 */

void cgx64_brtrue(int n) {
    cgx64_br("jz", n);
}

void cgx64_brfalse(int n) {
    cgx64_br("jnz", n);
}

void cgx64_jump(int n) {
    lgen("%s\t%c%d", "jmp", n);
}

/*
 * ============================================================================
 * SECTION: Switch Statement Support
 * ============================================================================
 */

void cgx64_ldswtch(int n) {
    lgen("%s\t$%c%d,%%rdx", "movq", n);
}

void cgx64_calswtch(void) {
    gen("jmp\tswitch");
}

void cgx64_case(int v, int l) {
    lgen2(".quad\t%d,%c%d", v, l);
}

/*
 * ============================================================================
 * SECTION: Store Operations
 * ============================================================================
 */

void cgx64_storib(void) {
    ngen("%s\t%%al,(%%rdx)", "movb", 0);
}

void cgx64_storiw(void) {
    ngen("%s\t%%rax,(%%rdx)", "movq", 0);
}

void cgx64_storlb(int n) {
    ngen("%s\t%%al,%d(%%rbp)", "movb", n);
}

void cgx64_storlw(int n) {
    ngen("%s\t%%rax,%d(%%rbp)", "movq", n);
}

void cgx64_storsb(int n) {
    lgen("%s\t%%al,%c%d", "movb", n);
}

void cgx64_storsw(int n) {
    lgen("%s\t%%rax,%c%d", "movq", n);
}

void cgx64_storgb(char *s) {
    sgen("%s\t%%al,%s", "movb", s);
}

void cgx64_storgw(char *s) {
    sgen("%s\t%%rax,%s", "movq", s);
}

/*
 * ============================================================================
 * SECTION: Function Call Operations
 * ============================================================================
 */

void cgx64_initlw(int v, int a) {
    ngen2("%s\t$%d,%d(%%rbp)", "movq", v, a);
}

void cgx64_call(char *s) {
    sgen("%s\t%s", "call", s);
}

void cgx64_calr(void) {
    gen("call\t*%rax");
}

void cgx64_stack(int n) {
    ngen("%s\t$%d,%%rsp", "addq", n);
}

void cgx64_entry(int lsize) {
    (void)lsize;  /* Not used for x86-64 */
    gen("pushq\t%rbp");
    gen("movq\t%rsp,%rbp");
}

void cgx64_exit(void) {
    gen("popq\t%rbp");
    gen("ret");
}

/*
 * ============================================================================
 * SECTION: Data Definition
 * ============================================================================
 */

void cgx64_defb(int v) {
    ngen("%s\t%d", ".byte", v);
}

void cgx64_defw(int v) {
    ngen("%s\t%d", ".quad", v);
}

void cgx64_defp(int v) {
    ngen("%s\t%d", ".quad", v);
}

void cgx64_defl(int v) {
    lgen("%s\t%c%d", ".quad", v);
}

void cgx64_defc(int c) {
    ngen("%s\t'%c'", ".byte", c);
}

void cgx64_gbss(char *s, int z) {
    ngen(".comm\t%s,%d", s, z);
}

void cgx64_lbss(char *s, int z) {
    ngen(".lcomm\t%s,%d", s, z);
}

/*
 * ============================================================================
 * SECTION: Synthesizer Support
 * ============================================================================
 */

int cgx64_load2(void) {
    int   n, q;
    char *s, *op, *opb;

    op  = "movq";
    opb = "movb";
    n   = Q_val;
    s   = gsym(Q_name);
    
    switch (Q_type) {
    case addr_auto:
        ngen("%s\t%d(%%rbp),%%rcx", "leaq", n);
        break;
        
    case addr_static:
        lgen("%s\t$%c%d,%%rcx", op, n);
        break;
        
    case addr_globl:
        sgen("%s\t$%s,%%rcx", op, s);
        break;
        
    case addr_label:
        lgen("%s\t$%c%d,%%rcx", op, n);
        break;
        
    case literal:
        ngen("%s\t$%d,%%rcx", op, n);
        break;
        
    case auto_byte:
        cgx64_clear2();
        ngen("%s\t%d(%%rbp),%%cl", opb, n);
        break;
        
    case auto_word:
        ngen("%s\t%d(%%rbp),%%rcx", op, n);
        break;
        
    case static_byte:
        cgx64_clear2();
        lgen("%s\t%c%d,%%cl", opb, n);
        break;
        
    case static_word:
        lgen("%s\t%c%d,%%rcx", op, n);
        break;
        
    case globl_byte:
        cgx64_clear2();
        sgen("%s\t%s,%%cl", opb, s);
        break;
        
    case globl_word:
        sgen("%s\t%s,%%rcx", op, s);
        break;
        
    case empty:
        cgx64_pop2();
        break;
        
    default:
        fatal("internal: bad type in cgx64_load2()");
    }
    
    q = Q_type;
    Q_type = empty;
    return empty == q;
}

