include switches.inc

if pmw gt 0

.386                                    ;pmode/w
.model flat,prolog

extrn _seldata:word
extrn _setirqmask:near
else

.386p
code32  segment para public use32       ;tran
        assume cs:code32, ds:code32

endif

locals


include int.inc
;rxm
extrn rxmdata:dword, tick:near, foreach:near

public sb_test

;fÅr wav
public mixcount, mix16_s, startsample


b equ byte ptr
w equ word ptr

;include C:\SPRACHEN\C\H\debug.inc
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
;struc
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
include rxmstruc.inc

tsbdata struc
 sb_info        tdinfo ?

 sb_dmapos      dd ?            ;aktuelle dma-position
 sb_mixvor      dd ?            ;vorsprung des mixers in byte
 sb_mixpos      dd ?

 sb_dosamples   dd ?            ;samples noch zu berechnen
 sb_csamples    dd ?            ;samples pro kanal
 sb_donesamples dd ?            ;schon berechnete samples

 sb_chancnt     dd ?
 sb_aktchdata   dd ?

 sb_tsamples    dd ?
; sb_tile        dd ?

 sb_copy        dd ?
 sb_mix         dd ?

 sb_intflag     db ?
 sb_intDSP      db ?
 sb_dmanum      db ?            ;nummer des benutzten dma
 sb_dmashr      db ?
 align 4
 sb             tmix ?

;
 sb_wavsize     dd ?
 sb_wavpos      dd ?

 align 4
ends


if pmw gt 0
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
.data
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
endif

;sb_num = 2


;8 bit mixing mono
;8 bit mixing stereo
;8 bit mixing mono fÅr 16 bit ausgabe
;8 bit mixing stereo fÅr 16 bit ausgabe
;16 bit mixing stereo fÅr 16 bit ausgabe


DSP_RESET        = 06h
DSP_READ_DATA    = 0Ah
DSP_WRITE        = 0Ch
DSP_DATA_AVAIL   = 0Eh

MIXER_INDEX      =  4
MIXER_DATA       =  5

DSP1xx  = 1
DSP200  = 2
DSP201  = 3
DSP3xx  = 4
DSP4xx  = 5

pageport db 87h,83h,81h,82h,8Fh,8Bh,89h,8Ah

;sb int
if pmw gt 0
oldint     df ?
else
opmirq     dd ?
ormirq     dd ?
rmirqbuf   db 21 dup (?)
endif

sb_vmt tsvmt <sb_init, sb_done, sb_setvol, sb_play, sb_bpm>;, sb_waveplay>


if pmw gt 0
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
.code
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
endif

comment #
sb_getport proc
        ;esi -> trxmdata
        ;es:edi -> environment

        xor     eax,eax
        xor     ecx,ecx
        dec     ecx
        jmp     @@scanentry
@@envscan:
        repne   scasb           ;   BLASTER=
@@scanentry:
        cmp     byte ptr es:[edi],al
        je      @@weg
        cmp     dword ptr es:[edi],53414C42h   ;BLAS
        jne     @@envscan
        cmp     dword ptr es:[edi+4],3D524554h ;TER=
        jne     @@envscan

        add     edi,8
        lea     ebx,[esi].sb_i
@@l:
        mov     ch,es:[edi]
        inc     edi
        or      ch,ch
        jz      @@done
        cmp     ch,' '
        je      @@l
        and     ch,not 20h              ;upper case

        xor     eax,eax
@@z:
        mov     cl,es:[edi]
        sub     cl,'0'
        cmp     cl,10
        jae     @@c

        inc     edi
        shl     eax,4
        or      al,cl
        jmp     @@z
@@c:
        cmp     ch,'A'
        jne     @@I
        mov     d_base[ebx],eax
@@I:
        cmp     ch,'I'
        jne     @@D
        cmp     al,16
        jb      @@I0
        sub     al,6
@@I0:
        mov     d_irq[ebx],al
@@D:
        cmp     ch,'D'
        jne     @@H
        mov     d_dma[ebx],al
@@H:
        cmp     ch,'H'
        jne     @@l
        mov     d_hidma[ebx],al
        jmp     @@l

@@done:

        mov     [esi].tempvar,sb_num
@@weg:
        ret
sb_getport endp
#

sb_test proc near
        ;-> edi -> tdinfo
        ;<- eax = 0: sb nicht vorhanden, 1: sb vorhanden

        call    sb_reset

        cmp     al,0AAh
        mov     eax,0
        jne     @@weg

        mov     ah,DSP1xx
        test    [edi].d_flags,sb_dsp1
        jnz     @@v

                ;version abfragen
        mov     al,0E1h
        call    sb_write

        call    sb_read
        mov     ah,al
        call    sb_read
        cmp     eax,200h
        jbe     @@v
        inc     ah
@@v:
;   mov ah,DSP201
        mov     [edi].d_ver,ah

        mov     ebx,[edi].d_rate        ;Parameter korrigieren
        cmp     ebx,8000
        ja      @@rc0
        mov     ebx,8000
@@rc0:

        cmp     ah,DSP4xx
        jae     @@dsp4

        and     [edi].d_flags,not df16bit ;DSP 1.xx bis 3.xx nur 8 bit

        cmp     ah,DSP3xx
        je      @@dsp3                  ;nur DSP 3.xx stereo
        and     [edi].d_flags,not dfStereo
@@dsp3:

        test    [edi].d_flags,dfStereo
        setnz   cl
        shl     ebx,cl

        mov     ch,23                   ;ab DSP 2.01 max. rate = 43478 HZ

        cmp     ah,DSP201
        jae     @@201
        mov     ch,46                   ;max. rate = 21739 HZ
@@201:

        mov     eax,1000000             ;256-c = 1000000/rate
        xor     edx,edx
        div     ebx
        cmp     al,ch
        jae     @@rc1
        mov     al,ch
@@rc1:
        mov     ebx,eax                 ;rate = 1000000/(256-c)
        mov     eax,1000000
        xor     edx,edx
        div     ebx

        shr     eax,cl
        mov     ebx,eax
@@dsp4:
        cmp     ebx,44100
        jbe     @@rc2
        mov     ebx,44100
@@rc2:

        mov     [edi].d_rate,ebx
;        or      [edi].d_flags,dfDmabuf
        mov     [edi].d_mem,size tsbdata
        mov     [edi].d_dmabuf,65536
        lea     eax,sb_vmt
        mov     [edi].d_vmt,eax

        xor     eax,eax
        inc     eax
@@weg:
        ret
sb_test endp



sb_init proc near
        ;ebx -> trxmdata
        ;edi -> tsbdata

        push    edi
        mov     al,80h
        mov     ecx,65536
        mov     edi,[edi].d_dmabuf
        rep     stosb
        pop     edi

