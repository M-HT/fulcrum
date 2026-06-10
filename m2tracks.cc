/* encoding = IBM852 */
#include <SDL3/SDL.h>
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

struct tobject;

//VMT
typedef struct {
 int32_t o_size;
 void *o_readdata;
 int8_t *o_firstobj;
 int8_t *o_nextobj;
 void  (*o_dotracks)(tobject *esi);
 void *o_insert;
} tobjvmt;

//object:
//-------
#define ofMatrix    1
#define ofMesh	    2
#define ofAbsolute  4

typedef struct tobject {
 tobjvmt *o_vmt;

 uint32_t o_flags;

 //hierarchy-system: (pointer)
 tobject *o_owner;
 tobject *o_next;
 tobject *o_child;

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
 void *m_postrack;			//pointer

 //rotationtrack (4):
 void *m_rottrack;			//pointer

 //scaletrack (3):
// m_scaletrack	 dd ?			;pointer

 //morphtrack (2*vertices*3):
 void *m_morphtrack;			//pointer
// m_nmorphtrack  dd ?

 void *m_rgbtrack;			//pointer

 //hidetrack
 int32_t *m_hidetrack; 		//pointer, int key
 uint32_t m_hidekey;

} tmesh;


//camera:
//-------
typedef struct {
 tobject c_object;

 void *c_target;			//pointer

 //postrack (3):
 void *c_postrack;			//pointer

 //FOVtrack (1):
 void *c_FOVtrack;			//pointer
 float c_FOV;			//float

 //rolltrack (1):
 void *c_rolltrack;			//pointer
 float c_roll;			//float
} tcamera;

//target:
//-------
typedef struct {
 tobject t_object;

 //postrack (3):
 void *t_postrack;			//pointer
} ttarget;


//omni:
//-----
typedef struct {
 tobject i_object;

 //postrack (3):
 void *i_postrack;			//pointer

 //coltrack (3):
 void *i_coltrack;			//pointer
 float i_color[3];

 //hidetrack
 int32_t *i_hidetrack; 		//pointer, int key
 uint32_t i_hidekey;
} tomni;

//dummy:
//------
typedef struct {
 tobject d_object;

 int32_t d_special;

 //position
 float d_x[3];

 //hidetrack
 int32_t *d_hidetrack; 		//pointer, int key
 uint32_t d_hidekey;

 int32_t d_firstframe;
} tdummy;


typedef struct {
 //color track (3):
 void *bo_coltrack;			//pointer
 float bo_color[3];

 //multiplier track (1):
 void *bo_multtrack;			//pointer
 float bo_multiplier;

 //hidetrack
 int32_t *bo_hidetrack; 		//pointer, int key
 uint32_t bo_hidekey;
 uint32_t bo_hidden;
} tbumpomni;



