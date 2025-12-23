/*
 * SubC Compiler - Code Generator Compatibility Layer
 *
 * This header provides backward-compatible macros that redirect the
 * traditional cg*() function calls through the current target's vtable.
 *
 * Usage:
 *   Instead of calling cgadd() directly, code calls cgadd() which
 *   expands to CG->vtable->cgadd().
 *
 * This allows existing code to work unchanged while supporting
 * multiple targets through the vtable mechanism.
 *
 * Copyright (c) 2011-2025 - Public Domain (CC0)
 */

#ifndef CGEN_COMPAT_H
#define CGEN_COMPAT_H

#include "cgtarget.h"

/*
 * ============================================================================
 * Section Control
 * ============================================================================
 */
#define cgdata()        (CG->vtable->cgdata())
#define cgtext()        (CG->vtable->cgtext())
#define cgprelude()     (CG->vtable->cgprelude())
#define cgpostlude()    (CG->vtable->cgpostlude())
#define cgpublic(s)     (CG->vtable->cgpublic(s))
#define cgalign()       (CG->vtable->cgalign())

/*
 * ============================================================================
 * Literal and Clear
 * ============================================================================
 */
#define cglit(v)        (CG->vtable->cglit(v))
#define cgclear()       (CG->vtable->cgclear())
#define cgclear2()      (CG->vtable->cgclear2())

/*
 * ============================================================================
 * Load Operations
 * ============================================================================
 */
#define cgldgb(s)       (CG->vtable->cgldgb(s))
#define cgldgw(s)       (CG->vtable->cgldgw(s))
#define cgldlb(n)       (CG->vtable->cgldlb(n))
#define cgldlw(n)       (CG->vtable->cgldlw(n))
#define cgldsb(n)       (CG->vtable->cgldsb(n))
#define cgldsw(n)       (CG->vtable->cgldsw(n))
#define cgldla(n)       (CG->vtable->cgldla(n))
#define cgldsa(n)       (CG->vtable->cgldsa(n))
#define cgldga(s)       (CG->vtable->cgldga(s))
#define cgindb()        (CG->vtable->cgindb())
#define cgindw()        (CG->vtable->cgindw())
#define cgldlab(id)     (CG->vtable->cgldlab(id))

/*
 * ============================================================================
 * Stack Operations
 * ============================================================================
 */
#define cgpush()        (CG->vtable->cgpush())
#define cgpushlit(n)    (CG->vtable->cgpushlit(n))
#define cgpop2()        (CG->vtable->cgpop2())
#define cgswap()        (CG->vtable->cgswap())
#define cgpopptr()      (CG->vtable->cgpopptr())

/*
 * ============================================================================
 * Arithmetic Operations
 * ============================================================================
 */
#define cgadd()         (CG->vtable->cgadd())
#define cgsub()         (CG->vtable->cgsub())
#define cgmul()         (CG->vtable->cgmul())
#define cgdiv()         (CG->vtable->cgdiv())
#define cgmod()         (CG->vtable->cgmod())
#define cgneg()         (CG->vtable->cgneg())

/*
 * ============================================================================
 * Bitwise Operations
 * ============================================================================
 */
#define cgand()         (CG->vtable->cgand())
#define cgior()         (CG->vtable->cgior())
#define cgxor()         (CG->vtable->cgxor())
#define cgnot()         (CG->vtable->cgnot())
#define cgshl()         (CG->vtable->cgshl())
#define cgshr()         (CG->vtable->cgshr())

/*
 * ============================================================================
 * Comparison Operations
 * ============================================================================
 */
#define cgeq()          (CG->vtable->cgeq())
#define cgne()          (CG->vtable->cgne())
#define cglt()          (CG->vtable->cglt())
#define cggt()          (CG->vtable->cggt())
#define cgle()          (CG->vtable->cgle())
#define cgge()          (CG->vtable->cgge())
#define cgult()         (CG->vtable->cgult())
#define cgugt()         (CG->vtable->cgugt())
#define cgule()         (CG->vtable->cgule())
#define cguge()         (CG->vtable->cguge())

/*
 * ============================================================================
 * Conditional Branch Operations
 * ============================================================================
 */
