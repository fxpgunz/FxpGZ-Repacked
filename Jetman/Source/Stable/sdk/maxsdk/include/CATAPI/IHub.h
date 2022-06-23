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
	static const Interface_ID HUB_INTERFACE_FP(0x230756c7, 0x64d04246);
#define GetIHubInterface(anim)  (anim ? ((CatAPI::IHub*)anim->GetInterface(CatAPI::HUB_INTERFACE_FP)) : nullptr)

	static const int DEFAULT_NUM_CAT_BONES = 5;

	// We use the CATControl flags system so make sure 
	// no Hub flags overlap with the CATControl flags
	static const DWORD HUBFLAG_ALLOW_IKCONTRAINT_ROT = (1 << 16);
	static const DWORD HUBFLAG_INSPINE_RESTRICTS_MOVEMENT = (1 << 18);

	class ISpine;
	class ILimb;
	class ITail;

	class IHub : public FPMixinInterface
	{
	public:
		virtual ISpine*	GetInISpine() = 0;

		virtual	int		GetNumSpines() = 0;
		virtual	ISpine*	GetISpine(int index) = 0;
		virtual	ISpine*	AddSpine(int numbones = DEFAULT_NUM_CAT_BONES, BOOL loading = FALSE) = 0;
		virtual void	RemoveSpine(int index, BOOL deletenodes = TRUE) = 0;

		virtual	int		GetNumTails() = 0;
		virtual	ITail*	GetITail(int index) = 0;
		virtual	ITail*	AddTail(int numbones = DEFAULT_NUM_CAT_BONES, BOOL loading = FALSE) = 0;
		virtual void	RemoveTail(int index, BOOL deletenodes = TRUE) = 0;

		virtual	int		GetNumLimbs() = 0;
		virtual	ILimb*	GetILimb(int index) = 0;
		virtual void	RemoveLimb(int index, BOOL deletenodes = TRUE) = 0;

		virtual	ILimb*	AddArm(BOOL bCollarbone, BOOL bPalm) = 0;
		virtual	ILimb*	AddLeg(BOOL bCollarbone, BOOL bAnkle) = 0;

		virtual BOOL GetPinHub() = 0;
		virtual void SetPinHub(BOOL b) = 0;
	};
}
