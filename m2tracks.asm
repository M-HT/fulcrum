.486					;pmode/w
.model flat,prolog

locals


NULL = 0

include int.inc
include tracks.inc

include m2struct.inc

;public frame

public dotracks, initframecount, calcframecount
public calcbumpomni
public m_dotracksM2, c_dotracksM2, t_dotracksM2, i_dotracksM2, d_dotracksM2

public matrixmul, doviewer


;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
;struc
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
d equ dword ptr

tvec struc
 x1 dd ?
 x2 dd ?
 x3 dd ?
ends

;matrix
tmatrix struc
 A_1n label dword
 A_m1 label dword
 A_11		dd ?
 A_m2 label dword
 A_12		dd ?
 A_m3 label dword
 A_13		dd ?
 A_2n label dword
 A_21		dd ?
 A_22		dd ?
 A_23		dd ?
 A_3n label dword
 A_31		dd ?
 A_32		dd ?
 A_33		dd ?
ends

;VMT
tobjvmt struc
 o_size 	dd ?
 o_readdata	dd ?
 o_firstobj	dd ?
 o_nextobj	dd ?
 o_dotracks	dd ?
 o_insert	dd ?
ends

;object:
;-------
ofMatrix    = 1
ofMesh	    = 2
ofAbsolute  = 4

tobject struc
 o_vmt		dd ?

 o_flags	dd ?

 ;hierarchy-system: (pointer)
 o_owner	dd ?
 o_next 	dd ?
 o_child	dd ?

 ;state: (vektor/matrix)
 o_p		dd 3 dup(?)		;actual position (position and matrix have fixed order in memory)
 o_A		dd 9 dup(?)		;actual rotation and scale
 o_hidden	dd ?

ends


;mesh:
;-----
mfMorph 	= 8
mfRGBPreCalc	= 16		   ;G
mfBumpPreCalc	= 32		   ;B
mfDistance	= 64		   ;Z


tmesh struc
 m_object	tobject ?

 m_PolygonObject CMV2PolygonObject ?

 ;postrack (3):
 m_postrack	dd ?			;pointer

 ;rotationtrack (4):
 m_rottrack	dd ?			;pointer

 ;scaletrack (3):
; m_scaletrack	 dd ?			;pointer

 ;morphtrack (2*vertices*3):
 m_morphtrack	dd ?			;pointer
; m_nmorphtrack  dd ?

 m_rgbtrack   dd ?			;pointer

 ;hidetrack
 m_hidetrack	dd ?, ? 		;pointer, int key

ends


;camera:
;-------
tcamera struc
 c_object	tobject ?

 c_target	dd ?			;pointer

 ;postrack (3):
 c_postrack	dd ?			;pointer

 ;FOVtrack (1):
 c_FOVtrack	dd ?			;pointer
 c_FOV		dd ?			;float

 ;rolltrack (1):
 c_rolltrack	dd ?			;pointer
 c_roll 	dd ?			;float
ends

;target:
;-------
ttarget struc
 t_object	tobject ?

 ;postrack (3):
 t_postrack	dd ?			;pointer
ends


;omni:
;-----
tomni struc
 i_object	tobject ?

 ;postrack (3):
 i_postrack	dd ?			;pointer

 ;coltrack (3):
 i_coltrack	dd ?			;pointer
 i_color	dd 3 dup(?)

 ;hidetrack
 i_hidetrack	dd ?, ? 		;pointer, int key
ends

;dummy:
;------
tdummy struc
 d_object	tobject ?

 d_special	dd ?

 ;position
 d_x		dd ?,?,?

 ;hidetrack
 d_hidetrack	dd ?, ? 		;pointer, int key

 d_firstframe	dd ?
ends


tbumpomni struc
 ;color track (3):
 bo_coltrack	dd ?			;pointer
 bo_color	dd 3 dup(?)

 ;multiplier track (1):
 bo_multtrack	dd ?			;pointer
 bo_multiplier	dd ?

 ;hidetrack
 bo_hidetrack	dd ?, ? 		;pointer, int key
 bo_hidden	dd ?
ends



;linear track:
;-------------
tltrack struc
 lt_startstate	dd ?
 lt_startframe	dd ?
 lt_endstate	dd ?
 lt_endframe	dd ?
 lt_keys	dd ?
 lt_data	dd ?
