.386                                    ;pmode/w
.model flat,prolog

locals

gfx = 1

NULL = 0

erzlen = 24000
scrollen = 200000

include tracks.inc
include int.inc

;include ..\demo\vesa.inc
extrn setpal:near
include vesa.inc

public erzdata, initedata, starterz

extrn keypressed:near


b equ byte ptr
w equ word ptr
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
;struc
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±

tpal struc
 p_red          db 256 dup(?)
 p_green        db 256 dup(?)
 p_blue         db 256 dup(?)
ends

tvec struc
 x1             dd ?
 x2             dd ?
 x3             dd ?
ends

tpoint struc
 p_x            dd ?
 p_y            dd ?
ends

tcpoint struc
 cp_x           dd ?
 cp_y           dd ?
 cp_dx          dd ?
 cp_dy          dd ?
 cp_v           dd ?
ends

tchain struc
 c_points       dd ?                    ;int
 c_pointlist    dd ?                    ;pointer
 c_ringlist     dd ?                    ;pointer
 c_normalslist  dd ?                    ;pointer
 c_lastpoint    dd ?                    ;int
ends

terzdata struc
 e_ring         dd ?
 e_sphere       dd ?                    ;sphere
 e_ringlist     dd ?                    ;drop
 e_normalslist  dd ?                    ;drop normals
 e_chains       dd ?                    ;int
 e_chain        dd ?                    ;pointer

 e_divtab       dd ?
 e_buffer       dd ?

 e_pal          dd ?
 e_picdata      dd ?
 e_xres1        dd ?
 e_yres1        dd ?
 e_pic1         dd ?
 e_xres2        dd ?
 e_yres2        dd ?
 e_pic2         dd ?
 e_paltab       dd ?

 e_camtrack     dd ?
 e_targettrack  dd ?

 e_tempdata     dd ?

 e_lines        dd ?
 e_scroller     dd ?
ends

tringparams struc
 r_xpos         dd ?
 r_zpos         dd ?
 r_xstep        dd ?
 r_zstep        dd ?
 r_sin          dd ?
 r_cos          dd ?
 r_radius       dd ?
ends

ringsize = ringpoints*(size tvec)


tviewer struc
 v_p            dd 3 dup(?)             ;local coordinate system
 v_l1           dd 3 dup(?)
 v_l2           dd 3 dup(?)
 v_l3           dd 3 dup(?)
ends


tscreenpoint struc
 sp_x           dd ?
 sp_y           dd ?
 sp_z           dd ?
 sp_data label dword
 sp_u           dd ?
 sp_v           dd ?
ends

flatpsize = size tscreenpoint


;polygon point for scanline subdivision
tscan struc ;fixed size
 s_p            dd ?
 s_z            dd ?
 s_u            dd ?
 s_v            dd ?
ends

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
.data
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±

ringpoints      = 8
hsphererings    = 4
meltrings       = 6

c_rstep         dd 0.7853981    ;2*pi/ringpoints
c_1_hsr         dd 0.25         ;1/(hsphererings)
c_1_mr          dd 0.142857     ;1/(meltrings+1)
c_asub          dd 0.082        ;4/(meltrings+1)^2
c_amul          dd 2.3;1.53125  ;0.5/(4.0/(meltrings+1))^2
c_ts            dd 1.714285     ;4/(meltrings+1)*(meltrings/2)   (tropf-start)
c_tzstep        dd -0.571428    ;4/(meltrings+1)                 (tropf-ystep)
c_tx            dd 0.428571     ;0.5/(meltrings+1)*(meltrings/2) (tropf-x)

c_0_5           dd 0.5
c_fun1fake dd 1.001

;vesa
linbuf          dd ?
xbytes          dd ?
xres            dd ?
yres            dd ?
pbytes          dd ?

xmid            dd ? ;float
ymid            dd ? ;float

;maincount       dd ?
mainstart       dd ?
;frame           dd 0                    ;global frame counter
stepframe       dd 0
step             = 128
c_tscale        dd 0.0078125 ;1/step
t               dd ?

erzdata terzdata <?>

viewer tviewer <?>
target tvec <?>
;c_1_33 dd 1.333333
c_xfov dd 2.0
c_yfov dd 2.66667
;nscale dd 32760.0


;E_x     dd 0.0
E_y     dd -14.0;100.0
E_slope dd 0.05;0.01
E_yadd  dd 0.2;0.06

rscale  dd 0.1;0.01       ;scale for rotate speed
vscale  dd -0.1;-0.01      ;scale for velocity
c_16    dd 16.0
gravity dd 4.0;1.0

c_ymin  dd -30.0       ;min y fÅr tropfen


backpoints  label dword
tscreenpoint <-4.0, 1.0,-15.0, 0.001, 0.999 >
tscreenpoint <-4.0, 1.0, 15.0, 0.001, 0.001 >
tscreenpoint <22.0, 1.0, 15.0, 0.999, 0.001 >
tscreenpoint <22.0, 1.0,-15.0, 0.999, 0.999 >

;testvars
tx dd ?
ty dd ?
c_t dd 8.0;18.0

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
.code
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±

doviewer proc near

        lea     ebx,target
        lea     edi,viewer

                ;direction vector u to target
        fld     [ebx].x1
        fsub    [edi].v_p.x1
        fld     [ebx].x2
        fsub    [edi].v_p.x2
        fld     [ebx].x3
        fsub    [edi].v_p.x3

                ;calculate local coordinate system l1, l2, l3 (l3 = view axis)
                                        ;l3 = u/|u|
                                        ;l1 = ((u) x (0,0,1))/|(u) x (0,0,1)|
                                        ;   = (u2,-u1,0)/sqrt(u1^2 + u2^2)
                                        ;l2 = l1 x l3  = | a2*b3        |
                                        ; (for l1 = a)   |-a1*b3        |
                                        ; (and l2 = b)   | a1*b2 - a2*b1|

        fld     st(2)                   ;make unit length
        fmul    st,st(0)                ;u1*u1
        fld     st(2)
        fmul    st,st(0)                ;u2*u2
        faddp   st(1),st
        fld     st(1)                   ;st(1) = (u1^2 + u2^2)
        fmul    st,st(0)                ;u3*u3
        fadd    st,st(1)
        fsqrt                           ;sqrt(u1^2 + u2^2 + u3^2) (length of u)

                ;l3                     ;l3 = u/|u|
        fld     st(4)
        fdiv    st,st(1)
        fstp    v_l3.x1[edi]
        fld     st(3)
        fdiv    st,st(1)
        fstp    v_l3.x2[edi]
        fdivr   st,st(2)
        fstp    v_l3.x3[edi]

                ;l1                     ;l1 = (u2,-u1,0)/sqrt(u1^2 + u2^2)
        fsqrt                           ;sqrt(u1^2 + u2^2)

        fld     st(2)                   ;u2/sqrt(u1^2 + u2^2)
        fdiv    st,st(1)
        fstp    [edi].v_l1.x1

        fdivp   st(3),st                ;-u1/sqrt(u1^2 + u2^2)
        fstp    st                      ;remove u3
        fstp    st                      ;remove u2
        fchs
        fstp    [edi].v_l1.x2

        fldz                            ;0
        fstp    [edi].v_l1.x3

                ;l2                     ;l2 = l1 x l3
        fld     v_l3.x1[edi]            ;b1
        fld     v_l1.x2[edi]            ;a2
        fld     v_l3.x2[edi]            ;b2
        fld     v_l1.x1[edi]            ;a1
        fld     v_l3.x3[edi]            ;b3

        fld     st(3)
        fmul    st,st(1)
;      fmul c_1_33
        fstp    v_l2.x1[edi]            ;a2*b3

        fmul    st,st(1)
        fchs
;      fmul c_1_33
        fstp    v_l2.x2[edi]            ;-a1*b3

        fmulp   st(1),st
        fxch    st(1)
        fmulp   st(2),st
        fsubrp  st(1),st
;      fmul c_1_33
        fstp    v_l2.x3[edi]            ;a1*b2-a2*b1

        ret
doviewer endp




fun1 proc near ;f(t) = sqrt(1 - t^2)
        ;-> st(0) = t
        ;<- st(0) = f(t), st(1) = t
        fld     c_fun1fake
;        fld1                            ;1
        fld     st(1)
        fmul    st,st(0)                ;t^2
        fsubp   st(1),st
;        fabs
        fsqrt
        ret
fun1 endp


fun2 proc near ;f(t) = a/2*(cos(2*pi*t) - 1) + 1
        ;-> st(0) = a/2, st(1) = t
        ;<- st(0) = f(t), st(1) = t

        fldpi
        fadd    st,st(0)
        fmul    st,st(2)
        fcos
        fld1
        fsubp   st(1),st
        fmulp   st(1),st
        fld1
        faddp   st(1),st

        ret
fun2 endp

fun3 proc near ;f(t) = a/2*sin(2*pi*t)
        ;-> st(0) = a/2, st(1) = t
        ;<- st(0) = f(t), st(1) = t

        fldpi
        fadd    st,st(0)
        fmul    st,st(2)
        fsin
        fmulp   st(1),st

        ret
fun3 endp

makering proc near;pascal
;local   rp

        mov     edi,erzdata.e_ring     ;edi -> 2D-template-ring

 ;       fldpi
 ;       mov     rp,ringpoints/2
 ;       fild    rp
 ;       fdivp   st(1),st                ;2pi/ringpoints

        fldz
        mov     ecx,ringpoints
@@l:
        fld     st
        fsincos
        fstp    [edi].p_x
        fstp    [edi].p_y

        fadd    c_rstep;st,st(1)
        add     edi,size tpoint
        dec     ecx
        jnz     @@l

        fstp    st
;        fstp    st

        ret
makering endp


placering proc near
        ;-> esi -> ringparams
        ;-> edi -> ring

        push    ebx ecx
        mov     ebx,erzdata.e_ring     ;esi -> 2D-template-ring

        mov     ecx,ringpoints

@@l:                                    ;x = x*cos(a)*radius + xpos
                                        ;y = y*radius
                                        ;z = x*sin(a)*radius + zpos

        fld     [ebx].x1
        fmul    [esi].r_sin
        fchs
        fmul    [esi].r_radius
        fadd    [esi].r_xpos
        fstp    [edi].x1

        fld     [ebx].x2
        fmul    [esi].r_radius
        fstp    [edi].x2

        fld     [ebx].x1
        fmul    [esi].r_cos
        fmul    [esi].r_radius
        fadd    [esi].r_zpos
        fstp    [edi].x3

        add     ebx,8
        add     edi,size tvec

        dec     ecx
        jnz     @@l

        pop     ecx ebx
        ret
placering endp

placering_inc proc near

        call    placering

        fld     [esi].r_xpos
        fadd    [esi].r_xstep
        fstp    [esi].r_xpos
        fld     [esi].r_zpos
        fadd    [esi].r_zstep
        fstp    [esi].r_zpos
@@weg:  ret
placering_inc endp

