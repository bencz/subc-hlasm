/*
 * SubC Compiler - Intel 386 FreeBSD Target
 *
 * Target: 386-freebsd
 *
 * Copyright (c) 2011-2025 - Public Domain (CC0)
 */

#include "../../cgtarget.h"
#include "../../cg_os_configs.h"
#include "cg_386_arch.h"

struct cg_target target_386_freebsd = {
    "386-freebsd",
    "Intel 386 (FreeBSD)",
    &cg_386_arch,
    &cg_os_freebsd_elf32,
    &cg_386_vtable,
    NULL,
    NULL,
    NULL
};