;        add     eax,0FFFFh
;        xor     ax,ax
;        mov     [edi].d_dmabuf,eax

;        mov     ecx,0FFFFh              ;dma buffer lîschen
;        mov     dl,80h
;@@dma:
;        mov     [eax+ecx],dl
;        dec     ecx
;        jns     @@dma

        call    sb_setc                 ;ebx -> trxmdata

                ;mix-init
        test    [edi].d_flags,df16bit
        jnz     @@m16
        test    [edi].d_flags,dfStereo
        jnz     @@st
                ;8 bit mono
        mov     eax,offset copy8_m
        mov     [edi].sb_copy,eax
        mov     eax,offset mix8_m
        mov     [edi].sb_mix,eax
        jmp     @@v

@@st:           ;8 bit stereo
        mov     eax,offset copy8_s
        mov     [edi].sb_copy,eax
        mov     eax,offset mix8_s
        mov     [edi].sb_mix,eax

@@v:            ;8 bit : volume table
        mov     ecx,33*256-1;65*256-1;
@@vl:   mov     al,cl
        imul    ch
        shr     eax,5;6;
        mov     [edi].sb.m_voltab[ecx],al
        dec     ecx
        jns     @@vl
                ;8 bit: mix-buffer
        mov     ecx,(size m_mixbuf)/4 -1;1024 -1            ;4K
        mov     eax,4096
@@ml:   mov     [edi].sb.m_mixbuf[ecx*4],eax
        dec     ecx
        jns     @@ml
comment #
                ;8 bit: clipping table
        mov     ecx,256*32-1 +256
@@cl:   mov     eax,ecx
        shr     eax,1
        or      ah,ah
        jz      @@cl0
        mov     al,255
@@cl0:  mov     [edi].sb.m_clip[ecx -256],al
        dec     ecx
        jnz     @@cl
#
        jmp     @@mixer

@@m16:  ;16 bit
        mov     eax,offset copy16_s
        mov     [edi].sb_copy,eax
        mov     eax,offset mix16_s
        mov     [edi].sb_mix,eax

        ;16 bit: mix-buffer
        mov     ecx,(size m_mixbuf16)/4 -1 ;2048 -1            ;8K
        mov     eax,32768 * 1024
@@ml16: mov     [edi].sb.m_mixbuf16[ecx*4],eax
        dec     ecx
        jns     @@ml16


@@mixer:        ;mixer bei DSP 3.xx (sbpro)
        cmp     [edi].d_ver,DSP3xx
        jb      @@check

        mov     edx,[edi].d_base
        add     dl,MIXER_INDEX
        cmp     [edi].d_ver,DSP4xx
        jae     @@DSP4xx

        mov     al,0Eh                  ;filter & stereo register
        mov     ah,20h                  ;kein filter (20h)
        test    [edi].d_flags,dfStereo
        jz      @@2
        or      ah,02h                  ;stereo (02h)
@@2:    out     dx,ax

        mov     ah,[edi].d_mastervol    ;set master volume
        or      ah,ah
        jz      @@check

        shr     ah,4
        mov     al,ah
        shl     al,4
        or      ah,al
        mov     al,22h
        out     dx,ax

        mov     al,04h                  ;set voice volume
        mov     ah,0FFh
        out     dx,ax

        jmp     @@check

@@DSP4xx:
        mov     ah,[edi].d_mastervol    ;set master volume
        or      ah,ah
        jz      @@check

        mov     al,30h
        out     dx,ax
        inc     al
        out     dx,ax
        mov     ah,0FFh
        inc     al
        out     dx,ax
        inc     al
        out     dx,ax

         ;int
;     mov al,80h
;     mov ah,02h
;     out dx,ax
         ;dma
;     mov al,81h
;     mov ah,22h
;     out dx,ax
@@check:
                ;dma number, dma-pos-shift
        mov     cl,0
        mov     al,[edi].d_dma1
        test    [edi].d_flags,df16bit
        jz      @@8
                ;16 bit
        mov     al,[edi].d_dma2
        cmp     al,4
        jae     @@8
        inc     cl
@@8:
        mov     [edi].sb_dmanum,al
        test    [edi].d_flags,dfStereo
        jz      @@m
        inc     cl
@@m:    mov     [edi].sb_dmashr,cl


                ;DMA & IRQ check
        mov     [edi].sb_intDSP,0ffh
        call    sb_install
;comment #
        call    sb_dma


                ;DSP 1.xx bis 3.xx
        mov     al,14h                  ;8-bit single-cycle DMA
        cmp     [edi].d_ver,DSP4xx
        jb      @@6
                ;DSP 4.xx
        mov     al,0C2h                 ;8-bit single-cycle
        test    [edi].d_flags,df16bit
        jz      @@5
        mov     al,0B2h                 ;16-bit single-cycle
@@5:    call    sb_write
        xor     al,al                   ;mono unsigned
@@6:

        call    sb_write
        mov     al,1
        call    sb_write                ;size = 2
        mov     al,0
        call    sb_write


;        mov     eax,100
;        call    delay
        mov     eax,maincount
@@wait: mov     edx,maincount
        sub     edx,eax
        cmp     edx,100
        jl      @@wait

        mov     ebx,edi
        call    sb_getdmapos
        or      al,al
        mov     edx,1
        jz      @@err         ;dma nicht gestartet (al = 0)
        shr     eax,8
        jnz     @@ok
        mov     edx,2         ;int nicht gekommen (eax < dmabuflen)
@@err:
        push    edx
        call    sb_deinstall
        pop     eax
        jmp     @@weg
@@ok:
;#
        mov     al,[edi].d_ver
        mov     [edi].sb_intDSP,al
        mov     [edi].sb_dmapos,0
                ;check OK

        call    sb_dma

        mov     eax,[edi].d_rate        ;samples pro sekunde
        shr     eax,3                   ;1/8 Sekunde
        mov     [edi].sb_mixvor,eax
        mov     [edi].sb.m_tsamples,1

;        call    sb_install
        call    sb_start

        xor     eax,eax
@@weg:  ret
sb_init endp

sb_done proc near
        ;edi -> tsbdata

        call    sb_reset
        call    sb_reset
        mov     al,0D3h
        call    sb_write

        call    sb_deinstall

        ret
sb_done endp

sb_setvol proc near
        ;-> eax = vol (0..32*8), ideal 12*8

        test    [edi].d_flags,df16bit
        jnz     @@16bit

        imul    eax,32768/(12*8)
                ;8 bit: clipping table
        mov     ecx,-128*32
@@l:
        mov     ebx,ecx
        imul    ebx,eax
        sar     ebx,16
        add     ebx,80h
        or      bh,bh
        jz      @@0
        sar     ebx,31
        not     ebx
