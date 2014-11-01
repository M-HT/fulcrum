#include <cstdint>
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

static uint32_t pPolygonDistanceBuffer1;
static uint32_t pPolygonDistanceBuffer2;
static uint32_t iNumPolygons;

const static float fNum_0 = 0;

#include "m2struct.inc.h"
//ends

//.code


extern "C" uint32_t MV2AddPolygonsASM(uint32_t _pPolygonDistanceBuffer, uint32_t _ppPolygon3Edge, uint32_t _iNumAddPolygons) {
	double fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13;
	uint32_t eax, edx, ecx, edi, ebx, esi, ebp;
	uint32_t stack_var00, stack_var01;




	CurPolygonCounter = ( 0 );

	edi = _pPolygonDistanceBuffer;
	esi = _ppPolygon3Edge;
	ebp = _iNumAddPolygons;


	if ((ebp & ebp) == 0) goto MV2AddPolygonsASM_NoAddPolygons;

MV2AddPolygonsASM_PolygonLoop:
	edx = ( *((uint32_t *)(esi)) );

	ecx = ( ((CMV2Polygon *)edx)->CMV2Polygon__m_iFlag );
	ecx = ecx & ( 1 );
	if (( (int32_t)ecx ) == 0) goto MV2AddPolygonsASM_OneSided;

	eax = ( ((CMV2Polygon *)edx)->CMV2Polygon__m_pDot1 );
	ebx = ( ((CMV2Polygon *)edx)->CMV2Polygon__m_pDot2 );
	ecx = ( ((CMV2Polygon *)edx)->CMV2Polygon__m_pDot3 );

	eax = ( ((CMV2Dot3D *)eax)->CMV2Dot3D__m_pPos );
	ebx = ( ((CMV2Dot3D *)ebx)->CMV2Dot3D__m_pPos );
	ecx = ( ((CMV2Dot3D *)ecx)->CMV2Dot3D__m_pPos );


	fpu_reg10 = ( ((CMV2Dot3DPos *)ebx)->CMV2Dot3DPos__m_fScreenX );
	fpu_reg10 = fpu_reg10 - ( ((CMV2Dot3DPos *)eax)->CMV2Dot3DPos__m_fScreenX );
	fpu_reg11 = ( ((CMV2Dot3DPos *)ebx)->CMV2Dot3DPos__m_fScreenY );
	fpu_reg11 = fpu_reg11 - ( ((CMV2Dot3DPos *)eax)->CMV2Dot3DPos__m_fScreenY );
	fpu_reg12 = ( ((CMV2Dot3DPos *)ecx)->CMV2Dot3DPos__m_fScreenX );
	fpu_reg12 = fpu_reg12 - ( ((CMV2Dot3DPos *)eax)->CMV2Dot3DPos__m_fScreenX );
	fpu_reg13 = ( ((CMV2Dot3DPos *)ecx)->CMV2Dot3DPos__m_fScreenY );
	fpu_reg13 = fpu_reg13 - ( ((CMV2Dot3DPos *)eax)->CMV2Dot3DPos__m_fScreenY );
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

	stack_var00 = ebx;
	stack_var01 = eax;
	ebx = ( ((CMV2Polygon *)edx)->CMV2Polygon__m_pDot3 );
	eax = ( ((CMV2Polygon *)edx)->CMV2Polygon__m_pDot2 );
	((CMV2Polygon *)edx)->CMV2Polygon__m_pDot2 = ebx;
	((CMV2Polygon *)edx)->CMV2Polygon__m_pDot3 = eax;
	eax = stack_var01;
	ebx = stack_var00;

MV2AddPolygonsASM_Clockwise:
	goto MV2AddPolygonsASM_TwoSided;
MV2AddPolygonsASM_OneSided:
	eax = ( ((CMV2Polygon *)edx)->CMV2Polygon__m_pDot1 );
	ebx = ( ((CMV2Polygon *)edx)->CMV2Polygon__m_pDot2 );
	ecx = ( ((CMV2Polygon *)edx)->CMV2Polygon__m_pDot3 );

	eax = ( ((CMV2Dot3D *)eax)->CMV2Dot3D__m_pPos );
	ebx = ( ((CMV2Dot3D *)ebx)->CMV2Dot3D__m_pPos );
	ecx = ( ((CMV2Dot3D *)ecx)->CMV2Dot3D__m_pPos );


	fpu_reg10 = ( ((CMV2Dot3DPos *)ebx)->CMV2Dot3DPos__m_fScreenX );
	fpu_reg10 = fpu_reg10 - ( ((CMV2Dot3DPos *)eax)->CMV2Dot3DPos__m_fScreenX );
	fpu_reg11 = ( ((CMV2Dot3DPos *)ebx)->CMV2Dot3DPos__m_fScreenY );
	fpu_reg11 = fpu_reg11 - ( ((CMV2Dot3DPos *)eax)->CMV2Dot3DPos__m_fScreenY );
	fpu_reg12 = ( ((CMV2Dot3DPos *)ecx)->CMV2Dot3DPos__m_fScreenX );
	fpu_reg12 = fpu_reg12 - ( ((CMV2Dot3DPos *)eax)->CMV2Dot3DPos__m_fScreenX );
	fpu_reg13 = ( ((CMV2Dot3DPos *)ecx)->CMV2Dot3DPos__m_fScreenY );
	fpu_reg13 = fpu_reg13 - ( ((CMV2Dot3DPos *)eax)->CMV2Dot3DPos__m_fScreenY );
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

	fpu_reg10 = ( ((CMV2Dot3DPos *)eax)->CMV2Dot3DPos__m_TransfPos__m_fZ );
	fpu_reg10 = fpu_reg10 + ( ((CMV2Dot3DPos *)ebx)->CMV2Dot3DPos__m_TransfPos__m_fZ );
//st0 = Dot1Z + Dot2Z

	fpu_reg11 = ( ((CMV2Dot3DPos *)ecx)->CMV2Dot3DPos__m_TransfPos__m_fZ );
//st0 = Dot3Z, st1 = Dot1Z + Dot2Z

	fpu_reg10 = fpu_reg10 + fpu_reg11;
//st0 = Dot1Z + Dot2Z + Dot3Z

	fpu_reg10 = fpu_reg10 + ( ((CMV2Polygon *)edx)->CMV2Polygon__m_fAddDistance );

	fpu_reg10 = -fpu_reg10;
//st0 = -(Dot1Z + Dot2Z + Dot3Z)

	((CMV2PolygonDistance *)edi)->CMV2PolygonDistance__m_fAverageDistance = fpu_reg10;
//	not 	ds:[edi].CMV2PolygonDistance__m_fAverageDistance
	((CMV2PolygonDistance *)edi)->CMV2PolygonDistance__m_pPolygon = edx;

	edi = edi + ( 8 );
	CurPolygonCounter = CurPolygonCounter + 1;

MV2AddPolygonsASM_PolygonNotVisible:
	esi = esi + ( 4 );

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2AddPolygonsASM_PolygonLoop;
MV2AddPolygonsASM_NoAddPolygons:



	eax = CurPolygonCounter;
	return eax;
}







