#include <cstdint>
#include <cmath>
#if !defined(NO_FPU_CONTROL)
#include <fpu_control.h>
#endif
#include "cc.h"
//;/*
//.486

//.model flat

//LOCALS

//data32 SEGMENT PAGE
//;.data
//fpucw						dw ?
//oldfpucw					dw ?
//oh_low 						dd ?
//co_low						dd ?
//co_hi						dd ?

//align 32
static uint32_t pMV2Camera;
static uint32_t ppMV2Dot3DPos;
static uint32_t dwNumDots;

static float fCameraX;
static float fCameraY;
static float fCameraZ;
static float fCameraFX;
static float fCameraFY;
static float fCameraFZ;
static float fCameraRX;
static float fCameraRY;
static float fCameraRZ;
static float fCameraDX;
static float fCameraDY;
static float fCameraDZ;

static uint32_t dwClipXmin;
static uint32_t dwClipYmin;
static uint32_t dwClipXmax;
static uint32_t dwClipYmax;

static uint32_t dwXmax;
static uint32_t dwYmax;
static float fXmax;
static float fYmax;
static float fXmaxDiv2;
static float fYmaxDiv2;

static float fScreenFactor;
const static float fAspectRatio = 1.333333333f;

const static float fNum_1 = 1.0f;
const static float fNum_01 = 0.1f;
//fNum_05						dd 0.5
//fNum_10Exp_10				dd 0.0000000001
const static float fNum_2Exp20 = 1048576.0f;

//fTemp						dd ?
static uint32_t lTemp;
//wTemp						dw ?
//align 4
//bTemp						db ?
//align 4
//ends


//.code


#include "m2struct.inc.h"








