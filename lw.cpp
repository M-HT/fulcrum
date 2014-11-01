// Filename: lw.cpp
//
// Developed by Oliver Belaifa in 1997
//

#include<stdio.h>
#include<string.h>
#include<m2raw.h>
#include<m2except.h>

#define _LW_CPP_DEBUGMODE 0

//Constructors

CLWDot::CLWDot()
{
}


CLWPolygon::CLWPolygon()
{
	m_ppLWDots[0] = NULL;
	m_ppLWDots[1] = NULL;
	m_ppLWDots[2] = NULL;
}


CLWObject::CLWObject()
{
	m_pLWDots = NULL;
	m_lNumLWDots = 0;
	m_pLWPolygons = NULL;
	m_lNumLWPolygons = 0;
	m_pcTextureFileName = NULL;

	m_lFlagXAxis = 1;
	m_lFlagYAxis = 0;
	m_lFlagZAxis = 0;

	m_fXSize = 0;
	m_fYSize = 0.1;
	m_fZSize = 0.1;

	m_fXCenter = 0;
	m_fYCenter = 0;
	m_fZCenter = 0;
}


//Destructors

CLWDot::~CLWDot()
{
}


CLWPolygon::~CLWPolygon()
{
/*
	if (m_ppLWDots[0])
		delete m_ppLWDots[0];

	if (m_ppLWDots[1])
		delete m_ppLWDots[1];

	if (m_ppLWDots[2])
		delete m_ppLWDots[2];
*/
}


CLWObject::~CLWObject()
{
	if (m_pLWDots)
		delete [] m_pLWDots;

	if (m_pLWPolygons)
		delete [] m_pLWPolygons;

	if (m_pcTextureFileName)
		delete [] m_pcTextureFileName;
}



float CLWObject::GetFloat(unsigned char *pcMem)

{
  unsigned char *pcA;
  float fResult;

  pcA = (unsigned char *)&fResult;

  pcA[0] = pcMem[3];
  pcA[1] = pcMem[2];
  pcA[2] = pcMem[1];
  pcA[3] = pcMem[0];

  return fResult;
}



ULONG CLWObject::GetUlong(unsigned char *pcMem)

{
  unsigned char *pcA;
  ULONG lResult;

  pcA = (unsigned char *)&lResult;

  pcA[0] = pcMem[3];
  pcA[1] = pcMem[2];
  pcA[2] = pcMem[1];
  pcA[3] = pcMem[0];

  return lResult;
}



UWORD CLWObject::GetUShortInt(unsigned char *pcMem)

{
  unsigned char *pcA;
  UWORD wResult;

  pcA = (unsigned char *)&wResult;

  pcA[0] = pcMem[1];
  pcA[1] = pcMem[0];

  return wResult;
}



char *CLWObject::GetHunkPtr(const char *pcHunk, char *pcStart, char *pcEnd)

{
	char  *pcCur = pcStart;

	while (pcCur < pcEnd && strncmp(pcCur, pcHunk, 4))
		pcCur++;

	if (pcCur == pcEnd)
	{
/*
		#if _LW_CPP_DEBUGMODE
			printf("'%s' hunk not found.\n", pcHunk);
		#endif
*/

		return NULL;
	}

	return pcCur;
}



char *CLWObject::SetTIMG(char *pcCurScanPTR)