@@0:
        mov     [edi].sb.m_clip[ecx],bl

        inc     ecx
        cmp     ecx,128*32
        jl      @@l
        jmp     @@weg

@@16bit:
        mov     [edi].sb.m_vol,eax         ;vol = 32*8 entspricht *1

@@weg:  ret
sb_setvol endp

sb_play proc near
        ;edi -> tsbdata
        ;esi -> rxmdata

        mov     edx,offset sb_sconv
        call    foreach


;        mov     ebx,edi
;        call    sb_getdmapos
;        add     eax,[edi].sb_mixvor
;        mov     [edi].sb_mixpos,eax

        mov     eax,140*65536
        mov     edx,offset sb_int
        call    newhandler

        ret
sb_play endp

;called by foreach
sb_sconv proc near
        ;-> edi -> tsbdata
        ;-> esi -> tsampleh

        mov     ebx,[esi].sDataptr      ;ebx -> sample data
        mov     sOffset[esi],ebx

        test    sType[esi],sf16bit
        jz      @@weg
                ;16 bit
        test    [edi].d_flags,df16bit
        jnz     @@16bit

                ;16 bit -> 8 bit
        mov     ecx,[esi].sLoope        ;ecx = sample length
        xor     edx,edx
@@l:    inc     edx
        mov     al,[ebx+edx]
        mov     [ebx],al
        inc     ebx
        dec     ecx
        jns     @@l                     ;one extra byte

        and     [esi].sType,not sf16bit ;sample now 8 bit
        jmp     @@weg

@@16bit:
        shr     [esi].sOffset,1

@@weg:  ret
sb_sconv endp

sb_bpm proc near  ;ecx und esi nicht benutzen
        ;eax = frequenz*65536
        ;edi -> trxmdata

        mov     ebx,[edi].s_drvmem      ;ebx -> tsbdata

        mov     ebp,eax

;        mov     eax,45*65536            ;frequenz mu· >= 45Hz sein
;        xor     edx,edx
;        div     ebp
;        inc     eax
;        mov     [ebx].sb_tile,eax       ;eax = tick-unterteilung
;        imul    ebx,eax                 ;ebx = frequenz * teilung

        mov     eax,[ebx].d_rate
        shl     eax,16
        xor     edx,edx
        div     ebp
        mov     [ebx].sb_tsamples,eax   ;samples pro teil-tick

        shl     eax,8
        xor     edx,edx
        div     [edi].head.hdChannels
        mov     [ebx].sb_csamples,eax   ;samples pro kanal (8 bit nachkomma)

        ret
sb_bpm endp


;---wave play-----------------------------------------------------------------
comment #

samples = 512

sb_waveplay proc near
        ;esi -> wavedata
        ;eax = len
        ;edi -> tsbdata

        mov     [edi].sb_wavpos,esi
        mov     [edi].sb_wavsize,eax

        mov     ebx,edi
        call    sb_getdmapos
        add     eax,[edi].sb_mixvor
        and     eax,not (samples-1)     ;samples zu kopieren
        mov     [edi].sb_mixpos,eax

        mov     eax,100*65536
        mov     edx,offset sb_waveint
        call    newhandler

@@weg:  ret
sb_waveplay endp


sb_waveint proc near                    ;100 mal pro sekunde
        mov     edi,rxmdata
        mov     ebx,[edi].s_drvmem

        call    sb_getdmapos            ;dma-pos in samples

        add     eax,[ebx].sb_mixvor
        mov     edx,[ebx].sb_mixpos
        sub     eax,edx

        cmp     eax,samples
        jl      @@weg

        add     [ebx].sb_mixpos,samples

        mov     ecx,samples
        test    [ebx].d_flags,dfStereo
        jz      @@0
        shl     ecx,1
        shl     edx,1
@@0:
        test    [ebx].d_flags,df16bit
        jz      @@1
        shl     ecx,1
        shl     edx,1
@@1:
        cmp     [ebx].sb_wavsize,ecx
        jl      @@kill

        mov     esi,[ebx].sb_wavpos

        add     [ebx].sb_wavpos,ecx
        sub     [ebx].sb_wavsize,ecx

        mov     edi,[ebx].d_dmabuf
        add     di,dx
        shr     ecx,2
        rep     movsd

        jmp     @@weg
@@kill:
        mov     [edi].xmstatus,xmOff

        mov     eax,0
        mov     edx,offset sb_waveint
        call    newhandler

;        mov     al,80h
;        mov     edi,[ebx].d_dmabuf
;        add     di,dx
;        rep     stosb

@@weg:  ret
sb_waveint endp
#
;-----------------------------------------------------------------------------
sb_reset proc near
        ;-> rxm -> rxmdata
        ;<- al = 0AAh wenn erfolgreich
        mov     edx,[edi].d_base
        add     dl,DSP_RESET
        mov     al,1
        out     dx,al
        dec     al
@@delay:
        dec     al
        jnz     @@delay
        out     dx,al

        mov     ecx,100000
@@empty:
        mov     edx,[edi].d_base
        add     dl,DSP_DATA_AVAIL

        in      al,dx
        or      al,al
        js      @@0

        loop    @@empty
        jmp     @@weg
@@0:
        add     dl,DSP_READ_DATA-DSP_DATA_AVAIL
        in      al,dx
@@weg:  ret
sb_reset endp


sb_write proc near
        ;-> al = value
        push    eax
        mov     edx,[edi].d_base
        add     dl,DSP_WRITE
@@0:    in      al,dx
        or      al,al
        js      @@0                     ;bis bit 7 = 0
        pop     eax
        out     dx,al
        ret
sb_write endp

sb_read proc near
        ;<- al = value
        mov     edx,[edi].d_base
        add     dl,DSP_DATA_AVAIL
@@0:    in      al,dx
        or      al,al
        jns     @@0                     ;bis bit 7 <> 0
        add     dl,DSP_READ_DATA-DSP_DATA_AVAIL
        in      al,dx
        ret
sb_read endp

;sb_mixinit proc near


;@@weg:  ret
;sb_mixinit endp
sb_setc proc near ;zeitkonstante setzen
        ;ebx -> trxmdata
        ;edi -> tsbdata

        push    ebx
        mov     ebx,[edi].d_rate        ;ebx = rate

        cmp     [edi].d_ver,DSP4xx
        jae     @@sb16

                ;normale versionen
        mov     al,40h                  ;dsp: set samplerate
        call    sb_write

        test    [edi].d_flags,dfStereo
        setnz   cl                      ;mono: cl = 0 ; stereo: cl = 1
        mov     eax,1000000             ;h = 1000000 [/2] /freqenz
        shr     eax,cl                  ;/2 fÅr stereo
        xor     edx,edx
        div     ebx
        neg     al                      ;c = 256 - h
        call    sb_write

        jmp     @@0

