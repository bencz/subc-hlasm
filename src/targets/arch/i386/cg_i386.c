/*
 * SubC Compiler - i386 Code Generator
 *
 * This file implements the code generator for i386 architecture.
 * It provides the vtable, architecture description, and all code
 * generation functions for i386 targets.
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
static void i386_cgsynth(char *op);
static int  i386_cgload2(void);
static void i386_cgclear2(void);
static void i386_cgpop2(void);
static void i386_cgcmp(char *inst);
static void i386_cgbrcond(char *i, int n);
static void i386_cgbr(char *how, int n);

/*
 * ============================================================================
 * Code Generation Function Implementations
 * ============================================================================
 */

static void i386_cgdata(void)       { gen(".data"); }
static void i386_cgtext(void)       { gen(".text"); }
static void i386_cgprelude(void)    { }
static void i386_cgpostlude(void)   { }
static void i386_cgpublic(char *s)  { ngen(".globl\t%s", s, 0); }

static void i386_cgsynth(char *op) {
    int n;
    char *s;

    n = Q_val;
    s = gsym(Q_name);
    switch (Q_type) {
    case addr_auto:     ngen("%s\t%d(%%ebp),%%ecx", "leal", n);
                        sgen("%s\t%s,%%eax", op, "%ecx");
                        break;
    case addr_static:   lgen("%s\t$%c%d,%%eax", op, n); break;
    case addr_globl:    sgen("%s\t$%s,%%eax", op, s); break;
    case addr_label:    lgen("%s\t$%c%d,%%eax", op, n); break;
    case literal:       ngen("%s\t$%d,%%eax", op, n); break;
    case auto_word:     ngen("%s\t%d(%%ebp),%%eax", op, n); break;
    case static_word:   lgen("%s\t%c%d,%%eax", op, n); break;
    case globl_word:    sgen("%s\t%s,%%eax", op, s); break;
    case auto_byte:
    case static_byte:
    case globl_byte:    i386_cgload2();
                        ngen("%s\t%%ecx,%%eax", op, 0);
                        break;
    case empty:         i386_cgpop2();
                        sgen("%s\t%s,%%eax", op, "%ecx");
                        break;
    default:            fatal("internal: bad type in cgsynth()");
    }
    Q_type = empty;
}

static int i386_cgload2(void) {
    int n, q;
    char *s, *op, *opb;

    op = "movl";
    opb = "movb";
    n = Q_val;
    s = gsym(Q_name);
    switch (Q_type) {
    case addr_auto:     ngen("%s\t%d(%%ebp),%%ecx", "leal", n);
                        break;
    case addr_static:   lgen("%s\t$%c%d,%%ecx", op, n); break;
    case addr_globl:    sgen("%s\t$%s,%%ecx", op, s); break;
    case addr_label:    lgen("%s\t$%c%d,%%ecx", op, n); break;
    case literal:       ngen("%s\t$%d,%%ecx", op, n); break;
    case auto_byte:     i386_cgclear2();
                        ngen("%s\t%d(%%ebp),%%cl", opb, n);
                        break;
    case auto_word:     ngen("%s\t%d(%%ebp),%%ecx", op, n); break;
    case static_byte:   i386_cgclear2();
                        lgen("%s\t%c%d,%%cl", opb, n);
                        break;
    case static_word:   lgen("%s\t%c%d,%%ecx", op, n); break;
    case globl_byte:    i386_cgclear2();
                        sgen("%s\t%s,%%cl", opb, s);
                        break;
    case globl_word:    sgen("%s\t%s,%%ecx", op, s); break;
    case empty:         i386_cgpop2();
                        break;
    default:            fatal("internal: bad type in cgload2()");
    }
    q = Q_type;
    Q_type = empty;
    return empty == q;
}

