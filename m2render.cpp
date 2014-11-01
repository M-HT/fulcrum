// Filename: m2render.cpp
//
// Developed by Oliver Belaifa in 1997
//

#include<stdlib.h>
#include<stdio.h>
//#include<copro.h>
#include <math.h>
#include<m2render.h>
#include<demo.h>

extern "C"
{
	void MV2Dot3DPosProjectionASM(CMV2Camera *pMV2Camera,
		CMV2Dot3DPos **ppMV2Dot3DPos, int iNumDots);
	void MESEtoMV2Dot3DPosProjectionASM(CMV2Camera *pMV2Camera,
		CMV2Dot3DPos **ppMV2Dot3DPos, int iNumDots);
	int MV2AddPolygonsASM(CMV2PolygonDistance *pPolygonDistance,
		CMV2Polygon **ppPolygons, int iNumAddPolygons);

	void MV2SortRenderBufferASM(CMV2PolygonDistance *pPolygonDistanceBuffer1,
		CMV2PolygonDistance *pPolygonDistanceBuffer2,
		int iNumPolygons);

	void MV2DrawPolygonsASM(CMV2PolygonDistance *pPolygonDistance,
		int iNumPolygons, char *pcBackBuffer, int iXmax);

	void CMV2CopyPolygonListASM(CMV2Polygon **ppDest, CMV2Polygon **ppSrc,
		int iNumPolygons);

	void InitPolygonCallTableASM();
	void InitFltJumpTableTASM();
	void InitFltJumpTableTTrASM();
}



extern void CalcLookUpTables();



// profiling {
#include<timecntr.h>
#include<m2world.h>
#include<m2raw.h>
extern CMV2World *pWorld;

int iDrawPolysCounter;
float fDrawPolysFps;
int iClippingCounter;
float fClippingFps;
// }



//#include<mem.h>




void CMV2Render::FromMESEAddPolygonObject(CMV2Camera *pCamera,
	CMV2PolygonObject *pPolygonObject)
{
	CMV2Polygon **ppPolygons = pPolygonObject->m_ppPolygons;
	CMV2Dot3DPos **ppDot3DPos = pPolygonObject->m_ppDot3DPos;
	int iNumDot3DPos = pPolygonObject->m_iNumDot3DPos;
	int iNumPolygons = pPolygonObject->m_iNumPolygons;

	if (iNumDot3DPos)
	{
		MESEtoMV2Dot3DPosProjectionASM(pCamera, ppDot3DPos, iNumDot3DPos);
	}


//	memcpy(&m_ppSrcPolygonList[m_iCurNumSrcPolygonList], ppPolygons, iNumPolygons*4);

	CMV2CopyPolygonListASM(&m_ppSrcPolygonList[m_iCurNumSrcPolygonList], ppPolygons, iNumPolygons);
	m_iCurNumSrcPolygonList += iNumPolygons;
/*
	int iI;
	for (iI = 0; iI < iNumPolygons; iI++)
	{
		m_ppSrcPolygonList[m_iCurNumSrcPolygonList] = ppPolygons[iI];
		m_iCurNumSrcPolygonList++;
	}
*/
/*
	CMV2Polygon *pPolygons = pPolygonObject->m_pPolygons;
	CMV2Dot3DPos *pDot3DPos = pPolygonObject->m_pDot3DPos;

	CalcNorm(pDot3DPos, iNumDot3DPos, pPolygons, iNumPolygons);
	CalcRGB(pDot3DPos, iNumDot3DPos);
*/
}




