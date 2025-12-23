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

static void x64_cglit(int v)        { ngen("%s\t$%d,%%rax", "movq", v); }
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
static void x64_cgdefw(int v)       { ngen("%s\t%d", ".quad", v); }
static void x64_cgdefp(int v)       { ngen("%s\t%d", ".quad", v); }
static void x64_cgdefl(int v)       { lgen("%s\t%c%d", ".quad", v); }
static void x64_cgdefc(int c)       { ngen("%s\t'%c'", ".byte", c); }
static void x64_cggbss(char *s, int z) { ngen(".comm\t%s,%d", s, z); }
static void x64_cglbss(char *s, int z) { ngen(".lcomm\t%s,%d", s, z); }
static void x64_cgalign(void)       { /* unused */ }

/*
 * ============================================================================
 * x86-64 Architecture Description
 * ============================================================================
 */
struct cg_arch cg_arch_x86_64 = {
    "x86-64",           /* name */
    64,                 /* bits */
    1,                  /* char_size */
    8,                  /* int_size */
    8,                  /* ptr_size */
    8,                  /* bpw */
    ENDIAN_LITTLE,      /* endian */
    STACK_DOWN,         /* stack_dir */
    ASM_GAS,            /* asm_syntax */
    CC_SYSV_AMD64,      /* call_conv */
    FLOAT_IEEE754,      /* float_format */
    16,                 /* align_stack */
    8,                  /* align_data */
    16,                 /* align_func */
    1,                  /* has_mul */
    1,                  /* has_div */
    1,                  /* has_mod */
    1,                  /* has_byte_ops */
    0,                  /* needs_alignment */
    16,                 /* param_offset_base (return addr + saved rbp) */
    1,                  /* param_offset_dir (positive: 16, 24, 32...) */
    0,                  /* local_offset_base */
    -1                  /* local_offset_dir (negative: -8, -16, -24...) */
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
    
    /* Data Definition */
    x64_cgdefb,
    x64_cgdefw,
    x64_cgdefp,
    x64_cgdefl,
    x64_cgdefc,
    x64_cggbss,
    x64_cglbss,
    
    /* Alignment */
    x64_cgalign
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
    &cg_arch_x86_64,
    &cg_os_darwin,
    &cg_vtable_x86_64,
    NULL,
    NULL
};
