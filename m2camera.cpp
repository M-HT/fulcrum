// Filename: m2camera.cpp
//
// Developed by Oliver Belaifa in 1997
//


#include<stdlib.h>
#include<copro.h>
#include<m2camera.h>
#include<demo.h>


CMV2Camera::CMV2Camera()
{
	m_Pos = CVector3D(0, 0, 0);
	m_Front = CVector3D(0, 0, 1);
	m_Right = CVector3D(1, 0, 0);
	m_Down = CVector3D(0, 1, 0);

	m_Pos1 = m_Pos;
	m_Front1 = m_Front;
	m_Right1 = m_Right;
	m_Down1 = m_Down;

	m_fAngleF = 0;
	m_fAngleR = 0;
	m_fAngleD = 0;
	m_fAngleX = 0;
	m_fAngleY = 0;
	m_fAngleZ = 0;


// Der ScreenFactor wird spaeter in die Camera Vectoren einberechnet
	m_fScreenFactor = 256.0;
}



CMV2Camera::CMV2Camera(CVector3D Pos, CVector3D Front,
	CVector3D Right, CVector3D Down)
{
	m_Pos = Pos;
	m_Front = Front;
	m_Right = Right;
	m_Down = Down;

	m_Pos1 = m_Pos;
	m_Front1 = m_Front;
	m_Right1 = m_Right;
	m_Down1 = m_Down;

	m_fAngleF = 0;
	m_fAngleR = 0;
	m_fAngleD = 0;
	m_fAngleX = 0;
	m_fAngleY = 0;
	m_fAngleZ = 0;
	// Die Winkel muessen noch berechnet werden.


	m_fScreenFactor = 256.0;
}



CMV2Camera::CMV2Camera(CVector3D Pos, float fAngleF,
	float fAngleR, float fAngleD)
{
	m_Pos = Pos;

	m_Pos = CVector3D(0, 0, 0);
	m_Front = CVector3D(0, 0, 1);
	m_Right = CVector3D(1, 0, 0);
	m_Down = CVector3D(0, 1, 0);

	m_fAngleF = fAngleF;
	m_fAngleR = fAngleR;
	m_fAngleD = fAngleD;

	m_Pos1 = m_Pos;
	m_Front1 = m_Front;
	m_Right1 = m_Right;
	m_Down1 = m_Down;


	UpdateViewDirection();
}



CMV2Camera::~CMV2Camera()
{
}



void *CMV2Camera::operator new(size_t s) {
  return getmem(s);
}

void CMV2Camera::operator delete(void *p) {

}


// Methods


void CMV2Camera::SetScreenParameters(int iXmax, int iYmax,
	int iClipXmin, int iClipYmin, int iClipXmax, int iClipYmax)
{
	m_iXmax = iXmax;
	m_iYmax = iYmax;
	m_iClipXmin = iClipXmin;
	m_iClipYmin = iClipYmin;
	m_iClipXmax = iClipXmax;
	m_iClipYmax = iClipYmax;
}



void CMV2Camera::RestoreViewVectors()
{
	m_Front = CVector3D(0, 0, 1);
	m_Right = CVector3D(1, 0, 0);
	m_Down = CVector3D(0, 1, 0);
}


void CMV2Camera::Rotate(float fDeltaF, float fDeltaR, float fDeltaD)
{
	m_fAngleF += fDeltaF;
	m_fAngleR += fDeltaR;
	m_fAngleD += fDeltaD;

	UpdateViewDirection();
}



void CMV2Camera::IncPos(float fDX, float fDY, float fDZ)
{
	m_Pos = m_Pos1;
	m_Front = m_Front1;
	m_Right = m_Right1;
	m_Down = m_Down1;

	m_Pos += fDZ*m_Front + fDY*m_Down + fDX*m_Right;
	m_Pos1 = m_Pos;
}



void CMV2Camera::SetPos(float fX, float fY, float fZ)
{
	m_Pos = m_Pos1;
	m_Front = m_Front1;
	m_Right = m_Right1;
	m_Down = m_Down1;

	m_Pos = CVector3D(fX, fY, fZ);
	m_Pos1 = m_Pos;
}




void CMV2Camera::IncObjectPos(float fX, float fY, float fZ)
{
	m_Pos = m_Pos1;
	m_Front = m_Front1;
	m_Right = m_Right1;
	m_Down = m_Down1;

	m_Pos -= CVector3D(fX, fY, fZ);
	m_Pos1 = m_Pos;
}




void CMV2Camera::SetObjectPos(float fX, float fY, float fZ)
{
	m_Pos = m_Pos1;
	m_Front = m_Front1;
	m_Right = m_Right1;
	m_Down = m_Down1;

	m_Pos = -1*CVector3D(fX, fY, fZ);
	m_Pos1 = m_Pos;
}



void CMV2Camera::SetViewDirection(float fAngleF, float fAngleR, float fAngleD)
{
	m_fAngleF = fAngleF;
	m_fAngleR = fAngleR;
	m_fAngleD = fAngleD;

	UpdateViewDirection();
}



