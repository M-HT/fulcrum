#if !defined(NO_FPU_CONTROL)
#include <fpu_control.h>
#endif
#include "cc.h"
//;/*
//.486

//.model flat

//LOCALS

extern "C" uint32_t pcSparklesLookup[256];

//data32 SEGMENT PAGE
//.data

//fpucw						dw ?
//oldfpucw					dw ?
//oh_low 						dd ?
//co_low						dd ?
//co_hi						dd ?

//align 32
static uint32_t pMV2Camera;
static uint32_t pMV2Particles;
static uint32_t dwNumParticles;
static uint32_t pBackBuffer;

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

static float fScreenFactor;

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

static uint32_t dwScreenX;
static uint32_t dwScreenY;
static float fZNewRZP;

#if 0
//fRCol						dd 50.0
//fGCol						dd 180.0
//fBCol						dd 180.0
#endif

static float fRCol = 255.0;
static float fGCol = 255.0;
static float fBCol = 255.0;



static uint32_t dwRDest;
static uint32_t dwGDest;
static uint32_t dwBDest;

const static float fAngleTOSinTabfactor = 40.74366543f; //256.0/(2PI)
static uint32_t dwAngleX;
static uint32_t dwAngleY2;
static uint32_t dwAngleY1;
static uint32_t dwAngleZ;

const static float fScaling = 3.5f;




//*****************
const static float fNum_1 = 1.0f;
//fNum_01						dd 0.1
//fNum_05						dd 0.5
//fNum_10Exp_10				dd 0.0000000001
//fNum_2Exp20					dd 1048576.0

//fTemp						dd ?
static uint32_t lTemp;
//wTemp						dw ?
//align 4
//bTemp						db ?
//align 4
//*****************

static uint32_t dwTempR;
static uint32_t dwTempG;
static uint32_t dwTempB;
//ends

//.code

#include "m2struct.inc.h"