ends


;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
.data
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±

c_pi_2 dd 1.5707963
c_1_33 dd 1.3333333

;frame dd ?
mainstart dd ?
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
.code
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±

doltrackM2 proc pascal
arg	_track:dword, typesize:dword
	;edi -> result
local	temp

	push	esi

	mov	ebx,_track
@@r:
	mov	edx,frame
	cmp	edx,[ebx].lt_endframe
	jbe	@@i
		;end of key
	cmp	[ebx].lt_keys,0
	ja	@@next
		;end of track
	fld1
	jmp	@@e

@@next: 	;get next key
	mov	eax,[ebx].lt_endframe
	mov	[ebx].lt_startframe,eax ;old endframe is new startframe

	dec	[ebx].lt_keys

	mov	esi,[ebx].lt_data
	movsx	eax,byte ptr [esi]	;get scale value
	inc	esi
	mov	temp,eax
	fild	temp

	mov	edx,[ebx].lt_endstate
	xor	ecx,ecx
@@l:

	movsx	eax,byte ptr [esi+ecx]	;get differences
	mov	temp,eax
	fild	temp

	fscale

		mov	eax,[ebx].lt_startstate

	fld	d [edx+ecx*4]
	fadd	d [eax+ecx*4]
	fstp	d [eax+ecx*4]
	fstp	d [edx+ecx*4]


	inc	ecx
	cmp	ecx,typesize
	jb	@@l

	fstp	st

	mov	eax,[esi+ecx]		;get endframe
	mov	[ebx].lt_endframe,eax

	lea	eax,[esi+ecx+4] 	;update data pointer
	mov	[ebx].lt_data,eax
	jmp	@@r

@@i:		;interpolate		;edx = frame
	mov	eax,[ebx].lt_startframe
	sub	edx,eax 		;edx = (frame - sframe)
	mov	temp,edx
	fild	temp
	neg	eax
	add	eax,[ebx].lt_endframe	;eax = (eframe - sframe)
	mov	temp,eax
	fild	temp
	fdivp	st(1),st		;t = (frame - sframe) / (eframe - sframe);
@@e:
	mov	esi,[ebx].lt_startstate
	mov	edx,[ebx].lt_endstate

	xor	ecx,ecx
@@l2:
	fld	d [edx+ecx*4]
	fmul	st,st(1)
	fadd	d [esi+ecx*4]
	fstp	[edi].CMV2Dot3DPos__m_Pos__m_fX ;x1
	inc	ecx

		fld	d [edx+ecx*4]
	fmul	st,st(1)
	fadd	d [esi+ecx*4]
	fstp	[edi].CMV2Dot3DPos__m_Pos__m_fY ;x2
	inc	ecx

	fld	d [edx+ecx*4]
	fmul	st,st(1)
	fadd	d [esi+ecx*4]
	fstp	[edi].CMV2Dot3DPos__m_Pos__m_fZ ;x3
	inc	ecx

	add	edi,CMV2Dot3DPos__Size

		cmp	ecx,typesize
		jb	@@l2

		fstp	st

		pop	esi

		ret
doltrackM2 endp





dolRGBtrackM2 proc pascal
arg	_track:dword, typesize:dword
	;edi -> result
local	temp

	push	esi

	mov	ebx,_track
@@r:
	mov	edx,frame
	cmp	edx,[ebx].lt_endframe
	jbe	@@i
		;end of key
		cmp	[ebx].lt_keys,0
	ja	@@next
				;end of track
		fld1
		jmp	@@e

@@next: 	;get next key
		mov	eax,[ebx].lt_endframe
	mov	[ebx].lt_startframe,eax ;old endframe is new startframe

	dec	[ebx].lt_keys

	mov	esi,[ebx].lt_data
	movsx	eax,byte ptr [esi]	;get scale value
	inc	esi
	mov	temp,eax
	fild	temp

	mov	edx,[ebx].lt_endstate
	xor	ecx,ecx