@@sb16:         ;sb 16
        mov     al,41h
        call    sb_write
        mov     al,bh                   ;frequenz direkt
        call    sb_write
        mov     al,bl
        call    sb_write
@@0:
                ;incval = f * (65536 / rate)
        xor     eax,eax
        mov     edx,256
        div     ebx                     ;(65536 / rate) *2^24
        pop     ebx
        mov     [ebx].s_freqmul,eax     ;ebx -> trxmdata

@@weg:  ret
sb_setc endp

sb_dma proc near
        cli
      movzx ebx,[edi].sb_dmanum
      cmp bl,4
      jae @@16bit
;        test    [edi].d_flags,df16bit
;        jnz     @@16bit

@@8bit:
;        movzx   ebx,[edi].d_dma1
        mov     al,bl                   ;dma kanal wÑhlen
        or      al,4
        out     0ah,al

        xor     al,al                   ;flip-flop-reset
        out     0ch,al

        mov     al,bl                   ;dma channel
        add     al,48h +10h             ;transfer mode (+autoinit)
        out     0bh,al

        mov     edx,ebx                 ;adresse und lÑnge setzen
        shl     edx,1
        xor     al,al
        out     dx,al                   ;lo(offset) = 0
        out     dx,al                   ;hi(offset) = 0
        inc     edx
        dec     al
        out     dx,al                   ;size = FFFFh
        out     dx,al

        mov     dl,pageport[ebx]        ;page-port
        mov     al,b [edi].d_dmabuf[2]  ;page
        out     dx,al

        mov     al,bl                   ;dma freigeben
        out     0ah,al
        jmp     @@weg

@@16bit:
;        movzx   ebx,[edi].d_dma2
        mov     al,bl                   ;dma kanal wÑhlen
        out     0d4h,al                 ;bit 2 ist gesetzt
        and     bl,3

        xor     al,al                   ;flip-flop-reset
        out     0d8h,al

        mov     al,bl                   ;dma channel
        add     al,48h +10h             ;transfer mode (+autoinit)
        out     0d6h,al

        lea     edx,[ebx*4+0c0h]        ;adresse und lÑnge setzen
        xor     al,al                   ;               edx = c0h/c4h/c8h/cch
        out     dx,al                   ;lo(offset) = 0
        mov     al,b [edi].d_dmabuf[2]  ;page
        shl     al,7
        out     dx,al                   ;hi(offset) = 0 oder 80h

        inc     edx
        inc     edx
        mov     al,0FFh
        out     dx,al                   ;size = 7FFFh
        mov     al,7Fh
        out     dx,al

        mov     dl,pageport[ebx+4]      ;page-port
        mov     al,b [edi].d_dmabuf[2]  ;page
        ;and     al,0FEh
        out     dx,al

        mov     al,bl                   ;dma freigeben
        out     0d4h,al

@@weg:  sti
        ret
sb_dma endp

comment #
setdma8 proc pascal
        ;-> ebx = kanal
arg     mode, dmabuf, len

        cli
        mov     al,bl
        or      al,4
        out     0ah,al

        xor     al,al                   ;flip-flop-reset
        out     0ch,al

        mov     al,bl                   ;dma channel
        add     al,b mode               ;transfer mode
        out     0bh,al


        mov     edx,ebx                 ;adresse und lÑnge setzen
        shl     edx,1
        mov     eax,dmabuf              ;adresse
        out     dx,al                   ;lo
        mov     al,ah
        out     dx,al                   ;hi
        inc     edx
        mov     eax,len                 ;lÑnge-1
        out     dx,al                   ;lo
        mov     al,ah
        out     dx,al                   ;hi

        mov     dl,pageport[ebx]        ;page-port
        mov     al,b dmabuf[2]          ;page
        out     dx,al

        mov     al,bl                   ;dma freigeben
        out     0ah,al
        sti

        ret
setdma8 endp
#


sb_install proc near

if pmw gt 0
        mov     dl,[edi].d_irq
        add     dl,8
        test    dl,10h
        jz      @@0
        add     dl,70h-10h
@@0:
        push    es
        mov     ah,35h
        mov     al,dl
        int     21h
        mov     dword ptr oldint +0,ebx
        mov     word ptr oldint  +4,es
        pop     es

        push    ds
        mov     ah,25h
        mov     al,dl
        push    cs
        pop     ds
        mov     edx,offset sb_dmaint
        int     21h
        pop     ds
else
        mov     bl,[edi].d_irq
        call    _getirqvect
        mov     opmirq,edx
        mov     edx,offset sb_dmaint
        call    _setirqvect
        mov     edi,offset rmirqbuf
        call    _rmpmirqset
        mov     ormirq,eax

endif

        mov     bl,[edi].d_irq          ;sb-irq erlauben
        xor     al,al
        call    _setirqmask

        mov     al,20h
        out     20h,al
        ret
sb_install endp

sb_deinstall proc near

        mov     bl,[edi].d_irq          ;sb-irq sperren
        mov     al,1
        call    _setirqmask

if pmw gt 0
        mov     al,[edi].d_irq
        add     al,8
        test    al,10h
        jz      @@0
        add     al,70h-10h
@@0:
        push    ds
        mov     ah,25h
        lds     edx,oldint
        int     21h
        pop     ds
else

        mov     bl,[edi].d_irq          ;sb-int zurÅckstellen
        mov     eax,ormirq
        call    _rmpmirqfree
        mov     edx,opmirq
        call    _setirqvect             ;ret findet sich in _setirqvect
endif
        ret
sb_deinstall endp

sb_start proc near

        mov     al,0d1h                 ;speaker on
        call    sb_write

        mov     bh,[edi].d_ver
        cmp     bh,DSP4xx
        jae     @@4xx
        cmp     bh,DSP200
        jae     @@200
                ;DSP 1.xx
        mov     al,14h                  ;8-bit single-cycle DMA
        call    sb_write
        mov     al,0FFh
        call    sb_write                ;size = 65536
        call    sb_write
        jmp     @@weg
@@200:          ;DSP 2.00, 2.01+ und 3.xx
        mov     al,48h
        call    sb_write
        mov     al,0FFh
        call    sb_write                ;size = 65536
        call    sb_write

        mov     al,90h                  ;8-bit high-speed auto-init DMA
        test    [edi].d_flags,dfStereo
        jnz     @@hi
        cmp     [edi].d_rate,22050
        ja      @@hi
        mov     al,1Ch                  ;8-bit auto-init DMA
@@hi:   call    sb_write
        jmp     @@weg

