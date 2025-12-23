/*
 *	NMH's Simple C Compiler, 2011--2022
 *	Declaration parser
 */

#include "defs.h"
#include "data.h"
#include "decl.h"

static int declarator(int arg, int scls, char *name, int *pprim, int *psize,
			int *pval, int *pinit);

/*
 * enumdecl := { enumlist } ;
 *
 * enumlist :=
 *	  enumerator
 *	| enumerator , enumlist
 *
 * enumerator :=
 *	  IDENT
 *	| IDENT = constexpr
 */

static void enumdecl(int glob) {
	int	v = 0;
	char	name[NAMELEN+1];

	Token = scan();
	if (IDENT == Token)
		Token = scan();
	lbrace();
	while (RBRACE != Token) {
		copyname(name, Text);
		ident();
		if (ASSIGN == Token) {
			Token = scan();
			v = constexpr();
		}
		if (glob)
			addglob(name, PINT, TCONSTANT, 0, 0, v++, NULL, 0);
		else
			addloc(name, PINT, TCONSTANT, 0, 0, v++, 0);
		if (Token != COMMA)
			break;
		Token = scan();
		if (eofcheck()) return;
	}
	rbrace();
	semi();
}

/*
 * initlist :=
 *	  { const_list }
 *	| STRLIT
 *
 * const_list :=
 *	  constexpr
 *	| constexpr , const_list
 */

static int initlist(char *name, int prim) {
	int	n = 0, v;
	char	buf[30];

	gendata();
	genname(name);
	if (STRLIT == Token) {
		if (PCHAR != prim)
			error("initializer type mismatch: %s", name);
		gendefs(Text, Value);
		gendefb(0);
		genalign(Value-1);
		Token = scan();
		return Value-1;
	}
	lbrace();
	while (Token != RBRACE) {
		v = constexpr();
		if (PCHAR == prim) {
			if (v < 0 || v > 255) {
				sprintf(buf, "%d", v);
				error("initializer out of range: %s", buf);
			}
			gendefb(v);
		}
		else {
			gendefw(v);
		}
		n++;
		if (COMMA == Token)
			Token = scan();
		else
			break;
		if (eofcheck()) return 0;
	}
	if (PCHAR == prim) genalign(n);
	Token = scan();
	if (!n) error("too few initializers", NULL);
	return n;
}

/*
 * Type specifier flags for combining type keywords
 */
#define TS_VOID     0x0001
#define TS_CHAR     0x0002
#define TS_SHORT    0x0004
#define TS_INT      0x0008
#define TS_LONG     0x0010
#define TS_FLOAT    0x0020
#define TS_DOUBLE   0x0040
#define TS_SIGNED   0x0080
#define TS_UNSIGNED 0x0100
#define TS_STRUCT   0x0200
#define TS_UNION    0x0400

/*
 * Check if token is a type specifier keyword
 */
int istypespec(int t) {
	return t == CHAR || t == INT || t == SHORT || t == LONG ||
	       t == FLOAT || t == DOUBLE || t == SIGNED || t == UNSIGNED ||
	       t == VOID || t == STRUCT || t == UNION || t == CONST;
}

/*
 * Parse type specifiers and return primitive type
 * Handles combinations like: unsigned int, long int, signed char, etc.
 *
 * C89 valid combinations:
 *   void
 *   char, signed char, unsigned char
 *   short, short int, signed short, signed short int, unsigned short, unsigned short int
 *   int, signed, signed int, unsigned, unsigned int
 *   long, long int, signed long, signed long int, unsigned long, unsigned long int
 *   float
 *   double
 *   struct/union
 *
 * NOTE: This function expects Token to already contain the first type specifier.
 *       It does NOT consume the token after the type - caller must call scan() after.
 */
