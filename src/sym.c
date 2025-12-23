/*
 *	NMH's Simple C Compiler, 2011--2021
 *	Symbol table management
 */

#include "defs.h"
#include "data.h"
#include "decl.h"

int findglob(char *s) {
	int	i;

	for (i=0; i<Globs; i++) {
		if (	Types[i] != TMACRO && Stcls[i] != CMEMBER &&
			*s == *Names[i] && !strcmp(s, Names[i])
		)
			return i;
	}
	return 0;
}

int findloc(char *s) {
	int	i;

	for (i=Locs; i<NSYMBOLS; i++) {
		if (	Stcls[i] != CMEMBER &&
			*s == *Names[i] && !strcmp(s, Names[i])
		)
			return i;
	}
	return 0;
}

int findsym(char *s) {
	int	y;

	if ((y = findloc(s)) != 0) return y;
	return findglob(s);
}

int findmac(char *s) {
	int	i;

	for (i=0; i<Globs; i++)
		if (	TMACRO == Types[i] &&
			*s == *Names[i] && !strcmp(s, Names[i])
		)
			return i;
	return 0;
}

int findstruct(char *s) {
	int	i;

	for (i=Locs; i<NSYMBOLS; i++)
		if (	TSTRUCT == Types[i] &&
			*s == *Names[i] && !strcmp(s, Names[i])
		)
			return i;
	for (i=0; i<Globs; i++)
		if (	TSTRUCT == Types[i] &&
			*s == *Names[i] && !strcmp(s, Names[i])
		)
			return i;
	return 0;
}

int findmem(int y, char *s) {
	y++;
	while (	(y < Globs ||
		 (y >= Locs && y < NSYMBOLS)) &&
		CMEMBER ==  Stcls[y]
	) {
		if (*s == *Names[y] && !strcmp(s, Names[y]))
			return y;
		y++;
	}
	return 0;
}

int newglob(void) {
	int	p;

	if ((p = Globs++) >= Locs)
		fatal("too many global symbols");
	return p;
}

int newloc(void) {
	int	p;

	if ((p = --Locs) <= Globs)
		fatal("too many local symbols");
	return p;
}

#ifdef __SUBC__
 #define PTR_INT_CAST	(int)
#else
 #define PTR_INT_CAST	(int) (long)
#endif

char *galloc(int k, int align) {
	int	p, mask;

	k += align * sizeof(int);
	if (Nbot + k >= Ntop)
		fatal("out of space for symbol names");
	p = Nbot;
	Nbot += k;
	mask = sizeof(int)-1;
	if (align)
		while (PTR_INT_CAST &Nlist[p] & mask)
			p++;
	return &Nlist[p];
}

char *globname(char *s) {
	char	*p;
	
	p = galloc(strlen(s)+1, 0);
	strcpy(p, s);
	return p;
}

char *locname(char *s) {
	int	p, k;

	k = strlen(s) + 1;
	if (Nbot + k >= Ntop)
		fatal("out of space for symbol names");
	Ntop -= k;
	p = Ntop;
	strcpy(&Nlist[p], s);
	return &Nlist[p];
}

static void defglob(char *name, int prim, int type, int size, int val,
			int scls, int init)
{
	int	st, objsz;

	if (TCONSTANT == type || TFUNCTION == type) return;
	gendata();
	st = scls == CSTATIC;
	if (CPUBLIC == scls) genpublic(name);
	if (init && TARRAY == type)
		return;
	if (TARRAY != type && !(prim & STCMASK)) genname(name);

	/* Handle struct/union types */
	if (prim & STCMASK) {
		if (TARRAY == type)
			genbss(gsym(name), objsize(prim, TARRAY, size), st);
		else
			genbss(gsym(name), objsize(prim, TVARIABLE, size), st);
		return;
	}

	/* Get object size for this type */
	objsz = objsize(prim, TVARIABLE, 1);

	/* Handle arrays */
	if (TARRAY == type) {
		genbss(gsym(name), objsz * size, st);
		return;
	}

	/* Handle scalar types based on size - use architecture-specific sizes */
	if (objsz == CHARSIZE) {
		gendefb(val);
		genalign(1);
	}
	else if (objsz == SHORTSIZE) {
		gendefh(val);
	}
	else if (objsz == INTSIZE) {
		gendefw(val);  /* native int size */
	}
	else if (objsz == LONGSIZE && LONGSIZE != INTSIZE) {
		gendefd(val);  /* long when different from int (e.g., 4 bytes on 16-bit) */
	}
	else if (objsz == DOUBLESIZE) {
		gendefq(val);  /* 8 bytes for double */
	}
	else {
		/* Pointer types or unknown - use pointer size */
		gendefp(val);
	}
}

