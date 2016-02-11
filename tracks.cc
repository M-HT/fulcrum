/* encoding = IBM852 */
#include "cc.h"
//.386					//pmode/w
//.model flat,prolog

//locals

//public frame
//public dotrack, slerp, dorottrack, doltrack, dohidetrack

//±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
//struc
//±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
//d equ dword ptr

//matrix
typedef struct {
 union {
  float A_1n[3];
  struct {
   float A_m1[1];
   float A_m2[1];
   float A_m3[1];
  };
  struct {
   float		A_11;
   float		A_12;
   float		A_13;
  };
 };
 union {
  float A_2n[3];
  struct {
   float		A_21;
   float		A_22;
   float		A_23;
  };
 };
 union {
  float A_3n[3];
  struct {
   float		A_31;
   float		A_32;
   float		A_33;
  };
 };
} tmatrix;

//quaternion
typedef struct {  //fixed size of 16 byte
 float		q_w;
 float		q_x;
 float		q_y;
 float		q_z;
} tquaternion;

//track:
//------
typedef struct {
 uint32_t	t_data;			//track-data
 uint32_t	t_key;			//actual key
} ttrack;

typedef struct {
 uint32_t	th_mode;			//1: loop-track
 uint32_t	th_keys;			//number of gaps to interpolate (keys-1)
 uint32_t	th_sframe;			//start-frame
} ttrackh;

typedef struct {
 uint32_t	k_sframe;			//start-frame of gap
 //k_easefrom	 dd ?
 //k_easeto	 dd ?
 uint32_t	k_eframe;			//end-frame of gap
} tkey;

//linear track:
//-------------
typedef struct {
 uint32_t	lt_startstate;
 uint32_t	lt_startframe;
 uint32_t	lt_endstate;
 uint32_t	lt_endframe;
 uint32_t	lt_keys;
 uint32_t	lt_data;
} tltrack;



//±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
//.data
//±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±

const static float c_3		= 3.0f;
const static float c_sinmin	= 1E-6;

//extrn frame:dword
uint32_t		frame;			//global frame counter

//±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
//.code
//±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±

