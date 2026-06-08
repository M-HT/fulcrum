/* encoding = IBM852 */
#include <SDL2/SDL.h>
#include <string.h>
#include "cc.h"
//.486                                    //pmode/w
//.model flat,prolog

//locals

//testcode = 0

#include "tracks.inc.h"
#include "int.inc.h"
//extrn int_8t:near
extern "C" int keypressed(void);

//public scene

//public initmdata, startmese
//public m_dotracks, c_dotracks, l_dotracks, t_dotracks, b_dotracks

extern "C" uint32_t docredits(int8_t *mpic, uint32_t _eax);

//traceobject proc near                  ;motion control
//dotracks proc near
//m_dotracks proc near
//c_dotracks proc near
//l_dotracks proc near
//t_dotracks proc near
//b_dotracks proc near
// ;dotrack proc pascal
// ;slerp proc near
// ;dorottrack proc pascal
// ;doltrack proc pascal
// ;dohidetrack proc near
//doswitchtrack proc near

//doviewer proc near
//cubetree proc pascal


//texturedraw proc pascal                ;textures

//drawcube proc pascal                   ;texture cubes
//cubezclip proc pascal

//getnextmesh proc near                  ;texture objects
//xformhierarchy proc pascal
//drawhierarchy proc pascal
//drawface proc pascal
//subdrawface proc pascal
//drawwater proc pascal
//projection proc pascal

//xclip proc pascal                      ;x-clipers
//subxclip proc pascal

//polygon proc pascal                    ;texture drawers
//;gpolygon proc pascal
//subpolygon proc pascal
//;subgpolygon proc pascal
//wpolygon proc pascal

//makelgouraud proc near                 ;light
//dolights proc pascal
//ldrawface proc pascal
//sdrawfaces proc near

//ldraw proc pascal
//gldraw proc near

//lightclip proc pascal
//lscalexy proc near

//lconvert proc near                     ;light points -> flat points
//lpolygon proc pascal
//glpolygon proc pascal
//sublpolygon proc pascal

//scubezclip proc pascal                 ;shadow
//sprojection proc pascal
// ;sxclip proc near
//spolygon proc pascal

//insert proc near                       ;insert idx into z-sorted idx-list
//zprojectline proc pascal               ;zclip and project face for rendering onto z-line
//projectline proc pascal                ;project face for rendering onto z-line
//renderline proc pascal


//cproject proc near
//rtrace proc pascal
//vollight proc pascal
//yclip proc pascal                      ;y-clipping
//vvolpolygon proc pascal
//hvolpolygon proc pascal

//±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
//constants
//±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
#define _NULL 0
#define maxint 0x7FFFFFFF

//w equ word ptr
//d equ dword ptr

//light table
#define lvalues          64
//C
#define ld_lvalues       6
#define xtralvalues      (256-lvalues)
//lvalues*2 ;C

//fog table
#define fvalues          128
//C
#define ld_fvalues       7
#define xtrafvalues      128
//C

//±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
//struc
//±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±

#if defined(_MSC_VER)
#define __attribute__(x)
#endif

//vector
typedef struct {
 union {
  float xn[3];
  struct {
   float x1;
   float x2;
   float x3;
  };
 };
} tvec;

//matrix
typedef struct {
 union {
  float A_mn[9];
  struct {
   union {
    float A_1n[3];
    struct {
     float A_m1[1];
     float A_m2[1];
     float A_m3[1];
    };
    struct {
     float A_11;
     float A_12;
     float A_13;
    };
   };
   union {
    float A_2n[3];
    struct {
     float A_21;
     float A_22;
     float A_23;
    };
   };
   union {
    float A_3n[3];
    struct {
     float A_31;
     float A_32;
     float A_33;
    };
   };
  };
 };
} tmatrix;

//quaternion
#if 0
//tquaternion struc  ;fixed size of 16 byte
// q_w            dd ?
// q_x            dd ?
// q_y            dd ?
// q_z            dd ?
//ends
#endif

//vesa-info
#include "vesa.inc.h"

typedef struct {
 uint32_t p_pal[(lvalues+xtralvalues)*256];
} tpaltab;

typedef struct {
 tpaltab p16_pal;
 uint16_t p16_r[(fvalues+xtrafvalues)*256];
 uint16_t p16_g[(fvalues+xtrafvalues)*256];
 uint16_t p16_b[(fvalues+xtrafvalues)*256];
} tpaltab16;

typedef struct {
 tpaltab p32_pal;
 uint32_t p32_r[(fvalues+xtrafvalues)*256];
 uint32_t p32_g[(fvalues+xtrafvalues)*256];
 uint32_t p32_b[(fvalues+xtrafvalues)*256];
} tpaltab32;

struct tmaterial;
struct tobject;
struct tcube;
struct tcamera;

//scene
typedef struct {
 void *sc_pal;
 void *sc_paltab;
 int8_t *sc_bitmapdata;
 tmaterial *sc_materials;
 void *sc_ambient;                    //ambient track
 tobject *sc_root;
 tcube *sc_cubedata;
 tcamera *sc_cam;
 int8_t *sc_firstlight;
 int8_t *sc_lastlight;

 int8_t *sc_tempdata;                   //2*(tempcubes,vertices,tempfaces)
 void *sc_datapos;
 void *sc_ltab;
 void *sc_ftab;
 void *sc_buffer;
 float *sc_divtab;
 void *sc_lmap;

 void *sc_spot1;
 void *sc_spot2;
 void *sc_water;

 void *sc_idxlist;
 void *sc_idxlistend;
 void *sc_zline;
 void *sc_fogmap;
 void *sc_fogwater;

 void *sc_flares;
 void *sc_flaretab;

 uint32_t sc_intro;
} tscene;

typedef struct tmaterial {
 int8_t *mt_mapptr;
 uint32_t mt_mapand;
} tmaterial;

//VMT
typedef struct {
 int32_t o_size;
 void *o_readdata;
 int8_t *o_firstobj;
 int8_t *o_nextobj;
 void  (*o_dotracks)(tobject *esi);
} tobjvmt;



//object:
//-------
#define ofMatrix    1
#define ofMesh      2
#define ofAbsolute  4
#define ofCamera    65536

typedef struct tobject {
 tobjvmt *o_vmt;

 uint32_t o_flags;

 //hierarchy-system: (pointer)
 tobject *o_owner;
 tobject *o_next;
 tobject *o_child;

 //descent-like cube system: (pointer)
 tobject *o_nextincube;              //link to next object in cube
 tcube *o_cube;                    //link to cube

 //state: (vektor/matrix)
 union {
  float o_p_data[3];
  tvec  o_p;                         //actual position (position and matrix have fixed order in memory)
 };
 union {
  float   o_A_data[9];
  tmatrix o_A;                       //actual rotation and scale
 };
 uint32_t o_hidden;

 uint32_t o_stamp;                   //stamp to mark object as "used"
} tobject;


//face
//----
typedef struct {                        //face point
 uint32_t fp_vertex;                    //number reference
 float  fp_u;
 float  fp_v;
} tfp;

typedef struct {
 tfp f_p[3];
 tmaterial *f_mat;                        //pointer to material list
 uint32_t f_idx;
} tface;

struct tmesh;

typedef struct ttempface {
 tface *tf_faceptr;                   //-> tface
// tf_tlist       dd ?
 tmesh *tf_meshptr;
 ttempface *tf_next;
} ttempface;

//mesh:
//-----
#define mfMorph      8
#define mfSort       16
#define mfNocast     32
#define mfIllum      64
#define mfCorrected  128
#define mfWater      1024
#define mfFlare      2048

typedef struct tmesh {
 tobject m_object;

 uint32_t m_faces;
 tface *m_facelist;
 uint32_t m_vertices;
 float *m_vertexlist;

 //postrack (3):
 void *m_postrack;                   //pointer

 //rotationtrack (4):
 void *m_rottrack;                   //pointer

 //scaletrack (3):
// m_scaletrack   dd ?                    ;pointer

 //morphtrack (2*vertices*3):
 void *m_morphtrack;                 //pointer
 void *m_nmorphtrack;

 //hidetrack
 int32_t *m_hidetrack;               //pointer, int key
 uint32_t m_hidekey;


 float    m_z;                          //z value for pivot z sorting
 tmesh *m_nexttemp;                   //-> tmesh (tempcube mesh list)
 float *m_tlist;                      //-> list of transformed vertices

 float *m_vtlist;
 uint32_t m_vstamp;
} tmesh;


//viewer:
//-------
typedef struct {
 tobject v_object;

 void *v_target;                     //pointer

 //postrack (3):
 void *v_postrack;                   //pointer

 //FOVtrack (1):
 void *v_FOVtrack;                   //pointer
 float    v_FOV;                        //float

 //rolltrack (1):
 void *v_rolltrack;                  //pointer
 float    v_roll;                       //float
} toviewer;

//camera:
//-------
typedef struct tcamera {
 toviewer c_viewer;

 //switchtrack
 int32_t *c_switchtrack;             //pointer, int key
 uint32_t c_switchkey;             //pointer, int key

} tcamera;


//light:
//------
#define lfVol        128
#define lfWater      1024
#define lfFlare      2048

typedef struct {
 toviewer l_viewer;

 //hidetrack
 int32_t *l_hidetrack;               //pointer, int key
 int32_t l_hidekey;

 void *l_lmap;                       //0:round spot, 1:four spots, 2:water
} tlight;


//target:
//-------
typedef struct {
 tobject t_object;

 //postrack (3):
 void *t_postrack;                   //pointer
// t_poskey       dd ?                    ;int
} ttarget;


//bound:
//------
typedef struct {
 tobject b_object;

 //postrack (3):
 void *b_postrack;                   //pointer
// b_poskey       dd ?                    ;int
} btarget;


//track:
//------
typedef struct {
 void*		t_data;			//track-data
 uint32_t	t_key;			//actual key
} ttrack;

#if 0
//ttrackh struc
// th_mode        dd ?                    ;1: loop-track
// th_keys        dd ?                    ;number of gaps to interpolate (keys-1)
// th_sframe      dd ?                    ;start-frame
//ends
//
//tkey struc
// k_sframe       dd ?                    ;start-frame of gap
// ;k_easefrom     dd ?
// ;k_easeto       dd ?
// k_eframe       dd ?                    ;end-frame of gap
//ends
//
//
//;linear track:
//;-------------
//tltrack struc
// lt_startstate  dd ?
// lt_startframe  dd ?
// lt_endstate    dd ?
// lt_endframe    dd ?
// lt_keys        dd ?
// lt_data        dd ?
//ends
//
#endif

//cube
//----
#define t_sub 4
//cube-recursion level where to switch off texture-subdivision
#define l_sub 3
//cube-recursion level where to switch off light-subdivision

typedef struct {
 tvec     pl_p;
 tvec     pl_n;
 tcube *pl_sidecube;
} tplane;

typedef struct {
 uint32_t cp_p;                         //(int) -> vertex (* size tprojected)
 union {
  float  cp_data[2];
  struct {
   float  cp_u;
   float  cp_v;
  };
 };
// cp_l           dd ?                    ;light value
} tcpoint;

typedef struct {
 uint32_t cf_points;                    //number of points in face (*size tcpoint)
 tmaterial *cf_mat;                       //pointer to material list
 tvec   cf_p;
 tvec   cf_n;
} tcface;

struct ttempcube;

typedef struct tcube {
 uint32_t c_planes;
 tplane *c_planedata;
 uint32_t c_faces;
 void *c_facedata;
 float *c_vertex[8];
 tobject *c_objlist;

 uint32_t c_stamp;
 void *c_tc;
 uint32_t c_vstamp;
 ttempcube *c_vtc;
} tcube;

#pragma pack(1)

typedef struct __attribute__ ((__packed__)) {
 float   pr_x;
 float   pr_y;
 float   pr_z;
 float   pr_sx;
 float   pr_sy;
 uint8_t pr_inn;
} tprojected;

typedef struct __attribute__ ((__packed__)) ttempcube {
 tprojected tc_proj[8];                 //transformed vertices
 tcube *tc_cubeptr;                 //pointer to tcube
 tmesh *tc_meshlist;                //mesh-temp-list (pointer to tmesh)
 uint32_t   tc_recursion;
 uint32_t   tc_idx;
} ttempcube;

#pragma pack()





//viewer
//------
typedef struct {
 union {
  float v_p_data[3];
  tvec  v_p;                              //local coordinate system
 };
 union {
  tmatrix v_l;
  struct {
   union {
    float v_l1_data[3];
    tvec  v_l1;
   };
   union {
    float v_l2_data[3];
    tvec  v_l2;
   };
   union {
    float v_l3_data[3];
    tvec  v_l3;
   };
  };
 };
 tcube *v_cube;                       //pointer to cube where viewer is in

 uint32_t v_light;                      //= 0 for camera, = 1 for light
 ttempcube *v_cubelist;                   //pointer to temp cube list
 float    v_zmin;
 ttempface *v_tempfaces[16];              //pointer array to ttempface
 float    v_zmax;
 float    v_1_tan_fov;
} tviewer;

typedef struct {
 tviewer lv_viewer;
 union {
  float lv_l1_data[3];
  tvec  lv_l1;
 };
 union {
  float lv_l2_data[3];
  tvec  lv_l2;
 };
 uint32_t lv_flags;
} tlviewer;


//polygon point
typedef struct {
 float sp_x;
 float sp_y;
 float sp_z;
 union {
  float sp_data[4];
  struct { //mapping & gouraud
   float sp_u;
   float sp_v;
   float sp_l;
  };
  struct { //light
   float sp_lx;
   float sp_ly;
   float sp_lz;
   float sp_ll;
  };
 };
} tscreenpoint;

#define flatpsize (5*4)
#define flatvars (2-1)

#define gouraudpsize (6*4)
#define gouraudvars (3-1)

#define lightpsize (6*4)
#define lightvars (3-1)

#define glightpsize (7*4)
#define glightvars (4-1)

#define shadowpsize (3*4)

#define zlinepsize (3*4)
#define volpsize (4*4)
#define volvars (1-1)

//polygon point for scanline subdivision
typedef struct { //fixed size
 float s_p;
 float s_z;
 float s_u;
 float s_v;
} tscan;

//light polygon point for scanline subdivision
typedef struct { //fixed size
 float ls_p;
 float ls_z;
 float ls_lx;
 float ls_ly;
 float ls_lz;
} tlscan;

typedef struct {
 float    i_zval;
 uint32_t i_idx;
} tidxentry;

typedef struct {
 float w_x[256];
 float w_y[256];
 float w_xpy[512];
 float w_xmy[512];
 float w_scale[256];
} twater;

typedef struct {
 float f_sx;
 float f_sy;
// f_lx            dd ?
// f_ly            dd ?
 float f_mx;
 float f_my;
} tflarepoint;

//±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
//.data
//±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
static uint32_t debug;

//maincount dd 0
static uint32_t mainstart;

//creditnum dd 0
//creditstart dd 0

extern "C" {
tscene scene;
}
//frame           dd 0                    ;global frame counter
static uint32_t stamp = 0;

//ambient
static float ambient_f[3];
static uint32_t ambient = 0;
static uint32_t fog = 0;

//dotracks
const static float c_3 = 3.0f;
//c_sinmin        dd 1E-6
#define framemul 45

//viewer
static tviewer viewer;                  //camera
static tlviewer light;                  //light
const static float c_pi_2 = 1.570796f;
const static float c_1_33 = 1.333333f;  //x:y ratio 4:3 for camera

//xformhierarchy
const static float c_min = -1E10;
const static float c_max = 1E10;
const static float c_0_01 = 0.01f;
//c_15_9          dd 15.9

//drawhierarchy
const static float c_0_5 = 0.5f;
const static float c_0_03125 = 0.03125f;

//drawface
static uint32_t drawedflag;

//vesa
static uint32_t xbytes;
static uint32_t xres;
static uint32_t yres;
static uint8_t *linbuf;
static SDL_Renderer *renderer;
static SDL_Texture *texture;
static void (*copybuffer)(void);

static float xmid; //float
static float ymid; //float

//light & shadow
const static float c_lscale = 32740.0f; //128.0 * 256 (8 bit fraction)
#define sres 256
const static float sfres = 256.0f;
const static float smid = 128.0f;
const static float c_lgouraud = 11264.0f; //44.0 * 256 (8 bit fraction)

const static float c_lmap = 0.012f;     //0.0122718463 = pi/256
const static float c_lmap1 = 57.6f;     //lvalues *9/10

const static float pi_256 = 0.01227f; //pi/256.0
const static float pi_128 = 0.02454f; //pi/128.0
const static float pi_8 = 0.39270f;   //pi/8
const static float c_wscale = 9.2f;   //lvalues fvalues
const static float water_coeff[8] =
{ 0.075f, 0.125f, 0.05f, 0.1f, 0.05f, 0.075f, 0.055f, 0.1125f };

//z-line (volume light)
#define zxshift  5
//32
#define zxres    (1 << zxshift)
#define zyres    3
const static float z_a = 0.2;
const static float z_xmul = 16.0;//32.0    ;zxres/2
const static float z_ymul = 5.0f;     //(zyres-1)/(2*z_a)
const static float z_width = 0.04f;//6    ;breite der strahlen
const static float z_csize = 4.0f;     //konstante breite des entstehungspunktes

static float zxw;       //xmid*z_width
static float zyw;       //ymid*z_width

#define fogmapsize 16
#define ld_fogmapsize 4
#define fogmapsteps 32
const static float c_umax = 3830.0f;     //(fogmapsize-1)*256
const static float c_fogmap1 = 0.20944f;  //pi/(fogmapsize-1)
const static float c_fogmul = 0.6f;//0.42       ;fvalues / (fogmapsteps*9.6)
const static float c_fogadd = 0.08f;

//fogwater
const static float pi_32 = 0.0981748f;
const static float c_fwsin1 = 0.6f;//0.3
const static float c_fwsin2 = 1.0f;//0.5
const static float c_fwadd = 1.0f;
const static float c_fwmul = 10.3f;//15.0      ;(fogmapsteps-1)/3

const static float divtab32[49] = {
     0.0f,
     32.000000f,
     16.000000f,
     10.666667f,
     8.000000f,
     6.400000f,
     5.333333f,
     4.571429f,
     4.000000f,
     3.555556f,
     3.200000f,
     2.909091f,
     2.666667f,
     2.461538f,
     2.285714f,
     2.133333f,
     2.000000f,
     1.882353f,
     1.777778f,
     1.684211f,
     1.600000f,
     1.523810f,
     1.454545f,
     1.391304f,
     1.333333f,
     1.280000f,
     1.230769f,
     1.185185f,
     1.142857f,
     1.103448f,
     1.066667f,
     1.032258f,
     1.000000f,
     0.969697f,
     0.941176f,
     0.914286f,
     0.888889f,
     0.864865f,
     0.842105f,
     0.820513f,
     0.800000f,
     0.780488f,
     0.761905f,
     0.744186f,
     0.727273f,
     0.711111f,
     0.695652f,
     0.680851f,
     0.666667f};

const static float c_32 = 32.000000f;
const static float c_16 = 16.000000f;
const static float c_10_7 = 10.666667f;
const static float c_8  = 8.000000f;
const static float c_4  = 4.000000f;
const static float c_2  = 2.000000f;
const static float c_0_91 = 0.914286f;
const static float c_0_8 = 0.800000f;
const static float c_0_7 = 0.695652f;

const static float c_32768 = 32768.0f;
const static float flaretab[16] = { 0.3f, 0.8f, 1.3f, 1.8f, 2.1f, 2.5f, 2.7f, 3.1f,
         3.5f, 4.2f, 4.7f, 5.0f, 5.4f, 5.9f, 6.1f, 6.3f };
static uint32_t flare_idx;
static uint32_t flare_dx;
static uint32_t flare_dy;
static uint32_t flare_ycmp;
const static float flare_res = 32600.0f;
const static float flare_2s = -2.2f;
const static float flare_2x = 0.125f;   //x/160
const static float flare_2y = 0.1666f;  //y/160*1.3333
const static float flare_3s = -0.6f;
const static float flare_3x = 0.03125f;
const static float flare_3y = 0.0416f;
const static float flare_4s = -1.6f;
const static float flare_4x = 0.01875f;
const static float flare_4y = 0.0249999375f;
//±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
//.code
//±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±


static void traceobject(tobject *esi) {
	realnum fpu_reg10, fpu_reg11;
	uint32_t ecx;
	tobject *eax, *edx;
	tcube *edi, *eb2;
	tplane *ebx;
//-> esi -> tobject

	edi = esi->o_cube;

	if (edi == 0) goto traceobject_weg;

	ecx = ( edi->c_planes );
	if (ecx == 0) goto traceobject_weg;
	ebx = ( edi->c_planedata );
traceobject_l:

	fpu_reg10 = ( esi->o_p.x1 );
	fpu_reg10 = fpu_reg10 - ( ebx->pl_p.x1 );
	fpu_reg10 = fpu_reg10 * ( ebx->pl_n.x1 );

	fpu_reg11 = ( esi->o_p.x2 );
	fpu_reg11 = fpu_reg11 - ( ebx->pl_p.x2 );
	fpu_reg11 = fpu_reg11 * ( ebx->pl_n.x2 );
	fpu_reg10 = fpu_reg10 + fpu_reg11;

	fpu_reg11 = ( esi->o_p.x3 );
	fpu_reg11 = fpu_reg11 - ( ebx->pl_p.x3 );
	fpu_reg11 = fpu_reg11 * ( ebx->pl_n.x3 );
	fpu_reg10 = fpu_reg10 + fpu_reg11;


//st < 0: entered side-cube


	if (fpu_reg10 > 0.0) goto traceobject_next;



//entered side-cube: modify links
	eb2 = ( ebx->pl_sidecube ); //ebx -> side-cube
//bla_:

	edx = ( edi->c_objlist );

	if (esi != edx) goto traceobject_search;
//object at 1st position in list
	eax = ( esi->o_nextincube ); //remove from list
	edi->c_objlist = eax;
	goto traceobject_ins;
traceobject_search: //objekt at 2nd to last position
//or      edx,edx
//jz      @@ins
//search entry
	if (esi == ( edx->o_nextincube )) goto traceobject_rem;
	edx = ( edx->o_nextincube );
	goto traceobject_search;
traceobject_rem:
	eax = ( esi->o_nextincube ); //remove from list
	edx->o_nextincube = eax;

traceobject_ins: //insert in list of side-cube
	esi->o_cube = eb2;
	eax = ( eb2->c_objlist );
	esi->o_nextincube = eax;
	eb2->c_objlist = esi;
	goto traceobject_weg;
traceobject_next:

	ebx = ebx + ( 1 );
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto traceobject_l;

traceobject_weg:
	return;
}


//------ recursive procedure to calculate the tracks
static void dotracks(tobject *esi) {
	realnum fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13, fpu_reg14, fpu_reg15;
	uint32_t edx, ebx;
	tobject *edi;
//-> esi = *this
//<- esi = _NULL
dotracks_next:

	if (esi == 0) goto dotracks_weg;

	( esi->o_vmt->o_dotracks )(esi); //virtual method to calculate the tracks

//A = matrix, p = position
//o = owner

//A = Ao*A (matrix multiplication)
	edi = ( esi->o_owner );

	if (edi == 0) goto dotracks_n; //no owner


	if ((( esi->o_flags ) & ( ofAbsolute )) != 0) goto dotracks_n;


	if ((( esi->o_flags ) & ( ofMatrix )) == 0) goto dotracks_pos;

	ebx = 0; //ebx = n
dotracks_l2:
	fpu_reg10 = ( esi->o_A_data[ebx + 0] ); //n-th column of A
	fpu_reg11 = ( esi->o_A_data[ebx + 3] );
	fpu_reg12 = ( esi->o_A_data[ebx + 6] );

	edx = 0; //edx = m
dotracks_l1:
	fpu_reg13 = ( edi->o_A_data[edx + 0] ); //* m-th row of Ao
	fpu_reg13 = fpu_reg13 * fpu_reg10;
	fpu_reg14 = ( edi->o_A_data[edx + 1] );
	fpu_reg14 = fpu_reg14 * fpu_reg11;
	fpu_reg15 = ( edi->o_A_data[edx + 2] );
	fpu_reg15 = fpu_reg15 * fpu_reg12;
	fpu_reg14 = fpu_reg14 + fpu_reg15;
	fpu_reg13 = fpu_reg13 + fpu_reg14;
	esi->o_A_data[ebx + edx] = fpu_reg13; //= new element (m,n) of A

	edx = edx + ( 3 );

	if (edx < ( 3 * 3 )) goto dotracks_l1;





	ebx = ebx + ( 1 );

	if (ebx < ( 3 )) goto dotracks_l2;
dotracks_pos: //p = Ao*p + po

	fpu_reg10 = ( esi->o_p.x1 ); //column vektor p
	fpu_reg11 = ( esi->o_p.x2 );
	fpu_reg12 = ( esi->o_p.x3 );

	edx = 0; //edx = m for A
	ebx = 0; //ebx = m for p
dotracks_l3:
	fpu_reg13 = ( edi->o_A_data[edx + 0] ); //* m-th row of Ao
	fpu_reg13 = fpu_reg13 * fpu_reg10;
	fpu_reg14 = ( edi->o_A_data[edx + 1] );
	fpu_reg14 = fpu_reg14 * fpu_reg11;
	fpu_reg15 = ( edi->o_A_data[edx + 2] );
	fpu_reg15 = fpu_reg15 * fpu_reg12;
	fpu_reg14 = fpu_reg14 + fpu_reg15;
	fpu_reg13 = fpu_reg13 + fpu_reg14;
	fpu_reg13 = fpu_reg13 + ( edi->o_p_data[ebx] );
	esi->o_p_data[ebx] = fpu_reg13; //= new element (m,1) of p

	edx = edx + 3;
	ebx = ebx + 1;

	if (ebx < ( 3 )) goto dotracks_l3;





dotracks_n:
	traceobject(esi);

	dotracks(esi->o_child); //next hierarchy level

	esi = ( esi->o_next ); //next object
	goto dotracks_next;
dotracks_weg:
	return;
}



static void doswitchtrack(void *ebx, tobject *esi) { //pascal
	uint32_t eax, edx, ecx;
	int32_t *edi;
	tobject *ed2;
	tcube *eb2, *ed3, *ec2;
//ebx -> track

	edi = (int32_t *)( ((ttrack *)(ebx))->t_data ); //edi -> track

	ec2 = 0; //no switch
	edx = ( ((ttrack *)(ebx))->t_key ); //edx = key

doswitchtrack_l:
//end of track? (edi -> keys)
	if (edx >= ( *((uint32_t *)(edi)) )) goto doswitchtrack_w;

	eax = ( *((uint32_t *)(edi + edx * 2 + (1))) ); //check switch-frame

	if (eax > frame) goto doswitchtrack_w; //not yet reached
	ecx = ( *((uint32_t *)(edi + edx * 2 + (1 + 1))) ); //get cubenum
	ec2 = &scene.sc_cubedata[ecx]; //change cube number to cube pointer
	edx = edx + 1; //next key
	goto doswitchtrack_l;
doswitchtrack_w:
	((ttrack *)(ebx))->t_key = edx; //write key back

	if (ec2 == 0) goto doswitchtrack_weg;
	ed3 = ( esi->o_cube );
	eb2 = ec2;
//jmp     bla_
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
//bla_:

	ed2 = ( ed3->c_objlist );

	if (esi != ed2) goto doswitchtrack_search;
//object at 1st position in list
	ed3->c_objlist = esi->o_nextincube; //remove from list
	goto doswitchtrack_ins;
doswitchtrack_search: //objekt at 2nd to last position
//or      edx,edx
//jz      @@ins
//search entry
	if (esi == ( ed2->o_nextincube )) goto doswitchtrack_rem;
	ed2 = ( ed2->o_nextincube );
	goto doswitchtrack_search;
doswitchtrack_rem:
	ed2->o_nextincube = esi->o_nextincube; //remove from list

doswitchtrack_ins: //insert in list of side-cube
	esi->o_cube = eb2;
	esi->o_nextincube = eb2->c_objlist;
	eb2->c_objlist = esi;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
doswitchtrack_weg:
	return;
}




extern "C" void m_dotracks(tmesh *esi) {
	uint32_t edx;
//-> esi -> tmesh


	if ((( esi->m_object.o_flags ) & ( ofAbsolute )) != 0) goto m_dotracks_abs;

//rotation track (4)
	dorottrack(( esi->m_rottrack ), (float *)&(esi->m_object.o_A));
#if 0
//                ;scale track (3)
//        lea     edi,o_p[esi]
//        call    dotrack pascal, [esi].m_scaletrack, 3
//
//        mov     ecx,2                   ;scale-value into rotation matrix
//@@l:
//        fld     o_p[esi+ecx*4]
//        fld     o_A[esi+ecx*4]
//        fmul    st,st(1)
//        fstp    o_A[esi+ecx*4]
//        fld     o_A[esi+ecx*4+3*4]
//        fmul    st,st(1)
//        fstp    o_A[esi+ecx*4+3*4]
//        fld     o_A[esi+ecx*4+6*4]
//        fmulp   st(1)
//        fstp    o_A[esi+ecx*4+6*4]
//        dec     ecx
//        jns     @@l
#endif
//position track (3)
	dotrack(( esi->m_postrack ), ( 3 ), (float *)&(esi->m_object.o_p));

	goto m_dotracks_abs0;
m_dotracks_abs: //absolute

//position track (3)
	doltrack(( esi->m_postrack ), ( 3 ), (float *)&(esi->m_object.o_p));


//morphtrack (2*vertices*3)

	if ((( esi->m_object.o_flags ) & ( mfMorph )) == 0) goto m_dotracks_m0;

	edx = ( esi->m_vertices );
	edx = ( edx + edx * 2 ); //edx*3
	doltrack(( esi->m_morphtrack ), edx, esi->m_vertexlist);
//comment #
//normals
	edx = ( esi->m_vertices );
	edx = ( edx + edx * 2 ); //edx*3
	doltrack(( esi->m_nmorphtrack ), edx, edx + esi->m_vertexlist);
//#
m_dotracks_m0:

m_dotracks_abs0:

//hidetrack
	dohidetrack(&(esi->m_hidetrack), (int32_t *)&(esi->m_object.o_hidden)); //modifies o_hidden[esi]
//        mov     eax,m_hidden[esi]       ;copy flag
//        mov     o_hidden[esi],eax

	return;
}



static void v_dotracks(toviewer *esi) {
//position track (3)
	dotrack(( esi->v_postrack ), ( 3 ), (float *)&(esi->v_object.o_p)); //eax, 3

//FOV track (1)
	dotrack(( esi->v_FOVtrack ), ( 1 ), (float *)&(esi->v_FOV)); //eax, 1

//roll track (1)
	dotrack(( esi->v_rolltrack ), ( 1 ), (float *)&(esi->v_roll)); //eax, 1

	return;
}


extern "C" void c_dotracks(tcamera *esi) {
//-> esi -> tcamera

	v_dotracks(&(esi->c_viewer));

	doswitchtrack(&(esi->c_switchtrack), &(esi->c_viewer.v_object));

//no matrix
	return;
}


extern "C" void l_dotracks(tlight *esi) {
//-> esi -> tlight

	v_dotracks(&(esi->l_viewer));

//hidetrack
	dohidetrack(&(esi->l_hidetrack), (int32_t *)&(esi->l_viewer.v_object.o_hidden)); //modifies o_hidden[esi]

//l_dotracks_weg:
	return;
}


extern "C" void t_dotracks(ttarget *esi) {
//-> esi -> ttarget

//position track (3)
	dotrack(( esi->t_postrack ), ( 3 ), (float *)&(esi->t_object.o_p));

//no matrix
//t_dotracks_weg:
	return;
}


extern "C" void b_dotracks(btarget *esi) {
//-> esi -> tbound

//position track (3)

	if ((( esi->b_object.o_flags ) & ( ofAbsolute )) != 0) goto b_dotracks_a;
	dotrack(( esi->b_postrack ), ( 3 ), (float *)&(esi->b_object.o_p));
	goto b_dotracks_a0;
b_dotracks_a:
	doltrack(( esi->b_postrack ), ( 3 ), (float *)&(esi->b_object.o_p));
b_dotracks_a0:

//no matrix

//b_dotracks_weg:
	return;
}




static void doviewer(toviewer *esi, tviewer *edi) {
	realnum fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13, fpu_reg14, fpu_reg15;
	uint32_t ecx;
	tobject *ebx;
//-> esi -> toviewer
//-> edi -> tviewer
//<- esi -> toviewer
//<- edi -> tviewer

	edi->v_cube = esi->v_object.o_cube;

	ebx = (tobject *)( esi->v_target );

//direction vector u to target
	fpu_reg10 = ( esi->v_object.o_p.x1 );
	edi->v_p.x1 = fpu_reg10;
	fpu_reg10 = ( ebx->o_p.x1 ) - fpu_reg10;
	fpu_reg11 = ( esi->v_object.o_p.x2 );
	edi->v_p.x2 = fpu_reg11;
	fpu_reg11 = ( ebx->o_p.x2 ) - fpu_reg11;
	fpu_reg12 = ( esi->v_object.o_p.x3 );
	edi->v_p.x3 = fpu_reg12;
	fpu_reg12 = ( ebx->o_p.x3 ) - fpu_reg12;

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
	edi->v_l3.x1 = fpu_reg15;
	fpu_reg15 = fpu_reg11;
	fpu_reg15 = fpu_reg15 / fpu_reg14;
	edi->v_l3.x2 = fpu_reg15;
	fpu_reg14 = fpu_reg12 / fpu_reg14;
	edi->v_l3.x3 = fpu_reg14;

//l1                     ;l1 = (u2,-u1,0)/sqrt(u1^2 + u2^2)
	fpu_reg13 = sqrt(fpu_reg13); //sqrt(u1^2 + u2^2)

	fpu_reg14 = fpu_reg11; //u2/sqrt(u1^2 + u2^2)
	fpu_reg14 = fpu_reg14 / fpu_reg13;
	edi->v_l1.x1 = fpu_reg14;

	fpu_reg10 = fpu_reg10 / fpu_reg13; //-u1/sqrt(u1^2 + u2^2)
//remove u3
//remove u2
	fpu_reg10 = -fpu_reg10;
	edi->v_l1.x2 = fpu_reg10;

	fpu_reg10 = 0.0; //0
	edi->v_l1.x3 = fpu_reg10;

//l2                     ;l2 = l1 x l3
	fpu_reg10 = ( edi->v_l3.x1 ); //b1
	fpu_reg11 = ( edi->v_l1.x2 ); //a2
	fpu_reg12 = ( edi->v_l3.x2 ); //b2
	fpu_reg13 = ( edi->v_l1.x1 ); //a1
	fpu_reg14 = ( edi->v_l3.x3 ); //b3

	fpu_reg15 = fpu_reg11;
	fpu_reg15 = fpu_reg15 * fpu_reg14;
	edi->v_l2.x1 = fpu_reg15; //a2*b3

	fpu_reg14 = fpu_reg14 * fpu_reg13;
	fpu_reg14 = -fpu_reg14;
	edi->v_l2.x2 = fpu_reg14; //-a1*b3

	fpu_reg12 = fpu_reg12 * fpu_reg13;
	{ realnum tmp = fpu_reg11; fpu_reg11 = fpu_reg12; fpu_reg12 = tmp; }
	fpu_reg10 = fpu_reg10 * fpu_reg12;
	fpu_reg10 = fpu_reg11 - fpu_reg10;
	edi->v_l2.x3 = fpu_reg10; //a1*b2-a2*b1

//calc l1 and l2 scale factors for camera opening angle
//        fld     v_pi_2                  ;1/tan(FOV) = tan(pi/2 - FOV)
//        fsub    v_FOV[esi]              ;FOV = halve camera view angle
//        fptan
//        fstp    st
//        fld     st
//        fmulp   st(2),st                ;st(2) = (x:y-ratio) * 1/tan(FOV)

//calc l1 and l2 scale factors for rotation about the view axis
	fpu_reg10 = ( esi->v_roll );
	fpu_reg11 = cos(fpu_reg10);
	fpu_reg10 = sin(fpu_reg10);

//rotate & scale l1 and l2
	ecx = ( 2 );
doviewer_l:
//l1' = (l1*cos(roll) + l2*sin(roll))/tan(FOV)
	fpu_reg12 = ( edi->v_l1_data[ecx] );
	fpu_reg12 = fpu_reg12 * fpu_reg11; //cos
	fpu_reg13 = ( edi->v_l2_data[ecx] );
	fpu_reg13 = fpu_reg13 * fpu_reg10; //sin
	fpu_reg12 = fpu_reg12 + fpu_reg13;
//        fmul    st,st(3)                ;1/tan(FOV)
//l2' = (l2*cos(roll) + l1*sin(roll))*(x:y ratio)/tan(FOV)
	fpu_reg13 = ( edi->v_l2_data[ecx] );
	fpu_reg13 = fpu_reg13 * fpu_reg11; //cos
	fpu_reg14 = ( edi->v_l1_data[ecx] );
	fpu_reg14 = fpu_reg14 * fpu_reg10; //sin
	fpu_reg13 = fpu_reg13 - fpu_reg14;
//        fmul    st,st(5)                ;(x:y ratio)/tan(FOV)

	edi->v_l2_data[ecx] = fpu_reg13;
	edi->v_l1_data[ecx] = fpu_reg12;
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) >= 0) goto doviewer_l;

//        fstp    st
//        fstp    st


	return;
}



static void adjustcamera(tcamera *esi, tviewer *edi) {
	realnum fpu_reg10, fpu_reg11, fpu_reg12;
	uint32_t ecx;
//-> esi -> tcamera
//-> edi -> tviewer

//calc l1 and l2 scale factors for camera opening angle
	fpu_reg10 = c_pi_2; //1/tan(FOV) = tan(pi/2 - FOV)
	fpu_reg10 = fpu_reg10 - ( esi->c_viewer.v_FOV ); //FOV = halve camera view angle
	fpu_reg10 = tan(fpu_reg10);
	fpu_reg11 = 1.0;

	edi->v_1_tan_fov = fpu_reg10;
	fpu_reg11 = fpu_reg10;
	fpu_reg11 = fpu_reg11 * c_1_33; //st(0) = (x:y-ratio) * 1/tan(FOV)

	ecx = ( 2 );
adjustcamera_l:
	fpu_reg12 = ( edi->v_l1_data[ecx] );
	fpu_reg12 = fpu_reg12 * fpu_reg10;
	edi->v_l1_data[ecx] = fpu_reg12;
	fpu_reg12 = ( edi->v_l2_data[ecx] );
	fpu_reg12 = fpu_reg12 * fpu_reg11;
	edi->v_l2_data[ecx] = fpu_reg12;
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) >= 0) goto adjustcamera_l;




	return;
}



static void adjustlight(tlight *esi, tlviewer *edi) {
	realnum fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13;
	uint32_t ecx;
//-> esi -> tlight
//-> edi -> tlviewer

	edi->lv_flags = esi->l_viewer.v_object.o_flags;

//calc l1 and l2 scale factors for camera opening angle
	fpu_reg10 = ( esi->l_viewer.v_FOV );
	fpu_reg10 = tan(fpu_reg10);
	fpu_reg11 = 1.0;

	edi->lv_viewer.v_1_tan_fov = fpu_reg10;
	fpu_reg11 = fpu_reg10;
	fpu_reg11 = fpu_reg11 * c_0_91; //vol. light smaller than spot area
	{ realnum tmp = fpu_reg10; fpu_reg10 = fpu_reg11; fpu_reg11 = tmp; }
	fpu_reg12 = 1.0;
	fpu_reg11 = fpu_reg12 / fpu_reg11; //st(0) = 1/tan(FOV)
//st(1) = tan(FOV)

	ecx = ( 2 );
adjustlight_l:
	fpu_reg12 = ( edi->lv_viewer.v_l1_data[ecx] );
	fpu_reg13 = fpu_reg12;
	fpu_reg13 = fpu_reg13 * fpu_reg11;
	edi->lv_viewer.v_l1_data[ecx] = fpu_reg13;
	fpu_reg12 = fpu_reg12 * fpu_reg10;
	edi->lv_l1_data[ecx] = fpu_reg12;

	fpu_reg12 = ( edi->lv_viewer.v_l2_data[ecx] );
	fpu_reg13 = fpu_reg12;
	fpu_reg13 = fpu_reg13 * fpu_reg11;
	edi->lv_viewer.v_l2_data[ecx] = fpu_reg13;
	fpu_reg12 = fpu_reg12 * fpu_reg10;
	edi->lv_l2_data[ecx] = fpu_reg12;
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) >= 0) goto adjustlight_l;




	return;
}




static void cubetree(uint32_t numcubes, tviewer *esi, ttempcube *edi) {
	realnum fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13, fpu_reg14, fpu_reg15;
	uint32_t eax, ecx;
	tprojected *edx;
	tobject *es2, *stack_var00;
	tvec *ea2;
	tplane *ed2;
	ttempcube *ed3;
	tmesh **ec2, *ed4;
	tcube *ebx;
	tviewer *eb2;

//-> esi -> tviewer (camera/light)
//-> edi -> tempdata
	ttempcube *readptr, *listend;
	tplane *planeptr;
	uint32_t planecnt, recursion;
	float xmax, ymax, xmin, ymin;
	float nxmax, nymax, nxmin, nymin;
	uint8_t c1, c2, c3, c4;

//edi -> dest. tempcube
//ebx -> source cube
	esi->v_cubelist = edi; //edi -> destination in tempcube list
	readptr = edi; //read position in tempcube list
//     sub readptr,size ttempcube
	listend = edi;
	ebx = ( esi->v_cube ); //ebx -> source-cube

	planeptr = ebx->c_planedata;
	planecnt = ebx->c_planes;

	stamp = stamp + 1;
	c1 = (( 50 )); //c1 = 50 to indicate first pass


	fpu_reg10 = 1.0;
	xmax = fpu_reg10;
	ymax = fpu_reg10;
	nxmin = fpu_reg10;
	nymin = fpu_reg10;
	fpu_reg10 = -fpu_reg10;
	xmin = fpu_reg10;
	ymin = fpu_reg10;
	nxmax = fpu_reg10;
	nymax = fpu_reg10;

	recursion = ( 0 );
cubetree_l: //cube loop
	edi->tc_recursion = recursion;

	edi->tc_cubeptr = ebx; //connect cube to temp-cube

	ebx->c_stamp = stamp; //mark cube as "used"
//8 vertices to transform
	ecx = 0; //vertex-count (0 to 7)
	edi->tc_meshlist = 0; //clear mesh-temp-list

	edx = ( &(edi->tc_proj[0]) ); //edx -> tprojected
cubetree_v_l: //vertex transform loop
//v : vertex to transform
//l : position of viewer
//l1: right hand vector of viewer
//l2: up vector of viewer
//l3: viewing direction
//x : transformed vector

	ea2 = (tvec *)( ebx->c_vertex[ecx] ); //eax -> v

	fpu_reg10 = ( ea2->x1 ); //v-l (relative position to viewer)
	fpu_reg10 = fpu_reg10 - ( esi->v_p.x1 );
	fpu_reg11 = ( ea2->x2 );
	fpu_reg11 = fpu_reg11 - ( esi->v_p.x2 );
	fpu_reg12 = ( ea2->x3 );
	fpu_reg12 = fpu_reg12 - ( esi->v_p.x3 ); //st(2) - st(0) = (v-l)

	fpu_reg13 = ( esi->v_l1.x1 ); //(v-l)*l1 = x
	fpu_reg13 = fpu_reg13 * fpu_reg10;
	fpu_reg14 = ( esi->v_l1.x2 );
	fpu_reg14 = fpu_reg14 * fpu_reg11;
	fpu_reg15 = ( esi->v_l1.x3 );
	fpu_reg15 = fpu_reg15 * fpu_reg12;
	fpu_reg14 = fpu_reg14 + fpu_reg15;
	fpu_reg13 = fpu_reg13 + fpu_reg14;
	edx->pr_x = fpu_reg13;

	fpu_reg13 = ( esi->v_l2.x1 ); //(v-l)*l2 = y
	fpu_reg13 = fpu_reg13 * fpu_reg10;
	fpu_reg14 = ( esi->v_l2.x2 );
	fpu_reg14 = fpu_reg14 * fpu_reg11;
	fpu_reg15 = ( esi->v_l2.x3 );
	fpu_reg15 = fpu_reg15 * fpu_reg12;
	fpu_reg14 = fpu_reg14 + fpu_reg15;
	fpu_reg13 = fpu_reg13 + fpu_reg14;
	edx->pr_y = fpu_reg13;

	fpu_reg13 = ( esi->v_l3.x1 ); //(v-l)*l3 = z
	fpu_reg10 = fpu_reg10 * fpu_reg13;
	fpu_reg13 = ( esi->v_l3.x2 );
	fpu_reg11 = fpu_reg11 * fpu_reg13;
	fpu_reg13 = ( esi->v_l3.x3 );
	fpu_reg12 = fpu_reg12 * fpu_reg13;
	fpu_reg11 = fpu_reg11 + fpu_reg12;
	fpu_reg10 = fpu_reg10 + fpu_reg11;
	edx->pr_z = fpu_reg10;

	edx = edx + ( 1 );
	ecx = ecx + 1;

	if (ecx < ( 8 )) goto cubetree_v_l;
//test if the cube is visible
//ecx = 8
//is this the first pass (c1 == 50)?
	if (c1 != ( 50 )) goto cubetree_not1st;
	c1 = (( (uint8_t)ecx )); //c1 = 8 (right border counter)
	goto cubetree_1st; //jump to first pass label
cubetree_not1st:
	c1 = (( (uint8_t)ecx )); //c1 = 8 (right border counter)
	c2 = (( (uint8_t)ecx )); //c2 = 8 (left border counter)
	c3 = (( (uint8_t)ecx )); //c3 = 8 (upper border counter)
	c4 = (( (uint8_t)ecx )); //c4 = 8 (lower border counter)

	edx = ( &(edi->tc_proj[0]) ); //edx -> tprojected
cubetree_t_l: //vertex test loop
//cube not visible if counter empty
	if (( (int32_t)ecx ) == 0) goto cubetree_n_in;


	fpu_reg10 = ( edx->pr_x ); //test right border
	fpu_reg11 = ( edx->pr_z );
	fpu_reg11 = fpu_reg11 * xmax;
//x > z*xmax ?
	eax = ((fpu_reg11 < fpu_reg10)?0x100:0);
	eax = set_high_byte(eax, ( (uint8_t)(eax >> 8) ) & ( 1 )); //if vertex can not be seen
	c1 = (c1 - ( (uint8_t)(eax >> 8) )); // decremet right border counter

	fpu_reg11 = ( edx->pr_z ); //test left border
	fpu_reg11 = fpu_reg11 * xmin;
//  st(1)                   ;x < z*xmin ?
	eax = ((fpu_reg11 < fpu_reg10)?0x100:0);
	eax = set_high_byte(eax, ( (uint8_t)(eax >> 8) ) & ( 1 ));
	eax = set_high_byte(eax, ( (uint8_t)(eax >> 8) ) - 1); //if vertex can not be seen
	c2 = (c2 + ( (uint8_t)(eax >> 8) )); // decremet left border counter

//fstp    st

	fpu_reg10 = ( edx->pr_y ); //test upper border
	fpu_reg11 = ( edx->pr_z );
	fpu_reg11 = fpu_reg11 * ymax;
//y > z*ymax ?
	eax = ((fpu_reg11 < fpu_reg10)?0x100:0);
	eax = set_high_byte(eax, ( (uint8_t)(eax >> 8) ) & ( 1 )); //if vertex can not be seen
	c3 = (c3 - ( (uint8_t)(eax >> 8) )); // decremet right border counter

	fpu_reg11 = ( edx->pr_z ); //test lower border
	fpu_reg11 = fpu_reg11 * ymin;
//   st(1)                   ;y < z*ymin ?
	eax = ((fpu_reg11 < fpu_reg10)?0x100:0);
	eax = set_high_byte(eax, ( (uint8_t)(eax >> 8) ) & ( 1 ));
	eax = set_high_byte(eax, ( (uint8_t)(eax >> 8) ) - 1); //if vertex can not be seen
	c4 = (c4 + ( (uint8_t)(eax >> 8) )); // decremet left border counter

//fstp    st


	edx = edx + ( 1 );
	ecx = ecx - 1;

//continue only if all vertices lie
	if (( (uint8_t)ecx ) == c1) goto cubetree_t_l; // out of one border so far

	if (( (uint8_t)ecx ) == c2) goto cubetree_t_l;

	if (( (uint8_t)ecx ) == c3) goto cubetree_t_l;

	if (( (uint8_t)ecx ) == c4) goto cubetree_t_l;
//cube visible if it came so far
cubetree_1st:

	edx = ( &(edi->tc_proj[0]) ); //edx -> tprojected
	ecx = ( 8 );
cubetree_p_l: //projection loop

	fpu_reg10 = ( edx->pr_y );
	fpu_reg11 = ( edx->pr_x );
	fpu_reg12 = ( edx->pr_z );
//st(0) = z, st(1) = x, st(2) = y
	fpu_reg13 = 1.0;



	if (fpu_reg13 > fpu_reg12) goto cubetree_clip;
	fpu_reg11 = fpu_reg11 / fpu_reg12; //x/z
	fpu_reg10 = fpu_reg10 / fpu_reg12; //y/z

	edx->pr_inn = (uint8_t) (( 1 ));
	goto cubetree_clip0;
cubetree_clip:
//remove z
//        or      clip,1                  ;set clip flag
	edx->pr_inn = (uint8_t) (( 0 ));

cubetree_clip0:




	if (fpu_reg11 < nxmax) goto cubetree_xmax0;
	nxmax = fpu_reg11;
//jmp     @@x0
cubetree_xmax0:



	if (fpu_reg11 > nxmin) goto cubetree_x0;
	nxmin = fpu_reg11;
cubetree_x0:
	edx->pr_sx = fpu_reg11;





	if (fpu_reg10 < nymax) goto cubetree_ymax0;
	nymax = fpu_reg10;
//jmp     @@y0
cubetree_ymax0:



	if (fpu_reg10 > nymin) goto cubetree_y0;
	nymin = fpu_reg10;
cubetree_y0:
	edx->pr_sy = fpu_reg10;


	edx = edx + ( 1 );
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto cubetree_p_l;


//        cmp     v_light[esi],0          ;camera or light?
//        jne     @@m
//        mov     eax,stamp               ;camera :
//        mov     c_vstamp[ebx],eax       ; extra mark for light procedures
//        mov     c_vtc[ebx],edi          ; extra link to temp-cube
//@@m:
	edi = edi + ( 1 ); //new tempcube
	numcubes = ( (int32_t)numcubes ) - 1;
	if (( (int32_t)numcubes ) == 0) goto cubetree_w;
//cmp     edi,v_cubelistmax[esi]  ;reached end of tempcube-list?
//jae     @@w                     ;yes -> exit loop
cubetree_n_in: //label for cube not visible


	ed2 = planeptr;
	eax = planecnt;
cubetree_used:
cubetree_new:

	if (( (int32_t)eax ) == 0) goto cubetree_next;
	ebx = ( ed2->pl_sidecube );

	ed2 = ed2 + ( 1 );
	eax = eax - 1;

	ecx = stamp;

	if (ecx == ( ebx->c_stamp )) goto cubetree_used;

	planeptr = ed2;
	planecnt = eax;
	goto cubetree_l;
cubetree_next:
	readptr = readptr + ( 1 );
	ed3 = readptr;

//end of hierarchy level?
	if (ed3 < listend) goto cubetree_n2;

	if (ed3 >= edi) goto cubetree_w; //no new cubes read
	listend = edi;
	recursion = recursion + 1;

	fpu_reg10 = nxmax; //minimize view area



	if (fpu_reg10 > xmax) goto cubetree_xmax1;
	xmax = fpu_reg10;
cubetree_xmax1:


	fpu_reg10 = nxmin;



	if (fpu_reg10 < xmin) goto cubetree_xmin1;
	xmin = fpu_reg10;
cubetree_xmin1:


	fpu_reg10 = nymax;



	if (fpu_reg10 > ymax) goto cubetree_ymax1;
	ymax = fpu_reg10;
cubetree_ymax1:


	fpu_reg10 = nymin;



	if (fpu_reg10 < ymin) goto cubetree_ymin1;
	ymin = fpu_reg10;
cubetree_ymin1:



	fpu_reg10 = 1.0;
	nxmin = fpu_reg10;
	nymin = fpu_reg10;
	fpu_reg10 = -fpu_reg10;
	nxmax = fpu_reg10;
	nymax = fpu_reg10;
cubetree_n2:

	ebx = ( ed3->tc_cubeptr );
	ed2 = ( ebx->c_planedata );
	eax = ( ebx->c_planes );
	goto cubetree_new;
cubetree_w: //end of loop


	scene.sc_datapos = edi; //edi -> cubelistend
//v_tlist[esi],edi
//-----
//generate the tempcube temp-meshlist
	listend = edi;

	edi = ( esi->v_cubelist );
	eb2 = esi; //ebx -> tviewer
cubetree_c_l: //temp cube loop

	if (edi >= listend) goto cubetree_weg;

	es2 = ( edi->tc_cubeptr->c_objlist ); //esi -> tobject
cubetree_o_l: //object loop

	if (es2 == 0) goto cubetree_mw;
	stack_var00 = es2;

cubetree_owner:
//go to top hierarchy level,
	if (( es2->o_owner ) == ( _NULL )) goto cubetree_top; // whole hierarchy trees are
	es2 = ( es2->o_owner ); // inserted into the temp-meshlist
	goto cubetree_owner;
cubetree_top:

	if (( es2->o_hidden ) != ( 0 )) goto cubetree_hw; //top level hidden

	if (( es2->o_stamp ) == stamp) goto cubetree_hw; //is object already used?
	es2->o_stamp = stamp; //mark it as used


	if ((( es2->o_flags ) & ( ofMesh )) == 0) goto cubetree_hw;

	fpu_reg10 = ( es2->o_p.x1 ); //calc z distance from viewer
	fpu_reg10 = fpu_reg10 * ( eb2->v_l3.x1 ); //ebx -> tviewer
	fpu_reg11 = ( es2->o_p.x2 );
	fpu_reg11 = fpu_reg11 * ( eb2->v_l3.x2 );
	fpu_reg10 = fpu_reg10 + fpu_reg11;
	fpu_reg11 = ( es2->o_p.x3 );
	fpu_reg11 = fpu_reg11 * ( eb2->v_l3.x3 );
	fpu_reg10 = fpu_reg10 + fpu_reg11;

	((tmesh *)es2)->m_z = fpu_reg10;

	//ec2 = ( &(edi->tc_meshlist) ); //ecx -> next pointer
	// despite warning, ec2 is an aligned pointer value
	ec2 = (tmesh **)&(((uint8_t *)edi)[offsetof(ttempcube, tc_meshlist)]); //ecx -> next pointer
cubetree_i_l: //edx -> tmesh
	ed4 = ( *ec2 );
//temp list z sorting
	if (ed4 == 0) goto cubetree_ins;

	eax = ((fpu_reg10 < ( ed4->m_z ))?0x100:0);
	eax = set_high_byte(eax, ( (uint8_t)(eax >> 8) ) ^ ( (uint8_t)(eb2->v_light) )); //reverse z order for light

	if ((eax & 0x100) == 0) goto cubetree_ins;
	ec2 = ( &(ed4->m_nexttemp) );
	goto cubetree_i_l;
cubetree_ins:
	((tmesh *)es2)->m_nexttemp = ed4;
	*ec2 = (tmesh *)es2;

//remove z

cubetree_hw:
	es2 = stack_var00;
	es2 = ( es2->o_nextincube );
	goto cubetree_o_l;
cubetree_mw:
	edi = edi + ( 1 );
	goto cubetree_c_l;
cubetree_weg:
	return;
}



static uint32_t xclip(uint32_t spsize, uint32_t vars, tscreenpoint *&_ebx, tscreenpoint *&_edi) { //x-clipping
	realnum fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13;
	uint32_t eax, edx, ecx;
	tscreenpoint *edi = _edi, *ebx = _ebx, *esi;
//data variables -1
//-> ebx = *sp
//-> edi = *sp_end
//<- ebx = *sp
//<- edi = *sp_end
//<- wenn weniger als 3 punkte, mit jbe wegspringen
	tscreenpoint *_sp, *_sp_end; //beides pointer

	fpu_reg10 = 0.0; //st = x
	edx = /*(edx & 0xffffff00) |*/ (uint8_t)(( 1 )); //dl = i, 1 to 0
xclip_i_l:
	_sp = ebx; //ebx -> sp[z] (quellpunkte) (z = 0)
	_sp_end = edi; //edi -> sp[d] (zielpunkte)  (d = m)

xclip_z_l:
	fpu_reg11 = ( ebx->sp_x ); //inn = (sp[z].sx >= x);

	eax = /*(eax & 0xffff0000) |*/ ((fpu_reg11 < fpu_reg10)?0x100:0);
	edx = set_high_byte(edx, ( (uint8_t)(eax >> 8) )); //dh = inn
	eax = set_high_byte(eax, ( (uint8_t)(eax >> 8) ) ^ ( (uint8_t)edx )); //inn xor i

	if ((eax & 0x100) == 0) goto xclip_0;
//punkt innerhalb
	memcpy(edi, ebx, spsize); //sp[d] = sp[z] und d++
	edi = (tscreenpoint *)(((uintptr_t)edi) + spsize);
xclip_0:
	esi = ebx;
	esi = (tscreenpoint *)(((uintptr_t)esi) + spsize); //nz = z+1
//if (nz >= m) nz = 0;
	if (esi < _sp_end) goto xclip_wrap;
	esi = _sp;
xclip_wrap:

	fpu_reg11 = ( esi->sp_x ); //if (inn ^ (sp[nz].sx >= x))

	eax = /*(eax & 0xffff0000) |*/ ((fpu_reg11 < fpu_reg10)?0x100:0);
	eax = set_high_byte(eax, ( (uint8_t)(eax >> 8) ) ^ ( (uint8_t)(edx >> 8) )); //dh = inn

	if ((eax & 0x100) == 0) goto xclip_1;
//dieser oder n„chster punkt auáerhalb

	edi->sp_x = fpu_reg10; //sp[d].sx speichern

//r berechnen
	fpu_reg11 = fpu_reg10; //x                    ;(x - sp[z].sx)
	fpu_reg11 = fpu_reg11 - ( ebx->sp_x );
	fpu_reg12 = ( esi->sp_x ); // /(sp[nz].sx - sp[z].sx)
	fpu_reg12 = fpu_reg12 - ( ebx->sp_x );
	fpu_reg11 = fpu_reg11 / fpu_reg12; //st = r

	fpu_reg12 = ( ebx->sp_y ); //sp[d].sy berechnen
	fpu_reg13 = ( esi->sp_y ); // sp[z].sy + r*(sp[nz].sy - sp[z].sy)
	fpu_reg13 = fpu_reg13 - fpu_reg12;
	fpu_reg13 = fpu_reg13 * fpu_reg11;
	fpu_reg12 = fpu_reg12 + fpu_reg13;
	edi->sp_y = fpu_reg12;

	ecx = vars;
xclip_vars:
	fpu_reg12 = ( esi->sp_data[ecx] );
	fpu_reg12 = fpu_reg12 - ( ebx->sp_data[ecx] );
	fpu_reg12 = fpu_reg12 * fpu_reg11;
	fpu_reg12 = fpu_reg12 + ( ebx->sp_data[ecx] );
	edi->sp_data[ecx] = fpu_reg12;
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) >= 0) goto xclip_vars;

//r entfernen

	edi = (tscreenpoint *)(((uintptr_t)edi) + spsize); //d++
xclip_1:
//wz > z?
	if (esi <= ebx) goto xclip_notz_l;
	ebx = esi; //entspricht ebx += spsize
	goto xclip_z_l; //z-schleife
xclip_notz_l:
	ebx = esi; //entspricht ebx += spsize

	ebx = _sp_end; //ebx -> sp
//edi -> sp_end
	eax = (uintptr_t)edi - (uintptr_t)ebx;
	eax = eax >> ( 1 );

	if (eax <= spsize) goto xclip_w;

	fpu_reg10 = fpu_reg10 + ( (int32_t)xres ); //x += xres
	edx = /*(edx & 0xffffff00) |*/ (uint8_t)(( (int8_t)edx ) - 1); //i-schleife
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


static uint32_t subxclip(uint32_t spsize, uint32_t vars_1, tscreenpoint *&_ebx, tscreenpoint *&_edi) { //x-clipping fr scanline subdiv.
	realnum fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13;
	uint32_t eax, edx, ecx;
	tscreenpoint *edi = _edi, *ebx = _ebx, *esi;
//data variables -1
//-> ebx = *sp
//-> edi = *sp_end
//<- ebx = *sp
//<- edi = *sp_end
//<- wenn weniger als 3 punkte, mit jbe wegspringen
	tscreenpoint *_sp, *_sp_end; //beides pointer

	fpu_reg10 = 0.0; //st = x
	edx = /*(edx & 0xffffff00) |*/ (uint8_t)(( 1 )); //dl = i, 1 to 0
subxclip_i_l:
	_sp = ebx; //ebx -> sp[z] (quellpunkte) (z = 0)
	_sp_end = edi; //edi -> sp[d] (zielpunkte)  (d = m)

subxclip_z_l:
	fpu_reg11 = ( (ebx)->sp_x ); //inn = (sp[z].sx >= x);

	eax = /*(eax & 0xffff0000) |*/ ((fpu_reg11 < fpu_reg10)?0x100:0);
	edx = set_high_byte(edx, ( (uint8_t)(eax >> 8) )); //dh = inn
	eax = set_high_byte(eax, ( (uint8_t)(eax >> 8) ) ^ ( (uint8_t)edx )); //inn xor i

	if ((eax & 0x100) == 0) goto subxclip_0;
//punkt innerhalb
	memcpy(edi, ebx, spsize); //sp[d] = sp[z] und d++
	edi = (tscreenpoint *)(((uintptr_t)edi) + spsize);
subxclip_0:
	esi = ebx;
	esi = (tscreenpoint *)(((uintptr_t)esi) + spsize); //nz = z+1
//if (nz >= m) nz = 0;
	if (esi < _sp_end) goto subxclip_wrap;
	esi = _sp;
subxclip_wrap:

	fpu_reg11 = ( esi->sp_x ); //if (inn ^ (sp[nz].sx >= x))

	eax = /*(eax & 0xffff0000) |*/ ((fpu_reg11 < fpu_reg10)?0x100:0);
	eax = set_high_byte(eax, ( (uint8_t)(eax >> 8) ) ^ ( (uint8_t)(edx >> 8) )); //dh = inn

	if ((eax & 0x100) == 0) goto subxclip_1;
//dieser oder n„chster punkt auáerhalb
	edi->sp_x = fpu_reg10;
//sp[d].sy berechnen
	fpu_reg11 = ( esi->sp_y ); //(sp[nz].sy - sp[z].sy)
	fpu_reg11 = fpu_reg11 - ( ebx->sp_y );
	fpu_reg12 = ( esi->sp_x ); // /(sp[nz].sx - sp[z].sx)
	fpu_reg12 = fpu_reg12 - ( ebx->sp_x );
	fpu_reg11 = fpu_reg11 / fpu_reg12;
	fpu_reg12 = fpu_reg10; //x
	fpu_reg12 = fpu_reg12 - ( ebx->sp_x ); //*(x - sp[z].sx)
	fpu_reg11 = fpu_reg11 * fpu_reg12;
	fpu_reg11 = fpu_reg11 + ( ebx->sp_y ); //+sp[z].sy
	edi->sp_y = fpu_reg11;

	fpu_reg11 = ( ebx->sp_x ); //ax = sp[z].sx *sp[z].z;
	fpu_reg11 = fpu_reg11 * ( ebx->sp_z );

	fpu_reg12 = ( esi->sp_x ); //ux = sp[nz].sx*sp[nz].z - ax;
	fpu_reg12 = fpu_reg12 * ( esi->sp_z );
	fpu_reg12 = fpu_reg12 - fpu_reg11;

//r = (x * sp[z].z - ax) / (ux - (sp[nz].z - sp[z].z) * x);
	fpu_reg13 = ( esi->sp_z ); //ux - (sp[nz].z - sp[z].z) * x
	fpu_reg13 = fpu_reg13 - ( ebx->sp_z );
	fpu_reg13 = fpu_reg13 * fpu_reg10; //x
	fpu_reg12 = fpu_reg12 - fpu_reg13;
	fpu_reg13 = ( ebx->sp_z ); //(x * sp[z].z - ax)
	fpu_reg13 = fpu_reg13 * fpu_reg10; //x
	fpu_reg11 = fpu_reg13 - fpu_reg11;
	fpu_reg11 = fpu_reg11 / fpu_reg12; //st = r

	fpu_reg12 = ( esi->sp_z ); //sp[d].z
	fpu_reg12 = fpu_reg12 - ( ebx->sp_z );
	fpu_reg12 = fpu_reg12 * fpu_reg11;
	fpu_reg12 = fpu_reg12 + ( ebx->sp_z );
	edi->sp_z = fpu_reg12;

	ecx = vars_1;
subxclip_l: //sp[d].u
	fpu_reg12 = ( esi->sp_data[ecx] );
	fpu_reg12 = fpu_reg12 - ( ebx->sp_data[ecx] );
	fpu_reg12 = fpu_reg12 * fpu_reg11;
	fpu_reg12 = fpu_reg12 + ( ebx->sp_data[ecx] );
	edi->sp_data[ecx] = fpu_reg12;
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) >= 0) goto subxclip_l;
//r entfernen

	edi = (tscreenpoint *)(((uintptr_t)edi) + spsize); //d++
subxclip_1:
//wz > z?
	if (esi <= ebx) goto subxclip_notz_l;
	ebx = esi; //entspricht ebx += spsize
	goto subxclip_z_l; //z-schleife
subxclip_notz_l:
	ebx = esi; //entspricht ebx += spsize

	ebx = _sp_end; //ebx -> sp
//edi -> sp_end
	eax = (uintptr_t)edi - (uintptr_t)ebx;
	eax = eax >> ( 1 );

	if (eax <= spsize) goto subxclip_w;

	fpu_reg10 = fpu_reg10 + ( (int32_t)xres ); //x += xres
	edx = /*(edx & 0xffffff00) |*/ (uint8_t)(( (int8_t)edx ) - 1); //i-schleife
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


static void polygon(tscreenpoint *_sp, uint32_t sp_end, tmaterial *mat, uint32_t ifl0) {
	realnum fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13;
	uint32_t eax, edx, ecx, ebx, esi, ebp;
	uint8_t *edi, *es2;
	uint32_t *ed2;
	uint32_t stack_var00, stack_var01, stack_var02;
// byte layout : ifl0
//_sp = *sp, zeiger auf screenpoints
//<- drawedflag = 0 : face gezeichnet
	uint32_t pend, y, lb, lc, rb, rc; //int
	uint32_t *x_y;
	float lx, ldx, lu, ldu, lv, ldv; //float
	float rx, rdx, ru, rdu, rv, rdv; //float
	uint32_t xa, xe, txt_x, txt_y, mapand; //int
	int8_t *mapptr;

	mapptr = mat->mt_mapptr;
	mapand = mat->mt_mapand;


	eax = 0; //start- und endpunkt bestimmen
	lb = eax;
	rb = eax;
	pend = eax;

	es2 = (uint8_t *)_sp; //esi -> screenpoints
	fpu_reg10 = ( ((tscreenpoint *)es2)->sp_y ); //st(0) = ymax
	fpu_reg11 = fpu_reg10; //st(1) = ymin

	ecx = sp_end;
	ecx = ecx - ( flatpsize ); //esi+ecx -> sp[sp_end-1]
polygon_max_l:

	fpu_reg12 = ( ((tscreenpoint *)(es2 + ecx))->sp_y );
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

	y = (int32_t)ceil(fpu_reg10); //y = ceil(ymin)

	edx = y;
//y-clipping
	if (( (int32_t)edx ) >= ( (int32_t)yres )) goto polygon_weg;


	if (( (int32_t)edx ) > 0) goto polygon_y0;
	edx = 0;
	y = edx;
polygon_y0:
	edx = ( (int32_t)edx ) * ( (int32_t)xres );
	x_y = edx + (uint32_t *)scene.sc_buffer;

	lc = ( 1 );
	rc = ( 1 );

polygon_y_l: //y-schleife

//links
	lc = ( (int32_t)lc ) - 1;
	if (( (int32_t)lc ) != 0) goto polygon_l_nz;
	edi = (uint8_t *)_sp;
	esi = lb;
polygon_lc:

	if (esi == pend) goto polygon_fertig; //lb == pend -> unten angekommen
	ebx = esi; //ia = lb
	{ uint32_t carry = (esi < ( flatpsize ))?1:0; esi = esi - ( flatpsize ); //lb--
	  if (carry == 0) goto polygon_l0; } //wrap
	esi = esi + sp_end;
polygon_l0:
	fpu_reg10 = ( ((tscreenpoint *)(esi + edi))->sp_y ); //lc = ceil(sp[lb].sy) - y
	lc = (int32_t)ceil(fpu_reg10);
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
	txt_x = (int32_t)ceil(fpu_reg12);
	lu = fpu_reg12;

//ldv = (sp[lb].v - sp[ia].v)/(sp[lb].y - sp[ia].y);
	fpu_reg12 = ( ((tscreenpoint *)(edi + esi))->sp_v );
	fpu_reg12 = fpu_reg12 - ( ((tscreenpoint *)(edi + ebx))->sp_v );
	fpu_reg12 = fpu_reg12 / fpu_reg11;
	ldv = fpu_reg12;

//lv = ldv  *(y  - sp[ia].y) + sp[ia].v;
	fpu_reg12 = fpu_reg12 * fpu_reg10;
	fpu_reg12 = fpu_reg12 + ( ((tscreenpoint *)(edi + ebx))->sp_v );
	txt_y = (int32_t)ceil(fpu_reg12);
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
	txt_x = (int32_t)ceil(fpu_reg10);
	lu = fpu_reg10;

	fpu_reg10 = lv;
	fpu_reg10 = fpu_reg10 + ldv;
	txt_y = (int32_t)ceil(fpu_reg10);
	lv = fpu_reg10;

	fpu_reg10 = lx;
	fpu_reg10 = fpu_reg10 + ldx; //lx bleibt im copro
	lx = fpu_reg10;
polygon_l_z:
//rechts
	rc = ( (int32_t)rc ) - 1;
	if (( (int32_t)rc ) != 0) goto polygon_r_nz;
	edi = (uint8_t *)_sp;
	esi = rb;
polygon_rc:

	if (esi == pend) goto polygon_fertig2; //rb == pend -> unten angekommen
	ebx = esi; //ia = rb
	esi = esi + ( flatpsize ); //rb++

	if (esi < sp_end) goto polygon_r0; //wrap
	esi = 0;
polygon_r0:
	fpu_reg11 = ( ((tscreenpoint *)(esi + edi))->sp_y ); //rc = ceil(sp[rb].sy) - y
	rc = (int32_t)ceil(fpu_reg11);
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
//fist txt_x
	ru = fpu_reg13;

//rdv = (sp[rb].v - sp[ia].v)/(sp[rb].y - sp[ia].y);
	fpu_reg13 = ( ((tscreenpoint *)(edi + esi))->sp_v );
	fpu_reg13 = fpu_reg13 - ( ((tscreenpoint *)(edi + ebx))->sp_v );
	fpu_reg13 = fpu_reg13 / fpu_reg12;
	rdv = fpu_reg13;

//rv = rdv  *(y  - sp[ia].y) + sp[ia].v;
	fpu_reg13 = fpu_reg13 * fpu_reg11;
	fpu_reg13 = fpu_reg13 + ( ((tscreenpoint *)(edi + ebx))->sp_v );
//fist txt_y
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
//fist txt_x
	ru = fpu_reg11;

	fpu_reg11 = rv;
	fpu_reg11 = fpu_reg11 + rdv;
//fist txt_y
	rv = fpu_reg11;

	fpu_reg11 = rx;
	fpu_reg11 = fpu_reg11 + rdx; //rx bleibt im copro
	rx = fpu_reg11;
polygon_r_z:

	xe = (int32_t)ceil(fpu_reg11); //xe = ceil(rx) (rx entfernen)
	xa = (int32_t)ceil(fpu_reg10); //xa = ceil(lx) (lx entfernen)

//eax = iflp
//ebx = x0Yy
//ecx = CCCU
//edx = 00?X
//esi = TTTT
//edi = PPPP
//ebp = u0Vv

	ecx = xe;
	ecx = ( (int32_t)ecx ) - ( (int32_t)xa ); //ecx = CCCC
	if (( (int32_t)ecx ) <= 0) goto polygon_w;
	drawedflag = ( 0 );
	ed2 = xa + x_y; //edi = PPPP

	ebx = txt_x; //ebx = 00Xx
	edx = (uint32_t)( (uint8_t)(ebx >> 8) ); //edx = 00?X
	ebx = ebx << ( 24 ); //ebx = x000
	ebx = (ebx & 0xffff0000) | (uint16_t)(( (uint16_t)txt_y )); //ebx = x0Yy

	es2 = (uint8_t *)scene.sc_divtab;
	fpu_reg10 = ru;
	fpu_reg10 = fpu_reg10 - lu;
	fpu_reg10 = fpu_reg10 * ( ((float *)(es2))[ecx] );
	txt_x = (int32_t)ceil(fpu_reg10);
	fpu_reg10 = rv;
	fpu_reg10 = fpu_reg10 - lv;
	fpu_reg10 = fpu_reg10 * ( ((float *)(es2))[ecx] );
	txt_y = (int32_t)ceil(fpu_reg10);

	ecx = ecx - 1;
	eax = txt_x; //eax = 00Uu
	ecx = ecx << ( 8 ); //ecx = CCC0
	ecx = (ecx & 0xffffff00) | (uint8_t)(( (uint8_t)(eax >> 8) )); //ecx = CCCU
	eax = eax << ( 24 ); //eax = u000
	eax = (eax & 0xffff0000) | (uint16_t)(( (uint16_t)txt_y )); //eax = u0Vv
	es2 = (uint8_t *)mapptr; //esi = TTTT

	stack_var00 = ( 0 /*ebp*/ ); //ebp : lokale variablen
	stack_var01 = mapand;
	stack_var02 = ifl0;
	ebp = eax;
	eax = stack_var02; //eax = ifl0

	edx = set_high_byte(edx, ( (uint8_t)(ebx >> 8) ));
polygon_inner:
	edx = edx & stack_var01;
	eax = (eax & 0xffffff00) | (uint8_t)(( *((uint8_t *)(es2 + edx)) ));
	{ uint32_t carry = (UINT32_MAX - ebx < ebp)?1:0; ebx = ebx + ebp;
	  edx = (edx & 0xffffff00) | (uint8_t)(( (uint8_t)edx ) + ( (uint8_t)ecx ) + carry); }
	*ed2 = eax;
	edx = set_high_byte(edx, ( (uint8_t)(ebx >> 8) ));
	ed2 = ed2 + 1;
	{ uint32_t carry = (ecx < ( 256 ))?1:0; ecx = ecx - ( 256 );
	  if (carry == 0) goto polygon_inner; }

	eax = stack_var01; //mapand entfernen
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


#if 0
//gpolygon proc pascal
//arg     _sp, sp_end, mat, ifl0; byte layout : ifl0
//        ;_sp = *sp, zeiger auf screenpoints
//
//local   pend, y, x_y, lb, lc, rb, rc ;int
//local   lx, ldx, lu, ldu, lv, ldv ;float
//local   rx, rdx, ru, rdu, rv, rdv ;float
//local   xa, xe, txt_x, txt_y, mapptr, mapand ;int
//
//        mov     esi,mat
//        mov     eax,[esi].mt_mapptr
//        mov     mapptr,eax
//        mov     eax,[esi].mt_mapand
//        mov     mapand,eax
//
//
//        xor     eax,eax                 ;start- und endpunkt bestimmen
//        mov     lb,eax
//        mov     rb,eax
//        mov     pend,eax
//
//        mov     esi,_sp                 ;esi -> screenpoints
//        fld     sp_y[esi]               ;st(0) = ymax
//        fld     st                      ;st(1) = ymin
//
//        mov      ecx,sp_end
//        sub      ecx,gouraudpsize       ;esi+ecx -> sp[sp_end-1]
//@@max_l:
//
//        fld     sp_y[esi+ecx]
//        fcom    st(1);ymax              ;gr”áten y-wert finden (endpunkt)
//        fstsw   ax
//        sahf
//        jb      @@max
//        mov     pend,ecx
//        fst     st(1);ymax
//        jmp     @@min
//@@max:
//        fcom    st(2);ymin              ;kleinsten y-wert finden (startpunkt)
//        fstsw   ax
//        sahf
//        ja      @@min
//        mov     lb,ecx
//        mov     rb,ecx
//        fst     st(2);ymin
//@@min:  fstp    st
//
//        sub     ecx,gouraudpsize
//        jnz     @@max_l
//        fstp    st                      ;ymax entfernen
//
//        fistp   y                       ;y = ceil(ymin)
//
//        mov     eax,y
//
//        cmp     eax,yres                ;y-clipping
//        jge     @@weg
//
//        or      eax,eax
//        jg      @@y0
//        mov     y,0
//        xor     eax,eax
//@@y0:
//        imul    eax,xres
//        mov     edx,scene.sc_buffer
//        shr     edx,2
//        add     eax,edx
//        mov     x_y,eax
//
//        mov     lc,1
//        mov     rc,1
//
//@@y_l:  ;y-schleife
//
//                ;links
//        dec     lc
//        jnz     @@l_nz
//        mov     edi,_sp
//        mov     esi,lb
//@@lc:
//        cmp     esi,pend
//        je      @@fertig                ;lb == pend -> unten angekommen
//        mov     ebx,esi                 ;ia = lb
//        sub     esi,gouraudpsize        ;lb--
//        jnc     @@l0                    ;wrap
//        add     esi,sp_end
//@@l0:
//        fld     sp_y[esi+edi]           ;lc = ceil(sp[lb].sy) - y
//        fistp   lc
//        mov     eax,y
//        sub     lc,eax
//        jle     @@lc                    ;while lc <= 0
//        mov     lb,esi
//
//
//        fild    y
//        fsub    sp_y[ebx+edi]           ;(y  - sp[ia].y)
//
//        fld     sp_y[edi+esi]           ;(sp[lb].y - sp[ia].y)
//        fsub    sp_y[edi+ebx]
//
//                ;ldu = (sp[lb].u - sp[ia].u)/(sp[lb].y - sp[ia].y);
//        fld     sp_u[edi+esi]
//        fsub    sp_u[edi+ebx]
//        fdiv    st,st(1)
//        fst     ldu
//
//                ;lu = ldu  *(y  - sp[ia].y) + sp[ia].u;
//        fmul    st,st(2)
//        fadd    sp_u[edi+ebx]
//      fist txt_x
//        fstp    lu
//
//                ;ldv = (sp[lb].v - sp[ia].v)/(sp[lb].y - sp[ia].y);
//        fld     sp_v[edi+esi]
//        fsub    sp_v[edi+ebx]
//        fdiv    st,st(1)
//        fst     ldv
//
//                ;lv = ldv  *(y  - sp[ia].y) + sp[ia].v;
//        fmul    st,st(2)
//        fadd    sp_v[edi+ebx]
//      fist txt_y
//        fstp    lv
//
//                ;ldx = (sp[lb].x - sp[ia].x)/(sp[lb].y - sp[ia].y);
//        fld     sp_x[edi+esi]
//        fsub    sp_x[edi+ebx]
//        fdivrp  st(1),st
//        fst     ldx
//
//                ;lx = ldx  *(y  - sp[ia].y) + sp[ia].x;
//        fmulp   st(1),st
//        fadd    sp_x[edi+ebx]
//        fst     lx
//
//        jmp     @@l_z
//@@l_nz:
//        fld     lu
//        fadd    ldu
//      fist txt_x
//        fstp    lu
//
//        fld     lv
//        fadd    ldv
//      fist txt_y
//        fstp    lv
//
//        fld     lx
//        fadd    ldx                     ;lx bleibt im copro
//        fst     lx
//@@l_z:
//                ;rechts
//        dec     rc
//        jnz     @@r_nz
//        mov     edi,_sp
//        mov     esi,rb
//@@rc:
//        cmp     esi,pend
//        je      @@fertig                ;rb == pend -> unten angekommen
//        mov     ebx,esi                 ;ia = rb
//        add     esi,gouraudpsize        ;rb++
//        cmp     esi,sp_end
//        jb      @@r0                    ;wrap
//        xor     esi,esi
//@@r0:
//        fld     sp_y[esi+edi]           ;rc = ceil(sp[rb].sy) - y
//        fistp   rc
//        mov     eax,y
//        sub     rc,eax
//        jle     @@rc                    ;while lc <= 0
//        mov     rb,esi
//
//
//        fild    y
//        fsub    sp_y[ebx+edi]           ;(y  - sp[ia].y)
//
//        fld     sp_y[edi+esi]           ;(sp[rb].y - sp[ia].y)
//        fsub    sp_y[edi+ebx]
//
//                ;rdu = (sp[rb].u - sp[ia].u)/(sp[rb].y - sp[ia].y);
//        fld     sp_u[edi+esi]
//        fsub    sp_u[edi+ebx]
//        fdiv    st,st(1)
//        fst     rdu
//
//                ;ru = rdu  *(y  - sp[ia].y) + sp[ia].u;
//        fmul    st,st(2)
//        fadd    sp_u[edi+ebx]
//;      fist txt_x
//        fstp    ru
//
//                ;rdv = (sp[rb].v - sp[ia].v)/(sp[rb].y - sp[ia].y);
//        fld     sp_v[edi+esi]
//        fsub    sp_v[edi+ebx]
//        fdiv    st,st(1)
//        fst     rdv
//
//                ;rv = rdv  *(y  - sp[ia].y) + sp[ia].v;
//        fmul    st,st(2)
//        fadd    sp_v[edi+ebx]
//;      fist txt_y
//        fstp    rv
//
//                ;rdx = (sp[rb].x - sp[ia].x)/(sp[rb].y - sp[ia].y);
//        fld     sp_x[edi+esi]
//        fsub    sp_x[edi+ebx]
//        fdivrp  st(1),st
//        fst     rdx
//
//                ;rx = rdx  *(y  - sp[ia].y) + sp[ia].x;
//        fmulp   st(1),st
//        fadd    sp_x[edi+ebx]
//        fst     rx
//
//        jmp     @@r_z
//@@r_nz:
//        fld     ru
//        fadd    rdu
//;      fist txt_x
//        fstp    ru
//
//        fld     rv
//        fadd    rdv
//;      fist txt_y
//        fstp    rv
//
//        fld     rx
//        fadd    rdx                     ;rx bleibt im copro
//        fst     rx
//@@r_z:
//
//        fistp   xe                      ;xe = ceil(rx) (rx entfernen)
//        fistp   xa                      ;xa = ceil(lx) (lx entfernen)
//
//;eax = iflp
//;ebx = x0Yy
//;ecx = CCCU
//;edx = 00?X
//;esi = TTTT
//;edi = PPPP
//;ebp = u0Vv
//
//        mov     edi,xa
//        mov     ecx,xe
//        sub     ecx,edi                 ;ecx = CCCC
//        jle     @@w
//        add     edi,x_y                 ;edi = PPPP
//
//        mov     ebx,txt_x               ;ebx = 00Xx
//        movzx   edx,bh                  ;edx = 00?X
//        shl     ebx,24                  ;ebx = x000
//        mov     bx,word ptr txt_y       ;ebx = x0Yy
//
//        mov     esi,scene.sc_divtab
//        fld     ru
//        fsub    lu
//        fmul    dword ptr[esi+ecx*4]
//        fistp   txt_x
//        fld     rv
//        fsub    lv
//        fmul    dword ptr[esi+ecx*4]
//        fistp   txt_y
//
//        dec     ecx
//        mov     eax,txt_x               ;eax = 00Uu
//        shl     ecx,8                   ;ecx = CCC0
//        mov     cl,ah                   ;ecx = CCCU
//        shl     eax,24                  ;eax = u000
//        mov     ax,word ptr txt_y       ;eax = u0Vv
//
//        mov     esi,mapptr              ;esi = TTTT
//        push    ebp                     ;ebp : lokale variablen
//        push    mapand
//        push    ifl0
//        mov     ebp,eax
//        pop     eax
//
//        mov     dh,bh
//@@inner:
//        and     edx,[esp]
//        add     ebx,ebp
//        mov     al,[esi+edx]
//        adc     dl,cl
//        mov     [edi*4],eax
//        sub     ecx,256
//        mov     dh,bh
//        inc     edi
//        jnc     @@inner
//
//        pop     eax                     ;mapand entfernen
//        pop     ebp
//
//@@w:
//
//        mov     eax,xres
//        add     x_y,eax
//        inc     y
//        mov     eax,yres
//        cmp     eax,y                   ;while (y < yres)
//        jg      @@y_l
//
//@@fertig:
//
//@@weg:  ret
//gpolygon endp
#endif

static void subpolygon(tscreenpoint *_sp, uint32_t sp_end, tmaterial *mat, uint32_t ifl0) { //texture mapping mit scanline subdivision
	realnum fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13, fpu_reg14, fpu_reg15, fpu_reg16;
	uint32_t eax, edx, ecx, ebx, esi, ebp;
	uint8_t *edi, *es2;
	uint32_t *ed2;
	uint32_t stack_var00, stack_var01, stack_var02, stack_var03;
// byte layout : ifl0
//_sp = *sp, zeiger auf screenpoints
//<- drawedflag = 0 : face gezeichnet
	uint32_t pend, y, lb, lc, rb, rc; //int
	uint32_t *x_y;
	float lx, ldx, rx, rdx; //float
	uint32_t xa, xe, txt_x, txt_y, mapand; //int
	int8_t *mapptr;
	tscan la, lu, ra, ru, a, u;

	mapptr = mat->mt_mapptr;
	mapand = mat->mt_mapand;


	eax = 0; //start- und endpunkt bestimmen
	lb = eax;
	rb = eax;
	pend = eax;

	es2 = (uint8_t *)_sp; //esi -> screenpoints
	fpu_reg10 = ( ((tscreenpoint *)es2)->sp_y ); //st(0) = ymax
	fpu_reg11 = fpu_reg10; //st(1) = ymin

	ecx = sp_end;
	ecx = ecx - ( flatpsize ); //esi+ecx -> sp[sp_end-1]
subpolygon_max_l:

	fpu_reg12 = ( ((tscreenpoint *)(es2 + ecx))->sp_y );
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

	y = (int32_t)ceil(fpu_reg10); //y = ceil(ymin)

	eax = y;

//y-clipping
	if (( (int32_t)eax ) >= ( (int32_t)yres )) goto subpolygon_weg;


	if (( (int32_t)eax ) > 0) goto subpolygon_y0;
	eax = 0;
	y = eax;
subpolygon_y0:
	eax = ( (int32_t)eax ) * ( (int32_t)xres );
//  dec     eax
	x_y = eax + (uint32_t *)scene.sc_buffer;

	lc = ( 1 );
	rc = ( 1 );

	fpu_reg10 = c_16; //st(0) = 16

subpolygon_y_l: //y-schleife

//links
	lc = ( (int32_t)lc ) - 1;
	if (( (int32_t)lc ) != 0) goto subpolygon_l_nz;
	edi = (uint8_t *)_sp;
	esi = lb;
subpolygon_lc:

	if (esi == pend) goto subpolygon_fertig; //lb == pend -> unten angekommen
	ebx = esi; //ia = lb
	{ uint32_t carry = (esi < ( flatpsize ))?1:0; esi = esi - ( flatpsize ); //lb--
	  if (carry == 0) goto subpolygon_l0; } //wrap
	esi = esi + sp_end;
subpolygon_l0:
	fpu_reg11 = ( ((tscreenpoint *)(esi + edi))->sp_y ); //lc = ceil(sp[lb].sy) - y
	lc = (int32_t)ceil(fpu_reg11);
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
	edi = (uint8_t *)_sp;
	esi = rb;
subpolygon_rc:


	if (esi == pend) goto subpolygon_fertig2; //rb == pend -> unten angekommen
	ebx = esi; //ia = rb
	esi = esi + ( flatpsize ); //rb++

	if (esi < sp_end) goto subpolygon_r0; //wrap
	esi = 0;
subpolygon_r0:
	fpu_reg12 = ( ((tscreenpoint *)(esi + edi))->sp_y ); //rc = ceil(sp[rb].sy) - y
	rc = (int32_t)ceil(fpu_reg12);
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
	txt_x = (int32_t)ceil(fpu_reg14); //textur-startkoordinate

	fpu_reg13 = fpu_reg13 * lu.s_v; //*r               ;a.v = la.v + r*lu.v;
	fpu_reg13 = fpu_reg13 + la.s_v;
	a.s_v = fpu_reg13;
	txt_y = (int32_t)ceil(fpu_reg13); //textur-startkoordinate

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

	xe = (int32_t)ceil(fpu_reg12); //xe = ceil(rx) (rx entfernen)
	fpu_reg11 = ceil(fpu_reg11);
	xa = (int32_t)ceil(fpu_reg11); //xa = ceil(lx) (lx nicht entfernen)


	ecx = xe;
	ecx = ecx - xa; //ecx = CCCC
	ed2 = xa + x_y; //edi = PPPP

subpolygon_l:

	if (( (int32_t)ecx ) >= ( 24 )) goto subpolygon_div;

//weniger als 24 pixel

	if (( (int32_t)ecx ) <= 0) goto subpolygon_w;
	drawedflag = ( 0 );
	ebx = txt_x; //ebx = 00Xx
	edx = (uint32_t)( (uint8_t)(ebx >> 8) ); //edx = 00?X
	ebx = ebx << ( 24 ); //ebx = x000
	ebx = (ebx & 0xffff0000) | (uint16_t)(( (uint16_t)txt_y )); //ebx = x0Yy

	eax = 0;
	stack_var00 = eax; //push pixel counter (= 0)

	es2 = (uint8_t *)scene.sc_divtab;
	fpu_reg12 = a.s_u;
	fpu_reg12 = fpu_reg12 + u.s_u; //a.u + u.u = texutur - endkoordinate
	fpu_reg12 = fpu_reg12 - ( (int32_t)txt_x );
	fpu_reg12 = fpu_reg12 * ( ((float *)(es2))[ecx] );
	txt_x = (int32_t)ceil(fpu_reg12);

	fpu_reg12 = a.s_v;
	fpu_reg12 = fpu_reg12 + u.s_v; //a.v + u.v = texutur - endkoordinate
	fpu_reg12 = fpu_reg12 - ( (int32_t)txt_y );
	fpu_reg12 = fpu_reg12 * ( ((float *)(es2))[ecx] );
	txt_y = (int32_t)ceil(fpu_reg12);

	eax = txt_x; //eax = 00Uu
	ecx = set_high_byte(ecx, ( (uint8_t)(eax >> 8) )); //ecx = 00UC
	eax = eax << ( 24 ); //eax = u000
	eax = (eax & 0xffff0000) | (uint16_t)(( (uint16_t)txt_y )); //eax = u0Vv

	goto subpolygon_5; //st(0) = x ;st(1) = 16
subpolygon_div: //scanline subdivision
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
	txt_x = (int32_t)ceil(fpu_reg13);

	fpu_reg12 = fpu_reg12 * u.s_v;
	fpu_reg12 = fpu_reg12 + a.s_v; //a.v + r*u.v
	txt_y = (int32_t)ceil(fpu_reg12);


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
	es2 = (uint8_t *)mapptr; //esi = TTTT
	stack_var01 = ( 0 /*ebp*/ ); //ebp : lokale variablen
	stack_var02 = mapand;
	stack_var03 = ifl0;
	ebp = eax;
	eax = stack_var03;

	edx = set_high_byte(edx, ( (uint8_t)(ebx >> 8) ));
subpolygon_inner:
	edx = edx & stack_var02;
	eax = (eax & 0xffffff00) | (uint8_t)(( *((uint8_t *)(es2 + edx)) ));
	{ uint32_t carry = (UINT32_MAX - ebx < ebp)?1:0; ebx = ebx + ebp;
	  edx = (edx & 0xffffff00) | (uint8_t)(( (uint8_t)edx ) + ( (uint8_t)(ecx >> 8) ) + carry); }
	*ed2 = eax;
	edx = set_high_byte(edx, ( (uint8_t)(ebx >> 8) ));
	ed2 = ed2 + 1;
	ecx = (ecx & 0xffffff00) | (uint8_t)(( (int8_t)ecx ) - 1);
	if (( (int8_t)ecx ) != 0) goto subpolygon_inner;

	eax = stack_var02; //mapand entfernen
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


#if 0
//subgpolygon proc pascal ;texture mapping mit scanline subdivision
//arg     _sp, sp_end, mat, ifl0; byte layout : ifl0
//        ;_sp = *sp, zeiger auf screenpoints
//
//local   pend, y, x_y, lb, lc, rb, rc ;int
//local   lx, ldx, rx, rdx ;float
//local   xa, xe, txt_x, txt_y, mapptr, mapand ;int
//local   la:dword:4, lu:dword:4, ra:dword:4, ru:dword:4, a:dword:4, u:dword:4
//
//        mov     esi,mat
//        mov     eax,[esi].mt_mapptr
//        mov     mapptr,eax
//        mov     eax,[esi].mt_mapand
//        mov     mapand,eax
//
//
//        xor     eax,eax                 ;start- und endpunkt bestimmen
//        mov     lb,eax
//        mov     rb,eax
//        mov     pend,eax
//
//        mov     esi,_sp                 ;esi -> screenpoints
//        fld     sp_y[esi]               ;st(0) = ymax
//        fld     st                      ;st(1) = ymin
//
//        mov      ecx,sp_end
//        sub      ecx,gouraudpsize       ;esi+ecx -> sp[sp_end-1]
//@@max_l:
//
//        fld     sp_y[esi+ecx]
//        fcom    st(1);ymax              ;gr”áten y-wert finden (endpunkt)
//        fstsw   ax
//        sahf
//        jb      @@max
//        mov     pend,ecx
//        fst     st(1);ymax
//        jmp     @@min
//@@max:
//        fcom    st(2);ymin              ;kleinsten y-wert finden (startpunkt)
//        fstsw   ax
//        sahf
//        ja      @@min
//        mov     lb,ecx
//        mov     rb,ecx
//        fst     st(2);ymin
//@@min:  fstp    st
//
//        sub     ecx,gouraudpsize
//        jnz     @@max_l
//        fstp    st                      ;ymax entfernen
//
//        fistp   y                       ;y = ceil(ymin)
//
//        mov     eax,y
//
//        cmp     eax,yres                ;y-clipping
//        jge     @@weg
//
//        or      eax,eax
//        jg      @@y0
//        xor     eax,eax
//        mov     y,eax
//@@y0:
//        imul    eax,xres
//    ;  dec     eax
//        mov     edx,scene.sc_buffer
//        shr     edx,2
//        add     eax,edx
//        mov     x_y,eax
//
//        mov     lc,1
//        mov     rc,1
//
//        fld     c_16                    ;st(0) = 16
//
//@@y_l:  ;y-schleife
//
//                ;links
//        dec     lc
//        jnz     @@l_nz
//        mov     edi,_sp
//        mov     esi,lb
//@@lc:
//        cmp     esi,pend
//        je      @@fertig                ;lb == pend -> unten angekommen
//        mov     ebx,esi                 ;ia = lb
//        sub     esi,gouraudpsize        ;lb--
//        jnc     @@l0                    ;wrap
//        add     esi,sp_end
//@@l0:
//        fld     sp_y[esi+edi]          ;lc = ceil(sp[lb].sy) - y
//        fistp   lc
//        mov     eax,y
//        sub     lc,eax
//        jle     @@lc                    ;while lc <= 0
//        mov     lb,esi
//
//        fld     sp_z[esi+edi]           ;sp[lb].z einlagern
//
//        ;la
//        fld     sp_z[ebx+edi]           ;la.z = sp[ia].z;
//        fst     la.s_z
//
//        fld     sp_y[ebx+edi]          ;la.y = sp[ia].sy*sp[ia].z;
//        fmul    st,st(1)
//        fst     la.s_p;la.y
//
//        fld     sp_u[ebx+edi]           ;la.u = sp[ia].u;
//        fst     la.s_u
//
//        fld     sp_v[ebx+edi]           ;la.v = sp[ia].v;
//        fst     la.s_v
//
//        ;lu
//        fsubr   sp_v[esi+edi]           ;lu.v = sp[lb].v - sp[ia].v;
//        fstp    lu.s_v
//
//        fsubr   sp_u[esi+edi]           ;lu.u = sp[lb].u - sp[ia].u;
//        fstp    lu.s_u
//
//        fld     sp_y[esi+edi]          ;lu.y = sp[lb].sy*sp[lb].z - la.y;
//        fmul    st,st(3);sp[lb].z
//        fsubrp  st(1),st
//        fstp    lu.s_p;lu.y
//        fsubp   st(1),st                ;lu.z = sp[lb].z - sp[ia].z;
//        fstp    lu.s_z
//
//
//                ;ldx = (sp[lb].sx - sp[ia].sx)/(sp[lb].sy - sp[ia].sy);
//        fld     sp_x[esi+edi]
//        fsub    sp_x[ebx+edi]
//        fld     sp_y[esi+edi]
//        fsub    sp_y[ebx+edi]
//        fdivp   st(1),st
//        fst     ldx
//
//                ;lx = ldx   *(y  - sp[ia].sy) + sp[ia].sx;
//        fild    y
//        fsub    sp_y[ebx+edi]
//        fmulp   st(1),st
//        fadd    sp_x[ebx+edi]
//        fst     lx
//        jmp     @@l_z
//@@l_nz:
//        fld     lx
//        fadd    ldx                     ;lx bleibt im copro
//        fst     lx
//@@l_z:
//                ;rechts
//        dec     rc
//        jnz     @@r_nz
//        mov     edi,_sp
//        mov     esi,rb
//@@rc:
//
//        cmp     esi,pend
//        je      @@fertig                ;rb == pend -> unten angekommen
//        mov     ebx,esi                 ;ia = rb
//        add     esi,gouraudpsize        ;rb++
//        cmp     esi,sp_end
//        jb      @@r0                    ;wrap
//        xor     esi,esi
//@@r0:
//        fld     sp_y[esi+edi]          ;rc = ceil(sp[rb].sy) - y
//        fistp   rc
//        mov     eax,y
//        sub     rc,eax
//        jle     @@rc                    ;while rc <= 0
//        mov     rb,esi
//
//        fld     sp_z[esi+edi]           ;sp[lb].z einlagern
//
//        ;ra
//        fld     sp_z[ebx+edi]           ;ra.z = sp[ia].z;
//        fst     ra.s_z
//
//        fld     sp_y[ebx+edi]          ;ra.y = sp[ia].sy*sp[ia].z;
//        fmul    st,st(1)
//        fst     ra.s_p;ra.y
//
//        fld     sp_u[ebx+edi]           ;ra.u = sp[ia].u;
//        fst     ra.s_u
//
//        fld     sp_v[ebx+edi]           ;ra.v = sp[ia].v;
//        fst     ra.s_v
//
//        ;ru
//        fsubr   sp_v[esi+edi]           ;ru.v = sp[rb].v - sp[ia].v;
//        fstp    ru.s_v
//
//        fsubr   sp_u[esi+edi]           ;ru.u = sp[rb].u - sp[ia].u;
//        fstp    ru.s_u
//
//        fld     sp_y[esi+edi]          ;ru.y = sp[rb].sy*sp[rb].z - ra.y;
//        fmul    st,st(3);sp[lb].z
//        fsubrp  st(1),st
//        fstp    ru.s_p;ru.y
//
//        fsubp   st(1),st                ;ru.z = sp[rb].z - sp[ia].z;
//        fstp    ru.s_z
//
//
//                ;rdx = (sp[rb].sx - sp[ia].sx)/(sp[rb].sy - sp[ia].sy);
//        fld     sp_x[esi+edi]
//        fsub    sp_x[ebx+edi]
//        fld     sp_y[esi+edi]
//        fsub    sp_y[ebx+edi]
//        fdivp   st(1),st
//        fst     rdx
//
//                ;rx = rdx   *(y  - sp[ia].sy) + sp[ia].sx;
//        fild    y
//        fsub    sp_y[ebx+edi]
//        fmulp   st(1),st
//        fadd    sp_x[ebx+edi]
//        fst     rx
//        jmp     @@r_z
//@@r_nz:
//        fld     rx
//        fadd    rdx                     ;rx bleibt im copro
//        fst     rx
//@@r_z:
//
//                                        ;st(0) = rx,  st(1) = lx
//                                        ;st(2) = 0.5, st(3) = 16
//
//                ;links: r = (y *la.z - la.y) / (lu.y - lu.z*y );
//        fild    y
//        fmul    la.s_z
//        fsub    la.s_p;-la.y
//        fild    y
//        fmul    lu.s_z
//        fsubr   lu.s_p;lu.y-
//        fdivp   st(1),st                ;st = r
//
//        fld     lu.s_z                  ;a.z = la.z + r*lu.z;
//        fmul    st,st(1);*r
//        fadd    la.s_z
//        fst     a.s_z
//        fmul    st,st(3);*lx            ;a.x = a.z*lx;
//        fstp    a.s_p;a.x
//
//        fld     lu.s_u                  ;a.u = la.u + r*lu.u;
//        fmul    st,st(1);*r
//        fadd    la.s_u
//        fst     a.s_u
//        fistp   txt_x                   ;textur-startkoordinate
//
//        fmul    lu.s_v;*r               ;a.v = la.v + r*lu.v;
//        fadd    la.s_v
//        fst     a.s_v
//        fistp   txt_y                   ;textur-startkoordinate
//
//                ;rechts: r = (y *ra.z - ra.y) / (ru.y - ru.z*y );
//        fild    y
//        fmul    ra.s_z
//        fsub    ra.s_p;ra.y
//        fild    y
//        fmul    ru.s_z
//        fsubr   ru.s_p;ru.y-
//        fdivp   st(1),st                ;st = r
//
//        fld     ru.s_z                  ;u.z = ra.z + r*ru.z;
//        fmul    st,st(1)
//        fadd    ra.s_z
//
//        fld     st                      ;u.x = u.z*rx - a.x;
//        fmul    st,st(3);*rx
//        fsub    a.s_p;-a.x
//        fstp    u.s_p;u.x
//
//        fsub    a.s_z                   ;u.z -= a.z;
//        fstp    u.s_z
//
//        fld     ru.s_u                  ;u.u = ra.u + r*ru.u - a.u;
//        fmul    st,st(1);*r
//        fadd    ra.s_u
//        fsub    a.s_u
//        fstp    u.s_u
//
//        fmul    ru.s_v;*r               ;u.v = ra.v + r*ru.v - a.v;
//        fadd    ra.s_v
//        fsub    a.s_v
//        fstp    u.s_v
//
//        fistp   xe                      ;xe = ceil(rx) (rx entfernen)
//        frndint
//        fist    xa                      ;xa = ceil(lx) (lx nicht entfernen)
//
//
//        mov     edi,xa
//        mov     ecx,xe
//        sub     ecx,edi                 ;ecx = CCCC
//        add     edi,x_y                 ;edi = PPPP
//
//@@l:
//        cmp     ecx,24
//        jge     @@div
//
//                ;weniger als 24 pixel
//        or      ecx,ecx
//        jle     @@w
//
//        mov     ebx,txt_x               ;ebx = 00Xx
//        movzx   edx,bh                  ;edx = 00?X
//        shl     ebx,24                  ;ebx = x000
//        mov     bx,word ptr txt_y       ;ebx = x0Yy
//
//        xor     eax,eax
//        push    eax                     ;push pixel counter (= 0)
//
//        mov     esi,scene.sc_divtab
//        fld     a.s_u
//        fadd    u.s_u                   ;a.u + u.u = texutur - endkoordinate
//        fisub   txt_x
//        fmul    dword ptr [esi+ecx*4]
//        fistp   txt_x
//
//        fld     a.s_v
//        fadd    u.s_v                   ;a.v + u.v = texutur - endkoordinate
//        fisub   txt_y
//        fmul    dword ptr [esi+ecx*4]
//        fistp   txt_y
//
//        mov     eax,txt_x               ;eax = 00Uu
//        mov     ch,ah                   ;ecx = 00UC
//        shl     eax,24                  ;eax = u000
//        mov     ax,word ptr txt_y       ;eax = u0Vv
//
//        jmp     @@5
//                                        ;st(0) = x
//                                        ;st(1) = 16
//@@div:          ;scanline subdivision
//        mov     edx,txt_x               ;edx = 00Xx
//        mov     ebx,txt_y               ;ebx = 00Yy
//
//        sub     ecx,16
//        push    ecx                     ;push pixel counter
//
//        fadd    st,st(1);16             ;x um 16 weiter
//        fld     st;x                    ;r = (x *a.z - a.x) / (u.x - u.z*x )
//        fmul    a.s_z
//        fsub    a.s_p;a.x
//        fld     st(1);x
//        fmul    u.s_z
//        fsubr   u.s_p;u.x
//        fdivp   st(1),st                ;st = r
//
//        fld     st
//
//        fmul    u.s_u
//        fadd    a.s_u                   ;a.u + r*u.u
//        fistp   txt_x
//
//        fmul    u.s_v
//        fadd    a.s_v                   ;a.v + r*u.v
//        fistp   txt_y
//
//
//        mov     eax,txt_x
//        mov     esi,txt_y
//        sub     eax,edx                 ;eax = 00Uu
//        sub     esi,ebx                 ;esi = 00Vv
//
//        shr     eax,4
//        shr     esi,4
//
//        mov     ch,ah                   ;ecx = 00U0
//        shl     eax,24                  ;eax = u000
//        mov     cl,16                   ;ecx = 00UC
//        mov     ax,si                   ;eax = u0Vv
//
//        shl     ebx,8                   ;ebx = 0Yy0
//        shrd    ebx,edx,8               ;ebx = x0Yy
//        movzx   edx,dh                  ;edx = 00?X
//@@5:
//        mov     esi,mapptr              ;esi = TTTT
//        push    ebp                     ;ebp : lokale variablen
//        push    mapand
//        push    ifl0
//        mov     ebp,eax
//        pop     eax
//
//        mov     dh,bh
//@@inner:
//        and     edx,[esp]
//        add     ebx,ebp
//        mov     al,[esi+edx]
//        adc     dl,ch
//        mov     [edi*4],eax
//        mov     dh,bh
//        inc     edi
//        dec     cl
//        jnz     @@inner
//
//        pop     eax                     ;mapand entfernen
//        pop     ebp
//        pop     ecx
//        jmp     @@l
//@@w:
//        fstp    st                      ;x entfernen
//
//        mov     eax,xres
//        add     x_y,eax
//        inc     y
//        mov     eax,yres
//        cmp     eax,y                   ;while (y < yres)
//        jg      @@y_l
//
//@@fertig:
//        fstp    st                      ;16 entfernen
//
//@@weg:  ret
//subgpolygon endp
#endif

static void wpolygon(tscreenpoint *_sp, uint32_t sp_end) {
	realnum fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13;
	uint32_t eax, edx, ecx, ebx, esi, ebp;
	uint8_t *edi, *es2;
	uint32_t *ed2;
	uint32_t stack_var00;

//_sp = *sp, zeiger auf screenpoints
//<- eax = -1 : face nicht gezeichnet
	uint32_t pend, y, lb, lc, rb, rc; //int
	uint32_t *x_y;
	float lx, ldx, lu, ldu, lv, ldv; //float
	float rx, rdx, ru, rdu, rv, rdv; //float
	uint32_t xa, xe, txt_x, txt_y; //int


	eax = 0; //start- und endpunkt bestimmen
	lb = eax;
	rb = eax;
	pend = eax;

	es2 = (uint8_t *)_sp; //esi -> screenpoints
	fpu_reg10 = ( ((tscreenpoint *)es2)->sp_y ); //st(0) = ymax
	fpu_reg11 = fpu_reg10; //st(1) = ymin

	ecx = sp_end;
	ecx = ecx - ( flatpsize ); //esi+ecx -> sp[sp_end-1]
wpolygon_max_l:

	fpu_reg12 = ( ((tscreenpoint *)(es2 + ecx))->sp_y );
//ymax              ;gr”áten y-wert finden (endpunkt)


	if (fpu_reg12 < fpu_reg11) goto wpolygon_max;
	pend = ecx;
	fpu_reg11 = fpu_reg12; //ymax
	goto wpolygon_min;
wpolygon_max:
//ymin              ;kleinsten y-wert finden (startpunkt)


	if (fpu_reg12 > fpu_reg10) goto wpolygon_min;
	lb = ecx;
	rb = ecx;
	fpu_reg10 = fpu_reg12; //ymin
wpolygon_min:

	ecx = ( (int32_t)ecx ) - ( flatpsize );
	if (( (int32_t)ecx ) != 0) goto wpolygon_max_l;
//ymax entfernen

	y = (int32_t)ceil(fpu_reg10); //y = ceil(ymin)

	edx = y;
//y-clipping
	if (( (int32_t)edx ) >= ( (int32_t)yres )) goto wpolygon_weg;


	if (( (int32_t)edx ) > 0) goto wpolygon_y0;
	edx = 0;
	y = edx;
wpolygon_y0:
	edx = ( (int32_t)edx ) * ( (int32_t)xres );
	x_y = edx + (uint32_t *)scene.sc_buffer;

	lc = ( 1 );
	rc = ( 1 );

wpolygon_y_l: //y-schleife

//links
	lc = ( (int32_t)lc ) - 1;
	if (( (int32_t)lc ) != 0) goto wpolygon_l_nz;
	edi = (uint8_t *)_sp;
	esi = lb;
wpolygon_lc:

	if (esi == pend) goto wpolygon_fertig; //lb == pend -> unten angekommen
	ebx = esi; //ia = lb
	{ uint32_t carry = (esi < ( flatpsize ))?1:0; esi = esi - ( flatpsize ); //lb--
	  if (carry == 0) goto wpolygon_l0; } //wrap
	esi = esi + sp_end;
wpolygon_l0:
	fpu_reg10 = ( ((tscreenpoint *)(esi + edi))->sp_y ); //lc = ceil(sp[lb].sy) - y
	lc = (int32_t)ceil(fpu_reg10);
	eax = y;
	lc = ( (int32_t)lc ) - ( (int32_t)eax );
	if (( (int32_t)lc ) <= 0) goto wpolygon_lc; //while lc <= 0
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
	txt_x = (int32_t)ceil(fpu_reg12);
	lu = fpu_reg12;

//ldv = (sp[lb].v - sp[ia].v)/(sp[lb].y - sp[ia].y);
	fpu_reg12 = ( ((tscreenpoint *)(edi + esi))->sp_v );
	fpu_reg12 = fpu_reg12 - ( ((tscreenpoint *)(edi + ebx))->sp_v );
	fpu_reg12 = fpu_reg12 / fpu_reg11;
	ldv = fpu_reg12;

//lv = ldv  *(y  - sp[ia].y) + sp[ia].v;
	fpu_reg12 = fpu_reg12 * fpu_reg10;
	fpu_reg12 = fpu_reg12 + ( ((tscreenpoint *)(edi + ebx))->sp_v );
	txt_y = (int32_t)ceil(fpu_reg12);
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

	goto wpolygon_l_z;
wpolygon_l_nz:
	fpu_reg10 = lu;
	fpu_reg10 = fpu_reg10 + ldu;
	txt_x = (int32_t)ceil(fpu_reg10);
	lu = fpu_reg10;

	fpu_reg10 = lv;
	fpu_reg10 = fpu_reg10 + ldv;
	txt_y = (int32_t)ceil(fpu_reg10);
	lv = fpu_reg10;

	fpu_reg10 = lx;
	fpu_reg10 = fpu_reg10 + ldx; //lx bleibt im copro
	lx = fpu_reg10;
wpolygon_l_z:
//rechts
	rc = ( (int32_t)rc ) - 1;
	if (( (int32_t)rc ) != 0) goto wpolygon_r_nz;
	edi = (uint8_t *)_sp;
	esi = rb;
wpolygon_rc:

	if (esi == pend) goto wpolygon_fertig2; //rb == pend -> unten angekommen
	ebx = esi; //ia = rb
	esi = esi + ( flatpsize ); //rb++

	if (esi < sp_end) goto wpolygon_r0; //wrap
	esi = 0;
wpolygon_r0:
	fpu_reg11 = ( ((tscreenpoint *)(esi + edi))->sp_y ); //rc = ceil(sp[rb].sy) - y
	rc = (int32_t)ceil(fpu_reg11);
	eax = y;
	rc = ( (int32_t)rc ) - ( (int32_t)eax );
	if (( (int32_t)rc ) <= 0) goto wpolygon_rc; //while lc <= 0
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

	goto wpolygon_r_z;
wpolygon_r_nz:
	fpu_reg11 = ru;
	fpu_reg11 = fpu_reg11 + rdu;
	ru = fpu_reg11;

	fpu_reg11 = rv;
	fpu_reg11 = fpu_reg11 + rdv;
	rv = fpu_reg11;

	fpu_reg11 = rx;
	fpu_reg11 = fpu_reg11 + rdx; //rx bleibt im copro
	rx = fpu_reg11;
wpolygon_r_z:

	xe = (int32_t)ceil(fpu_reg11); //xe = ceil(rx) (rx entfernen)
	xa = (int32_t)ceil(fpu_reg10); //xa = ceil(lx) (lx entfernen)

//eax = 00Up
//ebx = x0Yy
//ecx = CCCC
//edx = 00?X
//esi = TTTT
//edi = PPPP
//ebp = u0Vv

	ecx = xe;
	ecx = ( (int32_t)ecx ) - ( (int32_t)xa ); //ecx = CCCC
	if (( (int32_t)ecx ) <= 0) goto wpolygon_w;
	ed2 = xa + x_y; //edi = PPPP

	ebx = txt_x; //ebx = 00Xx
	edx = (uint32_t)( (uint8_t)(ebx >> 8) ); //edx = 00?X
	ebx = ebx << ( 24 ); //ebx = x000
	ebx = (ebx & 0xffff0000) | (uint16_t)(( (uint16_t)txt_y )); //ebx = x0Yy

	es2 = (uint8_t *)scene.sc_divtab;
	fpu_reg10 = ru;
	fpu_reg10 = fpu_reg10 - lu;
	fpu_reg10 = fpu_reg10 * ( ((float *)(es2))[ecx] );
	txt_x = (int32_t)ceil(fpu_reg10);
	fpu_reg10 = rv;
	fpu_reg10 = fpu_reg10 - lv;
	fpu_reg10 = fpu_reg10 * ( ((float *)(es2))[ecx] );
	txt_y = (int32_t)ceil(fpu_reg10);

	eax = txt_x; //eax = 00Uu
	esi = eax;
	esi = esi << ( 24 ); //esi = u000
	esi = (esi & 0xffff0000) | (uint16_t)(( (uint16_t)txt_y )); //esi = u0Vv

	stack_var00 = ( 0 /*ebp*/ ); //ebp : lokale variablen
	ebp = esi;

	esi = frame;
	esi = esi << ( 16 - 6 );
	esi = esi & ( 0x70000 );
	es2 = esi + (uint8_t *)scene.sc_water; //esi = TTTT

	edx = set_high_byte(edx, ( (uint8_t)(ebx >> 8) ));
wpolygon_inner:
	eax = (eax & 0xffffff00) | (uint8_t)(( *((uint8_t *)(es2 + edx)) ));
	{ uint32_t carry = (UINT32_MAX - ebx < ebp)?1:0; ebx = ebx + ebp;
	  edx = (edx & 0xffffff00) | (uint8_t)(( (uint8_t)edx ) + ( (uint8_t)(eax >> 8) ) + carry); }
	edx = set_high_byte(edx, ( (uint8_t)(ebx >> 8) ));
//add     [edi*4+2],al
	((uint8_t *)ed2)[2] = (uint8_t) (( (uint8_t)eax ));
	ed2 = ed2 + 1;
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto wpolygon_inner;

	ebp = stack_var00;
wpolygon_w:
	eax = xres;
	x_y = x_y + eax;
	y = y + 1;
	eax = yres;
//while (y < yres)
	if (( (int32_t)eax ) > ( (int32_t)y )) goto wpolygon_y_l;

	goto wpolygon_fertig;
wpolygon_fertig2:

wpolygon_fertig:
wpolygon_weg:
	return;
}



static uint32_t cubezclip(tcpoint *eax, void *&_ebx, ttempcube *esi, tscreenpoint *&_edi) {
	realnum fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13;
	uint32_t ea2, ecx;
	tprojected *edx, *ec2;
	tcpoint *ebx = (tcpoint *)_ebx;
	uint8_t *eb2;
	tscreenpoint *edi = _edi;
//arg     spsize, vars
//-> ebx -> sp     (source points of type tcpoint)
//-> eax -> sp_end
//-> esi -> ttempcube
//-> edi -> dp     (dest points of type tscreenpoint)

//<- ebx -> dp
//<- esi -> ttempcube
//<- edi -> dp_end
//<- flags: if less than 3 points, jump with jbe
	tcpoint *_sp, *_sp_end;
	tscreenpoint *_dp;

	_sp = ebx;
	_sp_end = eax;
	_dp = edi;

	fpu_reg10 = 1.0;
cubezclip_z_l:
//inn = true : point in front of plane
	edx = (tprojected *)( ((uintptr_t)&(esi->tc_proj[0])) + ebx->cp_p ); //edx -> tprojected (this point)
//if (inn)
//inn = (z >= 1.0)
	if ((( edx->pr_inn ) & ( 1 )) == 0) goto cubezclip_0;

//point in front of projection plane (visible)
	fpu_reg11 = ( edx->pr_sx );
	fpu_reg11 = fpu_reg11 + fpu_reg10;
	fpu_reg11 = fpu_reg11 * xmid;
	edi->sp_x = fpu_reg11; //x = xres/2 *(1 + x/z)

	fpu_reg11 = ( edx->pr_sy );
	fpu_reg11 = fpu_reg10 - fpu_reg11;
	fpu_reg11 = fpu_reg11 * ymid;
	edi->sp_y = fpu_reg11; //y = yres/2 *(1 - y/z)

	fpu_reg11 = ( edx->pr_z );
	edi->sp_z = fpu_reg11;

	ecx = ( flatvars ); //gouraudvars                ;data (mapping etc.)
cubezclip_l0:
	edi->sp_data[ecx] = ebx->cp_data[ecx];
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) >= 0) goto cubezclip_l0;

	edi = (tscreenpoint *)( ((uintptr_t)edi) + flatpsize ); //gouraudpsize        ;one destination point added
//end of if statement
cubezclip_0:
	eax = ebx; //ebx -> actual source point
	eax = eax + ( 1 ); //eax -> next source point
//wrap if at last point
	if (eax < _sp_end) goto cubezclip_wrap;
	eax = _sp;
cubezclip_wrap:

	ec2 = (tprojected *)( ((uintptr_t)&(esi->tc_proj[0])) + eax->cp_p ); //ecx -> tprojected (next point)
//if (inn ^ (nz >= 1.0))
	ea2 = /*(ea2 & 0xffffff00) |*/ (uint8_t)(( edx->pr_inn ));
	ea2 = /*(ea2 & 0xffffff00) |*/ (uint8_t)(( (uint8_t)ea2 ) ^ ( ec2->pr_inn ));
	if (( (int8_t)ea2 ) != 0) goto cubezclip_not1;
	goto cubezclip_1;
cubezclip_not1:

//this or next point behind projection plane
//r = (1.0-z)/(nz-z)
	fpu_reg11 = 1.0;
	edi->sp_z = fpu_reg11; //z = 1.0
	fpu_reg11 = fpu_reg11 - ( edx->pr_z );
	fpu_reg12 = ( ec2->pr_z );
	fpu_reg12 = fpu_reg12 - ( edx->pr_z );
	fpu_reg11 = fpu_reg11 / fpu_reg12; //st = r

	fpu_reg12 = ( edx->pr_x ); //x' = x + r*(nx-x)
	fpu_reg13 = ( ec2->pr_x );
	fpu_reg13 = fpu_reg13 - fpu_reg12;
	fpu_reg13 = fpu_reg13 * fpu_reg11;
	fpu_reg12 = fpu_reg12 + fpu_reg13;
	fpu_reg12 = fpu_reg12 + fpu_reg10; //x = xres/2 *(1 + x')
	fpu_reg12 = fpu_reg12 * xmid;
	edi->sp_x = fpu_reg12;

	fpu_reg12 = ( edx->pr_y ); //y' = y + r*(ny-y)
	fpu_reg13 = ( ec2->pr_y );
	fpu_reg13 = fpu_reg13 - fpu_reg12;
	fpu_reg13 = fpu_reg13 * fpu_reg11;
	fpu_reg12 = fpu_reg12 + fpu_reg13;
	fpu_reg12 = fpu_reg10 - fpu_reg12; //y = yres/2 *(1 - y')
	fpu_reg12 = fpu_reg12 * ymid;
	edi->sp_y = fpu_reg12;

	ecx = ( flatvars ); //gouraudvars                ;data (mapping etc.)
cubezclip_l1:
	fpu_reg12 = ( ebx->cp_data[ecx] );
	fpu_reg13 = ( eax->cp_data[ecx] );
	fpu_reg13 = fpu_reg13 - fpu_reg12;
	fpu_reg13 = fpu_reg13 * fpu_reg11;
	fpu_reg12 = fpu_reg12 + fpu_reg13;
	edi->sp_data[ecx] = fpu_reg12;
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) >= 0) goto cubezclip_l1;

//remove r
	edi = (tscreenpoint *)( ((uintptr_t)edi) + flatpsize ); //gouraudpsize        ;one destination point added
//end of if statement
cubezclip_1:
//ns > s?
	if (eax <= ebx) goto cubezclip_notz_l;
	ebx = eax; //is same as ebx += point size
	goto cubezclip_z_l; //repeat loop
cubezclip_notz_l:
	ebx = eax; //is same as ebx += point size

//remove 1.0

	eb2 = (uint8_t *)_dp;
	ea2 = (uintptr_t)edi - (uintptr_t)eb2;
//gouraudpsize*2
	if (ea2 <= ( flatpsize * 2 )) goto cubezclip_w; //less than 3 points

	fpu_reg10 = ( ((tscreenpoint *)(eb2 + flatpsize * 2))->sp_x ); //gouraudpsize*2]
	fpu_reg10 = fpu_reg10 - ( ((tscreenpoint *)eb2)->sp_x );
	fpu_reg11 = ( ((tscreenpoint *)(eb2 + flatpsize))->sp_y ); //gouraudpsize]
	fpu_reg11 = fpu_reg11 - ( ((tscreenpoint *)eb2)->sp_y );
	fpu_reg10 = fpu_reg10 * fpu_reg11;
	fpu_reg11 = ( ((tscreenpoint *)(eb2 + flatpsize * 2))->sp_y ); //gouraudpsize*2]
	fpu_reg11 = fpu_reg11 - ( ((tscreenpoint *)eb2)->sp_y );
	fpu_reg12 = ( ((tscreenpoint *)(eb2 + flatpsize))->sp_x ); //gouraudpsize]
	fpu_reg12 = fpu_reg12 - ( ((tscreenpoint *)eb2)->sp_x );
	fpu_reg11 = fpu_reg11 * fpu_reg12;
//fcomp
//fstsw   ax
//fstp    st



	if (fpu_reg11 <= fpu_reg10) goto cubezclip_w;
	ea2 = 0;
	goto cubezclip_weg;
cubezclip_w:
	ea2 = ( 1 );

cubezclip_weg:
	_edi = edi;
	_ebx = (void *)eb2;
	return ea2;
}


static uint32_t drawcube(uint32_t _eax, ttempcube *esi) {
	uint32_t eax = _eax;
	tcube *ebx;
	tscreenpoint *eb4, *edi;
	uint8_t *eb2, *stack_var00;
	void *eb3;
	ttempcube *stack_var01;
//-> eax = idx
//-> esi -> ttempcube
//<- esi -> ttempcube
	uint32_t z, ifl0;
	tmaterial *mat;

	esi->tc_idx = eax;
	eax = set_high_byte(eax, ( (uint8_t)fog ));
	eax = ((eax & 0xff000000) >> 24) | ((eax & 0x00ff0000) >> 8) | ((eax & 0x0000ff00) << 8) | ((eax & 0x000000ff) << 24);
//       shl     eax,24
	eax = set_high_byte(eax, ( (uint8_t)ambient )); //20h;10h
	ifl0 = eax;

	ebx = ( esi->tc_cubeptr );
	ebx->c_vstamp = stamp;
	ebx->c_vtc = esi;

	eax = ( ebx->c_faces );

	if (( (int32_t)eax ) == 0) goto drawcube_weg;
	z = eax;
	eb2 = (uint8_t *)( ebx->c_facedata );
drawcube_l: //ebx -> tcface
	mat = ( ((tcface *)eb2)->cf_mat );
	eax = ( ((tcface *)eb2)->cf_points );
	eb2 = eb2 + ( sizeof(tcface) );
//ebx -> source points of type tcpoint
	stack_var00 = eax + eb2; //eax -> sp_end / next face
	edi = (tscreenpoint *)scene.sc_datapos;
	eb3 = (void *)eb2;
	eax = cubezclip((tcpoint *)stack_var00, eb3, esi, edi); //esi -> ttempcube
	eb4 = (tscreenpoint *)eb3;

	if (( (int32_t)eax ) != 0) goto drawcube_w1;

	stack_var01 = esi;

	if (( esi->tc_recursion ) > ( t_sub )) goto drawcube_affine;
//with scanline subdivision
	eax = subxclip(( flatpsize ), ( flatvars ), eb4, edi); //gouraudpsize, gouraudvars

	if (( (int32_t)eax ) != 0) goto drawcube_w0;
//sfDrawed flag
	subpolygon(eb4, (uintptr_t)edi - (uintptr_t)eb4, mat, ifl0);
	goto drawcube_w0;
drawcube_affine: //without scanline subdivision
	eax = xclip(( flatpsize ), ( flatvars ), eb4, edi); //gouraudpsize, gouraudvars

	if (( (int32_t)eax ) != 0) goto drawcube_w0;
//sfDrawed flag
	polygon(eb4, (uintptr_t)edi - (uintptr_t)eb4, mat, ifl0);

drawcube_w0: //esi -> ttempcube
	esi = stack_var01;
drawcube_w1: //ebx -> tcface
	eb2 = stack_var00;
	z = ( (int32_t)z ) - 1;
	if (( (int32_t)z ) != 0) goto drawcube_l;

drawcube_weg:
	return eax;
}


static void insert(uint32_t _edx, realnum _fpu_reg9) {
	realnum fpu_reg9 = _fpu_reg9, fpu_reg10, fpu_reg11;
	uint32_t edx = _edx;
	tscene *edi;
	tidxentry *ebx;
//-> edx = idx
//-> st(0) = z-value


	edi = ( &(scene) );
	ebx = (tidxentry *)( edi->sc_idxlistend );

insert_s_l: //search insert position

	if (ebx <= ( edi->sc_idxlist )) goto insert_0;
	ebx = ebx - ( 1 );

//highest zval first


	if (fpu_reg9 > ( ebx->i_zval )) goto insert_s_l;
	ebx = ebx + ( 1 );
insert_0:

	fpu_reg10 = fpu_reg9;
insert_m_l: //move entries

	if (ebx >= ( edi->sc_idxlistend )) goto insert_1;
	fpu_reg11 = ( ebx->i_zval );
	{ realnum tmp = fpu_reg10; fpu_reg10 = fpu_reg11; fpu_reg11 = tmp; }
	ebx->i_zval = fpu_reg11;
	{ uint32_t value = edx; edx = ( ebx->i_idx ); ebx->i_idx = value; }

	ebx = ebx + ( 1 );
	goto insert_m_l;
insert_1:
	ebx->i_zval = fpu_reg10;
	ebx->i_idx = edx;
	edi->sc_idxlistend = (void *)(( (uintptr_t)edi->sc_idxlistend ) + ( sizeof(tidxentry) ));

//insert_weg:
	return;
}


//zclip and project face for rendering onto z-line
static uint32_t zprojectline(tscreenpoint *&_ebx, tscreenpoint *&_edi) {
	realnum fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13;
	uint32_t eax, edx;
	tscreenpoint *edi = _edi, *ebx = _ebx, *esi;
//arg     spsize:dword, vars_1:dword ;data variables -1
//-> ebx = *sp
//-> edi = *sp_end
//<- ebx = *sp
//<- edi = *sp_end
//<- flags: if less than 3 points, jump with jbe
	tscreenpoint *_sp, *_sp_end; //both are pointers

//x, y, z: components of source point
//d : prefix for destination points
//n : prefix for next sorce point

	_sp = ebx; //ebx -> source points
	_sp_end = edi; //edi -> destination points

	fpu_reg10 = 1.0;
zprojectline_z_l:
//inn = true : point in front of plane
	fpu_reg11 = ( ebx->sp_z ); //inn = (z >= 1)

	eax = /*(eax & 0xffff0000) |*/ ((fpu_reg11 < fpu_reg10)?0x100:0);
	edx = set_high_byte(0 /*edx*/, ( (uint8_t)(eax >> 8) )); //dh = inn ;if (inn)

	if ((eax & 0x100) != 0) goto zprojectline_0;
//point in front of projection plane (visible)
	fpu_reg11 = ( ebx->sp_z );
	edi->sp_z = fpu_reg11;

	fpu_reg12 = ( ebx->sp_x ); //x = (x/z + 1) *xmul
	fpu_reg12 = fpu_reg12 / fpu_reg11;
	fpu_reg12 = fpu_reg12 + fpu_reg10;
	fpu_reg12 = fpu_reg12 * z_xmul;
	edi->sp_x = fpu_reg12;

	fpu_reg12 = ( ebx->sp_y ); //y = (y/z + a) *ymul
	fpu_reg11 = fpu_reg12 / fpu_reg11;
	fpu_reg11 = fpu_reg11 + z_a;
	fpu_reg11 = fpu_reg11 * z_ymul;
	edi->sp_y = fpu_reg11;

	edi = (tscreenpoint *)( ((uintptr_t)edi) + zlinepsize ); //one destination point added
//end of if statement
zprojectline_0:
	esi = ebx; //ebx -> actual source point
	esi = (tscreenpoint *)( ((uintptr_t)esi) + zlinepsize ); //esi -> next source point
//wrap if at last point
	if (esi < _sp_end) goto zprojectline_wrap;
	esi = _sp;
zprojectline_wrap:
//if (inn ^ (nz >= 1))
	fpu_reg11 = ( esi->sp_z );

	eax = /*(eax & 0xffff0000) |*/ ((fpu_reg11 < fpu_reg10)?0x100:0);
	eax = set_high_byte(eax, ( (uint8_t)(eax >> 8) ) ^ ( (uint8_t)(edx >> 8) )); //dh = inn

	if ((eax & 0x100) == 0) goto zprojectline_1;
//this or next point behind projection plane
//r = (1.0-z)/(nz-z)
	fpu_reg11 = 1.0;
	edi->sp_z = fpu_reg11; //dz = 1.0
	fpu_reg11 = fpu_reg11 - ( ebx->sp_z );
	fpu_reg12 = ( esi->sp_z );
	fpu_reg12 = fpu_reg12 - ( ebx->sp_z );
	fpu_reg11 = fpu_reg11 / fpu_reg12; //st = r

	fpu_reg12 = ( ebx->sp_x ); //x' = x + r*(nx-x)
	fpu_reg13 = ( esi->sp_x );
	fpu_reg13 = fpu_reg13 - fpu_reg12;
	fpu_reg13 = fpu_reg13 * fpu_reg11;
	fpu_reg12 = fpu_reg12 + fpu_reg13;
	fpu_reg12 = fpu_reg12 + fpu_reg10; //x = (x' + 1) *xmul
	fpu_reg12 = fpu_reg12 * z_xmul;
	edi->sp_x = fpu_reg12;

	fpu_reg12 = ( ebx->sp_y ); //y' = y + r*(ny-y)
	fpu_reg13 = ( esi->sp_y );
	fpu_reg13 = fpu_reg13 - fpu_reg12;
	fpu_reg11 = fpu_reg11 * fpu_reg13; //remove r
	fpu_reg11 = fpu_reg11 + fpu_reg12;
	fpu_reg11 = fpu_reg11 + z_a; //y = (y' + a) *ymul
	fpu_reg11 = fpu_reg11 * z_ymul;
	edi->sp_y = fpu_reg11;


	edi = (tscreenpoint *)( ((uintptr_t)edi) + zlinepsize ); //one destination point added
//end of if statement
zprojectline_1:
//ns > s?
	if (esi <= ebx) goto zprojectline_notz_l;
	ebx = esi; //is same as ebx += point size
	goto zprojectline_z_l; //repeat loop
zprojectline_notz_l:
	ebx = esi; //is same as ebx += point size

	ebx = _sp_end;
//ebx -> start of s-points
//remove 1.0

	eax = (uintptr_t)edi - (uintptr_t)ebx;
//        shr     eax,1
//spsize
	if (eax <= ( zlinepsize * 2 )) goto zprojectline_w;
	eax = 0;
	goto zprojectline_weg;
zprojectline_w:
	eax = ( 1 );
zprojectline_weg:
	_edi = edi;
	_ebx = ebx;
	return eax;
}


//project face for rendering onto z-line
static void projectline(uint32_t spsize, tscreenpoint *&_ebx, tscreenpoint *&_edi) {
	realnum fpu_reg10, fpu_reg11, fpu_reg12;
	tscreenpoint *edx, *edi = _edi, *ebx = _ebx;

//-> ebx -> sp
//-> edi -> sp_end

	edx = edi;
projectline_l:
	fpu_reg10 = ( ebx->sp_lz );
	fpu_reg11 = ( ebx->sp_lx ); //x = (x/z + 1) *xmul
	fpu_reg11 = fpu_reg11 / fpu_reg10;
	fpu_reg12 = 1.0;
	fpu_reg11 = fpu_reg11 + fpu_reg12;
	fpu_reg11 = fpu_reg11 * z_xmul;
	edi->sp_x = fpu_reg11;

	fpu_reg11 = ( ebx->sp_ly ); //y = (y/z + a) *ymul
	fpu_reg11 = fpu_reg11 / fpu_reg10;
	fpu_reg11 = fpu_reg11 + z_a;
	fpu_reg11 = fpu_reg11 * z_ymul;
	edi->sp_y = fpu_reg11;
	edi->sp_z = fpu_reg10;

	ebx = (tscreenpoint *)( ((uintptr_t)ebx) + spsize ); //lightpsize
	edi = (tscreenpoint *)( ((uintptr_t)edi) + zlinepsize );

	if (ebx < edx) goto projectline_l;

//projectline_weg:
	_edi = edi;
	_ebx = ebx;
}


#if 0
//gprojectline proc near
//        ;-> ebx -> sp
//        ;-> edi -> sp_end
//
//        mov     edx,edi
//@@l:
//        fld     sp_lz[ebx]
//        fld     sp_lx[ebx]
//        fld1
//        faddp   st(1),st
//        fdiv    st,st(1)
//        fmul    z_xmul
//        fstp    sp_x[edi]
//        fld     sp_y[ebx]
//        fadd    z_a
//        fdiv    st,st(1)
//        fmul    z_ymul
//        fstp    sp_y[edi]
//        fstp    sp_z[edi]
//
//        add     ebx,glightpsize
//        add     edi,zlinepsize
//        cmp     ebx,edx
//        jb      @@l
//
//@@weg:  ret
//gprojectline endp
//
#endif

static void renderline(tscreenpoint *_sp, uint32_t sp_end) {
	realnum fpu_reg10, fpu_reg11, fpu_reg12;
	uint32_t eax, ecx, ed2, ebx, esi;
	uint8_t *edi, *es2;

//_sp = *sp, zeiger auf screenpoints

	uint32_t pend, y, ymax, lb, lc, rb, rc; //int
	uint8_t *x_y;
	float lx, ldx, rx, rdx, lz, ldz, rz, rdz; //float
	uint32_t xa, xe; //int


	eax = 0; //start- und endpunkt bestimmen
	lb = eax;
	rb = eax;
	pend = eax;

	es2 = (uint8_t *)_sp; //esi -> screenpoints
	fpu_reg10 = ( ((tscreenpoint *)es2)->sp_y ); //st(0) = ymax
	fpu_reg11 = fpu_reg10; //st(1) = ymin

	ecx = sp_end;
	ecx = ecx - ( zlinepsize ); //esi+ecx -> sp[sp_end-1]
renderline_max_l:

	fpu_reg12 = ( ((tscreenpoint *)(es2 + ecx))->sp_y );
//ymax              ;gr”áten y-wert finden (endpunkt)


	if (fpu_reg12 < fpu_reg11) goto renderline_max;
	pend = ecx;
	fpu_reg11 = fpu_reg12; //ymax
	goto renderline_min;
renderline_max:
//ymin              ;kleinsten y-wert finden (startpunkt)


	if (fpu_reg12 > fpu_reg10) goto renderline_min;
	lb = ecx;
	rb = ecx;
	fpu_reg10 = fpu_reg12; //ymin
renderline_min:

	ecx = ( (int32_t)ecx ) - ( zlinepsize );
	if (( (int32_t)ecx ) != 0) goto renderline_max_l;
	ymax = (int32_t)ceil(fpu_reg11); //ymax entfernen

	y = (int32_t)ceil(fpu_reg10); //y = ceil(ymin)

	eax = y;

//y-clipping
	if (( (int32_t)eax ) >= ( zyres )) goto renderline_weg;


	if (( (int32_t)eax ) > 0) goto renderline_y0;
	eax = 0;
	y = eax;
renderline_y0:

	if (( (int32_t)eax ) >= ( (int32_t)ymax )) goto renderline_weg;

	eax = eax << ( zxshift + 2 ); //6+2;*64*4
	x_y = eax + (uint8_t *)scene.sc_zline;

	lc = ( 1 );
	rc = ( 1 );

renderline_y_l: //y-schleife

//links
	lc = ( (int32_t)lc ) - 1;
	if (( (int32_t)lc ) != 0) goto renderline_l_nz;
	edi = (uint8_t *)_sp;
	esi = lb;
renderline_lc:

	if (esi == pend) goto renderline_fertig; //lb == pend -> unten angekommen
	ebx = esi; //ia = lb
	esi = esi + ( zlinepsize ); //rb++

	if (esi < sp_end) goto renderline_l0; //wrap
	esi = 0;
renderline_l0:
	fpu_reg10 = ( ((tscreenpoint *)(edi + esi))->sp_y ); //lc = ceil(sp[lb].sy) - y
	lc = (int32_t)ceil(fpu_reg10);
	eax = y;
	lc = ( (int32_t)lc ) - ( (int32_t)eax );
	if (( (int32_t)lc ) <= 0) goto renderline_lc; //while lc <= 0
	lb = esi;

	fpu_reg10 = ( (int32_t)y );
	fpu_reg10 = fpu_reg10 - ( ((tscreenpoint *)(edi + ebx))->sp_y ); //(y  - sp[ia].y)

	fpu_reg11 = ( ((tscreenpoint *)(edi + esi))->sp_y ); //(sp[lb].y - sp[ia].y)
	fpu_reg11 = fpu_reg11 - ( ((tscreenpoint *)(edi + ebx))->sp_y );

//ldz = (sp[lb].u - sp[ia].u)/(sp[lb].y - sp[ia].y);
	fpu_reg12 = ( ((tscreenpoint *)(edi + esi))->sp_z );
	fpu_reg12 = fpu_reg12 - ( ((tscreenpoint *)(edi + ebx))->sp_z );
	fpu_reg12 = fpu_reg12 / fpu_reg11;
	ldz = fpu_reg12;

//lz = ldz  *(y  - sp[ia].y) + sp[ia].z;
	fpu_reg12 = fpu_reg12 * fpu_reg10;
	fpu_reg12 = fpu_reg12 + ( ((tscreenpoint *)(edi + ebx))->sp_z );
	lz = fpu_reg12;

//ldx = (sp[lb].x - sp[ia].x)/(sp[lb].y - sp[ia].y);
	fpu_reg12 = ( ((tscreenpoint *)(edi + esi))->sp_x );
	fpu_reg12 = fpu_reg12 - ( ((tscreenpoint *)(edi + ebx))->sp_x );
	fpu_reg11 = fpu_reg12 / fpu_reg11;
	ldx = fpu_reg11;

//lx = ldx  *(y  - sp[ia].y) + sp[ia].x;
	fpu_reg10 = fpu_reg10 * fpu_reg11;
	fpu_reg10 = fpu_reg10 + ( ((tscreenpoint *)(edi + ebx))->sp_x );
	lx = fpu_reg10; //lx bleibt im copro

	goto renderline_l_z;
renderline_l_nz:
	fpu_reg10 = lz;
	fpu_reg10 = fpu_reg10 + ldz;
	lz = fpu_reg10;

	fpu_reg10 = lx;
	fpu_reg10 = fpu_reg10 + ldx; //lx bleibt im copro
	lx = fpu_reg10;
renderline_l_z:

	xa = (int32_t)ceil(fpu_reg10); //xa = ceil(lx) (lx entfernen)

//rechts
	rc = ( (int32_t)rc ) - 1;
	if (( (int32_t)rc ) != 0) goto renderline_r_nz;
	edi = (uint8_t *)_sp;
	esi = rb;
renderline_rc:


	if (esi == pend) goto renderline_fertig; //rb == pend -> unten angekommen
	ebx = esi; //ia = rb
	{ uint32_t carry = (esi < ( zlinepsize ))?1:0; esi = esi - ( zlinepsize ); //lb--
	  if (carry == 0) goto renderline_r0; } //wrap
	esi = esi + sp_end;
renderline_r0:
	fpu_reg10 = ( ((tscreenpoint *)(edi + esi))->sp_y ); //rc = ceil(sp[rb].sy) - y
	rc = (int32_t)ceil(fpu_reg10);
	eax = y;
	rc = ( (int32_t)rc ) - ( (int32_t)eax );
	if (( (int32_t)rc ) <= 0) goto renderline_rc; //while rc <= 0
	rb = esi;

	fpu_reg10 = ( (int32_t)y );
	fpu_reg10 = fpu_reg10 - ( ((tscreenpoint *)(edi + ebx))->sp_y ); //(y  - sp[ia].y)

	fpu_reg11 = ( ((tscreenpoint *)(edi + esi))->sp_y ); //(sp[rb].y - sp[ia].y)
	fpu_reg11 = fpu_reg11 - ( ((tscreenpoint *)(edi + ebx))->sp_y );

//rdz = (sp[rb].z - sp[ia].z)/(sp[rb].y - sp[ia].y);
	fpu_reg12 = ( ((tscreenpoint *)(edi + esi))->sp_z );
	fpu_reg12 = fpu_reg12 - ( ((tscreenpoint *)(edi + ebx))->sp_z );
	fpu_reg12 = fpu_reg12 / fpu_reg11;
	rdz = fpu_reg12;

//rz = rdz  *(y  - sp[ia].y) + sp[ia].z;
	fpu_reg12 = fpu_reg12 * fpu_reg10;
	fpu_reg12 = fpu_reg12 + ( ((tscreenpoint *)(edi + ebx))->sp_z );
	rz = fpu_reg12;

//rdx = (sp[rb].x - sp[ia].x)/(sp[rb].y - sp[ia].y);
	fpu_reg12 = ( ((tscreenpoint *)(edi + esi))->sp_x );
	fpu_reg12 = fpu_reg12 - ( ((tscreenpoint *)(edi + ebx))->sp_x );
	fpu_reg11 = fpu_reg12 / fpu_reg11;
	rdx = fpu_reg11;

//rx = rdx  *(y  - sp[ia].y) + sp[ia].x;
	fpu_reg10 = fpu_reg10 * fpu_reg11;
	fpu_reg10 = fpu_reg10 + ( ((tscreenpoint *)(edi + ebx))->sp_x );
	rx = fpu_reg10; //rx bleibt im copro

	goto renderline_r_z;
renderline_r_nz:
	fpu_reg10 = rz;
	fpu_reg10 = fpu_reg10 + rdz;
	rz = fpu_reg10;

	fpu_reg10 = rx;
	fpu_reg10 = fpu_reg10 + rdx; //rx bleibt im copro
	rx = fpu_reg10;
renderline_r_z:
	xe = (int32_t)ceil(fpu_reg10); //xe = ceil(rx) (rx entfernen)

	ed2 = xa;
	ed2 = ed2 - 1;

	if (( (int32_t)ed2 ) >= 0) goto renderline_c0;
	ed2 = 0;
renderline_c0:
	ecx = xe;
	ecx = ecx + 1;

	if (( (int32_t)ecx ) < ( zxres )) goto renderline_c1;
	ecx = ( zxres );
renderline_c1:

	ecx = ( (int32_t)ecx ) - ( (int32_t)ed2 ); //ecx = pixel in einer zeile
	if (( (int32_t)ecx ) <= 0) goto renderline_w;
	ed2 = ed2 << ( 2 ); //*4
	edi = ed2 + x_y; //edi -> pixel


	fpu_reg10 = lz;
	es2 = (uint8_t *)scene.sc_divtab;
	fpu_reg11 = rz;
	fpu_reg11 = fpu_reg11 - fpu_reg10;
	fpu_reg11 = fpu_reg11 * ( ((float *)(es2))[ecx] );
	{ realnum tmp = fpu_reg10; fpu_reg10 = fpu_reg11; fpu_reg11 = tmp; }

renderline_l:



	if (fpu_reg11 > ( *((float *)(edi)) )) goto renderline_0;
	*((float *)(edi)) = fpu_reg11;
renderline_0:
	edi = edi + ( 4 );
	fpu_reg11 = fpu_reg11 + fpu_reg10;
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto renderline_l;




renderline_w:
	x_y = x_y + ( zxres * 4 );
	y = y + 1;
//while (y < zyres)
	if (( (int32_t)y ) < ( zyres )) goto renderline_y_l;

renderline_fertig:

renderline_weg:
	return;
}



static uint32_t getnextmesh(tmesh *&_esi) {
	tmesh *eax, *edx, *esi = _esi;
//-> esi -> tobject
//<- Z-flag = 0: no next mesh
getnextmesh_l:
	edx = (tmesh *)( esi->m_object.o_child );

	if (edx != 0) goto getnextmesh_t;
getnextmesh_o:
	eax = (tmesh *)( esi->m_object.o_owner );

	if (eax == 0) goto getnextmesh_weg;
//getnextmesh_n:
	edx = (tmesh *)( esi->m_object.o_next );

	if (edx != 0) goto getnextmesh_t;

	esi = eax;
	goto getnextmesh_o;
getnextmesh_t:
	esi = edx;

	if ((( esi->m_object.o_flags ) & ( ofMesh )) == 0) goto getnextmesh_l;
	eax = esi;
getnextmesh_weg:
	_esi = esi;
	return eax ? 1 : 0;
}



static tmesh *xformhierarchy(tmesh *esi, tviewer *edi) {
	realnum fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13, fpu_reg14, fpu_reg15, fpu_reg16, fpu_reg17;
	uint32_t eax, edx, ecx, ebx;
	uint8_t *ea2;
	tvec *ed2, *eb2;
	ttempface *ed3;
	tface *eb3;
//-> esi -> top level mesh
//-> edi -> tviewer (camera or light)
//<- eax -> idx
//<- esi -> top level mesh
//<- edi -> tviewer
	tvec p;
	tmatrix A;
	uint32_t ar_idx;
	uint32_t p_x1;


	fpu_reg10 = c_min;
	fpu_reg11 = c_max; //st(0) = zmin, st(1) = zmax
xformhierarchy_m_l0: //edi -> tviewer

	if ((( esi->m_object.o_flags ) & ( ofAbsolute )) != 0) goto xformhierarchy_abs;
//calculate matrix A = Av * Ao (v:viewer, o:object)
	ebx = 0; //ebx = n
xformhierarchy_l0:
	fpu_reg12 = ( esi->m_object.o_A.A_1n[ebx] ); //n-th column of A
	fpu_reg13 = ( esi->m_object.o_A.A_2n[ebx] );
	fpu_reg14 = ( esi->m_object.o_A.A_3n[ebx] );

	edx = 0; //edx = m
xformhierarchy_l1:
	fpu_reg15 = ( edi->v_l.A_mn[0 + edx] ); //* m-th row of Al
	fpu_reg15 = fpu_reg15 * fpu_reg12;
	fpu_reg16 = ( edi->v_l.A_mn[1 + edx] );
	fpu_reg16 = fpu_reg16 * fpu_reg13;
	fpu_reg17 = ( edi->v_l.A_mn[2 + edx] );
	fpu_reg17 = fpu_reg17 * fpu_reg14;
	fpu_reg16 = fpu_reg16 + fpu_reg17;
	fpu_reg15 = fpu_reg15 + fpu_reg16;
	A.A_mn[edx + ebx] = fpu_reg15; //= new element (m,n) of A

	edx = edx + ( 3 );

	if (edx < ( 3 * 3 )) goto xformhierarchy_l1;





	ebx = ebx + 1;

	if (ebx < ( 3 )) goto xformhierarchy_l0;

	goto xformhierarchy_abs0;
xformhierarchy_abs: //absolute: matrix A = Av (only position valid)
	ecx = ( 9 - 1 );
	eb2 = ( &(edi->v_l1) );
xformhierarchy_l:
	A.A_mn[ecx] = ( ((float *)(eb2))[ecx] );
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) >= 0) goto xformhierarchy_l;

xformhierarchy_abs0:
//p = Av*(po-pv)
	fpu_reg12 = ( esi->m_object.o_p.x1 ); //(po-pv)
	fpu_reg12 = fpu_reg12 - ( edi->v_p.x1 );
	fpu_reg13 = ( esi->m_object.o_p.x2 );
	fpu_reg13 = fpu_reg13 - ( edi->v_p.x2 );
	fpu_reg14 = ( esi->m_object.o_p.x3 );
	fpu_reg14 = fpu_reg14 - ( edi->v_p.x3 );

	ebx = 0; //ebx = m
xformhierarchy_l2:
	edx = ( ebx + ebx * 2 );
	fpu_reg15 = ( edi->v_l.A_mn[0 + edx] ); //* m-th row of Av
	fpu_reg15 = fpu_reg15 * fpu_reg12;
	fpu_reg16 = ( edi->v_l.A_mn[1 + edx] );
	fpu_reg16 = fpu_reg16 * fpu_reg13;
	fpu_reg17 = ( edi->v_l.A_mn[2 + edx] );
	fpu_reg17 = fpu_reg17 * fpu_reg14;
	fpu_reg16 = fpu_reg16 + fpu_reg17;
	fpu_reg15 = fpu_reg15 + fpu_reg16;
	p.xn[ebx] = fpu_reg15; //= new element (m,1) of p

	ebx = ebx + 1;

	if (ebx < ( 3 )) goto xformhierarchy_l2;





//----

//transform all vertices
	eb2 = (tvec *)scene.sc_datapos; //ebx -> list of transformed vertices
	esi->m_tlist = (float *)eb2; //save list start for this mesh

//edi -> tviewer
	if (( edi->v_light ) != ( 0 )) goto xformhierarchy_light;
	esi->m_vtlist = (float *)eb2; //if camera: reference for light procs
	esi->m_vstamp = stamp;
xformhierarchy_light:
//esi -> tmesh
	ecx = ( esi->m_vertices );
	ed2 = (tvec *)( esi->m_vertexlist );

xformhierarchy_t_l: //vertex v (calc A*v + p)
	fpu_reg12 = ( ed2->x1 );
	fpu_reg13 = ( ed2->x2 );
	fpu_reg14 = ( ed2->x3 );

	fpu_reg15 = A.A_11; //* 1-st row of A
	fpu_reg15 = fpu_reg15 * fpu_reg12;
	fpu_reg16 = A.A_12;
	fpu_reg16 = fpu_reg16 * fpu_reg13;
	fpu_reg17 = A.A_13;
	fpu_reg17 = fpu_reg17 * fpu_reg14;
	fpu_reg16 = fpu_reg16 + fpu_reg17;
	fpu_reg15 = fpu_reg15 + fpu_reg16;
	fpu_reg15 = fpu_reg15 + p.x1;
	eb2->x1 = fpu_reg15; //= x1 of the vertex

	fpu_reg15 = A.A_21; //* 2-nd row of A
	fpu_reg15 = fpu_reg15 * fpu_reg12;
	fpu_reg16 = A.A_22;
	fpu_reg16 = fpu_reg16 * fpu_reg13;
	fpu_reg17 = A.A_23;
	fpu_reg17 = fpu_reg17 * fpu_reg14;
	fpu_reg16 = fpu_reg16 + fpu_reg17;
	fpu_reg15 = fpu_reg15 + fpu_reg16;
	fpu_reg15 = fpu_reg15 + p.x2;
	eb2->x2 = fpu_reg15; //= x2 of the vertex

	fpu_reg15 = A.A_31; //* 3-rd row of A
	fpu_reg12 = fpu_reg12 * fpu_reg15;
	fpu_reg15 = A.A_32;
	fpu_reg13 = fpu_reg13 * fpu_reg15;
	fpu_reg15 = A.A_33;
	fpu_reg14 = fpu_reg14 * fpu_reg15;
	fpu_reg13 = fpu_reg13 + fpu_reg14;
	fpu_reg12 = fpu_reg12 + fpu_reg13;
	fpu_reg12 = fpu_reg12 + p.x3;
	eb2->x3 = fpu_reg12; //= x3 of the vertex

//zmin


	if (fpu_reg12 >= fpu_reg11) goto xformhierarchy_zmin0;
	fpu_reg11 = fpu_reg12; //zmin
xformhierarchy_zmin0:
//zmax


	if (fpu_reg12 <= fpu_reg10) goto xformhierarchy_zmax0;
	fpu_reg10 = fpu_reg12; //zmax
xformhierarchy_zmax0:

	ed2 = ed2 + ( 1 );
	eb2 = eb2 + ( 1 );

	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto xformhierarchy_t_l;


//transform normals
//edi -> tviewer
	if (( edi->v_light ) == ( 0 )) goto xformhierarchy_nw;
	ecx = ( esi->m_vertices ); //esi -> tmesh
//edx -> normals
xformhierarchy_t_l1: //normal n (calc A*n)
	fpu_reg12 = ( ed2->x1 );
	fpu_reg12 = fpu_reg12 * A.A_31;
	fpu_reg13 = ( ed2->x2 );
	fpu_reg13 = fpu_reg13 * A.A_32;
	fpu_reg14 = ( ed2->x3 );
	fpu_reg14 = fpu_reg14 * A.A_33;
	fpu_reg13 = fpu_reg13 + fpu_reg14;
	fpu_reg12 = fpu_reg12 + fpu_reg13;
	*((float *)(eb2)) = fpu_reg12; //= x3 of the vertex

	ed2 = ed2 + ( 1 );
	eb2 = eb2 + ( 1 ); //4

	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto xformhierarchy_t_l1;

xformhierarchy_nw:
	scene.sc_datapos = (void *)eb2;
//esi -> tmesh
	eax = getnextmesh(esi); //Z-flag = 0: no next mesh

	if (( (int32_t)eax ) != 0) goto xformhierarchy_m_l0;


//esi -> top level mesh
//edi -> tviewer
	if ((( esi->m_object.o_flags ) & ( mfSort )) == 0) goto xformhierarchy_Nsort;
//----
//sort faces

//clear tempfaces pointer array
	ecx = ( 15 );
xformhierarchy_clear:
	edi->v_tempfaces[ecx] = ( _NULL );
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) >= 0) goto xformhierarchy_clear;

//make temp-faces
	fpu_reg12 = fpu_reg10;
	fpu_reg13 = fpu_reg11;
	fpu_reg14 = c_3;
	fpu_reg13 = fpu_reg13 * fpu_reg14;
	fpu_reg12 = fpu_reg12 * fpu_reg14;
//        fsub    c_0_01                  ;zmin -= 0.01 (force round up)
	fpu_reg14 = fpu_reg12; //zmax
	fpu_reg14 = fpu_reg14 - fpu_reg13; //zmin
	fpu_reg14 = c_16 / fpu_reg14; //15_9                  ;scale = 15.9/(zmax-zmin)


xformhierarchy_m_l1: //edi -> tviewer
	ed3 = (ttempface *)scene.sc_datapos; //edx -> temp-face-list
	ecx = ( esi->m_faces );
	if (ecx == 0) goto xformhierarchy_m_w;
	p_x1 = ecx;
	eb3 = ( esi->m_facelist ); //ebx -> tface
xformhierarchy_f_l: //edx -> ttempface
	ed3->tf_faceptr = eb3; //connect face to temp-face

	ea2 = (uint8_t *)( esi->m_tlist );
	ed3->tf_meshptr = esi; //tf_tlist,eax
//calc z distance of face
	ecx = ( eb3->f_p[0].fp_vertex );
	fpu_reg15 = ( ((tvec *)(ea2 + ecx))->x3 );
	ecx = ( eb3->f_p[1].fp_vertex );
	fpu_reg15 = fpu_reg15 + ( ((tvec *)(ea2 + ecx))->x3 );
	ecx = ( eb3->f_p[2].fp_vertex );
	fpu_reg15 = fpu_reg15 + ( ((tvec *)(ea2 + ecx))->x3 );

	fpu_reg15 = fpu_reg15 - fpu_reg13; //(z-zmin)*scale
	fpu_reg15 = fpu_reg15 * fpu_reg14;
	ar_idx = (int32_t)ceil(fpu_reg15); //ar_idx = ceil()
	ecx = ar_idx;

	ecx = ecx - 1;
	ecx = ecx & ( 15 );
//  cmp ecx,0
//  jge @@t0
//  xor ecx,ecx
//@@t0:
//  cmp ecx,15
//  jbe @@t1
//  mov ecx,15
//@@t1:
	ea2 = (uint8_t *)( edi->v_tempfaces[ecx] );
	edi->v_tempfaces[ecx] = ed3;
	ed3->tf_next = (ttempface *)ea2;

	ed3 = ed3 + ( 1 );
	eb3 = eb3 + ( 1 );
	p_x1 = ( (int32_t)p_x1 ) - 1;
	if (( (int32_t)p_x1 ) != 0) goto xformhierarchy_f_l;
xformhierarchy_m_w:
	scene.sc_datapos = (void *)ed3;

	eax = getnextmesh(esi);

	if (( (int32_t)eax ) != 0) goto xformhierarchy_m_l1;

//remove zmin*3
//remove zmax*3
//remove scale

xformhierarchy_Nsort: //edi -> tviewer
	edi->v_zmin = fpu_reg11; //remove zmin
	edi->v_zmax = fpu_reg10; //remove zmax

	return esi;
}



static uint32_t projection(uint32_t spsize, uint32_t vars_1, tscreenpoint *&_ebx, tscreenpoint *&_edi) {
	realnum fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13;
	uint32_t eax, edx, ecx;
	tscreenpoint *edi = _edi, *ebx = _ebx, *esi;
//data variables -1
//-> ebx = *sp
//-> edi = *sp_end
//<- ebx = *sp
//<- edi = *sp_end
//<- flags: if less than 3 points, jump with jbe
	tscreenpoint *_sp, *_sp_end; //both are pointers

//x, y, z: components of source point
//d : prefix for destination points
//n : prefix for next sorce point

	_sp = ebx; //ebx -> source points
	_sp_end = edi; //edi -> destination points

	fpu_reg10 = 1.0;
projection_z_l:
//inn = true : point in front of plane
	fpu_reg11 = ( ebx->sp_z ); //inn = (z >= 1)

	eax = /*(eax & 0xffff0000) |*/ ((fpu_reg11 < fpu_reg10)?0x100:0);
	edx = set_high_byte(0 /*edx*/, ( (uint8_t)(eax >> 8) )); //dh = inn
//if (inn)
	if ((eax & 0x100) != 0) goto projection_0;
//point in front of projection plane (visible)
	fpu_reg11 = ( ebx->sp_z );
	edi->sp_z = fpu_reg11;

	fpu_reg12 = ( ebx->sp_x ); //x = xres/2 *(1 + x/z)
	fpu_reg12 = fpu_reg12 / fpu_reg11;
	fpu_reg12 = fpu_reg12 + fpu_reg10;
	fpu_reg12 = fpu_reg12 * xmid;
	edi->sp_x = fpu_reg12;

	fpu_reg12 = ( ebx->sp_y ); //y = yres/2 *(1 - y/z)
	fpu_reg11 = fpu_reg12 / fpu_reg11;
	fpu_reg11 = fpu_reg10 - fpu_reg11;
	fpu_reg11 = fpu_reg11 * ymid;
	edi->sp_y = fpu_reg11;

	ecx = vars_1; //mapping
projection_l1:
	edi->sp_data[ecx] = ebx->sp_data[ecx];
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) >= 0) goto projection_l1;

	edi = (tscreenpoint *)( ((uintptr_t)edi) + spsize ); //one destination point added
//end of if statement
projection_0:
	esi = ebx; //ebx -> actual source point
	esi = (tscreenpoint *)( ((uintptr_t)esi) + spsize ); //esi -> next source point
//wrap if at last point
	if (esi < _sp_end) goto projection_wrap;
	esi = _sp;
projection_wrap:
//if (inn ^ (nz >= 1))
	fpu_reg11 = ( esi->sp_z );

	eax = /*(eax & 0xffff0000) |*/ ((fpu_reg11 < fpu_reg10)?0x100:0);
	eax = set_high_byte(eax, ( (uint8_t)(eax >> 8) ) ^ ( (uint8_t)(edx >> 8) )); //dh = inn

	if ((eax & 0x100) == 0) goto projection_1;
//this or next point behind projection plane
//r = (1.0-z)/(nz-z)
	fpu_reg11 = 1.0;
	edi->sp_z = fpu_reg11; //dz = 1.0
	fpu_reg11 = fpu_reg11 - ( ebx->sp_z );
	fpu_reg12 = ( esi->sp_z );
	fpu_reg12 = fpu_reg12 - ( ebx->sp_z );
	fpu_reg11 = fpu_reg11 / fpu_reg12; //st = r

	fpu_reg12 = ( ebx->sp_x ); //x' = x + r*(nx-x)
	fpu_reg13 = ( esi->sp_x );
	fpu_reg13 = fpu_reg13 - fpu_reg12;
	fpu_reg13 = fpu_reg13 * fpu_reg11;
	fpu_reg12 = fpu_reg12 + fpu_reg13;
	fpu_reg12 = fpu_reg12 + fpu_reg10; //x = xres/2 *(1 + x')
	fpu_reg12 = fpu_reg12 * xmid;
	edi->sp_x = fpu_reg12;

	fpu_reg12 = ( ebx->sp_y ); //y' = y + r*(ny-y)
	fpu_reg13 = ( esi->sp_y );
	fpu_reg13 = fpu_reg13 - fpu_reg12;
	fpu_reg13 = fpu_reg13 * fpu_reg11;
	fpu_reg12 = fpu_reg12 + fpu_reg13;
	fpu_reg12 = fpu_reg10 - fpu_reg12; //y = yres/2 *(1 - y')
	fpu_reg12 = fpu_reg12 * ymid;
	edi->sp_y = fpu_reg12;

	ecx = vars_1;
projection_l2:
	fpu_reg12 = ( ebx->sp_data[ecx] );
	fpu_reg13 = ( esi->sp_data[ecx] );
	fpu_reg13 = fpu_reg13 - fpu_reg12;
	fpu_reg13 = fpu_reg13 * fpu_reg11;
	fpu_reg12 = fpu_reg12 + fpu_reg13;
	edi->sp_data[ecx] = fpu_reg12;
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) >= 0) goto projection_l2;
//remove r

	edi = (tscreenpoint *)( ((uintptr_t)edi) + spsize ); //one destination point added
//end of if statement
projection_1:
//ns > s?
	if (esi <= ebx) goto projection_notz_l;
	ebx = esi; //is same as ebx += point size
	goto projection_z_l; //repeat loop
projection_notz_l:
	ebx = esi; //is same as ebx += point size

	ebx = _sp_end;
//ebx -> start of s-points
//remove 1.0

	eax = (uintptr_t)edi - (uintptr_t)ebx;
	eax = eax >> ( 1 );

	if (eax <= spsize) goto projection_w;
	eax = 0;
	goto projection_weg;
projection_w:
	eax = ( 1 );

projection_weg:
	_edi = edi;
	_ebx = ebx;
	return eax;
}


static void drawface(uint32_t _eax, tvec *_ebx, uint32_t _edx, tface *esi) {
	realnum fpu_reg10, fpu_reg11, fpu_reg12;
	uint32_t eax = _eax, edx = _edx, ecx;
	tscreenpoint *edi, *eb2;
	uint8_t *ebx = (uint8_t *)_ebx;
	tfp *es2;
	tface *stack_var00;
//-> eax = idx
//-> edx = light value
//-> ebx -> transformed vertices list
//-> esi -> tface
	uint32_t ifl0;
	tmaterial *mat;

	stack_var00 = esi;

	esi->f_idx = eax; //store face index in face
	eax = set_high_byte(eax, ( (uint8_t)fog ));
	eax = ((eax & 0xff000000) >> 24) | ((eax & 0x00ff0000) >> 8) | ((eax & 0x0000ff00) << 8) | ((eax & 0x000000ff) << 24);
	eax = set_high_byte(eax, ( (uint8_t)edx ));
	ifl0 = eax; //index, fog value, light value

	mat = ( esi->f_mat ); //material

//if (offset f_p) gt 0
	es2 = ( &(esi->f_p[0]) );
//endif
	edi = (tscreenpoint *)scene.sc_datapos;
	ecx = ( 3 );
drawface_p_l:
	edx = ( es2->fp_vertex );

	edi->sp_x = ((tvec *)(ebx + edx))->x1;
	edi->sp_y = ((tvec *)(ebx + edx))->x2;
	edi->sp_z = ((tvec *)(ebx + edx))->x3;

	edi->sp_u = es2->fp_u;
	edi->sp_v = es2->fp_v;

	es2 = es2 + ( 1 );
	edi = (tscreenpoint *)( ((uintptr_t)edi) + flatpsize );
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto drawface_p_l;

	eb2 = (tscreenpoint *)scene.sc_datapos;

	drawedflag = ( - 1 );
	eax = projection(( flatpsize ), ( flatvars ), eb2, edi);

	if (( (int32_t)eax ) != 0) goto drawface_0; //less than tree points

	ebx = (uint8_t *)eb2;
	fpu_reg10 = ( ((tscreenpoint *)(ebx + flatpsize * 2))->sp_x ); //lie points clock-wise?
	fpu_reg10 = fpu_reg10 - ( ((tscreenpoint *)ebx)->sp_x );
	fpu_reg11 = ( ((tscreenpoint *)(ebx + flatpsize))->sp_y );
	fpu_reg11 = fpu_reg11 - ( ((tscreenpoint *)ebx)->sp_y );
	fpu_reg10 = fpu_reg10 * fpu_reg11;
	fpu_reg11 = ( ((tscreenpoint *)(ebx + flatpsize * 2))->sp_y );
	fpu_reg11 = fpu_reg11 - ( ((tscreenpoint *)ebx)->sp_y );
	fpu_reg12 = ( ((tscreenpoint *)(ebx + flatpsize))->sp_x );
	fpu_reg12 = fpu_reg12 - ( ((tscreenpoint *)ebx)->sp_x );
	fpu_reg11 = fpu_reg11 * fpu_reg12;
	eb2 = (tscreenpoint *)ebx;



	if (fpu_reg11 <= fpu_reg10) goto drawface_0; //face not visible

	eax = xclip(( flatpsize ), ( flatvars ), eb2, edi); //!

	if (( (int32_t)eax ) != 0) goto drawface_0;
	polygon(eb2, (uintptr_t)edi - (uintptr_t)eb2, mat, ifl0); //edi = (number of points)*(flatpsize)
drawface_0:
	esi = stack_var00; //esi -> tface
	eax = drawedflag;
	esi->f_idx = ( esi->f_idx ) | eax; //face index = -1 if not drawn
	return;
}


static void subdrawface(uint32_t _eax, tvec *_ebx, uint32_t _edx, tface *esi) {
	realnum fpu_reg10, fpu_reg11, fpu_reg12;
	uint32_t eax = _eax, edx = _edx, ecx;
	tscreenpoint *edi, *eb2;
	uint8_t *ebx = (uint8_t *)_ebx;
	tfp *es2;
	tface *stack_var00;
//-> eax = idx
//-> edx = light value
//-> ebx -> transformed vertices list
//-> esi -> tface
	uint32_t ifl0;
	tmaterial *mat;

	stack_var00 = esi;

	esi->f_idx = eax; //store face index in face
	eax = set_high_byte(eax, ( (uint8_t)fog ));
	eax = ((eax & 0xff000000) >> 24) | ((eax & 0x00ff0000) >> 8) | ((eax & 0x0000ff00) << 8) | ((eax & 0x000000ff) << 24);
	eax = set_high_byte(eax, ( (uint8_t)edx ));
	ifl0 = eax; //index, fog value, light value

	mat = ( esi->f_mat ); //material

//if (offset f_p) gt 0
	es2 = ( &(esi->f_p[0]) );
//endif
	edi = (tscreenpoint *)scene.sc_datapos;
	ecx = ( 3 );
subdrawface_p_l:
	edx = ( es2->fp_vertex );

	edi->sp_x = ((tvec *)(ebx + edx))->x1;
	edi->sp_y = ((tvec *)(ebx + edx))->x2;
	edi->sp_z = ((tvec *)(ebx + edx))->x3;

	edi->sp_u = es2->fp_u;
	edi->sp_v = es2->fp_v;

	es2 = es2 + ( 1 );
	edi = (tscreenpoint *)( ((uintptr_t)edi) + flatpsize );
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto subdrawface_p_l;

	eb2 = (tscreenpoint *)scene.sc_datapos;

	drawedflag = ( - 1 );
	eax = projection(( flatpsize ), ( flatvars ), eb2, edi);

	if (( (int32_t)eax ) != 0) goto subdrawface_0; //less than tree points

	ebx = (uint8_t *)eb2;
	fpu_reg10 = ( ((tscreenpoint *)(ebx + flatpsize * 2))->sp_x ); //lie points clock-wise?
	fpu_reg10 = fpu_reg10 - ( ((tscreenpoint *)ebx)->sp_x );
	fpu_reg11 = ( ((tscreenpoint *)(ebx + flatpsize))->sp_y );
	fpu_reg11 = fpu_reg11 - ( ((tscreenpoint *)ebx)->sp_y );
	fpu_reg10 = fpu_reg10 * fpu_reg11;
	fpu_reg11 = ( ((tscreenpoint *)(ebx + flatpsize * 2))->sp_y );
	fpu_reg11 = fpu_reg11 - ( ((tscreenpoint *)ebx)->sp_y );
	fpu_reg12 = ( ((tscreenpoint *)(ebx + flatpsize))->sp_x );
	fpu_reg12 = fpu_reg12 - ( ((tscreenpoint *)ebx)->sp_x );
	fpu_reg11 = fpu_reg11 * fpu_reg12;
	eb2 = (tscreenpoint *)ebx;


	if (fpu_reg11 <= fpu_reg10) goto subdrawface_0; //face not visible

	eax = subxclip(( flatpsize ), ( flatvars ), eb2, edi); //!

	if (( (int32_t)eax ) != 0) goto subdrawface_0;
	subpolygon(eb2, (uintptr_t)edi - (uintptr_t)eb2, mat, ifl0); //edi = (number of points)*(flatpsize)
subdrawface_0:
	esi = stack_var00; //esi -> tface
	eax = drawedflag;
	esi->f_idx = ( esi->f_idx ) | eax; //face index = -1 if not drawn
	return;
}



static uint32_t drawhierarchy(uint32_t idx, tmesh *esi) {
	realnum fpu_reg10, fpu_reg11;
	uint32_t eax, edx, ecx;
	tvec *ebx;
	tface *es2;
	ttempface *es3;
	tmesh *eb2, *stack_var00;
	tvec *stack_var01;
	ttempface *stack_var02;

//-> esi -> top level mesh
//<- eax = idx-count
	uint32_t z, amb;


	if ((( esi->m_object.o_flags ) & ( mfSort )) != 0) goto drawhierarchy_sort;
//draw unsorted
	fpu_reg10 = viewer.v_zmin;
	fpu_reg10 = fpu_reg10 + viewer.v_zmax;
	fpu_reg10 = fpu_reg10 * c_0_5;
	edx = idx;
	insert(edx, fpu_reg10); //insert into z-sorted index list

drawhierarchy_m_l:
	ecx = ( esi->m_faces );
	if (ecx == 0) goto drawhierarchy_m_w;
	z = ecx;
	stack_var00 = esi; //esi -> tmesh

	edx = ( esi->m_object.o_flags ); //edx = o_flags

	eax = ( lvalues - 1 );

	if ((edx & ( mfIllum )) != 0) goto drawhierarchy_amb0;
	eax = ambient;
drawhierarchy_amb0:
	amb = eax;

	ebx = (tvec *)( esi->m_tlist ); //ebx -> transformed vertices list
	es2 = ( esi->m_facelist ); //esi -> tface


	if ((edx & ( mfCorrected )) != 0) goto drawhierarchy_c_l;

drawhierarchy_f_l0: //face loop (affine)
	stack_var01 = ebx;

	eax = idx;
	edx = amb;
	drawface(eax, ebx, edx, es2); //ebx -> transformed vertices list
//esi -> tface (saved)
	ebx = stack_var01;
	es2 = es2 + ( 1 );
	z = ( (int32_t)z ) - 1;
	if (( (int32_t)z ) != 0) goto drawhierarchy_f_l0;
	goto drawhierarchy_c0;
drawhierarchy_c_l: //face loop (corrected)
	stack_var01 = ebx;

	eax = idx;
	edx = amb;
	subdrawface(eax, ebx, edx, es2); //ebx -> transformed vertices list
//esi -> tface (saved)
	ebx = stack_var01;
	es2 = es2 + ( 1 );
	z = ( (int32_t)z ) - 1;
	if (( (int32_t)z ) != 0) goto drawhierarchy_c_l;
drawhierarchy_c0:
	esi = stack_var00;
drawhierarchy_m_w:
	eax = getnextmesh(esi);

	if (( (int32_t)eax ) != 0) goto drawhierarchy_m_l;

	eax = ( 1 ); //unsorted: 1 index used
	goto drawhierarchy_weg;
drawhierarchy_sort: //draw sorted
	fpu_reg10 = viewer.v_zmax;
	fpu_reg11 = viewer.v_zmin;
	fpu_reg11 = fpu_reg11 - fpu_reg10;
	fpu_reg11 = fpu_reg11 * c_0_03125; //1/32
	{ realnum tmp = fpu_reg10; fpu_reg10 = fpu_reg11; fpu_reg11 = tmp; }
	fpu_reg11 = fpu_reg11 + fpu_reg10;

	ecx = ( 15 );
drawhierarchy_i_l: //insert loop
	edx = idx;
	edx = edx + ecx;
	insert(edx, fpu_reg11); //insert into z-sorted index list
	fpu_reg11 = fpu_reg11 + fpu_reg10;
	fpu_reg11 = fpu_reg11 + fpu_reg10;
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) >= 0) goto drawhierarchy_i_l;



	z = ( 15 );
drawhierarchy_a_l: //array loop
	ecx = z;
	es3 = ( viewer.v_tempfaces[ecx] );

drawhierarchy_f_l1: //face loop
//esi -> ttempface
	if (es3 == 0) goto drawhierarchy_f_w;
	stack_var02 = es3;

	eb2 = ( es3->tf_meshptr ); //ebx ->tmesh
	edx = ( lvalues - 1 );

	if ((( eb2->m_object.o_flags ) & ( mfIllum )) != 0) goto drawhierarchy_amb1;
	edx = ambient;
drawhierarchy_amb1:
	ebx = (tvec *)( eb2->m_tlist );
	es2 = ( es3->tf_faceptr );

	eax = idx;
	eax = eax + z;
//        mov     edx,ambient;20h;10h
	drawface(eax, ebx, edx, es2); //esi -> tface

	es3 = stack_var02;
	es3 = ( es3->tf_next );
	goto drawhierarchy_f_l1;
drawhierarchy_f_w:

	z = ( (int32_t)z ) - 1;
	if (( (int32_t)z ) >= 0) goto drawhierarchy_a_l;
	eax = ( 16 ); //sorted: 16 indecies used

drawhierarchy_weg:
	return eax;
}


static void drawwater(tmesh *esi) {
	realnum fpu_reg10, fpu_reg11, fpu_reg12;
	uint32_t eax, edx, ecx;
	tscreenpoint *edi;
	uint8_t *ebx;
	tscreenpoint *eb2;
	tface *es2;
	tfp *es3;
	tmesh *stack_var00;
	uint8_t *stack_var01;
	tface *stack_var02;
//-> esi -> top level mesh
	uint32_t z;
//only unsorted
drawwater_m_l:
	ecx = ( esi->m_faces );

	if (( (int32_t)ecx ) == 0) goto drawwater_m_w;
	z = ecx;
	stack_var00 = esi;
//esi -> tmesh

	ebx = (uint8_t *)( esi->m_tlist ); //ebx -> transformed vertices list
	es2 = ( esi->m_facelist ); //esi -> tface
drawwater_f_l0: //face loop
	stack_var01 = ebx;

//-> ebx -> transformed vertices list
	stack_var02 = es2; //-> esi -> tface

//if (offset f_p) gt 0
	es3 = ( &(es2->f_p[0]) );
//endif
	edi = (tscreenpoint *)scene.sc_datapos;
	ecx = ( 3 );
drawwater_p_l:
	edx = ( es3->fp_vertex );

	edi->sp_x = ((tvec *)(ebx + edx))->x1;
	edi->sp_y = ((tvec *)(ebx + edx))->x2;
	edi->sp_z = ((tvec *)(ebx + edx))->x3;

	edi->sp_u = es3->fp_u;
	edi->sp_v = es3->fp_v;

	es3 = es3 + ( 1 );
	edi = (tscreenpoint *)( ((uintptr_t)edi) + flatpsize );
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto drawwater_p_l;

	eb2 = (tscreenpoint *)scene.sc_datapos;

	eax = projection(( flatpsize ), ( flatvars ), eb2, edi);

	if (( (int32_t)eax ) != 0) goto drawwater_0; //less than tree points

	ebx = (uint8_t *)eb2;
	fpu_reg10 = ( ((tscreenpoint *)(ebx + flatpsize * 2))->sp_x ); //lie points clock-wise?
	fpu_reg10 = fpu_reg10 - ( ((tscreenpoint *)ebx)->sp_x );
	fpu_reg11 = ( ((tscreenpoint *)(ebx + flatpsize))->sp_y );
	fpu_reg11 = fpu_reg11 - ( ((tscreenpoint *)ebx)->sp_y );
	fpu_reg10 = fpu_reg10 * fpu_reg11;
	fpu_reg11 = ( ((tscreenpoint *)(ebx + flatpsize * 2))->sp_y );
	fpu_reg11 = fpu_reg11 - ( ((tscreenpoint *)ebx)->sp_y );
	fpu_reg12 = ( ((tscreenpoint *)(ebx + flatpsize))->sp_x );
	fpu_reg12 = fpu_reg12 - ( ((tscreenpoint *)ebx)->sp_x );
	fpu_reg11 = fpu_reg11 * fpu_reg12;
	eb2 = (tscreenpoint *)ebx;



	if (fpu_reg11 <= fpu_reg10) goto drawwater_0; //face not visible

	eax = xclip(( flatpsize ), ( flatvars ), eb2, edi);

	if (( (int32_t)eax ) != 0) goto drawwater_0;
	wpolygon(eb2, (uintptr_t)edi - (uintptr_t)eb2); //edi = (number of points)*(flatpsize)
drawwater_0:
	es2 = stack_var02; //esi -> tface (saved)

	ebx = stack_var01;
	es2 = es2 + ( 1 );
	z = ( (int32_t)z ) - 1;
	if (( (int32_t)z ) != 0) goto drawwater_f_l0;

	esi = stack_var00;
drawwater_m_w:
	eax = getnextmesh(esi);

	if (( (int32_t)eax ) != 0) goto drawwater_m_l;

	return;
}




static void texturedraw(void) {
	uint32_t eax;
	tviewer *edi;
	ttempcube *esi, *stack_var00;
	tmesh *es2, *stack_var01;
	uint32_t c_idx, m_idx;

	c_idx = ( 255 );
	m_idx = ( 0 );
	esi = (ttempcube *)scene.sc_datapos;
texturedraw_c_l: //cube loop
	esi = esi - ( 1 );

	if (esi < viewer.v_cubelist) goto texturedraw_w; //no more cubes

	eax = c_idx;
	eax = drawcube(eax, esi); //esi -> ttempcube
	c_idx = c_idx - 1;

	stack_var00 = esi;
	es2 = ( esi->tc_meshlist ); //esi -> mesh
texturedraw_h_l:

	if (es2 == 0) goto texturedraw_h_w;

	edi = ( &(viewer) );

	stack_var01 = es2;
	es2 = xformhierarchy(es2, edi); //transform one hierarchy


	if ((( es2->m_object.o_flags ) & ( mfWater )) != 0) goto texturedraw_0;
	eax = drawhierarchy(m_idx, es2);
	m_idx = m_idx + eax;
	goto texturedraw_1;
texturedraw_0:
	drawwater(es2);
texturedraw_1:
	es2 = stack_var01;

	if ((( es2->m_object.o_flags ) & ( mfFlare )) == 0) goto texturedraw_2;
	eax = m_idx;
	flare_idx = eax;
texturedraw_2:
	es2 = ( es2->m_nexttemp );
	goto texturedraw_h_l;
texturedraw_h_w:

	esi = stack_var00; //esi -> ttempcube
	goto texturedraw_c_l; //next cube
texturedraw_w: //  mov     eax,c_idx

	return;
}



//comment #
static uint32_t fclip(tflarepoint *edi) {
	realnum fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13;
	uint32_t ecx;
	ecx = 0;

	fpu_reg10 = 0.0;
//sx bound check
	fpu_reg11 = ( edi->f_sx );



	if (fpu_reg11 > ( (int32_t)xres )) goto fclip_w;
	fpu_reg11 = ( edi[1].f_sx );



	if (fpu_reg11 < fpu_reg10) goto fclip_w;
//sy bound check
	fpu_reg11 = ( edi->f_sy );



	if (fpu_reg11 > ( (int32_t)yres )) goto fclip_w;
	fpu_reg11 = ( edi[1].f_sy );



	if (fpu_reg11 < fpu_reg10) goto fclip_w;

//sx clip
	fpu_reg11 = ( edi->f_sx );



	if (fpu_reg11 > fpu_reg10) goto fclip_0;

	fpu_reg11 = ( edi->f_sx );
	fpu_reg12 = ( edi[1].f_sx );
	fpu_reg12 = fpu_reg11 - fpu_reg12;
	fpu_reg11 = fpu_reg11 / fpu_reg12;

	fpu_reg12 = ( edi->f_mx );
	fpu_reg13 = ( edi[1].f_mx );
	fpu_reg13 = fpu_reg13 - fpu_reg12;
	fpu_reg11 = fpu_reg11 * fpu_reg13;
	fpu_reg11 = fpu_reg11 + fpu_reg12;
	edi->f_mx = fpu_reg11;

	edi->f_sx = fpu_reg10; //sx = 0

fclip_0:
	fpu_reg11 = ( edi[1].f_sx );



	if (fpu_reg11 < ( (int32_t)xres )) goto fclip_1;

	fpu_reg11 = ( edi[1].f_sx );
	fpu_reg12 = ( (int32_t)xres );
	edi[1].f_sx = fpu_reg12; //sx = xres
	fpu_reg12 = fpu_reg12 - fpu_reg11;
	fpu_reg13 = ( edi->f_sx );
	fpu_reg11 = fpu_reg13 - fpu_reg11;
	fpu_reg11 = fpu_reg12 / fpu_reg11;

	fpu_reg12 = ( edi[1].f_mx );
	fpu_reg13 = ( edi->f_mx );
	fpu_reg13 = fpu_reg13 - fpu_reg12;
	fpu_reg11 = fpu_reg11 * fpu_reg13;
	fpu_reg11 = fpu_reg11 + fpu_reg12;
	edi[1].f_mx = fpu_reg11;

fclip_1:
//sy clip
	fpu_reg11 = ( edi->f_sy );



	if (fpu_reg11 > fpu_reg10) goto fclip_2;

	fpu_reg11 = ( edi->f_sy );
	fpu_reg12 = ( edi[1].f_sy );
	fpu_reg12 = fpu_reg11 - fpu_reg12;
	fpu_reg11 = fpu_reg11 / fpu_reg12;

	fpu_reg12 = ( edi->f_my );
	fpu_reg13 = ( edi[1].f_my );
	fpu_reg13 = fpu_reg13 - fpu_reg12;
	fpu_reg11 = fpu_reg11 * fpu_reg13;
	fpu_reg11 = fpu_reg11 + fpu_reg12;
	edi->f_my = fpu_reg11;

	edi->f_sy = fpu_reg10; //sy = 0

fclip_2:
	fpu_reg11 = ( edi[1].f_sy );



	if (fpu_reg11 < ( (int32_t)yres )) goto fclip_3;

	fpu_reg11 = ( edi[1].f_sy );
	fpu_reg12 = ( (int32_t)yres );
	edi[1].f_sy = fpu_reg12; //sy = yres
	fpu_reg12 = fpu_reg12 - fpu_reg11;
	fpu_reg13 = ( edi->f_sy );
	fpu_reg11 = fpu_reg13 - fpu_reg11;
	fpu_reg11 = fpu_reg12 / fpu_reg11;

	fpu_reg12 = ( edi[1].f_my );
	fpu_reg13 = ( edi->f_my );
	fpu_reg13 = fpu_reg13 - fpu_reg12;
	fpu_reg11 = fpu_reg11 * fpu_reg13;
	fpu_reg11 = fpu_reg11 + fpu_reg12;
	edi[1].f_my = fpu_reg11;

fclip_3:

	ecx = ecx + 1; //ecx = 0: not visible

fclip_w:

//fclip_weg:
	return ecx;
}


static void fdraw(uint32_t intensity, tflarepoint *edi) {
	realnum fpu_reg10;
	uint32_t eax, edx, ecx, ebx, ebp;
	float *ea2;
	uint8_t *ed2, *ec2, *esi, *stack_var00;
	unsigned int ec3;

	uint32_t xa, xe, ya, ye;
	uint32_t mx, my, mu, mv;
	uint32_t xcount, ycount;


	fpu_reg10 = ( edi->f_sx );
	xa = (int32_t)ceil(fpu_reg10);
	fpu_reg10 = ( edi[1].f_sx );
	xe = (int32_t)ceil(fpu_reg10);
	fpu_reg10 = ( edi->f_sy );
	ya = (int32_t)ceil(fpu_reg10);
	fpu_reg10 = ( edi[1].f_sy );
	ye = (int32_t)ceil(fpu_reg10);

	ebx = xa;
	ecx = xe;
	ecx = ( (int32_t)ecx ) - ( (int32_t)ebx );
	if (( (int32_t)ecx ) <= 0) goto fdraw_weg;

	eax = ya;
	edx = ye;
	edx = ( (int32_t)edx ) - ( (int32_t)eax );
	if (( (int32_t)edx ) <= 0) goto fdraw_weg;

	eax = ( (int32_t)eax ) * ( (int32_t)xres );
	ebx = ebx + eax;
	ebx = ebx << ( 2 );

	ea2 = (float *)scene.sc_divtab;
	fpu_reg10 = ( edi->f_mx );
	mx = (int32_t)ceil(fpu_reg10);
	fpu_reg10 = ( edi[1].f_mx ) - fpu_reg10;
	fpu_reg10 = fpu_reg10 * ( ea2[ecx] );
	mu = (int32_t)ceil(fpu_reg10);

	fpu_reg10 = ( edi->f_my );
	my = (int32_t)ceil(fpu_reg10);
	fpu_reg10 = ( edi[1].f_my ) - fpu_reg10;
	fpu_reg10 = fpu_reg10 * ( ea2[edx] );
	mv = (int32_t)ceil(fpu_reg10);

	xcount = ecx;
	ycount = edx;

	ed2 = (uint8_t *)scene.sc_buffer;
	ed2 = ed2 + ( 2 );
	ed2 = ed2 + ebx;

	esi = (uint8_t *)scene.sc_flares;
	edx = (uint32_t)( (uint16_t)my );

	intensity = intensity << ( 6 );
	ec2 = (uint8_t *)scene.sc_flaretab;
	ec2 = ec2 + intensity;

	ebx = mu;
//eax = 000x
//ebx = 00Uu
//edx = LLYX
//esi =
//edi = SSSS
fdraw_y_l:
	eax = mx;
	edx = (edx & 0xffffff00) | (uint8_t)(( (uint8_t)(eax >> 8) ));

	stack_var00 = ed2;

	ebp = xcount;
fdraw_x_l:
//      mov     ah,[esi+edx]
	ec3 = ( *((uint8_t *)(esi + edx)) );
	{ uint32_t carry = (UINT8_MAX - ( (uint8_t)eax ) < ( (uint8_t)ebx ))?1:0; eax = (eax & 0xffffff00) | (uint8_t)(( (uint8_t)eax ) + ( (uint8_t)ebx ));
	  edx = (edx & 0xffffff00) | (uint8_t)(( (uint8_t)edx ) + ( (uint8_t)(ebx >> 8) ) + carry); }
	eax = set_high_byte(eax, ( *((uint8_t *)(ec2 + ec3)) ));
	*((uint8_t *)(ed2)) = (uint8_t) (( *((uint8_t *)(ed2)) ) + ( (uint8_t)(eax >> 8) ));
	ed2 = ed2 + ( 4 );
	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto fdraw_x_l;

	eax = mv;
	{ uint32_t carry = (UINT8_MAX - ( (uint8_t)my ) < ( (uint8_t)eax ))?1:0; my = (my & 0xffffff00) | (uint8_t)(( (uint8_t)my ) + ( (uint8_t)eax ));
	  edx = set_high_byte(edx, ( (uint8_t)(edx >> 8) ) + ( (uint8_t)(eax >> 8) ) + carry); }

	ed2 = stack_var00;

	eax = xres;
	eax = eax << ( 2 );
	ed2 = ed2 + eax;
	ycount = ( (int32_t)ycount ) - 1;
	if (( (int32_t)ycount ) != 0) goto fdraw_y_l;

fdraw_weg:
	return;
}


static void flare(void) {
	realnum fpu_reg10, fpu_reg11, fpu_reg12;
	uint32_t eax, edx, ecx, edi, ebx;
	tflarepoint *ed2;
	tviewer *eb2, *esi;
	uint8_t *es2;
	float p[3];
	uint32_t isx, isy;
	float sx, sy, sz;
	float su, sv;
	uint32_t intensity, coscl;

	eb2 = ( &(viewer) );
	esi = ( &(light.lv_viewer) );

	ecx = ( 2 );
flare_l0:
	fpu_reg10 = ( esi->v_p_data[ecx] );
	fpu_reg10 = fpu_reg10 - ( eb2->v_p_data[ecx] );
	p[ecx] = fpu_reg10;
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) >= 0) goto flare_l0;

//distance light to camera
	fpu_reg10 = ( p[0] );
	fpu_reg10 = fpu_reg10 * ( eb2->v_l3_data[0] );
	fpu_reg11 = ( p[1] );
	fpu_reg11 = fpu_reg11 * ( eb2->v_l3_data[1] );
	fpu_reg12 = ( p[2] );
	fpu_reg12 = fpu_reg12 * ( eb2->v_l3_data[2] );
	fpu_reg11 = fpu_reg11 + fpu_reg12;
	fpu_reg10 = fpu_reg10 + fpu_reg11;

	sz = fpu_reg10;
	fpu_reg11 = 1.0;



	if (fpu_reg11 > fpu_reg10) goto flare_weg;

//cos a (angle between camera and light)
	fpu_reg10 = ( eb2->v_l3_data[0] );
	fpu_reg10 = fpu_reg10 * ( esi->v_l3_data[0] );
	fpu_reg11 = ( eb2->v_l3_data[1] );
	fpu_reg11 = fpu_reg11 * ( esi->v_l3_data[1] );
	fpu_reg12 = ( eb2->v_l3_data[2] );
	fpu_reg12 = fpu_reg12 * ( esi->v_l3_data[2] );
	fpu_reg11 = fpu_reg11 + fpu_reg12;
	fpu_reg10 = fpu_reg10 + fpu_reg11;
	fpu_reg10 = -fpu_reg10;

	fpu_reg10 = fpu_reg10 * c_16;
	coscl = (int32_t)ceil(fpu_reg10);

	if (( (int32_t)coscl ) <= ( 0 )) goto flare_weg;

//x pos on screen
	fpu_reg10 = ( p[0] );
	fpu_reg10 = fpu_reg10 * ( eb2->v_l1_data[0] );
	fpu_reg11 = ( p[1] );
	fpu_reg11 = fpu_reg11 * ( eb2->v_l1_data[1] );
	fpu_reg12 = ( p[2] );
	fpu_reg12 = fpu_reg12 * ( eb2->v_l1_data[2] );
	fpu_reg11 = fpu_reg11 + fpu_reg12;
	fpu_reg10 = fpu_reg10 + fpu_reg11;
	fpu_reg10 = fpu_reg10 / sz;
//        fld1
//        faddp   st(1)
	fpu_reg10 = fpu_reg10 * xmid;
	su = fpu_reg10;
	fpu_reg10 = fpu_reg10 + xmid;
	isx = (int32_t)ceil(fpu_reg10);
	sx = fpu_reg10;

//y pos on screen
	fpu_reg10 = ( p[0] );
	fpu_reg10 = fpu_reg10 * ( eb2->v_l2_data[0] );
	fpu_reg11 = ( p[1] );
	fpu_reg11 = fpu_reg11 * ( eb2->v_l2_data[1] );
	fpu_reg12 = ( p[2] );
	fpu_reg12 = fpu_reg12 * ( eb2->v_l2_data[2] );
	fpu_reg11 = fpu_reg11 + fpu_reg12;
	fpu_reg10 = fpu_reg10 + fpu_reg11;
	fpu_reg10 = fpu_reg10 / sz;
//        fld1
//        fsubrp  st(1)
	fpu_reg10 = fpu_reg10 * ymid;
	fpu_reg10 = -fpu_reg10;
	sv = fpu_reg10;
	fpu_reg10 = fpu_reg10 + ymid;
	isy = (int32_t)ceil(fpu_reg10);
	sy = fpu_reg10;


	intensity = ( 0 );

	eax = /*(eax & 0xffffff00) |*/ (uint8_t)(( (uint8_t)flare_idx ));
	ebx = isx;
	ebx = ebx - 1;
	ecx = xres;
	edx = isy;
	edx = ( (int32_t)edx ) * ( (int32_t)xres );
	es2 = (uint8_t *)scene.sc_buffer;
	es2 = es2 + ( 3 );
//if testcode gt 0
// xor ah,ah
//endif

	if (ebx >= ecx) goto flare_0;

	if (edx >= flare_ycmp) goto flare_0;
	edi = ( ebx + edx );
//if testcode gt 0
//   mov [esi+edi*4-3],ah
//endif

	if (( (uint8_t)eax ) <= ( *((uint8_t *)(es2 + edi * 4)) )) goto flare_0;
	intensity = intensity + 1;
flare_0:
	ebx = ebx + flare_dx;


	if (ebx >= ecx) goto flare_1;
	edi = ( ebx + edx );
//if testcode gt 0
//   mov [esi+edi*4-3],ah
//endif

	if (( (uint8_t)eax ) <= ( *((uint8_t *)(es2 + edi * 4)) )) goto flare_1;
	intensity = intensity + 1;
flare_1:
	ebx = ebx - flare_dx;
	edx = edx + flare_dy;


	if (edx >= flare_ycmp) goto flare_2;
	edi = ( ebx + edx );
//if testcode gt 0
//   mov [esi+edi*4-3],ah
//endif

	if (( (uint8_t)eax ) <= ( *((uint8_t *)(es2 + edi * 4)) )) goto flare_2;
	intensity = intensity + 1;
flare_2:
	edx = edx - flare_dy;
	ebx = ebx - flare_dx;


	if (ebx >= ecx) goto flare_3;
	edi = ( ebx + edx );
//if testcode gt 0
//   mov [esi+edi*4-3],ah
//endif

	if (( (uint8_t)eax ) <= ( *((uint8_t *)(es2 + edi * 4)) )) goto flare_3;
	intensity = intensity + 1;
flare_3:
	ebx = ebx + flare_dx;
	edx = edx - flare_dy;


	if (edx >= flare_ycmp) goto flare_4;
	edi = ( ebx + edx );
//if testcode gt 0
//   mov [esi+edi*4-3],ah
//endif

	if (( (uint8_t)eax ) <= ( *((uint8_t *)(es2 + edi * 4)) )) goto flare_4;
	intensity = intensity + 1;
flare_4:

	if (intensity == ( 0 )) goto flare_weg;

	eax = coscl;
	eax = ( (int32_t)eax ) * ( (int32_t)intensity );
	eax = ( (int32_t)eax ) * ( 1000 );
	eax = eax >> ( 16 - 3 );

	if (eax <= ( 8 )) goto flare_5;
	eax = ( 8 );
flare_5:
	intensity = eax;

//flare 1
	ed2 = (tflarepoint *)scene.sc_datapos;
	fpu_reg10 = c_10_7;
	fpu_reg10 = fpu_reg10 / sz;
//screen area
	fpu_reg11 = xmid;
	fpu_reg11 = fpu_reg11 * fpu_reg10; //xmid*16/sz
	fpu_reg12 = fpu_reg11;
	fpu_reg12 = sx - fpu_reg12;
	ed2->f_sx = fpu_reg12;
	fpu_reg11 = fpu_reg11 + sx;
	ed2[1].f_sx = fpu_reg11;
	fpu_reg11 = ymid;
	fpu_reg11 = fpu_reg11 * c_1_33;
	fpu_reg10 = fpu_reg10 * fpu_reg11; //ymid*16/sz
	fpu_reg11 = fpu_reg10;
	fpu_reg11 = sy - fpu_reg11;
	ed2->f_sy = fpu_reg11;
	fpu_reg10 = fpu_reg10 + sy;
	ed2[1].f_sy = fpu_reg10;

//map area                ;(x:128,y:0, x:256,y:128)*256
	fpu_reg10 = c_32768;
	ed2->f_mx = fpu_reg10;
	fpu_reg10 = fpu_reg10 + flare_res; //c_lscale
	ed2[1].f_mx = fpu_reg10;
	fpu_reg10 = 0.0;
	ed2->f_my = fpu_reg10;
	fpu_reg10 = flare_res; //c_lscale
	ed2[1].f_my = fpu_reg10;

	ecx = fclip(ed2);
	if (ecx == 0) goto flare_clip1;
	fdraw(intensity, ed2);
flare_clip1:

//flare 2
	ed2 = (tflarepoint *)scene.sc_datapos;
//screen area
	fpu_reg10 = su;
	fpu_reg10 = fpu_reg10 * flare_2s;
	fpu_reg10 = fpu_reg10 + sx; //xpos
	fpu_reg11 = xmid;
	fpu_reg11 = fpu_reg11 * flare_2x;
	fpu_reg12 = fpu_reg11;
	fpu_reg12 = fpu_reg10 - fpu_reg12;
	ed2->f_sx = fpu_reg12;
	fpu_reg10 = fpu_reg10 + fpu_reg11;
	ed2[1].f_sx = fpu_reg10;

	fpu_reg10 = sv;
	fpu_reg10 = fpu_reg10 * flare_2s;
	fpu_reg10 = fpu_reg10 + sy; //ypos
	fpu_reg11 = ymid;
	fpu_reg11 = fpu_reg11 * flare_2y;
	fpu_reg12 = fpu_reg11;
	fpu_reg12 = fpu_reg10 - fpu_reg12;
	ed2->f_sy = fpu_reg12;
	fpu_reg10 = fpu_reg10 + fpu_reg11;
	ed2[1].f_sy = fpu_reg10;

//map area               ;(x:0,y:0, x:128,y:128)*256
	fpu_reg10 = 0.0;
	ed2->f_mx = fpu_reg10;
	ed2->f_my = fpu_reg10;
	fpu_reg10 = flare_res;
	ed2[1].f_mx = fpu_reg10;
	ed2[1].f_my = fpu_reg10;

	ecx = fclip(ed2);
	if (ecx == 0) goto flare_clip2;
	fdraw(intensity, ed2);
flare_clip2:

//flare 3
	ed2 = (tflarepoint *)scene.sc_datapos;
//screen area
	fpu_reg10 = su;
	fpu_reg10 = fpu_reg10 * flare_3s;
	fpu_reg10 = fpu_reg10 + sx; //xpos
	fpu_reg11 = xmid;
	fpu_reg11 = fpu_reg11 * flare_3x;
	fpu_reg12 = fpu_reg11;
	fpu_reg12 = fpu_reg10 - fpu_reg12;
	ed2->f_sx = fpu_reg12;
	fpu_reg10 = fpu_reg10 + fpu_reg11;
	ed2[1].f_sx = fpu_reg10;

	fpu_reg10 = sv;
	fpu_reg10 = fpu_reg10 * flare_3s;
	fpu_reg10 = fpu_reg10 + sy; //ypos
	fpu_reg11 = ymid;
	fpu_reg11 = fpu_reg11 * flare_3y;
	fpu_reg12 = fpu_reg11;
	fpu_reg12 = fpu_reg10 - fpu_reg12;
	ed2->f_sy = fpu_reg12;
	fpu_reg10 = fpu_reg10 + fpu_reg11;
	ed2[1].f_sy = fpu_reg10;

//map area               ;(x:0,y:0, x:128,y:128)*256
	fpu_reg10 = 0.0;
	ed2->f_mx = fpu_reg10;
	ed2->f_my = fpu_reg10;
	fpu_reg10 = flare_res;
	ed2[1].f_mx = fpu_reg10;
	ed2[1].f_my = fpu_reg10;

	ecx = fclip(ed2);
	if (ecx == 0) goto flare_clip3;
	fdraw(intensity, ed2);
flare_clip3:
//flare 4
	ed2 = (tflarepoint *)scene.sc_datapos;
//screen area
	fpu_reg10 = su;
	fpu_reg10 = fpu_reg10 * flare_4s;
	fpu_reg10 = fpu_reg10 + sx; //xpos
	fpu_reg11 = xmid;
	fpu_reg11 = fpu_reg11 * flare_4x;
	fpu_reg12 = fpu_reg11;
	fpu_reg12 = fpu_reg10 - fpu_reg12;
	ed2->f_sx = fpu_reg12;
	fpu_reg10 = fpu_reg10 + fpu_reg11;
	ed2[1].f_sx = fpu_reg10;

	fpu_reg10 = sv;
	fpu_reg10 = fpu_reg10 * flare_4s;
	fpu_reg10 = fpu_reg10 + sy; //ypos
	fpu_reg11 = ymid;
	fpu_reg11 = fpu_reg11 * flare_4y;
	fpu_reg12 = fpu_reg11;
	fpu_reg12 = fpu_reg10 - fpu_reg12;
	ed2->f_sy = fpu_reg12;
	fpu_reg10 = fpu_reg10 + fpu_reg11;
	ed2[1].f_sy = fpu_reg10;

//map area                ;(x:128,y:0, x:256,y:128)*256
	fpu_reg10 = c_32768;
	ed2->f_mx = fpu_reg10;
	fpu_reg10 = fpu_reg10 + flare_res; //c_lscale
	ed2[1].f_mx = fpu_reg10;
	fpu_reg10 = 0.0;
	ed2->f_my = fpu_reg10;
	fpu_reg10 = flare_res; //c_lscale
	ed2[1].f_my = fpu_reg10;

	ecx = fclip(ed2);
	if (ecx == 0) goto flare_clip4;
	fdraw(intensity, ed2);
flare_clip4:

flare_weg:
	return;
}

//#

static uint32_t lightclip(uint32_t spsize, uint32_t _eax, tscreenpoint *&_ebx, uint32_t _edx, tscreenpoint *&_edi, realnum _fpu_reg9) {
	realnum fpu_reg9 = _fpu_reg9, fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13, fpu_reg14;
	uint32_t eax = _eax, edx = _edx, ecx;
	uint8_t *edi = (uint8_t *)_edi, *ebx = (uint8_t *)_ebx, *esi;
//, vars:dword ;data variables -1
//-> st(0) = abstand
//-> ebx -> sp
//-> edi -> sp_end
//-> edx = offset sp_lx, sp_ly oder sp_lz
//-> +/- flag in bit 31 von eax

//<- ebx -> sp
//<- edi -> sp_end
//<- wenn weniger als 3 punkte, mit jbe wegspringen

	uint8_t *_sp, *_sp_end; //beides pointer

	_sp = ebx; //ebx -> sp[z] (quellpunkte) (z = 0)
	_sp_end = edi; //edi -> sp[d] (zielpunkte)  (d = m)

//@@z_l:
	fpu_reg10 = ( ((tscreenpoint *)ebx)->sp_lz ); //inn = (sp[z].tz >= abstand);
	fpu_reg11 = ( *((float *)(ebx + edx)) );

	if (( (int32_t)eax ) >= 0) goto lightclip_0;
	fpu_reg11 = -fpu_reg11;
lightclip_0: //st(0) = sp[z].tz
	fpu_reg10 = fpu_reg10 + fpu_reg11;

lightclip_z_l:

	ecx = set_high_byte(0 /*ecx*/, ( 1 )); //ch = 1: nicht innerhalb
//abstand


	if (fpu_reg10 < fpu_reg9) goto lightclip__inn;
//punkt innerhalb
	memcpy(edi, ebx, spsize); //sp[d] = sp[z] und d++
	edi = edi + spsize;
	ecx = 0;
lightclip__inn: //ch = 0
	esi = ebx;
	esi = esi + spsize; //nz = z+1
//if (nz >= m) nz = 0;
	if (esi < _sp_end) goto lightclip_wrap;
	esi = _sp;
lightclip_wrap:

	fpu_reg11 = ( ((tscreenpoint *)esi)->sp_lz ); //if (inn ^ (sp[nz].tz >= abstand))
	fpu_reg12 = ( *((float *)(esi + edx)) );

	if (( (int32_t)eax ) >= 0) goto lightclip_1;
	fpu_reg12 = -fpu_reg12;
lightclip_1: //st(0) = sp[nz].tz
	fpu_reg11 = fpu_reg11 + fpu_reg12;

//abstand
	eax = (eax & 0xffff0000) | ((fpu_reg11 < fpu_reg9)?0x100:0);
	eax = set_high_byte(eax, ( (uint8_t)(eax >> 8) ) ^ ( (uint8_t)(ecx >> 8) )); //ch = inn

	if ((eax & 0x100) == 0) goto lightclip_2;
//dieser oder n„chster punkt auáerhalb
//r = (abstand - l[z].tz)/(l[nz].tz-l[z].tz);
	fpu_reg12 = fpu_reg9;
	fpu_reg12 = fpu_reg12 - fpu_reg10; //abstand - l[z].tz
	fpu_reg13 = fpu_reg11;
	fpu_reg13 = fpu_reg13 - fpu_reg10; //l[nz].tz-l[z].tz
	fpu_reg12 = fpu_reg12 / fpu_reg13; //st = r

	ecx = spsize; //vars
//        add     ecx,3
//d[dm] = s[z] + r*(s[nz]-s[z]) fr x, y, z, lx, ly, lz
lightclip_l:
	fpu_reg13 = ( ((tscreenpoint *)(ebx + ecx + - 4))->sp_x );
	fpu_reg14 = ( ((tscreenpoint *)(esi + ecx + - 4))->sp_x );
	fpu_reg14 = fpu_reg14 - fpu_reg13;
	fpu_reg14 = fpu_reg14 * fpu_reg12;
	fpu_reg13 = fpu_reg13 + fpu_reg14;
	((tscreenpoint *)(edi + ecx + - 4))->sp_x = fpu_reg13;
	ecx = ( (int32_t)ecx ) - ( 4 ); //dec     ecx
	if (( (int32_t)ecx ) != 0) goto lightclip_l; //jns     @@l
//r entfernen

	edi = edi + spsize; //d++
lightclip_2:
	{ realnum tmp = fpu_reg10; fpu_reg10 = fpu_reg11; fpu_reg11 = tmp; }


//wz > z?
	if (esi <= ebx) goto lightclip_notz_l;
	ebx = esi; //entspricht ebx += spsize
	goto lightclip_z_l; //z-schleife
lightclip_notz_l:
	ebx = esi; //entspricht ebx += spsize



	ebx = _sp_end; //ebx -> sp
//edi -> sp_end
	eax = (uintptr_t)edi - (uintptr_t)ebx;
	eax = eax >> ( 1 );

	if (eax <= spsize) goto lightclip_w;
	eax = 0;
	goto lightclip_weg;
lightclip_w:
	eax = ( 1 );
lightclip_weg:
	_edi = (tscreenpoint *)edi;
	_ebx = (tscreenpoint *)ebx;
	return eax;
}


static void lscalexy(uint32_t spsize, tscreenpoint *ebx, tscreenpoint *edi) {
	realnum fpu_reg10, fpu_reg11;


	fpu_reg10 = c_lscale; //lx*=128*256 und ly*=128*256
lscalexy_l:
	fpu_reg11 = ( ebx->sp_lx );
	fpu_reg11 = fpu_reg11 * fpu_reg10;
	ebx->sp_lx = fpu_reg11;
	fpu_reg11 = ( ebx->sp_ly );
	fpu_reg11 = fpu_reg11 * fpu_reg10;
	ebx->sp_ly = fpu_reg11;

	ebx = (tscreenpoint *)( ((uintptr_t)ebx) + spsize );

	if (ebx < edi) goto lscalexy_l;


//lscalexy_weg:
	return;
}


static tscreenpoint *lconvert(tscreenpoint *ebx, tscreenpoint *edi) { //light points -> flat points (projiziert die licht-koordinaten)
	realnum fpu_reg10, fpu_reg11;
	float eax;
	tscreenpoint *edx, *esi;
//-> ebx -> sp
//-> edi -> sp_end

	edx = edi;
	edi = ebx; //tscreenpoint muá krzer sein als tlightpoint
	esi = ebx;
lconvert_l:
	fpu_reg10 = ( esi->sp_lz );
	fpu_reg11 = ( esi->sp_lx );
	fpu_reg11 = fpu_reg11 / fpu_reg10;
	edi->sp_x = esi->sp_x;
	eax = ( esi->sp_y );
	edi->sp_u = fpu_reg11;
	fpu_reg10 = ( esi->sp_ly ) / fpu_reg10;
	edi->sp_y = eax;
	esi = (tscreenpoint *)( ((uintptr_t)esi) + lightpsize );
	edi->sp_v = fpu_reg10;
	edi = (tscreenpoint *)( ((uintptr_t)edi) + flatpsize );

	if (esi < edx) goto lconvert_l;

//lconvert_weg:
	return edi;
}


static tscreenpoint *glconvert(tscreenpoint *ebx, tscreenpoint *edi) { //light points -> flat points (projiziert die licht-koordinaten)
	realnum fpu_reg10, fpu_reg11;
	tscreenpoint *edx, *esi;
//-> ebx -> sp
//-> edi -> sp_end

	edx = edi;
	edi = ebx; //tscreenpoint muá krzer sein als tlightpoint
	esi = ebx;
glconvert_l:
	fpu_reg10 = ( esi->sp_lz );
	fpu_reg11 = ( esi->sp_lx );
	fpu_reg11 = fpu_reg11 / fpu_reg10;
	edi->sp_x = esi->sp_x;
	edi->sp_y = esi->sp_y;
	edi->sp_u = fpu_reg11;
	fpu_reg10 = ( esi->sp_ly ) / fpu_reg10;
	edi->sp_l = esi->sp_ll;
	esi = (tscreenpoint *)( ((uintptr_t)esi) + glightpsize );
	edi->sp_v = fpu_reg10;
	edi = (tscreenpoint *)( ((uintptr_t)edi) + gouraudpsize );

	if (esi < edx) goto glconvert_l;

//glconvert_weg:
	return edi;
}


static void lpolygon(tscreenpoint *_sp, uint32_t sp_end, uint32_t idx) { //licht-polygon
	realnum fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13;
	uint32_t eax, ecx, ebx, esi, ebp;
	float *ea2;
	uint8_t *edx, *edi, *es2;
	uint32_t *ed2;
	uint32_t ed3, stack_var00;
//mapptr, zval : dword
//_sp = *sp, zeiger auf screenpoints

	uint32_t pend, y, lb, lc, rb, rc; //int
	uint32_t *x_y;
	float lx, ldx, lu, ldu, lv, ldv;
	float rx, rdx, ru, rdu, rv, rdv;
	uint32_t xa, xe, txt_x, txt_y; //int


	eax = 0; //start- und endpunkt bestimmen
	lb = eax;
	rb = eax;
	pend = eax;

	es2 = (uint8_t *)_sp; //esi -> screenpoints
	fpu_reg10 = ( ((tscreenpoint *)es2)->sp_y ); //st(0) = ymax
	fpu_reg11 = fpu_reg10; //st(1) = ymin

	ecx = sp_end;
	ecx = ecx - ( flatpsize ); //esi+ecx -> sp[sp_end-1]
lpolygon_max_l:

	fpu_reg12 = ( ((tscreenpoint *)(es2 + ecx))->sp_y );
//ymax              ;gr”áten y-wert finden (endpunkt)


	if (fpu_reg12 < fpu_reg11) goto lpolygon_max;
	pend = ecx;
	fpu_reg11 = fpu_reg12; //ymax
	goto lpolygon_min;
lpolygon_max:
//ymin              ;kleinsten y-wert finden (startpunkt)


	if (fpu_reg12 > fpu_reg10) goto lpolygon_min;
	lb = ecx;
	rb = ecx;
	fpu_reg10 = fpu_reg12; //ymin
lpolygon_min:

	ecx = ( (int32_t)ecx ) - ( flatpsize );
	if (( (int32_t)ecx ) != 0) goto lpolygon_max_l;
//ymax entfernen

	y = (int32_t)ceil(fpu_reg10); //y = ceil(ymin)

	eax = y;

//y-clipping
	if (( (int32_t)eax ) >= ( (int32_t)yres )) goto lpolygon_weg;


	if (( (int32_t)eax ) > 0) goto lpolygon_y0;
	y = ( 0 );
	eax = 0;
lpolygon_y0:
	eax = ( (int32_t)eax ) * ( (int32_t)xres );
	x_y = eax + (uint32_t *)scene.sc_buffer;

	lc = ( 1 );
	rc = ( 1 );

lpolygon_y_l: //y-schleife

//links
	lc = ( (int32_t)lc ) - 1;
	if (( (int32_t)lc ) != 0) goto lpolygon_l_nz;
	edi = (uint8_t *)_sp;
	esi = lb;
lpolygon_lc:

	if (esi == pend) goto lpolygon_fertig; //lb == pend -> unten angekommen
	ebx = esi; //ia = lb
	{ uint32_t carry = (esi < ( flatpsize ))?1:0; esi = esi - ( flatpsize ); //lb--
	  if (carry == 0) goto lpolygon_l0; } //wrap
	esi = esi + sp_end;
lpolygon_l0:
	fpu_reg10 = ( ((tscreenpoint *)(esi + edi))->sp_y ); //lc = ceil(sp[lb].sy) - y
	lc = (int32_t)ceil(fpu_reg10);
	eax = y;
	lc = ( (int32_t)lc ) - ( (int32_t)eax );
	if (( (int32_t)lc ) <= 0) goto lpolygon_lc; //while lc <= 0
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
	txt_x = (int32_t)ceil(fpu_reg12);
	lu = fpu_reg12;

//ldv = (sp[lb].v - sp[ia].v)/(sp[lb].y - sp[ia].y);
	fpu_reg12 = ( ((tscreenpoint *)(edi + esi))->sp_v );
	fpu_reg12 = fpu_reg12 - ( ((tscreenpoint *)(edi + ebx))->sp_v );
	fpu_reg12 = fpu_reg12 / fpu_reg11;
	ldv = fpu_reg12;

//lv = ldv  *(y  - sp[ia].y) + sp[ia].v;
	fpu_reg12 = fpu_reg12 * fpu_reg10;
	fpu_reg12 = fpu_reg12 + ( ((tscreenpoint *)(edi + ebx))->sp_v );
	txt_y = (int32_t)ceil(fpu_reg12);
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

	goto lpolygon_l_z;
lpolygon_l_nz:
	fpu_reg10 = lu;
	fpu_reg10 = fpu_reg10 + ldu;
	txt_x = (int32_t)ceil(fpu_reg10);
	lu = fpu_reg10;

	fpu_reg10 = lv;
	fpu_reg10 = fpu_reg10 + ldv;
	txt_y = (int32_t)ceil(fpu_reg10);
	lv = fpu_reg10;

	fpu_reg10 = lx;
	fpu_reg10 = fpu_reg10 + ldx; //lx bleibt im copro
	lx = fpu_reg10;
lpolygon_l_z:
//rechts
	rc = ( (int32_t)rc ) - 1;
	if (( (int32_t)rc ) != 0) goto lpolygon_r_nz;
	edi = (uint8_t *)_sp;
	esi = rb;
lpolygon_rc:

	if (esi == pend) goto lpolygon_fertig2; //rb == pend -> unten angekommen
	ebx = esi; //ia = rb
	esi = esi + ( flatpsize ); //rb++

	if (esi < sp_end) goto lpolygon_r0; //wrap
	esi = 0;
lpolygon_r0:
	fpu_reg11 = ( ((tscreenpoint *)(esi + edi))->sp_y ); //rc = ceil(sp[rb].sy) - y
	rc = (int32_t)ceil(fpu_reg11);
	eax = y;
	rc = ( (int32_t)rc ) - ( (int32_t)eax );
	if (( (int32_t)rc ) <= 0) goto lpolygon_rc; //while lc <= 0
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

	goto lpolygon_r_z;
lpolygon_r_nz:
	fpu_reg11 = ru;
	fpu_reg11 = fpu_reg11 + rdu;
	ru = fpu_reg11;

	fpu_reg11 = rv;
	fpu_reg11 = fpu_reg11 + rdv;
	rv = fpu_reg11;

	fpu_reg11 = rx;
	fpu_reg11 = fpu_reg11 + rdx; //rx bleibt im copro
	rx = fpu_reg11;
lpolygon_r_z:

	xe = (int32_t)ceil(fpu_reg11); //xe = ceil(rx) (rx entfernen)
	xa = (int32_t)ceil(fpu_reg10); //xa = ceil(lx) (lx entfernen)

//eax = 00iU
//ebx = x0Yy
//ecx = 000?
//edx = TT?X
//esi = CCCC
//edi = PPPP
//ebp = u0Vv

	esi = xe;
	esi = ( (int32_t)esi ) - ( (int32_t)xa ); //esi = CCCC
	if (( (int32_t)esi ) <= 0) goto lpolygon_w;
	ed2 = xa + x_y; //edi = PPPP

	ebx = (uint32_t)( (uint16_t)txt_x ); //ebx = 00Xx
	edx = (uint8_t *)scene.sc_lmap; //edx = TT00
	ed3 = /*(ed3 & 0xffffff00) |*/ (uint8_t)(( (uint8_t)(ebx >> 8) )); //edx = TT?X
	ebx = ebx << ( 24 ); //ebx = x000
	ebx = (ebx & 0xffff0000) | (uint16_t)(( (uint16_t)txt_y )); //ebx = x0Yy
	ed3 = /*(ed3 & 0xffffff00) |*/ (uint8_t)(( (uint8_t)ed3 ) + ( 0x80 ));
	ebx = set_high_byte(ebx, ( (uint8_t)(ebx >> 8) ) + ( 0x80 ));

	ea2 = (float *)scene.sc_divtab;
	fpu_reg10 = ru;
	fpu_reg10 = fpu_reg10 - lu;
	fpu_reg10 = fpu_reg10 * ( ea2[esi] );
	txt_x = (int32_t)ceil(fpu_reg10);
	fpu_reg10 = rv;
	fpu_reg10 = fpu_reg10 - lv;
	fpu_reg10 = fpu_reg10 * ( ea2[esi] );
	txt_y = (int32_t)ceil(fpu_reg10);

	ecx = (uint32_t)( (uint16_t)txt_x ); //ecx = 00Uu
	eax = /*(eax & 0xffffff00) |*/ (uint8_t)(( (uint8_t)(ecx >> 8) )); //eax = 000U
	eax = set_high_byte(eax, ( (uint8_t)idx )); //eax = 00iU
	ecx = ecx << ( 24 ); //ecx = u000
	ecx = (ecx & 0xffff0000) | (uint16_t)(( (uint16_t)txt_y )); //ecx = u0Vv

	stack_var00 = ( 0 /*ebp*/ );
	ebp = ecx;

//keine lokalen variablen mehr zug„nglich
lpolygon_inner:
	ed3 = set_high_byte(ed3, ( (uint8_t)(ebx >> 8) ));
//schreiberlaubnis
	if (( (uint8_t)(eax >> 8) ) != ( *(((uint8_t *)ed2) + (3)) )) goto lpolygon_i0;
	ecx = (ecx & 0xffffff00) | (uint8_t)(( *((uint8_t *)(edx + ed3)) )); //cl = helligkeit
	{ uint32_t carry = (UINT32_MAX - ebx < ebp)?1:0; ebx = ebx + ebp;
	  ed3 = (ed3 & 0xffffff00) | (uint8_t)(( (uint8_t)ed3 ) + ( (uint8_t)eax ) + carry); }
	*(((uint8_t *)ed2) + (1)) = (uint8_t) (( *(((uint8_t *)ed2) + (1)) ) + ( (uint8_t)ecx )); //+2
	ed2 = ed2 + 1;
	esi = ( (int32_t)esi ) - 1;
	if (( (int32_t)esi ) == 0) goto lpolygon_iw;
	goto lpolygon_inner;
lpolygon_i0:
	{ uint32_t carry = (UINT32_MAX - ebx < ebp)?1:0; ebx = ebx + ebp;
	  ed3 = (ed3 & 0xffffff00) | (uint8_t)(( (uint8_t)ed3 ) + ( (uint8_t)eax ) + carry); }
	ed2 = ed2 + 1;
	esi = ( (int32_t)esi ) - 1;
	if (( (int32_t)esi ) != 0) goto lpolygon_inner;
lpolygon_iw:
	ebp = stack_var00;
lpolygon_w:

	eax = xres;
	x_y = x_y + eax;
	y = y + 1;
	eax = yres;
//while (y < yres)
	if (( (int32_t)eax ) > ( (int32_t)y )) goto lpolygon_y_l;

	goto lpolygon_fertig;
lpolygon_fertig2:

lpolygon_fertig:

lpolygon_weg:
	return;
}


static void glpolygon(tscreenpoint *_sp, uint32_t sp_end, uint32_t idx) { //licht-polygon
	realnum fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13;
	uint32_t eax, ecx, ebx, esi, ebp;
	float *ea2;
	uint8_t *edx, *edi, *es2;
	uint32_t *ed2;
	uint32_t ed3, stack_var00;
//mapptr, idx : dword
//_sp = *sp, zeiger auf screenpoints

	uint32_t pend, y, lb, lc, rb, rc; //int
	uint32_t *x_y;
	float lx, ldx, lu, ldu, lv, ldv, ll, ldl;
	float rx, rdx, ru, rdu, rv, rdv, rl, rdl;
	uint32_t xa, xe, txt_x, txt_y, txt_l; //int


	eax = 0; //start- und endpunkt bestimmen
	lb = eax;
	rb = eax;
	pend = eax;

	es2 = (uint8_t *)_sp; //esi -> screenpoints
	fpu_reg10 = ( ((tscreenpoint *)es2)->sp_y ); //st(0) = ymax
	fpu_reg11 = fpu_reg10; //st(1) = ymin

	ecx = sp_end;
	ecx = ecx - ( gouraudpsize ); //esi+ecx -> sp[sp_end-1]
glpolygon_max_l:

	fpu_reg12 = ( ((tscreenpoint *)(es2 + ecx))->sp_y );
//ymax              ;gr”áten y-wert finden (endpunkt)


	if (fpu_reg12 < fpu_reg11) goto glpolygon_max;
	pend = ecx;
	fpu_reg11 = fpu_reg12; //ymax
	goto glpolygon_min;
glpolygon_max:
//ymin              ;kleinsten y-wert finden (startpunkt)


	if (fpu_reg12 > fpu_reg10) goto glpolygon_min;
	lb = ecx;
	rb = ecx;
	fpu_reg10 = fpu_reg12; //ymin
glpolygon_min:

	ecx = ( (int32_t)ecx ) - ( gouraudpsize );
	if (( (int32_t)ecx ) != 0) goto glpolygon_max_l;
//ymax entfernen

	y = (int32_t)ceil(fpu_reg10); //y = ceil(ymin)

	eax = y;

//y-clipping
	if (( (int32_t)eax ) >= ( (int32_t)yres )) goto glpolygon_weg;


	if (( (int32_t)eax ) > 0) goto glpolygon_y0;
	y = ( 0 );
	eax = 0;
glpolygon_y0:
	eax = ( (int32_t)eax ) * ( (int32_t)xres );
	x_y = eax + (uint32_t *)scene.sc_buffer;

	lc = ( 1 );
	rc = ( 1 );

glpolygon_y_l: //y-schleife

//links
	lc = ( (int32_t)lc ) - 1;
	if (( (int32_t)lc ) != 0) goto glpolygon_l_nz;
	edi = (uint8_t *)_sp;
	esi = lb;
glpolygon_lc:

	if (esi == pend) goto glpolygon_fertig; //lb == pend -> unten angekommen
	ebx = esi; //ia = lb
	{ uint32_t carry = (esi < ( gouraudpsize ))?1:0; esi = esi - ( gouraudpsize ); //lb--
	  if (carry == 0) goto glpolygon_l0; } //wrap
	esi = esi + sp_end;
glpolygon_l0:
	fpu_reg10 = ( ((tscreenpoint *)(esi + edi))->sp_y ); //lc = ceil(sp[lb].sy) - y
	lc = (int32_t)ceil(fpu_reg10);
	eax = y;
	lc = ( (int32_t)lc ) - ( (int32_t)eax );
	if (( (int32_t)lc ) <= 0) goto glpolygon_lc; //while lc <= 0
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
	txt_x = (int32_t)ceil(fpu_reg12);
	lu = fpu_reg12;

//ldv = (sp[lb].v - sp[ia].v)/(sp[lb].y - sp[ia].y);
	fpu_reg12 = ( ((tscreenpoint *)(edi + esi))->sp_v );
	fpu_reg12 = fpu_reg12 - ( ((tscreenpoint *)(edi + ebx))->sp_v );
	fpu_reg12 = fpu_reg12 / fpu_reg11;
	ldv = fpu_reg12;

//lv = ldv  *(y  - sp[ia].y) + sp[ia].v;
	fpu_reg12 = fpu_reg12 * fpu_reg10;
	fpu_reg12 = fpu_reg12 + ( ((tscreenpoint *)(edi + ebx))->sp_v );
	txt_y = (int32_t)ceil(fpu_reg12);
	lv = fpu_reg12;

//ldl = (sp[lb].l - sp[ia].l)/(sp[lb].y - sp[ia].y);
	fpu_reg12 = ( ((tscreenpoint *)(edi + esi))->sp_l );
	fpu_reg12 = fpu_reg12 - ( ((tscreenpoint *)(edi + ebx))->sp_l );
	fpu_reg12 = fpu_reg12 / fpu_reg11;
	ldl = fpu_reg12;

//ll = ldl  *(y  - sp[ia].y) + sp[ia].l;
	fpu_reg12 = fpu_reg12 * fpu_reg10;
	fpu_reg12 = fpu_reg12 + ( ((tscreenpoint *)(edi + ebx))->sp_l );
	txt_l = (int32_t)ceil(fpu_reg12);
	ll = fpu_reg12;

//ldx = (sp[lb].x - sp[ia].x)/(sp[lb].y - sp[ia].y);
	fpu_reg12 = ( ((tscreenpoint *)(edi + esi))->sp_x );
	fpu_reg12 = fpu_reg12 - ( ((tscreenpoint *)(edi + ebx))->sp_x );
	fpu_reg11 = fpu_reg12 / fpu_reg11;
	ldx = fpu_reg11;

//lx = ldx  *(y  - sp[ia].y) + sp[ia].x;
	fpu_reg10 = fpu_reg10 * fpu_reg11;
	fpu_reg10 = fpu_reg10 + ( ((tscreenpoint *)(edi + ebx))->sp_x );
	lx = fpu_reg10;

	goto glpolygon_l_z;
glpolygon_l_nz:
	fpu_reg10 = lu;
	fpu_reg10 = fpu_reg10 + ldu;
	txt_x = (int32_t)ceil(fpu_reg10);
	lu = fpu_reg10;

	fpu_reg10 = lv;
	fpu_reg10 = fpu_reg10 + ldv;
	txt_y = (int32_t)ceil(fpu_reg10);
	lv = fpu_reg10;

	fpu_reg10 = ll;
	fpu_reg10 = fpu_reg10 + ldl;
	txt_l = (int32_t)ceil(fpu_reg10);
	ll = fpu_reg10;

	fpu_reg10 = lx;
	fpu_reg10 = fpu_reg10 + ldx; //lx bleibt im copro
	lx = fpu_reg10;
glpolygon_l_z:
//rechts
	rc = ( (int32_t)rc ) - 1;
	if (( (int32_t)rc ) != 0) goto glpolygon_r_nz;
	edi = (uint8_t *)_sp;
	esi = rb;
glpolygon_rc:

	if (esi == pend) goto glpolygon_fertig2; //rb == pend -> unten angekommen
	ebx = esi; //ia = rb
	esi = esi + ( gouraudpsize ); //rb++

	if (esi < sp_end) goto glpolygon_r0; //wrap
	esi = 0;
glpolygon_r0:
	fpu_reg11 = ( ((tscreenpoint *)(esi + edi))->sp_y ); //rc = ceil(sp[rb].sy) - y
	rc = (int32_t)ceil(fpu_reg11);
	eax = y;
	rc = ( (int32_t)rc ) - ( (int32_t)eax );
	if (( (int32_t)rc ) <= 0) goto glpolygon_rc; //while lc <= 0
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

//rdl = (sp[rb].l - sp[ia].l)/(sp[rb].y - sp[ia].y);
	fpu_reg13 = ( ((tscreenpoint *)(edi + esi))->sp_l );
	fpu_reg13 = fpu_reg13 - ( ((tscreenpoint *)(edi + ebx))->sp_l );
	fpu_reg13 = fpu_reg13 / fpu_reg12;
	rdl = fpu_reg13;

//rl = rdl  *(y  - sp[ia].y) + sp[ia].l;
	fpu_reg13 = fpu_reg13 * fpu_reg11;
	fpu_reg13 = fpu_reg13 + ( ((tscreenpoint *)(edi + ebx))->sp_l );
	rl = fpu_reg13;

//rdx = (sp[rb].x - sp[ia].x)/(sp[rb].y - sp[ia].y);
	fpu_reg13 = ( ((tscreenpoint *)(edi + esi))->sp_x );
	fpu_reg13 = fpu_reg13 - ( ((tscreenpoint *)(edi + ebx))->sp_x );
	fpu_reg12 = fpu_reg13 / fpu_reg12;
	rdx = fpu_reg12;

//rx = rdx  *(y  - sp[ia].y) + sp[ia].x;
	fpu_reg11 = fpu_reg11 * fpu_reg12;
	fpu_reg11 = fpu_reg11 + ( ((tscreenpoint *)(edi + ebx))->sp_x );
	rx = fpu_reg11;

	goto glpolygon_r_z;
glpolygon_r_nz:
	fpu_reg11 = ru;
	fpu_reg11 = fpu_reg11 + rdu;
	ru = fpu_reg11;

	fpu_reg11 = rv;
	fpu_reg11 = fpu_reg11 + rdv;
	rv = fpu_reg11;

	fpu_reg11 = rl;
	fpu_reg11 = fpu_reg11 + rdl;
	rl = fpu_reg11;

	fpu_reg11 = rx;
	fpu_reg11 = fpu_reg11 + rdx; //rx bleibt im copro
	rx = fpu_reg11;
glpolygon_r_z:

	xe = (int32_t)ceil(fpu_reg11); //xe = ceil(rx) (rx entfernen)
	xa = (int32_t)ceil(fpu_reg10); //xa = ceil(lx) (lx entfernen)

//eax = WwiU
//ebx = x0Yy
//ecx = Ll0X
//edx = TT??
//esi = CCCC
//edi = PPPP
//ebp = u0Vv

	esi = xe;
	esi = ( (int32_t)esi ) - ( (int32_t)xa ); //esi = CCCC
	if (( (int32_t)esi ) <= 0) goto glpolygon_w;
	ed2 = xa + x_y; //edi = PPPP

	ebx = (uint32_t)( (uint16_t)txt_x ); //ebx = 00Xx
	ecx = (uint32_t)( (uint16_t)txt_l ); //ecx = 00Ll
	ebx = set_high_byte(ebx, ( (uint8_t)(ebx >> 8) ) + ( 0x80 ));
	ecx = ecx << ( 16 ); //ecx = Ll00
	ecx = (ecx & 0xffffff00) | (uint8_t)(( (uint8_t)(ebx >> 8) )); //ecx = Ll0X
	ebx = ebx << ( 24 ); //ebx = x000
	ebx = (ebx & 0xffff0000) | (uint16_t)(( (uint16_t)txt_y )); //ebx = x0Yy
	ebx = set_high_byte(ebx, ( (uint8_t)(ebx >> 8) ) + ( 0x80 ));


	ea2 = (float *)scene.sc_divtab;
	fpu_reg10 = ru;
	fpu_reg10 = fpu_reg10 - lu;
	fpu_reg10 = fpu_reg10 * ( ea2[esi] );
	txt_x = (int32_t)ceil(fpu_reg10);
	fpu_reg10 = rv;
	fpu_reg10 = fpu_reg10 - lv;
	fpu_reg10 = fpu_reg10 * ( ea2[esi] );
	txt_y = (int32_t)ceil(fpu_reg10);
	fpu_reg10 = rl;
	fpu_reg10 = fpu_reg10 - ll;
	fpu_reg10 = fpu_reg10 * ( ea2[esi] );
	txt_l = (int32_t)ceil(fpu_reg10);

	eax = (uint32_t)( (uint16_t)txt_l ); //eax = 00Ww
	eax = eax << ( 16 ); //eax = Ww00
	ed3 = (uint32_t)( (uint16_t)txt_x ); //edx = 00Uu
	eax = set_high_byte(eax, ( (uint8_t)idx )); //eax = Wwi0
	eax = (eax & 0xffffff00) | (uint8_t)(( (uint8_t)(ed3 >> 8) )); //eax = WwiU
	ed3 = ed3 << ( 24 ); //edx = u000
	ed3 = (ed3 & 0xffff0000) | (uint16_t)(( (uint16_t)txt_y )); //edx = u0Vv

	stack_var00 = ( 0 /*ebp*/ ); //ebp : lokale variablen
	ebp = ed3; //ebp = u0Vv
	edx = (uint8_t *)scene.sc_lmap; //edx = TT??


glpolygon_inner:
//schreiberlaubnis
	if (( (uint8_t)(eax >> 8) ) != ( *(((uint8_t *)ed2) + (3)) )) goto glpolygon_i0;

	ed3 = /*(ed3 & 0xffffff00) |*/ (uint8_t)(( (uint8_t)ecx ));
	ed3 = set_high_byte(ed3, ( (uint8_t)(ebx >> 8) ));
	ecx = ((ecx & 0xff000000) >> 24) | ((ecx & 0x00ff0000) >> 8) | ((ecx & 0x0000ff00) << 8) | ((ecx & 0x000000ff) << 24);
	ed3 = set_high_byte(ed3, ( *((uint8_t *)(edx + ed3)) )); //dl = helligkeit
	ed3 = (ed3 & 0xffffff00) | (uint8_t)(( (uint8_t)ecx ));
	ecx = ((ecx & 0xff000000) >> 24) | ((ecx & 0x00ff0000) >> 8) | ((ecx & 0x0000ff00) << 8) | ((ecx & 0x000000ff) << 24);
	ed3 = (ed3 & 0xffffff00) | (uint8_t)(( ((uint8_t *)(edx + ed3))[65536] )); //scene.lmap + 65536 -> gouraud table
	{ uint32_t carry = (UINT32_MAX - ebx < ebp)?1:0; ebx = ebx + ebp;
	  ecx = ecx + eax + carry; }
	*(((uint8_t *)ed2) + (1)) = (uint8_t) (( *(((uint8_t *)ed2) + (1)) ) + ( (uint8_t)ed3 ));
	ed2 = ed2 + 1;
	esi = ( (int32_t)esi ) - 1;
	if (( (int32_t)esi ) == 0) goto glpolygon_iw;
	goto glpolygon_inner;
glpolygon_i0:
	{ uint32_t carry = (UINT32_MAX - ebx < ebp)?1:0; ebx = ebx + ebp;
	  ecx = ecx + eax + carry; }
	ed2 = ed2 + 1;
	esi = ( (int32_t)esi ) - 1;
	if (( (int32_t)esi ) != 0) goto glpolygon_inner;
glpolygon_iw:

	ebp = stack_var00;
glpolygon_w:

	eax = xres;
	x_y = x_y + eax;
	y = y + 1;
	eax = yres;
//while (y < yres)
	if (( (int32_t)eax ) > ( (int32_t)y )) goto glpolygon_y_l;

	goto glpolygon_fertig;
glpolygon_fertig2:

glpolygon_fertig:

glpolygon_weg:
	return;
}



static void sublpolygon(tscreenpoint *_sp, uint32_t sp_end, uint32_t zval) { //licht-polygon
	realnum fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13, fpu_reg14, fpu_reg15, fpu_reg16, fpu_reg17;
	uint32_t eax, edx, ecx, ebx, esi, ebp;
	uint8_t *edi, *es2;
	uint32_t *ed2;
	uint32_t stack_var00, stack_var01, stack_var02;
//mapptr, zval : dword
//_sp = *sp, zeiger auf screenpoints

	uint32_t pend, y, lb, lc, rb, rc; //int
	uint32_t *x_y;
	float lx, ldx, rx, rdx; //float
	uint32_t xa, xe, txt_x, txt_y; //int
	tlscan la, lu, ra, ru, a, u;


	eax = 0; //start- und endpunkt bestimmen
	lb = eax;
	rb = eax;
	pend = eax;

	es2 = (uint8_t *)_sp; //esi -> screenpoints
	fpu_reg10 = ( ((tscreenpoint *)es2)->sp_y ); //st(0) = ymax
	fpu_reg11 = fpu_reg10; //st(1) = ymin

	ecx = sp_end;
	ecx = ecx - ( lightpsize ); //esi+ecx -> lp[sp_end-1]
sublpolygon_max_l:

	fpu_reg12 = ( ((tscreenpoint *)(es2 + ecx))->sp_y );
//ymax              ;gr”áten y-wert finden (endpunkt)


	if (fpu_reg12 < fpu_reg11) goto sublpolygon_max;
	pend = ecx;
	fpu_reg11 = fpu_reg12; //ymax
	goto sublpolygon_min;
sublpolygon_max:
//ymin              ;kleinsten y-wert finden (startpunkt)


	if (fpu_reg12 > fpu_reg10) goto sublpolygon_min;
	lb = ecx;
	rb = ecx;
	fpu_reg10 = fpu_reg12; //ymin
sublpolygon_min:

	ecx = ( (int32_t)ecx ) - ( lightpsize );
	if (( (int32_t)ecx ) != 0) goto sublpolygon_max_l;
//ymax entfernen

	y = (int32_t)ceil(fpu_reg10); //y = ceil(ymin)

	eax = y;

//y-clipping
	if (( (int32_t)eax ) >= ( (int32_t)yres )) goto sublpolygon_weg;


	if (( (int32_t)eax ) > 0) goto sublpolygon_y0;
	y = ( 0 );
	eax = 0;
sublpolygon_y0:
	eax = ( (int32_t)eax ) * ( (int32_t)xres );
	x_y = eax + (uint32_t *)scene.sc_buffer;

	lc = ( 1 );
	rc = ( 1 );

	fpu_reg10 = c_32; //c_16                    ;st(0) = 16

sublpolygon_y_l: //y-schleife

//links
	lc = ( (int32_t)lc ) - 1;
	if (( (int32_t)lc ) != 0) goto sublpolygon_l_nz;
	edi = (uint8_t *)_sp;
	esi = lb;
sublpolygon_lc:

	if (esi == pend) goto sublpolygon_fertig; //lb == pend -> unten angekommen
	ebx = esi; //ia = lb
	{ uint32_t carry = (esi < ( lightpsize ))?1:0; esi = esi - ( lightpsize ); //lb--
	  if (carry == 0) goto sublpolygon_l0; } //wrap
	esi = esi + sp_end;
sublpolygon_l0:
	fpu_reg11 = ( ((tscreenpoint *)(esi + edi))->sp_y ); //lc = ceil(lp[lb].sy) - y
	lc = (int32_t)ceil(fpu_reg11);
	eax = y;
	lc = ( (int32_t)lc ) - ( (int32_t)eax );
	if (( (int32_t)lc ) <= 0) goto sublpolygon_lc; //while lc <= 0
	lb = esi;

	fpu_reg11 = ( ((tscreenpoint *)(esi + edi))->sp_z ); //lp[lb].z einlagern

//la
	fpu_reg12 = ( ((tscreenpoint *)(ebx + edi))->sp_z ); //la.z = lp[ia].z;
	la.ls_z = fpu_reg12;

	fpu_reg13 = ( ((tscreenpoint *)(ebx + edi))->sp_y ); //la.y = lp[ia].sy*lp[ia].z;
	fpu_reg13 = fpu_reg13 * fpu_reg12;
	la.ls_p = fpu_reg13; //la.y

	fpu_reg14 = ( ((tscreenpoint *)(ebx + edi))->sp_lx ); //la.lx = lp[ia].lx;
	la.ls_lx = fpu_reg14;

	fpu_reg15 = ( ((tscreenpoint *)(ebx + edi))->sp_ly ); //la.ly = lp[ia].ly;
	la.ls_ly = fpu_reg15;

	fpu_reg16 = ( ((tscreenpoint *)(ebx + edi))->sp_lz ); //la.lz = lp[ia].lz;
	la.ls_lz = fpu_reg16;

//lu
	fpu_reg16 = ( ((tscreenpoint *)(esi + edi))->sp_lz ) - fpu_reg16; //lu.lz = lp[lb].lz - lp[ia].lz;
	lu.ls_lz = fpu_reg16;

	fpu_reg15 = ( ((tscreenpoint *)(esi + edi))->sp_ly ) - fpu_reg15; //lu.ly = lp[lb].ly - lp[ia].ly;
	lu.ls_ly = fpu_reg15;

	fpu_reg14 = ( ((tscreenpoint *)(esi + edi))->sp_lx ) - fpu_reg14; //lu.lx = lp[lb].lx - lp[ia].lx;
	lu.ls_lx = fpu_reg14;

	fpu_reg14 = ( ((tscreenpoint *)(esi + edi))->sp_y ); //lu.y = lp[lb].y*lp[lb].z - la.y;
	fpu_reg14 = fpu_reg14 * fpu_reg11; //lp[lb].z
	fpu_reg13 = fpu_reg14 - fpu_reg13;
	lu.ls_p = fpu_reg13; //lu.y
	fpu_reg11 = fpu_reg11 - fpu_reg12; //lu.z = lp[lb].z - lp[ia].z;
	lu.ls_z = fpu_reg11;


//ldx = (lp[lb].x - lp[ia].x)/(lp[lb].y - lp[ia].y);
	fpu_reg11 = ( ((tscreenpoint *)(esi + edi))->sp_x );
	fpu_reg11 = fpu_reg11 - ( ((tscreenpoint *)(ebx + edi))->sp_x );
	fpu_reg12 = ( ((tscreenpoint *)(esi + edi))->sp_y );
	fpu_reg12 = fpu_reg12 - ( ((tscreenpoint *)(ebx + edi))->sp_y );
	fpu_reg11 = fpu_reg11 / fpu_reg12;
	ldx = fpu_reg11;

//lx = ldx   *(y  - lp[ia].y) + lp[ia].x;
	fpu_reg12 = ( (int32_t)y );
	fpu_reg12 = fpu_reg12 - ( ((tscreenpoint *)(ebx + edi))->sp_y );
	fpu_reg11 = fpu_reg11 * fpu_reg12;
	fpu_reg11 = fpu_reg11 + ( ((tscreenpoint *)(ebx + edi))->sp_x );
	lx = fpu_reg11;
	goto sublpolygon_l_z;
sublpolygon_l_nz:
	fpu_reg11 = lx;
	fpu_reg11 = fpu_reg11 + ldx; //lx bleibt im copro
	lx = fpu_reg11;
sublpolygon_l_z:
//rechts
	rc = ( (int32_t)rc ) - 1;
	if (( (int32_t)rc ) != 0) goto sublpolygon_r_nz;
	edi = (uint8_t *)_sp;
	esi = rb;
sublpolygon_rc:


	if (esi == pend) goto sublpolygon_fertig2; //rb == pend -> unten angekommen
	ebx = esi; //ia = rb
	esi = esi + ( lightpsize ); //rb++

	if (esi < sp_end) goto sublpolygon_r0; //wrap
	esi = 0;
sublpolygon_r0:
	fpu_reg12 = ( ((tscreenpoint *)(esi + edi))->sp_y ); //rc = ceil(lp[rb].sy) - y
	rc = (int32_t)ceil(fpu_reg12);
	eax = y;
	rc = ( (int32_t)rc ) - ( (int32_t)eax );
	if (( (int32_t)rc ) <= 0) goto sublpolygon_rc; //while rc <= 0
	rb = esi;

	fpu_reg12 = ( ((tscreenpoint *)(esi + edi))->sp_z ); //lp[lb].z einlagern

//ra
	fpu_reg13 = ( ((tscreenpoint *)(ebx + edi))->sp_z ); //ra.z = lp[ia].z;
	ra.ls_z = fpu_reg13;

	fpu_reg14 = ( ((tscreenpoint *)(ebx + edi))->sp_y ); //ra.y = lp[ia].sy*lp[ia].z;
	fpu_reg14 = fpu_reg14 * fpu_reg13;
	ra.ls_p = fpu_reg14; //ra.y

	fpu_reg15 = ( ((tscreenpoint *)(ebx + edi))->sp_lx ); //ra.lx = lp[ia].lx;
	ra.ls_lx = fpu_reg15;

	fpu_reg16 = ( ((tscreenpoint *)(ebx + edi))->sp_ly ); //ra.ly = lp[ia].ly;
	ra.ls_ly = fpu_reg16;

	fpu_reg17 = ( ((tscreenpoint *)(ebx + edi))->sp_lz ); //ra.lz = lp[ia].lz;
	ra.ls_lz = fpu_reg17;

//ru
	fpu_reg17 = ( ((tscreenpoint *)(esi + edi))->sp_lz ) - fpu_reg17; //ru.lz = lp[rb].lz - lp[ia].lz;
	ru.ls_lz = fpu_reg17;

	fpu_reg16 = ( ((tscreenpoint *)(esi + edi))->sp_ly ) - fpu_reg16; //ru.ly = lp[rb].ly - lp[ia].ly;
	ru.ls_ly = fpu_reg16;

	fpu_reg15 = ( ((tscreenpoint *)(esi + edi))->sp_lx ) - fpu_reg15; //ru.lx = lp[rb].lx - lp[ia].lx;
	ru.ls_lx = fpu_reg15;

	fpu_reg15 = ( ((tscreenpoint *)(esi + edi))->sp_y ); //ru.y = lp[rb].sy*lp[rb].z - ra.y;
	fpu_reg15 = fpu_reg15 * fpu_reg12; //lp[lb].z
	fpu_reg14 = fpu_reg15 - fpu_reg14;
	ru.ls_p = fpu_reg14; //ru.y

	fpu_reg12 = fpu_reg12 - fpu_reg13; //ru.z = lp[rb].z - lp[ia].z;
	ru.ls_z = fpu_reg12;


//rdx = (lp[rb].x - lp[ia].x)/(lp[rb].y - lp[ia].y);
	fpu_reg12 = ( ((tscreenpoint *)(esi + edi))->sp_x );
	fpu_reg12 = fpu_reg12 - ( ((tscreenpoint *)(ebx + edi))->sp_x );
	fpu_reg13 = ( ((tscreenpoint *)(esi + edi))->sp_y );
	fpu_reg13 = fpu_reg13 - ( ((tscreenpoint *)(ebx + edi))->sp_y );
	fpu_reg12 = fpu_reg12 / fpu_reg13;
	rdx = fpu_reg12;

//rx = rdx   *(y  - lp[ia].y) + lp[ia].x;
	fpu_reg13 = ( (int32_t)y );
	fpu_reg13 = fpu_reg13 - ( ((tscreenpoint *)(ebx + edi))->sp_y );
	fpu_reg12 = fpu_reg12 * fpu_reg13;
	fpu_reg12 = fpu_reg12 + ( ((tscreenpoint *)(ebx + edi))->sp_x );
	rx = fpu_reg12;
	goto sublpolygon_r_z;
sublpolygon_r_nz:
	fpu_reg12 = rx;
	fpu_reg12 = fpu_reg12 + rdx; //rx bleibt im copro
	rx = fpu_reg12;
sublpolygon_r_z:

//st(0) = rx,  st(1) = lx
//st(2) = 0.5, st(3) = 16

//links: r = (y *la.z - la.y) / (lu.y - lu.z*y );
	fpu_reg13 = ( (int32_t)y ); //opti
	fpu_reg13 = fpu_reg13 * la.ls_z;
	fpu_reg13 = fpu_reg13 - la.ls_p; //-la.y
	fpu_reg14 = ( (int32_t)y );
	fpu_reg14 = fpu_reg14 * lu.ls_z;
	fpu_reg14 = lu.ls_p - fpu_reg14; //lu.y-
	fpu_reg13 = fpu_reg13 / fpu_reg14; //st = r

	fpu_reg14 = lu.ls_z; //a.z = la.z + r*lu.z;
	fpu_reg14 = fpu_reg14 * fpu_reg13; //*r
	fpu_reg14 = fpu_reg14 + la.ls_z;
	a.ls_z = fpu_reg14;
	fpu_reg14 = fpu_reg14 * fpu_reg11; //*lx            ;a.x = a.z*lx;
	a.ls_p = fpu_reg14; //a.x

	fpu_reg14 = lu.ls_lz;
	fpu_reg14 = fpu_reg14 * fpu_reg13; //*r             ;a.lz = la.lz + r*lu.lz;
	fpu_reg14 = fpu_reg14 + la.ls_lz;
	a.ls_lz = fpu_reg14;

	fpu_reg15 = lu.ls_lx; //a.lx = la.lx + r*lu.lx;
	fpu_reg15 = fpu_reg15 * fpu_reg13; //*r
	fpu_reg15 = fpu_reg15 + la.ls_lx;
	a.ls_lx = fpu_reg15;

	fpu_reg16 = lu.ls_ly; //a.ly = la.ly + r*lu.ly;
	fpu_reg16 = fpu_reg16 * fpu_reg13; //*r
	fpu_reg16 = fpu_reg16 + la.ls_ly;
	a.ls_ly = fpu_reg16;

	fpu_reg16 = fpu_reg16 / fpu_reg14; //a.ly/a.lz
	txt_y = (int32_t)ceil(fpu_reg16);
	fpu_reg14 = fpu_reg15 / fpu_reg14; //a.lx/a.lz
	txt_x = (int32_t)ceil(fpu_reg14);

//r entfernen
//rechts: r = (y *ra.z - ra.y) / (ru.y - ru.z*y );
	fpu_reg13 = ( (int32_t)y ); //opti
	fpu_reg13 = fpu_reg13 * ra.ls_z;
	fpu_reg13 = fpu_reg13 - ra.ls_p; //ra.y
	fpu_reg14 = ( (int32_t)y );
	fpu_reg14 = fpu_reg14 * ru.ls_z;
	fpu_reg14 = ru.ls_p - fpu_reg14; //ru.y-
	fpu_reg13 = fpu_reg13 / fpu_reg14; //st = r

	fpu_reg14 = ru.ls_z; //u.z = ra.z + r*ru.z;
	fpu_reg14 = fpu_reg14 * fpu_reg13;
	fpu_reg14 = fpu_reg14 + ra.ls_z;

	fpu_reg15 = fpu_reg14; //u.x = u.z*rx - a.x;
	fpu_reg15 = fpu_reg15 * fpu_reg12; //*rx
	fpu_reg15 = fpu_reg15 - a.ls_p; //-a.x
	u.ls_p = fpu_reg15; //u.x

	fpu_reg14 = fpu_reg14 - a.ls_z; //u.z -= a.z;
	u.ls_z = fpu_reg14;

	fpu_reg14 = ru.ls_lx; //u.lx = ra.lx + r*ru.lx - a.lx;
	fpu_reg14 = fpu_reg14 * fpu_reg13; //*r
	fpu_reg14 = fpu_reg14 + ra.ls_lx;
	fpu_reg14 = fpu_reg14 - a.ls_lx;
	u.ls_lx = fpu_reg14;

	fpu_reg14 = ru.ls_ly; //u.ly = ra.ly + r*ru.ly - a.ly;
	fpu_reg14 = fpu_reg14 * fpu_reg13; //*r
	fpu_reg14 = fpu_reg14 + ra.ls_ly;
	fpu_reg14 = fpu_reg14 - a.ls_ly;
	u.ls_ly = fpu_reg14;

	fpu_reg13 = fpu_reg13 * ru.ls_lz; //*r             ;u.lz = ra.lz + r*ru.lz - a.lz;
	fpu_reg13 = fpu_reg13 + ra.ls_lz;
	fpu_reg13 = fpu_reg13 - a.ls_lz;
	u.ls_lz = fpu_reg13;

	xe = (int32_t)ceil(fpu_reg12); //xe = ceil(rx) (rx entfernen)
	fpu_reg11 = ceil(fpu_reg11);
	xa = (int32_t)ceil(fpu_reg11); //xa = ceil(lx) (lx nicht entfernen)


	ecx = xe;
	ecx = ecx - xa; //ecx = pixel in einer zeile
	ed2 = xa + x_y; //edi -> pixel

sublpolygon_l:
	edx = txt_x; //edx = 00Xx
	ebx = txt_y; //ebx = 00Yy

//24                 ;16
	if (( (int32_t)ecx ) >= ( 48 )) goto sublpolygon_div;

//weniger als 24 pixel

	if (( (int32_t)ecx ) <= 0) goto sublpolygon_w;

	esi = ecx; //esi : zu setzende pixel
	ecx = 0;
	stack_var00 = ecx;

	fpu_reg12 = a.ls_lz;
	fpu_reg12 = fpu_reg12 + u.ls_lz;

	fpu_reg13 = a.ls_lx; //textur - endkoordinate
	fpu_reg13 = fpu_reg13 + u.ls_lx; //(a.lx + 1*u.lx)/(a.lz + 1*u.lz)
	fpu_reg13 = fpu_reg13 / fpu_reg12;
	fpu_reg13 = fpu_reg13 - ( (int32_t)txt_x );
	fpu_reg13 = fpu_reg13 * ( divtab32[esi] ); //16
	txt_x = (int32_t)ceil(fpu_reg13);

	fpu_reg13 = a.ls_ly; //textur - endkoordinate
	fpu_reg13 = fpu_reg13 + u.ls_ly; //(a.ly + 1*u.ly)/(a.lz + 1*u.lz)
	fpu_reg12 = fpu_reg13 / fpu_reg12;
	fpu_reg12 = fpu_reg12 - ( (int32_t)txt_y );
	fpu_reg12 = fpu_reg12 * ( divtab32[esi] );
	txt_y = (int32_t)ceil(fpu_reg12);

	eax = txt_x;
	ecx = txt_y;

	goto sublpolygon_5; //st(0) = x ;st(1) = 16
sublpolygon_div: //scanline subdivision

	fpu_reg11 = fpu_reg11 + fpu_reg10; //16             ;x um 16 weiter
	fpu_reg12 = fpu_reg11; //x                    ;r = (x *a.z - a.x) / (u.x - u.z*x )
	fpu_reg12 = fpu_reg12 * a.ls_z;
	fpu_reg12 = fpu_reg12 - a.ls_p; //a.x
	fpu_reg13 = fpu_reg11; //x
	fpu_reg13 = fpu_reg13 * u.ls_z;
	fpu_reg13 = u.ls_p - fpu_reg13; //u.x
	fpu_reg12 = fpu_reg12 / fpu_reg13; //st = r

	fpu_reg13 = u.ls_lz; //(a.lz + r*u.lz)
	fpu_reg13 = fpu_reg13 * fpu_reg12;
	fpu_reg13 = fpu_reg13 + a.ls_lz;

	fpu_reg14 = u.ls_lx; //x = (a.ly + r*u.ly)/(a.lz + r*u.lz)
	fpu_reg14 = fpu_reg14 * fpu_reg12;
	fpu_reg14 = fpu_reg14 + a.ls_lx;
	fpu_reg14 = fpu_reg14 / fpu_reg13;
	txt_x = (int32_t)ceil(fpu_reg14);

	fpu_reg14 = u.ls_ly; //y = (a.ly + r*u.ly)/(a.lz + r*u.lz)
	fpu_reg14 = fpu_reg14 * fpu_reg12;
	fpu_reg14 = fpu_reg14 + a.ls_ly;
	fpu_reg13 = fpu_reg14 / fpu_reg13;
	txt_y = (int32_t)ceil(fpu_reg13);

//r entfernen

	esi = ( 32 ); //16                  ;esi : zu setzende pixel
	ecx = ecx - esi;
	stack_var00 = ecx; //ecx : x-z„hler

	eax = txt_x;
	ecx = txt_y;
	eax = eax - edx; //eax = 00Uu
	ecx = ecx - ebx; //ecx = 00Vv

sublpolygon_5:
	stack_var01 = ( 0 /*ebp*/ ); //ebp : lokale variablen

	edx = set_high_byte(edx, ( (uint8_t)(edx >> 8) ) + ( 0x80 ));
	ebx = set_high_byte(ebx, ( (uint8_t)(ebx >> 8) ) + ( 0x80 ));

	esi = esi | zval;
	stack_var02 = esi;
	es2 = (uint8_t *)scene.sc_lmap; //mapptr              ;esi = TTTT

	ebx = ebx << ( 16 );
	ecx = ecx << ( 16 - 5 ); //v/16
	eax = eax >> ( 5 ); //u/16
	ebx = ebx >> ( 8 );
	ecx = ecx >> ( 8 );
	ebx = (ebx >> ( 8 )) | (edx << (32 - ( 8 ))); //ebx = x0Yy
	ecx = (ecx >> ( 8 )) | (eax << (32 - ( 8 ))); //ecx = u0Vv
	edx = (uint32_t)( (uint8_t)(edx >> 8) ); //edx = 000X
	ebp = (uint32_t)( (uint8_t)(eax >> 8) ); //ebp = 000U
//keine lokalen variablen mehr zug„nglich
	eax = stack_var02; //eax = 00zC

	edx = set_high_byte(edx, ( (uint8_t)(ebx >> 8) ));
	{ uint32_t value = ebp; ebp = ecx; ecx = value; }
sublpolygon_inner:

	if (( (uint8_t)(eax >> 8) ) != ( *(((uint8_t *)ed2) + (3)) )) goto sublpolygon_i0;
	edx = set_high_byte(edx, ( *((uint8_t *)(edx + es2)) )); //15
	{ uint32_t carry = (UINT32_MAX - ebx < ebp)?1:0; ebx = ebx + ebp; //ecx
	  edx = (edx & 0xffffff00) | (uint8_t)(( (uint8_t)edx ) + ( (uint8_t)ecx ) + carry); } //adc     edx,ebp
	*(((uint8_t *)ed2) + (1)) = (uint8_t) (( *(((uint8_t *)ed2) + (1)) ) + ( (uint8_t)(edx >> 8) )); //+2
	ed2 = ed2 + 1;
	edx = set_high_byte(edx, ( (uint8_t)(ebx >> 8) ));
	eax = (eax & 0xffffff00) | (uint8_t)(( (int8_t)eax ) - 1);
	if (( (int8_t)eax ) == 0) goto sublpolygon_iw;
	goto sublpolygon_inner;
sublpolygon_i0:
	{ uint32_t carry = (UINT32_MAX - ebx < ebp)?1:0; ebx = ebx + ebp; //ecx
	  edx = (edx & 0xffffff00) | (uint8_t)(( (uint8_t)edx ) + ( (uint8_t)ecx ) + carry); } //adc     edx,ebp
	ed2 = ed2 + 1;
	edx = set_high_byte(edx, ( (uint8_t)(ebx >> 8) ));
	eax = (eax & 0xffffff00) | (uint8_t)(( (int8_t)eax ) - 1);
	if (( (int8_t)eax ) != 0) goto sublpolygon_inner;
sublpolygon_iw:

	ebp = stack_var01;
	ecx = stack_var00;
	goto sublpolygon_l;
sublpolygon_w:
//x entfernen

	eax = xres;
	x_y = x_y + eax;
	y = y + 1;
	eax = yres;
//while (y < yres)
	if (( (int32_t)eax ) > ( (int32_t)y )) goto sublpolygon_y_l;

	goto sublpolygon_fertig;
sublpolygon_fertig2:

sublpolygon_fertig:
//16.0 entfernen

sublpolygon_weg:
	return;
}


static uint32_t scubezclip(tcpoint *eax, void *&_ebx, ttempcube *_esi, tscreenpoint *&_edi) {
	realnum fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13;
	uint32_t ea2;
	tprojected *edx, *ecx;
	tscreenpoint *edi = _edi, *eb2;
	tcpoint *ebx = (tcpoint *)_ebx;
	uint8_t *esi = (uint8_t *)_esi;
	tcpoint *stack_var00;
//-> ebx -> sp     (source points of type tcpoint)
//-> eax -> sp_end
//-> esi -> ttempcube
//-> edi -> dp     (dest points of type tscreenpoint)

//<- ebx -> dp
//<- esi -> ttempcube
//<- edi -> dp_end
//<- flags: if less than 3 points, jump with jbe
	tcpoint *_sp, *_sp_end;
	tscreenpoint *_dp;

	_sp = ebx;
	_sp_end = eax;
	_dp = edi;

	fpu_reg10 = 1.0;
scubezclip_z_l:
//inn = true : point in front of plane
	edx = ( &(((ttempcube *)(esi + ((tcpoint *)ebx)->cp_p))->tc_proj[0]) ); //edx -> tprojected (this point)
//if (inn)
//inn = (z >= 1.0)
	if ((( edx->pr_inn ) & ( 1 )) == 0) goto scubezclip_0;

//point in front of projection plane (visible)
	fpu_reg11 = ( edx->pr_sx );
	fpu_reg11 = fpu_reg11 + fpu_reg10;
	fpu_reg11 = fpu_reg11 * smid;
	edi->sp_x = fpu_reg11; //x = sres/2 *(1 + x/z)

	fpu_reg11 = ( edx->pr_sy );
	fpu_reg11 = fpu_reg11 + fpu_reg10; //subr
	fpu_reg11 = fpu_reg11 * smid;
	edi->sp_y = fpu_reg11; //y = sres/2 *(1 + y/z)

//        fld     [edx].pr_z
//        fstp    [edi].sp_z

	edi = (tscreenpoint *)(( (uintptr_t)edi) + shadowpsize ); //one destination point added
//end of if statement
scubezclip_0:
	eax = ebx; //ebx -> actual source point
	eax = eax + ( 1 ); //eax -> next source point
//wrap if at last point
	if (eax < _sp_end) goto scubezclip_wrap;
	eax = _sp;
scubezclip_wrap:

	ecx = ( &(((ttempcube *)(esi + eax->cp_p))->tc_proj[0]) ); //ecx -> tprojected (next point)
//if (inn ^ (nz >= 1.0))
	stack_var00 = eax;
	ea2 = /*(ea2 & 0xffffff00) |*/ (uint8_t)(( edx->pr_inn ));
	ea2 = /*(ea2 & 0xffffff00) |*/ (uint8_t)(( (uint8_t)ea2 ) ^ ( ecx->pr_inn ));
	if (( (int8_t)ea2 ) != 0) goto scubezclip_not1;
	eax = stack_var00;
	goto scubezclip_1;
scubezclip_not1:
	eax = stack_var00;

//this or next point behind projection plane
//r = (1.0-z)/(nz-z)
	fpu_reg11 = 1.0;
//        fst     [edi].sp_z              ;z = 1.0
	fpu_reg11 = fpu_reg11 - ( edx->pr_z );
	fpu_reg12 = ( ecx->pr_z );
	fpu_reg12 = fpu_reg12 - ( edx->pr_z );
	fpu_reg11 = fpu_reg11 / fpu_reg12; //st = r

	fpu_reg12 = ( edx->pr_x ); //x' = x + r*(nx-x)
	fpu_reg13 = ( ecx->pr_x );
	fpu_reg13 = fpu_reg13 - fpu_reg12;
	fpu_reg13 = fpu_reg13 * fpu_reg11;
	fpu_reg12 = fpu_reg12 + fpu_reg13;
	fpu_reg12 = fpu_reg12 + fpu_reg10; //x = sres/2 *(1 + x')
	fpu_reg12 = fpu_reg12 * smid;
	edi->sp_x = fpu_reg12;

	fpu_reg12 = ( edx->pr_y ); //y' = y + r*(ny-y)
	fpu_reg13 = ( ecx->pr_y );
	fpu_reg13 = fpu_reg13 - fpu_reg12;
	fpu_reg13 = fpu_reg13 * fpu_reg11;
	fpu_reg12 = fpu_reg12 + fpu_reg13;
	fpu_reg12 = fpu_reg12 + fpu_reg10; //subr          ;y = sres/2 *(1 - y')
	fpu_reg12 = fpu_reg12 * smid;
	edi->sp_y = fpu_reg12;

//remove r

	edi = (tscreenpoint *)( ((uintptr_t)edi) + shadowpsize ); //one destination point added
//end of if statement
scubezclip_1:
//ns > s?
	if (eax <= ebx) goto scubezclip_notz_l;
	ebx = eax; //is same as ebx += point size
	goto scubezclip_z_l; //repeat loop
scubezclip_notz_l:
	ebx = eax; //is same as ebx += point size

//remove 1.0

	eb2 = _dp;
	ea2 = (uintptr_t)edi - (uintptr_t)eb2;
//less than 3 points ?
	if (ea2 <= ( shadowpsize * 2 )) goto scubezclip_w;

	ea2 = 0;
	goto scubezclip_weg;
scubezclip_w:
	ea2 = ( 1 );
scubezclip_weg:
	_edi = edi;
	_ebx = (void *)eb2;
	return ea2;
}


static uint32_t sprojection(tscreenpoint *&_ebx, tscreenpoint *&_edi) {
	realnum fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13;
	uint32_t eax, edx;
	tscreenpoint *ebx = _ebx, *esi, *edi = _edi;
//-> ebx = *sp
//-> edi = *sp_end
//<- ebx = *sp
//<- edi = *sp_end
//<- flags: if less than 3 points, jump with jbe
	tscreenpoint *_sp, *_sp_end; //both are pointers

//x, y, z: components of source point
//d : prefix for destination points
//n : prefix for next sorce point

	_sp = ebx; //ebx -> source points
	_sp_end = edi; //edi -> destination points

	fpu_reg10 = 1.0;
sprojection_z_l:
//inn = true : point in front of plane
	fpu_reg11 = ( ebx->sp_z ); //inn = (z >= 1)

	eax = /*(eax & 0xffff0000) |*/ ((fpu_reg11 < fpu_reg10)?0x100:0);
	edx = set_high_byte(0 /*edx*/, ( (uint8_t)(eax >> 8) )); //dh = inn ;if (inn)

	if ((eax & 0x100) != 0) goto sprojection_0;
//point in front of projection plane (visible)
	fpu_reg11 = ( ebx->sp_z );
	edi->sp_z = fpu_reg11;

	fpu_reg12 = ( ebx->sp_x ); //x = xres/2 *(1 + x/z)
	fpu_reg12 = fpu_reg12 / fpu_reg11;
	fpu_reg12 = fpu_reg12 + fpu_reg10;
	fpu_reg12 = fpu_reg12 * smid;
	edi->sp_x = fpu_reg12;

	fpu_reg12 = ( ebx->sp_y ); //y = yres/2 *(1 + y/z)
	fpu_reg11 = fpu_reg12 / fpu_reg11;
	fpu_reg11 = fpu_reg11 + fpu_reg10; //subr
	fpu_reg11 = fpu_reg11 * smid;
	edi->sp_y = fpu_reg11;

	edi = (tscreenpoint *)( ((uintptr_t)edi) + shadowpsize ); //one destination point added
//end of if statement
sprojection_0:
	esi = ebx; //ebx -> actual source point
	esi = (tscreenpoint *)( ((uintptr_t)esi) + shadowpsize ); //esi -> next source point
//wrap if at last point
	if (esi < _sp_end) goto sprojection_wrap;
	esi = _sp;
sprojection_wrap:
//if (inn ^ (nz >= 1))
	fpu_reg11 = ( esi->sp_z );

	eax = /*(eax & 0xffff0000) |*/ ((fpu_reg11 < fpu_reg10)?0x100:0);
	eax = set_high_byte(eax, ( (uint8_t)(eax >> 8) ) ^ ( (uint8_t)(edx >> 8) )); //dh = inn

	if ((eax & 0x100) == 0) goto sprojection_1;
//this or next point behind projection plane
//r = (1.0-z)/(nz-z)
	fpu_reg11 = 1.0;
	edi->sp_z = fpu_reg11; //dz = 1.0
	fpu_reg11 = fpu_reg11 - ( ebx->sp_z );
	fpu_reg12 = ( esi->sp_z );
	fpu_reg12 = fpu_reg12 - ( ((tscreenpoint *)ebx)->sp_z );
	fpu_reg11 = fpu_reg11 / fpu_reg12; //st = r

	fpu_reg12 = ( ebx->sp_x ); //x' = x + r*(nx-x)
	fpu_reg13 = ( esi->sp_x );
	fpu_reg13 = fpu_reg13 - fpu_reg12;
	fpu_reg13 = fpu_reg13 * fpu_reg11;
	fpu_reg12 = fpu_reg12 + fpu_reg13;
	fpu_reg12 = fpu_reg12 + fpu_reg10; //x = xres/2 *(1 + x')
	fpu_reg12 = fpu_reg12 * xmid;
	edi->sp_x = fpu_reg12;

	fpu_reg12 = ( ebx->sp_y ); //y' = y + r*(ny-y)
	fpu_reg13 = ( esi->sp_y );
	fpu_reg13 = fpu_reg13 - fpu_reg12;
	fpu_reg11 = fpu_reg11 * fpu_reg13; //remove r
	fpu_reg11 = fpu_reg11 + fpu_reg12;
	fpu_reg11 = fpu_reg11 + fpu_reg10; //subr          ;y = yres/2 *(1 + y')
	fpu_reg11 = fpu_reg11 * ymid;
	edi->sp_y = fpu_reg11;

	edi = (tscreenpoint *)( ((uintptr_t)edi) + shadowpsize ); //one destination point added
//end of if statement
sprojection_1:
//ns > s?
	if (esi <= ebx) goto sprojection_notz_l;
	ebx = esi; //is same as ebx += point size
	goto sprojection_z_l; //repeat loop
sprojection_notz_l:
	ebx = esi; //is same as ebx += point size

	ebx = _sp_end; //ebx -> start of points array
//edi -> end of points array

//remove 1.0

	eax = (uintptr_t)edi - (uintptr_t)ebx;
//less than 3 points ?
	if (eax <= ( shadowpsize * 2 )) goto sprojection_w;
	eax = 0;
	goto sprojection_weg;
sprojection_w:
	eax = ( 1 );

sprojection_weg:
	_edi = edi;
	_ebx = ebx;
	return eax;
}


#if 0
//sxclip proc pascal ;x-clipping
//        ;-> ebx = *sp
//        ;-> edi = *sp_end
//        ;<- ebx = *sp
//        ;<- edi = *sp_end
//        ;<- wenn weniger als 3 punkte, mit jbe wegspringen
//local   _sp:dword, _sp_end:dword ;beides pointer
//local   i:byte
//
//        fldz                            ;st = x
//        mov     i,1
//@@i_l:
//        mov     _sp,ebx                 ;ebx -> sp[z] (quellpunkte) (z = 0)
//        mov     _sp_end,edi             ;edi -> sp[d] (zielpunkte)  (d = m)
//
//        xor     dl,dl                   ;dl = flag
//@@z_l:
//
//        fld     sp_x[ebx]               ;inn = (sp[z].sx >= x);
//        fcomp   st(1)
//        fstsw   ax
//        mov     dh,ah                   ;dh = inn
//        xor     ah,i
//        sahf
//        jnc     @@0
//                ;punkt innerhalb
//        mov     esi,ebx
//        mov     ecx,shadowpsize
//        shr     ecx,2
//        rep     movsd                   ;sp[d] = sp[z] und d++
//@@0:
//        mov     esi,ebx
//        add     esi,shadowpsize         ;nz = z+1
//        cmp     esi,_sp_end             ;if (nz >= m) nz = 0;
//        jb      @@wrap
//        mov     esi,_sp
//@@wrap:
//
//        fld     sp_x[esi]              ;if (inn ^ (sp[nz].sx >= x))
//        fcomp   st(1)
//        fstsw   ax
//        xor     ah,dh                   ;dh = inn
//        sahf
//        jnc     @@1
//                ;dieser oder n„chster punkt auáerhalb
//        inc     dl                      ;flag = 1
//
//        fst     sp_x[edi]              ;sp[d].sx speichern
//
//                ;r berechnen
//        fld     st;x                    ;(x - sp[z].sx)
//        fsub    sp_x[ebx]
//        fld     sp_x[esi]              ;/(sp[nz].sx - sp[z].sx)
//        fsub    sp_x[ebx]
//        fdivp   st(1),st                ;st = r
//
//
//        fld     sp_y[ebx]              ;sp[d].sy berechnen
//        fld     sp_y[esi]              ; sp[z].sy + r*(sp[nz].sy - sp[z].sy)
//        fsub    st,st(1)
//        fmulp   st(2),st               ;r entfernen
//        faddp   st(1),st
//        fstp    sp_y[edi]
//
//
//        add     edi,shadowpsize         ;d++
//@@1:
//        cmp     esi,ebx                 ;wz > z?
//        mov     ebx,esi                 ;entspricht ebx += shadowpsize
//        ja      @@z_l                   ;z-schleife
//
//                                        ;ebx -> anfang der z-punkte (sp)
//        mov     esi,_sp_end             ;esi -> anfang der d-punkte (sp_end)
//                                        ;edi -> ende der d-punkte
//        mov     ecx,edi
//        sub     ecx,esi                 ;ecx = anzahl zu kopierender byte
//        jz      @@w                     ; kein punkt brig (ZF = 1)
//
//        mov     edi,esi                 ;wenn flag = 0: nicht kopieren,
//        or      dl,dl                   ; edi = _sp_end zurckgeben
//        jz      @@2
//
//        mov     edi,ebx                 ;edi -> anfang der z-punkte
//        shr     ecx,2                   ;ecx = anzahl zu kopierender dwords
//        rep     movsd
//@@2:
//
//        fadd    sfres                   ;x += sres
//        dec     i                       ;i-schleife
//        jns     @@i_l
//
//        mov     eax,edi
//        sub     eax,ebx
//        cmp     eax,shadowpsize*2
//@@w:    fstp    st                      ;x entfernen
//@@weg:  ret
//sxclip endp
#endif

static void spolygon(tscreenpoint *_sp, uint32_t sp_end) {
	realnum fpu_reg10, fpu_reg11, fpu_reg12;
	uint32_t eax, ecx, ed2, ebx, esi;
	uint8_t *edi, *es2;

//_sp = *sp, zeiger auf screenpoints

	uint32_t pend, y, ymax, lb, lc, rb, rc; //int
	uint8_t *x_y;
	float lx, ldx, rx, rdx; //float
	uint32_t xa, xe; //int


	eax = 0; //start- und endpunkt bestimmen
	lb = eax;
	rb = eax;
	pend = eax;

	es2 = (uint8_t *)_sp; //esi -> screenpoints
	fpu_reg10 = ( ((tscreenpoint *)es2)->sp_y ); //st(0) = ymax
	fpu_reg11 = fpu_reg10; //st(1) = ymin

	ecx = sp_end;
	ecx = ecx - ( shadowpsize ); //esi+ecx -> sp[sp_end-1]
spolygon_max_l:

	fpu_reg12 = ( ((tscreenpoint *)(es2 + ecx))->sp_y );
//ymax              ;gr”áten y-wert finden (endpunkt)


	if (fpu_reg12 < fpu_reg11) goto spolygon_max;
	pend = ecx;
	fpu_reg11 = fpu_reg12; //ymax
	goto spolygon_min;
spolygon_max:
//ymin              ;kleinsten y-wert finden (startpunkt)


	if (fpu_reg12 > fpu_reg10) goto spolygon_min;
	lb = ecx;
	rb = ecx;
	fpu_reg10 = fpu_reg12; //ymin
spolygon_min:

	ecx = ( (int32_t)ecx ) - ( shadowpsize );
	if (( (int32_t)ecx ) != 0) goto spolygon_max_l;
	ymax = (int32_t)ceil(fpu_reg11); //ymax entfernen

	y = (int32_t)ceil(fpu_reg10); //y = ceil(ymin)

	eax = y;

//y-clipping
	if (( (int32_t)eax ) >= ( sres )) goto spolygon_weg;


	if (( (int32_t)eax ) > 0) goto spolygon_y0;
	eax = 0;
	y = eax;
spolygon_y0:

	if (( (int32_t)eax ) >= ( (int32_t)ymax )) goto spolygon_weg;

//        mov     ebx,_mapptr             ;lmap kopieren
//        mov     esi,[ebx]
//        cmp     esi,scene.sc_lmap
//        je      @@lmap
//        mov     edi,scene.sc_lmap
//        mov     [ebx],edi
//        mov     ecx,65536/4
//        rep     movsd
//        mov     esi,[ebx];lmap
//@@lmap:


	eax = eax << ( 8 );
	x_y = eax + (uint8_t *)scene.sc_lmap; //esi;mapptr

	lc = ( 1 );
	rc = ( 1 );

spolygon_y_l: //y-schleife

//links
	lc = ( (int32_t)lc ) - 1;
	if (( (int32_t)lc ) != 0) goto spolygon_l_nz;
	edi = (uint8_t *)_sp;
	esi = lb;
spolygon_lc:

	if (esi == pend) goto spolygon_fertig; //lb == pend -> unten angekommen
	ebx = esi; //ia = lb
	{ uint32_t carry = (esi < ( shadowpsize ))?1:0; esi = esi - ( shadowpsize ); //lb--
	  if (carry == 0) goto spolygon_l0; } //wrap
	esi = esi + sp_end;
spolygon_l0:
	fpu_reg10 = ( ((tscreenpoint *)(esi + edi))->sp_y ); //lc = ceil(sp[lb].sy) - y
	lc = (int32_t)ceil(fpu_reg10);
	eax = y;
	lc = ( (int32_t)lc ) - ( (int32_t)eax );
	if (( (int32_t)lc ) <= 0) goto spolygon_lc; //while lc <= 0
	lb = esi;


//ldx = (sp[lb].sx - sp[ia].sx)/(sp[lb].sy - sp[ia].sy);
	fpu_reg10 = ( ((tscreenpoint *)(esi + edi))->sp_x );
	fpu_reg10 = fpu_reg10 - ( ((tscreenpoint *)(ebx + edi))->sp_x );
	fpu_reg11 = ( ((tscreenpoint *)(esi + edi))->sp_y );
	fpu_reg11 = fpu_reg11 - ( ((tscreenpoint *)(ebx + edi))->sp_y );
	fpu_reg10 = fpu_reg10 / fpu_reg11;
	ldx = fpu_reg10;

//lx = ldx   *(y  - sp[ia].sy) + sp[ia].sx;
	fpu_reg11 = ( (int32_t)y );
	fpu_reg11 = fpu_reg11 - ( ((tscreenpoint *)(ebx + edi))->sp_y );
	fpu_reg10 = fpu_reg10 * fpu_reg11;
	fpu_reg10 = fpu_reg10 + ( ((tscreenpoint *)(ebx + edi))->sp_x );
	lx = fpu_reg10;
	goto spolygon_l_z;
spolygon_l_nz:
	fpu_reg10 = lx;
	fpu_reg10 = fpu_reg10 + ldx; //lx bleibt im copro
	lx = fpu_reg10;
spolygon_l_z:

	xa = (int32_t)ceil(fpu_reg10); //xa = ceil(lx) (lx entfernen)

//rechts
	rc = ( (int32_t)rc ) - 1;
	if (( (int32_t)rc ) != 0) goto spolygon_r_nz;
	edi = (uint8_t *)_sp;
	esi = rb;
spolygon_rc:


	if (esi == pend) goto spolygon_fertig; //rb == pend -> unten angekommen
	ebx = esi; //ia = rb
	esi = esi + ( shadowpsize ); //rb++

	if (esi < sp_end) goto spolygon_r0; //wrap
	esi = 0;
spolygon_r0:
	fpu_reg10 = ( ((tscreenpoint *)(esi + edi))->sp_y ); //rc = ceil(sp[rb].sy) - y
	rc = (int32_t)ceil(fpu_reg10);
	eax = y;
	rc = ( (int32_t)rc ) - ( (int32_t)eax );
	if (( (int32_t)rc ) <= 0) goto spolygon_rc; //while rc <= 0
	rb = esi;


//rdx = (sp[rb].sx - sp[ia].sx)/(sp[rb].sy - sp[ia].sy);
	fpu_reg10 = ( ((tscreenpoint *)(esi + edi))->sp_x );
	fpu_reg10 = fpu_reg10 - ( ((tscreenpoint *)(ebx + edi))->sp_x );
	fpu_reg11 = ( ((tscreenpoint *)(esi + edi))->sp_y );
	fpu_reg11 = fpu_reg11 - ( ((tscreenpoint *)(ebx + edi))->sp_y );
	fpu_reg10 = fpu_reg10 / fpu_reg11;
	rdx = fpu_reg10;

//rx = rdx   *(y  - sp[ia].sy) + sp[ia].sx;
	fpu_reg11 = ( (int32_t)y );
	fpu_reg11 = fpu_reg11 - ( ((tscreenpoint *)(ebx + edi))->sp_y );
	fpu_reg10 = fpu_reg10 * fpu_reg11;
	fpu_reg10 = fpu_reg10 + ( ((tscreenpoint *)(ebx + edi))->sp_x );
	rx = fpu_reg10;
	goto spolygon_r_z;
spolygon_r_nz:
	fpu_reg10 = rx;
	fpu_reg10 = fpu_reg10 + rdx; //rx bleibt im copro
	rx = fpu_reg10;
spolygon_r_z:
//st(0) = rx,  st(1) = lx
	xe = (int32_t)ceil(fpu_reg10); //xe = ceil(rx) (rx entfernen)

	ed2 = xa;
//dec edi


	if (( (int32_t)ed2 ) >= 0) goto spolygon_c0;
	ed2 = 0;
spolygon_c0:
	ecx = xe; //<
//inc ecx

	if (( (int32_t)ecx ) < ( sres )) goto spolygon_c1;
	ecx = ( sres );
spolygon_c1:

	ecx = ( (int32_t)ecx ) - ( (int32_t)ed2 ); //ecx = pixel in einer zeile
	if (( (int32_t)ecx ) <= 0) goto spolygon_w;
	edi = ed2 + x_y; //edi -> pixel

	eax = 0;
//     cmp ecx,10
//     jb  @@o5
//     mov edx,ecx
//     mov ecx,edi
//     neg ecx
//     and ecx,3
//     sub edx,ecx
//     rep stosb

//     mov ecx,edx
//     shr ecx,2
//     rep stosd
//     mov ecx,edx
//     and ecx,3

//@@o5:
	memset(edi, (uint8_t)eax, ecx);

spolygon_w:
	eax = ( sres );
	x_y = x_y + eax;
	y = y + 1;
//while (y < sres)
	if (( (int32_t)eax ) > ( (int32_t)y )) goto spolygon_y_l;

spolygon_fertig:

spolygon_weg:
	return;
}


static void cproject(tviewer *esi, realnum &_fpu_reg7, realnum &_fpu_reg8, realnum &_fpu_reg9) {
	realnum fpu_reg7 = _fpu_reg7, fpu_reg8 = _fpu_reg8, fpu_reg9 = _fpu_reg9, fpu_reg10, fpu_reg11, fpu_reg12;
//esi -> camera




//st(2) = x1, st(1) = x2, st(0) = x3
	fpu_reg10 = ( esi->v_l3.x1 ); //calc x3 (z)
	fpu_reg10 = fpu_reg10 * fpu_reg7;
	fpu_reg11 = ( esi->v_l3.x2 );
	fpu_reg11 = fpu_reg11 * fpu_reg8;
	fpu_reg10 = fpu_reg10 + fpu_reg11;
	fpu_reg11 = ( esi->v_l3.x3 );
	fpu_reg11 = fpu_reg11 * fpu_reg9;
	fpu_reg10 = fpu_reg10 + fpu_reg11; //st(0) = z
	{ realnum tmp = fpu_reg9; fpu_reg9 = fpu_reg10; fpu_reg10 = tmp; }
//st(3) = x1, st(2) = x2, st(0) = x3
	fpu_reg11 = ( esi->v_l1.x1 ); //calc x1 (x)
	fpu_reg11 = fpu_reg11 * fpu_reg7;
	fpu_reg12 = ( esi->v_l1.x2 );
	fpu_reg12 = fpu_reg12 * fpu_reg8;
	fpu_reg11 = fpu_reg11 + fpu_reg12;
	fpu_reg12 = ( esi->v_l1.x3 );
	fpu_reg12 = fpu_reg12 * fpu_reg10;
	fpu_reg11 = fpu_reg11 + fpu_reg12; //st(0) = x
	fpu_reg11 = fpu_reg11 + fpu_reg9;
	fpu_reg11 = fpu_reg11 * xmid; //(z+x)*xmid, (1+x/z) = (z+x)/z
	{ realnum tmp = fpu_reg7; fpu_reg7 = fpu_reg11; fpu_reg11 = tmp; }
//st(3) = x2, st(1) = x3, st(0) = x1
	fpu_reg11 = fpu_reg11 * ( esi->v_l2.x1 ); //calc x2 (y)
	fpu_reg12 = ( esi->v_l2.x2 );
	fpu_reg12 = fpu_reg12 * fpu_reg8;
	fpu_reg11 = fpu_reg11 + fpu_reg12;
	fpu_reg12 = ( esi->v_l2.x3 );
	fpu_reg10 = fpu_reg10 * fpu_reg12; //st(0) = y
	fpu_reg10 = fpu_reg10 + fpu_reg11;
	fpu_reg10 = fpu_reg9 - fpu_reg10;
	fpu_reg10 = fpu_reg10 * ymid; //(z-y)*ymid, (1-y/z) = (z-y)/z
	fpu_reg8 = fpu_reg10;

	_fpu_reg7 = fpu_reg7;
	_fpu_reg8 = fpu_reg8;
	_fpu_reg9 = fpu_reg9;
}


static tcube *rtrace(tvec *b, tvec *u, tcube *oldcube, tcube *ebx, realnum &_fpu_reg10) {
	realnum fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13;
	uint32_t ecx;
	tcube *eax, *edx;
	tplane *eb2;
	tvec *esi;

//-> ebx -> tcube
//-> st(0) = r
//<- edx = sidecube (or _NULL)

	fpu_reg10 = 0.0; //r_min
	edx = 0; //sidecube

	ecx = ( ((tcube *)ebx)->c_planes );
	if (ecx == 0) goto rtrace_weg;
	eb2 = ( ((tcube *)ebx)->c_planedata );
rtrace_l:
	eax = ( eb2->pl_sidecube );

	if (eax == oldcube) goto rtrace_next;

	esi = b;
	fpu_reg11 = ( eb2->pl_p.x1 );
	fpu_reg11 = fpu_reg11 - ( esi->x1 );
	fpu_reg11 = fpu_reg11 * ( eb2->pl_n.x1 );
	fpu_reg12 = ( eb2->pl_p.x2 );
	fpu_reg12 = fpu_reg12 - ( esi->x2 );
	fpu_reg12 = fpu_reg12 * ( eb2->pl_n.x2 );
	fpu_reg11 = fpu_reg11 + fpu_reg12;
	fpu_reg12 = ( eb2->pl_p.x3 );
	fpu_reg12 = fpu_reg12 - ( esi->x3 );
	fpu_reg12 = fpu_reg12 * ( eb2->pl_n.x3 );
	fpu_reg11 = fpu_reg11 + fpu_reg12; //st = (p-b)*n

//st > 0: entered side-cube


	if (fpu_reg11 < 0.0) goto rtrace_w;


	esi = u; //r - 1.0 = (p-b)*n/(u*n)
	fpu_reg12 = ( esi->x1 );
	fpu_reg12 = fpu_reg12 * ( eb2->pl_n.x1 );
	fpu_reg13 = ( esi->x2 );
	fpu_reg13 = fpu_reg13 * ( eb2->pl_n.x2 );
	fpu_reg12 = fpu_reg12 + fpu_reg13;
	fpu_reg13 = ( esi->x3 );
	fpu_reg13 = fpu_reg13 * ( eb2->pl_n.x3 );
	fpu_reg12 = fpu_reg12 + fpu_reg13;

	fpu_reg11 = fpu_reg11 / fpu_reg12; //st = r - 1.0

//r < r_min?


	if (fpu_reg11 > fpu_reg10) goto rtrace_w;
	{ realnum tmp = fpu_reg10; fpu_reg10 = fpu_reg11; fpu_reg11 = tmp; }
	edx = ( eb2->pl_sidecube );
rtrace_w:

rtrace_next:
	eb2 = eb2 + ( 1 );
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto rtrace_l;

rtrace_weg:
	_fpu_reg10 = fpu_reg10;
	return edx;
}



static uint32_t yclip(uint32_t spsize, uint32_t vars, tscreenpoint *&_ebx, tscreenpoint *&_edi) { //y-clipping
	realnum fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13;
	uint32_t eax, edx, ecx;
	tscreenpoint *edi = _edi, *ebx = _ebx, *esi;
//data variables -1
//-> ebx = *sp
//-> edi = *sp_end
//<- ebx = *sp
//<- edi = *sp_end
//<- wenn weniger als 3 punkte, mit jbe wegspringen
	tscreenpoint *_sp, *_sp_end; //beides pointer

	fpu_reg10 = 0.0; //st = y
	edx = /*(edx & 0xffffff00) |*/ (uint8_t)(( 1 )); //dl = i, 1 to 0
yclip_i_l:
	_sp = ebx; //ebx -> sp[z] (quellpunkte) (z = 0)
	_sp_end = edi; //edi -> sp[d] (zielpunkte)  (d = m)

yclip_z_l:
	fpu_reg11 = ( ebx->sp_y ); //inn = (sp[z].y >= y);

	eax = /*(eax & 0xffff0000) |*/ ((fpu_reg11 < fpu_reg10)?0x100:0);
	edx = set_high_byte(edx, ( (uint8_t)(eax >> 8) )); //dh = inn
	eax = set_high_byte(eax, ( (uint8_t)(eax >> 8) ) ^ ( (uint8_t)edx )); //inn xor i

	if ((eax & 0x100) == 0) goto yclip_0;
//punkt innerhalb
	memcpy(edi, ebx, spsize); //sp[d] = sp[z] und d++
	edi = (tscreenpoint *)( ((uintptr_t)edi) + spsize );
yclip_0:
	esi = ebx;
	esi = (tscreenpoint *)( ((uintptr_t)esi) + spsize ); //nz = z+1
//if (nz >= m) nz = 0;
	if (esi < _sp_end) goto yclip_wrap;
	esi = _sp;
yclip_wrap:

	fpu_reg11 = ( esi->sp_y ); //if (inn ^ (sp[nz].y >= y))

	eax = /*(eax & 0xffff0000) |*/ ((fpu_reg11 < fpu_reg10)?0x100:0);
	eax = set_high_byte(eax, ( (uint8_t)(eax >> 8) ) ^ ( (uint8_t)(edx >> 8) )); //dh = inn

	if ((eax & 0x100) == 0) goto yclip_1;
//dieser oder n„chster punkt auáerhalb

	edi->sp_y = fpu_reg10; //sp[d].y speichern

//r berechnen
	fpu_reg11 = fpu_reg10; //y                    ;(y - sp[z].y)
	fpu_reg11 = fpu_reg11 - ( ebx->sp_y );
	fpu_reg12 = ( esi->sp_y ); // /(sp[nz].y - sp[z].y)
	fpu_reg12 = fpu_reg12 - ( ebx->sp_y );
	fpu_reg11 = fpu_reg11 / fpu_reg12; //st = r

	fpu_reg12 = ( ebx->sp_x ); //sp[d].x berechnen
	fpu_reg13 = ( esi->sp_x ); // sp[z].x + r*(sp[nz].x - sp[z].x)
	fpu_reg13 = fpu_reg13 - fpu_reg12;
	fpu_reg13 = fpu_reg13 * fpu_reg11;
	fpu_reg12 = fpu_reg12 + fpu_reg13;
	edi->sp_x = fpu_reg12;

	ecx = vars;
yclip_vars:
	fpu_reg12 = ( esi->sp_data[ecx] );
	fpu_reg12 = fpu_reg12 - ( ebx->sp_data[ecx] );
	fpu_reg12 = fpu_reg12 * fpu_reg11;
	fpu_reg12 = fpu_reg12 + ( ebx->sp_data[ecx] );
	edi->sp_data[ecx] = fpu_reg12;
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) >= 0) goto yclip_vars;

//r entfernen

	edi = (tscreenpoint *)( ((uintptr_t)edi) + spsize ); //d++
yclip_1:
//wz > z?
	if (esi <= ebx) goto yclip_notz_l;
	ebx = esi; //entspricht ebx += spsize
	goto yclip_z_l; //z-schleife
yclip_notz_l:
	ebx = esi; //entspricht ebx += spsize

	ebx = _sp_end; //ebx -> sp
//edi -> sp_end
	eax = (uintptr_t)edi - (uintptr_t)ebx;
	eax = eax >> ( 1 );

	if (eax <= spsize) goto yclip_w;

	fpu_reg10 = fpu_reg10 + ( (int32_t)yres ); //y += yres
	edx = /*(edx & 0xffffff00) |*/ (uint8_t)(( (int8_t)edx ) - 1); //i-schleife
	if (( (int8_t)edx ) >= 0) goto yclip_i_l;

//y entfernen
	eax = 0;
	goto yclip_weg;
yclip_w: //y entfernen
	eax = ( 1 );
yclip_weg:
	_edi = edi;
	_ebx = ebx;
	return eax;
}




static void vvolpolygon(tscreenpoint *_sp, uint32_t sp_end, uint32_t *cubelistpos, uint32_t *cubelistend, int32_t cubeliststep, tidxentry *idxlistpos, tidxentry *idxlistend, int32_t idxliststep, tvec *ca, tvec *cu, uint8_t *ipmap, uint8_t *mapptr) {
	realnum fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13, fpu_reg14, fpu_reg15;
	uint32_t eax, edx, ecx, ebx, esi, ebp;
	uint8_t *edi, *es2;
	uint32_t *ed2, *eb2;
	tidxentry *es3;
	uint32_t stack_var00;

//arg
//arg
//_sp = *sp, zeiger auf screenpoints

//call vvolpolygon pascal, ebx, edi,
//  cubelists, cubeliste, cubeliststep,
//  idxlistpos, idxlistend, idxliststep, ca, cu, ipmap, mapptr

	uint32_t pend, y, lb, lc, rb, rc; //int
	uint32_t *x_y;
	uint32_t cc, cidx, oc; //int  counter for cube permissions, counter for object permissions
	float lx, ldx, lu, ldu; //float
	float rx, rdx, ru, rdu; //float
	uint32_t xa, xe, txt_x; //int


	eax = 0; //start- und endpunkt bestimmen
	lb = eax;
	rb = eax;
	pend = eax;

	es2 = (uint8_t *)_sp; //esi -> screenpoints
	fpu_reg10 = ( ((tscreenpoint *)es2)->sp_y ); //st(0) = ymax
	fpu_reg11 = fpu_reg10; //st(1) = ymin

	ecx = sp_end;
	ecx = ecx - ( volpsize ); //esi+ecx -> sp[sp_end-1]
vvolpolygon_max_l:

	fpu_reg12 = ( ((tscreenpoint *)(es2 + ecx))->sp_y );
//ymax              ;gr”áten y-wert finden (endpunkt)


	if (fpu_reg12 < fpu_reg11) goto vvolpolygon_max;
	pend = ecx;
	fpu_reg11 = fpu_reg12; //ymax
	goto vvolpolygon_min;
vvolpolygon_max:
//ymin              ;kleinsten y-wert finden (startpunkt)


	if (fpu_reg12 > fpu_reg10) goto vvolpolygon_min;
	lb = ecx;
	rb = ecx;
	fpu_reg10 = fpu_reg12; //ymin
vvolpolygon_min:

	ecx = ( (int32_t)ecx ) - ( volpsize );
	if (( (int32_t)ecx ) != 0) goto vvolpolygon_max_l;
//ymax entfernen

	y = (int32_t)ceil(fpu_reg10); //y = ceil(ymin)

	edx = y;
//y-clipping
	if (( (int32_t)edx ) >= ( (int32_t)yres )) goto vvolpolygon_weg;


	if (( (int32_t)edx ) > 0) goto vvolpolygon_y0;
	edx = 0;
	y = edx;
vvolpolygon_y0:
	edx = ( (int32_t)edx ) * ( (int32_t)xres );
	x_y = edx + (uint32_t *)scene.sc_buffer;

	lc = ( 1 );
	rc = ( 1 );

	cc = ( 1 );
	cidx = ( 256 );

//object permission counter
	es3 = idxlistpos;
	oc = ( maxint );

	if (es3 == idxlistend) goto vvolpolygon_y_l;

	fpu_reg10 = ( es3->i_zval ); //r = (zval - a3)/u3
	fpu_reg11 = fpu_reg10;
	fpu_reg11 = fpu_reg11 - ( ca->x3 );
	fpu_reg11 = fpu_reg11 / ( cu->x3 ); //st(0) = r, st(1) = zval

	fpu_reg12 = ( cu->x2 ); //y = (a2 + r*u2)/zval
	fpu_reg11 = fpu_reg11 * fpu_reg12;
	fpu_reg11 = fpu_reg11 + ( ca->x2 );
	fpu_reg10 = fpu_reg11 / fpu_reg10;

	oc = (int32_t)ceil(fpu_reg10);
	eax = y;
	oc = oc - eax;


vvolpolygon_y_l: //y-schleife

//links
	lc = ( (int32_t)lc ) - 1;
	if (( (int32_t)lc ) != 0) goto vvolpolygon_l_nz;
	edi = (uint8_t *)_sp;
	esi = lb;
vvolpolygon_lc:

	if (esi == pend) goto vvolpolygon_fertig; //lb == pend -> unten angekommen
	ebx = esi; //ia = lb
	{ uint32_t carry = (esi < ( volpsize ))?1:0; esi = esi - ( volpsize ); //lb--
	  if (carry == 0) goto vvolpolygon_l0; } //wrap
	esi = esi + sp_end;
vvolpolygon_l0:
	fpu_reg10 = ( ((tscreenpoint *)(esi + edi))->sp_y ); //lc = ceil(sp[lb].sy) - y
	lc = (int32_t)ceil(fpu_reg10);
	eax = y;
	lc = ( (int32_t)lc ) - ( (int32_t)eax );
	if (( (int32_t)lc ) <= 0) goto vvolpolygon_lc; //while lc <= 0
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
//fist txt_x
	lu = fpu_reg12;

//ldx = (sp[lb].x - sp[ia].x)/(sp[lb].y - sp[ia].y);
	fpu_reg12 = ( ((tscreenpoint *)(edi + esi))->sp_x );
	fpu_reg12 = fpu_reg12 - ( ((tscreenpoint *)(edi + ebx))->sp_x );
	fpu_reg11 = fpu_reg12 / fpu_reg11;
	ldx = fpu_reg11;

//lx = ldx  *(y  - sp[ia].y) + sp[ia].x;
	fpu_reg10 = fpu_reg10 * fpu_reg11;
	fpu_reg10 = fpu_reg10 + ( ((tscreenpoint *)(edi + ebx))->sp_x );
	lx = fpu_reg10;

	goto vvolpolygon_l_z;
vvolpolygon_l_nz:
	fpu_reg10 = lu;
	fpu_reg10 = fpu_reg10 + ldu;
//fist txt_x
	lu = fpu_reg10;

	fpu_reg10 = lx;
	fpu_reg10 = fpu_reg10 + ldx;
	lx = fpu_reg10; //lx bleibt im copro
vvolpolygon_l_z:

//rechts
	rc = ( (int32_t)rc ) - 1;
	if (( (int32_t)rc ) != 0) goto vvolpolygon_r_nz;
	edi = (uint8_t *)_sp;
	esi = rb;
vvolpolygon_rc:

	if (esi == pend) goto vvolpolygon_fertig2; //rb == pend -> unten angekommen
	ebx = esi; //ia = rb
	esi = esi + ( volpsize ); //rb++

	if (esi < sp_end) goto vvolpolygon_r0; //wrap
	esi = 0;
vvolpolygon_r0:
	fpu_reg11 = ( ((tscreenpoint *)(esi + edi))->sp_y ); //rc = ceil(sp[rb].sy) - y
	rc = (int32_t)ceil(fpu_reg11);
	eax = y;
	rc = ( (int32_t)rc ) - ( (int32_t)eax );
	if (( (int32_t)rc ) <= 0) goto vvolpolygon_rc; //while lc <= 0
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

//rdx = (sp[rb].x - sp[ia].x)/(sp[rb].y - sp[ia].y);
	fpu_reg13 = ( ((tscreenpoint *)(edi + esi))->sp_x );
	fpu_reg13 = fpu_reg13 - ( ((tscreenpoint *)(edi + ebx))->sp_x );
	fpu_reg12 = fpu_reg13 / fpu_reg12;
	rdx = fpu_reg12;

//rx = rdx  *(y  - sp[ia].y) + sp[ia].x;
	fpu_reg11 = fpu_reg11 * fpu_reg12;
	fpu_reg11 = fpu_reg11 + ( ((tscreenpoint *)(edi + ebx))->sp_x );
	rx = fpu_reg11;

	goto vvolpolygon_r_z;
vvolpolygon_r_nz:
	fpu_reg11 = ru;
	fpu_reg11 = fpu_reg11 + rdu;
	ru = fpu_reg11;

	fpu_reg11 = rx;
	fpu_reg11 = fpu_reg11 + rdx;
	rx = fpu_reg11; //rx bleibt im copro
vvolpolygon_r_z:

	xe = (int32_t)ceil(fpu_reg11); //xe = ceil(rx) (rx entfernen)
	xa = (int32_t)ceil(fpu_reg10); //xa = ceil(lx) (lx entfernen)

//update cube permissions
	cc = ( (int32_t)cc ) - 1;
	if (( (int32_t)cc ) != 0) goto vvolpolygon_c_nz;
	eb2 = cubelistpos;
vvolpolygon_cc1:
	edi = ipmap;
	ecx = ( *eb2 ); //ecx = idx

	if (( (int32_t)ecx ) < 0) goto vvolpolygon_11;
	eax = cidx;
	cidx = ecx;
	edi = edi + eax;

	ecx = ( (int32_t)ecx ) - ( (int32_t)eax ); //ecx = indices to change
	if (( (int32_t)ecx ) < 0) goto vvolpolygon_not10;
	eax = /*(eax & 0xffffff00) |*/ (uint8_t)(( 0 )); //default: disable some indices
	goto vvolpolygon_10;
vvolpolygon_not10:
	edi = edi + ( (int32_t)ecx ); //enable some indices
	ecx = - ( (int32_t)ecx );
	eax = /*(eax & 0xffffff00) |*/ (uint8_t)(( 1 ));
vvolpolygon_10:
	memset(edi, (uint8_t)eax, ecx);
vvolpolygon_11:

	if (eb2 == cubelistend) goto vvolpolygon_c_nz;
	eax = ( eb2[1] ); //eax = y
	eb2 = (uint32_t *)( ((uintptr_t)eb2) + cubeliststep ); //update cubelistpos (+8 or -8)
	eax = ( (int32_t)eax ) - ( (int32_t)y );
	if (( (int32_t)eax ) <= 0) goto vvolpolygon_cc1;
//        jz      @@cc1
	cc = eax;
	cubelistpos = eb2;
vvolpolygon_c_nz:
//update object permissions
	oc = ( (int32_t)oc ) - 1;
	if (( (int32_t)oc ) > 0) goto vvolpolygon_o_nz;
	es3 = idxlistpos;
vvolpolygon_oc1:

	eax = ( es3->i_idx );
	*((uint8_t *)(ipmap + eax)) = (uint8_t) (( *((uint8_t *)(ipmap + eax)) ) ^ ( 1 ));

	es3 = (tidxentry *)( ((uintptr_t)es3) + idxliststep );
	oc = ( maxint );

	if (es3 == idxlistend) goto vvolpolygon_o_nz;

	fpu_reg10 = ( es3->i_zval ); //r = (zval - a3)/u3
	fpu_reg11 = fpu_reg10;
	//ebx = ca; //ebx -> ca (sttzvektor)
	//edx = cu; //edx -> cu (richtungsvektor)
	fpu_reg11 = fpu_reg11 - ( ca->x3 );
	fpu_reg11 = fpu_reg11 / ( cu->x3 ); //st(0) = r, st(1) = zval

	fpu_reg12 = ( cu->x2 ); //y = (a2 + r*u2)/zval
	fpu_reg11 = fpu_reg11 * fpu_reg12;
	fpu_reg11 = fpu_reg11 + ( ca->x2 );
	fpu_reg10 = fpu_reg11 / fpu_reg10;

	oc = (int32_t)ceil(fpu_reg10);
	eax = y;
	oc = ( (int32_t)oc ) - ( (int32_t)eax );
	if (( (int32_t)oc ) <= 0) goto vvolpolygon_oc1;
	idxlistpos = es3;
vvolpolygon_o_nz:


//eax = 00?x
//ebx = 00Uu
//ecx = III?
//edx = 000X
//esi = TTTT
//edi = PPPP
//ebp = CCCC

	edx = xe;
	edx = ( (int32_t)edx ) - ( (int32_t)xa ); //edx = CCCC
	if (( (int32_t)edx ) <= 0) goto vvolpolygon_w;

	fpu_reg10 = lu; //lu
	fpu_reg11 = ru;
	fpu_reg11 = fpu_reg11 - fpu_reg10; //ru-lu

	fpu_reg12 = lx; //lx
	fpu_reg13 = rx;
	fpu_reg13 = fpu_reg13 - fpu_reg12; //rx-lx

	fpu_reg14 = ( (int32_t)xa );
	fpu_reg14 = fpu_reg14 - fpu_reg12;
	fpu_reg14 = fpu_reg14 / fpu_reg13;
	fpu_reg14 = fpu_reg14 * fpu_reg11; //(xa-lx)/(rx-lx) * (ru-lu)


	fpu_reg15 = ( (int32_t)xe );
	fpu_reg15 = fpu_reg15 - fpu_reg12;
	fpu_reg15 = fpu_reg15 / fpu_reg13;
	ed2 = xa + x_y; //edi = PPPP
	fpu_reg15 = fpu_reg15 * fpu_reg11; //(xe-lx)/(rx-lx) * (ru-lu)
	fpu_reg15 = fpu_reg15 - fpu_reg14;
	es2 = (uint8_t *)scene.sc_divtab;
	fpu_reg15 = fpu_reg15 * ( ((float *)(es2))[edx] );
	txt_x = (int32_t)ceil(fpu_reg15);

	fpu_reg10 = fpu_reg10 + fpu_reg14;


	ebx = txt_x; //ebx = 00Uu

	txt_x = (int32_t)ceil(fpu_reg10);

	//ecx = ipmap; //ecx = IIII
	es2 = mapptr; //esi = TTTT
	eax = txt_x; //eax = 00Xx

//        add     edi,x_y                 ;edi = PPPP
//        mov     eax,txt_x               ;eax = 00Xx

//        mov     esi,scene.sc_divtab
//        fld     ru
//        fsub    lu
//        fmul    dword ptr[esi+edx*4-4]
//        fistp   txt_x

//        mov     ecx,ipmap               ;ecx = IIII
//        mov     esi,mapptr              ;esi = TTTT
//        mov     ebx,txt_x               ;ebx = 00Uu

	stack_var00 = ( 0 /*ebp*/ ); //ebp : lokale variablen
	ebp = edx; //ebp = CCCC
	edx = (uint32_t)( (uint8_t)(eax >> 8) ); //edx = 000X

vvolpolygon_inner:
	ecx = /*(ecx & 0xffffff00) |*/ (uint8_t)(( *(((uint8_t *)ed2) + (3)) )); //idx
	eax = set_high_byte(eax, ( *((uint8_t *)(es2 + edx)) ));
//index permission map
	if ((( *((uint8_t *)(ipmap + ecx)) ) & ( 1 )) == 0) goto vvolpolygon_i0;
	*(((uint8_t *)ed2) + (2)) = (uint8_t) (( *(((uint8_t *)ed2) + (2)) ) + ( (uint8_t)(eax >> 8) ));
vvolpolygon_i0:
	{ uint32_t carry = (UINT8_MAX - ( (uint8_t)eax ) < ( (uint8_t)ebx ))?1:0; eax = (eax & 0xffffff00) | (uint8_t)(( (uint8_t)eax ) + ( (uint8_t)ebx ));
	  edx = (edx & 0xffffff00) | (uint8_t)(( (uint8_t)edx ) + ( (uint8_t)(ebx >> 8) ) + carry); }
	ed2 = ed2 + 1;
	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto vvolpolygon_inner;

	ebp = stack_var00;
vvolpolygon_w:
	eax = xres;
	x_y = x_y + eax;
	y = y + 1;
	eax = yres;
//while (y < yres)
	if (( (int32_t)eax ) > ( (int32_t)y )) goto vvolpolygon_y_l;

	goto vvolpolygon_fertig;
vvolpolygon_fertig2:

vvolpolygon_fertig:

vvolpolygon_weg:
	return;
}



static void hvolpolygon(tscreenpoint *_sp, uint32_t sp_end, uint32_t *cubelistpos, uint32_t *cubelistend, int32_t cubeliststep, tidxentry *idxlistpos, tidxentry *idxlistend, int32_t idxliststep, tvec *ca, tvec *cu, uint8_t *ipmap, uint8_t *mapptr) {
	realnum fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13, fpu_reg14, fpu_reg15;
	uint32_t eax, edx, ecx, ebx, esi, ebp;
	uint8_t *edi, *es2;
	uint32_t *ed2, *eb2;
	tidxentry *es3;
	uint32_t stack_var00;

//arg
//arg
//_sp = *sp, zeiger auf screenpoints

//call hvolpolygon pascal, ebx, edi,
//  cubelists, cubeliste, cubeliststep,
//  idxlistpos, idxlistend, idxliststep, ca, cu, ipmap, mapptr

	uint32_t pend, x, lb, lc, rb, rc; //int
	uint32_t *x_y;
	uint32_t cc, cidx, oc; //int  counter for cube permissions, counter for object permissions
	float ly, ldy, lu, ldu; //float
	float ry, rdy, ru, rdu; //float
	uint32_t ya, ye, txt_x; //int


	eax = 0; //start- und endpunkt bestimmen
	lb = eax;
	rb = eax;
	pend = eax;

	es2 = (uint8_t *)_sp; //esi -> screenpoints
	fpu_reg10 = ( ((tscreenpoint *)es2)->sp_x ); //st(0) = xmax
	fpu_reg11 = fpu_reg10; //st(1) = xmin

	ecx = sp_end;
	ecx = ecx - ( volpsize ); //esi+ecx -> sp[sp_end-1]
hvolpolygon_max_l:

	fpu_reg12 = ( ((tscreenpoint *)(es2 + ecx))->sp_x );
//ymax              ;gr”áten x-wert finden (endpunkt)


	if (fpu_reg12 < fpu_reg11) goto hvolpolygon_max;
	pend = ecx;
	fpu_reg11 = fpu_reg12; //ymax
	goto hvolpolygon_min;
hvolpolygon_max:
//ymin              ;kleinsten x-wert finden (startpunkt)


	if (fpu_reg12 > fpu_reg10) goto hvolpolygon_min;
	lb = ecx;
	rb = ecx;
	fpu_reg10 = fpu_reg12; //ymin
hvolpolygon_min:

	ecx = ( (int32_t)ecx ) - ( volpsize );
	if (( (int32_t)ecx ) != 0) goto hvolpolygon_max_l;
//xmax entfernen

	x = (int32_t)ceil(fpu_reg10); //x = ceil(xmin)

	edx = x;
//x-clipping
	if (( (int32_t)edx ) >= ( (int32_t)xres )) goto hvolpolygon_weg;


	if (( (int32_t)edx ) > 0) goto hvolpolygon_x0;
	edx = 0;
	x = edx;
hvolpolygon_x0:
	x_y = edx + (uint32_t *)scene.sc_buffer;

	lc = ( 1 );
	rc = ( 1 );

	cc = ( 1 );
	cidx = ( 256 );

//object permission counter
	es3 = idxlistpos;
	oc = ( maxint );

	if (es3 == idxlistend) goto hvolpolygon_x_l;

	fpu_reg10 = ( es3->i_zval ); //r = (zval - a3)/u3
	fpu_reg11 = fpu_reg10;
	fpu_reg11 = fpu_reg11 - ( ca->x3 );
	fpu_reg11 = fpu_reg11 / ( cu->x3 ); //st(0) = r, st(1) = zval

	fpu_reg12 = ( cu->x1 ); //x = (a1 + r*u1)/zval
	fpu_reg11 = fpu_reg11 * fpu_reg12;
	fpu_reg11 = fpu_reg11 + ( ca->x1 );
	fpu_reg10 = fpu_reg11 / fpu_reg10;

	oc = (int32_t)ceil(fpu_reg10);
	eax = x;
	oc = oc - eax;


hvolpolygon_x_l: //x-schleife

//links
	lc = ( (int32_t)lc ) - 1;
	if (( (int32_t)lc ) != 0) goto hvolpolygon_l_nz;
	edi = (uint8_t *)_sp;
	esi = lb;
hvolpolygon_lc:

	if (esi == pend) goto hvolpolygon_fertig; //lb == pend -> rechts angekommen
	ebx = esi; //ia = lb
	{ uint32_t carry = (esi < ( volpsize ))?1:0; esi = esi - ( volpsize ); //lb--
	  if (carry == 0) goto hvolpolygon_l0; } //wrap
	esi = esi + sp_end;
hvolpolygon_l0:
	fpu_reg10 = ( ((tscreenpoint *)(esi + edi))->sp_x ); //lc = ceil(sp[lb].sx) - x
	lc = (int32_t)ceil(fpu_reg10);
	eax = x;
	lc = ( (int32_t)lc ) - ( (int32_t)eax );
	if (( (int32_t)lc ) <= 0) goto hvolpolygon_lc; //while lc <= 0
	lb = esi;


	fpu_reg10 = ( (int32_t)x );
	fpu_reg10 = fpu_reg10 - ( ((tscreenpoint *)(ebx + edi))->sp_x ); //(x  - sp[ia].x)

	fpu_reg11 = ( ((tscreenpoint *)(edi + esi))->sp_x ); //(sp[lb].x - sp[ia].x)
	fpu_reg11 = fpu_reg11 - ( ((tscreenpoint *)(edi + ebx))->sp_x );

//ldu = (sp[lb].u - sp[ia].u)/(sp[lb].x - sp[ia].x);
	fpu_reg12 = ( ((tscreenpoint *)(edi + esi))->sp_u );
	fpu_reg12 = fpu_reg12 - ( ((tscreenpoint *)(edi + ebx))->sp_u );
	fpu_reg12 = fpu_reg12 / fpu_reg11;
	ldu = fpu_reg12;

//lu = ldu  *(x  - sp[ia].x) + sp[ia].u;
	fpu_reg12 = fpu_reg12 * fpu_reg10;
	fpu_reg12 = fpu_reg12 + ( ((tscreenpoint *)(edi + ebx))->sp_u );
//      fist txt_x
	lu = fpu_reg12;

//ldy = (sp[lb].y - sp[ia].y)/(sp[lb].x - sp[ia].x);
	fpu_reg12 = ( ((tscreenpoint *)(edi + esi))->sp_y );
	fpu_reg12 = fpu_reg12 - ( ((tscreenpoint *)(edi + ebx))->sp_y );
	fpu_reg11 = fpu_reg12 / fpu_reg11;
	ldy = fpu_reg11;

//ly = ldy  *(x  - sp[ia].x) + sp[ia].y;
	fpu_reg10 = fpu_reg10 * fpu_reg11;
	fpu_reg10 = fpu_reg10 + ( ((tscreenpoint *)(edi + ebx))->sp_y );
	ly = fpu_reg10;

	goto hvolpolygon_l_z;
hvolpolygon_l_nz:
	fpu_reg10 = lu;
	fpu_reg10 = fpu_reg10 + ldu;
//      fist txt_x
	lu = fpu_reg10;

	fpu_reg10 = ly;
	fpu_reg10 = fpu_reg10 + ldy; //lx bleibt im copro
	ly = fpu_reg10;
hvolpolygon_l_z:
//rechts
	rc = ( (int32_t)rc ) - 1;
	if (( (int32_t)rc ) != 0) goto hvolpolygon_r_nz;
	edi = (uint8_t *)_sp;
	esi = rb;
hvolpolygon_rc:

	if (esi == pend) goto hvolpolygon_fertig2; //rb == pend -> unten angekommen
	ebx = esi; //ia = rb
	esi = esi + ( volpsize ); //rb++

	if (esi < sp_end) goto hvolpolygon_r0; //wrap
	esi = 0;
hvolpolygon_r0:
	fpu_reg11 = ( ((tscreenpoint *)(esi + edi))->sp_x ); //rc = ceil(sp[rb].sx) - x
	rc = (int32_t)ceil(fpu_reg11);
	eax = x;
	rc = ( (int32_t)rc ) - ( (int32_t)eax );
	if (( (int32_t)rc ) <= 0) goto hvolpolygon_rc; //while lc <= 0
	rb = esi;


	fpu_reg11 = ( (int32_t)x );
	fpu_reg11 = fpu_reg11 - ( ((tscreenpoint *)(ebx + edi))->sp_x ); //(x  - sp[ia].x)

	fpu_reg12 = ( ((tscreenpoint *)(edi + esi))->sp_x ); //(sp[rb].x - sp[ia].x)
	fpu_reg12 = fpu_reg12 - ( ((tscreenpoint *)(edi + ebx))->sp_x );

//rdu = (sp[rb].u - sp[ia].u)/(sp[rb].x - sp[ia].x);
	fpu_reg13 = ( ((tscreenpoint *)(edi + esi))->sp_u );
	fpu_reg13 = fpu_reg13 - ( ((tscreenpoint *)(edi + ebx))->sp_u );
	fpu_reg13 = fpu_reg13 / fpu_reg12;
	rdu = fpu_reg13;

//ru = rdu  *(x  - sp[ia].x) + sp[ia].u;
	fpu_reg13 = fpu_reg13 * fpu_reg11;
	fpu_reg13 = fpu_reg13 + ( ((tscreenpoint *)(edi + ebx))->sp_u );
	ru = fpu_reg13;

//rdy = (sp[rb].y - sp[ia].y)/(sp[rb].x - sp[ia].x);
	fpu_reg13 = ( ((tscreenpoint *)(edi + esi))->sp_y );
	fpu_reg13 = fpu_reg13 - ( ((tscreenpoint *)(edi + ebx))->sp_y );
	fpu_reg12 = fpu_reg13 / fpu_reg12;
	rdy = fpu_reg12;

//ry = rdy  *(x  - sp[ia].x) + sp[ia].y;
	fpu_reg11 = fpu_reg11 * fpu_reg12;
	fpu_reg11 = fpu_reg11 + ( ((tscreenpoint *)(edi + ebx))->sp_y );
	ry = fpu_reg11;

	goto hvolpolygon_r_z;
hvolpolygon_r_nz:
	fpu_reg11 = ru;
	fpu_reg11 = fpu_reg11 + rdu;
	ru = fpu_reg11;

	fpu_reg11 = ry;
	fpu_reg11 = fpu_reg11 + rdy; //ry bleibt im copro
	ry = fpu_reg11;
hvolpolygon_r_z:

	ye = (int32_t)ceil(fpu_reg11); //ye = ceil(ry) (ry entfernen)
	ya = (int32_t)ceil(fpu_reg10); //ya = ceil(ly) (ly entfernen)

//update cube permissions
	cc = ( (int32_t)cc ) - 1;
	if (( (int32_t)cc ) != 0) goto hvolpolygon_c_nz;
	eb2 = cubelistpos;
hvolpolygon_cc1:
	edi = ipmap;
	ecx = ( *eb2 ); //ecx = idx

	if (( (int32_t)ecx ) < 0) goto hvolpolygon_11;
	eax = cidx;
	cidx = ecx;
	edi = edi + eax;

	ecx = ( (int32_t)ecx ) - ( (int32_t)eax ); //ecx = indices to change
	if (( (int32_t)ecx ) < 0) goto hvolpolygon_not10;
	eax = /*(eax & 0xffffff00) |*/ (uint8_t)(( 0 )); //default: disable some indices
	goto hvolpolygon_10;
hvolpolygon_not10:
	edi = edi + ( (int32_t)ecx ); //enable some indices
	ecx = - ( (int32_t)ecx );
	eax = /*(eax & 0xffffff00) |*/ (uint8_t)(( 1 ));
hvolpolygon_10:
	memset(edi, (uint8_t)eax, ecx);
hvolpolygon_11:

	if (eb2 == cubelistend) goto hvolpolygon_c_nz;
	eax = ( eb2[1] ); //eax = x
	eb2 = (uint32_t *)( ((uintptr_t)eb2) + cubeliststep ); //update cubelistpos (+8 or -8)
//sub     eax,y
	eax = ( (int32_t)eax ) - ( (int32_t)x );
	if (( (int32_t)eax ) <= 0) goto hvolpolygon_cc1;
//        jz      @@cc1
	cc = eax;
	cubelistpos = eb2;
hvolpolygon_c_nz:
//update object permissions
	oc = ( (int32_t)oc ) - 1;
	if (( (int32_t)oc ) > 0) goto hvolpolygon_o_nz;
	es3 = idxlistpos;
hvolpolygon_oc1:

	eax = ( es3->i_idx );
	*((uint8_t *)(ipmap + eax)) = (uint8_t) (( *((uint8_t *)(ipmap + eax)) ) ^ ( 1 ));

	es3 = (tidxentry *)( ((uintptr_t)es3) + idxliststep );
	oc = ( maxint );

	if (es3 == idxlistend) goto hvolpolygon_o_nz;

	fpu_reg10 = ( es3->i_zval ); //r = (zval - a3)/u3
	fpu_reg11 = fpu_reg10;
	//ebx = ca; //ebx -> ca (sttzvektor)
	//edx = cu; //edx -> cu (richtungsvektor)
	fpu_reg11 = fpu_reg11 - ( ca->x3 );
	fpu_reg11 = fpu_reg11 / ( cu->x3 ); //st(0) = r, st(1) = zval

	fpu_reg12 = ( cu->x1 ); //x = (a1 + r*u1)/zval
	fpu_reg11 = fpu_reg11 * fpu_reg12;
	fpu_reg11 = fpu_reg11 + ( ca->x1 );
	fpu_reg10 = fpu_reg11 / fpu_reg10;

	oc = (int32_t)ceil(fpu_reg10);
	eax = x;
	oc = ( (int32_t)oc ) - ( (int32_t)eax );
	if (( (int32_t)oc ) <= 0) goto hvolpolygon_oc1;
	idxlistpos = es3;
hvolpolygon_o_nz:


//eax = 00?x
//ebx = 00Uu
//ecx = III?
//edx = 000X
//esi = TTTT
//edi = PPPP
//ebp = CCCC

	edx = ye;
	edx = ( (int32_t)edx ) - ( (int32_t)ya ); //edx = CCCC
	if (( (int32_t)edx ) <= 0) goto hvolpolygon_w;

	fpu_reg10 = lu; //lu
	fpu_reg11 = ru;
	fpu_reg11 = fpu_reg11 - fpu_reg10; //ru-lu

	fpu_reg12 = ly; //ly
	fpu_reg13 = ry;
	fpu_reg13 = fpu_reg13 - fpu_reg12; //ry-ly

	fpu_reg14 = ( (int32_t)ya );
	fpu_reg14 = fpu_reg14 - fpu_reg12;
	fpu_reg14 = fpu_reg14 / fpu_reg13;
	fpu_reg14 = fpu_reg14 * fpu_reg11; //(ya-ly)/(ry-ly) * (ru-lu)


	fpu_reg15 = ( (int32_t)ye );
	fpu_reg15 = fpu_reg15 - fpu_reg12;
	fpu_reg15 = fpu_reg15 / fpu_reg13;
	ed2 = (( (int32_t)ya ) * ( (int32_t)xres )) + x_y; //edi = PPPP
	fpu_reg15 = fpu_reg15 * fpu_reg11; //(ye-ly)/(ry-ly) * (ru-lu)
	fpu_reg15 = fpu_reg15 - fpu_reg14;
	es2 = (uint8_t *)scene.sc_divtab;
	fpu_reg15 = fpu_reg15 * ( ((float *)(es2))[edx] );
	txt_x = (int32_t)ceil(fpu_reg15);

	fpu_reg10 = fpu_reg10 + fpu_reg14;


	ebx = txt_x; //ebx = 00Uu

	txt_x = (int32_t)ceil(fpu_reg10);

	//ecx = ipmap; //ecx = IIII
	es2 = mapptr; //esi = TTTT
	eax = txt_x; //eax = 00Xx

//        imul    edi,xres
//        add     edi,x_y                 ;edi = PPPP
//        mov     eax,txt_x               ;eax = 00Xx

//        mov     esi,scene.sc_divtab
//        fld     ru
//        fsub    lu
//        fmul    dword ptr[esi+edx*4-4]
//        fistp   txt_x

//        mov     ecx,ipmap               ;ecx = IIII
//        mov     esi,mapptr              ;esi = TTTT
//        mov     ebx,txt_x               ;ebx = 00Uu

	stack_var00 = ( 0 /*ebp*/ ); //ebp : lokale variablen
	ebp = edx; //ebp = CCCC
	edx = (uint32_t)( (uint8_t)(eax >> 8) ); //edx = 000X

hvolpolygon_inner:
	ecx = /*(ecx & 0xffffff00) |*/ (uint8_t)(( *(((uint8_t *)ed2) + (3)) )); //idx
	eax = set_high_byte(eax, ( *((uint8_t *)(es2 + edx)) ));
//index permission map
	if ((( *((uint8_t *)(ipmap + ecx)) ) & ( 1 )) == 0) goto hvolpolygon_i0;
//      mov byte ptr [edi*4+2],20
	*(((uint8_t *)ed2) + (2)) = (uint8_t) (( *(((uint8_t *)ed2) + (2)) ) + ( (uint8_t)(eax >> 8) ));
hvolpolygon_i0:
	{ uint32_t carry = (UINT8_MAX - ( (uint8_t)eax ) < ( (uint8_t)ebx ))?1:0; eax = (eax & 0xffffff00) | (uint8_t)(( (uint8_t)eax ) + ( (uint8_t)ebx ));
	  edx = (edx & 0xffffff00) | (uint8_t)(( (uint8_t)edx ) + ( (uint8_t)(ebx >> 8) ) + carry); }
	ed2 = ed2 + xres;
	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) != 0) goto hvolpolygon_inner;

	ebp = stack_var00;
hvolpolygon_w:
	x_y = x_y + 1;
	x = x + 1;
	eax = xres;
//while (y < yres)
	if (( (int32_t)eax ) > ( (int32_t)x )) goto hvolpolygon_x_l;

	goto hvolpolygon_fertig;
hvolpolygon_fertig2:

hvolpolygon_fertig:

hvolpolygon_weg:
	return;
}



static void vollight(uint32_t vstamp, tscreenpoint *datapos, uint32_t lightflags) {
	realnum fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13, fpu_reg14, fpu_reg15;
	uint32_t eax, edx, ecx, es2;
	tcube *ed2, *eb2;
	uint32_t *ed3, *edi;
	tidxentry *ed4, *eb3;
	uint8_t *ed5;
	tscreenpoint *ed6, *eb4;
	tlviewer *ebx;
	tviewer *esi;
	tlviewer *stack_var00;
	tviewer *stack_var01;


	tvec a, b, u;
	uint32_t xz;
	tvec vx, sx;
	uint32_t yz;
	tvec vy, sy;
	tvec cp, ca, cb, cu;
	float xa, ya, xb, yb;
	tcube *oldcube, *cubeptr;
	float *zvalues;
	uint8_t *ipmap;
	uint32_t *cubelist, *cubelists, *cubeliste;
	uint32_t cubeliststep;
	tidxentry *idxlistpos, *idxlistend;
	uint32_t idxliststep;
	float asize, bsize, bscale;

	//eax = scene.sc_zline; //rendered z-buffer lines
//   add eax,zxres*4
	zvalues = (float *)scene.sc_zline;

	//eax = scene.sc_lmap; //permission map for indices
	ipmap = (uint8_t *)scene.sc_lmap;

	//eax = ipmap + ( 256 ); //cube id's and y values
	cubelist = (uint32_t *)( ipmap + 256 ); // format: idx, y, idx, y, ..., idx


	esi = ( &(viewer) ); //transform light position into
	ebx = ( &(light) ); // local camera coordinate system
	fpu_reg10 = ( esi->v_1_tan_fov );
	fpu_reg10 = fpu_reg10 * ( ebx->lv_viewer.v_1_tan_fov );
	bscale = fpu_reg10;
	fpu_reg10 = ( ebx->lv_viewer.v_p.x1 ); //p(light) - p(camera)
	fpu_reg10 = fpu_reg10 - ( esi->v_p.x1 );
	fpu_reg11 = ( ebx->lv_viewer.v_p.x2 );
	fpu_reg11 = fpu_reg11 - ( esi->v_p.x2 );
	fpu_reg12 = ( ebx->lv_viewer.v_p.x3 );
	fpu_reg12 = fpu_reg12 - ( esi->v_p.x3 );
	cproject(esi, fpu_reg10, fpu_reg11, fpu_reg12); //transform to camera coordinate system
	cp.x3 = fpu_reg12;
	cp.x2 = fpu_reg11;
	cp.x1 = fpu_reg10;
//calc vy, sx, sy
	fpu_reg10 = ( *((float *)(((uint8_t *)scene.sc_divtab) + (( zxres / 2 ) * 4))) );
	fpu_reg11 = ( ebx->lv_l1.x1 ); //vy = -(l1 + l2*a) + l3
	vy.x1 = fpu_reg11; //sx = l1/(zxres/2)
	fpu_reg11 = fpu_reg11 * fpu_reg10;
	sx.x1 = fpu_reg11;
	fpu_reg11 = ( ebx->lv_l1.x2 );
	vy.x2 = fpu_reg11;
	fpu_reg11 = fpu_reg11 * fpu_reg10;
	sx.x2 = fpu_reg11;
	fpu_reg11 = ( ebx->lv_l1.x3 );
	vy.x3 = fpu_reg11;
	fpu_reg10 = fpu_reg10 * fpu_reg11;
	sx.x3 = fpu_reg10;

	fpu_reg10 = z_a; //sy = l2*a
	fpu_reg11 = ( ebx->lv_l2.x1 );
	fpu_reg11 = fpu_reg11 * fpu_reg10;
	sy.x1 = fpu_reg11;
	fpu_reg11 = fpu_reg11 + vy.x1;
//    fstp sy.x1
//    fld  vy.x1
	fpu_reg11 = ( ebx->lv_viewer.v_l3.x1 ) - fpu_reg11;
	vy.x1 = fpu_reg11;

	fpu_reg11 = ( ebx->lv_l2.x2 );
	fpu_reg11 = fpu_reg11 * fpu_reg10;
	sy.x2 = fpu_reg11;
	fpu_reg11 = fpu_reg11 + vy.x2;
//    fstp sy.x2
//    fld  vy.x2
	fpu_reg11 = ( ebx->lv_viewer.v_l3.x2 ) - fpu_reg11;
	vy.x2 = fpu_reg11;

	fpu_reg11 = ( ebx->lv_l2.x3 );
	fpu_reg10 = fpu_reg10 * fpu_reg11;
	sy.x3 = fpu_reg10;
	fpu_reg10 = fpu_reg10 + vy.x3;
//    fstp sy.x3
//    fld  vy.x3
	fpu_reg10 = ( ebx->lv_viewer.v_l3.x3 ) - fpu_reg10;
	vy.x3 = fpu_reg10;



	yz = ( zyres ); //-1
vollight_y_l: //y-loop
	xz = ( - 1 + zxres ); // /2

	vx.x1 = vy.x1;
	vx.x2 = vy.x2;
	vx.x3 = vy.x3;
vollight_x_l: //x-loop
//ebx -> light (tviewer)
//edi -> camera (tviewer)
//        mov     eax,[ebx].v_p.x1
//        mov     ecx,[ebx].v_p.x2
//        mov     edx,[ebx].v_p.x3
//        mov     a.x1,eax
//        mov     a.x2,ecx
//        mov     a.x3,edx

	fpu_reg10 = cp.x3; //mov     edx,cp.x3
	ca.x1 = cp.x1;
	ca.x2 = cp.x2;
	ca.x3 = fpu_reg10; //mov     ca.x3,edx               ;keep az

	oldcube = ( _NULL );
	cubeptr = ( ebx->lv_viewer.v_cube );

	fpu_reg11 = ( *zvalues );
	zvalues = zvalues + ( 1 );




	if (fpu_reg11 < c_max) goto vollight_t0;

//  ftst
//  fstsw ax
//  sahf
//  jne @@t0
//remove zvalue
//remove az
	goto vollight_w1;
vollight_t0:
	fpu_reg12 = 0.0; //1
	asize = fpu_reg12;
	fpu_reg12 = bscale;
	fpu_reg12 = fpu_reg12 * fpu_reg11;
	bsize = fpu_reg12;
//      fst bsize


	fpu_reg12 = vx.x1; //make u, a, b
	fpu_reg12 = fpu_reg12 * fpu_reg11;
	u.x1 = fpu_reg12;
	fpu_reg13 = ( ebx->lv_viewer.v_p.x1 );
	a.x1 = fpu_reg13;
	fpu_reg13 = fpu_reg13 + fpu_reg12;
//        fld     st
//        fadd    a.x1
	b.x1 = fpu_reg13;
	fpu_reg13 = vx.x2;
	fpu_reg13 = fpu_reg13 * fpu_reg11;
	u.x2 = fpu_reg13;
	fpu_reg14 = ( ebx->lv_viewer.v_p.x2 );
	a.x2 = fpu_reg14;
	fpu_reg14 = fpu_reg14 + fpu_reg13;
//        fld     st
//        fadd    a.x2
	b.x2 = fpu_reg14;
	fpu_reg14 = vx.x3;
	fpu_reg14 = fpu_reg14 * fpu_reg11;
	u.x3 = fpu_reg14;
	fpu_reg15 = ( ebx->lv_viewer.v_p.x3 );
	a.x3 = fpu_reg15;
	fpu_reg15 = fpu_reg15 + fpu_reg14;
//        fld     st
//        fadd    p.x3
	b.x3 = fpu_reg15;
	cproject(esi, fpu_reg12, fpu_reg13, fpu_reg14);
	cu.x3 = fpu_reg14;
	fpu_reg14 = fpu_reg14 + ca.x3;
	cb.x3 = fpu_reg14;
	fpu_reg11 = fpu_reg14; //remove zval & keep bz
	cu.x2 = fpu_reg13;
	fpu_reg13 = fpu_reg13 + ca.x2;
	cb.x2 = fpu_reg13;
	cu.x1 = fpu_reg12;
	fpu_reg12 = fpu_reg12 + ca.x1;
	cb.x1 = fpu_reg12;

//        fstp    st

//        fld     ca.x3
//        fld     cb.x3

	stack_var00 = ebx;
	stack_var01 = esi;

//z-clipping
	fpu_reg12 = 1.0; //z-clip line a-b

//az < 1.0?
	eax = /*(eax & 0xffff0000) |*/ ((fpu_reg12 < fpu_reg10)?0x100:0);
	edx = /*(edx & 0xffffff00) |*/ (uint8_t)(( (uint8_t)(eax >> 8) ));
//bz < 1.0?
	eax = /*(eax & 0xffff0000) |*/ ((fpu_reg12 < fpu_reg11)?0x100:0);
	eax = set_high_byte(eax, ( (uint8_t)(eax >> 8) ) ^ ( (uint8_t)edx ));

	if ((eax & 0x100) == 0) goto vollight_Nclip;
//clip

//r = (1.0-az)/(bz-az)
	fpu_reg11 = fpu_reg11 - fpu_reg10;
	fpu_reg12 = 1.0;
	fpu_reg10 = fpu_reg12 - fpu_reg10;
	fpu_reg10 = fpu_reg10 / fpu_reg11; //st(0) = r


	if ((( (uint8_t)edx ) & ( 1 )) != 0) goto vollight_bclip;
//az < 1.0
	fpu_reg11 = 1.0;
	fpu_reg10 = fpu_reg11 - fpu_reg10; //r = 1-r
//st(0) = r(z=0)
vollight_t_l: //find crossing point (a+r*u) in cubes
	ed2 = rtrace(&(b), &(u), oldcube, cubeptr, fpu_reg11);

	if (ed2 == 0) goto vollight_0;
	fpu_reg11 = -fpu_reg11;
//trace cubes until r(z=0) < r(cube)


	if (fpu_reg11 > fpu_reg10) goto vollight_0;

	oldcube = cubeptr;
	cubeptr = ed2;
	goto vollight_t_l;
vollight_0:


	fpu_reg11 = u.x1; //recalc u and a
	fpu_reg11 = fpu_reg11 * fpu_reg10;
	u.x1 = fpu_reg11;
	fpu_reg11 = b.x1 - fpu_reg11;
	a.x1 = fpu_reg11;
	fpu_reg11 = u.x2;
	fpu_reg11 = fpu_reg11 * fpu_reg10;
	u.x2 = fpu_reg11;
	fpu_reg11 = b.x2 - fpu_reg11;
	a.x2 = fpu_reg11;
	fpu_reg11 = u.x3;
	fpu_reg11 = fpu_reg11 * fpu_reg10;
	u.x3 = fpu_reg11;
	fpu_reg11 = b.x3 - fpu_reg11;
	a.x3 = fpu_reg11;

	fpu_reg11 = cu.x1; //recalc cu and ca
	fpu_reg11 = fpu_reg11 * fpu_reg10;
	cu.x1 = fpu_reg11;
	fpu_reg11 = cb.x1 - fpu_reg11;
	ca.x1 = fpu_reg11;
	fpu_reg11 = cu.x2;
	fpu_reg10 = fpu_reg10 * fpu_reg11; //remove r
	cu.x2 = fpu_reg10;
	fpu_reg10 = cb.x2 - fpu_reg10;
	ca.x2 = fpu_reg10;
	fpu_reg10 = 1.0;
	ca.x3 = fpu_reg10; //ca.x3 = 1.0
	fpu_reg10 = cb.x3 - fpu_reg10;
	cu.x3 = fpu_reg10; //cu.x3 = cb.x3 - 1.0

	goto vollight_Nw;
vollight_bclip: //bz < 1.0

	fpu_reg11 = u.x1; //recalc u and b
	fpu_reg11 = fpu_reg11 * fpu_reg10;
	u.x1 = fpu_reg11;
	fpu_reg11 = fpu_reg11 + a.x1;
	b.x1 = fpu_reg11;
	fpu_reg11 = u.x2;
	fpu_reg11 = fpu_reg11 * fpu_reg10;
	u.x2 = fpu_reg11;
	fpu_reg11 = fpu_reg11 + a.x2;
	b.x2 = fpu_reg11;
	fpu_reg11 = u.x3;
	fpu_reg11 = fpu_reg11 * fpu_reg10;
	u.x3 = fpu_reg11;
	fpu_reg11 = fpu_reg11 + a.x3;
	b.x3 = fpu_reg11;

	fpu_reg11 = cu.x1; //recalc cu and cb
	fpu_reg11 = fpu_reg11 * fpu_reg10;
	cu.x1 = fpu_reg11;
	fpu_reg11 = fpu_reg11 + ca.x1;
	cb.x1 = fpu_reg11;
//  fld     cu.x3                   ;recalc cu and cb
//  fmul    st,st(1)
//  fst     cu.x3
//  fadd    ca.x3
//  fstp    cb.x3
	fpu_reg11 = cu.x2;
	fpu_reg11 = fpu_reg11 * fpu_reg10;
	cu.x2 = fpu_reg11;
	fpu_reg11 = fpu_reg11 + ca.x2;
	cb.x2 = fpu_reg11;
	fpu_reg11 = 1.0;
	cb.x3 = fpu_reg11; //cb.x3 = 1.0
	fpu_reg11 = fpu_reg11 - ca.x3;
	cu.x3 = fpu_reg11; //cu.x3 = 1.0 - ca.x3

	fpu_reg10 = fpu_reg10 * bsize; //remove r
	bsize = fpu_reg10;


	goto vollight_Nw;
vollight_Nclip:




	if ((( (uint8_t)edx ) & ( 1 )) == 0) goto vollight_w; //line not visible

vollight_Nw:

	memset(ipmap, 0, 256); //clear index permission map

//test direction (horizontal/vertical)
	fpu_reg10 = ca.x1;
	fpu_reg10 = fpu_reg10 / ca.x3;
	xa = fpu_reg10;
	fpu_reg11 = cb.x1;
	fpu_reg11 = fpu_reg11 / cb.x3;
	xb = fpu_reg11;
	fpu_reg10 = fpu_reg10 - fpu_reg11;
	fpu_reg10 = fabs(fpu_reg10);

	fpu_reg11 = ca.x2;
	fpu_reg11 = fpu_reg11 / ca.x3;
	ya = fpu_reg11;
	fpu_reg12 = cb.x2;
	fpu_reg12 = fpu_reg12 / cb.x3;
	yb = fpu_reg12;
	fpu_reg11 = fpu_reg11 - fpu_reg12;
	fpu_reg11 = fabs(fpu_reg11);




	if (fpu_reg11 < fpu_reg10) goto vollight_hor;
//vertical
	edi = cubelist;

vollight_t_lv: //make cube idx and y list
	eb2 = cubeptr; // format: idx, y, idx, y, ..., idx

	eax = ( eb2->c_vstamp );

	if (eax == vstamp) goto vollight_notv0;
	eax = ( - 1 );
	goto vollight_v0;
vollight_notv0:
	eax = ( eb2->c_vtc->tc_idx );
vollight_v0:
	*edi = eax;

	ed2 = rtrace(&(b), &(u), oldcube, eb2, fpu_reg10);

	if (ed2 == 0) goto vollight_v1; //no more cubes to trace

	oldcube = cubeptr;
	cubeptr = ed2;

	fpu_reg11 = cu.x2; //y = cb.x2+(r-1)*cu.x2
	fpu_reg11 = fpu_reg11 * fpu_reg10;
	fpu_reg11 = fpu_reg11 + cb.x2;

	fpu_reg12 = cu.x3; //z = cb.x3+(r-1)*cu.x3
	fpu_reg12 = fpu_reg12 * fpu_reg10;
	fpu_reg12 = fpu_reg12 + cb.x3;

	fpu_reg11 = fpu_reg11 / fpu_reg12; //sy = y/z

	edi[1] = (int32_t)ceil(fpu_reg11); //store y
	edi = edi + ( 2 );

	goto vollight_t_lv;
vollight_v1:
//edi = cubelist end
	ed3 = cubelist; //edx = cubelist start
	ecx = ( 8 ); //ecx = cubelist step


	fpu_reg10 = cb.x3; //z distance from camera of b
	fpu_reg11 = ca.x3; //z distance from camera of a

	fpu_reg12 = ya; //default: a higher on screen



	if (fpu_reg12 < yb) goto vollight_v4;
//b higher on screen
	{ realnum tmp = fpu_reg10; fpu_reg10 = fpu_reg11; fpu_reg11 = tmp; } //st(0) = z distance of higher point
	{ uint32_t *value = edi; edi = ed3; ed3 = value; } //exchange cubelists and cubeliste
	ecx = - ( (int32_t)ecx );
vollight_v4:
	cubelists = ed3;
	cubeliste = edi;
	cubeliststep = ecx;
//st(0) = z value of higher point
//st(1) = z value of lower point

//ipmap for objects
	eb3 = (tidxentry *)scene.sc_idxlist; //make index permission map for
	//edi = ipmap; // highest point
vollight_i_lv:

	if (eb3 >= (tidxentry *)scene.sc_idxlistend) goto vollight_v5;
//search lesser z value in idxlist


	if (fpu_reg11 > ( eb3->i_zval )) goto vollight_v5;
	eax = ( eb3->i_idx );
	*((uint8_t *)(ipmap + eax)) = (uint8_t) (( 1 ));
	eb3 = eb3 + ( 1 );
	goto vollight_i_lv;
vollight_v5:

	ed4 = (tidxentry *)scene.sc_idxlistend; //default: decreasing z-values
	ecx = ( sizeof(tidxentry) ); //increase addresses
//ebx -> first idxentry to test
//  st(1)                   ;remove z values


	if (fpu_reg11 > fpu_reg10) goto vollight_v6;
	ed4 = (tidxentry *)scene.sc_idxlist; //increasing zvalues
//at beginning of buffer?
	if (eb3 <= ed4) goto vollight_v6;
	eb3 = (tidxentry *)( ((uintptr_t)eb3) - ecx ); //ebx -> first idxentry to test
	ecx = - ( (int32_t)ecx ); //decrease addresses
vollight_v6:
	idxlistpos = eb3;
	idxlistend = ed4;
	idxliststep = ecx;


	fpu_reg10 = zxw; //x_width
//negative if b higher a
	if (( (int32_t)cubeliststep ) < ( 0 )) goto vollight_v9;
	fpu_reg10 = -fpu_reg10;
vollight_v9:

	ed5 = (uint8_t *)datapos;
	eb4 = (tscreenpoint *)ed5;

	fpu_reg11 = ca.x3; //st = z
	fpu_reg11 = fpu_reg10 / fpu_reg11; //st = x_width/z
//  fld z_csize
//  fadd asize
//  fmulp st(1),st
	fpu_reg11 = fpu_reg11 * asize;

	fpu_reg12 = xa; //st = x/z
	fpu_reg12 = fpu_reg12 + fpu_reg11;
	((tscreenpoint *)ed5)->sp_x = fpu_reg12; //x/z + x_width/z
	fpu_reg11 = xa - fpu_reg11;
	((tscreenpoint *)(ed5 + volpsize))->sp_x = fpu_reg11; //x/z - x_width/z
	fpu_reg11 = ya; //st = y/z
	((tscreenpoint *)ed5)->sp_y = fpu_reg11;
	((tscreenpoint *)(ed5 + volpsize))->sp_y = fpu_reg11;
	fpu_reg11 = 0.0;
	((tscreenpoint *)ed5)->sp_u = fpu_reg11;
	fpu_reg11 = c_umax;
	((tscreenpoint *)(ed5 + volpsize))->sp_u = fpu_reg11;

	ed5 = ( ed5 + (2 * volpsize) );

	fpu_reg11 = cb.x3; //st = z
	fpu_reg10 = fpu_reg10 / fpu_reg11; //st = x_width/z
//  fld z_csize
//  fadd bsize
//  fmulp st(1),st
	fpu_reg10 = fpu_reg10 * bsize;

	fpu_reg11 = xb; //st = x/z
	fpu_reg11 = fpu_reg11 - fpu_reg10;
	((tscreenpoint *)ed5)->sp_x = fpu_reg11; //x/z - x_width/z
	fpu_reg10 = fpu_reg10 + xb;
	((tscreenpoint *)(ed5 + volpsize))->sp_x = fpu_reg10; //x/z + x_width/z
	fpu_reg10 = yb; //st = y/z
	((tscreenpoint *)ed5)->sp_y = fpu_reg10;
	((tscreenpoint *)(ed5 + volpsize))->sp_y = fpu_reg10;
	fpu_reg10 = c_umax;
	((tscreenpoint *)ed5)->sp_u = fpu_reg10;
	fpu_reg10 = 0.0;
	((tscreenpoint *)(ed5 + volpsize))->sp_u = fpu_reg10;

	ed5 = ( ed5 + (2 * volpsize) );

	ed6 = (tscreenpoint *)ed5;
	eax = xclip(( volpsize ), ( volvars ), eb4, ed6);

	if (( (int32_t)eax ) != 0) goto vollight_w;



	if ((lightflags & ( lfWater )) != 0) goto vollight_vwa0;
	es2 = ( ( fogmapsteps - 1 ) * fogmapsize );
	goto vollight_vwa1;
vollight_vwa0:
	eax = frame;
	edx = yz;
	edx = edx << ( zxshift + 3 );
	eax = eax + edx;
	eax = eax & ( 0x1F << zxshift ); //time-resolution = 32
	eax = eax + xz;
	es2 = (uint32_t)( *((uint8_t *)(eax + (uint8_t *)scene.sc_fogwater)) );
	es2 = es2 << ( ld_fogmapsize ); //*fogmapsize
vollight_vwa1:

	vvolpolygon(eb4, (uintptr_t)ed6 - (uintptr_t)eb4, cubelists, cubeliste, cubeliststep, idxlistpos, idxlistend, idxliststep, &(ca), &(cu), ipmap, es2 + (uint8_t *)scene.sc_fogmap);
// pascal, ebx, edi,
//  cubelists, cubeliste, cubeliststep,
//  idxlistpos, idxlistend, idxliststep, ca, cu, ipmap, mapptr

	goto vollight_w; //pop esi edx ;jmp @@weg
vollight_hor: //horizontal
//jmp @@w
	edi = cubelist;

vollight_t_lh: //make cube idx and x list
	eb2 = cubeptr; // format: idx, x, idx, x, ..., idx

	eax = ( eb2->c_vstamp );

	if (eax == vstamp) goto vollight_noth0;
	eax = ( - 1 );
	goto vollight_h0;
vollight_noth0:
	eax = ( eb2->c_vtc->tc_idx );
vollight_h0:
	*edi = eax;

	ed2 = rtrace(&(b), &(u), oldcube, eb2, fpu_reg10);

	if (ed2 == 0) goto vollight_h1; //no more cubes to trace

	oldcube = cubeptr;
	cubeptr = ed2;

	fpu_reg11 = cu.x1; //x = cb.x1+(r-1)*cu.x1
	fpu_reg11 = fpu_reg11 * fpu_reg10;
	fpu_reg11 = fpu_reg11 + cb.x1;

	fpu_reg12 = cu.x3; //z = cb.x3+(r-1)*cu.x3
	fpu_reg12 = fpu_reg12 * fpu_reg10;
	fpu_reg12 = fpu_reg12 + cb.x3;

	fpu_reg11 = fpu_reg11 / fpu_reg12; //sx = x/z

	edi[1] = (int32_t)ceil(fpu_reg11); //store x
	edi = edi + ( 2 );

	goto vollight_t_lh;
vollight_h1:
//edi = cubelist end
	ed3 = cubelist; //edx = cubelist start
	ecx = ( 8 ); //ecx = cubelist step


	fpu_reg10 = cb.x3; //z distance from camera of b
	fpu_reg11 = ca.x3; //z distance from camera of a

	fpu_reg12 = xa; //default: a more left on screen



	if (fpu_reg12 < xb) goto vollight_h4;
//b more left on screen
	{ realnum tmp = fpu_reg10; fpu_reg10 = fpu_reg11; fpu_reg11 = tmp; } //st(0) = z distance of higher point
	{ uint32_t *value = edi; edi = ed3; ed3 = value; } //exchange cubelists and cubeliste
	ecx = - ( (int32_t)ecx );
vollight_h4:
	cubelists = ed3;
	cubeliste = edi;
	cubeliststep = ecx;
//st(0) = z value of left point
//st(1) = z value of right point

//ipmap for objects
	eb3 = (tidxentry *)scene.sc_idxlist; //make index permission map for
	//edi = ipmap; // left point
vollight_i_lh:

	if (eb3 >= (tidxentry *)scene.sc_idxlistend) goto vollight_h5;
//search lesser z value in idxlist


	if (fpu_reg11 > ( eb3->i_zval )) goto vollight_h5;
	eax = ( eb3->i_idx );
	*((uint8_t *)(ipmap + eax)) = (uint8_t) (( 1 ));
	eb3 = eb3 + ( 1 );
	goto vollight_i_lh;
vollight_h5:

	ed4 = (tidxentry *)scene.sc_idxlistend; //default: decreasing z-values
	ecx = ( sizeof(tidxentry) ); //increase addresses
//ebx -> first idxentry to test
//  st(1)                   ;remove z values


	if (fpu_reg11 > fpu_reg10) goto vollight_h6;
	ed4 = (tidxentry *)scene.sc_idxlist; //increasing zvalues
//at beginning of buffer?
	if (eb3 <= ed4) goto vollight_h6;
	eb3 = (tidxentry *)( ((uintptr_t)eb3) - ecx); //ebx -> first idxentry to test
	ecx = - ( (int32_t)ecx ); //decrease addresses
vollight_h6:
	idxlistpos = eb3;
	idxlistend = ed4;
	idxliststep = ecx;


	fpu_reg10 = zyw; //y_width
//negative if b more left than a
	if (( (int32_t)cubeliststep ) < ( 0 )) goto vollight_h9;
	fpu_reg10 = -fpu_reg10;
vollight_h9:

	ed5 = (uint8_t *)datapos;
	eb4 = (tscreenpoint *)ed5;

	fpu_reg11 = ca.x3; //st = z
	fpu_reg11 = fpu_reg10 / fpu_reg11; //st = y_width/z
//  fld z_csize
//  fadd asize
//  fmulp st(1),st
	fpu_reg11 = fpu_reg11 * asize;

	fpu_reg12 = ya; //st = y/z
	fpu_reg12 = fpu_reg12 + fpu_reg11;
	((tscreenpoint *)ed5)->sp_y = fpu_reg12; //y/z + y_width/z
	fpu_reg11 = ya - fpu_reg11;
	((tscreenpoint *)(ed5 + volpsize))->sp_y = fpu_reg11; //y/z - y_width/z
	fpu_reg11 = xa; //st = x/z
	((tscreenpoint *)ed5)->sp_x = fpu_reg11;
	((tscreenpoint *)(ed5 + volpsize))->sp_x = fpu_reg11;
	fpu_reg11 = 0.0;
	((tscreenpoint *)ed5)->sp_u = fpu_reg11;
	fpu_reg11 = c_umax;
	((tscreenpoint *)(ed5 + volpsize))->sp_u = fpu_reg11;

	ed5 = ( ed5 + (2 * volpsize) );

	fpu_reg11 = cb.x3; //st = z
	fpu_reg10 = fpu_reg10 / fpu_reg11; //st = y_width/z
//  fld z_csize
//  fadd bsize
//  fmulp st(1),st
	fpu_reg10 = fpu_reg10 * bsize;

	fpu_reg11 = yb; //st = y/z
	fpu_reg11 = fpu_reg11 - fpu_reg10;
	((tscreenpoint *)ed5)->sp_y = fpu_reg11; //y/z - y_width/z
	fpu_reg10 = fpu_reg10 + yb;
	((tscreenpoint *)(ed5 + volpsize))->sp_y = fpu_reg10; //y/z + y_width/z
	fpu_reg10 = xb; //st = x/z
	((tscreenpoint *)ed5)->sp_x = fpu_reg10;
	((tscreenpoint *)(ed5 + volpsize))->sp_x = fpu_reg10;
	fpu_reg10 = c_umax;
	((tscreenpoint *)ed5)->sp_u = fpu_reg10;
	fpu_reg10 = 0.0;
	((tscreenpoint *)(ed5 + volpsize))->sp_u = fpu_reg10;

	ed5 = ( ed5 + (2 * volpsize) );

	ed6 = (tscreenpoint *)ed5;
	eax = yclip(( volpsize ), ( volvars ), eb4, ed6);

	if (( (int32_t)eax ) != 0) goto vollight_w;



	if ((lightflags & ( lfWater )) != 0) goto vollight_hwa0;
	es2 = ( ( fogmapsteps - 1 ) * fogmapsize );
	goto vollight_hwa1;
vollight_hwa0:
	eax = frame;
	edx = yz;
	edx = edx << ( zxshift + 2 );
	eax = eax + edx;
	eax = eax & ( 0x1F << zxshift ); //time-resolution = 32
	eax = eax + xz;
	es2 = (uint32_t)( *((uint8_t *)(eax + (uint8_t *)scene.sc_fogwater)) );
	es2 = es2 << ( ld_fogmapsize ); //*fogmapsize
vollight_hwa1:

	hvolpolygon(eb4, (uintptr_t)ed6 - (uintptr_t)eb4, cubelists, cubeliste, cubeliststep, idxlistpos, idxlistend, idxliststep, &(ca), &(cu), ipmap, es2 + (uint8_t *)scene.sc_fogmap);
// pascal, ebx, edi,
//  cubelists, cubeliste, cubeliststep,
//  idxlistpos, idxlistend, idxliststep, ca, cu, ipmap, mapptr





vollight_w:
	esi = stack_var01;
	ebx = stack_var00;
vollight_w1:
//comment #
	fpu_reg10 = sx.x1; //update in x direction
	fpu_reg10 = fpu_reg10 + vx.x1;
	vx.x1 = fpu_reg10;
	fpu_reg10 = sx.x2;
	fpu_reg10 = fpu_reg10 + vx.x2;
	vx.x2 = fpu_reg10;
	fpu_reg10 = sx.x3;
	fpu_reg10 = fpu_reg10 + vx.x3;
	vx.x3 = fpu_reg10;
//#
	xz = ( (int32_t)xz ) - 1;
	if (( (int32_t)xz ) >= 0) goto vollight_x_l;
//comment #
	fpu_reg10 = sy.x1; //update in y direction
	fpu_reg10 = fpu_reg10 + vy.x1;
	vy.x1 = fpu_reg10;
	fpu_reg10 = sy.x2;
	fpu_reg10 = fpu_reg10 + vy.x2;
	vy.x2 = fpu_reg10;
	fpu_reg10 = sy.x3;
	fpu_reg10 = fpu_reg10 + vy.x3;
	vy.x3 = fpu_reg10;
//#
	yz = ( (int32_t)yz ) - 1;
	if (( (int32_t)yz ) != 0) goto vollight_y_l;


//vollight_weg:
	return;
}



static void ldraw(uint32_t idx, uint32_t recursion, tscreenpoint *ebx, tscreenpoint *edi) {
	realnum fpu_reg10;
	uint32_t eax, edx;
	tscreenpoint *stack_var00, *stack_var01;
//mapptr, idx, recursion
//-> ebx -> sp
//-> edi -> sp_end

	fpu_reg10 = 0.0; //st = abstand = 0.0
	eax = 0; //x-clip
	edx = ( offsetof(tscreenpoint, sp_lx) );
	eax = lightclip(( lightpsize ), eax, ebx, edx, edi, fpu_reg10); //, lightvars

	if (( (int32_t)eax ) != 0) goto ldraw_0;

	eax = 0;
	eax = eax - 1;
	edx = ( offsetof(tscreenpoint, sp_lx) );
	eax = lightclip(( lightpsize ), eax, ebx, edx, edi, fpu_reg10); //, lightvars

	if (( (int32_t)eax ) != 0) goto ldraw_0;

	eax = 0; //y-clip
	edx = ( offsetof(tscreenpoint, sp_ly) );
	eax = lightclip(( lightpsize ), eax, ebx, edx, edi, fpu_reg10); //, lightvars

	if (( (int32_t)eax ) != 0) goto ldraw_0;

	eax = 0;
	eax = eax - 1;
	edx = ( offsetof(tscreenpoint, sp_ly) );
	eax = lightclip(( lightpsize ), eax, ebx, edx, edi, fpu_reg10); //, lightvars

	if (( (int32_t)eax ) != 0) goto ldraw_0;
//remove distance

	fpu_reg10 = 1.0; //st = abstand = 1.0
	eax = 0; //z-clip
	edx = ( offsetof(tscreenpoint, sp_lz) );
	eax = lightclip(( lightpsize ), eax, ebx, edx, edi, fpu_reg10); //, lightvars

	if (( (int32_t)eax ) != 0) goto ldraw_0;
//remove distance

//draw onto z-line
//volume light turned on?
	if ((light.lv_flags & ( lfVol )) == 0) goto ldraw_Nvol;
	stack_var00 = ebx;
	stack_var01 = edi;
	projectline(( lightpsize ), ebx, edi);
	renderline(ebx, (uintptr_t)edi - (uintptr_t)ebx);
	edi = stack_var01;
	ebx = stack_var00;
ldraw_Nvol:

	lscalexy(( lightpsize ), ebx, edi);

	eax = projection(( lightpsize ), ( lightvars ), ebx, edi);

	if (( (int32_t)eax ) != 0) goto ldraw_weg;

//      jmp @@_sub
	if (recursion > ( l_sub )) goto ldraw__sub;
	eax = subxclip(( lightpsize ), ( lightvars ), ebx, edi);

	if (( (int32_t)eax ) != 0) goto ldraw_weg;
	eax = idx;
	eax = eax << ( 8 );
	sublpolygon(ebx, (uintptr_t)edi - (uintptr_t)ebx, eax); //mapptr, eax
	goto ldraw_weg;
ldraw__sub: //ohne subdivision
	edi = lconvert(ebx, edi);
	eax = xclip(( flatpsize ), ( flatvars ), ebx, edi);

	if (( (int32_t)eax ) != 0) goto ldraw_weg;
	lpolygon(ebx, (uintptr_t)edi - (uintptr_t)ebx, idx); //mapptr, eax
	goto ldraw_weg;
ldraw_0:

ldraw_weg:
	return;
}


static void gldraw(uint32_t idx, tscreenpoint *ebx, tscreenpoint *edi) {
	realnum fpu_reg10;
	uint32_t eax, edx;
	tscreenpoint *stack_var00, *stack_var01;
//mapptr:dword, idx:dword
//-> ebx -> sp
//-> edi -> sp_end

	fpu_reg10 = 0.0; //st = abstand = 0.0
	eax = 0; //x-clip
	edx = ( offsetof(tscreenpoint, sp_lx) );
	eax = lightclip(( glightpsize ), eax, ebx, edx, edi, fpu_reg10); //, glightvars

	if (( (int32_t)eax ) != 0) goto gldraw_0;

	eax = 0;
	eax = eax - 1;
	edx = ( offsetof(tscreenpoint, sp_lx) );
	eax = lightclip(( glightpsize ), eax, ebx, edx, edi, fpu_reg10); //, glightvars

	if (( (int32_t)eax ) != 0) goto gldraw_0;

	eax = 0; //y-clip
	edx = ( offsetof(tscreenpoint, sp_ly) );
	eax = lightclip(( glightpsize ), eax, ebx, edx, edi, fpu_reg10); //, glightvars

	if (( (int32_t)eax ) != 0) goto gldraw_0;

	eax = 0;
	eax = eax - 1;
	edx = ( offsetof(tscreenpoint, sp_ly) );
	eax = lightclip(( glightpsize ), eax, ebx, edx, edi, fpu_reg10); //, glightvars

	if (( (int32_t)eax ) != 0) goto gldraw_0;
//remove distance

	fpu_reg10 = 1.0; //st = abstand = 1.0
	eax = 0; //y-clip
	edx = ( offsetof(tscreenpoint, sp_lz) );
	eax = lightclip(( glightpsize ), eax, ebx, edx, edi, fpu_reg10); //, glightvars

	if (( (int32_t)eax ) != 0) goto gldraw_0;
//remove distance

//draw onto z-line
//volume light turned on?
	if ((light.lv_flags & ( lfVol )) == 0) goto gldraw_Nvol;
	stack_var00 = ebx;
	stack_var01 = edi;
	projectline(( glightpsize ), ebx, edi);
	renderline(ebx, (uintptr_t)edi - (uintptr_t)ebx);
	edi = stack_var01;
	ebx = stack_var00;
gldraw_Nvol:

	lscalexy(( glightpsize ), ebx, edi);

	eax = projection(( glightpsize ), ( glightvars ), ebx, edi);

	if (( (int32_t)eax ) != 0) goto gldraw_weg;

	edi = glconvert(ebx, edi);
	eax = xclip(( gouraudpsize ), ( gouraudvars ), ebx, edi);

	if (( (int32_t)eax ) != 0) goto gldraw_weg;
	glpolygon(ebx, (uintptr_t)edi - (uintptr_t)ebx, idx); //mapptr, idx
	goto gldraw_weg;
gldraw_0:
gldraw_weg:
	return;
}



static void makelgouraud(void) {
	uint32_t eax, edx, ecx;
	uint8_t *ec2;

	ec2 = (uint8_t *)scene.sc_lmap;
	ec2 = ec2 + ( 65536 );

	ecx = set_high_byte(/*ecx*/ 0, ( lvalues - 1 ));
makelgouraud_t_l: //texture light value loop

	ecx = (ecx & 0xffffff00) | (uint8_t)(( 127 ));
makelgouraud_g_l: //gouraud light value loop
	eax = (int32_t)( (int8_t)ecx );
	eax = ( (int32_t)eax ) - ( 12 );
	if (( (int32_t)eax ) < 0) goto makelgouraud_4;
	edx = (uint32_t)( (uint8_t)(ecx >> 8) );
	eax = ( (int32_t)eax ) * ( (int32_t)edx );
	eax = eax >> ( 5 );

	if (eax < edx) goto makelgouraud_5;
	eax = edx;

	goto makelgouraud_5;
makelgouraud_4:
	eax = 0;
makelgouraud_5:

	*((uint8_t *)(ec2 + ecx)) = (uint8_t) (( (uint8_t)eax ));

	ecx = (ecx & 0xffffff00) | (uint8_t)(( (uint8_t)ecx ) - 1);
	if (( (uint8_t)ecx ) != 0x7f) goto makelgouraud_g_l;

	{ uint32_t carry = (( (uint8_t)(ecx >> 8) ) < ( 1 ))?1:0; ecx = set_high_byte(ecx, ( (uint8_t)(ecx >> 8) ) - ( 1 ));
	  if (carry == 0) goto makelgouraud_t_l; }


//makelgouraud_weg:
	return;
}


static void ldrawface(uint32_t vstamp, tface *ebx, tmesh *esi, tscreenpoint *&_edi) {
	realnum fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13, fpu_reg14, fpu_reg15, fpu_reg16;
	uint32_t eax, ed2, ecx;
	uint8_t *edx, *edi = (uint8_t *)_edi, *es2;
	tfp *eb2;
	tscreenpoint *ed3, *eb3, *stack_var00;
	tmesh *stack_var01;

	uint32_t idx;
//-> ebx -> tface
//-> esi -> tmesh
//-> edi -> shadow faces buffer pos
//<- edi -> new shadow faces buffer pos

//visibility test
//(a          ) x (b          ) *  p
//(v[0] - v[1]) x (v[2] - v[1]) * (v[1])

	eax = ( ebx->f_p[0].fp_vertex ); //eax -> v[0]
	ed2 = ( ebx->f_p[1].fp_vertex ); //edx -> v[1]
	ecx = ( ebx->f_p[2].fp_vertex ); //ecx -> v[2]

	stack_var01 = esi;
	es2 = (uint8_t *)( esi->m_tlist );

	fpu_reg10 = ( ((tvec *)(es2 + ed2))->x1 ); //v[1] = p
	fpu_reg11 = ( ((tvec *)(es2 + ed2))->x2 );
	fpu_reg12 = ( ((tvec *)(es2 + ed2))->x3 );
	((tscreenpoint *)(edi + shadowpsize))->sp_z = fpu_reg12;

//1. komponente
	fpu_reg13 = ( ((tvec *)(es2 + eax))->x2 );
	((tscreenpoint *)edi)->sp_y = fpu_reg13; //store shadow face in buffer
	fpu_reg13 = fpu_reg13 - fpu_reg11; //a2
	fpu_reg14 = ( ((tvec *)(es2 + ecx))->x3 );
	((tscreenpoint *)(edi + 2 * shadowpsize))->sp_z = fpu_reg14;
	fpu_reg14 = fpu_reg14 - fpu_reg12; //b3
	fpu_reg13 = fpu_reg13 * fpu_reg14;

	fpu_reg14 = ( ((tvec *)(es2 + eax))->x3 );
	fpu_reg14 = fpu_reg14 - fpu_reg12; //a3
	fpu_reg15 = ( ((tvec *)(es2 + ecx))->x2 );
	fpu_reg15 = fpu_reg15 - fpu_reg11; //b2
	fpu_reg14 = fpu_reg14 * fpu_reg15;

	fpu_reg13 = fpu_reg13 - fpu_reg14;
	fpu_reg13 = fpu_reg13 * fpu_reg10; //*p1

//2. komponente
	fpu_reg14 = ( ((tvec *)(es2 + eax))->x3 );
	((tscreenpoint *)edi)->sp_z = fpu_reg14;
	fpu_reg14 = fpu_reg14 - fpu_reg12; //a3
	fpu_reg15 = ( ((tvec *)(es2 + ecx))->x1 );
	((tscreenpoint *)(edi + 2 * shadowpsize))->sp_x = fpu_reg15;
	fpu_reg15 = fpu_reg15 - fpu_reg10; //b1
	fpu_reg14 = fpu_reg14 * fpu_reg15;

	fpu_reg15 = ( ((tvec *)(es2 + eax))->x1 );
	fpu_reg15 = fpu_reg15 - fpu_reg10; //a1
	fpu_reg16 = ( ((tvec *)(es2 + ecx))->x3 );
	fpu_reg16 = fpu_reg16 - fpu_reg12; //b3
	fpu_reg15 = fpu_reg15 * fpu_reg16;

	fpu_reg14 = fpu_reg14 - fpu_reg15;
	fpu_reg14 = fpu_reg14 * fpu_reg11; //*p2
	fpu_reg13 = fpu_reg13 + fpu_reg14;

//3. komponente
	fpu_reg14 = ( ((tvec *)(es2 + eax))->x1 );
	((tscreenpoint *)edi)->sp_x = fpu_reg14;
	fpu_reg14 = fpu_reg14 - fpu_reg10; //a1
	fpu_reg15 = ( ((tvec *)(es2 + ecx))->x2 );
	((tscreenpoint *)(edi + 2 * shadowpsize))->sp_y = fpu_reg15;
	fpu_reg15 = fpu_reg15 - fpu_reg11; //b2
	fpu_reg14 = fpu_reg14 * fpu_reg15;

	fpu_reg15 = ( ((tvec *)(es2 + eax))->x2 );
	fpu_reg15 = fpu_reg15 - fpu_reg11; //a2
	fpu_reg16 = ( ((tvec *)(es2 + ecx))->x1 );
	fpu_reg16 = fpu_reg16 - fpu_reg10; //b1
	fpu_reg15 = fpu_reg15 * fpu_reg16;

	fpu_reg14 = fpu_reg14 - fpu_reg15;
	fpu_reg12 = fpu_reg12 * fpu_reg14; //*p3
	fpu_reg12 = fpu_reg12 + fpu_reg13;




	if (fpu_reg12 >= 0.0) goto ldrawface_notshadow;


	((tscreenpoint *)(edi + shadowpsize))->sp_y = fpu_reg11;
	((tscreenpoint *)(edi + shadowpsize))->sp_x = fpu_reg10;

	esi = stack_var01; //esi -> tmesh

	goto ldrawface_shadow;
ldrawface_notshadow:

	((tscreenpoint *)(edi + shadowpsize))->sp_y = fpu_reg11;
	((tscreenpoint *)(edi + shadowpsize))->sp_x = fpu_reg10;

	esi = stack_var01; //esi -> tmesh

//face lit (discard shadow face)
	eax = vstamp;
//object seen from camera?
	if (eax != ( esi->m_vstamp )) goto ldrawface_weg;

	eax = ( ebx->f_idx );
//face visible?
	if (( (int32_t)eax ) < 0) goto ldrawface_zline;
	idx = eax;

//if (offset f_p) gt 0
	eb2 = ( &(ebx->f_p[0]) ); //ebx -> tfp
//endif
	eax = ( esi->m_vertices ); //eax = number of vertices
	edx = (uint8_t *)( esi->m_tlist ); //edx -> vertices from light's view
	eax = ( eax + eax * 2 ); //eax*3
	edx = ( edx + eax * 4 ); //edx -> normals

	eax = ( eb2[2].fp_vertex );
	fpu_reg10 = ( *((float *)(edx + eax)) ); //.x3



	if (fpu_reg10 < c_lgouraud) goto ldrawface_g3;
	eax = ( eb2[1].fp_vertex );
	fpu_reg11 = ( *((float *)(edx + eax)) ); //.x3



	if (fpu_reg11 < c_lgouraud) goto ldrawface_g2;
	eax = ( eb2->fp_vertex );
	fpu_reg12 = ( *((float *)(edx + eax)) ); //.x3



	if (fpu_reg12 < c_lgouraud) goto ldrawface_g1;

	edx = (uint8_t *)( esi->m_vtlist ); //edx -> vertices from camera's view
	es2 = (uint8_t *)( esi->m_tlist ); //esi -> vertices from light's view
//edi -> mem for lightpoints

//without gouraud shading
	stack_var00 = (tscreenpoint *)edi;

	eax = ( eb2->fp_vertex );
	fpu_reg13 = ( ((tvec *)(edx + eax))->x1 );
	((tscreenpoint *)edi)->sp_x = fpu_reg13;
	fpu_reg13 = ( ((tvec *)(edx + eax))->x2 );
	((tscreenpoint *)edi)->sp_y = fpu_reg13;
	fpu_reg13 = ( ((tvec *)(edx + eax))->x3 );
	((tscreenpoint *)edi)->sp_z = fpu_reg13;
	fpu_reg13 = ( ((tvec *)(es2 + eax))->x1 );
	((tscreenpoint *)edi)->sp_lx = fpu_reg13;
	fpu_reg13 = ( ((tvec *)(es2 + eax))->x2 );
	((tscreenpoint *)edi)->sp_ly = fpu_reg13;
	fpu_reg13 = ( ((tvec *)(es2 + eax))->x3 );
	((tscreenpoint *)edi)->sp_lz = fpu_reg13;


	eb2 = eb2 + ( 1 );
	edi = edi + ( lightpsize );

	eax = ( eb2->fp_vertex );
	fpu_reg12 = ( ((tvec *)(edx + eax))->x1 );
	((tscreenpoint *)edi)->sp_x = fpu_reg12;
	fpu_reg12 = ( ((tvec *)(edx + eax))->x2 );
	((tscreenpoint *)edi)->sp_y = fpu_reg12;
	fpu_reg12 = ( ((tvec *)(edx + eax))->x3 );
	((tscreenpoint *)edi)->sp_z = fpu_reg12;
	fpu_reg12 = ( ((tvec *)(es2 + eax))->x1 );
	((tscreenpoint *)edi)->sp_lx = fpu_reg12;
	fpu_reg12 = ( ((tvec *)(es2 + eax))->x2 );
	((tscreenpoint *)edi)->sp_ly = fpu_reg12;
	fpu_reg12 = ( ((tvec *)(es2 + eax))->x3 );
	((tscreenpoint *)edi)->sp_lz = fpu_reg12;


	eb2 = eb2 + ( 1 );
	edi = edi + ( lightpsize );

	eax = ( eb2->fp_vertex );
	fpu_reg11 = ( ((tvec *)(edx + eax))->x1 );
	((tscreenpoint *)edi)->sp_x = fpu_reg11;
	fpu_reg11 = ( ((tvec *)(edx + eax))->x2 );
	((tscreenpoint *)edi)->sp_y = fpu_reg11;
	fpu_reg11 = ( ((tvec *)(edx + eax))->x3 );
	((tscreenpoint *)edi)->sp_z = fpu_reg11;
	fpu_reg11 = ( ((tvec *)(es2 + eax))->x1 );
	((tscreenpoint *)edi)->sp_lx = fpu_reg11;
	fpu_reg11 = ( ((tvec *)(es2 + eax))->x2 );
	((tscreenpoint *)edi)->sp_ly = fpu_reg11;
	fpu_reg11 = ( ((tvec *)(es2 + eax))->x3 );
	((tscreenpoint *)edi)->sp_lz = fpu_reg11;


	eb2 = eb2 + ( 1 );
	edi = edi + ( lightpsize );

	eb3 = stack_var00; //ebx -> lightpoints
//edi -> lightpoints-end
	ldraw(idx, ( - 1 ), eb3, (tscreenpoint *)edi);

	edi = (uint8_t *)stack_var00;
	goto ldrawface_weg;
ldrawface_g3:
	eax = ( eb2[1].fp_vertex );
	fpu_reg11 = ( *((float *)(edx + eax)) ); //.x3
ldrawface_g2:
	eax = ( eb2->fp_vertex );
	fpu_reg12 = ( *((float *)(edx + eax)) ); //.x3
ldrawface_g1:
	edx = (uint8_t *)( esi->m_vtlist ); //edx -> vertices from camera's view
	es2 = (uint8_t *)( esi->m_tlist ); //esi -> vertices from light's view
//edi -> mem for lightpoints
//ldrawface_g: //with gouraud shading
	stack_var00 = (tscreenpoint *)edi;

	eax = ( eb2->fp_vertex );
	fpu_reg13 = ( ((tvec *)(edx + eax))->x1 );
	((tscreenpoint *)edi)->sp_x = fpu_reg13;
	fpu_reg13 = ( ((tvec *)(edx + eax))->x2 );
	((tscreenpoint *)edi)->sp_y = fpu_reg13;
	fpu_reg13 = ( ((tvec *)(edx + eax))->x3 );
	((tscreenpoint *)edi)->sp_z = fpu_reg13;
	fpu_reg13 = ( ((tvec *)(es2 + eax))->x1 );
	((tscreenpoint *)edi)->sp_lx = fpu_reg13;
	fpu_reg13 = ( ((tvec *)(es2 + eax))->x2 );
	((tscreenpoint *)edi)->sp_ly = fpu_reg13;
	fpu_reg13 = ( ((tvec *)(es2 + eax))->x3 );
	((tscreenpoint *)edi)->sp_lz = fpu_reg13;
	((tscreenpoint *)edi)->sp_ll = fpu_reg12;

	eb2 = eb2 + ( 1 );
	edi = edi + ( glightpsize );

	eax = ( eb2->fp_vertex );
	fpu_reg12 = ( ((tvec *)(edx + eax))->x1 );
	((tscreenpoint *)edi)->sp_x = fpu_reg12;
	fpu_reg12 = ( ((tvec *)(edx + eax))->x2 );
	((tscreenpoint *)edi)->sp_y = fpu_reg12;
	fpu_reg12 = ( ((tvec *)(edx + eax))->x3 );
	((tscreenpoint *)edi)->sp_z = fpu_reg12;
	fpu_reg12 = ( ((tvec *)(es2 + eax))->x1 );
	((tscreenpoint *)edi)->sp_lx = fpu_reg12;
	fpu_reg12 = ( ((tvec *)(es2 + eax))->x2 );
	((tscreenpoint *)edi)->sp_ly = fpu_reg12;
	fpu_reg12 = ( ((tvec *)(es2 + eax))->x3 );
	((tscreenpoint *)edi)->sp_lz = fpu_reg12;
	((tscreenpoint *)edi)->sp_ll = fpu_reg11;

	eb2 = eb2 + ( 1 );
	edi = edi + ( glightpsize );

	eax = ( eb2->fp_vertex );
	fpu_reg11 = ( ((tvec *)(edx + eax))->x1 );
	((tscreenpoint *)edi)->sp_x = fpu_reg11;
	fpu_reg11 = ( ((tvec *)(edx + eax))->x2 );
	((tscreenpoint *)edi)->sp_y = fpu_reg11;
	fpu_reg11 = ( ((tvec *)(edx + eax))->x3 );
	((tscreenpoint *)edi)->sp_z = fpu_reg11;
	fpu_reg11 = ( ((tvec *)(es2 + eax))->x1 );
	((tscreenpoint *)edi)->sp_lx = fpu_reg11;
	fpu_reg11 = ( ((tvec *)(es2 + eax))->x2 );
	((tscreenpoint *)edi)->sp_ly = fpu_reg11;
	fpu_reg11 = ( ((tvec *)(es2 + eax))->x3 );
	((tscreenpoint *)edi)->sp_lz = fpu_reg11;
	((tscreenpoint *)edi)->sp_ll = fpu_reg10;

	eb2 = eb2 + ( 1 );
	edi = edi + ( glightpsize );

	eb3 = stack_var00; //ebx -> lightpoints
//edi -> lightpoints-end
	gldraw(idx, eb3, (tscreenpoint *)edi);

	edi = (uint8_t *)stack_var00;
	goto ldrawface_weg;
ldrawface_zline: //render back-faces onto z-line
//   jmp @@weg;!
//volume light turned on?
	if ((light.lv_flags & ( lfVol )) == 0) goto ldrawface_weg;
	stack_var00 = (tscreenpoint *)edi;
	es2 = (uint8_t *)( esi->m_tlist ); //esi -> vertices from light's view
	ecx = ( 2 );
//if (offset f_p) gt 0
	eb2 = ( &(ebx->f_p[0]) ); //ebx -> tfp
//endif
ldrawface_l2:
	eax = ( eb2->fp_vertex );
	fpu_reg10 = ( ((tvec *)(es2 + eax))->x1 );
	((tscreenpoint *)edi)->sp_x = fpu_reg10;
	fpu_reg10 = ( ((tvec *)(es2 + eax))->x2 );
	((tscreenpoint *)edi)->sp_y = fpu_reg10;
	fpu_reg10 = ( ((tvec *)(es2 + eax))->x3 );
	((tscreenpoint *)edi)->sp_z = fpu_reg10;

	eb2 = eb2 + ( 1 );
	edi = edi + ( zlinepsize );
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) >= 0) goto ldrawface_l2;

	eb3 = stack_var00; //ebx -> points
//edi -> points-end

	ed3 = (tscreenpoint *)edi;
	eax = zprojectline(eb3, ed3);

	if (( (int32_t)eax ) != 0) goto ldrawface_w;
	renderline(eb3, (uintptr_t)ed3 - (uintptr_t)eb3);

ldrawface_w:
	edi = (uint8_t *)stack_var00;
	goto ldrawface_weg;
ldrawface_shadow: //face in shadow
//esi -> tmesh
//cast shadows?
	if ((( esi->m_object.o_flags ) & ( mfNocast )) != 0) goto ldrawface_weg;
	edi = edi + ( 3 * shadowpsize ); //keep shadow face

ldrawface_weg:
	_edi = (tscreenpoint *)edi;
}


static void sdrawfaces(tscreenpoint *edi) { //pascal
	uint32_t eax;
	tscreenpoint *ebx, *stack_var00;
//arg     _mapptr ;pointer to pointer
//-> edi -> end of shadow faces buffer

//scene.sc_datapos -> shadow faces buffer
sdrawfaces_l:
	ebx = (tscreenpoint *)( ((uintptr_t)edi) + (- shadowpsize * 3) ); //ebx -> start of points array
//edi -> end of points array

	if (ebx < (tscreenpoint *)scene.sc_datapos) goto sdrawfaces_weg;
	stack_var00 = ebx;
	eax = sprojection(ebx, edi);

	if (( (int32_t)eax ) != 0) goto sdrawfaces_0;
//       call    sxclip
//        jbe     @@sw

	spolygon(ebx, (uintptr_t)edi - (uintptr_t)ebx); //, _mapptr
sdrawfaces_0:
	edi = stack_var00;
	goto sdrawfaces_l;
sdrawfaces_weg:
	return;
}


static void dolights(void) {
	realnum fpu_reg10, fpu_reg11;
	uint32_t eax, edx, ecx;
	tcface *ec2, *ebx;
	tscreenpoint *edi, *eb4;
	float *ed2;
	tcube *eb2;
	void *eb3;
	tface *eb5;
	ttempface *eb6;
	tlight *esi;
	uint8_t *es2;
	ttempcube *es3;
	tmesh *es4;
	tcface *stack_var00;
	tmesh *stack_var01;
	uint32_t vstamp;
	void *datapos;
	tlight *lightptr;
	ttempcube *tc, *vtc, *cubelistend;
	uint32_t z, flags; //alle beleuchteten w„nde vormerken
//1. schatten: w„nde
//   schleife:
//2.   licht: polygone
//3.   schatten: plygone
//4. licht: w„nde

//if testcode gt 0
//local lightcount
// mov lightcount,0
//endif
	vstamp = stamp;

	datapos = scene.sc_datapos;

	lightptr = (tlight *)scene.sc_firstlight;

dolights_l_l: //esi -> tlight
	esi = lightptr;

	if (esi >= (tlight *)scene.sc_lastlight) goto dolights_weg;

	if (( esi->l_viewer.v_object.o_hidden ) != 0) goto dolights_next; //versteckt

	if (( esi->l_viewer.v_object.o_cube ) == 0) goto dolights_next; //in keinem wrfel

//if testcode gt 0
// inc lightcount
//endif
//volume light
//clear rendered z-buffer lines
	if ((( esi->l_viewer.v_object.o_flags ) & ( lfVol )) == 0) goto dolights_Nvol;
	ed2 = (float *)scene.sc_zline;
	ecx = ( 3 * zxres );
	for (; ecx != 0; ecx--, ed2+=1) *ed2 = c_max;
dolights_Nvol:
//make viewer
	doviewer(&(esi->l_viewer), &(light.lv_viewer)); //esi -> tlight
	adjustlight(esi, &(light));

//lens flare
//esi -> tlight
	if ((( esi->l_viewer.v_object.o_flags ) & ( lfFlare )) == 0) goto dolights_Nflare;
	flare();
dolights_Nflare:

//copy map
//esi -> tlight
	if ((( esi->l_viewer.v_object.o_flags ) & ( lfWater )) != 0) goto dolights_notNwater;
	es2 = (uint8_t *)( esi->l_lmap );
	goto dolights_Nwater;
dolights_notNwater:
	es2 = (uint8_t *)( esi->l_lmap );
	eax = frame;
	eax = eax << ( 16 - 6 );
	eax = eax & ( 0x70000 );
	es2 = es2 + eax;
dolights_Nwater:
	memcpy(scene.sc_lmap, es2, 65536); //copy lightmap

	//esi = ( &(light) ); //cubetree bzgl. licht
	cubetree(( 26 ), &(light.lv_viewer), (ttempcube *)datapos); //16

	tc = light.lv_viewer.v_cubelist; //esi -> licht-tempcube
	cubelistend = (ttempcube *)scene.sc_datapos;

dolights_c_l: //wrfel durchgehen (vorne nach hinten)
	es3 = tc;

	if (es3 >= cubelistend) goto dolights_vol; //n„chstes licht

	eb2 = ( es3->tc_cubeptr ); //ebx -> cube
	vtc = ( eb2->c_vtc ); //vtc : viewer-tempcube

	ecx = ( eb2->c_faces ); //w„nde durchgehen
	if (ecx == 0) goto dolights_o;
	z = ecx;
	ebx = (tcface *)( eb2->c_facedata ); //ebx -> faces
dolights_s_l0:
	fpu_reg10 = light.lv_viewer.v_p.x1;
	fpu_reg10 = fpu_reg10 - ( ebx->cf_p.x1 );
	fpu_reg10 = fpu_reg10 * ( ebx->cf_n.x1 );
	fpu_reg11 = light.lv_viewer.v_p.x2;
	fpu_reg11 = fpu_reg11 - ( ebx->cf_p.x2 );
	fpu_reg11 = fpu_reg11 * ( ebx->cf_n.x2 );
	fpu_reg10 = fpu_reg10 + fpu_reg11;
	fpu_reg11 = light.lv_viewer.v_p.x3;
	fpu_reg11 = fpu_reg11 - ( ebx->cf_p.x3 );
	fpu_reg11 = fpu_reg11 * ( ebx->cf_n.x3 );
	fpu_reg10 = fpu_reg10 + fpu_reg11;

	flags = flags << ( 1 );




	if (fpu_reg10 <= 0.0) goto dolights_nots0;

	goto dolights_s0;
dolights_nots0:

//1. wand wirft schatten
	flags = flags + 1;
	stack_var00 = ebx;
	eax = ( ebx->cf_points );
	ebx = ebx + ( 1 );
//ebx -> source points of type tcpoint
	//eax = eax + ebx; //eax -> sp_end / next face

	edi = (tscreenpoint *)scene.sc_datapos;
	eb3 = (void *)ebx;
	eax = scubezclip((tcpoint *)(eax + (uintptr_t)ebx), eb3, es3, edi); //esi -> light-ttempcube
	eb4 = (tscreenpoint *)eb3;

	if (( (int32_t)eax ) != 0) goto dolights_sw;
	spolygon(eb4, (uintptr_t)edi - (uintptr_t)eb4);

dolights_sw:
	ebx = stack_var00;
dolights_s0:

	ebx = (tcface *)( ((uintptr_t)ebx) + ebx->cf_points );
	ebx = ebx + ( 1 );
	z = ( (int32_t)z ) - 1;
	if (( (int32_t)z ) != 0) goto dolights_s_l0;
dolights_o:

//---

	es4 = ( tc->tc_meshlist );
dolights_h_l:

	if (es4 == 0) goto dolights_h0;
	stack_var01 = es4; //esi -> top level mesh

	if ((( es4->m_object.o_flags ) & ( mfWater )) != 0) goto dolights_hw;

	es4 = xformhierarchy(es4, &(light.lv_viewer)); //transform one hierarchy


	if ((( es4->m_object.o_flags ) & ( mfSort )) != 0) goto dolights_sort;
dolights_m_l: //unsorted
	ecx = ( es4->m_faces );
	if (ecx == 0) goto dolights_mw;
	z = ecx;
	edi = (tscreenpoint *)scene.sc_datapos; //edi -> shadow faces buffer
	eb5 = ( es4->m_facelist );
dolights_f_l0: //ebx -> tface
	//esi -> tmesh
//2. polygon wird beleuchtet
	ldrawface(vstamp, eb5, es4, edi);

	eb5 = eb5 + ( 1 );
	z = ( (int32_t)z ) - 1;
	if (( (int32_t)z ) != 0) goto dolights_f_l0;
dolights_mw:
	eax = getnextmesh(es4);

	if (( (int32_t)eax ) != 0) goto dolights_m_l;
//3. polygon wirft schatten
	sdrawfaces(edi); //edi -> shadow faces buffer

	goto dolights_hw;
dolights_sort: //sorted                 ;edi -> tviewer

	z = ( 15 );
dolights_a_l: //array loop
	ecx = z;
	ecx = ecx ^ ( 15 ); //reverse order
	eb6 = ( light.lv_viewer.v_tempfaces[ecx] );
	edi = (tscreenpoint *)scene.sc_datapos; //edi -> shadow faces buffer
dolights_f_l1:
//ebx -> ttempface
	if (eb6 == 0) goto dolights_fw;
	es4 = ( eb6->tf_meshptr ); //esi -> tmesh
//2. polygon wird beleuchtet
	ldrawface(vstamp, eb6->tf_faceptr, es4, edi);

	eb6 = ( eb6->tf_next );
	goto dolights_f_l1;
dolights_fw: //3. polygon wirft schatten
	sdrawfaces(edi); //edi -> shadow faces buffer

	z = ( (int32_t)z ) - 1;
	if (( (int32_t)z ) >= 0) goto dolights_a_l;

dolights_hw:
	es4 = stack_var01;
	es4 = ( es4->m_nexttemp );
	goto dolights_h_l;
dolights_h0:
#if 0
//      mov esi,tc
//      mov esi,[esi].tc_cubeptr
//      mov esi,[esi].c_objlist
//@@fll:
//      or esi,esi
//      jz @@flw
//      test [esi].o_flags,ofCamera
//      jnz @@fl
//      mov esi,[esi].o_nextincube
//      jmp @@fll
//@@fl:
//      call flare
//@@flw:
#endif
//---            ;w„nde erneut durchgehen
	es3 = tc;
	eb2 = ( es3->tc_cubeptr ); //ebx -> cube
	eax = vstamp;

	if (eax != ( eb2->c_vstamp )) goto dolights_nc; //wrfel nicht sichtbar
	ecx = ( eb2->c_faces );
	if (ecx == 0) goto dolights_nc;
	z = ecx;
	ecx = /*(ecx & 0xffffff00) |*/ (uint8_t)(- ( (int8_t)ecx ));
	ecx = /*(ecx & 0xffffff00) |*/ (uint8_t)(( (uint8_t)ecx ) + ( 32 ));
	flags = flags << ( (uint8_t)ecx );
	ebx = (tcface *)( eb2->c_facedata ); //ebx -> tcface (cube-face)
dolights_s_l1: //side loop
//ebx -> tcface

	ecx = ( ebx->cf_points );
	ebx = ebx + ( 1 ); //ebx -> tcpoint (cube-point)
	ec2 = (tcface *)(ecx + (uintptr_t)ebx);
	stack_var00 = ec2; //ecx -> next face


	if ((flags & ( 0x80000000 )) == 0) goto dolights_notns;
	flags = flags << ( 1 );
	goto dolights_ns;
dolights_notns:
	flags = flags << ( 1 );
//4. wand wird beleuchtet

	edi = (tscreenpoint *)scene.sc_datapos;

dolights_p_l: //point loop
	edx = ( ((tcpoint *)ebx)->cp_p );

	es2 = (uint8_t *)tc; //esi -> tempcube
	((tscreenpoint *)edi)->sp_lx = ((ttempcube *)(es2 + edx))->tc_proj[0].pr_x;
	((tscreenpoint *)edi)->sp_ly = ((ttempcube *)(es2 + edx))->tc_proj[0].pr_y;
	((tscreenpoint *)edi)->sp_lz = ((ttempcube *)(es2 + edx))->tc_proj[0].pr_z;

	es2 = (uint8_t *)vtc; //esi -> camera-tempcube
	((tscreenpoint *)edi)->sp_x = ((ttempcube *)(es2 + edx))->tc_proj[0].pr_x;
	((tscreenpoint *)edi)->sp_y = ((ttempcube *)(es2 + edx))->tc_proj[0].pr_y;
	((tscreenpoint *)edi)->sp_z = ((ttempcube *)(es2 + edx))->tc_proj[0].pr_z;

	edi = (tscreenpoint *)( ((uintptr_t)edi) + lightpsize );
	ebx = (tcface *)( ((uintptr_t)ebx) + sizeof(tcpoint) ); //next point

	if (ebx < ec2) goto dolights_p_l;
//esi -> camera-tempcube

	ldraw(( ((ttempcube *)es2)->tc_idx ), ( ((ttempcube *)es2)->tc_recursion ), (tscreenpoint *)scene.sc_datapos, edi);

dolights_ns: //next side
	ebx = stack_var00; //ebx -> tcface

	z = ( (int32_t)z ) - 1;
	if (( (int32_t)z ) != 0) goto dolights_s_l1;

dolights_nc: //next cube
	tc = tc + ( 1 );
	goto dolights_c_l;
dolights_vol:
	esi = lightptr; //esi -> tlight
	eax = ( esi->l_viewer.v_object.o_flags );
//[esi].o_flags,lfVol
	if ((eax & ( lfVol )) == 0) goto dolights_next;
	vollight(vstamp, (tscreenpoint *)datapos, eax);

dolights_next: //next light
	lightptr = lightptr + ( 1 );
	goto dolights_l_l;
dolights_weg:
//if testcode gt 0
// mov edi,scene.sc_buffer
// mov eax,fvalues-1
// shl eax,16
// mov ecx,lightcount
// jecxz @@tw
//@@tl0:
// mov [edi],eax
// mov dword ptr [edi+4],0
// add edi,8
// dec ecx
// jnz @@tl0
//@@tw:
//endif
	return;
}




//---INIT-----------------------

static void initrgb16(tvesa *esi, tpaltab16 *edi) {
	uint32_t eax, edx, ecx, ebx, ebp;
//-> esi -> tvesa
//-> edi -> tpaltab16

	ebx = ( ( fvalues + xtrafvalues ) - 1 ); //fog value (0 .. fvalues-1)
initrgb16_l0:
	ebp = ( 255 ); //color value (0 .. 255)
initrgb16_l1:
	eax = ( 255 );
	eax = eax - ebp;
	eax = ( (int32_t)eax ) * ( (int32_t)ebx );
	eax = eax >> ( ld_fvalues );
	eax = eax + ebp;

	if (eax <= ( 255 )) goto initrgb16_clip;
	eax = ( 255 );
initrgb16_clip:
//r
	edx = eax;
	ecx = /*(ecx & 0xffffff00) |*/ (uint8_t)(( esi->vesa_redbits ));
	edx = edx << ( (uint8_t)ecx );
	edx = edx >> ( 8 );
	ecx = (ecx & 0xffffff00) | (uint8_t)(( esi->vesa_redpos ));
	edx = edx << ( (uint8_t)ecx );
	ecx = ebp;
	ecx = set_high_byte(ecx, ( (uint8_t)ebx ));
	edi->p16_r[ecx] = (uint16_t) (( (uint16_t)edx ));
//g
	edx = eax;
	ecx = (ecx & 0xffffff00) | (uint8_t)(( esi->vesa_greenbits ));
	edx = edx << ( (uint8_t)ecx );
	edx = edx >> ( 8 );
	ecx = (ecx & 0xffffff00) | (uint8_t)(( esi->vesa_greenpos ));
	edx = edx << ( (uint8_t)ecx );
	ecx = ebp;
	ecx = set_high_byte(ecx, ( (uint8_t)ebx ));
	edi->p16_g[ecx] = (uint16_t) (( (uint16_t)edx ));
//b
	edx = eax;
	ecx = (ecx & 0xffffff00) | (uint8_t)(( esi->vesa_bluebits ));
	edx = edx << ( (uint8_t)ecx );
	edx = edx >> ( 8 );
	ecx = (ecx & 0xffffff00) | (uint8_t)(( esi->vesa_bluepos ));
	edx = edx << ( (uint8_t)ecx );
	ecx = ebp;
	ecx = set_high_byte(ecx, ( (uint8_t)ebx ));
	edi->p16_b[ecx] = (uint16_t) (( (uint16_t)edx ));

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) >= 0) goto initrgb16_l1;

	ebx = ( (int32_t)ebx ) - 1;
	if (( (int32_t)ebx ) >= 0) goto initrgb16_l0;

//initrgb16_weg:
	return;
}


static void initrgb24_32(tvesa *esi, tpaltab32 *edi) {
	uint32_t eax, edx, ecx, ebx, ebp;
//-> esi -> tvesa
//-> edi -> tpaltab32

	ebx = ( ( fvalues + xtrafvalues ) - 1 ); //fog value (0 - fvalues-1)
initrgb24_32_l0:
	ebp = ( 255 ); //color value (0 - 255)
initrgb24_32_l1:
	eax = ( 255 );
	eax = eax - ebp;
	eax = ( (int32_t)eax ) * ( (int32_t)ebx );
	eax = eax >> ( ld_fvalues );
	eax = eax + ebp;

	if (eax <= ( 255 )) goto initrgb24_32_clip;
	eax = ( 255 );
initrgb24_32_clip:
//r
	edx = eax;
	ecx = /*(ecx & 0xffffff00) |*/ (uint8_t)(( esi->vesa_redbits ));
	edx = edx << ( (uint8_t)ecx );
	edx = edx >> ( 8 );
	ecx = (ecx & 0xffffff00) | (uint8_t)(( esi->vesa_redpos ));
	edx = edx << ( (uint8_t)ecx );
	ecx = ebp;
	ecx = set_high_byte(ecx, ( (uint8_t)ebx ));
	edi->p32_r[ecx] = edx;
//g
	edx = eax;
	ecx = (ecx & 0xffffff00) | (uint8_t)(( esi->vesa_greenbits ));
	edx = edx << ( (uint8_t)ecx );
	edx = edx >> ( 8 );
	ecx = (ecx & 0xffffff00) | (uint8_t)(( esi->vesa_greenpos ));
	edx = edx << ( (uint8_t)ecx );
	ecx = ebp;
	ecx = set_high_byte(ecx, ( (uint8_t)ebx ));
	edi->p32_g[ecx] = edx;
//b
	edx = eax;
	ecx = (ecx & 0xffffff00) | (uint8_t)(( esi->vesa_bluebits ));
	edx = edx << ( (uint8_t)ecx );
	edx = edx >> ( 8 );
	ecx = (ecx & 0xffffff00) | (uint8_t)(( esi->vesa_bluepos ));
	edx = edx << ( (uint8_t)ecx );
	ecx = ebp;
	ecx = set_high_byte(ecx, ( (uint8_t)ebx ));
	edi->p32_b[ecx] = edx;

	ebp = ( (int32_t)ebp ) - 1;
	if (( (int32_t)ebp ) >= 0) goto initrgb24_32_l1;

	ebx = ( (int32_t)ebx ) - 1;
	if (( (int32_t)ebx ) >= 0) goto initrgb24_32_l0;

//initrgb24_32_weg:
	return;
}


static void copybuffer16(void) {
	uint32_t eax, edx, ecx, ebp;
	uint32_t stack_var00;
	tpaltab16 *ebx;
	uint8_t *esi, *edi, *stack_var01;
	esi = (uint8_t *)scene.sc_buffer;
	edi = linbuf;

	ebx = (tpaltab16 *)scene.sc_paltab;
	edx = 0;
	ecx = yres;
copybuffer16_y_l:
	stack_var00 = ecx;
	stack_var01 = edi;

	ecx = xres;
	ecx = ecx >> ( 1 );
copybuffer16_x_l:
	eax = (uint32_t)( ((uint16_t *)(esi))[2] ); //eax = 00lt
	edx = set_high_byte(edx, ( *((uint8_t *)(esi + (2 + 4))) )); //dh = f
	eax = ( ebx->p16_pal.p_pal[eax] ); //eax = 0bgr

	edx = (edx & 0xffffff00) | (uint8_t)(( (uint8_t)eax )); //rot
	ebp = /*(ebp & 0xffff0000) |*/ (uint16_t)(( ebx->p16_r[edx] ));

	edx = (edx & 0xffffff00) | (uint8_t)(( (uint8_t)(eax >> 8) )); //grn
	eax = eax >> ( 8 );
	ebp = (ebp & 0xffff0000) | (uint16_t)(( (uint16_t)ebp ) | ( ebx->p16_g[edx] ));

	edx = (edx & 0xffffff00) | (uint8_t)(( (uint8_t)(eax >> 8) )); //blau
	ebp = (ebp & 0xffff0000) | (uint16_t)(( (uint16_t)ebp ) | ( ebx->p16_b[edx] ));

	ebp = ebp << ( 16 );

	eax = (uint32_t)( *((uint16_t *)(esi)) ); //eax = 00lt
	edx = set_high_byte(edx, ( ((uint8_t *)(esi))[2] )); //dh = f
	eax = ( ebx->p16_pal.p_pal[eax] ); //eax = 0bgr

	edx = (edx & 0xffffff00) | (uint8_t)(( (uint8_t)eax )); //rot
	ebp = (ebp & 0xffff0000) | (uint16_t)(( ebx->p16_r[edx] ));

	edx = (edx & 0xffffff00) | (uint8_t)(( (uint8_t)(eax >> 8) )); //grn
	eax = eax >> ( 8 );
	ebp = (ebp & 0xffff0000) | (uint16_t)(( (uint16_t)ebp ) | ( ebx->p16_g[edx] ));

	edx = (edx & 0xffffff00) | (uint8_t)(( (uint8_t)(eax >> 8) )); //blau
	ebp = (ebp & 0xffff0000) | (uint16_t)(( (uint16_t)ebp ) | ( ebx->p16_b[edx] ));

	esi = esi + ( 8 );
	*((uint32_t *)(edi)) = ebp;
	edi = edi + ( 4 );
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto copybuffer16_x_l;

	edi = stack_var01;
	ecx = stack_var00;
	edi = edi + xbytes;
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto copybuffer16_y_l;

	return;
}


static void copybuffer24(void) {
	uint32_t eax, edx, ecx, ebp;
	uint32_t stack_var00, stack_var02;
	tpaltab32 *ebx;
	uint8_t *esi, *edi, *stack_var01;
	esi = (uint8_t *)scene.sc_buffer;
	edi = linbuf;

	ebx = (tpaltab32 *)scene.sc_paltab;
	edx = 0;

	ecx = yres;
copybuffer24_y_l:
	stack_var00 = ecx;
	stack_var01 = edi;

	ecx = xres;
	ecx = ecx >> ( 2 );
	stack_var02 = ecx; //x-counter on stack
copybuffer24_x_l:
//1.
	eax = (uint32_t)( *((uint16_t *)(esi)) ); //eax = 00lt
	edx = set_high_byte(edx, ( ((uint8_t *)(esi))[2] )); //dh = f
	eax = ( ebx->p32_pal.p_pal[eax] ); //eax = 0gbr

	edx = (edx & 0xffffff00) | (uint8_t)(( (uint8_t)eax )); //rot
	ebp = ( ebx->p32_r[edx] );

	edx = (edx & 0xffffff00) | (uint8_t)(( (uint8_t)(eax >> 8) )); //grn
	esi = esi + ( 4 );
	eax = eax >> ( 8 );
	ebp = ebp | ( ebx->p32_g[edx] );

	edx = (edx & 0xffffff00) | (uint8_t)(( (uint8_t)(eax >> 8) )); //blau
	ebp = ebp | ( ebx->p32_b[edx] );
//-
//2.
	eax = (uint32_t)( *((uint16_t *)(esi)) ); //eax = 00lt
	edx = set_high_byte(edx, ( ((uint8_t *)(esi))[2] )); //dh = f
	eax = ( ebx->p32_pal.p_pal[eax] ); //eax = 0gbr

	edx = (edx & 0xffffff00) | (uint8_t)(( (uint8_t)eax )); //rot
	ebp = ebp << ( 8 );
	ecx = ( ebx->p32_r[edx] );

	edx = (edx & 0xffffff00) | (uint8_t)(( (uint8_t)(eax >> 8) )); //grn
	esi = esi + ( 4 );
	eax = eax >> ( 8 );
	ecx = ecx | ( ebx->p32_g[edx] );

	edx = (edx & 0xffffff00) | (uint8_t)(( (uint8_t)(eax >> 8) )); //blau
	ecx = ecx | ( ebx->p32_b[edx] );
//--
//3.
	eax = (uint32_t)( *((uint16_t *)(esi)) ); //eax = 00lt
	edx = set_high_byte(edx, ( ((uint8_t *)(esi))[2] )); //dh = f
	ebp = (ebp >> ( 8 )) | (ecx << (32 - ( 8 )));
	eax = ( ebx->p32_pal.p_pal[eax] ); //eax = 0gbr
	*((uint32_t *)(edi)) = ebp;
	edx = (edx & 0xffffff00) | (uint8_t)(( (uint8_t)eax )); //rot
	edi = edi + ( 4 );

	ebp = ( ebx->p32_r[edx] );

	edx = (edx & 0xffffff00) | (uint8_t)(( (uint8_t)(eax >> 8) )); //grn
	esi = esi + ( 4 );
	eax = eax >> ( 8 );
	ebp = ebp | ( ebx->p32_g[edx] );

	edx = (edx & 0xffffff00) | (uint8_t)(( (uint8_t)(eax >> 8) )); //blau
	ecx = ecx << ( 8 );
	ebp = ebp | ( ebx->p32_b[edx] );
//--
//4.
	eax = (uint32_t)( *((uint16_t *)(esi)) ); //eax = 00lt
	edx = set_high_byte(edx, ( ((uint8_t *)(esi))[2] )); //dh = f
	ecx = (ecx >> ( 16 )) | (ebp << (32 - ( 16 )));
	eax = ( ebx->p32_pal.p_pal[eax] ); //eax = 0gbr
	*((uint32_t *)(edi)) = ecx;
	edx = (edx & 0xffffff00) | (uint8_t)(( (uint8_t)eax )); //rot
	edi = edi + ( 4 );

	ecx = ( ebx->p32_r[edx] );

	edx = (edx & 0xffffff00) | (uint8_t)(( (uint8_t)(eax >> 8) )); //grn
	ebp = ebp << ( 8 );
	eax = eax >> ( 8 );
	ecx = ecx | ( ebx->p32_g[edx] );

	edx = (edx & 0xffffff00) | (uint8_t)(( (uint8_t)(eax >> 8) )); //blau
	esi = esi + ( 4 );
	ecx = ecx | ( ebx->p32_b[edx] );


	ecx = (ecx << ( 8 )) | (ebp >> (32 - ( 8 )));
	*((uint32_t *)(edi)) = ecx;
	edi = edi + ( 4 );


	stack_var02 = ((int32_t)stack_var02) - 1;
	if (((int32_t)stack_var02) != 0) goto copybuffer24_x_l;
	ecx = stack_var02; //remove x-counter

	edi = stack_var01;
	ecx = stack_var00;
	edi = edi + xbytes;
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto copybuffer24_y_l;

	return;
}


static void copybuffer32(void) {
	uint32_t eax, edx, ecx, ebp;
	uint32_t stack_var00;
	tpaltab32 *ebx;
	uint8_t *esi, *edi, *stack_var01;
	esi = (uint8_t *)scene.sc_buffer;
	edi = linbuf;

	ebx = (tpaltab32 *)scene.sc_paltab;
	edx = 0;

	ecx = yres;
copybuffer32_y_l:
	stack_var00 = ecx;
	stack_var01 = edi;
	ecx = xres;
copybuffer32_x_l:
	eax = (uint32_t)( *((uint16_t *)(esi)) ); //eax = 00lt
	edx = set_high_byte(edx, ( ((uint8_t *)(esi))[2] )); //dh = f
	eax = ( ebx->p32_pal.p_pal[eax] ); //eax = 0gbr

	edx = (edx & 0xffffff00) | (uint8_t)(( (uint8_t)eax )); //rot
	ebp = ( ebx->p32_r[edx] );

	edx = (edx & 0xffffff00) | (uint8_t)(( (uint8_t)(eax >> 8) )); //grn
	eax = eax >> ( 8 );
	ebp = ebp | ( ebx->p32_g[edx] );

	edx = (edx & 0xffffff00) | (uint8_t)(( (uint8_t)(eax >> 8) )); //blau
	ebp = ebp | ( ebx->p32_b[edx] );

	esi = esi + ( 4 );
	*((uint32_t *)(edi)) = ebp;
	edi = edi + ( 4 );
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto copybuffer32_x_l;

	edi = stack_var01;
	ecx = stack_var00;
	edi = edi + xbytes;
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto copybuffer32_y_l;

	return;
}



static void initpaltab(tvesa *esi) {
	uint32_t eax, edx, ecx, ebp, es2;
	void *ea2;
	uint8_t *ebx;
	tpaltab *edi;
//-> esi -> tvesa

//calc palette in all light values
	edi = (tpaltab *)scene.sc_paltab; //edi -> tpaltab

	ecx = ( lvalues + xtralvalues - 1 );
initpaltab_l0:
	ebx = (uint8_t *)scene.sc_pal;
	ebp = 0;
initpaltab_l1:
	edx = 0;
	es2 = ( 2 );
initpaltab_l2:
	eax = (uint32_t)( *((uint8_t *)(ebx + es2)) ); //g,b,r
	eax = ( (int32_t)eax ) * ( (int32_t)ecx );
	eax = eax >> ( ld_lvalues );

	if (eax < ( 255 )) goto initpaltab_clip;
	eax = (eax & 0xffffff00) | (uint8_t)(( 255 ));
initpaltab_clip:
	edx = edx << ( 8 );
	edx = (edx & 0xffffff00) | (uint8_t)(( (uint8_t)eax ));
	es2 = ( (int32_t)es2 ) - 1;
	if (( (int32_t)es2 ) >= 0) goto initpaltab_l2;
//edx = 0gbr
	eax = ebp;
	eax = set_high_byte(eax, ( (uint8_t)ecx ));
	edi->p_pal[eax] = edx;

	ebx = ebx + ( 3 );
	ebp = ebp + 1;

	if (ebp < ( 256 )) goto initpaltab_l1;

	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) >= 0) goto initpaltab_l0;

//esi -> tvesa
//edi -> paltab
//bpp,16
	if (( esi->vesa_pbytes ) > ( 2 )) goto initpaltab_24_32;
	initrgb16(esi, (tpaltab16 *)edi);
	ea2 = ( (void *)&copybuffer16 );
	goto initpaltab_w;
initpaltab_24_32:
	initrgb24_32(esi, (tpaltab32 *)edi);
	ea2 = ( (void *)&copybuffer32 );
//bpp,24
	if (( esi->vesa_pbytes ) > ( 3 )) goto initpaltab_32;
	ea2 = ( (void *)&copybuffer24 );
initpaltab_32:

initpaltab_w:
	copybuffer = (void (*)(void))ea2;

//initpaltab_weg:
	return;
}


static void init1(void) {
	realnum fpu_reg10, fpu_reg11;
	uint32_t eax, edx, ecx;
	float *edi;

//xmid & ymid
	fpu_reg10 = ( (int32_t)xres );
	fpu_reg10 = fpu_reg10 * c_0_5;
	xmid = fpu_reg10;
	fpu_reg10 = ( (int32_t)yres );
	fpu_reg10 = fpu_reg10 * c_0_5;
	ymid = fpu_reg10;


//divtab
	eax = xres;
	edx = yres;

	if (eax > edx) goto init1_0;
	eax = edx;
init1_0: //eax = max(xres,yres) +1
	eax = eax + 1;

	edi = (float *)scene.sc_divtab;
	ecx = ( 1 );
	fpu_reg10 = 0.0;
	*edi = fpu_reg10;
init1_l:
	fpu_reg11 = 1.0;
	fpu_reg10 = fpu_reg10 + fpu_reg11;
	fpu_reg11 = 1.0;
	fpu_reg11 = fpu_reg11 / fpu_reg10;
	edi[ecx] = fpu_reg11;
	ecx = ecx + 1;
//max(xres,yres) +1
	if (ecx <= eax) goto init1_l;


//viewer
	viewer.v_light = ( 0 );
	light.lv_viewer.v_light = ( 1 );

	return;
}



static void makespot(uint8_t *edi, realnum _fpu_reg9) {
	realnum fpu_reg9 = _fpu_reg9, fpu_reg10, fpu_reg11;
	uint32_t eax, ebx;
//-> edi -> lmap
//st(0) = sin-scale

	uint32_t x, y, h;

	y = ( 255 );
makespot_y_l:
	fpu_reg10 = ( (int32_t)y );
	fpu_reg10 = fpu_reg10 * fpu_reg9; //pi_256
	fpu_reg10 = sin(fpu_reg10);
	fpu_reg10 = fpu_reg10 * c_lmap1;

	x = ( 255 );
makespot_x_l:
	fpu_reg11 = ( (int32_t)x );
	fpu_reg11 = fpu_reg11 * fpu_reg9; //pi_256
	fpu_reg11 = sin(fpu_reg11);
	fpu_reg11 = fpu_reg11 * fpu_reg10;
	fpu_reg11 = fabs(fpu_reg11);
	h = (int32_t)ceil(fpu_reg11);

	ebx = x;
	ebx = set_high_byte(ebx, ( (uint8_t)y ));

	eax = h;
//        cmp     eax,lvalues-1
//        jbe     @@0
//        mov     eax,lvalues-1
//makespot_0:
//      mov al,lvalues-1
	*((uint8_t *)(edi + ebx)) = (uint8_t) (( (uint8_t)eax ));

	x = ( (int32_t)x ) - 1;
	if (( (int32_t)x ) >= 0) goto makespot_x_l;


	y = ( (int32_t)y ) - 1;
	if (( (int32_t)y ) >= 0) goto makespot_y_l;



//  xor eax,eax
//  mov ecx,8
//  mov ebx,256*124
//@@t_l:
//  mov [edi+ebx+126],eax
//  mov [edi+ebx+130],eax
//  add ebx,256
//  dec ecx
//  jnz @@t_l
	return;
}


static void makewater(uint8_t *edi) {
	realnum fpu_reg10, fpu_reg11, fpu_reg12, fpu_reg13;
	uint32_t eax, edx, ecx, es2;
	twater *ebx;
	const float *esi;
	uint32_t x, y, p, h;


	p = ( 7 );
makewater_p_l:
	fpu_reg10 = ( (int32_t)p );
	fpu_reg10 = fpu_reg10 * pi_8;


//pre-calculations
	ebx = (twater *)scene.sc_tempdata;
	esi = ( &(water_coeff[0]) );

	ecx = ( 511 );
	h = ecx;
makewater_l0:
	fpu_reg11 = ( (int32_t)h );

	if (ecx > ( 255 )) goto makewater_0;
//x dependent water
	fpu_reg12 = fpu_reg11;
	fpu_reg12 = fpu_reg12 * ( *esi );
	fpu_reg12 = fpu_reg12 + fpu_reg10;
	fpu_reg12 = sin(fpu_reg12);

	fpu_reg13 = fpu_reg11;
	fpu_reg13 = fpu_reg13 * ( esi[1] );
	fpu_reg13 = fpu_reg13 - fpu_reg10;
	fpu_reg13 = sin(fpu_reg13);

	fpu_reg12 = fpu_reg12 + fpu_reg13;
	ebx->w_x[ecx] = fpu_reg12;

//y dependent water
	fpu_reg12 = fpu_reg11;
	fpu_reg12 = fpu_reg12 * ( esi[2] );
	fpu_reg12 = fpu_reg12 + fpu_reg10;
	fpu_reg12 = sin(fpu_reg12);

	fpu_reg13 = fpu_reg11;
	fpu_reg13 = fpu_reg13 * ( esi[3] );
	fpu_reg13 = fpu_reg13 - fpu_reg10;
	fpu_reg13 = sin(fpu_reg13);

	fpu_reg12 = fpu_reg12 + fpu_reg13;
	ebx->w_y[ecx] = fpu_reg12;


//x and y dependent scale, scale = sin(fx)*sin(fy)*4.0
	fpu_reg12 = fpu_reg11;
	fpu_reg12 = fpu_reg12 * pi_256; //fx = pi*x/256.0, fy = pi*y/256.0
	fpu_reg12 = sin(fpu_reg12); //sin(fx)
	fpu_reg12 = fpu_reg12 * c_2;
	ebx->w_scale[ecx] = fpu_reg12;

makewater_0:

//x+y dependent water
	fpu_reg12 = fpu_reg11;
	fpu_reg12 = fpu_reg12 * ( esi[4] );
	fpu_reg12 = fpu_reg12 + fpu_reg10;
	fpu_reg12 = sin(fpu_reg12);

	fpu_reg13 = fpu_reg11;
	fpu_reg13 = fpu_reg13 * ( esi[5] );
	fpu_reg13 = fpu_reg13 - fpu_reg10;
	fpu_reg13 = sin(fpu_reg13);
	fpu_reg12 = fpu_reg12 + fpu_reg13;
	ebx->w_xpy[ecx] = fpu_reg12;

//x-y dependent water
	fpu_reg12 = fpu_reg11;
	fpu_reg12 = fpu_reg12 * ( esi[6] );
	fpu_reg12 = fpu_reg12 + fpu_reg10;
	fpu_reg12 = sin(fpu_reg12);

	fpu_reg13 = fpu_reg11;
	fpu_reg13 = fpu_reg13 * ( esi[7] );
	fpu_reg13 = fpu_reg13 - fpu_reg10;
	fpu_reg13 = sin(fpu_reg13);
	fpu_reg12 = fpu_reg12 + fpu_reg13;
	ebx->w_xmy[ecx] = fpu_reg12;



	h = h - 1;
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) >= 0) goto makewater_l0;



	y = ( 255 );
makewater_y_l:
	x = ( 255 );
makewater_x_l:


	eax = x;
	edx = y;

	fpu_reg11 = ( ebx->w_x[eax] );
	fpu_reg11 = fpu_reg11 + ( ebx->w_y[edx] );
	es2 = ( eax + edx );
	fpu_reg11 = fpu_reg11 + ( ebx->w_xpy[es2] );
	es2 = ( eax + (255) );
	es2 = es2 - edx;
	fpu_reg11 = fpu_reg11 + ( ebx->w_xmy[es2] );
	fpu_reg11 = fabs(fpu_reg11);
//fx = pi*x/256.0 ,fy = pi*y/256.0
	fpu_reg12 = ( ebx->w_scale[eax] ); //scale = sin(fx)*sin(fy)*4.0;
	fpu_reg12 = fpu_reg12 * ( ebx->w_scale[edx] );

	fpu_reg13 = 1.0; //if (scale > 1.0) scale = 1.0;



	if (fpu_reg13 > fpu_reg12) goto makewater_1;

	fpu_reg12 = 1.0;
makewater_1:
	fpu_reg11 = fpu_reg11 * fpu_reg12;
	fpu_reg11 = fpu_reg11 * c_wscale;

	h = (int32_t)ceil(fpu_reg11);

	edx = p;
	edx = edx << ( 16 );
	edx = (edx & 0xffffff00) | (uint8_t)(( (uint8_t)x ));
	edx = set_high_byte(edx, ( (uint8_t)y ));

	eax = h;

	if (( (uint8_t)eax ) <= ( lvalues - 1 )) goto makewater_clip;
	eax = (eax & 0xffffff00) | (uint8_t)(( lvalues - 1 ));
makewater_clip:
	*((uint8_t *)(edi + edx)) = (uint8_t) (( (uint8_t)eax ));

	x = ( (int32_t)x ) - 1;
	if (( (int32_t)x ) >= 0) goto makewater_x_l;

	y = ( (int32_t)y ) - 1;
	if (( (int32_t)y ) >= 0) goto makewater_y_l;


	p = ( (int32_t)p ) - 1;
	if (( (int32_t)p ) >= 0) goto makewater_p_l;

	return;
}


static void makefogmap(void) {
	realnum fpu_reg10, fpu_reg11;
	uint32_t eax, ebx;
	uint8_t *edi;
	uint32_t x, h, val;


//zxw & zyw
	fpu_reg10 = z_width;
	fpu_reg11 = xmid;
	fpu_reg11 = fpu_reg11 * fpu_reg10;
	zxw = fpu_reg11;
	fpu_reg11 = ymid;
	fpu_reg11 = fpu_reg11 * c_1_33;
	fpu_reg10 = fpu_reg10 * fpu_reg11;
	zyw = fpu_reg10;

//fogmaps
	x = ( fogmapsize - 1 );
	edi = (uint8_t *)scene.sc_fogmap;
makefogmap_x_l:

	fpu_reg10 = ( (int32_t)x );
	fpu_reg10 = fpu_reg10 * c_fogmap1;
	fpu_reg10 = sin(fpu_reg10);
	fpu_reg10 = fpu_reg10 + c_fogadd;
	fpu_reg10 = fpu_reg10 * c_fogmul;

	h = ( fogmapsteps ); //intensity steps
makefogmap_h_l:
	ebx = h;
	ebx = ebx - 1;
	ebx = ( (int32_t)ebx ) * ( fogmapsize );
	ebx = ebx + x;

	fpu_reg11 = ( (int32_t)h );
	fpu_reg11 = fpu_reg11 * fpu_reg10;
	val = (int32_t)ceil(fpu_reg11);
	eax = val;
//    mov al,63
	*((uint8_t *)(edi + ebx)) = (uint8_t) (( (uint8_t)eax ));

	h = ( (int32_t)h ) - 1;
	if (( (int32_t)h ) != 0) goto makefogmap_h_l;


	x = ( (int32_t)x ) - 1;
	if (( (int32_t)x ) >= 0) goto makefogmap_x_l;

	return;
}


static void makefogwater(void) { //animation table for fogmaps
	realnum fpu_reg10, fpu_reg11, fpu_reg12;
	uint32_t eax;
	uint8_t *edi;
	uint32_t p, x, val;

	edi = (uint8_t *)scene.sc_fogwater;

	p = ( 31 );
makefogwater_p_l:
	fpu_reg10 = ( (int32_t)p );
	fpu_reg10 = fpu_reg10 * pi_32;

	x = ( zxres - 1 );
makefogwater_x_l:

	fpu_reg11 = ( (int32_t)x );
	fpu_reg11 = fpu_reg11 * c_fwsin1;
	fpu_reg11 = fpu_reg11 + fpu_reg10;
	fpu_reg11 = sin(fpu_reg11);

	fpu_reg12 = ( (int32_t)x );
	fpu_reg12 = fpu_reg12 * c_fwsin2;
	fpu_reg12 = fpu_reg12 - fpu_reg10;
	fpu_reg12 = sin(fpu_reg12);

	fpu_reg11 = fpu_reg11 + fpu_reg12;
	fpu_reg11 = fabs(fpu_reg11);
	fpu_reg11 = fpu_reg11 + c_fwadd;
	fpu_reg11 = fpu_reg11 * c_fwmul;

	val = (int32_t)ceil(fpu_reg11);
	eax = val;
	*((uint8_t *)(edi)) = (uint8_t) (( (uint8_t)eax ));
	edi = edi + 1;

	x = ( (int32_t)x ) - 1;
	if (( (int32_t)x ) >= 0) goto makefogwater_x_l;



	p = ( (int32_t)p ) - 1;
	if (( (int32_t)p ) >= 0) goto makefogwater_p_l;

//makefogwater_weg:
	return;
}


static void makeflares(void) {
	realnum fpu_reg10, fpu_reg11;
	uint32_t eax, edx, ecx, ebx;
	uint8_t *edi;
	uint32_t x, y, c, z;

	edi = (uint8_t *)scene.sc_flares;

	y = ( 63 );
makeflares_y_l:
	x = ( 63 );
makeflares_x_l:

	ebx = ( 64 * 256 + 64 );
	ebx = (ebx & 0xffffff00) | (uint8_t)(( (uint8_t)ebx ) + ( (uint8_t)x ));
	ebx = set_high_byte(ebx, ( (uint8_t)(ebx >> 8) ) + ( (uint8_t)y ));

	fpu_reg10 = ( (int32_t)x );
	fpu_reg10 = fpu_reg10 * fpu_reg10;
	fpu_reg11 = ( (int32_t)y );
	fpu_reg11 = fpu_reg11 * fpu_reg11;
	fpu_reg10 = fpu_reg10 + fpu_reg11;
	fpu_reg10 = sqrt(fpu_reg10);
	fpu_reg11 = fpu_reg10;

//flare 1
	c = ( 53 );
	fpu_reg11 = fpu_reg11 - ( (int32_t)c );
	fpu_reg11 = fabs(fpu_reg11);
	fpu_reg11 = -fpu_reg11;
	c = ( 10 );
	fpu_reg11 = fpu_reg11 + ( (int32_t)c );



	if (fpu_reg11 < 0.0) goto makeflares_0;
	fpu_reg11 = fpu_reg11 * c_2;
	c = (int32_t)ceil(fpu_reg11);

	eax = /*(eax & 0xffffff00) |*/ (uint8_t)(( (uint8_t)c ));
	*((uint8_t *)(edi + ebx)) = (uint8_t) (( (uint8_t)eax ));

makeflares_0:

//flare 2

	c = ( 50 ); //63
	fpu_reg10 = fpu_reg10 - ( (int32_t)c );
	fpu_reg10 = -fpu_reg10;



	if (fpu_reg10 < 0.0) goto makeflares_1;
	c = (int32_t)ceil(fpu_reg10);

	eax = /*(eax & 0xffffff00) |*/ (uint8_t)(( (uint8_t)c ));
//        shr     al,1

	if (( (uint8_t)eax ) < ( 20 )) goto makeflares_2;
	eax = /*(eax & 0xffffff00) |*/ (uint8_t)(( 20 ));
makeflares_2:
	*((uint8_t *)(edi + ebx + (128))) = (uint8_t) (( (uint8_t)eax ));

makeflares_1:

	x = x - 1;

	if (( (int32_t)x ) >= ( - 64 )) goto makeflares_x_l;

	y = y - 1;

	if (( (int32_t)y ) >= ( - 64 )) goto makeflares_y_l;

//comment ^


	z = ( 15 );
makeflares_z_l:
	c = ( 2 * 256 );
	ebx = z;
	fpu_reg10 = ( flaretab[ebx] );
	fpu_reg11 = cos(fpu_reg10);
	fpu_reg10 = sin(fpu_reg10);
	fpu_reg11 = fpu_reg11 * ( (int32_t)c );
	x = (int32_t)ceil(fpu_reg11);
	fpu_reg10 = fpu_reg10 * ( (int32_t)c );
	y = (int32_t)ceil(fpu_reg10);


	ecx = ( 32 );
	ebx = 0;
	edx = ( 64 * 256 + 64 );
makeflares_l:
	eax = x;
	{ uint32_t carry = (UINT8_MAX - ( (uint8_t)ebx ) < ( (uint8_t)eax ))?1:0; ebx = (ebx & 0xffffff00) | (uint8_t)(( (uint8_t)ebx ) + ( (uint8_t)eax ));
	  edx = (edx & 0xffffff00) | (uint8_t)(( (uint8_t)edx ) + ( (uint8_t)(eax >> 8) ) + carry); }
	eax = y;
	{ uint32_t carry = (UINT8_MAX - ( (uint8_t)(ebx >> 8) ) < ( (uint8_t)eax ))?1:0; ebx = set_high_byte(ebx, ( (uint8_t)(ebx >> 8) ) + ( (uint8_t)eax ));
	  edx = set_high_byte(edx, ( (uint8_t)(edx >> 8) ) + ( (uint8_t)(eax >> 8) ) + carry); }


	if (( (int8_t)edx ) < ( 2 )) goto makeflares_w;

	if (( (int8_t)edx ) >= ( 127 )) goto makeflares_w;

	if (( (int8_t)(edx >> 8) ) < ( 2 )) goto makeflares_w;

	if (( (int8_t)(edx >> 8) ) >= ( 127 )) goto makeflares_w;

	*((uint32_t *)(edi + edx + (- 512 - 2 + 128))) = ( *((uint32_t *)(edi + edx + (- 512 - 2 + 128))) ) + ( 0x01020201 );
	*((uint32_t *)(edi + edx + (- 256 - 2 + 128))) = ( *((uint32_t *)(edi + edx + (- 256 - 2 + 128))) ) + ( 0x02050503 );
	*((uint32_t *)(edi + edx + (- 0 - 2 + 128))) = ( *((uint32_t *)(edi + edx + (- 0 - 2 + 128))) ) + ( 0x02060603 );
	*((uint32_t *)(edi + edx + (256 - 2 + 128))) = ( *((uint32_t *)(edi + edx + (256 - 2 + 128))) ) + ( 0x01020201 );

	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto makeflares_l;
makeflares_w:
	z = ( (int32_t)z ) - 1;
	if (( (int32_t)z ) >= 0) goto makeflares_z_l;
//^
	return;
}


static void makeflaretab(void) {
	uint32_t eax, edx, ecx, ebx;
	uint8_t *edi;

	eax = xres;
	edx = 0;
	ebx = ( 266 ); //401
	{ uint64_t d1 = (((uint64_t)edx) << 32) | eax; uint32_t d2 = ebx; eax = d1 / d2; edx = d1 % d2; }
//        inc     eax
	flare_dx = eax;

	eax = yres;
	edx = 0;
	ebx = ( 200 ); //301
	{ uint64_t d1 = (((uint64_t)edx) << 32) | eax; uint32_t d2 = ebx; eax = d1 / d2; edx = d1 % d2; }
//        inc     eax
	eax = ( (int32_t)eax ) * ( (int32_t)xres );
	flare_dy = eax;

	eax = xres;
	eax = ( (int32_t)eax ) * ( (int32_t)yres );
	flare_ycmp = eax;

	edi = (uint8_t *)scene.sc_flaretab;
	edi = edi + ( 9 * 64 - 1 );

	edx = ( 8 );
makeflaretab_h_l:
	ecx = ( 63 );
makeflaretab_v_l:
	eax = edx;
	eax = ( (int32_t)eax ) * ( (int32_t)ecx );
	eax = eax >> ( 3 ); // /8

	*((uint8_t *)(edi)) = (uint8_t) (( (uint8_t)eax ));
	edi = edi - 1;

	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) >= 0) goto makeflaretab_v_l;

	edx = ( (int32_t)edx ) - 1;
	if (( (int32_t)edx ) >= 0) goto makeflaretab_h_l;

	return;
}


#if 0
//clearbuffer proc near
//        mov     edi,scene.sc_buffer
//        mov     ecx,xres
//        imul    ecx,yres
//        xor     eax,eax
//        rep     stosd
//
//@@weg:  ret
//clearbuffer endp
#endif



#if 0
//trigger proc pascal
//        ;st(0) = new value
//        ;edi -> old value
//local   temp
//
//        fistp   temp
//        mov     edx,[edi]
//        mov     eax,temp
//
//        cmp     eax,edx
//        jle     @@0
//        dec     eax
//@@0:
//        or      eax,eax
//        jns     @@2
//        xor     eax,eax
//@@2:    mov     [edi],eax
//@@weg:  ret
//trigger endp
#endif

extern "C" void initmdata(tvesa *esi, uint32_t _eax) {
	realnum fpu_reg10;
	uint32_t eax = _eax;
	uint8_t *edi;
	//uint32_t stack_var00;
//-> esi -> tvesa
//-> eax = debug
//cld
	//stack_var00 = ( 0 /*ebp*/ );
	debug = eax;

	xbytes = esi->vesa_xbytes;
	xres = esi->vesa_xres;
	yres = esi->vesa_yres;
	linbuf = (uint8_t *)esi->vesa_linbuf;
	texture = (SDL_Texture *)esi->vesa_texture;
	renderer = (SDL_Renderer *)esi->vesa_renderer;
	initpaltab(esi); //esi -> tvesa

	init1();
	makelgouraud();

	edi = (uint8_t *)scene.sc_spot1;
	fpu_reg10 = pi_256;
	makespot(edi, fpu_reg10);
	edi = (uint8_t *)scene.sc_spot2;
	fpu_reg10 = pi_128;
	makespot(edi, fpu_reg10);
	edi = (uint8_t *)scene.sc_water;
	makewater(edi);

	makefogmap();
	makefogwater();

	makeflares();
	makeflaretab();

	//ebp = stack_var00;
//initmdata_weg:
	return;
}


extern "C" void startmese(uint32_t _eax) {
	realnum fpu_reg10;
	uint32_t eax = _eax, edx/*, ebp*/;
	tviewer *edi;
	tcamera *esi;
	void *pixels;
	int pitch;
	//uint32_t stack_var00;
//-> eax = startframe
	//stack_var00 = ( 0 /*ebp*/ );
//cld
#if 0
//    cmp debug,0
//    jne @@t1
//    or eax,eax
//    jz @@t1
//
//    push eax
//    call i8_done
//    pop eax
//    push eax
//@@t0:call int_8t
//    dec eax
//    jnz @@t0
//    call i8_init
//    pop eax
//@@t1:
#endif

	edx = maincount;
	edx = edx - eax;
	mainstart = edx;

startmese_nextframe:
// jmp @@t0

startmese_l2: //update tracks
	eax = maincount;
	eax = eax - mainstart;
	edx = eax;
	edx = edx - frame;
//framemul;
	if (( (int32_t)edx ) < ( 128 )) goto startmese_0;
	frame = frame + ( 128 ); //framemul;
	dotracks(scene.sc_root);
	goto startmese_l2;
startmese_0:
	frame = eax;

//frame-init
	scene.sc_idxlistend = scene.sc_idxlist;

//ambient track
	dotrack(scene.sc_ambient, ( 2 ), &(ambient_f[0]));

	fpu_reg10 = ( ambient_f[0] );
	ambient = (int32_t)ceil(fpu_reg10);
//        lea     edi,ambient
//        call    trigger
//mov ambient,0;63
	fpu_reg10 = ( ambient_f[1] );
	fog = (int32_t)ceil(fpu_reg10);
//  mov fog, 120
//        lea     edi,fog
//        call    trigger

//calculate tracks
	dotracks(scene.sc_root);

	esi = scene.sc_cam;
//fld c_0_7
//fstp [esi].c_FOV
	edi = ( &(viewer) );
	doviewer(&(esi->c_viewer), edi);
	adjustcamera(esi, edi);

	//esi = ( &(viewer) ); //cubetree bzgl. beobachter
	cubetree(( 35 ), &viewer, (ttempcube *)scene.sc_tempdata); //25

//       call    clearbuffer
	texturedraw();
	dolights();


	if (scene.sc_intro == ( 0 )) goto startmese_ex;
// credits
	eax = frame;
	eax = docredits((int8_t *)scene.sc_tempdata, eax);
//eax = -1 : end
	if (( (int32_t)eax ) < 0) goto startmese_wait;
	mainstart = mainstart + eax;

	goto startmese_ex0;
startmese_ex:
//71500
	if (( (int32_t)frame ) > ( 76500 )) goto startmese_w;
startmese_ex0:

	SDL_RenderClear(renderer);
	SDL_LockTexture(texture, NULL, &pixels, &pitch);
	linbuf = (uint8_t *)pixels;
	xbytes = pitch;

	copybuffer();

	SDL_UnlockTexture(texture);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);


//debug = 1
	if (( (int32_t)debug ) > ( 0 )) goto startmese_nextframe;
	if (( (int32_t)debug ) < ( 0 )) goto startmese_d0;
//debug = 0: normal
//    @@t0:
	eax = keypressed();

	if (( (int32_t)eax ) == 0) goto startmese_nextframe;
//        mov     ah,1
//        int     16h
//        jz      @@nextframe
	goto startmese_w;
startmese_d0: //debug = -1: single step
	eax = 0;
//int     16h
	edx = debug;
	edx = - ( (int32_t)edx );
//add     maincount,edx
//esc?
	if (( (uint8_t)eax ) != ( 27 )) goto startmese_nextframe;


startmese_wait:
	eax = maincount;
	mainstart = eax;
startmese_w_l:
//        mov     ah,1
//        int     16h
	eax = keypressed();

	if (( (int32_t)eax ) != 0) goto startmese_w;
	eax = maincount;
	eax = eax - mainstart;

	if (( (int32_t)eax ) < ( 5000 )) goto startmese_w_l;
startmese_w:
//        mov     ax,3
//        int     10h
	//ebp = stack_var00;

	return;
}



