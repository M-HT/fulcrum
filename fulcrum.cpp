#include <stdlib.h> //atoi
#include <stdio.h>  //FILE
//#include <string.h>
//#include <conio.h>
//#include "file.h"
#include "demo.h"
#include "credits.h"
#include "mese.h"
#include "moove.h"
#include "erz.h"
#include "rxm.h"


//global
static const int modes = 4;
static tvesa vesa[modes];


//xm-player
static int dflags = df16bit | dfStereo | dfReverse;
static int mixrate = 44100;



//externs
extern void ShowPic(tstream &s, tvesa &vesa);


static void readconfig(void) {
  int m;

  for (m = 0; m < modes; m++)
  {
    //vesa[m].mode = 0;
#ifdef PANDORA
    vesa[m].xres = 640;
    vesa[m].yres = 480;
    vesa[m].bpplist[0] = 16;
#elif defined(GCW0)
    vesa[m].xres = 320;
    vesa[m].yres = 240;
    vesa[m].bpplist[0] = 16;
#else
    vesa[m].xres = 1280;
    vesa[m].yres = 960;
    vesa[m].bpplist[0] = 32;
#endif
    vesa[m].area = 100;
  }

  //dflags |= dfNosound;
  dflags &= ~dfReverse;
}

static void initxm(void) {
  int t;
  tdinfo dinfo;
  void *drv_mem = NULL;

  rxminit();

  //t = 0;
  //no sound
  //if (!t) n_test(&dinfo);

  dinfo.rate = mixrate;
  dinfo.flags = dflags;

  t = rxmdevinit(&dinfo,drv_mem);
  if (t) {
    if (t == 1) exit("DMA error");
    exit("IRQ error");
  }
  rxmsetvol(12*8);
}

static void loadxm(tstream &s, const char *name, int pt) {
  int len;
  void *xmdata;

  s.openfile(name);
  len = s.getint();
  xmdata = getmem(len);
  s.read(xmdata,len);
  rxmplay(xmdata,len,pt);
}



static void donexm(void) {
  rxmdevdone();
  i8_done();
}


void setmode(int mode) {
  i8_done();
  if (!setmode(vesa[mode])) exit("Error initing mode");
  i8_init();
}

static int delay(unsigned int len) {
  unsigned int mainstart;

  mainstart = SDL_GetTicks();
  do {
    SDL_Delay(1);
    if (keypressed()) return 1;
  } while (SDL_GetTicks() - mainstart < len);
  return 0;
}

int main(int argn, char **argv) {
  int sound = 1;
  int jump = 0;

  if (argn >= 2) {
   switch (argv[1][0]) {
    case 'n' : sound = 0;
               break;
    case '1' : jump = 1;
               break;
    case '2' : jump = 2;
               break;
    case '3' : jump = 3;
               break;
    case '4' : jump = 4;
               break;
   }
  }

  memset(vesa,0,modes*sizeof(tvesa));
  //vesa[3].flags = 1; //erz-effekt kann 8 bit
  readconfig();
  if ((dflags & dfNosound)) sound = 0;

  initvesa(vesa,modes);

  tarjstream s("fulcrum.dat");
//  tstream s;

  if ( SDL_Init (SDL_INIT_VIDEO) != 0 ) exit("Error: Couldn't initialize SDL");
  atexit(SDL_Quit);


  i8_init();
  if (sound) initxm();

  int memsize = vesa[1].xres*vesa[1].yres*28 + 16000000;
  if (sound == 0) memsize -= 1000000;
  initmem(memsize);

  if (jump == 1) goto main;
  if (jump == 2) goto pic;
  if (jump == 3) goto extro;
  if (jump == 4) goto erz;

  //intro part
  setmode(0);

  initcredits(s,vesa[0]);
  initmese(s,vesa[0],1     ,0     ,0      ,0);
                   //camera,tracks,ambient,debug);
  if (sound) loadxm(s,"fx.rxm",0);
  startmese(0/*frame*/);

  if (keypressed()) {
    if (sound) rxmstop(xmStop);
    goto weg;
  }
  if (sound) rxmstop(xmFade);

//goto weg;
//if (keypressed()) getch();


  emptymem();


main:
  //main part
  setmode(1);
  initmoove(s,vesa[1]);

  if (sound) loadxm(s,"looping.rxm",0);
  startmoove();

  if (keypressed()) {
    if (sound) rxmstop(xmStop);
    goto weg;
  }
  if (sound) rxmstop(xmStop);


  emptymem();
pic:
  //adler pic
  setmode(2);
  ShowPic(s, vesa[2]);
  if (delay(8000)) goto weg;
  emptymem();
extro:
  //extro
  setmode(0);
//  initcredits(s,vesa[0]);
  initmese(s,vesa[0],0     ,1     ,1      ,0);
                   //camera,tracks,ambient,debug);

  if (sound) loadxm(s,"fx.rxm",0x20);
  startmese(/*60000*/0/*frame*/);

  if (keypressed()) {
    if (sound) rxmstop(xmStop);
    goto weg;
  }
  if (sound) rxmstop(xmFade);

  emptymem();

erz:
  //erzeffekt
  setmode(3);

  initerz(s,vesa[3]);

  if (sound) loadxm(s,"bunga.rxm",0);
  starterz();

  if (keypressed()) {
    if (sound) rxmstop(xmStop);
    goto weg;
  }
  if (sound) rxmstop(xmFade);
weg:
  if (sound) donexm();

  i8_done();
  textmode();
//checkit();

  return 0;
};
