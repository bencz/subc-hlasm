         COPY  PDPTOP
         CSECT
	* Program text area
	         DS    0F
	*
	* Code Section
	*
	*
	* Data Section
	*
	GLOB9214 EQU   *
	         DC    C'a'
	         DC    C'b'
	         DC    C'c'
	         DC    X'00'
	*
	* Code Section
	*
	         DS    0F
	* X-func SOMA1913 prologue
	SOMA1913 PDPPRLG CINDEX=0,FRAME=184,BASER=12,ENTRY=YES
	         B     @@FEN0
	         LTORG
	@@FEN0   EQU   *
	         DROP  12
	         BALR  12,0
	         USING *,12
	@@PG0    EQU   *
	         LR    11,1
	         L     10,=A(@@PGT0)
	* Function SOMA1913 code
	         A     11,=F'96'
	         L     2,=F'0'
	         ST    2,96(,13)
	         L     2,=F'0'
	         ST    2,92(,13)
L2:
	         L     2,92(,13)
	         ST    2,184(,13)
	         L     2,4(11)
	         L     3,184(,13)
	         CR    2,3
	         BL   L6
	         B     L4
L6:
	         B     L3
L5:
	         L     2,=F'1'
	         ST    2,184(,13)
	         L     2,92(,13)
	         L     3,184(,13)
	         AR    2,3
	         ST    2,92(,13)
	         B     L2
L3:
	         L     2,92(,13)
	         SLA   2,2
	         ST    2,184(,13)
	         L     2,0(11)
	         L     3,184(,13)
	         AR    2,3
	         L     2,0(,2)
	         ST    2,184(,13)
	         L     2,96(,13)
	         L     3,184(,13)
	         AR    2,3
	         ST    2,96(,13)
	         B     L5
L4:
	         L     2,96(,13)
	         B     L1
L1:
	         A     11,=F'-96'
	         LR    15,2
	* Function SOMA1913 epilogue
	         PDPEPIL
	* Function SOMA1913 literal pool
	         DS    0F
	         LTORG
	* Function SOMA1913 page table
	         DS    0F
	@@PGT0   EQU   *
	         DC    A(@@PG0)
	         DS    0F
	* X-func TAMA0533 prologue
	TAMA0533 PDPPRLG CINDEX=1,FRAME=184,BASER=12,ENTRY=YES
	         B     @@FEN1
	         LTORG
	@@FEN1   EQU   *
	         DROP  12
	         BALR  12,0
	         USING *,12
	@@PG1    EQU   *
	         LR    11,1
	         L     10,=A(@@PGT1)
	* Function TAMA0533 code
	         A     11,=F'92'
	         L     2,=F'0'
	         ST    2,92(,13)
L9:
	         L     2,92(,13)
	         ST    2,180(,13)
	         L     2,0(11)
	         L     3,180(,13)
	         AR    2,3
	         SR    0,0
	         IC    0,0(,2)
	         LR    2,0
	         ST    2,180(,13)
	         L     2,=F'0'
	         L     3,180(,13)
	         CR    2,3
	         BNE   L10
	         B     L8
L10:
	         L     2,=F'1'
	         ST    2,180(,13)
	         L     2,92(,13)
	         L     3,180(,13)
	         AR    2,3
	         ST    2,92(,13)
	         B     L9
L8:
	         L     2,92(,13)
	         B     L7
L7:
	         A     11,=F'-92'
	         LR    15,2
	* Function TAMA0533 epilogue
	         PDPEPIL
	* Function TAMA0533 literal pool
	         DS    0F
	         LTORG
	* Function TAMA0533 page table
	         DS    0F
	@@PGT1   EQU   *
	         DC    A(@@PG1)
	         DS    0F
	* X-func MAIN prologue
	MAIN     PDPPRLG CINDEX=2,FRAME=200,BASER=12,ENTRY=YES
	         B     @@FEN2
	         LTORG
	@@FEN2   EQU   *
	         DROP  12
	         BALR  12,0
	         USING *,12
	@@PG2    EQU   *
	         LR    11,1
	         L     10,=A(@@PGT2)
	* Function MAIN code
	         A     11,=F'108'
	         LA    2,100(,13)
	         ST    2,196(,13)
	         L     2,=F'1'
	         L     3,196(,13)
	         L     3,192(,13)
	         ST    2,0(,3)
	         LA    2,100(,13)
	         ST    2,192(,13)
	         L     2,=F'4'
	         L     3,192(,13)
	         AR    2,3
	         ST    2,192(,13)
	         L     2,=F'2'
	         L     3,192(,13)
	         L     3,188(,13)
	         ST    2,0(,3)
	         LA    2,100(,13)
	         ST    2,188(,13)
	         L     2,=F'8'
	         L     3,188(,13)
	         AR    2,3
	         ST    2,188(,13)
	         L     2,=F'3'
	         L     3,188(,13)
	         L     3,184(,13)
	         ST    2,0(,3)
	         L     2,=F'3'
	         ST    2,184(,13)
	         LA    2,100(,13)
	         ST    2,188(,13)
	         LA    1,192(,13)
	         L     15,=A(SOMA1913)
	         BALR  14,15
	         LR    2,15
	         A     11,=F'8'
	         ST    2,104(,13)
	         LA    2,GLOB9214
	         ST    2,200(,13)
	         LA    1,204(,13)
	         L     15,=A(TAMA0533)
	         BALR  14,15
	         LR    2,15
	         A     11,=F'4'
	         ST    2,108(,13)
	         L     2,=F'6'
	         ST    2,208(,13)
	         L     2,104(,13)
	         L     3,208(,13)
	         CR    2,3
	         BNE   L13
	         B     L12
L13:
	         L     2,=F'1'
	         B     L11
L12:
	         L     2,=F'3'
	         ST    2,208(,13)
	         L     2,108(,13)
	         L     3,208(,13)
	         CR    2,3
	         BNE   L15
	         B     L14
L15:
	         L     2,=F'3'
	         B     L11
L14:
	         L     2,=F'0'
	         B     L11
L11:
	         A     11,=F'-108'
	         LR    15,2
	* Function MAIN epilogue
	         PDPEPIL
	* Function MAIN literal pool
	         DS    0F
	         LTORG
	* Function MAIN page table
	         DS    0F
	@@PGT2   EQU   *
	         DC    A(@@PG2)
         END   @@MAIN
