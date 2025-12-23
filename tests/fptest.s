	.model small
	.code
	.data
	public	tests_passed
tests_passed:	dw	0
	public	tests_failed
tests_failed:	dw	0
	.code
	public	report
report:	push	bp
	mov	bp,sp
	mov	ax,[bp+6]
	or	ax,ax
	jnz	L3
	jmp	L2
L3:
	mov	ax,tests_passed
	mov	cx,1
	add	tests_passed,cx
	jmp	L4
L2:
	mov	ax,tests_failed
	mov	cx,1
	add	tests_failed,cx
L4:
L1:
	pop	bp
	ret
	public	float_add
float_add:	push	bp
	mov	bp,sp
	fld	dword ptr [bp+4]
	fld	dword ptr [bp+6]
	faddp	st(1),st
	jmp	L5
L5:
	pop	bp
	ret
	public	float_sub
float_sub:	push	bp
	mov	bp,sp
	fld	dword ptr [bp+4]
	fld	dword ptr [bp+6]
	fsubrp	st(1),st
	jmp	L6
L6:
	pop	bp
	ret
	public	float_mul
float_mul:	push	bp
	mov	bp,sp
	fld	dword ptr [bp+4]
	fld	dword ptr [bp+6]
	fmulp	st(1),st
	jmp	L7
L7:
	pop	bp
	ret
	public	float_div
float_div:	push	bp
	mov	bp,sp
	fld	dword ptr [bp+4]
	fld	dword ptr [bp+6]
	fdivrp	st(1),st
	jmp	L8
L8:
	pop	bp
	ret
	public	test_float_arithmetic
test_float_arithmetic:	push	bp
	mov	bp,sp
	add	sp,-14
	mov	ax,1
	mov	[bp-14],ax
	.data
L10:
	dd	0
	dd	1073217536
	fld	qword ptr L10
	.code
	fstp	dword ptr [bp-4]
	.data
L11:
	dd	0
	dd	1074003968
	fld	qword ptr L11
	.code
	fstp	dword ptr [bp-8]
	fld	dword ptr [bp-8]
	push	ax
	fld	dword ptr [bp-4]
	push	ax
	call	float_add
	add	sp,4
	fstp	dword ptr [bp-12]
	.data
L12:
	dd	0
	dd	1075052544
	fld	qword ptr L12
	.code
	fstp	dword ptr [bp-4]
	.data
L13:
	dd	0
	dd	1074266112
	fld	qword ptr L13
	.code
	fstp	dword ptr [bp-8]
	fld	dword ptr [bp-8]
	push	ax
	fld	dword ptr [bp-4]
	push	ax
	call	float_sub
	add	sp,4
	fstp	dword ptr [bp-12]
	.data
L14:
	dd	0
	dd	1073741824
	fld	qword ptr L14
	.code
	fstp	dword ptr [bp-4]
	.data
L15:
	dd	0
	dd	1074266112
	fld	qword ptr L15
	.code
	fstp	dword ptr [bp-8]
	fld	dword ptr [bp-8]
	push	ax
	fld	dword ptr [bp-4]
	push	ax
	call	float_mul
	add	sp,4
	fstp	dword ptr [bp-12]
	.data
L16:
	dd	0
	dd	1076101120
	fld	qword ptr L16
	.code
	fstp	dword ptr [bp-4]
	.data
L17:
	dd	0
	dd	1073741824
	fld	qword ptr L17
	.code
	fstp	dword ptr [bp-8]
	fld	dword ptr [bp-8]
	push	ax
	fld	dword ptr [bp-4]
	push	ax
	call	float_div
	add	sp,4
	fstp	dword ptr [bp-12]
	.data
L18:
	dd	0
	dd	1073741824
	fld	qword ptr L18
	.code
	fstp	dword ptr [bp-4]
	.data
L19:
	dd	0
	dd	1074266112
	fld	qword ptr L19
	.code
	fstp	dword ptr [bp-8]
	fld	dword ptr [bp-4]
	fld	dword ptr [bp-8]
	faddp	st(1),st
	fld	dword ptr [bp-4]
	fld	dword ptr [bp-8]
	fsubrp	st(1),st
	fmulp	st(1),st
	fstp	dword ptr [bp-12]
	mov	ax,[bp-14]
	jmp	L9
L9:
	add	sp,14
	pop	bp
	ret
	public	double_add
double_add:	push	bp
	mov	bp,sp
	fld	qword ptr [bp+4]
	fld	qword ptr [bp+6]
	faddp	st(1),st
	jmp	L20
L20:
	pop	bp
	ret
	public	double_sub
double_sub:	push	bp
	mov	bp,sp
	fld	qword ptr [bp+4]
	fld	qword ptr [bp+6]
	fsubrp	st(1),st
	jmp	L21
L21:
	pop	bp
	ret
	public	double_mul
double_mul:	push	bp
	mov	bp,sp
	fld	qword ptr [bp+4]
	fld	qword ptr [bp+6]
	fmulp	st(1),st
	jmp	L22
L22:
	pop	bp
	ret
	public	double_div
double_div:	push	bp
	mov	bp,sp
	fld	qword ptr [bp+4]
	fld	qword ptr [bp+6]
	fdivrp	st(1),st
	jmp	L23
L23:
	pop	bp
	ret
	public	test_double_arithmetic
test_double_arithmetic:	push	bp
	mov	bp,sp
	add	sp,-26
	mov	ax,1
	mov	[bp-26],ax
	.data
L25:
	dd	0
	dd	1073217536
	fld	qword ptr L25
	.code
	fstp	qword ptr [bp-8]
	.data
L26:
	dd	0
	dd	1074003968
	fld	qword ptr L26
	.code
	fstp	qword ptr [bp-16]
	fld	qword ptr [bp-16]
	push	ax
	fld	qword ptr [bp-8]
	push	ax
	call	double_add
	add	sp,4
	fstp	qword ptr [bp-24]
	.data
L27:
	dd	0
	dd	1075052544
	fld	qword ptr L27
	.code
	fstp	qword ptr [bp-8]
	.data
L28:
	dd	0
	dd	1074266112
	fld	qword ptr L28
	.code
	fstp	qword ptr [bp-16]
	fld	qword ptr [bp-16]
	push	ax
	fld	qword ptr [bp-8]
	push	ax
	call	double_sub
	add	sp,4
	fstp	qword ptr [bp-24]
	.data
L29:
	dd	0
	dd	1073741824
	fld	qword ptr L29
	.code
	fstp	qword ptr [bp-8]
	.data
L30:
	dd	0
	dd	1074266112
	fld	qword ptr L30
	.code
	fstp	qword ptr [bp-16]
	fld	qword ptr [bp-16]
	push	ax
	fld	qword ptr [bp-8]
	push	ax
	call	double_mul
	add	sp,4
	fstp	qword ptr [bp-24]
	.data
L31:
	dd	0
	dd	1076101120
	fld	qword ptr L31
	.code
	fstp	qword ptr [bp-8]
	.data
L32:
	dd	0
	dd	1073741824
	fld	qword ptr L32
	.code
	fstp	qword ptr [bp-16]
	fld	qword ptr [bp-16]
	push	ax
	fld	qword ptr [bp-8]
	push	ax
	call	double_div
	add	sp,4
	fstp	qword ptr [bp-24]
	.data
L33:
	dd	1116494300
	dd	1072939210
	fld	qword ptr L33
	.code
	fstp	qword ptr [bp-8]
	.data
L34:
	dd	1167159827
	dd	1076084938
	fld	qword ptr L34
	.code
	fstp	qword ptr [bp-16]
	fld	qword ptr [bp-16]
	push	ax
	fld	qword ptr [bp-8]
	push	ax
	call	double_add
	add	sp,4
	fstp	qword ptr [bp-24]
	mov	ax,[bp-26]
	jmp	L24
L24:
	add	sp,26
	pop	bp
	ret
	public	float_eq
float_eq:	push	bp
	mov	bp,sp
	fld	dword ptr [bp+4]
	fld	dword ptr [bp+6]
	fcompp
	fnstsw	ax
	sahf
	xor	dx,dx
	pop	cx
	cmp	cx,ax
	jne	L36
	inc	dx
L36:
	mov	ax,dx
	jmp	L35
L35:
	pop	bp
	ret
	public	float_ne
float_ne:	push	bp
	mov	bp,sp
	fld	dword ptr [bp+4]
	fld	dword ptr [bp+6]
	fcompp
	fnstsw	ax
	sahf
	xor	dx,dx
	pop	cx
	cmp	cx,ax
	je	L38
	inc	dx
L38:
	mov	ax,dx
	jmp	L37
L37:
	pop	bp
	ret
	public	float_lt
float_lt:	push	bp
	mov	bp,sp
	fld	dword ptr [bp+4]
	fld	dword ptr [bp+6]
	fcompp
	fnstsw	ax
	sahf
	xor	dx,dx
	pop	cx
	cmp	cx,ax
	jge	L40
	inc	dx
L40:
	mov	ax,dx
	jmp	L39
L39:
	pop	bp
	ret
	public	float_gt
float_gt:	push	bp
	mov	bp,sp
	fld	dword ptr [bp+4]
	fld	dword ptr [bp+6]
	fcompp
	fnstsw	ax
	sahf
	xor	dx,dx
	pop	cx
	cmp	cx,ax
	jle	L42
	inc	dx