int primtype(int t, char *s) {
	int	p, y;
	char	sname[NAMELEN+1];
	int	flags = 0;

	/* Handle struct/union specially */
	if (t == STRUCT || t == UNION) {
		p = (t == STRUCT) ? PSTRUCT : PUNION;
		if (!s) {
			Token = scan();
			copyname(sname, Text);
			s = sname;
			if (IDENT != Token) {
				error("struct/union name expected: %s", Text);
				return p;
			}
		}
		if ((y = findstruct(s)) == 0 || Prims[y] != p)
			error("no such struct/union: %s", s);
		p |= y;
		return p;
	}

	/* Collect type specifier flags from first token */
	switch (t) {
	case VOID:     flags |= TS_VOID; break;
	case CHAR:     flags |= TS_CHAR; break;
	case SHORT:    flags |= TS_SHORT; break;
	case INT:      flags |= TS_INT; break;
	case LONG:     flags |= TS_LONG; break;
	case FLOAT:    flags |= TS_FLOAT; break;
	case DOUBLE:   flags |= TS_DOUBLE; break;
	case SIGNED:   flags |= TS_SIGNED; break;
	case UNSIGNED: flags |= TS_UNSIGNED; break;
	case CONST:    break;  /* const is ignored (no-op) */
	default:       return PINT;  /* default to int */
	}

	/* Look ahead for more type specifiers */
	for (;;) {
		Token = scan();
		switch (Token) {
		case CHAR:     flags |= TS_CHAR; continue;
		case SHORT:    flags |= TS_SHORT; continue;
		case INT:      flags |= TS_INT; continue;
		case LONG:     flags |= TS_LONG; continue;
		case SIGNED:   flags |= TS_SIGNED; continue;
		case UNSIGNED: flags |= TS_UNSIGNED; continue;
		case CONST:    continue;  /* const is ignored */
		default:
			/* Not a type specifier - put it back */
			reject();
			break;
		}
		break;
	}

	/* Validate and convert flags to primitive type */
	if (flags & TS_VOID) {
		if (flags & ~TS_VOID)
			error("invalid type combination with void", NULL);
		return PVOID;
	}

	if (flags & TS_FLOAT) {
		if (flags & ~TS_FLOAT)
			error("invalid type combination with float", NULL);
		return PFLOAT;
	}

	if (flags & TS_DOUBLE) {
		if (flags & ~(TS_DOUBLE | TS_LONG))
			error("invalid type combination with double", NULL);
		return PDOUBLE;  /* long double treated as double */
	}

	if (flags & TS_CHAR) {
		if (flags & ~(TS_CHAR | TS_SIGNED | TS_UNSIGNED))
			error("invalid type combination with char", NULL);
		if (flags & TS_UNSIGNED)
			return PUCHAR;
		if (flags & TS_SIGNED)
			return PSCHAR;
		return PCHAR;  /* plain char */
	}

	if (flags & TS_SHORT) {
		if (flags & ~(TS_SHORT | TS_INT | TS_SIGNED | TS_UNSIGNED))
			error("invalid type combination with short", NULL);
		if (flags & TS_UNSIGNED)
			return PUSHORT;
		return PSHORT;
	}

	if (flags & TS_LONG) {
		if (flags & ~(TS_LONG | TS_INT | TS_SIGNED | TS_UNSIGNED))
			error("invalid type combination with long", NULL);
		if (flags & TS_UNSIGNED)
			return PULONG;
		return PLONG;
	}

	/* int or signed/unsigned alone */
	if (flags & TS_UNSIGNED)
		return PUINT;
	if (flags & TS_SIGNED)
		return PINT;
	if (flags & TS_INT)
		return PINT;

	/* No type specifiers - default to int */
	return PINT;
}

int usertype(char *s) {
	int	y;

	if ((y = findsym(s)) == 0) return 0;
	return CTYPE == Stcls[y]? y: 0;
}

/*
 * pmtrdecl :=
 *	  ( )
 *	| ( pmtrlist )
 *	| ( pmtrlist , ... )
 *
 * pmtrlist :=
 *	  primtype declarator
 *	| primtype declarator , pmtrlist
 *	| usertype declarator
 *	| usertype declarator , pmtrlist
 *
 * usertype :=
 *	  TYPEDEF_NAME
 */