calcsphere proc near
local   ringparams:byte:size tringparams
local   x

        mov     edi,erzdata.e_sphere

                ;make bottom hemisphere
        fldz
        fst     ringparams.r_xpos
        fst     ringparams.r_cos
        fstp    ringparams.r_xstep

        fld1
        fst     ringparams.r_zpos
        fchs
        fst     ringparams.r_sin
        fstp    x
        fld     c_1_hsr
        fchs
        fstp    ringparams.r_zstep

        mov     ecx,hsphererings*2+1
@@l:
        fld     x
        call    fun1
        fstp    ringparams.r_radius
        fadd    c_1_hsr
        fstp    x

        push    esi
        lea     esi,ringparams
        call    placering_inc ;esi->ringparams, edi->ring
        pop     esi

        dec     ecx
        jnz     @@l

@@weg:  ret
calcsphere endp

calcdrop proc pascal
local   ringparams:byte:size tringparams
local   x, rings, normals

        mov     edi,erzdata.e_ringlist

                ;make melting segment
        fldz
        fst     ringparams.r_xpos
        fstp    ringparams.r_xstep
        fld     c_ts
        fstp    ringparams.r_zpos
        fld     c_tzstep
        fstp    ringparams.r_zstep

        fldz
        fstp    ringparams.r_cos
        fld1
        fchs
        fstp    ringparams.r_sin

        fld     c_tx
        fstp    x
        mov     ecx,meltrings/2
@@l0:

        fld     x
        fld     c_0_5;a_2
        call    fun2
        fstp    ringparams.r_radius
        fsub    c_1_mr
        fstp    x

        push    esi
        lea     esi,ringparams
        call    placering_inc ;esi->ringparams, edi->ring
        pop     esi

        dec     ecx
        jnz     @@l0


                ;make normals of melting segment
        push    edi
        mov     edi,erzdata.e_normalslist

        fld     c_tx
        fstp    x
        mov     ecx,meltrings/2
@@l1:
        fld     x
        fld     c_0_5;a_2
        call    fun3                    ;slope of melting area
        call    fun1                    ;calc radius
        fstp    ringparams.r_radius

        fstp    ringparams.r_zpos       ;displacement

        fsub    c_1_mr                  ;increment x
        fstp    x


        push    esi
        lea     esi,ringparams
        call    placering ;esi->ringparams, edi->normals
        pop     esi

        dec     ecx
        jnz     @@l1

        mov     normals,edi
        pop     edi
        mov     rings,edi

                ;make bottom hemisphere
        fldz
        fstp    ringparams.r_zpos
        fld     c_1_hsr
        fchs
        fstp    ringparams.r_zstep

        fldz
        fstp    x

        mov     ecx,hsphererings+1
@@l2:
        fld     x
        call    fun1
        fstp    ringparams.r_radius
        fadd    c_1_hsr
        fstp    x

        push    esi
        lea     esi,ringparams
        call    placering_inc ;esi->ringparams, edi->ring
        pop     esi

        dec     ecx
        jnz     @@l2

                ;make normals of bottom hemisphere
        mov     eax,rings
        mov     edx,normals
@@l3:
        fld     [eax].x1
        fstp    [edx].x1
        fld     [eax].x2
        fstp    [edx].x2
        fld     [eax].x3
        fstp    [edx].x3

        add     eax,size tvec
        add     edx,size tvec
        cmp     eax,edi
        jb      @@l3


        ret
calcdrop endp


initchains proc pascal

local   ringparams:byte:size tringparams
local   z, x
local   rings, normals

        mov     esi,erzdata.e_chain
        mov     ecx,erzdata.e_chains
        mov     z,ecx
@@l:            ;make chain

        mov     ebx,[esi].c_pointlist   ;esi -> tchain
        mov     edi,[esi].c_ringlist

                ;make top hemisphere

        fld     [ebx+size tcpoint].cp_y
        fsub    [ebx].cp_y
        fld     [ebx+size tcpoint].cp_x
        fsub    [ebx].cp_x

        fpatan
        fsincos

        fld     [ebx].cp_x
        fsub    st,st(1)
        fstp    ringparams.r_xpos
        fld     [ebx].cp_y
        fsub    st,st(2)
        fstp    ringparams.r_zpos

        fst     ringparams.r_cos
        fmul    c_1_hsr
        fstp    ringparams.r_xstep
        fst     ringparams.r_sin
        fmul    c_1_hsr
        fstp    ringparams.r_zstep


        fld1
        fstp    x

        mov     ecx,hsphererings+1
@@l0:
        fld     x
        call    fun1
        fstp    ringparams.r_radius
        fsub    c_1_hsr
        fstp    x

        push    esi
        lea     esi,ringparams
        call    placering_inc ;esi->ringparams, edi->ring
        pop     esi

        dec     ecx
        jnz     @@l0

                ;make normals of top hemisphere
        mov     eax,[esi].c_ringlist       ;esi -> tchain
        mov     edx,[esi].c_normalslist
@@l1:
        fld     [eax].x1
        fsub    [ebx].cp_x
        fstp    [edx].x1
        fld     [eax].x2
        fstp    [edx].x2
        fld     [eax].x3
        fsub    [ebx].cp_y
        fstp    [edx].x3

        add     eax,size tvec
        add     edx,size tvec
        cmp     eax,edi
        jb      @@l1
        mov     rings,edi
        mov     normals,edx


                ;make middle points
        add     ebx,size tcpoint
        fld1
        fstp    ringparams.r_radius
        mov     ecx,[esi].c_points      ;esi -> tchain
        sub     ecx,2

@@l2:           ;make ring

        fld     [ebx].cp_x
        fstp    ringparams.r_xpos
        fld     [ebx].cp_y
        fstp    ringparams.r_zpos

        fld     [ebx+size tcpoint].cp_y
        fsub    [ebx-size tcpoint].cp_y
        fld     [ebx+size tcpoint].cp_x
        fsub    [ebx-size tcpoint].cp_x

        fpatan
        fsincos

        fstp    ringparams.r_cos
        fstp    ringparams.r_sin

        push    esi
        lea     esi,ringparams
        call    placering ;esi->ringparams, edi->ring
        pop     esi

                ;make normals of ring
        mov     eax,rings
        mov     edx,normals
@@l3:
        fld     [eax].x1
        fsub    [ebx].cp_x
        fstp    [edx].x1
        fld     [eax].x2
        fstp    [edx].x2
        fld     [eax].x3
        fsub    [ebx].cp_y
        fstp    [edx].x3

        add     eax,size tvec
        add     edx,size tvec
        cmp     eax,edi
        jb      @@l3
        mov     rings,edi
        mov     normals,edx

        add     ebx,size tcpoint
        dec     ecx
        jnz     @@l2


                ;outer loop
        add     esi,size tchain
        dec     z
        jnz     @@l

@@weg:  ret
initchains endp

updatepoints proc pascal
local   z
   fld E_y
   fadd E_yadd
   fstp E_y
        mov     esi,erzdata.e_chain
        mov     ecx,erzdata.e_chains
        mov     z,ecx
@@l:
        mov     ebx,[esi].c_pointlist

        mov     edx,[esi].c_lastpoint
        mov     eax,edx
        dec     eax
        js      @@f                     ;edx = c_lastpoint
                ;melting point
        imul    eax,size tcpoint
        add     ebx,eax

        fld     [ebx].cp_x               ;add last dx
        fadd    [ebx].cp_dx
        fst    [ebx].cp_x ;fstp
     fmul E_slope
        fld     [ebx].cp_y               ;add last dy
        fadd    [ebx].cp_dy
        fst     [ebx].cp_y

      faddp st(1),st
        fld     E_y                     ;melting started if y < E_y
        fcompp
        fstsw   ax
        sahf
        jb      @@mw                    ;edx = c_lastpoint

        fld     [ebx].cp_y               ;dy
        fsub    [ebx-size tcpoint].cp_y
        fld     [ebx].cp_x               ;dx
        fsub    [ebx-size tcpoint].cp_x

                ;energiezufuhr
;        fld     [ebx].cp_x
;        fsub    E_x
;        fabs
;        fmul    E_slope
;        fchs
        fld1
;        faddp   st(1),st

        fadd    [ebx].cp_v               ;update speed
        ftst
        fstsw   ax
        sahf
        ja      @@gt0
        fstp    st
        fldz
@@gt0:
        fst     [ebx].cp_v

                                        ;rotate
        fmul    st,st(1)                ;rotate speed = v*dx*rscale
        fmul    rscale

        fld     st
        fmulp   st(2),st
        fmulp   st(2),st

        fld     [ebx].cp_v               ;move down
        fmul    vscale
        fsubrp  st(1),st
        fstp    [ebx].cp_dy              ;dy = v*vscale - dx*(rotate speed)

        fstp    [ebx].cp_dx              ;dx = dy*(rotate speed)

        fld     [ebx].cp_x               ;disconnect point from chain?
        fadd    [ebx].cp_dx
        fsub    [ebx-size tcpoint].cp_x
        fmul    st,st(0)
        fld     [ebx].cp_y
        fadd    [ebx].cp_dy
        fsub    [ebx-size tcpoint].cp_y
        fmul    st,st(0)
        faddp   st(1),st
        fcomp   c_16
        fstsw   ax
        sahf
        jb      @@mw
        dec     [esi].c_lastpoint
        cmp     edx,2                   ;edx = c_lastpoint
        jg      @@mw
        mov     [esi].c_lastpoint,0     ;last 2 points
@@mw:   add     ebx,size tcpoint
@@f:            ;falling points

        mov     ecx,[esi].c_points
        sub     ecx,edx                 ;edx = c_lastpoint
        jz      @@1
@@f_l:
        fld     [ebx].cp_x               ;add last dx
        fadd    [ebx].cp_dx
        fstp    [ebx].cp_x
        fld     [ebx].cp_y               ;add last dy
        fadd    [ebx].cp_dy
        fstp    [ebx].cp_y


        fldz                            ;dx = 0
        fstp    [ebx].cp_dx
        fld     [ebx].cp_v               ;v = v + gravity
        fadd    gravity
        fst     [ebx].cp_v
        fmul    vscale                  ;dy = v*vscale
        fstp    [ebx].cp_dy

        add     ebx,size tcpoint
        dec     ecx
        jnz     @@f_l
@@1:
        add     esi,size tchain
        dec     z
        jnz     @@l

        ret
updatepoints endp


updatechains proc pascal
local   ringparams:byte:size tringparams
local   ringparams2:byte:size tringparams
local   z, angle, dangle, a_2, x, rings, normals

        mov     esi,erzdata.e_chain
        mov     ecx,erzdata.e_chains
        mov     z,ecx
