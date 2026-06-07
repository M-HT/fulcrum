#include <malloc.h>
#include "cc.h"
#include "m2struct.inc.h"

//;/*
//.486

//.model flat

//LOCALS

//data32 SEGMENT PAGE
//.data

//EXTRN	ppPolygons			:DWORD
extern "C" uint32_t *pcBackBuffer;
extern "C" uint32_t dwXmax;
extern "C" uint32_t iPixelCounter;
extern "C" uint8_t pAngleJoinTab[65536];
extern "C" uint32_t pLightBrightnessTab[256];
extern "C" uint8_t pBumpShadeLookup[65536];

//align 32

// todo: initialize to zero ?
static uint32_t pBackBumpBuffer[1600];
static uint32_t pBackTextureBuffer[1600];
static uint32_t pBackShadeBuffer[1600];

static uint32_t *pcTexture;
static uint16_t *pcBumpmap;

static CMV2Dot3D *pDot1;
static CMV2Dot3D *pDot2;
static CMV2Dot3D *pDot3;


static float fYCounter;
static float fYCounter1;
static float fYCounter2;


static uint32_t dwYCounter;
static uint32_t dwYCounter1;
static uint32_t dwYCounter2;

static uint32_t dwYOffset1;
static uint32_t dwYOffset2;

static uint32_t dwScreenX1;
static uint32_t dwScreenX2;
static uint32_t dwScreenX3;

static uint32_t dwDeltaScreenX;
static uint32_t dwDeltaScreenX1;
static uint32_t dwDeltaScreenX2;
//dwDeltaScreenXf				dd ?
//dwDeltaScreenX1f			dd ?
//dwDeltaScreenX2f			dd ?

static uint32_t dwAdderScreenX1;
static uint32_t dwAdderScreenX1f;
static uint32_t dwAdderScreenX2;
static uint32_t dwAdderScreenX2f;

static float fMaxXCounterRZP;
static uint32_t dwMaxXCounter;

static float fTextureUDivZ1NSBC;			// NSBC = Not Subpixelcorrect
static float fTextureUDivZ2NSBC;
static float fTextureUDivZ3NSBC;
static float fTextureVDivZ1NSBC;			// NSBC = Not Subpixelcorrect
static float fTextureVDivZ2NSBC;
static float fTextureVDivZ3NSBC;


static float fTextureUDivZ1;
static float fTextureUDivZ3;
static float fTextureUDivZdY;
static float fTextureUDivZdY1;
static float fTextureUDivZdY2;

static float fTextureVDivZ1;
static float fTextureVDivZ3;
static float fTextureVDivZdY;
static float fTextureVDivZdY1;
static float fTextureVDivZdY2;

static float fOneDivZ1NSBC;
static float fOneDivZ2NSBC;
static float fOneDivZ3NSBC;

static float fOneDivZdY;
static float fOneDivZdY1;
static float fOneDivZdY2;

static float fOneDivZ1;
//fOneDivZ2					dd ?
static float fOneDivZ3;

static float fOneDivZdX;
//fOneDivZdXsl				dd ?

static float fR1;
static float fR3;
static float fRdY;
static float fRdY1;
static float fRdY2;

static float fG1;
static float fG3;
static float fGdY;
static float fGdY1;
static float fGdY2;

static float fB1;
static float fB3;
static float fBdY;
static float fBdY1;
static float fBdY2;

static float fRdX;
static float fGdX;
static float fBdX;
static uint32_t dwRdX;
static uint32_t dwGdX;
static uint32_t dwBdX;

static float fScreenYError1;
static float fScreenYError2;
static float fScreenYError3;


static float fTextureVDivZdX;		// sl = SpanLength
static float fTextureUDivZdX;
//fTextureVDivZdXsl			dd ?		; sl = SpanLength
//fTextureUDivZdXsl			dd ?


static uint32_t dwTextureU;
static uint32_t dwTextureV;
static uint32_t dwTextureVdX;
static uint32_t dwTextureUdX;

const static float fNum_2EXP20 = 1048576.0f;
const static float fNum_2EXP_20 = 0.0000009536743164f;

const static float fSpanLengthFactorsRZP[17] = { -1,
							1.0f,
							0.5f,
							0.3333333333f,
							0.25f,
							0.2f,
							0.1666666666f,
							0.142857142f,
							0.125f,
							0.1111111111f,
							0.1f,
							0.0909090909f,
							0.0833333333f,
							0.076923076f,
							0.071428571f,
							0.0666666666f,
							0.0625f};

#define dwSpanLength				16


//***> TODO: include the SpanLengthFactor in MaxXCounter
//***> TODO: Check the include of 2EXP20 in MaxXCounter

static uint32_t dwCurXCounter;
static uint32_t dwNextXCounter;
static uint32_t dwScanlineXCounter;


//***> TODO: eliminate old loop variables
//dwRemnantPixel				dd ?
//dwSpanCounter				dd ?


//YLoopVariable


static void *dwESPStartPoint;
//dwOldESP					dd ?
static uint32_t dwScreenXf;


// Linear Mapper

static float fTextureU1;
static float fTextureU3;
static float fTextureUdY;
static float fTextureUdY1;
static float fTextureUdY2;

static float fTextureV1;
static float fTextureV3;
static float fTextureVdY;
static float fTextureVdY1;
static float fTextureVdY2;

static float fTextureVdX;
static float fTextureUdX;

static uint32_t dwRGBIntLoop_eax1;
static uint32_t dwRGBIntLoop_ecx1;
static uint32_t dwRGBIntLoop_esi1;
static uint32_t dwRGBIntLoop_ebp;



//fpucw						dw ?
//oldfpucw					dw ?
//oh_low 						dd ?
//co_low						dd ?
//co_hi						dd ?
//ends

//.code

static void *allocated_pointer = NULL;
static uint32_t allocated_size = 0;

static void *mem_alloc_endptr(uint32_t size)
{
    if (allocated_pointer != NULL)
    {
        if (size <= allocated_size)
        {
            return (void *) (allocated_size + (uintptr_t) allocated_pointer);
        }

        free(allocated_pointer);
        allocated_pointer = NULL;
    }

    allocated_size = (size & 0xffff0000) + 0x10000;
    allocated_pointer = malloc(allocated_size);
    return (void *) (allocated_size + (uintptr_t) allocated_pointer);
}

extern "C" void MV2DrawPolygonTBASM(CMV2Polygon *_edi1) {
	float fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13, fpu_reg14, fpu_reg15, fpu_reg16, fpu_reg17, fpu_reg18;
	uint32_t eax1, edx1, ecx1, edi1, ebx1, esi1, ebp;
	uint32_t stack_var00;
// edi = pPolygon
	CMV2Dot3D *eax2, *ebx2, *ecx2, *edx2, *esi2, *edi2;
	CMV2Dot3DPos *eax3, *ebx3, *ecx3, *edx3, *esi3, *edi3;
	CMV2ScanlinerPerspTG *SPTG, *edx4;
	CMV2ScanlinerLinTG *SLTG, *edx5;

	eax2 = _edi1->CMV2Polygon__m_pDot1;
	ebx2 = _edi1->CMV2Polygon__m_pDot2;
	ecx2 = _edi1->CMV2Polygon__m_pDot3;
//eax - ecx: pDot3D1 - pDot3D3

	pDot1 = eax2;
	pDot2 = ebx2;
	pDot3 = ecx2;

	eax3 = eax2->CMV2Dot3D__m_pPos;
	ebx3 = ebx2->CMV2Dot3D__m_pPos;
	ecx3 = ecx2->CMV2Dot3D__m_pPos;
//eax - ecx: pDot3DPos1 - pDot3DPos3
	pcTexture = (uint32_t *)_edi1->CMV2Polygon__m_pcTexture;
	pcBumpmap = (uint16_t *)_edi1->CMV2Polygon__m_pcBumpmap;

	edx1 = eax3->CMV2Dot3DPos__m_iScreenY;
	esi1 = ebx3->CMV2Dot3DPos__m_iScreenY;
	edi1 = ecx3->CMV2Dot3DPos__m_iScreenY;
	eax2 = pDot1;
	ebx2 = pDot2;
	ecx2 = pDot3;

// find dot with smallest y

	if (edx1 <= esi1) goto MV2DrawPolygonTBASM_Dot1YSmallerDot2Y; // dot1.y <= dot2.y


	if (esi1 <= edi1) goto MV2DrawPolygonTBASM_Dot2YSmallest; // dot2.y <(=) dot3.y&dot1.y
	goto MV2DrawPolygonTBASM_Dot3YSmallest; // dot3.y <    dot2.y&dot1.y
MV2DrawPolygonTBASM_Dot1YSmallerDot2Y:

	if (edx1 >= edi1) goto MV2DrawPolygonTBASM_Dot3YSmallest; // dot3.y <    dot1.y&dot2.y

//MV2DrawPolygonTBASM_Dot1YSmallest:
	edx2 = eax2;
	esi2 = ebx2;
	edi2 = ecx2;
	goto MV2DrawPolygonTBASM_DotsSorted;
MV2DrawPolygonTBASM_Dot2YSmallest:
	edx2 = ebx2;
	esi2 = ecx2;
	edi2 = eax2;
	goto MV2DrawPolygonTBASM_DotsSorted;
MV2DrawPolygonTBASM_Dot3YSmallest:
	edx2 = ecx2;
	esi2 = eax2;
	edi2 = ebx2;
	goto MV2DrawPolygonTBASM_DotsSorted;
MV2DrawPolygonTBASM_DotsSorted:

// edx = pDot1
// esi = pDot2
// edi = pDot3

	pDot1 = edx2;
	pDot2 = esi2;
	pDot3 = edi2;

	edx3 = edx2->CMV2Dot3D__m_pPos;
	esi3 = esi2->CMV2Dot3D__m_pPos;
	edi3 = edi2->CMV2Dot3D__m_pPos;

	eax1 = edx3->CMV2Dot3DPos__m_iScreenY;
	ebx1 = esi3->CMV2Dot3DPos__m_iScreenY;
	ecx1 = edi3->CMV2Dot3DPos__m_iScreenY;


	if (eax1 == ebx1) goto MV2DrawPolygonTBASM_PolygonType4;


	if (ebx1 == ecx1) goto MV2DrawPolygonTBASM_PolygonType3;
	if (ebx1 < ecx1) goto MV2DrawPolygonTBASM_PolygonType2;


//=============>                <==============
//=============> Polygon Type 1 <==============
//=============>                <==============
//MV2DrawPolygonTBASM_PolygonType1:
// edx = pDot1
// esi = pDot2
// edi = pDot3

	fpu_reg10 = edx3->CMV2Dot3DPos__m_fScreenY;
	fpu_reg11 = esi3->CMV2Dot3DPos__m_fScreenY;
	fpu_reg12 = edi3->CMV2Dot3DPos__m_fScreenY;
//st0 = (y3), st1 = (y2), st2 = (y1)

	fpu_reg13 = fpu_reg11;
	fpu_reg13 = fpu_reg13 - fpu_reg12;
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
//st0 = (y3), st1 = (y2 - y3), st2 = (y2), st3 = (y1)

	fpu_reg13 = fpu_reg13 - fpu_reg10;
	{ float tmp = fpu_reg11; fpu_reg11 = fpu_reg13; fpu_reg13 = tmp; }
//st0 = (y2), st1 = (y2 - y3), st2 = (y3 - y1), st3 = (y1)

	fpu_reg13 = fpu_reg13 - fpu_reg10;
	{ float tmp = fpu_reg11; fpu_reg11 = fpu_reg13; fpu_reg13 = tmp; }
//st0 = (y3 - y1), st1 = (y2 - y3), st2 = (y2 - y1), st3 = (y1)

	fYCounter1 = fpu_reg13;

	fpu_reg14 = 1.0f;
	fpu_reg11 = fpu_reg14 / fpu_reg11;

	eax1 = dwXmax;
	ebx1 = edx3->CMV2Dot3DPos__m_iScreenY;
	ebx1 = ebx1 - 1; // subpixel/clipping reason
	eax1 = ( (int32_t)eax1 ) * ( (int32_t)ebx1 );
	//eax1 = eax1 + pcBackBuffer;
	dwYOffset1 = eax1;

	fpu_reg14 = 1.0f;
	fpu_reg12 = fpu_reg14 / fpu_reg12;

	eax1 = dwXmax;
	ebx1 = edi3->CMV2Dot3DPos__m_iScreenY;
	ebx1 = ebx1 - 1;
	eax1 = ( (int32_t)eax1 ) * ( (int32_t)ebx1 );
	//eax1 = eax1 + pcBackBuffer;
	dwYOffset2 = eax1;

	fpu_reg14 = 1.0f;
	fpu_reg13 = fpu_reg14 / fpu_reg13;

	eax1 = edx3->CMV2Dot3DPos__m_iScreenY;
	ebx1 = esi3->CMV2Dot3DPos__m_iScreenY;
	ecx1 = edi3->CMV2Dot3DPos__m_iScreenY;

	ebp = ebx1;
	ebp = ebp - eax1;
	dwYCounter = ebp;
	ebp = ecx1;
	ebp = ebp - eax1;
	dwYCounter1 = ebp;
	ebp = ebx1;
	ebp = ebp - ecx1;
	dwYCounter2 = ebp;

	// fpu_reg10 = 0.0f;

//st0 = 1/(y3 - y1), st1 = 1/(y2 - y3), st2 = 1/(y2 - y1)

//===>                                                                <===
//===>	Screen Delta Calculation {                                   <===
//===>                                                                <===
//st0 = 1/YCounter1, st1 = 1/YCounter2, st2 = 1/YCounter
	fpu_reg14 = edx3->CMV2Dot3DPos__m_fScreenX;
	fpu_reg15 = esi3->CMV2Dot3DPos__m_fScreenX;
	fpu_reg16 = edi3->CMV2Dot3DPos__m_fScreenX;
//st0 = (x3), st1 = (x2), st2 = (x1)
//st4 = 1/YCounter1, st5 = 1/YCounter2, st6 = 1/YCounter

	fpu_reg17 = fpu_reg15;
	fpu_reg17 = fpu_reg17 - fpu_reg16;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 - fpu_reg14;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 * fpu_reg12;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 - fpu_reg14;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 * fpu_reg13;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg17; fpu_reg17 = tmp; }
//ffree   st
//fincstp

	fpu_reg16 = fpu_reg16 * fpu_reg11;
//st0 = DeltaScreenX, st1 = DeltaScreenX2,
//st2 = DeltaScreenX1, st3 = 1/YCounter1,
//st4 = 1/YCounter2, st5 = 1/YCounter

	fpu_reg17 = edx3->CMV2Dot3DPos__m_fScreenYError;
	fpu_reg18 = fpu_reg17;
	fpu_reg18 = fpu_reg18 * fpu_reg14;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg18 = fpu_reg18 * fpu_reg16;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg18 = fpu_reg18 + edx3->CMV2Dot3DPos__m_fScreenX;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg18 = fpu_reg18 + edx3->CMV2Dot3DPos__m_fScreenX;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
//st0 = ScreenX1
//st1 = ScreenX2
//st2 = DeltaScreenX, st3 = DeltaScreenX2
//st4 = DeltaScreenX1, st5 = 1/YCounter1
//st6 = 1/YCounter2, st7 = 1/YCounter

	dwScreenX1 = (int32_t)ceilf(fpu_reg18);

	fpu_reg18 = edi3->CMV2Dot3DPos__m_fScreenYError;
	fpu_reg18 = fpu_reg18 * fpu_reg15;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
//st0 = dwScreenX2
//st1 = ScreenYError3*DeltaScreenX2
//st2 = DeltaScreenX, st3 = DeltaScreenX2
//st4 = DeltaScreenX1, st5 = 1/YCounter13
//st6 = 1/YCounter32, st7 = 1/YCounter12

	dwScreenX2 = (int32_t)ceilf(fpu_reg18);

	fpu_reg17 = fpu_reg17 + edi3->CMV2Dot3DPos__m_fScreenX;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg17; fpu_reg17 = tmp; }
//st0 = DeltaScreenX1
//st1 = DeltaScreenX, st2 = DeltaScreenX2
//st3 = ScreenYError3*DeltaScreenX3 + ScreenX3, st4 = 1/YCounter13
//st5 = 1/YCounter32, st6 = 1/YCounter12

	dwDeltaScreenX1 = (int32_t)ceilf(fpu_reg17);
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
//st0 = DeltaScreenX3, st1 = DeltaScreenX2
//st2 = ScreenYError3*DeltaScreenX3 + ScreenX3, st3 = 1/YCounter13
//st4 = 1/YCounter32, st5 = 1/YCounter12

	dwDeltaScreenX2 = (int32_t)ceilf(fpu_reg16);
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
//st0 = ScreenYError3*DeltaScreenX3 + ScreenX3
//st1 = DeltaScreenX2, st2 = 1/YCounter13
//st3 = 1/YCounter32, st4 = 1/YCounter12

	dwScreenX3 = (int32_t)ceilf(fpu_reg15);

//===>	max_x_counter calculation	{                                <===

//st0 = DeltaScreenX
//st1 = 1/YCounter13, st2 = 1/YCounter32, st3 = 1/YCounter12

	fpu_reg15 = fYCounter1;
	fpu_reg15 = fpu_reg15 * fpu_reg14;
	fpu_reg15 = fpu_reg15 + edx3->CMV2Dot3DPos__m_fScreenX;
	fpu_reg15 = fpu_reg15 - edi3->CMV2Dot3DPos__m_fScreenX; //fp dep.
	dwMaxXCounter = (int32_t)ceilf(fpu_reg15);
	fpu_reg16 = 1.0f;
	fpu_reg15 = fpu_reg16 / fpu_reg15;
	fMaxXCounterRZP = fpu_reg15; //fp dep.
//===>	max_x_counter calculation	}                                <===

	dwDeltaScreenX = (int32_t)ceilf(fpu_reg14);

//===>                                                                <===
//===>	Screen Delta Calculation 	}                                <===
//===>                                                                <===
//st0 = 1/YCounter1, st1 = 1/YCounter2, st2 = 1/YCounter


//===>                                                                <===
//===>	R DeltaY Calculation {                            	 		 <===
//===>                                                                <===

	fpu_reg14 = edx3->CMV2Dot3DPos__m_fR;
	fpu_reg15 = esi3->CMV2Dot3DPos__m_fR;
	fpu_reg16 = edi3->CMV2Dot3DPos__m_fR;
	fpu_reg17 = fpu_reg15;
	fpu_reg17 = fpu_reg17 - fpu_reg16;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 - fpu_reg14;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 * fpu_reg12;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 - fpu_reg14;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 * fpu_reg13;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg17; fpu_reg17 = tmp; }
//ffree   st
//fincstp

	fpu_reg16 = fpu_reg16 * fpu_reg11;
//st0 = RdY, st1 = RdY2,
//st2 = RdY1, st3 = 1/YCounter1,
//st4 = 1/YCounter2, st5 = 1/YCounter

	fpu_reg17 = edx3->CMV2Dot3DPos__m_fScreenYError;
	fpu_reg18 = edi3->CMV2Dot3DPos__m_fScreenYError;
	fpu_reg18 = fpu_reg18 * fpu_reg15;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg18 = fpu_reg18 * fpu_reg14;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg18 = fpu_reg18 + edi3->CMV2Dot3DPos__m_fR;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg18 = fpu_reg18 + edx3->CMV2Dot3DPos__m_fR;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }

	fR3 = fpu_reg18;
	fR1 = fpu_reg17;
	fRdY = fpu_reg16;
	fRdY2 = fpu_reg15;
	fRdY1 = fpu_reg14;
//===>                                                                <===
//===>	R DeltaY Calculation }                            	 		 <===
//===>                                                                <===


//===>                                                                <===
//===>	G DeltaY Calculation {                            	 		 <===
//===>                                                                <===

	fpu_reg14 = edx3->CMV2Dot3DPos__m_fG;
	fpu_reg15 = esi3->CMV2Dot3DPos__m_fG;
	fpu_reg16 = edi3->CMV2Dot3DPos__m_fG;
	fpu_reg17 = fpu_reg15;
	fpu_reg17 = fpu_reg17 - fpu_reg16;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 - fpu_reg14;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 * fpu_reg12;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 - fpu_reg14;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 * fpu_reg13;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg17; fpu_reg17 = tmp; }
//ffree   st
//fincstp

	fpu_reg16 = fpu_reg16 * fpu_reg11;
	fpu_reg17 = edx3->CMV2Dot3DPos__m_fScreenYError;
	fpu_reg18 = edi3->CMV2Dot3DPos__m_fScreenYError;
	fpu_reg18 = fpu_reg18 * fpu_reg15;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg18 = fpu_reg18 * fpu_reg14;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg18 = fpu_reg18 + edi3->CMV2Dot3DPos__m_fG;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg18 = fpu_reg18 + edx3->CMV2Dot3DPos__m_fG;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }

	fG3 = fpu_reg18;
	fG1 = fpu_reg17;
	fGdY = fpu_reg16;
	fGdY2 = fpu_reg15;
	fGdY1 = fpu_reg14;
//===>                                                                <===
//===>	G DeltaY Calculation }                            	 		 <===
//===>                                                                <===

//===>                                                                <===
//===>	B DeltaY Calculation {                            	 		 <===
//===>                                                                <===
	fpu_reg14 = edx3->CMV2Dot3DPos__m_fB;
	fpu_reg15 = esi3->CMV2Dot3DPos__m_fB;
	fpu_reg16 = edi3->CMV2Dot3DPos__m_fB;
	fpu_reg17 = fpu_reg15;
	fpu_reg17 = fpu_reg17 - fpu_reg16;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 - fpu_reg14;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 * fpu_reg12;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 - fpu_reg14;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 * fpu_reg13;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg17; fpu_reg17 = tmp; }
//ffree   st
//fincstp

	fpu_reg16 = fpu_reg16 * fpu_reg11;
	fpu_reg17 = edx3->CMV2Dot3DPos__m_fScreenYError;
	fpu_reg18 = edi3->CMV2Dot3DPos__m_fScreenYError;
	fpu_reg18 = fpu_reg18 * fpu_reg15;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg18 = fpu_reg18 * fpu_reg14;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg18 = fpu_reg18 + edi3->CMV2Dot3DPos__m_fB;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg18 = fpu_reg18 + edx3->CMV2Dot3DPos__m_fB;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }

	fB3 = fpu_reg18;
	fB1 = fpu_reg17;
	fBdY = fpu_reg16;
	fBdY2 = fpu_reg15;
	fBdY1 = fpu_reg14;
//===>                                                                <===
//===>	B DeltaY Calculation }                            	 		 <===
//===>                                                                <===


//st0 = 1/YCounter1, st1 = 1/YCounter2, st2 = 1/YCounter
//===>                                                                <===
//===>	RGB DeltaX Calculation {                            	 	 <===
//===>                                                                <===
	fpu_reg14 = fMaxXCounterRZP;
	fpu_reg14 = fpu_reg14 * fNum_2EXP20;
	fpu_reg15 = fYCounter1;
	fpu_reg16 = fBdY;
	fpu_reg17 = fGdY;
	fpu_reg18 = fRdY;
//st0 = RdY, st1 = GdY, st2 = BdY, st3 = YCounter1
//st4 = 1/MaxXCounter, st5 = 1/YCounter1, st6 = 1/YCounter2
//st7 = 1/YCounter

	fpu_reg18 = fpu_reg18 * fpu_reg15;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg18 = fpu_reg18 * fpu_reg15;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
//st0 = RdY*YCounter1, st1 = GdY*YCounter1, st2 = BdY
//st3 = YCounter1, st4 = 1/MaxXCounter, st5 = 1/YCounter1
//st6 = 1/YCounter2, st7 = 1/YCounter

	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg18; fpu_reg18 = tmp; }
	fpu_reg15 = fpu_reg15 * fpu_reg18;
//st0 = GdY*YCounter1, st1 = RdY*YCounter1, st2 = BdY*YCounter1
//st3 = 1/MaxXCounter, st4 = 1/YCounter1, st5 = 1/YCounter2
//st6 = 1/YCounter

	fpu_reg17 = fpu_reg17 + edx3->CMV2Dot3DPos__m_fG;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 + edx3->CMV2Dot3DPos__m_fR;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 + edx3->CMV2Dot3DPos__m_fB;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }

	fpu_reg17 = fpu_reg17 - edi3->CMV2Dot3DPos__m_fG ;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 - edi3->CMV2Dot3DPos__m_fR;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 - edi3->CMV2Dot3DPos__m_fB;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }

	fpu_reg17 = fpu_reg17 * fpu_reg14;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 * fpu_reg14;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg14 = fpu_reg14 * fpu_reg17;

	fRdX = fpu_reg16;
	dwRdX = (int32_t)ceilf(fpu_reg16);
	fGdX = fpu_reg15;
	dwGdX = (int32_t)ceilf(fpu_reg15);
	fBdX = fpu_reg14;
	dwBdX = (int32_t)ceilf(fpu_reg14);

//===>                                                                <===
//===>	RGB DeltaX Calculation }                            	 	 <===
//===>                                                                <===

	ebx1 = dwYCounter;

	if (ebx1 >= ( dwSpanLength )) goto MV2DrawPolygonTBASM_PerspPT1;

	eax1 = edx3->CMV2Dot3DPos__m_iScreenX;
	ebx1 = esi3->CMV2Dot3DPos__m_iScreenX;
	ebp = eax1;
	ecx1 = edi3->CMV2Dot3DPos__m_iScreenX;

	eax1 = eax1 - ebx1; // p1 - p2
	ebx1 = ebx1 - ecx1; // p2 - p3
	ecx1 = ecx1 - ebp; // p3 - p1


	if ( (( (int32_t)eax1 ) - ( 0 )) >= 0) goto MV2DrawPolygonTBASM_PerspCheck1PT1;
	eax1 = - ( (int32_t)eax1 );
MV2DrawPolygonTBASM_PerspCheck1PT1:

	if ( (( (int32_t)ebx1 ) - ( 0 )) >= 0) goto MV2DrawPolygonTBASM_PerspCheck2PT1;
	ebx1 = - ( (int32_t)ebx1 );
MV2DrawPolygonTBASM_PerspCheck2PT1:

	if ( (( (int32_t)ecx1 ) - ( 0 )) >= 0) goto MV2DrawPolygonTBASM_PerspCheck3PT1;
	ecx1 = - ( (int32_t)ecx1 );
MV2DrawPolygonTBASM_PerspCheck3PT1:


	if (eax1 >= ( dwSpanLength )) goto MV2DrawPolygonTBASM_PerspPT1;


	if (ebx1 >= ( dwSpanLength )) goto MV2DrawPolygonTBASM_PerspPT1;


	if (ecx1 >= ( dwSpanLength )) goto MV2DrawPolygonTBASM_PerspPT1;

	goto MV2DrawPolygonTBASM_LinearPolygonType1;
MV2DrawPolygonTBASM_PerspPT1:


	fOneDivZ1NSBC = edx3->CMV2Dot3DPos__m_fZNewRZP;
	fOneDivZ2NSBC = esi3->CMV2Dot3DPos__m_fZNewRZP;
	fOneDivZ3NSBC = edi3->CMV2Dot3DPos__m_fZNewRZP;

	fScreenYError1 = edx3->CMV2Dot3DPos__m_fScreenYError;
	fScreenYError3 = edi3->CMV2Dot3DPos__m_fScreenYError;

	edx2 = pDot1;
	esi2 = pDot2;
	edi2 = pDot3;

//===>                                                                <===
//===>	TextureUDivZ DeltaY Calculation {                            <===
//===>                                                                <===
//st0=1/YCounter1, st1=1/YCounter2, st2=1/YCounter

	fpu_reg14 = edx2->CMV2Dot3D__m_fTextureU;
	fpu_reg14 = fpu_reg14 * fOneDivZ1NSBC;
	fpu_reg15 = esi2->CMV2Dot3D__m_fTextureU;
	fpu_reg15 = fpu_reg15 * fOneDivZ2NSBC;
	fpu_reg16 = edi2->CMV2Dot3D__m_fTextureU;
	fpu_reg16 = fpu_reg16 * fOneDivZ3NSBC;

	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg16; fpu_reg16 = tmp; }
	fTextureUDivZ1NSBC = fpu_reg16;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg16; fpu_reg16 = tmp; }
	fTextureUDivZ3NSBC = fpu_reg16;

//st0 = (tu3/z3), st1 = (tu2/z2), st2 = (tu1/z1)
//st4 = 1/YCounter1, st5 = 1/YCounter2, st6 = 1/YCounter

	fpu_reg17 = fpu_reg15;
	fpu_reg17 = fpu_reg17 - fpu_reg16;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
//st0 = (tu3/z3), st1 = (tu2/z2 - tu3/z3), st2 = (tu2/z2)
//st3 = (tu1/z1)
//st4 = 1/YCounter1, st5 = 1/YCounter2, st6 = 1/YCounter

	fpu_reg17 = fpu_reg17 - fpu_reg14;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
//st0 = (tu2/z2 - tu3/z3), st1 = (tu3/z3 - tu1/z1)
//st2 = (tu2/z2), st3 = (tu1/z1)
//st4 = 1/YCounter1, st5 = 1/YCounter2, st6 = 1/YCounter

	fpu_reg17 = fpu_reg17 * fpu_reg12;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
//st0 = (tu2/z2), st1 = (tu3/z3 - tu1/z1)
//st2 = (tu2/z2 - tu3/z3)/YCounter2, st3 = (tu1/z1)
//st4 = 1/YCounter1, st5 = 1/YCounter2, st6 = 1/YCounter

	fpu_reg17 = fpu_reg17 - fpu_reg14;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
//st0 = (tu3/z3 - tu1/z1), st1 = (tu2/z2 - tu1/z1)
//st2 = (tu2/z2 - tu3/z3)/YCounter2,
//st3 = (tu1/z1)
//st4 = 1/YCounter1, st5 = 1/YCounter2, st6 = 1/YCounter

	fpu_reg17 = fpu_reg17 * fpu_reg13;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg17; fpu_reg17 = tmp; }
//st0 = (tu1/z1), st1 = (tu2/z2 - tu1/z1),
//st2 = (tu3/z3 - tu2/z2)/YCounter2
//st3 = (tu3/z3 - tu1/z1)/YCounter1
//st4 = 1/YCounter1, st5 = 1/YCounter2, st6 = 1/YCounter

//ffree   st
//fincstp

//st0 = (tu2/z2 - tu1/z1), st1 = (tu3/z3 - tu2/z2)/YCounter2
//st2 = (tu3/z3 - tu1/z1)/YCounter1
//st3 = 1/YCounter1, st4 = 1/YCounter2, st5 = 1/YCounter

	fpu_reg16 = fpu_reg16 * fpu_reg11;
//st0 = TextureUDivZdY, st1 = TextureUDivZdY2
//st2 = TextureUDivZdY1, st3 = 1/YCounter1
//st4 = 1/YCounter2, st5 = 1/YCounter

	fpu_reg17 = fScreenYError1;
	fpu_reg18 = fScreenYError3;
//st0 = ScreenYError3, st1 = ScreenYError1
//st2 = TextureUDivZdY, st3 = TextureUDivZdY2
//st4 = TextureUDivZdY1, st5 = 1/YCounter1
//st6 = 1/YCounter2, st7 = 1/YCounter

	fpu_reg18 = fpu_reg18 * fpu_reg15;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
