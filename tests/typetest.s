	.text
	.data
	.globl	_c
_c:	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.globl	_sc
_sc:	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.globl	_uc
_uc:	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.globl	_s
_s:	.word	0
	.globl	_si
_si:	.word	0
	.globl	_ss
_ss:	.word	0
	.globl	_ssi
_ssi:	.word	0
	.globl	_us
_us:	.word	0
	.globl	_usi
_usi:	.word	0
	.globl	_i
_i:	.quad	0
	.globl	_x
_x:	.quad	0
	.globl	_sx
_sx:	.quad	0
	.globl	_u
_u:	.quad	0
	.globl	_ui
_ui:	.quad	0
	.globl	_l
_l:	.quad	0
	.globl	_li
_li:	.quad	0
	.globl	_sl
_sl:	.quad	0
	.globl	_sli
_sli:	.quad	0
	.globl	_ul
_ul:	.quad	0
	.globl	_uli
_uli:	.quad	0
	.globl	_sp
_sp:	.quad	0
	.globl	_uip
_uip:	.quad	0
	.globl	_lp
_lp:	.quad	0
	.globl	_ulp
_ulp:	.quad	0
	.globl	_ci
_ci:	.quad	0
	.globl	_ccp
_ccp:	.quad	0
	.text
	.globl	_add_shorts
_add_shorts:	pushq	%rbp
	movq	%rsp,%rbp
	pushq	%rdi
	pushq	%rsi
	movq	-16(%rbp),%rax
	pushq	%rax
	movq	-8(%rbp),%rax
	popq	%rcx
	addq	%rcx,%rax
	jmp	L1
L1:
	popq	%rbp
	ret
	.globl	_mul_ulong
_mul_ulong:	pushq	%rbp
	movq	%rsp,%rbp
	pushq	%rdi
	pushq	%rsi
	movq	-16(%rbp),%rax
	pushq	%rax
	movq	-8(%rbp),%rax
	popq	%rcx
	imulq	%rcx,%rax
	jmp	L2
L2:
	popq	%rbp
	ret
	.globl	_main
_main:	pushq	%rbp
	movq	%rsp,%rbp
	addq	$-24,%rsp
	movq	$100,%rax
	movq	%rax,-8(%rbp)
	movq	$50000,%rax
	movq	%rax,-16(%rbp)
	movq	$100000,%rax
	movq	%rax,-24(%rbp)
	movq	$20,%rax
	movq	%rax,%rsi
	movq	$10,%rax
	movq	%rax,%rdi
	call	_add_shorts
	movq	%rax,_s
	movq	$2000,%rax
	movq	%rax,%rsi
	movq	$1000,%rax
	movq	%rax,%rdi
	call	_mul_ulong
	movq	%rax,_ul
	movq	$0,%rax
	jmp	L3
L3:
	addq	$24,%rsp
	popq	%rbp
	ret
