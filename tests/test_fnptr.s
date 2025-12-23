	.text
	.globl	test_void
test_void:	pushl	%ebp
	movl	%esp,%ebp
L1:
	popl	%ebp
	ret
	.globl	test_int
test_int:	pushl	%ebp
	movl	%esp,%ebp
L2:
	popl	%ebp
	ret
	.globl	test_str
test_str:	pushl	%ebp
	movl	%esp,%ebp
L3:
	popl	%ebp
	ret
	.globl	test_two
test_two:	pushl	%ebp
	movl	%esp,%ebp
L4:
	popl	%ebp
	ret
	.globl	test_ret
test_ret:	pushl	%ebp
	movl	%esp,%ebp
	movl	$42,%eax
	jmp	L5
L5:
	popl	%ebp
	ret
	.globl	test_retstr
test_retstr:	pushl	%ebp
	movl	%esp,%ebp
	movl	8(%ebp),%eax
	jmp	L6
L6:
	popl	%ebp
	ret
	.data
	.globl	vtable
	.comm	vtable,24
	.text
	.globl	test_calls
test_calls:	pushl	%ebp
	movl	%esp,%ebp
	addl	$-4,%esp
	movl	$vtable,%eax
	pushl	%eax
	movl	$test_void,%eax
	popl	%edx
	movl	%eax,(%edx)
	movl	$vtable,%eax
	pushl	%eax
	movl	$4,%eax
	popl	%ecx
	addl	%ecx,%eax
	pushl	%eax
	movl	$test_int,%eax
	popl	%edx
	movl	%eax,(%edx)
	movl	$vtable,%eax
	pushl	%eax
	movl	$8,%eax
	popl	%ecx
	addl	%ecx,%eax
	pushl	%eax
	movl	$test_str,%eax
	popl	%edx
	movl	%eax,(%edx)
	movl	$vtable,%eax
	pushl	%eax
	movl	$12,%eax
	popl	%ecx
	addl	%ecx,%eax
	pushl	%eax
	movl	$test_two,%eax
	popl	%edx
	movl	%eax,(%edx)
	movl	$vtable,%eax
	pushl	%eax
	movl	$16,%eax
	popl	%ecx
	addl	%ecx,%eax
	pushl	%eax
	movl	$test_ret,%eax
	popl	%edx
	movl	%eax,(%edx)
	movl	$vtable,%eax
	pushl	%eax
	movl	$20,%eax
	popl	%ecx
	addl	%ecx,%eax
	pushl	%eax
	movl	$test_retstr,%eax
	popl	%edx
	movl	%eax,(%edx)
	movl	$vtable,%eax
	movl	(%eax),%eax
	pushl	%eax
	popl	%ecx
	xchgl	%eax,%ecx
	call	*%eax
	movl	$vtable,%eax
	pushl	%eax
	movl	$4,%eax
	popl	%ecx
	addl	%ecx,%eax
	movl	(%eax),%eax
	pushl	%eax
	movl	$10,%eax
	pushl	%eax
	popl	%ecx
	xchgl	%eax,%ecx
	call	*%eax
	addl	$4,%esp
	.data
L8:
	.byte	'h'
	.byte	'e'
	.byte	'l'
	.byte	'l'
	.byte	'o'
	.byte	0
	.byte	0
	.byte	0
	.text
	movl	$vtable,%eax
	pushl	%eax
	movl	$8,%eax
	popl	%ecx
	addl	%ecx,%eax
	movl	(%eax),%eax
	pushl	%eax
	movl	$L8,%eax
	pushl	%eax
	popl	%ecx
	xchgl	%eax,%ecx
	call	*%eax
	addl	$4,%esp
	movl	$vtable,%eax
	pushl	%eax
	movl	$12,%eax
	popl	%ecx
	addl	%ecx,%eax
	movl	(%eax),%eax
	pushl	%eax
	movl	$2,%eax
	pushl	%eax
	movl	$1,%eax
	pushl	%eax
	popl	%ecx
	xchgl	%eax,%ecx
	call	*%eax
	addl	$8,%esp
	movl	$vtable,%eax
	pushl	%eax
	movl	$16,%eax
	popl	%ecx
	addl	%ecx,%eax
	movl	(%eax),%eax
	pushl	%eax
	popl	%ecx
	xchgl	%eax,%ecx
	call	*%eax
	movl	%eax,-4(%ebp)
	.data
L9:
	.byte	'w'
	.byte	'o'
	.byte	'r'
	.byte	'l'
	.byte	'd'
	.byte	0
	.byte	0
	.byte	0
	.text
	movl	$vtable,%eax
	pushl	%eax
	movl	$20,%eax
	popl	%ecx
	addl	%ecx,%eax
	movl	(%eax),%eax
	pushl	%eax
	movl	$L9,%eax
	pushl	%eax
	popl	%ecx
	xchgl	%eax,%ecx
	call	*%eax
	addl	$4,%esp
	movl	-4(%ebp),%eax
	jmp	L7
L7:
	addl	$4,%esp
	popl	%ebp
	ret
	.globl	main
main:	pushl	%ebp
	movl	%esp,%ebp
	call	test_calls
	pushl	%eax
	movl	$42,%eax
	popl	%ecx
	xchgl	%eax,%ecx
	subl	%ecx,%eax
	jmp	L10
L10:
	popl	%ebp
	ret
