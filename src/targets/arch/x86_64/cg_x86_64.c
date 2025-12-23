/*
 * SubC Compiler - x86-64 Code Generator
 *
 * This file implements the code generator for x86-64 architecture.
 * It provides the vtable, architecture description, and all code
 * generation functions for x86-64 targets.
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
static void x64_cgsynth(char *op);
static void x64_cgcmp(char *inst);
static void x64_cgbrcond(char *i, int n);
static void x64_cgbr(char *how, int n);
static int  x64_cgload2(void);
static void x64_cgclear2(void);
static void x64_cgpop2(void);

/*
 * ============================================================================
 * Code Generation Function Implementations
 * ============================================================================
 */

static void x64_cgdata(void)        { gen(".data"); }
static void x64_cgtext(void)        { gen(".text"); }
static void x64_cgprelude(void)     { }
static void x64_cgpostlude(void)    { }
static void x64_cgpublic(char *s)   { ngen(".globl\t%s", s, 0); }

static void x64_cgsynth(char *op) {
    int n;
    char *s;

    n = Q_val;
    s = gsym(Q_name);
    switch (Q_type) {
    case addr_auto:     ngen("%s\t%d(%%rbp),%%rcx", "leaq", n);
                        sgen("%s\t%s,%%rax", op, "%rcx");
                        break;
    case addr_static:   lgen("%s\t$%c%d,%%rax", op, n); break;
    case addr_globl:    sgen("%s\t$%s,%%rax", op, s); break;
    case addr_label:    lgen("%s\t$%c%d,%%rax", op, n); break;
    case literal:       ngen("%s\t$%d,%%rax", op, n); break;
    case auto_word:     ngen("%s\t%d(%%rbp),%%rax", op, n); break;
    case static_word:   lgen("%s\t%c%d,%%rax", op, n); break;
    case globl_word:    sgen("%s\t%s,%%rax", op, s); break;
    case auto_byte:
    case static_byte:
    case globl_byte:    x64_cgload2();
                        ngen("%s\t%%rcx,%%rax", op, 0);
                        break;
    case empty:         x64_cgpop2();
                        sgen("%s\t%s,%%rax", op, "%rcx");
                        break;
    default:            fatal("internal: bad type in cgsynth()");
    }
    Q_type = empty;
}

static int x64_cgload2(void) {
    int n, q;
    char *s, *op, *opb;

    op = "movq";
    opb = "movb";
    n = Q_val;
    s = gsym(Q_name);
    switch (Q_type) {
    case addr_auto:     ngen("%s\t%d(%%rbp),%%rcx", "leaq", n);
                        break;
    case addr_static:   lgen("%s\t$%c%d,%%rcx", op, n); break;
    case addr_globl:    sgen("%s\t$%s,%%rcx", op, s); break;
    case addr_label:    lgen("%s\t$%c%d,%%rcx", op, n); break;
    case literal:       ngen("%s\t$%d,%%rcx", op, n); break;
    case auto_byte:     x64_cgclear2();
                        ngen("%s\t%d(%%rbp),%%cl", opb, n);
                        break;
    case auto_word:     ngen("%s\t%d(%%rbp),%%rcx", op, n); break;
    case static_byte:   x64_cgclear2();
                        lgen("%s\t%c%d,%%cl", opb, n);
                        break;
    case static_word:   lgen("%s\t%c%d,%%rcx", op, n); break;
    case globl_byte:    x64_cgclear2();
                        sgen("%s\t%s,%%cl", opb, s);
                        break;
    case globl_word:    sgen("%s\t%s,%%rcx", op, s); break;
    case empty:         x64_cgpop2();
                        break;
    default:            fatal("internal: bad type in cgload2()");
    }
    q = Q_type;
    Q_type = empty;
    return empty == q;
}

static void x64_cglit(long v)       { ngen("%s\t$%ld,%%rax", "movq", v); }
static void x64_cgclear(void)       { gen("xorq\t%rax,%rax"); }
static void x64_cgclear2(void)      { gen("xorq\t%rcx,%rcx"); }
static void x64_cgldgb(char *s)     { sgen("%s\t%s,%%al", "movb", s); }
static void x64_cgldgw(char *s)     { sgen("%s\t%s,%%rax", "movq", s); }
static void x64_cgldlb(int n)       { ngen("%s\t%d(%%rbp),%%al", "movb", n); }
static void x64_cgldlw(int n)       { ngen("%s\t%d(%%rbp),%%rax", "movq", n); }
static void x64_cgldsb(int n)       { lgen("%s\t%c%d,%%al", "movb", n); }
static void x64_cgldsw(int n)       { lgen("%s\t%c%d,%%rax", "movq", n); }
static void x64_cgldla(int n)       { ngen("%s\t%d(%%rbp),%%rax", "leaq", n); }
static void x64_cgldsa(int n)       { lgen("%s\t$%c%d,%%rax", "movq", n); }
static void x64_cgldga(char *s)     { sgen("%s\t$%s,%%rax", "movq", s); }

static void x64_cgindb(void) {
    gen("movq\t%rax,%rdx");
    x64_cgclear();
    gen("movb\t(%rdx),%al");
}

static void x64_cgindw(void)        { gen("movq\t(%rax),%rax"); }
static void x64_cgldlab(int id)     { lgen("%s\t$%c%d,%%rax", "movq", id); }

static void x64_cgpush(void)        { gen("pushq\t%rax"); }
static void x64_cgpushlit(int n)    { ngen("%s\t$%d", "pushq", n); }
static void x64_cgpop2(void)        { gen("popq\t%rcx"); }
static void x64_cgswap(void)        { gen("xchgq\t%rax,%rcx"); }
static void x64_cgpopptr(void)      { gen("popq\t%rdx"); }

