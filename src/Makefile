#
# SubC Cross-Compiler - Source Makefile
#
# This builds the cross-compiler that supports all targets in a single binary.
# No configure script needed - just run 'make scc0'.
#
# Copyright (c) 2011-2025 - Public Domain (CC0)
#

# Installation paths (optional - for 'make install')
PREFIX=		/usr/local
BINDIR=		$(PREFIX)/bin
SCCDIR=		$(PREFIX)/lib/scc

# Compiler flags
CC=		cc
CFLAGS=		-O2 -I.
LINT=		gcc -O3 -c -o /dev/null -ansi -pedantic -Wall

#
# Source files
#

# Code generator sources (all architectures compiled together)
CG_ARCH=	targets/arch/i386/cg_i386.c \
		targets/arch/x86_64/cg_x86_64.c \
		targets/arch/8086/cg_8086.c \
		targets/arch/arm/cg_arm.c \
		targets/arch/aarch64/cg_aarch64.c

# Target management and OS configs
CG_SRC=		cgtarget.c targets/os/cg_os_configs.c $(CG_ARCH)

# Main compiler sources
SRC=		decl.c error.c expr.c gen.c main.c misc.c \
		opt.c prep.c scan.c stmt.c sym.c tree.c $(CG_SRC)

# Header files
HDRS=		cgtarget.h cgen_compat.h cgen.h data.h decl.h defs.h prec.h \
		types.h tokens.h ops.h

#
# Main targets
#

# Default: build the cross-compiler
all: scc0

# Build the cross-compiler using host CC
scc0: $(SRC) $(HDRS)
	$(CC) $(CFLAGS) -o scc0 $(SRC)
	@echo ""
	@echo "Cross-compiler built successfully!"
	@echo "Run './scc0 -L' to list available targets."

# List available targets
list: scc0
	./scc0 -L

#
# Lint/static analysis
#
lint:
	@for f in $(SRC); do \
		echo "Checking $$f..."; \
		$(LINT) $$f; \
	done

#
# Installation
#
install: scc0
	@mkdir -p $(BINDIR)
	@mkdir -p $(SCCDIR)/include
	cp scc0 $(BINDIR)/scc
	@echo "Installed scc to $(BINDIR)/scc"

#
# Clean
#
clean:
	rm -f scc0 scc1 scc scc86 *.o *.s *.core core a.out *.tmp *.exe
