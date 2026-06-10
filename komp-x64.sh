#!/bin/sh
CXFLAGS=-c\ -pipe\ -std=c++11\ -O3\ -DNO_FPU_CONTROL\ -ffast-math\ -fsingle-precision-constant\ -I./\ -I./include\ -I/usr/include\ -I/usr/local/include

rm *.o
c++ $CXFLAGS fulcrum.cpp
c++ $CXFLAGS vesac.cpp
c++ $CXFLAGS demo.cpp
c++ $CXFLAGS arj1.cpp
c++ $CXFLAGS picview.cpp
c++ $CXFLAGS inierz.cpp
c++ $CXFLAGS inicred.cpp
c++ $CXFLAGS inimese.cpp
c++ $CXFLAGS inimoove.cpp
c++ $CXFLAGS gfxstrio.cpp
c++ $CXFLAGS lw.cpp
c++ $CXFLAGS timecntr.cpp
c++ $CXFLAGS 2deye.cpp
c++ $CXFLAGS 2dlink.cpp
c++ $CXFLAGS bfade.cpp
c++ $CXFLAGS fire.cpp
c++ $CXFLAGS m2camera.cpp
c++ $CXFLAGS m2clip.cpp
c++ $CXFLAGS m2except.cpp
c++ $CXFLAGS m2lookup.cpp
c++ $CXFLAGS m2partic.cpp
c++ $CXFLAGS m2raw.cpp
c++ $CXFLAGS m2read.cpp
c++ $CXFLAGS m2render.cpp
c++ $CXFLAGS m2vector.cpp
c++ $CXFLAGS m2world.cpp

CXFLAGS=-c\ -pipe\ -std=c++11\ -O3\ -DNO_FPU_CONTROL\ -ffast-math\ -Wall\ -Wextra\ -I./\ -I./include\ -I/usr/include\ -I/usr/local/include

c++ $CXFLAGS vesa.cc
c++ $CXFLAGS int.cc
c++ $CXFLAGS gmvasm.cc
c++ $CXFLAGS 2deffect.cc
c++ $CXFLAGS -Wno-maybe-uninitialized erz.cc
c++ $CXFLAGS tracks.cc
c++ $CXFLAGS credits.cc
c++ $CXFLAGS -Wno-maybe-uninitialized mese.cc
c++ $CXFLAGS gstrasm.cc
c++ $CXFLAGS m2ppro.cc
c++ $CXFLAGS m2serv.cc
c++ $CXFLAGS m2sort.cc
c++ $CXFLAGS m2dotpro.cc
c++ $CXFLAGS m2tracks.cc
c++ $CXFLAGS m2pman.cc
c++ $CXFLAGS m2pt.cc
c++ $CXFLAGS m2pttr.cc
c++ $CXFLAGS m2ptg.cc
c++ $CXFLAGS m2ptb.cc
c++ $CXFLAGS m2ptf.cc
c++ $CXFLAGS m2ptftr.cc
c++ $CXFLAGS -Wno-unused-parameter -DMUSIC_BASS rxm.cc

c++ -pipe -o fulcrum *.o -lSDL3 -lbass -L./bass/lib/x64