@@l:

        mov     eax,[esi].c_lastpoint
        mov     ebx,eax

                ;make melting segment

        dec     ebx
        dec     ebx
        js      @@w                     ;edx = c_lastpoint
        jz      @@last2
        imul    ebx,size tcpoint
        add     ebx,[esi].c_pointlist

        add     eax,hsphererings-2
        imul    eax,ringsize
        mov     edi,eax
        add     edi,[esi].c_ringlist
        add     eax,[esi].c_normalslist
        mov     normals,eax

        fld     t
        fld     [ebx+size tcpoint].cp_dy ;position of last point
        fmul    st,st(1)
        fadd    [ebx+size tcpoint].cp_y  ;(x = x + t*dx)
        fld     [ebx+size tcpoint].cp_dx
        fmul    st,st(2)
        fadd    [ebx+size tcpoint].cp_x  ;(y = y + t*dy)

        fld     [ebx].cp_y               ;angle of last point
        fst     ringparams.r_zpos
        fsubr   st,st(2)
        fmul    c_1_mr
        fst     ringparams.r_zstep
        fld     [ebx].cp_x
        fst     ringparams.r_xpos
        fsubr   st,st(2)
        fmul    c_1_mr
        fst     ringparams.r_xstep

        fld     st(1)                   ;calc a/2
        fmul    st,st(0)
        fld     st(1)
        fmul    st,st(0)
        faddp   st(1),st
        fsub    c_asub
        ftst
        fstsw   ax
        sahf
        ja      @@gt0
        fstp    st
        fldz
@@gt0:
        fmul    c_amul
        fstp    a_2

        fpatan

        fld     [ebx-size tcpoint].cp_y  ;angle of second last point
        fsubr   st,st(3)
        fld     [ebx-size tcpoint].cp_x
        fsubr   st,st(3)
        fpatan
        fst     angle
        fsubp   st(1),st
    fldpi
    fcom  st(1)
    fstsw ax
    sahf
    jb @@0
    fchs
    fcom st(1)
    fstsw ax
    sahf
    jb @@1
@@0:
    fadd st,st(0)
    fsubp st(1),st
    jmp @@2
@@1:
    fstp st
@@2:
        fmul    c_1_mr
        fstp    dangle

        fstp    st                      ;remove position of last point
        fstp    st
        fstp    st                      ;remove t

        fldz
        fstp    x

        mov     ecx,meltrings+1

@@l0:

        fld     angle
        fsincos
        fst     ringparams.r_cos
        fstp    ringparams2.r_cos
        fst     ringparams.r_sin
        fstp    ringparams2.r_sin


        fld     x
        fld     a_2
        call    fun2
        fstp    ringparams.r_radius

        fld     a_2
        call    fun3                    ;slope of melting area
        call    fun1                    ;calc radius
        fstp    ringparams2.r_radius

        fld     ringparams2.r_cos       ;calc displacement
        fmul    st,st(1)
        fstp    ringparams2.r_xpos
        fmul    ringparams2.r_sin
        fstp    ringparams2.r_zpos

        fadd    c_1_mr
        fstp    x

        push    esi
        lea     esi,ringparams
        call    placering_inc ;esi->ringparams, edi->ring

        push    edi
        mov     edi,normals
        lea     esi,ringparams2
        call    placering ;esi->ringparams2, edi->normals
        mov     normals,edi
        pop     edi

        pop     esi

        fld     angle
        fadd    dangle
        fstp    angle

        dec     ecx
        jnz     @@l0

        mov     rings,edi

                ;make bottom hemisphere
        fld     ringparams.r_xpos ;keep position
        fstp    ringparams2.r_xpos
        fld     ringparams.r_zpos
        fstp    ringparams2.r_zpos

        fldz
        fstp    x

        fld     ringparams.r_cos
        fmul    c_1_hsr
        fstp    ringparams.r_xstep
        fld     ringparams.r_sin
        fmul    c_1_hsr
        fstp    ringparams.r_zstep

        mov     ecx,hsphererings+1
@@l1:
        fld     x
        call    fun1
        fstp    ringparams.r_radius
;        fld     x
        fadd    c_1_hsr
        fstp    x

        push    esi
        lea     esi,ringparams
        call    placering_inc ;esi->ringparams, edi->ring
        pop     esi

        dec     ecx
        jnz     @@l1

                ;make normals of bottom hemisphere
        mov     eax,rings
        mov     edx,normals
@@l2:
        fld     [eax].x1
        fsub    ringparams2.r_xpos      ;center point of sphere
        fstp    [edx].x1
        fld     [eax].x2
        fstp    [edx].x2
        fld     [eax].x3
        fsub    ringparams2.r_zpos
        fstp    [edx].x3

        add     eax,size tvec
        add     edx,size tvec
        cmp     eax,edi
        jb      @@l2

        jmp     @@w

@@last2:        ;the last 2 points

        mov     ebx,[esi].c_pointlist
        mov     edi,[esi].c_ringlist

                ;make top hemisphere

        fld     t
        fld     [ebx+size tcpoint].cp_dy ;position of last point
        fmul    st,st(1)
        fadd    [ebx+size tcpoint].cp_y  ;(x = x + t*dx)
        fld     [ebx+size tcpoint].cp_dx
        fmul    st,st(2)
        fadd    [ebx+size tcpoint].cp_x  ;(y = y + t*dy)

        fld     [ebx].cp_y               ;angle of last point
        fsubr   st,st(2)
        fmul    c_1_mr
        fst     ringparams2.r_zstep     ;zstep for melting segment
        fld     [ebx].cp_x
        fsubr   st,st(2)
        fmul    c_1_mr
        fst     ringparams2.r_xstep     ;xstep for melting segment

        fld     st(1)                   ;calc a/2
        fmul    st,st(0)
        fld     st(1)
        fmul    st,st(0)
        faddp   st(1),st
        fsub    c_asub
        ftst
        fstsw   ax
        sahf
        ja      @@z1
        fstp    st
        fldz
@@z1:
        fmul    c_amul
        fstp    a_2

        fpatan
        fsincos

        fld     [ebx].cp_x
        fsub    st,st(1)
        fstp    ringparams.r_xpos
        fld     [ebx].cp_y
        fsub    st,st(2)
        fstp    ringparams.r_zpos

        fst     ringparams.r_cos
        fst     ringparams2.r_cos
        fmul    c_1_hsr
        fstp    ringparams.r_xstep
        fst     ringparams.r_sin
        fst     ringparams2.r_sin
        fmul    c_1_hsr
        fstp    ringparams.r_zstep

        fstp    st
        fstp    st
        fstp    st

        fld1
        fstp    x

        mov     ecx,hsphererings
@@l3:
        fld     x
        call    fun1
        fstp    ringparams.r_radius
        fsub    c_1_hsr
        fstp    x

        push    esi
        lea     esi,ringparams
        call    placering_inc ;esi->ringparams, edi->ring
        pop     esi

        dec     ecx
        jnz     @@l3

                ;make normals of top hemisphere
        mov     eax,[esi].c_ringlist       ;esi -> tchain
        mov     edx,[esi].c_normalslist
@@l4:
        fld     [eax].x1
        fsub    [ebx].cp_x
        fstp    [edx].x1
        fld     [eax].x2
        fstp    [edx].x2
        fld     [eax].x3
        fsub    [ebx].cp_y
        fstp    [edx].x3

        add     eax,size tvec
        add     edx,size tvec
        cmp     eax,edi
        jb      @@l4
;        mov     rings,edi
        mov     normals,edx


                ;make melting segment
        fldz
        fstp    x

        mov     ecx,meltrings+1

@@l5:

        fld     x
        fld     a_2
        call    fun2
        fstp    ringparams.r_radius
        fadd    c_1_mr
        fstp    x

        push    esi
        lea     esi,ringparams
        call    placering ;esi->ringparams, edi->ring
        pop     esi

        fld     ringparams.r_xpos
        fadd    ringparams2.r_xstep
        fstp    ringparams.r_xpos
        fld     ringparams.r_zpos
        fadd    ringparams2.r_zstep
        fstp    ringparams.r_zpos

        dec     ecx
        jnz     @@l5


                ;make normals of melting segment
        push    edi
        mov     edi,normals

        fldz
        fstp    x
        mov     ecx,meltrings+1
@@l6:
        fld     x
        fld     a_2
        call    fun3                    ;slope of melting area
        call    fun1                    ;calc radius
        fstp    ringparams2.r_radius

        fld     ringparams2.r_cos       ;calc displacement
        fmul    st,st(1)
        fstp    ringparams2.r_xpos
        fmul    ringparams2.r_sin
        fstp    ringparams2.r_zpos

        fadd    c_1_mr                  ;increment x
        fstp    x


        push    esi
        lea     esi,ringparams2
        call    placering ;esi->ringparams2, edi->normals
        pop     esi

        dec     ecx
        jnz     @@l6

        mov     normals,edi
        pop     edi
        mov     rings,edi

                ;make bottom hemisphere
        fld     ringparams.r_xpos ;keep position
        fstp    ringparams2.r_xpos
        fld     ringparams.r_zpos
        fstp    ringparams2.r_zpos

        fldz
        fstp    x

        mov     ecx,hsphererings+1
@@l7:
        fld     x
        call    fun1
        fstp    ringparams.r_radius
        fadd    c_1_hsr
        fstp    x

        push    esi
        lea     esi,ringparams
        call    placering_inc ;esi->ringparams, edi->ring
        pop     esi

        dec     ecx
        jnz     @@l7

                ;make normals of bottom hemisphere
        mov     eax,rings
        mov     edx,normals
@@l8:
        fld     [eax].x1
        fsub    ringparams2.r_xpos;[ebx].cp_x
        fstp    [edx].x1
        fld     [eax].x2
        fstp    [edx].x2
        fld     [eax].x3
        fsub    ringparams2.r_zpos;[ebx].cp_y
        fstp    [edx].x3

        add     eax,size tvec
        add     edx,size tvec
        cmp     eax,edi
        jb      @@l8


@@w:
        add     esi,size tchain
        dec     z
        jnz     @@l

        ret
updatechains endp

checkdir proc near
        ;-> ebx = *sp

        fld     sp_x[ebx+flatpsize*2]   ;lie points clock-wise?
        fsub    sp_x[ebx]
        fld     sp_y[ebx+flatpsize]
        fsub    sp_y[ebx]
        fmulp   st(1),st
        fld     sp_y[ebx+flatpsize*2]
        fsub    sp_y[ebx]
        fld     sp_x[ebx+flatpsize]
        fsub    sp_x[ebx]
        fmulp   st(1),st
        fcompp
        fstsw   ax
        sahf
        ret
checkdir endp


drawback proc near


        mov     edi,erzdata.e_tempdata
        lea     esi,viewer
        lea     ebx,backpoints

        mov     ecx,4
