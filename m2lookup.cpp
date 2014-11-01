// Filename: m2lookup.cpp
//
// Developed by Oliver Belaifa in 1997
//

//#include<copro.h>
#include <math.h>
#include<demo.h>

extern "C"
{
	unsigned char pShadeLookup[65536];
	unsigned char pBumpShadeLookup[65536];
	unsigned char pAngleJoinTab[65536];
	unsigned int  pLightBrightnessTab[256];
}

const int iNumSplitAngles = 256;


void CalcShadeLookup()
{
	int iBrightness;
	int iCol;
	const int iHighlite = 256;

	for (iBrightness = 0; iBrightness < 256; iBrightness++)
		for (iCol = 0; iCol < 256; iCol++)
		{
			float fA = (iBrightness*iCol)/255.0;

			if (iBrightness > iHighlite)
			{
				fA += 0.3*255.0/(255.0 - iHighlite)*float(iBrightness - iHighlite);
				if (fA > 255.0) fA = 255.0;
			}

			unsigned char cA = (unsigned char)fA;
			pShadeLookup[256*iBrightness + iCol] = cA;
		}
}




void CalcBumpShadeLookup()
{
	int iBrightness;
	int iCol;
	const int iHighlite = 160;

	for (iBrightness = 0; iBrightness < 256; iBrightness++)
		for (iCol = 0; iCol < 256; iCol++)
		{
//			float fBrightness = (float)iBrightness;

			int iBrightness2 = iBrightness;
			if (iBrightness2 < 100)
				iBrightness2 = 100;

			float fA = (iBrightness2*iCol)/255.0;

			if (iBrightness2 > iHighlite)
			{
				fA += 0.3*255.0/(255.0 - iHighlite)*float(iBrightness2 - iHighlite);
				if (fA > 255.0) fA = 255.0;
			}

			unsigned char cA = (unsigned char)fA;
			pBumpShadeLookup[256*iBrightness + iCol] = cA;
		}
}





void CalcAngleJoinTab()
{
	int iY;
	int iX;
	for (iY = 0; iY < 256; iY++)
		for (iX = 0; iX < 256; iX++)
		{
			int iAngleU = iX - 128;
			int iAngleV = iY - 128;

			float fAngleU = (float)iAngleU;
			float fAngleV = (float)iAngleV;

			float fAbs = 300.0 - 2.5*sqrtf(fAngleU*fAngleU + fAngleV*fAngleV);
			if (fAbs > 255.0)
				fAbs = 255.0;
			if (fAbs < 0)
				fAbs = 0;

			unsigned char cAbs = (unsigned char)fAbs;

			pAngleJoinTab[iY*256 + iX] = cAbs;
		}
}



void CalcLightBrightnessTab(int iRGB, float fBFactor)
// BFactor Should be 0 <= fBFactor <= 1.0(Original)

{
	int iR = (iRGB&0xff0000)>>16;
	int iG = (iRGB&0x00ff00)>>8;
	int iB = (iRGB&0x0000ff);

	float fR = (float)iR;
	float fG = (float)iG;
	float fB = (float)iB;

	float fDelta = fBFactor/255.0;
	float fCurFactor = 0;

	int iI;
	for (iI = 0; iI < 256; iI++)
	{
		float fFactor;
		if (fCurFactor < 0.1)
			fFactor = 0.1;
		else
			fFactor = fCurFactor;

		float fCurR = fFactor*fR;
		float fCurG = fFactor*fG;
		float fCurB = fFactor*fB;

		int iCurR = (int)fCurR;
		int iCurG = (int)fCurG;
		int iCurB = (int)fCurB;

		if (iCurR > 255) iCurR = 255;
		if (iCurG > 255) iCurG = 255;
		if (iCurB > 255) iCurB = 255;

		int iCurRGB = iCurR*65536 + iCurG*256 + iCurB;

		pLightBrightnessTab[iI] = iCurRGB;

		fCurFactor += fDelta;
	}
}




void FromMeseCalcLightBrightnessTab(float *fRGB, float fBFactor)
// BFactor Should be 0 <= fBFactor <= 1.0(Original)

{
	float fR = fRGB[0]*256.0;
	float fG = fRGB[1]*256.0;
	float fB = fRGB[2]*256.0;

	float fDelta = fBFactor/255.0;
	float fCurFactor = 0;

	int iI;
	for (iI = 0; iI < 256; iI++)
	{
		float fFactor;
		if (fCurFactor < 0.1)
			fFactor = 0.1;
		else
			fFactor = fCurFactor;

		float fCurR = fFactor*fR;
		float fCurG = fFactor*fG;
		float fCurB = fFactor*fB;

		int iCurR = (int)fCurR;
		int iCurG = (int)fCurG;
		int iCurB = (int)fCurB;

		if (iCurR > 255) iCurR = 255;
		if (iCurG > 255) iCurG = 255;
		if (iCurB > 255) iCurB = 255;

		int iCurRGB = iCurR*65536 + iCurG*256 + iCurB;

		pLightBrightnessTab[iI] = iCurRGB;

		fCurFactor += fDelta;
	}
}




void CalcLookUpTables()
{
	CalcShadeLookup();
	CalcBumpShadeLookup();
	CalcAngleJoinTab();
	CalcLightBrightnessTab(0xffffff, 1.0);
}