static int pmtrdecls(void) {
	char	name[NAMELEN+1];
	int	utype, prim, type, size, na, addr;
	int	dummy;
	int	num_arg_regs;

	if (RPAREN == Token)
		return 0;
	na = 0;
	num_arg_regs = CG_NUM_ARG_REGS;
	
	/*
	 * Calculate parameter offset base.
	 * 
	 * For register-based ABIs (x86-64, ARM):
	 *   Parameters passed in registers are saved to stack by cgfnentry().
	 *   They are at negative offsets: -8(%rbp), -16(%rbp), etc.
	 *   Parameters beyond num_arg_regs are on the caller's stack at
	 *   positive offsets.
	 *
	 * For stack-based ABIs (i386, 8086):
	 *   All parameters are on the stack at positive offsets.
	 *   Use CG_PARAM_OFFSET_BASE as before.
	 */
	if (num_arg_regs > 0) {
		/* Register-based ABI: first param at -BPW from frame pointer */
		addr = -BPW;
	} else {
		/* Stack-based ABI: use architecture-specific offset */
		addr = CG_PARAM_OFFSET_BASE;
	}
	
	for (;;) {
		utype = 0;
		if (na > 0 && ELLIPSIS == Token) {
			Token = scan();
			na = -(na + 1);
			break;
		}
		else if (IDENT == Token &&
			 (utype = usertype(Text)) == 0)
		{
			prim = PINT;
		}
		else {
			if (	istypespec(Token) ||
				(IDENT == Token && utype != 0)
			) {
				name[0] = 0;
				prim = utype? Prims[utype]:
					primtype(Token, NULL);
				Token = scan();
				if (RPAREN == Token && prim == PVOID && !na)
					return 0;
			}
			else {
				error("type specifier expected at: %s", Text);
				Token = synch(RPAREN);
				return na;
			}
		}
		size = 1;
		type = declarator(1, CAUTO, name, &prim, &size, &dummy,
				&dummy);
		if ((utype && TARRAY == Types[utype]) || TARRAY == type) {
			prim = pointerto(prim);
			type = TVARIABLE;
		}
		addloc(name, prim, type, CAUTO, size, addr, 0);
		
		na++;
		/*
		 * Advance to next parameter.
		 * For register-based ABIs: continue with negative offsets
		 * until we exceed num_arg_regs, then switch to positive.
		 * For stack-based ABIs: use CG_PARAM_OFFSET_DIR.
		 *
		 * na is now the count of parameters processed (1-based).
		 * If na < num_arg_regs, next param is still in registers.
		 * If na == num_arg_regs, next param is first on stack.
		 * If na > num_arg_regs, next param is on stack.
		 */
		if (num_arg_regs > 0) {
			if (na < num_arg_regs) {
				/* Next param still in register args: go more negative */
				addr -= BPW;
			} else if (na == num_arg_regs) {
				/* Next param is first on stack */
				addr = CG_PARAM_OFFSET_BASE;
			} else {
				/* Stack args: positive direction */
				addr += BPW;
			}
		} else {
			addr += CG_PARAM_OFFSET_DIR * BPW;
		}
		if (COMMA == Token)
			Token = scan();
		else
			break;
	}
	return na;
}

