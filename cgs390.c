/*
 * ==========================================================================
 * SubC Compiler - S/390 HLASM Code Generator
 * ==========================================================================
 *
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to <https://unlicense.org>
 *
 * ==========================================================================
 *
 * Original SubC Compiler by Nils M Holm, 2011, 2012, 2022
 * S/390 HLASM backend by Alexandre Bencz, 2025
 *
 * ==========================================================================
 *
 * TARGET ARCHITECTURE: IBM System/390 (S/390)
 * ------------------------------------------
 * - Addressing mode: 31-bit (AMODE 31)
 * - Residency mode:  31-bit (RMODE 31) or 24-bit (RMODE 24)
 * - Byte order:      Big-endian
 * - Word size:       32 bits (fullword)
 * - Pointer size:    4 bytes (31 bits effective address)
 * - Alignment:       Fullword (4 bytes) for integers
 *
 * This code generator produces HLASM (High Level Assembler) source code
 * compatible with IBM OS/390, z/OS, and MVS systems. The generated code
 * uses ESA/390 instructions (AHI, LHI) which are available on all
 * System/390 and later processors.
 *
 * For pure System/370 compatibility, define S390_COMPAT_370 to use
 * only S/370 base instructions (no AHI/LHI).
 *
 * CALLING CONVENTION (Standard OS/390 Linkage):
 * ---------------------------------------------
 * This compiler implements the standard OS/390 linkage convention:
 *
 * CALLER responsibilities:
 * - Build parameter list: array of fullword addresses pointing to parameters
 * - Set high-order bit (bit 0) of last parameter address to indicate end
 * - Load R1 with address of parameter list
 * - Load R15 with address of called routine (for BALR/BASR)
 * - Load R13 with address of caller's 72-byte save area
 * - Branch via BALR R14,R15 or BAS R14,routine
 *
 * CALLEE responsibilities:
 * - Save registers R14-R12 at offset 12 in caller's save area (pointed by R13)
 * - Establish addressability (load base register)
 * - Obtain own save area and chain it to caller's
 * - Access parameters via R1 (each entry is address of actual parameter)
 * - Place return value in R15 (or R0:R1 for 64-bit values)
 * - Restore registers and return via BR R14
 *
 * SAVE AREA LAYOUT (18 fullwords = 72 bytes):
 *   Offset  Contents
 *   0       Reserved (used by PL/I)
 *   4       Address of previous save area (back chain)
 *   8       Address of next save area (forward chain)
 *   12      R14 (return address)
 *   16      R15 (entry point address)
 *   20      R0
 *   24      R1
 *   ...
 *   68      R12
 *
 * PARAMETER LIST FORMAT:
 *   +0   A(parm1)      Address of first parameter
 *   +4   A(parm2)      Address of second parameter
 *   ...
 *   +n   X'80',AL3(parmN)  Last parameter (high bit set)
 *
 * REGISTER USAGE:
 * --------------
 * R0      - Work register (volatile, not preserved across calls)
 * R1      - Parameter list pointer (volatile, standard OS/390 linkage)
 * R2      - Primary accumulator (internal use for expression evaluation)
 * R3      - Secondary operand register (internal use)
 * R4-R5   - Register pair for multiply/divide operations
 * R6-R10  - Callee-saved temporaries (preserved across calls)
 * R11     - Local frame base pointer (points to local variables)
 * R12     - Program base register (addressability via USING)
 * R13     - Save area chain pointer (standard OS/390 linkage)
 * R14     - Return address register
 * R15     - Entry point / return value / stack pointer
 *
 * STACK FRAME LAYOUT (Dynamic Storage Area):
 * -----------------------------------------
 *         +----------------------+ High addresses
 *         | Caller's save area   | (72 bytes, pointed by old R13)
 *         +----------------------+
 *         | Parameter list       | (built by caller before call)
 *         +----------------------+
 *         | Our save area        | (72 bytes)
 *         +----------------------+ <- R13 (our save area)
 *         | Local variables      |
 *         +----------------------+ <- R11 (frame pointer)
 *         | Dynamic stack area   |
 *         +----------------------+ <- R15 (stack pointer)
 *         | ...                  | Low addresses
 *
 * HLASM SOURCE FORMAT:
 * -------------------
 * Columns 1-8:   Label (optional, max 8 characters)
 * Column 9:      Blank (space)
 * Columns 10-14: Operation mnemonic
 * Column 15:     Blank (space)
 * Columns 16-71: Operands and comments
 * Column 72:     Continuation character (if needed)
 * Columns 73-80: Sequence number (optional, ignored)
 *
 * ==========================================================================
 */

#include "defs.h"
#include "data.h"
#include "decl.h"
#include "cgen.h"

/*
 * Compatibility mode selection:
 * - Define S390_COMPAT_370 for pure System/370 compatibility (no AHI/LHI)
 * - By default, ESA/390 instructions are used for better performance
 */
#ifndef S390_COMPAT_370
#define S390_USE_ESA390 1
#endif

/* ========================================================================
 * SECTION DIRECTIVES
 * ========================================================================
 * These functions emit comments to mark data and code sections.
 * In HLASM, we use a single CSECT, so these are informational only.
 */
void cgdata(void)	{ genraw("*\n* DATA SECTION\n*\n"); }
void cgtext(void)	{ genraw("*\n* CODE SECTION\n*\n"); }

/*
 * cgprelude - Emit program prologue
 *
 * Generates the HLASM program header including:
 * - CSECT definition for the program
 * - AMODE/RMODE for 31-bit addressing
 * - Register equates for symbolic register names
 * - Switch dispatch routine (@SWITCH) for switch/case statements
 */
