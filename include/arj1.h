#ifndef _ARJ_H
#define _ARJ_H

#define arjbuflen      26624

void make_crctable();
long find_header(FILE *fd);
int read_header(FILE *fd);
void get_filename(char *, int);
int decode(FILE *, unsigned char *);


#endif 