@@4xx:          ;DSP 4.xx
        mov     al,0C6h                 ;8-bit auto-init
        mov     bl,0FFh
        test    [edi].d_flags,df16bit
        jz      @@5
        mov     al,0B6h                 ;16-bit auto-init
        mov     bl,07Fh
@@5:    call    sb_write

        xor     al,al                   ;mono unsigned
        test    [edi].d_flags,dfStereo
        jz      @@6
        or      al,20h                  ;stereo
@@6:    call    sb_write

        mov     al,0FFh                 ;8 bit: 0FFFFh, 16 bit: 07FFFh
        call    sb_write                ;lo
        mov     al,bl
        call    sb_write                ;hi

@@weg:  ret
sb_start endp

sb_dmaint proc
      ;pushad
        push    eax ebx edx edi ds
      ;push ds es
        mov     ds,cs:_seldata
      ;push ds
      ;pop es

        mov     edi,rxmdata
        mov     edi,[edi].s_drvmem

        cmp     [edi].sb_intDSP,DSP1xx;d_ver,DSP1xx
        ja      @@0
                ;DSP 1.xx
        mov     al,14h                  ;8-bit single-cycle DMA
        call    sb_write
        mov     al,0FFh
        call    sb_write
        call    sb_write
@@0:
;        mov     ebx,65536               ;ebx = dma-len
        mov     edx,[edi].d_base        ;edx = sb-base
        add     dl,0Eh
        test    [edi].d_flags,df16bit
        jz      @@1
                ;16 bit
;        shr     ebx,1                   ;dma-len nur 32768 einzelne samples
        inc     dl
@@1:    in      al,dx                   ;int-ack bei sb


        mov     eax,65536
        cmp     [edi].sb_dmanum,4
        jb      @@2
                ;16 bit dma
        shr     eax,1
@@2:    add     [edi].sb_dmapos,eax     ;dma-pos aktualisieren

                ;weg
        cmp     [edi].d_irq,7
;    pop es ds
        pop     ds edi edx ebx
        mov     al,20h                  ;int-ack bei controller
        jbe     @@i7
        out     0A0h,al                 ;irq > 7
@@i7:
        out     20h,al
;   popad
        pop     eax
        sti
        iretd
sb_dmaint endp


sb_getdmapos proc near
        ;-> ebx -> tsbdata
        ;<- eax = aktuelle DMA-position

        xor     eax,eax                 ;al = 0
        dec     ah                      ;ah = 0FFh (and-maske)

     movzx  edx,[ebx].sb_dmanum
     cmp dl,4
     jae @@16
;        test    [ebx].d_flags,df16bit
;        jnz     @@16
                ;8 bit dma
;        movzx   edx,[ebx].d_dma1
        shl     edx,1                   ;adre·register(0/2/4/6)
        out     0ch,al                  ;flip-flop-reset
        jmp     @@in

@@16:           ;16 bit dma
;        movzx   edx,[ebx].d_dma2
        and     dl,3
        shl     dl,2
        add     dl,0c0h                 ;adre·register (c0h/c4h/c8h/cch)
        shr     ah,1                    ;ah = 07Fh
        out     0d8h,al                 ;flip-flop-reset
@@in:

@@l:
        in      al,dx
        mov     cl,al
        in      al,dx
        mov     ch,al
        in      al,dx
        cmp     al,cl
        je      @@0
        mov     cl,al
        in      al,dx
        mov     ch,al
@@0:

        mov     al,cl
        and     ah,ch

;        in      al,dx
;        mov     ah,al
;        in      al,dx
;        xchg    al,ah
;        cmp     al,0ffh
;        jne     @@l


;        and     eax,ecx
;     shr     eax,1


        add     eax,[ebx].sb_dmapos
      mov     cl,[ebx].sb_dmashr
      shr     eax,cl
;        test    [ebx].d_flags,dfStereo
;        jz      @@m
;        shr     eax,1                   ;stereo
;@@m:

        ret
sb_getdmapos endp


sb_int proc near
        mov     edi,rxmdata
        mov     ebx,s_drvmem[edi]
        cmp     [ebx].sb_intflag,0
        jne     @@weg                   ;noch im int
        mov     [ebx].sb_intflag,1
        mov     al,20h
        out     20h,al
        sti                             ;interrupts frei
;-------

        call    sb_getdmapos            ;eax = dma-pos
        sub     eax,[ebx].sb_mixpos
        js      @@d0
        add     eax,[ebx].sb_mixvor     ;nicht hinterhergekommen
        add     sb_mixpos[ebx],eax
        xor     eax,eax
@@d0:
        add     eax,[ebx].sb_mixvor
        shl     eax,8
        sub     eax,[ebx].sb_donesamples
        mov     [ebx].sb_dosamples,eax

        mov     esi,[ebx].sb_aktchdata  ;aktuelles channel-record
@@l:

        mov     eax,[ebx].sb_csamples
        cmp     eax,[ebx].sb_dosamples
        jge     @@w
        sub     [ebx].sb_dosamples,eax
  add [ebx].sb_donesamples,eax

        dec     [ebx].sb_chancnt        ;kanal-zÑhler
        jg      @@l1
        mov     eax,[edi].head.hdChannels
        mov     [ebx].sb_chancnt,eax

        mov     ecx,[ebx].sb.m_tsamples ;mixbuffer kopieren
        movzx   edx,w [ebx].sb_mixpos
        add     [ebx].sb_mixpos,ecx
  mov [ebx].sb_donesamples,0
        mov     esi,[ebx].d_dmabuf
        call    [ebx].sb_copy

;        dec     [ebx].sb.m_tilecnt      ;teilung-zÑhler
;        jg      @@l0

        call    tick                    ;neuer tick
        mov     ebx,[edi].s_drvmem

        mov     eax,[ebx].sb_tsamples   ;tsamples und tile Åbernehmen
        mov     [ebx].sb.m_tsamples,eax
;        mov     eax,[ebx].sb_tile
;        mov     [ebx].sb.m_tile,eax
;        mov     [ebx].sb.m_tilecnt,eax


@@l0:   lea     esi,[edi].chdata
@@l1:
        mov     eax,[ebx].sb_mix
        lea     ebx,[ebx].sb            ;ebx -> tmix
        push    edi
        mov     edi,cmixpos[esi]        ;edi = cpos

        call    eax

        mov     cmixpos[esi],edi
        pop     edi
        mov     ebx,[edi].s_drvmem      ;ebx -> tsbdata
        add     esi,size tchannel
        jmp     @@l

@@w:
        mov     [ebx].sb_aktchdata,esi  ;aktuelles channel-record speichern

;-------
        cli
        mov     [ebx].sb_intflag,0
@@weg:  ret
sb_int endp


