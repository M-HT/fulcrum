#include "cc.h"
//;/*
//.486

//.model flat

//LOCALS

//data32 SEGMENT PAGE
//.data

static uint32_t CurPolygonCounter;

//align	32
static uint32_t SortBuffer[1024];

const static float fNum_0 = 0;

#include "m2struct.inc.h"
//ends

//.code


extern "C" uint32_t MV2AddPolygonsASM(CMV2PolygonDistance *_pPolygonDistanceBuffer, CMV2Polygon **_ppPolygon3Edge, uint32_t _iNumAddPolygons) {
	realnum fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13;
	CMV2Dot3DPos *eax, *ecx, *ebx;
	CMV2Dot3DPos *stack_var00;
	CMV2Polygon *edx;
	CMV2Dot3D *stack_var01;




	CurPolygonCounter = ( 0 );


	if (_iNumAddPolygons == 0) goto MV2AddPolygonsASM_NoAddPolygons;

MV2AddPolygonsASM_PolygonLoop:
	edx = *_ppPolygon3Edge;

	if (( edx->CMV2Polygon__m_iFlag & 1 ) == 0) goto MV2AddPolygonsASM_OneSided;

	eax = edx->CMV2Polygon__m_pDot1->CMV2Dot3D__m_pPos;
	ebx = edx->CMV2Polygon__m_pDot2->CMV2Dot3D__m_pPos;
	ecx = edx->CMV2Polygon__m_pDot3->CMV2Dot3D__m_pPos;


	fpu_reg10 = ebx->CMV2Dot3DPos__m_fScreenX;
	fpu_reg10 = fpu_reg10 - eax->CMV2Dot3DPos__m_fScreenX;
	fpu_reg11 = ebx->CMV2Dot3DPos__m_fScreenY;
	fpu_reg11 = fpu_reg11 - eax->CMV2Dot3DPos__m_fScreenY;
	fpu_reg12 = ecx->CMV2Dot3DPos__m_fScreenX;
	fpu_reg12 = fpu_reg12 - eax->CMV2Dot3DPos__m_fScreenX;
	fpu_reg13 = ecx->CMV2Dot3DPos__m_fScreenY;
	fpu_reg13 = fpu_reg13 - eax->CMV2Dot3DPos__m_fScreenY;
// st0 = b.y = p3.y - p1.y
// st1 = b.x = p3.x - p1.x
// st2 = a.y = p2.y - p1.y
// st3 = a.x = p2.x - p1.x

	fpu_reg10 = fpu_reg10 * fpu_reg13;
	fpu_reg11 = fpu_reg11 * fpu_reg12;
	fpu_reg10 = fpu_reg11 - fpu_reg10; // a.y*b.x - a.x*b.y > 0

	stack_var00 = eax;

// tasm 3.0 bug (= fstsw)

	if (fpu_reg10 > fNum_0) goto MV2AddPolygonsASM_notClockwise;
	eax = stack_var00;
	goto MV2AddPolygonsASM_Clockwise;
MV2AddPolygonsASM_notClockwise:
	eax = stack_var00;

	stack_var01 = edx->CMV2Polygon__m_pDot2;
	edx->CMV2Polygon__m_pDot2 = edx->CMV2Polygon__m_pDot3;
	edx->CMV2Polygon__m_pDot3 = stack_var01;

MV2AddPolygonsASM_Clockwise:
	goto MV2AddPolygonsASM_TwoSided;
MV2AddPolygonsASM_OneSided:
	eax = edx->CMV2Polygon__m_pDot1->CMV2Dot3D__m_pPos;
	ebx = edx->CMV2Polygon__m_pDot2->CMV2Dot3D__m_pPos;
	ecx = edx->CMV2Polygon__m_pDot3->CMV2Dot3D__m_pPos;


	fpu_reg10 = ebx->CMV2Dot3DPos__m_fScreenX;
	fpu_reg10 = fpu_reg10 - eax->CMV2Dot3DPos__m_fScreenX;
	fpu_reg11 = ebx->CMV2Dot3DPos__m_fScreenY;
	fpu_reg11 = fpu_reg11 - eax->CMV2Dot3DPos__m_fScreenY;
	fpu_reg12 = ecx->CMV2Dot3DPos__m_fScreenX;
	fpu_reg12 = fpu_reg12 - eax->CMV2Dot3DPos__m_fScreenX;
	fpu_reg13 = ecx->CMV2Dot3DPos__m_fScreenY;
	fpu_reg13 = fpu_reg13 - eax->CMV2Dot3DPos__m_fScreenY;
// st0 = b.y = p3.y - p1.y
// st1 = b.x = p3.x - p1.x
// st2 = a.y = p2.y - p1.y
// st3 = a.x = p2.x - p1.x

	fpu_reg10 = fpu_reg10 * fpu_reg13;
	fpu_reg11 = fpu_reg11 * fpu_reg12;
	fpu_reg10 = fpu_reg11 - fpu_reg10; // a.y*b.x - a.x*b.y > 0

	stack_var00 = eax;

// tasm 3.0 bug (= fstsw)

	if (fpu_reg10 <= fNum_0) goto MV2AddPolygonsASM_notPolygonNotVisible;
	eax = stack_var00;
	goto MV2AddPolygonsASM_PolygonNotVisible;
MV2AddPolygonsASM_notPolygonNotVisible:
	eax = stack_var00;
MV2AddPolygonsASM_TwoSided:

	fpu_reg10 = eax->CMV2Dot3DPos__m_TransfPos__m_fZ;
	fpu_reg10 = fpu_reg10 + ebx->CMV2Dot3DPos__m_TransfPos__m_fZ;
//st0 = Dot1Z + Dot2Z

	fpu_reg11 = ecx->CMV2Dot3DPos__m_TransfPos__m_fZ;
//st0 = Dot3Z, st1 = Dot1Z + Dot2Z

	fpu_reg10 = fpu_reg10 + fpu_reg11;
//st0 = Dot1Z + Dot2Z + Dot3Z

	fpu_reg10 = fpu_reg10 + edx->CMV2Polygon__m_fAddDistance;

	fpu_reg10 = -fpu_reg10;
//st0 = -(Dot1Z + Dot2Z + Dot3Z)

	_pPolygonDistanceBuffer->CMV2PolygonDistance__m_fAverageDistance = fpu_reg10;
//	not 	ds:[edi].CMV2PolygonDistance__m_fAverageDistance
	_pPolygonDistanceBuffer->CMV2PolygonDistance__m_pPolygon = edx;

	_pPolygonDistanceBuffer++;
	CurPolygonCounter = CurPolygonCounter + 1;

MV2AddPolygonsASM_PolygonNotVisible:
	_ppPolygon3Edge++;

	_iNumAddPolygons--;
	if (_iNumAddPolygons != 0) goto MV2AddPolygonsASM_PolygonLoop;
MV2AddPolygonsASM_NoAddPolygons:



	return CurPolygonCounter;
}







