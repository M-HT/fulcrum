#include <memory.h>
#include "cc.h"
//;/*
//.486

//.model flat

//LOCALS

//.data

extern "C" uint32_t pFireRGBLookUp[256];

//static uint32_t pcBackBuffer1;
//static uint32_t pcBackBuffer2;
//static uint32_t piPolarTab;
//static uint32_t pfDistFunc;
//static uint32_t pfSinTab;
//static uint32_t pfCosTab;
//static uint32_t pcDestBackBuffer;
//static uint32_t dwCrossfadeFactor;


static uint32_t pBuffer[4000];

//dwXCounter			dd ?
//static uint32_t dwYCounter;
//static uint32_t dwYOffset;

//static uint32_t dwXmax;
//static uint32_t dwYmax;
//static uint32_t dwXmaxDiv2;
//static uint32_t dwYmaxDiv2;

const static float fDistanceScaleRZP = 0.25f; // 1/4
//static uint32_t dwX;
//static uint32_t dwY;
//DreamJmpTab			dd 9 dup (?)
//FadeJmpTab			dd 9 dup (?)
//BeamFadeJmpTab			dd 9 dup (?)


//static uint32_t pSrc;
//static uint32_t pDest;
//static uint32_t dwSrcXmax;
//static uint32_t dwSrcYmax;
//static uint32_t dwDestXmax;
//static uint32_t dwDestYmax;

//static uint32_t dwDeltaX;
//static uint32_t dwDeltaY;
//static uint32_t dwCurY;

//const static float fNum2Exp16 = 65536.0f;

//static uint32_t pcSrcBack;

//static uint32_t pcMoveListTab;
//static uint32_t pcMask;
//static uint32_t pcPic;
//static uint32_t pcDestBack;
//static uint32_t dwNum;
//static uint32_t pcFade;
//static uint32_t dwFadeFactor;

//static uint32_t piSinTabX;
//static uint32_t piSinTabY;


//static uint32_t pcAmpTab;
//static uint32_t pcYAmpTab;



//.code

extern "C" void Init2DDreamJumpTableASM(void) {
//push		edx

//mov		edx,offset DreamJmpTab
//mov		dword ptr ds:[edx].00h,offset DreamCase0
//mov		dword ptr ds:[edx].04h,offset DreamCase1
//mov		dword ptr ds:[edx].08h,offset DreamCase2
//mov		dword ptr ds:[edx].0ch,offset DreamCase3
//mov		dword ptr ds:[edx].10h,offset DreamCase4
//mov		dword ptr ds:[edx].14h,offset DreamCase5
//mov		dword ptr ds:[edx].18h,offset DreamCase6
//mov		dword ptr ds:[edx].1ch,offset DreamCase7
//mov		dword ptr ds:[edx].20h,offset DreamCase8

//pop		edx

    memset(pBuffer, 0, 4*4000);

	return;
}








extern "C" void Init2DFadeJumpTableASM(void) {
//push		edx

//mov		edx,offset FadeJmpTab
//mov		dword ptr ds:[edx].00h,offset FadeCase0
//mov		dword ptr ds:[edx].04h,offset FadeCase1
//mov		dword ptr ds:[edx].08h,offset FadeCase2
//mov		dword ptr ds:[edx].0ch,offset FadeCase3
//mov		dword ptr ds:[edx].10h,offset FadeCase4
//mov		dword ptr ds:[edx].14h,offset FadeCase5
//mov		dword ptr ds:[edx].18h,offset FadeCase6
//mov		dword ptr ds:[edx].1ch,offset FadeCase7
//mov		dword ptr ds:[edx].20h,offset FadeCase8

//pop		edx

	return;
}






extern "C" void Init2DBeamFadeJumpTableASM(void) {
//push		edx

//mov		edx,offset BeamFadeJmpTab
//mov		dword ptr ds:[edx].00h,offset BeamFadeCase0
//mov		dword ptr ds:[edx].04h,offset BeamFadeCase1
//mov		dword ptr ds:[edx].08h,offset BeamFadeCase2
//mov		dword ptr ds:[edx].0ch,offset BeamFadeCase3
//mov		dword ptr ds:[edx].10h,offset BeamFadeCase4
//mov		dword ptr ds:[edx].14h,offset BeamFadeCase5
//mov		dword ptr ds:[edx].18h,offset BeamFadeCase6
//mov		dword ptr ds:[edx].1ch,offset BeamFadeCase7
//mov		dword ptr ds:[edx].20h,offset BeamFadeCase8

//pop		edx

	return;
}






