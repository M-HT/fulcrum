#include <stdio.h>
#include "demo.h"
#include "copro.h"


const lvalues     = 64;
const xtralvalues = 256-lvalues;//lvalues*2;
const fvalues     = 128;
const xtrafvalues = 128;

const zxres       = 64;

const fogmapsteps = 32;
const fogmapsize  = 16;

const tempsize    = 80000;//65536;


const sfTeilung   = 1;
const sf4punkt    = 4;
const sfBitmap    = 8;
const sfSidecube  = 16;

const ofMatrix    = 1;
const ofMesh      = 2;
const ofAbsolute  = 4;
//const ofCamera    = 65536;
const mfMorph     = 8;

const lfWater     = 1024;

const idMesh      = 0;
const idCamera    = 1;
const idSpotlight = 2;
const idTarget    = 3;
const idBound     = 4;



//---------------------------------------------

class tvec {
  protected:
    float x[3];

  public:
    tvec &operator=(const tvec &v) {
      for(int z = 0; z < 3; z++) x[z] = v.x[z];
      return *this;
    };

    tvec &operator-=(const tvec& v) {
      for(int z = 0; z < 3; z++) x[z] -= v.x[z];
      return *this;
    };

    tvec operator%(const tvec& v) const {
      tvec n;

      n.x[0] = x[1]*v.x[2]-x[2]*v.x[1];
      n.x[1] = x[2]*v.x[0]-x[0]*v.x[2];
      n.x[2] = x[0]*v.x[1]-x[1]*v.x[0];

      return n;
    };

    void operator=(const float *v) {
      for(int z = 0; z < 3; z++) x[z] = v[z];
    };

    friend void operator<<(float *, const tvec &);
};

void operator<<(float *u, const tvec &v) {
  for(int z = 0; z < 3; z++) u[z] = v.x[z];
};


//---------------------------------------------


const int s2p[6][4] = {  //side -> points
  {2,3,7,6},
  {0,4,7,3},
  {0,1,5,4},
  {1,2,6,5},
  {4,5,6,7},
  {0,3,2,1}};

struct tmaterial {
  char *mapptr;
  int mapand;
};


struct tvmt;
struct tcube;

struct tobject {
  tvmt *vmt;

  int flags;

// hierarchie-system:
  tobject *owner;
  tobject *next;
  tobject *child;

// descent-verfahren:
  tobject *nextincube;
  tcube *cube;

// zustand:
  float position[3];
  float matrix[9];
  int hidden;

  int stamp;
};


struct tfp {                            //face point
  int vertex;                           //number reference
  float u,v;                            //mapping
};

struct tface {
  tfp p[3];
  tmaterial *mat;
  int idx;
};


struct tmesh {
  tobject object;

  int faces;
  tface *facelist;
  int vertices;
  float *vertexlist;
//  float *normalslist;

// postrack (3):
  void *postrack;

// rotationtrack (4):
  void *rottrack;

// scaletrack (3):
//  void *scaletrack;

// morphtrack (2*vertices*3):
  void *morphtrack;
  void *nmorphtrack;

// hidetrack:
  int *hidetrack;
  int hidekey;

  float z;
  tmesh *nexttemp;                   //next mesh in cube mesh temp-list
  float *tlist;                      //list of transformed vertices

  float *vtlist;
  int vstamp;
};

struct tviewer {
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

struct tcamera {
  tviewer viewer;

// switchtrack:
  int *switchtrack;
  int switchkey;

};

struct tlight {
  tviewer viewer;

// hidetrack:
  int *hidetrack;
  int hidekey;

  void *lmap;
};


struct ttarget {
  tobject object;

// postrack (3):
  void *postrack;
};

struct tbound {
  tobject object;

// postrack (3):
  void *postrack;
};


//---CUBE--------

struct tmapping {
 float u;
 float v;
// float l;
};


struct tside {
  int flags;
  int mat;
  tmapping mapping[4];
  tcube *sidecube;
};



struct tplane {
  float p[3];
  float n[3];
  tcube *sidecube;
};

struct tcpoint {
  int p;                                //number reference to cube vertex
  tmapping m;
};

struct tcface {
  int points;
  tmaterial *mat;
  float p[3];
  float n[3];
};

struct tcube {
  int planes;
  tplane *planedata;
  int faces;
  void *facedata;

