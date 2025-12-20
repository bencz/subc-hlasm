/*
 * SubC Compiler - Intel 386 Code Generator Interface
 *
 * This header exposes the i386 code generation functions so they can be
 * used by multiple target definitions (386-linux, 386-freebsd, etc.).
 *
 * The actual implementations are in cg_386.c.
 * Each OS-specific target file includes this header and defines its
 * own struct cg_target with the appropriate OS configuration.
 *
 * Copyright (c) 2011-2025 - Public Domain (CC0)
 */

#ifndef CG_386_H
#define CG_386_H

#include "../../cgtarget.h"

/*
 * ============================================================================
 * Code Generation Functions
 * ============================================================================
 *
 * These functions implement the i386 code generator. They are shared by
 * all i386 targets regardless of operating system.
 */

/* Section control */
void cg386_data(void);
void cg386_text(void);
void cg386_prelude(void);
void cg386_postlude(void);
void cg386_public(char *s);
void cg386_align(void);

/* Literal and clear */
void cg386_lit(int v);
void cg386_clear(void);
void cg386_clear2(void);

/* Load operations */
void cg386_ldgb(char *s);
void cg386_ldgw(char *s);
void cg386_ldlb(int n);
void cg386_ldlw(int n);
void cg386_ldsb(int n);
void cg386_ldsw(int n);
void cg386_ldla(int n);
void cg386_ldsa(int n);
void cg386_ldga(char *s);
void cg386_indb(void);
void cg386_indw(void);
void cg386_ldlab(int id);

/* Stack operations */
void cg386_push(void);
void cg386_pushlit(int n);
void cg386_pop2(void);
void cg386_swap(void);
void cg386_popptr(void);

/* Arithmetic operations */
void cg386_add(void);
void cg386_sub(void);
void cg386_mul(void);
void cg386_div(void);
void cg386_mod(void);
void cg386_neg(void);

/* Bitwise operations */
void cg386_and(void);
void cg386_ior(void);
void cg386_xor(void);
void cg386_not(void);
void cg386_shl(void);
void cg386_shr(void);

/* Comparison operations */
void cg386_eq(void);
void cg386_ne(void);
void cg386_lt(void);
void cg386_gt(void);
void cg386_le(void);
void cg386_ge(void);
void cg386_ult(void);
void cg386_ugt(void);
void cg386_ule(void);
void cg386_uge(void);

/* Conditional branch operations */
void cg386_breq(int n);
void cg386_brne(int n);
void cg386_brlt(int n);
void cg386_brgt(int n);
void cg386_brle(int n);
void cg386_brge(int n);
void cg386_brult(int n);
void cg386_brugt(int n);
void cg386_brule(int n);
void cg386_bruge(int n);

/* Boolean and logic operations */
void cg386_lognot(void);
void cg386_bool(void);

/* Pointer scaling operations */
void cg386_scale(void);
void cg386_scale2(void);
void cg386_unscale(void);
void cg386_scaleby(int v);
void cg386_scale2by(int v);
void cg386_unscaleby(int v);

/* Increment/decrement via pointer */
void cg386_ldinc(void);
void cg386_inc1pi(int v);
void cg386_dec1pi(int v);
void cg386_inc2pi(int v);
void cg386_dec2pi(int v);

/* Increment/decrement local variables */
void cg386_incpl(int a, int v);
void cg386_decpl(int a, int v);
void cg386_inclw(int a);
void cg386_declw(int a);
void cg386_inclb(int a);
void cg386_declb(int a);

/* Increment/decrement static variables */
void cg386_incps(int a, int v);
void cg386_decps(int a, int v);
void cg386_incsw(int a);
void cg386_decsw(int a);
void cg386_incsb(int a);
void cg386_decsb(int a);

/* Increment/decrement global variables */
void cg386_incpg(char *s, int v);
void cg386_decpg(char *s, int v);
void cg386_incgw(char *s);
void cg386_decgw(char *s);
void cg386_incgb(char *s);
void cg386_decgb(char *s);

/* Increment/decrement via indirect */
void cg386_inc1iw(void);
void cg386_dec1iw(void);
void cg386_inc2iw(void);
void cg386_dec2iw(void);
void cg386_inc1ib(void);
void cg386_dec1ib(void);
void cg386_inc2ib(void);
void cg386_dec2ib(void);

/* Unconditional branch */
void cg386_brtrue(int n);
void cg386_brfalse(int n);
void cg386_jump(int n);

/* Switch statement support */
void cg386_ldswtch(int n);
void cg386_calswtch(void);
void cg386_case(int v, int l);

/* Store operations */
void cg386_storib(void);
void cg386_storiw(void);
void cg386_storlb(int n);
void cg386_storlw(int n);
void cg386_storsb(int n);
void cg386_storsw(int n);
void cg386_storgb(char *s);
void cg386_storgw(char *s);

/* Function call operations */
void cg386_initlw(int v, int a);
void cg386_call(char *s);
void cg386_calr(void);
void cg386_stack(int n);
void cg386_entry(void);
void cg386_exit(void);

/* Data definition */
void cg386_defb(int v);
void cg386_defw(int v);
void cg386_defp(int v);
void cg386_defl(int v);
void cg386_defc(int c);
void cg386_gbss(char *s, int z);
void cg386_lbss(char *s, int z);

/* Synthesizer support */
int cg386_load2(void);

#endif /* CG_386_H */
