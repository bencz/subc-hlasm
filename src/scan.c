/*
 *	NMH's Simple C Compiler, 2011--2021
 *	Lexical analysis (scanner)
 */

#include "defs.h"
#include "data.h"
#include "decl.h"

int next(void) {
	int	c;

	if (Putback) {
		c = Putback;
		Putback = 0;
		return c;
	}
	if (Mp) {
		if ('\0' == *Macp[Mp-1]) {
			Macp[Mp-1] = NULL;
			return Macc[--Mp];
		}
		else {
			return *Macp[Mp-1]++;
		}
	}
	c = fgetc(Infile);
	if ('\n' == c) Line++;
	return c;
}

void putback(int c) {
	Putback = c;
}

static int hexchar(void) {
	int	c, h, n = 0, f = 0;

	while (isxdigit(c = next())) {
		h = chrpos("0123456789abcdef", tolower(c));
		n = n * 16 + h;
		f = 1;
	}
	putback(c);
	if (!f)
		error("missing digits after '\\x'", NULL);
	if (n > 255)
		error("value out of range after '\\x'", NULL);
	return n;
}

static int scanch(void) {
	int	i, c, c2;

	c = next();
	if ('\\' == c) {
		switch (c = next()) {
		case 'a': return '\a';
		case 'b': return '\b';
		case 'f': return '\f';
		case 'n': return '\n';
		case 'r': return '\r';
		case 't': return '\t';
		case 'v': return '\v';
		case '\\': return '\\';
		case '"': return '"' | 256;
		case '\'': return '\'';
		case '0': case '1': case '2':
		case '3': case '4': case '5':
		case '6': case '7':
			for (i = c2 = 0; isdigit(c) && c < '8'; c = next()) {
				if (++i > 3) break;
				c2 = c2 * 8 + (c - '0');
			}
			putback(c);
			return c2;
		case 'x':
			return hexchar();
		default:
			scnerror("unknown escape sequence: %s", c);
			return ' ';
		}
	}
	else {
		return c;
	}
}

/*
 * Scan a numeric literal (integer or floating-point)
 * Returns 0 for integer, 1 for float, 2 for double
 */
static int scannumber(int c, long *ival) {
	long	val;
	int	radix, k, i = 0;
	int	is_float = 0;
	int	has_exp = 0;
	double	fval = 0.0;
	double	frac = 0.1;

	val = 0;
	radix = 10;
	if ('0' == c) {
		Text[i++] = '0';
		c = next();
		if (c == 'x' || c == 'X') {
			radix = 16;
			Text[i++] = c;
			c = next();
		}
		else if (c == '.') {
			/* 0.xxx - decimal float, keep radix=10 */
			radix = 10;
		}
		else if (c == 'e' || c == 'E') {
			/* 0e... - decimal float with exponent */
			radix = 10;
		}
		else {
			radix = 8;
		}
	}
	/* Integer part */
	while ((k = chrpos("0123456789abcdef", tolower(c))) >= 0) {
		Text[i++] = c;
		if (k >= radix)
			scnerror("invalid digit in integer literal: %s", c);
		val = val * radix + k;
		fval = fval * radix + k;
		c = next();
	}
	/* Check for decimal point (only for base 10) */
	if (c == '.' && radix == 10) {
		is_float = 1;
		Text[i++] = c;
		c = next();
		/* Fractional part */
		while (isdigit(c)) {
			Text[i++] = c;
			fval = fval + (c - '0') * frac;
			frac *= 0.1;
			c = next();
		}
	}
	/* Check for exponent (only for base 10) */
	if ((c == 'e' || c == 'E') && radix == 10) {
		int	exp_sign = 1;
		int	exp_val = 0;
		is_float = 1;
		has_exp = 1;
		Text[i++] = c;
		c = next();
		if (c == '+' || c == '-') {
			if (c == '-') exp_sign = -1;
			Text[i++] = c;
			c = next();
		}
		if (!isdigit(c)) {
			scnerror("invalid exponent in floating literal: %s", c);
		}
		while (isdigit(c)) {
			Text[i++] = c;
			exp_val = exp_val * 10 + (c - '0');
			c = next();
		}
		/* Apply exponent */
		exp_val *= exp_sign;
		while (exp_val > 0) { fval *= 10.0; exp_val--; }
		while (exp_val < 0) { fval /= 10.0; exp_val++; }
	}
	/* Check for type suffix */
	if (c == 'f' || c == 'F') {
		is_float = 1;  /* float suffix */
		Text[i++] = c;
		c = next();
		Fvalue = fval;
		putback(c);
		Text[i] = 0;
		*ival = 0;
		return 1;  /* float */
	}
	else if (c == 'l' || c == 'L') {
		Text[i++] = c;
		c = next();
		if (is_float) {
			/* long double - treat as double for now */
			Fvalue = fval;
			putback(c);
			Text[i] = 0;
			*ival = 0;
			return 2;  /* double */
		}
		/* Check for UL or LU suffix */
		if (c == 'u' || c == 'U') {
			Text[i++] = c;
			c = next();
		}
		putback(c);
		Text[i] = 0;
		*ival = val;
		return 0;  /* long integer */
	}
	else if (c == 'u' || c == 'U') {
		Text[i++] = c;
		c = next();
		/* Check for UL suffix */
		if (c == 'l' || c == 'L') {
			Text[i++] = c;
			c = next();
		}
		putback(c);
		Text[i] = 0;
		*ival = val;
		return 0;  /* unsigned integer */
	}
	putback(c);
	Text[i] = 0;
	if (is_float) {
		Fvalue = fval;
		*ival = 0;
		return 2;  /* double (default for floating literals) */
	}
	*ival = val;
	return 0;  /* integer */
}