@@l0:
        fld     [ebx].sp_x
        fsub    [esi].v_p.x1
        fld     [ebx].sp_y
        fsub    [esi].v_p.x2
        fld     [ebx].sp_z
        fsub    [esi].v_p.x3

        fld     [esi].v_l3.x1           ;z
        fmul    st,st(3)
        fld     [esi].v_l3.x2
        fmul    st,st(3)
        faddp   st(1),st
        fld     [esi].v_l3.x3
        fmul    st,st(2)
        faddp   st(1),st
        fstp    [edi].sp_z

        fld     [esi].v_l1.x1           ;x
        fmul    st,st(3)
        fld     [esi].v_l1.x2
        fmul    st,st(3)
        faddp   st(1),st
        fld     [esi].v_l1.x3
        fmul    st,st(2)
        faddp   st(1),st
       fmul    c_xfov
        fdiv    [edi].sp_z              ;x = (1+x/z)*xmid
        fld1
        faddp   st(1),st
        fmul    xmid
        fstp    [edi].sp_x

        fld     [esi].v_l2.x1           ;y
        fmulp   st(3),st
        fld     [esi].v_l2.x2
        fmulp   st(2),st
        fld     [esi].v_l2.x3
        fmulp   st(1),st
        faddp   st(1),st
        faddp   st(1),st
       fmul    c_yfov;c_1_33
        fdiv    [edi].sp_z              ;y = (1-y/z)*ymid
        fld1
        fsubrp  st(1),st
        fmul    ymid
        fstp    [edi].sp_y

        fld     [ebx].sp_u
        fmul    erzdata.e_xres1
        fstp    [edi].sp_u
        fld     [ebx].sp_v
        fmul    erzdata.e_yres1
        fstp    [edi].sp_v

        add     ebx,flatpsize
        add     edi,flatpsize
        dec     ecx
        jnz     @@l0

        lea     ebx,[edi-4*flatpsize]

        call    subxclip
        jbe     @@weg
        sub     edi,ebx
        call    subpolygon pascal, ebx, edi
@@weg:  ret
drawback endp



subxclip proc pascal ;x-clipping fÅr scanline subdiv.
        ;-> ebx = *sp
        ;-> edi = *sp_end
        ;<- ebx = *sp
        ;<- edi = *sp_end
        ;<- wenn weniger als 3 punkte, mit jbe wegspringen
local   _sp, _sp_end ;beides pointer

        fldz                            ;st = x
        mov     dl,1                    ;dl = i, 1 to 0
@@i_l:
        mov     _sp,ebx                 ;ebx -> sp[z] (quellpunkte) (z = 0)
        mov     _sp_end,edi             ;edi -> sp[d] (zielpunkte)  (d = m)

@@z_l:
        fld     sp_x[ebx]               ;inn = (sp[z].sx >= x);
        fcomp   st(1)
        fstsw   ax
        mov     dh,ah                   ;dh = inn
        xor     ah,dl                   ;inn xor i
        sahf
        jnc     @@0
                ;punkt innerhalb
        mov     esi,ebx
        mov     ecx,flatpsize/4
        rep     movsd                   ;sp[d] = sp[z] und d++
@@0:
        mov     esi,ebx
        add     esi,flatpsize           ;nz = z+1
        cmp     esi,_sp_end             ;if (nz >= m) nz = 0;
        jb      @@wrap
        mov     esi,_sp
@@wrap:

        fld     sp_x[esi]              ;if (inn ^ (sp[nz].sx >= x))
        fcomp   st(1)
        fstsw   ax
        xor     ah,dh                   ;dh = inn
        sahf
        jnc     @@1
                ;dieser oder nÑchster punkt au·erhalb
        fst     sp_x[edi]
                                        ;sp[d].sy berechnen
        fld     sp_y[esi]              ;(sp[nz].sy - sp[z].sy)
        fsub    sp_y[ebx]
        fld     sp_x[esi]              ;/(sp[nz].sx - sp[z].sx)
        fsub    sp_x[ebx]
        fdivp   st(1),st
        fld     st(1);x
        fsub    sp_x[ebx]              ;*(x - sp[z].sx)
        fmulp   st(1),st
        fadd    sp_y[ebx]              ;+sp[z].sy
        fstp    sp_y[edi]

        fld     sp_x[ebx]              ;ax = sp[z].sx *sp[z].z;
        fmul    sp_z[ebx]

        fld     sp_x[esi]              ;ux = sp[nz].sx*sp[nz].z - ax;
        fmul    sp_z[esi]
        fsub    st,st(1)

                ;r = (x * sp[z].z - ax) / (ux - (sp[nz].z - sp[z].z) * x);
        fld     sp_z[esi]               ;ux - (sp[nz].z - sp[z].z) * x
        fsub    sp_z[ebx]
        fmul    st,st(3);x
        fsubp   st(1),st
        fld     sp_z[ebx]               ;(x * sp[z].z - ax)
        fmul    st,st(3);x
        fsubrp  st(2),st
        fdivp   st(1),st                ;st = r

        fld     sp_z[esi]               ;sp[d].z
        fsub    sp_z[ebx]
        fmul    st,st(1)
        fadd    sp_z[ebx]
        fstp    sp_z[edi]

        mov     ecx,1
@@l:    fld     sp_u[esi+ecx*4]         ;sp[d].u
        fsub    sp_u[ebx+ecx*4]
        fmul    st,st(1)
        fadd    sp_u[ebx+ecx*4]
        fstp    sp_u[edi+ecx*4]
        dec     ecx
        jns     @@l
        fstp    st                      ;r entfernen

        add     edi,flatpsize           ;d++
@@1:
        cmp     esi,ebx                 ;wz > z?
        mov     ebx,esi                 ;entspricht ebx += spsize
        ja      @@z_l                   ;z-schleife

        mov     ebx,_sp_end             ;ebx -> sp
                                        ;edi -> sp_end
        mov     eax,edi
        sub     eax,ebx
        shr     eax,1
        cmp     eax,flatpsize
        jbe     @@w

        fiadd   xres                    ;x += xres
        dec     dl                      ;i-schleife
        jns     @@i_l

@@w:    fstp    st                      ;x entfernen
@@weg:  ret
subxclip endp


subpolygon proc pascal ;texture mapping mit scanline subdivision
arg     _sp, sp_end
        ;_sp = *sp, zeiger auf screenpoints

local   pend, y, x_y, lb, lc, rb, rc ;int
local   lx, ldx, rx, rdx ;float
local   xa, xe, txt_x, txt_y ;int
local   la:dword:4, lu:dword:4, ra:dword:4, ru:dword:4, a:dword:4, u:dword:4


        xor     eax,eax                 ;start- und endpunkt bestimmen
        mov     lb,eax
        mov     rb,eax
        mov     pend,eax

        mov     esi,_sp                 ;esi -> screenpoints
        fld     sp_y[esi]               ;st(0) = ymax
        fld     st                      ;st(1) = ymin

        mov      ecx,sp_end
        sub      ecx,flatpsize          ;esi+ecx -> sp[sp_end-1]
@@max_l:

        fld     sp_y[esi+ecx]
        fcom    st(1);ymax              ;grî·ten y-wert finden (endpunkt)
        fstsw   ax
        sahf
        jb      @@max
        mov     pend,ecx
        fst     st(1);ymax
        jmp     @@min
@@max:
        fcom    st(2);ymin              ;kleinsten y-wert finden (startpunkt)
        fstsw   ax
        sahf
        ja      @@min
        mov     lb,ecx
        mov     rb,ecx
        fst     st(2);ymin
@@min:  fstp    st

        sub     ecx,flatpsize
        jnz     @@max_l
        fstp    st                      ;ymax entfernen

        fistp   y                       ;y = ceil(ymin)

        mov     eax,y

        cmp     eax,yres                ;y-clipping
        jge     @@weg

        or      eax,eax
        jg      @@y0
        xor     eax,eax
        mov     y,eax
@@y0:
        imul    eax,xres
        mov     edx,erzdata.e_buffer
;        shr     edx,2
;mov edx,0A0000h
        add     eax,edx
        mov     x_y,eax

        mov     lc,1
        mov     rc,1

        fld     c_16                    ;st(0) = 16

@@y_l:  ;y-schleife

                ;links
        dec     lc
        jnz     @@l_nz
        mov     edi,_sp
        mov     esi,lb
@@lc:
        cmp     esi,pend
        je      @@fertig                ;lb == pend -> unten angekommen
        mov     ebx,esi                 ;ia = lb
        sub     esi,flatpsize           ;lb--
        jnc     @@l0                    ;wrap
        add     esi,sp_end
@@l0:
        fld     sp_y[esi+edi]          ;lc = ceil(sp[lb].sy) - y
        fistp   lc
        mov     eax,y
        sub     lc,eax
        jle     @@lc                    ;while lc <= 0
        mov     lb,esi

        fld     sp_z[esi+edi]           ;sp[lb].z einlagern

        ;la
        fld     sp_z[ebx+edi]           ;la.z = sp[ia].z;
        fst     la.s_z

        fld     sp_y[ebx+edi]          ;la.y = sp[ia].sy*sp[ia].z;
        fmul    st,st(1)
        fst     la.s_p;la.y

        fld     sp_u[ebx+edi]           ;la.u = sp[ia].u;
        fst     la.s_u

        fld     sp_v[ebx+edi]           ;la.v = sp[ia].v;
        fst     la.s_v

        ;lu
        fsubr   sp_v[esi+edi]           ;lu.v = sp[lb].v - sp[ia].v;
        fstp    lu.s_v

        fsubr   sp_u[esi+edi]           ;lu.u = sp[lb].u - sp[ia].u;
        fstp    lu.s_u

        fld     sp_y[esi+edi]          ;lu.y = sp[lb].sy*sp[lb].z - la.y;
        fmul    st,st(3);sp[lb].z
        fsubrp  st(1),st
        fstp    lu.s_p;lu.y
        fsubp   st(1),st                ;lu.z = sp[lb].z - sp[ia].z;
        fstp    lu.s_z


                ;ldx = (sp[lb].sx - sp[ia].sx)/(sp[lb].sy - sp[ia].sy);
        fld     sp_x[esi+edi]
        fsub    sp_x[ebx+edi]
        fld     sp_y[esi+edi]
        fsub    sp_y[ebx+edi]
        fdivp   st(1),st
        fst     ldx

                ;lx = ldx   *(y  - sp[ia].sy) + sp[ia].sx;
        fild    y
        fsub    sp_y[ebx+edi]
        fmulp   st(1),st
        fadd    sp_x[ebx+edi]
        fst     lx
        jmp     @@l_z
@@l_nz:
        fld     lx
        fadd    ldx                     ;lx bleibt im copro
        fst     lx
@@l_z:
                ;rechts
        dec     rc
        jnz     @@r_nz
        mov     edi,_sp
        mov     esi,rb
@@rc:

        cmp     esi,pend
        je      @@fertig                ;rb == pend -> unten angekommen
        mov     ebx,esi                 ;ia = rb
        add     esi,flatpsize           ;rb++
        cmp     esi,sp_end
        jb      @@r0                    ;wrap
        xor     esi,esi
