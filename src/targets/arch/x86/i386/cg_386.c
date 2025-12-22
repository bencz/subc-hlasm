/*
 * SubC Compiler - Intel 386 (32-bit) Target
 *
 * This file implements the code generator for the Intel 386 architecture.
 * It generates GNU Assembler (GAS) syntax assembly code using AT&T notation.
 *
 * Architecture characteristics:
 *   - 32-bit registers and pointers
 *   - Little-endian byte order
 *   - Stack grows downward (toward lower addresses)
 *   - C calling convention (cdecl): caller cleans stack
 *
 * Register usage:
 *   %eax - Primary accumulator, return value
 *   %ecx - Auxiliary register, shift count
 *   %edx - Used for division, multiplication overflow
 *   %ebp - Frame pointer (base pointer)
 *   %esp - Stack pointer
 *
 * Copyright (c) 2011-2025 - Public Domain (CC0)
 */

#include "defs.h"
#include "data.h"
#include "decl.h"
#include "cgtarget.h"
#include "cg_386.h"

/*
 * ============================================================================
 * SECTION: Helper Functions
 * ============================================================================
 */

/*
 * cg386_synth - Synthesize operation with queued operand
 *
 * @op: The operation mnemonic (e.g., "addl", "subl")
 *
 * This function handles the "synthesizing" code generator pattern.
 * Instead of always loading values into registers, it can operate
 * directly on memory operands when possible, reducing code size.
 */
void cg386_synth(char *op) {
    int   n;
    char *s;

    n = Q_val;
    s = gsym(Q_name);
    
    switch (Q_type) {
    case addr_auto:
        /* Local variable address: use LEA then operate */
        ngen("%s\t%d(%%ebp),%%ecx", "leal", n);
        sgen("%s\t%s,%%eax", op, "%ecx");
        break;
        
    case addr_static:
        /* Static variable address */
        lgen("%s\t$%c%d,%%eax", op, n);
        break;
        
    case addr_globl:
        /* Global variable address */
        sgen("%s\t$%s,%%eax", op, s);
        break;
        
    case addr_label:
        /* Label address */
        lgen("%s\t$%c%d,%%eax", op, n);
        break;
        
    case literal:
        /* Immediate value */
        ngen("%s\t$%d,%%eax", op, n);
        break;
        
    case auto_word:
        /* Local word variable */
        ngen("%s\t%d(%%ebp),%%eax", op, n);
        break;
        
    case static_word:
        /* Static word variable */
        lgen("%s\t%c%d,%%eax", op, n);
        break;
        
    case globl_word:
        /* Global word variable */
        sgen("%s\t%s,%%eax", op, s);
        break;
        
    case auto_byte:
    case static_byte:
    case globl_byte:
        /* Byte operands: must load first, then operate */
        cg386_load2();
        ngen("%s\t%%ecx,%%eax", op, 0);
        break;
        
    case empty:
        /* Value is on stack: pop and operate */
        cg386_pop2();
        sgen("%s\t%s,%%eax", op, "%ecx");
        break;
        
    default:
        fatal("internal: bad type in cg386_synth()");
    }
    
    Q_type = empty;
}

/*
 * cg386_cmp - Generate comparison and set result
 *
 * @inst: The conditional jump instruction for FALSE case
 *
 * Compares accumulator with operand and sets accumulator to 0 or 1.
 * Uses the inverse jump because we're setting 1 on the fall-through path.
 */
void cg386_cmp(char *inst) {
    int lab;
    
    lab = label();
    
    /* Clear result register */
    gen("xorl\t%edx,%edx");
    
    if (empty == Q_type) {
        /* Operand is on stack */
        cg386_pop2();
        gen("cmpl\t%eax,%ecx");
    }
    else {
        /* Use synthesized comparison */
        cg386_synth("cmpl");
    }
    
    /* Jump over increment if condition is FALSE */
    lgen("%s\t%c%d", inst, lab);
    
    /* Set result to 1 */
    gen("incl\t%edx");
    
    genlab(lab);
    
    /* Move result to accumulator */
    gen("movl\t%edx,%eax");
}

/*
 * cg386_brcond - Generate conditional branch
 *
 * @i: The conditional jump instruction
 * @n: Target label number
 *
 * Compares and branches to label n if condition is true.
 */