#define cgbreq(n)       (CG->vtable->cgbreq(n))
#define cgbrne(n)       (CG->vtable->cgbrne(n))
#define cgbrlt(n)       (CG->vtable->cgbrlt(n))
#define cgbrgt(n)       (CG->vtable->cgbrgt(n))
#define cgbrle(n)       (CG->vtable->cgbrle(n))
#define cgbrge(n)       (CG->vtable->cgbrge(n))
#define cgbrult(n)      (CG->vtable->cgbrult(n))
#define cgbrugt(n)      (CG->vtable->cgbrugt(n))
#define cgbrule(n)      (CG->vtable->cgbrule(n))
#define cgbruge(n)      (CG->vtable->cgbruge(n))

/*
 * ============================================================================
 * Boolean and Logic Operations
 * ============================================================================
 */
#define cglognot()      (CG->vtable->cglognot())
#define cgbool()        (CG->vtable->cgbool())

/*
 * ============================================================================
 * Pointer Scaling Operations
 * ============================================================================
 */
#define cgscale()       (CG->vtable->cgscale())
#define cgscale2()      (CG->vtable->cgscale2())
#define cgunscale()     (CG->vtable->cgunscale())
#define cgscaleby(v)    (CG->vtable->cgscaleby(v))
#define cgscale2by(v)   (CG->vtable->cgscale2by(v))
#define cgunscaleby(v)  (CG->vtable->cgunscaleby(v))

/*
 * ============================================================================
 * Increment/Decrement via Pointer
 * ============================================================================
 */
#define cgldinc()       (CG->vtable->cgldinc())
#define cginc1pi(v)     (CG->vtable->cginc1pi(v))
#define cgdec1pi(v)     (CG->vtable->cgdec1pi(v))
#define cginc2pi(v)     (CG->vtable->cginc2pi(v))
#define cgdec2pi(v)     (CG->vtable->cgdec2pi(v))

/*
 * ============================================================================
 * Increment/Decrement Local Variables
 * ============================================================================
 */
#define cgincpl(a,v)    (CG->vtable->cgincpl(a,v))
#define cgdecpl(a,v)    (CG->vtable->cgdecpl(a,v))
#define cginclw(a)      (CG->vtable->cginclw(a))
#define cgdeclw(a)      (CG->vtable->cgdeclw(a))
#define cginclb(a)      (CG->vtable->cginclb(a))
#define cgdeclb(a)      (CG->vtable->cgdeclb(a))

/*
 * ============================================================================
 * Increment/Decrement Static Variables
 * ============================================================================
 */
#define cgincps(a,v)    (CG->vtable->cgincps(a,v))
#define cgdecps(a,v)    (CG->vtable->cgdecps(a,v))
#define cgincsw(a)      (CG->vtable->cgincsw(a))
#define cgdecsw(a)      (CG->vtable->cgdecsw(a))
#define cgincsb(a)      (CG->vtable->cgincsb(a))
#define cgdecsb(a)      (CG->vtable->cgdecsb(a))

/*
 * ============================================================================
 * Increment/Decrement Global Variables
 * ============================================================================
 */
#define cgincpg(s,v)    (CG->vtable->cgincpg(s,v))
#define cgdecpg(s,v)    (CG->vtable->cgdecpg(s,v))
#define cgincgw(s)      (CG->vtable->cgincgw(s))
#define cgdecgw(s)      (CG->vtable->cgdecgw(s))
#define cgincgb(s)      (CG->vtable->cgincgb(s))
#define cgdecgb(s)      (CG->vtable->cgdecgb(s))

/*
 * ============================================================================
 * Increment/Decrement via Indirect
 * ============================================================================
 */
#define cginc1iw()      (CG->vtable->cginc1iw())
#define cgdec1iw()      (CG->vtable->cgdec1iw())
#define cginc2iw()      (CG->vtable->cginc2iw())
#define cgdec2iw()      (CG->vtable->cgdec2iw())
#define cginc1ib()      (CG->vtable->cginc1ib())
#define cgdec1ib()      (CG->vtable->cgdec1ib())
#define cginc2ib()      (CG->vtable->cginc2ib())
#define cgdec2ib()      (CG->vtable->cgdec2ib())

/*
 * ============================================================================
 * Unconditional Branch
 * ============================================================================
 */
#define cgbrtrue(n)     (CG->vtable->cgbrtrue(n))
#define cgbrfalse(n)    (CG->vtable->cgbrfalse(n))
#define cgjump(n)       (CG->vtable->cgjump(n))

/*
 * ============================================================================
 * Switch Statement Support
 * ============================================================================
 */
#define cgldswtch(n)    (CG->vtable->cgldswtch(n))
#define cgcalswtch()    (CG->vtable->cgcalswtch())
#define cgcase(v,l)     (CG->vtable->cgcase(v,l))

