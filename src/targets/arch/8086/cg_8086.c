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
static void m86_cglit(long v);
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

static void m86_cglit(long v)       { ngen("%s\tax,%ld", "mov", v); }
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
static void m86_cgcase(int v, int l, int tbl) { (void)tbl; lgen2("dw\t%d,%c%d", v, l); }

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
static void m86_cgdefh(int v)       { ngen("%s\t%d", "dw", v); }   /* 2 bytes */
static void m86_cgdefw(int v)       { ngen("%s\t%d", "dw", v); }   /* 2 bytes on 8086 (native int) */
static void m86_cgdefd(int v)       { ngen("%s\t%d", "dd", v); }   /* 4 bytes (for long) */
static void m86_cgdefp(int v)       { ngen("%s\t%d", "dw", v); }   /* 2 bytes (near pointer) */
static void m86_cgdefl(int v, int tbl) { (void)tbl; lgen("%s\t%c%d", "dw", v); }
static void m86_cgdefq(int v)       { ngen("%s\t%d", "dd", v); ngen("%s\t%d", "dd", 0); } /* 8 bytes (2x dd) */
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
 * 8086 x87 Floating-Point Operations (8087/80287 coprocessor)
 * ============================================================================
 *
 * The x87 FPU uses a stack-based architecture with 8 registers (ST0-ST7).
 * ST0 is the top of stack. Operations typically work on ST0 and ST1.
 *
 * IEEE 754 format:
 *   float (32-bit):  1 sign + 8 exponent + 23 mantissa
 *   double (64-bit): 1 sign + 11 exponent + 52 mantissa
 */

/* Load float from local variable to ST0 */
static void m86_cgfloads_x87(int n) {
    ngen("%s\tdword ptr [bp%+d]", "fld", n);
}

/* Load double from local variable to ST0 */
static void m86_cgfloadd_x87(int n) {
    ngen("%s\tqword ptr [bp%+d]", "fld", n);
}

/* Load float from global symbol to ST0 */
static void m86_cgfloadgs_x87(char *s) {
    sgen("%s\tdword ptr %s", "fld", s);
}

/* Load double from global symbol to ST0 */
static void m86_cgfloadgd_x87(char *s) {
    sgen("%s\tqword ptr %s", "fld", s);
}

/* Store ST0 to local float variable */
static void m86_cgfstores_x87(int n) {
    ngen("%s\tdword ptr [bp%+d]", "fstp", n);
}

/* Store ST0 to local double variable */
static void m86_cgfstored_x87(int n) {
    ngen("%s\tqword ptr [bp%+d]", "fstp", n);
}

/* Store ST0 to global float symbol */
static void m86_cgfstoregs_x87(char *s) {
    sgen("%s\tdword ptr %s", "fstp", s);
}

/* Store ST0 to global double symbol */
static void m86_cgfstoregsd_x87(char *s) {
    sgen("%s\tqword ptr %s", "fstp", s);
}

/* Load float literal from label */
static void m86_cgflits_x87(int lab) {
    lgen("%s\tdword ptr %c%d", "fld", lab);
}

/* Load double literal from label */
static void m86_cgflitd_x87(int lab) {
    lgen("%s\tqword ptr %c%d", "fld", lab);
}

/* Float addition: ST0 = ST1 + ST0, pop ST1 */
static void m86_cgfadds_x87(void) {
    gen("faddp\tst(1),st");
}

/* Double addition (same as float on x87) */
static void m86_cgfaddd_x87(void) {
    gen("faddp\tst(1),st");
}

/* Float subtraction: ST0 = ST1 - ST0, pop ST1 */
static void m86_cgfsubs_x87(void) {
    gen("fsubrp\tst(1),st");  /* reverse subtract: ST1 - ST0 */
}

/* Double subtraction */
static void m86_cgfsubd_x87(void) {
    gen("fsubrp\tst(1),st");
}

/* Float multiplication: ST0 = ST1 * ST0, pop ST1 */
static void m86_cgfmuls_x87(void) {
    gen("fmulp\tst(1),st");
}

