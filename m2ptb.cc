#include <malloc.h>
#include "cc.h"
//;/*
//.486

//.model flat

//LOCALS

//data32 SEGMENT PAGE
//.data

//EXTRN	ppPolygons			:DWORD
extern "C" uint32_t pcBackBuffer;
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

static uint32_t pcTexture;
static uint32_t pcBumpmap;

static uint32_t pDot1;
static uint32_t pDot2;
static uint32_t pDot3;


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


static uint32_t dwESPStartPoint;
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

static uint32_t dwRGBIntLoop_eax;
static uint32_t dwRGBIntLoop_ecx;
static uint32_t dwRGBIntLoop_esi;
static uint32_t dwRGBIntLoop_ebp;



//fpucw						dw ?
//oldfpucw					dw ?
//oh_low 						dd ?
//co_low						dd ?
//co_hi						dd ?
//ends

//.code

#include "m2struct.inc.h"

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

extern "C" void MV2DrawPolygonTBASM(uint32_t _edi) {
	float fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13, fpu_reg14, fpu_reg15, fpu_reg16, fpu_reg17, fpu_reg18;
	uint32_t eax, edx, ecx, edi = _edi, ebx, esi, ebp;
	uint32_t stack_var00;
// edi = pPolygon
	CMV2ScanlinerPerspTG *SPTG;
	CMV2ScanlinerLinTG *SLTG;

	eax = ( ((CMV2Polygon *)edi)->CMV2Polygon__m_pDot1 );
	ebx = ( ((CMV2Polygon *)edi)->CMV2Polygon__m_pDot2 );
	ecx = ( ((CMV2Polygon *)edi)->CMV2Polygon__m_pDot3 );
//eax - ecx: pDot3D1 - pDot3D3

	pDot1 = eax;
	pDot2 = ebx;
	pDot3 = ecx;

	edx = ( ((CMV2Polygon *)edi)->CMV2Polygon__m_pcTexture );
	eax = ( ((CMV2Dot3D *)eax)->CMV2Dot3D__m_pPos );
	edx = edx >> ( 2 );
	ebx = ( ((CMV2Dot3D *)ebx)->CMV2Dot3D__m_pPos );
	ecx = ( ((CMV2Dot3D *)ecx)->CMV2Dot3D__m_pPos );
//eax - ecx: pDot3DPos1 - pDot3DPos3
	pcTexture = edx;
	edx = ( ((CMV2Polygon *)edi)->CMV2Polygon__m_pcBumpmap );
	edx = edx >> ( 1 );
	pcBumpmap = edx;

	edx = ( ((CMV2Dot3DPos *)eax)->CMV2Dot3DPos__m_iScreenY );
	esi = ( ((CMV2Dot3DPos *)ebx)->CMV2Dot3DPos__m_iScreenY );
	edi = ( ((CMV2Dot3DPos *)ecx)->CMV2Dot3DPos__m_iScreenY );
	eax = pDot1;
	ebx = pDot2;
	ecx = pDot3;

// find dot with smallest y

	if (edx <= esi) goto MV2DrawPolygonTBASM_Dot1YSmallerDot2Y; // dot1.y <= dot2.y


	if (esi <= edi) goto MV2DrawPolygonTBASM_Dot2YSmallest; // dot2.y <(=) dot3.y&dot1.y
	goto MV2DrawPolygonTBASM_Dot3YSmallest; // dot3.y <    dot2.y&dot1.y
MV2DrawPolygonTBASM_Dot1YSmallerDot2Y:

	if (edx >= edi) goto MV2DrawPolygonTBASM_Dot3YSmallest; // dot3.y <    dot1.y&dot2.y

//MV2DrawPolygonTBASM_Dot1YSmallest:
	edx = eax;
	esi = ebx;
	edi = ecx;
	goto MV2DrawPolygonTBASM_DotsSorted;
MV2DrawPolygonTBASM_Dot2YSmallest:
	edx = ebx;
	esi = ecx;
	edi = eax;
	goto MV2DrawPolygonTBASM_DotsSorted;
MV2DrawPolygonTBASM_Dot3YSmallest:
	edx = ecx;
	esi = eax;
	edi = ebx;
	goto MV2DrawPolygonTBASM_DotsSorted;
MV2DrawPolygonTBASM_DotsSorted:

// edx = pDot1
// esi = pDot2
// edi = pDot3

	pDot1 = edx;
	pDot2 = esi;
	pDot3 = edi;

	edx = ( ((CMV2Dot3D *)edx)->CMV2Dot3D__m_pPos );
	esi = ( ((CMV2Dot3D *)esi)->CMV2Dot3D__m_pPos );
	edi = ( ((CMV2Dot3D *)edi)->CMV2Dot3D__m_pPos );

	eax = ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_iScreenY );
	ebx = ( ((CMV2Dot3DPos *)esi)->CMV2Dot3DPos__m_iScreenY );
	ecx = ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_iScreenY );


	if (eax == ebx) goto MV2DrawPolygonTBASM_PolygonType4;


	if (ebx == ecx) goto MV2DrawPolygonTBASM_PolygonType3;
	if (ebx < ecx) goto MV2DrawPolygonTBASM_PolygonType2;


//=============>                <==============
//=============> Polygon Type 1 <==============
//=============>                <==============
//MV2DrawPolygonTBASM_PolygonType1:
// edx = pDot1
// esi = pDot2
// edi = pDot3

	fpu_reg10 = ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fScreenY );
	fpu_reg11 = ( ((CMV2Dot3DPos *)esi)->CMV2Dot3DPos__m_fScreenY );
	fpu_reg12 = ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fScreenY );
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

	eax = dwXmax;
	ebx = ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_iScreenY );
	ebx = ebx - 1; // subpixel/clipping reason
	eax = ( (int32_t)eax ) * ( (int32_t)ebx );
	eax = eax + pcBackBuffer;
	dwYOffset1 = eax;

	fpu_reg14 = 1.0f;
	fpu_reg12 = fpu_reg14 / fpu_reg12;

	eax = dwXmax;
	ebx = ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_iScreenY );
	ebx = ebx - 1;
	eax = ( (int32_t)eax ) * ( (int32_t)ebx );
	eax = eax + pcBackBuffer;
	dwYOffset2 = eax;

	fpu_reg14 = 1.0f;
	fpu_reg13 = fpu_reg14 / fpu_reg13;

	eax = ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_iScreenY );
	ebx = ( ((CMV2Dot3DPos *)esi)->CMV2Dot3DPos__m_iScreenY );
	ecx = ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_iScreenY );

	ebp = ebx;
	ebp = ebp - eax;
	dwYCounter = ebp;
	ebp = ecx;
	ebp = ebp - eax;
	dwYCounter1 = ebp;
	ebp = ebx;
	ebp = ebp - ecx;
	dwYCounter2 = ebp;

	// fpu_reg10 = 0.0f;

//st0 = 1/(y3 - y1), st1 = 1/(y2 - y3), st2 = 1/(y2 - y1)

//===>                                                                <===
//===>	Screen Delta Calculation {                                   <===
//===>                                                                <===
//st0 = 1/YCounter1, st1 = 1/YCounter2, st2 = 1/YCounter
	fpu_reg14 = ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fScreenX );
	fpu_reg15 = ( ((CMV2Dot3DPos *)esi)->CMV2Dot3DPos__m_fScreenX );
	fpu_reg16 = ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fScreenX );
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

	fpu_reg17 = ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fScreenYError );
	fpu_reg18 = fpu_reg17;
	fpu_reg18 = fpu_reg18 * fpu_reg14;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg18 = fpu_reg18 * fpu_reg16;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg18 = fpu_reg18 + ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fScreenX );
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg18 = fpu_reg18 + ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fScreenX );
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
//st0 = ScreenX1
//st1 = ScreenX2
//st2 = DeltaScreenX, st3 = DeltaScreenX2
//st4 = DeltaScreenX1, st5 = 1/YCounter1
//st6 = 1/YCounter2, st7 = 1/YCounter

	dwScreenX1 = (int32_t)ceilf(fpu_reg18);

	fpu_reg18 = ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fScreenYError );
	fpu_reg18 = fpu_reg18 * fpu_reg15;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
//st0 = dwScreenX2
//st1 = ScreenYError3*DeltaScreenX2
//st2 = DeltaScreenX, st3 = DeltaScreenX2
//st4 = DeltaScreenX1, st5 = 1/YCounter13
//st6 = 1/YCounter32, st7 = 1/YCounter12

	dwScreenX2 = (int32_t)ceilf(fpu_reg18);

	fpu_reg17 = fpu_reg17 + ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fScreenX );
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
	fpu_reg15 = fpu_reg15 + ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fScreenX );
	fpu_reg15 = fpu_reg15 - ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fScreenX ); //fp dep.
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

	fpu_reg14 = ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fR );
	fpu_reg15 = ( ((CMV2Dot3DPos *)esi)->CMV2Dot3DPos__m_fR );
	fpu_reg16 = ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fR );
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

	fpu_reg17 = ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fScreenYError );
	fpu_reg18 = ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fScreenYError );
	fpu_reg18 = fpu_reg18 * fpu_reg15;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg18 = fpu_reg18 * fpu_reg14;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg18 = fpu_reg18 + ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fR );
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg18 = fpu_reg18 + ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fR );
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

	fpu_reg14 = ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fG );
	fpu_reg15 = ( ((CMV2Dot3DPos *)esi)->CMV2Dot3DPos__m_fG );
	fpu_reg16 = ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fG );
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
	fpu_reg17 = ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fScreenYError );
	fpu_reg18 = ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fScreenYError );
	fpu_reg18 = fpu_reg18 * fpu_reg15;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg18 = fpu_reg18 * fpu_reg14;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg18 = fpu_reg18 + ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fG );
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg18 = fpu_reg18 + ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fG );
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
	fpu_reg14 = ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fB );
	fpu_reg15 = ( ((CMV2Dot3DPos *)esi)->CMV2Dot3DPos__m_fB );
	fpu_reg16 = ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fB );
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
	fpu_reg17 = ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fScreenYError );
	fpu_reg18 = ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fScreenYError );
	fpu_reg18 = fpu_reg18 * fpu_reg15;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg18 = fpu_reg18 * fpu_reg14;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg18 = fpu_reg18 + ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fB );
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg18 = fpu_reg18 + ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fB );
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

	fpu_reg17 = fpu_reg17 + ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fG );
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 + ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fR );
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 + ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fB );
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }

	fpu_reg17 = fpu_reg17 - ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fG );
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 - ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fR );
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 - ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fB );
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

	ebx = dwYCounter;

	if (ebx >= ( dwSpanLength )) goto MV2DrawPolygonTBASM_PerspPT1;

	eax = ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_iScreenX );
	ebx = ( ((CMV2Dot3DPos *)esi)->CMV2Dot3DPos__m_iScreenX );
	ebp = eax;
	ecx = ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_iScreenX );

	eax = eax - ebx; // p1 - p2
	ebx = ebx - ecx; // p2 - p3
	ecx = ecx - ebp; // p3 - p1


	if ( (( (int32_t)eax ) - ( 0 )) >= 0) goto MV2DrawPolygonTBASM_PerspCheck1PT1;
	eax = - ( (int32_t)eax );
MV2DrawPolygonTBASM_PerspCheck1PT1:

	if ( (( (int32_t)ebx ) - ( 0 )) >= 0) goto MV2DrawPolygonTBASM_PerspCheck2PT1;
	ebx = - ( (int32_t)ebx );
MV2DrawPolygonTBASM_PerspCheck2PT1:

	if ( (( (int32_t)ecx ) - ( 0 )) >= 0) goto MV2DrawPolygonTBASM_PerspCheck3PT1;
	ecx = - ( (int32_t)ecx );
MV2DrawPolygonTBASM_PerspCheck3PT1:


	if (eax >= ( dwSpanLength )) goto MV2DrawPolygonTBASM_PerspPT1;


	if (ebx >= ( dwSpanLength )) goto MV2DrawPolygonTBASM_PerspPT1;


	if (ecx >= ( dwSpanLength )) goto MV2DrawPolygonTBASM_PerspPT1;

	goto MV2DrawPolygonTBASM_LinearPolygonType1;
MV2DrawPolygonTBASM_PerspPT1:


	fOneDivZ1NSBC = ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fZNewRZP;
	fOneDivZ2NSBC = ((CMV2Dot3DPos *)esi)->CMV2Dot3DPos__m_fZNewRZP;
	fOneDivZ3NSBC = ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fZNewRZP;

	fScreenYError1 = ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fScreenYError;
	fScreenYError3 = ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fScreenYError;

	edx = pDot1;
	esi = pDot2;
	edi = pDot3;

