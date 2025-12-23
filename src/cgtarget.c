/*
 * SubC Compiler - Target Management Implementation
 *
 * This file implements the target management functions for the
 * cross-compilation framework.
 *
 * Copyright (c) 2011-2025 - Public Domain (CC0)
 */

#include <stdio.h>
#include <string.h>
#include "cgtarget.h"

/* Current active target */
struct cg_target *CG = NULL;

/*
 * External target declarations
 * Each target is defined in its architecture file:
 *   - targets/arch/i386/cg_i386.c
 *   - targets/arch/x86_64/cg_x86_64.c
 *   - targets/arch/8086/cg_8086.c
 *   - targets/arch/arm/cg_arm.c
 */

/* i386 targets */
extern struct cg_target cg_target_linux_386;
extern struct cg_target cg_target_freebsd_386;
extern struct cg_target cg_target_netbsd_386;
extern struct cg_target cg_target_openbsd_386;
extern struct cg_target cg_target_windows_386;

/* x86-64 targets */
extern struct cg_target cg_target_linux_x86_64;
extern struct cg_target cg_target_freebsd_x86_64;
extern struct cg_target cg_target_netbsd_x86_64;
extern struct cg_target cg_target_darwin_x86_64;

/* 8086 targets */
extern struct cg_target cg_target_dos_8086;
extern struct cg_target cg_target_dos_8086_x87;

/* ARM targets */
extern struct cg_target cg_target_freebsd_armv6;

/* AArch64 targets */
extern struct cg_target cg_target_linux_aarch64;
extern struct cg_target cg_target_darwin_aarch64;

/*
 * Target registry - array of all available targets
 */
static struct cg_target *targets[] = {
    /* i386 */
    &cg_target_linux_386,
    &cg_target_freebsd_386,
    &cg_target_netbsd_386,
    &cg_target_openbsd_386,
    &cg_target_windows_386,
    /* x86-64 */
    &cg_target_linux_x86_64,
    &cg_target_freebsd_x86_64,
    &cg_target_netbsd_x86_64,
    &cg_target_darwin_x86_64,
    /* 8086 */
    &cg_target_dos_8086,
    &cg_target_dos_8086_x87,
    /* ARM */
    &cg_target_freebsd_armv6,
    /* AArch64 */
    &cg_target_linux_aarch64,
    &cg_target_darwin_aarch64,
    NULL
};

/*
 * Initialize target system
 * Sets default target based on host platform
 */
void cg_init_targets(void) {
    /* Default to first available target if CG not set */
    if (CG == NULL && targets[0] != NULL) {
        CG = targets[0];
    }
}

/*
 * Find target by name
 * Returns NULL if not found
 */
struct cg_target *cg_find_target(char *name) {
    int i;
    
    if (name == NULL) {
        return NULL;
    }
    
    for (i = 0; targets[i] != NULL; i++) {
        if (strcmp(targets[i]->name, name) == 0) {
            return targets[i];
        }
    }
    
    return NULL;
}

/*
 * Set current target by name
 * Returns 0 on success, -1 on failure
 */
int cg_set_target(char *name) {
    struct cg_target *t;
    
    t = cg_find_target(name);
    if (t == NULL) {
        return -1;
    }
    
    CG = t;
    return 0;
}

/*
 * Get current target
 */
struct cg_target *cg_get_target(void) {
    return CG;
}

/*
 * List all available targets to stdout
 */
void cg_list_targets(void) {
    int i;
    
    printf("Available targets:\n");
    for (i = 0; targets[i] != NULL; i++) {
        printf("  %-20s %s\n", 
               targets[i]->name, 
               targets[i]->description);
    }
}

/*
 * Get assembler command for current target
 * Checks target-specific override first, then falls back to OS config
 */
char *cg_get_asm_cmd(void) {
    if (CG == NULL) {
        return NULL;
    }
    
    if (CG->asm_cmd_override != NULL) {
        return CG->asm_cmd_override;
    }
    
    return CG->os->asm_cmd;
}

/*
 * Get linker command for current target
 * Checks target-specific override first, then falls back to OS config
 */
char *cg_get_ld_cmd(void) {
    if (CG == NULL) {
        return NULL;
    }
    
    if (CG->ld_cmd_override != NULL) {
        return CG->ld_cmd_override;
    }
    
    return CG->os->ld_cmd;
}
