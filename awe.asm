include switches.inc

if pmw gt 0

.386                                    ;pmode/w
.model flat,prolog
.stack 1000h
else

.386p
code32  segment para public use32       ;tran
        assume cs:code32, ds:code32

endif

locals



include int.inc
;rxm
extrn rxmdata:dword, tick:near, foreach:near

public a_test


b equ byte ptr
w equ word ptr

;include d:\sprachen\c\h\debug.inc
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
;struc
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
include rxmstruc.inc

tadata struc
 a_info         tdinfo ?

 a_mem          dd ?
 a_slen         dd ?
 a_extralen     dd ?
 a_memleft      dd ?
 a_step         dd ?
; a_loope        dd ?
 a_writeport    dw ?
 a_div          db ?

 align 4
ends


if pmw gt 0
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
.data
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
endif

MIXER_INDEX      =  4
MIXER_DATA       =  5

;a tadata ?

a_volumes label byte
;include vol.dat
db 0,64,72,77,80,83,85,87,88,90,91,92,93,94,95,96
db 96,97,98,98,99,99,100,101,101,101,102,102,103,103,104,104
db 104,105,105,105,106,106,106,107,107,107,108,108,108,108,109,109
db 109,109,110,110,110,110,110,111,111,111,111,111,112,112,112,112
db 112,113,113,113,113,113,113,114,114,114,114,114,114,115,115,115
db 115,115,115,115,116,116,116,116,116,116,116,116,117,117,117,117
db 117,117,117,117,118,118,118,118,118,118,118,118,118,119,119,119
db 119,119,119,119,119,119,119,120,120,120,120,120,120,120,120,120
db 120,121,121,121,121,121,121,121,121,121,121,121,121,122,122,122
db 122,122,122,122,122,122,122,122,122,122,123,123,123,123,123,123
db 123,123,123,123,123,123,123,123,124,124,124,124,124,124,124,124
db 124,124,124,124,124,124,124,125,125,125,125,125,125,125,125,125
db 125,125,125,125,125,125,125,126,126,126,126,126,126,126,126,126
db 126,126,126,126,126,126,126,126,126,127,127,127,127,127,127,127
db 127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127
db 127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127
db 127

