/*
 * SubC Compiler - Intel 8086 (16-bit DOS) Target
 *
 * This file implements the code generator for the Intel 8086 architecture.
 * It generates Turbo Assembler (TASM) syntax assembly code for DOS.
 *
 * Architecture characteristics:
 *   - 16-bit registers and pointers
 *   - Little-endian byte order
 *   - Stack grows downward (toward lower addresses)
 *   - Small memory model (64KB code, 64KB data)
 *
 * Register usage:
 *   AX - Primary accumulator, return value
 *   CX - Auxiliary register, shift count
 *   DX - Used for division, multiplication overflow
 *   BX - Base register for indirect addressing
 *   BP - Frame pointer (base pointer)
 *   SP - Stack pointer
 *   SI - Switch table pointer
 *
 * Key differences from 386:
 *   - 16-bit registers (ax vs eax)
 *   - No 32-bit immediate addressing
 *   - Different instruction syntax (TASM vs GAS)
 *   - Pointer size is 2 bytes instead of 4
 *   - Uses BX for indirect addressing (not EDX)
 *
 * Copyright (c) 2011-2025 - Public Domain (CC0)
 */

#include "defs.h"
#include "data.h"
#include "decl.h"
#include "cgtarget.h"
#include "cg_8086_funcs.h"

/*
 * ============================================================================
 * SECTION: Helper Functions
 * ============================================================================
 */

/*
 * cg86_synth - Synthesize operation with queued operand
 *
 * @op: The operation mnemonic (e.g., "add", "sub")
 *
 * TASM syntax: op dst, src (Intel order, opposite of AT&T)
 */
void cg86_synth(char *op) {
    int   n;
    char *s;

    n = Q_val;
    s = gsym(Q_name);
    
    switch (Q_type) {
    case addr_auto:
        /* LEA not available for all cases on 8086, use manual calc */
        ngen("%s\tcx,[bp%+d]", "lea", n);
        sgen("%s\tax,%s", op, "cx");
        break;
        
    case addr_static:
        lgen("%s\tax,offset %c%d", op, n);
        break;
        
    case addr_globl:
        sgen("%s\tax,offset %s", op, s);
        break;
        
    case addr_label:
        lgen("%s\tax,offset %c%d", op, n);
        break;
        
    case literal:
        ngen("%s\tax,%d", op, n);
        break;
        
    case auto_word:
        ngen("%s\tax,[bp%+d]", op, n);
        break;
        
    case static_word:
        lgen("%s\tax,%c%d", op, n);
        break;
        
    case globl_word:
        sgen("%s\tax,%s", op, s);
        break;
        
    case auto_byte:
    case static_byte:
    case globl_byte:
        cg86_load2();
        ngen("%s\tax,cx", op, 0);
        break;
        
    case empty:
        cg86_pop2();
        sgen("%s\tax,%s", op, "cx");
        break;
        
    default:
        fatal("internal: bad type in cg86_synth()");
    }
    
    Q_type = empty;
}

/*
 * cg86_cmp - Generate comparison and set result
 */
void cg86_cmp(char *inst) {
    int lab;
    
    lab = label();
    gen("xor\tdx,dx");
    
    if (empty == Q_type) {
        cg86_pop2();
        gen("cmp\tcx,ax");
    }
    else {
        cg86_synth("cmp");
    }
    
    lgen("%s\t%c%d", inst, lab);
    gen("inc\tdx");
    genlab(lab);
    gen("mov\tax,dx");
}

/*
 * cg86_brcond - Generate conditional branch
 */
void cg86_brcond(char *i, int n) {
    int lab;
    
    lab = label();
    
    if (empty == Q_type) {
        cg86_pop2();
        gen("cmp\tcx,ax");
    }
    else {
        cg86_synth("cmp");
    }
    
    lgen("%s\t%c%d", i, lab);
    lgen("%s\t%c%d", "jmp", n);
    genlab(lab);
}

/*
 * cg86_br - Generate branch on accumulator value
 */