/*
 * ============================================================================
 * Store Operations
 * ============================================================================
 */
#define cgstorib()      (CG->vtable->cgstorib())
#define cgstoriw()      (CG->vtable->cgstoriw())
#define cgstorlb(n)     (CG->vtable->cgstorlb(n))
#define cgstorlw(n)     (CG->vtable->cgstorlw(n))
#define cgstorsb(n)     (CG->vtable->cgstorsb(n))
#define cgstorsw(n)     (CG->vtable->cgstorsw(n))
#define cgstorgb(s)     (CG->vtable->cgstorgb(s))
#define cgstorgw(s)     (CG->vtable->cgstorgw(s))

/*
 * ============================================================================
 * Function Call Operations
 * ============================================================================
 */
#define cginitlw(v,a)   (CG->vtable->cginitlw(v,a))
#define cgcall(s)       (CG->vtable->cgcall(s))
#define cgcalr()        (CG->vtable->cgcalr())
#define cgstack(n)      (CG->vtable->cgstack(n))
#define cgentry()       (CG->vtable->cgentry())
#define cgexit()        (CG->vtable->cgexit())

/*
 * ============================================================================
 * ABI-Compliant Calling Convention Support
 * ============================================================================
 */
#define cgpusharg(n)    (CG->vtable->cgpusharg(n))
#define cgcallprep(n)   (CG->vtable->cgcallprep(n))
#define cgcallend(n)    (CG->vtable->cgcallend(n))
#define cgfnentry(n)    (CG->vtable->cgfnentry(n))
#define CG_NUM_ARG_REGS (CG->arch->num_arg_regs)

/*
 * ============================================================================
 * Data Definition
 * ============================================================================
 */
#define cgdefb(v)       (CG->vtable->cgdefb(v))
#define cgdefh(v)       (CG->vtable->cgdefh(v))    /* half-word (2 bytes) */
#define cgdefw(v)       (CG->vtable->cgdefw(v))    /* word (native int size) */
#define cgdefd(v)       (CG->vtable->cgdefd(v))    /* double-word (4 bytes) */
#define cgdefp(v)       (CG->vtable->cgdefp(v))
#define cgdefl(v)       (CG->vtable->cgdefl(v))
#define cgdefc(c)       (CG->vtable->cgdefc(c))
#define cgdefq(v)       (CG->vtable->cgdefq(v))    /* quad-word (8 bytes) */
#define cggbss(s,z)     (CG->vtable->cggbss(s,z))
#define cglbss(s,z)     (CG->vtable->cglbss(s,z))

/*
 * ============================================================================
 * Synthesizer Support
 * ============================================================================
 */
#define cgload2()       (CG->vtable->cgload2())
#define cgsynth(op)     (CG->vtable->cgsynth(op))

/*
 * ============================================================================
 * Floating-Point Operations
 * ============================================================================
 */

/* Load/Store floating-point values */
#define cgfloads(n)     (CG->vtable->cgfloads ? CG->vtable->cgfloads(n) : (void)0)
#define cgfloadd(n)     (CG->vtable->cgfloadd ? CG->vtable->cgfloadd(n) : (void)0)
#define cgfloadgs(s)    (CG->vtable->cgfloadgs ? CG->vtable->cgfloadgs(s) : (void)0)
#define cgfloadgd(s)    (CG->vtable->cgfloadgd ? CG->vtable->cgfloadgd(s) : (void)0)
#define cgfstores(n)    (CG->vtable->cgfstores ? CG->vtable->cgfstores(n) : (void)0)
#define cgfstored(n)    (CG->vtable->cgfstored ? CG->vtable->cgfstored(n) : (void)0)
#define cgfstoregs(s)   (CG->vtable->cgfstoregs ? CG->vtable->cgfstoregs(s) : (void)0)
#define cgfstoregsd(s)  (CG->vtable->cgfstoregsd ? CG->vtable->cgfstoregsd(s) : (void)0)

/* Floating-point literals */
#define cgflits(lab)    (CG->vtable->cgflits ? CG->vtable->cgflits(lab) : (void)0)
#define cgflitd(lab)    (CG->vtable->cgflitd ? CG->vtable->cgflitd(lab) : (void)0)

