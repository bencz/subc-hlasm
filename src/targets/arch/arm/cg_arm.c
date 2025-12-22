/*
 * SubC Compiler - ARMv6 Code Generator
 *
 * This file implements the code generator for ARMv6 architecture.
 * It provides the vtable, architecture description, and all code
 * generation functions for ARM targets.
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
static void arm_cglit2(int v, int aux);
static void arm_cglocladdr(int n, int aux);
static void arm_cgstataddr(int n, int aux);
static void arm_cgglobaddr(char *s, int aux);
static void arm_cgind2b(void);
static void arm_cgind2w(void);
static void arm_cgstorb(void);
static void arm_cgstorw(void);
static void arm_cgcmp(char *inst);
static void arm_cgbrcond(char *i, int n);
static void arm_cgbr(char *how, int n);
static int  arm_cgload2(void);
static void arm_cgpop2(void);
static void arm_cgindb(void);
static void arm_cgindw(void);
static void arm_cgdefw(int v);
static void arm_cgdefl(int v);

/*
 * ============================================================================
 * Code Generation Function Implementations
 * ============================================================================
 */

static void arm_cgdata(void)        { gen(".data"); }
static void arm_cgtext(void)        { gen(".text"); }
static void arm_cgprelude(void)     { gen(".syntax unified"); }
static void arm_cgpostlude(void)    { }
static void arm_cgpublic(char *s)   { ngen(".globl\t%s", s, 0); }

/*
 * Functions dealing with PC-relative addressing
 */
static void arm_cglit2(int v, int aux) {
    int l, skip;
    if (0 <= v && v <= 127) {
        if (aux == 2)
            ngen("%s\tr2,#%d", "mov", v);
        else if (aux == 1)
            ngen("%s\tr1,#%d", "mov", v);
        else
            ngen("%s\tr0,#%d", "mov", v);
    }
    else if (-128 <= v && v < 0) {
        v = -v-1;
        if (aux == 2)
            ngen("%s\tr2,#%d", "mvn", v);
        else if (aux == 1)
            ngen("%s\tr1,#%d", "mvn", v);
        else
            ngen("%s\tr0,#%d", "mvn", v);
    }
    else {
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
        arm_cgdefw(v);
        genlab(skip);
    }
}

