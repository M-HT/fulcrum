// Filename: include\m2partic.h
//
// Developed by Oliver Belaifa in 1997
//


#ifndef _M2PARTIC_H_INCLUDED
#define _M2PARTIC_H_INCLUDED

#include<m2vector.h>
#include<m2camera.h>
#include<m2raw.h>
#include<demo.h>


class CMV2Particle
{
public:
	CVector3D 	m_LastPos;
	CVector3D	m_NextPos;
	CVector3D	m_InterpPos;
	CVector3D 	m_StartPos;
	CVector3D	m_EndPos;

	int 		m_iDuration;

	CMV2Particle();
	~CMV2Particle();
};



class CMV2PBernoulli
{
public:
	CMV2Particle   *m_pParticles;
	int			m_iNumParticles;
	float	    *m_pfSinTab;
	int			m_iSinTabRange;

	float		m_fSF;			// SmokeFactor
	float		m_fMorphFactor;

	int 		m_iCurFrame;

	int			m_iToBeRendered;
	int			m_iFrame;
	CMV2Camera 	m_Camera;

	void Init(tstream &s, int m_iXmax, int m_iYmax);
	void Draw(char *pcBackBuffer);
	void Calc(int iCounter);
	void SetRandomPos();

	CMV2PBernoulli();
	~CMV2PBernoulli();
};



#endif
