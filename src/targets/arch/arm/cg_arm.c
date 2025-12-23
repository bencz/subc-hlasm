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
static void arm_cgdefh(int v)       { ngen("%s\t%d", ".short", v); }  /* 2 bytes */
static void arm_cgdefw(int v)       { ngen("%s\t%d", ".long", v); }   /* 4 bytes (native int) */
static void arm_cgdefd(int v)       { ngen("%s\t%d", ".long", v); }   /* 4 bytes (for long) */
static void arm_cgdefp(int v)       { ngen("%s\t%d", ".long", v); }
static void arm_cgdefl(int v)       { lgen("%s\t%c%d", ".long", v); }
static void arm_cgdefq(int v)       { ngen("%s\t%d", ".long", v); ngen("%s\t%d", ".long", 0); } /* 8 bytes */
static void arm_cgdefc(int c)       { ngen("%s\t'%c'", ".byte", c); }
static void arm_cggbss(char *s, int z) { ngen(".comm\t%s,%d", s, z); }
static void arm_cglbss(char *s, int z) { ngen(".lcomm\t%s,%d", s, z); }
static void arm_cgalign(void)       { gen(".align 2"); }

/*
 * ============================================================================
 * ARM VFP Floating-Point Operations
 * ============================================================================
 *
 * ARM VFP (Vector Floating Point) provides hardware floating-point support.
 * VFPv2 and later use s0-s31 for single-precision and d0-d15 for double.
 * We use s0/d0 as the primary FP accumulator and s1/d1 as secondary.
 *
 * IEEE 754 format:
 *   float (32-bit):  1 sign + 8 exponent + 23 mantissa
 *   double (64-bit): 1 sign + 11 exponent + 52 mantissa
 */

/* Load float from local variable to s0 */
static void arm_cgfloads(int n) {
    arm_cglocladdr(n, 1);
    gen("vldr\ts0,[r1]");
}

/* Load double from local variable to d0 */
static void arm_cgfloadd(int n) {
    arm_cglocladdr(n, 1);
    gen("vldr\td0,[r1]");
}

/* Load float from global symbol to s0 */
static void arm_cgfloadgs(char *s) {
    arm_cgglobaddr(s, 1);
    gen("vldr\ts0,[r1]");
}

/* Load double from global symbol to d0 */
static void arm_cgfloadgd(char *s) {
    arm_cgglobaddr(s, 1);
    gen("vldr\td0,[r1]");
}

/* Store s0 to local float variable */
static void arm_cgfstores(int n) {
    arm_cglocladdr(n, 1);
    gen("vstr\ts0,[r1]");
}

/* Store d0 to local double variable */
static void arm_cgfstored(int n) {
    arm_cglocladdr(n, 1);
    gen("vstr\td0,[r1]");
}

/* Store s0 to global float symbol */
static void arm_cgfstoregs(char *s) {
    arm_cgglobaddr(s, 1);
    gen("vstr\ts0,[r1]");
}

/* Store d0 to global double symbol */
static void arm_cgfstoregsd(char *s) {
    arm_cgglobaddr(s, 1);
    gen("vstr\td0,[r1]");
}

/* Load float literal from label */
static void arm_cgflits(int lab) {
    arm_cgstataddr(lab, 1);
    gen("vldr\ts0,[r1]");
}

/* Load double literal from label */
static void arm_cgflitd(int lab) {
    arm_cgstataddr(lab, 1);
    gen("vldr\td0,[r1]");
}

/* Float addition: s0 = s0 + s1 */
static void arm_cgfadds(void) {
    gen("vadd.f32\ts0,s0,s1");
}

/* Double addition */
static void arm_cgfaddd(void) {
    gen("vadd.f64\td0,d0,d1");
}

/* Float subtraction: s0 = s0 - s1 */
static void arm_cgfsubs(void) {
    gen("vsub.f32\ts0,s0,s1");
}