static void i386_cglit(int v)       { ngen("%s\t$%d,%%eax", "movl", v); }
static void i386_cgclear(void)      { gen("xorl\t%eax,%eax"); }
static void i386_cgclear2(void)     { gen("xorl\t%ecx,%ecx"); }
static void i386_cgldgb(char *s)    { sgen("%s\t%s,%%al", "movb", s); }
static void i386_cgldgw(char *s)    { sgen("%s\t%s,%%eax", "movl", s); }
static void i386_cgldlb(int n)      { ngen("%s\t%d(%%ebp),%%al", "movb", n); }
static void i386_cgldlw(int n)      { ngen("%s\t%d(%%ebp),%%eax", "movl", n); }
static void i386_cgldsb(int n)      { lgen("%s\t%c%d,%%al", "movb", n); }
static void i386_cgldsw(int n)      { lgen("%s\t%c%d,%%eax", "movl", n); }
static void i386_cgldla(int n)      { ngen("%s\t%d(%%ebp),%%eax", "leal", n); }
static void i386_cgldsa(int n)      { lgen("%s\t$%c%d,%%eax", "movl", n); }
static void i386_cgldga(char *s)    { sgen("%s\t$%s,%%eax", "movl", s); }

static void i386_cgindb(void) {
    gen("movl\t%eax,%edx");
    i386_cgclear();
    gen("movb\t(%edx),%al");
}

static void i386_cgindw(void)       { gen("movl\t(%eax),%eax"); }
static void i386_cgldlab(int id)    { lgen("%s\t$%c%d,%%eax", "movl", id); }

static void i386_cgpush(void)       { gen("pushl\t%eax"); }
static void i386_cgpushlit(int n)   { ngen("%s\t$%d", "pushl", n); }
static void i386_cgpop2(void)       { gen("popl\t%ecx"); }
static void i386_cgswap(void)       { gen("xchgl\t%eax,%ecx"); }
static void i386_cgpopptr(void)     { gen("popl\t%edx"); }

static void i386_cgand(void)        { i386_cgsynth("andl"); }
static void i386_cgior(void)        { i386_cgsynth("orl"); }
static void i386_cgxor(void)        { i386_cgsynth("xorl"); }
static void i386_cgadd(void)        { gen("addl\t%ecx,%eax"); }
static void i386_cgmul(void)        { gen("imull\t%ecx,%eax"); }
static void i386_cgsub(void)        { gen("subl\t%ecx,%eax"); }

static void i386_cgdiv(void) {
    gen("cdq");
    gen("idivl\t%ecx");
}

static void i386_cgmod(void) {
    i386_cgdiv();
    gen("movl\t%edx,%eax");
}

static void i386_cgshl(void)        { gen("shll\t%cl,%eax"); }
static void i386_cgshr(void)        { gen("sarl\t%cl,%eax"); }

static void i386_cgcmp(char *inst) {
    int lab;
    lab = label();
    gen("xorl\t%edx,%edx");
    if (empty == Q_type) {
        i386_cgpop2();
        gen("cmpl\t%eax,%ecx");
    }
    else {
        i386_cgsynth("cmpl");
    }
    lgen("%s\t%c%d", inst, lab);
    gen("incl\t%edx");
    genlab(lab);
    gen("movl\t%edx,%eax");
}

static void i386_cgeq(void)         { i386_cgcmp("jne"); }
static void i386_cgne(void)         { i386_cgcmp("je"); }
static void i386_cglt(void)         { i386_cgcmp("jge"); }
static void i386_cggt(void)         { i386_cgcmp("jle"); }
static void i386_cgle(void)         { i386_cgcmp("jg"); }
static void i386_cgge(void)         { i386_cgcmp("jl"); }
static void i386_cgult(void)        { i386_cgcmp("jae"); }
static void i386_cgugt(void)        { i386_cgcmp("jbe"); }
static void i386_cgule(void)        { i386_cgcmp("ja"); }
static void i386_cguge(void)        { i386_cgcmp("jb"); }

static void i386_cgbrcond(char *i, int n) {
    int lab;
    lab = label();
    if (empty == Q_type) {
        i386_cgpop2();
        gen("cmpl\t%eax,%ecx");
    }
    else {
        i386_cgsynth("cmpl");
    }
    lgen("%s\t%c%d", i, lab);
    lgen("%s\t%c%d", "jmp", n);
    genlab(lab);
}

