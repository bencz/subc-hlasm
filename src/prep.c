/*
 *	NMH's Simple C Compiler, 2011,2012,2014
 *	Preprocessor
 *
 *	Extended 2025 for C89 compliance:
 *	- #if / #elif with constant expression evaluation
 *	- defined() operator
 */

#include "defs.h"
#include "data.h"
#include "decl.h"

/*
 * ============================================================================
 * Preprocessor Expression Evaluator
 * ============================================================================
 *
 * Evaluates constant expressions for #if and #elif directives.
 * Supports: integers, defined(), !, ~, *, /, %, +, -, <<, >>,
 *           <, <=, >, >=, ==, !=, &, ^, |, &&, ||, ?:
 */

static char	*PPExpr;	/* Current position in expression */
static int	PPToken;	/* Current token */
static long	PPValue;	/* Current numeric value */
static char	PPIdent[NAMELEN+1];	/* Current identifier */

/* Forward declarations for expression parser */
static long pp_expr(void);
static long pp_ternary(void);
static long pp_logor(void);
static long pp_logand(void);
static long pp_bitor(void);
static long pp_bitxor(void);
static long pp_bitand(void);
static long pp_equality(void);
static long pp_relational(void);
static long pp_shift(void);
static long pp_additive(void);
static long pp_multiplicative(void);
static long pp_unary(void);
static long pp_primary(void);

/* Token types for preprocessor expression */
enum {
	PP_EOF = 0, PP_NUM, PP_IDENT, PP_DEFINED,
	PP_LPAREN, PP_RPAREN,
	PP_NOT, PP_TILDE,
	PP_STAR, PP_SLASH, PP_MOD,
	PP_PLUS, PP_MINUS,
	PP_LSHIFT, PP_RSHIFT,
	PP_LT, PP_LE, PP_GT, PP_GE,
	PP_EQ, PP_NE,
	PP_AND, PP_XOR, PP_OR,
	PP_LOGAND, PP_LOGOR,
	PP_QMARK, PP_COLON
};

/* Skip whitespace */
static void pp_skipws(void) {
	while (*PPExpr && isspace(*PPExpr))
		PPExpr++;
}

