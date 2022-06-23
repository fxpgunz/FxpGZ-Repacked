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

class [!output CLASS_NAME] : public [!output SUPER_CLASS_NAME]
{
public:
	//Constructor/Destructor
	[!output CLASS_NAME]();
	virtual ~[!output CLASS_NAME]();

	virtual const TCHAR* Description();

	//TODO: Implement the following methods
	virtual const TCHAR* AuthorName()       { return _T(""); }
	virtual const TCHAR* CopyrightMessage() { return _T(""); }
	virtual UINT         Version()          { return (VERSION_3DSMAX); }
	//TODO: Return the flags that describes the capabilities of the plug-in
	virtual DWORD        Capability() { return( IMGFLT_FILTER | IMGFLT_MASK | IMGFLT_CONTROL ); }

	virtual void ShowAbout( HWND hWnd );
	virtual BOOL ShowControl( HWND hWnd );
	virtual BOOL Render( HWND hWnd );
	void         FilterUpdate();
	INT_PTR      Control(HWND ,UINT ,WPARAM ,LPARAM );
};


[!output TEMPLATESTRING_CLASSDESC]

[!if PARAM_MAPS != 0]
[!output TEMPLATESTRING_PARAMBLOCKDESC]
[!endif]


INT_PTR CALLBACK [!output CLASS_NAME]CtrlDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
	static [!output CLASS_NAME]* filter = nullptr;
	if (message == WM_INITDIALOG) 
		filter = ([!output CLASS_NAME] *)lParam;
	if (filter) 
		return (filter->Control(hWnd,message,wParam,lParam));
	else
		return 0;
}


static INT_PTR CALLBACK AboutCtrlDlgProc(
		HWND hWnd,UINT msg,WPARAM wParam,LPARAM /*lParam*/)
{
	switch(msg) {
		
		case WM_INITDIALOG:
			CenterWindow(hWnd,GetParent(hWnd));
			return TRUE;
		
		case WM_COMMAND:
		  switch (LOWORD(wParam)) {
			 
			 case IDOK:
				  EndDialog(hWnd,1);
				  break;
		  }
		  return 1;
	}
	return 0;
}

//--- [!output CLASS_NAME] -------------------------------------------------------
[!output CLASS_NAME]::[!output CLASS_NAME]()
{

}

[!output CLASS_NAME]::~[!output CLASS_NAME]()
{

}

const TCHAR *[!output CLASS_NAME]::Description()
{
	//TODO: Return the description of the filter
	return _T("");
}

BOOL [!output CLASS_NAME]::ShowControl(HWND hWnd) {
	return (DialogBoxParam(
		hInstance,
		MAKEINTRESOURCE(IDD_PANEL),
		hWnd,
		(DLGPROC)[!output CLASS_NAME]CtrlDlgProc,
		(LPARAM)this));
}

INT_PTR [!output CLASS_NAME]::Control(HWND hWnd,UINT message,WPARAM /*wParam*/,LPARAM ) 
{
	//TODO: Implement the message handling code here
	switch (message)
	{
	case WM_INITDIALOG:
		{
			//-- Make Dialog Interactive
			MakeDlgInteractive(hWnd);
			HWND hWndParent = TheManager->AppWnd();
			CenterWindow(hWnd,hWndParent);
			return 1;
		}
	case WM_CLOSE:
		{
			DestroyWindow(hWnd);
			return 1;
		}
	}

	return 0;
}

void [!output CLASS_NAME]::FilterUpdate()
{
	//This method is called when an instance of the filter is created or updated
}

void [!output CLASS_NAME]::ShowAbout(HWND hWnd)
{
	  DialogBoxParam(
		 hInstance,
		 MAKEINTRESOURCE(IDD_ABOUT),
		 hWnd,
		 (DLGPROC)AboutCtrlDlgProc,
		 (LPARAM)this);
}


BOOL [!output CLASS_NAME]::Render(HWND /*hWnd*/)
{
	//TODO: Implement the image altering code here
	return TRUE;
}
