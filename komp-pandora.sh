#!/bin/sh
CXFLAGS=-c\ -pipe\ -march=armv7-a\ -mcpu=cortex-a8\ -mtune=cortex-a8\ -mfpu=neon\ -mfloat-abi=softfp\ -std=c++11\ -O3\ -DPANDORA\ -DNO_FPU_CONTROL\ -ffast-math\ -fsingle-precision-constant\ -I./\ -I./include\ -I$PNDSDK/usr/include

rm *.o
$PNDSDK/bin/pandora-g++ $CXFLAGS fulcrum.cpp
$PNDSDK/bin/pandora-g++ $CXFLAGS vesac.cpp
$PNDSDK/bin/pandora-g++ $CXFLAGS demo.cpp
$PNDSDK/bin/pandora-g++ $CXFLAGS arj1.cpp
$PNDSDK/bin/pandora-g++ $CXFLAGS picview.cpp
$PNDSDK/bin/pandora-g++ $CXFLAGS inierz.cpp
$PNDSDK/bin/pandora-g++ $CXFLAGS inicred.cpp
$PNDSDK/bin/pandora-g++ $CXFLAGS inimese.cpp
$PNDSDK/bin/pandora-g++ $CXFLAGS inimoove.cpp
$PNDSDK/bin/pandora-g++ $CXFLAGS gfxstrio.cpp
$PNDSDK/bin/pandora-g++ $CXFLAGS lw.cpp
$PNDSDK/bin/pandora-g++ $CXFLAGS timecntr.cpp
$PNDSDK/bin/pandora-g++ $CXFLAGS 2deye.cpp
$PNDSDK/bin/pandora-g++ $CXFLAGS 2dlink.cpp
$PNDSDK/bin/pandora-g++ $CXFLAGS bfade.cpp
$PNDSDK/bin/pandora-g++ $CXFLAGS fire.cpp
$PNDSDK/bin/pandora-g++ $CXFLAGS m2camera.cpp
$PNDSDK/bin/pandora-g++ $CXFLAGS m2clip.cpp
$PNDSDK/bin/pandora-g++ $CXFLAGS m2except.cpp
$PNDSDK/bin/pandora-g++ $CXFLAGS m2lookup.cpp
$PNDSDK/bin/pandora-g++ $CXFLAGS m2partic.cpp
$PNDSDK/bin/pandora-g++ $CXFLAGS m2raw.cpp
$PNDSDK/bin/pandora-g++ $CXFLAGS m2read.cpp
$PNDSDK/bin/pandora-g++ $CXFLAGS m2render.cpp
$PNDSDK/bin/pandora-g++ $CXFLAGS m2vector.cpp
$PNDSDK/bin/pandora-g++ $CXFLAGS m2world.cpp

CXFLAGS=-c\ -pipe\ -march=armv7-a\ -mcpu=cortex-a8\ -mtune=cortex-a8\ -mfpu=neon\ -mfloat-abi=softfp\ -std=c++11\ -O3\ -DPANDORA\ -DNO_FPU_CONTROL\ -ffast-math\ -Wall\ -Wextra\ -I./\ -I./include\ -I$PNDSDK/usr/include

$PNDSDK/bin/pandora-g++ $CXFLAGS vesa.cc
$PNDSDK/bin/pandora-g++ $CXFLAGS int.cc
$PNDSDK/bin/pandora-g++ $CXFLAGS gmvasm.cc
$PNDSDK/bin/pandora-g++ $CXFLAGS 2deffect.cc
$PNDSDK/bin/pandora-g++ $CXFLAGS -Wno-maybe-uninitialized erz.cc
$PNDSDK/bin/pandora-g++ $CXFLAGS tracks.cc
$PNDSDK/bin/pandora-g++ $CXFLAGS credits.cc
$PNDSDK/bin/pandora-g++ $CXFLAGS -Wno-maybe-uninitialized -Wno-uninitialized mese.cc
$PNDSDK/bin/pandora-g++ $CXFLAGS gstrasm.cc
$PNDSDK/bin/pandora-g++ $CXFLAGS m2ppro.cc
$PNDSDK/bin/pandora-g++ $CXFLAGS m2serv.cc
$PNDSDK/bin/pandora-g++ $CXFLAGS m2sort.cc
$PNDSDK/bin/pandora-g++ $CXFLAGS m2dotpro.cc
$PNDSDK/bin/pandora-g++ $CXFLAGS m2tracks.cc
$PNDSDK/bin/pandora-g++ $CXFLAGS m2pman.cc
$PNDSDK/bin/pandora-g++ $CXFLAGS m2pt.cc
$PNDSDK/bin/pandora-g++ $CXFLAGS m2pttr.cc
$PNDSDK/bin/pandora-g++ $CXFLAGS m2ptg.cc
$PNDSDK/bin/pandora-g++ $CXFLAGS m2ptb.cc
$PNDSDK/bin/pandora-g++ $CXFLAGS m2ptf.cc
$PNDSDK/bin/pandora-g++ $CXFLAGS m2ptftr.cc
$PNDSDK/bin/pandora-g++ $CXFLAGS -Wno-unused-parameter -DMUSIC_BASS rxm.cc

$PNDSDK/bin/pandora-g++ -s -o fulcrum *.o -lSDL -lts -lbass -L$PNDSDK/usr/lib -L./bass/lib/arm