void cgprelude(void) {
	genraw("*=====================================================================\n");
	genraw("*\n");
	genraw("* SubC Compiler Output - IBM System/390 HLASM\n");
	genraw("*\n");
#ifdef S390_USE_ESA390
	genraw("* Target Architecture: ESA/390, z/Architecture\n");
	genraw("* Instruction Set:     ESA/390 (uses AHI, LHI)\n");
#else
	genraw("* Target Architecture: System/370 Compatible\n");
	genraw("* Instruction Set:     S/370 Base (no AHI, LHI)\n");
#endif
	genraw("* Addressing Mode:     31-bit (AMODE 31)\n");
	genraw("* Linkage Convention:  OS/390 Standard Linkage\n");
	genraw("*\n");
	genraw("* Register Usage:\n");
	genraw("*   R0-R1   Work registers (volatile)\n");
	genraw("*   R1      Parameter list pointer on entry\n");
	genraw("*   R2-R3   Expression evaluation\n");
	genraw("*   R4-R5   Multiply/divide register pair\n");
	genraw("*   R6-R10  Preserved across calls\n");
	genraw("*   R11     Local frame base pointer\n");
	genraw("*   R12     Program base register\n");
	genraw("*   R13     Save area chain pointer\n");
	genraw("*   R14     Return address\n");
	genraw("*   R15     Entry point / Return value / Stack\n");
	genraw("*\n");
	genraw("*=====================================================================\n");
	genraw("*\n");
	genraw("         PRINT NOGEN\n");
	genraw("SUBC     CSECT\n");
	genraw("SUBC     AMODE 31\n");
	genraw("SUBC     RMODE ANY\n");
	genraw("*\n");
	genraw("*---------------------------------------------------------------------\n");
	genraw("* Register Equates\n");
	genraw("*---------------------------------------------------------------------\n");
	genraw("R0       EQU   0                    Work register (volatile)\n");
	genraw("R1       EQU   1                    Parameter list / Work (volatile)\n");
	genraw("R2       EQU   2                    Primary accumulator\n");
	genraw("R3       EQU   3                    Secondary operand\n");
	genraw("R4       EQU   4                    Multiply/divide (even reg)\n");
	genraw("R5       EQU   5                    Multiply/divide (odd reg)\n");
	genraw("R6       EQU   6                    Preserved\n");
	genraw("R7       EQU   7                    Preserved\n");
	genraw("R8       EQU   8                    Preserved\n");
	genraw("R9       EQU   9                    Preserved\n");
	genraw("R10      EQU   10                   Preserved\n");
	genraw("R11      EQU   11                   Frame base pointer\n");
	genraw("R12      EQU   12                   Program base register\n");
	genraw("R13      EQU   13                   Save area chain pointer\n");
	genraw("R14      EQU   14                   Return address\n");
	genraw("R15      EQU   15                   Entry/Return/Stack pointer\n");
	genraw("*\n");
	genraw("*---------------------------------------------------------------------\n");
	genraw("* Runtime: Switch/Case Dispatch Routine\n");
	genraw("*---------------------------------------------------------------------\n");
	genraw("* Entry:  R2 = switch expression value\n");
	genraw("*         R4 = address of case table\n");
	genraw("*\n");
	genraw("* Case table format:\n");
	genraw("*         DC    F'n'         Number of case entries\n");
	genraw("*         DC    F'value1'    First case value\n");
	genraw("*         DC    A(label1)    First case branch target\n");
	genraw("*         ...                (repeated for each case)\n");
	genraw("*         DC    A(default)   Default branch target\n");
	genraw("*---------------------------------------------------------------------\n");
	genraw("@SWITCH  DS    0H\n");
	genraw("         L     R0,0(,R4)           Load number of cases\n");
	genraw("         LA    R1,4(,R4)           R1 -> first case entry\n");
	genraw("@SWL     C     R2,0(,R1)           Compare value with case\n");
	genraw("         BE    @SWF                Match found, branch\n");
	genraw("         LA    R1,8(,R1)           Advance to next entry\n");
	genraw("         BCT   R0,@SWL             Loop until all checked\n");
	genraw("         L     R1,0(,R1)           No match: load default addr\n");
	genraw("         BR    R1                  Branch to default\n");
	genraw("@SWF     L     R1,4(,R1)           Load matched case address\n");
	genraw("         BR    R1                  Branch to case code\n");
	genraw("*\n");
	genraw("*=====================================================================\n");
	genraw("* Program Code\n");
	genraw("*=====================================================================\n");
	genraw("*\n");
}

/*
 * cgpostlude - Emit program epilogue
 *
 * Generates the HLASM program trailer including:
 * - LTORG directive to place literal pool
 * - END directive to mark end of assembly
 */
void cgpostlude(void) {
	genraw("*\n");
	genraw("* -----------------------------------------------------------------\n");
	genraw("* Literal Pool\n");
	genraw("* -----------------------------------------------------------------\n");
	genraw("         LTORG\n");
	genraw("*\n");
	genraw("         END\n");
}

/*
 * cgpublic - Declare a symbol as externally visible
 *
 * Emits an ENTRY directive to make the symbol available for external linkage.
 */
void cgpublic(char *s) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         ENTRY %s\n", s);
}

/* ========================================================================
 * LABEL AND NAME GENERATION
 * ========================================================================
 */

/*
 * genlab - Generate a numeric label
 *
 * Creates a label in HLASM format using the LPREFIX character followed
 * by the numeric ID. Labels are padded to 8 characters (HLASM requirement).
 * DS 0H ensures halfword alignment without reserving storage.
 */
void genlab(int id) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "%c%-7d DS    0H\n", LPREFIX, id);
}

/*
 * genname - Generate a symbol name label
 *
 * Creates a named label for functions and global variables.
 * The name is prefixed with PREFIX character to avoid conflicts
 * with HLASM reserved words.
 */
void genname(char *name) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "%-8s", gsym(name));
}

/* ========================================================================
 * LOAD OPERATIONS
 * ========================================================================
 * These functions load values into the primary accumulator (R2).
 */

/*
 * cglit - Load a literal (immediate) value into R2
 *
 * For small values (-32768 to 32767), uses LHI (Load Halfword Immediate)
 * which is more efficient. For larger values, loads from a literal pool.
 *
 * Note: LHI is an ESA/390 instruction. For S/370 compatibility,
 * define S390_COMPAT_370 to always use literal pool loads.
 */
void cglit(int v) {
	if (NULL == Outfile) return;
#ifdef S390_USE_ESA390
	if (v >= -32768 && v <= 32767) {
		fprintf(Outfile, "         LHI   R2,%d\n", v);
	} else {
		fprintf(Outfile, "         L     R2,=F'%d'\n", v);
	}
#else
	/* S/370 compatible: always use literal pool */
	fprintf(Outfile, "         L     R2,=F'%d'\n", v);
#endif
}

/*
 * cgclear - Clear the accumulator (R2 = 0)
 *
 * Uses SR (Subtract Register) which is the most efficient way
 * to zero a register on S/390.
 */
void cgclear(void) {
	gen("SR    R2,R2");
}

