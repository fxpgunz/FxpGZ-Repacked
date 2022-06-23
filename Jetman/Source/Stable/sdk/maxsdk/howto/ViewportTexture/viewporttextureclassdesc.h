#pragma once

//**************************************************************************/
// Copyright (c) 1998-2007 Autodesk, Inc.
// All rights reserved.
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/
// DESCRIPTION: Class describer defined outside plug-in object implementation
//              This shows it is possible to make clean C++ header and src 
//              files for 3ds Max plugins.
// AUTHOR: Jean-Francois Yelle, Devtech M&E
//***************************************************************************/

#include "max.h"
#include "iparamb2.h"

// some way of destructing the dynamically allocated singleton.
class ViewportTextureClassDescAutoDestruction;

class ViewportTextureClassDesc : public ClassDesc2 
{
public:
	static ViewportTextureClassDesc& GetInstance();
	virtual ~ViewportTextureClassDesc();

	virtual int IsPublic();
	virtual void* Create(BOOL loading = FALSE);
	virtual const TCHAR* ClassName();
	virtual SClass_ID SuperClassID();
	virtual Class_ID ClassID();
	virtual const TCHAR* Category();

	virtual const TCHAR* InternalName(); // returns fixed parsable name (scripter-visible name)
	virtual HINSTANCE HInstance(); // returns owning module handle

private:
	// singleton help emphasize single instance nature of this object.
	// ctor, dtor private
	ViewportTextureClassDesc();
	

	// We use a lazy allocation mechanism because static allocated paramblockdesc 
	// construction need it and we cannot control static element creation order.
	static ViewportTextureClassDesc* sm_instance;
};
