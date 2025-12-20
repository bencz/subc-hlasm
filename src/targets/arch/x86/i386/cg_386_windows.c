/*
 * SubC Compiler - Intel 386 Windows Target
 *
 * Target: 386-windows
 *
 * Copyright (c) 2011-2025 - Public Domain (CC0)
 */

#include "../../cgtarget.h"
#include "../../cg_os_configs.h"
#include "cg_386_arch.h"

struct cg_target target_386_windows = {
    "386-windows",
    "Intel 386 (Windows/MinGW)",
    &cg_386_arch,
    &cg_os_windows_pe32,
    &cg_386_vtable,
    NULL,
    NULL,
    NULL
};