/*
 * cgldgb - Load global byte into R2
 *
 * First clears R2, then uses IC (Insert Character) to load
 * the byte into the low-order position. This performs zero-extension.
 */
void cgldgb(char *s) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         SR    R2,R2\n");
	fprintf(Outfile, "         IC    R2,%s\n", s);
}

/*
 * cgldgw - Load global word (fullword) into R2
 */
void cgldgw(char *s) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         L     R2,%s\n", s);
}

/*
 * cgldlb - Load local byte into R2
 *
 * Loads a byte from the local stack frame at offset n from R11.
 * Zero-extends the byte to a fullword.
 */
void cgldlb(int n) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         SR    R2,R2\n");
	fprintf(Outfile, "         IC    R2,%d(,R11)\n", n);
}

/*
 * cgldlw - Load local word (fullword) into R2
 *
 * Loads a fullword from the local stack frame at offset n from R11.
 */
void cgldlw(int n) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         L     R2,%d(,R11)\n", n);
}

/*
 * cgldsb - Load static byte into R2
 *
 * Loads a byte from a static variable (identified by label number).
 * Zero-extends the byte to a fullword.
 */
void cgldsb(int n) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         SR    R2,R2\n");
	fprintf(Outfile, "         IC    R2,%c%d\n", LPREFIX, n);
}

/*
 * cgldsw - Load static word (fullword) into R2
 */
void cgldsw(int n) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         L     R2,%c%d\n", LPREFIX, n);
}

/*
 * cgldla - Load address of local variable into R2
 *
 * Computes the effective address of a local variable using LA.
 * The variable is at offset n from the frame pointer (R11).
 */
void cgldla(int n) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         LA    R2,%d(,R11)\n", n);
}

/*
 * cgldsa - Load address of static variable into R2
 */
void cgldsa(int n) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         LA    R2,%c%d\n", LPREFIX, n);
}

/*
 * cgldga - Load address of global variable into R2
 */
void cgldga(char *s) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         LA    R2,%s\n", s);
}

/*
 * cgindb - Indirect load byte: R2 = *(char*)R2
 *
 * Dereferences R2 as a pointer to char and loads the byte.
 * The byte is zero-extended to a fullword.
 */
void cgindb(void) {
	gen("LR    R4,R2");
	gen("SR    R2,R2");
	gen("IC    R2,0(,R4)");
}

/*
 * cgindw - Indirect load word: R2 = *(int*)R2
 *
 * Dereferences R2 as a pointer to int and loads the fullword.
 */
void cgindw(void) {
	gen("L     R2,0(,R2)");
}

/*
 * cgargc - Load argument count (argc)
 *
 * In our calling convention, argc is passed as the first parameter
 * at offset 0 from the frame pointer.
 */
void cgargc(void) {
	gen("L     R2,0(,R11)");
}

/*
 * cgldlab - Load label address into R2
 *
 * Loads the address of a code label (used for computed gotos, etc.)
 */
void cgldlab(int id) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         LA    R2,%c%d\n", LPREFIX, id);
}

/* ========================================================================
 * STACK OPERATIONS
 * ========================================================================
 * The stack grows downward (toward lower addresses).
 * R15 is the stack pointer.
 */

/*
 * cgpush - Push R2 (accumulator) onto the stack
 *
 * Decrements stack pointer by 4 (fullword) and stores R2.
 * Used for saving intermediate results and building parameter lists.
 */
void cgpush(void) {
	if (NULL == Outfile) return;
#ifdef S390_USE_ESA390
	fprintf(Outfile, "         AHI   R15,-4             Decrement stack ptr\n");
#else
	fprintf(Outfile, "         S     R15,=F'4'          Decrement stack ptr\n");
#endif
	fprintf(Outfile, "         ST    R2,0(,R15)         Push R2 to stack\n");
}

/*
 * cgpushlit - Push a literal (immediate) value onto the stack
 *
 * Loads the value into R0 and pushes it. Used for passing
 * constant values as function arguments.
 */
void cgpushlit(int n) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "* Push literal value %d\n", n);
#ifdef S390_USE_ESA390
	if (n >= -32768 && n <= 32767) {
		fprintf(Outfile, "         LHI   R0,%d              Load immediate\n", n);
	} else {
		fprintf(Outfile, "         L     R0,=F'%d'          Load from literal\n", n);
	}
	fprintf(Outfile, "         AHI   R15,-4             Decrement stack ptr\n");
#else
	fprintf(Outfile, "         L     R0,=F'%d'          Load from literal\n", n);
	fprintf(Outfile, "         S     R15,=F'4'          Decrement stack ptr\n");
#endif
	fprintf(Outfile, "         ST    R0,0(,R15)         Push to stack\n");
}

/*
 * cgpop2 - Pop from stack into R3 (secondary operand register)
 *
 * Used to retrieve the left operand for binary operations.
 * After pop: R3 = left operand, R2 = right operand
 */
void cgpop2(void) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         L     R3,0(,R15)         Pop left operand to R3\n");
#ifdef S390_USE_ESA390
	fprintf(Outfile, "         AHI   R15,4              Increment stack ptr\n");
#else
	fprintf(Outfile, "         A     R15,=F'4'          Increment stack ptr\n");
#endif
}

/*
 * cgswap - Swap R2 and R3
 *
 * Used when operand order needs to be reversed for non-commutative
 * operations (subtraction, division, modulo).
 */
void cgswap(void) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         LR    R0,R2              Swap R2 <-> R3\n");
	fprintf(Outfile, "         LR    R2,R3\n");
	fprintf(Outfile, "         LR    R3,R0\n");
}

/* ========================================================================
 * ARITHMETIC AND LOGICAL OPERATIONS
 * ========================================================================
 * Binary operations: After cgpop2(), R3 contains the first operand
 * (left side) and R2 contains the second operand (right side).
 * Result is placed in R2.
 */

/*
 * cgand - Bitwise AND: R2 = R3 & R2
 */
void cgand(void) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         NR    R2,R3              R2 = R3 AND R2\n");
}

/*
 * cgxor - Bitwise XOR: R2 = R3 ^ R2
 */
void cgxor(void) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         XR    R2,R3              R2 = R3 XOR R2\n");
}

/*
 * cgior - Bitwise OR: R2 = R3 | R2
 */
void cgior(void) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         OR    R2,R3              R2 = R3 OR R2\n");
}

/*
 * cgadd - Addition: R2 = R3 + R2
 */