/* Double multiplication */
static void m86_cgfmuld_x87(void) {
    gen("fmulp\tst(1),st");
}

/* Float division: ST0 = ST1 / ST0, pop ST1 */
static void m86_cgfdivs_x87(void) {
    gen("fdivrp\tst(1),st");  /* reverse divide: ST1 / ST0 */
}

/* Double division */
static void m86_cgfdivd_x87(void) {
    gen("fdivrp\tst(1),st");
}

/* Float negation: ST0 = -ST0 */
static void m86_cgfnegs_x87(void) {
    gen("fchs");
}

/* Double negation */
static void m86_cgfnegd_x87(void) {
    gen("fchs");
}

/* Compare floats and set CPU flags (requires FNSTSW/SAHF) */
static void m86_cgfcmps_x87(void) {
    gen("fcompp");           /* Compare ST0 with ST1, pop both */
    gen("fnstsw\tax");       /* Store FPU status word to AX */
    gen("sahf");             /* Store AH into CPU flags */
}

/* Compare doubles */
static void m86_cgfcmpd_x87(void) {
    gen("fcompp");
    gen("fnstsw\tax");
    gen("sahf");
}

/* Float == comparison, result in AX (0 or 1) */
static void m86_cgfeqs_x87(void) {
    int lab = label();
    m86_cgfcmps_x87();
    gen("xor\tax,ax");
    lgen("%s\t%c%d", "jne", lab);
    gen("inc\tax");
    genlab(lab);
}

static void m86_cgfeqd_x87(void) {
    int lab = label();
    m86_cgfcmpd_x87();
    gen("xor\tax,ax");
    lgen("%s\t%c%d", "jne", lab);
    gen("inc\tax");
    genlab(lab);
}

/* Float != comparison */
static void m86_cgfnes_x87(void) {
    int lab = label();
    m86_cgfcmps_x87();
    gen("xor\tax,ax");
    lgen("%s\t%c%d", "je", lab);
    gen("inc\tax");
    genlab(lab);
}

static void m86_cgfned_x87(void) {
    int lab = label();
    m86_cgfcmpd_x87();
    gen("xor\tax,ax");
    lgen("%s\t%c%d", "je", lab);
    gen("inc\tax");
    genlab(lab);
}

/* Float < comparison */
static void m86_cgflts_x87(void) {
    int lab = label();
    m86_cgfcmps_x87();
    gen("xor\tax,ax");
    lgen("%s\t%c%d", "jae", lab);  /* not below = not less than */
    gen("inc\tax");
    genlab(lab);
}

static void m86_cgfltd_x87(void) {
    int lab = label();
    m86_cgfcmpd_x87();
    gen("xor\tax,ax");
    lgen("%s\t%c%d", "jae", lab);
    gen("inc\tax");
    genlab(lab);
}

/* Float > comparison */
static void m86_cgfgts_x87(void) {
    int lab = label();
    m86_cgfcmps_x87();
    gen("xor\tax,ax");
    lgen("%s\t%c%d", "jbe", lab);  /* not above = not greater than */
    gen("inc\tax");
    genlab(lab);
}

static void m86_cgfgtd_x87(void) {
    int lab = label();
    m86_cgfcmpd_x87();
    gen("xor\tax,ax");
    lgen("%s\t%c%d", "jbe", lab);
    gen("inc\tax");
    genlab(lab);
}

/* Float <= comparison */
static void m86_cgfles_x87(void) {
    int lab = label();
    m86_cgfcmps_x87();
    gen("xor\tax,ax");
    lgen("%s\t%c%d", "ja", lab);   /* above = greater than */
    gen("inc\tax");
    genlab(lab);
}

static void m86_cgfled_x87(void) {
    int lab = label();
    m86_cgfcmpd_x87();
    gen("xor\tax,ax");
    lgen("%s\t%c%d", "ja", lab);
    gen("inc\tax");
    genlab(lab);
}

