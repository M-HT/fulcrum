;/*
.486

.model flat

LOCALS

.data

EXTRN	pFireRGBLookUp		:DWORD

pcBackBuffer1			dd ?
pcBackBuffer2			dd ?
piPolarTab			dd ?
pfDistFunc			dd ?
pfSinTab			dd ?
pfCosTab			dd ?
pcDestBackBuffer		dd ?
dwCrossfadeFactor		dd ?


pBuffer				dd 4000 dup(0)

dwXCounter			dd ?
dwYCounter			dd ?
dwYOffset			dd ?

dwXmax				dd ?
dwYmax				dd ?
dwXmaxDiv2			dd ?
dwYmaxDiv2			dd ?

fDistanceScaleRZP		dd 0.25	; 1/4
dwX				dd ?
dwY				dd ?
DreamJmpTab			dd 9 dup (?)
FadeJmpTab			dd 9 dup (?)
BeamFadeJmpTab			dd 9 dup (?)


pSrc				dd ?
pDest				dd ?
dwSrcXmax			dd ?
dwSrcYmax			dd ?
dwDestXmax			dd ?
dwDestYmax			dd ?

dwDeltaX			dd ?
dwDeltaY			dd ?
dwCurY				dd ?

fNum2Exp16			dd 65536.0

pcSrcBack			dd ?

pcMoveListTab			dd ?
pcMask				dd ?
pcPic				dd ?
pcDestBack			dd ?
dwNum				dd ?
pcFade				dd ?
dwFadeFactor			dd ?

piSinTabX			dd ?
piSinTabY			dd ?


pcAmpTab			dd ?
pcYAmpTab			dd ?



.code

public Init2DDreamJumpTableASM
Init2DDreamJumpTableASM proc
	push		edx

	mov		edx,offset DreamJmpTab
	mov		dword ptr ds:[edx].00h,offset DreamCase0
	mov		dword ptr ds:[edx].04h,offset DreamCase1
	mov		dword ptr ds:[edx].08h,offset DreamCase2
	mov		dword ptr ds:[edx].0ch,offset DreamCase3
	mov		dword ptr ds:[edx].10h,offset DreamCase4
	mov		dword ptr ds:[edx].14h,offset DreamCase5
	mov		dword ptr ds:[edx].18h,offset DreamCase6
	mov		dword ptr ds:[edx].1ch,offset DreamCase7
	mov		dword ptr ds:[edx].20h,offset DreamCase8

	pop		edx

	ret
endp Init2DDreamJumpTableASM






public Init2DFadeJumpTableASM
Init2DFadeJumpTableASM proc
	push		edx

	mov		edx,offset FadeJmpTab
	mov		dword ptr ds:[edx].00h,offset FadeCase0
	mov		dword ptr ds:[edx].04h,offset FadeCase1
	mov		dword ptr ds:[edx].08h,offset FadeCase2
	mov		dword ptr ds:[edx].0ch,offset FadeCase3
	mov		dword ptr ds:[edx].10h,offset FadeCase4
	mov		dword ptr ds:[edx].14h,offset FadeCase5
	mov		dword ptr ds:[edx].18h,offset FadeCase6
	mov		dword ptr ds:[edx].1ch,offset FadeCase7
	mov		dword ptr ds:[edx].20h,offset FadeCase8

	pop		edx

	ret
endp Init2DFadeJumpTableASM




public Init2DBeamFadeJumpTableASM
Init2DBeamFadeJumpTableASM proc
	push		edx

	mov		edx,offset BeamFadeJmpTab
	mov		dword ptr ds:[edx].00h,offset BeamFadeCase0
	mov		dword ptr ds:[edx].04h,offset BeamFadeCase1
	mov		dword ptr ds:[edx].08h,offset BeamFadeCase2
	mov		dword ptr ds:[edx].0ch,offset BeamFadeCase3
	mov		dword ptr ds:[edx].10h,offset BeamFadeCase4
	mov		dword ptr ds:[edx].14h,offset BeamFadeCase5
	mov		dword ptr ds:[edx].18h,offset BeamFadeCase6
	mov		dword ptr ds:[edx].1ch,offset BeamFadeCase7
	mov		dword ptr ds:[edx].20h,offset BeamFadeCase8

	pop		edx

	ret
endp Init2DBeamFadeJumpTableASM




public Draw2DDreamASM
Draw2DDreamASM proc
	ARG _pBackB1:DWORD, _pBackB2:DWORD, _pPolarTab:DWORD, _pDistFunc:DWORD, _pSinTab:DWORD, _pCosTab:DWORD, _pDestBBack:DWORD, _dwCFadeFactor:DWORD, _dwXmax:DWORD, _dwYmax:DWORD
	push	ebp
	mov		ebp,esp
	pushad

	mov		eax,_pBackB1
	mov		ebx,_pBackB2
	mov		ecx,_pPolarTab
	mov		edx,_pDistFunc
	mov		esi,_pSinTab
	mov		edi,_pCosTab

	mov		pcBackBuffer1,eax
	mov		pcBackBuffer2,ebx
	mov		piPolarTab,ecx
	mov		pfDistFunc,edx
	mov		pfSinTab,esi
	mov		pfCosTab,edi

	mov		eax,_pDestBBack
	mov		ebx,_dwCFadeFactor
	mov		ecx,_dwXmax
	mov		edx,_dwYmax
	mov		pcDestBackBuffer,eax
	mov		dwCrossfadeFactor,ebx
	mov		dwXmax,ecx
	mov		dwYmax,edx
	shr		ecx,1
	shr		edx,1
	mov		dwXmaxDiv2,ecx
	mov		dwYmaxDiv2,edx

	xor		eax,eax
	mov		dwYOffset,eax			;dwYOffset init.

	mov		eax,dwYmax
	mov		dwYCounter,eax			;dwYCounter init.

	fld		fDistanceScaleRZP

 @@YLoop:
 	mov		eax,pfSinTab
 	mov		ebx,pfCosTab

	mov		esi,dwYOffset
	shl		esi,2
	add		esi,piPolarTab

	xor		edi,edi
	mov		ebp,dwXmax
 @@XLoop:
 ; eax = pSinTab
 ; ebx = pCosTab
 ; ecx =
 ; edx =
 ; esi = PolarTab
 ; edi = Dest
 ; ebp = XCounter

	mov		edx,ds:[esi]
	mov		ecx,edx
	shr		edx,16				;Distance
	and		ecx,0ffffh
	mov		ds:[edi].pBuffer,edx
	shl		edx,2
	add		edx,pfDistFunc


	fld		dword ptr ds:[eax + ecx*4]
	fild		dword ptr ds:[edi].pBuffer
	fmul		dword ptr ds:[edx]
	fmul		st,st(2)
	fld		dword ptr ds:[ebx + ecx*4]
		;st0 = Cos, st1 = Distance, st2 = Sin, st3 = 1/DSCale
	fmul		st,st(1)
	fxch		st(2)
		;st0 = Sin, st1 = Distance, st2 = X, st3 = 1/DScale

	fmulp		st(1),st
	fxch
		;st0 = X, st1 = Y, st2 = 1/DScale

	fistp		dwX
	fistp		dwY
		;st0 = 1/DScale

	mov		ecx,dwX
	mov		edx,dwY
	add		ecx,dwXmaxDiv2
	add		edx,dwYmaxDiv2

	imul		edx,dwXmax
	add		ecx,edx
	mov		edx,ecx

	shl		ecx,2
	shl		edx,2
	add		ecx,pcBackBuffer1
	add		edx,pcBackBuffer2

	mov		ecx,ds:[ecx]	;Col1
	mov		edx,ds:[edx]	;Col1

	mov		ds:[edi + 4].pBuffer,ecx
	mov		ds:[edi + 8].pBuffer,edx

	add		esi,4
	add		edi,12

	dec		ebp
	jnz		@@XLoop

	xor		esi,esi
	mov             edi,dwYOffset
	shl		edi,2
	add		edi,pcDestBackBuffer		; edi init.
	mov		ebp,dwXmax			; ebp init.
@@CopyXLoop:
	mov		eax,dwCrossfadeFactor
	sub		eax,ds:[esi].pBuffer
	sar		eax,7

	cmp		eax,0
	jns		@@FadeMinOK
	xor		eax,eax
@@FadeMinOK:
	cmp		eax,8
	jbe		@@FadeMaxOK
	mov		eax,8
@@FadeMaxOK:

	jmp		ds:[4*eax].DreamJmpTab

DreamCase0:
	mov		ecx,ds:[esi + 4].pBuffer
	mov		ds:[edi],ecx
	jmp		@@FadeDone

DreamCase1:
	mov		eax,ds:[esi + 4].pBuffer
	mov		ecx,ds:[esi + 8].pBuffer
	mov		ebx,eax
	mov		edx,ecx

	and		eax,0f8f8f8f8h		;1
	and		ecx,0f8f8f8f8h		;2
	and		ebx,007070707h		;1f
	and		edx,007070707h		;2f

	sub		ecx,eax
	sub		edx,ebx

	lea		eax,[8*eax]
	lea		ebx,[8*ebx]
	add		eax,ecx
	add		ebx,edx

	and		ebx,0f8f8f8f8h
	add		ebx,eax
	shr		ebx,3
	mov		ds:[edi],ebx

	jmp		@@FadeDone

DreamCase2:
	mov		eax,ds:[esi + 4].pBuffer
	mov		ecx,ds:[esi + 8].pBuffer
	mov		ebx,eax
	mov		edx,ecx

	and		eax,0f8f8f8f8h
	and		ecx,0f8f8f8f8h
	and		ebx,007070707h
	and		edx,007070707h

	lea		eax,[2*eax + eax]
	lea		ebx,[2*ebx + ebx]
	lea		ecx,[2*ecx]
	lea		edx,[2*edx]
	lea		eax,[2*eax]
	lea		ebx,[2*ebx]

	add		eax,ecx
	add		ebx,edx

	and		ebx,0f8f8f8f8h
	add		ebx,eax
	shr		ebx,3
	mov		ds:[edi],ebx
	jmp		@@FadeDone

DreamCase3:
	mov		eax,ds:[esi + 4].pBuffer
	mov		ecx,ds:[esi + 8].pBuffer
	mov		ebx,eax
	mov		edx,ecx

	and		eax,0f8f8f8f8h
	and		ecx,0f8f8f8f8h
	and		ebx,007070707h
	and		edx,007070707h

	lea		eax,[4*eax + eax]
	lea		ebx,[4*ebx + ebx]
	lea		ecx,[2*ecx + ecx]
	lea		edx,[2*edx + edx]

	add		eax,ecx
	add		ebx,edx

	and		ebx,0f8f8f8f8h
	add		ebx,eax
	shr		ebx,3
	mov		ds:[edi],ebx
	jmp		@@FadeDone

DreamCase4:
	mov		eax,ds:[esi + 4].pBuffer
	mov		ecx,ds:[esi + 8].pBuffer
	mov		ebx,eax
	mov		edx,ecx

	and		eax,0f8f8f8f8h
	and		ecx,0f8f8f8f8h
	and		ebx,007070707h
	and		edx,007070707h

	lea		eax,[4*eax]
	lea		ebx,[4*ebx]
	lea		ecx,[4*ecx]
	lea		edx,[4*edx]

	add		eax,ecx
	add		ebx,edx

	and		ebx,0f8f8f8f8h
	add		ebx,eax
	shr		ebx,3
	mov		ds:[edi],ebx
	jmp		@@FadeDone

DreamCase5:
	mov		eax,ds:[esi + 4].pBuffer
	mov		ecx,ds:[esi + 8].pBuffer
	mov		ebx,eax
	mov		edx,ecx

	and		eax,0f8f8f8f8h
	and		ecx,0f8f8f8f8h
	and		ebx,007070707h
	and		edx,007070707h

	lea		eax,[2*eax + eax]
	lea		ebx,[2*ebx + ebx]
	lea		ecx,[4*ecx + ecx]
	lea		edx,[4*edx + edx]

	add		eax,ecx
	add		ebx,edx

	and		ebx,0f8f8f8f8h
	add		ebx,eax
	shr		ebx,3
	mov		ds:[edi],ebx
	jmp		@@FadeDone
DreamCase6:
	mov		eax,ds:[esi + 4].pBuffer
	mov		ecx,ds:[esi + 8].pBuffer
	mov		ebx,eax
	mov		edx,ecx

	and		eax,0f8f8f8f8h
	and		ecx,0f8f8f8f8h
	and		ebx,007070707h
	and		edx,007070707h

	lea		ecx,[2*ecx + ecx]
	lea		edx,[2*edx + edx]
	lea		eax,[2*eax]
	lea		ebx,[2*ebx]
	lea		ecx,[2*ecx]
	lea		edx,[2*edx]

	add		eax,ecx
	add		ebx,edx

	and		ebx,0f8f8f8f8h
	add		ebx,eax
	shr		ebx,3
	mov		ds:[edi],ebx
	jmp		@@FadeDone
DreamCase7:
	mov		eax,ds:[esi + 4].pBuffer
	mov		ecx,ds:[esi + 8].pBuffer
	mov		ebx,eax
	mov		edx,ecx

	and		eax,0f8f8f8f8h
	and		ecx,0f8f8f8f8h
	and		ebx,007070707h
	and		edx,007070707h

	sub		eax,ecx
	sub		ebx,edx

	lea		ecx,[8*ecx]
	lea		edx,[8*edx]

	add		eax,ecx
	add		ebx,edx

	and		ebx,0f8f8f8f8h
	add		ebx,eax
	shr		ebx,3
	mov		ds:[edi],ebx
	jmp		@@FadeDone
DreamCase8:
	mov		ecx,ds:[esi + 8].pBuffer
	mov		ds:[edi],ecx
	jmp		@@FadeDone

@@FadeDone:

	add		esi,12
	add		edi,4

	dec		ebp
	jnz		@@CopyXLoop

	mov		eax,dwXmax
	add		dwYOffset,eax
	dec		dwYCounter
	jnz		@@YLoop

	ffree		st(0)

	popad
	pop		ebp
	ret
endp Draw2DDreamASM





public Scale32ASM
Scale32ASM proc
	ARG _pSrc:DWORD, _pDest:DWORD, _dwSrcXmax:DWORD, _dwSrcYmax:DWORD, _dwDestXmax:DWORD, _dwDestYmax:DWORD
	push	ebp
	mov		ebp,esp
	pushad

	mov	eax,_pSrc
	mov	ebx,_pDest
	mov	ecx,_dwSrcXmax
	mov	edx,_dwSrcYmax
	mov	esi,_dwDestXmax
	mov	edi,_dwDestYmax

	mov	pSrc,eax
	mov	pDest,ebx
	mov	dwSrcXmax,ecx
	mov	dwSrcYmax,edx
	mov	dwDestXmax,esi
	mov	dwDestYmax,edi

	mov	edi,pDest			; edi init.

	xor	eax,eax
	mov	dwCurY,eax

	fild	dwSrcXmax
	fidiv	dwDestXmax
	fmul	fNum2Exp16
	fistp	dwDeltaX

	fild	dwSrcYmax
	fidiv	dwDestYmax
	fmul	fNum2Exp16
	fistp	dwDeltaY


	mov	ebx,dwDeltaX
	mov	ecx,ebx
	sar	ebx,16				; ebx init.
	shl	ecx,16				; ecx init.

	mov	eax,dwDestYmax
	mov	dwYCounter,eax

@@YLoop:
	mov	esi,dwCurY
	shr	esi,16
	imul	esi,dwSrcXmax
	mov	eax,pSrc			; esi init.
	shr	eax,2
	add	esi,eax

	xor	edx,edx				; edx init.

	mov	ebp,dwDestXmax			; XCounter
@@XLoop:
; eax =
; ebx =                 dX
; ecx = dXf
; edx = Xf
; esi = src + Y*DestXMax + X
; edi = dest
; ebp = xcounter

	mov	eax,ds:[4*esi]
	mov	ds:[edi],eax

	add	edx,ecx
	adc	esi,ebx

	add	edi,4

	dec	ebp
	jnz	@@XLoop

	mov	eax,dwCurY
	add	eax,dwDeltaY
	mov	dwCurY,eax

	dec	dwYCounter
	jnz	@@YLoop


	popad
	pop		ebp
	ret
endp Scale32ASM





public Scale8ASM
Scale8ASM proc
	ARG _pSrc:DWORD, _pDest:DWORD, _dwSrcXmax:DWORD, _dwSrcYmax:DWORD, _dwDestXmax:DWORD, _dwDestYmax:DWORD
	push	ebp
	mov		ebp,esp
	pushad

	mov	eax,_pSrc
	mov	ebx,_pDest
	mov	ecx,_dwSrcXmax
	mov	edx,_dwSrcYmax
	mov	esi,_dwDestXmax
	mov	edi,_dwDestYmax

	mov	pSrc,eax
	mov	pDest,ebx
	mov	dwSrcXmax,ecx
	mov	dwSrcYmax,edx
	mov	dwDestXmax,esi
	mov	dwDestYmax,edi

	mov	edi,pDest			; edi init.

	xor	eax,eax
	mov	dwCurY,eax

	fild	dwSrcXmax
	fidiv	dwDestXmax
	fmul	fNum2Exp16
	fistp	dwDeltaX

	fild	dwSrcYmax
	fidiv	dwDestYmax
	fmul	fNum2Exp16
	fistp	dwDeltaY


	mov	ebx,dwDeltaX
	mov	ecx,ebx
	sar	ebx,16				; ebx init.
	shl	ecx,16				; ecx init.

	mov	eax,dwDestYmax
	mov	dwYCounter,eax

@@YLoop:
	mov	esi,dwCurY
	shr	esi,16
	imul	esi,dwSrcXmax
	add	esi,pSrc			; esi init.

	xor	edx,edx				; edx init.

	mov	ebp,dwDestXmax			; XCounter
@@XLoop:
; eax =
; ebx =                 dX
; ecx = dXf
; edx = Xf
; esi = src + Y*DestXMax + X
; edi = dest
; ebp = xcounter

	mov	al,ds:[esi]
	mov	ds:[edi],al

	add	edx,ecx
	adc	esi,ebx

	add	edi,1

	dec	ebp
	jnz	@@XLoop

	mov	eax,dwCurY
	add	eax,dwDeltaY
	mov	dwCurY,eax

	dec	dwYCounter
	jnz	@@YLoop


	popad
	pop		ebp
	ret
endp Scale8ASM







public Draw2DEyeASM
Draw2DEyeASM proc
	ARG     _pMoveListTab:DWORD, _pMask:DWORD, _pPic:DWORD, _pSrcBack:DWORD, _pDestBack:DWORD, _dwNum:DWORD
	push	ebp
	mov	ebp,esp
	pushad

	mov	eax,_pMoveListTab
	mov	ebx,_pMask
	mov	ecx,_pPic
	mov	edx,_pSrcBack
	mov	esi,_pDestBack
	mov	edi,_dwNum

	mov	pcMoveListTab,eax
	mov	pcMask,ebx
	mov	pcPic,ecx
	mov	pcSrcBack,edx
	mov	pcDestBack,esi
	mov	dwNum,edi


	mov	ebx,pcMoveListTab
	mov	ecx,pcMask
	mov	edx,pcPic
	mov	esi,pcSrcBack
	mov	edi,pcDestBack
	mov	ebp,dwNum

@@Loop:
; eax =
; ebx = MoveListTab
; ecx = Mask
; edx = Pic
; esi = SrcBack
; edi = DestBack
; ebp = counter
	push	ebx
	cmp	byte ptr ds:[ecx],0
      	jz	@@DontDraw

	mov	eax,ds:[ebx]			; from Movelist: offset
	mov	ebx,ds:[4*eax + esi]		; col from SrcBackBuffer

	mov	eax,ds:[edx]			; col from Pic
	and	ebx,0fefefefeh
	and	eax,0fefefefeh
	add	ebx,eax
	shr	ebx,1

	mov	ds:[edi],ebx			; setpixel
	jmp	@@Drawed
@@DontDraw:
	mov	eax,ds:[edx]			; col from Pic
	and	eax,0fefefefeh
	shr	eax,1
	mov	ds:[edi],eax			; setpixel
@@Drawed:
	inc	ecx
	pop	ebx
	add	edx,4
	add	ebx,4
	add	edi,4

	dec	ebp
	jnz	@@Loop

	popad
	pop		ebp
	ret
endp Draw2DEyeASM






public Draw2DFadeASM
Draw2DFadeASM proc
	ARG     _pBackBuffer1:DWORD, _pBackBuffer2:DWORD, _pDestBack:DWORD, _pFade:DWORD, _dwFadeFactor:DWORD, _dwNum:DWORD
	push	ebp
	mov	ebp,esp
	pushad


	mov	eax,_pBackBuffer1
	mov	ebx,_pBackBuffer2
	mov	ecx,_pDestBack
	mov	edx,_pFade
	mov	esi,_dwFadeFactor
	mov	edi,_dwNum

	mov	pcBackBuffer1,eax
	mov	pcBackBuffer2,ebx
	mov	pcDestBack,ecx
	mov	pcFade,edx
	mov	dwFadeFactor,esi
	mov	dwNum,edi

	mov	ecx,dwNum
	mov	edx,pcFade
	mov	esi,pcBackBuffer1
	mov	edi,pcDestBack
	mov	ebp,pcBackBuffer2
@@Loop:
; eax =
; ebx =
; ecx = Counter
; edx = FadePic
; esi = SrcBack1
; edi = DestBack
; ebp = SrcBack2
	xor		ebx,ebx

	push		ecx
	mov		bl,ds:[edx]
	push		edx
	add		ebx,dwFadeFactor
	sar		ebx,2

	cmp		ebx,0
	jns		@@FadeMinOK
	xor		ebx,ebx
@@FadeMinOK:
	cmp		ebx,8
	jbe		@@FadeMaxOK
	mov		ebx,8
@@FadeMaxOK:

	jmp		ds:[4*ebx].FadeJmpTab

FadeCase0:
	mov		ecx,ds:[esi]
	mov		ds:[edi],ecx
	jmp		@@FadeDone

FadeCase1:
	mov		eax,ds:[esi]
	mov		ecx,ds:[ebp]
	mov		ebx,eax
	mov		edx,ecx

	and		eax,0f8f8f8f8h		;1
	and		ecx,0f8f8f8f8h		;2
	and		ebx,007070707h		;1f
	and		edx,007070707h		;2f

	sub		ecx,eax
	sub		edx,ebx

	lea		eax,[8*eax]
	lea		ebx,[8*ebx]
	add		eax,ecx
	add		ebx,edx

	and		ebx,0f8f8f8f8h
	add		ebx,eax
	shr		ebx,3
	mov		ds:[edi],ebx

	jmp		@@FadeDone

FadeCase2:
	mov		eax,ds:[esi]
	mov		ecx,ds:[ebp]
	mov		ebx,eax
	mov		edx,ecx

	and		eax,0f8f8f8f8h
	and		ecx,0f8f8f8f8h
	and		ebx,007070707h
	and		edx,007070707h

	lea		eax,[2*eax + eax]
	lea		ebx,[2*ebx + ebx]
	lea		ecx,[2*ecx]
	lea		edx,[2*edx]
	lea		eax,[2*eax]
	lea		ebx,[2*ebx]

	add		eax,ecx
	add		ebx,edx

	and		ebx,0f8f8f8f8h
	add		ebx,eax
	shr		ebx,3
	mov		ds:[edi],ebx
	jmp		@@FadeDone

FadeCase3:
	mov		eax,ds:[esi]
	mov		ecx,ds:[ebp]
	mov		ebx,eax
	mov		edx,ecx

	and		eax,0f8f8f8f8h
	and		ecx,0f8f8f8f8h
	and		ebx,007070707h
	and		edx,007070707h

	lea		eax,[4*eax + eax]
	lea		ebx,[4*ebx + ebx]
	lea		ecx,[2*ecx + ecx]
	lea		edx,[2*edx + edx]

	add		eax,ecx
	add		ebx,edx

	and		ebx,0f8f8f8f8h
	add		ebx,eax
	shr		ebx,3
	mov		ds:[edi],ebx
	jmp		@@FadeDone

FadeCase4:
	mov		eax,ds:[esi]
	mov		ecx,ds:[ebp]
	mov		ebx,eax
	mov		edx,ecx

	and		eax,0f8f8f8f8h
	and		ecx,0f8f8f8f8h
	and		ebx,007070707h
	and		edx,007070707h

	lea		eax,[4*eax]
	lea		ebx,[4*ebx]
	lea		ecx,[4*ecx]
	lea		edx,[4*edx]

	add		eax,ecx
	add		ebx,edx

	and		ebx,0f8f8f8f8h
	add		ebx,eax
	shr		ebx,3
	mov		ds:[edi],ebx
	jmp		@@FadeDone

FadeCase5:
	mov		eax,ds:[esi]
	mov		ecx,ds:[ebp]
	mov		ebx,eax
	mov		edx,ecx

	and		eax,0f8f8f8f8h
	and		ecx,0f8f8f8f8h
	and		ebx,007070707h
	and		edx,007070707h

	lea		eax,[2*eax + eax]
	lea		ebx,[2*ebx + ebx]
	lea		ecx,[4*ecx + ecx]
	lea		edx,[4*edx + edx]

	add		eax,ecx
	add		ebx,edx

	and		ebx,0f8f8f8f8h
	add		ebx,eax
	shr		ebx,3
	mov		ds:[edi],ebx
	jmp		@@FadeDone
FadeCase6:
	mov		eax,ds:[esi]
	mov		ecx,ds:[ebp]
	mov		ebx,eax
	mov		edx,ecx

	and		eax,0f8f8f8f8h
	and		ecx,0f8f8f8f8h
	and		ebx,007070707h
	and		edx,007070707h

	lea		ecx,[2*ecx + ecx]
	lea		edx,[2*edx + edx]
	lea		eax,[2*eax]
	lea		ebx,[2*ebx]
	lea		ecx,[2*ecx]
	lea		edx,[2*edx]

	add		eax,ecx
	add		ebx,edx

	and		ebx,0f8f8f8f8h
	add		ebx,eax
	shr		ebx,3
	mov		ds:[edi],ebx
	jmp		@@FadeDone
FadeCase7:
	mov		eax,ds:[esi]
	mov		ecx,ds:[ebp]
	mov		ebx,eax
	mov		edx,ecx

	and		eax,0f8f8f8f8h
	and		ecx,0f8f8f8f8h
	and		ebx,007070707h
	and		edx,007070707h

	sub		eax,ecx
	sub		ebx,edx

	lea		ecx,[8*ecx]
	lea		edx,[8*edx]

	add		eax,ecx
	add		ebx,edx

	and		ebx,0f8f8f8f8h
	add		ebx,eax
	shr		ebx,3
	mov		ds:[edi],ebx
	jmp		@@FadeDone
FadeCase8:
	mov		ecx,ds:[ebp]
	mov		ds:[edi],ecx
	jmp		@@FadeDone

@@FadeDone:
	pop	edx
	pop	ecx
	add	esi,4
	add	edi,4
	add	ebp,4

	inc	edx
	dec	ecx
	jnz	@@Loop

	popad
	pop		ebp
	ret
endp Draw2DFadeASM



























public Draw2DBeamFadeASM
Draw2DBeamFadeASM proc
	ARG	_pBackB1:DWORD, _pBackB2:DWORD, _pDestB:DWORD, _dwFFactor:DWORD, _dwXmax:DWORD, _dwYmax:DWORD, _pSinTabX:DWORD, _pSinTabY:DWORD

	push	ebp
	mov	ebp,esp
	pushad

	mov	eax,_pBackB1
	mov	ebx,_pBackB2
	mov	ecx,_pDestB
	mov	edx,_pFade
	mov	esi,_dwFFactor
	mov	edi,_dwXmax

	mov	pcBackBuffer1,eax
	mov	pcBackBuffer2,ebx
	mov	pcDestBack,ecx
	mov	pcFade,edx
	mov	dwFadeFactor,esi
	mov	dwXmax,edi

	mov     eax,_dwYmax
	mov	ebx,_pSinTabX
	mov	ecx,_pSinTabY

	mov     dwYmax,eax
	mov	piSinTabX,ebx
	mov	piSinTabY,ecx

	mov	eax,dwYmax
	mov	dwYCounter,eax

	xor	eax,eax
	mov	dwY,eax

@@YLoop:
	mov	ebx,dwY
	mov	edx,piSinTabY

	mov	ecx,ebx
	imul	ecx,dwXmax
	shl	ecx,2
	add	ecx,pcBackBuffer1

	mov	esi,ds:[edx + 4*ebx]
	imul	esi,dwXmax
	shl	esi,2
	add	esi,pcBackBuffer2		; esi init.

	mov	edx,piSinTabX
	xor	ebx,ebx				; ebx init.
	xor	edi,edi				; edi init.
	mov	ebp,dwXmax			; ebp init.


@@BeamXLoop:
; eax =
; ebx =
; ecx = pBackBuffer1 + (YOffset + DeltaSinY)
; edx = SinTabX
; esi = pBackBuffer2 + (YOffset + DeltaSinY)
; edi = pBuffer == x
; ebp = XCounter

; st0 = Amplitude
	mov	eax,ds:[edx + 4*edi]

	mov	ebx,ds:[ecx + 4*edi]
	mov	ds:[8*edi + 0].pBuffer,ebx

	mov	eax,ds:[esi + 4*eax]
	mov	ds:[8*edi + 4].pBuffer,eax

	inc	edi
	dec	ebp
	jnz	@@BeamXLoop


	xor	esi,esi
	mov	edi,dwY
	imul	edi,dwXmax
	shl	edi,2
	add	edi,pcDestBack

	mov	ebp,dwXmax

	mov	eax,dwFadeFactor
	cmp	eax,0
	jns	@@FadeFactorMinOK
	xor	eax,eax
@@FadeFactorMinOK:
	cmp	eax,8
	jbe	@@FadeFactorMaxOK
	mov	eax,8
@@FadeFactorMaxOK:
	jmp	ds:[eax*4].BeamFadeJmpTab

; eax =
; ebx =
; ecx =
; edx =
; esi = pBuffer
; edi = dest
; ebp = XCounter


BeamFadeCase0:
	mov	eax,ds:[esi].pBuffer
	mov	ds:[edi],eax

	add	esi,8
	add	edi,4

	dec	ebp
	jnz	BeamFadeCase0
	jmp	@@Done

BeamFadeCase1:
	mov	eax,ds:[esi + 0].pBuffer
	mov	ecx,ds:[esi + 4].pBuffer
	mov	ebx,eax
	mov	edx,ecx

	cmp	ecx,0
	jnz	@@NotBlack1
	mov	ds:[edi],eax
	add	esi,8
	add	edi,4
	dec	ebp
	jnz	BeamFadeCase1
	jmp	@@Done
@@NotBlack1:

	and	eax,0f8f8f8f8h		;1
	and	ecx,0f8f8f8f8h		;2
	and	ebx,007070707h		;1f
	and	edx,007070707h		;2f

	sub	ecx,eax
	sub	edx,ebx

	lea	eax,[8*eax]
	lea	ebx,[8*ebx]
	add	eax,ecx
	add	ebx,edx

	and	ebx,0f8f8f8f8h
	add	ebx,eax
	shr	ebx,3
	mov	ds:[edi],ebx

	add	esi,8
	add	edi,4
	dec	ebp
	jnz	BeamFadeCase1
	jmp	@@Done

BeamFadeCase2:
	mov	eax,ds:[esi + 0].pBuffer
	mov	ecx,ds:[esi + 4].pBuffer
	mov	ebx,eax
	mov	edx,ecx

	cmp	ecx,0
	jnz	@@NotBlack2
	mov	ds:[edi],eax
	add	esi,8
	add	edi,4
	dec	ebp
	jnz	BeamFadeCase2
	jmp	@@Done
@@NotBlack2:

	and	eax,0f8f8f8f8h
	and	ecx,0f8f8f8f8h
	and	ebx,007070707h
	and	edx,007070707h

	lea	eax,[2*eax + eax]
	lea	ebx,[2*ebx + ebx]
	lea	ecx,[2*ecx]
	lea	edx,[2*edx]
	lea	eax,[2*eax]
	lea	ebx,[2*ebx]

	add	eax,ecx
	add	ebx,edx

	and	ebx,0f8f8f8f8h
	add	ebx,eax
	shr	ebx,3
	mov	ds:[edi],ebx

	add	esi,8
	add	edi,4
	dec	ebp
	jnz	BeamFadeCase2
	jmp	@@Done

BeamFadeCase3:
	mov	eax,ds:[esi + 0].pBuffer
	mov	ecx,ds:[esi + 4].pBuffer
	mov	ebx,eax
	mov	edx,ecx

	cmp	ecx,0
	jnz	@@NotBlack3
	mov	ds:[edi],eax
	add	esi,8
	add	edi,4
	dec	ebp
	jnz	BeamFadeCase3
	jmp	@@Done
@@NotBlack3:

	and	eax,0f8f8f8f8h
	and	ecx,0f8f8f8f8h
	and	ebx,007070707h
	and	edx,007070707h

	lea	eax,[4*eax + eax]
	lea	ebx,[4*ebx + ebx]
	lea	ecx,[2*ecx + ecx]
	lea	edx,[2*edx + edx]

	add	eax,ecx
	add	ebx,edx

	and	ebx,0f8f8f8f8h
	add	ebx,eax
	shr	ebx,3
	mov	ds:[edi],ebx
	add	esi,8
	add	edi,4
	dec	ebp
	jnz	BeamFadeCase3
	jmp	@@Done


BeamFadeCase4:
	mov	eax,ds:[esi + 0].pBuffer
	mov	ecx,ds:[esi + 4].pBuffer
	mov	ebx,eax
	mov	edx,ecx

	cmp	ecx,0
	jnz	@@NotBlack4
	mov	ds:[edi],eax
	add	esi,8
	add	edi,4
	dec	ebp
	jnz	BeamFadeCase4
	jmp	@@Done
@@NotBlack4:

	and	eax,0f8f8f8f8h
	and	ecx,0f8f8f8f8h
	and	ebx,007070707h
	and	edx,007070707h

	lea	eax,[4*eax]
	lea	ebx,[4*ebx]
	lea	ecx,[4*ecx]
	lea	edx,[4*edx]

	add	eax,ecx
	add	ebx,edx

	and	ebx,0f8f8f8f8h
	add	ebx,eax
	shr	ebx,3
	mov	ds:[edi],ebx

	add    	esi,8
	add	edi,4
	dec	ebp
	jnz	BeamFadeCase4
	jmp	@@Done

BeamFadeCase5:
	mov	eax,ds:[esi + 0].pBuffer
	mov	ecx,ds:[esi + 4].pBuffer
	mov	ebx,eax
	mov	edx,ecx

	cmp	ecx,0
	jnz	@@NotBlack5
	mov	ds:[edi],eax
	add	esi,8
	add	edi,4
	dec	ebp
	jnz	BeamFadeCase5
	jmp	@@Done
@@NotBlack5:

	and	eax,0f8f8f8f8h
	and	ecx,0f8f8f8f8h
	and	ebx,007070707h
	and	edx,007070707h

	lea	eax,[2*eax + eax]
	lea	ebx,[2*ebx + ebx]
	lea	ecx,[4*ecx + ecx]
	lea	edx,[4*edx + edx]

	add	eax,ecx
	add	ebx,edx

	and	ebx,0f8f8f8f8h
	add	ebx,eax
	shr	ebx,3
	mov	ds:[edi],ebx

	add    	esi,8
	add	edi,4
	dec	ebp
	jnz	BeamFadeCase5
	jmp	@@Done

BeamFadeCase6:
	mov	eax,ds:[esi + 0].pBuffer
	mov	ecx,ds:[esi + 4].pBuffer
	mov	ebx,eax
	mov	edx,ecx

	cmp	ecx,0
	jnz	@@NotBlack6
	mov	ds:[edi],eax
	add	esi,8
	add	edi,4
	dec	ebp
	jnz	BeamFadeCase6
	jmp	@@Done
@@NotBlack6:

	and	eax,0f8f8f8f8h
	and	ecx,0f8f8f8f8h
	and	ebx,007070707h
	and	edx,007070707h

	lea	ecx,[2*ecx + ecx]
	lea	edx,[2*edx + edx]
	lea	eax,[2*eax]
	lea	ebx,[2*ebx]
	lea	ecx,[2*ecx]
	lea	edx,[2*edx]

	add	eax,ecx
	add	ebx,edx

	and	ebx,0f8f8f8f8h
	add	ebx,eax
	shr	ebx,3
	mov	ds:[edi],ebx

	add    	esi,8
	add	edi,4
	dec	ebp
	jnz	BeamFadeCase6
	jmp	@@Done

BeamFadeCase7:
	mov	eax,ds:[esi + 0].pBuffer
	mov	ecx,ds:[esi + 4].pBuffer
	mov	ebx,eax
	mov	edx,ecx

	cmp	ecx,0
	jnz	@@NotBlack7
	mov	ds:[edi],eax
	add	esi,8
	add	edi,4
	dec	ebp
	jnz	BeamFadeCase7
	jmp	@@Done
@@NotBlack7:

	and	eax,0f8f8f8f8h
	and	ecx,0f8f8f8f8h
	and	ebx,007070707h
	and	edx,007070707h

	sub	eax,ecx
	sub	ebx,edx

	lea	ecx,[8*ecx]
	lea	edx,[8*edx]

	add	eax,ecx
	add	ebx,edx

	and	ebx,0f8f8f8f8h
	add	ebx,eax
	shr	ebx,3
	mov	ds:[edi],ebx

	add    	esi,8
 	add	edi,4
	dec	ebp
	jnz	BeamFadeCase7
	jmp	@@Done

BeamFadeCase8:
	mov	ecx,ds:[esi + 4].pBuffer

	cmp	ecx,0
	jnz	@@NotBlack8
	mov	eax,ds:[esi + 0].pBuffer
	mov	ds:[edi],eax
	add	esi,8
	add	edi,4
	dec	ebp
	jnz	BeamFadeCase8
	jmp	@@Done
@@NotBlack8:
	mov	ds:[edi],ecx

	add    	esi,8
 	add	edi,4
	dec	ebp
	jnz	BeamFadeCase8
	jmp	@@Done


@@Done:
	inc	dwY

	dec	dwYCounter
	jnz	@@YLoop

	popad
	pop		ebp
	ret
endp Draw2DBeamFadeASM





public DrawFireASM
DrawFireASM proc
	ARG	_pDestBack:DWORD, _pAmpTab:DWORD, _pYAmpTab:DWORD, _dwXmax:DWORD, _dwYmax:DWORD
	push	ebp
	mov	ebp,esp
	pushad

	mov	eax,_pDestBack
	mov	ebx,_pAmpTab
	mov	ecx,_pYAmpTab
	mov	edx,_dwXmax
	mov	esi,_dwYmax

	mov	pcDestBack,eax
	mov	pcAmpTab,ebx
	mov	pcYAmpTab,ecx
	mov	dwXmax,edx
	mov	dwYmax,esi

	mov	esi,pcAmpTab
	mov	ebp,pcYAmpTab

	xor	eax,eax
	mov	dwY,eax

	mov	ecx,dwYmax

	shl	ecx,8				; ch init.

	xor	ebx,ebx				; (ebx init.)
@@YLoop:
	mov	eax,dwY
	mov	bl,ds:[ebp + eax]		; ebx init.

	shl	eax,10
	mov	edi,pcDestBack
	add	edi,eax				; edi init.

	mov	eax,dwXmax
	mov	cl,al				; cl init.

@@XLoop:
; eax =
; ebx =         	| YAmp
; ecx = 	YCounter| XCounter
; edx =
; esi = AmpTab
; edi = Dest
; ebp = YAmpTab
	xor	edx,edx
	xor	eax,eax
	mov	dl,ds:[esi]
	add	edx,ebx

	mov	eax,ds:[4*edx].pFireRGBLookUp
	mov	ds:[edi],eax

	inc	esi
	add	edi,4

	dec	cl
	jnz	@@XLoop

	inc	dwY

	dec	ch
	jnz	@@YLoop



	popad
	pop		ebp
	ret
endp DrawFireASM






END
