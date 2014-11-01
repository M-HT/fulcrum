// Filename: m2dots.cpp
//
// Developed by Oliver Belaifa in 1997
//

#include<stdlib.h>
#include<stdio.h>
//#include<copro.h>
#include <math.h>
#include<m2except.h>
#include<m2raw.h>
#include<demo.h>



// Constructors
CMV2Dot3DPos::CMV2Dot3DPos()
{
	m_Pos = CVector3D(0, 0, 0);
	m_iScreenX = 0;
	m_iScreenY = 0;
	m_fScreenX = 0;
	m_fScreenY = 0;
	m_iClipFlag = 0;
	m_fR = 0;
	m_fG = 0;
	m_fB = 0;
	m_fAmbientR = 0;
	m_fAmbientG = 0;
	m_fAmbientB = 0;
	m_Norm = CVector3D(0, 0, 0);
}



CMV2Dot3DPos::CMV2Dot3DPos(float fX, float fY, float fZ)
{
	m_Pos = CVector3D(fX, fY, fZ);
	m_iScreenX = 0;
	m_iScreenY = 0;
	m_fScreenX = 0;
	m_fScreenY = 0;
	m_iClipFlag = 0;
	m_fR = 0;
	m_fG = 0;
	m_fB = 0;
	m_fAmbientR = 0;
	m_fAmbientG = 0;
	m_fAmbientB = 0;
	m_Norm = CVector3D(0, 0, 0);
}



CMV2Dot3D::CMV2Dot3D()
{
	m_fTextureU = 0;
	m_fTextureV = 0;
	m_pPos = NULL;
}




CMV2Polygon::CMV2Polygon()
{
	m_pDot1 = NULL;
	m_pDot2 = NULL;
	m_pDot3 = NULL;
	m_pcTexture = NULL;
	m_fAddDistance = 0;
}




CMV2OmniLight::CMV2OmniLight()
{
	m_Pos = CVector3D(0, 0, 0);
	m_TransfPos = CVector3D(0, 0, 0);
	m_fR = 0;
	m_fG = 0;
	m_fB = 0;
	m_fStart = 0;
	m_fEnd = 0;
}



//new/delete

/*
void *CMV2Dot3DPos::operator new(size_t s) {
  return getmem(s);
}

void CMV2Dot3DPos::operator delete(void *p) {

}

void *CMV2Dot3D::operator new(size_t s) {
  return getmem(s);
}

void CMV2Dot3D::operator delete(void *p) {

}

void *CMV2Polygon::operator new(size_t s) {
  return getmem(s);
}

void CMV2Polygon::operator delete(void *p) {

}
*/
void *CMV2OmniLight::operator new(size_t s) {
  return getmem(s);
}

void CMV2OmniLight::operator delete(void *p) {

}

void *CMV2PolygonObject::operator new(size_t s) {
  return getmem(s);
}

void CMV2PolygonObject::operator delete(void *p) {

}





CMV2OmniLight::~CMV2OmniLight()
{
}



