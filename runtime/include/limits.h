/*
 *	NMH's Simple C Compiler, 2011--2025
 *	limits.h - C89 integer limits
 *
 *	Note: Actual values depend on target architecture.
 *	This header provides common definitions.
 */

#ifndef _LIMITS_H
#define _LIMITS_H

/* Number of bits in a char */
#define CHAR_BIT	8

/* Minimum and maximum values for signed char */
#define SCHAR_MIN	(-128)
#define SCHAR_MAX	127

/* Maximum value for unsigned char */
#define UCHAR_MAX	255

/* Minimum and maximum values for char (SubC: char is unsigned) */
#define CHAR_MIN	0
#define CHAR_MAX	255

/* Minimum and maximum values for short */
#define SHRT_MIN	(-32768)
#define SHRT_MAX	32767

/* Maximum value for unsigned short */
#define USHRT_MAX	65535

/* Minimum and maximum values for int (architecture-dependent) */
#if __SIZEOF_INT__ == 2
#define INT_MIN		(-32768)
#define INT_MAX		32767
#define UINT_MAX	65535U
#elif __SIZEOF_INT__ == 8
#define INT_MIN		(-9223372036854775807L - 1)
#define INT_MAX		9223372036854775807L
#define UINT_MAX	18446744073709551615UL
#else
/* Default: 32-bit int */
#define INT_MIN		(-2147483647 - 1)
#define INT_MAX		2147483647
#define UINT_MAX	4294967295U
#endif

/* Minimum and maximum values for long (architecture-dependent) */
#if __SIZEOF_LONG__ == 8
#define LONG_MIN	(-9223372036854775807L - 1)
#define LONG_MAX	9223372036854775807L
#define ULONG_MAX	18446744073709551615UL
#else
/* Default: 32-bit long */
#define LONG_MIN	(-2147483647L - 1)
#define LONG_MAX	2147483647L
#define ULONG_MAX	4294967295UL
#endif

/* Multibyte character maximum length */
#define MB_LEN_MAX	1

#endif /* _LIMITS_H */
