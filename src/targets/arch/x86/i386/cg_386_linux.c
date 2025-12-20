/*
 * SubC Compiler - Intel 386 Linux Target
 *
 * This file defines the target descriptor for i386 Linux.
 * It combines the shared i386 architecture with Linux OS config.
 *
 * Target: 386-linux
 *
 * Copyright (c) 2011-2025 - Public Domain (CC0)
 */

#include "../../cgtarget.h"
#include "../../cg_os_configs.h"
#include "cg_386_arch.h"

/*
 * Target descriptor for i386 Linux
 */
struct cg_target target_386_linux = {
    "386-linux",
    "Intel 386 (Linux)",
    &cg_386_arch,
    &cg_os_linux_elf32,
    &cg_386_vtable,
    NULL,                           /* asm_cmd_override */
    NULL,                           /* ld_cmd_override */
    NULL                            /* ext_data */
};

/*
 * Backward compatibility alias: "386" -> "386-linux"
 */
struct cg_target target_386 = {
    "386",
    "Intel 386 (32-bit)",
    &cg_386_arch,
    &cg_os_linux_elf32,
    &cg_386_vtable,
    NULL,
    NULL,
    NULL
};