/* Float >= comparison */
static void m86_cgfges_x87(void) {
    int lab = label();
    m86_cgfcmps_x87();
    gen("xor\tax,ax");
    lgen("%s\t%c%d", "jb", lab);   /* below = less than */
    gen("inc\tax");
    genlab(lab);
}

static void m86_cgfged_x87(void) {
    int lab = label();
    m86_cgfcmpd_x87();
    gen("xor\tax,ax");
    lgen("%s\t%c%d", "jb", lab);
    gen("inc\tax");
    genlab(lab);
}

/* Convert integer (in AX) to float in ST0 */
static void m86_cgitofs_x87(void) {
    gen("push\tax");         /* Push integer to stack */
    gen("fild\tword ptr [sp]"); /* Load integer to FPU */
    gen("add\tsp,2");        /* Clean up stack */
}

/* Convert integer to double in ST0 */
static void m86_cgitofd_x87(void) {
    gen("push\tax");
    gen("fild\tword ptr [sp]");
    gen("add\tsp,2");
}

/* Convert float in ST0 to integer in AX */
static void m86_cgftois_x87(void) {
    gen("sub\tsp,2");        /* Make room on stack */
    gen("fistp\tword ptr [sp]"); /* Store integer, pop FPU */
    gen("pop\tax");          /* Get result in AX */
}

/* Convert double in ST0 to integer in AX */
static void m86_cgftoid_x87(void) {
    gen("sub\tsp,2");
    gen("fistp\tword ptr [sp]");
    gen("pop\tax");
}

/* Convert float to double (no-op on x87, both are 80-bit internally) */
static void m86_cgstod_x87(void) {
    /* No operation needed - x87 uses 80-bit extended precision internally */
}

/* Convert double to float (no-op on x87) */
static void m86_cgdtos_x87(void) {
    /* No operation needed */
}

/* Push FP value (duplicate ST0) */
static void m86_cgfpush_x87(void) {
    gen("fld\tst(0)");       /* Duplicate ST0 */
}

/* Pop FP stack (discard ST0) */
static void m86_cgfpop_x87(void) {
    gen("fstp\tst(0)");      /* Pop and discard */
}

/* Exchange ST0 and ST1 */
static void m86_cgfxch_x87(void) {
    gen("fxch\tst(1)");
}

/* Define float constant in data section */
static void m86_cgdeffloat_x87(double v) {
    union { float f; unsigned int i; } u;
    u.f = (float)v;
    ngen("%s\t%u", "dd", u.i);
}

/* Define double constant in data section */
static void m86_cgdefdouble_x87(double v) {
    union { double d; unsigned int i[2]; } u;
    u.d = v;
    ngen("%s\t%u", "dd", u.i[0]);    /* Low 32 bits first (little-endian) */
    ngen("%s\t%u", "dd", u.i[1]);    /* High 32 bits */
}

/*
 * ============================================================================
 * 8086 Software Floating-Point Emulation
 * ============================================================================
 *
 * For systems without an 8087 coprocessor, we generate calls to runtime
 * library functions that implement IEEE 754 arithmetic in software.
 *
 * Calling convention for FP emulation:
 *   - Float (32-bit): passed in DX:AX (high:low)
 *   - Double (64-bit): passed on stack (8 bytes)
 *   - Return value: same as arguments
 */

/* Load float from local variable to DX:AX */
static void m86_cgfloads_emu(int n) {
    ngen("%s\tax,[bp%+d]", "mov", n);
    ngen("%s\tdx,[bp%+d]", "mov", n+2);
}

/* Load double from local - push 8 bytes to stack for operations */
static void m86_cgfloadd_emu(int n) {
    /* For emulation, we keep doubles on the CPU stack */
    ngen("%s\tax,[bp%+d]", "mov", n+6);
    gen("push\tax");
    ngen("%s\tax,[bp%+d]", "mov", n+4);
    gen("push\tax");
    ngen("%s\tax,[bp%+d]", "mov", n+2);
    gen("push\tax");
    ngen("%s\tax,[bp%+d]", "mov", n);
    gen("push\tax");
}

