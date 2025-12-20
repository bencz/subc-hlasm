/*
 * SubC Compiler - x86-64 Code Generator Function Declarations
 *
 * This header exposes the x86-64 code generation functions so they can be
 * referenced by the vtable in cg_x86_64_arch.c.
 *
 * Copyright (c) 2011-2025 - Public Domain (CC0)
 */

#ifndef CG_X86_64_FUNCS_H
#define CG_X86_64_FUNCS_H

/* Section control */
void cgx64_data(void);
void cgx64_text(void);
void cgx64_prelude(void);
void cgx64_postlude(void);
void cgx64_public(char *s);
void cgx64_align(void);

/* Literal and clear */
void cgx64_lit(int v);
void cgx64_clear(void);
void cgx64_clear2(void);

/* Load operations */
void cgx64_ldgb(char *s);
void cgx64_ldgw(char *s);
void cgx64_ldlb(int n);
void cgx64_ldlw(int n);
void cgx64_ldsb(int n);
void cgx64_ldsw(int n);
void cgx64_ldla(int n);
void cgx64_ldsa(int n);
void cgx64_ldga(char *s);
void cgx64_indb(void);
void cgx64_indw(void);
void cgx64_ldlab(int id);

/* Stack operations */
void cgx64_push(void);
void cgx64_pushlit(int n);
void cgx64_pop2(void);
void cgx64_swap(void);
void cgx64_popptr(void);

/* Arithmetic operations */
void cgx64_add(void);
void cgx64_sub(void);
void cgx64_mul(void);
void cgx64_div(void);
void cgx64_mod(void);
void cgx64_neg(void);

/* Bitwise operations */
void cgx64_and(void);
void cgx64_ior(void);
void cgx64_xor(void);
void cgx64_not(void);
void cgx64_shl(void);
void cgx64_shr(void);

/* Comparison operations */
void cgx64_eq(void);
void cgx64_ne(void);
void cgx64_lt(void);
void cgx64_gt(void);
void cgx64_le(void);
void cgx64_ge(void);
void cgx64_ult(void);
void cgx64_ugt(void);
void cgx64_ule(void);
void cgx64_uge(void);

/* Conditional branch operations */
void cgx64_breq(int n);
void cgx64_brne(int n);
void cgx64_brlt(int n);
void cgx64_brgt(int n);
void cgx64_brle(int n);
void cgx64_brge(int n);
void cgx64_brult(int n);
void cgx64_brugt(int n);
void cgx64_brule(int n);
void cgx64_bruge(int n);

/* Boolean and logic operations */
void cgx64_lognot(void);
void cgx64_bool(void);

/* Pointer scaling operations */
void cgx64_scale(void);
void cgx64_scale2(void);
void cgx64_unscale(void);
void cgx64_scaleby(int v);
void cgx64_scale2by(int v);
void cgx64_unscaleby(int v);

/* Increment/decrement via pointer */
void cgx64_ldinc(void);
void cgx64_inc1pi(int v);
void cgx64_dec1pi(int v);
void cgx64_inc2pi(int v);
void cgx64_dec2pi(int v);

/* Increment/decrement local variables */
void cgx64_incpl(int a, int v);
void cgx64_decpl(int a, int v);
void cgx64_inclw(int a);
void cgx64_declw(int a);
void cgx64_inclb(int a);
void cgx64_declb(int a);

/* Increment/decrement static variables */
void cgx64_incps(int a, int v);
void cgx64_decps(int a, int v);
void cgx64_incsw(int a);
void cgx64_decsw(int a);
void cgx64_incsb(int a);
void cgx64_decsb(int a);

/* Increment/decrement global variables */
void cgx64_incpg(char *s, int v);
void cgx64_decpg(char *s, int v);
void cgx64_incgw(char *s);
void cgx64_decgw(char *s);
void cgx64_incgb(char *s);
void cgx64_decgb(char *s);

/* Increment/decrement via indirect */
void cgx64_inc1iw(void);
void cgx64_dec1iw(void);
void cgx64_inc2iw(void);
void cgx64_dec2iw(void);
void cgx64_inc1ib(void);
void cgx64_dec1ib(void);
void cgx64_inc2ib(void);
void cgx64_dec2ib(void);

/* Unconditional branch */
void cgx64_brtrue(int n);
void cgx64_brfalse(int n);
void cgx64_jump(int n);

/* Switch statement support */
void cgx64_ldswtch(int n);
void cgx64_calswtch(void);
void cgx64_case(int v, int l);

/* Store operations */
void cgx64_storib(void);
void cgx64_storiw(void);
void cgx64_storlb(int n);
void cgx64_storlw(int n);
void cgx64_storsb(int n);
void cgx64_storsw(int n);
void cgx64_storgb(char *s);
void cgx64_storgw(char *s);

/* Function call operations */
void cgx64_initlw(int v, int a);
void cgx64_call(char *s);
void cgx64_calr(void);
void cgx64_stack(int n);
void cgx64_entry(void);
void cgx64_exit(void);

/* Data definition */
void cgx64_defb(int v);
void cgx64_defw(int v);
void cgx64_defp(int v);
void cgx64_defl(int v);
void cgx64_defc(int c);
void cgx64_gbss(char *s, int z);
void cgx64_lbss(char *s, int z);

/* Synthesizer support */
int cgx64_load2(void);

#endif /* CG_X86_64_FUNCS_H */
