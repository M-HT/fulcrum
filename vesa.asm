.386                                    ;pmode/w
.model flat,prolog

locals

public getvbeinfoblock
public getmodenum
public getmodeinfoblock
public mapaddress
public setmode
public setpal
public clearlinbuf
public textmode
public keypressed
public mode13

extrn grab:near
  extrn maincount:dword


b equ byte ptr
w equ word ptr

;--- VbeInfoBlock ---
VbeInfoBlock struc
VbeSignature            dd ?            ; VBE Signature ('VESA')
VbeVersion              dw ?            ; VBE Version (0200h)
OemStringPtr            dd ?            ; Pointer to OEM String
Capabilities            db 4 dup (?)    ; Capabilities of graphics controller
VideoModePtr            dd ?            ; Pointer to VideoModeList
TotalMemory             dw ?            ; Number of 64kb memory blocks
                                        ; Added for VBE 2.0
OemSoftwareRev          dw ?            ; VBE implementation Software revision
OemVendorNamePtr        dd ?            ; Pointer to Vendor Name String
OemProductNamePtr       dd ?            ; Pointer to Product Name String
OemProductRevPtr        dd ?            ; Pointer to Product Revision String
Reserved_v              db 222 dup (?)  ; Reserved for VBE implementation scratch ; area
OemData                 db 256 dup (?)  ; Data Area for OEM Strings
VbeInfoBlock ends


include vesa.inc

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
.data
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
oldmode dw 0
vbeinfoseg dw ?

;int-call
r_regs label byte
r_EDI    label dword
r_DI     dw ?, ?

r_ESI    dd ?
r_EBP    dd ?
r_free1  dd ?

r_EBX    label dword
r_BX     dw ?, ?

r_EDX    label dword
r_DX     dw ?, ?

r_ECX    label dword
r_CX     dw ?, ?

r_EAX    label dword
r_AX     dw ?, ?

r_flags  dw ?
r_ES     dw ?
r_DS     dw ?
r_FS     dw ?
r_GS     dw ?
r_IP     dw ?
r_CS     dw ?
r_SS_SP  label dword
r_SP     dw ?
r_SS     dw ?

linbuf   dd ?
memsize  dd ?
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
.code
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±

int10h proc near

        mov     ax,300h                 ;Simulate Real Mode Interrupt
        mov     bx,10h                  ;int 10h
        xor     ecx,ecx
        mov     r_flags,cx
        mov     r_SS_SP,ecx
        lea     edi,r_regs
        int     31h

        ret
endp

getvbeinfoblock proc near
        ;<- eax -> VbeInfoBlock

                ;vbe-info holen
        mov     ax,100h                 ;allocate DOS mem
        mov     bx,1024/16;768/16
        int     31h                     ;AX = real mode segment address
        jc      @@err                   ;DX = protected mode selector for memory block

        mov     vbeinfoseg,ax
        mov     r_ES,ax                 ;ES:DI -> lo-mem-buffer
        mov     r_DI,0

        and     eax,0FFFFh
        shl     eax,4

        mov     [eax],'2EBV'            ;'VBE2'
        mov     r_AX,4F00h              ;VESA info
        push    eax
        call    int10h
        pop     eax
        cmp     r_AX,004Fh
        jne     @@v_err

        jmp     @@weg
@@err:
@@v_err:xor     eax,eax
@@weg:  ret
endp

getmodenum proc near
        ;-> eax = list_num
        ;<- eax = mode number
        movzx   ebx,vbeinfoseg
        shl     ebx,4
        shl     eax,1
        add     eax,[ebx].VideoModePtr

        mov     ebx,eax
        and     eax,0FFFFh
        shr     ebx,12
        and     ebx,0FFFF0h

        movsx   eax,word ptr[ebx + eax]
        ret
endp

getmodeinfoblock proc near
        ;-> eax = mode
        ;<- eax -> ModeInfoBlock

        mov     r_CX,ax                 ;CX = zu testender videomode

        movzx   eax,vbeinfoseg
        add     eax,512/16

        mov     r_ES,ax                 ;ES:DI -> lo-mem-buffer
        mov     r_DI,0

        mov     r_AX,4F01h              ;VESA mode info
        push    eax
        call    int10h
        pop     eax
        cmp     r_AX,004Fh
        jne     @@v_err

        shl     eax,4

        jmp     @@weg

@@v_err:xor     eax,eax
@@weg:  ret
endp

mapaddress proc near
        ;-> eax = PhysBasePtr
        ;-> edx = size
        ;<- eax = linear mem

        mov     cx,ax                   ;cx + bx*65536 = physical address
        shld    ebx,eax,16

        mov     di,dx                   ;di + si*65536 = size
        shld    esi,edx,16

        mov     ax,800h                 ;Physical Address Mapping
        int     31h
        mov     eax,0
        jc      @@err

        shrd    eax,ebx,16
        mov     ax,cx

@@err:  ret
endp

setmode proc near
        ;-> esi -> tvesa
        ;<- eax = result
        mov     eax,[esi].vesa_physbaseptr;original linear frame buffer address
        mov     linbuf,eax
        mov     eax,[esi].vesa_memsize
        shr     eax,2
        mov     memsize,eax

        mov     ebx,[esi].vesa_mode     ;VESA mode setzen
        cmp     bx,oldmode
        je      @@0
        mov     oldmode,bx

        or      bh,040h;0C0h
        mov     ax,4F02h
        int     10h
        cmp     ax,004Fh
        jne     @@err
@@0:                                    ;init successful
        call    clearlinbuf
        mov     eax,1
        jmp     @@weg

@@err:
        xor     eax,eax
;        mov     oldmode,ax
@@weg:  ret
endp

setpal proc near
        ;esi -> palette

        mov     cl,2 ;6 bit palette
        movzx   edi,vbeinfoseg
        mov     r_ES,di
        shl     edi,4
        mov     edx,255
@@l0:
        mov     al,[esi]
        shr     al,cl
        mov     [edi+2],al
        mov     al,[esi+1]
        shr     al,cl
        mov     [edi+1],al
        mov     al,[esi+2]
        shr     al,cl
        mov     [edi],al

        add     esi,3
        add     edi,4
        dec     edx
        jns     @@l0

        mov     r_BX,0
        mov     r_CX,256
        mov     r_DX,0
        mov     r_DI,0
        mov     r_AX,4F09h
        call    int10h
        ret
setpal endp


clearlinbuf proc near
        xor     eax,eax
        mov     ecx,memsize
        mov     edi,linbuf
        rep     stosd
        ret
clearlinbuf endp

textmode proc near
        cmp     oldmode,0
        je      @@weg
        mov     ax,3
        int     10h
        mov     oldmode,0
;        mov     ax,1112h
;        int     10h

        ;consume key
;        call    keypressed
;        jz      @@weg
;        mov     ah,07h
;        int     21h
;        or      al,al
;        jnz     @@weg
;        mov     ah,07h
;        int     21h

        mov     ah,1
        int     16h
        jz      @@weg
        xor     ah,ah
        int     16h

@@weg:  ret
endp

keypressed proc near
        push    ebx ecx edx esi edi
;  call grab
;  add maincount, 40
        mov     ah,1
        int     16h
        jz      @@0
        cmp     al,'s'
        jne     @@weg
        call    grab
        mov     ah,0
        int     16h
@@0:    xor     eax,eax
@@weg:  pop     edi esi edx ecx ebx
        ret
endp

mode13 proc near
        mov     ax,13h
        mov     oldmode,ax
        int     10h
        ret
mode13 endp


end