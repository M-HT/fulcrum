.386					;pmode/w
.model flat,prolog

locals

public frame
public dotrack, slerp, dorottrack, doltrack, dohidetrack

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
;struc
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
d equ dword ptr

;matrix
tmatrix struc
 A_1n label dword
 A_m1 label dword
 A_11		dd ?
 A_m2 label dword
 A_12		dd ?
 A_m3 label dword
 A_13		dd ?
 A_2n label dword
 A_21		dd ?
 A_22		dd ?
 A_23		dd ?
 A_3n label dword
 A_31		dd ?
 A_32		dd ?
 A_33		dd ?
ends

;quaternion
tquaternion struc  ;fixed size of 16 byte
 q_w		dd ?
 q_x		dd ?
 q_y		dd ?
 q_z		dd ?
ends

;track:
;------
ttrack struc
 t_data 	dd ?			;track-data
 t_key		dd ?			;actual key
ends

ttrackh struc
 th_mode	dd ?			;1: loop-track
 th_keys	dd ?			;number of gaps to interpolate (keys-1)
 th_sframe	dd ?			;start-frame
ends

tkey struc
 k_sframe	dd ?			;start-frame of gap
 ;k_easefrom	 dd ?
 ;k_easeto	 dd ?
 k_eframe	dd ?			;end-frame of gap
ends

;linear track:
;-------------
tltrack struc
 lt_startstate	dd ?
 lt_startframe	dd ?
 lt_endstate	dd ?
 lt_endframe	dd ?
 lt_keys	dd ?
 lt_data	dd ?
ends



;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
.data
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±

c_3		dd 3.0
c_sinmin	dd 1E-6

;extrn frame:dword
frame		dd ?			;global frame counter

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
.code
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±

dotrack proc pascal
arg	_track:dword, typesize:dword
	;edi -> result
local	key:dword

	push	esi

	mov	ebx,_track		;ebx -> ttrack
	mov	eax,t_key[ebx]
	mov	key,eax
	mov	esi,t_data[ebx] 	;esi -> track-data


	mov	ecx,th_keys[esi]
	imul	ecx,size tkey - 4
	add	ecx,size ttrackh	;ecx + esi -> control points

	mov	eax,th_sframe[esi]	;eax = startframe
	cmp	th_mode[esi],0
	jne	@@loop

		;single
	mov	edx,frame		;edx = frame
	cmp	edx,eax
	jbe	@@c			;before track-start
	mov	eax,[esi+ecx-4] 	;eax = endframe
	cmp	edx,eax
	jb	@@key
					;behind track-end
	mov	eax,typesize
	imul	eax,3*4
	imul	eax,th_keys[esi]
	add	ecx,eax

@@c:	add	esi,ecx 		;copy first or last control point
	mov	ecx,typesize
	rep	movsd
	jmp	@@weg

@@loop:
		;loop & repeat
	mov	ebx,[esi+ecx-4]
	sub	ebx,eax 		;ebx = endframe - startframe

	neg	eax
	add	eax,frame		;eax = frame - startframe
	cdq
	idiv	ebx
	or	edx,edx
	jns	@@0
	add	edx,ebx
@@0:	add	edx,th_sframe[esi]

@@key:					;edx = frame

		;keypos = (site ttrackh) - 4 + key*(size tkey - 4)
	mov	ebx,size tkey - 4
	imul	ebx,key
	add	ebx,size ttrackh - 4
	add	ebx,esi

	cmp	edx,k_sframe[ebx]	;search actual key
	jae	@@1
	mov	key,0
	lea	ebx,[esi + size ttrackh - 4]
@@1:
	cmp	edx,k_eframe[ebx]
	jb	@@2 ;e
	add	ebx,size tkey - 4
	inc	key
	jmp	@@1
@@2:


@@spline:
		;t
	mov	eax,k_sframe[ebx]
	sub	edx,eax 		;edx = (frame - sframe)
	mov	dword ptr [edi],edx	;temp
	fild	dword ptr [edi]
	neg	eax
	add	eax,k_eframe[ebx]	;eax = (eframe - sframe)
	mov	dword ptr [edi],eax	;temp
	fild	dword ptr [edi]
	fdivp	st(1),st		;t = (frame - sframe) / (eframe - sframe);

		;P1 * ((( 2*t)-3)*t*t +1)
		;P4 * (((-2*t)+3)*t*t)
	fld	st(0)
	fadd	st,st(0)		;2*t
	fsub	c_3			;-3
	fmul	st,st(1)		;*t
	fmul	st,st(1)		;*t
	fld	st
	fchs
	fld1
	fadd	st(2),st		;+1
		;st = 1.0; st(1) = R1, st(2) = R4, st(3) = t

		;R1 * (t-2)*t*t+t
		;R4 * (t-1)*t*t
	fadd	st,st(0)
	fsubr	st,st(3)		;t-2
	fmul	st,st(3)		;*t
	fmul	st,st(3)		;*t
	fadd	st,st(3)		;+t

	fld1
	fsubr	st,st(4)		;t-1
	fmul	st,st(4)		;*t
	fmul	st,st(4)		;*t
		;st = R4, st(1) = R1, st(2) = P4, st(3) = P1


	add	esi,ecx 		;esi -> control points

	mov	ebx,typesize
	mov	ecx,ebx 		;ecx = typesize
	shl	ebx,2
	mov	edx,ebx 		;edx = typesize*4
	sub	ebx,ecx
	shl	ebx,2
	mov	eax,ebx 		;eax = typesize*12
	imul	ebx,key
	add	esi,ebx 		;esi -> P1, R1, R4, P4

	sub	eax,4			;eax = "reset-value" for esi
@@l:
	fld	dword ptr [esi]
	fmul	st,st(4)		;P1
	add	esi,edx

	fld	dword ptr [esi]
	fmul	st,st(3)		;R1
	faddp	st(1),st
	add	esi,edx

	fld	dword ptr [esi]
	fmul	st,st(2)		;R4
	faddp	st(1),st
	add	esi,edx

	fld	dword ptr [esi]
	fmul	st,st(4)		;P4
	faddp	st(1),st

	fstp	dword ptr [edi]

	sub	esi,eax 		;reset control point pointer
	add	edi,4			;next target component

	dec	ecx
	jnz	@@l

	mov	ecx,5
@@clear:
	fstp	st
	dec	ecx
	jnz	@@clear

@@weg:
	mov	ebx,_track		;ebx -> ttrack
	mov	eax,key
	mov	t_key[ebx],eax		;write key back

	pop	esi
	ret

dotrack endp


slerp proc near
	;esi -> input quaternions (lie one after another)
	;edi -> resulting quaternions
	;st = t
	;edx = repetitions, for 3 it does this (qi:in, qr:result):
	      ;qr0 = slerp(qi0,qi1,t)
	      ;qr1 = slerp(qi1,qi2,t)
	      ;qr2 = slerp(qi2,qi3,t)

@@next:
		;calculate q1 * q2 = cos(th)
	mov	ecx,3
	fldz
@@skalar:
	fld	dword ptr [esi+ecx*4]
	fmul	dword ptr [esi+ecx*4+4*4]
	faddp	st(1),st
	dec	ecx
	jns	@@skalar
		;st = cos(th), st(1) = r


		;calculate sin(th) = sqrt(1-cos(th)^2)
	fld	st(0)
	fmul	st,st(0)
	fld1
	fsubrp	st(1),st		;st(0) = sin(th)^2 = 1-cos(th)^2
	fcom	c_sinmin

	fstsw	ax
	sahf
	jb	@@linear

	fsqrt				;st(0) = sin(th), st(1) = cos(th)

	fst	st(3)
	fxch	st(1)
	fpatan	;st(0) = th, st(1) = r, st(2) = sin(th)



		;calculate i1 = sin((1-r)*th)/sinth
	fld1
	fsub	st,st(2)		;st = 1-r
	fmul	st,st(1)		;*th
	fsin
	fdiv	st,st(3)		;/sinth

		;calculate i2 = sin(r*th)/sinth
	fld	st(2)			;st = r
	fmul	st,st(2)		;*th
	fsin
	fdiv	st,st(4)		;/sinth

	ffree	st(4)
	jmp	@@i

@@linear:	;st(2) = r
	fstp	st
		;i1 = 1-r
	fld1
	fsub	st,st(2)
		;i2 = r
	fld	st(2)

@@i:
	;q = q1*i1 + q2*i2
	mov	ecx,4
@@l:
	fld	dword ptr [esi]
	fmul	st,st(2)
	fld	dword ptr [esi+4*4]
	fmul	st,st(2)
	add	esi,4
	faddp	st(1),st
	fstp	dword ptr [edi]
	add	edi,4
	dec	ecx
	jnz	@@l

	mov	ecx,3
@@clear:
	fstp	st
	dec	ecx
	jnz	@@clear


	dec	edx
	jnz	@@next

