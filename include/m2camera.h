// Filename: include\m_camera.h
//
// Developed by Oliver Belaifa in 1997
//

#include<m2vector.h>

#ifndef _M2CAMERA_H_INCLUDED
#define _M2CAMERA_H_INCLUDED

class CMV2Camera

{
public:
	CVector3D	m_Pos, m_Right, m_Down, m_Front;

	float       m_fAngleF, m_fAngleR, m_fAngleD;
	float       m_fAngleX, m_fAngleY, m_fAngleZ;

	float       m_fScreenFactor;

	int         m_iXmax, m_iYmax;

	int         m_iClipXmin, m_iClipYmin;
	int         m_iClipXmax, m_iClipYmax;


//test
	CVector3D m_Pos1, m_Front1, m_Right1, m_Down1;


// Constructors
	CMV2Camera();
	CMV2Camera(CVector3D Pos, CVector3D Front, CVector3D Right,
		CVector3D Down);
	CMV2Camera(CVector3D Pos, float fAngleF, float fAngleR, float fAngleD);

// Destructors
	~CMV2Camera();

//
	void *operator new(size_t);
	void operator delete(void *);

// Methods
	void SetScreenParameters(int iXmax, int iYmax, int iClipXmin,
		int iClipYmin, int iClipXmax, int iClipYmax);
	void IncPos(float fDX, float fDY, float fDZ);
	void IncObjectPos(float fX, float fY, float fZ);
	void SetObjectPos(float fX, float fY, float fZ);
	void RestoreViewVectors();
	void Rotate(float fDeltaF, float fDeltaR, float fDeltaD);

	void RotateObject(float fDAngleX, float fDAngleY, float fDAngleZ);
	void UpdateObject();
	void RotateObjectX(float fDAngle);
	void RotateObjectY(float fDAngle);
	void RotateObjectZ(float fDAngle);
	void SetPos(float fX, float fY, float fZ);
	void SetViewDirection(float fAngleF, float fAngleR, float fAngleD);
	void UpdateViewDirection();
};

#endif