//===>                                                                <===
//===>	TextureUDivZ DeltaY Calculation {                            <===
//===>                                                                <===
//st0=1/YCounter1, st1=1/YCounter2, st2=1/YCounter

	fpu_reg14 = ( ((CMV2Dot3D *)edx)->CMV2Dot3D__m_fTextureU );
	fpu_reg14 = fpu_reg14 * fOneDivZ1NSBC;
	fpu_reg15 = ( ((CMV2Dot3D *)esi)->CMV2Dot3D__m_fTextureU );
	fpu_reg15 = fpu_reg15 * fOneDivZ2NSBC;
	fpu_reg16 = ( ((CMV2Dot3D *)edi)->CMV2Dot3D__m_fTextureU );
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

	fpu_reg14 = ( ((CMV2Dot3D *)edx)->CMV2Dot3D__m_fTextureV );
	fpu_reg14 = fpu_reg14 * fOneDivZ1NSBC;
	fpu_reg15 = ( ((CMV2Dot3D *)esi)->CMV2Dot3D__m_fTextureV );
	fpu_reg15 = fpu_reg15 * fOneDivZ2NSBC;
	fpu_reg16 = ( ((CMV2Dot3D *)edi)->CMV2Dot3D__m_fTextureV );
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





	eax = dwDeltaScreenX1;
	ebx = dwDeltaScreenX;
	esi = eax;
	edi = ebx;
	eax = eax << ( 12 );
	ebx = ebx << ( 12 );
	esi = ( (int32_t)esi ) >> ( 20 );
	edi = ( (int32_t)edi ) >> ( 20 );
	esi = esi + dwXmax;
	edi = edi + dwXmax;
	dwAdderScreenX1 = esi;
	dwAdderScreenX2 = edi;
	dwAdderScreenX1f = eax;
	dwAdderScreenX2f = ebx;

	eax = dwScreenX1;
	ebx = dwScreenX2;
	esi = eax;
	edi = ebx;
	eax = eax << ( 12 );
	ebx = ebx << ( 12 );
	esi = ( (int32_t)esi ) >> ( 20 );
	edi = ( (int32_t)edi ) >> ( 20 );
	esi = esi + dwYOffset1;
	edi = edi + dwYOffset1;

	ebp = dwYCounter1;
	ecx = edi;

//mov		dwOldESP,esp
//and		esp,0ffffffe0h
	SPTG = (CMV2ScanlinerPerspTG *) mem_alloc_endptr(dwYCounter * sizeof(CMV2ScanlinerPerspTG));
//mov		dwESPStartPoint,esp
	dwESPStartPoint = (uint32_t)SPTG;

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
	eax = eax >> ( 12 );
//sub		esp,CMV2ScanlinerPerspTG__size
	SPTG--;

	dwScreenXf = eax;
	ecx = ecx - esi; // XCounter
	eax = eax << ( 12 );

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
	SPTG->CMV2ScanlinerPerspTG__dwScreenX1Offset = esi;
	SPTG->CMV2ScanlinerPerspTG__dwXCounter = ecx;

	fpu_reg14 = fpu_reg14 + fOneDivZdY1;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg14 = fpu_reg14 + fTextureUDivZdY1;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg14 = fpu_reg14 + fTextureVDivZdY1;
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }

	{ uint32_t carry = (UINT32_MAX - ebx < dwAdderScreenX2f)?1:0; ebx = ebx + dwAdderScreenX2f;
	  edi = edi + dwAdderScreenX2 + carry; }
	{ uint32_t carry = (UINT32_MAX - eax < dwAdderScreenX1f)?1:0; eax = eax + dwAdderScreenX1f;
	  esi = esi + dwAdderScreenX1 + carry; }
	ecx = edi;

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2DrawPolygonTBASM_PerspPT1ScanlinerTGLoopPass1;
//fcompp
//fcomp




//***> TODO: paire the instructions..

	eax = dwDeltaScreenX2;
	esi = eax;
	eax = eax << ( 12 );
	esi = ( (int32_t)esi ) >> ( 20 );
	esi = esi + dwXmax;
	dwAdderScreenX1 = esi;
	dwAdderScreenX1f = eax;

	eax = dwScreenX3;
	esi = eax;
	eax = eax << ( 12 );
	esi = ( (int32_t)esi ) >> ( 20 );
	esi = esi + dwYOffset2;

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
	eax = eax >> ( 12 );
//sub		esp,CMV2ScanlinerPerspTG__size
	SPTG--;

	dwScreenXf = eax;
	ecx = ecx - esi; // XCounter
	eax = eax << ( 12 );

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
	SPTG->CMV2ScanlinerPerspTG__dwScreenX1Offset = esi;
	SPTG->CMV2ScanlinerPerspTG__dwXCounter = ecx;

	fpu_reg14 = fpu_reg14 + fOneDivZdY2;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg14 = fpu_reg14 + fTextureUDivZdY2;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg14 = fpu_reg14 + fTextureVDivZdY2;
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }

	{ uint32_t carry = (UINT32_MAX - ebx < dwAdderScreenX2f)?1:0; ebx = ebx + dwAdderScreenX2f;
	  edi = edi + dwAdderScreenX2 + carry; }
	{ uint32_t carry = (UINT32_MAX - eax < dwAdderScreenX1f)?1:0; eax = eax + dwAdderScreenX1f;
	  esi = esi + dwAdderScreenX1 + carry; }
	ecx = edi;

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2DrawPolygonTBASM_PerspPT1ScanlinerTGLoopPass2;
//fcompp
//fcomp





	edx = dwESPStartPoint;
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
	edx = edx - ( CMV2ScanlinerPerspTG__size );

	fpu_reg15 = ( ((CMV2ScanlinerPerspTG *)edx)->CMV2ScanlinerPerspTG__fScreenXError );
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

	((CMV2ScanlinerPerspTG *)edx)->CMV2ScanlinerPerspTG__dwR = (int32_t)ceilf(fpu_reg17);
	((CMV2ScanlinerPerspTG *)edx)->CMV2ScanlinerPerspTG__dwG = (int32_t)ceilf(fpu_reg16);
	((CMV2ScanlinerPerspTG *)edx)->CMV2ScanlinerPerspTG__dwB = (int32_t)ceilf(fpu_reg15);
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
	edx = edx - ( CMV2ScanlinerPerspTG__size );

	fpu_reg15 = ( ((CMV2ScanlinerPerspTG *)edx)->CMV2ScanlinerPerspTG__fScreenXError );
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

	((CMV2ScanlinerPerspTG *)edx)->CMV2ScanlinerPerspTG__dwR = (int32_t)ceilf(fpu_reg17);
	((CMV2ScanlinerPerspTG *)edx)->CMV2ScanlinerPerspTG__dwG = (int32_t)ceilf(fpu_reg16);
	((CMV2ScanlinerPerspTG *)edx)->CMV2ScanlinerPerspTG__dwB = (int32_t)ceilf(fpu_reg15);
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
	fpu_reg10 = ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fScreenY );
	fpu_reg11 = ( ((CMV2Dot3DPos *)esi)->CMV2Dot3DPos__m_fScreenY );
	fpu_reg12 = ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fScreenY );
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

	eax = dwXmax;
	ebx = ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_iScreenY );
	ebx = ebx - 1;
	eax = ( (int32_t)eax ) * ( (int32_t)ebx );
	eax = eax + pcBackBuffer;
	dwYOffset1 = eax;

	fpu_reg14 = 1.0f;
	fpu_reg12 = fpu_reg14 / fpu_reg12;

	eax = dwXmax;
	ebx = ( ((CMV2Dot3DPos *)esi)->CMV2Dot3DPos__m_iScreenY );
	ebx = ebx - 1;
	eax = ( (int32_t)eax ) * ( (int32_t)ebx );
	eax = eax + pcBackBuffer;
	dwYOffset2 = eax;

	fpu_reg14 = 1.0f;
	fpu_reg13 = fpu_reg14 / fpu_reg13;

	eax = ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_iScreenY );
	ebx = ( ((CMV2Dot3DPos *)esi)->CMV2Dot3DPos__m_iScreenY );
	ecx = ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_iScreenY );

	ebp = ebx;
	ebp = ebp - eax;
	dwYCounter1 = ebp;
	ebp = ecx;
	ebp = ebp - eax;
	dwYCounter = ebp;
	ebp = ecx;
	ebp = ebp - ebx;
	dwYCounter2 = ebp;

	// fpu_reg10 = 0.0f;

//st0 = 1/YCounter, st1 = 1/YCounter2, st2 = 1/YCounter1
//===>                                                                <===
//===>	Screen Delta Calculation {                                   <===
//===>                                                                <===
	fpu_reg14 = ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fScreenX );
	fpu_reg15 = ( ((CMV2Dot3DPos *)esi)->CMV2Dot3DPos__m_fScreenX );
	fpu_reg16 = ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fScreenX );
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

	fpu_reg17 = ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fScreenYError );
	fpu_reg18 = fpu_reg17;
	fpu_reg18 = fpu_reg18 * fpu_reg16;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg18 = fpu_reg18 * fpu_reg14;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg18 = fpu_reg18 + ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fScreenX );
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg18 = fpu_reg18 + ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fScreenX );
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
//st0 = ScreenYError1*DeltaScreenX1 + ScreenX1
//st1 = ScreenYError1*DeltaScreenX + ScreenX1
//st2 = DeltaScreenX1, st3 = DeltaScreenX2
//st4 = DeltaScreenX, st3 = 1/YCounter,
//st4 = 1/YCounter2, st5 = 1/YCounter1
	dwScreenX2 = (int32_t)ceilf(fpu_reg18);

	fpu_reg18 = ( ((CMV2Dot3DPos *)esi)->CMV2Dot3DPos__m_fScreenYError );
	fpu_reg18 = fpu_reg18 * fpu_reg15;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	dwScreenX1 = (int32_t)ceilf(fpu_reg18);
	fpu_reg17 = fpu_reg17 + ( ((CMV2Dot3DPos *)esi)->CMV2Dot3DPos__m_fScreenX );
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	dwDeltaScreenX1 = (int32_t)ceilf(fpu_reg17);
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	dwDeltaScreenX2 = (int32_t)ceilf(fpu_reg16);
	dwScreenX3 = (int32_t)ceilf(fpu_reg15);

//===>	MaxXCounter calculation	{        	                         <===
//st0 = DeltaScreenX
//st1 = 1/YCounter, st2 = 1/YCounter2, st3 = 1/YCounter1
	fpu_reg15 = ( ((CMV2Dot3DPos *)esi)->CMV2Dot3DPos__m_fScreenX );
	fpu_reg16 = fYCounter1;
	fpu_reg16 = fpu_reg16 * fpu_reg14;
	fpu_reg16 = fpu_reg16 + ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fScreenX );
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

	fpu_reg14 = ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fR );
	fpu_reg14 = fpu_reg14 - ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fR );
	fpu_reg15 = ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fG );
	fpu_reg15 = fpu_reg15 - ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fG );
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = fpu_reg15 * fpu_reg13;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = fpu_reg15 * fpu_reg13; // fmul stall (+1 Cycle)
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg16 = ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fScreenYError );
	fpu_reg17 = fpu_reg16;
	fpu_reg17 = fpu_reg17 * fpu_reg15;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 * fpu_reg14; // fmul stall (+1 Cycle)
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
//st0 = ScreenYError1*RdY
//st1 = ScreenYError1*GdY
//st2 = RdY, st3 = GdY
//st4 = 1/YCounter, st5 = 1/YCounter2, st6 = 1/YCounter1

	fpu_reg17 = fpu_reg17 + ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fR );
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 + ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fG );
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }

	fR1 = fpu_reg17;
	fG1 = fpu_reg16;
	fRdY = fpu_reg15;
	fGdY = fpu_reg14;

	fpu_reg14 = ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fB );
	fpu_reg14 = fpu_reg14 - ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fB );
	fpu_reg14 = fpu_reg14 * fpu_reg13;
//st0 = BdY, st1 = 1/YCounter, st2 = 1/YCounter2, st3 = 1/YCounter1

	fpu_reg15 = ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fScreenYError );
	fpu_reg15 = fpu_reg15 * fpu_reg14;
	fpu_reg15 = fpu_reg15 + ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fB );
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

	fpu_reg18 = fpu_reg18 + ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fB );
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg18 = fpu_reg18 + ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fG );
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg18; fpu_reg18 = tmp; }
	fpu_reg18 = fpu_reg18 + ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fR );
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg18; fpu_reg18 = tmp; }

	fpu_reg18 = ( ((CMV2Dot3DPos *)esi)->CMV2Dot3DPos__m_fB ) - fpu_reg18;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg18 = ( ((CMV2Dot3DPos *)esi)->CMV2Dot3DPos__m_fG ) - fpu_reg18;
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg18; fpu_reg18 = tmp; }
	fpu_reg18 = ( ((CMV2Dot3DPos *)esi)->CMV2Dot3DPos__m_fR ) - fpu_reg18;
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
	ebx = dwYCounter;

	if (ebx >= ( dwSpanLength )) goto MV2DrawPolygonTBASM_PerspPT2;

	eax = ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_iScreenX );
	ebx = ( ((CMV2Dot3DPos *)esi)->CMV2Dot3DPos__m_iScreenX );
	ebp = eax;
	ecx = ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_iScreenX );

	eax = eax - ebx; // p1 - p2
	ebx = ebx - ecx; // p2 - p3
	ecx = ecx - ebp; // p3 - p1


	if ( (( (int32_t)eax ) - ( 0 )) >= 0) goto MV2DrawPolygonTBASM_PerspCheck1PT2;
	eax = - ( (int32_t)eax );
