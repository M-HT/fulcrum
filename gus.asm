include switches.inc

if pmw gt 0

.386                                    ;pmode/w
.model flat,prolog

extrn _seldata:word
extrn _setirqmask:near

else

.386p
code32  segment para public use32       ;pmode/asm
        assume cs:code32, ds:code32

endif
locals



include int.inc
;rxm
extrn rxmdata:dword, tick:near, foreach:near

public u_test


b equ byte ptr
w equ word ptr
;include ..\h\debug.inc
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
;struc
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
include rxmstruc.inc

tudata struc
 u_info         tdinfo ?

 u_command      dd ?

 u_reg52        dw ?

 u_mem          dd ?            ;0: 0K, 1: 256K, 2: 512K, 3: 768K, 4: 1M
 u_memblock     dd 4 dup(?)

 u_slen         dd ?
 u_extralen     dd ?
 u_memleft      dd ?
 u_fit          dd ?
 u_div          db ?

 u_volumes      db 257 dup(?)
 u_ramptab      db 256 dup(?)

;
; u_dmablock     dd ?
; u_wavpos       dd ?
; u_wavsize      dd ?

 align 4
ends

ld_dmasize = 11   ;2048   ;in samples
dmasize = 1 shl ld_dmasize
dmablocks = 32768/dmasize

if pmw gt 0
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
.data
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
endif

;u_voice   = base + 102h
;u_command = base + 103h
;u_datalo  = base + 104h
;u_datahi  = base + 105h
;
;u_dramio  = base + 107h


if enhanced gt 0
iw_memconfig label dword
dd 00000001h ;0
dd 00000101h ;1
dd 01010101h ;2
dd 00000401h ;3
dd 04040401h ;4
dd 00040101h ;5
dd 04040101h ;6
dd 00000004h ;7
dd 00000404h ;8
dd 04040404h ;9
dd 00000010h ;A
dd 00001010h ;B
endif

;pageport db 87h,83h,81h,82h,8Fh,8Bh,89h,8Ah

u_vmt  tsvmt <u_init, u_done, u_setvol, u_play, u_bpm>;, u_waveplay>

if pmw gt 0
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
.code
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
endif

comment #
if incGusenv gt 0
public gusenv

gusenv proc
        ;esi -> tdinfo
        ;es:edi -> environment

        mov     ecx,size tdinfo/4 -1
@@l:
        mov     dword ptr ds:[esi+ecx*4],0
        dec     ecx
        jns     @@l

        xor     ecx,ecx
        dec     ecx
        jmp     @@scanentry
@@envscan:
        xor     eax,eax
        repne   scasb
@@scanentry:
        cmp     byte ptr es:[edi],0
        je      @@weg
if enhanced gt 0                                ;INTERWAVE=
        cmp     dword ptr es:[edi],45544E49h    ;INTE
        jne     @@u
        cmp     dword ptr es:[edi+4],56415752h  ;RWAV
        jne     @@u
        cmp     word ptr es:[edi+8],3D45h       ;E=
        jne     @@u
        or      [esi].d_flags,gus_iw
@@u:
endif
                                                ;ULTRASND=
        cmp     dword ptr es:[edi],52544C55h    ;ULTR
        jne     @@envscan
        cmp     dword ptr es:[edi+4],444E5341h  ;ASND
        jne     @@envscan
        cmp     byte ptr es:[edi+8],'='
        jne     @@envscan

        cmp     byte ptr es:[edi+9],' '
        mov     al,es:[edi+10]
        jne     @@0
        mov     al,es:[edi+11]
@@0:
        sub     al,'0'
        shl     al,4
        mov     ah,2
        mov     [esi].d_base,eax
if enhanced gt 0
        jmp     @@envscan
endif

@@weg:
        ret
gusenv endp
endif
#

u_test proc near
        ;-> edi -> tdinfo
        ;<- eax = 0: gus nicht vorhanden, 1: gus vorhanden
        push    ebp

                ;GUS-reset
        mov     edx,[edi].d_base        ;Line out aus(!)
        mov     al,2
        out     dx,al

        lea     ebp,[edx + 103h]

        mov     edx,ebp;command
        mov     al,4ch                  ;Init-Register
        out     dx,al
        inc     edx
        inc     edx;datahi
        xor     al,al
        out     dx,al
        call    u_delay

        mov     edx,ebp;command
        mov     al,4ch
        out     dx,al
        inc     edx
        inc     edx;datahi
        mov     al,1
        out     dx,al
        call    u_delay

;        mov     ebx,1
        call    u_memsize
        xor     eax,eax
        jecxz   @@weg

        mov     edx,ebp;command         ;DMA Control Register resetten
        mov     al,41h
        out     dx,al
        inc     edx
        inc     edx;datahi
        mov     al,0
        out     dx,al

        mov     edx,ebp;command         ;Timer Control Register resetten
        mov     al,45h
        out     dx,al
        inc     edx
        inc     edx;datahi
        mov     al,0
        out     dx,al

        mov     edx,ebp;command         ;Sampling Control Register resetten
        mov     al,49h
        out     dx,al
        inc     edx
        inc     edx;datahi
        mov     al,0
        out     dx,al

        mov     edx,[edi].d_base        ;Evtl. DMA Interrupts leeren
        add     dl,6;status
        in      al,dx

        mov     edx,ebp;command
        mov     al,41h
        out     dx,al
        inc     edx
        inc     edx;datahi
        in      al,dx

        mov     edx,ebp;command         ;Evtl. Sampling Interrupts leeren
        mov     al,49h
        out     dx,al
        inc     edx
        inc     edx;datahi
        in      al,dx

        mov     edx,ebp;command         ;IRQ Status Register lesen
        mov     al,8Fh                  ;==> Es liegen jetzt keine unbearbeiteten
        out     dx,al                   ;    Interrupts an
        inc     edx
        inc     edx;datahi
        in      al,dx

        mov     [edi].d_mem,size tudata
        lea     eax,u_vmt
        mov     [edi].d_vmt,eax
;
        test    [edi].d_flags,dfWave    ;dmabuf if wave-output
        jz      @@0
        mov     [edi].d_dmabuf,dmasize*2;2 byte pro sample
@@0:
;
        or      [edi].d_flags,df16bit or dfStereo
        mov     [edi].d_rate,44100

if enhanced gt 0
        test    [edi].d_flags,gus_iw
        setnz   [edi].d_ver
endif

        xor     eax,eax
        inc     eax
@@weg:  pop     ebp
        ret
u_test endp


