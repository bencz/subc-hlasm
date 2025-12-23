/*
 * Test: Architecture-specific predefined macros
 *
 * Tests that architecture macros are defined based on target:
 * - __i386__ for i386 targets
 * - __x86_64__ for x86-64 targets  
 * - __aarch64__ for AArch64 targets
 * - __arm__ for ARM targets
 * - __8086__ for DOS 8086 targets
 *
 * At least one architecture macro should be defined.
 */

int arch_bits = 0;

#ifdef __i386__
int arch_i386 = 1;
#else
int arch_i386 = 0;
#endif

#ifdef __x86_64__
int arch_x86_64 = 1;
#else
int arch_x86_64 = 0;
#endif

#ifdef __aarch64__
int arch_aarch64 = 1;
#else
int arch_aarch64 = 0;
#endif

#ifdef __arm__
int arch_arm = 1;
#else
int arch_arm = 0;
#endif

#ifdef __8086__
int arch_8086 = 1;
#else
int arch_8086 = 0;
#endif

int main(void) {
    /* At least one arch should be defined */
    int any_arch;
    any_arch = arch_i386 + arch_x86_64 + arch_aarch64 + arch_arm + arch_8086;
    return any_arch > 0 ? 0 : 1;
}
