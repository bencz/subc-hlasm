/*
 * SubC Compiler - x86-64 Architecture Definition
 *
 * This file defines the shared architecture properties and vtable
 * for all x86-64 targets. The code generation functions are in cg_x86_64.c.
 *
 * Copyright (c) 2011-2025 - Public Domain (CC0)
 */

#include "../../cgtarget.h"
#include "cg_x86_64_funcs.h"

/*
 * ============================================================================
 * Architecture Properties
 * ============================================================================
 */
struct cg_arch cg_x86_64_arch = {
    /* Data type sizes */
    64,                             /* bits */
    1,                              /* char_size */
    2,                              /* short_size */
    8,                              /* int_size */
    8,                              /* long_size */
    8,                              /* ptr_size */
    8,                              /* bpw */
    
    /* Architecture characteristics */
    ENDIAN_LITTLE,
    STACK_DOWN,
    ASM_GAS,
    CC_SYSV_AMD64,
    FLOAT_IEEE754,
    
    /* Alignment */
    16,                             /* align_stack (16-byte for SSE) */
    8,                              /* align_data */
    16,                             /* align_func */
    
    /* Hardware capabilities */
    1,                              /* has_mul */
    1,                              /* has_div */
    1,                              /* has_mod */
    1,                              /* has_barrel_shift */
    1,                              /* has_byte_ops */
    0,                              /* needs_alignment */
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
struct cg_vtable cg_x86_64_vtable = {
    /* Section Control */
    cgx64_data,
    cgx64_text,
    cgx64_prelude,
    cgx64_postlude,
    cgx64_public,
    cgx64_name,
    cgx64_funcname,
    cgx64_align,
    
    /* Literal and Clear */
    cgx64_lit,
    cgx64_clear,
    cgx64_clear2,
    
    /* Load Operations */
    cgx64_ldgb,
    cgx64_ldgw,
    cgx64_ldlb,
    cgx64_ldlw,
    cgx64_ldsb,
    cgx64_ldsw,
    cgx64_ldla,
    cgx64_ldsa,
    cgx64_ldga,
    cgx64_indb,
    cgx64_indw,
    cgx64_ldlab,
    
    /* Stack Operations */
    cgx64_push,
    cgx64_pushlit,
    cgx64_pop2,
    cgx64_swap,
    cgx64_popptr,
    
    /* Arithmetic Operations */
    cgx64_add,
    cgx64_sub,
    cgx64_mul,
    cgx64_div,
    cgx64_mod,
    cgx64_neg,
    
    /* Bitwise Operations */
    cgx64_and,
    cgx64_ior,
    cgx64_xor,
    cgx64_not,
    cgx64_shl,
    cgx64_shr,
    
    /* Comparison Operations */
    cgx64_eq,
    cgx64_ne,
    cgx64_lt,
    cgx64_gt,
    cgx64_le,
    cgx64_ge,
    cgx64_ult,
    cgx64_ugt,
    cgx64_ule,
    cgx64_uge,
    
    /* Conditional Branch Operations */
    cgx64_breq,
    cgx64_brne,
    cgx64_brlt,
    cgx64_brgt,
    cgx64_brle,
    cgx64_brge,
    cgx64_brult,
    cgx64_brugt,
    cgx64_brule,
    cgx64_bruge,
    
    /* Boolean and Logic Operations */
    cgx64_lognot,
    cgx64_bool,
    
    /* Pointer Scaling Operations */
    cgx64_scale,
    cgx64_scale2,
    cgx64_unscale,
    cgx64_scaleby,
    cgx64_scale2by,
    cgx64_unscaleby,
    
    /* Increment/Decrement via Pointer */
    cgx64_ldinc,
    cgx64_inc1pi,
    cgx64_dec1pi,
    cgx64_inc2pi,
    cgx64_dec2pi,
    
    /* Increment/Decrement Local Variables */
    cgx64_incpl,
    cgx64_decpl,
    cgx64_inclw,
    cgx64_declw,
    cgx64_inclb,
    cgx64_declb,
    
    /* Increment/Decrement Static Variables */
    cgx64_incps,
    cgx64_decps,
    cgx64_incsw,
    cgx64_decsw,
    cgx64_incsb,
    cgx64_decsb,
    
    /* Increment/Decrement Global Variables */
    cgx64_incpg,
    cgx64_decpg,
    cgx64_incgw,
    cgx64_decgw,
    cgx64_incgb,
    cgx64_decgb,
    
    /* Increment/Decrement via Indirect */
    cgx64_inc1iw,
    cgx64_dec1iw,
    cgx64_inc2iw,
    cgx64_dec2iw,
    cgx64_inc1ib,
    cgx64_dec1ib,
    cgx64_inc2ib,
    cgx64_dec2ib,
    
    /* Unconditional Branch */
    cgx64_brtrue,
    cgx64_brfalse,
    cgx64_jump,
    
    /* Switch Statement Support */
    cgx64_ldswtch,
    cgx64_calswtch,
    cgx64_case,
    
    /* Store Operations */
    cgx64_storib,
    cgx64_storiw,
    cgx64_storlb,
    cgx64_storlw,
    cgx64_storsb,
    cgx64_storsw,
    cgx64_storgb,
    cgx64_storgw,
    
    /* Function Call Operations */
    cgx64_initlw,
    cgx64_call,
    cgx64_calr,
    cgx64_stack,
    cgx64_entry,
    cgx64_exit,
    
    /* System Runtime Calling Convention Support */
    NULL,                           /* cgmovearg - not implemented for x86-64 */
    0,                              /* maxregargs - would be 6 for SysV AMD64 */
    NULL,                           /* cgemitargs - not implemented for x86-64 */
    
    /* Data Definition */
    cgx64_defb,
    cgx64_defw,
    cgx64_defp,
    cgx64_defl,
    cgx64_defc,
    cgx64_gbss,
    cgx64_lbss,
    
    /* Synthesizer Support */
    cgx64_load2
};