static void i386_cgbreq(int n)      { i386_cgbrcond("je", n); }
static void i386_cgbrne(int n)      { i386_cgbrcond("jne", n); }
static void i386_cgbrlt(int n)      { i386_cgbrcond("jl", n); }
static void i386_cgbrgt(int n)      { i386_cgbrcond("jg", n); }
static void i386_cgbrle(int n)      { i386_cgbrcond("jle", n); }
static void i386_cgbrge(int n)      { i386_cgbrcond("jge", n); }
static void i386_cgbrult(int n)     { i386_cgbrcond("jb", n); }
static void i386_cgbrugt(int n)     { i386_cgbrcond("ja", n); }
static void i386_cgbrule(int n)     { i386_cgbrcond("jbe", n); }
static void i386_cgbruge(int n)     { i386_cgbrcond("jae", n); }

static void i386_cgneg(void)        { gen("negl\t%eax"); }
static void i386_cgnot(void)        { gen("notl\t%eax"); }

static void i386_cglognot(void) {
    gen("negl\t%eax");
    gen("sbbl\t%eax,%eax");
    gen("incl\t%eax");
}

static void i386_cgscale(void)      { gen("shll\t$2,%eax"); }
static void i386_cgscale2(void)     { gen("shll\t$2,%ecx"); }
static void i386_cgunscale(void)    { gen("shrl\t$2,%eax"); }

static void i386_cgscaleby(int v) {
    ngen("%s\t$%d,%%ecx", "movl", v);
    gen("mull\t%ecx");
}

static void i386_cgscale2by(int v) {
    gen("pushl\t%eax");
    ngen("%s\t$%d,%%eax", "movl", v);
    gen("mull\t%ecx");
    gen("movl\t%eax,%ecx");
    gen("popl\t%eax");
}

static void i386_cgunscaleby(int v) {
    ngen("%s\t$%d,%%ecx", "movl", v);
    gen("xorl\t%edx,%edx");
    gen("divl\t%ecx");
}

static void i386_cgbool(void) {
    gen("negl\t%eax");
    gen("sbbl\t%eax,%eax");
    gen("negl\t%eax");
}

static void i386_cgldinc(void)      { gen("movl\t%eax,%edx"); }
static void i386_cginc1pi(int v)    { ngen("%s\t$%d,(%%eax)", "addl", v); }
static void i386_cgdec1pi(int v)    { ngen("%s\t$%d,(%%eax)", "subl", v); }
static void i386_cginc2pi(int v)    { ngen("%s\t$%d,(%%edx)", "addl", v); }
static void i386_cgdec2pi(int v)    { ngen("%s\t$%d,(%%edx)", "subl", v); }
static void i386_cgincpl(int a, int v)  { ngen2("%s\t$%d,%d(%%ebp)", "addl", v, a); }
static void i386_cgdecpl(int a, int v)  { ngen2("%s\t$%d,%d(%%ebp)", "subl", v, a); }
static void i386_cgincps(int a, int v)  { lgen2("addl\t$%d,%c%d", v, a); }
static void i386_cgdecps(int a, int v)  { lgen2("subl\t$%d,%c%d", v, a); }
static void i386_cgincpg(char *s, int v) { sgen2("%s\t$%d,%s", "addl", v, s); }
static void i386_cgdecpg(char *s, int v) { sgen2("%s\t$%d,%s", "subl", v, s); }
static void i386_cginc1iw(void)     { ngen("%s\t(%%eax)", "incl", 0); }
static void i386_cgdec1iw(void)     { ngen("%s\t(%%eax)", "decl", 0); }
static void i386_cginc2iw(void)     { ngen("%s\t(%%edx)", "incl", 0); }
static void i386_cgdec2iw(void)     { ngen("%s\t(%%edx)", "decl", 0); }
static void i386_cginclw(int a)     { ngen("%s\t%d(%%ebp)", "incl", a); }
static void i386_cgdeclw(int a)     { ngen("%s\t%d(%%ebp)", "decl", a); }
static void i386_cgincsw(int a)     { lgen("%s\t%c%d", "incl", a); }
static void i386_cgdecsw(int a)     { lgen("%s\t%c%d", "decl", a); }
static void i386_cgincgw(char *s)   { sgen("%s\t%s", "incl", s); }
static void i386_cgdecgw(char *s)   { sgen("%s\t%s", "decl", s); }
static void i386_cginc1ib(void)     { ngen("%s\t(%%eax)", "incb", 0); }
static void i386_cgdec1ib(void)     { ngen("%s\t(%%eax)", "decb", 0); }
static void i386_cginc2ib(void)     { ngen("%s\t(%%edx)", "incb", 0); }
static void i386_cgdec2ib(void)     { ngen("%s\t(%%edx)", "decb", 0); }
static void i386_cginclb(int a)     { ngen("%s\t%d(%%ebp)", "incb", a); }
static void i386_cgdeclb(int a)     { ngen("%s\t%d(%%ebp)", "decb", a); }
static void i386_cgincsb(int a)     { lgen("%s\t%c%d", "incb", a); }
static void i386_cgdecsb(int a)     { lgen("%s\t%c%d", "decb", a); }
static void i386_cgincgb(char *s)   { sgen("%s\t%s", "incb", s); }
static void i386_cgdecgb(char *s)   { sgen("%s\t%s", "decb", s); }

