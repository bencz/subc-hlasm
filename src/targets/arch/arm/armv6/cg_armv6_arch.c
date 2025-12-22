/*
 * SubC Compiler - ARMv6 Architecture Definition
 *
 * Copyright (c) 2011-2025 - Public Domain (CC0)
 */

#include "../../cgtarget.h"
#include "cg_armv6_funcs.h"

/*
 * ============================================================================
 * Architecture Properties
 * ============================================================================
 */
struct cg_arch cg_armv6_arch = {
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
    ASM_ARM,
    CC_AAPCS,
    FLOAT_IEEE754,                  /* VFP uses IEEE 754 */
    
    /* Alignment */
    8,                              /* align_stack */
    4,                              /* align_data */
    4,                              /* align_func */
    
    /* Hardware capabilities */
    1,                              /* has_mul */
    0,                              /* has_div (not on ARMv6) */
    0,                              /* has_mod */
    1,                              /* has_barrel_shift */
    1,                              /* has_byte_ops */
    1,                              /* needs_alignment */
    0,                              /* needs_pic */
    1,                              /* has_frame_ptr */
    
    /* Stack frame layout */
    0,                              /* param_offset_base - calculated as 2*BPW */
    0,                              /* param_offset_dir - follows stack_dir */
    0,                              /* local_offset_base */
    0,                              /* stack_slot_size - use BPW */
    
    /* Symbol transformation */
    NULL                            /* symbol_transform - use default */
};

/*
 * ============================================================================
 * Code Generation Vtable
 * ============================================================================
 */
struct cg_vtable cg_armv6_vtable = {
    /* Section Control */
    cgarm_data,
    cgarm_text,
    cgarm_prelude,
    cgarm_postlude,
    cgarm_public,
    cgarm_name,
    cgarm_funcname,
    cgarm_align,
    
    /* Literal and Clear */
    cgarm_lit,
    cgarm_clear,
    cgarm_clear2,
    
    /* Load Operations */
    cgarm_ldgb,
    cgarm_ldgw,
    cgarm_ldlb,
    cgarm_ldlw,
    cgarm_ldsb,
    cgarm_ldsw,
    cgarm_ldla,
    cgarm_ldsa,
    cgarm_ldga,
    cgarm_indb,
    cgarm_indw,
    cgarm_ldlab,
    
    /* Stack Operations */
    cgarm_push,
    cgarm_pushlit,
    cgarm_pop2,
    cgarm_swap,
    cgarm_popptr,
    
    /* Arithmetic Operations */
    cgarm_add,
    cgarm_sub,
    cgarm_mul,
    cgarm_div,
    cgarm_mod,
    cgarm_neg,
    
    /* Bitwise Operations */
    cgarm_and,
    cgarm_ior,
    cgarm_xor,
    cgarm_not,
    cgarm_shl,
    cgarm_shr,
    
    /* Comparison Operations */
    cgarm_eq,
    cgarm_ne,
    cgarm_lt,
    cgarm_gt,
    cgarm_le,
    cgarm_ge,
    cgarm_ult,
    cgarm_ugt,
    cgarm_ule,
    cgarm_uge,
    
    /* Conditional Branch Operations */
    cgarm_breq,
    cgarm_brne,
    cgarm_brlt,
    cgarm_brgt,
    cgarm_brle,
    cgarm_brge,
    cgarm_brult,
    cgarm_brugt,
    cgarm_brule,
    cgarm_bruge,
    
    /* Boolean and Logic Operations */
    cgarm_lognot,
    cgarm_bool,
    
    /* Pointer Scaling Operations */
    cgarm_scale,
    cgarm_scale2,
    cgarm_unscale,
    cgarm_scaleby,
    cgarm_scale2by,
    cgarm_unscaleby,
    
    /* Increment/Decrement via Pointer */
    cgarm_ldinc,
    cgarm_inc1pi,
    cgarm_dec1pi,
    cgarm_inc2pi,
    cgarm_dec2pi,
    
    /* Increment/Decrement Local Variables */
    cgarm_incpl,
    cgarm_decpl,
    cgarm_inclw,
    cgarm_declw,
    cgarm_inclb,
    cgarm_declb,
    
    /* Increment/Decrement Static Variables */
    cgarm_incps,
    cgarm_decps,
    cgarm_incsw,
    cgarm_decsw,
    cgarm_incsb,
    cgarm_decsb,
    
    /* Increment/Decrement Global Variables */
    cgarm_incpg,
    cgarm_decpg,
    cgarm_incgw,
    cgarm_decgw,
    cgarm_incgb,
    cgarm_decgb,
    
    /* Increment/Decrement via Indirect */
    cgarm_inc1iw,
    cgarm_dec1iw,
    cgarm_inc2iw,
    cgarm_dec2iw,
    cgarm_inc1ib,
    cgarm_dec1ib,
    cgarm_inc2ib,
    cgarm_dec2ib,
    
    /* Unconditional Branch */
    cgarm_brtrue,
    cgarm_brfalse,
    cgarm_jump,
    
    /* Switch Statement Support */
    cgarm_ldswtch,
    cgarm_calswtch,
    cgarm_case,
    
    /* Store Operations */
    cgarm_storib,
    cgarm_storiw,
    cgarm_storlb,
    cgarm_storlw,
    cgarm_storsb,
    cgarm_storsw,
    cgarm_storgb,
    cgarm_storgw,
    
    /* Function Call Operations */
    cgarm_initlw,
    cgarm_call,
    cgarm_calr,
    cgarm_stack,
    cgarm_entry,
    cgarm_exit,
    
    /* System Runtime Calling Convention Support */
    NULL,                           /* cgmovearg - not implemented for ARMv6 */
    0,                              /* maxregargs - would be 4 for AAPCS */
    NULL,                           /* cgemitargs - not implemented for ARMv6 */
    
    /* Data Definition */
    cgarm_defb,
    cgarm_defw,
    cgarm_defp,
    cgarm_defl,
    cgarm_defc,
    cgarm_gbss,
    cgarm_lbss,
    
    /* Synthesizer Support */
    cgarm_load2
};
