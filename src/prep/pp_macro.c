/*
 * SubC Compiler - Preprocessor Macro Management
 *
 * Handles #define and #undef directives, including function-like macros.
 */

#include "prep.h"

/* Helper: strip // comments from macro definition */
void strip_line_comment(char *buf) {
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
int parse_macro_params(char *params[], int maxparams) {
	int nparams = 0;
	int c;

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

void defmac(void) {
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

void undef(void) {
	char	name[NAMELEN+1];
	int	y;

	Token = scanraw();
	copyname(name, Text);
	if (IDENT != Token)
		error("identifier expected after '#undef': %s", Text);
	if ((y = findmac(name)) != 0)
		Names[y] = "#undef'd";
}