copy8_m proc near
;        mov     ecx,[ebx].sb.m_tsamples ;mixbuffer kopieren
;        movzx   edx,w [ebx].sb_mixpos
;        add     [ebx].sb_mixpos,ecx
;        mov     esi,[ebx].d_dmabuf

        dec     ecx

        push    edi
        mov     ebp,4096
        lea     edi,[ebx].sb.m_mixbuf
@@c:
        mov     eax,[edi+ecx*4]
        mov     [edi+ecx*4],ebp
        mov     al,[ebx].sb.m_clipstart[eax]
        mov     [esi+edx],al
        inc     dx
        dec     ecx
        jns     @@c
        pop     edi

        ret
copy8_m endp

copy8_s proc near
;        mov     ecx,[ebx].sb.m_tsamples ;mixbuffer kopieren
;        movzx   edx,w [ebx].sb_mixpos
;        add     [ebx].sb_mixpos,ecx
;        mov     esi,[ebx].d_dmabuf

        shl     dx,1
        dec     ecx

        push    edi
        mov     ebp,4096
        lea     edi,sb.m_mixbuf[ebx]
@@c:
        movzx   eax,w [edi+ecx*4]       ;links
        mov     al,[ebx].sb.m_clipstart[eax]
        mov     [esi+edx],al
        inc     dx
        movsx   eax,w [edi+ecx*4 +2]    ;rechts
        mov     [edi+ecx*4],ebp
        mov     al,[ebx].sb.m_clip[eax]
        mov     [esi+edx],al
        inc     dx

        dec     ecx
        jns     @@c
        pop     edi

        ret
copy8_s endp

copy16_s proc near
;        mov     ecx,[ebx].sb.m_tsamples ;mixbuffer kopieren
;        movzx   edx,w [ebx].sb_mixpos
;        add     [ebx].sb_mixpos,ecx
;        mov     esi,[ebx].d_dmabuf

        shl     dx,2
        shl     ecx,1                   ;*2 fÅr stereo
        dec     ecx

        mov     ebp,32768 * 1024 +1

@@mbl:
        mov     eax,[ebx].sb.m_mixbuf16[ecx*4]
        sar     eax,8+2                 ;/1024
        cmp     eax,65536
        jae     @@clip
@@mb0:
        mov     [ebx].sb.m_mixbuf16[ecx*4],ebp
        mov     [esi+edx],ax
        add     dx,2

        dec     ecx
        jns     @@mbl
        ret

@@clip:
        sar     eax,31
        not     eax
        jmp     @@mb0

copy16_s endp

mixcount proc near
        ;-> ecx = samples
        ;-> edi = sample-position
        ;<- ecx = samples
        ;<- edi = sample-position
        ;<- ebp = inc-val

        mov     ebp,cmixincval[esi]
        cmp     cmixdir[esi],1
        je      @@sr
                ;vorwÑrts
@@sv:   mov     edx,edi                 ;edx = cpos
        sub     edx,cmixloope[esi]        ;samples bis loop-ende (negativ)
        jge     @@loope                 ;anschlag
        mov     eax,cmixposn[esi]          ;eax = cposn
        shrd    eax,edx,16
        sar     edx,16
        neg     ebp
        cmp     edx,ebp
        jg      @@s1
        jmp     @@w

@@sr:           ;rÅckwÑrts
        mov     edx,edi                 ;edx = cpos
        sub     edx,cmixloops[esi]        ;samples bis loop-anfang
        jl      @@loops                 ;anschlag
        mov     eax,cmixposn[esi]          ;eax = cposn
        shrd    eax,edx,16
        sar     edx,16
        cmp     edx,ebp
        jge     @@w
@@s1:
        idiv    ebp
        inc     eax
        neg     ebp                     ;ebp = cincval (fÅr rÅckwÑrts negativ)

        cmp     eax,ecx                 ;ecx = m_tsamples
        ja      @@w
        mov     ecx,eax
        jmp     @@w

@@loope:;sample am loop-ende
        ;edi = cpos
        ;edx = cpos - cloope

        test    cmixtype[esi],sfLoop      ;kein loop: sample zu ende
        jz      @@stop
        test    cmixtype[esi],sfPingpong  ;ping-pong loop?
        jnz     @@pp
                ;forward
        mov     edi,edx
        add     edi,cmixloops[esi]
        jmp     @@sv

@@pp:           ;ping-pong
        mov     edi,cmixloope[esi]
        neg     cmixposn[esi]
        sbb     edi,edx                 ;"spiegelung" von edi an cloope
        mov     cmixdir[esi],1
        jmp     @@sr

@@loops:;sample am loop-anfang
        ;edi = cpos
        ;edx = cpos - cloops

        mov     edi,cmixloops[esi]
        neg     cmixposn[esi]
        sbb     edi,edx
        mov     cmixdir[esi],0
        jmp     @@sv

@@stop:         ;sample anhalten
        mov     edi,cmixloope[esi]
        mov     cmixdir[esi],2
        xor     ebp,ebp

@@w:    sub     m_samples[ebx],ecx
        ret
mixcount endp

vol0 proc near
        ;-> ecx = samples
        ;-> edi = sample-position
        ;-> ebp = inc-val

        mov     eax,ebp
        imul    ecx
        shld    edx,eax,16
        shl     eax,16
        add     edi,edx
        add     cmixposn[esi],eax
@@weg:  ret
vol0 endp

startsample proc near
        mov     edi,csample[esi]
        mov     cmixpos[esi],edi
        mov     eax,cloops[esi]
        mov     cmixloops[esi],eax
        mov     eax,cloope[esi]
        mov     cmixloope[esi],eax
        mov     al,ctype[esi]
        mov     cmixtype[esi],al
        xor     eax,eax
        mov     cmixposn[esi],eax
        mov     cmixdir[esi],al
@@weg:  ret
startsample endp

mix8_m proc near                        ;8 bit mono mixing

        test    caktiv[esi],afAktiv
        jz      @@cweg

        mov     eax,cincval[esi]
        mov     cmixincval[esi],eax

        test    caktiv[esi],afStart
        jz      @@nostart
;        mov     eax,m_tile[ebx]
;        cmp     m_tilecnt[ebx],eax
;        jne     @@nostart

        call    startsample
@@nostart:
        cmp     cmixdir[esi],2             ;sample steht
        jae     @@cweg

        mov     eax,m_tsamples[ebx]
        mov     m_samples[ebx],eax

@@sl:
     mov ecx,m_samples[ebx]
        call    mixcount                ;ebp = incval, ecx = samples

        mov     eax,cfinalvol[esi]     ;0..256
        shl     eax,5                   ;ah = vol (0..32)
        jz      @@v0

        mov     edx,ebp
        shl     edx,16
        mov     m_inclo[ebx],edx
        sar     ebp,16
        mov     m_inchi[ebx],ebp
        mov     ebp,cmixposn[esi]          ;ebp = cposn

        push    esi
        mov     edx,m_samples[ebx]
        lea     esi,m_mixbuf[ebx+edx*4]

        dec     ecx
