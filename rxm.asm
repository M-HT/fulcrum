include switches.inc

if pmw gt 0

.386                                    ;pmode/w
.model flat,prolog

;extrn __psp:word
else

.386p
code32  segment para public use32       ;pmode/asm
                assume cs:code32, ds:code32

endif
locals



include int.inc

;c-interface
public getdmabuf, cleardmabuf, freedmabuf
public n_test
public rxminit
public rxmdevinit, rxmdevdone, rxmsetvol
public rxmplay, rxmstop, rxmcontinue, rxmskip
;public waveplay, waveend

;intern
public rxmdata, tick, foreach


;gamesnd
if gamesound gt 0
public Note_instr
extrn gamesounds:near
endif

include rxmstruc.inc

;include \sprachen\c\h\debug.inc
if pmw gt 0
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
.data
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
endif

;mchan = 4
;mute db (mchan+1) dup(0), 1,32 dup(0)
;mute db 6 dup(0), 1, 32 dup(0)

;sstart = 0

rxmdata dd 0


pos   equ 0
neg   equ 1
sig   equ 2

t0    equ 4
t1_x  equ 8
tx    equ t0 + t1_x
keep  equ 16
keep2 equ 32

fxoptions label byte
db pos + t1_x                ; 0 //. Appregio
db pos + t1_x + keep         ; 1 //  Porta up
db neg + t1_x + keep         ; 2 //  Porta down
db pos + t0   + keep         ; 3 //  Fine porta up
db neg + t0   + keep         ; 4 //  Fine prota down
db pos + t0   + keep         ; 5 //  Extra fine porta up
db neg + t0   + keep         ; 6 //  Extra fine porta down
db pos + tx   + keep         ; 7 //  Tone porta
db pos                       ; 8 //   Set glissando control
db pos + tx   + keep2        ; 9 //  Vibrato
db pos        + keep         ;10 //   Set vibrato speed
db pos                       ;11 //   Set vibrato control
db pos + t0                  ;12 //. Set volume
db sig + t1_x                ;13 //  Volume slide up/down      *
db sig + t1_x + keep         ;14 //  Volume slide up/down
db sig + t0                  ;15 //  Fine volume slide up/down *
db pos + t0   + keep         ;16 //  Fine volume slide up
db neg + t0   + keep         ;17 //  Fine volume slide down
db pos + t1_x + keep2        ;18 //  Tremolo
db pos                       ;19 //   Set tremolo speed
db pos                       ;20 //   Set tremolo control
db pos + tx                  ;21 //  Note cut
db pos + t1_x + keep         ;22 //  Tremor
db pos + t0                  ;23 //  Set global volume
db sig + t1_x + keep         ;24 //  Global volume slide
db pos + t0                  ;25 //. Set panning
db sig + t1_x                ;26 //  Panning slide left/right  *
db sig + t1_x + keep         ;27 //  Panning slide left/right
db pos + t0                  ;28 //. Position jump
db pos + t0                  ;29 //  Pattern break
db pos + t0                  ;30 //  Pattern delay
db pos + t0                  ;31 //  Set loop begin/loop
db pos + t0                  ;32 //  Set tempo
db pos + t0                  ;33 //  Set bpm
db pos + t0   + keep         ;34 //. Sample offset
db pos + tx                  ;35 //  Note delay
db pos + t0                  ;36 //  Key off
db pos + t0                  ;37 //  Set envelope position
db pos + tx                  ;38 //  Retrig
db pos + tx   + keep2        ;39 //  Multi retrig

db 0                         ;40
db 0                         ;41
db 0                         ;42
db 0                         ;43
db 0                         ;44
db 0                         ;45
db 0                         ;46
db 0                         ;47
db pos + t0   + keep         ;48 //  Note & instrument

fxtab label dword
dd Appregio       - fxprocs  ; 0 //. Appregio
dd Addnote        - fxprocs  ; 1 //  Porta up
dd Addnote        - fxprocs  ; 2 //  Porta down
dd Addnote        - fxprocs  ; 3 //  Fine porta up
dd Addnote        - fxprocs  ; 4 //  Fine prota down
dd Addnotex       - fxprocs  ; 5 //  Extra fine porta up
dd Addnotex       - fxprocs  ; 6 //  Extra fine porta down
dd Toneporta      - fxprocs  ; 7 //  Tone porta
dd 0                         ; 8 //   Set glissando control (not supported)
dd Vibrato        - fxprocs  ; 9 //  Vibrato
dd 0                         ;10 //   Set vibrato speed
dd 0                         ;11 //   Set vibrato control
dd Setvol         - fxprocs  ;12 //. Set volume
dd Addvol         - fxprocs  ;13 //  Volume slide up/down
dd Addvol         - fxprocs  ;14 //  Volume slide up/down
dd Addvol         - fxprocs  ;15 //  Fine volume slide up/down
dd Addvol         - fxprocs  ;16 //  Fine volume slide up
dd Addvol         - fxprocs  ;17 //  Fine volume slide down
dd Tremolo        - fxprocs  ;18 //  Tremolo
dd 0                         ;19 //   Set tremolo speed
dd 0                         ;20 //   Set tremolo control
dd Notecut        - fxprocs  ;21 //  Note cut
dd Tremor         - fxprocs  ;22 //  Tremor
dd Setgvol        - fxprocs  ;23 //  Set global volume
dd Gvolslide      - fxprocs  ;24 //  Global volume slide
dd Setpan         - fxprocs  ;25 //. Set panning
dd Addpan         - fxprocs  ;26 //  Panning slide left/right
dd Addpan         - fxprocs  ;27 //  Panning slide left/right
dd Posjump        - fxprocs  ;28 //. Position jump
dd Patbreak       - fxprocs  ;29 //  Pattern break
dd Patdelay       - fxprocs  ;30 //  Pattern delay
dd Setloop        - fxprocs  ;31 //  Set loop begin/loop
dd Settempo       - fxprocs  ;32 //  Set tempo
dd Setbpm         - fxprocs  ;33 //  Set bpm
dd Sampleofs      - fxprocs  ;34 //. Sample offset
dd Notedelay      - fxprocs  ;35 //  Note delay
dd Keyoff         - fxprocs  ;36 //  Key off
dd Setenvpos      - fxprocs  ;37 //  Set envelope position
dd Retrig         - fxprocs  ;38 //  Retrig
dd Multiretrig    - fxprocs  ;39 //  Multi retrig
dd 0                         ;40 //   2nd param

dd 0                         ;41
dd 0                         ;42
dd 0                         ;43
dd 0                         ;44
dd 0                         ;45
dd 0                         ;46
dd 0                         ;47
dd Note_instr     - fxprocs  ;48 //  Note & instrument




