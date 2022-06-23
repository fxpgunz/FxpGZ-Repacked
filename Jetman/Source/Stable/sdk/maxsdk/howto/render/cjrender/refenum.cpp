//***************************************************************************
// CJRender - [refenum.cpp] Sample Plugin Renderer for 3D Studio MAX.
// 
// By Christer Janson - Kinetix
//
// Description:
// Implementation of enumeration classes
//
//***************************************************************************

#include "maxincl.h"
#include "cjrender.h"
#include "refenum.h"


//***************************************************************************
// Base class of the map enumerator
//***************************************************************************

int EnumMaps(MtlBase *mb, int subMtl,  MtlEnum &tenum)
{
	if (IsTex(mb)) {
		if (!tenum.proc(mb,subMtl)) {
			return 0;
		}
	}
	for (int i=0; i<mb->NumSubTexmaps(); i++) {
		Texmap *st = mb->GetSubTexmap(i); 
		if (st) {
			int subm = (mb->IsMultiMtl()&&subMtl<0)?i:subMtl;
			if (mb->SubTexmapOn(i)) {
				if (!EnumMaps(st,subm,tenum)) {
					return 0;
				}
			}
		}
	}
	if (IsMtl(mb)) {
		Mtl *m = (Mtl *)mb;
		for (int i=0; i<m->NumSubMtls(); i++) {
			Mtl *sm = m->GetSubMtl(i);
			if (sm) {
				int subm = (mb->IsMultiMtl()&&subMtl<0)?i:subMtl;
				if (!EnumMaps(sm,subm,tenum)) {
					return 0;
				}
			}
		}
	}
	return 1;
}


//***************************************************************************
// Enumerate a material tree
//***************************************************************************

int EnumMtlTree(MtlBase *mb, int subMtl, MtlEnum &tenum)
{
	for (int i=0; i<mb->NumSubTexmaps(); i++) {
		Texmap *st = mb->GetSubTexmap(i);
		if (st) {
			if (!EnumMtlTree(st,subMtl, tenum)) {
				return 0;
			}
		}
	}
	if (IsTex(mb)) {
		if (!tenum.proc(mb,subMtl)) {
			return 0;
		}
	}
	if (IsMtl(mb)) {
		Mtl *m = (Mtl *)mb;
		for (int i=0; i<m->NumSubMtls(); i++) {
			Mtl *sm = m->GetSubMtl(i);
			int subm = (mb->IsMultiMtl()&&subMtl<0)?i:subMtl;
			if (sm) {
				if (!EnumMtlTree(sm,subm,tenum)) {
					return 0;
				}
			}
		}
		if (!tenum.proc(mb,subMtl)) {
			return 0;
		}
	}
	return 1;
}

//***************************************************************************
// Constructor of map enumerator
//***************************************************************************

GetMaps::GetMaps(MtlBaseLib *mbl)
{
	mlib = mbl;
}

//***************************************************************************
// Implementation of the map enumerator
//***************************************************************************

int GetMaps::proc(ReferenceMaker *rm)
{
	if (IsTex((MtlBase*)rm))
		mlib->AddMtl((MtlBase *)rm);
	return REF_ENUM_CONTINUE;
}


//***************************************************************************
// Class to manage names of missing maps
//***************************************************************************

CheckFileNames::CheckFileNames(NameTab* n)
{
	missingMaps = n;
}

//***************************************************************************
// Add a name to the list if it's not already there
//***************************************************************************

void CheckFileNames::RecordAsset(const MaxSDK::AssetManagement::AssetUser&asset)
{ 

	if (asset.GetId()!=MaxSDK::AssetManagement::kInvalidId) {
		if (missingMaps->FindName(asset.GetFileName())<0) {
			missingMaps->AddName(asset.GetFileName());
		}
	}
}


//***************************************************************************
// Constructor of map loader
// Map loader enum proc
//***************************************************************************

MapLoadEnum::MapLoadEnum(TimeValue time)
{ 
	t = time; 
}

int MapLoadEnum::proc(MtlBase *m, int subMtlNum)
{
	Texmap *tm = (Texmap *)m;
	tm->LoadMapFiles(t);
	return 1;
}

//***************************************************************************
// Constructor of AutoReflect enumerator
// AutoReflect enum proc
// Note:
//***************************************************************************

MapSetupEnum::MapSetupEnum(TimeValue time, CJRenderer* renderer, Instance* inst)
	: rmc(renderer, inst)
{
	t = time; 
	r = renderer;
}

int MapSetupEnum::proc(MtlBase *m, int subMtlNum)
{
	ULONG reqmask = MTLREQ_AUTOREFLECT | MTLREQ_AUTOMIRROR | MTLREQ_PREPRO;
	if (m->LocalRequirements(subMtlNum) & reqmask) {
		rmc.SetSubMtlIndex(subMtlNum);
		m->BuildMaps(t, rmc);
	}
	return 1;
}


//***************************************************************************
// Enumerator to call RenderBegin() on all objects
//***************************************************************************

BeginEnum::BeginEnum(TimeValue startTime)
{
	t = startTime;
}

int BeginEnum::proc(ReferenceMaker *rm)
{
	rm->RenderBegin(t);
	return REF_ENUM_CONTINUE;
}

//***************************************************************************
// Enumerator to call RenderEnd() on all objects
//***************************************************************************

EndEnum::EndEnum(TimeValue endTime)
{
	t = endTime;
}

int EndEnum::proc(ReferenceMaker *rm)
{
	rm->RenderEnd(t);
	return REF_ENUM_CONTINUE;
}
