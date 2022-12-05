loadi 	$512, %r30
call	func
store	%r30, $1024
hlt

.func
loadi	$7, %r0
ret