L42:
	mov	ax,dx
	jmp	L41
L41:
	pop	bp
	ret
	public	float_le
float_le:	push	bp
	mov	bp,sp
	fld	dword ptr [bp+4]
	fld	dword ptr [bp+6]
	fcompp
	fnstsw	ax
	sahf
	xor	dx,dx
	pop	cx
	cmp	cx,ax
	jg	L44
	inc	dx
L44:
	mov	ax,dx
	jmp	L43
L43:
	pop	bp
	ret
	public	float_ge
float_ge:	push	bp
	mov	bp,sp
	fld	dword ptr [bp+4]
	fld	dword ptr [bp+6]
	fcompp
	fnstsw	ax
	sahf
	xor	dx,dx
	pop	cx
	cmp	cx,ax
	jl	L46
	inc	dx
L46:
	mov	ax,dx
	jmp	L45
L45:
	pop	bp
	ret
	public	test_float_comparisons
test_float_comparisons:	push	bp
	mov	bp,sp
	add	sp,-12
	mov	ax,1
	mov	[bp-12],ax
	.data
L48:
	dd	0
	dd	1072693248
	fld	qword ptr L48
	.code
	fstp	dword ptr [bp-4]
	.data
L49:
	dd	0
	dd	1073741824
	fld	qword ptr L49
	.code
	fstp	dword ptr [bp-8]
	fld	dword ptr [bp-4]
	push	ax
	fld	dword ptr [bp-4]
	push	ax
	call	float_eq
	add	sp,4
	mov	[bp-10],ax
	fld	dword ptr [bp-8]
	push	ax
	fld	dword ptr [bp-4]
	push	ax
	call	float_eq
	add	sp,4
	mov	[bp-10],ax
	fld	dword ptr [bp-8]
	push	ax
	fld	dword ptr [bp-4]
	push	ax
	call	float_ne
	add	sp,4
	mov	[bp-10],ax
	fld	dword ptr [bp-4]
	push	ax
	fld	dword ptr [bp-4]
	push	ax
	call	float_ne
	add	sp,4
	mov	[bp-10],ax
	fld	dword ptr [bp-8]
	push	ax
	fld	dword ptr [bp-4]
	push	ax
	call	float_lt
	add	sp,4
	mov	[bp-10],ax
	fld	dword ptr [bp-4]
	push	ax
	fld	dword ptr [bp-8]
	push	ax
	call	float_lt
	add	sp,4
	mov	[bp-10],ax
	fld	dword ptr [bp-4]
	push	ax
	fld	dword ptr [bp-8]
	push	ax
	call	float_gt
	add	sp,4
	mov	[bp-10],ax
	fld	dword ptr [bp-8]
	push	ax
	fld	dword ptr [bp-4]
	push	ax
	call	float_gt
	add	sp,4
	mov	[bp-10],ax
	fld	dword ptr [bp-8]
	push	ax
	fld	dword ptr [bp-4]
	push	ax
	call	float_le
	add	sp,4
	mov	[bp-10],ax
	fld	dword ptr [bp-4]
	push	ax
	fld	dword ptr [bp-4]
	push	ax
	call	float_le
	add	sp,4
	mov	[bp-10],ax
	fld	dword ptr [bp-4]
	push	ax
	fld	dword ptr [bp-8]
	push	ax
	call	float_le
	add	sp,4
	mov	[bp-10],ax
	fld	dword ptr [bp-4]
	push	ax
	fld	dword ptr [bp-8]
	push	ax
	call	float_ge
	add	sp,4
	mov	[bp-10],ax
	fld	dword ptr [bp-4]
	push	ax
	fld	dword ptr [bp-4]
	push	ax
	call	float_ge
	add	sp,4
	mov	[bp-10],ax
	fld	dword ptr [bp-8]
	push	ax
	fld	dword ptr [bp-4]
	push	ax
	call	float_ge
	add	sp,4
	mov	[bp-10],ax
	mov	ax,[bp-12]
	jmp	L47
L47:
	add	sp,12
	pop	bp
	ret
	public	double_eq
double_eq:	push	bp
	mov	bp,sp
	fld	qword ptr [bp+4]
	fld	qword ptr [bp+6]
	fcompp
	fnstsw	ax
	sahf
	xor	dx,dx
	pop	cx
	cmp	cx,ax
	jne	L51
	inc	dx
L51:
	mov	ax,dx
	jmp	L50
L50:
	pop	bp
	ret
	public	double_ne
double_ne:	push	bp
	mov	bp,sp
	fld	qword ptr [bp+4]
	fld	qword ptr [bp+6]
	fcompp
	fnstsw	ax
	sahf
	xor	dx,dx
	pop	cx
	cmp	cx,ax
	je	L53
	inc	dx
L53:
	mov	ax,dx
	jmp	L52
L52:
	pop	bp
	ret
	public	double_lt
double_lt:	push	bp
	mov	bp,sp
	fld	qword ptr [bp+4]
	fld	qword ptr [bp+6]
	fcompp
	fnstsw	ax
	sahf
	xor	dx,dx
	pop	cx
	cmp	cx,ax
	jge	L55
	inc	dx
L55:
	mov	ax,dx
	jmp	L54
L54:
	pop	bp
	ret
	public	double_gt
double_gt:	push	bp
	mov	bp,sp
	fld	qword ptr [bp+4]
	fld	qword ptr [bp+6]
	fcompp
	fnstsw	ax
	sahf
	xor	dx,dx
	pop	cx
	cmp	cx,ax
	jle	L57
	inc	dx
L57:
	mov	ax,dx
	jmp	L56
L56:
	pop	bp
	ret
	public	double_le
double_le:	push	bp
	mov	bp,sp
	fld	qword ptr [bp+4]
	fld	qword ptr [bp+6]
	fcompp
	fnstsw	ax
	sahf
	xor	dx,dx
	pop	cx
	cmp	cx,ax
	jg	L59
	inc	dx
L59:
	mov	ax,dx
	jmp	L58
L58:
	pop	bp
	ret
	public	double_ge
double_ge:	push	bp
	mov	bp,sp
	fld	qword ptr [bp+4]
	fld	qword ptr [bp+6]
	fcompp
	fnstsw	ax
	sahf
	xor	dx,dx
	pop	cx
	cmp	cx,ax
	jl	L61
	inc	dx
L61:
	mov	ax,dx
	jmp	L60
L60:
	pop	bp
	ret
	public	test_double_comparisons
test_double_comparisons:	push	bp
	mov	bp,sp
	add	sp,-20
	mov	ax,1
	mov	[bp-20],ax
	.data
L63:
	dd	0
	dd	1072693248
	fld	qword ptr L63
	.code
	fstp	qword ptr [bp-8]
	.data
L64:
	dd	0
	dd	1073741824
	fld	qword ptr L64
	.code
	fstp	qword ptr [bp-16]
	fld	qword ptr [bp-8]
	push	ax
	fld	qword ptr [bp-8]
	push	ax
	call	double_eq
	add	sp,4
	mov	[bp-18],ax
	fld	qword ptr [bp-16]
	push	ax
	fld	qword ptr [bp-8]
	push	ax
	call	double_ne
	add	sp,4
	mov	[bp-18],ax
	fld	qword ptr [bp-16]
	push	ax
	fld	qword ptr [bp-8]
	push	ax
	call	double_lt
	add	sp,4
	mov	[bp-18],ax
	fld	qword ptr [bp-8]
	push	ax
	fld	qword ptr [bp-16]
	push	ax
	call	double_gt
	add	sp,4
	mov	[bp-18],ax
	fld	qword ptr [bp-16]
	push	ax
	fld	qword ptr [bp-8]
	push	ax
	call	double_le
	add	sp,4
	mov	[bp-18],ax
	fld	qword ptr [bp-8]
	push	ax
	fld	qword ptr [bp-16]
	push	ax
	call	double_ge
	add	sp,4
	mov	[bp-18],ax
	mov	ax,[bp-20]
	jmp	L62
L62:
	add	sp,20
	pop	bp
	ret
	public	int_to_float
int_to_float:	push	bp
	mov	bp,sp
	fld	dword ptr [bp+4]
	jmp	L65
L65:
	pop	bp
	ret
	public	float_to_int
float_to_int:	push	bp
	mov	bp,sp
	mov	ax,[bp+4]
	jmp	L66
L66:
	pop	bp
	ret
	public	int_to_double
int_to_double:	push	bp
	mov	bp,sp
	fld	qword ptr [bp+4]
	jmp	L67
L67:
	pop	bp
	ret
	public	double_to_int
double_to_int:	push	bp
	mov	bp,sp
	mov	ax,[bp+4]
	jmp	L68
L68:
	pop	bp
	ret
	public	float_to_double
float_to_double:	push	bp
	mov	bp,sp
	fld	qword ptr [bp+4]
	jmp	L69
L69:
	pop	bp
	ret
	public	double_to_float
double_to_float:	push	bp
	mov	bp,sp
	fld	dword ptr [bp+4]
	jmp	L70
L70:
	pop	bp
	ret
	public	test_float_conversions
test_float_conversions:	push	bp
	mov	bp,sp
	add	sp,-18
	mov	ax,1
	mov	[bp-18],ax
	mov	ax,42
	mov	[bp-2],ax
	mov	ax,[bp-2]
	push	ax
	call	int_to_float
	add	sp,2
	fstp	dword ptr [bp-8]
	.data
