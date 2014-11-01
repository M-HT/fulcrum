#include<stdlib.h>
//#include<conio.h>
#include<stdio.h>
#include<demo.h>
#include<m2raw.h>
#include<m2camera.h>
#include<timecntr.h>
#include<m2world.h>
//#include<m2image.h>
#include<m2render.h>
#include<m2partic.h>
#include<2dlink.h>

#define framemul (1000/15)
#define xfade1 580//589
#define xfade2 1558
#define mbeamf 3069

const int ofMatrix	  = 1;
const int ofMesh	  = 2;
const int ofAbsolute  = 4;
const int mfMorph	  = 8;
const int mfRGBPreCalc  = 16;		// G
const int mfBumpPreCalc = 32;		// B
const int mfDistance    = 64;		// Z

const int numclasses  = 5;
const int idMesh	  = 0;
const int idCamera	  = 1;
const int idTarget	  = 2;
const int idOmni	  = 3;
const int idDummy	  = 4;



struct tvmt;

struct tobject {
  tvmt *vmt;

  int flags;

// hierarchie-system:
  tobject *owner;
  tobject *next;
  tobject *child;

// zustand:
  float position[3];
  float matrix[9];
  int hidden;

};

struct tmesh {
  tobject object;

  CMV2PolygonObject PolygonObject;

// postrack (3):
  void *postrack;

// rotationtrack (4):
  void *rottrack;

// scaletrack (3):
//  void *scaletrack;

// morphtrack (2*vertices*3):
  void *morphtrack;
//  void *nmorphtrack;

  void *rgbtrack;

// hidetrack:
  int *hidetrack;
  int hidekey;

};


struct tcamera {
  tobject object;

  void *target;

// postrack (3):
  void *postrack;

// FOVtrack (1):
  void *FOVtrack;
  float FOV;

// rolltrack (1):
  void *rolltrack;
  float roll;

};


struct ttarget {
  tobject object;

// postrack (3):
  void *postrack;
};

struct tomni {
  tobject object;

// postrack (3):
  void *postrack;

// coltrack (3):
  void *coltrack;
  float color[3];

// hidetrack:
  int *hidetrack;
  int hidekey;
};

struct tdummy {
  tobject object;

// special effect
  int special;

// position
  float x[3];

// hidetrack:
  int *hidetrack;
  int hidekey;

  int firstframe;
};




struct tmaterial {
  int flag;
  char *txtmap;
  char *bumpmap;
};


struct tbumpomni {
// coltrack (3):
  void *coltrack;
  float color[3];

// multiplier track (1)
  void *multtrack;
  float multiplier;

// hidetrack:
  int *hidetrack;
  int hidekey;
  int hidden;
};


//---VMT--------

struct tvmt {
  int size;
  void (*readdata)(tstream &, tobject *);
  char *firstobj;
  char *nextobj;
  void (*dotracks)(void);
  void (*insert)(tobject *);
};


void m_readM2(tstream &s, tobject *obj);
void c_readM2(tstream &s, tobject *obj);
void t_readM2(tstream &s, tobject *obj);
void i_readM2(tstream &s, tobject *obj);
void d_readM2(tstream &s, tobject *obj);

//from m2tracks
extern "C" {
void initframecount();
//#pragma aux initframecount "*" modify [eax]

void calcframecount(int);
//#pragma aux calcframecount "*" parm [eax] modify [eax]

void dotracks(tobject *);
//#pragma aux dotracks "*" parm [esi] modify [eax ebx ecx edx esi edi]

void calcbumpomni(tbumpomni *);
//#pragma aux calcbumpomni "*" parm [esi] modify [eax ebx ecx edx esi edi]

void doviewer(tcamera *, CMV2Camera *);
//#pragma aux doviewer "*" parm [esi] [edi] modify [eax ebx ecx edx esi edi]

void matrixmul(tobject *, CMV2Camera *, CMV2Camera *);
//#pragma aux matrixmul "*" parm [esi] [edx] [edi] modify [eax ebx ecx edx esi edi]


void m_dotracksM2(void);
void c_dotracksM2(void);
void t_dotracksM2(void);
void i_dotracksM2(void);
void d_dotracksM2(void);
};

void m_insert(tobject *);
void c_insert(tobject *);
void t_insert(tobject *);
void i_insert(tobject *);
void d_insert(tobject *);


