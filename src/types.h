/*
 *	NMH's Simple C Compiler, 2011--2025
 *	Type definitions
 *
 *	Extended type system supporting all C89 types:
 *	- char, signed char, unsigned char
 *	- short, unsigned short
 *	- int, unsigned int
 *	- long, unsigned long
 *	- float, double
 */

#ifndef TYPES_H
#define TYPES_H

/* types */
enum {
	TVARIABLE = 1,
	TARRAY,
	TFUNCTION,
	TCONSTANT,
	TMACRO,
	TSTRUCT
};

/*
 * Primitive types encoding:
 *
 * Bits 0-7:   Base type
 * Bits 8-9:   Pointer level (0=value, 1=ptr, 2=ptr-to-ptr)
 * Bit 10:     Unsigned flag
 * Bits 13-15: Struct/Union mask (preserved for compatibility)
 *
 * Base types (0-31):
 *   PCHAR=1, PINT=2, PSHORT=3, PLONG=4, PFLOAT=5, PDOUBLE=6, PVOID=7
 *
 * For backward compatibility, the original enum values are preserved
 * and new types are added with higher values.
 */

/* Base primitive types */
enum {
	/* Original types - preserved for compatibility */
	PCHAR = 1,          /* signed char (C89: plain char is implementation-defined) */
	PINT,               /* signed int */
	CHARPTR,            /* char* */
	INTPTR,             /* int* */
	CHARPP,             /* char** */
	INTPP,              /* int** */
	PVOID,              /* void */
	VOIDPTR,            /* void* */
	VOIDPP,             /* void** */
	FUNPTR,             /* int(*)() - function pointer */

	/* New C89 types */
	PUCHAR,             /* unsigned char */
	PSCHAR,             /* signed char (explicit) */
	PSHORT,             /* short (signed short int) */
	PUSHORT,            /* unsigned short */
	PUINT,              /* unsigned int */
	PLONG,              /* long (signed long int) */
	PULONG,             /* unsigned long */
	PFLOAT,             /* float */
	PDOUBLE,            /* double */

	/* Pointers to new types */
	UCHARPTR,           /* unsigned char* */
	SCHARPTR,           /* signed char* */
	SHORTPTR,           /* short* */
	USHORTPTR,          /* unsigned short* */
	UINTPTR,            /* unsigned int* */
	LONGPTR,            /* long* */
	ULONGPTR,           /* unsigned long* */
	FLOATPTR,           /* float* */
	DOUBLEPTR,          /* double* */

	/* Pointer-to-pointer for new types */
	UCHARPP,            /* unsigned char** */
	SCHARPP,            /* signed char** */
	SHORTPP,            /* short** */
	USHORTPP,           /* unsigned short** */
	UINTPP,             /* unsigned int** */
	LONGPP,             /* long** */
	ULONGPP,            /* unsigned long** */
	FLOATPP,            /* float** */
	DOUBLEPP,           /* double** */

	/* Struct/Union types - must be high values for masking */
	PSTRUCT = 0x2000,
	PUNION  = 0x4000,
	STCPTR  = 0x6000,
	STCPP   = 0x8000,
	UNIPTR  = 0xA000,
	UNIPP   = 0xC000,
	STCMASK = 0xE000
};

/*
 * Type classification macros
 */

/* Check if type is an integer type (includes char, short, int, long, signed/unsigned) */
#define IS_INTTYPE(p) \
	((p) == PCHAR || (p) == PINT || (p) == PUCHAR || (p) == PSCHAR || \
	 (p) == PSHORT || (p) == PUSHORT || (p) == PUINT || \
	 (p) == PLONG || (p) == PULONG)

/* Check if type is unsigned */
#define IS_UNSIGNED(p) \
	((p) == PUCHAR || (p) == PUSHORT || (p) == PUINT || (p) == PULONG)

/* Check if type is a floating-point type */
#define IS_FLOATTYPE(p) \
	((p) == PFLOAT || (p) == PDOUBLE)

/* Check if type is arithmetic (integer or floating-point) */
#define IS_ARITHTYPE(p) \
	(IS_INTTYPE(p) || IS_FLOATTYPE(p))

/* Check if type is a pointer */
#define IS_PTRTYPE(p) \
	((p) == CHARPTR || (p) == INTPTR || (p) == VOIDPTR || (p) == FUNPTR || \
	 (p) == UCHARPTR || (p) == SCHARPTR || (p) == SHORTPTR || \
	 (p) == USHORTPTR || (p) == UINTPTR || (p) == LONGPTR || \
	 (p) == ULONGPTR || (p) == FLOATPTR || (p) == DOUBLEPTR || \
	 (((p) & STCMASK) == STCPTR) || (((p) & STCMASK) == UNIPTR))

/* Check if type is pointer-to-pointer */
#define IS_PPTYPE(p) \
	((p) == CHARPP || (p) == INTPP || (p) == VOIDPP || \
	 (p) == UCHARPP || (p) == SCHARPP || (p) == SHORTPP || \
	 (p) == USHORTPP || (p) == UINTPP || (p) == LONGPP || \
	 (p) == ULONGPP || (p) == FLOATPP || (p) == DOUBLEPP || \
	 (((p) & STCMASK) == STCPP) || (((p) & STCMASK) == UNIPP))

/* Get the base type (remove pointer levels) */
#define BASE_TYPE(p) \
	(IS_PPTYPE(p) ? deref(deref(p)) : IS_PTRTYPE(p) ? deref(p) : (p))

/* storage classes */
enum {
	CPUBLIC = 1,
	CEXTERN,
	CSTATIC,
	CLSTATC,
	CAUTO,
	CSPROTO,
	CMEMBER,
	CSTCDEF,
	CTYPE
};

/* lvalue structure */
enum {
	LVSYM,
	LVPRIM,
	LVADDR,
	LV
};

/* AST node */
struct node_stc {
	int		op;
	struct node_stc	*left, *right;
	long		args[1];
};

#define node	struct node_stc

#endif /* TYPES_H */