extern "C" void Draw2DDreamASM(int8_t *_pBackB1, int8_t *_pBackB2, uint32_t *_pPolarTab, float *_pDistFunc, float *_pSinTab, float *_pCosTab, int8_t *_pDestBBack, uint32_t _dwCFadeFactor, uint32_t _dwXmax, uint32_t _dwYmax) {
	realnum fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13;
	uint32_t eax, edx, ecx, edi, ebx, esi, ebp;
	uint32_t _dwYCounter, _dwYOffset, _dwXmaxDiv2, _dwYmaxDiv2, _dwX, _dwY;
	int8_t *_pLoopDestBBack;
	uint32_t *_pLoopPolarTab;




	_dwXmaxDiv2 = _dwXmax >> ( 1 );
	_dwYmaxDiv2 = _dwYmax >> ( 1 );

	_dwYOffset = 0; //_dwYOffset init.

	_dwYCounter = _dwYmax; //_dwYCounter init.

	fpu_reg10 = fDistanceScaleRZP;

Draw2DDreamASM_YLoop:
	_pLoopPolarTab = _dwYOffset + _pPolarTab;

	edi = 0;
	ebp = _dwXmax;
Draw2DDreamASM_XLoop:
// eax =
// ebx =
// ecx =
// edx =
// esi =
// edi =
// ebp = XCounter

	edx = ( *((uint32_t *)(_pLoopPolarTab)) );
	ecx = edx;
	edx = edx >> ( 16 ); //Distance
	ecx = ecx & ( 0x0ffff );
	*((uint32_t *)(((uintptr_t)&(pBuffer[0])) + edi)) = edx;


	fpu_reg11 = ( ((float *)(_pSinTab))[ecx] );
	fpu_reg12 = ( *((int32_t *)(((uintptr_t)&(pBuffer[0])) + edi)) );
	fpu_reg12 = fpu_reg12 * ( *((float *)(edx + _pDistFunc)) );
	fpu_reg12 = fpu_reg12 * fpu_reg10;
	fpu_reg13 = ( ((float *)(_pCosTab))[ecx] );
//st0 = Cos, st1 = Distance, st2 = Sin, st3 = 1/DSCale
	fpu_reg13 = fpu_reg13 * fpu_reg12;
	{ realnum tmp = fpu_reg11; fpu_reg11 = fpu_reg13; fpu_reg13 = tmp; }
//st0 = Sin, st1 = Distance, st2 = X, st3 = 1/DScale

	fpu_reg12 = fpu_reg12 * fpu_reg13;
	{ realnum tmp = fpu_reg12; fpu_reg12 = fpu_reg11; fpu_reg11 = tmp; }
//st0 = X, st1 = Y, st2 = 1/DScale

	_dwX = (int32_t)round(fpu_reg12);
	_dwY = (int32_t)round(fpu_reg11);
//st0 = 1/DScale

	ecx = _dwX;
	edx = _dwY;
	ecx = ecx + _dwXmaxDiv2;
	edx = edx + _dwYmaxDiv2;

	edx = ( (int32_t)edx ) * ( (int32_t)_dwXmax );
	ecx = ecx + edx;
	edx = ecx;

	ecx = ecx << ( 2 );
	edx = edx << ( 2 );

	ecx = ( *((uint32_t *)(ecx + _pBackB1)) ); //Col1
	edx = ( *((uint32_t *)(edx + _pBackB2)) ); //Col1

	*((uint32_t *)(((uintptr_t)&(pBuffer[0])) + edi + 4)) = ecx;
	*((uint32_t *)(((uintptr_t)&(pBuffer[0])) + edi + 8)) = edx;

	_pLoopPolarTab = _pLoopPolarTab + ( 1 );
	edi = edi + ( 12 );

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto Draw2DDreamASM_XLoop;

	esi = 0;
	edi = _dwYOffset;
	edi = edi << ( 2 );
	_pLoopDestBBack = edi + _pDestBBack; // edi init.
	ebp = _dwXmax; // ebp init.
Draw2DDreamASM_CopyXLoop:
	eax = _dwCFadeFactor;
	eax = eax - ( *((uint32_t *)(((uintptr_t)&(pBuffer[0])) + esi)) );
	eax = ( (int32_t)eax ) >> ( 7 );


	if ( (( (int32_t)eax ) - ( 0 )) >= 0) goto Draw2DDreamASM_FadeMinOK;
	eax = 0;
Draw2DDreamASM_FadeMinOK:

	if (eax <= ( 8 )) goto Draw2DDreamASM_FadeMaxOK;
	eax = ( 8 );
Draw2DDreamASM_FadeMaxOK:

//jmp		ds:[4*eax].DreamJmpTab
	switch (eax) {
	case 0:
//DreamCase0:
	ecx = ( *((uint32_t *)(((uintptr_t)&(pBuffer[0])) + esi + 4)) );
	*((uint32_t *)(_pLoopDestBBack)) = ecx;
//jmp		@@FadeDone
	break;
	case 1:
//DreamCase1:
	eax = ( *((uint32_t *)(((uintptr_t)&(pBuffer[0])) + esi + 4)) );
	ecx = ( *((uint32_t *)(((uintptr_t)&(pBuffer[0])) + esi + 8)) );
	ebx = eax;
	edx = ecx;

	eax = eax & ( 0x0f8f8f8f8 ); //1
	ecx = ecx & ( 0x0f8f8f8f8 ); //2
	ebx = ebx & ( 0x007070707 ); //1f
	edx = edx & ( 0x007070707 ); //2f

	ecx = ecx - eax;
	edx = edx - ebx;

	eax = ( eax * 8 );
	ebx = ( ebx * 8 );
	eax = eax + ecx;
	ebx = ebx + edx;

	ebx = ebx & ( 0x0f8f8f8f8 );
	ebx = ebx + eax;
	ebx = ebx >> ( 3 );
	*((uint32_t *)(_pLoopDestBBack)) = ebx;

//jmp		@@FadeDone
	break;
	case 2:
//DreamCase2:
	eax = ( *((uint32_t *)(((uintptr_t)&(pBuffer[0])) + esi + 4)) );
	ecx = ( *((uint32_t *)(((uintptr_t)&(pBuffer[0])) + esi + 8)) );
	ebx = eax;
	edx = ecx;

	eax = eax & ( 0x0f8f8f8f8 );
	ecx = ecx & ( 0x0f8f8f8f8 );
	ebx = ebx & ( 0x007070707 );
	edx = edx & ( 0x007070707 );

	eax = ( eax + eax * 2 );
	ebx = ( ebx + ebx * 2 );
	ecx = ( ecx * 2 );
	edx = ( edx * 2 );
	eax = ( eax * 2 );
	ebx = ( ebx * 2 );

	eax = eax + ecx;
	ebx = ebx + edx;

	ebx = ebx & ( 0x0f8f8f8f8 );
	ebx = ebx + eax;
	ebx = ebx >> ( 3 );
	*((uint32_t *)(_pLoopDestBBack)) = ebx;
//jmp		@@FadeDone
	break;
	case 3:
//DreamCase3:
	eax = ( *((uint32_t *)(((uintptr_t)&(pBuffer[0])) + esi + 4)) );
	ecx = ( *((uint32_t *)(((uintptr_t)&(pBuffer[0])) + esi + 8)) );
	ebx = eax;
	edx = ecx;

	eax = eax & ( 0x0f8f8f8f8 );
	ecx = ecx & ( 0x0f8f8f8f8 );
	ebx = ebx & ( 0x007070707 );
	edx = edx & ( 0x007070707 );

	eax = ( eax + eax * 4 );
	ebx = ( ebx + ebx * 4 );
	ecx = ( ecx + ecx * 2 );
	edx = ( edx + edx * 2 );

	eax = eax + ecx;
	ebx = ebx + edx;

	ebx = ebx & ( 0x0f8f8f8f8 );
	ebx = ebx + eax;
	ebx = ebx >> ( 3 );
	*((uint32_t *)(_pLoopDestBBack)) = ebx;
//jmp		@@FadeDone
	break;
	case 4:
//DreamCase4:
	eax = ( *((uint32_t *)(((uintptr_t)&(pBuffer[0])) + esi + 4)) );
	ecx = ( *((uint32_t *)(((uintptr_t)&(pBuffer[0])) + esi + 8)) );
	ebx = eax;
	edx = ecx;

	eax = eax & ( 0x0f8f8f8f8 );
	ecx = ecx & ( 0x0f8f8f8f8 );
	ebx = ebx & ( 0x007070707 );
	edx = edx & ( 0x007070707 );

	eax = ( eax * 4 );
	ebx = ( ebx * 4 );
	ecx = ( ecx * 4 );
	edx = ( edx * 4 );

	eax = eax + ecx;
	ebx = ebx + edx;

	ebx = ebx & ( 0x0f8f8f8f8 );
	ebx = ebx + eax;
	ebx = ebx >> ( 3 );
	*((uint32_t *)(_pLoopDestBBack)) = ebx;
//jmp		@@FadeDone
	break;
	case 5:
//DreamCase5:
	eax = ( *((uint32_t *)(((uintptr_t)&(pBuffer[0])) + esi + 4)) );
	ecx = ( *((uint32_t *)(((uintptr_t)&(pBuffer[0])) + esi + 8)) );
	ebx = eax;
	edx = ecx;

	eax = eax & ( 0x0f8f8f8f8 );
	ecx = ecx & ( 0x0f8f8f8f8 );
	ebx = ebx & ( 0x007070707 );
	edx = edx & ( 0x007070707 );

	eax = ( eax + eax * 2 );
	ebx = ( ebx + ebx * 2 );
	ecx = ( ecx + ecx * 4 );
	edx = ( edx + edx * 4 );

	eax = eax + ecx;
	ebx = ebx + edx;

	ebx = ebx & ( 0x0f8f8f8f8 );
	ebx = ebx + eax;
	ebx = ebx >> ( 3 );
	*((uint32_t *)(_pLoopDestBBack)) = ebx;
//jmp		@@FadeDone
	break;
	case 6:
//DreamCase6:
	eax = ( *((uint32_t *)(((uintptr_t)&(pBuffer[0])) + esi + 4)) );
	ecx = ( *((uint32_t *)(((uintptr_t)&(pBuffer[0])) + esi + 8)) );
	ebx = eax;
	edx = ecx;

	eax = eax & ( 0x0f8f8f8f8 );
	ecx = ecx & ( 0x0f8f8f8f8 );
	ebx = ebx & ( 0x007070707 );
	edx = edx & ( 0x007070707 );

	ecx = ( ecx + ecx * 2 );
	edx = ( edx + edx * 2 );
	eax = ( eax * 2 );
	ebx = ( ebx * 2 );
	ecx = ( ecx * 2 );
	edx = ( edx * 2 );

	eax = eax + ecx;
	ebx = ebx + edx;

	ebx = ebx & ( 0x0f8f8f8f8 );
	ebx = ebx + eax;
	ebx = ebx >> ( 3 );
	*((uint32_t *)(_pLoopDestBBack)) = ebx;
//jmp		@@FadeDone
	break;
	case 7:
//DreamCase7:
	eax = ( *((uint32_t *)(((uintptr_t)&(pBuffer[0])) + esi + 4)) );
	ecx = ( *((uint32_t *)(((uintptr_t)&(pBuffer[0])) + esi + 8)) );
	ebx = eax;
	edx = ecx;

	eax = eax & ( 0x0f8f8f8f8 );
	ecx = ecx & ( 0x0f8f8f8f8 );
	ebx = ebx & ( 0x007070707 );
	edx = edx & ( 0x007070707 );

	eax = eax - ecx;
	ebx = ebx - edx;

	ecx = ( ecx * 8 );
	edx = ( edx * 8 );

	eax = eax + ecx;
	ebx = ebx + edx;

	ebx = ebx & ( 0x0f8f8f8f8 );
	ebx = ebx + eax;
	ebx = ebx >> ( 3 );
	*((uint32_t *)(_pLoopDestBBack)) = ebx;
//jmp		@@FadeDone
	break;
	case 8:
//DreamCase8:
	ecx = ( *((uint32_t *)(((uintptr_t)&(pBuffer[0])) + esi + 8)) );
	*((uint32_t *)(_pLoopDestBBack)) = ecx;
//jmp		@@FadeDone
	break;
//@@FadeDone:
	}

	esi = esi + ( 12 );
	_pLoopDestBBack = _pLoopDestBBack + ( 4 );

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto Draw2DDreamASM_CopyXLoop;

	eax = _dwXmax;
	_dwYOffset = _dwYOffset + eax;
	_dwYCounter = ( (int32_t)_dwYCounter ) - 1;
	if (( (int32_t)_dwYCounter ) != 0) goto Draw2DDreamASM_YLoop;

//ffree		st(0)




	return;
}







