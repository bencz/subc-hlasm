/*
 * SubC Compiler - x86-64 Architecture Definition
 *
 * This file declares the shared architecture properties and vtable
 * for all x86-64 targets (Linux, FreeBSD, Darwin, etc.).
 *
 * Copyright (c) 2011-2025 - Public Domain (CC0)
 */

#ifndef CG_X86_64_ARCH_H
#define CG_X86_64_ARCH_H

#include "../../cgtarget.h"

/*
 * Shared architecture properties for all x86-64 targets.
 */
extern struct cg_arch cg_x86_64_arch;

/*
 * Shared vtable for all x86-64 targets.
 */
extern struct cg_vtable cg_x86_64_vtable;

#endif /* CG_X86_64_ARCH_H */
