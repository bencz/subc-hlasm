/*
 * SubC Compiler - AArch64 (ARM64) Code Generator
 *
 * This file implements the code generator for AArch64 architecture.
 * It provides the vtable, architecture description, and all code
 * generation functions for ARM64 targets (Linux and Darwin/Apple Silicon).
 *
 * AArch64 is the 64-bit execution state of the ARMv8 architecture.
 * Key differences from ARMv6/ARMv7:
 *   - 64-bit general purpose registers (x0-x30, sp)
 *   - 32-bit views available as w0-w30
 *   - PC is not a general purpose register
 *   - No conditional execution on most instructions
 *   - Different calling convention (AAPCS64)
 *
 * Register usage (AAPCS64):
 *   x0-x7:   Argument/result registers
 *   x8:      Indirect result location register
 *   x9-x15:  Temporary registers (caller-saved)
 *   x16-x17: Intra-procedure-call scratch registers
 *   x18:     Platform register (reserved)
 *   x19-x28: Callee-saved registers
 *   x29:     Frame pointer (fp)
 *   x30:     Link register (lr)
 *   sp:      Stack pointer
 *
 * We use:
 *   x0:  Primary accumulator
 *   x1:  Secondary register for binary operations
 *   x2:  Tertiary scratch register
 *   x29: Frame pointer
 *   x30: Link register
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
static void a64_cglit2(long v, int aux);
static void a64_cglocladdr(int n, int aux);
static void a64_cgstataddr(int n, int aux);
static void a64_cgglobaddr(char *s, int aux);
static void a64_cgind2b(void);
static void a64_cgind2w(void);
static void a64_cgstorb(void);
static void a64_cgstorw(void);
static void a64_cgcmp(char *inst);
static void a64_cgbrcond(char *i, int n);
static void a64_cgbr(char *how, int n);
static int  a64_cgload2(void);
static void a64_cgpop2(void);
static void a64_cgindb(void);
static void a64_cgindw(void);

/*
 * ============================================================================
 * Code Generation Function Implementations
 * ============================================================================
 */

static void a64_cgdata(void)        { gen(".data"); }
static void a64_cgtext(void)        { gen(".text"); }

static void a64_cgprelude(void) {
    /* No special prelude needed for AArch64 GAS */
}

static void a64_cgpostlude(void)    { }
static void a64_cgpublic(char *s)   { ngen(".globl\t%s", s, 0); }

/*
 * Load immediate value into register
 * AArch64 has limited immediate encoding, so large values need multiple instructions
 */
static void a64_cglit2(long v, int aux) {
    int l, skip;
    
    /* AArch64 can load 16-bit immediates with mov */
    if (v >= 0 && v <= 65535) {
        if (aux == 2)
            ngen("%s\tx2,#%d", "mov", (int)v);
        else if (aux == 1)
            ngen("%s\tx1,#%d", "mov", (int)v);
        else
            ngen("%s\tx0,#%d", "mov", (int)v);
    }
    else if (v >= -65536 && v < 0) {
        /* Use mov for small negative values */
        if (aux == 2)
            ngen("%s\tx2,#%d", "mov", (int)v);
        else if (aux == 1)
            ngen("%s\tx1,#%d", "mov", (int)v);
        else
            ngen("%s\tx0,#%d", "mov", (int)v);
    }
    else {
        /* For larger values, use ldr with literal pool */
        l = label();
        skip = label();
        if (aux == 2)
            lgen("%s\tx2,%c%d", "ldr", l);
        else if (aux == 1)
            lgen("%s\tx1,%c%d", "ldr", l);
        else
            lgen("%s\tx0,%c%d", "ldr", l);
        lgen("%s\t%c%d", "b", skip);
        genlab(l);
        ngen("%s\t%d", ".quad", (int)v);
        genlab(skip);
    }
}

/*
 * Simplified literal load - handles most common cases efficiently
 */
static void a64_cglit(long v) {
    int l, skip;
    
    if (v >= 0 && v <= 65535) {
        ngen("%s\tx0,#%d", "mov", (int)v);
    }
    else if (v >= -65536 && v < 0) {
        ngen("%s\tx0,#%d", "mov", (int)v);
    }
    else {
        /* Use literal pool for large constants */
        l = label();
        skip = label();
        lgen("%s\tx0,%c%d", "ldr", l);
        lgen("%s\t%c%d", "b", skip);
        genlab(l);
        ngen("%s\t%d", ".quad", (int)v);
        genlab(skip);
    }
}

static void a64_cglocladdr(int n, int aux) {
    if (n >= -255 && n <= 255) {
        if (aux)
            ngen("%s\tx1,x29,#%d", "add", n);
        else
            ngen("%s\tx0,x29,#%d", "add", n);
    }
    else if (n >= -4095 && n <= 4095) {
        if (n >= 0) {
            if (aux)
                ngen("%s\tx1,x29,#%d", "add", n);
            else
                ngen("%s\tx0,x29,#%d", "add", n);
        } else {
            if (aux)
                ngen("%s\tx1,x29,#%d", "sub", -n);
            else
                ngen("%s\tx0,x29,#%d", "sub", -n);
        }
    }
    else {
        /* Large offset - load into temp register first */
        a64_cglit2(n, 2);
        if (aux)
            gen("add\tx1,x29,x2");
        else
            gen("add\tx0,x29,x2");
    }
}

static void a64_cgstataddr(int n, int aux) {
    if (aux)
        lgen("%s\tx1,%c%d", "adr", n);
    else
        lgen("%s\tx0,%c%d", "adr", n);
}

/*
 * Load global address using literal pool (similar to ARM approach)
 * AArch64's adrp/add sequence is complex, so we use a simpler approach
 */
