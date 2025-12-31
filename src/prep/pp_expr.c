/*
 * SubC Compiler - Preprocessor Expression Evaluator
 *
 * Evaluates constant expressions for #if and #elif directives.
 * Supports: integers, defined(), !, ~, *, /, %, +, -, <<, >>,
 *           <, <=, >, >=, ==, !=, &, ^, |, &&, ||, ?:
 */

#include "prep.h"

/* Expression evaluator state - global for this module */
char	*PPExpr;
int	PPToken;
long	PPValue;
char	PPIdent[NAMELEN+1];

/* Forward declarations */
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
int pp_eval(char *expr) {
	long result;
	PPExpr = expr;
	pp_next();
	result = pp_expr();
	return result != 0;
}
