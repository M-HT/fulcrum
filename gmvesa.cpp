#include<stdio.h>
#include<m2except.h>
#include<gmvesa.h>
#include<string.h>


extern "C"
{
	void VesaGetSVgaInfoASM(CVesaInfo *pVesaInfo);
	long VesaGetModeInfoASM(ULONG lMode, CVesaModeInfo *pVesaModeInfo);
	void *GetLfbMappedPtrASM(void *pLfb);
	long VesaGraphicsOnASM(ULONG Mode);
	void VesaGraphicsOffASM();
	void CopyBufferToScreenASM(CGfxModeVesa *pGfxModeVesa,
		CVesaMode *pVesaMode, char *pScreenBuffer);
	void SetPaletteASM(char *cPalette, SBYTE cDelta);
}




CVesaInfo::CVesaInfo()
{
}



CVesaInfo::~CVesaInfo()
{
}



CVesaModeInfo::CVesaModeInfo()
{
}



CVesaModeInfo::~CVesaModeInfo()
{
}


CVesaMode::CVesaMode()
{
}


CVesaMode::~CVesaMode()
{
}



CGfxModeVesa::CGfxModeVesa()

{
	VesaGetSVgaInfoASM(&m_VesaInfo);

	if (!strcmp("VESA", m_VesaInfo.m_cSignature))
	{
		m_bVesaSupported = 1;
		m_fVesaVersion = ((float)m_VesaInfo.m_wVersion)/256.0;
	}
	else
		m_bVesaSupported = 0;

	PrintVesaBanner();
	SearchSupportedModes();
}



CGfxModeVesa::~CGfxModeVesa()
{
}



void CGfxModeVesa::PrintVesaBanner()

{
	if (m_bVesaSupported)
	{
		char *cOemName = (char *)(m_VesaInfo.m_wOemNameSegment*16 +
			m_VesaInfo.m_wOemNameOffset);

		printf("\n");
		printf("  þ VESA %.1f found.\n", m_fVesaVersion);
		printf("  þ OEM Extension: %s\n", cOemName);

	}
	else
		printf("\n  þ VESA not found.\n");
}






void CGfxModeVesa::SetOneVesaMode(UWORD wMode, CVesaMode *pVesaMode,
	CVesaModeInfo *pVesaModeInfo)

{
	pVesaMode->m_wId = wMode;
	pVesaMode->m_wXmax = pVesaModeInfo->m_wXmax;
	pVesaMode->m_wYmax = pVesaModeInfo->m_wYmax;
	pVesaMode->m_bBitsPerPixel = pVesaModeInfo->m_bBitsPerPixel;
	pVesaMode->m_bBytesPerPixel = pVesaModeInfo->m_bBitsPerPixel/8;

	if (pVesaModeInfo->m_bBitsPerPixel%8)
		pVesaMode->m_bBytesPerPixel++;

	try
	{
		if (!pVesaModeInfo->m_wModeAttributes&1)
			throw new CMV2NormException("Error in searching Vesa Modes", __FILE__, __LINE__);
	}
	catch (CMV2NormException *pException)
	{
		pException->PrintError();
		delete pException;
	}

	if (m_fVesaVersion >= 2.0)
	{
		if (pVesaModeInfo->m_wModeAttributes&128)
		{
			m_pLfb = (void *)pVesaModeInfo->m_lPhysicalAddressLfb;
			m_pMappedLfb = GetLfbMappedPtrASM(m_pLfb);

			pVesaMode->m_bLfbSupported = 1;
		}
		else
			pVesaMode->m_bLfbSupported = 0;
	}
}




void CGfxModeVesa::SearchSupportedModes()