static void a64_cgglobaddr(char *s, int aux) {
    int l, skip;
    
    l = label();
    if (aux)
        lgen("%s\tx1,%c%d", "ldr", l);
    else
        lgen("%s\tx0,%c%d", "ldr", l);
    skip = label();
    lgen("%s\t%c%d", "b", skip);
    genlab(l);
    sgen("%s\t%s", ".quad", s);
    genlab(skip);
}

static void a64_cgind2b(void)       { gen("ldrb\tw1,[x1]"); }
static void a64_cgind2w(void)       { gen("ldr\tx1,[x1]"); }
static void a64_cgstorb(void)       { gen("strb\tw0,[x1]"); }
static void a64_cgstorw(void)       { gen("str\tx0,[x1]"); }

/* cgsynth is unused in AArch64 - returns 0 */
static void a64_cgsynth(char *op) {
    (void)op;
    /* unused */
}

static int a64_cgload2(void) {
    int n, q;
    char *s;

    n = Q_val;
    s = gsym(Q_name);
    switch (Q_type) {
    case addr_auto:     a64_cglocladdr(n, 1); break;
    case addr_static:   a64_cgstataddr(n, 1); break;
    case addr_globl:    a64_cgglobaddr(s, 1); break;
    case addr_label:    a64_cgstataddr(n, 1); break;
    case literal:       a64_cglit2(n, 1); break;
    case auto_byte:     a64_cglocladdr(n, 1);
                        a64_cgind2b();
                        break;
    case auto_word:     a64_cglocladdr(n, 1);
                        a64_cgind2w();
                        break;
    case static_byte:   a64_cgstataddr(n, 1);
                        a64_cgind2b();
                        break;
    case static_word:   a64_cgstataddr(n, 1);
                        a64_cgind2w();
                        break;
    case globl_byte:    a64_cgglobaddr(s, 1);
                        a64_cgind2b();
                        break;
    case globl_word:    a64_cgglobaddr(s, 1);
                        a64_cgind2w();
                        break;
    case empty:         a64_cgpop2();
                        break;
    default:            fatal("internal: bad type in cgload2()");
    }
    q = Q_type;
    Q_type = empty;
    return empty == q;
}

static void a64_cgclear(void)       { gen("mov\tx0,#0"); }
static void a64_cgclear2(void)      { gen("mov\tx1,#0"); }

static void a64_cgldgb(char *s) {
    a64_cgglobaddr(s, 0);
    a64_cgindb();
}

static void a64_cgldgw(char *s) {
    a64_cgglobaddr(s, 0);
    a64_cgindw();
}

static void a64_cgldlb(int n) {
    a64_cglocladdr(n, 0);
    a64_cgindb();
}

static void a64_cgldlw(int n) {
    a64_cglocladdr(n, 0);
    a64_cgindw();
}

static void a64_cgldsb(int n) {
    a64_cgstataddr(n, 0);
    a64_cgindb();
}

static void a64_cgldsw(int n) {
    a64_cgstataddr(n, 0);
    a64_cgindw();
}

static void a64_cgldla(int n)       { a64_cglocladdr(n, 0); }
static void a64_cgldsa(int n)       { a64_cgstataddr(n, 0); }
static void a64_cgldga(char *s)     { a64_cgglobaddr(s, 0); }

static void a64_cgindb(void) {
    gen("ldrb\tw0,[x0]");
}

static void a64_cgindw(void) {
    gen("ldr\tx0,[x0]");
}

static void a64_cgldlab(int id)     { a64_cgstataddr(id, 0); }

static void a64_cgpush(void)        { gen("str\tx0,[sp,#-16]!"); }

static void a64_cgpushlit(int n) {
    a64_cglit(n);
    a64_cgpush();
}

static void a64_cgpop2(void)        { gen("ldr\tx1,[sp],#16"); }

static void a64_cgswap(void) {
    gen("mov\tx2,x0");
    gen("mov\tx0,x1");
    gen("mov\tx1,x2");
}

static void a64_cgpopptr(void)      { gen("ldr\tx2,[sp],#16"); }

static void a64_cgand(void) {
    a64_cgload2();
    gen("and\tx0,x0,x1");
}

static void a64_cgior(void) {
    a64_cgload2();
    gen("orr\tx0,x0,x1");
}

static void a64_cgxor(void) {
    a64_cgload2();
    gen("eor\tx0,x0,x1");
}

static void a64_cgadd(void)         { gen("add\tx0,x0,x1"); }
static void a64_cgmul(void)         { gen("mul\tx0,x0,x1"); }
static void a64_cgsub(void)         { gen("sub\tx0,x0,x1"); }

static void a64_cgdiv(void) {
    gen("sdiv\tx0,x0,x1");
}

static void a64_cgmod(void) {
    gen("sdiv\tx2,x0,x1");
    gen("msub\tx0,x2,x1,x0");
}

static void a64_cgshl(void)         { gen("lsl\tx0,x0,x1"); }
static void a64_cgshr(void)         { gen("asr\tx0,x0,x1"); }

static void a64_cgcmp(char *cond) {
    gen("mov\tx3,x0");
    if (empty == Q_type) {
        a64_cgpop2();
        gen("cmp\tx1,x3");
    }
    else {
        a64_cgload2();
        gen("cmp\tx3,x1");
    }
    sgen("%s\tx0,%s", "cset", cond);
}

static void a64_cgeq(void)          { a64_cgcmp("eq"); }
static void a64_cgne(void)          { a64_cgcmp("ne"); }
static void a64_cglt(void)          { a64_cgcmp("lt"); }
static void a64_cggt(void)          { a64_cgcmp("gt"); }
static void a64_cgle(void)          { a64_cgcmp("le"); }
static void a64_cgge(void)          { a64_cgcmp("ge"); }
static void a64_cgult(void)         { a64_cgcmp("lo"); }
static void a64_cgugt(void)         { a64_cgcmp("hi"); }
static void a64_cgule(void)         { a64_cgcmp("ls"); }
static void a64_cguge(void)         { a64_cgcmp("hs"); }

