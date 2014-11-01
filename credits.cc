#include <cstdint>
#include <SDL/SDL.h>
#include "cc.h"
//.486                                    //pmode/w
//.model flat,prolog

//locals

#include "int.inc.h"

//public creddata
//public initcdata
//public docredits

extern "C" int keypressed(void);

//±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
//constants
//±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
//NULL = 0
//maxint = 7FFFFFFFh

//b equ byte ptr
//w equ word ptr
//d equ dword ptr

#define ld_fvalues 5
#define fvalues 32

//±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
//struc
//±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±

//vesa-info
#include "vesa.inc.h"
extern "C" void clearlinbuf(void);

typedef struct {
 uint8_t p_red[256];
 uint8_t p_green[256];
 uint8_t p_blue[256];
} tpal;

typedef struct {
 uint32_t cr_xres;
 uint32_t cr_yres;
 uint32_t cr_picdata;
 uint32_t cr_xpos;
 uint32_t cr_ypos;
 uint32_t cr_starttime;
 uint32_t cr_startstep;
 uint32_t cr_fadespeed;
 uint32_t cr_paltab;
} tcredit;

typedef struct {
 uint32_t d_xpos;
 uint32_t d_credit[2];
} tcreditdef;

typedef struct {
 uint32_t pt_red;
 uint32_t pt_green;
 uint32_t pt_blue;
 uint32_t pt_trans;
 uint32_t pt_paltab;
} tpaltab;

typedef struct {
 uint32_t cd_xres;
 uint32_t cd_yres;
 uint32_t cd_pal;       //-> tpal
 uint32_t cd_picdata;
 uint32_t cd_starttime;
 uint32_t cd_scrollspeed;
 uint32_t cd_scrolltime;
 uint32_t cd_enginetime;

 int32_t cd_credits;    //-> array of tcredit

 uint32_t cd_nocreditdefs;
 uint32_t cd_creditdefs;//-> array of tcreditdef

 uint32_t cd_nopaltabs;
 uint32_t cd_paltabs;   //16 bit: fvalues*256*2, 32 bit: fvalues*256*4
 uint32_t cd_paltab;

 uint32_t cd_creditnum;
} tcreddata;



//±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
//.data
//±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±



tcreddata creddata;

static uint32_t pbytes;
static uint32_t xbytes;
static uint32_t xres;
static uint32_t yres;
static uint32_t linbuf;
static uint32_t bufend;
static SDL_Surface *screen;


static uint32_t xstep;
static uint32_t ystep;

//±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
//.code
//±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±