MV2DrawPolygonTBASM_PerspCheck1PT2:

	if ( (( (int32_t)ebx ) - ( 0 )) >= 0) goto MV2DrawPolygonTBASM_PerspCheck2PT2;
	ebx = - ( (int32_t)ebx );
MV2DrawPolygonTBASM_PerspCheck2PT2:

	if ( (( (int32_t)ecx ) - ( 0 )) >= 0) goto MV2DrawPolygonTBASM_PerspCheck3PT2;
	ecx = - ( (int32_t)ecx );
MV2DrawPolygonTBASM_PerspCheck3PT2:


	if (eax >= ( dwSpanLength )) goto MV2DrawPolygonTBASM_PerspPT2;


	if (ebx >= ( dwSpanLength )) goto MV2DrawPolygonTBASM_PerspPT2;


	if (ecx >= ( dwSpanLength )) goto MV2DrawPolygonTBASM_PerspPT2;

	goto MV2DrawPolygonTBASM_LinearPolygonType2;
MV2DrawPolygonTBASM_PerspPT2:



	fOneDivZ1NSBC = ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fZNewRZP;
	fOneDivZ2NSBC = ((CMV2Dot3DPos *)esi)->CMV2Dot3DPos__m_fZNewRZP;
	fOneDivZ3NSBC = ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fZNewRZP;

	fScreenYError1 = ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fScreenYError;
	fScreenYError2 = ((CMV2Dot3DPos *)esi)->CMV2Dot3DPos__m_fScreenYError;

	edx = pDot1;
	esi = pDot2;
	edi = pDot3;


//===>                                                                <===
//===>	TextureU&VDivZ DeltaY Calculation {                          <===
//===>                                                                <===
//st0 = 1/YCounter, st1 = 1/YCounter2, st2 = 1/YCounter1

	fpu_reg14 = ( ((CMV2Dot3D *)edx)->CMV2Dot3D__m_fTextureU );
	fpu_reg14 = fpu_reg14 * fOneDivZ1NSBC;
	fpu_reg15 = ( ((CMV2Dot3D *)edi)->CMV2Dot3D__m_fTextureU );
	fpu_reg15 = fpu_reg15 * fOneDivZ3NSBC;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
//st0 = tu1/z1, st1 = tu3/z3
	fTextureUDivZ1NSBC = fpu_reg15;
	fpu_reg14 = fpu_reg14 - fpu_reg15;
//st0 = (tu3/z3 - tu1/z1)
//st1 = 1/YCounter, st2 = 1/YCounter2, st3 = 1/YCounter1

	fpu_reg15 = ( ((CMV2Dot3D *)edx)->CMV2Dot3D__m_fTextureV );
	fpu_reg15 = fpu_reg15 * fOneDivZ1NSBC;
	fpu_reg16 = ( ((CMV2Dot3D *)edi)->CMV2Dot3D__m_fTextureV );
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
	fpu_reg15 = ( ((CMV2Dot3D *)esi)->CMV2Dot3D__m_fTextureU );
	fpu_reg15 = fpu_reg15 * fpu_reg14;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = fpu_reg15 * ( ((CMV2Dot3D *)esi)->CMV2Dot3D__m_fTextureV );
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





	eax = dwDeltaScreenX1;
	ebx = dwDeltaScreenX;
	esi = eax;
	edi = ebx;
	eax = eax << ( 12 );
	ebx = ebx << ( 12 );
	esi = ( (int32_t)esi ) >> ( 20 );
	edi = ( (int32_t)edi ) >> ( 20 );
	esi = esi + dwXmax;
	edi = edi + dwXmax;
	dwAdderScreenX2 = esi;
	dwAdderScreenX1 = edi;
	dwAdderScreenX2f = eax;
	dwAdderScreenX1f = ebx;

	eax = dwScreenX1;
	ebx = dwScreenX2;
	esi = eax;
	edi = ebx;
	eax = eax << ( 12 );
	ebx = ebx << ( 12 );
	esi = ( (int32_t)esi ) >> ( 20 );
	edi = ( (int32_t)edi ) >> ( 20 );
	esi = esi + dwYOffset1;
	edi = edi + dwYOffset1;

	ebp = dwYCounter1;
	ecx = edi;

//mov		dwOldESP,esp
//and		esp,0ffffffe0h
	SPTG = (CMV2ScanlinerPerspTG *) mem_alloc_endptr(dwYCounter * sizeof(CMV2ScanlinerPerspTG));
//mov		dwESPStartPoint,esp
	dwESPStartPoint = (uint32_t)SPTG;

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
	eax = eax >> ( 12 );
//sub		esp,CMV2ScanlinerPerspTG__size
	SPTG--;

	dwScreenXf = eax;
	ecx = ecx - esi; // XCounter
	eax = eax << ( 12 );

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
	SPTG->CMV2ScanlinerPerspTG__dwScreenX1Offset = esi;
	SPTG->CMV2ScanlinerPerspTG__dwXCounter = ecx;

	fpu_reg14 = fpu_reg14 + fOneDivZdY;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg14 = fpu_reg14 + fTextureUDivZdY;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg14 = fpu_reg14 + fTextureVDivZdY;
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }

	{ uint32_t carry = (UINT32_MAX - ebx < dwAdderScreenX2f)?1:0; ebx = ebx + dwAdderScreenX2f;
	  edi = edi + dwAdderScreenX2 + carry; }
	{ uint32_t carry = (UINT32_MAX - eax < dwAdderScreenX1f)?1:0; eax = eax + dwAdderScreenX1f;
	  esi = esi + dwAdderScreenX1 + carry; }
	ecx = edi;

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2DrawPolygonTBASM_PerspPT2ScanlinerTGLoopPass1;

//***> TODO: paire the instructions..
	ebx = dwDeltaScreenX2;
	edi = ebx;
	ebx = ebx << ( 12 );
	edi = ( (int32_t)edi ) >> ( 20 );
	edi = edi + dwXmax;
	dwAdderScreenX2 = edi;
	dwAdderScreenX2f = ebx;

	ebx = dwScreenX3;
	edi = ebx;
	ebx = ebx << ( 12 );
	edi = ( (int32_t)edi ) >> ( 20 );
	edi = edi + dwYOffset2;

	ebp = dwYCounter2;
	ecx = edi;

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
	eax = eax >> ( 12 );
//sub		esp,CMV2ScanlinerPerspTG__size
	SPTG--;

	dwScreenXf = eax;
	ecx = ecx - esi; // XCounter
	eax = eax << ( 12 );

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
	SPTG->CMV2ScanlinerPerspTG__dwScreenX1Offset = esi;
	SPTG->CMV2ScanlinerPerspTG__dwXCounter = ecx;

	fpu_reg14 = fpu_reg14 + fOneDivZdY;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg14 = fpu_reg14 + fTextureUDivZdY;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg14 = fpu_reg14 + fTextureVDivZdY;
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }

	{ uint32_t carry = (UINT32_MAX - ebx < dwAdderScreenX2f)?1:0; ebx = ebx + dwAdderScreenX2f;
	  edi = edi + dwAdderScreenX2 + carry; }
	{ uint32_t carry = (UINT32_MAX - eax < dwAdderScreenX1f)?1:0; eax = eax + dwAdderScreenX1f;
	  esi = esi + dwAdderScreenX1 + carry; }
	ecx = edi;

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2DrawPolygonTBASM_PerspPT2ScanlinerTGLoopPass2;
//fcompp
//fcomp




	edx = dwESPStartPoint;
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
	edx = edx - ( CMV2ScanlinerPerspTG__size );

	fpu_reg15 = ( ((CMV2ScanlinerPerspTG *)edx)->CMV2ScanlinerPerspTG__fScreenXError );
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

	((CMV2ScanlinerPerspTG *)edx)->CMV2ScanlinerPerspTG__dwR = (int32_t)ceilf(fpu_reg17);
	((CMV2ScanlinerPerspTG *)edx)->CMV2ScanlinerPerspTG__dwG = (int32_t)ceilf(fpu_reg16);
	((CMV2ScanlinerPerspTG *)edx)->CMV2ScanlinerPerspTG__dwB = (int32_t)ceilf(fpu_reg15);
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

	fpu_reg10 = ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fScreenY );
	fpu_reg11 = ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fScreenY );
	fpu_reg12 = ( ((CMV2Dot3DPos *)esi)->CMV2Dot3DPos__m_fScreenY );
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

	eax = dwXmax;
	ebx = ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_iScreenY );
	ebx = ebx - 1;
	eax = ( (int32_t)eax ) * ( (int32_t)ebx );
	eax = eax + pcBackBuffer;
	dwYOffset1 = eax;

	fpu_reg10 = fpu_reg12 / fpu_reg10;
//st0 = 1/(y2 - y1), st1 = 1/(y3 - y1)

	eax = ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_iScreenY );
	eax = eax - ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_iScreenY );
	dwYCounter = eax;

//===>                                                                <===
//===>	Screen Delta Calculation {                                   <===
//===>                                                                <===
//st0 = 1/YCounter2, st1 = 1/YCounter1

	fpu_reg12 = ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fScreenX );
	fpu_reg13 = ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fScreenX );
	fpu_reg14 = ( ((CMV2Dot3DPos *)esi)->CMV2Dot3DPos__m_fScreenX );
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

	fpu_reg14 = ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fScreenYError );
	fpu_reg15 = fpu_reg14;
	fpu_reg15 = fpu_reg15 * fpu_reg12;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = fpu_reg15 * fpu_reg13;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = fpu_reg15 + ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fScreenX );
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = fpu_reg15 + ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fScreenX );
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
	fpu_reg13 = fpu_reg13 + ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fScreenX );
	fpu_reg13 = ( ((CMV2Dot3DPos *)esi)->CMV2Dot3DPos__m_fScreenX ) - fpu_reg13; //fp dep.
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

	fpu_reg12 = ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fR );
	fpu_reg12 = fpu_reg12 - ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fR );
	fpu_reg13 = ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fG );
	fpu_reg13 = fpu_reg13 - ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fG );
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
	fpu_reg13 = fpu_reg13 * fpu_reg10;
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
	fpu_reg13 = fpu_reg13 * fpu_reg10; // fmul stall (+1 Cycle)
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
	fpu_reg14 = ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fScreenYError );
	fpu_reg15 = fpu_reg14;
	fpu_reg15 = fpu_reg15 * fpu_reg13;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = fpu_reg15 * fpu_reg12; // fmul stall (+1 Cycle)
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
//st0 = ScreenYError1*RdY
//st1 = ScreenYError1*GdY
//st2 = RdY, st3 = GdY
//st4 = 1/YCounter2, st5 = 1/YCounter1

	fpu_reg15 = fpu_reg15 + ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fR );
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = fpu_reg15 + ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fG );
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }

	fR1 = fpu_reg15;
	fG1 = fpu_reg14;
	fRdY1 = fpu_reg13;
	fGdY1 = fpu_reg12;

	fpu_reg12 = ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fB );
	fpu_reg12 = fpu_reg12 - ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fB );
	fpu_reg12 = fpu_reg12 * fpu_reg10;
//st0 = BdY, st1 = 1/YCounter2, st2 = 1/YCounter1

	fpu_reg13 = ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fScreenYError );
	fpu_reg13 = fpu_reg13 * fpu_reg12;
	fpu_reg13 = fpu_reg13 + ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fB );
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

	fpu_reg16 = fpu_reg16 + ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fB );
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = fpu_reg16 + ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fG );
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg16 = fpu_reg16 + ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fR );
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg16; fpu_reg16 = tmp; }

	fpu_reg16 = ( ((CMV2Dot3DPos *)esi)->CMV2Dot3DPos__m_fB ) - fpu_reg16;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = ( ((CMV2Dot3DPos *)esi)->CMV2Dot3DPos__m_fG ) - fpu_reg16;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg16 = ( ((CMV2Dot3DPos *)esi)->CMV2Dot3DPos__m_fR ) - fpu_reg16;
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
	ebx = dwYCounter;

	if (ebx >= ( dwSpanLength )) goto MV2DrawPolygonTBASM_PerspPT3;

	eax = ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_iScreenX );
	ebx = ( ((CMV2Dot3DPos *)esi)->CMV2Dot3DPos__m_iScreenX );
	ebp = eax;
	ecx = ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_iScreenX );

	eax = eax - ebx; // p1 - p2
	ebx = ebx - ecx; // p2 - p3
	ecx = ecx - ebp; // p3 - p1


	if ( (( (int32_t)eax ) - ( 0 )) >= 0) goto MV2DrawPolygonTBASM_PerspCheck1PT3;
	eax = - ( (int32_t)eax );
MV2DrawPolygonTBASM_PerspCheck1PT3:

	if ( (( (int32_t)ebx ) - ( 0 )) >= 0) goto MV2DrawPolygonTBASM_PerspCheck2PT3;
	ebx = - ( (int32_t)ebx );