L72:
	dd	2576980378
	dd	1074633113
	fld	qword ptr L72
	.code
	fstp	dword ptr [bp-8]
	fld	dword ptr [bp-8]
	push	ax
	call	float_to_int
	add	sp,2
	mov	[bp-4],ax
	mov	ax,12345
	mov	[bp-2],ax
	mov	ax,[bp-2]
	push	ax
	call	int_to_double
	add	sp,2
	fstp	qword ptr [bp-16]
	.data
L73:
	dd	3435973837
	dd	1076088012
	fld	qword ptr L73
	.code
	fstp	qword ptr [bp-16]
	fld	qword ptr [bp-16]
	push	ax
	call	double_to_int
	add	sp,2
	mov	[bp-4],ax
	.data
L74:
	dd	0
	dd	1073217536
	fld	qword ptr L74
	.code
	fstp	dword ptr [bp-8]
	fld	dword ptr [bp-8]
	push	ax
	call	float_to_double
	add	sp,2
	fstp	qword ptr [bp-16]
	.data
L75:
	dd	0
	dd	1074003968
	fld	qword ptr L75
	.code
	fstp	qword ptr [bp-16]
	fld	qword ptr [bp-16]
	push	ax
	call	double_to_float
	add	sp,2
	fstp	dword ptr [bp-8]
	mov	ax,-10
	mov	[bp-2],ax
	mov	ax,[bp-2]
	push	ax
	call	int_to_float
	add	sp,2
	fstp	dword ptr [bp-8]
	fld	dword ptr [bp-8]
	push	ax
	call	float_to_int
	add	sp,2
	mov	[bp-4],ax
	mov	ax,[bp-18]
	jmp	L71
L71:
	add	sp,18
	pop	bp
	ret
	public	sum_10_floats
sum_10_floats:	push	bp
	mov	bp,sp
	fld	dword ptr [bp+4]
	fld	dword ptr [bp+6]
	faddp	st(1),st
	fld	dword ptr [bp+8]
	faddp	st(1),st
	fld	dword ptr [bp+10]
	faddp	st(1),st
	fld	dword ptr [bp+12]
	faddp	st(1),st
	fld	dword ptr [bp+14]
	faddp	st(1),st
	fld	dword ptr [bp+16]
	faddp	st(1),st
	fld	dword ptr [bp+18]
	faddp	st(1),st
	fld	dword ptr [bp+20]
	faddp	st(1),st
	fld	dword ptr [bp+22]
	faddp	st(1),st
	jmp	L76
L76:
	pop	bp
	ret
	public	sum_20_floats
sum_20_floats:	push	bp
	mov	bp,sp
	fld	dword ptr [bp+4]
	fld	dword ptr [bp+6]
	faddp	st(1),st
	fld	dword ptr [bp+8]
	faddp	st(1),st
	fld	dword ptr [bp+10]
	faddp	st(1),st
	fld	dword ptr [bp+12]
	faddp	st(1),st
	fld	dword ptr [bp+14]
	faddp	st(1),st
	fld	dword ptr [bp+16]
	faddp	st(1),st
	fld	dword ptr [bp+18]
	faddp	st(1),st
	fld	dword ptr [bp+20]
	faddp	st(1),st
	fld	dword ptr [bp+22]
	faddp	st(1),st
	fld	dword ptr [bp+24]
	faddp	st(1),st
	fld	dword ptr [bp+26]
	faddp	st(1),st
	fld	dword ptr [bp+28]
	faddp	st(1),st
	fld	dword ptr [bp+30]
	faddp	st(1),st
	fld	dword ptr [bp+32]
	faddp	st(1),st
	fld	dword ptr [bp+34]
	faddp	st(1),st
	fld	dword ptr [bp+36]
	faddp	st(1),st
	fld	dword ptr [bp+38]
	faddp	st(1),st
	fld	dword ptr [bp+40]
	faddp	st(1),st
	fld	dword ptr [bp+42]
	faddp	st(1),st
	jmp	L77
L77:
	pop	bp
	ret
	public	sum_40_floats
sum_40_floats:	push	bp
	mov	bp,sp
	add	sp,-4
	fld	dword ptr [bp+4]
	fld	dword ptr [bp+6]
	faddp	st(1),st
	fld	dword ptr [bp+8]
	faddp	st(1),st
	fld	dword ptr [bp+10]
	faddp	st(1),st
	fld	dword ptr [bp+12]
	faddp	st(1),st
	fld	dword ptr [bp+14]
	faddp	st(1),st
	fld	dword ptr [bp+16]
	faddp	st(1),st
	fld	dword ptr [bp+18]
	faddp	st(1),st
	fld	dword ptr [bp+20]
	faddp	st(1),st
	fld	dword ptr [bp+22]
	faddp	st(1),st
	fstp	dword ptr [bp-4]
	fld	dword ptr [bp-4]
	fld	dword ptr [bp+24]
	faddp	st(1),st
	fld	dword ptr [bp+26]
	faddp	st(1),st
	fld	dword ptr [bp+28]
	faddp	st(1),st
	fld	dword ptr [bp+30]
	faddp	st(1),st
	fld	dword ptr [bp+32]
	faddp	st(1),st
	fld	dword ptr [bp+34]
	faddp	st(1),st
	fld	dword ptr [bp+36]
	faddp	st(1),st
	fld	dword ptr [bp+38]
	faddp	st(1),st
	fld	dword ptr [bp+40]
	faddp	st(1),st
	fld	dword ptr [bp+42]
	faddp	st(1),st
	fstp	dword ptr [bp-4]
	fld	dword ptr [bp-4]
	fld	dword ptr [bp+44]
	faddp	st(1),st
	fld	dword ptr [bp+46]
	faddp	st(1),st
	fld	dword ptr [bp+48]
	faddp	st(1),st
	fld	dword ptr [bp+50]
	faddp	st(1),st
	fld	dword ptr [bp+52]
	faddp	st(1),st
	fld	dword ptr [bp+54]
	faddp	st(1),st
	fld	dword ptr [bp+56]
	faddp	st(1),st
	fld	dword ptr [bp+58]
	faddp	st(1),st
	fld	dword ptr [bp+60]
	faddp	st(1),st
	fld	dword ptr [bp+62]
	faddp	st(1),st
	fstp	dword ptr [bp-4]
	fld	dword ptr [bp-4]
	fld	dword ptr [bp+64]
	faddp	st(1),st
	fld	dword ptr [bp+66]
	faddp	st(1),st
	fld	dword ptr [bp+68]
	faddp	st(1),st
	fld	dword ptr [bp+70]
	faddp	st(1),st
	fld	dword ptr [bp+72]
	faddp	st(1),st
	fld	dword ptr [bp+74]
	faddp	st(1),st
	fld	dword ptr [bp+76]
	faddp	st(1),st
	fld	dword ptr [bp+78]
	faddp	st(1),st
	fld	dword ptr [bp+80]
	faddp	st(1),st
	fld	dword ptr [bp+82]
	faddp	st(1),st
	fstp	dword ptr [bp-4]
	fld	dword ptr [bp-4]
	jmp	L78
L78:
	add	sp,4
	pop	bp
	ret
	public	test_many_float_params
test_many_float_params:	push	bp
	mov	bp,sp
	add	sp,-6
	mov	ax,1
	mov	[bp-6],ax
	.data
L80:
	dd	0
	dd	1072693248
L81:
	dd	0
	dd	1073741824
L82:
	dd	0
	dd	1074266112
L83:
	dd	0
	dd	1074790400
L84:
	dd	0
	dd	1075052544
L85:
	dd	0
	dd	1075314688
L86:
	dd	0
	dd	1075576832
L87:
	dd	0
	dd	1075838976
L88:
	dd	0
	dd	1075970048
L89:
	dd	0
	dd	1076101120
	fld	qword ptr L89
	push	ax
	fld	qword ptr L88
	push	ax
	fld	qword ptr L87
	push	ax
	fld	qword ptr L86
	push	ax
	fld	qword ptr L85
	push	ax
	fld	qword ptr L84
	push	ax
	fld	qword ptr L83
	push	ax
	fld	qword ptr L82
	push	ax
	fld	qword ptr L81
	push	ax
	fld	qword ptr L80
	push	ax
	.code
	call	sum_10_floats
	add	sp,20
	fstp	dword ptr [bp-4]
	.data
L90:
	dd	0
	dd	1072693248
L91:
	dd	0
	dd	1073741824
L92:
	dd	0
	dd	1074266112
L93:
	dd	0
	dd	1074790400
L94:
	dd	0
	dd	1075052544
L95:
	dd	0
	dd	1075314688
L96:
	dd	0
	dd	1075576832
L97:
	dd	0
	dd	1075838976
L98:
	dd	0
	dd	1075970048
L99:
	dd	0
	dd	1076101120
L100:
	dd	0
	dd	1076232192
L101:
	dd	0
	dd	1076363264
L102:
	dd	0
	dd	1076494336
L103:
	dd	0
	dd	1076625408
L104:
	dd	0
	dd	1076756480
L105:
	dd	0
	dd	1076887552
L106:
	dd	0
	dd	1076953088
L107:
	dd	0
	dd	1077018624
L108:
	dd	0
	dd	1077084160