extern "C" void MV2SortRenderBufferASM(uint32_t _pPolygonDistanceBuffer1, uint32_t _pPolygonDistanceBuffer2, uint32_t _iNumPolygons) {
	uint32_t eax, edx, ecx, edi, ebx, esi, ebp;




//push	es

	eax = _pPolygonDistanceBuffer1;
	ebx = _pPolygonDistanceBuffer2;
	ecx = _iNumPolygons;
	pPolygonDistanceBuffer1 = eax;
	pPolygonDistanceBuffer2 = ebx;
	iNumPolygons = ecx;

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

	edi = ( (uint32_t)&(SortBuffer[0]) );
//cld
	ecx = ( 256 );
	eax = 0;
	for (; ecx != 0; ecx--, edi+=4) *(uint32_t *)edi = eax;

	ebx = 0;
	esi = pPolygonDistanceBuffer1;
	ebp = iNumPolygons;

MV2SortRenderBufferASM_FrequencyLoop3:
	ebx = (ebx & 0xffffff00) | (uint8_t)(( ((uint8_t *)(esi))[2] ));
	SortBuffer[ebx] = ( SortBuffer[ebx] ) + 1;
	esi = esi + ( 8 );

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2SortRenderBufferASM_FrequencyLoop3;


	edx = 0;
	edi = ( 255 );
	ebp = ( 256 );

MV2SortRenderBufferASM_InitPtrLoop3:
	ecx = ( SortBuffer[edi] );
	SortBuffer[edi] = edx;
	edx = edx + ecx;
	edi = edi - 1;

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2SortRenderBufferASM_InitPtrLoop3;


	ebx = 0;
	esi = pPolygonDistanceBuffer1;
	edi = pPolygonDistanceBuffer2;
	ebp = iNumPolygons;

MV2SortRenderBufferASM_SortLoop3:
	ecx = ( *((uint32_t *)(esi)) );
	ebx = (ebx & 0xffffff00) | (uint8_t)(( ((uint8_t *)(esi))[2] ));
	eax = ( SortBuffer[ebx] );
	edx = ( ((uint32_t *)(esi))[1] );
	*((uint32_t *)(edi + eax * 8)) = ecx;
	*((uint32_t *)(edi + eax * 8 + (4))) = edx;
	SortBuffer[ebx] = ( SortBuffer[ebx] ) + 1;

	esi = esi + ( 8 );

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2SortRenderBufferASM_SortLoop3;

//===>                                                                <===
//===>	Sort Pass 4                                                  <===
//===>                                                                <===

	edi = ( (uint32_t)&(SortBuffer[0]) );
//cld
	ecx = ( 256 );
	eax = 0;
	for (; ecx != 0; ecx--, edi+=4) *(uint32_t *)edi = eax;

	ebx = 0;
	esi = pPolygonDistanceBuffer2;
	ebp = iNumPolygons;

MV2SortRenderBufferASM_FrequencyLoop4:
	ebx = (ebx & 0xffffff00) | (uint8_t)(( ((uint8_t *)(esi))[3] ));
	SortBuffer[ebx] = ( SortBuffer[ebx] ) + 1;
	esi = esi + ( 8 );

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2SortRenderBufferASM_FrequencyLoop4;


	edx = 0;
	edi = ( 255 );
	ebp = ( 256 );

MV2SortRenderBufferASM_InitPtrLoop4:
	ecx = ( SortBuffer[edi] );
	SortBuffer[edi] = edx;
	edx = edx + ecx;
	edi = edi - 1;

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2SortRenderBufferASM_InitPtrLoop4;

	ebx = 0;
	esi = pPolygonDistanceBuffer2;
	edi = pPolygonDistanceBuffer1;
	ebp = iNumPolygons;

MV2SortRenderBufferASM_SortLoop4:
	ecx = ( *((uint32_t *)(esi)) );
	ebx = (ebx & 0xffffff00) | (uint8_t)(( ((uint8_t *)(esi))[3] ));
	eax = ( SortBuffer[ebx] );
	edx = ( ((uint32_t *)(esi))[1] );
	ecx = ecx & ( 0x07fffffff ); // without sign bit (maybe wrong)
	*((uint32_t *)(edi + eax * 8)) = ecx;
	*((uint32_t *)(edi + eax * 8 + (4))) = edx;
	SortBuffer[ebx] = ( SortBuffer[ebx] ) + 1;

	esi = esi + ( 8 );

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2SortRenderBufferASM_SortLoop4;
//pop		es




	return;
}