void cg86_br(char *how, int n) {
    int lab;
    
    lab = label();
    gen("or\tax,ax");
    lgen("%s\t%c%d", how, lab);
    lgen("%s\t%c%d", "jmp", n);
    genlab(lab);
}

/*
 * Helper functions for increment/decrement operations
 * 8086 doesn't have inc/dec with immediate, need to use add/sub
 */

void cg86_incind(char *r, char *i, int v) {
    if (strcmp(r, "bx")) sgen("%s\tbx,%s", "mov", r);
    ngen("%s\tcx,%d", "mov", v);
    sgen("%s\t[bx],cx", i, NULL);
}

void cg86_incloc(char *i, int a, int v) {
    ngen("%s\tcx,%d", "mov", v);
    ngen("%s\t[bp%+d],cx", i, a);
}

void cg86_incstat(char *i, int a, int v) {
    ngen("%s\tcx,%d", "mov", v);
    lgen("%s\t%c%d,cx", i, a);
}

void cg86_incglob(char *i, char *s, int v) {
    ngen("%s\tcx,%d", "mov", v);
    sgen("%s\t%s,cx", i, s);
}

void cg86_incbind(char *r, char *i, int v) {
    if (strcmp(r, "bx")) sgen("%s\tbx,%s", "mov", r);
    ngen("%s\tcl,%d", "mov", v);
    sgen("%s\tbyte ptr [bx],cl", i, NULL);
}

void cg86_incbloc(char *i, int a, int v) {
    ngen("%s\tcl,%d", "mov", v);
    ngen("%s\tbyte ptr [bp%+d],cl", i, v);
}

void cg86_incbstat(char *i, int a, int v) {
    ngen("%s\tcl,%d", "mov", v);
    lgen("%s\t%c%d,cl", i, a);
}

void cg86_incbglob(char *i, char *s, int v) {
    ngen("%s\tcl,%d", "mov", v);
    sgen("%s\t%s,cl", i, s);
}

/*
 * ============================================================================
 * SECTION: Section Control Functions
 * ============================================================================
 */

void cg86_data(void) {
    gen(".data");
}

void cg86_text(void) {
    gen(".code");
}

void cg86_prelude(void) {
    gen(".model small");
}

void cg86_postlude(void) {
    gen("end");
}

void cg86_public(char *s) {
    ngen("public\t%s", s, 0);
}

void cg86_name(char *s) {
    sgen("%s:", s, "");
}

void cg86_funcname(char *s) {
    sgen("%s:", s, "");
}

void cg86_align(void) {
    /* Not typically needed for 8086 */
}

/*
 * ============================================================================
 * SECTION: Literal and Clear Functions
 * ============================================================================
 */

void cg86_lit(int v) {
    ngen("%s\tax,%d", "mov", v);
}

void cg86_clear(void) {
    gen("xor\tax,ax");
}

void cg86_clear2(void) {
    gen("xor\tcx,cx");
}

/*
 * ============================================================================
 * SECTION: Load Operations
 * ============================================================================
 */

void cg86_ldgb(char *s) {
    sgen("%s\tal,%s", "mov", s);
}

void cg86_ldgw(char *s) {
    sgen("%s\tax,%s", "mov", s);
}

void cg86_ldlb(int n) {
    ngen("%s\tal,[bp%+d]", "mov", n);
}

void cg86_ldlw(int n) {
    ngen("%s\tax,[bp%+d]", "mov", n);
}

void cg86_ldsb(int n) {
    lgen("%s\tal,%c%d", "mov", n);
}

void cg86_ldsw(int n) {
    lgen("%s\tax,%c%d", "mov", n);
}

void cg86_ldla(int n) {
    ngen("%s\tax,[bp%+d]", "lea", n);
}

void cg86_ldsa(int n) {
    lgen("%s\tax,offset %c%d", "mov", n);
}

void cg86_ldga(char *s) {
    sgen("%s\tax,offset %s", "mov", s);
}

void cg86_indb(void) {
    gen("mov\tbx,ax");
    cg86_clear();
    gen("mov\tal,[bx]");
}

