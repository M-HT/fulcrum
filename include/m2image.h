// Filename: include\m2image.h
//
// Developed by Oliver Belaifa in 1997
//

#include<stypes.h>


#ifndef _M2IMAGE_H_INCLUDED
#define _M2IMAGE_H_INCLUDED

class CImage
{
public:
	char *m_pcData;
	long m_lSizeOfData;
	UWORD m_iXmax, m_iYmax;
	char m_cBytesPerPixel;
	char *m_pcPalette;

	void LoadPCX(char *pcFileName);
	void LoadPCX8Bit(FILE *pFile);
	void LoadPCX24Bit(FILE *pFile);

	CImage();
	~CImage();
};

#endif
