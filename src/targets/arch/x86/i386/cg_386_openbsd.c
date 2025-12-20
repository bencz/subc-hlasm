/*
 * SubC Compiler - Intel 386 OpenBSD Target
 *
 * Target: 386-openbsd
 *
 * Copyright (c) 2011-2025 - Public Domain (CC0)
 */

#include "../../cgtarget.h"
#include "../../cg_os_configs.h"
#include "cg_386_arch.h"

struct cg_target target_386_openbsd = {
    "386-openbsd",
    "Intel 386 (OpenBSD)",
    &cg_386_arch,
    &cg_os_openbsd_elf32,
    &cg_386_vtable,
    NULL,
    NULL,
    NULL
};