L109:
	dd	0
	dd	1077149696
	fld	qword ptr L109
	push	ax
	fld	qword ptr L108
	push	ax
	fld	qword ptr L107
	push	ax
	fld	qword ptr L106
	push	ax
	fld	qword ptr L105
	push	ax
	fld	qword ptr L104
	push	ax
	fld	qword ptr L103
	push	ax
	fld	qword ptr L102
	push	ax
	fld	qword ptr L101
	push	ax
	fld	qword ptr L100
	push	ax
	fld	qword ptr L99
	push	ax
	fld	qword ptr L98
	push	ax
	fld	qword ptr L97
	push	ax
	fld	qword ptr L96
	push	ax
	fld	qword ptr L95
	push	ax
	fld	qword ptr L94
	push	ax
	fld	qword ptr L93
	push	ax
	fld	qword ptr L92
	push	ax
	fld	qword ptr L91
	push	ax
	fld	qword ptr L90
	push	ax
	.code
	call	sum_20_floats
	add	sp,40
	fstp	dword ptr [bp-4]
	.data
L110:
	dd	0
	dd	1072693248
L111:
	dd	0
	dd	1073741824
L112:
	dd	0
	dd	1074266112
L113:
	dd	0
	dd	1074790400
L114:
	dd	0
	dd	1075052544
L115:
	dd	0
	dd	1075314688
L116:
	dd	0
	dd	1075576832
L117:
	dd	0
	dd	1075838976
L118:
	dd	0
	dd	1075970048
L119:
	dd	0
	dd	1076101120
L120:
	dd	0
	dd	1076232192
L121:
	dd	0
	dd	1076363264
L122:
	dd	0
	dd	1076494336
L123:
	dd	0
	dd	1076625408
L124:
	dd	0
	dd	1076756480
L125:
	dd	0
	dd	1076887552
L126:
	dd	0
	dd	1076953088
L127:
	dd	0
	dd	1077018624
L128:
	dd	0
	dd	1077084160
L129:
	dd	0
	dd	1077149696
L130:
	dd	0
	dd	1077215232
L131:
	dd	0
	dd	1077280768
L132:
	dd	0
	dd	1077346304
L133:
	dd	0
	dd	1077411840
L134:
	dd	0
	dd	1077477376
L135:
	dd	0
	dd	1077542912
L136:
	dd	0
	dd	1077608448
L137:
	dd	0
	dd	1077673984
L138:
	dd	0
	dd	1077739520
L139:
	dd	0
	dd	1077805056
L140:
	dd	0
	dd	1077870592
L141:
	dd	0
	dd	1077936128
L142:
	dd	0
	dd	1077968896
L143:
	dd	0
	dd	1078001664
L144:
	dd	0
	dd	1078034432
L145:
	dd	0
	dd	1078067200
L146:
	dd	0
	dd	1078099968
L147:
	dd	0
	dd	1078132736
L148:
	dd	0
	dd	1078165504
L149:
	dd	0
	dd	1078198272
	fld	qword ptr L149
	push	ax
	fld	qword ptr L148
	push	ax
	fld	qword ptr L147
	push	ax
	fld	qword ptr L146
	push	ax
	fld	qword ptr L145
	push	ax
	fld	qword ptr L144
	push	ax
	fld	qword ptr L143
	push	ax
	fld	qword ptr L142
	push	ax
	fld	qword ptr L141
	push	ax
	fld	qword ptr L140
	push	ax
	fld	qword ptr L139
	push	ax
	fld	qword ptr L138
	push	ax
	fld	qword ptr L137
	push	ax
	fld	qword ptr L136
	push	ax
	fld	qword ptr L135
	push	ax
	fld	qword ptr L134
	push	ax
	fld	qword ptr L133
	push	ax
	fld	qword ptr L132
	push	ax
	fld	qword ptr L131
	push	ax
	fld	qword ptr L130
	push	ax
	fld	qword ptr L129
	push	ax
	fld	qword ptr L128
	push	ax
	fld	qword ptr L127
	push	ax
	fld	qword ptr L126
	push	ax
	fld	qword ptr L125
	push	ax
	fld	qword ptr L124
	push	ax
	fld	qword ptr L123
	push	ax
	fld	qword ptr L122
	push	ax
	fld	qword ptr L121
	push	ax
	fld	qword ptr L120
	push	ax
	fld	qword ptr L119
	push	ax
	fld	qword ptr L118
	push	ax
	fld	qword ptr L117
	push	ax
	fld	qword ptr L116
	push	ax
	fld	qword ptr L115
	push	ax
	fld	qword ptr L114
	push	ax
	fld	qword ptr L113
	push	ax
	fld	qword ptr L112
	push	ax
	fld	qword ptr L111
	push	ax
	fld	qword ptr L110
	push	ax
	.code
	call	sum_40_floats
	add	sp,80
	fstp	dword ptr [bp-4]
	mov	ax,[bp-6]
	jmp	L79
L79:
	add	sp,6
	pop	bp
	ret
	public	sum_10_doubles
sum_10_doubles:	push	bp
	mov	bp,sp
	fld	qword ptr [bp+4]
	fld	qword ptr [bp+6]
	faddp	st(1),st
	fld	qword ptr [bp+8]
	faddp	st(1),st
	fld	qword ptr [bp+10]
	faddp	st(1),st
	fld	qword ptr [bp+12]
	faddp	st(1),st
	fld	qword ptr [bp+14]
	faddp	st(1),st
	fld	qword ptr [bp+16]
	faddp	st(1),st
	fld	qword ptr [bp+18]
	faddp	st(1),st
	fld	qword ptr [bp+20]
	faddp	st(1),st
	fld	qword ptr [bp+22]
	faddp	st(1),st
	jmp	L150
L150:
	pop	bp
	ret
	public	sum_20_doubles
sum_20_doubles:	push	bp
	mov	bp,sp
	fld	qword ptr [bp+4]
	fld	qword ptr [bp+6]
	faddp	st(1),st
	fld	qword ptr [bp+8]
	faddp	st(1),st
	fld	qword ptr [bp+10]
	faddp	st(1),st
	fld	qword ptr [bp+12]
	faddp	st(1),st
	fld	qword ptr [bp+14]
	faddp	st(1),st
	fld	qword ptr [bp+16]
	faddp	st(1),st
	fld	qword ptr [bp+18]
	faddp	st(1),st
	fld	qword ptr [bp+20]
	faddp	st(1),st
	fld	qword ptr [bp+22]
	faddp	st(1),st
	fld	qword ptr [bp+24]
	faddp	st(1),st
	fld	qword ptr [bp+26]
	faddp	st(1),st
	fld	qword ptr [bp+28]
	faddp	st(1),st
	fld	qword ptr [bp+30]
	faddp	st(1),st
	fld	qword ptr [bp+32]
	faddp	st(1),st
	fld	qword ptr [bp+34]
	faddp	st(1),st
	fld	qword ptr [bp+36]
	faddp	st(1),st
	fld	qword ptr [bp+38]
	faddp	st(1),st
	fld	qword ptr [bp+40]
	faddp	st(1),st
	fld	qword ptr [bp+42]
	faddp	st(1),st
	jmp	L151
L151:
	pop	bp
	ret
	public	sum_40_doubles
sum_40_doubles:	push	bp
	mov	bp,sp
	add	sp,-8
	fld	qword ptr [bp+4]
	fld	qword ptr [bp+6]
	faddp	st(1),st
	fld	qword ptr [bp+8]
	faddp	st(1),st
	fld	qword ptr [bp+10]
	faddp	st(1),st
	fld	qword ptr [bp+12]
	faddp	st(1),st
	fld	qword ptr [bp+14]
	faddp	st(1),st
	fld	qword ptr [bp+16]
	faddp	st(1),st
	fld	qword ptr [bp+18]
	faddp	st(1),st
	fld	qword ptr [bp+20]
	faddp	st(1),st
	fld	qword ptr [bp+22]
	faddp	st(1),st
	fstp	qword ptr [bp-8]
	fld	qword ptr [bp-8]
	fld	qword ptr [bp+24]
	faddp	st(1),st
	fld	qword ptr [bp+26]
	faddp	st(1),st
	fld	qword ptr [bp+28]
	faddp	st(1),st
	fld	qword ptr [bp+30]
	faddp	st(1),st
	fld	qword ptr [bp+32]
	faddp	st(1),st
	fld	qword ptr [bp+34]
	faddp	st(1),st
	fld	qword ptr [bp+36]
	faddp	st(1),st
	fld	qword ptr [bp+38]
	faddp	st(1),st
	fld	qword ptr [bp+40]
	faddp	st(1),st
	fld	qword ptr [bp+42]
	faddp	st(1),st
	fstp	qword ptr [bp-8]
	fld	qword ptr [bp-8]
	fld	qword ptr [bp+44]
	faddp	st(1),st
	fld	qword ptr [bp+46]
	faddp	st(1),st
	fld	qword ptr [bp+48]
	faddp	st(1),st
	fld	qword ptr [bp+50]
	faddp	st(1),st
	fld	qword ptr [bp+52]
	faddp	st(1),st
	fld	qword ptr [bp+54]
	faddp	st(1),st
	fld	qword ptr [bp+56]
	faddp	st(1),st
	fld	qword ptr [bp+58]
	faddp	st(1),st
	fld	qword ptr [bp+60]
	faddp	st(1),st
	fld	qword ptr [bp+62]
	faddp	st(1),st
	fstp	qword ptr [bp-8]
	fld	qword ptr [bp-8]
	fld	qword ptr [bp+64]
	faddp	st(1),st
	fld	qword ptr [bp+66]
	faddp	st(1),st
	fld	qword ptr [bp+68]
	faddp	st(1),st
	fld	qword ptr [bp+70]
	faddp	st(1),st
	fld	qword ptr [bp+72]
	faddp	st(1),st
	fld	qword ptr [bp+74]
	faddp	st(1),st
	fld	qword ptr [bp+76]
	faddp	st(1),st
	fld	qword ptr [bp+78]
	faddp	st(1),st
	fld	qword ptr [bp+80]
	faddp	st(1),st
	fld	qword ptr [bp+82]
	faddp	st(1),st
	fstp	qword ptr [bp-8]
	fld	qword ptr [bp-8]
	jmp	L152
