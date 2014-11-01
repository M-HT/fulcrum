//#include<copro.h>
#include <math.h>
#include<stdlib.h>

#include<2dlink.h>



extern "C"
{
	void Draw2DEyeASM(unsigned int *piMoveListTab,
		char *pcMask, char *pcPic, char *pcSrcBack,
		char *pcDestBack, int iNum);

}




void C2DEye::Draw(char *pcPic, char *pcMask, char *pcSrcBackBuffer,
	char *pcDestBackBuffer)
{
	Draw2DEyeASM(m_piMoveListTab, pcMask, pcPic, pcSrcBackBuffer,
		pcDestBackBuffer, m_iXmax*m_iYmax);


}





void C2DEye::CalcMoveListTab()
{

	float fAmp = 0.1;

	int iX, iY;
	for (iY = 0; iY < m_iYmax; iY++)
		for (iX = 0; iX < m_iXmax; iX++)
		{
			float fDX = (float)(iX - m_iXmax/2);
			float fDY = (float)(iY - m_iYmax/2);

			fDX *= (1.0 - fAmp - fAmp*cosf(fDX/(320.0*m_fResFactor)*2.0*3.1415926))*
			       (1.0 - fAmp - fAmp*cosf(fDY/(320.0*m_fResFactor)*2.0*3.1415926));

			fDY *= (1.0 - fAmp - fAmp*cosf(fDY/(320.0*m_fResFactor)*2.0*3.1415926));

			int iSrcX = (int)fDX;
			int iSrcY = (int)fDY;

			iSrcX += m_iXmax/2;
			iSrcY += m_iYmax/2;

			if (iSrcX < 0) iSrcX = 0;
			if (iSrcX >= m_iXmax) iSrcX = m_iXmax - 1;
			if (iSrcY < 0) iSrcY = 0;
			if (iSrcY >= m_iYmax) iSrcY = m_iYmax - 1;

			m_piMoveListTab[iY*m_iXmax + iX] = iSrcY*m_iXmax + iSrcX;
			//m_piMoveListTab[iY*m_iXmax + iX] = iY*m_iXmax + iX;

		}
}




void C2DEye::Init(int iXmax, int iYmax)
{
	m_iXmax = iXmax;
	m_iYmax = iYmax;

	m_fResFactor = ((float)iXmax)/320.0;
	m_piMoveListTab = (unsigned int *)getmem(4*m_iXmax*m_iYmax);
//		 new unsigned int [m_iXmax*m_iYmax];


	CalcMoveListTab();
}









C2DEye::C2DEye()
{
	m_iXmax = 0;
	m_iYmax = 0;

	m_piMoveListTab = NULL;
}



C2DEye::~C2DEye()
{
}