@@r0:
        fld     sp_y[esi+edi]          ;rc = ceil(sp[rb].sy) - y
        fistp   rc
        mov     eax,y
        sub     rc,eax
        jle     @@rc                    ;while rc <= 0
        mov     rb,esi

        fld     sp_z[esi+edi]           ;sp[lb].z einlagern

        ;ra
        fld     sp_z[ebx+edi]           ;ra.z = sp[ia].z;
        fst     ra.s_z

        fld     sp_y[ebx+edi]          ;ra.y = sp[ia].sy*sp[ia].z;
        fmul    st,st(1)
        fst     ra.s_p;ra.y

        fld     sp_u[ebx+edi]           ;ra.u = sp[ia].u;
        fst     ra.s_u

        fld     sp_v[ebx+edi]           ;ra.v = sp[ia].v;
        fst     ra.s_v

        ;ru
        fsubr   sp_v[esi+edi]           ;ru.v = sp[rb].v - sp[ia].v;
        fstp    ru.s_v

        fsubr   sp_u[esi+edi]           ;ru.u = sp[rb].u - sp[ia].u;
        fstp    ru.s_u

        fld     sp_y[esi+edi]          ;ru.y = sp[rb].sy*sp[rb].z - ra.y;
        fmul    st,st(3);sp[lb].z
        fsubrp  st(1),st
        fstp    ru.s_p;ru.y

        fsubp   st(1),st                ;ru.z = sp[rb].z - sp[ia].z;
        fstp    ru.s_z


                ;rdx = (sp[rb].sx - sp[ia].sx)/(sp[rb].sy - sp[ia].sy);
        fld     sp_x[esi+edi]
        fsub    sp_x[ebx+edi]
        fld     sp_y[esi+edi]
        fsub    sp_y[ebx+edi]
        fdivp   st(1),st
        fst     rdx

                ;rx = rdx   *(y  - sp[ia].sy) + sp[ia].sx;
        fild    y
        fsub    sp_y[ebx+edi]
        fmulp   st(1),st
        fadd    sp_x[ebx+edi]
        fst     rx
        jmp     @@r_z
@@r_nz:
        fld     rx
        fadd    rdx                     ;rx bleibt im copro
        fst     rx
@@r_z:

                                        ;st(0) = rx,  st(1) = lx
                                        ;st(2) = 0.5, st(3) = 16

                ;links: r = (y *la.z - la.y) / (lu.y - lu.z*y );
        fild    y
        fmul    la.s_z
        fsub    la.s_p;-la.y
        fild    y
        fmul    lu.s_z
        fsubr   lu.s_p;lu.y-
        fdivp   st(1),st                ;st = r

        fld     lu.s_z                  ;a.z = la.z + r*lu.z;
        fmul    st,st(1);*r
        fadd    la.s_z
        fst     a.s_z
        fmul    st,st(3);*lx            ;a.x = a.z*lx;
        fstp    a.s_p;a.x

        fld     lu.s_u                  ;a.u = la.u + r*lu.u;
        fmul    st,st(1);*r
        fadd    la.s_u
        fst     a.s_u
        fistp   txt_x                   ;textur-startkoordinate

        fmul    lu.s_v;*r               ;a.v = la.v + r*lu.v;
        fadd    la.s_v
        fst     a.s_v
        fistp   txt_y                   ;textur-startkoordinate

                ;rechts: r = (y *ra.z - ra.y) / (ru.y - ru.z*y );
        fild    y
        fmul    ra.s_z
        fsub    ra.s_p;ra.y
        fild    y
        fmul    ru.s_z
        fsubr   ru.s_p;ru.y-
        fdivp   st(1),st                ;st = r

        fld     ru.s_z                  ;u.z = ra.z + r*ru.z;
        fmul    st,st(1)
        fadd    ra.s_z

        fld     st                      ;u.x = u.z*rx - a.x;
        fmul    st,st(3);*rx
        fsub    a.s_p;-a.x
        fstp    u.s_p;u.x

        fsub    a.s_z                   ;u.z -= a.z;
        fstp    u.s_z

        fld     ru.s_u                  ;u.u = ra.u + r*ru.u - a.u;
        fmul    st,st(1);*r
        fadd    ra.s_u
        fsub    a.s_u
        fstp    u.s_u

        fmul    ru.s_v;*r               ;u.v = ra.v + r*ru.v - a.v;
        fadd    ra.s_v
        fsub    a.s_v
        fstp    u.s_v

        fistp   xe                      ;xe = ceil(rx) (rx entfernen)
        frndint
        fist    xa                      ;xa = ceil(lx) (lx nicht entfernen)


        mov     edi,xa
        mov     ecx,xe
        sub     ecx,edi                 ;ecx = CCCC
        add     edi,x_y                 ;edi = PPPP

@@l:
        cmp     ecx,24
        jge     @@div

                ;weniger als 24 pixel
        or      ecx,ecx
        jle     @@w

        mov     ebx,txt_x               ;ebx = 00Xx
        movzx   edx,bh                  ;edx = 00?X
        shl     ebx,24                  ;ebx = x000
        mov     bx,word ptr txt_y       ;ebx = x0Yy

        xor     eax,eax
        push    eax                     ;push pixel counter (= 0)

        mov     esi,erzdata.e_divtab
        fld     a.s_u
        fadd    u.s_u                   ;a.u + u.u = texutur - endkoordinate
        fisub   txt_x
        fmul    dword ptr [esi+ecx*4]
        fistp   txt_x

        fld     a.s_v
        fadd    u.s_v                   ;a.v + u.v = texutur - endkoordinate
        fisub   txt_y
        fmul    dword ptr [esi+ecx*4]
        fistp   txt_y

        mov     eax,txt_x               ;eax = 00Uu
        mov     ch,ah                   ;ecx = 00UC
        shl     eax,24                  ;eax = u000
        mov     ax,word ptr txt_y       ;eax = u0Vv

        jmp     @@5
                                        ;st(0) = x
                                        ;st(1) = 16
@@div:          ;scanline subdivision
        mov     edx,txt_x               ;edx = 00Xx
        mov     ebx,txt_y               ;ebx = 00Yy

        sub     ecx,16
        push    ecx                     ;push pixel counter

        fadd    st,st(1);16             ;x um 16 weiter
        fld     st;x                    ;r = (x *a.z - a.x) / (u.x - u.z*x )
        fmul    a.s_z
        fsub    a.s_p;a.x
        fld     st(1);x
        fmul    u.s_z
        fsubr   u.s_p;u.x
        fdivp   st(1),st                ;st = r

        fld     st

        fmul    u.s_u
        fadd    a.s_u                   ;a.u + r*u.u
        fistp   txt_x

        fmul    u.s_v
        fadd    a.s_v                   ;a.v + r*u.v
        fistp   txt_y


        mov     eax,txt_x
        mov     esi,txt_y
        sub     eax,edx                 ;eax = 00Uu
        sub     esi,ebx                 ;esi = 00Vv

        shr     eax,4
        shr     esi,4

        mov     ch,ah                   ;ecx = 00U0
        shl     eax,24                  ;eax = u000
        mov     cl,16                   ;ecx = 00UC
        mov     ax,si                   ;eax = u0Vv

        shl     ebx,8                   ;ebx = 0Yy0
        shrd    ebx,edx,8               ;ebx = x0Yy
        movzx   edx,dh                  ;edx = 00?X
@@5:
        mov     esi,erzdata.e_pic1     ;esi = TTTT
        push    ebp                     ;ebp : lokale variablen
        mov     ebp,eax

        mov     dh,bh
@@inner:
        add     ebx,ebp
        mov     al,[esi+edx]
        adc     dl,ch
if gfx gt 0
;        mov     [edi*4],eax
        mov     [edi],al
endif
        mov     dh,bh
        inc     edi
        dec     cl
        jnz     @@inner

        pop     ebp
        pop     ecx
        jmp     @@l
@@w:
        fstp    st                      ;x entfernen

        mov     eax,xres
        add     x_y,eax
        inc     y
        mov     eax,yres
        cmp     eax,y                   ;while (y < yres)
        jg      @@y_l

@@fertig:
        fstp    st                      ;16 entfernen

@@weg:  ret
subpolygon endp




xformchain proc pascal
arg     rings
        ;-> ebx -> ringlist
        ;-> edx -> normalslist
        ;-> edi -> tempdata
local   z, xy, uv, temp


        mov     xy,edi
        lea     esi,viewer

        mov     ecx,rings
        imul    ecx,ringpoints
@@l0:
        fld     [ebx].x1
        fsub    [esi].v_p.x1
        fld     [ebx].x2
        fsub    [esi].v_p.x2
        fld     [ebx].x3
        fsub    [esi].v_p.x3

        fld     [esi].v_l3.x1           ;z
        fmul    st,st(3)
        fld     [esi].v_l3.x2
        fmul    st,st(3)
        faddp   st(1),st
        fld     [esi].v_l3.x3
        fmul    st,st(2)
        faddp   st(1),st
        fstp    z

        fld     [esi].v_l1.x1           ;x
        fmul    st,st(3)
        fld     [esi].v_l1.x2
        fmul    st,st(3)
        faddp   st(1),st
        fld     [esi].v_l1.x3
        fmul    st,st(2)
        faddp   st(1),st
       fmul    c_xfov
        fdiv    z                       ;x = (1+x/z)*xmid
        fld1
        faddp   st(1),st
        fmul    xmid
        fstp    [edi].p_x

        fld     [esi].v_l2.x1           ;y
        fmulp   st(3),st
        fld     [esi].v_l2.x2
        fmulp   st(2),st
        fld     [esi].v_l2.x3
        fmulp   st(1),st
        faddp   st(1),st
        faddp   st(1),st
       fmul    c_yfov;c_1_33
        fdiv    z                       ;y = (1-y/z)*ymid
        fld1
        fsubrp  st(1),st
        fmul    ymid
        fstp    [edi].p_y


        add     ebx,size tvec
        add     edi,size tpoint
        dec     ecx
        jnz     @@l0

        mov     uv,edi

        mov     ecx,rings
        imul    ecx,ringpoints
@@l1:
        fld     [edx].x1
        fld     [edx].x2
        fld     [edx].x3

        fld     [esi].v_l1.x1           ;x
        fmul    st,st(3)
        fld     [esi].v_l1.x2
        fmul    st,st(3)
        faddp   st(1),st
        fld     [esi].v_l1.x3
        fmul    st,st(2)
        faddp   st(1),st
    fld1
    faddp st(1),st
    fmul erzdata.e_xres2;nscale
        fstp    [edi].p_x

        fld     [esi].v_l2.x1           ;y
        fmulp   st(3),st
        fld     [esi].v_l2.x2
        fmulp   st(2),st
        fld     [esi].v_l2.x3
        fmulp   st(1),st
        faddp   st(1),st
        faddp   st(1),st
    fld1
    faddp st(1),st
    fmul erzdata.e_yres2;nscale
        fstp    [edi].p_y


        add     edx,size tvec
        add     edi,size tpoint
        dec     ecx
        jnz     @@l1


        mov     temp,edi

        mov     esi,xy
        mov     ebx,uv

        mov     ecx,rings
        dec     ecx
        mov     z,ecx