//st0 = ScreenYError1, st1 = ScreenYError3*TextureUDivZdY2
//st2 = TextureUDivZdY, st3 = TextureUDivZdY2
//st4 = TextureUDivZdY1, st5 = 1/YCounter1
//st6 = 1/YCounter2, st7 = 1/YCounter

	fpu_reg18 = fpu_reg18 * fpu_reg14;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
//st0 = ScreenYError3*TextureUDivZdY2
//st1 = ScreenYError1*TextureUDivZdY1
//st2 = TextureUDivZdY, st3 = TextureUDivZdY2
//st4 = TextureUDivZdY1, st5 = 1/YCounter1
//st6 = 1/YCounter2, st7 = 1/YCounter

	fpu_reg18 = fpu_reg18 + fTextureUDivZ3NSBC;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
//st0 = ScreenYError1*TextureUDivZdY1
//st1 = ScreenYError3*TextureUDivZdY2 + TextureU3
//st2 = TextureUDivZdY, st3 = TextureUDivZdY2
//st4 = TextureUDivZdY1, st5 = 1/YCounter1
//st6 = 1/YCounter2, st7 = 1/YCounter

	fpu_reg18 = fpu_reg18 + fTextureUDivZ1NSBC;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
//st0 = ScreenYError3*TextureUDivZdY2 + TextureU3
//st1 = ScreenYError1*TextureUDivZdY1 + TextureU1
//st2 = TextureUDivZdY, st3 = TextureUDivZdY2
//st4 = TextureUDivZdY1, st5 = 1/YCounter1
//st6 = 1/YCounter2, st7 = 1/YCounter

	fTextureUDivZ3 = fpu_reg18;
	fTextureUDivZ1 = fpu_reg17;
	fTextureUDivZdY = fpu_reg16;
	fTextureUDivZdY2 = fpu_reg15;
	fTextureUDivZdY1 = fpu_reg14;
//===>                                                                <===
//===>	TextureUDivZ DeltaY Calculation }                            <===
//===>                                                                <===


//===>                                                                <===
//===>	TextureVDivZ DeltaY Calculation {                            <===
//===>                                                                <===
//st0=1/YCounter1, st1=1/YCounter2, st2=1/YCounter

	fpu_reg14 = edx2->CMV2Dot3D__m_fTextureV;
	fpu_reg14 = fpu_reg14 * fOneDivZ1NSBC;
	fpu_reg15 = esi2->CMV2Dot3D__m_fTextureV;
	fpu_reg15 = fpu_reg15 * fOneDivZ2NSBC;
	fpu_reg16 = edi2->CMV2Dot3D__m_fTextureV;
	fpu_reg16 = fpu_reg16 * fOneDivZ3NSBC;

	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg16; fpu_reg16 = tmp; }
	fTextureVDivZ1NSBC = fpu_reg16;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg16; fpu_reg16 = tmp; }
	fTextureVDivZ3NSBC = fpu_reg16;

//st0 = (tv3/z3), st1 = (tv2/z2), st2 = (tv1/z1)
//st4 = 1/YCounter1, st5 = 1/YCounter2, st6 = 1/YCounter

	fpu_reg17 = fpu_reg15;
	fpu_reg17 = fpu_reg17 - fpu_reg16;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
//st0 = (tv3/z3), st1 = (tv2/z2 - tv3/z3), st2 = (tv2/z2)
//st3 = (tv1/z1)
//st4 = 1/YCounter1, st5 = 1/YCounter2, st6 = 1/YCounter

	fpu_reg17 = fpu_reg17 - fpu_reg14;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
//st0 = (tv2/z2 - tv3/z3), st1 = (tv3/z3 - tv1/z1)
//st2 = (tv2/z2), st3 = (tv1/z1)
//st4 = 1/YCounter1, st5 = 1/YCounter2, st6 = 1/YCounter

	fpu_reg17 = fpu_reg17 * fpu_reg12;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
//st0 = (tv2/z2), st1 = (tv3/z3 - tv1/z1)
//st2 = (tv2/z2 - tv3/z3)/YCounter2, st3 = (tv1/z1)
//st4 = 1/YCounter1, st5 = 1/YCounter2, st6 = 1/YCounter

	fpu_reg17 = fpu_reg17 - fpu_reg14;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
//st0 = (tv3/z3 - tv1/z1), st1 = (tv2/z2 - tv1/z1)
//st2 = (tv2/z2 - tv3/z3)/YCounter2,
//st3 = (tv1/z1)
//st4 = 1/YCounter1, st5 = 1/YCounter2, st6 = 1/YCounter

	fpu_reg17 = fpu_reg17 * fpu_reg13;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg17; fpu_reg17 = tmp; }
//st0 = (tv1/z1), st1 = (tv2/z2 - tv1/z1),
//st2 = (tv3/z3 - tv2/z2)/YCounter2
//st3 = (tv3/z3 - tv1/z1)/YCounter1
//st4 = 1/YCounter1, st5 = 1/YCounter2, st6 = 1/YCounter

//ffree   st
//fincstp

//st0 = (tv2/z2 - tv1/z1), st1 = (tv3/z3 - tv2/z2)/YCounter2
//st2 = (tv3/z3 - tv1/z1)/YCounter1
//st3 = 1/YCounter1, st4 = 1/YCounter2, st5 = 1/YCounter

	fpu_reg16 = fpu_reg16 * fpu_reg11;
//st0 = TextureVDivZdY, st1 = TextureVDivZdY2
//st2 = TextureVDivZdY1, st3 = 1/YCounter1
//st4 = 1/YCounter2, st5 = 1/YCounter

	fpu_reg17 = fScreenYError1;
	fpu_reg18 = fScreenYError3;
//st0 = ScreenYError3, st1 = ScreenYError1
//st2 = TextureVDivZdY, st3 = TextureVDivZdY2
//st4 = TextureVDivZdY1, st5 = 1/YCounter1
//st6 = 1/YCounter2, st7 = 1/YCounter

	fpu_reg18 = fpu_reg18 * fpu_reg15;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
//st0 = ScreenYError1, st1 = ScreenYError3*TextureVDivZdY2
//st2 = TextureVDivZdY, st3 = TextureVDivZdY2
//st4 = TextureVDivZdY1, st5 = 1/YCounter1
//st6 = 1/YCounter2, st7 = 1/YCounter

	fpu_reg18 = fpu_reg18 * fpu_reg14;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
//st0 = ScreenYError3*TextureVDivZdY2
//st1 = ScreenYError1*TextureVDivZdY1
//st2 = TextureVDivZdY, st3 = TextureVDivZdY2
//st4 = TextureVDivZdY1, st5 = 1/YCounter1
//st6 = 1/YCounter2, st7 = 1/YCounter

	fpu_reg18 = fpu_reg18 + fTextureVDivZ3NSBC;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
//st0 = ScreenYError1*TextureVDivZdY1
//st1 = ScreenYError3*TextureVDivZdY2 + TextureV3
//st2 = TextureVDivZdY, st3 = TextureVDivZdY2
//st4 = TextureVDivZdY1, st5 = 1/YCounter1
//st6 = 1/YCounter2, st7 = 1/YCounter


	fpu_reg18 = fpu_reg18 + fTextureVDivZ1NSBC;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
//st0 = ScreenYError3*TextureVDivZdY2 + TextureV3
//st1 = ScreenYError1*TextureVDivZdY1 + TextureV1
//st2 = TextureVDivZdY, st3 = TextureVDivZdY2
//st4 = TextureVDivZdY1, st5 = 1/YCounter1
//st6 = 1/YCounter2, st7 = 1/YCounter

	fTextureVDivZ3 = fpu_reg18;
	fTextureVDivZ1 = fpu_reg17;
	fTextureVDivZdY = fpu_reg16;
	fTextureVDivZdY2 = fpu_reg15;
	fTextureVDivZdY1 = fpu_reg14;

//===>                                                                <===
//===>	TextureVDivZ DeltaY Calculation }                            <===
//===>                                                                <===



//===>                                                                <===
//===>	OneDivZ DeltaY Calculation {                                 <===
//===>                                                                <===
//st0=1/YCounter1, st1=1/YCounter2, st2=1/YCounter

	fpu_reg14 = fOneDivZ1NSBC;
	fpu_reg15 = fOneDivZ2NSBC;
	fpu_reg16 = fOneDivZ3NSBC;
//st0 = (1/z3), st1 = (1/z2), st2 = (1/z1)
//st4 = 1/YCounter1, st5 = 1/YCounter2, st6 = 1/YCounter

	fpu_reg17 = fpu_reg15;
	fpu_reg17 = fpu_reg17 - fpu_reg16;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
//st0 = (1/z3), st1 = (1/z2 - 1/z3), st2 = (1/z2)
//st3 = (1/z1)
//st4 = 1/YCounter1, st5 = 1/YCounter2, st6 = 1/YCounter

	fpu_reg17 = fpu_reg17 - fpu_reg14;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
//st0 = (1/z2 - 1/z3), st1 = (1/z3 - 1/z1)
//st2 = (1/z2), st3 = (1/z1)
//st4 = 1/YCounter1, st5 = 1/YCounter2, st6 = 1/YCounter

	fpu_reg17 = fpu_reg17 * fpu_reg12;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
//st0 = (1/z2), st1 = (1/z3 - 1/z1)
//st2 = (1/z2 - 1/z3)/YCounter2, st3 = (1/z1)
//st4 = 1/YCounter1, st5 = 1/YCounter2, st6 = 1/YCounter

	fpu_reg17 = fpu_reg17 - fpu_reg14;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
//st0 = (1/z3 - 1/z1), st1 = (1/z2 - 1/z1)
//st2 = (1/z2 - 1/z3)/YCounter2,
//st3 = (1/z1)
//st4 = 1/YCounter1, st5 = 1/YCounter2, st6 = 1/YCounter

	fpu_reg17 = fpu_reg17 * fpu_reg13;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg17; fpu_reg17 = tmp; }
//st0 = (1/z1), st1 = (1/z2 - 1/z1),
//st2 = (1/z3 - 1/z2)/YCounter2
//st3 = (1/z3 - 1/z1)/YCounter1
//st4 = 1/YCounter1, st5 = 1/YCounter2, st6 = 1/YCounter

//ffree   st
//fincstp

//st0 = (1/z2 - 1/z1), st1 = (1/z3 - 1/z2)/YCounter2
//st2 = (1/z3 - 1/z1)/YCounter1
//st3 = 1/YCounter1, st4 = 1/YCounter2, st5 = 1/YCounter

	fpu_reg16 = fpu_reg16 * fpu_reg11;
//st0 = OneDivZdY, st1 = OneDivZdY2
//st2 = OneDivZdY1, st3 = 1/YCounter1
//st4 = 1/YCounter2, st5 = 1/YCounter

	fpu_reg17 = fScreenYError1;
	fpu_reg18 = fScreenYError3;
//st0 = ScreenYError3, st1 = ScreenYError1
//st2 = OneDivZdY, st3 = OneDivZdY2
//st4 = OneDivZdY1, st5 = 1/YCounter1
//st6 = 1/YCounter2, st7 = 1/YCounter

	fpu_reg18 = fpu_reg18 * fpu_reg15;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
//st0 = ScreenYError1, st1 = ScreenYError3*OneDivZdY2
//st2 = OneDivZdY, st3 = OneDivZdY2
//st4 = OneDivZdY1, st5 = 1/YCounter1
//st6 = 1/YCounter2, st7 = 1/YCounter

	fpu_reg18 = fpu_reg18 * fpu_reg14;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
//st0 = ScreenYError3*OneDivZdY2
//st1 = ScreenYError1*OneDivZdY1
//st2 = OneDivZdY, st3 = OneDivZdY2
//st4 = OneDivZdY1, st5 = 1/YCounter1
//st6 = 1/YCounter2, st7 = 1/YCounter

	fpu_reg18 = fpu_reg18 + fOneDivZ3NSBC;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
//st0 = ScreenYError1*OneDivZdY1
//st1 = ScreenYError3*OneDivZdY2 + OneDivZ3
//st2 = OneDivZdY, st3 = OneDivZdY2
//st4 = OneDivZdY1, st5 = 1/YCounter1
//st6 = 1/YCounter2, st7 = 1/YCounter

	fpu_reg18 = fpu_reg18 + fOneDivZ1NSBC;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
//st0 = ScreenYError3*OneDivZdY2 + OneDivZ3
//st1 = ScreenYError1*OneDivZdY1 + OneDivZ1
//st2 = OneDivZdY, st3 = OneDivZdY2
//st4 = OneDivZdY1, st5 = 1/YCounter1
//st6 = 1/YCounter2, st7 = 1/YCounter

	fOneDivZ3 = fpu_reg18;
	fOneDivZ1 = fpu_reg17;
	fOneDivZdY = fpu_reg16;
	fOneDivZdY2 = fpu_reg15;
	fOneDivZdY1 = fpu_reg14;

//===>                                                                <===
//===>	OneDivZ DeltaY Calculation }                                 <===
//===>                                                                <===



//===>                                                                <===
//===>	TextureU&VDivZ DeltaX Calculation {                          <===
//===>                                                                <===

	fpu_reg14 = fMaxXCounterRZP;
	fpu_reg14 = fpu_reg14 * fNum_2EXP20;
	fpu_reg15 = fYCounter1;
	fpu_reg16 = fTextureVDivZdY;
	fpu_reg17 = fTextureUDivZdY;
//st0 = TextureUDivZdY, st1 = TextureVDivZdY
//st2 = YCounter1, st3 = 1/MaxXCounter
//st4 = 1/YCounter1, st5 = 1/YCounter2, st6 = 1/YCounter

	fpu_reg17 = fpu_reg17 * fpu_reg15;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
//st0 = TextureVDivZDY, st1 = YCounter1*TextureUDivZDY
//st2 = YCounter1, st3 = 1/MaxXCounter
//st4 = 1/YCounter1, st5 = 1/YCounter2, st6 = 1/YCounter

	fpu_reg15 = fpu_reg15 * fpu_reg17; // fmul stall (+ 1 Cycle)
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
//st0 = YCounter1*TextureVDivZDY, st1 = YCounter1*TextureUDivZDY
//st2 = 1/MaxXCounter
//st3 = 1/YCounter1, st4 = 1/YCounter2, st5 = 1/YCounter

	fpu_reg16 = fpu_reg16 + fTextureVDivZ1NSBC;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
//st0 = YCounter1*TextureUDivZDY
//st1 = YCounter1*TextureVDivZDY + TextureVDivZ1
//st2 = 1/MaxXCounter
//st3 = 1/YCounter1, st4 = 1/YCounter2, st5 = 1/YCounter

	fpu_reg16 = fpu_reg16 + fTextureUDivZ1NSBC;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
//st0 = YCounter1*TextureVDivZDY + TextureVDivZ1
//st1 = YCounter1*TextureUDivZDY + TextureUDivZ1
//st2 = 1/MaxXCounter
//st3 = 1/YCounter1, st4 = 1/YCounter2, st5 = 1/YCounter

	fpu_reg16 = fpu_reg16 - fTextureVDivZ3NSBC;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
//st0 = YCounter1*TextureUDivZDY + TextureUDivZ1
//st1 = YCounter1*TextureVDivZDY + TextureVDivZ1 - TextureVDivZ3
//st2 = 1/MaxXCounter
//st3 = 1/YCounter1, st4 = 1/YCounter2, st5 = 1/YCounter

	fpu_reg16 = fpu_reg16 - fTextureUDivZ3NSBC;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
//st0 = YCounter1*TextureVDivZDY + TextureVDivZ1 - TextureVDivZ3
//st1 = YCounter1*TextureUDivZDY + TextureUDivZ1 - TextureUDivZ3
//st2 = 1/MaxXCounter
//st3 = 1/YCounter1, st4 = 1/YCounter2, st5 = 1/YCounter

	fpu_reg16 = fpu_reg16 * fpu_reg14;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
//st0 = YCounter1*TextureUDivZDY + TextureUDivZ1 - TextureUDivZ3
//st1 = TextureVDivZdX
//st2 = 1/MaxXCounter
//st3 = 1/YCounter1, st4 = 1/YCounter2, st5 = 1/YCounter

	fpu_reg16 = fpu_reg16 * fpu_reg14;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
//st0 = TextureVDivZdX
//st1 = TextureUDivZdX
//st2 = 1/MaxXCounter
//st3 = 1/YCounter1, st4 = 1/YCounter2, st5 = 1/YCounter
	fTextureVDivZdX = fpu_reg16;
	fTextureUDivZdX = fpu_reg15;
//===>                                                                <===
//===>	TextureU&VDivZ DeltaX Calculation }                          <===
//===>                                                                <===



//===>                                                                <===
//===>	OneDivZ DeltaX Calculation {                          		 <===
//===>                                                                <===
//st0 = 1/MaxXCounter
//st1 = 1/YCounter1, st2 = 1/YCounter2, st3 = 1/YCounter

	fpu_reg15 = fOneDivZ1NSBC;
	fpu_reg15 = fpu_reg15 - fOneDivZ3NSBC;
//st0 = OneDivZ1 - OneDivZ3
//st1 = 1/MaxXCounter
//st2 = 1/YCounter1, st3 = 1/YCounter2, st4 = 1/YCounter

	fpu_reg16 = fYCounter1;
	fpu_reg16 = fpu_reg16 * fOneDivZdY;
//st0 = YCounter1*OneDivZdY, st1 = OneDivZ1 - OneDivZ3
//st2 = 1/MaxXCounter
//st3 = 1/YCounter1, st4 = 1/YCounter2, st5 = 1/YCounter

	fpu_reg15 = fpu_reg15 + fpu_reg16;
//st0 = YCounter1*OneDivZdY + OneDivZ1 - OneDivZ3
//st1 = 1/MaxXCounter
//st2 = 1/YCounter1, st3 = 1/YCounter2, st4 = 1/YCounter

	fpu_reg14 = fpu_reg14 * fpu_reg15;
//st1 = OneDivZdX
//st2 = 1/YCounter1, st3 = 1/YCounter2, st4 = 1/YCounter

	fOneDivZdX = fpu_reg14;
//===>                                                                <===
//===>	OneDivZ DeltaX Calculation }                          		 <===
//===>                                                                <===

//ffree	st(2)
//ffree	st(1)
//ffree	st(0)





	eax1 = dwDeltaScreenX1;
	ebx1 = dwDeltaScreenX;
	esi1 = eax1;
	edi1 = ebx1;
	eax1 = eax1 << ( 12 );
	ebx1 = ebx1 << ( 12 );
	esi1 = ( (int32_t)esi1 ) >> ( 20 );
	edi1 = ( (int32_t)edi1 ) >> ( 20 );
	esi1 = esi1 + dwXmax;
	edi1 = edi1 + dwXmax;
	dwAdderScreenX1 = esi1;
	dwAdderScreenX2 = edi1;
	dwAdderScreenX1f = eax1;
	dwAdderScreenX2f = ebx1;

	eax1 = dwScreenX1;
	ebx1 = dwScreenX2;
	esi1 = eax1;
	edi1 = ebx1;
	eax1 = eax1 << ( 12 );
	ebx1 = ebx1 << ( 12 );
	esi1 = ( (int32_t)esi1 ) >> ( 20 );
	edi1 = ( (int32_t)edi1 ) >> ( 20 );
	esi1 = esi1 + dwYOffset1;
	edi1 = edi1 + dwYOffset1;

	ebp = dwYCounter1;
	ecx1 = edi1;

//mov		dwOldESP,esp
//and		esp,0ffffffe0h
	SPTG = (CMV2ScanlinerPerspTG *) mem_alloc_endptr(dwYCounter * sizeof(CMV2ScanlinerPerspTG));
//mov		dwESPStartPoint,esp
	dwESPStartPoint = (void *)SPTG;

	fpu_reg10 = fNum_2EXP_20;
	fpu_reg11 = fNum_2EXP20;
	fpu_reg12 = fTextureVDivZ1;
	fpu_reg13 = fTextureUDivZ1;
	fpu_reg14 = fOneDivZ1;

MV2DrawPolygonTBASM_PerspPT1ScanlinerTGLoopPass1:
// eax = x1f (20 bit fraction)
// ebx = x2f (20 bit fraction)
// ecx = XCounter (init. BackBuffer + YOffset + X2)
// edx =
// esi = BackBuffer + YOffset + X1
// edi = BackBuffer + YOffset + X2
// ebp = YCounter

// st0 = OneDivZ
// st1 = TextureUDivZ
// st2 = TextureVDivZ
// st3 = 2^20
// st4 = 2^-20

//***> TODO: Pipeline the fpu better
	eax1 = eax1 >> ( 12 );
//sub		esp,CMV2ScanlinerPerspTG__size
	SPTG--;

	dwScreenXf = eax1;
	ecx1 = ecx1 - esi1; // XCounter
	eax1 = eax1 << ( 12 );

	fpu_reg15 = ( (int32_t)dwScreenXf );
	fpu_reg15 = fpu_reg11 - fpu_reg15;
// st0 = 2^20 - ScreenXf, st1 = OneDivZ
// st2 = TextureUDivZ, st3 = TextureVDivZ,
// st4 = 2^20, st5 = 2^-20

	fpu_reg15 = fpu_reg15 * fpu_reg10;
//fst		dword ptr ss:[esp].CMV2ScanlinerPerspTG__fScreenXError
	SPTG->CMV2ScanlinerPerspTG__fScreenXError = fpu_reg15;
// st0 = ScreenXError

	fpu_reg16 = fTextureUDivZdX;
	fpu_reg16 = fpu_reg16 * fpu_reg15;
	fpu_reg17 = fTextureVDivZdX;
	fpu_reg17 = fpu_reg17 * fpu_reg15;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 * fOneDivZdX;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
// st0 = TextureVDivZdX*ScreenXError
// st1 = TextureUDivZdX*ScreenXError
// st2 = OneDivZdX*ScreenXError, st3 = OneDivZ
// st4 = TextureUDivZ, st5 = TextureVDivZ,
// st6 = 2^20, st7 = 2^-20

	fpu_reg17 = fpu_reg17 + fpu_reg12;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 + fpu_reg13;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 + fpu_reg14;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
// st0 = TextureUDivZ*ScreenXError + TextureUDivZ
// st1 = TextureVDivZ*ScreenXError + TextureVDivZ
// st2 = OneDivZ*ScreenXError + OneDivZ
// st3 = OneDivZ
// st4 = TextureUDivZ, st5 = TextureVDivZ,
// st6 = 2^20, st7 = 2^-20

//fstp	DWORD PTR ss:[esp].CMV2ScanlinerPerspTG__fTextureUDivZ
//fstp	DWORD PTR ss:[esp].CMV2ScanlinerPerspTG__fTextureVDivZ
//fstp	DWORD PTR ss:[esp].CMV2ScanlinerPerspTG__fOneDivZ
	SPTG->CMV2ScanlinerPerspTG__fTextureUDivZ = fpu_reg17;
	SPTG->CMV2ScanlinerPerspTG__fTextureVDivZ = fpu_reg16;
	SPTG->CMV2ScanlinerPerspTG__fOneDivZ = fpu_reg15;
// st0 = OneDivZ, st1 = TextureUDivZ, st2 = TextureVDivZ
// st3 = 2^20, st4 = 2^-20

//mov		ss:[esp].CMV2ScanlinerPerspTG__dwScreenX1Offset,esi
//mov		ss:[esp].CMV2ScanlinerPerspTG__dwXCounter,ecx
	SPTG->CMV2ScanlinerPerspTG__dwScreenX1Offset = esi1;
	SPTG->CMV2ScanlinerPerspTG__dwXCounter = ecx1;

	fpu_reg14 = fpu_reg14 + fOneDivZdY1;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg14 = fpu_reg14 + fTextureUDivZdY1;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg14 = fpu_reg14 + fTextureVDivZdY1;
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }

	{ uint32_t carry = (UINT32_MAX - ebx1 < dwAdderScreenX2f)?1:0; ebx1 = ebx1 + dwAdderScreenX2f;
	  edi1 = edi1 + dwAdderScreenX2 + carry; }
	{ uint32_t carry = (UINT32_MAX - eax1 < dwAdderScreenX1f)?1:0; eax1 = eax1 + dwAdderScreenX1f;
	  esi1 = esi1 + dwAdderScreenX1 + carry; }
	ecx1 = edi1;

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2DrawPolygonTBASM_PerspPT1ScanlinerTGLoopPass1;
//fcompp
//fcomp




//***> TODO: paire the instructions..

	eax1 = dwDeltaScreenX2;
	esi1 = eax1;
	eax1 = eax1 << ( 12 );
	esi1 = ( (int32_t)esi1 ) >> ( 20 );
	esi1 = esi1 + dwXmax;
	dwAdderScreenX1 = esi1;
	dwAdderScreenX1f = eax1;

	eax1 = dwScreenX3;
	esi1 = eax1;
	eax1 = eax1 << ( 12 );
	esi1 = ( (int32_t)esi1 ) >> ( 20 );
	esi1 = esi1 + dwYOffset2;

	ebp = dwYCounter2;

	fpu_reg12 = fTextureVDivZ3;
	fpu_reg13 = fTextureUDivZ3;
	fpu_reg14 = fOneDivZ3;

MV2DrawPolygonTBASM_PerspPT1ScanlinerTGLoopPass2:
// eax = x1f (20 bit fraction)
// ebx = x2f (20 bit fraction)
// ecx = XCounter (init. BackBuffer + YOffset + X2)
// edx =
// esi = BackBuffer + YOffset + X1
// edi = BackBuffer + YOffset + X2
// ebp = YCounter

// st0 = OneDivZ
// st1 = TextureUDivZ
// st2 = TextureVDivZ
// st3 = 2^20
// st4 = 2^-20

//***> TODO: Pipeline the fpu better
	eax1 = eax1 >> ( 12 );
//sub		esp,CMV2ScanlinerPerspTG__size
	SPTG--;

	dwScreenXf = eax1;
	ecx1 = ecx1 - esi1; // XCounter
	eax1 = eax1 << ( 12 );

	fpu_reg15 = ( (int32_t)dwScreenXf );
	fpu_reg15 = fpu_reg11 - fpu_reg15;
// st0 = 2^20 - ScreenXf, st1 = OneDivZ
// st2 = TextureUDivZ, st3 = TextureVDivZ,
// st4 = 2^20, st5 = 2^-20

	fpu_reg15 = fpu_reg15 * fpu_reg10;
//fst		dword ptr ss:[esp].CMV2ScanlinerPerspTG__fScreenXError
	SPTG->CMV2ScanlinerPerspTG__fScreenXError = fpu_reg15;
// st0 = ScreenXError

	fpu_reg16 = fTextureUDivZdX;
	fpu_reg16 = fpu_reg16 * fpu_reg15;
	fpu_reg17 = fTextureVDivZdX;
	fpu_reg17 = fpu_reg17 * fpu_reg15;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 * fOneDivZdX;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
// st0 = TextureVDivZdX*ScreenXError
// st1 = TextureUDivZdX*ScreenXError
// st2 = OneDivZdX*ScreenXError, st3 = OneDivZ
// st4 = TextureUDivZ, st5 = TextureVDivZ,
// st6 = 2^20, st7 = 2^-20

	fpu_reg17 = fpu_reg17 + fpu_reg12;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 + fpu_reg13;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 + fpu_reg14;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
// st0 = TextureUDivZ*ScreenXError + TextureUDivZ
// st1 = TextureVDivZ*ScreenXError + TextureVDivZ
// st2 = OneDivZ*ScreenXError + OneDivZ
// st3 = OneDivZ
// st4 = TextureUDivZ, st5 = TextureVDivZ,
// st6 = 2^20, st7 = 2^-20

//fstp	DWORD PTR ss:[esp].CMV2ScanlinerPerspTG__fTextureUDivZ
//fstp	DWORD PTR ss:[esp].CMV2ScanlinerPerspTG__fTextureVDivZ
//fstp	DWORD PTR ss:[esp].CMV2ScanlinerPerspTG__fOneDivZ
	SPTG->CMV2ScanlinerPerspTG__fTextureUDivZ = fpu_reg17;
	SPTG->CMV2ScanlinerPerspTG__fTextureVDivZ = fpu_reg16;
	SPTG->CMV2ScanlinerPerspTG__fOneDivZ = fpu_reg15;
// st0 = OneDivZ, st1 = TextureUDivZ, st2 = TextureVDivZ
// st3 = 2^20, st4 = 2^-20

//mov		ss:[esp].CMV2ScanlinerPerspTG__dwScreenX1Offset,esi
//mov		ss:[esp].CMV2ScanlinerPerspTG__dwXCounter,ecx
	SPTG->CMV2ScanlinerPerspTG__dwScreenX1Offset = esi1;
	SPTG->CMV2ScanlinerPerspTG__dwXCounter = ecx1;

	fpu_reg14 = fpu_reg14 + fOneDivZdY2;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg14 = fpu_reg14 + fTextureUDivZdY2;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg14 = fpu_reg14 + fTextureVDivZdY2;
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }

	{ uint32_t carry = (UINT32_MAX - ebx1 < dwAdderScreenX2f)?1:0; ebx1 = ebx1 + dwAdderScreenX2f;
	  edi1 = edi1 + dwAdderScreenX2 + carry; }
	{ uint32_t carry = (UINT32_MAX - eax1 < dwAdderScreenX1f)?1:0; eax1 = eax1 + dwAdderScreenX1f;
	  esi1 = esi1 + dwAdderScreenX1 + carry; }
	ecx1 = edi1;

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2DrawPolygonTBASM_PerspPT1ScanlinerTGLoopPass2;
//fcompp
//fcomp





	edx4 = (CMV2ScanlinerPerspTG *)dwESPStartPoint;
	ebp = dwYCounter1;

	fpu_reg12 = fB1;
	fpu_reg13 = fG1;
	fpu_reg14 = fR1;
MV2DrawPolygonTBASM_PerspPT1GouraudPass1:
// eax =
// ebx =
// ecx =
// edx =
// esi =
// edi =
// ebp = YCounter

// st0 = R
// st1 = G
// st2 = B
// st3 = 2^20
// st4 = 2^-20

//***> TODO: Pipeline the fpu better
	edx4--;

	fpu_reg15 = edx4->CMV2ScanlinerPerspTG__fScreenXError;
// st0 = ScreenXError, st0 = R, st1 = G, st2 = B
// st3 = 2^20, st4 = 2^-20

	fpu_reg16 = fGdX;
	fpu_reg16 = fpu_reg16 * fpu_reg15;
	fpu_reg17 = fRdX;
	fpu_reg17 = fpu_reg17 * fpu_reg15;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 * fBdX;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
// st0 = RdX*ScreenXError, st1 = GdX*ScreenXError
// st2 = BdX*ScreenXError, st3 = R, st4 = G, st5 = B
// st6 = 2^20, st7 = 2^-20

	fpu_reg17 = fpu_reg17 + fpu_reg14;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 + fpu_reg13;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 + fpu_reg12;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
// st0 = RdX*ScreenXError + R, st1 = GdX*ScreenXError + G
// st2 = BdX*ScreenXError + B, st3 = R, st4 = G, st5 = B
// st6 = 2^20, st7 = 2^-20

	edx4->CMV2ScanlinerPerspTG__dwR = (int32_t)ceilf(fpu_reg17);
	edx4->CMV2ScanlinerPerspTG__dwG = (int32_t)ceilf(fpu_reg16);
	edx4->CMV2ScanlinerPerspTG__dwB = (int32_t)ceilf(fpu_reg15);
