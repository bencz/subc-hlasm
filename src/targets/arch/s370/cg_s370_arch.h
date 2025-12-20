/*
 * SubC Compiler - IBM System/370 Architecture Definition Header
 *
 * This file declares the shared architecture properties and vtable
 * for all S/370 targets.
 *
 * Architecture characteristics:
 *   - 24-bit addressing (can address up to 16MB)
 *   - 32-bit registers (but only 24 bits for addresses)
 *   - Big-endian byte order
 *   - Stack grows upward (toward higher addresses)
 *   - IBM Hexadecimal floating point format
 *   - HLASM (High Level Assembler) syntax
 *
 * Copyright (c) 2025 - Public Domain (CC0)
 */

#ifndef CG_S370_ARCH_H
#define CG_S370_ARCH_H

#include "../../cgtarget.h"

/* Architecture properties - shared by all S/370 targets */
extern struct cg_arch cg_s370_arch;

/* Code generation vtable - shared by all S/370 targets */
extern struct cg_vtable cg_s370_vtable;

#endif /* CG_S370_ARCH_H */
