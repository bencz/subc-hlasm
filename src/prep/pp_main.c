/*
 * SubC Compiler - Preprocessor Main Module
 *
 * Main preprocessor entry points and utility functions.
 */

#include "prep.h"

void playmac(char *s) {
	if (Mp >= MAXNMAC) fatal("too many nested macros");
	Macc[Mp] = next();
	Macp[Mp++] = s;
}

int getln(char *buf, int max) {
	int	k, total = 0;
	char	*p = buf;

	while (total < max - 1) {
		if (fgets(p, max - total, Infile) == NULL) {
			if (total == 0) return 0;
			break;
		}
		k = strlen(p);
		if (k) p[--k] = 0;  /* Remove newline */
		if (k && '\r' == p[k-1]) p[--k] = 0;  /* Remove CR */
		
		/* Check for line continuation (backslash at end) */
		if (k > 0 && p[k-1] == '\\') {
			p[k-1] = ' ';  /* Replace backslash with space */
			p += k;
			total += k;
			Line++;  /* Count the continued line */
		} else {
			total += k;
			break;
		}
	}
	return total;
}

void pperror(void) {
	char	buf[TEXTLEN+1];

	if ('\n' == Putback)
		buf[0] = 0;
	else
		getln(buf, TEXTLEN-1);
	error("#error: %s", buf);
	exit(1);
}

static char FNbuf[TEXTLEN];

void setline(void) {
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

void junkln(void) {
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