@@l:

	movsx	eax,byte ptr [esi+ecx]	;get differences
	mov	temp,eax
	fild	temp

	fscale

	mov	eax,[ebx].lt_startstate

		fld	d [edx+ecx*4]
	fadd	d [eax+ecx*4]
	fstp	d [eax+ecx*4]
	fstp	d [edx+ecx*4]


	inc	ecx
	cmp	ecx,typesize
	jb	@@l

	fstp	st

	mov	eax,[esi+ecx]		;get endframe
		mov	[ebx].lt_endframe,eax

	lea	eax,[esi+ecx+4] 	;update data pointer
	mov	[ebx].lt_data,eax
	jmp	@@r

@@i:		;interpolate		;edx = frame
		mov	eax,[ebx].lt_startframe
	sub	edx,eax 		;edx = (frame - sframe)
	mov	temp,edx
	fild	temp
	neg	eax
	add	eax,[ebx].lt_endframe	;eax = (eframe - sframe)
		mov	temp,eax
		fild	temp
		fdivp	st(1),st		;t = (frame - sframe) / (eframe - sframe);
@@e:
		mov	esi,[ebx].lt_startstate
		mov	edx,[ebx].lt_endstate

		xor	ecx,ecx
@@l2:
		fld	d [edx+ecx*4]
		fmul	st,st(1)
		fadd	d [esi+ecx*4]
;	      fchs
		fstp	[edi].CMV2Dot3DPos__m_fR
		inc	ecx

		fld	d [edx+ecx*4]
		fmul	st,st(1)
		fadd	d [esi+ecx*4]
		fstp	[edi].CMV2Dot3DPos__m_fG
		inc	ecx

		fld	d [edx+ecx*4]
		fmul	st,st(1)
		fadd	d [esi+ecx*4]
		fstp	[edi].CMV2Dot3DPos__m_fB
		inc	ecx

		add	edi,CMV2Dot3DPos__Size

		cmp	ecx,typesize
		jb	@@l2

		fstp	st

		pop	esi

		ret
dolRGBtrackM2 endp






m_dotracksM2 proc near
	;-> esi -> tmesh

		;hidetrack
	lea	ebx,[esi].m_hidetrack
	lea	edi,[esi].o_hidden
	call	dohidetrack


	test	[esi].o_flags,ofAbsolute
	jnz	@@abs

			;rotation track (4)
	lea	edi,o_A[esi]
	call	dorottrack pascal, [esi].m_rottrack

			;position track (3)
	lea	edi,[esi].o_p
	call	dotrack pascal, [esi].m_postrack, 3

	cmp	[esi].o_hidden,0
	ja	@@weg
	jmp	@@abs0

@@abs:	;absolute
	cmp	[esi].o_hidden,0
	ja	@@weg

			;position track (3)
	lea	edi,[esi].o_p
	call	doltrack pascal, [esi].m_postrack, 3


			;morphtrack (2*vertices*3)
	test	[esi].o_flags,mfMorph
	jz	@@m0

	mov	edi,[esi].m_PolygonObject.CMV2PolygonObject__m_pDot3DPos
	mov	edx,[esi].m_PolygonObject.CMV2PolygonObject__m_iNumDot3DPos
	lea	edx,[edx*2+edx] 	;edx*3
	call	doltrackM2 pascal, [esi].m_morphtrack, edx
@@m0:

@@abs0:
	test	[esi].o_flags,mfRGBPreCalc or mfBumpPreCalc
	jz	@@NotRGBPreCalc

	mov	edi,[esi].m_PolygonObject.CMV2PolygonObject__m_pDot3DPos
	mov	edx,[esi].m_PolygonObject.CMV2PolygonObject__m_iNumDot3DPos
	lea	edx,[edx*2+edx] 	;edx*3
	call	dolRGBtrackM2 pascal, [esi].m_rgbtrack, edx

@@NotRGBPreCalc:

@@weg:	ret
m_dotracksM2 endp


c_dotracksM2 proc near

				;position track (3)
		lea	edi,[esi].o_p
		call	dotrack pascal, [esi].c_postrack, 3;eax, 3

				;FOV track (1)
		lea	edi,[esi].c_FOV
		call	dotrack pascal, [esi].c_FOVtrack, 1;eax, 1

				;roll track (1)
	lea	edi,[esi].c_roll
	call	dotrack pascal, [esi].c_rolltrack, 1;eax, 1

		;no matrix
	ret
c_dotracksM2 endp


t_dotracksM2 proc near
	;-> esi -> ttarget

		;position track (3)
	lea	edi,[esi].o_p
	call	dotrack pascal, [esi].t_postrack, 3

		;no matrix
