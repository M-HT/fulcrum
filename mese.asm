.486                                    ;pmode/w
.model flat,prolog

locals

testcode = 0

include tracks.inc
include int.inc
;extrn int_8t:near
extrn keypressed:near

public scene

public initmdata, startmese
public m_dotracks, c_dotracks, l_dotracks, t_dotracks, b_dotracks

extrn docredits:near

;traceobject proc near                  ;motion control
;dotracks proc near
;m_dotracks proc near
;c_dotracks proc near
;l_dotracks proc near
;t_dotracks proc near
;b_dotracks proc near
; ;dotrack proc pascal
; ;slerp proc near
; ;dorottrack proc pascal
; ;doltrack proc pascal
; ;dohidetrack proc near
;doswitchtrack proc near

;doviewer proc near
;cubetree proc pascal


;texturedraw proc pascal                ;textures

;drawcube proc pascal                   ;texture cubes
;cubezclip proc pascal

;getnextmesh proc near                  ;texture objects
;xformhierarchy proc pascal
;drawhierarchy proc pascal
;drawface proc pascal
;subdrawface proc pascal
;drawwater proc pascal
;projection proc pascal

;xclip proc pascal                      ;x-clipers
;subxclip proc pascal

;polygon proc pascal                    ;texture drawers
;;gpolygon proc pascal
;subpolygon proc pascal
;;subgpolygon proc pascal
;wpolygon proc pascal

;makelgouraud proc near                 ;light
;dolights proc pascal
;ldrawface proc pascal
;sdrawfaces proc near

;ldraw proc pascal
;gldraw proc near

;lightclip proc pascal
;lscalexy proc near

;lconvert proc near                     ;light points -> flat points
;lpolygon proc pascal
;glpolygon proc pascal
;sublpolygon proc pascal

;scubezclip proc pascal                 ;shadow
;sprojection proc pascal
; ;sxclip proc near
;spolygon proc pascal

;insert proc near                       ;insert idx into z-sorted idx-list
;zprojectline proc pascal               ;zclip and project face for rendering onto z-line
;projectline proc pascal                ;project face for rendering onto z-line
;renderline proc pascal


;cproject proc near
;rtrace proc pascal
;vollight proc pascal
;yclip proc pascal                      ;y-clipping
;vvolpolygon proc pascal
;hvolpolygon proc pascal

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
;constants
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
NULL = 0
maxint = 7FFFFFFFh

w equ word ptr
d equ dword ptr

;light table
lvalues          = 64 ;C
ld_lvalues       = 6
xtralvalues      = 256-lvalues;lvalues*2 ;C

;fog table
fvalues          = 128 ;C
ld_fvalues       = 7
xtrafvalues      = 128 ;C

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
;struc
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±

;vector
tvec struc
 x1             dd ?
 x2             dd ?
 x3             dd ?
ends

;matrix
tmatrix struc
 A_1n label dword
 A_m1 label dword
 A_11           dd ?
 A_m2 label dword
 A_12           dd ?
 A_m3 label dword
 A_13           dd ?
 A_2n label dword
 A_21           dd ?
 A_22           dd ?
 A_23           dd ?
 A_3n label dword
 A_31           dd ?
 A_32           dd ?
 A_33           dd ?
ends

;quaternion
comment #
tquaternion struc  ;fixed size of 16 byte
 q_w            dd ?
 q_x            dd ?
 q_y            dd ?
 q_z            dd ?
ends
#

;vesa-info
include vesa.inc

tpaltab struc
 p_pal   dd (lvalues+xtralvalues)*256 dup(?)
ends

tpaltab16 struc
 p16_pal tpaltab ?
 p16_r   dw (fvalues+xtrafvalues)*256 dup(?)
 p16_g   dw (fvalues+xtrafvalues)*256 dup(?)
 p16_b   dw (fvalues+xtrafvalues)*256 dup(?)
ends

tpaltab32 struc
 p32_pal tpaltab ?
 p32_r   dd (fvalues+xtrafvalues)*256 dup(?)
 p32_g   dd (fvalues+xtrafvalues)*256 dup(?)
 p32_b   dd (fvalues+xtrafvalues)*256 dup(?)
ends


;scene
tscene struc
 sc_pal         dd ?
 sc_paltab      dd ?
 sc_bitmapdata  dd ?
 sc_materials   dd ?
 sc_ambient     dd ?                    ;ambient track
 sc_root        dd ?
 sc_cubedata    dd ?
 sc_cam         dd ?
 sc_firstlight  dd ?
 sc_lastlight   dd ?

 sc_tempdata    dd ?                    ;2*(tempcubes,vertices,tempfaces)
 sc_datapos     dd ?
 sc_ltab        dd ?
 sc_ftab        dd ?
 sc_buffer      dd ?
 sc_divtab      dd ?
 sc_lmap        dd ?

 sc_spot1       dd ?
 sc_spot2       dd ?
 sc_water       dd ?

 sc_idxlist     dd ?
 sc_idxlistend  dd ?
 sc_zline       dd ?
 sc_fogmap      dd ?
 sc_fogwater    dd ?

 sc_flares      dd ?
 sc_flaretab    dd ?

 sc_intro       dd ?
ends

tmaterial struc
 mt_mapptr      dd ?
 mt_mapand      dd ?
ends

;VMT
tobjvmt struc
 o_size         dd ?
 o_readdata     dd ?
 o_firstobj     dd ?
 o_nextobj      dd ?
 o_dotracks     dd ?
ends



;object:
;-------
ofMatrix    = 1
ofMesh      = 2
ofAbsolute  = 4
ofCamera    = 65536

tobject struc
 o_vmt          dd ?

 o_flags        dd ?

 ;hierarchy-system: (pointer)
 o_owner        dd ?
 o_next         dd ?
 o_child        dd ?

 ;descent-like cube system: (pointer)
 o_nextincube   dd ?                    ;link to next object in cube
 o_cube         dd ?                    ;link to cube

 ;state: (vektor/matrix)
 o_p            dd 3 dup(?)             ;actual position (position and matrix have fixed order in memory)
 o_A            dd 9 dup(?)             ;actual rotation and scale
 o_hidden       dd ?

 o_stamp        dd ?                    ;stamp to mark object as "used"
ends


;face
;----
tfp struc                               ;face point
 fp_vertex      dd ?                    ;number reference
 fp_u           dd ?
 fp_v           dd ?
ends

tface struc
 f_p            tfp 3 dup(?)
 f_mat          dd ?                    ;pointer to material list
 f_idx          dd ?
ends

ttempface struc
 tf_faceptr     dd ?                    ;-> tface
; tf_tlist       dd ?
tf_meshptr  dd ?
 tf_next        dd ?
ends

;mesh:
;-----
mfMorph      = 8
mfSort       = 16
mfNocast     = 32
mfIllum      = 64
mfCorrected  = 128
mfWater      = 1024
mfFlare      = 2048

tmesh struc
 m_object       tobject ?

 m_faces        dd ?
 m_facelist     dd ?
 m_vertices     dd ?
 m_vertexlist   dd ?

 ;postrack (3):
 m_postrack     dd ?                    ;pointer

 ;rotationtrack (4):
 m_rottrack     dd ?                    ;pointer

 ;scaletrack (3):
; m_scaletrack   dd ?                    ;pointer

 ;morphtrack (2*vertices*3):
 m_morphtrack   dd ?                    ;pointer
 m_nmorphtrack  dd ?

 ;hidetrack
 m_hidetrack    dd ?, ?                 ;pointer, int key


 m_z            dd ?                    ;z value for pivot z sorting
 m_nexttemp     dd ?                    ;-> tmesh (tempcube mesh list)
 m_tlist        dd ?                    ;-> list of transformed vertices

 m_vtlist       dd ?
 m_vstamp       dd ?
ends


;viewer:
;-------
toviewer struc
 v_object       tobject ?

 v_target       dd ?                    ;pointer

 ;postrack (3):
 v_postrack     dd ?                    ;pointer

 ;FOVtrack (1):
 v_FOVtrack     dd ?                    ;pointer
 v_FOV          dd ?                    ;float

 ;rolltrack (1):
 v_rolltrack    dd ?                    ;pointer
 v_roll         dd ?                    ;float
ends

;camera:
;-------
tcamera struc
 c_viewer       toviewer ?

 ;switchtrack
 c_switchtrack  dd ?, ?                 ;pointer, int key

ends


;light:
;------
lfVol        = 128
lfWater      = 1024
lfFlare      = 2048

tlight struc
 l_viewer       toviewer ?

 ;hidetrack
 l_hidetrack    dd ?, ?                 ;pointer, int key

 l_lmap         dd ?                    ;0:round spot, 1:four spots, 2:water
ends


;target:
;-------
ttarget struc
 t_object       tobject ?

 ;postrack (3):
 t_postrack     dd ?                    ;pointer
; t_poskey       dd ?                    ;int
ends


;bound:
;------
ttarget struc
 b_object       tobject ?

 ;postrack (3):
 b_postrack     dd ?                    ;pointer
; b_poskey       dd ?                    ;int
ends


comment #
;track:
;------
ttrack struc
 t_data         dd ?                    ;track-data
 t_key          dd ?                    ;actual key
ends

ttrackh struc
 th_mode        dd ?                    ;1: loop-track
 th_keys        dd ?                    ;number of gaps to interpolate (keys-1)
 th_sframe      dd ?                    ;start-frame
ends

tkey struc
 k_sframe       dd ?                    ;start-frame of gap
 ;k_easefrom     dd ?
 ;k_easeto       dd ?
 k_eframe       dd ?                    ;end-frame of gap
ends


;linear track:
;-------------
tltrack struc
 lt_startstate  dd ?
 lt_startframe  dd ?
 lt_endstate    dd ?
 lt_endframe    dd ?
 lt_keys        dd ?
 lt_data        dd ?
ends

#

;cube
;----
t_sub = 4               ;cube-recursion level where to switch off texture-subdivision
l_sub = 3               ;cube-recursion level where to switch off light-subdivision

tplane struc
 pl_p           tvec ?
 pl_n           tvec ?
 pl_sidecube    dd ?
ends

tcpoint struc
 cp_p           dd ?                    ;(int) -> vertex (* size tprojected)
 cp_data label dword
 cp_u           dd ?
 cp_v           dd ?
; cp_l           dd ?                    ;light value
ends

tcface struc
 cf_points      dd ?                    ;number of points in face (*size tcpoint)
 cf_mat         dd ?                    ;pointer to material list
 cf_p           tvec ?
 cf_n           tvec ?
ends

tcube struc
 c_planes       dd ?
 c_planedata    dd ?
 c_faces        dd ?
 c_facedata     dd ?
 c_vertex       dd 8 dup(?)
 c_objlist      dd ?

 c_stamp        dd ?

 c_vstamp       dd ?
 c_vtc          dd ?
ends

tprojected struc
 pr_x           dd ?
 pr_y           dd ?
 pr_z           dd ?
 pr_sx          dd ?
 pr_sy          dd ?
 pr_inn         db ?
ends

ttempcube struc
 tc_proj        tprojected 8 dup(?)     ;transformed vertices
 tc_cubeptr     dd ?                    ;pointer to tcube
 tc_meshlist    dd ?                    ;mesh-temp-list (pointer to tmesh)
 tc_recursion   dd ?
 tc_idx         dd ?
ends





;viewer
;------
tviewer struc
 v_p            dd 3 dup(?)             ;local coordinate system
 v_l1           dd 3 dup(?)
 v_l2           dd 3 dup(?)
 v_l3           dd 3 dup(?)
 v_cube         dd ?                    ;pointer to cube where viewer is in

 v_light        dd ?                    ;= 0 for camera, = 1 for light
 v_cubelist     dd ?                    ;pointer to temp cube list
 v_zmin         dd ?
 v_tempfaces    dd 16 dup(?)            ;pointer array to ttempface
 v_zmax         dd ?
 v_1_tan_fov    dd ?
ends

tlviewer struc
 lv_viewer      tviewer ?
 lv_l1          dd 3 dup(?)
 lv_l2          dd 3 dup(?)
 lv_flags       dd ?
ends


;polygon point
tscreenpoint struc
 sp_x           dd ?
 sp_y           dd ?
 sp_z           dd ?
 sp_data label dword
 union
 struc    ;mapping & gouraud
 sp_u           dd ?
 sp_v           dd ?
 sp_l           dd ?
 ends
 struc    ;light
 sp_lx          dd ?
 sp_ly          dd ?
 sp_lz          dd ?
 sp_ll          dd ?
 ends
 ends
ends

flatpsize = 5*4
flatvars = 2-1

gouraudpsize = 6*4
gouraudvars = 3-1

lightpsize = 6*4
lightvars = 3-1

glightpsize = 7*4
glightvars = 4-1

shadowpsize = 3*4

zlinepsize = 3*4
volpsize = 4*4
volvars = 1-1

;polygon point for scanline subdivision
tscan struc ;fixed size
 s_p            dd ?
 s_z            dd ?
 s_u            dd ?
 s_v            dd ?
ends

;light polygon point for scanline subdivision
tlscan struc ;fixed size
 ls_p            dd ?
 ls_z            dd ?
 ls_lx           dd ?
 ls_ly           dd ?
 ls_lz           dd ?
ends

tidxentry struc
 i_zval          dd ?
 i_idx           dd ?
ends

twater struc
 w_x             dd 256 dup(?)
 w_y             dd 256 dup(?)
 w_xpy           dd 512 dup(?)
 w_xmy           dd 512 dup(?)
 w_scale         dd 256 dup(?)
ends

tflarepoint struc
 f_sx            dd ?
 f_sy            dd ?
; f_lx            dd ?
; f_ly            dd ?
 f_mx            dd ?
 f_my            dd ?
ends

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
.data
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
debug dd ?

;maincount dd 0
mainstart dd ?

;creditnum dd 0
;creditstart dd 0

scene tscene <?>
;frame           dd 0                    ;global frame counter
stamp           dd 0

;ambient
ambient_f       dd 3 dup(?)
ambient         dd 0
fog             dd 0

;dotracks
c_3             dd 3.0
;c_sinmin        dd 1E-6
framemul = 45

;viewer
viewer tviewer <?>                      ;camera
light tlviewer <?>                      ;light
c_pi_2          dd 1.570796
c_1_33          dd 1.333333             ;x:y ratio 4:3 for camera

;xformhierarchy
c_min           dd -1E10
c_max           dd 1E10
c_0_01          dd 0.01
;c_15_9          dd 15.9

;drawhierarchy
c_0_5           dd 0.5
c_0_03125       dd 0.03125

;drawface
drawedflag      dd ?

;vesa
xbytes          dd ?
xres            dd ?
yres            dd ?
linbuf          dd ?
copybuffer      dd ?

xmid            dd ? ;float
ymid            dd ? ;float

;light & shadow
c_lscale        dd 32740.0              ;128.0 * 256 (8 bit fraction)
sres             = 256
sfres           dd 256.0
smid            dd 128.0
c_lgouraud      dd 11264.0              ;44.0 * 256 (8 bit fraction)

c_lmap          dd 0.012                ;0.0122718463 = pi/256
c_lmap1         dd 57.6                 ;lvalues *9/10

pi_256          dd 0.01227 ;pi/256.0
pi_128          dd 0.02454 ;pi/128.0
pi_8            dd 0.39270 ;pi/8
c_wscale        dd 9.2                  ;lvalues fvalues
water_coeff label dword
dd 0.075, 0.125, 0.05, 0.1, 0.05, 0.075, 0.055, 0.1125

;z-line (volume light)
zxshift  = 5       ;32
zxres    = 1 shl zxshift
zyres    = 3
z_a     dd 0.2
z_xmul  dd 16.0;32.0    ;zxres/2
z_ymul  dd 5.0     ;(zyres-1)/(2*z_a)
z_width dd 0.04;6    ;breite der strahlen
z_csize dd 4.0     ;konstante breite des entstehungspunktes

zxw     dd ?       ;xmid*z_width
zyw     dd ?       ;ymid*z_width

fogmapsize = 16
ld_fogmapsize = 4
fogmapsteps = 32
c_umax  dd 3830.0     ;(fogmapsize-1)*256
c_fogmap1 dd 0.20944  ;pi/(fogmapsize-1)
c_fogmul dd 0.6;0.42       ;fvalues / (fogmapsteps*9.6)
c_fogadd dd 0.08

;fogwater
pi_32 dd 0.0981748
c_fwsin1 dd 0.6;0.3
c_fwsin2 dd 1.0;0.5
c_fwadd  dd 1.0
c_fwmul  dd 10.3;15.0      ;(fogmapsteps-1)/3

divtab32 label dword
     dd 0
c_32 dd 32.000000
c_16 dd 16.000000
c_10_7 dd 10.666667
c_8  dd 8.000000
     dd 6.400000
     dd 5.333333
     dd 4.571429
c_4  dd 4.000000
     dd 3.555556
     dd 3.200000
     dd 2.909091
     dd 2.666667
     dd 2.461538
     dd 2.285714
     dd 2.133333
c_2  dd 2.000000
     dd 1.882353
     dd 1.777778
     dd 1.684211
     dd 1.600000
     dd 1.523810
     dd 1.454545
     dd 1.391304
     dd 1.333333
     dd 1.280000
     dd 1.230769
     dd 1.185185
     dd 1.142857
     dd 1.103448
     dd 1.066667
     dd 1.032258
     dd 1.000000
     dd 0.969697
     dd 0.941176
c_0_91 dd 0.914286
     dd 0.888889
     dd 0.864865
     dd 0.842105
     dd 0.820513
c_0_8 dd 0.800000
     dd 0.780488
     dd 0.761905
     dd 0.744186
     dd 0.727273
     dd 0.711111
c_0_7 dd 0.695652
     dd 0.680851
     dd 0.666667


c_32768 dd 32768.0
flaretab dd 0.3, 0.8, 1.3, 1.8, 2.1, 2.5, 2.7, 3.1
         dd 3.5, 4.2, 4.7, 5.0, 5.4, 5.9, 6.1, 6.3
flare_idx dd ?
flare_dx dd ?
flare_dy dd ?
flare_ycmp dd ?
flare_res dd 32600.0
flare_2s dd -2.2
flare_2x dd 0.125                       ;x/160
flare_2y dd 0.1666                      ;y/160*1.3333
flare_3s dd -0.6
flare_3x dd 0.03125
flare_3y dd 0.0416
flare_4s dd -1.6
flare_4x dd 0.01875
flare_4y dd 0.0249999375
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
.code
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±


traceobject proc near
        ;-> esi -> tobject

        mov     edi,o_cube[esi]
        or      edi,edi
        jz      @@weg

        mov     ecx,c_planes[edi]
        jecxz   @@weg
        mov     ebx,c_planedata[edi]
@@l:

        fld     o_p[esi].x1
        fsub    pl_p[ebx].x1
        fmul    pl_n[ebx].x1

        fld     o_p[esi].x2
        fsub    pl_p[ebx].x2
        fmul    pl_n[ebx].x2
        faddp   st(1),st

        fld     o_p[esi].x3
        fsub    pl_p[ebx].x3
        fmul    pl_n[ebx].x3
        faddp   st(1),st


        ftst                            ;st < 0: entered side-cube
        fstsw   ax
        fstp    st
        sahf
        ja      @@next


                ;entered side-cube: modify links
        mov     ebx,pl_sidecube[ebx]    ;ebx -> side-cube
bla_:

        mov     edx,c_objlist[edi]
        cmp     esi,edx
        jne     @@search
                                        ;object at 1st position in list
        mov     eax,o_nextincube[esi]   ;remove from list
        mov     c_objlist[edi],eax
        jmp     @@ins

@@search:                               ;objekt at 2nd to last position
        ;or      edx,edx
        ;jz      @@ins
        cmp     esi,o_nextincube[edx]   ;search entry
        je      @@rem
        mov     edx,o_nextincube[edx]
        jmp     @@search
@@rem:
        mov     eax,o_nextincube[esi]   ;remove from list
        mov     o_nextincube[edx],eax

@@ins:                                  ;insert in list of side-cube
        mov     o_cube[esi],ebx
        mov     eax,c_objlist[ebx]
        mov     o_nextincube[esi],eax
        mov     c_objlist[ebx],esi
        jmp     @@weg

@@next:
        add     ebx,size tplane
        dec     ecx
        jnz     @@l

@@weg:  ret
traceobject endp

;------ recursive procedure to calculate the tracks
dotracks proc near
        ;-> esi = *this
        ;<- esi = NULL
@@next:
        or      esi,esi
        jz      @@weg

        mov     ebx,o_vmt[esi]          ;ebx -> vmt
        call    o_dotracks[ebx]         ;virtual method to calculate the tracks

                                        ;A = matrix, p = position
                                        ;o = owner

                                        ;A = Ao*A (matrix multiplication)
        mov     edi,o_owner[esi]
        or      edi,edi
        jz      @@n                     ;no owner

        test    o_flags[esi],ofAbsolute
        jnz     @@n

        test    o_flags[esi],ofMatrix
        jz      @@pos

        xor     ebx,ebx                 ;ebx = n
@@l2:
        fld     o_A[esi + ebx]          ;n-th column of A
        fld     o_A[esi + ebx+3*4]
        fld     o_A[esi + ebx+6*4]

        xor     edx,edx                 ;edx = m
@@l1:
        fld     o_A[edi + edx]          ;* m-th row of Ao
        fmul    st,st(3)
        fld     o_A[edi + edx+1*4]
        fmul    st,st(3)
        fld     o_A[edi + edx+2*4]
        fmul    st,st(3)
        faddp   st(1)
        faddp   st(1)
        lea     eax,[ebx+edx]
        fstp    o_A[esi + eax]          ;= new element (m,n) of A

        add     edx,3*4
        cmp     edx,3*3*4
        jb      @@l1

        fstp    st
        fstp    st
        fstp    st

        add     ebx,4
        cmp     ebx,3*4
        jb      @@l2
@@pos:                                  ;p = Ao*p + po

        fld     o_p[esi].x1             ;column vektor p
        fld     o_p[esi].x2
        fld     o_p[esi].x3

        xor     edx,edx                 ;edx = m for A
        xor     ebx,ebx                 ;ebx = m for p
@@l3:
        fld     o_A[edi + edx]          ;* m-th row of Ao
        fmul    st,st(3)
        fld     o_A[edi + edx+1*4]
        fmul    st,st(3)
        fld     o_A[edi + edx+2*4]
        fmul    st,st(3)
        faddp   st(1)
        faddp   st(1)
        fadd    o_p[edi + ebx*4]
        fstp    o_p[esi + ebx*4]        ;= new element (m,1) of p

        add     edx,3*4
        inc     ebx
        cmp     ebx,3
        jb      @@l3

        fstp    st
        fstp    st
        fstp    st

@@n:
        call    traceobject

        push    esi                     ;next hierarchy level
        mov     esi,o_child[esi]
        call    dotracks
        pop     esi

        mov     esi,o_next[esi]         ;next object
        jmp     @@next

@@weg:  ret
dotracks endp


m_dotracks proc near
        ;-> esi -> tmesh

        test    [esi].o_flags,ofAbsolute
        jnz     @@abs

                ;rotation track (4)
        lea     edi,o_A[esi]
        call    dorottrack pascal, [esi].m_rottrack
comment #
                ;scale track (3)
        lea     edi,o_p[esi]
        call    dotrack pascal, [esi].m_scaletrack, 3

        mov     ecx,2                   ;scale-value into rotation matrix
@@l:
        fld     o_p[esi+ecx*4]
        fld     o_A[esi+ecx*4]
        fmul    st,st(1)
        fstp    o_A[esi+ecx*4]
        fld     o_A[esi+ecx*4+3*4]
        fmul    st,st(1)
        fstp    o_A[esi+ecx*4+3*4]
        fld     o_A[esi+ecx*4+6*4]
        fmulp   st(1)
        fstp    o_A[esi+ecx*4+6*4]
        dec     ecx
        jns     @@l
#
                ;position track (3)
        lea     edi,[esi].o_p
        call    dotrack pascal, [esi].m_postrack, 3

        jmp     @@abs0

@@abs:          ;absolute

                ;position track (3)
        lea     edi,[esi].o_p
        call    doltrack pascal, [esi].m_postrack, 3


                ;morphtrack (2*vertices*3)
        test    [esi].o_flags,mfMorph
        jz      @@m0

        mov     edi,[esi].m_vertexlist
        mov     edx,[esi].m_vertices
        lea     edx,[edx*2+edx]         ;edx*3
        call    doltrack pascal, [esi].m_morphtrack, edx
;comment #
;normals
        mov     edi,[esi].m_vertexlist
        mov     edx,[esi].m_vertices
        lea     edx,[edx*2+edx]         ;edx*3
        lea     edi,[edi+edx*4]
        call    doltrack pascal, [esi].m_nmorphtrack, edx
;#
@@m0:

@@abs0:

                ;hidetrack
        lea     ebx,m_hidetrack[esi]
        lea     edi,o_hidden[esi]
        call    dohidetrack             ;modifies o_hidden[esi]
;        mov     eax,m_hidden[esi]       ;copy flag
;        mov     o_hidden[esi],eax

        ret
m_dotracks endp


v_dotracks proc near

                ;position track (3)
        lea     edi,[esi].o_p
        call    dotrack pascal, [esi].v_postrack, 3;eax, 3

                ;FOV track (1)
        lea     edi,[esi].v_FOV
        call    dotrack pascal, [esi].v_FOVtrack, 1;eax, 1

                ;roll track (1)
        lea     edi,[esi].v_roll
        call    dotrack pascal, [esi].v_rolltrack, 1;eax, 1

        ret
v_dotracks endp

c_dotracks proc near
        ;-> esi -> tcamera

        call    v_dotracks

        lea     ebx,c_switchtrack[esi]
        call    doswitchtrack

                ;no matrix
        ret
c_dotracks endp

l_dotracks proc near
        ;-> esi -> tlight

        call    v_dotracks

                ;hidetrack
        lea     ebx,l_hidetrack[esi]
        lea     edi,o_hidden[esi]
        call    dohidetrack             ;modifies o_hidden[esi]

@@weg:  ret
l_dotracks endp

t_dotracks proc near
        ;-> esi -> ttarget

                ;position track (3)
        lea     edi,[esi].o_p
        call    dotrack pascal, [esi].t_postrack, 3

                ;no matrix
@@weg:  ret
endp

b_dotracks proc near
        ;-> esi -> tbound

                ;position track (3)
        lea     edi,[esi].o_p
        test    [esi].o_flags,ofAbsolute
        jnz     @@a
        call    dotrack pascal, b_postrack[esi], 3
        jmp     @@a0
@@a:    call    doltrack pascal, b_postrack[esi], 3
@@a0:

                ;no matrix

@@weg:  ret
b_dotracks endp


doswitchtrack proc near;pascal
        ;ebx -> track

        mov     edi,[ebx]               ;edi -> track

        xor     ecx,ecx                 ;no switch
        mov     edx,[ebx+4]             ;edx = key

@@l:    cmp     edx,[edi]               ;end of track? (edi -> keys)
        jae     @@w

        mov     eax,[edi+4+edx*8]       ;check switch-frame
        cmp     eax,frame
        ja      @@w                     ;not yet reached
        mov     ecx,[edi+4+edx*8+4]     ;get cubenum
        inc     edx                     ;next key
        jmp     @@l

@@w:
        mov     [ebx+4],edx             ;write key back

        jecxz   @@weg
        mov     edi,o_cube[esi]
        mov     ebx,ecx
        jmp     bla_

@@weg:  ret
doswitchtrack endp




doviewer proc near
        ;-> esi -> toviewer
        ;-> edi -> tviewer
        ;<- esi -> toviewer
        ;<- edi -> tviewer

        mov     eax,o_cube[esi]
        mov     v_cube[edi],eax

        mov     ebx,v_target[esi]

                ;direction vector u to target
        fld     o_p.x1[esi]
        fst     v_p.x1[edi]
        fsubr   o_p.x1[ebx]
        fld     o_p.x2[esi]
        fst     v_p.x2[edi]
        fsubr   o_p.x2[ebx]
        fld     o_p.x3[esi]
        fst     v_p.x3[edi]
        fsubr   o_p.x3[ebx]

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
        fstp    v_l2.x1[edi]            ;a2*b3

        fmul    st,st(1)
        fchs
        fstp    v_l2.x2[edi]            ;-a1*b3

        fmulp   st(1),st
        fxch    st(1)
        fmulp   st(2),st
        fsubrp  st(1),st
        fstp    v_l2.x3[edi]            ;a1*b2-a2*b1

                ;calc l1 and l2 scale factors for camera opening angle
;        fld     v_pi_2                  ;1/tan(FOV) = tan(pi/2 - FOV)
;        fsub    v_FOV[esi]              ;FOV = halve camera view angle
;        fptan
;        fstp    st
;        fld     st
;        fmulp   st(2),st                ;st(2) = (x:y-ratio) * 1/tan(FOV)

                ;calc l1 and l2 scale factors for rotation about the view axis
        fld     v_roll[esi]
        fsincos

                ;rotate & scale l1 and l2
        mov     ecx,2
@@l:
                ;l1' = (l1*cos(roll) + l2*sin(roll))/tan(FOV)
        fld     [edi].v_l1[ecx*4]
        fmul    st,st(1)                ;cos
        fld     [edi].v_l2[ecx*4]
        fmul    st,st(3)                ;sin
        faddp   st(1),st
;        fmul    st,st(3)                ;1/tan(FOV)
                ;l2' = (l2*cos(roll) + l1*sin(roll))*(x:y ratio)/tan(FOV)
        fld     [edi].v_l2[ecx*4]
        fmul    st,st(2)                ;cos
        fld     [edi].v_l1[ecx*4]
        fmul    st,st(4)                ;sin
        fsubp   st(1),st
;        fmul    st,st(5)                ;(x:y ratio)/tan(FOV)

        fstp    [edi].v_l2[ecx*4]
        fstp    [edi].v_l1[ecx*4]
        dec     ecx
        jns     @@l

;        fstp    st
;        fstp    st
        fstp    st
        fstp    st
        ret
doviewer endp


adjustcamera proc near
        ;-> esi -> tcamera
        ;-> edi -> tviewer

                ;calc l1 and l2 scale factors for camera opening angle
        fld     c_pi_2                  ;1/tan(FOV) = tan(pi/2 - FOV)
        fsub    [esi].v_FOV             ;FOV = halve camera view angle
        fptan
        fstp    st
        fst     [edi].v_1_tan_fov
        fld     st
        fmul    c_1_33                  ;st(0) = (x:y-ratio) * 1/tan(FOV)

        mov     ecx,2
@@l:
        fld     [edi].v_l1[ecx*4]
        fmul    st,st(2)
        fstp    [edi].v_l1[ecx*4]
        fld     [edi].v_l2[ecx*4]
        fmul    st,st(1)
        fstp    [edi].v_l2[ecx*4]
        dec     ecx
        jns     @@l

        fstp    st
        fstp    st

        ret
