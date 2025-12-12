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
         ENTRY Cmatrix
Cmatrix       DS    64X
*
* CODE SECTION
*
         ENTRY Cget
Cget    	STM   R14,R12,12(R13)
	LR    R11,R15
	AHI   R15,-96
	ST    R13,4(R15)
	LR    R13,R15
         LA    R2,Cmatrix
	AHI   R15,-4
	ST    R2,0(R15)
         L     R2,16(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,4
	L     R3,0(R15)
	AHI   R15,4
	MR    R2,R3
	LR    R2,R3
	AHI   R15,-4
	ST    R2,0(R15)
         L     R2,12(R11)
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
	SLL   R2,2
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
	L     R2,0(R2)
         B     L1
L1       DS    0H
	L     R13,4(R13)
	LM    R14,R12,12(R13)
	BR    R14
         ENTRY Cset
Cset    	STM   R14,R12,12(R13)
	LR    R11,R15
	AHI   R15,-96
	ST    R13,4(R15)
	LR    R13,R15
         LA    R2,Cmatrix
	AHI   R15,-4
	ST    R2,0(R15)
         L     R2,20(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,4
	L     R3,0(R15)
	AHI   R15,4
	MR    R2,R3
	LR    R2,R3
	AHI   R15,-4
	ST    R2,0(R15)
         L     R2,16(R11)
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
	SLL   R2,2
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
	AHI   R15,-4
	ST    R2,0(R15)
         L     R2,12(R11)
	L     R4,0(R15)
	AHI   R15,4
	ST    R2,0(R4)
L2       DS    0H
	L     R13,4(R13)
	LM    R14,R12,12(R13)
	BR    R14
         ENTRY Cmain
Cmain   	STM   R14,R12,12(R13)
	LR    R11,R15
	AHI   R15,-96
	ST    R13,4(R15)
	LR    R13,R15
         AHI   R15,-12
         LHI   R2,0
         ST    R2,-4(R11)
L4       DS    0H
         L     R2,-4(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,4
         SR    R4,R4
         CR    R3,R2
         BNL    L8
         AHI   R4,1
L8      DS    0H
         LR    R2,R4
         LTR   R2,R2
         BZ    L6
         B     L5
L7       DS    0H
         L     R2,-4(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,1
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
         ST    R2,-4(R11)
         B     L4
L5       DS    0H
         LHI   R2,0
         ST    R2,-8(R11)
L9       DS    0H
         L     R2,-8(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,4
         SR    R4,R4
         CR    R3,R2
         BNL    L13
         AHI   R4,1
L13      DS    0H
         LR    R2,R4
         LTR   R2,R2
         BZ    L11
         B     L10
L12      DS    0H
         L     R2,-8(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,1
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
         ST    R2,-8(R11)
         B     L9
L10      DS    0H
         L     R2,-4(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         L     R2,-8(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         L     R2,-4(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,4
	L     R3,0(R15)
	AHI   R15,4
	MR    R2,R3
	LR    R2,R3
	AHI   R15,-4
	ST    R2,0(R15)
         L     R2,-8(R11)
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R0,3
	AHI   R15,-4
	ST    R0,0(R15)
         BAS   R14,Cset
         AHI   R15,16
         B     L12
L11      DS    0H
         B     L7
L6       DS    0H
         LHI   R2,0
         ST    R2,-12(R11)
         LHI   R2,0
         ST    R2,-4(R11)
L14      DS    0H
         L     R2,-4(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,4
         SR    R4,R4
         CR    R3,R2
         BNL    L18
         AHI   R4,1
L18      DS    0H
         LR    R2,R4
         LTR   R2,R2
         BZ    L16
         B     L15
L17      DS    0H
         L     R2,-4(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,1
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
         ST    R2,-4(R11)
         B     L14
L15      DS    0H
         L     R2,-12(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         L     R2,-4(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         L     R2,-4(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R0,2
	AHI   R15,-4
	ST    R0,0(R15)
         BAS   R14,Cget
         AHI   R15,12
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
         ST    R2,-12(R11)
         B     L17
L16      DS    0H
         L     R2,-12(R11)
         B     L3
L3       DS    0H
         AHI   R15,12
	L     R13,4(R13)
	LM    R14,R12,12(R13)
	BR    R14
*
         END
