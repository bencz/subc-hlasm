PREFIX=	/u
EXESFX=	
SCCDIR=	$(PREFIX)/scc
BINDIR=	$(PREFIX)/bin
SC86DIR=$(PREFIX)/scc86

LINT=	gcc -std=c89 -O3 -c -o /dev/null -ansi -pedantic -Wall

# Include paths for headers
INCLUDES=	-Iinclude -Itargets/arch \
		-Itargets/arch/x86/i386 -Itargets/arch/x86/x86_64 \
		-Itargets/arch/x86/i8086 -Itargets/arch/arm/armv6 \
		-Itargets/arch/arm/arm64 -Itargets/arch/s370

# Modular structure sources
# Frontend - Lexer
SRC_LEXER=	frontend/lexer/scan.c

# Frontend - Preprocessor
SRC_PREP=	frontend/preprocessor/prep.c

# Frontend - Parser
SRC_PARSER=	frontend/parser/decl.c \
		frontend/parser/expr.c \
		frontend/parser/stmt.c

# AST
SRC_AST=	ast/tree.c

# Optimizer
SRC_OPT=	optimizer/opt.c

# Code Generator
SRC_CODEGEN=	codegen/gen.c

# Symbols
SRC_SYM=	symbols/sym.c

# Common utilities
SRC_COMMON=	common/error.c \
		common/misc.c

# All compiler sources
SRC=	main.c \
		$(SRC_LEXER) $(SRC_PREP) $(SRC_PARSER) \
		$(SRC_AST) $(SRC_OPT) $(SRC_CODEGEN) \
		$(SRC_SYM) $(SRC_COMMON)

# Modular headers
HDRS_MODULAR=	frontend/lexer/tokens.h frontend/lexer/scan.h \
		frontend/preprocessor/prep.h \
		frontend/parser/decl.h frontend/parser/expr.h \
		frontend/parser/stmt.h frontend/parser/prec.h \
		ast/ops.h ast/tree.h \
		optimizer/opt.h \
		codegen/gen.h \
		symbols/types.h symbols/sym.h \
		common/error.h common/misc.h

# Cross-compiler target architecture sources
# Core framework
TGTCORE= targets/arch/cgtarget.c \
	targets/arch/cg_os_configs.c

# i386 targets
TGT386=	targets/arch/x86/i386/cg_386.c \
	targets/arch/x86/i386/cg_386_arch.c \
	targets/arch/x86/i386/cg_386_linux.c \
	targets/arch/x86/i386/cg_386_freebsd.c \
	targets/arch/x86/i386/cg_386_netbsd.c \
	targets/arch/x86/i386/cg_386_openbsd.c \
	targets/arch/x86/i386/cg_386_windows.c

# x86-64 targets
TGTX64=	targets/arch/x86/x86_64/cg_x86_64.c \
	targets/arch/x86/x86_64/cg_x86_64_arch.c \
	targets/arch/x86/x86_64/cg_x86_64_linux.c \
	targets/arch/x86/x86_64/cg_x86_64_freebsd.c \
	targets/arch/x86/x86_64/cg_x86_64_netbsd.c \
	targets/arch/x86/x86_64/cg_x86_64_darwin.c

# 8086 targets
TGT86=	targets/arch/x86/i8086/cg_8086.c \
	targets/arch/x86/i8086/cg_8086_arch.c \
	targets/arch/x86/i8086/cg_8086_dos.c

# ARMv6 targets
TGTARMV6= targets/arch/arm/armv6/cg_armv6.c \
	targets/arch/arm/armv6/cg_armv6_arch.c \
	targets/arch/arm/armv6/cg_armv6_linux.c

# ARM64 targets
TGTARM64= targets/arch/arm/arm64/cg_arm64.c \
	targets/arch/arm/arm64/cg_arm64_arch.c \
	targets/arch/arm/arm64/cg_arm64_linux.c \
	targets/arch/arm/arm64/cg_arm64_darwin.c

