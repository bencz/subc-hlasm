*
* CODE SECTION
*
*
* SubC Compiler Output - S/390 HLASM
*
         PRINT NOGEN
SUBC     CSECT
         USING SUBC,R12
*
* Register equates
R0       EQU   0
R1       EQU   1
R2       EQU   2
R3       EQU   3
R4       EQU   4
R5       EQU   5
R6       EQU   6
R7       EQU   7
R8       EQU   8
R9       EQU   9
R10      EQU   10
R11      EQU   11
R12      EQU   12
R13      EQU   13
R14      EQU   14
R15      EQU   15
*
         ENTRY Cis_prime
Cis_prime	STM   R14,R12,12(R13)
	LR    R11,R15
	AHI   R15,-96
	ST    R13,4(R15)
	LR    R13,R15
         AHI   R15,-4
         L     R2,12(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,1
         SR    R4,R4
         CR    R3,R2
         BH    L2
         AHI   R4,1
L2      DS    0H
         LR    R2,R4
         LTR   R2,R2
         BZ    L3
         LHI   R2,0
         B     L1
L3       DS    0H
         L     R2,12(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,3
         SR    R4,R4
         CR    R3,R2
         BH    L4
         AHI   R4,1
L4      DS    0H
         LR    R2,R4
         LTR   R2,R2
         BZ    L5
         LHI   R2,1
         B     L1
L5       DS    0H
         L     R2,12(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,2
	L     R3,0(R15)
	AHI   R15,4
	LR    R0,R2
	LR    R2,R3
	LR    R3,R0
	LR    R4,R3
	SRDA  R4,32
	DR    R4,R2
	LR    R2,R4
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,0
         SR    R4,R4
         CR    R3,R2
         BNE    L6
         AHI   R4,1
L6      DS    0H
         LR    R2,R4
         LTR   R2,R2
         BZ    L7
         LHI   R2,0
         B     L1
L7       DS    0H
         L     R2,12(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,3
	L     R3,0(R15)
	AHI   R15,4
	LR    R0,R2
	LR    R2,R3
	LR    R3,R0
	LR    R4,R3
	SRDA  R4,32
	DR    R4,R2
	LR    R2,R4
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,0
         SR    R4,R4
         CR    R3,R2
         BNE    L8
         AHI   R4,1
L8      DS    0H
         LR    R2,R4
         LTR   R2,R2
         BZ    L9
         LHI   R2,0
         B     L1
L9       DS    0H
         LHI   R2,5
         ST    R2,-4(R11)
L11      DS    0H
         L     R2,-4(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         L     R2,-4(R11)
	L     R3,0(R15)
	AHI   R15,4
	MR    R2,R3
	LR    R2,R3
	AHI   R15,-4
	ST    R2,0(R15)
         L     R2,12(R11)
         SR    R4,R4
         CR    R3,R2
         BH    L12
         AHI   R4,1
L12      DS    0H
         LR    R2,R4
         LTR   R2,R2
         BZ    L10
         L     R2,12(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         L     R2,-4(R11)
	L     R3,0(R15)
	AHI   R15,4
	LR    R0,R2
	LR    R2,R3
	LR    R3,R0
	LR    R4,R3
	SRDA  R4,32
	DR    R4,R2
	LR    R2,R4
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,0
         SR    R4,R4
         CR    R3,R2
         BNE    L13
         AHI   R4,1
L13      DS    0H
         LR    R2,R4
         LTR   R2,R2
         BZ    L14
         LHI   R2,0
         B     L1
L14      DS    0H
         L     R2,12(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         L     R2,-4(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,2
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
	L     R3,0(R15)
	AHI   R15,4
	LR    R0,R2
	LR    R2,R3
	LR    R3,R0
	LR    R4,R3
	SRDA  R4,32
	DR    R4,R2
	LR    R2,R4
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,0
         SR    R4,R4
         CR    R3,R2
         BNE    L15
         AHI   R4,1
L15      DS    0H
         LR    R2,R4
         LTR   R2,R2
         BZ    L16
         LHI   R2,0
         B     L1
L16      DS    0H
         L     R2,-4(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,6
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
         ST    R2,-4(R11)
         B     L11
L10      DS    0H
         LHI   R2,1
         B     L1
L1       DS    0H
         AHI   R15,4
	L     R13,4(R13)
	LM    R14,R12,12(R13)
	BR    R14
         ENTRY Ccount_primes
Ccount_primes	STM   R14,R12,12(R13)
	LR    R11,R15
	AHI   R15,-96
	ST    R13,4(R15)
	LR    R13,R15
         AHI   R15,-8
         LHI   R2,0
         ST    R2,-8(R11)
         LHI   R2,2
         ST    R2,-4(R11)
L18      DS    0H
         L     R2,-4(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         L     R2,12(R11)
         SR    R4,R4
         CR    R3,R2
         BH    L22
         AHI   R4,1
L22      DS    0H
         LR    R2,R4
         LTR   R2,R2
         BZ    L20
         B     L19
L21      DS    0H
         L     R2,-4(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,1
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
         ST    R2,-4(R11)
         B     L18
L19      DS    0H
         L     R2,-4(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R0,1
	AHI   R15,-4
	ST    R0,0(R15)
         BAS   R14,Cis_prime
         AHI   R15,8
         LTR   R2,R2
         BZ    L23
         L     R2,-8(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,1
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
         ST    R2,-8(R11)
L23      DS    0H
         B     L21
L20      DS    0H
         L     R2,-8(R11)
         B     L17
L17      DS    0H
         AHI   R15,8
	L     R13,4(R13)
	LM    R14,R12,12(R13)
	BR    R14
         ENTRY Cmain
Cmain   	STM   R14,R12,12(R13)
	LR    R11,R15
	AHI   R15,-96
	ST    R13,4(R15)
	LR    R13,R15
         LHI   R2,30
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R0,1
	AHI   R15,-4
	ST    R0,0(R15)
         BAS   R14,Ccount_primes
         AHI   R15,8
         B     L24
L24      DS    0H
	L     R13,4(R13)
	LM    R14,R12,12(R13)
	BR    R14
*
         END
