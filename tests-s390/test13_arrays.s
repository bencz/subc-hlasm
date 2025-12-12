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
*
* DATA SECTION
*
         ENTRY Carr
Carr       DS    40X
*
* CODE SECTION
*
         ENTRY Cmain
Cmain   	STM   R14,R12,12(R13)
	LR    R11,R15
	AHI   R15,-96
	ST    R13,4(R15)
	LR    R13,R15
         AHI   R15,-28
         LHI   R2,0
         ST    R2,-4(R11)
L2       DS    0H
         L     R2,-4(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,10
         SR    R4,R4
         CR    R3,R2
         BNL    L6
         AHI   R4,1
L6      DS    0H
         LR    R2,R4
         LTR   R2,R2
         BZ    L4
         B     L3
L5       DS    0H
         L     R2,-4(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,1
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
         ST    R2,-4(R11)
         B     L2
L3       DS    0H
         LA    R2,Carr
	AHI   R15,-4
	ST    R2,0(R15)
         L     R2,-4(R11)
	SLL   R2,2
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
	AHI   R15,-4
	ST    R2,0(R15)
         L     R2,-4(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,2
	L     R3,0(R15)
	AHI   R15,4
	MR    R2,R3
	LR    R2,R3
	L     R4,0(R15)
	AHI   R15,4
	ST    R2,0(R4)
         B     L5
L4       DS    0H
         LHI   R2,0
         ST    R2,-4(R11)
L7       DS    0H
         L     R2,-4(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,5
         SR    R4,R4
         CR    R3,R2
         BNL    L11
         AHI   R4,1
L11      DS    0H
         LR    R2,R4
         LTR   R2,R2
         BZ    L9
         B     L8
L10      DS    0H
         L     R2,-4(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,1
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
         ST    R2,-4(R11)
         B     L7
L8       DS    0H
         LA    R2,-28(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         L     R2,-4(R11)
	SLL   R2,2
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
	AHI   R15,-4
	ST    R2,0(R15)
         L     R2,-4(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,1
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
	L     R4,0(R15)
	AHI   R15,4
	ST    R2,0(R4)
         B     L10
L9       DS    0H
         LHI   R2,0
         ST    R2,-8(R11)
         LHI   R2,0
         ST    R2,-4(R11)
L12      DS    0H
         L     R2,-4(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,10
         SR    R4,R4
         CR    R3,R2
         BNL    L16
         AHI   R4,1
L16      DS    0H
         LR    R2,R4
         LTR   R2,R2
         BZ    L14
         B     L13
L15      DS    0H
         L     R2,-4(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,1
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
         ST    R2,-4(R11)
         B     L12
L13      DS    0H
         L     R2,-8(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         LA    R2,Carr
	AHI   R15,-4
	ST    R2,0(R15)
         L     R2,-4(R11)
	SLL   R2,2
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
	L     R2,0(R2)
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
         ST    R2,-8(R11)
         B     L15
L14      DS    0H
         LHI   R2,0
         ST    R2,-4(R11)
L17      DS    0H
         L     R2,-4(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,5
         SR    R4,R4
         CR    R3,R2
         BNL    L21
         AHI   R4,1
L21      DS    0H
         LR    R2,R4
         LTR   R2,R2
         BZ    L19
         B     L18
L20      DS    0H
         L     R2,-4(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,1
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
         ST    R2,-4(R11)
         B     L17
L18      DS    0H
         L     R2,-8(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         LA    R2,-28(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         L     R2,-4(R11)
	SLL   R2,2
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
	L     R2,0(R2)
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
         ST    R2,-8(R11)
         B     L20
L19      DS    0H
         L     R2,-8(R11)
         B     L1
L1       DS    0H
         AHI   R15,28
	L     R13,4(R13)
	LM    R14,R12,12(R13)
	BR    R14
*
         END
