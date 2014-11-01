#include <cstdint>
#include <cmath>
#include <SDL/SDL.h>
#include "cc.h"
//.386                                    //pmode/w
//.model flat,prolog

//locals

//gfx = 1

//NULL = 0

#define erzlen 24000
#define scrollen 200000

#include "tracks.inc.h"
#include "int.inc.h"

//include ..\demo\vesa.inc
//extrn setpal:near
#include "vesa.inc.h"

//public erzdata, initedata, starterz

extern "C" int keypressed(void);


//b equ byte ptr
//w equ word ptr
//±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
//struc
//±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±

typedef struct {
 uint8_t p_red[256];
 uint8_t p_green[256];
 uint8_t p_blue[256];
} tpal;

typedef struct {
 float x1;
 float x2;
 float x3;
} tvec;

typedef struct {
 float p_x;
 float p_y;
} tpoint;

typedef struct {
 float cp_x;
 float cp_y;
 float cp_dx;
 float cp_dy;
 float cp_v;
} tcpoint;

typedef struct {
 uint32_t c_points;                         //int
 uint32_t c_pointlist;                      //pointer
 uint32_t c_ringlist;                       //pointer
 uint32_t c_normalslist;                    //pointer
 uint32_t c_lastpoint;                      //int
} tchain;

typedef struct {
 uint32_t e_ring;
 uint32_t e_sphere;                         //sphere
 uint32_t e_ringlist;                       //drop
 uint32_t e_normalslist;                    //drop normals
 uint32_t e_chains;                         //int
 uint32_t e_chain;                          //pointer

 uint32_t e_divtab;
 uint32_t e_buffer;

 uint32_t e_pal;
 uint32_t e_picdata;
 float e_xres1;
 float e_yres1;
 uint32_t e_pic1;
 float e_xres2;
 float e_yres2;
 uint32_t e_pic2;
 uint32_t e_paltab;

 uint32_t e_camtrack;
 uint32_t e_targettrack;

 uint32_t e_tempdata;

 uint32_t e_lines;
 uint32_t e_scroller;
} terzdata;

typedef struct {
 float r_xpos;
 float r_zpos;
 float r_xstep;
 float r_zstep;
 float r_sin;
 float r_cos;
 float r_radius;
} tringparams;


typedef struct {
 tvec v_p;             //local coordinate system
 tvec v_l1;
 tvec v_l2;
 tvec v_l3;
} tviewer;


typedef struct {
 float sp_x;
 float sp_y;
 float sp_z;
 union {
  float sp_data[2];
  struct {
   float sp_u;
   float sp_v;
  };
 };
} tscreenpoint;

#define flatpsize (sizeof(tscreenpoint))


//polygon point for scanline subdivision
typedef struct { //fixed size
 float s_p;
 float s_z;
 float s_u;
 float s_v;
} tscan;

//±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
//.data
//±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±

#define ringpoints      8
#define hsphererings    4
#define meltrings       6

#define ringsize (ringpoints*sizeof(tvec))

const static float c_rstep  = 0.7853981f;    //2*pi/ringpoints
const static float c_1_hsr  = 0.25f;         //1/(hsphererings)
const static float c_1_mr   = 0.142857f;     //1/(meltrings+1)
const static float c_asub   = 0.082f;        //4/(meltrings+1)^2
const static float c_amul   = 2.3f;//1.53125 //0.5/(4.0/(meltrings+1))^2
const static float c_ts     = 1.714285f;     //4/(meltrings+1)*(meltrings/2)   (tropf-start)
const static float c_tzstep = -0.571428f;    //4/(meltrings+1)                 (tropf-ystep)
const static float c_tx     = 0.428571f;     //0.5/(meltrings+1)*(meltrings/2) (tropf-x)

const static float c_0_5    = 0.5f;
const static float c_fun1fake = 1.001f;

//vesa
static uint32_t linbuf;
static uint32_t xbytes;
static uint32_t xres;
static uint32_t yres;
static uint32_t pbytes;
static SDL_Surface *screen;

static float xmid; //float
static float ymid; //float

//maincount       dd ?
static uint32_t mainstart;
//frame           dd 0                    //global frame counter
static uint32_t stepframe = 0;
#define step 128
const static float c_tscale = 0.0078125f; //1/step
static float t;

terzdata erzdata;

static tviewer viewer;
static tvec target;
//c_1_33 dd 1.333333
const static float c_xfov = 2.0f;
const static float c_yfov = 2.66667f;
//nscale dd 32760.0


//E_x     dd 0.0
static float E_y = -14.0f;//100.0
const static float E_slope = 0.05f;//0.01
const static float E_yadd = 0.2f;//0.06

const static float rscale = 0.1f;//0.01       //scale for rotate speed
const static float vscale = -0.1f;//-0.01      //scale for velocity
const static float c_16 = 16.0f;
const static float gravity = 4.0f;//1.0

const static float c_ymin = -30.0f;       //min y fr tropfen


const static tscreenpoint backpoints[4] = {
{-4.0f, 1.0f,-15.0f, {{0.001f, 0.999f}} },
{-4.0f, 1.0f, 15.0f, {{0.001f, 0.001f}} },
{22.0f, 1.0f, 15.0f, {{0.999f, 0.001f}} },
{22.0f, 1.0f,-15.0f, {{0.999f, 0.999f}} }};

//testvars
//static uint32_t tx;
//static uint32_t ty;
//const static float c_t = 8.0f;//18.0

//±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
//.code
//±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±

static void doviewer(void) {
	double fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13, fpu_reg14, fpu_reg15;
	uint32_t edi, ebx;

	ebx = ( (uint32_t)&(target) );
	edi = ( (uint32_t)&(viewer) );

//direction vector u to target
	fpu_reg10 = ( ((tvec *)ebx)->x1 );
	fpu_reg10 = fpu_reg10 - ( ((tviewer *)edi)->v_p.x1 );
	fpu_reg11 = ( ((tvec *)ebx)->x2 );
	fpu_reg11 = fpu_reg11 - ( ((tviewer *)edi)->v_p.x2 );
	fpu_reg12 = ( ((tvec *)ebx)->x3 );
	fpu_reg12 = fpu_reg12 - ( ((tviewer *)edi)->v_p.x3 );

//calculate local coordinate system l1, l2, l3 (l3 = view axis)
//l3 = u/|u|
//l1 = ((u) x (0,0,1))/|(u) x (0,0,1)|
//   = (u2,-u1,0)/sqrt(u1^2 + u2^2)
//l2 = l1 x l3  = | a2*b3        |
// (for l1 = a)   |-a1*b3        |
// (and l2 = b)   | a1*b2 - a2*b1|

	fpu_reg13 = fpu_reg10; //make unit length
	fpu_reg13 = fpu_reg13 * fpu_reg13; //u1*u1
	fpu_reg14 = fpu_reg11;
	fpu_reg14 = fpu_reg14 * fpu_reg14; //u2*u2
	fpu_reg13 = fpu_reg13 + fpu_reg14;
	fpu_reg14 = fpu_reg12; //st(1) = (u1^2 + u2^2)
	fpu_reg14 = fpu_reg14 * fpu_reg14; //u3*u3
	fpu_reg14 = fpu_reg14 + fpu_reg13;
	fpu_reg14 = sqrt(fpu_reg14); //sqrt(u1^2 + u2^2 + u3^2) (length of u)

//l3                     ;l3 = u/|u|
	fpu_reg15 = fpu_reg10;
	fpu_reg15 = fpu_reg15 / fpu_reg14;
	((tviewer *)edi)->v_l3.x1 = fpu_reg15;
	fpu_reg15 = fpu_reg11;
	fpu_reg15 = fpu_reg15 / fpu_reg14;
	((tviewer *)edi)->v_l3.x2 = fpu_reg15;
	fpu_reg14 = fpu_reg12 / fpu_reg14;
	((tviewer *)edi)->v_l3.x3 = fpu_reg14;

//l1                     ;l1 = (u2,-u1,0)/sqrt(u1^2 + u2^2)
	fpu_reg13 = sqrt(fpu_reg13); //sqrt(u1^2 + u2^2)

	fpu_reg14 = fpu_reg11; //u2/sqrt(u1^2 + u2^2)
	fpu_reg14 = fpu_reg14 / fpu_reg13;
	((tviewer *)edi)->v_l1.x1 = fpu_reg14;

	fpu_reg10 = fpu_reg10 / fpu_reg13; //-u1/sqrt(u1^2 + u2^2)
//remove u3
//remove u2
	fpu_reg10 = -fpu_reg10;
	((tviewer *)edi)->v_l1.x2 = fpu_reg10;

	fpu_reg10 = 0.0; //0
	((tviewer *)edi)->v_l1.x3 = fpu_reg10;

//l2                     ;l2 = l1 x l3
	fpu_reg10 = ( ((tviewer *)edi)->v_l3.x1 ); //b1
	fpu_reg11 = ( ((tviewer *)edi)->v_l1.x2 ); //a2
	fpu_reg12 = ( ((tviewer *)edi)->v_l3.x2 ); //b2
	fpu_reg13 = ( ((tviewer *)edi)->v_l1.x1 ); //a1
	fpu_reg14 = ( ((tviewer *)edi)->v_l3.x3 ); //b3

	fpu_reg15 = fpu_reg11;
	fpu_reg15 = fpu_reg15 * fpu_reg14;
//      fmul c_1_33
	((tviewer *)edi)->v_l2.x1 = fpu_reg15; //a2*b3

	fpu_reg14 = fpu_reg14 * fpu_reg13;
	fpu_reg14 = -fpu_reg14;
//      fmul c_1_33
	((tviewer *)edi)->v_l2.x2 = fpu_reg14; //-a1*b3

	fpu_reg12 = fpu_reg12 * fpu_reg13;
	{ double tmp = fpu_reg12; fpu_reg12 = fpu_reg11; fpu_reg11 = tmp; }
	fpu_reg10 = fpu_reg10 * fpu_reg12;
	fpu_reg10 = fpu_reg11 - fpu_reg10;
//      fmul c_1_33
	((tviewer *)edi)->v_l2.x3 = fpu_reg10; //a1*b2-a2*b1

	return;
}





static void fun1(double _fpu_reg9, double &_fpu_reg10) { //f(t) = sqrt(1 - t^2)
	double fpu_reg9 = _fpu_reg9, fpu_reg10, fpu_reg11;
//-> st(0) = t
//<- st(0) = f(t), st(1) = t
	fpu_reg10 = c_fun1fake;
//        fld1                            ;1
	fpu_reg11 = fpu_reg9;
	fpu_reg11 = fpu_reg11 * fpu_reg11; //t^2
	fpu_reg10 = fpu_reg10 - fpu_reg11;
//        fabs
	fpu_reg10 = sqrt(fpu_reg10);
	_fpu_reg10 = fpu_reg10;
}



static void fun2(double _fpu_reg8, double &_fpu_reg9) { //f(t) = a/2*(cos(2*pi*t) - 1) + 1
	double fpu_reg8 = _fpu_reg8, fpu_reg9 = _fpu_reg9, fpu_reg10, fpu_reg11;
//-> st(0) = a/2, st(1) = t
//<- st(0) = f(t), st(1) = t

	fpu_reg10 = M_PI;
	fpu_reg10 = fpu_reg10 + fpu_reg10;
	fpu_reg10 = fpu_reg10 * fpu_reg8;
	fpu_reg10 = cos(fpu_reg10);
	fpu_reg11 = 1.0;
	fpu_reg10 = fpu_reg10 - fpu_reg11;
	fpu_reg9 = fpu_reg9 * fpu_reg10;
	fpu_reg10 = 1.0;
	fpu_reg9 = fpu_reg9 + fpu_reg10;

	_fpu_reg9 = fpu_reg9;
}


static void fun3(double _fpu_reg8, double &_fpu_reg9) { //f(t) = a/2*sin(2*pi*t)
	double fpu_reg8 = _fpu_reg8, fpu_reg9 = _fpu_reg9, fpu_reg10;
//-> st(0) = a/2, st(1) = t
//<- st(0) = f(t), st(1) = t

	fpu_reg10 = M_PI;
	fpu_reg10 = fpu_reg10 + fpu_reg10;
	fpu_reg10 = fpu_reg10 * fpu_reg8;
	fpu_reg10 = sin(fpu_reg10);
	fpu_reg9 = fpu_reg9 * fpu_reg10;

	_fpu_reg9 = fpu_reg9;
}


static void makering(void) { //pascal
	double fpu_reg10, fpu_reg11, fpu_reg12;
	uint32_t ecx, edi;
//local   rp

	edi = erzdata.e_ring; //edi -> 2D-template-ring

//       fldpi
//       mov     rp,ringpoints/2
//       fild    rp
//       fdivp   st(1),st                ;2pi/ringpoints

	fpu_reg10 = 0.0;
	ecx = ( ringpoints );
makering_l:
	fpu_reg11 = fpu_reg10;
	fpu_reg12 = cos(fpu_reg11);
	fpu_reg11 = sin(fpu_reg11);
	((tpoint *)edi)->p_x = fpu_reg12;
	((tpoint *)edi)->p_y = fpu_reg11;

	fpu_reg10 = fpu_reg10 + c_rstep; //st,st(1)
	edi = edi + ( sizeof(tpoint) );
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto makering_l;


//        fstp    st

	return;
}



static void placering(uint32_t _esi, uint32_t &_edi) {
	double fpu_reg10;
	uint32_t ecx, edi = _edi, ebx, esi = _esi;
	uint32_t stack_var00, stack_var01;
//-> esi -> ringparams
//-> edi -> ring

	stack_var00 = ( 0 /*ebx*/ );
	stack_var01 = ( 0 /*ecx*/ );
	ebx = erzdata.e_ring; //esi -> 2D-template-ring

	ecx = ( ringpoints );

placering_l: //x = x*cos(a)*radius + xpos
//y = y*radius
//z = x*sin(a)*radius + zpos

	fpu_reg10 = ( ((tvec *)ebx)->x1 );
	fpu_reg10 = fpu_reg10 * ( ((tringparams *)esi)->r_sin );
	fpu_reg10 = -fpu_reg10;
	fpu_reg10 = fpu_reg10 * ( ((tringparams *)esi)->r_radius );
	fpu_reg10 = fpu_reg10 + ( ((tringparams *)esi)->r_xpos );
	((tvec *)edi)->x1 = fpu_reg10;

	fpu_reg10 = ( ((tvec *)ebx)->x2 );
	fpu_reg10 = fpu_reg10 * ( ((tringparams *)esi)->r_radius );
	((tvec *)edi)->x2 = fpu_reg10;

	fpu_reg10 = ( ((tvec *)ebx)->x1 );
	fpu_reg10 = fpu_reg10 * ( ((tringparams *)esi)->r_cos );
	fpu_reg10 = fpu_reg10 * ( ((tringparams *)esi)->r_radius );
	fpu_reg10 = fpu_reg10 + ( ((tringparams *)esi)->r_zpos );
	((tvec *)edi)->x3 = fpu_reg10;

	ebx = ebx + ( 8 );
	edi = edi + ( sizeof(tvec) );

	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto placering_l;

	ecx = stack_var01;
	ebx = stack_var00;
	_edi = edi;
}


static void placering_inc(uint32_t _esi, uint32_t &_edi) {
	double fpu_reg10;
	uint32_t edi = _edi, esi = _esi;

	placering(esi, edi);

	fpu_reg10 = ( ((tringparams *)esi)->r_xpos );
	fpu_reg10 = fpu_reg10 + ( ((tringparams *)esi)->r_xstep );
	((tringparams *)esi)->r_xpos = fpu_reg10;
	fpu_reg10 = ( ((tringparams *)esi)->r_zpos );
	fpu_reg10 = fpu_reg10 + ( ((tringparams *)esi)->r_zstep );
	((tringparams *)esi)->r_zpos = fpu_reg10;
//placering_inc_weg:
	_edi = edi;
}


static void calcsphere(void) {
	double fpu_reg10, fpu_reg11;
	uint32_t ecx, edi, esi;
	uint32_t stack_var00;
	tringparams ringparams;
	float x;

	edi = erzdata.e_sphere;

//make bottom hemisphere
	fpu_reg10 = 0.0;
	ringparams.r_xpos = fpu_reg10;
	ringparams.r_cos = fpu_reg10;
	ringparams.r_xstep = fpu_reg10;

	fpu_reg10 = 1.0;
	ringparams.r_zpos = fpu_reg10;
	fpu_reg10 = -fpu_reg10;
	ringparams.r_sin = fpu_reg10;
	x = fpu_reg10;
	fpu_reg10 = c_1_hsr;
	fpu_reg10 = -fpu_reg10;
	ringparams.r_zstep = fpu_reg10;

	ecx = ( hsphererings * 2 + 1 );
calcsphere_l:
	fpu_reg10 = x;
	fun1(fpu_reg10, fpu_reg11);
	ringparams.r_radius = fpu_reg11;
	fpu_reg10 = fpu_reg10 + c_1_hsr;
	x = fpu_reg10;

	stack_var00 = ( 0 /*esi*/ );
	esi = ( (uint32_t)&(ringparams) );
	placering_inc(esi, edi); //esi->ringparams, edi->ring
	esi = stack_var00;

	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto calcsphere_l;

//calcsphere_weg:
	return;
}


