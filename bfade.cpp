
#include<demo.h>
#include<stdlib.h>
//#include<copro.h>
#include <math.h>
#include<2dlink.h>


extern "C"
{
	void Draw2DBeamFadeASM(char *pcBackB1, char *pcBackB2,
		char *pcDestB, int iFFactor,
		int iXmax, int iYmax, int *piSinTabX, int *piSinTabY);

	void Init2DBeamFadeJumpTableASM();
};



void C2DBeamFade::Draw(int iFrame, char *pcBack1, char *pcBack2,
	char *pcDestB)
{
	CalcSinTab(m_fFX, m_fFY, m_fAX, m_fAY, m_fDX, m_fDY);

	Draw2DBeamFadeASM(pcBack1, pcBack2,
		pcDestB, (int)m_fAlpha, m_iXmax, m_iYmax,
		m_piSinTabX, m_piSinTabY);

	float fTC = (float)iFrame;
	fTC /= 1000.0;

	m_fDX = fTC*15;
	m_fDY = fTC*15;

	m_fAX = 5.0 - fTC*2.5;
	m_fAY = 5.0 - fTC*2.5;

	m_fFX = 12.0 - 1*fTC;
	m_fFY = 15.0 - 1.5*fTC;

	if (m_fFX < 3.0)  m_fFX = 3.0;
	if (m_fFY < 3.0)  m_fFY = 3.0;

	if (m_fAX < 0) m_fAX = 0;
	if (m_fAY < 0) m_fAY = 0;

	m_fAlpha = fTC*6;
}



void C2DBeamFade::CalcSinTab(float fFreqX, float fFreqY,
	float fAmpX, float fAmpY, float fDeltaX, float fDeltaY)
{
	float fXmax = (float)m_iXmax;
	float fYmax = (float)m_iYmax;

	int iI;
	for (iI = 0; iI < m_iXmax; iI++)
	{
		float fI = float(iI);
		float fValueX = fAmpX*sinf(fI*(fFreqX)/fXmax*2.0*3.14159 - fDeltaX);

		int iValueX = int(fValueX) + iI;
		if (iValueX < 0) iValueX = 0;
		if (iValueX >= m_iXmax) iValueX = m_iXmax - 1;

		m_piSinTabX[iI] = iValueX;
	}

	for (iI = 0; iI < m_iYmax; iI++)
	{
		float fI = float(iI);
		float fValueY = fAmpY*sinf(fI*(fFreqY)/fYmax*2.0*3.14159 - fDeltaY);

		int iValueY = int(fValueY) + iI;
		if (iValueY < 0) iValueY = 0;
		if (iValueY >= m_iYmax) iValueY = m_iYmax - 1;

		m_piSinTabY[iI] = iValueY;
	}

}


void C2DBeamFade::Init(int iXmax, int iYmax)
{
	m_iXmax = iXmax;
	m_iYmax = iYmax;

	m_piSinTabX = (int *)getmem(4*m_iXmax);
		 //new int[m_iXmax];
	m_piSinTabY = (int *)getmem(4*m_iYmax);
		 //new int[m_iYmax];

	m_fFX = 3.0;
	m_fFY = 4.0;
	m_fAX = 5;
	m_fAY = 5;
	m_fAlpha = 0;

	Init2DBeamFadeJumpTableASM();
}



C2DBeamFade::C2DBeamFade()
{
 	m_iXmax = 0;
	m_iYmax = 0;
	m_piSinTabX = NULL;
	m_piSinTabY = NULL;
	m_fFX = 0;
	m_fFY = 0;
	m_fAX = 0;
	m_fAY = 0;
	m_fDX = 0;
	m_fDY = 0;
}


C2DBeamFade::~C2DBeamFade()
{
}