MV2DrawPolygonTBASM_PerspCheck2PT3:

	if ( (( (int32_t)ecx ) - ( 0 )) >= 0) goto MV2DrawPolygonTBASM_PerspCheck3PT3;
	ecx = - ( (int32_t)ecx );
MV2DrawPolygonTBASM_PerspCheck3PT3:


	if (eax >= ( dwSpanLength )) goto MV2DrawPolygonTBASM_PerspPT3;


	if (ebx >= ( dwSpanLength )) goto MV2DrawPolygonTBASM_PerspPT3;


	if (ecx >= ( dwSpanLength )) goto MV2DrawPolygonTBASM_PerspPT3;

	goto MV2DrawPolygonTBASM_LinearPolygonType3;
MV2DrawPolygonTBASM_PerspPT3:



	fOneDivZ1NSBC = ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fZNewRZP;
	fOneDivZ2NSBC = ((CMV2Dot3DPos *)esi)->CMV2Dot3DPos__m_fZNewRZP;
	fOneDivZ3NSBC = ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fZNewRZP;

	fScreenYError1 = ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fScreenYError;
	fScreenYError2 = ((CMV2Dot3DPos *)esi)->CMV2Dot3DPos__m_fScreenYError;

	edx = pDot1;
	esi = pDot2;
	edi = pDot3;


//===>                                                                <===
//===>	TextureU&VDivZ DeltaY Calculation {                          <===
//===>                                                                <===
//st0 = 1/YCounter2, st1 = 1/YCounter1

	fpu_reg12 = ( ((CMV2Dot3D *)edx)->CMV2Dot3D__m_fTextureU );
	fpu_reg12 = fpu_reg12 * fOneDivZ1NSBC;
	fpu_reg13 = ( ((CMV2Dot3D *)edi)->CMV2Dot3D__m_fTextureU );
	fpu_reg13 = fpu_reg13 * fOneDivZ3NSBC;
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
//st0 = tu1/z1, st1 = tu3/z3
	fTextureUDivZ1NSBC = fpu_reg13;
	fpu_reg12 = fpu_reg12 - fpu_reg13;
//st0 = (tu3/z3 - tu1/z1)
//st1 = 1/YCounter2, st2 = 1/YCounter1

	fpu_reg13 = ( ((CMV2Dot3D *)edx)->CMV2Dot3D__m_fTextureV );
	fpu_reg13 = fpu_reg13 * fOneDivZ1NSBC;
	fpu_reg14 = ( ((CMV2Dot3D *)edi)->CMV2Dot3D__m_fTextureV );
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
	fpu_reg13 = ( ((CMV2Dot3D *)esi)->CMV2Dot3D__m_fTextureU );
	fpu_reg13 = fpu_reg13 * fpu_reg12;
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
	fpu_reg13 = fpu_reg13 * ( ((CMV2Dot3D *)esi)->CMV2Dot3D__m_fTextureV );
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



	eax = dwDeltaScreenX1;
	ebx = dwDeltaScreenX2;
	esi = eax;
	edi = ebx;
	eax = eax << ( 12 );
	ebx = ebx << ( 12 );
	esi = ( (int32_t)esi ) >> ( 20 );
	edi = ( (int32_t)edi ) >> ( 20 );
	esi = esi + dwXmax;
	edi = edi + dwXmax;
	dwAdderScreenX1 = esi;
	dwAdderScreenX2 = edi;
	dwAdderScreenX1f = eax;
	dwAdderScreenX2f = ebx;

	eax = dwScreenX1;
	ebx = dwScreenX2;
	esi = eax;
	edi = ebx;
	eax = eax << ( 12 );
	ebx = ebx << ( 12 );
	esi = ( (int32_t)esi ) >> ( 20 );
	edi = ( (int32_t)edi ) >> ( 20 );
	esi = esi + dwYOffset1;
	edi = edi + dwYOffset1;

	ebp = dwYCounter;
	ecx = edi;

//mov		dwOldESP,esp
//and		esp,0ffffffe0h
	SPTG = (CMV2ScanlinerPerspTG *) mem_alloc_endptr(dwYCounter * sizeof(CMV2ScanlinerPerspTG));
//mov		dwESPStartPoint,esp
	dwESPStartPoint = (uint32_t)SPTG;

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
	eax = eax >> ( 12 );
//sub		esp,CMV2ScanlinerPerspTG__size
	SPTG--;

	dwScreenXf = eax;
	ecx = ecx - esi; // XCounter
	eax = eax << ( 12 );

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
	SPTG->CMV2ScanlinerPerspTG__dwScreenX1Offset = esi;
	SPTG->CMV2ScanlinerPerspTG__dwXCounter = ecx;

	fpu_reg14 = fpu_reg14 + fOneDivZdY1;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg14 = fpu_reg14 + fTextureUDivZdY1;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg14 = fpu_reg14 + fTextureVDivZdY1;
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }

	{ uint32_t carry = (UINT32_MAX - ebx < dwAdderScreenX2f)?1:0; ebx = ebx + dwAdderScreenX2f;
	  edi = edi + dwAdderScreenX2 + carry; }
	{ uint32_t carry = (UINT32_MAX - eax < dwAdderScreenX1f)?1:0; eax = eax + dwAdderScreenX1f;
	  esi = esi + dwAdderScreenX1 + carry; }
	ecx = edi;

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2DrawPolygonTBASM_PerspPT3ScanlinerTGLoop;
//fcompp
//fcomp




	edx = dwESPStartPoint;
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
	edx = edx - ( CMV2ScanlinerPerspTG__size );

	fpu_reg15 = ( ((CMV2ScanlinerPerspTG *)edx)->CMV2ScanlinerPerspTG__fScreenXError );
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

	((CMV2ScanlinerPerspTG *)edx)->CMV2ScanlinerPerspTG__dwR = (int32_t)ceilf(fpu_reg17);
	((CMV2ScanlinerPerspTG *)edx)->CMV2ScanlinerPerspTG__dwG = (int32_t)ceilf(fpu_reg16);
	((CMV2ScanlinerPerspTG *)edx)->CMV2ScanlinerPerspTG__dwB = (int32_t)ceilf(fpu_reg15);
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

	eax = ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_iScreenY );
	eax = eax - ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_iScreenY );

	if (( (int32_t)eax ) <= 0) goto MV2DrawPolygonTBASM_PolygonNotVisible;

	dwYCounter = eax;

	fpu_reg10 = ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fScreenY );
	fpu_reg11 = ( ((CMV2Dot3DPos *)esi)->CMV2Dot3DPos__m_fScreenY );
	fpu_reg12 = ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fScreenY );
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

	eax = dwXmax;
	ebx = ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_iScreenY );
	ebx = ebx - 1;
	eax = ( (int32_t)eax ) * ( (int32_t)ebx );
	eax = eax + pcBackBuffer;
	dwYOffset1 = eax;

	fpu_reg13 = 1.0f;
	fpu_reg12 = fpu_reg13 / fpu_reg12;
	// fpu_reg10 = 0.0f;

//===>                                                                <===
//===>	Screen Delta Calculation {                                   <===
//===>                                                                <===
//st0 = 1/YCounter2, st1 = 1/YCounter1

	fpu_reg13 = ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fScreenX );
	fpu_reg14 = ( ((CMV2Dot3DPos *)esi)->CMV2Dot3DPos__m_fScreenX );
	fpu_reg15 = ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fScreenX );
//st0 = (x3), st1 = (x2), st2 = (x1)
//st3 = 1/YCounter2, st4 = 1/YCounter1

	fpu_reg13 = fpu_reg15 - fpu_reg13;
	fpu_reg14 = fpu_reg15 - fpu_reg14;
	fpu_reg14 = fpu_reg14 * fpu_reg12;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg14 = fpu_reg14 * fpu_reg11; // fmul stall (+1 Cycle)
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg15 = ( ((CMV2Dot3DPos *)esi)->CMV2Dot3DPos__m_fScreenYError );
	fpu_reg16 = ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fScreenYError );
//st0 = ScreenYError1, st1 = ScreenYError2
//st2 = DeltaScreenX2, st3 = DeltaScreenX1
//st4 = 1/YCounter2, st5 = 1/YCounter1

	fpu_reg16 = fpu_reg16 * fpu_reg13;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = fpu_reg16 * fpu_reg14;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = fpu_reg16 + ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fScreenX );
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = fpu_reg16 + ( ((CMV2Dot3DPos *)esi)->CMV2Dot3DPos__m_fScreenX );
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

	fpu_reg14 = fpu_reg14 + ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fScreenX );
	fpu_reg14 = ( ((CMV2Dot3DPos *)esi)->CMV2Dot3DPos__m_fScreenX ) - fpu_reg14; //fp dep.
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

	fpu_reg13 = ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fR );
	fpu_reg13 = fpu_reg13 - ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fR );
	fpu_reg14 = ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fG );
	fpu_reg14 = fpu_reg14 - ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fG );
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg14 = fpu_reg14 * fpu_reg11;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg14 = fpu_reg14 * fpu_reg11; // fmul stall (+1 Cycle)
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg15 = ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fScreenYError );
	fpu_reg16 = fpu_reg15;
	fpu_reg16 = fpu_reg16 * fpu_reg14;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = fpu_reg16 * fpu_reg13; // fmul stall (+1 Cycle)
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
//st0 = ScreenYError1*RdY
//st1 = ScreenYError1*GdY
//st2 = RdY, st3 = GdY
//st4 = 1/YCounter2, st5 = 1/YCounter1

	fpu_reg16 = fpu_reg16 + ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fR );
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = fpu_reg16 + ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fG );
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }

	fR1 = fpu_reg16;
	fG1 = fpu_reg15;
	fRdY1 = fpu_reg14;
	fGdY1 = fpu_reg13;

	fpu_reg13 = ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fB );
	fpu_reg13 = fpu_reg13 - ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fB );
	fpu_reg13 = fpu_reg13 * fpu_reg11;
//st0 = BdY, st1 = 1/YCounter2, st2 = 1/YCounter1

	fpu_reg14 = ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fScreenYError );
	fpu_reg14 = fpu_reg14 * fpu_reg13;
	fpu_reg14 = fpu_reg14 + ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fB );
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

	fpu_reg17 = fpu_reg17 + ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fB );
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 + ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fG );
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 + ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fR );
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }

	fpu_reg17 = ( ((CMV2Dot3DPos *)esi)->CMV2Dot3DPos__m_fB ) - fpu_reg17;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = ( ((CMV2Dot3DPos *)esi)->CMV2Dot3DPos__m_fG ) - fpu_reg17;
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = ( ((CMV2Dot3DPos *)esi)->CMV2Dot3DPos__m_fR ) - fpu_reg17;
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
	ebx = dwYCounter;

	if (ebx >= ( dwSpanLength )) goto MV2DrawPolygonTBASM_PerspPT4;

	eax = ( ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_iScreenX );
	ebx = ( ((CMV2Dot3DPos *)esi)->CMV2Dot3DPos__m_iScreenX );
	ebp = eax;
	ecx = ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_iScreenX );

	eax = eax - ebx; // p1 - p2
	ebx = ebx - ecx; // p2 - p3
	ecx = ecx - ebp; // p3 - p1


	if ( (( (int32_t)eax ) - ( 0 )) >= 0) goto MV2DrawPolygonTBASM_PerspCheck1PT4;
	eax = - ( (int32_t)eax );
MV2DrawPolygonTBASM_PerspCheck1PT4:

	if ( (( (int32_t)ebx ) - ( 0 )) >= 0) goto MV2DrawPolygonTBASM_PerspCheck2PT4;
	ebx = - ( (int32_t)ebx );
MV2DrawPolygonTBASM_PerspCheck2PT4:

	if ( (( (int32_t)ecx ) - ( 0 )) >= 0) goto MV2DrawPolygonTBASM_PerspCheck3PT4;
	ecx = - ( (int32_t)ecx );
MV2DrawPolygonTBASM_PerspCheck3PT4:


	if (eax >= ( dwSpanLength )) goto MV2DrawPolygonTBASM_PerspPT4;


	if (ebx >= ( dwSpanLength )) goto MV2DrawPolygonTBASM_PerspPT4;


	if (ecx >= ( dwSpanLength )) goto MV2DrawPolygonTBASM_PerspPT4;

	goto MV2DrawPolygonTBASM_LinearPolygonType4;
MV2DrawPolygonTBASM_PerspPT4:


	fOneDivZ1NSBC = ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fZNewRZP;
	fOneDivZ2NSBC = ((CMV2Dot3DPos *)esi)->CMV2Dot3DPos__m_fZNewRZP;
	fOneDivZ3NSBC = ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fZNewRZP;

	fScreenYError1 = ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fScreenYError;
	fScreenYError2 = ((CMV2Dot3DPos *)esi)->CMV2Dot3DPos__m_fScreenYError;

	edx = pDot1;
	esi = pDot2;
	edi = pDot3;