n_vmt  tsvmt <n_init, n_done, n_setvol, n_play, n_bpm>;, 0>

pmsel   dw 0
dmasize dd ?
dmabuf  dd ?


if pmw gt 0
.data?
rxmd trxmdata ?
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
.code
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
endif


getdmabuf proc near
        ;-> eax = size
     mov dmasize,eax
        push    eax
        shr     eax,4-1                 ;size*2 in paragraphs
        mov     ebx,eax
        mov     ax,0100h
        int     31h                     ;alloc dos mem
        mov     pmsel,dx
        pop     edx                     ;edx = size
        jc      @@0                     ;CF set if error
        movzx   eax,ax
        shl     eax,4                   ;eax -> dmabuffer
        movzx   ebx,ax                  ;page test
        add     ebx,edx
        cmp     ebx,10000h
        jbe     @@weg
        add     eax,0FFFFh
        xor     ax,ax
      mov dmabuf,eax
        jmp     @@weg
@@0:
        xor     eax,eax                 ;return 0 on error
@@weg:  ret
getdmabuf endp

cleardmabuf proc near
        cmp     pmsel,0
        je      @@weg
        mov     edi,dmabuf
        mov     al,80h
        mov     ecx,dmasize
        rep     stosb
@@weg:  ret
cleardmabuf endp


freedmabuf proc near
        mov     ax,0101h
        mov     dx,pmsel
        or      dx,dx
        jz      @@weg
        int     31h
        mov     pmsel,0
@@weg:  ret
freedmabuf endp

rxminit proc near
        ;for pmode/asm : -> edi -> trxmdata
if pmw gt 0
        lea     edi,rxmd
endif
        mov     rxmdata,edi
        xor     eax,eax
        mov     ecx,(size trxmdata)/4
        rep     stosd

        ret
rxminit endp

comment #
rxmdetect proc near
        ;<- eax = soundcard

        push    ebp

        mov     esi,rxmdata

if pmw gt 0
        push    es
        mov     es,__psp
        mov     es,es:[2Ch]
        xor     edi,edi
else
        mov     edi,_pspa
        sub     edi,_code32a
        movzx   edi,word ptr [edi+2ch]  ;env-realmode-segment
        shl     edi,4
        sub     edi,_code32a
endif
        push    edi
        call    sb_getport
        call    a_getport
        pop     edi
        call    u_getport

        movzx   eax,tempvar[esi]

if pmw gt 0
        pop     es
endif
        pop     ebp

        ret
rxmdetect endp
#

rxmdevinit proc near
        ;esi -> tdinfo
        ;edi -> driver-mem
        push    ebp
        mov     ebx,rxmdata

        mov     [ebx].s_drvmem,edi      ;driver-mem-pointer speichern

        push    esi                     ;copy vmt
        mov     esi,[esi].d_vmt
        lea     edi,[ebx].s_vmt
        mov     ecx,(size tsvmt) / 4
        rep     movsd
        pop     esi

        xor     eax,eax                 ;driver-mem l”schen
        mov     ecx,d_mem[esi]          ;driver-mem-size
;        jecxz   @@weg
        mov     edi,[ebx].s_drvmem
        rep     stosb


        mov     edi,[ebx].s_drvmem      ;sinfo kopieren
        mov     ecx,size tdinfo         ;esi -> tdinfo
        rep     movsb                   ;edi -> driver-mem

        mov     edi,[ebx].s_drvmem
        call    [ebx].s_vmt.v_init

@@weg:  pop     ebp
        ret
rxmdevinit endp

rxmdevdone proc near

        push    ebp
        mov     ebx,rxmdata

        mov     edi,[ebx].s_drvmem
        call    [ebx].s_vmt.v_done

        pop     ebp
        ret
rxmdevdone endp

rxmsetvol proc near
        ;-> eax = vol

        push    ebp
        mov     ebx,rxmdata

        mov     edi,[ebx].s_drvmem
        call    [ebx].s_vmt.v_setvol

        pop     ebp
        ret
rxmsetvol endp

rxmplay proc near
        ;-> esi -> rxm
        ;-> eax = start-pattern

        push    ebp
        push    eax
        mov     ebx,rxmdata

                ;clear indices
        lea     edi,[ebx].pattern
        mov     ecx,(256+128+128)
        xor     eax,eax
        rep     stosd

                ;clear channel data
        lea     edi,[ebx].chdata
        mov     ecx,32*(size tchannel)/4
        rep     stosd                   ;eax = 0

                ;header
        lea     edi,[ebx].head
        mov     ecx,size theader-256
        add     ecx,hdSonglen[esi]
        rep     movsb

        mov     edi,ebx                 ;edi -> trxmdata

if gamesound gt 0                       ;gamesounds
        mov     eax,[edi].head.hdChannels
        mov     [edi].pchannels,eax     ;pattern-channels
        add     eax,[edi].schannels     ;sound-channels
        mov     [edi].head.hdChannels,eax
endif
                ;pattern
        xor     ecx,ecx
@@pl:
        mov     [edi].pattern[ecx*4],esi
        add     esi,patSize[esi]
        inc     ecx
        cmp     ecx,[edi].head.hdPatterns
        jb      @@pl

                ;instruments
        xor     ecx,ecx
@@il:
        mov     al,iFollows[esi]
        movzx   edx,iSamples[esi]
        mov     ebx,esi                 ;ebx -> instrument
        add     esi,size tinstrh

        or      edx,edx                 ;0 samples?
        jz      @@next
        mov     [edi].instr[ecx*4],ebx  ;instrument speichern

        shr     al,1
        jnc     @@notesmp
        add     esi,size tnotesmp
@@notesmp:
        shr     al,1
        jnc     @@volenv
        movzx   ebx,envLastpoint[esi]
        lea     esi,[esi + (size tenvelope)+2 - 11*4 + ebx*4]
                                        ;+2 wegen fadeout
@@volenv:
        shr     al,1
        jnc     @@panenv
        movzx   ebx,envLastpoint[esi]
        lea     esi,[esi + (size tenvelope) - 11*4 + ebx*4]
@@panenv:
        shr     al,1
        jnc     @@vibrato
        add     esi,size tvibrato
@@vibrato:
                                        ;edx = number of samples
                ;samples
        mov     [edi].sample[ecx*4],esi
@@sl:
        test    [edi].head.hdFlags,xmConverted
        jnz     @@se

        push    ecx
        mov     ebx,sDataptr[esi]
        add     ebx,esi
        mov     sDataptr[esi],ebx

        mov     ecx,[esi].sLoope
        add     ecx,Xtra

        xor     eax,eax

ifndef no16bit
        test    sType[esi],sf16bit
        jnz     @@16l
endif
            ;8bit
