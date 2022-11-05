;  a3f    208     // 208 Hz
;  b3f    233     // 233 Hz
;  b3     247     // 247 Hz
;  c4     261     // 261 Hz MIDDLE C
;  c4s    277     // 277 Hz
;  e4f    311     // 311 Hz    
;  f4     349     // 349 Hz 
;  a4f    415     // 415 Hz  
;  b4f    466     // 466 Hz 
;  b4     493     // 493 Hz 
;  c5     523     // 523 Hz 
;  c5s    554     // 554 Hz
;  e5f    622     // 622 Hz  
;  f5     698     // 698 Hz 
;  f5s    740     // 740 Hz
;  a5f    831     // 831 Hz 

start:
        gosub c4
done:
	jr done

a5f:
	mov r1, 1
	mov r6, 7
	mov r7, 15
	mov r8, 15
a5f_loop:
	xor out,r1
	mov r2,15
delay0:
	dec r2
        skip z,1
	jr delay0 ; 1042Hz
	and r1,r1 ; 1026Hz
	and r1,r1 ; 1000Hz
	and r1,r1 ; 986Hz
	and r1,r1 ; 962Hz
	and r1,r1 ; 949Hz
	and r1,r1 ; 923Hz
	and r1,r1 ;
	and r1,r1 ;
	and r1,r1 ; 850Hz
	dec r8
	skip nz,2
	mov r8,15 ; reload lsb
	dec r7    ; decrement 2sb
	skip nz,2
	mov r7,15 ; decrement 3sb
	dec r6
	skip nz,1
	ret r0,0
	jr a5f_loop

c4:
	mov r1, 1
	mov r6, 3
	mov r7, 13
	mov r8, 15
c4_loop:
	xor out,r1
	mov r2,15
c4_d0:
	dec r2
        skip z,1
	jr c4_d0 ; 1042Hz
	mov r2,15
c4_d1:
	dec r2
        skip z,1
	jr c4_d1 ;  821 Hz
	mov r2,15
c4_d2:
	dec r2
        skip z,1
	jr c4_d2 ; 350Hz
	mov r2,15
c4_d3:
	dec r2
        skip z,1
	jr c4_d3 ; 302Hz
	
	; nested loop counters
	dec r8
	skip nz,2
	mov r8,15 ; reload lsb
	dec r7    ; decrement 2sb
	skip nz,2
	mov r7,13 ; decrement 3sb
	dec r6
	skip nz,2
	mov out,0
	ret r0,0
	jr c4_loop