extern "C" void MV2ParticleProjectionASM(uint32_t _pMV2Camera, uint32_t _pMV2Particles, uint32_t _dwNumParticles, uint32_t _pBackBuffer) {
	float fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13, fpu_reg14, fpu_reg15, fpu_reg16, fpu_reg17, fpu_reg18;
	uint32_t eax, edx, ecx, edi, ebx, esi, ebp;
	uint32_t stack_var00, stack_var01, stack_var02;



#if !defined(NO_FPU_CONTROL)
#if defined(__i386__)
	fpu_control_t oldcw;
	{
		fpu_control_t cw;
		_FPU_GETCW(oldcw);
		cw = oldcw & ~((_FPU_EXTENDED | _FPU_DOUBLE | _FPU_SINGLE));
		cw |= (_FPU_SINGLE);
		_FPU_SETCW(cw);
	}
#elif defined(__arm__)
#else
		todo
#endif
#endif
#if 0
//;****
//	fstcw   oldfpucw
//	mov     ax,oldfpucw
//	and     eax,0fcffh
//;	and		eax,0fbffh
//;	or		eax,00800h
//	mov     fpucw,ax
//	fldcw   fpucw
//;****
#endif

	eax = _pMV2Camera;
	ebx = _pMV2Particles;
	ecx = _dwNumParticles;
	edx = _pBackBuffer;

	pMV2Camera = eax;
	pMV2Particles = ebx;
	dwNumParticles = ecx;
	pBackBuffer = edx;

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

//	dec		eax					; wegen des clippings
//	dec		ecx

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

	fScreenFactor = ((CMV2Camera *)esi)->CMV2Camera__m_fScreenFactor;

//	fld     ds:[esi].CMV2Camera__m_fScreenFactor
	fpu_reg10 = 1.0f;
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


	esi = pMV2Particles;
	edi = pBackBuffer;
	ebp = dwNumParticles;

MV2ParticleProjectionASM_ParticleLoop:
	fpu_reg10 = ( ((CMV2Particle *)esi)->CMV2Particle__m_InterpPos__m_fX );
	fpu_reg10 = fpu_reg10 * fScaling;
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

	fpu_reg14 = ( ((CMV2Particle *)esi)->CMV2Particle__m_InterpPos__m_fZ );
	fpu_reg14 = fpu_reg14 * fScaling;
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

	fpu_reg15 = ( ((CMV2Particle *)esi)->CMV2Particle__m_InterpPos__m_fY );
	fpu_reg15 = -fpu_reg15;
	fpu_reg15 = fpu_reg15 * fScaling;
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
//st1 = fProjectionF, st2 = fProjectionD
//st3 = fProjectionR

//ffree	st(0)
//fincstp



	eax = (eax & 0xffff0000) | ((fpu_reg14 < fNum_1)?0x100:0); // tasm 3.0 bug (= fstsw)
	eax = eax & ( 0x0100 ); // sahf

// ClippingFlag
// Bit0 = xmin, Bit1 = xmax
// Bit2 = ymin, Bit3 = ymax
// Bit8 = zmin, (Bit9 = zmax)
// zmin, zmax, ymin, ymax, xmin, xmax

	fpu_reg15 = 1.0f;
	fpu_reg14 = fpu_reg15 / fpu_reg14;
//st0 = 1/fProjectionF, st1 = fProjectionR
//st2 = fProjectionD
	ecx = eax;

	fZNewRZP = fpu_reg14;

	fpu_reg12 = fpu_reg12 * fpu_reg14;
	fpu_reg13 = fpu_reg13 * fpu_reg14;
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
//st0 = fProjectionR/fProjectionF
//st1 = fProjectionD/fProjectionF

	fpu_reg13 = fpu_reg13 + fNum_1;
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
	fpu_reg13 = fpu_reg13 + fNum_1;
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
	fpu_reg13 = fpu_reg13 * fXmaxDiv2;
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
	fpu_reg13 = fpu_reg13 * fYmaxDiv2;
	{ float tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }

	dwScreenX = (int32_t)roundf(fpu_reg13);
	dwScreenY = (int32_t)roundf(fpu_reg12);

#if 0
//	fadd	fXmaxDiv2
//	fxch
//	fadd	fYmaxDiv2
//	fxch
//
//	fistp	dwScreenX
//	fistp	dwScreenY
#endif

	eax = dwScreenX;
	ebx = dwScreenY;


	if (( (int32_t)eax ) >= ( (int32_t)dwClipXmin )) goto MV2ParticleProjectionASM_NotUnderXmin;
	ecx = ecx | ( 1 );

MV2ParticleProjectionASM_NotUnderXmin:

	if (( (int32_t)eax ) < ( (int32_t)dwClipXmax )) goto MV2ParticleProjectionASM_NotAboveXmax; // jng
	ecx = ecx | ( 2 );

MV2ParticleProjectionASM_NotAboveXmax:

	if (( (int32_t)ebx ) >= ( (int32_t)dwClipYmin )) goto MV2ParticleProjectionASM_NotUnderYmin;
	ecx = ecx | ( 4 );

MV2ParticleProjectionASM_NotUnderYmin:

	if (( (int32_t)ebx ) < ( (int32_t)dwClipYmax )) goto MV2ParticleProjectionASM_NotAboveYmax; // jng
	ecx = ecx | ( 8 );

MV2ParticleProjectionASM_NotAboveYmax:

	if ( (( (int32_t)ecx ) - ( 0 )) != 0) goto MV2ParticleProjectionASM_DontDrawParticle;

//pushad
	stack_var00 = eax;
	stack_var01 = ebx;
	stack_var02 = edx;
	eax = ( ((CMV2Particle *)esi)->CMV2Particle__m_iDuration );

	eax = eax & ( 0x0ff );
	eax = ( pcSparklesLookup[eax] );
	ebx = eax;
	edx = eax;
	eax = eax & ( 0x0ff0000 );
	ebx = ebx & ( 0x000ff00 );
	edx = ( 0x00000ff );
	eax = eax >> ( 16 );
	ebx = ebx >> ( 8 );
	dwTempR = eax;
	dwTempG = ebx;
	dwTempB = edx;
	fpu_reg12 = ( (int32_t)dwTempR );
	fpu_reg13 = ( (int32_t)dwTempG );
	fpu_reg14 = ( (int32_t)dwTempB );
	{ float tmp = fpu_reg14; fpu_reg14 = fpu_reg13; fpu_reg13 = tmp; }
	fGCol = fpu_reg14;
	fBCol = fpu_reg13;
	fRCol = fpu_reg12;
//popad
	edx = stack_var02;
	ebx = stack_var01;
	eax = stack_var00;

	ecx = dwXmax;
	ebx = ( (int32_t)ebx ) * ( (int32_t)ecx );
	ebx = ebx + eax;

	fpu_reg12 = 1.0f;
	fpu_reg13 = fZNewRZP;
	fpu_reg12 = fpu_reg12 - fpu_reg13;
//st0 = 1/ZNew, st1 = 1 - 1/ZNew

	eax = ( ((uint32_t *)(edi))[ebx] );
	ecx = eax;
	edx = eax;
	eax = eax & ( 0x0ff0000 );
	ecx = ecx & ( 0x000ff00 );
	edx = edx & ( 0x00000ff );
	eax = eax >> ( 16 );
	ecx = ecx >> ( 8 );
	dwRDest = eax;
	dwGDest = ecx;
	dwBDest = edx;

	fpu_reg14 = ( (int32_t)dwRDest );
	fpu_reg14 = fpu_reg14 * fpu_reg12;
	fpu_reg15 = fRCol;
	fpu_reg15 = fpu_reg15 * fpu_reg13;
	fpu_reg14 = fpu_reg14 + fpu_reg15;
	dwRDest = (int32_t)roundf(fpu_reg14);

	fpu_reg14 = ( (int32_t)dwGDest );
	fpu_reg14 = fpu_reg14 * fpu_reg12;
	fpu_reg15 = fGCol;
	fpu_reg15 = fpu_reg15 * fpu_reg13;
	fpu_reg14 = fpu_reg14 + fpu_reg15;
	dwGDest = (int32_t)roundf(fpu_reg14);

	fpu_reg14 = ( (int32_t)dwBDest );
	fpu_reg14 = fpu_reg14 * fpu_reg12;
	fpu_reg15 = fBCol;
	fpu_reg15 = fpu_reg15 * fpu_reg13;
	fpu_reg14 = fpu_reg14 + fpu_reg15;
	dwBDest = (int32_t)roundf(fpu_reg14);

//ffree 	st(1)
//ffree	st(0)



	eax = dwRDest;
	ecx = dwGDest;
	edx = dwBDest;
	eax = eax << ( 16 );
	ecx = ecx << ( 8 );
	eax = eax | edx;
	eax = eax | ecx;

	((uint32_t *)(edi))[ebx] = eax;


MV2ParticleProjectionASM_DontDrawParticle:



	esi = esi + ( CMV2Particle__Size );
	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto MV2ParticleProjectionASM_ParticleLoop;


#if !defined(NO_FPU_CONTROL)
#if defined(__i386__)
	{
		_FPU_SETCW(oldcw);
	}
#elif defined(__arm__)
#else
		todo
#endif
#endif
#if 0
//	fldcw   oldfpucw
#endif


	return;
}