extern "C" void MV2SortRenderBufferASM(CMV2PolygonDistance *_pPolygonDistanceBuffer1, CMV2PolygonDistance *_pPolygonDistanceBuffer2, uint32_t _iNumPolygons) {
	uint32_t eax, edx, ecx, ebx, ebp;
	CMV2PolygonDistance *esi, *edi;




//push	es

//push	ds
//pop		es
#if 0
//;===>                                                                <===
//;===>	Sort Pass 1                                                  <===
//;===>                                                                <===
//
//	mov		edi,offset SortBuffer
//	;cld
//	mov		ecx,256
//	xor		eax,eax
//	rep		stosd
//
//	xor		ebx,ebx
//	mov		esi,pPolygonDistanceBuffer1
//	mov		ebp,iNumPolygons
//
//@@FrequencyLoop1:
//	mov     bl,ds:[esi]
//	inc		dword ptr ds:[4*ebx].SortBuffer
//	add		esi,8
//
//	dec		ebp
//	jnz		@@FrequencyLoop1
//
//	xor		edx,edx
//	mov		edi,255
//	mov		ebp,256
//
//@@InitPtrLoop1:
//	mov		ecx,ds:[4*edi].SortBuffer
//	mov		ds:[4*edi].SortBuffer,edx
//	add		edx,ecx
//	dec		edi					; mit ebp machbar, aber langsamer..
//
//	dec		ebp
//	jnz		@@InitPtrLoop1
//
//
//	xor		ebx,ebx
//	mov		esi,pPolygonDistanceBuffer1
//	mov		edi,pPolygonDistanceBuffer2
//	mov		ebp,iNumPolygons
//
//@@SortLoop1:
//	mov		ecx,ds:[esi]
//	mov		bl,cl
//	mov		eax,ds:[4*ebx].SortBuffer
//	mov		edx,ds:[esi + 4]
//	mov		ds:[edi + 8*eax],ecx
//	mov		ds:[edi + 8*eax + 4],edx
//	inc		dword ptr ds:[4*ebx].SortBuffer
//
//	add		esi,8
//
//	dec		ebp
//	jnz		@@SortLoop1
//
//;===>                                                                <===
//;===>	Sort Pass 2                                                  <===
//;===>                                                                <===
//
//	mov		edi,offset SortBuffer
//	cld
//	mov		ecx,256
//	xor		eax,eax
//	rep		stosd
//
//	xor		ebx,ebx
//	mov		esi,pPolygonDistanceBuffer2
//	mov		ebp,iNumPolygons
//
//@@FrequencyLoop2:
//	mov     bl,ds:[esi + 1]
//	inc		dword ptr ds:[4*ebx].SortBuffer
//	add		esi,8
//
//	dec		ebp
//	jnz		@@FrequencyLoop2
//
//
//	xor		edx,edx
//	mov		edi,255
//	mov		ebp,256
//
//@@InitPtrLoop2:
//	mov		ecx,ds:[4*edi].SortBuffer
//	mov		ds:[4*edi].SortBuffer,edx
//	add		edx,ecx
//	dec		edi
//
//	dec		ebp
//	jnz		@@InitPtrLoop2
//
//
//	xor		ebx,ebx
//	mov		esi,pPolygonDistanceBuffer2
//	mov		edi,pPolygonDistanceBuffer1
//	mov		ebp,iNumPolygons
//
//@@SortLoop2:
//	mov		ecx,ds:[esi]
//	mov		bl,ch
//	mov		eax,ds:[4*ebx].SortBuffer
//	mov		edx,ds:[esi + 4]
//	mov		ds:[edi + 8*eax],ecx
//	mov		ds:[edi + 8*eax + 4],edx
//	inc		dword ptr ds:[4*ebx].SortBuffer
//
//	add		esi,8
//
//	dec		ebp
//	jnz		@@SortLoop2
#endif
//===>                                                                <===
//===>	Sort Pass 3                                                  <===
//===>                                                                <===

	for (ecx = 0; ecx < 256; ecx++) SortBuffer[ecx] = 0;

	ebx = 0;
	esi = _pPolygonDistanceBuffer1;
	ebp = _iNumPolygons;

MV2SortRenderBufferASM_FrequencyLoop3:
	ebx = ((uint8_t *)esi)[2];
	SortBuffer[ebx] = ( SortBuffer[ebx] ) + 1;
	esi++;

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2SortRenderBufferASM_FrequencyLoop3;


	edx = 0;
	ebp = ( 256 );

MV2SortRenderBufferASM_InitPtrLoop3:
	ecx = ( SortBuffer[ebp - 1] );
	SortBuffer[ebp - 1] = edx;
	edx = edx + ecx;

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2SortRenderBufferASM_InitPtrLoop3;


	ebx = 0;
	esi = _pPolygonDistanceBuffer1;
	edi = _pPolygonDistanceBuffer2;
	ebp = _iNumPolygons;

MV2SortRenderBufferASM_SortLoop3:
	ebx = ((uint8_t *)esi)[2];
	eax = ( SortBuffer[ebx] );
	edi[eax].CMV2PolygonDistance__m_fAverageDistance = esi->CMV2PolygonDistance__m_fAverageDistance;
	edi[eax].CMV2PolygonDistance__m_pPolygon = esi->CMV2PolygonDistance__m_pPolygon;
	SortBuffer[ebx] = ( SortBuffer[ebx] ) + 1;

	esi++;

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2SortRenderBufferASM_SortLoop3;

//===>                                                                <===
//===>	Sort Pass 4                                                  <===
//===>                                                                <===

	for (ecx = 0; ecx < 256; ecx++) SortBuffer[ecx] = 0;

	ebx = 0;
	esi = _pPolygonDistanceBuffer2;
	ebp = _iNumPolygons;

MV2SortRenderBufferASM_FrequencyLoop4:
	ebx = ((uint8_t *)esi)[3];
	SortBuffer[ebx] = ( SortBuffer[ebx] ) + 1;
	esi++;

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2SortRenderBufferASM_FrequencyLoop4;


	edx = 0;
	ebp = ( 256 );

MV2SortRenderBufferASM_InitPtrLoop4:
	ecx = ( SortBuffer[ebp - 1] );
	SortBuffer[ebp - 1] = edx;
	edx = edx + ecx;

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2SortRenderBufferASM_InitPtrLoop4;

	ebx = 0;
	esi = _pPolygonDistanceBuffer2;
	edi = _pPolygonDistanceBuffer1;
	ebp = _iNumPolygons;

MV2SortRenderBufferASM_SortLoop4:
	ecx = *((uint32_t *)esi);
	ebx = ((uint8_t *)esi)[3];
	eax = ( SortBuffer[ebx] );
	ecx = ecx & ( 0x07fffffff ); // without sign bit (maybe wrong)
	*(uint32_t *)&(edi[eax]) = ecx;
	edi[eax].CMV2PolygonDistance__m_pPolygon = esi->CMV2PolygonDistance__m_pPolygon;
	SortBuffer[ebx] = ( SortBuffer[ebx] ) + 1;

	esi++;

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2SortRenderBufferASM_SortLoop4;
//pop		es




	return;
}