//===>                                                                <===
//===>	TextureU&VDivZ DeltaY Calculation {                          <===
//===>                                                                <===
//st0 = 1/YCounter2, st1 = 1/YCounter1


	fpu_reg13 = ( ((CMV2Dot3D *)edx)->CMV2Dot3D__m_fTextureU );
	fpu_reg13 = fpu_reg13 * fOneDivZ1NSBC;
	fpu_reg14 = ( ((CMV2Dot3D *)edi)->CMV2Dot3D__m_fTextureU );
	fpu_reg14 = fpu_reg14 * fOneDivZ3NSBC;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
//st0 = tu1/z1, st1 = tu3/z3
	fTextureUDivZ1NSBC = fpu_reg14;
	fpu_reg13 = fpu_reg13 - fpu_reg14;
//st0 = (tu3/z3 - tu1/z1)
//st1 = 1/YCounter2, st2 = 1/YCounter1

	fpu_reg14 = ( ((CMV2Dot3D *)edx)->CMV2Dot3D__m_fTextureV );
	fpu_reg14 = fpu_reg14 * fOneDivZ1NSBC;
	fpu_reg15 = ( ((CMV2Dot3D *)edi)->CMV2Dot3D__m_fTextureV );
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
	fpu_reg14 = ( ((CMV2Dot3D *)esi)->CMV2Dot3D__m_fTextureU );
	fpu_reg14 = fpu_reg14 * fpu_reg13;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg14 = fpu_reg14 * ( ((CMV2Dot3D *)esi)->CMV2Dot3D__m_fTextureV );
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




	eax = dwDeltaScreenX1;
	ebx = dwDeltaScreenX2;
	esi = eax;
	edi = ebx;
	eax = eax << ( 12 );
	ebx = ebx << ( 12 );
	esi = ( (int32_t)esi ) >> ( 20 );
	edi = ( (int32_t)edi ) >> ( 20 );
	esi = esi + dwXmax;
	edi = edi + dwXmax;
	dwAdderScreenX1 = esi;
	dwAdderScreenX2 = edi;
	dwAdderScreenX1f = eax;
	dwAdderScreenX2f = ebx;

	eax = dwScreenX1;
	ebx = dwScreenX2;
	esi = eax;
	edi = ebx;
	eax = eax << ( 12 );
	ebx = ebx << ( 12 );
	esi = ( (int32_t)esi ) >> ( 20 );
	edi = ( (int32_t)edi ) >> ( 20 );
	esi = esi + dwYOffset1;
	edi = edi + dwYOffset1;

	ebp = dwYCounter;
	ecx = edi;

//mov		dwOldESP,esp
//and		esp,0ffffffe0h
	SPTG = (CMV2ScanlinerPerspTG *) mem_alloc_endptr(dwYCounter * sizeof(CMV2ScanlinerPerspTG));
//mov		dwESPStartPoint,esp
	dwESPStartPoint = (uint32_t)SPTG;

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
	eax = eax >> ( 12 );
//sub		esp,CMV2ScanlinerPerspTG__size
	SPTG--;

	dwScreenXf = eax;
	ecx = ecx - esi; // XCounter
	eax = eax << ( 12 );

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
	SPTG->CMV2ScanlinerPerspTG__dwScreenX1Offset = esi;
	SPTG->CMV2ScanlinerPerspTG__dwXCounter = ecx;

	fpu_reg14 = fpu_reg14 + fOneDivZdY1;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg14 = fpu_reg14 + fTextureUDivZdY1;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg14 = fpu_reg14 + fTextureVDivZdY1;
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }

	{ uint32_t carry = (UINT32_MAX - ebx < dwAdderScreenX2f)?1:0; ebx = ebx + dwAdderScreenX2f;
	  edi = edi + dwAdderScreenX2 + carry; }
	{ uint32_t carry = (UINT32_MAX - eax < dwAdderScreenX1f)?1:0; eax = eax + dwAdderScreenX1f;
	  esi = esi + dwAdderScreenX1 + carry; }
	ecx = edi;

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2DrawPolygonTBASM_PerspPT4ScanlinerTGLoop;
//fcompp
//fcomp




	edx = dwESPStartPoint;
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
	edx = edx - ( CMV2ScanlinerPerspTG__size );

	fpu_reg15 = ( ((CMV2ScanlinerPerspTG *)edx)->CMV2ScanlinerPerspTG__fScreenXError );
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

	((CMV2ScanlinerPerspTG *)edx)->CMV2ScanlinerPerspTG__dwR = (int32_t)ceilf(fpu_reg17);
	((CMV2ScanlinerPerspTG *)edx)->CMV2ScanlinerPerspTG__dwG = (int32_t)ceilf(fpu_reg16);
	((CMV2ScanlinerPerspTG *)edx)->CMV2ScanlinerPerspTG__dwB = (int32_t)ceilf(fpu_reg15);
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

	edx = dwRdX;
	ecx = edx;
	edx = edx << ( 8 );
	ecx = ecx << ( 24 );
	edx = edx & ( 0x000ff0000 );
	dwRGBIntLoop_ebp = ecx;
	dwRGBIntLoop_esi = edx;

	ecx = dwGdX;
	edx = dwBdX;
	eax = 0;
	ebx = 0;
	eax = (eax & 0xffffff00) | (uint8_t)(( (uint8_t)edx ));
	ebx = (ebx & 0xffffff00) | (uint8_t)(( (uint8_t)(edx >> 8) ));
	eax = set_high_byte(eax, ( (uint8_t)ecx ));
	ebx = set_high_byte(ebx, ( (uint8_t)(ecx >> 8) ));
	dwRGBIntLoop_eax = eax;
	dwRGBIntLoop_ecx = ebx;


MV2DrawPolygonTBASM_PerspTGYLoop:
//mov		ecx,ss:[esp].CMV2ScanlinerPerspTG__dwXCounter
	ecx = SPTG->CMV2ScanlinerPerspTG__dwXCounter;
//	mov		edi,ss:[esp].CMV2ScanlinerPerspTG__dwScreenX1Offset
//	mov		edi,offset pBackTextureBuffer
	edi = 0;

	iPixelCounter = iPixelCounter + ecx; //***********

// ecx = XCounter

	if (( (int32_t)ecx ) <= 0) goto MV2DrawPolygonTBASM_PerspTGNoXLoop; //***********
//	jz		@@PerspTGNoXLoop

	eax = ecx;
	dwScanlineXCounter = ecx;


	if (( (int32_t)ecx ) <= ( dwSpanLength )) goto MV2DrawPolygonTBASM_PerspTGFirstRemnantSpan;
	ecx = ( dwSpanLength );
MV2DrawPolygonTBASM_PerspTGFirstRemnantSpan:

	eax = eax - ( dwSpanLength );
	dwNextXCounter = ecx;

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
	fpu_reg16 = fpu_reg16 * ( fSpanLengthFactorsRZP[ecx] );
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg16 = fpu_reg16 * ( fSpanLengthFactorsRZP[ecx] );
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }

//st0 = (VR - VL)/SpanLength, st1 = (UR - UL)/SpanLength
//st2 = UR, st3 = VR, st4 = UR/ZR, st5 = VR/ZR, st6 = 1/ZR

	dwTextureVdX = (int32_t)ceilf(fpu_reg16);
	dwTextureUdX = (int32_t)ceilf(fpu_reg15);
//st0 = UR, st1 = VR, st2 = UR/ZR, st3 = VR/ZR, st4 = 1/ZR
//right becomes left, left will be calculated in folowing lines..
//st0 = UL, st1 = VL, st2 = UL/ZL, st3 = VL/ZL, st4 = 1/ZL

//ecx = dwNextXCounter ( = CurXCounter )



	if (( (int32_t)eax ) <= 0) goto MV2DrawPolygonTBASM_PerspTGDoRemnantPixel;
	ecx = eax;


	if (( (int32_t)ecx ) <= ( dwSpanLength )) goto MV2DrawPolygonTBASM_PerspTGSecondRemnantSpan;
	ecx = ( dwSpanLength );
MV2DrawPolygonTBASM_PerspTGSecondRemnantSpan:

	ebx = dwNextXCounter;
	eax = eax - ( dwSpanLength );
	dwCurXCounter = ebx;
	dwNextXCounter = ecx;
	dwScanlineXCounter = eax;

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

	ebx = dwTextureUdX;
	eax = dwTextureVdX;
	ebp = ebx;
	esi = eax;
	ebx = ebx >> ( 16 ); // bl init.
	eax = eax >> ( 16 );
	esi = esi << ( 16 ); // esi init.
	ebp = ebp << ( 16 ); // ebp init.
	ebx = set_high_byte(ebx, ( (uint8_t)eax )); // bh init.
	eax = dwTextureU;
	ecx = dwTextureV;
	edx = eax;
	eax = eax << ( 16 );
	edx = edx >> ( 16 ); // dl init.
	ebx = ebx | eax; // ebx init.
	eax = ecx;
	ecx = ecx << ( 16 ); // hi ecx init.
	eax = eax >> ( 16 );
	ecx = ecx | dwCurXCounter; // ecx init.
	edx = set_high_byte(edx, ( (uint8_t)eax )); // dh init.
	edx = edx | pcTexture; // edx init.

MV2DrawPolygonTBASM_PerspTGXLoop:
// eax = col
// ebx = txf			| dty 	| dtX
// ecx = tyf			|		| XCounter
// edx = pTexture	| ty	| tx
// esi = dtyf
// edi =	destination
// ebp = dtxf

	eax = edx;
	edx = edx & ( 0x0ffff );
	edx = edx + pcBumpmap;
	*((uint32_t *)(((uint32_t)&(pBackBumpBuffer[0])) + edi)) = edx; // ->pBackBumpBuffer
	edx = eax;

	eax = ( *((uint32_t *)(edx * 4)) );
	*((uint32_t *)(((uint32_t)&(pBackTextureBuffer[0])) + edi)) = eax;

	{ uint32_t carry = (UINT32_MAX - ebx < ebp)?1:0; ebx = ebx + ebp;
	  edx = (edx & 0xffffff00) | (uint8_t)(( (uint8_t)edx ) + ( (uint8_t)ebx ) + carry); }
	{ uint32_t carry = (UINT32_MAX - ecx < esi)?1:0; ecx = ecx + esi;
	  edx = set_high_byte(edx, ( (uint8_t)(edx >> 8) ) + ( (uint8_t)(ebx >> 8) ) + carry); }
	edi = edi + ( 4 );

	ecx = (ecx & 0xffffff00) | (uint8_t)(( (int8_t)ecx ) - 1);
	if (( (int8_t)ecx ) != 0) goto MV2DrawPolygonTBASM_PerspTGXLoop;

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

	eax = dwNextXCounter;
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg16 = fpu_reg16 * ( fSpanLengthFactorsRZP[eax] );
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg16 = fpu_reg16 * ( fSpanLengthFactorsRZP[eax] );
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
//st0 = (VR - VL)/SpanLength, st1 = (UR - UL)/SpanLength
//st2 = UR, st3 = VR, st4 = UR/ZR, st5 = VR/ZR, st6 = 1/ZR

	dwTextureVdX = (int32_t)ceilf(fpu_reg16);
	dwTextureUdX = (int32_t)ceilf(fpu_reg15);
//st0 = UR, st1 = VR, st2 = UR/ZR, st3 = VR/ZR, st4 = 1/ZR
//right becomes left, left will be calculated in folowing lines..

//st0 = UL, st1 = VL, st2 = UL/ZL, st3 = VL/ZL, st4 = 1/ZL


	if (( (int32_t)dwScanlineXCounter ) <= ( 0 )) goto MV2DrawPolygonTBASM_PerspTGDoRemnantPixel;

	ecx = dwScanlineXCounter;
	eax = ecx;

	if (( (int32_t)ecx ) <= ( dwSpanLength )) goto MV2DrawPolygonTBASM_PerspTGRemnantSpanFollows;
	ecx = ( dwSpanLength );
MV2DrawPolygonTBASM_PerspTGRemnantSpanFollows:

	ebx = dwNextXCounter;
	eax = eax - ( dwSpanLength );

	dwCurXCounter = ebx;
	dwNextXCounter = ecx;
	dwScanlineXCounter = eax;


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
	ebx = dwTextureUdX;
	eax = dwTextureVdX;
	ebp = ebx;
	esi = eax;
	ebx = ebx >> ( 16 ); // bl init.
	eax = eax >> ( 16 );
	esi = esi << ( 16 ); // esi init.
	ebp = ebp << ( 16 ); // ebp init.
	ebx = set_high_byte(ebx, ( (uint8_t)eax )); // bh init.
	eax = dwTextureU;
	ecx = dwTextureV;
	edx = eax;
	eax = eax << ( 16 );
	edx = edx >> ( 16 ); // dl init.
	ebx = ebx | eax; // ebx init.
	eax = ecx;
	ecx = ecx << ( 16 ); // hi ecx init.
	eax = eax >> ( 16 );
	ecx = ecx | dwNextXCounter; // ecx init.
	edx = set_high_byte(edx, ( (uint8_t)eax )); // dh init.
	edx = edx | pcTexture; // edx init.