adjustcamera endp


adjustlight proc near
        ;-> esi -> tlight
        ;-> edi -> tlviewer

        mov     eax,o_flags[esi]
        mov     lv_flags[edi],eax

                ;calc l1 and l2 scale factors for camera opening angle
        fld     [esi].v_FOV
        fptan
        fstp    st
        fst     [edi].v_1_tan_fov
        fld     st
        fmul    c_0_91                   ;vol. light smaller than spot area
        fxch    st(1)
        fld1
        fdivrp  st(1),st                ;st(0) = 1/tan(FOV)
                                        ;st(1) = tan(FOV)

        mov     ecx,2
@@l:
        fld     [edi].v_l1[ecx*4]
        fld     st
        fmul    st,st(2)
        fstp    [edi].v_l1[ecx*4]
        fmul    st,st(2)
        fstp    [edi].lv_l1[ecx*4]

        fld     [edi].v_l2[ecx*4]
        fld     st
        fmul    st,st(2)
        fstp    [edi].v_l2[ecx*4]
        fmul    st,st(2)
        fstp    [edi].lv_l2[ecx*4]
        dec     ecx
        jns     @@l

        fstp    st
        fstp    st

        ret
adjustlight endp



cubetree proc pascal
arg     numcubes
        ;-> esi -> tviewer (camera/light)
        ;-> edi -> tempdata
local   readptr, listend, planeptr, planecnt, recursion
local   xmax, ymax, xmin, ymin
local   nxmax, nymax, nxmin, nymin
local   c1:byte, c2:byte, c3:byte, c4:byte

;edi -> dest. tempcube
;ebx -> source cube
        mov     [esi].v_cubelist,edi    ;edi -> destination in tempcube list
        mov     readptr,edi             ;read position in tempcube list
;     sub readptr,size ttempcube
        mov     listend,edi
        mov     ebx,[esi].v_cube        ;ebx -> source-cube

        mov     eax,c_planedata[ebx]
        mov     planeptr,eax
        mov     eax,c_planes[ebx]
        mov     planecnt,eax

        inc     stamp
        mov     c1,50                   ;c1 = 50 to indicate first pass


        fld1
        fst     xmax
        fst     ymax
        fst     nxmin
        fst     nymin
        fchs
        fst     xmin
        fst     ymin
        fst     nxmax
        fstp    nymax

        mov     recursion,0
@@l:            ;cube loop
        mov     eax,recursion
        mov     [edi].tc_recursion,eax

        mov     [edi].tc_cubeptr,ebx    ;connect cube to temp-cube

        mov     eax,stamp               ;mark cube as "used"
        mov     [ebx].c_stamp,eax
                                        ;8 vertices to transform
        xor     ecx,ecx                 ;vertex-count (0 to 7)
        mov     [edi].tc_meshlist,ecx   ;clear mesh-temp-list

        lea     edx,[edi].tc_proj       ;edx -> tprojected
@@v_l:          ;vertex transform loop
                                        ;v : vertex to transform
                                        ;l : position of viewer
                                        ;l1: right hand vector of viewer
                                        ;l2: up vector of viewer
                                        ;l3: viewing direction
                                        ;x : transformed vector

        mov     eax,[ebx].c_vertex[ecx*4];eax -> v

        fld     [eax].x1                ;v-l (relative position to viewer)
        fsub    [esi].v_p.x1
        fld     [eax].x2
        fsub    [esi].v_p.x2
        fld     [eax].x3
        fsub    [esi].v_p.x3            ;st(2) - st(0) = (v-l)

        fld     [esi].v_l1.x1           ;(v-l)*l1 = x
        fmul    st,st(3)
        fld     [esi].v_l1.x2
        fmul    st,st(3)
        fld     [esi].v_l1.x3
        fmul    st,st(3)
        faddp   st(1),st
        faddp   st(1),st
        fstp    [edx].pr_x

        fld     [esi].v_l2.x1           ;(v-l)*l2 = y
        fmul    st,st(3)
        fld     [esi].v_l2.x2
        fmul    st,st(3)
        fld     [esi].v_l2.x3
        fmul    st,st(3)
        faddp   st(1),st
        faddp   st(1),st
        fstp    [edx].pr_y

        fld     [esi].v_l3.x1           ;(v-l)*l3 = z
        fmulp   st(3),st
        fld     [esi].v_l3.x2
        fmulp   st(2),st
        fld     [esi].v_l3.x3
        fmulp   st(1),st
        faddp   st(1),st
        faddp   st(1),st
        fstp    [edx].pr_z

        add     edx,size tprojected
        inc     ecx
        cmp     ecx,8
        jb      @@v_l
                                        ;test if the cube is visible
                ;ecx = 8
        cmp     c1,50                   ;is this the first pass (c1 == 50)?
        mov     c1,cl                   ;c1 = 8 (right border counter)
        je      @@1st                   ;jump to first pass label
        mov     c2,cl                   ;c2 = 8 (left border counter)
        mov     c3,cl                   ;c3 = 8 (upper border counter)
        mov     c4,cl                   ;c4 = 8 (lower border counter)

        xor     edx,edx
@@t_l:          ;vertex test loop
        or      ecx,ecx                 ;cube not visible if counter empty
        jz      @@n_in


        fld     [edi].tc_proj[edx].pr_x ;test right border
        fld     [edi].tc_proj[edx].pr_z
        fmul    xmax
        fcomp   st(1)                   ;x > z*xmax ?
        fstsw   ax
        and     ah,1                    ;if vertex can not be seen
        sub     c1,ah                   ; decremet right border counter

        fld     [edi].tc_proj[edx].pr_z ;test left border
        fmul    xmin
        fcompp;  st(1)                   ;x < z*xmin ?
        fstsw   ax
        and     ah,1
        dec     ah                      ;if vertex can not be seen
        add     c2,ah                   ; decremet left border counter

        ;fstp    st

        fld     [edi].tc_proj[edx].pr_y ;test upper border
        fld     [edi].tc_proj[edx].pr_z
        fmul    ymax
        fcomp   st(1)                   ;y > z*ymax ?
        fstsw   ax
        and     ah,1                    ;if vertex can not be seen
        sub     c3,ah                   ; decremet right border counter

        fld     [edi].tc_proj[edx].pr_z ;test lower border
        fmul    ymin
        fcompp;   st(1)                   ;y < z*ymin ?
        fstsw   ax
        and     ah,1
        dec     ah                      ;if vertex can not be seen
        add     c4,ah                   ; decremet left border counter

        ;fstp    st


        add     edx,size tprojected
        dec     ecx

        cmp     cl,c1                   ;continue only if all vertices lie
        je      @@t_l                   ; out of one border so far
        cmp     cl,c2
        je      @@t_l
        cmp     cl,c3
        je      @@t_l
        cmp     cl,c4
        je      @@t_l
                ;cube visible if it came so far
@@1st:

        lea     edx,[edi].tc_proj       ;edx -> tprojected
        mov     ecx,8
@@p_l:          ;projection loop

        fld     [edx].pr_y
        fld     [edx].pr_x
        fld     [edx].pr_z
                                        ;st(0) = z, st(1) = x, st(2) = y
        fld1
        fcomp   st(1)
        fstsw   ax
        sahf
        ja      @@clip
        fdiv    st(1),st                ;x/z
        fdivp   st(2),st                ;y/z

        mov     [edx].pr_inn,1
        jmp     @@clip0
@@clip:
        fstp    st                      ;remove z
;        or      clip,1                  ;set clip flag
        mov     [edx].pr_inn,0

@@clip0:

        fcom    nxmax
        fstsw   ax
        sahf
        jb      @@xmax0
        fst     nxmax
        ;jmp     @@x0
@@xmax0:
        fcom    nxmin
        fstsw   ax
        sahf
        ja      @@x0
        fst     nxmin
@@x0:
        fstp    [edx].pr_sx


        fcom    nymax
        fstsw   ax
        sahf
        jb      @@ymax0
        fst     nymax
        ;jmp     @@y0
@@ymax0:
        fcom    nymin
        fstsw   ax
        sahf
        ja      @@y0
        fst     nymin
@@y0:
        fstp    [edx].pr_sy


        add     edx,size tprojected
        dec     ecx
        jnz     @@p_l


;        cmp     v_light[esi],0          ;camera or light?
;        jne     @@m
;        mov     eax,stamp               ;camera :
;        mov     c_vstamp[ebx],eax       ; extra mark for light procedures
;        mov     c_vtc[ebx],edi          ; extra link to temp-cube
;@@m:
        add     edi,size ttempcube      ;new tempcube
        dec     numcubes
        jz      @@w
        ;cmp     edi,v_cubelistmax[esi]  ;reached end of tempcube-list?
        ;jae     @@w                     ;yes -> exit loop
@@n_in:         ;label for cube not visible


        mov     edx,planeptr
        mov     eax,planecnt
@@used:
@@new:
        or      eax,eax
        je      @@next
        mov     ebx,pl_sidecube[edx]

        add     edx,size tplane
        dec     eax

        mov     ecx,stamp
        cmp     ecx,c_stamp[ebx]
        je      @@used

        mov     planeptr,edx
        mov     planecnt,eax
        jmp     @@l

@@next: add     readptr,size ttempcube
        mov     edx,readptr

        cmp     edx,listend             ;end of hierarchy level?
        jb      @@n2
        cmp     edx,edi
        jae     @@w                     ;no new cubes read
        mov     listend,edi
        inc     recursion

        fld     nxmax                   ;minimize view area
        fcom    xmax
        fstsw   ax
        sahf
        ja      @@xmax1
        fst     xmax
@@xmax1:
        fstp    st

        fld     nxmin
        fcom    xmin
        fstsw   ax
        sahf
        jb      @@xmin1
        fst     xmin
@@xmin1:
        fstp    st

        fld     nymax
        fcom    ymax
        fstsw   ax
        sahf
        ja      @@ymax1
        fst     ymax
@@ymax1:
        fstp    st

        fld     nymin
        fcom    ymin
        fstsw   ax
        sahf
        jb      @@ymin1
        fst     ymin
@@ymin1:
        fstp    st


        fld1
        fst     nxmin
        fst     nymin
        fchs
        fst     nxmax
        fstp    nymax
@@n2:

        mov     ebx,tc_cubeptr[edx]
        mov     edx,c_planedata[ebx]
        mov     eax,c_planes[ebx]
        jmp     @@new


@@w:            ;end of loop


        mov     scene.sc_datapos,edi    ;edi -> cubelistend
        ;v_tlist[esi],edi
;-----
                ;generate the tempcube temp-meshlist
        mov     listend,edi

        mov     edi,v_cubelist[esi]
        mov     ebx,esi                 ;ebx -> tviewer
@@c_l:          ;temp cube loop
        cmp     edi,listend
        jae     @@weg

        mov     eax,[edi].tc_cubeptr
        mov     esi,[eax].c_objlist     ;esi -> tobject
@@o_l:          ;object loop
        or      esi,esi
        jz      @@mw
        push    esi

@@owner:cmp     [esi].o_owner,NULL      ;go to top hierarchy level,
        je      @@top                   ; whole hierarchy trees are
        mov     esi,[esi].o_owner       ; inserted into the temp-meshlist
        jmp     @@owner
@@top:
        cmp     [esi].o_hidden,0
        jne     @@hw                    ;top level hidden

        mov     eax,stamp               ;is object already used?
        cmp     [esi].o_stamp,eax
        je      @@hw
        mov     [esi].o_stamp,eax       ;mark it as used

        test    [esi].o_flags,ofMesh
        jz      @@hw

        fld     [esi].o_p.x1            ;calc z distance from viewer
        fmul    [ebx].v_l3.x1           ;ebx -> tviewer
        fld     [esi].o_p.x2
        fmul    [ebx].v_l3.x2
        faddp   st(1),st
        fld     [esi].o_p.x3
        fmul    [ebx].v_l3.x3
        faddp   st(1),st

        fst     [esi].m_z

        lea     ecx,[edi].tc_meshlist   ;ecx -> next pointer
@@i_l:  mov     edx,[ecx]               ;edx -> tmesh
        or      edx,edx                 ;temp list z sorting
        jz      @@ins
        fcom    [edx].m_z
        fstsw   ax
        xor     ah,byte ptr[ebx].v_light;reverse z order for light
        sahf
        jnc     @@ins
        lea     ecx,[edx].m_nexttemp
        jmp     @@i_l
@@ins:  mov     [esi].m_nexttemp,edx
        mov     [ecx],esi

        fstp    st                      ;remove z

@@hw:   pop     esi
        mov     esi,[esi].o_nextincube
        jmp     @@o_l

@@mw:   add     edi,size ttempcube
        jmp     @@c_l

@@weg:  ret
cubetree endp


texturedraw proc pascal
local   c_idx, m_idx

        mov     c_idx,255
        mov     m_idx,0
        mov     esi,scene.sc_datapos
@@c_l:          ;cube loop
        sub     esi,size ttempcube
        cmp     esi,viewer.v_cubelist
        jb      @@w                     ;no more cubes

        mov     eax,c_idx
        call    drawcube                ;esi -> ttempcube
        dec     c_idx

        push    esi
        mov     esi,tc_meshlist[esi]    ;esi -> mesh
@@h_l:
        or      esi,esi
        jz      @@h_w

        lea     edi,viewer

        push    esi
        call    xformhierarchy          ;transform one hierarchy

        test    [esi].o_flags,mfWater
        jnz     @@0
        call    drawhierarchy pascal, m_idx
        add     m_idx,eax
        jmp     @@1
@@0:
        call    drawwater
@@1:
        pop     esi
      test [esi].o_flags,mfFlare
      jz @@2
      mov eax,m_idx
      mov flare_idx,eax
@@2:
        mov     esi,[esi].m_nexttemp
        jmp     @@h_l
@@h_w:

        pop     esi                     ;esi -> ttempcube
        jmp     @@c_l                   ;next cube

@@w:  ;  mov     eax,c_idx

        ret
texturedraw endp

drawcube proc pascal
        ;-> eax = idx
        ;-> esi -> ttempcube
        ;<- esi -> ttempcube
local   z, mat, ifl0

        mov     [esi].tc_idx,eax
       mov     ah,byte ptr fog
       bswap   eax
;       shl     eax,24
       mov     ah,byte ptr ambient;20h;10h
        mov     ifl0,eax

        mov     ebx,[esi].tc_cubeptr
      mov eax,stamp
      mov [ebx].c_vstamp,eax
      mov [ebx].c_vtc,esi

        mov     eax,[ebx].c_faces
        or      eax,eax
        jz      @@weg
        mov     z,eax
        mov     ebx,[ebx].c_facedata
@@l:                                    ;ebx -> tcface
        mov     eax,[ebx].cf_mat
        mov     mat,eax
        mov     eax,[ebx].cf_points
        add     ebx,size tcface
                                        ;ebx -> source points of type tcpoint
        add     eax,ebx                 ;eax -> sp_end / next face
        push    eax
        mov     edi,scene.sc_datapos
        call    cubezclip               ;esi -> ttempcube
        jbe     @@w1

        cmp     [esi].tc_recursion, t_sub
        push    esi
        ja      @@affine
                ;with scanline subdivision
        call    subxclip pascal, flatpsize, flatvars;gouraudpsize, gouraudvars
        jbe     @@w0
        sub     edi,ebx
        ;sfDrawed flag
        call    subpolygon pascal, ebx, edi, mat, ifl0
        jmp     @@w0

@@affine:       ;without scanline subdivision
        call    xclip pascal, flatpsize, flatvars;gouraudpsize, gouraudvars
        jbe     @@w0
        sub     edi,ebx
        ;sfDrawed flag
        call    polygon pascal, ebx, edi, mat, ifl0

@@w0:   pop     esi                     ;esi -> ttempcube
@@w1:   pop     ebx                     ;ebx -> tcface
        dec     z
        jnz     @@l

@@weg:  ret
drawcube endp


cubezclip proc pascal
;arg     spsize, vars
        ;-> ebx -> sp     (source points of type tcpoint)
        ;-> eax -> sp_end
        ;-> esi -> ttempcube
        ;-> edi -> dp     (dest points of type tscreenpoint)

        ;<- ebx -> dp
        ;<- esi -> ttempcube
        ;<- edi -> dp_end
        ;<- flags: if less than 3 points, jump with jbe
local   _sp, _sp_end, _dp

        mov     _sp,ebx
        mov     _sp_end,eax
        mov     _dp,edi

        fld1
@@z_l:
                                        ;inn = true : point in front of plane
        mov     edx,[ebx].cp_p
        lea     edx,[esi].tc_proj[edx]  ;edx -> tprojected (this point)
                ;if (inn)
        test    [edx].pr_inn,1          ;inn = (z >= 1.0)
        jz      @@0

                ;point in front of projection plane (visible)
        fld     [edx].pr_sx
        fadd    st,st(1)
        fmul    xmid
        fstp    [edi].sp_x              ;x = xres/2 *(1 + x/z)

        fld     [edx].pr_sy
        fsubr   st,st(1)
        fmul    ymid
        fstp    [edi].sp_y              ;y = yres/2 *(1 - y/z)

        fld     [edx].pr_z
        fstp    [edi].sp_z

        mov     ecx,flatvars;gouraudvars                ;data (mapping etc.)
@@l0:   mov     eax,[ebx].cp_data[ecx*4]
        mov     [edi].sp_data[ecx*4],eax
        dec     ecx
        jns     @@l0

        add     edi,flatpsize;gouraudpsize        ;one destination point added
                ;end of if statement
@@0:
        mov     eax,ebx                 ;ebx -> actual source point
        add     eax,size tcpoint        ;eax -> next source point
        cmp     eax,_sp_end             ;wrap if at last point
        jb      @@wrap
        mov     eax,_sp
@@wrap:

        mov     ecx,[eax].cp_p
        lea     ecx,[esi].tc_proj[ecx]  ;ecx -> tprojected (next point)
                ;if (inn ^ (nz >= 1.0))
        push    eax
        mov     al,[edx].pr_inn
        xor     al,[ecx].pr_inn
        pop     eax
        jz      @@1

                ;this or next point behind projection plane
                ;r = (1.0-z)/(nz-z)
        fld1
        fst     [edi].sp_z              ;z = 1.0
        fsub    [edx].pr_z
        fld     [ecx].pr_z
        fsub    [edx].pr_z
        fdivp   st(1),st                ;st = r

        fld     [edx].pr_x              ;x' = x + r*(nx-x)
        fld     [ecx].pr_x
        fsub    st,st(1)
        fmul    st,st(2)
        faddp   st(1),st
        fadd    st,st(2)                ;x = xres/2 *(1 + x')
        fmul    xmid
        fstp    sp_x[edi]

        fld     [edx].pr_y              ;y' = y + r*(ny-y)
        fld     [ecx].pr_y
        fsub    st,st(1)
        fmul    st,st(2)
        faddp   st(1),st
        fsubr   st,st(2)                ;y = yres/2 *(1 - y')
        fmul    ymid
        fstp    sp_y[edi]

        mov     ecx,flatvars;gouraudvars                ;data (mapping etc.)
@@l1:   fld     [ebx].cp_data[ecx*4]
        fld     [eax].cp_data[ecx*4]
        fsub    st,st(1)
        fmul    st,st(2)
        faddp   st(1),st
        fstp    sp_data[edi+ecx*4]
        dec     ecx
        jns     @@l1

        fstp    st                      ;remove r
        add     edi,flatpsize;gouraudpsize        ;one destination point added
                ;end of if statement
@@1:
        cmp     eax,ebx                 ;ns > s?
        mov     ebx,eax                 ;is same as ebx += point size
        ja      @@z_l                   ;repeat loop

        fstp    st                      ;remove 1.0

        mov     ebx,_dp
        mov     eax,edi
        sub     eax,ebx
        cmp     eax,flatpsize*2;gouraudpsize*2
        jbe     @@weg                   ;less than 3 points

        fld     sp_x[ebx+flatpsize*2];gouraudpsize*2]
        fsub    sp_x[ebx]
        fld     sp_y[ebx+flatpsize];gouraudpsize]
        fsub    sp_y[ebx]
        fmulp   st(1),st
        fld     sp_y[ebx+flatpsize*2];gouraudpsize*2]
        fsub    sp_y[ebx]
        fld     sp_x[ebx+flatpsize];gouraudpsize]
        fsub    sp_x[ebx]
        fmulp   st(1),st
        fcomp
        fstsw   ax
        fstp    st
        sahf

@@weg:  ret
cubezclip endp

getnextmesh proc near
        ;-> esi -> tobject
        ;<- Z-flag = 0: no next mesh
@@l:
        mov     edx,[esi].o_child
        or      edx,edx
        jnz     @@t
@@o:
        mov     eax,[esi].o_owner
        or      eax,eax
        jz      @@weg
@@n:
        mov     edx,[esi].o_next
        or      edx,edx
        jnz     @@t

        mov     esi,eax
        jmp     @@o

@@t:    mov     esi,edx
        test    [esi].o_flags,ofMesh
        jz      @@l

@@weg:  ret
getnextmesh endp


xformhierarchy proc pascal
        ;-> esi -> top level mesh
        ;-> edi -> tviewer (camera or light)
        ;<- eax -> idx
        ;<- esi -> top level mesh
        ;<- edi -> tviewer
local   p:dword:3, A:dword:9
local   ar_idx


        fld     c_min
        fld     c_max                   ;st(0) = zmin, st(1) = zmax
@@m_l0:                                 ;edi -> tviewer
        test    [esi].o_flags,ofAbsolute
        jnz     @@abs
                ;calculate matrix A = Av * Ao (v:viewer, o:object)
        xor     ebx,ebx                 ;ebx = n
@@l0:
        fld     [esi].o_A[ebx*4].A_1n   ;n-th column of A
        fld     [esi].o_A[ebx*4].A_2n
        fld     [esi].o_A[ebx*4].A_3n

        xor     edx,edx                 ;edx = m
@@l1:
        fld     [edi].v_l1[edx].A_m1    ;* m-th row of Al
        fmul    st,st(3)
        fld     [edi].v_l1[edx].A_m2
        fmul    st,st(3)
        fld     [edi].v_l1[edx].A_m3
        fmul    st,st(3)
        faddp   st(1)
        faddp   st(1)
        lea     eax,[ebx*4+edx]
        fstp    A[eax]                  ;= new element (m,n) of A

        add     edx,3*4
        cmp     edx,3*3*4
        jb      @@l1

        fstp    st
        fstp    st
        fstp    st

        inc     ebx
        cmp     ebx,3
        jb      @@l0

        jmp     @@abs0

@@abs:          ;absolute: matrix A = Av (only position valid)
        mov     ecx,9-1
        lea     ebx,[edi].v_l1
@@l:    mov     eax,[ebx+ecx*4]
        mov     A[ecx*4],eax
        dec     ecx
        jns     @@l

@@abs0:
                                        ;p = Av*(po-pv)
        fld     [esi].o_p.x1            ;(po-pv)
        fsub    [edi].v_p.x1
        fld     [esi].o_p.x2
        fsub    [edi].v_p.x2
        fld     [esi].o_p.x3
        fsub    [edi].v_p.x3

        xor     ebx,ebx                 ;ebx = m
@@l2:
        lea     edx,[ebx+ebx*2]
        fld     [edi].v_l1[edx*4].A_m1  ;* m-th row of Av
        fmul    st,st(3)
        fld     [edi].v_l1[edx*4].A_m2
        fmul    st,st(3)
        fld     [edi].v_l1[edx*4].A_m3
        fmul    st,st(3)
        faddp   st(1)
        faddp   st(1)
        fstp    p[ebx*4]                ;= new element (m,1) of p

        inc     ebx
        cmp     ebx,3
        jb      @@l2

        fstp    st
        fstp    st
        fstp    st

;----

                ;transform all vertices
        mov     ebx,scene.sc_datapos   ;ebx -> list of transformed vertices
        mov     [esi].m_tlist,ebx       ;save list start for this mesh

        cmp     [edi].v_light,0         ;edi -> tviewer
        jne     @@light
        mov     [esi].m_vtlist,ebx      ;if camera: reference for light procs
        mov     eax,stamp
        mov     [esi].m_vstamp,eax
@@light:
                                        ;esi -> tmesh
        mov     ecx,[esi].m_vertices
        mov     edx,[esi].m_vertexlist

@@t_l:  fld     [edx].x1                ;vertex v (calc A*v + p)
        fld     [edx].x2
        fld     [edx].x3

        fld     A.A_11                  ;* 1-st row of A
        fmul    st,st(3)
        fld     A.A_12
        fmul    st,st(3)
        fld     A.A_13
        fmul    st,st(3)
        faddp   st(1)
        faddp   st(1)
        fadd    p.x1
        fstp    [ebx].x1                ;= x1 of the vertex

        fld     A.A_21                  ;* 2-nd row of A
        fmul    st,st(3)
        fld     A.A_22
        fmul    st,st(3)
        fld     A.A_23
        fmul    st,st(3)
        faddp   st(1)
        faddp   st(1)
        fadd    p.x2
        fstp    [ebx].x2                ;= x2 of the vertex

        fld     A.A_31                  ;* 3-rd row of A
        fmulp   st(3),st
        fld     A.A_32
        fmulp   st(2),st
        fld     A.A_33
        fmulp   st(1),st
        faddp   st(1)
        faddp   st(1)
        fadd    p.x3
        fst     [ebx].x3                ;= x3 of the vertex

        fcom    st(1)                   ;zmin
        fstsw   ax
        sahf
        jae     @@zmin0
        fst     st(1)                   ;zmin
@@zmin0:
        fcom    st(2)                   ;zmax
        fstsw   ax
        sahf
        jbe     @@zmax0
        fst     st(2)                   ;zmax
@@zmax0:fstp    st

        add     edx,size tvec
        add     ebx,size tvec

        dec     ecx
        jnz     @@t_l


        ;transform normals
        cmp     [edi].v_light,0         ;edi -> tviewer
        je      @@nw
        mov     ecx,[esi].m_vertices    ;esi -> tmesh
                                        ;edx -> normals
@@t_l1: fld     [edx].x1                ;normal n (calc A*n)
        fmul    A.A_31
        fld     [edx].x2
        fmul    A.A_32
        fld     [edx].x3
        fmul    A.A_33
        faddp   st(1)
        faddp   st(1)
        fstp    dword ptr [ebx]         ;= x3 of the vertex

        add     edx,size tvec
        add     ebx,size tvec;4

        dec     ecx
        jnz     @@t_l1

@@nw:
        mov     scene.sc_datapos,ebx
                                        ;esi -> tmesh
        call    getnextmesh             ;Z-flag = 0: no next mesh
        jnz     @@m_l0


                                        ;esi -> top level mesh
        test    [esi].o_flags,mfSort    ;edi -> tviewer
        jz      @@Nsort
;----
                ;sort faces

                ;clear tempfaces pointer array
        mov     ecx,15
@@clear:
        mov     [edi].v_tempfaces[ecx*4],NULL
        dec     ecx
        jns     @@clear

                ;make temp-faces
      fld st(1)
      fld st(1)
      fld c_3
      fmul st(1),st
      fmulp st(2),st
;        fsub    c_0_01                  ;zmin -= 0.01 (force round up)
        fld     st(1)                   ;zmax
        fsub    st,st(1)                ;zmin
        fdivr   c_16;15_9                  ;scale = 15.9/(zmax-zmin)


@@m_l1:                                 ;edi -> tviewer
        mov     edx,scene.sc_datapos   ;edx -> temp-face-list
        mov     ecx,[esi].m_faces
        jecxz   @@m_w
        mov     p,ecx
        mov     ebx,[esi].m_facelist    ;ebx -> tface
@@f_l:                                  ;edx -> ttempface
        mov     [edx].tf_faceptr,ebx    ;connect face to temp-face

        mov     eax,[esi].m_tlist
        mov     [edx].tf_meshptr,esi;tf_tlist,eax
                                        ;calc z distance of face
        mov     ecx,[ebx].f_p[0].fp_vertex
        fld     [eax+ecx].x3
        mov     ecx,[ebx].f_p[1*(size tfp)].fp_vertex
        fadd    [eax+ecx].x3
        mov     ecx,[ebx].f_p[2*(size tfp)].fp_vertex
        fadd    [eax+ecx].x3

        fsub    st,st(2)                ;(z-zmin)*scale
        fmul    st,st(1)
        fistp   ar_idx                  ;ar_idx = ceil()
        mov     ecx,ar_idx

  dec ecx
  and ecx,15
;  cmp ecx,0
;  jge @@t0
;  xor ecx,ecx
;@@t0:
;  cmp ecx,15
;  jbe @@t1
;  mov ecx,15
;@@t1:
        mov     eax,[edi].v_tempfaces[ecx*4]
        mov     [edi].v_tempfaces[ecx*4],edx
        mov     [edx].tf_next,eax

        add     edx,size ttempface
        add     ebx,size tface
        dec     p
        jnz     @@f_l
@@m_w:
        mov     scene.sc_datapos,edx

        call    getnextmesh
        jnz     @@m_l1

        fstp    st                      ;remove zmin*3
        fstp    st                      ;remove zmax*3
        fstp    st                      ;remove scale

@@Nsort:                                ;edi -> tviewer
        fstp    [edi].v_zmin            ;remove zmin
        fstp    [edi].v_zmax            ;remove zmax

        ret
xformhierarchy endp


drawhierarchy proc pascal
arg     idx
        ;-> esi -> top level mesh
        ;<- eax = idx-count
local   z, amb

        test    [esi].o_flags,mfSort
        jnz     @@sort
                ;draw unsorted
        fld     viewer.v_zmin
        fadd    viewer.v_zmax
        fmul    c_0_5
        mov     edx,idx
        call    insert                  ;insert into z-sorted index list
        fstp    st
@@m_l:
        mov     ecx,[esi].m_faces
        jecxz   @@m_w
        mov     z,ecx
        push    esi                     ;esi -> tmesh

        mov     edx,[esi].o_flags       ;edx = o_flags

   mov eax,lvalues-1
   test edx,mfIllum
   jnz @@amb0
   mov eax,ambient
@@amb0:
   mov amb,eax

        mov     ebx,[esi].m_tlist       ;ebx -> transformed vertices list
        mov     esi,[esi].m_facelist    ;esi -> tface

   test edx,mfCorrected
   jnz @@c_l

@@f_l0:         ;face loop (affine)
        push    ebx

        mov     eax,idx
        mov     edx,amb
        call    drawface                ;ebx -> transformed vertices list
                                        ;esi -> tface (saved)
        pop     ebx
        add     esi,size tface
        dec     z
        jnz     @@f_l0
        jmp     @@c0

@@c_l:          ;face loop (corrected)
        push    ebx

        mov     eax,idx
        mov     edx,amb
        call    subdrawface             ;ebx -> transformed vertices list
                                        ;esi -> tface (saved)
        pop     ebx
        add     esi,size tface
        dec     z
        jnz     @@c_l