// st0 = R, st1 = G, st2 = B, st3 = 2^20, st4 = 2^-20

	fpu_reg14 = fpu_reg14 + fRdY1;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg14 = fpu_reg14 + fGdY1;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg14 = fpu_reg14 + fBdY1;
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2DrawPolygonTBASM_PerspPT1GouraudPass1;
//fcompp
//fcomp




	ebp = dwYCounter2;
	fpu_reg12 = fB3;
	fpu_reg13 = fG3;
	fpu_reg14 = fR3;
MV2DrawPolygonTBASM_PerspPT1GouraudPass2:
// eax =
// ebx =
// ecx =
// edx =
// esi =
// edi =
// ebp = YCounter

// st0 = R
// st1 = G
// st2 = B
// st3 = 2^20
// st4 = 2^-20

//***> TODO: Pipeline the fpu better
	edx4--;

	fpu_reg15 = edx4->CMV2ScanlinerPerspTG__fScreenXError;
// st0 = ScreenXError, st0 = R, st1 = G, st2 = B
// st3 = 2^20, st4 = 2^-20

	fpu_reg16 = fGdX;
	fpu_reg16 = fpu_reg16 * fpu_reg15;
	fpu_reg17 = fRdX;
	fpu_reg17 = fpu_reg17 * fpu_reg15;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 * fBdX;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
// st0 = RdX*ScreenXError, st1 = GdX*ScreenXError
// st2 = BdX*ScreenXError, st3 = R, st4 = G, st5 = B
// st6 = 2^20, st7 = 2^-20

	fpu_reg17 = fpu_reg17 + fpu_reg14;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 + fpu_reg13;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 + fpu_reg12;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
// st0 = RdX*ScreenXError + R, st1 = GdX*ScreenXError + G
// st2 = BdX*ScreenXError + B, st3 = R, st4 = G, st5 = B
// st6 = 2^20, st7 = 2^-20

	edx4->CMV2ScanlinerPerspTG__dwR = (int32_t)ceilf(fpu_reg17);
	edx4->CMV2ScanlinerPerspTG__dwG = (int32_t)ceilf(fpu_reg16);
	edx4->CMV2ScanlinerPerspTG__dwB = (int32_t)ceilf(fpu_reg15);
// st0 = R, st1 = G, st2 = B, st3 = 2^20, st4 = 2^-20

	fpu_reg14 = fpu_reg14 + fRdY2;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg14 = fpu_reg14 + fGdY2;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg14 = fpu_reg14 + fBdY2;
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2DrawPolygonTBASM_PerspPT1GouraudPass2;

//ffree	st(4)
//ffree	st(3)
//ffree	st(2)
//ffree	st(1)
//ffree	st(0)







	goto MV2DrawPolygonTBASM_DoPerspTGYLoop;
MV2DrawPolygonTBASM_PolygonType2:
//=============>                <==============
//=============> Polygon Type 2 <==============
//=============>                <==============
// edx = pDot1
// esi = pDot2
// edi = pDot3
	fpu_reg10 = edx3->CMV2Dot3DPos__m_fScreenY;
	fpu_reg11 = esi3->CMV2Dot3DPos__m_fScreenY;
	fpu_reg12 = edi3->CMV2Dot3DPos__m_fScreenY;
//st0 = (y3), st1 = (y2), st2 = (y1)

	fpu_reg13 = fpu_reg12;
	fpu_reg13 = fpu_reg13 - fpu_reg11;
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
//st0 = (y3), st1 = (y3 - y2), st2 = (y2), st3 = (y1)

	fpu_reg13 = fpu_reg13 - fpu_reg10;
	{ float tmp = fpu_reg11; fpu_reg11 = fpu_reg13; fpu_reg13 = tmp; }
//st0 = (y2), st1 = (y3 - y2), st2 = (y3 - y1), st3 = (y1)

	fpu_reg13 = fpu_reg13 - fpu_reg10;
	fYCounter1 = fpu_reg13;
	{ float tmp = fpu_reg11; fpu_reg11 = fpu_reg13; fpu_reg13 = tmp; }
//st0 = (y3 - y1), st1 = (y3 - y2), st2 = (y2 - y1), st3 = (y1)

	fpu_reg14 = 1.0f;
	fpu_reg11 = fpu_reg14 / fpu_reg11;

	eax1 = dwXmax;
	ebx1 = edx3->CMV2Dot3DPos__m_iScreenY;
	ebx1 = ebx1 - 1;
	eax1 = ( (int32_t)eax1 ) * ( (int32_t)ebx1 );
	//eax1 = eax1 + pcBackBuffer;
	dwYOffset1 = eax1;

	fpu_reg14 = 1.0f;
	fpu_reg12 = fpu_reg14 / fpu_reg12;

	eax1 = dwXmax;
	ebx1 = esi3->CMV2Dot3DPos__m_iScreenY;
	ebx1 = ebx1 - 1;
	eax1 = ( (int32_t)eax1 ) * ( (int32_t)ebx1 );
	//eax1 = eax1 + pcBackBuffer;
	dwYOffset2 = eax1;

	fpu_reg14 = 1.0f;
	fpu_reg13 = fpu_reg14 / fpu_reg13;

	eax1 = edx3->CMV2Dot3DPos__m_iScreenY;
	ebx1 = esi3->CMV2Dot3DPos__m_iScreenY;
	ecx1 = edi3->CMV2Dot3DPos__m_iScreenY;

	ebp = ebx1;
	ebp = ebp - eax1;
	dwYCounter1 = ebp;
	ebp = ecx1;
	ebp = ebp - eax1;
	dwYCounter = ebp;
	ebp = ecx1;
	ebp = ebp - ebx1;
	dwYCounter2 = ebp;

	// fpu_reg10 = 0.0f;

//st0 = 1/YCounter, st1 = 1/YCounter2, st2 = 1/YCounter1
//===>                                                                <===
//===>	Screen Delta Calculation {                                   <===
//===>                                                                <===
	fpu_reg14 = edx3->CMV2Dot3DPos__m_fScreenX;
	fpu_reg15 = esi3->CMV2Dot3DPos__m_fScreenX;
	fpu_reg16 = edi3->CMV2Dot3DPos__m_fScreenX;
//st0 = (x3), st1 = (x2), st2 = (x1)
//st3 = 1/YCounter, st4 = 1/YCounter2, st5 = 1/YCounter1

	fpu_reg17 = fpu_reg16;
	fpu_reg17 = fpu_reg17 - fpu_reg15;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 - fpu_reg14;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 * fpu_reg12;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 - fpu_reg14;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 * fpu_reg13;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg17; fpu_reg17 = tmp; }
//ffree   st
//fincstp

	fpu_reg16 = fpu_reg16 * fpu_reg11;
//st0 = DeltaScreenX1, st1 = DeltaScreenX2
//st2 = DeltaScreenX
//st3 = 1/YCounter, st4 = 1/YCounter2, st5 = 1/YCounter1

	fpu_reg17 = edx3->CMV2Dot3DPos__m_fScreenYError;
	fpu_reg18 = fpu_reg17;
	fpu_reg18 = fpu_reg18 * fpu_reg16;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg18 = fpu_reg18 * fpu_reg14;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg18 = fpu_reg18 + edx3->CMV2Dot3DPos__m_fScreenX;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg18 = fpu_reg18 + edx3->CMV2Dot3DPos__m_fScreenX;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
//st0 = ScreenYError1*DeltaScreenX1 + ScreenX1
//st1 = ScreenYError1*DeltaScreenX + ScreenX1
//st2 = DeltaScreenX1, st3 = DeltaScreenX2
//st4 = DeltaScreenX, st3 = 1/YCounter,
//st4 = 1/YCounter2, st5 = 1/YCounter1
	dwScreenX2 = (int32_t)ceilf(fpu_reg18);

	fpu_reg18 = esi3->CMV2Dot3DPos__m_fScreenYError;
	fpu_reg18 = fpu_reg18 * fpu_reg15;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	dwScreenX1 = (int32_t)ceilf(fpu_reg18);
	fpu_reg17 = fpu_reg17 + esi3->CMV2Dot3DPos__m_fScreenX;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	dwDeltaScreenX1 = (int32_t)ceilf(fpu_reg17);
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	dwDeltaScreenX2 = (int32_t)ceilf(fpu_reg16);
	dwScreenX3 = (int32_t)ceilf(fpu_reg15);

//===>	MaxXCounter calculation	{        	                         <===
//st0 = DeltaScreenX
//st1 = 1/YCounter, st2 = 1/YCounter2, st3 = 1/YCounter1
	fpu_reg15 = esi3->CMV2Dot3DPos__m_fScreenX;
	fpu_reg16 = fYCounter1;
	fpu_reg16 = fpu_reg16 * fpu_reg14;
	fpu_reg16 = fpu_reg16 + edx3->CMV2Dot3DPos__m_fScreenX;
	fpu_reg15 = fpu_reg15 - fpu_reg16;
	dwMaxXCounter = (int32_t)ceilf(fpu_reg15);
	fpu_reg16 = 1.0f;
	fpu_reg15 = fpu_reg16 / fpu_reg15;
//st0=1/MaxXCounter, st1 = fDeltascreenX
//st2=1/y_counter13, st3=1/y_counter12, st4=1/y_counter32
	fMaxXCounterRZP = fpu_reg15; //fp dep.

//===>	MaxXCounter calculation	}        	                         <===
	dwDeltaScreenX = (int32_t)ceilf(fpu_reg14);

//===>                                                                <===
//===>	Screen Delta Calculation }                                   <===
//===>                                                                <===
//st0 = 1/YCounter, st1 = 1/YCounter2, st2 = 1/YCounter1

//===>                                                                <===
//===>	RGB DeltaY Calculation {   		                         	 <===
//===>                                                                <===

	fpu_reg14 = edi3->CMV2Dot3DPos__m_fR;
	fpu_reg14 = fpu_reg14 - edx3->CMV2Dot3DPos__m_fR;
	fpu_reg15 = edi3->CMV2Dot3DPos__m_fG;
	fpu_reg15 = fpu_reg15 - edx3->CMV2Dot3DPos__m_fG;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = fpu_reg15 * fpu_reg13;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = fpu_reg15 * fpu_reg13; // fmul stall (+1 Cycle)
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg16 = edx3->CMV2Dot3DPos__m_fScreenYError;
	fpu_reg17 = fpu_reg16;
	fpu_reg17 = fpu_reg17 * fpu_reg15;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 * fpu_reg14; // fmul stall (+1 Cycle)
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
//st0 = ScreenYError1*RdY
//st1 = ScreenYError1*GdY
//st2 = RdY, st3 = GdY
//st4 = 1/YCounter, st5 = 1/YCounter2, st6 = 1/YCounter1

	fpu_reg17 = fpu_reg17 + edx3->CMV2Dot3DPos__m_fR;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 + edx3->CMV2Dot3DPos__m_fG;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }

	fR1 = fpu_reg17;
	fG1 = fpu_reg16;
	fRdY = fpu_reg15;
	fGdY = fpu_reg14;

	fpu_reg14 = edi3->CMV2Dot3DPos__m_fB;
	fpu_reg14 = fpu_reg14 - edx3->CMV2Dot3DPos__m_fB;
	fpu_reg14 = fpu_reg14 * fpu_reg13;
//st0 = BdY, st1 = 1/YCounter, st2 = 1/YCounter2, st3 = 1/YCounter1

	fpu_reg15 = edx3->CMV2Dot3DPos__m_fScreenYError;
	fpu_reg15 = fpu_reg15 * fpu_reg14;
	fpu_reg15 = fpu_reg15 + edx3->CMV2Dot3DPos__m_fB;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fBdY = fpu_reg15;
	fB1 = fpu_reg14;
//===>                                                                <===
//===>	RGB DeltaY Calculation }   		                         	 <===
//===>                                                                <===



//===>                                                                <===
//===>	RGB DeltaX Calculation {   		                         	 <===
//===>                                                                <===
	fpu_reg14 = fMaxXCounterRZP;
	fpu_reg14 = fpu_reg14 * fNum_2EXP20;
	fpu_reg15 = fYCounter1;
	fpu_reg16 = fRdY;
	fpu_reg17 = fGdY;
	fpu_reg18 = fBdY;
//st0 = fBdY, st1 = fGdY, st2 = fRdY
//st3 = YCounter1, st4 = 1/MaxXCounter
//st5 = 1/YCounter, st6 = 1/YCounter2, st7 = 1/YCounter1

	fpu_reg18 = fpu_reg18 * fpu_reg15;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg18 = fpu_reg18 * fpu_reg15;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg18; fpu_reg18 = tmp; }
	fpu_reg18 = fpu_reg18 * fpu_reg15;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg18; fpu_reg18 = tmp; }

	fpu_reg18 = fpu_reg18 + edx3->CMV2Dot3DPos__m_fB;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg18 = fpu_reg18 + edx3->CMV2Dot3DPos__m_fG;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg18; fpu_reg18 = tmp; }
	fpu_reg18 = fpu_reg18 + edx3->CMV2Dot3DPos__m_fR;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg18; fpu_reg18 = tmp; }

	fpu_reg18 = esi3->CMV2Dot3DPos__m_fB - fpu_reg18;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg18 = esi3->CMV2Dot3DPos__m_fG - fpu_reg18;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg18; fpu_reg18 = tmp; }
	fpu_reg18 = esi3->CMV2Dot3DPos__m_fR - fpu_reg18;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg18; fpu_reg18 = tmp; }

	fpu_reg18 = fpu_reg18 * fpu_reg14;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg18 = fpu_reg18 * fpu_reg14;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg18; fpu_reg18 = tmp; }
	fpu_reg18 = fpu_reg18 * fpu_reg14;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg18; fpu_reg18 = tmp; }

	fBdX = fpu_reg18;
	dwBdX = (int32_t)ceilf(fpu_reg18);
	fGdX = fpu_reg17;
	dwGdX = (int32_t)ceilf(fpu_reg17);
	fRdX = fpu_reg16;
	dwRdX = (int32_t)ceilf(fpu_reg16);


//===>                                                                <===
//===>	RGB DeltaX Calculation }   		                         	 <===
//===>                                                                <===
	ebx1 = dwYCounter;

	if (ebx1 >= ( dwSpanLength )) goto MV2DrawPolygonTBASM_PerspPT2;

	eax1 = edx3->CMV2Dot3DPos__m_iScreenX;
	ebx1 = esi3->CMV2Dot3DPos__m_iScreenX;
	ebp = eax1;
	ecx1 = edi3->CMV2Dot3DPos__m_iScreenX;

	eax1 = eax1 - ebx1; // p1 - p2
	ebx1 = ebx1 - ecx1; // p2 - p3
	ecx1 = ecx1 - ebp; // p3 - p1


	if ( (( (int32_t)eax1 ) - ( 0 )) >= 0) goto MV2DrawPolygonTBASM_PerspCheck1PT2;
	eax1 = - ( (int32_t)eax1 );
MV2DrawPolygonTBASM_PerspCheck1PT2:

	if ( (( (int32_t)ebx1 ) - ( 0 )) >= 0) goto MV2DrawPolygonTBASM_PerspCheck2PT2;
	ebx1 = - ( (int32_t)ebx1 );
MV2DrawPolygonTBASM_PerspCheck2PT2:

	if ( (( (int32_t)ecx1 ) - ( 0 )) >= 0) goto MV2DrawPolygonTBASM_PerspCheck3PT2;
	ecx1 = - ( (int32_t)ecx1 );
MV2DrawPolygonTBASM_PerspCheck3PT2:


	if (eax1 >= ( dwSpanLength )) goto MV2DrawPolygonTBASM_PerspPT2;


	if (ebx1 >= ( dwSpanLength )) goto MV2DrawPolygonTBASM_PerspPT2;


	if (ecx1 >= ( dwSpanLength )) goto MV2DrawPolygonTBASM_PerspPT2;

	goto MV2DrawPolygonTBASM_LinearPolygonType2;
MV2DrawPolygonTBASM_PerspPT2:



	fOneDivZ1NSBC = edx3->CMV2Dot3DPos__m_fZNewRZP;
	fOneDivZ2NSBC = esi3->CMV2Dot3DPos__m_fZNewRZP;
	fOneDivZ3NSBC = edi3->CMV2Dot3DPos__m_fZNewRZP;

	fScreenYError1 = edx3->CMV2Dot3DPos__m_fScreenYError;
	fScreenYError2 = esi3->CMV2Dot3DPos__m_fScreenYError;

	edx2 = pDot1;
	esi2 = pDot2;
	edi2 = pDot3;


//===>                                                                <===
//===>	TextureU&VDivZ DeltaY Calculation {                          <===
//===>                                                                <===
//st0 = 1/YCounter, st1 = 1/YCounter2, st2 = 1/YCounter1

	fpu_reg14 = edx2->CMV2Dot3D__m_fTextureU;
	fpu_reg14 = fpu_reg14 * fOneDivZ1NSBC;
	fpu_reg15 = edi2->CMV2Dot3D__m_fTextureU;
	fpu_reg15 = fpu_reg15 * fOneDivZ3NSBC;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
//st0 = tu1/z1, st1 = tu3/z3
	fTextureUDivZ1NSBC = fpu_reg15;
	fpu_reg14 = fpu_reg14 - fpu_reg15;
//st0 = (tu3/z3 - tu1/z1)
//st1 = 1/YCounter, st2 = 1/YCounter2, st3 = 1/YCounter1

	fpu_reg15 = edx2->CMV2Dot3D__m_fTextureV;
	fpu_reg15 = fpu_reg15 * fOneDivZ1NSBC;
	fpu_reg16 = edi2->CMV2Dot3D__m_fTextureV;
	fpu_reg16 = fpu_reg16 * fOneDivZ3NSBC;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
//st0 = tv1/z1, st1 = vu3/z3
	fTextureVDivZ1NSBC = fpu_reg16;
	fpu_reg15 = fpu_reg15 - fpu_reg16;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
//st0 = (tu3/z3 - tu1/z1), st1 = (tv3/z3 - tv1/z1)
//st2 = 1/YCounter, st3 = 1/YCounter2, st4 = 1/YCounter1

	fpu_reg15 = fpu_reg15 * fpu_reg13;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = fpu_reg15 * fpu_reg13; // fmul stall (+1 Cycle)
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg16 = fScreenYError1;
	fpu_reg17 = fpu_reg16;
	fpu_reg17 = fpu_reg17 * fpu_reg15;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 * fpu_reg14; // fmul stall (+1 Cycle)
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
//st0 = ScreenYError1*TextureUDivZdY
//st1 = ScreenYError1*TextureVDivZdY
//st2 = TextureUDivZdY, st3 = TextureVDivZdY
//st4 = 1/YCounter, st5 = 1/YCounter2, st6 = 1/YCounter1

	fpu_reg17 = fpu_reg17 + fTextureUDivZ1NSBC;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 + fTextureVDivZ1NSBC;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fTextureUDivZ1 = fpu_reg17;
	fTextureVDivZ1 = fpu_reg16;
	fTextureUDivZdY = fpu_reg15;
	fTextureVDivZdY = fpu_reg14;

//===>                                                                <===
//===>	TextureU&VDivZ DeltaY Calculation }                          <===
//===>                                                                <===


//===>                                                                <===
//===>	OneDivZ DeltaY Calculation {                            	 <===
//===>                                                                <===
//***> TODO: Optimize

	fpu_reg14 = fOneDivZ3NSBC;
	fpu_reg14 = fpu_reg14 - fOneDivZ1NSBC;
//st0 = (1/z3 - 1/z1)
//st1 = 1/YCounter, st2 = 1/YCounter2, st3 = 1/YCounter1

	fpu_reg14 = fpu_reg14 * fpu_reg13;
//st0 = OneDivZdY

	fpu_reg15 = fScreenYError1;
	fpu_reg15 = fpu_reg15 * fpu_reg14;
//st0 = ScreenYError1*OneDivZdY
//st1 = OneDivZdY
	fpu_reg15 = fpu_reg15 + fOneDivZ1NSBC;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
//st0 = OneDivZdY
//st1 = OneDivZ1

	fOneDivZdY = fpu_reg15;
	fOneDivZ1 = fpu_reg14;
//===>                                                                <===
//===>	OneDivZ DeltaY Calculation }                            	 <===
//===>                                                                <===
//st0 = 1/YCounter, st1 = 1/YCounter2, st2 = 1/YCounter1

//===>                                                                <===
//===>	TextureU&VDivZ DeltaX Calculation {                          <===
//===>                                                                <===

	fpu_reg14 = fOneDivZ2NSBC;
	fpu_reg15 = esi2->CMV2Dot3D__m_fTextureU;
	fpu_reg15 = fpu_reg15 * fpu_reg14;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = fpu_reg15 * esi2->CMV2Dot3D__m_fTextureV;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fTextureUDivZ2NSBC = fpu_reg15;
	fTextureVDivZ2NSBC = fpu_reg14;

	fpu_reg14 = fMaxXCounterRZP;
	fpu_reg14 = fpu_reg14 * fNum_2EXP20;
	fpu_reg15 = fYCounter1;
	fpu_reg16 = fTextureVDivZdY;
	fpu_reg17 = fTextureUDivZdY;
//st0 = TextureUDivZdY, st1 = TextureVDivZdY
//st2 = YCounter1, st3 = 1/MaxXCounter
//st4 = 1/YCounter, st5 = 1/YCounter2, st6 = 1/YCounter1

	fpu_reg17 = fpu_reg17 * fpu_reg15;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg15 = fpu_reg15 * fpu_reg17; // fmul stall (+ 1 Cycle)
	fpu_reg16 = fpu_reg16 + fTextureUDivZ1NSBC;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = fpu_reg16 + fTextureVDivZ1NSBC;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = fTextureUDivZ2NSBC - fpu_reg16;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = fTextureVDivZ2NSBC - fpu_reg16;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = fpu_reg16 * fpu_reg14;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg14 = fpu_reg14 * fpu_reg16;
//st0 = fTextureUDivZdX
//st1 = fTextureVDivZdX
//st2 = 1/MaxXCounter
//st3 = 1/YCounter, st4 = 1/YCounter2, st5 = 1/YCounter1

	fTextureUDivZdX = fpu_reg15;
	fTextureVDivZdX = fpu_reg14;
//===>                                                                <===
//===>	TextureU&VDivZ DeltaX Calculation }                          <===
//===>                                                                <===


//===>                                                                <===
//===>	OneDivZ DeltaX Calculation {                          		 <===
//===>                                                                <===
//***> TODO: Optimize

	fpu_reg14 = fMaxXCounterRZP;
	fpu_reg14 = fpu_reg14 * fNum_2EXP20;
	fpu_reg15 = fYCounter1;
	fpu_reg15 = fpu_reg15 * fOneDivZdY;
//st0 = OneDivZ1dY*YCounter1
//st1 = 1/MaxXCounter
//st2 = 1/YCounter, st3 = 1/YCounter2, st4 = 1/YCounter1

	fpu_reg15 = fpu_reg15 + fOneDivZ1NSBC;
	fpu_reg15 = fOneDivZ2NSBC - fpu_reg15;
	fpu_reg14 = fpu_reg14 * fpu_reg15;
//st0 = fOneDivZdX
//st2 = 1/YCounter, st3 = 1/YCounter2, st4 = 1/YCounter1

	fOneDivZdX = fpu_reg14;
//===>                                                                <===
//===>	OneDivZ DeltaX Calculation }                          		 <===
//===>                                                                <===
//ffree	st(2)
//ffree	st(1)
//ffree	st(0)





	eax1 = dwDeltaScreenX1;
	ebx1 = dwDeltaScreenX;
	esi1 = eax1;
	edi1 = ebx1;
	eax1 = eax1 << ( 12 );
	ebx1 = ebx1 << ( 12 );
	esi1 = ( (int32_t)esi1 ) >> ( 20 );
	edi1 = ( (int32_t)edi1 ) >> ( 20 );
	esi1 = esi1 + dwXmax;
	edi1 = edi1 + dwXmax;
	dwAdderScreenX2 = esi1;
	dwAdderScreenX1 = edi1;
	dwAdderScreenX2f = eax1;
	dwAdderScreenX1f = ebx1;

	eax1 = dwScreenX1;
	ebx1 = dwScreenX2;
	esi1 = eax1;
	edi1 = ebx1;
	eax1 = eax1 << ( 12 );
	ebx1 = ebx1 << ( 12 );
	esi1 = ( (int32_t)esi1 ) >> ( 20 );
	edi1 = ( (int32_t)edi1 ) >> ( 20 );
	esi1 = esi1 + dwYOffset1;
	edi1 = edi1 + dwYOffset1;

	ebp = dwYCounter1;
	ecx1 = edi1;

//mov		dwOldESP,esp
//and		esp,0ffffffe0h
	SPTG = (CMV2ScanlinerPerspTG *) mem_alloc_endptr(dwYCounter * sizeof(CMV2ScanlinerPerspTG));
//mov		dwESPStartPoint,esp
	dwESPStartPoint = (void *)SPTG;

	fpu_reg10 = fNum_2EXP_20;
	fpu_reg11 = fNum_2EXP20;
	fpu_reg12 = fTextureVDivZ1;
	fpu_reg13 = fTextureUDivZ1;
	fpu_reg14 = fOneDivZ1;

MV2DrawPolygonTBASM_PerspPT2ScanlinerTGLoopPass1:
// eax = x1f (20 bit fraction)
// ebx = x2f (20 bit fraction)
// ecx = XCounter (init. BackBuffer + YOffset + X2)
// edx =
// esi = BackBuffer + YOffset + X1
// edi = BackBuffer + YOffset + X2
// ebp = YCounter

// st0 = OneDivZ
// st1 = TextureUDivZ
// st2 = TextureVDivZ
// st3 = 2^20
// st4 = 2^-20

//***> TODO: Pipeline the fpu better
	eax1 = eax1 >> ( 12 );
//sub		esp,CMV2ScanlinerPerspTG__size
	SPTG--;

	dwScreenXf = eax1;
	ecx1 = ecx1 - esi1; // XCounter
	eax1 = eax1 << ( 12 );

	fpu_reg15 = ( (int32_t)dwScreenXf );
	fpu_reg15 = fpu_reg11 - fpu_reg15;
// st0 = 2^20 - ScreenXf, st1 = OneDivZ
// st2 = TextureUDivZ, st3 = TextureVDivZ,
// st4 = 2^20, st5 = 2^-20

	fpu_reg15 = fpu_reg15 * fpu_reg10;
//fst		dword ptr ss:[esp].CMV2ScanlinerPerspTG__fScreenXError
	SPTG->CMV2ScanlinerPerspTG__fScreenXError = fpu_reg15;
// st0 = ScreenXError

	fpu_reg16 = fTextureUDivZdX;
	fpu_reg16 = fpu_reg16 * fpu_reg15;
	fpu_reg17 = fTextureVDivZdX;
	fpu_reg17 = fpu_reg17 * fpu_reg15;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 * fOneDivZdX;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
// st0 = TextureVDivZdX*ScreenXError
// st1 = TextureUDivZdX*ScreenXError
// st2 = OneDivZdX*ScreenXError, st3 = OneDivZ
// st4 = TextureUDivZ, st5 = TextureVDivZ,
// st6 = 2^20, st7 = 2^-20

	fpu_reg17 = fpu_reg17 + fpu_reg12;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 + fpu_reg13;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 + fpu_reg14;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
// st0 = TextureUDivZ*ScreenXError + TextureUDivZ
// st1 = TextureVDivZ*ScreenXError + TextureVDivZ
// st2 = OneDivZ*ScreenXError + OneDivZ
// st3 = OneDivZ
// st4 = TextureUDivZ, st5 = TextureVDivZ,
// st6 = 2^20, st7 = 2^-20

//fstp	DWORD PTR ss:[esp].CMV2ScanlinerPerspTG__fTextureUDivZ
//fstp	DWORD PTR ss:[esp].CMV2ScanlinerPerspTG__fTextureVDivZ
//fstp	DWORD PTR ss:[esp].CMV2ScanlinerPerspTG__fOneDivZ
	SPTG->CMV2ScanlinerPerspTG__fTextureUDivZ = fpu_reg17;
	SPTG->CMV2ScanlinerPerspTG__fTextureVDivZ = fpu_reg16;
	SPTG->CMV2ScanlinerPerspTG__fOneDivZ = fpu_reg15;
// st0 = OneDivZ, st1 = TextureUDivZ, st2 = TextureVDivZ
// st3 = 2^20, st4 = 2^-20

//mov		ss:[esp].CMV2ScanlinerPerspTG__dwScreenX1Offset,esi
//mov		ss:[esp].CMV2ScanlinerPerspTG__dwXCounter,ecx
	SPTG->CMV2ScanlinerPerspTG__dwScreenX1Offset = esi1;
	SPTG->CMV2ScanlinerPerspTG__dwXCounter = ecx1;

	fpu_reg14 = fpu_reg14 + fOneDivZdY;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg14 = fpu_reg14 + fTextureUDivZdY;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg14 = fpu_reg14 + fTextureVDivZdY;
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }

	{ uint32_t carry = (UINT32_MAX - ebx1 < dwAdderScreenX2f)?1:0; ebx1 = ebx1 + dwAdderScreenX2f;
	  edi1 = edi1 + dwAdderScreenX2 + carry; }
	{ uint32_t carry = (UINT32_MAX - eax1 < dwAdderScreenX1f)?1:0; eax1 = eax1 + dwAdderScreenX1f;
	  esi1 = esi1 + dwAdderScreenX1 + carry; }
	ecx1 = edi1;

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2DrawPolygonTBASM_PerspPT2ScanlinerTGLoopPass1;

//***> TODO: paire the instructions..
	ebx1 = dwDeltaScreenX2;
	edi1 = ebx1;
	ebx1 = ebx1 << ( 12 );
	edi1 = ( (int32_t)edi1 ) >> ( 20 );
	edi1 = edi1 + dwXmax;
	dwAdderScreenX2 = edi1;
	dwAdderScreenX2f = ebx1;

	ebx1 = dwScreenX3;
	edi1 = ebx1;
	ebx1 = ebx1 << ( 12 );
	edi1 = ( (int32_t)edi1 ) >> ( 20 );
	edi1 = edi1 + dwYOffset2;

	ebp = dwYCounter2;
	ecx1 = edi1;

MV2DrawPolygonTBASM_PerspPT2ScanlinerTGLoopPass2:
// eax = x1f (20 bit fraction)
// ebx = x2f (20 bit fraction)
// ecx = XCounter (init. BackBuffer + YOffset + X2)
// edx =
// esi = BackBuffer + YOffset + X1
// edi = BackBuffer + YOffset + X2
// ebp = YCounter

// st0 = OneDivZ
// st1 = TextureUDivZ
// st2 = TextureVDivZ
// st3 = 2^20
// st4 = 2^-20

//***> TODO: Pipeline the fpu better
	eax1 = eax1 >> ( 12 );
//sub		esp,CMV2ScanlinerPerspTG__size
	SPTG--;

	dwScreenXf = eax1;
	ecx1 = ecx1 - esi1; // XCounter
	eax1 = eax1 << ( 12 );

	fpu_reg15 = ( (int32_t)dwScreenXf );
	fpu_reg15 = fpu_reg11 - fpu_reg15;