extern "C" void MESEtoMV2Dot3DPosProjectionASM(uint32_t _pMV2Camera, uint32_t _ppMV2Dot3DPos, uint32_t _dwNumDots) {
	float fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13, fpu_reg14, fpu_reg15, fpu_reg16, fpu_reg17, fpu_reg18;
	uint32_t eax, edx, ecx, edi, ebx, esi, ebp;





#if !defined(NO_FPU_CONTROL)
	fpu_control_t oldcw;
	{
		fpu_control_t cw;
		_FPU_GETCW(oldcw);
#if defined(__i386__)
		cw = oldcw & ~((_FPU_RC_NEAREST | _FPU_RC_DOWN | _FPU_RC_UP | _FPU_RC_ZERO) | (_FPU_EXTENDED | _FPU_DOUBLE | _FPU_SINGLE));
		cw |= (_FPU_RC_UP | _FPU_SINGLE);
#elif defined(__arm__)
		cw = oldcw & ~(0x00C00000);
		cw |= (0x00400000);
		/*
		  0b00 - Round to Nearest (RN) mode
		  0b01 - Round towards Plus Infinity (RP) mode
		  0b10 - Round towards Minus Infinity (RM) mode
		  0b11 - Round towards Zero (RZ) mode.
		*/
#else
		todo
#endif
		_FPU_SETCW(cw);
    }
#endif
#if 0
//;****
//	fstcw   oldfpucw
//	mov     ax,oldfpucw
//	and     eax,0fcffh
//	and		eax,0fbffh
//	or		eax,00800h
//	mov     fpucw,ax
//	fldcw   fpucw
//;****
#endif

	eax = _pMV2Camera;
	ebx = _ppMV2Dot3DPos;
	ecx = _dwNumDots;

	pMV2Camera = eax;
	ppMV2Dot3DPos = ebx;
	dwNumDots = ecx;

	esi = pMV2Camera;
	eax = ( ((CMV2Camera *)esi)->CMV2Camera__m_iXmax );
	ebx = ( ((CMV2Camera *)esi)->CMV2Camera__m_iYmax );

	dwXmax = eax;
	dwYmax = ebx;
	fpu_reg10 = ( (int32_t)dwXmax );
	fpu_reg11 = ( (int32_t)dwYmax );
	fYmax = fpu_reg11;
	fXmax = fpu_reg10;

	eax = eax >> ( 1 );
	ebx = ebx >> ( 1 );

	lTemp = eax;
	fpu_reg10 = ( (int32_t)lTemp );
	fXmaxDiv2 = fpu_reg10;

	lTemp = ebx;
	fpu_reg10 = ( (int32_t)lTemp );
	fYmaxDiv2 = fpu_reg10;

	eax = ( ((CMV2Camera *)esi)->CMV2Camera__m_iClipXmin );
	ebx = ( ((CMV2Camera *)esi)->CMV2Camera__m_iClipXmax );
	ecx = ( ((CMV2Camera *)esi)->CMV2Camera__m_iClipYmin );
	edx = ( ((CMV2Camera *)esi)->CMV2Camera__m_iClipYmax );

	eax = eax + 1; // wegen des clippings
	ecx = ecx + 1;

	dwClipXmin = eax;
	dwClipXmax = ebx;
	dwClipYmin = ecx;
	dwClipYmax = edx;

	fCameraX = ((CMV2Camera *)esi)->CMV2Camera__m_Pos__m_fX;
	fCameraY = ((CMV2Camera *)esi)->CMV2Camera__m_Pos__m_fY;
	fCameraZ = ((CMV2Camera *)esi)->CMV2Camera__m_Pos__m_fZ;

	fCameraFX = ((CMV2Camera *)esi)->CMV2Camera__m_Front__m_fX;
	fCameraFY = ((CMV2Camera *)esi)->CMV2Camera__m_Front__m_fY;
	fCameraFZ = ((CMV2Camera *)esi)->CMV2Camera__m_Front__m_fZ;

	fpu_reg10 = 1.0f;
	fScreenFactor = fpu_reg10;

//	fld     ds:[esi].CMV2Camera__m_fScreenFactor

	fpu_reg10 = fScreenFactor;
	fpu_reg11 = ( ((CMV2Camera *)esi)->CMV2Camera__m_Right__m_fX );
	fpu_reg11 = fpu_reg11 * fpu_reg10;
//st0 = CameraRX*ScreenFactor, st1 = ScreenFactor

	fpu_reg12 = ( ((CMV2Camera *)esi)->CMV2Camera__m_Right__m_fY );
	fpu_reg12 = fpu_reg12 * fpu_reg10;
	fpu_reg13 = ( ((CMV2Camera *)esi)->CMV2Camera__m_Right__m_fZ );
	fpu_reg13 = fpu_reg13 * fpu_reg10;
//st0 = CameraRZ*ScreenFactor, st1 = CameraRY*ScreenFactor
//st2 = CameraRX*ScreenFactor, st3 = ScreenFactor

	fpu_reg14 = ( ((CMV2Camera *)esi)->CMV2Camera__m_Down__m_fX );
	fpu_reg14 = fpu_reg14 * fpu_reg10;
	fpu_reg15 = ( ((CMV2Camera *)esi)->CMV2Camera__m_Down__m_fY );
	fpu_reg15 = fpu_reg15 * fpu_reg10;
	fpu_reg16 = ( ((CMV2Camera *)esi)->CMV2Camera__m_Down__m_fZ );
	fpu_reg16 = fpu_reg16 * fpu_reg10;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg16; fpu_reg16 = tmp; }
//st0 = CameraDX*ScreenFactor, st1 = CameraDY*ScreenFactor
//st2 = CameraDZ*ScreenFactor, st3 = CameraRZ*ScreenFactor
//st4 = CameraRY*ScreenFactor, st5 = CameraRX*ScreenFactor
//st6 = ScreenFactor

	fCameraDX = fpu_reg16;
	fCameraDY = fpu_reg15;
	fCameraDZ = fpu_reg14;
	fCameraRZ = fpu_reg13;
	fCameraRY = fpu_reg12;
	fCameraRX = fpu_reg11;
//ffree	st(0)


	edx = ppMV2Dot3DPos;
	ebp = dwNumDots;
	edi = ( *((uint32_t *)(edx)) );

MESEtoMV2Dot3DPosProjectionASM_DotLoop:
	fpu_reg10 = ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_Pos__m_fX );
//	fsub	fCameraX
//st0 = VectorX

	fpu_reg11 = fCameraFX;
	fpu_reg11 = fpu_reg11 * fpu_reg10;
//st0 = fCameraFX*VectorX, st1 = VectorX

	fpu_reg12 = fCameraRX;
	fpu_reg12 = fpu_reg12 * fpu_reg10;
