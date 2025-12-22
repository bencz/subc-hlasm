/*
 * SubC Compiler - ARM64 (AArch64) Architecture Definition
 *
 * This file defines the shared architecture properties and vtable
 * for all ARM64 targets. The code generation functions are in cg_arm64.c.
 *
 * Architecture characteristics:
 *   - 64-bit registers and pointers
 *   - Little-endian byte order (default)
 *   - Stack grows downward (toward lower addresses)
 *   - 16-byte stack alignment required
 *   - IEEE 754 floating point
 *   - AAPCS64 calling convention
 *
 * Copyright (c) 2025 - Public Domain (CC0)
 */

#include "../../cgtarget.h"
#include "cg_arm64_funcs.h"

/*
 * ============================================================================
 * Architecture Properties
 * ============================================================================
 *
 * Shared by all ARM64 targets regardless of operating system.
 */
struct cg_arch cg_arm64_arch = {
    /* Data type sizes (in bytes) */
    64,                             /* bits - 64-bit architecture */
    1,                              /* char_size */
    2,                              /* short_size */
    8,                              /* int_size - 64-bit for SubC on ARM64 */
    8,                              /* long_size */
    8,                              /* ptr_size */
    8,                              /* bpw - bytes per word */
    
    /* Architecture characteristics */
    ENDIAN_LITTLE,                  /* Little-endian (default) */
    STACK_DOWN,                     /* Stack grows downward */
    ASM_GAS,                        /* GNU Assembler syntax */
    CC_AAPCS64,                     /* AAPCS64 calling convention */
    FLOAT_IEEE754,                  /* IEEE 754 floating point */
    
    /* Alignment requirements */
    16,                             /* align_stack - 16-byte alignment required */
    8,                              /* align_data - 8-byte for 64-bit data */
    4,                              /* align_func - 4-byte instruction alignment */
    
    /* Hardware capabilities */
    1,                              /* has_mul - MUL instruction */
    1,                              /* has_div - SDIV/UDIV instructions */
    1,                              /* has_mod - via MSUB */
    1,                              /* has_barrel_shift - LSL/LSR/ASR */
    1,                              /* has_byte_ops - LDRB/STRB */
    1,                              /* needs_alignment - yes */
    1,                              /* needs_pic - position independent code */
    1,                              /* has_frame_ptr - x29 */
    
    /* Stack frame layout */
    0,                              /* param_offset_base - calculated as 2*BPW */
    0,                              /* param_offset_dir - follows stack_dir */
    0,                              /* local_offset_base */
    16,                             /* stack_slot_size - 16-byte alignment required */
    
    /* Symbol transformation */
    NULL,                           /* symbol_transform - use default */
    
    /* System runtime calling convention */
    8                               /* maxregargs - AAPCS64 uses x0-x7 */
};

/*
 * ============================================================================
 * Code Generation Vtable
 * ============================================================================
 *
 * Shared by all ARM64 targets. Points to functions in cg_arm64.c.
 */
struct cg_vtable cg_arm64_vtable = {
    /* Section Control */
    cga64_data,
    cga64_text,
    cga64_prelude,
    cga64_postlude,
    cga64_public,
    cga64_name,
    cga64_funcname,
    cga64_align,
    
    /* Literal and Clear */
    cga64_lit,
    cga64_clear,
    cga64_clear2,
    
    /* Load Operations */
    cga64_ldgb,
    cga64_ldgw,
    cga64_ldlb,
    cga64_ldlw,
    cga64_ldsb,
    cga64_ldsw,
    cga64_ldla,
    cga64_ldsa,
    cga64_ldga,
    cga64_indb,
    cga64_indw,
    cga64_ldlab,
    
    /* Stack Operations */
    cga64_push,
    cga64_pushlit,
    cga64_pop2,
    cga64_swap,
    cga64_popptr,
    
    /* Arithmetic Operations */
    cga64_add,
    cga64_sub,
    cga64_mul,
    cga64_div,
    cga64_mod,
    cga64_neg,
    
    /* Bitwise Operations */
    cga64_and,
    cga64_ior,
    cga64_xor,
    cga64_not,
    cga64_shl,
    cga64_shr,
    
    /* Comparison Operations */
    cga64_eq,
    cga64_ne,
    cga64_lt,
    cga64_gt,
    cga64_le,
    cga64_ge,
    cga64_ult,
    cga64_ugt,
    cga64_ule,
    cga64_uge,
    
    /* Conditional Branch Operations */
    cga64_breq,
    cga64_brne,
    cga64_brlt,
    cga64_brgt,
    cga64_brle,
    cga64_brge,
    cga64_brult,
    cga64_brugt,
    cga64_brule,
    cga64_bruge,
    
    /* Boolean and Logic Operations */
    cga64_lognot,
    cga64_bool,
    
    /* Pointer Scaling Operations */
    cga64_scale,
    cga64_scale2,
    cga64_unscale,
    cga64_scaleby,
    cga64_scale2by,
    cga64_unscaleby,
    
    /* Increment/Decrement via Pointer */
    cga64_ldinc,
    cga64_inc1pi,
    cga64_dec1pi,
    cga64_inc2pi,
    cga64_dec2pi,
    
    /* Increment/Decrement Local Variables */
    cga64_incpl,
    cga64_decpl,
    cga64_inclw,
    cga64_declw,
    cga64_inclb,
    cga64_declb,
    
    /* Increment/Decrement Static Variables */
    cga64_incps,
    cga64_decps,
    cga64_incsw,
    cga64_decsw,
    cga64_incsb,
    cga64_decsb,
    
    /* Increment/Decrement Global Variables */
    cga64_incpg,
    cga64_decpg,
    cga64_incgw,
    cga64_decgw,
    cga64_incgb,
    cga64_decgb,
    
    /* Increment/Decrement via Indirect */
    cga64_inc1iw,
    cga64_dec1iw,
    cga64_inc2iw,
    cga64_dec2iw,
    cga64_inc1ib,
    cga64_dec1ib,
    cga64_inc2ib,
    cga64_dec2ib,
    
    /* Unconditional Branch */
    cga64_brtrue,
    cga64_brfalse,
    cga64_jump,
    
    /* Switch Statement Support */
    cga64_ldswtch,
    cga64_calswtch,
    cga64_case,
    
    /* Store Operations */
    cga64_storib,
    cga64_storiw,
    cga64_storlb,
    cga64_storlw,
    cga64_storsb,
    cga64_storsw,
    cga64_storgb,
    cga64_storgw,
    
    /* Function Call Operations */
    cga64_initlw,
    cga64_call,
    cga64_calr,
    cga64_stack,
    cga64_entry,
    cga64_exit,
    
    /* System Runtime Calling Convention Support */
    cga64_movearg,
    cga64_emitargs,                 /* cgemitargs - emit args using AAPCS64 */
    
    /* Data Definition */
    cga64_defb,
    cga64_defw,
    cga64_defp,
    cga64_defl,
    cga64_defc,
    cga64_gbss,
    cga64_lbss,
    
    /* Synthesizer Support */
    cga64_load2
};