static void calcdrop(void) {
	double fpu_reg10, fpu_reg11, fpu_reg12;
	uint32_t eax, edx, ecx, edi, esi;
	uint32_t stack_var00, stack_var01;
	tringparams ringparams;
	float x;
	uint32_t rings, normals;

	edi = erzdata.e_ringlist;

//make melting segment
	fpu_reg10 = 0.0;
	ringparams.r_xpos = fpu_reg10;
	ringparams.r_xstep = fpu_reg10;
	fpu_reg10 = c_ts;
	ringparams.r_zpos = fpu_reg10;
	fpu_reg10 = c_tzstep;
	ringparams.r_zstep = fpu_reg10;

	fpu_reg10 = 0.0;
	ringparams.r_cos = fpu_reg10;
	fpu_reg10 = 1.0;
	fpu_reg10 = -fpu_reg10;
	ringparams.r_sin = fpu_reg10;

	fpu_reg10 = c_tx;
	x = fpu_reg10;
	ecx = ( meltrings / 2 );
calcdrop_l0:

	fpu_reg10 = x;
	fpu_reg11 = c_0_5; //a_2
	fun2(fpu_reg10, fpu_reg11);
	ringparams.r_radius = fpu_reg11;
	fpu_reg10 = fpu_reg10 - c_1_mr;
	x = fpu_reg10;

	stack_var00 = ( 0 /*esi*/ );
	esi = ( (uint32_t)&(ringparams) );
	placering_inc(esi, edi); //esi->ringparams, edi->ring
	esi = stack_var00;

	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto calcdrop_l0;


//make normals of melting segment
	stack_var00 = edi;
	edi = erzdata.e_normalslist;

	fpu_reg10 = c_tx;
	x = fpu_reg10;
	ecx = ( meltrings / 2 );
calcdrop_l1:
	fpu_reg10 = x;
	fpu_reg11 = c_0_5; //a_2
	fun3(fpu_reg10, fpu_reg11); //slope of melting area
	fun1(fpu_reg11, fpu_reg12); //calc radius
	ringparams.r_radius = fpu_reg12;

	ringparams.r_zpos = fpu_reg11; //displacement

	fpu_reg10 = fpu_reg10 - c_1_mr; //increment x
	x = fpu_reg10;


	stack_var01 = ( 0 /*esi*/ );
	esi = ( (uint32_t)&(ringparams) );
	placering(esi, edi); //esi->ringparams, edi->normals
	esi = stack_var01;

	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto calcdrop_l1;

	normals = edi;
	edi = stack_var00;
	rings = edi;

//make bottom hemisphere
	fpu_reg10 = 0.0;
	ringparams.r_zpos = fpu_reg10;
	fpu_reg10 = c_1_hsr;
	fpu_reg10 = -fpu_reg10;
	ringparams.r_zstep = fpu_reg10;

	fpu_reg10 = 0.0;
	x = fpu_reg10;

	ecx = ( hsphererings + 1 );
calcdrop_l2:
	fpu_reg10 = x;
	fun1(fpu_reg10, fpu_reg11);
	ringparams.r_radius = fpu_reg11;
	fpu_reg10 = fpu_reg10 + c_1_hsr;
	x = fpu_reg10;

	stack_var00 = ( 0 /*esi*/ );
	esi = ( (uint32_t)&(ringparams) );
	placering_inc(esi, edi); //esi->ringparams, edi->ring
	esi = stack_var00;

	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto calcdrop_l2;

//make normals of bottom hemisphere
	eax = rings;
	edx = normals;
calcdrop_l3:
	fpu_reg10 = ( ((tvec *)eax)->x1 );
	((tvec *)edx)->x1 = fpu_reg10;
	fpu_reg10 = ( ((tvec *)eax)->x2 );
	((tvec *)edx)->x2 = fpu_reg10;
	fpu_reg10 = ( ((tvec *)eax)->x3 );
	((tvec *)edx)->x3 = fpu_reg10;

	eax = eax + ( sizeof(tvec) );
	edx = edx + ( sizeof(tvec) );

	if (eax < edi) goto calcdrop_l3;


	return;
}



static void initchains(void) {
	double fpu_reg10, fpu_reg11, fpu_reg12;
	uint32_t eax, edx, ecx, edi, ebx, esi;
	uint32_t stack_var00;

	tringparams ringparams;
	uint32_t z;
	float x;
	uint32_t rings, normals;

	esi = erzdata.e_chain;
	ecx = erzdata.e_chains;
	z = ecx;
initchains_l: //make chain

	ebx = ( ((tchain *)esi)->c_pointlist ); //esi -> tchain
	edi = ( ((tchain *)esi)->c_ringlist );

//make top hemisphere

	fpu_reg10 = ( ((tcpoint *)ebx)[1].cp_y );
	fpu_reg10 = fpu_reg10 - ( ((tcpoint *)ebx)->cp_y );
	fpu_reg11 = ( ((tcpoint *)ebx)[1].cp_x );
	fpu_reg11 = fpu_reg11 - ( ((tcpoint *)ebx)->cp_x );

	fpu_reg10 = atan2(fpu_reg10, fpu_reg11);
	fpu_reg11 = cos(fpu_reg10);
	fpu_reg10 = sin(fpu_reg10);

	fpu_reg12 = ( ((tcpoint *)ebx)->cp_x );
	fpu_reg12 = fpu_reg12 - fpu_reg11;
	ringparams.r_xpos = fpu_reg12;
	fpu_reg12 = ( ((tcpoint *)ebx)->cp_y );
	fpu_reg12 = fpu_reg12 - fpu_reg10;
	ringparams.r_zpos = fpu_reg12;

	ringparams.r_cos = fpu_reg11;
	fpu_reg11 = fpu_reg11 * c_1_hsr;
	ringparams.r_xstep = fpu_reg11;
	ringparams.r_sin = fpu_reg10;
	fpu_reg10 = fpu_reg10 * c_1_hsr;
	ringparams.r_zstep = fpu_reg10;


	fpu_reg10 = 1.0;
	x = fpu_reg10;

	ecx = ( hsphererings + 1 );
initchains_l0:
	fpu_reg10 = x;
	fun1(fpu_reg10, fpu_reg11);
	ringparams.r_radius = fpu_reg11;
	fpu_reg10 = fpu_reg10 - c_1_hsr;
	x = fpu_reg10;

	stack_var00 = esi;
	esi = ( (uint32_t)&(ringparams) );
	placering_inc(esi, edi); //esi->ringparams, edi->ring
	esi = stack_var00;

	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto initchains_l0;

//make normals of top hemisphere
	eax = ( ((tchain *)esi)->c_ringlist ); //esi -> tchain
	edx = ( ((tchain *)esi)->c_normalslist );
initchains_l1:
	fpu_reg10 = ( ((tvec *)eax)->x1 );
	fpu_reg10 = fpu_reg10 - ( ((tcpoint *)ebx)->cp_x );
	((tvec *)edx)->x1 = fpu_reg10;
	fpu_reg10 = ( ((tvec *)eax)->x2 );
	((tvec *)edx)->x2 = fpu_reg10;
	fpu_reg10 = ( ((tvec *)eax)->x3 );
	fpu_reg10 = fpu_reg10 - ( ((tcpoint *)ebx)->cp_y );
	((tvec *)edx)->x3 = fpu_reg10;

	eax = eax + ( sizeof(tvec) );
	edx = edx + ( sizeof(tvec) );

	if (eax < edi) goto initchains_l1;
	rings = edi;
	normals = edx;


//make middle points
	ebx = ebx + ( sizeof(tcpoint) );
	fpu_reg10 = 1.0;
	ringparams.r_radius = fpu_reg10;
	ecx = ( ((tchain *)esi)->c_points ); //esi -> tchain
	ecx = ecx - ( 2 );

initchains_l2: //make ring

	fpu_reg10 = ( ((tcpoint *)ebx)->cp_x );
	ringparams.r_xpos = fpu_reg10;
	fpu_reg10 = ( ((tcpoint *)ebx)->cp_y );
	ringparams.r_zpos = fpu_reg10;

	fpu_reg10 = ( ((tcpoint *)ebx)[1].cp_y );
	fpu_reg10 = fpu_reg10 - ( ((tcpoint *)ebx)[-1].cp_y );
	fpu_reg11 = ( ((tcpoint *)ebx)[1].cp_x );
	fpu_reg11 = fpu_reg11 - ( ((tcpoint *)ebx)[-1].cp_x );

	fpu_reg10 = atan2(fpu_reg10, fpu_reg11);
	fpu_reg11 = cos(fpu_reg10);
	fpu_reg10 = sin(fpu_reg10);

	ringparams.r_cos = fpu_reg11;
	ringparams.r_sin = fpu_reg10;

	stack_var00 = esi;
	esi = ( (uint32_t)&(ringparams) );
	placering(esi, edi); //esi->ringparams, edi->ring
	esi = stack_var00;

//make normals of ring
	eax = rings;
	edx = normals;
initchains_l3:
	fpu_reg10 = ( ((tvec *)eax)->x1 );
	fpu_reg10 = fpu_reg10 - ( ((tcpoint *)ebx)->cp_x );
	((tvec *)edx)->x1 = fpu_reg10;
	fpu_reg10 = ( ((tvec *)eax)->x2 );
	((tvec *)edx)->x2 = fpu_reg10;
	fpu_reg10 = ( ((tvec *)eax)->x3 );
	fpu_reg10 = fpu_reg10 - ( ((tcpoint *)ebx)->cp_y );
	((tvec *)edx)->x3 = fpu_reg10;

	eax = eax + ( sizeof(tvec) );
	edx = edx + ( sizeof(tvec) );

	if (eax < edi) goto initchains_l3;
	rings = edi;
	normals = edx;

	ebx = ebx + ( sizeof(tcpoint) );
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto initchains_l2;


//outer loop
	esi = esi + ( sizeof(tchain) );
	z = ( (int32_t)z ) - 1;
	if (( (int32_t)z ) != 0) goto initchains_l;

//initchains_weg:
	return;
}


static void updatepoints(void) {
	double fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13;
	uint32_t eax, edx, ecx, ebx, esi;
	uint32_t z;
	fpu_reg10 = E_y;
	fpu_reg10 = fpu_reg10 + E_yadd;
	E_y = fpu_reg10;
	esi = erzdata.e_chain;
	ecx = erzdata.e_chains;
	z = ecx;
updatepoints_l:
	ebx = ( ((tchain *)esi)->c_pointlist );

	edx = ( ((tchain *)esi)->c_lastpoint );
	eax = edx;
	eax = ( (int32_t)eax ) - 1;
	if (( (int32_t)eax ) < 0) goto updatepoints_f; //edx = c_lastpoint
//melting point
	eax = ( (int32_t)eax ) * ( sizeof(tcpoint) );
	ebx = ebx + eax;

	fpu_reg10 = ( ((tcpoint *)ebx)->cp_x ); //add last dx
	fpu_reg10 = fpu_reg10 + ( ((tcpoint *)ebx)->cp_dx );
	((tcpoint *)ebx)->cp_x = fpu_reg10; //fstp
	fpu_reg10 = fpu_reg10 * E_slope;
	fpu_reg11 = ( ((tcpoint *)ebx)->cp_y ); //add last dy
	fpu_reg11 = fpu_reg11 + ( ((tcpoint *)ebx)->cp_dy );
	((tcpoint *)ebx)->cp_y = fpu_reg11;

	fpu_reg10 = fpu_reg10 + fpu_reg11;
	fpu_reg11 = E_y; //melting started if y < E_y



	if (fpu_reg11 < fpu_reg10) goto updatepoints_mw; //edx = c_lastpoint

	fpu_reg10 = ( ((tcpoint *)ebx)->cp_y ); //dy
	fpu_reg10 = fpu_reg10 - ( ((tcpoint *)ebx)[-1].cp_y );
	fpu_reg11 = ( ((tcpoint *)ebx)->cp_x ); //dx
	fpu_reg11 = fpu_reg11 - ( ((tcpoint *)ebx)[-1].cp_x );

//energiezufuhr
//        fld     [ebx].cp_x
//        fsub    E_x
//        fabs
//        fmul    E_slope
//        fchs
	fpu_reg12 = 1.0;
//        faddp   st(1),st

	fpu_reg12 = fpu_reg12 + ( ((tcpoint *)ebx)->cp_v ); //update speed



	if (fpu_reg12 > 0.0) goto updatepoints_gt0;

	fpu_reg12 = 0.0;
updatepoints_gt0:
	((tcpoint *)ebx)->cp_v = fpu_reg12;

//rotate
	fpu_reg12 = fpu_reg12 * fpu_reg11; //rotate speed = v*dx*rscale
	fpu_reg12 = fpu_reg12 * rscale;

	fpu_reg13 = fpu_reg12;
	fpu_reg11 = fpu_reg11 * fpu_reg13;
	fpu_reg10 = fpu_reg10 * fpu_reg12;

	fpu_reg12 = ( ((tcpoint *)ebx)->cp_v ); //move down
	fpu_reg12 = fpu_reg12 * vscale;
	fpu_reg11 = fpu_reg12 - fpu_reg11;
	((tcpoint *)ebx)->cp_dy = fpu_reg11; //dy = v*vscale - dx*(rotate speed)

	((tcpoint *)ebx)->cp_dx = fpu_reg10; //dx = dy*(rotate speed)

	fpu_reg10 = ( ((tcpoint *)ebx)->cp_x ); //disconnect point from chain?
	fpu_reg10 = fpu_reg10 + ( ((tcpoint *)ebx)->cp_dx );
	fpu_reg10 = fpu_reg10 - ( ((tcpoint *)ebx)[-1].cp_x );
	fpu_reg10 = fpu_reg10 * fpu_reg10;
	fpu_reg11 = ( ((tcpoint *)ebx)->cp_y );
	fpu_reg11 = fpu_reg11 + ( ((tcpoint *)ebx)->cp_dy );
	fpu_reg11 = fpu_reg11 - ( ((tcpoint *)ebx)[-1].cp_y );
	fpu_reg11 = fpu_reg11 * fpu_reg11;
	fpu_reg10 = fpu_reg10 + fpu_reg11;



	if (fpu_reg10 < c_16) goto updatepoints_mw;
	((tchain *)esi)->c_lastpoint = ( ((tchain *)esi)->c_lastpoint ) - 1;
//edx = c_lastpoint
	if (( (int32_t)edx ) > ( 2 )) goto updatepoints_mw;
	((tchain *)esi)->c_lastpoint = ( 0 ); //last 2 points
updatepoints_mw:
	ebx = ebx + ( sizeof(tcpoint) );
updatepoints_f: //falling points

	ecx = ( ((tchain *)esi)->c_points );
	ecx = ( (int32_t)ecx ) - ( (int32_t)edx ); //edx = c_lastpoint
	if (( (int32_t)ecx ) == 0) goto updatepoints_1;
updatepoints_f_l:
	fpu_reg10 = ( ((tcpoint *)ebx)->cp_x ); //add last dx
	fpu_reg10 = fpu_reg10 + ( ((tcpoint *)ebx)->cp_dx );
	((tcpoint *)ebx)->cp_x = fpu_reg10;
	fpu_reg10 = ( ((tcpoint *)ebx)->cp_y ); //add last dy
	fpu_reg10 = fpu_reg10 + ( ((tcpoint *)ebx)->cp_dy );
	((tcpoint *)ebx)->cp_y = fpu_reg10;


	fpu_reg10 = 0.0; //dx = 0
	((tcpoint *)ebx)->cp_dx = fpu_reg10;
	fpu_reg10 = ( ((tcpoint *)ebx)->cp_v ); //v = v + gravity
	fpu_reg10 = fpu_reg10 + gravity;
	((tcpoint *)ebx)->cp_v = fpu_reg10;
	fpu_reg10 = fpu_reg10 * vscale; //dy = v*vscale
	((tcpoint *)ebx)->cp_dy = fpu_reg10;

	ebx = ebx + ( sizeof(tcpoint) );
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto updatepoints_f_l;
updatepoints_1:
	esi = esi + ( sizeof(tchain) );
	z = ( (int32_t)z ) - 1;
	if (( (int32_t)z ) != 0) goto updatepoints_l;

	return;
}