@@weg:	ret
t_dotracksM2 endp

i_dotracksM2 proc near

		;position track (3)
	lea	edi,[esi].o_p
	call	dotrack pascal, [esi].i_postrack, 3

		;color track (3)
	lea	edi,[esi].i_color
	call	dotrack pascal, [esi].i_coltrack, 3

		;hidetrack
	lea	ebx,[esi].i_hidetrack
	lea	edi,[esi].o_hidden
	call	dohidetrack

		;no matrix
@@weg:	ret
i_dotracksM2 endp

d_dotracksM2 proc near

		;hidetrack
	lea	ebx,[esi].d_hidetrack
	lea	edi,[esi].o_hidden
	call	dohidetrack

		;no matrix
@@weg:	ret
d_dotracksM2 endp


doviewer proc near
	;-> esi -> tcamera
	;-> edi -> CMV2Camera
	;<- esi -> tcamera
	;<- edi -> CMV2Camera


	mov	ebx,c_target[esi]

		;direction vector u to target
	fld	o_p.x1[esi]
	fst	[edi].CMV2Camera__m_Pos__m_fX
	fsubr	o_p.x1[ebx]
	fld	o_p.x2[esi]
	fst	[edi].CMV2Camera__m_Pos__m_fY
	fsubr	o_p.x2[ebx]
	fld	o_p.x3[esi]
	fst	[edi].CMV2Camera__m_Pos__m_fZ
	fsubr	o_p.x3[ebx]

		;calculate local coordinate system l1, l2, l3 (l3 = view axis)
					;l3 = u/|u|
					;l1 = ((u) x (0,0,1))/|(u) x (0,0,1)|
					;   = (u2,-u1,0)/sqrt(u1^2 + u2^2)
					;l2 = l1 x l3  = | a2*b3	|
					; (for l1 = a)	 |-a1*b3	|
					; (and l2 = b)	 | a1*b2 - a2*b1|

	fld	st(2)			;make unit length
	fmul	st,st(0)		;u1*u1
	fld	st(2)
	fmul	st,st(0)		;u2*u2
	faddp	st(1),st
	fld	st(1)			;st(1) = (u1^2 + u2^2)
	fmul	st,st(0)		;u3*u3
	fadd	st,st(1)
	fsqrt				;sqrt(u1^2 + u2^2 + u3^2) (length of u)

		;l3			;l3 = u/|u|
	fld	st(4)
	fdiv	st,st(1)
;      fchs
	fstp	[edi].CMV2Camera__m_Front__m_fX
	fld	st(3)
	fdiv	st,st(1)
;      fchs
	fstp	[edi].CMV2Camera__m_Front__m_fY
	fdivr	st,st(2)
;      fchs
	fstp	[edi].CMV2Camera__m_Front__m_fZ

		;l1			;l1 = (u2,-u1,0)/sqrt(u1^2 + u2^2)
	fsqrt				;sqrt(u1^2 + u2^2)

	fld	st(2)			;u2/sqrt(u1^2 + u2^2)
	fdiv	st,st(1)
	fstp	[edi].CMV2Camera__m_Right__m_fX

	fdivp	st(3),st		;-u1/sqrt(u1^2 + u2^2)
	fstp	st			;remove u3
	fstp	st			;remove u2
	fchs
	fstp	[edi].CMV2Camera__m_Right__m_fY

	fldz				;0
	fstp	[edi].CMV2Camera__m_Right__m_fZ

		;l2			;l2 = l1 x l3
	fld	[edi].CMV2Camera__m_Front__m_fX 	   ;b1
	fld	[edi].CMV2Camera__m_Right__m_fY 	   ;a2
	fld	[edi].CMV2Camera__m_Front__m_fY 	   ;b2
	fld	[edi].CMV2Camera__m_Right__m_fX 	   ;a1
	fld	[edi].CMV2Camera__m_Front__m_fZ 	   ;b3

	fld	st(3)
	fmul	st,st(1)
	fstp	[edi].CMV2Camera__m_Down__m_fX		  ;a2*b3

	fmul	st,st(1)
	fchs
	fstp	[edi].CMV2Camera__m_Down__m_fY		  ;-a1*b3

	fmulp	st(1),st
	fxch	st(1)
	fmulp	st(2),st
	fsubrp	st(1),st
	fstp	[edi].CMV2Camera__m_Down__m_fZ		  ;a1*b2-a2*b1

		;calc l1 and l2 scale factors for camera opening angle
	fld	c_pi_2			;1/tan(FOV) = tan(pi/2 - FOV)
	fsub	c_FOV[esi]		;FOV = halve camera view angle
	fptan
	fstp	st
	fld	st
	fmul	c_1_33