u_init proc near
        ;edi -> tudata

        mov     ebp,[edi].d_base        ;command port
        add     ebp,103h
        mov     [edi].u_command,ebp


if enhanced gt 0
        cmp     [edi].d_ver,0
        jne     @@enh
endif
                ;old GUS
        call    u_memsize               ;mem size
        mov     [edi].u_mem,ecx

        mov     edx,ebp;command         ;Anzahl Stimmen zunÑchst auf 32
        mov     al,0Eh
        out     dx,al
        inc     edx
        inc     edx;datahi
        mov     al,0C0h or 31
        out     dx,al

if enhanced gt 0
        jmp     @@old0

@@enh:          ;IW

        mov     edx,ebp;command         ;enhanced mode
        mov     al,19h
        out     dx,al
        inc     edx
        inc     edx;datahi
        mov     al,1
        out     dx,al

        mov     edx,ebp;command         ;memory config = 0Ch
        mov     al,52h
        out     dx,al
        inc     edx
        in      ax,dx

        and     ax,0FFF0h
        mov     [edi].u_reg52,ax
        or      al,0Ch
        mov     bx,ax

        mov     edx,ebp;command
        mov     al,52h
        out     dx,al
        inc     edx
        mov     ax,bx
        out     dx,ax

        mov     edx,ebp;command         ;enable ram access, auto inc
        mov     al,53h
        out     dx,al
        inc     edx
        inc     edx
        in      al,dx

        and     al,0FCh
        inc     al
        mov     bl,al

        mov     edx,ebp;command
        mov     al,53h
        out     dx,al
        inc     edx
        inc     edx
        mov     al,bl
        out     dx,al

        call    iw_findmem              ;ch = memory config

        mov     edx,ebp;command         ;write memory config
        mov     al,52h
        out     dx,al
        inc     edx
        mov     ax,[edi].u_reg52
        or      al,ch
        out     dx,ax

@@old0:
endif

   call resetchannels
comment #
                ;GUS-init
        mov     cl,31                   ;In Schleife die Stimmen ausschalten
@@vc_l:

        mov     edx,ebp                 ;Stimme wÑhlen
        dec     edx;voice
        mov     al,cl
        out     dx,al

        mov     edx,ebp;command         ;Stimme stoppen
        xor     al,al
        out     dx,al
        inc     edx
        inc     edx;datahi
        mov     al,3
        out     dx,al

        mov     edx,ebp;command         ;volume-ramping stoppen
        mov     al,0dh
        out     dx,al
        inc     edx
        inc     edx;datahi
        mov     al,3
        out     dx,al

;        mov     edx,ebp;command         ;Ramping-Faktor setzen
;        mov     al,6
;        out     dx,al
;        inc     edx
;        inc     edx;datahi
;        mov     al,31                   ;schnellst mîglich = 63
;        out     dx,al

        mov     edx,ebp;command         ;LautstÑrke auf 0 setzen
        mov     al,9
        out     dx,al
        inc     edx;datalo
        ;xor     eax,eax
        mov     ax,4000h
        out     dx,ax

if enhanced gt 0
        cmp     [edi].d_ver,0
        je      @@old1

        mov     edx,ebp;command         ;stimme aktiv & altes panning
        mov     al,15h
        out     dx,al
        inc     edx
        inc     edx;datahi
        mov     al,0
        out     dx,al

        mov     edx,ebp;command         ;Effekte Resetten
        mov     al,14h
        out     dx,al
        inc     edx
        inc     edx;datahi
        mov     al,0
        out     dx,al

        mov     edx,ebp;command         ;Effekt LautstÑrke
        mov     al,16h
        out     dx,al
        inc     edx
        inc     edx;datahi
        xor     eax,eax
        out     dx,ax

        mov     edx,ebp;command         ;Effekt LautstÑrke Fein
        mov     al,1Dh
        out     dx,al
        inc     edx
        inc     edx;datahi
        xor     eax,eax
        out     dx,ax
@@old1:
endif
        dec     cl
        jns     @@vc_l                  ;fÅr alle Stimmen wiederholen
#

        mov     edx,ebp;command         ;Reset durchfÅhren
        mov     al,4ch
        out     dx,al
        inc     edx
        inc     edx;datahi
        mov     al,7                    ;GF1 Master IRQ einschalten
        out     dx,al

        mov     edx,[edi].d_base         ;Line out & dma an
        mov     al,8;0
        out     dx,al
    xor eax,eax
        ret
u_init endp

resetchannels proc near
        ;-> edi -> tudata
        ;-> ebp = u_command

                ;GUS-init
        mov     cl,31                   ;In Schleife die Stimmen ausschalten
@@vc_l:

        mov     edx,ebp                 ;Stimme wÑhlen
        dec     edx;voice
        mov     al,cl
        out     dx,al

        mov     edx,ebp;command         ;Stimme stoppen
        xor     al,al
        out     dx,al
        inc     edx
        inc     edx;datahi
        mov     al,3
        out     dx,al

        mov     edx,ebp;command         ;volume-ramping stoppen
        mov     al,0dh
        out     dx,al
        inc     edx
        inc     edx;datahi
        mov     al,3
        out     dx,al

;        mov     edx,ebp;command         ;Ramping-Faktor setzen
;        mov     al,6
;        out     dx,al
;        inc     edx
;        inc     edx;datahi
;        mov     al,31                   ;schnellst mîglich = 63
;        out     dx,al

        mov     edx,ebp;command         ;LautstÑrke auf 0 setzen
        mov     al,9
        out     dx,al
        inc     edx;datalo
        ;xor     eax,eax
        mov     ax,4000h
        out     dx,ax

if enhanced gt 0
        cmp     [edi].d_ver,0
        je      @@old1

        mov     edx,ebp;command         ;stimme aktiv & altes panning
        mov     al,15h
        out     dx,al
        inc     edx
        inc     edx;datahi
        mov     al,0
        out     dx,al

        mov     edx,ebp;command         ;Effekte Resetten
        mov     al,14h
        out     dx,al
        inc     edx
        inc     edx;datahi
        mov     al,0
        out     dx,al

        mov     edx,ebp;command         ;Effekt LautstÑrke
        mov     al,16h
        out     dx,al
        inc     edx
        inc     edx;datahi
        xor     eax,eax
        out     dx,ax

        mov     edx,ebp;command         ;Effekt LautstÑrke Fein
        mov     al,1Dh
        out     dx,al
        inc     edx
        inc     edx;datahi
        xor     eax,eax
        out     dx,ax