void cg386_brcond(char *i, int n) {
    int lab;
    
    lab = label();
    
    if (empty == Q_type) {
        cg386_pop2();
        gen("cmpl\t%eax,%ecx");
    }
    else {
        cg386_synth("cmpl");
    }
    
    /* Jump over the branch if condition is FALSE */
    lgen("%s\t%c%d", i, lab);
    
    /* Take the branch */
    lgen("%s\t%c%d", "jmp", n);
    
    genlab(lab);
}

/*
 * cg386_br - Generate branch on accumulator value
 *
 * @how: Jump instruction for zero case
 * @n: Target label number
 */
void cg386_br(char *how, int n) {
    int lab;
    
    lab = label();
    
    /* Test accumulator */
    gen("orl\t%eax,%eax");
    
    /* Jump over branch if condition met */
    lgen("%s\t%c%d", how, lab);
    
    /* Take the branch */
    lgen("%s\t%c%d", "jmp", n);
    
    genlab(lab);
}

/*
 * ============================================================================
 * SECTION: Section Control Functions
 * ============================================================================
 */

void cg386_data(void) {
    gen(".data");
}

void cg386_text(void) {
    gen(".text");
}

void cg386_prelude(void) {
    /* No prelude needed for GAS */
}

void cg386_postlude(void) {
    /* No postlude needed for GAS */
}

void cg386_public(char *s) {
    ngen(".globl\t%s", s, 0);
}

void cg386_name(char *s) {
    sgen("%s:", s, "");
}

void cg386_funcname(char *s) {
    sgen("%s:", s, "");
}

void cg386_align(void) {
    /* Alignment not typically needed for 386 */
}

/*
 * ============================================================================
 * SECTION: Literal and Clear Functions
 * ============================================================================
 */

void cg386_lit(int v) {
    ngen("%s\t$%d,%%eax", "movl", v);
}

void cg386_clear(void) {
    gen("xorl\t%eax,%eax");
}

void cg386_clear2(void) {
    gen("xorl\t%ecx,%ecx");
}

/*
 * ============================================================================
 * SECTION: Load Operations
 * ============================================================================
 */

void cg386_ldgb(char *s) {
    sgen("%s\t%s,%%al", "movb", s);
}

void cg386_ldgw(char *s) {
    sgen("%s\t%s,%%eax", "movl", s);
}

void cg386_ldlb(int n) {
    ngen("%s\t%d(%%ebp),%%al", "movb", n);
}

void cg386_ldlw(int n) {
    ngen("%s\t%d(%%ebp),%%eax", "movl", n);
}

void cg386_ldsb(int n) {
    lgen("%s\t%c%d,%%al", "movb", n);
}

void cg386_ldsw(int n) {
    lgen("%s\t%c%d,%%eax", "movl", n);
}

void cg386_ldla(int n) {
    ngen("%s\t%d(%%ebp),%%eax", "leal", n);
}

void cg386_ldsa(int n) {
    lgen("%s\t$%c%d,%%eax", "movl", n);
}

void cg386_ldga(char *s) {
    sgen("%s\t$%s,%%eax", "movl", s);
}

void cg386_indb(void) {
    gen("movl\t%eax,%edx");
    cg386_clear();
    gen("movb\t(%edx),%al");
}

void cg386_indw(void) {
    gen("movl\t(%eax),%eax");
}

void cg386_ldlab(int id) {
    lgen("%s\t$%c%d,%%eax", "movl", id);
}

/*
 * ============================================================================
 * SECTION: Stack Operations
 * ============================================================================
 */

void cg386_push(void) {
    gen("pushl\t%eax");
}

void cg386_pushlit(int n) {
    ngen("%s\t$%d", "pushl", n);
}

void cg386_pop2(void) {
    gen("popl\t%ecx");
}

void cg386_swap(void) {
    gen("xchgl\t%eax,%ecx");
}

void cg386_popptr(void) {
    gen("popl\t%edx");
}

/*
 * ============================================================================
 * SECTION: Arithmetic Operations
 * ============================================================================
 */

void cg386_add(void) {
    gen("addl\t%ecx,%eax");
}

void cg386_sub(void) {
    gen("subl\t%ecx,%eax");
}

void cg386_mul(void) {
    gen("imull\t%ecx,%eax");
}

void cg386_div(void) {
    gen("cdq");           /* Sign-extend EAX into EDX:EAX */
    gen("idivl\t%ecx");   /* Divide EDX:EAX by ECX */
}