static void a64_cgbrcond(char *i, int n) {
    int lab;
    lab = label();
    if (empty == Q_type) {
        a64_cgpop2();
        gen("cmp\tx1,x0");
    }
    else {
        a64_cgload2();
        gen("cmp\tx0,x1");
    }
    lgen("%s\t%c%d", i, lab);
    lgen("%s\t%c%d", "b", n);
    genlab(lab);
}

static void a64_cgbreq(int n)       { a64_cgbrcond("b.eq", n); }
static void a64_cgbrne(int n)       { a64_cgbrcond("b.ne", n); }
static void a64_cgbrlt(int n)       { a64_cgbrcond("b.lt", n); }
static void a64_cgbrgt(int n)       { a64_cgbrcond("b.gt", n); }
static void a64_cgbrle(int n)       { a64_cgbrcond("b.le", n); }
static void a64_cgbrge(int n)       { a64_cgbrcond("b.ge", n); }
static void a64_cgbrult(int n)      { a64_cgbrcond("b.lo", n); }
static void a64_cgbrugt(int n)      { a64_cgbrcond("b.hi", n); }
static void a64_cgbrule(int n)      { a64_cgbrcond("b.ls", n); }
static void a64_cgbruge(int n)      { a64_cgbrcond("b.hs", n); }

static void a64_cgneg(void)         { gen("neg\tx0,x0"); }
static void a64_cgnot(void)         { gen("mvn\tx0,x0"); }

static void a64_cglognot(void) {
    gen("cmp\tx0,#0");
    gen("cset\tx0,eq");
}

static void a64_cgscale(void)       { gen("lsl\tx0,x0,#3"); }
static void a64_cgscale2(void)      { gen("lsl\tx1,x1,#3"); }
static void a64_cgunscale(void)     { gen("lsr\tx0,x0,#3"); }

static void a64_cgscaleby(int v) {
    a64_cglit2(v, 1);
    gen("mul\tx0,x0,x1");
}

static void a64_cgscale2by(int v) {
    a64_cglit2(v, 2);
    gen("mul\tx1,x1,x2");
}

static void a64_cgunscaleby(int v) {
    a64_cglit2(v, 1);
    gen("udiv\tx0,x0,x1");
}

static void a64_cgbool(void) {
    gen("cmp\tx0,#0");
    gen("cset\tx0,ne");
}

static void a64_cgldinc(void)       { gen("mov\tx2,x0"); }

static void a64_cginc1pi(int v) {
    a64_cglit2(v, 1);
    gen("ldr\tx3,[x0]");
    gen("add\tx3,x3,x1");
    gen("str\tx3,[x0]");
}

static void a64_cgdec1pi(int v) {
    a64_cglit2(v, 1);
    gen("ldr\tx3,[x0]");
    gen("sub\tx3,x3,x1");
    gen("str\tx3,[x0]");
}

static void a64_cginc2pi(int v) {
    a64_cglit2(v, 1);
    gen("ldr\tx3,[x2]");
    gen("add\tx3,x3,x1");
    gen("str\tx3,[x2]");
}

static void a64_cgdec2pi(int v) {
    a64_cglit2(v, 1);
    gen("ldr\tx3,[x2]");
    gen("sub\tx3,x3,x1");
    gen("str\tx3,[x2]");
}

static void a64_cgincpl(int a, int v) {
    a64_cglit2(v, 2);
    a64_cglocladdr(a, 1);
    gen("ldr\tx3,[x1]");
    gen("add\tx3,x3,x2");
    gen("str\tx3,[x1]");
}

static void a64_cgdecpl(int a, int v) {
    a64_cglit2(v, 2);
    a64_cglocladdr(a, 1);
    gen("ldr\tx3,[x1]");
    gen("sub\tx3,x3,x2");
    gen("str\tx3,[x1]");
}

static void a64_cgincps(int a, int v) {
    a64_cglit2(v, 2);
    a64_cgstataddr(a, 1);
    gen("ldr\tx3,[x1]");
    gen("add\tx3,x3,x2");
    gen("str\tx3,[x1]");
}

static void a64_cgdecps(int a, int v) {
    a64_cglit2(v, 2);
    a64_cgstataddr(a, 1);
    gen("ldr\tx3,[x1]");
    gen("sub\tx3,x3,x2");
    gen("str\tx3,[x1]");
}

static void a64_cgincpg(char *s, int v) {
    a64_cglit2(v, 2);
    a64_cgglobaddr(s, 1);
    gen("ldr\tx3,[x1]");
    gen("add\tx3,x3,x2");
    gen("str\tx3,[x1]");
}

static void a64_cgdecpg(char *s, int v) {
    a64_cglit2(v, 2);
    a64_cgglobaddr(s, 1);
    gen("ldr\tx3,[x1]");
    gen("sub\tx3,x3,x2");
    gen("str\tx3,[x1]");
}

static void a64_cginc1iw(void) {
    gen("ldr\tx1,[x0]");
    gen("add\tx1,x1,#1");
    gen("str\tx1,[x0]");
}

static void a64_cgdec1iw(void) {
    gen("ldr\tx1,[x0]");
    gen("sub\tx1,x1,#1");
    gen("str\tx1,[x0]");
}

static void a64_cginc2iw(void) {
    gen("ldr\tx1,[x2]");
    gen("add\tx1,x1,#1");
    gen("str\tx1,[x2]");
}

static void a64_cgdec2iw(void) {
    gen("ldr\tx1,[x2]");
    gen("sub\tx1,x1,#1");
    gen("str\tx1,[x2]");
}