int redeclare(char *name, int oldcls, int newcls) {
	switch (oldcls) {
	case CEXTERN:
		if (newcls != CPUBLIC && newcls != CEXTERN)
			error("extern symbol redeclared static: %s", name);
		return newcls;
	case CPUBLIC:
		if (CEXTERN == newcls)
			return CPUBLIC;
		if (newcls != CPUBLIC) {
			error("extern symbol redeclared static: %s", name);
			return CPUBLIC;
		}
		break;
	case CSPROTO:
		if (newcls != CSTATIC && newcls != CSPROTO)
			error("static symbol redeclared extern: %s", name);
		return newcls;
	case CSTATIC:
		if (CSPROTO == newcls)
			return CSTATIC;
		if (newcls != CSTATIC) {
			error("static symbol redeclared extern: %s", name);
			return CSTATIC;
		}
		break;
	case CTYPE:
		error("redefinition of typedef name", Text);
		return CTYPE;
		break;
	}
	error("redefined symbol: %s", name);
	return newcls;
}

int addglob(char *name, int prim, int type, int scls, int size, int val,
		char *mtext, int init)
{
	int	y;

	if (0 == *name)
		y = 0;
	else if ((y = findglob(name)) != 0) {
		scls = redeclare(name, Stcls[y], scls);
		if (CTYPE == scls) return y;
		if (TFUNCTION == Types[y])
			mtext = Mtext[y];
	}
	if (0 == y) {
 		y = newglob();
		Names[y] = globname(name);
	}
	else if (TFUNCTION == Types[y] || TMACRO == Types[y]) {
		if (Prims[y] != prim || Types[y] != type)
			error("redefinition does not match prior type: %s",
				name);
	}
	if (CPUBLIC == scls || CSTATIC == scls)
		defglob(name, prim, type, size, val, scls, init);
	Prims[y] = prim;
	Types[y] = type;
	Stcls[y] = scls;
	Sizes[y] = size;
	Vals[y] = val;
	Mtext[y] = mtext;
	return y;
}

static void defloc(int prim, int type, int size, int val, int init) {
	int	objsz;

	gendata();
	if (type != TARRAY && !(prim &STCMASK)) genlab(val);

	/* Handle struct/union types */
	if (prim & STCMASK) {
		if (TARRAY == type)
			genbss(labname(val), objsize(prim, TARRAY, size), 1);
		else
			genbss(labname(val), objsize(prim, TVARIABLE, size), 1);
		return;
	}

	/* Get object size for this type */
	objsz = objsize(prim, TVARIABLE, 1);

	/* Handle arrays */
	if (TARRAY == type) {
		genbss(labname(val), objsz * size, 1);
		return;
	}

	/* Handle scalar types based on size - use architecture-specific sizes */
	if (objsz == CHARSIZE) {
		gendefb(init);
		genalign(1);
	}
	else if (objsz == SHORTSIZE) {
		gendefh(init);
	}
	else if (objsz == INTSIZE) {
		gendefw(init);
	}
	else if (objsz == LONGSIZE && LONGSIZE != INTSIZE) {
		gendefd(init);
	}
	else if (objsz == DOUBLESIZE) {
		gendefq(init);
	}
	else {
		gendefp(init);
	}
}

int addloc(char *name, int prim, int type, int scls, int size, int val,
		int init)
{
	int	y;

	if (findloc(name))
		error("redefinition of: %s", name);
 	y = newloc();
	if (CLSTATC == scls) defloc(prim, type, size, val, init);
	Names[y] = locname(name);
	Prims[y] = prim;
	Types[y] = type;
	Stcls[y] = scls;
	Sizes[y] = size;
	Vals[y] = val;
	return y;
}

void clrlocs(void) {
	Ntop = POOLSIZE;
	Locs = NSYMBOLS;
}

int objsize(int prim, int type, int size) {
	int	k = 0, sp;

	sp = prim & STCMASK;

	/* Base types */
	if (PINT == prim || PUINT == prim)
		k = INTSIZE;
	else if (PCHAR == prim || PUCHAR == prim || PSCHAR == prim)
		k = CHARSIZE;
	else if (PSHORT == prim || PUSHORT == prim)
		k = SHORTSIZE;
	else if (PLONG == prim || PULONG == prim)
		k = LONGSIZE;
	else if (PFLOAT == prim)
		k = FLOATSIZE;
	else if (PDOUBLE == prim)
		k = DOUBLESIZE;
	/* Pointer types */
	else if (INTPTR == prim || CHARPTR == prim || VOIDPTR == prim)
		k = PTRSIZE;
	else if (INTPP == prim || CHARPP == prim || VOIDPP == prim)
		k = PTRSIZE;
	else if (UCHARPTR == prim || SCHARPTR == prim)
		k = PTRSIZE;
	else if (SHORTPTR == prim || USHORTPTR == prim)
		k = PTRSIZE;
	else if (UINTPTR == prim)
		k = PTRSIZE;
	else if (LONGPTR == prim || ULONGPTR == prim)
		k = PTRSIZE;
	else if (FLOATPTR == prim || DOUBLEPTR == prim)
		k = PTRSIZE;
	else if (UCHARPP == prim || SCHARPP == prim)
		k = PTRSIZE;
	else if (SHORTPP == prim || USHORTPP == prim)
		k = PTRSIZE;
	else if (UINTPP == prim)
		k = PTRSIZE;
	else if (LONGPP == prim || ULONGPP == prim)
		k = PTRSIZE;
	else if (FLOATPP == prim || DOUBLEPP == prim)
		k = PTRSIZE;
	/* Struct/union types */
	else if (STCPTR == sp || STCPP == sp)
		k = PTRSIZE;
	else if (UNIPTR == sp || UNIPP == sp)
		k = PTRSIZE;
	else if (PSTRUCT == sp || PUNION == sp)
		k = Sizes[prim & ~STCMASK];
	else if (FUNPTR == prim)
		k = PTRSIZE;

	if (TFUNCTION == type || TCONSTANT == type || TMACRO == type)
		return -1;
	if (TARRAY == type)
		k *= size;
	return k;
}

