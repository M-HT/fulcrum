// Filename: include\gfxstrIO.h
//
// Developed by Oliver Belaifa in 1997
//

#ifndef _GFXSTR_H_INCLUDED
#define _GFXSTR_H_INCLUDED



class CGfxStrIO
{
public:
	int		m_iXmax, m_iYmax;
	int		m_iPosX, m_iPosY;

	int		m_iBackBufferXmax, m_iBackBufferYmax;
	void   *m_pBackBuffer;		

	int		m_iBytesPerPixel;

	int		m_iCharacterXSize, m_iCharacterYSize;

	void	Init(int iXmax, int iYmax);
	void	Free();
	void	TakeBackBuffer(void *pBackBuffer);

	void	WriteStr(char *pcString);
	void 	WriteStr(char *pcString, char *pcBackBuffer);
	void	SetPos(int iX, int iY);
	void	ResetPos();

	CGfxStrIO();
	~CGfxStrIO();
};



#endif