/* Load float from global symbol */
static void m86_cgfloadgs_emu(char *s) {
    sgen("%s\tax,%s", "mov", s);
    sgen("%s\tdx,%s+2", "mov", s);
}

/* Load double from global symbol */
static void m86_cgfloadgd_emu(char *s) {
    sgen("%s\tax,%s+6", "mov", s);
    gen("push\tax");
    sgen("%s\tax,%s+4", "mov", s);
    gen("push\tax");
    sgen("%s\tax,%s+2", "mov", s);
    gen("push\tax");
    sgen("%s\tax,%s", "mov", s);
    gen("push\tax");
}

/* Store DX:AX to local float variable */
static void m86_cgfstores_emu(int n) {
    ngen("%s\t[bp%+d],ax", "mov", n);
    ngen("%s\t[bp%+d],dx", "mov", n+2);
}

/* Store double from stack to local variable */
static void m86_cgfstored_emu(int n) {
    gen("pop\tax");
    ngen("%s\t[bp%+d],ax", "mov", n);
    gen("pop\tax");
    ngen("%s\t[bp%+d],ax", "mov", n+2);
    gen("pop\tax");
    ngen("%s\t[bp%+d],ax", "mov", n+4);
    gen("pop\tax");
    ngen("%s\t[bp%+d],ax", "mov", n+6);
}

/* Store float to global symbol */
static void m86_cgfstoregs_emu(char *s) {
    sgen("%s\t%s,ax", "mov", s);
    sgen("%s\t%s+2,dx", "mov", s);
}

/* Store double to global symbol */
static void m86_cgfstoregsd_emu(char *s) {
    gen("pop\tax");
    sgen("%s\t%s,ax", "mov", s);
    gen("pop\tax");
    sgen("%s\t%s+2,ax", "mov", s);
    gen("pop\tax");
    sgen("%s\t%s+4,ax", "mov", s);
    gen("pop\tax");
    sgen("%s\t%s+6,ax", "mov", s);
}

/* Load float literal */
static void m86_cgflits_emu(int lab) {
    lgen("%s\tax,%c%d", "mov", lab);
    lgen("%s\tdx,%c%d+2", "mov", lab);
}

/* Load double literal */
static void m86_cgflitd_emu(int lab) {
    lgen("%s\tax,%c%d+6", "mov", lab);
    gen("push\tax");
    lgen("%s\tax,%c%d+4", "mov", lab);
    gen("push\tax");
    lgen("%s\tax,%c%d+2", "mov", lab);
    gen("push\tax");
    lgen("%s\tax,%c%d", "mov", lab);
    gen("push\tax");
}

/* Float addition via library call */
static void m86_cgfadds_emu(void) {
    gen("call\t__fpadd");    /* Args in DX:AX and CX:BX, result in DX:AX */
}

static void m86_cgfaddd_emu(void) {
    gen("call\t__dpadd");    /* Args on stack, result on stack */
}

static void m86_cgfsubs_emu(void) {
    gen("call\t__fpsub");
}

static void m86_cgfsubd_emu(void) {
    gen("call\t__dpsub");
}

static void m86_cgfmuls_emu(void) {
    gen("call\t__fpmul");
}

static void m86_cgfmuld_emu(void) {
    gen("call\t__dpmul");
}

static void m86_cgfdivs_emu(void) {
    gen("call\t__fpdiv");
}

static void m86_cgfdivd_emu(void) {
    gen("call\t__dpdiv");
}

static void m86_cgfnegs_emu(void) {
    gen("call\t__fpneg");
}

static void m86_cgfnegd_emu(void) {
    gen("call\t__dpneg");
}

static void m86_cgfcmps_emu(void) {
    gen("call\t__fpcmp");    /* Result in flags */
}

static void m86_cgfcmpd_emu(void) {
    gen("call\t__dpcmp");
}