// st0 = 2^20 - ScreenXf, st1 = OneDivZ
// st2 = TextureUDivZ, st3 = TextureVDivZ,
// st4 = 2^20, st5 = 2^-20

	fpu_reg15 = fpu_reg15 * fpu_reg10;
//fst		dword ptr ss:[esp].CMV2ScanlinerPerspTG__fScreenXError
	SPTG->CMV2ScanlinerPerspTG__fScreenXError = fpu_reg15;
// st0 = ScreenXError

	fpu_reg16 = fTextureUDivZdX;
	fpu_reg16 = fpu_reg16 * fpu_reg15;
	fpu_reg17 = fTextureVDivZdX;
	fpu_reg17 = fpu_reg17 * fpu_reg15;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 * fOneDivZdX;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
// st0 = TextureVDivZdX*ScreenXError
// st1 = TextureUDivZdX*ScreenXError
// st2 = OneDivZdX*ScreenXError, st3 = OneDivZ
// st4 = TextureUDivZ, st5 = TextureVDivZ,
// st6 = 2^20, st7 = 2^-20

	fpu_reg17 = fpu_reg17 + fpu_reg12;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 + fpu_reg13;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 + fpu_reg14;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
// st0 = TextureUDivZ*ScreenXError + TextureUDivZ
// st1 = TextureVDivZ*ScreenXError + TextureVDivZ
// st2 = OneDivZ*ScreenXError + OneDivZ
// st3 = OneDivZ
// st4 = TextureUDivZ, st5 = TextureVDivZ,
// st6 = 2^20, st7 = 2^-20

//fstp	DWORD PTR ss:[esp].CMV2ScanlinerPerspTG__fTextureUDivZ
//fstp	DWORD PTR ss:[esp].CMV2ScanlinerPerspTG__fTextureVDivZ
//fstp	DWORD PTR ss:[esp].CMV2ScanlinerPerspTG__fOneDivZ
	SPTG->CMV2ScanlinerPerspTG__fTextureUDivZ = fpu_reg17;
	SPTG->CMV2ScanlinerPerspTG__fTextureVDivZ = fpu_reg16;
	SPTG->CMV2ScanlinerPerspTG__fOneDivZ = fpu_reg15;
// st0 = OneDivZ, st1 = TextureUDivZ, st2 = TextureVDivZ
// st3 = 2^20, st4 = 2^-20

//mov		ss:[esp].CMV2ScanlinerPerspTG__dwScreenX1Offset,esi
//mov		ss:[esp].CMV2ScanlinerPerspTG__dwXCounter,ecx
	SPTG->CMV2ScanlinerPerspTG__dwScreenX1Offset = esi1;
	SPTG->CMV2ScanlinerPerspTG__dwXCounter = ecx1;

	fpu_reg14 = fpu_reg14 + fOneDivZdY;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg14 = fpu_reg14 + fTextureUDivZdY;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg14 = fpu_reg14 + fTextureVDivZdY;
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }

	{ uint32_t carry = (UINT32_MAX - ebx1 < dwAdderScreenX2f)?1:0; ebx1 = ebx1 + dwAdderScreenX2f;
	  edi1 = edi1 + dwAdderScreenX2 + carry; }
	{ uint32_t carry = (UINT32_MAX - eax1 < dwAdderScreenX1f)?1:0; eax1 = eax1 + dwAdderScreenX1f;
	  esi1 = esi1 + dwAdderScreenX1 + carry; }
	ecx1 = edi1;

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2DrawPolygonTBASM_PerspPT2ScanlinerTGLoopPass2;
//fcompp
//fcomp




	edx4 = (CMV2ScanlinerPerspTG *)dwESPStartPoint;
	ebp = dwYCounter;

	fpu_reg12 = fB1;
	fpu_reg13 = fG1;
	fpu_reg14 = fR1;
MV2DrawPolygonTBASM_PerspPT2GouraudPass1_2:
// eax =
// ebx =
// ecx =
// edx =
// esi =
// edi =
// ebp = YCounter

// st0 = R
// st1 = G
// st2 = B
// st3 = 2^20
// st4 = 2^-20

//***> TODO: Pipeline the fpu better
	edx4--;

	fpu_reg15 = edx4->CMV2ScanlinerPerspTG__fScreenXError;
// st0 = ScreenXError, st0 = R, st1 = G, st2 = B
// st3 = 2^20, st4 = 2^-20

	fpu_reg16 = fGdX;
	fpu_reg16 = fpu_reg16 * fpu_reg15;
	fpu_reg17 = fRdX;
	fpu_reg17 = fpu_reg17 * fpu_reg15;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 * fBdX;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
// st0 = RdX*ScreenXError, st1 = GdX*ScreenXError
// st2 = BdX*ScreenXError, st3 = R, st4 = G, st5 = B
// st6 = 2^20, st7 = 2^-20

	fpu_reg17 = fpu_reg17 + fpu_reg14;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 + fpu_reg13;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 + fpu_reg12;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
// st0 = RdX*ScreenXError + R, st1 = GdX*ScreenXError + G
// st2 = BdX*ScreenXError + B, st3 = R, st4 = G, st5 = B
// st6 = 2^20, st7 = 2^-20

	edx4->CMV2ScanlinerPerspTG__dwR = (int32_t)ceilf(fpu_reg17);
	edx4->CMV2ScanlinerPerspTG__dwG = (int32_t)ceilf(fpu_reg16);
	edx4->CMV2ScanlinerPerspTG__dwB = (int32_t)ceilf(fpu_reg15);
// st0 = R, st1 = G, st2 = B, st3 = 2^20, st4 = 2^-20

	fpu_reg14 = fpu_reg14 + fRdY;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg14 = fpu_reg14 + fGdY;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg14 = fpu_reg14 + fBdY;
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2DrawPolygonTBASM_PerspPT2GouraudPass1_2;

//ffree	st(4)
//ffree	st(3)
//ffree	st(2)
//ffree	st(1)
//ffree	st(0)






	goto MV2DrawPolygonTBASM_DoPerspTGYLoop;
MV2DrawPolygonTBASM_PolygonType3:
//=============>                <==============
//=============> Polygon Type 3 <==============
//=============>                <==============
// edx = pDot1
// esi = pDot2
// edi = pDot3

	fpu_reg10 = edx3->CMV2Dot3DPos__m_fScreenY;
	fpu_reg11 = edi3->CMV2Dot3DPos__m_fScreenY;
	fpu_reg12 = esi3->CMV2Dot3DPos__m_fScreenY;
//st0 = (y2), st1 = (y3), st2 = (y1)

	fpu_reg12 = fpu_reg12 - fpu_reg10;
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg11; fpu_reg11 = tmp; }
//st0 = (y3), st1 = (y2 - y1), st2 = (y1)

	fpu_reg10 = fpu_reg12 - fpu_reg10;
//st0 = (y2 - y1), st1 = (y3 - y1)
	fYCounter2 = fpu_reg11;

	fpu_reg12 = 1.0f;
	fpu_reg13 = fpu_reg12;
	fpu_reg11 = fpu_reg13 / fpu_reg11;
//st0 = 1.0, st1 = (y2 - y1), st2 = 1/(y3 - y1)

	eax1 = dwXmax;
	ebx1 = edx3->CMV2Dot3DPos__m_iScreenY;
	ebx1 = ebx1 - 1;
	eax1 = ( (int32_t)eax1 ) * ( (int32_t)ebx1 );
	//eax1 = eax1 + pcBackBuffer;
	dwYOffset1 = eax1;

	fpu_reg10 = fpu_reg12 / fpu_reg10;
//st0 = 1/(y2 - y1), st1 = 1/(y3 - y1)

	eax1 = edi3->CMV2Dot3DPos__m_iScreenY;
	eax1 = eax1 - edx3->CMV2Dot3DPos__m_iScreenY;
	dwYCounter = eax1;

//===>                                                                <===
//===>	Screen Delta Calculation {                                   <===
//===>                                                                <===
//st0 = 1/YCounter2, st1 = 1/YCounter1

	fpu_reg12 = edx3->CMV2Dot3DPos__m_fScreenX;
	fpu_reg13 = edi3->CMV2Dot3DPos__m_fScreenX;
	fpu_reg14 = esi3->CMV2Dot3DPos__m_fScreenX;
//st0 = (x2), st1 = (x3), st2 = (x1)
//st3 = 1/YCounter2, st4 = 1/YCounter1

	fpu_reg14 = fpu_reg14 - fpu_reg12;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg12 = fpu_reg14 - fpu_reg12;
	fpu_reg13 = fpu_reg13 * fpu_reg11;
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
	fpu_reg13 = fpu_reg13 * fpu_reg10; // fmul stall (+1 Cycle)
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
//st0 = DeltaScreenX2, st1 = DeltaScreenX1
//st2 = 1/YCounter2, st3 = 1/YCounter1

	fpu_reg14 = edx3->CMV2Dot3DPos__m_fScreenYError;
	fpu_reg15 = fpu_reg14;
	fpu_reg15 = fpu_reg15 * fpu_reg12;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = fpu_reg15 * fpu_reg13;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = fpu_reg15 + edx3->CMV2Dot3DPos__m_fScreenX;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = fpu_reg15 + edx3->CMV2Dot3DPos__m_fScreenX;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
//st0 = ScreenYError1*DeltaScreenX1 + ScreenX1
//st1 = ScreenYError1*DeltaScreenX2 + ScreenX1
//st2 = DeltaScreenX2, st3 = DeltaScreenX1
//st4 = 1/YCounter2, st5 = 1/YCounter1

	dwScreenX1 = (int32_t)ceilf(fpu_reg15);
	dwScreenX2 = (int32_t)ceilf(fpu_reg14);
	dwDeltaScreenX2 = (int32_t)ceilf(fpu_reg13);
//st0 = DeltaScreenX1, st0 = 1/YCounter2, st1 = 1/YCounter1


//===>	MaxXCounter calculation	{        	                         <===
	fpu_reg13 = fYCounter2;
	fpu_reg13 = fpu_reg13 * fpu_reg12;
	fpu_reg13 = fpu_reg13 + edx3->CMV2Dot3DPos__m_fScreenX;
	fpu_reg13 = esi3->CMV2Dot3DPos__m_fScreenX - fpu_reg13; //fp dep.
	dwMaxXCounter = (int32_t)ceilf(fpu_reg13);
	fpu_reg14 = 1.0f;
	fpu_reg13 = fpu_reg14 / fpu_reg13;
//st0 = 1/MaxXCounter, st1 = DeltascreenX1
//st2 = 1/YCounter2, st3 = 1/YCounter1
	fMaxXCounterRZP = fpu_reg13; //fp dep.
//===>	MaxXCounter calculation	}        	                         <===

	dwDeltaScreenX1 = (int32_t)ceilf(fpu_reg12);
//===>                                                                <===
//===>	Screen Delta Calculation }                                   <===
//===>                                                                <===

//st0 = 1/YCounter2, st1 = 1/YCounter1

//===>                                                                <===
//===>	RGB DeltaY Calculation {   		                         	 <===
//===>                                                                <===

	fpu_reg12 = edi3->CMV2Dot3DPos__m_fR;
	fpu_reg12 = fpu_reg12 - edx3->CMV2Dot3DPos__m_fR;
	fpu_reg13 = edi3->CMV2Dot3DPos__m_fG;
	fpu_reg13 = fpu_reg13 - edx3->CMV2Dot3DPos__m_fG;
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
	fpu_reg13 = fpu_reg13 * fpu_reg10;
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
	fpu_reg13 = fpu_reg13 * fpu_reg10; // fmul stall (+1 Cycle)
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
	fpu_reg14 = edx3->CMV2Dot3DPos__m_fScreenYError;
	fpu_reg15 = fpu_reg14;
	fpu_reg15 = fpu_reg15 * fpu_reg13;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = fpu_reg15 * fpu_reg12; // fmul stall (+1 Cycle)
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
//st0 = ScreenYError1*RdY
//st1 = ScreenYError1*GdY
//st2 = RdY, st3 = GdY
//st4 = 1/YCounter2, st5 = 1/YCounter1

	fpu_reg15 = fpu_reg15 + edx3->CMV2Dot3DPos__m_fR;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = fpu_reg15 + edx3->CMV2Dot3DPos__m_fG;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }

	fR1 = fpu_reg15;
	fG1 = fpu_reg14;
	fRdY1 = fpu_reg13;
	fGdY1 = fpu_reg12;

	fpu_reg12 = edi3->CMV2Dot3DPos__m_fB;
	fpu_reg12 = fpu_reg12 - edx3->CMV2Dot3DPos__m_fB;
	fpu_reg12 = fpu_reg12 * fpu_reg10;
//st0 = BdY, st1 = 1/YCounter2, st2 = 1/YCounter1

	fpu_reg13 = edx3->CMV2Dot3DPos__m_fScreenYError;
	fpu_reg13 = fpu_reg13 * fpu_reg12;
	fpu_reg13 = fpu_reg13 + edx3->CMV2Dot3DPos__m_fB;
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
	fBdY1 = fpu_reg13;
	fB1 = fpu_reg12;
//===>                                                                <===
//===>	RGB DeltaY Calculation }   		                         	 <===
//===>                                                                <===


//===>                                                                <===
//===>	RGB DeltaX Calculation {   		                         	 <===
//===>                                                                <===
	fpu_reg12 = fMaxXCounterRZP;
	fpu_reg12 = fpu_reg12 * fNum_2EXP20;
	fpu_reg13 = fYCounter2;
	fpu_reg14 = fRdY1;
	fpu_reg15 = fGdY1;
	fpu_reg16 = fBdY1;
//st0 = fBdY1, st1 = fGdY1, st2 = fRdY1
//st3 = YCounter2, st4 = 1/MaxXCounter
//st5 = 1/YCounter2, st6 = 1/YCounter1

	fpu_reg16 = fpu_reg16 * fpu_reg13;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = fpu_reg16 * fpu_reg13;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg16 = fpu_reg16 * fpu_reg13;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg16; fpu_reg16 = tmp; }

	fpu_reg16 = fpu_reg16 + edx3->CMV2Dot3DPos__m_fB;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = fpu_reg16 + edx3->CMV2Dot3DPos__m_fG;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg16 = fpu_reg16 + edx3->CMV2Dot3DPos__m_fR;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg16; fpu_reg16 = tmp; }

	fpu_reg16 = esi3->CMV2Dot3DPos__m_fB - fpu_reg16;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = esi3->CMV2Dot3DPos__m_fG - fpu_reg16;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg16 = esi3->CMV2Dot3DPos__m_fR - fpu_reg16;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg16; fpu_reg16 = tmp; }

	fpu_reg16 = fpu_reg16 * fpu_reg12;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = fpu_reg16 * fpu_reg12;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg16 = fpu_reg16 * fpu_reg12;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg16; fpu_reg16 = tmp; }

	fBdX = fpu_reg16;
	dwBdX = (int32_t)ceilf(fpu_reg16);
	fGdX = fpu_reg15;
	dwGdX = (int32_t)ceilf(fpu_reg15);
	fRdX = fpu_reg14;
	dwRdX = (int32_t)ceilf(fpu_reg14);


//===>                                                                <===
//===>	RGB DeltaX Calculation }   		                         	 <===
//===>                                                                <===
	ebx1 = dwYCounter;

	if (ebx1 >= ( dwSpanLength )) goto MV2DrawPolygonTBASM_PerspPT3;

	eax1 = edx3->CMV2Dot3DPos__m_iScreenX;
	ebx1 = esi3->CMV2Dot3DPos__m_iScreenX;
	ebp = eax1;
	ecx1 = edi3->CMV2Dot3DPos__m_iScreenX;

	eax1 = eax1 - ebx1; // p1 - p2
	ebx1 = ebx1 - ecx1; // p2 - p3
	ecx1 = ecx1 - ebp; // p3 - p1


	if ( (( (int32_t)eax1 ) - ( 0 )) >= 0) goto MV2DrawPolygonTBASM_PerspCheck1PT3;
	eax1 = - ( (int32_t)eax1 );
MV2DrawPolygonTBASM_PerspCheck1PT3:

	if ( (( (int32_t)ebx1 ) - ( 0 )) >= 0) goto MV2DrawPolygonTBASM_PerspCheck2PT3;
	ebx1 = - ( (int32_t)ebx1 );
MV2DrawPolygonTBASM_PerspCheck2PT3:

	if ( (( (int32_t)ecx1 ) - ( 0 )) >= 0) goto MV2DrawPolygonTBASM_PerspCheck3PT3;
	ecx1 = - ( (int32_t)ecx1 );
MV2DrawPolygonTBASM_PerspCheck3PT3:


	if (eax1 >= ( dwSpanLength )) goto MV2DrawPolygonTBASM_PerspPT3;


	if (ebx1 >= ( dwSpanLength )) goto MV2DrawPolygonTBASM_PerspPT3;


	if (ecx1 >= ( dwSpanLength )) goto MV2DrawPolygonTBASM_PerspPT3;

	goto MV2DrawPolygonTBASM_LinearPolygonType3;
MV2DrawPolygonTBASM_PerspPT3:



	fOneDivZ1NSBC = edx3->CMV2Dot3DPos__m_fZNewRZP;
	fOneDivZ2NSBC = esi3->CMV2Dot3DPos__m_fZNewRZP;
	fOneDivZ3NSBC = edi3->CMV2Dot3DPos__m_fZNewRZP;

	fScreenYError1 = edx3->CMV2Dot3DPos__m_fScreenYError;
	fScreenYError2 = esi3->CMV2Dot3DPos__m_fScreenYError;

	edx2 = pDot1;
	esi2 = pDot2;
	edi2 = pDot3;


//===>                                                                <===
//===>	TextureU&VDivZ DeltaY Calculation {                          <===
//===>                                                                <===
//st0 = 1/YCounter2, st1 = 1/YCounter1

	fpu_reg12 = edx2->CMV2Dot3D__m_fTextureU;
	fpu_reg12 = fpu_reg12 * fOneDivZ1NSBC;
	fpu_reg13 = edi2->CMV2Dot3D__m_fTextureU;
	fpu_reg13 = fpu_reg13 * fOneDivZ3NSBC;
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
//st0 = tu1/z1, st1 = tu3/z3
	fTextureUDivZ1NSBC = fpu_reg13;
	fpu_reg12 = fpu_reg12 - fpu_reg13;
//st0 = (tu3/z3 - tu1/z1)
//st1 = 1/YCounter2, st2 = 1/YCounter1

	fpu_reg13 = edx2->CMV2Dot3D__m_fTextureV;
	fpu_reg13 = fpu_reg13 * fOneDivZ1NSBC;
	fpu_reg14 = edi2->CMV2Dot3D__m_fTextureV;
	fpu_reg14 = fpu_reg14 * fOneDivZ3NSBC;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
//st0 = tv1/z1, st1 = tu3/z3
	fTextureVDivZ1NSBC = fpu_reg14;
	fpu_reg13 = fpu_reg13 - fpu_reg14;
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
//st0 = (tu3/z3 - tu1/z1), st1 = (tv3/z3 - tv1/z1)
//st2 = 1/YCounter2, st3 = 1/YCounter1

	fpu_reg13 = fpu_reg13 * fpu_reg10;
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
	fpu_reg13 = fpu_reg13 * fpu_reg10; // fmul stall (+1 Cycle)
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
//st0 = TextureUDivZdY1, st1 = TextureVDivZdY1
//st2 = 1/YCounter2, st3 = 1/YCounter1

	fpu_reg14 = fScreenYError1;
	fpu_reg15 = fpu_reg14;
	fpu_reg15 = fpu_reg15 * fpu_reg13;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = fpu_reg15 * fpu_reg12; // fmul stall (+1 Cycle)
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = fpu_reg15 + fTextureUDivZ1NSBC;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = fpu_reg15 + fTextureVDivZ1NSBC;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
//st0 = ScreenYError1*TextureUDivZdY1 + TextureUDivZ1
//st1 = ScreenYError1*TextureVDivZdY1 + TextureVDivZ1
//st2 = TextureUDivZdY1, st3 = TextureVDivZdY1
//st4 = 1/YCounter2, st5 = 1/YCounter1

	fTextureUDivZ1 = fpu_reg15;
	fTextureVDivZ1 = fpu_reg14;
	fTextureUDivZdY1 = fpu_reg13;
	fTextureVDivZdY1 = fpu_reg12;
//===>                                                                <===
//===>	TextureU&VDivZ DeltaY Calculation }                          <===
//===>                                                                <===




//===>                                                                <===
//===>	OneDivZ DeltaY Calculation {                            	 <===
//===>                                                                <===
//***> TODO: Optimize

	fpu_reg12 = fOneDivZ3NSBC;
	fpu_reg12 = fpu_reg12 - fOneDivZ1NSBC;
//st0 = (1/z3 - 1/z1)
//st1 = 1/YCounter2, st2 = 1/YCounter1

	fpu_reg12 = fpu_reg12 * fpu_reg10;
//st0 = OneDivZdY

	fpu_reg13 = fScreenYError1;
	fpu_reg13 = fpu_reg13 * fpu_reg12;
//st0 = ScreenYError1*OneDivZdY
//st1 = OneDivZdY
	fpu_reg13 = fpu_reg13 + fOneDivZ1NSBC;
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
//st0 = OneDivZdY
//st1 = OneDivZ1

	fOneDivZdY1 = fpu_reg13;
	fOneDivZ1 = fpu_reg12;
//===>                                                                <===
//===>	OneDivZ DeltaY Calculation }                            	 <===
//===>                                                                <===




//===>                                                                <===
//===>	TextureU&VDivZ DeltaX Calculation {                          <===
//===>                                                                <===
	fpu_reg12 = fOneDivZ2NSBC;
	fpu_reg13 = esi2->CMV2Dot3D__m_fTextureU;
	fpu_reg13 = fpu_reg13 * fpu_reg12;
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
	fpu_reg13 = fpu_reg13 * esi2->CMV2Dot3D__m_fTextureV;
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
	fTextureUDivZ2NSBC = fpu_reg13;
	fTextureVDivZ2NSBC = fpu_reg12;

	fpu_reg12 = fMaxXCounterRZP;
	fpu_reg12 = fpu_reg12 * fNum_2EXP20;
	fpu_reg13 = fYCounter2;
	fpu_reg14 = fTextureVDivZdY1;
	fpu_reg15 = fTextureUDivZdY1;
//st0 = TextureUDivZdY1, st1 = TextureVDivZdY1
//st2 = YCounter2, st3 = 1/MaxXCounter
//st4 = 1/YCounter2, st5 = 1/YCounter1

	fpu_reg15 = fpu_reg15 * fpu_reg13;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg13 = fpu_reg13 * fpu_reg15; // fmul stall (+ 1 Cycle)
	fpu_reg14 = fpu_reg14 + fTextureUDivZ1NSBC;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg14 = fpu_reg14 + fTextureVDivZ1NSBC;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg14 = fTextureUDivZ2NSBC - fpu_reg14;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg14 = fTextureVDivZ2NSBC - fpu_reg14;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg14 = fpu_reg14 * fpu_reg12;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg12 = fpu_reg12 * fpu_reg14;
//st0 = fTextureUDivZdX
//st1 = fTextureVDivZdX
//st2 = 1/MaxXCounter
//st3 = 1/YCounter2, st4 = 1/YCounter1

	fTextureUDivZdX = fpu_reg13;
	fTextureVDivZdX = fpu_reg12;
//===>                                                                <===
//===>	TextureU&VDivZ DeltaX Calculation }                          <===
//===>                                                                <===


//===>                                                                <===
//===>	OneDivZ DeltaX Calculation {                          		 <===
//===>                                                                <===
//***> TODO: Optimize

	fpu_reg12 = fMaxXCounterRZP;
	fpu_reg12 = fpu_reg12 * fNum_2EXP20;
	fpu_reg13 = fYCounter2;
	fpu_reg13 = fpu_reg13 * fOneDivZdY1;
//st0 = OneDivZ1dY1*YCounter2
//st1 = 1/MaxXCounter
//st2 = 1/YCounter2, st3 = 1/YCounter1

	fpu_reg13 = fpu_reg13 + fOneDivZ1NSBC;
	fpu_reg13 = fOneDivZ2NSBC - fpu_reg13;
	fpu_reg12 = fpu_reg12 * fpu_reg13;
//st0 = fOneDivZdX
//st2 = 1/YCounter2, st3 = 1/YCounter1

	fOneDivZdX = fpu_reg12;
//===>                                                                <===
//===>	OneDivZ DeltaX Calculation }                          		 <===
//===>                                                                <===

//ffree	st(1)
//ffree	st(0)



	eax1 = dwDeltaScreenX1;
	ebx1 = dwDeltaScreenX2;
	esi1 = eax1;
	edi1 = ebx1;
	eax1 = eax1 << ( 12 );
	ebx1 = ebx1 << ( 12 );
	esi1 = ( (int32_t)esi1 ) >> ( 20 );
	edi1 = ( (int32_t)edi1 ) >> ( 20 );
	esi1 = esi1 + dwXmax;
	edi1 = edi1 + dwXmax;
	dwAdderScreenX1 = esi1;
	dwAdderScreenX2 = edi1;
	dwAdderScreenX1f = eax1;
	dwAdderScreenX2f = ebx1;

	eax1 = dwScreenX1;
	ebx1 = dwScreenX2;
	esi1 = eax1;
	edi1 = ebx1;
	eax1 = eax1 << ( 12 );
	ebx1 = ebx1 << ( 12 );
	esi1 = ( (int32_t)esi1 ) >> ( 20 );
	edi1 = ( (int32_t)edi1 ) >> ( 20 );
	esi1 = esi1 + dwYOffset1;
	edi1 = edi1 + dwYOffset1;

	ebp = dwYCounter;
	ecx1 = edi1;

//mov		dwOldESP,esp
//and		esp,0ffffffe0h
	SPTG = (CMV2ScanlinerPerspTG *) mem_alloc_endptr(dwYCounter * sizeof(CMV2ScanlinerPerspTG));
//mov		dwESPStartPoint,esp
	dwESPStartPoint = (void *)SPTG;

	fpu_reg10 = fNum_2EXP_20;
	fpu_reg11 = fNum_2EXP20;
	fpu_reg12 = fTextureVDivZ1;
	fpu_reg13 = fTextureUDivZ1;
	fpu_reg14 = fOneDivZ1;

MV2DrawPolygonTBASM_PerspPT3ScanlinerTGLoop:
// eax = x1f (20 bit fraction)
// ebx = x2f (20 bit fraction)
// ecx = XCounter (init. BackBuffer + YOffset + X2)
// edx =
// esi = BackBuffer + YOffset + X1
// edi = BackBuffer + YOffset + X2
// ebp = YCounter

// st0 = OneDivZ
// st1 = TextureUDivZ
// st2 = TextureVDivZ
// st3 = 2^20
// st4 = 2^-20

//***> TODO: Pipeline the fpu better
	eax1 = eax1 >> ( 12 );
//sub		esp,CMV2ScanlinerPerspTG__size
	SPTG--;

	dwScreenXf = eax1;
	ecx1 = ecx1 - esi1; // XCounter
	eax1 = eax1 << ( 12 );

	fpu_reg15 = ( (int32_t)dwScreenXf );
	fpu_reg15 = fpu_reg11 - fpu_reg15;
// st0 = 2^20 - ScreenXf, st1 = OneDivZ
// st2 = TextureUDivZ, st3 = TextureVDivZ,
// st4 = 2^20, st5 = 2^-20

	fpu_reg15 = fpu_reg15 * fpu_reg10;
//fst		dword ptr ss:[esp].CMV2ScanlinerPerspTG__fScreenXError
	SPTG->CMV2ScanlinerPerspTG__fScreenXError = fpu_reg15;
// st0 = ScreenXError

	fpu_reg16 = fTextureUDivZdX;
	fpu_reg16 = fpu_reg16 * fpu_reg15;
	fpu_reg17 = fTextureVDivZdX;
	fpu_reg17 = fpu_reg17 * fpu_reg15;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 * fOneDivZdX;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
// st0 = TextureVDivZdX*ScreenXError
// st1 = TextureUDivZdX*ScreenXError
// st2 = OneDivZdX*ScreenXError, st3 = OneDivZ
// st4 = TextureUDivZ, st5 = TextureVDivZ,
// st6 = 2^20, st7 = 2^-20

	fpu_reg17 = fpu_reg17 + fpu_reg12;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 + fpu_reg13;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 + fpu_reg14;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
// st0 = TextureUDivZ*ScreenXError + TextureUDivZ
// st1 = TextureVDivZ*ScreenXError + TextureVDivZ
// st2 = OneDivZ*ScreenXError + OneDivZ
// st3 = OneDivZ
// st4 = TextureUDivZ, st5 = TextureVDivZ,
// st6 = 2^20, st7 = 2^-20

//fstp	DWORD PTR ss:[esp].CMV2ScanlinerPerspTG__fTextureUDivZ
//fstp	DWORD PTR ss:[esp].CMV2ScanlinerPerspTG__fTextureVDivZ
//fstp	DWORD PTR ss:[esp].CMV2ScanlinerPerspTG__fOneDivZ
	SPTG->CMV2ScanlinerPerspTG__fTextureUDivZ = fpu_reg17;
	SPTG->CMV2ScanlinerPerspTG__fTextureVDivZ = fpu_reg16;
	SPTG->CMV2ScanlinerPerspTG__fOneDivZ = fpu_reg15;
// st0 = OneDivZ, st1 = TextureUDivZ, st2 = TextureVDivZ
// st3 = 2^20, st4 = 2^-20

//mov		ss:[esp].CMV2ScanlinerPerspTG__dwScreenX1Offset,esi
//mov		ss:[esp].CMV2ScanlinerPerspTG__dwXCounter,ecx
	SPTG->CMV2ScanlinerPerspTG__dwScreenX1Offset = esi1;
	SPTG->CMV2ScanlinerPerspTG__dwXCounter = ecx1;

	fpu_reg14 = fpu_reg14 + fOneDivZdY1;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg14 = fpu_reg14 + fTextureUDivZdY1;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg14 = fpu_reg14 + fTextureVDivZdY1;
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }

	{ uint32_t carry = (UINT32_MAX - ebx1 < dwAdderScreenX2f)?1:0; ebx1 = ebx1 + dwAdderScreenX2f;
	  edi1 = edi1 + dwAdderScreenX2 + carry; }
	{ uint32_t carry = (UINT32_MAX - eax1 < dwAdderScreenX1f)?1:0; eax1 = eax1 + dwAdderScreenX1f;
	  esi1 = esi1 + dwAdderScreenX1 + carry; }
	ecx1 = edi1;

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2DrawPolygonTBASM_PerspPT3ScanlinerTGLoop;
//fcompp
//fcomp




	edx4 = (CMV2ScanlinerPerspTG *)dwESPStartPoint;
	ebp = dwYCounter;

	fpu_reg12 = fB1;
	fpu_reg13 = fG1;
	fpu_reg14 = fR1;
MV2DrawPolygonTBASM_PerspPT3Gouraud:
// eax =
// ebx =
// ecx =
// edx =
// esi =
// edi =
// ebp = YCounter

// st0 = R
// st1 = G
// st2 = B
// st3 = 2^20
// st4 = 2^-20