static void mosaic(uint32_t gstep, uint32_t dpic, uint32_t _esi) {
	uint32_t eax, edx, ecx, edi, ebx, esi = _esi;
//-> esi -> tcredit

	uint32_t t;
	uint32_t xa, xe, ya, ye;
	uint32_t xaw, xew, yaw, yew;
	uint32_t x, y;


//  do {

	eax = gstep;
	eax = eax - ( 256 );
	eax = eax << ( 8 );
	edx = 0;

	ebx = ( 256 );
	ebx = ebx - ( ((tcredit *)esi)->cr_startstep );

	{ int64_t d1 = (((uint64_t)edx) << 32) | eax; int32_t d2 = ( (int32_t)ebx ); eax = d1 / d2; edx = d1 % d2; }
	eax = eax + ( 256 );
	t = eax; //transparency (0..256)


	eax = ( ((tcredit *)esi)->cr_yres ); //gmy = yres << 7
	eax = eax << ( 8 - 1 );
	edx = ((int32_t)eax) >> 31; //ya = gmy % gstep - gstep
	{ int64_t d1 = (int64_t)(int32_t)eax; int32_t d2 = ( (int32_t)gstep ); eax = d1 / d2; edx = d1 % d2; }
	edx = edx - gstep;
	ya = edx;


mosaic_do1: //do {
	eax = ya; //yaw = ya >> 8
	eax = ( (int32_t)eax ) >> ( 8 );
	if (( (int32_t)eax ) >= 0) goto mosaic_0; //if (yaw < 0) yaw = 0
	eax = 0;
mosaic_0:
	yaw = eax;

	eax = ya; //ye = ya + gstep
	eax = eax + gstep;
	ye = eax;
	eax = ( (int32_t)eax ) >> ( 8 ); //yew = ye >> 8

	if (( (int32_t)eax ) < ( (int32_t)(((tcredit *)esi)->cr_yres) )) goto mosaic_1; //if (yew > yres) yew = yres
	eax = ( ((tcredit *)esi)->cr_yres );
mosaic_1:
	yew = eax;


	eax = ( ((tcredit *)esi)->cr_xres ); //gmx = xres << 7
	eax = eax << ( 8 - 1 );
	edx = ((int32_t)eax) >> 31; //xa = gmx % gstep - gstep;
	{ int64_t d1 = (int64_t)(int32_t)eax; int32_t d2 = ( (int32_t)gstep ); eax = d1 / d2; edx = d1 % d2; }
	edx = edx - gstep;
	xa = edx;

mosaic_do2: //do {
	eax = xa; //xaw = xa >> 8
	eax = ( (int32_t)eax ) >> ( 8 );
	if (( (int32_t)eax ) >= 0) goto mosaic_2; //if (xaw < 0) xaw = 0
	eax = 0;
mosaic_2:
	xaw = eax;

	eax = xa; //xe = xa + gstep
	eax = eax + gstep;
	xe = eax;
	eax = ( (int32_t)eax ) >> ( 8 ); //xew = xe >> 8

	if (( (int32_t)eax ) < ( (int32_t)(((tcredit *)esi)->cr_xres) )) goto mosaic_3; //if (xew > xres) xew = xres
	eax = ( ((tcredit *)esi)->cr_xres );
mosaic_3:
	xew = eax;


	ebx = yaw; //ebx = yaw
	eax = yew;
	eax = ( (int32_t)eax ) - ( (int32_t)ebx );
	if (( (int32_t)eax ) <= 0) goto mosaic_w0;
	y = eax;

	edx = xaw; //edx = xaw
	eax = xew;
	eax = ( (int32_t)eax ) - ( (int32_t)edx );
	if (( (int32_t)eax ) <= 0) goto mosaic_w0;
	x = eax;

	ebx = ( (int32_t)ebx ) * ( (int32_t)(((tcredit *)esi)->cr_xres) ); //ebx = offset
	ebx = ebx + edx;

	edi = ( ((tcredit *)esi)->cr_picdata ); //edi -> source-pic
	edi = edi + ebx; //edi -> source-square
	eax = 0; //col = 0
	ecx = set_high_byte(0 /*ecx*/, ( (uint8_t)y ));
mosaic_yl0:
	ecx = (ecx & 0xffffff00) | (uint8_t)(( (uint8_t)x ));
mosaic_xl0:

	edx = (uint32_t)( *((uint8_t *)(edi)) );
	eax = eax + edx;
	edi = edi + 1;
	ecx = (ecx & 0xffffff00) | (uint8_t)(( (int8_t)ecx ) - 1);
	if (( (int8_t)ecx ) != 0) goto mosaic_xl0;
	edi = edi - x;
	edi = edi + ( ((tcredit *)esi)->cr_xres );
	ecx = set_high_byte(ecx, ( (int8_t)(ecx >> 8) ) - 1);
	if (( (int8_t)(ecx >> 8) ) != 0) goto mosaic_yl0;

	ecx = x; //ebx = count = x*y
	ecx = ( (int32_t)ecx ) * ( (int32_t)y );
	{ uint64_t m = ((int64_t)(int32_t)eax) * ( (int32_t)t ); eax = (uint32_t)m; edx = (uint32_t)(m >> 32); }
	{ int64_t d1 = (((uint64_t)edx) << 32) | eax; int32_t d2 = ( (int32_t)ecx ); eax = d1 / d2; edx = d1 % d2; } //col /= count

	edi = dpic; //edi -> dest-pic
	edi = edi + ebx;
	ecx = set_high_byte(ecx, ( (uint8_t)y ));
mosaic_yl1:
	ecx = (ecx & 0xffffff00) | (uint8_t)(( (uint8_t)x ));
mosaic_xl1:
	*((uint8_t *)(edi)) = (uint8_t) (( (uint8_t)(eax >> 8) ));
	edi = edi + 1;
	ecx = (ecx & 0xffffff00) | (uint8_t)(( (int8_t)ecx ) - 1);
	if (( (int8_t)ecx ) != 0) goto mosaic_xl1;
	edi = edi - x;
	edi = edi + ( ((tcredit *)esi)->cr_xres );
	ecx = set_high_byte(ecx, ( (int8_t)(ecx >> 8) ) - 1);
	if (( (int8_t)(ecx >> 8) ) != 0) goto mosaic_yl1;

mosaic_w0:
	eax = xe; //xa = xe
	xa = eax;

	eax = xew; //} while (xew < xres)

	if (( (int32_t)eax ) < ( (int32_t)(((tcredit *)esi)->cr_xres) )) goto mosaic_do2;

	eax = ye; //ya = ye
	ya = eax;

	eax = yew; //} while (yew < yres)

	if (( (int32_t)eax ) < ( (int32_t)(((tcredit *)esi)->cr_yres) )) goto mosaic_do1;


//mosaic_weg:
	return;
}




