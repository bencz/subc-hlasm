/*
 * SubC Compiler - 8086 Code Generator
 *
 * This file implements the code generator for 8086 architecture.
 * It provides the vtable, architecture description, and all code
 * generation functions for DOS 8086 targets.
 *
 * Copyright (c) 2011-2025 - Public Domain (CC0)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cgtarget.h"
#include "defs.h"
#include "data.h"
#include "decl.h"

/*
 * Forward declarations for internal helper functions
 */
static void m86_cgsynth(char *op);
static void m86_cgcmp(char *inst);
static void m86_cgbrcond(char *i, int n);
static void m86_cgbr(char *how, int n);
static int  m86_cgload2(void);
static void m86_cgclear2(void);
static void m86_cgpop2(void);
static void m86_cglit(int v);
static void m86_cgpush(void);
static void m86_cgstorlw(int n);

/* Helper functions for increment/decrement operations */
static void m86_cgincind(char *r, char *i, int v);
static void m86_cgincloc(char *i, int a, int v);
static void m86_cgincstat(char *i, int a, int v);
static void m86_cgincglob(char *i, char *s, int v);
static void m86_cgincbind(char *r, char *i, int v);
static void m86_cgincbloc(char *i, int a, int v);
static void m86_cgincbstat(char *i, int a, int v);
static void m86_cgincbglob(char *i, char *s, int v);

/*
 * ============================================================================
 * Code Generation Function Implementations
 * ============================================================================
 */

static void m86_cgdata(void)        { gen(".data"); }
static void m86_cgtext(void)        { gen(".code"); }
static void m86_cgprelude(void)     { gen(".model small"); }
static void m86_cgpostlude(void)    { gen("end"); }
static void m86_cgpublic(char *s)   { ngen("public\t%s", s, 0); }

static void m86_cgsynth(char *op) {
    int n;
    char *s;

    n = Q_val;
    s = gsym(Q_name);
    switch (Q_type) {
    case addr_auto:     ngen("%s\tcx,[bp%+d]", "leal", n);
                        sgen("%s\tax,%s", op, "cx");
                        break;
    case addr_static:   lgen("%s\tax,offset %c%d", op, n); break;
    case addr_globl:    sgen("%s\tax,offset %s", op, s); break;
    case addr_label:    lgen("%s\tax,offset %c%d", op, n); break;
    case literal:       ngen("%s\tax,%d", op, n); break;
    case auto_word:     ngen("%s\tax,[bp%+d]", op, n); break;
    case static_word:   lgen("%s\tax,%c%d", op, n); break;
    case globl_word:    sgen("%s\tax,%s", op, s); break;
    case auto_byte:
    case static_byte:
    case globl_byte:    m86_cgload2();
                        ngen("%s\tax,cx", op, 0);
                        break;
    case empty:         m86_cgpop2();
                        sgen("%s\tax,%s", op, "cx");
                        break;
    default:            fatal("internal: bad type in cgsynth()");
    }
    Q_type = empty;
}

static int m86_cgload2(void) {
    int n, q;
    char *s, *op;

    op = "mov";
    n = Q_val;
    s = gsym(Q_name);
    switch (Q_type) {
    case addr_auto:     ngen("%s\tcx,[bp%+d]", "lea", n);
                        break;
    case addr_static:   lgen("%s\tcx,offset %c%d", op, n); break;
    case addr_globl:    sgen("%s\tcx,offset %s", op, s); break;
    case addr_label:    lgen("%s\tcx,offset %c%d", op, n); break;
    case literal:       ngen("%s\tcx,%d", op, n); break;
    case auto_byte:     m86_cgclear2();
                        ngen("%s\tcl,[bp%+d]", op, n);
                        break;
    case auto_word:     ngen("%s\tcx,[bp%+d]", op, n); break;
    case static_byte:   m86_cgclear2();
                        lgen("%s\tcl,%c%d", op, n);
                        break;
    case static_word:   lgen("%s\tcx,%c%d", op, n); break;
    case globl_byte:    m86_cgclear2();
                        sgen("%s\tcl,%s", op, s);
                        break;
    case globl_word:    sgen("%s\tcx,%s", op, s); break;
    case empty:         m86_cgpop2();
                        break;
    default:            fatal("internal: bad type in cgload2()");
    }
    q = Q_type;
    Q_type = empty;
    return empty == q;
}

