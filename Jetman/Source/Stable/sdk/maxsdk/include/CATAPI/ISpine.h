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
	static const Interface_ID I_SPINE_FP(0x5a18648b, 0x43df284a);
#define GetCATISpineControlFPInterface(anim)  (anim ? ((CatAPI::ISpine*)anim->GetInterface(CatAPI::I_SPINE_FP)) : nullptr)

	class IHub;
	class ISpine : public FPMixinInterface
	{
	public:

		virtual int		GetSpineID() = 0;
		virtual void	SetSpineID(int id) = 0;

		virtual IHub*	GetBaseIHub() = 0;
		virtual IHub*	GetTipIHub() = 0;

		virtual float GetRotWeight(float index) = 0;

		virtual float	GetSpineLength() = 0;
		virtual void	SetSpineLength(float val) = 0;

		virtual float	GetSpineWidth() = 0;
		virtual void	SetSpineWidth(float val) = 0;

		virtual float	GetSpineDepth() = 0;
		virtual void	SetSpineDepth(float val) = 0;

		virtual void	SetSpineFK(BOOL tf) = 0;
		virtual BOOL	GetSpineFK() const = 0;

		/** Returns the absolute/relative ratio.  At 1.0f the tip hub is absolute and does not inherit rotations from the base. */
		virtual void	SetAbsRel(TimeValue t, float val) = 0;
		virtual float	GetAbsRel(TimeValue t, Interval& valid) = 0;
	};
}