L152:
	add	sp,8
	pop	bp
	ret
	public	test_many_double_params
test_many_double_params:	push	bp
	mov	bp,sp
	add	sp,-10
	mov	ax,1
	mov	[bp-10],ax
	.data
L154:
	dd	0
	dd	1072693248
L155:
	dd	0
	dd	1073741824
L156:
	dd	0
	dd	1074266112
L157:
	dd	0
	dd	1074790400
L158:
	dd	0
	dd	1075052544
L159:
	dd	0
	dd	1075314688
L160:
	dd	0
	dd	1075576832
L161:
	dd	0
	dd	1075838976
L162:
	dd	0
	dd	1075970048
L163:
	dd	0
	dd	1076101120
	fld	qword ptr L163
	push	ax
	fld	qword ptr L162
	push	ax
	fld	qword ptr L161
	push	ax
	fld	qword ptr L160
	push	ax
	fld	qword ptr L159
	push	ax
	fld	qword ptr L158
	push	ax
	fld	qword ptr L157
	push	ax
	fld	qword ptr L156
	push	ax
	fld	qword ptr L155
	push	ax
	fld	qword ptr L154
	push	ax
	.code
	call	sum_10_doubles
	add	sp,20
	fstp	qword ptr [bp-8]
	.data
L164:
	dd	0
	dd	1072693248
L165:
	dd	0
	dd	1073741824
L166:
	dd	0
	dd	1074266112
L167:
	dd	0
	dd	1074790400
L168:
	dd	0
	dd	1075052544
L169:
	dd	0
	dd	1075314688
L170:
	dd	0
	dd	1075576832
L171:
	dd	0
	dd	1075838976
L172:
	dd	0
	dd	1075970048
L173:
	dd	0
	dd	1076101120
L174:
	dd	0
	dd	1076232192
L175:
	dd	0
	dd	1076363264
L176:
	dd	0
	dd	1076494336
L177:
	dd	0
	dd	1076625408
L178:
	dd	0
	dd	1076756480
L179:
	dd	0
	dd	1076887552
L180:
	dd	0
	dd	1076953088
L181:
	dd	0
	dd	1077018624
L182:
	dd	0
	dd	1077084160
L183:
	dd	0
	dd	1077149696
	fld	qword ptr L183
	push	ax
	fld	qword ptr L182
	push	ax
	fld	qword ptr L181
	push	ax
	fld	qword ptr L180
	push	ax
	fld	qword ptr L179
	push	ax
	fld	qword ptr L178
	push	ax
	fld	qword ptr L177
	push	ax
	fld	qword ptr L176
	push	ax
	fld	qword ptr L175
	push	ax
	fld	qword ptr L174
	push	ax
	fld	qword ptr L173
	push	ax
	fld	qword ptr L172
	push	ax
	fld	qword ptr L171
	push	ax
	fld	qword ptr L170
	push	ax
	fld	qword ptr L169
	push	ax
	fld	qword ptr L168
	push	ax
	fld	qword ptr L167
	push	ax
	fld	qword ptr L166
	push	ax
	fld	qword ptr L165
	push	ax
	fld	qword ptr L164
	push	ax
	.code
	call	sum_20_doubles
	add	sp,40
	fstp	qword ptr [bp-8]
	.data
L184:
	dd	0
	dd	1072693248
L185:
	dd	0
	dd	1073741824
L186:
	dd	0
	dd	1074266112
L187:
	dd	0
	dd	1074790400
L188:
	dd	0
	dd	1075052544
L189:
	dd	0
	dd	1075314688
L190:
	dd	0
	dd	1075576832
L191:
	dd	0
	dd	1075838976
L192:
	dd	0
	dd	1075970048
L193:
	dd	0
	dd	1076101120
L194:
	dd	0
	dd	1076232192
L195:
	dd	0
	dd	1076363264
L196:
	dd	0
	dd	1076494336
L197:
	dd	0
	dd	1076625408
L198:
	dd	0
	dd	1076756480
L199:
	dd	0
	dd	1076887552
L200:
	dd	0
	dd	1076953088
L201:
	dd	0
	dd	1077018624
L202:
	dd	0
	dd	1077084160
L203:
	dd	0
	dd	1077149696
L204:
	dd	0
	dd	1077215232
L205:
	dd	0
	dd	1077280768
L206:
	dd	0
	dd	1077346304
L207:
	dd	0
	dd	1077411840
L208:
	dd	0
	dd	1077477376
L209:
	dd	0
	dd	1077542912
L210:
	dd	0
	dd	1077608448
L211:
	dd	0
	dd	1077673984
L212:
	dd	0
	dd	1077739520
L213:
	dd	0
	dd	1077805056
L214:
	dd	0
	dd	1077870592
L215:
	dd	0
	dd	1077936128
L216:
	dd	0
	dd	1077968896
L217:
	dd	0
	dd	1078001664
L218:
	dd	0
	dd	1078034432
L219:
	dd	0
	dd	1078067200
L220:
	dd	0
	dd	1078099968
L221:
	dd	0
	dd	1078132736
L222:
	dd	0
	dd	1078165504
L223:
	dd	0
	dd	1078198272
	fld	qword ptr L223
	push	ax
	fld	qword ptr L222
	push	ax
	fld	qword ptr L221
	push	ax
	fld	qword ptr L220
	push	ax
	fld	qword ptr L219
	push	ax
	fld	qword ptr L218
	push	ax
	fld	qword ptr L217
	push	ax
	fld	qword ptr L216
	push	ax
	fld	qword ptr L215
	push	ax
	fld	qword ptr L214
	push	ax
	fld	qword ptr L213
	push	ax
	fld	qword ptr L212
	push	ax
	fld	qword ptr L211
	push	ax
	fld	qword ptr L210
	push	ax
	fld	qword ptr L209
	push	ax
	fld	qword ptr L208
	push	ax
	fld	qword ptr L207
	push	ax
	fld	qword ptr L206
	push	ax
	fld	qword ptr L205
	push	ax
	fld	qword ptr L204
	push	ax
	fld	qword ptr L203
	push	ax
	fld	qword ptr L202
	push	ax
	fld	qword ptr L201
	push	ax
	fld	qword ptr L200
	push	ax
	fld	qword ptr L199
	push	ax
	fld	qword ptr L198
	push	ax
	fld	qword ptr L197
	push	ax
	fld	qword ptr L196
	push	ax
	fld	qword ptr L195
	push	ax
	fld	qword ptr L194
	push	ax
	fld	qword ptr L193
	push	ax
	fld	qword ptr L192
	push	ax
	fld	qword ptr L191
	push	ax
	fld	qword ptr L190
	push	ax
	fld	qword ptr L189
	push	ax
	fld	qword ptr L188
	push	ax
	fld	qword ptr L187
	push	ax
	fld	qword ptr L186
	push	ax
	fld	qword ptr L185
	push	ax
	fld	qword ptr L184
	push	ax
	.code
	call	sum_40_doubles
	add	sp,80
	fstp	qword ptr [bp-8]
	mov	ax,[bp-10]
	jmp	L153
L153:
	add	sp,10
	pop	bp
	ret
	public	mixed_add
mixed_add:	push	bp
	mov	bp,sp
	fld	dword ptr [bp+4]
	fld	dword ptr [bp+6]
	faddp	st(1),st
	jmp	L224
L224:
	pop	bp
	ret
	public	mixed_mul
mixed_mul:	push	bp
	mov	bp,sp
	fld	qword ptr [bp+4]
	fld	qword ptr [bp+6]
	fmulp	st(1),st
	jmp	L225
L225:
	pop	bp
	ret
	public	mixed_compare
mixed_compare:	push	bp
	mov	bp,sp
	fld	dword ptr [bp+4]
	fld	dword ptr [bp+6]
	fcompp
	fnstsw	ax
	sahf
	xor	dx,dx
	pop	cx
	cmp	cx,ax
	jge	L227
	inc	dx
L227:
	mov	ax,dx
	jmp	L226
L226:
	pop	bp
	ret
	public	compute_average
compute_average:	push	bp
	mov	bp,sp
	add	sp,-4
	mov	ax,[bp+6]
	push	ax
	mov	ax,[bp+4]
	pop	cx
	add	ax,cx
	push	ax
	mov	ax,[bp+8]
	pop	cx
	add	ax,cx
	push	ax
	mov	ax,[bp+10]
	pop	cx
	add	ax,cx
	push	ax
	mov	ax,[bp+12]
	pop	cx
	add	ax,cx
	fstp	dword ptr [bp-4]
	.data
