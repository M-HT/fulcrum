#include <cstdint>
#if !defined(NO_FPU_CONTROL)
#include <fpu_control.h>
#endif
#include "cc.h"
//;/*
//.486

//.model flat

//LOCALS

//.data

extern "C" void MV2DrawPolygonTASM(uint32_t _edi);
extern "C" void MV2DrawPolygonTfASM(uint32_t _edi);
extern "C" void MV2DrawPolygonTTrASM(uint32_t _edi);
extern "C" void MV2DrawPolygonTfTrASM(uint32_t _edi);
extern "C" void MV2DrawPolygonTGASM(uint32_t _edi);
extern "C" void MV2DrawPolygonTBASM(uint32_t _edi);


//public ppPolygons
//public pcBackBuffer
//public dwXmax

extern "C"
{
uint32_t ppPolygons;
uint32_t pcBackBuffer;
uint32_t dwXmax;


// profiling
//public iPixelCounter
uint32_t iPixelCounter;
}

static uint32_t iNumPolygons;




//fpucw						dw ?
//oldfpucw					dw ?
//oh_low 						dd ?
//co_low						dd ?
//co_hi						dd ?

//fTemp						dd ?
//lTemp						dd ?
//wTemp						dw ?
//align 4
//bTemp						db ?
//align 4


static void (*PolygonCallTable[8])(uint32_t _edi);

//.code

#include "m2struct.inc.h"




extern "C" void InitPolygonCallTableASM(void) {
	PolygonCallTable[0] = &MV2DrawPolygonTASM;
	PolygonCallTable[1] = &MV2DrawPolygonTfASM;
	PolygonCallTable[2] = &MV2DrawPolygonTTrASM;
	PolygonCallTable[3] = &MV2DrawPolygonTfTrASM;
	PolygonCallTable[4] = &MV2DrawPolygonTGASM;
	PolygonCallTable[5] = &MV2DrawPolygonTBASM;

	return;
}







extern "C" void MV2DrawPolygonsASM(uint32_t _ppPolygons, uint32_t _iNumPolygons, uint32_t _pcBackBuffer, uint32_t _dwXmax) {
	uint32_t eax, edx, ecx, edi, ebx;
	uint32_t stack_var00, stack_var01;




	iPixelCounter = ( 0 ); //*************

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
//	fstcw   oldfpucw
//	mov     ax,oldfpucw
//	and     eax,0fcffh
//	and		eax,0fbffh
//	or		eax,00800h
//	mov     fpucw,ax
//	fldcw   fpucw
#endif

	eax = _ppPolygons;
	eax = eax + ( 4 );
	ebx = _iNumPolygons;
	ecx = _pcBackBuffer;
	ecx = ecx >> ( 2 );
	edx = _dwXmax;

	ppPolygons = eax;
	iNumPolygons = ebx;
	pcBackBuffer = ecx;
	dwXmax = edx;

	edx = ppPolygons;
	ecx = iNumPolygons;
MV2DrawPolygonsASM_PolygonLoop:
	edi = ( *((uint32_t *)(edx)) );
	eax = ( ((CMV2Polygon *)edi)->CMV2Polygon__m_iType );

	stack_var00 = ecx;
	stack_var01 = edx;
	PolygonCallTable[eax](edi);
	edx = stack_var01;
	ecx = stack_var00;

	edx = edx + ( 8 );
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto MV2DrawPolygonsASM_PolygonLoop;

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