static uint32_t copypic(uint32_t xpos, uint32_t _eax, uint32_t _edx, uint32_t _esi, uint32_t &_edi) {
	uint32_t eax = _eax, edx = _edx, ecx, edi = _edi, ebx, esi = _esi, ebp;
	uint32_t stack_var00, stack_var01;
//-> eax = actual ypos
//-> edx = end ypos
//-> esi -> tcredit
//-> edi -> pos in framebuffer
//<- eax = new ypos
//<- edi -> new pos in framebuffer

	uint32_t ypos, yend;

	edx = edx << ( 8 );

	if (( (int32_t)eax ) >= ( (int32_t)edx )) goto copypic_weg;
	yend = edx;
	ebx = creddata.cd_paltab;
copypic_yl:
	ypos = eax;
	esi = (uint32_t)( (uint8_t)(eax >> 8) );
	esi = ( (int32_t)esi ) * ( (int32_t)creddata.cd_xres );
//        add     esi,xpos
	esi = esi + creddata.cd_picdata;
	eax = xpos; //eax = xpos

	stack_var00 = edi;
	stack_var01 = ( 0 /*ebp*/ );
	ecx = xstep;
	ebp = (uint32_t)( (uint8_t)(ecx >> 8) );
//xor     ch,ch

	ecx = set_high_byte(ecx, ( (uint8_t)eax )); //ch xpos fraction
	eax = eax >> ( 8 );
	esi = esi + eax; //add xpos whole part

//or      bppflag,0
	if (pbytes > ( 3 )) goto copypic_32;
	if (pbytes == ( 3 )) goto copypic_24;
//        jz      @@24
//        jns     @@32

//------ 16 bit
//copypic_16:
	edx = xres;
copypic_l_16:
	eax = (uint32_t)( *((uint8_t *)(esi)) );
	eax = (eax & 0xffff0000) | (uint16_t)(( ((uint16_t *)(ebx))[eax] ));
	{ uint32_t carry = (UINT8_MAX - ( (uint8_t)(ecx >> 8) ) < ( (uint8_t)ecx ))?1:0; ecx = set_high_byte(ecx, ( (uint8_t)(ecx >> 8) ) + ( (uint8_t)ecx ));
	  esi = esi + ebp + carry; }
	*((uint16_t *)(edi)) = (uint16_t) (( (uint16_t)eax ));
	edi = edi + ( 2 );
	edx = ( (int32_t)edx ) - 1;
	if (( (int32_t)edx ) != 0) goto copypic_l_16;

	goto copypic_endcase;
copypic_24: //------ 24 bit
	edx = xres;
copypic_l_24:
	eax = (uint32_t)( *((uint8_t *)(esi)) );
	eax = ( ((uint32_t *)(ebx))[eax] );
	{ uint32_t carry = (UINT8_MAX - ( (uint8_t)(ecx >> 8) ) < ( (uint8_t)ecx ))?1:0; ecx = set_high_byte(ecx, ( (uint8_t)(ecx >> 8) ) + ( (uint8_t)ecx ));
	  esi = esi + ebp + carry; }
	*((uint16_t *)(edi)) = (uint16_t) (( (uint16_t)eax ));
	eax = ((eax & 0xff000000) >> 24) | ((eax & 0x00ff0000) >> 8) | ((eax & 0x0000ff00) << 8) | ((eax & 0x000000ff) << 24);
	((uint8_t *)(edi))[2] = (uint8_t) (( (uint8_t)(eax >> 8) ));
	edi = edi + ( 3 );
	edx = ( (int32_t)edx ) - 1;
	if (( (int32_t)edx ) != 0) goto copypic_l_24;

	goto copypic_endcase;
copypic_32: //------ 32 bit
	edx = xres;
copypic_l_32:
	eax = (uint32_t)( *((uint8_t *)(esi)) );
	eax = ( ((uint32_t *)(ebx))[eax] );
	{ uint32_t carry = (UINT8_MAX - ( (uint8_t)(ecx >> 8) ) < ( (uint8_t)ecx ))?1:0; ecx = set_high_byte(ecx, ( (uint8_t)(ecx >> 8) ) + ( (uint8_t)ecx ));
	  esi = esi + ebp + carry; }
	*((uint32_t *)(edi)) = eax;
	edi = edi + ( 4 );
	edx = ( (int32_t)edx ) - 1;
	if (( (int32_t)edx ) != 0) goto copypic_l_32;

//------
copypic_endcase:

	ebp = stack_var01;
	edi = stack_var00;
	edi = edi + xbytes;

	if (edi >= bufend) goto copypic_weg;

	eax = ypos;
	eax = eax + ystep;

	if (( (int32_t)eax ) < ( (int32_t)yend )) goto copypic_yl;

copypic_weg:
	_edi = edi;
	return eax;
}


