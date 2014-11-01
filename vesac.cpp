#include <stdlib.h> //exit(int)
#include <stdio.h> //printf
#include "vesa.h"
#include "rxm.h"

//exit
void exit(void) {
  textmode();
  i8_done();
  exit(1);
};

void exit(const char *s) {
  textmode();
  printf("%s!\n",s);
  i8_done();
  exit(1);
};


static void getvesa(tvesa *vesa, int modes) {
  int modecnt;

  modecnt = 1;
  while (modecnt <= modes) {
    vesa->physbaseptr = NULL;

    vesa->pbytes = (vesa->bpplist[0]+7) >> 3;
    vesa->xbytes = vesa->xres * vesa->pbytes;
    vesa->memsize = vesa->xbytes * vesa->yres;
    vesa->linbuf = NULL;

    if (vesa->bpplist[0] == 32)
    {
      vesa->redbits = 8;
      vesa->redpos = 0;
      vesa->greenbits = 8;
      vesa->greenpos = 8;
      vesa->bluebits = 8;
      vesa->bluepos = 16;
    }
    else
    {
      vesa->redbits = 5;
      vesa->redpos = 0;
      vesa->greenbits = 6;
      vesa->greenpos = 5;
      vesa->bluebits = 5;
      vesa->bluepos = 11;
    }

    vesa++;
    modecnt++;
  }
}

void initvesa(tvesa *vesa, int modes) {
  getvesa(vesa,modes);
}