static void m86_cglit(int v)        { ngen("%s\tax,%d", "mov", v); }
static void m86_cgclear(void)       { gen("xor\tax,ax"); }
static void m86_cgclear2(void)      { gen("xor\tcx,cx"); }
static void m86_cgldgb(char *s)     { sgen("%s\tal,%s", "mov", s); }
static void m86_cgldgw(char *s)     { sgen("%s\tax,%s", "mov", s); }
static void m86_cgldlb(int n)       { ngen("%s\tal,[bp%+d]", "mov", n); }
static void m86_cgldlw(int n)       { ngen("%s\tax,[bp%+d]", "mov", n); }
static void m86_cgldsb(int n)       { lgen("%s\tal,%c%d", "mov", n); }
static void m86_cgldsw(int n)       { lgen("%s\tax,%c%d", "mov", n); }
static void m86_cgldla(int n)       { ngen("%s\tax,[bp%+d]", "lea", n); }
static void m86_cgldsa(int n)       { lgen("%s\tax,offset %c%d", "mov", n); }
static void m86_cgldga(char *s)     { sgen("%s\tax,offset %s", "mov", s); }

static void m86_cgindb(void) {
    gen("mov\tbx,ax");
    m86_cgclear();
    gen("mov\tal,[bx]");
}

static void m86_cgindw(void) {
    gen("mov\tbx,ax");
    gen("mov\tax,[bx]");
}

static void m86_cgldlab(int id)     { lgen("%s\tax,offset %c%d", "mov", id); }

static void m86_cgpush(void)        { gen("push\tax"); }

static void m86_cgpushlit(int n) {
    m86_cglit(n);
    m86_cgpush();
}

static void m86_cgpop2(void)        { gen("pop\tcx"); }
static void m86_cgswap(void)        { gen("xchg\tax,cx"); }
static void m86_cgpopptr(void)      { gen("pop\tbx"); }

static void m86_cgand(void)         { m86_cgsynth("and"); }
static void m86_cgior(void)         { m86_cgsynth("or"); }
static void m86_cgxor(void)         { m86_cgsynth("xor"); }
static void m86_cgadd(void)         { gen("add\tax,cx"); }
static void m86_cgmul(void)         { gen("imul\tcx"); }
static void m86_cgsub(void)         { gen("sub\tax,cx"); }

static void m86_cgdiv(void) {
    gen("cwd");
    gen("idiv\tcx");
}

static void m86_cgmod(void) {
    m86_cgdiv();
    gen("mov\tax,dx");
}

static void m86_cgshl(void)         { gen("shl\tax,cl"); }
static void m86_cgshr(void)         { gen("sar\tax,cl"); }

static void m86_cgcmp(char *inst) {
    int lab;
    lab = label();
    gen("xor\tdx,dx");
    if (empty == Q_type) {
        m86_cgpop2();
        gen("cmp\tcx,ax");
    }
    else {
        m86_cgsynth("cmp");
    }
    lgen("%s\t%c%d", inst, lab);
    gen("inc\tdx");
    genlab(lab);
    gen("mov\tax,dx");
}

static void m86_cgeq(void)          { m86_cgcmp("jne"); }
static void m86_cgne(void)          { m86_cgcmp("je"); }
static void m86_cglt(void)          { m86_cgcmp("jge"); }
static void m86_cggt(void)          { m86_cgcmp("jle"); }
static void m86_cgle(void)          { m86_cgcmp("jg"); }
static void m86_cgge(void)          { m86_cgcmp("jl"); }
static void m86_cgult(void)         { m86_cgcmp("jae"); }
static void m86_cgugt(void)         { m86_cgcmp("jbe"); }
static void m86_cgule(void)         { m86_cgcmp("ja"); }
static void m86_cguge(void)         { m86_cgcmp("jb"); }