# S/370 targets
TGTS370= targets/arch/s370/cg_s370.c \
	targets/arch/s370/cg_s370_arch.c \
	targets/arch/s370/cg_s370_mvs.c \
	targets/arch/s370/cg_s370xa_arch.c \
	targets/arch/s370/cg_s370xa_mvs.c

# All target sources
TGTSRC=	$(TGTCORE) $(TGT386) $(TGTX64) $(TGT86) $(TGTARMV6) $(TGTARM64) $(TGTS370)

# All sources (compiler + targets)
ALLSRC=	$(SRC) $(TGTSRC)

HDRS=	include/data.h include/decl.h include/defs.h \
	include/prec.h include/types.h include/tokens.h include/ops.h \
	targets/arch/cgtarget.h targets/arch/cgen_compat.h

# Runtime library directory (outside src/)
RUNTIME=	../runtime
LIBDIR=		$(RUNTIME)/lib
INCDIR=		$(RUNTIME)/include

LIBOBJS=$(LIBDIR)/init.o $(LIBDIR)/abort.o $(LIBDIR)/abs.o $(LIBDIR)/atexit.o $(LIBDIR)/atoi.o \
	$(LIBDIR)/bsearch.o $(LIBDIR)/calloc.o $(LIBDIR)/clearerr.o $(LIBDIR)/ctime.o \
	$(LIBDIR)/ctype.o $(LIBDIR)/difftime.o $(LIBDIR)/exit.o $(LIBDIR)/fclose.o \
	$(LIBDIR)/fdopen.o $(LIBDIR)/feof.o $(LIBDIR)/ferror.o $(LIBDIR)/fflush.o $(LIBDIR)/fgetc.o \
	$(LIBDIR)/fgetpos.o $(LIBDIR)/fgets.o $(LIBDIR)/fileno.o $(LIBDIR)/fopen.o \
	$(LIBDIR)/fprintf.o $(LIBDIR)/fputc.o $(LIBDIR)/fputs.o $(LIBDIR)/fread.o $(LIBDIR)/free.o \
	$(LIBDIR)/freopen.o $(LIBDIR)/fscanf.o $(LIBDIR)/fseek.o $(LIBDIR)/fsetpos.o \
	$(LIBDIR)/ftell.o $(LIBDIR)/fwrite.o $(LIBDIR)/getchar.o $(LIBDIR)/getenv.o \
	$(LIBDIR)/kprintf.o $(LIBDIR)/malloc.o $(LIBDIR)/memchr.o $(LIBDIR)/memcmp.o \
	$(LIBDIR)/memcpy.o $(LIBDIR)/memmove.o $(LIBDIR)/memset.o $(LIBDIR)/perror.o \
	$(LIBDIR)/printf.o $(LIBDIR)/putchar.o $(LIBDIR)/puts.o $(LIBDIR)/qsort.o $(LIBDIR)/rand.o \
	$(LIBDIR)/realloc.o $(LIBDIR)/remove.o $(LIBDIR)/rename.o $(LIBDIR)/rewind.o \
	$(LIBDIR)/scanf.o $(LIBDIR)/setbuf.o $(LIBDIR)/setvbuf.o $(LIBDIR)/sprintf.o \
	$(LIBDIR)/sscanf.o $(LIBDIR)/strcat.o $(LIBDIR)/strchr.o $(LIBDIR)/strcmp.o \
	$(LIBDIR)/strcpy.o $(LIBDIR)/strcspn.o $(LIBDIR)/strdup.o $(LIBDIR)/strerror.o \
	$(LIBDIR)/strlen.o $(LIBDIR)/strncat.o $(LIBDIR)/strncmp.o $(LIBDIR)/strncpy.o \
	$(LIBDIR)/strpbrk.o $(LIBDIR)/strrchr.o $(LIBDIR)/strspn.o $(LIBDIR)/strtok.o \
	$(LIBDIR)/strtol.o $(LIBDIR)/system.o $(LIBDIR)/time.o $(LIBDIR)/tmpfile.o \
	$(LIBDIR)/tmpnam.o $(LIBDIR)/ungetc.o $(LIBDIR)/varargs.o $(LIBDIR)/vformat.o \
	$(LIBDIR)/vfprintf.o $(LIBDIR)/vprintf.o $(LIBDIR)/vscan.o $(LIBDIR)/vsprintf.o