@@c0:
        pop     esi
@@m_w:
        call    getnextmesh
        jnz     @@m_l

        mov     eax,1                   ;unsorted: 1 index used
        jmp     @@weg

@@sort:         ;draw sorted
        fld     viewer.v_zmax
        fld     viewer.v_zmin
        fsub    st,st(1)
        fmul    c_0_03125  ;1/32
        fxch    st(1)
        fadd    st,st(1)

        mov     ecx,15
@@i_l:          ;insert loop
        mov     edx,idx
        add     edx,ecx
        call    insert                  ;insert into z-sorted index list
        fadd    st,st(1)
        fadd    st,st(1)
        dec     ecx
        jns     @@i_l
        fstp    st
        fstp    st

        mov     z,15
@@a_l:          ;array loop
        mov     ecx,z
        mov     esi,viewer.v_tempfaces[ecx*4]

@@f_l1:         ;face loop
        or      esi,esi                 ;esi -> ttempface
        jz      @@f_w
        push    esi

        mov     ebx,[esi].tf_meshptr    ;ebx ->tmesh
   mov edx,lvalues-1
   test [ebx].o_flags,mfIllum
   jnz @@amb1
   mov edx,ambient
@@amb1:
      mov ebx,[ebx].m_tlist
        mov     esi,[esi].tf_faceptr

        mov     eax,idx
        add     eax,z
;        mov     edx,ambient;20h;10h
        call    drawface                ;esi -> tface

        pop     esi
        mov     esi,[esi].tf_next
        jmp     @@f_l1
@@f_w:

        dec     z
        jns     @@a_l
        mov     eax,16                  ;sorted: 16 indecies used

@@weg:  ret
drawhierarchy endp

drawface proc pascal
        ;-> eax = idx
        ;-> edx = light value
        ;-> ebx -> transformed vertices list
        ;-> esi -> tface
local   ifl0, mat

        push    esi

        mov     [esi].f_idx,eax         ;store face index in face
        mov     ah,byte ptr fog
        bswap   eax
        mov     ah,dl
        mov     ifl0,eax                 ;index, fog value, light value

        mov     eax,[esi].f_mat         ;material
        mov     mat,eax

if (offset f_p) gt 0
        lea     esi,[esi].f_p
endif
        mov     edi,scene.sc_datapos
        mov     ecx,3
@@p_l:
        mov     edx,[esi].fp_vertex

        mov     eax,[ebx+edx].x1
        mov     [edi].sp_x,eax
        mov     eax,[ebx+edx].x2
        mov     [edi].sp_y,eax
        mov     eax,[ebx+edx].x3
        mov     [edi].sp_z,eax

        mov     eax,[esi].fp_u
        mov     [edi].sp_u,eax
        mov     eax,[esi].fp_v
        mov     [edi].sp_v,eax

        add     esi,size tfp
        add     edi,flatpsize
        dec     ecx
        jnz     @@p_l

        mov     ebx,scene.sc_datapos

        mov     drawedflag,-1
        call    projection pascal, flatpsize, flatvars
        jbe     @@0                     ;less than tree points

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
        jbe     @@0                     ;face not visible

        call    xclip pascal, flatpsize, flatvars ;!
        jbe     @@0
        sub     edi,ebx                 ;edi = (number of points)*(flatpsize)
        call    polygon pascal, ebx, edi, mat, ifl0
@@0:
        pop     esi                     ;esi -> tface
        mov     eax,drawedflag
        or      [esi].f_idx,eax         ;face index = -1 if not drawn
        ret
drawface endp

subdrawface proc pascal
        ;-> eax = idx
        ;-> edx = light value
        ;-> ebx -> transformed vertices list
        ;-> esi -> tface
local   ifl0, mat

        push    esi

        mov     [esi].f_idx,eax         ;store face index in face
        mov     ah,byte ptr fog
        bswap   eax
        mov     ah,dl
        mov     ifl0,eax                 ;index, fog value, light value

        mov     eax,[esi].f_mat         ;material
        mov     mat,eax

if (offset f_p) gt 0
        lea     esi,[esi].f_p
endif
        mov     edi,scene.sc_datapos
        mov     ecx,3
@@p_l:
        mov     edx,[esi].fp_vertex

        mov     eax,[ebx+edx].x1
        mov     [edi].sp_x,eax
        mov     eax,[ebx+edx].x2
        mov     [edi].sp_y,eax
        mov     eax,[ebx+edx].x3
        mov     [edi].sp_z,eax

        mov     eax,[esi].fp_u
        mov     [edi].sp_u,eax
        mov     eax,[esi].fp_v
        mov     [edi].sp_v,eax

        add     esi,size tfp
        add     edi,flatpsize
        dec     ecx
        jnz     @@p_l

        mov     ebx,scene.sc_datapos

        mov     drawedflag,-1
        call    projection pascal, flatpsize, flatvars
        jbe     @@0                     ;less than tree points

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
        jbe     @@0                     ;face not visible

        call    subxclip pascal, flatpsize, flatvars ;!
        jbe     @@0
        sub     edi,ebx                 ;edi = (number of points)*(flatpsize)
        call    subpolygon pascal, ebx, edi, mat, ifl0
@@0:
        pop     esi                     ;esi -> tface
        mov     eax,drawedflag
        or      [esi].f_idx,eax         ;face index = -1 if not drawn
        ret
subdrawface endp


drawwater proc pascal
        ;-> esi -> top level mesh
local   z
                                        ;only unsorted
@@m_l:
        mov     ecx,[esi].m_faces
        or      ecx,ecx
        jz      @@m_w
        mov     z,ecx
        push    esi
                                        ;esi -> tmesh

        mov     ebx,[esi].m_tlist       ;ebx -> transformed vertices list
        mov     esi,[esi].m_facelist    ;esi -> tface
@@f_l0:         ;face loop
        push    ebx

                                        ;-> ebx -> transformed vertices list
        push    esi                     ;-> esi -> tface

if (offset f_p) gt 0
        lea     esi,[esi].f_p
endif
        mov     edi,scene.sc_datapos
        mov     ecx,3
@@p_l:
        mov     edx,[esi].fp_vertex

        mov     eax,[ebx+edx].x1
        mov     [edi].sp_x,eax
        mov     eax,[ebx+edx].x2
        mov     [edi].sp_y,eax
        mov     eax,[ebx+edx].x3
        mov     [edi].sp_z,eax

        mov     eax,[esi].fp_u
        mov     [edi].sp_u,eax
        mov     eax,[esi].fp_v
        mov     [edi].sp_v,eax

        add     esi,size tfp
        add     edi,flatpsize
        dec     ecx
        jnz     @@p_l

        mov     ebx,scene.sc_datapos

        call    projection pascal, flatpsize, flatvars
        jbe     @@0                     ;less than tree points

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
        jbe     @@0                     ;face not visible

        call    xclip pascal, flatpsize, flatvars
        jbe     @@0
        sub     edi,ebx                 ;edi = (number of points)*(flatpsize)
        call    wpolygon pascal, ebx, edi
@@0:
        pop     esi                     ;esi -> tface (saved)

        pop     ebx
        add     esi,size tface
        dec     z
        jnz     @@f_l0

        pop     esi
@@m_w:
        call    getnextmesh
        jnz     @@m_l

        ret
drawwater endp



projection proc pascal
arg     spsize:dword, vars_1:dword ;data variables -1
        ;-> ebx = *sp
        ;-> edi = *sp_end
        ;<- ebx = *sp
        ;<- edi = *sp_end
        ;<- flags: if less than 3 points, jump with jbe
local   _sp:dword, _sp_end:dword ;both are pointers

                                        ;x, y, z: components of source point
                                        ;d : prefix for destination points
                                        ;n : prefix for next sorce point

        mov     _sp,ebx                 ;ebx -> source points
        mov     _sp_end,edi             ;edi -> destination points

        fld1
@@z_l:
                                        ;inn = true : point in front of plane
        fld     sp_z[ebx]               ;inn = (z >= 1)
        fcomp   st(1)

        fstsw   ax
        mov     dh,ah                   ;dh = inn
                ;if (inn)
        sahf
        jb      @@0
                ;point in front of projection plane (visible)
        fld     sp_z[ebx]
        fst     sp_z[edi]

        fld     sp_x[ebx]               ;x = xres/2 *(1 + x/z)
        fdiv    st,st(1)
        fadd    st,st(2)
        fmul    xmid
        fstp    sp_x[edi]

        fld     sp_y[ebx]               ;y = yres/2 *(1 - y/z)
        fdivrp  st(1),st
        fsubr   st,st(1)
        fmul    ymid
        fstp    sp_y[edi]

        mov     ecx,vars_1              ;mapping
@@l1:   mov     eax,sp_data[ebx+ecx*4]
        mov     sp_data[edi+ecx*4],eax
        dec     ecx
        jns     @@l1

        add     edi,spsize              ;one destination point added
                ;end of if statement
@@0:
        mov     esi,ebx                 ;ebx -> actual source point
        add     esi,spsize              ;esi -> next source point
        cmp     esi,_sp_end             ;wrap if at last point
        jb      @@wrap
        mov     esi,_sp
