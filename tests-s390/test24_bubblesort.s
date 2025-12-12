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
         ENTRY Cswap
Cswap   	STM   R14,R12,12(R13)
	LR    R11,R15
	AHI   R15,-96
	ST    R13,4(R15)
	LR    R13,R15
         AHI   R15,-4
         L     R2,16(R11)
	L     R2,0(R2)
         ST    R2,-4(R11)
         L     R2,16(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         L     R2,12(R11)
	L     R2,0(R2)
	L     R4,0(R15)
	AHI   R15,4
	ST    R2,0(R4)
         L     R2,12(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         L     R2,-4(R11)
	L     R4,0(R15)
	AHI   R15,4
	ST    R2,0(R4)
L1       DS    0H
         AHI   R15,4
	L     R13,4(R13)
	LM    R14,R12,12(R13)
	BR    R14
         ENTRY Cbubble_sort
Cbubble_sort	STM   R14,R12,12(R13)
	LR    R11,R15
	AHI   R15,-96
	ST    R13,4(R15)
	LR    R13,R15
         AHI   R15,-8
         LHI   R2,0
         ST    R2,-4(R11)
L3       DS    0H
         L     R2,-4(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         L     R2,12(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,1
	L     R3,0(R15)
	AHI   R15,4
	LR    R0,R2
	LR    R2,R3
	LR    R3,R0
	SR    R3,R2
	LR    R2,R3
         SR    R4,R4
         CR    R3,R2
         BNL    L7
         AHI   R4,1
L7      DS    0H
         LR    R2,R4
         LTR   R2,R2
         BZ    L5
         B     L4
L6       DS    0H
         L     R2,-4(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,1
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
         ST    R2,-4(R11)
         B     L3
L4       DS    0H
         LHI   R2,0
         ST    R2,-8(R11)
L8       DS    0H
         L     R2,-8(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         L     R2,12(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         L     R2,-4(R11)
	L     R3,0(R15)
	AHI   R15,4
	LR    R0,R2
	LR    R2,R3
	LR    R3,R0
	SR    R3,R2
	LR    R2,R3
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,1
	L     R3,0(R15)
	AHI   R15,4
	LR    R0,R2
	LR    R2,R3
	LR    R3,R0
	SR    R3,R2
	LR    R2,R3
         SR    R4,R4
         CR    R3,R2
         BNL    L12
         AHI   R4,1
L12      DS    0H
         LR    R2,R4
         LTR   R2,R2
         BZ    L10
         B     L9
L11      DS    0H
         L     R2,-8(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,1
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
         ST    R2,-8(R11)
         B     L8
L9       DS    0H
         LA    R2,Carr
	AHI   R15,-4
	ST    R2,0(R15)
         L     R2,-8(R11)
	SLL   R2,2
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
	L     R2,0(R2)
	AHI   R15,-4
	ST    R2,0(R15)
         LA    R2,Carr
	AHI   R15,-4
	ST    R2,0(R15)
         L     R2,-8(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,1
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
	SLL   R2,2
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
	L     R2,0(R2)
         SR    R4,R4
         CR    R3,R2
         BNH    L13
         AHI   R4,1
L13      DS    0H
         LR    R2,R4
         LTR   R2,R2
         BZ    L14
         LA    R2,Carr
	AHI   R15,-4
	ST    R2,0(R15)
         L     R2,-8(R11)
	SLL   R2,2
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
	AHI   R15,-4
	ST    R2,0(R15)
         LA    R2,Carr
	AHI   R15,-4
	ST    R2,0(R15)
         L     R2,-8(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,1
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
	SLL   R2,2
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R0,2
	AHI   R15,-4
	ST    R0,0(R15)
         BAS   R14,Cswap
         AHI   R15,12
L14      DS    0H
         B     L11
L10      DS    0H
         B     L6
L5       DS    0H
L2       DS    0H
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
         LA    R2,Carr
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,0
	SLL   R2,2
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,64
	L     R4,0(R15)
	AHI   R15,4
	ST    R2,0(R4)
         LA    R2,Carr
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,1
	SLL   R2,2
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,34
	L     R4,0(R15)
	AHI   R15,4
	ST    R2,0(R4)
         LA    R2,Carr
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,2
	SLL   R2,2
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,25
	L     R4,0(R15)
	AHI   R15,4
	ST    R2,0(R4)
         LA    R2,Carr
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,3
	SLL   R2,2
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,12
	L     R4,0(R15)
	AHI   R15,4
	ST    R2,0(R4)
         LA    R2,Carr
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,4
	SLL   R2,2
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,22
	L     R4,0(R15)
	AHI   R15,4
	ST    R2,0(R4)
         LA    R2,Carr
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,5
	SLL   R2,2
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,11
	L     R4,0(R15)
	AHI   R15,4
	ST    R2,0(R4)
         LA    R2,Carr
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,6
	SLL   R2,2
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,90
	L     R4,0(R15)
	AHI   R15,4
	ST    R2,0(R4)
         LA    R2,Carr
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,7
	SLL   R2,2
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,5
	L     R4,0(R15)
	AHI   R15,4
	ST    R2,0(R4)
         LA    R2,Carr
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,8
	SLL   R2,2
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,77
	L     R4,0(R15)
	AHI   R15,4
	ST    R2,0(R4)
         LA    R2,Carr
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,9
	SLL   R2,2
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,1
	L     R4,0(R15)
	AHI   R15,4
	ST    R2,0(R4)
         LHI   R2,10
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R0,1
	AHI   R15,-4
	ST    R0,0(R15)
         BAS   R14,Cbubble_sort
         AHI   R15,8
         LA    R2,Carr
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,0
	SLL   R2,2
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
	L     R2,0(R2)
         B     L15
L15      DS    0H
	L     R13,4(R13)
	LM    R14,R12,12(R13)
	BR    R14
*
         END