static uint32_t copycpic(uint32_t xpos, uint32_t mpic, uint32_t _eax, uint32_t &_esi, uint32_t &_edi) {
	uint32_t eax = _eax, edx, ecx, edi = _edi, ebx, esi = _esi;
	uint32_t stack_var00;
//-> eax = actual ypos
//-> esi -> tcredit
//-> edi -> pos in framebuffer
//<- eax = new ypos
//<- edi -> new pos in framebuffer

	uint32_t ypos, yend;
	uint32_t credit;
	uint32_t c1, c2, c3, z, hiadd;
	uint8_t frac;

	edx = ( ((tcredit *)esi)->cr_ypos );
	edx = edx + ( ((tcredit *)esi)->cr_yres );
	edx = edx << ( 8 );

	if (( (int32_t)eax ) >= ( (int32_t)edx )) goto copycpic_weg;
	yend = edx;
	ypos = eax;

	ebx = xstep; //ebx = xstep
	ecx = xres;
	eax = ( ((tcredit *)esi)->cr_xpos );
	eax = eax << ( 8 );
	edx = 0;
	{ uint64_t d1 = (uint64_t)eax; uint32_t d2 = ebx; eax = d1 / d2; edx = d1 % d2; }
	ecx = ecx - eax;
	c1 = eax;
	eax = ( ((tcredit *)esi)->cr_xres );
	eax = eax << ( 8 );
	edx = 0;
	{ uint64_t d1 = (uint64_t)eax; uint32_t d2 = ebx; eax = d1 / d2; edx = d1 % d2; }
	ecx = ecx - eax;
	c2 = eax;
	c3 = ecx;

	ecx = (ecx & 0xffffff00) | (uint8_t)(( (uint8_t)ebx )); //cl = loadd
	ebx = ebx >> ( 8 );
	hiadd = ebx;

	credit = esi;

	ebx = ( ((tcredit *)esi)->cr_paltab );
copycpic_yl:
	esi = (uint32_t)( (uint8_t)(ypos >> 8) );
	esi = ( (int32_t)esi ) * ( (int32_t)creddata.cd_xres );
	eax = xpos;
	ecx = set_high_byte(ecx, ( (uint8_t)eax ));
	eax = eax >> ( 8 );
	esi = esi + eax; //xpos
	esi = esi + creddata.cd_picdata;

//        xor     ch,ch

	stack_var00 = edi;

//or      bppflag,0
	if (pbytes > ( 3 )) goto copycpic_32;
	if (pbytes == ( 3 )) goto copycpic_24;
//        jz      @@24
//        jns     @@32
//------ 16 bit
//copycpic_16:
	edx = c1;
copycpic_l1_16:
	eax = (uint32_t)( *((uint8_t *)(esi)) );
	eax = (eax & 0xffff0000) | (uint16_t)(( ((uint16_t *)(ebx))[eax] ));
	{ uint32_t carry = (UINT8_MAX - ( (uint8_t)(ecx >> 8) ) < ( (uint8_t)ecx ))?1:0; ecx = set_high_byte(ecx, ( (uint8_t)(ecx >> 8) ) + ( (uint8_t)ecx ));
	  esi = esi + hiadd + carry; }
	*((uint16_t *)(edi)) = (uint16_t) (( (uint16_t)eax ));
	edi = edi + ( 2 );
	edx = ( (int32_t)edx ) - 1;
	if (( (int32_t)edx ) != 0) goto copycpic_l1_16;

	eax = credit;
	edx = (uint32_t)( (uint8_t)(ypos >> 8) );
	edx = edx - ( ((tcredit *)eax)->cr_ypos );
	edx = ( (int32_t)edx ) * ( (int32_t)(((tcredit *)eax)->cr_xres) );
	edx = edx + mpic;
//sub     edx,esi
	frac = (( 0 ));

	eax = c2;
	z = eax;
copycpic_l2_16:
	eax = (uint32_t)( *((uint8_t *)(esi)) );
	eax = set_high_byte(eax, ( *((uint8_t *)(edx)) ));
	{ uint32_t carry = (UINT8_MAX - ( (uint8_t)(ecx >> 8) ) < ( (uint8_t)ecx ))?1:0; ecx = set_high_byte(ecx, ( (uint8_t)(ecx >> 8) ) + ( (uint8_t)ecx ));
	  esi = esi + hiadd + carry; }
	eax = (eax & 0xffff0000) | (uint16_t)(( ((uint16_t *)(ebx))[eax] ));
	{ uint32_t carry = (UINT8_MAX - frac < ( (uint8_t)ecx ))?1:0; frac = (frac + ( (uint8_t)ecx ));
	  edx = edx + hiadd + carry; }
	*((uint16_t *)(edi)) = (uint16_t) (( (uint16_t)eax ));
	edi = edi + ( 2 );
	z = ( (int32_t)z ) - 1;
	if (( (int32_t)z ) != 0) goto copycpic_l2_16;

	edx = c3;
copycpic_l3_16:
	eax = (uint32_t)( *((uint8_t *)(esi)) );
	eax = (eax & 0xffff0000) | (uint16_t)(( ((uint16_t *)(ebx))[eax] ));
	{ uint32_t carry = (UINT8_MAX - ( (uint8_t)(ecx >> 8) ) < ( (uint8_t)ecx ))?1:0; ecx = set_high_byte(ecx, ( (uint8_t)(ecx >> 8) ) + ( (uint8_t)ecx ));
	  esi = esi + hiadd + carry; }
	*((uint16_t *)(edi)) = (uint16_t) (( (uint16_t)eax ));
	edi = edi + ( 2 );
	edx = ( (int32_t)edx ) - 1;
	if (( (int32_t)edx ) != 0) goto copycpic_l3_16;

	goto copycpic_endcase;
copycpic_24: //------ 24 bit
	edx = c1;
copycpic_l1_24:
	eax = (uint32_t)( *((uint8_t *)(esi)) );
	eax = ( ((uint32_t *)(ebx))[eax] );
	{ uint32_t carry = (UINT8_MAX - ( (uint8_t)(ecx >> 8) ) < ( (uint8_t)ecx ))?1:0; ecx = set_high_byte(ecx, ( (uint8_t)(ecx >> 8) ) + ( (uint8_t)ecx ));
	  esi = esi + hiadd + carry; }
	*((uint16_t *)(edi)) = (uint16_t) (( (uint16_t)eax ));
	eax = ((eax & 0xff000000) >> 24) | ((eax & 0x00ff0000) >> 8) | ((eax & 0x0000ff00) << 8) | ((eax & 0x000000ff) << 24);
	((uint8_t *)(edi))[2] = (uint8_t) (( (uint8_t)(eax >> 8) ));
	edi = edi + ( 3 );
	edx = ( (int32_t)edx ) - 1;
	if (( (int32_t)edx ) != 0) goto copycpic_l1_24;

	eax = credit;
	edx = (uint32_t)( (uint8_t)(ypos >> 8) );
	edx = edx - ( ((tcredit *)eax)->cr_ypos );
	edx = ( (int32_t)edx ) * ( (int32_t)(((tcredit *)eax)->cr_xres) );
	edx = edx + mpic;
//        sub     edx,esi
	frac = (( 0 ));

	eax = c2;
	z = eax;
copycpic_l2_24:
	eax = (uint32_t)( *((uint8_t *)(esi)) );
	eax = set_high_byte(eax, ( *((uint8_t *)(edx)) ));
	{ uint32_t carry = (UINT8_MAX - ( (uint8_t)(ecx >> 8) ) < ( (uint8_t)ecx ))?1:0; ecx = set_high_byte(ecx, ( (uint8_t)(ecx >> 8) ) + ( (uint8_t)ecx ));
	  esi = esi + hiadd + carry; }
	eax = ( ((uint32_t *)(ebx))[eax] );
	*((uint16_t *)(edi)) = (uint16_t) (( (uint16_t)eax ));
	{ uint32_t carry = (UINT8_MAX - frac < ( (uint8_t)ecx ))?1:0; frac = (frac + ( (uint8_t)ecx ));
	  edx = edx + hiadd + carry; }
	eax = ((eax & 0xff000000) >> 24) | ((eax & 0x00ff0000) >> 8) | ((eax & 0x0000ff00) << 8) | ((eax & 0x000000ff) << 24);
	((uint8_t *)(edi))[2] = (uint8_t) (( (uint8_t)(eax >> 8) ));
	edi = edi + ( 3 );
	z = ( (int32_t)z ) - 1;
	if (( (int32_t)z ) != 0) goto copycpic_l2_24;

	edx = c3;
copycpic_l3_24:
	eax = (uint32_t)( *((uint8_t *)(esi)) );
	eax = ( ((uint32_t *)(ebx))[eax] );
	{ uint32_t carry = (UINT8_MAX - ( (uint8_t)(ecx >> 8) ) < ( (uint8_t)ecx ))?1:0; ecx = set_high_byte(ecx, ( (uint8_t)(ecx >> 8) ) + ( (uint8_t)ecx ));
	  esi = esi + hiadd + carry; }
	*((uint16_t *)(edi)) = (uint16_t) (( (uint16_t)eax ));
	eax = ((eax & 0xff000000) >> 24) | ((eax & 0x00ff0000) >> 8) | ((eax & 0x0000ff00) << 8) | ((eax & 0x000000ff) << 24);
	((uint8_t *)(edi))[2] = (uint8_t) (( (uint8_t)(eax >> 8) ));
	edi = edi + ( 3 );
	edx = ( (int32_t)edx ) - 1;
	if (( (int32_t)edx ) != 0) goto copycpic_l3_24;

	goto copycpic_endcase;
copycpic_32: //------ 32 bit
	edx = c1;
copycpic_l1_32:
	eax = (uint32_t)( *((uint8_t *)(esi)) );
	eax = ( ((uint32_t *)(ebx))[eax] );
	{ uint32_t carry = (UINT8_MAX - ( (uint8_t)(ecx >> 8) ) < ( (uint8_t)ecx ))?1:0; ecx = set_high_byte(ecx, ( (uint8_t)(ecx >> 8) ) + ( (uint8_t)ecx ));
	  esi = esi + hiadd + carry; }
	*((uint32_t *)(edi)) = eax;
	edi = edi + ( 4 );
	edx = ( (int32_t)edx ) - 1;
	if (( (int32_t)edx ) != 0) goto copycpic_l1_32;

	eax = credit;
	edx = (uint32_t)( (uint8_t)(ypos >> 8) );
	edx = edx - ( ((tcredit *)eax)->cr_ypos );
	edx = ( (int32_t)edx ) * ( (int32_t)(((tcredit *)eax)->cr_xres) );
	edx = edx + mpic;
//        sub     edx,esi
	frac = (( 0 ));

	eax = c2;
	z = eax;
copycpic_l2_32:
	eax = (uint32_t)( *((uint8_t *)(esi)) );
	eax = set_high_byte(eax, ( *((uint8_t *)(edx)) ));
	{ uint32_t carry = (UINT8_MAX - ( (uint8_t)(ecx >> 8) ) < ( (uint8_t)ecx ))?1:0; ecx = set_high_byte(ecx, ( (uint8_t)(ecx >> 8) ) + ( (uint8_t)ecx ));
	  esi = esi + hiadd + carry; }
	eax = ( ((uint32_t *)(ebx))[eax] );
	{ uint32_t carry = (UINT8_MAX - frac < ( (uint8_t)ecx ))?1:0; frac = (frac + ( (uint8_t)ecx ));
	  edx = edx + hiadd + carry; }
	*((uint32_t *)(edi)) = eax;
	edi = edi + ( 4 );
	z = ( (int32_t)z ) - 1;
	if (( (int32_t)z ) != 0) goto copycpic_l2_32;

	edx = c3;
copycpic_l3_32:
	eax = (uint32_t)( *((uint8_t *)(esi)) );
	eax = ( ((uint32_t *)(ebx))[eax] );
	{ uint32_t carry = (UINT8_MAX - ( (uint8_t)(ecx >> 8) ) < ( (uint8_t)ecx ))?1:0; ecx = set_high_byte(ecx, ( (uint8_t)(ecx >> 8) ) + ( (uint8_t)ecx ));
	  esi = esi + hiadd + carry; }
	*((uint32_t *)(edi)) = eax;
	edi = edi + ( 4 );
	edx = ( (int32_t)edx ) - 1;
	if (( (int32_t)edx ) != 0) goto copycpic_l3_32;
//------
copycpic_endcase:

	edi = stack_var00;
	edi = edi + xbytes;

	eax = ypos;
	eax = eax + ystep;
	ypos = eax;

	if (( (int32_t)eax ) < ( (int32_t)yend )) goto copycpic_yl;

copycpic_weg:
	_edi = edi;
	_esi = esi;
	return eax;
}



