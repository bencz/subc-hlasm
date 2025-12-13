# S/390 HLASM target support added by ALEXANDRE BENCZ

CC=	cc
CFLAGS=	-O -m32 -std=c89
LINT=	gcc -c -o /dev/null -std=c89 -pedantic -Wall 

# Target selection: 386 (default) or s390
TARGET=	386

# Source files - code generator is selected by TARGET
CGFILE=	cg$(TARGET).c
FILES=	cexpr.c $(CGFILE) decl.c error.c expr.c gen.c main.c misc.c \
	prep.c scan.c stmt.c sym.c
OBJS=	cexpr.o cg$(TARGET).o decl.o error.o expr.o gen.o main.o misc.o \
	prep.o scan.o stmt.o sym.o

all:	scc0 lib/crt0.o lib/libscc.a

scc0:	$(FILES)
	$(CC) $(CFLAGS) -o scc0 $(FILES)

lib/crt0.o:	lib/crt0.s
	as -32 -o lib/crt0.o lib/crt0.s

lib/libscc.a:
	make -f lib/Makefile

test:	scc $(FILES)
	./scc -o scc2 $(FILES)
	cmp scc scc2 && rm -f scc2

scc:	scc1 $(FILES)
	./scc1 -o scc $(FILES)

scc1:	scc0 lib/crt0.o lib/libscc.a $(FILES)
	./scc0 -o scc1 $(FILES)

test-all:	test
	./scc -o ptest tests/ptest.c
	./ptest
	./scc -o libtest tests/libtest.c
	cp tests/stdio.ok .
	./libtest
	./scc -o systest tests/systest.c
	./systest

$(FILES):	data.h decl.h defs.h prec.h
gen.c:		cgen.h

lint:
	$(LINT) cexpr.c
	$(LINT) $(CGFILE)
	$(LINT) decl.c
	$(LINT) error.c
	$(LINT) expr.c
	$(LINT) gen.c
	$(LINT) main.c
	$(LINT) misc.c
	$(LINT) prep.c
	$(LINT) scan.c
	$(LINT) stmt.c
	$(LINT) sym.c

# Build for S/390 target (generates HLASM with ESA/390 instructions)
scc-s390:
	$(MAKE) TARGET=s390 scc0
	mv scc0 scc-s390

# Build for S/370 compatible target (no AHI/LHI instructions)
scc-s370:
	$(MAKE) TARGET=s390 CFLAGS="$(CFLAGS) -DS390_COMPAT_370" scc0
	mv scc0 scc-s370

# Test S/390 code generation
# Source: tests-hlasm/*.c  Output: tests-s390/*.s
test-s390: scc-s390
	@echo "=== Compiling S/390 (ESA/390) test programs ==="
	@mkdir -p tests-s390
	@rm -f tests-s390/*.s
	@for f in tests-hlasm/*.c; do \
		base=$$(basename $$f .c); \
		echo "Compiling $$base.c -> tests-s390/$$base.s"; \
		./scc-s390 -S -o tests-s390/$$base.s $$f; \
	done
	@echo "=== All S/390 tests compiled in tests-s390/ ==="

# Test S/370 code generation
# Source: tests-hlasm/*.c  Output: tests-s370/*.s
test-s370: scc-s370
	@echo "=== Compiling S/370 compatible test programs ==="
	@mkdir -p tests-s370
	@rm -f tests-s370/*.s
	@for f in tests-hlasm/*.c; do \
		base=$$(basename $$f .c); \
		echo "Compiling $$base.c -> tests-s370/$$base.s"; \
		./scc-s370 -S -o tests-s370/$$base.s $$f; \
	done
	@echo "=== All S/370 tests compiled in tests-s370/ ==="

csums:
	csum -u <_sums >_newsums ; mv -f _newsums _sums

sums:	clean
	find . -type f | grep -v _sums | csum >_sums

clean:
	rm -f scc0 scc1 scc2 scc scc-s390 scc-s370 test.s *.o *.core core a.out lib/libscc.a \
		tests/*.o lib/*.o book/*.o libtest ptest systest stdio.ok \
		subc-b2022.zip
	rm -rf tests-s370 tests-s390

arc:	clean
	zip -9r subc-b2022.zip *