void CMV2Render::AddPolygonObject(CMV2Camera *pCamera,
	CMV2PolygonObject *pPolygonObject)
{
	CMV2Polygon **ppPolygons = pPolygonObject->m_ppPolygons;
	CMV2Dot3DPos **ppDot3DPos = pPolygonObject->m_ppDot3DPos;
	int iNumDot3DPos = pPolygonObject->m_iNumDot3DPos;
	int iNumPolygons = pPolygonObject->m_iNumPolygons;

	if (iNumDot3DPos)
		MV2Dot3DPosProjectionASM(pCamera, ppDot3DPos, iNumDot3DPos);

	CMV2CopyPolygonListASM(&m_ppSrcPolygonList[m_iCurNumSrcPolygonList], ppPolygons, iNumPolygons);
	m_iCurNumSrcPolygonList += iNumPolygons;


	CMV2Polygon *pPolygons = pPolygonObject->m_pPolygons;
	CMV2Dot3DPos *pDot3DPos = pPolygonObject->m_pDot3DPos;

	CalcNorm(pDot3DPos, iNumDot3DPos, pPolygons, iNumPolygons);
//	CalcRGB(pDot3DPos, iNumDot3DPos);
	CalcPhongAngles(pDot3DPos, iNumDot3DPos);
}


// TODO: ASM
void CMV2Render::CalcNorm(CMV2Dot3DPos *pDots, int iNumDots,
	CMV2Polygon *pPolygons, int iNumPolygons)
{
	int iI;

	for (iI = 0; iI < iNumDots; iI++)
	{
		pDots[iI].m_Norm.m_fX = 0;
		pDots[iI].m_Norm.m_fY = 0;
		pDots[iI].m_Norm.m_fZ = 0;
	}

	for (iI = 0; iI < iNumPolygons; iI++)
	{
		CMV2Dot3DPos *pDot1 = pPolygons[iI].m_pDot1->m_pPos;
		CMV2Dot3DPos *pDot2 = pPolygons[iI].m_pDot2->m_pPos;
		CMV2Dot3DPos *pDot3 = pPolygons[iI].m_pDot3->m_pPos;
/*
		float fAx = pDot3->m_TransfPos.m_fX - pDot1->m_TransfPos.m_fX;
		float fAy = pDot3->m_TransfPos.m_fY - pDot1->m_TransfPos.m_fY;
		float fAz = pDot3->m_TransfPos.m_fZ - pDot1->m_TransfPos.m_fZ;
		float fBx = pDot2->m_TransfPos.m_fX - pDot1->m_TransfPos.m_fX;
		float fBy = pDot2->m_TransfPos.m_fY - pDot1->m_TransfPos.m_fY;
		float fBz = pDot2->m_TransfPos.m_fZ - pDot1->m_TransfPos.m_fZ;
*/

		float fAx = pDot3->m_Pos.m_fX - pDot1->m_Pos.m_fX;
		float fAy = pDot3->m_Pos.m_fY - pDot1->m_Pos.m_fY;
		float fAz = pDot3->m_Pos.m_fZ - pDot1->m_Pos.m_fZ;
		float fBx = pDot2->m_Pos.m_fX - pDot1->m_Pos.m_fX;
		float fBy = pDot2->m_Pos.m_fY - pDot1->m_Pos.m_fY;
		float fBz = pDot2->m_Pos.m_fZ - pDot1->m_Pos.m_fZ;

		float fNx = fAy*fBz - fBy*fAz;
		float fNy = fAz*fBx - fBz*fAx;
		float fNz = fAx*fBy - fBx*fAy;

		pDot1->m_Norm.m_fX += fNx;
		pDot1->m_Norm.m_fY += fNy;
		pDot1->m_Norm.m_fZ += fNz;

		pDot2->m_Norm.m_fX += fNx;
		pDot2->m_Norm.m_fY += fNy;
		pDot2->m_Norm.m_fZ += fNz;

		pDot3->m_Norm.m_fX += fNx;
		pDot3->m_Norm.m_fY += fNy;
		pDot3->m_Norm.m_fZ += fNz;
	}

	for (iI = 0; iI < iNumDots; iI++)
	{
		float fNx = pDots[iI].m_Norm.m_fX;
		float fNy = pDots[iI].m_Norm.m_fY;
		float fNz = pDots[iI].m_Norm.m_fZ;

		float fAbs = sqrtf(fNx*fNx + fNy*fNy + fNz*fNz);

		if (fAbs > 0)
		{
			float fAbsRZP = 1/fAbs;

			fNx *= fAbsRZP;
			fNy *= fAbsRZP;
			fNz *= fAbsRZP;
		}

		pDots[iI].m_Norm.m_fX = fNx;
		pDots[iI].m_Norm.m_fY = fNy;
		pDots[iI].m_Norm.m_fZ = fNz;
	}
}