void cg86_indw(void) {
    gen("mov\tbx,ax");
    gen("mov\tax,[bx]");
}

void cg86_ldlab(int id) {
    lgen("%s\tax,offset %c%d", "mov", id);
}

/*
 * ============================================================================
 * SECTION: Stack Operations
 * ============================================================================
 */

void cg86_push(void) {
    gen("push\tax");
}

void cg86_pushlit(int n) {
    /* 8086 doesn't have push immediate, must load first */
    cg86_lit(n);
    cg86_push();
}

void cg86_pop2(void) {
    gen("pop\tcx");
}

void cg86_swap(void) {
    gen("xchg\tax,cx");
}

void cg86_popptr(void) {
    gen("pop\tbx");
}

/*
 * ============================================================================
 * SECTION: Arithmetic Operations
 * ============================================================================
 */

void cg86_add(void) {
    gen("add\tax,cx");
}

void cg86_sub(void) {
    gen("sub\tax,cx");
}

void cg86_mul(void) {
    gen("imul\tcx");
}

void cg86_div(void) {
    gen("cwd");           /* Sign-extend AX into DX:AX */
    gen("idiv\tcx");      /* Divide DX:AX by CX */
}

void cg86_mod(void) {
    cg86_div();
    gen("mov\tax,dx");    /* Remainder is in DX */
}

void cg86_neg(void) {
    gen("neg\tax");
}

/*
 * ============================================================================
 * SECTION: Bitwise Operations
 * ============================================================================
 */

void cg86_and(void) {
    cg86_synth("and");
}

void cg86_ior(void) {
    cg86_synth("or");
}

void cg86_xor(void) {
    cg86_synth("xor");
}

void cg86_not(void) {
    gen("not\tax");
}

void cg86_shl(void) {
    gen("shl\tax,cl");
}

void cg86_shr(void) {
    gen("sar\tax,cl");
}

/*
 * ============================================================================
 * SECTION: Comparison Operations
 * ============================================================================
 */

void cg86_eq(void)  { cg86_cmp("jne"); }
void cg86_ne(void)  { cg86_cmp("je");  }
void cg86_lt(void)  { cg86_cmp("jge"); }
void cg86_gt(void)  { cg86_cmp("jle"); }
void cg86_le(void)  { cg86_cmp("jg");  }
void cg86_ge(void)  { cg86_cmp("jl");  }
void cg86_ult(void) { cg86_cmp("jae"); }
void cg86_ugt(void) { cg86_cmp("jbe"); }
void cg86_ule(void) { cg86_cmp("ja");  }
void cg86_uge(void) { cg86_cmp("jb");  }

/*
 * ============================================================================
 * SECTION: Conditional Branch Operations
 * ============================================================================
 */

void cg86_breq(int n)  { cg86_brcond("je",  n); }
void cg86_brne(int n)  { cg86_brcond("jne", n); }
void cg86_brlt(int n)  { cg86_brcond("jl",  n); }
void cg86_brgt(int n)  { cg86_brcond("jg",  n); }
void cg86_brle(int n)  { cg86_brcond("jle", n); }
void cg86_brge(int n)  { cg86_brcond("jge", n); }
void cg86_brult(int n) { cg86_brcond("jb",  n); }
void cg86_brugt(int n) { cg86_brcond("ja",  n); }
void cg86_brule(int n) { cg86_brcond("jbe", n); }
void cg86_bruge(int n) { cg86_brcond("jae", n); }

/*
 * ============================================================================
 * SECTION: Boolean and Logic Operations
 * ============================================================================
 */

void cg86_lognot(void) {
    gen("neg\tax");
    gen("sbb\tax,ax");
    gen("inc\tax");
}

void cg86_bool(void) {
    gen("neg\tax");
    gen("sbb\tax,ax");
    gen("neg\tax");
}

/*
 * ============================================================================
 * SECTION: Pointer Scaling Operations
 * ============================================================================
 *
 * On 8086, sizeof(int) = 2, so we shift by 1 bit.
 */