static void x64_cgand(void)         { x64_cgsynth("andq"); }
static void x64_cgior(void)         { x64_cgsynth("orq"); }
static void x64_cgxor(void)         { x64_cgsynth("xorq"); }
static void x64_cgadd(void)         { gen("addq\t%rcx,%rax"); }
static void x64_cgmul(void)         { gen("imulq\t%rcx,%rax"); }
static void x64_cgsub(void)         { gen("subq\t%rcx,%rax"); }

static void x64_cgdiv(void) {
    gen("cqo");
    gen("idivq\t%rcx");
}

static void x64_cgmod(void) {
    x64_cgdiv();
    gen("movq\t%rdx,%rax");
}

static void x64_cgshl(void)         { gen("shlq\t%cl,%rax"); }
static void x64_cgshr(void)         { gen("sarq\t%cl,%rax"); }

static void x64_cgcmp(char *inst) {
    int lab;
    lab = label();
    gen("xorq\t%rdx,%rdx");
    if (empty == Q_type) {
        x64_cgpop2();
        gen("cmpq\t%rax,%rcx");
    }
    else {
        x64_cgsynth("cmpq");
    }
    lgen("%s\t%c%d", inst, lab);
    gen("incq\t%rdx");
    genlab(lab);
    gen("movq\t%rdx,%rax");
}

static void x64_cgeq(void)          { x64_cgcmp("jne"); }
static void x64_cgne(void)          { x64_cgcmp("je"); }
static void x64_cglt(void)          { x64_cgcmp("jge"); }
static void x64_cggt(void)          { x64_cgcmp("jle"); }
static void x64_cgle(void)          { x64_cgcmp("jg"); }
static void x64_cgge(void)          { x64_cgcmp("jl"); }
static void x64_cgult(void)         { x64_cgcmp("jae"); }
static void x64_cgugt(void)         { x64_cgcmp("jbe"); }
static void x64_cgule(void)         { x64_cgcmp("ja"); }
static void x64_cguge(void)         { x64_cgcmp("jb"); }

static void x64_cgbrcond(char *i, int n) {
    int lab;
    lab = label();
    if (empty == Q_type) {
        x64_cgpop2();
        gen("cmpq\t%rax,%rcx");
    }
    else {
        x64_cgsynth("cmpq");
    }
    lgen("%s\t%c%d", i, lab);
    lgen("%s\t%c%d", "jmp", n);
    genlab(lab);
}

static void x64_cgbreq(int n)       { x64_cgbrcond("je", n); }
static void x64_cgbrne(int n)       { x64_cgbrcond("jne", n); }
static void x64_cgbrlt(int n)       { x64_cgbrcond("jl", n); }
static void x64_cgbrgt(int n)       { x64_cgbrcond("jg", n); }
static void x64_cgbrle(int n)       { x64_cgbrcond("jle", n); }
static void x64_cgbrge(int n)       { x64_cgbrcond("jge", n); }
static void x64_cgbrult(int n)      { x64_cgbrcond("jb", n); }
static void x64_cgbrugt(int n)      { x64_cgbrcond("ja", n); }
static void x64_cgbrule(int n)      { x64_cgbrcond("jbe", n); }
static void x64_cgbruge(int n)      { x64_cgbrcond("jae", n); }

static void x64_cgneg(void)         { gen("negq\t%rax"); }
static void x64_cgnot(void)         { gen("notq\t%rax"); }

static void x64_cglognot(void) {
    gen("negq\t%rax");
    gen("sbbq\t%rax,%rax");
    gen("incq\t%rax");
}

static void x64_cgscale(void)       { gen("shlq\t$3,%rax"); }
static void x64_cgscale2(void)      { gen("shlq\t$3,%rcx"); }
static void x64_cgunscale(void)     { gen("shrq\t$3,%rax"); }

static void x64_cgscaleby(int v) {
    ngen("%s\t$%d,%%rcx", "movq", v);
    gen("mulq\t%rcx");
}

static void x64_cgscale2by(int v) {
    gen("pushq\t%rax");
    ngen("%s\t$%d,%%rax", "movq", v);
    gen("mulq\t%rcx");
    gen("movq\t%rax,%rcx");
    gen("popq\t%rax");
}

static void x64_cgunscaleby(int v) {
    ngen("%s\t$%d,%%rcx", "movq", v);
    gen("xorq\t%rdx,%rdx");
    gen("divq\t%rcx");
}

static void x64_cgbool(void) {
    gen("negq\t%rax");
    gen("sbbq\t%rax,%rax");
    gen("negq\t%rax");
}

