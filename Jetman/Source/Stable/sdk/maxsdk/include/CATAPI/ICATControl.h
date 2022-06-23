//////////////////////////////////////////////////////////////////////////////
//
//  Copyright 2009 Autodesk, Inc.  All rights reserved.
//  Copyright 2003 Character Animation Technologies.
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.   
//
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../ifnpub.h"

/**********************************************************************
	ICATControl: Published functions for CATControl.
	CAT Rigs are held together by a combination of CATControls and
	CATNodeControls. CATNodeControl is derrived from CATControl so
	every TM Controller is derived from CATNodeControl.
	Given an INode, to see if this INode is a CAT INode, use the following
	line...
	ICATControl* catcontrolfp = GetCATICATControlFPInterface(node->GetTMController());
	*/
namespace CatAPI
{
	static const Interface_ID CATCONTROL_INTERFACE_FP(0xd83dc1, 0x4c851b6c);
#define GetCATICATControlFPInterface(anim)  (anim ? ((CatAPI::ICATControl*)anim->GetInterface(CatAPI::CATCONTROL_INTERFACE_FP)) : nullptr)

	class ICATControl : public FPMixinInterface {
	public:
		// Every character is maintained by its catparent
		virtual INode* GetCATParent() = 0;

		virtual MSTR GetName() = 0;
		virtual void SetName(MSTR newname, BOOL quiet = FALSE) = 0;

		// TODO: Move these fn's to INodeControl
		virtual void PasteLayer(Control* ctrl, int fromindex, int toindex, BOOL instance) = 0;
		virtual BOOL PasteFromCtrl(ReferenceTarget* ctrl, BOOL bMirrorData) = 0;

		virtual BOOL	SaveClip(MSTR filename, TimeValue start_t = 0, TimeValue end_t = -100) = 0;
		virtual INode*	LoadClip(MSTR filename, TimeValue t, BOOL bMirrorData) = 0;

		virtual BOOL	SavePose(MSTR filename) = 0;
		virtual INode*	LoadPose(MSTR filename, TimeValue t, BOOL bMirrorData) = 0;

		virtual void CollapsePoseToCurrLayer(TimeValue t) = 0;
		virtual void CollapseTimeRangeToCurrLayer(TimeValue start_t, TimeValue end_t, TimeValue freq) = 0;
		virtual void ResetTransforms(TimeValue t) = 0;
	};
}