@@old1:
endif
        dec     cl
        jns     @@vc_l                  ;fÅr alle Stimmen wiederholen

@@weg:  ret
resetchannels endp


u_done proc near
        ;edi -> tudata

        mov     edx,[edi].d_base           ;Line out aus(!)
        mov     al,2
        out     dx,al
        ret
u_done endp

u_setvol proc near
        ;-> eax = vol (0..32*8), ideal = 12*8
                ;gus volumes
        imul    eax,2200/(12*8)         ;gus max vol = 4096, standard = 2200
        mov     ecx,256                 ;257 volumes (0-256)
@@l:
        mov     edx,ecx
        imul    edx,eax
        shr     edx,8
      mov ebx,3500                      ;vol should be less than 3500
      cmp edx,ebx
      ja @@0
      mov ebx,edx
@@0:
        inc     ebx
        mov     edx,10000h
@@s:
        sub     edx,1000h
        shl     ebx,1
        btr     ebx,12
        jnc     @@s

        or      ebx,edx
        mov     [edi].u_volumes[ecx],bh

        dec     ecx
        jns     @@l
        ret
u_setvol endp

u_play proc near
        ;edi -> tudata
        ;esi -> rxmdata
 mov ebp,[edi].u_command
 call resetchannels

        call    u_freq

ifndef noResample
if enhanced gt 0
        cmp     [edi].d_ver,0
        je      @@old
        call    iw_xm2mem
        jmp     @@0
@@old:
endif
endif
        call    u_xm2mem

@@0:
        ret
u_play endp

u_bpm proc near
        ;edi -> rxmdata
        ;eax = frequenz*65536
        mov     edx,offset u_int
        call    newhandler
        ret
u_bpm endp

;---wave play-----------------------------------------------------------------
comment #

u_waveplay proc near
        ;esi -> wavedata
        ;eax = len
        ;edi -> tudata

        mov     [edi].u_wavpos,esi
        mov     [edi].u_wavsize,eax

        mov     [edi].u_dmablock,dmablocks

        mov     ebp,[edi].u_command

                ;gus-mem lîschen
        mov     bl,1
@@hi:   mov     edx,ebp;command         ;Hi-Byte der GUS-DRAM Adresse setzen
        mov     al,44h
        out     dx,al
        inc     edx
        inc     edx;datahi
        mov     al,bl
        out     dx,al

        mov     ecx,32767+16
@@lo:   mov     edx,ebp;command         ;Lo-Word der GUS-DRAM Adresse setzen
        mov     al,43h
        out     dx,al
        inc     edx;datalo
        mov     ax,cx
        out     dx,ax

        add     edx,3;dramio            ;0 ausgeben
        xor     al,al
        out     dx,al

        dec     ecx
        jns     @@lo
        dec     bl
        jns     @@hi


if enhanced gt 0
        cmp     [edi].d_ver,0
        je      @@old0

        mov     edx,ebp;command
        mov     al,50h
        out     dx,al
        inc     edx
        inc     edx;datahi
        xor     al,al
        out     dx,al

@@old0:
endif
        mov     edx,ebp;command         ;Anzahl Stimmen auf 14
        mov     al,0Eh
        out     dx,al
        inc     edx
        inc     edx;datahi
        mov     al,0C0h or (14-1)
        out     dx,al

                                        ;2 stimmen starten
        mov     cl,1
@@vl0:
        mov     edx,ebp                 ;Stimme wÑhlen
        dec     edx;voice
        mov     al,cl
        out     dx,al

        mov     edx,ebp;command         ;44100 Hz
        mov     al,01h
        out     dx,al
        inc     edx
        mov     ax,1024
        out     dx,ax

        movzx   ebx,cl                  ;start
        imul    ebx,dmablocks*dmasize
        mov     ch,02h
        call    pos_out
        mov     ch,0ah                  ;actual address
        call    pos_out

        add     ebx,dmablocks*dmasize-1   ;end
        mov     ch,04h
        call    pos_out

        mov     edx,ebp;command         ;Panning
        mov     al,0Ch
        out     dx,al
        inc     edx
        inc     edx;datahi
        mov     al,cl
        imul    eax,0fh
        out     dx,al

        mov     edx,ebp;command         ;volume
        mov     al,09h
        out     dx,al
        inc     edx
        mov     ax,65000
        out     dx,ax

        dec     cl
        jns     @@vl0

        mov     eax,(102400/dmasize)*65536  ;install handler
        mov     edx,offset u_waveint
        call    newhandler

        mov     cl,1
@@vl1:
        mov     edx,ebp                 ;Stimme wÑhlen
        dec     edx;voice
        mov     al,cl
        out     dx,al

        mov     edx,ebp;command         ;voice control
        mov     al,00h
        out     dx,al
        inc     edx
        inc     edx;datahi
        mov     al,0Ch                  ;16 bit (04h), Loop (08h)
        out     dx,al

        dec     cl
        jns     @@vl1


;@@t0:
;  call u_waveint
;  jmp @@t0

@@weg:  ret
u_waveplay endp

u_waveint proc near
        mov     ebx,rxmdata
        mov     edi,[ebx].s_drvmem
        mov     ebp,[edi].u_command

;        cmp     [edi].u_dmaactive,0
;        jne     @@weg

        cmp     [edi].u_wavsize,dmasize*4
        jl      @@kill                  ;ebx -> trxmdata

        mov     esi,[edi].u_wavpos
        mov     ebx,[edi].u_dmablock
        shr     ebx,1
        jc      @@0

        mov     edx,ebp                 ;select voice 0
        dec     edx;voice
        mov     al,0
        out     dx,al

        mov     edx,ebp;command         ;get actual address
        mov     al,8Ah
        out     dx,al
        inc     edx
        xor     eax,eax
        in      ax,dx
        and     eax,1FFFFh shr 7
        shr     eax,ld_dmasize-7
        cmp     eax,ebx
        je      @@weg

        jmp     @@1

@@0:    add     esi,2
        add     [edi].u_wavpos,dmasize*4
        sub     [edi].u_wavsize,dmasize*4
@@1:
        mov     ecx,dmasize
        mov     ebx,[edi].d_dmabuf
@@copy:
        mov     ax,[esi]
        add     esi,4
        mov     [ebx],ax
        add     ebx,2
        dec     ecx
        jnz     @@copy

        call    u_dmaupload

        jmp     @@weg
@@kill:
        mov     [ebx].xmstatus,xmOff    ;ebx -> trxmdata

        mov     eax,0                   ;remove handler
        mov     edx,offset u_waveint
        call    newhandler

        mov     cl,1
