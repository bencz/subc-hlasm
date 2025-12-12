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
         ENTRY Cfib
Cfib       DS    80X
*
* CODE SECTION
*
         ENTRY Cfibonacci
Cfibonacci	STM   R14,R12,12(R13)
	LR    R11,R15
	AHI   R15,-96
	ST    R13,4(R15)
	LR    R13,R15
         AHI   R15,-4
         LA    R2,Cfib
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,0
	SLL   R2,2
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,0
	L     R4,0(R15)
	AHI   R15,4
	ST    R2,0(R4)
         LA    R2,Cfib
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,1
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
         LHI   R2,2
         ST    R2,-4(R11)
L2       DS    0H
         L     R2,-4(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         L     R2,12(R11)
         SR    R4,R4
         CR    R3,R2
         BH    L6
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
         LA    R2,Cfib
	AHI   R15,-4
	ST    R2,0(R15)
         L     R2,-4(R11)
	SLL   R2,2
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
	AHI   R15,-4
	ST    R2,0(R15)
         LA    R2,Cfib
	AHI   R15,-4
	ST    R2,0(R15)
         L     R2,-4(R11)
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
	SLL   R2,2
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
	L     R2,0(R2)
	AHI   R15,-4
	ST    R2,0(R15)
         LA    R2,Cfib
	AHI   R15,-4
	ST    R2,0(R15)
         L     R2,-4(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,2
	L     R3,0(R15)
	AHI   R15,4
	LR    R0,R2
	LR    R2,R3
	LR    R3,R0
	SR    R3,R2
	LR    R2,R3
	SLL   R2,2
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
	L     R2,0(R2)
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
	L     R4,0(R15)
	AHI   R15,4
	ST    R2,0(R4)
         B     L5
L4       DS    0H
         LA    R2,Cfib
	AHI   R15,-4
	ST    R2,0(R15)
         L     R2,12(R11)
	SLL   R2,2
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
	L     R2,0(R2)
         B     L1
L1       DS    0H
         AHI   R15,4
	L     R13,4(R13)
	LM    R14,R12,12(R13)
	BR    R14
         ENTRY Cmain
Cmain   	STM   R14,R12,12(R13)
	LR    R11,R15
	AHI   R15,-96
	ST    R13,4(R15)
	LR    R13,R15
         LHI   R2,10
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R0,1
	AHI   R15,-4
	ST    R0,0(R15)
         BAS   R14,Cfibonacci
         AHI   R15,8
         B     L7
L7       DS    0H
	L     R13,4(R13)
	LM    R14,R12,12(R13)
	BR    R14
*
         END
