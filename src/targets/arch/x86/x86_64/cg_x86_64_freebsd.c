/*
 * SubC Compiler - x86-64 FreeBSD Target
 *
 * Target: x86-64-freebsd
 *
 * Copyright (c) 2011-2025 - Public Domain (CC0)
 */

#include "../../cgtarget.h"
#include "../../cg_os_configs.h"
#include "cg_x86_64_arch.h"

struct cg_target target_x86_64_freebsd = {
    "x86-64-freebsd",
    "x86-64 / AMD64 (FreeBSD)",
    &cg_x86_64_arch,
    &cg_os_freebsd_elf64,
    &cg_x86_64_vtable,
    NULL,
    NULL,
    NULL
};