@@8bit: ;inner loop fÅr 8 bit mono
        mov     edx,ebp
        shr     edx,16+3
        mov     dl,[edi+1]

        mov     al,m_voltab[ebx+edx]
        mov     dl,[edi]
        neg     dh
        add     dh,32
        add     al,m_voltab[ebx+edx]

        add     ebp,m_inclo[ebx]
        movsx   edx,m_voltab[ebx+eax]
        adc     edi,m_inchi[ebx]

        add     [esi+ecx*4],edx

        dec     ecx
        jns     @@8bit

        pop     esi
        mov     cmixposn[esi],ebp
@@v1:
        cmp     m_samples[ebx],0
        jg      @@sl                    ;noch nicht fertig

@@cweg:
        ret

@@v0:   call    vol0
        jmp     @@v1
mix8_m endp

mix8_s proc near                        ;8 bit stereo mixing


        test    caktiv[esi],afAktiv
        jz      @@cweg

        mov     eax,cfinalvol[esi]      ;0..256
        mov     edx,cfinalpan[esi]      ;panning
        or      dl,dl
        js      @@p0
                ;0 .. 7Fh (links .. mitte)
        mov     clivol[esi],eax
        imul    edx
        shr     eax,7
        mov     crevol[esi],eax
        jmp     @@p1
@@p0:           ;80h .. FFh (mitte .. rechts)
        mov     crevol[esi],eax
        not     dl
        imul    edx
        shr     eax,7
        mov     clivol[esi],eax
@@p1:
        mov     eax,cincval[esi]
        mov     cmixincval[esi],eax

        test    caktiv[esi],afStart
        jz      @@nostart
;        mov     eax,m_tile[ebx]
;        cmp     m_tilecnt[ebx],eax
;        jne     @@nostart

        call    startsample
@@nostart:
        cmp     cmixdir[esi],2             ;sample steht
        jae     @@cweg

        mov     eax,m_tsamples[ebx]
        mov     m_samples[ebx],eax

        mov     eax,clivol[esi]        ;0..256
        shl     eax,5;6;
        mov     m_livol[ebx],eax
        mov     eax,crevol[esi]        ;0..256
        shl     eax,5;6;
        mov     m_revol[ebx],eax

@@sl:
     mov ecx,m_samples[ebx]
        call    mixcount                ;ebp = incval, ecx = samples

        cmp     cfinalvol[esi],0
        je      @@v0

        mov     edx,ebp
        shl     edx,16
        mov     m_inclo[ebx],edx
        sar     ebp,16
        mov     m_inchi[ebx],ebp
        mov     ebp,cmixposn[esi]          ;ebp = cposn

        push    esi
        mov     edx,m_samples[ebx]
        lea     esi,m_mixbuf[ebx+edx*4]

        dec     ecx
@@8bit: ;inner loop fÅr 8 bit stereo
        mov     edx,ebp
        shr     edx,16+3;2;
        mov     dl,[edi+1]

        mov     eax,m_revol[ebx]
        mov     al,m_voltab[ebx+edx]
        mov     dl,[edi]
        neg     dh
        add     dh,32;64;
        add     al,m_voltab[ebx+edx]

        add     ebp,m_inclo[ebx]
        movsx   edx,m_voltab[ebx+eax]
        adc     edi,m_inchi[ebx]
        mov     ah,b m_livol[ebx+1]
        sal     edx,16
        movsx   eax,m_voltab[ebx+eax]

        add     eax,edx
        add     [esi+ecx*4],eax

        dec     ecx
        jns     @@8bit

        pop     esi
        mov     cmixposn[esi],ebp
@@v1:
        cmp     m_samples[ebx],0
        jg      @@sl                    ;noch nicht fertig

@@cweg:
        ret

@@v0:   call    vol0
        jmp     @@v1
mix8_s endp

mix16_s proc near                       ;stereo 16 bit mixing
        ;ebx -> tmix
        ;esi -> tchannel
        ;edi = cmixpos

        test    caktiv[esi],afStop
        jz      @@aktiv

;        mov     ecx,[ebx].m_tile
;        cmp     [ebx].m_tilecnt,ecx
;        jne     @@aktiv
@@stop:
        mov     eax,colivol[esi]
        mov     edx,corevol[esi]
        mov     m_liadd[ebx],eax
        mov     m_readd[ebx],edx
        shl     eax,6
        shl     edx,6
        mov     m_livol[ebx],eax
        mov     m_revol[ebx],edx

        mov     eax,[ebx].m_tsamples ;eax = m_tsamples
        mov     m_samples[ebx],eax

        call    slide

@@aktiv:
        test    caktiv[esi],afAktiv
        jz      @@cweg

        mov     eax,cfinalvol[esi]      ;0..256
        imul    eax,[ebx].m_vol
        shr     eax,3+3

        neg     eax                     ;combine panning and volume
        mov     edx,cfinalpan[esi]      ;left
        imul    edx,edx
        sub     edx,65536
        imul    edx,eax
        shr     edx,16
        mov     clivol[esi],edx

        mov     edx,cfinalpan[esi]      ;right
        sub     edx,256
        imul    edx,edx
        sub     edx,65536
        imul    edx,eax
        shr     edx,16
        mov     crevol[esi],edx


        mov     eax,cincval[esi]
        mov     cmixincval[esi],eax

        mov     eax,[ebx].m_tsamples ;eax = m_tsamples
        mov     m_samples[ebx],eax

;        mov     ecx,[ebx].m_tile
;        cmp     [ebx].m_tilecnt,ecx
;        jne     @@ramp_e

        test    caktiv[esi],afStart
        jz      @@play
        call    startsample             ;start
        xor     edx,edx
        jmp     @@ramp

@@play: cmp     cmixdir[esi],2          ;play
        jne     @@nstp
        and     caktiv[esi],not afPlay
        jmp     @@stop
@@nstp:
        mov     eax,colivol[esi]
        mov     edx,corevol[esi]

@@ramp: mov     m_livol[ebx],eax
        mov     m_revol[ebx],edx

        sub     eax,clivol[esi]
        sub     edx,crevol[esi]
;        neg     eax
;        neg     edx
        mov     m_liadd[ebx],eax
        mov     m_readd[ebx],edx

        or      eax,edx
        jz      @@ramp_e

        shl     m_livol[ebx],6
        shl     m_revol[ebx],6

        call    slide

@@ramp_e:
        mov     eax,clivol[esi]
        mov     edx,crevol[esi]
        mov     m_livol[ebx],eax
        mov     m_revol[ebx],edx


