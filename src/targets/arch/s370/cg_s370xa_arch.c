/*
 * SubC Compiler - IBM System/370-XA Architecture Definition
 *
 * This file defines the architecture properties for S/370-XA (31-bit mode).
 * It reuses the same vtable as S/370 since the instruction set is identical.
 *
 * S/370-XA extends S/370 with:
 *   - 31-bit addressing (2GB address space vs 16MB)
 *   - Same instruction set as S/370
 *   - Same register usage
 *
 * Copyright (c) 2025 - Public Domain (CC0)
 */

#include "cgtarget.h"

/*
 * ============================================================================
 * Architecture Properties for S/370-XA (31-bit)
 * ============================================================================
 *
 * The only difference from S/370 is the addressing mode (31-bit vs 24-bit).
 * All other properties remain the same.
 */
struct cg_arch cg_s370xa_arch = {
    /* Data type sizes (in bytes) */
    31,                             /* bits - 31-bit addressing */
    1,                              /* char_size */
    2,                              /* short_size */
    4,                              /* int_size - fullword */
    4,                              /* long_size - fullword */
    4,                              /* ptr_size - fullword (31-bit addr in 32-bit reg) */
    4,                              /* bpw - bytes per word */
    
    /* Architecture characteristics */
    ENDIAN_BIG,                     /* Big-endian */
    STACK_UP,                       /* Stack grows upward */
    ASM_HLASM,                      /* HLASM syntax */
    CC_HLASM,                       /* S/370 calling convention */
    FLOAT_IBM_HEX,                  /* IBM Hexadecimal floating point */
    
    /* Alignment requirements */
    8,                              /* align_stack - doubleword */
    4,                              /* align_data - fullword */
    8,                              /* align_func - doubleword */
    
    /* Hardware capabilities */
    1,                              /* has_mul - MR instruction */
    1,                              /* has_div - DR instruction */
    1,                              /* has_mod - DR gives remainder */
    1,                              /* has_barrel_shift - SLL/SRL/SLA/SRA */
    1,                              /* has_byte_ops - IC/STC */
    1,                              /* needs_alignment - yes, for fullwords */
    0,                              /* needs_pic - no */
    1                               /* has_frame_ptr - R13 save area */
};