void cg86_scale(void) {
    gen("shl\tax,1");
}

void cg86_scale2(void) {
    gen("shl\tcx,1");
}

void cg86_unscale(void) {
    gen("shr\tax,1");
}

void cg86_scaleby(int v) {
    ngen("%s\tcx,%d", "mov", v);
    gen("mul\tcx");
}

void cg86_scale2by(int v) {
    gen("push\tax");
    ngen("%s\tax,%d", "mov", v);
    gen("mul\tcx");
    gen("mov\tcx,ax");
    gen("pop\tax");
}

void cg86_unscaleby(int v) {
    ngen("%s\tcx,%d", "mov", v);
    gen("xor\tdx,dx");
    gen("div\tcx");
}

/*
 * ============================================================================
 * SECTION: Increment/Decrement Operations
 * ============================================================================
 */

void cg86_ldinc(void) {
    /* On 8086, we use BX for indirect, value already there after indw */
}

void cg86_inc1pi(int v) { cg86_incind("ax", "add", v); }
void cg86_dec1pi(int v) { cg86_incind("ax", "sub", v); }
void cg86_inc2pi(int v) { cg86_incind("bx", "add", v); }
void cg86_dec2pi(int v) { cg86_incind("bx", "sub", v); }

void cg86_incpl(int a, int v) { cg86_incloc("add", a, v); }
void cg86_decpl(int a, int v) { cg86_incloc("sub", a, v); }
void cg86_inclw(int a) { cg86_incloc("add", a, 1); }
void cg86_declw(int a) { cg86_incloc("sub", a, 1); }
void cg86_inclb(int a) { cg86_incbloc("add", a, 1); }
void cg86_declb(int a) { cg86_incbloc("sub", a, 1); }

void cg86_incps(int a, int v) { cg86_incstat("add", a, v); }
void cg86_decps(int a, int v) { cg86_incstat("sub", a, v); }
void cg86_incsw(int a) { cg86_incstat("add", a, 1); }
void cg86_decsw(int a) { cg86_incstat("sub", a, 1); }
void cg86_incsb(int a) { cg86_incbstat("add", a, 1); }
void cg86_decsb(int a) { cg86_incbstat("sub", a, 1); }

void cg86_incpg(char *s, int v) { cg86_incglob("add", s, v); }
void cg86_decpg(char *s, int v) { cg86_incglob("sub", s, v); }
void cg86_incgw(char *s) { cg86_incglob("add", s, 1); }
void cg86_decgw(char *s) { cg86_incglob("sub", s, 1); }
void cg86_incgb(char *s) { cg86_incbglob("add", s, 1); }
void cg86_decgb(char *s) { cg86_incbglob("sub", s, 1); }

void cg86_inc1iw(void) { cg86_incind("ax", "add", 1); }
void cg86_dec1iw(void) { cg86_incind("ax", "sub", 1); }
void cg86_inc2iw(void) { cg86_incind("bx", "add", 1); }
void cg86_dec2iw(void) { cg86_incind("bx", "sub", 1); }
void cg86_inc1ib(void) { cg86_incbind("ax", "add", 1); }
void cg86_dec1ib(void) { cg86_incbind("ax", "sub", 1); }
void cg86_inc2ib(void) { cg86_incbind("bx", "add", 1); }
void cg86_dec2ib(void) { cg86_incbind("bx", "sub", 1); }

/*
 * ============================================================================
 * SECTION: Unconditional Branch Operations
 * ============================================================================
 */

void cg86_brtrue(int n) {
    cg86_br("jz", n);
}

void cg86_brfalse(int n) {
    cg86_br("jnz", n);
}

void cg86_jump(int n) {
    lgen("%s\t%c%d", "jmp", n);
}

/*
 * ============================================================================
 * SECTION: Switch Statement Support
 * ============================================================================
 */

void cg86_ldswtch(int n) {
    lgen("%s\tsi,offset %c%d", "mov", n);
}

void cg86_calswtch(void) {
    gen("jmp\tswitch");
}