void cgadd(void) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         AR    R2,R3              R2 = R3 + R2\n");
}

/*
 * cgmul - Signed multiplication: R2 = R3 * R2
 *
 * S/390 MR instruction requires an even-odd register pair (R4:R5).
 * The multiplicand must be in the odd register (R5) of the pair.
 * MR multiplies R5 by the operand and places the 64-bit result
 * in R4:R5 (R4=high, R5=low).
 */
void cgmul(void) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "* Multiply: R2 = R3 * R2\n");
	fprintf(Outfile, "         LR    R5,R3              Multiplicand to R5\n");
	fprintf(Outfile, "         MR    R4,R2              R4:R5 = R5 * R2\n");
	fprintf(Outfile, "         LR    R2,R5              Result (low 32) to R2\n");
}

/*
 * cgsub - Subtraction: R2 = R3 - R2
 *
 * Since R3 has the left operand and R2 has the right operand,
 * we compute R3 - R2 and move result to R2.
 */
void cgsub(void) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         SR    R3,R2              R3 = R3 - R2\n");
	fprintf(Outfile, "         LR    R2,R3              Result to R2\n");
}

/*
 * cgdiv - Signed division: R2 = R3 / R2
 *
 * S/390 DR instruction divides a 64-bit dividend in an even-odd
 * register pair by a 32-bit divisor.
 * - Dividend: R4:R5 (64-bit signed, sign-extended from R3)
 * - Divisor: R2 (32-bit signed)
 * - Quotient: R5
 * - Remainder: R4
 */
void cgdiv(void) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "* Divide: R2 = R3 / R2\n");
	fprintf(Outfile, "         LR    R5,R3              Dividend to R5\n");
	fprintf(Outfile, "         LR    R4,R5              Copy for sign extend\n");
	fprintf(Outfile, "         SRA   R4,31             Sign extend to R4\n");
	fprintf(Outfile, "         DR    R4,R2              R4:R5 / R2\n");
	fprintf(Outfile, "         LR    R2,R5              Quotient to R2\n");
}

/*
 * cgmod - Modulo (remainder): R2 = R3 % R2
 *
 * Same as division, but we take the remainder from R4.
 */
void cgmod(void) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "* Modulo: R2 = R3 %% R2\n");
	fprintf(Outfile, "         LR    R5,R3              Dividend to R5\n");
	fprintf(Outfile, "         LR    R4,R5              Copy for sign extend\n");
	fprintf(Outfile, "         SRA   R4,31             Sign extend to R4\n");
	fprintf(Outfile, "         DR    R4,R2              R4:R5 / R2\n");
	fprintf(Outfile, "         LR    R2,R4              Remainder to R2\n");
}

/*
 * cgshl - Shift left logical: R2 = R2 << R3
 *
 * The shift amount is in R3. S/390 shift instructions use the
 * low 6 bits of the address field as the shift count.
 */
void cgshl(void) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         SLL   R2,0(R3)           R2 = R2 << R3\n");
}

/*
 * cgshr - Shift right arithmetic: R2 = R2 >> R3
 *
 * Arithmetic shift preserves the sign bit (for signed integers).
 */
void cgshr(void) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         SRA   R2,0(R3)           R2 = R2 >> R3 (arith)\n");
}

/* ========================================================================
 * COMPARISON OPERATIONS
 * ========================================================================
 * These functions compare R3 (left operand) with R2 (right operand)
 * and set R2 to 1 if the condition is true, 0 otherwise.
 */

/*
 * cgcmp - Generic comparison helper
 *
 * Compares R3 with R2 and sets R2 to 1 if condition is FALSE
 * (the branch instruction skips the increment if condition is TRUE).
 * The logic is inverted because we branch OVER the "set to 1" code.
 */
void cgcmp(char *inst) {
	int lab;
	lab = label();
	if (NULL == Outfile) return;
	fprintf(Outfile, "         SR    R4,R4\n");           /* R4 = 0 (result) */
	fprintf(Outfile, "         CR    R3,R2\n");           /* Compare R3 with R2 */
	fprintf(Outfile, "         %s    %c%d\n", inst, LPREFIX, lab);  /* Branch if true */
#ifdef S390_USE_ESA390
	fprintf(Outfile, "         AHI   R4,1\n");            /* Set result to 1 */
#else
	fprintf(Outfile, "         LA    R4,1\n");            /* Set result to 1 */
#endif
	fprintf(Outfile, "%c%-7d DS    0H\n", LPREFIX, lab);  /* Target label */
	fprintf(Outfile, "         LR    R2,R4\n");           /* Move result to R2 */
}

/* Equal: R2 = (R3 == R2) ? 1 : 0 */
void cgeq(void)  { cgcmp("BNE"); }  /* Branch if NOT equal (skip set) */

/* Not equal: R2 = (R3 != R2) ? 1 : 0 */
void cgne(void)  { cgcmp("BE"); }   /* Branch if equal (skip set) */

/* Less than: R2 = (R3 < R2) ? 1 : 0 */
void cglt(void)  { cgcmp("BNL"); }  /* Branch if NOT low (skip set) */

/* Greater than: R2 = (R3 > R2) ? 1 : 0 */
void cggt(void)  { cgcmp("BNH"); }  /* Branch if NOT high (skip set) */

/* Less or equal: R2 = (R3 <= R2) ? 1 : 0 */
void cgle(void)  { cgcmp("BH"); }   /* Branch if high (skip set) */

/* Greater or equal: R2 = (R3 >= R2) ? 1 : 0 */
void cgge(void)  { cgcmp("BL"); }   /* Branch if low (skip set) */

/* ========================================================================
 * UNARY OPERATIONS
 * ========================================================================
 */

/*
 * cgneg - Arithmetic negation: R2 = -R2
 *
 * LCR (Load Complement Register) computes the two's complement.
 */
void cgneg(void) {
	gen("LCR   R2,R2");
}

/*
 * cgnot - Bitwise NOT (one's complement): R2 = ~R2
 *
 * XOR with all 1s inverts all bits.
 */
void cgnot(void) {
	gen("X     R2,=F'-1'");
}

/*
 * cglognot - Logical NOT: R2 = !R2
 *
 * Returns 1 if R2 is zero, 0 otherwise.
 */
