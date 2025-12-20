         COPY  PDPTOP
         CSECT
	* Program text area
	         DS    0F
	*
	* Code Section
	*
	         DS    0F
	* X-func SOMA prologue
	SOMA     PDPPRLG CINDEX=0,FRAME=176,BASER=12,ENTRY=YES
	         B     @@FEN0
	         LTORG
	@@FEN0   EQU   *
	         DROP  12
	         BALR  12,0
	         USING *,12
	@@PG0    EQU   *
	         LR    11,1
	         L     10,=A(@@PGT0)
	* Function SOMA code
	         A     11,=F'88'
	         L     2,4(11)
	         ST    2,176(,13)
	         L     2,0(11)
	         L     3,176(,13)
	         AR    2,3
	         B     L1
L1:
	         A     11,=F'-88'
	         LR    15,2
	* Function SOMA epilogue
	         PDPEPIL
	* Function SOMA literal pool
	         DS    0F
	         LTORG
	* Function SOMA page table
	         DS    0F
	@@PGT0   EQU   *
	         DC    A(@@PG0)
	         DS    0F
	* X-func MAIN prologue
	MAIN     PDPPRLG CINDEX=1,FRAME=192,BASER=12,ENTRY=YES
	         B     @@FEN1
	         LTORG
	@@FEN1   EQU   *
	         DROP  12
	         BALR  12,0
	         USING *,12
	@@PG1    EQU   *
	         LR    11,1
	         L     10,=A(@@PGT1)
	* Function MAIN code
	         A     11,=F'100'
	         L     2,=F'10'
	         ST    2,92(,13)
	         L     2,=F'20'
	         ST    2,96(,13)
	         L     2,96(,13)
	         ST    2,188(,13)
	         L     2,92(,13)
	         ST    2,192(,13)
	         LA    1,196(,13)
	         L     15,=A(SOMA)
	         BALR  14,15
	         LR    2,15
	         A     11,=F'8'
	         ST    2,100(,13)
	         L     2,=F'30'
	         ST    2,204(,13)
	         L     2,100(,13)
	         L     3,204(,13)
	         CR    2,3
	         BNE   L4
	         B     L3
L4:
	         L     2,=F'1'
	         B     L2
L3:
	         L     2,=F'0'
	         B     L2
L2:
	         A     11,=F'-100'
	         LR    15,2
	* Function MAIN epilogue
	         PDPEPIL
	* Function MAIN literal pool
	         DS    0F
	         LTORG
	* Function MAIN page table
	         DS    0F
	@@PGT1   EQU   *
	         DC    A(@@PG1)
         END   @@MAIN
