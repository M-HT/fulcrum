#ifndef CLASSES_H
#define CLASSES_H


const ecMem = 203;


const coIndexError = -1;
const coOverflow   = -2;


struct tpoint {
  int x,y;
};

void exit(char *);

class tclass {
  public:
    tclass();
    virtual ~tclass();
    virtual tclass* copy();
};


class tstring;

class tstream : public tclass {
  public:
    tstream(char *, int mode);
    tstream(const tstring &, int mode);
    virtual ~tstream();
    virtual int read(void * buf, int len);
    virtual int write(void * buf, int len);
    virtual int getpos();
    virtual seek(int);

    char getch();
    putch(char);
    int getshort();
    putshort(int);
    int getint();
    putint(int);
    float getfloat();
    putfloat(float);

//  protected:
    file f;
};

class tdbstream : public tstream {
  public:
    tdbstream(char *name);
    tdbstream(const tstring &);
    virtual int read(void *buf, int len);
    virtual int write(void *buf, int len);
    virtual seek(int pos);

    putlabel(char * name);
  protected:
    int xpos;
};



class tstring : public tclass {
  public:
    tstring(int);
    tstring(const tstring&);
    tstring(char const *);
    virtual ~tstring();
    virtual tclass* copy();

    tstring & operator=(const tstring &);
    tstring & operator+=(const tstring &);
    tstring operator+(const tstring &) const;

    tstring operator()(int pos, int l) const;  //substring operator
    char operator[](int);
    int operator==(const tstring&) const;

    tstring & operator=(const char *);
    tstring & operator+=(const char *);
    tstring operator+(const char *) const;
    friend tstring operator+(const char *, const tstring &);
    int operator==(const char *) const;

    tstring & operator+=(const char);

    setlen(int);
    int pos(int, char) const;
    ins(int pos, char);
    del(int pos, int l);
    upcase();
    downcase();

    int load(tstream &s);
    store(tstream &s);

    cstring(char *, int) const;

    print(int e = 0) const;
    int len;
  protected:
    int maxlen;
    char* data;
};



class tcollection : public tclass {
  public:
    tcollection();
    tcollection(const tcollection&);
    virtual ~tcollection();

    tcollection& operator=(const tcollection&);
    tclass* operator[](int);

    virtual insert(tclass * item);
    atinsert(int index, tclass *item);

    atremove(int index);                //remove from list
    removeitem(tclass * item);
    removeall();
    atdelete(int index);                //remove & delete
    deleteitem(tclass * item);
    deleteall();

    int indexof(tclass * item);         //-1 if item not in collection
    int count;

    setlimit(int alimit);

    error(int code, int info);
  protected:
    idxchk(int index);

    int limit;
    tclass **items;
};

#endif //CLASSES_H
