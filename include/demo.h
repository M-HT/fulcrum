#ifndef _DEMO_H
#define _DEMO_H

#include <stdio.h>  //printf, FILE
#include "vesa.h"
#include "arj1.h"



//mem procs
void *allocmem(int size);                   //get mem from heap
void initmem(int size);                     //init memory pool
void emptymem();                            //empty memory pool
void *getmem(int size);                     //get mem from memory pool
void *getclearmem(int size);                //get cleared mem
void *getalignmem(int size, int align);     //get aligned mem
void *getalignclearmem(int size, int align);//get aligned cleared mem
void *gettempmem(int size);

void checkit();


//data file reader
class tstream {
  public:
    tstream();
    virtual ~tstream();
    virtual void openfile(const char *);
    virtual int read(void * buf, int len);
    virtual void skip(int len);


    unsigned char getch();
    int getint();
    float getfloat();
  protected:
    FILE *fileopen(const char *filename, const char *mode);
    FILE *file;
};


struct tfiledef {
  char name[100];
  int pos;
  tfiledef *next;
};


class tarjstream : public tstream {
  public:
    tarjstream(const char *);
    virtual ~tarjstream();
    virtual void openfile(const char *);
    virtual int read(void * buf, int len);
    virtual void skip(int len);

  protected:
    tfiledef *head;
    char *mem;
    int mempos;
};


//tracks
struct ttrack {
  void *data;
  int key;
};

struct tltrack {
  float *startstate;
  int startframe;
  float *endstate;
  int endframe;
  int keys;
  void *data;
};

ttrack *readtrack(tstream &s);
tltrack *readltrack(tstream &s, int typesize);
int *readhtrack(tstream &s);

#endif //_DEMO_H
