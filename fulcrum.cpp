#include <stdlib.h> //atoi
#include <stdio.h>  //FILE
#include <string.h>
#include <conio.h>
#include "file.h"
#include "demo.h"
#include "credits.h"
#include "mese.h"
#include "moove.h"
#include "erz.h"
#include "rxm.h"


//global
const modes = 4;
tvesa vesa[modes];


//xm-player
int dflags = df16bit | dfStereo | dfReverse;
//int vol = 12;
int mixrate = 44100;
int mastervol = 0;
int iwflag = 1;



//externs
extern void ShowPic(tstream &s, tvesa &vesa);

extern "C" volatile int maincount;
#pragma aux maincount "*"

extern "C" reset_timestep();
#pragma aux reset_timestep "*"


//grabber
tvesa *aktvesa;

struct theader {
 char  ident_len;   //zahl an byte, die nach dem header zu berspringen ist
 char  cmap_type;   // = 1: palette vorhanden
 char  image_type;  //bit0 = 1: mit palette, bit1 = 1: rgb, bit3 = 1: gepackt
 short cmap_origin; //?
 short cmap_len;    //palettenl„nge
 char  centry_size; //bit pro paletteneintrag
 short image_xorg;  //x - ursprung
 short image_yorg;  //y - ursprung
 short image_width; //x - aufl”sung
 short image_height;//y - aufl”sung
 char  pixel_size;  //bit pro pixel
 char  image_discr; //bit5 = 0: bild "auf dem kopf"
};

void grab();
#pragma aux grab "grab"

void grab() {
  FILE *f;
  static int num = 0;
  char fname[16];
  theader header;
  char *cp;
  int x, y, i;
  char col[3];

  if (aktvesa->redbits != 8 || aktvesa->greenbits != 8 || aktvesa->bluebits != 8) return;

  sprintf(fname, "pic%05d.tga", num);
  num++;
  f = fopen(fname,"wb");

  header.ident_len = 0;
  header.cmap_type = 0;   // = 1: palette vorhanden
  header.image_type = 2;  //bit0 = 1: mit palette, bit1 = 1: rgb, bit3 = 1: gepackt
  header.cmap_origin = 0; //?
  header.cmap_len = 0;    //palettenl„nge
  header.centry_size = 0; //bit pro paletteneintrag
  header.image_xorg = 0;  //x - ursprung
  header.image_yorg = 0;  //y - ursprung
  header.image_width = aktvesa->xres; //x - aufl”sung
  header.image_height = aktvesa->yres;//y - aufl”sung
  header.pixel_size = 24;  //bit pro pixel
  header.image_discr = 32; //bit5 = 0: bild "auf dem kopf"
  fwrite(&header,1,sizeof(theader),f);

  for (y = 0; y < aktvesa->yres; y++) {
    cp = (char *) aktvesa->linbuf + y*aktvesa->xbytes;
    for (x = 0; x < aktvesa->xres; x++) {
      i = *(int *) cp;

      col[2] = i >> aktvesa->redpos;
      col[1] = i >> aktvesa->greenpos;
      col[0] = i >> aktvesa->bluepos;
      fwrite(col,1,3,f);

      cp += aktvesa->pbytes;
    }
  }

  fclose(f);
}


void readline(FILE *f, char *str) {
  int c;
  int count = 0;
  int z;

  do {
    if ((c = fgetc(f)) == EOF) break;
    if (c > 32 && count < 45) {
      if (c >= 'A' && c <= 'Z') c += 32;
      str[count] = c;
      count++;
    }
  } while (c != 10);
  str[count] = '\0';
};

int hex(char *s) {
  int r = 0;

  while (*s) {
    r <<=4;
    if (*s >= '0' && *s <= '9') r += *s - '0';
    if (*s >= 'a' && *s <= 'f') r += *s - 'a' + 10;
    if (*s >= 'A' && *s <= 'F') r += *s - 'A' + 10;
    s++;
  };
  return r;
}

void readlist(char *s, char *values, int novalues) {
  int z;
  for(z = 0; z < novalues; z++) values[z] = 0;
  z = 0;
  while (*s && novalues > 0) {
    if (*s >= '0' && *s <= '9') {
      *values *= 10;
      *values += *s - '0';
    };
    if (*s == ',') {
      values++;
      novalues--;
    }
    s++;
  }
}

int atob(char *s) {
  int i;
  i = atoi(s);
  if (i) return i;
  if (*s == 'y' || *s == 'Y') return 1;
  return 0;
}