@@vl0:
        mov     edx,ebp                 ;Stimme wÑhlen
        dec     edx;voice
        mov     al,cl
        out     dx,al

        mov     edx,ebp;command         ;voice control
        mov     al,00h
        out     dx,al
        inc     edx
        inc     edx;datahi
        mov     al,03h                  ;stop
        out     dx,al

        dec     cl
        jns     @@vl0

@@weg:  ret
u_waveint endp

u_dmaupload proc near

;        mov     edx,ebp;command         ;DMA Control Register resetten
;        mov     al,41h
;        out     dx,al
;        inc     edx
;        inc     edx;datahi
;        mov     al,0
;        out     dx,al
;        call    u_delay

        mov     edx,ebp;command         ;DMA Control Register resetten
        mov     al,41h
        out     dx,al
        inc     edx
        inc     edx;datahi
        in      al,dx

        mov     edx,ebp;command         ;dma dest address
        mov     al,42h
        out     dx,al
        inc     edx

        mov     eax,[edi].u_dmablock
        shr     eax,1
        jnc     @@0
        add     eax,dmablocks+0
@@0:    shl     eax,ld_dmasize-3

        cmp     [edi].d_dma1,4
        jae     @@dma16

        out     dx,ax                   ;startaddress/8 (in 16 bit samples)
        call    u_dma8
        jmp     @@1
@@dma16:
        shr     eax,1                   ;startaddress/8 (in 16 bit samples)
        out     dx,ax
        call    u_dma16
@@1:

;        mov     [edi].u_dmaactive,1
        mov     edx,ebp;command         ;start dma
        mov     al,41h
        out     dx,al
        inc     edx
        inc     edx;datahi
        mov     al,[edi].d_dma1         ;8/16 bit select
        and     al,4
        add     al,0C1h
        out     dx,al

        inc     [edi].u_dmablock
        cmp     [edi].u_dmablock,dmablocks*2
        jb      @@weg
        mov     [edi].u_dmablock,0
        sub     [edi].u_wavpos,4
        add     [edi].u_wavsize,4

;        and     [edi].u_dmablock,dmablocks*2-1

@@weg:  ret
u_dmaupload endp

u_dma8 proc near

        movzx   ebx,[edi].d_dma1        ;dma kanal wÑhlen
        mov     al,bl
        or      al,4
        out     0ah,al

        xor     al,al                   ;flip-flop-reset
        out     0ch,al

        mov     al,bl                   ;dma channel
        add     al,08h; +10h             ;transfer mode (+autoinit)
        out     0bh,al


        mov     edx,ebx                 ;adresse und lÑnge setzen
        shl     edx,1                   ;edx = 00h/02h/04h/06h
        mov     eax,[edi].d_dmabuf
        out     dx,al                   ;lo(offset)
        mov     al,ah
        out     dx,al                   ;hi(offset)
        inc     edx
        mov     ax,dmasize*2-1          ;dmasize in samples
        out     dx,al                   ;size = FFFFh
        mov     al,ah
        out     dx,al

        mov     dl,pageport[ebx]        ;page-port
        mov     al,b [edi].d_dmabuf[2]
        out     dx,al

        mov     al,bl                   ;dma freigeben
        out     0ah,al

        ret
u_dma8 endp

u_dma16 proc near

        movzx   ebx,[edi].d_dma1        ;dma kanal wÑhlen
        mov     al,bl                   ;bit 2 ist gesetzt
        out     0d4h,al
        and     bl,3

        xor     al,al                   ;flip-flop-reset
        out     0d8h,al

        mov     al,bl                   ;dma channel
        add     al,08h; +10h             ;transfer mode (+autoinit)
        out     0d6h,al

        lea     edx,[ebx*4+0c0h]        ;adresse und lÑnge setzen
                                        ;edx = c0h/c4h/c8h/cch
        mov     eax,[edi].d_dmabuf
        shr     eax,1
        out     dx,al                   ;lo(offset)
        mov     al,ah
        out     dx,al                   ;hi(offset)

        inc     edx
        inc     edx
        mov     ax,dmasize-1
        out     dx,al                   ;size
        mov     al,ah
        out     dx,al

        mov     dl,pageport[ebx+4]      ;page-port
        mov     al,b [edi].d_dmabuf[2] ;page
        ;and     al,0FEh
        out     dx,al

        mov     al,bl                   ;dma freigeben
        out     0d4h,al

        ret
u_dma16 endp
#
;-----------------------------------------------------------------------------


u_delay proc near
        ;<- al = dram

        push    ecx
        push    edx
        mov     ecx,16

@@l:    mov     edx,[edi].d_base
        add     edx,107h;dramio
        in      al,dx
        loop    @@l

        pop     edx
        pop     ecx
        ret
u_delay endp

u_setadr proc near
        ;eax = adresse

        push    eax

        mov     edx,[edi].d_base
        add     edx,103h;command        ;Lo-Word
        mov     al,43h
        out     dx,al
        inc     edx;datalo
        pop     eax                     ;eax = adresse
        out     dx,ax

        dec     edx;command             ;Hi-Byte
        mov     al,44h
        out     dx,al
        inc     edx
        inc     edx;datahi
        shr     eax,16
        out     dx,al

        inc     edx
        inc     edx;dramio

@@weg:  ret
u_setadr endp



u_memsize proc near
        ;-> edi -> rxmdata
        ;-> ebp = u_command
        ;<- ecx = anzahl 256K-mem-blîcke

        xor     ecx,ecx
@@mem:
        mov     ebx,ecx
        shl     ebx,18

        mov     eax,ebx
        call    u_setadr
        mov     al,0AAh
        out     dx,al

        lea     eax,[ebx+1]
        call    u_setadr
        mov     al,055h
        out     dx,al

        mov     eax,ebx
        call    u_setadr

        call    u_delay
        cmp     al,0AAh
        jne     @@weg

        inc     ecx
        cmp     ecx,4
        jb      @@mem
@@weg:
        ;mov     [edi].u_mem,ecx
        ret
u_memsize endp


if enhanced gt 0
iw_findmem proc near
        ;-> edi = tudata
        ;-> ebp = u_command
        ;<- ch = lmcfi

                ;detect memory configuration
        xor     ebx,ebx
@@m_l0:
        xor     ecx,ecx