AS=	as
AR=	ar -rc
RANLIB=	ranlib

all:	scc0 crt0 ncrt0 lib

scc:	scc1 $(ALLSRC)
	./scc1 -o scc $(ALLSRC)

scc1:	scc0 $(LIBDIR)/crt0.o $(LIBDIR)/libscc.a $(ALLSRC)
	./scc0 -o scc1 $(ALLSRC)

scc0:	$(ALLSRC)
	$(CC) $(INCLUDES) -o scc0 $(ALLSRC)

lib:	$(LIBDIR)/libscc.a

crt0:	$(LIBDIR)/crt0.o

ncrt0:	$(LIBDIR)/ncrt0.o

exe:	scc.exe sccmain.exe

$(LIBDIR)/crt0.o:	$(LIBDIR)/crt0.s
	$(AS) -o $(LIBDIR)/crt0.o $(LIBDIR)/crt0.s

$(LIBDIR)/ncrt0.o:	$(LIBDIR)/ncrt0.s
	$(AS) -o $(LIBDIR)/ncrt0.o $(LIBDIR)/ncrt0.s

$(LIBDIR)/ncrt0.s:	$(LIBDIR)/crt0.s
	sed -e '/C_init.*[;@#]INIT/d' \
	    -e 's/\(.*\)Cexit\(.*\)EXIT/\1C_exit\2EXIT/' \
	    <$(LIBDIR)/crt0.s >$(LIBDIR)/ncrt0.s

$(LIBDIR)/libscc.a:	$(LIBOBJS)
	$(AR) $(LIBDIR)/libscc.a $(LIBOBJS)
	$(RANLIB) $(LIBDIR)/libscc.a

$(SRC):		$(HDRS)
libscc.a:	$(OBJS)

.c.o:
	./scc0 -c $<

scc.exe:	main.c
	./scc0 -o scc.exe main.c

sccmain.exe:	$(SRC) targets/lib/ulibc.c targets/dos-8086/dosmain.c
	./scc0 -c $(SRC) && rm -f main.o
	./scc0 -o ulibc.o -c targets/lib/ulibc.c
	./scc0 -o dosmain.o -c targets/dos-8086/dosmain.c
	sld -o sccmain.exe $(LIBDIR)/crt0.o *.o $(LIBDIR)/libscc.a

tests:	test ptest systest libtest sigtest
	@echo "Looks good!"

test:	scc $(ALLSRC)
	sh ../tests/triple-test.sh

ptest:	scc0 $(LIBDIR)/crt0.o $(LIBDIR)/libscc.a ../tests/ptest.src ../tests/ptest.awk
	awk -f ../tests/ptest.awk <../tests/ptest.src >../tests/ptest.c
	./scc0 -o ../tests/ptest ../tests/ptest.c
	../tests/ptest
	rm ../tests/ptest

systest:	scc0 $(LIBDIR)/crt0.o $(LIBDIR)/libscc.a ../tests/systest.c
	./scc0 -o ../tests/systest ../tests/systest.c
	 (cd ../tests && ./systest)
	rm ../tests/systest

libtest:	scc0 $(LIBDIR)/crt0.o $(LIBDIR)/libscc.a ../tests/libtest.c
	./scc0 -o ../tests/libtest$(EXESFX) ../tests/libtest.c
	(cd ../tests && ./libtest$(EXESFX))
	rm ../tests/libtest$(EXESFX)

sigtest:	scc0 $(LIBDIR)/crt0.o $(LIBDIR)/libscc.a ../tests/sigtest.c
	./scc0 -o ../tests/sigtest ../tests/sigtest.c
	(cd ../tests && ./sigtest)
	rm ../tests/sigtest

dirs:
	-mkdir $(PREFIX)
	-mkdir $(SCCDIR)
	-mkdir $(BINDIR)
	-mkdir $(SCCDIR)/include
	-mkdir $(SCCDIR)/lib

