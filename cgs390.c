/*
 *	NMH's Simple C Compiler, 2011,2012,2022
 *	S/390 HLASM target description
 *	Backend for IBM System/390 mainframes
 *
 *	S/390 HLASM backend by ALEXANDRE BENCZ
 */

#include "defs.h"
#include "data.h"
#include "decl.h"
#include "cgen.h"

/*
 * S/390 Register Usage:
 * R0      - Work register (not preserved across calls)
 * R1      - Work register / parameter passing
 * R2      - Primary accumulator (like EAX)
 * R3      - Secondary operand (like ECX)
 * R4      - Auxiliary register (like EDX)
 * R5-R10  - Temporaries (callee-saved)
 * R11     - Local base pointer (like EBP)
 * R12     - Program base register (USING)
 * R13     - Save area pointer
 * R14     - Return address
 * R15     - Stack pointer (like ESP)
 *
 * HLASM Format:
 * Columns 1-8:   Label (optional)
 * Column 9:      Blank
 * Columns 10-14: Operation
 * Column 15:     Blank
 * Columns 16-71: Operands and comments
 * Column 72:     Continuation character
 * Columns 73-80: Sequence number (optional)
 */

static int	Lbl_counter = 0;

/* Helper to generate unique temp labels for HLASM */
static char *tmplabel(void) {
	static char buf[16];
	sprintf(buf, "@T%04d", Lbl_counter++);
	return buf;
}

/* Section directives */
void cgdata(void)	{ genraw("*\n* DATA SECTION\n*\n"); }
void cgtext(void)	{ genraw("*\n* CODE SECTION\n*\n"); }

void cgprelude(void) {
	genraw("*\n");
	genraw("* SubC Compiler Output - S/390 HLASM\n");
	genraw("*\n");
	genraw("         PRINT NOGEN\n");
	genraw("SUBC     CSECT\n");
	genraw("         USING SUBC,R12\n");
	genraw("*\n");
	genraw("* Register equates\n");
	genraw("R0       EQU   0\n");
	genraw("R1       EQU   1\n");
	genraw("R2       EQU   2\n");
	genraw("R3       EQU   3\n");
	genraw("R4       EQU   4\n");
	genraw("R5       EQU   5\n");
	genraw("R6       EQU   6\n");
	genraw("R7       EQU   7\n");
	genraw("R8       EQU   8\n");
	genraw("R9       EQU   9\n");
	genraw("R10      EQU   10\n");
	genraw("R11      EQU   11\n");
	genraw("R12      EQU   12\n");
	genraw("R13      EQU   13\n");
	genraw("R14      EQU   14\n");
	genraw("R15      EQU   15\n");
	genraw("*\n");
}

void cgpostlude(void) {
	genraw("*\n");
	genraw("         END\n");
}

void cgpublic(char *s) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         ENTRY %s\n", s);
}

/* Generate label in HLASM format - label in column 1, padded to 8 chars */
void genlab(int id) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "%c%-7d DS    0H\n", LPREFIX, id);
}

/* Generate symbol name in HLASM format */
void genname(char *name) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "%-8s", gsym(name));
}

/* Load literal value into R2 (accumulator) */
void cglit(int v) {
	if (NULL == Outfile) return;
	if (v >= -32768 && v <= 32767) {
		fprintf(Outfile, "         LHI   R2,%d\n", v);
	} else {
		fprintf(Outfile, "         L     R2,=F'%d'\n", v);
	}
}

/* Clear accumulator (R2 = 0) */
void cgclear(void) {
	gen("SR    R2,R2");
}

/* Load global byte into R2 */
void cgldgb(char *s) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         SR    R2,R2\n");
	fprintf(Outfile, "         IC    R2,%s\n", s);
}

/* Load global word into R2 */
void cgldgw(char *s) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         L     R2,%s\n", s);
}

/* Load local byte into R2 (offset from R11) */
void cgldlb(int n) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         SR    R2,R2\n");
	fprintf(Outfile, "         IC    R2,%d(R11)\n", n);
}

/* Load local word into R2 (offset from R11) */
void cgldlw(int n) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         L     R2,%d(R11)\n", n);
}

/* Load static byte into R2 */
void cgldsb(int n) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         SR    R2,R2\n");
	fprintf(Outfile, "         IC    R2,%c%d\n", LPREFIX, n);
}

