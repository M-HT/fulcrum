#ifndef _VESA_H
#define _VESA_H

//exit procs
void exit();
void exit(char *s);


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
};

extern "C" {

int setmode(tvesa &);
#pragma aux setmode "*" parm [esi] modify [eax ebx ecx esi edi]

int clearlinbuf(void);
#pragma aux clearlinbuf "*" modify [eax ecx edi]

void textmode(void);
#pragma aux textmode "*" modify [eax]

int keypressed(void);
#pragma aux keypressed "*" value [eax] modify [eax]

};

void initvesa(tvesa *, int modes);


#endif