static void a64_cginclw(int a) {
    a64_cglocladdr(a, 1);
    gen("ldr\tx2,[x1]");
    gen("add\tx2,x2,#1");
    gen("str\tx2,[x1]");
}

static void a64_cgdeclw(int a) {
    a64_cglocladdr(a, 1);
    gen("ldr\tx2,[x1]");
    gen("sub\tx2,x2,#1");
    gen("str\tx2,[x1]");
}

static void a64_cgincsw(int a) {
    a64_cgstataddr(a, 1);
    gen("ldr\tx2,[x1]");
    gen("add\tx2,x2,#1");
    gen("str\tx2,[x1]");
}

static void a64_cgdecsw(int a) {
    a64_cgstataddr(a, 1);
    gen("ldr\tx2,[x1]");
    gen("sub\tx2,x2,#1");
    gen("str\tx2,[x1]");
}

static void a64_cgincgw(char *s) {
    a64_cgglobaddr(s, 1);
    gen("ldr\tx2,[x1]");
    gen("add\tx2,x2,#1");
    gen("str\tx2,[x1]");
}

static void a64_cgdecgw(char *s) {
    a64_cgglobaddr(s, 1);
    gen("ldr\tx2,[x1]");
    gen("sub\tx2,x2,#1");
    gen("str\tx2,[x1]");
}

static void a64_cginc1ib(void) {
    gen("ldrb\tw1,[x0]");
    gen("add\tw1,w1,#1");
    gen("strb\tw1,[x0]");
}

static void a64_cgdec1ib(void) {
    gen("ldrb\tw1,[x0]");
    gen("sub\tw1,w1,#1");
    gen("strb\tw1,[x0]");
}

static void a64_cginc2ib(void) {
    gen("ldrb\tw1,[x2]");
    gen("add\tw1,w1,#1");
    gen("strb\tw1,[x2]");
}

static void a64_cgdec2ib(void) {
    gen("ldrb\tw1,[x2]");
    gen("sub\tw1,w1,#1");
    gen("strb\tw1,[x2]");
}

static void a64_cginclb(int a) {
    a64_cglocladdr(a, 1);
    gen("ldrb\tw2,[x1]");
    gen("add\tw2,w2,#1");
    gen("strb\tw2,[x1]");
}

static void a64_cgdeclb(int a) {
    a64_cglocladdr(a, 1);
    gen("ldrb\tw2,[x1]");
    gen("sub\tw2,w2,#1");
    gen("strb\tw2,[x1]");
}

static void a64_cgincsb(int a) {
    a64_cgstataddr(a, 1);
    gen("ldrb\tw2,[x1]");
    gen("add\tw2,w2,#1");
    gen("strb\tw2,[x1]");
}

static void a64_cgdecsb(int a) {
    a64_cgstataddr(a, 1);
    gen("ldrb\tw2,[x1]");
    gen("sub\tw2,w2,#1");
    gen("strb\tw2,[x1]");
}

static void a64_cgincgb(char *s) {
    a64_cgglobaddr(s, 1);
    gen("ldrb\tw2,[x1]");
    gen("add\tw2,w2,#1");
    gen("strb\tw2,[x1]");
}

static void a64_cgdecgb(char *s) {
    a64_cgglobaddr(s, 1);
    gen("ldrb\tw2,[x1]");
    gen("sub\tw2,w2,#1");
    gen("strb\tw2,[x1]");
}

static void a64_cgbr(char *how, int n) {
    int lab;
    lab = label();
    gen("cmp\tx0,#0");
    lgen("%s\t%c%d", how, lab);
    lgen("%s\t%c%d", "b", n);
    genlab(lab);
}

static void a64_cgbrtrue(int n)     { a64_cgbr("b.eq", n); }
static void a64_cgbrfalse(int n)    { a64_cgbr("b.ne", n); }
static void a64_cgjump(int n)       { lgen("%s\t%c%d", "b", n); }
static void a64_cgldswtch(int n)    { a64_cgstataddr(n, 1); }
static void a64_cgcalswtch(void)    { gen("b\tswitch"); }
static void a64_cgcase(int v, int l) { lgen2(".quad\t%d,%c%d", v, l); }

static void a64_cgstorib(void)      { gen("strb\tw0,[x2]"); }
static void a64_cgstoriw(void)      { gen("str\tx0,[x2]"); }

static void a64_cgstorlb(int n) {
    a64_cglocladdr(n, 1);
    a64_cgstorb();
}

static void a64_cgstorlw(int n) {
    a64_cglocladdr(n, 1);
    a64_cgstorw();
}

static void a64_cgstorsb(int n) {
    a64_cgstataddr(n, 1);
    a64_cgstorb();
}

static void a64_cgstorsw(int n) {
    a64_cgstataddr(n, 1);
    a64_cgstorw();
}

static void a64_cgstorgb(char *s) {
    a64_cgglobaddr(s, 1);
    a64_cgstorb();
}

static void a64_cgstorgw(char *s) {
    a64_cgglobaddr(s, 1);
    a64_cgstorw();
}

static void a64_cginitlw(int v, int a) {
    a64_cglit(v);
    a64_cglocladdr(a, 1);
    a64_cgstorw();
}

static void a64_cgcall(char *s)     { sgen("%s\t%s", "bl", s); }
static void a64_cgcalr(void)        { gen("blr\tx0"); }