static void arm_cglocladdr(int n, int aux) {
    if (n < -255 || n > 255) {
        arm_cglit2(n, 2);
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

static void arm_cgstataddr(int n, int aux) {
    int l, skip;

    l = label();
    if (aux)
        lgen("%s\tr1,%c%d", "ldr", l);
    else
        lgen("%s\tr0,%c%d", "ldr", l);
    skip = label();
    lgen("%s\t%c%d", "b", skip);
    genlab(l);
    arm_cgdefl(n);
    genlab(skip);
}

static void arm_cgglobaddr(char *s, int aux) {
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

static void arm_cgind2b(void)       { gen("ldrb\tr1,[r1]"); }
static void arm_cgind2w(void)       { gen("ldr\tr1,[r1]"); }
static void arm_cgstorb(void)       { gen("strb\tr0,[r1]"); }
static void arm_cgstorw(void)       { gen("str\tr0,[r1]"); }

/* cgsynth is unused in ARM - returns 0 */
static void arm_cgsynth(char *op) {
    (void)op;
    /* unused */
}

static int arm_cgload2(void) {
    int n, q;
    char *s;

    n = Q_val;
    s = gsym(Q_name);
    switch (Q_type) {
    case addr_auto:     arm_cglocladdr(n, 1); break;
    case addr_static:   arm_cgstataddr(n, 1); break;
    case addr_globl:    arm_cgglobaddr(s, 1); break;
    case addr_label:    arm_cgstataddr(n, 1); break;
    case literal:       arm_cglit2(n, 1); break;
    case auto_byte:     arm_cglocladdr(n, 1);
                        arm_cgind2b();
                        break;
    case auto_word:     arm_cglocladdr(n, 1);
                        arm_cgind2w();
                        break;
    case static_byte:   arm_cgstataddr(n, 1);
                        arm_cgind2b();
                        break;
    case static_word:   arm_cgstataddr(n, 1);
                        arm_cgind2w();
                        break;
    case globl_byte:    arm_cgglobaddr(s, 1);
                        arm_cgind2b();
                        break;
    case globl_word:    arm_cgglobaddr(s, 1);
                        arm_cgind2w();
                        break;
    case empty:         arm_cgpop2();
                        break;
    default:            fatal("internal: bad type in cgload2()");
    }
    q = Q_type;
    Q_type = empty;
    return empty == q;
}

static void arm_cglit(int v)        { arm_cglit2(v, 0); }
static void arm_cgclear(void)       { /* unused */ }
static void arm_cgclear2(void)      { /* unused */ }

static void arm_cgldgb(char *s) {
    arm_cgglobaddr(s, 0);
    arm_cgindb();
}

static void arm_cgldgw(char *s) {
    arm_cgglobaddr(s, 0);
    arm_cgindw();
}

static void arm_cgldlb(int n) {
    arm_cglocladdr(n, 0);
    arm_cgindb();
}

static void arm_cgldlw(int n) {
    arm_cglocladdr(n, 0);
    arm_cgindw();
}

static void arm_cgldsb(int n) {
    arm_cgstataddr(n, 0);
    arm_cgindb();
}

static void arm_cgldsw(int n) {
    arm_cgstataddr(n, 0);
    arm_cgindw();
}

static void arm_cgldla(int n)       { arm_cglocladdr(n, 0); }
static void arm_cgldsa(int n)       { arm_cgstataddr(n, 0); }
static void arm_cgldga(char *s)     { arm_cgglobaddr(s, 0); }
static void arm_cgindb(void)        { gen("ldrb\tr0,[r0]"); }
static void arm_cgindw(void)        { gen("ldr\tr0,[r0]"); }
static void arm_cgldlab(int id)     { arm_cgstataddr(id, 0); }

static void arm_cgpush(void)        { gen("push\t{r0}"); }

static void arm_cgpushlit(int n) {
    arm_cglit(n);
    arm_cgpush();
}

static void arm_cgpop2(void)        { gen("pop\t{r1}"); }

static void arm_cgswap(void) {
    gen("mov\tr2,r0");
    gen("mov\tr0,r1");
    gen("mov\tr1,r2");
}

static void arm_cgpopptr(void)      { gen("pop\t{r2}"); }

static void arm_cgand(void) {
    arm_cgload2();
    gen("and\tr0,r0,r1");
}

static void arm_cgior(void) {
    arm_cgload2();
    gen("orr\tr0,r0,r1");
}

static void arm_cgxor(void) {
    arm_cgload2();
    gen("eor\tr0,r0,r1");
}

static void arm_cgadd(void)         { gen("add\tr0,r0,r1"); }
static void arm_cgmul(void)         { gen("mul\tr0,r0,r1"); }
static void arm_cgsub(void)         { gen("sub\tr0,r0,r1"); }
static void arm_cgdiv(void)         { gen("bl\tsdiv"); }
static void arm_cgmod(void)         { gen("bl\tsrem"); }
static void arm_cgshl(void)         { gen("lsl\tr0,r0,r1"); }
static void arm_cgshr(void)         { gen("asr\tr0,r0,r1"); }

static void arm_cgcmp(char *inst) {
    gen("mov\tr3,r0");
    gen("mov\tr0,#0");
    if (empty == Q_type) {
        arm_cgpop2();
        gen("cmp\tr1,r3");
    }
    else {
        arm_cgload2();
        gen("cmp\tr3,r1");
    }
    ngen("%s\tr0,#%d", inst, 1);
}

static void arm_cgeq(void)          { arm_cgcmp("moveq"); }
static void arm_cgne(void)          { arm_cgcmp("movne"); }
static void arm_cglt(void)          { arm_cgcmp("movlt"); }
static void arm_cggt(void)          { arm_cgcmp("movgt"); }
static void arm_cgle(void)          { arm_cgcmp("movle"); }
static void arm_cgge(void)          { arm_cgcmp("movge"); }
static void arm_cgult(void)         { arm_cgcmp("movlo"); }
static void arm_cgugt(void)         { arm_cgcmp("movhi"); }
static void arm_cgule(void)         { arm_cgcmp("movls"); }
static void arm_cguge(void)         { arm_cgcmp("movhs"); }

static void arm_cgbrcond(char *i, int n) {
    int lab;
    lab = label();
    if (empty == Q_type) {
        arm_cgpop2();
        gen("cmp\tr1,r0");
    }
    else {
        arm_cgload2();
        gen("cmp\tr0,r1");
    }
    lgen("%s\t%c%d", i, lab);
    lgen("%s\t%c%d", "b", n);
    genlab(lab);
}

static void arm_cgbreq(int n)       { arm_cgbrcond("beq", n); }
static void arm_cgbrne(int n)       { arm_cgbrcond("bne", n); }
static void arm_cgbrlt(int n)       { arm_cgbrcond("blt", n); }
static void arm_cgbrgt(int n)       { arm_cgbrcond("bgt", n); }
static void arm_cgbrle(int n)       { arm_cgbrcond("ble", n); }
static void arm_cgbrge(int n)       { arm_cgbrcond("bge", n); }
static void arm_cgbrult(int n)      { arm_cgbrcond("blo", n); }
static void arm_cgbrugt(int n)      { arm_cgbrcond("bhi", n); }
static void arm_cgbrule(int n)      { arm_cgbrcond("bls", n); }
static void arm_cgbruge(int n)      { arm_cgbrcond("bhs", n); }

static void arm_cgneg(void)         { gen("neg\tr0,r0"); }
static void arm_cgnot(void)         { gen("mvn\tr0,r0"); }

static void arm_cglognot(void) {
    gen("cmp\tr0,#0");
    gen("mov\tr0,#0");
    gen("moveq\tr0,#1");
}

static void arm_cgscale(void)       { gen("lsl\tr0,#2"); }
static void arm_cgscale2(void)      { gen("lsl\tr1,#2"); }
static void arm_cgunscale(void)     { gen("lsr\tr0,#2"); }

static void arm_cgscaleby(int v) {
    arm_cglit2(v, 1);
    gen("mul\tr0,r0,r1");
}

static void arm_cgscale2by(int v) {
    arm_cglit2(v, 2);
    gen("mul\tr1,r1,r2");
}

static void arm_cgunscaleby(int v) {
    arm_cglit2(v, 1);
    gen("bl\tudiv");
}

static void arm_cgbool(void) {
    gen("cmp\tr0,#0");
    gen("mov\tr0,#0");
    gen("movne\tr0,#1");
}

static void arm_cgldinc(void)       { gen("mov\tr2,r0"); }

static void arm_cginc1pi(int v) {
    arm_cglit2(v, 1);
    gen("ldr\tr3,[r0]");
    gen("add\tr3,r3,r1");
    gen("str\tr3,[r0]");
}

static void arm_cgdec1pi(int v) {
    arm_cglit2(v, 1);
    gen("ldr\tr3,[r0]");
    gen("sub\tr3,r3,r1");
    gen("str\tr3,[r0]");
}

static void arm_cginc2pi(int v) {
    arm_cglit2(v, 1);
    gen("ldr\tr3,[r2]");
    gen("add\tr3,r3,r1");
    gen("str\tr3,[r2]");
}

static void arm_cgdec2pi(int v) {
    arm_cglit2(v, 1);
    gen("ldr\tr3,[r2]");
    gen("sub\tr3,r3,r1");
    gen("str\tr3,[r2]");
}

static void arm_cgincpl(int a, int v) {
    arm_cglit2(v, 2);
    arm_cglocladdr(a, 1);
    gen("ldr\tr3,[r1]");
    gen("add\tr3,r3,r2");
    gen("str\tr3,[r1]");
}

static void arm_cgdecpl(int a, int v) {
    arm_cglit2(v, 2);
    arm_cglocladdr(a, 1);
    gen("ldr\tr3,[r1]");
    gen("sub\tr3,r3,r2");
    gen("str\tr3,[r1]");
}

static void arm_cgincps(int a, int v) {
    arm_cglit2(v, 2);
    arm_cgstataddr(a, 1);
    gen("ldr\tr3,[r1]");
    gen("add\tr3,r3,r2");
    gen("str\tr3,[r1]");
}

static void arm_cgdecps(int a, int v) {
    arm_cglit2(v, 2);
    arm_cgstataddr(a, 1);
    gen("ldr\tr3,[r1]");
    gen("sub\tr3,r3,r2");
    gen("str\tr3,[r1]");
}

static void arm_cgincpg(char *s, int v) {
    arm_cglit2(v, 2);
    arm_cgglobaddr(s, 1);
    gen("ldr\tr3,[r1]");
    gen("add\tr3,r3,r2");
    gen("str\tr3,[r1]");
}

static void arm_cgdecpg(char *s, int v) {
    arm_cglit2(v, 2);
    arm_cgglobaddr(s, 1);
    gen("ldr\tr3,[r1]");
    gen("sub\tr3,r3,r2");
    gen("str\tr3,[r1]");
}

static void arm_cginc1iw(void) {
    gen("ldr\tr1,[r0]");
    gen("add\tr1,r1,#1");
    gen("str\tr1,[r0]");
}

static void arm_cgdec1iw(void) {
    gen("ldr\tr1,[r0]");
    gen("sub\tr1,r1,#1");
    gen("str\tr1,[r0]");
}

static void arm_cginc2iw(void) {
    gen("ldr\tr1,[r2]");
    gen("add\tr1,r1,#1");
    gen("str\tr1,[r2]");
}

static void arm_cgdec2iw(void) {
    gen("ldr\tr1,[r2]");
    gen("sub\tr1,r1,#1");
    gen("str\tr1,[r2]");
}

static void arm_cginclw(int a) {
    arm_cglocladdr(a, 1);
    gen("ldr\tr2,[r1]");
    gen("add\tr2,r2,#1");
    gen("str\tr2,[r1]");
}

static void arm_cgdeclw(int a) {
    arm_cglocladdr(a, 1);
    gen("ldr\tr2,[r1]");
    gen("sub\tr2,r2,#1");
    gen("str\tr2,[r1]");
}

static void arm_cgincsw(int a) {
    arm_cgstataddr(a, 1);
    gen("ldr\tr2,[r1]");
    gen("add\tr2,r2,#1");
    gen("str\tr2,[r1]");
}

static void arm_cgdecsw(int a) {
    arm_cgstataddr(a, 1);
    gen("ldr\tr2,[r1]");
    gen("sub\tr2,r2,#1");
    gen("str\tr2,[r1]");
}

static void arm_cgincgw(char *s) {
    arm_cgglobaddr(s, 1);
    gen("ldr\tr2,[r1]");
    gen("add\tr2,r2,#1");
    gen("str\tr2,[r1]");
}

static void arm_cgdecgw(char *s) {
    arm_cgglobaddr(s, 1);
    gen("ldr\tr2,[r1]");
    gen("sub\tr2,r2,#1");
    gen("str\tr2,[r1]");
}

static void arm_cginc1ib(void) {
    gen("ldrb\tr1,[r0]");
    gen("add\tr1,r1,#1");
    gen("strb\tr1,[r0]");
}

static void arm_cgdec1ib(void) {
    gen("ldrb\tr1,[r0]");
    gen("sub\tr1,r1,#1");
    gen("strb\tr1,[r0]");
}

static void arm_cginc2ib(void) {
    gen("ldrb\tr1,[r2]");
    gen("add\tr1,r1,#1");
    gen("strb\tr1,[r2]");
}

static void arm_cgdec2ib(void) {
    gen("ldrb\tr1,[r2]");
    gen("sub\tr1,r1,#1");
    gen("strb\tr1,[r2]");
}

static void arm_cginclb(int a) {
    arm_cglocladdr(a, 1);
    gen("ldrb\tr2,[r1]");
    gen("add\tr2,r2,#1");
    gen("strb\tr2,[r1]");
}

static void arm_cgdeclb(int a) {
    arm_cglocladdr(a, 1);
    gen("ldrb\tr2,[r1]");
    gen("sub\tr2,r2,#1");
    gen("strb\tr2,[r1]");
}

static void arm_cgincsb(int a) {
    arm_cgstataddr(a, 1);
    gen("ldrb\tr2,[r1]");
    gen("add\tr2,r2,#1");
    gen("strb\tr2,[r1]");
}

static void arm_cgdecsb(int a) {
    arm_cgstataddr(a, 1);
    gen("ldrb\tr2,[r1]");
    gen("sub\tr2,r2,#1");
    gen("strb\tr2,[r1]");
}

static void arm_cgincgb(char *s) {
    arm_cgglobaddr(s, 1);
    gen("ldrb\tr2,[r1]");
    gen("add\tr2,r2,#1");
    gen("strb\tr2,[r1]");
}

static void arm_cgdecgb(char *s) {
    arm_cgglobaddr(s, 1);
    gen("ldrb\tr2,[r1]");
    gen("sub\tr2,r2,#1");
    gen("strb\tr2,[r1]");
}

static void arm_cgbr(char *how, int n) {
    int lab;
    lab = label();
    gen("cmp\tr0,#0");
    lgen("%s\t%c%d", how, lab);
    lgen("%s\t%c%d", "b", n);
    genlab(lab);
}

static void arm_cgbrtrue(int n)     { arm_cgbr("beq", n); }
static void arm_cgbrfalse(int n)    { arm_cgbr("bne", n); }
static void arm_cgjump(int n)       { lgen("%s\t%c%d", "b", n); }
static void arm_cgldswtch(int n)    { arm_cgstataddr(n, 1); }
static void arm_cgcalswtch(void)    { gen("b\tswitch"); }
static void arm_cgcase(int v, int l) { lgen2(".long\t%d,%c%d", v, l); }

static void arm_cgstorib(void)      { gen("strb\tr0,[r2]"); }
static void arm_cgstoriw(void)      { gen("str\tr0,[r2]"); }

static void arm_cgstorlb(int n) {
    arm_cglocladdr(n, 1);
    arm_cgstorb();
}

static void arm_cgstorlw(int n) {
    arm_cglocladdr(n, 1);
    arm_cgstorw();
}

static void arm_cgstorsb(int n) {
    arm_cgstataddr(n, 1);
    arm_cgstorb();
}

static void arm_cgstorsw(int n) {
    arm_cgstataddr(n, 1);
    arm_cgstorw();
}

static void arm_cgstorgb(char *s) {
    arm_cgglobaddr(s, 1);
    arm_cgstorb();
}

static void arm_cgstorgw(char *s) {
    arm_cgglobaddr(s, 1);
    arm_cgstorw();
}

static void arm_cginitlw(int v, int a) {
    arm_cglit(v);
    arm_cglocladdr(a, 1);
    arm_cgstorw();
}

static void arm_cgcall(char *s)     { sgen("%s\t%s", "bl", s); }
static void arm_cgcalr(void)        { gen("blx\tr0"); }

static void arm_cgstack(int n) {
    arm_cglit2(n, 1);
    gen("add\tsp,sp,r1");
}

static void arm_cgentry(void) {
    gen("push\t{r11,lr}");
    gen("mov\tr11,sp");
}

static void arm_cgexit(void)        { gen("pop\t{r11,pc}"); }

static void arm_cgdefb(int v)       { ngen("%s\t%d", ".byte", v); }
static void arm_cgdefw(int v)       { ngen("%s\t%d", ".long", v); }
static void arm_cgdefp(int v)       { ngen("%s\t%d", ".long", v); }
static void arm_cgdefl(int v)       { lgen("%s\t%c%d", ".long", v); }
static void arm_cgdefc(int c)       { ngen("%s\t'%c'", ".byte", c); }
static void arm_cggbss(char *s, int z) { ngen(".comm\t%s,%d", s, z); }
static void arm_cglbss(char *s, int z) { ngen(".lcomm\t%s,%d", s, z); }
static void arm_cgalign(void)       { gen(".align 2"); }

/*
 * ============================================================================
 * ARMv6 Architecture Description
 * ============================================================================
 */
struct cg_arch cg_arch_armv6 = {
    "armv6",            /* name */
    32,                 /* bits */
    1,                  /* char_size */
    4,                  /* int_size */
    4,                  /* ptr_size */
    4,                  /* bpw */
    ENDIAN_LITTLE,      /* endian */
    STACK_DOWN,         /* stack_dir */
    ASM_ARM,            /* asm_syntax */
    CC_AAPCS,           /* call_conv */
    FLOAT_IEEE754,      /* float_format */
    8,                  /* align_stack */
    4,                  /* align_data */
    4,                  /* align_func */
    1,                  /* has_mul */
    0,                  /* has_div - uses software div */
    0,                  /* has_mod - uses software mod */
    1,                  /* has_byte_ops */
    1,                  /* needs_alignment */
    8,                  /* param_offset_base */
    0,                  /* param_offset_dir */
    0                   /* local_offset_base */
};

/*
 * ============================================================================
 * ARMv6 Code Generator Vtable
 * ============================================================================
 */
struct cg_vtable cg_vtable_armv6 = {
    /* Section Control */
    arm_cgdata,
    arm_cgtext,
    arm_cgprelude,
    arm_cgpostlude,
    arm_cgpublic,
    
    /* Synthesizer Support */
    arm_cgsynth,
    arm_cgload2,
    
    /* Literal and Clear */
    arm_cglit,
    arm_cgclear,
    arm_cgclear2,
    
    /* Load Operations - Global */
    arm_cgldgb,
    arm_cgldgw,
    
    /* Load Operations - Local */
    arm_cgldlb,
    arm_cgldlw,
    
    /* Load Operations - Static */
    arm_cgldsb,
    arm_cgldsw,
    
    /* Load Address Operations */
    arm_cgldla,
    arm_cgldsa,
    arm_cgldga,
    
    /* Indirect Load Operations */
    arm_cgindb,
    arm_cgindw,
    
    /* Load Label */
    arm_cgldlab,
    
    /* Stack Operations */
    arm_cgpush,
    arm_cgpushlit,
    arm_cgpop2,
    arm_cgswap,
    arm_cgpopptr,
    
    /* Arithmetic Operations */
    arm_cgand,
    arm_cgior,
    arm_cgxor,
    arm_cgadd,
    arm_cgsub,
    arm_cgmul,
    arm_cgdiv,
    arm_cgmod,
    arm_cgshl,
    arm_cgshr,
    
    /* Unary Operations */
    arm_cgneg,
    arm_cgnot,
    arm_cglognot,
    
    /* Scaling Operations */
    arm_cgscale,
    arm_cgscale2,
    arm_cgunscale,
    arm_cgscaleby,
    arm_cgscale2by,
    arm_cgunscaleby,
    
    /* Comparison Operations */
    arm_cgeq,
    arm_cgne,
    arm_cglt,
    arm_cggt,
    arm_cgle,
    arm_cgge,
    arm_cgult,
    arm_cgugt,
    arm_cgule,
    arm_cguge,
    
    /* Conditional Branch Operations */
    arm_cgbreq,
    arm_cgbrne,
    arm_cgbrlt,
    arm_cgbrgt,
    arm_cgbrle,
    arm_cgbrge,
    arm_cgbrult,
    arm_cgbrugt,
    arm_cgbrule,
    arm_cgbruge,
    
    /* Branch Operations */
    arm_cgbrtrue,
    arm_cgbrfalse,
    arm_cgjump,
    arm_cgldswtch,
    arm_cgcalswtch,
    arm_cgcase,
    
    /* Boolean Operations */
    arm_cgbool,
    
    /* Increment/Decrement via Pointer */
    arm_cgldinc,
    arm_cginc1pi,
    arm_cgdec1pi,
    arm_cginc2pi,
    arm_cgdec2pi,
    
    /* Increment/Decrement Pointer - Local/Static/Global */
    arm_cgincpl,
    arm_cgdecpl,
    arm_cgincps,
    arm_cgdecps,
    arm_cgincpg,
    arm_cgdecpg,
    
    /* Increment/Decrement Indirect Word */
    arm_cginc1iw,
    arm_cgdec1iw,
    arm_cginc2iw,
    arm_cgdec2iw,
    
    /* Increment/Decrement Local Word */
    arm_cginclw,
    arm_cgdeclw,
    
    /* Increment/Decrement Static Word */
    arm_cgincsw,
    arm_cgdecsw,
    
    /* Increment/Decrement Global Word */
    arm_cgincgw,
    arm_cgdecgw,
    
    /* Increment/Decrement Indirect Byte */
    arm_cginc1ib,
    arm_cgdec1ib,
    arm_cginc2ib,
    arm_cgdec2ib,
    
    /* Increment/Decrement Local Byte */
    arm_cginclb,
    arm_cgdeclb,
    
    /* Increment/Decrement Static Byte */
    arm_cgincsb,
    arm_cgdecsb,
    
    /* Increment/Decrement Global Byte */
    arm_cgincgb,
    arm_cgdecgb,
    
    /* Store Operations */
    arm_cgstorib,
    arm_cgstoriw,
    arm_cgstorlb,
    arm_cgstorlw,
    arm_cgstorsb,
    arm_cgstorsw,
    arm_cgstorgb,
    arm_cgstorgw,
    
    /* Function Operations */
    arm_cginitlw,
    arm_cgcall,
    arm_cgcalr,
    arm_cgstack,
    arm_cgentry,
    arm_cgexit,
    
    /* Data Definition */
    arm_cgdefb,
    arm_cgdefw,
    arm_cgdefp,
    arm_cgdefl,
    arm_cgdefc,
    arm_cggbss,
    arm_cglbss,
    
    /* Alignment */
    arm_cgalign
};

/*
 * ============================================================================
 * External OS configurations
 * ============================================================================
 */
extern struct cg_os_config cg_os_freebsd;

/*
 * ============================================================================
 * Target Definitions for ARMv6
 * ============================================================================
 */
struct cg_target cg_target_freebsd_armv6 = {
    "freebsd-armv6",
    "FreeBSD ARMv6 (ELF, GAS syntax)",
    &cg_arch_armv6,
    &cg_os_freebsd,
    &cg_vtable_armv6,
    NULL,
    NULL
};
