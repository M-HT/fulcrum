include switches.inc


if pmw gt 0

.386                                    ;pmode/w
.model flat,prolog

else

.386p
code32  segment para public use32       ;tran
        assume cs:code32, ds:code32

include pmode.inc
endif

locals

public i8_init, i8_done

public maincount
public newhandler
public add_time
public reset_timestep

if pmw gt 0
public _seldata
public _setirqmask
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
.data
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
oldint8    df 0       ;deinit nur wenn ungleich 0

else

opmirq     dd ?
ormirq     dd 0     ;deinit nur wenn ungleich 0
rmirqbuf   db 21 dup (?)

endif


c_1 = 65536

hnum = 4

handler label dword
dd hnum dup(0)
constant label dword
dd hnum dup(0)
counter label dword
dd hnum dup(0)


akttime dd 0
timestep dd 256
maintime label dword
mainfract db 0
;  fake dd 0
maincount dd 0

if pmw gt 0
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
.code
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±

_seldata dw ?
endif

int_8 proc
        pushad
        push    ds es
        mov     ds,cs:_seldata
        push    ds
        pop     es
        cld

        mov     eax,timestep
        add     maintime,eax
        adc     byte ptr maintime[4],0

;        inc     maincount

        mov     ebx,hnum-1
@@l:
        mov     eax,constant[ebx*4]
        cmp     eax,c_1                 ;constant < 65536 : one-shot
        jae     @@1
        dec     eax
        js      @@w
        mov     constant[ebx*4],eax
        jnz     @@w
        jmp     @@2
@@1:
        add     counter[ebx*4],eax      ;periodischer aufruf
        cmp     counter[ebx*4],1000*c_1
        jb      @@w
        sub     counter[ebx*4],1000*c_1
@@2:
        push    ebx                     ;handler aufrufen
        call    handler[ebx*4]
        pop     ebx
@@w:
        dec     ebx
        jns     @@l

@@weg:  pop     es ds
        mov     al,20h
        out     20h,al
        popad
        sti
        iretd
int_8 endp

newhandler proc near
        ;eax = frequenz
        ;edx = handler
        mov     ebx,hnum-1              ;alten handler suchen
@@l0:   cmp     handler[ebx*4],edx
        jne     @@0
        mov     constant[ebx*4],eax     ;neue frequenz
        jmp     @@weg
@@0:    dec     ebx
        jns     @@l0

        xor     ebx,ebx                 ;neuen handler einrichten
@@l1:   cmp     constant[ebx*4],0
        jne     @@1
        mov     handler[ebx*4],edx
        mov     constant[ebx*4],eax
        jmp     @@weg
@@1:    inc     ebx
        cmp     ebx,hnum
        jb      @@l1

@@weg:  ret
newhandler endp

i8_init proc near

if pmw gt 0
        mov     _seldata,ds

        push    es
        mov     ax,3508h
        int     21h
        mov     dword ptr oldint8 +0,ebx
        mov     word ptr oldint8  +4,es
        pop     es

        push    ds
        mov     ax,2508h
        push    cs
        pop     ds
        mov     edx,offset int_8
        int     21h
        pop     ds
else
        mov     bl,0                    ;irq0 = int8
        call    _getirqvect
        mov     opmirq,edx

        mov     edx,offset int_8
        call    _setirqvect
        mov     edi,offset rmirqbuf
        call    _rmpmirqset
        mov     ormirq,eax
endif
        cli
        mov     al,34h
        out     43h,al                  ;z„hler setzen
        mov     ax,1193                 ;1193180 div 1000
        out     40h,al                  ;1000 mal pro sekunde
        mov     al,ah
        out     40h,al
        sti

        ret
i8_init endp

i8_done proc near
if pmw gt 0
        cmp     word ptr oldint8 +4,0
else
        cmp     ormirq,0
endif
        je      @@weg

        cli
        mov     al,34h
        out     43h,al
        xor     al,al                   ;z„hler wieder auf 18.2 mal/s
        out     40h,al
        out     40h,al
        sti

if pmw gt 0
        push    ds
        mov     ax,2508h
        lds     edx,oldint8
        int     21h
        pop     ds

        mov     word ptr oldint8 +4,0
else
        mov     bl,0                    ;irq0 = int8
        mov     eax,ormirq
        call    _rmpmirqfree
        mov     edx,opmirq
        call    _setirqvect

        mov     ormirq,0
endif

@@weg:  ret
i8_done endp

add_time proc near

        add     eax,akttime
        mov     akttime,eax
        sub     eax,maintime
        sar     eax,7
        jns     @@0                     ;pause vorangegangen
        mov     eax,maintime
        mov     akttime,eax
        xor     eax,eax
@@0:
        mov     timestep,eax
        ret
add_time endp


reset_timestep proc near

        mov     timestep,256

        ret
reset_timestep endp


;delay proc near
        ;-> eax = delay

;        mov     timestep,256

;        mov     edx,maincount
;        add     edx,eax
;@@d:
;        cmp     edx,maincount
;        jg      @@d

;        ret
;delay endp


if pmw gt 0
_setirqmask proc near
        mov     _seldata,ds

        mov     cl,bl
        mov     bx,0fffeh
        movzx   dx,al
        rol     bx,cl
        shl     dx,cl
        in      al,0a1h
        mov     ah,al
        in      al,21h
        and     ax,bx
        or      ax,dx
        out     21h,al
        mov     al,ah
        out     0a1h,al

        ret
_setirqmask endp
endif


if pmw eq 0
code32  ends
endif

end