{
	long lChunkLength;
	char *pcPathFileName;
	char *pcFileName;
	char *pcTempSTR;

	pcCurScanPTR += 4;
	lChunkLength = GetUShortInt((unsigned char *)pcCurScanPTR);
	pcCurScanPTR += 2;

	pcPathFileName = strdup(pcCurScanPTR);

	if (pcPathFileName == NULL)
		throw new CMV2NormException("memory allocation error", __FILE__, __LINE__);

	// get only filename

	pcFileName = new char[8 + 1 + 3 + 1];	// name + "." + suffix + 0

	pcTempSTR = strchr(pcPathFileName, *("."));

	if (pcTempSTR == NULL)
	{
		char cError[80];
		sprintf(cError, "invalid filename in TIMG: %s", pcPathFileName);
		throw new CMV2NormException(cError, __FILE__, __LINE__);
	}

	pcTempSTR[0] = 0;		// "." = end
	pcTempSTR = strrchr(pcPathFileName, 92);   // search for backslash

	if (pcTempSTR == NULL)
	{
		pcTempSTR = strrchr(pcPathFileName, *("/"));

		if (pcTempSTR == NULL)
			pcTempSTR = pcPathFileName;
		else
			pcTempSTR++;        // behind the backslash: filename
	}
	else
		pcTempSTR++;            // behind the backslash: filename

	strcpy(pcFileName, pcTempSTR);
	strncat(pcFileName, ".pcx", 4);

	delete [] pcPathFileName;

	m_pcTextureFileName = pcFileName;

	#if DEBUG
		printf(" Name of Image: '%s'\n", m_pcTextureFileName);
	#endif

	pcCurScanPTR += lChunkLength;

	return pcCurScanPTR;
}


char *CLWObject::SetTFLG(char *pcCurScanPTR)

/*
		  1 1 0 0 1 0 0 (bits)
		  | | | | | | |
		| | | | | | `x axis
		| | | | | `y axis
		| | | | `z axis
		| | | `world coords
		| | `negative image
		| `pixel blending
		`antialias

*/


{
  int iTFlagTemp;

  pcCurScanPTR += 6;

  iTFlagTemp = GetUShortInt((unsigned char *)pcCurScanPTR);

  m_lFlagXAxis = (iTFlagTemp&1) ? 1 : 0;
  m_lFlagYAxis = (iTFlagTemp&2) ? 1 : 0;
  m_lFlagZAxis = (iTFlagTemp&4) ? 1 : 0;

  #if DEBUG
	printf(" Texture flag x-axis: %d\n", m_lFlagXAxis);
	printf(" Texture flag y-axis: %d\n", m_lFlagYAxis);
	printf(" Texture flag z-axis: %d\n", m_lFlagZAxis);
  #endif

  pcCurScanPTR += 2;

  return pcCurScanPTR;
}



char *CLWObject::SetTSIZ(char *pcCurScanPTR)

{
  pcCurScanPTR += 6;
  m_fXSize = GetFloat((unsigned char *)pcCurScanPTR);
  pcCurScanPTR += 4;
  m_fYSize = GetFloat((unsigned char *)pcCurScanPTR);
  pcCurScanPTR += 4;
  m_fZSize = GetFloat((unsigned char *)pcCurScanPTR);
  pcCurScanPTR += 4;

  #if DEBUG
	printf(" XSize: %f\n", m_fXSize);
	printf(" YSize: %f\n", m_fYSize);
	printf(" ZSize: %f\n", m_fZSize);
  #endif

  return pcCurScanPTR;
}




char *CLWObject::SetTCTR(char *pcCurScanPTR)

{
  pcCurScanPTR += 6;
  m_fXCenter = GetFloat((unsigned char *)pcCurScanPTR);
  pcCurScanPTR += 4;
  m_fYCenter = GetFloat((unsigned char *)pcCurScanPTR);
  pcCurScanPTR += 4;
  m_fZCenter = GetFloat((unsigned char *)pcCurScanPTR);
  pcCurScanPTR += 4;

  #if DEBUG
	printf(" XCenter: %f\n", m_fXCenter);
	printf(" YCenter: %f\n", m_fYCenter);
	printf(" ZCenter: %f\n", m_fZCenter);
  #endif

  return pcCurScanPTR;
}



void CLWObject::MapPlanar(CMV2Dot3D *pDot)

{
	float fX, fY, fZ;
	float fU, fV;

	fX = pDot->m_pPos->m_Pos.m_fX;
	fY = pDot->m_pPos->m_Pos.m_fY;
	fZ = pDot->m_pPos->m_Pos.m_fZ;

	fX -= m_fXCenter;
	fY -= m_fYCenter;
	fZ -= m_fZCenter;

	fU = (m_lFlagXAxis == 1) ? fZ/m_fZSize + 0.5 : fX/m_fXSize + 0.5;
	fV = (m_lFlagYAxis == 1) ? -fZ/m_fZSize + 0.5 : -fY/m_fYSize + 0.5;

	// Ouch!!
	pDot->m_fTextureU = 0.1*fU*65536.0*256.0;
	pDot->m_fTextureV = 0.1*fV*65536.0*256.0;
}