/* Load static word into R2 */
void cgldsw(int n) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         L     R2,%c%d\n", LPREFIX, n);
}

/* Load address of local variable into R2 */
void cgldla(int n) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         LA    R2,%d(R11)\n", n);
}

/* Load address of static variable into R2 */
void cgldsa(int n) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         LA    R2,%c%d\n", LPREFIX, n);
}

/* Load address of global variable into R2 */
void cgldga(char *s) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         LA    R2,%s\n", s);
}

/* Indirect load byte: R2 = *(char*)R2 */
void cgindb(void) {
	gen("LR    R4,R2");
	gen("SR    R2,R2");
	gen("IC    R2,0(R4)");
}

/* Indirect load word: R2 = *(int*)R2 */
void cgindw(void) {
	gen("L     R2,0(R2)");
}

/* Load argc (first parameter) */
void cgargc(void) {
	gen("L     R2,0(R11)");
}

/* Load label address into R2 */
void cgldlab(int id) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         LA    R2,%c%d\n", LPREFIX, id);
}

/* Push R2 onto stack */
void cgpush(void) {
	gen("AHI   R15,-4");
	gen("ST    R2,0(R15)");
}

/* Push literal onto stack */
void cgpushlit(int n) {
	if (NULL == Outfile) return;
	if (n >= -32768 && n <= 32767) {
		fprintf(Outfile, "         LHI   R0,%d\n", n);
	} else {
		fprintf(Outfile, "         L     R0,=F'%d'\n", n);
	}
	gen("AHI   R15,-4");
	gen("ST    R0,0(R15)");
}

/* Pop into R3 (secondary register) */
void cgpop2(void) {
	gen("L     R3,0(R15)");
	gen("AHI   R15,4");
}

/* Swap R2 and R3 */
void cgswap(void) {
	gen("LR    R0,R2");
	gen("LR    R2,R3");
	gen("LR    R3,R0");
}

/* Binary AND: R2 = R3 & R2 */
void cgand(void) {
	gen("NR    R2,R3");
}

/* Binary XOR: R2 = R3 ^ R2 */
void cgxor(void) {
	gen("XR    R2,R3");
}

/* Binary OR: R2 = R3 | R2 */
void cgior(void) {
	gen("OR    R2,R3");
}

/* Add: R2 = R3 + R2 */
void cgadd(void) {
	gen("AR    R2,R3");
}

/* Multiply: R2 = R3 * R2 */
void cgmul(void) {
	gen("MR    R2,R3");
	gen("LR    R2,R3");
}

/* Subtract: R2 = R3 - R2 */
void cgsub(void) {
	gen("SR    R3,R2");
	gen("LR    R2,R3");
}

/* Divide: R2 = R3 / R2 (signed) */
void cgdiv(void) {
	gen("LR    R4,R3");
	gen("SRDA  R4,32");
	gen("DR    R4,R2");
	gen("LR    R2,R5");
}

/* Modulo: R2 = R3 % R2 */
void cgmod(void) {
	gen("LR    R4,R3");
	gen("SRDA  R4,32");
	gen("DR    R4,R2");
	gen("LR    R2,R4");
}

/* Shift left: R2 = R2 << R3 */
void cgshl(void) {
	gen("SLL   R2,0(R3)");
}

/* Shift right arithmetic: R2 = R2 >> R3 */
void cgshr(void) {
	gen("SRA   R2,0(R3)");
}

/* Compare and set result */
void cgcmp(char *inst) {
	int lab;
	lab = label();
	if (NULL == Outfile) return;
	fprintf(Outfile, "         SR    R4,R4\n");
	fprintf(Outfile, "         CR    R3,R2\n");
	fprintf(Outfile, "         %s    %c%d\n", inst, LPREFIX, lab);
	fprintf(Outfile, "         AHI   R4,1\n");
	fprintf(Outfile, "%c%d      DS    0H\n", LPREFIX, lab);
	fprintf(Outfile, "         LR    R2,R4\n");
}

void cgeq(void)  { cgcmp("BNE"); }
void cgne(void)  { cgcmp("BE"); }
void cglt(void)  { cgcmp("BNL"); }
void cggt(void)  { cgcmp("BNH"); }
void cgle(void)  { cgcmp("BH"); }
void cgge(void)  { cgcmp("BL"); }