{
	UWORD *pwFirstMode = (UWORD *)(m_VesaInfo.m_wSupportedModesSegment*16 +
		m_VesaInfo.m_wSupportedModesOffset);

	UWORD *pwModes;

	CVesaModeInfo VesaModeInfo;
	CVesaMode *pFirstVesaMode, *pVesaModes;

	UBYTE bBitsPerPixel;

	UWORD wNumVesaModes = 0;

	pwModes = pwFirstMode;
	while (*pwModes != 0xffff)
	{
		if (VesaGetModeInfoASM((ULONG)*pwModes, &VesaModeInfo))
		{
			bBitsPerPixel = VesaModeInfo.m_bBitsPerPixel;
/*
			if (BitsPerPixel == 8 || BitsPerPixel == 15 ||
				BitsPerPixel == 16 || BitsPerPixel == 32)
				wNumVesaModes++;
*/
			if (bBitsPerPixel == 32)
				wNumVesaModes++;
		}
		pwModes++;
	}

	pFirstVesaMode = pVesaModes = new CVesaMode[wNumVesaModes + 1];
		// NumVesaModes + Mode 0x13

	try
	{
		if (pFirstVesaMode == NULL)
			throw new CMV2NormException("Memory allocation error", __FILE__, __LINE__);
	}
	catch(CMV2NormException *Exception)
	{
		Exception->PrintError();
		delete Exception;
	}

	pwModes = pwFirstMode;

	while (*pwModes != 0xffff)
	{
		if (VesaGetModeInfoASM((ULONG)*pwModes, &VesaModeInfo))

		if (VesaModeInfo.m_bBitsPerPixel == 32)
		{
			SetOneVesaMode(*pwModes, pVesaModes, &VesaModeInfo);
			pVesaModes++;
		}

	  pwModes++;
	}

	// settings for mode 0x13 (320x200x8bit)

	pVesaModes->m_wId = 0x13;
	pVesaModes->m_wXmax = 320;
	pVesaModes->m_wYmax = 200;
	pVesaModes->m_bBitsPerPixel = 8;
	pVesaModes->m_bBytesPerPixel = 1;
	pVesaModes->m_bLfbSupported = 0;

	pVesaModes = pFirstVesaMode;
/*
// debugging
	for (long i = 0; i <= wNumVesaModes; i++)
	{
		printf("mode nr.: %.4X  -  %dx%dx%d    (%d bytes per pixel)  LfbSupport:%d\n",
		pVesaModes[i].m_wId,
		pVesaModes[i].m_wXmax,
		pVesaModes[i].m_wYmax,
		pVesaModes[i].m_bBitsPerPixel,
		pVesaModes[i].m_bBytesPerPixel,
		pVesaModes[i].m_bLfbSupported);
	}
// debugging ends
*/

	m_pVesaModes = pFirstVesaMode;
	m_wNumVesaModes = wNumVesaModes;
}



void CGfxModeVesa::SetVesaMode(UWORD wLocalId)
{
	if (wLocalId <= m_wNumVesaModes)
	{
		try
		{
			if (m_pVesaModes[wLocalId].m_bLfbSupported)
			{
				if (!VesaGraphicsOnASM((ULONG)(0x4000 + m_pVesaModes[wLocalId].m_wId)))
					throw new CMV2NormException("Cannot switch in Graphic Mode", __FILE__, __LINE__);
			}
			else
			{
				if (!VesaGraphicsOnASM((ULONG)m_pVesaModes[wLocalId].m_wId))
					throw new CMV2NormException("Cannot switch in Graphic Mode", __FILE__, __LINE__);
			}

			m_lCurrentModeLocalId = (ULONG)wLocalId;

			m_lXmax = m_pVesaModes[wLocalId].m_wXmax;
			m_lYmax = m_pVesaModes[wLocalId].m_wYmax;
			m_lClippingXmin = 0;
			m_lClippingXmax = m_lXmax;
			m_lClippingYmin = 0;
			m_lClippingYmax = m_lYmax;


		}
		catch(CMV2NormException *pException)
		{
			pException->PrintError();
			delete pException;
		}

	}
}


void CGfxModeVesa::VesaOff()

{
	VesaGraphicsOffASM();
}



void CGfxModeVesa::CopyBufferToScreen(char *pcScreenBuffer)
{
	CopyBufferToScreenASM(this, &m_pVesaModes[m_lCurrentModeLocalId], pcScreenBuffer);
}






void CGfxModeVesa::ChooseGfxMode()
{
	UWORD wI;
	UWORD wLocalId;

	for (wI = 0; wI <= m_wNumVesaModes; wI++)
//	for (wI = 0; wI < m_wNumVesaModes; wI++)
	{
		printf("      %.2d:     %.4X  -  %dx%d",
			wI,
			m_pVesaModes[wI].m_wId,
			m_pVesaModes[wI].m_wXmax,
			m_pVesaModes[wI].m_wYmax);

		if (m_pVesaModes[wI].m_bLfbSupported)
			printf("l\n");
	}

	printf("\n");

	fscanf(stdin, "%d", &wLocalId);

	try
	{
		if (wLocalId > m_wNumVesaModes)
			throw new CMV2NormException("Invalid LocalId", __FILE__, __LINE__);
	}
	catch (CMV2NormException *pException)
	{
		pException->PrintError();
		delete pException;
	}

	SetVesaMode(wLocalId);
}



void CGfxModeVesa::SetPalette(char *cPalette, SBYTE bDelta)
{
	SetPaletteASM(cPalette, bDelta);
}