static long scanint(int c) {
	long	val;
	scannumber(c, &val);
	return val;
}

static int scanstr(char *buf) {
	int	i, c;

	buf[0] = '"';
	for (i=1; i<TEXTLEN-2; i++) {
		if ((c = scanch()) == '"') {
			buf[i++] = '"';
			buf[i] = 0;
			return Value = i;
		}
		buf[i] = c;
	}
	fatal("string literal too long");
	return 0;
}

static int scanident(int c, char *buf, int lim) {
	int	i = 0;

	while (isalpha(c) || isdigit(c) || '_' == c) {
		if (lim-1 == i) {
			error("identifier too long", NULL);
			i++;
		}
		else if (i < lim-1) {
			buf[i++] = c;
		}
		c = next();
	}
	putback(c);
	buf[i] = 0;
	return i;
}

int skip(void) {
	int	c, p, nl;

	c = next();
	nl = 0;
	for (;;) {
		if (EOF == c) {
			strcpy(Text, "<EOF>");
			return EOF;
		}
		while (' ' == c || '\t' == c || '\n' == c ||
			'\r' == c || '\f' == c
		) {
			if ('\n' == c) nl = 1;
			c = next();
		}
		if (nl && c == '#') {
			preproc();
			c = next();
			continue;
		}
		nl = 0;
		if (c != '/')
			break;
		c = next();
		if (c != '*' && c != '/') {
			putback(c);
			c = '/';
			break;
		}
		if (c == '/') {
			while ((c = next()) != EOF) {
				if (c == '\n') break;
			}
                }
                else {
			p = 0;
			while ((c = next()) != EOF) {
				if ('/' == c && '*' == p) {
					c = next();
					break;
				}
				p = c;
			}
		}
	}
	return c;
}

