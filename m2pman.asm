;/*
.486

.model flat

LOCALS

.data

EXTRN MV2DrawPolygonTASM	:PROC
EXTRN MV2DrawPolygonTfASM	:PROC
EXTRN MV2DrawPolygonTTrASM	:PROC
EXTRN MV2DrawPolygonTfTrASM :PROC
EXTRN MV2DrawPolygonTGASM	:PROC
EXTRN MV2DrawPolygonTBASM	:PROC


public ppPolygons
public pcBackBuffer
public dwXmax

ppPolygons					dd ?
pcBackBuffer				dd ?
dwXmax						dd ?


; profiling
public iPixelCounter
iPixelCounter				dd ?


iNumPolygons				dd ?




fpucw						dw ?
oldfpucw					dw ?
oh_low 						dd ?
co_low						dd ?
co_hi						dd ?

fTemp						dd ?
lTemp						dd ?
wTemp						dw ?
align 4
bTemp						db ?
align 4


PolygonCallTable			dd 8 dup (?)

.code

include m2struct.inc




public InitPolygonCallTableASM
InitPolygonCallTableASM proc
	mov		edx,offset PolygonCallTable
	mov		dword ptr ds:[edx].00h,offset MV2DrawPolygonTASM
	mov		dword ptr ds:[edx].04h,offset MV2DrawPolygonTfASM
	mov		dword ptr ds:[edx].08h,offset MV2DrawPolygonTTrASM
	mov		dword ptr ds:[edx].0ch,offset MV2DrawPolygonTfTrASM
	mov		dword ptr ds:[edx].10h,offset MV2DrawPolygonTGASM
	mov		dword ptr ds:[edx].14h,offset MV2DrawPolygonTBASM

	ret
endp InitPolygonCallTableASM





public MV2DrawPolygonsASM
MV2DrawPolygonsASM proc
	ARG _ppPolygons:DWORD, _iNumPolygons:DWORD, _pcBackBuffer:DWORD, _dwXmax:DWORD
	push	ebp
	mov		ebp,esp
	pushad

	mov 	iPixelCounter,0			;*************

	fstcw   oldfpucw
	mov     ax,oldfpucw
	and     eax,0fcffh
	and		eax,0fbffh
	or		eax,00800h
	mov     fpucw,ax
	fldcw   fpucw

	mov		eax,_ppPolygons
	add		eax,4
	mov		ebx,_iNumPolygons
	mov		ecx,_pcBackBuffer
	shr		ecx,2
	mov		edx,_dwXmax

	mov	   	ppPolygons,eax
	mov		iNumPolygons,ebx
	mov		pcBackBuffer,ecx
	mov		dwXmax,edx

	mov		edx,ppPolygons
	mov		ecx,iNumPolygons
@@PolygonLoop:
	mov		edi,ds:[edx]
	mov		eax,ds:[edi].CMV2Polygon__m_iType

	push	ecx
	push	edx
	call	dword ptr [4*eax].PolygonCallTable
	pop		edx
	pop		ecx

	add		edx,8
	dec		ecx
	jnz		@@PolygonLoop

	fldcw   oldfpucw

	popad
	pop		ebp
	ret
endp MV2DrawPolygonsASM






END