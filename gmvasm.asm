;/*
.486

.model flat

LOCALS

.data
		public lScreenCopyCounter


lScreenCopyCounter	dd ?

align 4
RegStruct:
RegStruct_edi		label	dword
RegStruct_di		dw	?,?
RegStruct_esi		label	dword
RegStruct_si		dw	?,?
RegStruct_ebp		label	dword
RegStruct_bp		dw	?,?
			dd	?
RegStruct_ebx		label	dword
RegStruct_bx		label	word
RegStruct_bl		db	?
RegStruct_bh		db	?,?,?
RegStruct_edx		label	dword
RegStruct_dx		label	word
RegStruct_dl		db	?
RegStruct_dh		db	?,?,?
RegStruct_ecx		label	dword
RegStruct_cx		label	word
RegStruct_cl		db	?
RegStruct_ch		db	?,?,?
RegStruct_eax		label	dword
RegStruct_ax		label	word
RegStruct_al		db	?
RegStruct_ah		db	?,?,?
RegStruct_flags 	dw	?
RegStruct_es		dw	?
RegStruct_ds		dw	?
RegStruct_fs		dw	?
RegStruct_gs		dw	?
RegStruct_ip		dw	?
RegStruct_cs		dw	?
RegStruct_sp		dw	?
RegStruct_ss		dw	?
RegStruct_end:

RealModeSegment 	dw ?
ProtectedModeSelector	dw ?

SupportedMode		dd ?

TextOn			db 1	    ; annahme: programm wird im text modus
				    ; gestartet.
TextMode		db ?

NumBytes		dd ?

pMappedLfb		dd ?


pGfxModeVesa		dd ?
pVesaMode		dd ?
pScreenBuffer		dd ?


lTemp			dd ?



.code




CVesaModeInfo struc
    CVesaModeInfo__m_wModeAttributes	    dw ?
    CVesaModeInfo__m_bWindowAAttributes     db ?
    CVesaModeInfo__m_bWindowBAttributes     db ?
    CVesaModeInfo__m_wWindowGranularity     dw ?
    CVesaModeInfo__m_wWindowSize	    dw ?
    CVesaModeInfo__m_wSegmentWindowA	    dw ?
    CVesaModeInfo__m_wSegmentWindowB	    dw ?

    CVesaModeInfo__m_wWindowPosFuncPtr	    dd ?
    CVesaModeInfo__m_wBytesPerScanline	    dw ?

    CVesaModeInfo__m_wXmax		    dw ?
    CVesaModeInfo__m_wYmax		    dw ?

    CVesaModeInfo__m_bTextXmax		    db ?
    CVesaModeInfo__m_bTextYmax		    db ?

    CVesaModeInfo__m_bNumMemoryPlanes	    db ?
    CVesaModeInfo__m_bBitsPerPixel	    db ?
    CVesaModeInfo__m_bNumBanks		    db ?
    CVesaModeInfo__m_bMemoryModelType	    db ?
    CVesaModeInfo__m_bSizeOfBank	    db ?
    CVesaModeInfo__m_bNumImagePages	    db ?
    CVesaModeInfo__m_bReserved1 	    db ?

    CVesaModeInfo__m_bRedMaskSize	    db ?
    CVesaModeInfo__m_bRedFieldPos	    db ?
    CVesaModeInfo__m_bGreenMaskSize	    db ?
    CVesaModeInfo__m_bGreenFieldPos	    db ?
    CVesaModeInfo__m_bBlueMaskSize	    db ?
    CVesaModeInfo__m_bBlueFieldPos	    db ?
    CVesaModeInfo__m_bReservedMaskSize	    db ?
    CVesaModeInfo__m_bReservedFieldPos	    db ?
    CVesaModeInfo__m_bDirectColModeInfo     db ?

    CVesaModeInfo__m_lPhysicalAddressLfb    dd ?
    CVesaModeInfo__m_lPtrOffscreenMem	    dd ?
    CVesaModeInfo__m_wOffscreenMemSize	    dw ?

    CVesaModeInfo__m_pReserved2 	    dd ?
ends





CGfxModeVesa struc
    CGfxModeVesa__m_bVesaSupported	    db ?
    CGfxModeVesa__m_fVesaVersion	    dd ?
    CGfxModeVesa__m_lCurrentModeLocalId     dd ?
    CGfxModeVesa__m_pLfb		    dd ?
    CGfxModeVesa__m_pMappedLfb		    dd ?
    CGfxModeVesa__m_lXmax		    dd ?
    CGfxModeVesa__m_lYmax		    dd ?
    CGfxModeVesa__m_lClippingXmin	    dd ?
    CGfxModeVesa__m_lClippingXmax	    dd ?
    CGfxModeVesa__m_lClippingYmin	    dd ?
    CGfxModeVesa__m_lClippingYmax	    dd ?
;   ...
;   ...
;   ...
ends


CVesaMode struc
    CVesaMode__m_wId			    dw ?
    CVesaMode__m_wXmax			    dw ?
    CVesaMode__m_wYmax			    dw ?
    CVesaMode__m_wBytesPerScanline	    dw ?
    CVesaMode__m_bBitsPerPixel		    db ?
    CVesaMode__m_bBytesPerPixel 	    db ?
    CVesaMode__m_wLocalId		    dw ?
    CVesaMode__m_bLfbSupported		    db ?
ends




public VesaGetSVgaInfoASM
VesaGetSVgaInfoASM proc
    arg AllocatedMemory:dword	    ; size of allocated memroy should be
				    ; at least 256 byte
    push    ebp
    mov     ebp,esp
    pushad

    mov     eax,0100h		    ; function: allocate dos memory
    mov     ebx,1024
    shr     ebx,2
    int     31h 		    ; out:
    jc	    @@error
				    ; ax: real mode segmet
				    ; dx: real mode offset
    mov     RealModeSegment,ax
    mov     ProtectedModeSelector,dx

    push    ds
    pop     es
    mov     ecx,offset RegStruct_end
    mov     edi,offset RegStruct
    sub     ecx,edi

    xor     al,al
    cld
    rep     stosb		    ; clear the realmode register
				    ; structure
    mov     RegStruct_eax,00004f00h
    mov     ax,RealModeSegment
    mov     RegStruct_es,ax
    mov     RegStruct_ds,ax

    mov     eax,0300h		    ; function: simulate real mode
				    ; interrupt
    mov     ebx,0010h		    ; real mode interrupt number
    xor     ecx,ecx
    push    ds
    pop     es
    mov     edi,offset RegStruct
    int     31h

    push    ds
    push    ds
    pop     es
    mov     edi,AllocatedMemory
    mov     ds,ProtectedModeSelector
    xor     esi,esi
    mov     ecx,256/4
    cld
    rep     movsd
    pop     ds

    mov     eax,0101h		    ; function: free dos memory
    mov     dx,ProtectedModeSelector
    int     31h
    jc	    @@error

    jmp     @@ende
@@error:
@@ende:

    popad
    pop     ebp

    ret
endp VesaGetSVgaInfoASM









public VesaGetModeInfoASM
VesaGetModeInfoASM proc
    arg Mode :dword, AllocatedMemory:dword
				    ; size of allocated memroy should be
				    ; at least 256 byte
    push    ebp
    mov     ebp,esp
    pushad

    mov     eax,0100h		    ; function: allocate dos memory
    mov     ebx,1024
    shr     ebx,2

    int     31h 		    ; out:
    jc	    @@error
				    ; ax: real mode segmet
				    ; dx: real mode offset
    mov     RealModeSegment,ax
    mov     ProtectedModeSelector,dx

    push    ds
    pop     es
    mov     ecx,offset RegStruct_end
    mov     edi,offset RegStruct
    sub     ecx,edi
    xor     al,al
    cld
    rep     stosb		    ; clear the realmode register
				    ; structure

    mov     RegStruct_eax,00004f01h
    mov     eax,Mode
    mov     RegStruct_ecx,eax
    mov     ax,RealModeSegment
    mov     RegStruct_es,ax
    mov     RegStruct_ds,ax

    mov     eax,0300h		    ; function: simulate real mode
				    ; interrupt
    mov     ebx,0010h		    ; real mode interrupt number
    xor     ecx,ecx
    push    ds
    pop     es
    mov     edi,offset RegStruct
    int     31h

    push    ds
    push    ds
    pop     es
    mov     edi,AllocatedMemory
    mov     ds,ProtectedModeSelector
    xor     esi,esi
    mov     ecx,256/4
    cld
    rep     movsd		    ; copy to linear framebuffer
    pop     ds

    mov     eax,0101h		    ; function: free dos memory
    mov     dx,ProtectedModeSelector
    int     31h
    jc	    @@error

    mov     SupportedMode,1

    jmp     @@ende
@@error:

    mov     SupportedMode,0
@@ende:
    popad
    pop     ebp

    mov     eax,SupportedMode
    ret
endp VesaGetModeInfoASM






public VesaGraphicsOnASM
VesaGraphicsOnASM proc
    arg     Mode:dword

    push    ebp
    mov     ebp,esp
    pushad

    mov     al,TextOn
    cmp     al,0
    je	    @@not_done

    mov     eax,00000f00h
    int     10h
    mov     TextMode,al
    xor     eax,eax
    mov     TextOn,al

    mov     ax,4f02h
    mov     ebx,Mode
    int     10h

    cmp     ax,0004ch
    je	    @@ok
    mov     lTemp,1
    jmp     @@weiter

@@ok:
    mov     lTemp,0

@@weiter:
@@not_done:
    popad
    pop     ebp

    mov     eax,lTemp

    ret
endp VesaGraphicsOnASM




public VesaGraphicsOffASM
VesaGraphicsOffASM proc

    pushad
    mov     al,TextOn
    cmp     al,0
    jne     @@end
    xor     eax,eax
    mov     al,TextMode
    int     10h
    mov     TextOn,1

@@end:
    popad
    ret
endp VesaGraphicsOffASM





public CopyBufferToScreenASM
CopyBufferToScreenASM proc
    arg _pGfxModeVesa:DWORD, _pVesaMode:DWORD, _pScreenBuffer:DWORD
    push    ebp
    mov     ebp,esp
    pushad

    mov     eax,_pGfxModeVesa
    mov     ebx,_pVesaMode
    mov     ecx,_pScreenBuffer
    mov     pGfxModeVesa,eax
    mov     pVesaMode,ebx
    mov     pScreenBuffer,ecx

    mov     ebp,pVesaMode

    push    ds
    pop     es

    movzx   eax,ds:[ebp].CVesaMode__m_wXmax
    movzx   ebx,ds:[ebp].CVesaMode__m_wYmax
    mul     ebx
    xor     ebx,ebx
    mov     bl,ds:[ebp].CVesaMode__m_bBytesPerPixel
    mul     ebx
    mov     NumBytes,eax

    mov     ebp,pVesaMode
    mov     al,[ebp].CVesaMode__m_bLfbSupported
    cmp     al,0
    jne     @@copy_to_lfb

    mov     esi,pScreenBuffer
    xor     edx,edx

    mov     ebx,NumBytes
    cmp     ebx,65536
    jbe     @@mode13

@@page_loop:
    mov     ebx,NumBytes
    cmp     ebx,65536
    jbe     @@last_page

    mov     ecx,16384
    sub     ebx,65536
    mov     NumBytes,ebx
    jmp     @@not_last_page

@@last_page:
    mov     ecx,ebx
    shr     ecx,2
    xor     ebx,ebx
    mov     NumBytes,ebx

@@not_last_page:
    mov     eax,04f05h
    xor     ebx,ebx
    int     10h

    mov     edi,0a0000h
    cld
    rep     movsd

    inc     edx 		; increment page number
    cmp     NumBytes,0
    jnz     @@page_loop
    jmp     @@finished
@@mode13:
    mov     ecx,16000
    mov     edi,0a0000h
    cld
    rep     movsd
    jmp     @@finished

@@copy_to_lfb:
    mov     ebp,pGfxModeVesa

    push    es
    push    ds
    pop     es
    mov     ecx,NumBytes
    shr     ecx,2
    mov     esi,pScreenBuffer
    mov     edi,[ebp].CGfxModeVesa__m_pMappedLfb
    cld
    rep     movsd
comment &
@@motionblur:
    mov     eax,ds:[edi]
    mov     ebx,ds:[esi]

    and     eax,000fefefeh
    and     ebx,000fefefeh
    shr     eax,1
    shr     ebx,1
    add     eax,ebx
    mov     ds:[edi],eax

    add     edi,4
    add     esi,4

    dec     ecx
    jnz     @@motionblur
&

    pop     es
@@finished:

    popad
    pop     ebp
    ret
endp CopyBufferToScreenASM








public CopyBufferToScreenMode13ASM
CopyBufferToScreenMode13ASM proc
    arg _pScreenBuffer :dword

    push    ebp
    mov     ebp,esp

    pushad

    push    ds
    pop     es

    mov     esi,_pScreenBuffer
    mov     ecx,16000

    mov     edi,0a0000h
    cld
    rep     movsd

    popad
    pop     ebp
    ret
endp CopyBufferToScreenMode13ASM






public GetLfbMappedPtrASM
GetLfbMappedPtrASM proc
    arg pLfb:dword

    push    ebp
    mov     ebp,esp

    pushad

    mov     ax,0800h		; function: physical pddress mapping
    mov     ecx,pLfb
    shld    ebx,ecx,16		; linear addres that can be used
				; to access the physical memory
    mov     edi,4096*1024
    shld    esi,edi,16

    int     31h
    jc	    @@error

    shl     ebx,16
    mov     bx,cx
    mov     pMappedLfb,ebx
    jmp     @@end

@@error:
    mov     eax,0ffffffffh
    mov     pMappedLfb,eax

@@end:
    popad
    pop     ebp

    mov     eax,pMappedLfb

    ret
endp GetLfbMappedPtrASM





public SetPaletteASM
SetPaletteASM proc
    arg Palette:dword, Delta:byte

    push    ebp
    mov     ebp,esp
    pushad

    mov     edi,Palette
    mov     bx,0
    mov     cx,256

@@next_col:
    mov     dx,3c8h
    mov     al,bl
    out     dx,al
    inc     dx

    mov     al,[es:edi]
    shr     al,2
    add     al,Delta

    cmp     al,0
    jns     @@red_positiv

    mov     al,0
    jmp     @@red_ok

@@red_positiv:
    cmp     al,63
    jbe     @@red_ok
    mov     al,63

@@red_ok:
    out     dx,al		;red
    mov     al,[es:edi + 1]
    shr     al,2
    add     al,Delta
    cmp     al,0
    jns     @@green_positiv

    mov     al,0
    jmp     @@green_ok

@@green_positiv:
    cmp     al,63
    jbe     @@green_ok
    mov     al,63

@@green_ok:
    out     dx,al		;green
    mov     al,[es:edi + 2]
    shr     al,2
    add     al,Delta
    cmp     al,0
    jns     @@blue_positiv

    mov     al,0
    jmp     @@blue_ok

@@blue_positiv:
    cmp     al,63
    jbe     @@blue_ok
    mov     al,63

@@blue_ok:
    out     dx,al		;blue
    inc     bx
    add     edi,3
    dec     cx
    jnz     @@next_col

    popad
    pop ebp
    ret
endp SetPaletteASM




public InitZeroMem
InitZeroMem proc
    ARG pMem:DWORD, lNum:DWORD

    push    ebp
    mov     ebp,esp
    pushad

    mov     edi,pMem
    mov     ax,ds
    mov     es,ax
    cld
    mov     ecx,lNum
    shr     ecx,2
    xor     eax,eax
;   mov     eax,01010101h

    cld
    rep     stosd

    popad
    pop     ebp

    ret
endp InitZeroMem




public SetPixelMem
SetPixelMem proc
    ARG lX:DWORD, lY:DWORD, pMem:DWORD, lXmax:DWORD, lCol:DWORD
    push    ebp
    mov     ebp,esp
    pushad

    mov     eax,lY
    imul    eax,lXmax
    add     eax,lX
    shl     eax,2
    add     eax,pMem
    mov     ebx,lCol

    mov     ds:[eax],ebx

comment &
    mov     ecx,ds:[eax]
    and     ebx,0ffffffh
    and     ecx,0ffffffh
    add     ecx,ebx
    cmp     ecx,0ffffffh
    jbe     @@ok
    mov     ecx,0ffffffh
@@ok:
    mov     ds:[eax],ecx
&
    popad
    pop     ebp

    ret
endp SetPixelMem




public TransformTo16Bit
TransformTo16Bit proc
    arg SrcScreenBuffer:dword, DestScreenBuffer:dword, Palette:dword, NumPixels:dword

    push    ebp
    mov     ebp,esp
    pushad

    mov     eax,NumPixels

    mov     esi,SrcScreenBuffer
    mov     edi,DestScreenBuffer
    mov     edx,Palette

    xor     ebx,ebx
@@loop:
    mov     bl,ds:[esi]

    lea     ecx,ds:[ebx + 2*ebx]	; ecx = 3*ebx
    mov     ecx,ds:[edx + ecx]		; ecx = r, g, b

    push    eax
    push    ebx

    mov     eax,ecx
    mov     ebx,ecx

    and     eax,0000000f8h
    and     ebx,00000f800h
    and     ecx,000f80000h

    shr     eax,3
    shr     ebx,3 + 3
    shr     ecx,3 + 6

    shl     eax,10
    shr     ecx,10;r <-> b

    or	    ecx,eax
    or	    ecx,ebx

    mov     ds:[edi],cx

    pop     ebx
    pop     eax

    add     edi,2
    inc     esi
    dec     eax
    jnz     @@loop

    popad
    pop     ebp
    ret
endp TransformTo16Bit








public TransformTo32Bit
TransformTo32Bit proc
	arg SrcScreenBuffer:dword, DestScreenBuffer:dword, Palette:dword, NumPixels:dword

	push    ebp
	mov ebp,esp
	pushad

	mov eax,NumPixels

	mov esi,SrcScreenBuffer
	mov edi,DestScreenBuffer
	mov edx,Palette

	xor ebx,ebx
@@loop:
	mov bl,ds:[esi]

	lea ecx,ds:[ebx + 2*ebx]	    ; ecx = 3*ebx
	mov ecx,ds:[edx + ecx]	    ; ecx = r, g, b

	mov ds:[edi + 2],cl
	mov ds:[edi + 1],ch
	shr ecx,8
	mov ds:[edi],ch
	mov byte ptr ds:[edi + 3],0


	add edi,4
	inc esi

	dec eax
	jnz @@loop


    popad
    pop ebp
    ret
endp TransformTo32Bit



public ScaleTo16Bit
ScaleTo16Bit proc
    arg SrcScreenBuffer:dword, DestScreenBuffer:dword, NumPixels:dword

    push    ebp
    mov     ebp,esp
    pushad

    mov     eax,NumPixels

    mov     esi,SrcScreenBuffer
    mov     edi,DestScreenBuffer
    mov     edx,Palette

    xor     ebx,ebx
@@loop:
    mov     bh,ds:[esi]
    mov     ds:[edi],bx

    add     edi,2
    inc     esi
    dec     eax
    jnz     @@loop

    popad
    pop     ebp
    ret
endp ScaleTo16Bit




public Transform24To16Bit
Transform24To16Bit proc
    arg SrcScreenBuffer:dword, DestScreenBuffer:dword, NumPixels:dword

    push    ebp
    mov     ebp,esp
    pushad

    mov     eax,NumPixels

    mov     esi,SrcScreenBuffer
    mov     edi,DestScreenBuffer

    xor     ebx,ebx
@@loop:
    mov     ebx,ds:[esi]
    mov     ecx,ebx
    mov     edx,ebx

    and     ebx,0000000f8h
    and     ecx,00000f800h
    and     edx,000f80000h

    shr     ebx,3
    shr     ecx,3 + 3
    shr     edx,3 + 6

    shl     ebx,10
    shr     edx,10;r <-> b

    or	    edx,ebx
    or	    edx,ecx

    mov     ds:[edi],dx

    add     edi,2
    add     esi,3
    dec     eax
    jnz     @@loop


    popad
    pop ebp
    ret
endp Transform24To16Bit




include vesa.inc

;------!!!
tcoltab16 struc
  ct_r16 dw 256 dup(?)
  ct_g16 dw 256 dup(?)
  ct_b16 dw 256 dup(?)
ends

tcoltab32 struc
  ct_r32 dd 256 dup(?)
  ct_g32 dd 256 dup(?)
  ct_b32 dd 256 dup(?)
ends

public makecoltab16
makecoltab16 proc near
	;-> esi -> tvesa
	;-> edi -> coltab

	mov	edx,255
@@l:
	mov	eax,edx
	mov	cl,[esi].vesa_redbits
	shl	eax,cl
	shr	eax,8
	mov	cl,[esi].vesa_redpos
	shl	eax,cl
	mov	[edi].ct_r16[edx*2],ax

	mov	eax,edx
	mov	cl,[esi].vesa_greenbits
	shl	eax,cl
	shr	eax,8
	mov	cl,[esi].vesa_greenpos
	shl	eax,cl
	mov	[edi].ct_g16[edx*2],ax

	mov	eax,edx
	mov	cl,[esi].vesa_bluebits
	shl	eax,cl
	shr	eax,8
	mov	cl,[esi].vesa_bluepos
	shl	eax,cl
	mov	[edi].ct_b16[edx*2],ax

	dec	edx
	jns	@@l

	ret
makecoltab16 endp

public makecoltab32
makecoltab32 proc near
	;-> esi -> tvesa
	;-> edi -> coltab

	mov	edx,255
@@l:
	mov	eax,edx
	mov	cl,[esi].vesa_redbits
	shl	eax,cl
	shr	eax,8
	mov	cl,[esi].vesa_redpos
	shl	eax,cl
	mov	[edi].ct_r32[edx*4],eax

	mov	eax,edx
	mov	cl,[esi].vesa_greenbits
	shl	eax,cl
	shr	eax,8
	mov	cl,[esi].vesa_greenpos
	shl	eax,cl
	mov	[edi].ct_g32[edx*4],eax

	mov	eax,edx
	mov	cl,[esi].vesa_bluebits
	shl	eax,cl
	shr	eax,8
	mov	cl,[esi].vesa_bluepos
	shl	eax,cl
	mov	[edi].ct_b32[edx*4],eax

	dec	edx
	jns	@@l

	ret
makecoltab32 endp


public CopyBufferASM
CopyBufferASM proc near
	;-> esi -> source-screenbuffer
	;-> ebx -> tvesa
	;-> edx -> coltab

	push	ebp

	mov	edi,ds:[ebx].vesa_linbuf

	or	edx,edx
	jz	@@direct

	cmp	[ebx].vesa_pbytes,3
	ja	@@32
	je	@@24
	call	copybuffer16
	jmp	@@weg
@@24:	call	copybuffer24
	jmp	@@weg
@@32:	call	copybuffer32
	jmp	@@weg

@@direct:
	cmp	[ebx].vesa_pbytes,3
	ja	@@d32
	call	copydirect24
	jmp	@@weg
@@d32:
	call	copydirect32

@@weg:	pop	ebp
	ret
CopyBufferASM endp

copybuffer16 proc near
	;-> esi -> source-screenbuffer
	;-> edi -> dest-buffer
	;-> ebx -> tvesa
	;-> edx -> coltab

	mov	ecx,ds:[ebx].vesa_yres
	xor	eax,eax
@@YLoop:
	push	ecx
	push	edi

	mov	ecx,ds:[ebx].vesa_xres
	shr	ecx,1
	push	ebx
@@XLoop:
	mov	al,[esi+4]
	mov	bx,[edx].ct_b16[eax*2]
	mov	al,[esi+5]
	or	bx,[edx].ct_g16[eax*2]
	mov	al,[esi+6]
	or	bx,[edx].ct_r16[eax*2]

	shl	ebx,16

	mov	al,[esi]
	mov	bx,[edx].ct_b16[eax*2]
	mov	al,[esi+1]
	or	bx,[edx].ct_g16[eax*2]
	mov	al,[esi+2]
	add	esi,8
	or	bx,[edx].ct_r16[eax*2]

	mov	[edi],ebx
	add	edi,4

	dec	ecx
	jnz	@@XLoop

	pop	ebx
	pop	edi
	add	edi,ds:[ebx].vesa_xbytes

	pop	ecx
	dec	ecx
	jnz	@@YLoop

	ret
copybuffer16 endp

copybuffer24 proc near
	;-> esi -> source-screenbuffer
	;-> edi -> dest-buffer
	;-> ebx -> tvesa
	;-> edx -> coltab

	mov	ecx,ds:[ebx].vesa_yres
	xor	eax,eax
@@YLoop:
	push	ecx
	push	edi

	mov	ecx,ds:[ebx].vesa_xres
	shr	ecx,2
	push	ebx
@@XLoop:

		;1.
	mov	al,[esi]
	mov	ebp,[edx].ct_b32[eax*4]
	mov	al,[esi+1]
	or	ebp,[edx].ct_g32[eax*4]
	mov	al,[esi+2]
	add	esi,4
	or	ebp,[edx].ct_r32[eax*4]

		;2.
      shl     ebp,8
	mov	al,[esi]
	mov	ebx,[edx].ct_b32[eax*4]
	mov	al,[esi+1]
	or	ebx,[edx].ct_g32[eax*4]
	mov	al,[esi+2]
	add	esi,4
	or	ebx,[edx].ct_r32[eax*4]

		;3.
      shrd    ebp,ebx,8
      mov     [edi],ebp
      add     edi,4
	mov	al,[esi]
	mov	ebp,[edx].ct_b32[eax*4]
	mov	al,[esi+1]
	or	ebp,[edx].ct_g32[eax*4]
	mov	al,[esi+2]
	add	esi,4
	or	ebp,[edx].ct_r32[eax*4]
      shl     ebx,8

		;4.
      shrd    ebx,ebp,16
      mov     [edi],ebx
      add     edi,4
      shl     ebp,8
	mov	al,[esi]
	mov	ebx,[edx].ct_b32[eax*4]
	mov	al,[esi+1]
	or	ebx,[edx].ct_g32[eax*4]
	mov	al,[esi+2]
	add	esi,4
	or	ebx,[edx].ct_r32[eax*4]

      shld    ebx,ebp,8
      mov     [edi],ebx
      add     edi,4

	dec	ecx
	jnz	@@XLoop

	pop	ebx
	pop	edi
	add	edi,ds:[ebx].vesa_xbytes

	pop	ecx
	dec	ecx
	jnz	@@YLoop

	ret
copybuffer24 endp

copybuffer32 proc near
	;-> esi -> source-screenbuffer
	;-> edi -> dest-buffer
	;-> ebx -> tvesa
	;-> edx -> coltab

	mov	ecx,ds:[ebx].vesa_yres
	xor	eax,eax
@@YLoop:
	push	ecx
	push	edi

	mov	ecx,ds:[ebx].vesa_xres
	push	ebx
@@XLoop:
	mov	al,[esi]
	mov	ebx,[edx].ct_b32[eax*4]
	mov	al,[esi+1]
	or	ebx,[edx].ct_g32[eax*4]
	mov	al,[esi+2]
	add	esi,4
	or	ebx,[edx].ct_r32[eax*4]

	mov	[edi],ebx
	add	edi,4

	dec	ecx
	jnz	@@XLoop

	pop	ebx
	pop	edi
	add	edi,ds:[ebx].vesa_xbytes

	pop	ecx
	dec	ecx
	jnz	@@YLoop

	ret
copybuffer32 endp

copydirect24 proc near
	;-> esi -> source-screenbuffer
	;-> edi -> dest-buffer
	;-> ebx -> tvesa

	mov	eax,ds:[ebx].vesa_yres

@@YLoop:
	push	edi

	mov	ecx,ds:[ebx].vesa_xres
	shr	ecx,2
@@XLoop:

		;1.
	mov	ebp,[esi]
	add	esi,4

		;2.
      shl     ebp,8
	mov	edx,[esi]
	add	esi,4

		;3.
      shrd    ebp,edx,8
      mov     [edi],ebp
      add     edi,4
	mov	ebp,[esi]
	add	esi,4
      shl     edx,8

		;4.
      shrd    edx,ebp,16
      mov     [edi],edx
      add     edi,4
      shl     ebp,8
	mov	edx,[esi]
	add	esi,4

      shld    edx,ebp,8
      mov     [edi],edx
      add     edi,4

	dec	ecx
	jnz	@@XLoop

	pop	edi
	add	edi,ds:[ebx].vesa_xbytes

	dec	eax
	jnz	@@YLoop

	ret
copydirect24 endp

copydirect32 proc near
	;-> esi -> source-screenbuffer
	;-> edi -> dest-buffer
	;-> ebx -> tvesa

	mov	edx,ds:[ebx].vesa_yres

@@YLoop:
	push	edi

	mov	ecx,ds:[ebx].vesa_xres
	cld
	rep	movsd

	pop	edi
	add	edi,ds:[ebx].vesa_xbytes

	dec	edx
	jnz	@@YLoop

	ret
copydirect32 endp





public TransformToBumpmapASM
TransformToBumpmapASM proc
    arg pSrc:dword, pDest:dword, iNum:DWORD
    push    ebp
    mov     ebp,esp
    pushad

    mov     esi,pSrc
    mov     edi,pDest
    mov     ebp,iNum

    xor     ecx,ecx
    xor     edx,edx

@@Loop:

    xor     ebx,ebx
    inc     dl
    mov     bl,ds:[esi + edx]
    dec     dl
    mov     cl,ds:[esi + edx]
    sub     ebx,ecx
    add     ebx,128

    cmp     ebx,0
    jns     @@Ok1
    mov     ebx,0
@@Ok1:

    cmp     ebx,255
    jb	    @@Ok2
    mov     ebx,255
@@Ok2:

    mov     ds:[edi],bl
    inc     edi

    xor     ebx,ebx
    inc     dh
    mov     bl,ds:[esi + edx]
    dec     dh
    mov     cl,ds:[esi + edx]
    sub     ebx,ecx
    add     ebx,128

    cmp     ebx,0
    jns     @@Ok3
    mov     ebx,0
@@Ok3:

    cmp     ebx,255
    jb	    @@Ok4
    mov     ebx,255
@@Ok4:

    mov     ds:[edi],bl
    inc     edi
    inc     edx

    dec     ebp
    jnz     @@Loop

    popad
    pop     ebp
    ret
endp TransformToBumpmapASM


    END

;*/