static int keyword(char *s) {
	switch (*s) {
	case '#':
		switch (s[1]) {
		case 'd':
			if (!strcmp(s, "#define")) return P_DEFINE;
			break;
		case 'e':
			if (!strcmp(s, "#else")) return P_ELSE;
			if (!strcmp(s, "#elif")) return P_ELIF;
			if (!strcmp(s, "#endif")) return P_ENDIF;
			if (!strcmp(s, "#error")) return P_ERROR;
			break;
		case 'i':
			if (!strcmp(s, "#if")) return P_IF;
			if (!strcmp(s, "#ifdef")) return P_IFDEF;
			if (!strcmp(s, "#ifndef")) return P_IFNDEF;
			if (!strcmp(s, "#include")) return P_INCLUDE;
			break;
		case 'l':
			if (!strcmp(s, "#line")) return P_LINE;
			break;
		case 'p':
			if (!strcmp(s, "#pragma")) return P_PRAGMA;
			break;
		case 'u':
			if (!strcmp(s, "#undef")) return P_UNDEF;
			break;
		}
		break;
	case 'a':
		if (!strcmp(s, "auto")) return AUTO;
		break;
	case 'b':
		if (!strcmp(s, "break")) return BREAK;
		break;
	case 'c':
		if (!strcmp(s, "case")) return CASE;
		if (!strcmp(s, "char")) return CHAR;
		if (!strcmp(s, "const")) return CONST;
		if (!strcmp(s, "continue")) return CONTINUE;
		break;
	case 'd':
		if (!strcmp(s, "default")) return DEFAULT;
		if (!strcmp(s, "do")) return DO;
		if (!strcmp(s, "double")) return DOUBLE;
		break;
	case 'e':
		if (!strcmp(s, "else")) return ELSE;
		if (!strcmp(s, "enum")) return ENUM;
		if (!strcmp(s, "extern")) return EXTERN;
		break;
	case 'f':
		if (!strcmp(s, "float")) return FLOAT;
		if (!strcmp(s, "for")) return FOR;
		break;
	case 'i':
		if (!strcmp(s, "if")) return IF;
		if (!strcmp(s, "int")) return INT;
		break;
	case 'l':
		if (!strcmp(s, "long")) return LONG;
		break;
	case 'r':
		if (!strcmp(s, "register")) return REGISTER;
		if (!strcmp(s, "return")) return RETURN;
		break;
	case 's':
		if (!strcmp(s, "short")) return SHORT;
		if (!strcmp(s, "signed")) return SIGNED;
		if (!strcmp(s, "sizeof")) return SIZEOF;
		if (!strcmp(s, "static")) return STATIC;
		if (!strcmp(s, "struct")) return STRUCT;
		if (!strcmp(s, "switch")) return SWITCH;
		break;
	case 't':
		if (!strcmp(s, "typedef")) return TYPEDEF;
		break;
	case 'u':
		if (!strcmp(s, "union")) return UNION;
		if (!strcmp(s, "unsigned")) return UNSIGNED;
		break;
	case 'v':
		if (!strcmp(s, "void")) return VOID;
		if (!strcmp(s, "volatile")) return VOLATILE;
		break;
	case 'w':
		if (!strcmp(s, "while")) return WHILE;
		break;
	}
	return 0;
}

/*
 * Expand a function-like macro with arguments
 * mtext format: \x01 nparams \x01 param1 \x01 param2 \x01 ... \x01 body
 */