extern "C" void Scale32ASM(char *pcSrc, char *pcDest, int iSrcXmax, int iSrcYmax, int iDestXmax, int iDestYmax)
{
    uint32_t *dest = (uint32_t *)pcDest;
    unsigned int dwCurY = 0;
    int dwDeltaX = (((int64_t)iSrcXmax) << 16) / iDestXmax;
    int dwDeltaY = (((int64_t)iSrcYmax) << 16) / iDestYmax;

    for (int dwYCounter = iDestYmax; dwYCounter != 0; dwYCounter--)
    {
        uint32_t *src = ((uint32_t *)pcSrc) + (dwCurY >> 16) * iSrcXmax;
        unsigned int dwCurX = 0;
        for (int dwXCounter = iDestXmax; dwXCounter != 0; dwXCounter--)
        {
            *dest = src[dwCurX >> 16];
            dwCurX += dwDeltaX;
            dest++;
        }
        dwCurY += dwDeltaY;
    }
}







extern "C" void Scale8ASM(char *pcSrc, char *pcDest, int iSrcXmax, int iSrcYmax, int iDestXmax, int iDestYmax)
{
    uint8_t *dest = (uint8_t *)pcDest;
    unsigned int dwCurY = 0;
    int dwDeltaX = (((int64_t)iSrcXmax) << 16) / iDestXmax;
    int dwDeltaY = (((int64_t)iSrcYmax) << 16) / iDestYmax;

    for (int dwYCounter = iDestYmax; dwYCounter != 0; dwYCounter--)
    {
        uint8_t *src = ((uint8_t *)pcSrc) + (dwCurY >> 16) * iSrcXmax;
        unsigned int dwCurX = 0;
        for (int dwXCounter = iDestXmax; dwXCounter != 0; dwXCounter--)
        {
            *dest = src[dwCurX >> 16];
            dwCurX += dwDeltaX;
            dest++;
        }
        dwCurY += dwDeltaY;
    }
}