static void m86_cgbrcond(char *i, int n) {
    int lab;
    lab = label();
    if (empty == Q_type) {
        m86_cgpop2();
        gen("cmp\tcx,ax");
    }
    else {
        m86_cgsynth("cmp");
    }
    lgen("%s\t%c%d", i, lab);
    lgen("%s\t%c%d", "jmp", n);
    genlab(lab);
}

static void m86_cgbreq(int n)       { m86_cgbrcond("je", n); }
static void m86_cgbrne(int n)       { m86_cgbrcond("jne", n); }
static void m86_cgbrlt(int n)       { m86_cgbrcond("jl", n); }
static void m86_cgbrgt(int n)       { m86_cgbrcond("jg", n); }
static void m86_cgbrle(int n)       { m86_cgbrcond("jle", n); }
static void m86_cgbrge(int n)       { m86_cgbrcond("jge", n); }
static void m86_cgbrult(int n)      { m86_cgbrcond("jb", n); }
static void m86_cgbrugt(int n)      { m86_cgbrcond("ja", n); }
static void m86_cgbrule(int n)      { m86_cgbrcond("jbe", n); }
static void m86_cgbruge(int n)      { m86_cgbrcond("jae", n); }

static void m86_cgneg(void)         { gen("neg\tax"); }
static void m86_cgnot(void)         { gen("not\tax"); }

static void m86_cglognot(void) {
    gen("neg\tax");
    gen("sbb\tax,ax");
    gen("inc\tax");
}

static void m86_cgscale(void)       { gen("shl\tax,1"); }
static void m86_cgscale2(void)      { gen("shl\tcx,1"); }
static void m86_cgunscale(void)     { gen("shr\tax,1"); }

static void m86_cgscaleby(int v) {
    ngen("%s\tcx,%d", "mov", v);
    gen("mul\tcx");
}

static void m86_cgscale2by(int v) {
    gen("push\tax");
    ngen("%s\tax,%d", "mov", v);
    gen("mul\tcx");
    gen("mov\tcx,ax");
    gen("pop\tax");
}

static void m86_cgunscaleby(int v) {
    ngen("%s\tcx,%d", "mov", v);
    gen("xor\tdx,dx");
    gen("div\tcx");
}

static void m86_cgbool(void) {
    gen("neg\tax");
    gen("sbb\tax,ax");
    gen("neg\tax");
}

/* Helper functions for increment/decrement */
static void m86_cgincind(char *r, char *i, int v) {
    if (strcmp(r, "bx")) sgen("%s\tbx,%s", "mov", r);
    ngen("%s\tcx,%d", "mov", v);
    sgen("%s\t[bx],cx", i, NULL);
}

static void m86_cgincloc(char *i, int a, int v) {
    ngen("%s\tcx,%d", "mov", v);
    ngen("%s\t[bp%+d],cx", i, a);
}

static void m86_cgincstat(char *i, int a, int v) {
    ngen("%s\tcx,%d", "mov", v);
    lgen("%s\t%c%d,cx", i, a);
}

static void m86_cgincglob(char *i, char *s, int v) {
    ngen("%s\tcx,%d", "mov", v);
    sgen("%s\t%s,cx", i, s);
}

static void m86_cgincbind(char *r, char *i, int v) {
    if (strcmp(r, "bx")) sgen("%s\tbx,%s", "mov", r);
    ngen("%s\tcl,%d", "mov", v);
    sgen("%s\tbyte ptr [bx],cl", i, NULL);
}

static void m86_cgincbloc(char *i, int a, int v) {
    ngen("%s\tcl,%d", "mov", v);
    ngen("%s\tbyte ptr [bp%+d],cl", i, v);
}

static void m86_cgincbstat(char *i, int a, int v) {
    ngen("%s\tcl,%d", "mov", v);
    lgen("%s\t%c%d,cl", i, a);
}

static void m86_cgincbglob(char *i, char *s, int v) {
    ngen("%s\tcl,%d", "mov", v);
    sgen("%s\t%s,cl", i, s);
}

