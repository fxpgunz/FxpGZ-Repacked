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
// DESCRIPTION: Specialized ParamBlock Dialog proc to add a browser for a file
//              edit field (defined in the object associated IParamBlock2 
//              object.)
// AUTHOR:		Jean-Francois Yelle, Devtech M&E
//***************************************************************************/

#include "max.h"
#include "iparamm2.h"

class ViewportTextureDlgProc: public ParamMap2UserDlgProc 
{
public:
	ViewportTextureDlgProc();
	virtual ~ViewportTextureDlgProc();

	virtual INT_PTR DlgProc(TimeValue t, IParamMap2 *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	virtual void DeleteThis();
	virtual void SetParamBlock(IParamBlock2 *pb);	
	
private:
	IParamBlock2* m_pblock2;
};