int pointerto(int prim) {
	int	y;

	/* Check for too many levels of indirection */
	if (CHARPP == prim || INTPP == prim || VOIDPP == prim ||
	    UCHARPP == prim || SCHARPP == prim ||
	    SHORTPP == prim || USHORTPP == prim ||
	    UINTPP == prim || LONGPP == prim || ULONGPP == prim ||
	    FLOATPP == prim || DOUBLEPP == prim ||
	    FUNPTR == prim ||
	    (prim & STCMASK) == STCPP || (prim & STCMASK) == UNIPP
	)
		error("too many levels of indirection", NULL);

	/* Handle struct/union types */
	y = prim & ~STCMASK;
	switch (prim & STCMASK) {
	case PSTRUCT:	return STCPTR | y;
	case STCPTR:	return STCPP | y;
	case PUNION:	return UNIPTR | y;
	case UNIPTR:	return UNIPP | y;
	}

	/* Handle base types -> pointer */
	switch (prim) {
	case PCHAR:     return CHARPTR;
	case PSCHAR:    return SCHARPTR;
	case PUCHAR:    return UCHARPTR;
	case PSHORT:    return SHORTPTR;
	case PUSHORT:   return USHORTPTR;
	case PINT:      return INTPTR;
	case PUINT:     return UINTPTR;
	case PLONG:     return LONGPTR;
	case PULONG:    return ULONGPTR;
	case PFLOAT:    return FLOATPTR;
	case PDOUBLE:   return DOUBLEPTR;
	case PVOID:     return VOIDPTR;
	/* Handle pointer -> pointer-to-pointer */
	case CHARPTR:   return CHARPP;
	case SCHARPTR:  return SCHARPP;
	case UCHARPTR:  return UCHARPP;
	case SHORTPTR:  return SHORTPP;
	case USHORTPTR: return USHORTPP;
	case INTPTR:    return INTPP;
	case UINTPTR:   return UINTPP;
	case LONGPTR:   return LONGPP;
	case ULONGPTR:  return ULONGPP;
	case FLOATPTR:  return FLOATPP;
	case DOUBLEPTR: return DOUBLEPP;
	case VOIDPTR:   return VOIDPP;
	default:        return VOIDPP;
	}
}

/*
 * declarator :=
 *	  IDENT
 *	| * IDENT
 *	| * * IDENT
 *	| * IDENT [ constexpr ]
 *	| IDENT [ constexpr ]
 *	| IDENT = constexpr
 *	| IDENT [ ] = initlist
 *	| IDENT pmtrdecl
 *	| IDENT [ ]
 *	| * IDENT [ ]
 *	| ( * IDENT ) ( )
 */

static int declarator(int pmtr, int scls, char *name, int *pprim, int *psize,
			int *pval, int *pinit)
{
	int	type = TVARIABLE;
	int	ptrptr = 0;
	char	*unsupp;

	unsupp = "unsupported typedef syntax";
	if (STAR == Token) {
		Token = scan();
		*pprim = pointerto(*pprim);
		if (STAR == Token) {
			Token = scan();
			*pprim = pointerto(*pprim);
			ptrptr = 1;
		}
	}
	else if (LPAREN == Token) {
		if (CTYPE == scls)
			error(unsupp, NULL);
		if (*pprim != PINT)
			error("function pointers are limited to type 'int'",
				NULL);
		Token = scan();
		*pprim = FUNPTR;
		match(STAR, "(*name)()");
	}
	if (IDENT != Token) {
		error("missing identifier at: %s", Text);
		name[0] = 0;
	}
	else {
		copyname(name, Text);
		Token = scan();
	}
	if (FUNPTR == *pprim) {
		rparen();
		lparen();
		rparen();
	}
	if (!pmtr && ASSIGN == Token) {
		if (CTYPE == scls)
			error(unsupp, NULL);
		Token = scan();
		*pval = constexpr();
		if (PCHAR == *pprim)
			*pval &= 0xff;
		if (*pval && !inttype(*pprim))
			error("non-zero pointer initialization", NULL);
		*pinit = 1;
	}
	else if (!pmtr && LPAREN == Token) {
		if (CTYPE == scls)
			error(unsupp, NULL);
		Token = scan();
		*psize = pmtrdecls();
		rparen();
		return TFUNCTION;
	}
	else if (LBRACK == Token) {
		if (ptrptr)
			error("too many levels of indirection: %s", name);
		Token = scan();
		if (RBRACK == Token) {
			if (CTYPE == scls)
				error(unsupp, NULL);
			Token = scan();
			if (pmtr) {
				*pprim = pointerto(*pprim);
			}
			else {
				type = TARRAY;
				*psize = 1;
				if (ASSIGN == Token) {
					Token = scan();
					if (!inttype(*pprim))
						error("initialization of"
							" pointer array not"
							" supported",
							NULL);
					*psize = initlist(name, *pprim);
					if (CAUTO == scls)
						error("initialization of"
							" local arrays"
							" not supported: %s",
							name);
					*pinit = 1;
				}
				else if (CEXTERN != scls) {
					error("automatically-sized array"
						" lacking initialization: %s",
						name);
				}
			}
		}
		else {
			*psize = constexpr();
			if (*psize < 1) {
				error("invalid array size", NULL);
				*psize = 0;
			}
			type = TARRAY;
			rbrack();
		}
	}
	if (PVOID == *pprim)
		error("'void' is not a valid type: %s", name);
	return type;
}

