// Filename: m2world.cpp
//
// Developed by Oliver Belaifa in 1997
//

#include<stdlib.h>
#include<m2world.h>






void CMV2World::Init(int iXmax, int iYmax, char *pcBackBuffer)
{
	m_pWorldCamera = new CMV2Camera();
	m_pWorldCamera->SetScreenParameters(iXmax, iYmax, 0, 0,
		iXmax, iYmax);

	m_pcBackBuffer = pcBackBuffer;

	m_iXmax = iXmax;
	m_iYmax = iYmax;

	m_pTimeCounter = new CTimeCounter();
	m_pTimeCounter->Init();

	m_pGfxStrIO = new CGfxStrIO();
	m_pGfxStrIO->Init(iXmax, iYmax);
	m_pGfxStrIO->TakeBackBuffer((void *)m_pcBackBuffer);
}



void CMV2World::Free()
{
   /*
	if (m_pWorldCamera) delete m_pWorldCamera, m_pWorldCamera = NULL;
	if (m_pcBackBuffer) delete m_pcBackBuffer, m_pcBackBuffer = NULL;

	m_iXmax = 0;
	m_iYmax = 0;

	m_pTimeCounter->Free();
	if (m_pTimeCounter) delete m_pTimeCounter, m_pTimeCounter = NULL;

	m_pGfxStrIO->Free();
	if (m_pGfxStrIO) delete m_pGfxStrIO, m_pGfxStrIO = NULL;
  */
}



void CMV2World::RenderScene()
{

}



CMV2World::CMV2World()
{
	m_pcBackBuffer = NULL;
	m_pWorldCamera = NULL;

	m_iXmax = 0;
	m_iYmax = 0;

	m_pTimeCounter = NULL;
	m_pGfxStrIO = NULL;

}



CMV2World::~CMV2World()
{
}



void *CMV2World::operator new(size_t s) {
  return getmem(s);
}

void CMV2World::operator delete(void *p) {

}
