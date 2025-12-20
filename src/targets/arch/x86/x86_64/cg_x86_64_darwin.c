/*
 * SubC Compiler - x86-64 Darwin (macOS) Target
 *
 * Target: x86-64-darwin
 *
 * Copyright (c) 2011-2025 - Public Domain (CC0)
 */

#include "../../cgtarget.h"
#include "../../cg_os_configs.h"
#include "cg_x86_64_arch.h"

struct cg_target target_x86_64_darwin = {
    "x86-64-darwin",
    "x86-64 / AMD64 (Darwin/macOS)",
    &cg_x86_64_arch,
    &cg_os_darwin_macho64,
    &cg_x86_64_vtable,
    NULL,
    NULL,
    NULL
};