//linear track:
//-------------
typedef struct {
 float *lt_startstate;
 uint32_t lt_startframe;
 float *lt_endstate;
 uint32_t lt_endframe;
 uint32_t lt_keys;
 void *lt_data;
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

static void doltrackM2(tltrack *_track, uint32_t typesize, CMV2Dot3DPos *edi) {
	realnum fpu_reg10, fpu_reg11, fpu_reg12;
	uint32_t edx1, ecx;
	int8_t *esi1;
	float *eax, *edx2, *esi2;

//edi -> result
	uint32_t temp;

doltrackM2_r:
	edx1 = frame;

	if (edx1 <= _track->lt_endframe) goto doltrackM2_i;
//end of key

	if (( _track->lt_keys ) > ( 0 )) goto doltrackM2_next;
//end of track
	fpu_reg10 = 1.0;
	goto doltrackM2_e;
doltrackM2_next: //get next key
	_track->lt_startframe = _track->lt_endframe; //old endframe is new startframe

	_track->lt_keys = _track->lt_keys - 1;

	esi1 = (int8_t *)_track->lt_data;
	temp = (int32_t)( *esi1 ); //get scale value
	esi1 = esi1 + 1;

	edx2 = _track->lt_endstate;
	ecx = 0;
doltrackM2_l:

	fpu_reg11 = (int32_t)( *(esi1 + ecx) ); //get differences

	fpu_reg11 = ldexp(fpu_reg11, (int32_t)temp);

	eax = _track->lt_startstate;

	fpu_reg12 = edx2[ecx];
	fpu_reg12 = fpu_reg12 + eax[ecx];
	eax[ecx] = fpu_reg12;
	edx2[ecx] = fpu_reg11;


	ecx = ecx + 1;

	if (ecx < typesize) goto doltrackM2_l;



	_track->lt_endframe = *((uint32_t *)(esi1 + ecx)); //get endframe

	_track->lt_data = (void *)( esi1 + ecx + (4) ); //update data pointer
	goto doltrackM2_r;
doltrackM2_i: //interpolate		;edx = frame
	temp = _track->lt_startframe;
	edx1 = edx1 - temp; //edx = (frame - sframe)
	fpu_reg10 = ( (int32_t)edx1 );
	temp = - ( (int32_t)temp );
	temp = temp + _track->lt_endframe; //eax = (eframe - sframe)
	fpu_reg11 = ( (int32_t)temp );
	fpu_reg10 = fpu_reg10 / fpu_reg11; //t = (frame - sframe) / (eframe - sframe);
doltrackM2_e:
	esi2 = _track->lt_startstate;
	edx2 = _track->lt_endstate;

	ecx = 0;
doltrackM2_l2:
	fpu_reg11 = edx2[ecx];
	fpu_reg11 = fpu_reg11 * fpu_reg10;
	fpu_reg11 = fpu_reg11 + esi2[ecx];
	edi->CMV2Dot3DPos__m_Pos__m_fX = fpu_reg11; //x1
	ecx = ecx + 1;

	fpu_reg11 = edx2[ecx];
	fpu_reg11 = fpu_reg11 * fpu_reg10;
	fpu_reg11 = fpu_reg11 + esi2[ecx];
	edi->CMV2Dot3DPos__m_Pos__m_fY = fpu_reg11; //x2
	ecx = ecx + 1;

	fpu_reg11 = edx2[ecx];
	fpu_reg11 = fpu_reg11 * fpu_reg10;
	fpu_reg11 = fpu_reg11 + esi2[ecx];
	edi->CMV2Dot3DPos__m_Pos__m_fZ = fpu_reg11; //x3
	ecx = ecx + 1;

	edi++;


	if (ecx < typesize) goto doltrackM2_l2;



	return;
}






static void dolRGBtrackM2(tltrack *_track, uint32_t typesize, CMV2Dot3DPos *edi) {
	realnum fpu_reg10, fpu_reg11, fpu_reg12;
	uint32_t edx1, ecx;
	int8_t *esi1;
	float *eax, *edx2, *esi2;

//edi -> result
	uint32_t temp;

dolRGBtrackM2_r:
	edx1 = frame;

	if (edx1 <= _track->lt_endframe) goto dolRGBtrackM2_i;
//end of key

	if (( _track->lt_keys ) > ( 0 )) goto dolRGBtrackM2_next;
//end of track
	fpu_reg10 = 1.0;
	goto dolRGBtrackM2_e;
dolRGBtrackM2_next: //get next key
	_track->lt_startframe = _track->lt_endframe; //old endframe is new startframe

	_track->lt_keys = _track->lt_keys - 1;

	esi1 = (int8_t *)_track->lt_data;
	temp = (int32_t)( *esi1 ); //get scale value
	esi1 = esi1 + 1;

	edx2 = _track->lt_endstate;
	ecx = 0;
dolRGBtrackM2_l:

	fpu_reg11 = (int32_t)( *(esi1 + ecx) ); //get differences

	fpu_reg11 = ldexp(fpu_reg11, (int32_t)temp);

	eax = _track->lt_startstate;

	fpu_reg12 = edx2[ecx];
	fpu_reg12 = fpu_reg12 + ( eax[ecx] );
	eax[ecx] = fpu_reg12;
	edx2[ecx] = fpu_reg11;


	ecx = ecx + 1;

	if (ecx < typesize) goto dolRGBtrackM2_l;



	_track->lt_endframe = *((uint32_t *)(esi1 + ecx)); //get endframe

	_track->lt_data = (void *)( esi1 + ecx + (4) ); //update data pointer
	goto dolRGBtrackM2_r;
dolRGBtrackM2_i: //interpolate		;edx = frame
	temp = _track->lt_startframe;
	edx1 = edx1 - temp; //edx = (frame - sframe)
	fpu_reg10 = ( (int32_t)edx1 );
	temp = - ( (int32_t)temp );
	temp = temp + _track->lt_endframe; //eax = (eframe - sframe)
	fpu_reg11 = ( (int32_t)temp );
	fpu_reg10 = fpu_reg10 / fpu_reg11; //t = (frame - sframe) / (eframe - sframe);
dolRGBtrackM2_e:
	esi2 = _track->lt_startstate;
	edx2 = _track->lt_endstate;

	ecx = 0;
dolRGBtrackM2_l2:
	fpu_reg11 = edx2[ecx];
	fpu_reg11 = fpu_reg11 * fpu_reg10;
	fpu_reg11 = fpu_reg11 + esi2[ecx];
//	      fchs
	edi->CMV2Dot3DPos__m_fR = fpu_reg11;
	ecx = ecx + 1;

	fpu_reg11 = edx2[ecx];
	fpu_reg11 = fpu_reg11 * fpu_reg10;
	fpu_reg11 = fpu_reg11 + esi2[ecx];
	edi->CMV2Dot3DPos__m_fG = fpu_reg11;
	ecx = ecx + 1;

	fpu_reg11 = edx2[ecx];
	fpu_reg11 = fpu_reg11 * fpu_reg10;
	fpu_reg11 = fpu_reg11 + esi2[ecx];
	edi->CMV2Dot3DPos__m_fB = fpu_reg11;
	ecx = ecx + 1;

	edi++;


	if (ecx < typesize) goto dolRGBtrackM2_l2;



	return;
}







extern "C" void m_dotracksM2(tmesh *esi) {
//-> esi -> tmesh

//hidetrack
	dohidetrack(&(esi->m_hidetrack), (int32_t *)&(esi->m_object.o_hidden));



	if ((( esi->m_object.o_flags ) & ( ofAbsolute )) != 0) goto m_dotracksM2_abs;

//rotation track (4)
	dorottrack(esi->m_rottrack, (float *)&(esi->m_object.o_A));

//position track (3)
	dotrack(esi->m_postrack, 3, (float *)&(esi->m_object.o_p));


	if (( esi->m_object.o_hidden ) > ( 0 )) goto m_dotracksM2_weg;
	goto m_dotracksM2_abs0;
m_dotracksM2_abs: //absolute

	if (( esi->m_object.o_hidden ) > ( 0 )) goto m_dotracksM2_weg;

//position track (3)
	doltrack(esi->m_postrack, 3, (float *)&(esi->m_object.o_p));


//morphtrack (2*vertices*3)

	if ((( esi->m_object.o_flags ) & ( mfMorph )) == 0) goto m_dotracksM2_m0;

	doltrackM2((tltrack *)esi->m_morphtrack, 3 * esi->m_PolygonObject.CMV2PolygonObject__m_iNumDot3DPos, esi->m_PolygonObject.CMV2PolygonObject__m_pDot3DPos);
m_dotracksM2_m0:

m_dotracksM2_abs0:

	if ((( esi->m_object.o_flags ) & ( mfRGBPreCalc + mfBumpPreCalc )) == 0) goto m_dotracksM2_NotRGBPreCalc;

	dolRGBtrackM2((tltrack *)esi->m_rgbtrack, 3 * esi->m_PolygonObject.CMV2PolygonObject__m_iNumDot3DPos, esi->m_PolygonObject.CMV2PolygonObject__m_pDot3DPos);

m_dotracksM2_NotRGBPreCalc:

m_dotracksM2_weg:
	return;
}



extern "C" void c_dotracksM2(tcamera *esi) {
//position track (3)
	dotrack(esi->c_postrack, ( 3 ), (float *)&(esi->c_object.o_p)); //eax, 3

//FOV track (1)
	dotrack(esi->c_FOVtrack, ( 1 ), (float *)&(esi->c_FOV)); //eax, 1

//roll track (1)
	dotrack(esi->c_rolltrack, ( 1 ), (float *)&(esi->c_roll)); //eax, 1

//no matrix
	return;
}



extern "C" void t_dotracksM2(ttarget *esi) {
//-> esi -> ttarget

//position track (3)
	dotrack(esi->t_postrack, 3, (float *)&(esi->t_object.o_p));

//no matrix
//t_dotracksM2_weg:
	return;
}


extern "C" void i_dotracksM2(tomni *esi) {
//position track (3)
	dotrack(esi->i_postrack, 3, (float *)&(esi->i_object.o_p));

//color track (3)
	dotrack(esi->i_coltrack, 3, (float *)&(esi->i_color[0]));

//hidetrack
	dohidetrack(&(esi->i_hidetrack), (int32_t *)&(esi->i_object.o_hidden));

//no matrix
//i_dotracksM2_weg:
	return;
}


extern "C" void d_dotracksM2(tdummy *esi) {
//hidetrack
	dohidetrack(&(esi->d_hidetrack), (int32_t *)&(esi->d_object.o_hidden));

//no matrix
//d_dotracksM2_weg:
	return;
}



extern "C" void doviewer(tcamera *esi, CMV2Camera *edi) {
	realnum fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13, fpu_reg14, fpu_reg15, fpu_reg16;
	uint32_t ecx;
	tobject *ebx;
//-> esi -> tcamera
//-> edi -> CMV2Camera
//<- esi -> tcamera
//<- edi -> CMV2Camera


	ebx = (tobject *)esi->c_target;

//direction vector u to target
	fpu_reg10 = esi->c_object.o_p.x1;
	edi->CMV2Camera__m_Pos__m_fX = fpu_reg10;
	fpu_reg10 = ebx->o_p.x1 - fpu_reg10;
	fpu_reg11 = esi->c_object.o_p.x2;
	edi->CMV2Camera__m_Pos__m_fY = fpu_reg11;
	fpu_reg11 = ebx->o_p.x2 - fpu_reg11;
	fpu_reg12 = esi->c_object.o_p.x3;
	edi->CMV2Camera__m_Pos__m_fZ = fpu_reg12;
	fpu_reg12 = ebx->o_p.x3 - fpu_reg12;

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
	edi->CMV2Camera__m_Front__m_fX = fpu_reg15;
	fpu_reg15 = fpu_reg11;
	fpu_reg15 = fpu_reg15 / fpu_reg14;
//      fchs
	edi->CMV2Camera__m_Front__m_fY = fpu_reg15;
	fpu_reg14 = fpu_reg12 / fpu_reg14;
//      fchs
	edi->CMV2Camera__m_Front__m_fZ = fpu_reg14;

//l1			;l1 = (u2,-u1,0)/sqrt(u1^2 + u2^2)
	fpu_reg13 = sqrt(fpu_reg13); //sqrt(u1^2 + u2^2)

	fpu_reg14 = fpu_reg11; //u2/sqrt(u1^2 + u2^2)
	fpu_reg14 = fpu_reg14 / fpu_reg13;
	edi->CMV2Camera__m_Right__m_fX = fpu_reg14;

	fpu_reg10 = fpu_reg10 / fpu_reg13; //-u1/sqrt(u1^2 + u2^2)
//remove u3
//remove u2
	fpu_reg10 = -fpu_reg10;
	edi->CMV2Camera__m_Right__m_fY = fpu_reg10;

	fpu_reg10 = 0.0; //0
	edi->CMV2Camera__m_Right__m_fZ = fpu_reg10;

//l2			;l2 = l1 x l3
	fpu_reg10 = edi->CMV2Camera__m_Front__m_fX; //b1
	fpu_reg11 = edi->CMV2Camera__m_Right__m_fY; //a2
	fpu_reg12 = edi->CMV2Camera__m_Front__m_fY; //b2
	fpu_reg13 = edi->CMV2Camera__m_Right__m_fX; //a1
	fpu_reg14 = edi->CMV2Camera__m_Front__m_fZ; //b3

	fpu_reg15 = fpu_reg11;
	fpu_reg15 = fpu_reg15 * fpu_reg14;
	edi->CMV2Camera__m_Down__m_fX = fpu_reg15; //a2*b3

	fpu_reg14 = fpu_reg14 * fpu_reg13;
	fpu_reg14 = -fpu_reg14;
	edi->CMV2Camera__m_Down__m_fY = fpu_reg14; //-a1*b3

	fpu_reg12 = fpu_reg12 * fpu_reg13;
	{ realnum tmp = fpu_reg11; fpu_reg11 = fpu_reg12; fpu_reg12 = tmp; }
	fpu_reg10 = fpu_reg10 * fpu_reg12;
	fpu_reg10 = fpu_reg11 - fpu_reg10;
	edi->CMV2Camera__m_Down__m_fZ = fpu_reg10; //a1*b2-a2*b1

//calc l1 and l2 scale factors for camera opening angle
	fpu_reg10 = c_pi_2; //1/tan(FOV) = tan(pi/2 - FOV)
	fpu_reg10 = fpu_reg10 - esi->c_FOV; //FOV = halve camera view angle
	fpu_reg10 = tan(fpu_reg10);
	fpu_reg11 = 1.0;

	fpu_reg11 = fpu_reg10;
	fpu_reg11 = fpu_reg11 * c_1_33;
//	 fmulp	 st(2),st		 ;st(0) = (x:y-ratio) * 1/tan(FOV)

//calc l1 and l2 scale factors for rotation about the view axis
	fpu_reg12 = esi->c_roll;
	fpu_reg13 = cos(fpu_reg12);
	fpu_reg12 = sin(fpu_reg12);

//rotate & scale l1 and l2
	ecx = ( 2 );
doviewer_l:
//l1' = (l1*cos(roll) + l2*sin(roll))/tan(FOV)
	fpu_reg14 = edi->CMV2Camera__LabelFirstVector_data[ecx];
	fpu_reg14 = fpu_reg14 * fpu_reg13; //cos
	fpu_reg15 = edi->CMV2Camera__m_Down__m_fX_data[ecx];
	fpu_reg15 = fpu_reg15 * fpu_reg12; //sin
	fpu_reg14 = fpu_reg14 + fpu_reg15;
	fpu_reg14 = fpu_reg14 * fpu_reg10; //1/tan(FOV)
//l2' = (l2*cos(roll) + l1*sin(roll))*(x:y ratio)/tan(FOV)
	fpu_reg15 = edi->CMV2Camera__m_Down__m_fX_data[ecx];
	fpu_reg15 = fpu_reg15 * fpu_reg13; //cos
	fpu_reg16 = edi->CMV2Camera__LabelFirstVector_data[ecx];
	fpu_reg16 = fpu_reg16 * fpu_reg12; //sin
	fpu_reg15 = fpu_reg15 - fpu_reg16;
	fpu_reg15 = fpu_reg15 * fpu_reg11; //(x:y ratio)/tan(FOV)

	fpu_reg15 = -fpu_reg15;
	edi->CMV2Camera__m_Down__m_fX_data[ecx] = fpu_reg15;
	edi->CMV2Camera__LabelFirstVector_data[ecx] = fpu_reg14;
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) >= 0) goto doviewer_l;





	return;
}