/* Get next token from expression */
static void pp_next(void) {
	pp_skipws();
	
	if (!*PPExpr) {
		PPToken = PP_EOF;
		return;
	}
	
	/* Number */
	if (isdigit(*PPExpr)) {
		PPValue = 0;
		if (PPExpr[0] == '0' && (PPExpr[1] == 'x' || PPExpr[1] == 'X')) {
			/* Hex */
			PPExpr += 2;
			while (isxdigit(*PPExpr)) {
				int d;
				if (*PPExpr >= '0' && *PPExpr <= '9')
					d = *PPExpr - '0';
				else if (*PPExpr >= 'a' && *PPExpr <= 'f')
					d = *PPExpr - 'a' + 10;
				else
					d = *PPExpr - 'A' + 10;
				PPValue = PPValue * 16 + d;
				PPExpr++;
			}
		} else if (*PPExpr == '0') {
			/* Octal */
			while (*PPExpr >= '0' && *PPExpr <= '7') {
				PPValue = PPValue * 8 + (*PPExpr - '0');
				PPExpr++;
			}
		} else {
			/* Decimal */
			while (isdigit(*PPExpr)) {
				PPValue = PPValue * 10 + (*PPExpr - '0');
				PPExpr++;
			}
		}
		/* Skip suffix (L, U, UL, etc.) */
		while (*PPExpr == 'L' || *PPExpr == 'l' || 
		       *PPExpr == 'U' || *PPExpr == 'u')
			PPExpr++;
		PPToken = PP_NUM;
		return;
	}
	
	/* Character constant */
	if (*PPExpr == '\'') {
		PPExpr++;
		if (*PPExpr == '\\') {
			PPExpr++;
			switch (*PPExpr) {
			case 'n': PPValue = '\n'; break;
			case 't': PPValue = '\t'; break;
			case 'r': PPValue = '\r'; break;
			case '0': PPValue = '\0'; break;
			case '\\': PPValue = '\\'; break;
			case '\'': PPValue = '\''; break;
			default: PPValue = *PPExpr; break;
			}
		} else {
			PPValue = *PPExpr;
		}
		PPExpr++;
		if (*PPExpr == '\'') PPExpr++;
		PPToken = PP_NUM;
		return;
	}
	
	/* Identifier or 'defined' */
	if (isalpha(*PPExpr) || *PPExpr == '_') {
		int i = 0;
		while ((isalnum(*PPExpr) || *PPExpr == '_') && i < NAMELEN) {
			PPIdent[i++] = *PPExpr++;
		}
		PPIdent[i] = '\0';
		/* Skip rest if too long */
		while (isalnum(*PPExpr) || *PPExpr == '_')
			PPExpr++;
		if (!strcmp(PPIdent, "defined")) {
			PPToken = PP_DEFINED;
		} else {
			PPToken = PP_IDENT;
		}
		return;
	}
	
	/* Operators */
	switch (*PPExpr) {
	case '(':
		PPToken = PP_LPAREN;
		PPExpr++;
		return;
	case ')':
		PPToken = PP_RPAREN;
		PPExpr++;
		return;
	case '!':
		if (PPExpr[1] == '=') {
			PPToken = PP_NE;
			PPExpr += 2;
		} else {
			PPToken = PP_NOT;
			PPExpr++;
		}
		return;
	case '~':
		PPToken = PP_TILDE;
		PPExpr++;
		return;
	case '*':
		PPToken = PP_STAR;
		PPExpr++;
		return;
	case '/':
		PPToken = PP_SLASH;
		PPExpr++;
		return;
	case '%':
		PPToken = PP_MOD;
		PPExpr++;
		return;
	case '+':
		PPToken = PP_PLUS;
		PPExpr++;
		return;
	case '-':
		PPToken = PP_MINUS;
		PPExpr++;
		return;
	case '<':
		if (PPExpr[1] == '<') {
			PPToken = PP_LSHIFT;
			PPExpr += 2;
		} else if (PPExpr[1] == '=') {
			PPToken = PP_LE;
			PPExpr += 2;
		} else {
			PPToken = PP_LT;
			PPExpr++;
		}
		return;
	case '>':
		if (PPExpr[1] == '>') {
			PPToken = PP_RSHIFT;
			PPExpr += 2;
		} else if (PPExpr[1] == '=') {
			PPToken = PP_GE;
			PPExpr += 2;
		} else {
			PPToken = PP_GT;
			PPExpr++;
		}
		return;
	case '=':
		if (PPExpr[1] == '=') {
			PPToken = PP_EQ;
			PPExpr += 2;
		} else {
			/* Assignment not allowed in preprocessor */
			error("'=' not allowed in preprocessor expression", NULL);
			PPToken = PP_EOF;
		}
		return;
	case '&':
		if (PPExpr[1] == '&') {
			PPToken = PP_LOGAND;
			PPExpr += 2;
		} else {
			PPToken = PP_AND;
			PPExpr++;
		}
		return;
	case '^':
		PPToken = PP_XOR;
		PPExpr++;
		return;
	case '|':
		if (PPExpr[1] == '|') {
			PPToken = PP_LOGOR;
			PPExpr += 2;
		} else {
			PPToken = PP_OR;
			PPExpr++;
		}
		return;
	case '?':
		PPToken = PP_QMARK;
		PPExpr++;
		return;
	case ':':
		PPToken = PP_COLON;
		PPExpr++;
		return;
	default:
		/* Unknown character - skip */
		PPExpr++;
		pp_next();
		return;
	}
}

