/*
 * SubC Compiler - ARMv6 Code Generator Function Declarations
 *
 * Copyright (c) 2011-2025 - Public Domain (CC0)
 */

#ifndef CG_ARMV6_FUNCS_H
#define CG_ARMV6_FUNCS_H

/* Section control */
void cgarm_data(void);
void cgarm_text(void);
void cgarm_prelude(void);
void cgarm_postlude(void);
void cgarm_public(char *s);
void cgarm_align(void);

/* Literal and clear */
void cgarm_lit(int v);
void cgarm_clear(void);
void cgarm_clear2(void);

/* Load operations */
void cgarm_ldgb(char *s);
void cgarm_ldgw(char *s);
void cgarm_ldlb(int n);
void cgarm_ldlw(int n);
void cgarm_ldsb(int n);
void cgarm_ldsw(int n);
void cgarm_ldla(int n);
void cgarm_ldsa(int n);
void cgarm_ldga(char *s);
void cgarm_indb(void);
void cgarm_indw(void);
void cgarm_ldlab(int id);

/* Stack operations */
void cgarm_push(void);
void cgarm_pushlit(int n);
void cgarm_pop2(void);
void cgarm_swap(void);
void cgarm_popptr(void);

/* Arithmetic operations */
void cgarm_add(void);
void cgarm_sub(void);
void cgarm_mul(void);
void cgarm_div(void);
void cgarm_mod(void);
void cgarm_neg(void);

/* Bitwise operations */
void cgarm_and(void);
void cgarm_ior(void);
void cgarm_xor(void);
void cgarm_not(void);
void cgarm_shl(void);
void cgarm_shr(void);

/* Comparison operations */
void cgarm_eq(void);
void cgarm_ne(void);
void cgarm_lt(void);
void cgarm_gt(void);
void cgarm_le(void);
void cgarm_ge(void);
void cgarm_ult(void);
void cgarm_ugt(void);
void cgarm_ule(void);
void cgarm_uge(void);

/* Conditional branch operations */
void cgarm_breq(int n);
void cgarm_brne(int n);
void cgarm_brlt(int n);
void cgarm_brgt(int n);
void cgarm_brle(int n);
void cgarm_brge(int n);
void cgarm_brult(int n);
void cgarm_brugt(int n);
void cgarm_brule(int n);
void cgarm_bruge(int n);

/* Boolean and logic operations */
void cgarm_lognot(void);
void cgarm_bool(void);

/* Pointer scaling operations */
void cgarm_scale(void);
void cgarm_scale2(void);
void cgarm_unscale(void);
void cgarm_scaleby(int v);
void cgarm_scale2by(int v);
void cgarm_unscaleby(int v);

/* Increment/decrement via pointer */
void cgarm_ldinc(void);
void cgarm_inc1pi(int v);
void cgarm_dec1pi(int v);
void cgarm_inc2pi(int v);
void cgarm_dec2pi(int v);

/* Increment/decrement local variables */
void cgarm_incpl(int a, int v);
void cgarm_decpl(int a, int v);
void cgarm_inclw(int a);
void cgarm_declw(int a);
void cgarm_inclb(int a);
void cgarm_declb(int a);

/* Increment/decrement static variables */
void cgarm_incps(int a, int v);
void cgarm_decps(int a, int v);
void cgarm_incsw(int a);
void cgarm_decsw(int a);
void cgarm_incsb(int a);
void cgarm_decsb(int a);

/* Increment/decrement global variables */
void cgarm_incpg(char *s, int v);
void cgarm_decpg(char *s, int v);
void cgarm_incgw(char *s);
void cgarm_decgw(char *s);
void cgarm_incgb(char *s);
void cgarm_decgb(char *s);

/* Increment/decrement via indirect */
void cgarm_inc1iw(void);
void cgarm_dec1iw(void);
void cgarm_inc2iw(void);
void cgarm_dec2iw(void);
void cgarm_inc1ib(void);
void cgarm_dec1ib(void);
void cgarm_inc2ib(void);
void cgarm_dec2ib(void);

/* Unconditional branch */
void cgarm_brtrue(int n);
void cgarm_brfalse(int n);
void cgarm_jump(int n);

/* Switch statement support */
void cgarm_ldswtch(int n);
void cgarm_calswtch(void);
void cgarm_case(int v, int l);

/* Store operations */
void cgarm_storib(void);
void cgarm_storiw(void);
void cgarm_storlb(int n);
void cgarm_storlw(int n);
void cgarm_storsb(int n);
void cgarm_storsw(int n);
void cgarm_storgb(char *s);
void cgarm_storgw(char *s);

/* Function call operations */
void cgarm_initlw(int v, int a);
void cgarm_call(char *s);
void cgarm_calr(void);
void cgarm_stack(int n);
void cgarm_entry(void);
void cgarm_exit(void);

/* Data definition */
void cgarm_defb(int v);
void cgarm_defw(int v);
void cgarm_defp(int v);
void cgarm_defl(int v);
void cgarm_defc(int c);
void cgarm_gbss(char *s, int z);
void cgarm_lbss(char *s, int z);

/* Synthesizer support */
int cgarm_load2(void);

#endif /* CG_ARMV6_FUNCS_H */
