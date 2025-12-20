/*
 * SubC Compiler - IBM System/370 Code Generator Function Declarations
 *
 * This file declares all code generation functions for the S/370 target.
 * These functions generate HLASM (High Level Assembler) syntax.
 *
 * Register usage:
 *   R0  - Work register (not preserved across calls)
 *   R1  - Parameter register / work register
 *   R2  - Primary accumulator (return value)
 *   R3  - Secondary accumulator
 *   R4-R11 - General purpose (callee-saved)
 *   R12 - Base register (program addressability)
 *   R13 - Save area pointer (like frame pointer)
 *   R14 - Return address
 *   R15 - Entry point / return code
 *
 * Copyright (c) 2025 - Public Domain (CC0)
 */

#ifndef CG_S370_FUNCS_H
#define CG_S370_FUNCS_H

/* Section Control */
void cgs370_data(void);
void cgs370_text(void);
void cgs370_prelude(void);
void cgs370_postlude(void);
void cgs370_public(char *s);
void cgs370_name(char *s);
void cgs370_funcname(char *s);
void cgs370_align(void);

/* Literal and Clear */
void cgs370_lit(int n);
void cgs370_clear(void);
void cgs370_clear2(void);

/* Load Operations */
void cgs370_ldgb(char *s);
void cgs370_ldgw(char *s);
void cgs370_ldlb(int n);
void cgs370_ldlw(int n);
void cgs370_ldsb(int n);
void cgs370_ldsw(int n);
void cgs370_ldla(int n);
void cgs370_ldsa(int n);
void cgs370_ldga(char *s);
void cgs370_indb(void);
void cgs370_indw(void);
void cgs370_ldlab(int id);

/* Stack Operations */
void cgs370_push(void);
void cgs370_pushlit(int n);
void cgs370_pop2(void);
void cgs370_swap(void);
void cgs370_popptr(void);

/* Arithmetic Operations */
void cgs370_add(void);
void cgs370_sub(void);
void cgs370_mul(void);
void cgs370_div(void);
void cgs370_mod(void);
void cgs370_neg(void);

/* Bitwise Operations */
void cgs370_and(void);
void cgs370_ior(void);
void cgs370_xor(void);
void cgs370_not(void);
void cgs370_shl(void);
void cgs370_shr(void);

/* Comparison Operations */
void cgs370_eq(void);
void cgs370_ne(void);
void cgs370_lt(void);
void cgs370_gt(void);
void cgs370_le(void);
void cgs370_ge(void);
void cgs370_ult(void);
void cgs370_ugt(void);
void cgs370_ule(void);
void cgs370_uge(void);

/* Conditional Branch Operations */
void cgs370_breq(int n);
void cgs370_brne(int n);
void cgs370_brlt(int n);
void cgs370_brgt(int n);
void cgs370_brle(int n);
void cgs370_brge(int n);
void cgs370_brult(int n);
void cgs370_brugt(int n);
void cgs370_brule(int n);
void cgs370_bruge(int n);

/* Boolean and Logic Operations */
void cgs370_lognot(void);
void cgs370_bool(void);

/* Pointer Scaling Operations */
void cgs370_scale(void);
void cgs370_scale2(void);
void cgs370_unscale(void);
void cgs370_scaleby(int v);
void cgs370_scale2by(int v);
void cgs370_unscaleby(int v);

/* Increment/Decrement via Pointer */
void cgs370_ldinc(void);
void cgs370_inc1pi(int v);
void cgs370_dec1pi(int v);
void cgs370_inc2pi(int v);
void cgs370_dec2pi(int v);

/* Increment/Decrement Local Variables */
void cgs370_incpl(int a, int v);
void cgs370_decpl(int a, int v);
void cgs370_inclw(int a);
void cgs370_declw(int a);
void cgs370_inclb(int a);
void cgs370_declb(int a);

/* Increment/Decrement Static Variables */
void cgs370_incps(int a, int v);
void cgs370_decps(int a, int v);
void cgs370_incsw(int a);
void cgs370_decsw(int a);
void cgs370_incsb(int a);
void cgs370_decsb(int a);

/* Increment/Decrement Global Variables */
void cgs370_incpg(char *s, int v);
void cgs370_decpg(char *s, int v);
void cgs370_incgw(char *s);
void cgs370_decgw(char *s);
void cgs370_incgb(char *s);
void cgs370_decgb(char *s);

/* Increment/Decrement via Indirect */
void cgs370_inc1iw(void);
void cgs370_dec1iw(void);
void cgs370_inc2iw(void);
void cgs370_dec2iw(void);
void cgs370_inc1ib(void);
void cgs370_dec1ib(void);
void cgs370_inc2ib(void);
void cgs370_dec2ib(void);

/* Unconditional Branch */
void cgs370_brtrue(int n);
void cgs370_brfalse(int n);
void cgs370_jump(int n);

/* Switch Statement Support */
void cgs370_ldswtch(int n);
void cgs370_calswtch(void);
void cgs370_case(int v, int l);

/* Store Operations */
void cgs370_storib(void);
void cgs370_storiw(void);
void cgs370_storlb(int n);
void cgs370_storlw(int n);
void cgs370_storsb(int n);
void cgs370_storsw(int n);
void cgs370_storgb(char *s);
void cgs370_storgw(char *s);

/* Function Call Operations */
void cgs370_initlw(int v, int a);
void cgs370_call(char *s);
void cgs370_calr(void);
void cgs370_stack(int n);
void cgs370_entry(int lsize);
void cgs370_exit(void);

/* Data Definition */
void cgs370_defb(int v);
void cgs370_defw(int v);
void cgs370_defp(int v);
void cgs370_defl(int v);
void cgs370_defc(int c);
void cgs370_gbss(char *s, int z);
void cgs370_lbss(char *s, int z);

/* Synthesizer Support */
int cgs370_load2(void);

#endif /* CG_S370_FUNCS_H */
