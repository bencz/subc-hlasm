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
         ENTRY Cmain
Cmain   	STM   R14,R12,12(R13)
	LR    R11,R15
	AHI   R15,-96
	ST    R13,4(R15)
	LR    R13,R15
         AHI   R15,-8
         LHI   R2,2
         ST    R2,-4(R11)
         L     R2,-4(R11)
         B     L3
L4       DS    0H
         LHI   R2,10
         ST    R2,-8(R11)
         B     L2
L5       DS    0H
         LHI   R2,20
         ST    R2,-8(R11)
         B     L2
L6       DS    0H
         LHI   R2,30
         ST    R2,-8(R11)
         B     L2
L7       DS    0H
         LHI   R2,0
         ST    R2,-8(R11)
         B     L2
         B     L2
L3       DS    0H
         LA    R4,L8
	BAS   R14,@SWITCH
*
* DATA SECTION
*
L8       DS    0H
         DC    F'3'
         DC    F'1'
         DC    A(L4)
         DC    F'2'
         DC    A(L5)
         DC    F'3'
         DC    A(L6)
         DC    A(L7)
*
* CODE SECTION
*
L2       DS    0H
         L     R2,-8(R11)
         B     L1
L1       DS    0H
         AHI   R15,8
	L     R13,4(R13)
	LM    R14,R12,12(R13)
	BR    R14
*
         END
