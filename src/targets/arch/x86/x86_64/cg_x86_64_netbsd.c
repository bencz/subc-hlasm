/*
 * SubC Compiler - x86-64 NetBSD Target
 *
 * Target: x86-64-netbsd
 *
 * Copyright (c) 2011-2025 - Public Domain (CC0)
 */

#include "../../cgtarget.h"
#include "../../cg_os_configs.h"
#include "cg_x86_64_arch.h"

struct cg_target target_x86_64_netbsd = {
    "x86-64-netbsd",
    "x86-64 / AMD64 (NetBSD)",
    &cg_x86_64_arch,
    &cg_os_netbsd_elf64,
    &cg_x86_64_vtable,
    NULL,
    NULL,
    NULL
};