void CMV2Render::CalcPhongAngles(CMV2Dot3DPos *pDots, int iNumDots)
{
	if (m_iCurNumOmniLights)
	{
		CMV2OmniLight *pLight = m_ppOmniLights[0];
//		pPolygons->m_pDot1->m_pPos->m_Pos.m_fX += 0.01;


		int iI;
		for (iI = 0; iI < iNumDots; iI++)
		{
			CVector3D Vec = pLight->m_Pos - pDots->m_Pos;
			Vec.Norm();

			float fAngleN = pDots->m_Norm*Vec;
			float fAngleU = pDots->m_AngleU*Vec;
			float fAngleV = pDots->m_AngleV*Vec;
			pDots->m_fR = 128.0*256.0*fAngleN;
			pDots->m_fG = 128.0*256.0*(1.0 + fAngleU);
			pDots->m_fB = 128.0*256.0*(1.0 + fAngleV);

			pDots++;
		}

	}

/*
			float fCosAngleU1 = pPolygons->m_AngleU1*Vec1;
			float fCosAngleV1 = pPolygons->m_AngleV1*Vec1;
			float fCosNormU1 = pPolygons->m_AngleU1*pDot1->m_Norm;
			float fCosNormV1 = pPolygons->m_AngleV1*pDot1->m_Norm;
			fAngleU1 = cos(acos(fCosAngleU1) - acos(fCosNormU1) + 1.570796);
			fAngleV1 = cos(acos(fCosAngleV1) - acos(fCosNormV1) + 1.570796);

			float fCosAngleU2 = pPolygons->m_AngleU2*Vec2;
			float fCosAngleV2 = pPolygons->m_AngleV2*Vec2;
			float fCosNormU2 = pPolygons->m_AngleU2*pDot2->m_Norm;
			float fCosNormV2 = pPolygons->m_AngleV2*pDot2->m_Norm;
			fAngleU2 = cos(acos(fCosAngleU2) - acos(fCosNormU2) + 1.570796);
			fAngleV2 = cos(acos(fCosAngleV2) - acos(fCosNormV2) + 1.570796);

			float fCosAngleU3 = pPolygons->m_AngleU3*Vec3;
			float fCosAngleV3 = pPolygons->m_AngleV3*Vec3;
			float fCosNormU3 = pPolygons->m_AngleU3*pDot3->m_Norm;
			float fCosNormV3 = pPolygons->m_AngleV3*pDot3->m_Norm;
			fAngleU3 = cos(acos(fCosAngleU3) - acos(fCosNormU3) + 1.570796);
			fAngleV3 = cos(acos(fCosAngleV3) - acos(fCosNormV3) + 1.570796);

			pPolygons->m_fAngleU1 = 128.0*256.0*(1.0 + fAngleU1);
			pPolygons->m_fAngleV1 = 128.0*256.0*(1.0 + fAngleV1);
			pPolygons->m_fAngleU2 = 128.0*256.0*(1.0 + fAngleU2);
			pPolygons->m_fAngleV2 = 128.0*256.0*(1.0 + fAngleV2);
			pPolygons->m_fAngleU3 = 128.0*256.0*(1.0 + fAngleU3);
			pPolygons->m_fAngleV3 = 128.0*256.0*(1.0 + fAngleV3);
*/
}