static void m86_cgldinc(void)       { /* gen("mov\tbx,ax"); */ }
static void m86_cginc1pi(int v)     { m86_cgincind("ax", "add", v); }
static void m86_cgdec1pi(int v)     { m86_cgincind("ax", "sub", v); }
static void m86_cginc2pi(int v)     { m86_cgincind("bx", "add", v); }
static void m86_cgdec2pi(int v)     { m86_cgincind("bx", "sub", v); }
static void m86_cgincpl(int a, int v)   { m86_cgincloc("add", a, v); }
static void m86_cgdecpl(int a, int v)   { m86_cgincloc("sub", a, v); }
static void m86_cgincps(int a, int v)   { m86_cgincstat("add", a, v); }
static void m86_cgdecps(int a, int v)   { m86_cgincstat("sub", a, v); }
static void m86_cgincpg(char *s, int v) { m86_cgincglob("add", s, v); }
static void m86_cgdecpg(char *s, int v) { m86_cgincglob("sub", s, v); }
static void m86_cginc1iw(void)      { m86_cgincind("ax", "add", 1); }
static void m86_cgdec1iw(void)      { m86_cgincind("ax", "sub", 1); }
static void m86_cginc2iw(void)      { m86_cgincind("bx", "add", 1); }
static void m86_cgdec2iw(void)      { m86_cgincind("bx", "sub", 1); }
static void m86_cginclw(int a)      { m86_cgincloc("add", a, 1); }
static void m86_cgdeclw(int a)      { m86_cgincloc("sub", a, 1); }
static void m86_cgincsw(int a)      { m86_cgincstat("add", a, 1); }
static void m86_cgdecsw(int a)      { m86_cgincstat("sub", a, 1); }
static void m86_cgincgw(char *s)    { m86_cgincglob("add", s, 1); }
static void m86_cgdecgw(char *s)    { m86_cgincglob("sub", s, 1); }
static void m86_cginc1ib(void)      { m86_cgincbind("ax", "add", 1); }
static void m86_cgdec1ib(void)      { m86_cgincbind("ax", "sub", 1); }
static void m86_cginc2ib(void)      { m86_cgincbind("bx", "add", 1); }
static void m86_cgdec2ib(void)      { m86_cgincbind("bx", "sub", 1); }
static void m86_cginclb(int a)      { m86_cgincbloc("add", a, 1); }
static void m86_cgdeclb(int a)      { m86_cgincbloc("sub", a, 1); }
static void m86_cgincsb(int a)      { m86_cgincbstat("add", a, 1); }
static void m86_cgdecsb(int a)      { m86_cgincbstat("sub", a, 1); }
static void m86_cgincgb(char *s)    { m86_cgincglob("add", s, 1); }
static void m86_cgdecgb(char *s)    { m86_cgincglob("sub", s, 1); }

static void m86_cgbr(char *how, int n) {
    int lab;
    lab = label();
    gen("or\tax,ax");
    lgen("%s\t%c%d", how, lab);
    lgen("%s\t%c%d", "jmp", n);
    genlab(lab);
}

static void m86_cgbrtrue(int n)     { m86_cgbr("jz", n); }
static void m86_cgbrfalse(int n)    { m86_cgbr("jnz", n); }
static void m86_cgjump(int n)       { lgen("%s\t%c%d", "jmp", n); }
static void m86_cgldswtch(int n)    { lgen("%s\tsi,offset %c%d", "mov", n); }
static void m86_cgcalswtch(void)    { gen("jmp\tswitch"); }
static void m86_cgcase(int v, int l) { lgen2("dw\t%d,%c%d", v, l); }