static char *expand_funclike_macro(char *mtext) {
	static char expanded[TEXTLEN+1];
	char *args[MAXMACPARAMS];
	char argbuf[MAXMACPARAMS][MAXMACARGLEN];
	char *params[MAXMACPARAMS];
	int nparams, nargs, i, c, paren_depth;
	char *p, *body, *out;
	int in_string, in_char;
	
	/* Parse macro definition */
	p = mtext + 1;  /* Skip \x01 marker */
	nparams = (unsigned char)*p++;
	
	/* Extract parameter names */
	for (i = 0; i < nparams; i++) {
		if (*p != '\x01') {
			error("malformed macro definition", NULL);
			return mtext;
		}
		p++;  /* Skip \x01 */
		params[i] = p;
		while (*p && *p != '\x01') p++;
	}
	if (*p != '\x01') {
		error("malformed macro definition", NULL);
		return mtext;
	}
	p++;  /* Skip final \x01 before body */
	body = p;
	
	/* Skip whitespace before '(' */
	c = skip();
	if (c != '(') {
		/* Not a function call, don't expand */
		putback(c);
		return NULL;  /* Signal: don't expand as function-like */
	}
	
	/* Parse arguments */
	nargs = 0;
	paren_depth = 1;
	
	/* Handle empty argument list */
	c = skip();
	if (c == ')') {
		paren_depth = 0;
	} else {
		putback(c);
	}
	
	while (paren_depth > 0) {
		/* Collect one argument */
		char *ap = argbuf[nargs];
		int len = 0;
		in_string = 0;
		in_char = 0;
		
		while (1) {
			c = next();
			if (c == EOF || c == '\n') {
				error("unterminated macro argument", NULL);
				return mtext;
			}
			
			/* Track string/char literals */
			if (!in_char && c == '"') in_string = !in_string;
			else if (!in_string && c == '\'') in_char = !in_char;
			
			if (!in_string && !in_char) {
				if (c == '(') {
					paren_depth++;
				} else if (c == ')') {
					paren_depth--;
					if (paren_depth == 0) break;
				} else if (c == ',' && paren_depth == 1) {
					break;
				}
			}
			
			if (len < MAXMACARGLEN - 1) {
				ap[len++] = c;
			}
		}
		
		/* Trim trailing whitespace */
		while (len > 0 && isspace(ap[len-1])) len--;
		ap[len] = '\0';
		
		/* Trim leading whitespace */
		args[nargs] = ap;
		while (isspace(*args[nargs])) args[nargs]++;
		
		nargs++;
		if (nargs > MAXMACPARAMS) {
			error("too many macro arguments", NULL);
			return mtext;
		}
		
		if (paren_depth == 0) break;
	}
	
	/* Check argument count */
	if (nargs != nparams) {
		error("macro argument count mismatch", NULL);
		return mtext;
	}
	
	/* Expand body, substituting parameters */
	out = expanded;
	p = body;
	while (*p && out < expanded + TEXTLEN - 1) {
		/* Check for parameter reference */
		if (isalpha(*p) || *p == '_') {
			char ident[NAMELEN+1];
			char *ip = ident;
			char *start = p;
			
			while ((isalnum(*p) || *p == '_') && ip < ident + NAMELEN) {
				*ip++ = *p++;
			}
			*ip = '\0';
			
			/* Check if it's a parameter */
			for (i = 0; i < nparams; i++) {
				/* Compare with parameter name (null-terminated by \x01) */
				char *pn = params[i];
				char *id = ident;
				while (*pn && *pn != '\x01' && *id && *pn == *id) {
					pn++; id++;
				}
				if ((*pn == '\0' || *pn == '\x01') && *id == '\0') {
					/* Found parameter, substitute argument */
					char *arg = args[i];
					while (*arg && out < expanded + TEXTLEN - 1) {
						*out++ = *arg++;
					}
					break;
				}
			}
			if (i == nparams) {
				/* Not a parameter, copy identifier */
				char *id = ident;
				while (*id && out < expanded + TEXTLEN - 1) {
					*out++ = *id++;
				}
			}
		} else {
			*out++ = *p++;
		}
	}
	*out = '\0';
	
	return expanded;
}

static int macro(char *name) {
	int	y;
	char	*mtext, *expanded;

	y = findmac(name);
	if (!y || Types[y] != TMACRO)
		return 0;
	
	mtext = Mtext[y];
	
	/* Check if function-like macro (starts with \x01) */
	if (mtext && mtext[0] == '\x01') {
		expanded = expand_funclike_macro(mtext);
		if (expanded == NULL) {
			/* No '(' found, don't expand */
			return 0;
		}
		playmac(expanded);
	} else {
		playmac(mtext);
	}
	return 1;
}

