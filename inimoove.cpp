// Filename: main.cpp
//
// Developed by Oliver Belaifa in 1997
//

#include<stdio.h>
#include<stdlib.h>
#include<conio.h>

#include<2dlink.h>

#include<timecntr.h>

#include<gmvesa.h>
#include<m2camera.h>
#include<m2world.h>

#include<m2raw.h>
#include<m2render.h>

#include<copro.h>
#include<demo.h>

#include<2dlink.h>

extern "C"
{
void InitZeroMem(char *pMem, long lNum);

void SetPixelMem(int iX, int iY, char *pcMem, int iXmax, int iCol);

void TransformTo32Bit(char *pcSrc, char *cpDest, char *pcPalette,
  int iNumPixels);

void MV2DrawPolygonsASM(CMV2PolygonDistance *pPolygonDistance,
  int iNumPolygons, char *pcBackBuffer, int iXmax);

void MV2DrawLinearPolygonsASM(CMV2PolygonDistance *pPolygonDistance,
  int iNumPolygons, char *pcBackBuffer, int iXmax);

void Scale32ASM(char *pcSrc, char *pcDest, int iSrcXmax, int iSrcYmax,
	int iDestXmax, int iDestYmax);

void Scale8ASM(char *pcSrc, char *pcDest, int iSrcXmax, int iSrcYmax,
	int iDestXmax, int iDestYmax);

void Draw2DFadeASM(char *pBackBuffer1, char *pBackBuffer2,
	char *pDestBack, char *pFade, int dwFadeFactor,
	int dwNum);

void Init2DFadeJumpTableASM();

void CopyBufferASM(char *pSrcScreenBuffer, tvesa *pVesa, void *);
#pragma aux CopyBufferASM "*" parm [esi] [ebx] [edx] modify [eax ebx ecx edx esi edi]

void makecoltab16(tvesa &, void *);
#pragma aux makecoltab16 "*" parm [esi] [edi] modify [eax ebx edx esi edi]
void makecoltab32(tvesa &, void *);
#pragma aux makecoltab32 "*" parm [esi] [edi] modify [eax ebx edx esi edi]

extern void ShutDownMV2();
}



extern void InitPlayer(tstream &);
extern void WaitVR();



// extern avaiable {

CGfxModeVesa	 *pVesa;
CGfxStrIO	 *pGfxStrIO;
CMV2World	 *pWorld;

C2DLinkEffect  LinkEffect;
CFire	       Fire;
CMV2PBernoulli PBernoulli;
C2DEye	       EyeEffect;

C2DBeamFade    BeamFade;


CMV2Render *pRender;

tvesa *pMoove2Vesa;
void *coltab;
// }


char *pcBackBuffer1; // 32bit
char *pcBackBuffer2; // 32bit
char *pcDestBackBuffer; // 32bit

char *pcFade; // 8bit
char *pcEye;  // 32bit
char *pcMask; // 8bit








void InitMV2(tvesa &vesa)
{
	pMoove2Vesa = &vesa;

	pWorld = new CMV2World();
	pWorld->Init(pMoove2Vesa->xres, pMoove2Vesa->yres, pcDestBackBuffer);

	pGfxStrIO = pWorld->m_pGfxStrIO;

	pRender = new CMV2Render;
	pRender->Init();
	pRender->SetClipRanges(
		pWorld->m_pWorldCamera->m_iClipXmin,
		pWorld->m_pWorldCamera->m_iClipYmin,
		pWorld->m_pWorldCamera->m_iClipXmax,
		pWorld->m_pWorldCamera->m_iClipYmax,
		pWorld->m_pWorldCamera->m_iXmax,
		pWorld->m_pWorldCamera->m_iYmax);





}


void ShutDownMV2()
{
	pWorld->Free();
	delete pWorld;

//	pVesa->VesaOff();
//	delete pVesa;
}

// }






// Profiling {

extern int iAddPolys;
extern int iVSpacePolys;
extern int iToBeClipped;
extern int iDrawedPolys;

extern "C" int iPixelCounter;

// }







CMV2Dot3DPos *pDot3DPos;
CMV2Dot3DPos **ppDot3DPos;
CMV2Dot3D *pDot3D;
CMV2Polygon *pPolygon;
CMV2Polygon **ppPolygon;
char *pcTexture;
CMV2PolygonObject *pPolygonObject;
CMV2OmniLight OmniLight;
CMV2Camera Camera2;




void WaitVR()
{
  while (inp(0x3da) & 8) ;
  while (!(inp(0x3da) & 8)) ;
}