static void a64_cgstack(int n) {
    int aligned;
    
    if (n == 0) return;
    
    /* AArch64 requires 16-byte stack alignment */
    if (n > 0) {
        /* Deallocate: round up to 16-byte boundary */
        aligned = (n + 15) & ~15;
        if (aligned <= 4095) {
            ngen("%s\tsp,sp,#%d", "add", aligned);
        } else {
            a64_cglit2(aligned, 1);
            gen("add\tsp,sp,x1");
        }
    } else {
        /* Allocate: n is negative, make positive and round up */
        aligned = ((-n) + 15) & ~15;
        if (aligned <= 4095) {
            ngen("%s\tsp,sp,#%d", "sub", aligned);
        } else {
            a64_cglit2(aligned, 1);
            gen("sub\tsp,sp,x1");
        }
    }
}

static void a64_cgentry(void) {
    gen("stp\tx29,x30,[sp,#-16]!");
    gen("mov\tx29,sp");
}

static void a64_cgexit(void) {
    gen("mov\tsp,x29");
    gen("ldp\tx29,x30,[sp],#16");
    gen("ret");
}

static void a64_cgdefb(int v)       { ngen("%s\t%d", ".byte", v); }
static void a64_cgdefh(int v)       { ngen("%s\t%d", ".hword", v); }  /* 2 bytes */
static void a64_cgdefw(int v)       { ngen("%s\t%d", ".quad", v); }   /* 8 bytes (native int on AArch64) */
static void a64_cgdefd(int v)       { ngen("%s\t%d", ".word", v); }   /* 4 bytes */
static void a64_cgdefp(int v)       { ngen("%s\t%d", ".quad", v); }
static void a64_cgdefl(int v)       { lgen("%s\t%c%d", ".quad", v); }
static void a64_cgdefq(int v)       { ngen("%s\t%d", ".quad", v); }   /* 8 bytes */
static void a64_cgdefc(int c)       { ngen("%s\t'%c'", ".byte", c); }
static void a64_cggbss(char *s, int z) { ngen(".comm\t%s,%d", s, z); }
static void a64_cglbss(char *s, int z) { ngen(".lcomm\t%s,%d", s, z); }
static void a64_cgalign(void)       { gen(".align 3"); }

/*
 * ============================================================================
 * AArch64 NEON/FP Floating-Point Operations
 * ============================================================================
 *
 * AArch64 has mandatory floating-point support via the SIMD/FP unit.
 * FP registers are v0-v31 (128-bit), with scalar views:
 *   - s0-s31: 32-bit float
 *   - d0-d31: 64-bit double
 *
 * We use:
 *   d0/s0: Primary FP accumulator
 *   d1/s1: Secondary FP register for binary ops
 */

/* Load float from local variable to s0 */
static void a64_cgfloads(int n) {
    a64_cglocladdr(n, 1);
    gen("ldr\ts0,[x1]");
}

/* Load double from local variable to d0 */
static void a64_cgfloadd(int n) {
    a64_cglocladdr(n, 1);
    gen("ldr\td0,[x1]");
}

/* Load float from global symbol to s0 */
static void a64_cgfloadgs(char *s) {
    a64_cgglobaddr(s, 1);
    gen("ldr\ts0,[x1]");
}

/* Load double from global symbol to d0 */
static void a64_cgfloadgd(char *s) {
    a64_cgglobaddr(s, 1);
    gen("ldr\td0,[x1]");
}

/* Store s0 to local float variable */
static void a64_cgfstores(int n) {
    a64_cglocladdr(n, 1);
    gen("str\ts0,[x1]");
}

/* Store d0 to local double variable */
static void a64_cgfstored(int n) {
    a64_cglocladdr(n, 1);
    gen("str\td0,[x1]");
}

/* Store s0 to global float symbol */
static void a64_cgfstoregs(char *s) {
    a64_cgglobaddr(s, 1);
    gen("str\ts0,[x1]");
}

/* Store d0 to global double symbol */
static void a64_cgfstoregsd(char *s) {
    a64_cgglobaddr(s, 1);
    gen("str\td0,[x1]");
}

/* Load float literal from label */
static void a64_cgflits(int lab) {
    a64_cgstataddr(lab, 1);
    gen("ldr\ts0,[x1]");
}

/* Load double literal from label */
static void a64_cgflitd(int lab) {
    a64_cgstataddr(lab, 1);
    gen("ldr\td0,[x1]");
}

/* Float addition: s0 = s0 + s1 */
static void a64_cgfadds(void) {
    gen("fadd\ts0,s0,s1");
}

/* Double addition */
static void a64_cgfaddd(void) {
    gen("fadd\td0,d0,d1");
}

/* Float subtraction: s0 = s0 - s1 */
static void a64_cgfsubs(void) {
    gen("fsub\ts0,s0,s1");
}

/* Double subtraction */
static void a64_cgfsubd(void) {
    gen("fsub\td0,d0,d1");
}

/* Float multiplication */
static void a64_cgfmuls(void) {
    gen("fmul\ts0,s0,s1");
}

/* Double multiplication */
static void a64_cgfmuld(void) {
    gen("fmul\td0,d0,d1");
}

/* Float division */
static void a64_cgfdivs(void) {
    gen("fdiv\ts0,s0,s1");
}

/* Double division */
static void a64_cgfdivd(void) {
    gen("fdiv\td0,d0,d1");
}

/* Float negation: s0 = -s0 */
static void a64_cgfnegs(void) {
    gen("fneg\ts0,s0");
}

/* Double negation */
static void a64_cgfnegd(void) {
    gen("fneg\td0,d0");
}

/* Compare floats: s0 vs s1, set flags */
static void a64_cgfcmps(void) {
    gen("fcmp\ts0,s1");
}

/* Compare doubles */
static void a64_cgfcmpd(void) {
    gen("fcmp\td0,d1");
}

/* Float == comparison, result in x0 (0 or 1) */
static void a64_cgfeqs(void) {
    a64_cgfcmps();
    gen("cset\tx0,eq");
}

static void a64_cgfeqd(void) {
    a64_cgfcmpd();
    gen("cset\tx0,eq");
}