/* Primary: number, identifier, defined(), (expr) */
static long pp_primary(void) {
	long v;
	int y;
	char *mtext;
	
	switch (PPToken) {
	case PP_NUM:
		v = PPValue;
		pp_next();
		return v;
	case PP_IDENT:
		/* Check if it's a defined macro */
		y = findmac(PPIdent);
		if (y != 0) {
			/* Get macro value and try to parse as number */
			mtext = Mtext[y];
			if (mtext && *mtext) {
				/* Parse the macro value as a number */
				v = 0;
				if (mtext[0] == '0' && (mtext[1] == 'x' || mtext[1] == 'X')) {
					/* Hex */
					mtext += 2;
					while (isxdigit(*mtext)) {
						int d;
						if (*mtext >= '0' && *mtext <= '9')
							d = *mtext - '0';
						else if (*mtext >= 'a' && *mtext <= 'f')
							d = *mtext - 'a' + 10;
						else
							d = *mtext - 'A' + 10;
						v = v * 16 + d;
						mtext++;
					}
				} else if (mtext[0] == '0' && mtext[1] >= '0' && mtext[1] <= '7') {
					/* Octal */
					while (*mtext >= '0' && *mtext <= '7') {
						v = v * 8 + (*mtext - '0');
						mtext++;
					}
				} else if (isdigit(*mtext) || (*mtext == '-' && isdigit(mtext[1]))) {
					/* Decimal (possibly negative) */
					int neg = 0;
					if (*mtext == '-') { neg = 1; mtext++; }
					while (isdigit(*mtext)) {
						v = v * 10 + (*mtext - '0');
						mtext++;
					}
					if (neg) v = -v;
				}
				/* else: non-numeric macro value, treat as 0 */
				pp_next();
				return v;
			}
		}
		/* Undefined macro or empty value evaluates to 0 */
		pp_next();
		return 0;
	case PP_DEFINED:
		pp_next();
		if (PPToken == PP_LPAREN) {
			pp_next();
			if (PPToken != PP_IDENT) {
				error("identifier expected after 'defined('", NULL);
				return 0;
			}
			v = findmac(PPIdent) != 0 ? 1 : 0;
			pp_next();
			if (PPToken != PP_RPAREN) {
				error("')' expected after 'defined(name'", NULL);
			} else {
				pp_next();
			}
		} else if (PPToken == PP_IDENT) {
			v = findmac(PPIdent) != 0 ? 1 : 0;
			pp_next();
		} else {
			error("identifier expected after 'defined'", NULL);
			v = 0;
		}
		return v;
	case PP_LPAREN:
		pp_next();
		v = pp_expr();
		if (PPToken != PP_RPAREN) {
			error("')' expected in preprocessor expression", NULL);
		} else {
			pp_next();
		}
		return v;
	default:
		error("unexpected token in preprocessor expression", NULL);
		return 0;
	}
}

/* Unary: !, ~, -, + */
static long pp_unary(void) {
	switch (PPToken) {
	case PP_NOT:
		pp_next();
		return !pp_unary();
	case PP_TILDE:
		pp_next();
		return ~pp_unary();
	case PP_MINUS:
		pp_next();
		return -pp_unary();
	case PP_PLUS:
		pp_next();
		return pp_unary();
	default:
		return pp_primary();
	}
}

/* Multiplicative: *, /, % */
static long pp_multiplicative(void) {
	long v, v2;
	v = pp_unary();
	while (PPToken == PP_STAR || PPToken == PP_SLASH || PPToken == PP_MOD) {
		int op = PPToken;
		pp_next();
		v2 = pp_unary();
		switch (op) {
		case PP_STAR: v = v * v2; break;
		case PP_SLASH: v = v2 ? v / v2 : 0; break;
		case PP_MOD: v = v2 ? v % v2 : 0; break;
		}
	}
	return v;
}

/* Additive: +, - */
static long pp_additive(void) {
	long v;
	v = pp_multiplicative();
	while (PPToken == PP_PLUS || PPToken == PP_MINUS) {
		int op = PPToken;
		pp_next();
		if (op == PP_PLUS)
			v = v + pp_multiplicative();
		else
			v = v - pp_multiplicative();
	}
	return v;
}