;	 fmulp	 st(2),st		 ;st(0) = (x:y-ratio) * 1/tan(FOV)

		;calc l1 and l2 scale factors for rotation about the view axis
	fld	c_roll[esi]
	fsincos

		;rotate & scale l1 and l2
	mov	ecx,2
@@l:
		;l1' = (l1*cos(roll) + l2*sin(roll))/tan(FOV)
	fld	[edi].CMV2Camera__m_Right__m_fX[ecx*4]
	fmul	st,st(1)		;cos
	fld	[edi].CMV2Camera__m_Down__m_fX[ecx*4]
	fmul	st,st(3)		;sin
	faddp	st(1),st
	fmul	st,st(4)		;1/tan(FOV)
		;l2' = (l2*cos(roll) + l1*sin(roll))*(x:y ratio)/tan(FOV)
	fld	[edi].CMV2Camera__m_Down__m_fX[ecx*4]
	fmul	st,st(2)		;cos
	fld	[edi].CMV2Camera__m_Right__m_fX[ecx*4]
	fmul	st,st(4)		;sin
	fsubp	st(1),st
	fmul	st,st(4)		;(x:y ratio)/tan(FOV)

	fchs
	fstp	[edi].CMV2Camera__m_Down__m_fX[ecx*4]
	fstp	[edi].CMV2Camera__m_Right__m_fX[ecx*4]
	dec	ecx
	jns	@@l

	fstp	st
	fstp	st
	fstp	st
	fstp	st
	ret
doviewer endp

matrixmul proc near
	;-> esi -> tobject
	;-> edx -> CMV2Camera
	;-> edi -> CMV2Camera

	test	[esi].o_flags,ofAbsolute
	jnz	@@abs
		;calculate matrix A = Av * Ao (v:viewer, o:object)
	xor	ebx,ebx 		;ebx = n
@@l0:
	fld	[esi].o_A[ebx*4].A_1n	;n-th column of A
	fld	[esi].o_A[ebx*4].A_2n
	fld	[esi].o_A[ebx*4].A_3n

	xor	ecx,ecx 		;ecx = m
@@l1:
	fld	[edx].CMV2Camera__LabelFirstVector[ecx].A_m1 ;* m-th row of Al
	fmul	st,st(3)
	fld	[edx].CMV2Camera__LabelFirstVector[ecx].A_m2
	fmul	st,st(3)
	fld	[edx].CMV2Camera__LabelFirstVector[ecx].A_m3
	fmul	st,st(3)
	faddp	st(1)
	faddp	st(1)
	lea	eax,[ebx*4+ecx]
	fstp	[edi].CMV2Camera__LabelFirstVector[eax]      ;= new element (m,n) of A

	add	ecx,3*4
	cmp	ecx,3*3*4
	jb	@@l1

	fstp	st
	fstp	st
	fstp	st

	inc	ebx
	cmp	ebx,3
	jb	@@l0

	jmp	@@abs0

@@abs:		;absolute: matrix A = Av (only position valid)
	mov	ecx,9-1
	lea	ebx,[edx].CMV2Camera__LabelFirstVector
@@l:	mov	eax,[ebx+ecx*4]
	mov	[edi].CMV2Camera__LabelFirstVector[ecx*4],eax
	dec	ecx
	jns	@@l

@@abs0:
					;p = Av*(po-pv)
	fld	[esi].o_p.x1		;(po-pv)
	fsub	[edx].CMV2Camera__m_Pos__m_fX
	fld	[esi].o_p.x2
	fsub	[edx].CMV2Camera__m_Pos__m_fY
	fld	[esi].o_p.x3
	fsub	[edx].CMV2Camera__m_Pos__m_fZ

	xor	ebx,ebx 		;ebx = m