a_freq label dword
;include freq.dat
dd 0,12,23,35,46,57,69,80,92,103,114,126,137,148,159,171
dd 182,193,204,215,226,237,248,260,271,282,293,304,314,325,336,347
dd 358,369,380,391,401,412,423,434,444,455,466,476,487,498,508,519
dd 529,540,550,561,571,582,592,603,613,623,634,644,655,665,675,685
dd 696,706,716,726,737,747,757,767,777,787,797,807,817,827,838,848
dd 858,867,877,887,897,907,917,927,937,947,956,966,976,986,996,1005
dd 1015,1025,1034,1044,1054,1063,1073,1083,1092,1102,1111,1121,1130,1140,1149,1159
dd 1168,1178,1187,1197,1206,1216,1225,1234,1244,1253,1262,1272,1281,1290,1299,1309
dd 1318,1327,1336,1346,1355,1364,1373,1382,1391,1400,1410,1419,1428,1437,1446,1455
dd 1464,1473,1482,1491,1500,1509,1518,1527,1535,1544,1553,1562,1571,1580,1589,1597
dd 1606,1615,1624,1632,1641,1650,1659,1667,1676,1685,1693,1702,1711,1719,1728,1737
dd 1745,1754,1762,1771,1779,1788,1796,1805,1813,1822,1830,1839,1847,1856,1864,1873
dd 1881,1889,1898,1906,1914,1923,1931,1939,1948,1956,1964,1972,1981,1989,1997,2005
dd 2014,2022,2030,2038,2046,2055,2063,2071,2079,2087,2095,2103,2111,2119,2127,2135
dd 2143,2151,2159,2167,2175,2183,2191,2199,2207,2215,2223,2231,2239,2247,2255,2263
dd 2270,2278,2286,2294,2302,2310,2317,2325,2333,2341,2349,2356,2364,2372,2379,2387
dd 2395,2403,2410,2418,2426,2433,2441,2448,2456,2464,2471,2479,2486,2494,2502,2509
dd 2517,2524,2532,2539,2547,2554,2562,2569,2577,2584,2591,2599,2606,2614,2621,2629
dd 2636,2643,2651,2658,2665,2673,2680,2687,2695,2702,2709,2717,2724,2731,2738,2746
dd 2753,2760,2767,2775,2782,2789,2796,2803,2811,2818,2825,2832,2839,2846,2853,2860
dd 2868,2875,2882,2889,2896,2903,2910,2917,2924,2931,2938,2945,2952,2959,2966,2973
dd 2980,2987,2994,3001,3008,3015,3022,3029,3036,3042,3049,3056,3063,3070,3077,3084
dd 3091,3097,3104,3111,3118,3125,3131,3138,3145,3152,3158,3165,3172,3179,3185,3192
dd 3199,3206,3212,3219,3226,3232,3239,3246,3252,3259,3266,3272,3279,3286,3292,3299
dd 3305,3312,3318,3325,3332,3338,3345,3351,3358,3364,3371,3377,3384,3390,3397,3403
dd 3410,3416,3423,3429,3436,3442,3449,3455,3461,3468,3474,3481,3487,3493,3500,3506
dd 3513,3519,3525,3532,3538,3544,3551,3557,3563,3570,3576,3582,3588,3595,3601,3607
dd 3614,3620,3626,3632,3639,3645,3651,3657,3663,3670,3676,3682,3688,3694,3700,3707
dd 3713,3719,3725,3731,3737,3743,3750,3756,3762,3768,3774,3780,3786,3792,3798,3804
dd 3810,3816,3823,3829,3835,3841,3847,3853,3859,3865,3871,3877,3883,3889,3895,3901
dd 3906,3912,3918,3924,3930,3936,3942,3948,3954,3960,3966,3972,3977,3983,3989,3995
dd 4001,4007,4013,4019,4024,4030,4036,4042,4048,4053,4059,4065,4071,4077,4082,4088

a_vmt  tsvmt <a_init, a_done, a_setvol, a_play, a_bpm>;, 0>

if pmw gt 0
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
.code
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
endif

comment #
a_getport proc near
        ;esi -> trxmdata
        ;es:edi -> environment
        mov     eax,[esi].sb_i.d_base
        mov     [esi].a_i.d_base,eax

@@weg:  ret
a_getport endp
#

a_test proc near
        ;-> edi -> tdinfo
        ;<- eax = result

        mov     dx,7800h
        call    a_readw
        and     al,0Ch
        cmp     al,0Ch
        jne     @@err

        mov     dx,141Dh
        call    a_readw
        and     al,7Eh
        cmp     al,58h
        jne     @@err

        mov     dx,141Eh
        call    a_readw
        and     al,03h
        cmp     al,03h
        jne     @@err

        mov     [edi].d_mem,size tadata
        lea     eax,a_vmt
        mov     [edi].d_vmt,eax

        xor     eax,eax
        inc     eax
        jmp     @@weg
@@err:  xor     eax,eax
@@weg:  ret
a_test endp

a_init proc near
        ;ebx -> trxmdata
        ;edi -> twdata

                ;incval = f * (65536 / rate)*16
        mov     [ebx].s_freqmul,398915783;(65536 / 44100 *16) *2^24


                ;init envelope engine
        xor     dl,dl
