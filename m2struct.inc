;/*
CMV2Camera struc
	CMV2Camera__m_Pos__m_fX					dd ?
	CMV2Camera__m_Pos__m_fY                 dd ?
	CMV2Camera__m_Pos__m_fZ                 dd ?

CMV2Camera__LabelFirstVector				label dword
	CMV2Camera__m_Right__m_fX               dd ?
	CMV2Camera__m_Right__m_fY               dd ?
	CMV2Camera__m_Right__m_fZ               dd ?

	CMV2Camera__m_Down__m_fX                dd ?
	CMV2Camera__m_Down__m_fY                dd ?
	CMV2Camera__m_Down__m_fZ                dd ?

	CMV2Camera__m_Front__m_fX               dd ?
	CMV2Camera__m_Front__m_fY               dd ?
	CMV2Camera__m_Front__m_fZ               dd ?

	CMV2Camera__fAngleF                     dd ?
	CMV2Camera__fAngleR                     dd ?
	CMV2Camera__fAngleD                     dd ?
	CMV2Camera__fAngleX                     dd ?
	CMV2Camera__fAngleY                     dd ?
	CMV2Camera__fAngleZ                     dd ?

	CMV2Camera__m_fScreenFactor             dd ?

	CMV2Camera__m_iXmax                     dd ?
	CMV2Camera__m_iYmax                     dd ?
	CMV2Camera__m_iClipXmin                 dd ?
	CMV2Camera__m_iClipYmin                 dd ?
	CMV2Camera__m_iClipXmax                 dd ?
	CMV2Camera__m_iClipYmax                 dd ?
ends


CMV2Particle struc
	CMV2Particle__m_LastPos__m_fX           dd ?
	CMV2Particle__m_LastPos__m_fY           dd ?
	CMV2Particle__m_LastPos__m_fZ           dd ?

	CMV2Particle__m_NextPos__m_fX           dd ?
	CMV2Particle__m_NextPos__m_fY           dd ?
	CMV2Particle__m_NextPos__m_fZ           dd ?

	CMV2Particle__m_InterpPos__m_fX         dd ?
	CMV2Particle__m_InterpPos__m_fY         dd ?
	CMV2Particle__m_InterpPos__m_fZ         dd ?

	CMV2Particle__m_StartPos__m_fX          dd ?
	CMV2Particle__m_StartPos__m_fY          dd ?
	CMV2Particle__m_StartPos__m_fZ          dd ?

	CMV2Particle__m_EndPos__m_fX          	dd ?
	CMV2Particle__m_EndPos__m_fY         	dd ?
	CMV2Particle__m_EndPos__m_fZ          	dd ?

	CMV2Particle__m_iDuration               dd ?
ends
CMV2Particle__Size                          equ 16*4



CMV2Dot3DPos struc
	CMV2Dot3DPos__m_Pos__m_fX               dd ?
	CMV2Dot3DPos__m_Pos__m_fY               dd ?
	CMV2Dot3DPos__m_Pos__m_fZ               dd ?
	CMV2Dot3DPos__m_TransfPos__m_fX			dd ?
	CMV2Dot3DPos__m_TransfPos__m_fY			dd ?
	CMV2Dot3DPos__m_TransfPos__m_fZ			dd ?
	CMV2Dot3DPos__m_fZNewRZP                dd ?
	CMV2Dot3DPos__m_iScreenX                dd ?
	CMV2Dot3DPos__m_iScreenY                dd ?
	CMV2Dot3DPos__m_fScreenX                dd ?
	CMV2Dot3DPos__m_fScreenY                dd ?
	CMV2Dot3DPos__m_fScreenYError           dd ?
	CMV2Dot3DPos__m_iClipFlag               dd ?
	CMV2Dot3DPos__m_fR						dd ?  ; = AngleN
	CMV2Dot3DPos__m_fG						dd ?  ; = AngleU
	CMV2Dot3DPos__m_fB						dd ?  ; = AngleV
	CMV2Dot3DPos__m_fAmbientR				dd ?
	CMV2Dot3DPos__m_fAmbientG				dd ?
	CMV2Dot3DPos__m_fAmbientB				dd ?
	CMV2Dot3DPos__m_Norm__m_fX              dd ?
	CMV2Dot3DPos__m_Norm__m_fY              dd ?
	CMV2Dot3DPos__m_Norm__m_fZ              dd ?
	CMV2Dot3DPos__m_AngleU__m_fX			dd ?
	CMV2Dot3DPos__m_AngleU__m_fY			dd ?
	CMV2Dot3DPos__m_AngleU__m_fZ			dd ?
	CMV2Dot3DPos__m_AngleV__m_fX			dd ?
	CMV2Dot3DPos__m_AngleV__m_fY			dd ?
	CMV2Dot3DPos__m_AngleV__m_fZ			dd ?
;	CMV2Dot3DPos__m_fAngleN					dd ?
;	CMV2Dot3DPos__m_fAngleU					dd ?
;	CMV2Dot3DPos__m_fAngleV					dd ?
ends
CMV2Dot3DPos__Size 							equ 28*4


CMV2Dot3D struc
	CMV2Dot3D__m_pPos                       dd ?
	CMV2Dot3D__m_fTextureU                  dd ?
	CMV2Dot3D__m_fTextureV                  dd ?
ends
CMV2Dot3D__Size                             equ 3*4


CMV2Polygon struc
	CMV2Polygon__m_pDot1                    dd ?
	CMV2Polygon__m_pDot2                    dd ?
	CMV2Polygon__m_pDot3                    dd ?
	CMV2Polygon__m_pcTexture                dd ?
	CMV2Polygon__m_iType					dd ?
	CMV2Polygon__m_iFlag					dd ?
	CMV2Polygon__m_pcBumpmap				dd ?
	CMV2Polygon__m_fAddDistance				dd ?
ends
CMV2Polygon__Size                           equ 8*4



CMV2ScanlinerPerspT struc
	CMV2ScanlinerPerspT__fTextureUDivZ      dd ?
	CMV2ScanlinerPerspT__fTextureVDivZ      dd ?
	CMV2ScanlinerPerspT__fOneDivZ           dd ?
	CMV2ScanlinerPerspT__dwScreenX1Offset   dd ?
	CMV2ScanlinerPerspT__dwXCounter         dd ?
ends
CMV2ScanlinerPerspT__size                   equ 5*4



CMV2ScanlinerLinT struc
	CMV2ScanlinerLinT__dwTextureU        	dd ?
	CMV2ScanlinerLinT__dwTextureV        	dd ?
	CMV2ScanlinerLinT__dwScreenX1Offset     dd ?
	CMV2ScanlinerLinT__dwXCounter           dd ?
ends
CMV2ScanlinerLinT__size                   	equ 4*4



CMV2ScanlinerLinTG struc
	CMV2ScanlinerLinTG__dwTextureU        	dd ?
	CMV2ScanlinerLinTG__dwTextureV        	dd ?
	CMV2ScanlinerLinTG__dwScreenX1Offset    dd ?
	CMV2ScanlinerLinTG__dwXCounter          dd ?
	CMV2ScanlinerLinTG__dwR					dd ?
	CMV2ScanlinerLinTG__dwG					dd ?
	CMV2ScanlinerLinTG__dwB					dd ?
	CMV2ScanlinerLinTG__fScreenXError		dd ?
ends
CMV2ScanlinerLinTG__size                   	equ 8*4



CMV2ScanlinerLinTB struc
	CMV2ScanlinerLinTB__dwTextureU        	dd ?
	CMV2ScanlinerLinTB__dwTextureV        	dd ?
	CMV2ScanlinerLinTB__dwScreenX1Offset    dd ?
	CMV2ScanlinerLinTB__dwXCounter          dd ?
	CMV2ScanlinerLinTB__dwAngleN			dd ?
	CMV2ScanlinerLinTB__dwAngleU			dd ?
	CMV2ScanlinerLinTB__dwAngleV			dd ?
	CMV2ScanlinerLinTB__fScreenXError		dd ?
ends
CMV2ScanlinerLinTB__size                   	equ 8*4




CMV2ScanlinerPerspTG struc
	CMV2ScanlinerPerspTG__fTextureUDivZ		dd ?
	CMV2ScanlinerPerspTG__fTextureVDivZ		dd ?
	CMV2ScanlinerPerspTG__fOneDivZ			dd ?
	CMV2ScanlinerPerspTG__dwScreenX1Offset	dd ?
	CMV2ScanlinerPerspTG__dwXCounter		dd ?
	CMV2ScanlinerPerspTG__dwR				dd ?
	CMV2ScanlinerPerspTG__dwG				dd ?
	CMV2ScanlinerPerspTG__dwB				dd ?
	CMV2ScanlinerPerspTG__fScreenXError		dd ?
ends
CMV2ScanlinerPerspTG__size					equ 9*4




CMV2ScanlinerPerspTB struc
	CMV2ScanlinerPerspTB__fTextureUDivZ		dd ?
	CMV2ScanlinerPerspTB__fTextureVDivZ		dd ?
	CMV2ScanlinerPerspTB__fAngleUDivZ		dd ?
	CMV2ScanlinerPerspTB__fAngleVDivZ		dd ?
	CMV2ScanlinerPerspTB__fOneDivZ			dd ?
	CMV2ScanlinerPerspTB__dwScreenX1Offset	dd ?
	CMV2ScanlinerPerspTB__dwXCounter		dd ?
	CMV2ScanlinerPerspTB__fScreenXError		dd ?
ends
CMV2ScanlinerPerspTB__size					equ 8*4







CMV2PolygonDistance struc
	CMV2PolygonDistance__m_fAverageDistance dd ?
	CMV2PolygonDistance__m_pPolygon         dd ?
ends



CMV2PolygonObject struc
	CMV2PolygonObject__m_pPolygons          dd ?
	CMV2PolygonObject__m_ppPolygons         dd ?
	CMV2PolygonObject__m_iNumPolygons       dd ?
	CMV2PolygonObject__m_pDot3DPos          dd ?
	CMV2PolygonObject__m_ppDot3DPos         dd ?
	CMV2PolygonObject__m_iNumDot3DPos       dd ?
	CMV2PolygonObject__m_pDot3D             dd ?
	CMV2PolygonObject__m_iNumDot3D          dd ?
	CMV2PolygonObject__m_iFlagNormCalc		dd ?
ends
CMV2PolygonObject__size                     equ 10*4


