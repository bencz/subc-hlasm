/*
 * SubC Compiler - Intel 8086 Architecture Definition
 *
 * This file declares the shared architecture properties and vtable
 * for all 8086 targets (DOS).
 *
 * Copyright (c) 2011-2025 - Public Domain (CC0)
 */

#ifndef CG_8086_ARCH_H
#define CG_8086_ARCH_H

#include "../../cgtarget.h"

/*
 * Shared architecture properties for all 8086 targets.
 */
extern struct cg_arch cg_8086_arch;

/*
 * Shared vtable for all 8086 targets.
 */
extern struct cg_vtable cg_8086_vtable;

#endif /* CG_8086_ARCH_H */