void CMV2Render::CalcRGB(CMV2Dot3DPos *pDots, int iNumDots)
{
	int iI;

	for (iI = 0; iI < iNumDots; iI++)
	{
		CMV2Dot3DPos *pDot = &pDots[iI];

		pDot->m_fR = pDot->m_fAmbientR;
		pDot->m_fG = pDot->m_fAmbientG;
		pDot->m_fB = pDot->m_fAmbientB;
	}

	for (iI = 0; iI < m_iCurNumOmniLights; iI++)
	{
		CMV2OmniLight *pLight = m_ppOmniLights[iI];
		float fX = pLight->m_TransfPos.m_fX;
		float fY = pLight->m_TransfPos.m_fY;
		float fZ = pLight->m_TransfPos.m_fZ;
		float fStart = pLight->m_fStart;
		float fEnd = pLight->m_fEnd;
		float fDelta = fEnd - fStart;
		float fR = pLight->m_fR;
		float fG = pLight->m_fG;
		float fB = pLight->m_fB;

		int iJ;
		for (iJ = 0; iJ < iNumDots; iJ++)
		{
			float fAx = fX - pDots[iJ].m_TransfPos.m_fX;
			float fAy = fY - pDots[iJ].m_TransfPos.m_fY;
			float fAz = fZ - pDots[iJ].m_TransfPos.m_fZ;
			float fAbs = sqrtf(fAx*fAx + fAy*fAy + fAz*fAz);

			if (fAbs <= fEnd)
			{
				float fNx = pDots[iJ].m_Norm.m_fX;
				float fNy = pDots[iJ].m_Norm.m_fY;
				float fNz = pDots[iJ].m_Norm.m_fZ;

				float fFactor = (fEnd - fAbs)/fDelta;
				if (fFactor > 1.0)
					fFactor = 1.0;

				if (fFactor < 0)
					fFactor = 0;

				if (fAbs > 0)
				{
					float fAbsRZP = 1.0/fAbs;

					fAx *= fAbsRZP;
					fAy *= fAbsRZP;
					fAz *= fAbsRZP;

					fFactor *= fNx*fAx + fNy*fAy + fNz*fAz;

					if (fFactor > 0)
					{
						pDots[iJ].m_fR += fFactor*fR;
						pDots[iJ].m_fG += fFactor*fG;
						pDots[iJ].m_fB += fFactor*fB;
					}
				}
			}
		}
	}
}


/*

// TODO: ASM
void CMV2Render::FromMESEAddOmniLight(CMV2Camera *pCamera,
	CMV2OmniLight *pOmniLight)
{
	CMV2Dot3DPos Dot3DPos = CMV2Dot3DPos(pOmniLight->m_Pos.m_fX,
		pOmniLight->m_Pos.m_fY, pOmniLight->m_Pos.m_fZ);

	CMV2Dot3DPos **ppDot3DPos = new CMV2Dot3DPos *[0];
	ppDot3DPos[0] = &Dot3DPos;
	MV2Dot3DPosProjectionASM(pCamera, ppDot3DPos, 1);
	delete ppDot3DPos;

	pOmniLight->m_TransfPos.m_fX = Dot3DPos.m_TransfPos.m_fX;
	pOmniLight->m_TransfPos.m_fY = Dot3DPos.m_TransfPos.m_fY;
	pOmniLight->m_TransfPos.m_fZ = Dot3DPos.m_TransfPos.m_fZ;

	if (m_iCurNumOmniLights < m_iMaxNumOmniLights)
	{
		m_ppOmniLights[m_iCurNumOmniLights] = pOmniLight;
		m_iCurNumOmniLights++;
	}
}




void CMV2Render::AddOmniLight(CMV2Camera *pCamera,
	CMV2OmniLight *pOmniLight)
{
	CMV2Dot3DPos Dot3DPos = CMV2Dot3DPos(pOmniLight->m_Pos.m_fX,
		pOmniLight->m_Pos.m_fY, pOmniLight->m_Pos.m_fZ);

	CMV2Dot3DPos **ppDot3DPos = new CMV2Dot3DPos *[0];
	ppDot3DPos[0] = &Dot3DPos;
	MV2Dot3DPosProjectionASM(pCamera, ppDot3DPos, 1);
	delete ppDot3DPos;

	pOmniLight->m_TransfPos.m_fX = Dot3DPos.m_TransfPos.m_fX;
	pOmniLight->m_TransfPos.m_fY = Dot3DPos.m_TransfPos.m_fY;
	pOmniLight->m_TransfPos.m_fZ = Dot3DPos.m_TransfPos.m_fZ;

	if (m_iCurNumOmniLights < m_iMaxNumOmniLights)
	{
		m_ppOmniLights[m_iCurNumOmniLights] = pOmniLight;
		m_iCurNumOmniLights++;
	}
}

*/