static tvmt vmtlist[] = {
   {sizeof(tmesh)  ,m_readM2,NULL,NULL,m_dotracksM2, m_insert},
   {sizeof(tcamera),c_readM2,NULL,NULL,c_dotracksM2, c_insert},
   {sizeof(ttarget),t_readM2,NULL,NULL,t_dotracksM2, t_insert},
   {sizeof(tomni)  ,i_readM2,NULL,NULL,i_dotracksM2, i_insert},
   {sizeof(tdummy) ,d_readM2,NULL,NULL,d_dotracksM2, d_insert}
};



//global
tmaterial *matlist;
char *txtdata;
tobject *root;
tbumpomni *bumpomni;
char *firemap;


// Moove2 {

extern tvesa *pMoove2Vesa;
extern void *coltab;

extern char *pcBackBuffer1; // 32bit
extern char *pcBackBuffer2; // 32bit
extern char *pcDestBackBuffer; // 32bit

extern char *pcFade; // 8bit
extern char *pcEye;  // 32bit
extern char *pcMask; // 8bit


extern "C" void CopyBufferASM(char *pSrcScreenBuffer, tvesa *pVesa, void *);
//#pragma aux CopyBufferASM "*" parm [esi] [ebx] [edx] modify [eax ebx ecx edx esi edi]

extern void WaitVR();

extern void FromMeseCalcLightBrightnessTab(float *fRGB, float fBFactor);

extern "C" void InitZeroMem(char *pMem, long lNum);
//extern CGfxModeVesa	     *pVesa;
extern CGfxStrIO       *pGfxStrIO;
extern CMV2World       *pWorld;
extern void InitMV2();
extern CMV2Render *pRender;

extern "C" void Draw2DFadeASM(char *pBackBuffer1, char *pBackBuffer2,
		  char *pDestBack, char *pFade, int dwFadeFactor,
		  int dwNum);

extern C2DLinkEffect  LinkEffect;
extern CFire	      Fire;
extern CMV2PBernoulli PBernoulli;
extern C2DEye	      EyeEffect;

extern C2DBeamFade    BeamFade;


static CMV2Camera Camera;
static CMV2Camera Camera2;
// }

extern "C" int frame;

// Profiling {

extern int iAddPolys;
extern int iVSpacePolys;
extern int iToBeClipped;
extern int iDrawedPolys;
extern int iDrawPolysCounter;
extern float fDrawPolysFps;
extern "C" int iPixelCounter;
extern int iClippingCounter;
extern float fClippingFps;

// }

extern "C" void TransformToBumpmapASM(char *src, char *dest, int len);


tbumpomni *readbumpomni(tstream &s) {
  tbumpomni *bumpomni;

  bumpomni = (tbumpomni *) getclearmem(sizeof(tbumpomni));

  //color track
  bumpomni->coltrack = readtrack(s);
  //multiplier track
  bumpomni->multtrack = readtrack(s);
  //hide track
  bumpomni->hidetrack = readhtrack(s);

  return bumpomni;
}


tobject *readobj(tstream &s, tobject* owner, int toread) {
  tobject *next = NULL;
  tvmt *vmt;
  tobject *obj;
  int i;

  while (toread > 0) {
	//id
    vmt = &vmtlist[s.getint()];
    obj = (tobject *) vmt->nextobj;
    vmt->nextobj = &vmt->nextobj[vmt->size];
    obj->vmt = vmt;
    obj->owner = owner;
    obj->next = next;
    next = obj;

    //childs
    i = s.getint();

    //readdata
    vmt->readdata(s,obj);

    //read childs
    obj->child = readobj(s,obj,i);
    toread--;
  }
  return next;
};