static void m86_cgfeqs_emu(void) {
    gen("call\t__fpeq");     /* Result 0 or 1 in AX */
}

static void m86_cgfeqd_emu(void) {
    gen("call\t__dpeq");
}

static void m86_cgfnes_emu(void) {
    gen("call\t__fpne");
}

static void m86_cgfned_emu(void) {
    gen("call\t__dpne");
}

static void m86_cgflts_emu(void) {
    gen("call\t__fplt");
}

static void m86_cgfltd_emu(void) {
    gen("call\t__dplt");
}

static void m86_cgfgts_emu(void) {
    gen("call\t__fpgt");
}

static void m86_cgfgtd_emu(void) {
    gen("call\t__dpgt");
}

static void m86_cgfles_emu(void) {
    gen("call\t__fple");
}

static void m86_cgfled_emu(void) {
    gen("call\t__dple");
}

static void m86_cgfges_emu(void) {
    gen("call\t__fpge");
}

static void m86_cgfged_emu(void) {
    gen("call\t__dpge");
}

static void m86_cgitofs_emu(void) {
    gen("call\t__itof");     /* AX -> DX:AX */
}

static void m86_cgitofd_emu(void) {
    gen("call\t__itod");     /* AX -> 8 bytes on stack */
}

static void m86_cgftois_emu(void) {
    gen("call\t__ftoi");     /* DX:AX -> AX */
}

static void m86_cgftoid_emu(void) {
    gen("call\t__dtoi");     /* 8 bytes on stack -> AX */
}

static void m86_cgstod_emu(void) {
    gen("call\t__stod");     /* DX:AX -> 8 bytes on stack */
}

static void m86_cgdtos_emu(void) {
    gen("call\t__dtos");     /* 8 bytes on stack -> DX:AX */
}

/* Push float (DX:AX) to stack */
static void m86_cgfpush_emu(void) {
    gen("push\tdx");
    gen("push\tax");
}

/* Pop float from stack to DX:AX */
static void m86_cgfpop_emu(void) {
    gen("pop\tax");
    gen("pop\tdx");
}

/* Exchange - for emulation, swap DX:AX with CX:BX */
static void m86_cgfxch_emu(void) {
    gen("xchg\tax,bx");
    gen("xchg\tdx,cx");
}

/* Define float constant (same for both x87 and emulated) */
static void m86_cgdeffloat_emu(double v) {
    union { float f; unsigned int i; } u;
    u.f = (float)v;
    ngen("%s\t%u", "dd", u.i);
}

/* Define double constant */
static void m86_cgdefdouble_emu(double v) {
    union { double d; unsigned int i[2]; } u;
    u.d = v;
    ngen("%s\t%u", "dd", u.i[0]);
    ngen("%s\t%u", "dd", u.i[1]);
}

/*
 * ============================================================================
 * 8086 cdecl Calling Convention Support
 * ============================================================================
 *
 * 8086 cdecl passes ALL arguments on the stack (right to left).
 * No registers are used for argument passing.
 */

/* cgpusharg - For cdecl, all args go to stack (same as cgpush) */
static void m86_cgpusharg(int argnum) {
    (void)argnum;  /* unused - all args go to stack */
    gen("push\tax");
}

/* cgcallprep - No special preparation needed for cdecl */
static void m86_cgcallprep(int nargs) {
    (void)nargs;  /* unused */
}

/* cgcallend - Clean up stack after call */
static void m86_cgcallend(int nargs) {
    if (nargs > 0) {
        ngen("%s\tsp,%d", "add", nargs * 2);
    }
}

/* cgfnentry - Standard function entry (same as cgentry for cdecl) */
static void m86_cgfnentry(int nparams) {
    (void)nparams;  /* unused - params already on stack */
    gen("push\tbp");
    gen("mov\tbp,sp");
}

/*
 * ============================================================================
 * Stack Frame Layout Functions
 * ============================================================================
 */

static struct cg_frame_info m86_frame_info;

