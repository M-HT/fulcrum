#ifndef _ERZ_H
#define _ERZ_H


void initerz(tstream &, tvesa &);

extern "C" {
void starterz();
#pragma aux starterz "*" modify [eax ebx ecx edx esi edi]
}

#endif //_ERZ_H