void m_readM2(tstream &s, tobject *obj) {
  tmesh *mesh = (tmesh *) obj;
  int count, z;

  //flags
  obj->flags = s.getint() + ofMatrix + ofMesh;
//  int iShade = s.getint();

  //vertices	   (int)
  count = s.getint();	//z = vertices
  mesh->PolygonObject.m_iNumDot3DPos = count;
  mesh->PolygonObject.m_pDot3DPos = (CMV2Dot3DPos *) getmem(count*sizeof(CMV2Dot3DPos));//new CMV2Dot3DPos[count];
  mesh->PolygonObject.m_ppDot3DPos = (CMV2Dot3DPos **) getmem(count*sizeof(CMV2Dot3DPos *));//new CMV2Dot3DPos *[count];
  for (z = 0; z < count; z++)
	mesh->PolygonObject.m_ppDot3DPos[z] = &mesh->PolygonObject.m_pDot3DPos[z];



  if (!(obj->flags & mfMorph)) {       //if not mfMorph
    for (z = 0; z < count; z++)
      s.read(&mesh->PolygonObject.m_pDot3DPos[z].m_Pos, 3*sizeof(float));
  }


/*
  for (z = 0; z < count; z++)
  {
		int iRGBCol = 0;
		s.read(&iRGBCol, 3);

		CMV2Dot3DPos *pDotPos = &mesh->PolygonObject.m_pDot3DPos[z];

				pDotPos->m_fAmbientR = float((iRGBCol&0xff0000)>>8);
				pDotPos->m_fAmbientG = float((iRGBCol&0x00ff00));
				pDotPos->m_fAmbientB = float((iRGBCol&0x0000ff)<<8);

				pDotPos->m_fAmbientR = 65535;
				pDotPos->m_fAmbientG = 65535;
				pDotPos->m_fAmbientB = 65535;



				pDotPos->m_fR = pDotPos->m_fAmbientR;
		pDotPos->m_fG = pDotPos->m_fAmbientG;
		pDotPos->m_fB = pDotPos->m_fAmbientB;
  }
*/
//    s.read(&mesh->PolygonObject.m_pDot3DPos[z].m_iRGBCol, 3);

  if (obj->flags & (mfRGBPreCalc | mfBumpPreCalc)) {
	mesh->rgbtrack = readltrack(s, mesh->PolygonObject.m_iNumDot3DPos*3);
  }

  //faces	   (int)		   ;only for mem alloc
  count = s.getint();
  mesh->PolygonObject.m_pPolygons = (CMV2Polygon *) getmem(count*sizeof(CMV2Polygon));//new CMV2Polygon[count];
  mesh->PolygonObject.m_ppPolygons = (CMV2Polygon **) getmem(count*sizeof(CMV2Polygon *));//new CMV2Polygon *[count];

  for (z = 0; z < count; z++)
	mesh->PolygonObject.m_ppPolygons[z] = &mesh->PolygonObject.m_pPolygons[z];
  mesh->PolygonObject.m_pDot3D = (CMV2Dot3D *) getmem(3*count*sizeof(CMV2Dot3D));//new CMV2Dot3D[3*count];
  mesh->PolygonObject.m_iNumDot3D = 3*count;

  CMV2Polygon *pPolygon = mesh->PolygonObject.m_pPolygons;
  CMV2Dot3D *pDot3D = mesh->PolygonObject.m_pDot3D;
  CMV2Dot3DPos *pDot3DPos = mesh->PolygonObject.m_pDot3DPos;

  while ((count = s.getint())) { //number of material assigns
	tmaterial *mat;

	mat = &matlist[s.getint()]; //material
	int iJump = s.getint(); //jump table index

	int iCurNumPolygon = 0;
	for(z = 0; z < count; z++) {
	  int i;
	  for (i = 0; i < 3; i++)
	  {
		pDot3D[i].m_pPos = &pDot3DPos[s.getint()];
		pDot3D[i].m_fTextureU = s.getfloat();
		pDot3D[i].m_fTextureV = s.getfloat();
	  }

	  pPolygon->m_pDot1 = &pDot3D[0];
	  pPolygon->m_pDot2 = &pDot3D[1];
	  pPolygon->m_pDot3 = &pDot3D[2];

	  pPolygon->m_pcTexture = mat->txtmap;
	  pPolygon->m_pcBumpmap = mat->bumpmap;
	  pPolygon->m_iType = iJump;
	  pPolygon->m_iFlag = 0;

	  pDot3D += 3;
	  pPolygon++;
	}
  };

  mesh->PolygonObject.m_iNumPolygons = pPolygon -
	mesh->PolygonObject.m_pPolygons;

  int iNumPolygons = mesh->PolygonObject.m_iNumPolygons;
  pPolygon = mesh->PolygonObject.m_pPolygons;
  int iI;

  if (obj->flags & mfDistance)
  {
	for (iI = 0; iI < iNumPolygons; iI++)
		pPolygon[iI].m_fAddDistance = 20.0;
  }
  else
  {
	for (iI = 0; iI < iNumPolygons; iI++)
		pPolygon[iI].m_fAddDistance = 0.0;
  }

  if (!(obj->flags & ofAbsolute)) {
	//position track
	mesh->postrack = readtrack(s);
	//rotation track
	mesh->rottrack = readtrack(s);
	//scale track
//    mesh->scaletrack = readtrack(s);
	//morph track
//    if (obj->flags & mfMorph) mesh->morphtrack = readtrack(s);
  } else {
	//position track
	mesh->postrack = readltrack(s,3);
	//morph track
	if (obj->flags & mfMorph) {	  //vertices*3
	  mesh->morphtrack = readltrack(s,mesh->PolygonObject.m_iNumDot3DPos*3);
//	mesh->nmorphtrack = readltrack(s,mesh->PolygonObject.m_iNumDot3DPos*3);
	}
  }

  //hide track
  mesh->hidetrack = readhtrack(s);
}