void CMV2Render::SetClipRanges(int iCXmin, int iCYmin,
	int iCXmax, int iCYmax, int iXmax, int iYmax)
{
	m_fCXmin = (float)iCXmin;
	m_fCYmin = (float)iCYmin;
	m_fCXmax = (float)iCXmax;
	m_fCYmax = (float)iCYmax;

	m_fCXmin += 0.5;
	m_fCYmin += 0.5;
	m_fCXmax += 0.5;
	m_fCYmax += 0.5;

	m_fCXmin *= 1024.0*1024.0;
	m_fCXmax *= 1024.0*1024.0;

	m_fXmax = (float)iXmax;
	m_fYmax = (float)iYmax;

	m_fXmax *= 1024.0*1024.0;
}





void CMV2Render::Clear()
{
	m_iCurNumSrcPolygonList = 0;
	m_iCurNumDestPolygonList = 0;
	m_iCurNumSrcCPolygonList = 0;
	m_iCurNumDestCPolygonList = 0;
	m_iCurNumCDot3DPos = 0;
	m_iCurNumCDot3D = 0;
	m_iCurNumCPolygon = 0;
	m_iCurNumOmniLights = 0;
}




void CMV2Render::Init()
{
	m_iMaxNumPolygonList = 16000;
	m_iMaxNumCPolygonList = 8000;
	m_iMaxNumCDot3DPos = 4000;
	m_iMaxNumCDot3D = 4000;
	m_iMaxNumCPolygon = 8000;
	m_iMaxNumOmniLights = 20;

	m_ppPolygonList1 = (CMV2Polygon **) getmem(m_iMaxNumPolygonList*sizeof(CMV2Polygon *));//new CMV2Polygon *[m_iMaxNumPolygonList];
	m_ppPolygonList2 = (CMV2Polygon **) getmem(m_iMaxNumPolygonList*sizeof(CMV2Polygon *));//new CMV2Polygon *[m_iMaxNumPolygonList];
	m_ppSrcPolygonList = m_ppPolygonList1;
	m_ppDestPolygonList = m_ppPolygonList2;

	m_ppCPolygonList1 = (CMV2Polygon **) getmem(m_iMaxNumCPolygonList*sizeof(CMV2Polygon *));//new CMV2Polygon *[m_iMaxNumCPolygonList];
	m_ppCPolygonList2 = (CMV2Polygon **) getmem(m_iMaxNumCPolygonList*sizeof(CMV2Polygon *));//new CMV2Polygon *[m_iMaxNumCPolygonList];
	m_ppSrcCPolygonList = m_ppCPolygonList1;
	m_ppDestCPolygonList = m_ppCPolygonList2;

	m_pCDot3DPos = (CMV2Dot3DPos *) getmem(m_iMaxNumCDot3DPos*sizeof(CMV2Dot3DPos));//new CMV2Dot3DPos[m_iMaxNumCDot3DPos];
	m_pCDot3D = (CMV2Dot3D *) getmem(m_iMaxNumCDot3D*sizeof(CMV2Dot3D));//new CMV2Dot3D[m_iMaxNumCDot3D];
	m_pCPolygon = (CMV2Polygon *) getmem(m_iMaxNumCPolygon*sizeof(CMV2Polygon));//new CMV2Polygon[m_iMaxNumCPolygon];

	m_pRenderBuffer = new CMV2RenderBuffer(m_iMaxNumPolygonList);

	m_ppOmniLights = (CMV2OmniLight **) getmem(m_iMaxNumOmniLights*sizeof(CMV2OmniLight *));//new CMV2OmniLight *[m_iMaxNumOmniLights];

	InitPolygonCallTableASM();
	InitFltJumpTableTASM();
	InitFltJumpTableTTrASM();
	CalcLookUpTables();
}