@@l0:
        xor     eax,eax
        mov     dh,28h                  ;envelope 1 to pitch & filter
        call    a_writew
        mov     dh,64h                  ;envelope 1 delay
        call    a_writew
        mov     dh,66h                  ;envelope 1 hold & attack
        call    a_writew
        mov     dh,74h                  ;envelope 1 sustain & decay
        call    a_writew
        mov     dh,44h                  ;envelope 2 delay
        call    a_writew
        mov     dh,46h                  ;envelope 2 hold & attack
        call    a_writew
        mov     dh,54h                  ;envelope 2 sustain & decay
        mov     ax,0807Fh
        call    a_writew
        mov     dh,08h                  ;pitch
        mov     ax,0E000h
        call    a_writew
        mov     dh,18h                  ;filter cutoff & volume
        mov     ax,0FF00h
        call    a_writew
        mov     dh,38h                  ;LFO1 to pitch & filter
        xor     eax,eax
        call    a_writew
        mov     dh,48h                  ;LFO1 to volume & LFO1 frequnecy
        mov     eax,18h
        call    a_writew
        mov     dh,58h                  ;LFO2 to pitch & LFO2 frequency
        call    a_writew
        mov     dh,68h                  ;???
        xor     eax,eax
        call    a_writew
        mov     dh,76h                  ;LFO2 delay
        call    a_writew
        mov     dh,56h                  ;LFO1 delay
        call    a_writew

        inc     dl
        cmp     dl,32
        jb      @@l0


        mov     cx,2
        call    a_wait

                ;init sound engine
        xor     dl,dl
@@l1:
        mov     dh,10h                  ;??? & reverb send & ???
        xor     eax,eax
        call    a_writed
        mov     dh,30h                  ;overall volume & overall filter cutoff
        mov     eax,0FFFFh
        call    a_writed
        mov     dh,60h                  ;pan & loop start
        xor     eax,eax
        call    a_writed
        mov     dh,70h                  ;chorus & loop end
        call    a_writed
        mov     dh,00h                  ;???
        call    a_writed
        mov     dh,20h                  ;???
        mov     eax,0FFFFh
        call    a_writed
        mov     dh,04h                  ;FilterQ & DramControl & PlayPosition
        xor     eax,eax
        call    a_writed
        mov     dh,50h                  ;???
        call    a_writed
        mov     dh,40h                  ;???
        call    a_writed

        inc     dl
        cmp     dl,32
        jb      @@l1

                ;init effects engine

;        xor     eax,eax
;        mov     dx,1414h                ;read offset = 0
;        call    a_writew
;        inc     dl                      ;? = 0
;        call    a_writew
;        inc     dl                      ;write offset = 0
;        call    a_writew
;        dec     dl                      ;? = 0
;        call    a_writew


;  InitEffect(EffectData);
;  AWE32Wait(0x400);
;  InitEffect2(EffectData);
;  for(k=0;k<0x14;++k)
;    AWE32RegDW(0x20+k,0xA20,0);
;  InitEffect2(EffectData2);

                ;init DRAM refresh
        mov     dx,541Eh                ;EG2 sustain & decay
        mov     ax,80h
        call    a_writew
        mov     dh,60h                  ;pan & loop start
        mov     eax,0FFFFFFE0h
        call    a_writed
        mov     dh,70h                  ;chorus send & loop end
        mov     eax,0FFFFE8h
        call    a_writed
        mov     dh,10h                  ;??? & reverb send & ???
        xor     eax,eax
        call    a_writed
        mov     dh,00h                  ;???
        xor     eax,eax
        call    a_writed
        mov     dh,04h                  ;Filter & dram control & playpos
        mov     eax,0FFFFE3h
        call    a_writed

        mov     dx,541Fh                ;EG2 sustain & decay
        mov     ax,80h
        call    a_writew
        mov     dh,60h                  ;pan & loop start
        mov     eax,0FFFFFFF0h
        call    a_writed
        mov     dh,70h                  ;chorus send & loop end
        mov     eax,0FFFFF8h
        call    a_writed
        mov     dh,10h                  ;??? & reverb send & ???
        mov     eax,0FFh
        call    a_writed
        mov     dh,00h                  ;???
        mov     eax,08000h
        call    a_writed
        mov     dh,04h                  ;Filter & dram control & playpos
        mov     eax,0FFFFF3h
        call    a_writed


        mov     dx,101Eh                ;??? & reverb send & ???
        xor     eax,eax
        call    a_writew

        mov     edx,[edi].d_base
        add     edx,0E22h - 220h
@@w0:
        in      ax,dx
        test    ah,10
        jz      @@w0
