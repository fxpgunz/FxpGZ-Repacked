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
	static const Interface_ID TAIL_INTERFACE_FP(0x17392178, 0x3c020f81);
#define GetCATITailFPInterface(anim)  (anim ? ((CatAPI::ITail*)anim->GetInterface(CatAPI::TAIL_INTERFACE_FP)) : nullptr)

	class IHub;
	class ITail : public FPMixinInterface
	{
	public:
		virtual IHub*		GetIHub() = 0;
	};
}
