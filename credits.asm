.486                                    ;pmode/w
.model flat,prolog

locals

include int.inc

public creddata
public initcdata
public docredits

extrn keypressed:near

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
;constants
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
NULL = 0
maxint = 7FFFFFFFh

b equ byte ptr
w equ word ptr
d equ dword ptr

ld_fvalues = 5
fvalues = 32

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
;struc
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±

;vesa-info
include vesa.inc
extrn clearlinbuf:near

tpal struc
 p_red          db 256 dup(?)
 p_green        db 256 dup(?)
 p_blue         db 256 dup(?)
ends

tcredit struc
 cr_xres        dd ?
 cr_yres        dd ?
 cr_picdata     dd ?
 cr_xpos        dd ?
 cr_ypos        dd ?
 cr_starttime   dd ?
 cr_startstep   dd ?
 cr_fadespeed   dd ?
 cr_paltab      dd ?
ends

tcreditdef struc
 d_xpos         dd ?
 d_credit       dd 2 dup(?)
ends

tpaltab struc
 pt_red         dd ?
 pt_green       dd ?
 pt_blue        dd ?
 pt_trans       dd ?
 pt_paltab      dd ?
ends

tcreddata struc
 cd_xres        dd ?
 cd_yres        dd ?
 cd_pal         dd ?    ;-> tpal
 cd_picdata     dd ?
 cd_starttime   dd ?
 cd_scrollspeed dd ?
 cd_scrolltime  dd ?
 cd_enginetime  dd ?

 cd_credits     dd ?    ;-> array of tcredit

 cd_nocreditdefs dd ?
 cd_creditdefs  dd ?    ;-> array of tcreditdef

 cd_nopaltabs   dd ?
 cd_paltabs     dd ?    ;16 bit: fvalues*256*2, 32 bit: fvalues*256*4
 cd_paltab      dd ?

 cd_creditnum   dd ?
ends



;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
.data
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±



creddata tcreddata ?

pbytes  dd ?
xbytes  dd ?
xres    dd ?
yres    dd ?
linbuf  dd ?
bufend  dd ?


xstep   dd ?
ystep   dd ?

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
.code
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±



mosaic proc pascal
        ;-> esi -> tcredit
arg     gstep, dpic
local   t
local   xa,xe, ya,ye
local   xaw,xew, yaw,yew
local   x,y


