// Filename: include\m2render.h
//
// Developed by Oliver Belaifa in 1997
//

#ifndef _M2RENDER_H_INCLUDED
#define _M2RENDER_H_INCLUDED

#include<m2raw.h>
#include<m2camera.h>
#include<m2except.h>




class CMV2PolygonDistance
{
public:
	float m_fAverageDistance;
	CMV2Polygon *m_pPolygon;

	CMV2PolygonDistance();
	~CMV2PolygonDistance();

//	void *operator new(size_t);
//	void operator delete(void *);
};





class CMV2RenderBuffer
{
public:
	CMV2PolygonDistance *m_pPolygonDistanceBuffer1;
	CMV2PolygonDistance *m_pPolygonDistanceBuffer2;
	int m_iMaxPolygonDistances;
	int m_iCurNumPolygonDistances;

	void ClearBuffer();
	void AddPolygons(CMV2PolygonObject *pPolygonObject);
	void AddPolygons(CMV2Polygon **ppPolygons, int iNumPolygons);

	void SortThem();

	CMV2RenderBuffer();
	CMV2RenderBuffer(int iMaxPolygonDistances);
	~CMV2RenderBuffer();

	void *operator new(size_t);
	void operator delete(void *);
};





class CMV2Render
{
public:
	CMV2Polygon	  	  **m_ppPolygonList1;
	CMV2Polygon	  	  **m_ppPolygonList2;
	CMV2Polygon	  	  **m_ppSrcPolygonList;
	CMV2Polygon	  	  **m_ppDestPolygonList;
	int					m_iMaxNumPolygonList;
	int					m_iCurNumSrcPolygonList;
	int					m_iCurNumDestPolygonList;

	CMV2Polygon	  	  **m_ppCPolygonList1;
	CMV2Polygon	  	  **m_ppCPolygonList2;
	CMV2Polygon	  	  **m_ppSrcCPolygonList;
	CMV2Polygon	  	  **m_ppDestCPolygonList;
	int					m_iMaxNumCPolygonList;
	int					m_iCurNumSrcCPolygonList;
	int					m_iCurNumDestCPolygonList;

	CMV2Dot3DPos   	   *m_pCDot3DPos;
	int					m_iMaxNumCDot3DPos;
	int					m_iCurNumCDot3DPos;

	CMV2Dot3D	   	   *m_pCDot3D;
	int					m_iMaxNumCDot3D;
	int					m_iCurNumCDot3D;

	CMV2Polygon	   	   *m_pCPolygon;
	int					m_iMaxNumCPolygon;
	int					m_iCurNumCPolygon;

	CMV2RenderBuffer   *m_pRenderBuffer;

	float 				m_fCXmin, m_fCYmin, m_fCXmax, m_fCYmax;
	float				m_fXmax, m_fYmax;

	CMV2OmniLight  	  **m_ppOmniLights;
	int					m_iMaxNumOmniLights;
	int					m_iCurNumOmniLights;

	void SetClipRanges(int iCXmin, int iCYmin, int iCXmax, int iCYmax,
		int iXmax, int iYmax);
	void Clear();
	void Clip2D();
	void Clip2D(float fCOx, float fCOy, float fCRx, float fCRy);
	void Clip2DZ(float fZ, float fZd);
	void AddPolygonObject(CMV2Camera *pCamera,
		CMV2PolygonObject *pPolygonObject);
//	void AddOmniLight(CMV2Camera *pCamera,
//		CMV2OmniLight *pOmniLight);

	void FromMESEAddPolygonObject(CMV2Camera *pCamera,
		CMV2PolygonObject *pPolygonObject);
//	void FromMESEAddOmniLight(CMV2Camera *pCamera,
//		CMV2OmniLight *pOmniLight);

	void DoRendering(char *pcBackBuffer, int iXmax);

	void CalcNorm(CMV2Dot3DPos *pDots, int iNumDots,
		CMV2Polygon *pPolygons, int iNumPolygons);

	void CalcRGB(CMV2Dot3DPos *pDots, int iNumDots);
	void CalcPhongAngles(CMV2Dot3DPos *pDots, int iNumDots);

	void Init();
	void Free();

	CMV2Render();
	~CMV2Render();

	void *operator new(size_t);
	void operator delete(void *);
};





#endif

