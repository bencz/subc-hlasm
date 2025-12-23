/*
 *	NMH's Simple C Compiler, 2011--2025
 *	Definitions (Modular Version)
 *
 *	This is the modular version of defs.h that includes
 *	all the separated header files.
 */

#ifndef DEFS_H
#define DEFS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* Target architecture framework */
#include "cgtarget.h"
#include "cgen_compat.h"

/* Modular headers */
#include "types.h"
#include "tokens.h"
#include "ops.h"

#define VERSION		"2025-12-22"

#ifndef SCCDIR
 #define SCCDIR		"."
#endif

#define PREFIX		'C'
#define LPREFIX		'L'

/*
 * Type sizes - use architecture-specific values from cgtarget.h
 * These macros access CG->arch->* for cross-compilation support
 */
#define CHARSIZE	CG_CHARSIZE
#define SHORTSIZE	CG_SHORTSIZE
#define LONGSIZE	CG_LONGSIZE
#define FLOATSIZE	CG_FLOATSIZE
#define DOUBLESIZE	CG_DOUBLESIZE

#define TEXTLEN		512
#define NAMELEN		32

#define MAXFILES	32
#define MAXINCDIRS	16

#define MAXIFDEF	16
#define MAXNMAC		32
#define MAXCASE		1024
#define MAXBREAK	64
#define MAXLOCINIT	128
#define MAXFNARGS	127

/* assert(NSYMBOLS < PSTRUCT) */
#define NSYMBOLS	1024
#define POOLSIZE	16384
#define NODEPOOLSZ	4096	/* ints */

/* debug options */
enum {
	D_LSYM = 1,
	D_GSYM = 2,
	D_STAT = 4
};

/* addressing modes */
enum {
	empty,
	addr_auto,
	addr_static,
	addr_globl,
	addr_label,
	literal,
	auto_byte,
	auto_word,
	static_byte,
	static_word,
	globl_byte,
	globl_word
};

/* compare instructions */
enum {
	cnone,
	equal,
	not_equal,
	less,
	greater,
	less_equal,
	greater_equal,
	below,
	above,
	below_equal,
	above_equal
};

/* boolean instructions */
enum {
	bnone,
	lognot,
	normalize
};

#endif /* DEFS_H */

