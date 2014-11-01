// Filename: m2partic.cpp
//
// Developed by Oliver Belaifa in 1997
//

#include<stdlib.h>
#include<stdio.h>
//#include<copro.h>
#include <math.h>
#include<m2partic.h>

//#include<m2image.h>


extern "C"
{
	void SetPixelMem(int iX, int iY, char *pcMem, int iXmax, int iCol);

	void MV2ParticleProjectionASM(CMV2Camera *pMV2Camera,
		CMV2Particle *pMV2Particles, int iNumParticles,
		char *pcBackBuffer);

	void MV2ParticleDoBernoulliASM(CMV2Particle *pParticles,
		int iNumParticles, float *pfSinTab, int iCurTime,
		float fAmpX, float fAmpY1, float fAmpY2, float fAmpZ,
		float fFreqX, float fFreqY1, float fFreqY2, float fFreqZ,
		float fSinDX, float fSinDY1, float fSinDY2, float fSinDZ,
		float fDY1, float fDY2);

	void MV2ParticleDoBernoulliMorphToObjectASM(CMV2Particle *pParticles,
		int iNumParticles, float *pfSinTab, int iCurTime,
		float fAmpX, float fAmpY1, float fAmpY2, float fAmpZ,
		float fFreqX, float fFreqY1, float fFreqY2, float fFreqZ,
		float fSinDX, float fSinDY1, float fSinDY2, float fSinDZ,
		float fDY1, float fDY2, float fMFactor);

	void MV2ParticleInterpolateASM(CMV2Particle *pParticles,
		int iNumParticles, float fMorphFactor);


void Calc32BitLookupASM(char *pcPalette, char *pcSparklesLookup);

char pcSparklesLookup[256*4];

}






void CMV2PBernoulli::Init(tstream &s, int m_iXmax, int m_iYmax)
{
	int iI;
	char pal[768];

//	s.openfile("adler.pos");

	//read dots
	m_iNumParticles = s.getint();
	m_pParticles = (CMV2Particle *) getmem(m_iNumParticles*sizeof(CMV2Particle));//new CMV2Particle[m_iNumParticles];

	for (iI = 0; iI < m_iNumParticles; iI++)
	{
	   m_pParticles[iI].m_EndPos.m_fX = s.getfloat();
	   m_pParticles[iI].m_EndPos.m_fY = s.getfloat();
	   m_pParticles[iI].m_EndPos.m_fZ = s.getfloat();
	}


	//read palette
	s.read(pal,768);
	Calc32BitLookupASM(pal, pcSparklesLookup);
/*
	CImage *pImage = new CImage();
	pImage->LoadPCX("part.pcx");
	Calc32BitLookupASM(pImage->m_pcPalette, pcSparklesLookup);
	delete pImage;
*/





	m_iSinTabRange = 256;
	m_pfSinTab = (float *) getmem(m_iSinTabRange*sizeof(float));//new float[m_iSinTabRange];

	for (iI = 0; iI < m_iSinTabRange; iI++)
	{
		float fCurAngle = 2.0*3.14159*(float(iI))/(float(m_iSinTabRange));
		m_pfSinTab[iI] = sinf(fCurAngle);
	}

	SetRandomPos();

	Calc(m_iCurFrame);
	m_iCurFrame++;

	m_iToBeRendered = 0;
	m_iFrame = 0;
}












static int wrand(void)
{
    return (int)( rand() * (32767.0/((double)RAND_MAX)) );
}





void CMV2PBernoulli::SetRandomPos()
{
	int iRange = 10;
	int iI;
	for (iI = 0; iI < m_iNumParticles; iI++)
	{
		m_pParticles[iI].m_NextPos = CVector3D(wrand()/500000.0, wrand()/500000.0,
		wrand()/500000.0);

		m_pParticles[iI].m_StartPos = m_pParticles[iI].m_NextPos;
		m_pParticles[iI].m_iDuration = wrand()/2000;
	}
}



void CMV2PBernoulli::Calc(int iCounter)
{
       	float g = 12.0;
    	int h = 10;

	/*
	void MV2ParticleDoBernoulliMorphToObjectASM(CMV2Particle *pParticles,
		int iNumParticles, float *pfSinTab, int iCurTime,
		float fAmpX, float fAmpY1, float fAmpY2, float fAmpZ,
		float fFreqX, float fFreqY1, float fFreqY2, float fFreqZ,
		float fSinDX, float fSinDY1, float fSinDY2, float fSinDZ,
		float fDY1, float fDY2, float fMFactor);
	*/

	if (iCounter <= 200)
	{
		MV2ParticleDoBernoulliMorphToObjectASM(
		m_pParticles, m_iNumParticles,
		m_pfSinTab, iCounter,
		0.03, 0.02, 0.02, 0.02,
		9.0*m_fSF, 7.0*m_fSF, 7.0*m_fSF, 11.0*m_fSF,
		0.1*m_fSF, 0.125*m_fSF,0.167*m_fSF, 0.142*m_fSF,
		-0.8*1.0, -0.8*1.0, m_fMorphFactor);
	}

	m_fSF /= 1.025;

	if (iCounter > 100 + h)
		m_fMorphFactor += 0.0002*g;
	else
	if (iCounter > 105 + h)
		m_fMorphFactor += 0.0004*g;
	else
	if (iCounter > 110 + h)
		m_fMorphFactor += 0.0008*g;
	else
	if (iCounter > 115 + h)
		m_fMorphFactor += 0.0016*g;
	else
	if (iCounter > 120 + h)
		m_fMorphFactor += 0.0032*g;
	else
	if (iCounter > 125 + h)
		m_fMorphFactor += 0.0064*g;
	else
	if (iCounter > 130 + h)
		m_fMorphFactor += 0.0128*g;
	else
	if (iCounter > 135 + h)
		m_fMorphFactor += 0.0064*g;
	else
	if (iCounter > 140 + h)
		m_fMorphFactor += 0.0032*g;
	else
	if (iCounter > 145 + h)
		m_fMorphFactor += 0.0016*g;
	else
	if (iCounter > 150 + h)
		m_fMorphFactor += 0.0008*g;
	else
	if (iCounter > 155 + h)
		m_fMorphFactor += 0.0004*g;
	else
	if (iCounter > 160 + h)
		m_fMorphFactor += 0.0002*g;

	if (m_fMorphFactor > 1.0)
		m_fMorphFactor = 1.0;

}