int upgrade_array(int utype, int type, int *size) {
	if (utype && TARRAY == Types[utype]) {
		if (TARRAY == type)
			error("unsupported typedef (array of array)", NULL);
		*size = *size? *size * Sizes[utype]: Sizes[utype];
		return TARRAY;
	}
	return type;
}

/*
 * localdecls :=
 *        ldecl
 *      | ldecl localdecls
 *
 * ldecl :=
 *	  primtype ldecl_list ;
 *	| usertype ldecl_list ;
 *	| lclass primtype ldecl_list ;
 *	| lclass ldecl_list ;
 *	| enum_decl
 *	| struct_decl
 *
 * lclass :=
 *	| AUTO
 *	| EXTERN
 *	| REGISTER
 *	| STATIC
 *	| VOLATILE
 *
 * ldecl_list :=
 *	  declarator
 *	| declarator , ldecl_list
 */

static int localdecls(void) {
	char	name[NAMELEN+1];
	int	utype, prim, type, size, addr, val, ini;
	int	stat, extn;
	int	pbase, rsize;

	/*
	 * Use architecture-specific local variable offset base.
	 * For STACK_DOWN (x86): locals at negative offsets (e.g., -4, -8, -12...)
	 * For STACK_UP (S/370): locals at positive offsets from save area
	 */
	addr = CG_LOCAL_OFFSET_BASE;
	Nli = 0;
	utype = 0;
	while ( AUTO == Token || EXTERN == Token || REGISTER == Token ||
		STATIC == Token || VOLATILE == Token || CONST == Token ||
		istypespec(Token) || ENUM == Token ||
		(IDENT == Token && (utype = usertype(Text)) != 0)
	) {
		if (ENUM == Token) {
			enumdecl(0);
			continue;
		}
		extn = stat = 0;
		if (AUTO == Token || REGISTER == Token || STATIC == Token ||
			VOLATILE == Token || EXTERN == Token || CONST == Token
		) {
			stat = STATIC == Token;
			extn = EXTERN == Token;
			Token = scan();
			if (istypespec(Token)) {
				prim = primtype(Token, NULL);
				Token = scan();
			}
			else if (utype) {
				prim = Prims[utype];
			}
			else
				prim = PINT;
		}
		else if (utype) {
			prim = Prims[utype];
			Token = scan();
		}
		else {
			prim = primtype(Token, NULL);
			Token = scan();
		}
		pbase = prim;
		for (;;) {
			prim = pbase;
			if (eofcheck()) return 0;
			size = 1;
			ini = val = 0;
			type = declarator(0, CAUTO, name, &prim, &size,
					&val, &ini);
			type = upgrade_array(utype, type, &size);
			rsize = objsize(prim, type, size);
			rsize = (rsize + INTSIZE-1) / INTSIZE * INTSIZE;
			if (stat) {
				addloc(name, prim, type, CLSTATC, size,
					label(), val);
			}
			else if (extn) {
				addloc(name, prim, type, CEXTERN, size,
					0, val);
			}
			else {
				/*
				 * Allocate local variable using arch-specific direction.
				 * CG_LOCAL_OFFSET_DIR is -1 for STACK_DOWN, 1 for STACK_UP.
				 */
				addr += CG_LOCAL_OFFSET_DIR * rsize;
				addloc(name, prim, type, CAUTO, size, addr, 0);
			}
			if (ini && !stat) {
				if (Nli >= MAXLOCINIT) {
					error("too many local initializers",
						NULL);
					Nli = 0;
				}
				LIaddr[Nli] = addr;
				LIval[Nli++] = val;
			}
			if (COMMA == Token)
				Token = scan();
			else
				break;
		}
		semi();
		utype = 0;
	}
	return addr;
}

