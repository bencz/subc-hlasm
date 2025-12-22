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

struct cg_os_config cg_os_linux = {
    OS_LINUX,           /* os_type */
    OBJ_ELF,            /* obj_format */
    0,                  /* underscore_sym */
    "Linux",            /* os_name */
    "as -o %s %s",      /* asm_cmd */
    "ld -o %s -dynamic-linker /lib64/ld-linux-x86-64.so.2 " 
        "/usr/lib/crt1.o /usr/lib/crti.o -lc /usr/lib/crtn.o",  /* ld_cmd */
    "-lc",              /* sys_libc */
    "a.out"             /* aout_name */
};

/*
 * ============================================================================
 * FreeBSD Configurations
 * ============================================================================
 */

struct cg_os_config cg_os_freebsd = {
    OS_FREEBSD,         /* os_type */
    OBJ_ELF,            /* obj_format */
    0,                  /* underscore_sym */
    "FreeBSD",          /* os_name */
    "as -o %s %s",      /* asm_cmd */
    "ld -o %s -dynamic-linker /libexec/ld-elf.so.1 "
        "/usr/lib/crt1.o /usr/lib/crti.o -lc /usr/lib/crtn.o",  /* ld_cmd */
    "-lc",              /* sys_libc */
    "a.out"             /* aout_name */
};

/*
 * ============================================================================
 * NetBSD Configurations
 * ============================================================================
 */

struct cg_os_config cg_os_netbsd = {
    OS_NETBSD,          /* os_type */
    OBJ_ELF,            /* obj_format */
    0,                  /* underscore_sym */
    "NetBSD",           /* os_name */
    "as -o %s %s",      /* asm_cmd */
    "ld -o %s -dynamic-linker /libexec/ld.elf_so "
        "/usr/lib/crt0.o /usr/lib/crti.o -lc /usr/lib/crtn.o",  /* ld_cmd */
    "-lc",              /* sys_libc */
    "a.out"             /* aout_name */
};

/*
 * ============================================================================
 * OpenBSD Configurations
 * ============================================================================
 */

struct cg_os_config cg_os_openbsd = {
    OS_OPENBSD,         /* os_type */
    OBJ_ELF,            /* obj_format */
    0,                  /* underscore_sym */
    "OpenBSD",          /* os_name */
    "as -o %s %s",      /* asm_cmd */
    "ld -o %s -dynamic-linker /usr/libexec/ld.so "
        "/usr/lib/crt0.o -lc",  /* ld_cmd */
    "-lc",              /* sys_libc */
    "a.out"             /* aout_name */
};

/*
 * ============================================================================
 * Darwin (macOS) Configurations
 * ============================================================================
 */

struct cg_os_config cg_os_darwin = {
    OS_DARWIN,          /* os_type */
    OBJ_MACHO,          /* obj_format */
    1,                  /* underscore_sym */
    "Darwin",           /* os_name */
    "as -o %s %s",      /* asm_cmd */
    "ld -o %s -lSystem",  /* ld_cmd */
    "-lSystem",         /* sys_libc */
    "a.out"             /* aout_name */
};

/*
 * ============================================================================
 * Windows (MinGW) Configurations
 * ============================================================================
 */

struct cg_os_config cg_os_windows = {
    OS_WINDOWS,         /* os_type */
    OBJ_PE,             /* obj_format */
    1,                  /* underscore_sym */
    "Windows",          /* os_name */
    "as -o %s %s",      /* asm_cmd */
    "ld -o %s -lmsvcrt",  /* ld_cmd */
    "-lmsvcrt",         /* sys_libc */
    "a.exe"             /* aout_name */
};

/*
 * ============================================================================
 * DOS Configurations
 * ============================================================================
 */

struct cg_os_config cg_os_dos = {
    OS_DOS,             /* os_type */
    OBJ_OMF,            /* obj_format */
    0,                  /* underscore_sym */
    "DOS",              /* os_name */
    "s86 -o %s %s",     /* asm_cmd */
    "sld -o %s",        /* ld_cmd */
    "",                 /* sys_libc */
    "a.exe"             /* aout_name */
};

/*
 * ============================================================================
 * MVS (IBM Mainframe) Configurations
 * ============================================================================
 */

struct cg_os_config cg_os_mvs = {
    OS_MVS,             /* os_type */
    OBJ_GOFF,           /* obj_format */
    0,                  /* underscore_sym */
    "MVS",              /* os_name */
    NULL,               /* asm_cmd - HLASM handled differently */
    NULL,               /* ld_cmd - Binder handled differently */
    "",                 /* sys_libc */
    "MODULE"            /* aout_name */
};
