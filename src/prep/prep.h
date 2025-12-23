/*
 * SubC Compiler - Preprocessor Module
 *
 * Modular preprocessor implementation for C89 compliance.
 * Split into multiple files for maintainability.
 *
 * Copyright (c) 2011-2025 - Public Domain (CC0)
 */

#ifndef PREP_H
#define PREP_H

#include "../defs.h"
#include "../data.h"
#include "../decl.h"

/*
 * ============================================================================
 * Preprocessor Expression Evaluator (pp_expr.c)
 * ============================================================================
 */

/* Token types for preprocessor expression */
enum pp_token {
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

/* Expression evaluator state */
extern char	*PPExpr;
extern int	PPToken;
extern long	PPValue;
extern char	PPIdent[NAMELEN+1];

/* Expression evaluator functions */
int pp_eval(char *expr);

/*
 * ============================================================================
 * Macro Management (pp_macro.c)
 * ============================================================================
 */

/* Helper functions */
void strip_line_comment(char *buf);
int parse_macro_params(char *params[], int maxparams);

/* Directive handlers */
void defmac(void);
void undef(void);

/*
 * ============================================================================
 * Conditional Compilation (pp_cond.c)
 * ============================================================================
 */

void ifdef(int expect);
void p_if(void);
void p_elif(void);
void p_else(void);
void endif(void);

/*
 * ============================================================================
 * Include Processing (pp_include.c)
 * ============================================================================
 */

FILE *try_open_include(char *file, char *path, int pathlen);
void include(void);

/*
 * ============================================================================
 * Main Preprocessor (pp_main.c)
 * ============================================================================
 */

void playmac(char *s);
int getln(char *buf, int max);
void pperror(void);
void setline(void);
void junkln(void);
int frozen(int depth);
void preproc(void);

#endif /* PREP_H */
