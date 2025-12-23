/*
 * SubC Compiler - IEEE 754 Floating-Point Emulation Library Header
 *
 * This header declares the software floating-point emulation functions
 * for 8086 systems without an 8087 coprocessor.
 *
 * Copyright (c) 2025 - Public Domain (CC0)
 */

#ifndef FPEMU_H
#define FPEMU_H

/*
 * Float (32-bit) Operations
 * Float is passed/returned in DX:AX (DX=high 16 bits, AX=low 16 bits)
 * Second operand for binary ops is in CX:BX
 */

/* Negation */
void __fpneg(void);     /* DX:AX = -DX:AX */

/* Comparison - returns -1, 0, or 1 in AX */
void __fpcmp(void);     /* Compare DX:AX with CX:BX */

/* Relational comparisons - return 0 or 1 in AX */
void __fpeq(void);      /* DX:AX == CX:BX */
void __fpne(void);      /* DX:AX != CX:BX */
void __fplt(void);      /* DX:AX < CX:BX */
void __fpgt(void);      /* DX:AX > CX:BX */
void __fple(void);      /* DX:AX <= CX:BX */
void __fpge(void);      /* DX:AX >= CX:BX */

/* Arithmetic */
void __fpadd(void);     /* DX:AX = DX:AX + CX:BX */
void __fpsub(void);     /* DX:AX = DX:AX - CX:BX */
void __fpmul(void);     /* DX:AX = DX:AX * CX:BX */
void __fpdiv(void);     /* DX:AX = DX:AX / CX:BX */

/* Conversions */
void __itof(void);      /* AX (int) -> DX:AX (float) */
void __ftoi(void);      /* DX:AX (float) -> AX (int) */

/*
 * Double (64-bit) Operations
 * Double is passed on the stack (8 bytes)
 * Result is also on the stack
 */

/* Negation */
void __dpneg(void);     /* Negate double on stack */

/* Comparison */
void __dpcmp(void);     /* Compare two doubles on stack */

/* Relational comparisons - return 0 or 1 in AX */
void __dpeq(void);
void __dpne(void);
void __dplt(void);
void __dpgt(void);
void __dple(void);
void __dpge(void);

/* Arithmetic */
void __dpadd(void);     /* Add two doubles on stack */
void __dpsub(void);     /* Subtract */
void __dpmul(void);     /* Multiply */
void __dpdiv(void);     /* Divide */

/* Conversions */
void __itod(void);      /* AX (int) -> double on stack */
void __dtoi(void);      /* double on stack -> AX (int) */
void __stod(void);      /* DX:AX (float) -> double on stack */
void __dtos(void);      /* double on stack -> DX:AX (float) */

#endif /* FPEMU_H */