/* Float != comparison */
static void a64_cgfnes(void) {
    a64_cgfcmps();
    gen("cset\tx0,ne");
}

static void a64_cgfned(void) {
    a64_cgfcmpd();
    gen("cset\tx0,ne");
}

/* Float < comparison */
static void a64_cgflts(void) {
    a64_cgfcmps();
    gen("cset\tx0,lt");
}

static void a64_cgfltd(void) {
    a64_cgfcmpd();
    gen("cset\tx0,lt");
}

/* Float > comparison */
static void a64_cgfgts(void) {
    a64_cgfcmps();
    gen("cset\tx0,gt");
}

static void a64_cgfgtd(void) {
    a64_cgfcmpd();
    gen("cset\tx0,gt");
}

/* Float <= comparison */
static void a64_cgfles(void) {
    a64_cgfcmps();
    gen("cset\tx0,le");
}

static void a64_cgfled(void) {
    a64_cgfcmpd();
    gen("cset\tx0,le");
}

/* Float >= comparison */
static void a64_cgfges(void) {
    a64_cgfcmps();
    gen("cset\tx0,ge");
}

static void a64_cgfged(void) {
    a64_cgfcmpd();
    gen("cset\tx0,ge");
}

/* Convert integer (in x0) to float in s0 */
static void a64_cgitofs(void) {
    gen("scvtf\ts0,x0");
}

/* Convert integer to double in d0 */
static void a64_cgitofd(void) {
    gen("scvtf\td0,x0");
}

/* Convert float in s0 to integer in x0 */
static void a64_cgftois(void) {
    gen("fcvtzs\tx0,s0");
}

/* Convert double in d0 to integer in x0 */
static void a64_cgftoid(void) {
    gen("fcvtzs\tx0,d0");
}

/* Convert float to double */
static void a64_cgstod(void) {
    gen("fcvt\td0,s0");
}

/* Convert double to float */
static void a64_cgdtos(void) {
    gen("fcvt\ts0,d0");
}

/* Push FP value (copy s0 to s1 for binary ops) */
static void a64_cgfpush(void) {
    gen("fmov\ts1,s0");
}

/* Pop FP stack (no-op for AArch64) */
static void a64_cgfpop(void) {
    /* No operation needed */
}

/* Exchange s0 and s1 */
static void a64_cgfxch(void) {
    gen("fmov\ts2,s0");
    gen("fmov\ts0,s1");
    gen("fmov\ts1,s2");
}

/* Define float constant in data section */
static void a64_cgdeffloat(double v) {
    union { float f; unsigned int i; } u;
    u.f = (float)v;
    ngen("%s\t%u", ".word", u.i);
}

/* Define double constant in data section */
static void a64_cgdefdouble(double v) {
    union { double d; unsigned long i; } u;
    u.d = v;
    ngen("%s\t%lu", ".quad", u.i);
}

/*
 * ============================================================================
 * AArch64 AAPCS64 Calling Convention Support
 * ============================================================================
 *
 * AAPCS64 uses the following registers for integer arguments:
 *   arg 0: x0
 *   arg 1: x1
 *   arg 2: x2
 *   arg 3: x3
 *   arg 4: x4
 *   arg 5: x5
 *   arg 6: x6
 *   arg 7: x7
 *   arg 8+: pushed on stack (right to left)
 *
 * Return value is in x0.
 * x0-x7 are argument registers (caller-saved)
 * x9-x15 are temporary registers (caller-saved)
 * x19-x28 are callee-saved
 * x29 is frame pointer, x30 is link register
 */

/* Argument register names for AAPCS64 */
static char *a64_arg_regs[] = { "x0", "x1", "x2", "x3", "x4", "x5", "x6", "x7" };

/*
 * cgpusharg - Move accumulator to argument position
 * For args 0-7: move to appropriate register
 * For args 8+: push to stack
 */
static void a64_cgpusharg(int argnum) {
    if (argnum < 8) {
        sgen("%s\t%s,x0", "mov", a64_arg_regs[argnum]);
    } else {
        gen("str\tx0,[sp,#-16]!");
    }
}

/* cgcallprep - Prepare for function call (align stack if needed) */
static void a64_cgcallprep(int nargs) {
    int stack_args;
    
    if (nargs <= 8) {
        return;
    }
    
    stack_args = nargs - 8;
    /* Ensure 16-byte stack alignment for AAPCS64 */
    if (stack_args & 1) {
        gen("sub\tsp,sp,#16");
    }
}

/*
 * cgcallend - Clean up after function call
 * Adjust stack pointer for stack-passed arguments only
 */
static void a64_cgcallend(int nargs) {
    int stack_args;
    int adjust;
    
    if (nargs <= 8) {
        return;  /* No stack cleanup needed */
    }
    
    stack_args = nargs - 8;
    adjust = stack_args * 16;  /* Each push is 16 bytes for alignment */
    
    /* Add alignment padding if we added it in cgcallprep */
    if (stack_args & 1) {
        adjust += 16;
    }
    
    ngen("%s\tsp,sp,#%d", "add", adjust);
}

/*
 * cgfnentry - Function entry with parameter info
 * For functions with register args, save registers to stack
 * so they can be accessed via frame pointer offsets.
 *
 * Stack layout after cgfnentry for a function with N params (N <= 8):
 *   x29-8:   saved x0 (arg 0)
 *   x29-16:  saved x1 (arg 1)
 *   ...
 *   x29-64:  saved x7 (arg 7)
 *
 * For variadic functions (nparams < 0), we save all 8 argument registers.
 */
