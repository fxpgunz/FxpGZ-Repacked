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
// DESCRIPTION: Appwizard generated plugin
// AUTHOR: 
//***************************************************************************/

#include "[!output PROJECT_NAME].h"

#define [!output CLASS_NAME]_CLASS_ID	Class_ID([!output CLASSID1], [!output CLASSID2])

class [!output CLASS_NAME] : public [!output SUPER_CLASS_NAME] {
public:

	virtual void BeginEditParams(Interface* ip,ITVUtility* iu);
	virtual void EndEditParams(Interface* ip,ITVUtility* iu);

	virtual void Init(HWND hWnd);
	virtual void Destroy(HWND hWnd);

	virtual void TrackSelectionChanged();
	virtual void NodeSelectionChanged();
	virtual void KeySelectionChanged();
	virtual void TimeSelectionChanged();
	virtual void MajorModeChanged();
	virtual void TrackListChanged();

	virtual void DeleteThis() { }

	// Singleton access
	static [!output CLASS_NAME]* GetInstance()
	{
		static [!output CLASS_NAME] the[!output CLASS_NAME];
		return &the[!output CLASS_NAME]; 
	}
private:
	//Constructor/Destructor
	[!output CLASS_NAME]();
	~[!output CLASS_NAME]();

	HWND        hPanel;
	ITVUtility* iu;
};

[!output TEMPLATESTRING_CLASSDESC]

[!if PARAM_MAPS != 0]
[!output TEMPLATESTRING_PARAMBLOCKDESC]
[!endif]

static INT_PTR CALLBACK [!output CLASS_NAME]DlgProc(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
		case WM_INITDIALOG:
			[!output CLASS_NAME]::GetInstance()->Init(hWnd);
			CenterWindow(hWnd,GetParent(hWnd));
			break;

		case WM_DESTROY:
			[!output CLASS_NAME]::GetInstance()->Destroy(hWnd);
			break;

		case WM_COMMAND:
			break;

		case WM_CLOSE:
			DestroyWindow(hWnd);
			break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			GetCOREInterface()->RollupMouseMessage(hWnd,msg,wParam,lParam); 
			break;

		default:
			return 0;
	}
	return 1;
}

[!output CLASS_NAME]::[!output CLASS_NAME]()
	: hPanel(nullptr)
	, iu(nullptr)
{
	
}

[!output CLASS_NAME]::~[!output CLASS_NAME]()
{
	hPanel = nullptr;
	iu = nullptr;
}

void [!output CLASS_NAME]::BeginEditParams(Interface*, ITVUtility* iu)
{
	this->iu = iu;
	hPanel = CreateDialogParam(
		hInstance,
		MAKEINTRESOURCE(IDD_PANEL),
		iu->GetTrackViewHWnd(),
		[!output CLASS_NAME]DlgProc,
		(LONG_PTR)this);
}

void [!output CLASS_NAME]::EndEditParams(Interface* ip,ITVUtility*)
{
	this->iu = nullptr;
	ip->DeleteRollupPage(hPanel);
	hPanel = nullptr;
}

void [!output CLASS_NAME]::TrackSelectionChanged()
{

}

void [!output CLASS_NAME]::NodeSelectionChanged()
{

}

void [!output CLASS_NAME]::KeySelectionChanged()
{

}

void [!output CLASS_NAME]::TimeSelectionChanged()
{

}

void [!output CLASS_NAME]::MajorModeChanged()
{

}

void [!output CLASS_NAME]::TrackListChanged()
{

}

void [!output CLASS_NAME]::Init(HWND /*handle*/)
{

}

void [!output CLASS_NAME]::Destroy(HWND /*handle*/)
{

}