MV2DrawPolygonTBASM_PerspTGRemnantPixelXLoop:
// eax = col
// ebx = txf			| dty 	| dtX
// ecx = tyf			|		| XCounter
// edx = pTexture	| ty	| tx
// esi = dtyf
// edi =	destination
// ebp = dtxf

	eax = edx;
	edx = edx & ( 0x0ffff );
	edx = edx + pcBumpmap;
	*((uint32_t *)(((uint32_t)&(pBackBumpBuffer[0])) + edi)) = edx; // ->pBackBumpBuffer
	edx = eax;

	eax = ( *((uint32_t *)(edx * 4)) );
	*((uint32_t *)(((uint32_t)&(pBackTextureBuffer[0])) + edi)) = eax;

	{ uint32_t carry = (UINT32_MAX - ebx < ebp)?1:0; ebx = ebx + ebp;
	  edx = (edx & 0xffffff00) | (uint8_t)(( (uint8_t)edx ) + ( (uint8_t)ebx ) + carry); }
	{ uint32_t carry = (UINT32_MAX - ecx < esi)?1:0; ecx = ecx + esi;
	  edx = set_high_byte(edx, ( (uint8_t)(edx >> 8) ) + ( (uint8_t)(ebx >> 8) ) + carry); }
	edi = edi + ( 4 );

	ecx = (ecx & 0xffffff00) | (uint8_t)(( (int8_t)ecx ) - 1);
	if (( (int8_t)ecx ) != 0) goto MV2DrawPolygonTBASM_PerspTGRemnantPixelXLoop;


//mov		ebp,ss:[esp].CMV2ScanlinerPerspTG__dwR
//mov		edx,ss:[esp].CMV2ScanlinerPerspTG__dwG
//mov     esi,ss:[esp].CMV2ScanlinerPerspTG__dwB
//mov		ebx,ss:[esp].CMV2ScanlinerPerspTG__dwXCounter
	ebp = SPTG->CMV2ScanlinerPerspTG__dwR;
	edx = SPTG->CMV2ScanlinerPerspTG__dwG;
	esi = SPTG->CMV2ScanlinerPerspTG__dwB;
	ebx = SPTG->CMV2ScanlinerPerspTG__dwXCounter;
	edi = 0;

MV2DrawPolygonTBASM_PerspBumpLoop:
// eax =
// ebx = XCounter
// ecx =
// edx =					|UAngle	|UAnglef
// esi = 				|VAngle	|VAnglef
// edi = Bump-,ShadeBuffer	(Loaded with BumbBuffer)
// ebp = 				|NAngle |NAnglef
	stack_var00 = ebx;

	ebx = 0;

	if (ebp > ( 0x0ffff )) goto MV2DrawPolygonTBASM_PerspAmbient;

	eax = ( *((uint32_t *)(((uint32_t)&(pBackBumpBuffer[0])) + edi)) );
	ecx = 0;
	ebx = set_high_byte(ebx, ( (uint8_t)(ebx >> 8) ) ^ ( *((uint8_t *)(eax * 2)) )); //UBump
	ecx = set_high_byte(ecx, ( *((uint8_t *)(eax * 2 + (1))) )); //VBump
	ebx = ebx - ( 128 * 256 );
	ecx = ecx - ( 128 * 256 );
	eax = 0;
	ebx = ebx + edx; //UAngle + UBump
	ecx = ecx + esi; //VAngle + VBump


	if (ebx > ( 0x0ffff )) goto MV2DrawPolygonTBASM_PerspAmbientBump;

	if (ecx > ( 0x0ffff )) goto MV2DrawPolygonTBASM_PerspAmbientBump;
	eax = (eax & 0xffffff00) | (uint8_t)(( (uint8_t)(ebx >> 8) ));
	eax = set_high_byte(eax, ( (uint8_t)(ecx >> 8) ));
	ebx = 0;
	ebx = (ebx & 0xffffff00) | (uint8_t)(( pAngleJoinTab[eax] )); //pAngleJoin
	goto MV2DrawPolygonTBASM_PerspAmbient;
MV2DrawPolygonTBASM_PerspAmbientBump:
	ebx = 0;
MV2DrawPolygonTBASM_PerspAmbient:
	ebx = ( pLightBrightnessTab[ebx] ); //Helligkeit
	*((uint32_t *)(((uint32_t)&(pBackShadeBuffer[0])) + edi)) = ebx; //ShadeBuffer

	edi = edi + ( 4 );
	ebx = stack_var00;
	edx = edx + dwGdX;
	esi = esi + dwBdX;
	ebp = ebp + dwRdX;

	ebx = ( (int32_t)ebx ) - 1;
	if (( (int32_t)ebx ) != 0) goto MV2DrawPolygonTBASM_PerspBumpLoop;



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
	edi = SPTG->CMV2ScanlinerPerspTG__dwScreenX1Offset;
	ebp = SPTG->CMV2ScanlinerPerspTG__dwXCounter;
	esi = 0;
	eax = 0;
	ebx = 0;
	ecx = 0;
	edx = 0;
	edi = edi - 1;

MV2DrawPolygonTBASM_PerspShadeLoop:
// eax = r
// ebx = shaded rgb
// ecx = g
// edx = b
// esi = offset
// edi = dest
// ebp = XCounter

	eax = ( *((uint32_t *)(((uint32_t)&(pBackShadeBuffer[0])) + esi)) );
	ebx = ( *((uint32_t *)(((uint32_t)&(pBackTextureBuffer[0])) + esi)) );

	eax = eax & ( 0x0ffffff );
	ebx = ebx & ( 0x0ffffff );
	edx = set_high_byte(edx, ( (uint8_t)eax ));
	ecx = set_high_byte(ecx, ( (uint8_t)(eax >> 8) ));
	edx = (edx & 0xffffff00) | (uint8_t)(( (uint8_t)ebx ));
	ecx = (ecx & 0xffffff00) | (uint8_t)(( (uint8_t)(ebx >> 8) ));
	eax = eax >> ( 8 );
	esi = esi + ( 4 );
	ebx = ebx >> ( 16 );
	edi = edi + 1;
	eax = (eax & 0xffffff00) | (uint8_t)(( (uint8_t)ebx ));

	ebx = (ebx & 0xffffff00) | (uint8_t)(( pBumpShadeLookup[ecx] )); // g
	ebx = set_high_byte(ebx, ( pBumpShadeLookup[eax] )); // r
	ebx = ebx << ( 8 );
	ebx = (ebx & 0xffffff00) | (uint8_t)(( pBumpShadeLookup[edx] )); // b

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
	*((uint32_t *)(edi * 4)) = ebx;

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
	fScreenYError1 = ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fScreenYError;
	fScreenYError3 = ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fScreenYError;

	edx = pDot1;
	esi = pDot2;
	edi = pDot3;

//===>                                                                <===
//===>	TextureU DeltaY Calculation {                            	 <===
//===>                                                                <===
	fpu_reg14 = ( ((CMV2Dot3D *)edx)->CMV2Dot3D__m_fTextureU );
	fpu_reg15 = ( ((CMV2Dot3D *)esi)->CMV2Dot3D__m_fTextureU );
	fpu_reg16 = ( ((CMV2Dot3D *)edi)->CMV2Dot3D__m_fTextureU );
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
	fpu_reg18 = fpu_reg18 + ( ((CMV2Dot3D *)edi)->CMV2Dot3D__m_fTextureU );
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg18 = fpu_reg18 + ( ((CMV2Dot3D *)edx)->CMV2Dot3D__m_fTextureU );
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
	fpu_reg14 = ( ((CMV2Dot3D *)edx)->CMV2Dot3D__m_fTextureV );
	fpu_reg15 = ( ((CMV2Dot3D *)esi)->CMV2Dot3D__m_fTextureV );
	fpu_reg16 = ( ((CMV2Dot3D *)edi)->CMV2Dot3D__m_fTextureV );
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
	fpu_reg18 = fpu_reg18 + ( ((CMV2Dot3D *)edi)->CMV2Dot3D__m_fTextureV );
	{ float tmp = fpu_reg18; fpu_reg18 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg18 = fpu_reg18 + ( ((CMV2Dot3D *)edx)->CMV2Dot3D__m_fTextureV );
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
	fpu_reg16 = fpu_reg16 + ( ((CMV2Dot3D *)edx)->CMV2Dot3D__m_fTextureV );
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = fpu_reg16 + ( ((CMV2Dot3D *)edx)->CMV2Dot3D__m_fTextureU );
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = fpu_reg16 - ( ((CMV2Dot3D *)edi)->CMV2Dot3D__m_fTextureV );
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = fpu_reg16 - ( ((CMV2Dot3D *)edi)->CMV2Dot3D__m_fTextureU );
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






	eax = dwDeltaScreenX1;
	ebx = dwDeltaScreenX;
	esi = eax;
	edi = ebx;
	eax = eax << ( 12 );
	ebx = ebx << ( 12 );
	esi = ( (int32_t)esi ) >> ( 20 );
	edi = ( (int32_t)edi ) >> ( 20 );
	esi = esi + dwXmax;
	edi = edi + dwXmax;
	dwAdderScreenX1 = esi;
	dwAdderScreenX2 = edi;
	dwAdderScreenX1f = eax;
	dwAdderScreenX2f = ebx;

	eax = dwScreenX1;
	ebx = dwScreenX2;
	esi = eax;
	edi = ebx;
	eax = eax << ( 12 );
	ebx = ebx << ( 12 );
	esi = ( (int32_t)esi ) >> ( 20 );
	edi = ( (int32_t)edi ) >> ( 20 );
	esi = esi + dwYOffset1;
	edi = edi + dwYOffset1;

	ebp = dwYCounter1;
	ecx = edi;

//mov		dwOldESP,esp
//and		esp,0ffffffe0h
	SLTG = (CMV2ScanlinerLinTG *) mem_alloc_endptr(dwYCounter * sizeof(CMV2ScanlinerLinTG));
//mov		dwESPStartPoint,esp
	dwESPStartPoint = (uint32_t)SLTG;

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
	eax = eax >> ( 12 );
//sub		esp,CMV2ScanlinerLinTG__size
	SLTG--;

	dwScreenXf = eax;
	ecx = ecx - esi; // XCounter
	eax = eax << ( 12 );

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
	SLTG->CMV2ScanlinerLinTG__dwScreenX1Offset = esi;
	SLTG->CMV2ScanlinerLinTG__dwXCounter = ecx;
// st0 = TextureU, st1 = TextureV
// st2 = TextureUdX, st3 = TextureVdX
// st4 = 2^20, st5 = 2^-20

	fpu_reg15 = fpu_reg15 + fTextureUdY1;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = fpu_reg15 + fTextureVdY1;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }

	{ uint32_t carry = (UINT32_MAX - ebx < dwAdderScreenX2f)?1:0; ebx = ebx + dwAdderScreenX2f;
	  edi = edi + dwAdderScreenX2 + carry; }
	{ uint32_t carry = (UINT32_MAX - eax < dwAdderScreenX1f)?1:0; eax = eax + dwAdderScreenX1f;
	  esi = esi + dwAdderScreenX1 + carry; }
	ecx = edi;

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2DrawPolygonTBASM_LinPT1TexturePass1;



	eax = dwDeltaScreenX2;
	esi = eax;
	eax = eax << ( 12 );
	esi = ( (int32_t)esi ) >> ( 20 );
	esi = esi + dwXmax;
	dwAdderScreenX1 = esi;
	dwAdderScreenX1f = eax;

	eax = dwScreenX3;
	esi = eax;
	eax = eax << ( 12 );
	esi = ( (int32_t)esi ) >> ( 20 );
	esi = esi + dwYOffset2;

	ebp = dwYCounter2;
	ecx = edi;

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
	eax = eax >> ( 12 );
//sub		esp,CMV2ScanlinerLinTG__size
	SLTG--;

	dwScreenXf = eax;
	ecx = ecx - esi; // XCounter
	eax = eax << ( 12 );

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
	SLTG->CMV2ScanlinerLinTG__dwScreenX1Offset = esi;
	SLTG->CMV2ScanlinerLinTG__dwXCounter = ecx;