/*
 * Get frame layout information for 8086.
 * All parameters are on the stack (cdecl calling convention).
 * Stack layout:
 *   bp+4:  first parameter
 *   bp+6:  second parameter
 *   ...
 *   bp+0:  saved bp
 *   bp-2:  first local variable
 *   bp-4:  second local variable
 *   ...
 *
 * Uses CG->arch macros for consistency across the codebase.
 */
static struct cg_frame_info *m86_cggetframeinfo(int nparams) {
    (void)nparams;
    
    /* Parameters at positive offsets from bp */
    m86_frame_info.param_base = CG_PARAM_OFFSET_BASE;
    m86_frame_info.param_dir = CG_PARAM_OFFSET_DIR;
    
    /* Locals at negative offsets from bp */
    m86_frame_info.local_base = CG_LOCAL_OFFSET_BASE;
    m86_frame_info.local_dir = CG_LOCAL_OFFSET_DIR;
    
    m86_frame_info.stack_align = CG_STACK_ALIGN;
    m86_frame_info.num_reg_args = CG_NUM_ARG_REGS;  /* cdecl: 0 args in regs */
    m86_frame_info.stack_arg_base = CG_PARAM_OFFSET_BASE;
    
    return &m86_frame_info;
}

/*
 * Calculate offset for parameter N (0-based).
 * All params on stack at bp+4, bp+6, bp+8, ... (BPW spacing)
 *
 * Uses CG->arch macros for consistency.
 */
static int m86_cgparamoffset(int paramnum, int nparams) {
    (void)nparams;
    /* cdecl: all args on stack with BPW spacing */
    return CG_PARAM_OFFSET_BASE + paramnum * BPW;
}

/*
 * Calculate offset for local variable.
 * Locals grow downward from bp.
 */
static int m86_cglocaloffset(int size, int current_offset) {
    /* Align size to 2 bytes */
    int aligned_size = (size + 1) & ~1;
    return current_offset - aligned_size;
}

/*
 * Align local variable offset.
 */
static int m86_cgalignlocal(int offset, int size) {
    int aligned_size = (size + 1) & ~1;
    return (offset - aligned_size + 1) & ~1;
}

/*
 * ============================================================================
 * 8086 Architecture Description
 * ============================================================================
 */
/* 8086 with software FP emulation (no coprocessor required) */
struct cg_arch cg_arch_8086 = {
    "8086",             /* name */
    16,                 /* bits */
    1,                  /* char_size */
    2,                  /* short_size */
    2,                  /* int_size */
    4,                  /* long_size (32-bit on 16-bit arch) */
    2,                  /* ptr_size */
    4,                  /* float_size */
    8,                  /* double_size */
    2,                  /* bpw */
    ENDIAN_LITTLE,      /* endian */
    STACK_DOWN,         /* stack_dir */
    ASM_TASM,           /* asm_syntax */
    CC_CDECL,           /* call_conv */
    FLOAT_IEEE754,      /* float_format - IEEE 754 via emulation */
    FPU_EMULATED,       /* fpu_type - software emulation */
    2,                  /* align_stack */
    2,                  /* align_data */
    2,                  /* align_func */
    1,                  /* has_mul */
    1,                  /* has_div */
    1,                  /* has_mod */
    1,                  /* has_byte_ops */
    0,                  /* needs_alignment */
    4,                  /* param_offset_base (return addr + saved bp) */
    1,                  /* param_offset_dir (positive: 4, 6, 8...) */
    0,                  /* local_offset_base */
    -1,                 /* local_offset_dir (negative: -2, -4, -6...) */
    0,                  /* num_arg_regs (cdecl: all args on stack) */
    NULL,               /* symbol_transform (use default) */
    /* Compiler limits - reduced for 16-bit memory constraints */
    31,                 /* namelen (TASM limit) */
    256,                /* max_case (reduced from 1024) */
    32,                 /* max_break (reduced from 64) */
    64,                 /* max_locinit (reduced from 128) */
    0,                  /* max_fnargs (default: 127) */
    512,                /* nsymbols (reduced from 1024) */
    8192,               /* poolsize (reduced from 16384) */
    2048,               /* nodepoolsz (reduced from 4096) */
    0                   /* label_prefix (default: 'L') */
};