extern "C" void dotrack(uint32_t _track, uint32_t typesize, uint32_t _edi) {
	realnum fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13, fpu_reg14, fpu_reg15, fpu_reg16;
	uint32_t eax, edx, ecx, edi = _edi, ebx, esi;
	uint32_t stack_var00;

//edi -> result
	uint32_t key;

	stack_var00 = ( 0 /*esi*/ );

	ebx = _track; //ebx -> ttrack
	eax = ( ((ttrack *)ebx)->t_key );
	key = eax;
	esi = ( ((ttrack *)ebx)->t_data ); //esi -> track-data


	ecx = ( ((ttrackh *)esi)->th_keys );
	ecx = ( (int32_t)ecx ) * ( sizeof(tkey) - 4 );
	ecx = ecx + ( sizeof(ttrackh) ); //ecx + esi -> control points

	eax = ( ((ttrackh *)esi)->th_sframe ); //eax = startframe

	if (( ((ttrackh *)esi)->th_mode ) != ( 0 )) goto dotrack_loop;

//single
	edx = frame; //edx = frame

	if (edx <= eax) goto dotrack_c; //before track-start
	eax = ( *((uint32_t *)(esi + ecx + (- 4))) ); //eax = endframe

	if (edx < eax) goto dotrack_key;
//behind track-end
	eax = typesize;
	eax = ( (int32_t)eax ) * ( 3 * 4 );
	eax = ( (int32_t)eax ) * ( (int32_t)(((ttrackh *)esi)->th_keys) );
	ecx = ecx + eax;

dotrack_c: //copy first or last control point
	esi = esi + ecx;
	ecx = typesize;
	for (; ecx != 0; ecx--, esi+=4, edi+=4) *(uint32_t *)edi = *(uint32_t *)esi;
	goto dotrack_weg;
dotrack_loop:
//loop & repeat
	ebx = ( *((uint32_t *)(esi + ecx + (- 4))) );
	ebx = ebx - eax; //ebx = endframe - startframe

	eax = - ( (int32_t)eax );
	eax = eax + frame; //eax = frame - startframe
	edx = ((int32_t)eax) >> 31;
	{ int64_t d1 = (int64_t)(int32_t)eax; int32_t d2 = ( (int32_t)ebx ); eax = d1 / d2; edx = d1 % d2; }

	if (( (int32_t)edx ) >= 0) goto dotrack_0;
	edx = edx + ebx;
dotrack_0:
	edx = edx + ( ((ttrackh *)esi)->th_sframe );

dotrack_key: //edx = frame

//keypos = (site ttrackh) - 4 + key*(size tkey - 4)
	ebx = ( sizeof(tkey) - 4 );
	ebx = ( (int32_t)ebx ) * ( (int32_t)key );
	ebx = ebx + ( sizeof(ttrackh) - 4 );
	ebx = ebx + esi;

//search actual key
	if (edx >= ( ((tkey *)ebx)->k_sframe )) goto dotrack_1;
	key = ( 0 );
	ebx = ( esi + (sizeof(ttrackh) - 4) );
dotrack_1:

	if (edx < ( ((tkey *)ebx)->k_eframe )) goto dotrack_2; //e
	ebx = ebx + ( sizeof(tkey) - 4 );
	key = key + 1;
	goto dotrack_1;
dotrack_2:


//dotrack_spline:
//t
	eax = ( ((tkey *)ebx)->k_sframe );
	edx = edx - eax; //edx = (frame - sframe)
	*((uint32_t *)(edi)) = edx; //temp
	fpu_reg10 = ( *((int32_t *)(edi)) );
	eax = - ( (int32_t)eax );
	eax = eax + ( ((tkey *)ebx)->k_eframe ); //eax = (eframe - sframe)
	*((uint32_t *)(edi)) = eax; //temp
	fpu_reg11 = ( *((int32_t *)(edi)) );
	fpu_reg10 = fpu_reg10 / fpu_reg11; //t = (frame - sframe) / (eframe - sframe);

//P1 * ((( 2*t)-3)*t*t +1)
//P4 * (((-2*t)+3)*t*t)
	fpu_reg11 = fpu_reg10;
	fpu_reg11 = fpu_reg11 + fpu_reg11; //2*t
	fpu_reg11 = fpu_reg11 - c_3; //-3
	fpu_reg11 = fpu_reg11 * fpu_reg10; //*t
	fpu_reg11 = fpu_reg11 * fpu_reg10; //*t
	fpu_reg12 = fpu_reg11;
	fpu_reg12 = -fpu_reg12;
	fpu_reg13 = 1.0;
	fpu_reg11 = fpu_reg11 + fpu_reg13; //+1
//st = 1.0; st(1) = R1, st(2) = R4, st(3) = t

//R1 * (t-2)*t*t+t
//R4 * (t-1)*t*t
	fpu_reg13 = fpu_reg13 + fpu_reg13;
	fpu_reg13 = fpu_reg10 - fpu_reg13; //t-2
	fpu_reg13 = fpu_reg13 * fpu_reg10; //*t
	fpu_reg13 = fpu_reg13 * fpu_reg10; //*t
	fpu_reg13 = fpu_reg13 + fpu_reg10; //+t

	fpu_reg14 = 1.0;
	fpu_reg14 = fpu_reg10 - fpu_reg14; //t-1
	fpu_reg14 = fpu_reg14 * fpu_reg10; //*t
	fpu_reg14 = fpu_reg14 * fpu_reg10; //*t
//st = R4, st(1) = R1, st(2) = P4, st(3) = P1


	esi = esi + ecx; //esi -> control points

	ebx = typesize;
	ecx = ebx; //ecx = typesize
	ebx = ebx << ( 2 );
	edx = ebx; //edx = typesize*4
	ebx = ebx - ecx;
	ebx = ebx << ( 2 );
	eax = ebx; //eax = typesize*12
	ebx = ( (int32_t)ebx ) * ( (int32_t)key );
	esi = esi + ebx; //esi -> P1, R1, R4, P4

	eax = eax - ( 4 ); //eax = "reset-value" for esi
dotrack_l:
	fpu_reg15 = ( *((float *)(esi)) );
	fpu_reg15 = fpu_reg15 * fpu_reg11; //P1
	esi = esi + edx;

	fpu_reg16 = ( *((float *)(esi)) );
	fpu_reg16 = fpu_reg16 * fpu_reg13; //R1
	fpu_reg15 = fpu_reg15 + fpu_reg16;
	esi = esi + edx;

	fpu_reg16 = ( *((float *)(esi)) );
	fpu_reg16 = fpu_reg16 * fpu_reg14; //R4
	fpu_reg15 = fpu_reg15 + fpu_reg16;
	esi = esi + edx;

	fpu_reg16 = ( *((float *)(esi)) );
	fpu_reg16 = fpu_reg16 * fpu_reg12; //P4
	fpu_reg15 = fpu_reg15 + fpu_reg16;

	*((float *)(edi)) = fpu_reg15;

	esi = esi - eax; //reset control point pointer
	edi = edi + ( 4 ); //next target component

	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto dotrack_l;







dotrack_weg:
	ebx = _track; //ebx -> ttrack
	eax = key;
	((ttrack *)ebx)->t_key = eax; //write key back

	esi = stack_var00;
	return;
}



