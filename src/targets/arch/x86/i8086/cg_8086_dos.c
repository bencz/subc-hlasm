/*
 * SubC Compiler - Intel 8086 DOS Target
 *
 * Target: 8086-dos (or just "8086")
 *
 * Copyright (c) 2011-2025 - Public Domain (CC0)
 */

#include "../../cgtarget.h"
#include "../../cg_os_configs.h"
#include "cg_8086_arch.h"

struct cg_target target_8086_dos = {
    "8086-dos",
    "Intel 8086 (DOS)",
    &cg_8086_arch,
    &cg_os_dos_omf16,
    &cg_8086_vtable,
    NULL,
    NULL,
    NULL
};

/* Backward compatibility alias */
struct cg_target target_8086_alias = {
    "8086",
    "Intel 8086 (16-bit DOS)",
    &cg_8086_arch,
    &cg_os_dos_omf16,
    &cg_8086_vtable,
    NULL,
    NULL,
    NULL
};