dirs86:
	-mkdir $(PREFIX)
	-mkdir $(SC86DIR)
	-mkdir $(BINDIR)
	-mkdir $(SC86DIR)/include
	-mkdir $(SC86DIR)/lib

install:	all
	./scc0 -o scc -D 'SCCDIR="$(SCCDIR)"' $(ALLSRC)
	cp scc $(BINDIR)/scc$(EXESFX)
	strip $(BINDIR)/scc$(EXESFX)
	cp $(INCDIR)/* $(SCCDIR)/include
	cp $(LIBDIR)/libscc.a $(LIBDIR)/crt0.o $(LIBDIR)/ncrt0.o $(SCCDIR)/lib

install-scc86:	all
	scc -o scc86 -D 'SCCDIR="$(SC86DIR)"' $(ALLSRC)
	cp scc86 $(BINDIR)/scc86$(EXESFX)
	strip $(BINDIR)/scc86$(EXESFX)
	cp $(INCDIR)/* $(SC86DIR)/include
	cp $(LIBDIR)/libscc.a $(LIBDIR)/crt0.o $(LIBDIR)/ncrt0.o $(SC86DIR)/lib

subc-dos.zip:
	mkdir subc-dos
	mkdir subc-dos/include
	mkdir subc-dos/lib
	mkdir subc-dos/src
	cp $(INCDIR)/* subc-dos/include
	cp $(LIBDIR)/crt0.o $(LIBDIR)/ncrt0.o $(LIBDIR)/libscc.a subc-dos/lib
	cp *.[ch] targets/lib/ulibc.c targets/dos-8086/dosmain.c subc-dos/src
	cp targets/dos-8086/dosbuild.bat targets/dos-8086/link.lst subc-dos
	cp scc.exe sccmain.exe subc-dos
	cp ../s86/*.exe subc-dos
	echo "This a binary-only archive of SubC for DOS/8086." \
		> subc-dos/ABOUT.TXT
	echo "You can find the source code at http://t3x.org." \
		>> subc-dos/ABOUT.TXT
	echo "The SubC compiler is in the public domain." \
		>> subc-dos/ABOUT.TXT
	echo "WARNING:" >> subc-dos/ABOUT.TXT
	echo "Most text files in this archive are in Unix format!" \
		>> subc-dos/ABOUT.TXT
	sed -e 's/$$/~/' <../README | tr '~' '\015' >subc-dos/README.TXT
	zip -9r subc-dos.zip subc-dos
	rm -r subc-dos

lint:
	$(LINT) main.c
	$(LINT) frontend/lexer/scan.c
	$(LINT) frontend/preprocessor/prep.c
	$(LINT) frontend/parser/decl.c
	$(LINT) frontend/parser/expr.c
	$(LINT) frontend/parser/stmt.c
	$(LINT) ast/tree.c
	$(LINT) optimizer/opt.c
	$(LINT) codegen/gen.c
	$(LINT) symbols/sym.c
	$(LINT) common/error.c
	$(LINT) common/misc.c
	$(LINT) targets/arch/cgtarget.c
	$(LINT) targets/arch/cg_os_configs.c
	$(LINT) targets/arch/x86/i386/cg_386.c
	$(LINT) targets/arch/x86/i386/cg_386_arch.c
	$(LINT) targets/arch/x86/x86_64/cg_x86_64.c
	$(LINT) targets/arch/x86/x86_64/cg_x86_64_arch.c
	$(LINT) targets/arch/x86/i8086/cg_8086.c
	$(LINT) targets/arch/x86/i8086/cg_8086_arch.c
	$(LINT) targets/arch/arm/armv6/cg_armv6.c
	$(LINT) targets/arch/arm/arm64/cg_arm64.c
	$(LINT) targets/arch/arm/arm64/cg_arm64_arch.c

clean:
	rm -f scc0 scc1 scc2 scc scc86 test.s *.o *.core core a.out \
		*.tmp $(LIBDIR)/*.o $(LIBDIR)/ncrt0.s $(LIBDIR)/libscc.a *.exe subc-dos.zip