@@l2:
        mov     ecx,ringpoints-1
@@l3:
        mov     edi,temp
        mov     eax,[esi].p_x
        mov     edx,[esi].p_y
        mov     [edi].sp_x,eax
        mov     [edi].sp_y,edx
        mov     eax,[ebx].p_x
        mov     edx,[ebx].p_y
        mov     [edi].sp_u,eax
        mov     [edi].sp_v,edx
        add     edi,size tscreenpoint

        mov     eax,[esi+size tpoint].p_x
        mov     edx,[esi+size tpoint].p_y
        mov     [edi].sp_x,eax
        mov     [edi].sp_y,edx
        mov     eax,[ebx+size tpoint].p_x
        mov     edx,[ebx+size tpoint].p_y
        mov     [edi].sp_u,eax
        mov     [edi].sp_v,edx
        add     edi,size tscreenpoint

        mov     eax,[esi+(ringpoints+1)*size tpoint].p_x
        mov     edx,[esi+(ringpoints+1)*size tpoint].p_y
        mov     [edi].sp_x,eax
        mov     [edi].sp_y,edx
        mov     eax,[ebx+(ringpoints+1)*size tpoint].p_x
        mov     edx,[ebx+(ringpoints+1)*size tpoint].p_y
        mov     [edi].sp_u,eax
        mov     [edi].sp_v,edx
        add     edi,size tscreenpoint

        mov     eax,[esi+ringpoints*size tpoint].p_x
        mov     edx,[esi+ringpoints*size tpoint].p_y
        mov     [edi].sp_x,eax
        mov     [edi].sp_y,edx
        mov     eax,[ebx+ringpoints*size tpoint].p_x
        mov     edx,[ebx+ringpoints*size tpoint].p_y
        mov     [edi].sp_u,eax
        mov     [edi].sp_v,edx
        add     edi,size tscreenpoint


        push    ecx esi ebx
        mov     ebx,temp

        call    checkdir
        jbe     @@w0
        call    xclip
        jbe     @@w0
        sub     edi,ebx
        call    polygon pascal, ebx, edi
@@w0:

        pop     ebx esi ecx
        add     esi,size tpoint
        add     ebx,size tpoint
        dec     ecx
        jnz     @@l3

        mov     edi,temp
        mov     eax,[esi].p_x
        mov     edx,[esi].p_y
        mov     [edi].sp_x,eax
        mov     [edi].sp_y,edx
        mov     eax,[ebx].p_x
        mov     edx,[ebx].p_y
        mov     [edi].sp_u,eax
        mov     [edi].sp_v,edx
        add     edi,size tscreenpoint

        mov     eax,[esi+(1-ringpoints)*size tpoint].p_x
        mov     edx,[esi+(1-ringpoints)*size tpoint].p_y
        mov     [edi].sp_x,eax
        mov     [edi].sp_y,edx
        mov     eax,[ebx+(1-ringpoints)*size tpoint].p_x
        mov     edx,[ebx+(1-ringpoints)*size tpoint].p_y
        mov     [edi].sp_u,eax
        mov     [edi].sp_v,edx
        add     edi,size tscreenpoint

        mov     eax,[esi+size tpoint].p_x
        mov     edx,[esi+size tpoint].p_y
        mov     [edi].sp_x,eax
        mov     [edi].sp_y,edx
        mov     eax,[ebx+size tpoint].p_x
        mov     edx,[ebx+size tpoint].p_y
        mov     [edi].sp_u,eax
        mov     [edi].sp_v,edx
        add     edi,size tscreenpoint

        mov     eax,[esi+ringpoints*size tpoint].p_x
        mov     edx,[esi+ringpoints*size tpoint].p_y
        mov     [edi].sp_x,eax
        mov     [edi].sp_y,edx
        mov     eax,[ebx+ringpoints*size tpoint].p_x
        mov     edx,[ebx+ringpoints*size tpoint].p_y
        mov     [edi].sp_u,eax
        mov     [edi].sp_v,edx
        add     edi,size tscreenpoint


        push    esi ebx

        mov     ebx,temp

        call    checkdir
        jbe     @@w1
        call    xclip
        jbe     @@w1
        sub     edi,ebx
        call    polygon pascal, ebx, edi
@@w1:

        pop     ebx esi
        add     esi,size tpoint
        add     ebx,size tpoint


        dec     z
        jnz     @@l2

@@weg:  ret
xformchain endp

drawchains proc pascal
local   z

        mov     esi,erzdata.e_chain    ;esi -> tchain
        mov     ecx,erzdata.e_chains
        mov     z,ecx
@@l:

        mov     eax,[esi].c_lastpoint
        or      eax,eax
        jz      @@drops

        add     eax,hsphererings*2+meltrings;eax = rings
        mov     ebx,[esi].c_ringlist
        mov     edx,[esi].c_normalslist
        mov     edi,erzdata.e_tempdata
        push    esi
        call    xformchain pascal, eax
        pop     esi
@@drops:
        mov     eax,[esi].c_lastpoint
        mov     ecx,[esi].c_points      ;esi -> tchain
        sub     ecx,eax                 ;ecx = tropfen
        jz      @@w
        imul    ebx,eax,size tcpoint
        add     ebx,[esi].c_pointlist
        push    esi

        or      eax,eax                 ;no chain left?
        jnz     @@t_l
                                        ;last drop as sphere
        fld     [ebx].cp_y
        fcomp   c_ymin
        fstsw   ax
        sahf
        jb      @@w0

        fld     [ebx].cp_dx
        fmul    t
        fadd    [ebx].cp_x
        fld     [ebx].cp_dy
        fmul    t
        fadd    [ebx].cp_y

        push    ebx ecx

        mov     edi,erzdata.e_tempdata
        mov     ebx,edi
        mov     esi,erzdata.e_sphere

        mov     ecx,(hsphererings*2+1)*ringpoints
@@l0:
        fld     [esi].x1
        fadd    st,st(2)
        fstp    [edi].x1
        fld     [esi].x2
        fstp    [edi].x2
        fld     [esi].x3
        fadd    st,st(1)
        fstp    [edi].x3

        add     esi,size tvec
        add     edi,size tvec
        dec     ecx
        jnz     @@l0

        fstp    st
        fstp    st

                ;ebx -> ringlist, edi -> tempdata
        mov     edx,erzdata.e_sphere
        call    xformchain pascal, hsphererings*2+1

        pop     ecx ebx

        add     ebx,size tcpoint
        dec     ecx

@@t_l:

        fld     [ebx].cp_y
        fcomp   c_ymin
        fstsw   ax
        sahf
        jb      @@w0

        fld     [ebx].cp_dx
        fmul    t
        fadd    [ebx].cp_x
        fld     [ebx].cp_dy
        fmul    t
        fadd    [ebx].cp_y

        push    ebx ecx

        mov     edi,erzdata.e_tempdata
        mov     ebx,edi
        mov     esi,erzdata.e_ringlist

        mov     ecx,(meltrings/2+1+hsphererings)*ringpoints
@@l1:
        fld     [esi].x1
        fadd    st,st(2)
        fstp    [edi].x1
        fld     [esi].x2
        fstp    [edi].x2
        fld     [esi].x3
        fadd    st,st(1)
        fstp    [edi].x3

        add     esi,size tvec
        add     edi,size tvec
        dec     ecx
        jnz     @@l1

        fstp    st
        fstp    st

                ;ebx -> ringlist, edi -> tempdata
        mov     edx,erzdata.e_normalslist
        call    xformchain pascal, meltrings/2+1+hsphererings

        pop     ecx ebx

        add     ebx,size tcpoint
        dec     ecx
        jnz     @@t_l
@@w0:
        pop     esi                     ;esi -> tchain
@@w:
        add     esi,size tchain
        dec     z
        jnz     @@l

@@weg:  ret
drawchains endp

xclip proc pascal ;x-clipping
        ;-> ebx = *sp
        ;-> edi = *sp_end
        ;<- ebx = *sp
        ;<- edi = *sp_end
        ;<- wenn weniger als 3 punkte, mit jbe wegspringen
local   _sp:dword, _sp_end:dword ;beides pointer

        fldz                            ;st = x
        mov     dl,1                    ;dl = i, 1 to 0
@@i_l:
        mov     _sp,ebx                 ;ebx -> sp[z] (quellpunkte) (z = 0)
        mov     _sp_end,edi             ;edi -> sp[d] (zielpunkte)  (d = m)

@@z_l:
        fld     sp_x[ebx]               ;inn = (sp[z].sx >= x);
        fcomp   st(1)
        fstsw   ax
        mov     dh,ah                   ;dh = inn
        xor     ah,dl                   ;inn xor i
        sahf
        jnc     @@0
                ;punkt innerhalb
        mov     esi,ebx
        mov     ecx,flatpsize/4
        rep     movsd                   ;sp[d] = sp[z] und d++
@@0:
        mov     esi,ebx
        add     esi,flatpsize           ;nz = z+1
        cmp     esi,_sp_end             ;if (nz >= m) nz = 0;
        jb      @@wrap
        mov     esi,_sp
@@wrap:

        fld     sp_x[esi]               ;if (inn ^ (sp[nz].sx >= x))
        fcomp   st(1)
        fstsw   ax
        xor     ah,dh                   ;dh = inn
        sahf
        jnc     @@1
                ;dieser oder nÑchster punkt au·erhalb

        fst     sp_x[edi]               ;sp[d].sx speichern

                ;r berechnen
        fld     st;x                    ;(x - sp[z].sx)
        fsub    sp_x[ebx]
        fld     sp_x[esi]               ;/(sp[nz].sx - sp[z].sx)
        fsub    sp_x[ebx]
        fdivp   st(1),st                ;st = r

        fld     sp_y[ebx]               ;sp[d].sy berechnen
        fld     sp_y[esi]               ; sp[z].sy + r*(sp[nz].sy - sp[z].sy)
        fsub    st,st(1)
        fmul    st,st(2)
        faddp   st(1),st
        fstp    sp_y[edi]

        mov     ecx,1
@@vars: fld     [esi].sp_data[ecx*4]
        fsub    [ebx].sp_data[ecx*4]
        fmul    st,st(1)
        fadd    [ebx].sp_data[ecx*4]
        fstp    [edi].sp_data[ecx*4]
        dec     ecx
        jns     @@vars

        fstp    st                      ;r entfernen

        add     edi,flatpsize           ;d++
@@1:
        cmp     esi,ebx                 ;wz > z?
        mov     ebx,esi                 ;entspricht ebx += spsize
        ja      @@z_l                   ;z-schleife

        mov     ebx,_sp_end             ;ebx -> sp
                                        ;edi -> sp_end
        mov     eax,edi
        sub     eax,ebx
        cmp     eax,flatpsize*2
        jbe     @@w

        fiadd   xres                    ;x += xres
        dec     dl                      ;i-schleife
        jns     @@i_l


@@w:    fstp    st                      ;x entfernen

        ret
xclip endp

