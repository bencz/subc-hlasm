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
         ENTRY Cstack
Cstack       DS    80X
         ENTRY Csp
Csp              DC    F'0'
*
* CODE SECTION
*
         ENTRY Cstack_init
Cstack_init	STM   R14,R12,12(R13)
	LR    R11,R15
	AHI   R15,-96
	ST    R13,4(R15)
	LR    R13,R15
         LHI   R2,0
         ST    R2,Csp
L1       DS    0H
	L     R13,4(R13)
	LM    R14,R12,12(R13)
	BR    R14
         ENTRY Cstack_empty
Cstack_empty	STM   R14,R12,12(R13)
	LR    R11,R15
	AHI   R15,-96
	ST    R13,4(R15)
	LR    R13,R15
         L     R2,Csp
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,0
         SR    R4,R4
         CR    R3,R2
         BNE    L3
         AHI   R4,1
L3      DS    0H
         LR    R2,R4
         B     L2
L2       DS    0H
	L     R13,4(R13)
	LM    R14,R12,12(R13)
	BR    R14
         ENTRY Cstack_full
Cstack_full	STM   R14,R12,12(R13)
	LR    R11,R15
	AHI   R15,-96
	ST    R13,4(R15)
	LR    R13,R15
         L     R2,Csp
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,20
         SR    R4,R4
         CR    R3,R2
         BL    L5
         AHI   R4,1
L5      DS    0H
         LR    R2,R4
         B     L4
L4       DS    0H
	L     R13,4(R13)
	LM    R14,R12,12(R13)
	BR    R14
         ENTRY Cpush
Cpush   	STM   R14,R12,12(R13)
	LR    R11,R15
	AHI   R15,-96
	ST    R13,4(R15)
	LR    R13,R15
         LHI   R0,0
	AHI   R15,-4
	ST    R0,0(R15)
         BAS   R14,Cstack_full
         AHI   R15,4
         LTR   R2,R2
         LHI   R2,1
         BZ    L7
         SR    R2,R2
L7      DS    0H
         LTR   R2,R2
         BZ    L8
         LA    R2,Cstack
	AHI   R15,-4
	ST    R2,0(R15)
         L     R2,Csp
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
         L     R2,Csp
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R2,1
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
         ST    R2,Csp
L8       DS    0H
L6       DS    0H
	L     R13,4(R13)
	LM    R14,R12,12(R13)
	BR    R14
         ENTRY Cpop
Cpop    	STM   R14,R12,12(R13)
	LR    R11,R15
	AHI   R15,-96
	ST    R13,4(R15)
	LR    R13,R15
         LHI   R0,0
	AHI   R15,-4
	ST    R0,0(R15)
         BAS   R14,Cstack_empty
         AHI   R15,4
         LTR   R2,R2
         LHI   R2,1
         BZ    L10
         SR    R2,R2
L10      DS    0H
         LTR   R2,R2
         BZ    L11
         L     R2,Csp
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
         ST    R2,Csp
         LA    R2,Cstack
	AHI   R15,-4
	ST    R2,0(R15)
         L     R2,Csp
	SLL   R2,2
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
	L     R2,0(R2)
         B     L9
L11      DS    0H
         LHI   R2,1
	LCR   R2,R2
         B     L9
L9       DS    0H
	L     R13,4(R13)
	LM    R14,R12,12(R13)
	BR    R14
         ENTRY Cpeek
Cpeek   	STM   R14,R12,12(R13)
	LR    R11,R15
	AHI   R15,-96
	ST    R13,4(R15)
	LR    R13,R15
         LHI   R0,0
	AHI   R15,-4
	ST    R0,0(R15)
         BAS   R14,Cstack_empty
         AHI   R15,4
         LTR   R2,R2
         LHI   R2,1
         BZ    L13
         SR    R2,R2
L13      DS    0H
         LTR   R2,R2
         BZ    L14
         LA    R2,Cstack
	AHI   R15,-4
	ST    R2,0(R15)
         L     R2,Csp
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
         B     L12
L14      DS    0H
         LHI   R2,1
	LCR   R2,R2
         B     L12
L12      DS    0H
	L     R13,4(R13)
	LM    R14,R12,12(R13)
	BR    R14
         ENTRY Cmain
Cmain   	STM   R14,R12,12(R13)
	LR    R11,R15
	AHI   R15,-96
	ST    R13,4(R15)
	LR    R13,R15
         AHI   R15,-4
         LHI   R0,0
	AHI   R15,-4
	ST    R0,0(R15)
         BAS   R14,Cstack_init
         AHI   R15,4
         LHI   R2,10
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R0,1
	AHI   R15,-4
	ST    R0,0(R15)
         BAS   R14,Cpush
         AHI   R15,8
         LHI   R2,20
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R0,1
	AHI   R15,-4
	ST    R0,0(R15)
         BAS   R14,Cpush
         AHI   R15,8
         LHI   R2,30
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R0,1
	AHI   R15,-4
	ST    R0,0(R15)
         BAS   R14,Cpush
         AHI   R15,8
         LHI   R2,0
         ST    R2,-4(R11)
L17      DS    0H
         LHI   R0,0
	AHI   R15,-4
	ST    R0,0(R15)
         BAS   R14,Cstack_empty
         AHI   R15,4
         LTR   R2,R2
         LHI   R2,1
         BZ    L18
         SR    R2,R2
L18      DS    0H
         LTR   R2,R2
         BZ    L16
         L     R2,-4(R11)
	AHI   R15,-4
	ST    R2,0(R15)
         LHI   R0,0
	AHI   R15,-4
	ST    R0,0(R15)
         BAS   R14,Cpop
         AHI   R15,4
	L     R3,0(R15)
	AHI   R15,4
	AR    R2,R3
         ST    R2,-4(R11)
         B     L17
L16      DS    0H
         L     R2,-4(R11)
         B     L15
L15      DS    0H
         AHI   R15,4
	L     R13,4(R13)
	LM    R14,R12,12(R13)
	BR    R14
*
         END
