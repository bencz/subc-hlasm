#
# SubC Compiler - Cross-Compilation Build System
#
# This compiler supports multiple targets from a single binary.
# Use 'scc -L' to list available targets.
# Use 'scc -T <target>' to select a target.
#

VERSION=	2025-cross
ARC=		subc-$(VERSION).tgz

# Default target: build the cross-compiler
default: all

all:
	@echo "Building SubC cross-compiler..."
	cd src && $(MAKE) scc0
	@echo ""
	@echo "Build complete! The compiler is at src/scc0"
	@echo "Use './src/scc0 -L' to list available targets."
	@echo "Use './src/scc0 -T <target> -S file.c' to compile."

# Build the s86 assembler/linker tools (for DOS 8086 target)
s86:
	cd s86 && $(MAKE)

# Build everything including s86 tools
full: all s86
	@echo "Full build complete (compiler + s86 tools)"

# Run tests (requires native runtime - see below)
test:
	cd src && $(MAKE) test

# Clean all build artifacts
clean:
	cd src && $(MAKE) clean
	cd s86 && $(MAKE) clean
	rm -f tests/ptest.c $(ARC)
	rm -f tests/ptest tests/systest tests/libtest

# Create distribution archive
dist: clean
	tar cvfz $(ARC) \
		Makefile README Changes BUGS Todo FAQ Status Porting \
		CC0 NOTES-DOS NOTES-WINDOWS \
		src/ runtime/ s86/ tests/

# Show help
help:
	@echo "SubC Cross-Compiler Build System"
	@echo ""
	@echo "Targets:"
	@echo "  all      - Build the cross-compiler (default)"
	@echo "  s86      - Build s86 assembler/linker for DOS"
	@echo "  full     - Build compiler + s86 tools"
	@echo "  clean    - Remove all build artifacts"
	@echo "  dist     - Create distribution archive"
	@echo "  help     - Show this help"
	@echo ""
	@echo "After building, use:"
	@echo "  ./src/scc0 -L              List available targets"
	@echo "  ./src/scc0 -T dos-8086 -S file.c   Compile for DOS"
	@echo "  ./src/scc0 -T linux-x86-64 -S file.c   Compile for Linux"

.PHONY: default all s86 full test clean dist help
