/*
 * SubC Compiler - IBM System/370 Architecture Definition
 *
 * This file defines the shared architecture properties and vtable
 * for all S/370 targets. The code generation functions are in cg_s370.c.
 *
 * Architecture characteristics:
 *   - 24-bit addressing (16MB address space)
 *   - 32-bit general purpose registers
 *   - Big-endian byte order
 *   - Stack grows upward (STACK_UP)
 *   - IBM Hexadecimal floating point (FLOAT_IBM_HEX)
 *   - HLASM (High Level Assembler) syntax
 *
 * Copyright (c) 2025 - Public Domain (CC0)
 */

#include "cgtarget.h"
#include "cg_s370_funcs.h"
#include <stdio.h>

/*
 * MVS Symbol Transformation
 * 
 * HLASM has strict symbol naming rules:
 * - Maximum 8 characters
 * - Must be uppercase
 * - First character must be A-Z, @, #, or $
 * 
 * This function transforms C symbol names to valid HLASM names.
 */
static unsigned int s370_sym_hash(char *s) {
    unsigned int h = 0;
    while (*s) {
        h = h * 31 + (unsigned char)*s++;
    }
    return h;
}

char *s370_symbol_transform(char *s) {
    static char name[12];  /* 8 chars + null + margin */
    char *p;
    int i, len;
    unsigned int hash;
    
    /* Skip 'C' prefix if present (e.g., Cmain -> main) */
    p = s;
    if (*p == 'C' && p[1] >= 'a' && p[1] <= 'z') {
        p++;
    }
    
    /* Calculate length */
    for (len = 0; p[len]; len++);
    
    if (len <= 8) {
        /* Short enough, just convert to uppercase */
        for (i = 0; p[i] && i < 8; i++) {
            if (p[i] >= 'a' && p[i] <= 'z') {
                name[i] = p[i] - 'a' + 'A';
            } else {
                name[i] = p[i];
            }
        }
        name[i] = '\0';
    } else {
        /* Too long: use first 4 chars + 4-digit hash */
        hash = s370_sym_hash(p) % 10000;
        for (i = 0; i < 4 && p[i]; i++) {
            if (p[i] >= 'a' && p[i] <= 'z') {
                name[i] = p[i] - 'a' + 'A';
            } else {
                name[i] = p[i];
            }
        }
        sprintf(&name[4], "%04u", hash);
    }
    return name;
}

/*
 * ============================================================================
 * Architecture Properties
 * ============================================================================
 *
 * Shared by all S/370 targets regardless of operating system.
 *
 * Note: Although S/370 has 32-bit registers, addresses are 24-bit.
 * We use 32-bit for int/ptr since that's the register size.
 */
struct cg_arch cg_s370_arch = {
    /* Data type sizes (in bytes) */
    24,                             /* bits - 24-bit addressing */
    1,                              /* char_size */
    2,                              /* short_size */
    4,                              /* int_size - fullword */
    4,                              /* long_size - fullword */
    4,                              /* ptr_size - fullword (24-bit addr in 32-bit reg) */
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
    1,                              /* has_frame_ptr - R13 save area */
    
    /* Stack frame layout for MVS */
    0,                              /* param_offset_base - params via R11 at offset 0 */
    1,                              /* param_offset_dir - always positive offsets */
    88,                             /* local_offset_base - after 72-byte save area + 16 reserved */
    0,                              /* stack_slot_size - use BPW */
    
    /* Symbol transformation */
    s370_symbol_transform,          /* Convert to uppercase, 8-char limit */
    
    /* System runtime calling convention */
    0                               /* maxregargs - S/370 uses parameter list */
};

/*
 * ============================================================================
 * Code Generation Vtable
 * ============================================================================
 *
 * Shared by all S/370 targets. Points to functions in cg_s370.c.
 */
struct cg_vtable cg_s370_vtable = {
    /* Section Control */
    cgs370_data,
    cgs370_text,
    cgs370_prelude,
    cgs370_postlude,
    cgs370_public,
    cgs370_name,
    cgs370_funcname,
    cgs370_align,
    
    /* Literal and Clear */
    cgs370_lit,
    cgs370_clear,
    cgs370_clear2,
    