L229:
	dd	0
	dd	1075052544
	.code
	fld	dword ptr [bp-4]
	fld	qword ptr L229
	fdivrp	st(1),st
	jmp	L228
L228:
	add	sp,4
	pop	bp
	ret
	public	test_mixed_operations
test_mixed_operations:	push	bp
	mov	bp,sp
	add	sp,-16
	mov	ax,1
	mov	[bp-16],ax
	mov	ax,10
	mov	[bp-2],ax
	.data
L231:
	dd	0
	dd	1074003968
	fld	qword ptr L231
	.code
	fstp	dword ptr [bp-6]
	fld	dword ptr [bp-6]
	push	ax
	mov	ax,[bp-2]
	push	ax
	call	mixed_add
	add	sp,4
	fstp	dword ptr [bp-6]
	mov	ax,3
	mov	[bp-2],ax
	.data
L232:
	dd	0
	dd	1074921472
	fld	qword ptr L232
	.code
	fstp	qword ptr [bp-14]
	fld	qword ptr [bp-14]
	push	ax
	mov	ax,[bp-2]
	push	ax
	call	mixed_mul
	add	sp,4
	fstp	qword ptr [bp-14]
	mov	ax,5
	mov	[bp-2],ax
	.data
L233:
	dd	0
	dd	1075183616
	fld	qword ptr L233
	.code
	fstp	dword ptr [bp-6]
	fld	dword ptr [bp-6]
	push	ax
	mov	ax,[bp-2]
	push	ax
	call	mixed_compare
	add	sp,4
	mov	[bp-2],ax
	mov	ax,50
	push	ax
	mov	ax,40
	push	ax
	mov	ax,30
	push	ax
	mov	ax,20
	push	ax
	mov	ax,10
	push	ax
	call	compute_average
	add	sp,10
	fstp	dword ptr [bp-6]
	mov	ax,[bp-16]
	jmp	L230
L230:
	add	sp,16
	pop	bp
	ret
	public	negate_float
negate_float:	push	bp
	mov	bp,sp
	fld	dword ptr [bp+4]
	fchs
	jmp	L234
L234:
	pop	bp
	ret
	public	negate_double
negate_double:	push	bp
	mov	bp,sp
	fld	qword ptr [bp+4]
	fchs
	jmp	L235
L235:
	pop	bp
	ret
	public	test_float_negation
test_float_negation:	push	bp
	mov	bp,sp
	add	sp,-14
	mov	ax,1
	mov	[bp-14],ax
	.data
L237:
	dd	0
	dd	1075052544
	fld	qword ptr L237
	.code
	fstp	dword ptr [bp-4]
	fld	dword ptr [bp-4]
	push	ax
	call	negate_float
	add	sp,2
	fstp	dword ptr [bp-4]
	.data
L238:
	dd	0
	dd	1074266112
	fld	qword ptr L238
	fchs
	.code
	fstp	dword ptr [bp-4]
	fld	dword ptr [bp-4]
	push	ax
	call	negate_float
	add	sp,2
	fstp	dword ptr [bp-4]
	.data
L239:
	dd	0
	dd	1075707904
	fld	qword ptr L239
	.code
	fstp	qword ptr [bp-12]
	fld	qword ptr [bp-12]
	push	ax
	call	negate_double
	add	sp,2
	fstp	qword ptr [bp-12]
	.data
L240:
	dd	0
	dd	1074003968
	fld	qword ptr L240
	fchs
	.code
	fstp	qword ptr [bp-12]
	fld	qword ptr [bp-12]
	push	ax
	call	negate_double
	add	sp,2
	fstp	qword ptr [bp-12]
	mov	ax,[bp-14]
	jmp	L236
L236:
	add	sp,14
	pop	bp
	ret
	public	test_float_assignment
test_float_assignment:	push	bp
	mov	bp,sp
	add	sp,-82
	mov	ax,1
	mov	[bp-82],ax
	.data
L242:
	dd	0
	dd	1072693248
	fld	qword ptr L242
	.code
	fstp	dword ptr [bp-4]
	.data
L243:
	dd	0
	dd	1073741824
	fld	qword ptr L243
	.code
	fstp	dword ptr [bp-8]
	.data
L244:
	dd	0
	dd	1074266112
	fld	qword ptr L244
	.code
	fstp	dword ptr [bp-12]
	.data
L245:
	dd	0
	dd	1074790400
	fld	qword ptr L245
	.code
	fstp	dword ptr [bp-16]
	.data
L246:
	dd	0
	dd	1075052544
	fld	qword ptr L246
	.code
	fstp	dword ptr [bp-20]
	.data
L247:
	dd	0
	dd	1075314688
	fld	qword ptr L247
	.code
	fstp	dword ptr [bp-24]
	.data
L248:
	dd	0
	dd	1075576832
	fld	qword ptr L248
	.code
	fstp	dword ptr [bp-28]
	.data
L249:
	dd	0
	dd	1075838976
	fld	qword ptr L249
	.code
	fstp	dword ptr [bp-32]
	.data
L250:
	dd	0
	dd	1075970048
	fld	qword ptr L250
	.code
	fstp	dword ptr [bp-36]
	.data
L251:
	dd	0
	dd	1076101120
	fld	qword ptr L251
	.code
	fstp	dword ptr [bp-40]
	.data
L252:
	dd	0
	dd	1075183616
	fld	qword ptr L252
	.code
	fstp	dword ptr [bp-12]
	fstp	dword ptr [bp-8]
	fstp	dword ptr [bp-4]
	.data
L253:
	dd	0
	dd	1072693248
	fld	qword ptr L253
	.code
	fstp	qword ptr [bp-48]
	.data
L254:
	dd	0
	dd	1073741824
	fld	qword ptr L254
	.code
	fstp	qword ptr [bp-56]
	.data
L255:
	dd	0
	dd	1074266112
	fld	qword ptr L255
	.code
	fstp	qword ptr [bp-64]
	.data
L256:
	dd	0
	dd	1074790400
	fld	qword ptr L256
	.code
	fstp	qword ptr [bp-72]
	.data
L257:
	dd	0
	dd	1075052544
	fld	qword ptr L257
	.code
	fstp	qword ptr [bp-80]
	fld	dword ptr [bp-8]
	fld	dword ptr [bp-12]
	faddp	st(1),st
	fld	dword ptr [bp-16]
	fld	dword ptr [bp-20]
	fsubrp	st(1),st
	fmulp	st(1),st
	fld	dword ptr [bp-24]
	fdivrp	st(1),st
	fstp	dword ptr [bp-4]
	mov	ax,[bp-82]
	jmp	L241
L241:
	add	sp,82
	pop	bp
	ret
	.data
	public	global_float
global_float:	dd	0
	public	global_double
global_double:	dd	0
	dd	0
	.code
	public	get_global_float
get_global_float:	push	bp
	mov	bp,sp
	fld	dword ptr global_float
	jmp	L258
L258:
	pop	bp
	ret
	public	set_global_float
set_global_float:	push	bp
	mov	bp,sp
	fld	dword ptr [bp+4]
	fstp	dword ptr global_float
L259:
	pop	bp
	ret
	public	get_global_double
get_global_double:	push	bp
	mov	bp,sp
	fld	qword ptr global_double
	jmp	L260
L260:
	pop	bp
	ret
	public	set_global_double
set_global_double:	push	bp
	mov	bp,sp
	fld	qword ptr [bp+4]
	fstp	qword ptr global_double
L261:
	pop	bp
	ret
	public	test_global_floats
test_global_floats:	push	bp
	mov	bp,sp
	add	sp,-14
	mov	ax,1
	mov	[bp-14],ax
	.data
L263:
	dd	1374389535
	dd	1074339512
	fld	qword ptr L263
	push	ax
	.code
	call	set_global_float
	add	sp,2
	call	get_global_float
	fstp	dword ptr [bp-4]
	.data
L264:
	dd	2511009680
	dd	1074118409
	fld	qword ptr L264
	push	ax
	.code
	call	set_global_double
	add	sp,2
	call	get_global_double
	fstp	qword ptr [bp-12]
	.data
L265:
	dd	0
	dd	1073217536
	fld	qword ptr L265
	.code
	fstp	dword ptr global_float
	.data
L266:
	dd	0
	dd	1074003968
	fld	qword ptr L266
	.code
	fstp	qword ptr global_double
	.data
L267:
	dd	0
	dd	1072693248
	.code
	fld	dword ptr global_float
	fld	qword ptr L267
	faddp	st(1),st
	fstp	dword ptr [bp-4]
	.data
L268:
	dd	0
	dd	1073741824
	.code
	fld	qword ptr global_double
	fld	qword ptr L268
	fmulp	st(1),st
	fstp	qword ptr [bp-12]
	mov	ax,[bp-14]
	jmp	L262
L262:
	add	sp,14
	pop	bp
	ret
	.data
	public	float_array
float_array:	db	0 dup 40
	public	double_array
double_array:	db	0 dup 80
	.code
	public	test_float_arrays
test_float_arrays:	push	bp
	mov	bp,sp
	add	sp,-16
	mov	ax,1
	mov	[bp-16],ax
	mov	ax,0
	mov	[bp-2],ax
L271:
	mov	ax,[bp-2]
	push	ax
	mov	ax,10
	pop	cx
	cmp	cx,ax
	jl	L272
	jmp	L270