static void x64_cgldinc(void)       { gen("movq\t%rax,%rdx"); }
static void x64_cginc1pi(int v)     { ngen("%s\t$%d,(%%rax)", "addq", v); }
static void x64_cgdec1pi(int v)     { ngen("%s\t$%d,(%%rax)", "subq", v); }
static void x64_cginc2pi(int v)     { ngen("%s\t$%d,(%%rdx)", "addq", v); }
static void x64_cgdec2pi(int v)     { ngen("%s\t$%d,(%%rdx)", "subq", v); }
static void x64_cgincpl(int a, int v)   { ngen2("%s\t$%d,%d(%%rbp)", "addq", v, a); }
static void x64_cgdecpl(int a, int v)   { ngen2("%s\t$%d,%d(%%rbp)", "subq", v, a); }
static void x64_cgincps(int a, int v)   { lgen2("addq\t$%d,%c%d", v, a); }
static void x64_cgdecps(int a, int v)   { lgen2("subq\t$%d,%c%d", v, a); }
static void x64_cgincpg(char *s, int v) { sgen2("%s\t$%d,%s", "addq", v, s); }
static void x64_cgdecpg(char *s, int v) { sgen2("%s\t$%d,%s", "subq", v, s); }
static void x64_cginc1iw(void)      { ngen("%s\t(%%rax)", "incq", 0); }
static void x64_cgdec1iw(void)      { ngen("%s\t(%%rax)", "decq", 0); }
static void x64_cginc2iw(void)      { ngen("%s\t(%%rdx)", "incq", 0); }
static void x64_cgdec2iw(void)      { ngen("%s\t(%%rdx)", "decq", 0); }
static void x64_cginclw(int a)      { ngen("%s\t%d(%%rbp)", "incq", a); }
static void x64_cgdeclw(int a)      { ngen("%s\t%d(%%rbp)", "decq", a); }
static void x64_cgincsw(int a)      { lgen("%s\t%c%d", "incq", a); }
static void x64_cgdecsw(int a)      { lgen("%s\t%c%d", "decq", a); }
static void x64_cgincgw(char *s)    { sgen("%s\t%s", "incq", s); }
static void x64_cgdecgw(char *s)    { sgen("%s\t%s", "decq", s); }
static void x64_cginc1ib(void)      { ngen("%s\t(%%rax)", "incb", 0); }
static void x64_cgdec1ib(void)      { ngen("%s\t(%%rax)", "decb", 0); }
static void x64_cginc2ib(void)      { ngen("%s\t(%%rdx)", "incb", 0); }
static void x64_cgdec2ib(void)      { ngen("%s\t(%%rdx)", "decb", 0); }
static void x64_cginclb(int a)      { ngen("%s\t%d(%%rbp)", "incb", a); }
static void x64_cgdeclb(int a)      { ngen("%s\t%d(%%rbp)", "decb", a); }
static void x64_cgincsb(int a)      { lgen("%s\t%c%d", "incb", a); }
static void x64_cgdecsb(int a)      { lgen("%s\t%c%d", "decb", a); }
static void x64_cgincgb(char *s)    { sgen("%s\t%s", "incb", s); }
static void x64_cgdecgb(char *s)    { sgen("%s\t%s", "decb", s); }

static void x64_cgbr(char *how, int n) {
    int lab;
    lab = label();
    gen("orq\t%rax,%rax");
    lgen("%s\t%c%d", how, lab);
    lgen("%s\t%c%d", "jmp", n);
    genlab(lab);
}

static void x64_cgbrtrue(int n)     { x64_cgbr("jz", n); }
static void x64_cgbrfalse(int n)    { x64_cgbr("jnz", n); }
static void x64_cgjump(int n)       { lgen("%s\t%c%d", "jmp", n); }
static void x64_cgldswtch(int n)    { lgen("%s\t$%c%d,%%rdx", "movq", n); }
static void x64_cgcalswtch(void)    { gen("jmp\tswitch"); }
static void x64_cgcase(int v, int l) { lgen2(".quad\t%d,%c%d", v, l); }

static void x64_cgstorib(void)      { ngen("%s\t%%al,(%%rdx)", "movb", 0); }
static void x64_cgstoriw(void)      { ngen("%s\t%%rax,(%%rdx)", "movq", 0); }
static void x64_cgstorlb(int n)     { ngen("%s\t%%al,%d(%%rbp)", "movb", n); }
static void x64_cgstorlw(int n)     { ngen("%s\t%%rax,%d(%%rbp)", "movq", n); }
static void x64_cgstorsb(int n)     { lgen("%s\t%%al,%c%d", "movb", n); }
static void x64_cgstorsw(int n)     { lgen("%s\t%%rax,%c%d", "movq", n); }
static void x64_cgstorgb(char *s)   { sgen("%s\t%%al,%s", "movb", s); }
static void x64_cgstorgw(char *s)   { sgen("%s\t%%rax,%s", "movq", s); }

static void x64_cginitlw(int v, int a) { ngen2("%s\t$%d,%d(%%rbp)", "movq", v, a); }
static void x64_cgcall(char *s)     { sgen("%s\t%s", "call", s); }
static void x64_cgcalr(void)        { gen("call\t*%rax"); }
static void x64_cgstack(int n)      { ngen("%s\t$%d,%%rsp", "addq", n); }

static void x64_cgentry(void) {
    gen("pushq\t%rbp");
    gen("movq\t%rsp,%rbp");
}

static void x64_cgexit(void) {
    gen("popq\t%rbp");
    gen("ret");
}

static void x64_cgdefb(int v)       { ngen("%s\t%d", ".byte", v); }
static void x64_cgdefh(int v)       { ngen("%s\t%d", ".word", v); }   /* 2 bytes */
static void x64_cgdefw(int v)       { ngen("%s\t%d", ".quad", v); }   /* 8 bytes on x86-64 (native int) */
static void x64_cgdefd(int v)       { ngen("%s\t%d", ".long", v); }   /* 4 bytes */
static void x64_cgdefp(int v)       { ngen("%s\t%d", ".quad", v); }
static void x64_cgdefl(int v)       { lgen("%s\t%c%d", ".quad", v); }
static void x64_cgdefq(int v)       { ngen("%s\t%d", ".quad", v); }   /* 8 bytes */
static void x64_cgdefc(int c)       { ngen("%s\t'%c'", ".byte", c); }
static void x64_cggbss(char *s, int z) { ngen(".comm\t%s,%d", s, z); }
static void x64_cglbss(char *s, int z) { ngen(".lcomm\t%s,%d", s, z); }
static void x64_cgalign(void)       { /* unused */ }

