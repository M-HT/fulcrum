#include <stdio.h>
//#include "file.h"
#include "demo.h"


const fvalues = 32;


struct tcredit {
 int xres;
 int yres;
 char *picdata;
 int xpos;
 int ypos;
 int starttime;
 int startstep;
 int fadespeed;
 void *paltab;
};

struct tcreditdef {
 int xpos;
 tcredit *credit[2];
};

struct tpaltab {
 int red;
 int green;
 int blue;
 int trans;
 void *paltab;		  //16 bit: fvalues*256*2, 32 bit: fvalues*256*4
};

struct tcreddata {
 int xres;
 int yres;
 void *pal;
 char *picdata;
 int starttime;
 int scrollspeed;
 int scrolltime;
 int enginetime;

 tcredit *credits;

 int nocreditdefs;
 tcreditdef *creditdefs;

 int nopaltabs;
 tpaltab *paltabs;
 void *paltab;

 int creditnum;
};


/*

main data:
----------
 xres		(int)
 yres		(int)
 pal		(tpal)
 picdata	(n chars)

 starttime	(int)
 scrollspeed	(int)
 scrolltime	(int)
 enginetime	(int)

 nopaltabs	(int)
 paltabs	(n red,green,blue,trans)

 nocredits	(int)
 credits	(n tcredit)

 nocreditdefs	(int)
 creditdefs	(n tcreditdef)

tpal:
-----
 red		(256 chars)
 green		(256 chars)
 blue		(256 chars)

credit:
-------
 xres		(int)
 yres		(int)
 picdata	(n chars)
 xpos		(int)
 ypos		(int)
 starttime	(int)
 startstep	(int)
 fadespeed	(int)
 paltab 	(int)

creditdef:
----------
 xpos		(int)
 credit 	(2 ints)

*/

extern "C" tcreddata creddata;
#pragma aux creddata "*"

extern "C" {
void initcdata(tvesa &);
#pragma aux initcdata "*" parm [esi] modify [eax ebx ecx edx esi edi]
}


void initcredits(tstream &s, tvesa &vesa) {
  int z, count;
  tcredit *credit;
  tcreditdef *creditdef;
  tpaltab *paltab;

  s.openfile("credits.bin");

  //xres
  z = creddata.xres = s.getint();
  //yres
  z *= creddata.yres = s.getint();
  //pal
  creddata.pal = getmem(768);
  s.read(creddata.pal,768);
  //picdata
  creddata.picdata = (char *) getmem(z);
  s.read(creddata.picdata,z);

  //start time
  creddata.starttime = s.getint();
  //scrollspeed
  creddata.scrollspeed = s.getint();
  //scrolltime
  creddata.scrolltime = s.getint();
  //engine time
  creddata.enginetime = s.getint();

  //number of paltabs
  count = creddata.nopaltabs = s.getint();
  paltab = creddata.paltabs = (tpaltab *) getmem(count*sizeof(tpaltab));
  z = (vesa.pbytes <= 2) ? fvalues*256*2 : fvalues*256*4;
  while (count) {
    //red
    paltab->red = s.getint();
    //green
    paltab->green = s.getint();
    //blue
    paltab->blue = s.getint();
    //transparency
    paltab->trans = s.getint();
    //paltab
    creddata.paltab = paltab->paltab = getmem(z);

    paltab++;
    count--;
  }

  //number of credits
  count = s.getint();
  credit = creddata.credits = (tcredit *) getmem(count*sizeof(tcredit));
  while (count) {
    //xres
    z = credit->xres = s.getint();
    //yres
    z *= credit->yres = s.getint();
    //picdata
    credit->picdata = (char *) getmem(z);
    s.read(credit->picdata,z);
    //xpos
    credit->xpos = s.getint();
    //ypos
    credit->ypos = s.getint();
    //starttime
    credit->starttime = s.getint();
    //startstep
    credit->startstep = s.getint();
    //fadespeed
    credit->fadespeed = s.getint();
    //paltab
    credit->paltab = creddata.paltabs[s.getint()].paltab;

    credit++;
    count--;
  }

  //number of creditdefs
  count = creddata.nocreditdefs = s.getint();
  creditdef = creddata.creditdefs = (tcreditdef *) getmem(count*sizeof(tcreditdef));
  while (count) {
    //xpos
    creditdef->xpos = s.getint();
    //credit references
    z = s.getint();
    creditdef->credit[0] = (z != -1) ? &creddata.credits[z] : NULL;
    z = s.getint();
    creditdef->credit[1] = (z != -1) ? &creddata.credits[z] : NULL;

    creditdef++;
    count--;
  }

  initcdata(vesa);
}