//st0 = fCameraRX*VectorX, st1 = fCameraFX*VectorX
//st2 = VectorX

	fpu_reg13 = fCameraDX;
	fpu_reg13 = fpu_reg13 * fpu_reg10;
	// fpu_reg10 = 0.0f;
//st0 = fCameraDX*VectorX, st1 = fCameraRX*VectorX
//st2 = fCameraFX*VectorX

	fpu_reg14 = ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_Pos__m_fY );
//	fsub	fCameraY
//st0 = VectorY, st1 = fCameraDX*VectorX
//st2 = fCameraRX*VectorX, st3 = fCameraFX*VectorX

	fpu_reg15 = fCameraFY;
	fpu_reg15 = fpu_reg15 * fpu_reg14;
//st0 = fCameraFY*VectorY, st1 = VectorY
//st1 = fCameraDX*VectorX
//st2 = fCameraRX*VectorX, st3 = fCameraFX*VectorX

	fpu_reg16 = fCameraRY;
	fpu_reg16 = fpu_reg16 * fpu_reg14;
//st0 = fCameraRY*VectorY, st1 = fCameraFY*VectorY
//st2 = VectorY, st3 = fCameraDX*VectorX
//st4 = fCameraRX*VectorX, st5 = fCameraFX*VectorX

	fpu_reg17 = fCameraDY;
	fpu_reg17 = fpu_reg17 * fpu_reg14;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
//st0 = fCameraFY*VectorY, st1 = fCameraRY*VectorY
//st2 = fCameraDY*VectorY, st3 = VectorY
//st4 = fCameraDX*VectorX
//st5 = fCameraRX*VectorX, st6 = fCameraFX*VectorX

	fpu_reg11 = fpu_reg11 + fpu_reg17;
//st0 = fCameraRY*VectorY, st1 = fCameraDY*VectorY
//st2 = VectorY, st3 = fCameraDX*VectorX
//st4 = fCameraRX*VectorX
//st5 = fCameraFX*VectorX + fCameraFY*VectorY

	fpu_reg12 = fpu_reg12 + fpu_reg16;
//st0 = fCameraDY*VectorY
//st1 = VectorY, st2 = fCameraDX*VectorX
//st3 = fCameraRX*VectorX + fCameraRY*VectorY
//st4 = fCameraFX*VectorX + fCameraFY*VectorY

	fpu_reg13 = fpu_reg13 + fpu_reg15;
	{ float tmp = fpu_reg11; fpu_reg11 = fpu_reg14; fpu_reg14 = tmp; }
	// fpu_reg11 = 0.0f;
//st0 = fCameraFX*VectorX + fCameraFY*VectorY
//st1 = fCameraDX*VectorX + fCameraDY*VectorY
//st2 = fCameraRX*VectorX + fCameraRY*VectorY

	fpu_reg15 = ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_Pos__m_fZ );
//	fsub	fCameraZ
//st0 = VectorZ
//st1 = fCameraFX*VectorX + fCameraFY*VectorY
//st2 = fCameraDX*VectorX + fCameraDY*VectorY
//st3 = fCameraRX*VectorX + fCameraRY*VectorY

	fpu_reg16 = fCameraFZ;
	fpu_reg16 = fpu_reg16 * fpu_reg15;
//st0 = fCameraZ*VectorZ, st1 = VectorZ
//st2 = fCameraFX*VectorX + fCameraFY*VectorY
//st3 = fCameraDX*VectorX + fCameraDY*VectorY
//st4 = fCameraRX*VectorX + fCameraRY*VectorY

	fpu_reg17 = fCameraRZ;
	fpu_reg17 = fpu_reg17 * fpu_reg15;
//st0 = fCameraRZ*VectorZ, st1 = fCameraFZ*VectorZ
//st2 = VectorZ
//st3 = fCameraFX*VectorX + fCameraFY*VectorY
//st4 = fCameraDX*VectorX + fCameraDY*VectorY
//st5 = fCameraRX*VectorX + fCameraRY*VectorY

	fpu_reg18 = fCameraDZ;
	fpu_reg18 = fpu_reg18 * fpu_reg15;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg18; fpu_reg18 = tmp; }
//st0 = fCameraFZ*VectorZ, st1 = fCameraRZ*VectorZ
//st2 = fCameraDZ*VectorZ, st3 = VectorZ
//st4 = fCameraFX*VectorX + fCameraFY*VectorY
//st5 = fCameraDX*VectorX + fCameraDY*VectorY
//st6 = fCameraRX*VectorX + fCameraRY*VectorY

	fpu_reg14 = fpu_reg14 + fpu_reg18;