void CMV2PBernoulli::Draw(char *pcBackBuffer)
{
	m_iToBeRendered = 0;

	int iTimeCounter = m_iFrame;

	int iLastFrame = iTimeCounter/100;
	int iNextFrame = iLastFrame + 1;

	while (m_iCurFrame < iNextFrame)
	{
		Calc(m_iCurFrame);
		m_iCurFrame++;
	}

	int iModulu = iTimeCounter%100;
	float fMorphFactor = (float)iModulu;
	fMorphFactor /= 100;
	MV2ParticleInterpolateASM(m_pParticles, m_iNumParticles, fMorphFactor);
	MV2ParticleProjectionASM(&m_Camera, m_pParticles, m_iNumParticles,
		pcBackBuffer);
}




CMV2PBernoulli::CMV2PBernoulli()
{
	m_pParticles = NULL;
	m_iNumParticles = 0;
	m_pfSinTab = NULL;
	m_iSinTabRange = 0;

	m_fSF = 4.0;
	m_fMorphFactor = 0;

	m_iCurFrame = -1;
}





CMV2PBernoulli::~CMV2PBernoulli()
{
}




CMV2Particle::CMV2Particle()
{
}



CMV2Particle::~CMV2Particle()
{
}






/*
void DoBernoulli(
	float fAmpX, float fAmpY1, float fAmpY2, float fAmpZ,
	float fFreqX, float fFreqY1, float fFreqY2, float fFreqZ,
	float fSinDX, float fSinDY1, float fSinDY2, float fSinDZ,
	float fDY1, float fDY2)
{

		const float fTWO_PI_RZP = 1/(2.0*3.14159);
		float fSinTabRange = float(m_iSinTabRange);

		int iI;
		for (iI = 0; iI < m_iNumParticles; iI++)
		{
				if (m_pParticles[iI].m_iDuration == 0)
				{
						m_pParticles[iI].m_Pos = m_pParticles[iI].m_StartPos;
						m_pParticles[iI].m_iDuration = 1000;
				}
				else
				{
						float fAngleX = m_pParticles[iI].m_Pos.m_fY*fFreqX + m_iCurTimePos*fSinDX;
						float fAngleY1 = m_pParticles[iI].m_Pos.m_fZ*fFreqY1 + m_iCurTimePos*fSinDY1;
						float fAngleY2 = m_pParticles[iI].m_Pos.m_fX*fFreqY2 + m_iCurTimePos*fSinDY2;
						float fAngleZ = m_pParticles[iI].m_Pos.m_fX*fFreqZ + m_iCurTimePos*fSinDZ;

						int iAngleX = int(fTWO_PI_RZP*fAngleX*fSinTabRange)%m_iSinTabRange;
						int iAngleY1 = int(fTWO_PI_RZP*fAngleY1*fSinTabRange)%m_iSinTabRange;
	 					int iAngleY2 = int(fTWO_PI_RZP*fAngleY2*fSinTabRange)%m_iSinTabRange;
						int iAngleZ = int(fTWO_PI_RZP*fAngleZ*fSinTabRange)%m_iSinTabRange;

						if (iAngleX < 0) iAngleX += m_iSinTabRange;
						if (iAngleY1 < 0) iAngleY1 += m_iSinTabRange;
						if (iAngleY2 < 0) iAngleY2 += m_iSinTabRange;
						if (iAngleZ < 0) iAngleZ += m_iSinTabRange;


						m_pParticles[iI].m_Pos.m_fX += fAmpX*m_fSinTab[iAngleX];
						m_pParticles[iI].m_Pos.m_fY += fAmpY1*(fDY1 + m_fSinTab[iAngleY1]);
						m_pParticles[iI].m_Pos.m_fY += fAmpY2*(fDY2 + m_fSinTab[iAngleY2]);
                        m_pParticles[iI].m_Pos.m_fZ += fAmpZ*m_fSinTab[iAngleZ];

						m_pParticles[iI].m_iDuration--;
				}
		}

	m_iCurTimePos++;
}
*/

