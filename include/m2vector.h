// Filename: include\m_vector.h
//
// Developed by Oliver Belaifa in 1997
//


#ifndef _M2VECTOR_H_INCLUDED
#define _M2VECTOR_H_INCLUDED


class CVector3D

{
public:
	float m_fX, m_fY, m_fZ;

	CVector3D();
	CVector3D(const float fX, const float fY, const float fZ);
	~CVector3D();

	CVector3D &operator +=(const CVector3D &a);
	CVector3D &operator -=(const CVector3D &a);
	CVector3D &operator *=(const float &r);
	CVector3D &operator /=(const float &r);

	void Norm();
};


CVector3D operator +(const CVector3D &a, const CVector3D &b);
CVector3D operator -(const CVector3D &a, const CVector3D &b);
CVector3D operator *(const float &r, const CVector3D &a);
CVector3D operator *(const CVector3D &a, const float &r);
CVector3D operator /(const CVector3D &a, const float &r);
CVector3D operator /(const float &r, const CVector3D &a);

// Skalarmultiplikation
float operator *(const CVector3D &a, const CVector3D &b);

// Vectorprodukt
CVector3D operator ^(const CVector3D &a, const CVector3D &b);


#endif