// st0 = TextureU, st1 = TextureV
// st2 = TextureUdX, st3 = TextureVdX
// st4 = 2^20, st5 = 2^-20

	fpu_reg15 = fpu_reg15 + fTextureUdY2;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = fpu_reg15 + fTextureVdY2;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }

	{ uint32_t carry = (UINT32_MAX - ebx < dwAdderScreenX2f)?1:0; ebx = ebx + dwAdderScreenX2f;
	  edi = edi + dwAdderScreenX2 + carry; }
	{ uint32_t carry = (UINT32_MAX - eax < dwAdderScreenX1f)?1:0; eax = eax + dwAdderScreenX1f;
	  esi = esi + dwAdderScreenX1 + carry; }
	ecx = edi;

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2DrawPolygonTBASM_LinPT1TexturePass2;



	edx = dwESPStartPoint;
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
	edx = edx - ( CMV2ScanlinerLinTG__size );

	fpu_reg15 = ( ((CMV2ScanlinerLinTG *)edx)->CMV2ScanlinerLinTG__fScreenXError );
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

	((CMV2ScanlinerLinTG *)edx)->CMV2ScanlinerLinTG__dwR = (int32_t)ceilf(fpu_reg17);
	((CMV2ScanlinerLinTG *)edx)->CMV2ScanlinerLinTG__dwG = (int32_t)ceilf(fpu_reg16);
	((CMV2ScanlinerLinTG *)edx)->CMV2ScanlinerLinTG__dwB = (int32_t)ceilf(fpu_reg15);
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
	edx = edx - ( CMV2ScanlinerLinTG__size );

	fpu_reg15 = ( ((CMV2ScanlinerLinTG *)edx)->CMV2ScanlinerLinTG__fScreenXError );
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

	((CMV2ScanlinerLinTG *)edx)->CMV2ScanlinerLinTG__dwR = (int32_t)ceilf(fpu_reg17);
	((CMV2ScanlinerLinTG *)edx)->CMV2ScanlinerLinTG__dwG = (int32_t)ceilf(fpu_reg16);
	((CMV2ScanlinerLinTG *)edx)->CMV2ScanlinerLinTG__dwB = (int32_t)ceilf(fpu_reg15);
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
	fScreenYError1 = ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fScreenYError;
	fScreenYError2 = ((CMV2Dot3DPos *)esi)->CMV2Dot3DPos__m_fScreenYError;

	edx = pDot1;
	esi = pDot2;
	edi = pDot3;

//===>                                                                <===
//===>	TextureUV DeltaY Calculation {                            	 <===
//===>                                                                <===
	fpu_reg14 = ( ((CMV2Dot3D *)edi)->CMV2Dot3D__m_fTextureU );
	fpu_reg14 = fpu_reg14 - ( ((CMV2Dot3D *)edx)->CMV2Dot3D__m_fTextureU );
	fpu_reg15 = ( ((CMV2Dot3D *)edi)->CMV2Dot3D__m_fTextureV );
	fpu_reg15 = fpu_reg15 - ( ((CMV2Dot3D *)edx)->CMV2Dot3D__m_fTextureV );
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

	fpu_reg17 = fpu_reg17 + ( ((CMV2Dot3D *)edx)->CMV2Dot3D__m_fTextureU );
	{ float tmp = fpu_reg17; fpu_reg17 = fpu_reg16; fpu_reg16 = tmp; }
	fpu_reg17 = fpu_reg17 + ( ((CMV2Dot3D *)edx)->CMV2Dot3D__m_fTextureV );
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
	fpu_reg16 = fpu_reg16 + ( ((CMV2Dot3D *)edx)->CMV2Dot3D__m_fTextureU );
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = fpu_reg16 + ( ((CMV2Dot3D *)edx)->CMV2Dot3D__m_fTextureV );
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = ( ((CMV2Dot3D *)esi)->CMV2Dot3D__m_fTextureU ) - fpu_reg16;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = ( ((CMV2Dot3D *)esi)->CMV2Dot3D__m_fTextureV ) - fpu_reg16;
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






	eax = dwDeltaScreenX;
	ebx = dwDeltaScreenX1;
	esi = eax;
	edi = ebx;
	eax = eax << ( 12 );
	ebx = ebx << ( 12 );
	esi = ( (int32_t)esi ) >> ( 20 );
	edi = ( (int32_t)edi ) >> ( 20 );
	esi = esi + dwXmax;
	edi = edi + dwXmax;
	dwAdderScreenX1 = esi;
	dwAdderScreenX2 = edi;
	dwAdderScreenX1f = eax;
	dwAdderScreenX2f = ebx;

	eax = dwScreenX1;
	ebx = dwScreenX2;
	esi = eax;
	edi = ebx;
	eax = eax << ( 12 );
	ebx = ebx << ( 12 );
	esi = ( (int32_t)esi ) >> ( 20 );
	edi = ( (int32_t)edi ) >> ( 20 );
	esi = esi + dwYOffset1;
	edi = edi + dwYOffset1;

	ebp = dwYCounter1;
	ecx = edi;

//mov		dwOldESP,esp
//and		esp,0ffffffe0h
	SLTG = (CMV2ScanlinerLinTG *) mem_alloc_endptr(dwYCounter * sizeof(CMV2ScanlinerLinTG));
//mov		dwESPStartPoint,esp
	dwESPStartPoint = (uint32_t)SLTG;

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
	eax = eax >> ( 12 );
//sub		esp,CMV2ScanlinerLinTG__size
	SLTG--;

	dwScreenXf = eax;
	ecx = ecx - esi; // XCounter
	eax = eax << ( 12 );

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
	SLTG->CMV2ScanlinerLinTG__dwScreenX1Offset = esi;
	SLTG->CMV2ScanlinerLinTG__dwXCounter = ecx;
// st0 = TextureU, st1 = TextureV
// st2 = TextureUdX, st3 = TextureVdX
// st4 = 2^20, st5 = 2^-20

	fpu_reg15 = fpu_reg15 + fTextureUdY;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = fpu_reg15 + fTextureVdY;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }

	{ uint32_t carry = (UINT32_MAX - ebx < dwAdderScreenX2f)?1:0; ebx = ebx + dwAdderScreenX2f;
	  edi = edi + dwAdderScreenX2 + carry; }
	{ uint32_t carry = (UINT32_MAX - eax < dwAdderScreenX1f)?1:0; eax = eax + dwAdderScreenX1f;
	  esi = esi + dwAdderScreenX1 + carry; }
	ecx = edi;

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2DrawPolygonTBASM_LinPT2TexturePass1;

	ebx = dwDeltaScreenX2;
	edi = ebx;
	ebx = ebx << ( 12 );
	edi = ( (int32_t)edi ) >> ( 20 );
	edi = edi + dwXmax;
	dwAdderScreenX2 = edi;
	dwAdderScreenX2f = ebx;

	ebx = dwScreenX3;
	edi = ebx;
	ebx = ebx << ( 12 );
	edi = ( (int32_t)edi ) >> ( 20 );
	edi = edi + dwYOffset2;

	ebp = dwYCounter2;
	ecx = edi;

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
	eax = eax >> ( 12 );
//sub		esp,CMV2ScanlinerLinTG__size
	SLTG--;

	dwScreenXf = eax;
	ecx = ecx - esi; // XCounter
	eax = eax << ( 12 );

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
	SLTG->CMV2ScanlinerLinTG__dwScreenX1Offset = esi;
	SLTG->CMV2ScanlinerLinTG__dwXCounter = ecx;
// st0 = TextureU, st1 = TextureV
// st2 = TextureUdX, st3 = TextureVdX
// st4 = 2^20, st5 = 2^-20

	fpu_reg15 = fpu_reg15 + fTextureUdY;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = fpu_reg15 + fTextureVdY;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }

	{ uint32_t carry = (UINT32_MAX - ebx < dwAdderScreenX2f)?1:0; ebx = ebx + dwAdderScreenX2f;
	  edi = edi + dwAdderScreenX2 + carry; }
	{ uint32_t carry = (UINT32_MAX - eax < dwAdderScreenX1f)?1:0; eax = eax + dwAdderScreenX1f;
	  esi = esi + dwAdderScreenX1 + carry; }
	ecx = edi;

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2DrawPolygonTBASM_LinPT2TexturePass2;



	edx = dwESPStartPoint;
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
	edx = edx - ( CMV2ScanlinerLinTG__size );

	fpu_reg15 = ( ((CMV2ScanlinerLinTG *)edx)->CMV2ScanlinerLinTG__fScreenXError );
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

	((CMV2ScanlinerLinTG *)edx)->CMV2ScanlinerLinTG__dwR = (int32_t)ceilf(fpu_reg17);
	((CMV2ScanlinerLinTG *)edx)->CMV2ScanlinerLinTG__dwG = (int32_t)ceilf(fpu_reg16);
	((CMV2ScanlinerLinTG *)edx)->CMV2ScanlinerLinTG__dwB = (int32_t)ceilf(fpu_reg15);
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
	fScreenYError1 = ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fScreenYError;
	fScreenYError2 = ((CMV2Dot3DPos *)esi)->CMV2Dot3DPos__m_fScreenYError;

	edx = pDot1;
	esi = pDot2;
	edi = pDot3;

//===>                                                                <===
//===>	TextureUV DeltaY Calculation {                            	 <===
//===>                                                                <===
	fpu_reg12 = ( ((CMV2Dot3D *)edi)->CMV2Dot3D__m_fTextureU );
	fpu_reg12 = fpu_reg12 - ( ((CMV2Dot3D *)edx)->CMV2Dot3D__m_fTextureU );
	fpu_reg13 = ( ((CMV2Dot3D *)edi)->CMV2Dot3D__m_fTextureV );
	fpu_reg13 = fpu_reg13 - ( ((CMV2Dot3D *)edx)->CMV2Dot3D__m_fTextureV );
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

	fpu_reg15 = fpu_reg15 + ( ((CMV2Dot3D *)edx)->CMV2Dot3D__m_fTextureU );
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = fpu_reg15 + ( ((CMV2Dot3D *)edx)->CMV2Dot3D__m_fTextureV );
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
	fpu_reg14 = fpu_reg14 + ( ((CMV2Dot3D *)edx)->CMV2Dot3D__m_fTextureU );
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg14 = fpu_reg14 + ( ((CMV2Dot3D *)edx)->CMV2Dot3D__m_fTextureV );
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg14 = ( ((CMV2Dot3D *)esi)->CMV2Dot3D__m_fTextureU ) - fpu_reg14;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg14 = ( ((CMV2Dot3D *)esi)->CMV2Dot3D__m_fTextureV ) - fpu_reg14;
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




	eax = dwDeltaScreenX1;
	ebx = dwDeltaScreenX2;
	esi = eax;
	edi = ebx;
	eax = eax << ( 12 );
	ebx = ebx << ( 12 );
	esi = ( (int32_t)esi ) >> ( 20 );
	edi = ( (int32_t)edi ) >> ( 20 );
	esi = esi + dwXmax;
	edi = edi + dwXmax;
	dwAdderScreenX1 = esi;
	dwAdderScreenX2 = edi;
	dwAdderScreenX1f = eax;
	dwAdderScreenX2f = ebx;

	eax = dwScreenX1;
	ebx = dwScreenX2;
	esi = eax;
	edi = ebx;
	eax = eax << ( 12 );
	ebx = ebx << ( 12 );
	esi = ( (int32_t)esi ) >> ( 20 );
	edi = ( (int32_t)edi ) >> ( 20 );
	esi = esi + dwYOffset1;
	edi = edi + dwYOffset1;

//mov		dwOldESP,esp
//and		esp,0ffffffe0h
	SLTG = (CMV2ScanlinerLinTG *) mem_alloc_endptr(dwYCounter * sizeof(CMV2ScanlinerLinTG));
//mov		dwESPStartPoint,esp
	dwESPStartPoint = (uint32_t)SLTG;

	ebp = dwYCounter;
	ecx = edi;

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
	eax = eax >> ( 12 );
//sub		esp,CMV2ScanlinerLinTG__size
	SLTG--;

	dwScreenXf = eax;
	ecx = ecx - esi; // XCounter
	eax = eax << ( 12 );

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
	SLTG->CMV2ScanlinerLinTG__dwScreenX1Offset = esi;
	SLTG->CMV2ScanlinerLinTG__dwXCounter = ecx;
// st0 = TextureU, st1 = TextureV
// st2 = TextureUdX, st3 = TextureVdX
// st4 = 2^20, st5 = 2^-20

	fpu_reg15 = fpu_reg15 + fTextureUdY1;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = fpu_reg15 + fTextureVdY1;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }

	{ uint32_t carry = (UINT32_MAX - ebx < dwAdderScreenX2f)?1:0; ebx = ebx + dwAdderScreenX2f;
	  edi = edi + dwAdderScreenX2 + carry; }
	{ uint32_t carry = (UINT32_MAX - eax < dwAdderScreenX1f)?1:0; eax = eax + dwAdderScreenX1f;
	  esi = esi + dwAdderScreenX1 + carry; }
	ecx = edi;

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2DrawPolygonTBASM_LinPT3Texture;



	edx = dwESPStartPoint;
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
	edx = edx - ( CMV2ScanlinerLinTG__size );

	fpu_reg15 = ( ((CMV2ScanlinerLinTG *)edx)->CMV2ScanlinerLinTG__fScreenXError );
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

	((CMV2ScanlinerLinTG *)edx)->CMV2ScanlinerLinTG__dwR = (int32_t)ceilf(fpu_reg17);
	((CMV2ScanlinerLinTG *)edx)->CMV2ScanlinerLinTG__dwG = (int32_t)ceilf(fpu_reg16);
	((CMV2ScanlinerLinTG *)edx)->CMV2ScanlinerLinTG__dwB = (int32_t)ceilf(fpu_reg15);
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
	fScreenYError1 = ((CMV2Dot3DPos *)edx)->CMV2Dot3DPos__m_fScreenYError;
	fScreenYError2 = ((CMV2Dot3DPos *)esi)->CMV2Dot3DPos__m_fScreenYError;

	edx = pDot1;
	esi = pDot2;
	edi = pDot3;

