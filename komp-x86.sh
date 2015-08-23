#!/bin/sh
CXFLAGS=-c\ -m32\ -mfpmath=387\ -std=c++11\ -O3\ -DNO_FPU_CONTROL\ -ffast-math\ -fsingle-precision-constant\ -I./\ -I./include

rm *.o
g++ $CXFLAGS fulcrum.cpp
g++ $CXFLAGS vesac.cpp
g++ $CXFLAGS demo.cpp
g++ $CXFLAGS arj1.cpp
g++ $CXFLAGS picview.cpp
g++ $CXFLAGS inierz.cpp
g++ $CXFLAGS inicred.cpp
g++ $CXFLAGS inimese.cpp
g++ $CXFLAGS inimoove.cpp
g++ $CXFLAGS gfxstrio.cpp
g++ $CXFLAGS lw.cpp
g++ $CXFLAGS timecntr.cpp
g++ $CXFLAGS 2deye.cpp
g++ $CXFLAGS 2dlink.cpp
g++ $CXFLAGS bfade.cpp
g++ $CXFLAGS fire.cpp
g++ $CXFLAGS m2camera.cpp
g++ $CXFLAGS m2clip.cpp
g++ $CXFLAGS m2except.cpp
g++ $CXFLAGS m2lookup.cpp
g++ $CXFLAGS m2partic.cpp
g++ $CXFLAGS m2raw.cpp
g++ $CXFLAGS m2read.cpp
g++ $CXFLAGS m2render.cpp
g++ $CXFLAGS m2vector.cpp
g++ $CXFLAGS m2world.cpp

CXFLAGS=-c\ -m32\ -mfpmath=387\ -std=c++11\ -O3\ -DNO_FPU_CONTROL\ -ffast-math\ -Wall\ -Wextra\ -I./\ -I./include

g++ $CXFLAGS vesa.cc
g++ $CXFLAGS int.cc
g++ $CXFLAGS gmvasm.cc
g++ $CXFLAGS 2deffect.cc
g++ $CXFLAGS -Wno-maybe-uninitialized erz.cc
g++ $CXFLAGS tracks.cc
g++ $CXFLAGS credits.cc
g++ $CXFLAGS -Wno-maybe-uninitialized mese.cc
g++ $CXFLAGS gstrasm.cc
g++ $CXFLAGS m2ppro.cc
g++ $CXFLAGS m2serv.cc
g++ $CXFLAGS m2sort.cc
g++ $CXFLAGS m2dotpro.cc
g++ $CXFLAGS m2tracks.cc
g++ $CXFLAGS m2pman.cc
g++ $CXFLAGS m2pt.cc
g++ $CXFLAGS m2pttr.cc
g++ $CXFLAGS m2ptg.cc
g++ $CXFLAGS m2ptb.cc
g++ $CXFLAGS m2ptf.cc
g++ $CXFLAGS m2ptftr.cc
g++ $CXFLAGS -Wno-unused-parameter -DMUSIC_BASS rxm.cc

g++ -m32 -o fulcrum *.o -lSDL -lbass -L./bass/lib/x86
