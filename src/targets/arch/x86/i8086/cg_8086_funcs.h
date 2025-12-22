/*
 * SubC Compiler - Intel 8086 Code Generator Function Declarations
 *
 * Copyright (c) 2011-2025 - Public Domain (CC0)
 */

#ifndef CG_8086_FUNCS_H
#define CG_8086_FUNCS_H

/* Section control */
void cg86_data(void);
void cg86_text(void);
void cg86_prelude(void);
void cg86_postlude(void);
void cg86_public(char *s);
void cg86_name(char *s);
void cg86_funcname(char *s);
void cg86_align(void);

/* Literal and clear */
void cg86_lit(int v);
void cg86_clear(void);
void cg86_clear2(void);

/* Load operations */
void cg86_ldgb(char *s);
void cg86_ldgw(char *s);
void cg86_ldlb(int n);
void cg86_ldlw(int n);
void cg86_ldsb(int n);
void cg86_ldsw(int n);
void cg86_ldla(int n);
void cg86_ldsa(int n);
void cg86_ldga(char *s);
void cg86_indb(void);
void cg86_indw(void);
void cg86_ldlab(int id);

/* Stack operations */
void cg86_push(void);
void cg86_pushlit(int n);
void cg86_pop2(void);
void cg86_swap(void);
void cg86_popptr(void);

/* Arithmetic operations */
void cg86_add(void);
void cg86_sub(void);
void cg86_mul(void);
void cg86_div(void);
void cg86_mod(void);
void cg86_neg(void);

/* Bitwise operations */
void cg86_and(void);
void cg86_ior(void);
void cg86_xor(void);
void cg86_not(void);
void cg86_shl(void);
void cg86_shr(void);

/* Comparison operations */
void cg86_eq(void);
void cg86_ne(void);
void cg86_lt(void);
void cg86_gt(void);
void cg86_le(void);
void cg86_ge(void);
void cg86_ult(void);
void cg86_ugt(void);
void cg86_ule(void);
void cg86_uge(void);

/* Conditional branch operations */
void cg86_breq(int n);
void cg86_brne(int n);
void cg86_brlt(int n);
void cg86_brgt(int n);
void cg86_brle(int n);
void cg86_brge(int n);
void cg86_brult(int n);
void cg86_brugt(int n);
void cg86_brule(int n);
void cg86_bruge(int n);

/* Boolean and logic operations */
void cg86_lognot(void);
void cg86_bool(void);

/* Pointer scaling operations */
void cg86_scale(void);
void cg86_scale2(void);
void cg86_unscale(void);
void cg86_scaleby(int v);
void cg86_scale2by(int v);
void cg86_unscaleby(int v);

/* Increment/decrement via pointer */
void cg86_ldinc(void);
void cg86_inc1pi(int v);
void cg86_dec1pi(int v);
void cg86_inc2pi(int v);
void cg86_dec2pi(int v);

/* Increment/decrement local variables */
void cg86_incpl(int a, int v);
void cg86_decpl(int a, int v);
void cg86_inclw(int a);
void cg86_declw(int a);
void cg86_inclb(int a);
void cg86_declb(int a);

/* Increment/decrement static variables */
void cg86_incps(int a, int v);
void cg86_decps(int a, int v);
void cg86_incsw(int a);
void cg86_decsw(int a);
void cg86_incsb(int a);
void cg86_decsb(int a);

/* Increment/decrement global variables */
void cg86_incpg(char *s, int v);
void cg86_decpg(char *s, int v);
void cg86_incgw(char *s);
void cg86_decgw(char *s);
void cg86_incgb(char *s);
void cg86_decgb(char *s);

/* Increment/decrement via indirect */
void cg86_inc1iw(void);
void cg86_dec1iw(void);
void cg86_inc2iw(void);
void cg86_dec2iw(void);
void cg86_inc1ib(void);
void cg86_dec1ib(void);
void cg86_inc2ib(void);
void cg86_dec2ib(void);

/* Unconditional branch */
void cg86_brtrue(int n);
void cg86_brfalse(int n);
void cg86_jump(int n);

/* Switch statement support */
void cg86_ldswtch(int n);
void cg86_calswtch(void);
void cg86_case(int v, int l);

/* Store operations */
void cg86_storib(void);
void cg86_storiw(void);
void cg86_storlb(int n);
void cg86_storlw(int n);
void cg86_storsb(int n);
void cg86_storsw(int n);
void cg86_storgb(char *s);
void cg86_storgw(char *s);

/* Function call operations */
void cg86_initlw(int v, int a);
void cg86_call(char *s);
void cg86_calr(void);
void cg86_stack(int n);
void cg86_entry(int lsize, int nparams);
void cg86_exit(void);

/* Data definition */
void cg86_defb(int v);
void cg86_defw(int v);
void cg86_defp(int v);
void cg86_defl(int v);
void cg86_defc(int c);
void cg86_gbss(char *s, int z);
void cg86_lbss(char *s, int z);

/* Synthesizer support */
int cg86_load2(void);

#endif /* CG_8086_FUNCS_H */