//***********************
//***********************
//***********************
//2D Effects {
//***********************
//***********************
//***********************
/*
char *pcBackBuffer1;
char *pcBackBuffer2;
char *pcDestBackBuffer;
C2DLinkEffect LinkEffect;

CTimeCounter TimeCounter;
CGfxStrIO GfxStrIO;


static void Init2DEffect(tvesa *vesa)
{
    pMoove2Vesa = vesa;

    pcBackBuffer1 = new char[320*200*4];
    pcBackBuffer2 = new char[320*200*4];
    pcDestBackBuffer = new char[320*200*4];

// pic1 load {
    CImage *pImage = new CImage();
    pImage->LoadPCX("fulcrum.pcx");
    TransformTo32Bit(pImage->m_pcData, pcBackBuffer1,
	pImage->m_pcPalette, 64000);
    delete pImage;
// }

// pic1 load {
    pImage = new CImage();
    pImage->LoadPCX("main.pcx");
    TransformTo32Bit(pImage->m_pcData, pcBackBuffer2,
	pImage->m_pcPalette, 64000);
    delete pImage;
// }

    LinkEffect.Init();

	TimeCounter.Init();
    GfxStrIO.Init(320, 200);
	GfxStrIO.TakeBackBuffer(pcDestBackBuffer);
}



static void ShutDown2DEffect()
{
}



void Simulate2DEffect()
{
    char cCh = 0;
    while (cCh != 27)
    {
	if (kbhit())
	{
	    cCh = getch ();
	}


	TimeCounter.StartTimeCounter();
	LinkEffect.Draw(pcBackBuffer2, pcBackBuffer1, pcDestBackBuffer);
	TimeCounter.StopTimeCounter();
	float fFps = TimeCounter.GetFPS();

	GfxStrIO.ResetPos();

	char pcText[80];
	sprintf(pcText, "fps: %.2f", fFps);
	GfxStrIO.WriteStr(pcText);

	WaitVR();
	CopyBuffer32PitchASM(pcDestBackBuffer, pMoove2Vesa);
    }
}

*/




//***********************
//***********************
//***********************
//2D Effects }
//***********************
//***********************
//***********************


void initcoltab(tvesa &vesa) {

  if (vesa.redbits == 8 && vesa.greenbits == 8 && vesa.bluebits == 8 &&
	vesa.redpos == 16 && vesa.greenpos == 8 && vesa.bluepos == 0) {
	//direct mode
	coltab = NULL;
  } else {
	//convert mode
	if (vesa.pbytes <= 2) {
	  //15, 16 bit
	  coltab = getmem(3*256*2);
	  makecoltab16(vesa,coltab);
	} else {
	  //24, 32 bit
	  coltab = getmem(3*256*4);
	  makecoltab32(vesa,coltab);
	}
  }
}


void loadpic(tstream &s, tvesa &vesa, char *buf) {
  char pal[768];
  int xres, yres;

  s.read(pal,768);
  xres = s.getint();
  yres = s.getint();

  char *pic = (char *)gettempmem(xres*yres);

  s.read(pic,xres*yres);

  Scale8ASM(pic, pcDestBackBuffer, xres, yres, vesa.xres, vesa.yres);
  TransformTo32Bit(pcDestBackBuffer, buf, pal, vesa.xres*vesa.yres);
}

void loadmask(tstream &s, tvesa &vesa, char *buf) {
  int xres, yres;
  char *pic;

  //auge (ohne palette)
  xres = s.getint();
  yres = s.getint();

  pic = (char *) gettempmem(xres*yres);
  s.read(pic,xres*yres);

  Scale8ASM(pic, buf, xres, yres, vesa.xres, vesa.yres);
}

void initmoove(tstream &s, tvesa &vesa) {

  finit(0x127F); //I=affine, rc=nearest, pc=double

  int iNumPixels = vesa.xres*vesa.yres;

  pcBackBuffer1 = (char *)getmem(4*iNumPixels);
  pcBackBuffer2 = (char *)getmem(4*iNumPixels);
  pcDestBackBuffer = (char *)getmem(4*iNumPixels);
  pcFade = (char *)getmem(iNumPixels);
  pcEye  = (char *)getmem(4*iNumPixels);
  pcMask = (char *)getmem(iNumPixels);



  initcoltab(vesa); //fr bit-tiefenunabh„ngige kopierroutinen

  s.openfile("2dfx.bin");
  //startpic
  loadpic(s, vesa, pcBackBuffer1);
  CopyBufferASM(pcBackBuffer1, &vesa, coltab);

  //start-link
  LinkEffect.Init(vesa.xres, vesa.yres);

  //partikel
  PBernoulli.Init(s,vesa.xres, vesa.yres);

  //fischauge
  EyeEffect.Init(vesa.xres, vesa.yres);
  loadpic(s, vesa, pcEye);
  loadmask(s, vesa, pcMask);

  //crossfade
  loadmask(s, vesa, pcFade);
  Init2DFadeJumpTableASM();

  //monster beam
  BeamFade.Init(vesa.xres, vesa.yres);

  //fire
  Fire.Init(s, 128,128);

  InitPlayer(s);
  InitMV2(vesa);
}
