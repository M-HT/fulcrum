#ifndef FILE_H
#define FILE_H

struct tfile {
 int   handle;
 int   error;
 int   filepos;
 char  flags;
 int   bufpos;
 int   bufend;
 char  buf[2048];
};

struct tsearch {
 char  fill[21];
 char  attr;
 short time;
 short date;
 int   size;
 char  name[13];
 char  free[256-43];
};

#define ofRead      0
#define ofWrite     1
#define ofReadwrite 2
#define ofCreate    256

#define faArchive   0x20

typedef struct tfile file;


#ifndef __cplusplus

//void f_err();

void f_open(file *f, char *name, int mode);
  #pragma aux f_open parm [esi] [edx] [eax] modify [eax ecx edx edi]

void f_close(file *f);
  #pragma aux f_close parm [esi] modify [eax ebx ecx edx esi edi]

int f_read(file *f, void *p, int size);
  #pragma aux f_read parm [esi] [edi] [edx] value [eax] modify [eax ebx ecx edx esi edi]

int f_write(file *f, void *p, int size);
  #pragma aux f_write parm [edi] [esi] [edx] value [eax] modify [eax ebx ecx edx esi edi]

int f_getpos(file *f);
  #pragma aux f_getpos parm [esi] value [eax] modify [eax]

void f_seek(file *f, int pos);
  #pragma aux f_seek parm [esi] [edx] modify [eax ebx ecx edx esi edi]

int f_findfirst(char *, int, struct tsearch *);
  #pragma aux f_findfirst parm [eax] [ecx] [edx] value [eax] modify [eax ecx edx]

int f_findnext();
  #pragma aux f_findnext value [eax] modify [eax]

#endif
#ifdef __cplusplus

extern "C" {

//void f_err();

void f_open(file &f, char *name, int mode);
  #pragma aux f_open parm [esi] [edx] [eax] modify [eax ecx edx edi]

void f_close(file &f);
  #pragma aux f_close parm [esi] modify [eax ebx ecx edx esi edi]

int  f_read(file &f, void *p, int size);
  #pragma aux f_read parm [esi] [edi] [edx] value [eax] modify [eax ebx ecx edx esi edi]

int  f_write(file &f, void *p, int size);
  #pragma aux f_write parm [edi] [esi] [edx] value [eax] modify [eax ebx ecx edx esi edi]

int  f_getpos(file &f);
  #pragma aux f_getpos parm [esi] value [eax] modify [eax]

void f_seek(file &f, int pos);
  #pragma aux f_seek parm [esi] [edx] modify [eax ebx ecx edx esi edi]



};

#endif

#endif