//***************************************************************************
// CJRender - [refenum.h] Sample Plugin Renderer for 3D Studio MAX.
// 
// By Christer Janson - Kinetix
//
// Description:
// Definition of enumeration classes
//
//***************************************************************************

class MtlEnum {
	public:
		virtual ~MtlEnum() {;}
		virtual int proc(MtlBase *m, int subMtlNum) = 0;
};

class GetMaps: public RefEnumProc {
	MtlBaseLib *mlib;
	public:
	int proc(ReferenceMaker *rm);
	GetMaps(MtlBaseLib *mbl);
};

class CheckFileNames: public AssetEnumCallback {
	public:
		NameTab* missingMaps;
		BitmapInfo bi;
		CheckFileNames(NameTab* n);
		void RecordAsset(const MaxSDK::AssetManagement::AssetUser&asset);
};

class MapLoadEnum:public MtlEnum {
	public:
		TimeValue t;

	   	MapLoadEnum(TimeValue time);
		virtual int proc(MtlBase *m, int subMtlNum);
};

class MapSetupEnum:public MtlEnum {
		TimeValue t;
		CJRenderer* r;
		CJRenderMapsContext rmc;

	public:

	   	MapSetupEnum(TimeValue time, CJRenderer* renderer, Instance* inst);
		virtual int proc(MtlBase *m, int subMtlNum);
};

//***************************************************************************
// Enumerators to manage the RenderBegin()/RenderEnd() calls
//***************************************************************************

class BeginEnum : public RefEnumProc {
	public:
	TimeValue t;
	BeginEnum(TimeValue startTime);
	int proc(ReferenceMaker *rm);
};

class EndEnum : public RefEnumProc {
	public:
	TimeValue t;
	EndEnum(TimeValue endTime);
	int proc(ReferenceMaker *rm);
};

//***************************************************************************
// Prototypes
//***************************************************************************

int		EnumMaps(MtlBase *mb, int subMtl,  MtlEnum &tenum);
int		EnumMtlTree(MtlBase *mb, int subMtl, MtlEnum &tenum);