@@m_l1:
        mov     eax,ebx                ;ebx*4M + ecx*256k markieren
        shl     eax,4
        add     eax,ecx
        shl     eax,18
        call    u_setadr
        mov     al,0AAh
        add     al,cl
        out     dx,al
        not     al
        out     dx,al

        mov     eax,ebx                ;anfang auf Åberschreibung ÅberprÅfen
        shl     eax,22
        call    u_setadr
        in      al,dx
        cmp     al,0AAh
        jne     @@next
        in      al,dx
        cmp     al,055h
        jne     @@next

        mov     eax,ebx                ;ebx*4M + ecx*256k prÅfen
        shl     eax,4
        add     eax,ecx
        shl     eax,18
        call    u_setadr
        mov     ah,0AAh
        add     ah,cl
        in      al,dx
        cmp     al,ah
        jne     @@next
        not     ah
        in      al,dx
        cmp     al,ah
        jne     @@next

        inc     ecx
        cmp     ecx,16
        jb      @@m_l1
@@next:
        shrd    esi,ecx,8               ;esi = mem-config
        inc     ebx
        cmp     ebx,4
        jb      @@m_l0

                ;find a suitable memory config number
        xor     ecx,ecx                 ;cl : config counter (0 - 0Bh)
        xor     edx,edx                 ;dl : mem size with actual config
@@rep:                                  ;ch : max-mem config
        mov     eax,esi                 ;dh : max-mem
        movzx   ebx,cl
        mov     ebx,iw_memconfig[ebx*4] ;get config
        xor     dl,dl
@@l:
        cmp     al,bl
        jb      @@last
        add     dl,bl
        shr     ebx,8
        shr     eax,8
        jnz     @@l

@@last: add     dl,al
        cmp     dh,dl
        jae     @@10
        mov     dh,dl
        mov     ch,cl
@@10:
        inc     cl
        cmp     cl,0Bh
        jbe     @@rep

        movzx   edx,dh
        mov     [edi].u_mem,edx         ;edx = mem in 256K blocks
@@weg:  ret
iw_findmem endp

endif

u_freq proc near
        ;esi -> trxmdata
        ;edi -> tudata


if enhanced gt 0
        mov     ebx,14

        cmp     [edi].d_ver,0
        jne     @@enh
endif
        mov     ebx,[esi].head.hdChannels

        mov     edx,[edi].u_command      ;aktive kanÑle
        mov     al,0Eh
        out     dx,al
        inc     edx
        inc     edx
        cmp     bl,14
        jae     @@chan
        mov     bl,14
@@chan:
        mov     al,bl
        dec     al
        or      al,0C0h
        out     dx,al
@@enh:

        ;incval = f * (1024 * 1.619695497 * channels)/1000000

        imul    eax,ebx,7123496 /256    ;*(1024 * 1.619695497)/1000000 * 2^24
        mov     [esi].s_freqmul,eax


        ;ramp-faktoren
        mov     ecx,255  ;s = d*16/(f*t)
                         ;f = 1000000/1.619695497 * channels Hz
                         ;t = 2.2 ms

                         ;16* 1.619695497 /(1000000*0.0022) * 256 = 3
@@rl:                    ;s = d * 3 * channels / 256
        lea     eax,[ecx*2+ecx]         ;*3
        imul    ebx
        cmp     ah,63
        jbe     @@0
        mov     ah,63
@@0:    or      ah,ah
        jnz     @@1
        inc     ah
@@1:
        mov     [edi].u_ramptab[ecx],ah
        dec     ecx
        jns     @@rl

        ret
u_freq endp

ifdef noResample
;sample data < 256k
u_xm2mem proc near
        mov     ebp,10h                 ;ram-offset (=10h wegen pingpong-bug)

        mov     edx,offset u_upload
        call    foreach

        ret
u_xm2mem endp

;called by foreach
u_upload proc near
        ;-> edi -> tudata
        ;-> esi -> tsampleh

        mov     ebx,esi

        mov     esi,[ebx].sDataptr      ;esi -> sampledaten

        mov     ecx,sLoope[ebx]
        inc     ecx

ifdef no16bit
        mov     sOffset[ebx],ebp
else
        mov     eax,ebp
        test    sType[ebx],sf16bit
        jz      @@8
                ;16 bit
        shl     ecx,1
        shr     eax,1
@@8:
        mov     sOffset[ebx],eax
endif

                ;kopieren
@@hi:   mov     edx,[edi].u_command     ;Hi-Byte der GUS-DRAM Adresse setzen
        mov     al,44h
        out     dx,al
        inc     edx
        inc     edx;datahi
        mov     eax,ebp
        shr     eax,16
        out     dx,al
@@lo:   mov     edx,[edi].u_command     ;Lo-Word der GUS-DRAM Adresse setzen
        mov     al,43h
        out     dx,al
        inc     edx;datalo
        mov     eax,ebp
        out     dx,ax

        add     edx,3;dramio          ;Byte laden und ausgeben
        lodsb
        out     dx,al

        inc     ebp
        dec     ecx
        jz      @@weg
        or      bp,bp
        jnz     @@lo
        jz      @@hi                    ;Åberlauf wenn bp = 0

@@weg:  ret
u_upload endp

else
;sample data > 256k
u_len proc near
        ;-> esi -> tsampleh
        ;<- eax = sample-lÑnge
        ;<- ebx = extra-lÑnge

        mov     eax,sLoope[esi]         ;eax = sample-lÑnge ohne extra-bytes
        mov     ebx,1                   ;ebx = extra-bytes

        cmp     eax,256
        ja      @@weg
        add     ebx,eax                 ;sample kÅrzer als 256
        xor     eax,eax
@@weg:  ret
u_len endp


u_xm2mem proc near

        mov     [edi].u_slen,0
        mov     [edi].u_extralen,10h
        mov     [edi].u_fit,1           ;passt-flag

        mov     ecx,3
@@0:    xor     eax,eax
        cmp     ecx,[edi].u_mem
        jb      @@1
        mov     eax,40000h              ;u_memblock = 0 : leer
@@1:    mov     [edi].u_memblock[ecx*4],eax
        dec     ecx
        jnz     @@0
        mov     [edi].u_memblock[0],10h ;wegen pingpong-bug

        mov     edx,offset u_sizecount
        call    foreach

        cmp     [edi].u_fit,0
        je      @@res
                ;passt

        mov     edx,offset u_upload
        call    foreach
        jmp     @@weg

@@res:          ;resample
        mov     eax,[edi].u_mem
        shl     eax,18
        mov     [edi].u_memleft,eax

        sub     eax,[edi].u_extralen
        mov     edx,[edi].u_slen

        mov     [edi].u_div,-1           ;faktor bestimmen