@@w1:
        in      ax,dx
        test    ah,10
        jnz     @@w1

        mov     edx,[edi].d_base
        add     edx,622h - 220h
        mov     ax,4828h
        out     dx,ax

        mov     dx,301Eh                ;overall volume & overall filter cutoff
        mov     eax,0FFFFFFFFh
        call    a_writed
        inc     dl
        call    a_writed


        mov     edx,[edi].d_base
        add     dl,MIXER_INDEX

        mov     ah,[edi].d_mastervol    ;set master volume
        or      ah,ah
        jz      @@vol0

        mov     al,30h
        out     dx,ax
        inc     al
        out     dx,ax
        mov     ah,0FFh
        inc     al
        out     dx,ax
        inc     al
        out     dx,ax
@@vol0:

     xor eax,eax

        ret
a_init endp

a_done proc near



        ret
a_done endp

a_setvol proc near



        ret
a_setvol endp




a_play proc near
        ;edi -> tadata
        ;esi -> rxmdata

        call    a_enabledram
        call    a_checkdram
;   mov eax,[edi].a_mem
        call    a_xm2mem

        call    a_disabledram

        ret
a_play endp


a_bpm proc near
        ;edi -> rxmdata
        ;eax = frequenz*65536
        mov     edx,offset a_int
        call    newhandler
        ret
a_bpm endp

;-----------------------------------------------------------------------------

a_setidx proc near
        ;-> edx = index/port
        push    eax
        push    edx

        mov     eax,edx
        and     eax,0F000h
        shr     eax,12-5
        or      al,dl
        mov     edx,[edi].d_base
        add     edx,0E22h - 220h
        out     dx,ax

        pop     edx

        mov     dl,dh
        and     edx,0C02h
        add     edx,[edi].d_base
        add     edx,620h - 220h

        pop     eax

        ret
a_setidx endp

a_writew proc near
        ;-> ax = value
        ;-> edx = index/port

        push    edx
        call    a_setidx
        out     dx,ax
        pop     edx

        ret
a_writew endp

a_writed proc near
        ;-> eax = value
        ;-> edx = index/port

        push    edx

        call    a_setidx

        push    eax
        out     dx,ax
        shr     eax,16
        add     dl,2
        out     dx,ax
        pop     eax

        pop     edx

        ret
a_writed endp

a_readw proc near
        ;-> edx = index/port
        ;<- ax = value

        push    edx
        call    a_setidx
        in      ax,dx
        pop     edx
        ret
a_readw endp

a_wait proc near
        ;-> cx = time
        mov     edx,161Bh
        call    a_readw
        mov     bx,ax
@@l:
        call    a_readw
        sub     ax,bx
        cmp     ax,cx
        jb      @@l

@@weg:  ret
a_wait endp

a_enabledram proc near

        mov     dx,141Eh                ;enable DRAM access
        mov     ax,20h
        call    a_writew

        xor     dl,dl
@@l:
        mov     dh,54h                  ;EG2 sustain & decay
        mov     ax,80h
        call    a_writew
        mov     dh,30h                  ;overall volume & overall filter cutoff
        xor     eax,eax
        call    a_writed
        mov     dh,20h                  ;???
        call    a_writed
        mov     dh,60h                  ;pan & loop start
        call    a_writed
        mov     dh,70h                  ;chorus send & loop end
        call    a_writed
        mov     dh,10h                  ;??? & reverb send & ???
        mov     eax,40000000h
        call    a_writed
        mov     dh,00h                  ;???
        call    a_writed
        mov     dh,04h                  ;Filter & dram control & playpos
        or      dl,dl
        setnz   al
        or      al,2
        shl     eax,25
        call    a_writed

        inc     dl
        cmp     dl,30
        jb      @@l

@@weg:  ret
a_enabledram endp


a_disabledram proc near

        xor     dl,dl
@@l:
        mov     dh,04h                  ;Filter & dram control & playpos
        xor     eax,eax
        call    a_writed
        mov     dh,54h                  ;EG2 sustain & decay
        mov     ax,807Fh
        call    a_writew

        inc     dl
        cmp     dl,30
        jb      @@l

@@weg:  ret
a_disabledram endp