/* 8086 with x87 coprocessor (8087/80287) */
struct cg_arch cg_arch_8086_x87 = {
    "8086-x87",         /* name */
    16,                 /* bits */
    1,                  /* char_size */
    2,                  /* short_size */
    2,                  /* int_size */
    4,                  /* long_size (32-bit on 16-bit arch) */
    2,                  /* ptr_size */
    4,                  /* float_size */
    8,                  /* double_size */
    2,                  /* bpw */
    ENDIAN_LITTLE,      /* endian */
    STACK_DOWN,         /* stack_dir */
    ASM_TASM,           /* asm_syntax */
    CC_CDECL,           /* call_conv */
    FLOAT_IEEE754,      /* float_format - IEEE 754 native */
    FPU_X87,            /* fpu_type - x87 coprocessor */
    2,                  /* align_stack */
    2,                  /* align_data */
    2,                  /* align_func */
    1,                  /* has_mul */
    1,                  /* has_div */
    1,                  /* has_mod */
    1,                  /* has_byte_ops */
    0,                  /* needs_alignment */
    4,                  /* param_offset_base (return addr + saved bp) */
    1,                  /* param_offset_dir (positive: 4, 6, 8...) */
    0,                  /* local_offset_base */
    -1,                 /* local_offset_dir (negative: -2, -4, -6...) */
    0,                  /* num_arg_regs (cdecl: all args on stack) */
    NULL,               /* symbol_transform (use default) */
    /* Compiler limits - reduced for 16-bit memory constraints */
    31,                 /* namelen (TASM limit) */
    256,                /* max_case (reduced from 1024) */
    32,                 /* max_break (reduced from 64) */
    64,                 /* max_locinit (reduced from 128) */
    0,                  /* max_fnargs (default: 127) */
    512,                /* nsymbols (reduced from 1024) */
    8192,               /* poolsize (reduced from 16384) */
    2048,               /* nodepoolsz (reduced from 4096) */
    0                   /* label_prefix (default: 'L') */
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
    
    /* ABI-Compliant Calling Convention */
    m86_cgpusharg,
    NULL,  /* cgpusharg_vararg - use default (8086 cdecl uses stack) */
    m86_cgcallprep,
    m86_cgcallend,
    m86_cgfnentry,
    
    /* Stack Frame Layout */
    m86_cggetframeinfo,
    m86_cgparamoffset,
    m86_cglocaloffset,
    m86_cgalignlocal,
    
    /* Data Definition */
    m86_cgdefb,
    m86_cgdefh,
    m86_cgdefw,
    m86_cgdefd,
    m86_cgdefp,
    m86_cgdefl,
    m86_cgdefc,
    m86_cgdefq,
    m86_cggbss,
    m86_cglbss,
    
    /* Alignment */
    m86_cgalign,
    
    /* Floating-Point Operations - Software Emulation */
    m86_cgfloads_emu,
    m86_cgfloadd_emu,
    m86_cgfloadgs_emu,
    m86_cgfloadgd_emu,
    m86_cgfstores_emu,
    m86_cgfstored_emu,
    m86_cgfstoregs_emu,
    m86_cgfstoregsd_emu,
    m86_cgflits_emu,
    m86_cgflitd_emu,
    m86_cgfadds_emu,
    m86_cgfaddd_emu,
    m86_cgfsubs_emu,
    m86_cgfsubd_emu,
    m86_cgfmuls_emu,
    m86_cgfmuld_emu,
    m86_cgfdivs_emu,
    m86_cgfdivd_emu,
    m86_cgfnegs_emu,
    m86_cgfnegd_emu,
    m86_cgfcmps_emu,
    m86_cgfcmpd_emu,
    m86_cgfeqs_emu,
    m86_cgfeqd_emu,
    m86_cgfnes_emu,
    m86_cgfned_emu,
    m86_cgflts_emu,
    m86_cgfltd_emu,
    m86_cgfgts_emu,
    m86_cgfgtd_emu,
    m86_cgfles_emu,
    m86_cgfled_emu,
    m86_cgfges_emu,
    m86_cgfged_emu,
    m86_cgitofs_emu,
    m86_cgitofd_emu,
    m86_cgftois_emu,
    m86_cgftoid_emu,
    m86_cgstod_emu,
    m86_cgdtos_emu,
    m86_cgfpush_emu,
    m86_cgfpop_emu,
    m86_cgfxch_emu,
    m86_cgdeffloat_emu,
    m86_cgdefdouble_emu
};

