#include <stdlib.h> //exit(int)
#include <stdio.h> //printf
#include "vesa.h"
#include "rxm.h"


//exit
void exit() {
  textmode();
  i8_done();
  exit(1);
};

void exit(char *s) {
  textmode();
  printf("%s!\n",s);
  i8_done();
  exit(1);
};

//vesa

typedef unsigned short WORD;

_Packed struct tVbeInfoBlock {
   char VbeSignature[4];  // Set it to "VBE2"
   WORD VbeVersion;
   char *OemStringPtr;

   char Capabilities[4];
   WORD *VideoModePtr;
   WORD TotalMemory;

//--------- (only VBE2)
   WORD OemSoftwareRev;
   char *OemVendorNamePtr;
   char *OemProductNamePtr;
   char *OemProductRevPtr;

   char Reserved[222];
   char OemData[256];   // OEM String pointers point to this Memory area
};


_Packed struct tModeInfoBlock {
  // Vesa 1.0
  WORD ModeAttributes;
  char WinAAttributes;
  char WinBAttributes;
  WORD WinGranularity;
  WORD WinSize;
  WORD WinASegment;
  WORD WinBSegment;
  int  WinFuncPtr;
  WORD BytesPerScanLine;

  // Vesa 1.2
  WORD XResolution;
  WORD YResolution;
  char XCharSize;
  char YCharSize;
  char NumberOfPlanes;
  char BitsPerPixel;
  char NumberOfBanks;
  char MemoryModel;
  char BankSize;
  char NumberOfImagePages;
  char Reserved1;

  // Colorfield Information
  char RedMaskSize;
  char RedFieldPosition;
  char GreenMaskSize;
  char GreenFieldPosition;
  char BlueMaskSize;
  char BlueFieldPosition;
  char RsvdMaskSize;
  char RsvbFieldPosition;
  char DirectColorModeInfo;

  // Vesa 2.0
  void *PhysBasePtr;
  int  OffScreenMemOffset;
  WORD OffScreenMemSize;

  char reserved2[206];
};


extern "C" {

tVbeInfoBlock *getvbeinfoblock();
#pragma aux getvbeinfoblock "*" modify [eax ebx ecx edx esi edi]

int getmodenum(int);
#pragma aux getmodenum "*" parm [eax] modify [eax ebx]

tModeInfoBlock *getmodeinfoblock(int);
#pragma aux getmodeinfoblock "*" parm [eax] modify [eax ebx ecx edx esi edi]

void *mapaddress(void *, int);
#pragma aux mapaddress "*" parm [eax] [edx] modify [eax ebx ecx edx esi edi]

};



int checkmode(tModeInfoBlock *mi, int b8) {
  if (!mi) return 0;
  //0x80 = LFB, 0x10 = Graphics mode, 0x01 = mode supported
  if (mi->ModeAttributes & 0x91 != 0x91) return 0;
  if (mi->MemoryModel == 4 && mi->BitsPerPixel == 8 && b8) return 1;
  if (mi->MemoryModel == 6 && mi->BitsPerPixel > 8) return 1;
  return 0;
};


tModeInfoBlock *findmode(tvesa &v, int bpp) {
  int modecnt, modenum;
  tModeInfoBlock *mi;

  modecnt = 0;
  while ((modenum = getmodenum(modecnt)) != -1) {
    mi = getmodeinfoblock(modenum);
    if (checkmode(mi,v.flags)) {
      if (   mi->BitsPerPixel == bpp
          && mi->XResolution == v.xres
          && mi->YResolution == v.yres) {

        v.mode = modenum;
        return mi;
      }
    }
    modecnt ++;
  }
  return NULL;
}

void copymode(tModeInfoBlock *mi, tvesa &v) {
  v.physbaseptr = mi->PhysBasePtr;

  v.xbytes = mi->BytesPerScanLine;
  v.xres = mi->XResolution;
  v.yres = mi->YResolution;
  v.pbytes = (mi->BitsPerPixel+7) >> 3;
  v.memsize = v.xbytes * v.yres;
  v.linbuf = NULL;

  v.redbits = mi->RedMaskSize;
  v.redpos = mi->RedFieldPosition;
  v.greenbits = mi->GreenMaskSize;
  v.greenpos = mi->GreenFieldPosition;
  v.bluebits = mi->BlueMaskSize;
  v.bluepos = mi->BlueFieldPosition;
};


