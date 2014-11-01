#ifndef _MESE_H
#define _MESE_H


void initmese(tstream &, tvesa &, int camera, int tracks, int ambient, int debug);

extern "C" {
void startmese(int frame);
//#pragma aux startmese "*" parm [eax] modify [eax ebx ecx edx esi edi]
}

void tempfree();

#endif //_MESE_H
