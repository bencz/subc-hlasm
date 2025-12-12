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
         ENTRY Cglobal_val
Cglobal_val         DC    F'0'
*
* CODE SECTION
*
         ENTRY Cmain
Cmain   	STM   R14,R12,12(R13)
	LR    R11,R15
	AHI   R15,-96
	ST    R13,4(R15)
	LR    R13,R15
         AHI   R15,-8
         LHI   R2,42
         ST    R2,-4(R11)
         LA    R2,-4(R11)
         ST    R2,-8(R11)
         L     R2,-8(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,100
	L     R4,0(R15)
	AHI   R15,4
	ST    R2,0(R4)
         L     R2,-8(R11)
	L     R2,0(R2)
         ST    R2,Cglobal_val
         L     R2,-4(R11)
         B     L1
L1       DS    0H
         AHI   R15,8
	L     R13,4(R13)
	LM    R14,R12,12(R13)
	BR    R14
*
         END