static void i386_cgbr(char *how, int n) {
    int lab;
    lab = label();
    gen("orl\t%eax,%eax");
    lgen("%s\t%c%d", how, lab);
    lgen("%s\t%c%d", "jmp", n);
    genlab(lab);
}

static void i386_cgbrtrue(int n)    { i386_cgbr("jz", n); }
static void i386_cgbrfalse(int n)   { i386_cgbr("jnz", n); }
static void i386_cgjump(int n)      { lgen("%s\t%c%d", "jmp", n); }
static void i386_cgldswtch(int n)   { lgen("%s\t$%c%d,%%edx", "movl", n); }
static void i386_cgcalswtch(void)   { gen("jmp\tswitch"); }
static void i386_cgcase(int v, int l) { lgen2(".long\t%d,%c%d", v, l); }

static void i386_cgstorib(void)     { ngen("%s\t%%al,(%%edx)", "movb", 0); }
static void i386_cgstoriw(void)     { ngen("%s\t%%eax,(%%edx)", "movl", 0); }
static void i386_cgstorlb(int n)    { ngen("%s\t%%al,%d(%%ebp)", "movb", n); }
static void i386_cgstorlw(int n)    { ngen("%s\t%%eax,%d(%%ebp)", "movl", n); }
static void i386_cgstorsb(int n)    { lgen("%s\t%%al,%c%d", "movb", n); }
static void i386_cgstorsw(int n)    { lgen("%s\t%%eax,%c%d", "movl", n); }
static void i386_cgstorgb(char *s)  { sgen("%s\t%%al,%s", "movb", s); }
static void i386_cgstorgw(char *s)  { sgen("%s\t%%eax,%s", "movl", s); }

static void i386_cginitlw(int v, int a) { ngen2("%s\t$%d,%d(%%ebp)", "movl", v, a); }
static void i386_cgcall(char *s)    { sgen("%s\t%s", "call", s); }
static void i386_cgcalr(void)       { gen("call\t*%eax"); }
static void i386_cgstack(int n)     { ngen("%s\t$%d,%%esp", "addl", n); }

static void i386_cgentry(void) {
    gen("pushl\t%ebp");
    gen("movl\t%esp,%ebp");
}

static void i386_cgexit(void) {
    gen("popl\t%ebp");
    gen("ret");
}

static void i386_cgdefb(int v)      { ngen("%s\t%d", ".byte", v); }
static void i386_cgdefw(int v)      { ngen("%s\t%d", ".long", v); }
static void i386_cgdefp(int v)      { ngen("%s\t%d", ".long", v); }
static void i386_cgdefl(int v)      { lgen("%s\t%c%d", ".long", v); }
static void i386_cgdefc(int c)      { ngen("%s\t'%c'", ".byte", c); }
static void i386_cggbss(char *s, int z) { ngen(".comm\t%s,%d", s, z); }
static void i386_cglbss(char *s, int z) { ngen(".lcomm\t%s,%d", s, z); }
static void i386_cgalign(void)      { /* unused */ }

/*
 * ============================================================================
 * i386 cdecl Calling Convention Support
 * ============================================================================
 *
 * i386 cdecl passes ALL arguments on the stack (right to left).
 * No registers are used for argument passing.
 */

/* cgpusharg - For cdecl, all args go to stack (same as cgpush) */
static void i386_cgpusharg(int argnum) {
    (void)argnum;  /* unused - all args go to stack */
    gen("pushl\t%eax");
}