void CMV2PolygonObject::GetLWOB(CLWObject *pLWObject)
{
	try
	{
		int iI;

		m_iNumDot3DPos = pLWObject->m_lNumLWDots;
		m_pDot3DPos = new CMV2Dot3DPos[m_iNumDot3DPos];

		if (m_pDot3DPos == NULL)
			throw new CMV2NormException("memory allocation error",
				__FILE__, __LINE__);

		m_ppDot3DPos = new CMV2Dot3DPos *[m_iNumDot3DPos];
		if (m_ppDot3DPos == NULL)
			throw new CMV2NormException("memory allocation error",
				__FILE__, __LINE__);

		for (iI = 0; iI < m_iNumDot3DPos; iI++)
		{
			m_pDot3DPos[iI] = CMV2Dot3DPos(
				pLWObject->m_pLWDots[iI].m_Pos.m_fX,
				pLWObject->m_pLWDots[iI].m_Pos.m_fY,
				pLWObject->m_pLWDots[iI].m_Pos.m_fZ);

			m_ppDot3DPos[iI] = &(m_pDot3DPos[iI]);
		}

		m_iNumDot3D = m_iNumDot3DPos;
		m_pDot3D = new CMV2Dot3D[m_iNumDot3D];
		if (m_pDot3D == NULL)
			throw new CMV2NormException("memory allocation error",
				__FILE__, __LINE__);

		for (iI = 0; iI < m_iNumDot3D; iI++)
		{
			m_pDot3D[iI].m_pPos = &(m_pDot3DPos[iI]);
			pLWObject->MapPlanar(&m_pDot3D[iI]);
		}

		m_iNumPolygons = pLWObject->m_lNumLWPolygons;
		m_pPolygons = new CMV2Polygon[m_iNumPolygons];
		if (m_pPolygons == NULL)
			throw new CMV2NormException("memory allocation error",
				__FILE__, __LINE__);

		m_ppPolygons = new CMV2Polygon *[m_iNumPolygons];
		if (m_ppPolygons == NULL)
			throw new CMV2NormException("memory allocation error",
				__FILE__, __LINE__);

		for (iI = 0; iI < m_iNumPolygons; iI++)
		{
			m_pPolygons[iI].m_pDot1 =
				&(m_pDot3D[pLWObject->m_pLWPolygons[iI].m_ppLWDots[0]->m_lID]);
			m_pPolygons[iI].m_pDot2 =
				&(m_pDot3D[pLWObject->m_pLWPolygons[iI].m_ppLWDots[2]->m_lID]);
			m_pPolygons[iI].m_pDot3 =
				&(m_pDot3D[pLWObject->m_pLWPolygons[iI].m_ppLWDots[1]->m_lID]);

			m_ppPolygons[iI] = &(m_pPolygons[iI]);
		}

	}
	catch (CMV2NormException *pException)
	{
		pException->PrintError();
		delete pException;
	}
}



void CMV2PolygonObject::UpdateNorms()
{
	int iI;
	for (iI = 0; iI < m_iNumDot3DPos; iI++)
	{
		m_pDot3DPos[iI].m_Norm.m_fX = 0;
		m_pDot3DPos[iI].m_Norm.m_fY = 0;
		m_pDot3DPos[iI].m_Norm.m_fZ = 0;
	}

	for (iI = 0; iI < m_iNumPolygons; iI++)
	{
		CMV2Dot3DPos *pDot1 = m_pPolygons[iI].m_pDot1->m_pPos;
		CMV2Dot3DPos *pDot2 = m_pPolygons[iI].m_pDot2->m_pPos;
		CMV2Dot3DPos *pDot3 = m_pPolygons[iI].m_pDot3->m_pPos;

		float fAx = pDot3->m_Pos.m_fX - pDot1->m_Pos.m_fX;
		float fAy = pDot3->m_Pos.m_fY - pDot1->m_Pos.m_fY;
		float fAz = pDot3->m_Pos.m_fZ - pDot1->m_Pos.m_fZ;

		float fBx = pDot2->m_Pos.m_fX - pDot1->m_Pos.m_fX;
		float fBy = pDot2->m_Pos.m_fY - pDot1->m_Pos.m_fY;
		float fBz = pDot2->m_Pos.m_fZ - pDot1->m_Pos.m_fZ;

		float fNx = fAy*fBz - fBy*fAz;
		float fNy = fAz*fBx - fBz*fAx;
		float fNz = fAx*fBy - fBx*fAy;

		CVector3D Norm = CVector3D(fNx, fNy, fNz);
		Norm.Norm();

		pDot1->m_Norm += Norm;
		pDot2->m_Norm += Norm;
		pDot3->m_Norm += Norm;
	}

	for (iI = 0; iI < m_iNumDot3DPos; iI++)
	{
		float fNx = m_pDot3DPos[iI].m_Norm.m_fX;
		float fNy = m_pDot3DPos[iI].m_Norm.m_fY;
		float fNz = m_pDot3DPos[iI].m_Norm.m_fZ;

		float fAbs = sqrtf(fNx*fNx + fNy*fNy + fNz*fNz);

		if (fAbs > 0)
		{
			float fAbsRZP = 1/fAbs;

			fNx *= fAbsRZP;
			fNy *= fAbsRZP;
			fNz *= fAbsRZP;
		}

		m_pDot3DPos[iI].m_Norm.m_fX = fNx;
		m_pDot3DPos[iI].m_Norm.m_fY = fNy;
		m_pDot3DPos[iI].m_Norm.m_fZ = fNz;
	}
}






