;/*
.486

.model flat

LOCALS

.data
.code

CurPolygonCounter				dd ?

align	32
SortBuffer						dd 1024 dup (?)

pPolygonDistanceBuffer1			dd ?
pPolygonDistanceBuffer2			dd ?
iNumPolygons					dd ?

fNum_0							dd 0

include m2structs.inc



public MV2AddPolygonsASM
MV2AddPolygonsASM proc
	ARG _pPolygonDistanceBuffer:DWORD, _ppPolygon3Edge:DWORD, _iNumAddPolygons:DWORD
	push	ebp
	mov		ebp,esp
	pushad

	mov		CurPolygonCounter,0

	mov		edi,_pPolygonDistanceBuffer
	mov		esi,_ppPolygon3Edge
	mov		ebp,_iNumAddPolygons

	test	ebp,ebp
	jz		@@NoAddPolygons

@@PolygonLoop:
	mov		edx,ds:[esi]

	mov		ecx,ds:[edx].CMV2Polygon__m_iFlag
	and		ecx,1
	je		@@OneSided

	mov		eax,ds:[edx].CMV2Polygon__m_pDot1
	mov		ebx,ds:[edx].CMV2Polygon__m_pDot2
	mov		ecx,ds:[edx].CMV2Polygon__m_pDot3

	mov		eax,ds:[eax].CMV2Dot3D__m_pPos
	mov		ebx,ds:[ebx].CMV2Dot3D__m_pPos
	mov		ecx,ds:[ecx].CMV2Dot3D__m_pPos


	fld		ds:[ebx].CMV2Dot3DPos__m_fScreenX
	fsub	ds:[eax].CMV2Dot3DPos__m_fScreenX
	fld     ds:[ebx].CMV2Dot3DPos__m_fScreenY
	fsub	ds:[eax].CMV2Dot3DPos__m_fScreenY
	fld		ds:[ecx].CMV2Dot3DPos__m_fScreenX
	fsub	ds:[eax].CMV2Dot3DPos__m_fScreenX
	fld		ds:[ecx].CMV2Dot3DPos__m_fScreenY
	fsub	ds:[eax].CMV2Dot3DPos__m_fScreenY
	; st0 = b.y = p3.y - p1.y
	; st1 = b.x = p3.x - p1.x
	; st2 = a.y = p2.y - p1.y
	; st3 = a.x = p2.x - p1.x

	fmulp	st(3),st
	fmulp	st(1),st
	fsubrp	st(1),st            ; a.y*b.x - a.x*b.y > 0

	fcomp	DWORD PTR fNum_0
	push	eax
	fnstsw	ax					; tasm 3.0 bug (= fstsw)
	sahf
	pop		eax

	jna		@@Clockwise

	push	ebx
	push	eax
	mov		ebx,ds:[edx].CMV2Polygon__m_pDot3
	mov		eax,ds:[edx].CMV2Polygon__m_pDot2
	mov		ds:[edx].CMV2Polygon__m_pDot2,ebx
	mov		ds:[edx].CMV2Polygon__m_pDot3,eax
	pop		eax
	pop		ebx

@@Clockwise:
	jmp		@@TwoSided
@@OneSided:
	mov		eax,ds:[edx].CMV2Polygon__m_pDot1
	mov		ebx,ds:[edx].CMV2Polygon__m_pDot2
	mov		ecx,ds:[edx].CMV2Polygon__m_pDot3

	mov		eax,ds:[eax].CMV2Dot3D__m_pPos
	mov		ebx,ds:[ebx].CMV2Dot3D__m_pPos
	mov		ecx,ds:[ecx].CMV2Dot3D__m_pPos


	fld		ds:[ebx].CMV2Dot3DPos__m_fScreenX
	fsub	ds:[eax].CMV2Dot3DPos__m_fScreenX
	fld     ds:[ebx].CMV2Dot3DPos__m_fScreenY
	fsub	ds:[eax].CMV2Dot3DPos__m_fScreenY
	fld		ds:[ecx].CMV2Dot3DPos__m_fScreenX
	fsub	ds:[eax].CMV2Dot3DPos__m_fScreenX
	fld		ds:[ecx].CMV2Dot3DPos__m_fScreenY
	fsub	ds:[eax].CMV2Dot3DPos__m_fScreenY
	; st0 = b.y = p3.y - p1.y
	; st1 = b.x = p3.x - p1.x
	; st2 = a.y = p2.y - p1.y
	; st3 = a.x = p2.x - p1.x

	fmulp	st(3),st
	fmulp	st(1),st
	fsubrp	st(1),st            ; a.y*b.x - a.x*b.y > 0

	fcomp	DWORD PTR fNum_0
	push	eax
	fnstsw	ax					; tasm 3.0 bug (= fstsw)
	sahf
	pop		eax

	ja      @@PolygonNotVisible
@@TwoSided:

	fld		dword ptr ds:[eax].CMV2Dot3DPos__m_TransfPos__m_fZ
	fadd	dword ptr ds:[ebx].CMV2Dot3DPos__m_TransfPos__m_fZ
		;st0 = Dot1Z + Dot2Z

	fld		dword ptr ds:[ecx].CMV2Dot3DPos__m_TransfPos__m_fZ
		;st0 = Dot3Z, st1 = Dot1Z + Dot2Z

	faddp	st(1),st
		;st0 = Dot1Z + Dot2Z + Dot3Z

	fadd	dword ptr ds:[edx].CMV2Polygon__m_fAddDistance

	fchs
		;st0 = -(Dot1Z + Dot2Z + Dot3Z)

	fstp	dword ptr ds:[edi].CMV2PolygonDistance__m_fAverageDistance
;	not 	ds:[edi].CMV2PolygonDistance__m_fAverageDistance
	mov	    ds:[edi].CMV2PolygonDistance__m_pPolygon,edx

	add		edi,8
	inc		CurPolygonCounter

@@PolygonNotVisible:
	add		esi,4

	dec		ebp
	jnz		@@PolygonLoop
@@NoAddPolygons:

	popad
	pop		ebp

	mov		eax,CurPolygonCounter
	ret
endp MV2AddPolygonsASM





public MV2SortRenderBufferASM
MV2SortRenderBufferASM proc
	ARG _pPolygonDistanceBuffer1:DWORD, _pPolygonDistanceBuffer2:DWORD, _iNumPolygons:DWORD
	push	ebp
	mov		ebp,esp
	pushad

	push	es

	mov		eax,_pPolygonDistanceBuffer1
	mov		ebx,_pPolygonDistanceBuffer2
	mov		ecx,_iNumPolygons
	mov		pPolygonDistanceBuffer1,eax
	mov		pPolygonDistanceBuffer2,ebx
	mov		iNumPolygons,ecx

	push	ds
	pop		es
comment &
;===>                                                                <===
;===>	Sort Pass 1                                                  <===
;===>                                                                <===

	mov		edi,offset SortBuffer
	cld
	mov		ecx,256
	xor		eax,eax
	rep		stosd

	xor		ebx,ebx
	mov		esi,pPolygonDistanceBuffer1
	mov		ebp,iNumPolygons

@@FrequencyLoop1:
	mov     bl,ds:[esi]
	inc		dword ptr ds:[4*ebx].SortBuffer
	add		esi,8

	dec		ebp
	jnz		@@FrequencyLoop1

	xor		edx,edx
	mov		edi,255
	mov		ebp,256

@@InitPtrLoop1:
	mov		ecx,ds:[4*edi].SortBuffer
	mov		ds:[4*edi].SortBuffer,edx
	add		edx,ecx
	dec		edi					; mit ebp machbar, aber langsamer..

	dec		ebp
	jnz		@@InitPtrLoop1


	xor		ebx,ebx
	mov		esi,pPolygonDistanceBuffer1
	mov		edi,pPolygonDistanceBuffer2
	mov		ebp,iNumPolygons

@@SortLoop1:
	mov		ecx,ds:[esi]
	mov		bl,cl
	mov		eax,ds:[4*ebx].SortBuffer
	mov		edx,ds:[esi + 4]
	mov		ds:[edi + 8*eax],ecx
	mov		ds:[edi + 8*eax + 4],edx
	inc		dword ptr ds:[4*ebx].SortBuffer

	add		esi,8

	dec		ebp
	jnz		@@SortLoop1

;===>                                                                <===
;===>	Sort Pass 2                                                  <===
;===>                                                                <===

	mov		edi,offset SortBuffer
	cld
	mov		ecx,256
	xor		eax,eax
	rep		stosd

	xor		ebx,ebx
	mov		esi,pPolygonDistanceBuffer2
	mov		ebp,iNumPolygons

@@FrequencyLoop2:
	mov     bl,ds:[esi + 1]
	inc		dword ptr ds:[4*ebx].SortBuffer
	add		esi,8

	dec		ebp
	jnz		@@FrequencyLoop2


	xor		edx,edx
	mov		edi,255
	mov		ebp,256

@@InitPtrLoop2:
	mov		ecx,ds:[4*edi].SortBuffer
	mov		ds:[4*edi].SortBuffer,edx
	add		edx,ecx
	dec		edi

	dec		ebp
	jnz		@@InitPtrLoop2


	xor		ebx,ebx
	mov		esi,pPolygonDistanceBuffer2
	mov		edi,pPolygonDistanceBuffer1
	mov		ebp,iNumPolygons

@@SortLoop2:
	mov		ecx,ds:[esi]
	mov		bl,ch
	mov		eax,ds:[4*ebx].SortBuffer
	mov		edx,ds:[esi + 4]
	mov		ds:[edi + 8*eax],ecx
	mov		ds:[edi + 8*eax + 4],edx
	inc		dword ptr ds:[4*ebx].SortBuffer

	add		esi,8

	dec		ebp
	jnz		@@SortLoop2
&
;===>                                                                <===
;===>	Sort Pass 3                                                  <===
;===>                                                                <===

	mov		edi,offset SortBuffer
	cld
	mov		ecx,256
	xor		eax,eax
	rep		stosd

	xor		ebx,ebx
	mov		esi,pPolygonDistanceBuffer1
	mov		ebp,iNumPolygons

@@FrequencyLoop3:
	mov     bl,ds:[esi + 2]
	inc		dword ptr ds:[4*ebx].SortBuffer
	add		esi,8

	dec		ebp
	jnz		@@FrequencyLoop3


	xor		edx,edx
	mov		edi,255
	mov		ebp,256

@@InitPtrLoop3:
	mov		ecx,ds:[4*edi].SortBuffer
	mov		ds:[4*edi].SortBuffer,edx
	add		edx,ecx
	dec		edi

	dec		ebp
	jnz		@@InitPtrLoop3


	xor		ebx,ebx
	mov		esi,pPolygonDistanceBuffer1
	mov		edi,pPolygonDistanceBuffer2
	mov		ebp,iNumPolygons

@@SortLoop3:
	mov		ecx,ds:[esi]
	mov		bl,ds:[esi + 2]
	mov		eax,ds:[4*ebx].SortBuffer
	mov		edx,ds:[esi + 4]
	mov		ds:[edi + 8*eax],ecx
	mov		ds:[edi + 8*eax + 4],edx
	inc		dword ptr ds:[4*ebx].SortBuffer

	add		esi,8

	dec		ebp
	jnz		@@SortLoop3

;===>                                                                <===
;===>	Sort Pass 4                                                  <===
;===>                                                                <===

	mov		edi,offset SortBuffer
	cld
	mov		ecx,256
	xor		eax,eax
	rep		stosd

	xor		ebx,ebx
	mov		esi,pPolygonDistanceBuffer2
	mov		ebp,iNumPolygons

@@FrequencyLoop4:
	mov     bl,ds:[esi + 3]
	inc		dword ptr ds:[4*ebx].SortBuffer
	add		esi,8

	dec		ebp
	jnz		@@FrequencyLoop4


	xor		edx,edx
	mov		edi,255
	mov		ebp,256

@@InitPtrLoop4:
	mov		ecx,ds:[4*edi].SortBuffer
	mov		ds:[4*edi].SortBuffer,edx
	add		edx,ecx
	dec		edi

	dec		ebp
	jnz		@@InitPtrLoop4

	xor		ebx,ebx
	mov		esi,pPolygonDistanceBuffer2
	mov		edi,pPolygonDistanceBuffer1
	mov		ebp,iNumPolygons

@@SortLoop4:
	mov		ecx,ds:[esi]
	mov		bl,ds:[esi + 3]
	mov		eax,ds:[4*ebx].SortBuffer
	mov		edx,ds:[esi + 4]
	and		ecx,07fffffffh			; without sign bit (maybe wrong)
	mov		ds:[edi + 8*eax],ecx
	mov		ds:[edi + 8*eax + 4],edx
	inc		dword ptr ds:[4*ebx].SortBuffer

	add		esi,8

	dec		ebp
	jnz		@@SortLoop4
	pop		es


	popad
	pop		ebp
	ret
endp MV2SortRenderBufferASM










END