  float *vertex[8];
  tobject *objlist;
  int stamp;
  void *tc;
  int vstamp;
  void *vtc;
};


struct tidxentry {
  float zval;
  int idx;
};

//-----------
struct tpaltab {
  int pal[(lvalues+xtralvalues)*256];
};

struct tpaltab16 {
  tpaltab paltab;
  short r[(fvalues+xtrafvalues)*256];
  short g[(fvalues+xtrafvalues)*256];
  short b[(fvalues+xtrafvalues)*256];
};

struct tpaltab32 {
  tpaltab paltab;
  int r[(fvalues+xtrafvalues)*256];
  int g[(fvalues+xtrafvalues)*256];
  int b[(fvalues+xtrafvalues)*256];
};

struct tscene {
  void *pal;
  void *paltab;
  char *bitmapdata;
  tmaterial *materials;
  void *ambient;         //ambient track
  tobject *root;
  tcube *cubedata;
  tcamera *cam;
  char *firstlight;
  char *lastlight;


  char *tempdata; //tempcubes, transformed vertices, tempfaces
  void *datapos;
  void *ltab;
  void *ftab;
  void *buffer;   //screenbuffer (xres*yres*4)
  float *divtab;  //1/x, x = 1 to xres
  void *lmap;

  void *spot1;
  void *spot2;
  void *water;

  void *idxlist;
  void *idxlistend;
  void *zline;
  void *fogmap;
  void *fogwater;

  void *flares;
  void *flaretab;

  int intro;
};


//---VMT--------

struct tvmt {
  int size;
  void (*readdata)(tstream &, tobject *, int);
  char *firstobj;
  char *nextobj;
  void *dotracks; //(*dotracks)(void);
};

const numclasses = 5;

void m_readdata(tstream &s, tobject *obj, int);
void c_readdata(tstream &s, tobject *obj, int);
void l_readdata(tstream &s, tobject *obj, int);
void t_readdata(tstream &s, tobject *obj, int);
void b_readdata(tstream &s, tobject *obj, int);

extern "C" tscene scene;
#pragma aux scene "*"

extern "C" {
void initmdata(tvesa &, int);
#pragma aux initmdata "*" parm [esi] [eax] modify [eax ebx ecx edx esi edi]
void startmese(int);
#pragma aux startmese "*" parm [eax] modify [eax ebx ecx edx esi edi]

void m_dotracks(void);
#pragma aux m_dotracks "*"
void c_dotracks(void);
#pragma aux c_dotracks "*"
void l_dotracks(void);
#pragma aux l_dotracks "*"
void t_dotracks(void);
#pragma aux t_dotracks "*"
void b_dotracks(void);
#pragma aux b_dotracks "*"
};


static tvmt vmtlist[] = {
   {sizeof(tmesh)  ,m_readdata,NULL,NULL,m_dotracks},
   {sizeof(tcamera),c_readdata,NULL,NULL,c_dotracks},
   {sizeof(tlight) ,l_readdata,NULL,NULL,l_dotracks},
   {sizeof(ttarget),t_readdata,NULL,NULL,t_dotracks},
   {sizeof(tbound) ,b_readdata,NULL,NULL,b_dotracks}
};


//---GLOBAL-------
//tscene *scene;



void skiptrack(tstream &s) {
  s.skip(s.getint());
}

void skiphtrack(tstream &s) {
  s.skip(s.getint()*4);
}

tobject *readobj(tstream &s, tobject* owner, int toread, int htracknum) {
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

    //cube reference
    i = s.getint();
    if (i != -1) {
      obj->cube = &scene.cubedata[i];
      obj->nextincube = obj->cube->objlist;
      obj->cube->objlist = obj;
    } else obj->cube = NULL;

    //childs
    i = s.getint();
    //readdata
    vmt->readdata(s,obj,htracknum);

    //read childs
    obj->child = readobj(s,obj,i,htracknum);

    toread--;
  }
  return next;
};

void m_readdata(tstream &s, tobject *obj, int htracknum) {
  tmesh *mesh = (tmesh *) obj;
  int count, z;

  //flags
  obj->flags = s.getint() | ofMatrix | ofMesh;

  //faces/material
  mesh->facelist = (tface *) getmem(s.getint()*sizeof(tface));// new tface[s.getint()];
  while (count = s.getint()) {
    tmaterial *mat;

    mat = &scene.materials[s.getint()]; //material
    for(z = 0; z < count; z++) {
      s.read(mesh->facelist[mesh->faces].p,sizeof(tfp)*3);
      mesh->facelist[mesh->faces].mat = mat;
      mesh->faces++;
    };
  };

  //vertices
  z = s.getint();                       //z = vertices
  mesh->vertices = z;
  mesh->vertexlist = (float *) getmem(2*z*3*sizeof(float)); //new float[2*z*3];
  if (!(obj->flags & mfMorph)) {       //if not mfMorph
    //vertices & normals
    s.read(mesh->vertexlist,2*z*3*sizeof(float));
  };

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
    if (obj->flags & mfMorph) {
      mesh->morphtrack = readltrack(s,mesh->vertices*3);
      mesh->nmorphtrack = readltrack(s,mesh->vertices*3);
    };
  };

  //hide track
  if (htracknum) skiphtrack(s);
  z = s.getint();
  mesh->hidetrack = (int *) getmem((z+1)*sizeof(int)); //new int[z+1];
  mesh->hidetrack[0] = z;
  s.read(&mesh->hidetrack[1],z*sizeof(int));
  if (!htracknum) skiphtrack(s);

  for(z = 0; z < mesh->faces; z++) {
    mesh->facelist[z].p[0].vertex <<= 2;
    mesh->facelist[z].p[1].vertex <<= 2;
    mesh->facelist[z].p[2].vertex <<= 2;
  };
};

