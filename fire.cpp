#include<demo.h>
#include<stdlib.h>
#include<copro.h>
#include<2dlink.h>


extern "C"
{
	void DrawFireASM(char *pDestBack,
		unsigned char *pAmpTab, unsigned char *YAmpTab,
		int m_iXmax, int m_iYmax);
	int	pFireRGBLookUp[256];
}




void CFire::Draw(char *pcDestBack, int iFrame)
{
	float fTimeCounter = (float)iFrame;
	fTimeCounter/= 1000;

	CalcYAmpTab(fTimeCounter);

	DrawFireASM(pcDestBack, m_pcAmpTab, m_pcYAmpTab,
		m_iXmax, m_iYmax);
}





void CFire::CalcAmpTab()
{
	float fFreq = 0.6;
	float fXmax = (float)m_iXmax;
	float fYmax = (float)m_iYmax;

	float fAmp = 191.0;
	float fDeltaAmp = -191.0/fXmax;



	int iX, iY;
	for (iY = 0; iY < m_iYmax; iY++)
	{
		for (iX = 0; iX < m_iXmax; iX++)
		{
			float fX = (float)iX - m_iXmax/2;
			float fValue = 16.0 + 16.0*sin(fX*fFreq/fXmax*2.0*3.14159*fFreq);

			fValue += fAmp;

			if (fValue > 191.0) fValue = 191.0;
			if (fValue < 0) fValue = 0;

			unsigned char cValue = (unsigned char)fValue;
			m_pcAmpTab[iY*m_iYmax + iX] = cValue;
		}

		fFreq += 0.015;

		fAmp += fDeltaAmp;
	}
}



void CFire::CalcYAmpTab(float fTime)
{
	float fDelta = fTime*2;
	float fXmax = (float)m_iXmax;

	float fFreq = 4.0;

	int iX;
	for (iX = 0; iX < m_iXmax; iX++)
	{
		float fX = (float)iX;
		float fValue = 16.0 + 16.0*sin((fX*fFreq/fXmax + fDelta)*2.0*3.14159);

		if (fValue > 32.0) fValue = 32.0;

		unsigned char cValue = (unsigned char)fValue;
		m_pcYAmpTab[iX] = cValue;
	}
}




void CFire::Init(tstream &s, int iXmax, int iYmax)
{
	m_iXmax = iXmax;
	m_iYmax = iYmax;

	m_pcAmpTab = (unsigned char *)getmem(m_iXmax*m_iYmax);
		 //new unsigned char[m_iXmax*m_iYmax];
	m_pcYAmpTab = (unsigned char *)getmem(m_iYmax);
		 //new unsigned char[m_iYmax];

	CalcAmpTab();
	CalcYAmpTab(0);


	int iI;
	for (iI = 0; iI < 256; iI++)
		pFireRGBLookUp[iI] = (s.getch() << 16) + (s.getch() << 8) + s.getch();
}



CFire::CFire()
{
	m_iXmax = 0;
	m_iYmax = 0;

	m_pcAmpTab = NULL;
	m_pcYAmpTab = NULL;
}



CFire::~CFire()
{
}