void CMV2Render::DoRendering(char *pcBackBuffer, int iXmax)
{
	CTimeCounter TimeCounter = *pWorld->m_pTimeCounter; //*****

	TimeCounter.StartTimeCounter(); 		   //*****
	Clip2D();
	TimeCounter.StopTimeCounter();  		   //*****
	fClippingFps = TimeCounter.GetFPS(); 	   //*****
	iClippingCounter = TimeCounter.GetTSC(); //*****

	m_pRenderBuffer->ClearBuffer();
	m_pRenderBuffer->AddPolygons(m_ppSrcPolygonList,
		m_iCurNumSrcPolygonList);

	m_pRenderBuffer->SortThem();

	TimeCounter.StartTimeCounter(); 		   //*****
	if (m_pRenderBuffer->m_iCurNumPolygonDistances)
	{
		MV2DrawPolygonsASM(
			m_pRenderBuffer->m_pPolygonDistanceBuffer1,
			m_pRenderBuffer->m_iCurNumPolygonDistances,
			pcBackBuffer, iXmax);
	}

	TimeCounter.StopTimeCounter();  		   //*****
	fDrawPolysFps = TimeCounter.GetFPS(); 	   //*****
	iDrawPolysCounter = TimeCounter.GetTSC(); //*****
}












void CMV2Render::Free()
{
/*
	if (m_ppPolygonList1) delete []m_ppPolygonList1, m_ppPolygonList1 = NULL;
	if (m_ppPolygonList2) delete []m_ppPolygonList2, m_ppPolygonList2 = NULL;
	m_ppSrcPolygonList = NULL;
	m_ppDestPolygonList = NULL;

	if (m_ppCPolygonList1) delete []m_ppCPolygonList1, m_ppCPolygonList1 = NULL;
	if (m_ppCPolygonList2) delete []m_ppCPolygonList2, m_ppCPolygonList2 = NULL;
	m_ppSrcCPolygonList = NULL;
	m_ppDestCPolygonList = NULL;

	if (m_pCDot3DPos) delete []m_pCDot3DPos, m_pCDot3DPos = NULL;
	if (m_pCDot3D) delete []m_pCDot3D, m_pCDot3D = NULL;
	if (m_pCPolygon) delete []m_pCPolygon, m_pCPolygon = NULL;

	if (m_pRenderBuffer) delete []m_pRenderBuffer, m_pRenderBuffer = NULL;
	if (m_ppOmniLights) delete []m_ppOmniLights; m_ppOmniLights = NULL;
*/
}


CMV2Render::CMV2Render()
{
	m_ppPolygonList1 = NULL;
	m_ppPolygonList2 = NULL;
	m_ppSrcPolygonList = NULL;
	m_ppDestPolygonList = NULL;
	m_iMaxNumPolygonList = 0;
	m_iCurNumSrcPolygonList = 0;
	m_iCurNumDestPolygonList = 0;

	m_ppCPolygonList1 = NULL;
	m_ppCPolygonList2 = NULL;
	m_ppSrcCPolygonList = NULL;
	m_ppDestCPolygonList = NULL;
	m_iMaxNumCPolygonList = 0;
	m_iCurNumSrcCPolygonList = 0;
	m_iCurNumDestCPolygonList = 0;

	m_pCDot3DPos = NULL;
	m_iMaxNumCDot3DPos = 0;
	m_iCurNumCDot3DPos = 0;

	m_pCDot3D = NULL;
	m_iMaxNumCDot3D = 0;
	m_iCurNumCDot3D = 0;

	m_pCPolygon = NULL;
	m_iMaxNumCPolygon = 0;
	m_iCurNumCPolygon = 0;

	m_pRenderBuffer = NULL;

	m_ppOmniLights = NULL;
	m_iMaxNumOmniLights = 0;
	m_iCurNumOmniLights = 0;
}





