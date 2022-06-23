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
	//Constructor/Destructor
	[!output CLASS_NAME]();
	~[!output CLASS_NAME]();

	int				ExtCount();					// Number of extensions supported
	const TCHAR *	Ext(int n);					// Extension #n (i.e. "3DS")
	const TCHAR *	LongDesc();					// Long ASCII description (i.e. "Autodesk 3D Studio File")
	const TCHAR *	ShortDesc();				// Short ASCII description (i.e. "3D Studio")
	const TCHAR *	AuthorName();				// ASCII Author name
	const TCHAR *	CopyrightMessage();			// ASCII Copyright message
	const TCHAR *	OtherMessage1();			// Other message #1
	const TCHAR *	OtherMessage2();			// Other message #2
	unsigned int	Version();					// Version number * 100 (i.e. v3.01 = 301)
	void			ShowAbout(HWND hWnd);		// Show DLL's "About..." box

	BOOL SupportsOptions(int ext, DWORD options);
	int  DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0);
};


[!output TEMPLATESTRING_CLASSDESC]

[!if PARAM_MAPS != 0]
[!output TEMPLATESTRING_PARAMBLOCKDESC]
[!endif]


INT_PTR CALLBACK [!output CLASS_NAME]OptionsDlgProc(HWND hWnd,UINT message,WPARAM,LPARAM lParam) {
	static [!output CLASS_NAME]* imp = nullptr;

	switch(message) {
		case WM_INITDIALOG:
			imp = ([!output CLASS_NAME] *)lParam;
			CenterWindow(hWnd,GetParent(hWnd));
			return TRUE;

		case WM_CLOSE:
			EndDialog(hWnd, 0);
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

int [!output CLASS_NAME]::ExtCount()
{
	#pragma message(TODO("Returns the number of file name extensions supported by the plug-in."))
	return 1;
}

const TCHAR *[!output CLASS_NAME]::Ext(int /*i*/)
{		
	#pragma message(TODO("Return the 'i-th' file name extension (i.e. \"3DS\")."))
	return _T("");
}

const TCHAR *[!output CLASS_NAME]::LongDesc()
{
	#pragma message(TODO("Return long ASCII description (i.e. \"Targa 2.0 Image File\")"))
	return _T("");
}
	
const TCHAR *[!output CLASS_NAME]::ShortDesc() 
{			
	#pragma message(TODO("Return short ASCII description (i.e. \"Targa\")"))
	return _T("");
}

const TCHAR *[!output CLASS_NAME]::AuthorName()
{			
	#pragma message(TODO("Return ASCII Author name"))
	return _T("");
}

const TCHAR *[!output CLASS_NAME]::CopyrightMessage() 
{	
	#pragma message(TODO("Return ASCII Copyright message"))
	return _T("");
}

const TCHAR *[!output CLASS_NAME]::OtherMessage1() 
{		
	//TODO: Return Other message #1 if any
	return _T("");
}

const TCHAR *[!output CLASS_NAME]::OtherMessage2() 
{		
	//TODO: Return other message #2 in any
	return _T("");
}

unsigned int [!output CLASS_NAME]::Version()
{				
	#pragma message(TODO("Return Version number * 100 (i.e. v3.01 = 301)"))
	return 100;
}

void [!output CLASS_NAME]::ShowAbout(HWND /*hWnd*/)
{			
	// Optional
}

BOOL [!output CLASS_NAME]::SupportsOptions(int /*ext*/, DWORD /*options*/)
{
	#pragma message(TODO("Decide which options to support.  Simply return true for each option supported by each Extension the exporter supports."))
	return TRUE;
}


int	[!output CLASS_NAME]::DoExport(const TCHAR* /*name*/, ExpInterface* /*ei*/, Interface* /*ip*/, BOOL suppressPrompts, DWORD /*options*/)
{
	#pragma message(TODO("Implement the actual file Export here and"))

	if(!suppressPrompts)
		DialogBoxParam(hInstance, 
				MAKEINTRESOURCE(IDD_PANEL), 
				GetActiveWindow(), 
				[!output CLASS_NAME]OptionsDlgProc, (LPARAM)this);

	#pragma message(TODO("return TRUE If the file is exported properly"))
	return FALSE;
}