static void slerp(uint32_t _edx, uint32_t _esi, uint32_t _edi, realnum _fpu_reg9) {
	realnum fpu_reg8, fpu_reg9 = _fpu_reg9, fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13, fpu_reg14;
	uint32_t edx = _edx, ecx, edi = _edi, esi = _esi;
//esi -> input quaternions (lie one after another)
//edi -> resulting quaternions
//st = t
//edx = repetitions, for 3 it does this (qi:in, qr:result):
//qr0 = slerp(qi0,qi1,t)
//qr1 = slerp(qi1,qi2,t)
//qr2 = slerp(qi2,qi3,t)

slerp_next:
//calculate q1 * q2 = cos(th)
	ecx = ( 3 );
	fpu_reg10 = 0.0;
slerp_skalar:
	fpu_reg11 = ( ((float *)(esi))[ecx] );
	fpu_reg11 = fpu_reg11 * ( ((float *)(esi))[ecx + 4] );
	fpu_reg10 = fpu_reg10 + fpu_reg11;
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) >= 0) goto slerp_skalar;
//st = cos(th), st(1) = r


//calculate sin(th) = sqrt(1-cos(th)^2)
	fpu_reg11 = fpu_reg10;
	fpu_reg11 = fpu_reg11 * fpu_reg11;
	fpu_reg12 = 1.0;
	fpu_reg11 = fpu_reg12 - fpu_reg11; //st(0) = sin(th)^2 = 1-cos(th)^2



	if (fpu_reg11 < c_sinmin) goto slerp_linear;

	fpu_reg11 = sqrt(fpu_reg11); //st(0) = sin(th), st(1) = cos(th)

	fpu_reg8 = fpu_reg11;
	{ realnum tmp = fpu_reg11; fpu_reg11 = fpu_reg10; fpu_reg10 = tmp; }
	fpu_reg10 = atan2(fpu_reg10, fpu_reg11); //st(0) = th, st(1) = r, st(2) = sin(th)



//calculate i1 = sin((1-r)*th)/sinth
	fpu_reg11 = 1.0;
	fpu_reg11 = fpu_reg11 - fpu_reg9; //st = 1-r
	fpu_reg11 = fpu_reg11 * fpu_reg10; //*th
	fpu_reg11 = sin(fpu_reg11);
	fpu_reg11 = fpu_reg11 / fpu_reg8; // /sinth