extern "C" void matrixmul(tobject *esi, CMV2Camera *edx, CMV2Camera *edi) {
	realnum fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13, fpu_reg14, fpu_reg15;
	uint32_t ecx, ebx;
//-> esi -> tobject
//-> edx -> CMV2Camera
//-> edi -> CMV2Camera


	if ((( esi->o_flags ) & ( ofAbsolute )) != 0) goto matrixmul_abs;
//calculate matrix A = Av * Ao (v:viewer, o:object)
	ebx = 0; //ebx = n
matrixmul_l0:
	fpu_reg10 = esi->o_A.A_1n[ebx]; //n-th column of A
	fpu_reg11 = esi->o_A.A_2n[ebx];
	fpu_reg12 = esi->o_A.A_3n[ebx];

	ecx = 0; //ecx = m
matrixmul_l1:
	fpu_reg13 = edx->CMV2Camera__LabelFirstVector.A_m1[ecx]; //* m-th row of Al
	fpu_reg13 = fpu_reg13 * fpu_reg10;
	fpu_reg14 = edx->CMV2Camera__LabelFirstVector.A_m2[ecx];
	fpu_reg14 = fpu_reg14 * fpu_reg11;
	fpu_reg15 = edx->CMV2Camera__LabelFirstVector.A_m3[ecx];
	fpu_reg15 = fpu_reg15 * fpu_reg12;
	fpu_reg14 = fpu_reg14 + fpu_reg15;
	fpu_reg13 = fpu_reg13 + fpu_reg14;
	edi->CMV2Camera__LabelFirstVector_data[ecx + ebx] = fpu_reg13; //= new element (m,n) of A

	ecx = ecx + 3;

	if (ecx < 3 * 3) goto matrixmul_l1;





	ebx = ebx + 1;

	if (ebx < ( 3 )) goto matrixmul_l0;

	goto matrixmul_abs0;
matrixmul_abs: //absolute: matrix A = Av (only position valid)
	ecx = ( 9 - 1 );
matrixmul_l:
	edi->CMV2Camera__LabelFirstVector_data[ecx] = edx->CMV2Camera__LabelFirstVector_data[ecx];
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) >= 0) goto matrixmul_l;

