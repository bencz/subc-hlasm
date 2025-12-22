/*
 * SubC Compiler - ARM64 Darwin (macOS) Target Descriptor
 *
 * This file defines the target descriptor for ARM64 running Darwin/macOS.
 * It combines the architecture properties, vtable, and OS configuration.
 *
 * Target: arm64-darwin (Apple Silicon Macs - M1, M2, M3, etc.)
 *
 * Darwin/macOS specifics:
 *   - Mach-O object format
 *   - Symbols require underscore prefix
 *   - Uses @PAGE/@PAGEOFF for PC-relative addressing
 *   - Requires linking with -lSystem
 *
 * Copyright (c) 2025 - Public Domain (CC0)
 */

#include "../../cgtarget.h"
#include "cg_arm64_arch.h"

/*
 * OS Configuration for Darwin ARM64 (Apple Silicon)
 */
static struct cg_os_config cg_os_darwin_arm64 = {
    OS_DARWIN,
    OBJ_MACHO,
    1,                              /* underscore prefix for symbols */
    "Darwin",
    "as -o %s %s",                  /* assembler command */
    "ld -o %s %s/lib/%scrt0.o -lSystem -syslibroot `xcrun --show-sdk-path` -e _start -arch arm64",
    "clang -arch arm64 -o %s",      /* ld_cmd_sysrt: use clang as driver */
    "-lSystem",                     /* sys_libc */
    "a.out"                         /* default output name */
};

/*
 * Target Descriptor for ARM64 Darwin
 */
struct cg_target target_arm64_darwin = {
    "arm64-darwin",                 /* name */
    "ARM64/AArch64 (Darwin/macOS)", /* description */
    &cg_arm64_arch,                 /* arch */
    &cg_os_darwin_arm64,            /* os */
    &cg_arm64_vtable,               /* vtable */
    NULL,                           /* asm_cmd_override */
    NULL,                           /* ld_cmd_override */
    NULL                            /* ext_data */
};

/*
 * Alias: "aarch64-darwin"
 */
struct cg_target target_aarch64_darwin = {
    "aarch64-darwin",               /* name */
    "ARM64/AArch64 (Darwin/macOS)", /* description */
    &cg_arm64_arch,                 /* arch */
    &cg_os_darwin_arm64,            /* os */
    &cg_arm64_vtable,               /* vtable */
    NULL,                           /* asm_cmd_override */
    NULL,                           /* ld_cmd_override */
    NULL                            /* ext_data */
};

/*
 * Alias: "arm64-macos" for convenience
 */
struct cg_target target_arm64_macos = {
    "arm64-macos",                  /* name */
    "ARM64/AArch64 (macOS)",        /* description */
    &cg_arm64_arch,                 /* arch */
    &cg_os_darwin_arm64,            /* os */
    &cg_arm64_vtable,               /* vtable */
    NULL,                           /* asm_cmd_override */
    NULL,                           /* ld_cmd_override */
    NULL                            /* ext_data */
};