//calculate i2 = sin(r*th)/sinth
	fpu_reg12 = fpu_reg9; //st = r
	fpu_reg12 = fpu_reg12 * fpu_reg10; //*th
	fpu_reg12 = sin(fpu_reg12);
	fpu_reg12 = fpu_reg12 / fpu_reg8; // /sinth

	// fpu_reg8 = 0.0;
	goto slerp_i;
slerp_linear: //st(2) = r

//i1 = 1-r
	fpu_reg11 = 1.0;
	fpu_reg11 = fpu_reg11 - fpu_reg9;
//i2 = r
	fpu_reg12 = fpu_reg9;

slerp_i:
//q = q1*i1 + q2*i2
	ecx = ( 4 );
slerp_l:
	fpu_reg13 = ( *((float *)(esi)) );
	fpu_reg13 = fpu_reg13 * fpu_reg11;
	fpu_reg14 = ( ((float *)(esi))[4] );
	fpu_reg14 = fpu_reg14 * fpu_reg12;
	esi = esi + ( 4 );
	fpu_reg13 = fpu_reg13 + fpu_reg14;
	*((float *)(edi)) = fpu_reg13;
	edi = edi + ( 4 );
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto slerp_l;






	edx = ( (int32_t)edx ) - 1;
	if (( (int32_t)edx ) != 0) goto slerp_next;

//slerp_weg:
	return;
}