static void updatechains(void) {
	double fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13, fpu_reg14, fpu_reg15, fpu_reg16;
	uint32_t eax, edx, ecx, edi, ebx, esi;
	uint32_t stack_var00, stack_var01;
	tringparams ringparams;
	tringparams ringparams2;
	uint32_t z;
	float angle, dangle, a_2, x;
	uint32_t rings, normals;

	esi = erzdata.e_chain;
	ecx = erzdata.e_chains;
	z = ecx;
updatechains_l:

	eax = ( ((tchain *)esi)->c_lastpoint );
	ebx = eax;

//make melting segment

	ebx = ebx - 1;
	ebx = ( (int32_t)ebx ) - 1;
	if (( (int32_t)ebx ) < 0) goto updatechains_w; //edx = c_lastpoint
	if (( (int32_t)ebx ) == 0) goto updatechains_last2;
	ebx = ( (int32_t)ebx ) * ( sizeof(tcpoint) );
	ebx = ebx + ( ((tchain *)esi)->c_pointlist );

	eax = eax + ( hsphererings - 2 );
	eax = ( (int32_t)eax ) * ( ringsize );
	edi = eax;
	edi = edi + ( ((tchain *)esi)->c_ringlist );
	eax = eax + ( ((tchain *)esi)->c_normalslist );
	normals = eax;

	fpu_reg10 = t;
	fpu_reg11 = ( ((tcpoint *)ebx)[1].cp_dy ); //position of last point
	fpu_reg11 = fpu_reg11 * fpu_reg10;
	fpu_reg11 = fpu_reg11 + ( ((tcpoint *)ebx)[1].cp_y ); //(x = x + t*dx)
	fpu_reg12 = ( ((tcpoint *)ebx)[1].cp_dx );
	fpu_reg12 = fpu_reg12 * fpu_reg10;
	fpu_reg12 = fpu_reg12 + ( ((tcpoint *)ebx)[1].cp_x ); //(y = y + t*dy)

	fpu_reg13 = ( ((tcpoint *)ebx)->cp_y ); //angle of last point
	ringparams.r_zpos = fpu_reg13;
	fpu_reg13 = fpu_reg11 - fpu_reg13;
	fpu_reg13 = fpu_reg13 * c_1_mr;
	ringparams.r_zstep = fpu_reg13;
	fpu_reg14 = ( ((tcpoint *)ebx)->cp_x );
	ringparams.r_xpos = fpu_reg14;
	fpu_reg14 = fpu_reg12 - fpu_reg14;
	fpu_reg14 = fpu_reg14 * c_1_mr;
	ringparams.r_xstep = fpu_reg14;

	fpu_reg15 = fpu_reg13; //calc a/2
	fpu_reg15 = fpu_reg15 * fpu_reg15;
	fpu_reg16 = fpu_reg14;
	fpu_reg16 = fpu_reg16 * fpu_reg16;
	fpu_reg15 = fpu_reg15 + fpu_reg16;
	fpu_reg15 = fpu_reg15 - c_asub;



	if (fpu_reg15 > 0.0) goto updatechains_gt0;

	fpu_reg15 = 0.0;
updatechains_gt0:
	fpu_reg15 = fpu_reg15 * c_amul;
	a_2 = fpu_reg15;

	fpu_reg13 = atan2(fpu_reg13, fpu_reg14);

	fpu_reg14 = ( ((tcpoint *)ebx)[-1].cp_y ); //angle of second last point
	fpu_reg14 = fpu_reg11 - fpu_reg14;
	fpu_reg15 = ( ((tcpoint *)ebx)[-1].cp_x );
	fpu_reg15 = fpu_reg12 - fpu_reg15;
	fpu_reg14 = atan2(fpu_reg14, fpu_reg15);
	angle = fpu_reg14;
	fpu_reg13 = fpu_reg13 - fpu_reg14;
	fpu_reg14 = M_PI;



	if (fpu_reg14 < fpu_reg13) goto updatechains_0;
	fpu_reg14 = -fpu_reg14;



	if (fpu_reg14 < fpu_reg13) goto updatechains_1;
updatechains_0:
	fpu_reg14 = fpu_reg14 + fpu_reg14;
	fpu_reg13 = fpu_reg13 - fpu_reg14;
	goto updatechains_2;
updatechains_1:

updatechains_2:
	fpu_reg13 = fpu_reg13 * c_1_mr;
	dangle = fpu_reg13;

//remove position of last point

//remove t

	fpu_reg10 = 0.0;
	x = fpu_reg10;

	ecx = ( meltrings + 1 );

updatechains_l0:

	fpu_reg10 = angle;
	fpu_reg11 = cos(fpu_reg10);
	fpu_reg10 = sin(fpu_reg10);
	ringparams.r_cos = fpu_reg11;
	ringparams2.r_cos = fpu_reg11;
	ringparams.r_sin = fpu_reg10;
	ringparams2.r_sin = fpu_reg10;


	fpu_reg10 = x;
	fpu_reg11 = a_2;
	fun2(fpu_reg10, fpu_reg11);
	ringparams.r_radius = fpu_reg11;

	fpu_reg11 = a_2;
	fun3(fpu_reg10, fpu_reg11); //slope of melting area
	fun1(fpu_reg11, fpu_reg12); //calc radius
	ringparams2.r_radius = fpu_reg12;

	fpu_reg12 = ringparams2.r_cos; //calc displacement
	fpu_reg12 = fpu_reg12 * fpu_reg11;
	ringparams2.r_xpos = fpu_reg12;
	fpu_reg11 = fpu_reg11 * ringparams2.r_sin;
	ringparams2.r_zpos = fpu_reg11;

	fpu_reg10 = fpu_reg10 + c_1_mr;
	x = fpu_reg10;

	stack_var00 = esi;
	esi = ( (uint32_t)&(ringparams) );
	placering_inc(esi, edi); //esi->ringparams, edi->ring

	stack_var01 = edi;
	edi = normals;
	esi = ( (uint32_t)&(ringparams2) );
	placering(esi, edi); //esi->ringparams2, edi->normals
	normals = edi;
	edi = stack_var01;

	esi = stack_var00;

	fpu_reg10 = angle;
	fpu_reg10 = fpu_reg10 + dangle;
	angle = fpu_reg10;

	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto updatechains_l0;

	rings = edi;

//make bottom hemisphere
	fpu_reg10 = ringparams.r_xpos; //keep position
	ringparams2.r_xpos = fpu_reg10;
	fpu_reg10 = ringparams.r_zpos;
	ringparams2.r_zpos = fpu_reg10;

	fpu_reg10 = 0.0;
	x = fpu_reg10;

	fpu_reg10 = ringparams.r_cos;
	fpu_reg10 = fpu_reg10 * c_1_hsr;
	ringparams.r_xstep = fpu_reg10;
	fpu_reg10 = ringparams.r_sin;
	fpu_reg10 = fpu_reg10 * c_1_hsr;
	ringparams.r_zstep = fpu_reg10;

	ecx = ( hsphererings + 1 );
updatechains_l1:
	fpu_reg10 = x;
	fun1(fpu_reg10, fpu_reg11);
	ringparams.r_radius = fpu_reg11;
//        fld     x
	fpu_reg10 = fpu_reg10 + c_1_hsr;
	x = fpu_reg10;

	stack_var00 = esi;
	esi = ( (uint32_t)&(ringparams) );
	placering_inc(esi, edi); //esi->ringparams, edi->ring
	esi = stack_var00;

	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto updatechains_l1;

//make normals of bottom hemisphere
	eax = rings;
	edx = normals;
updatechains_l2:
	fpu_reg10 = ( ((tvec *)eax)->x1 );
	fpu_reg10 = fpu_reg10 - ringparams2.r_xpos; //center point of sphere
	((tvec *)edx)->x1 = fpu_reg10;
	fpu_reg10 = ( ((tvec *)eax)->x2 );
	((tvec *)edx)->x2 = fpu_reg10;
	fpu_reg10 = ( ((tvec *)eax)->x3 );
	fpu_reg10 = fpu_reg10 - ringparams2.r_zpos;
	((tvec *)edx)->x3 = fpu_reg10;

	eax = eax + ( sizeof(tvec) );
	edx = edx + ( sizeof(tvec) );

	if (eax < edi) goto updatechains_l2;

	goto updatechains_w;
updatechains_last2: //the last 2 points

	ebx = ( ((tchain *)esi)->c_pointlist );
	edi = ( ((tchain *)esi)->c_ringlist );

//make top hemisphere

	fpu_reg10 = t;
	fpu_reg11 = ( ((tcpoint *)ebx)[1].cp_dy ); //position of last point
	fpu_reg11 = fpu_reg11 * fpu_reg10;
	fpu_reg11 = fpu_reg11 + ( ((tcpoint *)ebx)[1].cp_y ); //(x = x + t*dx)
	fpu_reg12 = ( ((tcpoint *)ebx)[1].cp_dx );
	fpu_reg12 = fpu_reg12 * fpu_reg10;
	fpu_reg12 = fpu_reg12 + ( ((tcpoint *)ebx)[1].cp_x ); //(y = y + t*dy)

	fpu_reg13 = ( ((tcpoint *)ebx)->cp_y ); //angle of last point
	fpu_reg13 = fpu_reg11 - fpu_reg13;
	fpu_reg13 = fpu_reg13 * c_1_mr;
	ringparams2.r_zstep = fpu_reg13; //zstep for melting segment
	fpu_reg14 = ( ((tcpoint *)ebx)->cp_x );
	fpu_reg14 = fpu_reg12 - fpu_reg14;
	fpu_reg14 = fpu_reg14 * c_1_mr;
	ringparams2.r_xstep = fpu_reg14; //xstep for melting segment

	fpu_reg15 = fpu_reg13; //calc a/2
	fpu_reg15 = fpu_reg15 * fpu_reg15;
	fpu_reg16 = fpu_reg14;
	fpu_reg16 = fpu_reg16 * fpu_reg16;
	fpu_reg15 = fpu_reg15 + fpu_reg16;
	fpu_reg15 = fpu_reg15 - c_asub;



	if (fpu_reg15 > 0.0) goto updatechains_z1;

	fpu_reg15 = 0.0;
updatechains_z1:
	fpu_reg15 = fpu_reg15 * c_amul;
	a_2 = fpu_reg15;

	fpu_reg13 = atan2(fpu_reg13, fpu_reg14);
	fpu_reg14 = cos(fpu_reg13);
	fpu_reg13 = sin(fpu_reg13);

	fpu_reg15 = ( ((tcpoint *)ebx)->cp_x );
	fpu_reg15 = fpu_reg15 - fpu_reg14;
	ringparams.r_xpos = fpu_reg15;
	fpu_reg15 = ( ((tcpoint *)ebx)->cp_y );
	fpu_reg15 = fpu_reg15 - fpu_reg13;
	ringparams.r_zpos = fpu_reg15;

	ringparams.r_cos = fpu_reg14;
	ringparams2.r_cos = fpu_reg14;
	fpu_reg14 = fpu_reg14 * c_1_hsr;
	ringparams.r_xstep = fpu_reg14;
	ringparams.r_sin = fpu_reg13;
	ringparams2.r_sin = fpu_reg13;
	fpu_reg13 = fpu_reg13 * c_1_hsr;
	ringparams.r_zstep = fpu_reg13;





	fpu_reg10 = 1.0;
	x = fpu_reg10;

	ecx = ( hsphererings );
updatechains_l3:
	fpu_reg10 = x;
	fun1(fpu_reg10, fpu_reg11);
	ringparams.r_radius = fpu_reg11;
	fpu_reg10 = fpu_reg10 - c_1_hsr;
	x = fpu_reg10;

	stack_var00 = esi;
	esi = ( (uint32_t)&(ringparams) );
	placering_inc(esi, edi); //esi->ringparams, edi->ring
	esi = stack_var00;

	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto updatechains_l3;

//make normals of top hemisphere
	eax = ( ((tchain *)esi)->c_ringlist ); //esi -> tchain
	edx = ( ((tchain *)esi)->c_normalslist );
updatechains_l4:
	fpu_reg10 = ( ((tvec *)eax)->x1 );
	fpu_reg10 = fpu_reg10 - ( ((tcpoint *)ebx)->cp_x );
	((tvec *)edx)->x1 = fpu_reg10;
	fpu_reg10 = ( ((tvec *)eax)->x2 );
	((tvec *)edx)->x2 = fpu_reg10;
	fpu_reg10 = ( ((tvec *)eax)->x3 );
	fpu_reg10 = fpu_reg10 - ( ((tcpoint *)ebx)->cp_y );
	((tvec *)edx)->x3 = fpu_reg10;

	eax = eax + ( sizeof(tvec) );
	edx = edx + ( sizeof(tvec) );

	if (eax < edi) goto updatechains_l4;
//        mov     rings,edi
	normals = edx;


//make melting segment
	fpu_reg10 = 0.0;
	x = fpu_reg10;

	ecx = ( meltrings + 1 );

updatechains_l5:

	fpu_reg10 = x;
	fpu_reg11 = a_2;
	fun2(fpu_reg10, fpu_reg11);
	ringparams.r_radius = fpu_reg11;
	fpu_reg10 = fpu_reg10 + c_1_mr;
	x = fpu_reg10;

	stack_var00 = esi;
	esi = ( (uint32_t)&(ringparams) );
	placering(esi, edi); //esi->ringparams, edi->ring
	esi = stack_var00;

	fpu_reg10 = ringparams.r_xpos;
	fpu_reg10 = fpu_reg10 + ringparams2.r_xstep;
	ringparams.r_xpos = fpu_reg10;
	fpu_reg10 = ringparams.r_zpos;
	fpu_reg10 = fpu_reg10 + ringparams2.r_zstep;
	ringparams.r_zpos = fpu_reg10;

	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto updatechains_l5;


//make normals of melting segment
	stack_var00 = edi;
	edi = normals;

	fpu_reg10 = 0.0;
	x = fpu_reg10;
	ecx = ( meltrings + 1 );
updatechains_l6:
	fpu_reg10 = x;
	fpu_reg11 = a_2;
	fun3(fpu_reg10, fpu_reg11); //slope of melting area
	fun1(fpu_reg11, fpu_reg12); //calc radius
	ringparams2.r_radius = fpu_reg12;

	fpu_reg12 = ringparams2.r_cos; //calc displacement
	fpu_reg12 = fpu_reg12 * fpu_reg11;
	ringparams2.r_xpos = fpu_reg12;
	fpu_reg11 = fpu_reg11 * ringparams2.r_sin;
	ringparams2.r_zpos = fpu_reg11;

	fpu_reg10 = fpu_reg10 + c_1_mr; //increment x
	x = fpu_reg10;


	stack_var01 = esi;
	esi = ( (uint32_t)&(ringparams2) );
	placering(esi, edi); //esi->ringparams2, edi->normals
	esi = stack_var01;

	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto updatechains_l6;

	normals = edi;
	edi = stack_var00;
	rings = edi;

//make bottom hemisphere
	fpu_reg10 = ringparams.r_xpos; //keep position
	ringparams2.r_xpos = fpu_reg10;
	fpu_reg10 = ringparams.r_zpos;
	ringparams2.r_zpos = fpu_reg10;

	fpu_reg10 = 0.0;
	x = fpu_reg10;

	ecx = ( hsphererings + 1 );
updatechains_l7:
	fpu_reg10 = x;
	fun1(fpu_reg10, fpu_reg11);
	ringparams.r_radius = fpu_reg11;
	fpu_reg10 = fpu_reg10 + c_1_hsr;
	x = fpu_reg10;

	stack_var00 = esi;
	esi = ( (uint32_t)&(ringparams) );
	placering_inc(esi, edi); //esi->ringparams, edi->ring
	esi = stack_var00;

	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto updatechains_l7;

//make normals of bottom hemisphere
	eax = rings;
	edx = normals;
updatechains_l8:
	fpu_reg10 = ( ((tvec *)eax)->x1 );
	fpu_reg10 = fpu_reg10 - ringparams2.r_xpos; //[ebx].cp_x
	((tvec *)edx)->x1 = fpu_reg10;
	fpu_reg10 = ( ((tvec *)eax)->x2 );
	((tvec *)edx)->x2 = fpu_reg10;
	fpu_reg10 = ( ((tvec *)eax)->x3 );
	fpu_reg10 = fpu_reg10 - ringparams2.r_zpos; //[ebx].cp_y
	((tvec *)edx)->x3 = fpu_reg10;

	eax = eax + ( sizeof(tvec) );
	edx = edx + ( sizeof(tvec) );

	if (eax < edi) goto updatechains_l8;


updatechains_w:
	esi = esi + ( sizeof(tchain) );
	z = ( (int32_t)z ) - 1;
	if (( (int32_t)z ) != 0) goto updatechains_l;

	return;
}


static void checkdir(uint32_t _ebx, double &_fpu_reg10, double &_fpu_reg11) {
	double fpu_reg10, fpu_reg11, fpu_reg12;
	uint32_t ebx = _ebx;
//-> ebx = *sp

	fpu_reg10 = ( ((tscreenpoint *)ebx)[2].sp_x ); //lie points clock-wise?
	fpu_reg10 = fpu_reg10 - ( ((tscreenpoint *)ebx)->sp_x );
	fpu_reg11 = ( ((tscreenpoint *)ebx)[1].sp_y );
	fpu_reg11 = fpu_reg11 - ( ((tscreenpoint *)ebx)->sp_y );
	fpu_reg10 = fpu_reg10 * fpu_reg11;
	fpu_reg11 = ( ((tscreenpoint *)ebx)[2].sp_y );
	fpu_reg11 = fpu_reg11 - ( ((tscreenpoint *)ebx)->sp_y );
	fpu_reg12 = ( ((tscreenpoint *)ebx)[1].sp_x );
	fpu_reg12 = fpu_reg12 - ( ((tscreenpoint *)ebx)->sp_x );
	fpu_reg11 = fpu_reg11 * fpu_reg12;
	_fpu_reg10 = fpu_reg10;
	_fpu_reg11 = fpu_reg11;
}