/* Negate: R2 = -R2 */
void cgneg(void) {
	gen("LCR   R2,R2");
}

/* Bitwise NOT: R2 = ~R2 */
void cgnot(void) {
	gen("X     R2,=F'-1'");
}

/* Logical NOT: R2 = !R2 */
void cglognot(void) {
	int lab;
	lab = label();
	if (NULL == Outfile) return;
	fprintf(Outfile, "         LTR   R2,R2\n");
	fprintf(Outfile, "         LHI   R2,1\n");
	fprintf(Outfile, "         BZ    %c%d\n", LPREFIX, lab);
	fprintf(Outfile, "         SR    R2,R2\n");
	fprintf(Outfile, "%c%d      DS    0H\n", LPREFIX, lab);
}

/* Scale for int pointer: R2 = R2 * 4 */
void cgscale(void) {
	gen("SLL   R2,2");
}

/* Scale R3 for int pointer: R3 = R3 * 4 */
void cgscale2(void) {
	gen("SLL   R3,2");
}

/* Unscale: R2 = R2 / 4 */
void cgunscale(void) {
	gen("SRA   R2,2");
}

/* Convert to boolean: R2 = (R2 != 0) ? 1 : 0 */
void cgbool(void) {
	int lab;
	lab = label();
	if (NULL == Outfile) return;
	fprintf(Outfile, "         LTR   R2,R2\n");
	fprintf(Outfile, "         BZ    %c%d\n", LPREFIX, lab);
	fprintf(Outfile, "         LHI   R2,1\n");
	fprintf(Outfile, "%c%d      DS    0H\n", LPREFIX, lab);
}

/* Load R2 into R4 for increment operations */
void cgldinc(void) {
	gen("LR    R4,R2");
}

/* Increment pointer indirect (via R2) by 4 */
void cginc1pi(void) {
	gen("L     R0,0(R2)");
	gen("AHI   R0,4");
	gen("ST    R0,0(R2)");
}

/* Decrement pointer indirect (via R2) by 4 */
void cgdec1pi(void) {
	gen("L     R0,0(R2)");
	gen("AHI   R0,-4");
	gen("ST    R0,0(R2)");
}

/* Increment pointer indirect (via R4) by 4 */
void cginc2pi(void) {
	gen("L     R0,0(R4)");
	gen("AHI   R0,4");
	gen("ST    R0,0(R4)");
}

/* Decrement pointer indirect (via R4) by 4 */
void cgdec2pi(void) {
	gen("L     R0,0(R4)");
	gen("AHI   R0,-4");
	gen("ST    R0,0(R4)");
}

/* Increment local pointer by 4 */
void cgincpl(int a) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         L     R0,%d(R11)\n", a);
	fprintf(Outfile, "         AHI   R0,4\n");
	fprintf(Outfile, "         ST    R0,%d(R11)\n", a);
}

/* Decrement local pointer by 4 */
void cgdecpl(int a) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         L     R0,%d(R11)\n", a);
	fprintf(Outfile, "         AHI   R0,-4\n");
	fprintf(Outfile, "         ST    R0,%d(R11)\n", a);
}

/* Increment static pointer by 4 */
void cgincps(int a) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         L     R0,%c%d\n", LPREFIX, a);
	fprintf(Outfile, "         AHI   R0,4\n");
	fprintf(Outfile, "         ST    R0,%c%d\n", LPREFIX, a);
}

/* Decrement static pointer by 4 */
void cgdecps(int a) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         L     R0,%c%d\n", LPREFIX, a);
	fprintf(Outfile, "         AHI   R0,-4\n");
	fprintf(Outfile, "         ST    R0,%c%d\n", LPREFIX, a);
}

/* Increment global pointer by 4 */
void cgincpg(char *s) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         L     R0,%s\n", s);
	fprintf(Outfile, "         AHI   R0,4\n");
	fprintf(Outfile, "         ST    R0,%s\n", s);
}

/* Decrement global pointer by 4 */
void cgdecpg(char *s) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         L     R0,%s\n", s);
	fprintf(Outfile, "         AHI   R0,-4\n");
	fprintf(Outfile, "         ST    R0,%s\n", s);
}

/* Increment word indirect (via R2) */
void cginc1iw(void) {
	gen("L     R0,0(R2)");
	gen("AHI   R0,1");
	gen("ST    R0,0(R2)");
}

