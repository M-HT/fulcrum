;/*
.486

.model flat

LOCALS

.data
.code


include m2struct.inc



public MV2CalcNormASM
MV2CalcNormASM proc
	ARG _pDot3DPos:DWORD, _iNumDot3DPos:DWORD, _pPolygons:DWORD, _iNumPolygons:DWORD
	push	ebp
	mov		ebp,esp
	pushad



	popad
	pop		ebp
	ret
endp MV2SelectClipPolygonsASM


END
