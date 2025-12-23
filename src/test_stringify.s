	.text
	.data
	.globl	var1
var1:	.long	10
	.globl	var2
var2:	.long	20
	.text
	.globl	main
main:	pushl	%ebp
	movl	%esp,%ebp
	addl	$-4,%esp
	.data
L2:
	.byte	'h'
	.byte	'e'
	.byte	'l'
	.byte	'l'
	.byte	'o'
	.byte	0
	.byte	0
	.byte	0
	.text
	movl	$L2,%eax
	movl	%eax,-4(%ebp)
	movl	var2,%eax
	pushl	%eax
	movl	var1,%eax
	popl	%ecx
	addl	%ecx,%eax
	jmp	L1
L1:
	addl	$4,%esp
	popl	%ebp
	ret