static uint32_t subxclip(uint32_t &_ebx, uint32_t &_edi) { //x-clipping fr scanline subdiv.
	double fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13;
	uint32_t eax, edx, ecx, edi = _edi, ebx = _ebx, esi;
//-> ebx = *sp
//-> edi = *sp_end
//<- ebx = *sp
//<- edi = *sp_end
//<- wenn weniger als 3 punkte, mit jbe wegspringen
	uint32_t _sp, _sp_end; //beides pointer

	fpu_reg10 = 0.0; //st = x
	edx = /*(edx & 0xffffff00) |*/ (uint8_t)(( 1 )); //dl = i, 1 to 0
subxclip_i_l:
	_sp = ebx; //ebx -> sp[z] (quellpunkte) (z = 0)
	_sp_end = edi; //edi -> sp[d] (zielpunkte)  (d = m)

subxclip_z_l:
	fpu_reg11 = ( ((tscreenpoint *)ebx)->sp_x ); //inn = (sp[z].sx >= x);

	eax = /*(eax & 0xffff0000) |*/ ((fpu_reg11 < fpu_reg10)?0x100:0);
	edx = set_high_byte(edx, ( (uint8_t)(eax >> 8) )); //dh = inn
	eax = set_high_byte(eax, ( (uint8_t)(eax >> 8) ) ^ ( (uint8_t)edx )); //inn xor i

	if ((eax & 0x100) == 0) goto subxclip_0;
//punkt innerhalb
	esi = ebx;
	ecx = ( flatpsize / 4 );
	for (; ecx != 0; ecx--, esi+=4, edi+=4) *(uint32_t *)edi = *(uint32_t *)esi; //sp[d] = sp[z] und d++
subxclip_0:
	esi = ebx;
	esi = esi + ( flatpsize ); //nz = z+1
//if (nz >= m) nz = 0;
	if (esi < _sp_end) goto subxclip_wrap;
	esi = _sp;
subxclip_wrap:

	fpu_reg11 = ( ((tscreenpoint *)esi)->sp_x ); //if (inn ^ (sp[nz].sx >= x))

	eax = (eax & 0xffff0000) | ((fpu_reg11 < fpu_reg10)?0x100:0);
	eax = set_high_byte(eax, ( (uint8_t)(eax >> 8) ) ^ ( (uint8_t)(edx >> 8) )); //dh = inn

	if ((eax & 0x100) == 0) goto subxclip_1;
//dieser oder n„chster punkt auáerhalb
	((tscreenpoint *)edi)->sp_x = fpu_reg10;
//sp[d].sy berechnen
	fpu_reg11 = ( ((tscreenpoint *)esi)->sp_y ); //(sp[nz].sy - sp[z].sy)
	fpu_reg11 = fpu_reg11 - ( ((tscreenpoint *)ebx)->sp_y );
	fpu_reg12 = ( ((tscreenpoint *)esi)->sp_x ); // /(sp[nz].sx - sp[z].sx)
	fpu_reg12 = fpu_reg12 - ( ((tscreenpoint *)ebx)->sp_x );
	fpu_reg11 = fpu_reg11 / fpu_reg12;
	fpu_reg12 = fpu_reg10; //x
	fpu_reg12 = fpu_reg12 - ( ((tscreenpoint *)ebx)->sp_x ); //*(x - sp[z].sx)
	fpu_reg11 = fpu_reg11 * fpu_reg12;
	fpu_reg11 = fpu_reg11 + ( ((tscreenpoint *)ebx)->sp_y ); //+sp[z].sy
	((tscreenpoint *)edi)->sp_y = fpu_reg11;

	fpu_reg11 = ( ((tscreenpoint *)ebx)->sp_x ); //ax = sp[z].sx *sp[z].z;
	fpu_reg11 = fpu_reg11 * ( ((tscreenpoint *)ebx)->sp_z );

	fpu_reg12 = ( ((tscreenpoint *)esi)->sp_x ); //ux = sp[nz].sx*sp[nz].z - ax;
	fpu_reg12 = fpu_reg12 * ( ((tscreenpoint *)esi)->sp_z );
	fpu_reg12 = fpu_reg12 - fpu_reg11;

//r = (x * sp[z].z - ax) / (ux - (sp[nz].z - sp[z].z) * x);
	fpu_reg13 = ( ((tscreenpoint *)esi)->sp_z ); //ux - (sp[nz].z - sp[z].z) * x
	fpu_reg13 = fpu_reg13 - ( ((tscreenpoint *)ebx)->sp_z );
	fpu_reg13 = fpu_reg13 * fpu_reg10; //x
	fpu_reg12 = fpu_reg12 - fpu_reg13;
	fpu_reg13 = ( ((tscreenpoint *)ebx)->sp_z ); //(x * sp[z].z - ax)
	fpu_reg13 = fpu_reg13 * fpu_reg10; //x
	fpu_reg11 = fpu_reg13 - fpu_reg11;
	fpu_reg11 = fpu_reg11 / fpu_reg12; //st = r

	fpu_reg12 = ( ((tscreenpoint *)esi)->sp_z ); //sp[d].z
	fpu_reg12 = fpu_reg12 - ( ((tscreenpoint *)ebx)->sp_z );
	fpu_reg12 = fpu_reg12 * fpu_reg11;
	fpu_reg12 = fpu_reg12 + ( ((tscreenpoint *)ebx)->sp_z );
	((tscreenpoint *)edi)->sp_z = fpu_reg12;

	ecx = ( 1 );
subxclip_l: //sp[d].u
	fpu_reg12 = ( ((tscreenpoint *)esi)->sp_data[ecx] );
	fpu_reg12 = fpu_reg12 - ( ((tscreenpoint *)ebx)->sp_data[ecx] );
	fpu_reg12 = fpu_reg12 * fpu_reg11;
	fpu_reg12 = fpu_reg12 + ( ((tscreenpoint *)ebx)->sp_data[ecx] );
	((tscreenpoint *)edi)->sp_data[ecx] = fpu_reg12;
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) >= 0) goto subxclip_l;
//r entfernen

	edi = edi + ( flatpsize ); //d++
subxclip_1:
//wz > z?
	if (esi <= ebx) goto subxclip_z_l_2;
	ebx = esi; //entspricht ebx += spsize
	goto subxclip_z_l; //z-schleife
subxclip_z_l_2:


	ebx = _sp_end; //ebx -> sp
//edi -> sp_end
	eax = edi;
	eax = eax - ebx;
	eax = eax >> ( 1 );

	if (eax <= ( flatpsize )) goto subxclip_w;

	fpu_reg10 = fpu_reg10 + ( (int32_t)xres ); //x += xres
	edx = (edx & 0xffffff00) | (uint8_t)(( (int8_t)edx ) - 1); //i-schleife
	if (( (int8_t)edx ) >= 0) goto subxclip_i_l;

//x entfernen
	eax = 0;
	goto subxclip_weg;
subxclip_w: //x entfernen
	eax = ( 1 );

subxclip_weg:
	_edi = edi;
	_ebx = ebx;
	return eax;
}



static void subpolygon(uint32_t _sp, uint32_t sp_end) { //texture mapping mit scanline subdivision
	double fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13, fpu_reg14, fpu_reg15, fpu_reg16;
	uint32_t eax, edx, ecx, edi, ebx, esi, ebp;
	uint32_t stack_var00, stack_var01;

//_sp = *sp, zeiger auf screenpoints

	uint32_t pend, y, x_y, lb, lc, rb, rc; //int
	float lx, ldx, rx, rdx; //float
	uint32_t xa, xe, txt_x, txt_y; //int
	tscan la, lu, ra, ru, a, u;


	eax = 0; //start- und endpunkt bestimmen
	lb = eax;
	rb = eax;
	pend = eax;

	esi = _sp; //esi -> screenpoints
	fpu_reg10 = ( ((tscreenpoint *)esi)->sp_y ); //st(0) = ymax
	fpu_reg11 = fpu_reg10; //st(1) = ymin

	ecx = sp_end;
	ecx = ecx - ( flatpsize ); //esi+ecx -> sp[sp_end-1]
subpolygon_max_l:

	fpu_reg12 = ( ((tscreenpoint *)(esi + ecx))->sp_y );
//ymax              ;gr”áten y-wert finden (endpunkt)


	if (fpu_reg12 < fpu_reg11) goto subpolygon_max;
	pend = ecx;
	fpu_reg11 = fpu_reg12; //ymax
	goto subpolygon_min;
subpolygon_max:
//ymin              ;kleinsten y-wert finden (startpunkt)


	if (fpu_reg12 > fpu_reg10) goto subpolygon_min;
	lb = ecx;
	rb = ecx;
	fpu_reg10 = fpu_reg12; //ymin
subpolygon_min:

	ecx = ( (int32_t)ecx ) - ( flatpsize );
	if (( (int32_t)ecx ) != 0) goto subpolygon_max_l;
//ymax entfernen

	y =  (int32_t)ceil(fpu_reg10); //y = ceil(ymin)

	eax = y;

//y-clipping
	if (( (int32_t)eax ) >= ( (int32_t)yres )) goto subpolygon_weg;


	if (( (int32_t)eax ) > 0) goto subpolygon_y0;
	eax = 0;
	y = eax;
subpolygon_y0:
	eax = ( (int32_t)eax ) * ( (int32_t)xres );
	edx = erzdata.e_buffer;
//        shr     edx,2
//mov edx,0A0000h
	eax = eax + edx;
	x_y = eax;

	lc = ( 1 );
	rc = ( 1 );

	fpu_reg10 = c_16; //st(0) = 16

subpolygon_y_l: //y-schleife

//links
	lc = ( (int32_t)lc ) - 1;
	if (( (int32_t)lc ) != 0) goto subpolygon_l_nz;
	edi = _sp;
	esi = lb;
subpolygon_lc:

	if (esi == pend) goto subpolygon_fertig; //lb == pend -> unten angekommen
	ebx = esi; //ia = lb
	{ uint32_t carry = (esi < ( flatpsize ))?1:0; esi = esi - ( flatpsize ); //lb--
	  if (carry == 0) goto subpolygon_l0; } //wrap
	esi = esi + sp_end;
subpolygon_l0:
	fpu_reg11 = ( ((tscreenpoint *)(esi + edi))->sp_y ); //lc = ceil(sp[lb].sy) - y
	lc =  (int32_t)ceil(fpu_reg11);
	eax = y;
	lc = ( (int32_t)lc ) - ( (int32_t)eax );
	if (( (int32_t)lc ) <= 0) goto subpolygon_lc; //while lc <= 0
	lb = esi;

	fpu_reg11 = ( ((tscreenpoint *)(esi + edi))->sp_z ); //sp[lb].z einlagern

//la
	fpu_reg12 = ( ((tscreenpoint *)(ebx + edi))->sp_z ); //la.z = sp[ia].z;
	la.s_z = fpu_reg12;

	fpu_reg13 = ( ((tscreenpoint *)(ebx + edi))->sp_y ); //la.y = sp[ia].sy*sp[ia].z;
	fpu_reg13 = fpu_reg13 * fpu_reg12;
	la.s_p = fpu_reg13; //la.y

	fpu_reg14 = ( ((tscreenpoint *)(ebx + edi))->sp_u ); //la.u = sp[ia].u;
	la.s_u = fpu_reg14;

	fpu_reg15 = ( ((tscreenpoint *)(ebx + edi))->sp_v ); //la.v = sp[ia].v;
	la.s_v = fpu_reg15;

//lu
	fpu_reg15 = ( ((tscreenpoint *)(esi + edi))->sp_v ) - fpu_reg15; //lu.v = sp[lb].v - sp[ia].v;
	lu.s_v = fpu_reg15;

	fpu_reg14 = ( ((tscreenpoint *)(esi + edi))->sp_u ) - fpu_reg14; //lu.u = sp[lb].u - sp[ia].u;
	lu.s_u = fpu_reg14;

	fpu_reg14 = ( ((tscreenpoint *)(esi + edi))->sp_y ); //lu.y = sp[lb].sy*sp[lb].z - la.y;
	fpu_reg14 = fpu_reg14 * fpu_reg11; //sp[lb].z
	fpu_reg13 = fpu_reg14 - fpu_reg13;
	lu.s_p = fpu_reg13; //lu.y
	fpu_reg11 = fpu_reg11 - fpu_reg12; //lu.z = sp[lb].z - sp[ia].z;
	lu.s_z = fpu_reg11;


//ldx = (sp[lb].sx - sp[ia].sx)/(sp[lb].sy - sp[ia].sy);
	fpu_reg11 = ( ((tscreenpoint *)(esi + edi))->sp_x );
	fpu_reg11 = fpu_reg11 - ( ((tscreenpoint *)(ebx + edi))->sp_x );
	fpu_reg12 = ( ((tscreenpoint *)(esi + edi))->sp_y );
	fpu_reg12 = fpu_reg12 - ( ((tscreenpoint *)(ebx + edi))->sp_y );
	fpu_reg11 = fpu_reg11 / fpu_reg12;
	ldx = fpu_reg11;

//lx = ldx   *(y  - sp[ia].sy) + sp[ia].sx;
	fpu_reg12 = ( (int32_t)y );
	fpu_reg12 = fpu_reg12 - ( ((tscreenpoint *)(ebx + edi))->sp_y );
	fpu_reg11 = fpu_reg11 * fpu_reg12;
	fpu_reg11 = fpu_reg11 + ( ((tscreenpoint *)(ebx + edi))->sp_x );
	lx = fpu_reg11;
	goto subpolygon_l_z;
subpolygon_l_nz:
	fpu_reg11 = lx;
	fpu_reg11 = fpu_reg11 + ldx; //lx bleibt im copro
	lx = fpu_reg11;
subpolygon_l_z:
//rechts
	rc = ( (int32_t)rc ) - 1;
	if (( (int32_t)rc ) != 0) goto subpolygon_r_nz;
	edi = _sp;
	esi = rb;
subpolygon_rc:


	if (esi == pend) goto subpolygon_fertig2; //rb == pend -> unten angekommen
	ebx = esi; //ia = rb
	esi = esi + ( flatpsize ); //rb++

	if (esi < sp_end) goto subpolygon_r0; //wrap
	esi = 0;
subpolygon_r0:
	fpu_reg12 = ( ((tscreenpoint *)(esi + edi))->sp_y ); //rc = ceil(sp[rb].sy) - y
	rc =  (int32_t)ceil(fpu_reg12);
	eax = y;
	rc = ( (int32_t)rc ) - ( (int32_t)eax );
	if (( (int32_t)rc ) <= 0) goto subpolygon_rc; //while rc <= 0
	rb = esi;

	fpu_reg12 = ( ((tscreenpoint *)(esi + edi))->sp_z ); //sp[lb].z einlagern

//ra
	fpu_reg13 = ( ((tscreenpoint *)(ebx + edi))->sp_z ); //ra.z = sp[ia].z;
	ra.s_z = fpu_reg13;

	fpu_reg14 = ( ((tscreenpoint *)(ebx + edi))->sp_y ); //ra.y = sp[ia].sy*sp[ia].z;
	fpu_reg14 = fpu_reg14 * fpu_reg13;
	ra.s_p = fpu_reg14; //ra.y

	fpu_reg15 = ( ((tscreenpoint *)(ebx + edi))->sp_u ); //ra.u = sp[ia].u;
	ra.s_u = fpu_reg15;

	fpu_reg16 = ( ((tscreenpoint *)(ebx + edi))->sp_v ); //ra.v = sp[ia].v;
	ra.s_v = fpu_reg16;

//ru
	fpu_reg16 = ( ((tscreenpoint *)(esi + edi))->sp_v ) - fpu_reg16; //ru.v = sp[rb].v - sp[ia].v;
	ru.s_v = fpu_reg16;

	fpu_reg15 = ( ((tscreenpoint *)(esi + edi))->sp_u ) - fpu_reg15; //ru.u = sp[rb].u - sp[ia].u;
	ru.s_u = fpu_reg15;

	fpu_reg15 = ( ((tscreenpoint *)(esi + edi))->sp_y ); //ru.y = sp[rb].sy*sp[rb].z - ra.y;
	fpu_reg15 = fpu_reg15 * fpu_reg12; //sp[lb].z
	fpu_reg14 = fpu_reg15 - fpu_reg14;
	ru.s_p = fpu_reg14; //ru.y

	fpu_reg12 = fpu_reg12 - fpu_reg13; //ru.z = sp[rb].z - sp[ia].z;
	ru.s_z = fpu_reg12;


//rdx = (sp[rb].sx - sp[ia].sx)/(sp[rb].sy - sp[ia].sy);
	fpu_reg12 = ( ((tscreenpoint *)(esi + edi))->sp_x );
	fpu_reg12 = fpu_reg12 - ( ((tscreenpoint *)(ebx + edi))->sp_x );
	fpu_reg13 = ( ((tscreenpoint *)(esi + edi))->sp_y );
	fpu_reg13 = fpu_reg13 - ( ((tscreenpoint *)(ebx + edi))->sp_y );
	fpu_reg12 = fpu_reg12 / fpu_reg13;
	rdx = fpu_reg12;

//rx = rdx   *(y  - sp[ia].sy) + sp[ia].sx;
	fpu_reg13 = ( (int32_t)y );
	fpu_reg13 = fpu_reg13 - ( ((tscreenpoint *)(ebx + edi))->sp_y );
	fpu_reg12 = fpu_reg12 * fpu_reg13;
	fpu_reg12 = fpu_reg12 + ( ((tscreenpoint *)(ebx + edi))->sp_x );
	rx = fpu_reg12;
	goto subpolygon_r_z;
subpolygon_r_nz:
	fpu_reg12 = rx;
	fpu_reg12 = fpu_reg12 + rdx; //rx bleibt im copro
	rx = fpu_reg12;
subpolygon_r_z:

//st(0) = rx,  st(1) = lx
//st(2) = 0.5, st(3) = 16

//links: r = (y *la.z - la.y) / (lu.y - lu.z*y );
	fpu_reg13 = ( (int32_t)y );
	fpu_reg13 = fpu_reg13 * la.s_z;
	fpu_reg13 = fpu_reg13 - la.s_p; //-la.y
	fpu_reg14 = ( (int32_t)y );
	fpu_reg14 = fpu_reg14 * lu.s_z;
	fpu_reg14 = lu.s_p - fpu_reg14; //lu.y-
	fpu_reg13 = fpu_reg13 / fpu_reg14; //st = r

	fpu_reg14 = lu.s_z; //a.z = la.z + r*lu.z;
	fpu_reg14 = fpu_reg14 * fpu_reg13; //*r
	fpu_reg14 = fpu_reg14 + la.s_z;
	a.s_z = fpu_reg14;
	fpu_reg14 = fpu_reg14 * fpu_reg11; //*lx            ;a.x = a.z*lx;
	a.s_p = fpu_reg14; //a.x

	fpu_reg14 = lu.s_u; //a.u = la.u + r*lu.u;
	fpu_reg14 = fpu_reg14 * fpu_reg13; //*r
	fpu_reg14 = fpu_reg14 + la.s_u;
	a.s_u = fpu_reg14;
	txt_x =  (int32_t)ceil(fpu_reg14); //textur-startkoordinate

	fpu_reg13 = fpu_reg13 * lu.s_v; //*r               ;a.v = la.v + r*lu.v;
	fpu_reg13 = fpu_reg13 + la.s_v;
	a.s_v = fpu_reg13;
	txt_y =  (int32_t)ceil(fpu_reg13); //textur-startkoordinate

//rechts: r = (y *ra.z - ra.y) / (ru.y - ru.z*y );
	fpu_reg13 = ( (int32_t)y );
	fpu_reg13 = fpu_reg13 * ra.s_z;
	fpu_reg13 = fpu_reg13 - ra.s_p; //ra.y
	fpu_reg14 = ( (int32_t)y );
	fpu_reg14 = fpu_reg14 * ru.s_z;
	fpu_reg14 = ru.s_p - fpu_reg14; //ru.y-
	fpu_reg13 = fpu_reg13 / fpu_reg14; //st = r

	fpu_reg14 = ru.s_z; //u.z = ra.z + r*ru.z;
	fpu_reg14 = fpu_reg14 * fpu_reg13;
	fpu_reg14 = fpu_reg14 + ra.s_z;

	fpu_reg15 = fpu_reg14; //u.x = u.z*rx - a.x;
	fpu_reg15 = fpu_reg15 * fpu_reg12; //*rx
	fpu_reg15 = fpu_reg15 - a.s_p; //-a.x
	u.s_p = fpu_reg15; //u.x

	fpu_reg14 = fpu_reg14 - a.s_z; //u.z -= a.z;
	u.s_z = fpu_reg14;

	fpu_reg14 = ru.s_u; //u.u = ra.u + r*ru.u - a.u;
	fpu_reg14 = fpu_reg14 * fpu_reg13; //*r
	fpu_reg14 = fpu_reg14 + ra.s_u;
	fpu_reg14 = fpu_reg14 - a.s_u;
	u.s_u = fpu_reg14;

	fpu_reg13 = fpu_reg13 * ru.s_v; //*r               ;u.v = ra.v + r*ru.v - a.v;
	fpu_reg13 = fpu_reg13 + ra.s_v;
	fpu_reg13 = fpu_reg13 - a.s_v;
	u.s_v = fpu_reg13;

	xe =  (int32_t)ceil(fpu_reg12); //xe = ceil(rx) (rx entfernen)
	fpu_reg11 = ceil(fpu_reg11);
	xa =  (int32_t)ceil(fpu_reg11); //xa = ceil(lx) (lx nicht entfernen)


	edi = xa;
	ecx = xe;
	ecx = ecx - edi; //ecx = CCCC
	edi = edi + x_y; //edi = PPPP

subpolygon_l:

	if (( (int32_t)ecx ) >= ( 24 )) goto subpolygon_div;

//weniger als 24 pixel

	if (( (int32_t)ecx ) <= 0) goto subpolygon_w;

	ebx = txt_x; //ebx = 00Xx
	edx = (uint32_t)( (uint8_t)(ebx >> 8) ); //edx = 00?X
	ebx = ebx << ( 24 ); //ebx = x000
	ebx = (ebx & 0xffff0000) | (uint16_t)(( (uint16_t)txt_y )); //ebx = x0Yy

	eax = 0;
	stack_var00 = eax; //push pixel counter (= 0)

	esi = erzdata.e_divtab;
	fpu_reg12 = a.s_u;
	fpu_reg12 = fpu_reg12 + u.s_u; //a.u + u.u = texutur - endkoordinate
	fpu_reg12 = fpu_reg12 - ( (int32_t)txt_x );
	fpu_reg12 = fpu_reg12 * ( ((float *)(esi))[ecx] );
	txt_x =  (int32_t)ceil(fpu_reg12);

	fpu_reg12 = a.s_v;
	fpu_reg12 = fpu_reg12 + u.s_v; //a.v + u.v = texutur - endkoordinate
	fpu_reg12 = fpu_reg12 - ( (int32_t)txt_y );
	fpu_reg12 = fpu_reg12 * ( ((float *)(esi))[ecx] );
	txt_y =  (int32_t)ceil(fpu_reg12);

	eax = txt_x; //eax = 00Uu
	ecx = set_high_byte(ecx, ( (uint8_t)(eax >> 8) )); //ecx = 00UC
	eax = eax << ( 24 ); //eax = u000
	eax = (eax & 0xffff0000) | (uint16_t)(( (uint16_t)txt_y )); //eax = u0Vv

	goto subpolygon_5;
subpolygon_div: //scanline subdivision
//st(0) = x
//st(1) = 16
	edx = txt_x; //edx = 00Xx
	ebx = txt_y; //ebx = 00Yy

	ecx = ecx - ( 16 );
	stack_var00 = ecx; //push pixel counter

	fpu_reg11 = fpu_reg11 + fpu_reg10; //16             ;x um 16 weiter
	fpu_reg12 = fpu_reg11; //x                    ;r = (x *a.z - a.x) / (u.x - u.z*x )
	fpu_reg12 = fpu_reg12 * a.s_z;
	fpu_reg12 = fpu_reg12 - a.s_p; //a.x
	fpu_reg13 = fpu_reg11; //x
	fpu_reg13 = fpu_reg13 * u.s_z;
	fpu_reg13 = u.s_p - fpu_reg13; //u.x
	fpu_reg12 = fpu_reg12 / fpu_reg13; //st = r

	fpu_reg13 = fpu_reg12;

	fpu_reg13 = fpu_reg13 * u.s_u;
	fpu_reg13 = fpu_reg13 + a.s_u; //a.u + r*u.u
	txt_x =  (int32_t)ceil(fpu_reg13);

	fpu_reg12 = fpu_reg12 * u.s_v;
	fpu_reg12 = fpu_reg12 + a.s_v; //a.v + r*u.v
	txt_y =  (int32_t)ceil(fpu_reg12);


	eax = txt_x;
	esi = txt_y;
	eax = eax - edx; //eax = 00Uu
	esi = esi - ebx; //esi = 00Vv

	eax = eax >> ( 4 );
	esi = esi >> ( 4 );

	ecx = set_high_byte(ecx, ( (uint8_t)(eax >> 8) )); //ecx = 00U0
	eax = eax << ( 24 ); //eax = u000
	ecx = (ecx & 0xffffff00) | (uint8_t)(( 16 )); //ecx = 00UC
	eax = (eax & 0xffff0000) | (uint16_t)(( (uint16_t)esi )); //eax = u0Vv

	ebx = ebx << ( 8 ); //ebx = 0Yy0
	ebx = (ebx >> ( 8 )) | (edx << (32 - ( 8 ))); //ebx = x0Yy
	edx = (uint32_t)( (uint8_t)(edx >> 8) ); //edx = 00?X
subpolygon_5:
	esi = erzdata.e_pic1; //esi = TTTT
	stack_var01 = ( 0 /*ebp*/ ); //ebp : lokale variablen
	ebp = eax;

	edx = set_high_byte(edx, ( (uint8_t)(ebx >> 8) ));
subpolygon_inner:
	eax = (eax & 0xffffff00) | (uint8_t)(( *((uint8_t *)(esi + edx)) ));
	{ uint32_t carry = (UINT32_MAX - ebx < ebp)?1:0; ebx = ebx + ebp;
	  edx = (edx & 0xffffff00) | (uint8_t)(( (uint8_t)edx ) + ( (uint8_t)(ecx >> 8) ) + carry); }
//if gfx gt 0
//        mov     [edi*4],eax
	*((uint8_t *)(edi)) = (uint8_t) (( (uint8_t)eax ));
//endif
	edx = set_high_byte(edx, ( (uint8_t)(ebx >> 8) ));
	edi = edi + 1;
	ecx = (ecx & 0xffffff00) | (uint8_t)(( (int8_t)ecx ) - 1);
	if (( (int8_t)ecx ) != 0) goto subpolygon_inner;

	ebp = stack_var01;
	ecx = stack_var00;
	goto subpolygon_l;
subpolygon_w:
//x entfernen

	eax = xres;
	x_y = x_y + eax;
	y = y + 1;
	eax = yres;
//while (y < yres)
	if (( (int32_t)eax ) > ( (int32_t)y )) goto subpolygon_y_l;

	goto subpolygon_fertig;
subpolygon_fertig2:
//lx entfernen
subpolygon_fertig:
//16 entfernen

subpolygon_weg:
	return;
}