a_checkdram proc near

        mov     dx,1416h                ;address for writing
        mov     eax,0200000h
        mov     a_mem[edi],eax
        call    a_writed
        mov     dx,141Ah                ;memory read/write data
        mov     ax,1234h
        call    a_writew
        mov     ax,7777h
        call    a_writew

@@l:

        mov     cx,2
        call    a_wait
        mov     dx,1414h                ;address for reading
        mov     eax,0200000h
        call    a_writed

        mov     dx,141Ah                ;memory read/write data
        call    a_readw                 ;skip first word
        call    a_readw
        cmp     ax,1234h
        jne     @@weg
        call    a_readw
        cmp     ax,7777h
        jne     @@weg

        add     [edi].a_mem,8000h
        mov     eax,[edi].a_mem
        cmp     eax,16*100000h
        jae     @@weg

        mov     dx,1416h                ;address for writing
        call    a_writed
        mov     dx,141Ah
        mov     ax,0FFFFh
        call    a_writew                ;memory read/write data

        jmp     @@l

@@weg:  ret
a_checkdram endp

;-

comment #
a_testsample proc near

        mov     dx,1416h
        mov     eax,0200000h
        call    a_writed

        mov     ecx,65536
@@0:
        mov     dx,141Ah
        mov     ah,cl
        call    a_writew
        dec     ecx
        jnz     @@0

@@weg:  ret
a_testsample endp

a_testnote proc near
        xor     dl,dl

        mov     dh,08h                  ;pitch
        mov     ax,57344
        call    a_writew

        mov     dh,60h                  ;loopstart
        mov     eax,200100h
        call    a_writed
        mov     dh,70h                  ;loopend
        mov     eax,20FF00h
        call    a_writed
        mov     dh,04                   ;play pos.
        mov     eax,200100h
        call    a_writed

        mov     dh,54h                  ;envelope 2 gus-ramp & dest-vol & speed (sustain & decay)
        mov     ax,0FF03h
        call    a_writew


        mov     ebx,65536/4;*16            ;frequenz (65536*16 = 44100Hz)

@@t:
        bsr     ecx,ebx
        sub     ecx,9
      mov eax,ebx
        shr     eax,cl

        mov     eax,a_freq[(eax-512)*4]
        shl     ecx,12                  ;*4096
        lea     eax,[eax+ecx+(57344-45056)]

        mov     dh,08h                  ;pitch
        call    a_writew

        imul    ebx,271
        shr     ebx,8
        jmp     @@t

@@weg:  ret
a_testnote endp

a_noteoff proc near
        xor     dl,dl

        mov     dh,54h
        mov     ax,0807Fh
        call    a_writew
        mov     dh,74h
        call    a_writew

@@weg:  ret
a_noteoff endp
#


a_xm2mem proc near
        ;-> edi -> tadata
        ;-> esi -> trxmdata

        mov     a_slen[edi],0
        mov     a_extralen[edi],0
        mov     edx,offset a_sizecount
        call    foreach

        mov     ebp,0200000h            ;ebp = ram-offset
        mov     eax,a_mem[edi]
        sub     eax,ebp
;    mov eax,020000h
        mov     a_memleft[edi],eax

        sub     eax,a_extralen[edi]
        mov     edx,a_slen[edi]

        mov     a_div[edi],-1           ;faktor bestimmen
@@div_l:
        inc     a_div[edi]
        shr     edx,1
        cmp     edx,eax
        jl      @@0
        cmp     a_div[edi],4
        jb      @@div_l
@@0:
                                        ;samples ins ram laden

        mov     dx,1416h
        mov     eax,ebp
        call    a_writed

        mov     dx,141Ah                ;write data
        call    a_setidx                ;dx = write-port
        mov     a_writeport[edi],dx

        mov     edx,offset a_resample
        call    foreach



@@weg:  ret
a_xm2mem endp

a_len proc near
        ;-> esi -> tsampleh
        ;<- eax = sample-l„nge
        ;<- ebx = extra-l„nge

        mov     eax,sLoope[esi]         ;eax = sample-l„nge ohne extra-bytes
        mov     ebx,3                   ;ebx = extra-bytes
        test    sType[esi],sfLoop
        jnz     @@0
                ;no loop
        mov     ebx,8
        jmp     @@1