static void m86_cgstorib(void)      { ngen("%s\t[bx],al", "mov", 0); }
static void m86_cgstoriw(void)      { ngen("%s\t[bx],ax", "mov", 0); }
static void m86_cgstorlb(int n)     { ngen("%s\t[bp%+d],al", "mov", n); }
static void m86_cgstorlw(int n)     { ngen("%s\t[bp%+d],ax", "mov", n); }
static void m86_cgstorsb(int n)     { lgen("%s\t%c%d,al", "mov", n); }
static void m86_cgstorsw(int n)     { lgen("%s\t%c%d,ax", "mov", n); }
static void m86_cgstorgb(char *s)   { sgen("%s\t%s,al", "mov", s); }
static void m86_cgstorgw(char *s)   { sgen("%s\t%s,ax", "mov", s); }

static void m86_cginitlw(int v, int a) {
    m86_cglit(v);
    m86_cgstorlw(a);
}

static void m86_cgcall(char *s)     { sgen("%s\t%s", "call", s); }
static void m86_cgcalr(void)        { gen("call\tax"); }
static void m86_cgstack(int n)      { ngen("%s\tsp,%d", "add", n); }

static void m86_cgentry(void) {
    gen("push\tbp");
    gen("mov\tbp,sp");
}

static void m86_cgexit(void) {
    gen("pop\tbp");
    gen("ret");
}

static void m86_cgdefb(int v)       { ngen("%s\t%d", "db", v); }
static void m86_cgdefw(int v)       { ngen("%s\t%d", "dw", v); }
static void m86_cgdefp(int v)       { ngen("%s\t%d", "dw", v); }
static void m86_cgdefl(int v)       { lgen("%s\t%c%d", "dw", v); }
static void m86_cgdefc(int c)       { ngen("%s\t'%c'", "db", c); }

static void m86_cggbss(char *s, int z) {
    genraw(s);
    genraw(":");
    ngen("%s\t0 dup %d", "db", z);
}

static void m86_cglbss(char *s, int z) { m86_cggbss(s, z); }
static void m86_cgalign(void)       { /* unused */ }

/*
 * ============================================================================
 * 8086 Architecture Description
 * ============================================================================
 */
struct cg_arch cg_arch_8086 = {
    "8086",             /* name */
    16,                 /* bits */
    1,                  /* char_size */
    2,                  /* int_size */
    2,                  /* ptr_size */
    2,                  /* bpw */
    ENDIAN_LITTLE,      /* endian */
    STACK_DOWN,         /* stack_dir */
    ASM_TASM,           /* asm_syntax */
    CC_CDECL,           /* call_conv */
    FLOAT_NONE,         /* float_format */
    2,                  /* align_stack */
    2,                  /* align_data */
    2,                  /* align_func */
    1,                  /* has_mul */
    1,                  /* has_div */
    1,                  /* has_mod */
    1,                  /* has_byte_ops */
    0,                  /* needs_alignment */
    4,                  /* param_offset_base (return addr + saved bp) */
    0,                  /* param_offset_dir */
    0                   /* local_offset_base */
};

/*
 * ============================================================================
 * 8086 Code Generator Vtable
 * ============================================================================
 */
struct cg_vtable cg_vtable_8086 = {
    /* Section Control */
    m86_cgdata,
    m86_cgtext,
    m86_cgprelude,
    m86_cgpostlude,
    m86_cgpublic,
    
    /* Synthesizer Support */
    m86_cgsynth,
    m86_cgload2,
    
    /* Literal and Clear */
    m86_cglit,
    m86_cgclear,
    m86_cgclear2,
    
    /* Load Operations - Global */
    m86_cgldgb,
    m86_cgldgw,
    
    /* Load Operations - Local */
    m86_cgldlb,
    m86_cgldlw,
    
    /* Load Operations - Static */
    m86_cgldsb,
    m86_cgldsw,
    
    /* Load Address Operations */
    m86_cgldla,
    m86_cgldsa,
    m86_cgldga,
    
    /* Indirect Load Operations */
    m86_cgindb,
    m86_cgindw,
    
    /* Load Label */
    m86_cgldlab,
    
    /* Stack Operations */
    m86_cgpush,
    m86_cgpushlit,
    m86_cgpop2,
    m86_cgswap,
    m86_cgpopptr,
    
