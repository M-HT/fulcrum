// Filename: m2clip.cpp
//
// Developed by Oliver Belaifa in 1997
//


#include<stdlib.h>
#include<m2render.h>




extern "C"
{
	void MV2SelectClipPolygonsASM(CMV2Polygon **ppSrcList,
		CMV2Polygon **ppSrcCList, CMV2Polygon **ppDestList,
		int iNumSrcList, int *piNumSrcCList, int *piNumDestList);

}




// profiling
int iAddPolys;
int iVSpacePolys;
int iToBeClipped;
int iDrawedPolys;
//


static CMV2Polygon *pPolygon;
static int iPolygonType;
static int iPolygonFlag;
static CMV2Dot3D *pDot1;
static CMV2Dot3D *pDot2;
static CMV2Dot3D *pDot3;
static CMV2Dot3DPos *pDotPos1;
static CMV2Dot3DPos *pDotPos2;
static CMV2Dot3DPos *pDotPos3;
static int iClipFlag1;
static int iClipFlag2;
static int iClipFlag3;
static float fScreenXFactor = 1.0/(1024.0*1024.0);


float RoundUP(float fNum)
{
	if (fNum >= 0)
		return float(1 + long(fNum));
	else
		return float(long(fNum));
}





void CMV2Render::Clip2D()
{

	iAddPolys = m_iCurNumSrcPolygonList;	//****************

	int iI;

	for (iI = 0; iI < m_iCurNumSrcPolygonList; iI++)
	{
		pPolygon = m_ppSrcPolygonList[iI];
		pDot1 = pPolygon->m_pDot1;
		pDot2 = pPolygon->m_pDot2;
		pDot3 = pPolygon->m_pDot3;
		pDotPos1 = pDot1->m_pPos;
		pDotPos2 = pDot2->m_pPos;
		pDotPos3 = pDot3->m_pPos;
		iClipFlag1 = pDotPos1->m_iClipFlag;
		iClipFlag2 = pDotPos2->m_iClipFlag;
		iClipFlag3 = pDotPos3->m_iClipFlag;
/*
		int iClipFlagAND = iClipFlag1 & iClipFlag2 & iClipFlag3;

		if (!iClipFlagAND)
		{

			int iClipFlagOR = iClipFlag1 | iClipFlag2 | iClipFlag3;
			if (iClipFlagOR)
			{
				m_ppSrcCPolygonList[m_iCurNumSrcCPolygonList] = pPolygon;
				m_iCurNumSrcCPolygonList++;
			}
			else
			{
				// polygon in viewspace
				m_ppDestPolygonList[m_iCurNumDestPolygonList] = pPolygon;
				m_iCurNumDestPolygonList++;
			}
		}
*/
	}

	MV2SelectClipPolygonsASM(m_ppSrcPolygonList,
		m_ppSrcCPolygonList, m_ppDestPolygonList,
		m_iCurNumSrcPolygonList, &m_iCurNumSrcCPolygonList,
		&m_iCurNumDestPolygonList);

	iVSpacePolys = m_iCurNumDestPolygonList; //**************
	iToBeClipped = m_iCurNumSrcCPolygonList; //**************

	CMV2Polygon **ppTempPolygon;
	int	iTempNumPolygon;

	// Zmin
	for (iI = 0; iI < m_iCurNumSrcCPolygonList; iI++)
	{
		pPolygon = m_ppSrcCPolygonList[iI];
		Clip2DZ(0.1, 1.0);
	}
	ppTempPolygon = m_ppSrcCPolygonList;
	m_ppSrcCPolygonList = m_ppDestCPolygonList;
	m_ppDestCPolygonList = ppTempPolygon;
	m_iCurNumSrcCPolygonList = m_iCurNumDestCPolygonList;
	m_iCurNumDestCPolygonList = 0;

	// Xmin
	for (iI = 0; iI < m_iCurNumSrcCPolygonList; iI++)
	{
		pPolygon = m_ppSrcCPolygonList[iI];
		Clip2D(m_fCXmin, 0, 0, -1.0);
	}

	ppTempPolygon = m_ppSrcCPolygonList;
	m_ppSrcCPolygonList = m_ppDestCPolygonList;
	m_ppDestCPolygonList = ppTempPolygon;
	m_iCurNumSrcCPolygonList = m_iCurNumDestCPolygonList;
	m_iCurNumDestCPolygonList = 0;

	// Ymin
	for (iI = 0; iI < m_iCurNumSrcCPolygonList; iI++)
	{
		pPolygon = m_ppSrcCPolygonList[iI];
		Clip2D(0, m_fCYmin, 1.0, 0);

	}

	ppTempPolygon = m_ppSrcCPolygonList;
	m_ppSrcCPolygonList = m_ppDestCPolygonList;
	m_ppDestCPolygonList = ppTempPolygon;
	m_iCurNumSrcCPolygonList = m_iCurNumDestCPolygonList;
	m_iCurNumDestCPolygonList = 0;


	// Ymax
	for (iI = 0; iI < m_iCurNumSrcCPolygonList; iI++)
	{
		pPolygon = m_ppSrcCPolygonList[iI];
		Clip2D(m_fCXmax, 0.0, 0, 1.0);

	}

	ppTempPolygon = m_ppSrcCPolygonList;
	m_ppSrcCPolygonList = m_ppDestCPolygonList;
	m_ppDestCPolygonList = ppTempPolygon;
	m_iCurNumSrcCPolygonList = m_iCurNumDestCPolygonList;
	m_iCurNumDestCPolygonList = 0;

	// Xmax
	for (iI = 0; iI < m_iCurNumSrcCPolygonList; iI++)
	{
		pPolygon = m_ppSrcCPolygonList[iI];
		Clip2D(0, m_fCYmax, -1.0, 0);
	}

	ppTempPolygon = m_ppSrcCPolygonList;
	m_ppSrcCPolygonList = m_ppDestCPolygonList;
	m_ppDestCPolygonList = ppTempPolygon;
	m_iCurNumSrcCPolygonList = m_iCurNumDestCPolygonList;
	m_iCurNumDestCPolygonList = 0;


	// All Polygons clipped yet (all in viewspace)
	for (iI = 0; iI < m_iCurNumSrcCPolygonList; iI++)
	{
		m_ppDestPolygonList[m_iCurNumDestPolygonList] =
			m_ppSrcCPolygonList[iI];

		m_iCurNumDestPolygonList++;
	}

	ppTempPolygon = m_ppSrcPolygonList;
	m_ppSrcPolygonList = m_ppDestPolygonList;
	m_ppDestPolygonList = ppTempPolygon;
	m_iCurNumSrcPolygonList = m_iCurNumDestPolygonList;
	m_iCurNumDestPolygonList = 0;

	iDrawedPolys = m_iCurNumSrcPolygonList;	//*****************
}