static int intcmp(int *x1, int *x2) {
	while (*x1 && *x1 == *x2)
		x1++, x2++;
	return *x1 - *x2;
}

static void signature(int fn, int from, int to) {
	int	types[MAXFNARGS+1], i;

	if (to - from > MAXFNARGS)
		error("too many function parameters", Names[fn]);
	for (i=0; i<MAXFNARGS && from < to; i++)
		types[i] = Prims[--to];
	types[i] = 0;
	if (NULL == Mtext[fn]) {
		Mtext[fn] = galloc((i+1) * sizeof(int), 1);
		memcpy(Mtext[fn], types, (i+1) * sizeof(int));
	}
	else if (intcmp((int *) Mtext[fn], types))
		error("declaration does not match prior prototype: %s",
			Names[fn]);
}

/*
 * decl :=
 *	  declarator { localdecls stmt_list }
 *	| decl_list ;
 *
 * decl_list :=
 *	  declarator
 *	| declarator , decl_list
 */

void decl(int clss, int prim, int utype) {
	char	name[NAMELEN+1];
	int	pbase, type, size = 0, val, init;
	int	lsize;

	pbase = prim;
	for (;;) {
		prim = pbase;
		val = 0;
		init = 0;
		type = declarator(0, clss, name, &prim, &size, &val, &init);
		type = upgrade_array(utype, type, &size);
		if (TFUNCTION == type) {
			clss = clss == CSTATIC? CSPROTO: CEXTERN;
			Thisfn = addglob(name, prim, type, clss, size, 0,
					NULL, 0);
			signature(Thisfn, Locs, NSYMBOLS);
			if (LBRACE == Token) {
				clss = clss == CSPROTO? CSTATIC:
					clss == CEXTERN? CPUBLIC: clss;
				Thisfn = addglob(name, prim, type, clss, size,
					0, NULL, 0);
				Token = scan();
				lsize = localdecls();
				gentext();
				if (CPUBLIC == clss) genpublic(name);
				genaligntext();
				genname(name);
				genfnentry(size);  /* size contains nparams from pmtrdecls */
				genstack(lsize);
				genlocinit();
				Retlab = label();
				compound(0);
				genlab(Retlab);
				genstack(-lsize);
				genexit();
				if (O_debug & D_LSYM)
					dumpsyms("LOCALS: ", name, Locs,
						NSYMBOLS);
			}
			else {
				semi();
			}
			clrlocs();
			return;
		}
		if (CEXTERN == clss && init) {
			error("initialization of 'extern': %s", name);
		}
		addglob(name, prim, type, clss, size, val, NULL, init);
		if (COMMA == Token)
			Token = scan();
		else
			break;
	}
	semi();
}

/*
 * structdecl :=
 *	  STRUCT IDENT { member_list } opt_decl ;
 *	| UNION IDENT { member_list } opt_decl ;
 *	| STRUCT { member_list } opt_decl ;
 *	| UNION { member_list } opt_decl ;
 *
 * opt_decl :=
 *      | decl
 *
 * member_list :=
 *	  primtype mdecl_list ;
 *	| primtype mdecl_list ; member_list
 *	| usertype mdecl_list ;
 *	| usertype mdecl_list ; member_list
 *
 * mdecl_list :=
 *	  declarator
 *	| declatator , mdecl_list
 */

