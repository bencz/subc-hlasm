/*
 * SubC Compiler - ARMv6 Linux Target
 *
 * Target: armv6-linux
 *
 * Copyright (c) 2011-2025 - Public Domain (CC0)
 */

#include "../../cgtarget.h"
#include "../../cg_os_configs.h"
#include "cg_armv6_arch.h"

struct cg_target target_armv6_linux = {
    "armv6-linux",
    "ARMv6 (Linux)",
    &cg_armv6_arch,
    &cg_os_linux_elf32,
    &cg_armv6_vtable,
    NULL,
    NULL,
    NULL
};

/* Backward compatibility alias */
struct cg_target target_armv6_alias = {
    "armv6",
    "ARMv6 (32-bit ARM)",
    &cg_armv6_arch,
    &cg_os_linux_elf32,
    &cg_armv6_vtable,
    NULL,
    NULL,
    NULL
};
