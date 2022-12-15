.start
	loadi	$1024, %r30
	loadi	$7, %r0
	call	fibonacci
	hlt

.fibonacci
	loadi	$0, %r1
	loadi	$0, %r2
	loadi	$1, %r3
	loadi	$2, %r4
	.test
		sub	%r0, %r1, %r5
		bre	%r5, done
		add	%r2, %r3, %r4
		move	%r3, %r2
		move	%r4, %r3
		addi	$1, %r1, %r1
		jmp	test
	.done
		move %r2, %r31
		ret