static char *typename(int p) {
	switch (p & STCMASK) {
	case PSTRUCT:	return "STRUCT";
	case STCPTR:	return "STCT*";
	case STCPP:	return "STCT**";
	case PUNION:	return "UNION";
	case UNIPTR:	return "UNIO*";
	case UNIPP:	return "UNIO**";
	}
	switch (p) {
	case PCHAR:     return "CHAR";
	case PSCHAR:    return "SCHAR";
	case PUCHAR:    return "UCHAR";
	case PSHORT:    return "SHORT";
	case PUSHORT:   return "USHORT";
	case PINT:      return "INT";
	case PUINT:     return "UINT";
	case PLONG:     return "LONG";
	case PULONG:    return "ULONG";
	case PFLOAT:    return "FLOAT";
	case PDOUBLE:   return "DOUBLE";
	case PVOID:     return "VOID";
	case CHARPTR:   return "CHAR*";
	case SCHARPTR:  return "SCHAR*";
	case UCHARPTR:  return "UCHAR*";
	case SHORTPTR:  return "SHORT*";
	case USHORTPTR: return "USHRT*";
	case INTPTR:    return "INT*";
	case UINTPTR:   return "UINT*";
	case LONGPTR:   return "LONG*";
	case ULONGPTR:  return "ULONG*";
	case FLOATPTR:  return "FLOAT*";
	case DOUBLEPTR: return "DOUBL*";
	case VOIDPTR:   return "VOID*";
	case FUNPTR:    return "FUN*";
	case CHARPP:    return "CHAR**";
	case SCHARPP:   return "SCHR**";
	case UCHARPP:   return "UCHR**";
	case SHORTPP:   return "SHRT**";
	case USHORTPP:  return "USHT**";
	case INTPP:     return "INT**";
	case UINTPP:    return "UINT**";
	case LONGPP:    return "LONG**";
	case ULONGPP:   return "ULNG**";
	case FLOATPP:   return "FLT**";
	case DOUBLEPP:  return "DBL**";
	case VOIDPP:    return "VOID**";
	default:        return "n/a";
	}
}

void dumpsyms(char *title, char *sub, int from, int to) {
	int	i;
	int	*p;

	printf("\n===== %s%s =====\n", title, sub);
	printf(	"PRIM    TYPE  STCLS   SIZE  VALUE  NAME [MVAL]/(SIG)\n"
		"------  ----  -----  -----  -----  -----------------\n");
	for (i = from; i < to; i++) {
		printf("%-6s  %s  %s  %5d  %5d  %s",
			typename(Prims[i]),
			TVARIABLE == Types[i]? "VAR ":
				TARRAY == Types[i]? "ARRY":
				TFUNCTION == Types[i]? "FUN ":
				TCONSTANT == Types[i]? "CNST":
				TMACRO == Types[i]? "MAC ":
				TSTRUCT == Types[i]? "STCT": "n/a",
			CPUBLIC == Stcls[i]? "PUBLC":
				CEXTERN == Stcls[i]? "EXTRN":
				CSTATIC == Stcls[i]? "STATC":
				CSPROTO == Stcls[i]? "STATP":
				CLSTATC == Stcls[i]? "LSTAT":
				CAUTO   == Stcls[i]? "AUTO ":
				CMEMBER == Stcls[i]? "MEMBR":
				CTYPE   == Stcls[i]? "TYPE ": "n/a  ",
			Sizes[i],
			Vals[i],
			Names[i]);
		if (TMACRO == Types[i])
			printf(" [\"%s\"]", Mtext[i]);
		if (TFUNCTION == Types[i]) {
			printf(" (");
			for (p = (int *) Mtext[i]; *p; p++) {
				printf("%s", typename(*p));
				if (p[1]) printf(", ");
			}
			putchar(')');
		}
		putchar('\n');
	}
}
