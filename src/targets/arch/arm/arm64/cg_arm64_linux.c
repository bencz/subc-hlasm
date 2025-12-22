/*
 * SubC Compiler - ARM64 Linux Target Descriptor
 *
 * This file defines the target descriptor for ARM64 running Linux.
 * It combines the architecture properties, vtable, and OS configuration.
 *
 * Target: arm64-linux (also known as aarch64-linux)
 *
 * Copyright (c) 2025 - Public Domain (CC0)
 */

#include "../../cgtarget.h"
#include "../../cg_os_configs.h"
#include "cg_arm64_arch.h"

/*
 * OS Configuration for Linux ARM64
 *
 * Note: We define a local config since the shared cg_os_linux_elf64
 * may have x86-64 specific settings.
 */
static struct cg_os_config cg_os_linux_arm64 = {
    OS_LINUX,
    OBJ_ELF,
    0,                              /* no underscore prefix */
    "Linux",
    "as -o %s %s",                  /* assembler command */
    "ld -o %s %s/lib/%scrt0.o",     /* linker command */
    "gcc -o %s",                    /* ld_cmd_sysrt: use gcc as driver */
    "",                             /* sys_libc */
    "a.out"                         /* default output name */
};

/*
 * Target Descriptor for ARM64 Linux
 */
struct cg_target target_arm64_linux = {
    "arm64-linux",                  /* name */
    "ARM64/AArch64 (Linux)",        /* description */
    &cg_arm64_arch,                 /* arch */
    &cg_os_linux_arm64,             /* os */
    &cg_arm64_vtable,               /* vtable */
    NULL,                           /* asm_cmd_override */
    NULL,                           /* ld_cmd_override */
    NULL                            /* ext_data */
};

/*
 * Backward compatibility alias: "aarch64-linux"
 */
struct cg_target target_aarch64_linux = {
    "aarch64-linux",                /* name */
    "ARM64/AArch64 (Linux)",        /* description */
    &cg_arm64_arch,                 /* arch */
    &cg_os_linux_arm64,             /* os */
    &cg_arm64_vtable,               /* vtable */
    NULL,                           /* asm_cmd_override */
    NULL,                           /* ld_cmd_override */
    NULL                            /* ext_data */
};

/*
 * Simple alias: "arm64"
 */
struct cg_target target_arm64_alias = {
    "arm64",                        /* name */
    "ARM64/AArch64 (64-bit ARM)",   /* description */
    &cg_arm64_arch,                 /* arch */
    &cg_os_linux_arm64,             /* os - default to Linux */
    &cg_arm64_vtable,               /* vtable */
    NULL,                           /* asm_cmd_override */
    NULL,                           /* ld_cmd_override */
    NULL                            /* ext_data */
};
