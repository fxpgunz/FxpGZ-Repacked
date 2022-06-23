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
	static const Interface_ID BONEGROUPMANAGER_INTERFACE_FP(0x202b38a8, 0x2746669a);
#define GetCATIBoneGroupManagerFPInterface(anim)  (anim ? ((CatAPI::IBoneGroupManager*)anim->GetInterface(CatAPI::BONEGROUPMANAGER_INTERFACE_FP)) : nullptr)

	//////////////////////////////////////////////////////////////////////////
	// The group manager classes essentially own and manage their 
	// associated bones as a logical collection.  Examples are
	// LimbData, TailData, DigitData, and BoneData
	class INodeControl;

	class IBoneGroupManager : public FPMixinInterface {
	public:

		// Access to child bones:
		virtual int				GetNumBones() const = 0;
		virtual void			SetNumBones(int i) = 0;
		virtual INodeControl*	GetBoneINodeControl(int i) = 0;

		// Const accessors
		inline const INodeControl* GetBoneINodeControl(int i) const { return const_cast<IBoneGroupManager*>(this)->GetBoneINodeControl(i); }
	};
}