/*
 * ============================================================================
 * x86-64 SSE2 Floating-Point Operations
 * ============================================================================
 *
 * x86-64 uses SSE/SSE2 for floating-point operations (mandatory on all x86-64).
 * SSE uses XMM registers (xmm0-xmm15), each 128 bits wide.
 * We use xmm0 as the primary FP accumulator and xmm1 as secondary.
 *
 * IEEE 754 format:
 *   float (32-bit):  1 sign + 8 exponent + 23 mantissa  (scalar: ss suffix)
 *   double (64-bit): 1 sign + 11 exponent + 52 mantissa (scalar: sd suffix)
 */

/* Load float from local variable to xmm0 */
static void x64_cgfloads(int n) {
    ngen("%s\t%d(%%rbp),%%xmm0", "movss", n);
}

/* Load double from local variable to xmm0 */
static void x64_cgfloadd(int n) {
    ngen("%s\t%d(%%rbp),%%xmm0", "movsd", n);
}

/* Load float from global symbol to xmm0 */
static void x64_cgfloadgs(char *s) {
    sgen("%s\t%s(%%rip),%%xmm0", "movss", s);
}

/* Load double from global symbol to xmm0 */
static void x64_cgfloadgd(char *s) {
    sgen("%s\t%s(%%rip),%%xmm0", "movsd", s);
}

/* Store xmm0 to local float variable */
static void x64_cgfstores(int n) {
    ngen("%s\t%%xmm0,%d(%%rbp)", "movss", n);
}

/* Store xmm0 to local double variable */
static void x64_cgfstored(int n) {
    ngen("%s\t%%xmm0,%d(%%rbp)", "movsd", n);
}

/* Store xmm0 to global float symbol */
static void x64_cgfstoregs(char *s) {
    sgen("%s\t%%xmm0,%s(%%rip)", "movss", s);
}

/* Store xmm0 to global double symbol */
static void x64_cgfstoregsd(char *s) {
    sgen("%s\t%%xmm0,%s(%%rip)", "movsd", s);
}

/* Load float literal from label */
static void x64_cgflits(int lab) {
    lgen("%s\t%c%d(%%rip),%%xmm0", "movss", lab);
}

/* Load double literal from label */
static void x64_cgflitd(int lab) {
    lgen("%s\t%c%d(%%rip),%%xmm0", "movsd", lab);
}

/* Float addition: xmm0 = xmm0 + xmm1 */
static void x64_cgfadds(void) {
    gen("addss\t%xmm1,%xmm0");
}

/* Double addition */
static void x64_cgfaddd(void) {
    gen("addsd\t%xmm1,%xmm0");
}

/* Float subtraction: xmm0 = xmm0 - xmm1 */
static void x64_cgfsubs(void) {
    gen("subss\t%xmm1,%xmm0");
}

/* Double subtraction */
static void x64_cgfsubd(void) {
    gen("subsd\t%xmm1,%xmm0");
}

/* Float multiplication */
static void x64_cgfmuls(void) {
    gen("mulss\t%xmm1,%xmm0");
}

/* Double multiplication */
static void x64_cgfmuld(void) {
    gen("mulsd\t%xmm1,%xmm0");
}

/* Float division */
static void x64_cgfdivs(void) {
    gen("divss\t%xmm1,%xmm0");
}

/* Double division */
static void x64_cgfdivd(void) {
    gen("divsd\t%xmm1,%xmm0");
}

/* Float negation: xmm0 = -xmm0 */
static void x64_cgfnegs(void) {
    /* XOR with sign bit mask (0x80000000) */
    gen("movd\t%xmm0,%eax");
    gen("xorl\t$0x80000000,%eax");
    gen("movd\t%eax,%xmm0");
}

/* Double negation */
static void x64_cgfnegd(void) {
    /* XOR with sign bit mask (0x8000000000000000) */
    gen("movq\t%xmm0,%rax");
    gen("movabsq\t$0x8000000000000000,%rcx");
    gen("xorq\t%rcx,%rax");
    gen("movq\t%rax,%xmm0");
}

/* Compare floats: xmm0 vs xmm1, set flags via COMISS */
static void x64_cgfcmps(void) {
    gen("comiss\t%xmm1,%xmm0");
}

/* Compare doubles */
static void x64_cgfcmpd(void) {
    gen("comisd\t%xmm1,%xmm0");
}

/* Float == comparison, result in RAX (0 or 1) */
static void x64_cgfeqs(void) {
    int lab = label();
    x64_cgfcmps();
    gen("xorq\t%rax,%rax");
    lgen("%s\t%c%d", "jne", lab);
    lgen("%s\t%c%d", "jp", lab);  /* Also check parity for NaN */
    gen("incq\t%rax");
    genlab(lab);
}

static void x64_cgfeqd(void) {
    int lab = label();
    x64_cgfcmpd();
    gen("xorq\t%rax,%rax");
    lgen("%s\t%c%d", "jne", lab);
    lgen("%s\t%c%d", "jp", lab);
    gen("incq\t%rax");
    genlab(lab);
}

/* Float != comparison */
static void x64_cgfnes(void) {
    int lab = label();
    x64_cgfcmps();
    gen("xorq\t%rax,%rax");
    lgen("%s\t%c%d", "je", lab);
    gen("incq\t%rax");
    genlab(lab);
}

static void x64_cgfned(void) {
    int lab = label();
    x64_cgfcmpd();
    gen("xorq\t%rax,%rax");
    lgen("%s\t%c%d", "je", lab);
    gen("incq\t%rax");
    genlab(lab);
}

