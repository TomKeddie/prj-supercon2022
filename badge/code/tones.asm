start:
	mov r0,1
mainloop:
	xor out,r0
	mov r1,15
delay0:
	dec r1
        skip z,1
	jr delay0
	jr mainloop
ASCII "time to add some padding"
