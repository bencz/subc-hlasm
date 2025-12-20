/*
 * SubC Compiler - Operating System Configurations
 *
 * This file declares pre-defined OS configurations that can be shared
 * across multiple target architectures. Each OS config contains:
 *   - Object file format
 *   - Assembler and linker commands
 *   - Symbol naming conventions
 *
 * Usage:
 *   In your target file, reference the appropriate OS config:
 *
 *     extern struct cg_os_config cg_os_linux_elf32;
 *     struct cg_target my_target = { ..., &cg_os_linux_elf32, ... };
 *
 * Copyright (c) 2011-2025 - Public Domain (CC0)
 */

#ifndef CG_OS_CONFIGS_H
#define CG_OS_CONFIGS_H

#include "cgtarget.h"

/*
 * ============================================================================
 * Linux Configurations
 * ============================================================================
 */

/* Linux 32-bit (i386) - ELF format */
extern struct cg_os_config cg_os_linux_elf32;

/* Linux 64-bit (x86-64) - ELF format */
extern struct cg_os_config cg_os_linux_elf64;

/*
 * ============================================================================
 * BSD Configurations
 * ============================================================================
 */

/* FreeBSD 32-bit (i386) - ELF format */
extern struct cg_os_config cg_os_freebsd_elf32;

/* FreeBSD 64-bit (x86-64) - ELF format */
extern struct cg_os_config cg_os_freebsd_elf64;

/* NetBSD 32-bit (i386) - ELF format */
extern struct cg_os_config cg_os_netbsd_elf32;

/* NetBSD 64-bit (x86-64) - ELF format */
extern struct cg_os_config cg_os_netbsd_elf64;

/* OpenBSD 32-bit (i386) - ELF format */
extern struct cg_os_config cg_os_openbsd_elf32;

/*
 * ============================================================================
 * Darwin (macOS) Configurations
 * ============================================================================
 */

/* Darwin 64-bit (x86-64) - Mach-O format */
extern struct cg_os_config cg_os_darwin_macho64;

/*
 * ============================================================================
 * Windows Configurations
 * ============================================================================
 */

/* Windows 32-bit (i386) - PE/COFF format */
extern struct cg_os_config cg_os_windows_pe32;

/*
 * ============================================================================
 * DOS Configurations
 * ============================================================================
 */

/* DOS 16-bit (8086) - OMF format */
extern struct cg_os_config cg_os_dos_omf16;

#endif /* CG_OS_CONFIGS_H */
