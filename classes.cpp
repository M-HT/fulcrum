#include <stdio.h>
#include <stdlib.h> //exit
#include <malloc.h>
#include <string.h>

#include "file.h"
#include "classes.h"


//exit
void exit(char * s) {
  printf("\n%s!\n",s);
  exit(1);
};

//tclass
tclass::tclass() {
//  memset(&this[1],0,_msize(this)-sizeof(*this));
};

tclass::~tclass() {

};

tclass* tclass::copy() {
  exit("Copy failure");
  return NULL;
};


//streams
tstream::tstream(char * name, int mode) {

  f_open(f,name,mode);
};

tstream::tstream(const tstring & name, int mode) {
  char s[80];

  name.cstring(s,80);
  f_open(f,s,mode);
};

tstream::~tstream() {
  f_close(f);
};

int tstream::read(void *buf, int len) {
  return(f_read(f,buf,len));
};

tstream::seek(int pos) {
  f_seek(f,pos);
};

int tstream::getpos() {
  return(f_getpos(f));
};

int tstream::write(void *buf, int len) {
  return(f_write(f,buf,len));
};

char tstream::getch() {
  char c;

  read(&c,1);
  return c;
};

tstream::putch(char c) {
  write(&c,1);
};

int tstream::getshort() {
  int i = 0;

  read(&i,2);
  return i;
};

tstream::putshort(int i) {
  write(&i,2);
};

int tstream::getint() {
  int i;

  read(&i,4);
  return i;
};

tstream::putint(int i) {
  write(&i,4);
};

float tstream::getfloat() {
  float f;

  read(&f,4);
  return f;
};

tstream::putfloat(float f) {
//  int * i = (int *) &f;
//  putint(*i & 0xFFFFFF00);
//  putint(*i & 0xFFFFF000);

  write(&f,4);
};

tdbstream::tdbstream(char *name) : tstream(name,ofCreate), xpos(0) {
};

tdbstream::tdbstream(const tstring & name) : tstream(name,ofCreate), xpos(0) {
};

int tdbstream::read(void *buf, int len) {
  return 0;
};

int tdbstream::write(void *buf, int len) {
  char *c;
  int z,i;
  char db[3];

  for(z=0; z < len; z++) {
    if (!xpos) f_write(f,"db ",3); else f_write(f,",",1);
    i = ((char *) buf)[z];
    db[2] = '0' + i % 10;
    db[1] = '0' + (i/10) % 10;
    db[0] = '0' + i / 100;
    f_write(f,db,3);

    if (++xpos >= 16) {
      xpos = 0;
      f_write(f,"\r\n",2);
    };
  };
  return len;
};

tdbstream::seek(int pos) {
};

tdbstream::putlabel(char * l) {
  xpos = 0;
  f_write(f,"\r\n",2);
  f_write(f,l,strlen(l));
  f_write(f,"\r\n",2);
};

//tstring
tstring::tstring(int amaxlen) : len(0) {

  maxlen = amaxlen;
  data = new char[maxlen];
};

tstring::tstring(const tstring& s) {

  len = s.len;
  maxlen = s.maxlen;
  data = new char[maxlen];
  memcpy(data,s.data,len);
};

tstring::tstring(char const * s) {

  len = strlen(s);
  maxlen = len;
  data = new char[maxlen];
  memcpy(data,s,len);
};

tstring::~tstring() {

  delete data;
};

tclass* tstring::copy() {

  return new tstring(*this);
};

tstring& tstring::operator=(const tstring& s) {

  len = (s.len <= maxlen) ? s.len : maxlen;
  memcpy(data,s.data,len);
  return *this;
};

tstring& tstring::operator+=(const tstring& s) {
  int l;

  l = (len + s.len <= maxlen) ? s.len : maxlen - len;
  memcpy(&data[len],s.data,l);
  len += l;
  return *this;
};

tstring tstring::operator+(const tstring& s) const {
  tstring n(len + s.len);

  n += *this;
  return n += s;
};

tstring tstring::operator()(int pos, int l) const {

  if (pos+l > len) {
    l = (pos >= len) ? 0 : len-pos;
  };
  tstring n(l);
  n.len = l;
  memcpy(n.data,&data[pos],l);

  return n;
};

char tstring::operator[](int pos) {
  if (pos < 0 || pos >=len) return 0;
  return data[pos];
};

int tstring::operator==(const tstring& s) const {
  if (len == s.len) {
    return !memcmp(data,s.data,len);
  } else return 0;
};

tstring & tstring::operator=(const char * cstring) {

  len = strlen(cstring);
  if (len > maxlen) len = maxlen;
  memcpy(data,cstring,len);
  return *this;
};

tstring & tstring::operator+=(const char * cstring) {
  int l;

  l = strlen(cstring);
  if (len + l > maxlen) l = maxlen - len;
  memcpy(&data[len],cstring,l);
  len += l;
  return *this;
};