/* Shift: <<, >> */
static long pp_shift(void) {
	long v;
	v = pp_additive();
	while (PPToken == PP_LSHIFT || PPToken == PP_RSHIFT) {
		int op = PPToken;
		pp_next();
		if (op == PP_LSHIFT)
			v = v << pp_additive();
		else
			v = v >> pp_additive();
	}
	return v;
}

/* Relational: <, <=, >, >= */
static long pp_relational(void) {
	long v;
	v = pp_shift();
	while (PPToken == PP_LT || PPToken == PP_LE || 
	       PPToken == PP_GT || PPToken == PP_GE) {
		int op = PPToken;
		pp_next();
		switch (op) {
		case PP_LT: v = v < pp_shift(); break;
		case PP_LE: v = v <= pp_shift(); break;
		case PP_GT: v = v > pp_shift(); break;
		case PP_GE: v = v >= pp_shift(); break;
		}
	}
	return v;
}

/* Equality: ==, != */
static long pp_equality(void) {
	long v;
	v = pp_relational();
	while (PPToken == PP_EQ || PPToken == PP_NE) {
		int op = PPToken;
		pp_next();
		if (op == PP_EQ)
			v = v == pp_relational();
		else
			v = v != pp_relational();
	}
	return v;
}

/* Bitwise AND: & */
static long pp_bitand(void) {
	long v;
	v = pp_equality();
	while (PPToken == PP_AND) {
		pp_next();
		v = v & pp_equality();
	}
	return v;
}

/* Bitwise XOR: ^ */
static long pp_bitxor(void) {
	long v;
	v = pp_bitand();
	while (PPToken == PP_XOR) {
		pp_next();
		v = v ^ pp_bitand();
	}
	return v;
}

/* Bitwise OR: | */
static long pp_bitor(void) {
	long v;
	v = pp_bitxor();
	while (PPToken == PP_OR) {
		pp_next();
		v = v | pp_bitxor();
	}
	return v;
}

/* Logical AND: && */
static long pp_logand(void) {
	long v;
	v = pp_bitor();
	while (PPToken == PP_LOGAND) {
		pp_next();
		v = v && pp_bitor();
	}
	return v;
}

/* Logical OR: || */
static long pp_logor(void) {
	long v;
	v = pp_logand();
	while (PPToken == PP_LOGOR) {
		pp_next();
		v = v || pp_logand();
	}
	return v;
}

/* Ternary: ? : */
static long pp_ternary(void) {
	long v, v2, v3;
	v = pp_logor();
	if (PPToken == PP_QMARK) {
		pp_next();
		v2 = pp_expr();
		if (PPToken != PP_COLON) {
			error("':' expected in ternary expression", NULL);
			return v ? v2 : 0;
		}
		pp_next();
		v3 = pp_ternary();
		return v ? v2 : v3;
	}
	return v;
}

/* Top-level expression */
static long pp_expr(void) {
	return pp_ternary();
}

/*
 * Evaluate a preprocessor constant expression
 * Returns non-zero if expression is true
 */
static int pp_eval(char *expr) {
	long result;
	PPExpr = expr;
	pp_next();
	result = pp_expr();
	return result != 0;
}

/*
 * ============================================================================
 * End of Expression Evaluator
 * ============================================================================
 */

void playmac(char *s) {
	if (Mp >= MAXNMAC) fatal("too many nested macros");
	Macc[Mp] = next();
	Macp[Mp++] = s;
}

int getln(char *buf, int max) {
	int	k;

	if (fgets(buf, max, Infile) == NULL) return 0;
	k = strlen(buf);
	if (k) buf[--k] = 0;
	if (k && '\r' == buf[k-1]) buf[--k] = 0;
	return k;
}