//***> TODO: Pipeline the fpu better
	edx4--;

	fpu_reg15 = edx4->CMV2ScanlinerPerspTG__fScreenXError;
// st0 = ScreenXError, st0 = R, st1 = G, st2 = B
// st3 = 2^20, st4 = 2^-20

	fpu_reg16 = fGdX;
	fpu_reg16 = fpu_reg16 * fpu_reg15;
	fpu_reg17 = fRdX;
	fpu_reg17 = fpu_reg17 * fpu_reg15;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 * fBdX;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
// st0 = RdX*ScreenXError, st1 = GdX*ScreenXError
// st2 = BdX*ScreenXError, st3 = R, st4 = G, st5 = B
// st6 = 2^20, st7 = 2^-20

	fpu_reg17 = fpu_reg17 + fpu_reg14;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 + fpu_reg13;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 + fpu_reg12;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
// st0 = RdX*ScreenXError + R, st1 = GdX*ScreenXError + G
// st2 = BdX*ScreenXError + B, st3 = R, st4 = G, st5 = B
// st6 = 2^20, st7 = 2^-20

	edx4->CMV2ScanlinerPerspTG__dwR = (int32_t)ceilf(fpu_reg17);
	edx4->CMV2ScanlinerPerspTG__dwG = (int32_t)ceilf(fpu_reg16);
	edx4->CMV2ScanlinerPerspTG__dwB = (int32_t)ceilf(fpu_reg15);
// st0 = R, st1 = G, st2 = B, st3 = 2^20, st4 = 2^-20

	fpu_reg14 = fpu_reg14 + fRdY1;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg14 = fpu_reg14 + fGdY1;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg14 = fpu_reg14 + fBdY1;
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2DrawPolygonTBASM_PerspPT3Gouraud;

//ffree	st(4)
//ffree	st(3)
//ffree	st(2)
//ffree	st(1)
//ffree	st(0)






	goto MV2DrawPolygonTBASM_DoPerspTGYLoop;
MV2DrawPolygonTBASM_PolygonType4:
//=============>                <==============
//=============> Polygon Type 4 <==============
//=============>                <==============
// edx = pDot1
// esi = pDot2
// edi = pDot3

	eax1 = edi3->CMV2Dot3DPos__m_iScreenY;
	eax1 = eax1 - edx3->CMV2Dot3DPos__m_iScreenY;

	if (( (int32_t)eax1 ) <= 0) goto MV2DrawPolygonTBASM_PolygonNotVisible;

	dwYCounter = eax1;

	fpu_reg10 = edx3->CMV2Dot3DPos__m_fScreenY;
	fpu_reg11 = esi3->CMV2Dot3DPos__m_fScreenY;
	fpu_reg12 = edi3->CMV2Dot3DPos__m_fScreenY;
//st0 = (y3), st1 = (y2), st2 = (y1)

	fpu_reg13 = fpu_reg12;
	fpu_reg13 = fpu_reg13 - fpu_reg11;
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
//st0 = (y3), st1 = (y3 - y2), st2 = (y2), st3 = (y1)

	fpu_reg13 = fpu_reg13 - fpu_reg10;
	{ float tmp = fpu_reg11; fpu_reg11 = fpu_reg13; fpu_reg13 = tmp; }
//st0 = (y2), st1 = (y3 - y2), st2 = (y3 - y1), st3 = (y1)

	fpu_reg13 = fpu_reg13 - fpu_reg10;
	fYCounter = fpu_reg13; // Attention! YCounter = y2 - y1 is nearly 0
//st0 = (y3 - y2), st1 = (y3 - y1), st2 = (y1)

	fpu_reg13 = 1.0f;
	fpu_reg11 = fpu_reg13 / fpu_reg11;
//st0 = 1/(y3 - y2), st1 = 1/(y3 - y1)

	eax1 = dwXmax;
	ebx1 = edx3->CMV2Dot3DPos__m_iScreenY;
	ebx1 = ebx1 - 1;
	eax1 = ( (int32_t)eax1 ) * ( (int32_t)ebx1 );
	//eax1 = eax1 + pcBackBuffer;
	dwYOffset1 = eax1;

	fpu_reg13 = 1.0f;
	fpu_reg12 = fpu_reg13 / fpu_reg12;
	// fpu_reg10 = 0.0f;

//===>                                                                <===
//===>	Screen Delta Calculation {                                   <===
//===>                                                                <===
//st0 = 1/YCounter2, st1 = 1/YCounter1

	fpu_reg13 = edx3->CMV2Dot3DPos__m_fScreenX;
	fpu_reg14 = esi3->CMV2Dot3DPos__m_fScreenX;
	fpu_reg15 = edi3->CMV2Dot3DPos__m_fScreenX;
//st0 = (x3), st1 = (x2), st2 = (x1)
//st3 = 1/YCounter2, st4 = 1/YCounter1

	fpu_reg13 = fpu_reg15 - fpu_reg13;
	fpu_reg14 = fpu_reg15 - fpu_reg14;
	fpu_reg14 = fpu_reg14 * fpu_reg12;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg14 = fpu_reg14 * fpu_reg11; // fmul stall (+1 Cycle)
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg15 = esi3->CMV2Dot3DPos__m_fScreenYError;
	fpu_reg16 = edx3->CMV2Dot3DPos__m_fScreenYError;
//st0 = ScreenYError1, st1 = ScreenYError2
//st2 = DeltaScreenX2, st3 = DeltaScreenX1
//st4 = 1/YCounter2, st5 = 1/YCounter1

	fpu_reg16 = fpu_reg16 * fpu_reg13;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = fpu_reg16 * fpu_reg14;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = fpu_reg16 + edx3->CMV2Dot3DPos__m_fScreenX;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = fpu_reg16 + esi3->CMV2Dot3DPos__m_fScreenX;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
//st0 = ScreenYError1*DeltaScreenX1 + ScreenX1
//st1 = ScreenYError2*DeltaScreenX2 + ScreenX2
//st2 = DeltaScreenX2, st3 = DeltaScreenX1
//st4 = 1/YCounter2, st5 = 1/YCounter1

	dwScreenX1 = (int32_t)ceilf(fpu_reg16);
	dwScreenX2 = (int32_t)ceilf(fpu_reg15);
	dwDeltaScreenX2 = (int32_t)ceilf(fpu_reg14);
//st0 = DeltaScreenX1, st1 = 1/YCounter2, st2 = 1/YCounter1

//===>	MaxXCounter calculation	{        	                         <===

	fpu_reg14 = fYCounter;
	fpu_reg14 = fpu_reg14 * fpu_reg13;
//st0 = fYCounter*DeltaScreenX1, st1 = DeltaScreenX1
//st2 = 1/YCounter2, st3 = 1/YCounter1

	fpu_reg14 = fpu_reg14 + edx3->CMV2Dot3DPos__m_fScreenX;
	fpu_reg14 = esi3->CMV2Dot3DPos__m_fScreenX - fpu_reg14; //fp dep.
//st0 = ScreenX2 - (fYCounter12*DeltaScreenX1 + ScreenX1)
//st1 = DeltaScreenX1, st2 = 1/YCounter2, st3 = 1/YCounter1
	dwMaxXCounter = (int32_t)ceilf(fpu_reg14);
	fpu_reg15 = 1.0f;
	fpu_reg14 = fpu_reg15 / fpu_reg14;
	fMaxXCounterRZP = fpu_reg14; //fp dep.

//===>	MaxXCounter calculation	}        	                         <===

	dwDeltaScreenX1 = (int32_t)ceilf(fpu_reg13);
//st0 = 1/YCounter2, st1 = 1/YCounter1
//===>                                                                <===
//===>	Screen Delta Calculation }                                   <===
//===>                                                                <===

//===>                                                                <===
//===>	RGB DeltaY Calculation {   		                         	 <===
//===>                                                                <===
//st0 = 1/YCounter2, st1 = 1/YCounter1

	fpu_reg13 = edi3->CMV2Dot3DPos__m_fR;
	fpu_reg13 = fpu_reg13 - edx3->CMV2Dot3DPos__m_fR;
	fpu_reg14 = edi3->CMV2Dot3DPos__m_fG;
	fpu_reg14 = fpu_reg14 - edx3->CMV2Dot3DPos__m_fG;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg14 = fpu_reg14 * fpu_reg11;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg14 = fpu_reg14 * fpu_reg11; // fmul stall (+1 Cycle)
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg15 = edx3->CMV2Dot3DPos__m_fScreenYError;
	fpu_reg16 = fpu_reg15;
	fpu_reg16 = fpu_reg16 * fpu_reg14;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = fpu_reg16 * fpu_reg13; // fmul stall (+1 Cycle)
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
//st0 = ScreenYError1*RdY
//st1 = ScreenYError1*GdY
//st2 = RdY, st3 = GdY
//st4 = 1/YCounter2, st5 = 1/YCounter1

	fpu_reg16 = fpu_reg16 + edx3->CMV2Dot3DPos__m_fR;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = fpu_reg16 + edx3->CMV2Dot3DPos__m_fG;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }

	fR1 = fpu_reg16;
	fG1 = fpu_reg15;
	fRdY1 = fpu_reg14;
	fGdY1 = fpu_reg13;

	fpu_reg13 = edi3->CMV2Dot3DPos__m_fB;
	fpu_reg13 = fpu_reg13 - edx3->CMV2Dot3DPos__m_fB;
	fpu_reg13 = fpu_reg13 * fpu_reg11;
//st0 = BdY, st1 = 1/YCounter2, st2 = 1/YCounter1

	fpu_reg14 = edx3->CMV2Dot3DPos__m_fScreenYError;
	fpu_reg14 = fpu_reg14 * fpu_reg13;
	fpu_reg14 = fpu_reg14 + edx3->CMV2Dot3DPos__m_fB;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fBdY1 = fpu_reg14;
	fB1 = fpu_reg13;
//===>                                                                <===
//===>	RGB DeltaY Calculation }   		                         	 <===
//===>                                                                <===


//===>                                                                <===
//===>	RGB DeltaX Calculation {   		                         	 <===
//===>                                                                <===
	fpu_reg13 = fMaxXCounterRZP;
	fpu_reg13 = fpu_reg13 * fNum_2EXP20;
	fpu_reg14 = fYCounter;
	fpu_reg15 = fRdY1;
	fpu_reg16 = fGdY1;
	fpu_reg17 = fBdY1;
//st0 = fBdY1, st1 = fGdY1, st2 = fRdY1
//st3 = YCounter2, st4 = 1/MaxXCounter
//st5 = 1/YCounter2, st6 = 1/YCounter1

	fpu_reg17 = fpu_reg17 * fpu_reg14;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 * fpu_reg14;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 * fpu_reg14;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }

	fpu_reg17 = fpu_reg17 + edx3->CMV2Dot3DPos__m_fB;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 + edx3->CMV2Dot3DPos__m_fG;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 + edx3->CMV2Dot3DPos__m_fR;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }

	fpu_reg17 = esi3->CMV2Dot3DPos__m_fB - fpu_reg17;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = esi3->CMV2Dot3DPos__m_fG - fpu_reg17;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = esi3->CMV2Dot3DPos__m_fR - fpu_reg17;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }

	fpu_reg17 = fpu_reg17 * fpu_reg13;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 * fpu_reg13;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 * fpu_reg13;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }

	fBdX = fpu_reg17;
	dwBdX = (int32_t)ceilf(fpu_reg17);
	fGdX = fpu_reg16;
	dwGdX = (int32_t)ceilf(fpu_reg16);
	fRdX = fpu_reg15;
	dwRdX = (int32_t)ceilf(fpu_reg15);


//===>                                                                <===
//===>	RGB DeltaX Calculation }   		                         	 <===
//===>                                                                <===
	ebx1 = dwYCounter;

	if (ebx1 >= ( dwSpanLength )) goto MV2DrawPolygonTBASM_PerspPT4;

	eax1 = edx3->CMV2Dot3DPos__m_iScreenX;
	ebx1 = esi3->CMV2Dot3DPos__m_iScreenX;
	ebp = eax1;
	ecx1 = edi3->CMV2Dot3DPos__m_iScreenX;

	eax1 = eax1 - ebx1; // p1 - p2
	ebx1 = ebx1 - ecx1; // p2 - p3
	ecx1 = ecx1 - ebp; // p3 - p1


	if ( (( (int32_t)eax1 ) - ( 0 )) >= 0) goto MV2DrawPolygonTBASM_PerspCheck1PT4;
	eax1 = - ( (int32_t)eax1 );
MV2DrawPolygonTBASM_PerspCheck1PT4:

	if ( (( (int32_t)ebx1 ) - ( 0 )) >= 0) goto MV2DrawPolygonTBASM_PerspCheck2PT4;
	ebx1 = - ( (int32_t)ebx1 );
MV2DrawPolygonTBASM_PerspCheck2PT4:

	if ( (( (int32_t)ecx1 ) - ( 0 )) >= 0) goto MV2DrawPolygonTBASM_PerspCheck3PT4;
	ecx1 = - ( (int32_t)ecx1 );
MV2DrawPolygonTBASM_PerspCheck3PT4:


	if (eax1 >= ( dwSpanLength )) goto MV2DrawPolygonTBASM_PerspPT4;


	if (ebx1 >= ( dwSpanLength )) goto MV2DrawPolygonTBASM_PerspPT4;


	if (ecx1 >= ( dwSpanLength )) goto MV2DrawPolygonTBASM_PerspPT4;

	goto MV2DrawPolygonTBASM_LinearPolygonType4;
MV2DrawPolygonTBASM_PerspPT4:


	fOneDivZ1NSBC = edx3->CMV2Dot3DPos__m_fZNewRZP;
	fOneDivZ2NSBC = esi3->CMV2Dot3DPos__m_fZNewRZP;
	fOneDivZ3NSBC = edi3->CMV2Dot3DPos__m_fZNewRZP;

	fScreenYError1 = edx3->CMV2Dot3DPos__m_fScreenYError;
	fScreenYError2 = esi3->CMV2Dot3DPos__m_fScreenYError;

	edx2 = pDot1;
	esi2 = pDot2;
	edi2 = pDot3;



//===>                                                                <===
//===>	TextureU&VDivZ DeltaY Calculation {                          <===
//===>                                                                <===
//st0 = 1/YCounter2, st1 = 1/YCounter1


	fpu_reg13 = edx2->CMV2Dot3D__m_fTextureU;
	fpu_reg13 = fpu_reg13 * fOneDivZ1NSBC;
	fpu_reg14 = edi2->CMV2Dot3D__m_fTextureU;
	fpu_reg14 = fpu_reg14 * fOneDivZ3NSBC;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
//st0 = tu1/z1, st1 = tu3/z3
	fTextureUDivZ1NSBC = fpu_reg14;
	fpu_reg13 = fpu_reg13 - fpu_reg14;
//st0 = (tu3/z3 - tu1/z1)
//st1 = 1/YCounter2, st2 = 1/YCounter1

	fpu_reg14 = edx2->CMV2Dot3D__m_fTextureV;
	fpu_reg14 = fpu_reg14 * fOneDivZ1NSBC;
	fpu_reg15 = edi2->CMV2Dot3D__m_fTextureV;
	fpu_reg15 = fpu_reg15 * fOneDivZ3NSBC;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
//st0 = tv1/z1, st1 = tu3/z3
	fTextureVDivZ1NSBC = fpu_reg15;
	fpu_reg14 = fpu_reg14 - fpu_reg15;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
//st0 = (tu3/z3 - tu1/z1), st1 = (tv3/z3 - tv1/z1)
//st2 = 1/YCounter2, st3 = 1/YCounter1

	fpu_reg14 = fpu_reg14 * fpu_reg11;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg14 = fpu_reg14 * fpu_reg11; // fmul stall (+1 Cycle)
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
//st0 = TextureUDivZdY1, st1 = TextureVDivZdY1
//st2 = 1/YCounter2, st3 = 1/YCounter1

	fpu_reg15 = fScreenYError1;
	fpu_reg16 = fpu_reg15;
	fpu_reg16 = fpu_reg16 * fpu_reg14;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = fpu_reg16 * fpu_reg13; // fmul stall (+1 Cycle)
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = fpu_reg16 + fTextureUDivZ1NSBC;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = fpu_reg16 + fTextureVDivZ1NSBC;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
//st0 = ScreenYError1*TextureUDivZdY1 + TextureUDivZ
//st1 = ScreenYError1*TextureVDivZdY1 + TextureVDivZdY1
//st2 = DeltaTextureX1, st3 = DeltaTextureY1
//st4 = 1/YCounter2, st5 = 1/YCounter1

	fTextureUDivZ1 = fpu_reg16;
	fTextureVDivZ1 = fpu_reg15;
	fTextureUDivZdY1 = fpu_reg14;
	fTextureVDivZdY1 = fpu_reg13;
//st0 = 1/YCounter2, st1 = 1/YCounter1
//===>                                                                <===
//===>	TextureU&VDivZ DeltaY Calculation }                          <===
//===>                                                                <===

//===>                                                                <===
//===>	OneDivZ DeltaY Calculation {                            	 <===
//===>                                                                <===
//***> TODO: Optimize

	fpu_reg13 = fOneDivZ3NSBC;
	fpu_reg13 = fpu_reg13 - fOneDivZ1NSBC;
//st0 = (1/z3 - 1/z1)
//st1 = 1/YCounter2, st2 = 1/YCounter1

	fpu_reg13 = fpu_reg13 * fpu_reg11;
//st0 = OneDivZdY

	fpu_reg14 = fScreenYError1;
	fpu_reg14 = fpu_reg14 * fpu_reg13;
//st0 = ScreenYError1*OneDivZdY
//st1 = OneDivZdY
	fpu_reg14 = fpu_reg14 + fOneDivZ1NSBC;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
//st0 = OneDivZdY
//st1 = OneDivZ1

	fOneDivZdY1 = fpu_reg14;
	fOneDivZ1 = fpu_reg13;
//===>                                                                <===
//===>	OneDivZ DeltaY Calculation }                            	 <===
//===>                                                                <===



//===>                                                                <===
//===>	TextureU&VDivZ DeltaX Calculation {                          <===
//===>                                                                <===

	fpu_reg13 = fOneDivZ2NSBC;
	fpu_reg14 = esi2->CMV2Dot3D__m_fTextureU;
	fpu_reg14 = fpu_reg14 * fpu_reg13;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg14 = fpu_reg14 * esi2->CMV2Dot3D__m_fTextureV;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fTextureUDivZ2NSBC = fpu_reg14;
	fTextureVDivZ2NSBC = fpu_reg13;

	fpu_reg13 = fMaxXCounterRZP;
	fpu_reg13 = fpu_reg13 * fNum_2EXP20;
	fpu_reg14 = fYCounter;
	fpu_reg15 = fTextureVDivZdY1;
	fpu_reg16 = fTextureUDivZdY1;
//st0 = fTextureUDivZdY1, st1 = fTextureVDivZdY1
//st2 = YCounter, st3 = 1/MaxXCounter
//st4 = 1/YCounter2, st5 = 1/YCounter1

	fpu_reg16 = fpu_reg16 * fpu_reg14;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg14 = fpu_reg14 * fpu_reg16; // fmul stall (+ 1 Cycle)
	fpu_reg15 = fpu_reg15 + fTextureUDivZ1NSBC;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = fpu_reg15 + fTextureVDivZ1NSBC;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = fTextureUDivZ2NSBC - fpu_reg15;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = fTextureVDivZ2NSBC - fpu_reg15;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = fpu_reg15 * fpu_reg13;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg13 = fpu_reg13 * fpu_reg15;
//st0 = fTextureUDivZdX
//st1 = fTextureVDivZdX
//st2 = 1/YCounter2, st4 = 1/YCounter1

	fTextureUDivZdX = fpu_reg14;
	fTextureVDivZdX = fpu_reg13;
//===>                                                                <===
//===>	TextureU&VDivZ DeltaX Calculation }                          <===
//===>                                                                <===



//===>                                                                <===
//===>	OneDivZ DeltaX Calculation {                          		 <===
//===>                                                                <===
//***> TODO: Optimize

	fpu_reg13 = fMaxXCounterRZP;
	fpu_reg13 = fpu_reg13 * fNum_2EXP20;
	fpu_reg14 = fYCounter;
	fpu_reg14 = fpu_reg14 * fOneDivZdY1;
//st0 = OneDivZ1dY1*YCounter1
//st1 = 1/MaxXCounter
//st2 = 1/YCounter2, st3 = 1/YCounter1

	fpu_reg14 = fpu_reg14 + fOneDivZ1NSBC;
	fpu_reg14 = fOneDivZ2NSBC - fpu_reg14;
	fpu_reg13 = fpu_reg13 * fpu_reg14;
//st0 = fOneDivZdX
//st2 = 1/YCounter2, st3 = 1/YCounter1

	fOneDivZdX = fpu_reg13;
//===>                                                                <===
//===>	OneDivZ DeltaX Calculation }                          		 <===
//===>                                                                <===

//ffree	st(1)
//ffree	st(0)




	eax1 = dwDeltaScreenX1;
	ebx1 = dwDeltaScreenX2;
	esi1 = eax1;
	edi1 = ebx1;
	eax1 = eax1 << ( 12 );
	ebx1 = ebx1 << ( 12 );
	esi1 = ( (int32_t)esi1 ) >> ( 20 );
	edi1 = ( (int32_t)edi1 ) >> ( 20 );
	esi1 = esi1 + dwXmax;
	edi1 = edi1 + dwXmax;
	dwAdderScreenX1 = esi1;
	dwAdderScreenX2 = edi1;
	dwAdderScreenX1f = eax1;
	dwAdderScreenX2f = ebx1;

	eax1 = dwScreenX1;
	ebx1 = dwScreenX2;
	esi1 = eax1;
	edi1 = ebx1;
	eax1 = eax1 << ( 12 );
	ebx1 = ebx1 << ( 12 );
	esi1 = ( (int32_t)esi1 ) >> ( 20 );
	edi1 = ( (int32_t)edi1 ) >> ( 20 );
	esi1 = esi1 + dwYOffset1;
	edi1 = edi1 + dwYOffset1;

	ebp = dwYCounter;
	ecx1 = edi1;

//mov		dwOldESP,esp
//and		esp,0ffffffe0h
	SPTG = (CMV2ScanlinerPerspTG *) mem_alloc_endptr(dwYCounter * sizeof(CMV2ScanlinerPerspTG));
//mov		dwESPStartPoint,esp
	dwESPStartPoint = (void *)SPTG;

	fpu_reg10 = fNum_2EXP_20;
	fpu_reg11 = fNum_2EXP20;
	fpu_reg12 = fTextureVDivZ1;
	fpu_reg13 = fTextureUDivZ1;
	fpu_reg14 = fOneDivZ1;

MV2DrawPolygonTBASM_PerspPT4ScanlinerTGLoop:
// eax = x1f (20 bit fraction)
// ebx = x2f (20 bit fraction)
// ecx = XCounter (init. BackBuffer + YOffset + X2)
// edx =
// esi = BackBuffer + YOffset + X1
// edi = BackBuffer + YOffset + X2
// ebp = YCounter

// st0 = OneDivZ
// st1 = TextureUDivZ
// st2 = TextureVDivZ
// st3 = 2^20
// st4 = 2^-20

//***> TODO: Pipeline the fpu better
	eax1 = eax1 >> ( 12 );
//sub		esp,CMV2ScanlinerPerspTG__size
	SPTG--;

	dwScreenXf = eax1;
	ecx1 = ecx1 - esi1; // XCounter
	eax1 = eax1 << ( 12 );

	fpu_reg15 = ( (int32_t)dwScreenXf );
	fpu_reg15 = fpu_reg11 - fpu_reg15;
// st0 = 2^20 - ScreenXf, st1 = OneDivZ
// st2 = TextureUDivZ, st3 = TextureVDivZ,
// st4 = 2^20, st5 = 2^-20

	fpu_reg15 = fpu_reg15 * fpu_reg10;
//fst		dword ptr ss:[esp].CMV2ScanlinerPerspTG__fScreenXError
	SPTG->CMV2ScanlinerPerspTG__fScreenXError = fpu_reg15;
// st0 = ScreenXError

	fpu_reg16 = fTextureUDivZdX;
	fpu_reg16 = fpu_reg16 * fpu_reg15;
	fpu_reg17 = fTextureVDivZdX;
	fpu_reg17 = fpu_reg17 * fpu_reg15;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 * fOneDivZdX;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
// st0 = TextureVDivZdX*ScreenXError
// st1 = TextureUDivZdX*ScreenXError
// st2 = OneDivZdX*ScreenXError, st3 = OneDivZ
// st4 = TextureUDivZ, st5 = TextureVDivZ,
// st6 = 2^20, st7 = 2^-20

	fpu_reg17 = fpu_reg17 + fpu_reg12;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 + fpu_reg13;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 + fpu_reg14;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
// st0 = TextureUDivZ*ScreenXError + TextureUDivZ
// st1 = TextureVDivZ*ScreenXError + TextureVDivZ
// st2 = OneDivZ*ScreenXError + OneDivZ
// st3 = OneDivZ
// st4 = TextureUDivZ, st5 = TextureVDivZ,
// st6 = 2^20, st7 = 2^-20

//fstp	DWORD PTR ss:[esp].CMV2ScanlinerPerspTG__fTextureUDivZ
//fstp	DWORD PTR ss:[esp].CMV2ScanlinerPerspTG__fTextureVDivZ
//fstp	DWORD PTR ss:[esp].CMV2ScanlinerPerspTG__fOneDivZ
	SPTG->CMV2ScanlinerPerspTG__fTextureUDivZ = fpu_reg17;
	SPTG->CMV2ScanlinerPerspTG__fTextureVDivZ = fpu_reg16;
	SPTG->CMV2ScanlinerPerspTG__fOneDivZ = fpu_reg15;
// st0 = OneDivZ, st1 = TextureUDivZ, st2 = TextureVDivZ
// st3 = 2^20, st4 = 2^-20

//mov		ss:[esp].CMV2ScanlinerPerspTG__dwScreenX1Offset,esi
//mov		ss:[esp].CMV2ScanlinerPerspTG__dwXCounter,ecx
	SPTG->CMV2ScanlinerPerspTG__dwScreenX1Offset = esi1;
	SPTG->CMV2ScanlinerPerspTG__dwXCounter = ecx1;

	fpu_reg14 = fpu_reg14 + fOneDivZdY1;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg14 = fpu_reg14 + fTextureUDivZdY1;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg14 = fpu_reg14 + fTextureVDivZdY1;
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }

	{ uint32_t carry = (UINT32_MAX - ebx1 < dwAdderScreenX2f)?1:0; ebx1 = ebx1 + dwAdderScreenX2f;
	  edi1 = edi1 + dwAdderScreenX2 + carry; }
	{ uint32_t carry = (UINT32_MAX - eax1 < dwAdderScreenX1f)?1:0; eax1 = eax1 + dwAdderScreenX1f;
	  esi1 = esi1 + dwAdderScreenX1 + carry; }
	ecx1 = edi1;

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2DrawPolygonTBASM_PerspPT4ScanlinerTGLoop;
//fcompp
//fcomp




	edx4 = (CMV2ScanlinerPerspTG *)dwESPStartPoint;
	ebp = dwYCounter;

	fpu_reg12 = fB1;
	fpu_reg13 = fG1;
	fpu_reg14 = fR1;
MV2DrawPolygonTBASM_PerspPT4Gouraud:
// eax =
// ebx =
// ecx =
// edx =
// esi =
// edi =
// ebp = YCounter

// st0 = R
// st1 = G
// st2 = B
// st3 = 2^20
// st4 = 2^-20

//***> TODO: Pipeline the fpu better
	edx4--;

	fpu_reg15 = edx4->CMV2ScanlinerPerspTG__fScreenXError;
// st0 = ScreenXError, st0 = R, st1 = G, st2 = B
// st3 = 2^20, st4 = 2^-20

	fpu_reg16 = fGdX;
	fpu_reg16 = fpu_reg16 * fpu_reg15;
	fpu_reg17 = fRdX;
	fpu_reg17 = fpu_reg17 * fpu_reg15;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 * fBdX;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
// st0 = RdX*ScreenXError, st1 = GdX*ScreenXError
// st2 = BdX*ScreenXError, st3 = R, st4 = G, st5 = B
// st6 = 2^20, st7 = 2^-20

	fpu_reg17 = fpu_reg17 + fpu_reg14;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 + fpu_reg13;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 + fpu_reg12;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
// st0 = RdX*ScreenXError + R, st1 = GdX*ScreenXError + G
// st2 = BdX*ScreenXError + B, st3 = R, st4 = G, st5 = B
// st6 = 2^20, st7 = 2^-20

	edx4->CMV2ScanlinerPerspTG__dwR = (int32_t)ceilf(fpu_reg17);
	edx4->CMV2ScanlinerPerspTG__dwG = (int32_t)ceilf(fpu_reg16);
	edx4->CMV2ScanlinerPerspTG__dwB = (int32_t)ceilf(fpu_reg15);
// st0 = R, st1 = G, st2 = B, st3 = 2^20, st4 = 2^-20

	fpu_reg14 = fpu_reg14 + fRdY1;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg14 = fpu_reg14 + fGdY1;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg14 = fpu_reg14 + fBdY1;
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2DrawPolygonTBASM_PerspPT4Gouraud;

//ffree	st(4)
//ffree	st(3)
//ffree	st(2)
//ffree	st(1)
//ffree	st(0)






	goto MV2DrawPolygonTBASM_DoPerspTGYLoop;
MV2DrawPolygonTBASM_DoPerspTGYLoop:
//================> 								<=====================
//================> 								<=====================
//================> 	Perspective					<=====================
//================>  Texture Gouraud InnerLoop	<=====================
//================> 								<=====================
//================> 								<=====================
// eax = 			|	dgf	|	dbf
// ebx = rf			|	gf	|	bf
// ecx = XCounter	|	dg	|	db
// edx =		|	r	|	g	|	b
// esi = 	|	dr
// edi = dest
// ebp = drf

	edx1 = dwRdX;
	ecx1 = edx1;
	edx1 = edx1 << ( 8 );
	ecx1 = ecx1 << ( 24 );
	edx1 = edx1 & ( 0x000ff0000 );
	dwRGBIntLoop_ebp = ecx1;
	dwRGBIntLoop_esi1 = edx1;

	ecx1 = dwGdX;
	edx1 = dwBdX;
	eax1 = 0;
	ebx1 = 0;
	eax1 = (eax1 & 0xffffff00) | (uint8_t)(( (uint8_t)edx1 ));
	ebx1 = (ebx1 & 0xffffff00) | (uint8_t)(( (uint8_t)(edx1 >> 8) ));
	eax1 = set_high_byte(eax1, ( (uint8_t)ecx1 ));
	ebx1 = set_high_byte(ebx1, ( (uint8_t)(ecx1 >> 8) ));
	dwRGBIntLoop_eax1 = eax1;
	dwRGBIntLoop_ecx1 = ebx1;


MV2DrawPolygonTBASM_PerspTGYLoop:
//mov		ecx,ss:[esp].CMV2ScanlinerPerspTG__dwXCounter
	ecx1 = SPTG->CMV2ScanlinerPerspTG__dwXCounter;
//	mov		edi,ss:[esp].CMV2ScanlinerPerspTG__dwScreenX1Offset
//	mov		edi,offset pBackTextureBuffer
	edi1 = 0;

	iPixelCounter = iPixelCounter + ecx1; //***********