@@0:            ;loop
        test    sType[esi],sfPingpong
        jz      @@1
                ;pingpong
        add     eax,eax
        sub     eax,sLoops[esi]
@@1:

        cmp     eax,256
        ja      @@weg
        add     ebx,eax                 ;sample krzer als 256
        xor     eax,eax
@@weg:  ret
a_len endp

;called by foreach
a_sizecount proc near
        ;-> edi -> tadata
        ;-> esi -> tsampleh

        call    a_len
        add     a_slen[edi],eax         ;eax = sample-l„nge
        add     a_extralen[edi],ebx     ;ebx = extra-l„nge
@@weg:  ret
a_sizecount endp

;called by foreach
a_resample proc near
        ;-> edi -> tadata
        ;-> esi -> tsampleh
        ;-> ebp = ram-offset

        mov     cl,a_div[edi]
        mov     eax,a_slen[edi]
        shr     eax,cl
        add     eax,a_extralen[edi]     ;eax = aktueller platzbedarf
        cmp     eax,a_memleft[edi]
        jle     @@0
        inc     cl                      ;platzbedarf zu groá
@@0:
;    or cl,cl
;    jz @@t0
;   mov eax,'res '
;   call str_
;@@t0:

        call    a_len                   ;eax = sample-l„nge
                                        ;ebx = extra-l„nge

        sub     a_slen[edi],eax
        sub     a_extralen[edi],ebx

        shr     eax,cl                  ;sample-l„nge = 0: nicht komprimierbar
        jnz     @@1
        xor     cl,cl
@@1:

        add     eax,ebx                 ;eax = gesamter platzbedarf
        sub     a_memleft[edi],eax
        mov     sOffset[esi],-1
;     jns @@t0
;     call hex_
;     jmp      @@weg
        js      @@weg                   ;mem voll
;@@t0:
        mov     sOffset[esi],ebp
        mov     sResample[esi],cl

        mov     eax,1
        shl     eax,cl
        mov     a_step[edi],eax

        shr     sLoops[esi],cl
        shr     sLoope[esi],cl

        test    sType[esi],sfLoop
        jnz     @@loop

                ;no loop

        mov     eax,sLoope[esi]          ;eax = samples to copy
        mov     sLoops[esi],eax
        add     sLoope[esi],5

        xor     ebx,ebx                  ;ebx = offset
        mov     edx,a_step[edi]          ;edx = step
        call    a_move

        mov     eax,8
        sub     ebx,edx
        xor     edx,edx
        call    a_move

        jmp     @@weg

@@loop:         ;loop

                ;forward
        mov     eax,sLoope[esi]          ;eax = samples to copy
        xor     ebx,ebx                  ;ebx = offset
        mov     edx,a_step[edi]          ;edx = step
        call    a_move


        test    sType[esi],sfPingpong
        jz      @@l3


        mov     eax,sLoope[esi]          ;rckw„rts
        sub     eax,sLoops[esi]
        add     sLoope[esi],eax
        sub     ebx,edx
        inc     ebx
        neg     edx
        call    a_move

        neg     edx
@@l3:
        mov     eax,3                   ;letzte 3 samples
        mov     ebx,sLoops[esi]
        shl     ebx,cl
        call    a_move


@@weg:  ret
a_resample endp


a_move proc near
        ;-> eax = samples to copy
        ;-> ebx = offset
        ;-> edx = step
        ;<- ebx = offset+samples*step
        push    ecx
        mov     ecx,edx
        mov     dx,[edi].a_writeport

        add     ebp,eax
        push    ebp
        mov     ebp,eax

        push    esi
        test    sType[esi],sf16bit
        mov     esi,[esi].sDataptr
        jnz     @@16
                ;8bit
        xor     al,al
@@8_l:
        mov     ah,[esi+ebx]
        add     ebx,ecx
        out     dx,ax
        dec     ebp
        jnz     @@8_l
        jmp     @@0
@@16:           ;16bit
@@16_l:
        mov     ax,[esi+ebx*2]
        add     ebx,ecx
        out     dx,ax
        dec     ebp
        jnz     @@16_l
