;/*
.486

.model flat

LOCALS

	EXTRN	pcSparklesLookup	:DWORD

.data

fpucw						dw ?
oldfpucw					dw ?
oh_low 						dd ?
co_low						dd ?
co_hi						dd ?

align 32
pMV2Camera					dd ?
pMV2Particles				dd ?
dwNumParticles				dd ?
pBackBuffer					dd ?

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

fScreenFactor				dd ?

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

dwScreenX					dd ?
dwScreenY					dd ?
fZNewRZP					dd ?

comment &
fRCol						dd 50.0
fGCol						dd 180.0
fBCol						dd 180.0
&

fRCol						dd 255.0
fGCol						dd 255.0
fBCol						dd 255.0



dwRDest						dd ?
dwGDest						dd ?
dwBDest						dd ?

fAngleTOSinTabfactor		dd 40.74366543	;256.0/(2PI)
dwAngleX					dd ?
dwAngleY2					dd ?
dwAngleY1					dd ?
dwAngleZ					dd ?

fScaling					dd 3.5




;*****************
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
;*****************

dwTempR						dd ?
dwTempG						dd ?
dwTempB						dd ?


.code

include m2structs.inc


public MV2ParticleProjectionASM
MV2ParticleProjectionASM proc
	ARG _pMV2Camera:DWORD, _pMV2Particles:DWORD, _dwNumParticles:DWORD, _pBackBuffer:DWORD

	push	ebp
	mov		ebp,esp
	pushad

;****
	fstcw   oldfpucw
	mov     ax,oldfpucw
	and     eax,0fcffh
;	and		eax,0fbffh
;	or		eax,00800h
	mov     fpucw,ax
	fldcw   fpucw
;****

	mov		eax,_pMV2Camera
	mov		ebx,_pMV2Particles
	mov		ecx,_dwNumParticles
	mov		edx,_pBackBuffer

	mov		pMV2Camera,eax
	mov		pMV2Particles,ebx
	mov		dwNumParticles,ecx
	mov		pBackBuffer,edx

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

 ;	dec		eax					; wegen des clippings
 ;	dec		ecx

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

	mov		eax,ds:[esi].CMV2Camera__m_fScreenFactor
	mov		fScreenFactor,eax

;	fld     ds:[esi].CMV2Camera__m_fScreenFactor
	fld1
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

	mov		esi,pMV2Particles
	mov		edi,pBackBuffer
	mov		ebp,dwNumParticles

@@ParticleLoop:
	fld		DWORD PTR ds:[esi].CMV2Particle__m_InterpPos__m_fX
	fmul	fScaling
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

	fld		DWORD PTR ds:[esi].CMV2Particle__m_InterpPos__m_fZ
	fmul	fScaling
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

	fld		DWORD PTR ds:[esi].CMV2Particle__m_InterpPos__m_fY
	fchs
	fmul	fScaling
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
		;st1 = fProjectionF, st2 = fProjectionD
		;st3 = fProjectionR

	ffree	st(0)
	fincstp

	fcom	DWORD PTR fNum_1
	fnstsw	ax					; tasm 3.0 bug (= fstsw)

; ClippingFlag
; Bit0 = xmin, Bit1 = xmax
; Bit2 = ymin, Bit3 = ymax
; Bit8 = zmin, (Bit9 = zmax)
; zmin, zmax, ymin, ymax, xmin, xmax

	and		eax,0100h			; sahf

	fld1
	fdivrp	st(1),st
		;st0 = 1/fProjectionF, st1 = fProjectionR
		;st2 = fProjectionD
	mov		ecx,eax

	fst		fZNewRZP

	fmul	st(2),st
	fmulp	st(1),st
	fxch
		;st0 = fProjectionR/fProjectionF
		;st1 = fProjectionD/fProjectionF

	fadd	fNum_1
	fxch
	fadd	fNum_1
	fxch
	fmul	fXmaxDiv2
	fxch
	fmul	fYmaxDiv2
	fxch

	fistp	dwScreenX
	fistp	dwScreenY

comment &
	fadd	fXmaxDiv2
	fxch
	fadd	fYmaxDiv2
	fxch

	fistp	dwScreenX
	fistp	dwScreenY
&

	mov		eax,dwScreenX
	mov     	ebx,dwScreenY

	cmp		eax,dwClipXmin
 	jge		@@NotUnderXmin
	or		ecx,1

@@NotUnderXmin:
	cmp		eax,dwClipXmax
	jnge	@@NotAboveXmax	; jng
	or		ecx,2

@@NotAboveXmax:
	cmp     ebx,dwClipYmin
	jge		@@NotUnderYmin
	or		ecx,4

@@NotUnderYmin:
	cmp		ebx,dwClipYmax
	jnge	@@NotAboveYmax	; jng
	or		ecx,8

@@NotAboveYmax:
	cmp		ecx,0
	jnz		@@DontDrawParticle

	pushad
	mov		eax,ds:[esi].CMV2Particle__m_iDuration

	and		eax,0ffh
	mov     	eax,ds:[eax*4].pcSparklesLookup
	mov		ebx,eax
	mov		edx,eax
	and		eax,0ff0000h
	and		ebx,000ff00h
	mov		edx,00000ffh
	shr		eax,16
	shr		ebx,8
	mov		dwTempR,eax
	mov		dwTempG,ebx
	mov		dwTempB,edx
	fild	dwTempR
	fild	dwTempG
	fild	dwTempB
	fxch
	fstp	fGCol
	fstp	fBCol
	fstp	fRCol
	popad

	mov		ecx,dwXmax
	imul	ebx,ecx
	add		ebx,eax

	fld1
	fld		fZNewRZP
	fsub	st(1),st
		;st0 = 1/ZNew, st1 = 1 - 1/ZNew

	mov		eax,ds:[edi + 4*ebx]
	mov		ecx,eax
	mov		edx,eax
	and		eax,0ff0000h
	and		ecx,000ff00h
	and		edx,00000ffh
	shr		eax,16
	shr		ecx,8
	mov		dwRDest,eax
	mov		dwGDest,ecx
	mov		dwBDest,edx

	fild	dwRDest
	fmul	st,st(2)
	fld		fRCol
	fmul	st,st(2)
	faddp	st(1),st
	fistp	dwRDest

	fild	dwGDest
	fmul	st,st(2)
	fld		fGCol
	fmul	st,st(2)
	faddp	st(1),st
	fistp	dwGDest

	fild	dwBDest
	fmul	st,st(2)
	fld		fBCol
	fmul	st,st(2)
	faddp	st(1),st
	fistp	dwBDest

	ffree 	st(1)
	ffree	st(0)

	mov		eax,dwRDest
	mov		ecx,dwGDest
	mov		edx,dwBDest
	shl		eax,16
	shl		ecx,8
	or		eax,edx
	or		eax,ecx

	mov		ds:[edi + 4*ebx],eax


@@DontDrawParticle:

	add		esi,CMV2Particle__Size
	dec		ebp
	jnz		@@ParticleLoop


	fldcw   oldfpucw
	popad
	pop		ebp
	ret
endp MV2ParticleProjectionASM





public MV2ParticleDoBernoulliASM
MV2ParticleDoBernoulliASM proc
	ARG pParticles:DWORD,_dwNumParticles:DWORD,pSinTab:DWORD,dwCurTime:DWORD,fAX:DWORD,fAY1:DWORD,fAY2:DWORD,fAZ:DWORD,fFX:DWORD,fFY1:DWORD,fFY2:DWORD,fFZ:DWORD,fSDX:DWORD,fSDY1:DWORD,fSDY2:DWORD,fSDZ:DWORD,fDY1:DWORD,fDY2:DWORD
	push	ebp
	mov		ebp,esp
	pushad

	mov		esi,pParticles
	mov		ecx,_dwNumParticles
	mov		edi,pSinTab

@@ParticleLoop:
	cmp		DWORD PTR ds:[esi].CMV2Particle__m_iDuration,0
	jnz		@@DurationOk

	mov    	eax,ds:[esi].CMV2Particle__m_StartPos__m_fX
	mov    	ebx,ds:[esi].CMV2Particle__m_StartPos__m_fY
	mov    	edx,ds:[esi].CMV2Particle__m_StartPos__m_fZ

	mov    	ds:[esi].CMV2Particle__m_NextPos__m_fX,eax
	mov    	ds:[esi].CMV2Particle__m_NextPos__m_fY,ebx
	mov    	ds:[esi].CMV2Particle__m_NextPos__m_fZ,edx

	mov		eax,12000
	mov		ds:[esi].CMV2Particle__m_iDuration,eax

@@DurationOk:
	mov	eax,ds:[esi].CMV2Particle__m_NextPos__m_fX
	mov	ebx,ds:[esi].CMV2Particle__m_NextPos__m_fY
	mov	edx,ds:[esi].CMV2Particle__m_NextPos__m_fZ

	fld		ds:[esi].CMV2Particle__m_NextPos__m_fY
	fmul	fFX

	fld		ds:[esi].CMV2Particle__m_NextPos__m_fZ
	fmul	fFY1

	fld		ds:[esi].CMV2Particle__m_NextPos__m_fX
	fmul	fFY2

	fld		ds:[esi].CMV2Particle__m_NextPos__m_fX
	fmul	fFZ
	fxch	st(3)
		;st0=ParticleY*fFreqX, st1=ParticleX*fFreqY2
		;st2=ParticleZ*fFreqY1, st3=ParticleX*fFreqZ

	fild	dwCurTime
	fld		st
	fmul	fSDX
	fld		st(1)
	fmul	fSDY1
	fld		st(2)
	fmul	fSDY2
	fxch	st(3)
	fmul	fSDZ
		;st0=CurTime*fSinZ, st1=CurTime*fSinY2
		;st2=CurTime*fSinY1, st3=CurTime*fSinX
		;st4=ParticleY*fFreqX, st5=ParticleX*fFreqY2
		;st6=ParticleZ*fFreqY1, st7=ParticleX*fFreqZ

	faddp	st(7),st
		;st0=CurTime*fSinY2
		;st1=CurTime*fSinY1, st2=CurTime*fSinX
		;st3=ParticleY*fFreqX, st4=ParticleX*fFreqY2
		;st5=ParticleZ*fFreqY1, st6=fAngleZ

	faddp	st(4),st
		;st0=CurTime*fSinY1, st1=CurTime*fSinX
		;st2=ParticleY*fFreqX, st3=fAngleY2
		;st4=ParticleZ*fFreqY1, st5=fAngleZ

	faddp	st(4),st
		;st0=CurTime*fSinX
		;st1=ParticleY*fFreqX, st2=fAngleY2
		;st3=fAngleY1, st4=fAngleZ

	faddp	st(1),st
		;st0=fAngleX, st1=fAngleY2
		;st2=fAngleY1, st3=fAngleZ

	fld		fAngleTOSinTabfactor
	fmul	st(1),st
	fmul	st(2),st
	fmul	st(3),st
	fmul	st(4),st

	ffree	st(0)
	fincstp

	fistp	dwAngleX
	fistp	dwAngleY2
	fistp	dwAngleY1
	fistp	dwAngleZ

	mov	ds:[esi].CMV2Particle__m_LastPos__m_fX,eax
	mov	ds:[esi].CMV2Particle__m_LastPos__m_fY,ebx
	mov	ds:[esi].CMV2Particle__m_LastPos__m_fZ,edx

	mov		eax,dwAngleX
	mov     ebx,dwAngleY2
	and		eax,0ffh
	and		ebx,0ffh

	fld		DWORD PTR ds:[edi + 4*eax]
	fmul	fAX
	fld		DWORD PTR ds:[edi + 4*ebx]
	fadd	fDY2
	fmul	fAY2
	fxch

	fadd	DWORD PTR ds:[esi].CMV2Particle__m_NextPos__m_fX
	fxch
	fadd	DWORD PTR ds:[esi].CMV2Particle__m_NextPos__m_fY
	fxch
	fstp	DWORD PTR ds:[esi].CMV2Particle__m_NextPos__m_fX
	fstp	DWORD PTR ds:[esi].CMV2Particle__m_NextPos__m_fY


	mov		eax,dwAngleY1
	mov     ebx,dwAngleZ
	and		eax,0ffh
	and		ebx,0ffh

	fld		DWORD PTR ds:[edi + 4*eax]
	fadd	fDY1
	fmul	fAY1
	fld		DWORD PTR ds:[edi + 4*ebx]
	fmul	fAZ
	fxch

	fadd	DWORD PTR ds:[esi].CMV2Particle__m_NextPos__m_fY
	fxch
	fadd	DWORD PTR ds:[esi].CMV2Particle__m_NextPos__m_fZ
	fxch
	fstp	DWORD PTR ds:[esi].CMV2Particle__m_NextPos__m_fY
	fstp	DWORD PTR ds:[esi].CMV2Particle__m_NextPos__m_fZ


	dec		DWORD PTR ds:[esi].CMV2Particle__m_iDuration
	add		esi,CMV2Particle__Size

	dec		ecx
	jnz		@@ParticleLoop

	popad
	pop		ebp
	ret
endp MV2ParticleDoBernoulliASM





public MV2ParticleDoBernoulliMorphToObjectASM
MV2ParticleDoBernoulliMorphToObjectASM proc
	ARG pParticles:DWORD,_dwNumParticles:DWORD,pSinTab:DWORD,dwCurTime:DWORD,fAX:DWORD,fAY1:DWORD,fAY2:DWORD,fAZ:DWORD,fFX:DWORD,fFY1:DWORD,fFY2:DWORD,fFZ:DWORD,fSDX:DWORD,fSDY1:DWORD,fSDY2:DWORD,fSDZ:DWORD,fDY1:DWORD,fDY2:DWORD,fMFactor:DWORD
	push	ebp
	mov		ebp,esp
	pushad

	mov		esi,pParticles
	mov		ecx,_dwNumParticles
	mov		edi,pSinTab

@@ParticleLoop:
	mov	eax,ds:[esi].CMV2Particle__m_NextPos__m_fX
	mov	ebx,ds:[esi].CMV2Particle__m_NextPos__m_fY
	mov	edx,ds:[esi].CMV2Particle__m_NextPos__m_fZ
	mov	ds:[esi].CMV2Particle__m_LastPos__m_fX,eax
	mov	ds:[esi].CMV2Particle__m_LastPos__m_fY,ebx
	mov	ds:[esi].CMV2Particle__m_LastPos__m_fZ,edx

	cmp		DWORD PTR ds:[esi].CMV2Particle__m_iDuration,0
	jnz		@@DurationOk

	mov    	eax,ds:[esi].CMV2Particle__m_StartPos__m_fX
	mov    	ebx,ds:[esi].CMV2Particle__m_StartPos__m_fY
	mov    	edx,ds:[esi].CMV2Particle__m_StartPos__m_fZ

	mov    	ds:[esi].CMV2Particle__m_NextPos__m_fX,eax
	mov    	ds:[esi].CMV2Particle__m_NextPos__m_fY,ebx
	mov    	ds:[esi].CMV2Particle__m_NextPos__m_fZ,edx

	mov		eax,12000
	mov		ds:[esi].CMV2Particle__m_iDuration,eax
@@DurationOk:
	fld		ds:[esi].CMV2Particle__m_NextPos__m_fY
	fmul	fFX

	fld		ds:[esi].CMV2Particle__m_NextPos__m_fZ
	fmul	fFY1

	fld		ds:[esi].CMV2Particle__m_NextPos__m_fX
	fmul	fFY2

	fld		ds:[esi].CMV2Particle__m_NextPos__m_fX
	fmul	fFZ
	fxch	st(3)
		;st0=ParticleY*fFreqX, st1=ParticleX*fFreqY2
		;st2=ParticleZ*fFreqY1, st3=ParticleX*fFreqZ

	fild	dwCurTime
	fld		st
	fmul	fSDX
	fld		st(1)
	fmul	fSDY1
	fld		st(2)
	fmul	fSDY2
	fxch	st(3)
	fmul	fSDZ
		;st0=CurTime*fSinZ, st1=CurTime*fSinY2
		;st2=CurTime*fSinY1, st3=CurTime*fSinX
		;st4=ParticleY*fFreqX, st5=ParticleX*fFreqY2
		;st6=ParticleZ*fFreqY1, st7=ParticleX*fFreqZ

	faddp	st(7),st
		;st0=CurTime*fSinY2
		;st1=CurTime*fSinY1, st2=CurTime*fSinX
		;st3=ParticleY*fFreqX, st4=ParticleX*fFreqY2
		;st5=ParticleZ*fFreqY1, st6=fAngleZ

	faddp	st(4),st
		;st0=CurTime*fSinY1, st1=CurTime*fSinX
		;st2=ParticleY*fFreqX, st3=fAngleY2
		;st4=ParticleZ*fFreqY1, st5=fAngleZ

	faddp	st(4),st
		;st0=CurTime*fSinX
		;st1=ParticleY*fFreqX, st2=fAngleY2
		;st3=fAngleY1, st4=fAngleZ

	faddp	st(1),st
		;st0=fAngleX, st1=fAngleY2
		;st2=fAngleY1, st3=fAngleZ

	fld		fAngleTOSinTabfactor
	fmul	st(1),st
	fmul	st(2),st
	fmul	st(3),st
	fmul	st(4),st

	ffree	st(0)
	fincstp

	fistp	dwAngleX
	fistp	dwAngleY2
	fistp	dwAngleY1
	fistp	dwAngleZ

	mov		eax,dwAngleX
	mov     ebx,dwAngleY2
	and		eax,0ffh
	and		ebx,0ffh

	fld		DWORD PTR ds:[edi + 4*eax]
	fmul	fAX
	fld		DWORD PTR ds:[edi + 4*ebx]
	fadd	fDY2
	fmul	fAY2
	fxch

	fadd	DWORD PTR ds:[esi].CMV2Particle__m_NextPos__m_fX
	fxch
	fadd	DWORD PTR ds:[esi].CMV2Particle__m_NextPos__m_fY
	fxch
	fstp	DWORD PTR ds:[esi].CMV2Particle__m_NextPos__m_fX
	fstp	DWORD PTR ds:[esi].CMV2Particle__m_NextPos__m_fY


	mov		eax,dwAngleY1
	mov     ebx,dwAngleZ
	and		eax,0ffh
	and		ebx,0ffh

	fld		DWORD PTR ds:[edi + 4*eax]
	fadd	fDY1
	fmul	fAY1
	fld		DWORD PTR ds:[edi + 4*ebx]
	fmul	fAZ
	fxch

	fadd	DWORD PTR ds:[esi].CMV2Particle__m_NextPos__m_fY
	fxch
	fadd	DWORD PTR ds:[esi].CMV2Particle__m_NextPos__m_fZ
	fxch
	fstp	DWORD PTR ds:[esi].CMV2Particle__m_NextPos__m_fY
	fstp	DWORD PTR ds:[esi].CMV2Particle__m_NextPos__m_fZ

	fld	fMFactor
	fld		DWORD PTR ds:[esi].CMV2Particle__m_EndPos__m_fX
	fsub	DWORD PTR ds:[esi].CMV2Particle__m_NextPos__m_fX
	fld		DWORD PTR ds:[esi].CMV2Particle__m_EndPos__m_fY
	fsub	DWORD PTR ds:[esi].CMV2Particle__m_NextPos__m_fY
	fld		DWORD PTR ds:[esi].CMV2Particle__m_EndPos__m_fZ
	fsub	DWORD PTR ds:[esi].CMV2Particle__m_NextPos__m_fZ
		;st0 = EndZ - Z, st1 = EndY - Y, st2 = EndX - X
		;st3 = MFactor

	fmul	st,st(3)
	fxch
	fmul	st,st(3)
	fxch
	fxch	st(2)
	fmul	st,st(3)
	fxch	st(2)
		;st0 = (EndZ - Z)*MFactor, st1 = (EndY - Y)*MFactor
		;st2 = (EndX - X)*MFactor, st3 = MFactor

	fadd	DWORD PTR ds:[esi].CMV2Particle__m_NextPos__m_fZ
	fxch
	fadd	DWORD PTR ds:[esi].CMV2Particle__m_NextPos__m_fY
	fxch
	fxch	st(2)
	fadd	DWORD PTR ds:[esi].CMV2Particle__m_NextPos__m_fX
	fxch	st(2)

	fstp	DWORD PTR ds:[esi].CMV2Particle__m_NextPos__m_fZ
	fstp	DWORD PTR ds:[esi].CMV2Particle__m_NextPos__m_fY
	fstp	DWORD PTR ds:[esi].CMV2Particle__m_NextPos__m_fX


	dec		DWORD PTR ds:[esi].CMV2Particle__m_iDuration
	fcomp

	add		esi,CMV2Particle__Size

	dec		ecx
	jnz		@@ParticleLoop

	popad
	pop		ebp
	ret
endp MV2ParticleDoBernoulliMorphToObjectASM





public MV2ParticleInterpolateASM
MV2ParticleInterpolateASM proc
	ARG _pParticles:DWORD,_dwNumParticles:DWORD, _fMorphFactor:DWORD
	push	ebp
	mov		ebp,esp
	pushad

	mov	esi,_pParticles
	mov	ecx,_dwNumParticles
	fld	dword ptr _fMorphFactor
		;st0 = MorphFactor

@@ParticleLoop:
	fld	ds:[esi].CMV2Particle__m_NextPos__m_fX
	fsub	ds:[esi].CMV2Particle__m_LastPos__m_fX
	fld	ds:[esi].CMV2Particle__m_NextPos__m_fY
	fsub	ds:[esi].CMV2Particle__m_LastPos__m_fY
	fld	ds:[esi].CMV2Particle__m_NextPos__m_fZ
	fsub	ds:[esi].CMV2Particle__m_LastPos__m_fZ
		;st0 = NPosZ - LPosZ, st1 = NPosY - LPosY
		;st2 = NPosX - LPosX, st3 = MorphFactor

	fmul	st,st(3)
	fxch
	fmul	st,st(3)
	fxch
	fxch	st(2)
	fmul	st,st(3)
	fxch	st(2)

	fadd	ds:[esi].CMV2Particle__m_LastPos__m_fZ
	fxch
	fadd	ds:[esi].CMV2Particle__m_LastPos__m_fY
	fxch
	fxch	st(2)
	fadd	ds:[esi].CMV2Particle__m_LastPos__m_fX
	fxch	st(2)

	fstp	ds:[esi].CMV2Particle__m_InterpPos__m_fZ
	fstp	ds:[esi].CMV2Particle__m_InterpPos__m_fY
	fstp	ds:[esi].CMV2Particle__m_InterpPos__m_fX

	add	esi,CMV2Particle__Size

	dec	ecx
	jnz	@@ParticleLoop

	ffree	st(0)

	popad
	pop		ebp
	ret
endp MV2ParticleInterpolateASM



	END