/* Float < comparison */
static void x64_cgflts(void) {
    int lab = label();
    x64_cgfcmps();
    gen("xorq\t%rax,%rax");
    lgen("%s\t%c%d", "jae", lab);
    gen("incq\t%rax");
    genlab(lab);
}

static void x64_cgfltd(void) {
    int lab = label();
    x64_cgfcmpd();
    gen("xorq\t%rax,%rax");
    lgen("%s\t%c%d", "jae", lab);
    gen("incq\t%rax");
    genlab(lab);
}

/* Float > comparison */
static void x64_cgfgts(void) {
    int lab = label();
    x64_cgfcmps();
    gen("xorq\t%rax,%rax");
    lgen("%s\t%c%d", "jbe", lab);
    gen("incq\t%rax");
    genlab(lab);
}

static void x64_cgfgtd(void) {
    int lab = label();
    x64_cgfcmpd();
    gen("xorq\t%rax,%rax");
    lgen("%s\t%c%d", "jbe", lab);
    gen("incq\t%rax");
    genlab(lab);
}

/* Float <= comparison */
static void x64_cgfles(void) {
    int lab = label();
    x64_cgfcmps();
    gen("xorq\t%rax,%rax");
    lgen("%s\t%c%d", "ja", lab);
    gen("incq\t%rax");
    genlab(lab);
}

static void x64_cgfled(void) {
    int lab = label();
    x64_cgfcmpd();
    gen("xorq\t%rax,%rax");
    lgen("%s\t%c%d", "ja", lab);
    gen("incq\t%rax");
    genlab(lab);
}

/* Float >= comparison */
static void x64_cgfges(void) {
    int lab = label();
    x64_cgfcmps();
    gen("xorq\t%rax,%rax");
    lgen("%s\t%c%d", "jb", lab);
    gen("incq\t%rax");
    genlab(lab);
}

static void x64_cgfged(void) {
    int lab = label();
    x64_cgfcmpd();
    gen("xorq\t%rax,%rax");
    lgen("%s\t%c%d", "jb", lab);
    gen("incq\t%rax");
    genlab(lab);
}

/* Convert integer (in RAX) to float in xmm0 */
static void x64_cgitofs(void) {
    gen("cvtsi2ssq\t%rax,%xmm0");
}

/* Convert integer to double in xmm0 */
static void x64_cgitofd(void) {
    gen("cvtsi2sdq\t%rax,%xmm0");
}

/* Convert float in xmm0 to integer in RAX */
static void x64_cgftois(void) {
    gen("cvttss2siq\t%xmm0,%rax");
}

/* Convert double in xmm0 to integer in RAX */
static void x64_cgftoid(void) {
    gen("cvttsd2siq\t%xmm0,%rax");
}

/* Convert float to double */
static void x64_cgstod(void) {
    gen("cvtss2sd\t%xmm0,%xmm0");
}

/* Convert double to float */
static void x64_cgdtos(void) {
    gen("cvtsd2ss\t%xmm0,%xmm0");
}

/* Push FP value (copy xmm0 to xmm1 for binary ops) */
static void x64_cgfpush(void) {
    gen("movaps\t%xmm0,%xmm1");
}

/* Pop FP stack (no-op for SSE - values stay in registers) */
static void x64_cgfpop(void) {
    /* No operation needed */
}

/* Exchange xmm0 and xmm1 */
static void x64_cgfxch(void) {
    gen("movaps\t%xmm0,%xmm2");
    gen("movaps\t%xmm1,%xmm0");
    gen("movaps\t%xmm2,%xmm1");
}

/* Define float constant in data section */
static void x64_cgdeffloat(double v) {
    union { float f; unsigned int i; } u;
    u.f = (float)v;
    ngen("%s\t%u", ".long", u.i);
}

/* Define double constant in data section */
static void x64_cgdefdouble(double v) {
    union { double d; unsigned int i[2]; } u;
    u.d = v;
    ngen("%s\t%u", ".long", u.i[0]);
    ngen("%s\t%u", ".long", u.i[1]);
}

/*
 * ============================================================================
 * x86-64 System V ABI Calling Convention Support
 * ============================================================================
 *
 * System V AMD64 ABI uses the following registers for integer arguments:
 *   arg 0: %rdi
 *   arg 1: %rsi
 *   arg 2: %rdx
 *   arg 3: %rcx
 *   arg 4: %r8
 *   arg 5: %r9
 *   arg 6+: pushed on stack (right to left)
 *
 * Return value is in %rax.
 * Caller saves: %rax, %rcx, %rdx, %rsi, %rdi, %r8, %r9, %r10, %r11
 * Callee saves: %rbx, %rbp, %r12, %r13, %r14, %r15
 */

/* Argument register names for System V AMD64 ABI */
static char *x64_arg_regs[] = { "%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9" };

/*
 * cgpusharg - Move accumulator to argument position
 * For args 0-5: move to appropriate register
 * For args 6+: push to stack
 */
static void x64_cgpusharg(int argnum) {
    if (argnum < 6) {
        sgen("%s\t%%rax,%s", "movq", x64_arg_regs[argnum]);
    } else {
        gen("pushq\t%rax");
    }
}

/*
 * cgcallprep - Prepare for function call
 * Align stack to 16 bytes if needed for stack-passed arguments
 */
static void x64_cgcallprep(int nargs) {
    int stack_args;
    
    if (nargs <= 6) {
        stack_args = 0;
    } else {
        stack_args = nargs - 6;
        /* Ensure 16-byte stack alignment before call */
        if (stack_args & 1) {
            gen("subq\t$8,%rsp");
        }
    }
}

/*
 * cgcallend - Clean up after function call
 * Adjust stack pointer for stack-passed arguments only
 */
