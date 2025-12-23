/*
 *	NMH's Simple C Compiler, 2011--2025
 *	float.h - C89 floating-point limits
 *
 *	IEEE 754 single and double precision values.
 */

#ifndef _FLOAT_H
#define _FLOAT_H

/* Radix of exponent representation */
#define FLT_RADIX	2

/* Number of base-FLT_RADIX digits in the mantissa */
#define FLT_MANT_DIG	24
#define DBL_MANT_DIG	53

/* Number of decimal digits of precision */
#define FLT_DIG		6
#define DBL_DIG		15

/* Minimum negative integer such that FLT_RADIX raised to that power minus 1 is a normalized float */
#define FLT_MIN_EXP	(-125)
#define DBL_MIN_EXP	(-1021)

/* Minimum negative integer such that 10 raised to that power is a normalized float */
#define FLT_MIN_10_EXP	(-37)
#define DBL_MIN_10_EXP	(-307)

/* Maximum integer such that FLT_RADIX raised to that power minus 1 is a representable float */
#define FLT_MAX_EXP	128
#define DBL_MAX_EXP	1024

/* Maximum integer such that 10 raised to that power is a representable float */
#define FLT_MAX_10_EXP	38
#define DBL_MAX_10_EXP	308

/* Maximum representable finite floating-point number */
#define FLT_MAX		3.40282347e+38F
#define DBL_MAX		1.7976931348623157e+308

/* Minimum positive normalized floating-point number */
#define FLT_MIN		1.17549435e-38F
#define DBL_MIN		2.2250738585072014e-308

/* Difference between 1.0 and the least value greater than 1.0 */
#define FLT_EPSILON	1.19209290e-07F
#define DBL_EPSILON	2.2204460492503131e-16

/* Rounding mode for floating-point addition */
#define FLT_ROUNDS	1	/* round to nearest */

#endif /* _FLOAT_H */
