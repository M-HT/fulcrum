;/*
.486

.model flat


.data

CoLow				dd ?
CoHi				dd ?

.code


public GetTimeStampCounterPosASM
GetTimeStampCounterPosASM proc
	pushad

	; Now do the actual timing
	db      0fh,31h
	mov     CoLow,eax
	mov     CoHi,edx

	popad
	
	mov		eax,CoLow
	ret
GetTimeStampCounterPosASM endp 


	END