void v_readdata(tstream &s, tviewer &viewer) {

  //target
  viewer.target = &vmtlist[idTarget].firstobj[s.getint() * sizeof(ttarget)];
  //position track
  viewer.postrack = readtrack(s);
  //FOV track
  viewer.FOVtrack = readtrack(s);
  //roll track
  viewer.rolltrack = readtrack(s);
};

void c_readdata(tstream &s, tobject *obj, int htracknum) {
  tcamera *cam = (tcamera *) obj;
  int z;
  int *i;

  v_readdata(s,cam->viewer);

//  obj->flags |= ofCamera;

  //switch track
  z = s.getint();
  cam->switchtrack = (int *) getmem((z*2+1)*sizeof(int)); //new int[z*2+1];
  cam->switchtrack[0] = z;
  s.read(&cam->switchtrack[1],z*2*sizeof(int));

  i = &cam->switchtrack[1+1]; //change cube numbers to cube pointers
  while (z > 0) {
    *i = (int) &scene.cubedata[*i];
    i+=2;
    z--;
  }
};

void l_readdata(tstream &s, tobject *obj, int htracknum) {
  tlight *light = (tlight *) obj;
  int z;

  v_readdata(s,light->viewer);

  //hide track
  if (htracknum) skiphtrack(s);
  z = s.getint();
  light->hidetrack = (int *) getmem((z+1)*sizeof(int)); //new int[z+1];
  light->hidetrack[0] = z;
  s.read(&light->hidetrack[1],z*sizeof(int));
  if (!htracknum) skiphtrack(s);

  //flags
  obj->flags |= s.getint();

  //map
  z = s.getint();
  light->lmap = scene.spot1;
  if (z == 1) light->lmap = scene.spot2;
  if (obj->flags & lfWater) light->lmap = scene.water;
};


void t_readdata(tstream &s, tobject *obj, int htracknum) {
  ttarget *target = (ttarget *) obj;

  //position track
  target->postrack = readtrack(s);
};

void b_readdata(tstream &s, tobject *obj, int htracknum) {
  tbound *bound = (tbound *) obj;

  //flags
  obj->flags = s.getint();

  //position track
  if (obj->flags & ofAbsolute) bound->postrack = readltrack(s,3);
    else bound->postrack = readtrack(s);
};



void readlevel(tstream &s) {
  float *vertexdata;
  int count, cz, z;
  tcube *cube;

  tside side[6];
  int flags;

  tplane *plane;

  int facesize;
  void *facedata;
  tcface *face;
  tcpoint *point;

  int skip, i, ez;
  int e[4];
  tvec a,b,c;


  count = s.getint();
  vertexdata = (float *) getmem(count*sizeof(float)); //new float[count];
  s.read(vertexdata,count*sizeof(float));

  count = s.getint();
  cube = scene.cubedata;
  for(cz = 0; cz < count; cz++) {

    //sides
    facesize = 0;
    for(z = 0; z < 6; z++) {

      side[z].flags = flags = s.getint();
      //bitmap
      if (flags & sfBitmap) {
        side[z].mat = s.getint();
        s.read(side[z].mapping,4*sizeof(tmapping));

        cube->faces++;
        facesize += sizeof(tcface) + 3*sizeof(tcpoint);
        if (flags & sf4punkt) facesize += sizeof(tcpoint); else {
          cube->faces++;
          facesize += sizeof(tcface) + 3*sizeof(tcpoint);
        };

      };
      //sidecube
      if (flags & sfSidecube) {
        side[z].sidecube = &scene.cubedata[s.getint()];
        cube->planes ++;
        if (!(flags & sf4punkt)) cube->planes++;
      };
    };

    //vertex references
    for(z = 0; z < 8; z++) cube->vertex[z] = &vertexdata[s.getint()];


    //side normals
    plane = (tplane *) getmem(cube->planes*sizeof(tplane)); //new tplane[cube->planes];
    cube->planedata = plane;
    facedata = (char *) getmem(facesize); //new char[facesize];
    cube->facedata = facedata;
    for(z = 0; z < 6; z++) {

      flags = side[z].flags;
      skip = flags & (sf4punkt + sfTeilung);

      do {
        ez = 0;
        for(i = 0; i < 4; i++) {
          if (i != skip) {
            e[ez] = i;
            ez++;
          };
        };

        a = cube->vertex[s2p[z][e[0]]];
        b = cube->vertex[s2p[z][e[1]]];
        c = cube->vertex[s2p[z][e[2]]];
        b -= a;
        c -= a;
        b = c % b;
        //len = b.betr();
        //if (len > 0.001) b /= len;
        if (flags & sfSidecube) {
          plane->p << a;
          plane->n << b;
          plane->sidecube = side[z].sidecube;
          plane++;
        };

        if (flags & sfBitmap) {
          face = (tcface*) facedata;
          face->points = ez *sizeof(tcpoint);
          face->mat = &scene.materials[side[z].mat];
          face->p << a;
          face->n << b;
          point = (tcpoint *) &face[1];
          for(i = 0; i < ez; i++) {
            point->p = s2p[z][e[i]] *(5*4+1); //* size tprojected
            point->m = side[z].mapping[e[i]];
            point++;
          };
          facedata = (void *) point;
        };

        skip += 2;
      } while (skip < 4);

    };

    cube++;
  };
};


