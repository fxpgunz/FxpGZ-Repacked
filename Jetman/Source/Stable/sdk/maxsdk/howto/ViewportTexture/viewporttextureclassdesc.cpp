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

#include "viewporttextureclassdesc.h"
#include "viewporttexture.h"
#include "resource.h"

ViewportTextureClassDesc* ViewportTextureClassDesc::sm_instance = NULL;

ViewportTextureClassDesc& ViewportTextureClassDesc::GetInstance()
{
	if (!sm_instance)
	{
		sm_instance = new ViewportTextureClassDesc;
	}
	return *sm_instance;
}

int ViewportTextureClassDesc::IsPublic()
{ 
	return TRUE; 
}

void* ViewportTextureClassDesc::Create(BOOL loading /*= FALSE*/) 		
{ 
	return new ViewportTexture(); 
}

const TCHAR *	ViewportTextureClassDesc::ClassName() 			
{ 
	return GetString(IDS_CLASS_NAME); 
}

SClass_ID ViewportTextureClassDesc::SuperClassID() 				
{ 
	return GEOMOBJECT_CLASS_ID; 
}

Class_ID ViewportTextureClassDesc::ClassID() 						
{ 
	return ViewportTexture_CLASS_ID; 
}

const TCHAR* ViewportTextureClassDesc::Category() 				
{ 
	return GetString(IDS_CATEGORY); 
}

const TCHAR* ViewportTextureClassDesc::InternalName() 			
{ 
	return _T("ViewportTexture"); 
}

HINSTANCE ViewportTextureClassDesc::HInstance() 					
{ 
	return hInstance; 
}

ViewportTextureClassDesc::ViewportTextureClassDesc()
{
}

ViewportTextureClassDesc::~ViewportTextureClassDesc()
{
	sm_instance = NULL;
}