    /* Load Operations */
    cgs370_ldgb,
    cgs370_ldgw,
    cgs370_ldlb,
    cgs370_ldlw,
    cgs370_ldsb,
    cgs370_ldsw,
    cgs370_ldla,
    cgs370_ldsa,
    cgs370_ldga,
    cgs370_indb,
    cgs370_indw,
    cgs370_ldlab,
    
    /* Stack Operations */
    cgs370_push,
    cgs370_pushlit,
    cgs370_pop2,
    cgs370_swap,
    cgs370_popptr,
    
    /* Arithmetic Operations */
    cgs370_add,
    cgs370_sub,
    cgs370_mul,
    cgs370_div,
    cgs370_mod,
    cgs370_neg,
    
    /* Bitwise Operations */
    cgs370_and,
    cgs370_ior,
    cgs370_xor,
    cgs370_not,
    cgs370_shl,
    cgs370_shr,
    
    /* Comparison Operations */
    cgs370_eq,
    cgs370_ne,
    cgs370_lt,
    cgs370_gt,
    cgs370_le,
    cgs370_ge,
    cgs370_ult,
    cgs370_ugt,
    cgs370_ule,
    cgs370_uge,
    
    /* Conditional Branch Operations */
    cgs370_breq,
    cgs370_brne,
    cgs370_brlt,
    cgs370_brgt,
    cgs370_brle,
    cgs370_brge,
    cgs370_brult,
    cgs370_brugt,
    cgs370_brule,
    cgs370_bruge,
    
    /* Boolean and Logic Operations */
    cgs370_lognot,
    cgs370_bool,
    
    /* Pointer Scaling Operations */
    cgs370_scale,
    cgs370_scale2,
    cgs370_unscale,
    cgs370_scaleby,
    cgs370_scale2by,
    cgs370_unscaleby,
    
    /* Increment/Decrement via Pointer */
    cgs370_ldinc,
    cgs370_inc1pi,
    cgs370_dec1pi,
    cgs370_inc2pi,
    cgs370_dec2pi,
    
    /* Increment/Decrement Local Variables */
    cgs370_incpl,
    cgs370_decpl,
    cgs370_inclw,
    cgs370_declw,
    cgs370_inclb,
    cgs370_declb,
    
    /* Increment/Decrement Static Variables */
    cgs370_incps,
    cgs370_decps,
    cgs370_incsw,
    cgs370_decsw,
    cgs370_incsb,
    cgs370_decsb,
    
    /* Increment/Decrement Global Variables */
    cgs370_incpg,
    cgs370_decpg,
    cgs370_incgw,
    cgs370_decgw,
    cgs370_incgb,
    cgs370_decgb,
    
    /* Increment/Decrement via Indirect */
    cgs370_inc1iw,
    cgs370_dec1iw,
    cgs370_inc2iw,
    cgs370_dec2iw,
    cgs370_inc1ib,
    cgs370_dec1ib,
    cgs370_inc2ib,
    cgs370_dec2ib,
    
    /* Unconditional Branch */
    cgs370_brtrue,
    cgs370_brfalse,
    cgs370_jump,
    
    /* Switch Statement Support */
    cgs370_ldswtch,
    cgs370_calswtch,
    cgs370_case,
    
    /* Store Operations */
    cgs370_storib,
    cgs370_storiw,
    cgs370_storlb,
    cgs370_storlw,
    cgs370_storsb,
    cgs370_storsw,
    cgs370_storgb,
    cgs370_storgw,
    
    /* Function Call Operations */
    cgs370_initlw,
    cgs370_call,
    cgs370_calr,
    cgs370_stack,
    cgs370_entry,
    cgs370_exit,
    
    /* System Runtime Calling Convention Support */
    NULL,                           /* cgmovearg - not applicable for S/370 */
    NULL,                           /* cgemitargs - not applicable for S/370 */
    
    /* Data Definition */
    cgs370_defb,
    cgs370_defw,
    cgs370_defp,
    cgs370_defl,
    cgs370_defc,
    cgs370_gbss,
    cgs370_lbss,
    
    /* Synthesizer Support */
    cgs370_load2
};
