#include "cc.h"
//;/*
//.486

//.model flat

//LOCALS

//.data

//.code


#include "m2struct.inc.h"



extern "C" void MV2SelectClipPolygonsASM(CMV2Polygon **_ppSrcList, CMV2Polygon **_ppSrcCList, CMV2Polygon **_ppDestList, uint32_t _iNumSrcList, uint32_t *_piNumSrcCList, uint32_t *_piNumDestList) {
	uint32_t eax, ecx, ebx, ebp;
	uint32_t iNumSrcCList, iNumDestList;




	iNumSrcCList = *_piNumSrcCList;
	iNumDestList = *_piNumDestList;

	if (_iNumSrcList == 0) goto MV2SelectClipPolygonsASM_NoClippingCheck;

MV2SelectClipPolygonsASM_ClippingCheckLoop:
	eax = ( (*_ppSrcList)->CMV2Polygon__m_pDot1->CMV2Dot3D__m_pPos->CMV2Dot3DPos__m_iClipFlag );
	ebx = ( (*_ppSrcList)->CMV2Polygon__m_pDot2->CMV2Dot3D__m_pPos->CMV2Dot3DPos__m_iClipFlag );
	ecx = ( (*_ppSrcList)->CMV2Polygon__m_pDot3->CMV2Dot3D__m_pPos->CMV2Dot3DPos__m_iClipFlag );

	ebp = eax;
	ebp = ebp & ebx;
	ebp = ebp & ecx;
	if (( (int32_t)ebp ) != 0) goto MV2SelectClipPolygonsASM_NotVisible;

	ebp = eax;
	ebp = ebp | ebx;
	ebp = ebp | ecx;
	if (( (int32_t)ebp ) == 0) goto MV2SelectClipPolygonsASM_InViewSpace;
// To be clipped
	*_ppSrcCList = *_ppSrcList;
	_ppSrcCList++;
	iNumSrcCList = iNumSrcCList + 1;
	goto MV2SelectClipPolygonsASM_ToBeClipped;
MV2SelectClipPolygonsASM_InViewSpace:
	*_ppDestList = *_ppSrcList;
	_ppDestList++;
	iNumDestList = iNumDestList + 1;
MV2SelectClipPolygonsASM_ToBeClipped:
MV2SelectClipPolygonsASM_NotVisible:
	_ppSrcList++;

	_iNumSrcList--;
	if (_iNumSrcList != 0) goto MV2SelectClipPolygonsASM_ClippingCheckLoop;
MV2SelectClipPolygonsASM_NoClippingCheck:

	*_piNumSrcCList = iNumSrcCList;
	*_piNumDestList = iNumDestList;



	return;
}







extern "C" void CMV2CopyPolygonListASM(CMV2Polygon **_ppDest, CMV2Polygon **_ppSrc, uint32_t _iNumPolygons) {



//push	es

//push	ds
//pop		es

//cld
	for (; _iNumPolygons != 0; _iNumPolygons--, _ppSrc++, _ppDest++) *_ppDest = *_ppSrc;


//pop		es



	return;
}





extern "C" void Calc32BitLookupASM(uint8_t *_pcPalette, uint8_t *_pcSparklesLookup) {
	uint32_t eax, edx, ecx, ebx;




	edx = ( 256 );

Calc32BitLookupASM_Loop:
	eax = ( *((uint32_t *)_pcPalette) );
	ebx = eax;
	ecx = eax;
	eax = eax & ( 0x00000ff );
	ebx = ebx & ( 0x000ff00 );
	ecx = ecx & ( 0x0ff0000 );
	eax = eax << ( 16 );
	ecx = ecx >> ( 16 );
	eax = eax | ebx;
	eax = eax | ecx;

	*((uint32_t *)_pcSparklesLookup) = eax;
	_pcPalette += 3;
	_pcSparklesLookup += 4;

	edx = ( (int32_t)edx ) - 1;
	if (( (int32_t)edx ) != 0) goto Calc32BitLookupASM_Loop;



	return;
}