static void drawback(void) {
	double fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13, fpu_reg14;
	uint32_t eax, ecx, edi, ebx, esi;


	edi = erzdata.e_tempdata;
	esi = ( (uint32_t)&(viewer) );
	ebx = ( (uint32_t)&(backpoints[0]) );

	ecx = ( 4 );
drawback_l0:
	fpu_reg10 = ( ((tscreenpoint *)ebx)->sp_x );
	fpu_reg10 = fpu_reg10 - ( ((tviewer *)esi)->v_p.x1 );
	fpu_reg11 = ( ((tscreenpoint *)ebx)->sp_y );
	fpu_reg11 = fpu_reg11 - ( ((tviewer *)esi)->v_p.x2 );
	fpu_reg12 = ( ((tscreenpoint *)ebx)->sp_z );
	fpu_reg12 = fpu_reg12 - ( ((tviewer *)esi)->v_p.x3 );

	fpu_reg13 = ( ((tviewer *)esi)->v_l3.x1 ); //z
	fpu_reg13 = fpu_reg13 * fpu_reg10;
	fpu_reg14 = ( ((tviewer *)esi)->v_l3.x2 );
	fpu_reg14 = fpu_reg14 * fpu_reg11;
	fpu_reg13 = fpu_reg13 + fpu_reg14;
	fpu_reg14 = ( ((tviewer *)esi)->v_l3.x3 );
	fpu_reg14 = fpu_reg14 * fpu_reg12;
	fpu_reg13 = fpu_reg13 + fpu_reg14;
	((tscreenpoint *)edi)->sp_z = fpu_reg13;

	fpu_reg13 = ( ((tviewer *)esi)->v_l1.x1 ); //x
	fpu_reg13 = fpu_reg13 * fpu_reg10;
	fpu_reg14 = ( ((tviewer *)esi)->v_l1.x2 );
	fpu_reg14 = fpu_reg14 * fpu_reg11;
	fpu_reg13 = fpu_reg13 + fpu_reg14;
	fpu_reg14 = ( ((tviewer *)esi)->v_l1.x3 );
	fpu_reg14 = fpu_reg14 * fpu_reg12;
	fpu_reg13 = fpu_reg13 + fpu_reg14;
	fpu_reg13 = fpu_reg13 * c_xfov;
	fpu_reg13 = fpu_reg13 / ( ((tscreenpoint *)edi)->sp_z ); //x = (1+x/z)*xmid
	fpu_reg14 = 1.0;
	fpu_reg13 = fpu_reg13 + fpu_reg14;
	fpu_reg13 = fpu_reg13 * xmid;
	((tscreenpoint *)edi)->sp_x = fpu_reg13;

	fpu_reg13 = ( ((tviewer *)esi)->v_l2.x1 ); //y
	fpu_reg10 = fpu_reg10 * fpu_reg13;
	fpu_reg13 = ( ((tviewer *)esi)->v_l2.x2 );
	fpu_reg11 = fpu_reg11 * fpu_reg13;
	fpu_reg13 = ( ((tviewer *)esi)->v_l2.x3 );
	fpu_reg12 = fpu_reg12 * fpu_reg13;
	fpu_reg11 = fpu_reg11 + fpu_reg12;
	fpu_reg10 = fpu_reg10 + fpu_reg11;
	fpu_reg10 = fpu_reg10 * c_yfov; //c_1_33
	fpu_reg10 = fpu_reg10 / ( ((tscreenpoint *)edi)->sp_z ); //y = (1-y/z)*ymid
	fpu_reg11 = 1.0;
	fpu_reg10 = fpu_reg11 - fpu_reg10;
	fpu_reg10 = fpu_reg10 * ymid;
	((tscreenpoint *)edi)->sp_y = fpu_reg10;

	fpu_reg10 = ( ((tscreenpoint *)ebx)->sp_u );
	fpu_reg10 = fpu_reg10 * erzdata.e_xres1;
	((tscreenpoint *)edi)->sp_u = fpu_reg10;
	fpu_reg10 = ( ((tscreenpoint *)ebx)->sp_v );
	fpu_reg10 = fpu_reg10 * erzdata.e_yres1;
	((tscreenpoint *)edi)->sp_v = fpu_reg10;

	ebx = ebx + ( flatpsize );
	edi = edi + ( flatpsize );
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto drawback_l0;

	ebx = ( edi + (- 4 * flatpsize) );

	eax = subxclip(ebx, edi);

	if (( (int32_t)eax ) != 0) goto drawback_weg;
	edi = edi - ebx;
	subpolygon(ebx, edi);
drawback_weg:
	return;
}




static uint32_t xclip(uint32_t &_ebx, uint32_t &_edi) { //x-clipping
	double fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13;
	uint32_t eax, edx, ecx, edi = _edi, ebx = _ebx, esi;
//-> ebx = *sp
//-> edi = *sp_end
//<- ebx = *sp
//<- edi = *sp_end
//<- wenn weniger als 3 punkte, mit jbe wegspringen
	uint32_t _sp, _sp_end; //beides pointer

	fpu_reg10 = 0.0; //st = x
	edx = /*(edx & 0xffffff00) |*/ (uint8_t)(( 1 )); //dl = i, 1 to 0
xclip_i_l:
	_sp = ebx; //ebx -> sp[z] (quellpunkte) (z = 0)
	_sp_end = edi; //edi -> sp[d] (zielpunkte)  (d = m)

xclip_z_l:
	fpu_reg11 = ( ((tscreenpoint *)ebx)->sp_x ); //inn = (sp[z].sx >= x);

	eax = /*(eax & 0xffff0000) |*/ ((fpu_reg11 < fpu_reg10)?0x100:0);
	edx = set_high_byte(edx, ( (uint8_t)(eax >> 8) )); //dh = inn
	eax = set_high_byte(eax, ( (uint8_t)(eax >> 8) ) ^ ( (uint8_t)edx )); //inn xor i

	if ((eax & 0x100) == 0) goto xclip_0;
//punkt innerhalb
	esi = ebx;
	ecx = ( flatpsize / 4 );
	for (; ecx != 0; ecx--, esi+=4, edi+=4) *(uint32_t *)edi = *(uint32_t *)esi; //sp[d] = sp[z] und d++
xclip_0:
	esi = ebx;
	esi = esi + ( flatpsize ); //nz = z+1
//if (nz >= m) nz = 0;
	if (esi < _sp_end) goto xclip_wrap;
	esi = _sp;
xclip_wrap:

	fpu_reg11 = ( ((tscreenpoint *)esi)->sp_x ); //if (inn ^ (sp[nz].sx >= x))

	eax = (eax & 0xffff0000) | ((fpu_reg11 < fpu_reg10)?0x100:0);
	eax = set_high_byte(eax, ( (uint8_t)(eax >> 8) ) ^ ( (uint8_t)(edx >> 8) )); //dh = inn

	if ((eax & 0x100) == 0) goto xclip_1;
//dieser oder n„chster punkt auáerhalb

	((tscreenpoint *)edi)->sp_x = fpu_reg10; //sp[d].sx speichern

//r berechnen
	fpu_reg11 = fpu_reg10; //x                    ;(x - sp[z].sx)
	fpu_reg11 = fpu_reg11 - ( ((tscreenpoint *)ebx)->sp_x );
	fpu_reg12 = ( ((tscreenpoint *)esi)->sp_x ); // /(sp[nz].sx - sp[z].sx)
	fpu_reg12 = fpu_reg12 - ( ((tscreenpoint *)ebx)->sp_x );
	fpu_reg11 = fpu_reg11 / fpu_reg12; //st = r

	fpu_reg12 = ( ((tscreenpoint *)ebx)->sp_y ); //sp[d].sy berechnen
	fpu_reg13 = ( ((tscreenpoint *)esi)->sp_y ); // sp[z].sy + r*(sp[nz].sy - sp[z].sy)
	fpu_reg13 = fpu_reg13 - fpu_reg12;
	fpu_reg13 = fpu_reg13 * fpu_reg11;
	fpu_reg12 = fpu_reg12 + fpu_reg13;
	((tscreenpoint *)edi)->sp_y = fpu_reg12;

	ecx = ( 1 );
xclip_vars:
	fpu_reg12 = ( ((tscreenpoint *)esi)->sp_data[ecx] );
	fpu_reg12 = fpu_reg12 - ( ((tscreenpoint *)ebx)->sp_data[ecx] );
	fpu_reg12 = fpu_reg12 * fpu_reg11;
	fpu_reg12 = fpu_reg12 + ( ((tscreenpoint *)ebx)->sp_data[ecx] );
	((tscreenpoint *)edi)->sp_data[ecx] = fpu_reg12;
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) >= 0) goto xclip_vars;