static void a64_cgfnentry(int nparams) {
    int save_count;
    
    gen("stp\tx29,x30,[sp,#-16]!");
    gen("mov\tx29,sp");
    
    /* Determine how many registers to save */
    if (nparams < 0) {
        /* Variadic function: save all 8 argument registers */
        save_count = 8;
    } else if (nparams > 8) {
        save_count = 8;
    } else {
        save_count = nparams;
    }
    
    /*
     * Save argument registers to stack for access via frame pointer.
     * decl.c expects: arg0 at fp-8, arg1 at fp-16, arg2 at fp-24, etc.
     * We use str with pre-decrement to match this layout.
     */
    if (save_count >= 1) gen("str\tx0,[sp,#-8]!");
    if (save_count >= 2) gen("str\tx1,[sp,#-8]!");
    if (save_count >= 3) gen("str\tx2,[sp,#-8]!");
    if (save_count >= 4) gen("str\tx3,[sp,#-8]!");
    if (save_count >= 5) gen("str\tx4,[sp,#-8]!");
    if (save_count >= 6) gen("str\tx5,[sp,#-8]!");
    if (save_count >= 7) gen("str\tx6,[sp,#-8]!");
    if (save_count >= 8) gen("str\tx7,[sp,#-8]!");
    
    /* Ensure 16-byte stack alignment after saving odd number of registers */
    if (save_count > 0 && (save_count & 1)) {
        gen("sub\tsp,sp,#8");
    }
}

/*
 * ============================================================================
 * Symbol Transform Functions
 * ============================================================================
 */

/* Darwin/macOS requires underscore prefix for C symbols */
static char *a64_darwin_symbol_transform(char *s) {
    static char name[NAMELEN+2];
    name[0] = '_';
    copyname(&name[1], s);
    return name;
}

/*
 * ============================================================================
 * AArch64 Architecture Description
 * ============================================================================
 */
struct cg_arch cg_arch_aarch64 = {
    "aarch64",          /* name */
    64,                 /* bits */
    1,                  /* char_size */
    2,                  /* short_size */
    8,                  /* int_size (SubC uses 64-bit int on AArch64) */
    8,                  /* long_size */
    8,                  /* ptr_size */
    4,                  /* float_size */
    8,                  /* double_size */
    8,                  /* bpw */
    ENDIAN_LITTLE,      /* endian */
    STACK_DOWN,         /* stack_dir */
    ASM_GAS,            /* asm_syntax */
    CC_AAPCS64,         /* call_conv */
    FLOAT_IEEE754,      /* float_format */
    FPU_VFP,            /* fpu_type - NEON/FP (mandatory on AArch64) */
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
    8,                  /* num_arg_regs (x0-x7) */
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

/* Darwin/macOS AArch64 (Apple Silicon): same as aarch64 but with underscore prefix */
struct cg_arch cg_arch_aarch64_darwin = {
    "aarch64-darwin",   /* name */
    64,                 /* bits */
    1,                  /* char_size */
    2,                  /* short_size */
    8,                  /* int_size */
    8,                  /* long_size */
    8,                  /* ptr_size */
    4,                  /* float_size */
    8,                  /* double_size */
    8,                  /* bpw */
    ENDIAN_LITTLE,      /* endian */
    STACK_DOWN,         /* stack_dir */
    ASM_GAS,            /* asm_syntax */
    CC_AAPCS64,         /* call_conv */
    FLOAT_IEEE754,      /* float_format */
    FPU_VFP,            /* fpu_type */
    16,                 /* align_stack */
    8,                  /* align_data */
    16,                 /* align_func */
    1,                  /* has_mul */
    1,                  /* has_div */
    1,                  /* has_mod */
    1,                  /* has_byte_ops */
    0,                  /* needs_alignment */
    16,                 /* param_offset_base */
    1,                  /* param_offset_dir */
    0,                  /* local_offset_base */
    -1,                 /* local_offset_dir */
    8,                  /* num_arg_regs (x0-x7) */
    a64_darwin_symbol_transform,  /* symbol_transform (underscore prefix) */
    /* Compiler limits - 0 means use defaults */
    0,                  /* namelen */
    0,                  /* max_case */
    0,                  /* max_break */
    0,                  /* max_locinit */
    0,                  /* max_fnargs */
    0,                  /* nsymbols */
    0,                  /* poolsize */
    0,                  /* nodepoolsz */
    0                   /* label_prefix */
};

/*
 * ============================================================================
 * AArch64 Code Generator Vtable
 * ============================================================================
 */
struct cg_vtable cg_vtable_aarch64 = {
    /* Section Control */
    a64_cgdata,
    a64_cgtext,
    a64_cgprelude,
    a64_cgpostlude,
    a64_cgpublic,
    
    /* Synthesizer Support */
    a64_cgsynth,
    a64_cgload2,
    
    /* Literal and Clear */
    a64_cglit,
    a64_cgclear,
    a64_cgclear2,
    
    /* Load Operations - Global */
    a64_cgldgb,
    a64_cgldgw,
    
    /* Load Operations - Local */
    a64_cgldlb,
    a64_cgldlw,
    
    /* Load Operations - Static */
    a64_cgldsb,
    a64_cgldsw,
    
    /* Load Address Operations */
    a64_cgldla,
    a64_cgldsa,
    a64_cgldga,
    
    /* Indirect Load Operations */
    a64_cgindb,
    a64_cgindw,
    
    /* Load Label */
    a64_cgldlab,
    
    /* Stack Operations */
    a64_cgpush,
    a64_cgpushlit,
    a64_cgpop2,
    a64_cgswap,
    a64_cgpopptr,
    
    /* Arithmetic Operations */
    a64_cgand,
    a64_cgior,
    a64_cgxor,
    a64_cgadd,
    a64_cgsub,
    a64_cgmul,
    a64_cgdiv,
    a64_cgmod,
    a64_cgshl,
    a64_cgshr,
    
    /* Unary Operations */
    a64_cgneg,
    a64_cgnot,
    a64_cglognot,
    