polygon proc pascal
arg     _sp, sp_end
        ;_sp = *sp, zeiger auf screenpoints
local   pend, y, x_y, lb, lc, rb, rc ;int
local   lx, ldx, lu, ldu, lv, ldv ;float
local   rx, rdx, ru, rdu, rv, rdv ;float
local   xa, xe, txt_x, txt_y, mapand ;int


        xor     eax,eax                 ;start- und endpunkt bestimmen
        mov     lb,eax
        mov     rb,eax
        mov     pend,eax

        mov     esi,_sp                 ;esi -> screenpoints
        fld     sp_y[esi]               ;st(0) = ymax
        fld     st                      ;st(1) = ymin

        mov     ecx,sp_end
        sub     ecx,flatpsize           ;esi+ecx -> sp[sp_end-1]
@@max_l:

        fld     sp_y[esi+ecx]
        fcom    st(1);ymax              ;grî·ten y-wert finden (endpunkt)
        fstsw   ax
        sahf
        jb      @@max
        mov     pend,ecx
        fst     st(1);ymax
        jmp     @@min
@@max:
        fcom    st(2);ymin              ;kleinsten y-wert finden (startpunkt)
        fstsw   ax
        sahf
        ja      @@min
        mov     lb,ecx
        mov     rb,ecx
        fst     st(2);ymin
@@min:  fstp    st

        sub     ecx,flatpsize
        jnz     @@max_l
        fstp    st                      ;ymax entfernen

        fistp   y                       ;y = ceil(ymin)

        mov     edx,y
        cmp     edx,yres                ;y-clipping
        jge     @@weg

        or      edx,edx
        jg      @@y0
        xor     edx,edx
        mov     y,edx
@@y0:
        imul    edx,xres
        mov     eax,erzdata.e_buffer
;        shr     eax,2
;mov eax,0A0000h
        add     edx,eax
        mov     x_y,edx

        mov     lc,1
        mov     rc,1

@@y_l:  ;y-schleife

                ;links
        dec     lc
        jnz     @@l_nz
        mov     edi,_sp
        mov     esi,lb
@@lc:
        cmp     esi,pend
        je      @@fertig                ;lb == pend -> unten angekommen
        mov     ebx,esi                 ;ia = lb
        sub     esi,flatpsize           ;lb--
        jnc     @@l0                    ;wrap
        add     esi,sp_end
@@l0:
        fld     sp_y[esi+edi]           ;lc = ceil(sp[lb].sy) - y
        fistp   lc
        mov     eax,y
        sub     lc,eax
        jle     @@lc                    ;while lc <= 0
        mov     lb,esi


        fild    y
        fsub    sp_y[ebx+edi]           ;(y  - sp[ia].y)

        fld     sp_y[edi+esi]           ;(sp[lb].y - sp[ia].y)
        fsub    sp_y[edi+ebx]

                ;ldu = (sp[lb].u - sp[ia].u)/(sp[lb].y - sp[ia].y);
        fld     sp_u[edi+esi]
        fsub    sp_u[edi+ebx]
        fdiv    st,st(1)
        fst     ldu

                ;lu = ldu  *(y  - sp[ia].y) + sp[ia].u;
        fmul    st,st(2)
        fadd    sp_u[edi+ebx]
        fist    txt_x
        fstp    lu

                ;ldv = (sp[lb].v - sp[ia].v)/(sp[lb].y - sp[ia].y);
        fld     sp_v[edi+esi]
        fsub    sp_v[edi+ebx]
        fdiv    st,st(1)
        fst     ldv

                ;lv = ldv  *(y  - sp[ia].y) + sp[ia].v;
        fmul    st,st(2)
        fadd    sp_v[edi+ebx]
        fist    txt_y
        fstp    lv

                ;ldx = (sp[lb].x - sp[ia].x)/(sp[lb].y - sp[ia].y);
        fld     sp_x[edi+esi]
        fsub    sp_x[edi+ebx]
        fdivrp  st(1),st
        fst     ldx

                ;lx = ldx  *(y  - sp[ia].y) + sp[ia].x;
        fmulp   st(1),st
        fadd    sp_x[edi+ebx]
        fst     lx

        jmp     @@l_z
@@l_nz:
        fld     lu
        fadd    ldu
        fist    txt_x
        fstp    lu

        fld     lv
        fadd    ldv
        fist    txt_y
        fstp    lv

        fld     lx
        fadd    ldx                     ;lx bleibt im copro
        fst     lx
@@l_z:
                ;rechts
        dec     rc
        jnz     @@r_nz
        mov     edi,_sp
        mov     esi,rb
@@rc:
        cmp     esi,pend
        je      @@fertig                ;rb == pend -> unten angekommen
        mov     ebx,esi                 ;ia = rb
        add     esi,flatpsize           ;rb++
        cmp     esi,sp_end
        jb      @@r0                    ;wrap
        xor     esi,esi
@@r0:
        fld     sp_y[esi+edi]           ;rc = ceil(sp[rb].sy) - y
        fistp   rc
        mov     eax,y
        sub     rc,eax
        jle     @@rc                    ;while lc <= 0
        mov     rb,esi


        fild    y
        fsub    sp_y[ebx+edi]           ;(y  - sp[ia].y)

        fld     sp_y[edi+esi]           ;(sp[rb].y - sp[ia].y)
        fsub    sp_y[edi+ebx]

                ;rdu = (sp[rb].u - sp[ia].u)/(sp[rb].y - sp[ia].y);
        fld     sp_u[edi+esi]
        fsub    sp_u[edi+ebx]
        fdiv    st,st(1)
        fst     rdu

                ;ru = rdu  *(y  - sp[ia].y) + sp[ia].u;
        fmul    st,st(2)
        fadd    sp_u[edi+ebx]
        fstp    ru

                ;rdv = (sp[rb].v - sp[ia].v)/(sp[rb].y - sp[ia].y);
        fld     sp_v[edi+esi]
        fsub    sp_v[edi+ebx]
        fdiv    st,st(1)
        fst     rdv

                ;rv = rdv  *(y  - sp[ia].y) + sp[ia].v;
        fmul    st,st(2)
        fadd    sp_v[edi+ebx]
        fstp    rv

                ;rdx = (sp[rb].x - sp[ia].x)/(sp[rb].y - sp[ia].y);
        fld     sp_x[edi+esi]
        fsub    sp_x[edi+ebx]
        fdivrp  st(1),st
        fst     rdx

                ;rx = rdx  *(y  - sp[ia].y) + sp[ia].x;
        fmulp   st(1),st
        fadd    sp_x[edi+ebx]
        fst     rx

        jmp     @@r_z
@@r_nz:
        fld     ru
        fadd    rdu
        fstp    ru

        fld     rv
        fadd    rdv
        fstp    rv

        fld     rx
        fadd    rdx                     ;rx bleibt im copro
        fst     rx
@@r_z:

        fistp   xe                      ;xe = ceil(rx) (rx entfernen)
        fistp   xa                      ;xa = ceil(lx) (lx entfernen)

;eax = pppp
;ebx = x0Yy
;ecx = CCCU
;edx = 00?X
;esi = TTTT
;edi = PPPP
;ebp = u0Vv

        mov     edi,xa
        mov     ecx,xe
        sub     ecx,edi                 ;ecx = CCCC
        jle     @@w
        add     edi,x_y                 ;edi = PPPP

        mov     ebx,txt_x               ;ebx = 00Xx
        movzx   edx,bh                  ;edx = 00?X
        shl     ebx,24                  ;ebx = x000
        mov     bx,word ptr txt_y       ;ebx = x0Yy

        mov     esi,erzdata.e_divtab
        fld     ru
        fsub    lu
        fmul    dword ptr[esi+ecx*4]
        fistp   txt_x
        fld     rv
        fsub    lv
        fmul    dword ptr[esi+ecx*4]
        fistp   txt_y

        dec     ecx
        mov     eax,txt_x               ;eax = 00Uu
        shl     ecx,8                   ;ecx = CCC0
        mov     cl,ah                   ;ecx = CCCU
        shl     eax,24                  ;eax = u000
        mov     ax,word ptr txt_y       ;eax = u0Vv
        mov     esi,erzdata.e_pic2      ;esi = TTTT

        push    ebp                     ;ebp : lokale variablen
        mov     ebp,eax

        mov     dh,bh
@@inner:
        add     ebx,ebp
        mov     al,[esi+edx]
        adc     dl,cl
if gfx gt 0
        mov     [edi],al
        ;mov     [edi*4],eax
endif
        sub     ecx,256
        mov     dh,bh
        inc     edi
        jnc     @@inner

        pop     ebp

@@w:
        mov     eax,xres
        add     x_y,eax
        inc     y
        mov     eax,yres
        cmp     eax,y                   ;while (y < yres)
        jg      @@y_l

@@fertig:
@@weg:  ret
polygon endp

clearbuffer proc near

        mov     ecx,xres
        imul    ecx,yres
        shr     ecx,2
        xor     eax,eax
        mov     edi,erzdata.e_buffer
        rep     stosd

        ret
clearbuffer endp


copybuffer8 proc near

        mov     esi,erzdata.e_buffer
        mov     edi,linbuf
        mov     ebp,yres
@@y_l:
        push    edi

        mov     ecx,xres
        shr     ecx,2
        rep     movsd

        pop     edi
        add     edi,xbytes

        dec     ebp
        jnz     @@y_l

        ret
copybuffer8 endp

copybuffer16 proc near

        mov     esi,erzdata.e_buffer
        mov     edi,linbuf
        mov     ebx,erzdata.e_paltab
        mov     ebp,yres
        xor     eax,eax
@@y_l:
        push    edi

        mov     ecx,xres
        shr     ecx,1
@@x_l:
        mov     al,[esi+1]

        mov     dx,[ebx+eax*2]

        mov     al,[esi]
        shl     edx,16
        add     esi,2
        mov     dx,[ebx+eax*2]

        mov     [edi],edx
        add     edi,4

        dec     ecx
        jnz     @@x_l

        pop     edi
        add     edi,xbytes

        dec     ebp
        jnz     @@y_l

        ret
copybuffer16 endp

copybuffer24 proc near

        mov     esi,erzdata.e_buffer
        mov     edi,linbuf
        mov     ebx,erzdata.e_paltab
        mov     ebp,yres
        xor     eax,eax
@@y_l:
        push    edi ebp

        mov     ecx,xres
        shr     ecx,2
@@x_l:
                ;1.
        mov     al,[esi]
        inc     esi
        mov     ebp,[ebx+eax*4];030201h;

                ;2.
        mov     al,[esi]
      shl     ebp,8
        inc     esi
        mov     edx,[ebx+eax*4];060504h;

                ;3.
        mov     al,[esi]
      shrd    ebp,edx,8
        inc     esi
      mov     [edi],ebp
      add     edi,4
        mov     ebp,[ebx+eax*4];090807h;
      shl     edx,8

                ;4.
        mov     al,[esi]
      shrd    edx,ebp,16
        inc     esi
      mov     [edi],edx
      add     edi,4
      shl     ebp,8
        mov     edx,[ebx+eax*4];0c0b0ah;

      shld    edx,ebp,8
      mov     [edi],edx
      add     edi,4

        dec     ecx
        jnz     @@x_l

        pop     ebp edi
        add     edi,xbytes

        dec     ebp
        jnz     @@y_l

        ret
