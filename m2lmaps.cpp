// Filename: m2lmaps.cpp
//
// Developed by Oliver Belaifa in 1998
//

extern "C"
{
		void InitZeroMem(char *pMem, long lNum);
		void SetPixelMem(int iX, int iY, char *pcMem, int iXmax, int iCol);
}

extern CGfxModeVesa               *pVesa;
extern CGfxStrIO                  *pGfxStrIO;
extern CMV2World                  *pWorld;



// Class beschreibung
/*
{
	Polygons
	{
		pointer auf surface
		pointer auf 3 dots
		surface beschreibung (id) wegen der performance
	}

	surface
	{
		suface beschreibung (id) (bumpmapping, radiosity, gouraud, filtering)
		4 pointer auf 4 mipmapstufen
		wenn mipmapstufen pointer != null, schon vorberechnet
		surface cache pointer global bekannt
	}
}
*/







class CLightTrigDot
{
public:
	float m_fU, m_fV, m_fUError;
	float m_fR, m_fG, m_fB;

	CLightTrigDot();
	~CLightTrigDot();
};



class CLightTrig
{
public:
	CLightTrigDot *m_pDot1, m_pDot2, m_pDot3;
		// Dots are sorted (m_pDot1 is the highest)
	int	m_iPolygonType;

	CLightTrig();
	~CLightTrig();
};



class CLightMap
{
public:
	CLightTrig *m_pLightTrigs;
	int	m_iNumLightTrigs;

	float m_fU1, m_fV1, m_fU2, m_fV2, m_fU3, m_fV3;

public:
	void SetEdges(float fU1, float fV1, float fU2, float fV2,
		float fU3, float fV3);
private:
	float GiveVError(float fNum);
	float GetIntersection(float fL0X, float fL1X,
		float fG0X, float fG1X);
		// fL0X + result*fL1X = IntersectionPoint

	void DrawLine();

	CLightMaps();
	~CLightMaps();
};



void CLightMap::DrawLine(float fU1, float fV1, float fU2, float fV2,
		float fU3, float fV3)
{
	m_fU1 = fU1;
	m_fV1 = fV1;
	m_fU2 = fU2;
	m_fV2 = fV2;
	m_fU3 = fU3;
	m_fV3 = fV3;

	InitZeroMem(pWorld->m_pcBackBuffer, pWorld->m_iXmax*pWorld->m_iYmax*4);

	float m_fU1Error =






	pVesa->CopyBufferToScreen(pWorld->m_pcBackBuffer);
}



float CLightMap::GiveUError(float fU)
{
	if (fU >= 0)
		return float(1 + long(fU)) - fU;
	else
		return float(long(fU)) - fU;
}



float CLightMap::GetIntersection(float fL0X, float fL1X,
		float fG0X, float fG1X)
{
	 return fG1Y*(fL0X - fG0X) - fG1X*(fL0Y - fG0Y)/
		(fG1Y*fL1X - fG1X*fL1Y);
}






void CLightMap::SetEdges(float fU1, float fV1, float fU2, float fV2,
		float fU3, float fV3)
{
	m_fU1 = fU1;
	m_fV1 = fV1;
	m_fU2 = fU2;
	m_fV2 = fV2;
	m_fU3 = fU3;
	m_fV3 = fV3;





}