void c_readM2(tstream &s, tobject *obj) {
  tcamera *cam = (tcamera *) obj;

  //target
  cam->target = &vmtlist[idTarget].firstobj[s.getint() * sizeof(ttarget)];
  //position track
  cam->postrack = readtrack(s);
  //FOV track
  cam->FOVtrack = readtrack(s);
  //roll track
  cam->rolltrack = readtrack(s);
};



void t_readM2(tstream &s, tobject *obj) {
  ttarget *target = (ttarget *) obj;

  //position track
  target->postrack = readtrack(s);
};



void i_readM2(tstream &s, tobject *obj) {
  tomni *omni = (tomni *) obj;
  int z;

  //position track
  omni->postrack = readtrack(s);
  //color track
  omni->coltrack = readtrack(s);
  //hide track
  omni->hidetrack = readhtrack(s);
}

void d_readM2(tstream &s, tobject *obj) {
  tdummy *dummy = (tdummy *) obj;
  int z;

  //special effect
  dummy->special = s.getint();
  //position
  s.read(dummy->x,12);

  //hide track
  dummy->hidetrack = readhtrack(s);

  //first visibility
  dummy->firstframe = (dummy->hidetrack[0] < 2) ? 0 : dummy->hidetrack[2];
}

void readmaterials(tstream &s) {
  int count, z;
  tmaterial *mat;
  char pal[768];
  char *txtdata;
  char *bumpdata;
  char *map;
  char *tempdata;
//  char *firemap;

  //texturemaps    (int)
  count = s.getint();
  //alloc mem for texturedata (align 256k)
 txtdata = (char *) getalignmem((count+1) << 18, 1 << 18);
 firemap = txtdata + (count << 18);
  //convert to true color
  tempdata = (char *) gettempmem(65536);
  map = txtdata;
  for(z = 0; z < count; z++) {
    int i, p;

    //palette	     (768 chars)
    s.read(&pal,768);
    //bitmap	     (65536 chars)
    s.read(tempdata,65536);

    for(i = 0; i < 65536; i++) {
      p = ((uint8_t *)tempdata)[i]*3;
      map[i*4+2] = pal[p];
      map[i*4+1] = pal[p+1];
      map[i*4+0] = pal[p+2];
    }
    map += 1 << 18;
  }


  //bumpmaps	   (int)
  count = s.getint();
  //alloc mem for bumpmapdata (align 128k)
  bumpdata = (char *) getalignmem(count << 17, 1 << 17);
  //convert to displacement map
  tempdata = (char *) gettempmem(65536);
  map = bumpdata;
  for(z = 0; z < count; z++) {

    //bitmap	     (65536 chars)
    s.read(tempdata,65536);
    TransformToBumpmapASM(tempdata,map,65536);

    map += 1 << 17;
  }


  // materials	    (int)		    ;number of materials
  count = s.getint();
  matlist = (tmaterial *) getmem(count*sizeof(tmaterial));//new tmaterial[count];
  // materialdata   (n materials)
  s.read(matlist,count*sizeof(tmaterial));

  //convert material pointers
  mat = matlist;
  for(z = 0; z < count; z++) {
    if (!mat->flag) {
      mat->txtmap = &txtdata[((int) mat->txtmap) << 18];
      mat->bumpmap = ((int) mat->bumpmap != -1) ?
	&bumpdata[((int) mat->bumpmap) << 17] : NULL;
    } else {
      mat->txtmap = firemap;
      mat->bumpmap = NULL;
    }
    mat++;
  }
}


//mesh
void m_insert(tobject *obj) {

  matrixmul(obj,&Camera,&Camera2);
  pRender->FromMESEAddPolygonObject(&Camera2, &((tmesh *)obj)->PolygonObject);
}