@@div_l:
        inc     [edi].u_div
        shr     edx,1
        cmp     edx,eax
        jl      @@2
        cmp     [edi].u_div,4
        jb      @@div_l
@@2:

        mov     ebp,10h                 ;ram-offset (=10h wegen pingpong-bug)

        mov     edx,offset u_resample
        call    foreach

@@weg:  ret
u_xm2mem endp

;called by foreach
u_sizecount proc near
        ;-> edi -> tudata
        ;-> esi -> tsampleh

        call    u_len
        add     u_slen[edi],eax         ;eax = sample-lÑnge
        add     u_extralen[edi],ebx     ;ebx = extra-lÑnge

        mov     ecx,sLoope[esi]
        inc     ecx


        test    sType[esi],sf16bit
        jnz     @@16bit
                ;8 bit                  ;verteilung des samples auf 4 blîcke
        xor     ebx,ebx                 ;ebx = startblock
@@a0:
        xor     edx,edx                 ;edx = begrenzung
        mov     eax,ebx

@@a1:   inc     eax                     ;fÅr jeden folgenden freien block
        add     edx,40000h              ; wird begrenzung um 256k erhîht
        cmp     eax,4
        jae     @@a2
        cmp     [edi].u_memblock[eax*4],0
        je      @@a1
@@a2:
        mov     ebp,[edi].u_memblock[ebx*4];ebp = offset im 256k block
        mov     eax,ebp
        add     eax,ecx                 ;eax = belegte byte ab aktuellem block
        cmp     eax,edx
        jbe     @@a3                    ;passt?

        inc     ebx                     ;nÑchster block als startblock
        cmp     ebx,4
        jb      @@a0
        mov     [edi].u_fit,0           ;speicher reicht nicht
        jmp     @@weg

@@a3:
        mov     edx,ebx                 ;blocknummer = obere 2 bit der adresse
        shl     edx,18
        or      ebp,edx                 ;ebp = adresse im gus-ram

                                        ;zahl in eax auf aktuellen und
        mov     edx,40000h              ; folgende blîcke verteilen (max. 256k)
@@a4:
        cmp     eax,edx;40000h          ;pa·t rest in den block?
        jbe     @@a5
        mov     [edi].u_memblock[ebx*4],edx;block voll
        inc     ebx                     ;nÑchster block
        sub     eax,edx;40000h
        jmp     @@a4
@@a5:
        mov     [edi].u_memblock[ebx*4],eax;rest

        mov     sOffset[esi],ebp        ;Offset im gus-ram speichern

        jmp     @@weg

@@16bit:        ;16 bit
        shl     ecx,1
        xor     ebx,ebx                 ;verteilung des samples auf 4 blîcke
@@s0:
        mov     ebp,[edi].u_memblock[ebx*4];ebp = offset im 256k block
        inc     ebp
        and     ebp,not 1               ;align 2

        mov     eax,ebp
        add     eax,ecx
        cmp     eax,40000h              ;sample mu· in einen block passen
        jbe     @@s1

        inc     ebx                     ;nÑchster block (0-3)
        cmp     ebx,4
        jb      @@s0
        mov     [edi].u_fit,0           ;speicher reicht nicht
        jmp     @@weg
@@s1:
        mov     [edi].u_memblock[ebx*4],eax


        shl     ebx,18
        or      ebp,ebx
        mov     sOffset[esi],ebp

@@weg:  ret
u_sizecount endp

;called by foreach
u_upload proc near
        ;-> edi -> tudata
        ;-> esi -> tsampleh

        mov     ebx,esi

        mov     esi,[ebx].sDataptr      ;esi -> sampledaten

        mov     ecx,sLoope[ebx]
        inc     ecx
        mov     ebp,sOffset[ebx]

        test    sType[ebx],sf16bit
        jz      @@8
                ;16 bit
        shl     ecx,1

        mov     eax,ebp
        and     eax,3FFFFh              ;16 bit adresse berechnen
        shr     eax,1
        mov     edx,ebp
        and     edx,0C0000h
        or      eax,edx
        mov     sOffset[ebx],eax
@@8:

                ;kopieren
@@hi:   mov     edx,[edi].u_command     ;Hi-Byte der GUS-DRAM Adresse setzen
        mov     al,44h
        out     dx,al
        inc     edx
        inc     edx;datahi
        mov     eax,ebp
        shr     eax,16
        out     dx,al
@@lo:   mov     edx,[edi].u_command     ;Lo-Word der GUS-DRAM Adresse setzen
        mov     al,43h
        out     dx,al
        inc     edx;datalo
        mov     eax,ebp
        out     dx,ax

        add     edx,3;dramio          ;Byte laden und ausgeben
        lodsb
        out     dx,al

        inc     ebp
        dec     ecx
        jz      @@weg
        or      bp,bp
        jnz     @@lo
        jz      @@hi                    ;Åberlauf wenn bp = 0

@@weg:  ret
u_upload endp

;called by foreach
u_resample proc near
        ;-> edi -> tudata
        ;-> esi -> tsampleh

        mov     cl,u_div[edi]
        mov     eax,u_slen[edi]
        shr     eax,cl
        add     eax,u_extralen[edi]     ;eax = aktueller platzbedarf
        cmp     eax,u_memleft[edi]
        jle     @@0
        inc     cl                      ;platzbedarf zu gro·
@@0:
        call    u_len                   ;eax = sample-lÑnge
                                        ;ebx = extra-lÑnge
        sub     u_slen[edi],eax
        sub     u_extralen[edi],ebx

        shr     eax,cl                  ;sample-lÑnge = 0: nicht komprimierbar
        jnz     @@1
        xor     cl,cl
@@1:
        add     eax,ebx                 ;eax = gesamter platzbedarf
        sub     u_memleft[edi],eax
        mov     sOffset[esi],-1
;   jns @@t0
;   mov eax,'voll'
;   call str_
;   jmp @@weg
        js      @@weg                   ;mem voll
;@@t0:
        mov     sOffset[esi],ebp
        mov     sResample[esi],cl

        shr     sLoops[esi],cl
        shr     sLoope[esi],cl

        mov     eax,sLoope[esi]         ;eax = samples to copy
        mov     ebx,1
        shl     ebx,cl                  ;ebx = step
        xor     edx,edx                 ;edx = offset

        call    u_move                  ;samples Åbertragen

        mov     eax,1
        xor     ebx,ebx
        mov     edx,sLoops[esi]

        test    sType[esi],sfLoop
        jz      @@5
        test    sType[esi],sfPingpong
        jz      @@6
