/*
 * SubC Compiler - Operating System Configurations
 *
 * This file defines pre-defined OS configurations that can be shared
 * across multiple target architectures.
 *
 * Copyright (c) 2011-2025 - Public Domain (CC0)
 */

#include "cgtarget.h"

/*
 * ============================================================================
 * Linux Configurations
 * ============================================================================
 */

struct cg_os_config cg_os_linux_elf32 = {
    OS_LINUX,
    OBJ_ELF,
    0,                              /* no underscore prefix */
    "Linux",
    "as --32 -o %s %s",
    "ld -m elf_i386 -o %s %s/lib/%scrt0.o",
    "gcc -m32 -o %s",               /* ld_cmd_sysrt: use gcc as driver */
    "",
    "a.out"
};

struct cg_os_config cg_os_linux_elf64 = {
    OS_LINUX,
    OBJ_ELF,
    0,
    "Linux",
    "as -o %s %s",
    "ld -o %s %s/lib/%scrt0.o",
    "gcc -o %s",                    /* ld_cmd_sysrt */
    "",
    "a.out"
};

/*
 * ============================================================================
 * BSD Configurations
 * ============================================================================
 */

struct cg_os_config cg_os_freebsd_elf32 = {
    OS_FREEBSD,
    OBJ_ELF,
    0,
    "FreeBSD",
    "as --32 -o %s %s",
    "ld -m elf_i386_fbsd -o %s %s/lib/%scrt0.o",
    "cc -m32 -o %s",                /* ld_cmd_sysrt */
    "",
    "a.out"
};

struct cg_os_config cg_os_freebsd_elf64 = {
    OS_FREEBSD,
    OBJ_ELF,
    0,
    "FreeBSD",
    "as -o %s %s",
    "ld -o %s %s/lib/%scrt0.o",
    "cc -o %s",                     /* ld_cmd_sysrt */
    "",
    "a.out"
};

struct cg_os_config cg_os_netbsd_elf32 = {
    OS_NETBSD,
    OBJ_ELF,
    0,
    "NetBSD",
    "as --32 -o %s %s",
    "ld -m elf_i386 -o %s %s/lib/%scrt0.o",
    "cc -m32 -o %s",                /* ld_cmd_sysrt */
    "",
    "a.out"
};

struct cg_os_config cg_os_netbsd_elf64 = {
    OS_NETBSD,
    OBJ_ELF,
    0,
    "NetBSD",
    "as -o %s %s",
    "ld -o %s %s/lib/%scrt0.o",
    "cc -o %s",                     /* ld_cmd_sysrt */
    "",
    "a.out"
};

struct cg_os_config cg_os_openbsd_elf32 = {
    OS_OPENBSD,
    OBJ_ELF,
    0,
    "OpenBSD",
    "as --32 -o %s %s",
    "ld -m elf_i386 -o %s %s/lib/%scrt0.o",
    "cc -m32 -o %s",                /* ld_cmd_sysrt */
    "-lc",
    "a.out"
};

/*
 * ============================================================================
 * Darwin (macOS) Configurations
 * ============================================================================
 */

struct cg_os_config cg_os_darwin_macho64 = {
    OS_DARWIN,
    OBJ_MACHO,
    1,                              /* underscore prefix for symbols */
    "Darwin",
    "as -o %s %s",
    "ld -o %s %s/lib/%scrt0.o -lSystem",
    "clang -o %s",                  /* ld_cmd_sysrt */
    "-lSystem",
    "a.out"
};

/*
 * ============================================================================
 * Windows Configurations
 * ============================================================================
 */

struct cg_os_config cg_os_windows_pe32 = {
    OS_WINDOWS,
    OBJ_PE,
    1,                              /* underscore prefix for symbols */
    "Windows",
    "as -o %s %s",
    "ld -o %s %s/lib/%scrt0.o",
    "gcc -o %s",                    /* ld_cmd_sysrt */
    "-lkernel32 -lmsvcrt",
    "a.exe"
};

/*
 * ============================================================================
 * DOS Configurations
 * ============================================================================
 */

struct cg_os_config cg_os_dos_omf16 = {
    OS_DOS,
    OBJ_OMF,
    0,
    "DOS",
    "s86 -o %s %s",
    "sld -o %s %s/lib/%scrt0.o",
    NULL,                           /* ld_cmd_sysrt: not supported on DOS */
    "",
    "aout.exe"
};
