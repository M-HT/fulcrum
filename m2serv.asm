;/*
.486

.model flat

LOCALS

.data
piNumSrcCList				dd ?
piNumDestList				dd ?
iNumSrcCList				dd ?
iNumDestList				dd ?

.code


include m2struct.inc



public MV2SelectClipPolygonsASM
MV2SelectClipPolygonsASM proc
	ARG _ppSrcList:DWORD, _ppSrcCList:DWORD, _ppDestList:DWORD, _iNumSrcList:DWORD, _piNumSrcCList:DWORD, _piNumDestList:DWORD
	push	ebp
	mov		ebp,esp
	pushad

	mov		eax,_piNumSrcCList
	mov		ebx,_piNumDestList
	mov		piNumSrcCList,eax
	mov		piNumDestList,ebx

	mov		eax,ds:[eax]
	mov		ebx,ds:[ebx]
	mov		iNumSrcCList,eax
	mov		iNumDestList,ebx

	mov		edi,_ppDestList
	mov		esi,_ppSrcCList
	mov		edx,_ppSrcList
	mov		ebp,_iNumSrcList
	test	ebp,ebp
	jz		@@NoClippingCheck

@@ClippingCheckLoop:
	push	ebp
	mov		ebp,ds:[edx]
	mov		eax,ds:[ebp].CMV2Polygon__m_pDot1
	mov		ebx,ds:[ebp].CMV2Polygon__m_pDot2
	mov		ecx,ds:[ebp].CMV2Polygon__m_pDot3

	mov		eax,ds:[eax].CMV2Dot3D__m_pPos
	mov		ebx,ds:[ebx].CMV2Dot3D__m_pPos
	mov		ecx,ds:[ecx].CMV2Dot3D__m_pPos

	mov		eax,ds:[eax].CMV2Dot3DPos__m_iClipFlag
	mov		ebx,ds:[ebx].CMV2Dot3DPos__m_iClipFlag
	mov		ecx,ds:[ecx].CMV2Dot3DPos__m_iClipFlag

	mov		ebp,eax
	and		ebp,ebx
	and     ebp,ecx
	jnz		@@NotVisible

	mov		ebp,eax
	or		ebp,ebx
	or		ebp,ecx
	jz		@@InViewSpace
		; To be clipped
	mov		ebp,ds:[edx]
	mov		ds:[esi],ebp
	add		esi,4
	inc		iNumSrcCList
	jmp		@@ToBeClipped

@@InViewSpace:
	mov		ebp,ds:[edx]
	mov     ds:[edi],ebp
	add		edi,4
	inc		iNumDestList
@@ToBeClipped:
@@NotVisible:
	pop		ebp
	add		edx,4

	dec		ebp
	jnz		@@ClippingCheckLoop
@@NoClippingCheck:

	mov		eax,piNumSrcCList
	mov		ebx,piNumDestList
	mov		ecx,iNumSrcCList
	mov		edx,iNumDestList
	mov		ds:[eax],ecx
	mov		ds:[ebx],edx

	popad
	pop		ebp
	ret
endp MV2SelectClipPolygonsASM





public CMV2CopyPolygonListASM
CMV2CopyPolygonListASM proc
	ARG _ppDest:DWORD, _ppSrc:DWORD, _iNumPolygons:DWORD
	push	ebp
	mov		ebp,esp
	pushad
	push	es

	push	ds
	pop		es

	mov		ecx,_iNumPolygons
	mov		edi,_ppDest
	mov		esi,_ppSrc
	cld
	rep		movsd


	pop		es

	popad
	pop		ebp
	ret
endp CMV2CopyPolygonListASM



public Calc32BitLookupASM
Calc32BitLookupASM proc
	ARG _pcPalette:DWORD, _pcSparklesLookup:DWORD
	push	ebp
	mov		ebp,esp
	pushad

	mov		esi,_pcPalette
	mov		edi,_pcSparklesLookup
	mov		edx,256

@@Loop:
	mov		eax,ds:[esi]
	mov		ebx,eax
	mov		ecx,eax
	and		eax,00000ffh
	and		ebx,000ff00h
	and		ecx,0ff0000h
	shl		eax,16
	shr		ecx,16
	or		eax,ebx
	or		eax,ecx

	mov		ds:[edi],eax
	add		esi,3
	add		edi,4

	dec		edx
	jnz		@@Loop

	popad
	pop		ebp
	ret
endp Calc32BitLookupASM





END