@@wrap:
                ;if (inn ^ (nz >= 1))
        fld     sp_z[esi]
        fcomp   st(1)
        fstsw   ax
        xor     ah,dh                   ;dh = inn
        sahf
        jnc     @@1
                ;this or next point behind projection plane
                ;r = (1.0-z)/(nz-z)
        fld1
        fst     sp_z[edi]               ;dz = 1.0
        fsub    sp_z[ebx]
        fld     sp_z[esi]
        fsub    sp_z[ebx]
        fdivp   st(1),st                ;st = r

        fld     sp_x[ebx]               ;x' = x + r*(nx-x)
        fld     sp_x[esi]
        fsub    st,st(1)
        fmul    st,st(2)
        faddp   st(1),st
        fadd    st,st(2)                ;x = xres/2 *(1 + x')
        fmul    xmid
        fstp    sp_x[edi]

        fld     sp_y[ebx]               ;y' = y + r*(ny-y)
        fld     sp_y[esi]
        fsub    st,st(1)
        fmul    st,st(2)
        faddp   st(1),st
        fsubr   st,st(2)                ;y = yres/2 *(1 - y')
        fmul    ymid
        fstp    sp_y[edi]

        mov     ecx,vars_1
@@l2:   fld     sp_data[ebx+ecx*4]
        fld     sp_data[esi+ecx*4]
        fsub    st,st(1)
        fmul    st,st(2)
        faddp   st(1),st
        fstp    sp_data[edi+ecx*4]
        dec     ecx
        jns     @@l2
        fstp    st                      ;remove r

        add     edi,spsize              ;one destination point added
                ;end of if statement
@@1:
        cmp     esi,ebx                 ;ns > s?
        mov     ebx,esi                 ;is same as ebx += point size
        ja      @@z_l                   ;repeat loop

        mov     ebx,_sp_end
                                        ;ebx -> start of s-points
        fstp    st                      ;remove 1.0

        mov     eax,edi
        sub     eax,ebx
        shr     eax,1
        cmp     eax,spsize

@@weg:  ret
projection endp

xclip proc pascal ;x-clipping
arg     spsize:dword, vars:dword ;data variables -1
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
        mov     ecx,spsize
        shr     ecx,2
        rep     movsd                   ;sp[d] = sp[z] und d++
@@0:
        mov     esi,ebx
        add     esi,spsize              ;nz = z+1
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
                ;dieser oder n„chster punkt auáerhalb

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

        mov     ecx,vars
@@vars: fld     [esi].sp_data[ecx*4]
        fsub    [ebx].sp_data[ecx*4]
        fmul    st,st(1)
        fadd    [ebx].sp_data[ecx*4]
        fstp    [edi].sp_data[ecx*4]
        dec     ecx
        jns     @@vars

        fstp    st                      ;r entfernen

        add     edi,spsize              ;d++
@@1:
        cmp     esi,ebx                 ;wz > z?
        mov     ebx,esi                 ;entspricht ebx += spsize
        ja      @@z_l                   ;z-schleife

        mov     ebx,_sp_end             ;ebx -> sp
                                        ;edi -> sp_end
        mov     eax,edi
        sub     eax,ebx
        shr     eax,1
        cmp     eax,spsize
        jbe     @@w

        fiadd   xres                    ;x += xres
        dec     dl                      ;i-schleife
        jns     @@i_l

@@w:    fstp    st                      ;x entfernen
@@weg:  ret
xclip endp

subxclip proc pascal ;x-clipping fr scanline subdiv.
arg     spsize:dword, vars_1:dword ;data variables -1
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
        mov     ecx,spsize
        shr     ecx,2
        rep     movsd                   ;sp[d] = sp[z] und d++
@@0:
        mov     esi,ebx
        add     esi,spsize              ;nz = z+1
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
                ;dieser oder n„chster punkt auáerhalb
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

        mov     ecx,vars_1
@@l:    fld     sp_u[esi+ecx*4]         ;sp[d].u
        fsub    sp_u[ebx+ecx*4]
        fmul    st,st(1)
        fadd    sp_u[ebx+ecx*4]
        fstp    sp_u[edi+ecx*4]
        dec     ecx
        jns     @@l
        fstp    st                      ;r entfernen

        add     edi,spsize              ;d++
@@1:
        cmp     esi,ebx                 ;wz > z?
        mov     ebx,esi                 ;entspricht ebx += spsize
        ja      @@z_l                   ;z-schleife

        mov     ebx,_sp_end             ;ebx -> sp
                                        ;edi -> sp_end
        mov     eax,edi
        sub     eax,ebx
        shr     eax,1
        cmp     eax,spsize
        jbe     @@w

        fiadd   xres                    ;x += xres
        dec     dl                      ;i-schleife
        jns     @@i_l

@@w:    fstp    st                      ;x entfernen
@@weg:  ret
subxclip endp

polygon proc pascal
arg     _sp, sp_end, mat, ifl0; byte layout : ifl0
        ;_sp = *sp, zeiger auf screenpoints
        ;<- drawedflag = 0 : face gezeichnet
local   pend, y, x_y, lb, lc, rb, rc ;int
local   lx, ldx, lu, ldu, lv, ldv ;float
local   rx, rdx, ru, rdu, rv, rdv ;float
local   xa, xe, txt_x, txt_y, mapptr, mapand ;int

        mov     esi,mat
        mov     eax,[esi].mt_mapptr
        mov     mapptr,eax
        mov     eax,[esi].mt_mapand
        mov     mapand,eax


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
        fcom    st(1);ymax              ;gr”áten y-wert finden (endpunkt)
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
        mov     eax,scene.sc_buffer
        shr     eax,2
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
      fist txt_x
        fstp    lu

                ;ldv = (sp[lb].v - sp[ia].v)/(sp[lb].y - sp[ia].y);
        fld     sp_v[edi+esi]
        fsub    sp_v[edi+ebx]
        fdiv    st,st(1)
        fst     ldv

                ;lv = ldv  *(y  - sp[ia].y) + sp[ia].v;
        fmul    st,st(2)
        fadd    sp_v[edi+ebx]
      fist txt_y
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
      fist txt_x
        fstp    lu

        fld     lv
        fadd    ldv
      fist txt_y
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
      ;fist txt_x
        fstp    ru

                ;rdv = (sp[rb].v - sp[ia].v)/(sp[rb].y - sp[ia].y);
        fld     sp_v[edi+esi]
        fsub    sp_v[edi+ebx]
        fdiv    st,st(1)
        fst     rdv

                ;rv = rdv  *(y  - sp[ia].y) + sp[ia].v;
        fmul    st,st(2)
        fadd    sp_v[edi+ebx]
      ;fist txt_y
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
      ;fist txt_x
        fstp    ru

        fld     rv
        fadd    rdv
      ;fist txt_y
        fstp    rv

        fld     rx
        fadd    rdx                     ;rx bleibt im copro
        fst     rx
@@r_z:

        fistp   xe                      ;xe = ceil(rx) (rx entfernen)
        fistp   xa                      ;xa = ceil(lx) (lx entfernen)

;eax = iflp
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
        mov     drawedflag,0
        add     edi,x_y                 ;edi = PPPP

        mov     ebx,txt_x               ;ebx = 00Xx
        movzx   edx,bh                  ;edx = 00?X
        shl     ebx,24                  ;ebx = x000
        mov     bx,word ptr txt_y       ;ebx = x0Yy

        mov     esi,scene.sc_divtab
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
        mov     esi,mapptr              ;esi = TTTT

        push    ebp                     ;ebp : lokale variablen
        push    mapand
        push    ifl0
        mov     ebp,eax
        pop     eax                     ;eax = ifl0

        mov     dh,bh
@@inner:
        and     edx,[esp]
        add     ebx,ebp
        mov     al,[esi+edx]
        adc     dl,cl
        mov     [edi*4],eax
        sub     ecx,256
        mov     dh,bh
        inc     edi
        jnc     @@inner

        pop     eax                     ;mapand entfernen
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

comment #
gpolygon proc pascal
arg     _sp, sp_end, mat, ifl0; byte layout : ifl0
        ;_sp = *sp, zeiger auf screenpoints

local   pend, y, x_y, lb, lc, rb, rc ;int
local   lx, ldx, lu, ldu, lv, ldv ;float
local   rx, rdx, ru, rdu, rv, rdv ;float
local   xa, xe, txt_x, txt_y, mapptr, mapand ;int

        mov     esi,mat
        mov     eax,[esi].mt_mapptr
        mov     mapptr,eax
        mov     eax,[esi].mt_mapand
        mov     mapand,eax


        xor     eax,eax                 ;start- und endpunkt bestimmen
        mov     lb,eax
        mov     rb,eax
        mov     pend,eax

        mov     esi,_sp                 ;esi -> screenpoints
        fld     sp_y[esi]               ;st(0) = ymax
        fld     st                      ;st(1) = ymin

        mov      ecx,sp_end
        sub      ecx,gouraudpsize       ;esi+ecx -> sp[sp_end-1]
@@max_l:

        fld     sp_y[esi+ecx]
        fcom    st(1);ymax              ;gr”áten y-wert finden (endpunkt)
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

        sub     ecx,gouraudpsize
        jnz     @@max_l
        fstp    st                      ;ymax entfernen

        fistp   y                       ;y = ceil(ymin)

        mov     eax,y

        cmp     eax,yres                ;y-clipping
        jge     @@weg

        or      eax,eax
        jg      @@y0
        mov     y,0
        xor     eax,eax
@@y0:
        imul    eax,xres
        mov     edx,scene.sc_buffer
        shr     edx,2
        add     eax,edx
        mov     x_y,eax

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
        sub     esi,gouraudpsize        ;lb--
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
      fist txt_x
        fstp    lu

                ;ldv = (sp[lb].v - sp[ia].v)/(sp[lb].y - sp[ia].y);
        fld     sp_v[edi+esi]
        fsub    sp_v[edi+ebx]
        fdiv    st,st(1)
        fst     ldv

                ;lv = ldv  *(y  - sp[ia].y) + sp[ia].v;
        fmul    st,st(2)
        fadd    sp_v[edi+ebx]
      fist txt_y
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
      fist txt_x
        fstp    lu

        fld     lv
        fadd    ldv
      fist txt_y
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
        add     esi,gouraudpsize        ;rb++
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
;      fist txt_x
        fstp    ru

                ;rdv = (sp[rb].v - sp[ia].v)/(sp[rb].y - sp[ia].y);
        fld     sp_v[edi+esi]
        fsub    sp_v[edi+ebx]
        fdiv    st,st(1)
        fst     rdv

                ;rv = rdv  *(y  - sp[ia].y) + sp[ia].v;
        fmul    st,st(2)
        fadd    sp_v[edi+ebx]
;      fist txt_y
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
;      fist txt_x
        fstp    ru

        fld     rv
        fadd    rdv
;      fist txt_y
        fstp    rv

        fld     rx
        fadd    rdx                     ;rx bleibt im copro
        fst     rx
@@r_z:

        fistp   xe                      ;xe = ceil(rx) (rx entfernen)
        fistp   xa                      ;xa = ceil(lx) (lx entfernen)

;eax = iflp
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

        mov     esi,scene.sc_divtab
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

        mov     esi,mapptr              ;esi = TTTT
        push    ebp                     ;ebp : lokale variablen
        push    mapand
        push    ifl0
        mov     ebp,eax
        pop     eax

        mov     dh,bh
@@inner:
        and     edx,[esp]
        add     ebx,ebp
        mov     al,[esi+edx]
        adc     dl,cl
        mov     [edi*4],eax
        sub     ecx,256
        mov     dh,bh
        inc     edi
        jnc     @@inner

        pop     eax                     ;mapand entfernen
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
gpolygon endp
#

subpolygon proc pascal ;texture mapping mit scanline subdivision
arg     _sp, sp_end, mat, ifl0; byte layout : ifl0
        ;_sp = *sp, zeiger auf screenpoints
        ;<- drawedflag = 0 : face gezeichnet
local   pend, y, x_y, lb, lc, rb, rc ;int
local   lx, ldx, rx, rdx ;float
local   xa, xe, txt_x, txt_y, mapptr, mapand ;int
local   la:dword:4, lu:dword:4, ra:dword:4, ru:dword:4, a:dword:4, u:dword:4

        mov     esi,mat
        mov     eax,[esi].mt_mapptr
        mov     mapptr,eax
        mov     eax,[esi].mt_mapand
        mov     mapand,eax


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
        fcom    st(1);ymax              ;gr”áten y-wert finden (endpunkt)
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
    ;  dec     eax
        mov     edx,scene.sc_buffer
        shr     edx,2
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
        mov     drawedflag,0
        mov     ebx,txt_x               ;ebx = 00Xx
        movzx   edx,bh                  ;edx = 00?X
        shl     ebx,24                  ;ebx = x000
        mov     bx,word ptr txt_y       ;ebx = x0Yy

        xor     eax,eax
        push    eax                     ;push pixel counter (= 0)

        mov     esi,scene.sc_divtab
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
        mov     esi,mapptr              ;esi = TTTT
        push    ebp                     ;ebp : lokale variablen
        push    mapand
        push    ifl0
        mov     ebp,eax
        pop     eax

        mov     dh,bh
@@inner:
        and     edx,[esp]
        add     ebx,ebp
        mov     al,[esi+edx]
        adc     dl,ch
        mov     [edi*4],eax
        mov     dh,bh
        inc     edi
        dec     cl
        jnz     @@inner

        pop     eax                     ;mapand entfernen
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

comment #
subgpolygon proc pascal ;texture mapping mit scanline subdivision
arg     _sp, sp_end, mat, ifl0; byte layout : ifl0
        ;_sp = *sp, zeiger auf screenpoints

local   pend, y, x_y, lb, lc, rb, rc ;int
local   lx, ldx, rx, rdx ;float
local   xa, xe, txt_x, txt_y, mapptr, mapand ;int
local   la:dword:4, lu:dword:4, ra:dword:4, ru:dword:4, a:dword:4, u:dword:4

        mov     esi,mat
        mov     eax,[esi].mt_mapptr
        mov     mapptr,eax
        mov     eax,[esi].mt_mapand
        mov     mapand,eax


        xor     eax,eax                 ;start- und endpunkt bestimmen
        mov     lb,eax
        mov     rb,eax
        mov     pend,eax

        mov     esi,_sp                 ;esi -> screenpoints
        fld     sp_y[esi]               ;st(0) = ymax
        fld     st                      ;st(1) = ymin

        mov      ecx,sp_end
        sub      ecx,gouraudpsize       ;esi+ecx -> sp[sp_end-1]
@@max_l:

        fld     sp_y[esi+ecx]
        fcom    st(1);ymax              ;gr”áten y-wert finden (endpunkt)
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

        sub     ecx,gouraudpsize
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
    ;  dec     eax
        mov     edx,scene.sc_buffer
        shr     edx,2
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
        sub     esi,gouraudpsize        ;lb--
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
        add     esi,gouraudpsize        ;rb++
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

        mov     esi,scene.sc_divtab
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
        mov     esi,mapptr              ;esi = TTTT
        push    ebp                     ;ebp : lokale variablen
        push    mapand
        push    ifl0
        mov     ebp,eax
        pop     eax

        mov     dh,bh
@@inner:
        and     edx,[esp]
        add     ebx,ebp
        mov     al,[esi+edx]
        adc     dl,ch
        mov     [edi*4],eax
        mov     dh,bh
        inc     edi
        dec     cl
        jnz     @@inner

        pop     eax                     ;mapand entfernen
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
subgpolygon endp
#

wpolygon proc pascal
arg     _sp, sp_end
        ;_sp = *sp, zeiger auf screenpoints
        ;<- eax = -1 : face nicht gezeichnet
local   pend, y, x_y, lb, lc, rb, rc ;int
local   lx, ldx, lu, ldu, lv, ldv ;float
local   rx, rdx, ru, rdu, rv, rdv ;float
local   xa, xe, txt_x, txt_y ;int


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
        fcom    st(1);ymax              ;gr”áten y-wert finden (endpunkt)
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
        mov     eax,scene.sc_buffer
        shr     eax,2
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

;eax = 00Up
;ebx = x0Yy
;ecx = CCCC
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

        mov     esi,scene.sc_divtab
        fld     ru
        fsub    lu
        fmul    dword ptr[esi+ecx*4]
        fistp   txt_x
        fld     rv
        fsub    lv
        fmul    dword ptr[esi+ecx*4]
        fistp   txt_y

        mov     eax,txt_x               ;eax = 00Uu
        mov     esi,eax
        shl     esi,24                  ;esi = u000
        mov     si,word ptr txt_y       ;esi = u0Vv

        push    ebp                     ;ebp : lokale variablen
        mov     ebp,esi

        mov     esi,frame
        shl     esi,16-6
        and     esi,70000h
        add     esi,scene.sc_water     ;esi = TTTT

        mov     dh,bh
@@inner:
        mov     al,[esi+edx]
        add     ebx,ebp
        adc     dl,ah
        mov     dh,bh
        ;add     [edi*4+2],al
        mov     [edi*4+2],al
        inc     edi
        dec     ecx
        jnz     @@inner

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
wpolygon endp

makelgouraud proc near

        mov     ecx,scene.sc_lmap
        add     ecx,65536

        mov     ch,lvalues-1
@@t_l:          ;texture light value loop

        mov     cl,127
@@g_l:          ;gouraud light value loop
        movsx   eax,cl
        sub     eax,12
        js      @@4
        movzx   edx,ch
        imul    eax,edx
        shr     eax,5
        cmp     eax,edx
        jb      @@5
        mov     eax,edx

        jmp     @@5
@@4:    xor     eax,eax
@@5:

        mov     [ecx],al

        dec     cl
        jno     @@g_l

        sub     ch,1
        jnc     @@t_l


@@weg:  ret
makelgouraud endp

dolights proc pascal
local   vstamp, datapos, lightptr
local   tc, vtc, cubelistend
local   z, flags ;alle beleuchteten w„nde vormerken
;1. schatten: w„nde
;   schleife:
;2.   licht: polygone
;3.   schatten: plygone
;4. licht: w„nde

if testcode gt 0
local lightcount
 mov lightcount,0
endif
        mov     eax,stamp
        mov     vstamp,eax

        mov     eax,scene.sc_datapos
        mov     datapos,eax

        mov     esi,scene.sc_firstlight
        mov     lightptr,esi

@@l_l:  mov     esi,lightptr            ;esi -> tlight
        cmp     esi,scene.sc_lastlight
        jae     @@weg

        xor     eax,eax
        cmp     [esi].o_hidden,eax
        jne     @@next                  ;versteckt
        cmp     [esi].o_cube,eax
        je      @@next                  ;in keinem wrfel

if testcode gt 0
 inc lightcount
endif
                ;volume light
        test    [esi].o_flags,lfVol     ;clear rendered z-buffer lines
        jz      @@Nvol
        mov     edi,scene.sc_zline
        mov     eax,c_max
        mov     ecx,3*zxres
        rep     stosd
@@Nvol:
                ;make viewer
        lea     edi,light               ;edi -> tlviewer
        call    doviewer                ;esi -> tlight
        call    adjustlight

                ;lens flare
        test    [esi].o_flags,lfFlare   ;esi -> tlight
        jz      @@Nflare
        push    esi
        call    flare
        pop     esi
@@Nflare:

                ;copy map
        test    [esi].o_flags,lfWater   ;esi -> tlight
        mov     esi,[esi].l_lmap
        jz      @@Nwater
        mov     eax,frame
        shl     eax,16-6
        and     eax,70000h
        add     esi,eax
@@Nwater:
        mov     edi,scene.sc_lmap      ;copy lightmap
        mov     ecx,65536/4
        rep     movsd


        lea     esi,light               ;cubetree bzgl. licht
        mov     edi,datapos
        call    cubetree pascal, 26;16

        mov     esi,light.v_cubelist    ;esi -> licht-tempcube
        mov     tc,esi
        mov     eax,scene.sc_datapos
        mov     cubelistend,eax

@@c_l:          ;wrfel durchgehen (vorne nach hinten)
        mov     esi,tc
        cmp     esi,cubelistend
        jae     @@vol                   ;n„chstes licht

        mov     ebx,[esi].tc_cubeptr    ;ebx -> cube
        mov     edi,[ebx].c_vtc
        mov     vtc,edi                 ;vtc : viewer-tempcube

        mov     ecx,[ebx].c_faces       ;w„nde durchgehen
        jecxz   @@o
        mov     z,ecx
        mov     ebx,[ebx].c_facedata    ;ebx -> faces
@@s_l0:
        fld     light.v_p.x1
        fsub    [ebx].cf_p.x1
        fmul    [ebx].cf_n.x1
        fld     light.v_p.x2
        fsub    [ebx].cf_p.x2
        fmul    [ebx].cf_n.x2
        faddp   st(1),st
        fld     light.v_p.x3
        fsub    [ebx].cf_p.x3
        fmul    [ebx].cf_n.x3
        faddp   st(1),st
        ftst
        fstsw   ax

        shl     flags,1
        fstp    st

        sahf
        ja      @@s0
                ;1. wand wirft schatten
        inc     flags
        push    ebx
        mov     eax,[ebx].cf_points
        add     ebx,size tcface
                                        ;ebx -> source points of type tcpoint
        add     eax,ebx                 ;eax -> sp_end / next face

        mov     edi,scene.sc_datapos
        call    scubezclip              ;esi -> light-ttempcube
        jbe     @@sw
        sub     edi,ebx
        push    esi
        call    spolygon pascal, ebx, edi
        pop     esi                     ;esi -> light-ttempcube

@@sw:   pop     ebx
@@s0:

        add     ebx,[ebx].cf_points
        add     ebx,size tcface
        dec     z
        jnz     @@s_l0
@@o:

;---

        mov     esi,tc
        mov     esi,[esi].tc_meshlist
@@h_l:
        or      esi,esi
        jz      @@h0
        push    esi                     ;esi -> top level mesh
        test    [esi].o_flags,mfWater
        jnz     @@hw
        lea     edi,light

        call    xformhierarchy          ;transform one hierarchy

        test    [esi].o_flags,mfSort
        jnz     @@sort
@@m_l:          ;unsorted
        mov     ecx,[esi].m_faces
        jecxz   @@mw
        mov     z,ecx
        mov     edi,scene.sc_datapos   ;edi -> shadow faces buffer
        mov     ebx,[esi].m_facelist
@@f_l0:                                 ;ebx -> tface
        push    ebx esi                 ;esi -> tmesh
                ;2. polygon wird beleuchtet
        call    ldrawface pascal, vstamp

        pop     esi ebx
        add     ebx,size tface
        dec     z
        jnz     @@f_l0
@@mw:
        call    getnextmesh
        jnz     @@m_l
                ;3. polygon wirft schatten
        call    sdrawfaces              ;edi -> shadow faces buffer

        jmp     @@hw

@@sort:         ;sorted                 ;edi -> tviewer

        mov     z,15
@@a_l:          ;array loop
        mov     ecx,z
        xor     ecx,15                  ;reverse order
        mov     ebx,light.v_tempfaces[ecx*4]
        mov     edi,scene.sc_datapos   ;edi -> shadow faces buffer
@@f_l1: or      ebx,ebx                 ;ebx -> ttempface
        jz      @@fw
        push    ebx
        mov     esi,[ebx].tf_meshptr    ;esi -> tmesh
        mov     ebx,[ebx].tf_faceptr    ;ebx -> tface
                ;2. polygon wird beleuchtet
        call    ldrawface pascal, vstamp

        pop     ebx
        mov     ebx,[ebx].tf_next
        jmp     @@f_l1
@@fw:           ;3. polygon wirft schatten
        call    sdrawfaces              ;edi -> shadow faces buffer

        dec     z
        jns     @@a_l

@@hw:
        pop     esi
        mov     esi,[esi].m_nexttemp
        jmp     @@h_l
@@h0:
comment #
      mov esi,tc
      mov esi,[esi].tc_cubeptr
      mov esi,[esi].c_objlist
@@fll:
      or esi,esi
      jz @@flw
      test [esi].o_flags,ofCamera
      jnz @@fl
      mov esi,[esi].o_nextincube
      jmp @@fll
@@fl:
      call flare
@@flw:
#
;---            ;w„nde erneut durchgehen
        mov     esi,tc
        mov     ebx,[esi].tc_cubeptr    ;ebx -> cube
        mov     eax,vstamp
        cmp     eax,[ebx].c_vstamp
        jne     @@nc                    ;wrfel nicht sichtbar
        mov     ecx,[ebx].c_faces
        jecxz   @@nc
        mov     z,ecx
        neg     cl
        add     cl,32
        shl     flags,cl
        mov     ebx,[ebx].c_facedata    ;ebx -> tcface (cube-face)
@@s_l1:         ;side loop
                                        ;ebx -> tcface

        mov     ecx,[ebx].cf_points
        add     ebx,size tcface         ;ebx -> tcpoint (cube-point)
        add     ecx,ebx
        push    ecx                     ;ecx -> next face

        shl     flags,1
        jc      @@ns
                ;4. wand wird beleuchtet

        mov     edi,scene.sc_datapos

@@p_l:          ;point loop
        mov     edx,[ebx].cp_p

        mov     esi,tc                  ;esi -> tempcube
        mov     eax,[esi].tc_proj[edx].pr_x
        mov     [edi].sp_lx,eax
        mov     eax,[esi].tc_proj[edx].pr_y
        mov     [edi].sp_ly,eax
        mov     eax,[esi].tc_proj[edx].pr_z
        mov     [edi].sp_lz,eax

        mov     esi,vtc                 ;esi -> camera-tempcube
        mov     eax,[esi].tc_proj[edx].pr_x
        mov     [edi].sp_x,eax
        mov     eax,[esi].tc_proj[edx].pr_y
        mov     [edi].sp_y,eax
        mov     eax,[esi].tc_proj[edx].pr_z
        mov     [edi].sp_z,eax

        add     edi,lightpsize
        add     ebx,size tcpoint        ;next point
        cmp     ebx,ecx
        jb      @@p_l
                                        ;esi -> camera-tempcube
        mov     ebx,scene.sc_datapos

        call    ldraw pascal, [esi].tc_idx, [esi].tc_recursion

@@ns:           ;next side
        pop     ebx                     ;ebx -> tcface

        dec     z
        jnz     @@s_l1

@@nc:           ;next cube
        add     tc,size ttempcube
        jmp     @@c_l

@@vol:
        mov     esi,lightptr            ;esi -> tlight
        mov     eax,[esi].o_flags
        test    eax,lfVol;[esi].o_flags,lfVol
        jz      @@next
        call    vollight pascal, vstamp, datapos, eax

@@next:         ;next light
        add     lightptr,size tlight
        jmp     @@l_l

@@weg:
if testcode gt 0
 mov edi,scene.sc_buffer
 mov eax,fvalues-1
 shl eax,16
 mov ecx,lightcount
 jecxz @@tw
@@tl0:
 mov [edi],eax
 mov dword ptr [edi+4],0
 add edi,8
 dec ecx
 jnz @@tl0
@@tw:
endif
        ret
dolights endp

;comment #
flare proc pascal
local   p:dword:3
local   isx, isy, sx, sy, sz
local   su, sv
local   intensity, coscl

        lea     ebx,viewer
        lea     esi,light

        mov     ecx,2
@@l0:   fld     [esi].v_p[ecx*4]
        fsub    [ebx].v_p[ecx*4]
        fstp    p[ecx*4]
        dec     ecx
        jns     @@l0

                ;distance light to camera
        fld     p[0]
        fmul    [ebx].v_l3[0]
        fld     p[4]
        fmul    [ebx].v_l3[4]
        fld     p[8]
        fmul    [ebx].v_l3[8]
        faddp   st(1)
        faddp   st(1)

        fld1
        fcomp
        fstsw   ax
        fstp    sz
        sahf
        ja      @@weg

                ;cos a (angle between camera and light)
        fld     [ebx].v_l3[0]
        fmul    [esi].v_l3[0]
        fld     [ebx].v_l3[4]
        fmul    [esi].v_l3[4]
        fld     [ebx].v_l3[8]
        fmul    [esi].v_l3[8]
        faddp   st(1)
        faddp   st(1)
        fchs

        fmul    c_16
        fistp   coscl
        cmp     coscl,0
        jle     @@weg

                ;x pos on screen
        fld     p[0]
        fmul    [ebx].v_l1[0]
        fld     p[4]
        fmul    [ebx].v_l1[4]
        fld     p[8]
        fmul    [ebx].v_l1[8]
        faddp   st(1)
        faddp   st(1)
        fdiv    sz
;        fld1
;        faddp   st(1)
        fmul    xmid
       fst     su
       fadd    xmid
        fist    isx
        fstp    sx

                ;y pos on screen
        fld     p[0]
        fmul    [ebx].v_l2[0]
        fld     p[4]
        fmul    [ebx].v_l2[4]
        fld     p[8]
        fmul    [ebx].v_l2[8]
        faddp   st(1)
        faddp   st(1)
        fdiv    sz
;        fld1
;        fsubrp  st(1)
        fmul    ymid
       fchs
       fst     sv
       fadd    ymid
        fist    isy
        fstp    sy


        mov     intensity,0

        mov     al,byte ptr flare_idx
        mov     ebx,isx
      dec ebx
        mov     ecx,xres
        mov     edx,isy
        imul    edx,xres
        mov     esi,scene.sc_buffer
        add     esi,3
if testcode gt 0
 xor ah,ah
endif
        cmp     ebx,ecx
        jae     @@0
        cmp     edx,flare_ycmp
        jae     @@0
        lea     edi,[ebx+edx]
if testcode gt 0
   mov [esi+edi*4-3],ah
endif
        cmp     al,[esi+edi*4]
        jbe     @@0
        inc     intensity
@@0:
        add     ebx,flare_dx

        cmp     ebx,ecx
        jae     @@1
        lea     edi,[ebx+edx]
if testcode gt 0
   mov [esi+edi*4-3],ah
endif
        cmp     al,[esi+edi*4]
        jbe     @@1
        inc     intensity
@@1:
        sub     ebx,flare_dx
        add     edx,flare_dy

        cmp     edx,flare_ycmp
        jae     @@2
        lea     edi,[ebx+edx]
if testcode gt 0
   mov [esi+edi*4-3],ah
endif
        cmp     al,[esi+edi*4]
        jbe     @@2
        inc     intensity
@@2:
        sub     edx,flare_dy
        sub     ebx,flare_dx

        cmp     ebx,ecx
        jae     @@3
        lea     edi,[ebx+edx]
if testcode gt 0
   mov [esi+edi*4-3],ah
endif
        cmp     al,[esi+edi*4]
        jbe     @@3
        inc     intensity
@@3:
        add     ebx,flare_dx
        sub     edx,flare_dy

        cmp     edx,flare_ycmp
        jae     @@4
        lea     edi,[ebx+edx]
if testcode gt 0
   mov [esi+edi*4-3],ah
endif
        cmp     al,[esi+edi*4]
        jbe     @@4
        inc     intensity
@@4:
        cmp     intensity,0
        je      @@weg

        mov     eax,coscl
        imul    eax,intensity
        imul    eax,1000
        shr     eax,16-3
        cmp     eax,8
        jbe     @@5
        mov     eax,8
@@5:
        mov     intensity,eax

                ;flare 1
        mov     edi,scene.sc_datapos
        fld     c_10_7
        fdiv    sz
                ;screen area
        fld     xmid
        fmul    st,st(1)                ;xmid*16/sz
        fld     st
        fsubr   sx
        fstp    [edi].f_sx
        fadd    sx
        fstp    [edi+size tflarepoint].f_sx
        fld     ymid
        fmul    c_1_33
        fmulp   st(1),st                ;ymid*16/sz
        fld     st
        fsubr   sy
        fstp    [edi].f_sy
        fadd    sy
        fstp    [edi+size tflarepoint].f_sy

                ;map area                ;(x:128,y:0, x:256,y:128)*256
        fld     c_32768
        fst     [edi].f_mx
        fadd    flare_res;c_lscale
        fstp    [edi+size tflarepoint].f_mx
        fldz
        fstp    [edi].f_my
        fld     flare_res;c_lscale
        fstp    [edi+size tflarepoint].f_my

        call    fclip
        jecxz   @@clip1
        call    fdraw pascal, intensity
@@clip1:

                ;flare 2
        mov     edi,scene.sc_datapos
                ;screen area
        fld     su
        fmul    flare_2s
        fadd    sx                      ;xpos
        fld     xmid
        fmul    flare_2x
        fld     st
        fsubr   st,st(2)
        fstp    [edi].f_sx
        faddp   st(1),st
        fstp    [edi+size tflarepoint].f_sx

        fld     sv
        fmul    flare_2s
        fadd    sy                      ;ypos
        fld     ymid
        fmul    flare_2y
        fld     st
        fsubr   st,st(2)
        fstp    [edi].f_sy
        faddp   st(1),st
        fstp    [edi+size tflarepoint].f_sy

                ;map area               ;(x:0,y:0, x:128,y:128)*256
        fldz
        fst     [edi].f_mx
        fstp    [edi].f_my
        fld     flare_res
        fst     [edi+size tflarepoint].f_mx
        fstp    [edi+size tflarepoint].f_my

        call    fclip
        jecxz   @@clip2
        call    fdraw pascal, intensity
@@clip2:

                ;flare 3
        mov     edi,scene.sc_datapos
                ;screen area
        fld     su
        fmul    flare_3s
        fadd    sx                      ;xpos
        fld     xmid
        fmul    flare_3x
        fld     st
        fsubr   st,st(2)
        fstp    [edi].f_sx
        faddp   st(1),st
        fstp    [edi+size tflarepoint].f_sx

        fld     sv
        fmul    flare_3s
        fadd    sy                      ;ypos
        fld     ymid
        fmul    flare_3y
        fld     st
        fsubr   st,st(2)
        fstp    [edi].f_sy
        faddp   st(1),st
        fstp    [edi+size tflarepoint].f_sy

                ;map area               ;(x:0,y:0, x:128,y:128)*256
        fldz
        fst     [edi].f_mx
        fstp    [edi].f_my
        fld     flare_res
        fst     [edi+size tflarepoint].f_mx
        fstp    [edi+size tflarepoint].f_my

        call    fclip
        jecxz   @@clip3
        call    fdraw pascal, intensity
@@clip3:
                ;flare 4
        mov     edi,scene.sc_datapos
                ;screen area
        fld     su
        fmul    flare_4s
        fadd    sx                      ;xpos
        fld     xmid
        fmul    flare_4x
        fld     st
        fsubr   st,st(2)
        fstp    [edi].f_sx
        faddp   st(1),st
        fstp    [edi+size tflarepoint].f_sx

        fld     sv
        fmul    flare_4s
        fadd    sy                      ;ypos
        fld     ymid
        fmul    flare_4y
        fld     st
        fsubr   st,st(2)
        fstp    [edi].f_sy
        faddp   st(1),st
        fstp    [edi+size tflarepoint].f_sy

                ;map area                ;(x:128,y:0, x:256,y:128)*256
        fld     c_32768
        fst     [edi].f_mx
        fadd    flare_res;c_lscale
        fstp    [edi+size tflarepoint].f_mx
        fldz
        fstp    [edi].f_my
        fld     flare_res;c_lscale
        fstp    [edi+size tflarepoint].f_my

        call    fclip
        jecxz   @@clip4
        call    fdraw pascal, intensity
@@clip4:

@@weg:  ret
flare endp

fclip proc near
        xor     ecx,ecx

        fldz
                ;sx bound check
        fld     [edi].f_sx
        ficomp  xres
        fstsw   ax
        sahf
        ja      @@w
        fld     [edi+size tflarepoint].f_sx
        fcomp   st(1)
        fstsw   ax
        sahf
        jb      @@w
                ;sy bound check
        fld     [edi].f_sy
        ficomp  yres
        fstsw   ax
        sahf
        ja      @@w
        fld     [edi+size tflarepoint].f_sy
        fcomp   st(1)
        fstsw   ax
        sahf
        jb      @@w

                ;sx clip
        fld     [edi].f_sx
        fcomp   st(1)
        fstsw   ax
        sahf
        ja      @@0

        fld     [edi].f_sx
        fld     [edi+size tflarepoint].f_sx
        fsubr   st,st(1)
        fdivp   st(1),st

        fld     [edi].f_mx
        fld     [edi+size tflarepoint].f_mx
        fsub    st,st(1)
        fmulp   st(2),st
        faddp   st(1),st
        fstp    [edi].f_mx

        fst     [edi].f_sx              ;sx = 0

@@0:
        fld     [edi+size tflarepoint].f_sx
        ficomp  xres
        fstsw   ax
        sahf
        jb      @@1

        fld     [edi+size tflarepoint].f_sx
        fild    xres
        fst     [edi+size tflarepoint].f_sx ;sx = xres
        fsub    st,st(1)
        fld     [edi].f_sx
        fsubrp  st(2),st
        fdivrp  st(1),st

        fld     [edi+size tflarepoint].f_mx
        fld     [edi].f_mx
        fsub    st,st(1)
        fmulp   st(2),st
        faddp   st(1),st
        fstp    [edi+size tflarepoint].f_mx

@@1:
                ;sy clip
        fld     [edi].f_sy
        fcomp   st(1)
        fstsw   ax
        sahf
        ja      @@2

        fld     [edi].f_sy
        fld     [edi+size tflarepoint].f_sy
        fsubr   st,st(1)
        fdivp   st(1),st

        fld     [edi].f_my
        fld     [edi+size tflarepoint].f_my
        fsub    st,st(1)
        fmulp   st(2),st
        faddp   st(1),st
        fstp    [edi].f_my

        fst     [edi].f_sy              ;sy = 0

@@2:
        fld     [edi+size tflarepoint].f_sy
        ficomp  yres
        fstsw   ax
        sahf
        jb      @@3

        fld     [edi+size tflarepoint].f_sy
        fild    yres
        fst     [edi+size tflarepoint].f_sy ;sy = yres
        fsub    st,st(1)
        fld     [edi].f_sy
        fsubrp  st(2),st
        fdivrp  st(1),st

        fld     [edi+size tflarepoint].f_my
        fld     [edi].f_my
        fsub    st,st(1)
        fmulp   st(2),st
        faddp   st(1),st
        fstp    [edi+size tflarepoint].f_my

@@3:

        inc     ecx                     ;ecx = 0: not visible

@@w:    fstp    st

@@weg:  ret
fclip endp

fdraw proc pascal
arg     intensity
local   xa, xe, ya, ye
local   mx, my, mu, mv
local   xcount, ycount


        fld     [edi].f_sx
        fistp   xa
        fld     [edi+size tflarepoint].f_sx
        fistp   xe
        fld     [edi].f_sy
        fistp   ya
        fld     [edi+size tflarepoint].f_sy
        fistp   ye

        mov     ebx,xa
        mov     ecx,xe
        sub     ecx,ebx
        jle     @@weg

        mov     eax,ya
        mov     edx,ye
        sub     edx,eax
        jle     @@weg

        imul    eax,xres
        add     ebx,eax
        shl     ebx,2

        mov     eax,scene.sc_divtab
        fld     [edi].f_mx
        fist    mx
        fsubr   [edi+size tflarepoint].f_mx
        fmul    dword ptr [eax+ecx*4]
        fistp   mu

        fld     [edi].f_my
        fist    my
        fsubr   [edi+size tflarepoint].f_my
        fmul    dword ptr [eax+edx*4]
        fistp   mv

        mov     xcount,ecx
        mov     ycount,edx

        mov     edi,scene.sc_buffer
        add     edi,2
        add     edi,ebx

        mov     esi,scene.sc_flares
        movzx   edx,word ptr my

        shl     intensity,6
        mov     ecx,scene.sc_flaretab
        add     ecx,intensity

        mov     ebx,mu
                                        ;eax = 000x
                                        ;ebx = 00Uu
                                        ;edx = LLYX
                                        ;esi =
                                        ;edi = SSSS
@@y_l:
        mov     eax,mx
        mov     dl,ah

        push    edi

        push    ebp
        mov     ebp,xcount
@@x_l:
;      mov     ah,[esi+edx]
        or      cl,[esi+edx]
        add     al,bl
        adc     dl,bh
        mov     ah,[ecx]
        and     cl,0C0h
        add     [edi],ah
        add     edi,4
        dec     ebp
        jnz     @@x_l
        pop     ebp

        mov     eax,mv
        add     byte ptr my,al
        adc     dh,ah

        pop     edi

        mov     eax,xres
        shl     eax,2
        add     edi,eax
        dec     ycount
        jnz     @@y_l

@@weg:  ret
fdraw endp
;#


ldrawface proc pascal
arg     vstamp
local   idx
        ;-> ebx -> tface
        ;-> esi -> tmesh
        ;-> edi -> shadow faces buffer pos
        ;<- edi -> new shadow faces buffer pos

                ;visibility test
                ;(a          ) x (b          ) *  p
                ;(v[0] - v[1]) x (v[2] - v[1]) * (v[1])

        mov     eax,[ebx].f_p[0].fp_vertex            ;eax -> v[0]
        mov     edx,[ebx].f_p[1*(size tfp)].fp_vertex ;edx -> v[1]
        mov     ecx,[ebx].f_p[2*(size tfp)].fp_vertex ;ecx -> v[2]

        push    esi
        mov     esi,[esi].m_tlist

        fld     [esi+edx].x1            ;v[1] = p
        fld     [esi+edx].x2
        fld     [esi+edx].x3
       fst     [edi+shadowpsize].sp_z

                ;1. komponente
        fld     [esi+eax].x2
       fst     [edi].sp_y;store shadow face in buffer
        fsub    st,st(2)                ;a2
        fld     [esi+ecx].x3
       fst     [edi+2*shadowpsize].sp_z
        fsub    st,st(2)                ;b3
        fmulp   st(1),st

        fld     [esi+eax].x3
        fsub    st,st(2)                ;a3
        fld     [esi+ecx].x2
        fsub    st,st(4)                ;b2
        fmulp   st(1),st

        fsubp   st(1),st
        fmul    st,st(3)                ;*p1

                ;2. komponente
        fld     [esi+eax].x3
       fst     [edi].sp_z
        fsub    st,st(2)                ;a3
        fld     [esi+ecx].x1
       fst     [edi+2*shadowpsize].sp_x
        fsub    st,st(5)                ;b1
        fmulp   st(1),st

        fld     [esi+eax].x1
        fsub    st,st(5)                ;a1
        fld     [esi+ecx].x3
        fsub    st,st(4)                ;b3
        fmulp   st(1),st

        fsubp   st(1),st
        fmul    st,st(3)                ;*p2
        faddp   st(1),st

                ;3. komponente
        fld     [esi+eax].x1
       fst     [edi].sp_x
        fsub    st,st(4)                ;a1
        fld     [esi+ecx].x2
       fst     [edi+2*shadowpsize].sp_y
        fsub    st,st(4)                ;b2
        fmulp   st(1),st

        fld     [esi+eax].x2
        fsub    st,st(4)                ;a2
        fld     [esi+ecx].x1
        fsub    st,st(6)                ;b1
        fmulp   st(1),st

        fsubp   st(1),st
        fmulp   st(2),st                ;*p3
        faddp   st(1),st

        ftst
        fstsw   ax
       fstp    st
       fstp    [edi+shadowpsize].sp_y
       fstp    [edi+shadowpsize].sp_x

        pop     esi                     ;esi -> tmesh

        sahf

        jb      @@shadow
                ;face lit (discard shadow face)
        mov     eax,vstamp
        cmp     eax,[esi].m_vstamp      ;object seen from camera?
        jne     @@weg

        mov     eax,[ebx].f_idx
        or      eax,eax                 ;face visible?
        js      @@zline
        mov     idx,eax

if (offset f_p) gt 0
        lea     ebx,[ebx].f_p           ;ebx -> tfp
endif
        mov     eax,[esi].m_vertices    ;eax = number of vertices
        mov     edx,[esi].m_tlist       ;edx -> vertices from light's view
        lea     eax,[eax+eax*2]         ;eax*3
        lea     edx,[edx+eax*4]         ;edx -> normals

        mov     eax,[ebx+2*(size tfp)].fp_vertex
        fld     dword ptr [edx+eax];.x3
        fcom    c_lgouraud
        fstsw   ax
        mov     cl,ah
        mov     eax,[ebx+(size tfp)].fp_vertex
        fld     dword ptr [edx+eax];.x3
        fcom    c_lgouraud
        fstsw   ax
        or      cl,ah
        mov     eax,[ebx].fp_vertex
        fld     dword ptr [edx+eax];.x3
        fcom    c_lgouraud
        fstsw   ax
        or      ah,cl

        mov     edx,[esi].m_vtlist      ;edx -> vertices from camera's view
        mov     esi,[esi].m_tlist       ;esi -> vertices from light's view
                                        ;edi -> mem for lightpoints
        sahf
        jc      @@g
;jmp @@g
                ;without gouraud shading
        push    edi
        mov     ecx,2

@@l:
        mov     eax,[ebx].fp_vertex
        fld     [edx+eax].x1
        fstp    [edi].sp_x
        fld     [edx+eax].x2
        fstp    [edi].sp_y
        fld     [edx+eax].x3
        fstp    [edi].sp_z
        fld     [esi+eax].x1
        fstp    [edi].sp_lx
        fld     [esi+eax].x2
        fstp    [edi].sp_ly
        fld     [esi+eax].x3
        fstp    [edi].sp_lz
        fstp    st

        add     ebx,size tfp
        add     edi,lightpsize
        dec     ecx
        jns     @@l

        mov     ebx,[esp]               ;ebx -> lightpoints
                                        ;edi -> lightpoints-end
        call    ldraw pascal, idx, -1

        pop     edi
        jmp     @@weg

@@g:            ;with gouraud shading
        push    edi
        mov     ecx,2

@@l1:
        mov     eax,[ebx].fp_vertex
        fld     [edx+eax].x1
        fstp    [edi].sp_x
        fld     [edx+eax].x2
        fstp    [edi].sp_y
        fld     [edx+eax].x3
        fstp    [edi].sp_z
        fld     [esi+eax].x1
        fstp    [edi].sp_lx
        fld     [esi+eax].x2
        fstp    [edi].sp_ly
        fld     [esi+eax].x3
        fstp    [edi].sp_lz
        fstp    [edi].sp_ll

        add     ebx,size tfp
        add     edi,glightpsize
        dec     ecx
        jns     @@l1

        mov     ebx,[esp]               ;ebx -> lightpoints
                                        ;edi -> lightpoints-end
        call    gldraw pascal, idx

        pop     edi
        jmp     @@weg

@@zline:        ;render back-faces onto z-line
;   jmp @@weg;!
      test light.lv_flags,lfVol      ;volume light turned on?
      jz @@weg
        push    edi
        mov     esi,[esi].m_tlist       ;esi -> vertices from light's view
        mov     ecx,2
if (offset f_p) gt 0
        lea     ebx,[ebx].f_p           ;ebx -> tfp
endif
@@l2:
        mov     eax,[ebx].fp_vertex
        fld     [esi+eax].x1
        fstp    [edi].sp_x
        fld     [esi+eax].x2
        fstp    [edi].sp_y
        fld     [esi+eax].x3
        fstp    [edi].sp_z

        add     ebx,size tfp
        add     edi,zlinepsize
        dec     ecx
        jns     @@l2

        mov     ebx,[esp]               ;ebx -> points
                                        ;edi -> points-end

        call    zprojectline
        jbe     @@w
        sub     edi,ebx
        call    renderline pascal, ebx, edi

@@w:
        pop     edi
        jmp     @@weg

@@shadow:       ;face in shadow
                                        ;esi -> tmesh
        test    [esi].o_flags,mfNocast  ;cast shadows?
        jnz     @@weg
        add     edi,3*shadowpsize       ;keep shadow face

@@weg:  ret
ldrawface endp

sdrawfaces proc near;pascal
;arg     _mapptr ;pointer to pointer
        ;-> edi -> end of shadow faces buffer

                                        ;scene.sc_datapos -> shadow faces buffer
@@l:
        lea     ebx,[edi-shadowpsize*3] ;ebx -> start of points array
                                        ;edi -> end of points array
        cmp     ebx,scene.sc_datapos
        jb      @@weg
        push    ebx
        call    sprojection
        jbe     @@0
 ;       call    sxclip
 ;        jbe     @@sw
        sub     edi,ebx

        call    spolygon pascal, ebx, edi;, _mapptr
@@0:
        pop     edi
        jmp     @@l

@@weg:  ret
sdrawfaces endp

ldraw proc pascal
arg     idx, recursion;mapptr, idx, recursion
        ;-> ebx -> sp
        ;-> edi -> sp_end

        fldz                            ;st = abstand = 0.0
        xor     eax,eax                 ;x-clip
        mov     edx,offset sp_lx
        call    lightclip pascal, lightpsize;, lightvars
        jbe     @@0

        xor     eax,eax
        dec     eax
        mov     edx,offset sp_lx
        call    lightclip pascal, lightpsize;, lightvars
        jbe     @@0

        xor     eax,eax                 ;y-clip
        mov     edx,offset sp_ly
        call    lightclip pascal, lightpsize;, lightvars
        jbe     @@0

        xor     eax,eax
        dec     eax
        mov     edx,offset sp_ly
        call    lightclip pascal, lightpsize;, lightvars
        jbe     @@0
        fstp    st                      ;remove distance

        fld1                            ;st = abstand = 1.0
        xor     eax,eax                 ;z-clip
        mov     edx,offset sp_lz
        call    lightclip pascal, lightpsize;, lightvars
        jbe     @@0
        fstp    st                      ;remove distance

                ;draw onto z-line
      test light.lv_flags,lfVol      ;volume light turned on?
      jz @@Nvol
        push    ebx edi
        call    projectline pascal, lightpsize
        sub     edi,ebx
        call    renderline pascal, ebx, edi
        pop     edi ebx
@@Nvol:

        call    lscalexy pascal, lightpsize

        call    projection pascal, lightpsize, lightvars
        jbe     @@weg

        cmp     recursion,l_sub
;      jmp @@_sub
        ja      @@_sub
        call    subxclip pascal, lightpsize, lightvars
        jbe     @@weg
        sub     edi,ebx
        mov     eax,idx
        shl     eax,8
        call    sublpolygon pascal, ebx, edi, eax;mapptr, eax
        jmp     @@weg

@@_sub:         ;ohne subdivision
        call    lconvert
        call    xclip pascal, flatpsize, flatvars
        jbe     @@weg
        sub     edi,ebx
        call    lpolygon pascal, ebx, edi, idx;mapptr, eax
        jmp     @@weg

@@0:    fstp    st

@@weg:  ret
ldraw endp

gldraw proc pascal
arg     idx;mapptr:dword, idx:dword
        ;-> ebx -> sp
        ;-> edi -> sp_end

        fldz                            ;st = abstand = 0.0
        xor     eax,eax                 ;x-clip
        mov     edx,offset sp_lx
        call    lightclip pascal, glightpsize;, glightvars
        jbe     @@0

        xor     eax,eax
        dec     eax
        mov     edx,offset sp_lx
        call    lightclip pascal, glightpsize;, glightvars
        jbe     @@0

        xor     eax,eax                 ;y-clip
        mov     edx,offset sp_ly
        call    lightclip pascal, glightpsize;, glightvars
        jbe     @@0

        xor     eax,eax
        dec     eax
        mov     edx,offset sp_ly
        call    lightclip pascal, glightpsize;, glightvars
        jbe     @@0
        fstp    st                      ;remove distance

        fld1                            ;st = abstand = 1.0
        xor     eax,eax                 ;y-clip
        mov     edx,offset sp_lz
        call    lightclip pascal, glightpsize;, glightvars
        jbe     @@0
        fstp    st                      ;remove distance

                ;draw onto z-line
      test light.lv_flags,lfVol      ;volume light turned on?
      jz @@Nvol
        push    ebx edi
        call    projectline pascal, glightpsize
        sub     edi,ebx
        call    renderline pascal, ebx, edi
        pop     edi ebx
@@Nvol:

        call    lscalexy pascal, glightpsize

        call    projection pascal, glightpsize, glightvars
        jbe     @@weg

        call    glconvert
        call    xclip pascal, gouraudpsize, gouraudvars
        jbe     @@weg
        sub     edi,ebx
        call    glpolygon pascal, ebx, edi, idx;mapptr, idx
        jmp     @@weg

@@0:    fstp    st
@@weg:  ret
gldraw endp


lightclip proc pascal
arg     spsize:dword;, vars:dword ;data variables -1
        ;-> st(0) = abstand
        ;-> ebx -> sp
        ;-> edi -> sp_end
        ;-> edx = offset sp_lx, sp_ly oder sp_lz
        ;-> +/- flag in bit 31 von eax

        ;<- ebx -> sp
        ;<- edi -> sp_end
        ;<- wenn weniger als 3 punkte, mit jbe wegspringen

local   _sp, _sp_end ;beides pointer

        mov     _sp,ebx                 ;ebx -> sp[z] (quellpunkte) (z = 0)
        mov     _sp_end,edi             ;edi -> sp[d] (zielpunkte)  (d = m)

;@@z_l:
        fld     sp_lz[ebx]              ;inn = (sp[z].tz >= abstand);
        fld     dword ptr [ebx+edx]
        or      eax,eax
        jns     @@0
        fchs
@@0:    faddp   st(1),st                ;st(0) = sp[z].tz

@@z_l:

        fcom    st(1)                   ;abstand
        fstsw   ax
        mov     ch,1                    ;ch = 1: nicht innerhalb
        sahf
        jb      @@_inn
                ;punkt innerhalb
        mov     esi,ebx
        mov     ecx,spsize
        shr     ecx,2
        rep     movsd                   ;sp[d] = sp[z] und d++
@@_inn:                                 ;ch = 0
        mov     esi,ebx
        add     esi,spsize              ;nz = z+1
        cmp     esi,_sp_end             ;if (nz >= m) nz = 0;
        jb      @@wrap
        mov     esi,_sp
@@wrap:

        fld     sp_lz[esi]              ;if (inn ^ (sp[nz].tz >= abstand))
        fld     dword ptr [esi+edx]
        or      eax,eax
        jns     @@1
        fchs
@@1:    faddp   st(1),st                ;st(0) = sp[nz].tz

        fcom    st(2)                   ;abstand
        fstsw   ax
        xor     ah,ch                   ;ch = inn
        sahf
        jnc     @@2
                ;dieser oder n„chster punkt auáerhalb
                ;r = (abstand - l[z].tz)/(l[nz].tz-l[z].tz);
        fld     st(2)
        fsub    st,st(2)                 ;abstand - l[z].tz
        fld     st(1)
        fsub    st,st(3)                 ;l[nz].tz-l[z].tz
        fdivp   st(1),st                 ;st = r

        mov     ecx,spsize;vars
;        add     ecx,3
                ;d[dm] = s[z] + r*(s[nz]-s[z]) fr x, y, z, lx, ly, lz
@@l:    fld     sp_x[ebx+ecx-4]
        fld     sp_x[esi+ecx-4]
        fsub    st,st(1)
        fmul    st,st(2)
        faddp   st(1),st
        fstp    sp_x[edi+ecx-4]
        sub ecx,4;dec     ecx
        jnz @@l;jns     @@l
        fstp    st                      ;r entfernen

        add     edi,spsize              ;d++
@@2:
        fxch    st(1)
        fstp    st

        cmp     esi,ebx                 ;wz > z?
        mov     ebx,esi                 ;entspricht ebx += spsize
        ja      @@z_l                   ;z-schleife

        fstp    st

        mov     ebx,_sp_end             ;ebx -> sp
                                        ;edi -> sp_end
        mov     eax,edi
        sub     eax,ebx
        shr     eax,1
        cmp     eax,spsize

@@weg:  ret
lightclip endp

lscalexy proc pascal
arg     spsize

        push    ebx
        fld     c_lscale            ;lx*=128*256 und ly*=128*256
@@l:
        fld     sp_lx[ebx]
        fmul    st,st(1)
        fstp    sp_lx[ebx]
        fld     sp_ly[ebx]
        fmul    st,st(1)
        fstp    sp_ly[ebx]

        add     ebx,spsize
        cmp     ebx,edi
        jb      @@l

        fstp    st
        pop     ebx
@@weg:  ret
lscalexy endp

lconvert proc near ;light points -> flat points (projiziert die licht-koordinaten)
        ;-> ebx -> sp
        ;-> edi -> sp_end

        mov     edx,edi
        mov     edi,ebx ;tscreenpoint muá krzer sein als tlightpoint
        mov     esi,ebx
@@l:
       fld     sp_lz[esi]
       fld     sp_lx[esi]
       fdiv    st,st(1)
        mov     eax,sp_x[esi]
        mov     sp_x[edi],eax
        mov     eax,sp_y[esi]
       fstp    sp_u[edi]
       fdivr   sp_ly[esi]
        mov     sp_y[edi],eax
        add     esi,lightpsize
       fstp    sp_v[edi]
        add     edi,flatpsize
        cmp     esi,edx
        jb      @@l

@@weg:  ret
lconvert endp

glconvert proc near ;light points -> flat points (projiziert die licht-koordinaten)
        ;-> ebx -> sp
        ;-> edi -> sp_end

        mov     edx,edi
        mov     edi,ebx ;tscreenpoint muá krzer sein als tlightpoint
        mov     esi,ebx
@@l:
       fld     sp_lz[esi]
       fld     sp_lx[esi]
       fdiv    st,st(1)
        mov     eax,sp_x[esi]
        mov     sp_x[edi],eax
        mov     eax,sp_y[esi]
        mov     sp_y[edi],eax
       fstp    sp_u[edi]
       fdivr   sp_ly[esi]
        mov     eax,sp_ll[esi]
        mov     sp_l[edi],eax
        add     esi,glightpsize
       fstp    sp_v[edi]
        add     edi,gouraudpsize
        cmp     esi,edx
        jb      @@l

@@weg:  ret
glconvert endp

lpolygon proc pascal ;licht-polygon
arg     _sp, sp_end, idx;mapptr, zval : dword
        ;_sp = *sp, zeiger auf screenpoints

local   pend, y, x_y, lb, lc, rb, rc ;int
local   lx, ldx, lu, ldu, lv, ldv
local   rx, rdx, ru, rdu, rv, rdv
local   xa, xe, txt_x, txt_y ;int


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
        fcom    st(1);ymax              ;gr”áten y-wert finden (endpunkt)
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
        mov     y,0
        xor     eax,eax
@@y0:
        imul    eax,xres
        mov     edx,scene.sc_buffer
        shr     edx,2
        add     eax,edx
        mov     x_y,eax

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
      fist txt_x
        fstp    lu

                ;ldv = (sp[lb].v - sp[ia].v)/(sp[lb].y - sp[ia].y);
        fld     sp_v[edi+esi]
        fsub    sp_v[edi+ebx]
        fdiv    st,st(1)
        fst     ldv

                ;lv = ldv  *(y  - sp[ia].y) + sp[ia].v;
        fmul    st,st(2)
        fadd    sp_v[edi+ebx]
      fist txt_y
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
      fist txt_x
        fstp    lu

        fld     lv
        fadd    ldv
      fist txt_y
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

;eax = 00iU
;ebx = x0Yy
;ecx = 000?
;edx = TT?X
;esi = CCCC
;edi = PPPP
;ebp = u0Vv

        mov     edi,xa
        mov     esi,xe
        sub     esi,edi                 ;esi = CCCC
        jle     @@w
        add     edi,x_y                 ;edi = PPPP

        movzx   ebx,word ptr txt_x      ;ebx = 00Xx
        mov     edx,scene.sc_lmap       ;edx = TT00
        mov     dl,bh                   ;edx = TT?X
        shl     ebx,24                  ;ebx = x000
        mov     bx,word ptr txt_y       ;ebx = x0Yy
        add     dl,80h
        add     bh,80h

        mov     eax,scene.sc_divtab
        fld     ru
        fsub    lu
        fmul    dword ptr[eax+esi*4]
        fistp   txt_x
        fld     rv
        fsub    lv
        fmul    dword ptr[eax+esi*4]
        fistp   txt_y

        movzx   ecx,word ptr txt_x      ;ecx = 00Uu
        mov     al,ch                   ;eax = 000U
        mov     ah,byte ptr idx         ;eax = 00iU
        shl     ecx,24                  ;ecx = u000
        mov     cx,word ptr txt_y       ;ecx = u0Vv

        push    ebp
        mov     ebp,ecx

                ;keine lokalen variablen mehr zug„nglich
        jmp     @@inner
@@i0:
        add     ebx,ebp
        adc     dl,al
        inc     edi
        dec     esi
        jz      @@iw
@@inner:
        cmp     ah,[edi*4+3]            ;schreiberlaubnis
        mov     dh,bh
        jne     @@i0
        add     ebx,ebp
        mov     cl,[edx]                ;cl = helligkeit
        adc     dl,al
        add     [edi*4+1],cl  ;+2
        inc     edi
        dec     esi
        jnz     @@inner
@@iw:
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
lpolygon endp

glpolygon proc pascal ;licht-polygon
arg     _sp, sp_end, idx;mapptr, idx : dword
        ;_sp = *sp, zeiger auf screenpoints

local   pend, y, x_y, lb, lc, rb, rc ;int
local   lx, ldx, lu, ldu, lv, ldv, ll, ldl
local   rx, rdx, ru, rdu, rv, rdv, rl, rdl
local   xa, xe, txt_x, txt_y, txt_l ;int


        xor     eax,eax                 ;start- und endpunkt bestimmen
        mov     lb,eax
        mov     rb,eax
        mov     pend,eax

        mov     esi,_sp                 ;esi -> screenpoints
        fld     sp_y[esi]               ;st(0) = ymax
        fld     st                      ;st(1) = ymin

        mov     ecx,sp_end
        sub     ecx,gouraudpsize        ;esi+ecx -> sp[sp_end-1]
@@max_l:

        fld     sp_y[esi+ecx]
        fcom    st(1);ymax              ;gr”áten y-wert finden (endpunkt)
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

        sub     ecx,gouraudpsize
        jnz     @@max_l
        fstp    st                      ;ymax entfernen

        fistp   y                       ;y = ceil(ymin)

        mov     eax,y

        cmp     eax,yres                ;y-clipping
        jge     @@weg

        or      eax,eax
        jg      @@y0
        mov     y,0
        xor     eax,eax
@@y0:
        imul    eax,xres
        mov     edx,scene.sc_buffer
        shr     edx,2
        add     eax,edx
        mov     x_y,eax

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
        sub     esi,gouraudpsize           ;lb--
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
      fist txt_x
        fstp    lu

                ;ldv = (sp[lb].v - sp[ia].v)/(sp[lb].y - sp[ia].y);
        fld     sp_v[edi+esi]
        fsub    sp_v[edi+ebx]
        fdiv    st,st(1)
        fst     ldv

                ;lv = ldv  *(y  - sp[ia].y) + sp[ia].v;
        fmul    st,st(2)
        fadd    sp_v[edi+ebx]
      fist txt_y
        fstp    lv

                ;ldl = (sp[lb].l - sp[ia].l)/(sp[lb].y - sp[ia].y);
        fld     sp_l[edi+esi]
        fsub    sp_l[edi+ebx]
        fdiv    st,st(1)
        fst     ldl

                ;ll = ldl  *(y  - sp[ia].y) + sp[ia].l;
        fmul    st,st(2)
        fadd    sp_l[edi+ebx]
      fist txt_l
        fstp    ll

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
      fist txt_x
        fstp    lu

        fld     lv
        fadd    ldv
      fist txt_y
        fstp    lv

        fld     ll
        fadd    ldl
      fist txt_l
        fstp    ll

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
        add     esi,gouraudpsize           ;rb++
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

                ;rdl = (sp[rb].l - sp[ia].l)/(sp[rb].y - sp[ia].y);
        fld     sp_l[edi+esi]
        fsub    sp_l[edi+ebx]
        fdiv    st,st(1)
        fst     rdl

                ;rl = rdl  *(y  - sp[ia].y) + sp[ia].l;
        fmul    st,st(2)
        fadd    sp_l[edi+ebx]
        fstp    rl

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

        fld     rl
        fadd    rdl
        fstp    rl

        fld     rx
        fadd    rdx                     ;rx bleibt im copro
        fst     rx
@@r_z:

        fistp   xe                      ;xe = ceil(rx) (rx entfernen)
        fistp   xa                      ;xa = ceil(lx) (lx entfernen)

;eax = WwiU
;ebx = x0Yy
;ecx = Ll0X
;edx = TT??
;esi = CCCC
;edi = PPPP
;ebp = u0Vv

        mov     edi,xa
        mov     esi,xe
        sub     esi,edi                 ;esi = CCCC
        jle     @@w
        add     edi,x_y                 ;edi = PPPP

        movzx   ebx,word ptr txt_x      ;ebx = 00Xx
        movzx   ecx,word ptr txt_l      ;ecx = 00Ll
        add     bh,80h
        shl     ecx,16                  ;ecx = Ll00
        mov     cl,bh                   ;ecx = Ll0X
        shl     ebx,24                  ;ebx = x000
        mov     bx,word ptr txt_y       ;ebx = x0Yy
        add     bh,80h


        mov     eax,scene.sc_divtab
        fld     ru
        fsub    lu
        fmul    dword ptr[eax+esi*4]
        fistp   txt_x
        fld     rv
        fsub    lv
        fmul    dword ptr[eax+esi*4]
        fistp   txt_y
        fld     rl
        fsub    ll
        fmul    dword ptr[eax+esi*4]
        fistp   txt_l

        movzx   eax,word ptr txt_l      ;eax = 00Ww
        shl     eax,16                  ;eax = Ww00
        movzx   edx,word ptr txt_x      ;edx = 00Uu
        mov     ah,byte ptr idx         ;eax = Wwi0
        mov     al,dh                   ;eax = WwiU
        shl     edx,24                  ;edx = u000
        mov     dx,word ptr txt_y       ;edx = u0Vv

        push    ebp                     ;ebp : lokale variablen
        mov     ebp,edx                 ;ebp = u0Vv
        mov     edx,scene.sc_lmap       ;edx = TT??


        jmp     @@inner
@@i0:
        add     ebx,ebp
        adc     ecx,eax
        inc     edi
        dec     esi
        jz      @@iw
@@inner:
        cmp     ah,[edi*4+3]            ;schreiberlaubnis
        jne     @@i0

        mov     dl,cl
        mov     dh,bh
        bswap   ecx
        mov     dh,[edx]                ;dl = helligkeit
        mov     dl,cl
        add     ebx,ebp
        bswap   ecx
        mov     dl,[edx+65536]          ;scene.lmap + 65536 -> gouraud table
        adc     ecx,eax
        add     [edi*4+1],dl
        inc     edi
        dec     esi
        jnz     @@inner
@@iw:

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
glpolygon endp


sublpolygon proc pascal ;licht-polygon
arg     _sp, sp_end, zval;mapptr, zval : dword
        ;_sp = *sp, zeiger auf screenpoints

local   pend, y, x_y, lb, lc, rb, rc : dword ;int
local   lx, ldx, rx, rdx : dword ;float
local   xa, xe, txt_x, txt_y : dword ;int
local   la:dword:5, lu:dword:5, ra:dword:5, ru:dword:5, a:dword:5, u:dword:5


        xor     eax,eax                 ;start- und endpunkt bestimmen
        mov     lb,eax
        mov     rb,eax
        mov     pend,eax

        mov     esi,_sp                 ;esi -> screenpoints
        fld     sp_y[esi]               ;st(0) = ymax
        fld     st                      ;st(1) = ymin

        mov      ecx,sp_end
        sub      ecx,lightpsize  ;esi+ecx -> lp[sp_end-1]
@@max_l:

        fld     sp_y[esi+ecx]
        fcom    st(1);ymax              ;gr”áten y-wert finden (endpunkt)
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

        sub     ecx,lightpsize
        jnz     @@max_l
        fstp    st                      ;ymax entfernen

        fistp   y                       ;y = ceil(ymin)

        mov     eax,y

        cmp     eax,yres                ;y-clipping
        jge     @@weg

        or      eax,eax
        jg      @@y0
        mov     y,0
        xor     eax,eax
@@y0:
        imul    eax,xres
        mov     edx,scene.sc_buffer
        shr     edx,2
        add     eax,edx
        mov     x_y,eax

        mov     lc,1
        mov     rc,1

        fld     c_32;c_16                    ;st(0) = 16

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
        sub     esi,lightpsize    ;lb--
        jnc     @@l0                    ;wrap
        add     esi,sp_end
@@l0:
        fld     sp_y[esi+edi]           ;lc = ceil(lp[lb].sy) - y
        fistp   lc
        mov     eax,y
        sub     lc,eax
        jle     @@lc                    ;while lc <= 0
        mov     lb,esi

        fld     sp_z[esi+edi]           ;lp[lb].z einlagern

        ;la
        fld     sp_z[ebx+edi]           ;la.z = lp[ia].z;
        fst     la.ls_z

        fld     sp_y[ebx+edi]           ;la.y = lp[ia].sy*lp[ia].z;
        fmul    st,st(1)
        fst     la.ls_p;la.y

        fld     sp_lx[ebx+edi]          ;la.lx = lp[ia].lx;
        fst     la.ls_lx

        fld     sp_ly[ebx+edi]          ;la.ly = lp[ia].ly;
        fst     la.ls_ly

        fld     sp_lz[ebx+edi]          ;la.lz = lp[ia].lz;
        fst     la.ls_lz

        ;lu
        fsubr   sp_lz[esi+edi]          ;lu.lz = lp[lb].lz - lp[ia].lz;
        fstp    lu.ls_lz

        fsubr   sp_ly[esi+edi]          ;lu.ly = lp[lb].ly - lp[ia].ly;
        fstp    lu.ls_ly

        fsubr   sp_lx[esi+edi]          ;lu.lx = lp[lb].lx - lp[ia].lx;
        fstp    lu.ls_lx

        fld     sp_y[esi+edi]           ;lu.y = lp[lb].y*lp[lb].z - la.y;
        fmul    st,st(3);lp[lb].z
        fsubrp  st(1),st
        fstp    lu.ls_p;lu.y
        fsubp   st(1),st                ;lu.z = lp[lb].z - lp[ia].z;
        fstp    lu.ls_z


                ;ldx = (lp[lb].x - lp[ia].x)/(lp[lb].y - lp[ia].y);
        fld     sp_x[esi+edi]
        fsub    sp_x[ebx+edi]
        fld     sp_y[esi+edi]
        fsub    sp_y[ebx+edi]
        fdivp   st(1),st
        fst     ldx

                ;lx = ldx   *(y  - lp[ia].y) + lp[ia].x;
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
        add     esi,lightpsize   ;rb++
        cmp     esi,sp_end
        jb      @@r0                    ;wrap
        xor     esi,esi
@@r0:
        fld     sp_y[esi+edi]           ;rc = ceil(lp[rb].sy) - y
        fistp   rc
        mov     eax,y
        sub     rc,eax
        jle     @@rc                    ;while rc <= 0
        mov     rb,esi

        fld     sp_z[esi+edi]           ;lp[lb].z einlagern

        ;ra
        fld     sp_z[ebx+edi]           ;ra.z = lp[ia].z;
        fst     ra.ls_z

        fld     sp_y[ebx+edi]           ;ra.y = lp[ia].sy*lp[ia].z;
        fmul    st,st(1)
        fst     ra.ls_p;ra.y

        fld     sp_lx[ebx+edi]          ;ra.lx = lp[ia].lx;
        fst     ra.ls_lx

        fld     sp_ly[ebx+edi]          ;ra.ly = lp[ia].ly;
        fst     ra.ls_ly

        fld     sp_lz[ebx+edi]          ;ra.lz = lp[ia].lz;
        fst     ra.ls_lz

        ;ru
        fsubr   sp_lz[esi+edi]          ;ru.lz = lp[rb].lz - lp[ia].lz;
        fstp    ru.ls_lz

        fsubr   sp_ly[esi+edi]          ;ru.ly = lp[rb].ly - lp[ia].ly;
        fstp    ru.ls_ly

        fsubr   sp_lx[esi+edi]          ;ru.lx = lp[rb].lx - lp[ia].lx;
        fstp    ru.ls_lx

        fld     sp_y[esi+edi]           ;ru.y = lp[rb].sy*lp[rb].z - ra.y;
        fmul    st,st(3);lp[lb].z
        fsubrp  st(1),st
        fstp    ru.ls_p;ru.y

        fsubp   st(1),st                ;ru.z = lp[rb].z - lp[ia].z;
        fstp    ru.ls_z


                ;rdx = (lp[rb].x - lp[ia].x)/(lp[rb].y - lp[ia].y);
        fld     sp_x[esi+edi]
        fsub    sp_x[ebx+edi]
        fld     sp_y[esi+edi]
        fsub    sp_y[ebx+edi]
        fdivp   st(1),st
        fst     rdx

                ;rx = rdx   *(y  - lp[ia].y) + lp[ia].x;
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
        fild    y                       ;opti
        fmul    la.ls_z
        fsub    la.ls_p;-la.y
        fild    y
        fmul    lu.ls_z
        fsubr   lu.ls_p;lu.y-
        fdivp   st(1),st                ;st = r

        fld     lu.ls_z                 ;a.z = la.z + r*lu.z;
        fmul    st,st(1);*r
        fadd    la.ls_z
        fst     a.ls_z
        fmul    st,st(3);*lx            ;a.x = a.z*lx;
        fstp    a.ls_p;a.x

        fld     lu.ls_lz
        fmul    st,st(1);*r             ;a.lz = la.lz + r*lu.lz;
        fadd    la.ls_lz
        fst     a.ls_lz

        fld     lu.ls_lx                ;a.lx = la.lx + r*lu.lx;
        fmul    st,st(2);*r
        fadd    la.ls_lx
        fst     a.ls_lx

        fld     lu.ls_ly                ;a.ly = la.ly + r*lu.ly;
        fmul    st,st(3);*r
        fadd    la.ls_ly
        fst     a.ls_ly

        fdiv    st,st(2)                   ;a.ly/a.lz
        fistp   txt_y
        fdivrp  st(1),st                   ;a.lx/a.lz
        fistp   txt_x

        fstp    st                         ;r entfernen
                ;rechts: r = (y *ra.z - ra.y) / (ru.y - ru.z*y );
        fild    y                       ;opti
        fmul    ra.ls_z
        fsub    ra.ls_p;ra.y
        fild    y
        fmul    ru.ls_z
        fsubr   ru.ls_p;ru.y-
        fdivp   st(1),st                ;st = r

        fld     ru.ls_z                 ;u.z = ra.z + r*ru.z;
        fmul    st,st(1)
        fadd    ra.ls_z

        fld     st                      ;u.x = u.z*rx - a.x;
        fmul    st,st(3);*rx
        fsub    a.ls_p;-a.x
        fstp    u.ls_p;u.x

        fsub    a.ls_z                  ;u.z -= a.z;
        fstp    u.ls_z

        fld     ru.ls_lx                ;u.lx = ra.lx + r*ru.lx - a.lx;
        fmul    st,st(1);*r
        fadd    ra.ls_lx
        fsub    a.ls_lx
        fstp    u.ls_lx

        fld     ru.ls_ly                ;u.ly = ra.ly + r*ru.ly - a.ly;
        fmul    st,st(1);*r
        fadd    ra.ls_ly
        fsub    a.ls_ly
        fstp    u.ls_ly

        fmul    ru.ls_lz;*r             ;u.lz = ra.lz + r*ru.lz - a.lz;
        fadd    ra.ls_lz
        fsub    a.ls_lz
        fstp    u.ls_lz

        fistp   xe                      ;xe = ceil(rx) (rx entfernen)
        frndint
        fist    xa                      ;xa = ceil(lx) (lx nicht entfernen)


        mov     edi,xa
        mov     ecx,xe
        sub     ecx,edi                 ;ecx = pixel in einer zeile
        add     edi,x_y                 ;edi -> pixel

@@l:
        mov     edx,txt_x               ;edx = 00Xx
        mov     ebx,txt_y               ;ebx = 00Yy

        cmp     ecx,48;24                 ;16
        jge     @@div

                ;weniger als 24 pixel
        or      ecx,ecx
        jle     @@w

        mov     esi,ecx                 ;esi : zu setzende pixel
        xor     ecx,ecx
        push    ecx

        fld     a.ls_lz
        fadd    u.ls_lz

        fld     a.ls_lx                 ;textur - endkoordinate
        fadd    u.ls_lx                 ;(a.lx + 1*u.lx)/(a.lz + 1*u.lz)
        fdiv    st,st(1)
        fisub   txt_x
        fmul    divtab32[esi*4]           ;16
        fistp   txt_x

        fld     a.ls_ly                 ;textur - endkoordinate
        fadd    u.ls_ly                 ;(a.ly + 1*u.ly)/(a.lz + 1*u.lz)
        fdivrp  st(1),st
        fisub   txt_y
        fmul    divtab32[esi*4]
        fistp   txt_y

        mov     eax,txt_x
        mov     ecx,txt_y

        jmp     @@5
                                        ;st(0) = x
                                        ;st(1) = 16
@@div:          ;scanline subdivision

        fadd    st,st(1);16             ;x um 16 weiter
        fld     st;x                    ;r = (x *a.z - a.x) / (u.x - u.z*x )
        fmul    a.s_z
        fsub    a.s_p;a.x
        fld     st(1);x
        fmul    u.s_z
        fsubr   u.s_p;u.x
        fdivp   st(1),st                ;st = r

        fld     u.ls_lz                 ;(a.lz + r*u.lz)
        fmul    st,st(1)
        fadd    a.ls_lz

        fld     u.ls_lx                 ;x = (a.ly + r*u.ly)/(a.lz + r*u.lz)
        fmul    st,st(2)
        fadd    a.ls_lx
        fdiv    st,st(1)
        fistp   txt_x

        fld     u.ls_ly                 ;y = (a.ly + r*u.ly)/(a.lz + r*u.lz)
        fmul    st,st(2)
        fadd    a.ls_ly
        fdivrp  st(1),st
        fistp   txt_y

        fstp    st                      ;r entfernen

        mov     esi,32;16                  ;esi : zu setzende pixel
        sub     ecx,esi
        push    ecx                     ;ecx : x-z„hler

        mov     eax,txt_x
        mov     ecx,txt_y
        sub     eax,edx                 ;eax = 00Uu
        sub     ecx,ebx                 ;ecx = 00Vv

@@5:
        push    ebp                     ;ebp : lokale variablen

        add     dh,80h
        add     bh,80h

        or      esi,zval
        push    esi
        mov     esi,scene.sc_lmap;mapptr              ;esi = TTTT

        shl     ebx,16
        shl     ecx,16 - 5 ;v/16
        shr     eax,5 ;u/16
        shr     ebx,8
        shr     ecx,8
        shrd    ebx,edx,8               ;ebx = x0Yy
        shrd    ecx,eax,8               ;ecx = u0Vv
        movzx   edx,dh                  ;edx = 000X
        movzx   ebp,ah                  ;ebp = 000U
                ;keine lokalen variablen mehr zug„nglich
        pop     eax                     ;eax = 00zC

        mov     dh,bh
     xchg ebp,ecx
        jmp     @@inner
@@i0:
        add     ebx,ebp;ecx
        adc     dl,cl;adc     edx,ebp
        inc     edi
        dec     al
        mov     dh,bh
        jz      @@iw
@@inner:
        cmp     ah,[edi*4+3]
        jne     @@i0
        mov     dh,[edx+esi];15
        add     ebx,ebp;ecx
        adc     dl,cl;adc     edx,ebp
        add     [edi*4+1],dh   ;+2
        inc     edi
        dec     al
        mov     dh,bh
        jnz     @@inner
@@iw:

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
        fstp    st                      ;16.0 entfernen

@@weg:  ret
sublpolygon endp

scubezclip proc pascal
        ;-> ebx -> sp     (source points of type tcpoint)
        ;-> eax -> sp_end
        ;-> esi -> ttempcube
        ;-> edi -> dp     (dest points of type tscreenpoint)

        ;<- ebx -> dp
        ;<- esi -> ttempcube
        ;<- edi -> dp_end
        ;<- flags: if less than 3 points, jump with jbe
local   _sp, _sp_end, _dp

        mov     _sp,ebx
        mov     _sp_end,eax
        mov     _dp,edi

        fld1
@@z_l:
                                        ;inn = true : point in front of plane
        mov     edx,[ebx].cp_p
        lea     edx,[esi].tc_proj[edx]  ;edx -> tprojected (this point)
                ;if (inn)
        test    [edx].pr_inn,1          ;inn = (z >= 1.0)
        jz      @@0

                ;point in front of projection plane (visible)
        fld     [edx].pr_sx
        fadd    st,st(1)
        fmul    smid
        fstp    [edi].sp_x              ;x = sres/2 *(1 + x/z)

        fld     [edx].pr_sy
        fadd    st,st(1) ;subr
        fmul    smid
        fstp    [edi].sp_y              ;y = sres/2 *(1 + y/z)

;        fld     [edx].pr_z
;        fstp    [edi].sp_z

        add     edi,shadowpsize         ;one destination point added
                ;end of if statement
@@0:
        mov     eax,ebx                 ;ebx -> actual source point
        add     eax,size tcpoint        ;eax -> next source point
        cmp     eax,_sp_end             ;wrap if at last point
        jb      @@wrap
        mov     eax,_sp
@@wrap:

        mov     ecx,[eax].cp_p
        lea     ecx,[esi].tc_proj[ecx]  ;ecx -> tprojected (next point)
                ;if (inn ^ (nz >= 1.0))
        push    eax
        mov     al,[edx].pr_inn
        xor     al,[ecx].pr_inn
        pop     eax
        jz      @@1

                ;this or next point behind projection plane
                ;r = (1.0-z)/(nz-z)
        fld1
;        fst     [edi].sp_z              ;z = 1.0
        fsub    [edx].pr_z
        fld     [ecx].pr_z
        fsub    [edx].pr_z
        fdivp   st(1),st                ;st = r

        fld     [edx].pr_x              ;x' = x + r*(nx-x)
        fld     [ecx].pr_x
        fsub    st,st(1)
        fmul    st,st(2)
        faddp   st(1),st
        fadd    st,st(2)                ;x = sres/2 *(1 + x')
        fmul    smid
        fstp    sp_x[edi]

        fld     [edx].pr_y              ;y' = y + r*(ny-y)
        fld     [ecx].pr_y
        fsub    st,st(1)
        fmul    st,st(2)
        faddp   st(1),st
        fadd    st,st(2) ;subr          ;y = sres/2 *(1 - y')
        fmul    smid
        fstp    sp_y[edi]

        fstp    st                      ;remove r

        add     edi,shadowpsize         ;one destination point added
                ;end of if statement
@@1:
        cmp     eax,ebx                 ;ns > s?
        mov     ebx,eax                 ;is same as ebx += point size
        ja      @@z_l                   ;repeat loop

        fstp    st                      ;remove 1.0

        mov     ebx,_dp
        mov     eax,edi
        sub     eax,ebx
        cmp     eax,shadowpsize*2       ;less than 3 points ?

@@weg:  ret
scubezclip endp

sprojection proc pascal
        ;-> ebx = *sp
        ;-> edi = *sp_end
        ;<- ebx = *sp
        ;<- edi = *sp_end
        ;<- flags: if less than 3 points, jump with jbe
local   _sp:dword, _sp_end:dword ;both are pointers

                                        ;x, y, z: components of source point
                                        ;d : prefix for destination points
                                        ;n : prefix for next sorce point

        mov     _sp,ebx                 ;ebx -> source points
        mov     _sp_end,edi             ;edi -> destination points

        fld1
@@z_l:
                                        ;inn = true : point in front of plane
        fld     sp_z[ebx]               ;inn = (z >= 1)
        fcomp   st(1)

        fstsw   ax
        mov     dh,ah                   ;dh = inn
                ;if (inn)
        sahf
        jb      @@0
                ;point in front of projection plane (visible)
        fld     sp_z[ebx]
        fst     sp_z[edi]

        fld     sp_x[ebx]               ;x = xres/2 *(1 + x/z)
        fdiv    st,st(1)
        fadd    st,st(2)
        fmul    smid
        fstp    sp_x[edi]

        fld     sp_y[ebx]               ;y = yres/2 *(1 + y/z)
        fdivrp  st(1),st
        fadd    st,st(1);subr
        fmul    smid
        fstp    sp_y[edi]

        add     edi,shadowpsize         ;one destination point added
                ;end of if statement
@@0:
        mov     esi,ebx                 ;ebx -> actual source point
        add     esi,shadowpsize         ;esi -> next source point
        cmp     esi,_sp_end             ;wrap if at last point
        jb      @@wrap
        mov     esi,_sp
@@wrap:
                ;if (inn ^ (nz >= 1))
        fld     sp_z[esi]
        fcomp   st(1)
        fstsw   ax
        xor     ah,dh                   ;dh = inn
        sahf
        jnc     @@1
                ;this or next point behind projection plane
                ;r = (1.0-z)/(nz-z)
        fld1
        fst     sp_z[edi]               ;dz = 1.0
        fsub    sp_z[ebx]
        fld     sp_z[esi]
        fsub    sp_z[ebx]
        fdivp   st(1),st                ;st = r

        fld     sp_x[ebx]               ;x' = x + r*(nx-x)
        fld     sp_x[esi]
        fsub    st,st(1)
        fmul    st,st(2)
        faddp   st(1),st
        fadd    st,st(2)                ;x = xres/2 *(1 + x')
        fmul    xmid
        fstp    sp_x[edi]

        fld     sp_y[ebx]               ;y' = y + r*(ny-y)
        fld     sp_y[esi]
        fsub    st,st(1)
        fmulp   st(2),st                ;remove r
        faddp   st(1),st
        fadd    st,st(1) ;subr          ;y = yres/2 *(1 + y')
        fmul    ymid
        fstp    sp_y[edi]

        add     edi,shadowpsize         ;one destination point added
                ;end of if statement
@@1:
        cmp     esi,ebx                 ;ns > s?
        mov     ebx,esi                 ;is same as ebx += point size
        ja      @@z_l                   ;repeat loop

        mov     ebx,_sp_end             ;ebx -> start of points array
                                        ;edi -> end of points array

        fstp    st                      ;remove 1.0

        mov     eax,edi
        sub     eax,ebx
        cmp     eax,shadowpsize*2       ;less than 3 points ?

@@weg:  ret
sprojection endp

comment #
sxclip proc pascal ;x-clipping
        ;-> ebx = *sp
        ;-> edi = *sp_end
        ;<- ebx = *sp
        ;<- edi = *sp_end
        ;<- wenn weniger als 3 punkte, mit jbe wegspringen
local   _sp:dword, _sp_end:dword ;beides pointer
local   i:byte

        fldz                            ;st = x
        mov     i,1
@@i_l:
        mov     _sp,ebx                 ;ebx -> sp[z] (quellpunkte) (z = 0)
        mov     _sp_end,edi             ;edi -> sp[d] (zielpunkte)  (d = m)

        xor     dl,dl                   ;dl = flag
@@z_l:

        fld     sp_x[ebx]               ;inn = (sp[z].sx >= x);
        fcomp   st(1)
        fstsw   ax
        mov     dh,ah                   ;dh = inn
        xor     ah,i
        sahf
        jnc     @@0
                ;punkt innerhalb
        mov     esi,ebx
        mov     ecx,shadowpsize
        shr     ecx,2
        rep     movsd                   ;sp[d] = sp[z] und d++
@@0:
        mov     esi,ebx
        add     esi,shadowpsize         ;nz = z+1
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
                ;dieser oder n„chster punkt auáerhalb
        inc     dl                      ;flag = 1

        fst     sp_x[edi]              ;sp[d].sx speichern

                ;r berechnen
        fld     st;x                    ;(x - sp[z].sx)
        fsub    sp_x[ebx]
        fld     sp_x[esi]              ;/(sp[nz].sx - sp[z].sx)
        fsub    sp_x[ebx]
        fdivp   st(1),st                ;st = r


        fld     sp_y[ebx]              ;sp[d].sy berechnen
        fld     sp_y[esi]              ; sp[z].sy + r*(sp[nz].sy - sp[z].sy)
        fsub    st,st(1)
        fmulp   st(2),st               ;r entfernen
        faddp   st(1),st
        fstp    sp_y[edi]


        add     edi,shadowpsize         ;d++
@@1:
        cmp     esi,ebx                 ;wz > z?
        mov     ebx,esi                 ;entspricht ebx += shadowpsize
        ja      @@z_l                   ;z-schleife

                                        ;ebx -> anfang der z-punkte (sp)
        mov     esi,_sp_end             ;esi -> anfang der d-punkte (sp_end)
                                        ;edi -> ende der d-punkte
        mov     ecx,edi
        sub     ecx,esi                 ;ecx = anzahl zu kopierender byte
        jz      @@w                     ; kein punkt brig (ZF = 1)

        mov     edi,esi                 ;wenn flag = 0: nicht kopieren,
        or      dl,dl                   ; edi = _sp_end zurckgeben
        jz      @@2

        mov     edi,ebx                 ;edi -> anfang der z-punkte
        shr     ecx,2                   ;ecx = anzahl zu kopierender dwords
        rep     movsd
@@2:

        fadd    sfres                   ;x += sres
        dec     i                       ;i-schleife
        jns     @@i_l

        mov     eax,edi
        sub     eax,ebx
        cmp     eax,shadowpsize*2
@@w:    fstp    st                      ;x entfernen
@@weg:  ret
sxclip endp
#

spolygon proc pascal
arg     _sp, sp_end
        ;_sp = *sp, zeiger auf screenpoints

local   pend, y, ymax, x_y, lb, lc, rb, rc ;int
local   lx, ldx, rx, rdx ;float
local   xa, xe ;int


        xor     eax,eax                 ;start- und endpunkt bestimmen
        mov     lb,eax
        mov     rb,eax
        mov     pend,eax

        mov     esi,_sp                 ;esi -> screenpoints
        fld     sp_y[esi]               ;st(0) = ymax
        fld     st                      ;st(1) = ymin

        mov     ecx,sp_end
        sub     ecx,shadowpsize         ;esi+ecx -> sp[sp_end-1]
@@max_l:

        fld     sp_y[esi+ecx]
        fcom    st(1);ymax              ;gr”áten y-wert finden (endpunkt)
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

        sub     ecx,shadowpsize
        jnz     @@max_l
        fistp   ymax                    ;ymax entfernen

        fistp   y                       ;y = ceil(ymin)

        mov     eax,y

        cmp     eax,sres                ;y-clipping
        jge     @@weg

        or      eax,eax
        jg      @@y0
        xor     eax,eax
        mov     y,eax
@@y0:
        cmp     eax,ymax
        jge     @@weg

;        mov     ebx,_mapptr             ;lmap kopieren
;        mov     esi,[ebx]
;        cmp     esi,scene.sc_lmap
;        je      @@lmap
;        mov     edi,scene.sc_lmap
;        mov     [ebx],edi
;        mov     ecx,65536/4
;        rep     movsd
;        mov     esi,[ebx];lmap
;@@lmap:


        shl     eax,8
        add     eax,scene.sc_lmap;esi;mapptr
        mov     x_y,eax

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
        sub     esi,shadowpsize         ;lb--
        jnc     @@l0                    ;wrap
        add     esi,sp_end
@@l0:
        fld     sp_y[esi+edi]           ;lc = ceil(sp[lb].sy) - y
        fistp   lc
        mov     eax,y
        sub     lc,eax
        jle     @@lc                    ;while lc <= 0
        mov     lb,esi


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

        fistp   xa                      ;xa = ceil(lx) (lx entfernen)

                ;rechts
        dec     rc
        jnz     @@r_nz
        mov     edi,_sp
        mov     esi,rb
@@rc:

        cmp     esi,pend
        je      @@fertig                ;rb == pend -> unten angekommen
        mov     ebx,esi                 ;ia = rb
        add     esi,shadowpsize         ;rb++
        cmp     esi,sp_end
        jb      @@r0                    ;wrap
        xor     esi,esi
@@r0:
        fld     sp_y[esi+edi]           ;rc = ceil(sp[rb].sy) - y
        fistp   rc
        mov     eax,y
        sub     rc,eax
        jle     @@rc                    ;while rc <= 0
        mov     rb,esi


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
        fistp   xe                      ;xe = ceil(rx) (rx entfernen)

        mov     edi,xa
      ;dec edi

        or      edi,edi
        jns     @@c0
        xor     edi,edi
@@c0:
        mov     ecx,xe ;<
      ;inc ecx
        cmp     ecx,sres
        jl      @@c1
        mov     ecx,sres
@@c1:

        sub     ecx,edi                 ;ecx = pixel in einer zeile
        jle     @@w
        add     edi,x_y                 ;edi -> pixel

        xor     eax,eax
;     cmp ecx,10
;     jb  @@o5
;     mov edx,ecx
;     mov ecx,edi
;     neg ecx
;     and ecx,3
;     sub edx,ecx
;     rep stosb

;     mov ecx,edx
;     shr ecx,2
;     rep stosd
;     mov ecx,edx
;     and ecx,3

;@@o5:
        rep     stosb

@@w:
        mov     eax,sres
        add     x_y,eax
        inc     y
        cmp     eax,y                   ;while (y < sres)
        jg      @@y_l

@@fertig:

@@weg:  ret
spolygon endp

insert proc near
        ;-> edx = idx
        ;-> st(0) = z-value


        lea     edi,scene
        mov     ebx,[edi].sc_idxlistend

@@s_l:          ;search insert position
        cmp     ebx,[edi].sc_idxlist
        jbe     @@0
        sub     ebx,size tidxentry

        fcom    [ebx].i_zval            ;highest zval first
        fstsw   ax
        sahf
        ja      @@s_l
        add     ebx,size tidxentry
@@0:

        fld     st
@@m_l:          ;move entries
        cmp     ebx,[edi].sc_idxlistend
        jae     @@1
        fld     [ebx].i_zval
        fxch    st(1)
        fstp    [ebx].i_zval
        xchg    edx,[ebx].i_idx

        add     ebx,size tidxentry
        jmp     @@m_l

@@1:
        fstp    [ebx].i_zval
        mov     [ebx].i_idx,edx
        add     [edi].sc_idxlistend, size tidxentry

@@weg:  ret
insert endp

;zclip and project face for rendering onto z-line
zprojectline proc pascal
;arg     spsize:dword, vars_1:dword ;data variables -1
        ;-> ebx = *sp
        ;-> edi = *sp_end
        ;<- ebx = *sp
        ;<- edi = *sp_end
        ;<- flags: if less than 3 points, jump with jbe
local   _sp:dword, _sp_end:dword ;both are pointers

                                        ;x, y, z: components of source point
                                        ;d : prefix for destination points
                                        ;n : prefix for next sorce point

        mov     _sp,ebx                 ;ebx -> source points
        mov     _sp_end,edi             ;edi -> destination points

        fld1
@@z_l:
                                        ;inn = true : point in front of plane
        fld     sp_z[ebx]               ;inn = (z >= 1)
        fcomp   st(1)

        fstsw   ax
        mov     dh,ah                   ;dh = inn
                ;if (inn)
        sahf
        jb      @@0
                ;point in front of projection plane (visible)
        fld     sp_z[ebx]
        fst     sp_z[edi]

        fld     sp_x[ebx]               ;x = (x/z + 1) *xmul
        fdiv    st,st(1)
        fadd    st,st(2)
        fmul    z_xmul
        fstp    sp_x[edi]

        fld     sp_y[ebx]               ;y = (y/z + a) *ymul
        fdivrp  st(1),st
        fadd    z_a
        fmul    z_ymul
        fstp    sp_y[edi]

        add     edi,zlinepsize          ;one destination point added
                ;end of if statement
@@0:
        mov     esi,ebx                 ;ebx -> actual source point
        add     esi,zlinepsize          ;esi -> next source point
        cmp     esi,_sp_end             ;wrap if at last point
        jb      @@wrap
        mov     esi,_sp
@@wrap:
                ;if (inn ^ (nz >= 1))
        fld     sp_z[esi]
        fcomp   st(1)
        fstsw   ax
        xor     ah,dh                   ;dh = inn
        sahf
        jnc     @@1
                ;this or next point behind projection plane
                ;r = (1.0-z)/(nz-z)
        fld1
        fst     sp_z[edi]               ;dz = 1.0
        fsub    sp_z[ebx]
        fld     sp_z[esi]
        fsub    sp_z[ebx]
        fdivp   st(1),st                ;st = r

        fld     sp_x[ebx]               ;x' = x + r*(nx-x)
        fld     sp_x[esi]
        fsub    st,st(1)
        fmul    st,st(2)
        faddp   st(1),st
        fadd    st,st(2)                ;x = (x' + 1) *xmul
        fmul    z_xmul
        fstp    sp_x[edi]

        fld     sp_y[ebx]               ;y' = y + r*(ny-y)
        fld     sp_y[esi]
        fsub    st,st(1)
        fmulp   st(2),st                ;remove r
        faddp   st(1),st
        fadd    z_a                     ;y = (y' + a) *ymul
        fmul    z_ymul
        fstp    sp_y[edi]


        add     edi,zlinepsize          ;one destination point added
                ;end of if statement
@@1:
        cmp     esi,ebx                 ;ns > s?
        mov     ebx,esi                 ;is same as ebx += point size
        ja      @@z_l                   ;repeat loop

        mov     ebx,_sp_end
                                        ;ebx -> start of s-points
        fstp    st                      ;remove 1.0

        mov     eax,edi
        sub     eax,ebx
;        shr     eax,1
        cmp     eax,zlinepsize*2;spsize

@@weg:  ret
zprojectline endp

;project face for rendering onto z-line
projectline proc pascal
arg     spsize
        ;-> ebx -> sp
        ;-> edi -> sp_end

        mov     edx,edi
@@l:
        fld     sp_lz[ebx]
        fld     sp_lx[ebx]              ;x = (x/z + 1) *xmul
        fdiv    st,st(1)
        fld1
        faddp   st(1),st
        fmul    z_xmul
        fstp    sp_x[edi]

        fld     sp_ly[ebx]              ;y = (y/z + a) *ymul
        fdiv    st,st(1)
        fadd    z_a
        fmul    z_ymul
        fstp    sp_y[edi]
        fstp    sp_z[edi]

        add     ebx,spsize;lightpsize
        add     edi,zlinepsize
        cmp     ebx,edx
        jb      @@l

@@weg:  ret
projectline endp

comment #
gprojectline proc near
        ;-> ebx -> sp
        ;-> edi -> sp_end

        mov     edx,edi
@@l:
        fld     sp_lz[ebx]
        fld     sp_lx[ebx]
        fld1
        faddp   st(1),st
        fdiv    st,st(1)
        fmul    z_xmul
        fstp    sp_x[edi]
        fld     sp_y[ebx]
        fadd    z_a
        fdiv    st,st(1)
        fmul    z_ymul
        fstp    sp_y[edi]
        fstp    sp_z[edi]

        add     ebx,glightpsize
        add     edi,zlinepsize
        cmp     ebx,edx
        jb      @@l

@@weg:  ret
gprojectline endp

#

renderline proc pascal
arg     _sp, sp_end
        ;_sp = *sp, zeiger auf screenpoints

local   pend, y, ymax, x_y, lb, lc, rb, rc ;int
local   lx, ldx, rx, rdx, lz, ldz, rz, rdz ;float
local   xa, xe ;int


        xor     eax,eax                 ;start- und endpunkt bestimmen
        mov     lb,eax
        mov     rb,eax
        mov     pend,eax

        mov     esi,_sp                 ;esi -> screenpoints
        fld     sp_y[esi]               ;st(0) = ymax
        fld     st                      ;st(1) = ymin

        mov     ecx,sp_end
        sub     ecx,zlinepsize          ;esi+ecx -> sp[sp_end-1]
@@max_l:

        fld     sp_y[esi+ecx]
        fcom    st(1);ymax              ;gr”áten y-wert finden (endpunkt)
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

        sub     ecx,zlinepsize
        jnz     @@max_l
        fistp   ymax                    ;ymax entfernen

        fistp   y                       ;y = ceil(ymin)

        mov     eax,y

        cmp     eax,zyres               ;y-clipping
        jge     @@weg

        or      eax,eax
        jg      @@y0
        xor     eax,eax
        mov     y,eax
@@y0:
        cmp     eax,ymax
        jge     @@weg

        shl     eax,zxshift+2;6+2;*64*4
        add     eax,scene.sc_zline
        mov     x_y,eax

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
        add     esi,zlinepsize          ;rb++
        cmp     esi,sp_end
        jb      @@l0                    ;wrap
        xor     esi,esi
@@l0:
        fld     sp_y[edi+esi]           ;lc = ceil(sp[lb].sy) - y
        fistp   lc
        mov     eax,y
        sub     lc,eax
        jle     @@lc                    ;while lc <= 0
        mov     lb,esi

        fild    y
        fsub    sp_y[edi+ebx]           ;(y  - sp[ia].y)

        fld     sp_y[edi+esi]           ;(sp[lb].y - sp[ia].y)
        fsub    sp_y[edi+ebx]

                ;ldz = (sp[lb].u - sp[ia].u)/(sp[lb].y - sp[ia].y);
        fld     sp_z[edi+esi]
        fsub    sp_z[edi+ebx]
        fdiv    st,st(1)
        fst     ldz

                ;lz = ldz  *(y  - sp[ia].y) + sp[ia].z;
        fmul    st,st(2)
        fadd    sp_z[edi+ebx]
        fstp    lz

                ;ldx = (sp[lb].x - sp[ia].x)/(sp[lb].y - sp[ia].y);
        fld     sp_x[edi+esi]
        fsub    sp_x[edi+ebx]
        fdivrp  st(1),st
        fst     ldx

                ;lx = ldx  *(y  - sp[ia].y) + sp[ia].x;
        fmulp   st(1),st
        fadd    sp_x[edi+ebx]
        fst     lx                      ;lx bleibt im copro

        jmp     @@l_z
@@l_nz:
        fld     lz
        fadd    ldz
        fstp    lz

        fld     lx
        fadd    ldx                     ;lx bleibt im copro
        fst     lx
@@l_z:

        fistp   xa                      ;xa = ceil(lx) (lx entfernen)

                ;rechts
        dec     rc
        jnz     @@r_nz
        mov     edi,_sp
        mov     esi,rb
@@rc:

        cmp     esi,pend
        je      @@fertig                ;rb == pend -> unten angekommen
        mov     ebx,esi                 ;ia = rb
        sub     esi,zlinepsize          ;lb--
        jnc     @@r0                    ;wrap
        add     esi,sp_end
@@r0:
        fld     sp_y[edi+esi]           ;rc = ceil(sp[rb].sy) - y
        fistp   rc
        mov     eax,y
        sub     rc,eax
        jle     @@rc                    ;while rc <= 0
        mov     rb,esi

        fild    y
        fsub    sp_y[edi+ebx]           ;(y  - sp[ia].y)

        fld     sp_y[edi+esi]           ;(sp[rb].y - sp[ia].y)
        fsub    sp_y[edi+ebx]

                ;rdz = (sp[rb].z - sp[ia].z)/(sp[rb].y - sp[ia].y);
        fld     sp_z[edi+esi]
        fsub    sp_z[edi+ebx]
        fdiv    st,st(1)
        fst     rdz

                ;rz = rdz  *(y  - sp[ia].y) + sp[ia].z;
        fmul    st,st(2)
        fadd    sp_z[edi+ebx]
        fstp    rz

                ;rdx = (sp[rb].x - sp[ia].x)/(sp[rb].y - sp[ia].y);
        fld     sp_x[edi+esi]
        fsub    sp_x[edi+ebx]
        fdivrp  st(1),st
        fst     rdx

                ;rx = rdx  *(y  - sp[ia].y) + sp[ia].x;
        fmulp   st(1),st
        fadd    sp_x[edi+ebx]
        fst     rx                      ;rx bleibt im copro

        jmp     @@r_z
@@r_nz:
        fld     rz
        fadd    rdz
        fstp    rz

        fld     rx
        fadd    rdx                     ;rx bleibt im copro
        fst     rx
@@r_z:
        fistp   xe                      ;xe = ceil(rx) (rx entfernen)

        mov     edi,xa
      dec edi
        or      edi,edi
        jns     @@c0
        xor     edi,edi
@@c0:
        mov     ecx,xe
      inc ecx
        cmp     ecx,zxres
        jl      @@c1
        mov     ecx,zxres
@@c1:

        sub     ecx,edi                 ;ecx = pixel in einer zeile
        jle     @@w
        shl     edi,2;*4
        add     edi,x_y                 ;edi -> pixel


        fld     lz
        mov     esi,scene.sc_divtab
        fld     rz
        fsub    st,st(1)
        fmul    dword ptr [esi+ecx*4]
        fxch    st(1)

@@l:    fcom    dword ptr[edi]
        fstsw   ax
        sahf
        ja      @@0
        fst     dword ptr[edi]
@@0:
        add     edi,4
        fadd    st,st(1)
        dec     ecx
        jnz     @@l

        fstp    st
        fstp    st

@@w:
        add     x_y,zxres*4
        inc     y
        cmp     y,zyres                 ;while (y < zyres)
        jl      @@y_l

@@fertig:

@@weg:  ret
renderline endp


cproject proc near
        ;esi -> camera
                                        ;
                                        ;


                                        ;st(2) = x1, st(1) = x2, st(0) = x3
        fld     [esi].v_l3.x1           ;calc x3 (z)
        fmul    st,st(3)
        fld     [esi].v_l3.x2
        fmul    st,st(3)
        faddp   st(1),st
        fld     [esi].v_l3.x3
        fmul    st,st(2)
        faddp   st(1),st                ;st(0) = z
        fxch    st(1)
                                        ;st(3) = x1, st(2) = x2, st(0) = x3
        fld     [esi].v_l1.x1           ;calc x1 (x)
        fmul    st,st(4)
        fld     [esi].v_l1.x2
        fmul    st,st(4)
        faddp   st(1),st
        fld     [esi].v_l1.x3
        fmul    st,st(2)
        faddp   st(1),st                ;st(0) = x
        fadd    st,st(2)
        fmul    xmid                    ;(z+x)*xmid, (1+x/z) = (z+x)/z
        fxch    st(4)
                                        ;st(3) = x2, st(1) = x3, st(0) = x1
        fmul    [esi].v_l2.x1           ;calc x2 (y)
        fld     [esi].v_l2.x2
        fmul    st,st(4)
        faddp   st(1),st
        fld     [esi].v_l2.x3
        fmulp   st(2),st                ;st(0) = y
        faddp   st(1),st
        fsubr   st,st(1)
        fmul    ymid                    ;(z-y)*ymid, (1-y/z) = (z-y)/z
        fstp    st(2)

        ret
cproject endp

rtrace proc pascal
arg     b, u, oldcube
        ;-> ebx -> tcube
        ;-> st(0) = r
        ;<- edx = sidecube (or NULL)

        fldz                            ;r_min
        xor     edx,edx                 ;sidecube

        mov     ecx,c_planes[ebx]
        jecxz   @@weg
        mov     ebx,c_planedata[ebx]
@@l:
        mov     eax,[ebx].pl_sidecube
        cmp     eax,oldcube
        je      @@next

        mov     esi,b
        fld     [ebx].pl_p.x1
        fsub    [esi].x1
        fmul    [ebx].pl_n.x1
        fld     [ebx].pl_p.x2
        fsub    [esi].x2
        fmul    [ebx].pl_n.x2
        faddp   st(1),st
        fld     [ebx].pl_p.x3
        fsub    [esi].x3
        fmul    [ebx].pl_n.x3
        faddp   st(1),st                ;st = (p-b)*n

        ftst                            ;st > 0: entered side-cube
        fstsw   ax
        sahf
        jb      @@w


        mov     esi,u                   ;r - 1.0 = (p-b)*n/(u*n)
        fld     [esi].x1
        fmul    [ebx].pl_n.x1
        fld     [esi].x2
        fmul    [ebx].pl_n.x2
        faddp   st(1),st
        fld     [esi].x3
        fmul    [ebx].pl_n.x3
        faddp   st(1),st

        fdivp   st(1),st                ;st = r - 1.0

        fcom    st(1)                   ;r < r_min?
        fstsw   ax
        sahf
        ja      @@w
        fxch    st(1)
        mov     edx,[ebx].pl_sidecube
@@w:
        fstp    st
@@next:
        add     ebx,size tplane
        dec     ecx
        jnz     @@l

@@weg:  ret
rtrace endp


vollight proc pascal
arg     vstamp, datapos, lightflags

local   a:dword:3, b:dword:3, u:dword:3
local   xz, vx:dword:3, sx:dword:3, yz, vy:dword:3, sy:dword:3
local   cp:dword:3, ca:dword:3, cb:dword:3, cu:dword:3
local   xa, ya, xb, yb
local   oldcube, cubeptr
local   zvalues
local   ipmap
local   cubelist, cubelists, cubeliste, cubeliststep
local   idxlistpos, idxlistend, idxliststep
local   asize, bsize, bscale

        mov     eax,scene.sc_zline     ;rendered z-buffer lines
;   add eax,zxres*4
        mov     zvalues,eax

        mov     eax,scene.sc_lmap      ;permission map for indices
        mov     ipmap,eax

        add     eax,256                 ;cube id's and y values
        mov     cubelist,eax            ; format: idx, y, idx, y, ..., idx


        lea     esi,viewer              ;transform light position into
        lea     ebx,light               ; local camera coordinate system
    fld [esi].v_1_tan_fov
    fmul [ebx].v_1_tan_fov
    fstp bscale
        fld     [ebx].v_p.x1            ;p(light) - p(camera)
        fsub    [esi].v_p.x1
        fld     [ebx].v_p.x2
        fsub    [esi].v_p.x2
        fld     [ebx].v_p.x3
        fsub    [esi].v_p.x3
        call    cproject                ;transform to camera coordinate system
        fstp    cp.x3
        fstp    cp.x2
        fstp    cp.x1
                                        ;calc vy, sx, sy
        mov     eax,scene.sc_divtab
        fld     dword ptr [eax+(zxres/2)*4]
        fld     [ebx].lv_l1.x1          ;vy = -(l1 + l2*a) + l3
        fst     vy.x1                   ;sx = l1/(zxres/2)
        fmul    st,st(1)
        fstp    sx.x1
        fld     [ebx].lv_l1.x2
        fst     vy.x2
        fmul    st,st(1)
        fstp    sx.x2
        fld     [ebx].lv_l1.x3
        fst     vy.x3
        fmulp   st(1),st
        fstp    sx.x3

        fld     z_a                     ;sy = l2*a
        fld     [ebx].lv_l2.x1
        fmul    st,st(1)
        fst     sy.x1
        fadd    vy.x1
;    fstp sy.x1
;    fld  vy.x1
        fsubr   [ebx].v_l3.x1
        fstp    vy.x1

        fld     [ebx].lv_l2.x2
        fmul    st,st(1)
        fst     sy.x2
        fadd    vy.x2
;    fstp sy.x2
;    fld  vy.x2
        fsubr   [ebx].v_l3.x2
        fstp    vy.x2

        fld     [ebx].lv_l2.x3
        fmulp   st(1),st
        fst     sy.x3
        fadd    vy.x3
;    fstp sy.x3
;    fld  vy.x3
        fsubr   [ebx].v_l3.x3
        fstp    vy.x3



        mov     yz,zyres;-1
@@y_l:          ;y-loop
        mov     xz,-1 + zxres;/2

        mov     eax,vy.x1
        mov     ecx,vy.x2
        mov     edx,vy.x3
        mov     vx.x1,eax
        mov     vx.x2,ecx
        mov     vx.x3,edx
@@x_l:          ;x-loop
                                        ;ebx -> light (tviewer)
                                        ;edi -> camera (tviewer)
;        mov     eax,[ebx].v_p.x1
;        mov     ecx,[ebx].v_p.x2
;        mov     edx,[ebx].v_p.x3
;        mov     a.x1,eax
;        mov     a.x2,ecx
;        mov     a.x3,edx

        mov     eax,cp.x1
        mov     ecx,cp.x2
        fld cp.x3;mov     edx,cp.x3
        mov     ca.x1,eax
        mov     ca.x2,ecx
        fst ca.x3;mov     ca.x3,edx               ;keep az

        mov     eax,[ebx].v_cube
        mov     oldcube,NULL
        mov     cubeptr,eax

        mov     eax,zvalues
        fld     dword ptr [eax]
        add     zvalues,4

  fcom c_max
  fstsw ax
  sahf
  jb @@t0

;  ftst
;  fstsw ax
;  sahf
;  jne @@t0
  fstp st                             ;remove zvalue
  fstp st                             ;remove az
  jmp @@w1
@@t0:
      fldz;1
      fstp asize
     fld bscale
     fmul st,st(1)
     fstp bsize
;      fst bsize


        fld     vx.x1                   ;make u, a, b
        fmul    st,st(1)
        fst     u.x1
        fld     [ebx].v_p.x1
        fst     a.x1
        fadd    st,st(1)
;        fld     st
;        fadd    a.x1
        fstp    b.x1
        fld     vx.x2
        fmul    st,st(2)
        fst     u.x2
        fld     [ebx].v_p.x2
        fst     a.x2
        fadd    st,st(1)
;        fld     st
;        fadd    a.x2
        fstp    b.x2
        fld     vx.x3
        fmul    st,st(3)
        fst     u.x3
        fld     [ebx].v_p.x3
        fst     a.x3
        fadd    st,st(1)
;        fld     st
;        fadd    p.x3
        fstp    b.x3
        call    cproject
        fst     cu.x3
        fadd    ca.x3
        fst     cb.x3
        fstp    st(3)                   ;remove zval & keep bz
        fst     cu.x2
        fadd    ca.x2
        fstp    cb.x2
        fst     cu.x1
        fadd    ca.x1
        fstp    cb.x1

;        fstp    st

;        fld     ca.x3
;        fld     cb.x3

        push    ebx esi

                ;z-clipping
        fld1                             ;z-clip line a-b

        fcom    st(2)                    ;az < 1.0?
        fstsw   ax
        mov     dl,ah
        fcomp   st(1)                    ;bz < 1.0?
        fstsw   ax
        xor     ah,dl
        sahf
        jnc     @@Nclip
                ;clip

                ;r = (1.0-az)/(bz-az)
        fsub    st,st(1)
        fld1
        fsubrp  st(2),st
        fdivp   st(1),st                ;st(0) = r

        test    dl,1
        jnz     @@bclip
                ;az < 1.0
        fld1
        fsubrp  st(1),st                ;r = 1-r
                                        ;st(0) = r(z=0)
@@t_l:                                  ;find crossing point (a+r*u) in cubes
        lea     eax,b
        lea     edx,u
        mov     ebx,cubeptr
        call    rtrace pascal, eax, edx, oldcube
        or      edx,edx
        jz      @@0
        fchs
        fcomp   st(1)                   ;trace cubes until r(z=0) < r(cube)
        fstsw   ax
        sahf
        ja      @@0
        xchg    edx,cubeptr
        mov     oldcube,edx
        jmp     @@t_l
@@0:
        fstp    st

        fld     u.x1                    ;recalc u and a
        fmul    st,st(1)
        fst     u.x1
        fsubr   b.x1
        fstp    a.x1
        fld     u.x2
        fmul    st,st(1)
        fst     u.x2
        fsubr   b.x2
        fstp    a.x2
        fld     u.x3
        fmul    st,st(1)
        fst     u.x3
        fsubr   b.x3
        fstp    a.x3

        fld     cu.x1                   ;recalc cu and ca
        fmul    st,st(1)
        fst     cu.x1
        fsubr   cb.x1
        fstp    ca.x1
        fld     cu.x2
        fmulp   st(1),st                ;remove r
        fst     cu.x2
        fsubr   cb.x2
        fstp    ca.x2
        fld1
        fst     ca.x3                   ;ca.x3 = 1.0
        fsubr   cb.x3
        fstp    cu.x3                   ;cu.x3 = cb.x3 - 1.0

        jmp     @@Nw
@@bclip:        ;bz < 1.0

        fld     u.x1                    ;recalc u and b
        fmul    st,st(1)
        fst     u.x1
        fadd    a.x1
        fstp    b.x1
        fld     u.x2
        fmul    st,st(1)
        fst     u.x2
        fadd    a.x2
        fstp    b.x2
        fld     u.x3
        fmul    st,st(1)
        fst     u.x3
        fadd    a.x3
        fstp    b.x3

        fld     cu.x1                   ;recalc cu and cb
        fmul    st,st(1)
        fst     cu.x1
        fadd    ca.x1
        fstp    cb.x1
;  fld     cu.x3                   ;recalc cu and cb
;  fmul    st,st(1)
;  fst     cu.x3
;  fadd    ca.x3
;  fstp    cb.x3
        fld     cu.x2
        fmul    st,st(1)
        fst     cu.x2
        fadd    ca.x2
        fstp    cb.x2
        fld1
        fst     cb.x3                   ;cb.x3 = 1.0
        fsub    ca.x3
        fstp    cu.x3                   ;cu.x3 = 1.0 - ca.x3

        fmul    bsize                   ;remove r
        fstp    bsize


        jmp     @@Nw
@@Nclip:
        fstp    st
        fstp    st

        test    dl,1
        jz      @@w                     ;line not visible

@@Nw:

        mov     edi,ipmap               ;clear index permission map
        xor     eax,eax
        mov     ecx,256/4
        rep     stosd

                                        ;test direction (horizontal/vertical)
        fld     ca.x1
        fdiv    ca.x3
        fst     xa
        fld     cb.x1
        fdiv    cb.x3
        fst     xb
        fsubp   st(1),st
        fabs

        fld     ca.x2
        fdiv    ca.x3
        fst     ya
        fld     cb.x2
        fdiv    cb.x3
        fst     yb
        fsubp   st(1),st
        fabs

        fcompp
        fstsw   ax
        sahf
        jb      @@hor
                ;vertical
        mov     edi,cubelist

@@t_lv:                                 ;make cube idx and y list
        mov     ebx,cubeptr             ; format: idx, y, idx, y, ..., idx

        mov     eax,[ebx].c_vstamp
        cmp     eax,vstamp
        mov     eax,-1
        jne     @@v0
        mov     edx,[ebx].c_vtc
        mov     eax,[edx].tc_idx
@@v0:
        mov     [edi],eax

        lea     eax,b
        lea     edx,u
        call    rtrace pascal, eax, edx, oldcube
        or      edx,edx
        jz      @@v1                    ;no more cubes to trace

        xchg    edx,cubeptr
        mov     oldcube,edx

        fld     cu.x2                   ;y = cb.x2+(r-1)*cu.x2
        fmul    st,st(1)
        fadd    cb.x2

        fld     cu.x3                   ;z = cb.x3+(r-1)*cu.x3
        fmul    st,st(2)
        fadd    cb.x3

        fdivp   st(1),st                ;sy = y/z

        fistp   dword ptr [edi+4]       ;store y
        add     edi,8
        fstp    st
        jmp     @@t_lv
@@v1:
        fstp    st                      ;edi = cubelist end
        mov     edx,cubelist            ;edx = cubelist start
        mov     ecx,8                   ;ecx = cubelist step


        fld     cb.x3                   ;z distance from camera of b
        fld     ca.x3                   ;z distance from camera of a

        fld     ya                      ;default: a higher on screen
        fcomp   yb
        fstsw   ax
        sahf
        jb      @@v4
                                        ;b higher on screen
        fxch    st(1)                   ;st(0) = z distance of higher point
        xchg    edi,edx                 ;exchange cubelists and cubeliste
        neg     ecx
@@v4:
        mov     cubelists,edx
        mov     cubeliste,edi
        mov     cubeliststep,ecx
                                        ;st(0) = z value of higher point
                                        ;st(1) = z value of lower point

                ;ipmap for objects
        mov     ebx,scene.sc_idxlist   ;make index permission map for
        mov     edi,ipmap               ; highest point
@@i_lv:
        cmp     ebx,scene.sc_idxlistend
        jae     @@v5
        fcom    [ebx].i_zval            ;search lesser z value in idxlist
        fstsw   ax
        sahf
        ja      @@v5
        mov     eax,[ebx].i_idx
        mov     byte ptr [edi+eax],1
        add     ebx,size tidxentry
        jmp     @@i_lv
@@v5:

        mov     edx,scene.sc_idxlistend;default: decreasing z-values
        mov     ecx,size tidxentry      ;increase addresses
                                        ;ebx -> first idxentry to test
        fcompp;  st(1)                   ;remove z values
        fstsw   ax
        sahf
        ja      @@v6
        mov     edx,scene.sc_idxlist   ;increasing zvalues
        cmp     ebx,edx                 ;at beginning of buffer?
        jbe     @@v6
        sub     ebx,ecx                 ;ebx -> first idxentry to test
        neg     ecx                     ;decrease addresses
@@v6:
        mov     idxlistpos,ebx
        mov     idxlistend,edx          ;
        mov     idxliststep,ecx


        fld     zxw                     ;x_width
        cmp     cubeliststep,0          ;negative if b higher a
        jl      @@v9
        fchs
@@v9:

        mov     edi,datapos
        mov     ebx,edi

        fld     ca.x3                   ;st = z
        fdivr   st,st(1)                ;st = x_width/z
;  fld z_csize
;  fadd asize
;  fmulp st(1),st
  fmul asize

        fld     xa                      ;st = x/z
        fadd    st,st(1)
        fstp    [edi].sp_x              ;x/z + x_width/z
        fsubr   xa
        fstp    [edi+volpsize].sp_x     ;x/z - x_width/z
        fld     ya                      ;st = y/z
        fst     [edi].sp_y
        fstp    [edi+volpsize].sp_y
        fldz
        fstp    [edi].sp_u
        fld     c_umax
        fstp    [edi+volpsize].sp_u

        lea     edi,[edi+2*volpsize]

        fld     cb.x3                   ;st = z
        fdivp   st(1),st                ;st = x_width/z
;  fld z_csize
;  fadd bsize
;  fmulp st(1),st
  fmul bsize

        fld     xb                      ;st = x/z
        fsub    st,st(1)
        fstp    [edi].sp_x              ;x/z - x_width/z
        fadd    xb
        fstp    [edi+volpsize].sp_x     ;x/z + x_width/z
        fld     yb                      ;st = y/z
        fst     [edi].sp_y
        fstp    [edi+volpsize].sp_y
        fld     c_umax
        fstp    [edi].sp_u
        fldz
        fstp    [edi+volpsize].sp_u

        lea     edi,[edi+2*volpsize]

        call    xclip pascal, volpsize, volvars
        jbe     @@w

        sub     edi,ebx

test    lightflags,lfWater
jnz  @@vwa0
mov    esi,(fogmapsteps-1)*fogmapsize
jmp   @@vwa1
@@vwa0:
mov     eax,frame
mov     edx,yz
shl     edx,zxshift+3
add     eax,edx
and     eax,1Fh shl zxshift        ;time-resolution = 32
add     eax,xz
add     eax,scene.sc_fogwater
movzx   esi,byte ptr [eax]
shl     esi,ld_fogmapsize                   ;*fogmapsize
@@vwa1:
add     esi,scene.sc_fogmap

 lea eax,ca
 lea edx,cu
call vvolpolygon pascal, ebx, edi, cubelists, cubeliste, cubeliststep, idxlistpos, idxlistend, idxliststep, eax, edx, ipmap, esi
; pascal, ebx, edi,
;  cubelists, cubeliste, cubeliststep,
;  idxlistpos, idxlistend, idxliststep, ca, cu, ipmap, mapptr

        jmp     @@w
        pop esi edx
        jmp @@weg
@@hor:          ;horizontal
;jmp @@w
        mov     edi,cubelist

@@t_lh:                                 ;make cube idx and x list
        mov     ebx,cubeptr             ; format: idx, x, idx, x, ..., idx

        mov     eax,[ebx].c_vstamp
        cmp     eax,vstamp
        mov     eax,-1
        jne     @@h0
        mov     edx,[ebx].c_vtc
        mov     eax,[edx].tc_idx
@@h0:
        mov     [edi],eax

        lea     eax,b
        lea     edx,u
        call    rtrace pascal, eax, edx, oldcube
        or      edx,edx
        jz      @@h1                    ;no more cubes to trace

        xchg    edx,cubeptr
        mov     oldcube,edx

        fld     cu.x1                   ;x = cb.x1+(r-1)*cu.x1
        fmul    st,st(1)
        fadd    cb.x1

        fld     cu.x3                   ;z = cb.x3+(r-1)*cu.x3
        fmul    st,st(2)
        fadd    cb.x3

        fdivp   st(1),st                ;sx = x/z

        fistp   dword ptr [edi+4]       ;store x
        add     edi,8
        fstp    st
        jmp     @@t_lh
@@h1:
        fstp    st                      ;edi = cubelist end
        mov     edx,cubelist            ;edx = cubelist start
        mov     ecx,8                   ;ecx = cubelist step


        fld     cb.x3                   ;z distance from camera of b
        fld     ca.x3                   ;z distance from camera of a

        fld     xa                      ;default: a more left on screen
        fcomp   xb
        fstsw   ax
        sahf
        jb      @@h4
                                        ;b more left on screen
        fxch    st(1)                   ;st(0) = z distance of higher point
        xchg    edi,edx                 ;exchange cubelists and cubeliste
        neg     ecx
@@h4:
        mov     cubelists,edx
        mov     cubeliste,edi
        mov     cubeliststep,ecx
                                        ;st(0) = z value of left point
                                        ;st(1) = z value of right point

                ;ipmap for objects
        mov     ebx,scene.sc_idxlist   ;make index permission map for
        mov     edi,ipmap               ; left point
@@i_lh:
        cmp     ebx,scene.sc_idxlistend
        jae     @@h5
        fcom    [ebx].i_zval            ;search lesser z value in idxlist
        fstsw   ax
        sahf
        ja      @@h5
        mov     eax,[ebx].i_idx
        mov     byte ptr [edi+eax],1
        add     ebx,size tidxentry
        jmp     @@i_lh
@@h5:

        mov     edx,scene.sc_idxlistend;default: decreasing z-values
        mov     ecx,size tidxentry      ;increase addresses
                                        ;ebx -> first idxentry to test
        fcompp;  st(1)                   ;remove z values
        fstsw   ax
        sahf
        ja      @@h6
        mov     edx,scene.sc_idxlist   ;increasing zvalues
        cmp     ebx,edx                 ;at beginning of buffer?
        jbe     @@h6
        sub     ebx,ecx                 ;ebx -> first idxentry to test
        neg     ecx                     ;decrease addresses
@@h6:
        mov     idxlistpos,ebx
        mov     idxlistend,edx          ;
        mov     idxliststep,ecx


        fld     zyw                     ;y_width
        cmp     cubeliststep,0          ;negative if b more left than a
        jl      @@h9
        fchs
@@h9:

        mov     edi,datapos
        mov     ebx,edi

        fld     ca.x3                   ;st = z
        fdivr   st,st(1)                ;st = y_width/z
;  fld z_csize
;  fadd asize
;  fmulp st(1),st
  fmul asize

        fld     ya                      ;st = y/z
        fadd    st,st(1)
        fstp    [edi].sp_y              ;y/z + y_width/z
        fsubr   ya
        fstp    [edi+volpsize].sp_y     ;y/z - y_width/z
        fld     xa                      ;st = x/z
        fst     [edi].sp_x
        fstp    [edi+volpsize].sp_x
        fldz
        fstp    [edi].sp_u
        fld     c_umax
        fstp    [edi+volpsize].sp_u

        lea     edi,[edi+2*volpsize]

        fld     cb.x3                   ;st = z
        fdivp   st(1),st                ;st = y_width/z
;  fld z_csize
;  fadd bsize
;  fmulp st(1),st
  fmul bsize

        fld     yb                      ;st = y/z
        fsub    st,st(1)
        fstp    [edi].sp_y              ;y/z - y_width/z
        fadd    yb
        fstp    [edi+volpsize].sp_y     ;y/z + y_width/z
        fld     xb                      ;st = x/z
        fst     [edi].sp_x
        fstp    [edi+volpsize].sp_x
        fld     c_umax
        fstp    [edi].sp_u
        fldz
        fstp    [edi+volpsize].sp_u

        lea     edi,[edi+2*volpsize]

        call    yclip pascal, volpsize, volvars
        jbe     @@w

        sub     edi,ebx

test    lightflags,lfWater
jnz  @@hwa0
mov    esi,(fogmapsteps-1)*fogmapsize
jmp   @@hwa1
@@hwa0:
mov     eax,frame
mov     edx,yz
shl     edx,zxshift+2
add     eax,edx
and     eax,1Fh shl zxshift        ;time-resolution = 32
add     eax,xz
add     eax,scene.sc_fogwater
movzx   esi,byte ptr [eax]
shl     esi,ld_fogmapsize                     ;*fogmapsize
@@hwa1:
add     esi,scene.sc_fogmap

 lea eax,ca
 lea edx,cu
call hvolpolygon pascal, ebx, edi, cubelists, cubeliste, cubeliststep, idxlistpos, idxlistend, idxliststep, eax, edx, ipmap, esi
; pascal, ebx, edi,
;  cubelists, cubeliste, cubeliststep,
;  idxlistpos, idxlistend, idxliststep, ca, cu, ipmap, mapptr





@@w:
        pop     esi ebx
@@w1:
;comment #
        fld     sx.x1                   ;update in x direction
        fadd    vx.x1
        fstp    vx.x1
        fld     sx.x2
        fadd    vx.x2
        fstp    vx.x2
        fld     sx.x3
        fadd    vx.x3
        fstp    vx.x3
;#
        dec     xz
        jns     @@x_l
;comment #
        fld     sy.x1                   ;update in y direction
        fadd    vy.x1
        fstp    vy.x1
        fld     sy.x2
        fadd    vy.x2
        fstp    vy.x2
        fld     sy.x3
        fadd    vy.x3
        fstp    vy.x3
;#
        dec     yz
        jnz     @@y_l


@@weg:  ret
vollight endp

yclip proc pascal ;y-clipping
arg     spsize:dword, vars:dword ;data variables -1
        ;-> ebx = *sp
        ;-> edi = *sp_end
        ;<- ebx = *sp
        ;<- edi = *sp_end
        ;<- wenn weniger als 3 punkte, mit jbe wegspringen
local   _sp:dword, _sp_end:dword ;beides pointer

        fldz                            ;st = y
        mov     dl,1                    ;dl = i, 1 to 0
@@i_l:
        mov     _sp,ebx                 ;ebx -> sp[z] (quellpunkte) (z = 0)
        mov     _sp_end,edi             ;edi -> sp[d] (zielpunkte)  (d = m)

@@z_l:
        fld     sp_y[ebx]               ;inn = (sp[z].y >= y);
        fcomp   st(1)
        fstsw   ax
        mov     dh,ah                   ;dh = inn
        xor     ah,dl                   ;inn xor i
        sahf
        jnc     @@0
                ;punkt innerhalb
        mov     esi,ebx
        mov     ecx,spsize
        shr     ecx,2
        rep     movsd                   ;sp[d] = sp[z] und d++
@@0:
        mov     esi,ebx
        add     esi,spsize              ;nz = z+1
        cmp     esi,_sp_end             ;if (nz >= m) nz = 0;
        jb      @@wrap
        mov     esi,_sp
@@wrap:

        fld     sp_y[esi]               ;if (inn ^ (sp[nz].y >= y))
        fcomp   st(1)
        fstsw   ax
        xor     ah,dh                   ;dh = inn
        sahf
        jnc     @@1
                ;dieser oder n„chster punkt auáerhalb

        fst     sp_y[edi]               ;sp[d].y speichern

                ;r berechnen
        fld     st;y                    ;(y - sp[z].y)
        fsub    sp_y[ebx]
        fld     sp_y[esi]               ;/(sp[nz].y - sp[z].y)
        fsub    sp_y[ebx]
        fdivp   st(1),st                ;st = r

        fld     sp_x[ebx]               ;sp[d].x berechnen
        fld     sp_x[esi]               ; sp[z].x + r*(sp[nz].x - sp[z].x)
        fsub    st,st(1)
        fmul    st,st(2)
        faddp   st(1),st
        fstp    sp_x[edi]

        mov     ecx,vars
@@vars: fld     [esi].sp_data[ecx*4]
        fsub    [ebx].sp_data[ecx*4]
        fmul    st,st(1)
        fadd    [ebx].sp_data[ecx*4]
        fstp    [edi].sp_data[ecx*4]
        dec     ecx
        jns     @@vars

        fstp    st                      ;r entfernen

        add     edi,spsize              ;d++
@@1:
        cmp     esi,ebx                 ;wz > z?
        mov     ebx,esi                 ;entspricht ebx += spsize
        ja      @@z_l                   ;z-schleife

        mov     ebx,_sp_end             ;ebx -> sp
                                        ;edi -> sp_end
        mov     eax,edi
        sub     eax,ebx
        shr     eax,1
        cmp     eax,spsize
        jbe     @@w

        fiadd   yres                    ;y += yres
        dec     dl                      ;i-schleife
        jns     @@i_l

@@w:    fstp    st                      ;y entfernen
@@weg:  ret
yclip endp



vvolpolygon proc pascal
arg _sp, sp_end, cubelistpos, cubelistend, cubeliststep, idxlistpos, idxlistend, idxliststep, ca, cu, ipmap, mapptr
;arg
;arg
;_sp = *sp, zeiger auf screenpoints

;call vvolpolygon pascal, ebx, edi,
;  cubelists, cubeliste, cubeliststep,
;  idxlistpos, idxlistend, idxliststep, ca, cu, ipmap, mapptr

local   pend, y, x_y, lb, lc, rb, rc ;int
local   cc, cidx, oc ;int  counter for cube permissions, counter for object permissions
local   lx, ldx, lu, ldu ;float
local   rx, rdx, ru, rdu ;float
local   xa, xe, txt_x, ;int


        xor     eax,eax                 ;start- und endpunkt bestimmen
        mov     lb,eax
        mov     rb,eax
        mov     pend,eax

        mov     esi,_sp                 ;esi -> screenpoints
        fld     sp_y[esi]               ;st(0) = ymax
        fld     st                      ;st(1) = ymin

        mov      ecx,sp_end
        sub      ecx,volpsize           ;esi+ecx -> sp[sp_end-1]
@@max_l:

        fld     [esi+ecx].sp_y
        fcom    st(1);ymax              ;gr”áten y-wert finden (endpunkt)
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

        sub     ecx,volpsize
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
        mov     eax,scene.sc_buffer
        shr     eax,2
        add     edx,eax
        mov     x_y,edx

        mov     lc,1
        mov     rc,1

        mov     cc,1
        mov     cidx,256

                ;object permission counter
        mov     esi,idxlistpos
        mov     oc,maxint
        cmp     esi,idxlistend
        je      @@y_l

        fld     [esi].i_zval            ;r = (zval - a3)/u3
        fld     st
        mov     ebx,ca
        mov     edx,cu
        fsub    [ebx].x3
        fdiv    [edx].x3                ;st(0) = r, st(1) = zval

        fld     [edx].x2                ;y = (a2 + r*u2)/zval
        fmulp   st(1),st
        fadd    [ebx].x2
        fdivrp  st(1),st

        fistp   oc
        mov     eax,y
        sub     oc,eax


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
        sub     esi,volpsize            ;lb--
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
      ;fist txt_x
        fstp    lu

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
      ;fist txt_x
        fstp    lu

        fld     lx
        fadd    ldx
        fst     lx                      ;lx bleibt im copro
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
        add     esi,volpsize            ;rb++
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

        fld     rx
        fadd    rdx
        fst     rx                      ;rx bleibt im copro
@@r_z:

        fistp   xe                      ;xe = ceil(rx) (rx entfernen)
        fistp   xa                      ;xa = ceil(lx) (lx entfernen)

                ;update cube permissions
        dec     cc
        jnz     @@c_nz
        mov     ebx,cubelistpos
@@cc1:
        mov     edi,ipmap
        mov     ecx,[ebx]               ;ecx = idx
        or      ecx,ecx
        js      @@11
        mov     eax,cidx
        mov     cidx,ecx
        add     edi,eax

        sub     ecx,eax                 ;ecx = indices to change
        mov     al,0                    ;default: disable some indices
        jns     @@10
        add     edi,ecx                 ;enable some indices
        neg     ecx
        mov     al,1
@@10:
        rep     stosb
@@11:
        cmp     ebx,cubelistend
        je      @@c_nz
        mov     eax,[ebx+4]             ;eax = y
        add     ebx,cubeliststep        ;update cubelistpos (+8 or -8)
        sub     eax,y
        jle     @@cc1
;        jz      @@cc1
        mov     cc,eax
        mov     cubelistpos,ebx
@@c_nz:
                ;update object permissions
        dec     oc
        jg      @@o_nz
        mov     esi,idxlistpos
@@oc1:

        mov     ebx,ipmap
        mov     eax,[esi].i_idx
        xor     byte ptr[ebx+eax],1

        add     esi,idxliststep
        mov     oc,maxint
        cmp     esi,idxlistend
        je      @@o_nz

        fld     [esi].i_zval            ;r = (zval - a3)/u3
        fld     st
        mov     ebx,ca                  ;ebx -> ca (sttzvektor)
        mov     edx,cu                  ;edx -> cu (richtungsvektor)
        fsub    [ebx].x3
        fdiv    [edx].x3                ;st(0) = r, st(1) = zval

        fld     [edx].x2                ;y = (a2 + r*u2)/zval
        fmulp   st(1),st
        fadd    [ebx].x2
        fdivrp  st(1),st

        fistp   oc
        mov     eax,y
        sub     oc,eax
        jle     @@oc1
        mov     idxlistpos,esi
@@o_nz:


;eax = 00?x
;ebx = 00Uu
;ecx = III?
;edx = 000X
;esi = TTTT
;edi = PPPP
;ebp = CCCC

        mov     edi,xa
        mov     edx,xe
        sub     edx,edi                 ;edx = CCCC
        jle     @@w

        fld     lu                      ;lu
        fld     ru
        fsub    st,st(1)                ;ru-lu

        fld     lx                      ;lx
        fld     rx
        fsub    st,st(1)                ;rx-lx

        fild    xa
        fsub    st,st(2)
        fdiv    st,st(1)
        fmul    st,st(3)                ;(xa-lx)/(rx-lx) * (ru-lu)


        fild    xe
        fsub    st,st(3)
        fdiv    st,st(2)
       add     edi,x_y                 ;edi = PPPP
        fmul    st,st(4)                ;(xe-lx)/(rx-lx) * (ru-lu)
        fsub    st,st(1)
        mov     esi,scene.sc_divtab
        fmul    dword ptr[esi+edx*4]
        fistp   txt_x

        faddp   st(4),st
        fstp    st
        fstp    st
       mov     ebx,txt_x               ;ebx = 00Uu
        fstp    st
        fistp   txt_x

        mov     ecx,ipmap               ;ecx = IIII
        mov     esi,mapptr              ;esi = TTTT
        mov     eax,txt_x               ;eax = 00Xx

;        add     edi,x_y                 ;edi = PPPP
;        mov     eax,txt_x               ;eax = 00Xx

;        mov     esi,scene.sc_divtab
;        fld     ru
;        fsub    lu
;        fmul    dword ptr[esi+edx*4-4]
;        fistp   txt_x

;        mov     ecx,ipmap               ;ecx = IIII
;        mov     esi,mapptr              ;esi = TTTT
;        mov     ebx,txt_x               ;ebx = 00Uu

        push    ebp                     ;ebp : lokale variablen
        mov     ebp,edx                 ;ebp = CCCC
        movzx   edx,ah                  ;edx = 000X

@@inner:
        mov     cl,[edi*4+3]            ;idx
        mov     ah,[esi+edx]
        test    byte ptr [ecx],1        ;index permission map
        jz      @@i0
        add     [edi*4+2],ah
@@i0:   add     al,bl
        adc     dl,bh
        inc     edi
        dec     ebp
        jnz     @@inner

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
vvolpolygon endp


hvolpolygon proc pascal
arg _sp, sp_end, cubelistpos, cubelistend, cubeliststep, idxlistpos, idxlistend, idxliststep, ca, cu, ipmap, mapptr
;arg
;arg
;_sp = *sp, zeiger auf screenpoints

;call hvolpolygon pascal, ebx, edi,
;  cubelists, cubeliste, cubeliststep,
;  idxlistpos, idxlistend, idxliststep, ca, cu, ipmap, mapptr

local   pend, x, x_y, lb, lc, rb, rc ;int
local   cc, cidx, oc ;int  counter for cube permissions, counter for object permissions
local   ly, ldy, lu, ldu ;float
local   ry, rdy, ru, rdu ;float
local   ya, ye, txt_x ;int


        xor     eax,eax                 ;start- und endpunkt bestimmen
        mov     lb,eax
        mov     rb,eax
        mov     pend,eax

        mov     esi,_sp                 ;esi -> screenpoints
        fld     sp_x[esi]               ;st(0) = xmax
        fld     st                      ;st(1) = xmin

        mov     ecx,sp_end
        sub     ecx,volpsize            ;esi+ecx -> sp[sp_end-1]
@@max_l:

        fld     [esi+ecx].sp_x
        fcom    st(1);ymax              ;gr”áten x-wert finden (endpunkt)
        fstsw   ax
        sahf
        jb      @@max
        mov     pend,ecx
        fst     st(1);ymax
        jmp     @@min
@@max:
        fcom    st(2);ymin              ;kleinsten x-wert finden (startpunkt)
        fstsw   ax
        sahf
        ja      @@min
        mov     lb,ecx
        mov     rb,ecx
        fst     st(2);ymin
@@min:  fstp    st

        sub     ecx,volpsize
        jnz     @@max_l
        fstp    st                      ;xmax entfernen

        fistp   x                       ;x = ceil(xmin)

        mov     edx,x
        cmp     edx,xres                ;x-clipping
        jge     @@weg

        or      edx,edx
        jg      @@x0
        xor     edx,edx
        mov     x,edx
@@x0:
        mov     eax,scene.sc_buffer
        shr     eax,2
        add     edx,eax
        mov     x_y,edx

        mov     lc,1
        mov     rc,1

        mov     cc,1
        mov     cidx,256

                ;object permission counter
        mov     esi,idxlistpos
        mov     oc,maxint
        cmp     esi,idxlistend
        je      @@x_l

        fld     [esi].i_zval            ;r = (zval - a3)/u3
        fld     st
        mov     ebx,ca
        mov     edx,cu
        fsub    [ebx].x3
        fdiv    [edx].x3                ;st(0) = r, st(1) = zval

        fld     [edx].x1                ;x = (a1 + r*u1)/zval
        fmulp   st(1),st
        fadd    [ebx].x1
        fdivrp  st(1),st

        fistp   oc
        mov     eax,x
        sub     oc,eax


@@x_l:  ;x-schleife

                ;links
        dec     lc
        jnz     @@l_nz
        mov     edi,_sp
        mov     esi,lb
@@lc:
        cmp     esi,pend
        je      @@fertig                ;lb == pend -> rechts angekommen
        mov     ebx,esi                 ;ia = lb
        sub     esi,volpsize            ;lb--
        jnc     @@l0                    ;wrap
        add     esi,sp_end
@@l0:
        fld     sp_x[esi+edi]           ;lc = ceil(sp[lb].sx) - x
        fistp   lc
        mov     eax,x
        sub     lc,eax
        jle     @@lc                    ;while lc <= 0
        mov     lb,esi


        fild    x
        fsub    sp_x[ebx+edi]           ;(x  - sp[ia].x)

        fld     sp_x[edi+esi]           ;(sp[lb].x - sp[ia].x)
        fsub    sp_x[edi+ebx]

                ;ldu = (sp[lb].u - sp[ia].u)/(sp[lb].x - sp[ia].x);
        fld     sp_u[edi+esi]
        fsub    sp_u[edi+ebx]
        fdiv    st,st(1)
        fst     ldu

                ;lu = ldu  *(x  - sp[ia].x) + sp[ia].u;
        fmul    st,st(2)
        fadd    sp_u[edi+ebx]
;      fist txt_x
        fstp    lu

                ;ldy = (sp[lb].y - sp[ia].y)/(sp[lb].x - sp[ia].x);
        fld     sp_y[edi+esi]
        fsub    sp_y[edi+ebx]
        fdivrp  st(1),st
        fst     ldy

                ;ly = ldy  *(x  - sp[ia].x) + sp[ia].y;
        fmulp   st(1),st
        fadd    sp_y[edi+ebx]
        fst     ly

        jmp     @@l_z
@@l_nz:
        fld     lu
        fadd    ldu
;      fist txt_x
        fstp    lu

        fld     ly
        fadd    ldy                     ;lx bleibt im copro
        fst     ly
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
        add     esi,volpsize            ;rb++
        cmp     esi,sp_end
        jb      @@r0                    ;wrap
        xor     esi,esi
@@r0:
        fld     sp_x[esi+edi]           ;rc = ceil(sp[rb].sx) - x
        fistp   rc
        mov     eax,x
        sub     rc,eax
        jle     @@rc                    ;while lc <= 0
        mov     rb,esi


        fild    x
        fsub    sp_x[ebx+edi]           ;(x  - sp[ia].x)

        fld     sp_x[edi+esi]           ;(sp[rb].x - sp[ia].x)
        fsub    sp_x[edi+ebx]

                ;rdu = (sp[rb].u - sp[ia].u)/(sp[rb].x - sp[ia].x);
        fld     sp_u[edi+esi]
        fsub    sp_u[edi+ebx]
        fdiv    st,st(1)
        fst     rdu

                ;ru = rdu  *(x  - sp[ia].x) + sp[ia].u;
        fmul    st,st(2)
        fadd    sp_u[edi+ebx]
        fstp    ru

                ;rdy = (sp[rb].y - sp[ia].y)/(sp[rb].x - sp[ia].x);
        fld     sp_y[edi+esi]
        fsub    sp_y[edi+ebx]
        fdivrp  st(1),st
        fst     rdy

                ;ry = rdy  *(x  - sp[ia].x) + sp[ia].y;
        fmulp   st(1),st
        fadd    sp_y[edi+ebx]
        fst     ry

        jmp     @@r_z
@@r_nz:
        fld     ru
        fadd    rdu
        fstp    ru

        fld     ry
        fadd    rdy                     ;ry bleibt im copro
        fst     ry
@@r_z:

        fistp   ye                      ;ye = ceil(ry) (ry entfernen)
        fistp   ya                      ;ya = ceil(ly) (ly entfernen)

                ;update cube permissions
        dec     cc
        jnz     @@c_nz
        mov     ebx,cubelistpos
@@cc1:
        mov     edi,ipmap
        mov     ecx,[ebx]               ;ecx = idx
        or      ecx,ecx
        js      @@11
        mov     eax,cidx
        mov     cidx,ecx
        add     edi,eax

        sub     ecx,eax                 ;ecx = indices to change
        mov     al,0                    ;default: disable some indices
        jns     @@10
        add     edi,ecx                 ;enable some indices
        neg     ecx
        mov     al,1
@@10:
        rep     stosb
@@11:
        cmp     ebx,cubelistend
        je      @@c_nz
        mov     eax,[ebx+4]             ;eax = x
        add     ebx,cubeliststep        ;update cubelistpos (+8 or -8)
        sub     eax,y
        jle     @@cc1
;        jz      @@cc1
        mov     cc,eax
        mov     cubelistpos,ebx
@@c_nz:
                ;update object permissions
        dec     oc
        jg      @@o_nz
        mov     esi,idxlistpos
@@oc1:

        mov     ebx,ipmap
        mov     eax,[esi].i_idx
        xor     byte ptr[ebx+eax],1

        add     esi,idxliststep
        mov     oc,maxint
        cmp     esi,idxlistend
        je      @@o_nz

        fld     [esi].i_zval            ;r = (zval - a3)/u3
        fld     st
        mov     ebx,ca                  ;ebx -> ca (sttzvektor)
        mov     edx,cu                  ;edx -> cu (richtungsvektor)
        fsub    [ebx].x3
        fdiv    [edx].x3                ;st(0) = r, st(1) = zval

        fld     [edx].x1                ;x = (a1 + r*u1)/zval
        fmulp   st(1),st
        fadd    [ebx].x1
        fdivrp  st(1),st

        fistp   oc
        mov     eax,x
        sub     oc,eax
        jle     @@oc1
        mov     idxlistpos,esi
@@o_nz:


;eax = 00?x
;ebx = 00Uu
;ecx = III?
;edx = 000X
;esi = TTTT
;edi = PPPP
;ebp = CCCC

        mov     edi,ya
        mov     edx,ye
        sub     edx,edi                 ;edx = CCCC
        jle     @@w

        fld     lu                      ;lu
        fld     ru
        fsub    st,st(1)                ;ru-lu

        fld     ly                      ;ly
        fld     ry
        fsub    st,st(1)                ;ry-ly

        fild    ya
        fsub    st,st(2)
        fdiv    st,st(1)
        fmul    st,st(3)                ;(ya-ly)/(ry-ly) * (ru-lu)


        fild    ye
        fsub    st,st(3)
        fdiv    st,st(2)
       imul    edi,xres
       add     edi,x_y                 ;edi = PPPP
        fmul    st,st(4)                ;(ye-ly)/(ry-ly) * (ru-lu)
        fsub    st,st(1)
        mov     esi,scene.sc_divtab
        fmul    dword ptr[esi+edx*4]
        fistp   txt_x

        faddp   st(4),st
        fstp    st
        fstp    st
       mov     ebx,txt_x               ;ebx = 00Uu
        fstp    st
        fistp   txt_x

        mov     ecx,ipmap               ;ecx = IIII
        mov     esi,mapptr              ;esi = TTTT
        mov     eax,txt_x               ;eax = 00Xx

;        imul    edi,xres
;        add     edi,x_y                 ;edi = PPPP
;        mov     eax,txt_x               ;eax = 00Xx

;        mov     esi,scene.sc_divtab
;        fld     ru
;        fsub    lu
;        fmul    dword ptr[esi+edx*4-4]
;        fistp   txt_x

;        mov     ecx,ipmap               ;ecx = IIII
;        mov     esi,mapptr              ;esi = TTTT
;        mov     ebx,txt_x               ;ebx = 00Uu

        push    ebp                     ;ebp : lokale variablen
        mov     ebp,edx                 ;ebp = CCCC
        movzx   edx,ah                  ;edx = 000X

@@inner:
        mov     cl,[edi*4+3]            ;idx
        mov     ah,[esi+edx]
        test    byte ptr [ecx],1        ;index permission map
        jz      @@i0
 ;      mov byte ptr [edi*4+2],20
        add     [edi*4+2],ah
@@i0:   add     al,bl
        adc     dl,bh
        add     edi,xres
        dec     ebp
        jnz     @@inner

        pop     ebp
@@w:
        inc     x_y
        inc     x
        mov     eax,xres
        cmp     eax,x                   ;while (y < yres)
        jg      @@x_l

@@fertig:

@@weg:  ret
hvolpolygon endp




;---INIT-----------------------

initrgb16 proc near
        ;-> esi -> tvesa
        ;-> edi -> tpaltab16

        mov     ebx,(fvalues+xtrafvalues)-1 ;fog value (0 .. fvalues-1)
@@l0:
        mov     ebp,255                 ;color value (0 .. 255)
@@l1:
        mov     eax,255
        sub     eax,ebp
        imul    eax,ebx
        shr     eax,ld_fvalues
        add     eax,ebp
        cmp     eax,255
        jbe     @@clip
        mov     eax,255
@@clip:
                                        ;r
        mov     edx,eax
        mov     cl,[esi].vesa_redbits
        shl     edx,cl
        shr     edx,8
        mov     cl,[esi].vesa_redpos
        shl     edx,cl
        mov     ecx,ebp
        mov     ch,bl
        mov     [edi].p16_r[ecx*2],dx
                                        ;g
        mov     edx,eax
        mov     cl,[esi].vesa_greenbits
        shl     edx,cl
        shr     edx,8
        mov     cl,[esi].vesa_greenpos
        shl     edx,cl
        mov     ecx,ebp
        mov     ch,bl
        mov     [edi].p16_g[ecx*2],dx
                                        ;b
        mov     edx,eax
        mov     cl,[esi].vesa_bluebits
        shl     edx,cl
        shr     edx,8
        mov     cl,[esi].vesa_bluepos
        shl     edx,cl
        mov     ecx,ebp
        mov     ch,bl
        mov     [edi].p16_b[ecx*2],dx

        dec     ebp
        jns     @@l1

        dec     ebx
        jns     @@l0

@@weg:  ret
initrgb16 endp

initrgb24_32 proc near
        ;-> esi -> tvesa
        ;-> edi -> tpaltab32

        mov     ebx,(fvalues+xtrafvalues)-1 ;fog value (0 - fvalues-1)
@@l0:
        mov     ebp,255                 ;color value (0 - 255)
@@l1:
        mov     eax,255
        sub     eax,ebp
        imul    eax,ebx
        shr     eax,ld_fvalues
        add     eax,ebp
        cmp     eax,255
        jbe     @@clip
        mov     eax,255
@@clip:
                                        ;r
        mov     edx,eax
        mov     cl,[esi].vesa_redbits
        shl     edx,cl
        shr     edx,8
        mov     cl,[esi].vesa_redpos
        shl     edx,cl
        mov     ecx,ebp
        mov     ch,bl
        mov     [edi].p32_r[ecx*4],edx
                                        ;g
        mov     edx,eax
        mov     cl,[esi].vesa_greenbits
        shl     edx,cl
        shr     edx,8
        mov     cl,[esi].vesa_greenpos
        shl     edx,cl
        mov     ecx,ebp
        mov     ch,bl
        mov     [edi].p32_g[ecx*4],edx
                                        ;b
        mov     edx,eax
        mov     cl,[esi].vesa_bluebits
        shl     edx,cl
        shr     edx,8
        mov     cl,[esi].vesa_bluepos
        shl     edx,cl
        mov     ecx,ebp
        mov     ch,bl
        mov     [edi].p32_b[ecx*4],edx

        dec     ebp
        jns     @@l1

        dec     ebx
        jns     @@l0

@@weg:  ret
initrgb24_32 endp

initpaltab proc near
        ;-> esi -> tvesa

                ;calc palette in all light values
        push    esi
        mov     edi,scene.sc_paltab    ;edi -> tpaltab

        mov     ecx,lvalues+xtralvalues-1
@@l0:
        mov     ebx,scene.sc_pal
        xor     ebp,ebp
@@l1:
        xor     edx,edx
        mov     esi,2
@@l2:
        movzx   eax,byte ptr[ebx+esi]   ;g,b,r
        imul    eax,ecx
        shr     eax,ld_lvalues
        cmp     eax,255
        jb      @@clip
        mov     al,255
@@clip:
        shl     edx,8
        mov     dl,al
        dec     esi
        jns     @@l2
                                        ;edx = 0gbr
        mov     eax,ebp
        mov     ah,cl
        mov     [edi].p_pal[eax*4],edx

        add     ebx,3
        inc     ebp
        cmp     ebp,256
        jb      @@l1

        dec     ecx
        jns     @@l0
        pop     esi

                                        ;esi -> tvesa
                                        ;edi -> paltab
        cmp     [esi].vesa_pbytes,2;bpp,16
        ja      @@24_32
        call    initrgb16
        lea     eax,copybuffer16
        jmp     @@w

@@24_32:call    initrgb24_32
        lea     eax,copybuffer32
        cmp     [esi].vesa_pbytes,3;bpp,24
        ja      @@32
        lea     eax,copybuffer24
@@32:

@@w:    mov     copybuffer,eax

@@weg:  ret
endp

init1 proc near

                ;xmid & ymid
        fild    xres
        fmul    c_0_5
        fstp    xmid
        fild    yres
        fmul    c_0_5
        fstp    ymid


;divtab
        mov     eax,xres
        mov     edx,yres
        cmp     eax,edx
        ja      @@0
        mov     eax,edx
@@0:    inc     eax                     ;eax = max(xres,yres) +1

        mov     edi,scene.sc_divtab
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
        cmp     ecx,eax                 ;max(xres,yres) +1
        jbe     @@l
        fstp    st

;viewer
        mov     viewer.v_light,0
        mov     light.v_light,1

        ret
init1 endp


makespot proc proc pascal
        ;-> edi -> lmap
        ;st(0) = sin-scale

local   x, y, h

        mov     y,255
@@y_l:
        fild    y
        fmul    st,st(1);pi_256
        fsin
        fmul    c_lmap1

        mov     x,255
@@x_l:
        fild    x
        fmul    st,st(2);pi_256
        fsin
        fmul    st,st(1)
        fabs
        fistp   h

        mov     ebx,x
        mov     bh,byte ptr y

        mov     eax,h
;        cmp     eax,lvalues-1
;        jbe     @@0
;        mov     eax,lvalues-1
@@0:
;      mov al,lvalues-1
        mov     [edi+ebx],al

        dec     x
        jns     @@x_l

        fstp    st
        dec     y
        jns     @@y_l

        fstp    st

;  xor eax,eax
;  mov ecx,8
;  mov ebx,256*124
;@@t_l:
;  mov [edi+ebx+126],eax
;  mov [edi+ebx+130],eax
;  add ebx,256
;  dec ecx
;  jnz @@t_l
        ret
makespot endp

makewater proc pascal
local   x, y, p, h


        mov     p,7
@@p_l:
        fild    p
        fmul    pi_8


                ;pre-calculations
        mov     ebx,scene.sc_tempdata
        lea     esi,water_coeff

        mov     ecx,511
        mov     h,ecx
@@l0:
        fild    h
        cmp     ecx,255
        ja      @@0
                ;x dependent water
        fld     st(0)
        fmul    dword ptr [esi]
        fadd    st,st(2)
        fsin

        fld     st(1)
        fmul    dword ptr [esi+4]
        fsub    st,st(3)
        fsin

        faddp   st(1),st
        fstp    [ebx+ecx*4].w_x

                ;y dependent water
        fld     st(0)
        fmul    dword ptr [esi+8]
        fadd    st,st(2)
        fsin

        fld     st(1)
        fmul    dword ptr [esi+12]
        fsub    st,st(3)
        fsin

        faddp   st(1),st
        fstp    [ebx+ecx*4].w_y


                ;x and y dependent scale, scale = sin(fx)*sin(fy)*4.0
        fld     st(0)
        fmul    pi_256                  ;fx = pi*x/256.0, fy = pi*y/256.0
        fsin                            ;sin(fx)
        fmul    c_2
        fstp    [ebx+ecx*4].w_scale

@@0:

                ;x+y dependent water
        fld     st(0)
        fmul    dword ptr [esi+16]
        fadd    st,st(2)
        fsin

        fld     st(1)
        fmul    dword ptr [esi+20]
        fsub    st,st(3)
        fsin
        faddp   st(1),st
        fstp    [ebx+ecx*4].w_xpy

                ;x-y dependent water
        fld     st(0)
        fmul    dword ptr [esi+24]
        fadd    st,st(2)
        fsin

        fld     st(1)
        fmul    dword ptr [esi+28]
        fsub    st,st(3)
        fsin
        faddp   st(1),st
        fstp    [ebx+ecx*4].w_xmy


        fstp    st
        dec     h
        dec     ecx
        jns     @@l0



        mov     y,255
@@y_l:
        mov     x,255
@@x_l:


        mov     eax,x
        mov     edx,y

        fld     [ebx+eax*4].w_x
        fadd    [ebx+edx*4].w_y
        lea     esi,[eax+edx]
        fadd    [ebx+esi*4].w_xpy
        lea     esi,[eax+255]
        sub     esi,edx
        fadd    [ebx+esi*4].w_xmy
        fabs
                                        ;fx = pi*x/256.0 ,fy = pi*y/256.0
        fld     [ebx+eax*4].w_scale     ;scale = sin(fx)*sin(fy)*4.0;
        fmul    [ebx+edx*4].w_scale

        fld1                            ;if (scale > 1.0) scale = 1.0;
        fcomp   st(1)
        fstsw   ax
        sahf
        ja      @@1
        fstp    st
        fld1
@@1:
        fmulp   st(1),st
        fmul    c_wscale

        fistp   h

        mov     edx,p
        shl     edx,16
        mov     dl,byte ptr x
        mov     dh,byte ptr y

        mov     eax,h
        cmp     al,lvalues-1
        jbe     @@clip
        mov     al,lvalues-1
@@clip:
        mov     [edi+edx],al

        dec     x
        jns     @@x_l

        dec     y
        jns     @@y_l

        fstp    st
        dec     p
        jns     @@p_l

        ret
makewater endp

makefogmap proc pascal
local   x,h,val


                ;zxw & zyw
        fld     z_width
        fld     xmid
        fmul    st,st(1)
        fstp    zxw
        fld     ymid
      fmul c_1_33
        fmulp   st(1),st
        fstp    zyw

                ;fogmaps
        mov     x,fogmapsize-1
        mov     edi,scene.sc_fogmap
@@x_l:

        fild    x
        fmul    c_fogmap1
        fsin
        fadd    c_fogadd
        fmul    c_fogmul

        mov     h,fogmapsteps           ;intensity steps
@@h_l:
        mov     ebx,h
        dec     ebx
        imul    ebx,fogmapsize
        add     ebx,x

        fild    h
        fmul    st,st(1)
        fistp   val
        mov     eax,val
;    mov al,63
        mov     [edi+ebx],al

        dec     h
        jnz     @@h_l

        fstp    st
        dec     x
        jns     @@x_l

        ret
makefogmap endp

makefogwater proc near ;animation table for fogmaps
local   p,x,val

        mov     edi,scene.sc_fogwater

        mov     p,31
@@p_l:
        fild    p
        fmul    pi_32

        mov     x,zxres-1
@@x_l:

        fild    x
        fmul    c_fwsin1
        fadd    st,st(1)
        fsin

        fild    x
        fmul    c_fwsin2
        fsub    st,st(2)
        fsin

        faddp   st(1),st
        fabs
        fadd    c_fwadd
        fmul    c_fwmul

        fistp   val
        mov     eax,val
        mov     [edi],al
        inc     edi

        dec     x
        jns     @@x_l

        fstp    st

        dec     p
        jns     @@p_l

@@weg:  ret
makefogwater endp

makeflares proc pascal
local x, y, c, z

        mov     edi,scene.sc_flares

        mov     y,63
@@y_l:  mov     x,63
@@x_l:

        mov     ebx,64*256+64
        add     bl,byte ptr x
        add     bh,byte ptr y

        fild    x
        fmul    st,st(0)
        fild    y
        fmul    st,st(0)
        faddp   st(1),st
        fsqrt
        fld     st

                ;flare 1
        mov     c,53
        fisub   c
        fabs
        fchs
        mov     c,10
        fiadd   c
        ftst
        fstsw   ax
        sahf
        jb      @@0
        fmul    c_2
        fist    c

        mov     al,byte ptr c
        mov     [edi+ebx],al

@@0:    fstp    st

                ;flare 2

        mov     c,50;63
        fisub   c
        fchs
        ftst
        fstsw   ax
        sahf
        jb      @@1
        fist    c

        mov     al,byte ptr c
;        shr     al,1
        cmp     al,20
        jb      @@2
        mov     al,20
@@2:
        mov     [edi+ebx+128],al

@@1:    fstp    st

        dec     x
        cmp     x,-64
        jge     @@x_l

        dec     y
        cmp     y,-64
        jge     @@y_l

;comment ^


        mov     z,15
@@z_l:
        mov     c,2*256
        mov     ebx,z
        fld     flaretab[ebx*4]
        fsincos
        fimul   c
        fistp   x
        fimul   c
        fistp   y


        mov     ecx,32
        xor     ebx,ebx
        mov     edx,64*256+64
@@l:
        mov     eax,x
        add     bl,al
        adc     dl,ah
        mov     eax,y
        add     bh,al
        adc     dh,ah

        cmp     dl,2
        jl      @@w
        cmp     dl,127
        jge     @@w
        cmp     dh,2
        jl      @@w
        cmp     dh,127
        jge     @@w

        add     dword ptr [edi+edx-512-2 +128],01020201h
        add     dword ptr [edi+edx-256-2 +128],02050503h
        add     dword ptr [edi+edx-  0-2 +128],02060603h
        add     dword ptr [edi+edx+256-2 +128],01020201h

        dec     ecx
        jnz     @@l
@@w:
        dec     z
        jns     @@z_l
;^
        ret
makeflares endp

makeflaretab proc near

        mov     eax,xres
        xor     edx,edx
        mov     ebx,266;401
        div     ebx
;        inc     eax
        mov     flare_dx,eax

        mov     eax,yres
        xor     edx,edx
        mov     ebx,200;301
        div     ebx
;        inc     eax
        imul    eax,xres
        mov     flare_dy,eax

        mov     eax,xres
        imul    eax,yres
        mov     flare_ycmp,eax

        mov     edi,scene.sc_flaretab
        add     edi,9*64-1

        mov     edx,8
@@h_l:
        mov     ecx,63
@@v_l:
        mov     eax,edx
        imul    eax,ecx
        shr     eax,3                   ;/8

        mov     [edi],al
        dec     edi

        dec     ecx
        jns     @@v_l

        dec     edx
        jns     @@h_l

        ret
makeflaretab endp

clearbuffer proc near
        mov     edi,scene.sc_buffer
        mov     ecx,xres
        imul    ecx,yres
        xor     eax,eax
        rep     stosd

@@weg:  ret
clearbuffer endp


copybuffer16 proc near
        mov     esi,scene.sc_buffer
        mov     edi,linbuf

        mov     ebx,scene.sc_paltab
        xor     edx,edx
        mov     ecx,yres
@@y_l:
        push    ecx edi

        mov     ecx,xres
        shr     ecx,1
@@x_l:
        movzx   eax,word ptr[esi+4]     ;eax = 00lt
        mov     dh,[esi+2+4]            ;dh = f
        mov     eax,[ebx].p_pal[eax*4]  ;eax = 0bgr

        mov     dl,al                   ;rot
        mov     bp,[ebx].p16_r[edx*2]

        mov     dl,ah                   ;grn
        shr     eax,8
        or      bp,[ebx].p16_g[edx*2]

        mov     dl,ah                   ;blau
        or      bp,[ebx].p16_b[edx*2]

        shl     ebp,16

        movzx   eax,word ptr[esi]       ;eax = 00lt
        mov     dh,[esi+2]              ;dh = f
        mov     eax,[ebx].p_pal[eax*4]  ;eax = 0bgr

        mov     dl,al                   ;rot
        mov     bp,[ebx].p16_r[edx*2]

        mov     dl,ah                   ;grn
        shr     eax,8
        or      bp,[ebx].p16_g[edx*2]

        mov     dl,ah                   ;blau
        or      bp,[ebx].p16_b[edx*2]

        add     esi,8
        mov     [edi],ebp
        add     edi,4
        dec     ecx
        jnz     @@x_l

        pop     edi ecx
        add     edi,xbytes
        dec     ecx
        jnz     @@y_l

        ret
copybuffer16 endp

copybuffer24 proc near
        mov     esi,scene.sc_buffer
        mov     edi,linbuf

        mov     ebx,scene.sc_paltab
        xor     edx,edx

        mov     ecx,yres
@@y_l:
        push    ecx edi

        mov     ecx,xres
        shr     ecx,2
        push    ecx                     ;x-counter on stack
@@x_l:
                ;1.
        movzx   eax,word ptr[esi]       ;eax = 00lt
        mov     dh,[esi+2]              ;dh = f
        mov     eax,[ebx].p_pal[eax*4]  ;eax = 0gbr

        mov     dl,al                   ;rot
        mov     ebp,[ebx].p32_r[edx*4]

        mov     dl,ah                   ;grn
     add     esi,4
        shr     eax,8
        or      ebp,[ebx].p32_g[edx*4]

        mov     dl,ah                   ;blau
        or      ebp,[ebx].p32_b[edx*4]
                ;-
                ;2.
        movzx   eax,word ptr[esi]       ;eax = 00lt
        mov     dh,[esi+2]              ;dh = f
        mov     eax,[ebx].p_pal[eax*4]  ;eax = 0gbr

        mov     dl,al                   ;rot
     shl     ebp,8
        mov     ecx,[ebx].p32_r[edx*4]

        mov     dl,ah                   ;grn
     add     esi,4
        shr     eax,8
        or      ecx,[ebx].p32_g[edx*4]

        mov     dl,ah                   ;blau
        or      ecx,[ebx].p32_b[edx*4]
                ;--
                   ;3.
           movzx   eax,word ptr[esi]       ;eax = 00lt
           mov     dh,[esi+2]              ;dh = f
        shrd    ebp,ecx,8
           mov     eax,[ebx].p_pal[eax*4]  ;eax = 0gbr
        mov     [edi],ebp
           mov     dl,al                   ;rot
        add     edi,4

        mov     ebp,[ebx].p32_r[edx*4]

        mov     dl,ah                   ;grn
     add     esi,4
        shr     eax,8
        or      ebp,[ebx].p32_g[edx*4]

        mov     dl,ah                   ;blau
     shl     ecx,8
        or      ebp,[ebx].p32_b[edx*4]
                ;--
                   ;4.
           movzx   eax,word ptr[esi]       ;eax = 00lt
           mov     dh,[esi+2]              ;dh = f
        shrd    ecx,ebp,16
           mov     eax,[ebx].p_pal[eax*4]  ;eax = 0gbr
        mov     [edi],ecx
           mov     dl,al                   ;rot
        add     edi,4

        mov     ecx,[ebx].p32_r[edx*4]

        mov     dl,ah                   ;grn
     shl     ebp,8
        shr     eax,8
        or      ecx,[ebx].p32_g[edx*4]

        mov     dl,ah                   ;blau
     add     esi,4
        or      ecx,[ebx].p32_b[edx*4]


        shld    ecx,ebp,8
        mov     [edi],ecx
        add     edi,4


        dec     dword ptr [esp]
        jnz     @@x_l
        pop     ecx                     ;remove x-counter

        pop     edi ecx
        add     edi,xbytes
        dec     ecx
        jnz     @@y_l

        ret
copybuffer24 endp

copybuffer32 proc near
        mov     esi,scene.sc_buffer
        mov     edi,linbuf

        mov     ebx,scene.sc_paltab
        xor     edx,edx

        mov     ecx,yres
@@y_l:
        push    ecx edi
        mov     ecx,xres
@@x_l:
        movzx   eax,word ptr[esi]       ;eax = 00lt
        mov     dh,[esi+2]              ;dh = f
        mov     eax,[ebx].p_pal[eax*4]  ;eax = 0gbr

        mov     dl,al                   ;rot
        mov     ebp,[ebx].p32_r[edx*4]

        mov     dl,ah                   ;grn
        shr     eax,8
        or      ebp,[ebx].p32_g[edx*4]

        mov     dl,ah                   ;blau
        or      ebp,[ebx].p32_b[edx*4]

        add     esi,4
        mov     [edi],ebp
        add     edi,4
        dec     ecx
        jnz     @@x_l

        pop     edi ecx
        add     edi,xbytes
        dec     ecx
        jnz     @@y_l

        ret
copybuffer32 endp



trigger proc pascal
        ;st(0) = new value
        ;edi -> old value
local   temp

        fistp   temp
        mov     edx,[edi]
        mov     eax,temp

        cmp     eax,edx
        jle     @@0
        dec     eax
@@0:
        or      eax,eax
        jns     @@2
        xor     eax,eax
@@2:    mov     [edi],eax
@@weg:  ret
trigger endp

initmdata proc near
        ;-> esi -> tvesa
        ;-> eax = debug
        cld
        push    ebp
     mov     debug,eax

        mov     eax,[esi].vesa_xbytes
        mov     xbytes,eax
        mov     eax,[esi].vesa_xres
        mov     xres,eax
        mov     eax,[esi].vesa_yres
        mov     yres,eax
        mov     eax,[esi].vesa_linbuf
        mov     linbuf,eax
        call    initpaltab              ;esi -> tvesa

        call    init1
        call    makelgouraud

        mov     edi,scene.sc_spot1
        fld     pi_256
        call    makespot
        mov     edi,scene.sc_spot2
        fld     pi_128
        call    makespot
        mov     edi,scene.sc_water
        call    makewater

        call    makefogmap
        call    makefogwater

        call    makeflares
        call    makeflaretab

        pop     ebp
@@weg:  ret
initmdata endp

startmese proc near
        ;-> eax = startframe
        push    ebp
        cld
comment #
    cmp debug,0
    jne @@t1
    or eax,eax
    jz @@t1

    push eax
    call i8_done
    pop eax
    push eax
@@t0:call int_8t
    dec eax
    jnz @@t0
    call i8_init
    pop eax
@@t1:
#

        mov     edx,maincount
    sub edx,eax
        mov     mainstart,edx

@@nextframe:
; jmp @@t0

@@l2:           ;update tracks
        mov     eax,maincount
        sub     eax,mainstart
        mov     edx,eax
        sub     edx,frame
        cmp     edx,128;framemul;
        jl      @@0
        add     frame,128;framemul;
        mov     esi,scene.sc_root
        call    dotracks
        jmp     @@l2
@@0:
        mov     frame,eax

                ;frame-init
        mov     eax,scene.sc_idxlist
        mov     scene.sc_idxlistend,eax

                ;ambient track
        lea     edi,ambient_f
        call    dotrack pascal, scene.sc_ambient, 2

        fld     ambient_f[0]
        fistp   ambient
;        lea     edi,ambient
;        call    trigger
;mov ambient,0;63
        fld     ambient_f[4]
        fistp   fog
;  mov fog, 120
;        lea     edi,fog
;        call    trigger

                ;calculate tracks
        mov     esi,scene.sc_root
        call    dotracks

        mov     esi,scene.sc_cam
;fld c_0_7
;fstp [esi].c_FOV
        lea     edi,viewer
        call    doviewer
        call    adjustcamera

        lea     esi,viewer              ;cubetree bzgl. beobachter
        mov     edi,scene.sc_tempdata
        call    cubetree pascal, 35;25

      ;       call    clearbuffer
        call    texturedraw
        call    dolights

cmp scene.sc_intro,0
je @@ex
; credits
  mov eax,frame
  call docredits pascal, scene.sc_tempdata
  or eax,eax ;eax = -1 : end
  js @@wait
  add mainstart,eax
;
jmp @@ex0
@@ex:
    cmp frame,76500;71500
    jg @@w
@@ex0:

        call    copybuffer


cmp debug,0     ;debug = 1
jg @@nextframe
jl @@d0
                ;debug = 0: normal
;    @@t0:
        call    keypressed
        or      eax,eax
        jz      @@nextframe
;        mov     ah,1
;        int     16h
;        jz      @@nextframe
        jmp     @@w

@@d0:           ;debug = -1: single step
        xor     eax,eax
        int     16h
        mov     edx,debug
        neg     edx
        add     maincount,edx
        cmp     al,27                   ;esc?
        jne     @@nextframe


@@wait:
        mov     eax,maincount
        mov     mainstart,eax
@@w_l:
        call    keypressed
        or      eax,eax
;        mov     ah,1
;        int     16h
        jnz     @@w
        mov     eax,maincount
        sub     eax,mainstart
        cmp     eax,5000
        jl      @@w_l
@@w:
;        mov     ax,3
;        int     10h
        pop     ebp

        ret
startmese endp

end