;/*
.486

.model flat

LOCALS

.data

EXTRN	ppPolygons			:DWORD
EXTRN	pcBackBuffer		:DWORD
EXTRN	dwXmax				:DWORD
EXTRN	iPixelCounter		:DWORD




align 32
pcTexture					dd ?


pDot1						dd ?
pDot2						dd ?
pDot3						dd ?


fYCounter					dd ?
fYCounter1					dd ?
fYCounter2					dd ?


dwYCounter					dd ?
dwYCounter1					dd ?
dwYCounter2					dd ?

dwYOffset1					dd ?
dwYOffset2					dd ?

dwScreenX1					dd ?
dwScreenX2					dd ?
dwScreenX3					dd ?

dwDeltaScreenX				dd ?
dwDeltaScreenX1				dd ?
dwDeltaScreenX2				dd ?
dwDeltaScreenXf				dd ?
dwDeltaScreenX1f			dd ?
dwDeltaScreenX2f			dd ?

dwAdderScreenX1				dd ?
dwAdderScreenX1f			dd ?
dwAdderScreenX2				dd ?
dwAdderScreenX2f			dd ?

fMaxXCounterRZP				dd ?
dwMaxXCounter				dd ?

fTextureUDivZ1NSBC			dd ?			; NSBC = Not Subpixelcorrect
fTextureUDivZ2NSBC			dd ?
fTextureUDivZ3NSBC			dd ?
fTextureVDivZ1NSBC			dd ?			; NSBC = Not Subpixelcorrect
fTextureVDivZ2NSBC			dd ?
fTextureVDivZ3NSBC			dd ?


fTextureUDivZ1				dd ?
fTextureUDivZ3				dd ?
fTextureUDivZdY				dd ?
fTextureUDivZdY1			dd ?
fTextureUDivZdY2			dd ?

fTextureVDivZ1				dd ?
fTextureVDivZ3				dd ?
fTextureVDivZdY				dd ?
fTextureVDivZdY1			dd ?
fTextureVDivZdY2			dd ?

fOneDivZ1NSBC				dd ?
fOneDivZ2NSBC				dd ?
fOneDivZ3NSBC				dd ?

fOneDivZdY					dd ?
fOneDivZdY1					dd ?
fOneDivZdY2					dd ?

fOneDivZ1					dd ?
fOneDivZ2					dd ?
fOneDivZ3					dd ?

fOneDivZdX					dd ?
fOneDivZdXsl				dd ?

fScreenYError1				dd ?
fScreenYError2				dd ?
fScreenYError3				dd ?


fTextureVDivZdX				dd ?		; sl = SpanLength
fTextureUDivZdX				dd ?
fTextureVDivZdXsl			dd ?		; sl = SpanLength
fTextureUDivZdXsl			dd ?


dwTextureU					dd ?
dwTextureV					dd ?
dwTextureVdX				dd ?
dwTextureUdX				dd ?

fNum_2EXP20					dd 1048576.0
fNum_2EXP_20				dd 0.0000009536743164

fSpanLengthFactorsRZP		dd -1
							dd 1.0
							dd 0.5
							dd 0.3333333333
							dd 0.25
							dd 0.2
							dd 0.1666666666
							dd 0.142857142
							dd 0.125
							dd 0.1111111111
							dd 0.1
							dd 0.0909090909
							dd 0.0833333333
							dd 0.076923076
							dd 0.071428571
							dd 0.0666666666
							dd 0.0625

dwSpanLength				equ 16


;***> TODO: include the SpanLengthFactor in MaxXCounter
;***> TODO: Check the include of 2EXP20 in MaxXCounter

dwCurXCounter				dd ?
dwNextXCounter				dd ?
dwScanlineXCounter			dd ?


;***> TODO: eliminate old loop variables
dwRemnantPixel				dd ?
dwSpanCounter				dd ?


;YLoopVariable

dwOldESP					dd ?
dwScreenXf					dd ?


; Linear Mapper

fTextureU1					dd ?
fTextureU3					dd ?
fTextureUdY					dd ?
fTextureUdY1				dd ?
fTextureUdY2				dd ?

fTextureV1					dd ?
fTextureV3					dd ?
fTextureVdY					dd ?
fTextureVdY1				dd ?
fTextureVdY2				dd ?

fTextureVdX					dd ?
fTextureUdX					dd ?

; Texture Filtering

dwXCounter					dd ?
PerspFltJumpTableX			dd 8 dup (?)
PerspFltJumpTableY			dd 8 dup (?)
LinearFltJumpTableX			dd 8 dup (?)
LinearFltJumpTableY			dd 8 dup (?)
dwTemp						dd ?


fpucw						dw ?
oldfpucw					dw ?
oh_low 						dd ?
co_low						dd ?
co_hi						dd ?


.code

include m2structs.inc




public InitFltJumpTableTTrASM
InitFltJumpTableTTrASM proc
	mov		edx,offset PerspFltJumpTableX
	mov		dword ptr ds:[edx].00h,offset PerspFltCaseX0
	mov		dword ptr ds:[edx].04h,offset PerspFltCaseX1
	mov		dword ptr ds:[edx].08h,offset PerspFltCaseX2
	mov		dword ptr ds:[edx].0ch,offset PerspFltCaseX3
	mov		dword ptr ds:[edx].10h,offset PerspFltCaseX4
	mov		dword ptr ds:[edx].14h,offset PerspFltCaseX5
	mov		dword ptr ds:[edx].18h,offset PerspFltCaseX6
	mov		dword ptr ds:[edx].1ch,offset PerspFltCaseX7

	mov		edx,offset PerspFltJumpTableY
	mov		dword ptr ds:[edx].00h,offset PerspFltCaseY0
	mov		dword ptr ds:[edx].04h,offset PerspFltCaseY1
	mov		dword ptr ds:[edx].08h,offset PerspFltCaseY2
	mov		dword ptr ds:[edx].0ch,offset PerspFltCaseY3
	mov		dword ptr ds:[edx].10h,offset PerspFltCaseY4
	mov		dword ptr ds:[edx].14h,offset PerspFltCaseY5
	mov		dword ptr ds:[edx].18h,offset PerspFltCaseY6
	mov		dword ptr ds:[edx].1ch,offset PerspFltCaseY7

	mov		edx,offset LinearFltJumpTableX
	mov		dword ptr ds:[edx].00h,offset LinearFltCaseX0
	mov		dword ptr ds:[edx].04h,offset LinearFltCaseX1
	mov		dword ptr ds:[edx].08h,offset LinearFltCaseX2
	mov		dword ptr ds:[edx].0ch,offset LinearFltCaseX3
	mov		dword ptr ds:[edx].10h,offset LinearFltCaseX4
	mov		dword ptr ds:[edx].14h,offset LinearFltCaseX5
	mov		dword ptr ds:[edx].18h,offset LinearFltCaseX6
	mov		dword ptr ds:[edx].1ch,offset LinearFltCaseX7

	mov		edx,offset LinearFltJumpTableY
	mov		dword ptr ds:[edx].00h,offset LinearFltCaseY0
	mov		dword ptr ds:[edx].04h,offset LinearFltCaseY1
	mov		dword ptr ds:[edx].08h,offset LinearFltCaseY2
	mov		dword ptr ds:[edx].0ch,offset LinearFltCaseY3
	mov		dword ptr ds:[edx].10h,offset LinearFltCaseY4
	mov		dword ptr ds:[edx].14h,offset LinearFltCaseY5
	mov		dword ptr ds:[edx].18h,offset LinearFltCaseY6
	mov		dword ptr ds:[edx].1ch,offset LinearFltCaseY7

	ret
endp InitFltJumpTableTTrASM





public MV2DrawPolygonTfTrASM
MV2DrawPolygonTfTrASM proc
		; edi = pPolygon

	mov		eax,ds:[edi].CMV2Polygon__m_pDot1
	mov		ebx,ds:[edi].CMV2Polygon__m_pDot2
	mov		ecx,ds:[edi].CMV2Polygon__m_pDot3
		;eax - ecx: pDot3D1 - pDot3D3

	mov		pDot1,eax
	mov		pDot2,ebx
	mov		pDot3,ecx

	mov		edx,ds:[edi].CMV2Polygon__m_pcTexture
	mov		eax,ds:[eax].CMV2Dot3D__m_pPos
	shr		edx,2
	mov		ebx,ds:[ebx].CMV2Dot3D__m_pPos
	mov		ecx,ds:[ecx].CMV2Dot3D__m_pPos
		;eax - ecx: pDot3DPos1 - pDot3DPos3
	mov		pcTexture,edx

	mov		edx,ds:[eax].CMV2Dot3DPos__m_iScreenY
	mov		esi,ds:[ebx].CMV2Dot3DPos__m_iScreenY
	mov		edi,ds:[ecx].CMV2Dot3DPos__m_iScreenY
	mov		eax,pDot1
	mov	   	ebx,pDot2
	mov		ecx,pDot3

						; find dot with smallest y
	cmp		edx,esi
	jbe     @@Dot1YSmallerDot2Y		; dot1.y <= dot2.y

	cmp		esi,edi
	jbe		@@Dot2YSmallest		; dot2.y <(=) dot3.y&dot1.y
	jmp		@@Dot3YSmallest		; dot3.y <    dot2.y&dot1.y

@@Dot1YSmallerDot2Y:
	cmp		edx,edi
	jae		@@Dot3YSmallest		; dot3.y <    dot1.y&dot2.y

@@Dot1YSmallest:
	mov		edx,eax
	mov		esi,ebx
	mov		edi,ecx
	jmp		@@DotsSorted

@@Dot2YSmallest:
	mov		edx,ebx
	mov		esi,ecx
	mov		edi,eax
	jmp		@@DotsSorted

@@Dot3YSmallest:
	mov		edx,ecx
	mov		esi,eax
	mov		edi,ebx
	jmp		@@DotsSorted
@@DotsSorted:

	; edx = pDot1
	; esi = pDot2
	; edi = pDot3

	mov		pDot1,edx
	mov		pDot2,esi
	mov		pDot3,edi

	mov		edx,ds:[edx].CMV2Dot3D__m_pPos
	mov		esi,ds:[esi].CMV2Dot3D__m_pPos
	mov		edi,ds:[edi].CMV2Dot3D__m_pPos

	mov		eax,ds:[edx].CMV2Dot3DPos__m_iScreenY
	mov		ebx,ds:[esi].CMV2Dot3DPos__m_iScreenY
	mov		ecx,ds:[edi].CMV2Dot3DPos__m_iScreenY

	cmp		eax,ebx
	je		@@PolygonType4

	cmp		ebx,ecx
	je		@@PolygonType3
	jb		@@PolygonType2


;=============>                <==============
;=============> Polygon Type 1 <==============
;=============>                <==============
@@PolygonType1:
	; edx = pDot1
	; esi = pDot2
	; edi = pDot3

	fld		dword ptr ds:[edx].CMV2Dot3DPos__m_fScreenY
	fld		dword ptr ds:[esi].CMV2Dot3DPos__m_fScreenY
	fld		dword ptr ds:[edi].CMV2Dot3DPos__m_fScreenY
		;st0 = (y3), st1 = (y2), st2 = (y1)

	fld		st(1)
	fsub	st,st(1)
	fxch
		;st0 = (y3), st1 = (y2 - y3), st2 = (y2), st3 = (y1)

	fsub	st,st(3)
	fxch	st(2)
		;st0 = (y2), st1 = (y2 - y3), st2 = (y3 - y1), st3 = (y1)

	fsub	st,st(3)
	fxch    st(2)
		;st0 = (y3 - y1), st1 = (y2 - y3), st2 = (y2 - y1), st3 = (y1)

	fst		fYCounter1

	fld1
	fdivrp	st(3),st

	mov		eax,dwXmax
	mov		ebx,ds:[edx].CMV2Dot3DPos__m_iScreenY
	dec		ebx      		; subpixel/clipping reason
	imul    eax,ebx
	add		eax,pcBackBuffer
	mov		dwYOffset1,eax

	fld1
	fdivrp	st(2),st

	mov		eax,dwXmax
	mov		ebx,ds:[edi].CMV2Dot3DPos__m_iScreenY
	dec		ebx
	imul	eax,ebx
	add		eax,pcBackBuffer
	mov		dwYOffset2,eax

	fld1
	fdivrp	st(1),st

	mov		eax,ds:[edx].CMV2Dot3DPos__m_iScreenY
	mov		ebx,ds:[esi].CMV2Dot3DPos__m_iScreenY
	mov		ecx,ds:[edi].CMV2Dot3DPos__m_iScreenY

	mov		ebp,ebx
	sub		ebp,eax
	mov		dwYCounter,ebp
	mov		ebp,ecx
	sub		ebp,eax
	mov		dwYCounter1,ebp
	mov		ebp,ebx
	sub		ebp,ecx
	mov		dwYCounter2,ebp

	ffree	st(3)

	;st0 = 1/(y3 - y1), st1 = 1/(y2 - y3), st2 = 1/(y2 - y1)

;===>                                                                <===
;===>	Screen Delta Calculation {                                   <===
;===>                                                                <===
		;st0 = 1/YCounter1, st1 = 1/YCounter2, st2 = 1/YCounter
	fld		dword ptr ds:[edx].CMV2Dot3DPos__m_fScreenX
	fld		dword ptr ds:[esi].CMV2Dot3DPos__m_fScreenX
	fld		dword ptr ds:[edi].CMV2Dot3DPos__m_fScreenX
		;st0 = (x3), st1 = (x2), st2 = (x1)
		;st4 = 1/YCounter1, st5 = 1/YCounter2, st6 = 1/YCounter

	fld		st(1)
	fsub	st,st(1)
	fxch
	fsub	st,st(3)
	fxch
	fmul	st,st(5)
	fxch	st(2)
	fsub	st,st(3)
	fxch
	fmul	st,st(4)
	fxch	st(3)
	ffree   st
	fincstp
	fmul	st,st(5)
		;st0 = DeltaScreenX, st1 = DeltaScreenX2,
		;st2 = DeltaScreenX1, st3 = 1/YCounter1,
		;st4 = 1/YCounter2, st5 = 1/YCounter

	fld		dword ptr ds:[edx].CMV2Dot3DPos__m_fScreenYError
	fld		st
	fmul	st,st(4)
	fxch
	fmul	st,st(2)
	fxch
	fadd	dword ptr ds:[edx].CMV2Dot3DPos__m_fScreenX
	fxch
	fadd	dword ptr ds:[edx].CMV2Dot3DPos__m_fScreenX
	fxch
		;st0 = ScreenX1
		;st1 = ScreenX2
		;st2 = DeltaScreenX, st3 = DeltaScreenX2
		;st4 = DeltaScreenX1, st5 = 1/YCounter1
		;st6 = 1/YCounter2, st7 = 1/YCounter

	fistp	dwScreenX1

	fld		dword ptr ds:[edi].CMV2Dot3DPos__m_fScreenYError
	fmul	st,st(3)
	fxch
		;st0 = dwScreenX2
		;st1 = ScreenYError3*DeltaScreenX2
		;st2 = DeltaScreenX, st3 = DeltaScreenX2
		;st4 = DeltaScreenX1, st5 = 1/YCounter13
		;st6 = 1/YCounter32, st7 = 1/YCounter12

	fistp	dwScreenX2

	fadd	dword ptr ds:[edi].CMV2Dot3DPos__m_fScreenX
	fxch	st(3)
		;st0 = DeltaScreenX1
		;st1 = DeltaScreenX, st2 = DeltaScreenX2
		;st3 = ScreenYError3*DeltaScreenX3 + ScreenX3, st4 = 1/YCounter13
		;st5 = 1/YCounter32, st6 = 1/YCounter12

	fistp	dwDeltaScreenX1
	fxch
		;st0 = DeltaScreenX3, st1 = DeltaScreenX2
		;st2 = ScreenYError3*DeltaScreenX3 + ScreenX3, st3 = 1/YCounter13
		;st4 = 1/YCounter32, st5 = 1/YCounter12

	fistp	dwDeltaScreenX2
	fxch
		;st0 = ScreenYError3*DeltaScreenX3 + ScreenX3
		;st1 = DeltaScreenX2, st2 = 1/YCounter13
		;st3 = 1/YCounter32, st4 = 1/YCounter12

	fistp	dwScreenX3

;===>	max_x_counter calculation	{                                <===

		;st0 = DeltaScreenX
		;st1 = 1/YCounter13, st2 = 1/YCounter32, st3 = 1/YCounter12

	fld		fYCounter1
	fmul	st,st(1)
	fadd	dword ptr ds:[edx].CMV2Dot3DPos__m_fScreenX
	fsub	dword ptr ds:[edi].CMV2Dot3DPos__m_fScreenX             ;fp dep.
	fist	dwMaxXCounter
	fld1
	fdivrp	st(1),st
	fstp	fMaxXCounterRZP											;fp dep.
;===>	max_x_counter calculation	}                                <===

	fistp	dwDeltaScreenX

;===>                                                                <===
;===>	Screen Delta Calculation 	}                                <===
;===>                                                                <===
		;st0 = 1/YCounter1, st1 = 1/YCounter2, st2 = 1/YCounter

	mov		ebx,dwYCounter
	cmp		ebx,dwSpanLength
	jae		@@PerspPT1

	mov		eax,ds:[edx].CMV2Dot3DPos__m_iScreenX
	mov		ebx,ds:[esi].CMV2Dot3DPos__m_iScreenX
	mov		ebp,eax
	mov		ecx,ds:[edi].CMV2Dot3DPos__m_iScreenX

	sub		eax,ebx		; p1 - p2
	sub		ebx,ecx		; p2 - p3
	sub		ecx,ebp		; p3 - p1

	cmp		eax,0
	jns		@@PerspCheck1PT1
	neg		eax
@@PerspCheck1PT1:
	cmp		ebx,0
	jns		@@PerspCheck2PT1
	neg		ebx
@@PerspCheck2PT1:
	cmp		ecx,0
	jns		@@PerspCheck3PT1
	neg		ecx
@@PerspCheck3PT1:

	cmp		eax,dwSpanLength
	jae		@@PerspPT1

	cmp		ebx,dwSpanLength
	jae		@@PerspPT1

	cmp		ecx,dwSpanLength
	jae		@@PerspPT1

	jmp		@@LinearPolygonType1
@@PerspPT1:


	mov		eax,ds:[edx].CMV2Dot3DPos__m_fZNewRZP
	mov		ebx,ds:[esi].CMV2Dot3DPos__m_fZNewRZP
	mov		ecx,ds:[edi].CMV2Dot3DPos__m_fZNewRZP
	mov		fOneDivZ1NSBC,eax
	mov		fOneDivZ2NSBC,ebx
	mov		fOneDivZ3NSBC,ecx

	mov		eax,ds:[edx].CMV2Dot3DPos__m_fScreenYError
	mov		ebx,ds:[edi].CMV2Dot3DPos__m_fScreenYError

	mov		fScreenYError1,eax
	mov		fScreenYError3,ebx

	mov		edx,pDot1
	mov		esi,pDot2
	mov		edi,pDot3

;===>                                                                <===
;===>	TextureUDivZ DeltaY Calculation {                            <===
;===>                                                                <===
	;st0=1/YCounter1, st1=1/YCounter2, st2=1/YCounter

	fld		dword ptr ds:[edx].CMV2Dot3D__m_fTextureU
	fmul	fOneDivZ1NSBC
	fld		dword ptr ds:[esi].CMV2Dot3D__m_fTextureU
	fmul	fOneDivZ2NSBC
	fld		dword ptr ds:[edi].CMV2Dot3D__m_fTextureU
	fmul	fOneDivZ3NSBC

	fxch	st(2)
	fst		fTextureUDivZ1NSBC
	fxch	st(2)
	fst     fTextureUDivZ3NSBC

		;st0 = (tu3/z3), st1 = (tu2/z2), st2 = (tu1/z1)
		;st4 = 1/YCounter1, st5 = 1/YCounter2, st6 = 1/YCounter

	fld		st(1)
	fsub	st,st(1)
	fxch
		;st0 = (tu3/z3), st1 = (tu2/z2 - tu3/z3), st2 = (tu2/z2)
		;st3 = (tu1/z1)
		;st4 = 1/YCounter1, st5 = 1/YCounter2, st6 = 1/YCounter

	fsub	st,st(3)
	fxch
		;st0 = (tu2/z2 - tu3/z3), st1 = (tu3/z3 - tu1/z1)
		;st2 = (tu2/z2), st3 = (tu1/z1)
		;st4 = 1/YCounter1, st5 = 1/YCounter2, st6 = 1/YCounter

	fmul	st,st(5)
	fxch	st(2)
		;st0 = (tu2/z2), st1 = (tu3/z3 - tu1/z1)
		;st2 = (tu2/z2 - tu3/z3)/YCounter2, st3 = (tu1/z1)
		;st4 = 1/YCounter1, st5 = 1/YCounter2, st6 = 1/YCounter

	fsub	st,st(3)
	fxch
		;st0 = (tu3/z3 - tu1/z1), st1 = (tu2/z2 - tu1/z1)
		;st2 = (tu2/z2 - tu3/z3)/YCounter2,
		;st3 = (tu1/z1)
		;st4 = 1/YCounter1, st5 = 1/YCounter2, st6 = 1/YCounter

	fmul	st,st(4)
	fxch	st(3)
		;st0 = (tu1/z1), st1 = (tu2/z2 - tu1/z1),
		;st2 = (tu3/z3 - tu2/z2)/YCounter2
		;st3 = (tu3/z3 - tu1/z1)/YCounter1
		;st4 = 1/YCounter1, st5 = 1/YCounter2, st6 = 1/YCounter

	ffree   st
	fincstp
		;st0 = (tu2/z2 - tu1/z1), st1 = (tu3/z3 - tu2/z2)/YCounter2
		;st2 = (tu3/z3 - tu1/z1)/YCounter1
		;st3 = 1/YCounter1, st4 = 1/YCounter2, st5 = 1/YCounter

	fmul	st,st(5)
		;st0 = TextureUDivZdY, st1 = TextureUDivZdY2
		;st2 = TextureUDivZdY1, st3 = 1/YCounter1
		;st4 = 1/YCounter2, st5 = 1/YCounter

	fld		fScreenYError1
	fld		fScreenYError3
		;st0 = ScreenYError3, st1 = ScreenYError1
		;st2 = TextureUDivZdY, st3 = TextureUDivZdY2
		;st4 = TextureUDivZdY1, st5 = 1/YCounter1
		;st6 = 1/YCounter2, st7 = 1/YCounter

	fmul	st,st(3)
	fxch
		;st0 = ScreenYError1, st1 = ScreenYError3*TextureUDivZdY2
		;st2 = TextureUDivZdY, st3 = TextureUDivZdY2
		;st4 = TextureUDivZdY1, st5 = 1/YCounter1
		;st6 = 1/YCounter2, st7 = 1/YCounter

	fmul	st,st(4)
	fxch
		;st0 = ScreenYError3*TextureUDivZdY2
		;st1 = ScreenYError1*TextureUDivZdY1
		;st2 = TextureUDivZdY, st3 = TextureUDivZdY2
		;st4 = TextureUDivZdY1, st5 = 1/YCounter1
		;st6 = 1/YCounter2, st7 = 1/YCounter

	fadd	fTextureUDivZ3NSBC
	fxch
		;st0 = ScreenYError1*TextureUDivZdY1
		;st1 = ScreenYError3*TextureUDivZdY2 + TextureU3
		;st2 = TextureUDivZdY, st3 = TextureUDivZdY2
		;st4 = TextureUDivZdY1, st5 = 1/YCounter1
		;st6 = 1/YCounter2, st7 = 1/YCounter

	fadd	fTextureUDivZ1NSBC
	fxch
		;st0 = ScreenYError3*TextureUDivZdY2 + TextureU3
		;st1 = ScreenYError1*TextureUDivZdY1 + TextureU1
		;st2 = TextureUDivZdY, st3 = TextureUDivZdY2
		;st4 = TextureUDivZdY1, st5 = 1/YCounter1
		;st6 = 1/YCounter2, st7 = 1/YCounter

	fstp	fTextureUDivZ3
	fstp	fTextureUDivZ1
	fstp	fTextureUDivZdY
	fstp	fTextureUDivZdY2
	fstp	fTextureUDivZdY1
;===>                                                                <===
;===>	TextureUDivZ DeltaY Calculation }                            <===
;===>                                                                <===


;===>                                                                <===
;===>	TextureVDivZ DeltaY Calculation {                            <===
;===>                                                                <===
	;st0=1/YCounter1, st1=1/YCounter2, st2=1/YCounter

	fld		dword ptr ds:[edx].CMV2Dot3D__m_fTextureV
	fmul	fOneDivZ1NSBC
	fld		dword ptr ds:[esi].CMV2Dot3D__m_fTextureV
	fmul	fOneDivZ2NSBC
	fld		dword ptr ds:[edi].CMV2Dot3D__m_fTextureV
	fmul	fOneDivZ3NSBC

	fxch	st(2)
	fst		fTextureVDivZ1NSBC
	fxch	st(2)
	fst     fTextureVDivZ3NSBC

		;st0 = (tv3/z3), st1 = (tv2/z2), st2 = (tv1/z1)
		;st4 = 1/YCounter1, st5 = 1/YCounter2, st6 = 1/YCounter

	fld		st(1)
	fsub	st,st(1)
	fxch
		;st0 = (tv3/z3), st1 = (tv2/z2 - tv3/z3), st2 = (tv2/z2)
		;st3 = (tv1/z1)
		;st4 = 1/YCounter1, st5 = 1/YCounter2, st6 = 1/YCounter

	fsub	st,st(3)
	fxch
		;st0 = (tv2/z2 - tv3/z3), st1 = (tv3/z3 - tv1/z1)
		;st2 = (tv2/z2), st3 = (tv1/z1)
		;st4 = 1/YCounter1, st5 = 1/YCounter2, st6 = 1/YCounter

	fmul	st,st(5)
	fxch	st(2)
		;st0 = (tv2/z2), st1 = (tv3/z3 - tv1/z1)
		;st2 = (tv2/z2 - tv3/z3)/YCounter2, st3 = (tv1/z1)
		;st4 = 1/YCounter1, st5 = 1/YCounter2, st6 = 1/YCounter

	fsub	st,st(3)
	fxch
		;st0 = (tv3/z3 - tv1/z1), st1 = (tv2/z2 - tv1/z1)
		;st2 = (tv2/z2 - tv3/z3)/YCounter2,
		;st3 = (tv1/z1)
		;st4 = 1/YCounter1, st5 = 1/YCounter2, st6 = 1/YCounter

	fmul	st,st(4)
	fxch	st(3)
		;st0 = (tv1/z1), st1 = (tv2/z2 - tv1/z1),
		;st2 = (tv3/z3 - tv2/z2)/YCounter2
		;st3 = (tv3/z3 - tv1/z1)/YCounter1
		;st4 = 1/YCounter1, st5 = 1/YCounter2, st6 = 1/YCounter

	ffree   st
	fincstp
		;st0 = (tv2/z2 - tv1/z1), st1 = (tv3/z3 - tv2/z2)/YCounter2
		;st2 = (tv3/z3 - tv1/z1)/YCounter1
		;st3 = 1/YCounter1, st4 = 1/YCounter2, st5 = 1/YCounter

	fmul	st,st(5)
		;st0 = TextureVDivZdY, st1 = TextureVDivZdY2
		;st2 = TextureVDivZdY1, st3 = 1/YCounter1
		;st4 = 1/YCounter2, st5 = 1/YCounter

	fld		fScreenYError1
	fld		fScreenYError3
		;st0 = ScreenYError3, st1 = ScreenYError1
		;st2 = TextureVDivZdY, st3 = TextureVDivZdY2
		;st4 = TextureVDivZdY1, st5 = 1/YCounter1
		;st6 = 1/YCounter2, st7 = 1/YCounter

	fmul	st,st(3)
	fxch
		;st0 = ScreenYError1, st1 = ScreenYError3*TextureVDivZdY2
		;st2 = TextureVDivZdY, st3 = TextureVDivZdY2
		;st4 = TextureVDivZdY1, st5 = 1/YCounter1
		;st6 = 1/YCounter2, st7 = 1/YCounter

	fmul	st,st(4)
	fxch
		;st0 = ScreenYError3*TextureVDivZdY2
		;st1 = ScreenYError1*TextureVDivZdY1
		;st2 = TextureVDivZdY, st3 = TextureVDivZdY2
		;st4 = TextureVDivZdY1, st5 = 1/YCounter1
		;st6 = 1/YCounter2, st7 = 1/YCounter

	fadd    fTextureVDivZ3NSBC
	fxch
		;st0 = ScreenYError1*TextureVDivZdY1
		;st1 = ScreenYError3*TextureVDivZdY2 + TextureV3
		;st2 = TextureVDivZdY, st3 = TextureVDivZdY2
		;st4 = TextureVDivZdY1, st5 = 1/YCounter1
		;st6 = 1/YCounter2, st7 = 1/YCounter


	fadd    fTextureVDivZ1NSBC
	fxch
		;st0 = ScreenYError3*TextureVDivZdY2 + TextureV3
		;st1 = ScreenYError1*TextureVDivZdY1 + TextureV1
		;st2 = TextureVDivZdY, st3 = TextureVDivZdY2
		;st4 = TextureVDivZdY1, st5 = 1/YCounter1
		;st6 = 1/YCounter2, st7 = 1/YCounter

	fstp	fTextureVDivZ3
	fstp	fTextureVDivZ1
	fstp	fTextureVDivZdY
	fstp	fTextureVDivZdY2
	fstp	fTextureVDivZdY1

;===>                                                                <===
;===>	TextureVDivZ DeltaY Calculation }                            <===
;===>                                                                <===



;===>                                                                <===
;===>	OneDivZ DeltaY Calculation {                                 <===
;===>                                                                <===
	;st0=1/YCounter1, st1=1/YCounter2, st2=1/YCounter

	fld		fOneDivZ1NSBC
	fld		fOneDivZ2NSBC
	fld     fOneDivZ3NSBC
		;st0 = (1/z3), st1 = (1/z2), st2 = (1/z1)
		;st4 = 1/YCounter1, st5 = 1/YCounter2, st6 = 1/YCounter

	fld		st(1)
	fsub	st,st(1)
	fxch
		;st0 = (1/z3), st1 = (1/z2 - 1/z3), st2 = (1/z2)
		;st3 = (1/z1)
		;st4 = 1/YCounter1, st5 = 1/YCounter2, st6 = 1/YCounter

	fsub	st,st(3)
	fxch
		;st0 = (1/z2 - 1/z3), st1 = (1/z3 - 1/z1)
		;st2 = (1/z2), st3 = (1/z1)
		;st4 = 1/YCounter1, st5 = 1/YCounter2, st6 = 1/YCounter

	fmul	st,st(5)
	fxch	st(2)
		;st0 = (1/z2), st1 = (1/z3 - 1/z1)
		;st2 = (1/z2 - 1/z3)/YCounter2, st3 = (1/z1)
		;st4 = 1/YCounter1, st5 = 1/YCounter2, st6 = 1/YCounter

	fsub	st,st(3)
	fxch
		;st0 = (1/z3 - 1/z1), st1 = (1/z2 - 1/z1)
		;st2 = (1/z2 - 1/z3)/YCounter2,
		;st3 = (1/z1)
		;st4 = 1/YCounter1, st5 = 1/YCounter2, st6 = 1/YCounter

	fmul	st,st(4)
	fxch	st(3)
		;st0 = (1/z1), st1 = (1/z2 - 1/z1),
		;st2 = (1/z3 - 1/z2)/YCounter2
		;st3 = (1/z3 - 1/z1)/YCounter1
		;st4 = 1/YCounter1, st5 = 1/YCounter2, st6 = 1/YCounter

	ffree   st
	fincstp
		;st0 = (1/z2 - 1/z1), st1 = (1/z3 - 1/z2)/YCounter2
		;st2 = (1/z3 - 1/z1)/YCounter1
		;st3 = 1/YCounter1, st4 = 1/YCounter2, st5 = 1/YCounter

	fmul	st,st(5)
		;st0 = OneDivZdY, st1 = OneDivZdY2
		;st2 = OneDivZdY1, st3 = 1/YCounter1
		;st4 = 1/YCounter2, st5 = 1/YCounter

	fld		fScreenYError1
	fld		fScreenYError3
		;st0 = ScreenYError3, st1 = ScreenYError1
		;st2 = OneDivZdY, st3 = OneDivZdY2
		;st4 = OneDivZdY1, st5 = 1/YCounter1
		;st6 = 1/YCounter2, st7 = 1/YCounter

	fmul	st,st(3)
	fxch
		;st0 = ScreenYError1, st1 = ScreenYError3*OneDivZdY2
		;st2 = OneDivZdY, st3 = OneDivZdY2
		;st4 = OneDivZdY1, st5 = 1/YCounter1
		;st6 = 1/YCounter2, st7 = 1/YCounter

	fmul	st,st(4)
	fxch
		;st0 = ScreenYError3*OneDivZdY2
		;st1 = ScreenYError1*OneDivZdY1
		;st2 = OneDivZdY, st3 = OneDivZdY2
		;st4 = OneDivZdY1, st5 = 1/YCounter1
		;st6 = 1/YCounter2, st7 = 1/YCounter

	fadd	fOneDivZ3NSBC
	fxch
		;st0 = ScreenYError1*OneDivZdY1
		;st1 = ScreenYError3*OneDivZdY2 + OneDivZ3
		;st2 = OneDivZdY, st3 = OneDivZdY2
		;st4 = OneDivZdY1, st5 = 1/YCounter1
		;st6 = 1/YCounter2, st7 = 1/YCounter

	fadd	fOneDivZ1NSBC
	fxch
		;st0 = ScreenYError3*OneDivZdY2 + OneDivZ3
		;st1 = ScreenYError1*OneDivZdY1 + OneDivZ1
		;st2 = OneDivZdY, st3 = OneDivZdY2
		;st4 = OneDivZdY1, st5 = 1/YCounter1
		;st6 = 1/YCounter2, st7 = 1/YCounter

	fstp	fOneDivZ3
	fstp	fOneDivZ1
	fstp	fOneDivZdY
	fstp	fOneDivZdY2
	fstp	fOneDivZdY1

;===>                                                                <===
;===>	OneDivZ DeltaY Calculation }                                 <===
;===>                                                                <===



;===>                                                                <===
;===>	TextureU&VDivZ DeltaX Calculation {                          <===
;===>                                                                <===

	fld		fMaxXCounterRZP
	fmul	fNum_2EXP20
	fld		fYCounter1
	fld		fTextureVDivZdY
	fld		fTextureUDivZdY
		;st0 = TextureUDivZdY, st1 = TextureVDivZdY
		;st2 = YCounter1, st3 = 1/MaxXCounter
		;st4 = 1/YCounter1, st5 = 1/YCounter2, st6 = 1/YCounter

	fmul	st,st(2)
	fxch
		;st0 = TextureVDivZDY, st1 = YCounter1*TextureUDivZDY
		;st2 = YCounter1, st3 = 1/MaxXCounter
		;st4 = 1/YCounter1, st5 = 1/YCounter2, st6 = 1/YCounter

	fmulp	st(2),st			; fmul stall (+ 1 Cycle)
	fxch
		;st0 = YCounter1*TextureVDivZDY, st1 = YCounter1*TextureUDivZDY
		;st2 = 1/MaxXCounter
		;st3 = 1/YCounter1, st4 = 1/YCounter2, st5 = 1/YCounter

	fadd	fTextureVDivZ1NSBC
	fxch
		;st0 = YCounter1*TextureUDivZDY
		;st1 = YCounter1*TextureVDivZDY + TextureVDivZ1
		;st2 = 1/MaxXCounter
		;st3 = 1/YCounter1, st4 = 1/YCounter2, st5 = 1/YCounter

	fadd	fTextureUDivZ1NSBC
	fxch
		;st0 = YCounter1*TextureVDivZDY + TextureVDivZ1
		;st1 = YCounter1*TextureUDivZDY + TextureUDivZ1
		;st2 = 1/MaxXCounter
		;st3 = 1/YCounter1, st4 = 1/YCounter2, st5 = 1/YCounter

	fsub	fTextureVDivZ3NSBC
	fxch
		;st0 = YCounter1*TextureUDivZDY + TextureUDivZ1
		;st1 = YCounter1*TextureVDivZDY + TextureVDivZ1 - TextureVDivZ3
		;st2 = 1/MaxXCounter
		;st3 = 1/YCounter1, st4 = 1/YCounter2, st5 = 1/YCounter

	fsub	fTextureUDivZ3NSBC
	fxch
		;st0 = YCounter1*TextureVDivZDY + TextureVDivZ1 - TextureVDivZ3
		;st1 = YCounter1*TextureUDivZDY + TextureUDivZ1 - TextureUDivZ3
		;st2 = 1/MaxXCounter
		;st3 = 1/YCounter1, st4 = 1/YCounter2, st5 = 1/YCounter

	fmul    st,st(2)
	fxch
		;st0 = YCounter1*TextureUDivZDY + TextureUDivZ1 - TextureUDivZ3
		;st1 = TextureVDivZdX
		;st2 = 1/MaxXCounter
		;st3 = 1/YCounter1, st4 = 1/YCounter2, st5 = 1/YCounter

	fmul	st,st(2)
	fxch
		;st0 = TextureVDivZdX
		;st1 = TextureUDivZdX
		;st2 = 1/MaxXCounter
		;st3 = 1/YCounter1, st4 = 1/YCounter2, st5 = 1/YCounter
	fstp	fTextureVDivZdX
	fstp	fTextureUDivZdX
;===>                                                                <===
;===>	TextureU&VDivZ DeltaX Calculation }                          <===
;===>                                                                <===



;===>                                                                <===
;===>	OneDivZ DeltaX Calculation {                          		 <===
;===>                                                                <===
		;st0 = 1/MaxXCounter
		;st1 = 1/YCounter1, st2 = 1/YCounter2, st3 = 1/YCounter

	fld		fOneDivZ1NSBC
	fsub	fOneDivZ3NSBC
		;st0 = OneDivZ1 - OneDivZ3
		;st1 = 1/MaxXCounter
		;st2 = 1/YCounter1, st3 = 1/YCounter2, st4 = 1/YCounter

	fld		fYCounter1
	fmul    fOneDivZdY
		;st0 = YCounter1*OneDivZdY, st1 = OneDivZ1 - OneDivZ3
		;st2 = 1/MaxXCounter
		;st3 = 1/YCounter1, st4 = 1/YCounter2, st5 = 1/YCounter

	faddp	st(1),st
		;st0 = YCounter1*OneDivZdY + OneDivZ1 - OneDivZ3
		;st1 = 1/MaxXCounter
		;st2 = 1/YCounter1, st3 = 1/YCounter2, st4 = 1/YCounter

	fmulp	st(1),st
		;st1 = OneDivZdX
		;st2 = 1/YCounter1, st3 = 1/YCounter2, st4 = 1/YCounter

	fstp	fOneDivZdX
;===>                                                                <===
;===>	OneDivZ DeltaX Calculation }                          		 <===
;===>                                                                <===

	ffree	st(2)
	ffree	st(1)
	ffree	st(0)

	mov		eax,dwDeltaScreenX1
	mov		ebx,dwDeltaScreenX
	mov		esi,eax
	mov		edi,ebx
	shl		eax,12
	shl		ebx,12
	sar		esi,20
	sar		edi,20
	add		esi,dwXmax
	add		edi,dwXmax
	mov		dwAdderScreenX1,esi
	mov		dwAdderScreenX2,edi
	mov		dwAdderScreenX1f,eax
	mov		dwAdderScreenX2f,ebx

	mov		eax,dwScreenX1
	mov		ebx,dwScreenX2
	mov		esi,eax
	mov		edi,ebx
	shl		eax,12
	shl		ebx,12
	sar		esi,20
	sar		edi,20
	add		esi,dwYOffset1
	add     edi,dwYOffset1

	mov		ebp,dwYCounter1
	mov		ecx,edi

	mov		dwOldESP,esp
	and		esp,0ffffffe0h

	fld		fNum_2EXP_20
	fld     fNum_2EXP20
	fld		fTextureVDivZ1
	fld		fTextureUDivZ1
	fld		fOneDivZ1

@@PerspPT1ScanlinerTLoopPass1:
				; eax = x1f (20 bit fraction)
				; ebx = x2f (20 bit fraction)
				; ecx = XCounter (init. BackBuffer + YOffset + X2)
				; edx =
				; esi = BackBuffer + YOffset + X1
				; edi = BackBuffer + YOffset + X2
				; ebp = YCounter

				; st0 = OneDivZ
				; st1 = TextureUDivZ
				; st2 = TextureVDivZ
				; st3 = 2^20
				; st4 = 2^-20

;***> TODO: Pipeline the fpu better
	shr		eax,12
	sub		esp,CMV2ScanlinerPerspT__size

	mov		dwScreenXf,eax
	sub		ecx,esi					; XCounter
	shl		eax,12

	fild	dwScreenXf
	fsubr	st,st(4)
		; st0 = 2^20 - ScreenXf, st1 = OneDivZ
		; st2 = TextureUDivZ, st3 = TextureVDivZ,
		; st4 = 2^20, st5 = 2^-20

	fmul	st,st(5)
		; st0 = ScreenXError

	fld		fTextureUDivZdX
	fmul	st,st(1)
	fld		fTextureVDivZdX
	fmul	st,st(2)
	fxch	st(2)
	fmul	fOneDivZdX
	fxch	st(2)
		; st0 = TextureVDivZdX*ScreenXError
		; st1 = TextureUDivZdX*ScreenXError
		; st2 = OneDivZdX*ScreenXError, st3 = OneDivZ
		; st4 = TextureUDivZ, st5 = TextureVDivZ,
		; st6 = 2^20, st7 = 2^-20

	fadd   	st,st(5)
	fxch
	fadd	st,st(4)
	fxch	st(2)
	fadd	st,st(3)
	fxch	st(2)
		; st0 = TextureUDivZ*ScreenXError + TextureUDivZ
		; st1 = TextureVDivZ*ScreenXError + TextureVDivZ
		; st2 = OneDivZ*ScreenXError + OneDivZ
		; st3 = OneDivZ
		; st4 = TextureUDivZ, st5 = TextureVDivZ,
		; st6 = 2^20, st7 = 2^-20

	fstp	DWORD PTR ss:[esp].CMV2ScanlinerPerspT__fTextureUDivZ
	fstp	DWORD PTR ss:[esp].CMV2ScanlinerPerspT__fTextureVDivZ
	fstp	DWORD PTR ss:[esp].CMV2ScanlinerPerspT__fOneDivZ
		; st0 = OneDivZ, st1 = TextureUDivZ, st2 = TextureVDivZ
		; st3 = 2^20, st4 = 2^-20

	mov		ss:[esp].CMV2ScanlinerPerspT__dwScreenX1Offset,esi
	mov		ss:[esp].CMV2ScanlinerPerspT__dwXCounter,ecx

	fadd	fOneDivZdY1
	fxch
	fadd	fTextureUDivZdY1
	fxch
	fxch	st(2)
	fadd	fTextureVDivZdY1
	fxch	st(2)

	add		ebx,dwAdderScreenX2f
	adc     edi,dwAdderScreenX2
	add		eax,dwAdderScreenX1f
	adc		esi,dwAdderScreenX1
	mov		ecx,edi

	dec		ebp
	jnz		@@PerspPT1ScanlinerTLoopPass1


	fcompp
	fcomp

;***> TODO: paire the instructions..

	mov		eax,dwDeltaScreenX2
	mov		esi,eax
	shl		eax,12
	sar		esi,20
	add		esi,dwXmax
	mov		dwAdderScreenX1,esi
	mov		dwAdderScreenX1f,eax

	mov		eax,dwScreenX3
	mov		esi,eax
	shl		eax,12
	sar		esi,20
	add		esi,dwYOffset2

	mov		ebp,dwYCounter2

	fld		fTextureVDivZ3
	fld		fTextureUDivZ3
	fld		fOneDivZ3

@@PerspPT1ScanlinerTLoopPass2:
				; eax = x1f (20 bit fraction)
				; ebx = x2f (20 bit fraction)
				; ecx = XCounter (init. BackBuffer + YOffset + X2)
				; edx =
				; esi = BackBuffer + YOffset + X1
				; edi = BackBuffer + YOffset + X2
				; ebp = YCounter

				; st0 = OneDivZ
				; st1 = TextureUDivZ
				; st2 = TextureVDivZ
				; st3 = 2^20
				; st4 = 2^-20

;***> TODO: Pipeline the fpu better
	shr		eax,12
	sub		esp,CMV2ScanlinerPerspT__size

	mov		dwScreenXf,eax
	sub		ecx,esi					; XCounter
	shl		eax,12

	fild	dwScreenXf
	fsubr	st,st(4)
		; st0 = 2^20 - ScreenXf, st1 = OneDivZ
		; st2 = TextureUDivZ, st3 = TextureVDivZ,
		; st4 = 2^20, st5 = 2^-20

	fmul	st,st(5)
		; st0 = ScreenXError

	fld		fTextureUDivZdX
	fmul	st,st(1)
	fld		fTextureVDivZdX
	fmul	st,st(2)
	fxch	st(2)
	fmul	fOneDivZdX
	fxch	st(2)
		; st0 = TextureVDivZdX*ScreenXError
		; st1 = TextureUDivZdX*ScreenXError
		; st2 = OneDivZdX*ScreenXError, st3 = OneDivZ
		; st4 = TextureUDivZ, st5 = TextureVDivZ,
		; st6 = 2^20, st7 = 2^-20

	fadd   	st,st(5)
	fxch
	fadd	st,st(4)
	fxch	st(2)
	fadd	st,st(3)
	fxch	st(2)
		; st0 = TextureUDivZ*ScreenXError + TextureUDivZ
		; st1 = TextureVDivZ*ScreenXError + TextureVDivZ
		; st2 = OneDivZ*ScreenXError + OneDivZ
		; st3 = OneDivZ
		; st4 = TextureUDivZ, st5 = TextureVDivZ,
		; st6 = 2^20, st7 = 2^-20

	fstp	DWORD PTR ss:[esp].CMV2ScanlinerPerspT__fTextureUDivZ
	fstp	DWORD PTR ss:[esp].CMV2ScanlinerPerspT__fTextureVDivZ
	fstp	DWORD PTR ss:[esp].CMV2ScanlinerPerspT__fOneDivZ
		; st0 = OneDivZ, st1 = TextureUDivZ, st2 = TextureVDivZ
		; st3 = 2^20, st4 = 2^-20

	mov		ss:[esp].CMV2ScanlinerPerspT__dwScreenX1Offset,esi
	mov		ss:[esp].CMV2ScanlinerPerspT__dwXCounter,ecx

	fadd	fOneDivZdY2
	fxch
	fadd	fTextureUDivZdY2
	fxch
	fxch	st(2)
	fadd	fTextureVDivZdY2
	fxch	st(2)

	add		ebx,dwAdderScreenX2f
	adc     edi,dwAdderScreenX2
	add		eax,dwAdderScreenX1f
	adc		esi,dwAdderScreenX1
	mov		ecx,edi

	dec		ebp
	jnz		@@PerspPT1ScanlinerTLoopPass2


	ffree	st(4)
	ffree	st(3)
	ffree	st(2)
	ffree	st(1)
	ffree	st(0)

	jmp		@@PerspTYLoop

;=============>                <==============
;=============> Polygon Type 2 <==============
;=============>                <==============
@@PolygonType2:
	; edx = pDot1
	; esi = pDot2
	; edi = pDot3
	fld		dword ptr ds:[edx].CMV2Dot3DPos__m_fScreenY
	fld		dword ptr ds:[esi].CMV2Dot3DPos__m_fScreenY
	fld		dword ptr ds:[edi].CMV2Dot3DPos__m_fScreenY
		;st0 = (y3), st1 = (y2), st2 = (y1)

	fld		st
	fsub	st,st(2)
	fxch
		;st0 = (y3), st1 = (y3 - y2), st2 = (y2), st3 = (y1)

	fsub	st,st(3)
	fxch	st(2)
		;st0 = (y2), st1 = (y3 - y2), st2 = (y3 - y1), st3 = (y1)

	fsub	st,st(3)
	fst		fYCounter1
	fxch    st(2)
		;st0 = (y3 - y1), st1 = (y3 - y2), st2 = (y2 - y1), st3 = (y1)

	fld1
	fdivrp	st(3),st

	mov		eax,dwXmax
	mov		ebx,ds:[edx].CMV2Dot3DPos__m_iScreenY
	dec		ebx
	imul    eax,ebx
	add		eax,pcBackBuffer
	mov		dwYOffset1,eax

	fld1
	fdivrp	st(2),st

	mov		eax,dwXmax
	mov		ebx,ds:[esi].CMV2Dot3DPos__m_iScreenY
	dec		ebx
	imul	eax,ebx
	add		eax,pcBackBuffer
	mov		dwYOffset2,eax

	fld1
	fdivrp	st(1),st

	mov		eax,ds:[edx].CMV2Dot3DPos__m_iScreenY
	mov		ebx,ds:[esi].CMV2Dot3DPos__m_iScreenY
	mov		ecx,ds:[edi].CMV2Dot3DPos__m_iScreenY

	mov		ebp,ebx
	sub		ebp,eax
	mov		dwYCounter1,ebp
	mov		ebp,ecx
	sub		ebp,eax
	mov		dwYCounter,ebp
	mov		ebp,ecx
	sub		ebp,ebx
	mov		dwYCounter2,ebp

	ffree	st(3)

		;st0 = 1/YCounter, st1 = 1/YCounter2, st2 = 1/YCounter1
;===>                                                                <===
;===>	Screen Delta Calculation {                                   <===
;===>                                                                <===
	fld		dword ptr ds:[edx].CMV2Dot3DPos__m_fScreenX
	fld		dword ptr ds:[esi].CMV2Dot3DPos__m_fScreenX
	fld		dword ptr ds:[edi].CMV2Dot3DPos__m_fScreenX
		;st0 = (x3), st1 = (x2), st2 = (x1)
		;st3 = 1/YCounter, st4 = 1/YCounter2, st5 = 1/YCounter1

	fld		st
	fsub	st,st(2)
	fxch
	fsub	st,st(3)
	fxch
	fmul	st,st(5)
	fxch	st(2)
	fsub	st,st(3)
	fxch
	fmul	st,st(4)
	fxch	st(3)
	ffree   st
	fincstp
	fmul	st,st(5)
		;st0 = DeltaScreenX1, st1 = DeltaScreenX2
		;st2 = DeltaScreenX
		;st3 = 1/YCounter, st4 = 1/YCounter2, st5 = 1/YCounter1

	fld		dword ptr ds:[edx].CMV2Dot3DPos__m_fScreenYError
	fld		st
	fmul	st,st(2)
	fxch
	fmul	st,st(4)
	fxch
	fadd	dword ptr ds:[edx].CMV2Dot3DPos__m_fScreenX
	fxch
	fadd	dword ptr ds:[edx].CMV2Dot3DPos__m_fScreenX
	fxch
		;st0 = ScreenYError1*DeltaScreenX1 + ScreenX1
		;st1 = ScreenYError1*DeltaScreenX + ScreenX1
		;st2 = DeltaScreenX1, st3 = DeltaScreenX2
		;st4 = DeltaScreenX, st3 = 1/YCounter,
		;st4 = 1/YCounter2, st5 = 1/YCounter1
	fistp	dwScreenX2

	fld		dword ptr ds:[esi].CMV2Dot3DPos__m_fScreenYError
	fmul	st,st(3)
	fxch
	fistp	dwScreenX1
	fadd	dword ptr ds:[esi].CMV2Dot3DPos__m_fScreenX
	fxch
	fistp	dwDeltaScreenX1
	fxch
	fistp	dwDeltaScreenX2
	fistp	dwScreenX3

;===>	MaxXCounter calculation	{        	                         <===
		;st0 = DeltaScreenX
		;st1 = 1/YCounter, st2 = 1/YCounter2, st3 = 1/YCounter1
	fld		dword ptr ds:[esi].CMV2Dot3DPos__m_fScreenX
	fld		fYCounter1
	fmul	st,st(2)
	fadd	dword ptr ds:[edx].CMV2Dot3DPos__m_fScreenX
	fsubp	st(1),st
	fist	dwMaxXCounter
	fld1
	fdivrp	st(1),st
		;st0=1/MaxXCounter, st1 = fDeltascreenX
		;st2=1/y_counter13, st3=1/y_counter12, st4=1/y_counter32
	fstp	fMaxXCounterRZP											;fp dep.

;===>	MaxXCounter calculation	}        	                         <===
	fistp	dwDeltaScreenX

;===>                                                                <===
;===>	Screen Delta Calculation }                                   <===
;===>                                                                <===
		;st0 = 1/YCounter, st1 = 1/YCounter2, st2 = 1/YCounter1

	mov		ebx,dwYCounter
	cmp		ebx,dwSpanLength
	jae		@@PerspPT2

	mov		eax,ds:[edx].CMV2Dot3DPos__m_iScreenX
	mov		ebx,ds:[esi].CMV2Dot3DPos__m_iScreenX
	mov		ebp,eax
	mov		ecx,ds:[edi].CMV2Dot3DPos__m_iScreenX

	sub		eax,ebx		; p1 - p2
	sub		ebx,ecx		; p2 - p3
	sub		ecx,ebp		; p3 - p1

	cmp		eax,0
	jns		@@PerspCheck1PT2
	neg		eax
@@PerspCheck1PT2:
	cmp		ebx,0
	jns		@@PerspCheck2PT2
	neg		ebx
@@PerspCheck2PT2:
	cmp		ecx,0
	jns		@@PerspCheck3PT2
	neg		ecx
@@PerspCheck3PT2:

	cmp		eax,dwSpanLength
	jae		@@PerspPT2

	cmp		ebx,dwSpanLength
	jae		@@PerspPT2

	cmp		ecx,dwSpanLength
	jae		@@PerspPT2

	jmp		@@LinearPolygonType2
@@PerspPT2:



	mov		eax,ds:[edx].CMV2Dot3DPos__m_fZNewRZP
	mov		ebx,ds:[esi].CMV2Dot3DPos__m_fZNewRZP
	mov		ecx,ds:[edi].CMV2Dot3DPos__m_fZNewRZP
	mov		fOneDivZ1NSBC,eax
	mov		fOneDivZ2NSBC,ebx
	mov		fOneDivZ3NSBC,ecx

	mov		eax,ds:[edx].CMV2Dot3DPos__m_fScreenYError
	mov		ebx,ds:[esi].CMV2Dot3DPos__m_fScreenYError
	mov     fScreenYError1,eax
	mov		fScreenYError2,ebx

	mov		edx,pDot1
	mov		esi,pDot2
	mov		edi,pDot3


;===>                                                                <===
;===>	TextureU&VDivZ DeltaY Calculation {                          <===
;===>                                                                <===
		;st0 = 1/YCounter, st1 = 1/YCounter2, st2 = 1/YCounter1

	fld		dword ptr ds:[edx].CMV2Dot3D__m_fTextureU
	fmul	fOneDivZ1NSBC
	fld		dword ptr ds:[edi].CMV2Dot3D__m_fTextureU
	fmul	fOneDivZ3NSBC
	fxch
			;st0 = tu1/z1, st1 = tu3/z3
	fst		fTextureUDivZ1NSBC
	fsubp	st(1),st
		;st0 = (tu3/z3 - tu1/z1)
		;st1 = 1/YCounter, st2 = 1/YCounter2, st3 = 1/YCounter1

	fld		dword ptr ds:[edx].CMV2Dot3D__m_fTextureV
	fmul	fOneDivZ1NSBC
	fld		dword ptr ds:[edi].CMV2Dot3D__m_fTextureV
	fmul	fOneDivZ3NSBC
	fxch
			;st0 = tv1/z1, st1 = vu3/z3
	fst		fTextureVDivZ1NSBC
	fsubp	st(1),st
	fxch
		;st0 = (tu3/z3 - tu1/z1), st1 = (tv3/z3 - tv1/z1)
		;st2 = 1/YCounter, st3 = 1/YCounter2, st4 = 1/YCounter1

	fmul	st,st(2)
	fxch
	fmul	st,st(2)      				; fmul stall (+1 Cycle)
	fxch
	fld		fScreenYError1
	fld		st
	fmul	st,st(2)
	fxch
	fmul	st,st(3)                    ; fmul stall (+1 Cycle)
	fxch
		;st0 = ScreenYError1*TextureUDivZdY
		;st1 = ScreenYError1*TextureVDivZdY
		;st2 = TextureUDivZdY, st3 = TextureVDivZdY
		;st4 = 1/YCounter, st5 = 1/YCounter2, st6 = 1/YCounter1

	fadd    fTextureUDivZ1NSBC
	fxch
	fadd	fTextureVDivZ1NSBC
	fxch
	fstp	fTextureUDivZ1
	fstp	fTextureVDivZ1
	fstp	fTextureUDivZdY
	fstp	fTextureVDivZdY

;===>                                                                <===
;===>	TextureU&VDivZ DeltaY Calculation }                          <===
;===>                                                                <===


;===>                                                                <===
;===>	OneDivZ DeltaY Calculation {                            	 <===
;===>                                                                <===
;***> TODO: Optimize

	fld		fOneDivZ3NSBC
	fsub	fOneDivZ1NSBC
		;st0 = (1/z3 - 1/z1)
		;st1 = 1/YCounter, st2 = 1/YCounter2, st3 = 1/YCounter1

	fmul	st,st(1)
		;st0 = OneDivZdY

	fld     fScreenYError1
	fmul    st,st(1)
		;st0 = ScreenYError1*OneDivZdY
		;st1 = OneDivZdY
	fadd	fOneDivZ1NSBC
	fxch
		;st0 = OneDivZdY
		;st1 = OneDivZ1

	fstp	fOneDivZdY
	fstp	fOneDivZ1
;===>                                                                <===
;===>	OneDivZ DeltaY Calculation }                            	 <===
;===>                                                                <===
		;st0 = 1/YCounter, st1 = 1/YCounter2, st2 = 1/YCounter1

;===>                                                                <===
;===>	TextureU&VDivZ DeltaX Calculation {                          <===
;===>                                                                <===

	fld		fOneDivZ2NSBC
	fld		dword ptr ds:[esi].CMV2Dot3D__m_fTextureU
	fmul    st,st(1)
	fxch
	fmul	dword ptr ds:[esi].CMV2Dot3D__m_fTextureV
	fxch
	fstp	fTextureUDivZ2NSBC
	fstp	fTextureVDivZ2NSBC

	fld		fMaxXCounterRZP
	fmul	fNum_2EXP20
	fld		fYCounter1
	fld		fTextureVDivZdY
	fld		fTextureUDivZdY
		;st0 = TextureUDivZdY, st1 = TextureVDivZdY
		;st2 = YCounter1, st3 = 1/MaxXCounter
		;st4 = 1/YCounter, st5 = 1/YCounter2, st6 = 1/YCounter1

	fmul	st,st(2)
	fxch
	fmulp	st(2),st			; fmul stall (+ 1 Cycle)
	fadd    fTextureUDivZ1NSBC
	fxch
	fadd    fTextureVDivZ1NSBC
	fxch
	fsubr	fTextureUDivZ2NSBC
	fxch
	fsubr	fTextureVDivZ2NSBC
	fxch
	fmul    st,st(2)
	fxch
	fmulp	st(2),st
		;st0 = fTextureUDivZdX
		;st1 = fTextureVDivZdX
		;st2 = 1/MaxXCounter
		;st3 = 1/YCounter, st4 = 1/YCounter2, st5 = 1/YCounter1

	fstp	fTextureUDivZdX
	fstp	fTextureVDivZdX
;===>                                                                <===
;===>	TextureU&VDivZ DeltaX Calculation }                          <===
;===>                                                                <===


;===>                                                                <===
;===>	OneDivZ DeltaX Calculation {                          		 <===
;===>                                                                <===
;***> TODO: Optimize

	fld		fMaxXCounterRZP
	fmul	fNum_2EXP20
	fld		fYCounter1
	fmul	fOneDivZdY
		;st0 = OneDivZ1dY*YCounter1
		;st1 = 1/MaxXCounter
		;st2 = 1/YCounter, st3 = 1/YCounter2, st4 = 1/YCounter1

	fadd	fOneDivZ1NSBC
	fsubr	fOneDivZ2NSBC
	fmulp	st(1),st
		;st0 = fOneDivZdX
		;st2 = 1/YCounter, st3 = 1/YCounter2, st4 = 1/YCounter1

	fstp	fOneDivZdX
;===>                                                                <===
;===>	OneDivZ DeltaX Calculation }                          		 <===
;===>                                                                <===
	ffree	st(2)
	ffree	st(1)
	ffree	st(0)

	mov		eax,dwDeltaScreenX1
	mov		ebx,dwDeltaScreenX
	mov		esi,eax
	mov		edi,ebx
	shl		eax,12
	shl		ebx,12
	sar		esi,20
	sar		edi,20
	add		esi,dwXmax
	add		edi,dwXmax
	mov		dwAdderScreenX2,esi
	mov		dwAdderScreenX1,edi
	mov		dwAdderScreenX2f,eax
	mov		dwAdderScreenX1f,ebx

	mov		eax,dwScreenX1
	mov		ebx,dwScreenX2
	mov		esi,eax
	mov		edi,ebx
	shl		eax,12
	shl		ebx,12
	sar		esi,20
	sar		edi,20
	add		esi,dwYOffset1
	add     edi,dwYOffset1

	mov		ebp,dwYCounter1
	mov		ecx,edi

	mov		dwOldESP,esp
	and		esp,0ffffffe0h

	fld		fNum_2EXP_20
	fld     fNum_2EXP20
	fld		fTextureVDivZ1
	fld		fTextureUDivZ1
	fld		fOneDivZ1

@@PerspPT2ScanlinerTLoopPass1:
				; eax = x1f (20 bit fraction)
				; ebx = x2f (20 bit fraction)
				; ecx = XCounter (init. BackBuffer + YOffset + X2)
				; edx =
				; esi = BackBuffer + YOffset + X1
				; edi = BackBuffer + YOffset + X2
				; ebp = YCounter

				; st0 = OneDivZ
				; st1 = TextureUDivZ
				; st2 = TextureVDivZ
				; st3 = 2^20
				; st4 = 2^-20

;***> TODO: Pipeline the fpu better
	shr		eax,12
	sub		esp,CMV2ScanlinerPerspT__size

	mov		dwScreenXf,eax
	sub		ecx,esi					; XCounter
	shl		eax,12

	fild	dwScreenXf
	fsubr	st,st(4)
		; st0 = 2^20 - ScreenXf, st1 = OneDivZ
		; st2 = TextureUDivZ, st3 = TextureVDivZ,
		; st4 = 2^20, st5 = 2^-20

	fmul	st,st(5)
		; st0 = ScreenXError

	fld		fTextureUDivZdX
	fmul	st,st(1)
	fld		fTextureVDivZdX
	fmul	st,st(2)
	fxch	st(2)
	fmul	fOneDivZdX
	fxch	st(2)
		; st0 = TextureVDivZdX*ScreenXError
		; st1 = TextureUDivZdX*ScreenXError
		; st2 = OneDivZdX*ScreenXError, st3 = OneDivZ
		; st4 = TextureUDivZ, st5 = TextureVDivZ,
		; st6 = 2^20, st7 = 2^-20

	fadd   	st,st(5)
	fxch
	fadd	st,st(4)
	fxch	st(2)
	fadd	st,st(3)
	fxch	st(2)
		; st0 = TextureUDivZ*ScreenXError + TextureUDivZ
		; st1 = TextureVDivZ*ScreenXError + TextureVDivZ
		; st2 = OneDivZ*ScreenXError + OneDivZ
		; st3 = OneDivZ
		; st4 = TextureUDivZ, st5 = TextureVDivZ,
		; st6 = 2^20, st7 = 2^-20

	fstp	DWORD PTR ss:[esp].CMV2ScanlinerPerspT__fTextureUDivZ
	fstp	DWORD PTR ss:[esp].CMV2ScanlinerPerspT__fTextureVDivZ
	fstp	DWORD PTR ss:[esp].CMV2ScanlinerPerspT__fOneDivZ
		; st0 = OneDivZ, st1 = TextureUDivZ, st2 = TextureVDivZ
		; st3 = 2^20, st4 = 2^-20

	mov		ss:[esp].CMV2ScanlinerPerspT__dwScreenX1Offset,esi
	mov		ss:[esp].CMV2ScanlinerPerspT__dwXCounter,ecx

	fadd	fOneDivZdY
	fxch
	fadd	fTextureUDivZdY
	fxch
	fxch	st(2)
	fadd	fTextureVDivZdY
	fxch	st(2)

	add		ebx,dwAdderScreenX2f
	adc     edi,dwAdderScreenX2
	add		eax,dwAdderScreenX1f
	adc		esi,dwAdderScreenX1
	mov		ecx,edi

	dec		ebp
	jnz		@@PerspPT2ScanlinerTLoopPass1

;***> TODO: paire the instructions..
	mov		ebx,dwDeltaScreenX2
	mov		edi,ebx
	shl		ebx,12
	sar		edi,20
	add		edi,dwXmax
	mov		dwAdderScreenX2,edi
	mov		dwAdderScreenX2f,ebx

	mov		ebx,dwScreenX3
	mov		edi,ebx
	shl		ebx,12
	sar		edi,20
	add		edi,dwYOffset2

	mov		ebp,dwYCounter2
	mov		ecx,edi

@@PerspPT2ScanlinerTLoopPass2:
				; eax = x1f (20 bit fraction)
				; ebx = x2f (20 bit fraction)
				; ecx = XCounter (init. BackBuffer + YOffset + X2)
				; edx =
				; esi = BackBuffer + YOffset + X1
				; edi = BackBuffer + YOffset + X2
				; ebp = YCounter

				; st0 = OneDivZ
				; st1 = TextureUDivZ
				; st2 = TextureVDivZ
				; st3 = 2^20
				; st4 = 2^-20

;***> TODO: Pipeline the fpu better
	shr		eax,12
	sub		esp,CMV2ScanlinerPerspT__size

	mov		dwScreenXf,eax
	sub		ecx,esi					; XCounter
	shl		eax,12

	fild	dwScreenXf
	fsubr	st,st(4)
		; st0 = 2^20 - ScreenXf, st1 = OneDivZ
		; st2 = TextureUDivZ, st3 = TextureVDivZ,
		; st4 = 2^20, st5 = 2^-20

	fmul	st,st(5)
		; st0 = ScreenXError

	fld		fTextureUDivZdX
	fmul	st,st(1)
	fld		fTextureVDivZdX
	fmul	st,st(2)
	fxch	st(2)
	fmul	fOneDivZdX
	fxch	st(2)
		; st0 = TextureVDivZdX*ScreenXError
		; st1 = TextureUDivZdX*ScreenXError
		; st2 = OneDivZdX*ScreenXError, st3 = OneDivZ
		; st4 = TextureUDivZ, st5 = TextureVDivZ,
		; st6 = 2^20, st7 = 2^-20

	fadd   	st,st(5)
	fxch
	fadd	st,st(4)
	fxch	st(2)
	fadd	st,st(3)
	fxch	st(2)
		; st0 = TextureUDivZ*ScreenXError + TextureUDivZ
		; st1 = TextureVDivZ*ScreenXError + TextureVDivZ
		; st2 = OneDivZ*ScreenXError + OneDivZ
		; st3 = OneDivZ
		; st4 = TextureUDivZ, st5 = TextureVDivZ,
		; st6 = 2^20, st7 = 2^-20

	fstp	DWORD PTR ss:[esp].CMV2ScanlinerPerspT__fTextureUDivZ
	fstp	DWORD PTR ss:[esp].CMV2ScanlinerPerspT__fTextureVDivZ
	fstp	DWORD PTR ss:[esp].CMV2ScanlinerPerspT__fOneDivZ
		; st0 = OneDivZ, st1 = TextureUDivZ, st2 = TextureVDivZ
		; st3 = 2^20, st4 = 2^-20

	mov		ss:[esp].CMV2ScanlinerPerspT__dwScreenX1Offset,esi
	mov		ss:[esp].CMV2ScanlinerPerspT__dwXCounter,ecx

	fadd	fOneDivZdY
	fxch
	fadd	fTextureUDivZdY
	fxch
	fxch	st(2)
	fadd	fTextureVDivZdY
	fxch	st(2)

	add		ebx,dwAdderScreenX2f
	adc     edi,dwAdderScreenX2
	add		eax,dwAdderScreenX1f
	adc		esi,dwAdderScreenX1
	mov		ecx,edi

	dec		ebp
	jnz		@@PerspPT2ScanlinerTLoopPass2

	ffree	st(4)
	ffree	st(3)
	ffree	st(2)
	ffree	st(1)
	ffree	st(0)

	jmp		@@PerspTYLoop


;=============>                <==============
;=============> Polygon Type 3 <==============
;=============>                <==============
@@PolygonType3:
	; edx = pDot1
	; esi = pDot2
	; edi = pDot3

	fld		dword ptr ds:[edx].CMV2Dot3DPos__m_fScreenY
	fld		dword ptr ds:[edi].CMV2Dot3DPos__m_fScreenY
	fld		dword ptr ds:[esi].CMV2Dot3DPos__m_fScreenY
		;st0 = (y2), st1 = (y3), st2 = (y1)

	fsub	st,st(2)
	fxch
		;st0 = (y3), st1 = (y2 - y1), st2 = (y1)

	fsubrp	st(2),st
		;st0 = (y2 - y1), st1 = (y3 - y1)
	fst		fYCounter2

	fld1
	fld		st
	fdivrp	st(2),st
		;st0 = 1.0, st1 = (y2 - y1), st2 = 1/(y3 - y1)

	mov		eax,dwXmax
	mov		ebx,ds:[edx].CMV2Dot3DPos__m_iScreenY
	dec		ebx
	imul    eax,ebx
	add		eax,pcBackBuffer
	mov		dwYOffset1,eax

	fdivrp	st(2),st
		;st0 = 1/(y2 - y1), st1 = 1/(y3 - y1)

	mov		eax,ds:[edi].CMV2Dot3DPos__m_iScreenY
	sub		eax,ds:[edx].CMV2Dot3DPos__m_iScreenY
	mov		dwYCounter,eax

;===>                                                                <===
;===>	Screen Delta Calculation {                                   <===
;===>                                                                <===
		;st0 = 1/YCounter2, st1 = 1/YCounter1

	fld		dword ptr ds:[edx].CMV2Dot3DPos__m_fScreenX
	fld		dword ptr ds:[edi].CMV2Dot3DPos__m_fScreenX
	fld		dword ptr ds:[esi].CMV2Dot3DPos__m_fScreenX
		;st0 = (x2), st1 = (x3), st2 = (x1)
		;st3 = 1/YCounter2, st4 = 1/YCounter1

	fsub	st,st(2)
	fxch
	fsubrp	st(2),st
	fmul	st,st(2)
	fxch
	fmul	st,st(3)				; fmul stall (+1 Cycle)
	fxch
		;st0 = DeltaScreenX2, st1 = DeltaScreenX1
		;st2 = 1/YCounter2, st3 = 1/YCounter1

	fld		dword ptr ds:[edx].CMV2Dot3DPos__m_fScreenYError
	fld		st
	fmul	st,st(3)
	fxch
	fmul	st,st(2)
	fxch
	fadd	dword ptr ds:[edx].CMV2Dot3DPos__m_fScreenX
	fxch
	fadd	dword ptr ds:[edx].CMV2Dot3DPos__m_fScreenX
	fxch
		;st0 = ScreenYError1*DeltaScreenX1 + ScreenX1
		;st1 = ScreenYError1*DeltaScreenX2 + ScreenX1
		;st2 = DeltaScreenX2, st3 = DeltaScreenX1
		;st4 = 1/YCounter2, st5 = 1/YCounter1

	fistp	dwScreenX1
	fistp	dwScreenX2
	fistp	dwDeltaScreenX2
		;st0 = DeltaScreenX1, st0 = 1/YCounter2, st1 = 1/YCounter1


;===>	MaxXCounter calculation	{        	                         <===
	fld		fYCounter2
	fmul	st,st(1)
	fadd	dword ptr ds:[edx].CMV2Dot3DPos__m_fScreenX
	fsubr	dword ptr ds:[esi].CMV2Dot3DPos__m_fScreenX             ;fp dep.
	fist	dwMaxXCounter
	fld1
	fdivrp	st(1),st
		;st0 = 1/MaxXCounter, st1 = DeltascreenX1
		;st2 = 1/YCounter2, st3 = 1/YCounter1
	fstp	fMaxXCounterRZP											;fp dep.
;===>	MaxXCounter calculation	}        	                         <===

	fistp	dwDeltaScreenX1
;===>                                                                <===
;===>	Screen Delta Calculation }                                   <===
;===>                                                                <===
		;st0 = 1/YCounter2, st1 = 1/YCounter1

	mov		ebx,dwYCounter
	cmp		ebx,dwSpanLength
	jae		@@PerspPT3

	mov		eax,ds:[edx].CMV2Dot3DPos__m_iScreenX
	mov		ebx,ds:[esi].CMV2Dot3DPos__m_iScreenX
	mov		ebp,eax
	mov		ecx,ds:[edi].CMV2Dot3DPos__m_iScreenX

	sub		eax,ebx		; p1 - p2
	sub		ebx,ecx		; p2 - p3
	sub		ecx,ebp		; p3 - p1

	cmp		eax,0
	jns		@@PerspCheck1PT3
	neg		eax
@@PerspCheck1PT3:
	cmp		ebx,0
	jns		@@PerspCheck2PT3
	neg		ebx
@@PerspCheck2PT3:
	cmp		ecx,0
	jns		@@PerspCheck3PT3
	neg		ecx
@@PerspCheck3PT3:

	cmp		eax,dwSpanLength
	jae		@@PerspPT3

	cmp		ebx,dwSpanLength
	jae		@@PerspPT3

	cmp		ecx,dwSpanLength
	jae		@@PerspPT3

	jmp		@@LinearPolygonType3
@@PerspPT3:



	mov		eax,ds:[edx].CMV2Dot3DPos__m_fZNewRZP
	mov		ebx,ds:[esi].CMV2Dot3DPos__m_fZNewRZP
	mov		ecx,ds:[edi].CMV2Dot3DPos__m_fZNewRZP
	mov		fOneDivZ1NSBC,eax
	mov		fOneDivZ2NSBC,ebx
	mov		fOneDivZ3NSBC,ecx

	mov		eax,ds:[edx].CMV2Dot3DPos__m_fScreenYError
	mov		ebx,ds:[esi].CMV2Dot3DPos__m_fScreenYError
	mov     fScreenYError1,eax
	mov		fScreenYError2,ebx

	mov		edx,pDot1
	mov		esi,pDot2
	mov		edi,pDot3


;===>                                                                <===
;===>	TextureU&VDivZ DeltaY Calculation {                          <===
;===>                                                                <===
		;st0 = 1/YCounter2, st1 = 1/YCounter1

	fld		dword ptr ds:[edx].CMV2Dot3D__m_fTextureU
	fmul	fOneDivZ1NSBC
	fld		dword ptr ds:[edi].CMV2Dot3D__m_fTextureU
	fmul	fOneDivZ3NSBC
	fxch
			;st0 = tu1/z1, st1 = tu3/z3
	fst		fTextureUDivZ1NSBC
	fsubp	st(1),st
		;st0 = (tu3/z3 - tu1/z1)
		;st1 = 1/YCounter2, st2 = 1/YCounter1

	fld		dword ptr ds:[edx].CMV2Dot3D__m_fTextureV
	fmul	fOneDivZ1NSBC
	fld		dword ptr ds:[edi].CMV2Dot3D__m_fTextureV
	fmul	fOneDivZ3NSBC
	fxch
			;st0 = tv1/z1, st1 = tu3/z3
	fst		fTextureVDivZ1NSBC
	fsubp	st(1),st
	fxch
		;st0 = (tu3/z3 - tu1/z1), st1 = (tv3/z3 - tv1/z1)
		;st2 = 1/YCounter2, st3 = 1/YCounter1

	fmul	st,st(3)
	fxch
	fmul	st,st(3)      				; fmul stall (+1 Cycle)
	fxch
		;st0 = TextureUDivZdY1, st1 = TextureVDivZdY1
		;st2 = 1/YCounter2, st3 = 1/YCounter1

	fld		fScreenYError1
	fld		st
	fmul	st,st(2)
	fxch
	fmul	st,st(3)                    ; fmul stall (+1 Cycle)
	fxch
	fadd	fTextureUDivZ1NSBC
	fxch
	fadd	fTextureVDivZ1NSBC
	fxch
		;st0 = ScreenYError1*TextureUDivZdY1 + TextureUDivZ1
		;st1 = ScreenYError1*TextureVDivZdY1 + TextureVDivZ1
		;st2 = TextureUDivZdY1, st3 = TextureVDivZdY1
		;st4 = 1/YCounter2, st5 = 1/YCounter1

	fstp	fTextureUDivZ1
	fstp	fTextureVDivZ1
	fstp	fTextureUDivZdY1
	fstp	fTextureVDivZdY1
;===>                                                                <===
;===>	TextureU&VDivZ DeltaY Calculation }                          <===
;===>                                                                <===




;===>                                                                <===
;===>	OneDivZ DeltaY Calculation {                            	 <===
;===>                                                                <===
;***> TODO: Optimize

	fld		fOneDivZ3NSBC
	fsub	fOneDivZ1NSBC
		;st0 = (1/z3 - 1/z1)
		;st1 = 1/YCounter2, st2 = 1/YCounter1

	fmul	st,st(2)
		;st0 = OneDivZdY

	fld     fScreenYError1
	fmul    st,st(1)
		;st0 = ScreenYError1*OneDivZdY
		;st1 = OneDivZdY
	fadd	fOneDivZ1NSBC
	fxch
		;st0 = OneDivZdY
		;st1 = OneDivZ1

	fstp	fOneDivZdY1
	fstp	fOneDivZ1
;===>                                                                <===
;===>	OneDivZ DeltaY Calculation }                            	 <===
;===>                                                                <===




;===>                                                                <===
;===>	TextureU&VDivZ DeltaX Calculation {                          <===
;===>                                                                <===
	fld		fOneDivZ2NSBC
	fld		dword ptr ds:[esi].CMV2Dot3D__m_fTextureU
	fmul    st,st(1)
	fxch
	fmul	dword ptr ds:[esi].CMV2Dot3D__m_fTextureV
	fxch
	fstp	fTextureUDivZ2NSBC
	fstp	fTextureVDivZ2NSBC

	fld		fMaxXCounterRZP
	fmul	fNum_2EXP20
	fld		fYCounter2
	fld		fTextureVDivZdY1
	fld		fTextureUDivZdY1
		;st0 = TextureUDivZdY1, st1 = TextureVDivZdY1
		;st2 = YCounter2, st3 = 1/MaxXCounter
		;st4 = 1/YCounter2, st5 = 1/YCounter1

	fmul	st,st(2)
	fxch
	fmulp	st(2),st			; fmul stall (+ 1 Cycle)
	fadd    fTextureUDivZ1NSBC
	fxch
	fadd    fTextureVDivZ1NSBC
	fxch
	fsubr	fTextureUDivZ2NSBC
	fxch
	fsubr	fTextureVDivZ2NSBC
	fxch
	fmul    st,st(2)
	fxch
	fmulp	st(2),st
		;st0 = fTextureUDivZdX
		;st1 = fTextureVDivZdX
		;st2 = 1/MaxXCounter
		;st3 = 1/YCounter2, st4 = 1/YCounter1

	fstp	fTextureUDivZdX
	fstp	fTextureVDivZdX
;===>                                                                <===
;===>	TextureU&VDivZ DeltaX Calculation }                          <===
;===>                                                                <===


;===>                                                                <===
;===>	OneDivZ DeltaX Calculation {                          		 <===
;===>                                                                <===
;***> TODO: Optimize

	fld		fMaxXCounterRZP
	fmul	fNum_2EXP20
	fld		fYCounter2
	fmul	fOneDivZdY1
		;st0 = OneDivZ1dY1*YCounter2
		;st1 = 1/MaxXCounter
		;st2 = 1/YCounter2, st3 = 1/YCounter1

	fadd	fOneDivZ1NSBC
	fsubr	fOneDivZ2NSBC
	fmulp	st(1),st
		;st0 = fOneDivZdX
		;st2 = 1/YCounter2, st3 = 1/YCounter1

	fstp	fOneDivZdX
;===>                                                                <===
;===>	OneDivZ DeltaX Calculation }                          		 <===
;===>                                                                <===

	ffree	st(1)
	ffree	st(0)

	mov		eax,dwDeltaScreenX1
	mov		ebx,dwDeltaScreenX2
	mov		esi,eax
	mov		edi,ebx
	shl		eax,12
	shl		ebx,12
	sar		esi,20
	sar		edi,20
	add		esi,dwXmax
	add		edi,dwXmax
	mov		dwAdderScreenX1,esi
	mov		dwAdderScreenX2,edi
	mov		dwAdderScreenX1f,eax
	mov		dwAdderScreenX2f,ebx

	mov		eax,dwScreenX1
	mov		ebx,dwScreenX2
	mov		esi,eax
	mov		edi,ebx
	shl		eax,12
	shl		ebx,12
	sar		esi,20
	sar		edi,20
	add		esi,dwYOffset1
	add     edi,dwYOffset1

	mov		ebp,dwYCounter
	mov		ecx,edi

	mov		dwOldESP,esp
	and		esp,0ffffffe0h

	fld		fNum_2EXP_20
	fld     fNum_2EXP20
	fld		fTextureVDivZ1
	fld		fTextureUDivZ1
	fld		fOneDivZ1

@@PerspPT3ScanlinerTLoopPass1:
				; eax = x1f (20 bit fraction)
				; ebx = x2f (20 bit fraction)
				; ecx = XCounter (init. BackBuffer + YOffset + X2)
				; edx =
				; esi = BackBuffer + YOffset + X1
				; edi = BackBuffer + YOffset + X2
				; ebp = YCounter

				; st0 = OneDivZ
				; st1 = TextureUDivZ
				; st2 = TextureVDivZ
				; st3 = 2^20
				; st4 = 2^-20

;***> TODO: Pipeline the fpu better
	shr		eax,12
	sub		esp,CMV2ScanlinerPerspT__size

	mov		dwScreenXf,eax
	sub		ecx,esi					; XCounter
	shl		eax,12

	fild	dwScreenXf
	fsubr	st,st(4)
		; st0 = 2^20 - ScreenXf, st1 = OneDivZ
		; st2 = TextureUDivZ, st3 = TextureVDivZ,
		; st4 = 2^20, st5 = 2^-20

	fmul	st,st(5)
		; st0 = ScreenXError

	fld		fTextureUDivZdX
	fmul	st,st(1)
	fld		fTextureVDivZdX
	fmul	st,st(2)
	fxch	st(2)
	fmul	fOneDivZdX
	fxch	st(2)
		; st0 = TextureVDivZdX*ScreenXError
		; st1 = TextureUDivZdX*ScreenXError
		; st2 = OneDivZdX*ScreenXError, st3 = OneDivZ
		; st4 = TextureUDivZ, st5 = TextureVDivZ,
		; st6 = 2^20, st7 = 2^-20

	fadd   	st,st(5)
	fxch
	fadd	st,st(4)
	fxch	st(2)
	fadd	st,st(3)
	fxch	st(2)
		; st0 = TextureUDivZ*ScreenXError + TextureUDivZ
		; st1 = TextureVDivZ*ScreenXError + TextureVDivZ
		; st2 = OneDivZ*ScreenXError + OneDivZ
		; st3 = OneDivZ
		; st4 = TextureUDivZ, st5 = TextureVDivZ,
		; st6 = 2^20, st7 = 2^-20

	fstp	DWORD PTR ss:[esp].CMV2ScanlinerPerspT__fTextureUDivZ
	fstp	DWORD PTR ss:[esp].CMV2ScanlinerPerspT__fTextureVDivZ
	fstp	DWORD PTR ss:[esp].CMV2ScanlinerPerspT__fOneDivZ
		; st0 = OneDivZ, st1 = TextureUDivZ, st2 = TextureVDivZ
		; st3 = 2^20, st4 = 2^-20

	mov		ss:[esp].CMV2ScanlinerPerspT__dwScreenX1Offset,esi
	mov		ss:[esp].CMV2ScanlinerPerspT__dwXCounter,ecx

	fadd	fOneDivZdY1
	fxch
	fadd	fTextureUDivZdY1
	fxch
	fxch	st(2)
	fadd	fTextureVDivZdY1
	fxch	st(2)

	add		ebx,dwAdderScreenX2f
	adc     edi,dwAdderScreenX2
	add		eax,dwAdderScreenX1f
	adc		esi,dwAdderScreenX1
	mov		ecx,edi

	dec		ebp
	jnz		@@PerspPT3ScanlinerTLoopPass1

	ffree	st(4)
	ffree	st(3)
	ffree	st(2)
	ffree	st(1)
	ffree	st(0)

	jmp		@@PerspTYLoop


;=============>                <==============
;=============> Polygon Type 4 <==============
;=============>                <==============
@@PolygonType4:
	; edx = pDot1
	; esi = pDot2
	; edi = pDot3

	mov		eax,ds:[edi].CMV2Dot3DPos__m_iScreenY
	sub		eax,ds:[edx].CMV2Dot3DPos__m_iScreenY
	test	eax,eax
	jle		@@PolygonNotVisible

	mov		dwYCounter,eax

	fld		dword ptr ds:[edx].CMV2Dot3DPos__m_fScreenY
	fld		dword ptr ds:[esi].CMV2Dot3DPos__m_fScreenY
	fld		dword ptr ds:[edi].CMV2Dot3DPos__m_fScreenY
		;st0 = (y3), st1 = (y2), st2 = (y1)

	fld		st
	fsub	st,st(2)
	fxch
		;st0 = (y3), st1 = (y3 - y2), st2 = (y2), st3 = (y1)

	fsub	st,st(3)
	fxch	st(2)
		;st0 = (y2), st1 = (y3 - y2), st2 = (y3 - y1), st3 = (y1)

	fsub	st,st(3)
	fstp	fYCounter			; Attention! YCounter = y2 - y1 is nearly 0
		;st0 = (y3 - y2), st1 = (y3 - y1), st2 = (y1)

	fld1
	fdivrp	st(2),st
		;st0 = 1/(y3 - y2), st1 = 1/(y3 - y1)

	mov		eax,dwXmax
	mov		ebx,ds:[edx].CMV2Dot3DPos__m_iScreenY
	dec		ebx
	imul    eax,ebx
	add		eax,pcBackBuffer
	mov		dwYOffset1,eax

	fld1
	fdivrp	st(1),st
	ffree	st(2)

;===>                                                                <===
;===>	Screen Delta Calculation {                                   <===
;===>                                                                <===
		;st0 = 1/YCounter2, st1 = 1/YCounter1

	fld		dword ptr ds:[edx].CMV2Dot3DPos__m_fScreenX
	fld		dword ptr ds:[esi].CMV2Dot3DPos__m_fScreenX
	fld		dword ptr ds:[edi].CMV2Dot3DPos__m_fScreenX
		;st0 = (x3), st1 = (x2), st2 = (x1)
		;st3 = 1/YCounter2, st4 = 1/YCounter1

	fsubr	st(2),st
	fsubrp	st(1),st
	fmul	st,st(2)
	fxch
	fmul	st,st(3)						; fmul stall (+1 Cycle)
	fxch
	fld		dword ptr ds:[esi].CMV2Dot3DPos__m_fScreenYError
	fld		dword ptr ds:[edx].CMV2Dot3DPos__m_fScreenYError
		;st0 = ScreenYError1, st1 = ScreenYError2
		;st2 = DeltaScreenX2, st3 = DeltaScreenX1
		;st4 = 1/YCounter2, st5 = 1/YCounter1

	fmul	st,st(3)
	fxch
	fmul	st,st(2)
	fxch
	fadd	dword ptr ds:[edx].CMV2Dot3DPos__m_fScreenX
	fxch
	fadd	dword ptr ds:[esi].CMV2Dot3DPos__m_fScreenX
	fxch
		;st0 = ScreenYError1*DeltaScreenX1 + ScreenX1
		;st1 = ScreenYError2*DeltaScreenX2 + ScreenX2
		;st2 = DeltaScreenX2, st3 = DeltaScreenX1
		;st4 = 1/YCounter2, st5 = 1/YCounter1

	fistp	dwScreenX1
	fistp	dwScreenX2
	fistp	dwDeltaScreenX2
		;st0 = DeltaScreenX1, st1 = 1/YCounter2, st2 = 1/YCounter1

;===>	MaxXCounter calculation	{        	                         <===

	fld		fYCounter
	fmul	st,st(1)
		;st0 = fYCounter*DeltaScreenX1, st1 = DeltaScreenX1
		;st2 = 1/YCounter2, st3 = 1/YCounter1

	fadd	dword ptr ds:[edx].CMV2Dot3DPos__m_fScreenX
	fsubr	dword ptr ds:[esi].CMV2Dot3DPos__m_fScreenX             ;fp dep.
		;st0 = ScreenX2 - (fYCounter12*DeltaScreenX1 + ScreenX1)
		;st1 = DeltaScreenX1, st2 = 1/YCounter2, st3 = 1/YCounter1
	fist	dwMaxXCounter
	fld1
	fdivrp	st(1),st
	fstp	fMaxXCounterRZP											;fp dep.

;===>	MaxXCounter calculation	}        	                         <===

	fistp	dwDeltaScreenX1
		;st0 = 1/YCounter2, st1 = 1/YCounter1
;===>                                                                <===
;===>	Screen Delta Calculation }                                   <===
;===>                                                                <===
	mov		ebx,dwYCounter
	cmp		ebx,dwSpanLength
	jae		@@PerspPT4

	mov		eax,ds:[edx].CMV2Dot3DPos__m_iScreenX
	mov		ebx,ds:[esi].CMV2Dot3DPos__m_iScreenX
	mov		ebp,eax
	mov		ecx,ds:[edi].CMV2Dot3DPos__m_iScreenX

	sub		eax,ebx		; p1 - p2
	sub		ebx,ecx		; p2 - p3
	sub		ecx,ebp		; p3 - p1

	cmp		eax,0
	jns		@@PerspCheck1PT4
	neg		eax
@@PerspCheck1PT4:
	cmp		ebx,0
	jns		@@PerspCheck2PT4
	neg		ebx
@@PerspCheck2PT4:
	cmp		ecx,0
	jns		@@PerspCheck3PT4
	neg		ecx
@@PerspCheck3PT4:

	cmp		eax,dwSpanLength
	jae		@@PerspPT4

	cmp		ebx,dwSpanLength
	jae		@@PerspPT4

	cmp		ecx,dwSpanLength
	jae		@@PerspPT4

	jmp		@@LinearPolygonType4
@@PerspPT4:

	mov		eax,ds:[edx].CMV2Dot3DPos__m_fZNewRZP
	mov		ebx,ds:[esi].CMV2Dot3DPos__m_fZNewRZP
	mov		ecx,ds:[edi].CMV2Dot3DPos__m_fZNewRZP
	mov		fOneDivZ1NSBC,eax
	mov		fOneDivZ2NSBC,ebx
	mov		fOneDivZ3NSBC,ecx

	mov		eax,ds:[edx].CMV2Dot3DPos__m_fScreenYError
	mov		ebx,ds:[esi].CMV2Dot3DPos__m_fScreenYError
	mov     fScreenYError1,eax
	mov		fScreenYError2,ebx

	mov		edx,pDot1
	mov		esi,pDot2
	mov		edi,pDot3



;===>                                                                <===
;===>	TextureU&VDivZ DeltaY Calculation {                          <===
;===>                                                                <===
		;st0 = 1/YCounter2, st1 = 1/YCounter1


	fld		dword ptr ds:[edx].CMV2Dot3D__m_fTextureU
	fmul	fOneDivZ1NSBC
	fld		dword ptr ds:[edi].CMV2Dot3D__m_fTextureU
	fmul	fOneDivZ3NSBC
	fxch
			;st0 = tu1/z1, st1 = tu3/z3
	fst		fTextureUDivZ1NSBC
	fsubp	st(1),st
		;st0 = (tu3/z3 - tu1/z1)
		;st1 = 1/YCounter2, st2 = 1/YCounter1

	fld		dword ptr ds:[edx].CMV2Dot3D__m_fTextureV
	fmul	fOneDivZ1NSBC
	fld		dword ptr ds:[edi].CMV2Dot3D__m_fTextureV
	fmul	fOneDivZ3NSBC
	fxch
			;st0 = tv1/z1, st1 = tu3/z3
	fst		fTextureVDivZ1NSBC
	fsubp	st(1),st
	fxch
		;st0 = (tu3/z3 - tu1/z1), st1 = (tv3/z3 - tv1/z1)
		;st2 = 1/YCounter2, st3 = 1/YCounter1

	fmul	st,st(3)
	fxch
	fmul	st,st(3)      				; fmul stall (+1 Cycle)
	fxch
		;st0 = TextureUDivZdY1, st1 = TextureVDivZdY1
		;st2 = 1/YCounter2, st3 = 1/YCounter1

	fld		fScreenYError1
	fld		st
	fmul	st,st(2)
	fxch
	fmul	st,st(3)                    ; fmul stall (+1 Cycle)
	fxch
	fadd	fTextureUDivZ1NSBC
	fxch
	fadd	fTextureVDivZ1NSBC
	fxch
		;st0 = ScreenYError1*TextureUDivZdY1 + TextureUDivZ
		;st1 = ScreenYError1*TextureVDivZdY1 + TextureVDivZdY1
		;st2 = DeltaTextureX1, st3 = DeltaTextureY1
		;st4 = 1/YCounter2, st5 = 1/YCounter1

	fstp	fTextureUDivZ1
	fstp	fTextureVDivZ1
	fstp	fTextureUDivZdY1
	fstp	fTextureVDivZdY1
		;st0 = 1/YCounter2, st1 = 1/YCounter1
;===>                                                                <===
;===>	TextureU&VDivZ DeltaY Calculation }                          <===
;===>                                                                <===

;===>                                                                <===
;===>	OneDivZ DeltaY Calculation {                            	 <===
;===>                                                                <===
;***> TODO: Optimize

	fld		fOneDivZ3NSBC
	fsub	fOneDivZ1NSBC
		;st0 = (1/z3 - 1/z1)
		;st1 = 1/YCounter2, st2 = 1/YCounter1

	fmul	st,st(2)
		;st0 = OneDivZdY

	fld     fScreenYError1
	fmul    st,st(1)
		;st0 = ScreenYError1*OneDivZdY
		;st1 = OneDivZdY
	fadd	fOneDivZ1NSBC
	fxch
		;st0 = OneDivZdY
		;st1 = OneDivZ1

	fstp	fOneDivZdY1
	fstp	fOneDivZ1
;===>                                                                <===
;===>	OneDivZ DeltaY Calculation }                            	 <===
;===>                                                                <===



;===>                                                                <===
;===>	TextureU&VDivZ DeltaX Calculation {                          <===
;===>                                                                <===

	fld		fOneDivZ2NSBC
	fld		dword ptr ds:[esi].CMV2Dot3D__m_fTextureU
	fmul    st,st(1)
	fxch
	fmul	dword ptr ds:[esi].CMV2Dot3D__m_fTextureV
	fxch
	fstp	fTextureUDivZ2NSBC
	fstp	fTextureVDivZ2NSBC

	fld		fMaxXCounterRZP
	fmul	fNum_2EXP20
	fld		fYCounter
	fld		fTextureVDivZdY1
	fld		fTextureUDivZdY1
		;st0 = fTextureUDivZdY1, st1 = fTextureVDivZdY1
		;st2 = YCounter, st3 = 1/MaxXCounter
		;st4 = 1/YCounter2, st5 = 1/YCounter1

	fmul	st,st(2)
	fxch
	fmulp	st(2),st			; fmul stall (+ 1 Cycle)
	fadd    fTextureUDivZ1NSBC
	fxch
	fadd    fTextureVDivZ1NSBC
	fxch
	fsubr	fTextureUDivZ2NSBC
	fxch
	fsubr	fTextureVDivZ2NSBC
	fxch
	fmul    st,st(2)
	fxch
	fmulp	st(2),st
		;st0 = fTextureUDivZdX
		;st1 = fTextureVDivZdX
		;st2 = 1/YCounter2, st4 = 1/YCounter1

	fstp	fTextureUDivZdX
	fstp	fTextureVDivZdX
;===>                                                                <===
;===>	TextureU&VDivZ DeltaX Calculation }                          <===
;===>                                                                <===



;===>                                                                <===
;===>	OneDivZ DeltaX Calculation {                          		 <===
;===>                                                                <===
;***> TODO: Optimize

	fld		fMaxXCounterRZP
	fmul	fNum_2EXP20
	fld		fYCounter
	fmul	fOneDivZdY1
		;st0 = OneDivZ1dY1*YCounter1
		;st1 = 1/MaxXCounter
		;st2 = 1/YCounter2, st3 = 1/YCounter1

	fadd	fOneDivZ1NSBC
	fsubr	fOneDivZ2NSBC
	fmulp	st(1),st
		;st0 = fOneDivZdX
		;st2 = 1/YCounter2, st3 = 1/YCounter1

	fstp	fOneDivZdX
;===>                                                                <===
;===>	OneDivZ DeltaX Calculation }                          		 <===
;===>                                                                <===

	ffree	st(1)
	ffree	st(0)

	mov		eax,dwDeltaScreenX1
	mov		ebx,dwDeltaScreenX2
	mov		esi,eax
	mov		edi,ebx
	shl		eax,12
	shl		ebx,12
	sar		esi,20
	sar		edi,20
	add		esi,dwXmax
	add		edi,dwXmax
	mov		dwAdderScreenX1,esi
	mov		dwAdderScreenX2,edi
	mov		dwAdderScreenX1f,eax
	mov		dwAdderScreenX2f,ebx

	mov		eax,dwScreenX1
	mov		ebx,dwScreenX2
	mov		esi,eax
	mov		edi,ebx
	shl		eax,12
	shl		ebx,12
	sar		esi,20
	sar		edi,20
	add		esi,dwYOffset1
	add     edi,dwYOffset1

	mov		ebp,dwYCounter
	mov		ecx,edi

	mov		dwOldESP,esp
	and		esp,0ffffffe0h

	fld		fNum_2EXP_20
	fld     fNum_2EXP20
	fld		fTextureVDivZ1
	fld		fTextureUDivZ1
	fld		fOneDivZ1

@@PerspPT4ScanlinerTLoopPass1:
				; eax = x1f (20 bit fraction)
				; ebx = x2f (20 bit fraction)
				; ecx = XCounter (init. BackBuffer + YOffset + X2)
				; edx =
				; esi = BackBuffer + YOffset + X1
				; edi = BackBuffer + YOffset + X2
				; ebp = YCounter

				; st0 = OneDivZ
				; st1 = TextureUDivZ
				; st2 = TextureVDivZ
				; st3 = 2^20
				; st4 = 2^-20

;***> TODO: Pipeline the fpu better
	shr		eax,12
	sub		esp,CMV2ScanlinerPerspT__size

	mov		dwScreenXf,eax
	sub		ecx,esi					; XCounter
	shl		eax,12

	fild	dwScreenXf
	fsubr	st,st(4)
		; st0 = 2^20 - ScreenXf, st1 = OneDivZ
		; st2 = TextureUDivZ, st3 = TextureVDivZ,
		; st4 = 2^20, st5 = 2^-20

	fmul	st,st(5)
		; st0 = ScreenXError

	fld		fTextureUDivZdX
	fmul	st,st(1)
	fld		fTextureVDivZdX
	fmul	st,st(2)
	fxch	st(2)
	fmul	fOneDivZdX
	fxch	st(2)
		; st0 = TextureVDivZdX*ScreenXError
		; st1 = TextureUDivZdX*ScreenXError
		; st2 = OneDivZdX*ScreenXError, st3 = OneDivZ
		; st4 = TextureUDivZ, st5 = TextureVDivZ,
		; st6 = 2^20, st7 = 2^-20

	fadd   	st,st(5)
	fxch
	fadd	st,st(4)
	fxch	st(2)
	fadd	st,st(3)
	fxch	st(2)
		; st0 = TextureUDivZ*ScreenXError + TextureUDivZ
		; st1 = TextureVDivZ*ScreenXError + TextureVDivZ
		; st2 = OneDivZ*ScreenXError + OneDivZ
		; st3 = OneDivZ
		; st4 = TextureUDivZ, st5 = TextureVDivZ,
		; st6 = 2^20, st7 = 2^-20

	fstp	DWORD PTR ss:[esp].CMV2ScanlinerPerspT__fTextureUDivZ
	fstp	DWORD PTR ss:[esp].CMV2ScanlinerPerspT__fTextureVDivZ
	fstp	DWORD PTR ss:[esp].CMV2ScanlinerPerspT__fOneDivZ
		; st0 = OneDivZ, st1 = TextureUDivZ, st2 = TextureVDivZ
		; st3 = 2^20, st4 = 2^-20

	mov		ss:[esp].CMV2ScanlinerPerspT__dwScreenX1Offset,esi
	mov		ss:[esp].CMV2ScanlinerPerspT__dwXCounter,ecx

	fadd	fOneDivZdY1
	fxch
	fadd	fTextureUDivZdY1
	fxch
	fxch	st(2)
	fadd	fTextureVDivZdY1
	fxch	st(2)

	add		ebx,dwAdderScreenX2f
	adc     edi,dwAdderScreenX2
	add		eax,dwAdderScreenX1f
	adc		esi,dwAdderScreenX1
	mov		ecx,edi

	dec		ebp
	jnz		@@PerspPT4ScanlinerTLoopPass1

	ffree	st(4)
	ffree	st(3)
	ffree	st(2)
	ffree	st(1)
	ffree	st(0)

	jmp		@@PerspTYLoop


@@PerspTYLoop:
	mov		ecx,ss:[esp].CMV2ScanlinerPerspT__dwXCounter
	mov		edi,ss:[esp].CMV2ScanlinerPerspT__dwScreenX1Offset

	add 	iPixelCounter,ecx			;***********

			; ecx = XCounter
	test	ecx,ecx
;	jz		@@PerspTNoXLoop
	jng		@@PerspTNoXLoop				;***********
	mov		dwXCounter,ecx

	mov		eax,ecx
	mov		dwScanlineXCounter,ecx

	cmp		ecx,dwSpanLength
	jng		@@PerspTFirstRemnantSpan
	mov		ecx,dwSpanLength
@@PerspTFirstRemnantSpan:

	sub		eax,dwSpanLength
	mov		dwNextXCounter,ecx

	fld		ss:[esp].CMV2ScanlinerPerspT__fOneDivZ
	fld		ss:[esp].CMV2ScanlinerPerspT__fTextureVDivZ
	fld		ss:[esp].CMV2ScanlinerPerspT__fTextureUDivZ
			;st0 = UL/ZL, st1 = VL/ZL, st2 = 1/ZL

	fld1
	fdiv	st,st(3)
			;st0 = ZL, st1 = UL/ZL, st2 = VL/ZL, st3 = 1/ZL

	fld		st
	fmul	st,st(2)
	fxch
	fmul	st,st(3)
	fxch
			;st0 = UL, st1 = VL, st2 = UL/ZL, st3 = VL/ZL
			;st4 = 1/ZL

	fild	dwNextXCounter
		;st0 = NextXCounter, st1 = UL, st2 = VL, st3 = UL/ZL
		;st4 = VL/ZL, st5 = 1/ZL
	fld		st
	fmul	fTextureUDivZdX
	fld		st(1)
	fmul	fTextureVDivZdX
	fxch
		;st0 = NextXCounter*fTextureUDivZ, st1 = NextXCounter*fTextureVDivZ
		;st2 = dwNextXCounter, st3 = UL, st4 = VL, st5 = UL/ZL
		;st6 = VL/ZL, st7 = 1/ZL
	faddp	st(5),st
	fxch
	fmul	fOneDivZdX
	fxch
	faddp	st(5),st
	faddp	st(5),st
			;st0 = UL, st1 = VL, st2 = UR/ZR, st3 = VR/ZR, st4 = 1/ZR

	fist	dwTextureU
	fxch
	fist	dwTextureV
	fxch

	fld1
	fdiv	st,st(5)
			;st0 = ZR, st1 = UL, st2 = VL, st3 = UR/ZR, st4 = VR/ZR
			;st5 = 1/ZR

	fld		st
		;st0 = ZR, st1 = ZR, st2 = UL, st3 = VL
		;st4 = UR/ZR, st5 = VR/ZR, st6 = 1/ZR

	fmul	st,st(4)
	fxch
	fmul	st,st(5)
	fxch
		;st0 = UR, st1 = VR, st2 = UL, st3 = VL
		;st4 = UR/ZR, st5 = VR/ZR, st6 = 1/ZR

	fsubr	st(2),st
	fxch
	fsubr	st(3),st
		;st0 = VR, st1 = UR, st2 = UR - UL, st3 = VR - VL
		;st4 = UR/ZR, st5 = VR/ZR, st6 = 1/ZR

	fxch	st(3)
	fmul	ds:[ecx*4].fSpanLengthFactorsRZP
	fxch
	fxch	st(2)
	fmul	ds:[ecx*4].fSpanLengthFactorsRZP
	fxch

		;st0 = (VR - VL)/SpanLength, st1 = (UR - UL)/SpanLength
		;st2 = UR, st3 = VR, st4 = UR/ZR, st5 = VR/ZR, st6 = 1/ZR

	fistp	dwTextureVdX
	fistp	dwTextureUdX
		;st0 = UR, st1 = VR, st2 = UR/ZR, st3 = VR/ZR, st4 = 1/ZR
		;right becomes left, left will be calculated in folowing lines..
		;st0 = UL, st1 = VL, st2 = UL/ZL, st3 = VL/ZL, st4 = 1/ZL

		;ecx = dwNextXCounter ( = CurXCounter )


	test	eax,eax
	jng		@@PerspTDoRemnantPixel
	mov		ecx,eax

	cmp		ecx,dwSpanLength
	jng		@@PerspTSecondRemnantSpan
	mov		ecx,dwSpanLength
@@PerspTSecondRemnantSpan:

	mov		ebx,dwNextXCounter
	sub		eax,dwSpanLength
	mov		dwCurXCounter,ebx
	mov		dwNextXCounter,ecx
	mov		dwScanlineXCounter,eax



		;st0 = UL, st1 = VL, st2 = UL/ZL, st3 = VL/ZL, st4 = 1/ZL
;***> TODO: Optimize
	fild	dwNextXCounter
		;st0 = NextXCounter, st1 = UL, st2 = VL, st3 = UL/ZL
		;st4 = VL/ZL, st5 = 1/ZL
	fld		st
	fmul	fTextureUDivZdX
	fld		st(1)
	fmul	fTextureVDivZdX
	fxch
		;st0 = NextXCounter*fTextureUDivZ, st1 = NextXCounter*fTextureVDivZ
		;st2 = dwNextXCounter, st3 = UL, st4 = VL, st5 = UL/ZL
		;st6 = VL/ZL, st7 = 1/ZL
	faddp	st(5),st
	fxch
	fmul	fOneDivZdX
	fxch
	faddp	st(5),st
	faddp	st(5),st
			;st0 = UL, st1 = VL, st2 = UR/ZR, st3 = VR/ZR, st4 = 1/ZR

@@PerspTSpanLoop:
			;st0 = UL, st1 = VL, st2 = UR/ZR, st3 = VR/ZR, st4 = 1/ZR
	fld1
	fdiv	st,st(5)

	mov		ebx,dwTextureUdX
	mov		eax,dwTextureVdX
	mov		ebp,ebx
	mov		esi,eax
	shr		ebx,16				; bl init.
	shr		eax,16
	shl		esi,16				; esi init.
	shl		ebp,16				; ebp init.
	mov		bh,al				; bh init.
	mov		eax,dwTextureU
	mov		ecx,dwTextureV
	mov		edx,eax
	shl		eax,16
	shr		edx,16				; dl init.
	or		ebx,eax				; ebx init.
	mov		eax,ecx
	shl		ecx,16				; hi ecx init.
	shr		eax,16
	or		ecx,dwCurXCounter	; ecx init.
	mov		dh,al				; dh init.
	or		edx,pcTexture		; edx init.

@@PerspTXLoop:
				; eax = col
				; ebx = txf			| dty 	| dtX
				; ecx = tyf			|		| XCounter
				; edx = pTexture	| ty	| tx
				; esi = dtyf
				; edi =	destination
				; ebp = dtxf

	push	edi
	push	ecx
	push	edx
	push	ebx

	add		ebx,ebp
	adc		dl,bl
	add		ecx,esi
	adc		dh,bh
	inc		edi

	dec		cl
	jnz		@@PerspTXLoop

		;st0 = ZR, st1 = UL, st2 = VL, st3 = UR/ZR
		;st4 = VR/ZR, st5 = 1/ZR

;***> TODO: Optimize
	fxch	st(2)
	fist	dwTextureV
	fxch
	fist	dwTextureU
	fxch
	fxch	st(2)

	fld		st
		;st0 = ZR, st1 = ZR, st2 = UL, st3 = VL
		;st4 = UR/ZR, st5 = VR/ZR, st6 = 1/ZR

	fmul	st,st(4)
	fxch
	fmul	st,st(5)
	fxch
		;st0 = UR, st1 = VR, st2 = UL, st3 = VL
		;st4 = UR/ZR, st5 = VR/ZR, st6 = 1/ZR

	fsubr	st(2),st
	fxch
	fsubr	st(3),st
		;st0 = VR, st1 = UR, st2 = UR - UL, st3 = VR - VL
		;st4 = UR/ZR, st5 = VR/ZR, st6 = 1/ZR

	mov 	eax,dwNextXCounter
	fxch	st(3)
	fmul	ds:[eax*4].fSpanLengthFactorsRZP
	fxch
	fxch	st(2)
	fmul	ds:[eax*4].fSpanLengthFactorsRZP
	fxch
		;st0 = (VR - VL)/SpanLength, st1 = (UR - UL)/SpanLength
		;st2 = UR, st3 = VR, st4 = UR/ZR, st5 = VR/ZR, st6 = 1/ZR

	fistp	dwTextureVdX
	fistp	dwTextureUdX
		;st0 = UR, st1 = VR, st2 = UR/ZR, st3 = VR/ZR, st4 = 1/ZR
		;right becomes left, left will be calculated in folowing lines..

		;st0 = UL, st1 = VL, st2 = UL/ZL, st3 = VL/ZL, st4 = 1/ZL

	cmp		dwScanlineXCounter,0
	jng		@@PerspTDoRemnantPixel

	mov		ecx,dwScanlineXCounter
	mov		eax,ecx
	cmp		ecx,dwSpanLength
	jng		@@PerspTRemnantSpanFollows
	mov		ecx,dwSpanLength
@@PerspTRemnantSpanFollows:

	mov		ebx,dwNextXCounter
	sub		eax,dwSpanLength

	mov		dwCurXCounter,ebx
	mov		dwNextXCounter,ecx
	mov		dwScanlineXCounter,eax


;***> TODO: Optimize
	fild	dwNextXCounter
		;st0 = NextXCounter, st1 = UL, st2 = VL, st3 = UL/ZL
		;st4 = VL/ZL, st5 = 1/ZL
	fld		st
	fmul	fTextureUDivZdX
	fld		st(1)
	fmul	fTextureVDivZdX
	fxch
		;st0 = NextXCounter*fTextureUDivZ, st1 = NextXCounter*fTextureVDivZ
		;st2 = dwNextXCounter, st3 = UL, st4 = VL, st5 = UL/ZL
		;st6 = VL/ZL, st7 = 1/ZL
	faddp	st(5),st
	fxch
	fmul	fOneDivZdX
	fxch
	faddp	st(5),st
	faddp	st(5),st
			;st0 = UL, st1 = VL, st2 = UR/ZR, st3 = VR/ZR, st4 = 1/ZR

	jmp		@@PerspTSpanLoop


@@PerspTDoRemnantPixel:
	mov		ebx,dwTextureUdX
	mov		eax,dwTextureVdX
	mov		ebp,ebx
	mov		esi,eax
	shr		ebx,16				; bl init.
	shr		eax,16
	shl		esi,16				; esi init.
	shl		ebp,16				; ebp init.
	mov		bh,al				; bh init.
	mov		eax,dwTextureU
	mov		ecx,dwTextureV
	mov		edx,eax
	shl		eax,16
	shr		edx,16				; dl init.
	or		ebx,eax				; ebx init.
	mov		eax,ecx
	shl		ecx,16				; hi ecx init.
	shr		eax,16
	or		ecx,dwNextXCounter	; ecx init.
	mov		dh,al				; dh init.
	or		edx,pcTexture		; edx init.

@@PerspTRemnantPixelXLoop:
				; eax = col
				; ebx = txf			| dty 	| dtX
				; ecx = tyf			|		| XCounter
				; edx = pTexture	| ty	| tx
				; esi = dtyf
				; edi =	destination
				; ebp = dtxf

	push	edi
	push	ecx
	push	edx
	push	ebx

	add		ebx,ebp
	adc		dl,bl
	add		ecx,esi
	adc		dh,bh
	inc		edi

	dec		cl
	jnz		@@PerspTRemnantPixelXLoop

;	mov		ds:[4*edi - 4],0ffffffh

	mov		ebp,dwXCounter

@@PerspFltLoop:
; eax = pTexture>>2	|ty		|tx
; ebx = pTexture>>2	|ty		|tx
; ecx =
; edx =
; esi =
; edi =
; ebp = XCounter

	pop		esi							; txf
	pop		ecx							; pTexture>>2	|ty		|tx

	shr		esi,29
	mov		edx,ecx
	mov		edi,offset PerspFltJumpTableX
	inc     dl

	jmp		ds:[edi + 4*esi]

PerspFltCaseX0:
	pop		ebx						; tyf
	dec		dl
	shr		ebx,29
	inc		dh
	mov		eax,ds:[4*ecx]
	mov		ecx,ds:[4*edx]

	lea		eax,[8*eax]
	lea		ebx,[4*ebx]
	lea		ecx,[8*ecx]
	add		ebx,offset PerspFltJumpTableY

	jmp		[ebx]

PerspFltCaseX1:
	mov		eax,ds:[4*ecx]				; 1. Texel
	mov		esi,ds:[4*edx]				; 2. Texel
	mov		ebx,eax
	mov		edi,esi

	inc		ch
	inc		dh

	and		eax,0f8f8f8f8h
	and		esi,0f8f8f8f8h
	and		ebx,007070707h
	and		edi,007070707h

	sub		esi,eax
	sub		edi,ebx

	lea		eax,[8*eax]
	lea		ebx,[8*ebx]

	add		eax,esi
	add		ebx,edi

	mov		ecx,ds:[4*ecx]				; 3. Texel
	and		ebx,0f8f8f8f8h
	mov		esi,ds:[4*edx]				; 4. Texel
	add		eax,ebx

	mov		edx,ecx
	mov		edi,esi

	and		ecx,0f8f8f8f8h
	and		esi,0f8f8f8f8h
	and		edx,007070707h
	and		edi,007070707h

	sub		esi,ecx
	sub		edi,edx

	pop		ebx						; tyf
	lea		ecx,[8*ecx]
	shr		ebx,29
	lea		edx,[8*edx]


	add		ecx,esi
	add		edx,edi
	lea		ebx,[4*ebx]

	and		edx,0f8f8f8f8h
	add		ebx,offset PerspFltJumpTableY
	add		ecx,edx
	jmp		[ebx]

PerspFltCaseX2:
	mov		eax,ds:[4*ecx]				; 1. Texel
	mov		esi,ds:[4*edx]				; 2. Texel
	mov		ebx,eax
	mov		edi,esi

	inc		ch
	inc		dh

	and		eax,0f8f8f8f8h
	and		ebx,007070707h
	and		esi,0f8f8f8f8h
	and		edi,007070707h

	lea		eax,[2*eax + eax]
	lea		ebx,[2*ebx + ebx]
	lea		esi,[2*esi]
	lea		edi,[2*edi]
	lea		eax,[2*eax]
	lea		ebx,[2*ebx]

	add		eax,esi
	add		ebx,edi

	mov		ecx,ds:[4*ecx]				; 3. Texel
	and		ebx,0f8f8f8f8h
	mov		esi,ds:[4*edx]				; 4. Texel
	add		eax,ebx

	mov		edx,ecx
	mov		edi,esi

	and		ecx,0f8f8f8f8h
	and		edx,007070707h
	and		esi,0f8f8f8f8h
	and		edi,007070707h

	lea		ecx,[2*ecx + ecx]
	lea		edx,[2*edx + edx]
	lea		esi,[2*esi]
	lea		edi,[2*edi]
	pop		ebx						; tyf
	lea		ecx,[2*ecx]
	shr		ebx,29
	lea		edx,[2*edx]


	add		ecx,esi
	add		edx,edi
	lea		ebx,[4*ebx]

	and		edx,0f8f8f8f8h
	add		ebx,offset PerspFltJumpTableY
	add		ecx,edx
	jmp		[ebx]

PerspFltCaseX3:
	mov		eax,ds:[4*ecx]				; 1. Texel
	mov		esi,ds:[4*edx]				; 2. Texel
	mov		ebx,eax
	mov		edi,esi

	inc		ch
	inc		dh

	and		eax,0f8f8f8f8h
	and		ebx,007070707h
	and		esi,0f8f8f8f8h
	and		edi,007070707h

	lea		eax,[4*eax + eax]
	lea		ebx,[4*ebx + ebx]
	lea		esi,[2*esi + esi]
	lea		edi,[2*edi + edi]

	add		eax,esi
	add		ebx,edi

	mov		ecx,ds:[4*ecx]				; 3. Texel
	and		ebx,0f8f8f8f8h
	mov		esi,ds:[4*edx]				; 4. Texel
	add		eax,ebx

	mov		edx,ecx
	mov		edi,esi

	and		ecx,0f8f8f8f8h
	and		edx,007070707h
	and		esi,0f8f8f8f8h
	and		edi,007070707h

	lea		ecx,[4*ecx + ecx]
	lea		edx,[4*edx + edx]
	pop		ebx						; tyf
	lea		esi,[2*esi + esi]
	shr		ebx,29
	lea		edi,[2*edi + edi]

	add		ecx,esi
	add		edx,edi
	lea		ebx,[4*ebx]

	and		edx,0f8f8f8f8h
	add		ebx,offset PerspFltJumpTableY
	add		ecx,edx
	jmp		[ebx]

PerspFltCaseX4:
	mov		eax,ds:[4*ecx]				; 1. Texel
	mov		esi,ds:[4*edx]				; 2. Texel
	mov		ebx,eax
	mov		edi,esi

	inc		ch
	inc		dh

	and		eax,0f8f8f8f8h
	and		ebx,007070707h
	and		esi,0f8f8f8f8h
	and		edi,007070707h

	lea		eax,[4*eax]
	lea		ebx,[4*ebx]
	lea		esi,[4*esi]
	lea		edi,[4*edi]

	add		eax,esi
	add		ebx,edi

	mov		ecx,ds:[4*ecx]				; 3. Texel
	and		ebx,0f8f8f8f8h
	mov		esi,ds:[4*edx]				; 4. Texel
	add		eax,ebx

	mov		edx,ecx
	mov		edi,esi

	and		ecx,0f8f8f8f8h
	and		edx,007070707h
	and		esi,0f8f8f8f8h
	and		edi,007070707h

	lea		ecx,[4*ecx]
	lea		edx,[4*edx]
	pop		ebx						; tyf
	lea		esi,[4*esi]
	shr		ebx,29
	lea		edi,[4*edi]

	add		ecx,esi
	add		edx,edi
	lea		ebx,[4*ebx]

	and		edx,0f8f8f8f8h
	add		ebx,offset PerspFltJumpTableY
	add		ecx,edx
	jmp		[ebx]

PerspFltCaseX5:
	mov		eax,ds:[4*ecx]				; 1. Texel
	mov		esi,ds:[4*edx]				; 2. Texel
	mov		ebx,eax
	mov		edi,esi

	inc		ch
	inc		dh

	and		eax,0f8f8f8f8h
	and		ebx,007070707h
	and		esi,0f8f8f8f8h
	and		edi,007070707h

	lea		eax,[2*eax + eax]
	lea		ebx,[2*ebx + ebx]
	lea		esi,[4*esi + esi]
	lea		edi,[4*edi + edi]

	add		eax,esi
	add		ebx,edi

	mov		ecx,ds:[4*ecx]				; 3. Texel
	and		ebx,0f8f8f8f8h
	mov		esi,ds:[4*edx]				; 4. Texel
	add		eax,ebx

	mov		edx,ecx
	mov		edi,esi

	and		ecx,0f8f8f8f8h
	and		edx,007070707h
	and		esi,0f8f8f8f8h
	and		edi,007070707h

	lea		ecx,[2*ecx + ecx]
	lea		edx,[2*edx + edx]
	pop		ebx						; tyf
	lea		esi,[4*esi + esi]
	shr		ebx,29
	lea		edi,[4*edi + edi]

	add		ecx,esi
	add		edx,edi
	lea		ebx,[4*ebx]

	and		edx,0f8f8f8f8h
	add		ebx,offset PerspFltJumpTableY
	add		ecx,edx
	jmp		[ebx]

PerspFltCaseX6:
	mov		eax,ds:[4*ecx]				; 1. Texel
	mov		esi,ds:[4*edx]				; 2. Texel
	mov		ebx,eax
	mov		edi,esi

	inc		ch
	inc		dh

	and		esi,0f8f8f8f8h
	and		edi,007070707h
	and		eax,0f8f8f8f8h
	and		ebx,007070707h

	lea		esi,[2*esi + esi]
	lea		edi,[2*edi + edi]
	lea		eax,[2*eax]
	lea		ebx,[2*ebx]
	lea		esi,[2*esi]
	lea		edi,[2*edi]

	add		eax,esi
	add		ebx,edi

	mov		ecx,ds:[4*ecx]				; 3. Texel
	and		ebx,0f8f8f8f8h
	mov		esi,ds:[4*edx]				; 4. Texel
	add		eax,ebx

	mov		edx,ecx
	mov		edi,esi

	and		esi,0f8f8f8f8h
	and		edi,007070707h
	and		ecx,0f8f8f8f8h
	and		edx,007070707h

	lea		esi,[2*esi + esi]
	lea		edi,[2*edi + edi]
	lea		ecx,[2*ecx]
	lea		edx,[2*edx]
	pop		ebx						; tyf
	lea		esi,[2*esi]
	shr		ebx,29
	lea		edi,[2*edi]

	add		ecx,esi
	add		edx,edi
	lea		ebx,[4*ebx]

	and		edx,0f8f8f8f8h
	add		ebx,offset PerspFltJumpTableY
	add		ecx,edx
	jmp		[ebx]

PerspFltCaseX7:
	mov		eax,ds:[4*ecx]				; 1. Texel
	mov		esi,ds:[4*edx]				; 2. Texel
	mov		ebx,eax
	mov		edi,esi

	inc		ch
	inc		dh

	and		eax,0f8f8f8f8h
	and		esi,0f8f8f8f8h
	and		ebx,007070707h
	and		edi,007070707h

	sub		eax,esi
	sub		ebx,edi

	lea		esi,[8*esi]
	lea		edi,[8*edi]

	add		eax,esi
	add		ebx,edi

	mov		ecx,ds:[4*ecx]				; 3. Texel
	and		ebx,0f8f8f8f8h
	mov		esi,ds:[4*edx]				; 4. Texel
	add		eax,ebx

	mov		edx,ecx
	mov		edi,esi

	and		ecx,0f8f8f8f8h
	and		esi,0f8f8f8f8h
	and		edx,007070707h
	and		edi,007070707h

	sub		ecx,esi
	sub		edx,edi

	pop		ebx						; tyf
	lea		esi,[8*esi]
	shr		ebx,29
	lea		edi,[8*edi]

	add		ecx,esi
	add		edx,edi
	lea		ebx,[4*ebx]

	and		edx,0f8f8f8f8h
	add		ebx,offset PerspFltJumpTableY
	add		ecx,edx
	jmp		[ebx]

PerspFltCaseY0:
	lea		eax,[eax*8]
	pop		edi
	shr		eax,6

	and		eax,0fefefefeh
	shr		eax,1
	mov		dwTemp,eax
	mov		eax,ds:[edi*4]
	and		eax,0fefefefeh
	shr		eax,1
	add		eax,dwTemp

	dec		ebp
	mov		ds:[4*edi],eax

	jnz		@@PerspFltLoop
	jmp		@@PerspFltDone

PerspFltCaseY1:
	mov		ebx,eax
	mov		edx,ecx
	and		eax,0c7c7c7c7h
	and		ecx,0c7c7c7c7h
	and		ebx,038383838h
	and		edx,038383838h

	sub		ecx,eax
	sub		edx,ebx

	lea		eax,[8*eax]
	lea		ebx,[8*ebx]

	add		eax,ecx
	add		ebx,edx

	and		ebx,0c7c7c7c7h
	add		eax,ebx

	pop		edi
	shr		eax,6

	and		eax,0fefefefeh
	shr		eax,1
	mov		dwTemp,eax
	mov		eax,ds:[edi*4]
	and		eax,0fefefefeh
	shr		eax,1
	add		eax,dwTemp


	dec		ebp
	mov		ds:[4*edi],eax

	jnz		@@PerspFltLoop
	jmp		@@PerspFltDone

PerspFltCaseY2:
	mov		ebx,eax
	mov		edx,ecx

	and		eax,0c7c7c7c7h
	and		ebx,038383838h
	and		ecx,0c7c7c7c7h
	and		edx,038383838h

	lea		eax,[2*eax + eax]
	lea		ebx,[2*ebx + ebx]
	lea		ecx,[2*ecx]
	lea		edx,[2*edx]
	lea		eax,[2*eax]
	lea		ebx,[2*ebx]

	add		eax,ecx
	add		ebx,edx

	and		ebx,0c7c7c7c7h
	add		eax,ebx

	pop		edi
	shr		eax,6

	and		eax,0fefefefeh
	shr		eax,1
	mov		dwTemp,eax
	mov		eax,ds:[edi*4]
	and		eax,0fefefefeh
	shr		eax,1
	add		eax,dwTemp

	dec		ebp
	mov		ds:[4*edi],eax

	jnz		@@PerspFltLoop
	jmp		@@PerspFltDone

PerspFltCaseY3:
	mov		ebx,eax
	mov		edx,ecx
	and		eax,0c7c7c7c7h
	and		ebx,038383838h
	and		ecx,0c7c7c7c7h
	and		edx,038383838h

	lea		eax,[4*eax + eax]
	lea		ebx,[4*ebx + ebx]
	lea		ecx,[2*ecx + ecx]
	lea		edx,[2*edx + edx]

	add		eax,ecx
	add		ebx,edx

	and		ebx,0c7c7c7c7h
	add		eax,ebx

	pop		edi
	shr		eax,6

	and		eax,0fefefefeh
	shr		eax,1
	mov		dwTemp,eax
	mov		eax,ds:[edi*4]
	and		eax,0fefefefeh
	shr		eax,1
	add		eax,dwTemp

	dec		ebp
	mov		ds:[4*edi],eax

	jnz		@@PerspFltLoop
	jmp		@@PerspFltDone

PerspFltCaseY4:
	mov		ebx,eax
	mov		edx,ecx
	and		eax,0c7c7c7c7h
	and		ebx,038383838h
	and		ecx,0c7c7c7c7h
	and		edx,038383838h

	lea		eax,[4*eax]
	lea		ebx,[4*ebx]
	lea		ecx,[4*ecx]
	lea		edx,[4*edx]

	add		eax,ecx
	add		ebx,edx

	and		ebx,0c7c7c7c7h
	add		eax,ebx

	pop		edi
	shr		eax,6

	and		eax,0fefefefeh
	shr		eax,1
	mov		dwTemp,eax
	mov		eax,ds:[edi*4]
	and		eax,0fefefefeh
	shr		eax,1
	add		eax,dwTemp

	dec		ebp
	mov		ds:[4*edi],eax

	jnz		@@PerspFltLoop
	jmp		@@PerspFltDone

PerspFltCaseY5:
	mov		ebx,eax
	mov		edx,ecx
	and		eax,0c7c7c7c7h
	and		ebx,038383838h
	and		ecx,0c7c7c7c7h
	and		edx,038383838h

	lea		eax,[2*eax + eax]
	lea		ebx,[2*ebx + ebx]
	lea		ecx,[4*ecx + ecx]
	lea		edx,[4*edx + edx]

	add		eax,ecx
	add		ebx,edx

	and		ebx,0c7c7c7c7h
	add		eax,ebx

	pop		edi
	shr		eax,6

	and		eax,0fefefefeh
	shr		eax,1
	mov		dwTemp,eax
	mov		eax,ds:[edi*4]
	and		eax,0fefefefeh
	shr		eax,1
	add		eax,dwTemp

	dec		ebp
	mov		ds:[4*edi],eax

	jnz		@@PerspFltLoop
	jmp		@@PerspFltDone

PerspFltCaseY6:
	mov		ebx,eax
	mov		edx,ecx
	and		ecx,0c7c7c7c7h
	and		edx,038383838h
	and		eax,0c7c7c7c7h
	and		ebx,038383838h

	lea		ecx,[2*ecx + ecx]
	lea		edx,[2*edx + edx]
	lea		eax,[2*eax]
	lea		ebx,[2*ebx]
	lea		ecx,[2*ecx]
	lea		edx,[2*edx]

	add		eax,ecx
	add		ebx,edx

	and		ebx,0c7c7c7c7h
	add		eax,ebx

	pop		edi
	shr		eax,6

	and		eax,0fefefefeh
	shr		eax,1
	mov		dwTemp,eax
	mov		eax,ds:[edi*4]
	and		eax,0fefefefeh
	shr		eax,1
	add		eax,dwTemp

	dec		ebp
	mov		ds:[4*edi],eax

	jnz		@@PerspFltLoop
	jmp		@@PerspFltDone

PerspFltCaseY7:
	mov		ebx,eax
	mov		edx,ecx
	and		eax,0c7c7c7c7h
	and		ecx,0c7c7c7c7h
	and		ebx,038383838h
	and		edx,038383838h

	sub		eax,ecx
	sub		ebx,edx

	lea		ecx,[8*ecx]
	lea		edx,[8*edx]

	add		eax,ecx
	add		ebx,edx

	and		ebx,0c7c7c7c7h
	add		eax,ebx

	pop		edi
	shr		eax,6

	and		eax,0fefefefeh
	shr		eax,1
	mov		dwTemp,eax
	mov		eax,ds:[edi*4]
	and		eax,0fefefefeh
	shr		eax,1
	add		eax,dwTemp

	dec		ebp
	mov		ds:[4*edi],eax

	jnz		@@PerspFltLoop
@@PerspFltDone:
@@PerspTNoXLoop:
	ffree	st(4)
	ffree	st(3)
	ffree	st(2)
	ffree	st(1)
	ffree	st(0)


	add		esp,CMV2ScanlinerPerspT__size

	dec		dwYCounter
	jnz		@@PerspTYLoop

	mov		esp,dwOldESP

@@PolygonNotVisible:

	jmp		@@Done

;======================> 				<==============================
;======================> 				<==============================
;======================> 				<==============================
;======================> 				<==============================
;======================> 				<==============================
;======================> 				<==============================
;======================> 				<==============================
;======================> 				<==============================
;======================> Linear Mapping <==============================
;======================> 				<==============================
;======================> 				<==============================
;======================> 				<==============================
;======================> 				<==============================
;======================> 				<==============================
;======================> 				<==============================
;======================> 				<==============================
;======================> 				<==============================


;=============>                <==============
;=============> Linear		   <==============
;=============> Polygon Type 1 <==============
;=============>                <==============

@@LinearPolygonType1:
	mov		eax,ds:[edx].CMV2Dot3DPos__m_fScreenYError
	mov		ebx,ds:[edi].CMV2Dot3DPos__m_fScreenYError
	mov     fScreenYError1,eax
	mov		fScreenYError3,ebx

	mov		edx,pDot1
	mov		esi,pDot2
	mov		edi,pDot3

;===>                                                                <===
;===>	TextureU DeltaY Calculation {                            	 <===
;===>                                                                <===

	;st0=1/YCounter13, st1=1/YCounter32, st2=1/YCounter12

	fld		dword ptr ds:[edx].CMV2Dot3D__m_fTextureU
	fld		dword ptr ds:[esi].CMV2Dot3D__m_fTextureU
	fld		dword ptr ds:[edi].CMV2Dot3D__m_fTextureU
	fld		st(1)
	fsub	st,st(1)
	fxch
	fsub	st,st(3)
	fxch
	fmul	st,st(5)
	fxch	st(2)
	fsub	st,st(3)
	fxch
	fmul	st,st(4)
	fxch	st(3)
	ffree   st
	fincstp
	fmul	st,st(5)
		;st0 = TextureUdY, st1 = TextureUdY2,
		;st2 = TextureUdY1, st3 = 1/YCounter1,
		;st4 = 1/YCounter2, st5 = 1/YCounter

	fld		fScreenYError1
	fld		fScreenYError3
	fmul	st,st(3)
	fxch
	fmul	st,st(4)
	fxch
	fadd	dword ptr ds:[edi].CMV2Dot3D__m_fTextureU
	fxch
	fadd	dword ptr ds:[edx].CMV2Dot3D__m_fTextureU
	fxch

	fstp	fTextureU3
	fstp	fTextureU1
	fstp	fTextureUdY
	fstp	fTextureUdY2
	fstp	fTextureUdY1

;===>                                                                <===
;===>	TextureU DeltaY Calculation }                            	 <===
;===>                                                                <===


;===>                                                                <===
;===>	TextureV DeltaY Calculation {                            	 <===
;===>                                                                <===
	fld		dword ptr ds:[edx].CMV2Dot3D__m_fTextureV
	fld		dword ptr ds:[esi].CMV2Dot3D__m_fTextureV
	fld		dword ptr ds:[edi].CMV2Dot3D__m_fTextureV
	fld		st(1)
	fsub	st,st(1)
	fxch
	fsub	st,st(3)
	fxch
	fmul	st,st(5)
	fxch	st(2)
	fsub	st,st(3)
	fxch
	fmul	st,st(4)
	fxch	st(3)
	ffree   st
	fincstp
	fmul	st,st(5)
		;st0 = TextureVdY, st1 = TextureVdY2,
		;st2 = TextureVdY1, st3 = 1/YCounter1,
		;st4 = 1/YCounter2, st5 = 1/YCounter

	fld		fScreenYError1
	fld		fScreenYError3
	fmul	st,st(3)
	fxch
	fmul	st,st(4)
	fxch
	fadd	dword ptr ds:[edi].CMV2Dot3D__m_fTextureV
	fxch
	fadd	dword ptr ds:[edx].CMV2Dot3D__m_fTextureV
	fxch

	fstp	fTextureV3
	fstp	fTextureV1
	fstp	fTextureVdY
	fstp	fTextureVdY2
	fstp	fTextureVdY1
;===>                                                                <===
;===>	TextureV DeltaY Calculation }                            	 <===
;===>                                                                <===


;===>                                                                <===
;===>	TextureUV DeltaX Calculation {                            	 <===
;===>                                                                <===
	fld		fMaxXCounterRZP
	fmul	fNum_2EXP20
	fld		fYCounter1
	fld		fTextureVdY
	fld		fTextureUdY
	fmul	st,st(2)
	fxch
	fmulp	st(2),st			; fmul stall (+ 1 Cycle)
	fxch
	fadd    dword ptr ds:[edx].CMV2Dot3D__m_fTextureV
	fxch
	fadd    dword ptr ds:[edx].CMV2Dot3D__m_fTextureU
	fxch
	fsub    dword ptr ds:[edi].CMV2Dot3D__m_fTextureV
	fxch
	fsub    dword ptr ds:[edi].CMV2Dot3D__m_fTextureU
	fxch
	fmul    st,st(2)
	fxch
	fmul	st,st(2)
	fxch

	fst		fTextureVdX
	fistp	dwTextureVdX
	fst		fTextureUdX
	fistp	dwTextureUdX
;===>                                                                <===
;===>	TextureUV DeltaX Calculation }                            	 <===
;===>                                                                <===

	ffree	st(3)
	ffree	st(2)
	ffree	st(1)
	ffree	st(0)

	mov		eax,dwDeltaScreenX1
	mov		ebx,dwDeltaScreenX
	mov		esi,eax
	mov		edi,ebx
	shl		eax,12
	shl		ebx,12
	sar		esi,20
	sar		edi,20
	add		esi,dwXmax
	add		edi,dwXmax
	mov		dwAdderScreenX1,esi
	mov		dwAdderScreenX2,edi
	mov		dwAdderScreenX1f,eax
	mov		dwAdderScreenX2f,ebx

	mov		eax,dwScreenX1
	mov		ebx,dwScreenX2
	mov		esi,eax
	mov		edi,ebx
	shl		eax,12
	shl		ebx,12
	sar		esi,20
	sar		edi,20
	add		esi,dwYOffset1
	add     edi,dwYOffset1

	mov		ebp,dwYCounter1
	mov		ecx,edi

	fld		fNum_2EXP20
	fld     fTextureVdY1
	fld		fTextureUdY1
	fld		fTextureV1
	fld		fTextureU1

@@PT1YloopPass1:
; eax = x1f (20 bit fraction)
; ebx = x2f (20 bit fraction)
; ecx =
; edx =
; esi = ScreenOffset + YOffset + x1
; edi = ScreenOffset + YOffset + x2

; st0 = TextureU<<16
; st1 = TextureV<<16
; st2 = dTextureU<<16
; st3 = dTextureV<<16
; st4 = fNum_2EXP20
	shr		eax,12
	push	eax

	fild	dword ptr [esp]
	shl		eax,12
	push	esi

	fld		st(5)
	fxch
	fsubp	st(1),st
	push	edi
	fmul	dword ptr fNum_2EXP_20

	add		ebx,dwAdderScreenX2f
	adc     edi,dwAdderScreenX2
	add		eax,dwAdderScreenX1f
	adc		esi,dwAdderScreenX1
	sub		esp,4

	fld		fTextureUdX
	fmul	st,st(1)
	fld		fTextureVdX
	fmul	st,st(2)
	fxch
	fadd	st,st(3)
	fxch
	fadd	st,st(4)
	fxch
	fistp	dword ptr ss:[esp + 12]			; TextureU
	fistp	dword ptr ss:[esp + 0]			; TextureV
	fcomp

	fadd	st,st(2)
	fxch

	fadd	st,st(3)
	fxch

	dec		ebp
	jnz		@@PT1YloopPass1
;stack:
;TextureV, ScreenX2Offset, ScreenX1Offset, TextureU
	ffree	st(4)
	ffree	st(3)
	ffree	st(2)
	ffree	st(1)
	ffree	st(0)

	mov		eax,dwDeltaScreenX2
	mov		esi,eax
	shl		eax,12
	sar		esi,20
	add		esi,dwXmax
	mov		dwAdderScreenX1,esi
	mov		dwAdderScreenX1f,eax

	mov		eax,dwScreenX3
	mov		esi,eax
	shl		eax,12
	sar		esi,20
	add		esi,dwYOffset2

	mov		ebp,dwYCounter2

	fld		fNum_2EXP20
	fld     fTextureVdY2
	fld		fTextureUdY2
	fld		fTextureV3
	fld		fTextureU3

@@PT1YloopPass2:
	shr		eax,12
	push	eax

	fild	dword ptr [esp]
	shl		eax,12
	push	esi

	fld		st(5)
	fxch
	fsubp	st(1),st
	push	edi
	fmul	dword ptr fNum_2EXP_20

	add		ebx,dwAdderScreenX2f
	adc     edi,dwAdderScreenX2
	add		eax,dwAdderScreenX1f
	adc		esi,dwAdderScreenX1
	sub		esp,4

	fld		fTextureUdX
	fmul	st,st(1)
	fld		fTextureVdX
	fmul	st,st(2)
	fxch
	fadd	st,st(3)
	fxch
	fadd	st,st(4)
	fxch
	fistp	dword ptr ss:[esp + 12]			; TextureU
	fistp	dword ptr ss:[esp + 0]			; TextureV
	fcomp

	fadd	st,st(2)
	fxch

	fadd	st,st(3)
	fxch

	dec		ebp
	jnz		@@PT1YloopPass2

	ffree	st(4)
	ffree	st(3)
	ffree	st(2)
	ffree	st(1)
	ffree	st(0)

	jmp		@@LinTYLoop

;=============>                <==============
;=============> Linear		   <==============
;=============> Polygon Type 2 <==============
;=============>                <==============
@@LinearPolygonType2:
	mov		eax,ds:[edx].CMV2Dot3DPos__m_fScreenYError
	mov		ebx,ds:[esi].CMV2Dot3DPos__m_fScreenYError
	mov     fScreenYError1,eax
	mov		fScreenYError2,ebx

	mov		edx,pDot1
	mov		esi,pDot2
	mov		edi,pDot3

;===>                                                                <===
;===>	TextureUV DeltaY Calculation {                            	 <===
;===>                                                                <===
	fld		dword ptr ds:[edi].CMV2Dot3D__m_fTextureU
	fsub    dword ptr ds:[edx].CMV2Dot3D__m_fTextureU
	fld		dword ptr ds:[edi].CMV2Dot3D__m_fTextureV
	fsub    dword ptr ds:[edx].CMV2Dot3D__m_fTextureV
	fxch
	fmul	st,st(2)
	fxch
	fmul	st,st(2)      				; fmul stall (+1 Cycle)
	fxch
	fld		fScreenYError1
	fld		st
	fmul	st,st(2)
	fxch
	fmul	st,st(3)                    ; fmul stall (+1 Cycle)
	fxch
		;st0 = ScreenYError1*DeltaTextureX
		;st1 = ScreenYError1*DeltaTextureY
		;st2 = DeltaTextureX, st3 = DeltaTextureY
		;st4 = 1/YCounter, st5 = 1/YCounter2, st6 = 1/YCounter1

	fadd	dword ptr ds:[edx].CMV2Dot3D__m_fTextureU
	fxch
	fadd	dword ptr ds:[edx].CMV2Dot3D__m_fTextureV
	fxch

	fstp	fTextureU1
	fstp	fTextureV1
	fstp	fTextureUdY
	fstp	fTextureVdY
;===>                                                                <===
;===>	TextureUV DeltaY Calculation }                            	 <===
;===>                                                                <===


;===>                                                                <===
;===>	TextureUV DeltaX Calculation {                            	 <===
;===>                                                                <===
	fld		fMaxXCounterRZP
	fmul	fNum_2EXP20
	fld		fYCounter1
	fld		fTextureVdY
	fld		fTextureUdY
		;st0 = DeltaTextureX1, st1 = DeltaTextureY1
		;st2 = YCounter1, st3 = 1/MaxXCounter
		;st4 = 1/YCounter, st5 = 1/YCounter2, st6 = 1/YCounter1

	fmul	st,st(2)
	fxch
	fmulp	st(2),st			; fmul stall (+ 1 Cycle)
	fadd    dword ptr ds:[edx].CMV2Dot3D__m_fTextureU
	fxch
	fadd    dword ptr ds:[edx].CMV2Dot3D__m_fTextureV
	fxch
	fsubr	dword ptr ds:[esi].CMV2Dot3D__m_fTextureU
	fxch
	fsubr	dword ptr ds:[esi].CMV2Dot3D__m_fTextureV
	fxch
	fmul    st,st(2)
	fxch
	fmul	st,st(2)
	fxch
		;st0 = TextureUdX
		;st1 = TextureVdX
		;st2 = 1/MaxXCounter
		;st3 = 1/YCounter, st4 = 1/YCounter2, st5 = 1/YCounter1

	fst		fTextureUdX
	fistp	dwTextureUdX
	fst		fTextureVdX
	fistp	dwTextureVdX

;===>                                                                <===
;===>	TextureUV DeltaX Calculation }                            	 <===
;===>                                                                <===

	ffree	st(3)
	ffree	st(2)
	ffree	st(1)
	ffree	st(0)

	mov		eax,dwDeltaScreenX
	mov		ebx,dwDeltaScreenX1
	mov		esi,eax
	mov		edi,ebx
	shl		eax,12
	shl		ebx,12
	sar		esi,20
	sar		edi,20
	add		esi,dwXmax
	add		edi,dwXmax
	mov		dwAdderScreenX1,esi
	mov		dwAdderScreenX2,edi
	mov		dwAdderScreenX1f,eax
	mov		dwAdderScreenX2f,ebx

	mov		eax,dwScreenX1
	mov		ebx,dwScreenX2
	mov		esi,eax
	mov		edi,ebx
	shl		eax,12
	shl		ebx,12
	sar		esi,20
	sar		edi,20
	add		esi,dwYOffset1
	add     edi,dwYOffset1

	mov		ebp,dwYCounter1

	fld		fNum_2EXP20
	fld     fTextureVdY
	fld		fTextureUdY
	fld		fTextureV1
	fld		fTextureU1

@@PT2YloopPass1:
	shr		eax,12
	push	eax

	fild	dword ptr [esp]
	shl		eax,12
	push	esi

	fld		st(5)
	fxch
	fsubp	st(1),st
	push	edi
	fmul	dword ptr fNum_2EXP_20

	add		ebx,dwAdderScreenX2f
	adc     edi,dwAdderScreenX2
	add		eax,dwAdderScreenX1f
	adc		esi,dwAdderScreenX1
	sub		esp,4

	fld		fTextureUdX
	fmul	st,st(1)
	fld		fTextureVdX
	fmul	st,st(2)
	fxch
	fadd	st,st(3)
	fxch
	fadd	st,st(4)
	fxch
	fistp	dword ptr ss:[esp + 12]			; TextureU
	fistp	dword ptr ss:[esp + 0]			; TextureV
	fcomp

	fadd	st,st(2)
	fxch

	fadd	st,st(3)
	fxch

	dec		ebp
	jnz		@@PT2YloopPass1
;stack:
;TextureV, ScreenX2Offset, ScreenX1Offset, TextureU


	mov		ebx,dwDeltaScreenX2
	mov		edi,ebx
	shl		ebx,12
	sar		edi,20
	add		edi,dwXmax
	mov		dwAdderScreenX2,edi
	mov		dwAdderScreenX2f,ebx

	mov		ebx,dwScreenX3
	mov		edi,ebx
	shl		ebx,12
	sar		edi,20
	add		edi,dwYOffset2

	mov		ebp,dwYCounter2

@@PT2YloopPass2:
	shr		eax,12
	push	eax

	fild	dword ptr [esp]
	shl		eax,12
	push	esi

	fld		st(5)
	fxch
	fsubp	st(1),st
	push	edi
	fmul	dword ptr fNum_2EXP_20

	add		ebx,dwAdderScreenX2f
	adc     edi,dwAdderScreenX2
	add		eax,dwAdderScreenX1f
	adc		esi,dwAdderScreenX1
	sub		esp,4

	fld		fTextureUdX
	fmul	st,st(1)
	fld		fTextureVdX
	fmul	st,st(2)
	fxch
	fadd	st,st(3)
	fxch
	fadd	st,st(4)
	fxch
	fistp	dword ptr ss:[esp + 12]			; TextureU
	fistp	dword ptr ss:[esp + 0]			; TextureV
	fcomp

	fadd	st,st(2)
	fxch

	fadd	st,st(3)
	fxch

	dec		ebp
	jnz		@@PT2YloopPass2

	ffree	st(4)
	ffree	st(3)
	ffree	st(2)
	ffree	st(1)
	ffree	st(0)

	jmp		@@LinTYLoop

;=============>                <==============
;=============> Linear		   <==============
;=============> Polygon Type 3 <==============
;=============>                <==============
@@LinearPolygonType3:
	mov		eax,ds:[edx].CMV2Dot3DPos__m_fScreenYError
	mov		ebx,ds:[esi].CMV2Dot3DPos__m_fScreenYError
	mov     fScreenYError1,eax
	mov		fScreenYError2,ebx

	mov		edx,pDot1
	mov		esi,pDot2
	mov		edi,pDot3

;===>                                                                <===
;===>	TextureUV DeltaY Calculation {                            	 <===
;===>                                                                <===
	fld		dword ptr ds:[edi].CMV2Dot3D__m_fTextureU
	fsub    dword ptr ds:[edx].CMV2Dot3D__m_fTextureU
	fld		dword ptr ds:[edi].CMV2Dot3D__m_fTextureV
	fsub    dword ptr ds:[edx].CMV2Dot3D__m_fTextureV
	fxch
	fmul	st,st(3)
	fxch
	fmul	st,st(3)      				; fmul stall (+1 Cycle)
	fxch
	fld		fScreenYError1
	fld		st
	fmul	st,st(2)
	fxch
	fmul	st,st(3)                    ; fmul stall (+1 Cycle)
	fxch
		;st0 = ScreenYError1*TextureUdY1
		;st1 = ScreenYError1*TextureVdY1
		;st2 = TextureUdY1, st3 = TextureVdY1
		;st4 = 1/YCounter2, st5 = 1/YCounter1

	fadd	dword ptr ds:[edx].CMV2Dot3D__m_fTextureU
	fxch
	fadd	dword ptr ds:[edx].CMV2Dot3D__m_fTextureV
	fxch

	fstp	fTextureU1
	fstp	fTextureV1
	fstp	fTextureUdY1
	fstp	fTextureVdY1

;===>                                                                <===
;===>	TextureUV DeltaY Calculation }                            	 <===
;===>                                                                <===


;===>                                                                <===
;===>	TextureUV DeltaX Calculation {                            	 <===
;===>                                                                <===
	fld		fMaxXCounterRZP
	fmul	fNum_2EXP20
	fld		fYCounter2
	fld		fTextureVdY1
	fld		fTextureUdY1
	fmul	st,st(2)
	fxch
	fmulp	st(2),st			; fmul stall (+ 1 Cycle)
	fadd    dword ptr ds:[edx].CMV2Dot3D__m_fTextureU
	fxch
	fadd    dword ptr ds:[edx].CMV2Dot3D__m_fTextureV
	fxch
	fsubr	dword ptr ds:[esi].CMV2Dot3D__m_fTextureU
	fxch
	fsubr	dword ptr ds:[esi].CMV2Dot3D__m_fTextureV
	fxch
		;st0 = TextureU2 - (YCounter2*TextureUdY1 + TextureU1)
		;st1 = TextureV2 - (YCounter2*TextureVdY1 + TextureV1)
		;st2 = 1/MaxXCounter
		;st3 = 1/YCounter2, st4 = 1/YCounter1

	fmul    st,st(2)
	fxch
	fmul	st,st(2)
	fxch

	fst		fTextureUdX
	fistp	dwTextureUdX
	fst		fTextureVdX
	fistp	dwTextureVdX
;===>                                                                <===
;===>	TextureUV DeltaX Calculation }                            	 <===
;===>                                                                <===

	ffree	st(2)
	ffree	st(1)
	ffree	st(0)

	mov		eax,dwDeltaScreenX1
	mov		ebx,dwDeltaScreenX2
	mov		esi,eax
	mov		edi,ebx
	shl		eax,12
	shl		ebx,12
	sar		esi,20
	sar		edi,20
	add		esi,dwXmax
	add		edi,dwXmax
	mov		dwAdderScreenX1,esi
	mov		dwAdderScreenX2,edi
	mov		dwAdderScreenX1f,eax
	mov		dwAdderScreenX2f,ebx

	mov		eax,dwScreenX1
	mov		ebx,dwScreenX2
	mov		esi,eax
	mov		edi,ebx
	shl		eax,12
	shl		ebx,12
	sar		esi,20
	sar		edi,20
	add		esi,dwYOffset1
	add     edi,dwYOffset1

	mov		ebp,dwYCounter

	fld		fNum_2EXP20
	fld     fTextureVdY1
	fld		fTextureUdY1
	fld		fTextureV1
	fld		fTextureU1

@@PT3YloopPass1:
	shr		eax,12
	push	eax

	fild	dword ptr [esp]
	shl		eax,12
	push	esi

	fld		st(5)
	fxch
	fsubp	st(1),st
	push	edi
	fmul	dword ptr fNum_2EXP_20

	add		ebx,dwAdderScreenX2f
	adc     edi,dwAdderScreenX2
	add		eax,dwAdderScreenX1f
	adc		esi,dwAdderScreenX1
	sub		esp,4

	fld		fTextureUdX
	fmul	st,st(1)
	fld		fTextureVdX
	fmul	st,st(2)
	fxch
	fadd	st,st(3)
	fxch
	fadd	st,st(4)
	fxch
	fistp	dword ptr ss:[esp + 12]			; TextureU
	fistp	dword ptr ss:[esp + 0]			; TextureV
	fcomp

	fadd	st,st(2)
	fxch

	fadd	st,st(3)
	fxch

	dec		ebp
	jnz		@@PT3YloopPass1

	ffree	st(4)
	ffree	st(3)
	ffree	st(2)
	ffree	st(1)
	ffree	st(0)

	jmp		@@LinTYLoop

;=============>                <==============
;=============> Linear		   <==============
;=============> Polygon Type 4 <==============
;=============>                <==============
@@LinearPolygonType4:
	mov		eax,ds:[edx].CMV2Dot3DPos__m_fScreenYError
	mov		ebx,ds:[esi].CMV2Dot3DPos__m_fScreenYError
	mov     fScreenYError1,eax
	mov		fScreenYError2,ebx

	mov		edx,pDot1
	mov		esi,pDot2
	mov		edi,pDot3

;===>                                                                <===
;===>	TextureUV DeltaY Calculation {                            	 <===
;===>                                                                <===
		;st0 = 1/YCounter13, st1 = 1/YCounter23

	fld		dword ptr ds:[edi].CMV2Dot3D__m_fTextureU
	fsub    dword ptr ds:[edx].CMV2Dot3D__m_fTextureU
	fld		dword ptr ds:[edi].CMV2Dot3D__m_fTextureV
	fsub    dword ptr ds:[edx].CMV2Dot3D__m_fTextureV
	fxch
	fmul	st,st(3)
	fxch
	fmul	st,st(3)      				; fmul stall (+1 Cycle)
	fxch
	fld		fScreenYError1
	fld		st
	fmul	st,st(2)
	fxch
	fmul	st,st(3)                    ; fmul stall (+1 Cycle)
	fxch
		;st0 = ScreenYError1*TextureUdY1
		;st1 = ScreenYError1*TextureVdY1
		;st2 = TextureUdY1, st3 = TextureVdY1
		;st4 = 1/YCounter2, st5 = 1/YCounter1

	fadd	dword ptr ds:[edx].CMV2Dot3D__m_fTextureU
	fxch
	fadd	dword ptr ds:[edx].CMV2Dot3D__m_fTextureV
	fxch

	fstp	fTextureU1
	fstp	fTextureV1
	fstp	fTextureUdY1
	fstp	fTextureVdY1
;===>                                                                <===
;===>	TextureUV DeltaY Calculation }                            	 <===
;===>                                                                <===


;===>                                                                <===
;===>	TextureUV DeltaX Calculation {                            	 <===
;===>                                                                <===
	fld		fMaxXCounterRZP
	fmul	fNum_2EXP20
	fld		fYCounter
	fld		fTextureVdY1
	fld		fTextureUdY1
	fmul	st,st(2)
	fxch
	fmulp	st(2),st			; fmul stall (+ 1 Cycle)
	fadd    dword ptr ds:[edx].CMV2Dot3D__m_fTextureU
	fxch
	fadd    dword ptr ds:[edx].CMV2Dot3D__m_fTextureV
	fxch
	fsubr	dword ptr ds:[esi].CMV2Dot3D__m_fTextureU
	fxch
	fsubr	dword ptr ds:[esi].CMV2Dot3D__m_fTextureV
	fxch
		;st0 = TextureU2 - (YCounter*TextureUdY1 + TextureU1)
		;st1 = TextureV2 - (YCounter*TextureVdY1 + TextureV1)
		;st2 = 1/MaxXCounter
		;st3 = 1/YCounter2, st4 = 1/YCounter1

	fmul    st,st(2)
	fxch
	fmul	st,st(2)
	fxch

	fst		fTextureUdX
	fistp	dwTextureUdX
	fst		fTextureVdX
	fistp	dwTextureVdX

;===>                                                                <===
;===>	TextureUV DeltaX Calculation }                            	 <===
;===>                                                                <===

	ffree	st(2)
	ffree	st(1)
	ffree	st(0)

	mov		eax,dwDeltaScreenX1
	mov		ebx,dwDeltaScreenX2
	mov		esi,eax
	mov		edi,ebx
	shl		eax,12
	shl		ebx,12
	sar		esi,20
	sar		edi,20
	add		esi,dwXmax
	add		edi,dwXmax
	mov		dwAdderScreenX1,esi
	mov		dwAdderScreenX2,edi
	mov		dwAdderScreenX1f,eax
	mov		dwAdderScreenX2f,ebx

	mov		eax,dwScreenX1
	mov		ebx,dwScreenX2
	mov		esi,eax
	mov		edi,ebx
	shl		eax,12
	shl		ebx,12
	sar		esi,20
	sar		edi,20
	add		esi,dwYOffset1
	add     edi,dwYOffset1

	mov		ebp,dwYCounter

	fld		fNum_2EXP20
	fld     fTextureVdY1
	fld		fTextureUdY1
	fld		fTextureV1
	fld		fTextureU1

@@PT4YloopPass1:
	shr		eax,12
	push	eax

	fild	dword ptr [esp]
	shl		eax,12
	push	esi

	fld		st(5)
	fxch
	fsubp	st(1),st
	push	edi
	fmul	dword ptr fNum_2EXP_20

	add		ebx,dwAdderScreenX2f
	adc     edi,dwAdderScreenX2
	add		eax,dwAdderScreenX1f
	adc		esi,dwAdderScreenX1
	sub		esp,4

	fld		fTextureUdX
	fmul	st,st(1)
	fld		fTextureVdX
	fmul	st,st(2)
	fxch
	fadd	st,st(3)
	fxch
	fadd	st,st(4)
	fxch
	fistp	dword ptr ss:[esp + 12]			; TextureU
	fistp	dword ptr ss:[esp + 0]			; TextureV
	fcomp

	fadd	st,st(2)
	fxch

	fadd	st,st(3)
	fxch

	dec		ebp
	jnz		@@PT4YloopPass1

	ffree	st(4)
	ffree	st(3)
	ffree	st(2)
	ffree	st(1)
	ffree	st(0)

	jmp		@@LinTYLoop


@@LinTYLoop:
	mov		eax,dwTextureUdX
	mov		edx,dwTextureVdX

	rol		eax,16
	rol		edx,16			; U!

	mov		esi,edx
	mov		ebx,ss:[esp + 12]	; TextureU
	mov		ebp,eax

	rol		ebx,16          ;
	and		esi,0ffff0000h	; esi init.	(!)
	mov		bh,dl			;
	mov		edx,pcTexture	; (!)

	and		ebp,0ffff0000h	; ebp init.	(!)
	mov		dl,bl           ; (!)
	mov		bl,al           ; ebx init.

	pop		ecx				; TextureV
	pop		eax				; ScreenX2Offset
	pop		edi     		; ScreenX1Offset, edi init.

	rol		ecx,16			; ecx init.
	add		esp,4
	sub		eax,edi			; eax init.
	mov		dh,cl			; edx init.

	add 	iPixelCounter,eax		;**************** profiling

	test	eax,eax
	jle		@@NoLinTXLoop
	mov		dwXCounter,eax

	mov		cx,ax
	push	ebp

@@LinTXLoop:
;*eax = counter
;*ebx =	txf				|dty	|dtx
;*ecx = tyf				|       |col
;*edx = pTexture		|ty		|tx
;*esi = dtyf
;*edi = destination
;*ebp = dtxf
	push	edi
	push	ecx
	push	edx
	push	ebx

	add  	ebx,ebp
	adc		dl,bl
	add		ecx,esi
	adc		dh,bh

	inc		edi

	dec		cx
	jnz		@@LinTXLoop

	mov		ebp,dwXCounter

@@LinearFltLoop:
; eax = pTexture>>2	|ty		|tx
; ebx = pTexture>>2	|ty		|tx
; ecx =
; edx =
; esi =
; edi =
; ebp = XCounter

	pop		esi							; txf
	pop		ecx							; pTexture>>2	|ty		|tx

	shr		esi,29
	mov		edx,ecx
	mov		edi,offset LinearFltJumpTableX
	inc     dl

	jmp		ds:[edi + 4*esi]

LinearFltCaseX0:
	pop		ebx						; tyf
	dec		dl
	shr		ebx,29
	inc		dh
	mov		eax,ds:[4*ecx]
	mov		ecx,ds:[4*edx]

	lea		eax,[8*eax]
	lea		ebx,[4*ebx]
	lea		ecx,[8*ecx]
	add		ebx,offset LinearFltJumpTableY

	jmp		[ebx]

LinearFltCaseX1:
	mov		eax,ds:[4*ecx]				; 1. Texel
	mov		esi,ds:[4*edx]				; 2. Texel
	mov		ebx,eax
	mov		edi,esi

	inc		ch
	inc		dh

	and		eax,0f8f8f8f8h
	and		esi,0f8f8f8f8h
	and		ebx,007070707h
	and		edi,007070707h

	sub		esi,eax
	sub		edi,ebx

	lea		eax,[8*eax]
	lea		ebx,[8*ebx]

	add		eax,esi
	add		ebx,edi

	mov		ecx,ds:[4*ecx]				; 3. Texel
	and		ebx,0f8f8f8f8h
	mov		esi,ds:[4*edx]				; 4. Texel
	add		eax,ebx

	mov		edx,ecx
	mov		edi,esi

	and		ecx,0f8f8f8f8h
	and		esi,0f8f8f8f8h
	and		edx,007070707h
	and		edi,007070707h

	sub		esi,ecx
	sub		edi,edx

	pop		ebx						; tyf
	lea		ecx,[8*ecx]
	shr		ebx,29
	lea		edx,[8*edx]


	add		ecx,esi
	add		edx,edi
	lea		ebx,[4*ebx]

	and		edx,0f8f8f8f8h
	add		ebx,offset LinearFltJumpTableY
	add		ecx,edx
	jmp		[ebx]

LinearFltCaseX2:
	mov		eax,ds:[4*ecx]				; 1. Texel
	mov		esi,ds:[4*edx]				; 2. Texel
	mov		ebx,eax
	mov		edi,esi

	inc		ch
	inc		dh

	and		eax,0f8f8f8f8h
	and		ebx,007070707h
	and		esi,0f8f8f8f8h
	and		edi,007070707h

	lea		eax,[2*eax + eax]
	lea		ebx,[2*ebx + ebx]
	lea		esi,[2*esi]
	lea		edi,[2*edi]
	lea		eax,[2*eax]
	lea		ebx,[2*ebx]

	add		eax,esi
	add		ebx,edi

	mov		ecx,ds:[4*ecx]				; 3. Texel
	and		ebx,0f8f8f8f8h
	mov		esi,ds:[4*edx]				; 4. Texel
	add		eax,ebx

	mov		edx,ecx
	mov		edi,esi

	and		ecx,0f8f8f8f8h
	and		edx,007070707h
	and		esi,0f8f8f8f8h
	and		edi,007070707h

	lea		ecx,[2*ecx + ecx]
	lea		edx,[2*edx + edx]
	lea		esi,[2*esi]
	lea		edi,[2*edi]
	pop		ebx						; tyf
	lea		ecx,[2*ecx]
	shr		ebx,29
	lea		edx,[2*edx]


	add		ecx,esi
	add		edx,edi
	lea		ebx,[4*ebx]

	and		edx,0f8f8f8f8h
	add		ebx,offset LinearFltJumpTableY
	add		ecx,edx
	jmp		[ebx]

LinearFltCaseX3:
	mov		eax,ds:[4*ecx]				; 1. Texel
	mov		esi,ds:[4*edx]				; 2. Texel
	mov		ebx,eax
	mov		edi,esi

	inc		ch
	inc		dh

	and		eax,0f8f8f8f8h
	and		ebx,007070707h
	and		esi,0f8f8f8f8h
	and		edi,007070707h

	lea		eax,[4*eax + eax]
	lea		ebx,[4*ebx + ebx]
	lea		esi,[2*esi + esi]
	lea		edi,[2*edi + edi]

	add		eax,esi
	add		ebx,edi

	mov		ecx,ds:[4*ecx]				; 3. Texel
	and		ebx,0f8f8f8f8h
	mov		esi,ds:[4*edx]				; 4. Texel
	add		eax,ebx

	mov		edx,ecx
	mov		edi,esi

	and		ecx,0f8f8f8f8h
	and		edx,007070707h
	and		esi,0f8f8f8f8h
	and		edi,007070707h

	lea		ecx,[4*ecx + ecx]
	lea		edx,[4*edx + edx]
	pop		ebx						; tyf
	lea		esi,[2*esi + esi]
	shr		ebx,29
	lea		edi,[2*edi + edi]

	add		ecx,esi
	add		edx,edi
	lea		ebx,[4*ebx]

	and		edx,0f8f8f8f8h
	add		ebx,offset LinearFltJumpTableY
	add		ecx,edx
	jmp		[ebx]

LinearFltCaseX4:
	mov		eax,ds:[4*ecx]				; 1. Texel
	mov		esi,ds:[4*edx]				; 2. Texel
	mov		ebx,eax
	mov		edi,esi

	inc		ch
	inc		dh

	and		eax,0f8f8f8f8h
	and		ebx,007070707h
	and		esi,0f8f8f8f8h
	and		edi,007070707h

	lea		eax,[4*eax]
	lea		ebx,[4*ebx]
	lea		esi,[4*esi]
	lea		edi,[4*edi]

	add		eax,esi
	add		ebx,edi

	mov		ecx,ds:[4*ecx]				; 3. Texel
	and		ebx,0f8f8f8f8h
	mov		esi,ds:[4*edx]				; 4. Texel
	add		eax,ebx

	mov		edx,ecx
	mov		edi,esi

	and		ecx,0f8f8f8f8h
	and		edx,007070707h
	and		esi,0f8f8f8f8h
	and		edi,007070707h

	lea		ecx,[4*ecx]
	lea		edx,[4*edx]
	pop		ebx						; tyf
	lea		esi,[4*esi]
	shr		ebx,29
	lea		edi,[4*edi]

	add		ecx,esi
	add		edx,edi
	lea		ebx,[4*ebx]

	and		edx,0f8f8f8f8h
	add		ebx,offset LinearFltJumpTableY
	add		ecx,edx
	jmp		[ebx]

LinearFltCaseX5:
	mov		eax,ds:[4*ecx]				; 1. Texel
	mov		esi,ds:[4*edx]				; 2. Texel
	mov		ebx,eax
	mov		edi,esi

	inc		ch
	inc		dh

	and		eax,0f8f8f8f8h
	and		ebx,007070707h
	and		esi,0f8f8f8f8h
	and		edi,007070707h

	lea		eax,[2*eax + eax]
	lea		ebx,[2*ebx + ebx]
	lea		esi,[4*esi + esi]
	lea		edi,[4*edi + edi]

	add		eax,esi
	add		ebx,edi

	mov		ecx,ds:[4*ecx]				; 3. Texel
	and		ebx,0f8f8f8f8h
	mov		esi,ds:[4*edx]				; 4. Texel
	add		eax,ebx

	mov		edx,ecx
	mov		edi,esi

	and		ecx,0f8f8f8f8h
	and		edx,007070707h
	and		esi,0f8f8f8f8h
	and		edi,007070707h

	lea		ecx,[2*ecx + ecx]
	lea		edx,[2*edx + edx]
	pop		ebx						; tyf
	lea		esi,[4*esi + esi]
	shr		ebx,29
	lea		edi,[4*edi + edi]

	add		ecx,esi
	add		edx,edi
	lea		ebx,[4*ebx]

	and		edx,0f8f8f8f8h
	add		ebx,offset LinearFltJumpTableY
	add		ecx,edx
	jmp		[ebx]

LinearFltCaseX6:
	mov		eax,ds:[4*ecx]				; 1. Texel
	mov		esi,ds:[4*edx]				; 2. Texel
	mov		ebx,eax
	mov		edi,esi

	inc		ch
	inc		dh

	and		esi,0f8f8f8f8h
	and		edi,007070707h
	and		eax,0f8f8f8f8h
	and		ebx,007070707h

	lea		esi,[2*esi + esi]
	lea		edi,[2*edi + edi]
	lea		eax,[2*eax]
	lea		ebx,[2*ebx]
	lea		esi,[2*esi]
	lea		edi,[2*edi]

	add		eax,esi
	add		ebx,edi

	mov		ecx,ds:[4*ecx]				; 3. Texel
	and		ebx,0f8f8f8f8h
	mov		esi,ds:[4*edx]				; 4. Texel
	add		eax,ebx

	mov		edx,ecx
	mov		edi,esi

	and		esi,0f8f8f8f8h
	and		edi,007070707h
	and		ecx,0f8f8f8f8h
	and		edx,007070707h

	lea		esi,[2*esi + esi]
	lea		edi,[2*edi + edi]
	lea		ecx,[2*ecx]
	lea		edx,[2*edx]
	pop		ebx						; tyf
	lea		esi,[2*esi]
	shr		ebx,29
	lea		edi,[2*edi]

	add		ecx,esi
	add		edx,edi
	lea		ebx,[4*ebx]

	and		edx,0f8f8f8f8h
	add		ebx,offset LinearFltJumpTableY
	add		ecx,edx
	jmp		[ebx]

LinearFltCaseX7:
	mov		eax,ds:[4*ecx]				; 1. Texel
	mov		esi,ds:[4*edx]				; 2. Texel
	mov		ebx,eax
	mov		edi,esi

	inc		ch
	inc		dh

	and		eax,0f8f8f8f8h
	and		esi,0f8f8f8f8h
	and		ebx,007070707h
	and		edi,007070707h

	sub		eax,esi
	sub		ebx,edi

	lea		esi,[8*esi]
	lea		edi,[8*edi]

	add		eax,esi
	add		ebx,edi

	mov		ecx,ds:[4*ecx]				; 3. Texel
	and		ebx,0f8f8f8f8h
	mov		esi,ds:[4*edx]				; 4. Texel
	add		eax,ebx

	mov		edx,ecx
	mov		edi,esi

	and		ecx,0f8f8f8f8h
	and		esi,0f8f8f8f8h
	and		edx,007070707h
	and		edi,007070707h

	sub		ecx,esi
	sub		edx,edi

	pop		ebx						; tyf
	lea		esi,[8*esi]
	shr		ebx,29
	lea		edi,[8*edi]

	add		ecx,esi
	add		edx,edi
	lea		ebx,[4*ebx]

	and		edx,0f8f8f8f8h
	add		ebx,offset LinearFltJumpTableY
	add		ecx,edx
	jmp		[ebx]

LinearFltCaseY0:
	lea		eax,[eax*8]
	pop		edi
	shr		eax,6

	and		eax,0fefefefeh
	shr		eax,1
	mov		dwTemp,eax
	mov		eax,ds:[edi*4]
	and		eax,0fefefefeh
	shr		eax,1
	add		eax,dwTemp

	dec		ebp
	mov		ds:[4*edi],eax

	jnz		@@LinearFltLoop
	jmp		@@LinearFltDone

LinearFltCaseY1:
	mov		ebx,eax
	mov		edx,ecx
	and		eax,0c7c7c7c7h
	and		ecx,0c7c7c7c7h
	and		ebx,038383838h
	and		edx,038383838h

	sub		ecx,eax
	sub		edx,ebx

	lea		eax,[8*eax]
	lea		ebx,[8*ebx]

	add		eax,ecx
	add		ebx,edx

	and		ebx,0c7c7c7c7h
	add		eax,ebx

	pop		edi
	shr		eax,6

	and		eax,0fefefefeh
	shr		eax,1
	mov		dwTemp,eax
	mov		eax,ds:[edi*4]
	and		eax,0fefefefeh
	shr		eax,1
	add		eax,dwTemp

	dec		ebp
	mov		ds:[4*edi],eax

	jnz		@@LinearFltLoop
	jmp		@@LinearFltDone

LinearFltCaseY2:
	mov		ebx,eax
	mov		edx,ecx

	and		eax,0c7c7c7c7h
	and		ebx,038383838h
	and		ecx,0c7c7c7c7h
	and		edx,038383838h

	lea		eax,[2*eax + eax]
	lea		ebx,[2*ebx + ebx]
	lea		ecx,[2*ecx]
	lea		edx,[2*edx]
	lea		eax,[2*eax]
	lea		ebx,[2*ebx]

	add		eax,ecx
	add		ebx,edx

	and		ebx,0c7c7c7c7h
	add		eax,ebx

	pop		edi
	shr		eax,6

	and		eax,0fefefefeh
	shr		eax,1
	mov		dwTemp,eax
	mov		eax,ds:[edi*4]
	and		eax,0fefefefeh
	shr		eax,1
	add		eax,dwTemp

	dec		ebp
	mov		ds:[4*edi],eax

	jnz		@@LinearFltLoop
	jmp		@@LinearFltDone

LinearFltCaseY3:
	mov		ebx,eax
	mov		edx,ecx
	and		eax,0c7c7c7c7h
	and		ebx,038383838h
	and		ecx,0c7c7c7c7h
	and		edx,038383838h

	lea		eax,[4*eax + eax]
	lea		ebx,[4*ebx + ebx]
	lea		ecx,[2*ecx + ecx]
	lea		edx,[2*edx + edx]

	add		eax,ecx
	add		ebx,edx

	and		ebx,0c7c7c7c7h
	add		eax,ebx

	pop		edi
	shr		eax,6

	and		eax,0fefefefeh
	shr		eax,1
	mov		dwTemp,eax
	mov		eax,ds:[edi*4]
	and		eax,0fefefefeh
	shr		eax,1
	add		eax,dwTemp

	dec		ebp
	mov		ds:[4*edi],eax

	jnz		@@LinearFltLoop
	jmp		@@LinearFltDone

LinearFltCaseY4:
	mov		ebx,eax
	mov		edx,ecx
	and		eax,0c7c7c7c7h
	and		ebx,038383838h
	and		ecx,0c7c7c7c7h
	and		edx,038383838h

	lea		eax,[4*eax]
	lea		ebx,[4*ebx]
	lea		ecx,[4*ecx]
	lea		edx,[4*edx]

	add		eax,ecx
	add		ebx,edx

	and		ebx,0c7c7c7c7h
	add		eax,ebx

	pop		edi
	shr		eax,6

	and		eax,0fefefefeh
	shr		eax,1
	mov		dwTemp,eax
	mov		eax,ds:[edi*4]
	and		eax,0fefefefeh
	shr		eax,1
	add		eax,dwTemp

	dec		ebp
	mov		ds:[4*edi],eax

	jnz		@@LinearFltLoop
	jmp		@@LinearFltDone

LinearFltCaseY5:
	mov		ebx,eax
	mov		edx,ecx
	and		eax,0c7c7c7c7h
	and		ebx,038383838h
	and		ecx,0c7c7c7c7h
	and		edx,038383838h

	lea		eax,[2*eax + eax]
	lea		ebx,[2*ebx + ebx]
	lea		ecx,[4*ecx + ecx]
	lea		edx,[4*edx + edx]

	add		eax,ecx
	add		ebx,edx

	and		ebx,0c7c7c7c7h
	add		eax,ebx

	pop		edi
	shr		eax,6

	and		eax,0fefefefeh
	shr		eax,1
	mov		dwTemp,eax
	mov		eax,ds:[edi*4]
	and		eax,0fefefefeh
	shr		eax,1
	add		eax,dwTemp

	dec		ebp
	mov		ds:[4*edi],eax

	jnz		@@LinearFltLoop
	jmp		@@LinearFltDone

LinearFltCaseY6:
	mov		ebx,eax
	mov		edx,ecx
	and		ecx,0c7c7c7c7h
	and		edx,038383838h
	and		eax,0c7c7c7c7h
	and		ebx,038383838h

	lea		ecx,[2*ecx + ecx]
	lea		edx,[2*edx + edx]
	lea		eax,[2*eax]
	lea		ebx,[2*ebx]
	lea		ecx,[2*ecx]
	lea		edx,[2*edx]

	add		eax,ecx
	add		ebx,edx

	and		ebx,0c7c7c7c7h
	add		eax,ebx

	pop		edi
	shr		eax,6

	and		eax,0fefefefeh
	shr		eax,1
	mov		dwTemp,eax
	mov		eax,ds:[edi*4]
	and		eax,0fefefefeh
	shr		eax,1
	add		eax,dwTemp

	dec		ebp
	mov		ds:[4*edi],eax

	jnz		@@LinearFltLoop
	jmp		@@LinearFltDone

LinearFltCaseY7:
	mov		ebx,eax
	mov		edx,ecx
	and		eax,0c7c7c7c7h
	and		ecx,0c7c7c7c7h
	and		ebx,038383838h
	and		edx,038383838h

	sub		eax,ecx
	sub		ebx,edx

	lea		ecx,[8*ecx]
	lea		edx,[8*edx]

	add		eax,ecx
	add		ebx,edx

	and		ebx,0c7c7c7c7h
	add		eax,ebx

	pop		edi
	shr		eax,6

	and		eax,0fefefefeh
	shr		eax,1
	mov		dwTemp,eax
	mov		eax,ds:[edi*4]
	and		eax,0fefefefeh
	shr		eax,1
	add		eax,dwTemp

	dec		ebp
	mov		ds:[4*edi],eax

	jnz		@@LinearFltLoop
@@LinearFltDone:

	pop		ebp

@@NoLinTXLoop:
	dec		dwYCounter
	jnz		@@LinTYLoop

@@Done:

	ret
endp MV2DrawPolygonTfTrASM



END