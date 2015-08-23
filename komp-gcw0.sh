#!/bin/sh
#CXFLAGS=-c\ -pipe\ -std=c++11\ -O3\ -DNO_FPU_CONTROL\ -DGCW0\ -ffast-math\ -fsingle-precision-constant\ -I./\ -I./include
CXFLAGS=-c\ -pipe\ -std=c++11\ -O3\ -DNO_FPU_CONTROL\ -DGCW0\ -fsingle-precision-constant\ -I./\ -I./include

rm *.o
/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS fulcrum.cpp
/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS vesac.cpp
/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS demo.cpp
/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS arj1.cpp
/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS picview.cpp
/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS inierz.cpp
/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS inicred.cpp
/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS inimese.cpp
/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS inimoove.cpp
/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS gfxstrio.cpp
/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS lw.cpp
/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS timecntr.cpp
/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS 2deye.cpp
/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS 2dlink.cpp
/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS bfade.cpp
/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS fire.cpp
/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS m2camera.cpp
/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS m2clip.cpp
/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS m2except.cpp
/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS m2lookup.cpp
/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS m2partic.cpp
/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS m2raw.cpp
/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS m2read.cpp
/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS m2render.cpp
/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS m2vector.cpp
/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS m2world.cpp

#CXFLAGS=-c\ -pipe\ -std=c++11\ -O3\ -DNO_FPU_CONTROL\ -DGCW0\ -ffast-math\ -Wall\ -Wextra\ -I./\ -I./include
CXFLAGS=-c\ -pipe\ -std=c++11\ -O3\ -DNO_FPU_CONTROL\ -DGCW0\ -Wall\ -Wextra\ -I./\ -I./include

/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS vesa.cc
/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS int.cc
/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS gmvasm.cc
/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS 2deffect.cc
/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS -Wno-maybe-uninitialized erz.cc
/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS tracks.cc
/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS credits.cc
/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS -Wno-maybe-uninitialized mese.cc
/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS gstrasm.cc
/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS m2ppro.cc
/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS m2serv.cc
/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS m2sort.cc
/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS m2dotpro.cc
/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS m2tracks.cc
/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS m2pman.cc
/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS m2pt.cc
/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS m2pttr.cc
/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS m2ptg.cc
/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS m2ptb.cc
/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS m2ptf.cc
/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS m2ptftr.cc
/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ $CXFLAGS -Wno-unused-parameter -Wno-literal-suffix -DMUSIC_DUMB rxm.cc -I../dumb-0.9.3/include

/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++ -o fulcrum *.o -lSDL -ldumb -L../dumb-0.9.3/lib/unix