@@sl:
     mov ecx,m_samples[ebx]
        call    mixcount                ;ebp = incval, ecx = samples

        cmp     cfinalvol[esi],0
        je      @@v0

        push    esi

        dec     ecx
        test    cmixtype[esi],sf16bit
        jnz     @@16bit
                ;8 bit
        mov     m_inclo[ebx],ebp
        sar     ebp,16
        mov     m_inchi[ebx],ebp

        mov     ebp,cmixposn[esi]          ;ebp = cposn
        shr     ebp,16

        mov     edx,m_samples[ebx]
        lea     esi,m_mixbuf16[ebx+edx*8]

@@8bit: ;inner loop fÅr 8 bit stereo
        movsx   eax,b [edi]
        movsx   edx,b [edi+1]
                ;interpolation
        sub     edx,eax                 ;differenz
        imul    edx,ebp
        sal     eax,8
        sar     edx,8
        add     eax,edx

        mov     edx,eax
        imul    edx,m_livol[ebx]        ;linker kanal
        imul    eax,m_revol[ebx]        ;rechter kanal
        add     [esi+ecx*8+4],edx
        add     [esi+ecx*8],eax         ;-32768*256..32767*256

        add     bp,w m_inclo[ebx]
        adc     edi,m_inchi[ebx]
        dec     ecx
        jns     @@8bit

        shl     ebp,16
        jmp     @@2

@@16bit:        ;16 bit
        mov     eax,ebp
        shr     eax,1                   ;15 bit nachkomma
        or      eax,not 7FFFh           ;unbenutzte bit auf 1
        mov     m_inclo[ebx],eax

        sar     ebp,16
        mov     m_inchi[ebx],ebp

        mov     ebp,cmixposn[esi]          ;ebp = cposn
        shr     ebp,17                  ;15 bit nachkomma

        mov     edx,m_samples[ebx]
        lea     esi,m_mixbuf16[ebx+edx*8]

@@16l:  ;inner loop fÅr 16 bit stereo
        movsx   eax,w [edi*2]
        movsx   edx,w [edi*2+2]
                ;interpolation
        sub     edx,eax                 ;differenz
        imul    edx,ebp
        sar     edx,15                  ;15 bit nachkomma
        add     eax,edx

        mov     edx,eax
        imul    eax,m_livol[ebx]        ;linker kanal
        imul    edx,m_revol[ebx]        ;rechter kanal
        add     [esi+ecx*8+4],eax
        add     [esi+ecx*8],edx         ;-32768*256..32767*256

        add     ebp,m_inclo[ebx]
        adc     edi,m_inchi[ebx]

        and     ebp,7FFFh               ;obere bits lîschen

        dec     ecx
        jns     @@16l

        shl     ebp,17
@@2:
        pop     esi
        mov     cmixposn[esi],ebp
@@v1:
   cmp m_samples[ebx],0
   jg @@sl                             ;noch nicht fertig
@@cweg:
        ret

@@v0:   call    vol0
        jmp     @@v1

mix16_s endp

slide proc near
        mov     ecx,64
@@sl:
        call    mixcount                ;ebp = incval, ecx = samples

        add     ecx,m_samples[ebx]      ;ecx = samples + rest-samples

        push    esi

        dec     ecx
        test    cmixtype[esi],sf16bit
        jnz     @@16bit
                ;8 bit
        mov     m_inclo[ebx],ebp
        sar     ebp,16
        mov     m_inchi[ebx],ebp

        mov     ebp,cmixposn[esi]          ;ebp = cposn
        shr     ebp,16

        mov     esi,m_livol[ebx]

@@8bit: ;inner loop fÅr 8 bit stereo
        movsx   eax,b [edi]
        movsx   edx,b [edi+1]
                ;interpolation
        sub     edx,eax                 ;differenz
        imul    edx,ebp
        sal     eax,8
        sar     edx,8
        add     eax,edx

        mov     edx,eax
        imul    edx,esi                 ;linker kanal
        imul    eax,m_revol[ebx]        ;rechter kanal
        sar     edx,6;7
        sar     eax,6;7
        add     m_mixbuf16[ebx+ecx*8+4],edx
        add     m_mixbuf16[ebx+ecx*8],eax       ;-32768*256..32767*256

        dec     ecx

        mov     eax,m_revol[ebx]        ;r
        add     bp,w m_inclo[ebx]
        adc     edi,m_inchi[ebx]
        sub     eax,m_readd[ebx]        ;r
        sub     esi,m_liadd[ebx]        ;l
        cmp     ecx,m_samples[ebx]
        mov     m_revol[ebx],eax        ;r

        jge     @@8bit

        shl     ebp,16
        jmp     @@2

@@16bit:        ;16 bit
        mov     eax,ebp
        shr     eax,1                   ;15 bit nachkomma
        or      eax,not 7FFFh           ;unbenutzte bit auf 1
        mov     m_inclo[ebx],eax

        sar     ebp,16
        mov     m_inchi[ebx],ebp

        mov     ebp,cmixposn[esi]          ;ebp = cposn
        shr     ebp,17                  ;15 bit nachkomma

        mov     esi,m_livol[ebx]

@@16l:  ;inner loop fÅr 16 bit stereo
        movsx   eax,w [edi*2]
        movsx   edx,w [edi*2+2]
                ;interpolation
        sub     edx,eax                 ;differenz
        imul    edx,ebp
        sar     edx,15                  ;15 bit nachkomma
        add     eax,edx

        mov     edx,eax
        imul    edx,esi                 ;linker kanal
        imul    eax,m_revol[ebx]        ;rechter kanal
        sar     edx,6;7
        sar     eax,6;7
        add     m_mixbuf16[ebx+ecx*8+4],edx
        add     m_mixbuf16[ebx+ecx*8],eax       ;-32768*256..32767*256

        dec     ecx

        mov     eax,m_revol[ebx]        ;r
        add     ebp,m_inclo[ebx]
        adc     edi,m_inchi[ebx]
        sub     eax,m_readd[ebx]        ;r
        sub     esi,m_liadd[ebx]        ;l
        and     ebp,7FFFh               ;obere bits lîschen
        cmp     ecx,m_samples[ebx]
        mov     m_revol[ebx],eax        ;r

        jge     @@16l

        shl     ebp,17
@@2:
        mov     m_livol[ebx],esi
        pop     esi
        mov     cmixposn[esi],ebp

        mov     ecx,m_samples[ebx]
        add     ecx,64
        sub     ecx,m_tsamples[ebx]
        jg      @@sl                     ;noch nicht fertig

@@weg:  ret
slide endp


if pmw eq 0
code32  ends
endif

end