    /* Scaling Operations */
    a64_cgscale,
    a64_cgscale2,
    a64_cgunscale,
    a64_cgscaleby,
    a64_cgscale2by,
    a64_cgunscaleby,
    
    /* Comparison Operations */
    a64_cgeq,
    a64_cgne,
    a64_cglt,
    a64_cggt,
    a64_cgle,
    a64_cgge,
    a64_cgult,
    a64_cgugt,
    a64_cgule,
    a64_cguge,
    
    /* Conditional Branch Operations */
    a64_cgbreq,
    a64_cgbrne,
    a64_cgbrlt,
    a64_cgbrgt,
    a64_cgbrle,
    a64_cgbrge,
    a64_cgbrult,
    a64_cgbrugt,
    a64_cgbrule,
    a64_cgbruge,
    
    /* Branch Operations */
    a64_cgbrtrue,
    a64_cgbrfalse,
    a64_cgjump,
    a64_cgldswtch,
    a64_cgcalswtch,
    a64_cgcase,
    
    /* Boolean Operations */
    a64_cgbool,
    
    /* Increment/Decrement via Pointer */
    a64_cgldinc,
    a64_cginc1pi,
    a64_cgdec1pi,
    a64_cginc2pi,
    a64_cgdec2pi,
    
    /* Increment/Decrement Pointer - Local/Static/Global */
    a64_cgincpl,
    a64_cgdecpl,
    a64_cgincps,
    a64_cgdecps,
    a64_cgincpg,
    a64_cgdecpg,
    
    /* Increment/Decrement Indirect Word */
    a64_cginc1iw,
    a64_cgdec1iw,
    a64_cginc2iw,
    a64_cgdec2iw,
    
    /* Increment/Decrement Local Word */
    a64_cginclw,
    a64_cgdeclw,
    
    /* Increment/Decrement Static Word */
    a64_cgincsw,
    a64_cgdecsw,
    
    /* Increment/Decrement Global Word */
    a64_cgincgw,
    a64_cgdecgw,
    
    /* Increment/Decrement Indirect Byte */
    a64_cginc1ib,
    a64_cgdec1ib,
    a64_cginc2ib,
    a64_cgdec2ib,
    
    /* Increment/Decrement Local Byte */
    a64_cginclb,
    a64_cgdeclb,
    
    /* Increment/Decrement Static Byte */
    a64_cgincsb,
    a64_cgdecsb,
    
    /* Increment/Decrement Global Byte */
    a64_cgincgb,
    a64_cgdecgb,
    
    /* Store Operations */
    a64_cgstorib,
    a64_cgstoriw,
    a64_cgstorlb,
    a64_cgstorlw,
    a64_cgstorsb,
    a64_cgstorsw,
    a64_cgstorgb,
    a64_cgstorgw,
    
    /* Function Operations */
    a64_cginitlw,
    a64_cgcall,
    a64_cgcalr,
    a64_cgstack,
    a64_cgentry,
    a64_cgexit,
    
    /* ABI-Compliant Calling Convention */
    a64_cgpusharg,
    a64_cgcallprep,
    a64_cgcallend,
    a64_cgfnentry,
    
    /* Data Definition */
    a64_cgdefb,
    a64_cgdefh,
    a64_cgdefw,
    a64_cgdefd,
    a64_cgdefp,
    a64_cgdefl,
    a64_cgdefc,
    a64_cgdefq,
    a64_cggbss,
    a64_cglbss,
    
    /* Alignment */
    a64_cgalign,
    
    /* Floating-Point Operations - NEON/FP */
    a64_cgfloads,
    a64_cgfloadd,
    a64_cgfloadgs,
    a64_cgfloadgd,
    a64_cgfstores,
    a64_cgfstored,
    a64_cgfstoregs,
    a64_cgfstoregsd,
    a64_cgflits,
    a64_cgflitd,
    a64_cgfadds,
    a64_cgfaddd,
    a64_cgfsubs,
    a64_cgfsubd,
    a64_cgfmuls,
    a64_cgfmuld,
    a64_cgfdivs,
    a64_cgfdivd,
    a64_cgfnegs,
    a64_cgfnegd,
    a64_cgfcmps,
    a64_cgfcmpd,
    a64_cgfeqs,
    a64_cgfeqd,
    a64_cgfnes,
    a64_cgfned,
    a64_cgflts,
    a64_cgfltd,
    a64_cgfgts,
    a64_cgfgtd,
    a64_cgfles,
    a64_cgfled,
    a64_cgfges,
    a64_cgfged,
    a64_cgitofs,
    a64_cgitofd,
    a64_cgftois,
    a64_cgftoid,
    a64_cgstod,
    a64_cgdtos,
    a64_cgfpush,
    a64_cgfpop,
    a64_cgfxch,
    a64_cgdeffloat,
    a64_cgdefdouble
};

/*
 * ============================================================================
 * External OS configurations
 * ============================================================================
 */
extern struct cg_os_config cg_os_linux;
extern struct cg_os_config cg_os_darwin;

/*
 * ============================================================================
 * Target Definitions for AArch64
 * ============================================================================
 */
struct cg_target cg_target_linux_aarch64 = {
    "linux-aarch64",
    "Linux AArch64/ARM64 (ELF, GAS syntax)",
    &cg_arch_aarch64,
    &cg_os_linux,
    &cg_vtable_aarch64,
    NULL,
    NULL
};

struct cg_target cg_target_darwin_aarch64 = {
    "darwin-aarch64",
    "Darwin/macOS AArch64 (Apple Silicon, Mach-O, GAS syntax)",
    &cg_arch_aarch64_darwin,
    &cg_os_darwin,
    &cg_vtable_aarch64,
    NULL,
    NULL
};
