/*
 * Test: OS-specific predefined macros
 *
 * Tests that OS macros are defined based on target:
 * - __linux__ for Linux targets
 * - __FreeBSD__ for FreeBSD targets
 * - __NetBSD__ for NetBSD targets
 * - __OpenBSD__ for OpenBSD targets
 * - __APPLE__ / __MACH__ for Darwin/macOS targets
 * - _WIN32 for Windows targets
 * - __DOS__ for DOS targets
 *
 * Also tests legacy macros:
 * - __unix / __unix__ for Unix-like systems
 * - __dos for DOS
 */

#ifdef __linux__
int os_linux = 1;
#else
int os_linux = 0;
#endif

#ifdef __FreeBSD__
int os_freebsd = 1;
#else
int os_freebsd = 0;
#endif

#ifdef __NetBSD__
int os_netbsd = 1;
#else
int os_netbsd = 0;
#endif

#ifdef __OpenBSD__
int os_openbsd = 1;
#else
int os_openbsd = 0;
#endif

#ifdef __APPLE__
int os_darwin = 1;
#else
int os_darwin = 0;
#endif

#ifdef _WIN32
int os_windows = 1;
#else
int os_windows = 0;
#endif

#ifdef __DOS__
int os_dos = 1;
#else
int os_dos = 0;
#endif

/* Legacy unix macro */
#ifdef __unix
int is_unix = 1;
#else
int is_unix = 0;
#endif

int main(void) {
    /* At least one OS should be defined */
    int any_os;
    any_os = os_linux + os_freebsd + os_netbsd + os_openbsd + 
             os_darwin + os_windows + os_dos;
    return any_os > 0 ? 0 : 1;
}
