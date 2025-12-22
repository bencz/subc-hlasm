/*
 * SubC Compiler - ARM64 (AArch64) Architecture Definition Header
 *
 * This file declares the shared architecture properties and vtable
 * for all ARM64 targets.
 *
 * Architecture characteristics:
 *   - 64-bit registers and pointers
 *   - Little-endian byte order (default, can be big-endian)
 *   - Stack grows downward (toward lower addresses)
 *   - IEEE 754 floating point
 *   - AAPCS64 calling convention
 *
 * Copyright (c) 2025 - Public Domain (CC0)
 */

#ifndef CG_ARM64_ARCH_H
#define CG_ARM64_ARCH_H

#include "../../cgtarget.h"

/* Architecture properties - shared by all ARM64 targets */
extern struct cg_arch cg_arm64_arch;

/* Code generation vtable - shared by all ARM64 targets */
extern struct cg_vtable cg_arm64_vtable;

#endif /* CG_ARM64_ARCH_H */
