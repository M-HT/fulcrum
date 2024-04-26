#ifndef _VESA_H
#define _VESA_H

#include <SDL2/SDL.h>

//exit procs
void exit(void);
void exit(const char *s);


//vesa procs

struct tvesa {
  int mode;
  void *physbaseptr;
  int xbytes;
  int xres;
  int yres;
  int pbytes;
  char bpplist[4];
  int area;
  int memsize;  //xbytes*yres
  void *linbuf;
  char flags;
  char redbits;
  char redpos;
  char greenbits;
  char greenpos;
  char bluebits;
  char bluepos;
  char reserved1;
  SDL_Texture *texture;
  SDL_Renderer *renderer;
};

extern "C" {

int setmode(tvesa &);

int clearlinbuf(void);

void textmode(void);

int keypressed(void);

};

void initvesa(tvesa *, int modes);


#endif