/* Vtable for 8086 with x87 coprocessor */
struct cg_vtable cg_vtable_8086_x87 = {
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
    
    /* ABI-Compliant Calling Convention */
    m86_cgpusharg,
    NULL,  /* cgpusharg_vararg - use default (8086 cdecl uses stack) */
    m86_cgcallprep,
    m86_cgcallend,
    m86_cgfnentry,
    
    /* Stack Frame Layout */
    m86_cggetframeinfo,
    m86_cgparamoffset,
    m86_cglocaloffset,
    m86_cgalignlocal,
    
    /* Data Definition */
    m86_cgdefb,
    m86_cgdefh,
    m86_cgdefw,
    m86_cgdefd,
    m86_cgdefp,
    m86_cgdefl,
    m86_cgdefc,
    m86_cgdefq,
    m86_cggbss,
    m86_cglbss,
    
    /* Alignment */
    m86_cgalign,
    
    /* Floating-Point Operations - x87 Coprocessor */
    m86_cgfloads_x87,
    m86_cgfloadd_x87,
    m86_cgfloadgs_x87,
    m86_cgfloadgd_x87,
    m86_cgfstores_x87,
    m86_cgfstored_x87,
    m86_cgfstoregs_x87,
    m86_cgfstoregsd_x87,
    m86_cgflits_x87,
    m86_cgflitd_x87,
    m86_cgfadds_x87,
    m86_cgfaddd_x87,
    m86_cgfsubs_x87,
    m86_cgfsubd_x87,
    m86_cgfmuls_x87,
    m86_cgfmuld_x87,
    m86_cgfdivs_x87,
    m86_cgfdivd_x87,
    m86_cgfnegs_x87,
    m86_cgfnegd_x87,
    m86_cgfcmps_x87,
    m86_cgfcmpd_x87,
    m86_cgfeqs_x87,
    m86_cgfeqd_x87,
    m86_cgfnes_x87,
    m86_cgfned_x87,
    m86_cgflts_x87,
    m86_cgfltd_x87,
    m86_cgfgts_x87,
    m86_cgfgtd_x87,
    m86_cgfles_x87,
    m86_cgfled_x87,
    m86_cgfges_x87,
    m86_cgfged_x87,
    m86_cgitofs_x87,
    m86_cgitofd_x87,
    m86_cgftois_x87,
    m86_cgftoid_x87,
    m86_cgstod_x87,
    m86_cgdtos_x87,
    m86_cgfpush_x87,
    m86_cgfpop_x87,
    m86_cgfxch_x87,
    m86_cgdeffloat_x87,
    m86_cgdefdouble_x87
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
/* DOS 8086 with software FP emulation (default) */
struct cg_target cg_target_dos_8086 = {
    "dos-8086",
    "DOS 8086 (OMF, TASM syntax, FP emulation)",
    &cg_arch_8086,
    &cg_os_dos,
    &cg_vtable_8086,
    NULL,
    NULL
};

/* DOS 8086 with x87 coprocessor support */
struct cg_target cg_target_dos_8086_x87 = {
    "dos-8086-x87",
    "DOS 8086 with 8087 coprocessor (OMF, TASM syntax)",
    &cg_arch_8086_x87,
    &cg_os_dos,
    &cg_vtable_8086_x87,
    NULL,
    NULL
};