/* Double subtraction */
static void arm_cgfsubd(void) {
    gen("vsub.f64\td0,d0,d1");
}

/* Float multiplication */
static void arm_cgfmuls(void) {
    gen("vmul.f32\ts0,s0,s1");
}

/* Double multiplication */
static void arm_cgfmuld(void) {
    gen("vmul.f64\td0,d0,d1");
}

/* Float division */
static void arm_cgfdivs(void) {
    gen("vdiv.f32\ts0,s0,s1");
}

/* Double division */
static void arm_cgfdivd(void) {
    gen("vdiv.f64\td0,d0,d1");
}

/* Float negation: s0 = -s0 */
static void arm_cgfnegs(void) {
    gen("vneg.f32\ts0,s0");
}

/* Double negation */
static void arm_cgfnegd(void) {
    gen("vneg.f64\td0,d0");
}

/* Compare floats: s0 vs s1, set FPSCR flags */
static void arm_cgfcmps(void) {
    gen("vcmp.f32\ts0,s1");
    gen("vmrs\tAPSR_nzcv,FPSCR");
}

/* Compare doubles */
static void arm_cgfcmpd(void) {
    gen("vcmp.f64\td0,d1");
    gen("vmrs\tAPSR_nzcv,FPSCR");
}

/* Float == comparison, result in r0 (0 or 1) */
static void arm_cgfeqs(void) {
    arm_cgfcmps();
    gen("mov\tr0,#0");
    gen("moveq\tr0,#1");
}

static void arm_cgfeqd(void) {
    arm_cgfcmpd();
    gen("mov\tr0,#0");
    gen("moveq\tr0,#1");
}

/* Float != comparison */
static void arm_cgfnes(void) {
    arm_cgfcmps();
    gen("mov\tr0,#0");
    gen("movne\tr0,#1");
}

static void arm_cgfned(void) {
    arm_cgfcmpd();
    gen("mov\tr0,#0");
    gen("movne\tr0,#1");
}

/* Float < comparison */
static void arm_cgflts(void) {
    arm_cgfcmps();
    gen("mov\tr0,#0");
    gen("movlt\tr0,#1");
}

static void arm_cgfltd(void) {
    arm_cgfcmpd();
    gen("mov\tr0,#0");
    gen("movlt\tr0,#1");
}

/* Float > comparison */
static void arm_cgfgts(void) {
    arm_cgfcmps();
    gen("mov\tr0,#0");
    gen("movgt\tr0,#1");
}

static void arm_cgfgtd(void) {
    arm_cgfcmpd();
    gen("mov\tr0,#0");
    gen("movgt\tr0,#1");
}

/* Float <= comparison */
static void arm_cgfles(void) {
    arm_cgfcmps();
    gen("mov\tr0,#0");
    gen("movle\tr0,#1");
}

static void arm_cgfled(void) {
    arm_cgfcmpd();
    gen("mov\tr0,#0");
    gen("movle\tr0,#1");
}

/* Float >= comparison */
static void arm_cgfges(void) {
    arm_cgfcmps();
    gen("mov\tr0,#0");
    gen("movge\tr0,#1");
}

static void arm_cgfged(void) {
    arm_cgfcmpd();
    gen("mov\tr0,#0");
    gen("movge\tr0,#1");
}

/* Convert integer (in r0) to float in s0 */
static void arm_cgitofs(void) {
    gen("vmov\ts0,r0");
    gen("vcvt.f32.s32\ts0,s0");
}

/* Convert integer to double in d0 */
static void arm_cgitofd(void) {
    gen("vmov\ts0,r0");
    gen("vcvt.f64.s32\td0,s0");
}

/* Convert float in s0 to integer in r0 */
static void arm_cgftois(void) {
    gen("vcvt.s32.f32\ts0,s0");
    gen("vmov\tr0,s0");
}

/* Convert double in d0 to integer in r0 */
static void arm_cgftoid(void) {
    gen("vcvt.s32.f64\ts0,d0");
    gen("vmov\tr0,s0");
}