@@weg:	ret

slerp endp


dorottrack proc pascal
arg	_track:dword
	;edi -> result
local	key:dword, q0:dword:4, q1:dword:4, q2:dword:4
	push	esi

	mov	ebx,_track		;ebx -> ttrack
	mov	eax,t_key[ebx]
	mov	key,eax
	mov	esi,t_data[ebx] 	;esi -> track-data


	mov	ecx,th_keys[esi]
	imul	ecx,size tkey - 4
	add	ecx,size ttrackh	;ecx + esi -> control points

	mov	eax,th_sframe[esi]	;eax = startframe
	cmp	th_mode[esi],0
	jne	@@loop

		;single
	mov	edx,frame		;edx = frame
	cmp	edx,eax
	jbe	@@c			;before track-start
	mov	eax,[esi+ecx-4] 	;eax = endframe
	cmp	edx,eax
	jb	@@key
					;behind track-end
      mov eax,3*4*4;3*typesize*4
      imul eax,th_keys[esi]
      add ecx,eax
;	 mov	 ecx,t_size[ebx]
;	 sub	 ecx,4*4		 ;4*typesize

@@c:	add	esi,ecx 		;ersten oder letzen kontrollpunkt
	jmp	@@notrack

@@loop:
		;loop & repeat
	mov	ebx,[esi+ecx-4]
	sub	ebx,eax 		;ebx = endframe - startframe

	neg	eax
	add	eax,frame		;eax = frame - startframe
	cdq
	idiv	ebx
	or	edx,edx
	jns	@@0
	add	edx,ebx
@@0:	add	edx,th_sframe[esi]

@@key:					;edx = frame

		;keypos = (site ttrackh) - 4 + key*(size tkey - 4)
	mov	ebx,size tkey - 4
	imul	ebx,key
	add	ebx,size ttrackh - 4
	add	ebx,esi

	cmp	edx,k_sframe[ebx]	;search actual key
	jae	@@1
	mov	key,0
	lea	ebx,[esi + size ttrackh - 4]
@@1:
	cmp	edx,k_eframe[ebx]
	jbe	@@2
	add	ebx,size tkey - 4
	inc	key
	jmp	@@1
@@2:


@@spline:
		;t
	mov	eax,k_sframe[ebx]
	sub	edx,eax 		;edx = (frame - sframe)
	mov	dword ptr [edi],edx	;temp
	fild	dword ptr [edi]
	neg	eax
	add	eax,k_eframe[ebx]	;eax = (eframe - sframe)
	mov	dword ptr [edi],eax	;temp
	fild	dword ptr [edi]
	fdivp	st(1),st		;t = (frame - sframe) / (eframe - sframe);


	add	esi,ecx 		;esi -> control points
	mov	ebx,key
	shl	ebx,2
	sub	ebx,key 		;ebx = key*4
	shl	ebx,4			;*16 (sizeof(quaternion) = 16)
	add	esi,ebx 		;esi -> qn, an, bn, qn1


	push	edi

	lea	edi,q0
	mov	edx,3			;3 repititions
	call	slerp			;q0 = slerp(qn,an,t);
					;q1 = slerp(an,bn1,t);
					;q2 = slerp(bn1,qn1,t);
	lea	esi,q0
	mov	edi,esi
	mov	edx,2			;2 repititions
	call	slerp			;q0 = slerp(q0,q1,t);
					;q1 = slerp(q1,q2,t);
	lea	esi,q0
	mov	edi,esi
	mov	edx,1			;1 repitition
	call	slerp			;q0 = slerp(q0,q1,t);

	fstp	st ;r

	pop	edi

	lea	esi,q0