// ecx = XCounter

	if (( (int32_t)ecx1 ) <= 0) goto MV2DrawPolygonTBASM_PerspTGNoXLoop; //***********
//	jz		@@PerspTGNoXLoop

	eax1 = ecx1;
	dwScanlineXCounter = ecx1;


	if (( (int32_t)ecx1 ) <= ( dwSpanLength )) goto MV2DrawPolygonTBASM_PerspTGFirstRemnantSpan;
	ecx1 = ( dwSpanLength );
MV2DrawPolygonTBASM_PerspTGFirstRemnantSpan:

	eax1 = eax1 - ( dwSpanLength );
	dwNextXCounter = ecx1;

//fld		ss:[esp].CMV2ScanlinerPerspTG__fOneDivZ
//fld		ss:[esp].CMV2ScanlinerPerspTG__fTextureVDivZ
//fld		ss:[esp].CMV2ScanlinerPerspTG__fTextureUDivZ
	fpu_reg10 = SPTG->CMV2ScanlinerPerspTG__fOneDivZ;
	fpu_reg11 = SPTG->CMV2ScanlinerPerspTG__fTextureVDivZ;
	fpu_reg12 = SPTG->CMV2ScanlinerPerspTG__fTextureUDivZ;
//st0 = UL/ZL, st1 = VL/ZL, st2 = 1/ZL

	fpu_reg13 = 1.0f;
	fpu_reg13 = fpu_reg13 / fpu_reg10;
//st0 = ZL, st1 = UL/ZL, st2 = VL/ZL, st3 = 1/ZL

	fpu_reg14 = fpu_reg13;
	fpu_reg14 = fpu_reg14 * fpu_reg12;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg14 = fpu_reg14 * fpu_reg11;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
//st0 = UL, st1 = VL, st2 = UL/ZL, st3 = VL/ZL
//st4 = 1/ZL

	fpu_reg15 = ( (int32_t)dwNextXCounter );
//st0 = NextXCounter, st1 = UL, st2 = VL, st3 = UL/ZL
//st4 = VL/ZL, st5 = 1/ZL
	fpu_reg16 = fpu_reg15;
	fpu_reg16 = fpu_reg16 * fTextureUDivZdX;
	fpu_reg17 = fpu_reg15;
	fpu_reg17 = fpu_reg17 * fTextureVDivZdX;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
//st0 = NextXCounter*fTextureUDivZ, st1 = NextXCounter*fTextureVDivZ
//st2 = dwNextXCounter, st3 = UL, st4 = VL, st5 = UL/ZL
//st6 = VL/ZL, st7 = 1/ZL
	fpu_reg12 = fpu_reg12 + fpu_reg17;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = fpu_reg16 * fOneDivZdX;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg11 = fpu_reg11 + fpu_reg16;
	fpu_reg10 = fpu_reg10 + fpu_reg15;
//st0 = UL, st1 = VL, st2 = UR/ZR, st3 = VR/ZR, st4 = 1/ZR

	dwTextureU = (int32_t)ceilf(fpu_reg14);
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	dwTextureV = (int32_t)ceilf(fpu_reg14);
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }

	fpu_reg15 = 1.0f;
	fpu_reg15 = fpu_reg15 / fpu_reg10;
//st0 = ZR, st1 = UL, st2 = VL, st3 = UR/ZR, st4 = VR/ZR
//st5 = 1/ZR

	fpu_reg16 = fpu_reg15;
//st0 = ZR, st1 = ZR, st2 = UL, st3 = VL
//st4 = UR/ZR, st5 = VR/ZR, st6 = 1/ZR

	fpu_reg16 = fpu_reg16 * fpu_reg12;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = fpu_reg16 * fpu_reg11;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
//st0 = UR, st1 = VR, st2 = UL, st3 = VL
//st4 = UR/ZR, st5 = VR/ZR, st6 = 1/ZR

	fpu_reg14 = fpu_reg16 - fpu_reg14;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg13 = fpu_reg16 - fpu_reg13;
//st0 = VR, st1 = UR, st2 = UR - UL, st3 = VR - VL
//st4 = UR/ZR, st5 = VR/ZR, st6 = 1/ZR

	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg16 = fpu_reg16 * ( fSpanLengthFactorsRZP[ecx1] );
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg16 = fpu_reg16 * ( fSpanLengthFactorsRZP[ecx1] );
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }

//st0 = (VR - VL)/SpanLength, st1 = (UR - UL)/SpanLength
//st2 = UR, st3 = VR, st4 = UR/ZR, st5 = VR/ZR, st6 = 1/ZR

	dwTextureVdX = (int32_t)ceilf(fpu_reg16);
	dwTextureUdX = (int32_t)ceilf(fpu_reg15);
//st0 = UR, st1 = VR, st2 = UR/ZR, st3 = VR/ZR, st4 = 1/ZR
//right becomes left, left will be calculated in folowing lines..
//st0 = UL, st1 = VL, st2 = UL/ZL, st3 = VL/ZL, st4 = 1/ZL

//ecx = dwNextXCounter ( = CurXCounter )



	if (( (int32_t)eax1 ) <= 0) goto MV2DrawPolygonTBASM_PerspTGDoRemnantPixel;
	ecx1 = eax1;


	if (( (int32_t)ecx1 ) <= ( dwSpanLength )) goto MV2DrawPolygonTBASM_PerspTGSecondRemnantSpan;
	ecx1 = ( dwSpanLength );
MV2DrawPolygonTBASM_PerspTGSecondRemnantSpan:

	ebx1 = dwNextXCounter;
	eax1 = eax1 - ( dwSpanLength );
	dwCurXCounter = ebx1;
	dwNextXCounter = ecx1;
	dwScanlineXCounter = eax1;

//st0 = UL, st1 = VL, st2 = UL/ZL, st3 = VL/ZL, st4 = 1/ZL
//***> TODO: Optimize
	fpu_reg15 = ( (int32_t)dwNextXCounter );
//st0 = NextXCounter, st1 = UL, st2 = VL, st3 = UL/ZL
//st4 = VL/ZL, st5 = 1/ZL
	fpu_reg16 = fpu_reg15;
	fpu_reg16 = fpu_reg16 * fTextureUDivZdX;
	fpu_reg17 = fpu_reg15;
	fpu_reg17 = fpu_reg17 * fTextureVDivZdX;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
//st0 = NextXCounter*fTextureUDivZ, st1 = NextXCounter*fTextureVDivZ
//st2 = dwNextXCounter, st3 = UL, st4 = VL, st5 = UL/ZL
//st6 = VL/ZL, st7 = 1/ZL
	fpu_reg12 = fpu_reg12 + fpu_reg17;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = fpu_reg16 * fOneDivZdX;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg11 = fpu_reg11 + fpu_reg16;
	fpu_reg10 = fpu_reg10 + fpu_reg15;
//st0 = UL, st1 = VL, st2 = UR/ZR, st3 = VR/ZR, st4 = 1/ZR

MV2DrawPolygonTBASM_PerspTGSpanLoop:
//st0 = UL, st1 = VL, st2 = UR/ZR, st3 = VR/ZR, st4 = 1/ZR
	fpu_reg15 = 1.0f;
	fpu_reg15 = fpu_reg15 / fpu_reg10;

	ebx1 = dwTextureUdX;
	eax1 = dwTextureVdX;
	ebp = ebx1;
	esi1 = eax1;
	ebx1 = ebx1 >> ( 16 ); // bl init.
	eax1 = eax1 >> ( 16 );
	esi1 = esi1 << ( 16 ); // esi init.
	ebp = ebp << ( 16 ); // ebp init.
	ebx1 = set_high_byte(ebx1, ( (uint8_t)eax1 )); // bh init.
	eax1 = dwTextureU;
	ecx1 = dwTextureV;
	edx1 = eax1;
	eax1 = eax1 << ( 16 );
	edx1 = edx1 >> ( 16 ); // dl init.
	ebx1 = ebx1 | eax1; // ebx init.
	eax1 = ecx1;
	ecx1 = ecx1 << ( 16 ); // hi ecx init.
	eax1 = eax1 >> ( 16 );
	ecx1 = ecx1 | dwCurXCounter; // ecx init.
	edx1 = set_high_byte(edx1, ( (uint8_t)eax1 )); // dh init.
	//edx1 = edx1 | pcTexture; // edx init.

MV2DrawPolygonTBASM_PerspTGXLoop:
// eax = col
// ebx = txf			| dty 	| dtX
// ecx = tyf			|		| XCounter
// edx = pTexture	| ty	| tx
// esi = dtyf
// edi =	destination
// ebp = dtxf

	eax1 = edx1;
	edx1 = edx1 & ( 0x0ffff );
	//edx1 = edx1 + pcBumpmap;
	pBackBumpBuffer[edi1] = edx1; // ->pBackBumpBuffer
	edx1 = eax1;

	eax1 = pcTexture[edx1];
	pBackTextureBuffer[edi1] = eax1;

	{ uint32_t carry = (UINT32_MAX - ebx1 < ebp)?1:0; ebx1 = ebx1 + ebp;
	  edx1 = (edx1 & 0xffffff00) | (uint8_t)(( (uint8_t)edx1 ) + ( (uint8_t)ebx1 ) + carry); }
	{ uint32_t carry = (UINT32_MAX - ecx1 < esi1)?1:0; ecx1 = ecx1 + esi1;
	  edx1 = set_high_byte(edx1, ( (uint8_t)(edx1 >> 8) ) + ( (uint8_t)(ebx1 >> 8) ) + carry); }
	edi1++;

	ecx1 = (ecx1 & 0xffffff00) | (uint8_t)(( (int8_t)ecx1 ) - 1);
	if (( (int8_t)ecx1 ) != 0) goto MV2DrawPolygonTBASM_PerspTGXLoop;

//st0 = ZR, st1 = UL, st2 = VL, st3 = UR/ZR
//st4 = VR/ZR, st5 = 1/ZR

//***> TODO: Optimize
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg15; fpu_reg15 = tmp; }
	dwTextureV = (int32_t)ceilf(fpu_reg15);
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	dwTextureU = (int32_t)ceilf(fpu_reg15);
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg15; fpu_reg15 = tmp; }

	fpu_reg16 = fpu_reg15;
//st0 = ZR, st1 = ZR, st2 = UL, st3 = VL
//st4 = UR/ZR, st5 = VR/ZR, st6 = 1/ZR

	fpu_reg16 = fpu_reg16 * fpu_reg12;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = fpu_reg16 * fpu_reg11;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
//st0 = UR, st1 = VR, st2 = UL, st3 = VL
//st4 = UR/ZR, st5 = VR/ZR, st6 = 1/ZR

	fpu_reg14 = fpu_reg16 - fpu_reg14;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg13 = fpu_reg16 - fpu_reg13;
//st0 = VR, st1 = UR, st2 = UR - UL, st3 = VR - VL
//st4 = UR/ZR, st5 = VR/ZR, st6 = 1/ZR

	eax1 = dwNextXCounter;
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg16 = fpu_reg16 * ( fSpanLengthFactorsRZP[eax1] );
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg16 = fpu_reg16 * ( fSpanLengthFactorsRZP[eax1] );
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
//st0 = (VR - VL)/SpanLength, st1 = (UR - UL)/SpanLength
//st2 = UR, st3 = VR, st4 = UR/ZR, st5 = VR/ZR, st6 = 1/ZR

	dwTextureVdX = (int32_t)ceilf(fpu_reg16);
	dwTextureUdX = (int32_t)ceilf(fpu_reg15);
//st0 = UR, st1 = VR, st2 = UR/ZR, st3 = VR/ZR, st4 = 1/ZR
//right becomes left, left will be calculated in folowing lines..

//st0 = UL, st1 = VL, st2 = UL/ZL, st3 = VL/ZL, st4 = 1/ZL


	if (( (int32_t)dwScanlineXCounter ) <= ( 0 )) goto MV2DrawPolygonTBASM_PerspTGDoRemnantPixel;

	ecx1 = dwScanlineXCounter;
	eax1 = ecx1;

	if (( (int32_t)ecx1 ) <= ( dwSpanLength )) goto MV2DrawPolygonTBASM_PerspTGRemnantSpanFollows;
	ecx1 = ( dwSpanLength );
MV2DrawPolygonTBASM_PerspTGRemnantSpanFollows:

	ebx1 = dwNextXCounter;
	eax1 = eax1 - ( dwSpanLength );

	dwCurXCounter = ebx1;
	dwNextXCounter = ecx1;
	dwScanlineXCounter = eax1;


//***> TODO: Optimize
	fpu_reg15 = ( (int32_t)dwNextXCounter );
//st0 = NextXCounter, st1 = UL, st2 = VL, st3 = UL/ZL
//st4 = VL/ZL, st5 = 1/ZL
	fpu_reg16 = fpu_reg15;
	fpu_reg16 = fpu_reg16 * fTextureUDivZdX;
	fpu_reg17 = fpu_reg15;
	fpu_reg17 = fpu_reg17 * fTextureVDivZdX;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
//st0 = NextXCounter*fTextureUDivZ, st1 = NextXCounter*fTextureVDivZ
//st2 = dwNextXCounter, st3 = UL, st4 = VL, st5 = UL/ZL
//st6 = VL/ZL, st7 = 1/ZL
	fpu_reg12 = fpu_reg12 + fpu_reg17;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = fpu_reg16 * fOneDivZdX;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg11 = fpu_reg11 + fpu_reg16;
	fpu_reg10 = fpu_reg10 + fpu_reg15;
//st0 = UL, st1 = VL, st2 = UR/ZR, st3 = VR/ZR, st4 = 1/ZR

	goto MV2DrawPolygonTBASM_PerspTGSpanLoop;
MV2DrawPolygonTBASM_PerspTGDoRemnantPixel:
	ebx1 = dwTextureUdX;
	eax1 = dwTextureVdX;
	ebp = ebx1;
	esi1 = eax1;
	ebx1 = ebx1 >> ( 16 ); // bl init.
	eax1 = eax1 >> ( 16 );
	esi1 = esi1 << ( 16 ); // esi init.
	ebp = ebp << ( 16 ); // ebp init.
	ebx1 = set_high_byte(ebx1, ( (uint8_t)eax1 )); // bh init.
	eax1 = dwTextureU;
	ecx1 = dwTextureV;
	edx1 = eax1;
	eax1 = eax1 << ( 16 );
	edx1 = edx1 >> ( 16 ); // dl init.
	ebx1 = ebx1 | eax1; // ebx init.
	eax1 = ecx1;
	ecx1 = ecx1 << ( 16 ); // hi ecx init.
	eax1 = eax1 >> ( 16 );
	ecx1 = ecx1 | dwNextXCounter; // ecx init.
	edx1 = set_high_byte(edx1, ( (uint8_t)eax1 )); // dh init.
	//edx1 = edx1 | pcTexture; // edx init.

MV2DrawPolygonTBASM_PerspTGRemnantPixelXLoop:
// eax = col
// ebx = txf			| dty 	| dtX
// ecx = tyf			|		| XCounter
// edx = pTexture	| ty	| tx
// esi = dtyf
// edi =	destination
// ebp = dtxf

	eax1 = edx1;
	edx1 = edx1 & ( 0x0ffff );
	//edx1 = edx1 + pcBumpmap;
	pBackBumpBuffer[edi1] = edx1; // ->pBackBumpBuffer
	edx1 = eax1;

	eax1 = pcTexture[edx1];
	pBackTextureBuffer[edi1] = eax1;

	{ uint32_t carry = (UINT32_MAX - ebx1 < ebp)?1:0; ebx1 = ebx1 + ebp;
	  edx1 = (edx1 & 0xffffff00) | (uint8_t)(( (uint8_t)edx1 ) + ( (uint8_t)ebx1 ) + carry); }
	{ uint32_t carry = (UINT32_MAX - ecx1 < esi1)?1:0; ecx1 = ecx1 + esi1;
	  edx1 = set_high_byte(edx1, ( (uint8_t)(edx1 >> 8) ) + ( (uint8_t)(ebx1 >> 8) ) + carry); }
	edi1++;

	ecx1 = (ecx1 & 0xffffff00) | (uint8_t)(( (int8_t)ecx1 ) - 1);
	if (( (int8_t)ecx1 ) != 0) goto MV2DrawPolygonTBASM_PerspTGRemnantPixelXLoop;


//mov		ebp,ss:[esp].CMV2ScanlinerPerspTG__dwR
//mov		edx,ss:[esp].CMV2ScanlinerPerspTG__dwG
//mov     esi,ss:[esp].CMV2ScanlinerPerspTG__dwB
//mov		ebx,ss:[esp].CMV2ScanlinerPerspTG__dwXCounter
	ebp = SPTG->CMV2ScanlinerPerspTG__dwR;
	edx1 = SPTG->CMV2ScanlinerPerspTG__dwG;
	esi1 = SPTG->CMV2ScanlinerPerspTG__dwB;
	ebx1 = SPTG->CMV2ScanlinerPerspTG__dwXCounter;
	edi1 = 0;

MV2DrawPolygonTBASM_PerspBumpLoop:
// eax =
// ebx = XCounter
// ecx =
// edx =					|UAngle	|UAnglef
// esi = 				|VAngle	|VAnglef
// edi = Bump-,ShadeBuffer	(Loaded with BumbBuffer)
// ebp = 				|NAngle |NAnglef
	stack_var00 = ebx1;

	ebx1 = 0;

	if (ebp > ( 0x0ffff )) goto MV2DrawPolygonTBASM_PerspAmbient;

	eax1 = pBackBumpBuffer[edi1];
	ecx1 = 0;
	ebx1 = set_high_byte(ebx1, ( (uint8_t)(ebx1 >> 8) ) ^ ( ((uint8_t *)&(pcBumpmap[eax1]))[0] )); //UBump
	ecx1 = set_high_byte(ecx1, ( ((uint8_t *)&(pcBumpmap[eax1]))[1] )); //VBump
	ebx1 = ebx1 - ( 128 * 256 );
	ecx1 = ecx1 - ( 128 * 256 );
	eax1 = 0;
	ebx1 = ebx1 + edx1; //UAngle + UBump
	ecx1 = ecx1 + esi1; //VAngle + VBump


	if (ebx1 > ( 0x0ffff )) goto MV2DrawPolygonTBASM_PerspAmbientBump;

	if (ecx1 > ( 0x0ffff )) goto MV2DrawPolygonTBASM_PerspAmbientBump;
	eax1 = (eax1 & 0xffffff00) | (uint8_t)(( (uint8_t)(ebx1 >> 8) ));
	eax1 = set_high_byte(eax1, ( (uint8_t)(ecx1 >> 8) ));
	ebx1 = 0;
	ebx1 = (ebx1 & 0xffffff00) | (uint8_t)(( pAngleJoinTab[eax1] )); //pAngleJoin
	goto MV2DrawPolygonTBASM_PerspAmbient;
MV2DrawPolygonTBASM_PerspAmbientBump:
	ebx1 = 0;
MV2DrawPolygonTBASM_PerspAmbient:
	ebx1 = ( pLightBrightnessTab[ebx1] ); //Helligkeit
	pBackShadeBuffer[edi1] = ebx1; //ShadeBuffer

	edi1++;
	ebx1 = stack_var00;
	edx1 = edx1 + dwGdX;
	esi1 = esi1 + dwBdX;
	ebp = ebp + dwRdX;

	ebx1 = ( (int32_t)ebx1 ) - 1;
	if (( (int32_t)ebx1 ) != 0) goto MV2DrawPolygonTBASM_PerspBumpLoop;



#if 0
//	mov		edx,ss:[esp].CMV2ScanlinerPerspTG__dwR
//	mov     ecx,ss:[esp].CMV2ScanlinerPerspTG__dwG
//	mov		eax,ss:[esp].CMV2ScanlinerPerspTG__dwB
//	mov		ebx,edx
//	shl		ebx,24
//	mov		bl,al
//	mov		bh,cl					; ebx init.
//
//	shl		edx,8
//	mov		dl,ah
//	mov		dh,ch					; edx init.
//
//	mov		ecx,ss:[esp].CMV2ScanlinerPerspTG__dwXCounter
//	shl		ecx,16
//	or		ecx,dwRGBIntLoop_ecx    ; ecx init.
//
//	mov     esi,dwRGBIntLoop_esi	; esi init.
//	mov		ebp,dwRGBIntLoop_ebp	; ebp init.
//	mov		eax,dwRGBIntLoop_eax	; eax init.
//
//	mov		edi,offset pBackShadeBuffer
//
//	sub		ecx,010000h
//		; eax = 			|	dgf	|	dbf
//		; ebx = rf			|	gf	|	bf
//		; ecx = XCounter	|	dg	|	db
//		; edx =		|	r	|	g	|	b
//		; esi = 	|	dr
//		; edi = dest
//		; ebp = drf
//@@PerspRGBXLoop:
//;	push	edx
//;	and		edx,0c0c0c0h
//	mov		ds:[edi],edx		; 1
//;	pop		edx
//
//	add		ebx,ebp
//	jnc		@@PerspRGBXLoopNC   ; 1
//	add		edx,10000h			; 1
//@@PerspRGBXLoopNC:
//	add		edx,esi				; 1
//
//	add		bl,al
//	adc		dl,cl				; 1
//	add		bh,ah
//	adc		dh,ch				; 1
//
//	add		edi,4
//
//	sub		ecx,10000h			; 1
//	jns		@@PerspRGBXLoop		;---
//								; 7 cycles
#endif

//mov		edi,ss:[esp].CMV2ScanlinerPerspTG__dwScreenX1Offset
//mov		ebp,ss:[esp].CMV2ScanlinerPerspTG__dwXCounter
	edi1 = SPTG->CMV2ScanlinerPerspTG__dwScreenX1Offset;
	ebp = SPTG->CMV2ScanlinerPerspTG__dwXCounter;
	esi1 = 0;
	eax1 = 0;
	ebx1 = 0;
	ecx1 = 0;
	edx1 = 0;
	edi1 = edi1 - 1;

MV2DrawPolygonTBASM_PerspShadeLoop:
// eax = r
// ebx = shaded rgb
// ecx = g
// edx = b
// esi = offset
// edi = dest
// ebp = XCounter

	eax1 = pBackShadeBuffer[esi1];
	ebx1 = pBackTextureBuffer[esi1];

	eax1 = eax1 & ( 0x0ffffff );
	ebx1 = ebx1 & ( 0x0ffffff );
	edx1 = set_high_byte(edx1, ( (uint8_t)eax1 ));
	ecx1 = set_high_byte(ecx1, ( (uint8_t)(eax1 >> 8) ));
	edx1 = (edx1 & 0xffffff00) | (uint8_t)(( (uint8_t)ebx1 ));
	ecx1 = (ecx1 & 0xffffff00) | (uint8_t)(( (uint8_t)(ebx1 >> 8) ));
	eax1 = eax1 >> ( 8 );
	esi1++;
	ebx1 = ebx1 >> ( 16 );
	edi1 = edi1 + 1;
	eax1 = (eax1 & 0xffffff00) | (uint8_t)(( (uint8_t)ebx1 ));

	ebx1 = (ebx1 & 0xffffff00) | (uint8_t)(( pBumpShadeLookup[ecx1] )); // g
	ebx1 = set_high_byte(ebx1, ( pBumpShadeLookup[eax1] )); // r
	ebx1 = ebx1 << ( 8 );
	ebx1 = (ebx1 & 0xffffff00) | (uint8_t)(( pBumpShadeLookup[edx1] )); // b

#if 0
//	ror		eax,8
//	ror		ebx,8
//	not		eax
//	sub		bh,ah
//	jnc		@@pok1
//	xor		bh,bh
//@@pok1:
//	rol		eax,8
//	rol		ebx,8
//
//	sub		bh,ah
//	jnc		@@pok2
//	xor		bh,bh
//@@pok2:
//	sub		bl,al
//	jnc		@@pok3
//	xor		bl,bl
//@@pok3:
//	inc		edi
//	add		esi,4
#endif
	pcBackBuffer[edi1] = ebx1;

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2DrawPolygonTBASM_PerspShadeLoop;

//ffree	st(4)
//ffree	st(3)
//ffree	st(2)
//ffree	st(1)
//ffree	st(0)





MV2DrawPolygonTBASM_PerspTGNoXLoop:

//add		esp,CMV2ScanlinerPerspTG__size
	SPTG++;

	dwYCounter = ( (int32_t)dwYCounter ) - 1;
	if (( (int32_t)dwYCounter ) != 0) goto MV2DrawPolygonTBASM_PerspTGYLoop;

//mov		esp,dwOldESP

MV2DrawPolygonTBASM_PolygonNotVisible:

	goto MV2DrawPolygonTBASM_Done;
MV2DrawPolygonTBASM_LinearPolygonType1:
//======================> 				<==============================
//======================> 				<==============================
//======================> 				<==============================
//======================> 				<==============================
//======================> 				<==============================
//======================> 				<==============================
//======================> 				<==============================
//======================> 				<==============================
//======================> Linear Mapping <==============================
//======================> 				<==============================
//======================> 				<==============================
//======================> 				<==============================
//======================> 				<==============================
//======================> 				<==============================
//======================> 				<==============================
//======================> 				<==============================
//======================> 				<==============================

//=============>                <==============
//=============> Linear		   <==============
//=============> Polygon Type 1 <==============
//=============>                <==============
	fScreenYError1 = edx3->CMV2Dot3DPos__m_fScreenYError;
	fScreenYError3 = edi3->CMV2Dot3DPos__m_fScreenYError;

	edx2 = pDot1;
	esi2 = pDot2;
	edi2 = pDot3;

//===>                                                                <===
//===>	TextureU DeltaY Calculation {                            	 <===
//===>                                                                <===
	fpu_reg14 = edx2->CMV2Dot3D__m_fTextureU;
	fpu_reg15 = esi2->CMV2Dot3D__m_fTextureU;
	fpu_reg16 = edi2->CMV2Dot3D__m_fTextureU;
	fpu_reg17 = fpu_reg15;
	fpu_reg17 = fpu_reg17 - fpu_reg16;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 - fpu_reg14;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 * fpu_reg12;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 - fpu_reg14;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 * fpu_reg13;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg17; fpu_reg17 = tmp; }
//ffree   st
//fincstp

	fpu_reg16 = fpu_reg16 * fpu_reg11;
//st0 = TextureUdY, st1 = TextureUdY2,
//st2 = TextureUdY1, st3 = 1/YCounter1,
//st4 = 1/YCounter2, st5 = 1/YCounter

	fpu_reg17 = fScreenYError1;
	fpu_reg18 = fScreenYError3;
	fpu_reg18 = fpu_reg18 * fpu_reg15;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg18 = fpu_reg18 * fpu_reg14;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg18 = fpu_reg18 + edi2->CMV2Dot3D__m_fTextureU;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg18 = fpu_reg18 + edx2->CMV2Dot3D__m_fTextureU;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }

	fTextureU3 = fpu_reg18;
	fTextureU1 = fpu_reg17;
	fTextureUdY = fpu_reg16;
	fTextureUdY2 = fpu_reg15;
	fTextureUdY1 = fpu_reg14;

//===>                                                                <===
//===>	TextureU DeltaY Calculation }                            	 <===
//===>                                                                <===


//===>                                                                <===
//===>	TextureV DeltaY Calculation {                            	 <===
//===>                                                                <===
	fpu_reg14 = edx2->CMV2Dot3D__m_fTextureV;
	fpu_reg15 = esi2->CMV2Dot3D__m_fTextureV;
	fpu_reg16 = edi2->CMV2Dot3D__m_fTextureV;
	fpu_reg17 = fpu_reg15;
	fpu_reg17 = fpu_reg17 - fpu_reg16;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 - fpu_reg14;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 * fpu_reg12;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 - fpu_reg14;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 * fpu_reg13;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg17; fpu_reg17 = tmp; }
//ffree   st
//fincstp

	fpu_reg16 = fpu_reg16 * fpu_reg11;
//st0 = TextureVdY, st1 = TextureVdY2,
//st2 = TextureVdY1, st3 = 1/YCounter1,
//st4 = 1/YCounter2, st5 = 1/YCounter

	fpu_reg17 = fScreenYError1;
	fpu_reg18 = fScreenYError3;
	fpu_reg18 = fpu_reg18 * fpu_reg15;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg18 = fpu_reg18 * fpu_reg14;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg18 = fpu_reg18 + edi2->CMV2Dot3D__m_fTextureV;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg18 = fpu_reg18 + edx2->CMV2Dot3D__m_fTextureV;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }

	fTextureV3 = fpu_reg18;
	fTextureV1 = fpu_reg17;
	fTextureVdY = fpu_reg16;
	fTextureVdY2 = fpu_reg15;
	fTextureVdY1 = fpu_reg14;
//===>                                                                <===
//===>	TextureV DeltaY Calculation }                            	 <===
//===>                                                                <===


//===>                                                                <===
//===>	TextureUV DeltaX Calculation {                            	 <===
//===>                                                                <===
	fpu_reg14 = fMaxXCounterRZP;
	fpu_reg14 = fpu_reg14 * fNum_2EXP20;
	fpu_reg15 = fYCounter1;
	fpu_reg16 = fTextureVdY;
	fpu_reg17 = fTextureUdY;
	fpu_reg17 = fpu_reg17 * fpu_reg15;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg15 = fpu_reg15 * fpu_reg17; // fmul stall (+ 1 Cycle)
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = fpu_reg16 + edx2->CMV2Dot3D__m_fTextureV;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = fpu_reg16 + edx2->CMV2Dot3D__m_fTextureU;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = fpu_reg16 - edi2->CMV2Dot3D__m_fTextureV;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = fpu_reg16 - edi2->CMV2Dot3D__m_fTextureU;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = fpu_reg16 * fpu_reg14;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = fpu_reg16 * fpu_reg14;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }

	fTextureVdX = fpu_reg16;
	dwTextureVdX = (int32_t)ceilf(fpu_reg16);
	fTextureUdX = fpu_reg15;
	dwTextureUdX = (int32_t)ceilf(fpu_reg15);
//===>                                                                <===
//===>	TextureUV DeltaX Calculation }                            	 <===
//===>                                                                <===

//ffree	st(3)
//ffree	st(2)
//ffree	st(1)
//ffree	st(0)






	eax1 = dwDeltaScreenX1;
	ebx1 = dwDeltaScreenX;
	esi1 = eax1;
	edi1 = ebx1;
	eax1 = eax1 << ( 12 );
	ebx1 = ebx1 << ( 12 );
	esi1 = ( (int32_t)esi1 ) >> ( 20 );
	edi1 = ( (int32_t)edi1 ) >> ( 20 );
	esi1 = esi1 + dwXmax;
	edi1 = edi1 + dwXmax;
	dwAdderScreenX1 = esi1;
	dwAdderScreenX2 = edi1;
	dwAdderScreenX1f = eax1;
	dwAdderScreenX2f = ebx1;

	eax1 = dwScreenX1;
	ebx1 = dwScreenX2;
	esi1 = eax1;
	edi1 = ebx1;
	eax1 = eax1 << ( 12 );
	ebx1 = ebx1 << ( 12 );
	esi1 = ( (int32_t)esi1 ) >> ( 20 );
	edi1 = ( (int32_t)edi1 ) >> ( 20 );
	esi1 = esi1 + dwYOffset1;
	edi1 = edi1 + dwYOffset1;

	ebp = dwYCounter1;
	ecx1 = edi1;

