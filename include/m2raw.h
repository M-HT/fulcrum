// Filename: include\m2raw.h
//
// Developed by Oliver Belaifa in 1997
//

#ifndef _M2RAW_H_INCLUDED
#define _M2RAW_H_INCLUDED

#include<stypes.h>
#include<m2vector.h>


class CMV2Dot3DPos
{
public:
	CVector3D       m_Pos;
	CVector3D		m_TransfPos;
	float           m_fZNewRZP;
	int             m_iScreenX, m_iScreenY;
	float           m_fScreenX, m_fScreenY;
	float           m_fScreenYError;
	int             m_iClipFlag;
	float			m_fR;	// = AngleN
	float			m_fG;	// = AngleU
	float			m_fB;	// = AngleV
	float			m_fAmbientR;
	float			m_fAmbientG;
	float			m_fAmbientB;
	CVector3D       m_Norm;
	CVector3D		m_AngleU;
	CVector3D		m_AngleV;
//	float			m_fAngleN;
//	float			m_fAngleU;
//	float			m_fAngleV;

	CMV2Dot3DPos();
	CMV2Dot3DPos(float fX, float fY, float fZ);

	~CMV2Dot3DPos();

//	void *operator new(size_t);
//	void operator delete(void *);
};



class CMV2Dot3D
{
public:
	CMV2Dot3DPos   *m_pPos;
	float           m_fTextureU, m_fTextureV;

	CMV2Dot3D();
	CMV2Dot3D(float fX, float fY, float fZ,
			float fTextureX, float fTextureY);
	CMV2Dot3D(float fX, float fY, float fZ,
			float fTextureX, float fTextureY,
			float fNormX, float fNormY, float fNormZ);

	~CMV2Dot3D();

//	void *operator new(size_t);
//	void operator delete(void *);
};





class CMV2Polygon
{
public:
	CMV2Dot3D  	   *m_pDot1, *m_pDot2, *m_pDot3;
	char       	   *m_pcTexture;
	int				m_iType;
	int				m_iFlag;
//***
	char		   *m_pcBumpmap;
	float			m_fAddDistance;

	CMV2Polygon();
	~CMV2Polygon();

//	void *operator new(size_t);
//	void operator delete(void *);
};




class CMV2OmniLight
{
public:
	CVector3D       m_Pos;
	CVector3D       m_TransfPos;
	float 			m_fR, m_fG, m_fB;
	float			m_fStart, m_fEnd;

	CMV2OmniLight();
	~CMV2OmniLight();

	void *operator new(size_t);
	void operator delete(void *);
};





class CLWObject;

class CMV2PolygonObject
{
public:
	CMV2Polygon	   *m_pPolygons;
	CMV2Polygon   **m_ppPolygons;
	int             m_iNumPolygons;

	CMV2Dot3DPos   *m_pDot3DPos;
	CMV2Dot3DPos  **m_ppDot3DPos;
	int             m_iNumDot3DPos;

	CMV2Dot3D      *m_pDot3D;
	int             m_iNumDot3D;

	int				m_iFlagNormCalc;

	void UpdateNorms();
	void UpdateBumpParameters();
	void GetLWOB(CLWObject *pLWObject);

	CMV2PolygonObject();
	~CMV2PolygonObject();

	void *operator new(size_t);
	void operator delete(void *);
};





class CLWDot
{
public:
	CVector3D m_Pos;
	ULONG m_lID;

	CLWDot();
	CLWDot(float fX, float fY, float fZ);
	~CLWDot();
};



class CLWPolygon
{
public:
	CLWDot    	   *m_ppLWDots[3];

	CLWPolygon();
	~CLWPolygon();
};



class CLWObject
{
public:
	CLWDot     	   *m_pLWDots;
	ULONG           m_lNumLWDots;
	CLWPolygon 	   *m_pLWPolygons;
	ULONG           m_lNumLWPolygons;

	char       	   *m_pcTextureFileName;

	long            m_lFlagXAxis;   // TFLG flags for TextureMapping
	long            m_lFlagYAxis;
	long            m_lFlagZAxis;

	float           m_fXSize;               // TSIZ XSize
	float           m_fYSize;       // TSIZ YSize
	float           m_fZSize;       // TSIZ TSize

	float           m_fXCenter;             // TCTR XCenter
	float           m_fYCenter;             // TCTR YCenter
	float           m_fZCenter;     // TCTR ZCenter

	void LoadLWOB(char *pcFileName);
	void LoadLWOBPoints(char *pcFileName);
	float GetFloat(unsigned char *pcMem);
	ULONG GetUlong(unsigned char *pcMem);
	UWORD GetUShortInt(unsigned char *pcMem);
	char *GetHunkPtr(char *pcHunk, char *pcStart, char *pcEnd);

	char *SetTIMG(char *pcCurScanPTR);
	char *SetTFLG(char *pcCurScanPTR);
	char *SetTSIZ(char *pcCurScanPTR);
	char *SetTCTR(char *pcCurScanPTR);

	void MapPlanar(CMV2Dot3D *pDot);

	CLWObject();
	~CLWObject();
};




#endif

