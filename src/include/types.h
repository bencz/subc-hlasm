/*
 *	NMH's Simple C Compiler, 2011--2025
 *	Type definitions
 *
 *	Extracted from defs.h for modular organization.
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

/* primitive types */
enum {
	PCHAR = 1,
	PINT,
	CHARPTR,
	INTPTR,
	CHARPP,
	INTPP,
	PVOID,
	VOIDPTR,
	VOIDPP,
	FUNPTR,
	PSTRUCT = 0x2000,
	PUNION  = 0x4000,
	STCPTR  = 0x6000,
	STCPP   = 0x8000,
	UNIPTR  = 0xA000,
	UNIPP   = 0xC000,
	STCMASK = 0xE000
};

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

#endif /* TYPES_H */