void CMV2Render::Clip2D(float fCOx, float fCOy, float fCRx, float fCRy)
{
	float fNx = -fCRy;
	float fNy = fCRx;

	iPolygonType = pPolygon->m_iType;
	iPolygonFlag = pPolygon->m_iFlag;
	pDot1 = pPolygon->m_pDot1;
	pDot2 = pPolygon->m_pDot2;
	pDot3 = pPolygon->m_pDot3;
	pDotPos1 = pDot1->m_pPos;
	pDotPos2 = pDot2->m_pPos;
	pDotPos3 = pDot3->m_pPos;

	int iNumOuterDots = 0;

	if ((pDotPos1->m_fScreenX - fCOx)*fNx +
		(pDotPos1->m_fScreenY - fCOy)*fNy < 0)
	{
		pDotPos1->m_iClipFlag = -1;
		iNumOuterDots++;
	}
	else
		pDotPos1->m_iClipFlag = 0;


	if ((pDotPos2->m_fScreenX - fCOx)*fNx +
		(pDotPos2->m_fScreenY - fCOy)*fNy < 0)
	{
		pDotPos2->m_iClipFlag = -1;
		iNumOuterDots++;
	}
	else
		pDotPos2->m_iClipFlag = 0;


	if ((pDotPos3->m_fScreenX - fCOx)*fNx +
		(pDotPos3->m_fScreenY - fCOy)*fNy < 0)
	{
		pDotPos3->m_iClipFlag = -1;
		iNumOuterDots++;
	}
	else
		pDotPos3->m_iClipFlag = 0;


	if (iNumOuterDots != 3)
	{
		if (iNumOuterDots > 0)
		{
			CMV2Dot3DPos *pTempDotPos;
			CMV2Dot3D *pTempDot;

			if (iNumOuterDots == 1)
			{
				if (pDotPos2->m_iClipFlag)
					pTempDot = pDot1, pDot1 = pDot2,
					pDot2 = pDot3, pDot3 = pTempDot,
					pTempDotPos = pDotPos1, pDotPos1 = pDotPos2,
					pDotPos2 = pDotPos3, pDotPos3 = pTempDotPos;
				else
					if (pDotPos3->m_iClipFlag)
						pTempDot = pDot1, pDot1 = pDot3,
						pDot3 = pDot2, pDot2 = pTempDot,
						pTempDotPos = pDotPos1, pDotPos1 = pDotPos3,
						pDotPos3 = pDotPos2, pDotPos2 = pTempDotPos;
			}
			else
			{
				if (!pDotPos2->m_iClipFlag)
					pTempDot = pDot1, pDot1 = pDot2,
					pDot2 = pDot3, pDot3 = pTempDot,
					pTempDotPos = pDotPos1, pDotPos1 = pDotPos2,
					pDotPos2 = pDotPos3, pDotPos3 = pTempDotPos;
				else
					if (!pDotPos3->m_iClipFlag)
						pTempDot = pDot1, pDot1 = pDot3,
						pDot3 = pDot2, pDot2 = pTempDot,
						pTempDotPos = pDotPos1, pDotPos1 = pDotPos3,
						pDotPos3 = pDotPos2, pDotPos2 = pTempDotPos;
			}

			float fScreenX1 = pDotPos1->m_fScreenX;
			float fScreenY1 = pDotPos1->m_fScreenY;
			float fScreenX2 = pDotPos2->m_fScreenX;
			float fScreenY2 = pDotPos2->m_fScreenY;
			float fScreenX3 = pDotPos3->m_fScreenX;
			float fScreenY3 = pDotPos3->m_fScreenY;

			// Two lines:  First: (Dot2 - Dot1) Second: (Dot3 - Dot1)

			float fDeltaX1 = fScreenX2 - fScreenX1;
			float fDeltaY1 = fScreenY2 - fScreenY1;
			float fDeltaX2 = fScreenX3 - fScreenX1;
			float fDeltaY2 = fScreenY3 - fScreenY1;

			float fOx = fCOx - fScreenX1;
			float fOy = fCOy - fScreenY1;

			float fNDet1 = fDeltaX1*fCRy - fCRx*fDeltaY1;
			float fNDet2 = fDeltaX2*fCRy - fCRx*fDeltaY2;
			float fTDet = fOx*fCRy - fCRx*fOy;

			float fT1 = fTDet/fNDet1;
			float fT2 = fTDet/fNDet2;


			// OneDivZ

			float fOneDivZ1 = pDotPos1->m_fZNewRZP;
			float fOneDivZ2 = pDotPos2->m_fZNewRZP;
			float fOneDivZ3 = pDotPos3->m_fZNewRZP;

			float fDeltaOneDivZ1 = fOneDivZ2 - fOneDivZ1;
			float fDeltaOneDivZ2 = fOneDivZ3 - fOneDivZ1;

			float fOneDivZNew1 = fDeltaOneDivZ1*fT1 + fOneDivZ1;
			float fOneDivZNew2 = fDeltaOneDivZ2*fT2 + fOneDivZ1;

			float fZNew1 = 1.0/fOneDivZNew1;
			float fZNew2 = 1.0/fOneDivZNew2;

			// TextureMapping

			float fTUDivZ1 = pDot1->m_fTextureU*fOneDivZ1;
			float fTVDivZ1 = pDot1->m_fTextureV*fOneDivZ1;
			float fTUDivZ2 = pDot2->m_fTextureU*fOneDivZ2;
			float fTVDivZ2 = pDot2->m_fTextureV*fOneDivZ2;
			float fTUDivZ3 = pDot3->m_fTextureU*fOneDivZ3;
			float fTVDivZ3 = pDot3->m_fTextureV*fOneDivZ3;

			float fDeltaTUDivZ1 = fTUDivZ2 - fTUDivZ1;
			float fDeltaTVDivZ1 = fTVDivZ2 - fTVDivZ1;
			float fDeltaTUDivZ2 = fTUDivZ3 - fTUDivZ1;
			float fDeltaTVDivZ2 = fTVDivZ3 - fTVDivZ1;

			float fTU1 = (fDeltaTUDivZ1*fT1 + fTUDivZ1)*fZNew1;
			float fTV1 = (fDeltaTVDivZ1*fT1 + fTVDivZ1)*fZNew1;

			float fTU2 = (fDeltaTUDivZ2*fT2 + fTUDivZ1)*fZNew2;
			float fTV2 = (fDeltaTVDivZ2*fT2 + fTVDivZ1)*fZNew2;

			// RGB

			float fR1 = pDotPos1->m_fR;
			float fG1 = pDotPos1->m_fG;
			float fB1 = pDotPos1->m_fB;
			float fR2 = pDotPos2->m_fR;
			float fG2 = pDotPos2->m_fG;
			float fB2 = pDotPos2->m_fB;
			float fR3 = pDotPos3->m_fR;
			float fG3 = pDotPos3->m_fG;
			float fB3 = pDotPos3->m_fB;

			float fDeltaR1 = fR2 - fR1;
			float fDeltaG1 = fG2 - fG1;
			float fDeltaB1 = fB2 - fB1;
			float fDeltaR2 = fR3 - fR1;
			float fDeltaG2 = fG3 - fG1;
			float fDeltaB2 = fB3 - fB1;

			float fRNew1 = fDeltaR1*fT1 + fR1;
			float fGNew1 = fDeltaG1*fT1 + fG1;
			float fBNew1 = fDeltaB1*fT1 + fB1;
			float fRNew2 = fDeltaR2*fT2 + fR1;
			float fGNew2 = fDeltaG2*fT2 + fG1;
			float fBNew2 = fDeltaB2*fT2 + fB1;

			// Get the new (clipped) dots
			CMV2Dot3DPos *pNewDotPos1 = &m_pCDot3DPos[m_iCurNumCDot3DPos];
			CMV2Dot3DPos *pNewDotPos2 = &m_pCDot3DPos[m_iCurNumCDot3DPos + 1];
			CMV2Dot3D *pNewDot1 = &m_pCDot3D[m_iCurNumCDot3D];
			CMV2Dot3D *pNewDot2 = &m_pCDot3D[m_iCurNumCDot3D + 1];

			m_iCurNumCDot3DPos += 2;
			m_iCurNumCDot3D += 2;

			// Set the Dot3DPos structures
			pNewDotPos1->m_fScreenX = fDeltaX1*fT1 + fScreenX1;
			pNewDotPos1->m_fScreenY = fDeltaY1*fT1 + fScreenY1;
			pNewDotPos1->m_iScreenX = int(pNewDotPos1->m_fScreenX*fScreenXFactor);
			pNewDotPos1->m_iScreenY = int(pNewDotPos1->m_fScreenY) + 1;
			pNewDotPos1->m_fScreenYError = RoundUP(pNewDotPos1->m_fScreenY)
				- pNewDotPos1->m_fScreenY;
			pNewDotPos1->m_TransfPos.m_fZ = fZNew1;
			pNewDotPos1->m_fZNewRZP = fOneDivZNew1;
			pNewDotPos1->m_fR = fRNew1;
			pNewDotPos1->m_fG = fGNew1;
			pNewDotPos1->m_fB = fBNew1;


			pNewDotPos2->m_fScreenX = fDeltaX2*fT2 + fScreenX1;
			pNewDotPos2->m_fScreenY = fDeltaY2*fT2 + fScreenY1;
			pNewDotPos2->m_iScreenX = int(pNewDotPos2->m_fScreenX*fScreenXFactor);
			pNewDotPos2->m_iScreenY = int(pNewDotPos2->m_fScreenY) + 1;
			pNewDotPos2->m_fScreenYError = RoundUP(pNewDotPos2->m_fScreenY)
				- pNewDotPos2->m_fScreenY;
			pNewDotPos2->m_TransfPos.m_fZ = fZNew2;
			pNewDotPos2->m_fZNewRZP = fOneDivZNew2;
			pNewDotPos2->m_fR = fRNew2;
			pNewDotPos2->m_fG = fGNew2;
			pNewDotPos2->m_fB = fBNew2;

			// Set the Dot3D structures
			pNewDot1->m_fTextureU = fTU1;
			pNewDot1->m_fTextureV = fTV1;
			pNewDot1->m_pPos = pNewDotPos1;

			pNewDot2->m_fTextureU = fTU2;
			pNewDot2->m_fTextureV = fTV2;
			pNewDot2->m_pPos = pNewDotPos2;


			if (iNumOuterDots == 1)
			{
				CMV2Polygon *pNewPolygon1 = &m_pCPolygon[m_iCurNumCPolygon];
				CMV2Polygon *pNewPolygon2 = &m_pCPolygon[m_iCurNumCPolygon + 1];
				m_iCurNumCPolygon += 2;

				pNewPolygon1->m_iType = iPolygonType;
				pNewPolygon1->m_iFlag = iPolygonFlag;
				pNewPolygon1->m_pDot1 = pNewDot1;
				pNewPolygon1->m_pDot2 = pDot2;
				pNewPolygon1->m_pDot3 = pDot3;
				pNewPolygon1->m_pcTexture = pPolygon->m_pcTexture;
				pNewPolygon1->m_pcBumpmap = pPolygon->m_pcBumpmap;
				pNewPolygon1->m_fAddDistance = pPolygon->m_fAddDistance;

				pNewPolygon2->m_iType = iPolygonType;
				pNewPolygon2->m_iFlag = iPolygonFlag;
				pNewPolygon2->m_pDot1 = pNewDot1;
				pNewPolygon2->m_pDot2 = pDot3;
				pNewPolygon2->m_pDot3 = pNewDot2;
				pNewPolygon2->m_pcTexture = pPolygon->m_pcTexture;
				pNewPolygon2->m_pcBumpmap = pPolygon->m_pcBumpmap;
				pNewPolygon2->m_fAddDistance = pPolygon->m_fAddDistance;

				m_ppDestCPolygonList[m_iCurNumDestCPolygonList] = pNewPolygon1;
				m_ppDestCPolygonList[m_iCurNumDestCPolygonList + 1] = pNewPolygon2;

				m_iCurNumDestCPolygonList += 2;
			}
			else
			{
				// iNumOuterDots = 2;

				CMV2Polygon *pNewPolygon1 = &m_pCPolygon[m_iCurNumCPolygon];
				m_iCurNumCPolygon++;

				pNewPolygon1->m_iType = iPolygonType;
				pNewPolygon1->m_iFlag = iPolygonFlag;
				pNewPolygon1->m_pDot1 = pDot1;
				pNewPolygon1->m_pDot2 = pNewDot1;
				pNewPolygon1->m_pDot3 = pNewDot2;
				pNewPolygon1->m_pcTexture = pPolygon->m_pcTexture;
				pNewPolygon1->m_pcBumpmap = pPolygon->m_pcBumpmap;
				pNewPolygon1->m_fAddDistance = pPolygon->m_fAddDistance;

				m_ppDestCPolygonList[m_iCurNumDestCPolygonList] = pNewPolygon1;
				m_iCurNumDestCPolygonList++;
			}
		}
		else
		{
				m_ppDestCPolygonList[m_iCurNumDestCPolygonList] = pPolygon;
				m_iCurNumDestCPolygonList++;
		}
	}
}