static void x64_cgcallend(int nargs) {
    int stack_args;
    int adjust;
    
    if (nargs <= 6) {
        return;  /* No stack cleanup needed */
    }
    
    stack_args = nargs - 6;
    adjust = stack_args * 8;
    
    /* Add alignment padding if we added it in cgcallprep */
    if (stack_args & 1) {
        adjust += 8;
    }
    
    ngen("%s\t$%d,%%rsp", "addq", adjust);
}

/*
 * cgfnentry - Function entry with parameter info
 * For variadic functions or functions with register args, save registers to stack
 * so they can be accessed via frame pointer offsets.
 *
 * Stack layout after cgfnentry for a function with N params (N <= 6):
 *   rbp-8:     saved rdi (arg 0)
 *   rbp-16:    saved rsi (arg 1)
 *   rbp-24:    saved rdx (arg 2)
 *   rbp-32:    saved rcx (arg 3)
 *   rbp-40:    saved r8  (arg 4)
 *   rbp-48:    saved r9  (arg 5)
 *   rbp-48-N:  local variables start here
 *
 * For variadic functions (nparams < 0), we save all 6 argument registers.
 */
static void x64_cgfnentry(int nparams) {
    int save_count;
    
    gen("pushq\t%rbp");
    gen("movq\t%rsp,%rbp");
    
    /* Determine how many registers to save */
    if (nparams < 0) {
        /* Variadic function: save all 6 argument registers */
        save_count = 6;
    } else if (nparams > 6) {
        save_count = 6;
    } else {
        save_count = nparams;
    }
    
    /* Save argument registers to stack */
    /* We save them in order so arg 0 is at rbp-8, arg 1 at rbp-16, etc. */
    if (save_count >= 1) gen("pushq\t%rdi");
    if (save_count >= 2) gen("pushq\t%rsi");
    if (save_count >= 3) gen("pushq\t%rdx");
    if (save_count >= 4) gen("pushq\t%rcx");
    if (save_count >= 5) gen("pushq\t%r8");
    if (save_count >= 6) gen("pushq\t%r9");
}

/*
 * ============================================================================
 * Stack Frame Layout Functions
 * ============================================================================
 */

static struct cg_frame_info x64_frame_info;

/*
 * Get frame layout information for x86-64.
 * Register args (0-5) are saved at negative offsets from rbp.
 * Stack args (6+) are at positive offsets from rbp.
 */
static struct cg_frame_info *x64_cggetframeinfo(int nparams) {
    int save_count;
    
    if (nparams < 0) {
        save_count = 6;  /* variadic: save all */
    } else if (nparams > 6) {
        save_count = 6;
    } else {
        save_count = nparams;
    }
    
    /* Register args saved at rbp-8, rbp-16, etc. */
    x64_frame_info.param_base = -8;
    x64_frame_info.param_dir = -1;  /* decreasing: -8, -16, -24... */
    
    /* Locals start after saved register args */
    x64_frame_info.local_base = -save_count * 8;
    x64_frame_info.local_dir = -1;  /* decreasing */
    
    x64_frame_info.stack_align = 16;
    x64_frame_info.num_reg_args = 6;
    x64_frame_info.stack_arg_base = 16;  /* first stack arg at rbp+16 */
    
    return &x64_frame_info;
}

/*
 * Calculate offset for parameter N.
 * Params 0-5: in registers, saved at rbp-8, rbp-16, ...
 * Params 6+: on stack at rbp+16, rbp+24, ...
 */
static int x64_cgparamoffset(int paramnum, int nparams) {
    (void)nparams;
    if (paramnum < 6) {
        return -8 * (paramnum + 1);  /* -8, -16, -24, -32, -40, -48 */
    } else {
        return 16 + (paramnum - 6) * 8;  /* +16, +24, +32, ... */
    }
}

/*
 * Calculate offset for local variable.
 * Locals grow downward from the end of saved register args.
 */
static int x64_cglocaloffset(int size, int current_offset) {
    /* Align size to 8 bytes */
    int aligned_size = (size + 7) & ~7;
    return current_offset - aligned_size;
}

/*
 * Align local variable offset.
 */
static int x64_cgalignlocal(int offset, int size) {
    int aligned_size = (size + 7) & ~7;
    return (offset - aligned_size + 1) & ~7;
}

/*
 * ============================================================================
 * Symbol Transform Functions
 * ============================================================================
 */

/* Darwin/macOS requires underscore prefix for C symbols */
static char *x64_darwin_symbol_transform(char *s) {
    static char name[NAMELEN+2];
    name[0] = '_';
    copyname(&name[1], s);
    return name;
}

/*
 * ============================================================================
 * x86-64 Architecture Description
 * ============================================================================
 */
