#include<demo.h>


extern "C"
{
	void Scale32ASM(char *pcSrc, char *pcDest, int iSrcXmax, int iSrcYmax,
		int iDestXmax, int iDestYmax);
};


extern void *coltab;
extern void initcoltab(tvesa &vesa);
extern "C" void CopyBufferASM(char *pSrcScreenBuffer, tvesa *pVesa, void *);
#pragma aux CopyBufferASM "*" parm [esi] [ebx] [edx] modify [eax ebx ecx edx esi edi]




void ShowPic(tstream &s, tvesa &vesa)
{
	initcoltab(vesa);

	s.openfile("karvel.bin");

	unsigned int *piMem = (unsigned int *)getmem(640*480*4);
	unsigned int *piPic = (unsigned int *)getmem(vesa.xres*vesa.yres*4);

	unsigned char cLow;
	unsigned char cHigh;

	unsigned int iR;
	unsigned int iG;
	unsigned int iB;

	unsigned short int siRGB16;
	unsigned int iRGB32;

	int iI;
	for (iI = 0; iI < 640*480; iI++)
	{
		cLow = s.getch();
		cHigh = s.getch();

		siRGB16 = (unsigned short int)cHigh;
		siRGB16 = siRGB16<<8;
		siRGB16 += cLow;

		iR = (unsigned int)siRGB16;
		iG = (unsigned int)siRGB16;
		iB = (unsigned int)siRGB16;

		iR = iR & 0xf800;
		iG = iG & 0x07E0;
		iB = iB & 0x001F;

		iR = iR<<8;
		iG = iG<<5;
		iB = iB<<3;

		iRGB32 = iR + iG + iB;

		piMem[iI] = iR + iG + iB;
	}


	Scale32ASM((char *)piMem, (char *)piPic, 640, 480,
		(int)vesa.xres, (int)vesa.yres);

	CopyBufferASM((char *)piPic, &vesa, coltab);

}




/*
void ShowPicNormal()
{
	unsigned int *dwMem = new unsigned int[640*480];
	FILE *pFile = fopen("karvel16.raw", "rb");

	unsigned int iR;
	unsigned int iG;
	unsigned int iB;

	unsigned short int siRGB16;
	unsigned int iRGB32;

	int iI;
	for (iI = 0; iI < 640*480; iI++)
	{
		fread(&siRGB16, sizeof(unsigned short int), 1, pFile);

		iR = (unsigned int)siRGB16;
		iG = (unsigned int)siRGB16;
		iB = (unsigned int)siRGB16;

		iR = iR & 0xf800;
		iG = iG & 0x07E0;
		iB = iB & 0x001F;

		iR = iR<<8;
		iG = iG<<5;
		iB = iB<<3;

		iRGB32 = iR + iG + iB;

		dwMem[iI] = iR + iG + iB;
	}

	CopyBuffer32PitchASM((char *)dwMem, &vesa);
}
*/

/*
void Convert()
{
	char *pcMem = new char[640*480*2];

	FILE *pSrcFile = fopen("karvel.raw", "rb");
	FILE *pDestFile = fopen("karvel16.raw", "wb");
	unsigned char cR;
	unsigned char cG;
	unsigned char cB;

	unsigned int siR;
	unsigned int siG;
	unsigned int siB;

	unsigned short int siRGB16;

	int iI;
	for (iI = 0; iI < 640*480; iI++)
	{
		fread(&cR,sizeof(unsigned char), 1, pSrcFile);
		fread(&cG,sizeof(unsigned char), 1, pSrcFile);
		fread(&cB,sizeof(unsigned char), 1, pSrcFile);

		siR = (unsigned short int)cR;
		siG = (unsigned short int)cG;
		siB = (unsigned short int)cB;

		siR = siR>>3;
		siG = siG>>2;
		siB = siB>>3;

		siR = siR<<11;
		siG = siG<<5;

		siRGB16 = siR + siG + siB;

		fwrite(&siRGB16, sizeof(unsigned short int), 1, pDestFile);
	}


	fclose(pSrcFile);
	fclose(pDestFile);
}

*/




