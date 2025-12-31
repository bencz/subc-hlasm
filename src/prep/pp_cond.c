/*
 * SubC Compiler - Preprocessor Conditional Compilation
 *
 * Handles #if, #ifdef, #ifndef, #elif, #else, #endif directives.
 */

#include "prep.h"

void ifdef(int expect) {
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
void p_if(void) {
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
void p_elif(void) {
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

void p_else(void) {
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

void endif(void) {
	if (!Isp)
		error("'#endif' without matching '#ifdef'", NULL);
	else
		Isp--;
}