void cg86_case(int v, int l) {
    lgen2("dw\t%d,%c%d", v, l);
}

/*
 * ============================================================================
 * SECTION: Store Operations
 * ============================================================================
 */

void cg86_storib(void) {
    ngen("%s\t[bx],al", "mov", 0);
}

void cg86_storiw(void) {
    ngen("%s\t[bx],ax", "mov", 0);
}

void cg86_storlb(int n) {
    ngen("%s\t[bp%+d],al", "mov", n);
}

void cg86_storlw(int n) {
    ngen("%s\t[bp%+d],ax", "mov", n);
}

void cg86_storsb(int n) {
    lgen("%s\t%c%d,al", "mov", n);
}

void cg86_storsw(int n) {
    lgen("%s\t%c%d,ax", "mov", n);
}

void cg86_storgb(char *s) {
    sgen("%s\t%s,al", "mov", s);
}

void cg86_storgw(char *s) {
    sgen("%s\t%s,ax", "mov", s);
}

/*
 * ============================================================================
 * SECTION: Function Call Operations
 * ============================================================================
 */

void cg86_initlw(int v, int a) {
    cg86_lit(v);
    cg86_storlw(a);
}

void cg86_call(char *s) {
    sgen("%s\t%s", "call", s);
}

void cg86_calr(void) {
    gen("call\tax");
}

void cg86_stack(int n) {
    ngen("%s\tsp,%d", "add", n);
}

void cg86_entry(int lsize) {
    (void)lsize;  /* Not used for 8086 */
    gen("push\tbp");
    gen("mov\tbp,sp");
}

void cg86_exit(void) {
    gen("pop\tbp");
    gen("ret");
}

/*
 * ============================================================================
 * SECTION: Data Definition
 * ============================================================================
 */

void cg86_defb(int v) {
    ngen("%s\t%d", "db", v);
}

void cg86_defw(int v) {
    ngen("%s\t%d", "dw", v);
}

void cg86_defp(int v) {
    ngen("%s\t%d", "dw", v);
}

void cg86_defl(int v) {
    lgen("%s\t%c%d", "dw", v);
}

void cg86_defc(int c) {
    ngen("%s\t'%c'", "db", c);
}

void cg86_gbss(char *s, int z) {
    genraw(s);
    genraw(":");
    ngen("%s\t0 dup %d", "db", z);
}

void cg86_lbss(char *s, int z) {
    cg86_gbss(s, z);
}

/*
 * ============================================================================
 * SECTION: Synthesizer Support
 * ============================================================================
 */

int cg86_load2(void) {
    int   n, q;
    char *s, *op;

    op = "mov";
    n  = Q_val;
    s  = gsym(Q_name);
    
    switch (Q_type) {
    case addr_auto:
        ngen("%s\tcx,[bp%+d]", "lea", n);
        break;
        
    case addr_static:
        lgen("%s\tcx,offset %c%d", op, n);
        break;
        
    case addr_globl:
        sgen("%s\tcx,offset %s", op, s);
        break;
        
    case addr_label:
        lgen("%s\tcx,offset %c%d", op, n);
        break;
        
    case literal:
        ngen("%s\tcx,%d", op, n);
        break;
        
    case auto_byte:
        cg86_clear2();
        ngen("%s\tcl,[bp%+d]", op, n);
        break;
        
    case auto_word:
        ngen("%s\tcx,[bp%+d]", op, n);
        break;
        
    case static_byte:
        cg86_clear2();
        lgen("%s\tcl,%c%d", op, n);
        break;
        
    case static_word:
        lgen("%s\tcx,%c%d", op, n);
        break;
        
    case globl_byte:
        cg86_clear2();
        sgen("%s\tcl,%s", op, s);
        break;
        
    case globl_word:
        sgen("%s\tcx,%s", op, s);
        break;
        
    case empty:
        cg86_pop2();
        break;
        
    default:
        fatal("internal: bad type in cg86_load2()");
    }
    
    q = Q_type;
    Q_type = empty;
    return empty == q;
}

