/*===========================================================================*\
 | 
 |  FILE:	CustomVData.h
 |			Project to demonstrate custom data per vertex
 |			Simply allows user to define a custom value and bind it to a vertex
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 6-4-99
 | 
\*===========================================================================*/

#ifndef __CVDSKEL__H
#define __CVDSKEL__H

#include "max.h"
#include "iparamm2.h"
#include "istdplug.h"
#include "meshadj.h"
#include "modstack.h"
#include "macrorec.h"
#include "random.h"
#include "resource.h"


#define	CVD_CLASSID		Class_ID(0x4fd31c27, 0xa9323c7)


TCHAR *GetString(int id);
extern ClassDesc* GetCustomVDataDesc();


// Paramblock2 name
enum { cvd_params, }; 
// Paramblock2 parameter list
enum { cvd_codev, };



/*===========================================================================*\
 |	CVDModifier class defn
\*===========================================================================*/

class CVDModifier : public Modifier{
	public:

		// Access to the interface
		static IObjParam *ip;

		// Global parameter block
		IParamBlock2	*pblock;

		// Our random number generator
	    Random  randomGen;


		//Constructor/Destructor
		CVDModifier();
		~CVDModifier() {}
		void DeleteThis() { delete this; }


		// Plugin identification
		void GetClassName(TSTR& s) { s= TSTR(GetString(IDS_CLASSNAME)); }  
		virtual Class_ID ClassID() { return CVD_CLASSID;}		
		const TCHAR *GetObjectName() { return GetString(IDS_CLASSNAME); }


		// Defines the behavior for this modifier
		// This is currently setup to be basic geometry 
		// modification of deformable objects
		ChannelMask ChannelsUsed()  { return GEOM_CHANNEL|TOPO_CHANNEL; }
		ChannelMask ChannelsChanged() { return GEOM_CHANNEL|TOPO_CHANNEL; }
		Class_ID InputType() { return triObjectClassID; }
		BOOL ChangeTopology() {return FALSE;}


		// Calculate the local validity from the parameters
		Interval LocalValidity(TimeValue t);
		Interval GetValidity(TimeValue t);


		// Object modification and notification of change
		void ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node);
		void NotifyInputChanged(const Interval& changeInt, PartID partID, RefMessage message, ModContext *mc);


		// Reference support
		int NumRefs() { return 1; }
		RefTargetHandle GetReference(int i);
private:
		virtual void SetReference(int i, RefTargetHandle rtarg);
public:
		RefTargetHandle Clone(RemapDir& remap);
		RefResult NotifyRefChanged( const Interval& changeInt,RefTargetHandle hTarget, 
		   PartID& partID, RefMessage message, BOOL propagate);

		
		// SubAnim support
		int NumSubs() { return 1; }
		Animatable* SubAnim(int i);
		TSTR SubAnimName(int i);


		// Direct paramblock access
		int	NumParamBlocks() { return 1; }	
		IParamBlock2* GetParamBlock(int i) { return pblock; }
		IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock->ID() == id) ? pblock : NULL; }
		int GetParamBlockIndex(int id) {return id;}

		// Does not use createmouse callbacks
		CreateMouseCallBack* GetCreateMouseCallBack() {return NULL;}

		// Load and unload our UI
		void BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev);
		void EndEditParams(IObjParam *ip, ULONG flags,Animatable *next);
		void InvalidateUI();
};


/*===========================================================================*\
 |	Dialog Processor
\*===========================================================================*/

class CVDModDlgProc : public ParamMap2UserDlgProc 
{
	public:
		CVDModifier *cvdm;

		CVDModDlgProc() {}
		CVDModDlgProc(CVDModifier *cvdm_in) { cvdm = cvdm_in; }

		INT_PTR DlgProc(TimeValue t, IParamMap2 *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		void DeleteThis() { }

		void SetThing(ReferenceTarget *m) {
			cvdm = (CVDModifier*)m;
			}

};


#endif