/* Helper: strip // comments from macro definition */
static void strip_line_comment(char *buf) {
	char	*p;
	int	in_string = 0, in_char = 0;

	for (p = buf; *p; p++) {
		if (!in_string && !in_char && *p == '/' && p[1] == '/') {
			*p = '\0';
			/* Trim trailing whitespace */
			while (p > buf && isspace(p[-1]))
				*--p = '\0';
			return;
		}
		if (*p == '"' && !in_char)
			in_string = !in_string;
		else if (*p == '\'' && !in_string)
			in_char = !in_char;
		else if (*p == '\\' && p[1])
			p++;  /* Skip escaped char */
	}
}

/*
 * Parse function-like macro parameters
 * Returns number of parameters, -1 on error
 * params[] will contain parameter names (up to MAXMACPARAMS)
 * Format stored: "\x01nparams\x01param1\x01param2\x01...\x01body"
 */
static int parse_macro_params(char *params[], int maxparams) {
	int nparams = 0;
	int c;
	char name[NAMELEN+1];
	int i;
	
	/* Skip opening '(' - already consumed */
	c = skip();
	
	/* Empty parameter list () */
	if (c == ')') {
		return 0;
	}
	
	putback(c);
	
	while (1) {
		/* Get parameter name */
		Token = scanraw();
		if (Token != IDENT) {
			error("identifier expected in macro parameter list", NULL);
			return -1;
		}
		if (nparams >= maxparams) {
			error("too many macro parameters", NULL);
			return -1;
		}
		/* Allocate and copy parameter name */
		params[nparams] = malloc(strlen(Text) + 1);
		if (params[nparams] == NULL) {
			fatal("out of memory for macro parameter");
		}
		strcpy(params[nparams], Text);
		nparams++;
		
		/* Check for comma or closing paren */
		c = skip();
		if (c == ')') {
			break;
		} else if (c == ',') {
			continue;
		} else {
			error("',' or ')' expected in macro parameter list", NULL);
			return -1;
		}
	}
	
	return nparams;
}

static void defmac(void) {
	char	name[NAMELEN+1];
	char	buf[TEXTLEN+1], *p;
	char	macbuf[TEXTLEN+1];
	char	*params[MAXMACPARAMS];
	int	nparams = 0;
	int	y, i, c;
	int	is_funclike = 0;

	Token = scanraw();
	if (Token != IDENT)
		error("identifier expected after '#define': %s", Text);
	copyname(name, Text);
	
	/* Check for function-like macro: '(' immediately after name (no space) */
	c = next();
	if (c == '(') {
		is_funclike = 1;
		nparams = parse_macro_params(params, MAXMACPARAMS);
		if (nparams < 0) {
			/* Error already reported, skip rest of line */
			while (!feof(Infile) && fgetc(Infile) != '\n')
				;
			Line++;
			return;
		}
	} else {
		putback(c);
	}
	
	if ('\n' == Putback)
		buf[0] = 0;
	else
		getln(buf, TEXTLEN-1);
	strip_line_comment(buf);
	for (p = buf; isspace(*p); p++)
		;
	
	if (is_funclike) {
		/* Build macro text with parameter info:
		 * Format: \x01 nparams \x01 param1 \x01 param2 \x01 ... \x01 body
		 */
		char *mp = macbuf;
		*mp++ = '\x01';  /* Marker for function-like macro */
		*mp++ = (char)nparams;
		for (i = 0; i < nparams; i++) {
			*mp++ = '\x01';
			strcpy(mp, params[i]);
			mp += strlen(params[i]);
			free(params[i]);  /* Free allocated param names */
		}
		*mp++ = '\x01';
		strcpy(mp, p);  /* Copy body */
		p = macbuf;
	}
	
	if ((y = findmac(name)) != 0) {
		if (strcmp(Mtext[y], p))
			error("macro redefinition: %s", name);
	}
	else {
		addglob(name, 0, TMACRO, 0, 0, 0, globname(p), 0);
	}
	Line++;
}

