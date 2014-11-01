//;/*
typedef struct {
	float CMV2Camera__m_Pos__m_fX;
	float CMV2Camera__m_Pos__m_fY;
	float CMV2Camera__m_Pos__m_fZ;

	union {
		float CMV2Camera__LabelFirstVector_data[3*3];
#ifdef M2STRUCT_HAS_TMATRIX
		tmatrix CMV2Camera__LabelFirstVector;
#endif
		struct {
			float CMV2Camera__m_Right__m_fX;
			float CMV2Camera__m_Right__m_fY;
			float CMV2Camera__m_Right__m_fZ;

			float CMV2Camera__m_Down__m_fX;
			float CMV2Camera__m_Down__m_fY;
			float CMV2Camera__m_Down__m_fZ;

			float CMV2Camera__m_Front__m_fX;
			float CMV2Camera__m_Front__m_fY;
			float CMV2Camera__m_Front__m_fZ;
		};
	};

	float CMV2Camera__fAngleF;
	float CMV2Camera__fAngleR;
	float CMV2Camera__fAngleD;
	float CMV2Camera__fAngleX;
	float CMV2Camera__fAngleY;
	float CMV2Camera__fAngleZ;

	float CMV2Camera__m_fScreenFactor;

	uint32_t CMV2Camera__m_iXmax;
	uint32_t CMV2Camera__m_iYmax;
	uint32_t CMV2Camera__m_iClipXmin;
	uint32_t CMV2Camera__m_iClipYmin;
	uint32_t CMV2Camera__m_iClipXmax;
	uint32_t CMV2Camera__m_iClipYmax;
} CMV2Camera;


typedef struct {
	float CMV2Particle__m_LastPos__m_fX;
	float CMV2Particle__m_LastPos__m_fY;
	float CMV2Particle__m_LastPos__m_fZ;

	float CMV2Particle__m_NextPos__m_fX;
	float CMV2Particle__m_NextPos__m_fY;
	float CMV2Particle__m_NextPos__m_fZ;

	float CMV2Particle__m_InterpPos__m_fX;
	float CMV2Particle__m_InterpPos__m_fY;
	float CMV2Particle__m_InterpPos__m_fZ;

	float CMV2Particle__m_StartPos__m_fX;
	float CMV2Particle__m_StartPos__m_fY;
	float CMV2Particle__m_StartPos__m_fZ;

	float CMV2Particle__m_EndPos__m_fX;
	float CMV2Particle__m_EndPos__m_fY;
	float CMV2Particle__m_EndPos__m_fZ;

	uint32_t CMV2Particle__m_iDuration;
} CMV2Particle;
#define CMV2Particle__Size                          (16*4)



typedef struct {
	float CMV2Dot3DPos__m_Pos__m_fX;
	float CMV2Dot3DPos__m_Pos__m_fY;
	float CMV2Dot3DPos__m_Pos__m_fZ;
	float CMV2Dot3DPos__m_TransfPos__m_fX;
	float CMV2Dot3DPos__m_TransfPos__m_fY;
	float CMV2Dot3DPos__m_TransfPos__m_fZ;
	float CMV2Dot3DPos__m_fZNewRZP;
	uint32_t CMV2Dot3DPos__m_iScreenX;
	uint32_t CMV2Dot3DPos__m_iScreenY;
	float CMV2Dot3DPos__m_fScreenX;
	float CMV2Dot3DPos__m_fScreenY;
	float CMV2Dot3DPos__m_fScreenYError;
	uint32_t CMV2Dot3DPos__m_iClipFlag;
	float CMV2Dot3DPos__m_fR;  // = AngleN
	float CMV2Dot3DPos__m_fG;  // = AngleU
	float CMV2Dot3DPos__m_fB;  // = AngleV
	float CMV2Dot3DPos__m_fAmbientR;
	float CMV2Dot3DPos__m_fAmbientG;
	float CMV2Dot3DPos__m_fAmbientB;
	float CMV2Dot3DPos__m_Norm__m_fX;
	float CMV2Dot3DPos__m_Norm__m_fY;
	float CMV2Dot3DPos__m_Norm__m_fZ;
	float CMV2Dot3DPos__m_AngleU__m_fX;
	float CMV2Dot3DPos__m_AngleU__m_fY;
	float CMV2Dot3DPos__m_AngleU__m_fZ;
	float CMV2Dot3DPos__m_AngleV__m_fX;
	float CMV2Dot3DPos__m_AngleV__m_fY;
	float CMV2Dot3DPos__m_AngleV__m_fZ;
//	CMV2Dot3DPos__m_fAngleN					dd ?
//	CMV2Dot3DPos__m_fAngleU					dd ?
//	CMV2Dot3DPos__m_fAngleV					dd ?
} CMV2Dot3DPos;
#define CMV2Dot3DPos__Size 							(28*4)


typedef struct {
	uint32_t CMV2Dot3D__m_pPos;
	float CMV2Dot3D__m_fTextureU;
	float CMV2Dot3D__m_fTextureV;
} CMV2Dot3D;
#define CMV2Dot3D__Size                             (3*4)