/* Decrement word indirect (via R2) */
void cgdec1iw(void) {
	gen("L     R0,0(R2)");
	gen("AHI   R0,-1");
	gen("ST    R0,0(R2)");
}

/* Increment word indirect (via R4) */
void cginc2iw(void) {
	gen("L     R0,0(R4)");
	gen("AHI   R0,1");
	gen("ST    R0,0(R4)");
}

/* Decrement word indirect (via R4) */
void cgdec2iw(void) {
	gen("L     R0,0(R4)");
	gen("AHI   R0,-1");
	gen("ST    R0,0(R4)");
}

/* Increment local word */
void cginclw(int a) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         L     R0,%d(R11)\n", a);
	fprintf(Outfile, "         AHI   R0,1\n");
	fprintf(Outfile, "         ST    R0,%d(R11)\n", a);
}

/* Decrement local word */
void cgdeclw(int a) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         L     R0,%d(R11)\n", a);
	fprintf(Outfile, "         AHI   R0,-1\n");
	fprintf(Outfile, "         ST    R0,%d(R11)\n", a);
}

/* Increment static word */
void cgincsw(int a) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         L     R0,%c%d\n", LPREFIX, a);
	fprintf(Outfile, "         AHI   R0,1\n");
	fprintf(Outfile, "         ST    R0,%c%d\n", LPREFIX, a);
}

/* Decrement static word */
void cgdecsw(int a) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         L     R0,%c%d\n", LPREFIX, a);
	fprintf(Outfile, "         AHI   R0,-1\n");
	fprintf(Outfile, "         ST    R0,%c%d\n", LPREFIX, a);
}

/* Increment global word */
void cgincgw(char *s) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         L     R0,%s\n", s);
	fprintf(Outfile, "         AHI   R0,1\n");
	fprintf(Outfile, "         ST    R0,%s\n", s);
}

/* Decrement global word */
void cgdecgw(char *s) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         L     R0,%s\n", s);
	fprintf(Outfile, "         AHI   R0,-1\n");
	fprintf(Outfile, "         ST    R0,%s\n", s);
}

/* Increment byte indirect (via R2) */
void cginc1ib(void) {
	gen("SR    R0,R0");
	gen("IC    R0,0(R2)");
	gen("AHI   R0,1");
	gen("STC   R0,0(R2)");
}

/* Decrement byte indirect (via R2) */
void cgdec1ib(void) {
	gen("SR    R0,R0");
	gen("IC    R0,0(R2)");
	gen("AHI   R0,-1");
	gen("STC   R0,0(R2)");
}

/* Increment byte indirect (via R4) */
void cginc2ib(void) {
	gen("SR    R0,R0");
	gen("IC    R0,0(R4)");
	gen("AHI   R0,1");
	gen("STC   R0,0(R4)");
}

/* Decrement byte indirect (via R4) */
void cgdec2ib(void) {
	gen("SR    R0,R0");
	gen("IC    R0,0(R4)");
	gen("AHI   R0,-1");
	gen("STC   R0,0(R4)");
}

/* Increment local byte */
void cginclb(int a) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         SR    R0,R0\n");
	fprintf(Outfile, "         IC    R0,%d(R11)\n", a);
	fprintf(Outfile, "         AHI   R0,1\n");
	fprintf(Outfile, "         STC   R0,%d(R11)\n", a);
}

/* Decrement local byte */
void cgdeclb(int a) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         SR    R0,R0\n");
	fprintf(Outfile, "         IC    R0,%d(R11)\n", a);
	fprintf(Outfile, "         AHI   R0,-1\n");
	fprintf(Outfile, "         STC   R0,%d(R11)\n", a);
}

/* Increment static byte */
void cgincsb(int a) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         SR    R0,R0\n");
	fprintf(Outfile, "         IC    R0,%c%d\n", LPREFIX, a);
	fprintf(Outfile, "         AHI   R0,1\n");
	fprintf(Outfile, "         STC   R0,%c%d\n", LPREFIX, a);
}

/* Decrement static byte */
void cgdecsb(int a) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         SR    R0,R0\n");
	fprintf(Outfile, "         IC    R0,%c%d\n", LPREFIX, a);
	fprintf(Outfile, "         AHI   R0,-1\n");
	fprintf(Outfile, "         STC   R0,%c%d\n", LPREFIX, a);
}

