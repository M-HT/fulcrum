// Filename: include\2dlink.h
//
// Developed by Oliver Belaifa in 1998
//


#ifndef _2DLINK_H_INCLUDED
#define _2DLINK_H_INCLUDED

#include<demo.h>
#include<m2vector.h>
#include<m2camera.h>
#include<m2raw.h>




class C2DLinkEffect
{
public:
	unsigned int   *m_piPolarTab;
	float	       *m_pfSinTab;
	float	       *m_pfCosTab; 	// sin->cos delta produce cachemisses
	float	       *m_pfDistFunc;

	int		m_iXmax;
	int		m_iYmax;
	float 		m_fResFactor;

	int 		m_iDistanceFuncRange;
	int 		m_iSinTabRange;
	float 		m_fSinTabRange;
	int 		m_iRange;
	int 		m_iPeriode;
	float 		m_fPeriode;

	void Init(int iXmax, int iYmax);
	void CalcPolarTab();
	void InitSinTab();
	void CalcDistanceFunc();
	void Draw(char *pcBackBuffer1, char *pcBackBuffer2,
		char *pcDestBackBuffer, int iFrame);

	C2DLinkEffect();
	~C2DLinkEffect();
};



class C2DEye
{
public:
	int			m_iXmax, m_iYmax;

	unsigned int   	       *m_piMoveListTab;
	float			m_fResFactor;

	void CalcMoveListTab();
	void Init(int iXmax, int iYmax);
	void Draw(char *pcPic, char *pcMask, char *pcSrcBuffer,
		char *pcDestBackBuffer);


	C2DEye();
	~C2DEye();
};




class C2DBeamFade
{
public:
	int		m_iXmax, m_iYmax;
	int	       *m_piSinTabX;
	int	       *m_piSinTabY;
	float 		m_fFX, m_fFY, m_fAX, m_fAY, m_fDX, m_fDY;
	float		m_fAlpha;

	void Init(int iXmax, int iYmax);
	void CalcSinTab(float fFreqX, float fFreqY,
		float fAmpX, float fAmpY, float fDeltaX, float fDeltaY);
	void Draw(int iFrame, char *pcBack1, char *pcBack2,
		char *pcDestB);



	C2DBeamFade();
	~C2DBeamFade();
};




class CFire
{
public:
	int		m_iXmax, m_iYmax;

	unsigned char  *m_pcAmpTab;
	unsigned char  *m_pcYAmpTab;

	void Init(tstream &s, int iXmax, int iYmax);
	void CalcYAmpTab(float fTime);
	void CalcAmpTab();
	void Draw(char *pcDestBack, int iFrame);

	CFire();
	~CFire();
};





#endif