void printmodes(int b8) {
  int modecnt, modenum;
  tModeInfoBlock *mi;
  int flag = 0;

  modecnt = 0;
  while ((modenum = getmodenum(modecnt)) != -1) {
    mi = getmodeinfoblock(modenum);
    if (checkmode(mi,b8)) {
      printf("Mode %X : X=%4d, Y=%4d, B=%2d    ",modenum,mi->XResolution,mi->YResolution,mi->BitsPerPixel);
      if (flag & 1) printf("\n");
      flag++;
    }
    modecnt ++;
  }
  if (flag & 1) printf("\n");
}

/*
void printmodes() {
  int modecnt, modenum;
  tModeInfoBlock *mi;
  char bpp;

  modecnt = 0;
  while ((modenum = getmodenum(modecnt)) != -1) {
    mi = getmodeinfoblock(modenum);
    if (checkmode(mi)) {
      switch(mi->BitsPerPixel) {
       case 15: bpp = 1;
       case 16: bpp <<= 1;
       case 24: bpp <<= 1;
       case 32: bpp <<= 1;
                break;
       default: bpp = 0;
      }
    }
    modecnt ++;
  }
}
*/

void getvesa(tvesa *vesa, int modes) {
  int z, modecnt;
  tVbeInfoBlock *vi;
  tModeInfoBlock *mi;

  vi = getvbeinfoblock();
  if (!vi) {
    exit("No VESA found");
  }

  if (vi->VbeVersion < 0x200) exit("No VESA 2.0 found");

  modecnt = 1;
  while (modecnt <= modes) {
    if (vesa->mode) {
      mi = getmodeinfoblock(vesa->mode);
    } else {
      mi = NULL;
      for (z = 0; z < 4; z++) {
        if (vesa->bpplist[z]) {
          mi = findmode(*vesa,vesa->bpplist[z]);
          if (mi) break;
        }
      }
    }
    if (!checkmode(mi,vesa->flags)) {
      printf("Supported modes:\n");
      printmodes(vesa->flags);
      printf("Error detecting mode %d! Please edit the config file.\n",modecnt);
      exit();
    }
    copymode(mi,*vesa);

    vesa++;
    modecnt++;
  }
}

void mapvesa(tvesa *vesa, int modes) {
  int z, max, i;
  void *p;

  do {
    max = 0;

    for(z = 0; z < modes; z++) {
      if (!vesa[z].linbuf && vesa[z].memsize > max) {
        max = vesa[z].memsize;
        i = z;
      };
    };
    if (!max) break;

    for(z = 0; z < modes; z++) {
      if (vesa[z].linbuf && vesa[z].physbaseptr == vesa[i].physbaseptr) {
        vesa[i].linbuf = vesa[z].linbuf;
      };
    };

    if (!vesa[i].linbuf) {
      p = mapaddress(vesa[i].physbaseptr,vesa[i].memsize);
      if (!p) exit("Can't map linear framebuffer");
      vesa[i].linbuf = p;
    };
  } while (1);
};


void scalevesa(tvesa *vesa, int modes) {
  int a, o;

  while (modes > 0) {

    vesa->physbaseptr = vesa->linbuf;

    a = vesa->area;
    if (a == 0 || a > 100) a = 100;
    if (a < 20) a = 20;

    o = (vesa->xres*(100-a)/200) & (~3);
    o *= vesa->pbytes;
    o += (vesa->yres*(100-a)/200)*vesa->xbytes;
    vesa->linbuf = (void *) ((int) vesa->linbuf + o);

    vesa->xres = ((vesa->xres*a)/100) & (~3);
    vesa->yres = ((vesa->yres*a)/100) & (~3);

    modes--;
    vesa++;
  }
}

void initvesa(tvesa *vesa, int modes) {
  getvesa(vesa,modes);
  mapvesa(vesa,modes);
  scalevesa(vesa,modes);
}



