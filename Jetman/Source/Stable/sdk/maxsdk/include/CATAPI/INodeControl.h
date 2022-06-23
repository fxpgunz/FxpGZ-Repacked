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
	static const Interface_ID I_NODECONTROL(0x1d956991, 0x7a721746);
#define GetCATINodeControlFPInterface(anim)  (anim ? ((CatAPI::INodeControl*)anim->GetInterface(CatAPI::I_NODECONTROL)) : nullptr)

	class IBoneGroupManager;
	class INodeControl : public FPMixinInterface
	{
	public:
		// Returns the owning data class (Eg LimbData, TailData etc.
		virtual IBoneGroupManager*		GetManager() { return NULL; }

		virtual INode*				GetNode() const = 0;

		virtual int					GetNumArbBones() = 0;
		virtual INodeControl*		AddArbBone(BOOL bAsNewGroup = FALSE) = 0;
		virtual INodeControl*		GetArbBone(int i) = 0;
		virtual Control*			CreateArbBoneController(BOOL bAsNewGroup = FALSE) = 0;

		virtual Control*			CreateLayerFloat() = 0;
		virtual int					NumLayerFloats() = 0;

		virtual Control*			GetLayerTrans() const = 0;

		virtual Matrix3				GetSetupMatrix() = 0;
		virtual void				SetSetupMatrix(Matrix3 tmSetup) = 0;

		virtual Point3				GetBoneDimensions() = 0;

		virtual MSTR				GetBoneAddress() = 0;

		// all the flags we see in the hierarchy panel
		virtual BOOL GetSetupStretchy() const = 0;
		virtual void SetSetupStretchy(BOOL b) = 0;
		virtual BOOL GetAnimationStretchy() const = 0;
		virtual void SetAnimationStretchy(BOOL b) = 0;
		virtual BOOL GetEffectHierarchy() = 0;
		virtual void SetEffectHierarchy(BOOL b) = 0;
		virtual BOOL GetUseSetupController() const = 0;
		virtual void SetUseSetupController(BOOL b) = 0;

		virtual BitArray*	GetSetupModeLocks() = 0;
		virtual void		SetSetupModeLocks(BitArray *locks) = 0;
		virtual BitArray*	GetAnimationLocks() = 0;
		virtual void		SetAnimationLocks(BitArray *locks) = 0;

		virtual BOOL		GetRelativeToSetupMode() = 0;
		virtual void		SetRelativeToSetupMode(BOOL b) = 0;

		virtual BitArray*	GetSetupModeInheritance() = 0;
		virtual void 		SetSetupModeInheritance(BitArray* v) = 0;
		virtual BitArray*	GetAnimModeInheritance() = 0;
		virtual void 		SetAnimModeInheritance(BitArray* v) = 0;

		virtual INode*		GetMirrorBone() = 0;
		virtual void		SetMirrorBone(INode* node) = 0;

		bool IsThisBoneSelected()
		{
			INode* pNode = GetNode();
			if (pNode != NULL)
				return pNode->Selected() != FALSE;
			return false;
		}
	};
}