extern "C" void MV2ParticleDoBernoulliASM(uint32_t pParticles, uint32_t _dwNumParticles, uint32_t pSinTab, uint32_t dwCurTime, float fAX, float fAY1, float fAY2, float fAZ, float fFX, float fFY1, float fFY2, float fFZ, float fSDX, float fSDY1, float fSDY2, float fSDZ, float fDY1, float fDY2) {
	realnum fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13, fpu_reg14, fpu_reg15, fpu_reg16, fpu_reg17;
	float feax, febx, fedx;
	uint32_t eax, /*edx,*/ ecx, edi, ebx, esi;




	esi = pParticles;
	ecx = _dwNumParticles;
	edi = pSinTab;

MV2ParticleDoBernoulliASM_ParticleLoop:

	if ( (( (int32_t)(((CMV2Particle *)esi)->CMV2Particle__m_iDuration) ) - ( 0 )) != 0) goto MV2ParticleDoBernoulliASM_DurationOk;

	((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fX = ((CMV2Particle *)esi)->CMV2Particle__m_StartPos__m_fX;
	((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fY = ((CMV2Particle *)esi)->CMV2Particle__m_StartPos__m_fY;
	((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fZ = ((CMV2Particle *)esi)->CMV2Particle__m_StartPos__m_fZ;

	eax = ( 12000 );
	((CMV2Particle *)esi)->CMV2Particle__m_iDuration = eax;

MV2ParticleDoBernoulliASM_DurationOk:
	feax = ( ((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fX );
	febx = ( ((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fY );
	fedx = ( ((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fZ );

	fpu_reg10 = ( ((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fY );
	fpu_reg10 = fpu_reg10 * fFX;

	fpu_reg11 = ( ((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fZ );
	fpu_reg11 = fpu_reg11 * fFY1;

	fpu_reg12 = ( ((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fX );
	fpu_reg12 = fpu_reg12 * fFY2;

	fpu_reg13 = ( ((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fX );
	fpu_reg13 = fpu_reg13 * fFZ;
	{ realnum tmp = fpu_reg10; fpu_reg10 = fpu_reg13; fpu_reg13 = tmp; }
//st0=ParticleY*fFreqX, st1=ParticleX*fFreqY2
//st2=ParticleZ*fFreqY1, st3=ParticleX*fFreqZ

	fpu_reg14 = ( (int32_t)dwCurTime );
	fpu_reg15 = fpu_reg14;
	fpu_reg15 = fpu_reg15 * fSDX;
	fpu_reg16 = fpu_reg14;
	fpu_reg16 = fpu_reg16 * fSDY1;
	fpu_reg17 = fpu_reg14;
	fpu_reg17 = fpu_reg17 * fSDY2;
	{ realnum tmp = fpu_reg14; fpu_reg14 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 * fSDZ;
//st0=CurTime*fSinZ, st1=CurTime*fSinY2
//st2=CurTime*fSinY1, st3=CurTime*fSinX
//st4=ParticleY*fFreqX, st5=ParticleX*fFreqY2
//st6=ParticleZ*fFreqY1, st7=ParticleX*fFreqZ

	fpu_reg10 = fpu_reg10 + fpu_reg17;
//st0=CurTime*fSinY2
//st1=CurTime*fSinY1, st2=CurTime*fSinX
//st3=ParticleY*fFreqX, st4=ParticleX*fFreqY2
//st5=ParticleZ*fFreqY1, st6=fAngleZ

	fpu_reg12 = fpu_reg12 + fpu_reg16;
//st0=CurTime*fSinY1, st1=CurTime*fSinX
//st2=ParticleY*fFreqX, st3=fAngleY2
//st4=ParticleZ*fFreqY1, st5=fAngleZ

	fpu_reg11 = fpu_reg11 + fpu_reg15;
//st0=CurTime*fSinX
//st1=ParticleY*fFreqX, st2=fAngleY2
//st3=fAngleY1, st4=fAngleZ

	fpu_reg13 = fpu_reg13 + fpu_reg14;
//st0=fAngleX, st1=fAngleY2
//st2=fAngleY1, st3=fAngleZ

	fpu_reg14 = fAngleTOSinTabfactor;
	fpu_reg13 = fpu_reg13 * fpu_reg14;
	fpu_reg12 = fpu_reg12 * fpu_reg14;
	fpu_reg11 = fpu_reg11 * fpu_reg14;
	fpu_reg10 = fpu_reg10 * fpu_reg14;

//ffree	st(0)
//fincstp


	dwAngleX = (int32_t)round(fpu_reg13);
	dwAngleY2 = (int32_t)round(fpu_reg12);
	dwAngleY1 = (int32_t)round(fpu_reg11);
	dwAngleZ = (int32_t)round(fpu_reg10);

	((CMV2Particle *)esi)->CMV2Particle__m_LastPos__m_fX = feax;
	((CMV2Particle *)esi)->CMV2Particle__m_LastPos__m_fY = febx;
	((CMV2Particle *)esi)->CMV2Particle__m_LastPos__m_fZ = fedx;

	eax = dwAngleX;
	ebx = dwAngleY2;
	eax = eax & ( 0x0ff );
	ebx = ebx & ( 0x0ff );

	fpu_reg10 = ( ((float *)(edi))[eax] );
	fpu_reg10 = fpu_reg10 * fAX;
	fpu_reg11 = ( ((float *)(edi))[ebx] );
	fpu_reg11 = fpu_reg11 + fDY2;
	fpu_reg11 = fpu_reg11 * fAY2;
	{ realnum tmp = fpu_reg11; fpu_reg11 = fpu_reg10; fpu_reg10 = tmp; }

	fpu_reg11 = fpu_reg11 + ( ((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fX );
	{ realnum tmp = fpu_reg11; fpu_reg11 = fpu_reg10; fpu_reg10 = tmp; }
	fpu_reg11 = fpu_reg11 + ( ((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fY );
	{ realnum tmp = fpu_reg11; fpu_reg11 = fpu_reg10; fpu_reg10 = tmp; }
	((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fX = fpu_reg11;
	((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fY = fpu_reg10;


	eax = dwAngleY1;
	ebx = dwAngleZ;
	eax = eax & ( 0x0ff );
	ebx = ebx & ( 0x0ff );

	fpu_reg10 = ( ((float *)(edi))[eax] );
	fpu_reg10 = fpu_reg10 + fDY1;
	fpu_reg10 = fpu_reg10 * fAY1;
	fpu_reg11 = ( ((float *)(edi))[ebx] );
	fpu_reg11 = fpu_reg11 * fAZ;
	{ realnum tmp = fpu_reg11; fpu_reg11 = fpu_reg10; fpu_reg10 = tmp; }

	fpu_reg11 = fpu_reg11 + ( ((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fY );
	{ realnum tmp = fpu_reg11; fpu_reg11 = fpu_reg10; fpu_reg10 = tmp; }
	fpu_reg11 = fpu_reg11 + ( ((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fZ );
	{ realnum tmp = fpu_reg11; fpu_reg11 = fpu_reg10; fpu_reg10 = tmp; }
	((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fY = fpu_reg11;
	((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fZ = fpu_reg10;


	((CMV2Particle *)esi)->CMV2Particle__m_iDuration = ( ((CMV2Particle *)esi)->CMV2Particle__m_iDuration ) - 1;
	esi = esi + ( CMV2Particle__Size );

	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto MV2ParticleDoBernoulliASM_ParticleLoop;



	return;
}







extern "C" void MV2ParticleDoBernoulliMorphToObjectASM(uint32_t pParticles, uint32_t _dwNumParticles, uint32_t pSinTab, uint32_t dwCurTime, float fAX, float fAY1, float fAY2, float fAZ, float fFX, float fFY1, float fFY2, float fFZ, float fSDX, float fSDY1, float fSDY2, float fSDZ, float fDY1, float fDY2, float fMFactor) {
	realnum fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13, fpu_reg14, fpu_reg15, fpu_reg16, fpu_reg17;
	uint32_t eax, /*edx,*/ ecx, edi, ebx, esi;




	esi = pParticles;
	ecx = _dwNumParticles;
	edi = pSinTab;

MV2ParticleDoBernoulliMorphToObjectASM_ParticleLoop:
	((CMV2Particle *)esi)->CMV2Particle__m_LastPos__m_fX = ((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fX;
	((CMV2Particle *)esi)->CMV2Particle__m_LastPos__m_fY = ((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fY;
	((CMV2Particle *)esi)->CMV2Particle__m_LastPos__m_fZ = ((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fZ;


	if ( (( (int32_t)(((CMV2Particle *)esi)->CMV2Particle__m_iDuration) ) - ( 0 )) != 0) goto MV2ParticleDoBernoulliMorphToObjectASM_DurationOk;

	((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fX = ((CMV2Particle *)esi)->CMV2Particle__m_StartPos__m_fX;
	((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fY = ((CMV2Particle *)esi)->CMV2Particle__m_StartPos__m_fY;
	((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fZ = ((CMV2Particle *)esi)->CMV2Particle__m_StartPos__m_fZ;

	eax = ( 12000 );
	((CMV2Particle *)esi)->CMV2Particle__m_iDuration = eax;
MV2ParticleDoBernoulliMorphToObjectASM_DurationOk:
	fpu_reg10 = ( ((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fY );
	fpu_reg10 = fpu_reg10 * fFX;

	fpu_reg11 = ( ((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fZ );
	fpu_reg11 = fpu_reg11 * fFY1;

	fpu_reg12 = ( ((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fX );
	fpu_reg12 = fpu_reg12 * fFY2;

	fpu_reg13 = ( ((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fX );
	fpu_reg13 = fpu_reg13 * fFZ;
	{ realnum tmp = fpu_reg10; fpu_reg10 = fpu_reg13; fpu_reg13 = tmp; }
//st0=ParticleY*fFreqX, st1=ParticleX*fFreqY2
//st2=ParticleZ*fFreqY1, st3=ParticleX*fFreqZ

	fpu_reg14 = ( (int32_t)dwCurTime );
	fpu_reg15 = fpu_reg14;
	fpu_reg15 = fpu_reg15 * fSDX;
	fpu_reg16 = fpu_reg14;
	fpu_reg16 = fpu_reg16 * fSDY1;
	fpu_reg17 = fpu_reg14;
	fpu_reg17 = fpu_reg17 * fSDY2;
	{ realnum tmp = fpu_reg14; fpu_reg14 = fpu_reg17; fpu_reg17 = tmp; }
	fpu_reg17 = fpu_reg17 * fSDZ;
//st0=CurTime*fSinZ, st1=CurTime*fSinY2
//st2=CurTime*fSinY1, st3=CurTime*fSinX
//st4=ParticleY*fFreqX, st5=ParticleX*fFreqY2
//st6=ParticleZ*fFreqY1, st7=ParticleX*fFreqZ

	fpu_reg10 = fpu_reg10 + fpu_reg17;
//st0=CurTime*fSinY2
//st1=CurTime*fSinY1, st2=CurTime*fSinX
//st3=ParticleY*fFreqX, st4=ParticleX*fFreqY2
//st5=ParticleZ*fFreqY1, st6=fAngleZ

	fpu_reg12 = fpu_reg12 + fpu_reg16;
//st0=CurTime*fSinY1, st1=CurTime*fSinX
//st2=ParticleY*fFreqX, st3=fAngleY2
//st4=ParticleZ*fFreqY1, st5=fAngleZ

	fpu_reg11 = fpu_reg11 + fpu_reg15;
//st0=CurTime*fSinX
//st1=ParticleY*fFreqX, st2=fAngleY2
//st3=fAngleY1, st4=fAngleZ

	fpu_reg13 = fpu_reg13 + fpu_reg14;
//st0=fAngleX, st1=fAngleY2
//st2=fAngleY1, st3=fAngleZ

	fpu_reg14 = fAngleTOSinTabfactor;
	fpu_reg13 = fpu_reg13 * fpu_reg14;
	fpu_reg12 = fpu_reg12 * fpu_reg14;
	fpu_reg11 = fpu_reg11 * fpu_reg14;
	fpu_reg10 = fpu_reg10 * fpu_reg14;

//ffree	st(0)
//fincstp


	dwAngleX = (int32_t)round(fpu_reg13);
	dwAngleY2 = (int32_t)round(fpu_reg12);
	dwAngleY1 = (int32_t)round(fpu_reg11);
	dwAngleZ = (int32_t)round(fpu_reg10);

	eax = dwAngleX;
	ebx = dwAngleY2;
	eax = eax & ( 0x0ff );
	ebx = ebx & ( 0x0ff );

	fpu_reg10 = ( ((float *)(edi))[eax] );
	fpu_reg10 = fpu_reg10 * fAX;
	fpu_reg11 = ( ((float *)(edi))[ebx] );
	fpu_reg11 = fpu_reg11 + fDY2;
	fpu_reg11 = fpu_reg11 * fAY2;
	{ realnum tmp = fpu_reg11; fpu_reg11 = fpu_reg10; fpu_reg10 = tmp; }

	fpu_reg11 = fpu_reg11 + ( ((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fX );
	{ realnum tmp = fpu_reg11; fpu_reg11 = fpu_reg10; fpu_reg10 = tmp; }
	fpu_reg11 = fpu_reg11 + ( ((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fY );
	{ realnum tmp = fpu_reg11; fpu_reg11 = fpu_reg10; fpu_reg10 = tmp; }
	((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fX = fpu_reg11;
	((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fY = fpu_reg10;


	eax = dwAngleY1;
	ebx = dwAngleZ;
	eax = eax & ( 0x0ff );
	ebx = ebx & ( 0x0ff );

	fpu_reg10 = ( ((float *)(edi))[eax] );
	fpu_reg10 = fpu_reg10 + fDY1;
	fpu_reg10 = fpu_reg10 * fAY1;
	fpu_reg11 = ( ((float *)(edi))[ebx] );
	fpu_reg11 = fpu_reg11 * fAZ;
	{ realnum tmp = fpu_reg11; fpu_reg11 = fpu_reg10; fpu_reg10 = tmp; }

	fpu_reg11 = fpu_reg11 + ( ((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fY );
	{ realnum tmp = fpu_reg11; fpu_reg11 = fpu_reg10; fpu_reg10 = tmp; }
	fpu_reg11 = fpu_reg11 + ( ((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fZ );
	{ realnum tmp = fpu_reg11; fpu_reg11 = fpu_reg10; fpu_reg10 = tmp; }
	((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fY = fpu_reg11;
	((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fZ = fpu_reg10;

	fpu_reg10 = fMFactor;
	fpu_reg11 = ( ((CMV2Particle *)esi)->CMV2Particle__m_EndPos__m_fX );
	fpu_reg11 = fpu_reg11 - ( ((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fX );
	fpu_reg12 = ( ((CMV2Particle *)esi)->CMV2Particle__m_EndPos__m_fY );
	fpu_reg12 = fpu_reg12 - ( ((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fY );
	fpu_reg13 = ( ((CMV2Particle *)esi)->CMV2Particle__m_EndPos__m_fZ );
	fpu_reg13 = fpu_reg13 - ( ((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fZ );
//st0 = EndZ - Z, st1 = EndY - Y, st2 = EndX - X
//st3 = MFactor

	fpu_reg13 = fpu_reg13 * fpu_reg10;
	{ realnum tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
	fpu_reg13 = fpu_reg13 * fpu_reg10;
	{ realnum tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
	{ realnum tmp = fpu_reg11; fpu_reg11 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg13 = fpu_reg13 * fpu_reg10;
	{ realnum tmp = fpu_reg11; fpu_reg11 = fpu_reg13; fpu_reg13 = tmp; }
//st0 = (EndZ - Z)*MFactor, st1 = (EndY - Y)*MFactor
//st2 = (EndX - X)*MFactor, st3 = MFactor

	fpu_reg13 = fpu_reg13 + ( ((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fZ );
	{ realnum tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
	fpu_reg13 = fpu_reg13 + ( ((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fY );
	{ realnum tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
	{ realnum tmp = fpu_reg11; fpu_reg11 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg13 = fpu_reg13 + ( ((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fX );
	{ realnum tmp = fpu_reg11; fpu_reg11 = fpu_reg13; fpu_reg13 = tmp; }

	((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fZ = fpu_reg13;
	((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fY = fpu_reg12;
	((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fX = fpu_reg11;


	((CMV2Particle *)esi)->CMV2Particle__m_iDuration = ( ((CMV2Particle *)esi)->CMV2Particle__m_iDuration ) - 1;
//fcomp


	esi = esi + ( CMV2Particle__Size );

	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto MV2ParticleDoBernoulliMorphToObjectASM_ParticleLoop;



	return;
}







extern "C" void MV2ParticleInterpolateASM(uint32_t _pParticles, uint32_t _dwNumParticles, float _fMorphFactor) {
	realnum fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13;
	uint32_t ecx, esi;




	esi = _pParticles;
	ecx = _dwNumParticles;
	fpu_reg10 = _fMorphFactor;
//st0 = MorphFactor

MV2ParticleInterpolateASM_ParticleLoop:
	fpu_reg11 = ( ((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fX );
	fpu_reg11 = fpu_reg11 - ( ((CMV2Particle *)esi)->CMV2Particle__m_LastPos__m_fX );
	fpu_reg12 = ( ((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fY );
	fpu_reg12 = fpu_reg12 - ( ((CMV2Particle *)esi)->CMV2Particle__m_LastPos__m_fY );
	fpu_reg13 = ( ((CMV2Particle *)esi)->CMV2Particle__m_NextPos__m_fZ );
	fpu_reg13 = fpu_reg13 - ( ((CMV2Particle *)esi)->CMV2Particle__m_LastPos__m_fZ );
//st0 = NPosZ - LPosZ, st1 = NPosY - LPosY
//st2 = NPosX - LPosX, st3 = MorphFactor

	fpu_reg13 = fpu_reg13 * fpu_reg10;
	{ realnum tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
	fpu_reg13 = fpu_reg13 * fpu_reg10;
	{ realnum tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
	{ realnum tmp = fpu_reg11; fpu_reg11 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg13 = fpu_reg13 * fpu_reg10;
	{ realnum tmp = fpu_reg11; fpu_reg11 = fpu_reg13; fpu_reg13 = tmp; }

	fpu_reg13 = fpu_reg13 + ( ((CMV2Particle *)esi)->CMV2Particle__m_LastPos__m_fZ );
	{ realnum tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
	fpu_reg13 = fpu_reg13 + ( ((CMV2Particle *)esi)->CMV2Particle__m_LastPos__m_fY );
	{ realnum tmp = fpu_reg13; fpu_reg13 = fpu_reg12; fpu_reg12 = tmp; }
	{ realnum tmp = fpu_reg11; fpu_reg11 = fpu_reg13; fpu_reg13 = tmp; }
	fpu_reg13 = fpu_reg13 + ( ((CMV2Particle *)esi)->CMV2Particle__m_LastPos__m_fX );
	{ realnum tmp = fpu_reg11; fpu_reg11 = fpu_reg13; fpu_reg13 = tmp; }

	((CMV2Particle *)esi)->CMV2Particle__m_InterpPos__m_fZ = fpu_reg13;
	((CMV2Particle *)esi)->CMV2Particle__m_InterpPos__m_fY = fpu_reg12;
	((CMV2Particle *)esi)->CMV2Particle__m_InterpPos__m_fX = fpu_reg11;

	esi = esi + ( CMV2Particle__Size );

	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto MV2ParticleInterpolateASM_ParticleLoop;

//ffree	st(0)




	return;
}