@@5:    mov     edx,sLoope[esi]
@@6:
        shl     edx,cl
        call    u_move

        and     sType[esi],not sf16bit

@@weg:  ret
u_resample endp

u_move proc near
        ;-> eax = samples to copy
        ;-> ebx = step
        ;-> edx = offset
        push    ecx esi
        mov     ecx,eax

        test    sType[esi],sf16bit
        jz      @@8
        shl     edx,1
        inc     edx
        shl     ebx,1

@@8:
        ;lea     esi,[esi + size tsampleh + edx]
        mov     esi,[esi].sDataptr
        add     esi,edx

                ;kopieren
@@hi:   mov     edx,[edi].u_command     ;Hi-Byte der GUS-DRAM Adresse setzen
        mov     al,44h
        out     dx,al
        inc     edx
        inc     edx;datahi
        mov     eax,ebp
        shr     eax,16
        out     dx,al
@@lo:   mov     edx,[edi].u_command     ;Lo-Word der GUS-DRAM Adresse setzen
        mov     al,43h
        out     dx,al
        inc     edx;datalo
        mov     eax,ebp
        out     dx,ax

        add     edx,3;dramio          ;Byte laden und ausgeben
        mov     al,[esi]
        add     esi,ebx
        out     dx,al

        inc     ebp
        dec     ecx
        jz      @@weg
        or      bp,bp
        jnz     @@lo
        jz      @@hi                    ;Åberlauf wenn bp = 0

@@weg:  pop     esi ecx
        ret
u_move endp

if enhanced gt 0
iw_xm2mem proc near
        ;-> edi -> tudata
        ;-> esi -> trxmdata

        xor     ebp,ebp                 ;ram-offset (bei iw geht 0)

        mov     eax,ebp
        call    u_setadr

        mov     u_slen[edi],0
        mov     u_extralen[edi],ebp
        mov     u_fit[edi],ebp

        mov     edx,offset iw_sizecount
        call    foreach

        mov     eax,u_mem[edi]
        shl     eax,18                  ;eax = mem size
        cmp     eax,u_fit[edi]
        jb      @@res
                ;passt

        mov     edx,offset iw_upload
        call    foreach
        jmp     @@weg

@@res:          ;resample
        mov     u_memleft[edi],eax

        sub     eax,u_extralen[edi]
        mov     edx,u_slen[edi]

        mov     u_div[edi],-1           ;faktor bestimmen
@@div_l:
        inc     u_div[edi]
        shr     edx,1
        cmp     edx,eax
        jl      @@0
        cmp     u_div[edi],4
        jb      @@div_l
@@0:


        mov     edx,offset iw_resample
        call    foreach


@@weg:  ret
iw_xm2mem endp


;called by foreach
iw_sizecount proc near
        ;-> edi -> tudata
        ;-> esi -> tsampleh

        call    u_len                   ;lÑngen bezogen auf 8 bit

        add     u_slen[edi],eax         ;eax = sample-lÑnge
        add     u_extralen[edi],ebx     ;ebx = extra-lÑnge


        add     eax,ebx
        test    sType[esi],sf16bit
        jz      @@8
                ;16 bit
        shl     eax,1
@@8:
        inc     eax
        and     eax,not 1               ;aufrunden

        add     u_fit[edi],eax
        ret
iw_sizecount endp

;called by foreach
iw_upload proc near
        ;-> edi -> tudata
        ;-> esi -> tsampleh

        mov     ebx,esi

;        add     esi,size tsampleh
        mov     esi,[ebx].sDataptr

        mov     ecx,sLoope[ebx]
        inc     ecx
        mov     eax,ebp

        test    sType[ebx],sf16bit
        jz      @@8bit
        shl     ecx,1
        shr     eax,1
@@8bit:
        mov     sOffset[ebx],eax        ;Offset im gus-ram speichern

        inc     ecx
        shr     ecx,1                   ;/2 (aufrunden)
        add     ebp,ecx
        add     ebp,ecx

        mov     edx,[edi].u_command
        mov     al,51h
        out     dx,al
        inc     edx
        rep     outsw

        ret
iw_upload endp

;called by foreach
iw_resample proc near
        ;-> edi -> tudata
        ;-> esi -> tsampleh

        mov     cl,u_div[edi]
        mov     eax,u_slen[edi]
        shr     eax,cl
        add     eax,u_extralen[edi]     ;eax = aktueller platzbedarf
        cmp     eax,u_memleft[edi]
        jle     @@0
        inc     cl                      ;platzbedarf zu gro·
@@0:
        call    u_len                   ;eax = sample-lÑnge
                                        ;ebx = extra-lÑnge
        sub     u_slen[edi],eax
        sub     u_extralen[edi],ebx

        shr     eax,cl                  ;sample-lÑnge = 0: nicht komprimierbar
        jnz     @@1
        xor     cl,cl
@@1:
        add     eax,ebx                 ;eax = gesamter platzbedarf
        sub     u_memleft[edi],eax
        mov     sOffset[esi],-1
;   jns @@t0
;   mov eax,'voll'
;   call str_
;   jmp @@weg
        js      @@weg                   ;mem voll
;@@t0:
        mov     sOffset[esi],ebp
        mov     sResample[esi],cl
;        or      sType[esi],sfResampled


        shr     sLoops[esi],cl
        shr     sLoope[esi],cl

        mov     eax,sLoope[esi]         ;eax = samples to copy
        mov     ebx,1
        shl     ebx,cl                  ;ebx = step
        xor     edx,edx                 ;edx = offset

        call    iw_move                  ;samples Åbertragen

        mov     eax,1
        xor     ebx,ebx
        mov     edx,sLoops[esi]

        test    sType[esi],sfLoop
        jz      @@5
        test    sType[esi],sfPingpong
        jz      @@6
@@5:    mov     edx,sLoope[esi]
@@6:
        shl     edx,cl
        call    iw_move

        and     sType[esi],not sf16bit

@@weg:  ret
iw_resample endp


iw_move proc near
        ;-> eax = samples to copy
        ;-> ebx = step
        ;-> edx = offset
        push    ecx esi
        mov     ecx,eax

        test    sType[esi],sf16bit
        jz      @@8
        shl     edx,1
        inc     edx
        shl     ebx,1

@@8:
;        lea     esi,[esi + size tsampleh + edx]
        mov     esi,[esi].sDataptr
        add     esi,edx


                ;kopieren
        mov     edx,[edi].d_base        ;Byte laden und ausgeben
        add     edx,107h;dramio
