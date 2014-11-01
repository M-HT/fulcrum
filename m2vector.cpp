// File: m2vector.cpp
//
// Developed by Oliver Belaifa in 1997

//#include<copro.h>
#include <math.h>
#include<stdio.h>
#include<m2vector.h>
#include<m2except.h>

// Constructors

CVector3D::CVector3D()
{
        m_fX = 0;
        m_fY = 0;
        m_fZ = 0;
}



CVector3D::CVector3D(const float fX,
                                         const float fY,
                                         const float fZ)
{
        m_fX = fX;
        m_fY = fY;
        m_fZ = fZ;
}



// Destructors

CVector3D::~CVector3D()
{
}




// šberladene Operatoren

CVector3D &CVector3D::operator +=(const CVector3D &a)
{
        m_fX += a.m_fX;
        m_fY += a.m_fY;
        m_fZ += a.m_fZ;

        return *this;
}



CVector3D &CVector3D::operator -=(const CVector3D &a)
{
        m_fX -= a.m_fX;
        m_fY -= a.m_fY;
        m_fZ -= a.m_fZ;

        return *this;
}



CVector3D &CVector3D::operator *=(const float &fR)
{
        m_fX *= fR;
        m_fY *= fR;
        m_fZ *= fR;

        return *this;
}



CVector3D &CVector3D::operator /=(const float &fR)
{
        m_fX /= fR;
        m_fY /= fR;
        m_fZ /= fR;

        return *this;
}



CVector3D operator +(const CVector3D &a,
                                         const CVector3D &b)
{
        return CVector3D(a.m_fX + b.m_fX, a.m_fY + b.m_fY, a.m_fZ + b.m_fZ);
}



CVector3D operator -(const CVector3D &a,
                                         const CVector3D &b)
{
        return CVector3D(a.m_fX - b.m_fX, a.m_fY - b.m_fY, a.m_fZ - b.m_fZ);
}




// Skalarmultiplikation
float operator *(const CVector3D &a,
                                 const CVector3D &b)
{
        return a.m_fX*b.m_fX + a.m_fY*b.m_fY + a.m_fZ*b.m_fZ;
}



// Vectorprodukt
CVector3D operator ^(const CVector3D &a,
                                         const CVector3D &b)
{
        return CVector3D(a.m_fY*b.m_fZ - a.m_fZ*b.m_fY,
                                         a.m_fZ*b.m_fX - a.m_fX*b.m_fZ,
                                         a.m_fX*b.m_fY - a.m_fY*b.m_fX);
}



CVector3D operator *(const float &fR,
                                         const CVector3D &a)
{
        return CVector3D(fR*a.m_fX, fR*a.m_fY, fR*a.m_fZ);
}



CVector3D operator *(const CVector3D &a,
                                         const float &fR)
{
        return CVector3D(a.m_fX*fR, a.m_fY*fR, a.m_fZ*fR);
}



CVector3D operator /(const CVector3D &a,
                                         const float &fR)
{
        try
        {
                if (fR == 0)
                        throw(new CMV2NormException("Dividide by Zero", __FILE__, __LINE__));
        }
        catch (CMV2NormException *Exception)
        {
                Exception->PrintError();
                delete Exception;
        }

        return CVector3D(a.m_fX/fR, a.m_fY/fR, a.m_fZ/fR);
}



CVector3D operator /(const float &fR,
                                         const CVector3D &a)
{
        try
        {
                if (a.m_fX == 0 || a.m_fY == 0 || a.m_fZ == 0)
                        throw(new CMV2NormException("Dividide by Zero", __FILE__, __LINE__));
        }
        catch (CMV2NormException *Exception)
        {
                Exception->PrintError();
                delete Exception;
        }

        return CVector3D(fR/a.m_fX, fR/a.m_fY, fR/a.m_fZ);
}




void CVector3D::Norm()
{
        float fAbsolute = sqrtf(m_fX*m_fX + m_fY*m_fY + m_fZ*m_fZ);
/*
		try
		{
				if (fAbsolute == 0)
						throw new CMV2NormException("Cannot Normalize a Null Vector", __FILE__, __LINE__);
		}
		catch (CMV2NormException *Exception)
		{
				Exception->PrintError();
				delete Exception;
		}
*/
		float fAbsoluteRzp;

		if (fAbsolute == 0)
			fAbsoluteRzp = 1.0;
		else
			fAbsoluteRzp = 1/fAbsolute;

		m_fX *= fAbsoluteRzp;
		m_fY *= fAbsoluteRzp;
		m_fZ *= fAbsoluteRzp;
}












