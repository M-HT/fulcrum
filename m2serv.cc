#include <cstdint>
#include "cc.h"
//;/*
//.486

//.model flat

//LOCALS

//.data
static uint32_t piNumSrcCList;
static uint32_t piNumDestList;
static uint32_t iNumSrcCList;
static uint32_t iNumDestList;

//.code


#include "m2struct.inc.h"



extern "C" void MV2SelectClipPolygonsASM(uint32_t _ppSrcList, uint32_t _ppSrcCList, uint32_t _ppDestList, uint32_t _iNumSrcList, uint32_t _piNumSrcCList, uint32_t _piNumDestList) {
	uint32_t eax, edx, ecx, edi, ebx, esi, ebp;
	uint32_t stack_var00;




	eax = _piNumSrcCList;
	ebx = _piNumDestList;
	piNumSrcCList = eax;
	piNumDestList = ebx;

	eax = ( *((uint32_t *)(eax)) );
	ebx = ( *((uint32_t *)(ebx)) );
	iNumSrcCList = eax;
	iNumDestList = ebx;

	edi = _ppDestList;
	esi = _ppSrcCList;
	edx = _ppSrcList;
	ebp = _iNumSrcList;

	if ((ebp & ebp) == 0) goto MV2SelectClipPolygonsASM_NoClippingCheck;

MV2SelectClipPolygonsASM_ClippingCheckLoop:
	stack_var00 = ebp;
	ebp = ( *((uint32_t *)(edx)) );
	eax = ( ((CMV2Polygon *)ebp)->CMV2Polygon__m_pDot1 );
	ebx = ( ((CMV2Polygon *)ebp)->CMV2Polygon__m_pDot2 );
	ecx = ( ((CMV2Polygon *)ebp)->CMV2Polygon__m_pDot3 );

	eax = ( ((CMV2Dot3D *)eax)->CMV2Dot3D__m_pPos );
	ebx = ( ((CMV2Dot3D *)ebx)->CMV2Dot3D__m_pPos );
	ecx = ( ((CMV2Dot3D *)ecx)->CMV2Dot3D__m_pPos );

	eax = ( ((CMV2Dot3DPos *)eax)->CMV2Dot3DPos__m_iClipFlag );
	ebx = ( ((CMV2Dot3DPos *)ebx)->CMV2Dot3DPos__m_iClipFlag );
	ecx = ( ((CMV2Dot3DPos *)ecx)->CMV2Dot3DPos__m_iClipFlag );

	ebp = eax;
	ebp = ebp & ebx;
	ebp = ebp & ecx;
	if (( (int32_t)ebp ) != 0) goto MV2SelectClipPolygonsASM_NotVisible;

	ebp = eax;
	ebp = ebp | ebx;
	ebp = ebp | ecx;
	if (( (int32_t)ebp ) == 0) goto MV2SelectClipPolygonsASM_InViewSpace;
// To be clipped
	ebp = ( *((uint32_t *)(edx)) );
	*((uint32_t *)(esi)) = ebp;
	esi = esi + ( 4 );
	iNumSrcCList = iNumSrcCList + 1;
	goto MV2SelectClipPolygonsASM_ToBeClipped;
MV2SelectClipPolygonsASM_InViewSpace:
	ebp = ( *((uint32_t *)(edx)) );
	*((uint32_t *)(edi)) = ebp;
	edi = edi + ( 4 );
	iNumDestList = iNumDestList + 1;
MV2SelectClipPolygonsASM_ToBeClipped:
MV2SelectClipPolygonsASM_NotVisible:
	ebp = stack_var00;
	edx = edx + ( 4 );

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2SelectClipPolygonsASM_ClippingCheckLoop;
MV2SelectClipPolygonsASM_NoClippingCheck:

	eax = piNumSrcCList;
	ebx = piNumDestList;
	ecx = iNumSrcCList;
	edx = iNumDestList;
	*((uint32_t *)(eax)) = ecx;
	*((uint32_t *)(ebx)) = edx;



	return;
}







extern "C" void CMV2CopyPolygonListASM(uint32_t _ppDest, uint32_t _ppSrc, uint32_t _iNumPolygons) {
	uint32_t ecx, edi, esi;



//push	es

//push	ds
//pop		es

	ecx = _iNumPolygons;
	edi = _ppDest;
	esi = _ppSrc;
//cld
	for (; ecx != 0; ecx--, esi+=4, edi+=4) *(uint32_t *)edi = *(uint32_t *)esi;


//pop		es



	return;
}





extern "C" void Calc32BitLookupASM(uint32_t _pcPalette, uint32_t _pcSparklesLookup) {
	uint32_t eax, edx, ecx, edi, ebx, esi;




	esi = _pcPalette;
	edi = _pcSparklesLookup;
	edx = ( 256 );

Calc32BitLookupASM_Loop:
	eax = ( *((uint32_t *)(esi)) );
	ebx = eax;
	ecx = eax;
	eax = eax & ( 0x00000ff );
	ebx = ebx & ( 0x000ff00 );
	ecx = ecx & ( 0x0ff0000 );
	eax = eax << ( 16 );
	ecx = ecx >> ( 16 );
	eax = eax | ebx;
	eax = eax | ecx;

	*((uint32_t *)(edi)) = eax;
	esi = esi + ( 3 );
	edi = edi + ( 4 );

	edx = ( (int32_t)edx ) - 1;
	if (( (int32_t)edx ) != 0) goto Calc32BitLookupASM_Loop;



	return;
}