;  do {

        mov     eax,gstep
        sub     eax,256
        shl     eax,8
        xor     edx,edx

        mov     ebx,256
        sub     ebx,[esi].cr_startstep

        idiv    ebx
        add     eax,256
        mov     t,eax                   ;transparency (0..256)


        mov     eax,[esi].cr_yres       ;gmy = yres << 7
        sal     eax,8-1
        cdq                             ;ya = gmy % gstep - gstep
        idiv    gstep
        sub     edx,gstep
        mov     ya,edx


@@do1:          ;do {
        mov     eax,ya                  ;yaw = ya >> 8
        sar     eax,8
        jns     @@0                     ;if (yaw < 0) yaw = 0
        xor     eax,eax
@@0:    mov     yaw,eax

        mov     eax,ya                  ;ye = ya + gstep
        add     eax,gstep
        mov     ye,eax
        sar     eax,8                   ;yew = ye >> 8
        cmp     eax,[esi].cr_yres
        jl      @@1                     ;if (yew > yres) yew = yres
        mov     eax,[esi].cr_yres
@@1:    mov     yew,eax


        mov     eax,[esi].cr_xres       ;gmx = xres << 7
        sal     eax,8-1
        cdq                             ;xa = gmx % gstep - gstep;
        idiv    gstep
        sub     edx,gstep
        mov     xa,edx

@@do2:          ;do {
        mov     eax,xa                  ;xaw = xa >> 8
        sar     eax,8
        jns     @@2                     ;if (xaw < 0) xaw = 0
        xor     eax,eax
@@2:    mov     xaw,eax

        mov     eax,xa                  ;xe = xa + gstep
        add     eax,gstep
        mov     xe,eax
        sar     eax,8                   ;xew = xe >> 8
        cmp     eax,[esi].cr_xres
        jl      @@3                     ;if (xew > xres) xew = xres
        mov     eax,[esi].cr_xres
@@3:    mov     xew,eax


        mov     ebx,yaw                 ;ebx = yaw
        mov     eax,yew
        sub     eax,ebx
        jle     @@w0
        mov     y,eax

        mov     edx,xaw                 ;edx = xaw
        mov     eax,xew
        sub     eax,edx
        jle     @@w0
        mov     x,eax

        imul    ebx,[esi].cr_xres       ;ebx = offset
        add     ebx,edx

        mov     edi,[esi].cr_picdata    ;edi -> source-pic
        add     edi,ebx                 ;edi -> source-square
        xor     eax,eax                 ;col = 0
        mov     ch,b y
@@yl0:
        mov     cl,b x
@@xl0:

        movzx   edx,b [edi]
        add     eax,edx
        inc     edi
        dec     cl
        jnz     @@xl0
        sub     edi,x
        add     edi,[esi].cr_xres
        dec     ch
        jnz     @@yl0

        mov     ecx,x                   ;ebx = count = x*y
        imul    ecx,y
        imul    t
        idiv    ecx                     ;col /= count

        mov     edi,dpic                ;edi -> dest-pic
        add     edi,ebx
        mov     ch,b y
@@yl1:
        mov     cl,b x
@@xl1:
        mov     [edi],ah
        inc     edi
        dec     cl
        jnz     @@xl1
        sub     edi,x
        add     edi,[esi].cr_xres
        dec     ch
        jnz     @@yl1

@@w0:
        mov     eax,xe                  ;xa = xe
        mov     xa,eax

        mov     eax,xew                 ;} while (xew < xres)
        cmp     eax,[esi].cr_xres
        jl      @@do2

        mov     eax,ye                  ;ya = ye
        mov     ya,eax

        mov     eax,yew                 ;} while (yew < yres)
        cmp     eax,[esi].cr_yres
        jl      @@do1


@@weg:  ret
mosaic endp



copypic proc pascal
        ;-> eax = actual ypos
        ;-> edx = end ypos
        ;-> esi -> tcredit
        ;-> edi -> pos in framebuffer
        ;<- eax = new ypos
        ;<- edi -> new pos in framebuffer
arg     xpos
local   ypos, yend

        shl     edx,8
        cmp     eax,edx
        jge     @@weg
        mov     yend,edx
        mov     ebx,creddata.cd_paltab
@@yl:
        mov     ypos,eax
        movzx   esi,ah
        imul    esi,creddata.cd_xres
;        add     esi,xpos
        add     esi,creddata.cd_picdata
        mov     eax,xpos                ;eax = xpos

        push    edi
        push    ebp
        mov     ecx,xstep
        movzx   ebp,ch
        ;xor     ch,ch

        mov     ch,al                   ;ch xpos fraction
        shr     eax,8
        add     esi,eax                 ;add xpos whole part

        cmp     pbytes,3;or      bppflag,0
;        jz      @@24
;        jns     @@32
        ja      @@32
        je      @@24

;------ 16 bit
@@16:
        mov     edx,xres
@@l_16: movzx   eax,b [esi]
        add     ch,cl
        mov     ax,[ebx+eax*2]
        adc     esi,ebp
        mov     [edi],ax
        add     edi,2
        dec     edx
        jnz     @@l_16

        jmp     @@endcase
;------ 24 bit
@@24:
        mov     edx,xres
@@l_24: movzx   eax,b [esi]
        add     ch,cl
        mov     eax,[ebx+eax*4]
        adc     esi,ebp
        mov     [edi],ax
        bswap   eax
        mov     [edi+2],ah
        add     edi,3
        dec     edx
        jnz     @@l_24

        jmp     @@endcase
;------ 32 bit
@@32:
        mov     edx,xres
@@l_32: movzx   eax,b [esi]
        add     ch,cl
        mov     eax,[ebx+eax*4]
        adc     esi,ebp
        mov     [edi],eax
        add     edi,4
        dec     edx
        jnz     @@l_32

;------
@@endcase:

        pop     ebp
        pop     edi
        add     edi,xbytes
        cmp     edi,bufend
        jae     @@weg

        mov     eax,ypos
        add     eax,ystep
        cmp     eax,yend
        jl      @@yl

@@weg:  ret
copypic endp

copycpic proc pascal
        ;-> eax = actual ypos
        ;-> esi -> tcredit
        ;-> edi -> pos in framebuffer
        ;<- eax = new ypos
        ;<- edi -> new pos in framebuffer
arg     xpos, mpic
local   ypos, yend
local   credit
local   c1, c2, c3, z, hiadd
local   frac:byte

        mov     edx,[esi].cr_ypos
        add     edx,[esi].cr_yres
        shl     edx,8
        cmp     eax,edx
        jge     @@weg
        mov     yend,edx
        mov     ypos,eax

        mov     ebx,xstep               ;ebx = xstep
        mov     ecx,xres
        mov     eax,[esi].cr_xpos
        shl     eax,8
        xor     edx,edx
        div     ebx
        sub     ecx,eax
        mov     c1,eax
        mov     eax,[esi].cr_xres
        shl     eax,8
        xor     edx,edx
        div     ebx
        sub     ecx,eax
        mov     c2,eax
        mov     c3,ecx

        mov     cl,bl                   ;cl = loadd
        shr     ebx,8
        mov     hiadd,ebx

        mov     credit,esi

        mov     ebx,[esi].cr_paltab
@@yl:
        movzx   esi,b ypos[1]
        imul    esi,creddata.cd_xres
        mov     eax,xpos
        mov     ch,al
        shr     eax,8
        add     esi,eax;xpos
        add     esi,creddata.cd_picdata

;        xor     ch,ch

        push    edi

        cmp     pbytes,3;or      bppflag,0
;        jz      @@24
;        jns     @@32
        ja      @@32
        je      @@24
;------ 16 bit
@@16:
        mov     edx,c1
@@l1_16:movzx   eax,b [esi]
        add     ch,cl
        mov     ax,[ebx+eax*2]
        adc     esi,hiadd
        mov     [edi],ax
        add     edi,2
        dec     edx
        jnz     @@l1_16

        mov     eax,credit
        movzx   edx,b ypos[1]
        sub     edx,[eax].cr_ypos
        imul    edx,[eax].cr_xres
        add     edx,mpic
        ;sub     edx,esi
        mov     frac,0

        mov     eax,c2
        mov     z,eax
@@l2_16:movzx   eax,b [esi]
        add     ch,cl
        mov     ah,[edx]
        adc     esi,hiadd
        add     frac,cl
        mov     ax,[ebx+eax*2]
        adc     edx,hiadd
        mov     [edi],ax
        add     edi,2
        dec     z
        jnz     @@l2_16

        mov     edx,c3
@@l3_16:movzx   eax,b [esi]
        add     ch,cl
        mov     ax,[ebx+eax*2]
        adc     esi,hiadd
        mov     [edi],ax
        add     edi,2
        dec     edx
        jnz     @@l3_16

        jmp     @@endcase
;------ 24 bit
@@24:
        mov     edx,c1
@@l1_24:movzx   eax,b [esi]
        add     ch,cl
        mov     eax,[ebx+eax*4]
        adc     esi,hiadd
        mov     [edi],ax
        bswap   eax
        mov     [edi+2],ah
        add     edi,3
        dec     edx
        jnz     @@l1_24

        mov     eax,credit
        movzx   edx,b ypos[1]
        sub     edx,[eax].cr_ypos
        imul    edx,[eax].cr_xres
        add     edx,mpic
;        sub     edx,esi
        mov     frac,0

        mov     eax,c2
        mov     z,eax
@@l2_24:movzx   eax,b [esi]
        add     ch,cl
        mov     ah,[edx]
        adc     esi,hiadd
        mov     eax,[ebx+eax*4]
        add     frac,cl
        mov     [edi],ax
        adc     edx,hiadd
        bswap   eax
        mov     [edi+2],ah
        add     edi,3
        dec     z
        jnz     @@l2_24

        mov     edx,c3
@@l3_24:movzx   eax,b [esi]
        add     ch,cl
        mov     eax,[ebx+eax*4]
        adc     esi,hiadd
        mov     [edi],ax
        bswap   eax
        mov     [edi+2],ah
        add     edi,3
        dec     edx
        jnz     @@l3_24

        jmp     @@endcase
;------ 32 bit
@@32:
        mov     edx,c1
@@l1_32:movzx   eax,b [esi]
        add     ch,cl
        mov     eax,[ebx+eax*4]
        adc     esi,hiadd
        mov     [edi],eax
        add     edi,4
        dec     edx
        jnz     @@l1_32

        mov     eax,credit
        movzx   edx,b ypos[1]
        sub     edx,[eax].cr_ypos
        imul    edx,[eax].cr_xres
        add     edx,mpic
;        sub     edx,esi
        mov     frac,0

        mov     eax,c2
        mov     z,eax
@@l2_32:movzx   eax,b [esi]
        add     ch,cl
        mov     ah,[edx]
        adc     esi,hiadd
        add     frac,cl
        mov     eax,[ebx+eax*4]
        adc     edx,hiadd
        mov     [edi],eax
        add     edi,4
        dec     z
        jnz     @@l2_32

        mov     edx,c3
@@l3_32:movzx   eax,b [esi]
        add     ch,cl
        mov     eax,[ebx+eax*4]
        adc     esi,hiadd
        mov     [edi],eax
        add     edi,4
        dec     edx
        jnz     @@l3_32
;------
@@endcase:

        pop     edi
        add     edi,xbytes

        mov     eax,ypos
        add     eax,ystep
        mov     ypos,eax
        cmp     eax,yend
        jl      @@yl

@@weg:  ret
copycpic endp


docredits proc pascal
arg     mpic;, creditnum            ;mpic: mosaic picture
        ;-> eax = frame
        ;<- eax = scrolltime
local   creditdef
local   mainstart, time
local   ypos, fbuf, xpos
local   last:byte

  lea esi,creddata
  sub eax,[esi].cd_starttime        ;eax = relative time
  jl @@no_c
        mov     edx,maincount
        sub     edx,eax
        mov     mainstart,edx

  mov edx,[esi].cd_enginetime
  add [esi].cd_starttime,edx

  mov eax,[esi].cd_creditnum      ;credit number
  mov edx,eax
  inc edx
  cmp edx,[esi].cd_nocreditdefs
  setae last
  inc [esi].cd_creditnum

        imul    eax,size tcreditdef
        add     eax,[esi].cd_creditdefs
        mov     creditdef,eax


    call    clearlinbuf

@@fl:

        mov     eax,maincount           ;time
        sub     eax,mainstart
        mov     time,eax

        imul    eax,creddata.cd_scrollspeed ;xpos
        sar     eax,10-8
        mov     ebx,creditdef
        mov     edx,[ebx].d_xpos
        shl     edx,8
        add     eax,edx
        mov     edx,creddata.cd_xres
        sub     edx,320
        shl     edx,8
        cmp     eax,edx
        jl      @@0
        mov     eax,edx
@@0:
        mov     ebx,eax                 ;round to multiple of xstep
        xor     edx,edx
        div     xstep
        sub     ebx,edx
        mov     xpos,ebx

        mov     ypos,0
        mov     edi,linbuf
        mov     fbuf,edi

        xor     ecx,ecx

@@cl:           ;credit loop
        mov     ebx,creditdef
        mov     esi,[ebx].d_credit[ecx*4]
        or      esi,esi
        jz      @@w0

        mov     eax,[esi].cr_starttime
        sub     eax,time
        jg      @@w0

        push    ecx

        push    eax esi
        mov     eax,ypos
        mov     edx,[esi].cr_ypos
        mov     edi,fbuf
        call    copypic pascal, xpos    ;only copy picture
        mov     fbuf,edi
        mov     ypos,eax
        pop     esi eax

        imul    eax,[esi].cr_fadespeed
        sar     eax,10
        add     eax,[esi].cr_startstep
        mov     ebx,[esi].cr_picdata    ;ebx -> mpic
        cmp     eax,256
        jle     @@1
        call    mosaic pascal, eax, mpic
        mov     ebx,mpic                ;ebx -> mpic
@@1:
        mov     edi,fbuf
        mov     eax,ypos
        call    copycpic pascal, xpos, ebx ;copy pic with credit
        mov     fbuf,edi
        mov     ypos,eax

        pop     ecx
@@w0:
        inc     ecx
        cmp     ecx,2
        jb      @@cl


        mov     eax,ypos                ;copy rest
        mov     edx,1000
        mov     edi,fbuf
        call    copypic pascal, xpos


        call    keypressed
        or      eax,eax
;        mov     ah,1
;        int     16h
        jnz     @@end
;    xor     eax,eax
;    int     16h
;    cmp     al,27
;    je      @@weg
;    add maincount,33

        mov     eax,creddata.cd_scrolltime
        cmp     time,eax
        jl      @@fl

        cmp     last,1
        je      @@end

        call    clearlinbuf
        mov     eax,creddata.cd_scrolltime
        jmp     @@weg

@@end:  xor     eax,eax
        dec     eax
        jmp     @@weg

@@no_c: xor     eax,eax
@@weg:  ret
docredits endp





initpaltab proc pascal
        ;-> ebx -> tpaltab
        ;-> esi -> tvesa
local   col, z, dmul

        mov     z,fvalues-1
@@fl:
        mov     eax,z
        imul    eax,[ebx].pt_trans
        mov     dmul,eax

        mov     edx,255
@@pl:
        mov     edi,creddata.cd_pal

        movzx   eax,b [edi].p_red[edx]  ;red
        mov     ecx,[ebx].pt_red
        sub     ecx,eax
        imul    ecx,dmul;z
        shr     ecx,ld_fvalues+8
        add     eax,ecx
        mov     cl,[esi].vesa_redbits
        shl     eax,cl
        shr     eax,8
        mov     cl,[esi].vesa_redpos
        shl     eax,cl
        mov     col,eax

        movzx   eax,b [edi].p_green[edx];green
        mov     ecx,[ebx].pt_green
        sub     ecx,eax
        imul    ecx,dmul;z
        shr     ecx,ld_fvalues+8
        add     eax,ecx
        mov     cl,[esi].vesa_greenbits
        shl     eax,cl
        shr     eax,8
        mov     cl,[esi].vesa_greenpos
        shl     eax,cl
        or      col,eax

        movzx   eax,b [edi].p_blue[edx] ;blue
        mov     ecx,[ebx].pt_blue
        sub     ecx,eax
        imul    ecx,dmul;z
        shr     ecx,ld_fvalues+8
        add     eax,ecx
        mov     cl,[esi].vesa_bluebits
        shl     eax,cl
        shr     eax,8
        mov     cl,[esi].vesa_bluepos
        shl     eax,cl
        or      eax,col

        mov     edi,[ebx].pt_paltab
        mov     ecx,edx
        mov     ch,b z
        cmp     [esi].vesa_pbytes,2;bpp,16
        ja      @@32
        mov     [edi+ecx*2],ax
        jmp     @@0
@@32:
        mov     [edi+ecx*4],eax
@@0:
        dec     edx
        jns     @@pl

        dec     z
        jns     @@fl

@@weg:  ret
initpaltab endp







initcdata proc near
        ;-> esi -> tvesa
        cld
        push    ebp


        mov     edx,[esi].vesa_xbytes
        mov     xbytes,edx
        mov     eax,[esi].vesa_xres
        mov     xres,eax
        mov     ebx,[esi].vesa_yres
        mov     yres,ebx
        mov     eax,[esi].vesa_linbuf
        mov     linbuf,eax

        imul    edx,ebx                 ;bufend
        add     eax,edx
        mov     bufend,eax

        mov     eax,[esi].vesa_pbytes;bpp      ;bppflag
        mov     pbytes,eax
;        sub     eax,17
;        sar     eax,3
;        mov     bppflag,eax             ;-1: 16 bit, 0: 24 bit, 1: 32 bit

                ;calc paltabs
        mov     ecx,creddata.cd_nopaltabs
        mov     ebx,creddata.cd_paltabs
@@ptl:
        push    ecx
        call    initpaltab              ;esi -> tvesa
        pop     ecx
        add     ebx,size tpaltab
        dec     ecx
        jnz     @@ptl

        mov     eax,320*256
        xor     edx,edx
        div     xres
        mov     xstep,eax
        mov     eax,creddata.cd_yres
        shl     eax,8
        xor     edx,edx
        div     yres
        mov     ystep,eax

        mov     creddata.cd_creditnum,0

@@weg:  pop     ebp
        ret
initcdata endp




end