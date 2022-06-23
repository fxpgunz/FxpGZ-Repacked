/**********************************************************************
 *<
	FILE: CASample.h

	DESCRIPTION:	Defines the various Custom Attributers being added

	CREATED BY:		Nikolai Sander

	HISTORY:		Created:  5/26/00

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/

#ifndef	__CUSTATTRIB_SAMPLE_H__
#define __CUSTATTRIB_SAMPLE_H__

#include "CustAttrib.h"
#include "imtl.h"

#define SIMPLE_CLASS_ID Class_ID(0x6351506a, 0x1f47bb78)
#define SWATCH_CLASS_ID Class_ID(0x5b621f96, 0x4ed628af)
#define NODE_CLASS_ID Class_ID(0x5b425f96, 0x4ed664af)
#define MCA_CLASS_ID Class_ID(0x5a05193c, 0x70420c1e)

#define PBLOCK_REF 0
#define MAP_REF 1


enum { cust_attrib_params };  // pblock ID

enum 
{ 
	ca_submap, 
	ca_color
};


class MatCustAttrib : public CustAttrib, public ISubMap
{
	
	IParamBlock2 *pblock;
	Texmap *submap;

public:
	
	MatCustAttrib();
	~MatCustAttrib();

	virtual RefResult NotifyRefChanged(const Interval& changeInt, RefTargetHandle hTarget, 
								   PartID& partID, RefMessage message, BOOL propagate){return REF_SUCCEED;}
	
	virtual ParamDlg *CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp);


	int	NumParamBlocks() { return 1; }			
	IParamBlock2* GetParamBlock(int i) { if(i == 0) return pblock; else return NULL;} 
	IParamBlock2* GetParamBlockByID(short id) { if(id == cust_attrib_params ) return pblock; else return NULL;} 

	int NumRefs() { return 2;}
private:
	virtual void SetReference(int i, RefTargetHandle rtarg);
public:
	RefTargetHandle GetReference(int i);
	
	virtual	int NumSubs();
	virtual	Animatable* SubAnim(int i);
	virtual TSTR SubAnimName(int i);
	virtual void GetClassName(MSTR& s) { s = _M("MatCustAttrib"); }

	SClass_ID		SuperClassID() {return CUST_ATTRIB_CLASS_ID;}
	Class_ID 		ClassID() {return MCA_CLASS_ID;}	
	

	// Methods to access texture maps of material
	int NumSubTexmaps() {return 1;}
	Texmap* GetSubTexmap(int i) {if(i == 0) return submap; else return NULL;}		
	void SetSubTexmap(int i, Texmap *m);
	TSTR GetSubTexmapSlotName(int i){ if(i==0) return _T("SubMap"); else return _T(" ");}
	virtual int MapSlotType(int i) { return 0; }

	ReferenceTarget *Clone(RemapDir &remap);
	virtual bool CheckCopyAttribTo(ICustAttribContainer *to) { return true; }
	
	virtual const TCHAR* GetName(){ return _T("DynamicMtl");}
	virtual void* GetInterface(ULONG id){ if(id==I_SUBMAP) return (ISubMap*)this; else return CustAttrib::GetInterface(id); }	
	virtual ReferenceTarget *GetRefTarget(){ return this;};
	
	void DeleteThis() { delete this;}


};





class SimpleCustAttrib : public CustAttrib
{
public:
	IParamBlock2 *pblock;
	HWND hPanel;

	SimpleCustAttrib();

	virtual RefResult NotifyRefChanged(const Interval& changeInt, RefTargetHandle hTarget, 
							   PartID& partID, RefMessage message, BOOL propagate){return REF_SUCCEED;}

	int	NumParamBlocks() { return 1; }					// return number of ParamBlocks in this instance
	IParamBlock2* GetParamBlock(int i) { return pblock; } // return i'th ParamBlock
	IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock->ID() == id) ? pblock : NULL; } // return id'd ParamBlock

	int NumRefs() { return 1;}
	virtual RefTargetHandle GetReference(int i) { if(i == 0) return pblock; else return NULL; }
private:
	virtual void SetReference(int i, RefTargetHandle rtarg) { if(i == 0) pblock = (IParamBlock2 *)rtarg;}
public:

	virtual	int NumSubs()  { return 1; }
	virtual	Animatable* SubAnim(int i) { return pblock; }
	virtual TSTR SubAnimName(int i){ return GetString(IDS_SIMPLECA);} 
	virtual void GetClassName(MSTR& s) { s = _M("SimpleCustAttrib"); }

	void BeginEditParams(IObjParam *ip,ULONG flags,Animatable *prev);
	void EndEditParams(IObjParam *ip, ULONG flags, Animatable *next);
	SClass_ID		SuperClassID() {return CUST_ATTRIB_CLASS_ID;}
	Class_ID 		ClassID() {return SIMPLE_CLASS_ID;}

	ReferenceTarget *Clone(RemapDir &remap);
	virtual bool CheckCopyAttribTo(ICustAttribContainer *to) { return true; }
	
	virtual const TCHAR* GetName(){ return _T("Simple CA");}
	void DeleteThis() { delete this;}

};

class SwatchCustAttrib : public CustAttrib
{
public:
	IParamBlock2 *pblock;
	HWND hPanel;

	SwatchCustAttrib();

	virtual RefResult NotifyRefChanged(const Interval& changeInt, RefTargetHandle hTarget, 
							   PartID& partID, RefMessage message, BOOL propagate){return REF_SUCCEED;}

	int	NumParamBlocks() { return 1; }					// return number of ParamBlocks in this instance
	IParamBlock2* GetParamBlock(int i) { return pblock; } // return i'th ParamBlock
	IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock->ID() == id) ? pblock : NULL; } // return id'd ParamBlock

	int NumRefs() { return 1;}
	virtual RefTargetHandle GetReference(int i) { if(i == 0) return pblock; else return NULL; }
private:
	virtual void SetReference(int i, RefTargetHandle rtarg) { if(i == 0) pblock = (IParamBlock2 *)rtarg;}
public:

	virtual	int NumSubs()  { return 1; }
	virtual	Animatable* SubAnim(int i) { return pblock; }
	virtual TSTR SubAnimName(int i){ return GetString(IDS_SWATCHCA);} 
	virtual void GetClassName(MSTR& s) { s = _M("SwatchCustAttrib"); }

	void BeginEditParams(IObjParam *ip,ULONG flags,Animatable *prev);
	void EndEditParams(IObjParam *ip, ULONG flags, Animatable *next);
	SClass_ID		SuperClassID() {return CUST_ATTRIB_CLASS_ID;}
	Class_ID 		ClassID() {return SWATCH_CLASS_ID;}

	ReferenceTarget *Clone(RemapDir &remap);
	virtual bool CheckCopyAttribTo(ICustAttribContainer *to) { return true; }
	
	virtual const TCHAR* GetName(){ return _T("Swatch CA");}
	void DeleteThis() { delete this;}

};

class NodeCustAttrib : public CustAttrib
{
public:
	IParamBlock2 *pblock;
	HWND hPanel;

	NodeCustAttrib();

	virtual RefResult NotifyRefChanged(const Interval& changeInt, RefTargetHandle hTarget, 
							   PartID& partID, RefMessage message, BOOL propagate){return REF_SUCCEED;}

	int	NumParamBlocks() { return 1; }					// return number of ParamBlocks in this instance
	IParamBlock2* GetParamBlock(int i) { return pblock; } // return i'th ParamBlock
	IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock->ID() == id) ? pblock : NULL; } // return id'd ParamBlock

	int NumRefs() { return 1;}
	virtual RefTargetHandle GetReference(int i) { if(i == 0) return pblock; else return NULL; }
private:
	virtual void SetReference(int i, RefTargetHandle rtarg) { if(i == 0) pblock = (IParamBlock2 *)rtarg;}
public:

	virtual	int NumSubs()  { return 1; }
	virtual	Animatable* SubAnim(int i) { return pblock; }
	virtual TSTR SubAnimName(int i){ return GetString(IDS_NODECA);} 
	virtual void GetClassName(MSTR& s) { s = _M("NodeCustAttrib"); }


	void BeginEditParams(IObjParam *ip,ULONG flags,Animatable *prev);
	void EndEditParams(IObjParam *ip, ULONG flags, Animatable *next);
	SClass_ID		SuperClassID() {return CUST_ATTRIB_CLASS_ID;}
	Class_ID 		ClassID() {return NODE_CLASS_ID;}

	ReferenceTarget *Clone(RemapDir &remap);
	virtual bool CheckCopyAttribTo(ICustAttribContainer *to) { return true; }
	
	virtual const TCHAR* GetName(){ return _T("Node CA");}
	void DeleteThis() { delete this;}

};

class MatCustAttribClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() {return 1;}
	void *			Create(BOOL loading) {return new MatCustAttrib;}
	const TCHAR *	ClassName() { return GetString(IDS_MATCACLASSNAME); }
	SClass_ID		SuperClassID() {return CUST_ATTRIB_CLASS_ID;}
	Class_ID 		ClassID() {return MCA_CLASS_ID;}
	const TCHAR* 	Category() {return _T("");}
	const TCHAR*	InternalName() { return _T("MatCustAttrib"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }			// returns owning module handle
	};

class SimpleAttribClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() {return 1;}
	void *			Create(BOOL loading) {return new SimpleCustAttrib;}
	const TCHAR *	ClassName() { return GetString(IDS_SIMPLE_CLASSNAME); }
	SClass_ID		SuperClassID() {return CUST_ATTRIB_CLASS_ID;}
	Class_ID 		ClassID() {return SIMPLE_CLASS_ID;}
	const TCHAR* 	Category() {return _T("");}
	const TCHAR*	InternalName() { return _T("SimpleCustAttrib"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }			// returns owning module handle
	};

class SwatchAttribClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() {return 1;}
	void *			Create(BOOL loading) {return new SwatchCustAttrib;}
	const TCHAR *	ClassName() { return GetString(IDS_SWATCH_CLASSNAME); }
	SClass_ID		SuperClassID() {return CUST_ATTRIB_CLASS_ID;}
	Class_ID 		ClassID() {return SWATCH_CLASS_ID;}
	const TCHAR* 	Category() {return _T("");}
	const TCHAR*	InternalName() { return _T("SimpleCustAttrib"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }			// returns owning module handle
	};

class NodeAttribClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() {return 1;}
	void *			Create(BOOL loading) {return new NodeCustAttrib;}
	const TCHAR *	ClassName() { return GetString(IDS_NODE_CLASSNAME); }
	SClass_ID		SuperClassID() {return CUST_ATTRIB_CLASS_ID;}
	Class_ID 		ClassID() {return NODE_CLASS_ID;}
	const TCHAR* 	Category() {return _T("");}
	const TCHAR*	InternalName() { return _T("NodeCustAttrib"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }			// returns owning module handle
	};

#endif