L272:
	mov	ax,offset float_array
	push	ax
	mov	ax,[bp-2]
	pop	cx
	add	ax,cx
	push	ax
	mov	ax,1
	push	ax
	mov	ax,[bp-2]
	pop	cx
	add	ax,cx
	pop	bx
	fstp	dword ptr [bp+0]
	mov	ax,1
	push	ax
	mov	ax,[bp-2]
	pop	cx
	add	ax,cx
	mov	[bp-2],ax
	jmp	L271
L270:
	.data
L273:
	dd	0
	dd	0
	fld	qword ptr L273
	.code
	fstp	dword ptr [bp-6]
	mov	ax,0
	mov	[bp-2],ax
L275:
	mov	ax,[bp-2]
	push	ax
	mov	ax,10
	pop	cx
	cmp	cx,ax
	jl	L276
	jmp	L274
L276:
	fld	dword ptr [bp-6]
	mov	ax,offset float_array
	push	ax
	mov	ax,[bp-2]
	pop	cx
	add	ax,cx
	fld	dword ptr [bp+0]
	faddp	st(1),st
	fstp	dword ptr [bp-6]
	mov	ax,1
	push	ax
	mov	ax,[bp-2]
	pop	cx
	add	ax,cx
	mov	[bp-2],ax
	jmp	L275
L274:
	mov	ax,0
	mov	[bp-2],ax
L278:
	mov	ax,[bp-2]
	push	ax
	mov	ax,10
	pop	cx
	cmp	cx,ax
	jl	L279
	jmp	L277
L279:
	.data
L280:
	dd	0
	dd	1071644672
	.code
	mov	ax,offset double_array
	push	ax
	mov	ax,[bp-2]
	pop	cx
	add	ax,cx
	push	ax
	mov	ax,1
	push	ax
	mov	ax,[bp-2]
	pop	cx
	add	ax,cx
	fld	qword ptr L280
	fmulp	st(1),st
	pop	bx
	fstp	qword ptr [bp+0]
	mov	ax,1
	push	ax
	mov	ax,[bp-2]
	pop	cx
	add	ax,cx
	mov	[bp-2],ax
	jmp	L278
L277:
	.data
L281:
	dd	0
	dd	0
	fld	qword ptr L281
	.code
	fstp	qword ptr [bp-14]
	mov	ax,0
	mov	[bp-2],ax
L283:
	mov	ax,[bp-2]
	push	ax
	mov	ax,10
	pop	cx
	cmp	cx,ax
	jl	L284
	jmp	L282
L284:
	fld	qword ptr [bp-14]
	mov	ax,offset double_array
	push	ax
	mov	ax,[bp-2]
	pop	cx
	add	ax,cx
	fld	qword ptr [bp+0]
	faddp	st(1),st
	fstp	qword ptr [bp-14]
	mov	ax,1
	push	ax
	mov	ax,[bp-2]
	pop	cx
	add	ax,cx
	mov	[bp-2],ax
	jmp	L283
L282:
	mov	ax,[bp-16]
	jmp	L269
L269:
	add	sp,16
	pop	bp
	ret
	public	swap_floats
swap_floats:	push	bp
	mov	bp,sp
	add	sp,-4
	mov	ax,[bp+4]
	fld	dword ptr [bp+0]
	fstp	dword ptr [bp-4]
	mov	ax,[bp+4]
	push	ax
	mov	ax,[bp+6]
	fld	dword ptr [bp+0]
	pop	bx
	fstp	dword ptr [bp+0]
	mov	ax,[bp+6]
	fld	dword ptr [bp-4]
	pop	bx
	fstp	dword ptr [bp+0]
L285:
	add	sp,4
	pop	bp
	ret
	public	swap_doubles
swap_doubles:	push	bp
	mov	bp,sp
	add	sp,-8
	mov	ax,[bp+4]
	fld	qword ptr [bp+0]
	fstp	qword ptr [bp-8]
	mov	ax,[bp+4]
	push	ax
	mov	ax,[bp+6]
	fld	qword ptr [bp+0]
	pop	bx
	fstp	qword ptr [bp+0]
	mov	ax,[bp+6]
	fld	qword ptr [bp-8]
	pop	bx
	fstp	qword ptr [bp+0]
L286:
	add	sp,8
	pop	bp
	ret
	public	test_float_pointers
test_float_pointers:	push	bp
	mov	bp,sp
	add	sp,-30
	mov	ax,1
	mov	[bp-30],ax
	.data
L288:
	dd	0
	dd	1072693248
	fld	qword ptr L288
	.code
	fstp	dword ptr [bp-4]
	.data
L289:
	dd	0
	dd	1073741824
	fld	qword ptr L289
	.code
	fstp	dword ptr [bp-8]
	lea	ax,[bp-8]
	push	ax
	lea	ax,[bp-4]
	push	ax
	call	swap_floats
	add	sp,4
	.data
L290:
	dd	0
	dd	1074266112
	fld	qword ptr L290
	.code
	fstp	qword ptr [bp-16]
	.data
L291:
	dd	0
	dd	1074790400
	fld	qword ptr L291
	.code
	fstp	qword ptr [bp-24]
	lea	ax,[bp-24]
	push	ax
	lea	ax,[bp-16]
	push	ax
	call	swap_doubles
	add	sp,4
	mov	ax,offset float_array
	mov	[bp-26],ax
	mov	ax,[bp-26]
	fld	dword ptr [bp+0]
	fstp	dword ptr [bp-4]
	mov	ax,1
	push	ax
	mov	ax,[bp-26]
	pop	cx
	add	ax,cx
	mov	[bp-26],ax
	mov	ax,[bp-26]
	fld	dword ptr [bp+0]
	fstp	dword ptr [bp-8]
	mov	ax,offset double_array
	mov	[bp-28],ax
	mov	ax,[bp-28]
	fld	qword ptr [bp+0]
	fstp	qword ptr [bp-16]
	mov	ax,1
	push	ax
	mov	ax,[bp-28]
	pop	cx
	add	ax,cx
	mov	[bp-28],ax
	mov	ax,[bp-28]
	fld	qword ptr [bp+0]
	fstp	qword ptr [bp-24]
	mov	ax,[bp-30]
	jmp	L287
L287:
	add	sp,30
	pop	bp
	ret
	public	max_float
max_float:	push	bp
	mov	bp,sp
	fld	dword ptr [bp+4]
	fld	dword ptr [bp+6]
	fcompp
	fnstsw	ax
	sahf
	pop	cx
	cmp	cx,ax
	jg	L294
	jmp	L293
L294:
	fld	dword ptr [bp+4]
	jmp	L292
	jmp	L295
L293:
	fld	dword ptr [bp+6]
	jmp	L292
L295:
L292:
	pop	bp
	ret
	public	min_float
min_float:	push	bp
	mov	bp,sp
	fld	dword ptr [bp+4]
	fld	dword ptr [bp+6]
	fcompp
	fnstsw	ax
	sahf
	pop	cx
	cmp	cx,ax
	jl	L298
	jmp	L297
L298:
	fld	dword ptr [bp+4]
	jmp	L296
	jmp	L299
L297:
	fld	dword ptr [bp+6]
	jmp	L296
L299:
L296:
	pop	bp
	ret
	public	abs_float
abs_float:	push	bp
	mov	bp,sp
	.data
L301:
	dd	0
	dd	0
	.code
	fld	dword ptr [bp+4]
	fld	qword ptr L301
	fcompp
	fnstsw	ax
	sahf
	pop	cx
	cmp	cx,ax
	jl	L303
	jmp	L302
L303:
	fld	dword ptr [bp+4]
	fchs
	jmp	L300
	jmp	L304
L302:
	fld	dword ptr [bp+4]
	jmp	L300
L304:
L300:
	pop	bp
	ret
	public	test_float_conditionals
test_float_conditionals:	push	bp
	mov	bp,sp
	add	sp,-6
	mov	ax,1
	mov	[bp-6],ax
	.data
L306:
	dd	0
	dd	1074266112
L307:
	dd	0
	dd	1075052544
	fld	qword ptr L307
	push	ax
	fld	qword ptr L306
	push	ax
	.code
	call	max_float
	add	sp,4
	fstp	dword ptr [bp-4]
	.data
L308:
	dd	0
	dd	1075576832
L309:
	dd	0
	dd	1073741824
	fld	qword ptr L309
	push	ax
	fld	qword ptr L308
	push	ax
	.code
	call	max_float
	add	sp,4
	fstp	dword ptr [bp-4]
	.data
L310:
	dd	0
	dd	1074266112
L311:
	dd	0
	dd	1075052544
	fld	qword ptr L311
	push	ax
	fld	qword ptr L310
	push	ax
	.code
	call	min_float
	add	sp,4
	fstp	dword ptr [bp-4]
	.data
L312:
	dd	0
	dd	1075576832
L313:
	dd	0
	dd	1073741824
	fld	qword ptr L313
	push	ax
	fld	qword ptr L312
	push	ax
	.code
	call	min_float
	add	sp,4
	fstp	dword ptr [bp-4]
	.data
L314:
	dd	0
	dd	1074921472
	fld	qword ptr L314
	fchs
	push	ax
	.code
	call	abs_float
	add	sp,2
	fstp	dword ptr [bp-4]
	.data
L315:
	dd	0
	dd	1074921472
	fld	qword ptr L315
	push	ax
	.code
	call	abs_float
	add	sp,2
	fstp	dword ptr [bp-4]
	mov	ax,[bp-6]
	jmp	L305