CMV2Render::~CMV2Render()
{
}





CMV2PolygonDistance::CMV2PolygonDistance()
{
}



CMV2RenderBuffer::CMV2RenderBuffer()
{
	m_pPolygonDistanceBuffer1 = NULL;
	m_pPolygonDistanceBuffer2 = NULL;

	m_iMaxPolygonDistances = 0;
	m_iCurNumPolygonDistances = 0;
}



CMV2RenderBuffer::CMV2RenderBuffer(int iMaxPolygonDistances)
{
	m_pPolygonDistanceBuffer1 =
	  (CMV2PolygonDistance *) getmem(iMaxPolygonDistances*sizeof(CMV2PolygonDistance));//new CMV2PolygonDistance[iMaxPolygonDistances];

	m_pPolygonDistanceBuffer2 =
	  (CMV2PolygonDistance *) getmem(iMaxPolygonDistances*sizeof(CMV2PolygonDistance));//new CMV2PolygonDistance[iMaxPolygonDistances];


	m_iMaxPolygonDistances = iMaxPolygonDistances;
	m_iCurNumPolygonDistances = 0;
}


//(*) getmem(*sizeof());//

//new/delete

/*void *CMV2PolygonDistance::operator new(size_t s) {
  return getmem(s);
}

void CMV2PolygonDistance::operator delete(void *p) {

}
*/
void *CMV2RenderBuffer::operator new(size_t s) {
  return getmem(s);
}

void CMV2RenderBuffer::operator delete(void *p) {

}

void *CMV2Render::operator new(size_t s) {
  return getmem(s);
}

void CMV2Render::operator delete(void *p) {

}


// Destructors
CMV2PolygonDistance::~CMV2PolygonDistance()
{
}



CMV2RenderBuffer::~CMV2RenderBuffer()
{
/*	if (m_pPolygonDistanceBuffer1)
		delete m_pPolygonDistanceBuffer1;

	if (m_pPolygonDistanceBuffer2)
		delete m_pPolygonDistanceBuffer2;
*/
}





void CMV2RenderBuffer::AddPolygons(CMV2Polygon **ppPolygons,
	int iNumPolygons)
{
	if (m_iCurNumPolygonDistances + iNumPolygons <=
		m_iMaxPolygonDistances)
	{
		m_iCurNumPolygonDistances +=
			MV2AddPolygonsASM(
			&(m_pPolygonDistanceBuffer1[m_iCurNumPolygonDistances]),
			ppPolygons, iNumPolygons);
	}
}





void CMV2RenderBuffer::AddPolygons(CMV2PolygonObject *pPolygonObject)
{
	int iNumAddPolygons = pPolygonObject->m_iNumPolygons;

	if (m_iCurNumPolygonDistances + iNumAddPolygons <=
		m_iMaxPolygonDistances)
	{
		m_iCurNumPolygonDistances +=
			MV2AddPolygonsASM(
			&(m_pPolygonDistanceBuffer1[m_iCurNumPolygonDistances]),
			pPolygonObject->m_ppPolygons, iNumAddPolygons);
	}
}



void CMV2RenderBuffer::ClearBuffer()
{
	m_iCurNumPolygonDistances = 0;
}


void CMV2RenderBuffer::SortThem()
{
	if (m_iCurNumPolygonDistances)
		MV2SortRenderBufferASM(m_pPolygonDistanceBuffer1,
			m_pPolygonDistanceBuffer2, m_iCurNumPolygonDistances);
}