void cg386_mod(void) {
    cg386_div();
    gen("movl\t%edx,%eax");  /* Remainder is in EDX */
}

void cg386_neg(void) {
    gen("negl\t%eax");
}

/*
 * ============================================================================
 * SECTION: Bitwise Operations
 * ============================================================================
 */

void cg386_and(void) {
    cg386_synth("andl");
}

void cg386_ior(void) {
    cg386_synth("orl");
}

void cg386_xor(void) {
    cg386_synth("xorl");
}

void cg386_not(void) {
    gen("notl\t%eax");
}

void cg386_shl(void) {
    gen("shll\t%cl,%eax");
}

void cg386_shr(void) {
    gen("sarl\t%cl,%eax");  /* Arithmetic shift (preserves sign) */
}

/*
 * ============================================================================
 * SECTION: Comparison Operations
 * ============================================================================
 */

/* Signed comparisons */
void cg386_eq(void)  { cg386_cmp("jne"); }
void cg386_ne(void)  { cg386_cmp("je");  }
void cg386_lt(void)  { cg386_cmp("jge"); }
void cg386_gt(void)  { cg386_cmp("jle"); }
void cg386_le(void)  { cg386_cmp("jg");  }
void cg386_ge(void)  { cg386_cmp("jl");  }

/* Unsigned comparisons */
void cg386_ult(void) { cg386_cmp("jae"); }
void cg386_ugt(void) { cg386_cmp("jbe"); }
void cg386_ule(void) { cg386_cmp("ja");  }
void cg386_uge(void) { cg386_cmp("jb");  }

/*
 * ============================================================================
 * SECTION: Conditional Branch Operations
 * ============================================================================
 */

/* Signed branches */
void cg386_breq(int n)  { cg386_brcond("je",  n); }
void cg386_brne(int n)  { cg386_brcond("jne", n); }
void cg386_brlt(int n)  { cg386_brcond("jl",  n); }
void cg386_brgt(int n)  { cg386_brcond("jg",  n); }
void cg386_brle(int n)  { cg386_brcond("jle", n); }
void cg386_brge(int n)  { cg386_brcond("jge", n); }

/* Unsigned branches */
void cg386_brult(int n) { cg386_brcond("jb",  n); }
void cg386_brugt(int n) { cg386_brcond("ja",  n); }
void cg386_brule(int n) { cg386_brcond("jbe", n); }
void cg386_bruge(int n) { cg386_brcond("jae", n); }

/*
 * ============================================================================
 * SECTION: Boolean and Logic Operations
 * ============================================================================
 */

void cg386_lognot(void) {
    gen("negl\t%eax");
    gen("sbbl\t%eax,%eax");
    gen("incl\t%eax");
}

void cg386_bool(void) {
    gen("negl\t%eax");
    gen("sbbl\t%eax,%eax");
    gen("negl\t%eax");
}

/*
 * ============================================================================
 * SECTION: Pointer Scaling Operations
 * ============================================================================
 *
 * These functions scale values for pointer arithmetic.
 * On 386, sizeof(int) = 4, so we shift by 2 bits.
 */

void cg386_scale(void) {
    gen("shll\t$2,%eax");
}

void cg386_scale2(void) {
    gen("shll\t$2,%ecx");
}

void cg386_unscale(void) {
    gen("shrl\t$2,%eax");
}

void cg386_scaleby(int v) {
    ngen("%s\t$%d,%%ecx", "movl", v);
    gen("mull\t%ecx");
}

void cg386_scale2by(int v) {
    gen("pushl\t%eax");
    ngen("%s\t$%d,%%eax", "movl", v);
    gen("mull\t%ecx");
    gen("movl\t%eax,%ecx");
    gen("popl\t%eax");
}

void cg386_unscaleby(int v) {
    ngen("%s\t$%d,%%ecx", "movl", v);
    gen("xorl\t%edx,%edx");
    gen("divl\t%ecx");
}

/*
 * ============================================================================
 * SECTION: Increment/Decrement Operations
 * ============================================================================
 */

/* --- Via pointer --- */

void cg386_ldinc(void) {
    gen("movl\t%eax,%edx");
}

void cg386_inc1pi(int v) {
    ngen("%s\t$%d,(%%eax)", "addl", v);
}

void cg386_dec1pi(int v) {
    ngen("%s\t$%d,(%%eax)", "subl", v);
}

