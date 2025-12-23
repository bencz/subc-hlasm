	.text
	.data
	.globl	global_short
global_short:	.word	0
	.globl	global_ushort
global_ushort:	.word	0
	.globl	global_long
global_long:	.long	0
	.globl	global_ulong
global_ulong:	.long	0
	.globl	global_uint
global_uint:	.long	0
	.text
	.globl	add_shorts
add_shorts:	pushl	%ebp
	movl	%esp,%ebp
	movl	12(%ebp),%eax
	pushl	%eax
	movl	8(%ebp),%eax
	popl	%ecx
	addl	%ecx,%eax
	jmp	L1
L1:
	popl	%ebp
	ret
	.globl	add_longs
add_longs:	pushl	%ebp
	movl	%esp,%ebp
	movl	12(%ebp),%eax
	pushl	%eax
	movl	8(%ebp),%eax
	popl	%ecx
	addl	%ecx,%eax
	jmp	L2
L2:
	popl	%ebp
	ret
	.globl	add_uints
add_uints:	pushl	%ebp
	movl	%esp,%ebp
	movl	12(%ebp),%eax
	pushl	%eax
	movl	8(%ebp),%eax
	popl	%ecx
	addl	%ecx,%eax
	jmp	L3
L3:
	popl	%ebp
	ret
	.globl	main
main:	pushl	%ebp
	movl	%esp,%ebp
	addl	$-16,%esp
	movl	$100,%eax
	movl	%eax,-4(%ebp)
	movl	$200,%eax
	movl	%eax,-8(%ebp)
	movl	$100000,%eax
	movl	%eax,-12(%ebp)
	movl	$200000,%eax
	movl	%eax,-16(%ebp)
	movl	$20,%eax
	pushl	%eax
	movl	$10,%eax
	pushl	%eax
	call	add_shorts
	addl	$8,%esp
	movl	%eax,global_short
	movl	$2000,%eax
	pushl	%eax
	movl	$1000,%eax
	pushl	%eax
	call	add_longs
	addl	$8,%esp
	movl	%eax,global_long
	movl	$600,%eax
	pushl	%eax
	movl	$500,%eax
	pushl	%eax
	call	add_uints
	addl	$8,%esp
	movl	%eax,global_uint
	movl	$0,%eax
	jmp	L4
L4:
	addl	$16,%esp
	popl	%ebp
	ret