L305:
	add	sp,6
	pop	bp
	ret
	public	sum_series
sum_series:	push	bp
	mov	bp,sp
	add	sp,-6
	.data
L317:
	dd	0
	dd	0
	fld	qword ptr L317
	.code
	fstp	dword ptr [bp-4]
	mov	ax,1
	mov	[bp-6],ax
L319:
	mov	ax,[bp-6]
	push	ax
	mov	ax,[bp+4]
	pop	cx
	cmp	cx,ax
	jle	L320
	jmp	L318
L320:
	fld	dword ptr [bp-4]
	fld	dword ptr [bp-6]
	faddp	st(1),st
	fstp	dword ptr [bp-4]
	mov	ax,1
	push	ax
	mov	ax,[bp-6]
	pop	cx
	add	ax,cx
	mov	[bp-6],ax
	jmp	L319
L318:
	fld	dword ptr [bp-4]
	jmp	L316
L316:
	add	sp,6
	pop	bp
	ret
	public	factorial_float
factorial_float:	push	bp
	mov	bp,sp
	add	sp,-6
	.data
L322:
	dd	0
	dd	1072693248
	fld	qword ptr L322
	.code
	fstp	dword ptr [bp-4]
	mov	ax,2
	mov	[bp-6],ax
L324:
	mov	ax,[bp-6]
	push	ax
	mov	ax,[bp+4]
	pop	cx
	cmp	cx,ax
	jle	L325
	jmp	L323
L325:
	fld	dword ptr [bp-4]
	fld	dword ptr [bp-6]
	fmulp	st(1),st
	fstp	dword ptr [bp-4]
	mov	ax,1
	push	ax
	mov	ax,[bp-6]
	pop	cx
	add	ax,cx
	mov	[bp-6],ax
	jmp	L324
L323:
	fld	dword ptr [bp-4]
	jmp	L321
L321:
	add	sp,6
	pop	bp
	ret
	public	test_float_loops
test_float_loops:	push	bp
	mov	bp,sp
	add	sp,-6
	mov	ax,1
	mov	[bp-6],ax
	mov	ax,10
	push	ax
	call	sum_series
	add	sp,2
	fstp	dword ptr [bp-4]
	mov	ax,100
	push	ax
	call	sum_series
	add	sp,2
	fstp	dword ptr [bp-4]
	mov	ax,5
	push	ax
	call	factorial_float
	add	sp,2
	fstp	dword ptr [bp-4]
	mov	ax,10
	push	ax
	call	factorial_float
	add	sp,2
	fstp	dword ptr [bp-4]
	mov	ax,[bp-6]
	jmp	L326
L326:
	add	sp,6
	pop	bp
	ret
	public	main
main:	push	bp
	mov	bp,sp
	mov	ax,0
	mov	tests_passed,ax
	mov	ax,0
	mov	tests_failed,ax
	.data
L328:
	db	'f'
	db	'l'
	db	'o'
	db	'a'
	db	't'
	db	95
	db	'a'
	db	'r'
	db	'i'
	db	't'
	db	'h'
	db	'm'
	db	'e'
	db	't'
	db	'i'
	db	'c'
	db	0
	db	0
	.code
	call	test_float_arithmetic
	push	ax
	mov	ax,offset L328
	push	ax
	call	report
	add	sp,4
	.data
L329:
	db	'd'
	db	'o'
	db	'u'
	db	'b'
	db	'l'
	db	'e'
	db	95
	db	'a'
	db	'r'
	db	'i'
	db	't'
	db	'h'
	db	'm'
	db	'e'
	db	't'
	db	'i'
	db	'c'
	db	0
	.code
	call	test_double_arithmetic
	push	ax
	mov	ax,offset L329
	push	ax
	call	report
	add	sp,4
	.data
L330:
	db	'f'
	db	'l'
	db	'o'
	db	'a'
	db	't'
	db	95
	db	'c'
	db	'o'
	db	'm'
	db	'p'
	db	'a'
	db	'r'
	db	'i'
	db	's'
	db	'o'
	db	'n'
	db	's'
	db	0
	.code
	call	test_float_comparisons
	push	ax
	mov	ax,offset L330
	push	ax
	call	report
	add	sp,4
	.data
L331:
	db	'd'
	db	'o'
	db	'u'
	db	'b'
	db	'l'
	db	'e'
	db	95
	db	'c'
	db	'o'
	db	'm'
	db	'p'
	db	'a'
	db	'r'
	db	'i'
	db	's'
	db	'o'
	db	'n'
	db	's'
	db	0
	db	0
	.code
	call	test_double_comparisons
	push	ax
	mov	ax,offset L331
	push	ax
	call	report
	add	sp,4
	.data
L332:
	db	'f'
	db	'l'
	db	'o'
	db	'a'
	db	't'
	db	95
	db	'c'
	db	'o'
	db	'n'
	db	'v'
	db	'e'
	db	'r'
	db	's'
	db	'i'
	db	'o'
	db	'n'
	db	's'
	db	0
	.code
	call	test_float_conversions
	push	ax
	mov	ax,offset L332
	push	ax
	call	report
	add	sp,4
	.data
L333:
	db	'm'
	db	'a'
	db	'n'
	db	'y'
	db	95
	db	'f'
	db	'l'
	db	'o'
	db	'a'
	db	't'
	db	95
	db	'p'
	db	'a'
	db	'r'
	db	'a'
	db	'm'
	db	's'
	db	0
	.code
	call	test_many_float_params
	push	ax
	mov	ax,offset L333
	push	ax
	call	report
	add	sp,4
	.data
L334:
	db	'm'
	db	'a'
	db	'n'
	db	'y'
	db	95
	db	'd'
	db	'o'
	db	'u'
	db	'b'
	db	'l'
	db	'e'
	db	95
	db	'p'
	db	'a'
	db	'r'
	db	'a'
	db	'm'
	db	's'
	db	0
	db	0
	.code
	call	test_many_double_params
	push	ax
	mov	ax,offset L334
	push	ax
	call	report
	add	sp,4
	.data
L335:
	db	'm'
	db	'i'
	db	'x'
	db	'e'
	db	'd'
	db	95
	db	'o'
	db	'p'
	db	'e'
	db	'r'
	db	'a'
	db	't'
	db	'i'
	db	'o'
	db	'n'
	db	's'
	db	0
	db	0
	.code
	call	test_mixed_operations
	push	ax
	mov	ax,offset L335
	push	ax
	call	report
	add	sp,4
	.data
L336:
	db	'f'
	db	'l'
	db	'o'
	db	'a'
	db	't'
	db	95
	db	'n'
	db	'e'
	db	'g'
	db	'a'
	db	't'
	db	'i'
	db	'o'
	db	'n'
	db	0
	db	0
	.code
	call	test_float_negation
	push	ax
	mov	ax,offset L336
	push	ax
	call	report
	add	sp,4
	.data
L337:
	db	'f'
	db	'l'
	db	'o'
	db	'a'
	db	't'
	db	95
	db	'a'
	db	's'
	db	's'
	db	'i'
	db	'g'
	db	'n'
	db	'm'
	db	'e'
	db	'n'
	db	't'
	db	0
	db	0
	.code
	call	test_float_assignment
	push	ax
	mov	ax,offset L337
	push	ax
	call	report
	add	sp,4
	.data
L338:
	db	'g'
	db	'l'
	db	'o'
	db	'b'
	db	'a'
	db	'l'
	db	95
	db	'f'
	db	'l'
	db	'o'
	db	'a'
	db	't'
	db	's'
	db	0
	.code
	call	test_global_floats
	push	ax
	mov	ax,offset L338
	push	ax
	call	report
	add	sp,4
	.data
L339:
	db	'f'
	db	'l'
	db	'o'
	db	'a'
	db	't'
	db	95
	db	'a'
	db	'r'
	db	'r'
	db	'a'
	db	'y'
	db	's'
	db	0
	db	0
	.code
	call	test_float_arrays
	push	ax
	mov	ax,offset L339
	push	ax
	call	report
	add	sp,4
	.data
L340:
	db	'f'
	db	'l'
	db	'o'
	db	'a'
	db	't'
	db	95
	db	'p'
	db	'o'
	db	'i'
	db	'n'
	db	't'
	db	'e'
	db	'r'
	db	's'
	db	0
	db	0
	.code
	call	test_float_pointers
	push	ax
	mov	ax,offset L340
	push	ax
	call	report
	add	sp,4
	.data
L341:
	db	'f'
	db	'l'
	db	'o'
	db	'a'
	db	't'
	db	95
	db	'c'
	db	'o'
	db	'n'
	db	'd'
	db	'i'
	db	't'
	db	'i'
	db	'o'
	db	'n'
	db	'a'
	db	'l'
	db	's'
	db	0
	db	0
	.code
	call	test_float_conditionals
	push	ax
	mov	ax,offset L341
	push	ax
	call	report
	add	sp,4
	.data
L342:
	db	'f'
	db	'l'
	db	'o'
	db	'a'
	db	't'
	db	95
	db	'l'
	db	'o'
	db	'o'
	db	'p'
	db	's'
	db	0
	.code
	call	test_float_loops
	push	ax
	mov	ax,offset L342
	push	ax
	call	report
	add	sp,4
	mov	ax,tests_failed
	jmp	L327
L327:
	pop	bp
	ret
	end
