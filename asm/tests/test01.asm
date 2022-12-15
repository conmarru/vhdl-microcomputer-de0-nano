.main
	loadi	$0, %r0
	loadi	$5, %r1
	jmp increment
	.back
	hlt

.increment
	loadi	$1, %r2
	.check 
	sub	%r1, %r0, %r3
	bre	%r3, done
	add	%r2, %r0, %r0
	jmp	check
	.done
	move	%r0, %r31
	jmp back