tstring tstring::operator+(const char * cstring) const {
  tstring n(len + strlen(cstring));

  n += *this;
  return n += cstring;
};

tstring operator+(const char * cstring, const tstring & s) {
  tstring n(strlen(cstring) + s.len);

  n += cstring;
  return n += s;
};

int tstring::operator==(const char * cstring) const {
  if (len == strlen(cstring)) {
    return !memcmp(data,cstring,len);
  } else return 0;
};

tstring & tstring::operator+=(const char c) {

  if (len + 1 <= maxlen) {
    data[len] = c;
    len++;
  };
  return *this;
};


tstring::setlen(int l) {

  if (l >= 0) len = (l <= maxlen) ? l : maxlen;
};

int tstring::pos(int pos, char c) const {
  int z = pos;

  while (z < len && data[z] != c) z++;
  return (z < len) ? z : -1;
};

tstring::ins(int pos, char c) {
  int z;

  if (pos >= 0 && pos <= len) {
    if (len < maxlen) len++;
    for(z = len-1; z > pos; z--) {
      data[z] = data[z-1];
    };
    data[pos] = c;
  };
};

tstring::del(int pos, int l) {
  int z;

  if (pos >= 0 && pos < len && l > 0) {
    if (pos + l >= len) len = pos; else {
      len -= l;
      for(z = pos; z < len;z++) data[z] = data[z+l];
    };
  };
};

tstring::upcase() {
  int z;

  for(z = 0; z < len; z++) {
    if (data[z] >= 'a' && data[z] <= 'z') data[z] -= 32;
  };
};

tstring::downcase() {
  int z;

  for(z = 0; z < len; z++) {
    if (data[z] >= 'A' && data[z] <= 'Z') data[z] += 32;
  };
};

int tstring::load(tstream &s) {
  char c;
  int z;

  z = -1;
  do {
    z++;
    s.read(&c,1);
    if (z < maxlen) data[z] = c;
  } while (c);

  len = (z <= maxlen) ? z : maxlen;
  return z+1;
};

tstring::store(tstream &s) {

  if (data) s.write(data,len);
  s.write("\0",1);
};

tstring::cstring(char * s, int l) const {

  --l;
  if (l > len) l = len;
  memcpy(s,data,l);
  s[l] = '\0';
};

tstring::print(int e) const {
  int z;

  for(z = 0; z < len; z++) {
    printf("%c",data[z]);
  };
  if (e) printf("\n");
};


//tcollection
tcollection::tcollection() : count(0), limit(0), items(NULL) {

};


tcollection::tcollection(const tcollection& coll) : limit(0), items(NULL) {
  int z;

  count = coll.count;
  setlimit(coll.limit);
  for(z=0; z < count; z++) {
    items[z] = coll.items[z]->copy();
  };
};

tcollection::~tcollection() {

  deleteall();
  free(items);//delete items;
};

tcollection& tcollection::operator=(const tcollection& coll) {
  int z;

  deleteall();
  count = coll.count;
  setlimit(coll.limit);
  for(z=0; z < count; z++) {
    items[z] = coll.items[z]->copy();
  };
  return *this;
};

tclass* tcollection::operator[](int index) {

  if (index < 0 || index >= count) error(coIndexError,index);
  return items[index];
};

tcollection::insert(tclass *item) {

  atinsert(count,item);
};

tcollection::atinsert(int index, tclass *item) {
  int z;

  if (count >= limit) setlimit(limit+256);
  if ((index < 0) | (index > count)) error(coIndexError,index);
  for(z = count; z > index; z--) {
    items[z] = items[z-1];
  };
  items[index] = item;
  count++;
};

tcollection::atremove(int index) {
  int z;

  if (index < 0 || index >= count) error(coIndexError,index);

  count--;
  for(z = index; z < count; z++) {
    items[z] = items[z+1];
  };
};

tcollection::removeitem(tclass * item) {
  int i;
  i = indexof(item);
  if (i != -1) atremove(i);
};

tcollection::removeall() {
  count = 0;
};

tcollection::atdelete(int index) {

  delete operator[](index);
  atremove(index);
};

tcollection::deleteitem(tclass * item) {
  int i;
  i = indexof(item);
  if (i != -1) atdelete(i);
};

tcollection::deleteall() {
  int z;

  for(z = 0; z < count; z++) {
    delete items[z];
  };
  count = 0;
};

int tcollection::indexof(tclass * item) {
  int z;

  for(z = count-1; (z >= 0) && (items[z] != item); z--);
  return z;
};

tcollection::setlimit(int alimit) {

  if (alimit < count) alimit = count;
  items = (tclass **) realloc(items,alimit*4);
  if (!items && alimit) error(coOverflow,alimit);
  limit = alimit;
};

tcollection::error(int code, int info) {

  printf("collection error %d, %d\n",code,info);
  exit(212);//falls kein exit mssen die anderen methoden berarbeitet werden
};





