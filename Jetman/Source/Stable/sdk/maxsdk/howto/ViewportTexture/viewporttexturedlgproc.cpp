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
// AUTHOR: Jean-Francois Yelle, Devtech M&E
//***************************************************************************/

#include "viewporttexturedlgproc.h"
#include "viewporttexture.h"
#include "viewporttextureclassdesc.h"
#include "resource.h"

ViewportTextureDlgProc::ViewportTextureDlgProc() :	
	m_pblock2(NULL)
{
}

ViewportTextureDlgProc::~ViewportTextureDlgProc()
{
}

INT_PTR ViewportTextureDlgProc::DlgProc(TimeValue t, IParamMap2 *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_COMMAND:
		if (wParam == IDC_BROWSE_BUTTON)
		{
			BitmapInfo lInfo;
			if (TheManager->SelectFileInput(&lInfo, hWnd))
			{
				m_pblock2->SetValue(ViewportTexture::widget_texturepath, GetCOREInterface()->GetTime(), (TCHAR*)lInfo.Name());
				ViewportTextureClassDesc::GetInstance().InvalidateUI();
			}
			return TRUE;			
		}
		break;
	}
	
	return FALSE;
}

void ViewportTextureDlgProc::DeleteThis()
{
	// we eat this since its static allocated.
	//delete this;
}

void ViewportTextureDlgProc::SetParamBlock(IParamBlock2* pb)
{
	m_pblock2 = pb;
}