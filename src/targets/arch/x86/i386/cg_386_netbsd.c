/*
 * SubC Compiler - Intel 386 NetBSD Target
 *
 * Target: 386-netbsd
 *
 * Copyright (c) 2011-2025 - Public Domain (CC0)
 */

#include "../../cgtarget.h"
#include "../../cg_os_configs.h"
#include "cg_386_arch.h"

struct cg_target target_386_netbsd = {
    "386-netbsd",
    "Intel 386 (NetBSD)",
    &cg_386_arch,
    &cg_os_netbsd_elf32,
    &cg_386_vtable,
    NULL,
    NULL,
    NULL
};