//camera
void c_insert(tobject *obj) {

}


//target
void t_insert(tobject *obj) {

}


//omni
void i_insert(tobject *obj) {

}

//dummy
void d_insert(tobject *obj) {
    tdummy *dummy = (tdummy *) obj;

    if (dummy->special == 0)
    {
       PBernoulli.m_Camera = Camera;
       PBernoulli.m_Camera.m_Pos -= CVector3D(dummy->x[0],
				    dummy->x[1], dummy->x[2]);
       PBernoulli.m_iToBeRendered = 1;
	   PBernoulli.m_iFrame = frame - dummy->firstframe;
    }
}


void traverse(tobject *obj) {

  while (obj) {

		if (!obj->hidden) obj->vmt->insert(obj);

		traverse(obj->child);
		obj = obj->next;
  };

};


void InitPlayer(tstream &s)
{
	int z;

	s.openfile("mainpart.mv2");

	//contents
	for(z = 0; z < numclasses; z++)
	{
	    vmtlist[z].firstobj = (char *) getclearmem(s.getint() * vmtlist[z].size);
	    vmtlist[z].nextobj = vmtlist[z].firstobj;
	};

	//materials
	readmaterials(s);

	//bump mapping omni
	bumpomni = readbumpomni(s);

	//objects
	root = readobj(s,NULL,s.getint());
}


void xfade(int xfstart, int xfend) {

  frame = xfstart;
  dotracks(root);
  doviewer((tcamera *) vmtlist[idCamera].firstobj, &Camera);
  pRender->Clear();
  traverse(root);
  pRender->DoRendering(pcBackBuffer1, pWorld->m_iXmax);
  if (PBernoulli.m_iToBeRendered) {
    PBernoulli.Draw(pcBackBuffer1);
  }

  frame = xfend;
  dotracks(root);
  doviewer((tcamera *) vmtlist[idCamera].firstobj, &Camera);
  pRender->Clear();
  traverse(root);
  pRender->DoRendering(pcBackBuffer2, pWorld->m_iXmax);

  while (!keypressed() && frame <= xfend) {
	calcframecount(0);
	Draw2DFadeASM(pcBackBuffer1, pcBackBuffer2, pcDestBackBuffer,
		  pcFade, -256 + (frame - xfstart)/5,
		  pMoove2Vesa->xres*pMoove2Vesa->yres);
    CopyBufferASM(pWorld->m_pcBackBuffer, pMoove2Vesa, coltab);
  }
}

void mbeam(int bstart, int brender, int bend) {

  frame = bstart;
  dotracks(root);
  doviewer((tcamera *) vmtlist[idCamera].firstobj, &Camera);
  pRender->Clear();
  traverse(root);
  pRender->DoRendering(pcBackBuffer1, pWorld->m_iXmax);

  frame = brender;
  dotracks(root);
  doviewer((tcamera *) vmtlist[idCamera].firstobj, &Camera);
  pRender->Clear();
  traverse(root);
  InitZeroMem(pcBackBuffer2, pWorld->m_iXmax*pWorld->m_iYmax*4);
  pRender->DoRendering(pcBackBuffer2, pWorld->m_iXmax);

  while (!keypressed() && frame <= bend) {
	calcframecount(0);

	BeamFade.Draw(frame - bstart, pcBackBuffer1, pcBackBuffer2,
		pcDestBackBuffer);

	CopyBufferASM(pcDestBackBuffer, pMoove2Vesa, coltab);
  }

}