struct cg_arch cg_arch_x86_64 = {
    "x86-64",           /* name */
    64,                 /* bits */
    1,                  /* char_size */
    2,                  /* short_size */
    8,                  /* int_size (SubC uses 64-bit int on x86-64) */
    8,                  /* long_size */
    8,                  /* ptr_size */
    4,                  /* float_size */
    8,                  /* double_size */
    8,                  /* bpw */
    ENDIAN_LITTLE,      /* endian */
    STACK_DOWN,         /* stack_dir */
    ASM_GAS,            /* asm_syntax */
    CC_SYSV_AMD64,      /* call_conv */
    FLOAT_IEEE754,      /* float_format */
    FPU_SSE,            /* fpu_type - SSE/SSE2 (standard on x86-64) */
    16,                 /* align_stack */
    8,                  /* align_data */
    16,                 /* align_func */
    1,                  /* has_mul */
    1,                  /* has_div */
    1,                  /* has_mod */
    1,                  /* has_byte_ops */
    0,                  /* needs_alignment */
    16,                 /* param_offset_base - NOT USED for register args */
    1,                  /* param_offset_dir */
    0,                  /* local_offset_base - adjusted by cgfnentry */
    -1,                 /* local_offset_dir (negative: -8, -16, -24...) */
    6,                  /* num_arg_regs (rdi, rsi, rdx, rcx, r8, r9) */
    NULL,               /* symbol_transform (use default) */
    /* Compiler limits - 0 means use defaults */
    0,                  /* namelen (default: 32) */
    0,                  /* max_case (default: 1024) */
    0,                  /* max_break (default: 64) */
    0,                  /* max_locinit (default: 128) */
    0,                  /* max_fnargs (default: 127) */
    0,                  /* nsymbols (default: 1024) */
    0,                  /* poolsize (default: 16384) */
    0,                  /* nodepoolsz (default: 4096) */
    0                   /* label_prefix (default: 'L') */
};

/* Darwin/macOS x86-64: same as x86-64 but with underscore prefix */
struct cg_arch cg_arch_x86_64_darwin = {
    "x86-64-darwin",    /* name */
    64,                 /* bits */
    1,                  /* char_size */
    2,                  /* short_size */
    8,                  /* int_size (SubC uses 64-bit int on x86-64) */
    8,                  /* long_size */
    8,                  /* ptr_size */
    4,                  /* float_size */
    8,                  /* double_size */
    8,                  /* bpw */
    ENDIAN_LITTLE,      /* endian */
    STACK_DOWN,         /* stack_dir */
    ASM_GAS,            /* asm_syntax */
    CC_SYSV_AMD64,      /* call_conv */
    FLOAT_IEEE754,      /* float_format */
    FPU_SSE,            /* fpu_type - SSE/SSE2 */
    16,                 /* align_stack */
    8,                  /* align_data */
    16,                 /* align_func */
    1,                  /* has_mul */
    1,                  /* has_div */
    1,                  /* has_mod */
    1,                  /* has_byte_ops */
    0,                  /* needs_alignment */
    16,                 /* param_offset_base - NOT USED for register args */
    1,                  /* param_offset_dir */
    0,                  /* local_offset_base - adjusted by cgfnentry */
    -1,                 /* local_offset_dir (negative: -8, -16, -24...) */
    6,                  /* num_arg_regs (rdi, rsi, rdx, rcx, r8, r9) */
    x64_darwin_symbol_transform,  /* symbol_transform (underscore prefix) */
    /* Compiler limits - 0 means use defaults */
    0,                  /* namelen (default: 32) */
    0,                  /* max_case (default: 1024) */
    0,                  /* max_break (default: 64) */
    0,                  /* max_locinit (default: 128) */
    0,                  /* max_fnargs (default: 127) */
    0,                  /* nsymbols (default: 1024) */
    0,                  /* poolsize (default: 16384) */
    0,                  /* nodepoolsz (default: 4096) */
    0                   /* label_prefix (default: 'L') */
};

/*
 * ============================================================================
 * x86-64 Code Generator Vtable
 * ============================================================================
 */
struct cg_vtable cg_vtable_x86_64 = {
    /* Section Control */
    x64_cgdata,
    x64_cgtext,
    x64_cgprelude,
    x64_cgpostlude,
    x64_cgpublic,
    
    /* Synthesizer Support */
    x64_cgsynth,
    x64_cgload2,
    
    /* Literal and Clear */
    x64_cglit,
    x64_cgclear,
    x64_cgclear2,
    
    /* Load Operations - Global */
    x64_cgldgb,
    x64_cgldgw,
    
    /* Load Operations - Local */
    x64_cgldlb,
    x64_cgldlw,
    
    /* Load Operations - Static */
    x64_cgldsb,
    x64_cgldsw,
    
    /* Load Address Operations */
    x64_cgldla,
    x64_cgldsa,
    x64_cgldga,
    
    /* Indirect Load Operations */
    x64_cgindb,
    x64_cgindw,
    
    /* Load Label */
    x64_cgldlab,
    
    /* Stack Operations */
    x64_cgpush,
    x64_cgpushlit,
    x64_cgpop2,
    x64_cgswap,
    x64_cgpopptr,
    
    /* Arithmetic Operations */
    x64_cgand,
    x64_cgior,
    x64_cgxor,
    x64_cgadd,
    x64_cgsub,
    x64_cgmul,
    x64_cgdiv,
    x64_cgmod,
    x64_cgshl,
    x64_cgshr,
    
    /* Unary Operations */
    x64_cgneg,
    x64_cgnot,
    x64_cglognot,
    
    /* Scaling Operations */
    x64_cgscale,
    x64_cgscale2,
    x64_cgunscale,
    x64_cgscaleby,
    x64_cgscale2by,
    x64_cgunscaleby,
    
    /* Comparison Operations */
    x64_cgeq,
    x64_cgne,
    x64_cglt,
    x64_cggt,
    x64_cgle,
    x64_cgge,
    x64_cgult,
    x64_cgugt,
    x64_cgule,
    x64_cguge,
    
    /* Conditional Branch Operations */
    x64_cgbreq,
    x64_cgbrne,
    x64_cgbrlt,
    x64_cgbrgt,
    x64_cgbrle,
    x64_cgbrge,
    x64_cgbrult,
    x64_cgbrugt,
    x64_cgbrule,
    x64_cgbruge,
    
    /* Branch Operations */
    x64_cgbrtrue,
    x64_cgbrfalse,
    x64_cgjump,
    x64_cgldswtch,
    x64_cgcalswtch,
    x64_cgcase,
    