copybuffer24 endp

copybuffer32 proc near

        mov     esi,erzdata.e_buffer
        mov     edi,linbuf
        mov     ebx,erzdata.e_paltab
        mov     ebp,yres
        xor     eax,eax
@@y_l:
        push    edi

        mov     ecx,xres
@@x_l:
        mov     al,[esi]
        inc     esi
        mov     edx,[ebx+eax*4]

        mov     [edi],edx
        add     edi,4

        dec     ecx
        jnz     @@x_l

        pop     edi
        add     edi,xbytes

        dec     ebp
        jnz     @@y_l

        ret
copybuffer32 endp

comment #
test_chain proc near
        ;ebx -> ringlist
        ;edx = rings

        imul    edx,ringpoints
@@l0:

   fld [ebx].x1
   fmul c_t
   fistp tx
   fld [ebx].x3
   fmul c_t
   fistp ty
   push ebx edx
   mov eax,tx
   add eax,20
   mov edx,ty
   neg edx
   add edx,160
   mov bl,15
if gfx gt 0
   call putpixel
endif
   pop edx ebx

        add     ebx,size tvec
        dec     edx
        jnz     @@l0

@@weg:  ret
test_chain endp

test_it proc near

        mov     esi,erzdata.e_chain
        mov     ecx,erzdata.e_chains
@@l:

        mov     edx,[esi].c_lastpoint
        or      edx,edx
        jz      @@w
        add     edx,2*hsphererings+meltrings
        mov     ebx,[esi].c_ringlist
        call    test_chain
@@w:
        add     esi,size tchain
        dec     ecx
        jnz     @@l

@@weg:  ret
test_it endp
#

init1 proc near

;divtab
        mov     edi,erzdata.e_divtab
        mov     ecx,1
        fldz
        fst     dword ptr [edi]
@@l:
        fld1
        faddp   st(1),st
        fld1
        fdiv    st,st(1)
        fstp    dword ptr [edi+ecx*4]
        inc     ecx
        cmp     ecx,xres
        jbe     @@l
        fstp    st                      ;edi -> divtab

;xmid & ymid
        fild    xres
        fmul    c_0_5
        fstp    xmid
        fild    yres
        fmul    c_0_5
        fstp    ymid

        ret
init1 endp

initpaltab proc pascal
        ;-> esi -> tvesa
local   col, z

        cmp     [esi].vesa_pbytes,1
        ja      @@dcol

        mov     esi,erzdata.e_pal
        call    setpal
        jmp     @@weg
@@dcol:

        mov     edx,255
        mov     ebx,255*3
        add     ebx,erzdata.e_pal
        mov     edi,erzdata.e_paltab
@@pl:
        movzx   eax,b [ebx]             ;red
        mov     cl,[esi].vesa_redbits
        shl     eax,cl
        shr     eax,8
        mov     cl,[esi].vesa_redpos
        shl     eax,cl
        mov     col,eax

        movzx   eax,b [ebx+1]           ;green
        mov     cl,[esi].vesa_greenbits
        shl     eax,cl
        shr     eax,8
        mov     cl,[esi].vesa_greenpos
        shl     eax,cl
        or      col,eax

        movzx   eax,b [ebx+2]           ;blue
        mov     cl,[esi].vesa_bluebits
        shl     eax,cl
        shr     eax,8
        mov     cl,[esi].vesa_bluepos
        shl     eax,cl
        or      eax,col

        cmp     [esi].vesa_pbytes,2
        ja      @@32
        mov     [edi+edx*2],ax
        jmp     @@0
@@32:
        mov     [edi+edx*4],eax
@@0:
        sub     ebx,3
        dec     edx
        jns     @@pl

@@weg:  ret
initpaltab endp



initedata proc near
        ;esi -> tvesa

        mov     eax,[esi].vesa_xbytes
        mov     xbytes,eax
        mov     eax,[esi].vesa_xres
        mov     xres,eax
        mov     eax,[esi].vesa_yres
        mov     yres,eax
        mov     eax,[esi].vesa_pbytes
        mov     pbytes,eax              ;2: 16 bit, 3: 24 bit, 4: 32 bit
        mov     eax,[esi].vesa_linbuf
        mov     linbuf,eax

        call    initpaltab

        call    init1
        call    makering
        call    calcsphere
        call    calcdrop
        call    initchains

        ret
initedata endp

upscroll proc pascal
local   oldlines
local   dolines
local   texty
local   xstep, ystep, ypos

        mov     oldlines,0

        mov     eax,320*256
        xor     edx,edx
        div     xres
        mov     xstep,eax

        mov     eax,240*256
        xor     edx,edx
        div     yres
        mov     ystep,eax
        mov     ypos,0

        mov     eax,maincount
        mov     mainstart,eax

        mov     esi,erzdata.e_buffer    ;esi -> buffer
@@scroll:
        mov     eax,maincount
        sub     eax,mainstart
        mov     frame,eax
;    add maincount,80
;        sub      eax,oldframe

                                        ;eax = frame
        imul    eax,yres
;     xor edx,edx
;     mov ebx,10000
;     div ebx
        shr     eax,14
    cmp eax,erzdata.e_lines
    jge @@weg

        mov     edx,oldlines
        mov     oldlines,eax
        sub     eax,edx                 ;eax = dolines

        cmp     eax,yres
        jb      @@c0
        mov     eax,yres
@@c0:
        mov     dolines,eax
        or      eax,eax
        jle     @@lw

        imul    eax,xres

        mov     edi,erzdata.e_buffer
        mov     esi,edi
        add     esi,eax

        mov     ecx,xres
        imul    ecx,yres
        sub     ecx,eax
        shr     ecx,2

        rep     movsd
@@y_l:
        mov     esi,ypos
        shr     esi,8
        imul    esi,320
        add     esi,erzdata.e_scroller



        xor     edx,edx                 ;eax = 000x
        xor     al,al                   ;ebx = UUUU
        mov     ebx,xstep               ;ecx = 000u
        mov     cl,bl                   ;edx = XXXX
        shr     ebx,8

        push    ebp     ;no locals!
        mov     ebp,xres                ;ebp = CCCC
@@x_l:
        mov     ah,[esi+edx]
        add     al,cl
        adc     edx,ebx

        mov     [edi],ah
        inc     edi

        dec     ebp
        jnz     @@x_l
        pop     ebp

                ;update y
        mov     eax,ystep
        add     ypos,eax

        dec     dolines
        jnz     @@y_l

        push    ebp
        cmp     pbytes,3
        ja      @@32
        je      @@24
        cmp     pbytes,2
        je      @@16
        call    copybuffer8
        jmp     @@1
@@16:   call    copybuffer16
        jmp     @@1
@@24:   call    copybuffer24
        jmp     @@1
@@32:   call    copybuffer32
@@1:
        pop     ebp
@@lw:

        mov     eax,frame
;        mov     oldframe,eax
        cmp     eax,scrollen
        jg      @@weg

        call    keypressed
        or      eax,eax
;        mov     ah,1
;        int     16h
        jz      @@scroll

@@weg:  ret
upscroll endp



starterz proc near
        push    ebp

comment #
if gfx gt 0
 call init256
 mov esi,erzdata.e_pal
 call setpal
endif
#
;     jmp @@up
        mov     eax,maincount
        mov     mainstart,eax

@@nextframe:
        call    clearbuffer

        mov     eax,maincount
        sub     eax,mainstart

        mov     frame,eax
        sub     eax,stepframe
@@l0:
        cmp     eax,step
        jl      @@0
        push    eax
        call    updatepoints
        pop     eax
        add     stepframe,step
        sub     eax,step
        jmp     @@l0
@@0:
                ;calc t
        mov     eax,frame
        sub     eax,stepframe
        mov     t,eax
        fild    t
        fmul    c_tscale
        fstp    t

        call    updatechains

                ;tracks
        lea     edi,viewer.v_p
        call    dotrack pascal, erzdata.e_camtrack, 3
        lea     edi,target
        call    dotrack pascal, erzdata.e_targettrack, 3

        call    doviewer
        call    drawback
        call    drawchains

;        mov     ebx,erzdata.e_ringlist
;        mov     edx,erzdata.e_normalslist
;        call    xformchain pascal, meltrings/2+1+hsphererings

;        call    test_it
;        mov     ebx,erzdata.e_ringlist
;        mov     edx,meltrings/2+1+hsphererings
;        call    test_chain

        cmp     pbytes,3
        ja      @@32
        je      @@24
        cmp     pbytes,2
        je      @@16
        call    copybuffer8
        jmp     @@1
@@16:   call    copybuffer16
        jmp     @@1
@@24:   call    copybuffer24
        jmp     @@1
@@32:   call    copybuffer32
@@1:

comment #
if gfx gt 0
 call readchar
 cmp eax,27
 je @@w
; call cls
endif
    add maincount,step;/8
    jmp @@nextframe
#
        cmp     frame,erzlen
        jg      @@up

        call    keypressed
        or      eax,eax
;        mov     ah,1
;        int     16h
        jz      @@nextframe
        jmp     @@w

@@up:
        call    upscroll

@@w:

comment #
if gfx gt 0
 call textmode
endif
#

@@weg:  pop     ebp
        ret
starterz endp



;testcode
comment #

init256 proc near
        mov     ax,13h
        int     10h
        ret
endp

textmode proc near
        mov     ax,3
        int     10h

;        mov     ax,1112h
;        int     10h

        ret
endp


setpal proc near                        ;set vga palette
        ;-> esi -> pal

        xor     eax,eax
        mov     dx,3c8h
        out     dx,al
        inc     dx

        mov     ecx,768
@@0:
        mov     al,[esi]
        shr     al,2;1
        cmp     al,63
        jbe     @@1
        mov     al,63
@@1:
        out     dx,al
        mov     eax,100
@@w:    dec     eax
        jnz     @@w

        inc     esi
        dec     ecx
        jnz     @@0

        ret
setpal endp

putpixel proc near
        ;eax = x
        ;edx = y
        ;bl  = f
        cmp     eax,320
        jae     @@weg
        cmp     edx,200
        jae     @@weg
        imul    edx,320
        add     eax,edx
        add     eax,0a0000h
        mov     [eax],bl

@@weg:
        ret
endp

cls proc near
        mov     edi,0a0000h
        xor     eax,eax
        mov     ecx,64000/4
        rep     stosd
@@weg:
        ret
endp


readchar proc near

        xor     eax,eax
        int     16h
        or      al,al
        jnz     @@0
        sub     al,ah
        or      eax,not 0FFh
        jmp     @@weg
@@0:    and     eax,0FFh

@@weg:
        ret
endp
#


end