//mov		dwOldESP,esp
//and		esp,0ffffffe0h
	SLTG = (CMV2ScanlinerLinTG *) mem_alloc_endptr(dwYCounter * sizeof(CMV2ScanlinerLinTG));
//mov		dwESPStartPoint,esp
	dwESPStartPoint = (void *)SLTG;

	fpu_reg10 = fNum_2EXP_20;
	fpu_reg11 = fNum_2EXP20;
	fpu_reg12 = fTextureVdX;
	fpu_reg13 = fTextureUdX;
	fpu_reg14 = fTextureV1;
	fpu_reg15 = fTextureU1;

MV2DrawPolygonTBASM_LinPT1TexturePass1:
// eax = x1f (20 bit fraction)
// ebx = x2f (20 bit fraction)
// ecx = XCounter (init. BackBuffer + YOffset + X2)
// edx =
// esi = BackBuffer + YOffset + X1
// edi = BackBuffer + YOffset + X2
// ebp = YCounter

// st0 = TextureU
// st1 = TextureV
// st2 = TextureUdX
// st3 = TextureVdX
// st4 = 2^20
// st5 = 2^-20

//***> TODO: Pipeline the fpu better
	eax1 = eax1 >> ( 12 );
//sub		esp,CMV2ScanlinerLinTG__size
	SLTG--;

	dwScreenXf = eax1;
	ecx1 = ecx1 - esi1; // XCounter
	eax1 = eax1 << ( 12 );

	fpu_reg16 = ( (int32_t)dwScreenXf );
	fpu_reg16 = fpu_reg11 - fpu_reg16;
// st0 = 2^20 - ScreenXf, st1 = TextureU
// st2 = TextureV, st3 = TextureUdX,
// st4 = TextureVdX, st5 = 2^20, st6 = 2^-20

	fpu_reg16 = fpu_reg16 * fpu_reg10;
// st0 = ScreenXError
//fst		dword ptr ss:[esp].CMV2ScanlinerLinTG__fScreenXError
	SLTG->CMV2ScanlinerLinTG__fScreenXError = fpu_reg16;

	fpu_reg17 = fpu_reg13;
	fpu_reg17 = fpu_reg17 * fpu_reg16;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 * fpu_reg12;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
// st0 = TextureUdX*ScreenXError
// st1 = TextureVdX*ScreenXError
// st2 = TextureU, st3 = TextureV
// st4 = TextureUdX, st5 = TextureVdX
// st6 = 2^20, st7 = 2^-20


	fpu_reg17 = fpu_reg17 + fpu_reg15;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 + fpu_reg14;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
// st0 = TextureUdX*ScreenXError + TextureU
// st1 = TextureVdX*ScreenXError + TextureV
// st2 = TextureU, st3 = TextureV
// st4 = TextureUdX, st5 = TextureVdX
// st6 = 2^20, st7 = 2^-20

//fistp	DWORD PTR ss:[esp].CMV2ScanlinerLinTG__dwTextureU
//fistp	DWORD PTR ss:[esp].CMV2ScanlinerLinTG__dwTextureV
	SLTG->CMV2ScanlinerLinTG__dwTextureU = (int32_t)ceilf(fpu_reg17);
	SLTG->CMV2ScanlinerLinTG__dwTextureV = (int32_t)ceilf(fpu_reg16);

//mov		ss:[esp].CMV2ScanlinerLinTG__dwScreenX1Offset,esi
//mov		ss:[esp].CMV2ScanlinerLinTG__dwXCounter,ecx
	SLTG->CMV2ScanlinerLinTG__dwScreenX1Offset = esi1;
	SLTG->CMV2ScanlinerLinTG__dwXCounter = ecx1;
// st0 = TextureU, st1 = TextureV
// st2 = TextureUdX, st3 = TextureVdX
// st4 = 2^20, st5 = 2^-20

	fpu_reg15 = fpu_reg15 + fTextureUdY1;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = fpu_reg15 + fTextureVdY1;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }

	{ uint32_t carry = (UINT32_MAX - ebx1 < dwAdderScreenX2f)?1:0; ebx1 = ebx1 + dwAdderScreenX2f;
	  edi1 = edi1 + dwAdderScreenX2 + carry; }
	{ uint32_t carry = (UINT32_MAX - eax1 < dwAdderScreenX1f)?1:0; eax1 = eax1 + dwAdderScreenX1f;
	  esi1 = esi1 + dwAdderScreenX1 + carry; }
	ecx1 = edi1;

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2DrawPolygonTBASM_LinPT1TexturePass1;



	eax1 = dwDeltaScreenX2;
	esi1 = eax1;
	eax1 = eax1 << ( 12 );
	esi1 = ( (int32_t)esi1 ) >> ( 20 );
	esi1 = esi1 + dwXmax;
	dwAdderScreenX1 = esi1;
	dwAdderScreenX1f = eax1;

	eax1 = dwScreenX3;
	esi1 = eax1;
	eax1 = eax1 << ( 12 );
	esi1 = ( (int32_t)esi1 ) >> ( 20 );
	esi1 = esi1 + dwYOffset2;

	ebp = dwYCounter2;
	ecx1 = edi1;

	fpu_reg12 = fTextureVdX;
	fpu_reg13 = fTextureUdX;
	fpu_reg14 = fTextureV3;
	fpu_reg15 = fTextureU3;
MV2DrawPolygonTBASM_LinPT1TexturePass2:
// eax = x1f (20 bit fraction)
// ebx = x2f (20 bit fraction)
// ecx = XCounter (init. BackBuffer + YOffset + X2)
// edx =
// esi = BackBuffer + YOffset + X1
// edi = BackBuffer + YOffset + X2
// ebp = YCounter

// st0 = TextureU
// st1 = TextureV
// st2 = TextureUdX
// st3 = TextureVdX
// st4 = 2^20
// st5 = 2^-20

//***> TODO: Pipeline the fpu better
	eax1 = eax1 >> ( 12 );
//sub		esp,CMV2ScanlinerLinTG__size
	SLTG--;

	dwScreenXf = eax1;
	ecx1 = ecx1 - esi1; // XCounter
	eax1 = eax1 << ( 12 );

	fpu_reg16 = ( (int32_t)dwScreenXf );
	fpu_reg16 = fpu_reg11 - fpu_reg16;
// st0 = 2^20 - ScreenXf, st1 = TextureU
// st2 = TextureV, st3 = TextureUdX,
// st4 = TextureVdX, st5 = 2^20, st6 = 2^-20

	fpu_reg16 = fpu_reg16 * fpu_reg10;
// st0 = ScreenXError
//fst		dword ptr ss:[esp].CMV2ScanlinerLinTG__fScreenXError
	SLTG->CMV2ScanlinerLinTG__fScreenXError = fpu_reg16;

	fpu_reg17 = fpu_reg13;
	fpu_reg17 = fpu_reg17 * fpu_reg16;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 * fpu_reg12;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
// st0 = TextureUdX*ScreenXError
// st1 = TextureVdX*ScreenXError
// st2 = TextureU, st3 = TextureV
// st4 = TextureUdX, st5 = TextureVdX
// st6 = 2^20, st7 = 2^-20


	fpu_reg17 = fpu_reg17 + fpu_reg15;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 + fpu_reg14;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
// st0 = TextureUdX*ScreenXError + TextureU
// st1 = TextureVdX*ScreenXError + TextureV
// st2 = TextureU, st3 = TextureV
// st4 = TextureUdX, st5 = TextureVdX
// st6 = 2^20, st7 = 2^-20

//fistp	DWORD PTR ss:[esp].CMV2ScanlinerLinTG__dwTextureU
//fistp	DWORD PTR ss:[esp].CMV2ScanlinerLinTG__dwTextureV
	SLTG->CMV2ScanlinerLinTG__dwTextureU = (int32_t)ceilf(fpu_reg17);
	SLTG->CMV2ScanlinerLinTG__dwTextureV = (int32_t)ceilf(fpu_reg16);

//mov		ss:[esp].CMV2ScanlinerLinTG__dwScreenX1Offset,esi
//mov		ss:[esp].CMV2ScanlinerLinTG__dwXCounter,ecx
	SLTG->CMV2ScanlinerLinTG__dwScreenX1Offset = esi1;
	SLTG->CMV2ScanlinerLinTG__dwXCounter = ecx1;
// st0 = TextureU, st1 = TextureV
// st2 = TextureUdX, st3 = TextureVdX
// st4 = 2^20, st5 = 2^-20

	fpu_reg15 = fpu_reg15 + fTextureUdY2;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = fpu_reg15 + fTextureVdY2;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }

	{ uint32_t carry = (UINT32_MAX - ebx1 < dwAdderScreenX2f)?1:0; ebx1 = ebx1 + dwAdderScreenX2f;
	  edi1 = edi1 + dwAdderScreenX2 + carry; }
	{ uint32_t carry = (UINT32_MAX - eax1 < dwAdderScreenX1f)?1:0; eax1 = eax1 + dwAdderScreenX1f;
	  esi1 = esi1 + dwAdderScreenX1 + carry; }
	ecx1 = edi1;

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2DrawPolygonTBASM_LinPT1TexturePass2;



	edx5 = (CMV2ScanlinerLinTG *)dwESPStartPoint;
	ebp = dwYCounter1;

	fpu_reg12 = fB1;
	fpu_reg13 = fG1;
	fpu_reg14 = fR1;
MV2DrawPolygonTBASM_LinPT1GouraudPass1:
// eax =
// ebx =
// ecx =
// edx =
// esi =
// edi =
// ebp = YCounter

// st0 = R
// st1 = G
// st2 = B
// st3 = 2^20
// st4 = 2^-20

//***> TODO: Pipeline the fpu better
	edx5--;

	fpu_reg15 = edx5->CMV2ScanlinerLinTG__fScreenXError;
// st0 = ScreenXError, st0 = R, st1 = G, st2 = B
// st3 = 2^20, st4 = 2^-20

	fpu_reg16 = fGdX;
	fpu_reg16 = fpu_reg16 * fpu_reg15;
	fpu_reg17 = fRdX;
	fpu_reg17 = fpu_reg17 * fpu_reg15;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 * fBdX;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
// st0 = RdX*ScreenXError, st1 = GdX*ScreenXError
// st2 = BdX*ScreenXError, st3 = R, st4 = G, st5 = B
// st6 = 2^20, st7 = 2^-20

	fpu_reg17 = fpu_reg17 + fpu_reg14;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 + fpu_reg13;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 + fpu_reg12;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
// st0 = RdX*ScreenXError + R, st1 = GdX*ScreenXError + G
// st2 = BdX*ScreenXError + B, st3 = R, st4 = G, st5 = B
// st6 = 2^20, st7 = 2^-20

	edx5->CMV2ScanlinerLinTG__dwR = (int32_t)ceilf(fpu_reg17);
	edx5->CMV2ScanlinerLinTG__dwG = (int32_t)ceilf(fpu_reg16);
	edx5->CMV2ScanlinerLinTG__dwB = (int32_t)ceilf(fpu_reg15);
// st0 = R, st1 = G, st2 = B, st3 = 2^20, st4 = 2^-20

	fpu_reg14 = fpu_reg14 + fRdY1;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg14 = fpu_reg14 + fGdY1;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg14 = fpu_reg14 + fBdY1;
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2DrawPolygonTBASM_LinPT1GouraudPass1;
//fcompp
//fcomp




	ebp = dwYCounter2;
	fpu_reg12 = fB3;
	fpu_reg13 = fG3;
	fpu_reg14 = fR3;
MV2DrawPolygonTBASM_LinPT1GouraudPass2:
// eax =
// ebx =
// ecx =
// edx =
// esi =
// edi =
// ebp = YCounter

// st0 = R
// st1 = G
// st2 = B
// st3 = 2^20
// st4 = 2^-20

//***> TODO: Pipeline the fpu better
	edx5--;

	fpu_reg15 = edx5->CMV2ScanlinerLinTG__fScreenXError;
// st0 = ScreenXError, st0 = R, st1 = G, st2 = B
// st3 = 2^20, st4 = 2^-20

	fpu_reg16 = fGdX;
	fpu_reg16 = fpu_reg16 * fpu_reg15;
	fpu_reg17 = fRdX;
	fpu_reg17 = fpu_reg17 * fpu_reg15;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 * fBdX;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
// st0 = RdX*ScreenXError, st1 = GdX*ScreenXError
// st2 = BdX*ScreenXError, st3 = R, st4 = G, st5 = B
// st6 = 2^20, st7 = 2^-20

	fpu_reg17 = fpu_reg17 + fpu_reg14;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 + fpu_reg13;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 + fpu_reg12;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
// st0 = RdX*ScreenXError + R, st1 = GdX*ScreenXError + G
// st2 = BdX*ScreenXError + B, st3 = R, st4 = G, st5 = B
// st6 = 2^20, st7 = 2^-20

	edx5->CMV2ScanlinerLinTG__dwR = (int32_t)ceilf(fpu_reg17);
	edx5->CMV2ScanlinerLinTG__dwG = (int32_t)ceilf(fpu_reg16);
	edx5->CMV2ScanlinerLinTG__dwB = (int32_t)ceilf(fpu_reg15);
// st0 = R, st1 = G, st2 = B, st3 = 2^20, st4 = 2^-20

	fpu_reg14 = fpu_reg14 + fRdY2;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg14 = fpu_reg14 + fGdY2;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg14 = fpu_reg14 + fBdY2;
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2DrawPolygonTBASM_LinPT1GouraudPass2;

//ffree	st(4)
//ffree	st(3)
//ffree	st(2)
//ffree	st(1)
//ffree	st(0)






	goto MV2DrawPolygonTBASM_DoLinTGYLoop;
MV2DrawPolygonTBASM_LinearPolygonType2:
//=============>                <==============
//=============> Linear		   <==============
//=============> Polygon Type 2 <==============
//=============>                <==============
//st0 = 1/YCounter, st1 = 1/YCounter2, st2 = 1/YCounter1
	fScreenYError1 = edx3->CMV2Dot3DPos__m_fScreenYError;
	fScreenYError2 = esi3->CMV2Dot3DPos__m_fScreenYError;

	edx2 = pDot1;
	esi2 = pDot2;
	edi2 = pDot3;

//===>                                                                <===
//===>	TextureUV DeltaY Calculation {                            	 <===
//===>                                                                <===
	fpu_reg14 = edi2->CMV2Dot3D__m_fTextureU;
	fpu_reg14 = fpu_reg14 - edx2->CMV2Dot3D__m_fTextureU;
	fpu_reg15 = edi2->CMV2Dot3D__m_fTextureV;
	fpu_reg15 = fpu_reg15 - edx2->CMV2Dot3D__m_fTextureV;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = fpu_reg15 * fpu_reg13;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = fpu_reg15 * fpu_reg13; // fmul stall (+1 Cycle)
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg16 = fScreenYError1;
	fpu_reg17 = fpu_reg16;
	fpu_reg17 = fpu_reg17 * fpu_reg15;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 * fpu_reg14; // fmul stall (+1 Cycle)
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
//st0 = ScreenYError1*DeltaTextureX
//st1 = ScreenYError1*DeltaTextureY
//st2 = DeltaTextureX, st3 = DeltaTextureY
//st4 = 1/YCounter, st5 = 1/YCounter2, st6 = 1/YCounter1

	fpu_reg17 = fpu_reg17 + edx2->CMV2Dot3D__m_fTextureU;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 + edx2->CMV2Dot3D__m_fTextureV;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }

	fTextureU1 = fpu_reg17;
	fTextureV1 = fpu_reg16;
	fTextureUdY = fpu_reg15;
	fTextureVdY = fpu_reg14;
//===>                                                                <===
//===>	TextureUV DeltaY Calculation }                            	 <===
//===>                                                                <===


//===>                                                                <===
//===>	TextureUV DeltaX Calculation {                            	 <===
//===>                                                                <===
	fpu_reg14 = fMaxXCounterRZP;
	fpu_reg14 = fpu_reg14 * fNum_2EXP20;
	fpu_reg15 = fYCounter1;
	fpu_reg16 = fTextureVdY;
	fpu_reg17 = fTextureUdY;
//st0 = DeltaTextureX1, st1 = DeltaTextureY1
//st2 = YCounter1, st3 = 1/MaxXCounter
//st4 = 1/YCounter, st5 = 1/YCounter2, st6 = 1/YCounter1

	fpu_reg17 = fpu_reg17 * fpu_reg15;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg15 = fpu_reg15 * fpu_reg17; // fmul stall (+ 1 Cycle)
	fpu_reg16 = fpu_reg16 + edx2->CMV2Dot3D__m_fTextureU;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = fpu_reg16 + edx2->CMV2Dot3D__m_fTextureV;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = esi2->CMV2Dot3D__m_fTextureU - fpu_reg16;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = esi2->CMV2Dot3D__m_fTextureV - fpu_reg16;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = fpu_reg16 * fpu_reg14;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = fpu_reg16 * fpu_reg14;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
//st0 = TextureUdX
//st1 = TextureVdX
//st2 = 1/MaxXCounter
//st3 = 1/YCounter, st4 = 1/YCounter2, st5 = 1/YCounter1

	fTextureUdX = fpu_reg16;
	dwTextureUdX = (int32_t)ceilf(fpu_reg16);
	fTextureVdX = fpu_reg15;
	dwTextureVdX = (int32_t)ceilf(fpu_reg15);

//===>                                                                <===
//===>	TextureUV DeltaX Calculation }                            	 <===
//===>                                                                <===
//ffree	st(3)
//ffree	st(2)
//ffree	st(1)
//ffree	st(0)






	eax1 = dwDeltaScreenX;
	ebx1 = dwDeltaScreenX1;
	esi1 = eax1;
	edi1 = ebx1;
	eax1 = eax1 << ( 12 );
	ebx1 = ebx1 << ( 12 );
	esi1 = ( (int32_t)esi1 ) >> ( 20 );
	edi1 = ( (int32_t)edi1 ) >> ( 20 );
	esi1 = esi1 + dwXmax;
	edi1 = edi1 + dwXmax;
	dwAdderScreenX1 = esi1;
	dwAdderScreenX2 = edi1;
	dwAdderScreenX1f = eax1;
	dwAdderScreenX2f = ebx1;

	eax1 = dwScreenX1;
	ebx1 = dwScreenX2;
	esi1 = eax1;
	edi1 = ebx1;
	eax1 = eax1 << ( 12 );
	ebx1 = ebx1 << ( 12 );
	esi1 = ( (int32_t)esi1 ) >> ( 20 );
	edi1 = ( (int32_t)edi1 ) >> ( 20 );
	esi1 = esi1 + dwYOffset1;
	edi1 = edi1 + dwYOffset1;

	ebp = dwYCounter1;
	ecx1 = edi1;

//mov		dwOldESP,esp
//and		esp,0ffffffe0h
	SLTG = (CMV2ScanlinerLinTG *) mem_alloc_endptr(dwYCounter * sizeof(CMV2ScanlinerLinTG));
//mov		dwESPStartPoint,esp
	dwESPStartPoint = (void *)SLTG;

	fpu_reg10 = fNum_2EXP_20;
	fpu_reg11 = fNum_2EXP20;
	fpu_reg12 = fTextureVdX;
	fpu_reg13 = fTextureUdX;
	fpu_reg14 = fTextureV1;
	fpu_reg15 = fTextureU1;

MV2DrawPolygonTBASM_LinPT2TexturePass1:
// eax = x1f (20 bit fraction)
// ebx = x2f (20 bit fraction)
// ecx = XCounter (init. BackBuffer + YOffset + X2)
// edx =
// esi = BackBuffer + YOffset + X1
// edi = BackBuffer + YOffset + X2
// ebp = YCounter

// st0 = TextureU
// st1 = TextureV
// st2 = TextureUdX
// st3 = TextureVdX
// st4 = 2^20
// st5 = 2^-20

//***> TODO: Pipeline the fpu better
	eax1 = eax1 >> ( 12 );
//sub		esp,CMV2ScanlinerLinTG__size
	SLTG--;

	dwScreenXf = eax1;
	ecx1 = ecx1 - esi1; // XCounter
	eax1 = eax1 << ( 12 );

	fpu_reg16 = ( (int32_t)dwScreenXf );
	fpu_reg16 = fpu_reg11 - fpu_reg16;
// st0 = 2^20 - ScreenXf, st1 = TextureU
// st2 = TextureV, st3 = TextureUdX,
// st4 = TextureVdX, st5 = 2^20, st6 = 2^-20

	fpu_reg16 = fpu_reg16 * fpu_reg10;
// st0 = ScreenXError
//fst		dword ptr ss:[esp].CMV2ScanlinerLinTG__fScreenXError
	SLTG->CMV2ScanlinerLinTG__fScreenXError = fpu_reg16;

	fpu_reg17 = fpu_reg13;
	fpu_reg17 = fpu_reg17 * fpu_reg16;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 * fpu_reg12;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
// st0 = TextureUdX*ScreenXError
// st1 = TextureVdX*ScreenXError
// st2 = TextureU, st3 = TextureV
// st4 = TextureUdX, st5 = TextureVdX
// st6 = 2^20, st7 = 2^-20


	fpu_reg17 = fpu_reg17 + fpu_reg15;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 + fpu_reg14;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
// st0 = TextureUdX*ScreenXError + TextureU
// st1 = TextureVdX*ScreenXError + TextureV
// st2 = TextureU, st3 = TextureV
// st4 = TextureUdX, st5 = TextureVdX
// st6 = 2^20, st7 = 2^-20

//fistp	DWORD PTR ss:[esp].CMV2ScanlinerLinTG__dwTextureU
//fistp	DWORD PTR ss:[esp].CMV2ScanlinerLinTG__dwTextureV
	SLTG->CMV2ScanlinerLinTG__dwTextureU = (int32_t)ceilf(fpu_reg17);
	SLTG->CMV2ScanlinerLinTG__dwTextureV = (int32_t)ceilf(fpu_reg16);

//mov		ss:[esp].CMV2ScanlinerLinTG__dwScreenX1Offset,esi
//mov		ss:[esp].CMV2ScanlinerLinTG__dwXCounter,ecx
	SLTG->CMV2ScanlinerLinTG__dwScreenX1Offset = esi1;
	SLTG->CMV2ScanlinerLinTG__dwXCounter = ecx1;
// st0 = TextureU, st1 = TextureV
// st2 = TextureUdX, st3 = TextureVdX
// st4 = 2^20, st5 = 2^-20

	fpu_reg15 = fpu_reg15 + fTextureUdY;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = fpu_reg15 + fTextureVdY;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }

	{ uint32_t carry = (UINT32_MAX - ebx1 < dwAdderScreenX2f)?1:0; ebx1 = ebx1 + dwAdderScreenX2f;
	  edi1 = edi1 + dwAdderScreenX2 + carry; }
	{ uint32_t carry = (UINT32_MAX - eax1 < dwAdderScreenX1f)?1:0; eax1 = eax1 + dwAdderScreenX1f;
	  esi1 = esi1 + dwAdderScreenX1 + carry; }
	ecx1 = edi1;

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2DrawPolygonTBASM_LinPT2TexturePass1;

	ebx1 = dwDeltaScreenX2;
	edi1 = ebx1;
	ebx1 = ebx1 << ( 12 );
	edi1 = ( (int32_t)edi1 ) >> ( 20 );
	edi1 = edi1 + dwXmax;
	dwAdderScreenX2 = edi1;
	dwAdderScreenX2f = ebx1;

	ebx1 = dwScreenX3;
	edi1 = ebx1;
	ebx1 = ebx1 << ( 12 );
	edi1 = ( (int32_t)edi1 ) >> ( 20 );
	edi1 = edi1 + dwYOffset2;

	ebp = dwYCounter2;
	ecx1 = edi1;

MV2DrawPolygonTBASM_LinPT2TexturePass2:
// eax = x1f (20 bit fraction)
// ebx = x2f (20 bit fraction)
// ecx = XCounter (init. BackBuffer + YOffset + X2)
// edx =
// esi = BackBuffer + YOffset + X1
// edi = BackBuffer + YOffset + X2
// ebp = YCounter

// st0 = TextureU
// st1 = TextureV
// st2 = TextureUdX
// st3 = TextureVdX
// st4 = 2^20
// st5 = 2^-20

//***> TODO: Pipeline the fpu better
	eax1 = eax1 >> ( 12 );
//sub		esp,CMV2ScanlinerLinTG__size
	SLTG--;

	dwScreenXf = eax1;
	ecx1 = ecx1 - esi1; // XCounter
	eax1 = eax1 << ( 12 );

	fpu_reg16 = ( (int32_t)dwScreenXf );
	fpu_reg16 = fpu_reg11 - fpu_reg16;
// st0 = 2^20 - ScreenXf, st1 = TextureU
// st2 = TextureV, st3 = TextureUdX,
// st4 = TextureVdX, st5 = 2^20, st6 = 2^-20

	fpu_reg16 = fpu_reg16 * fpu_reg10;
// st0 = ScreenXError
//fst		dword ptr ss:[esp].CMV2ScanlinerLinTG__fScreenXError
	SLTG->CMV2ScanlinerLinTG__fScreenXError = fpu_reg16;

	fpu_reg17 = fpu_reg13;
	fpu_reg17 = fpu_reg17 * fpu_reg16;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 * fpu_reg12;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
// st0 = TextureUdX*ScreenXError
// st1 = TextureVdX*ScreenXError
// st2 = TextureU, st3 = TextureV
// st4 = TextureUdX, st5 = TextureVdX
// st6 = 2^20, st7 = 2^-20


	fpu_reg17 = fpu_reg17 + fpu_reg15;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 + fpu_reg14;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
// st0 = TextureUdX*ScreenXError + TextureU
// st1 = TextureVdX*ScreenXError + TextureV
// st2 = TextureU, st3 = TextureV
// st4 = TextureUdX, st5 = TextureVdX
// st6 = 2^20, st7 = 2^-20

//fistp	DWORD PTR ss:[esp].CMV2ScanlinerLinTG__dwTextureU
//fistp	DWORD PTR ss:[esp].CMV2ScanlinerLinTG__dwTextureV
	SLTG->CMV2ScanlinerLinTG__dwTextureU = (int32_t)ceilf(fpu_reg17);
	SLTG->CMV2ScanlinerLinTG__dwTextureV = (int32_t)ceilf(fpu_reg16);

//mov		ss:[esp].CMV2ScanlinerLinTG__dwScreenX1Offset,esi
//mov		ss:[esp].CMV2ScanlinerLinTG__dwXCounter,ecx
	SLTG->CMV2ScanlinerLinTG__dwScreenX1Offset = esi1;
	SLTG->CMV2ScanlinerLinTG__dwXCounter = ecx1;
// st0 = TextureU, st1 = TextureV
// st2 = TextureUdX, st3 = TextureVdX
// st4 = 2^20, st5 = 2^-20

	fpu_reg15 = fpu_reg15 + fTextureUdY;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = fpu_reg15 + fTextureVdY;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }

	{ uint32_t carry = (UINT32_MAX - ebx1 < dwAdderScreenX2f)?1:0; ebx1 = ebx1 + dwAdderScreenX2f;
	  edi1 = edi1 + dwAdderScreenX2 + carry; }
	{ uint32_t carry = (UINT32_MAX - eax1 < dwAdderScreenX1f)?1:0; eax1 = eax1 + dwAdderScreenX1f;
	  esi1 = esi1 + dwAdderScreenX1 + carry; }
	ecx1 = edi1;

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2DrawPolygonTBASM_LinPT2TexturePass2;



	edx5 = (CMV2ScanlinerLinTG *)dwESPStartPoint;
	ebp = dwYCounter;

	fpu_reg12 = fB1;
	fpu_reg13 = fG1;
	fpu_reg14 = fR1;
MV2DrawPolygonTBASM_LinPT2GouraudPass1_2:
// eax =
// ebx =
// ecx =
// edx =
// esi =
// edi =
// ebp = YCounter

// st0 = R
// st1 = G
// st2 = B
// st3 = 2^20
// st4 = 2^-20

//***> TODO: Pipeline the fpu better
	edx5--;

	fpu_reg15 = edx5->CMV2ScanlinerLinTG__fScreenXError;
// st0 = ScreenXError, st0 = R, st1 = G, st2 = B
// st3 = 2^20, st4 = 2^-20

	fpu_reg16 = fGdX;
	fpu_reg16 = fpu_reg16 * fpu_reg15;
	fpu_reg17 = fRdX;
	fpu_reg17 = fpu_reg17 * fpu_reg15;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 * fBdX;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
// st0 = RdX*ScreenXError, st1 = GdX*ScreenXError
// st2 = BdX*ScreenXError, st3 = R, st4 = G, st5 = B
// st6 = 2^20, st7 = 2^-20

	fpu_reg17 = fpu_reg17 + fpu_reg14;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 + fpu_reg13;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 + fpu_reg12;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
// st0 = RdX*ScreenXError + R, st1 = GdX*ScreenXError + G
// st2 = BdX*ScreenXError + B, st3 = R, st4 = G, st5 = B
// st6 = 2^20, st7 = 2^-20

	edx5->CMV2ScanlinerLinTG__dwR = (int32_t)ceilf(fpu_reg17);
	edx5->CMV2ScanlinerLinTG__dwG = (int32_t)ceilf(fpu_reg16);
	edx5->CMV2ScanlinerLinTG__dwB = (int32_t)ceilf(fpu_reg15);
// st0 = R, st1 = G, st2 = B, st3 = 2^20, st4 = 2^-20

	fpu_reg14 = fpu_reg14 + fRdY;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg14 = fpu_reg14 + fGdY;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg14 = fpu_reg14 + fBdY;
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2DrawPolygonTBASM_LinPT2GouraudPass1_2;

//ffree	st(4)
//ffree	st(3)
//ffree	st(2)
//ffree	st(1)
//ffree	st(0)






	goto MV2DrawPolygonTBASM_DoLinTGYLoop;
MV2DrawPolygonTBASM_LinearPolygonType3:
//=============>                <==============
//=============> Linear		   <==============
//=============> Polygon Type 3 <==============
//=============>                <==============
	fScreenYError1 = edx3->CMV2Dot3DPos__m_fScreenYError;
	fScreenYError2 = esi3->CMV2Dot3DPos__m_fScreenYError;

	edx2 = pDot1;
	esi2 = pDot2;
	edi2 = pDot3;

//===>                                                                <===
//===>	TextureUV DeltaY Calculation {                            	 <===
//===>                                                                <===
	fpu_reg12 = edi2->CMV2Dot3D__m_fTextureU;
	fpu_reg12 = fpu_reg12 - edx2->CMV2Dot3D__m_fTextureU;
	fpu_reg13 = edi2->CMV2Dot3D__m_fTextureV;
	fpu_reg13 = fpu_reg13 - edx2->CMV2Dot3D__m_fTextureV;
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
	fpu_reg13 = fpu_reg13 * fpu_reg10;
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
	fpu_reg13 = fpu_reg13 * fpu_reg10; // fmul stall (+1 Cycle)
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
	fpu_reg14 = fScreenYError1;
	fpu_reg15 = fpu_reg14;
	fpu_reg15 = fpu_reg15 * fpu_reg13;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = fpu_reg15 * fpu_reg12; // fmul stall (+1 Cycle)
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
//st0 = ScreenYError1*TextureUdY1
//st1 = ScreenYError1*TextureVdY1
//st2 = TextureUdY1, st3 = TextureVdY1
//st4 = 1/YCounter2, st5 = 1/YCounter1

	fpu_reg15 = fpu_reg15 + edx2->CMV2Dot3D__m_fTextureU;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = fpu_reg15 + edx2->CMV2Dot3D__m_fTextureV;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }

	fTextureU1 = fpu_reg15;
	fTextureV1 = fpu_reg14;
	fTextureUdY1 = fpu_reg13;
	fTextureVdY1 = fpu_reg12;