void cglognot(void) {
	int lab;
	lab = label();
	if (NULL == Outfile) return;
	fprintf(Outfile, "         LTR   R2,R2\n");           /* Test R2 */
#ifdef S390_USE_ESA390
	fprintf(Outfile, "         LHI   R2,1\n");            /* Assume true (R2=0) */
#else
	fprintf(Outfile, "         LA    R2,1\n");            /* Assume true (R2=0) */
#endif
	fprintf(Outfile, "         BZ    %c%d\n", LPREFIX, lab);  /* Branch if zero */
	fprintf(Outfile, "         SR    R2,R2\n");           /* Was non-zero, set to 0 */
	fprintf(Outfile, "%c%-7d DS    0H\n", LPREFIX, lab);
}

/* ========================================================================
 * POINTER SCALING OPERATIONS
 * ========================================================================
 * These functions scale values for pointer arithmetic.
 * int pointers need to be scaled by 4 (sizeof(int)).
 */

/*
 * cgscale - Scale R2 for int pointer arithmetic: R2 = R2 * 4
 *
 * Shift left by 2 is equivalent to multiply by 4.
 */
void cgscale(void) {
	gen("SLL   R2,2");
}

/*
 * cgscale2 - Scale R3 for int pointer arithmetic: R3 = R3 * 4
 */
void cgscale2(void) {
	gen("SLL   R3,2");
}

/*
 * cgunscale - Unscale pointer difference: R2 = R2 / 4
 *
 * When subtracting two int pointers, the result must be
 * divided by sizeof(int) to get the element count.
 */
void cgunscale(void) {
	gen("SRA   R2,2");
}

/*
 * cgbool - Convert to boolean: R2 = (R2 != 0) ? 1 : 0
 *
 * Normalizes any non-zero value to 1.
 */
void cgbool(void) {
	int lab;
	lab = label();
	if (NULL == Outfile) return;
	fprintf(Outfile, "         LTR   R2,R2\n");           /* Test R2 */
	fprintf(Outfile, "         BZ    %c%d\n", LPREFIX, lab);  /* Skip if zero */
#ifdef S390_USE_ESA390
	fprintf(Outfile, "         LHI   R2,1\n");            /* Non-zero -> 1 */
#else
	fprintf(Outfile, "         LA    R2,1\n");            /* Non-zero -> 1 */
#endif
	fprintf(Outfile, "%c%-7d DS    0H\n", LPREFIX, lab);
}

/* ========================================================================
 * INCREMENT/DECREMENT OPERATIONS
 * ========================================================================
 * These functions implement pre/post increment/decrement for various
 * storage classes (local, static, global) and types (byte, word, pointer).
 *
 * Naming convention:
 * - cginc/cgdec: increment/decrement
 * - 1/2: via R2 or R4 (for post-increment, R4 holds saved address)
 * - p/i: pointer (by 4) or integer (by 1)
 * - b/w: byte or word
 * - l/s/g: local, static, or global
 */

/*
 * cgldinc - Save R2 to R4 for post-increment operations
 *
 * Before post-increment, we need to save the address so we can
 * return the old value while still incrementing the memory location.
 */
void cgldinc(void) {
	gen("LR    R4,R2");
}

/* Increment pointer indirect (via R2) by sizeof(int) = 4 */
void cginc1pi(void) {
	gen("L     R0,0(,R2)");
#ifdef S390_USE_ESA390
	gen("AHI   R0,4");
#else
	gen("A     R0,=F'4'");
#endif
	gen("ST    R0,0(,R2)");
}

/* Decrement pointer indirect (via R2) by 4 */
void cgdec1pi(void) {
	gen("L     R0,0(,R2)");
#ifdef S390_USE_ESA390
	gen("AHI   R0,-4");
#else
	gen("S     R0,=F'4'");
#endif
	gen("ST    R0,0(,R2)");
}

/* Increment pointer indirect (via R4) by 4 */
void cginc2pi(void) {
	gen("L     R0,0(,R4)");
#ifdef S390_USE_ESA390
	gen("AHI   R0,4");
#else
	gen("A     R0,=F'4'");
#endif
	gen("ST    R0,0(,R4)");
}

/* Decrement pointer indirect (via R4) by 4 */
void cgdec2pi(void) {
	gen("L     R0,0(,R4)");
#ifdef S390_USE_ESA390
	gen("AHI   R0,-4");
#else
	gen("S     R0,=F'4'");
#endif
	gen("ST    R0,0(,R4)");
}

/* Increment local pointer by 4 */
void cgincpl(int a) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         L     R0,%d(,R11)\n", a);
#ifdef S390_USE_ESA390
	fprintf(Outfile, "         AHI   R0,4\n");
#else
	fprintf(Outfile, "         A     R0,=F'4'\n");
#endif
	fprintf(Outfile, "         ST    R0,%d(,R11)\n", a);
}

/* Decrement local pointer by 4 */
void cgdecpl(int a) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         L     R0,%d(,R11)\n", a);
#ifdef S390_USE_ESA390
	fprintf(Outfile, "         AHI   R0,-4\n");
#else
	fprintf(Outfile, "         S     R0,=F'4'\n");
#endif
	fprintf(Outfile, "         ST    R0,%d(,R11)\n", a);
}

/* Increment static pointer by 4 */
void cgincps(int a) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         L     R0,%c%d\n", LPREFIX, a);
#ifdef S390_USE_ESA390
	fprintf(Outfile, "         AHI   R0,4\n");
#else
	fprintf(Outfile, "         A     R0,=F'4'\n");
#endif
	fprintf(Outfile, "         ST    R0,%c%d\n", LPREFIX, a);
}

/* Decrement static pointer by 4 */
void cgdecps(int a) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         L     R0,%c%d\n", LPREFIX, a);
#ifdef S390_USE_ESA390
	fprintf(Outfile, "         AHI   R0,-4\n");
#else
	fprintf(Outfile, "         S     R0,=F'4'\n");
#endif
	fprintf(Outfile, "         ST    R0,%c%d\n", LPREFIX, a);
}

/* Increment global pointer by 4 */
void cgincpg(char *s) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         L     R0,%s\n", s);
#ifdef S390_USE_ESA390
	fprintf(Outfile, "         AHI   R0,4\n");
#else
	fprintf(Outfile, "         A     R0,=F'4'\n");
#endif
	fprintf(Outfile, "         ST    R0,%s\n", s);
}