//===>                                                                <===
//===>	TextureUV DeltaY Calculation {                            	 <===
//===>                                                                <===
	fpu_reg13 = ( ((CMV2Dot3D *)edi)->CMV2Dot3D__m_fTextureU );
	fpu_reg13 = fpu_reg13 - ( ((CMV2Dot3D *)edx)->CMV2Dot3D__m_fTextureU );
	fpu_reg14 = ( ((CMV2Dot3D *)edi)->CMV2Dot3D__m_fTextureV );
	fpu_reg14 = fpu_reg14 - ( ((CMV2Dot3D *)edx)->CMV2Dot3D__m_fTextureV );
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

	fpu_reg16 = fpu_reg16 + ( ((CMV2Dot3D *)edx)->CMV2Dot3D__m_fTextureU );
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg15; fpu_reg15 = tmp; }
	fpu_reg16 = fpu_reg16 + ( ((CMV2Dot3D *)edx)->CMV2Dot3D__m_fTextureV );
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
	fpu_reg15 = fpu_reg15 + ( ((CMV2Dot3D *)edx)->CMV2Dot3D__m_fTextureU );
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = fpu_reg15 + ( ((CMV2Dot3D *)edx)->CMV2Dot3D__m_fTextureV );
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = ( ((CMV2Dot3D *)esi)->CMV2Dot3D__m_fTextureU ) - fpu_reg15;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = ( ((CMV2Dot3D *)esi)->CMV2Dot3D__m_fTextureV ) - fpu_reg15;
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





	eax = dwDeltaScreenX1;
	ebx = dwDeltaScreenX2;
	esi = eax;
	edi = ebx;
	eax = eax << ( 12 );
	ebx = ebx << ( 12 );
	esi = ( (int32_t)esi ) >> ( 20 );
	edi = ( (int32_t)edi ) >> ( 20 );
	esi = esi + dwXmax;
	edi = edi + dwXmax;
	dwAdderScreenX1 = esi;
	dwAdderScreenX2 = edi;
	dwAdderScreenX1f = eax;
	dwAdderScreenX2f = ebx;

	eax = dwScreenX1;
	ebx = dwScreenX2;
	esi = eax;
	edi = ebx;
	eax = eax << ( 12 );
	ebx = ebx << ( 12 );
	esi = ( (int32_t)esi ) >> ( 20 );
	edi = ( (int32_t)edi ) >> ( 20 );
	esi = esi + dwYOffset1;
	edi = edi + dwYOffset1;

//mov		dwOldESP,esp
//and		esp,0ffffffe0h
	SLTG = (CMV2ScanlinerLinTG *) mem_alloc_endptr(dwYCounter * sizeof(CMV2ScanlinerLinTG));
//mov		dwESPStartPoint,esp
	dwESPStartPoint = (uint32_t)SLTG;

	ebp = dwYCounter;
	ecx = edi;

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
	eax = eax >> ( 12 );
//sub		esp,CMV2ScanlinerLinTG__size
	SLTG--;

	dwScreenXf = eax;
	ecx = ecx - esi; // XCounter
	eax = eax << ( 12 );

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
	SLTG->CMV2ScanlinerLinTG__dwScreenX1Offset = esi;
	SLTG->CMV2ScanlinerLinTG__dwXCounter = ecx;
// st0 = TextureU, st1 = TextureV
// st2 = TextureUdX, st3 = TextureVdX
// st4 = 2^20, st5 = 2^-20

	fpu_reg15 = fpu_reg15 + fTextureUdY1;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg15 = fpu_reg15 + fTextureVdY1;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg14; fpu_reg14 = tmp; }

	{ uint32_t carry = (UINT32_MAX - ebx < dwAdderScreenX2f)?1:0; ebx = ebx + dwAdderScreenX2f;
	  edi = edi + dwAdderScreenX2 + carry; }
	{ uint32_t carry = (UINT32_MAX - eax < dwAdderScreenX1f)?1:0; eax = eax + dwAdderScreenX1f;
	  esi = esi + dwAdderScreenX1 + carry; }
	ecx = edi;

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2DrawPolygonTBASM_LinPT4Texture;



	edx = dwESPStartPoint;
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
	edx = edx - ( CMV2ScanlinerLinTG__size );

	fpu_reg15 = ( ((CMV2ScanlinerLinTG *)edx)->CMV2ScanlinerLinTG__fScreenXError );
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

	((CMV2ScanlinerLinTG *)edx)->CMV2ScanlinerLinTG__dwR = (int32_t)ceilf(fpu_reg17);
	((CMV2ScanlinerLinTG *)edx)->CMV2ScanlinerLinTG__dwG = (int32_t)ceilf(fpu_reg16);
	((CMV2ScanlinerLinTG *)edx)->CMV2ScanlinerLinTG__dwB = (int32_t)ceilf(fpu_reg15);
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
	edi = 0;
	ebx = dwTextureUdX;
	eax = dwTextureVdX;
	ebp = ebx;
	esi = eax;
	ebx = ebx >> ( 16 ); // bl init.
	eax = eax >> ( 16 );
	esi = esi << ( 16 ); // esi init.
	ebp = ebp << ( 16 ); // ebp init.
	ebx = set_high_byte(ebx, ( (uint8_t)eax )); // bh init.
//mov		eax,ss:[esp].CMV2ScanlinerLinTG__dwTextureU
//mov		ecx,ss:[esp].CMV2ScanlinerLinTG__dwTextureV
	eax = SLTG->CMV2ScanlinerLinTG__dwTextureU;
	ecx = SLTG->CMV2ScanlinerLinTG__dwTextureV;
	edx = eax;
	eax = eax << ( 16 );
	edx = edx >> ( 16 ); // dl init.
	ebx = ebx | eax; // ebx init.
	eax = ecx;
	ecx = ecx << ( 16 ); // hi ecx init.
	eax = eax >> ( 16 );
//or		ecx,ss:[esp].CMV2ScanlinerLinTG__dwXCounter	; ecx init.
	ecx = ecx | SLTG->CMV2ScanlinerLinTG__dwXCounter; // ecx init.
	edx = set_high_byte(edx, ( (uint8_t)eax )); // dh init.
	edx = edx | pcTexture; // edx init.


	if ((( (uint16_t)ecx ) & ( (uint16_t)ecx )) == 0) goto MV2DrawPolygonTBASM_LinTGNoXLoop;

MV2DrawPolygonTBASM_LinTXLoop:
//*eax = counter
//*ebx =	txf				|dty	|dtx
//*ecx = tyf				|       |col
//*edx = pTexture		|ty		|tx
//*esi = dtyf
//*edi = destination
//*ebp = dtxf
	eax = edx;
	edx = edx & ( 0x0ffff );
	edx = edx + pcBumpmap;
	*((uint32_t *)(((uint32_t)&(pBackBumpBuffer[0])) + edi)) = edx; // ->pBackBumpBuffer
	edx = eax;

	eax = ( *((uint32_t *)(edx * 4)) );
	*((uint32_t *)(((uint32_t)&(pBackTextureBuffer[0])) + edi)) = eax;

	{ uint32_t carry = (UINT32_MAX - ebx < ebp)?1:0; ebx = ebx + ebp;
	  edx = (edx & 0xffffff00) | (uint8_t)(( (uint8_t)edx ) + ( (uint8_t)ebx ) + carry); }
	{ uint32_t carry = (UINT32_MAX - ecx < esi)?1:0; ecx = ecx + esi;
	  edx = set_high_byte(edx, ( (uint8_t)(edx >> 8) ) + ( (uint8_t)(ebx >> 8) ) + carry); }

	edi = edi + ( 4 );

	ecx = (ecx & 0xffff0000) | (uint16_t)(( (int16_t)ecx ) - 1);
	if (( (int16_t)ecx ) != 0) goto MV2DrawPolygonTBASM_LinTXLoop;

//mov		ebp,ss:[esp].CMV2ScanlinerLinTG__dwR
//mov		edx,ss:[esp].CMV2ScanlinerLinTG__dwG
//mov     esi,ss:[esp].CMV2ScanlinerLinTG__dwB
//mov		ebx,ss:[esp].CMV2ScanlinerLinTG__dwXCounter
	ebp = SLTG->CMV2ScanlinerLinTG__dwR;
	edx = SLTG->CMV2ScanlinerLinTG__dwG;
	esi = SLTG->CMV2ScanlinerLinTG__dwB;
	ebx = SLTG->CMV2ScanlinerLinTG__dwXCounter;
	edi = 0;

MV2DrawPolygonTBASM_LinBumpLoop:
// eax =
// ebx = XCounter
// ecx =
// edx =					|UAngle	|UAnglef
// esi = 				|VAngle	|VAnglef
// edi = Bump-,ShadeBuffer	(Loaded with BumbBuffer)
// ebp = 				|NAngle |NAnglef
	stack_var00 = ebx;

	ebx = 0;

	if (ebp > ( 0x0ffff )) goto MV2DrawPolygonTBASM_LinAmbient;

	eax = ( *((uint32_t *)(((uint32_t)&(pBackBumpBuffer[0])) + edi)) );
	ecx = 0;
	ebx = set_high_byte(ebx, ( (uint8_t)(ebx >> 8) ) ^ ( *((uint8_t *)(eax * 2)) )); //UBump
	ecx = set_high_byte(ecx, ( *((uint8_t *)(eax * 2 + (1))) )); //VBump
	ebx = ebx - ( 128 * 256 );
	ecx = ecx - ( 128 * 256 );
	eax = 0;
	ebx = ebx + edx; //UAngle + UBump
	ecx = ecx + esi; //VAngle + VBump


	if (ebx > ( 0x0ffff )) goto MV2DrawPolygonTBASM_LinAmbientBump;

	if (ecx > ( 0x0ffff )) goto MV2DrawPolygonTBASM_LinAmbientBump;
	eax = (eax & 0xffffff00) | (uint8_t)(( (uint8_t)(ebx >> 8) ));
	eax = set_high_byte(eax, ( (uint8_t)(ecx >> 8) ));
	ebx = 0;
	ebx = (ebx & 0xffffff00) | (uint8_t)(( pAngleJoinTab[eax] )); //pAngleJoin
	goto MV2DrawPolygonTBASM_LinAmbient;
MV2DrawPolygonTBASM_LinAmbientBump:
	ebx = 0;
MV2DrawPolygonTBASM_LinAmbient:
	ebx = ( pLightBrightnessTab[ebx] ); //Helligkeit
	*((uint32_t *)(((uint32_t)&(pBackShadeBuffer[0])) + edi)) = ebx; //ShadeBuffer

	edi = edi + ( 4 );
	ebx = stack_var00;
	edx = edx + dwGdX;
	esi = esi + dwBdX;
	ebp = ebp + dwRdX;

	ebx = ( (int32_t)ebx ) - 1;
	if (( (int32_t)ebx ) != 0) goto MV2DrawPolygonTBASM_LinBumpLoop;

//mov		edi,ss:[esp].CMV2ScanlinerLinTG__dwScreenX1Offset
//mov		ebp,ss:[esp].CMV2ScanlinerLinTG__dwXCounter
	edi = SLTG->CMV2ScanlinerLinTG__dwScreenX1Offset;
	ebp = SLTG->CMV2ScanlinerLinTG__dwXCounter;
	esi = 0;
	eax = 0;
	ebx = 0;
	ecx = 0;
	edx = 0;
	edi = edi - 1;

MV2DrawPolygonTBASM_LinShadeLoop:
// eax = r
// ebx = shaded rgb
// ecx = g
// edx = b
// esi = offset
// edi = dest
// ebp = XCounter

	eax = ( *((uint32_t *)(((uint32_t)&(pBackShadeBuffer[0])) + esi)) );
	ebx = ( *((uint32_t *)(((uint32_t)&(pBackTextureBuffer[0])) + esi)) );
	eax = eax & ( 0x0ffffff );
	ebx = ebx & ( 0x0ffffff );
	edx = set_high_byte(edx, ( (uint8_t)eax ));
	ecx = set_high_byte(ecx, ( (uint8_t)(eax >> 8) ));
	edx = (edx & 0xffffff00) | (uint8_t)(( (uint8_t)ebx ));
	ecx = (ecx & 0xffffff00) | (uint8_t)(( (uint8_t)(ebx >> 8) ));
	eax = eax >> ( 8 );
	esi = esi + ( 4 );
	ebx = ebx >> ( 16 );
	edi = edi + 1;
	eax = (eax & 0xffffff00) | (uint8_t)(( (uint8_t)ebx ));

	ebx = (ebx & 0xffffff00) | (uint8_t)(( pBumpShadeLookup[ecx] )); // g
	ebx = set_high_byte(ebx, ( pBumpShadeLookup[eax] )); // r
	ebx = ebx << ( 8 );
	ebx = (ebx & 0xffffff00) | (uint8_t)(( pBumpShadeLookup[edx] )); // b
//	or		ebx,0ffh
	*((uint32_t *)(edi * 4)) = ebx;

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





