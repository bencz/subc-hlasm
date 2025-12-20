/*
 * SubC Compiler - Intel 386 Architecture Definition
 *
 * This file defines the shared architecture properties and vtable
 * for all i386 targets. The code generation functions are in cg_386.c.
 *
 * Copyright (c) 2011-2025 - Public Domain (CC0)
 */

#include "../../cgtarget.h"
#include "cg_386.h"

/*
 * ============================================================================
 * Architecture Properties
 * ============================================================================
 *
 * Shared by all i386 targets regardless of operating system.
 */
struct cg_arch cg_386_arch = {
    /* Data type sizes */
    32,                             /* bits */
    1,                              /* char_size */
    2,                              /* short_size */
    4,                              /* int_size */
    4,                              /* long_size */
    4,                              /* ptr_size */
    4,                              /* bpw */
    
    /* Architecture characteristics */
    ENDIAN_LITTLE,
    STACK_DOWN,
    ASM_GAS,
    CC_CDECL,
    FLOAT_IEEE754,
    
    /* Alignment */
    4,                              /* align_stack */
    4,                              /* align_data */
    4,                              /* align_func */
    
    /* Hardware capabilities */
    1,                              /* has_mul */
    1,                              /* has_div */
    1,                              /* has_mod */
    1,                              /* has_barrel_shift */
    1,                              /* has_byte_ops */
    0,                              /* needs_alignment */
    0,                              /* needs_pic */
    1                               /* has_frame_ptr */
};

/*
 * ============================================================================
 * Code Generation Vtable
 * ============================================================================
 *
 * Shared by all i386 targets. Points to functions in cg_386.c.
 */
struct cg_vtable cg_386_vtable = {
    /* Section Control */
    cg386_data,
    cg386_text,
    cg386_prelude,
    cg386_postlude,
    cg386_public,
    cg386_align,
    
    /* Literal and Clear */
    cg386_lit,
    cg386_clear,
    cg386_clear2,
    
    /* Load Operations */
    cg386_ldgb,
    cg386_ldgw,
    cg386_ldlb,
    cg386_ldlw,
    cg386_ldsb,
    cg386_ldsw,
    cg386_ldla,
    cg386_ldsa,
    cg386_ldga,
    cg386_indb,
    cg386_indw,
    cg386_ldlab,
    
    /* Stack Operations */
    cg386_push,
    cg386_pushlit,
    cg386_pop2,
    cg386_swap,
    cg386_popptr,
    
    /* Arithmetic Operations */
    cg386_add,
    cg386_sub,
    cg386_mul,
    cg386_div,
    cg386_mod,
    cg386_neg,
    
    /* Bitwise Operations */
    cg386_and,
    cg386_ior,
    cg386_xor,
    cg386_not,
    cg386_shl,
    cg386_shr,
    
    /* Comparison Operations */
    cg386_eq,
    cg386_ne,
    cg386_lt,
    cg386_gt,
    cg386_le,
    cg386_ge,
    cg386_ult,
    cg386_ugt,
    cg386_ule,
    cg386_uge,
    
    /* Conditional Branch Operations */
    cg386_breq,
    cg386_brne,
    cg386_brlt,
    cg386_brgt,
    cg386_brle,
    cg386_brge,
    cg386_brult,
    cg386_brugt,
    cg386_brule,
    cg386_bruge,
    
    /* Boolean and Logic Operations */
    cg386_lognot,
    cg386_bool,
    
    /* Pointer Scaling Operations */
    cg386_scale,
    cg386_scale2,
    cg386_unscale,
    cg386_scaleby,
    cg386_scale2by,
    cg386_unscaleby,
    
    /* Increment/Decrement via Pointer */
    cg386_ldinc,
    cg386_inc1pi,
    cg386_dec1pi,
    cg386_inc2pi,
    cg386_dec2pi,
    
    /* Increment/Decrement Local Variables */
    cg386_incpl,
    cg386_decpl,
    cg386_inclw,
    cg386_declw,
    cg386_inclb,
    cg386_declb,
    
    /* Increment/Decrement Static Variables */
    cg386_incps,
    cg386_decps,
    cg386_incsw,
    cg386_decsw,
    cg386_incsb,
    cg386_decsb,
    
    /* Increment/Decrement Global Variables */
    cg386_incpg,
    cg386_decpg,
    cg386_incgw,
    cg386_decgw,
    cg386_incgb,
    cg386_decgb,
    
    /* Increment/Decrement via Indirect */
    cg386_inc1iw,
    cg386_dec1iw,
    cg386_inc2iw,
    cg386_dec2iw,
    cg386_inc1ib,
    cg386_dec1ib,
    cg386_inc2ib,
    cg386_dec2ib,
    
    /* Unconditional Branch */
    cg386_brtrue,
    cg386_brfalse,
    cg386_jump,
    
    /* Switch Statement Support */
    cg386_ldswtch,
    cg386_calswtch,
    cg386_case,
    
    /* Store Operations */
    cg386_storib,
    cg386_storiw,
    cg386_storlb,
    cg386_storlw,
    cg386_storsb,
    cg386_storsw,
    cg386_storgb,
    cg386_storgw,
    
    /* Function Call Operations */
    cg386_initlw,
    cg386_call,
    cg386_calr,
    cg386_stack,
    cg386_entry,
    cg386_exit,
    
    /* Data Definition */
    cg386_defb,
    cg386_defw,
    cg386_defp,
    cg386_defl,
    cg386_defc,
    cg386_gbss,
    cg386_lbss,
    
    /* Synthesizer Support */
    cg386_load2
};