//===>                                                                <===
//===>	TextureUV DeltaY Calculation }                            	 <===
//===>                                                                <===


//===>                                                                <===
//===>	TextureUV DeltaX Calculation {                            	 <===
//===>                                                                <===
	fpu_reg12 = fMaxXCounterRZP;
	fpu_reg12 = fpu_reg12 * fNum_2EXP20;
	fpu_reg13 = fYCounter2;
	fpu_reg14 = fTextureVdY1;
	fpu_reg15 = fTextureUdY1;
	fpu_reg15 = fpu_reg15 * fpu_reg13;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg13 = fpu_reg13 * fpu_reg15; // fmul stall (+ 1 Cycle)
	fpu_reg14 = fpu_reg14 + edx2->CMV2Dot3D__m_fTextureU;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg14 = fpu_reg14 + edx2->CMV2Dot3D__m_fTextureV;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg14 = esi2->CMV2Dot3D__m_fTextureU - fpu_reg14;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg14 = esi2->CMV2Dot3D__m_fTextureV - fpu_reg14;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
//st0 = TextureU2 - (YCounter2*TextureUdY1 + TextureU1)
//st1 = TextureV2 - (YCounter2*TextureVdY1 + TextureV1)
//st2 = 1/MaxXCounter
//st3 = 1/YCounter2, st4 = 1/YCounter1

	fpu_reg14 = fpu_reg14 * fpu_reg12;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg14 = fpu_reg14 * fpu_reg12;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }

	fTextureUdX = fpu_reg14;
	dwTextureUdX = (int32_t)ceilf(fpu_reg14);
	fTextureVdX = fpu_reg13;
	dwTextureVdX = (int32_t)ceilf(fpu_reg13);
//===>                                                                <===
//===>	TextureUV DeltaX Calculation }                            	 <===
//===>                                                                <===

//ffree	st(2)
//ffree	st(1)
//ffree	st(0)




	eax1 = dwDeltaScreenX1;
	ebx1 = dwDeltaScreenX2;
	esi1 = eax1;
	edi1 = ebx1;
	eax1 = eax1 << ( 12 );
	ebx1 = ebx1 << ( 12 );
	esi1 = ( (int32_t)esi1 ) >> ( 20 );
	edi1 = ( (int32_t)edi1 ) >> ( 20 );
	esi1 = esi1 + dwXmax;
	edi1 = edi1 + dwXmax;
	dwAdderScreenX1 = esi1;
	dwAdderScreenX2 = edi1;
	dwAdderScreenX1f = eax1;
	dwAdderScreenX2f = ebx1;

	eax1 = dwScreenX1;
	ebx1 = dwScreenX2;
	esi1 = eax1;
	edi1 = ebx1;
	eax1 = eax1 << ( 12 );
	ebx1 = ebx1 << ( 12 );
	esi1 = ( (int32_t)esi1 ) >> ( 20 );
	edi1 = ( (int32_t)edi1 ) >> ( 20 );
	esi1 = esi1 + dwYOffset1;
	edi1 = edi1 + dwYOffset1;

//mov		dwOldESP,esp
//and		esp,0ffffffe0h
	SLTG = (CMV2ScanlinerLinTG *) mem_alloc_endptr(dwYCounter * sizeof(CMV2ScanlinerLinTG));
//mov		dwESPStartPoint,esp
	dwESPStartPoint = (void *)SLTG;

	ebp = dwYCounter;
	ecx1 = edi1;

	fpu_reg10 = fNum_2EXP_20;
	fpu_reg11 = fNum_2EXP20;
	fpu_reg12 = fTextureVdX;
	fpu_reg13 = fTextureUdX;
	fpu_reg14 = fTextureV1;
	fpu_reg15 = fTextureU1;

MV2DrawPolygonTBASM_LinPT3Texture:
// eax = x1f (20 bit fraction)
// ebx = x2f (20 bit fraction)
// ecx = XCounter (init. BackBuffer + YOffset + X2)
// edx =
// esi = BackBuffer + YOffset + X1
// edi = BackBuffer + YOffset + X2
// ebp = YCounter

// st0 = TextureU
// st1 = TextureV
// st2 = TextureUdX
// st3 = TextureVdX
// st4 = 2^20
// st5 = 2^-20

//***> TODO: Pipeline the fpu better
	eax1 = eax1 >> ( 12 );
//sub		esp,CMV2ScanlinerLinTG__size
	SLTG--;

	dwScreenXf = eax1;
	ecx1 = ecx1 - esi1; // XCounter
	eax1 = eax1 << ( 12 );

	fpu_reg16 = ( (int32_t)dwScreenXf );
	fpu_reg16 = fpu_reg11 - fpu_reg16;
// st0 = 2^20 - ScreenXf, st1 = TextureU
// st2 = TextureV, st3 = TextureUdX,
// st4 = TextureVdX, st5 = 2^20, st6 = 2^-20

	fpu_reg16 = fpu_reg16 * fpu_reg10;
// st0 = ScreenXError
//fst		dword ptr ss:[esp].CMV2ScanlinerLinTG__fScreenXError
	SLTG->CMV2ScanlinerLinTG__fScreenXError = fpu_reg16;

	fpu_reg17 = fpu_reg13;
	fpu_reg17 = fpu_reg17 * fpu_reg16;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 * fpu_reg12;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
// st0 = TextureUdX*ScreenXError
// st1 = TextureVdX*ScreenXError
// st2 = TextureU, st3 = TextureV
// st4 = TextureUdX, st5 = TextureVdX
// st6 = 2^20, st7 = 2^-20


	fpu_reg17 = fpu_reg17 + fpu_reg15;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 + fpu_reg14;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
// st0 = TextureUdX*ScreenXError + TextureU
// st1 = TextureVdX*ScreenXError + TextureV
// st2 = TextureU, st3 = TextureV
// st4 = TextureUdX, st5 = TextureVdX
// st6 = 2^20, st7 = 2^-20

//fistp	DWORD PTR ss:[esp].CMV2ScanlinerLinTG__dwTextureU
//fistp	DWORD PTR ss:[esp].CMV2ScanlinerLinTG__dwTextureV
	SLTG->CMV2ScanlinerLinTG__dwTextureU = (int32_t)ceilf(fpu_reg17);
	SLTG->CMV2ScanlinerLinTG__dwTextureV = (int32_t)ceilf(fpu_reg16);

//mov		ss:[esp].CMV2ScanlinerLinTG__dwScreenX1Offset,esi
//mov		ss:[esp].CMV2ScanlinerLinTG__dwXCounter,ecx
	SLTG->CMV2ScanlinerLinTG__dwScreenX1Offset = esi1;
	SLTG->CMV2ScanlinerLinTG__dwXCounter = ecx1;
// st0 = TextureU, st1 = TextureV
// st2 = TextureUdX, st3 = TextureVdX
// st4 = 2^20, st5 = 2^-20

	fpu_reg15 = fpu_reg15 + fTextureUdY1;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = fpu_reg15 + fTextureVdY1;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }

	{ uint32_t carry = (UINT32_MAX - ebx1 < dwAdderScreenX2f)?1:0; ebx1 = ebx1 + dwAdderScreenX2f;
	  edi1 = edi1 + dwAdderScreenX2 + carry; }
	{ uint32_t carry = (UINT32_MAX - eax1 < dwAdderScreenX1f)?1:0; eax1 = eax1 + dwAdderScreenX1f;
	  esi1 = esi1 + dwAdderScreenX1 + carry; }
	ecx1 = edi1;

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2DrawPolygonTBASM_LinPT3Texture;



	edx5 = (CMV2ScanlinerLinTG *)dwESPStartPoint;
	ebp = dwYCounter;

	fpu_reg12 = fB1;
	fpu_reg13 = fG1;
	fpu_reg14 = fR1;
MV2DrawPolygonTBASM_LinPT3Gouraud:
// eax =
// ebx =
// ecx =
// edx =
// esi =
// edi =
// ebp = YCounter

// st0 = R
// st1 = G
// st2 = B
// st3 = 2^20
// st4 = 2^-20

//***> TODO: Pipeline the fpu better
	edx5--;

	fpu_reg15 = edx5->CMV2ScanlinerLinTG__fScreenXError;
// st0 = ScreenXError, st0 = R, st1 = G, st2 = B
// st3 = 2^20, st4 = 2^-20

	fpu_reg16 = fGdX;
	fpu_reg16 = fpu_reg16 * fpu_reg15;
	fpu_reg17 = fRdX;
	fpu_reg17 = fpu_reg17 * fpu_reg15;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 * fBdX;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
// st0 = RdX*ScreenXError, st1 = GdX*ScreenXError
// st2 = BdX*ScreenXError, st3 = R, st4 = G, st5 = B
// st6 = 2^20, st7 = 2^-20

	fpu_reg17 = fpu_reg17 + fpu_reg14;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 + fpu_reg13;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 + fpu_reg12;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
// st0 = RdX*ScreenXError + R, st1 = GdX*ScreenXError + G
// st2 = BdX*ScreenXError + B, st3 = R, st4 = G, st5 = B
// st6 = 2^20, st7 = 2^-20

	edx5->CMV2ScanlinerLinTG__dwR = (int32_t)ceilf(fpu_reg17);
	edx5->CMV2ScanlinerLinTG__dwG = (int32_t)ceilf(fpu_reg16);
	edx5->CMV2ScanlinerLinTG__dwB = (int32_t)ceilf(fpu_reg15);
// st0 = R, st1 = G, st2 = B, st3 = 2^20, st4 = 2^-20

	fpu_reg14 = fpu_reg14 + fRdY1;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg14 = fpu_reg14 + fGdY1;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg14 = fpu_reg14 + fBdY1;
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2DrawPolygonTBASM_LinPT3Gouraud;

//ffree	st(4)
//ffree	st(3)
//ffree	st(2)
//ffree	st(1)
//ffree	st(0)






	goto MV2DrawPolygonTBASM_DoLinTGYLoop;
MV2DrawPolygonTBASM_LinearPolygonType4:
//=============>                <==============
//=============> Linear		   <==============
//=============> Polygon Type 4 <==============
//=============>                <==============
	fScreenYError1 = edx3->CMV2Dot3DPos__m_fScreenYError;
	fScreenYError2 = esi3->CMV2Dot3DPos__m_fScreenYError;

	edx2 = pDot1;
	esi2 = pDot2;
	edi2 = pDot3;

//===>                                                                <===
//===>	TextureUV DeltaY Calculation {                            	 <===
//===>                                                                <===
	fpu_reg13 = edi2->CMV2Dot3D__m_fTextureU;
	fpu_reg13 = fpu_reg13 - edx2->CMV2Dot3D__m_fTextureU;
	fpu_reg14 = edi2->CMV2Dot3D__m_fTextureV;
	fpu_reg14 = fpu_reg14 - edx2->CMV2Dot3D__m_fTextureV;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg14 = fpu_reg14 * fpu_reg11;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg14 = fpu_reg14 * fpu_reg11; // fmul stall (+1 Cycle)
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg15 = fScreenYError1;
	fpu_reg16 = fpu_reg15;
	fpu_reg16 = fpu_reg16 * fpu_reg14;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = fpu_reg16 * fpu_reg13; // fmul stall (+1 Cycle)
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
//st0 = ScreenYError1*TextureUdY1
//st1 = ScreenYError1*TextureVdY1
//st2 = TextureUdY1, st3 = TextureVdY1
//st4 = 1/YCounter2, st5 = 1/YCounter1

	fpu_reg16 = fpu_reg16 + edx2->CMV2Dot3D__m_fTextureU;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = fpu_reg16 + edx2->CMV2Dot3D__m_fTextureV;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }

	fTextureU1 = fpu_reg16;
	fTextureV1 = fpu_reg15;
	fTextureUdY1 = fpu_reg14;
	fTextureVdY1 = fpu_reg13;
//===>                                                                <===
//===>	TextureUV DeltaY Calculation }                            	 <===
//===>                                                                <===


//===>                                                                <===
//===>	TextureUV DeltaX Calculation {                            	 <===
//===>                                                                <===
	fpu_reg13 = fMaxXCounterRZP;
	fpu_reg13 = fpu_reg13 * fNum_2EXP20;
	fpu_reg14 = fYCounter;
	fpu_reg15 = fTextureVdY1;
	fpu_reg16 = fTextureUdY1;
	fpu_reg16 = fpu_reg16 * fpu_reg14;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg14 = fpu_reg14 * fpu_reg16; // fmul stall (+ 1 Cycle)
	fpu_reg15 = fpu_reg15 + edx2->CMV2Dot3D__m_fTextureU;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = fpu_reg15 + edx2->CMV2Dot3D__m_fTextureV;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = esi2->CMV2Dot3D__m_fTextureU - fpu_reg15;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = esi2->CMV2Dot3D__m_fTextureV - fpu_reg15;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
//st0 = TextureU2 - (YCounter*TextureUdY1 + TextureU1)
//st1 = TextureV2 - (YCounter*TextureVdY1 + TextureV1)
//st2 = 1/MaxXCounter
//st3 = 1/YCounter2, st4 = 1/YCounter1

	fpu_reg15 = fpu_reg15 * fpu_reg13;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = fpu_reg15 * fpu_reg13;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }

	fTextureUdX = fpu_reg15;
	dwTextureUdX = (int32_t)ceilf(fpu_reg15);
	fTextureVdX = fpu_reg14;
	dwTextureVdX = (int32_t)ceilf(fpu_reg14);

//===>                                                                <===
//===>	TextureUV DeltaX Calculation }                            	 <===
//===>                                                                <===

//ffree	st(2)
//ffree	st(1)
//ffree	st(0)





	eax1 = dwDeltaScreenX1;
	ebx1 = dwDeltaScreenX2;
	esi1 = eax1;
	edi1 = ebx1;
	eax1 = eax1 << ( 12 );
	ebx1 = ebx1 << ( 12 );
	esi1 = ( (int32_t)esi1 ) >> ( 20 );
	edi1 = ( (int32_t)edi1 ) >> ( 20 );
	esi1 = esi1 + dwXmax;
	edi1 = edi1 + dwXmax;
	dwAdderScreenX1 = esi1;
	dwAdderScreenX2 = edi1;
	dwAdderScreenX1f = eax1;
	dwAdderScreenX2f = ebx1;

	eax1 = dwScreenX1;
	ebx1 = dwScreenX2;
	esi1 = eax1;
	edi1 = ebx1;
	eax1 = eax1 << ( 12 );
	ebx1 = ebx1 << ( 12 );
	esi1 = ( (int32_t)esi1 ) >> ( 20 );
	edi1 = ( (int32_t)edi1 ) >> ( 20 );
	esi1 = esi1 + dwYOffset1;
	edi1 = edi1 + dwYOffset1;

//mov		dwOldESP,esp
//and		esp,0ffffffe0h
	SLTG = (CMV2ScanlinerLinTG *) mem_alloc_endptr(dwYCounter * sizeof(CMV2ScanlinerLinTG));
//mov		dwESPStartPoint,esp
	dwESPStartPoint = (void *)SLTG;

	ebp = dwYCounter;
	ecx1 = edi1;

	fpu_reg10 = fNum_2EXP_20;
	fpu_reg11 = fNum_2EXP20;
	fpu_reg12 = fTextureVdX;
	fpu_reg13 = fTextureUdX;
	fpu_reg14 = fTextureV1;
	fpu_reg15 = fTextureU1;

MV2DrawPolygonTBASM_LinPT4Texture:
// eax = x1f (20 bit fraction)
// ebx = x2f (20 bit fraction)
// ecx = XCounter (init. BackBuffer + YOffset + X2)
// edx =
// esi = BackBuffer + YOffset + X1
// edi = BackBuffer + YOffset + X2
// ebp = YCounter

// st0 = TextureU
// st1 = TextureV
// st2 = TextureUdX
// st3 = TextureVdX
// st4 = 2^20
// st5 = 2^-20

//***> TODO: Pipeline the fpu better
	eax1 = eax1 >> ( 12 );
//sub		esp,CMV2ScanlinerLinTG__size
	SLTG--;

	dwScreenXf = eax1;
	ecx1 = ecx1 - esi1; // XCounter
	eax1 = eax1 << ( 12 );

	fpu_reg16 = ( (int32_t)dwScreenXf );
	fpu_reg16 = fpu_reg11 - fpu_reg16;
// st0 = 2^20 - ScreenXf, st1 = TextureU
// st2 = TextureV, st3 = TextureUdX,
// st4 = TextureVdX, st5 = 2^20, st6 = 2^-20

	fpu_reg16 = fpu_reg16 * fpu_reg10;
// st0 = ScreenXError
//fst		dword ptr ss:[esp].CMV2ScanlinerLinTG__fScreenXError
	SLTG->CMV2ScanlinerLinTG__fScreenXError = fpu_reg16;

	fpu_reg17 = fpu_reg13;
	fpu_reg17 = fpu_reg17 * fpu_reg16;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 * fpu_reg12;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
// st0 = TextureUdX*ScreenXError
// st1 = TextureVdX*ScreenXError
// st2 = TextureU, st3 = TextureV
// st4 = TextureUdX, st5 = TextureVdX
// st6 = 2^20, st7 = 2^-20


	fpu_reg17 = fpu_reg17 + fpu_reg15;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 + fpu_reg14;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
// st0 = TextureUdX*ScreenXError + TextureU
// st1 = TextureVdX*ScreenXError + TextureV
// st2 = TextureU, st3 = TextureV
// st4 = TextureUdX, st5 = TextureVdX
// st6 = 2^20, st7 = 2^-20

//fistp	DWORD PTR ss:[esp].CMV2ScanlinerLinTG__dwTextureU
//fistp	DWORD PTR ss:[esp].CMV2ScanlinerLinTG__dwTextureV
	SLTG->CMV2ScanlinerLinTG__dwTextureU = (int32_t)ceilf(fpu_reg17);
	SLTG->CMV2ScanlinerLinTG__dwTextureV = (int32_t)ceilf(fpu_reg16);

//mov		ss:[esp].CMV2ScanlinerLinTG__dwScreenX1Offset,esi
//mov		ss:[esp].CMV2ScanlinerLinTG__dwXCounter,ecx
	SLTG->CMV2ScanlinerLinTG__dwScreenX1Offset = esi1;
	SLTG->CMV2ScanlinerLinTG__dwXCounter = ecx1;
// st0 = TextureU, st1 = TextureV
// st2 = TextureUdX, st3 = TextureVdX
// st4 = 2^20, st5 = 2^-20

	fpu_reg15 = fpu_reg15 + fTextureUdY1;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = fpu_reg15 + fTextureVdY1;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }

	{ uint32_t carry = (UINT32_MAX - ebx1 < dwAdderScreenX2f)?1:0; ebx1 = ebx1 + dwAdderScreenX2f;
	  edi1 = edi1 + dwAdderScreenX2 + carry; }
	{ uint32_t carry = (UINT32_MAX - eax1 < dwAdderScreenX1f)?1:0; eax1 = eax1 + dwAdderScreenX1f;
	  esi1 = esi1 + dwAdderScreenX1 + carry; }
	ecx1 = edi1;

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2DrawPolygonTBASM_LinPT4Texture;



	edx5 = (CMV2ScanlinerLinTG *)dwESPStartPoint;
	ebp = dwYCounter;

	fpu_reg12 = fB1;
	fpu_reg13 = fG1;
	fpu_reg14 = fR1;
MV2DrawPolygonTBASM_LinPT4Gouraud:
// eax =
// ebx =
// ecx =
// edx =
// esi =
// edi =
// ebp = YCounter

// st0 = R
// st1 = G
// st2 = B
// st3 = 2^20
// st4 = 2^-20

//***> TODO: Pipeline the fpu better
	edx5--;

	fpu_reg15 = edx5->CMV2ScanlinerLinTG__fScreenXError;
// st0 = ScreenXError, st0 = R, st1 = G, st2 = B
// st3 = 2^20, st4 = 2^-20

	fpu_reg16 = fGdX;
	fpu_reg16 = fpu_reg16 * fpu_reg15;
	fpu_reg17 = fRdX;
	fpu_reg17 = fpu_reg17 * fpu_reg15;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 * fBdX;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
// st0 = RdX*ScreenXError, st1 = GdX*ScreenXError
// st2 = BdX*ScreenXError, st3 = R, st4 = G, st5 = B
// st6 = 2^20, st7 = 2^-20

	fpu_reg17 = fpu_reg17 + fpu_reg14;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 + fpu_reg13;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 + fpu_reg12;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
// st0 = RdX*ScreenXError + R, st1 = GdX*ScreenXError + G
// st2 = BdX*ScreenXError + B, st3 = R, st4 = G, st5 = B
// st6 = 2^20, st7 = 2^-20

	edx5->CMV2ScanlinerLinTG__dwR = (int32_t)ceilf(fpu_reg17);
	edx5->CMV2ScanlinerLinTG__dwG = (int32_t)ceilf(fpu_reg16);
	edx5->CMV2ScanlinerLinTG__dwB = (int32_t)ceilf(fpu_reg15);
// st0 = R, st1 = G, st2 = B, st3 = 2^20, st4 = 2^-20

	fpu_reg14 = fpu_reg14 + fRdY1;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg14 = fpu_reg14 + fGdY1;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg14 = fpu_reg14 + fBdY1;
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2DrawPolygonTBASM_LinPT4Gouraud;

//ffree	st(4)
//ffree	st(3)
//ffree	st(2)
//ffree	st(1)
//ffree	st(0)






	goto MV2DrawPolygonTBASM_DoLinTGYLoop;
MV2DrawPolygonTBASM_DoLinTGYLoop:
//================> 								<=====================
//================> 								<=====================
//================> 	Linear						<=====================
//================>  Texture Bump InnerLoop		<=====================
//================> 								<=====================
//================> 								<=====================
MV2DrawPolygonTBASM_LinTGYLoop:
	edi1 = 0;
	ebx1 = dwTextureUdX;
	eax1 = dwTextureVdX;
	ebp = ebx1;
	esi1 = eax1;
	ebx1 = ebx1 >> ( 16 ); // bl init.
	eax1 = eax1 >> ( 16 );
	esi1 = esi1 << ( 16 ); // esi init.
	ebp = ebp << ( 16 ); // ebp init.
	ebx1 = set_high_byte(ebx1, ( (uint8_t)eax1 )); // bh init.
//mov		eax,ss:[esp].CMV2ScanlinerLinTG__dwTextureU
//mov		ecx,ss:[esp].CMV2ScanlinerLinTG__dwTextureV
	eax1 = SLTG->CMV2ScanlinerLinTG__dwTextureU;
	ecx1 = SLTG->CMV2ScanlinerLinTG__dwTextureV;
	edx1 = eax1;
	eax1 = eax1 << ( 16 );
	edx1 = edx1 >> ( 16 ); // dl init.
	ebx1 = ebx1 | eax1; // ebx init.
	eax1 = ecx1;
	ecx1 = ecx1 << ( 16 ); // hi ecx init.
	eax1 = eax1 >> ( 16 );
//or		ecx,ss:[esp].CMV2ScanlinerLinTG__dwXCounter	; ecx init.
	ecx1 = ecx1 | SLTG->CMV2ScanlinerLinTG__dwXCounter; // ecx init.
	edx1 = set_high_byte(edx1, ( (uint8_t)eax1 )); // dh init.
	//edx1 = edx1 | pcTexture; // edx init.


	if ((( (uint16_t)ecx1 ) & ( (uint16_t)ecx1 )) == 0) goto MV2DrawPolygonTBASM_LinTGNoXLoop;

MV2DrawPolygonTBASM_LinTXLoop:
//*eax = counter
//*ebx =	txf				|dty	|dtx
//*ecx = tyf				|       |col
//*edx = pTexture		|ty		|tx
//*esi = dtyf
//*edi = destination
//*ebp = dtxf
	eax1 = edx1;
	edx1 = edx1 & ( 0x0ffff );
	//edx1 = edx1 + pcBumpmap;
	pBackBumpBuffer[edi1] = edx1; // ->pBackBumpBuffer
	edx1 = eax1;

	eax1 = pcTexture[edx1];
	pBackTextureBuffer[edi1] = eax1;

	{ uint32_t carry = (UINT32_MAX - ebx1 < ebp)?1:0; ebx1 = ebx1 + ebp;
	  edx1 = (edx1 & 0xffffff00) | (uint8_t)(( (uint8_t)edx1 ) + ( (uint8_t)ebx1 ) + carry); }
	{ uint32_t carry = (UINT32_MAX - ecx1 < esi1)?1:0; ecx1 = ecx1 + esi1;
	  edx1 = set_high_byte(edx1, ( (uint8_t)(edx1 >> 8) ) + ( (uint8_t)(ebx1 >> 8) ) + carry); }

	edi1++;

	ecx1 = (ecx1 & 0xffff0000) | (uint16_t)(( (int16_t)ecx1 ) - 1);
	if (( (int16_t)ecx1 ) != 0) goto MV2DrawPolygonTBASM_LinTXLoop;

//mov		ebp,ss:[esp].CMV2ScanlinerLinTG__dwR
//mov		edx,ss:[esp].CMV2ScanlinerLinTG__dwG
//mov     esi,ss:[esp].CMV2ScanlinerLinTG__dwB
//mov		ebx,ss:[esp].CMV2ScanlinerLinTG__dwXCounter
	ebp = SLTG->CMV2ScanlinerLinTG__dwR;
	edx1 = SLTG->CMV2ScanlinerLinTG__dwG;
	esi1 = SLTG->CMV2ScanlinerLinTG__dwB;
	ebx1 = SLTG->CMV2ScanlinerLinTG__dwXCounter;
	edi1 = 0;

MV2DrawPolygonTBASM_LinBumpLoop:
// eax =
// ebx = XCounter
// ecx =
// edx =					|UAngle	|UAnglef
// esi = 				|VAngle	|VAnglef
// edi = Bump-,ShadeBuffer	(Loaded with BumbBuffer)
// ebp = 				|NAngle |NAnglef
	stack_var00 = ebx1;

	ebx1 = 0;

	if (ebp > ( 0x0ffff )) goto MV2DrawPolygonTBASM_LinAmbient;

	eax1 = pBackBumpBuffer[edi1];
	ecx1 = 0;
	ebx1 = set_high_byte(ebx1, ( (uint8_t)(ebx1 >> 8) ) ^ ( ((uint8_t *)&(pcBumpmap[eax1]))[0] )); //UBump
	ecx1 = set_high_byte(ecx1, ( ((uint8_t *)&(pcBumpmap[eax1]))[1] )); //VBump
	ebx1 = ebx1 - ( 128 * 256 );
	ecx1 = ecx1 - ( 128 * 256 );
	eax1 = 0;
	ebx1 = ebx1 + edx1; //UAngle + UBump
	ecx1 = ecx1 + esi1; //VAngle + VBump


	if (ebx1 > ( 0x0ffff )) goto MV2DrawPolygonTBASM_LinAmbientBump;

	if (ecx1 > ( 0x0ffff )) goto MV2DrawPolygonTBASM_LinAmbientBump;
	eax1 = (eax1 & 0xffffff00) | (uint8_t)(( (uint8_t)(ebx1 >> 8) ));
	eax1 = set_high_byte(eax1, ( (uint8_t)(ecx1 >> 8) ));
	ebx1 = 0;
	ebx1 = (ebx1 & 0xffffff00) | (uint8_t)(( pAngleJoinTab[eax1] )); //pAngleJoin
	goto MV2DrawPolygonTBASM_LinAmbient;
MV2DrawPolygonTBASM_LinAmbientBump:
	ebx1 = 0;
MV2DrawPolygonTBASM_LinAmbient:
	ebx1 = ( pLightBrightnessTab[ebx1] ); //Helligkeit
	pBackShadeBuffer[edi1] = ebx1; //ShadeBuffer

	edi1++;
	ebx1 = stack_var00;
	edx1 = edx1 + dwGdX;
	esi1 = esi1 + dwBdX;
	ebp = ebp + dwRdX;

	ebx1 = ( (int32_t)ebx1 ) - 1;
	if (( (int32_t)ebx1 ) != 0) goto MV2DrawPolygonTBASM_LinBumpLoop;

//mov		edi,ss:[esp].CMV2ScanlinerLinTG__dwScreenX1Offset
//mov		ebp,ss:[esp].CMV2ScanlinerLinTG__dwXCounter
	edi1 = SLTG->CMV2ScanlinerLinTG__dwScreenX1Offset;
	ebp = SLTG->CMV2ScanlinerLinTG__dwXCounter;
	esi1 = 0;
	eax1 = 0;
	ebx1 = 0;
	ecx1 = 0;
	edx1 = 0;
	edi1 = edi1 - 1;

MV2DrawPolygonTBASM_LinShadeLoop:
// eax = r
// ebx = shaded rgb
// ecx = g
// edx = b
// esi = offset
// edi = dest
// ebp = XCounter

	eax1 = pBackShadeBuffer[esi1];
	ebx1 = pBackTextureBuffer[esi1];
	eax1 = eax1 & ( 0x0ffffff );
	ebx1 = ebx1 & ( 0x0ffffff );
	edx1 = set_high_byte(edx1, ( (uint8_t)eax1 ));
	ecx1 = set_high_byte(ecx1, ( (uint8_t)(eax1 >> 8) ));
	edx1 = (edx1 & 0xffffff00) | (uint8_t)(( (uint8_t)ebx1 ));
	ecx1 = (ecx1 & 0xffffff00) | (uint8_t)(( (uint8_t)(ebx1 >> 8) ));
	eax1 = eax1 >> ( 8 );
	esi1++;
	ebx1 = ebx1 >> ( 16 );
	edi1 = edi1 + 1;
	eax1 = (eax1 & 0xffffff00) | (uint8_t)(( (uint8_t)ebx1 ));

	ebx1 = (ebx1 & 0xffffff00) | (uint8_t)(( pBumpShadeLookup[ecx1] )); // g
	ebx1 = set_high_byte(ebx1, ( pBumpShadeLookup[eax1] )); // r
	ebx1 = ebx1 << ( 8 );
	ebx1 = (ebx1 & 0xffffff00) | (uint8_t)(( pBumpShadeLookup[edx1] )); // b
//	or		ebx,0ffh
	pcBackBuffer[edi1] = ebx1;

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2DrawPolygonTBASM_LinShadeLoop;
MV2DrawPolygonTBASM_LinTGNoXLoop:
//add		esp,CMV2ScanlinerLinTG__size
	SLTG++;

	dwYCounter = ( (int32_t)dwYCounter ) - 1;
	if (( (int32_t)dwYCounter ) != 0) goto MV2DrawPolygonTBASM_LinTGYLoop;

//mov		esp,dwOldESP
MV2DrawPolygonTBASM_Done:


	return;
}