@@0:
        pop     esi
        mov     edx,ecx
        pop     ebp ecx

        ret
a_move endp
;---


a_int proc near
        mov     edi,rxmdata

        mov     eax,4
        mov     edx,offset a_change
        call    newhandler              ;4 ms one-shot

        call    tick

;---
                                        ;fr regul„re „nderung und stop
                                        ; nicht fr start
        lea     esi,[edi].chdata
        mov     dl,b [edi].head.hdChannels
        mov     edi,[edi].s_drvmem
        dec     dl
@@ch_l:
        ;lautst„rke-ramping
        test    caktiv[esi],afPlay or afStop
        jz      @@l_weg                 ;kein ramping n”tig

                ;ziel-lautst„rke
        mov     ah,0
        test    caktiv[esi],afStop      ;wird stimme gleich gestoppt?
        jnz     @@ramp

                ;stimme l„uft weiter
        mov     ebx,cfinalvol[esi]      ;0-256
        mov     ah,a_volumes[ebx]

@@ramp:
        mov     dh,54h                  ;envelope 2 gus-ramp & dest-vol & speed (sustain & decay)
        mov     al,7Fh
        or      ah,80h
        call    a_writew

@@l_weg:

        add     esi,size tchannel
        dec     dl                      ;alle kan„le...
        jns     @@ch_l

        ret
a_int endp



a_change proc near
        mov     edi,rxmdata

        lea     esi,[edi].chdata
        mov     dl,b [edi].head.hdChannels
        mov     edi,[edi].s_drvmem
        dec     dl
@@ch_l:
        test    caktiv[esi],afStart or afPlay or afStop
        jz      @@l_weg                 ;keine aktion erforderlich

        ;stimme stoppen
        test    caktiv[esi],afStop
        jz      @@aktiv

        test    caktiv[esi],afStart     ;gleich wieder starten?
        jz      @@l_weg

@@aktiv:;„nderung der stimme (kanal aktiv)

                ;frequenz
        mov     ebx,cincval[esi]
        bsr     ecx,ebx
        sub     ecx,9
        shr     ebx,cl
;     xor eax,eax
;     cmp ebx,512
;     jl @@t0
        mov     eax,a_freq[(ebx-512)*4]
;@@t0:
        shl     ecx,12                  ;*4096
        lea     eax,[eax+ecx+(57344-45056)]
;cmp eax,65536
;jb  @@ofl
;mov eax,'ofl!'
;call str_
;@@ofl:
        mov     dh,08h                  ;pitch
        call    a_writew

                ;panning
        mov     dh,60h                  ;panning & loopstart
        mov     al,b cfinalpan[esi]
        not     al
        shl     eax,24
        or      eax,cloops[esi]
        call    a_writed


        ;stimme starten
        test    caktiv[esi],afStart
        jz      @@l_weg

                ;Loop-Ende setzen
        mov     dh,70h                  ;chorus & loopend
        mov     eax,cloope[esi]
        call    a_writed

                ;Stimmenanfang setzen
        mov     dh,04                   ;play pos.
        mov     eax,csample[esi]
        call    a_writed

                ;volume ramping
        mov     dh,54h                  ;envelope 2 gus-ramp & dest-vol & speed (sustain & decay)
        mov     ebx,cfinalvol[esi]      ;0-256
        mov     ah,a_volumes[ebx]
        or      ah,80h
        mov     al,7Fh
        call    a_writew                ;stimme jetzt h”rbar...
@@l_weg:

        add     esi,size tchannel
        dec     dl                      ;alle kan„le...
        jns     @@ch_l
@@weg:  ret
a_change endp


comment #
start:
        jmp     cmain
        db      'WATCOM'
cmain:
        sti
        cld
        push    ds
        pop     es

        lea     edi,a
        mov     [edi].d_base,220h
        mov     [edi].a_mem,0

        call    a_init

        call    a_enabledram
        call    a_checkdram
        call    a_testsample
        call    a_disabledram

        call    a_testnote
        call    a_noteoff

        mov     ax,4c00h
        int     21h
#

end