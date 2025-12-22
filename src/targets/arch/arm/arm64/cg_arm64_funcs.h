/*
 * SubC Compiler - ARM64 (AArch64) Code Generator Function Declarations
 *
 * This file declares all code generation functions for the ARM64 target.
 * These functions generate GNU Assembler (GAS) syntax for AArch64.
 *
 * Register usage (AAPCS64 calling convention):
 *   x0-x7   - Arguments and return value (x0 = primary accumulator)
 *   x8      - Indirect result location register
 *   x9-x15  - Temporary registers (caller-saved)
 *   x16-x17 - Intra-procedure-call scratch registers
 *   x18     - Platform register (reserved)
 *   x19-x28 - Callee-saved registers
 *   x29     - Frame pointer (FP)
 *   x30     - Link register (LR)
 *   sp      - Stack pointer
 *
 * For SubC we use:
 *   x0  - Primary accumulator, return value
 *   x1  - Secondary accumulator, second operand
 *   x2  - Tertiary register, pointer for stores
 *   x3  - Scratch register
 *   x29 - Frame pointer
 *   x30 - Link register
 *   sp  - Stack pointer
 *
 * Copyright (c) 2025 - Public Domain (CC0)
 */

#ifndef CG_ARM64_FUNCS_H
#define CG_ARM64_FUNCS_H

/* Section control */
void cga64_data(void);
void cga64_text(void);
void cga64_prelude(void);
void cga64_postlude(void);
void cga64_public(char *s);
void cga64_name(char *s);
void cga64_funcname(char *s);
void cga64_align(void);

/* Literal and clear */
void cga64_lit(int v);
void cga64_clear(void);
void cga64_clear2(void);

/* Load operations */
void cga64_ldgb(char *s);
void cga64_ldgw(char *s);
void cga64_ldlb(int n);
void cga64_ldlw(int n);
void cga64_ldsb(int n);
void cga64_ldsw(int n);
void cga64_ldla(int n);
void cga64_ldsa(int n);
void cga64_ldga(char *s);
void cga64_indb(void);
void cga64_indw(void);
void cga64_ldlab(int id);

/* Stack operations */
void cga64_push(void);
void cga64_pushlit(int n);
void cga64_pop2(void);
void cga64_swap(void);
void cga64_popptr(void);

/* Arithmetic operations */
void cga64_add(void);
void cga64_sub(void);
void cga64_mul(void);
void cga64_div(void);
void cga64_mod(void);
void cga64_neg(void);

/* Bitwise operations */
void cga64_and(void);
void cga64_ior(void);
void cga64_xor(void);
void cga64_not(void);
void cga64_shl(void);
void cga64_shr(void);

/* Comparison operations */
void cga64_eq(void);
void cga64_ne(void);
void cga64_lt(void);
void cga64_gt(void);
void cga64_le(void);
void cga64_ge(void);
void cga64_ult(void);
void cga64_ugt(void);
void cga64_ule(void);
void cga64_uge(void);

/* Conditional branch operations */
void cga64_breq(int n);
void cga64_brne(int n);
void cga64_brlt(int n);
void cga64_brgt(int n);
void cga64_brle(int n);
void cga64_brge(int n);
void cga64_brult(int n);
void cga64_brugt(int n);
void cga64_brule(int n);
void cga64_bruge(int n);

/* Boolean and logic operations */
void cga64_lognot(void);
void cga64_bool(void);

/* Pointer scaling operations */
void cga64_scale(void);
void cga64_scale2(void);
void cga64_unscale(void);
void cga64_scaleby(int v);
void cga64_scale2by(int v);
void cga64_unscaleby(int v);

/* Increment/decrement via pointer */
void cga64_ldinc(void);
void cga64_inc1pi(int v);
void cga64_dec1pi(int v);
void cga64_inc2pi(int v);
void cga64_dec2pi(int v);

/* Increment/decrement local variables */
void cga64_incpl(int a, int v);
void cga64_decpl(int a, int v);
void cga64_inclw(int a);
void cga64_declw(int a);
void cga64_inclb(int a);
void cga64_declb(int a);

/* Increment/decrement static variables */
void cga64_incps(int a, int v);
void cga64_decps(int a, int v);
void cga64_incsw(int a);
void cga64_decsw(int a);
void cga64_incsb(int a);
void cga64_decsb(int a);

/* Increment/decrement global variables */
void cga64_incpg(char *s, int v);
void cga64_decpg(char *s, int v);
void cga64_incgw(char *s);
void cga64_decgw(char *s);
void cga64_incgb(char *s);
void cga64_decgb(char *s);

/* Increment/decrement via indirect */
void cga64_inc1iw(void);
void cga64_dec1iw(void);
void cga64_inc2iw(void);
void cga64_dec2iw(void);
void cga64_inc1ib(void);
void cga64_dec1ib(void);
void cga64_inc2ib(void);
void cga64_dec2ib(void);

/* Unconditional branch */
void cga64_brtrue(int n);
void cga64_brfalse(int n);
void cga64_jump(int n);

/* Switch statement support */
void cga64_ldswtch(int n);
void cga64_calswtch(void);
void cga64_case(int v, int l);

/* Store operations */
void cga64_storib(void);
void cga64_storiw(void);
void cga64_storlb(int n);
void cga64_storlw(int n);
void cga64_storsb(int n);
void cga64_storsw(int n);
void cga64_storgb(char *s);
void cga64_storgw(char *s);

/* Function call operations */
void cga64_initlw(int v, int a);
void cga64_call(char *s);
void cga64_calr(void);
void cga64_stack(int n);
void cga64_entry(int lsize);
void cga64_exit(void);
void cga64_movearg(int n);

/* Data definition */
void cga64_defb(int v);
void cga64_defw(int v);
void cga64_defp(int v);
void cga64_defl(int v);
void cga64_defc(int c);
void cga64_gbss(char *s, int z);
void cga64_lbss(char *s, int z);

/* Synthesizer support */
int cga64_load2(void);

/* Helper functions (internal) */
void cga64_lit2(int v, int aux);
void cga64_locladdr(int n, int aux);
void cga64_stataddr(int n, int aux);
void cga64_globaddr(char *s, int aux);

#endif /* CG_ARM64_FUNCS_H */