static void undef(void) {
	char	name[NAMELEN+1];
	int	y;

	Token = scanraw();
	copyname(name, Text);
	if (IDENT != Token)
		error("identifier expected after '#undef': %s", Text);
	if ((y = findmac(name)) != 0)
		Names[y] = "#undef'd";
}

static FILE *try_open_include(char *file, char *path, int pathlen) {
	int	i;
	FILE	*f;

	/* Try each include directory in order */
	for (i = 0; i < Nincdirs; i++) {
		if (strlen(Incdirs[i]) + strlen(file) + 2 < (size_t)pathlen) {
			strcpy(path, Incdirs[i]);
			strcat(path, "/");
			strcat(path, file);
			if ((f = fopen(path, "r")) != NULL)
				return f;
		}
	}

	/* Try SCCDIR/include as fallback */
	if (strlen(SCCDIR) + strlen(file) + 10 < (size_t)pathlen) {
		strcpy(path, SCCDIR);
		strcat(path, "/include/");
		strcat(path, file);
		if ((f = fopen(path, "r")) != NULL)
			return f;
	}

	return NULL;
}

static void include(void) {
	char	file[TEXTLEN+1], path[TEXTLEN+1];
	int	c, k;
	FILE	*inc, *oinfile;
	char	*ofile;
	int	oc, oline;

	if ((c = skip()) == '<')
		c = '>';
	k = getln(file, TEXTLEN-strlen(SCCDIR)-9);
	Line++;
	if (!k || file[k-1] != c)
		error("missing delimiter in '#include'", NULL);
	if (k) file[k-1] = 0;
	if (c == '"') {
		/* For "file", try current directory first */
		strcpy(path, file);
		inc = fopen(path, "r");
		/* If not found, try include directories */
		if (inc == NULL)
			inc = try_open_include(file, path, TEXTLEN);
	}
	else {
		/* For <file>, search include directories */
		inc = try_open_include(file, path, TEXTLEN);
	}
	if (inc == NULL)
		error("cannot open include file: %s", file);
	else {
		Inclev++;
		oc = next();
		oline = Line;
		ofile = File;
		oinfile = Infile;
		Line = 1;
		putback('\n');
		File = path;
		Infile = inc;
		Token = scan();
		while (XEOF != Token)
			top();
		Line = oline;
		File = ofile;
		Infile = oinfile;
		fclose(inc);
		putback(oc);
		Inclev--;
	}
}

static void ifdef(int expect) {
	char	name[NAMELEN+1];

	if (Isp >= MAXIFDEF)
		fatal("too many nested '#ifdef's");
	Token = scanraw();
	copyname(name, Text);
	if (IDENT != Token)
		error("identifier expected in '#ifdef'", NULL);
	if (frozen(1))
		Ifdefstk[Isp++] = P_IFNDEF;
	else if ((findmac(name) != 0) == expect)
		Ifdefstk[Isp++] = P_IFDEF;
	else
		Ifdefstk[Isp++] = P_IFNDEF;
}

/*
 * #if constant-expression
 */
static void p_if(void) {
	char	buf[TEXTLEN+1];
	int	result;

	if (Isp >= MAXIFDEF)
		fatal("too many nested '#if's");
	if ('\n' == Putback)
		buf[0] = 0;
	else
		getln(buf, TEXTLEN-1);
	Line++;
	if (frozen(1)) {
		Ifdefstk[Isp++] = P_IFNDEF;
	} else {
		result = pp_eval(buf);
		Ifdefstk[Isp++] = result ? P_IFDEF : P_IFNDEF;
	}
}

/*
 * #elif constant-expression
 *
 * Stack states:
 *   P_IFDEF  - a previous condition was true, code is active
 *   P_IFNDEF - no condition has been true yet, code is frozen
 *   P_ELSE   - we're in an #else block that's active (after false conditions)
 *   P_ELSENOT - we're past a true condition, all remaining code is frozen
 */