/* cgcallprep - No special preparation needed for cdecl */
static void i386_cgcallprep(int nargs) {
    (void)nargs;  /* unused */
}

/* cgcallend - Clean up stack after call */
static void i386_cgcallend(int nargs) {
    if (nargs > 0) {
        ngen("%s\t$%d,%%esp", "addl", nargs * 4);
    }
}

/* cgfnentry - Standard function entry (same as cgentry for cdecl) */
static void i386_cgfnentry(int nparams) {
    (void)nparams;  /* unused - params already on stack */
    gen("pushl\t%ebp");
    gen("movl\t%esp,%ebp");
}

/*
 * ============================================================================
 * i386 Architecture Description
 * ============================================================================
 */
struct cg_arch cg_arch_i386 = {
    "i386",             /* name */
    32,                 /* bits */
    1,                  /* char_size */
    4,                  /* int_size */
    4,                  /* ptr_size */
    4,                  /* bpw */
    ENDIAN_LITTLE,      /* endian */
    STACK_DOWN,         /* stack_dir */
    ASM_GAS,            /* asm_syntax */
    CC_CDECL,           /* call_conv */
    FLOAT_IEEE754,      /* float_format */
    4,                  /* align_stack */
    4,                  /* align_data */
    4,                  /* align_func */
    1,                  /* has_mul */
    1,                  /* has_div */
    1,                  /* has_mod */
    1,                  /* has_byte_ops */
    0,                  /* needs_alignment */
    8,                  /* param_offset_base (return addr + saved ebp) */
    1,                  /* param_offset_dir (positive: 8, 12, 16...) */
    0,                  /* local_offset_base */
    -1,                 /* local_offset_dir (negative: -4, -8, -12...) */
    0                   /* num_arg_regs (cdecl: all args on stack) */
};

/*
 * ============================================================================
 * i386 Code Generator Vtable
 * ============================================================================
 */
struct cg_vtable cg_vtable_i386 = {
    /* Section Control */
    i386_cgdata,
    i386_cgtext,
    i386_cgprelude,
    i386_cgpostlude,
    i386_cgpublic,
    
    /* Synthesizer Support */
    i386_cgsynth,
    i386_cgload2,
    
    /* Literal and Clear */
    i386_cglit,
    i386_cgclear,
    i386_cgclear2,
    
    /* Load Operations - Global */
    i386_cgldgb,
    i386_cgldgw,
    
    /* Load Operations - Local */
    i386_cgldlb,
    i386_cgldlw,
    
    /* Load Operations - Static */
    i386_cgldsb,
    i386_cgldsw,
    
    /* Load Address Operations */
    i386_cgldla,
    i386_cgldsa,
    i386_cgldga,
    
    /* Indirect Load Operations */
    i386_cgindb,
    i386_cgindw,
    
    /* Load Label */
    i386_cgldlab,
    
    /* Stack Operations */
    i386_cgpush,
    i386_cgpushlit,
    i386_cgpop2,
    i386_cgswap,
    i386_cgpopptr,
    
    /* Arithmetic Operations */
    i386_cgand,
    i386_cgior,
    i386_cgxor,
    i386_cgadd,
    i386_cgsub,
    i386_cgmul,
    i386_cgdiv,
    i386_cgmod,
    i386_cgshl,
    i386_cgshr,
    
    /* Unary Operations */
    i386_cgneg,
    i386_cgnot,
    i386_cglognot,
    
    /* Scaling Operations */
    i386_cgscale,
    i386_cgscale2,
    i386_cgunscale,
    i386_cgscaleby,
    i386_cgscale2by,
    i386_cgunscaleby,
    
    /* Comparison Operations */
    i386_cgeq,
    i386_cgne,
    i386_cglt,
    i386_cggt,
    i386_cgle,
    i386_cgge,
    i386_cgult,
    i386_cgugt,
    i386_cgule,
    i386_cguge,
    
    /* Conditional Branch Operations */
    i386_cgbreq,
    i386_cgbrne,
    i386_cgbrlt,
    i386_cgbrgt,
    i386_cgbrle,
    i386_cgbrge,
    i386_cgbrult,
    i386_cgbrugt,
    i386_cgbrule,
    i386_cgbruge,
    