matrixmul_abs0:
//p = Av*(po-pv)
	fpu_reg10 = esi->o_p.x1; //(po-pv)
	fpu_reg10 = fpu_reg10 - edx->CMV2Camera__m_Pos__m_fX;
	fpu_reg11 = esi->o_p.x2;
	fpu_reg11 = fpu_reg11 - edx->CMV2Camera__m_Pos__m_fY;
	fpu_reg12 = esi->o_p.x3;
	fpu_reg12 = fpu_reg12 - edx->CMV2Camera__m_Pos__m_fZ;

	ebx = 0; //ebx = m
matrixmul_l2:
	ecx = ( ebx + ebx * 2 );
	fpu_reg13 = edx->CMV2Camera__LabelFirstVector.A_m1[ecx]; //* m-th row of Av
	fpu_reg13 = fpu_reg13 * fpu_reg10;
	fpu_reg14 = edx->CMV2Camera__LabelFirstVector.A_m2[ecx];
	fpu_reg14 = fpu_reg14 * fpu_reg11;
	fpu_reg15 = edx->CMV2Camera__LabelFirstVector.A_m3[ecx];
	fpu_reg15 = fpu_reg15 * fpu_reg12;
	fpu_reg14 = fpu_reg14 + fpu_reg15;
	fpu_reg13 = fpu_reg13 + fpu_reg14;
	edi->CMV2Camera__m_Pos__m_fX_data[ebx] = fpu_reg13; //= new element (m,1) of p

	ebx = ebx + 1;

	if (ebx < ( 3 )) goto matrixmul_l2;