/* Floating-point arithmetic */
#define cgfadds()       (CG->vtable->cgfadds ? CG->vtable->cgfadds() : (void)0)
#define cgfaddd()       (CG->vtable->cgfaddd ? CG->vtable->cgfaddd() : (void)0)
#define cgfsubs()       (CG->vtable->cgfsubs ? CG->vtable->cgfsubs() : (void)0)
#define cgfsubd()       (CG->vtable->cgfsubd ? CG->vtable->cgfsubd() : (void)0)
#define cgfmuls()       (CG->vtable->cgfmuls ? CG->vtable->cgfmuls() : (void)0)
#define cgfmuld()       (CG->vtable->cgfmuld ? CG->vtable->cgfmuld() : (void)0)
#define cgfdivs()       (CG->vtable->cgfdivs ? CG->vtable->cgfdivs() : (void)0)
#define cgfdivd()       (CG->vtable->cgfdivd ? CG->vtable->cgfdivd() : (void)0)
#define cgfnegs()       (CG->vtable->cgfnegs ? CG->vtable->cgfnegs() : (void)0)
#define cgfnegd()       (CG->vtable->cgfnegd ? CG->vtable->cgfnegd() : (void)0)

/* Floating-point comparisons */
#define cgfcmps()       (CG->vtable->cgfcmps ? CG->vtable->cgfcmps() : (void)0)
#define cgfcmpd()       (CG->vtable->cgfcmpd ? CG->vtable->cgfcmpd() : (void)0)
#define cgfeqs()        (CG->vtable->cgfeqs ? CG->vtable->cgfeqs() : (void)0)
#define cgfeqd()        (CG->vtable->cgfeqd ? CG->vtable->cgfeqd() : (void)0)
#define cgfnes()        (CG->vtable->cgfnes ? CG->vtable->cgfnes() : (void)0)
#define cgfned()        (CG->vtable->cgfned ? CG->vtable->cgfned() : (void)0)
#define cgflts()        (CG->vtable->cgflts ? CG->vtable->cgflts() : (void)0)
#define cgfltd()        (CG->vtable->cgfltd ? CG->vtable->cgfltd() : (void)0)
#define cgfgts()        (CG->vtable->cgfgts ? CG->vtable->cgfgts() : (void)0)
#define cgfgtd()        (CG->vtable->cgfgtd ? CG->vtable->cgfgtd() : (void)0)
#define cgfles()        (CG->vtable->cgfles ? CG->vtable->cgfles() : (void)0)
#define cgfled()        (CG->vtable->cgfled ? CG->vtable->cgfled() : (void)0)
#define cgfges()        (CG->vtable->cgfges ? CG->vtable->cgfges() : (void)0)
#define cgfged()        (CG->vtable->cgfged ? CG->vtable->cgfged() : (void)0)

/* Type conversions */
#define cgitofs()       (CG->vtable->cgitofs ? CG->vtable->cgitofs() : (void)0)
#define cgitofd()       (CG->vtable->cgitofd ? CG->vtable->cgitofd() : (void)0)
#define cgftois()       (CG->vtable->cgftois ? CG->vtable->cgftois() : (void)0)
#define cgftoid()       (CG->vtable->cgftoid ? CG->vtable->cgftoid() : (void)0)
#define cgstod()        (CG->vtable->cgstod ? CG->vtable->cgstod() : (void)0)
#define cgdtos()        (CG->vtable->cgdtos ? CG->vtable->cgdtos() : (void)0)

/* Floating-point stack operations */
#define cgfpush()       (CG->vtable->cgfpush ? CG->vtable->cgfpush() : (void)0)
#define cgfpop()        (CG->vtable->cgfpop ? CG->vtable->cgfpop() : (void)0)
#define cgfxch()        (CG->vtable->cgfxch ? CG->vtable->cgfxch() : (void)0)

/* Define floating-point constants */
#define cgdeffloat(v)     (CG->vtable->cgdeffloat ? CG->vtable->cgdeffloat(v) : (void)0)
#define cgdefdouble(v)    (CG->vtable->cgdefdouble ? CG->vtable->cgdefdouble(v) : (void)0)

/*
 * ============================================================================
 * Architecture Property Access
 * ============================================================================
 */
#define BPW             (CG->arch->bpw)
#define INTSIZE         (CG->arch->int_size)
#define PTRSIZE         (CG->arch->ptr_size)
#define CPU             (CG->arch->name)

/*
 * ============================================================================
 * OS Configuration Access
 * ============================================================================
 */
#define OS              (CG->os->os_name)
#define AOUTNAME        (CG->os->aout_name)
#define LDCMD           (CG->os->ld_cmd)
#define ASCMD           (CG->os->asm_cmd)
#define SYSLIBC         (CG->os->sys_libc)

#endif /* CGEN_COMPAT_H */