void cg386_inc2pi(int v) {
    ngen("%s\t$%d,(%%edx)", "addl", v);
}

void cg386_dec2pi(int v) {
    ngen("%s\t$%d,(%%edx)", "subl", v);
}

/* --- Local variables --- */

void cg386_incpl(int a, int v) {
    ngen2("%s\t$%d,%d(%%ebp)", "addl", v, a);
}

void cg386_decpl(int a, int v) {
    ngen2("%s\t$%d,%d(%%ebp)", "subl", v, a);
}

void cg386_inclw(int a) {
    ngen("%s\t%d(%%ebp)", "incl", a);
}

void cg386_declw(int a) {
    ngen("%s\t%d(%%ebp)", "decl", a);
}

void cg386_inclb(int a) {
    ngen("%s\t%d(%%ebp)", "incb", a);
}

void cg386_declb(int a) {
    ngen("%s\t%d(%%ebp)", "decb", a);
}

/* --- Static variables --- */

void cg386_incps(int a, int v) {
    lgen2("addl\t$%d,%c%d", v, a);
}

void cg386_decps(int a, int v) {
    lgen2("subl\t$%d,%c%d", v, a);
}

void cg386_incsw(int a) {
    lgen("%s\t%c%d", "incl", a);
}

void cg386_decsw(int a) {
    lgen("%s\t%c%d", "decl", a);
}

void cg386_incsb(int a) {
    lgen("%s\t%c%d", "incb", a);
}

void cg386_decsb(int a) {
    lgen("%s\t%c%d", "decb", a);
}

/* --- Global variables --- */

void cg386_incpg(char *s, int v) {
    sgen2("%s\t$%d,%s", "addl", v, s);
}

void cg386_decpg(char *s, int v) {
    sgen2("%s\t$%d,%s", "subl", v, s);
}

void cg386_incgw(char *s) {
    sgen("%s\t%s", "incl", s);
}

void cg386_decgw(char *s) {
    sgen("%s\t%s", "decl", s);
}

void cg386_incgb(char *s) {
    sgen("%s\t%s", "incb", s);
}

void cg386_decgb(char *s) {
    sgen("%s\t%s", "decb", s);
}

/* --- Via indirect --- */

void cg386_inc1iw(void) {
    ngen("%s\t(%%eax)", "incl", 0);
}

void cg386_dec1iw(void) {
    ngen("%s\t(%%eax)", "decl", 0);
}

void cg386_inc2iw(void) {
    ngen("%s\t(%%edx)", "incl", 0);
}

void cg386_dec2iw(void) {
    ngen("%s\t(%%edx)", "decl", 0);
}

void cg386_inc1ib(void) {
    ngen("%s\t(%%eax)", "incb", 0);
}

void cg386_dec1ib(void) {
    ngen("%s\t(%%eax)", "decb", 0);
}

void cg386_inc2ib(void) {
    ngen("%s\t(%%edx)", "incb", 0);
}

void cg386_dec2ib(void) {
    ngen("%s\t(%%edx)", "decb", 0);
}

/*
 * ============================================================================
 * SECTION: Unconditional Branch Operations
 * ============================================================================
 */

void cg386_brtrue(int n) {
    cg386_br("jz", n);
}

void cg386_brfalse(int n) {
    cg386_br("jnz", n);
}

void cg386_jump(int n) {
    lgen("%s\t%c%d", "jmp", n);
}

/*
 * ============================================================================
 * SECTION: Switch Statement Support
 * ============================================================================
 */

void cg386_ldswtch(int n) {
    lgen("%s\t$%c%d,%%edx", "movl", n);
}

void cg386_calswtch(void) {
    gen("jmp\tswitch");
}

void cg386_case(int v, int l) {
    lgen2(".long\t%d,%c%d", v, l);
}

/*
 * ============================================================================
 * SECTION: Store Operations
 * ============================================================================
 */

void cg386_storib(void) {
    ngen("%s\t%%al,(%%edx)", "movb", 0);
}

void cg386_storiw(void) {
    ngen("%s\t%%eax,(%%edx)", "movl", 0);
}

void cg386_storlb(int n) {
    ngen("%s\t%%al,%d(%%ebp)", "movb", n);
}

void cg386_storlw(int n) {
    ngen("%s\t%%eax,%d(%%ebp)", "movl", n);
}

void cg386_storsb(int n) {
    lgen("%s\t%%al,%c%d", "movb", n);
}