/* Convert float to double */
static void arm_cgstod(void) {
    gen("vcvt.f64.f32\td0,s0");
}

/* Convert double to float */
static void arm_cgdtos(void) {
    gen("vcvt.f32.f64\ts0,d0");
}

/* Push FP value (copy s0 to s1 for binary ops) */
static void arm_cgfpush(void) {
    gen("vmov.f32\ts1,s0");
}

/* Pop FP stack (no-op for VFP) */
static void arm_cgfpop(void) {
    /* No operation needed */
}

/* Exchange s0 and s1 */
static void arm_cgfxch(void) {
    gen("vmov.f32\ts2,s0");
    gen("vmov.f32\ts0,s1");
    gen("vmov.f32\ts1,s2");
}

/* Define float constant in data section */
static void arm_cgdeffloat(int lab, unsigned int bits) {
    genlab(lab);
    ngen("%s\t%u", ".long", bits);
}

/* Define double constant in data section */
static void arm_cgdefdouble(int lab, unsigned int hi, unsigned int lo) {
    genlab(lab);
    ngen("%s\t%u", ".long", lo);
    ngen("%s\t%u", ".long", hi);
}

/*
 * ============================================================================
 * ARM AAPCS Calling Convention Support
 * ============================================================================
 *
 * ARM AAPCS uses the following registers for integer arguments:
 *   arg 0: r0
 *   arg 1: r1
 *   arg 2: r2
 *   arg 3: r3
 *   arg 4+: pushed on stack (right to left)
 *
 * Return value is in r0.
 * r0-r3 are caller-saved (scratch registers)
 * r4-r11 are callee-saved
 */

/* Argument register names for AAPCS */
static char *arm_arg_regs[] = { "r0", "r1", "r2", "r3" };

/*
 * cgpusharg - Move accumulator to argument position
 * For args 0-3: move to appropriate register
 * For args 4+: push to stack
 */
static void arm_cgpusharg(int argnum) {
    if (argnum < 4) {
        sgen("%s\t%s,r0", "mov", arm_arg_regs[argnum]);
    } else {
        gen("push\t{r0}");
    }
}

/* cgcallprep - Prepare for function call (align stack if needed) */
static void arm_cgcallprep(int nargs) {
    int stack_args;
    
    if (nargs <= 4) {
        return;
    }
    
    stack_args = nargs - 4;
    /* Ensure 8-byte stack alignment for AAPCS */
    if (stack_args & 1) {
        gen("sub\tsp,sp,#4");
    }
}

/*
 * cgcallend - Clean up after function call
 * Adjust stack pointer for stack-passed arguments only
 */
static void arm_cgcallend(int nargs) {
    int stack_args;
    int adjust;
    
    if (nargs <= 4) {
        return;  /* No stack cleanup needed */
    }
    
    stack_args = nargs - 4;
    adjust = stack_args * 4;
    
    /* Add alignment padding if we added it in cgcallprep */
    if (stack_args & 1) {
        adjust += 4;
    }
    
    ngen("%s\tsp,sp,#%d", "add", adjust);
}

/*
 * cgfnentry - Function entry with parameter info
 * For functions with register args, save registers to stack
 * so they can be accessed via frame pointer offsets.
 *
 * Stack layout after cgfnentry for a function with N params (N <= 4):
 *   r11-4:   saved r0 (arg 0)
 *   r11-8:   saved r1 (arg 1)
 *   r11-12:  saved r2 (arg 2)
 *   r11-16:  saved r3 (arg 3)
 *   r11-16-N: local variables start here
 *
 * For variadic functions (nparams < 0), we save all 4 argument registers.
 */