@@l:
        mov     al,[esi]
        add     esi,ebx
        out     dx,al

        inc     ebp
        dec     ecx
        jnz     @@l

        pop     esi ecx
        ret
iw_move endp
endif
endif



u_int proc near
        mov     edi,rxmdata

        mov     eax,4
        mov     edx,offset u_change
        call    newhandler              ;3 ms one-shot

        call    tick

;---
                                        ;fÅr regulÑre Ñnderung und stop
                                        ; nicht fÅr start
        lea     esi,[edi].chdata
        mov     ecx,[edi].head.hdChannels
        mov     edi,[edi].s_drvmem
        mov     ebp,[edi].u_command
        dec     ecx
@@ch_l:
        ;lautstÑrke-ramping
        test    caktiv[esi],afPlay or afStop
        jz      @@l_weg                 ;kein ramping nîtig

        mov     edx,ebp                 ;Stimme wÑhlen
        dec     edx;voice
        mov     al,cl
        out     dx,al

                ;start-lautstÑrke
        mov     edx,cofinalvol[esi]     ;lautstÑrke im vorigen tick
        mov     bl,[edi].u_volumes[edx] ;bl = start-lautstÑrke

                ;end-lautstÑrke
        mov     bh,40h;18
        test    caktiv[esi],afStop      ;wird stimme gleich gestoppt?
        jnz     @@ramp

                ;stimme lÑuft weiter
        mov     edx,cfinalvol[esi]      ;0-256
        mov     bh,[edi].u_volumes[edx]

@@ramp: call    u_ramp                  ;bl = start, bh = ende

@@l_weg:

        add     esi,size tchannel
        dec     ecx                     ;alle kanÑle...
        jns     @@ch_l

        ret
u_int endp


u_ramp proc near
        ;bl = start, bh = ende

        mov     al,8                    ;al : ramping obere lautstÑrke
        xor     ah,ah                   ;ah = modus-byte

        movzx   edx,bh
        sub     dl,bl
        jz      @@weg
        ja      @@0
                ;abwÑrts
        dec     al                      ;ramping untere lautstÑrke
        mov     ah,40h                  ;modus byte: abwÑrts rampen
        neg     dl
@@0:
        mov     bl,[edi].u_ramptab[edx] ;bl = ramp-faktor

        mov     edx,ebp;command         ;ziel-lautstÑrke setzen
        out     dx,al
        inc     edx
        inc     edx;datahi
        mov     al,bh
        out     dx,al

        mov     edx,ebp;command         ;ramp-faktor setzen
        mov     al,6
        out     dx,al
        inc     edx
        inc     edx;datahi
        mov     al,bl
        out     dx,al

        mov     edx,ebp;command         ;ramping richtung im volume control
        mov     al,0dh                  ; register setzen
        out     dx,al
        inc     edx
        inc     edx;datahi
        mov     al,ah
        out     dx,al

@@weg:  ret
u_ramp endp


u_change proc near
        mov     edi,rxmdata

        lea     esi,[edi].chdata
        mov     cl,b [edi].head.hdChannels
        mov     edi,[edi].s_drvmem
        mov     ebp,[edi].u_command
        dec     cl
@@ch_l:
        test    caktiv[esi],afStart or afPlay or afStop
        jz      @@l_weg                 ;keine aktion erforderlich

        mov     edx,ebp                 ;Stimme wÑhlen
        dec     edx;voice
        mov     al,cl
        out     dx,al

        ;stimme stoppen
        test    caktiv[esi],afStop
        jz      @@aktiv

        mov     edx,ebp;command         ;Stimme anhalten
        xor     al,al
        out     dx,al
        inc     edx
        inc     edx;datahi
        mov     al,3
        out     dx,al

comment #
        mov     edx,ebp;command          ;LautstÑrke auf 0
        mov     al,9
        out     dx,al
        inc     edx;datalo
        mov     ax,4000h
        out     dx,ax
#
        test    caktiv[esi],afStart     ;gleich wieder starten?
        jz      @@l_weg

@@aktiv:;Ñnderung der stimme (kanal aktiv)

                ;frequenz
        mov     edx,ebp;command         ;Befehl Voicefreqenz schreiben
        mov     al,1
        out     dx,al
        mov     eax,cincval[esi]        ;bei GUS: cincval = gus-frequenz
        inc     edx;datalo
        out     dx,ax

                ;panning
        mov     edx,ebp;command         ;Set Pan-Position
        mov     al,0Ch
        out     dx,al
        inc     edx
        inc     edx;datahi
        mov     al,b cfinalpan[esi]
        shr     al,4
        out     dx,al

        ;stimme starten
        test    caktiv[esi],afStart
        jz      @@l_weg

                ;Ramping anhalten
        mov     edx,ebp;command
        mov     al,0dh
        out     dx,al
        inc     edx
        inc     edx;datahi
        mov     al,3
        out     dx,al

                ;Loop-Start setzen
        mov     ebx,cloops[esi]
        mov     ch,02h
        call    pos_out

                ;Loop-Ende setzen
        mov     ebx,cloope[esi]
        mov     ch,04h
        call    pos_out

                ;lautstÑrke initialisieren
        mov     edx,ebp;command
        mov     al,9
        out     dx,al
        inc     edx;datalo
        mov     ax,4000h
        out     dx,ax

                ;Stimmenanfang setzen
        mov     ebx,csample[esi]
        mov     ch,0Ah
        call    pos_out

                ;volume ramping
        mov     edx,cfinalvol[esi]
        mov     bl,40h
        mov     bh,[edi].u_volumes[edx]
        call    u_ramp

        mov     edx,ebp;command         ;Voice Mode (loop, 8/16 bit)
        xor     al,al
        out     dx,al
        inc     edx
        inc     edx;datahi
        mov     al,ctype[esi]
;      and al,sfLoop + sfPingpong + sf16bit
        out     dx,al                   ;stimme startet jetzt...
@@l_weg:

        add     esi,size tchannel
        dec     cl                      ;alle kanÑle...
        jns     @@ch_l
@@weg:  ret
u_change endp



pos_out proc near
        mov     edx,ebp;command         ;HI-anteil
        mov     al,ch
        out     dx,al
        inc     edx
        mov     eax,ebx
        shr     eax,7
        out     dx,ax
        dec     edx                     ;LO-anteil
        mov     al,ch
        inc     al
        out     dx,al
        inc     edx
        mov     eax,ebx
        shl     eax,9
        out     dx,ax

        ret
pos_out endp

if pmw eq 0
code32  ends
endif

end

