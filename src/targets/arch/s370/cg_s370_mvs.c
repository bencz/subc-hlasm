/*
 * SubC Compiler - IBM System/370 MVS Target Descriptor
 *
 * This file defines the target descriptor for S/370 running MVS.
 * It combines the architecture properties, vtable, and OS configuration.
 *
 * Copyright (c) 2025 - Public Domain (CC0)
 */

#include "cgtarget.h"
#include "cg_s370_arch.h"

/*
 * OS Configuration for MVS
 */
static struct cg_os_config cg_s370_mvs_os = {
    OS_MVS,                         /* target_os */
    OBJ_GOFF,                       /* obj_format - Generalized Object File Format */
    0,                              /* underscore_sym - no underscore prefix */
    "MVS",                          /* os_name */
    "",                             /* asm_cmd - HLASM is run separately */
    "",                             /* ld_cmd - linkage editor is run separately */
    NULL,                           /* ld_cmd_sysrt - not applicable on MVS */
    "",                             /* sys_libc */
    "a.obj"                         /* aout_name */
};

/*
 * Target Descriptor for S/370 MVS
 */
struct cg_target cg_s370_mvs_target = {
    "s370",                         /* name */
    "IBM System/370 (24-bit MVS)",  /* description */
    &cg_s370_arch,                  /* arch */
    &cg_s370_mvs_os,                /* os */
    &cg_s370_vtable,                /* vtable */
    NULL,                           /* asm_cmd_override */
    NULL,                           /* ld_cmd_override */
    NULL                            /* ext_data */
};
