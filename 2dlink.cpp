#include"demo.h"
#include"copro.h"
#include"stdlib.h"
#include"2dlink.h"





extern "C"
{
	void Draw2DDreamASM(char *pBackB1, char *pBackB2,
		unsigned int *pPolarTab, float *pDistFunc, float *pSinTab,
		float *pCosTab, char *pDestBBack, int iCrossfadeFactor,
		int iXmax, int iYmax);

	void Init2DDreamJumpTableASM();
}



static int iCounter = -1;
static int iFade = 0;
static int iStartDelta = 0;


void C2DLinkEffect::Draw(char *pcBackBuffer1, char *pcBackBuffer2,
	char *pcDestBackBuffer, int iFrame)
{
	float fFrame = (float)iFrame;
	fFrame *= 0.56;

	if (iCounter < 0)
	{
		//iCounter = m_iDistanceFuncRange - m_iRange;
		iCounter = (int)((float)(m_iDistanceFuncRange - m_iRange)/
			m_fResFactor);
	}

	Draw2DDreamASM(pcBackBuffer1, pcBackBuffer2,
		m_piPolarTab, &m_pfDistFunc[(int)(m_fResFactor*((float)(iCounter)))],
		m_pfSinTab, m_pfCosTab, pcDestBackBuffer,
		iFade, m_iXmax, m_iYmax);

	if (fFrame > ((float)(iStartDelta + m_iPeriode))/m_fResFactor)
		iFade = (int)((fFrame - (float)(iStartDelta + m_iPeriode)
		/m_fResFactor)/1.2);

	iCounter = (int)((float)(m_iDistanceFuncRange - m_iRange)/m_fResFactor - fFrame);
}




void C2DLinkEffect::CalcPolarTab()
{
	int iX, iY;
	for (iY = 0; iY < m_iYmax; iY++)
		for (iX = 0; iX < m_iXmax; iX++)
		{
			float fX = (float)iX;
			float fY = (float)iY;

			float fDX = fX - m_iXmax/2;
			float fDY = fY - m_iYmax/2;

			float fAbs = sqrt(fDX*fDX + fDY*fDY);
			float fAngle;

			if (fAbs > 0)
			{
				float fAbsRZP = 1/fAbs;
				float fCos = fDX*fAbsRZP;
				float fSin = fDY*fAbsRZP;


//				fAngle = arctan(fSin,fCos) + pi;
//				if (fAngle < 0.0) fAngle = 0.0;
//				if (fAngle > 2*pi) fAngle = 2*pi;


				if (fCos == 0)
				{
					if (fSin > 0)
						fAngle = 0.5*3.1415926;
					else
						fAngle = 1.5*3.1415926;
				}
				else
				{
					fAngle = arctan(1.0,fSin/fCos);//atan(fSin/fCos);

					if (fCos < 0)
						fAngle += 3.1415926;
					else
						if (fSin < 0)
							fAngle += 2.0*3.1415926;
				}

			}
			else
				fAngle = 0;

			unsigned int iAbs = (unsigned int)(fAbs*float(m_iRange/(int)(256.0*m_fResFactor)));
			iAbs = iAbs<<16;

			unsigned int iAngle = (unsigned int)
				(m_fSinTabRange*fAngle/(2.0*3.1415926));

			unsigned int iRes = iAbs | iAngle;

			m_piPolarTab[iY*m_iXmax + iX] = iRes;
		}
}


void C2DLinkEffect::InitSinTab()
{
	int iI;
	for (iI = 0; iI < m_iSinTabRange; iI++)
	{
		float fI = (float)iI;
		float fAngle = 2.0*3.1415926*fI/m_fSinTabRange;
		m_pfSinTab[iI] = sin(fAngle);
		m_pfCosTab[iI] = cos(fAngle);
	}
}