/* Decrement global pointer by 4 */
void cgdecpg(char *s) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         L     R0,%s\n", s);
#ifdef S390_USE_ESA390
	fprintf(Outfile, "         AHI   R0,-4\n");
#else
	fprintf(Outfile, "         S     R0,=F'4'\n");
#endif
	fprintf(Outfile, "         ST    R0,%s\n", s);
}

/* Increment word indirect (via R2) by 1 */
void cginc1iw(void) {
	gen("L     R0,0(,R2)");
#ifdef S390_USE_ESA390
	gen("AHI   R0,1");
#else
	gen("LA    R0,1(,R0)");
#endif
	gen("ST    R0,0(,R2)");
}

/* Decrement word indirect (via R2) by 1 */
void cgdec1iw(void) {
	gen("L     R0,0(,R2)");
#ifdef S390_USE_ESA390
	gen("AHI   R0,-1");
#else
	gen("BCTR  R0,0");
#endif
	gen("ST    R0,0(,R2)");
}

/* Increment word indirect (via R4) by 1 */
void cginc2iw(void) {
	gen("L     R0,0(,R4)");
#ifdef S390_USE_ESA390
	gen("AHI   R0,1");
#else
	gen("LA    R0,1(,R0)");
#endif
	gen("ST    R0,0(,R4)");
}

/* Decrement word indirect (via R4) by 1 */
void cgdec2iw(void) {
	gen("L     R0,0(,R4)");
#ifdef S390_USE_ESA390
	gen("AHI   R0,-1");
#else
	gen("BCTR  R0,0");
#endif
	gen("ST    R0,0(,R4)");
}

/* Increment local word by 1 */
void cginclw(int a) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         L     R0,%d(,R11)\n", a);
#ifdef S390_USE_ESA390
	fprintf(Outfile, "         AHI   R0,1\n");
#else
	fprintf(Outfile, "         LA    R0,1(,R0)\n");
#endif
	fprintf(Outfile, "         ST    R0,%d(,R11)\n", a);
}

/* Decrement local word by 1 */
void cgdeclw(int a) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         L     R0,%d(,R11)\n", a);
#ifdef S390_USE_ESA390
	fprintf(Outfile, "         AHI   R0,-1\n");
#else
	fprintf(Outfile, "         BCTR  R0,0\n");
#endif
	fprintf(Outfile, "         ST    R0,%d(,R11)\n", a);
}

/* Increment static word by 1 */
void cgincsw(int a) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         L     R0,%c%d\n", LPREFIX, a);
#ifdef S390_USE_ESA390
	fprintf(Outfile, "         AHI   R0,1\n");
#else
	fprintf(Outfile, "         LA    R0,1(,R0)\n");
#endif
	fprintf(Outfile, "         ST    R0,%c%d\n", LPREFIX, a);
}

/* Decrement static word by 1 */
void cgdecsw(int a) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         L     R0,%c%d\n", LPREFIX, a);
#ifdef S390_USE_ESA390
	fprintf(Outfile, "         AHI   R0,-1\n");
#else
	fprintf(Outfile, "         BCTR  R0,0\n");
#endif
	fprintf(Outfile, "         ST    R0,%c%d\n", LPREFIX, a);
}

/* Increment global word by 1 */
void cgincgw(char *s) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         L     R0,%s\n", s);
#ifdef S390_USE_ESA390
	fprintf(Outfile, "         AHI   R0,1\n");
#else
	fprintf(Outfile, "         LA    R0,1(,R0)\n");
#endif
	fprintf(Outfile, "         ST    R0,%s\n", s);
}

/* Decrement global word by 1 */
void cgdecgw(char *s) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         L     R0,%s\n", s);
#ifdef S390_USE_ESA390
	fprintf(Outfile, "         AHI   R0,-1\n");
#else
	fprintf(Outfile, "         BCTR  R0,0\n");
#endif
	fprintf(Outfile, "         ST    R0,%s\n", s);
}

/* Increment byte indirect (via R2) by 1 */
void cginc1ib(void) {
	gen("SR    R0,R0");
	gen("IC    R0,0(,R2)");
#ifdef S390_USE_ESA390
	gen("AHI   R0,1");
#else
	gen("LA    R0,1(,R0)");
#endif
	gen("STC   R0,0(,R2)");
}

/* Decrement byte indirect (via R2) by 1 */
void cgdec1ib(void) {
	gen("SR    R0,R0");
	gen("IC    R0,0(,R2)");
#ifdef S390_USE_ESA390
	gen("AHI   R0,-1");
#else
	gen("BCTR  R0,0");
#endif
	gen("STC   R0,0(,R2)");
}

/* Increment byte indirect (via R4) by 1 */
void cginc2ib(void) {
	gen("SR    R0,R0");
	gen("IC    R0,0(,R4)");
#ifdef S390_USE_ESA390
	gen("AHI   R0,1");
#else
	gen("LA    R0,1(,R0)");
#endif
	gen("STC   R0,0(,R4)");
}

/* Decrement byte indirect (via R4) by 1 */
void cgdec2ib(void) {
	gen("SR    R0,R0");
	gen("IC    R0,0(,R4)");
#ifdef S390_USE_ESA390
	gen("AHI   R0,-1");
#else
	gen("BCTR  R0,0");
#endif
	gen("STC   R0,0(,R4)");
}

/* Increment local byte by 1 */
void cginclb(int a) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         SR    R0,R0\n");
	fprintf(Outfile, "         IC    R0,%d(,R11)\n", a);
#ifdef S390_USE_ESA390
	fprintf(Outfile, "         AHI   R0,1\n");
#else
	fprintf(Outfile, "         LA    R0,1(,R0)\n");
#endif
	fprintf(Outfile, "         STC   R0,%d(,R11)\n", a);
}

/* Decrement local byte by 1 */
void cgdeclb(int a) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         SR    R0,R0\n");
	fprintf(Outfile, "         IC    R0,%d(,R11)\n", a);
#ifdef S390_USE_ESA390
	fprintf(Outfile, "         AHI   R0,-1\n");
#else
	fprintf(Outfile, "         BCTR  R0,0\n");
#endif
	fprintf(Outfile, "         STC   R0,%d(,R11)\n", a);
}

/* Increment static byte by 1 */
void cgincsb(int a) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         SR    R0,R0\n");
	fprintf(Outfile, "         IC    R0,%c%d\n", LPREFIX, a);
#ifdef S390_USE_ESA390
	fprintf(Outfile, "         AHI   R0,1\n");