void CLWObject::LoadLWOB(char *pcFileName)

{
	#if _LW_CPP_DEBUGMODE
		printf("\nMethod: CLWObject::LoadLWOB(char *pcFileName = '%s')\n", pcFileName);
	#endif

	FILE   *pLWOBFile;

	try
	{
		if (!(pLWOBFile = fopen(pcFileName, "rb")))
		{
			char cError[80];
			sprintf(cError, "cannot open file: %s", pcFileName);
			throw new CMV2NormException(cError, __FILE__, __LINE__);
		}

		fseek(pLWOBFile, 0, SEEK_END);
		ULONG lLWOBFileSize = ftell(pLWOBFile);
		fseek(pLWOBFile, 0, SEEK_SET);

		#if _LW_CPP_DEBUGMODE
			printf("  FileSize: %-8d\n", lLWOBFileSize);
		#endif

		char *pcLWOB = new char[lLWOBFileSize];

		if (pcLWOB == NULL)
			throw new CMV2NormException("memory allocation error", __FILE__, __LINE__);

		if (lLWOBFileSize != fread(pcLWOB, 1, lLWOBFileSize, pLWOBFile))
		{
			char cError[80];
			sprintf(cError, "loading error: %s", pcFileName);
			throw new CMV2NormException(cError, __FILE__, __LINE__);
		}

		fclose(pLWOBFile);

		char *pcCurLWOBPtr = pcLWOB;

		if (strncmp(pcCurLWOBPtr, "FORM", 4))
		{
			char cError[80];
			sprintf(cError, "'FORM' hunk not found in %s", pcFileName);
			throw new CMV2NormException(cError, __FILE__, __LINE__);
		}

		pcCurLWOBPtr += 8;

		if (strncmp(pcCurLWOBPtr, "LWOB", 4))
		{
			char cError[80];
			sprintf(cError, "'FORM' hunk not found in %s", pcFileName);
			throw new CMV2NormException(cError, __FILE__, __LINE__);
		}

		pcCurLWOBPtr += 4;

		char *pcDotsPtr = GetHunkPtr("PNTS", pcCurLWOBPtr,
			pcLWOB + lLWOBFileSize);

		if (pcDotsPtr == NULL)
		{
			char cError[80];
			sprintf(cError, "'PNTS' hunk not found in %s", pcFileName);
			throw new CMV2NormException(cError, __FILE__, __LINE__);
		}

		char *pcPolygonsPtr = GetHunkPtr("POLS", pcCurLWOBPtr,
			pcLWOB + lLWOBFileSize);

		if (pcPolygonsPtr == NULL)
		{
			char cError[80];
			sprintf(cError, "'POLS' hunk not found in %s", pcFileName);
			throw new CMV2NormException(cError, __FILE__, __LINE__);
		}

		char *pcTIMGPtr = GetHunkPtr("TIMG", pcCurLWOBPtr,
			pcLWOB + lLWOBFileSize);

		if (pcTIMGPtr == NULL)
		{
			char cError[80];
			sprintf(cError, "'TIMG' hunk not found in %s", pcFileName);
			throw new CMV2NormException(cError, __FILE__, __LINE__);
		}

		char *pcTFLGPtr = GetHunkPtr("TFLG", pcCurLWOBPtr,
			pcLWOB + lLWOBFileSize);

		if (pcTFLGPtr == NULL)
		{
			char cError[80];
			sprintf(cError, "'TFLG' hunk not found in %s", pcFileName);
			throw new CMV2NormException(cError, __FILE__, __LINE__);
		}


		char *pcTSIZPtr = GetHunkPtr("TSIZ", pcCurLWOBPtr,
			pcLWOB + lLWOBFileSize);

		if (pcTSIZPtr == NULL)
		{
			char cError[80];
			sprintf(cError, "'TSIZ' hunk not found in %s", pcFileName);
			throw new CMV2NormException(cError, __FILE__, __LINE__);
		}


		char *pcTCTRPtr = GetHunkPtr("TCTR", pcCurLWOBPtr,
			pcLWOB + lLWOBFileSize);

		if (pcTCTRPtr == NULL)
		{
			char cError[80];
			sprintf(cError, "'TCTR' hunk not found in %s", pcFileName);
			throw new CMV2NormException(cError, __FILE__, __LINE__);
		}

		m_lNumLWDots = GetUlong((unsigned char *)(pcDotsPtr + 4))/12;
			//3*sizeof(float)

		#if _LW_CPP_DEBUGMODE
			printf("  NumLWDots: %-8d\n", m_lNumLWDots);
		#endif

		ULONG lAllPolygonsSize = GetUlong((unsigned char *)(pcPolygonsPtr + 4));
		char *pcScanPtr = pcPolygonsPtr + 8;

		m_lNumLWPolygons = 0;

		while (pcScanPtr < pcPolygonsPtr + 8 + lAllPolygonsSize)
		{
			int iPolygonNumDots = GetUShortInt((unsigned char *)pcScanPtr);

			if (iPolygonNumDots != 3)
			{
				char cError[80];
				sprintf(cError, "found %d edge polygon in file %s",
					iPolygonNumDots, pcFileName);
				throw new CMV2NormException(cError, __FILE__, __LINE__);
			}

			pcScanPtr += 2;	// skip iPolygonNumDots

			pcScanPtr += 2*iPolygonNumDots; // skip Dot IDs

			pcScanPtr += 2; // skip Surface

			m_lNumLWPolygons++;
		}

		#if _LW_CPP_DEBUGMODE
			printf("  NumLWPolygons: %-8d\n", m_lNumLWPolygons);
		#endif

		m_pLWDots = new CLWDot[m_lNumLWDots];
		if (m_pLWDots == NULL)
			throw new CMV2NormException("memory allocation error", __FILE__, __LINE__);

		pcScanPtr = pcDotsPtr + 8;

		unsigned int iI;

		for (iI = 0; iI < m_lNumLWDots; iI++)
		{
			m_pLWDots[iI].m_Pos.m_fX = GetFloat((unsigned char *)pcScanPtr);
			pcScanPtr += 4;

			m_pLWDots[iI].m_Pos.m_fY = GetFloat((unsigned char *)pcScanPtr);// - 3.0;
			pcScanPtr += 4;

			m_pLWDots[iI].m_Pos.m_fZ = GetFloat((unsigned char *)pcScanPtr);
			pcScanPtr += 4;

			m_pLWDots[iI].m_lID = iI;

			#if _LW_CPP_DEBUGMODE
				printf("    Dot %d   x:%f   y:%f   z:%f\n",
					iI,
					m_pLWDots[iI].m_Pos.m_fX,
					m_pLWDots[iI].m_Pos.m_fY,
					m_pLWDots[iI].m_Pos.m_fZ);
			#endif
		}


		m_pLWPolygons = new CLWPolygon[m_lNumLWPolygons];
		if (m_pLWPolygons == NULL)
			throw new CMV2NormException("memory allocation error", __FILE__, __LINE__);

		pcScanPtr = pcPolygonsPtr + 8;

		// Alle Polygone haben 3 Eckpunkte

		for (iI = 0; iI < m_lNumLWPolygons; iI++)
		{
			int iJ;

			pcScanPtr += 2;	// skip iPolygonNumDots
/*
			#if _LW_CPP_DEBUGMODE
				printf("\n");
			#endif
*/


			for (iJ = 0; iJ < 3; iJ++)
			{
				m_pLWPolygons[iI].m_ppLWDots[iJ] =
					&m_pLWDots[GetUShortInt((unsigned char *)pcScanPtr)];

/*
				#if _LW_CPP_DEBUGMODE
					printf("      Dot[%d]: %d", iJ,
						GetUShortInt((unsigned char *)pcScanPtr));
				#endif
*/

				pcScanPtr += 2;
			}

			pcScanPtr += 2;
		}

		SetTIMG(pcTIMGPtr);
		SetTFLG(pcTFLGPtr);
		SetTSIZ(pcTSIZPtr);
		SetTCTR(pcTCTRPtr);

		delete [] pcLWOB;
	}
	catch (CMV2NormException *pException)
	{
		pException->PrintError();
		delete pException;
	}
}


