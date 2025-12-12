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
         ENTRY Cfactorial
Cfactorial	STM   R14,R12,12(R13)
	LR    R11,R15
	AHI   R15,-96
	ST    R13,4(R15)
	LR    R13,R15
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
         LHI   R2,1
         B     L1
L3       DS    0H
         L     R2,12(R11)
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
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R0,1
	AHI   R15,-4
	ST    R0,0(R15)
         BAS   R14,Cfactorial
         AHI   R15,8
	L     R3,0(R15)
	AHI   R15,4
	MR    R2,R3
	LR    R2,R3
         B     L1
L1       DS    0H
	L     R13,4(R13)
	LM    R14,R12,12(R13)
	BR    R14
         ENTRY Cmain
Cmain   	STM   R14,R12,12(R13)
	LR    R11,R15
	AHI   R15,-96
	ST    R13,4(R15)
	LR    R13,R15
         LHI   R2,5
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R0,1
	AHI   R15,-4
	ST    R0,0(R15)
         BAS   R14,Cfactorial
         AHI   R15,8
         B     L4
L4       DS    0H
	L     R13,4(R13)
	LM    R14,R12,12(R13)
	BR    R14
*
         END
