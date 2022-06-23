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
//				3DXI file exporter project template
//				For a more in-depth exemple of a 3DXI exporter,
//				please refer to maxsdk\samples\igame\export.
// AUTHOR:		Jean-Francois Yelle - created Mar.20.2007
//***************************************************************************/

#include "[!output PROJECT_NAME].h"

#include "3dsmaxsdk_preinclude.h"
#include "IGame/IGame.h"


#define [!output CLASS_NAME]_CLASS_ID	Class_ID([!output CLASSID1], [!output CLASSID2])

class [!output CLASS_NAME] : public [!output SUPER_CLASS_NAME] 
{
	public:
		
		virtual int				ExtCount();					// Number of extensions supported
		virtual const TCHAR *	Ext(int n);					// Extension #n (i.e. "3DS")
		virtual const TCHAR *	LongDesc();					// Long ASCII description (i.e. "Autodesk 3D Studio File")
		virtual const TCHAR *	ShortDesc();				// Short ASCII description (i.e. "3D Studio")
		virtual const TCHAR *	AuthorName();				// ASCII Author name
		virtual const TCHAR *	CopyrightMessage();			// ASCII Copyright message
		virtual const TCHAR *	OtherMessage1();			// Other message #1
		virtual const TCHAR *	OtherMessage2();			// Other message #2
		virtual unsigned int	Version();					// Version number * 100 (i.e. v3.01 = 301)
		virtual void			ShowAbout(HWND hWnd);		// Show DLL's "About..." box

		virtual BOOL SupportsOptions(int ext, DWORD options);
		virtual int	DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0);

		//Constructor/Destructor
		[!output CLASS_NAME]();
		virtual ~[!output CLASS_NAME]();

	protected:
		int GetSceneNodes(INodeTab& i_nodeTab, INode* i_currentNode = NULL);
};


[!output TEMPLATESTRING_CLASSDESC]

[!if PARAM_MAPS != 0]
[!output TEMPLATESTRING_PARAMBLOCKDESC]
[!endif]


INT_PTR CALLBACK [!output CLASS_NAME]OptionsDlgProc(HWND hWnd,UINT message,WPARAM /*wParam*/,LPARAM lParam) {
	static [!output CLASS_NAME] *imp = NULL;

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
	// Returns the number of file name extensions supported by the plug-in.
	return 1;
}

const TCHAR *[!output CLASS_NAME]::Ext(int /*n*/)
{		
	#pragma message	(TODO("Return the 'i-th' file name extension (i.e. \"3DS\")."))
	return _T("");
}

const TCHAR *[!output CLASS_NAME]::LongDesc()
{
	#pragma message	(TODO("Return long ASCII description (i.e. \"Targa 2.0 Image File\""))
	return _T("");
}
	
const TCHAR *[!output CLASS_NAME]::ShortDesc() 
{			
	#pragma message	(TODO("Return short ASCII description (i.e. \"Targa\")"))
	return _T("");
}

const TCHAR *[!output CLASS_NAME]::AuthorName()
{			
	#pragma message	(TODO("Return ASCII Author name"))
	return _T("");
}

const TCHAR *[!output CLASS_NAME]::CopyrightMessage() 
{	
	#pragma message	(TODO("Return ASCII Copyright message"))
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
	#pragma message	(TODO("Return Version number * 100 (i.e. v3.01 = 301)"))
	return 100;
}

void [!output CLASS_NAME]::ShowAbout(HWND /*window*/)
{			
	// Optional
}

BOOL [!output CLASS_NAME]::SupportsOptions(int /*ext*/, DWORD /*options*/)
{
	// TODO Decide which options to support.  Simply return
	// true for each option supported by each Extension 
	// the exporter supports.

	return TRUE;
}

int [!output CLASS_NAME]::GetSceneNodes(INodeTab& i_nodeTab, INode* i_currentNode /*=NULL*/)
{
	int i;
	if (i_currentNode == nullptr)
	{
		i_currentNode = GetCOREInterface()->GetRootNode();
	}
	else // IGame will crash 3ds Max if it is initialized with the root node.
	{
		i_nodeTab.AppendNode(i_currentNode);
	}
	for (i = 0; i < i_currentNode->NumberOfChildren(); i++)
	{
		GetSceneNodes(i_nodeTab, i_currentNode->GetChildNode(i));
	}
	return i_nodeTab.Count();
}


int	[!output CLASS_NAME]::DoExport(const TCHAR* /*name*/, ExpInterface* /*ei*/,Interface* /*ip*/, BOOL suppressPrompts, DWORD /*options*/)
{
	// This is where the file export operation occur.

	if(!suppressPrompts)
		DialogBoxParam(hInstance, 
				MAKEINTRESOURCE(IDD_PANEL), 
				GetActiveWindow(), 
				[!output CLASS_NAME]OptionsDlgProc, (LPARAM)this);

	// Construct a tab with all this scene's nodes.
	// We could also only take the node currently selected, etc.
	INodeTab lNodes;
	GetSceneNodes(lNodes);

	// Initialize 3DXI (formerly IGame) object
	// For more information, please see 3ds Max SDK topic PG: Programming with 3DXI.
	IGameScene * pIgame = GetIGameInterface();
	pIgame->InitialiseIGame(lNodes);


	#pragma message	(TODO("Initialize and open your file to write"))
	#pragma message	(TODO("Access IGameScene object to extract useful information."))

	#pragma message	(TODO("return TRUE if the file was exported properly."))
	
	return FALSE;
}


 