#else
	fprintf(Outfile, "         LA    R0,1(,R0)\n");
#endif
	fprintf(Outfile, "         STC   R0,%c%d\n", LPREFIX, a);
}

/* Decrement static byte by 1 */
void cgdecsb(int a) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         SR    R0,R0\n");
	fprintf(Outfile, "         IC    R0,%c%d\n", LPREFIX, a);
#ifdef S390_USE_ESA390
	fprintf(Outfile, "         AHI   R0,-1\n");
#else
	fprintf(Outfile, "         BCTR  R0,0\n");
#endif
	fprintf(Outfile, "         STC   R0,%c%d\n", LPREFIX, a);
}

/* Increment global byte by 1 */
void cgincgb(char *s) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         SR    R0,R0\n");
	fprintf(Outfile, "         IC    R0,%s\n", s);
#ifdef S390_USE_ESA390
	fprintf(Outfile, "         AHI   R0,1\n");
#else
	fprintf(Outfile, "         LA    R0,1(,R0)\n");
#endif
	fprintf(Outfile, "         STC   R0,%s\n", s);
}

/* Decrement global byte by 1 */
void cgdecgb(char *s) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         SR    R0,R0\n");
	fprintf(Outfile, "         IC    R0,%s\n", s);
#ifdef S390_USE_ESA390
	fprintf(Outfile, "         AHI   R0,-1\n");
#else
	fprintf(Outfile, "         BCTR  R0,0\n");
#endif
	fprintf(Outfile, "         STC   R0,%s\n", s);
}

/* ========================================================================
 * BRANCH AND CONTROL FLOW OPERATIONS
 * ========================================================================
 */

/*
 * cgbr - Conditional branch helper
 *
 * Tests R2 and branches based on the condition code.
 * LTR sets CC: 0=zero, 1=negative, 2=positive
 */
void cgbr(char *how, int n) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         LTR   R2,R2\n");
	fprintf(Outfile, "         %s    %c%d\n", how, LPREFIX, n);
}

/* Branch if true (R2 != 0) */
void cgbrtrue(int n) {
	cgbr("BNZ", n);
}

/* Branch if false (R2 == 0) */
void cgbrfalse(int n) {
	cgbr("BZ", n);
}

/* Unconditional jump */
void cgjump(int n) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         B     %c%d\n", LPREFIX, n);
}

/* ========================================================================
 * SWITCH/CASE SUPPORT
 * ========================================================================
 * Switch statements use a dispatch table and the @SWITCH runtime routine.
 */

/* Load switch table address into R4 */
void cgldswtch(int n) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         LA    R4,%c%d\n", LPREFIX, n);
}

/* Call switch handler - branches to @SWITCH routine in prelude */
void cgcalswtch(void) {
	gen("BAS   R14,@SWITCH");
}

/* Emit a switch case entry: value and target label address */
void cgcase(int v, int l) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         DC    F'%d'\n", v);
	fprintf(Outfile, "         DC    A(%c%d)\n", LPREFIX, l);
}

/* ========================================================================
 * STORE OPERATIONS
 * ========================================================================
 */

/*
 * cgpopptr - Pop pointer from stack into R4
 *
 * Used before indirect stores to get the destination address.
 */
void cgpopptr(void) {
	gen("L     R4,0(,R15)");
#ifdef S390_USE_ESA390
	gen("AHI   R15,4");
#else
	gen("A     R15,=F'4'");
#endif
}

/* Store byte indirect: *(char*)R4 = R2 */
void cgstorib(void) {
	gen("STC   R2,0(,R4)");
}

/* Store word indirect: *(int*)R4 = R2 */
void cgstoriw(void) {
	gen("ST    R2,0(,R4)");
}

/* Store byte to local variable */
void cgstorlb(int n) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         STC   R2,%d(,R11)\n", n);
}

/* Store word to local variable */
void cgstorlw(int n) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         ST    R2,%d(,R11)\n", n);
}

/* Store byte to static variable */
void cgstorsb(int n) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         STC   R2,%c%d\n", LPREFIX, n);
}

/* Store word to static variable */
void cgstorsw(int n) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         ST    R2,%c%d\n", LPREFIX, n);
}

/* Store byte to global variable */
void cgstorgb(char *s) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         STC   R2,%s\n", s);
}

/* Store word to global variable */
void cgstorgw(char *s) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         ST    R2,%s\n", s);
}

/*
 * cginitlw - Initialize local word with a constant value
 *
 * Used for local variable initialization at function entry.
 */
void cginitlw(int v, int a) {
	if (NULL == Outfile) return;
#ifdef S390_USE_ESA390
	if (v >= -32768 && v <= 32767) {
		fprintf(Outfile, "         LHI   R0,%d\n", v);
	} else {
		fprintf(Outfile, "         L     R0,=F'%d'\n", v);
	}
#else
	fprintf(Outfile, "         L     R0,=F'%d'\n", v);
#endif
	fprintf(Outfile, "         ST    R0,%d(,R11)\n", a);
}

/* ========================================================================
 * FUNCTION CALL OPERATIONS (OS/390 Standard Linkage)
 * ========================================================================
 * This implementation uses OS/390 standard linkage conventions:
 * - R1 points to parameter address list (addresses of parameters)
 * - Last parameter address has high bit set (X'80') to mark end
 * - R13 points to save area chain
 * - R14 contains return address
 * - R15 contains entry point / return value
 *
 * Parameters are passed by building a list of addresses on the stack,
 * then setting R1 to point to this list before the call.
 */

/*
 * cgcall - Call a function by name using OS/390 standard linkage
 *
 * The parameter list has been built on the stack by cgpush operations.
 * R1 is loaded with the address of this parameter list.
 * BAS saves return address in R14 and branches to the function.
 */
void cgcall(char *s) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "*\n");
	fprintf(Outfile, "* Call function: %s\n", s);
	fprintf(Outfile, "*\n");
	fprintf(Outfile, "         LR    R1,R15            R1 -> parameter list\n");
	fprintf(Outfile, "         BAS   R14,%s            Branch and save\n", s);
}

/*
 * cgcalr - Call function via pointer in R2 (indirect call)
 *
 * Used for function pointer calls. R15 receives the entry point
 * from R2, and BALR performs the call saving return in R14.
 */