extern "C" void Draw2DEyeASM(uint8_t *_pMoveListTab, int8_t *_pMask, int8_t *_pPic, int8_t *_pSrcBack, int8_t *_pDestBack, uint32_t _dwNum) {
	uint32_t eax, ebx, ebp;
	uint8_t *stack_var00;
	int8_t *edx, *ecx, *edi, *esi;




	stack_var00 = _pMoveListTab;
	ecx = _pMask;
	edx = _pPic;
	esi = _pSrcBack;
	edi = _pDestBack;
	ebp = _dwNum;

Draw2DEyeASM_Loop:
// eax =
// stack_var00 = MoveListTab
// ecx = Mask
// edx = Pic
// esi = SrcBack
// edi = DestBack
// ebp = counter
	if ( (( *((int8_t *)(ecx)) ) - ( 0 )) == 0) goto Draw2DEyeASM_DontDraw;

	eax = ( *((uint32_t *)(stack_var00)) ); // from Movelist: offset
	ebx = ( ((uint32_t *)(esi))[eax] ); // col from SrcBackBuffer

	eax = ( *((uint32_t *)(edx)) ); // col from Pic
	ebx = ebx & ( 0x0fefefefe );
	eax = eax & ( 0x0fefefefe );
	ebx = ebx + eax;
	ebx = ebx >> ( 1 );

	*((uint32_t *)(edi)) = ebx; // setpixel
	goto Draw2DEyeASM_Drawed;
Draw2DEyeASM_DontDraw:
	eax = ( *((uint32_t *)(edx)) ); // col from Pic
	eax = eax & ( 0x0fefefefe );
	eax = eax >> ( 1 );
	*((uint32_t *)(edi)) = eax; // setpixel
Draw2DEyeASM_Drawed:
	ecx = ecx + 1;
	edx = edx + ( 4 );
	stack_var00 = stack_var00 + ( 4 );
	edi = edi + ( 4 );

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto Draw2DEyeASM_Loop;



	return;
}








