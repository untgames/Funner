OPTION	DOTNAME
;EXTERN	OPENSSL_cpuid_setup:NEAR
;.CRT$XCU SEGMENT READONLY DWORD
;DD	OPENSSL_cpuid_setup
;.CRT$XCU ENDS
_DATA	SEGMENT
COMM _OPENSSL_ia32cap_P:DWORD:2
_DATA ENDS
.text$	SEGMENT ALIGN(64) 'CODE'
PUBLIC	_OPENSSL_ia32_cpuid
ALIGN 16
_OPENSSL_ia32_cpuid:
L$_OPENSSL_ia32_cpuid_begin:
	push	ebp
	push	ebx
	push	esi
	push	edi
	xor	edx,edx
	pushfd
	pop	eax
	mov	ecx,eax
	xor	eax,2097152
	push	eax
	popfd
	pushfd
	pop	eax
	xor	ecx,eax
	bt	ecx,21
	jnc	L$000generic
	xor	eax,eax
	cpuid
	mov	edi,eax
	xor	eax,eax
	cmp	ebx,1970169159
	setne	al
	mov	ebp,eax
	cmp	edx,1231384169
	setne	al
	or	ebp,eax
	cmp	ecx,1818588270
	setne	al
	or	ebp,eax
	jz	L$001intel
	cmp	ebx,1752462657
	setne	al
	mov	esi,eax
	cmp	edx,1769238117
	setne	al
	or	esi,eax
	cmp	ecx,1145913699
	setne	al
	or	esi,eax
	jnz	L$001intel
	mov	eax,2147483648
	cpuid
	cmp	eax,2147483649
	jb	L$001intel
	mov	esi,eax
	mov	eax,2147483649
	cpuid
	or	ebp,ecx
	and	ebp,2049
	cmp	esi,2147483656
	jb	L$001intel
	mov	eax,2147483656
	cpuid
	movzx	esi,cl
	inc	esi
	mov	eax,1
	cpuid
	bt	edx,28
	jnc	L$000generic
	shr	ebx,16
	and	ebx,255
	cmp	ebx,esi
	ja	L$000generic
	and	edx,4026531839
	jmp	L$000generic
L$001intel:
	cmp	edi,4
	mov	edi,-1
	jb	L$002nocacheinfo
	mov	eax,4
	mov	ecx,0
	cpuid
	mov	edi,eax
	shr	edi,14
	and	edi,4095
L$002nocacheinfo:
	mov	eax,1
	cpuid
	and	edx,3220176895
	cmp	ebp,0
	jne	L$003notintel
	or	edx,1073741824
	and	ah,15
	cmp	ah,15
	jne	L$003notintel
	or	edx,1048576
L$003notintel:
	bt	edx,28
	jnc	L$000generic
	and	edx,4026531839
	cmp	edi,0
	je	L$000generic
	or	edx,268435456
	shr	ebx,16
	cmp	bl,1
	ja	L$000generic
	and	edx,4026531839
L$000generic:
	and	ebp,2048
	and	ecx,4294965247
	mov	esi,edx
	or	ebp,ecx
	bt	ecx,26
	jnc	L$004done
	bt	ecx,27
	jnc	L$005clear_xmm
	xor	ecx,ecx
db	15,1,208
	and	eax,6
	cmp	eax,6
	je	L$004done
	cmp	eax,2
	je	L$006clear_avx
L$005clear_xmm:
	and	ebp,4261412861
	and	esi,4278190079
L$006clear_avx:
	and	ebp,4026525695
L$004done:
	mov	eax,esi
	mov	edx,ebp
	pop	edi
	pop	esi
	pop	ebx
	pop	ebp
	ret
;extern	_OPENSSL_ia32cap_P
PUBLIC	_OPENSSL_rdtsc
align	16
_OPENSSL_rdtsc:
L$_OPENSSL_rdtsc_begin:
	xor	eax,eax
	xor	edx,edx
	lea	ecx,[_OPENSSL_ia32cap_P]
	bt	DWORD PTR [ecx],4
	jnc	L$007notsc
	rdtsc
L$007notsc:
	ret
PUBLIC	_OPENSSL_instrument_halt
align	16
_OPENSSL_instrument_halt:
L$_OPENSSL_instrument_halt_begin:
	lea	ecx,[_OPENSSL_ia32cap_P]
	bt	DWORD PTR [ecx],4
	jnc	L$008nohalt
