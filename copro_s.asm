.386
.model flat,prolog
locals

public finit
public sin
public cos
public arccos
public sqr
public sqrt
public fabs
public pow
public arctan

public fexp
public fscale

public cos2sin

public setrc
public ceil
public floor
public round


w equ word ptr
d equ dword ptr
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
.data
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
c_05 dd 0.499999
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
.code
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±

finit proc near
	finit
;	 push	 eax
	fldcw	w [esp+4]
;	 pop	 eax
	ret
finit endp

sin proc near
	fld	d [esp+4]
	fsin
;	 ret	 4
	fstp	d [esp+4]
	mov	eax,[esp+4]
	ret
endp

cos proc near
	fld	d [esp+4]
	fcos
;	 ret	 4
	fstp	d [esp+4]
	mov	eax,[esp+4]
	ret
endp

arccos proc near
	fld	d [esp+4]
	fld	st(0)
	fmul	st,st(0)
	fld1
	fsubrp	st(1),st		;st(0) = sinýà = 1-cosýà

	ftst
	fstsw	ax;sw
	;mov	 ax,sw
	sahf
	jae	@@0
	ffree	st(0)
	fincstp
	fldz
	jmp	@@1
@@0:
	fsqrt				;st(0) = sinà, st(1) = cosà
@@1:
	fxch	st(1)
	fpatan


;	 ret	 4
	fstp	d [esp+4]
	mov	eax,[esp+4]
	ret
endp


sqr proc near
	fld	d [esp+4]
	fmul	st,st(0)
;	 ret	 4
	fstp	d [esp+4]
	mov	eax,[esp+4]
	ret
endp

sqrt proc near
	fld	d [esp+4]
	fsqrt
;	 ret	 4
	fstp	d [esp+4]
	mov	eax,[esp+4]
	ret
endp

fabs proc near
	fld	d [esp+4]
	fabs
;	 ret	 4
	fstp	d [esp+4]
	mov	eax,[esp+4]
	ret
endp

pow proc near
	fld	d [esp+8]
	fld	d [esp+4]
	fyl2x
	fld	st
	frndint
	fxch	st(1)
	fsub	st,st(1)
	f2xm1
	fld1
	faddp	st(1),st
	fscale
	fstp	st(1)
;	 ret	 8
	fstp	d [esp+4]
	mov	eax,[esp+4]
	ret
endp

arctan proc near
	fld	d [esp+8]
	fld	d [esp+4]
	fpatan
;	 ret	 8
	fstp	d [esp+4]
	mov	eax,[esp+4]
	ret
arctan endp

fexp proc near
	fld	d [esp+4]
	fxtract
	fstp	st
	fistp	d [esp+4]
	mov	eax,[esp+4]

;	 ret	 4
	ret
fexp endp

fscale proc near
	fild	d [esp+8]
	fld	d [esp+4]
	fscale
	fstp	st(1)

;	 ret	 8
	fstp	d [esp+4]
	mov	eax,[esp+4]
	ret
fscale endp

cos2sin proc near
	fld	d [esp+4]
	fmul	st,st(0)
	fld1
	fsubrp	st(1)		     ;st(0) = sinýà = 1-cosýà

	ftst
	fstsw	ax
	sahf
	jae	@@0
	ffree	st(0)
	fldz
	jmp	@@1
@@0:
	fsqrt
@@1:
;	 ret	 4
	fstp	d [esp+4]
	mov	eax,[esp+4]
	ret
endp


setrc proc near

	push	eax
	fstcw	[esp]
	and	d [esp],0F3FFh
	shl	eax,10
	or	[esp],eax
	fldcw	[esp]
	pop	eax
	ret
endp

ceil proc near
	fld	d [esp+4]
	fadd	c_05
	fistp	d [esp+4]
	mov	eax,d [esp+4]
;	 ret	 4
	ret
endp

floor proc near
	fld	d [esp+4]
	fsub	c_05
	fistp	d [esp+4]
	mov	eax,d [esp+4]
;	 ret	 4
	ret
endp

round proc near
	fld	d [esp+4]
	frndint
;	 ret	 4
	fstp	d [esp+4]
	mov	eax,[esp+4]
	ret
endp

end