void CLWObject::LoadLWOBPoints(char *pcFileName)

{
	#if _LW_CPP_DEBUGMODE
		printf("\nMethod: CLWObject::LoadLWOB(char *pcFileName = '%s')\n", pcFileName);
	#endif

	FILE   *pLWOBFile;

	try
	{
		if (!(pLWOBFile = fopen(pcFileName, "rb")))
		{
			char cError[80];
			sprintf(cError, "cannot open file: %s", pcFileName);
			throw new CMV2NormException(cError, __FILE__, __LINE__);
		}

		fseek(pLWOBFile, 0, SEEK_END);
		ULONG lLWOBFileSize = ftell(pLWOBFile);
		fseek(pLWOBFile, 0, SEEK_SET);

		#if _LW_CPP_DEBUGMODE
			printf("  FileSize: %-8d\n", lLWOBFileSize);
		#endif

		char *pcLWOB = new char[lLWOBFileSize];

		if (pcLWOB == NULL)
			throw new CMV2NormException("memory allocation error", __FILE__, __LINE__);

		if (lLWOBFileSize != fread(pcLWOB, 1, lLWOBFileSize, pLWOBFile))
		{
			char cError[80];
			sprintf(cError, "loading error: %s", pcFileName);
			throw new CMV2NormException(cError, __FILE__, __LINE__);
		}

		fclose(pLWOBFile);

		char *pcCurLWOBPtr = pcLWOB;

		if (strncmp(pcCurLWOBPtr, "FORM", 4))
		{
			char cError[80];
			sprintf(cError, "'FORM' hunk not found in %s", pcFileName);
			throw new CMV2NormException(cError, __FILE__, __LINE__);
		}

		pcCurLWOBPtr += 8;

		if (strncmp(pcCurLWOBPtr, "LWOB", 4))
		{
			char cError[80];
			sprintf(cError, "'LWOB' hunk not found in %s", pcFileName);
			throw new CMV2NormException(cError, __FILE__, __LINE__);
		}

		pcCurLWOBPtr += 4;

		char *pcDotsPtr = GetHunkPtr("PNTS", pcCurLWOBPtr,
			pcLWOB + lLWOBFileSize);

		if (pcDotsPtr == NULL)
		{
			char cError[80];
			sprintf(cError, "'PNTS' hunk not found in %s", pcFileName);
			throw new CMV2NormException(cError, __FILE__, __LINE__);
		}

		m_lNumLWDots = GetUlong((unsigned char *)(pcDotsPtr + 4))/12;
			//3*sizeof(float)

		#if _LW_CPP_DEBUGMODE
			printf("  NumLWDots: %-8d\n", m_lNumLWDots);
		#endif

		m_pLWDots = new CLWDot[m_lNumLWDots];
		if (m_pLWDots == NULL)
			throw new CMV2NormException("memory allocation error", __FILE__, __LINE__);

		char *pcScanPtr = pcDotsPtr + 8;

		unsigned int iI;
		for (iI = 0; iI < m_lNumLWDots; iI++)
		{
			m_pLWDots[iI].m_Pos.m_fX = 0.3*GetFloat((unsigned char *)pcScanPtr);
			pcScanPtr += 4;

			m_pLWDots[iI].m_Pos.m_fY = -0.3*GetFloat((unsigned char *)pcScanPtr) - 5;
			pcScanPtr += 4;

			m_pLWDots[iI].m_Pos.m_fZ = 0.3*GetFloat((unsigned char *)pcScanPtr);
			pcScanPtr += 4;

			m_pLWDots[iI].m_lID = iI;

			#if _LW_CPP_DEBUGMODE
				printf("    Dot %d   x:%f   y:%f   z:%f\n",
					iI,
					m_pLWDots[iI].m_Pos.m_fX,
					m_pLWDots[iI].m_Pos.m_fY,
					m_pLWDots[iI].m_Pos.m_fZ);
			#endif
		}

		delete [] pcLWOB;
	}
	catch (CMV2NormException *pException)
	{
		pException->PrintError();
		delete pException;
	}
}
