#include <string.h>
#include "demo.h"

//#define MEM_CHECK

//memory
char *membase;
char *memptr;
char *memtop;

char *mem_err = "Out of memory";

#ifdef MEM_CHECK
struct tmemcheck {
  int i1;
  int i2;
  tmemcheck *next;
  int i3;
  int i4;
}

tmemcheck *head;
tmemcheck *current;

tmemcheck *getcheck() {
  tmemcheck *check;

  check = (tmemcheck *) memptr;
  memptr += sizeof(tmemcheck);
  if (memptr > memtop) exit(mem_err);
  check->i1 = 0xAAAAAAAA;
  check->i2 = -1;
  check->next = NULL;
  check->i3 = 0;
  check->i4 = 0x55555555;
  return check;
}
#endif

void *allocmem(int size) {
  void *mem;
  mem = (void *) new char[size];
  if (!mem) exit(mem_err);
  return mem;
}


void initmem(int size) {
  membase = (char *) allocmem(size);
  memtop = membase+size;
  emptymem();
}

void emptymem() {
  memptr = membase;


#ifdef MEM_CHECK
 head = getcheck();
 current = head;
#endif
}

void *getmem(int size) {
  void *p;

  size = (size+3) & ~3;
  p = (void *) memptr;
  memptr += size;
  if (memptr > memtop) exit(mem_err);

#ifdef MEM_CHECK
 current->next = getcheck();
 current = current->next;
#endif

  return p;
}

void *getclearmem(int size) {
  void *p;

  p = getmem(size);
  memset(p,0,size);
  return p;
}

void *getalignmem(int size, int align) {
  char *p;

  align--;
  p = (char *) ((int) (memptr + align) & ~(align));

  memptr = p + size;
  if (memptr > memtop) exit(mem_err);

#ifdef MEM_CHECK
 current->next = getcheck();
 current = current->next;
#endif

  return (void *) p;
}

void *getalignclearmem(int size, int align) {
  void *p;

  p = getalignmem(size,align);
  memset(p,0,size);
  return p;
}

void *gettempmem(int size) {
  if (memptr + size > memtop) exit(mem_err);
  return (void *) memptr;
}


void checkit() {
  printf("Mem used: %d\n",(int) (memptr - membase));
  printf("Mem free: %d\n",(int) (memtop - memptr));

#ifdef MEM_CHECK
  tmemcheck *current;
  int count;

  count = 0;
  current = head;
  while (current) {
    if (current->i1 != 0xAAAAAAAA || current->i2 != -1 || current->i3 != 0 || current->i4 != 0x55555555) {
      printf("Memory check error at block %d",count);
      exit("");
    }
    current = current->next;
    count++;
  }
  printf("%d mem blocks checked\n",count);
#endif
}



//data file reader
tstream::tstream() {
  file = NULL;
}

tstream::~tstream() {
  if (file) fclose(file);
}

void tstream::openfile(char *name) {
  if (file) fclose(file);
  file = fopen(name,"rb");
  if (!file) exit("File not found");
}

int tstream::read(void *buf, int len) {
  return fread(buf,1,len,file);
}

void tstream::skip(int len) {
  fseek(file,len,SEEK_CUR);
}

char tstream::getch() {
  char c;

  read(&c,1);
  return c;
}

int tstream::getint() {
  int i;

  read(&i,4);
  return i;
}

float tstream::getfloat() {
  float f;

  read(&f,4);
  return f;
}


//arj data file reader
tarjstream::tarjstream(char *name) {
  int compsize;
  int pos;
  tfiledef *filedef;

  file = fopen(name,"rb");
  if (!file) exit("Error reading datafile");
  mem = new char[arjbuflen];
  if (!mem) exit(mem_err);

  make_crctable();
  read_header(file); //first header

  pos = ftell(file);
  head = NULL;
  while (compsize = read_header(file)) {

    filedef = new tfiledef;
    if (!filedef) exit(mem_err);
    filedef->next = head;
    filedef->pos = pos;
    get_filename(filedef->name,100);

    head = filedef;

    pos = ftell(file) + compsize;
    fseek(file,compsize,SEEK_CUR);
  }
}

tarjstream::~tarjstream() {
  fclose(file);
  //delete mem;
}

void tarjstream::openfile(char *name) {
  tfiledef *current;
//  char msg[40];

  current = head;
  while (current) {
    if (stricmp(current->name,name) == 0) {
      fseek(file,current->pos,SEEK_SET);
      read_header(file);

      mempos = 0;
      decode(file,(unsigned char *) mem);
      return;
    }
    current = current->next;
  }
//  sprintf(msg, "File %s not found", name);
//  exit(msg);
  exit("File not found");
}

int tarjstream::read(void *buf, int len) {
  int l, bufpos;

  bufpos = 0;
  while (len > 0) {
    l = arjbuflen - mempos;
    if (l <= 0) {
      mempos = 0;
      decode(file,(unsigned char *) mem);
      l = arjbuflen;
    }

    if (l > len) l = len;

    memcpy(&((char *) buf)[bufpos],&mem[mempos],l);

    mempos += l;
    bufpos += l;
    len -= l;
  }
  return bufpos;
}

void tarjstream::skip(int len) {
  int l;

  while (len > 0) {
    l = arjbuflen - mempos;
    if (l <= 0) {
      mempos = 0;
      decode(file,(unsigned char *) mem);
      l = arjbuflen;
    }
    if (l > len) l = len;
    mempos += l;
    len -= l;
  }
}


//tracks

ttrack *readtrack(tstream &s) {
  ttrack *track;
  int size;

  track = (ttrack *) getmem(sizeof(ttrack)); //new ttrack;

  size = s.getint();
  track->data = (char *) getmem(size); //new char[size];
  s.read(track->data,size);
  track->key = 0;

  return track;
}

tltrack *readltrack(tstream &s, int typesize) {
  tltrack *ltrack;
  int z, size;

  ltrack = (tltrack *) getmem(sizeof(tltrack)); //new tltrack;

  ltrack->startstate = (float *) getmem(typesize*2*sizeof(float)); //new float[typesize*2];
  ltrack->endstate = &ltrack->startstate[typesize];
  s.read(ltrack->startstate,typesize*sizeof(float)); //startstate
  for(z = 0; z < typesize; z++) ltrack->endstate[z] = 0.0;//ltrack->startstate[z];
  ltrack->endframe = s.getint();                //startframe
  ltrack->startframe = ltrack->endframe-1;
  ltrack->keys = s.getint();                    //keys

  size = s.getint();                            //size
  ltrack->data = (char *) getmem(size); //new char[size];
  s.read(ltrack->data,size);                    //data

  return ltrack;
}

int *readhtrack(tstream &s) {
  int count;
  int *track;

  count = s.getint();
  track = (int *) getmem((count+1)*sizeof(int));
  track[0] = count;
  s.read(&track[1],count*sizeof(int));
  return track;
}
