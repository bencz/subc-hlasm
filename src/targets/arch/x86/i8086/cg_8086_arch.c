/*
 * SubC Compiler - Intel 8086 Architecture Definition
 *
 * This file defines the shared architecture properties and vtable
 * for all 8086 targets. The code generation functions are in cg_8086.c.
 *
 * Copyright (c) 2011-2025 - Public Domain (CC0)
 */

#include "../../cgtarget.h"
#include "cg_8086_funcs.h"

/*
 * ============================================================================
 * Architecture Properties
 * ============================================================================
 */
struct cg_arch cg_8086_arch = {
    /* Data type sizes */
    16,                             /* bits */
    1,                              /* char_size */
    2,                              /* short_size */
    2,                              /* int_size */
    4,                              /* long_size (not fully supported) */
    2,                              /* ptr_size */
    2,                              /* bpw */
    
    /* Architecture characteristics */
    ENDIAN_LITTLE,
    STACK_DOWN,
    ASM_TASM,
    CC_CDECL,
    FLOAT_NONE,                     /* 8086 has no FPU (8087 is separate) */
    
    /* Alignment */
    2,                              /* align_stack */
    2,                              /* align_data */
    2,                              /* align_func */
    
    /* Hardware capabilities */
    1,                              /* has_mul */
    1,                              /* has_div */
    1,                              /* has_mod */
    0,                              /* has_barrel_shift (only by 1 or CL) */
    1,                              /* has_byte_ops */
    0,                              /* needs_alignment */
    0,                              /* needs_pic */
    1                               /* has_frame_ptr */
};

/*
 * ============================================================================
 * Code Generation Vtable
 * ============================================================================
 */
struct cg_vtable cg_8086_vtable = {
    /* Section Control */
    cg86_data,
    cg86_text,
    cg86_prelude,
    cg86_postlude,
    cg86_public,
    cg86_align,
    
    /* Literal and Clear */
    cg86_lit,
    cg86_clear,
    cg86_clear2,
    
    /* Load Operations */
    cg86_ldgb,
    cg86_ldgw,
    cg86_ldlb,
    cg86_ldlw,
    cg86_ldsb,
    cg86_ldsw,
    cg86_ldla,
    cg86_ldsa,
    cg86_ldga,
    cg86_indb,
    cg86_indw,
    cg86_ldlab,
    
    /* Stack Operations */
    cg86_push,
    cg86_pushlit,
    cg86_pop2,
    cg86_swap,
    cg86_popptr,
    
    /* Arithmetic Operations */
    cg86_add,
    cg86_sub,
    cg86_mul,
    cg86_div,
    cg86_mod,
    cg86_neg,
    
    /* Bitwise Operations */
    cg86_and,
    cg86_ior,
    cg86_xor,
    cg86_not,
    cg86_shl,
    cg86_shr,
    
    /* Comparison Operations */
    cg86_eq,
    cg86_ne,
    cg86_lt,
    cg86_gt,
    cg86_le,
    cg86_ge,
    cg86_ult,
    cg86_ugt,
    cg86_ule,
    cg86_uge,
    
    /* Conditional Branch Operations */
    cg86_breq,
    cg86_brne,
    cg86_brlt,
    cg86_brgt,
    cg86_brle,
    cg86_brge,
    cg86_brult,
    cg86_brugt,
    cg86_brule,
    cg86_bruge,
    
    /* Boolean and Logic Operations */
    cg86_lognot,
    cg86_bool,
    
    /* Pointer Scaling Operations */
    cg86_scale,
    cg86_scale2,
    cg86_unscale,
    cg86_scaleby,
    cg86_scale2by,
    cg86_unscaleby,
    
    /* Increment/Decrement via Pointer */
    cg86_ldinc,
    cg86_inc1pi,
    cg86_dec1pi,
    cg86_inc2pi,
    cg86_dec2pi,
    
    /* Increment/Decrement Local Variables */
    cg86_incpl,
    cg86_decpl,
    cg86_inclw,
    cg86_declw,
    cg86_inclb,
    cg86_declb,
    
    /* Increment/Decrement Static Variables */
    cg86_incps,
    cg86_decps,
    cg86_incsw,
    cg86_decsw,
    cg86_incsb,
    cg86_decsb,
    
    /* Increment/Decrement Global Variables */
    cg86_incpg,
    cg86_decpg,
    cg86_incgw,
    cg86_decgw,
    cg86_incgb,
    cg86_decgb,
    
    /* Increment/Decrement via Indirect */
    cg86_inc1iw,
    cg86_dec1iw,
    cg86_inc2iw,
    cg86_dec2iw,
    cg86_inc1ib,
    cg86_dec1ib,
    cg86_inc2ib,
    cg86_dec2ib,
    
    /* Unconditional Branch */
    cg86_brtrue,
    cg86_brfalse,
    cg86_jump,
    
    /* Switch Statement Support */
    cg86_ldswtch,
    cg86_calswtch,
    cg86_case,
    
    /* Store Operations */
    cg86_storib,
    cg86_storiw,
    cg86_storlb,
    cg86_storlw,
    cg86_storsb,
    cg86_storsw,
    cg86_storgb,
    cg86_storgw,
    
    /* Function Call Operations */
    cg86_initlw,
    cg86_call,
    cg86_calr,
    cg86_stack,
    cg86_entry,
    cg86_exit,
    
    /* Data Definition */
    cg86_defb,
    cg86_defw,
    cg86_defp,
    cg86_defl,
    cg86_defc,
    cg86_gbss,
    cg86_lbss,
    
    /* Synthesizer Support */
    cg86_load2
};