@@8l:
        add     al,[ebx]
        mov     [ebx],al
        inc     ebx
        dec     ecx
        jnz     @@8l
ifndef no16bit
        jmp     @@8

@@16l:          ;16bit
        add     ax,[ebx]
        mov     [ebx],ax
        inc     ebx
        inc     ebx
        dec     ecx
        jnz     @@16l
endif
@@8:
        pop     ecx
@@se:
        add     esi,size tsampleh
        dec     edx
        jnz     @@sl
@@next:
        inc     ecx
        cmp     ecx,[edi].head.hdInstruments
        jb      @@il


                ;frequency table
        mov     ebx,2149422703          ;faktor    = 768û2 * 2^31
                                        ; 12 noten, 64 finetuneschritte
        mov     eax,1096155136          ;startwert = 535232 * 2^11
                                        ; 8363*2^6, frequenz fr C-10
        xor     ecx,ecx
@@fl:
ifndef no2tabs
        test    [edi].head.hdFlags,xmLinear
        jnz     @@linear
endif
ifndef noAmiga  ;amiga
        mov     esi,eax
                                        ;period = 8363*1712/frequency
        mov     eax,0C83B0000h          ;8363*1712 * 2^(11+12) = 6D3BC83B0000h
        mov     edx,6D3Bh
        div     esi
        mov     [edi].freq[ecx*4],eax
        mov     eax,esi
endif
ifndef no2tabs
        jmp     @@fl0
@@linear:
endif
ifndef noLinear ;linear
        mov     edx,eax                 ;ergebnis mit 11 nachkommabits
        shr     edx,11
        mov     [edi].freq[ecx*4],edx
endif
@@fl0:
        mul     ebx
        shrd    eax,edx,31              ;31 nachkommabits des faktors

        inc     ecx
        cmp     ch,3                    ;bis 768
        jb      @@fl

                ;appregio-mul-tab (nur amiga)
ifndef noAmiga
        mov     ebx,69433               ;faktor    = 12û2 * 2^16
        mov     eax,65536               ;startwert = 1 * 2^16
        xor     ecx,ecx
@@at:
        mov     edx,eax
        shr     edx,8
        mov     [edi].apptab[ecx*4],edx
        mul     ebx
        shrd    eax,edx,16
        inc     ecx
        cmp     cl,16
        jb      @@at
endif

                ;vibrato tables
        mov     ecx,127