void cg386_storsw(int n) {
    lgen("%s\t%%eax,%c%d", "movl", n);
}

void cg386_storgb(char *s) {
    sgen("%s\t%%al,%s", "movb", s);
}

void cg386_storgw(char *s) {
    sgen("%s\t%%eax,%s", "movl", s);
}

/*
 * ============================================================================
 * SECTION: Function Call Operations
 * ============================================================================
 */

void cg386_initlw(int v, int a) {
    ngen2("%s\t$%d,%d(%%ebp)", "movl", v, a);
}

void cg386_call(char *s) {
    sgen("%s\t%s", "call", s);
}

void cg386_calr(void) {
    gen("call\t*%eax");
}

void cg386_stack(int n) {
    ngen("%s\t$%d,%%esp", "addl", n);
}

void cg386_entry(int lsize, int nparams) {
    (void)lsize;  /* Not used for x86 */
    (void)nparams;  /* Not used for x86 */
    gen("pushl\t%ebp");
    gen("movl\t%esp,%ebp");
}

void cg386_exit(void) {
    gen("popl\t%ebp");
    gen("ret");
}

/*
 * ============================================================================
 * SECTION: Data Definition
 * ============================================================================
 */

void cg386_defb(int v) {
    ngen("%s\t%d", ".byte", v);
}

void cg386_defw(int v) {
    ngen("%s\t%d", ".long", v);
}

void cg386_defp(int v) {
    ngen("%s\t%d", ".long", v);
}

void cg386_defl(int v) {
    lgen("%s\t%c%d", ".long", v);
}

void cg386_defc(int c) {
    ngen("%s\t'%c'", ".byte", c);
}

void cg386_gbss(char *s, int z) {
    ngen(".comm\t%s,%d", s, z);
}

void cg386_lbss(char *s, int z) {
    ngen(".lcomm\t%s,%d", s, z);
}

/*
 * ============================================================================
 * SECTION: Synthesizer Support
 * ============================================================================
 */

/*
 * cg386_load2 - Load queued value into auxiliary register
 *
 * Returns: 1 if queue was empty (value already in register), 0 otherwise
 *
 * This function loads the queued operand into %ecx, handling all
 * the different addressing modes.
 */
int cg386_load2(void) {
    int   n, q;
    char *s, *op, *opb;

    op  = "movl";
    opb = "movb";
    n   = Q_val;
    s   = gsym(Q_name);
    
    switch (Q_type) {
    case addr_auto:
        ngen("%s\t%d(%%ebp),%%ecx", "leal", n);
        break;
        
    case addr_static:
        lgen("%s\t$%c%d,%%ecx", op, n);
        break;
        
    case addr_globl:
        sgen("%s\t$%s,%%ecx", op, s);
        break;
        
    case addr_label:
        lgen("%s\t$%c%d,%%ecx", op, n);
        break;
        
    case literal:
        ngen("%s\t$%d,%%ecx", op, n);
        break;
        
    case auto_byte:
        cg386_clear2();
        ngen("%s\t%d(%%ebp),%%cl", opb, n);
        break;
        
    case auto_word:
        ngen("%s\t%d(%%ebp),%%ecx", op, n);
        break;
        
    case static_byte:
        cg386_clear2();
        lgen("%s\t%c%d,%%cl", opb, n);
        break;
        
    case static_word:
        lgen("%s\t%c%d,%%ecx", op, n);
        break;
        
    case globl_byte:
        cg386_clear2();
        sgen("%s\t%s,%%cl", opb, s);
        break;
        
    case globl_word:
        sgen("%s\t%s,%%ecx", op, s);
        break;
        
    case empty:
        cg386_pop2();
        break;
        
    default:
        fatal("internal: bad type in cg386_load2()");
    }
    
    q = Q_type;
    Q_type = empty;
    return empty == q;
}

/*
 * ============================================================================
 * End of i386 code generation functions.
 *
 * Target descriptors are defined in separate files:
 *   - cg_386_linux.c   : Linux target
 *   - cg_386_freebsd.c : FreeBSD target
 *   - cg_386_netbsd.c  : NetBSD target
 *   - cg_386_openbsd.c : OpenBSD target
 *   - cg_386_windows.c : Windows target
 *
 * Each file includes cg_386.h and defines its struct cg_target with
 * the appropriate OS configuration and vtable pointing to these functions.
 * ============================================================================
 */