//r entfernen

	edi = edi + ( flatpsize ); //d++
xclip_1:
//wz > z?
	if (esi <= ebx) goto xclip_z_l_2;
	ebx = esi; //entspricht ebx += spsize
	goto xclip_z_l; //z-schleife
xclip_z_l_2:

	ebx = _sp_end; //ebx -> sp
//edi -> sp_end
	eax = edi;
	eax = eax - ebx;

	if (eax <= ( flatpsize * 2 )) goto xclip_w;

	fpu_reg10 = fpu_reg10 + ( (int32_t)xres ); //x += xres
	edx = (edx & 0xffffff00) | (uint8_t)(( (int8_t)edx ) - 1); //i-schleife
	if (( (int8_t)edx ) >= 0) goto xclip_i_l;

//x entfernen
	eax = 0;
	goto xclip_weg;
xclip_w: //x entfernen
	eax = ( 1 );

xclip_weg:
	_edi = edi;
	_ebx = ebx;
	return eax;
}


static void polygon(uint32_t _sp, uint32_t sp_end) {
	double fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13;
	uint32_t eax, edx, ecx, edi, ebx, esi, ebp;
	uint32_t stack_var00;

//_sp = *sp, zeiger auf screenpoints
	uint32_t pend, y, x_y, lb, lc, rb, rc; //int
	float lx, ldx, lu, ldu, lv, ldv; //float
	float rx, rdx, ru, rdu, rv, rdv; //float
	uint32_t xa, xe, txt_x, txt_y; //int
	//int32_t mapand;


	eax = 0; //start- und endpunkt bestimmen
	lb = eax;
	rb = eax;
	pend = eax;

	esi = _sp; //esi -> screenpoints
	fpu_reg10 = ( ((tscreenpoint *)esi)->sp_y ); //st(0) = ymax
	fpu_reg11 = fpu_reg10; //st(1) = ymin

	ecx = sp_end;
	ecx = ecx - ( flatpsize ); //esi+ecx -> sp[sp_end-1]
polygon_max_l:

	fpu_reg12 = ( ((tscreenpoint *)(esi + ecx))->sp_y );
//ymax              ;gr”áten y-wert finden (endpunkt)


	if (fpu_reg12 < fpu_reg11) goto polygon_max;
	pend = ecx;
	fpu_reg11 = fpu_reg12; //ymax
	goto polygon_min;
polygon_max:
//ymin              ;kleinsten y-wert finden (startpunkt)


	if (fpu_reg12 > fpu_reg10) goto polygon_min;
	lb = ecx;
	rb = ecx;
	fpu_reg10 = fpu_reg12; //ymin
polygon_min:

	ecx = ( (int32_t)ecx ) - ( flatpsize );
	if (( (int32_t)ecx ) != 0) goto polygon_max_l;
//ymax entfernen

	y =  (int32_t)ceil(fpu_reg10); //y = ceil(ymin)

	edx = y;
//y-clipping
	if (( (int32_t)edx ) >= ( (int32_t)yres )) goto polygon_weg;


	if (( (int32_t)edx ) > 0) goto polygon_y0;
	edx = 0;
	y = edx;
polygon_y0:
	edx = ( (int32_t)edx ) * ( (int32_t)xres );
	eax = erzdata.e_buffer;
//        shr     eax,2
//mov eax,0A0000h
	edx = edx + eax;
	x_y = edx;

	lc = ( 1 );
	rc = ( 1 );

polygon_y_l: //y-schleife

//links
	lc = ( (int32_t)lc ) - 1;
	if (( (int32_t)lc ) != 0) goto polygon_l_nz;
	edi = _sp;
	esi = lb;
polygon_lc:

	if (esi == pend) goto polygon_fertig; //lb == pend -> unten angekommen
	ebx = esi; //ia = lb
	{ uint32_t carry = (esi < ( flatpsize ))?1:0; esi = esi - ( flatpsize ); //lb--
	  if (carry == 0) goto polygon_l0; } //wrap
	esi = esi + sp_end;
polygon_l0:
	fpu_reg10 = ( ((tscreenpoint *)(esi + edi))->sp_y ); //lc = ceil(sp[lb].sy) - y
	lc =  (int32_t)ceil(fpu_reg10);
	eax = y;
	lc = ( (int32_t)lc ) - ( (int32_t)eax );
	if (( (int32_t)lc ) <= 0) goto polygon_lc; //while lc <= 0
	lb = esi;


	fpu_reg10 = ( (int32_t)y );
	fpu_reg10 = fpu_reg10 - ( ((tscreenpoint *)(ebx + edi))->sp_y ); //(y  - sp[ia].y)

	fpu_reg11 = ( ((tscreenpoint *)(edi + esi))->sp_y ); //(sp[lb].y - sp[ia].y)
	fpu_reg11 = fpu_reg11 - ( ((tscreenpoint *)(edi + ebx))->sp_y );

//ldu = (sp[lb].u - sp[ia].u)/(sp[lb].y - sp[ia].y);
	fpu_reg12 = ( ((tscreenpoint *)(edi + esi))->sp_u );
	fpu_reg12 = fpu_reg12 - ( ((tscreenpoint *)(edi + ebx))->sp_u );
	fpu_reg12 = fpu_reg12 / fpu_reg11;
	ldu = fpu_reg12;

//lu = ldu  *(y  - sp[ia].y) + sp[ia].u;
	fpu_reg12 = fpu_reg12 * fpu_reg10;
	fpu_reg12 = fpu_reg12 + ( ((tscreenpoint *)(edi + ebx))->sp_u );
	txt_x =  (int32_t)ceil(fpu_reg12);
	lu = fpu_reg12;

//ldv = (sp[lb].v - sp[ia].v)/(sp[lb].y - sp[ia].y);
	fpu_reg12 = ( ((tscreenpoint *)(edi + esi))->sp_v );
	fpu_reg12 = fpu_reg12 - ( ((tscreenpoint *)(edi + ebx))->sp_v );
	fpu_reg12 = fpu_reg12 / fpu_reg11;
	ldv = fpu_reg12;

//lv = ldv  *(y  - sp[ia].y) + sp[ia].v;
	fpu_reg12 = fpu_reg12 * fpu_reg10;
	fpu_reg12 = fpu_reg12 + ( ((tscreenpoint *)(edi + ebx))->sp_v );
	txt_y =  (int32_t)ceil(fpu_reg12);
	lv = fpu_reg12;

//ldx = (sp[lb].x - sp[ia].x)/(sp[lb].y - sp[ia].y);
	fpu_reg12 = ( ((tscreenpoint *)(edi + esi))->sp_x );
	fpu_reg12 = fpu_reg12 - ( ((tscreenpoint *)(edi + ebx))->sp_x );
	fpu_reg11 = fpu_reg12 / fpu_reg11;
	ldx = fpu_reg11;

//lx = ldx  *(y  - sp[ia].y) + sp[ia].x;
	fpu_reg10 = fpu_reg10 * fpu_reg11;
	fpu_reg10 = fpu_reg10 + ( ((tscreenpoint *)(edi + ebx))->sp_x );
	lx = fpu_reg10;

	goto polygon_l_z;
polygon_l_nz:
	fpu_reg10 = lu;
	fpu_reg10 = fpu_reg10 + ldu;
	txt_x =  (int32_t)ceil(fpu_reg10);
	lu = fpu_reg10;

	fpu_reg10 = lv;
	fpu_reg10 = fpu_reg10 + ldv;
	txt_y =  (int32_t)ceil(fpu_reg10);
	lv = fpu_reg10;

	fpu_reg10 = lx;
	fpu_reg10 = fpu_reg10 + ldx; //lx bleibt im copro
	lx = fpu_reg10;
polygon_l_z:
//rechts
	rc = ( (int32_t)rc ) - 1;
	if (( (int32_t)rc ) != 0) goto polygon_r_nz;
	edi = _sp;
	esi = rb;
polygon_rc:

	if (esi == pend) goto polygon_fertig2; //rb == pend -> unten angekommen
	ebx = esi; //ia = rb
	esi = esi + ( flatpsize ); //rb++

	if (esi < sp_end) goto polygon_r0; //wrap
	esi = 0;
polygon_r0:
	fpu_reg11 = ( ((tscreenpoint *)(esi + edi))->sp_y ); //rc = ceil(sp[rb].sy) - y
	rc =  (int32_t)ceil(fpu_reg11);
	eax = y;
	rc = ( (int32_t)rc ) - ( (int32_t)eax );
	if (( (int32_t)rc ) <= 0) goto polygon_rc; //while lc <= 0
	rb = esi;


	fpu_reg11 = ( (int32_t)y );
	fpu_reg11 = fpu_reg11 - ( ((tscreenpoint *)(ebx + edi))->sp_y ); //(y  - sp[ia].y)

	fpu_reg12 = ( ((tscreenpoint *)(edi + esi))->sp_y ); //(sp[rb].y - sp[ia].y)
	fpu_reg12 = fpu_reg12 - ( ((tscreenpoint *)(edi + ebx))->sp_y );

//rdu = (sp[rb].u - sp[ia].u)/(sp[rb].y - sp[ia].y);
	fpu_reg13 = ( ((tscreenpoint *)(edi + esi))->sp_u );
	fpu_reg13 = fpu_reg13 - ( ((tscreenpoint *)(edi + ebx))->sp_u );
	fpu_reg13 = fpu_reg13 / fpu_reg12;
	rdu = fpu_reg13;

//ru = rdu  *(y  - sp[ia].y) + sp[ia].u;
	fpu_reg13 = fpu_reg13 * fpu_reg11;
	fpu_reg13 = fpu_reg13 + ( ((tscreenpoint *)(edi + ebx))->sp_u );
	ru = fpu_reg13;

//rdv = (sp[rb].v - sp[ia].v)/(sp[rb].y - sp[ia].y);
	fpu_reg13 = ( ((tscreenpoint *)(edi + esi))->sp_v );
	fpu_reg13 = fpu_reg13 - ( ((tscreenpoint *)(edi + ebx))->sp_v );
	fpu_reg13 = fpu_reg13 / fpu_reg12;
	rdv = fpu_reg13;

//rv = rdv  *(y  - sp[ia].y) + sp[ia].v;
	fpu_reg13 = fpu_reg13 * fpu_reg11;
	fpu_reg13 = fpu_reg13 + ( ((tscreenpoint *)(edi + ebx))->sp_v );
	rv = fpu_reg13;

//rdx = (sp[rb].x - sp[ia].x)/(sp[rb].y - sp[ia].y);
	fpu_reg13 = ( ((tscreenpoint *)(edi + esi))->sp_x );
	fpu_reg13 = fpu_reg13 - ( ((tscreenpoint *)(edi + ebx))->sp_x );
	fpu_reg12 = fpu_reg13 / fpu_reg12;
	rdx = fpu_reg12;

//rx = rdx  *(y  - sp[ia].y) + sp[ia].x;
	fpu_reg11 = fpu_reg11 * fpu_reg12;
	fpu_reg11 = fpu_reg11 + ( ((tscreenpoint *)(edi + ebx))->sp_x );
	rx = fpu_reg11;

	goto polygon_r_z;
polygon_r_nz:
	fpu_reg11 = ru;
	fpu_reg11 = fpu_reg11 + rdu;
	ru = fpu_reg11;

	fpu_reg11 = rv;
	fpu_reg11 = fpu_reg11 + rdv;
	rv = fpu_reg11;

	fpu_reg11 = rx;
	fpu_reg11 = fpu_reg11 + rdx; //rx bleibt im copro
	rx = fpu_reg11;
polygon_r_z:

	xe =  (int32_t)ceil(fpu_reg11); //xe = ceil(rx) (rx entfernen)
	xa =  (int32_t)ceil(fpu_reg10); //xa = ceil(lx) (lx entfernen)

//eax = pppp
//ebx = x0Yy
//ecx = CCCU
//edx = 00?X
//esi = TTTT
//edi = PPPP
//ebp = u0Vv

	edi = xa;
	ecx = xe;
	ecx = ( (int32_t)ecx ) - ( (int32_t)edi ); //ecx = CCCC
	if (( (int32_t)ecx ) <= 0) goto polygon_w;
	edi = edi + x_y; //edi = PPPP

	ebx = txt_x; //ebx = 00Xx
	edx = (uint32_t)( (uint8_t)(ebx >> 8) ); //edx = 00?X
	ebx = ebx << ( 24 ); //ebx = x000
	ebx = (ebx & 0xffff0000) | (uint16_t)(( (uint16_t)txt_y )); //ebx = x0Yy

	esi = erzdata.e_divtab;
	fpu_reg10 = ru;
	fpu_reg10 = fpu_reg10 - lu;
	fpu_reg10 = fpu_reg10 * ( ((float *)(esi))[ecx] );
	txt_x =  (int32_t)ceil(fpu_reg10);
	fpu_reg10 = rv;
	fpu_reg10 = fpu_reg10 - lv;
	fpu_reg10 = fpu_reg10 * ( ((float *)(esi))[ecx] );
	txt_y =  (int32_t)ceil(fpu_reg10);

	ecx = ecx - 1;
	eax = txt_x; //eax = 00Uu
	ecx = ecx << ( 8 ); //ecx = CCC0
	ecx = (ecx & 0xffffff00) | (uint8_t)(( (uint8_t)(eax >> 8) )); //ecx = CCCU
	eax = eax << ( 24 ); //eax = u000
	eax = (eax & 0xffff0000) | (uint16_t)(( (uint16_t)txt_y )); //eax = u0Vv
	esi = erzdata.e_pic2; //esi = TTTT

	stack_var00 = ( 0 /*ebp*/ ); //ebp : lokale variablen
	ebp = eax;

	edx = set_high_byte(edx, ( (uint8_t)(ebx >> 8) ));
polygon_inner:
	eax = (eax & 0xffffff00) | (uint8_t)(( *((uint8_t *)(esi + edx)) ));
	{ uint32_t carry = (UINT32_MAX - ebx < ebp)?1:0; ebx = ebx + ebp;
	  edx = (edx & 0xffffff00) | (uint8_t)(( (uint8_t)edx ) + ( (uint8_t)ecx ) + carry); }
//if gfx gt 0
	*((uint8_t *)(edi)) = (uint8_t) (( (uint8_t)eax ));
//mov     [edi*4],eax
//endif
	edx = set_high_byte(edx, ( (uint8_t)(ebx >> 8) ));
	edi = edi + 1;
	{ uint32_t carry = (ecx < ( 256 ))?1:0; ecx = ecx - ( 256 );
	  if (carry == 0) goto polygon_inner; }

	ebp = stack_var00;

polygon_w:
	eax = xres;
	x_y = x_y + eax;
	y = y + 1;
	eax = yres;
//while (y < yres)
	if (( (int32_t)eax ) > ( (int32_t)y )) goto polygon_y_l;

	goto polygon_fertig;
polygon_fertig2:
//lx entfernen
polygon_fertig:
polygon_weg:
	return;
}


