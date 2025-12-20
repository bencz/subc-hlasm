/*
 * SubC Compiler - x86-64 Linux Target
 *
 * Target: x86-64-linux
 *
 * Copyright (c) 2011-2025 - Public Domain (CC0)
 */

#include "../../cgtarget.h"
#include "../../cg_os_configs.h"
#include "cg_x86_64_arch.h"

struct cg_target target_x86_64_linux = {
    "x86-64-linux",
    "x86-64 / AMD64 (Linux)",
    &cg_x86_64_arch,
    &cg_os_linux_elf64,
    &cg_x86_64_vtable,
    NULL,
    NULL,
    NULL
};

/* Backward compatibility alias */
struct cg_target target_x86_64_alias = {
    "x86-64",
    "x86-64 / AMD64 (64-bit)",
    &cg_x86_64_arch,
    &cg_os_linux_elf64,
    &cg_x86_64_vtable,
    NULL,
    NULL,
    NULL
};
