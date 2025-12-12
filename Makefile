CC=	cc
CFLAGS=	-O -m32
LINT=	gcc -c -o /dev/null -ansi -pedantic -Wall 
FILES=	cexpr.c cg386.c decl.c error.c expr.c gen.c main.c misc.c \
	prep.c scan.c stmt.c sym.c
OBJS=	cexpr.o cg386.o decl.o error.o expr.o gen.o main.o misc.o \
	prep.o scan.o stmt.o sym.o

all:	scc0 lib/crt0.o lib/libscc.a

scc0:	$(FILES)
	$(CC) -o scc0 $(FILES)

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
	$(LINT) cg386.c
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

csums:
	csum -u <_sums >_newsums ; mv -f _newsums _sums

sums:	clean
	find . -type f | grep -v _sums | csum >_sums

clean:
	rm -f scc0 scc1 scc2 scc test.s *.o *.core core a.out lib/libscc.a \
		tests/*.o lib/*.o book/*.o libtest ptest systest stdio.ok \
		subc-b2022.zip

arc:	clean
	zip -9r subc-b2022.zip *