static void xformchain(uint32_t rings, uint32_t _ebx, uint32_t _edx, uint32_t _edi) {
	double fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13, fpu_reg14;
	uint32_t eax, edx = _edx, ecx, edi = _edi, ebx = _ebx, esi;
	uint32_t stack_var00, stack_var01, stack_var02;

//-> ebx -> ringlist
//-> edx -> normalslist
//-> edi -> tempdata
	float z;
	uint32_t z1, xy, uv, temp;


	xy = edi;
	esi = ( (uint32_t)&(viewer) );

	ecx = rings;
	ecx = ( (int32_t)ecx ) * ( ringpoints );
xformchain_l0:
	fpu_reg10 = ( ((tvec *)ebx)->x1 );
	fpu_reg10 = fpu_reg10 - ( ((tviewer *)esi)->v_p.x1 );
	fpu_reg11 = ( ((tvec *)ebx)->x2 );
	fpu_reg11 = fpu_reg11 - ( ((tviewer *)esi)->v_p.x2 );
	fpu_reg12 = ( ((tvec *)ebx)->x3 );
	fpu_reg12 = fpu_reg12 - ( ((tviewer *)esi)->v_p.x3 );

	fpu_reg13 = ( ((tviewer *)esi)->v_l3.x1 ); //z
	fpu_reg13 = fpu_reg13 * fpu_reg10;
	fpu_reg14 = ( ((tviewer *)esi)->v_l3.x2 );
	fpu_reg14 = fpu_reg14 * fpu_reg11;
	fpu_reg13 = fpu_reg13 + fpu_reg14;
	fpu_reg14 = ( ((tviewer *)esi)->v_l3.x3 );
	fpu_reg14 = fpu_reg14 * fpu_reg12;
	fpu_reg13 = fpu_reg13 + fpu_reg14;
	z = fpu_reg13;

	fpu_reg13 = ( ((tviewer *)esi)->v_l1.x1 ); //x
	fpu_reg13 = fpu_reg13 * fpu_reg10;
	fpu_reg14 = ( ((tviewer *)esi)->v_l1.x2 );
	fpu_reg14 = fpu_reg14 * fpu_reg11;
	fpu_reg13 = fpu_reg13 + fpu_reg14;
	fpu_reg14 = ( ((tviewer *)esi)->v_l1.x3 );
	fpu_reg14 = fpu_reg14 * fpu_reg12;
	fpu_reg13 = fpu_reg13 + fpu_reg14;
	fpu_reg13 = fpu_reg13 * c_xfov;
	fpu_reg13 = fpu_reg13 / z; //x = (1+x/z)*xmid
	fpu_reg14 = 1.0;
	fpu_reg13 = fpu_reg13 + fpu_reg14;
	fpu_reg13 = fpu_reg13 * xmid;
	((tpoint *)edi)->p_x = fpu_reg13;

	fpu_reg13 = ( ((tviewer *)esi)->v_l2.x1 ); //y
	fpu_reg10 = fpu_reg10 * fpu_reg13;
	fpu_reg13 = ( ((tviewer *)esi)->v_l2.x2 );
	fpu_reg11 = fpu_reg11 * fpu_reg13;
	fpu_reg13 = ( ((tviewer *)esi)->v_l2.x3 );
	fpu_reg12 = fpu_reg12 * fpu_reg13;
	fpu_reg11 = fpu_reg11 + fpu_reg12;
	fpu_reg10 = fpu_reg10 + fpu_reg11;
	fpu_reg10 = fpu_reg10 * c_yfov; //c_1_33
	fpu_reg10 = fpu_reg10 / z; //y = (1-y/z)*ymid
	fpu_reg11 = 1.0;
	fpu_reg10 = fpu_reg11 - fpu_reg10;
	fpu_reg10 = fpu_reg10 * ymid;
	((tpoint *)edi)->p_y = fpu_reg10;


	ebx = ebx + ( sizeof(tvec) );
	edi = edi + ( sizeof(tpoint) );
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto xformchain_l0;

	uv = edi;

	ecx = rings;
	ecx = ( (int32_t)ecx ) * ( ringpoints );
xformchain_l1:
	fpu_reg10 = ( ((tvec *)edx)->x1 );
	fpu_reg11 = ( ((tvec *)edx)->x2 );
	fpu_reg12 = ( ((tvec *)edx)->x3 );

	fpu_reg13 = ( ((tviewer *)esi)->v_l1.x1 ); //x
	fpu_reg13 = fpu_reg13 * fpu_reg10;
	fpu_reg14 = ( ((tviewer *)esi)->v_l1.x2 );
	fpu_reg14 = fpu_reg14 * fpu_reg11;
	fpu_reg13 = fpu_reg13 + fpu_reg14;
	fpu_reg14 = ( ((tviewer *)esi)->v_l1.x3 );
	fpu_reg14 = fpu_reg14 * fpu_reg12;
	fpu_reg13 = fpu_reg13 + fpu_reg14;
	fpu_reg14 = 1.0;
	fpu_reg13 = fpu_reg13 + fpu_reg14;
	fpu_reg13 = fpu_reg13 * erzdata.e_xres2; //nscale
	((tpoint *)edi)->p_x = fpu_reg13;

	fpu_reg13 = ( ((tviewer *)esi)->v_l2.x1 ); //y
	fpu_reg10 = fpu_reg10 * fpu_reg13;
	fpu_reg13 = ( ((tviewer *)esi)->v_l2.x2 );
	fpu_reg11 = fpu_reg11 * fpu_reg13;
	fpu_reg13 = ( ((tviewer *)esi)->v_l2.x3 );
	fpu_reg12 = fpu_reg12 * fpu_reg13;
	fpu_reg11 = fpu_reg11 + fpu_reg12;
	fpu_reg10 = fpu_reg10 + fpu_reg11;
	fpu_reg11 = 1.0;
	fpu_reg10 = fpu_reg10 + fpu_reg11;
	fpu_reg10 = fpu_reg10 * erzdata.e_yres2; //nscale
	((tpoint *)edi)->p_y = fpu_reg10;


	edx = edx + ( sizeof(tvec) );
	edi = edi + ( sizeof(tpoint) );
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto xformchain_l1;


	temp = edi;

	esi = xy;
	ebx = uv;

	ecx = rings;
	ecx = ecx - 1;
	z1 = ecx;
xformchain_l2:
	ecx = ( ringpoints - 1 );
xformchain_l3:
	edi = temp;
	((tscreenpoint *)edi)->sp_x = ((tpoint *)esi)->p_x;
	((tscreenpoint *)edi)->sp_y = ((tpoint *)esi)->p_y;
	((tscreenpoint *)edi)->sp_u = ((tpoint *)ebx)->p_x;
	((tscreenpoint *)edi)->sp_v = ((tpoint *)ebx)->p_y;
	edi = edi + ( sizeof(tscreenpoint) );

	((tscreenpoint *)edi)->sp_x = ((tpoint *)esi)[1].p_x;
	((tscreenpoint *)edi)->sp_y = ((tpoint *)esi)[1].p_y;
	((tscreenpoint *)edi)->sp_u = ((tpoint *)ebx)[1].p_x;
	((tscreenpoint *)edi)->sp_v = ((tpoint *)ebx)[1].p_y;
	edi = edi + ( sizeof(tscreenpoint) );

	((tscreenpoint *)edi)->sp_x = ((tpoint *)esi)[( ringpoints + 1 )].p_x;
	((tscreenpoint *)edi)->sp_y = ((tpoint *)esi)[( ringpoints + 1 )].p_y;
	((tscreenpoint *)edi)->sp_u = ((tpoint *)ebx)[( ringpoints + 1 )].p_x;
	((tscreenpoint *)edi)->sp_v = ((tpoint *)ebx)[( ringpoints + 1 )].p_y;
	edi = edi + ( sizeof(tscreenpoint) );

	((tscreenpoint *)edi)->sp_x = ((tpoint *)esi)[ringpoints].p_x;
	((tscreenpoint *)edi)->sp_y = ((tpoint *)esi)[ringpoints].p_y;
	((tscreenpoint *)edi)->sp_u = ((tpoint *)ebx)[ringpoints].p_x;
	((tscreenpoint *)edi)->sp_v = ((tpoint *)ebx)[ringpoints].p_y;
	edi = edi + ( sizeof(tscreenpoint) );


	stack_var00 = ecx;
	stack_var01 = esi;
	stack_var02 = ebx;
	ebx = temp;

	checkdir(ebx, fpu_reg10, fpu_reg11);



	if (fpu_reg11 <= fpu_reg10) goto xformchain_w0;
	eax = xclip(ebx, edi);

	if (( (int32_t)eax ) != 0) goto xformchain_w0;
	edi = edi - ebx;
	polygon(ebx, edi);
xformchain_w0:

	ebx = stack_var02;
	esi = stack_var01;
	ecx = stack_var00;
	esi = esi + ( sizeof(tpoint) );
	ebx = ebx + ( sizeof(tpoint) );
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto xformchain_l3;

	edi = temp;
	((tscreenpoint *)edi)->sp_x = ((tpoint *)esi)->p_x;
	((tscreenpoint *)edi)->sp_y = ((tpoint *)esi)->p_y;
	((tscreenpoint *)edi)->sp_u = ((tpoint *)ebx)->p_x;
	((tscreenpoint *)edi)->sp_v = ((tpoint *)ebx)->p_y;
	edi = edi + ( sizeof(tscreenpoint) );

	((tscreenpoint *)edi)->sp_x = ((tpoint *)esi)[( 1 - ringpoints )].p_x;
	((tscreenpoint *)edi)->sp_y = ((tpoint *)esi)[( 1 - ringpoints )].p_y;
	((tscreenpoint *)edi)->sp_u = ((tpoint *)ebx)[( 1 - ringpoints )].p_x;
	((tscreenpoint *)edi)->sp_v = ((tpoint *)ebx)[( 1 - ringpoints )].p_y;
	edi = edi + ( sizeof(tscreenpoint) );

	((tscreenpoint *)edi)->sp_x = ((tpoint *)esi)[1].p_x;
	((tscreenpoint *)edi)->sp_y = ((tpoint *)esi)[1].p_y;
	((tscreenpoint *)edi)->sp_u = ((tpoint *)ebx)[1].p_x;
	((tscreenpoint *)edi)->sp_v = ((tpoint *)ebx)[1].p_y;
	edi = edi + ( sizeof(tscreenpoint) );

	((tscreenpoint *)edi)->sp_x = ((tpoint *)esi)[ringpoints].p_x;
	((tscreenpoint *)edi)->sp_y = ((tpoint *)esi)[ringpoints].p_y;
	((tscreenpoint *)edi)->sp_u = ((tpoint *)ebx)[ringpoints].p_x;
	((tscreenpoint *)edi)->sp_v = ((tpoint *)ebx)[ringpoints].p_y;
	edi = edi + ( sizeof(tscreenpoint) );


	stack_var00 = esi;
	stack_var01 = ebx;

	ebx = temp;

	checkdir(ebx, fpu_reg10, fpu_reg11);



	if (fpu_reg11 <= fpu_reg10) goto xformchain_w1;
	eax = xclip(ebx, edi);

	if (( (int32_t)eax ) != 0) goto xformchain_w1;
	edi = edi - ebx;
	polygon(ebx, edi);
xformchain_w1:

	ebx = stack_var01;
	esi = stack_var00;
	esi = esi + ( sizeof(tpoint) );
	ebx = ebx + ( sizeof(tpoint) );


	z1 = ( (int32_t)z1 ) - 1;
	if (( (int32_t)z1 ) != 0) goto xformchain_l2;

//xformchain_weg:
	return;
}


static void drawchains(void) {
	double fpu_reg10, fpu_reg11, fpu_reg12;
	uint32_t eax, edx, ecx, edi, ebx, esi;
	uint32_t stack_var00, stack_var01, stack_var02;
	uint32_t z;

	esi = erzdata.e_chain; //esi -> tchain
	ecx = erzdata.e_chains;
	z = ecx;
drawchains_l:

	eax = ( ((tchain *)esi)->c_lastpoint );

	if (( (int32_t)eax ) == 0) goto drawchains_drops;

	eax = eax + ( hsphererings * 2 + meltrings ); //eax = rings
	ebx = ( ((tchain *)esi)->c_ringlist );
	edx = ( ((tchain *)esi)->c_normalslist );
	edi = erzdata.e_tempdata;
	stack_var00 = esi;
	xformchain(eax, ebx, edx, edi);
	esi = stack_var00;
drawchains_drops:
	eax = ( ((tchain *)esi)->c_lastpoint );
	ecx = ( ((tchain *)esi)->c_points ); //esi -> tchain
	ecx = ( (int32_t)ecx ) - ( (int32_t)eax ); //ecx = tropfen
	if (( (int32_t)ecx ) == 0) goto drawchains_w;
	ebx = ( (int32_t)eax ) * ( sizeof(tcpoint) );
	ebx = ebx + ( ((tchain *)esi)->c_pointlist );
	stack_var00 = esi;

//no chain left?
	if (( (int32_t)eax ) != 0) goto drawchains_t_l;
//last drop as sphere
	fpu_reg10 = ( ((tcpoint *)ebx)->cp_y );



	if (fpu_reg10 < c_ymin) goto drawchains_w0;

	fpu_reg10 = ( ((tcpoint *)ebx)->cp_dx );
	fpu_reg10 = fpu_reg10 * t;
	fpu_reg10 = fpu_reg10 + ( ((tcpoint *)ebx)->cp_x );
	fpu_reg11 = ( ((tcpoint *)ebx)->cp_dy );
	fpu_reg11 = fpu_reg11 * t;
	fpu_reg11 = fpu_reg11 + ( ((tcpoint *)ebx)->cp_y );

	stack_var01 = ebx;
	stack_var02 = ecx;

	edi = erzdata.e_tempdata;
	ebx = edi;
	esi = erzdata.e_sphere;

	ecx = ( ( hsphererings * 2 + 1 ) * ringpoints );
drawchains_l0:
	fpu_reg12 = ( ((tvec *)esi)->x1 );
	fpu_reg12 = fpu_reg12 + fpu_reg10;
	((tvec *)edi)->x1 = fpu_reg12;
	fpu_reg12 = ( ((tvec *)esi)->x2 );
	((tvec *)edi)->x2 = fpu_reg12;
	fpu_reg12 = ( ((tvec *)esi)->x3 );
	fpu_reg12 = fpu_reg12 + fpu_reg11;
	((tvec *)edi)->x3 = fpu_reg12;

	esi = esi + ( sizeof(tvec) );
	edi = edi + ( sizeof(tvec) );
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto drawchains_l0;




//ebx -> ringlist, edi -> tempdata
	edx = erzdata.e_sphere;
	xformchain(( hsphererings * 2 + 1 ), ebx, edx, edi);

	ecx = stack_var02;
	ebx = stack_var01;

	ebx = ebx + ( sizeof(tcpoint) );
	ecx = ecx - 1;

drawchains_t_l:

	fpu_reg10 = ( ((tcpoint *)ebx)->cp_y );



	if (fpu_reg10 < c_ymin) goto drawchains_w0;

	fpu_reg10 = ( ((tcpoint *)ebx)->cp_dx );
	fpu_reg10 = fpu_reg10 * t;
	fpu_reg10 = fpu_reg10 + ( ((tcpoint *)ebx)->cp_x );
	fpu_reg11 = ( ((tcpoint *)ebx)->cp_dy );
	fpu_reg11 = fpu_reg11 * t;
	fpu_reg11 = fpu_reg11 + ( ((tcpoint *)ebx)->cp_y );

	stack_var01 = ebx;
	stack_var02 = ecx;

	edi = erzdata.e_tempdata;
	ebx = edi;
	esi = erzdata.e_ringlist;

	ecx = ( ( meltrings / 2 + 1 + hsphererings ) * ringpoints );
drawchains_l1:
	fpu_reg12 = ( ((tvec *)esi)->x1 );
	fpu_reg12 = fpu_reg12 + fpu_reg10;
	((tvec *)edi)->x1 = fpu_reg12;
	fpu_reg12 = ( ((tvec *)esi)->x2 );
	((tvec *)edi)->x2 = fpu_reg12;
	fpu_reg12 = ( ((tvec *)esi)->x3 );
	fpu_reg12 = fpu_reg12 + fpu_reg11;
	((tvec *)edi)->x3 = fpu_reg12;

	esi = esi + ( sizeof(tvec) );
	edi = edi + ( sizeof(tvec) );
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto drawchains_l1;




//ebx -> ringlist, edi -> tempdata
	edx = erzdata.e_normalslist;
	xformchain(( meltrings / 2 + 1 + hsphererings ), ebx, edx, edi);

	ecx = stack_var02;
	ebx = stack_var01;

	ebx = ebx + ( sizeof(tcpoint) );
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto drawchains_t_l;
drawchains_w0:
	esi = stack_var00; //esi -> tchain
drawchains_w:
	esi = esi + ( sizeof(tchain) );
	z = ( (int32_t)z ) - 1;
	if (( (int32_t)z ) != 0) goto drawchains_l;

//drawchains_weg:
	return;
}


static void clearbuffer(void) {
	uint32_t eax, ecx, edi;

	ecx = xres;
	ecx = ( (int32_t)ecx ) * ( (int32_t)yres );
	ecx = ecx >> ( 2 );
	eax = 0;
	edi = erzdata.e_buffer;
	for (; ecx != 0; ecx--, edi+=4) *(uint32_t *)edi = eax;

	return;
}



static void copybuffer8(void) {
	uint32_t ecx, edi, esi, ebp;
	uint32_t stack_var00;

	esi = erzdata.e_buffer;
	edi = linbuf;
	ebp = yres;
copybuffer8_y_l:
	stack_var00 = edi;

	ecx = xres;
	ecx = ecx >> ( 2 );
	for (; ecx != 0; ecx--, esi+=4, edi+=4) *(uint32_t *)edi = *(uint32_t *)esi;

	edi = stack_var00;
	edi = edi + xbytes;

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto copybuffer8_y_l;

	return;
}


static void copybuffer16(void) {
	uint32_t eax, edx, ecx, edi, ebx, esi, ebp;
	uint32_t stack_var00;

	esi = erzdata.e_buffer;
	edi = linbuf;
	ebx = erzdata.e_paltab;
	ebp = yres;
	eax = 0;
copybuffer16_y_l:
	stack_var00 = edi;

	ecx = xres;
	ecx = ecx >> ( 1 );
copybuffer16_x_l:
	eax = (eax & 0xffffff00) | (uint8_t)(( ((uint8_t *)(esi))[1] ));

	edx = /*(edx & 0xffff0000) |*/ (uint16_t)(( ((uint16_t *)(ebx))[eax] ));

	eax = (eax & 0xffffff00) | (uint8_t)(( *((uint8_t *)(esi)) ));
	edx = edx << ( 16 );
	esi = esi + ( 2 );
	edx = (edx & 0xffff0000) | (uint16_t)(( ((uint16_t *)(ebx))[eax] ));

	*((uint32_t *)(edi)) = edx;
	edi = edi + ( 4 );

	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto copybuffer16_x_l;

	edi = stack_var00;
	edi = edi + xbytes;

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto copybuffer16_y_l;

	return;
}