extern "C" void Draw2DFadeASM(int8_t *_pBackBuffer1, int8_t *_pBackBuffer2, int8_t *_pDestBack, int8_t *_pFade, uint32_t _dwFadeFactor, uint32_t _dwNum) {
	uint32_t eax, edx, ecx, ebx;
	uint32_t stack_var00;
	int8_t *edi, *esi, *ebp, *stack_var01;





	ecx = _dwNum;
	stack_var01 = _pFade;
	esi = _pBackBuffer1;
	edi = _pDestBack;
	ebp = _pBackBuffer2;
Draw2DFadeASM_Loop:
// eax =
// ebx =
// ecx = Counter
// stack_var01 = FadePic
// esi = SrcBack1
// edi = DestBack
// ebp = SrcBack2
	ebx = 0;

	stack_var00 = ecx;
	ebx = (ebx & 0xffffff00) | (uint8_t)(( *((uint8_t *)(stack_var01)) ));
	ebx = ebx + _dwFadeFactor;
	ebx = ( (int32_t)ebx ) >> ( 2 );


	if ( (( (int32_t)ebx ) - ( 0 )) >= 0) goto Draw2DFadeASM_FadeMinOK;
	ebx = 0;
Draw2DFadeASM_FadeMinOK:

	if (ebx <= ( 8 )) goto Draw2DFadeASM_FadeMaxOK;
	ebx = ( 8 );
Draw2DFadeASM_FadeMaxOK:

//jmp		ds:[4*ebx].FadeJmpTab
	switch (ebx) {
	case 0:
//FadeCase0:
	ecx = ( *((uint32_t *)(esi)) );
	*((uint32_t *)(edi)) = ecx;
//jmp		@@FadeDone
	break;
	case 1:
//FadeCase1:
	eax = ( *((uint32_t *)(esi)) );
	ecx = ( *((uint32_t *)(ebp)) );
	ebx = eax;
	edx = ecx;

	eax = eax & ( 0x0f8f8f8f8 ); //1
	ecx = ecx & ( 0x0f8f8f8f8 ); //2
	ebx = ebx & ( 0x007070707 ); //1f
	edx = edx & ( 0x007070707 ); //2f

	ecx = ecx - eax;
	edx = edx - ebx;

	eax = ( eax * 8 );
	ebx = ( ebx * 8 );
	eax = eax + ecx;
	ebx = ebx + edx;

	ebx = ebx & ( 0x0f8f8f8f8 );
	ebx = ebx + eax;
	ebx = ebx >> ( 3 );
	*((uint32_t *)(edi)) = ebx;

//jmp		@@FadeDone
	break;
	case 2:
//FadeCase2:
	eax = ( *((uint32_t *)(esi)) );
	ecx = ( *((uint32_t *)(ebp)) );
	ebx = eax;
	edx = ecx;

	eax = eax & ( 0x0f8f8f8f8 );
	ecx = ecx & ( 0x0f8f8f8f8 );
	ebx = ebx & ( 0x007070707 );
	edx = edx & ( 0x007070707 );

	eax = ( eax + eax * 2 );
	ebx = ( ebx + ebx * 2 );
	ecx = ( ecx * 2 );
	edx = ( edx * 2 );
	eax = ( eax * 2 );
	ebx = ( ebx * 2 );

	eax = eax + ecx;
	ebx = ebx + edx;

	ebx = ebx & ( 0x0f8f8f8f8 );
	ebx = ebx + eax;
	ebx = ebx >> ( 3 );
	*((uint32_t *)(edi)) = ebx;
//jmp		@@FadeDone
	break;
	case 3:
//FadeCase3:
	eax = ( *((uint32_t *)(esi)) );
	ecx = ( *((uint32_t *)(ebp)) );
	ebx = eax;
	edx = ecx;

	eax = eax & ( 0x0f8f8f8f8 );
	ecx = ecx & ( 0x0f8f8f8f8 );
	ebx = ebx & ( 0x007070707 );
	edx = edx & ( 0x007070707 );

	eax = ( eax + eax * 4 );
	ebx = ( ebx + ebx * 4 );
	ecx = ( ecx + ecx * 2 );
	edx = ( edx + edx * 2 );

	eax = eax + ecx;
	ebx = ebx + edx;

	ebx = ebx & ( 0x0f8f8f8f8 );
	ebx = ebx + eax;
	ebx = ebx >> ( 3 );
	*((uint32_t *)(edi)) = ebx;
//jmp		@@FadeDone
	break;
	case 4:
//FadeCase4:
	eax = ( *((uint32_t *)(esi)) );
	ecx = ( *((uint32_t *)(ebp)) );
	ebx = eax;
	edx = ecx;

	eax = eax & ( 0x0f8f8f8f8 );
	ecx = ecx & ( 0x0f8f8f8f8 );
	ebx = ebx & ( 0x007070707 );
	edx = edx & ( 0x007070707 );

	eax = ( eax * 4 );
	ebx = ( ebx * 4 );
	ecx = ( ecx * 4 );
	edx = ( edx * 4 );

	eax = eax + ecx;
	ebx = ebx + edx;

	ebx = ebx & ( 0x0f8f8f8f8 );
	ebx = ebx + eax;
	ebx = ebx >> ( 3 );
	*((uint32_t *)(edi)) = ebx;
//jmp		@@FadeDone
	break;
	case 5:
//FadeCase5:
	eax = ( *((uint32_t *)(esi)) );
	ecx = ( *((uint32_t *)(ebp)) );
	ebx = eax;
	edx = ecx;

	eax = eax & ( 0x0f8f8f8f8 );
	ecx = ecx & ( 0x0f8f8f8f8 );
	ebx = ebx & ( 0x007070707 );
	edx = edx & ( 0x007070707 );

	eax = ( eax + eax * 2 );
	ebx = ( ebx + ebx * 2 );
	ecx = ( ecx + ecx * 4 );
	edx = ( edx + edx * 4 );

	eax = eax + ecx;
	ebx = ebx + edx;

	ebx = ebx & ( 0x0f8f8f8f8 );
	ebx = ebx + eax;
	ebx = ebx >> ( 3 );
	*((uint32_t *)(edi)) = ebx;
//jmp		@@FadeDone
	break;
	case 6:
//FadeCase6:
	eax = ( *((uint32_t *)(esi)) );
	ecx = ( *((uint32_t *)(ebp)) );
	ebx = eax;
	edx = ecx;

	eax = eax & ( 0x0f8f8f8f8 );
	ecx = ecx & ( 0x0f8f8f8f8 );
	ebx = ebx & ( 0x007070707 );
	edx = edx & ( 0x007070707 );

	ecx = ( ecx + ecx * 2 );
	edx = ( edx + edx * 2 );
	eax = ( eax * 2 );
	ebx = ( ebx * 2 );
	ecx = ( ecx * 2 );
	edx = ( edx * 2 );

	eax = eax + ecx;
	ebx = ebx + edx;

	ebx = ebx & ( 0x0f8f8f8f8 );
	ebx = ebx + eax;
	ebx = ebx >> ( 3 );
	*((uint32_t *)(edi)) = ebx;
//jmp		@@FadeDone
	break;
	case 7:
//FadeCase7:
	eax = ( *((uint32_t *)(esi)) );
	ecx = ( *((uint32_t *)(ebp)) );
	ebx = eax;
	edx = ecx;

	eax = eax & ( 0x0f8f8f8f8 );
	ecx = ecx & ( 0x0f8f8f8f8 );
	ebx = ebx & ( 0x007070707 );
	edx = edx & ( 0x007070707 );

	eax = eax - ecx;
	ebx = ebx - edx;

	ecx = ( ecx * 8 );
	edx = ( edx * 8 );

	eax = eax + ecx;
	ebx = ebx + edx;

	ebx = ebx & ( 0x0f8f8f8f8 );
	ebx = ebx + eax;
	ebx = ebx >> ( 3 );
	*((uint32_t *)(edi)) = ebx;
//jmp		@@FadeDone
	break;
	case 8:
//FadeCase8:
	ecx = ( *((uint32_t *)(ebp)) );
	*((uint32_t *)(edi)) = ecx;
//jmp		@@FadeDone
	break;
//@@FadeDone:
	}
	ecx = stack_var00;
	esi = esi + ( 4 );
	edi = edi + ( 4 );
	ebp = ebp + ( 4 );

	stack_var01 = stack_var01 + 1;
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto Draw2DFadeASM_Loop;



	return;
}





