extern "C" void dorottrack(uint32_t _track, uint32_t _edi) {
	realnum fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13, fpu_reg14, fpu_reg15, fpu_reg16;
	uint32_t eax, edx, ecx, edi = _edi, ebx, esi;
	uint32_t stack_var00, stack_var01;

//edi -> result
	uint32_t key;
	//int32_t q0[4], q1[4], q2[4];
	float q0[4*3];
	stack_var00 = ( 0 /*esi*/ );

	ebx = _track; //ebx -> ttrack
	eax = ( ((ttrack *)ebx)->t_key );
	key = eax;
	esi = ( ((ttrack *)ebx)->t_data ); //esi -> track-data


	ecx = ( ((ttrackh *)esi)->th_keys );
	ecx = ( (int32_t)ecx ) * ( sizeof(tkey) - 4 );
	ecx = ecx + ( sizeof(ttrackh) ); //ecx + esi -> control points

	eax = ( ((ttrackh *)esi)->th_sframe ); //eax = startframe

	if (( ((ttrackh *)esi)->th_mode ) != ( 0 )) goto dorottrack_loop;

//single
	edx = frame; //edx = frame

	if (edx <= eax) goto dorottrack_c; //before track-start
	eax = ( *((uint32_t *)(esi + ecx + (- 4))) ); //eax = endframe

	if (edx < eax) goto dorottrack_key;
//behind track-end
	eax = ( 3 * 4 * 4 ); //3*typesize*4
	eax = ( (int32_t)eax ) * ( (int32_t)(((ttrackh *)esi)->th_keys) );
	ecx = ecx + eax;
//	 mov	 ecx,t_size[ebx]
//	 sub	 ecx,4*4		 ;4*typesize

dorottrack_c: //ersten oder letzen kontrollpunkt
	esi = esi + ecx;
	goto dorottrack_notrack;
dorottrack_loop:
//loop & repeat
	ebx = ( *((uint32_t *)(esi + ecx + (- 4))) );
	ebx = ebx - eax; //ebx = endframe - startframe

	eax = - ( (int32_t)eax );
	eax = eax + frame; //eax = frame - startframe
	edx = ((int32_t)eax) >> 31;
	{ int64_t d1 = (int64_t)(int32_t)eax; int32_t d2 = ( (int32_t)ebx ); eax = d1 / d2; edx = d1 % d2; }

	if (( (int32_t)edx ) >= 0) goto dorottrack_0;
	edx = edx + ebx;
dorottrack_0:
	edx = edx + ( ((ttrackh *)esi)->th_sframe );

dorottrack_key: //edx = frame

//keypos = (site ttrackh) - 4 + key*(size tkey - 4)
	ebx = ( sizeof(tkey) - 4 );
	ebx = ( (int32_t)ebx ) * ( (int32_t)key );
	ebx = ebx + ( sizeof(ttrackh) - 4 );
	ebx = ebx + esi;

//search actual key
	if (edx >= ( ((tkey *)ebx)->k_sframe )) goto dorottrack_1;
	key = ( 0 );
	ebx = ( esi + (sizeof(ttrackh) - 4) );
dorottrack_1:

	if (edx <= ( ((tkey *)ebx)->k_eframe )) goto dorottrack_2;
	ebx = ebx + ( sizeof(tkey) - 4 );
	key = key + 1;
	goto dorottrack_1;
dorottrack_2:


//dorottrack_spline:
//t
	eax = ( ((tkey *)ebx)->k_sframe );
	edx = edx - eax; //edx = (frame - sframe)
	*((uint32_t *)(edi)) = edx; //temp
	fpu_reg10 = ( *((int32_t *)(edi)) );
	eax = - ( (int32_t)eax );
	eax = eax + ( ((tkey *)ebx)->k_eframe ); //eax = (eframe - sframe)
	*((uint32_t *)(edi)) = eax; //temp
	fpu_reg11 = ( *((int32_t *)(edi)) );
	fpu_reg10 = fpu_reg10 / fpu_reg11; //t = (frame - sframe) / (eframe - sframe);


	esi = esi + ecx; //esi -> control points
	ebx = key;
	ebx = ebx << ( 2 );
	ebx = ebx - key; //ebx = key*4
	ebx = ebx << ( 4 ); //*16 (sizeof(quaternion) = 16)
	esi = esi + ebx; //esi -> qn, an, bn, qn1


	stack_var01 = edi;

	edi = ( (uint32_t)&(q0[0]) );
	edx = ( 3 ); //3 repititions
	slerp(edx, esi, edi, fpu_reg10); //q0 = slerp(qn,an,t);
//q1 = slerp(an,bn1,t);
//q2 = slerp(bn1,qn1,t);
	esi = ( (uint32_t)&(q0[0]) );
	edi = esi;
	edx = ( 2 ); //2 repititions
	slerp(edx, esi, edi, fpu_reg10); //q0 = slerp(q0,q1,t);
//q1 = slerp(q1,q2,t);
	esi = ( (uint32_t)&(q0[0]) );
	edi = esi;
	edx = ( 1 ); //1 repitition
	slerp(edx, esi, edi, fpu_reg10); //q0 = slerp(q0,q1,t);

//r

	edi = stack_var01;

	esi = ( (uint32_t)&(q0[0]) );
dorottrack_notrack: //frame out of track range
	fpu_reg10 = ( ((tquaternion *)esi)->q_x );
	fpu_reg10 = fpu_reg10 + fpu_reg10;
	fpu_reg11 = ( ((tquaternion *)esi)->q_y );
	fpu_reg11 = fpu_reg11 + fpu_reg11;
	fpu_reg12 = ( ((tquaternion *)esi)->q_z );
	fpu_reg12 = fpu_reg12 + fpu_reg12;
//st(2) = 2x, st(1) = 2y, st = 2z

//A11, A22, A33
	fpu_reg13 = ( ((tquaternion *)esi)->q_x );
	fpu_reg13 = fpu_reg13 * fpu_reg10;
	fpu_reg14 = ( ((tquaternion *)esi)->q_y );
	fpu_reg14 = fpu_reg14 * fpu_reg11;
	fpu_reg15 = ( ((tquaternion *)esi)->q_z );
	fpu_reg15 = fpu_reg15 * fpu_reg12;
//st(2) = 2xx, st(1) = 2yy, st = 2zz

	fpu_reg16 = 1.0; //1
	fpu_reg16 = fpu_reg16 - fpu_reg14; //-2yy
	fpu_reg16 = fpu_reg16 - fpu_reg15; //-2zz
	((tmatrix *)edi)->A_11 = fpu_reg16;

	fpu_reg16 = 1.0; //1
	fpu_reg16 = fpu_reg16 - fpu_reg13; //-2xx
	fpu_reg15 = fpu_reg16 - fpu_reg15; //-2zz
	((tmatrix *)edi)->A_22 = fpu_reg15;

	fpu_reg15 = 1.0; //1
	fpu_reg14 = fpu_reg15 - fpu_reg14; //-2yy
	fpu_reg13 = fpu_reg14 - fpu_reg13; //-2xx
	((tmatrix *)edi)->A_33 = fpu_reg13;

//A21, A12
	fpu_reg13 = ( ((tquaternion *)esi)->q_x );
	fpu_reg13 = fpu_reg13 * fpu_reg11;
	fpu_reg14 = ( ((tquaternion *)esi)->q_w );
	fpu_reg14 = fpu_reg14 * fpu_reg12;
//st(1) = 2yx, st = 2zw

	fpu_reg15 = fpu_reg13; //2xy
	fpu_reg15 = fpu_reg15 - fpu_reg14; //-2wz
	((tmatrix *)edi)->A_21 = fpu_reg15;
	fpu_reg13 = fpu_reg13 + fpu_reg14; //2xy+2wx
	((tmatrix *)edi)->A_12 = fpu_reg13;

//A13, A31
	fpu_reg13 = ( ((tquaternion *)esi)->q_z );
	fpu_reg13 = fpu_reg13 * fpu_reg10;
	fpu_reg14 = ( ((tquaternion *)esi)->q_w );
	fpu_reg14 = fpu_reg14 * fpu_reg11;
//st(1) = 2xz, st = 2yw

	fpu_reg15 = fpu_reg13; //2xz
	fpu_reg15 = fpu_reg15 - fpu_reg14; //-2yw
	((tmatrix *)edi)->A_13 = fpu_reg15;
	fpu_reg13 = fpu_reg13 + fpu_reg14; //2xz+2yw
	((tmatrix *)edi)->A_31 = fpu_reg13;

//A32, A23
	fpu_reg13 = ( ((tquaternion *)esi)->q_w );
	fpu_reg13 = fpu_reg13 * fpu_reg10;
	fpu_reg14 = ( ((tquaternion *)esi)->q_z );
	fpu_reg14 = fpu_reg14 * fpu_reg11;
//st(1) = 2xw, st = 2yz

	fpu_reg15 = fpu_reg13; //2xw
	fpu_reg15 = fpu_reg14 - fpu_reg15; //2yz-
	((tmatrix *)edi)->A_32 = fpu_reg15;
	fpu_reg13 = fpu_reg13 + fpu_reg14; //2yz+2xw
	((tmatrix *)edi)->A_23 = fpu_reg13;





//dorottrack_weg:
	ebx = _track; //ebx -> ttrack
	eax = key;
	((ttrack *)ebx)->t_key = eax; //write key back

	esi = stack_var00;
	return;
}




