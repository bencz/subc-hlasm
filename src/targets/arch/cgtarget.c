/*
 * SubC Compiler - Target Management Implementation
 *
 * This file implements target registration, selection, and management.
 * It uses the new composable target structure with separate arch, os, and vtable.
 *
 * Copyright (c) 2011-2025 - Public Domain (CC0)
 */

#include <stdio.h>
#include <string.h>
#include "cgtarget.h"

/*
 * ============================================================================
 * External Target Declarations
 * ============================================================================
 *
 * Each target is defined in its own file and declared here.
 */

/* i386 targets */
extern struct cg_target target_386;
extern struct cg_target target_386_linux;
extern struct cg_target target_386_freebsd;
extern struct cg_target target_386_netbsd;
extern struct cg_target target_386_openbsd;
extern struct cg_target target_386_windows;

/* x86-64 targets */
extern struct cg_target target_x86_64_alias;
extern struct cg_target target_x86_64_linux;
extern struct cg_target target_x86_64_freebsd;
extern struct cg_target target_x86_64_netbsd;
extern struct cg_target target_x86_64_darwin;

/* 8086 targets */
extern struct cg_target target_8086_alias;
extern struct cg_target target_8086_dos;

/* ARMv6 targets */
extern struct cg_target target_armv6_alias;
extern struct cg_target target_armv6_linux;

/* ARM64 targets */
extern struct cg_target target_arm64_alias;
extern struct cg_target target_arm64_linux;
extern struct cg_target target_aarch64_linux;
extern struct cg_target target_arm64_darwin;
extern struct cg_target target_aarch64_darwin;
extern struct cg_target target_arm64_macos;

/* S/370 targets */
extern struct cg_target cg_s370_mvs_target;
extern struct cg_target cg_s370xa_mvs_target;

/*
 * ============================================================================
 * Target Registry
 * ============================================================================
 */

#define MAX_TARGETS 48

static struct cg_target *targets[MAX_TARGETS];
static int num_targets = 0;

/* Global pointer to current target */
struct cg_target *CG = NULL;

/*
 * register_target - Add a target to the registry
 */
static void register_target(struct cg_target *t) {
    if (num_targets < MAX_TARGETS) {
        targets[num_targets++] = t;
    }
}

/*
 * cg_init_targets - Initialize the target subsystem
 *
 * Registers all built-in targets. Must be called before any other
 * target functions.
 */
void cg_init_targets(void) {
    num_targets = 0;
    
    /* i386 targets */
    register_target(&target_386);
    register_target(&target_386_linux);
    register_target(&target_386_freebsd);
    register_target(&target_386_netbsd);
    register_target(&target_386_openbsd);
    register_target(&target_386_windows);
    
    /* x86-64 targets */
    register_target(&target_x86_64_alias);
    register_target(&target_x86_64_linux);
    register_target(&target_x86_64_freebsd);
    register_target(&target_x86_64_netbsd);
    register_target(&target_x86_64_darwin);
    
    /* 8086 targets */
    register_target(&target_8086_alias);
    register_target(&target_8086_dos);
    
    /* ARMv6 targets */
    register_target(&target_armv6_alias);
    register_target(&target_armv6_linux);
    
    /* ARM64 targets */
    register_target(&target_arm64_alias);
    register_target(&target_arm64_linux);
    register_target(&target_aarch64_linux);
    register_target(&target_arm64_darwin);
    register_target(&target_aarch64_darwin);
    register_target(&target_arm64_macos);
    
    /* S/370 targets */
    register_target(&cg_s370_mvs_target);
    register_target(&cg_s370xa_mvs_target);
}

/*
 * cg_find_target - Find a target by name without selecting it
 */
struct cg_target *cg_find_target(char *name) {
    int i;
    
    for (i = 0; i < num_targets; i++) {
        if (strcmp(targets[i]->name, name) == 0) {
            return targets[i];
        }
    }
    return NULL;
}

/*
 * cg_set_target - Select a target by name
 *
 * Returns: 1 on success, 0 if target not found
 */
int cg_set_target(char *name) {
    struct cg_target *t;
    
    t = cg_find_target(name);
    if (t != NULL) {
        CG = t;
        return 1;
    }
    return 0;
}

/*
 * cg_get_target - Get the current target
 */
struct cg_target *cg_get_target(void) {
    return CG;
}

/*
 * cg_list_targets - Print list of available targets
 */
void cg_list_targets(void) {
    int i;
    
    printf("Available targets:\n");
    for (i = 0; i < num_targets; i++) {
        printf("  %-16s  %s\n", targets[i]->name, targets[i]->description);
    }
}

/*
 * ============================================================================
 * Helper Functions for Accessing Target Properties
 * ============================================================================
 */

/*
 * cg_get_asm_cmd - Get assembler command (checks override first)
 */
char *cg_get_asm_cmd(void) {
    if (CG->asm_cmd_override != NULL) {
        return CG->asm_cmd_override;
    }
    return CG->os->asm_cmd;
}

/*
 * cg_get_ld_cmd - Get linker command (checks override first)
 */
char *cg_get_ld_cmd(void) {
    if (CG->ld_cmd_override != NULL) {
        return CG->ld_cmd_override;
    }
    return CG->os->ld_cmd;
}