extern "C" void Draw2DBeamFadeASM(int8_t *_pBackB1, int8_t *_pBackB2, int8_t *_pDestB, uint32_t _dwFFactor, uint32_t _dwXmax, uint32_t _dwYmax, int32_t *_pSinTabX, int32_t *_pSinTabY) {
	uint32_t eax, edx, ecx, edi, ebx, esi, ebp;
	uint32_t _dwYCounter, _dwY;
	int8_t *_pLoopBackB1, *_pLoopBackB2, *_pLoopDestB;




	_dwYCounter = _dwYmax;

	_dwY = 0;

Draw2DBeamFadeASM_YLoop:
	ebx = _dwY;

	ecx = ebx;
	ecx = ( (int32_t)ecx ) * ( (int32_t)_dwXmax );
	ecx = ecx << ( 2 );
	_pLoopBackB1 = ecx + _pBackB1;

	esi = ( ((uint32_t *)(_pSinTabY))[ebx] );
	esi = ( (int32_t)esi ) * ( (int32_t)_dwXmax );
	esi = esi << ( 2 );
	_pLoopBackB2 = esi + _pBackB2; // esi init.

	ebx = 0; // ebx init.
	edi = 0; // edi init.
	ebp = _dwXmax; // ebp init.


Draw2DBeamFadeASM_BeamXLoop:
// eax =
// ebx =
// _pLoopBackB1 = pBackBuffer1 + (YOffset + DeltaSinY)
// edx =
// _pLoopBackB2 = pBackBuffer2 + (YOffset + DeltaSinY)
// edi = pBuffer == x
// ebp = XCounter

// st0 = Amplitude
	eax = ( ((uint32_t *)(_pSinTabX))[edi] );

	ebx = ( ((uint32_t *)(_pLoopBackB1))[edi] );
	*((uint32_t *)(((uintptr_t)&(pBuffer[0])) + edi * 8 + 0)) = ebx;

	eax = ( ((uint32_t *)(_pLoopBackB2))[eax] );
	*((uint32_t *)(((uintptr_t)&(pBuffer[0])) + edi * 8 + 4)) = eax;

	edi = edi + 1;
	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto Draw2DBeamFadeASM_BeamXLoop;


	esi = 0;
	edi = _dwY;
	edi = ( (int32_t)edi ) * ( (int32_t)_dwXmax );
	edi = edi << ( 2 );
	_pLoopDestB = edi + _pDestB;

	ebp = _dwXmax;

	eax = _dwFFactor;

	if ( (( (int32_t)eax ) - ( 0 )) >= 0) goto Draw2DBeamFadeASM_FadeFactorMinOK;
	eax = 0;
Draw2DBeamFadeASM_FadeFactorMinOK:

	if (eax <= ( 8 )) goto Draw2DBeamFadeASM_FadeFactorMaxOK;
	eax = ( 8 );
Draw2DBeamFadeASM_FadeFactorMaxOK:
//jmp	ds:[eax*4].BeamFadeJmpTab
	switch (eax) {

// eax =
// ebx =
// ecx =
// edx =
// esi = pBuffer
// _pLoopDestB = dest
// ebp = XCounter


	case 0:
Draw2DBeamFadeASM_BeamFadeCase0:
	eax = ( *((uint32_t *)(((uintptr_t)&(pBuffer[0])) + esi)) );
	*((uint32_t *)(_pLoopDestB)) = eax;

	esi = esi + ( 8 );
	_pLoopDestB = _pLoopDestB + ( 4 );

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto Draw2DBeamFadeASM_BeamFadeCase0;
//jmp	@@Done
	break;
	case 1:
Draw2DBeamFadeASM_BeamFadeCase1:
	eax = ( *((uint32_t *)(((uintptr_t)&(pBuffer[0])) + esi + 0)) );
	ecx = ( *((uint32_t *)(((uintptr_t)&(pBuffer[0])) + esi + 4)) );
	ebx = eax;
	edx = ecx;


	if ( (( (int32_t)ecx ) - ( 0 )) != 0) goto Draw2DBeamFadeASM_NotBlack1;
	*((uint32_t *)(_pLoopDestB)) = eax;
	esi = esi + ( 8 );
	_pLoopDestB = _pLoopDestB + ( 4 );
	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto Draw2DBeamFadeASM_BeamFadeCase1;
//jmp	@@Done
	break;
Draw2DBeamFadeASM_NotBlack1:

	eax = eax & ( 0x0f8f8f8f8 ); //1
	ecx = ecx & ( 0x0f8f8f8f8 ); //2
	ebx = ebx & ( 0x007070707 ); //1f
	edx = edx & ( 0x007070707 ); //2f

	ecx = ecx - eax;
	edx = edx - ebx;

	eax = ( eax * 8 );
	ebx = ( ebx * 8 );
	eax = eax + ecx;
	ebx = ebx + edx;

	ebx = ebx & ( 0x0f8f8f8f8 );
	ebx = ebx + eax;
	ebx = ebx >> ( 3 );
	*((uint32_t *)(_pLoopDestB)) = ebx;

	esi = esi + ( 8 );
	_pLoopDestB = _pLoopDestB + ( 4 );
	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto Draw2DBeamFadeASM_BeamFadeCase1;
//jmp	@@Done
	break;
	case 2:
Draw2DBeamFadeASM_BeamFadeCase2:
	eax = ( *((uint32_t *)(((uintptr_t)&(pBuffer[0])) + esi + 0)) );
	ecx = ( *((uint32_t *)(((uintptr_t)&(pBuffer[0])) + esi + 4)) );
	ebx = eax;
	edx = ecx;


	if ( (( (int32_t)ecx ) - ( 0 )) != 0) goto Draw2DBeamFadeASM_NotBlack2;
	*((uint32_t *)(_pLoopDestB)) = eax;
	esi = esi + ( 8 );
	_pLoopDestB = _pLoopDestB + ( 4 );
	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto Draw2DBeamFadeASM_BeamFadeCase2;
//jmp	@@Done
	break;
Draw2DBeamFadeASM_NotBlack2:

	eax = eax & ( 0x0f8f8f8f8 );
	ecx = ecx & ( 0x0f8f8f8f8 );
	ebx = ebx & ( 0x007070707 );
	edx = edx & ( 0x007070707 );

	eax = ( eax + eax * 2 );
	ebx = ( ebx + ebx * 2 );
	ecx = ( ecx * 2 );
	edx = ( edx * 2 );
	eax = ( eax * 2 );
	ebx = ( ebx * 2 );

	eax = eax + ecx;
	ebx = ebx + edx;

	ebx = ebx & ( 0x0f8f8f8f8 );
	ebx = ebx + eax;
	ebx = ebx >> ( 3 );
	*((uint32_t *)(_pLoopDestB)) = ebx;

	esi = esi + ( 8 );
	_pLoopDestB = _pLoopDestB + ( 4 );
	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto Draw2DBeamFadeASM_BeamFadeCase2;
//jmp	@@Done
	break;
	case 3:
Draw2DBeamFadeASM_BeamFadeCase3:
	eax = ( *((uint32_t *)(((uintptr_t)&(pBuffer[0])) + esi + 0)) );
	ecx = ( *((uint32_t *)(((uintptr_t)&(pBuffer[0])) + esi + 4)) );
	ebx = eax;
	edx = ecx;


	if ( (( (int32_t)ecx ) - ( 0 )) != 0) goto Draw2DBeamFadeASM_NotBlack3;
	*((uint32_t *)(_pLoopDestB)) = eax;
	esi = esi + ( 8 );
	_pLoopDestB = _pLoopDestB + ( 4 );
	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto Draw2DBeamFadeASM_BeamFadeCase3;
//jmp	@@Done
	break;
Draw2DBeamFadeASM_NotBlack3:

	eax = eax & ( 0x0f8f8f8f8 );
	ecx = ecx & ( 0x0f8f8f8f8 );
	ebx = ebx & ( 0x007070707 );
	edx = edx & ( 0x007070707 );

	eax = ( eax + eax * 4 );
	ebx = ( ebx + ebx * 4 );
	ecx = ( ecx + ecx * 2 );
	edx = ( edx + edx * 2 );

	eax = eax + ecx;
	ebx = ebx + edx;

	ebx = ebx & ( 0x0f8f8f8f8 );
	ebx = ebx + eax;
	ebx = ebx >> ( 3 );
	*((uint32_t *)(_pLoopDestB)) = ebx;
	esi = esi + ( 8 );
	_pLoopDestB = _pLoopDestB + ( 4 );
	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto Draw2DBeamFadeASM_BeamFadeCase3;
//jmp	@@Done
	break;
	case 4:
Draw2DBeamFadeASM_BeamFadeCase4:
	eax = ( *((uint32_t *)(((uintptr_t)&(pBuffer[0])) + esi + 0)) );
	ecx = ( *((uint32_t *)(((uintptr_t)&(pBuffer[0])) + esi + 4)) );
	ebx = eax;
	edx = ecx;


	if ( (( (int32_t)ecx ) - ( 0 )) != 0) goto Draw2DBeamFadeASM_NotBlack4;
	*((uint32_t *)(_pLoopDestB)) = eax;
	esi = esi + ( 8 );
	_pLoopDestB = _pLoopDestB + ( 4 );
	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto Draw2DBeamFadeASM_BeamFadeCase4;
//jmp	@@Done
	break;
Draw2DBeamFadeASM_NotBlack4:

	eax = eax & ( 0x0f8f8f8f8 );
	ecx = ecx & ( 0x0f8f8f8f8 );
	ebx = ebx & ( 0x007070707 );
	edx = edx & ( 0x007070707 );

	eax = ( eax * 4 );
	ebx = ( ebx * 4 );
	ecx = ( ecx * 4 );
	edx = ( edx * 4 );

	eax = eax + ecx;
	ebx = ebx + edx;

	ebx = ebx & ( 0x0f8f8f8f8 );
	ebx = ebx + eax;
	ebx = ebx >> ( 3 );
	*((uint32_t *)(_pLoopDestB)) = ebx;

	esi = esi + ( 8 );
	_pLoopDestB = _pLoopDestB + ( 4 );
	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto Draw2DBeamFadeASM_BeamFadeCase4;
//jmp	@@Done
	break;
	case 5:
Draw2DBeamFadeASM_BeamFadeCase5:
	eax = ( *((uint32_t *)(((uintptr_t)&(pBuffer[0])) + esi + 0)) );
	ecx = ( *((uint32_t *)(((uintptr_t)&(pBuffer[0])) + esi + 4)) );
	ebx = eax;
	edx = ecx;


	if ( (( (int32_t)ecx ) - ( 0 )) != 0) goto Draw2DBeamFadeASM_NotBlack5;
	*((uint32_t *)(_pLoopDestB)) = eax;
	esi = esi + ( 8 );
	_pLoopDestB = _pLoopDestB + ( 4 );
	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto Draw2DBeamFadeASM_BeamFadeCase5;
//jmp	@@Done
	break;
Draw2DBeamFadeASM_NotBlack5:

	eax = eax & ( 0x0f8f8f8f8 );
	ecx = ecx & ( 0x0f8f8f8f8 );
	ebx = ebx & ( 0x007070707 );
	edx = edx & ( 0x007070707 );

	eax = ( eax + eax * 2 );
	ebx = ( ebx + ebx * 2 );
	ecx = ( ecx + ecx * 4 );
	edx = ( edx + edx * 4 );

	eax = eax + ecx;
	ebx = ebx + edx;

	ebx = ebx & ( 0x0f8f8f8f8 );
	ebx = ebx + eax;
	ebx = ebx >> ( 3 );
	*((uint32_t *)(_pLoopDestB)) = ebx;

	esi = esi + ( 8 );
	_pLoopDestB = _pLoopDestB + ( 4 );
	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto Draw2DBeamFadeASM_BeamFadeCase5;
//jmp	@@Done
	break;
	case 6:
Draw2DBeamFadeASM_BeamFadeCase6:
	eax = ( *((uint32_t *)(((uintptr_t)&(pBuffer[0])) + esi + 0)) );
	ecx = ( *((uint32_t *)(((uintptr_t)&(pBuffer[0])) + esi + 4)) );
	ebx = eax;
	edx = ecx;


	if ( (( (int32_t)ecx ) - ( 0 )) != 0) goto Draw2DBeamFadeASM_NotBlack6;
	*((uint32_t *)(_pLoopDestB)) = eax;
	esi = esi + ( 8 );
	_pLoopDestB = _pLoopDestB + ( 4 );
	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto Draw2DBeamFadeASM_BeamFadeCase6;
//jmp	@@Done
	break;
Draw2DBeamFadeASM_NotBlack6:

	eax = eax & ( 0x0f8f8f8f8 );
	ecx = ecx & ( 0x0f8f8f8f8 );
	ebx = ebx & ( 0x007070707 );
	edx = edx & ( 0x007070707 );

	ecx = ( ecx + ecx * 2 );
	edx = ( edx + edx * 2 );
	eax = ( eax * 2 );
	ebx = ( ebx * 2 );
	ecx = ( ecx * 2 );
	edx = ( edx * 2 );

	eax = eax + ecx;
	ebx = ebx + edx;

	ebx = ebx & ( 0x0f8f8f8f8 );
	ebx = ebx + eax;
	ebx = ebx >> ( 3 );
	*((uint32_t *)(_pLoopDestB)) = ebx;

	esi = esi + ( 8 );
	_pLoopDestB = _pLoopDestB + ( 4 );
	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto Draw2DBeamFadeASM_BeamFadeCase6;
//jmp	@@Done
	break;
	case 7:
Draw2DBeamFadeASM_BeamFadeCase7:
	eax = ( *((uint32_t *)(((uintptr_t)&(pBuffer[0])) + esi + 0)) );
	ecx = ( *((uint32_t *)(((uintptr_t)&(pBuffer[0])) + esi + 4)) );
	ebx = eax;
	edx = ecx;


	if ( (( (int32_t)ecx ) - ( 0 )) != 0) goto Draw2DBeamFadeASM_NotBlack7;
	*((uint32_t *)(_pLoopDestB)) = eax;
	esi = esi + ( 8 );
	_pLoopDestB = _pLoopDestB + ( 4 );
	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto Draw2DBeamFadeASM_BeamFadeCase7;
//jmp	@@Done
	break;
Draw2DBeamFadeASM_NotBlack7:

	eax = eax & ( 0x0f8f8f8f8 );
	ecx = ecx & ( 0x0f8f8f8f8 );
	ebx = ebx & ( 0x007070707 );
	edx = edx & ( 0x007070707 );

	eax = eax - ecx;
	ebx = ebx - edx;

	ecx = ( ecx * 8 );
	edx = ( edx * 8 );

	eax = eax + ecx;
	ebx = ebx + edx;

	ebx = ebx & ( 0x0f8f8f8f8 );
	ebx = ebx + eax;
	ebx = ebx >> ( 3 );
	*((uint32_t *)(_pLoopDestB)) = ebx;

	esi = esi + ( 8 );
	_pLoopDestB = _pLoopDestB + ( 4 );
	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto Draw2DBeamFadeASM_BeamFadeCase7;
//jmp	@@Done
	break;
	case 8:
Draw2DBeamFadeASM_BeamFadeCase8:
	ecx = ( *((uint32_t *)(((uintptr_t)&(pBuffer[0])) + esi + 4)) );


	if ( (( (int32_t)ecx ) - ( 0 )) != 0) goto Draw2DBeamFadeASM_NotBlack8;
	eax = ( *((uint32_t *)(((uintptr_t)&(pBuffer[0])) + esi + 0)) );
	*((uint32_t *)(_pLoopDestB)) = eax;
	esi = esi + ( 8 );
	_pLoopDestB = _pLoopDestB + ( 4 );
	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto Draw2DBeamFadeASM_BeamFadeCase8;
//jmp	@@Done
	break;
Draw2DBeamFadeASM_NotBlack8:
	*((uint32_t *)(_pLoopDestB)) = ecx;

	esi = esi + ( 8 );
	_pLoopDestB = _pLoopDestB + ( 4 );
	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto Draw2DBeamFadeASM_BeamFadeCase8;
//jmp	@@Done
	break;
//@@Done:
	}
	_dwY = _dwY + 1;

	_dwYCounter = ( (int32_t)_dwYCounter ) - 1;
	if (( (int32_t)_dwYCounter ) != 0) goto Draw2DBeamFadeASM_YLoop;



	return;
}