void readconfig() {
  char str[48];
  char *val;
  FILE *f;
  int m;
  char *mstr;

  f = fopen("fulcrum.cfg","rb");
  if (!f) exit("Can't read config file (fulcrum.cfg)");
  while (!feof(f)) {
    readline(f,str);
    val = strchr(str,';');
    if (val) *val = '\0';
    val = strchr(str,'=');
    if (val) {
      *val = '\0';
      val++;

      if (!memcmp(str,"mode",4)) {
        m = str[4] - '1';
        if (m >= 0 && m < modes) {
          mstr = &str[5];
          if (!strcmp(mstr,"")) vesa[m].mode = hex(val);
          if (!strcmp(mstr,"_x")) vesa[m].xres = atoi(val);
          if (!strcmp(mstr,"_y")) vesa[m].yres = atoi(val);
          if (!strcmp(mstr,"_bit")) readlist(val,vesa[m].bpplist,4);
          if (!strcmp(mstr,"_a")) vesa[m].area = atoi(val);
        }
      }

      //player
      if (!strcmp(str,"nosound")) if (atob(val)) dflags |= dfNosound;
//      if (!strcmp(str,"volume")) vol = atoi(val);
      if (!strcmp(str,"16bit")) if (!atob(val)) dflags &= ~df16bit;
      if (!strcmp(str,"stereo")) if (!atob(val)) dflags &= ~dfStereo;
      if (!strcmp(str,"reverse")) if (atob(val)) dflags &= ~dfReverse;
      if (!strcmp(str,"rate")) mixrate = atoi(val);
      if (!strcmp(str,"sb_dsp1")) if (atob(val)) dflags |= sb_dsp1;
      if (!strcmp(str,"sb_awe")) if (atob(val)) dflags |= sb_awe;
      if (!strcmp(str,"sb_master")) mastervol = atoi(val);
      if (!strcmp(str,"gus_iw")) iwflag = atob(val);
    }
  }
  fclose(f);
}

int parse(char *s, int mul) {
  int r = 0;

  while (*s && (*s < '0' || *s > '9')) s++;
  while (*s >= '0' && *s <= '9') {
    r *= mul;
    r += *s - '0';
    s++;
  }
  return r;
}

int checkenv(tdinfo *dinfo) {
  int i;
  i = dinfo->base >= 0x200 && dinfo->base <= 0x290;
  i &= dinfo->irq >= 2 && dinfo->irq <= 15;
  i &= dinfo->dma1 <= 7;
  i &= dinfo->dma2 <= 7;
  return i;
}

int gusenv(tdinfo *dinfo, int flags, int iwflag) {
  char *s;

  memset(dinfo,0,sizeof(tdinfo));
  s = getenv("ULTRASND");
  dinfo->base = parse(s,16);
  s = strchr(s,',');
  if (!s) return 0;
  dinfo->dma1 = parse(s,10);
  s = strchr(s+1,',');
  if (!s) return 0;
  dinfo->dma2 = parse(s,10);
  s = strchr(s+1,',');
  if (!s) return 0;
  dinfo->irq = parse(s,10);

  if (iwflag >= 2 || (iwflag == 1 && getenv("INTERWAVE"))) dinfo->flags |= gus_iw;
  dinfo->flags |= flags;

  return checkenv(dinfo);
}

int sbenv(tdinfo *dinfo, int rate, int flags) {
  char *s;
  char *v;

  memset(dinfo,0,sizeof(tdinfo));
  s = getenv("BLASTER");
  v = strchr(s,'A');
  if (v) dinfo->base = parse(v,16);
  v = strchr(s,'I');
  if (v) dinfo->irq = parse(v,10);
  v = strchr(s,'D');
  if (!v) return 0;
  dinfo->dma1 = parse(v,10);
  v = strchr(s,'H');
  if (v) dinfo->dma2 = parse(v,10);
  dinfo->rate = rate;
  dinfo->flags = flags;
  return checkenv(dinfo);
}

void initxm() {
  int t;
  tdinfo dinfo;
  void *drv_mem = NULL;

  rxminit();

  t = 0;
  if (!(dflags & dfNosound)) {
    //gus
    if (gusenv(&dinfo,dflags,iwflag)) t = u_test(&dinfo);
    //sb
    if (!t && sbenv(&dinfo,mixrate,dflags)) {
      if (dflags & sb_awe) t = a_test(&dinfo);
      if (!t) t = sb_test(&dinfo);
    }
  }
  //no sound
  if (!t) n_test(&dinfo);

  if (dinfo.mem) drv_mem = allocmem(dinfo.mem);
  if (dinfo.dmabuf) {
    if (!(dinfo.dmabuf = getdmabuf(dinfo.dmabuf))) {
      exit("Not enough low mem");
    }
  }

  dinfo.mastervol = mastervol;

  t = rxmdevinit(&dinfo,drv_mem);
  if (t) {
    if (t == 1) exit("DMA error");
    exit("IRQ error");
  }
  rxmsetvol(12*8);
}

void loadxm(tstream &s, char *name, int pt) {
  int len;
  void *xmdata;

  s.openfile(name);
  len = s.getint();
  xmdata = getmem(len);
  s.read(xmdata,len);
  rxmplay(xmdata,pt);
}



void donexm() {
  rxmdevdone();
  freedmabuf();
  i8_done();
}


void setmode(int mode) {
 aktvesa = &vesa[mode];
  i8_done();
  cleardmabuf();
  if (!setmode(vesa[mode])) exit("Error initing mode");
  i8_init();
}

int delay(int len) {
  int mainstart;

  reset_timestep();
  mainstart = maincount;
  do {
    if (keypressed()) return 1;
  } while (maincount - mainstart < len);
  return 0;
}

void main(int argn, char **argv) {
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
  vesa[3].flags = 1; //erz-effekt kann 8 bit
  readconfig();
  if ((dflags & dfNosound)) sound = 0;

  initvesa(vesa,modes);

  tarjstream s("fulcrum.dat");
//  tstream s;


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
//  initcredits(s,vesa[0]);
  initmese(s,vesa[0],0     ,1     ,1      ,0);
                   //camera,tracks,ambient,debug);
  setmode(0);

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
};