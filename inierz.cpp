#include <stdlib.h>
#include <stdio.h>
#include "copro.h"
#include "demo.h"

#define ringpoints   8
#define hsphererings 4
#define meltrings    6


struct tcpoint {
 float x;
 float y;
 float dx;
 float dy;
 float v;
};

struct tchain {
 int points;
 tcpoint *pointlist;
 float *ringlist;
 float *normalslist;
 int lastpoint;
};


struct terzdata {
 float *ring;
 float *sphere;
 float *ringlist;
 float *normalslist;
 int chains;
 tchain *chain;

 float *divtab;
 char *buffer;

 char *pal;
 char *picdata;
 float xres1;
 float yres1;
 char *pic1;
 float xres2;
 float yres2;
 char *pic2;
 void *paltab;

 void *camtrack;
 void *targettrack;

 char *tempdata;

 int lines;
 char *scroller;
};


//upscroller

struct tchar {
 int width;
 char *data;
};

tchar *chars;

extern "C" terzdata erzdata;
#pragma aux erzdata "*"

extern "C" {
void initedata(tvesa &);
#pragma aux initedata "*" parm [esi] modify [eax ebx ecx edx esi edi]

}


void initchains2(tstream &s) {
  int chains;
  int count;
  tchain *chain;
  tcpoint *point;
  int n;


  chains = s.getint();
  erzdata.chains = chains;
  erzdata.chain = (tchain *) getmem(chains*sizeof(tchain));//new tchain[chains];
  chain = erzdata.chain;

  while (chains > 0) {
    count = s.getint();
    point = (tcpoint *) getclearmem(count*sizeof(tcpoint));//new tcpoint[count];

    chain->points = count;
    chain->lastpoint = count;
    chain->pointlist = point;

    n = (hsphererings*2+count+meltrings)*ringpoints*3;
    chain->ringlist = (float *) getmem(n*sizeof(float));//new float[n];
    chain->normalslist = (float *) getmem(n*sizeof(float));//new float[n];

    while (count > 0) {
	point->x = s.getfloat()*2.0;// + 10.0;
	point->y = s.getfloat()*-2.0;

//	  point->dx = 0.0;
//	  point->dy = 0.0;
//	  point->v = 0.0;

	point++;
	count--;
    }

    chain++;
    chains--;
  }
}

void drawchar(char *cdat, int cx, int cy, char *dest, int xpos, int ypos) {
  int x, y;

  for(y = 0; y < cy; y++) {
    for(x = 0; x < cx; x++) {
      dest[xpos + x + 320*(ypos + y)] = cdat[x + cx*y];
    }
  }
}

const xstart = 0;//20;
const ystart = 0;//20;
const xgap = 2;
const ygap = 2;
const lines = 240*100;

initupscroll(tstream &s, tvesa &vesa) {
  int height;
  int count, z;
  int size;
  char *buf;
  int curx, cury;
  int ch;
  tchar *c;

  //height (int)
  height = s.getint();
  //num chars (int)
  count = s.getint();
  chars = (tchar *) getmem(count*sizeof(tchar));

  for(z = 0; z < count; z++) {
    //width (int)
    chars[z].width = s.getint();

    size = chars[z].width*height;
    chars[z].data = (char *) getmem(size);
    s.read(chars[z].data,size);
  }

  //get text
  buf = erzdata.scroller;//(char *) getclearmem(lines*320);
  curx = xstart;
  cury = ystart;
  while((ch = s.getch()) != 254) {

    if (ch == 255) {
      curx = xstart;
      cury += height+ygap;
    } else {
      c = &chars[ch];

      if (cury < lines-height) drawchar(c->data,c->width,height,buf,curx,cury);

      curx += c->width + xgap;
    }
  }
//  for(z = 0; z < 255; z++) buf[z] = (z & 7) +248;
//  Scale8ASM(buf,&erzdata.buffer[vesa.xres*vesa.yres],320,lines,vesa.xres,vesa.yres*lines/240);
}

void initerz(tstream &s, tvesa &vesa) {
  int z;

  finit(0x1A7F); //I=affine, rc=up, pc=double

  erzdata.ring = (float *) getmem(ringpoints*2*sizeof(float));//new float[ringpoints*2];
  erzdata.sphere = (float *) getmem((hsphererings*2+1)*ringpoints*3*sizeof(float));//new float[(hsphererings*2+1)*ringpoints*3];
  erzdata.ringlist = (float *) getmem((meltrings/2+1+hsphererings)*ringpoints*3*sizeof(float));//new float[(meltrings/2+1+hsphererings)*ringpoints*3];
  erzdata.normalslist = (float *) getmem((meltrings/2+1+hsphererings)*ringpoints*3*sizeof(float));//new float[(meltrings/2+1+hsphererings)*ringpoints*3];

  s.openfile("erzdata.bin");
  initchains2(s);

  erzdata.divtab = (float *) getmem(vesa.xres*4+4);

  erzdata.buffer = (char *) getmem(vesa.xres*vesa.yres/* *(lines/240+1)*/);
  erzdata.lines = lines;//+1;
  erzdata.tempdata = (char *) getmem(32768); //new char[4096];

  s.openfile("erz.bin");

  //format: palette, datalen, data
  //	    map1: offset, xres, yres
  //	    map2: offset, xres, yres
  //	    camera track, target track
  erzdata.pal = (char *) getmem(768);
  s.read(erzdata.pal,768);
  z = s.getint();
  erzdata.picdata = (char *) getmem(z);
  s.read(erzdata.picdata,z);

  erzdata.pic1 = &erzdata.picdata[s.getint()];
  erzdata.xres1 = s.getint()*255.9;
  erzdata.yres1 = s.getint()*255.9;
  erzdata.pic2 = &erzdata.picdata[s.getint()];
  erzdata.xres2 = s.getint()*127.9;
  erzdata.yres2 = s.getint()*127.9;
  erzdata.paltab = getmem((vesa.pbytes <= 2) ? 256*2 : 256*4);

  erzdata.camtrack = readtrack(s);
  erzdata.targettrack = readtrack(s);

  erzdata.scroller = (char *) getclearmem(320*lines);
  initupscroll(s,vesa);

  initedata(vesa);
}
