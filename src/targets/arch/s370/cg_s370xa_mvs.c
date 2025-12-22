/*
 * SubC Compiler - IBM System/370-XA MVS Target Descriptor
 *
 * This file defines the target descriptor for S/370-XA (31-bit) running MVS/XA.
 * It reuses the same vtable as S/370 since the code generation is identical.
 *
 * Copyright (c) 2025 - Public Domain (CC0)
 */

#include "cgtarget.h"

/* Import shared vtable from S/370 */
extern struct cg_vtable cg_s370_vtable;

/* Import S/370-XA architecture properties */
extern struct cg_arch cg_s370xa_arch;

/*
 * OS Configuration for MVS/XA
 */
static struct cg_os_config cg_s370xa_mvs_os = {
    OS_MVS,                         /* target_os */
    OBJ_GOFF,                       /* obj_format - Generalized Object File Format */
    0,                              /* underscore_sym - no underscore prefix */
    "MVS/XA",                       /* os_name */
    "",                             /* asm_cmd - HLASM is run separately */
    "",                             /* ld_cmd - linkage editor is run separately */
    NULL,                           /* ld_cmd_sysrt - not applicable on MVS */
    "",                             /* sys_libc */
    "a.obj"                         /* aout_name */
};

/*
 * Target Descriptor for S/370-XA MVS
 *
 * Note: Uses the same vtable as S/370 (cg_s370_vtable) since the
 * instruction set is identical. Only the architecture properties differ.
 */
struct cg_target cg_s370xa_mvs_target = {
    "s370-xa",                      /* name */
    "IBM System/370-XA (31-bit MVS)", /* description */
    &cg_s370xa_arch,                /* arch - 31-bit addressing */
    &cg_s370xa_mvs_os,              /* os */
    &cg_s370_vtable,                /* vtable - REUSED from S/370! */
    NULL,                           /* asm_cmd_override */
    NULL,                           /* ld_cmd_override */
    NULL                            /* ext_data */
};