    /* Arithmetic Operations */
    m86_cgand,
    m86_cgior,
    m86_cgxor,
    m86_cgadd,
    m86_cgsub,
    m86_cgmul,
    m86_cgdiv,
    m86_cgmod,
    m86_cgshl,
    m86_cgshr,
    
    /* Unary Operations */
    m86_cgneg,
    m86_cgnot,
    m86_cglognot,
    
    /* Scaling Operations */
    m86_cgscale,
    m86_cgscale2,
    m86_cgunscale,
    m86_cgscaleby,
    m86_cgscale2by,
    m86_cgunscaleby,
    
    /* Comparison Operations */
    m86_cgeq,
    m86_cgne,
    m86_cglt,
    m86_cggt,
    m86_cgle,
    m86_cgge,
    m86_cgult,
    m86_cgugt,
    m86_cgule,
    m86_cguge,
    
    /* Conditional Branch Operations */
    m86_cgbreq,
    m86_cgbrne,
    m86_cgbrlt,
    m86_cgbrgt,
    m86_cgbrle,
    m86_cgbrge,
    m86_cgbrult,
    m86_cgbrugt,
    m86_cgbrule,
    m86_cgbruge,
    
    /* Branch Operations */
    m86_cgbrtrue,
    m86_cgbrfalse,
    m86_cgjump,
    m86_cgldswtch,
    m86_cgcalswtch,
    m86_cgcase,
    
    /* Boolean Operations */
    m86_cgbool,
    
    /* Increment/Decrement via Pointer */
    m86_cgldinc,
    m86_cginc1pi,
    m86_cgdec1pi,
    m86_cginc2pi,
    m86_cgdec2pi,
    
    /* Increment/Decrement Pointer - Local/Static/Global */
    m86_cgincpl,
    m86_cgdecpl,
    m86_cgincps,
    m86_cgdecps,
    m86_cgincpg,
    m86_cgdecpg,
    
    /* Increment/Decrement Indirect Word */
    m86_cginc1iw,
    m86_cgdec1iw,
    m86_cginc2iw,
    m86_cgdec2iw,
    
    /* Increment/Decrement Local Word */
    m86_cginclw,
    m86_cgdeclw,
    
    /* Increment/Decrement Static Word */
    m86_cgincsw,
    m86_cgdecsw,
    
    /* Increment/Decrement Global Word */
    m86_cgincgw,
    m86_cgdecgw,
    
    /* Increment/Decrement Indirect Byte */
    m86_cginc1ib,
    m86_cgdec1ib,
    m86_cginc2ib,
    m86_cgdec2ib,
    
    /* Increment/Decrement Local Byte */
    m86_cginclb,
    m86_cgdeclb,
    
    /* Increment/Decrement Static Byte */
    m86_cgincsb,
    m86_cgdecsb,
    
    /* Increment/Decrement Global Byte */
    m86_cgincgb,
    m86_cgdecgb,
    
    /* Store Operations */
    m86_cgstorib,
    m86_cgstoriw,
    m86_cgstorlb,
    m86_cgstorlw,
    m86_cgstorsb,
    m86_cgstorsw,
    m86_cgstorgb,
    m86_cgstorgw,
    
    /* Function Operations */
    m86_cginitlw,
    m86_cgcall,
    m86_cgcalr,
    m86_cgstack,
    m86_cgentry,
    m86_cgexit,
    
    /* Data Definition */
    m86_cgdefb,
    m86_cgdefw,
    m86_cgdefp,
    m86_cgdefl,
    m86_cgdefc,
    m86_cggbss,
    m86_cglbss,
    
    /* Alignment */
    m86_cgalign
};

/*
 * ============================================================================
 * External OS configurations
 * ============================================================================
 */
extern struct cg_os_config cg_os_dos;

/*
 * ============================================================================
 * Target Definitions for 8086
 * ============================================================================
 */
struct cg_target cg_target_dos_8086 = {
    "dos-8086",
    "DOS 8086 (OMF, TASM syntax)",
    &cg_arch_8086,
    &cg_os_dos,
    &cg_vtable_8086,
    NULL,
    NULL
};
