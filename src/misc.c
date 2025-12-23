/*
 *	NMH's Simple C Compiler, 2011,2014
 *	Miscellanea
 */

#include "defs.h"
#include "data.h"
#include "decl.h"

void init(void) {
	Line = 1;
	Putback = '\n';
	Rejected = -1;
	Errors = 0;
	Mp = 0;
	Expandmac = 1;
	Syntoken = 0;
	Isp = 0;
	Inclev = 0;
	Globs = 0;
	Locs = NSYMBOLS;
	Nbot = 0;
	Ntop = POOLSIZE;
	Ndmax = 0;
	Bsp = 0;
	Csp = 0;
	Q_type = empty;
	Q_cmp = cnone;
	Q_bool = bnone;
	addglob("", 0, 0, 0, 0, 0, NULL, 0);
	addglob("__SUBC__", 0, TMACRO, 0, 0, 0, globname(""), 0);
	if (!strcmp(OS, "DOS"))
		addglob("__dos", 0, TMACRO, 0, 0, 0, globname(""), 0);
	else
		addglob("__unix", 0, TMACRO, 0, 0, 0, globname(""), 0);
	Infile = stdin;
	File = "(stdin)";
	Basefile = NULL;
	Outfile = stdout;
	opt_init();
}

int chrpos(char *s, int c) {
	char	*p;

	p = strchr(s, c);
	return p? p-s: -1;
}

void copyname(char *name, char *s) {
	strncpy(name, s, NAMELEN);
	name[NAMELEN] = 0;
}

void match(int t, char *what) {
	if (Token == t) {
		Token = scan();
	}
	else {
		error("%s expected", what);
	}
}

void lparen(void) {
	match(LPAREN, "'('");
}

void rparen(void) {
	match(RPAREN, "')'");
}

void lbrace(void) {
	match(LBRACE, "'{'");
}

void rbrace(void) {
	match(RBRACE, "'}'");
}

void rbrack(void) {
	match(RBRACK, "']'");
}

void semi(void) {
	match(SEMI, "';'");
}

void colon(void) {
	match(COLON, "':'");
}

void ident(void) {
	match(IDENT, "identifier");
}

int eofcheck(void) {
	if (XEOF == Token) {
		error("missing '}'", NULL);
		return 1;
	}
	return 0;
}

/*
 * Check if type is an integer type (char, short, int, long, signed/unsigned)
 */
int inttype(int p) {
	return PINT == p || PCHAR == p || PUCHAR == p || PSCHAR == p ||
	       PSHORT == p || PUSHORT == p || PUINT == p ||
	       PLONG == p || PULONG == p;
}

/*
 * Check if type is unsigned
 */
int unsignedtype(int p) {
	return PUCHAR == p || PUSHORT == p || PUINT == p || PULONG == p;
}

/*
 * Check if type is a floating-point type
 */
int floattype(int p) {
	return PFLOAT == p || PDOUBLE == p;
}

/*
 * Check if type is arithmetic (integer or floating-point)
 */
int arithtype(int p) {
	return inttype(p) || floattype(p);
}

/*
 * Check if type is a composite type (struct or union)
 */
int comptype(int p) {
	p &= STCMASK;
	return p == PSTRUCT || p == PUNION;
}

/*
 * Check that value is not void
 */
void notvoid(int p) {
	if (PVOID == p)
		error("void value in expression", NULL);
}

/*
 * Get the size rank of an integer type for promotion purposes
 * Higher rank = larger type
 */
int typerank(int p) {
	switch (p) {
	case PCHAR:
	case PUCHAR:
	case PSCHAR:
		return 1;
	case PSHORT:
	case PUSHORT:
		return 2;
	case PINT:
	case PUINT:
		return 3;
	case PLONG:
	case PULONG:
		return 4;
	case PFLOAT:
		return 5;
	case PDOUBLE:
		return 6;
	default:
		return 0;
	}
}

/*
 * Integer promotion: char and short are promoted to int
 * C89 6.2.1.1: "A char, a short int, or an int bit-field, or their
 * signed or unsigned varieties... is converted to an int"
 */
int intpromote(int p) {
	switch (p) {
	case PCHAR:
	case PSCHAR:
	case PSHORT:
		return PINT;
	case PUCHAR:
	case PUSHORT:
		/* If int can represent all values, promote to int, else unsigned int */
		return PINT;  /* Assuming int is at least 32 bits */
	default:
		return p;
	}
}

/*
 * Usual arithmetic conversions (C89 6.2.1.5)
 * Returns the common type for binary operations
 */
int usualarith(int p1, int p2) {
	/* If either is double, result is double */
	if (p1 == PDOUBLE || p2 == PDOUBLE)
		return PDOUBLE;
	/* If either is float, result is float */
	if (p1 == PFLOAT || p2 == PFLOAT)
		return PFLOAT;

	/* Integer promotions first */
	p1 = intpromote(p1);
	p2 = intpromote(p2);

	/* If same type, done */
	if (p1 == p2)
		return p1;

	/* If both signed or both unsigned, use higher rank */
	if (unsignedtype(p1) == unsignedtype(p2)) {
		return typerank(p1) > typerank(p2) ? p1 : p2;
	}

	/* One signed, one unsigned */
	if (unsignedtype(p1)) {
		/* p1 is unsigned, p2 is signed */
		if (typerank(p1) >= typerank(p2))
			return p1;
		/* If signed can represent all unsigned values, use signed */
		return p2;  /* Simplified: assume signed type is large enough */
	} else {
		/* p2 is unsigned, p1 is signed */
		if (typerank(p2) >= typerank(p1))
			return p2;
		return p1;
	}
}