@@notrack:				;frame out of track range
	fld	q_x[esi]
	fadd	st,st(0)
	fld	q_y[esi]
	fadd	st,st(0)
	fld	q_z[esi]
	fadd	st,st(0)
		;st(2) = 2x, st(1) = 2y, st = 2z

		;A11, A22, A33
	fld	q_x[esi]
	fmul	st,st(3)
	fld	q_y[esi]
	fmul	st,st(3)
	fld	q_z[esi]
	fmul	st,st(3)
		;st(2) = 2xx, st(1) = 2yy, st = 2zz

	fld1				;1
	fsub	st,st(2)		;-2yy
	fsub	st,st(1)		;-2zz
	fstp	A_11[edi]

	fld1				;1
	fsub	st,st(3)		;-2xx
	fsubrp	st(1)			;-2zz
	fstp	A_22[edi]

	fld1				;1
	fsubrp	st(1)			;-2yy
	fsubrp	st(1)			;-2xx
	fstp	A_33[edi]

		;A21, A12
	fld	q_x[esi]
	fmul	st,st(2)
	fld	q_w[esi]
	fmul	st,st(2)
		;st(1) = 2yx, st = 2zw

	fld	st(1)			;2xy
	fsub	st,st(1)		;-2wz
	fstp	A_21[edi]
	faddp	st(1)			;2xy+2wx
	fstp	A_12[edi]

		;A13, A31
	fld	q_z[esi]
	fmul	st,st(3)
	fld	q_w[esi]
	fmul	st,st(3)
		;st(1) = 2xz, st = 2yw

	fld	st(1)			;2xz
	fsub	st,st(1)		;-2yw
	fstp	A_13[edi]
	faddp	st(1)			;2xz+2yw
	fstp	A_31[edi]

		;A32, A23
	fld	q_w[esi]
	fmul	st,st(3)
	fld	q_z[esi]
	fmul	st,st(3)
		;st(1) = 2xw, st = 2yz

	fld	st(1)			;2xw
	fsubr	st,st(1)		;2yz-
	fstp	A_32[edi]
	faddp	st(1)			;2yz+2xw
	fstp	A_23[edi]

	fstp	st
	fstp	st
	fstp	st

@@weg:
	mov	ebx,_track		;ebx -> ttrack
	mov	eax,key
	mov	t_key[ebx],eax		;write key back

	pop	esi
	ret

dorottrack endp



doltrack proc pascal
arg	_track:dword, typesize:dword
	;edi -> result
local	temp

	push	esi

	mov	ebx,_track
@@r:
	mov	edx,frame
	cmp	edx,[ebx].lt_endframe
	jbe	@@i
		;end of key
	cmp	[ebx].lt_keys,0
	ja	@@next
		;end of track
	fld1
	jmp	@@e

@@next: 	;get next key
	mov	eax,[ebx].lt_endframe
	mov	[ebx].lt_startframe,eax ;old endframe is new startframe

	dec	[ebx].lt_keys

	mov	esi,[ebx].lt_data
	movsx	eax,byte ptr [esi]	;get scale value
	inc	esi
	mov	temp,eax
	fild	temp

	mov	edx,[ebx].lt_endstate
	xor	ecx,ecx
@@l:

	movsx	eax,byte ptr [esi+ecx]	;get differences
	mov	temp,eax
	fild	temp

	fscale

	mov	eax,[ebx].lt_startstate

	fld	d [edx+ecx*4]
	fadd	d [eax+ecx*4]
	fstp	d [eax+ecx*4]
	fstp	d [edx+ecx*4]


	inc	ecx
	cmp	ecx,typesize
	jb	@@l

	fstp	st

	mov	eax,[esi+ecx]		;get endframe
	mov	[ebx].lt_endframe,eax

	lea	eax,[esi+ecx+4] 	;update data pointer
	mov	[ebx].lt_data,eax
	jmp	@@r

@@i:		;interpolate		;edx = frame
	mov	eax,[ebx].lt_startframe
	sub	edx,eax 		;edx = (frame - sframe)
	mov	temp,edx
	fild	temp
	neg	eax
	add	eax,[ebx].lt_endframe	;eax = (eframe - sframe)
	mov	temp,eax
	fild	temp
	fdivp	st(1),st		;t = (frame - sframe) / (eframe - sframe);
@@e:
	mov	esi,[ebx].lt_startstate
	mov	edx,[ebx].lt_endstate

	xor	ecx,ecx
@@l2:
	fld	d [edx+ecx*4]
	fmul	st,st(1)
	fadd	d [esi+ecx*4]
	fstp	d [edi+ecx*4]

	inc	ecx
	cmp	ecx,typesize
	jb	@@l2

	fstp	st

	pop	esi

	ret
doltrack endp

dohidetrack proc near;pascal
	;ebx -> track

	push	esi

	mov	esi,[ebx]		;esi -> track

	mov	edx,[ebx+4]		;edx = key

@@l:	cmp	edx,[esi]		;end of track? (esi -> keys)
	jae	@@w

	mov	eax,[esi+4+edx*4]	;check switch-frame
	cmp	eax,frame
	ja	@@w			;not yet reached
	xor	d [edi],1		;switch hidden-flag
	inc	edx			;next key
	jmp	@@l
@@w:
	mov	[ebx+4],edx		;write key back

	pop	esi
	ret
dohidetrack endp


end