/* encoding = IBM852 */
#include <SDL/SDL.h>
#include "cc.h"
//.486					;pmode/w
//.model flat,prolog

//locals


//NULL = 0

#include "int.inc.h"
#include "tracks.inc.h"

//;public frame

//public dotracks, initframecount, calcframecount
//public calcbumpomni
//public m_dotracksM2, c_dotracksM2, t_dotracksM2, i_dotracksM2, d_dotracksM2

//public matrixmul, doviewer


//±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
//struc
//±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
//d equ dword ptr

typedef struct {
 float x1;
 float x2;
 float x3;
} tvec;

//matrix
typedef struct {
 union {
  float A_m1[7];
  struct {
   float A_m1_2;
   float A_m2[7];
  };
  struct {
   float A_m1_3;
   float A_m2_3;
   float A_m3[7];
  };
  struct {
   union {
    float A_1n[3];
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
  };
 };
} tmatrix;

#define M2STRUCT_HAS_TMATRIX 1
#include "m2struct.inc.h"
#undef M2STRUCT_HAS_TMATRIX

//VMT
typedef struct {
 int32_t o_size;
 int32_t o_readdata;
 int32_t o_firstobj;
 int32_t o_nextobj;
 void  (*o_dotracks)(uint32_t _esi);
 int32_t o_insert;
} tobjvmt;

//object:
//-------
#define ofMatrix    1
#define ofMesh	    2
#define ofAbsolute  4

typedef struct {
 uint32_t o_vmt;

 uint32_t o_flags;

 //hierarchy-system: (pointer)
 uint32_t o_owner;
 uint32_t o_next;
 uint32_t o_child;

 //state: (vektor/matrix)
 union {
  float o_p_data[3];
  tvec o_p;                 //actual position (position and matrix have fixed order in memory)
 };
 union {
  float   o_A_data[9];
  tmatrix o_A;              //actual rotation and scale
 };
 uint32_t o_hidden;

} tobject;


//mesh:
//-----
#define mfMorph 	8
#define mfRGBPreCalc	16
   //G
#define mfBumpPreCalc	32
   //B
#define mfDistance	64
   //Z


typedef struct {
 tobject m_object;

 CMV2PolygonObject m_PolygonObject;

 //postrack (3):
 uint32_t m_postrack;			//pointer

 //rotationtrack (4):
 uint32_t m_rottrack;			//pointer

 //scaletrack (3):
// m_scaletrack	 dd ?			;pointer

 //morphtrack (2*vertices*3):
 uint32_t m_morphtrack;			//pointer
// m_nmorphtrack  dd ?

 uint32_t m_rgbtrack;			//pointer

 //hidetrack
 uint32_t m_hidetrack[2]; 		//pointer, int key

} tmesh;


//camera:
//-------
typedef struct {
 tobject c_object;

 uint32_t c_target;			//pointer

 //postrack (3):
 uint32_t c_postrack;			//pointer

 //FOVtrack (1):
 uint32_t c_FOVtrack;			//pointer
 float c_FOV;			//float

 //rolltrack (1):
 uint32_t c_rolltrack;			//pointer
 float c_roll;			//float
} tcamera;

//target:
//-------
typedef struct {
 tobject t_object;

 //postrack (3):
 uint32_t t_postrack;			//pointer
} ttarget;


//omni:
//-----
typedef struct {
 tobject i_object;

 //postrack (3):
 uint32_t i_postrack;			//pointer

 //coltrack (3):
 uint32_t i_coltrack;			//pointer
 int32_t i_color[3];

 //hidetrack
 uint32_t i_hidetrack[2]; 		//pointer, int key
} tomni;

//dummy:
//------
typedef struct {
 tobject d_object;

 int32_t d_special;

 //position
 int32_t d_x[3];

 //hidetrack
 uint32_t d_hidetrack[2]; 		//pointer, int key

 int32_t d_firstframe;
} tdummy;


typedef struct {
 //color track (3):
 uint32_t bo_coltrack;			//pointer
 int32_t bo_color[3];

 //multiplier track (1):
 uint32_t bo_multtrack;			//pointer
 int32_t bo_multiplier;

 //hidetrack
 uint32_t bo_hidetrack[2]; 		//pointer, int key
 uint32_t bo_hidden;
} tbumpomni;



//linear track:
//-------------
typedef struct {
 uint32_t lt_startstate;
 uint32_t lt_startframe;
 uint32_t lt_endstate;
 uint32_t lt_endframe;
 uint32_t lt_keys;
 uint32_t lt_data;
} tltrack;


//±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
//.data
//±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±

const static float c_pi_2 = 1.5707963f;
const static float c_1_33 = 1.3333333f;

//frame dd ?
static uint32_t mainstart;
//±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
//.code
//±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±

static void doltrackM2(uint32_t _track, uint32_t typesize, uint32_t _edi) {
	realnum fpu_reg10, fpu_reg11, fpu_reg12;
	uint32_t eax, edx, ecx, edi = _edi, ebx, esi;
	uint32_t stack_var00;

//edi -> result
	uint32_t temp;

	stack_var00 = ( 0 /*esi*/ );

	ebx = _track;
doltrackM2_r:
	edx = frame;

	if (edx <= ( ((tltrack *)ebx)->lt_endframe )) goto doltrackM2_i;
//end of key

	if (( ((tltrack *)ebx)->lt_keys ) > ( 0 )) goto doltrackM2_next;
//end of track
	fpu_reg10 = 1.0;
	goto doltrackM2_e;
doltrackM2_next: //get next key
	eax = ( ((tltrack *)ebx)->lt_endframe );
	((tltrack *)ebx)->lt_startframe = eax; //old endframe is new startframe

	((tltrack *)ebx)->lt_keys = ( ((tltrack *)ebx)->lt_keys ) - 1;

	esi = ( ((tltrack *)ebx)->lt_data );
	eax = (int32_t)( *((int8_t *)(esi)) ); //get scale value
	esi = esi + 1;
	temp = eax;

	edx = ( ((tltrack *)ebx)->lt_endstate );
	ecx = 0;
doltrackM2_l:

	eax = (int32_t)( *((int8_t *)(esi + ecx)) ); //get differences
	fpu_reg11 = ( (int32_t)eax );

	fpu_reg11 = ldexp(fpu_reg11, (int32_t)temp);

	eax = ( ((tltrack *)ebx)->lt_startstate );

	fpu_reg12 = ( ((float *)(edx))[ecx] );
	fpu_reg12 = fpu_reg12 + ( ((float *)(eax))[ecx] );
	((float *)(eax))[ecx] = fpu_reg12;
	((float *)(edx))[ecx] = fpu_reg11;


	ecx = ecx + 1;

	if (ecx < typesize) goto doltrackM2_l;



	eax = ( *((uint32_t *)(esi + ecx)) ); //get endframe
	((tltrack *)ebx)->lt_endframe = eax;

	eax = ( esi + ecx + (4) ); //update data pointer
	((tltrack *)ebx)->lt_data = eax;
	goto doltrackM2_r;
doltrackM2_i: //interpolate		;edx = frame
	eax = ( ((tltrack *)ebx)->lt_startframe );
	edx = edx - eax; //edx = (frame - sframe)
	temp = edx;
	fpu_reg10 = ( (int32_t)temp );
	eax = - ( (int32_t)eax );
	eax = eax + ( ((tltrack *)ebx)->lt_endframe ); //eax = (eframe - sframe)
	temp = eax;
	fpu_reg11 = ( (int32_t)temp );
	fpu_reg10 = fpu_reg10 / fpu_reg11; //t = (frame - sframe) / (eframe - sframe);
doltrackM2_e:
	esi = ( ((tltrack *)ebx)->lt_startstate );
	edx = ( ((tltrack *)ebx)->lt_endstate );

	ecx = 0;
doltrackM2_l2:
	fpu_reg11 = ( ((float *)(edx))[ecx] );
	fpu_reg11 = fpu_reg11 * fpu_reg10;
	fpu_reg11 = fpu_reg11 + ( ((float *)(esi))[ecx] );
	((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_Pos__m_fX = fpu_reg11; //x1
	ecx = ecx + 1;

	fpu_reg11 = ( ((float *)(edx))[ecx] );
	fpu_reg11 = fpu_reg11 * fpu_reg10;
	fpu_reg11 = fpu_reg11 + ( ((float *)(esi))[ecx] );
	((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_Pos__m_fY = fpu_reg11; //x2
	ecx = ecx + 1;

	fpu_reg11 = ( ((float *)(edx))[ecx] );
	fpu_reg11 = fpu_reg11 * fpu_reg10;
	fpu_reg11 = fpu_reg11 + ( ((float *)(esi))[ecx] );
	((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_Pos__m_fZ = fpu_reg11; //x3
	ecx = ecx + 1;

	edi = edi + ( CMV2Dot3DPos__Size );


	if (ecx < typesize) goto doltrackM2_l2;



	esi = stack_var00;

	return;
}






static void dolRGBtrackM2(uint32_t _track, uint32_t typesize, uint32_t _edi) {
	realnum fpu_reg10, fpu_reg11, fpu_reg12;
	uint32_t eax, edx, ecx, edi = _edi, ebx, esi;
	uint32_t stack_var00;

//edi -> result
	uint32_t temp;

	stack_var00 = ( 0 /*esi*/ );

	ebx = _track;
dolRGBtrackM2_r:
	edx = frame;

	if (edx <= ( ((tltrack *)ebx)->lt_endframe )) goto dolRGBtrackM2_i;
//end of key

	if (( ((tltrack *)ebx)->lt_keys ) > ( 0 )) goto dolRGBtrackM2_next;
//end of track
	fpu_reg10 = 1.0;
	goto dolRGBtrackM2_e;
dolRGBtrackM2_next: //get next key
	eax = ( ((tltrack *)ebx)->lt_endframe );
	((tltrack *)ebx)->lt_startframe = eax; //old endframe is new startframe

	((tltrack *)ebx)->lt_keys = ( ((tltrack *)ebx)->lt_keys ) - 1;

	esi = ( ((tltrack *)ebx)->lt_data );
	eax = (int32_t)( *((int8_t *)(esi)) ); //get scale value
	esi = esi + 1;
	temp = eax;

	edx = ( ((tltrack *)ebx)->lt_endstate );
	ecx = 0;
dolRGBtrackM2_l:

	eax = (int32_t)( *((int8_t *)(esi + ecx)) ); //get differences
	fpu_reg11 = ( (int32_t)eax );

	fpu_reg11 = ldexp(fpu_reg11, (int32_t)temp);

	eax = ( ((tltrack *)ebx)->lt_startstate );

	fpu_reg12 = ( ((float *)(edx))[ecx] );
	fpu_reg12 = fpu_reg12 + ( ((float *)(eax))[ecx] );
	((float *)(eax))[ecx] = fpu_reg12;
	((float *)(edx))[ecx] = fpu_reg11;


	ecx = ecx + 1;

	if (ecx < typesize) goto dolRGBtrackM2_l;



	eax = ( *((uint32_t *)(esi + ecx)) ); //get endframe
	((tltrack *)ebx)->lt_endframe = eax;

	eax = ( esi + ecx + (4) ); //update data pointer
	((tltrack *)ebx)->lt_data = eax;
	goto dolRGBtrackM2_r;
dolRGBtrackM2_i: //interpolate		;edx = frame
	eax = ( ((tltrack *)ebx)->lt_startframe );
	edx = edx - eax; //edx = (frame - sframe)
	temp = edx;
	fpu_reg10 = ( (int32_t)temp );
	eax = - ( (int32_t)eax );
	eax = eax + ( ((tltrack *)ebx)->lt_endframe ); //eax = (eframe - sframe)
	temp = eax;
	fpu_reg11 = ( (int32_t)temp );
	fpu_reg10 = fpu_reg10 / fpu_reg11; //t = (frame - sframe) / (eframe - sframe);
dolRGBtrackM2_e:
	esi = ( ((tltrack *)ebx)->lt_startstate );
	edx = ( ((tltrack *)ebx)->lt_endstate );

	ecx = 0;
dolRGBtrackM2_l2:
	fpu_reg11 = ( ((float *)(edx))[ecx] );
	fpu_reg11 = fpu_reg11 * fpu_reg10;
	fpu_reg11 = fpu_reg11 + ( ((float *)(esi))[ecx] );
//	      fchs
	((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fR = fpu_reg11;
	ecx = ecx + 1;

	fpu_reg11 = ( ((float *)(edx))[ecx] );
	fpu_reg11 = fpu_reg11 * fpu_reg10;
	fpu_reg11 = fpu_reg11 + ( ((float *)(esi))[ecx] );
	((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fG = fpu_reg11;
	ecx = ecx + 1;

	fpu_reg11 = ( ((float *)(edx))[ecx] );
	fpu_reg11 = fpu_reg11 * fpu_reg10;
	fpu_reg11 = fpu_reg11 + ( ((float *)(esi))[ecx] );
	((CMV2Dot3DPos *)edi)->CMV2Dot3DPos__m_fB = fpu_reg11;
	ecx = ecx + 1;

	edi = edi + ( CMV2Dot3DPos__Size );


	if (ecx < typesize) goto dolRGBtrackM2_l2;



	esi = stack_var00;

	return;
}







extern "C" void m_dotracksM2(uint32_t _esi) {
	uint32_t edx, edi, ebx, esi = _esi;
//-> esi -> tmesh

//hidetrack
	ebx = ( (uint32_t)&(((tmesh *)esi)->m_hidetrack[0]) );
	edi = ( (uint32_t)&(((tobject *)esi)->o_hidden) );
	dohidetrack(ebx, edi);



	if ((( ((tobject *)esi)->o_flags ) & ( ofAbsolute )) != 0) goto m_dotracksM2_abs;

//rotation track (4)
	edi = ( (uint32_t)&(((tobject *)esi)->o_A) );
	dorottrack(( ((tmesh *)esi)->m_rottrack ), edi);

//position track (3)
	edi = ( (uint32_t)&(((tobject *)esi)->o_p) );
	dotrack(( ((tmesh *)esi)->m_postrack ), ( 3 ), edi);


	if (( ((tobject *)esi)->o_hidden ) > ( 0 )) goto m_dotracksM2_weg;
	goto m_dotracksM2_abs0;
m_dotracksM2_abs: //absolute

	if (( ((tobject *)esi)->o_hidden ) > ( 0 )) goto m_dotracksM2_weg;

//position track (3)
	edi = ( (uint32_t)&(((tobject *)esi)->o_p) );
	doltrack(( ((tmesh *)esi)->m_postrack ), ( 3 ), edi);


//morphtrack (2*vertices*3)

	if ((( ((tobject *)esi)->o_flags ) & ( mfMorph )) == 0) goto m_dotracksM2_m0;

	edi = ( ((tmesh *)esi)->m_PolygonObject.CMV2PolygonObject__m_pDot3DPos );
	edx = ( ((tmesh *)esi)->m_PolygonObject.CMV2PolygonObject__m_iNumDot3DPos );
	edx = ( edx + edx * 2 ); //edx*3
	doltrackM2(( ((tmesh *)esi)->m_morphtrack ), edx, edi);
m_dotracksM2_m0:

m_dotracksM2_abs0:

	if ((( ((tobject *)esi)->o_flags ) & ( mfRGBPreCalc + mfBumpPreCalc )) == 0) goto m_dotracksM2_NotRGBPreCalc;

	edi = ( ((tmesh *)esi)->m_PolygonObject.CMV2PolygonObject__m_pDot3DPos );
	edx = ( ((tmesh *)esi)->m_PolygonObject.CMV2PolygonObject__m_iNumDot3DPos );
	edx = ( edx + edx * 2 ); //edx*3
	dolRGBtrackM2(( ((tmesh *)esi)->m_rgbtrack ), edx, edi);

m_dotracksM2_NotRGBPreCalc:

m_dotracksM2_weg:
	return;
}



extern "C" void c_dotracksM2(uint32_t _esi) {
	uint32_t edi, esi = _esi;

//position track (3)
	edi = ( (uint32_t)&(((tobject *)esi)->o_p) );
	dotrack(( ((tcamera *)esi)->c_postrack ), ( 3 ), edi); //eax, 3

//FOV track (1)
	edi = ( (uint32_t)&(((tcamera *)esi)->c_FOV) );
	dotrack(( ((tcamera *)esi)->c_FOVtrack ), ( 1 ), edi); //eax, 1

//roll track (1)
	edi = ( (uint32_t)&(((tcamera *)esi)->c_roll) );
	dotrack(( ((tcamera *)esi)->c_rolltrack ), ( 1 ), edi); //eax, 1

//no matrix
	return;
}



extern "C" void t_dotracksM2(uint32_t _esi) {
	uint32_t edi, esi = _esi;
//-> esi -> ttarget

//position track (3)
	edi = ( (uint32_t)&(((tobject *)esi)->o_p) );
	dotrack(( ((ttarget *)esi)->t_postrack ), ( 3 ), edi);

//no matrix
//t_dotracksM2_weg:
	return;
}


extern "C" void i_dotracksM2(uint32_t _esi) {
	uint32_t edi, ebx, esi = _esi;

//position track (3)
	edi = ( (uint32_t)&(((tobject *)esi)->o_p) );
	dotrack(( ((tomni *)esi)->i_postrack ), ( 3 ), edi);

//color track (3)
	edi = ( (uint32_t)&(((tomni *)esi)->i_color[0]) );
	dotrack(( ((tomni *)esi)->i_coltrack ), ( 3 ), edi);

//hidetrack
	ebx = ( (uint32_t)&(((tomni *)esi)->i_hidetrack[0]) );
	edi = ( (uint32_t)&(((tobject *)esi)->o_hidden) );
	dohidetrack(ebx, edi);

//no matrix
//i_dotracksM2_weg:
	return;
}


extern "C" void d_dotracksM2(uint32_t _esi) {
	uint32_t edi, ebx, esi = _esi;

//hidetrack
	ebx = ( (uint32_t)&(((tdummy *)esi)->d_hidetrack[0]) );
	edi = ( (uint32_t)&(((tobject *)esi)->o_hidden) );
	dohidetrack(ebx, edi);

//no matrix
//d_dotracksM2_weg:
	return;
}



extern "C" void doviewer(uint32_t _esi, uint32_t _edi) {
	realnum fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13, fpu_reg14, fpu_reg15, fpu_reg16;
	uint32_t ecx, edi = _edi, ebx, esi = _esi;
//-> esi -> tcamera
//-> edi -> CMV2Camera
//<- esi -> tcamera
//<- edi -> CMV2Camera


	ebx = ( ((tcamera *)esi)->c_target );

//direction vector u to target
	fpu_reg10 = ( ((tobject *)esi)->o_p.x1 );
	((CMV2Camera *)edi)->CMV2Camera__m_Pos__m_fX = fpu_reg10;
	fpu_reg10 = ( ((tobject *)ebx)->o_p.x1 ) - fpu_reg10;
	fpu_reg11 = ( ((tobject *)esi)->o_p.x2 );
	((CMV2Camera *)edi)->CMV2Camera__m_Pos__m_fY = fpu_reg11;
	fpu_reg11 = ( ((tobject *)ebx)->o_p.x2 ) - fpu_reg11;
	fpu_reg12 = ( ((tobject *)esi)->o_p.x3 );
	((CMV2Camera *)edi)->CMV2Camera__m_Pos__m_fZ = fpu_reg12;
	fpu_reg12 = ( ((tobject *)ebx)->o_p.x3 ) - fpu_reg12;

//calculate local coordinate system l1, l2, l3 (l3 = view axis)
//l3 = u/|u|
//l1 = ((u) x (0,0,1))/|(u) x (0,0,1)|
//   = (u2,-u1,0)/sqrt(u1^2 + u2^2)
//l2 = l1 x l3  = | a2*b3	|
// (for l1 = a)	 |-a1*b3	|
// (and l2 = b)	 | a1*b2 - a2*b1|

	fpu_reg13 = fpu_reg10; //make unit length
	fpu_reg13 = fpu_reg13 * fpu_reg13; //u1*u1
	fpu_reg14 = fpu_reg11;
	fpu_reg14 = fpu_reg14 * fpu_reg14; //u2*u2
	fpu_reg13 = fpu_reg13 + fpu_reg14;
	fpu_reg14 = fpu_reg12; //st(1) = (u1^2 + u2^2)
	fpu_reg14 = fpu_reg14 * fpu_reg14; //u3*u3
	fpu_reg14 = fpu_reg14 + fpu_reg13;
	fpu_reg14 = sqrt(fpu_reg14); //sqrt(u1^2 + u2^2 + u3^2) (length of u)

//l3			;l3 = u/|u|
	fpu_reg15 = fpu_reg10;
	fpu_reg15 = fpu_reg15 / fpu_reg14;
//      fchs
	((CMV2Camera *)edi)->CMV2Camera__m_Front__m_fX = fpu_reg15;
	fpu_reg15 = fpu_reg11;
	fpu_reg15 = fpu_reg15 / fpu_reg14;
//      fchs
	((CMV2Camera *)edi)->CMV2Camera__m_Front__m_fY = fpu_reg15;
	fpu_reg14 = fpu_reg12 / fpu_reg14;
//      fchs
	((CMV2Camera *)edi)->CMV2Camera__m_Front__m_fZ = fpu_reg14;

//l1			;l1 = (u2,-u1,0)/sqrt(u1^2 + u2^2)
	fpu_reg13 = sqrt(fpu_reg13); //sqrt(u1^2 + u2^2)

	fpu_reg14 = fpu_reg11; //u2/sqrt(u1^2 + u2^2)
	fpu_reg14 = fpu_reg14 / fpu_reg13;
	((CMV2Camera *)edi)->CMV2Camera__m_Right__m_fX = fpu_reg14;

	fpu_reg10 = fpu_reg10 / fpu_reg13; //-u1/sqrt(u1^2 + u2^2)
//remove u3
//remove u2
	fpu_reg10 = -fpu_reg10;
	((CMV2Camera *)edi)->CMV2Camera__m_Right__m_fY = fpu_reg10;

	fpu_reg10 = 0.0; //0
	((CMV2Camera *)edi)->CMV2Camera__m_Right__m_fZ = fpu_reg10;

//l2			;l2 = l1 x l3
	fpu_reg10 = ( ((CMV2Camera *)edi)->CMV2Camera__m_Front__m_fX ); //b1
	fpu_reg11 = ( ((CMV2Camera *)edi)->CMV2Camera__m_Right__m_fY ); //a2
	fpu_reg12 = ( ((CMV2Camera *)edi)->CMV2Camera__m_Front__m_fY ); //b2
	fpu_reg13 = ( ((CMV2Camera *)edi)->CMV2Camera__m_Right__m_fX ); //a1
	fpu_reg14 = ( ((CMV2Camera *)edi)->CMV2Camera__m_Front__m_fZ ); //b3

	fpu_reg15 = fpu_reg11;
	fpu_reg15 = fpu_reg15 * fpu_reg14;
	((CMV2Camera *)edi)->CMV2Camera__m_Down__m_fX = fpu_reg15; //a2*b3

	fpu_reg14 = fpu_reg14 * fpu_reg13;
	fpu_reg14 = -fpu_reg14;
	((CMV2Camera *)edi)->CMV2Camera__m_Down__m_fY = fpu_reg14; //-a1*b3

	fpu_reg12 = fpu_reg12 * fpu_reg13;
	{ realnum tmp = fpu_reg11; fpu_reg11 = fpu_reg12; fpu_reg12 = tmp; }
	fpu_reg10 = fpu_reg10 * fpu_reg12;
	fpu_reg10 = fpu_reg11 - fpu_reg10;
	((CMV2Camera *)edi)->CMV2Camera__m_Down__m_fZ = fpu_reg10; //a1*b2-a2*b1

//calc l1 and l2 scale factors for camera opening angle
	fpu_reg10 = c_pi_2; //1/tan(FOV) = tan(pi/2 - FOV)
	fpu_reg10 = fpu_reg10 - ( ((tcamera *)esi)->c_FOV ); //FOV = halve camera view angle
	fpu_reg10 = tan(fpu_reg10);
	fpu_reg11 = 1.0;

	fpu_reg11 = fpu_reg10;
	fpu_reg11 = fpu_reg11 * c_1_33;
//	 fmulp	 st(2),st		 ;st(0) = (x:y-ratio) * 1/tan(FOV)

//calc l1 and l2 scale factors for rotation about the view axis
	fpu_reg12 = ( ((tcamera *)esi)->c_roll );
	fpu_reg13 = cos(fpu_reg12);
	fpu_reg12 = sin(fpu_reg12);

//rotate & scale l1 and l2
	ecx = ( 2 );
doviewer_l:
//l1' = (l1*cos(roll) + l2*sin(roll))/tan(FOV)
	fpu_reg14 = ( ((CMV2Camera *)edi)->CMV2Camera__LabelFirstVector_data[ecx] );
	fpu_reg14 = fpu_reg14 * fpu_reg13; //cos
	fpu_reg15 = ( ((CMV2Camera *)(edi + ecx * 4))->CMV2Camera__m_Down__m_fX );
	fpu_reg15 = fpu_reg15 * fpu_reg12; //sin
	fpu_reg14 = fpu_reg14 + fpu_reg15;
	fpu_reg14 = fpu_reg14 * fpu_reg10; //1/tan(FOV)
//l2' = (l2*cos(roll) + l1*sin(roll))*(x:y ratio)/tan(FOV)
	fpu_reg15 = ( ((CMV2Camera *)(edi + ecx * 4))->CMV2Camera__m_Down__m_fX );
	fpu_reg15 = fpu_reg15 * fpu_reg13; //cos
	fpu_reg16 = ( ((CMV2Camera *)edi)->CMV2Camera__LabelFirstVector_data[ecx] );
	fpu_reg16 = fpu_reg16 * fpu_reg12; //sin
	fpu_reg15 = fpu_reg15 - fpu_reg16;
	fpu_reg15 = fpu_reg15 * fpu_reg11; //(x:y ratio)/tan(FOV)

	fpu_reg15 = -fpu_reg15;
	((CMV2Camera *)(edi + ecx * 4))->CMV2Camera__m_Down__m_fX = fpu_reg15;
	((CMV2Camera *)edi)->CMV2Camera__LabelFirstVector_data[ecx] = fpu_reg14;
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) >= 0) goto doviewer_l;





	return;
}


extern "C" void matrixmul(uint32_t _esi, uint32_t _edx, uint32_t _edi) {
	realnum fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13, fpu_reg14, fpu_reg15;
	uint32_t eax, edx = _edx, ecx, edi = _edi, ebx, esi = _esi;
//-> esi -> tobject
//-> edx -> CMV2Camera
//-> edi -> CMV2Camera


	if ((( ((tobject *)esi)->o_flags ) & ( ofAbsolute )) != 0) goto matrixmul_abs;
//calculate matrix A = Av * Ao (v:viewer, o:object)
	ebx = 0; //ebx = n
matrixmul_l0:
	fpu_reg10 = ( ((tobject *)esi)->o_A.A_1n[ebx] ); //n-th column of A
	fpu_reg11 = ( ((tobject *)esi)->o_A.A_2n[ebx] );
	fpu_reg12 = ( ((tobject *)esi)->o_A.A_3n[ebx] );

	ecx = 0; //ecx = m
matrixmul_l1:
	fpu_reg13 = ( ((CMV2Camera *)(edx + ecx))->CMV2Camera__LabelFirstVector.A_m1[0] ); //* m-th row of Al
	fpu_reg13 = fpu_reg13 * fpu_reg10;
	fpu_reg14 = ( ((CMV2Camera *)(edx + ecx))->CMV2Camera__LabelFirstVector.A_m2[0] );
	fpu_reg14 = fpu_reg14 * fpu_reg11;
	fpu_reg15 = ( ((CMV2Camera *)(edx + ecx))->CMV2Camera__LabelFirstVector.A_m3[0] );
	fpu_reg15 = fpu_reg15 * fpu_reg12;
	fpu_reg14 = fpu_reg14 + fpu_reg15;
	fpu_reg13 = fpu_reg13 + fpu_reg14;
	eax = ( ecx + ebx * 4 );
	((CMV2Camera *)(edi + eax))->CMV2Camera__LabelFirstVector_data[0] = fpu_reg13; //= new element (m,n) of A

	ecx = ecx + ( 3 * 4 );

	if (ecx < ( 3 * 3 * 4 )) goto matrixmul_l1;





	ebx = ebx + 1;

	if (ebx < ( 3 )) goto matrixmul_l0;

	goto matrixmul_abs0;
matrixmul_abs: //absolute: matrix A = Av (only position valid)
	ecx = ( 9 - 1 );
	ebx = ( (uint32_t)&(((CMV2Camera *)edx)->CMV2Camera__LabelFirstVector) );
matrixmul_l:
	((CMV2Camera *)edi)->CMV2Camera__LabelFirstVector_data[ecx] = ( ((float *)(ebx))[ecx] );
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) >= 0) goto matrixmul_l;

matrixmul_abs0:
//p = Av*(po-pv)
	fpu_reg10 = ( ((tobject *)esi)->o_p.x1 ); //(po-pv)
	fpu_reg10 = fpu_reg10 - ( ((CMV2Camera *)edx)->CMV2Camera__m_Pos__m_fX );
	fpu_reg11 = ( ((tobject *)esi)->o_p.x2 );
	fpu_reg11 = fpu_reg11 - ( ((CMV2Camera *)edx)->CMV2Camera__m_Pos__m_fY );
	fpu_reg12 = ( ((tobject *)esi)->o_p.x3 );
	fpu_reg12 = fpu_reg12 - ( ((CMV2Camera *)edx)->CMV2Camera__m_Pos__m_fZ );

	ebx = 0; //ebx = m
matrixmul_l2:
	ecx = ( ebx + ebx * 2 );
	fpu_reg13 = ( ((CMV2Camera *)edx)->CMV2Camera__LabelFirstVector.A_m1[ecx] ); //* m-th row of Av
	fpu_reg13 = fpu_reg13 * fpu_reg10;
	fpu_reg14 = ( ((CMV2Camera *)edx)->CMV2Camera__LabelFirstVector.A_m2[ecx] );
	fpu_reg14 = fpu_reg14 * fpu_reg11;
	fpu_reg15 = ( ((CMV2Camera *)edx)->CMV2Camera__LabelFirstVector.A_m3[ecx] );
	fpu_reg15 = fpu_reg15 * fpu_reg12;
	fpu_reg14 = fpu_reg14 + fpu_reg15;
	fpu_reg13 = fpu_reg13 + fpu_reg14;
	((CMV2Camera *)(edi + ebx * 4))->CMV2Camera__m_Pos__m_fX = fpu_reg13; //= new element (m,1) of p

	ebx = ebx + 1;

	if (ebx < ( 3 )) goto matrixmul_l2;





//matrixmul_weg:
	return;
}



extern "C" void dotracks(uint32_t _esi) {
	realnum fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13, fpu_reg14, fpu_reg15;
	uint32_t eax, edx, edi, ebx, esi = _esi;
	uint32_t stack_var00;
//-> esi = *this
//<- esi = NULL
dotracks_next:

	if (( (int32_t)esi ) == 0) goto dotracks_weg;

	ebx = ( ((tobject *)esi)->o_vmt ); //ebx -> vmt
	( ((tobjvmt *)ebx)->o_dotracks )(esi); //virtual method to calculate the tracks

//A = matrix, p = position
//o = owner

//A = Ao*A (matrix multiplication)
	edi = ( ((tobject *)esi)->o_owner );

	if (( (int32_t)edi ) == 0) goto dotracks_n; //no owner


	if ((( ((tobject *)esi)->o_flags ) & ( ofAbsolute )) != 0) goto dotracks_n;


	if ((( ((tobject *)esi)->o_flags ) & ( ofMatrix )) == 0) goto dotracks_pos;

	ebx = 0; //ebx = n
dotracks_l2:
	fpu_reg10 = ( ((tobject *)(esi + ebx))->o_A_data[0] ); //n-th column of A
	fpu_reg11 = ( ((tobject *)(esi + ebx))->o_A_data[3] );
	fpu_reg12 = ( ((tobject *)(esi + ebx))->o_A_data[6] );

	edx = 0; //edx = m
dotracks_l1:
	fpu_reg13 = ( ((tobject *)(edi + edx))->o_A_data[0] ); //* m-th row of Ao
	fpu_reg13 = fpu_reg13 * fpu_reg10;
	fpu_reg14 = ( ((tobject *)(edi + edx))->o_A_data[1] );
	fpu_reg14 = fpu_reg14 * fpu_reg11;
	fpu_reg15 = ( ((tobject *)(edi + edx))->o_A_data[2] );
	fpu_reg15 = fpu_reg15 * fpu_reg12;
	fpu_reg14 = fpu_reg14 + fpu_reg15;
	fpu_reg13 = fpu_reg13 + fpu_reg14;
	eax = ( ebx + edx );
	((tobject *)(esi + eax))->o_A_data[0] = fpu_reg13; //= new element (m,n) of A

	edx = edx + ( 3 * 4 );

	if (edx < ( 3 * 3 * 4 )) goto dotracks_l1;





	ebx = ebx + ( 4 );

	if (ebx < ( 3 * 4 )) goto dotracks_l2;
dotracks_pos: //p = Ao*p + po

	fpu_reg10 = ( ((tobject *)esi)->o_p.x1 ); //column vektor p
	fpu_reg11 = ( ((tobject *)esi)->o_p.x2 );
	fpu_reg12 = ( ((tobject *)esi)->o_p.x3 );

	edx = 0; //edx = m for A
	ebx = 0; //ebx = m for p
dotracks_l3:
	fpu_reg13 = ( ((tobject *)(edi + edx))->o_A_data[0] ); //* m-th row of Ao
	fpu_reg13 = fpu_reg13 * fpu_reg10;
	fpu_reg14 = ( ((tobject *)(edi + edx))->o_A_data[1] );
	fpu_reg14 = fpu_reg14 * fpu_reg11;
	fpu_reg15 = ( ((tobject *)(edi + edx))->o_A_data[2] );
	fpu_reg15 = fpu_reg15 * fpu_reg12;
	fpu_reg14 = fpu_reg14 + fpu_reg15;
	fpu_reg13 = fpu_reg13 + fpu_reg14;
	fpu_reg13 = fpu_reg13 + ( ((tobject *)edi)->o_p_data[ebx] );
	((tobject *)esi)->o_p_data[ebx] = fpu_reg13; //= new element (m,1) of p

	edx = edx + ( 3 * 4 );
	ebx = ebx + 1;

	if (ebx < ( 3 )) goto dotracks_l3;





dotracks_n:
	stack_var00 = esi; //next hierarchy level
	esi = ( ((tobject *)esi)->o_child );
	dotracks(esi);
	esi = stack_var00;

	esi = ( ((tobject *)esi)->o_next ); //next object
	goto dotracks_next;
dotracks_weg:
	return;
}


extern "C" void calcbumpomni(uint32_t _esi) {
	uint32_t edi, ebx, esi = _esi;
//-> esi -> tbumpomni

//color track (3)
	edi = ( (uint32_t)&(((tbumpomni *)esi)->bo_color[0]) );
	dotrack(( ((tbumpomni *)esi)->bo_coltrack ), ( 3 ), edi);

//multiplier track (1)
	edi = ( (uint32_t)&(((tbumpomni *)esi)->bo_multiplier) );
	dotrack(( ((tbumpomni *)esi)->bo_multtrack ), ( 1 ), edi);

//hidetrack
	ebx = ( (uint32_t)&(((tbumpomni *)esi)->bo_hidetrack[0]) );
	edi = ( (uint32_t)&(((tbumpomni *)esi)->bo_hidden) );
	dohidetrack(ebx, edi);

	return;
}


extern "C" void initframecount(void) {
	uint32_t eax;
	eax = maincount;
	mainstart = eax;
//  sub mainstart,3500*66	       ;****
	return;
}


extern "C" void calcframecount(uint32_t _eax) {
	uint32_t eax = _eax;
//-> eax = add-frame
	mainstart = mainstart + eax;
	eax = maincount;
	eax = eax - mainstart;
	frame = eax;

// add maincount,160		    ;****
	return;
}