void CMV2PolygonObject::UpdateBumpParameters()
{
	UpdateNorms();

	int iI;
	for (iI = 0; iI < m_iNumDot3DPos; iI++)
	{
		m_pDot3DPos[iI].m_AngleU.m_fX = 0;
		m_pDot3DPos[iI].m_AngleU.m_fY = 0;
		m_pDot3DPos[iI].m_AngleU.m_fZ = 0;
		m_pDot3DPos[iI].m_AngleV.m_fX = 0;
		m_pDot3DPos[iI].m_AngleV.m_fY = 0;
		m_pDot3DPos[iI].m_AngleV.m_fZ = 0;
	}

	CMV2Polygon *pPolygons = m_pPolygons;
	int iNumPolygons = m_iNumPolygons;
	for (iI = 0; iI < iNumPolygons; iI++)
	{
		CMV2Dot3D *pDot1 = pPolygons->m_pDot1;
		CMV2Dot3D *pDot2 = pPolygons->m_pDot2;
		CMV2Dot3D *pDot3 = pPolygons->m_pDot3;

		CVector3D AngleU =
			(pDot2->m_fTextureU - pDot1->m_fTextureU)*
			(pDot2->m_pPos->m_Pos - pDot1->m_pPos->m_Pos) +
			(pDot3->m_fTextureU - pDot1->m_fTextureU)*
			(pDot3->m_pPos->m_Pos - pDot1->m_pPos->m_Pos);
		CVector3D AngleV =
			(pDot2->m_fTextureV - pDot1->m_fTextureV)*
			(pDot2->m_pPos->m_Pos - pDot1->m_pPos->m_Pos) +
			(pDot3->m_fTextureV - pDot1->m_fTextureV)*
			(pDot3->m_pPos->m_Pos - pDot1->m_pPos->m_Pos);

		AngleU.Norm();
		AngleV.Norm();

		pDot1->m_pPos->m_AngleU += AngleU;
		pDot1->m_pPos->m_AngleV += AngleV;
		pDot2->m_pPos->m_AngleU += AngleU;
		pDot2->m_pPos->m_AngleV += AngleV;
		pDot3->m_pPos->m_AngleU += AngleU;
		pDot3->m_pPos->m_AngleV += AngleV;
		pPolygons++;
	}

	for (iI = 0; iI < m_iNumDot3DPos; iI++)
	{
		m_pDot3DPos[iI].m_AngleU.Norm();
		CVector3D Temp = m_pDot3DPos[iI].m_AngleU^m_pDot3DPos[iI].m_Norm;
		if (Temp*m_pDot3DPos[iI].m_AngleV > 0)
			m_pDot3DPos[iI].m_AngleV = Temp;
		else
			m_pDot3DPos[iI].m_AngleV = -1*Temp;

		m_pDot3DPos[iI].m_AngleV.Norm();
	}
}




// Destructors
CMV2Dot3DPos::~CMV2Dot3DPos()
{
}



CMV2Dot3D::~CMV2Dot3D()
{
}



CMV2Polygon::~CMV2Polygon()
{
}






CMV2PolygonObject::CMV2PolygonObject()
{
	m_pPolygons = NULL;
	m_ppPolygons = NULL;
	m_iNumPolygons = 0;

	m_pDot3DPos = NULL;
	m_ppDot3DPos = NULL;
	m_iNumDot3DPos = 0;

	m_pDot3D = NULL;
	m_iNumDot3D = 0;

	m_iFlagNormCalc = 0;
}



CMV2PolygonObject::~CMV2PolygonObject()
{
}