extern "C" uint32_t docredits(uint32_t mpic, uint32_t _eax) {
	uint32_t eax = _eax, edx, ecx, edi, ebx, esi;
	uint32_t stack_var00, stack_var01, stack_var02;
//, creditnum            ;mpic: mosaic picture
//-> eax = frame
//<- eax = scrolltime
	uint32_t creditdef;
	uint32_t mainstart, time;
	uint32_t ypos, fbuf, xpos;
	uint8_t last;

	esi = ( (uint32_t)&(creddata) );
	eax = ( (int32_t)eax ) - ( (int32_t)(((tcreddata *)esi)->cd_starttime) ); //eax = relative time
	if (( (int32_t)eax ) < 0) goto docredits_no_c;
	edx = maincount;
	edx = edx - eax;
	mainstart = edx;

	edx = ( ((tcreddata *)esi)->cd_enginetime );
	((tcreddata *)esi)->cd_starttime = ( ((tcreddata *)esi)->cd_starttime ) + edx;

	eax = ( ((tcreddata *)esi)->cd_creditnum ); //credit number
	edx = eax;
	edx = edx + 1;

	last = ((edx >= ( ((tcreddata *)esi)->cd_nocreditdefs ))?1:0);
	((tcreddata *)esi)->cd_creditnum = ( ((tcreddata *)esi)->cd_creditnum ) + 1;

	eax = ( (int32_t)eax ) * ( sizeof(tcreditdef) );
	eax = eax + ( ((tcreddata *)esi)->cd_creditdefs );
	creditdef = eax;


	clearlinbuf();

docredits_fl:

	eax = maincount; //time
	eax = eax - mainstart;
	time = eax;

	eax = ( (int32_t)eax ) * ( (int32_t)creddata.cd_scrollspeed ); //xpos
	eax = ( (int32_t)eax ) >> ( 10 - 8 );
	ebx = creditdef;
	edx = ( ((tcreditdef *)ebx)->d_xpos );
	edx = edx << ( 8 );
	eax = eax + edx;
	edx = creddata.cd_xres;
	edx = edx - ( 320 );
	edx = edx << ( 8 );

	if (( (int32_t)eax ) < ( (int32_t)edx )) goto docredits_0;
	eax = edx;
docredits_0:
	ebx = eax; //round to multiple of xstep
	edx = 0;
	{ uint64_t d1 = (uint64_t)eax; uint32_t d2 = xstep; eax = d1 / d2; edx = d1 % d2; }
	ebx = ebx - edx;
	xpos = ebx;

	SDL_LockSurface(screen);
	linbuf = (uint32_t)screen->pixels;
	bufend = linbuf + xbytes * yres;


	ypos = ( 0 );
	edi = linbuf;
	fbuf = edi;

	ecx = 0;

docredits_cl: //credit loop
	ebx = creditdef;
	esi = ( ((tcreditdef *)ebx)->d_credit[ecx] );

	if (( (int32_t)esi ) == 0) goto docredits_w0;

	eax = ( ((tcredit *)esi)->cr_starttime );
	eax = ( (int32_t)eax ) - ( (int32_t)time );
	if (( (int32_t)eax ) > 0) goto docredits_w0;

	stack_var00 = ecx;

	stack_var01 = eax;
	stack_var02 = esi;
	eax = ypos;
	edx = ( ((tcredit *)esi)->cr_ypos );
	edi = fbuf;
	eax = copypic(xpos, eax, edx, esi, edi); //only copy picture
	fbuf = edi;
	ypos = eax;
	esi = stack_var02;
	eax = stack_var01;

	eax = ( (int32_t)eax ) * ( (int32_t)(((tcredit *)esi)->cr_fadespeed) );
	eax = ( (int32_t)eax ) >> ( 10 );
	eax = eax + ( ((tcredit *)esi)->cr_startstep );
	ebx = ( ((tcredit *)esi)->cr_picdata ); //ebx -> mpic

	if (( (int32_t)eax ) <= ( 256 )) goto docredits_1;
	mosaic(eax, mpic, esi);
	ebx = mpic; //ebx -> mpic
docredits_1:
	edi = fbuf;
	eax = ypos;
	eax = copycpic(xpos, ebx, eax, esi, edi); //copy pic with credit
	fbuf = edi;
	ypos = eax;

	ecx = stack_var00;
docredits_w0:
	ecx = ecx + 1;

	if (ecx < ( 2 )) goto docredits_cl;


	eax = ypos; //copy rest
	edx = ( 1000 );
	edi = fbuf;
	eax = copypic(xpos, eax, edx, esi, edi);

	SDL_UnlockSurface(screen);
	SDL_Flip(screen);

//        mov     ah,1
//        int     16h
	eax = keypressed();

	if (( (int32_t)eax ) != 0) goto docredits_end;
//    xor     eax,eax
//    int     16h
//    cmp     al,27
//    je      @@weg
//    add maincount,33

	eax = creddata.cd_scrolltime;

	if (( (int32_t)time ) < ( (int32_t)eax )) goto docredits_fl;


	if (last == ( 1 )) goto docredits_end;

	clearlinbuf();
	eax = creddata.cd_scrolltime;
	goto docredits_weg;
docredits_end:
	eax = 0;
	eax = eax - 1;
	goto docredits_weg;
docredits_no_c:
	eax = 0;
docredits_weg:
	return eax;
}