static void arm_cgfnentry(int nparams) {
    int save_count;
    
    gen("push\t{r11,lr}");
    gen("mov\tr11,sp");
    
    /* Determine how many registers to save */
    if (nparams < 0) {
        /* Variadic function: save all 4 argument registers */
        save_count = 4;
    } else if (nparams > 4) {
        save_count = 4;
    } else {
        save_count = nparams;
    }
    
    /* Save argument registers to stack */
    if (save_count == 4) {
        gen("push\t{r0,r1,r2,r3}");
    } else if (save_count == 3) {
        gen("push\t{r0,r1,r2}");
    } else if (save_count == 2) {
        gen("push\t{r0,r1}");
    } else if (save_count == 1) {
        gen("push\t{r0}");
    }
}

/*
 * ============================================================================
 * ARMv6 Architecture Description
 * ============================================================================
 */
struct cg_arch cg_arch_armv6 = {
    "armv6",            /* name */
    32,                 /* bits */
    1,                  /* char_size */
    2,                  /* short_size */
    4,                  /* int_size */
    4,                  /* long_size */
    4,                  /* ptr_size */
    4,                  /* float_size */
    8,                  /* double_size */
    4,                  /* bpw */
    ENDIAN_LITTLE,      /* endian */
    STACK_DOWN,         /* stack_dir */
    ASM_ARM,            /* asm_syntax */
    CC_AAPCS,           /* call_conv */
    FLOAT_IEEE754,      /* float_format */
    FPU_VFP,            /* fpu_type - VFP (if available) or emulated */
    8,                  /* align_stack */
    4,                  /* align_data */
    4,                  /* align_func */
    1,                  /* has_mul */
    0,                  /* has_div - uses software div */
    0,                  /* has_mod - uses software mod */
    1,                  /* has_byte_ops */
    1,                  /* needs_alignment */
    8,                  /* param_offset_base - NOT USED for register args */
    1,                  /* param_offset_dir */
    0,                  /* local_offset_base - adjusted by cgfnentry */
    -1,                 /* local_offset_dir (negative: -4, -8, -12...) */
    4,                  /* num_arg_regs (r0, r1, r2, r3) */
    NULL                /* symbol_transform (use default) */
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
    
    /* ABI-Compliant Calling Convention */
    arm_cgpusharg,
    arm_cgcallprep,
    arm_cgcallend,
    arm_cgfnentry,
    
    /* Data Definition */
    arm_cgdefb,
    arm_cgdefh,
    arm_cgdefw,
    arm_cgdefd,
    arm_cgdefp,
    arm_cgdefl,
    arm_cgdefc,
    arm_cgdefq,
    arm_cggbss,
    arm_cglbss,
    
    /* Alignment */
    arm_cgalign,
    
    /* Floating-Point Operations - VFP */
    arm_cgfloads,
    arm_cgfloadd,
    arm_cgfloadgs,
    arm_cgfloadgd,
    arm_cgfstores,
    arm_cgfstored,
    arm_cgfstoregs,
    arm_cgfstoregsd,
    arm_cgflits,
    arm_cgflitd,
    arm_cgfadds,
    arm_cgfaddd,
    arm_cgfsubs,
    arm_cgfsubd,
    arm_cgfmuls,
    arm_cgfmuld,
    arm_cgfdivs,
    arm_cgfdivd,
    arm_cgfnegs,
    arm_cgfnegd,
    arm_cgfcmps,
    arm_cgfcmpd,
    arm_cgfeqs,
    arm_cgfeqd,
    arm_cgfnes,
    arm_cgfned,
    arm_cgflts,
    arm_cgfltd,
    arm_cgfgts,
    arm_cgfgtd,
    arm_cgfles,
    arm_cgfled,
    arm_cgfges,
    arm_cgfged,
    arm_cgitofs,
    arm_cgitofd,
    arm_cgftois,
    arm_cgftoid,
    arm_cgstod,
    arm_cgdtos,
    arm_cgfpush,
    arm_cgfpop,
    arm_cgfxch,
    arm_cgdeffloat,
    arm_cgdefdouble
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