void structdecl(int clss, int uniondecl) {
	int	utype, base, prim, size, dummy, type, addr = 0;
	char	name[NAMELEN+1], sname[NAMELEN+1];
	int	y, usize = 0;

	Token = scan();
	if (IDENT == Token) {
		copyname(sname, Text);
		Token = scan();
	}
	else {
		sname[0] = 0;
	}
	if (Token != LBRACE) {
		prim = primtype(uniondecl? UNION: STRUCT, sname);
		decl(clss, prim, 0);
		return;
	}
	y = addglob(sname, uniondecl? PUNION: PSTRUCT, TSTRUCT,
			CMEMBER, 0, 0, NULL, 0);
	Token = scan();
	utype = 0;
	while (	istypespec(Token) ||
		(IDENT == Token && (utype = usertype(Text)) != 0)
	) {
		base = utype? Prims[utype]: primtype(Token, NULL);
		size = 0;
		Token = scan();
		for (;;) {
			if (eofcheck()) return;
			prim = base;
			type = declarator(1, CMEMBER, name, &prim, &size,
						&dummy, &dummy);
			addglob(name, prim, type, CMEMBER, size, addr,
				NULL, 0);
			size = objsize(prim, type, size);
			if (size < 1)
				error("size of struct/union member"
					" is unknown: %s",
					name);
			if (uniondecl) {
				usize = size > usize? size: usize;
			}
			else {
				addr += size;
				addr = (addr + INTSIZE-1) / INTSIZE * INTSIZE;
			}
			if (Token != COMMA) break;
			Token = scan();
		}
		semi();
		utype = 0;
	}
	rbrace();
	Sizes[y] = uniondecl? usize: addr;
	if (Token != SEMI)
		decl(clss, Prims[y] | y, y);
	else
		semi();
}

/*
 * typedecl :=
 *	  TYPEDEF primtype decl
 *	| TYPEDEF usertype decl
 *	| TYPEDEF structdecl
 */

void typedecl(void) {
	int	utype, prim;

	Token = scan();
	if (STRUCT == Token || UNION == Token) {
		structdecl(CTYPE, UNION == Token);
	}
	else if ((utype = usertype(Text)) != 0) {
		Token = scan();
		decl(CTYPE, Prims[utype], utype);
	}
	else {
		prim = primtype(Token, NULL);
		Token = scan();
		decl(CTYPE, prim, 0);
	}
}

/*
 * top :=
 *	  ENUM enumdecl
 *	| decl
 *	| primtype decl
 *	| storclass decl
 *	| storclass primtype decl
 *	| typedecl
 *	| usertype decl
 *	| storclass usertype decl
 *
 * storclass :=
 *	  EXTERN
 *	| STATIC
 */

void top(void) {
	int	utype, prim, clss = CPUBLIC;

	/* Handle storage class specifiers */
	while (Token == EXTERN || Token == STATIC || Token == VOLATILE || Token == CONST) {
		if (Token == EXTERN) clss = CEXTERN;
		else if (Token == STATIC) clss = CSTATIC;
		/* VOLATILE and CONST are no-ops */
		Token = scan();
	}

	switch (Token) {
	case ENUM:
		enumdecl(1);
		break;
	case TYPEDEF:
		typedecl();
		break;
	case STRUCT:
	case UNION:
		structdecl(clss, UNION == Token);
		break;
	case IDENT:
		if ((utype = usertype(Text)) != 0) {
			Token = scan();
			decl(clss, Prims[utype], utype);
		}
		else
			decl(clss, PINT, 0);
		break;
	default:
		/* Check for type specifiers (char, int, short, long, etc.) */
		if (istypespec(Token)) {
			prim = primtype(Token, NULL);
			/* primtype() already scanned ahead and rejected the next token */
			Token = scan();
			decl(clss, prim, 0);
		}
		else {
			error("type specifier expected at: %s", Text);
			Token = synch(SEMI);
		}
		break;
	}
}

static void stats(void) {
	printf(	"Memory usage: "
		"Symbols: %5d/%5d, "
		"Names: %5d/%5d, "
		"Nodes: %5d/%5d\n",
		Globs, NSYMBOLS,
		Nbot, POOLSIZE,
		Ndmax, NODEPOOLSZ);
}

void defarg(char *s) {
	char	*p;

	if (NULL == s) return;
	if ((p = strchr(s, '=')) != NULL)
		*p++ = 0;
	else
		p = "";
	addglob(s, 0, TMACRO, 0, 0, 0, globname(p), 0);
	if (*p) *--p = '=';
}

void program(char *name, FILE *in, FILE *out, char *def) {
	init();
	defarg(def);
	Infile = in;
	Outfile = out;
	File = Basefile = name;
	genprelude();
	Token = scan();
	while (XEOF != Token)
		top();
	genpostlude();
	if (O_debug & D_GSYM) dumpsyms("GLOBALS", "", 1, Globs);
	if (O_debug & D_STAT) stats();
}
