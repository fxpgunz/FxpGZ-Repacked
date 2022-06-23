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

namespace CatAPI
{
	static const Interface_ID LIMB_INTERFACE_FP(0x230756c7, 0x64d04246);
#define GetCATILimbFPInterface(anim)  (anim ? ((CatAPI::ILimb*)anim->GetInterface(CatAPI::LIMB_INTERFACE_FP)) : nullptr)

	class INodeControl;
	class IHub;

	class ILimb : public FPMixinInterface
	{
	public:
		virtual IHub*				GetIHub() = 0;

		virtual INode*				CreateIKTarget() = 0;
		virtual BOOL				RemoveIKTarget() = 0;
		virtual INode*				GetIKTarget() = 0;

		virtual INode*				CreateUpNode() = 0;
		virtual BOOL				RemoveUpNode() = 0;
		virtual INode*				GetUpNode() = 0;

		virtual INodeControl*		CreateCollarbone() = 0;
		virtual BOOL				RemoveCollarbone() = 0;
		virtual INodeControl*		GetCollarboneINodeControl() = 0;

		virtual INodeControl*		CreatePalmAnkle() = 0;
		virtual BOOL				RemovePalmAnkle() = 0;
		virtual INodeControl*		GetPalmAnkleINodeControl() = 0;

		virtual ILimb*				GetSymLimb() = 0;

		virtual BOOL				GetisLeg() = 0;
		virtual BOOL				GetisArm() = 0;

		virtual int					GetLMR() = 0;
		virtual void				SetLMR(int lmr) = 0;

		//////////////////////////////////////////////////////////////
		virtual Matrix3				GettmIKTarget(TimeValue t, Interval& ivalid) = 0;
		virtual void				SetTemporaryIKTM(const Matrix3& tmTempIKTarget, const Matrix3& tmTempFKTarget) = 0;

		virtual void				MatchIKandFK(TimeValue t) = 0;
		virtual void				MoveIKTargetToEndOfLimb(TimeValue t) = 0;

		virtual float				GetIKFKRatio(TimeValue t, Interval &valid, int boneid = -1) = 0;
		virtual void				SetIKFKRatio(TimeValue t, float val) = 0;

		virtual float				GetForceFeedback(TimeValue t, Interval &valid) = 0;
		virtual void				SetForceFeedback(TimeValue t, float val) = 0;

		virtual float				GetLimbIKPos(TimeValue t, Interval &valid) = 0;
		virtual void				SetLimbIKPos(TimeValue t, float val) = 0;
	};
}