static void p_elif(void) {
	char	buf[TEXTLEN+1];
	int	result;

	if (!Isp) {
		error("'#elif' without matching '#if'", NULL);
		return;
	}
	if ('\n' == Putback)
		buf[0] = 0;
	else
		getln(buf, TEXTLEN-1);
	Line++;
	if (frozen(2)) {
		/* Outer block is frozen, stay frozen */
		;
	} else if (P_IFDEF == Ifdefstk[Isp-1]) {
		/* Previous #if or #elif was true, skip this and rest */
		Ifdefstk[Isp-1] = P_ELSENOT;
	} else if (P_ELSENOT == Ifdefstk[Isp-1]) {
		/* Already past a true condition, stay frozen */
		;
	} else if (P_IFNDEF == Ifdefstk[Isp-1]) {
		/* No condition true yet, evaluate this one */
		result = pp_eval(buf);
		Ifdefstk[Isp-1] = result ? P_IFDEF : P_IFNDEF;
	} else if (P_ELSE == Ifdefstk[Isp-1]) {
		/* #elif after #else is an error */
		error("'#elif' after '#else'", NULL);
	}
}

static void p_else(void) {
	if (!Isp)
		error("'#else' without matching '#if'", NULL);
	else if (frozen(2))
		;
	else if (P_IFDEF == Ifdefstk[Isp-1])
		Ifdefstk[Isp-1] = P_ELSENOT;
	else if (P_IFNDEF == Ifdefstk[Isp-1])
		Ifdefstk[Isp-1] = P_ELSE;
	else if (P_ELSENOT == Ifdefstk[Isp-1])
		/* Already past a true condition, stay frozen (no error) */
		;
	else if (P_ELSE == Ifdefstk[Isp-1])
		error("'#else' after '#else'", NULL);
	else
		error("'#else' without matching '#if'", NULL);
		
}

static void endif(void) {
	if (!Isp)
		error("'#endif' without matching '#ifdef'", NULL);
	else
		Isp--;
}

static void pperror(void) {
	char	buf[TEXTLEN+1];

	if ('\n' == Putback)
		buf[0] = 0;
	else
		getln(buf, TEXTLEN-1);
	error("#error: %s", buf);
	exit(1);
}

static char FNbuf[TEXTLEN];

static void setline(void) {
	char	buf[TEXTLEN+1], *p, *q;

	if ('\n' == Putback)
		buf[0] = 0;
	else
		getln(buf, TEXTLEN-1);
	Line = atoi(buf) - 1;
	if ((p = strchr(buf, '"')) != NULL) {
		p++;
		if ((q = strchr(p, '"')) != NULL) {
			*q = 0;
			File = strcpy(FNbuf, p);
		}
	}
}

static void junkln(void) {
	while (!feof(Infile) && fgetc(Infile) != '\n')
		;
	Line++;
}

int frozen(int depth) {
	return Isp >= depth &&
		(P_IFNDEF == Ifdefstk[Isp-depth] ||
		P_ELSENOT == Ifdefstk[Isp-depth]);
}

void preproc(void) {
	putback('#');
	Token = scanraw();
	if (	frozen(1) &&
		P_IF != Token && P_IFDEF != Token && P_IFNDEF != Token &&
		P_ELIF != Token && P_ELSE != Token && P_ENDIF != Token
	) {
		junkln();
		return;
	}
	switch (Token) {
	case P_DEFINE:	defmac(); break;
	case P_UNDEF:	undef(); break;
	case P_INCLUDE:	include(); break;
	case P_IF:	p_if(); break;
	case P_IFDEF:	ifdef(1); break;
	case P_IFNDEF:	ifdef(0); break;
	case P_ELIF:	p_elif(); break;
	case P_ELSE:	p_else(); break;
	case P_ENDIF:	endif(); break;
	case P_ERROR:	pperror(); break;
	case P_LINE:	setline(); break;
	case P_PRAGMA:	junkln(); break;
	default:	junkln(); break;
	}
}
