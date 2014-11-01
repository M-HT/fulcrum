// Filename: gfxstrIO.cpp
//
// Developed by Oliver Belaifa in 1997
//

#include<stdlib.h>
#include<gfxstrio.h>
#include<string.h>



extern "C"
{
	void DrawGfxCharacter32BitASM(char *pcString, int iBackBufferPos,
		void *pBackBuffer, int iXmax);
}



void CGfxStrIO::WriteStr(char *pcString)
{
	int iStringLength = strlen(pcString);

	if (m_iPosY != m_iYmax - 1)
	{
		if (m_iXmax + m_iPosX < iStringLength)
		{
			iStringLength = m_iXmax + m_iPosX;
		}

		int iI;
		for (iI = 0; iI < iStringLength; iI++)
		{
			DrawGfxCharacter32BitASM(&pcString[iI],
				(m_iPosY*m_iCharacterYSize*m_iBackBufferXmax +
				m_iPosX*m_iCharacterXSize)*m_iBytesPerPixel,
				m_pBackBuffer, m_iBackBufferXmax);
			m_iPosX++;
		}

		m_iPosX = 0;
		m_iPosY++;
	}
}



void CGfxStrIO::WriteStr(char *pcString, char *pcBackBuffer)
{
	int iStringLength = strlen(pcString);

	if (m_iPosY != m_iYmax - 1)
	{
		if (m_iXmax + m_iPosX < iStringLength)
		{
			iStringLength = m_iXmax + m_iPosX;
		}

		int iI;
		for (iI = 0; iI < iStringLength; iI++)
		{
			DrawGfxCharacter32BitASM(&pcString[iI],
				(m_iPosY*m_iCharacterYSize*m_iBackBufferXmax +
				m_iPosX*m_iCharacterXSize)*m_iBytesPerPixel,
				pcBackBuffer, m_iBackBufferXmax);
			m_iPosX++;
		}

		m_iPosX = 0;
		m_iPosY++;
	}
}



void CGfxStrIO::SetPos(int iX, int iY)
{
	if ((iX < m_iXmax) && (iY  < m_iYmax))
	{
		m_iPosX = iX;
		m_iPosY = iY;
	}
}



void CGfxStrIO::ResetPos()
{
	m_iPosX = 0;
	m_iPosY = 0;
}



void CGfxStrIO::TakeBackBuffer(void *pBackBuffer)
{
	m_pBackBuffer = pBackBuffer;
}



void CGfxStrIO::Init(int iXmax, int iYmax)
// iXmax,iYmax : BackBufferResolution
{
	m_iBackBufferXmax = iXmax;
	m_iBackBufferYmax = iYmax;

	m_iPosX = 0;
	m_iPosY = 0;

	m_iXmax = m_iBackBufferXmax/m_iCharacterXSize;
	m_iYmax = m_iBackBufferYmax/m_iCharacterYSize;

	m_iBytesPerPixel = 4;
}



void CGfxStrIO::Free()
{
}



CGfxStrIO::CGfxStrIO()
{
	m_iXmax = 0;
	m_iYmax = 0;
	m_iPosX = 0;
	m_iPosY = 0;
	m_iBackBufferXmax = 0;
	m_iBackBufferYmax = 0;
	m_pBackBuffer = NULL;

	m_iBytesPerPixel = 0;

	m_iCharacterXSize = 8;
	m_iCharacterYSize = 8;
}



CGfxStrIO::~CGfxStrIO()
{
}