static void initpaltab(uint32_t _ebx, uint32_t _esi) {
	uint32_t eax, edx, ecx, edi, ebx = _ebx, esi = _esi;
//-> ebx -> tpaltab
//-> esi -> tvesa
	uint32_t col, z, dmul;

	z = ( fvalues - 1 );
initpaltab_fl:
	eax = z;
	eax = ( (int32_t)eax ) * ( (int32_t)(((tpaltab *)ebx)->pt_trans) );
	dmul = eax;

	edx = ( 255 );
initpaltab_pl:
	edi = creddata.cd_pal;

	eax = (uint32_t)( ((tpal *)(edi + edx))->p_red[0] ); //red
	ecx = ( ((tpaltab *)ebx)->pt_red );
	ecx = ecx - eax;
	ecx = ( (int32_t)ecx ) * ( (int32_t)dmul ); //z
	ecx = ecx >> ( ld_fvalues + 8 );
	eax = eax + ecx;
	ecx = (ecx & 0xffffff00) | (uint8_t)(( ((tvesa *)esi)->vesa_redbits ));
	eax = eax << ( (uint8_t)ecx );
	eax = eax >> ( 8 );
	ecx = (ecx & 0xffffff00) | (uint8_t)(( ((tvesa *)esi)->vesa_redpos ));
	eax = eax << ( (uint8_t)ecx );
	col = eax;

	eax = (uint32_t)( ((tpal *)(edi + edx))->p_green[0] ); //green
	ecx = ( ((tpaltab *)ebx)->pt_green );
	ecx = ecx - eax;
	ecx = ( (int32_t)ecx ) * ( (int32_t)dmul ); //z
	ecx = ecx >> ( ld_fvalues + 8 );
	eax = eax + ecx;
	ecx = (ecx & 0xffffff00) | (uint8_t)(( ((tvesa *)esi)->vesa_greenbits ));
	eax = eax << ( (uint8_t)ecx );
	eax = eax >> ( 8 );
	ecx = (ecx & 0xffffff00) | (uint8_t)(( ((tvesa *)esi)->vesa_greenpos ));
	eax = eax << ( (uint8_t)ecx );
	col = col | eax;

	eax = (uint32_t)( ((tpal *)(edi + edx))->p_blue[0] ); //blue
	ecx = ( ((tpaltab *)ebx)->pt_blue );
	ecx = ecx - eax;
	ecx = ( (int32_t)ecx ) * ( (int32_t)dmul ); //z
	ecx = ecx >> ( ld_fvalues + 8 );
	eax = eax + ecx;
	ecx = (ecx & 0xffffff00) | (uint8_t)(( ((tvesa *)esi)->vesa_bluebits ));
	eax = eax << ( (uint8_t)ecx );
	eax = eax >> ( 8 );
	ecx = (ecx & 0xffffff00) | (uint8_t)(( ((tvesa *)esi)->vesa_bluepos ));
	eax = eax << ( (uint8_t)ecx );
	eax = eax | col;

	edi = ( ((tpaltab *)ebx)->pt_paltab );
	ecx = edx;
	ecx = set_high_byte(ecx, ( (uint8_t)z ));
//bpp,16
	if (( ((tvesa *)esi)->vesa_pbytes ) > ( 2 )) goto initpaltab_32;
	((uint16_t *)(edi))[ecx] = (uint16_t) (( (uint16_t)eax ));
	goto initpaltab_0;
initpaltab_32:
	((uint32_t *)(edi))[ecx] = eax;
initpaltab_0:
	edx = ( (int32_t)edx ) - 1;
	if (( (int32_t)edx ) >= 0) goto initpaltab_pl;

	z = ( (int32_t)z ) - 1;
	if (( (int32_t)z ) >= 0) goto initpaltab_fl;

//initpaltab_weg:
	return;
}








