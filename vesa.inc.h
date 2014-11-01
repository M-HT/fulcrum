
typedef struct {
 uint32_t vesa_mode;
 uint32_t vesa_physbaseptr;
 uint32_t vesa_xbytes;
 uint32_t vesa_xres;
 uint32_t vesa_yres;
 uint32_t vesa_pbytes;
 uint8_t vesa_bpplist[4];
 uint32_t vesa_area;
 uint32_t vesa_memsize;                  //xbytes*yres
 uint32_t vesa_linbuf;
 uint8_t vesa_flags;
 uint8_t vesa_redbits;
 uint8_t vesa_redpos;
 uint8_t vesa_greenbits;
 uint8_t vesa_greenpos;
 uint8_t vesa_bluebits;
 uint8_t vesa_bluepos;
 uint8_t vesa_reserved1;
 void *vesa_screen;
} tvesa;