    /* Boolean Operations */
    x64_cgbool,
    
    /* Increment/Decrement via Pointer */
    x64_cgldinc,
    x64_cginc1pi,
    x64_cgdec1pi,
    x64_cginc2pi,
    x64_cgdec2pi,
    
    /* Increment/Decrement Pointer - Local/Static/Global */
    x64_cgincpl,
    x64_cgdecpl,
    x64_cgincps,
    x64_cgdecps,
    x64_cgincpg,
    x64_cgdecpg,
    
    /* Increment/Decrement Indirect Word */
    x64_cginc1iw,
    x64_cgdec1iw,
    x64_cginc2iw,
    x64_cgdec2iw,
    
    /* Increment/Decrement Local Word */
    x64_cginclw,
    x64_cgdeclw,
    
    /* Increment/Decrement Static Word */
    x64_cgincsw,
    x64_cgdecsw,
    
    /* Increment/Decrement Global Word */
    x64_cgincgw,
    x64_cgdecgw,
    
    /* Increment/Decrement Indirect Byte */
    x64_cginc1ib,
    x64_cgdec1ib,
    x64_cginc2ib,
    x64_cgdec2ib,
    
    /* Increment/Decrement Local Byte */
    x64_cginclb,
    x64_cgdeclb,
    
    /* Increment/Decrement Static Byte */
    x64_cgincsb,
    x64_cgdecsb,
    
    /* Increment/Decrement Global Byte */
    x64_cgincgb,
    x64_cgdecgb,
    
    /* Store Operations */
    x64_cgstorib,
    x64_cgstoriw,
    x64_cgstorlb,
    x64_cgstorlw,
    x64_cgstorsb,
    x64_cgstorsw,
    x64_cgstorgb,
    x64_cgstorgw,
    
    /* Function Operations */
    x64_cginitlw,
    x64_cgcall,
    x64_cgcalr,
    x64_cgstack,
    x64_cgentry,
    x64_cgexit,
    
    /* ABI-Compliant Calling Convention */
    x64_cgpusharg,
    x64_cgcallprep,
    x64_cgcallend,
    x64_cgfnentry,
    
    /* Stack Frame Layout */
    x64_cggetframeinfo,
    x64_cgparamoffset,
    x64_cglocaloffset,
    x64_cgalignlocal,
    
    /* Data Definition */
    x64_cgdefb,
    x64_cgdefh,
    x64_cgdefw,
    x64_cgdefd,
    x64_cgdefp,
    x64_cgdefl,
    x64_cgdefc,
    x64_cgdefq,
    x64_cggbss,
    x64_cglbss,
    
    /* Alignment */
    x64_cgalign,
    
    /* Floating-Point Operations - SSE2 */
    x64_cgfloads,
    x64_cgfloadd,
    x64_cgfloadgs,
    x64_cgfloadgd,
    x64_cgfstores,
    x64_cgfstored,
    x64_cgfstoregs,
    x64_cgfstoregsd,
    x64_cgflits,
    x64_cgflitd,
    x64_cgfadds,
    x64_cgfaddd,
    x64_cgfsubs,
    x64_cgfsubd,
    x64_cgfmuls,
    x64_cgfmuld,
    x64_cgfdivs,
    x64_cgfdivd,
    x64_cgfnegs,
    x64_cgfnegd,
    x64_cgfcmps,
    x64_cgfcmpd,
    x64_cgfeqs,
    x64_cgfeqd,
    x64_cgfnes,
    x64_cgfned,
    x64_cgflts,
    x64_cgfltd,
    x64_cgfgts,
    x64_cgfgtd,
    x64_cgfles,
    x64_cgfled,
    x64_cgfges,
    x64_cgfged,
    x64_cgitofs,
    x64_cgitofd,
    x64_cgftois,
    x64_cgftoid,
    x64_cgstod,
    x64_cgdtos,
    x64_cgfpush,
    x64_cgfpop,
    x64_cgfxch,
    x64_cgdeffloat,
    x64_cgdefdouble
};

/*
 * ============================================================================
 * External OS configurations
 * ============================================================================
 */
extern struct cg_os_config cg_os_linux;
extern struct cg_os_config cg_os_freebsd;
extern struct cg_os_config cg_os_netbsd;
extern struct cg_os_config cg_os_darwin;

/*
 * ============================================================================
 * Target Definitions for x86-64
 * ============================================================================
 */
struct cg_target cg_target_linux_x86_64 = {
    "linux-x86-64",
    "Linux x86-64 (ELF, GAS syntax)",
    &cg_arch_x86_64,
    &cg_os_linux,
    &cg_vtable_x86_64,
    NULL,
    NULL
};

struct cg_target cg_target_freebsd_x86_64 = {
    "freebsd-x86-64",
    "FreeBSD x86-64 (ELF, GAS syntax)",
    &cg_arch_x86_64,
    &cg_os_freebsd,
    &cg_vtable_x86_64,
    NULL,
    NULL
};

struct cg_target cg_target_netbsd_x86_64 = {
    "netbsd-x86-64",
    "NetBSD x86-64 (ELF, GAS syntax)",
    &cg_arch_x86_64,
    &cg_os_netbsd,
    &cg_vtable_x86_64,
    NULL,
    NULL
};

struct cg_target cg_target_darwin_x86_64 = {
    "darwin-x86-64",
    "Darwin/macOS x86-64 (Mach-O, GAS syntax)",
    &cg_arch_x86_64_darwin,
    &cg_os_darwin,
    &cg_vtable_x86_64,
    NULL,
    NULL
};