extern "C" void initcdata(uint32_t _esi) {
	uint32_t eax, edx, ecx, ebx, esi = _esi/*, ebp*/;
	uint32_t /*stack_var00, */stack_var01;
//-> esi -> tvesa
//cld
	//stack_var00 = ( 0 /*ebp*/ );


	edx = ( ((tvesa *)esi)->vesa_xbytes );
	xbytes = edx;
	eax = ( ((tvesa *)esi)->vesa_xres );
	xres = eax;
	ebx = ( ((tvesa *)esi)->vesa_yres );
	yres = ebx;
	eax = ( ((tvesa *)esi)->vesa_linbuf );
	linbuf = eax;
	screen = (SDL_Surface *) ( ((tvesa *)esi)->vesa_screen );

	edx = ( (int32_t)edx ) * ( (int32_t)ebx ); //bufend
	eax = eax + edx;
	bufend = eax;

	eax = ( ((tvesa *)esi)->vesa_pbytes ); //bpp      ;bppflag
	pbytes = eax;
//        sub     eax,17
//        sar     eax,3
//        mov     bppflag,eax             ;-1: 16 bit, 0: 24 bit, 1: 32 bit

//calc paltabs
	ecx = creddata.cd_nopaltabs;
	ebx = creddata.cd_paltabs;
initcdata_ptl:
	stack_var01 = ecx;
	initpaltab(ebx, esi); //esi -> tvesa
	ecx = stack_var01;
	ebx = ebx + ( sizeof(tpaltab) );
	ecx = ( (int32_t)ecx ) - 1;
	if (( (int32_t)ecx ) != 0) goto initcdata_ptl;

	eax = ( 320 * 256 );
	edx = 0;
	{ uint64_t d1 = (uint64_t)eax; uint32_t d2 = xres; eax = d1 / d2; edx = d1 % d2; }
	xstep = eax;
	eax = creddata.cd_yres;
	eax = eax << ( 8 );
	edx = 0;
	{ uint64_t d1 = (uint64_t)eax; uint32_t d2 = yres; eax = d1 / d2; edx = d1 % d2; }
	ystep = eax;

	creddata.cd_creditnum = ( 0 );

//initcdata_weg:
	//ebp = stack_var00;
	return;
}