extern "C" void doltrack(uint32_t _track, uint32_t typesize, uint32_t _edi) {
	realnum fpu_reg10, fpu_reg11, fpu_reg12;
	uint32_t eax, edx, ecx, edi = _edi, ebx, esi;
	uint32_t stack_var00;

//edi -> result
	uint32_t temp;

	stack_var00 = ( 0 /*esi*/ );

	ebx = _track;
doltrack_r:
	edx = frame;

	if (edx <= ( ((tltrack *)ebx)->lt_endframe )) goto doltrack_i;
//end of key

	if (( ((tltrack *)ebx)->lt_keys ) > ( 0 )) goto doltrack_next;
//end of track
	fpu_reg10 = 1.0;
	goto doltrack_e;
doltrack_next: //get next key
	eax = ( ((tltrack *)ebx)->lt_endframe );
	((tltrack *)ebx)->lt_startframe = eax; //old endframe is new startframe

	((tltrack *)ebx)->lt_keys = ( ((tltrack *)ebx)->lt_keys ) - 1;

	esi = ( ((tltrack *)ebx)->lt_data );
	eax = (int32_t)( *((int8_t *)(esi)) ); //get scale value
	esi = esi + 1;
	temp = eax;

	edx = ( ((tltrack *)ebx)->lt_endstate );
	ecx = 0;
doltrack_l:

	eax = (int32_t)( *((int8_t *)(esi + ecx)) ); //get differences
	fpu_reg11 = ( (int32_t)eax );

	fpu_reg11 = ldexp(fpu_reg11, (int32_t)temp);

	eax = ( ((tltrack *)ebx)->lt_startstate );

	fpu_reg12 = ( ((float *)(edx))[ecx] );
	fpu_reg12 = fpu_reg12 + ( ((float *)(eax))[ecx] );
	((float *)(eax))[ecx] = fpu_reg12;
	((float *)(edx))[ecx] = fpu_reg11;


	ecx = ecx + 1;

	if (ecx < typesize) goto doltrack_l;



	eax = ( *((uint32_t *)(esi + ecx)) ); //get endframe
	((tltrack *)ebx)->lt_endframe = eax;

	eax = ( esi + ecx + (4) ); //update data pointer
	((tltrack *)ebx)->lt_data = eax;
	goto doltrack_r;
doltrack_i: //interpolate		;edx = frame
	eax = ( ((tltrack *)ebx)->lt_startframe );
	edx = edx - eax; //edx = (frame - sframe)
	temp = edx;
	fpu_reg10 = ( (int32_t)temp );
	eax = - ( (int32_t)eax );
	eax = eax + ( ((tltrack *)ebx)->lt_endframe ); //eax = (eframe - sframe)
	temp = eax;
	fpu_reg11 = ( (int32_t)temp );
	fpu_reg10 = fpu_reg10 / fpu_reg11; //t = (frame - sframe) / (eframe - sframe);
doltrack_e:
	esi = ( ((tltrack *)ebx)->lt_startstate );
	edx = ( ((tltrack *)ebx)->lt_endstate );

	ecx = 0;
doltrack_l2:
	fpu_reg11 = ( ((float *)(edx))[ecx] );
	fpu_reg11 = fpu_reg11 * fpu_reg10;
	fpu_reg11 = fpu_reg11 + ( ((float *)(esi))[ecx] );
	((float *)(edi))[ecx] = fpu_reg11;

	ecx = ecx + 1;

	if (ecx < typesize) goto doltrack_l2;



	esi = stack_var00;

	return;
}


extern "C" void dohidetrack(uint32_t _ebx, uint32_t _edi) { //pascal
	uint32_t eax, edx, edi = _edi, ebx = _ebx, esi;
	uint32_t stack_var00;
//ebx -> track

	stack_var00 = ( 0 /*esi*/ );

	esi = ( *((uint32_t *)(ebx)) ); //esi -> track

	edx = ( ((uint32_t *)(ebx))[1] ); //edx = key

dohidetrack_l:
//end of track? (esi -> keys)
	if (edx >= ( *((uint32_t *)(esi)) )) goto dohidetrack_w;

	eax = ( ((uint32_t *)(esi))[edx + 1] ); //check switch-frame

	if (eax > frame) goto dohidetrack_w; //not yet reached
	*((uint32_t *)(edi)) = ( *((uint32_t *)(edi)) ) ^ ( 1 ); //switch hidden-flag
	edx = edx + 1; //next key
	goto dohidetrack_l;
dohidetrack_w:
	((uint32_t *)(ebx))[1] = edx; //write key back

	esi = stack_var00;
	return;
}




