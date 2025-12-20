/*
 * SubC Compiler - Intel 386 Architecture Definition
 *
 * This file declares the shared architecture properties and vtable
 * for all i386 targets (Linux, FreeBSD, Windows, etc.).
 *
 * The actual code generation functions are in cg_386.c.
 * This file provides the shared cg_386_arch and cg_386_vtable.
 *
 * Copyright (c) 2011-2025 - Public Domain (CC0)
 */

#ifndef CG_386_ARCH_H
#define CG_386_ARCH_H

#include "../../cgtarget.h"

/*
 * Shared architecture properties for all i386 targets.
 * Contains CPU-specific settings like word size, endianness, registers.
 */
extern struct cg_arch cg_386_arch;

/*
 * Shared vtable for all i386 targets.
 * Contains pointers to all code generation functions.
 */
extern struct cg_vtable cg_386_vtable;

#endif /* CG_386_ARCH_H */