//matrixmul_weg:
	return;
}



extern "C" void dotracks(tobject *esi) {
	realnum fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13, fpu_reg14, fpu_reg15;
	uint32_t edx, ebx;
	tobject *edi;
//-> esi = *this
//<- esi = NULL
dotracks_next:

	if (esi == NULL) goto dotracks_weg;

	esi->o_vmt->o_dotracks(esi); //virtual method to calculate the tracks

//A = matrix, p = position
//o = owner

//A = Ao*A (matrix multiplication)
	edi = esi->o_owner;

	if (edi == NULL) goto dotracks_n; //no owner


	if ((( esi->o_flags ) & ( ofAbsolute )) != 0) goto dotracks_n;


	if ((( esi->o_flags ) & ( ofMatrix )) == 0) goto dotracks_pos;

	ebx = 0; //ebx = n
dotracks_l2:
	fpu_reg10 = esi->o_A_data[0 + ebx]; //n-th column of A
	fpu_reg11 = esi->o_A_data[3 + ebx];
	fpu_reg12 = esi->o_A_data[6 + ebx];

	edx = 0; //edx = m
dotracks_l1:
	fpu_reg13 = edi->o_A_data[edx + 0]; //* m-th row of Ao
	fpu_reg13 = fpu_reg13 * fpu_reg10;
	fpu_reg14 = edi->o_A_data[edx + 1];
	fpu_reg14 = fpu_reg14 * fpu_reg11;
	fpu_reg15 = edi->o_A_data[edx + 2];
	fpu_reg15 = fpu_reg15 * fpu_reg12;
	fpu_reg14 = fpu_reg14 + fpu_reg15;
	fpu_reg13 = fpu_reg13 + fpu_reg14;
	esi->o_A_data[edx + ebx] = fpu_reg13; //= new element (m,n) of A

	edx = edx + 3;

	if (edx < 3 * 3) goto dotracks_l1;





	ebx = ebx + 1;

	if (ebx < 3) goto dotracks_l2;
dotracks_pos: //p = Ao*p + po

	fpu_reg10 = esi->o_p.x1; //column vektor p
	fpu_reg11 = esi->o_p.x2;
	fpu_reg12 = esi->o_p.x3;

	edx = 0; //edx = m for A
	ebx = 0; //ebx = m for p
dotracks_l3:
	fpu_reg13 = edi->o_A_data[edx + 0]; //* m-th row of Ao
	fpu_reg13 = fpu_reg13 * fpu_reg10;
	fpu_reg14 = edi->o_A_data[edx + 1];
	fpu_reg14 = fpu_reg14 * fpu_reg11;
	fpu_reg15 = edi->o_A_data[edx + 2];
	fpu_reg15 = fpu_reg15 * fpu_reg12;
	fpu_reg14 = fpu_reg14 + fpu_reg15;
	fpu_reg13 = fpu_reg13 + fpu_reg14;
	fpu_reg13 = fpu_reg13 + edi->o_p_data[ebx];
	esi->o_p_data[ebx] = fpu_reg13; //= new element (m,1) of p

	edx = edx + 3;
	ebx = ebx + 1;

	if (ebx < ( 3 )) goto dotracks_l3;





dotracks_n:
	dotracks(esi->o_child); //next hierarchy level

	esi = esi->o_next; //next object
	goto dotracks_next;
dotracks_weg:
	return;
}


extern "C" void calcbumpomni(tbumpomni *esi) {
//-> esi -> tbumpomni

//color track (3)
	dotrack(esi->bo_coltrack, 3, (float *)&(esi->bo_color[0]));

//multiplier track (1)
	dotrack(esi->bo_multtrack, 1, (float *)&(esi->bo_multiplier));

//hidetrack
	dohidetrack(&(esi->bo_hidetrack), (int32_t *)&(esi->bo_hidden));

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