void startmoove()
{
   int DirectRender;

   Fire.Draw(firemap,0);

   Camera = *pWorld->m_pWorldCamera;
   Camera2 = *pWorld->m_pWorldCamera;

   frame = 0;
   dotracks(root);
   doviewer((tcamera *) vmtlist[idCamera].firstobj, &Camera);
   pRender->Clear();
   traverse(root);
   pRender->DoRendering(pcBackBuffer2, pWorld->m_iXmax);

   initframecount();
   while (!keypressed() && frame < 7500) {
     calcframecount(0);
     LinkEffect.Draw(pcBackBuffer1, pcBackBuffer2, pcDestBackBuffer, frame);
     CopyBufferASM(pcDestBackBuffer, pMoove2Vesa, coltab);
   }

   calcframecount(7500);
//				   //3715
   while (!keypressed() && frame < 3630*framemul) {
       calcframecount(0);

       if (frame >= 138*framemul && frame < 300*framemul) {
	 Fire.Draw(firemap,frame);
       }

       if (frame >= xfade1*framemul && frame < xfade1*framemul+1500) {
	 xfade(xfade1*framemul,xfade1*framemul+1500);
	 if (keypressed()) goto weg;
       }
       if (frame >= xfade2*framemul && frame < xfade2*framemul+1500) {
	 xfade(xfade2*framemul,xfade2*framemul+1500);
	 if (keypressed()) goto weg;
       }

       if (frame >= mbeamf*framemul && frame < mbeamf*framemul+2000) {
	 mbeam(mbeamf*framemul,(mbeamf+10)*framemul,mbeamf*framemul+2000);
	 if (keypressed()) goto weg;
       }

       dotracks(root);
       calcbumpomni(bumpomni);
       if (!bumpomni->hidden) FromMeseCalcLightBrightnessTab(bumpomni->color,bumpomni->multiplier);

       doviewer((tcamera *) vmtlist[idCamera].firstobj, &Camera);

       //InitZeroMem(pWorld->m_pcBackBuffer, pWorld->m_iXmax*pWorld->m_iYmax*4);
       pWorld->m_pTimeCounter->StartTimeCounter();
       pRender->Clear();

//	 pRender->FromMESEAddOmniLight(&Camera, &OmniLight);
       traverse(root);

       DirectRender = 1;

	   //fischauge
       if (frame > 764*framemul && frame < 1037*framemul) {
	 pRender->DoRendering(pcBackBuffer1, pWorld->m_iXmax);
	 EyeEffect.Draw(pcEye, pcMask, pcBackBuffer1, pcDestBackBuffer);
	 DirectRender = 0;
       }


       if (DirectRender)
	 pRender->DoRendering(pcDestBackBuffer, pWorld->m_iXmax);

       //partikelrauch
       if (PBernoulli.m_iToBeRendered) {
	 PBernoulli.Draw(pWorld->m_pcBackBuffer);
       }

       pWorld->m_pTimeCounter->StopTimeCounter();
       float fFps = pWorld->m_pTimeCounter->GetFPS();
       pWorld->m_pGfxStrIO->ResetPos();
/*
       char pcText[80];
       sprintf(pcText, "fps, engine: %.2f, %.2f", fFps, fDrawPolysFps);
       pWorld->m_pGfxStrIO->WriteStr(pcText);
       sprintf(pcText, "AP, DP: %d, %d", iAddPolys, iDrawedPolys);
       pWorld->m_pGfxStrIO->WriteStr(pcText);
*/

/*
       sprintf(pcText, "AddPolys: %d", iAddPolys);
       pWorld->m_pGfxStrIO->WriteStr(pcText);
       sprintf(pcText, "VSpacePolys: %d", iVSpacePolys);
       pWorld->m_pGfxStrIO->WriteStr(pcText);
       sprintf(pcText, "ToBeClipped: %d", iToBeClipped);
       pWorld->m_pGfxStrIO->WriteStr(pcText);
       sprintf(pcText, "DrawedPolys: %d", iDrawedPolys);
       pWorld->m_pGfxStrIO->WriteStr(pcText);
       sprintf(pcText, "DrawedPixels: %d", iPixelCounter);
       pWorld->m_pGfxStrIO->WriteStr(pcText);
       sprintf(pcText, "ClippingTSC: %d", iClippingCounter);
       pWorld->m_pGfxStrIO->WriteStr(pcText);
       sprintf(pcText, "ClippingFps: %.2f", fClippingFps);
       pWorld->m_pGfxStrIO->WriteStr(pcText);
*/
/*

       float fPixelDrawedFactor = float(iPixelCounter)/
	       float(pWorld->m_iXmax*pWorld->m_iYmax);
       sprintf(pcText, "DrawedPixelFactor: %.2f ", fPixelDrawedFactor);

       int iFlag = 0;
       while (fPixelDrawedFactor > 1.6)
       {
	       if (fPixelDrawedFactor >= 2.4)
		       iFlag = 1;
	       strcat(pcText, "!");
	       fPixelDrawedFactor -= 0.2;
       }

       if (iFlag)
	       strcat(pcText, " Ugh...");

       pWorld->m_pGfxStrIO->WriteStr(pcText);
*/
//     WaitVR();
       CopyBufferASM(pWorld->m_pcBackBuffer, pMoove2Vesa, coltab);

   }
   weg:;
}