void cgcalr(void) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "*\n");
	fprintf(Outfile, "* Indirect function call via pointer\n");
	fprintf(Outfile, "*\n");
	fprintf(Outfile, "         LR    R1,R15            R1 -> parameter list\n");
	fprintf(Outfile, "         LR    R15,R2            Entry point to R15\n");
	fprintf(Outfile, "         BALR  R14,R15           Branch and link\n");
}

/*
 * cgstack - Adjust stack pointer by n bytes
 *
 * Positive n: deallocate stack space (cleanup after call)
 * Negative n: allocate stack space (for locals or params)
 */
void cgstack(int n) {
	if (NULL == Outfile) return;
	if (n == 0) return;
	
	if (n > 0) {
		fprintf(Outfile, "* Deallocate %d bytes from stack\n", n);
	} else {
		fprintf(Outfile, "* Allocate %d bytes on stack\n", -n);
	}
	
#ifdef S390_USE_ESA390
	if (n >= -32768 && n <= 32767) {
		fprintf(Outfile, "         AHI   R15,%d\n", n);
	} else {
		if (n > 0)
			fprintf(Outfile, "         A     R15,=F'%d'\n", n);
		else
			fprintf(Outfile, "         S     R15,=F'%d'\n", -n);
	}
#else
	if (n > 0)
		fprintf(Outfile, "         A     R15,=F'%d'\n", n);
	else
		fprintf(Outfile, "         S     R15,=F'%d'\n", -n);
#endif
}

/* ========================================================================
 * FUNCTION PROLOGUE AND EPILOGUE (OS/390 Standard Linkage)
 * ========================================================================
 */

/*
 * cgentry - Function prologue (OS/390 standard entry linkage)
 *
 * Implements the standard OS/390 function entry sequence:
 *
 * On entry from caller:
 *   R1  = Address of parameter address list
 *   R13 = Address of caller's 72-byte save area
 *   R14 = Return address
 *   R15 = Entry point address of this function
 *
 * Entry sequence:
 *   1. Save registers R14-R12 in caller's save area (offset 12)
 *   2. Establish base register (R12) for addressability
 *   3. Allocate our own save area and local storage
 *   4. Chain save areas (back pointer at offset 4)
 *   5. Set up frame pointer (R11) for local variable access
 *   6. Save R1 (parameter list pointer) for later access
 */
void cgentry(void) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "*\n");
	fprintf(Outfile, "* Function entry - OS/390 standard linkage\n");
	fprintf(Outfile, "* On entry: R1=parmlist, R13=caller SA, R14=return, R15=entry\n");
	fprintf(Outfile, "*\n");
	fprintf(Outfile, "         STM   R14,R12,12(R13)   Save registers in caller SA\n");
	fprintf(Outfile, "         LR    R12,R15           Establish base register\n");
	fprintf(Outfile, "         LR    R2,R1             Save parm list pointer\n");
	fprintf(Outfile, "         LR    R11,R15           Frame pointer = entry point\n");
#ifdef S390_USE_ESA390
	fprintf(Outfile, "         AHI   R15,-80           Allocate save area (72+8)\n");
#else
	fprintf(Outfile, "         S     R15,=F'80'        Allocate save area (72+8)\n");
#endif
	fprintf(Outfile, "         ST    R13,4(,R15)       Chain: back ptr to caller SA\n");
	fprintf(Outfile, "         ST    R15,8(,R13)       Chain: fwd ptr from caller SA\n");
	fprintf(Outfile, "         LR    R13,R15           R13 -> our save area\n");
	fprintf(Outfile, "         ST    R2,72(,R13)       Save parm list at SA+72\n");
}

/*
 * cgexit - Function epilogue (OS/390 standard return linkage)
 *
 * Implements the standard OS/390 function exit sequence:
 *
 *   1. Move return value from R2 to R15 (OS/390 convention)
 *   2. Restore caller's save area pointer from back chain
 *   3. Restore all registers from caller's save area
 *   4. Return to caller via BR R14
 *
 * Note: The return value is kept in R2 internally during expression
 * evaluation, but OS/390 convention expects it in R15 on return.
 */
void cgexit(void) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "*\n");
	fprintf(Outfile, "* Function exit - OS/390 standard linkage\n");
	fprintf(Outfile, "* Return value in R2 -> moved to R15 for caller\n");
	fprintf(Outfile, "*\n");
	fprintf(Outfile, "         LR    R15,R2            Return value to R15\n");
	fprintf(Outfile, "         L     R13,4(,R13)       Restore caller's SA pointer\n");
	fprintf(Outfile, "         L     R14,12(,R13)      Restore return address\n");
	fprintf(Outfile, "         LM    R0,R12,20(R13)    Restore R0-R12\n");
	fprintf(Outfile, "         BR    R14               Return to caller\n");
}

/* ========================================================================
 * DATA DEFINITION DIRECTIVES
 * ========================================================================
 * These functions emit HLASM DC (Define Constant) and DS (Define Storage)
 * directives for initializing data.
 */

/*
 * cgdefb - Define a byte constant
 *
 * Emits a single byte in hexadecimal format.
 */
void cgdefb(int v) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         DC    X'%02X'\n", v & 0xFF);
}

/*
 * cgdefw - Define a word (fullword) constant
 *
 * Emits a 32-bit signed integer. HLASM F-type constants are
 * automatically aligned to fullword boundaries.
 */
void cgdefw(int v) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         DC    F'%d'\n", v);
}

/*
 * cgdefp - Define a pointer constant (address)
 *
 * Emits an address constant. A-type constants hold addresses
 * and are fullword aligned.
 */
void cgdefp(int v) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         DC    A(%d)\n", v);
}

/*
 * cgdefl - Define a label reference (address of label)
 *
 * Emits an address constant pointing to a labeled location.
 */
void cgdefl(int v) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         DC    A(%c%d)\n", LPREFIX, v);
}

/*
 * cgdefc - Define a character constant
 *
 * Emits a single character. Note: HLASM uses EBCDIC encoding
 * on mainframes, so character values may differ from ASCII.
 */
void cgdefc(int c) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         DC    C'%c'\n", c);
}

/*
 * cgbss - Define uninitialized storage (BSS)
 *
 * DS (Define Storage) reserves space without initializing it.
 * The 'X' type reserves bytes.
 */
void cgbss(char *s, int z) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "%-8s DS    %dX\n", s, z);
}

/* ========================================================================
 * END OF S/390 HLASM CODE GENERATOR
 * ========================================================================
 */