void initmese(tstream &s, tvesa &vesa, int camera, int tracks, int ambient, int debug) {
  int z, count;                                   //0 = intro, 1 = outtro

  scene.intro = !tracks;

  s.openfile("scene.bin");

  finit(0x1A7F); //I=affine, rc=up, pc=double
// todo: p-error im copro

  for(z = 0; z < numclasses; z++) {
    vmtlist[z].firstobj = (char *) getclearmem(s.getint()*vmtlist[z].size);
    vmtlist[z].nextobj = vmtlist[z].firstobj;
  };
  scene.cubedata = (tcube *) getclearmem(s.getint()*sizeof(tcube));
  //palette
  scene.pal = getmem(768);
  s.read(scene.pal,768);
  scene.paltab = getmem((vesa.pbytes <= 2) ? sizeof(tpaltab16) : sizeof(tpaltab32));
  //bitmapdata
  z = s.getint();
  scene.bitmapdata = (char *) getmem(z);
  s.read(scene.bitmapdata,z);
  //materials
  count = s.getint();
  scene.materials = (tmaterial *) getmem(count*sizeof(tmaterial));
  for(z = 0; z < count; z++) {
    scene.materials[z].mapptr = &scene.bitmapdata[s.getint()];
    scene.materials[z].mapand = s.getint();
  };

  //ambient track
  if (ambient == 0) {
    scene.ambient = readtrack(s);
    skiptrack(s);
  } else {
    skiptrack(s);
    scene.ambient = readtrack(s);
  }

  //light maps
  scene.spot1 = getmem(65536);
  scene.spot2 = getmem(65536);
  scene.water = getmem(65536*8);

  //objects
  scene.root = readobj(s,NULL,s.getint(),tracks);

  readlevel(s);

  scene.cam = ((tcamera *) vmtlist[idCamera].firstobj) + camera;
  scene.firstlight = vmtlist[idSpotlight].firstobj;
  scene.lastlight = vmtlist[idSpotlight].nextobj;

  scene.tempdata = (char *) getclearmem(tempsize); //getmem(tempsize);
//  scene.ltab = getmem((lvalues+xtralvalues)*256);
//  scene.ftab = getmem(65536); //char[(fvalues/*+xtrafvalues*/)*256];
  scene.buffer = getmem(vesa.xres*vesa.yres*4);

  scene.lmap = getalignmem(65536+lvalues*256,65536); //+gouraud table

  scene.divtab = (float *) getmem(((vesa.xres > vesa.yres) ? vesa.xres : vesa.yres)*4 + 8); //xres

  scene.idxlist = getmem(256*sizeof(tidxentry));
  scene.zline = getmem(3*zxres*4);
  scene.fogmap = getmem(fogmapsteps*fogmapsize);
  scene.fogwater = getmem(32*zxres); //zeit-aufl”sung * zxres

  scene.flares = getclearmem(128*256);
  scene.flaretab = getalignmem(9*64,64);

  initmdata(vesa,debug);
}


void tempfree() {
  int z;

  for(z = tempsize-1; z >= 0; z--) {
    if (scene.tempdata[z]) break;
  };
  printf("tempdata : %d\n",(tempsize-1)-z);
}