/* Increment global byte */
void cgincgb(char *s) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         SR    R0,R0\n");
	fprintf(Outfile, "         IC    R0,%s\n", s);
	fprintf(Outfile, "         AHI   R0,1\n");
	fprintf(Outfile, "         STC   R0,%s\n", s);
}

/* Decrement global byte */
void cgdecgb(char *s) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         SR    R0,R0\n");
	fprintf(Outfile, "         IC    R0,%s\n", s);
	fprintf(Outfile, "         AHI   R0,-1\n");
	fprintf(Outfile, "         STC   R0,%s\n", s);
}

/* Branch helper */
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

/* Load switch table address into R4 */
void cgldswtch(int n) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         LA    R4,%c%d\n", LPREFIX, n);
}

/* Call switch handler */
void cgcalswtch(void) {
	gen("BAS   R14,@SWITCH");
}

/* Switch case entry */
void cgcase(int v, int l) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         DC    F'%d'\n", v);
	fprintf(Outfile, "         DC    A(%c%d)\n", LPREFIX, l);
}

/* Pop pointer into R4 */
void cgpopptr(void) {
	gen("L     R4,0(R15)");
	gen("AHI   R15,4");
}

/* Store byte indirect: *(char*)R4 = R2 */
void cgstorib(void) {
	gen("STC   R2,0(R4)");
}

/* Store word indirect: *(int*)R4 = R2 */
void cgstoriw(void) {
	gen("ST    R2,0(R4)");
}

/* Store byte to local */
void cgstorlb(int n) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         STC   R2,%d(R11)\n", n);
}

/* Store word to local */
void cgstorlw(int n) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         ST    R2,%d(R11)\n", n);
}

/* Store byte to static */
void cgstorsb(int n) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         STC   R2,%c%d\n", LPREFIX, n);
}

/* Store word to static */
void cgstorsw(int n) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         ST    R2,%c%d\n", LPREFIX, n);
}

/* Store byte to global */
void cgstorgb(char *s) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         STC   R2,%s\n", s);
}

/* Store word to global */
void cgstorgw(char *s) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         ST    R2,%s\n", s);
}

/* Initialize local word with value */
void cginitlw(int v, int a) {
	if (NULL == Outfile) return;
	if (v >= -32768 && v <= 32767) {
		fprintf(Outfile, "         LHI   R0,%d\n", v);
	} else {
		fprintf(Outfile, "         L     R0,=F'%d'\n", v);
	}
	fprintf(Outfile, "         ST    R0,%d(R11)\n", a);
}

/* Call function by name */
void cgcall(char *s) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         BAS   R14,%s\n", s);
}

/* Call function via pointer in R2 */
void cgcalr(void) {
	gen("BASR  R14,R2");
}

/* Adjust stack pointer */
void cgstack(int n) {
	if (NULL == Outfile) return;
	if (n >= -32768 && n <= 32767) {
		fprintf(Outfile, "         AHI   R15,%d\n", n);
	} else {
		fprintf(Outfile, "         A     R15,=F'%d'\n", n);
	}
}

/* Function entry - save registers and set up frame */
void cgentry(void) {
	gen("STM   R14,R12,12(R13)");
	gen("LR    R11,R15");
	gen("AHI   R15,-96");
	gen("ST    R13,4(R15)");
	gen("LR    R13,R15");
}

/* Function exit - restore registers and return */
void cgexit(void) {
	gen("L     R13,4(R13)");
	gen("LM    R14,R12,12(R13)");
	gen("BR    R14");
}

/* Define byte constant */
void cgdefb(int v) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         DC    X'%02X'\n", v & 0xFF);
}

/* Define word (fullword) constant */
void cgdefw(int v) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         DC    F'%d'\n", v);
}

/* Define pointer constant */
void cgdefp(int v) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         DC    A(%d)\n", v);
}

/* Define label reference */
void cgdefl(int v) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         DC    A(%c%d)\n", LPREFIX, v);
}

/* Define character constant */
void cgdefc(int c) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "         DC    C'%c'\n", c);
}

/* Define BSS (uninitialized storage) */
void cgbss(char *s, int z) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "%s       DS    %dX\n", s, z);
}