@@l2:
	lea	ecx,[ebx+ebx*2]
	fld	[edx].CMV2Camera__LabelFirstVector[ecx*4].A_m1 ;* m-th row of Av
	fmul	st,st(3)
	fld	[edx].CMV2Camera__LabelFirstVector[ecx*4].A_m2
	fmul	st,st(3)
	fld	[edx].CMV2Camera__LabelFirstVector[ecx*4].A_m3
	fmul	st,st(3)
	faddp	st(1)
	faddp	st(1)
	fstp	[edi].CMV2Camera__m_Pos__m_fX[ebx*4]	       ;= new element (m,1) of p

	inc	ebx
	cmp	ebx,3
	jb	@@l2

	fstp	st
	fstp	st
	fstp	st

@@weg:	ret
matrixmul endp


dotracks proc near
	;-> esi = *this
	;<- esi = NULL
@@next:
	or	esi,esi
	jz	@@weg

	mov	ebx,o_vmt[esi]		;ebx -> vmt
	call	o_dotracks[ebx] 	;virtual method to calculate the tracks

					;A = matrix, p = position
					;o = owner

					;A = Ao*A (matrix multiplication)
	mov	edi,o_owner[esi]
	or	edi,edi
	jz	@@n			;no owner

	test	o_flags[esi],ofAbsolute
	jnz	@@n

	test	o_flags[esi],ofMatrix
	jz	@@pos

	xor	ebx,ebx 		;ebx = n
@@l2:
	fld	o_A[esi + ebx]		;n-th column of A
	fld	o_A[esi + ebx+3*4]
	fld	o_A[esi + ebx+6*4]

	xor	edx,edx 		;edx = m
@@l1:
	fld	o_A[edi + edx]		;* m-th row of Ao
	fmul	st,st(3)
	fld	o_A[edi + edx+1*4]
	fmul	st,st(3)
	fld	o_A[edi + edx+2*4]
	fmul	st,st(3)
	faddp	st(1)
	faddp	st(1)
	lea	eax,[ebx+edx]
	fstp	o_A[esi + eax]		;= new element (m,n) of A

	add	edx,3*4
	cmp	edx,3*3*4
	jb	@@l1

	fstp	st
	fstp	st
	fstp	st

	add	ebx,4
	cmp	ebx,3*4
	jb	@@l2
@@pos:					;p = Ao*p + po

	fld	o_p[esi].x1		;column vektor p
	fld	o_p[esi].x2
	fld	o_p[esi].x3

	xor	edx,edx 		;edx = m for A
	xor	ebx,ebx 		;ebx = m for p
@@l3:
	fld	o_A[edi + edx]		;* m-th row of Ao
	fmul	st,st(3)
	fld	o_A[edi + edx+1*4]
	fmul	st,st(3)
	fld	o_A[edi + edx+2*4]
	fmul	st,st(3)
	faddp	st(1)
	faddp	st(1)
	fadd	o_p[edi + ebx*4]
	fstp	o_p[esi + ebx*4]	;= new element (m,1) of p

	add	edx,3*4
	inc	ebx
	cmp	ebx,3
	jb	@@l3

	fstp	st
	fstp	st
	fstp	st

@@n:
	push	esi			;next hierarchy level
	mov	esi,o_child[esi]
	call	dotracks
	pop	esi

		mov	esi,o_next[esi] 	;next object
	jmp	@@next

@@weg:	ret
dotracks endp

calcbumpomni proc near
	;-> esi -> tbumpomni

		;color track (3)
	lea	edi,[esi].bo_color
	call	dotrack pascal, [esi].bo_coltrack, 3

		;multiplier track (1)
	lea	edi,[esi].bo_multiplier
	call	dotrack pascal, [esi].bo_multtrack, 1

		;hidetrack
	lea	ebx,[esi].bo_hidetrack
	lea	edi,[esi].bo_hidden
	call	dohidetrack

	ret
calcbumpomni endp

initframecount proc near
	mov	eax,maincount
	mov	mainstart,eax
;  sub mainstart,3500*66	       ;****
	ret
initframecount endp

calcframecount proc near
	;-> eax = add-frame
	add	mainstart,eax
	mov	eax,maincount
	sub	eax,mainstart
	mov	frame,eax

; add maincount,160		    ;****
	ret
calcframecount endp



end