extern "C" void DrawFireASM(int8_t *_pDestBack, uint8_t *_pAmpTab, uint8_t *_pYAmpTab, uint32_t _dwXmax, uint32_t _dwYmax) {
	uint32_t eax, edx, ecx, ebx;
	int8_t *edi;
	uint8_t *esi, *ebp;
	uint32_t _dwY;




	esi = _pAmpTab;
	ebp = _pYAmpTab;

	_dwY = 0;

	ecx = _dwYmax;

	ecx = ecx << ( 8 ); // ch init.

	ebx = 0; // (ebx init.)
DrawFireASM_YLoop:
	eax = _dwY;
	ebx = (ebx & 0xffffff00) | (uint8_t)(( *((uint8_t *)(ebp + eax)) )); // ebx init.

	eax = eax << ( 10 );
	edi = _pDestBack;
	edi = edi + eax; // edi init.

	eax = _dwXmax;
	ecx = (ecx & 0xffffff00) | (uint8_t)(( (uint8_t)eax )); // cl init.

DrawFireASM_XLoop:
// eax =
// ebx =         	| YAmp
// ecx = 	YCounter| XCounter
// edx =
// esi = AmpTab
// edi = Dest
// ebp = YAmpTab
	edx = 0;
	eax = 0;
	edx = (edx & 0xffffff00) | (uint8_t)(( *((uint8_t *)(esi)) ));
	edx = edx + ebx;

	eax = ( pFireRGBLookUp[edx] );
	*((uint32_t *)(edi)) = eax;

	esi = esi + 1;
	edi = edi + ( 4 );

	ecx = (ecx & 0xffffff00) | (uint8_t)(( (int8_t)ecx ) - 1);
	if (( (int8_t)ecx ) != 0) goto DrawFireASM_XLoop;

	_dwY = _dwY + 1;

	ecx = set_high_byte(ecx, ( (int8_t)(ecx >> 8) ) - 1);
	if (( (int8_t)(ecx >> 8) ) != 0) goto DrawFireASM_YLoop;





	return;
}