@@vl1:          ;sinus berechnung (f(x) = 64 - ((x-64)*2)^2 / 256
        lea     eax,[(ecx-64)*2]
        imul    eax
        sub     ah,64
        mov     [edi].vibrato[ecx+128],ah
        neg     ah
        mov     [edi].vibrato[ecx],ah
        dec     ecx
        jns     @@vl1


        mov     ecx,255
@@vl2:          ;
        mov     al,cl
        sar     al,1
        mov     [edi].vibrato[ecx+768],al ;ansteigend
        neg     al
        mov     [edi].vibrato[ecx+512],al ;abfallend
        neg     al
        and     al,80h
        add     al,40h
        mov     [edi].vibrato[ecx+256],al ;rechteck
        dec     ecx
        jns     @@vl2

        mov     [edi].globalvol,64
        mov     [edi].p_break,1
        pop     eax
        mov     [edi].p_pos,eax;sstart
        mov     [edi].p_rowcnt,0
        mov     [edi].t_tick,0
if wavConvert gt 0
        mov     [edi].rflag,0
endif
        mov     eax,[edi].head.hdBPM
        call    Setbpm

        push    edi
        mov     esi,edi
        mov     edi,[esi].s_drvmem
        call    [esi].s_vmt.v_play
        pop     edi


        mov     [edi].xmstatus,xmPlay   ;starten
;        mov     [edi].stopcnt,0

        pop     ebp
        ret
rxmplay endp

rxmstop proc near
        ;-> eax = xmFade or xmStop
        mov     edi,rxmdata
;        inc     [edi].stopcnt
        cmp     [edi].xmstatus,xmOff
        je      @@weg
        mov     [edi].xmstatus,al;xmFade

@@0:    cmp     [edi].xmstatus,xmOff
        jne     @@0

@@weg:  ret
rxmstop endp

rxmcontinue proc near

        mov     edi,rxmdata
;        dec     [edi].stopcnt
;        jnz     @@weg
        cmp     [edi].head.hdChannels,0
        je      @@weg
        mov     [edi].xmstatus,xmPlay

@@weg:  ret
rxmcontinue endp

rxmskip proc near
        ;-> eax = patterns to skip
        mov     edi,rxmdata

        cmp     [edi].xmstatus,xmOff
        je      @@1

        mov     [edi].xmstatus,xmStop   ;stop xm
@@0:    cmp     [edi].xmstatus,xmOff    ;wait for stop
        jne     @@0
@@1:
        mov     [edi].p_break,1         ;skip
        add     [edi].p_pos,eax
        jns     @@2
        mov     [edi].p_pos,0
@@2:    mov     [edi].p_rowcnt,0
        mov     [edi].t_tick,0

        mov     [edi].xmstatus,xmPlay   ;restart

@@weg:  ret
rxmskip endp

comment #
;wave
waveplay proc near
        ;esi -> wavedata
        ;eax = len
        push    ebp
        mov     ebx,rxmdata
        mov     edi,[ebx].s_drvmem

        mov     [ebx].xmstatus,xmPlay
        call    [ebx].s_vmt.v_waveplay

@@weg:  pop     ebp
        ret
waveplay endp

waveend proc near

        mov     edi,rxmdata
        xor     eax,eax
        cmp     [edi].xmstatus,xmOff
        sete    al

        ret
waveend endp
#

;------ nosound
n_test:
                ;-> edi -> tdinfo
                lea     eax,n_vmt
                mov     [edi].d_vmt,eax
                xor     eax,eax
                mov     [edi].d_mem,size tdinfo
                mov     [edi].d_dmabuf,eax
                inc     eax
n_init:
n_done:
n_setvol:
n_play: ret

n_bpm proc near
        ;eax = frequenz*65536
        mov     edx,offset n_int
        call    newhandler
        ret
n_bpm endp

n_int proc near
        mov     edi,rxmdata
        call    tick
        ret
n_int endp



foreach proc near
        ;-> esi -> rxmdata
        ;-> edx -> callback

        mov     ecx,[esi].head.hdInstruments
        jmp     @@iweg

@@i_l:  mov     ebx,[esi].instr[ecx*4]
        or      ebx,ebx
        jz      @@iweg

        push    esi
        mov     esi,[esi].sample[ecx*4]

        push    ecx
        movzx   ecx,iSamples[ebx]
                ;instrument-schleife
@@s_l:  push    ecx edx esi

        call    edx                     ;esi -> tsampleh

        pop     esi edx ecx
        add     esi,size tsampleh
        dec     ecx
        jnz     @@s_l

        pop     ecx esi                 ;esi -> trxmdata

@@iweg: dec     ecx
        jns     @@i_l

        ret
foreach endp
;------ player

tick proc near
        ;-> edi -> trxmdata
;    mov eax,'tick'
;    call str_
        mov     eax,[edi].t_usec
        call    add_time

        call    do_flags

        cmp     [edi].xmstatus,xmFade
        jb      @@s
                ;xm l„uft
        cmp     [edi].t_tick,0
        jne     @@0
                ;tick 0
        mov     [edi].t_extra,0
        call    do_pattern

@@0:            ;tick 0 - x
if gamesound gt 0                       ;gamesounds
        call    gamesounds
endif
        call    do_fx
        call    do_misc

        inc     [edi].t_tick            ;n„chster tick
        mov     eax,[edi].head.hdSpeed
        add     eax,[edi].t_extra
        cmp     eax,[edi].t_tick
        jne     @@1
        mov     [edi].t_tick,0
@@1:

        cmp     [edi].xmstatus,xmFade
        jne     @@weg
                ;xm wird ausgefadet
        dec     [edi].globalvol
        jns     @@weg                   ;lautst„rke bei 0 angekommen
        dec     [edi].xmstatus
        jmp     @@weg

@@s:            ;xm stoppen
        cmp     [edi].xmstatus,xmOff
        je      @@weg
        cmp     [edi].xmstatus,xmStop
        jne     @@d

        lea     esi,[edi].chdata        ;kanal-informationen l”schen
        mov     ecx,[edi].head.hdChannels
@@l:
        test    caktiv[esi],afPlay
        jz      @@5
        xor     caktiv[esi],afPlay or afStop
@@5:
        add     esi,size tchannel
        dec     ecx
        jnz     @@l
@@d:
        dec     [edi].xmstatus


@@weg:  ret
tick endp


do_flags proc near
        lea     esi,[edi].chdata
        mov     ecx,[edi].head.hdChannels
@@ch_l:
        mov     eax,cfinalvol[esi]
        mov     cofinalvol[esi],eax
        mov     eax,crevol[esi]
        mov     corevol[esi],eax
        mov     eax,clivol[esi]
        mov     colivol[esi],eax
if wavConvert gt 0
  mov eax,cenvvol[esi]
  mov coenvvol[esi],eax
  mov eax,cenvpan[esi]
  mov coenvpan[esi],eax
endif
        test    caktiv[esi],afStart    ;start -> play
        jz      @@start
        xor     caktiv[esi],afStart or afPlay
@@start:

        and     caktiv[esi],not (afStop + afVol) ;stop -> inaktiv

        add     esi,size tchannel
        dec     ecx
        jg      @@ch_l

        ret
do_flags endp


do_pattern proc near
;     mov eax,[edi].p_pos
;     call hex_
;     mov eax,[edi].p_rowcnt
;     call hex_

        mov     ecx,[edi].p_rowcnt
        mov     ebp,[edi].p_rows
        mov     ebx,[edi].p_pos
        mov     edx,[edi].p_offs

        shr     [edi].p_break,1
        jc      @@new

        inc     ecx                     ;ecx = p_rowcnt
@@1:    cmp     ecx,ebp                 ;ebp = p_rows
        jb      @@cont
        inc     ebx                     ;ebx = p_pos
        xor     ecx,ecx                 ;ecx = p_rowcnt
@@new:          ;new pattern
        mov     [edi].p_breakrow,ecx
        mov     [edi].p_bufc,0
        cmp     ebx,[edi].head.hdSonglen
        jb      @@next
                ;restart
        mov     ebx,[edi].head.hdRestart  ;ebx = p_pos
if wavConvert gt 0
    mov [edi].rflag,1
endif
@@next:
        movzx   edx,[edi].head.hdPtable[ebx]
                mov     edx,[edi].pattern[edx*4]
        mov     ebp,64
        or      edx,edx
        jz      @@empty

        mov     ebp,patRows[edx]
        mov     eax,patSize[edx]
        add     edx,size tpatternh
        cmp     eax,size tpatternh
        ja      @@1
@@empty:
        xor     edx,edx
@@cont:         ;continue
        mov     [edi].p_rowcnt,ecx
        mov     [edi].p_rows,ebp
        mov     [edi].p_pos,ebx
@@bseek:
        lea     esi,[edi].chdata
if gamesound gt 0                       ;gamesounds
        mov     ecx,[edi].pchannels
else
        mov     ecx,[edi].head.hdChannels
endif

@@ch_l: push    ecx

                ;read number of effects
        xor     eax,eax
        mov     cnote[esi],al
        or      edx,edx                 ;edx -> pattern-data
        jz      @@e                     ;pattern empty
        mov     al,[edx]
        inc     edx
@@e:

        mov     cnofx[esi],al
        mov     ecx,eax
        jmp     @@end
@@fxl:
        movzx   eax,word ptr [edx]
        inc     edx
        inc     edx
        test    al,80h
        jz      @@fx
                                ;note & instrument
        and     al,7Fh
        mov     cnote[esi],al
        mov     al,48
@@fx:           ;effect
        mov     cfx[esi+ecx],al
        mov     cparams[esi+ecx],ah

@@end:
        dec     ecx
        jns     @@fxl
        mov     ctick0[esi],1           ;tick 0 indicator

                ;next channel
        pop     ecx
        add     esi,size tchannel
        dec     ecx
        jnz     @@ch_l

        dec     [edi].p_breakrow
        jns     @@bseek

        mov     [edi].p_offs,edx

@@weg:  ret
do_pattern endp


do_fx proc near
        lea     esi,[edi].chdata
        mov     ecx,[edi].head.hdChannels
@@ch_l:
        push    ecx
                ;init
        mov     crelnote[esi],0

        movzx   ecx,cnofx[esi]
        jmp     @@end

@@fxl:          ;loop
        movzx   ebx,cfx[esi+ecx]
        mov     dl,fxoptions[ebx]

        cmp     ctick0[esi],0           ;tick 0 ?
        je      @@t1_x
                ;tick 0
        mov     al,cparams[esi+ecx]

        test    dl,keep or keep2        ;keep param if zero?
        jz      @@nokeep
        or      al,al                   ;is param zero?
        jz      @@keep
        test    dl,keep2                ;keep double param?
        jz      @@nokeep

        mov     ah,al
        shr     ah,4
        jz      @@keep2
        mov     coldparams[esi+ebx+1],ah;next entry for second param
@@keep2:
        and     al,0Fh
        jz      @@keep
@@nokeep:
        mov     coldparams[esi+ebx],al
@@keep:
        test    dl,t0
        jz      @@end
        jmp     @@param

@@t1_x: test    dl,t1_x
        jz      @@end

@@param:
        movzx   eax,coldparams[esi+ebx]
        test    dl,neg
        jnz     @@neg
        test    dl,sig
        jz      @@exec
                ;param is signed
        movsx   eax,al
        neg     eax                     ;jmp @@exec
@@neg:          ;negate param
        neg     eax
@@exec:
        ;call    fxtab[ebx*4]
        mov     edx,fxtab[ebx*4]
        add     edx,offset fxprocs
        call    edx                     ;eax = param, ebx = fx-no

@@end:
        dec     ecx
        jns     @@fxl
        mov     ctick0[esi],0

        pop     ecx

        add     esi,size tchannel
        dec     ecx
        jnz     @@ch_l

@@weg:  ret
do_fx endp


fxprocs:

ifdef noAppregio
Appregio:
endif
ifdef noToneporta
Toneporta:
endif
ifdef noVibrato
Vibrato:
endif
ifdef noTremolo
Tremolo:
endif
;ifdef noNotecut
;Notecut:
;endif
ifdef noTremor
Tremor:
endif
;ifdef noGvolslide
;Gvolslide:
;endif
ifdef noPosjump
Posjump:
endif
ifdef noPatbreak
Patbreak:
endif
ifdef noPatdelay
Patdelay:
endif
ifdef noSetloop
Setloop:
endif
ifdef noSettempo
Settempo:
endif
ifdef noSampleofs
Sampleofs:
endif
ifdef noNotedelay
Notedelay:
endif
ifdef noKeyoff
Keyoff:
endif
ifdef noSetenvpos
Setenvpos:
endif
ifdef noRetrig
Retrig:
endif
ifdef noMultiretrig
Multiretrig:
endif
emptyproc:
ret

ifndef noAppregio
Appregio proc near
        mov     edx,[edi].t_tick
@@d:    sub     edx,3
        ja      @@d
        jz      @@weg
        inc     edx
        jz      @@z2
                ;zyklus 1
        shr     eax,4
@@z2:           ;zyklus 2
        and     al,0Fh

ifndef noAmiga
                ;amiga
        mov     cappmul[esi],eax
endif

ifndef no2tabs
        test    [edi].head.hdFlags,xmLinear
        jz      @@weg
endif

ifndef noLinear
                ;linear
        shl     eax,6
        add     crelnote[esi],eax
endif

@@weg:  ret
Appregio endp
endif


Addnote proc near
        shl     eax,2
Addnotex:
        add     crnote[esi],eax         ;"anschlag" in final_val
        ret
Addnote endp


ifndef noToneporta
Toneporta proc near
        cmp     ctick0[esi],1           ;tick 0?
        jne     @@1_x
        movzx   eax,cnote[esi]
        dec     eax
        js      @@weg
        sal     eax,6                   ;64 finetune-schritte
        add     eax,ctune[esi]          ;tuning-anteil der alten note
ifndef noAmiga
        call    note2amiga
endif
        mov     cportad[esi],eax        ;ziel-realnote
        mov     cnote[esi],0            ;note l”schen
        jmp     @@weg
@@1_x:
        shl     eax,2

        mov     ebx,cportad[esi]
        or      ebx,ebx
        jz      @@weg

        cmp     ebx,crnote[esi]
        jl      @@t
                ;ziel liegt h”her (vorzeichenbehaftete zahlen)
        add     crnote[esi],eax
        cmp     ebx,crnote[esi]         ;ebx:ziel
        jg      @@weg
                ;ber ziel hinaus
        jmp     @@0
@@t:            ;ziel liegt tiefer
        sub     crnote[esi],eax
        cmp     ebx,crnote[esi]         ;ebx:ziel
        jl      @@weg
                ;unter ziel
@@0:    mov     crnote[esi],ebx

@@weg:  ret
Toneporta endp
endif


ifndef noVibrato
Vibrato proc near
        ;ebx -> effekt-nummer
        movzx   edx,cvibofs[esi]
        mov     dh,coldparams[esi+ebx+2];vibrato control
        and     dh,3
        movsx   edx,[edi].vibrato[edx]
        imul    edx
        sar     eax,3
        sub     crelnote[esi],eax

        cmp     [edi].t_tick,0
        je      @@weg
        mov     dl,coldparams[esi+ebx+1];vibrato speed
        shl     dl,2
        add     cvibofs[esi],dl
@@weg:  ret
Vibrato endp
endif


Setvol proc near

        or      caktiv[esi],afVol
        mov     cvol[esi],eax
        mov     crelvol[esi],0
        ret
Setvol endp


Addvol proc near
        add     eax,cvol[esi]
        jns     @@0
        xor     eax,eax
@@0:    cmp     eax,40h
        jbe     @@1
        mov     eax,40h
@@1:
        mov     cvol[esi],eax
        mov     crelvol[esi],0
        ret
endp


ifndef noTremolo
Tremolo proc near
        movzx   edx,ctreofs[esi]
        mov     dh,coldparams[esi+ebx+2];tremolo control
        and     dh,3
        movsx   edx,[edi].vibrato[edx]
        imul    edx
        sar     eax,4
        mov     crelvol[esi],eax

        mov     dl,coldparams[esi+ebx+1];tremolo speed
        shl     dl,2
        add     ctreofs[esi],dl
        ret
endp
endif


Notecut proc near
ifndef noNotecut
        cmp     eax,[edi].t_tick
        jne     @@weg
endif
Voloff:
        or      caktiv[esi],afVol
        mov     cvol[esi],0
        mov     crelvol[esi],0
@@weg:  ret
endp


ifndef noTremor
Tremor proc near
        mov     edx,eax
        and     eax,0Fh
        shr     edx,4
        inc     eax
        inc     edx
        add     edx,eax

        mov     ebx,[edi].t_tick
@@d:    sub     ebx,edx
        ja      @@d
        neg     ebx
        or      caktiv[esi],afVol
        mov     crelvol[esi],-64
        cmp     ebx,eax
        jb      @@weg
        mov     crelvol[esi],0
@@weg:  ret
endp
endif


Gvolslide proc near
ifndef noGvolslide
        add     eax,[edi].globalvol
        jns     @@0
        xor     eax,eax
@@0:    cmp     eax,40h
        jbe     @@1
        mov     eax,40h
@@1:
endif
Setgvol:
        cmp     [edi].xmstatus,xmPlay
        jne     @@weg
        mov     [edi].globalvol,eax
@@weg:  ret
endp


Addpan proc near
        add     eax,cpan[esi]
        or      ah,ah
        jz      @@0
        sar     eax,31
        inc     eax
        neg     al
@@0:
Setpan:
        mov     cpan[esi],eax
        ret
endp


ifndef noPosjump
Posjump proc near
        mov     [edi].p_break,1           ;1: neues pattern
        mov     [edi].p_pos,eax           ;neue song-position
        mov     [edi].p_rowcnt,0
        ret
endp
endif


ifndef noPatbreak
Patbreak proc near
        cmp     [edi].p_break,0
        jne     @@0
        inc     [edi].p_pos               ;n„chstes pattern
@@0:
        mov     [edi].p_break,1           ;1: neues pattern
        mov     [edi].p_rowcnt,eax
        ret
endp
endif


ifndef noPatdelay
Patdelay proc near
        imul    eax,[edi].head.hdSpeed
        mov     [edi].t_extra,eax
        ret
endp
endif


ifndef noSetloop
Setloop proc near
        or      eax,eax
        jnz     @@0
                ;position merken
        mov     eax,[edi].p_rowcnt
        mov     [edi].p_looppos,eax
        jmp     @@weg
@@0:
        cmp     [edi].p_loopcnt,0
        jg      @@1
        inc     eax
        mov     [edi].p_loopcnt,eax
@@1:
        dec     [edi].p_loopcnt
        jz      @@weg
                ;springen
        mov     [edi].p_break,1
        mov     eax,[edi].p_looppos
        mov     [edi].p_rowcnt,eax
@@weg:  ret
endp
endif


ifndef noSettempo
Settempo proc near
        or      eax,eax
        jnz     @@0
                ;end
        mov     [edi].p_break,1
        mov     eax,[edi].head.hdRestart
        mov     [edi].p_pos,eax
        mov     [edi].p_rowcnt,0
        jmp     @@weg
@@0:            ;set speed
        mov     [edi].head.hdSpeed,eax
@@weg:  ret
endp
endif


Setbpm proc near
        mov     ebx,eax
        mov     eax,1000 * 5/2 * 256
        xor     edx,edx
        div     ebx                     ;eax = dauer eines ticks in æs
        mov     [edi].t_usec,eax

        imul    eax,ebx,65536*2/5       ;frequenz = BPM*2/5
;        imul    eax,65536*2/5       ;frequenz = BPM*2/5

        jmp     [edi].s_vmt.v_bpm
endp


ifndef noSampleofs
Sampleofs proc near
ifdef noResample
        shl     eax,8
else
        push    ecx
        mov     cl,8
        sub     cl,cresample[esi]
        shl     eax,cl
        pop     ecx
endif
        add     csample[esi],eax

        ;cmp     eax,cloope[esi]
        ;jae     @@weg                   ;ber ende hinaus
        ;mov     cpos[esi],eax
@@weg:  ret
endp
endif


ifndef noNotedelay
Notedelay proc near
        cmp     eax,[edi].t_tick
        jb      @@weg
        je      @@0
        xor     ecx,ecx
        jmp     @@weg
@@0:    mov     ctick0[esi],1           ;simulate tick 0

@@weg:  ret
endp
endif


ifndef noKeyoff
Keyoff proc near
        test    caktiv[esi],afAktiv     ;kanal aktiv?
        jz      @@weg

        and     caktiv[esi],not afSustain

        mov     ebx,cinstrh[esi]        ;gltig weil aktiv
        test    iFollows[ebx],2         ;ffVolenv
        jz      Voloff

@@weg:  ret
Keyoff endp
endif


ifndef noSetenvpos
Setenvpos proc near
        test    caktiv[esi],afAktiv     ;kanal aktiv?
        jz      @@weg

        mov     ebx,cinstrh[esi]        ;ebx -> instrument

        mov     dl,iFollows[ebx]
        add     ebx,size tinstrh

        shr     dl,1
        jnc     @@notesmp
        add     ebx,size tnotesmp
@@notesmp:
                ;volume envelope
        shr     dl,1
        jnc     @@weg

        xor     edx,edx
@@l:
        cmp     dl,envLastpoint[ebx]
        jae     @@end
        inc     edx
        movzx   ebp,envPoint[ebx+edx*4]
        sub     eax,ebp
        ja      @@l
@@end:
        add     eax,ebp
        mov     cvolp[esi],edx
        mov     cvolc[esi],ax

@@weg:  ret
endp
endif


ifndef noRetrig
Retrig proc near
        test    caktiv[esi],afPlay
        jz      @@weg

        mov     edx,[edi].t_tick

        or      eax,eax                 ;eax = param
        jnz     @@d
        or      edx,edx                 ;wenn param = 0: retrig bei tick 0
        jnz     @@weg
@@d:
        sub     edx,eax
        ja      @@d
        jnz     @@weg

                ;retrig
        xor     caktiv[esi],afStart or afPlay or afStop

        mov     ebx,csampleh[esi]       ;ebx -> sample header
        mov     eax,sOffset[ebx]
        mov     csample[esi],eax

@@weg:  ret
endp
endif


ifndef noMultiretrig
Multiretrig proc near
;        test    caktiv[esi],afAktiv     ;kanal aktiv?
;        je      @@weg

        inc     cmulti[esi]
        cmp     cmulti[esi],eax
        jb      @@weg
        xor     eax,eax
        mov     cmulti[esi],eax
        mov     crelvol[esi],eax

                ;volume change          ;eax = 0
        push    ecx
        mov     dl,coldparams[esi+ebx+1];dl: volume parameter
        mov     cl,dl
        and     cl,7
        jz      @@w
                cmp     cl,6
        jae     @@0
                ;-1 .. -16 , +1 .. +16
        inc     eax
        test    dl,8
        jnz     @@1
        neg     eax
@@1:
        dec     cl
        shl     eax,cl
        add     eax,cvol[esi]
        jns     @@vol
        xor     eax,eax
        jmp     @@vol

@@0:            ;*2/3, *1/2, *3/2, *2/1
        mov     eax,2
        mov     ebx,3
        test    edx,1
        jz      @@2
        dec     eax
        dec     ebx
@@2:
        test    edx,8
        jz      @@3
        xchg    eax,ebx
@@3:
        imul    cvol[esi]
        idiv    ebx

@@vol:  mov     edx,40h
        cmp     eax,edx
        ja      @@v0
        mov     edx,eax
@@v0:   mov     cvol[esi],edx
@@w:    pop     ecx

                ;retrig
        test    caktiv[esi],afPlay
        jz      @@weg

        xor     caktiv[esi],afStart or afPlay or afStop

        mov     ebx,csampleh[esi]       ;edx -> sample header
                mov     eax,sOffset[ebx]
        mov     csample[esi],eax

@@weg:  ret
endp
endif

Note_instr proc near
        ;-> eax = saved 1-based instrument number

        dec     eax
        js      @@weg                   ;no current instrument

        movzx   edx,cnote[esi]          ;edx = note
        dec     edx
        js      @@instr

                ;neue note
        test    caktiv[esi],afPlay      ;altes sample ggf. stoppen
        jz      @@stop
        xor     caktiv[esi],afPlay or afStop
@@stop:

        mov     ebx,[edi].instr[eax*4]
        mov     cinstrh[esi],ebx        ;ebx -> instrument

        or      ebx,ebx
        jz      @@weg                   ;empty instrument

                ;neues sample
        test    iFollows[ebx],1         ;dh = 0
        jz      @@0
        mov     dh,Notesmp[ebx+edx+(size tinstrh)]
@@0:
        cmp     dh,iSamples[ebx]        ;empty sample
        jae     @@weg

        mov     ebx,[edi].sample[eax*4]
        jmp     @@ss
@@sl:           ;search sample
        add     ebx,size tsampleh ;sLength[ebx]
@@ss:   dec     dh
        jns     @@sl
        inc     dh                      ;dh = 0, dl = note

        mov     csampleh[esi],ebx       ;ebx -> sample header

                ;sample-variablen
        mov     eax,sOffset[ebx]        ;offset = -1: sample nicht geladen
        cmp     eax,-1
        je      @@weg
        mov     csample[esi],eax
        mov     ebp,eax
        add     eax,sLoops[ebx]
        mov     cloops[esi],eax
        add     ebp,sLoope[ebx]
        mov     cloope[esi],ebp

        mov     al,sType[ebx]
        mov     ctype[esi],al

ifndef noResample
        mov     al,sResample[ebx]
        mov     cresample[esi],al
endif
                ;real-note berechnen
        shl     edx,6                   ;64 finetune-schritte

        movsx   ebp,sRelnote[ebx]       ;relative note (ebx -> sample-header)
        sal     ebp,6
        movsx   eax,sFinetune[ebx]      ;finetune
        add     eax,ebp
        add     eax,2*12*64
        mov     ctune[esi],eax          ;tuning-anteil

        add     eax,edx
ifndef noAmiga
        call    note2amiga
endif
        mov     crnote[esi],eax         ;real-note

                ;kanal aktivieren
        or      caktiv[esi],afStart; or afSustain
@@nweg:

@@instr:;instrument bezogene „nderungen (sustain, volume, envelope ,vibrato)
        xor     edx,edx                 ;edx = 0
        cmp     cparams[esi+ecx],dl     ;instrument-nummer vorhanden?
        je      @@weg

                ;sustain
        or      caktiv[esi],afSustain

                ;volume reset
        mov     cvol[esi],edx           ;edx = 0
        mov     crelvol[esi],edx
        mov     cfadeoutvol[esi],32768

                ;envelope reset
        mov     cvolc[esi],dx
        mov     cvolp[esi],edx
if wavConvert gt 0
  mov cenvvol[esi],256
endif
        mov     cpanc[esi],dx
        mov     cpanp[esi],edx
if wavConvert gt 0
  mov cenvpan[esi],80h
endif
                ;autovibrato reset
        mov     cavofs[esi],dl          ;autovibrato-tabellenoffset
        mov     cswcnt[esi],edx         ;sweep-count

                ;vibrato reset
        test    coldparams[esi+11],4
        jnz     @@vib
        mov     cvibofs[esi],dl         ;vibrato reset
@@vib:
                ;tremolo reset
        test    coldparams[esi+20],4
        jnz     @@tre
        mov     ctreofs[esi],dl
@@tre:

        test    caktiv[esi],afAktiv     ;kanal aktiv?
        jz      @@weg

        mov     ebx,csampleh[esi]       ;ebx -> sample-header (gltig weil aktiv)
                ;volume & panning
        movzx   eax,sVol[ebx]
        mov     cvol[esi],eax
        movzx   eax,sPan[ebx]
                mov     cpan[esi],eax

@@weg:  ret
Note_instr endp

ifndef noAmiga
note2amiga proc near
        ;-> eax = realnote
        ;<- eax = amiga-period *(-1)
ifndef no2tabs
        test    [edi].head.hdFlags,xmLinear
        jnz     @@weg
endif
        push    ecx
        or      eax,eax
        jns     @@0
        xor     eax,eax
@@0:
        xor     edx,edx
        mov     ecx,12*64
        div     ecx

        mov     ecx,eax

        mov     eax,[edi].freq[edx*4]
        shr     eax,cl
        neg     eax
        pop     ecx

@@weg:  ret
note2amiga endp
endif

do_misc proc near
        lea     esi,[edi].chdata
        mov     ecx,[edi].head.hdChannels
@@ch_l: push    ecx

;    mov ebx,[edi].head.hdChannels
;    sub ebx,ecx
;    cmp mute[ebx],0
;    je @@next

        test    caktiv[esi],afAktiv     ;kanal aktiv?
        jz      @@next

                ;final volume
        mov     edx,cvol[esi]
        xor     eax,eax
        add     edx,crelvol[esi]        ;0-64
        js      @@0
        mov     eax,64
        cmp     edx,eax
        ja      @@0
        mov     eax,edx
@@0:
        imul    [edi].globalvol         ;global volume
        shr     eax,6-2
        mov     cfinalvol[esi],eax      ;0-256
if wavConvert gt 0
  mov cpatvol[esi],eax
endif
                ;final pan
        mov     eax,cpan[esi]
        mov     cfinalpan[esi],eax
if wavConvert gt 0
  mov cpatpan[esi],eax
endif
        mov     ebx,cinstrh[esi]        ;ebx -> instrument (gltig weil aktiv)
        mov     al,iFollows[ebx]
        add     ebx,size tinstrh

        shr     al,1
        jnc     @@notesmp
        add     ebx,size tnotesmp
@@notesmp:
                ;volume envelope
        shr     al,1
        mov     [edi].tempvar,al        ;tempvar = iFollows
        jnc     @@volenv
        mov     ecx,cvolp[esi]
        mov     bp,cvolc[esi]
        call    envelope
        mov     cvolp[esi],ecx
        mov     cvolc[esi],bp
        movzx   edx,envLastpoint[ebx]
        lea     ebx,[ebx + (size tenvelope)+2 - 11*4 + edx*4]
                ;fade-out
        mov     edx,cfadeoutvol[esi]
        test    caktiv[esi],afSustain   ;sustain?
        jnz     @@v1
        movzx   ebp,word ptr [ebx-2]
                sub     edx,ebp
        jns     @@v0
        xor     edx,edx
@@v0:   mov     cfadeoutvol[esi],edx

@@v1:   imul    edx

if wavConvert gt 0
   shr  eax,15
   mov  cenvvol[esi],eax
   imul cfinalvol[esi]
   shr  eax,8
else
        imul    cfinalvol[esi]
        shrd    eax,edx,8+15            ;8 for finalvol, 15 for fadeout
endif
        mov     cfinalvol[esi],eax
@@volenv:

                ;panning envelope
        shr     [edi].tempvar,1
        jnc     @@panenv
        mov     ecx,cpanp[esi]
        mov     bp,cpanc[esi]
        call    envelope
        mov     cpanp[esi],ecx
        mov     cpanc[esi],bp
if wavConvert gt 0
   mov cenvpan[esi],eax
endif
        movzx   edx,envLastpoint[ebx]
        lea     ebx,[ebx + (size tenvelope) - 11*4 + edx*4]
                ;FinalPan=Pan+(EnvelopePan-128)*(128-Abs(Pan-128))/128;
        mov     edx,cfinalpan[esi]
        mov     ebp,edx
        sub     edx,128
        js      @@p0
        neg     edx
@@p0:   add     edx,128                 ;edx = 128-Abs(Pan-128)

        sub     eax,128
        imul    edx
        sar     eax,7
        add     eax,ebp
        mov     cfinalpan[esi],eax
@@panenv:

        shr     [edi].tempvar,1
        jnc     @@avib
                ;auto vibrato
                movzx   edx,cavofs[esi]
        mov     al,vibRate[ebx]
        add     cavofs[esi],al

        mov     dh,vibType[ebx]
        movsx   eax,[edi].vibrato[edx]    ;wert aus tabelle
        movzx   edx,vibDepth[ebx]
        imul    edx                     ;vibrato-wert * depth

        movzx   ebp,vibSweep[ebx]
        or      ebp,ebp
        jz      @@a0                    ;vibrato-sweep = 0
        mov     edx,cswcnt[esi]
        cmp     edx,ebp
        ja      @@a0
                ;sweep-count noch nicht zu ende
        test    caktiv[esi],afSustain   ;bei "vorzeitigem" release:
        jz      @@avib                  ;kein vibrato mehr
        inc     cswcnt[esi]
        imul    edx                     ;vibrato-wert * sweep-count
        idiv    ebp                     ;/vibrato-sweep
@@a0:           ;sweep-count zu ende
        sar     eax,6                   ;/64
        add     crelnote[esi],eax
@@avib:
                ;inc-val
ifndef no2tabs
        test    [edi].head.hdFlags,xmLinear
        jnz     @@linear
endif
ifndef noAmiga  ;amiga
        mov     ebx,crnote[esi]
        add     ebx,crelnote[esi]
        neg     ebx
                                        ;8363*1712/max-frequenz = min-period
        mov     eax,1070464             ;max-frequenz (C-11) = 8363*2^7
        cmp     ebx,14                  ;min-period = 14
        jl      @@f0
        mov     eax,8363*1712           ;frequenz = 8363*1712/period
        xor     edx,edx
        div     ebx
        cmp     eax,131                 ;8363*2^(-6) (C-(-2)) = min-frequenz
        ja      @@f0
        mov     eax,131
@@f0:                                   ;eax = frequenz
        mov     ebx,cappmul[esi]
        mul     [edi].apptab[ebx*4]
        mov     cappmul[esi],0
ifndef noResample
        mov     cl,cresample[esi]
        shr     eax,cl
endif
endif
ifndef no2tabs
        jmp     @@f3
@@linear:
endif
ifndef noLinear ;linear
        mov     eax,crnote[esi]
        add     eax,crelnote[esi]
        jns     @@f1
        xor     eax,eax
@@f1:

        xor     edx,edx
        mov     ecx,12*64
        div     ecx

        mov     ecx,12
        sub     ecx,eax
        jns     @@f2
        mov     edx,12*64-1
        xor     ecx,ecx
@@f2:
        mov     eax,[edi].freq[edx*4]
        shl     eax,8
ifndef noResample
        add     cl,cresample[esi]
endif
        shr     eax,cl
endif
@@f3:
        mul     [edi].s_freqmul
        mov     cincval[esi],edx

              ;reverse stereo
      mov     ebx,[edi].s_drvmem
      test    [ebx].d_flags,dfReverse
      jz      @@next
      not     byte ptr cfinalpan[esi]
@@next:         ;n„chster kanal
        pop     ecx
        add     esi,size tchannel
        dec     ecx
        jnz     @@ch_l

@@weg:  ret
do_misc endp


envelope proc near
        ;-> ebx -> tenvelope
        ;-> ecx = envelope segment (=1 fr 1. intervall)
        ;-> bp  = segment count
        ;<- eax = envelope value
        ;<- ecx = new nevelope segment
        ;<- bp  = new segment count
        movzx   eax,envPoint[ebx+ecx*4+2]

        cmp     bp,envPoint[ebx+ecx*4]
        jae     @@point
                ;between 2 points       ;iy = ay + ix * (by-ay)
                                        ;          --
                                        ;          bx
        sub     ax,envPoint[ebx+ecx*4-2];ax = (by-ay)
        imul    bp                      ;     *ix
        idiv    envPoint[ebx+ecx*4]     ;     /bx
        add     ax,envPoint[ebx+ecx*4-2];     +ay
        jmp     @@inc

@@point:        ;on a point

                ;S
        test    caktiv[esi],afSustain   ;Sustain?
        setnz   dl

                ;LP
        cmp     cl,envLoope[ebx]        ;on loop-end-point?
        jne     @@0
        or      dl,2
@@0:
                ;SP
        cmp     cl,envSustain[ebx]      ;on sustain-point?
        jne     @@1
        or      dl,4
@@1:
                                        ;     SP  LP   S
                                        ;  ---------------------
                                        ;  0:  0   0   0 : x
                                        ;  1:  0   0   1 : x
                                        ;  2:  0   1   0 : loop
                                        ;  3:  0   1   1 : loop
                                        ;  4:  1   0   0 : x
                                        ;  5:  1   0   1 : sustain
                                        ;  6:  1   1   0 : x
                                        ;  7:  1   1   1 : loop
        cmp     dl,5
        je      @@weg
        xor     ebp,ebp
        dec     ebp                     ;segment count = -1
        cmp     dl,6
        je      @@x
        test    dl,2
        jz      @@x
                ;loop
        mov     cl,envLoops[ebx]
        movzx   eax,envPoint[ebx+ecx*4+2]

@@x:            ;no event
        cmp     cl,envLastpoint[ebx]    ;last point?
        jae     @@weg                   ;last point : segment count = -1

        inc     ecx                     ;next point
        inc     ebp
@@inc:  inc     ebp

@@weg:  ret
envelope endp


if pmw eq 0
code32  ends
endif

end