typedef struct {
	uint32_t CMV2Polygon__m_pDot1;
	uint32_t CMV2Polygon__m_pDot2;
	uint32_t CMV2Polygon__m_pDot3;
	uint32_t CMV2Polygon__m_pcTexture;
	uint32_t CMV2Polygon__m_iType;
	uint32_t CMV2Polygon__m_iFlag;
	uint32_t CMV2Polygon__m_pcBumpmap;
	float CMV2Polygon__m_fAddDistance;
} CMV2Polygon;
#define CMV2Polygon__Size                           (8*4)



typedef struct {
	float CMV2ScanlinerPerspT__fTextureUDivZ;
	float CMV2ScanlinerPerspT__fTextureVDivZ;
	float CMV2ScanlinerPerspT__fOneDivZ;
	uint32_t CMV2ScanlinerPerspT__dwScreenX1Offset;
	uint32_t CMV2ScanlinerPerspT__dwXCounter;
} CMV2ScanlinerPerspT;
#define CMV2ScanlinerPerspT__size                   (5*4)



typedef struct {
	uint32_t CMV2ScanlinerLinT__dwTextureU;
	uint32_t CMV2ScanlinerLinT__dwTextureV;
	uint32_t CMV2ScanlinerLinT__dwScreenX1Offset;
	uint32_t CMV2ScanlinerLinT__dwXCounter;
} CMV2ScanlinerLinT;
#define CMV2ScanlinerLinT__size                   	(4*4)



typedef struct {
	uint32_t CMV2ScanlinerLinTG__dwTextureU;
	uint32_t CMV2ScanlinerLinTG__dwTextureV;
	uint32_t CMV2ScanlinerLinTG__dwScreenX1Offset;
	uint32_t CMV2ScanlinerLinTG__dwXCounter;
	uint32_t CMV2ScanlinerLinTG__dwR;
	uint32_t CMV2ScanlinerLinTG__dwG;
	uint32_t CMV2ScanlinerLinTG__dwB;
	float CMV2ScanlinerLinTG__fScreenXError;
} CMV2ScanlinerLinTG;
#define CMV2ScanlinerLinTG__size                   	(8*4)



typedef struct {
	uint32_t CMV2ScanlinerLinTB__dwTextureU;
	uint32_t CMV2ScanlinerLinTB__dwTextureV;
	uint32_t CMV2ScanlinerLinTB__dwScreenX1Offset;
	uint32_t CMV2ScanlinerLinTB__dwXCounter;
	uint32_t CMV2ScanlinerLinTB__dwAngleN;
	uint32_t CMV2ScanlinerLinTB__dwAngleU;
	uint32_t CMV2ScanlinerLinTB__dwAngleV;
	float CMV2ScanlinerLinTB__fScreenXError;
} CMV2ScanlinerLinTB;
#define CMV2ScanlinerLinTB__size                   	(8*4)




typedef struct {
	float CMV2ScanlinerPerspTG__fTextureUDivZ;
	float CMV2ScanlinerPerspTG__fTextureVDivZ;
	float CMV2ScanlinerPerspTG__fOneDivZ;
	uint32_t CMV2ScanlinerPerspTG__dwScreenX1Offset;
	uint32_t CMV2ScanlinerPerspTG__dwXCounter;
	uint32_t CMV2ScanlinerPerspTG__dwR;
	uint32_t CMV2ScanlinerPerspTG__dwG;
	uint32_t CMV2ScanlinerPerspTG__dwB;
	float CMV2ScanlinerPerspTG__fScreenXError;
} CMV2ScanlinerPerspTG;
#define CMV2ScanlinerPerspTG__size					(9*4)




typedef struct {
	float CMV2ScanlinerPerspTB__fTextureUDivZ;
	float CMV2ScanlinerPerspTB__fTextureVDivZ;
	float CMV2ScanlinerPerspTB__fAngleUDivZ;
	float CMV2ScanlinerPerspTB__fAngleVDivZ;
	float CMV2ScanlinerPerspTB__fOneDivZ;
	uint32_t CMV2ScanlinerPerspTB__dwScreenX1Offset;
	uint32_t CMV2ScanlinerPerspTB__dwXCounter;
	float CMV2ScanlinerPerspTB__fScreenXError;
} CMV2ScanlinerPerspTB;
#define CMV2ScanlinerPerspTB__size					(8*4)







typedef struct {
	float CMV2PolygonDistance__m_fAverageDistance;
	uint32_t CMV2PolygonDistance__m_pPolygon;
} CMV2PolygonDistance;



typedef struct {
	uint32_t CMV2PolygonObject__m_pPolygons;
	uint32_t CMV2PolygonObject__m_ppPolygons;
	uint32_t CMV2PolygonObject__m_iNumPolygons;
	uint32_t CMV2PolygonObject__m_pDot3DPos;
	uint32_t CMV2PolygonObject__m_ppDot3DPos;
	uint32_t CMV2PolygonObject__m_iNumDot3DPos;
	uint32_t CMV2PolygonObject__m_pDot3D;
	uint32_t CMV2PolygonObject__m_iNumDot3D;
	uint32_t CMV2PolygonObject__m_iFlagNormCalc;
} CMV2PolygonObject;
#define CMV2PolygonObject__size                     (10*4)


