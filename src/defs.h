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

#define SCCLIBC		"%s/lib/libscc.a"

#define PREFIX		'C'
#define LPREFIX		'L'

#define CHARSIZE	1

#define TEXTLEN		512
#define NAMELEN		16

#define MAXFILES	32
#define MAXINCDIRS	16

#define MAXIFDEF	16
#define MAXNMAC		32
#define MAXCASE		256
#define MAXBREAK	16
#define MAXLOCINIT	32
#define MAXFNARGS	32

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