    /* Branch Operations */
    i386_cgbrtrue,
    i386_cgbrfalse,
    i386_cgjump,
    i386_cgldswtch,
    i386_cgcalswtch,
    i386_cgcase,
    
    /* Boolean Operations */
    i386_cgbool,
    
    /* Increment/Decrement via Pointer */
    i386_cgldinc,
    i386_cginc1pi,
    i386_cgdec1pi,
    i386_cginc2pi,
    i386_cgdec2pi,
    
    /* Increment/Decrement Pointer - Local/Static/Global */
    i386_cgincpl,
    i386_cgdecpl,
    i386_cgincps,
    i386_cgdecps,
    i386_cgincpg,
    i386_cgdecpg,
    
    /* Increment/Decrement Indirect Word */
    i386_cginc1iw,
    i386_cgdec1iw,
    i386_cginc2iw,
    i386_cgdec2iw,
    
    /* Increment/Decrement Local Word */
    i386_cginclw,
    i386_cgdeclw,
    
    /* Increment/Decrement Static Word */
    i386_cgincsw,
    i386_cgdecsw,
    
    /* Increment/Decrement Global Word */
    i386_cgincgw,
    i386_cgdecgw,
    
    /* Increment/Decrement Indirect Byte */
    i386_cginc1ib,
    i386_cgdec1ib,
    i386_cginc2ib,
    i386_cgdec2ib,
    
    /* Increment/Decrement Local Byte */
    i386_cginclb,
    i386_cgdeclb,
    
    /* Increment/Decrement Static Byte */
    i386_cgincsb,
    i386_cgdecsb,
    
    /* Increment/Decrement Global Byte */
    i386_cgincgb,
    i386_cgdecgb,
    
    /* Store Operations */
    i386_cgstorib,
    i386_cgstoriw,
    i386_cgstorlb,
    i386_cgstorlw,
    i386_cgstorsb,
    i386_cgstorsw,
    i386_cgstorgb,
    i386_cgstorgw,
    
    /* Function Operations */
    i386_cginitlw,
    i386_cgcall,
    i386_cgcalr,
    i386_cgstack,
    i386_cgentry,
    i386_cgexit,
    
    /* ABI-Compliant Calling Convention */
    i386_cgpusharg,
    i386_cgcallprep,
    i386_cgcallend,
    i386_cgfnentry,
    
    /* Data Definition */
    i386_cgdefb,
    i386_cgdefw,
    i386_cgdefp,
    i386_cgdefl,
    i386_cgdefc,
    i386_cggbss,
    i386_cglbss,
    
    /* Alignment */
    i386_cgalign
};

/*
 * ============================================================================
 * External OS configurations
 * ============================================================================
 */
extern struct cg_os_config cg_os_linux;
extern struct cg_os_config cg_os_freebsd;
extern struct cg_os_config cg_os_netbsd;
extern struct cg_os_config cg_os_openbsd;
extern struct cg_os_config cg_os_windows;

/*
 * ============================================================================
 * Target Definitions for i386
 * ============================================================================
 */
struct cg_target cg_target_linux_386 = {
    "linux-386",
    "Linux i386 (ELF, GAS syntax)",
    &cg_arch_i386,
    &cg_os_linux,
    &cg_vtable_i386,
    NULL,
    NULL
};

struct cg_target cg_target_freebsd_386 = {
    "freebsd-386",
    "FreeBSD i386 (ELF, GAS syntax)",
    &cg_arch_i386,
    &cg_os_freebsd,
    &cg_vtable_i386,
    NULL,
    NULL
};

struct cg_target cg_target_netbsd_386 = {
    "netbsd-386",
    "NetBSD i386 (ELF, GAS syntax)",
    &cg_arch_i386,
    &cg_os_netbsd,
    &cg_vtable_i386,
    NULL,
    NULL
};

struct cg_target cg_target_openbsd_386 = {
    "openbsd-386",
    "OpenBSD i386 (ELF, GAS syntax)",
    &cg_arch_i386,
    &cg_os_openbsd,
    &cg_vtable_i386,
    NULL,
    NULL
};

struct cg_target cg_target_windows_386 = {
    "windows-386",
    "Windows i386 (PE, GAS syntax)",
    &cg_arch_i386,
    &cg_os_windows,
    &cg_vtable_i386,
    NULL,
    NULL
};
