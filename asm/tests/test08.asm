.start
	loadi	$1024, %r30
	loadi	$0xa, %r0
	loadi	$5, %r1
	call 	sum
	move	%r31, %r2
	call	dif
	move	%r31, %r3
	hlt
.sum
	add	%r0, %r1, %r31
	ret
.dif
	sub	%r0, %r1, %r31
	ret
	
	

