// Filename: include\m2world.h
//
// Developed by Oliver Belaifa in 1997
//


#ifndef _M2WORLD_H_INCLUDED
#define _M2WORLD_H_INCLUDED


#include<m2camera.h>
#include<m2partic.h>
#include<timecntr.h>
#include<gfxstrio.h>




class CMV2World
{
public:
	char  		   	   *m_pcBackBuffer;
	CMV2Camera	   	   *m_pWorldCamera;

	int					m_iXmax, m_iYmax;

	CTimeCounter	   *m_pTimeCounter;
	CGfxStrIO		   *m_pGfxStrIO;

	void Init(int iXmax, int iYmax, char *pcBackBuffer);
	void Free();

	void RenderScene();

	CMV2World();
	~CMV2World();

	void *operator new(size_t);
	void operator delete(void *);

};






#endif
