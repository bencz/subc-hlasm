/*
 *	NMH's Simple C Compiler, 2011--2025
 *	stdarg.h - C89 variable arguments
 *
 *	Note: SubC uses a slightly different varargs mechanism.
 *	See varargs.h for the native SubC implementation.
 */

#ifndef _STDARG_H
#define _STDARG_H

typedef void *va_list;

#define va_start(ap, last)	((ap) = _va_start(&(last)))
#define va_arg(ap, type)	((type) _va_arg(&(ap)))
#define va_end(ap)		_va_end(&(ap))

extern void *_va_start(void *last);
extern int _va_arg(void **ap);
extern void _va_end(void **ap);

#endif /* _STDARG_H */