void C2DLinkEffect::CalcDistanceFunc()
{
	int iI;
	for (iI = 0; iI < m_iDistanceFuncRange; iI++)
		m_pfDistFunc[iI] = 1.0;


	float fAmp = 0.3;
	float fAmpDelta = 0.25/(m_fPeriode/2.0);
	int iCurPos = m_iRange + iStartDelta;
	for (iI = iCurPos; iI < iCurPos + m_iPeriode/2; iI++)
	{
		float fI = (float)iI;
		float fCos = cos((fI - (float)iCurPos)/m_fPeriode*2.0*3.14159);
		fCos *= fCos;
		float fValue = 1.0 - fAmp + fAmp*fCos;
		m_pfDistFunc[m_iDistanceFuncRange - iI] = fValue;

		fAmp += 0.5*fAmpDelta;
	}

	iCurPos += m_iPeriode/2;
	for (iI = iCurPos; iI < iCurPos + m_iPeriode/2; iI++)
	{
		float fI = (float)iI;
		float fCos = cos((fI - (float)iCurPos)/m_fPeriode*2.0*3.14159);
		fCos *= fCos;
		float fValue = 1.0 - fAmp + fAmp*fCos;
		m_pfDistFunc[m_iDistanceFuncRange - iI] = fValue;

		fAmp += 1.0*fAmpDelta;
	}
	iCurPos += m_iPeriode/2;

	for (iI = iCurPos; iI < iCurPos + m_iPeriode/2; iI++)
	{
		float fI = (float)iI;
		float fCos = cos((fI - (float)iCurPos)/m_fPeriode*2.0*3.14159);
		fCos *= fCos;
		float fValue = 1.0 - fAmp + fAmp*fCos;
		m_pfDistFunc[m_iDistanceFuncRange - iI] = fValue;

		fAmp += 1.5*fAmpDelta;
	}
	iCurPos += m_iPeriode/2;

	for (iI = iCurPos; iI < iCurPos + m_iPeriode/2; iI++)
	{
		float fI = (float)iI;
		float fCos = cos((fI - (float)iCurPos)/m_fPeriode*2.0*3.14159);
		fCos *= fCos;
		float fValue = 1.0 - fAmp + fAmp*fCos;
		m_pfDistFunc[m_iDistanceFuncRange - iI] = fValue;
	}
	iCurPos += m_iPeriode/2;


	for (iI = iCurPos; iI < iCurPos + m_iPeriode/2; iI++)
	{
		float fI = (float)iI;
		float fCos = cos((fI - (float)iCurPos)/m_fPeriode*2.0*3.14159);
		fCos *= fCos;
		float fValue = 1.0 - fAmp + fAmp*fCos;
		m_pfDistFunc[m_iDistanceFuncRange - iI] = fValue;

		fAmp -= 2*fAmpDelta;
	}
	iCurPos += m_iPeriode/2;


	for (iI = iCurPos; iI < iCurPos + m_iPeriode/2; iI++)
	{
		float fI = (float)iI;
		float fCos = cos((fI - (float)iCurPos)/m_fPeriode*2.0*3.14159);
		fCos *= fCos;
		float fValue = 1.0 - fAmp + fAmp*fCos;
		m_pfDistFunc[m_iDistanceFuncRange - iI] = fValue;

		fAmp -= 1.5*fAmpDelta;
	}
	iCurPos += m_iPeriode/2;

	for (iI = iCurPos; iI < iCurPos + m_iPeriode/2; iI++)
	{
		float fI = (float)iI;
		float fCos = cos((fI - (float)iCurPos)/m_fPeriode*2.0*3.14159);
		fCos *= fCos;
		float fValue = 1.0 - fAmp + fAmp*fCos;
		m_pfDistFunc[m_iDistanceFuncRange - iI] = fValue;

		fAmp -= 0.5*fAmpDelta;
	}
	iCurPos += m_iPeriode/2;


}




void C2DLinkEffect::Init(int iXmax, int iYmax)
{
	m_iXmax = iXmax;
	m_iYmax = iYmax;

	m_fResFactor = ((float)iXmax)/320.0;
	m_iDistanceFuncRange = (int)(50000.0*m_fResFactor);
	m_iSinTabRange = (int)(4000.0*m_fResFactor);
	m_fSinTabRange = 4000.0*m_fResFactor;
	m_iRange = (int)(1024.0*m_fResFactor);
	m_iPeriode = (int)(1024.0*m_fResFactor);
	m_fPeriode = 1024.0*m_fResFactor;

	m_piPolarTab = (unsigned int *)getmem(4*m_iXmax*m_iYmax);
		//new unsigned int[m_iXmax*m_iYmax];
	m_pfDistFunc = (float *)getmem(4*m_iDistanceFuncRange);
		//new float[m_iDistanceFuncRange];
	m_pfSinTab = (float *)getmem(4*m_iSinTabRange);
		//new float[m_iSinTabRange];
	m_pfCosTab = (float *)getmem(4*m_iSinTabRange);
		//new float[m_iSinTabRange];

	CalcPolarTab();
	InitSinTab();
	CalcDistanceFunc();
	Init2DDreamJumpTableASM();
}




C2DLinkEffect::C2DLinkEffect()
{
	m_iXmax = 0;
	m_iYmax = 0;

	m_fResFactor = 0;
	m_iDistanceFuncRange = 0;
	m_iSinTabRange = 0;
	m_fSinTabRange = 0;
	m_iRange = 0;
	m_iPeriode = 0;
	m_fPeriode = 0;

	m_piPolarTab = NULL;
	m_pfDistFunc = NULL;
	m_pfSinTab = NULL;
	m_pfCosTab = NULL;
}




C2DLinkEffect::~C2DLinkEffect()
{
}