static int scanpp(void) {
	int	c, t;

	if (Rejected != -1) {
		t = Rejected;
		Rejected = -1;
		strcpy(Text, Rejtext);
		Value = Rejval;
		return t;
	}
	for (;;) {
		Value = 0;
		c = skip();
		memset(Text, 0, 4);
		Text[0] = c;
		switch (c) {
		case '!':
			if ((c = next()) == '=') {
				Text[1] = '=';
				return NOTEQ;
			}
			else {
				putback(c);
				return XMARK;
			}
		case '%':
			if ((c = next()) == '=') {
				Text[1] = '=';
				return ASMOD;
			}
			else {
				putback(c);
				return MOD;
			}
		case '&':
			if ((c = next()) == '&') {
				Text[1] = '&';
				return LOGAND;
			}
			else if ('=' == c) {
				Text[1] = '=';
				return ASAND;
			}
			else {
				putback(c);
				return AMPER;
			}
		case '(':
			return LPAREN;
		case ')':
			return RPAREN;
		case '*':
			if ((c = next()) == '=') {
				Text[1] = '=';
				return ASMUL;
			}
			else {
				putback(c);
				return STAR;
			}
		case '+':
			if ((c = next()) == '+') {
				Text[1] = '+';
				return INCR;
			}
			else if ('=' == c) {
				Text[1] = '=';
				return ASPLUS;
			}
			else {
				putback(c);
				return PLUS;
			}
		case ',':
			return COMMA;
		case '-':
			if ((c = next()) == '-') {
				Text[1] = '-';
				return DECR;
			}
			else if ('=' == c) {
				Text[1] = '=';
				return ASMINUS;
			}
			else if ('>' == c) {
				Text[1] = '>';
				return ARROW;
			}
			else {
				putback(c);
				return MINUS;
			}
		case '/':
			if ((c = next()) == '=') {
				Text[1] = '=';
				return ASDIV;
			}
			else {
				putback(c);
				return SLASH;
			}
		case ':':
			return COLON;
		case ';':
			return SEMI;
		case '<':
			if ((c = next()) == '<') {
				Text[1] = '<';
				if ((c = next()) == '=') {
					Text[2] = '=';
					return ASLSHIFT;
				}
				else {
					putback(c);
					return LSHIFT;
				}
			}
			else if ('=' == c) {
				Text[1] = '=';
				return LTEQ;
			}
			else {
				putback(c);
				return LESS;
			}
		case '=':
			if ((c = next()) == '=') {
				Text[1] = '=';
				return EQUAL;
			}
			else {
				putback(c);
				return ASSIGN;
			}
		case '>':
			if ((c = next()) == '>') {
				Text[1] = '>';
				if ((c = next()) == '=') {
					Text[1] = '=';
					return ASRSHIFT;
				}
				else {
					putback(c);
					return RSHIFT;
				}
			}
			else if ('=' == c) {
				Text[1] = '=';
				return GTEQ;
			}
			else {
				putback(c);
				return GREATER;
			}
		case '?':
			return QMARK;
		case '[':
			return LBRACK;
		case ']':
			return RBRACK;
		case '^':
			if ((c = next()) == '=') {
				Text[1] = '=';
				return ASXOR;
			}
			else {
				putback(c);
				return CARET;
			}
		case '{':
			return LBRACE;
		case '|':
			if ((c = next()) == '|') {
				Text[1] = '|';
				return LOGOR;
			}
			else if ('=' == c) {
				Text[1] = '=';
				return ASOR;
			}
			else {
				putback(c);
				return PIPE;
			}
		case '}':
			return RBRACE;
		case '~':
			return TILDE;
		case EOF:
			strcpy(Text, "<EOF>");
			return XEOF;
		case '\'':
			Text[1] = Value = scanch();
			if ((c = next()) != '\'')
				error(
				 "expected '\\'' at end of char literal",
					NULL);
			Text[2] = '\'';
			return INTLIT;
		case '"':
			Value = scanstr(Text);
			return STRLIT;
		case '#':
			Text[0] = '#';
			scanident(next(), &Text[1], TEXTLEN-1);
			if ((t = keyword(Text)) != 0)
				return t;
			error("unknown preprocessor command: %s", Text);
			return IDENT;
		case '.':
			if ((c = next()) == '.') {
				Text[1] = Text[2] = '.';
				Text[3] = 0;
				if ((c = next()) == '.')
					return ELLIPSIS;
				putback(c);
				error("incomplete '...'", NULL);
				return ELLIPSIS;
			}
			/* Check for floating literal starting with '.' (e.g., .5) */
			if (isdigit(c)) {
				putback(c);
				putback('.');
				putback('0');
				c = next();
				scannumber(c, &Value);
				return FLOATLIT;
			}
			putback(c);
			return DOT;
		default:
			if (isdigit(c)) {
				int numtype = scannumber(c, &Value);
				if (numtype > 0) {
					return FLOATLIT;
				}
				return INTLIT;
			}
			else if (isalpha(c) || '_' == c) {
				Value = scanident(c, Text, TEXTLEN);
				if (Expandmac && macro(Text))
					break;
				if ((t = keyword(Text)) != 0)
					return t;
				return IDENT;
			}
			else {
				scnerror("funny input character: %s", c);
				break;
			}
		}
	}
}

int scan(void) {
	int	t;

	do {
		t = scanpp();
		if (!Inclev && Isp && XEOF == t)
			fatal("missing '#endif'");
	} while (frozen(1));
	if (t == Syntoken)
		Syntoken = 0;
	return t;
}

int scanraw(void) {
	int	t, oisp;

	oisp = Isp;
	Isp = 0;
	Expandmac = 0;
	t = scan();
	Expandmac = 1;
	Isp = oisp;
	return t;
}

void reject(void) {
	Rejected = Token;
	Rejval = Value;
	strcpy(Rejtext, Text);
}
