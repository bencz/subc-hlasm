/*
 *	NMH's Simple C Compiler, 2011--2025
 *	stddef.h
 */

#ifndef _STDDEF_H
#define _STDDEF_H

typedef int		ptrdiff_t;
typedef unsigned int	size_t;

#define NULL		((void *)0)

#define offsetof(type, member)	((size_t) &((type *)0)->member)

#endif