dd	2421723150
	and	eax,3
	jnz	L$008nohalt
	pushfd
	pop	eax
	bt	eax,9
	jnc	L$008nohalt
	rdtsc
	push	edx
	push	eax
	hlt
	rdtsc
	sub	eax,DWORD PTR [esp]
	sbb	edx,DWORD PTR [4+esp]
	add	esp,8
	ret
L$008nohalt:
	xor	eax,eax
	xor	edx,edx
	ret
PUBLIC	_OPENSSL_far_spin
align	16
_OPENSSL_far_spin:
L$_OPENSSL_far_spin_begin:
	pushfd
	pop	eax
	bt	eax,9
	jnc	L$009nospin
	mov	eax,DWORD PTR [4+esp]
	mov	ecx,DWORD PTR [8+esp]
dd	2430111262
	xor	eax,eax
	mov	edx,DWORD PTR [ecx]
	jmp	L$010spin
align	16
L$010spin:
	inc	eax
	cmp	edx,DWORD PTR [ecx]
	je	L$010spin
dd	529567888
	ret
L$009nospin:
	xor	eax,eax
	xor	edx,edx
	ret
PUBLIC	_OPENSSL_wipe_cpu
align	16
_OPENSSL_wipe_cpu:
L$_OPENSSL_wipe_cpu_begin:
	xor	eax,eax
	xor	edx,edx
	lea	ecx,[_OPENSSL_ia32cap_P]
	mov	ecx,DWORD PTR [ecx]
	bt	DWORD PTR [ecx],1
	jnc	L$011no_x87
dd	4007259865,4007259865,4007259865,4007259865,2430851995
L$011no_x87:
	lea	eax,[4+esp]
	ret
PUBLIC	_OPENSSL_atomic_add
align	16
_OPENSSL_atomic_add:
L$_OPENSSL_atomic_add_begin:
	mov	edx,DWORD PTR [4+esp]
	mov	ecx,DWORD PTR [8+esp]
	push	ebx
	nop
	mov	eax,DWORD PTR [edx]
L$012spin:
	lea	ebx,[ecx*1+eax]
	nop
dd	447811568
	jne	L$012spin
	mov	eax,ebx
	pop	ebx
	ret
PUBLIC	_OPENSSL_indirect_call
align	16
_OPENSSL_indirect_call:
L$_OPENSSL_indirect_call_begin:
	push	ebp
	mov	ebp,esp
	sub	esp,28
	mov	ecx,DWORD PTR [12+ebp]
	mov	DWORD PTR [esp],ecx
	mov	edx,DWORD PTR [16+ebp]
	mov	DWORD PTR [4+esp],edx
	mov	eax,DWORD PTR [20+ebp]
	mov	DWORD PTR [8+esp],eax
	mov	eax,DWORD PTR [24+ebp]
	mov	DWORD PTR [12+esp],eax
	mov	eax,DWORD PTR [28+ebp]
	mov	DWORD PTR [16+esp],eax
	mov	eax,DWORD PTR [32+ebp]
	mov	DWORD PTR [20+esp],eax
	mov	eax,DWORD PTR [36+ebp]
	mov	DWORD PTR [24+esp],eax
	call	DWORD PTR [8+ebp]
	mov	esp,ebp
	pop	ebp
	ret
PUBLIC	_OPENSSL_cleanse
align	16
_OPENSSL_cleanse:
L$_OPENSSL_cleanse_begin:
	mov	edx,DWORD PTR [4+esp]
	mov	ecx,DWORD PTR [8+esp]
	xor	eax,eax
	cmp	ecx,7
	jae	L$013lot
	cmp	ecx,0
	je	L$014ret
L$015little:
	mov	BYTE PTR [edx],al
	sub	ecx,1
	lea	edx,[1+edx]
	jnz	L$015little
L$014ret:
	ret
align	16
L$013lot:
	test	edx,3
	jz	L$016aligned
	mov	BYTE PTR [edx],al
	lea	ecx,[ecx-1]
	lea	edx,[1+edx]
	jmp	L$013lot
L$016aligned:
	mov	DWORD PTR [edx],eax
	lea	ecx,[ecx-4]
	test	ecx,-4
	lea	edx,[4+edx]
	jnz	L$016aligned
	cmp	ecx,0
	jne	L$015little
	ret
.text$ ENDS
END