//st0 = fCameraRZ*VectorZ
//st1 = fCameraDZ*VectorZ, st2 = VectorZ
//st3 = fProjectionF
//st4 = fCameraDX*VectorX + fCameraDY*VectorY
//st5 = fCameraRX*VectorX + fCameraRY*VectorY

	fpu_reg12 = fpu_reg12 + fpu_reg17;
//st0 = fCameraDZ*VectorZ, st1 = VectorZ
//st2 = fProjectionF, st3 = fCameraRX*VectorX + fCameraRY*VectorY
//st4 = fProjectionD

	fpu_reg13 = fpu_reg13 + fpu_reg16;
//st0 = VectorZ
//st1 = fProjectionF = ZNew, st2 = fProjectionD = YNew
//st3 = fProjectionR = XNew

//ffree	st(0)
//fincstp

//st0 = fProjectionF = ZNew, st1 = fProjectionD = YNew
//st2 = fProjectionR = XNew


	fpu_reg14 = fpu_reg14 + fCameraZ;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg14 = fpu_reg14 + fCameraY;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }
	fpu_reg14 = fpu_reg14 + fCameraX;
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }

	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_TransfPos__m_fY = fpu_reg14;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }
	((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_TransfPos__m_fX = fpu_reg14;
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }


	eax = (eax & 0xffff0000) | ((fpu_reg14 < fNum_01)?0x100:0); // tasm 3.0 bug (= fstsw)
	eax = eax & ( 0x0100 ); // sahf

// ClippingFlag
// Bit0 = xmin, Bit1 = xmax
// Bit2 = ymin, Bit3 = ymax
// Bit8 = zmin, (Bit9 = zmax)
// zmin, zmax, ymin, ymax, xmin, xmax

	((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_TransfPos__m_fZ = fpu_reg14;

	fpu_reg14 = fabsf(fpu_reg14);

	fpu_reg15 = 1.0f;
	fpu_reg14 = fpu_reg15 / fpu_reg14;
//st0 = 1/fProjectionF, st1 = fProjectionR
//st2 = fProjectionD
//MESEtoMV2Dot3DPosProjectionASM_NoZClipping:

	ecx = eax;

	((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fZNewRZP = fpu_reg14;

	fpu_reg12 = fpu_reg12 * fpu_reg14;
	fpu_reg13 = fpu_reg13 * fpu_reg14;
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
//st0 = fProjectionR/fProjectionF
//st1 = fProjectionD/fProjectionF
//	fadd	fXmaxDiv2
//	fxch
//	fadd	fYmaxDiv2
//	fxch

	fpu_reg13 = fpu_reg13 + fNum_1;
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
	fpu_reg13 = fpu_reg13 + fNum_1;
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
	fpu_reg13 = fpu_reg13 * fXmaxDiv2;
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
	fpu_reg13 = fpu_reg13 * fYmaxDiv2;
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }

	((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_iScreenX = (int32_t)ceilf(fpu_reg13);
	edx = edx + ( 4 );
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
	((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_iScreenY = (int32_t)ceilf(fpu_reg13);
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
	fpu_reg13 = fpu_reg13 * fNum_2Exp20;
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
	fpu_reg14 = fpu_reg13;
	((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fScreenY = fpu_reg14;
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }
	((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fScreenX = fpu_reg14;
	fpu_reg13 = ceilf(fpu_reg13);

	eax = ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_iScreenX );
	ebx = ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_iScreenY );

	fpu_reg13 = fpu_reg13 - fpu_reg12; // muss noch ueberprueft werden
// (optimierung)
	((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fScreenYError = fpu_reg13;
//ffree	st(0)





	if (( (int32_t)eax ) >= ( (int32_t)dwClipXmin )) goto MESEtoMV2Dot3DPosProjectionASM_NotUnderXmin;
	ecx = ecx | ( 1 );

MESEtoMV2Dot3DPosProjectionASM_NotUnderXmin:

	if (( (int32_t)eax ) <= ( (int32_t)dwClipXmax )) goto MESEtoMV2Dot3DPosProjectionASM_NotAboveXmax; // jnge
	ecx = ecx | ( 2 );

MESEtoMV2Dot3DPosProjectionASM_NotAboveXmax:

	if (( (int32_t)ebx ) >= ( (int32_t)dwClipYmin )) goto MESEtoMV2Dot3DPosProjectionASM_NotUnderYmin;
	ecx = ecx | ( 4 );

MESEtoMV2Dot3DPosProjectionASM_NotUnderYmin:

	if (( (int32_t)ebx ) <= ( (int32_t)dwClipYmax )) goto MESEtoMV2Dot3DPosProjectionASM_NotAboveYmax; // jnge
	ecx = ecx | ( 8 );

MESEtoMV2Dot3DPosProjectionASM_NotAboveYmax:
	((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_iClipFlag = ecx;
	edi = ( *((uint32_t *)(edx)) );

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MESEtoMV2Dot3DPosProjectionASM_DotLoop;

#if !defined(NO_FPU_CONTROL)
	{
		_FPU_SETCW(oldcw);
	}
#endif
#if 0
//	fldcw   oldfpucw
#endif


	return;
}










extern "C" void MV2Dot3DPosProjectionASM(uint32_t _pMV2Camera, uint32_t _ppMV2Dot3DPos, uint32_t _dwNumDots) {
	float fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13, fpu_reg14, fpu_reg15, fpu_reg16, fpu_reg17, fpu_reg18;
	uint32_t eax, edx, ecx, edi, ebx, esi, ebp;





#if !defined(NO_FPU_CONTROL)
	fpu_control_t oldcw;
	{
		fpu_control_t cw;
		_FPU_GETCW(oldcw);
#if defined(__i386__)
		cw = oldcw & ~((_FPU_RC_NEAREST | _FPU_RC_DOWN | _FPU_RC_UP | _FPU_RC_ZERO) | (_FPU_EXTENDED | _FPU_DOUBLE | _FPU_SINGLE));
		cw |= (_FPU_RC_UP | _FPU_SINGLE);
#elif defined(__arm__)
		cw = oldcw & ~(0x00C00000);
		cw |= (0x00400000);
		/*
		  0b00 - Round to Nearest (RN) mode
		  0b01 - Round towards Plus Infinity (RP) mode
		  0b10 - Round towards Minus Infinity (RM) mode
		  0b11 - Round towards Zero (RZ) mode.
		*/
#else
		todo
#endif
		_FPU_SETCW(cw);
    }
#endif
#if 0
//;****
//	fstcw   oldfpucw
//	mov     ax,oldfpucw
//	and     eax,0fcffh
//	and		eax,0fbffh
//	or		eax,00800h
//	mov     fpucw,ax
//	fldcw   fpucw
//;****
#endif

	eax = _pMV2Camera;
	ebx = _ppMV2Dot3DPos;
	ecx = _dwNumDots;

	pMV2Camera = eax;
	ppMV2Dot3DPos = ebx;
	dwNumDots = ecx;

	esi = pMV2Camera;
	eax = ( ((CMV2Camera *)esi)->CMV2Camera__m_iXmax );
	ebx = ( ((CMV2Camera *)esi)->CMV2Camera__m_iYmax );

	dwXmax = eax;
	dwYmax = ebx;
	fpu_reg10 = ( (int32_t)dwXmax );
	fpu_reg11 = ( (int32_t)dwYmax );
	fYmax = fpu_reg11;
	fXmax = fpu_reg10;

	eax = eax >> ( 1 );
	ebx = ebx >> ( 1 );

	lTemp = eax;
	fpu_reg10 = ( (int32_t)lTemp );
	fXmaxDiv2 = fpu_reg10;

	lTemp = ebx;
	fpu_reg10 = ( (int32_t)lTemp );
	fYmaxDiv2 = fpu_reg10;

	eax = ( ((CMV2Camera *)esi)->CMV2Camera__m_iClipXmin );
	ebx = ( ((CMV2Camera *)esi)->CMV2Camera__m_iClipXmax );
	ecx = ( ((CMV2Camera *)esi)->CMV2Camera__m_iClipYmin );
	edx = ( ((CMV2Camera *)esi)->CMV2Camera__m_iClipYmax );

	eax = eax + 1; // wegen des clippings
	ecx = ecx + 1;

	dwClipXmin = eax;
	dwClipXmax = ebx;
	dwClipYmin = ecx;
	dwClipYmax = edx;

	fCameraX = ((CMV2Camera *)esi)->CMV2Camera__m_Pos__m_fX;
	fCameraY = ((CMV2Camera *)esi)->CMV2Camera__m_Pos__m_fY;
	fCameraZ = ((CMV2Camera *)esi)->CMV2Camera__m_Pos__m_fZ;

	fCameraFX = ((CMV2Camera *)esi)->CMV2Camera__m_Front__m_fX;
	fCameraFY = ((CMV2Camera *)esi)->CMV2Camera__m_Front__m_fY;
	fCameraFZ = ((CMV2Camera *)esi)->CMV2Camera__m_Front__m_fZ;

	fpu_reg10 = fAspectRatio;
	fpu_reg11 = ( ((CMV2Camera *)esi)->CMV2Camera__m_Right__m_fX );
//st0 = CameraRX, st1 = AspectRatio
	fpu_reg12 = ( ((CMV2Camera *)esi)->CMV2Camera__m_Right__m_fY );
	fpu_reg13 = ( ((CMV2Camera *)esi)->CMV2Camera__m_Right__m_fZ );
//st0 = CameraRZ, st1 = CameraRY
//st2 = CameraRX, st3 = AspectRatio

	fpu_reg14 = ( ((CMV2Camera *)esi)->CMV2Camera__m_Down__m_fX );
	fpu_reg14 = fpu_reg14 * fpu_reg10;
	fpu_reg15 = ( ((CMV2Camera *)esi)->CMV2Camera__m_Down__m_fY );
	fpu_reg15 = fpu_reg15 * fpu_reg10;
	fpu_reg16 = ( ((CMV2Camera *)esi)->CMV2Camera__m_Down__m_fZ );
	fpu_reg16 = fpu_reg16 * fpu_reg10;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg16; fpu_reg16 = tmp; }
//st0 = CameraDX*AspectRatio, st1 = CameraDY
//st2 = CameraDZ*AspectRatio, st3 = CameraRZ
//st4 = CameraRY*AspectRatio, st5 = CameraRX
//st6 = AspectRatio

	fCameraDX = fpu_reg16;
	fCameraDY = fpu_reg15;
	fCameraDZ = fpu_reg14;
	fCameraRZ = fpu_reg13;
	fCameraRY = fpu_reg12;
	fCameraRX = fpu_reg11;
//ffree	st(0)


	edx = ppMV2Dot3DPos;
	ebp = dwNumDots;
	edi = ( *((uint32_t *)(edx)) );

MV2Dot3DPosProjectionASM_DotLoop:
	fpu_reg10 = ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_Pos__m_fX );
	fpu_reg10 = fpu_reg10 - fCameraX;
//st0 = VectorX

	fpu_reg11 = fCameraFX;
	fpu_reg11 = fpu_reg11 * fpu_reg10;
//st0 = fCameraFX*VectorX, st1 = VectorX

	fpu_reg12 = fCameraRX;
	fpu_reg12 = fpu_reg12 * fpu_reg10;
//st0 = fCameraRX*VectorX, st1 = fCameraFX*VectorX
//st2 = VectorX

	fpu_reg13 = fCameraDX;
	fpu_reg13 = fpu_reg13 * fpu_reg10;
	// fpu_reg10 = 0.0f;
//st0 = fCameraDX*VectorX, st1 = fCameraRX*VectorX
//st2 = fCameraFX*VectorX

	fpu_reg14 = ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_Pos__m_fY );
	fpu_reg14 = fpu_reg14 - fCameraY;
//st0 = VectorY, st1 = fCameraDX*VectorX
//st2 = fCameraRX*VectorX, st3 = fCameraFX*VectorX

	fpu_reg15 = fCameraFY;
	fpu_reg15 = fpu_reg15 * fpu_reg14;
//st0 = fCameraFY*VectorY, st1 = VectorY
//st1 = fCameraDX*VectorX
//st2 = fCameraRX*VectorX, st3 = fCameraFX*VectorX

	fpu_reg16 = fCameraRY;
	fpu_reg16 = fpu_reg16 * fpu_reg14;
//st0 = fCameraRY*VectorY, st1 = fCameraFY*VectorY
//st2 = VectorY, st3 = fCameraDX*VectorX
//st4 = fCameraRX*VectorX, st5 = fCameraFX*VectorX

	fpu_reg17 = fCameraDY;
	fpu_reg17 = fpu_reg17 * fpu_reg14;
	{ float tmp = fpu_reg15; fpu_reg15 = fpu_reg17; fpu_reg17 = tmp; }
//st0 = fCameraFY*VectorY, st1 = fCameraRY*VectorY
//st2 = fCameraDY*VectorY, st3 = VectorY
//st4 = fCameraDX*VectorX
//st5 = fCameraRX*VectorX, st6 = fCameraFX*VectorX

	fpu_reg11 = fpu_reg11 + fpu_reg17;
//st0 = fCameraRY*VectorY, st1 = fCameraDY*VectorY
//st2 = VectorY, st3 = fCameraDX*VectorX
//st4 = fCameraRX*VectorX
//st5 = fCameraFX*VectorX + fCameraFY*VectorY

	fpu_reg12 = fpu_reg12 + fpu_reg16;
//st0 = fCameraDY*VectorY
//st1 = VectorY, st2 = fCameraDX*VectorX
//st3 = fCameraRX*VectorX + fCameraRY*VectorY
//st4 = fCameraFX*VectorX + fCameraFY*VectorY

	fpu_reg13 = fpu_reg13 + fpu_reg15;
	{ float tmp = fpu_reg11; fpu_reg11 = fpu_reg14; fpu_reg14 = tmp; }
	// fpu_reg11 = 0.0f;
//st0 = fCameraFX*VectorX + fCameraFY*VectorY
//st1 = fCameraDX*VectorX + fCameraDY*VectorY
//st2 = fCameraRX*VectorX + fCameraRY*VectorY

	fpu_reg15 = ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_Pos__m_fZ );
	fpu_reg15 = fpu_reg15 - fCameraZ;
//st0 = VectorZ
//st1 = fCameraFX*VectorX + fCameraFY*VectorY
//st2 = fCameraDX*VectorX + fCameraDY*VectorY
//st3 = fCameraRX*VectorX + fCameraRY*VectorY

	fpu_reg16 = fCameraFZ;
	fpu_reg16 = fpu_reg16 * fpu_reg15;
//st0 = fCameraZ*VectorZ, st1 = VectorZ
//st2 = fCameraFX*VectorX + fCameraFY*VectorY
//st3 = fCameraDX*VectorX + fCameraDY*VectorY
//st4 = fCameraRX*VectorX + fCameraRY*VectorY

	fpu_reg17 = fCameraRZ;
	fpu_reg17 = fpu_reg17 * fpu_reg15;
//st0 = fCameraRZ*VectorZ, st1 = fCameraFZ*VectorZ
//st2 = VectorZ
//st3 = fCameraFX*VectorX + fCameraFY*VectorY
//st4 = fCameraDX*VectorX + fCameraDY*VectorY
//st5 = fCameraRX*VectorX + fCameraRY*VectorY

	fpu_reg18 = fCameraDZ;
	fpu_reg18 = fpu_reg18 * fpu_reg15;
	{ float tmp = fpu_reg16; fpu_reg16 = fpu_reg18; fpu_reg18 = tmp; }
//st0 = fCameraFZ*VectorZ, st1 = fCameraRZ*VectorZ
//st2 = fCameraDZ*VectorZ, st3 = VectorZ
//st4 = fCameraFX*VectorX + fCameraFY*VectorY
//st5 = fCameraDX*VectorX + fCameraDY*VectorY
//st6 = fCameraRX*VectorX + fCameraRY*VectorY

	fpu_reg14 = fpu_reg14 + fpu_reg18;
//st0 = fCameraRZ*VectorZ
//st1 = fCameraDZ*VectorZ, st2 = VectorZ
//st3 = fProjectionF
//st4 = fCameraDX*VectorX + fCameraDY*VectorY
//st5 = fCameraRX*VectorX + fCameraRY*VectorY

	fpu_reg12 = fpu_reg12 + fpu_reg17;
//st0 = fCameraDZ*VectorZ, st1 = VectorZ
//st2 = fProjectionF, st3 = fCameraRX*VectorX + fCameraRY*VectorY
//st4 = fProjectionD

	fpu_reg13 = fpu_reg13 + fpu_reg16;
//st0 = VectorZ
//st1 = fProjectionF = ZNew, st2 = fProjectionD = YNew
//st3 = fProjectionR = XNew

//ffree	st(0)
//fincstp

//st0 = fProjectionF = ZNew, st1 = fProjectionD = YNew
//st2 = fProjectionR = XNew

	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_TransfPos__m_fY = fpu_reg14;
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }
	((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_TransfPos__m_fX = fpu_reg14;
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }


	eax = (eax & 0xffff0000) | ((fpu_reg14 < fNum_01)?0x100:0); // tasm 3.0 bug (= fstsw)
	eax = eax & ( 0x0100 ); // sahf

// ClippingFlag
// Bit0 = xmin, Bit1 = xmax
// Bit2 = ymin, Bit3 = ymax
// Bit8 = zmin, (Bit9 = zmax)
// zmin, zmax, ymin, ymax, xmin, xmax

	((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_TransfPos__m_fZ = fpu_reg14;

	fpu_reg14 = fabsf(fpu_reg14);

	fpu_reg15 = 1.0f;
	fpu_reg14 = fpu_reg15 / fpu_reg14;
//st0 = 1/fProjectionF, st1 = fProjectionR
//st2 = fProjectionD

	ecx = eax;

	((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fZNewRZP = fpu_reg14;

	fpu_reg12 = fpu_reg12 * fpu_reg14;
	fpu_reg13 = fpu_reg13 * fpu_reg14;
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
//st0 = fProjectionR/fProjectionF
//st1 = fProjectionD/fProjectionF
//	fadd	fXmaxDiv2
//	fxch
//	fadd	fYmaxDiv2
//	fxch

	fpu_reg13 = fpu_reg13 + fNum_1;
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
	fpu_reg13 = fpu_reg13 + fNum_1;
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
	fpu_reg13 = fpu_reg13 * fXmaxDiv2;
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
	fpu_reg13 = fpu_reg13 * fYmaxDiv2;
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }

	((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_iScreenX = (int32_t)ceilf(fpu_reg13);
	edx = edx + ( 4 );
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
	((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_iScreenY = (int32_t)ceilf(fpu_reg13);
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
	fpu_reg13 = fpu_reg13 * fNum_2Exp20;
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
	fpu_reg14 = fpu_reg13;
	((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fScreenY = fpu_reg14;
	{ float tmp = fpu_reg12; fpu_reg12 = fpu_reg14; fpu_reg14 = tmp; }
	((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fScreenX = fpu_reg14;
	fpu_reg13 = ceilf(fpu_reg13);

	eax = ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_iScreenX );
	ebx = ( ((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_iScreenY );

	fpu_reg13 = fpu_reg13 - fpu_reg12; // muss noch ueberprueft werden
// (optimierung)
	((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fScreenYError = fpu_reg13;
//ffree	st(0)





	if (( (int32_t)eax ) >= ( (int32_t)dwClipXmin )) goto MV2Dot3DPosProjectionASM_NotUnderXmin;
	ecx = ecx | ( 1 );

MV2Dot3DPosProjectionASM_NotUnderXmin:

	if (( (int32_t)eax ) <= ( (int32_t)dwClipXmax )) goto MV2Dot3DPosProjectionASM_NotAboveXmax; // jnge
	ecx = ecx | ( 2 );

MV2Dot3DPosProjectionASM_NotAboveXmax:

	if (( (int32_t)ebx ) >= ( (int32_t)dwClipYmin )) goto MV2Dot3DPosProjectionASM_NotUnderYmin;
	ecx = ecx | ( 4 );

MV2Dot3DPosProjectionASM_NotUnderYmin:

	if (( (int32_t)ebx ) <= ( (int32_t)dwClipYmax )) goto MV2Dot3DPosProjectionASM_NotAboveYmax; // jnge
	ecx = ecx | ( 8 );

MV2Dot3DPosProjectionASM_NotAboveYmax:
	((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_iClipFlag = ecx;
	edi = ( *((uint32_t *)(edx)) );

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2Dot3DPosProjectionASM_DotLoop;

#if !defined(NO_FPU_CONTROL)
	{
		_FPU_SETCW(oldcw);
	}
#endif
#if 0
//	fldcw   oldfpucw
#endif



	return;
}