void CMV2Camera::UpdateViewDirection()
{
	float fSinF = sin(m_fAngleF);
	float fCosF = cos(m_fAngleF);
	float fSinR = sin(m_fAngleR);
	float fCosR = cos(m_fAngleR);
	float fSinD = sin(m_fAngleD);
	float fCosD = cos(m_fAngleD);

	m_Pos = m_Pos1;
	m_Front = m_Front1;
	m_Right = m_Right1;
	m_Down = m_Down1;

	RestoreViewVectors();

	CVector3D Temp = m_Front;

	m_Front = fCosD*m_Front - fSinD*m_Right;
	m_Right = fCosD*m_Right + fSinD*Temp;

	Temp = m_Front;

	m_Front = fCosR*m_Front + fSinR*m_Down;
	m_Down = fCosR*m_Down - fSinR*Temp;

	Temp = m_Right;

	m_Right = fCosF*m_Right - fSinF*m_Down;
	m_Down = fCosF*m_Down + fSinF*Temp;

	m_Pos1 = m_Pos;
	m_Front1 = m_Front;
	m_Right1 = m_Right;
	m_Down1 = m_Down;
}



void CMV2Camera::RotateObjectZ(float fDAngle)
{
	float fTemp;

	float fCos = cos(fDAngle);
	float fSin = sin(fDAngle);

	fTemp = m_Pos.m_fX;
	m_Pos.m_fX = fCos*m_Pos.m_fX - fSin*m_Pos.m_fY;
	m_Pos.m_fY = fCos*m_Pos.m_fY + fSin*fTemp;

	fTemp = m_Front.m_fX;
	m_Front.m_fX = fCos*m_Front.m_fX - fSin*m_Front.m_fY;
	m_Front.m_fY = fCos*m_Front.m_fY + fSin*fTemp;

	fTemp = m_Right.m_fX;
	m_Right.m_fX = fCos*m_Right.m_fX - fSin*m_Right.m_fY;
	m_Right.m_fY = fCos*m_Right.m_fY + fSin*fTemp;

	fTemp = m_Down.m_fX;
	m_Down.m_fX = fCos*m_Down.m_fX - fSin*m_Down.m_fY;
	m_Down.m_fY = fCos*m_Down.m_fY + fSin*fTemp;
}



void CMV2Camera::RotateObjectY(float fDAngle)
{
	float fTemp;

	float fCos = cos(fDAngle);
	float fSin = sin(fDAngle);

	fTemp = m_Pos.m_fX;
	m_Pos.m_fX = fCos*m_Pos.m_fX - fSin*m_Pos.m_fZ;
	m_Pos.m_fZ = fCos*m_Pos.m_fZ + fSin*fTemp;

	fTemp = m_Front.m_fX;
	m_Front.m_fX = fCos*m_Front.m_fX - fSin*m_Front.m_fZ;
	m_Front.m_fZ = fCos*m_Front.m_fZ + fSin*fTemp;

	fTemp = m_Right.m_fX;
	m_Right.m_fX = fCos*m_Right.m_fX - fSin*m_Right.m_fZ;
	m_Right.m_fZ = fCos*m_Right.m_fZ + fSin*fTemp;

	fTemp = m_Down.m_fX;
	m_Down.m_fX = fCos*m_Down.m_fX - fSin*m_Down.m_fZ;
	m_Down.m_fZ = fCos*m_Down.m_fZ + fSin*fTemp;
}


void CMV2Camera::RotateObjectX(float fDAngle)
{
	float fTemp;

	float fCos = cos(fDAngle);
	float fSin = sin(fDAngle);

	fTemp = m_Pos.m_fY;
	m_Pos.m_fY = fCos*m_Pos.m_fY + fSin*m_Pos.m_fZ;
	m_Pos.m_fZ = fCos*m_Pos.m_fZ - fSin*fTemp;

	fTemp = m_Front.m_fY;
	m_Front.m_fY = fCos*m_Front.m_fY + fSin*m_Front.m_fZ;
	m_Front.m_fZ = fCos*m_Front.m_fZ - fSin*fTemp;

	fTemp = m_Right.m_fY;
	m_Right.m_fY = fCos*m_Right.m_fY + fSin*m_Right.m_fZ;
	m_Right.m_fZ = fCos*m_Right.m_fZ - fSin*fTemp;

	fTemp = m_Down.m_fY;
	m_Down.m_fY = fCos*m_Down.m_fY + fSin*m_Down.m_fZ;
	m_Down.m_fZ = fCos*m_Down.m_fZ - fSin*fTemp;
}



void CMV2Camera::RotateObject(float fDAngleX, float fDAngleY,
	float fDAngleZ)
{
	m_fAngleX += fDAngleX;
	m_fAngleY += fDAngleY;
	m_fAngleZ += fDAngleZ;
}



void CMV2Camera::UpdateObject()
{
	m_Pos = m_Pos1;
	m_Front = m_Front1;
	m_Right = m_Right1;
	m_Down = m_Down1;
	RotateObjectX(m_fAngleX);
	RotateObjectY(m_fAngleY);
	RotateObjectZ(m_fAngleZ);
}










