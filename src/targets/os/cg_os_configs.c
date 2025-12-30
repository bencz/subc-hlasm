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
 * Predefined Macros by Operating System
 * ============================================================================
 */

static struct cg_predef_macro predef_linux[] = {
    { "__linux__", "1" },
    { "__linux", "1" },
    { "__unix__", "1" },
    { "__unix", "1" },
    { "__ELF__", "1" },
    { NULL, NULL }
};

static struct cg_predef_macro predef_freebsd[] = {
    { "__FreeBSD__", "1" },
    { "__unix__", "1" },
    { "__unix", "1" },
    { "__ELF__", "1" },
    { NULL, NULL }
};

static struct cg_predef_macro predef_netbsd[] = {
    { "__NetBSD__", "1" },
    { "__unix__", "1" },
    { "__unix", "1" },
    { "__ELF__", "1" },
    { NULL, NULL }
};

static struct cg_predef_macro predef_openbsd[] = {
    { "__OpenBSD__", "1" },
    { "__unix__", "1" },
    { "__unix", "1" },
    { "__ELF__", "1" },
    { NULL, NULL }
};

static struct cg_predef_macro predef_darwin[] = {
    { "__APPLE__", "1" },
    { "__MACH__", "1" },
    { "__unix__", "1" },
    { "__unix", "1" },
    { NULL, NULL }
};

static struct cg_predef_macro predef_windows[] = {
    { "_WIN32", "1" },
    { "__WIN32__", "1" },
    { "__WINDOWS__", "1" },
    { NULL, NULL }
};

static struct cg_predef_macro predef_dos[] = {
    { "__DOS__", "1" },
    { "__dos", "1" },
    { "MSDOS", "1" },
    { NULL, NULL }
};

static struct cg_predef_macro predef_mvs[] = {
    { "__MVS__", "1" },
    { "__IBM__", "1" },
    { "__EBCDIC__", "1" },
    { NULL, NULL }
};

/*
 * ============================================================================
 * Linux Configurations
 * ============================================================================
 */

/* Linux i386 (32-bit) */
struct cg_os_config cg_os_linux = {
    OS_LINUX,           /* os_type */
    OBJ_ELF,            /* obj_format */
    0,                  /* underscore_sym */
    "Linux",            /* os_name */
    "as --32 -o %s %s", /* asm_cmd */
    "ld -o %s -m elf_i386 -dynamic-linker /lib/ld-linux.so.2 "
        "/usr/lib/crt1.o /usr/lib/crti.o -lc /usr/lib/crtn.o",  /* ld_cmd */
    "-lc",              /* sys_libc */
    NULL,               /* scc_libc - not needed, uses system libc */
    "a.out",            /* aout_name */
    predef_linux        /* predef_macros */
};

/* Linux x86-64 (64-bit) */
struct cg_os_config cg_os_linux_64 = {
    OS_LINUX,           /* os_type */
    OBJ_ELF,            /* obj_format */
    0,                  /* underscore_sym */
    "Linux",            /* os_name */
    "as --64 -o %s %s", /* asm_cmd */
    "ld -o %s -m elf_x86_64 -dynamic-linker /lib64/ld-linux-x86-64.so.2 "
        "/usr/lib64/crt1.o /usr/lib64/crti.o -lc /usr/lib64/crtn.o",  /* ld_cmd */
    "-lc",              /* sys_libc */
    NULL,               /* scc_libc - not needed, uses system libc */
    "a.out",            /* aout_name */
    predef_linux        /* predef_macros */
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
    NULL,               /* scc_libc - not needed, uses system libc */
    "a.out",            /* aout_name */
    predef_freebsd      /* predef_macros */
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
    NULL,               /* scc_libc - not needed, uses system libc */
    "a.out",            /* aout_name */
    predef_netbsd       /* predef_macros */
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
    NULL,               /* scc_libc - not needed, uses system libc */
    "a.out",            /* aout_name */
    predef_openbsd      /* predef_macros */
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
    "ld -o %s -L /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/lib",  /* ld_cmd */
    "-lSystem",         /* sys_libc */
    NULL,               /* scc_libc - not needed, uses system libc */
    "a.out",            /* aout_name */
    predef_darwin       /* predef_macros */
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
    NULL,               /* scc_libc - not needed, uses system libc */
    "a.exe",            /* aout_name */
    predef_windows      /* predef_macros */
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
    "%s/lib/libscc.a", /* scc_libc - DOS needs SubC runtime library */
    "a.exe",            /* aout_name */
    predef_dos          /* predef_macros */
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
    NULL,               /* scc_libc - not needed */
    "MODULE",           /* aout_name */
    predef_mvs          /* predef_macros */
};
