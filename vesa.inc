
tvesa struc
 vesa_mode        dd ?
 vesa_physbaseptr dd ?
 vesa_xbytes      dd ?
 vesa_xres        dd ?
 vesa_yres        dd ?
 vesa_pbytes      dd ?
 vesa_bpplist     db 4 dup(?)
 vesa_area        dd ?
 vesa_memsize     dd ?                  ;xbytes*yres
 vesa_linbuf      dd ?
 vesa_flags       db ?
 vesa_redbits     db ?
 vesa_redpos      db ?
 vesa_greenbits   db ?
 vesa_greenpos    db ?
 vesa_bluebits    db ?
 vesa_bluepos     db ?
ends
