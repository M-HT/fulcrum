// Filename: include\gmvesa.h
//
// Developed by Oliver Belaifa in 1997
//

#include<stypes.h>

#ifndef _GMVESA_H_INCLUDED
#define _GMVESA_H_INCLUDED

class CVesaInfo
{
public:
	char    m_cSignature[4];
	UWORD	m_wVersion;
	UWORD	m_wOemNameOffset;
	UWORD	m_wOemNameSegment;
	UBYTE	m_bCapabilities[4];
	UWORD	m_wSupportedModesOffset;
	UWORD	m_wSupportedModesSegment;
	UBYTE  *m_pbReserved;

	char	dummy[256];

	CVesaInfo();
	~CVesaInfo();
};



class CVesaModeInfo
{
public:
	UWORD	m_wModeAttributes;
	UBYTE	m_bWindowAAttributes;
	UBYTE	m_bWindowBAttributes;
	UWORD	m_wWindowGranularity;
	UWORD	m_wWindowSize;
	UWORD	m_wSegmentWindowA;
	UWORD	m_wSegmentWindowB;

	ULONG	m_wWindowPosFuncPtr;
	UWORD	m_wBytesPerScanline;

	UWORD	m_wXmax;
	UWORD	m_wYmax;

	UBYTE	m_bTextXmax;
	UBYTE	m_bTextYmax;

	UBYTE	m_bNumMemoryPlanes;
	UBYTE	m_bBitsPerPixel;
	UBYTE	m_bNumBanks;
	UBYTE	m_bMemoryModelType;
	UBYTE	m_bSizeOfBank;			// in kb
	UBYTE	m_bNumImagePages;
	UBYTE	m_bReserved1;

	// vesa 1.2+

	UBYTE	m_bRedMaskSize;
	UBYTE	m_bRedFieldPos;
	UBYTE	m_bGreenMaskSize;
	UBYTE	m_bGreenFieldPos;
	UBYTE	m_bBlueMaskSize;
	UBYTE	m_bBlueFieldPos;
	UBYTE	m_bReservedMaskSize;
	UBYTE	m_bReservedFieldPos;
	UBYTE	m_bDirectColModeInfo;

	// vesa 2.0

	ULONG	m_lPhysicalAddressLfb;
	ULONG	m_lPtrOffscreenMem;
	UWORD	m_wOffscreenMemSize;

	UBYTE  *m_bReserved2;

	char	dummy[256];

	CVesaModeInfo();
	~CVesaModeInfo();
};



class CVesaMode
{
public:
	UWORD	m_wId;
	UWORD	m_wXmax;
	UWORD	m_wYmax;
	UWORD	m_wBytesPerScanline;
	UBYTE	m_bBitsPerPixel;
	UBYTE	m_bBytesPerPixel;
	UWORD	m_wLocalId;
	UBYTE	m_bLfbSupported;

	CVesaMode();
	~CVesaMode();
};

class CGfxModeVesa

{
public:
	UBYTE 		m_bVesaSupported;
	float		m_fVesaVersion;
	ULONG		m_lCurrentModeLocalId;
	void	   *m_pLfb;
	void	   *m_pMappedLfb;
	ULONG		m_lXmax;
	ULONG		m_lYmax;
	ULONG		m_lClippingXmin;
	ULONG		m_lClippingXmax;
	ULONG		m_lClippingYmin;
	ULONG		m_lClippingYmax;

	CVesaInfo   m_VesaInfo;     // Vesa info buffer
	CVesaMode  *m_pVesaModes;
	UWORD		m_wNumVesaModes;

	CGfxModeVesa();
	~CGfxModeVesa();

	void ChooseGfxMode();
	void CopyBufferToScreen(char *pcScreenBuffer);
	void PrintVesaBanner();
	void SearchSupportedModes();
	void SetOneVesaMode(UWORD wMode, CVesaMode *pVesaMode,
		CVesaModeInfo *pVesaModeInfo);
	void SetPalette(char *cPalette, SBYTE bDelta);
	void SetVesaMode(UWORD wLocalId);
	void VesaOff();
};


#endif