;/*
.486

.model flat

LOCALS

.data
fpucw						dw ?
oldfpucw					dw ?
oh_low 						dd ?
co_low						dd ?
co_hi						dd ?

align 32
pMV2Camera					dd ?
ppMV2Dot3DPos				dd ?
dwNumDots					dd ?

fCameraX					dd ?
fCameraY					dd ?
fCameraZ					dd ?
fCameraFX					dd ?
fCameraFY					dd ?
fCameraFZ					dd ?
fCameraRX					dd ?
fCameraRY					dd ?
fCameraRZ					dd ?
fCameraDX					dd ?
fCameraDY					dd ?
fCameraDZ					dd ?

dwClipXmin					dd ?
dwClipYmin					dd ?
dwClipXmax					dd ?
dwClipYmax					dd ?

dwXmax						dd ?
dwYmax						dd ?
fXmax						dd ?
fYmax						dd ?
fXmaxDiv2					dd ?
fYmaxDiv2					dd ?

fScreenFactor				dd ?
fAspectRatio				dd 1.333333333

fNum_1						dd 1.0
fNum_01						dd 0.1
fNum_05						dd 0.5
fNum_10Exp_10				dd 0.0000000001
fNum_2Exp20					dd 1048576.0

fTemp						dd ?
lTemp						dd ?
wTemp						dw ?
align 4
bTemp						db ?
align 4



.code


include m2structs.inc








public MESEtoMV2Dot3DPosProjectionASM
MESEtoMV2Dot3DPosProjectionASM proc
	ARG _pMV2Camera:DWORD, _ppMV2Dot3DPos:DWORD, _dwNumDots:DWORD
	push	ebp
	mov		ebp,esp
	pushad


;****
	fstcw   oldfpucw
	mov     ax,oldfpucw
	and     eax,0fcffh
	and		eax,0fbffh
	or		eax,00800h
	mov     fpucw,ax
	fldcw   fpucw
;****

	mov		eax,_pMV2Camera
	mov		ebx,_ppMV2Dot3DPos
	mov		ecx,_dwNumDots

	mov		pMV2Camera,eax
	mov		ppMV2Dot3DPos,ebx
	mov		dwNumDots,ecx

	mov     esi,pMV2Camera
	mov		eax,ds:[esi].CMV2Camera__m_iXmax
	mov		ebx,ds:[esi].CMV2Camera__m_iYmax

	mov		dwXmax,eax
	mov		dwYmax,ebx
	fild	dwXmax
	fild	dwYmax
	fstp	fYmax
	fstp	fXmax

	shr		eax,1
	shr		ebx,1

	mov		lTemp,eax
	fild	lTemp
	fstp	fXmaxDiv2

	mov		lTemp,ebx
	fild	lTemp
	fstp	fYmaxDiv2

	mov		eax,ds:[esi].CMV2Camera__m_iClipXmin
	mov		ebx,ds:[esi].CMV2Camera__m_iClipXmax
	mov		ecx,ds:[esi].CMV2Camera__m_iClipYmin
	mov		edx,ds:[esi].CMV2Camera__m_iClipYmax

	inc		eax					; wegen des clippings
	inc		ecx

	mov		dwClipXmin,eax
	mov		dwClipXmax,ebx
	mov		dwClipYmin,ecx
	mov		dwClipYmax,edx

	mov		eax,ds:[esi].CMV2Camera__m_Pos__m_fX
	mov		ebx,ds:[esi].CMV2Camera__m_Pos__m_fY
	mov		ecx,ds:[esi].CMV2Camera__m_Pos__m_fZ
	mov     fCameraX,eax
	mov		fCameraY,ebx
	mov		fCameraZ,ecx

	mov		eax,ds:[esi].CMV2Camera__m_Front__m_fX
	mov		ebx,ds:[esi].CMV2Camera__m_Front__m_fY
	mov		ecx,ds:[esi].CMV2Camera__m_Front__m_fZ
	mov     fCameraFX,eax
	mov		fCameraFY,ebx
	mov		fCameraFZ,ecx

	fld1
	fstp	fScreenFactor

;	fld     ds:[esi].CMV2Camera__m_fScreenFactor

	fld		fScreenFactor
	fld		ds:[esi].CMV2Camera__m_Right__m_fX
	fmul	st,st(1)
		;st0 = CameraRX*ScreenFactor, st1 = ScreenFactor

	fld		ds:[esi].CMV2Camera__m_Right__m_fY
	fmul	st,st(2)
	fld		ds:[esi].CMV2Camera__m_Right__m_fZ
	fmul	st,st(3)
		;st0 = CameraRZ*ScreenFactor, st1 = CameraRY*ScreenFactor
		;st2 = CameraRX*ScreenFactor, st3 = ScreenFactor

	fld		ds:[esi].CMV2Camera__m_Down__m_fX
	fmul	st,st(4)
	fld		ds:[esi].CMV2Camera__m_Down__m_fY
	fmul	st,st(5)
	fld		ds:[esi].CMV2Camera__m_Down__m_fZ
	fmul	st,st(6)
	fxch	st(2)
		;st0 = CameraDX*ScreenFactor, st1 = CameraDY*ScreenFactor
		;st2 = CameraDZ*ScreenFactor, st3 = CameraRZ*ScreenFactor
		;st4 = CameraRY*ScreenFactor, st5 = CameraRX*ScreenFactor
		;st6 = ScreenFactor

	fstp	fCameraDX
	fstp	fCameraDY
	fstp	fCameraDZ
	fstp	fCameraRZ
	fstp	fCameraRY
	fstp	fCameraRX
	ffree	st(0)

	mov		edx,ppMV2Dot3DPos
	mov		ebp,dwNumDots
	mov		edi,ds:[edx]

@@DotLoop:
	fld		DWORD PTR ds:[edi].CMV2Dot3DPos__m_Pos__m_fX
;	fsub	fCameraX
		;st0 = VectorX

	fld		fCameraFX
	fmul	st,st(1)
		;st0 = fCameraFX*VectorX, st1 = VectorX

	fld		fCameraRX
	fmul	st,st(2)
		;st0 = fCameraRX*VectorX, st1 = fCameraFX*VectorX
		;st2 = VectorX

	fld		fCameraDX
	fmul	st,st(3)
	ffree	st(3)
		;st0 = fCameraDX*VectorX, st1 = fCameraRX*VectorX
		;st2 = fCameraFX*VectorX

	fld		DWORD PTR ds:[edi].CMV2Dot3DPos__m_Pos__m_fY
;	fsub	fCameraY
		;st0 = VectorY, st1 = fCameraDX*VectorX
		;st2 = fCameraRX*VectorX, st3 = fCameraFX*VectorX

	fld		fCameraFY
	fmul	st,st(1)
		;st0 = fCameraFY*VectorY, st1 = VectorY
		;st1 = fCameraDX*VectorX
		;st2 = fCameraRX*VectorX, st3 = fCameraFX*VectorX

	fld		fCameraRY
	fmul	st,st(2)
		;st0 = fCameraRY*VectorY, st1 = fCameraFY*VectorY
		;st2 = VectorY, st3 = fCameraDX*VectorX
		;st4 = fCameraRX*VectorX, st5 = fCameraFX*VectorX

	fld		fCameraDY
	fmul	st,st(3)
	fxch	st(2)
		;st0 = fCameraFY*VectorY, st1 = fCameraRY*VectorY
		;st2 = fCameraDY*VectorY, st3 = VectorY
		;st4 = fCameraDX*VectorX
		;st5 = fCameraRX*VectorX, st6 = fCameraFX*VectorX

	faddp	st(6),st
		;st0 = fCameraRY*VectorY, st1 = fCameraDY*VectorY
		;st2 = VectorY, st3 = fCameraDX*VectorX
		;st4 = fCameraRX*VectorX
		;st5 = fCameraFX*VectorX + fCameraFY*VectorY

	faddp	st(4),st
		;st0 = fCameraDY*VectorY
		;st1 = VectorY, st2 = fCameraDX*VectorX
		;st3 = fCameraRX*VectorX + fCameraRY*VectorY
		;st4 = fCameraFX*VectorX + fCameraFY*VectorY

	faddp	st(2),st
	fxch	st(3)
	ffree	st(3)
		;st0 = fCameraFX*VectorX + fCameraFY*VectorY
		;st1 = fCameraDX*VectorX + fCameraDY*VectorY
		;st2 = fCameraRX*VectorX + fCameraRY*VectorY

	fld		DWORD PTR ds:[edi].CMV2Dot3DPos__m_Pos__m_fZ
;	fsub	fCameraZ
		;st0 = VectorZ
		;st1 = fCameraFX*VectorX + fCameraFY*VectorY
		;st2 = fCameraDX*VectorX + fCameraDY*VectorY
		;st3 = fCameraRX*VectorX + fCameraRY*VectorY

	fld		fCameraFZ
	fmul	st,st(1)
		;st0 = fCameraZ*VectorZ, st1 = VectorZ
		;st2 = fCameraFX*VectorX + fCameraFY*VectorY
		;st3 = fCameraDX*VectorX + fCameraDY*VectorY
		;st4 = fCameraRX*VectorX + fCameraRY*VectorY

	fld     fCameraRZ
	fmul	st,st(2)
		;st0 = fCameraRZ*VectorZ, st1 = fCameraFZ*VectorZ
		;st2 = VectorZ
		;st3 = fCameraFX*VectorX + fCameraFY*VectorY
		;st4 = fCameraDX*VectorX + fCameraDY*VectorY
		;st5 = fCameraRX*VectorX + fCameraRY*VectorY

	fld		fCameraDZ
	fmul	st,st(3)
	fxch	st(2)
		;st0 = fCameraFZ*VectorZ, st1 = fCameraRZ*VectorZ
		;st2 = fCameraDZ*VectorZ, st3 = VectorZ
		;st4 = fCameraFX*VectorX + fCameraFY*VectorY
		;st5 = fCameraDX*VectorX + fCameraDY*VectorY
		;st6 = fCameraRX*VectorX + fCameraRY*VectorY

	faddp	st(4),st
		;st0 = fCameraRZ*VectorZ
		;st1 = fCameraDZ*VectorZ, st2 = VectorZ
		;st3 = fProjectionF
		;st4 = fCameraDX*VectorX + fCameraDY*VectorY
		;st5 = fCameraRX*VectorX + fCameraRY*VectorY

	faddp	st(5),st
		;st0 = fCameraDZ*VectorZ, st1 = VectorZ
		;st2 = fProjectionF, st3 = fCameraRX*VectorX + fCameraRY*VectorY
		;st4 = fProjectionD

	faddp	st(3),st
		;st0 = VectorZ
		;st1 = fProjectionF = ZNew, st2 = fProjectionD = YNew
		;st3 = fProjectionR = XNew

	ffree	st(0)
	fincstp
		;st0 = fProjectionF = ZNew, st1 = fProjectionD = YNew
		;st2 = fProjectionR = XNew


	fadd	fCameraZ
	fxch
	fadd	fCameraY
	fxch
	fxch	st(2)
	fadd	fCameraX
	fxch	st(2)

	fxch
	fst		DWORD PTR ds:[edi].CMV2Dot3DPos__m_TransfPos__m_fY
	fxch
	fxch	st(2)
	fst		DWORD PTR ds:[edi].CMV2Dot3DPos__m_TransfPos__m_fX
	fxch	st(2)

	fcom	DWORD PTR fNum_01
	fnstsw	ax					; tasm 3.0 bug (= fstsw)

; ClippingFlag
; Bit0 = xmin, Bit1 = xmax
; Bit2 = ymin, Bit3 = ymax
; Bit8 = zmin, (Bit9 = zmax)
; zmin, zmax, ymin, ymax, xmin, xmax

	and		eax,0100h			; sahf

	fst		DWORD PTR ds:[edi].CMV2Dot3DPos__m_TransfPos__m_fZ

	fabs

	fld1
	fdivrp	st(1),st
		;st0 = 1/fProjectionF, st1 = fProjectionR
		;st2 = fProjectionD
@@NoZClipping:

	mov		ecx,eax

	fst		DWORD PTR ds:[edi].CMV2Dot3DPos__m_fZNewRZP

	fmul	st(2),st
	fmulp	st(1),st
	fxch
		;st0 = fProjectionR/fProjectionF
		;st1 = fProjectionD/fProjectionF
;	fadd	fXmaxDiv2
;	fxch
;	fadd	fYmaxDiv2
;	fxch

	fadd	fNum_1
	fxch
	fadd	fNum_1
	fxch
	fmul	fXmaxDiv2
	fxch
	fmul	fYmaxDiv2
	fxch

	fist	DWORD PTR ds:[edi].CMV2Dot3DPos__m_iScreenX
	add		edx,4
	fxch
	fist	DWORD PTR ds:[edi].CMV2Dot3DPos__m_iScreenY
	fxch
	fmul	fNum_2Exp20
	fxch
	fld		st
	fst		DWORD PTR ds:[edi].CMV2Dot3DPos__m_fScreenY
	fxch	st(2)
	fstp	DWORD PTR ds:[edi].CMV2Dot3DPos__m_fScreenX
	frndint

	mov		eax,dword ptr ds:[edi].CMV2Dot3DPos__m_iScreenX
	mov     ebx,dword ptr ds:[edi].CMV2Dot3DPos__m_iScreenY

	fsub	st,st(1)						; muss noch ueberprueft werden
											; (optimierung)
	fstp	DWORD PTR ds:[edi].CMV2Dot3DPos__m_fScreenYError
	ffree	st(0)

	cmp		eax,dwClipXmin
	jge		@@NotUnderXmin
	or		ecx,1

@@NotUnderXmin:
	cmp		eax,dwClipXmax
	jng		@@NotAboveXmax	; jnge
	or		ecx,2

@@NotAboveXmax:
	cmp     ebx,dwClipYmin
	jge		@@NotUnderYmin
	or		ecx,4

@@NotUnderYmin:
	cmp		ebx,dwClipYmax
	jng		@@NotAboveYmax	; jnge
	or		ecx,8

@@NotAboveYmax:
	mov     dword ptr ds:[edi].CMV2Dot3DPos__m_iClipFlag,ecx
	mov		edi,ds:[edx]

	dec		ebp
	jnz		@@DotLoop

	fldcw   oldfpucw

	popad
	pop		ebp
	ret
endp MESEtoMV2Dot3DPosProjectionASM








public MV2Dot3DPosProjectionASM
MV2Dot3DPosProjectionASM proc
	ARG _pMV2Camera:DWORD, _ppMV2Dot3DPos:DWORD, _dwNumDots:DWORD
	push	ebp
	mov		ebp,esp
	pushad


;****
	fstcw   oldfpucw
	mov     ax,oldfpucw
	and     eax,0fcffh
	and		eax,0fbffh
	or		eax,00800h
	mov     fpucw,ax
	fldcw   fpucw
;****

	mov		eax,_pMV2Camera
	mov		ebx,_ppMV2Dot3DPos
	mov		ecx,_dwNumDots

	mov		pMV2Camera,eax
	mov		ppMV2Dot3DPos,ebx
	mov		dwNumDots,ecx

	mov     esi,pMV2Camera
	mov		eax,ds:[esi].CMV2Camera__m_iXmax
	mov		ebx,ds:[esi].CMV2Camera__m_iYmax

	mov		dwXmax,eax
	mov		dwYmax,ebx
	fild	dwXmax
	fild	dwYmax
	fstp	fYmax
	fstp	fXmax

	shr		eax,1
	shr		ebx,1

	mov		lTemp,eax
	fild	lTemp
	fstp	fXmaxDiv2

	mov		lTemp,ebx
	fild	lTemp
	fstp	fYmaxDiv2

	mov		eax,ds:[esi].CMV2Camera__m_iClipXmin
	mov		ebx,ds:[esi].CMV2Camera__m_iClipXmax
	mov		ecx,ds:[esi].CMV2Camera__m_iClipYmin
	mov		edx,ds:[esi].CMV2Camera__m_iClipYmax

	inc		eax					; wegen des clippings
	inc		ecx

	mov		dwClipXmin,eax
	mov		dwClipXmax,ebx
	mov		dwClipYmin,ecx
	mov		dwClipYmax,edx

	mov		eax,ds:[esi].CMV2Camera__m_Pos__m_fX
	mov		ebx,ds:[esi].CMV2Camera__m_Pos__m_fY
	mov		ecx,ds:[esi].CMV2Camera__m_Pos__m_fZ
	mov     fCameraX,eax
	mov		fCameraY,ebx
	mov		fCameraZ,ecx

	mov		eax,ds:[esi].CMV2Camera__m_Front__m_fX
	mov		ebx,ds:[esi].CMV2Camera__m_Front__m_fY
	mov		ecx,ds:[esi].CMV2Camera__m_Front__m_fZ
	mov     fCameraFX,eax
	mov		fCameraFY,ebx
	mov		fCameraFZ,ecx

	fld		fAspectRatio
	fld		ds:[esi].CMV2Camera__m_Right__m_fX
		;st0 = CameraRX, st1 = AspectRatio
	fld		ds:[esi].CMV2Camera__m_Right__m_fY
	fld		ds:[esi].CMV2Camera__m_Right__m_fZ
		;st0 = CameraRZ, st1 = CameraRY
		;st2 = CameraRX, st3 = AspectRatio

	fld		ds:[esi].CMV2Camera__m_Down__m_fX
	fmul	st,st(4)
	fld		ds:[esi].CMV2Camera__m_Down__m_fY
	fmul	st,st(5)
	fld		ds:[esi].CMV2Camera__m_Down__m_fZ
	fmul	st,st(6)
	fxch	st(2)
		;st0 = CameraDX*AspectRatio, st1 = CameraDY
		;st2 = CameraDZ*AspectRatio, st3 = CameraRZ
		;st4 = CameraRY*AspectRatio, st5 = CameraRX
		;st6 = AspectRatio

	fstp	fCameraDX
	fstp	fCameraDY
	fstp	fCameraDZ
	fstp	fCameraRZ
	fstp	fCameraRY
	fstp	fCameraRX
	ffree	st(0)

	mov		edx,ppMV2Dot3DPos
	mov		ebp,dwNumDots
	mov		edi,ds:[edx]

@@DotLoop:
	fld		DWORD PTR ds:[edi].CMV2Dot3DPos__m_Pos__m_fX
	fsub	fCameraX
		;st0 = VectorX

	fld		fCameraFX
	fmul	st,st(1)
		;st0 = fCameraFX*VectorX, st1 = VectorX

	fld		fCameraRX
	fmul	st,st(2)
		;st0 = fCameraRX*VectorX, st1 = fCameraFX*VectorX
		;st2 = VectorX

	fld		fCameraDX
	fmul	st,st(3)
	ffree	st(3)
		;st0 = fCameraDX*VectorX, st1 = fCameraRX*VectorX
		;st2 = fCameraFX*VectorX

	fld		DWORD PTR ds:[edi].CMV2Dot3DPos__m_Pos__m_fY
	fsub	fCameraY
		;st0 = VectorY, st1 = fCameraDX*VectorX
		;st2 = fCameraRX*VectorX, st3 = fCameraFX*VectorX

	fld		fCameraFY
	fmul	st,st(1)
		;st0 = fCameraFY*VectorY, st1 = VectorY
		;st1 = fCameraDX*VectorX
		;st2 = fCameraRX*VectorX, st3 = fCameraFX*VectorX

	fld		fCameraRY
	fmul	st,st(2)
		;st0 = fCameraRY*VectorY, st1 = fCameraFY*VectorY
		;st2 = VectorY, st3 = fCameraDX*VectorX
		;st4 = fCameraRX*VectorX, st5 = fCameraFX*VectorX

	fld		fCameraDY
	fmul	st,st(3)
	fxch	st(2)
		;st0 = fCameraFY*VectorY, st1 = fCameraRY*VectorY
		;st2 = fCameraDY*VectorY, st3 = VectorY
		;st4 = fCameraDX*VectorX
		;st5 = fCameraRX*VectorX, st6 = fCameraFX*VectorX

	faddp	st(6),st
		;st0 = fCameraRY*VectorY, st1 = fCameraDY*VectorY
		;st2 = VectorY, st3 = fCameraDX*VectorX
		;st4 = fCameraRX*VectorX
		;st5 = fCameraFX*VectorX + fCameraFY*VectorY

	faddp	st(4),st
		;st0 = fCameraDY*VectorY
		;st1 = VectorY, st2 = fCameraDX*VectorX
		;st3 = fCameraRX*VectorX + fCameraRY*VectorY
		;st4 = fCameraFX*VectorX + fCameraFY*VectorY

	faddp	st(2),st
	fxch	st(3)
	ffree	st(3)
		;st0 = fCameraFX*VectorX + fCameraFY*VectorY
		;st1 = fCameraDX*VectorX + fCameraDY*VectorY
		;st2 = fCameraRX*VectorX + fCameraRY*VectorY

	fld		DWORD PTR ds:[edi].CMV2Dot3DPos__m_Pos__m_fZ
	fsub	fCameraZ
		;st0 = VectorZ
		;st1 = fCameraFX*VectorX + fCameraFY*VectorY
		;st2 = fCameraDX*VectorX + fCameraDY*VectorY
		;st3 = fCameraRX*VectorX + fCameraRY*VectorY

	fld		fCameraFZ
	fmul	st,st(1)
		;st0 = fCameraZ*VectorZ, st1 = VectorZ
		;st2 = fCameraFX*VectorX + fCameraFY*VectorY
		;st3 = fCameraDX*VectorX + fCameraDY*VectorY
		;st4 = fCameraRX*VectorX + fCameraRY*VectorY

	fld     fCameraRZ
	fmul	st,st(2)
		;st0 = fCameraRZ*VectorZ, st1 = fCameraFZ*VectorZ
		;st2 = VectorZ
		;st3 = fCameraFX*VectorX + fCameraFY*VectorY
		;st4 = fCameraDX*VectorX + fCameraDY*VectorY
		;st5 = fCameraRX*VectorX + fCameraRY*VectorY

	fld		fCameraDZ
	fmul	st,st(3)
	fxch	st(2)
		;st0 = fCameraFZ*VectorZ, st1 = fCameraRZ*VectorZ
		;st2 = fCameraDZ*VectorZ, st3 = VectorZ
		;st4 = fCameraFX*VectorX + fCameraFY*VectorY
		;st5 = fCameraDX*VectorX + fCameraDY*VectorY
		;st6 = fCameraRX*VectorX + fCameraRY*VectorY

	faddp	st(4),st
		;st0 = fCameraRZ*VectorZ
		;st1 = fCameraDZ*VectorZ, st2 = VectorZ
		;st3 = fProjectionF
		;st4 = fCameraDX*VectorX + fCameraDY*VectorY
		;st5 = fCameraRX*VectorX + fCameraRY*VectorY

	faddp	st(5),st
		;st0 = fCameraDZ*VectorZ, st1 = VectorZ
		;st2 = fProjectionF, st3 = fCameraRX*VectorX + fCameraRY*VectorY
		;st4 = fProjectionD

	faddp	st(3),st
		;st0 = VectorZ
		;st1 = fProjectionF = ZNew, st2 = fProjectionD = YNew
		;st3 = fProjectionR = XNew

	ffree	st(0)
	fincstp
		;st0 = fProjectionF = ZNew, st1 = fProjectionD = YNew
		;st2 = fProjectionR = XNew

	fxch
	fst		DWORD PTR ds:[edi].CMV2Dot3DPos__m_TransfPos__m_fY
	fxch
	fxch	st(2)
	fst		DWORD PTR ds:[edi].CMV2Dot3DPos__m_TransfPos__m_fX
	fxch	st(2)

	fcom	DWORD PTR fNum_01
	fnstsw	ax					; tasm 3.0 bug (= fstsw)

; ClippingFlag
; Bit0 = xmin, Bit1 = xmax
; Bit2 = ymin, Bit3 = ymax
; Bit8 = zmin, (Bit9 = zmax)
; zmin, zmax, ymin, ymax, xmin, xmax

	and		eax,0100h			; sahf

	fst		DWORD PTR ds:[edi].CMV2Dot3DPos__m_TransfPos__m_fZ

	fabs

	fld1
	fdivrp	st(1),st
		;st0 = 1/fProjectionF, st1 = fProjectionR
		;st2 = fProjectionD

	mov		ecx,eax

	fst		DWORD PTR ds:[edi].CMV2Dot3DPos__m_fZNewRZP

	fmul	st(2),st
	fmulp	st(1),st
	fxch
		;st0 = fProjectionR/fProjectionF
		;st1 = fProjectionD/fProjectionF
;	fadd	fXmaxDiv2
;	fxch
;	fadd	fYmaxDiv2
;	fxch

	fadd	fNum_1
	fxch
	fadd	fNum_1
	fxch
	fmul	fXmaxDiv2
	fxch
	fmul	fYmaxDiv2
	fxch

	fist	DWORD PTR ds:[edi].CMV2Dot3DPos__m_iScreenX
	add		edx,4
	fxch
	fist	DWORD PTR ds:[edi].CMV2Dot3DPos__m_iScreenY
	fxch
	fmul	fNum_2Exp20
	fxch
	fld		st
	fst		DWORD PTR ds:[edi].CMV2Dot3DPos__m_fScreenY
	fxch	st(2)
	fstp	DWORD PTR ds:[edi].CMV2Dot3DPos__m_fScreenX
	frndint

	mov		eax,dword ptr ds:[edi].CMV2Dot3DPos__m_iScreenX
	mov     ebx,dword ptr ds:[edi].CMV2Dot3DPos__m_iScreenY

	fsub	st,st(1)						; muss noch ueberprueft werden
											; (optimierung)
	fstp	DWORD PTR ds:[edi].CMV2Dot3DPos__m_fScreenYError
	ffree	st(0)

	cmp		eax,dwClipXmin
	jge		@@NotUnderXmin
	or		ecx,1

@@NotUnderXmin:
	cmp		eax,dwClipXmax
	jng		@@NotAboveXmax	; jnge
	or		ecx,2

@@NotAboveXmax:
	cmp     ebx,dwClipYmin
	jge		@@NotUnderYmin
	or		ecx,4

@@NotUnderYmin:
	cmp		ebx,dwClipYmax
	jng		@@NotAboveYmax	; jnge
	or		ecx,8

@@NotAboveYmax:
	mov     dword ptr ds:[edi].CMV2Dot3DPos__m_iClipFlag,ecx
	mov		edi,ds:[edx]

	dec		ebp
	jnz		@@DotLoop

	fldcw   oldfpucw

	popad
	pop		ebp
	ret
endp MV2Dot3DPosProjectionASM




END