void CMV2Render::Clip2DZ(float fZ, float fZd)
	// fZ: 	ClipPlane
	// fZd: Direction of NormVector
{
	iPolygonType = pPolygon->m_iType;
	iPolygonFlag = pPolygon->m_iFlag;

	pDot1 = pPolygon->m_pDot1;
	pDot2 = pPolygon->m_pDot2;
	pDot3 = pPolygon->m_pDot3;
	pDotPos1 = pDot1->m_pPos;
	pDotPos2 = pDot2->m_pPos;
	pDotPos3 = pDot3->m_pPos;

	int iNumOuterDots = 0;


	if ((pDotPos1->m_TransfPos.m_fZ - fZ)*fZd < 0)
	{
		pDotPos1->m_iClipFlag = -1;
		iNumOuterDots++;
	}
	else
		pDotPos1->m_iClipFlag = 0;


	if ((pDotPos2->m_TransfPos.m_fZ - fZ)*fZd < 0)
	{
		pDotPos2->m_iClipFlag = -1;
		iNumOuterDots++;
	}
	else
		pDotPos2->m_iClipFlag = 0;


	if ((pDotPos3->m_TransfPos.m_fZ - fZ)*fZd < 0)
	{
		pDotPos3->m_iClipFlag = -1;
		iNumOuterDots++;
	}
	else
		pDotPos3->m_iClipFlag = 0;



	if (iNumOuterDots != 3)
	{
		if (iNumOuterDots > 0)
		{
			CMV2Dot3DPos *pTempDotPos;
			CMV2Dot3D *pTempDot;

			if (iNumOuterDots == 1)
			{
				if (pDotPos2->m_iClipFlag)
					pTempDot = pDot1, pDot1 = pDot2,
					pDot2 = pDot3, pDot3 = pTempDot,
					pTempDotPos = pDotPos1, pDotPos1 = pDotPos2,
					pDotPos2 = pDotPos3, pDotPos3 = pTempDotPos;
				else
					if (pDotPos3->m_iClipFlag)
						pTempDot = pDot1, pDot1 = pDot3,
						pDot3 = pDot2, pDot2 = pTempDot,
						pTempDotPos = pDotPos1, pDotPos1 = pDotPos3,
						pDotPos3 = pDotPos2, pDotPos2 = pTempDotPos;
			}
			else
			{
				if (!pDotPos2->m_iClipFlag)
					pTempDot = pDot1, pDot1 = pDot2,
					pDot2 = pDot3, pDot3 = pTempDot,
					pTempDotPos = pDotPos1, pDotPos1 = pDotPos2,
					pDotPos2 = pDotPos3, pDotPos3 = pTempDotPos;
				else
					if (!pDotPos3->m_iClipFlag)
						pTempDot = pDot1, pDot1 = pDot3,
						pDot3 = pDot2, pDot2 = pTempDot,
						pTempDotPos = pDotPos1, pDotPos1 = pDotPos3,
						pDotPos3 = pDotPos2, pDotPos2 = pTempDotPos;
			}

			float fX1 = pDotPos1->m_TransfPos.m_fX;
			float fY1 = pDotPos1->m_TransfPos.m_fY;
			float fZ1 = pDotPos1->m_TransfPos.m_fZ;

			float fX2 = pDotPos2->m_TransfPos.m_fX;
			float fY2 = pDotPos2->m_TransfPos.m_fY;
			float fZ2 = pDotPos2->m_TransfPos.m_fZ;

			float fX3 = pDotPos3->m_TransfPos.m_fX;
			float fY3 = pDotPos3->m_TransfPos.m_fY;
			float fZ3 = pDotPos3->m_TransfPos.m_fZ;

			// Two lines:  First: (Dot2 - Dot1) Second: (Dot3 - Dot1)

			float fDeltaX1 = fX2 - fX1;
			float fDeltaY1 = fY2 - fY1;
			float fDeltaZ1 = fZ2 - fZ1;

			float fDeltaX2 = fX3 - fX1;
			float fDeltaY2 = fY3 - fY1;
			float fDeltaZ2 = fZ3 - fZ1;

			float fT1 = (fZ - fZ1)/(fDeltaZ1);
			float fT2 = (fZ - fZ1)/(fDeltaZ2);

			// Transformed Positions
			float fXNew1 = fDeltaX1*fT1 + fX1;
			float fYNew1 = fDeltaY1*fT1 + fY1;
			float fXNew2 = fDeltaX2*fT2 + fX1;
			float fYNew2 = fDeltaY2*fT2 + fY1;
			float fZNewRZP = 1/fZ;

			// TextureMapping
			float fTU1 = pDot1->m_fTextureU;
			float fTV1 = pDot1->m_fTextureV;
			float fTU2 = pDot2->m_fTextureU;
			float fTV2 = pDot2->m_fTextureV;
			float fTU3 = pDot3->m_fTextureU;
			float fTV3 = pDot3->m_fTextureV;

			float fDeltaTU1 = fTU2 - fTU1;
			float fDeltaTV1 = fTV2 - fTV1;
			float fDeltaTU2 = fTU3 - fTU1;
			float fDeltaTV2 = fTV3 - fTV1;

			float fTUNew1 = fDeltaTU1*fT1 + fTU1;
			float fTVNew1 = fDeltaTV1*fT1 + fTV1;

			float fTUNew2 = fDeltaTU2*fT2 + fTU1;
			float fTVNew2 = fDeltaTV2*fT2 + fTV1;

			// RGB

			float fR1 = pDotPos1->m_fR;
			float fG1 = pDotPos1->m_fG;
			float fB1 = pDotPos1->m_fB;
			float fR2 = pDotPos2->m_fR;
			float fG2 = pDotPos2->m_fG;
			float fB2 = pDotPos2->m_fB;
			float fR3 = pDotPos3->m_fR;
			float fG3 = pDotPos3->m_fG;
			float fB3 = pDotPos3->m_fB;

			float fDeltaR1 = fR2 - fR1;
			float fDeltaG1 = fG2 - fG1;
			float fDeltaB1 = fB2 - fB1;
			float fDeltaR2 = fR3 - fR1;
			float fDeltaG2 = fG3 - fG1;
			float fDeltaB2 = fB3 - fB1;

			float fRNew1 = fDeltaR1*fT1 + fR1;
			float fGNew1 = fDeltaG1*fT1 + fG1;
			float fBNew1 = fDeltaB1*fT1 + fB1;
			float fRNew2 = fDeltaR2*fT2 + fR1;
			float fGNew2 = fDeltaG2*fT2 + fG1;
			float fBNew2 = fDeltaB2*fT2 + fB1;

			// Get the new (clipped) dots
			CMV2Dot3DPos *pNewDotPos1 = &m_pCDot3DPos[m_iCurNumCDot3DPos];
			CMV2Dot3DPos *pNewDotPos2 = &m_pCDot3DPos[m_iCurNumCDot3DPos + 1];
			CMV2Dot3D *pNewDot1 = &m_pCDot3D[m_iCurNumCDot3D];
			CMV2Dot3D *pNewDot2 = &m_pCDot3D[m_iCurNumCDot3D + 1];

			m_iCurNumCDot3DPos += 2;
			m_iCurNumCDot3D += 2;

			// Set the Dot3DPos structures
			pNewDotPos1->m_TransfPos.m_fX = fXNew1;
			pNewDotPos1->m_TransfPos.m_fY = fYNew1;
			pNewDotPos1->m_TransfPos.m_fZ = fZ;
			pNewDotPos1->m_fZNewRZP = fZNewRZP;
			pNewDotPos1->m_fScreenX = (fXNew1*fZNewRZP + 1.0)*m_fXmax*0.5;
			pNewDotPos1->m_fScreenY = (fYNew1*fZNewRZP + 1.0)*m_fYmax*0.5;
			pNewDotPos1->m_iScreenX = int(pNewDotPos1->m_fScreenX*fScreenXFactor);
			pNewDotPos1->m_iScreenY = int(pNewDotPos1->m_fScreenY) + 1;
			pNewDotPos1->m_fScreenYError = RoundUP(pNewDotPos1->m_fScreenY)
				- pNewDotPos1->m_fScreenY;
			pNewDotPos1->m_fR = fRNew1;
			pNewDotPos1->m_fG = fGNew1;
			pNewDotPos1->m_fB = fBNew1;


			pNewDotPos2->m_TransfPos.m_fX = fXNew2;
			pNewDotPos2->m_TransfPos.m_fY = fYNew2;
			pNewDotPos2->m_TransfPos.m_fZ = fZ;
			pNewDotPos2->m_fZNewRZP = fZNewRZP;
			pNewDotPos2->m_fScreenX = (fXNew2*fZNewRZP + 1.0)*m_fXmax*0.5;
			pNewDotPos2->m_fScreenY = (fYNew2*fZNewRZP + 1.0)*m_fYmax*0.5;
			pNewDotPos2->m_iScreenX = int(pNewDotPos2->m_fScreenX*fScreenXFactor);
			pNewDotPos2->m_iScreenY = int(pNewDotPos2->m_fScreenY) + 1;
			pNewDotPos2->m_fScreenYError = RoundUP(pNewDotPos2->m_fScreenY)
				- pNewDotPos2->m_fScreenY;
			pNewDotPos2->m_fR = fRNew2;
			pNewDotPos2->m_fG = fGNew2;
			pNewDotPos2->m_fB = fBNew2;


			// Set the Dot3D structures
			pNewDot1->m_fTextureU = fTUNew1;
			pNewDot1->m_fTextureV = fTVNew1;
			pNewDot1->m_pPos = pNewDotPos1;

			pNewDot2->m_fTextureU = fTUNew2;
			pNewDot2->m_fTextureV = fTVNew2;
			pNewDot2->m_pPos = pNewDotPos2;


			if (iNumOuterDots == 1)
			{
				CMV2Polygon *pNewPolygon1 = &m_pCPolygon[m_iCurNumCPolygon];
				CMV2Polygon *pNewPolygon2 = &m_pCPolygon[m_iCurNumCPolygon + 1];
				m_iCurNumCPolygon += 2;

				pNewPolygon1->m_iType = iPolygonType;
				pNewPolygon1->m_iFlag = iPolygonFlag;
				pNewPolygon1->m_pDot1 = pNewDot1;
				pNewPolygon1->m_pDot2 = pDot2;
				pNewPolygon1->m_pDot3 = pDot3;
				pNewPolygon1->m_pcTexture = pPolygon->m_pcTexture;
				pNewPolygon1->m_pcBumpmap = pPolygon->m_pcBumpmap;
				pNewPolygon1->m_fAddDistance = pPolygon->m_fAddDistance;

				pNewPolygon2->m_iType = iPolygonType;
				pNewPolygon2->m_iFlag = iPolygonFlag;
				pNewPolygon2->m_pDot1 = pNewDot1;
				pNewPolygon2->m_pDot2 = pDot3;
				pNewPolygon2->m_pDot3 = pNewDot2;
				pNewPolygon2->m_pcTexture = pPolygon->m_pcTexture;
				pNewPolygon2->m_pcBumpmap = pPolygon->m_pcBumpmap;
				pNewPolygon2->m_fAddDistance = pPolygon->m_fAddDistance;

				m_ppDestCPolygonList[m_iCurNumDestCPolygonList] = pNewPolygon1;
				m_ppDestCPolygonList[m_iCurNumDestCPolygonList + 1] = pNewPolygon2;

				m_iCurNumDestCPolygonList += 2;
			}
			else
			{
				// iNumOuterDots = 2;

				CMV2Polygon *pNewPolygon1 = &m_pCPolygon[m_iCurNumCPolygon];
				m_iCurNumCPolygon++;

				pNewPolygon1->m_iType = iPolygonType;
				pNewPolygon1->m_iFlag = iPolygonFlag;
				pNewPolygon1->m_pDot1 = pDot1;
				pNewPolygon1->m_pDot2 = pNewDot1;
				pNewPolygon1->m_pDot3 = pNewDot2;
				pNewPolygon1->m_pcTexture = pPolygon->m_pcTexture;
				pNewPolygon1->m_pcBumpmap = pPolygon->m_pcBumpmap;
				pNewPolygon1->m_fAddDistance = pPolygon->m_fAddDistance;

				m_ppDestCPolygonList[m_iCurNumDestCPolygonList] = pNewPolygon1;
				m_iCurNumDestCPolygonList++;
			}
		}
		else
		{
				m_ppDestCPolygonList[m_iCurNumDestCPolygonList] = pPolygon;
				m_iCurNumDestCPolygonList++;
		}
	}
}










