#include <stdio.h>
#include <string.h>
#include "file.h"
#include "classes.h"
#include "13h.h"


void main(int argn, char **argv) {
  int ch;
  tstream *s;
  int pos1 = 0;

  int y;

  char pal[768];
  char *pic = (char *) 0xA0000;

  if (argn <= 1) s = new tstream("scene.bin",ofRead); else
    s = new tstream(argv[1],ofRead);
  s->seek(6*4);
//  s->read(pal,6*4);
  s->read(pal,768);

  init256();
  setpal(&pal,1);

  for(y = 0; y < 60; y++) putpixel(y+256,128,y);
  do {

    s->seek(pos1);
    for(y = 0; y < 200; y++) s->read(&pic[y*320],256);

    ch = readchar();

    switch( ch) {
     case -75  : pos1--;
                 break;
     case -77  : pos1++;
                 break;

     case -72  : pos1 -= 256;
                 break;
     case -80  : pos1 += 256;
                 break;
     case 'a'  : pos1 -= 256*64;
                 break;

     case 'y'  : pos1 += 256*64;
                 break;

     case -73  : pos1 -= 256*192;
                 break;
     case -81  : pos1 += 256*192;
                 break;

    };
    if (pos1 < 0) pos1 = 0;

    s->f.error = 0;
  } while (ch != 27);
  textmode();
  delete s;
};