static void copybuffer24(void) {
	uint32_t eax, edx, ecx, edi, ebx, esi, ebp;
	uint32_t stack_var00, stack_var01;

	esi = erzdata.e_buffer;
	edi = linbuf;
	ebx = erzdata.e_paltab;
	ebp = yres;
	eax = 0;
copybuffer24_y_l:
	stack_var00 = edi;
	stack_var01 = ebp;

	ecx = xres;
	ecx = ecx >> ( 2 );
copybuffer24_x_l:
//1.
	eax = (eax & 0xffffff00) | (uint8_t)(( *((uint8_t *)(esi)) ));
	esi = esi + 1;
	ebp = ( ((uint32_t *)(ebx))[eax] ); //030201h;

//2.
	eax = (eax & 0xffffff00) | (uint8_t)(( *((uint8_t *)(esi)) ));
	ebp = ebp << ( 8 );
	esi = esi + 1;
	edx = ( ((uint32_t *)(ebx))[eax] ); //060504h;

//3.
	eax = (eax & 0xffffff00) | (uint8_t)(( *((uint8_t *)(esi)) ));
	ebp = (ebp >> ( 8 )) | (edx << (32 - ( 8 )));
	esi = esi + 1;
	*((uint32_t *)(edi)) = ebp;
	edi = edi + ( 4 );
	ebp = ( ((uint32_t *)(ebx))[eax] ); //090807h;
	edx = edx << ( 8 );

//4.
	eax = (eax & 0xffffff00) | (uint8_t)(( *((uint8_t *)(esi)) ));
	edx = (edx >> ( 16 )) | (ebp << (32 - ( 16 )));
	esi = esi + 1;
	*((uint32_t *)(edi)) = edx;
	edi = edi + ( 4 );
	ebp = ebp << ( 8 );
	edx = ( ((uint32_t *)(ebx))[eax] ); //0c0b0ah;

	edx = (edx << ( 8 )) | (ebp >> (32 - ( 8 )));
	*((uint32_t *)(edi)) = edx;
	edi = edi + ( 4 );

	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto copybuffer24_x_l;

	ebp = stack_var01;
	edi = stack_var00;
	edi = edi + xbytes;

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto copybuffer24_y_l;

	return;
}


static void copybuffer32(void) {
	uint32_t eax, edx, ecx, edi, ebx, esi, ebp;
	uint32_t stack_var00;

	esi = erzdata.e_buffer;
	edi = linbuf;
	ebx = erzdata.e_paltab;
	ebp = yres;
	eax = 0;
copybuffer32_y_l:
	stack_var00 = edi;

	ecx = xres;
copybuffer32_x_l:
	eax = (eax & 0xffffff00) | (uint8_t)(( *((uint8_t *)(esi)) ));
	esi = esi + 1;
	edx = ( ((uint32_t *)(ebx))[eax] );

	*((uint32_t *)(edi)) = edx;
	edi = edi + ( 4 );

	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto copybuffer32_x_l;

	edi = stack_var00;
	edi = edi + xbytes;

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto copybuffer32_y_l;

	return;
}


#if 0
//test_chain proc near
//        ;ebx -> ringlist
//        ;edx = rings
//
//        imul    edx,ringpoints
//@@l0:
//
//   fld [ebx].x1
//   fmul c_t
//   fistp tx
//   fld [ebx].x3
//   fmul c_t
//   fistp ty
//   push ebx edx
//   mov eax,tx
//   add eax,20
//   mov edx,ty
//   neg edx
//   add edx,160
//   mov bl,15
//;if gfx gt 0
//   call putpixel
//;endif
//   pop edx ebx
//
//        add     ebx,size tvec
//        dec     edx
//        jnz     @@l0
//
//@@weg:  ret
//test_chain endp
//
//test_it proc near
//
//        mov     esi,erzdata.e_chain
//        mov     ecx,erzdata.e_chains
//@@l:
//
//        mov     edx,[esi].c_lastpoint
//        or      edx,edx
//        jz      @@w
//        add     edx,2*hsphererings+meltrings
//        mov     ebx,[esi].c_ringlist
//        call    test_chain
//@@w:
//        add     esi,size tchain
//        dec     ecx
//        jnz     @@l
//
//@@weg:  ret
//test_it endp
#endif

static void init1(void) {
	double fpu_reg10, fpu_reg11;
	uint32_t ecx, edi;

//divtab
	edi = erzdata.e_divtab;
	ecx = ( 1 );
	fpu_reg10 = 0.0;
	*((float *)(edi)) = fpu_reg10;
init1_l:
	fpu_reg11 = 1.0;
	fpu_reg10 = fpu_reg10 + fpu_reg11;
	fpu_reg11 = 1.0;
	fpu_reg11 = fpu_reg11 / fpu_reg10;
	((float *)(edi))[ecx] = fpu_reg11;
	ecx = ecx + 1;

	if (ecx <= xres) goto init1_l;
//edi -> divtab

//xmid & ymid
	fpu_reg10 = ( (int32_t)xres );
	fpu_reg10 = fpu_reg10 * c_0_5;
	xmid = fpu_reg10;
	fpu_reg10 = ( (int32_t)yres );
	fpu_reg10 = fpu_reg10 * c_0_5;
	ymid = fpu_reg10;

	return;
}


static void initpaltab(uint32_t _esi) {
	uint32_t eax, edx, ecx, edi, ebx, esi = _esi;
//-> esi -> tvesa
	uint32_t col;
	//int32_t z;


	if (( ((tvesa *)esi)->vesa_pbytes ) > ( 1 )) goto initpaltab_dcol;

	esi = erzdata.e_pal;
// todo:         call    setpal
	goto initpaltab_weg;
initpaltab_dcol:

	edx = ( 255 );
	ebx = ( 255 * 3 );
	ebx = ebx + erzdata.e_pal;
	edi = erzdata.e_paltab;
initpaltab_pl:
	eax = (uint32_t)( *((uint8_t *)(ebx)) ); //red
	ecx = /*(ecx & 0xffffff00) |*/ (uint8_t)(( ((tvesa *)esi)->vesa_redbits ));
	eax = eax << ( (uint8_t)ecx );
	eax = eax >> ( 8 );
	ecx = (ecx & 0xffffff00) | (uint8_t)(( ((tvesa *)esi)->vesa_redpos ));
	eax = eax << ( (uint8_t)ecx );
	col = eax;

	eax = (uint32_t)( ((uint8_t *)(ebx))[1] ); //green
	ecx = (ecx & 0xffffff00) | (uint8_t)(( ((tvesa *)esi)->vesa_greenbits ));
	eax = eax << ( (uint8_t)ecx );
	eax = eax >> ( 8 );
	ecx = (ecx & 0xffffff00) | (uint8_t)(( ((tvesa *)esi)->vesa_greenpos ));
	eax = eax << ( (uint8_t)ecx );
	col = col | eax;

	eax = (uint32_t)( ((uint8_t *)(ebx))[2] ); //blue
	ecx = (ecx & 0xffffff00) | (uint8_t)(( ((tvesa *)esi)->vesa_bluebits ));
	eax = eax << ( (uint8_t)ecx );
	eax = eax >> ( 8 );
	ecx = (ecx & 0xffffff00) | (uint8_t)(( ((tvesa *)esi)->vesa_bluepos ));
	eax = eax << ( (uint8_t)ecx );
	eax = eax | col;


	if (( ((tvesa *)esi)->vesa_pbytes ) > ( 2 )) goto initpaltab_32;
	((uint16_t *)(edi))[edx] = (uint16_t) (( (uint16_t)eax ));
	goto initpaltab_0;
initpaltab_32:
	((uint32_t *)(edi))[edx] = eax;
initpaltab_0:
	ebx = ebx - ( 3 );
	edx = ( (int32_t)edx ) - 1;
	if (( (int32_t)edx ) >= 0) goto initpaltab_pl;

initpaltab_weg:
	return;
}




extern "C" void initedata(uint32_t _esi) {
	uint32_t eax, esi = _esi;
//esi -> tvesa

	eax = ( ((tvesa *)esi)->vesa_xbytes );
	xbytes = eax;
	eax = ( ((tvesa *)esi)->vesa_xres );
	xres = eax;
	eax = ( ((tvesa *)esi)->vesa_yres );
	yres = eax;
	eax = ( ((tvesa *)esi)->vesa_pbytes );
	pbytes = eax; //2: 16 bit, 3: 24 bit, 4: 32 bit
	eax = ( ((tvesa *)esi)->vesa_linbuf );
	linbuf = eax;
	screen = (SDL_Surface *) ( ((tvesa *)esi)->vesa_screen );

	initpaltab(esi);

	init1();
	makering();
	calcsphere();
	calcdrop();
	initchains();

	return;
}


static void upscroll(void) {
	uint32_t eax, edx, ecx, edi, ebx, esi, ebp;
	uint32_t stack_var00;
	uint32_t oldlines;
	uint32_t dolines;
	//int32_t texty;
	uint32_t xstep, ystep, ypos;

	oldlines = ( 0 );

	eax = ( 320 * 256 );
	edx = 0;
	{ uint64_t d1 = (uint64_t)eax; uint32_t d2 = xres; eax = d1 / d2; edx = d1 % d2; }
	xstep = eax;

	eax = ( 240 * 256 );
	edx = 0;
	{ uint64_t d1 = (uint64_t)eax; uint32_t d2 = yres; eax = d1 / d2; edx = d1 % d2; }
	ystep = eax;
	ypos = ( 0 );

	eax = maincount;
	mainstart = eax;

	esi = erzdata.e_buffer; //esi -> buffer
upscroll_scroll:
	eax = maincount;
	eax = eax - mainstart;
	frame = eax;
//    add maincount,80
//        sub      eax,oldframe

//eax = frame
	eax = ( (int32_t)eax ) * ( (int32_t)yres );
//     xor edx,edx
//     mov ebx,10000
//     div ebx
	eax = eax >> ( 14 );

	if (( (int32_t)eax ) >= ( (int32_t)erzdata.e_lines )) goto upscroll_weg;

	edx = oldlines;
	oldlines = eax;
	eax = eax - edx; //eax = dolines


	if (eax < yres) goto upscroll_c0;
	eax = yres;
upscroll_c0:
	dolines = eax;

	if (( (int32_t)eax ) <= 0) goto upscroll_lw;

	eax = ( (int32_t)eax ) * ( (int32_t)xres );

	edi = erzdata.e_buffer;
	esi = edi;
	esi = esi + eax;

	ecx = xres;
	ecx = ( (int32_t)ecx ) * ( (int32_t)yres );
	ecx = ecx - eax;
	ecx = ecx >> ( 2 );

	for (; ecx != 0; ecx--, esi+=4, edi+=4) *(uint32_t *)edi = *(uint32_t *)esi;
upscroll_y_l:
	esi = ypos;
	esi = esi >> ( 8 );
	esi = ( (int32_t)esi ) * ( 320 );
	esi = esi + erzdata.e_scroller;



	edx = 0; //eax = 000x
	eax = (eax & 0xffffff00) | (uint8_t)(0); //ebx = UUUU
	ebx = xstep; //ecx = 000u
	ecx = (ecx & 0xffffff00) | (uint8_t)(( (uint8_t)ebx )); //edx = XXXX
	ebx = ebx >> ( 8 );

	stack_var00 = ( 0 /*ebp*/ ); //no locals!
	ebp = xres; //ebp = CCCC
upscroll_x_l:
	eax = set_high_byte(eax, ( *((uint8_t *)(esi + edx)) ));
	{ uint32_t carry = (UINT8_MAX - ( (uint8_t)eax ) < ( (uint8_t)ecx ))?1:0; eax = (eax & 0xffffff00) | (uint8_t)(( (uint8_t)eax ) + ( (uint8_t)ecx ));
	  edx = edx + ebx + carry; }

	*((uint8_t *)(edi)) = (uint8_t) (( (uint8_t)(eax >> 8) ));
	edi = edi + 1;

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto upscroll_x_l;
	ebp = stack_var00;

//update y
	eax = ystep;
	ypos = ypos + eax;

	dolines = ( (int32_t)dolines ) - 1;
	if (( (int32_t)dolines ) != 0) goto upscroll_y_l;

	stack_var00 = ( 0 /*ebp*/ );

	SDL_LockSurface(screen);
	linbuf = (uint32_t)screen->pixels;

	if (pbytes > ( 3 )) goto upscroll_32;
	if (pbytes == ( 3 )) goto upscroll_24;

	if (pbytes == ( 2 )) goto upscroll_16;
	copybuffer8();
	goto upscroll_1;
upscroll_16:
	copybuffer16();
	goto upscroll_1;
upscroll_24:
	copybuffer24();
	goto upscroll_1;
upscroll_32:
	copybuffer32();
upscroll_1:
	SDL_UnlockSurface(screen);
	SDL_Flip(screen);

	ebp = stack_var00;
upscroll_lw:

	eax = frame;
//        mov     oldframe,eax

	if (( (int32_t)eax ) > ( scrollen )) goto upscroll_weg;

//        mov     ah,1
//        int     16h
	eax = keypressed();

	if (( (int32_t)eax ) == 0) goto upscroll_scroll;

upscroll_weg:
	return;
}




extern "C" void starterz(void) {
	double fpu_reg10;
	uint32_t eax, edi/*, ebp*/;
	uint32_t /*stack_var00,*/ stack_var01;
	//stack_var00 = ( 0 /*ebp*/ );

#if 0
//if gfx gt 0
// call init256
// mov esi,erzdata.e_pal
// call setpal
//endif
#endif
//     jmp @@up
	eax = maincount;
	mainstart = eax;

starterz_nextframe:
	clearbuffer();

	eax = maincount;
	eax = eax - mainstart;

	frame = eax;
	eax = eax - stepframe;
starterz_l0:

	if (( (int32_t)eax ) < ( step )) goto starterz_0;
	stack_var01 = eax;
	updatepoints();
	eax = stack_var01;
	stepframe = stepframe + ( step );
	eax = eax - ( step );
	goto starterz_l0;
starterz_0:
//calc t
	eax = frame;
	eax = eax - stepframe;
	fpu_reg10 = ( (int32_t)eax );
	fpu_reg10 = fpu_reg10 * c_tscale;
	t = fpu_reg10;

	updatechains();

//tracks
	edi = ( (uint32_t)&(viewer.v_p) );
	dotrack(erzdata.e_camtrack, ( 3 ), edi);
	edi = ( (uint32_t)&(target) );
	dotrack(erzdata.e_targettrack, ( 3 ), edi);

	doviewer();
	drawback();
	drawchains();

//        mov     ebx,erzdata.e_ringlist
//        mov     edx,erzdata.e_normalslist
//        call    xformchain pascal, meltrings/2+1+hsphererings

//        call    test_it
//        mov     ebx,erzdata.e_ringlist
//        mov     edx,meltrings/2+1+hsphererings
//        call    test_chain


	SDL_LockSurface(screen);
	linbuf = (uint32_t)screen->pixels;

	if (pbytes > ( 3 )) goto starterz_32;
	if (pbytes == ( 3 )) goto starterz_24;

	if (pbytes == ( 2 )) goto starterz_16;
	copybuffer8();
	goto starterz_1;
starterz_16:
	copybuffer16();
	goto starterz_1;
starterz_24:
	copybuffer24();
	goto starterz_1;
starterz_32:
	copybuffer32();
starterz_1:
	SDL_UnlockSurface(screen);
	SDL_Flip(screen);

#if 0
//if gfx gt 0
// call readchar
// cmp eax,27
// je @@w
//; call cls
//endif
//    add maincount,step;/8
//    jmp @@nextframe
#endif

	if (( (int32_t)frame ) > ( erzlen )) goto starterz_up;

//        mov     ah,1
//        int     16h
	eax = keypressed();

	if (( (int32_t)eax ) == 0) goto starterz_nextframe;
	goto starterz_w;
starterz_up:
	upscroll();

starterz_w:

#if 0
//if gfx gt 0
// call textmode
//endif
#endif

//starterz_weg:
	//ebp = stack_var00;
	return;
}




//testcode
#if 0
//
//init256 proc near
//        mov     ax,13h
//        int     10h
//        ret
//endp
//
//textmode proc near
//        mov     ax,3
//        int     10h
//
//;        mov     ax,1112h
//;        int     10h
//
//        ret
//endp
//
//
//setpal proc near                        ;set vga palette
//        ;-> esi -> pal
//
//        xor     eax,eax
//        mov     dx,3c8h
//        out     dx,al
//        inc     dx
//
//        mov     ecx,768
//@@0:
//        mov     al,[esi]
//        shr     al,2;1
//        cmp     al,63
//        jbe     @@1
//        mov     al,63
//@@1:
//        out     dx,al
//        mov     eax,100
//@@w:    dec     eax
//        jnz     @@w
//
//        inc     esi
//        dec     ecx
//        jnz     @@0
//
//        ret
//setpal endp
//
//putpixel proc near
//        ;eax = x
//        ;edx = y
//        ;bl  = f
//        cmp     eax,320
//        jae     @@weg
//        cmp     edx,200
//        jae     @@weg
//        imul    edx,320
//        add     eax,edx
//        add     eax,0a0000h
//        mov     [eax],bl
//
//@@weg:
//        ret
//endp
//
//cls proc near
//        mov     edi,0a0000h
//        xor     eax,eax
//        mov     ecx,64000/4
//        rep     stosd
//@@weg:
//        ret
//endp
//
//
//readchar proc near
//
//        xor     eax,eax
//        int     16h
//        or      al,al
//        jnz     @@0
//        sub     al,ah
//        or      eax,not 0FFh
//        jmp     @@weg
//@@0:    and     eax,0FFh
//
//@@weg:
//        ret
//endp
#endif



