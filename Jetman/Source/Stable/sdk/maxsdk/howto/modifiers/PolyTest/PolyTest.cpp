/**********************************************************************
 *<
	FILE: PolyTest.cpp

	DESCRIPTION:   Testing tool for Polygon-based Meshes.

	CREATED BY: Steve Anderson

	HISTORY: created January 2004

 *>	Copyright (c) 2004, All Rights Reserved.
 **********************************************************************/

#include "PolyTest.h"
#include "buildver.h"

#include "3dsmaxport.h"

HINSTANCE hInstance;

#define MAX_MOD_OBJECTS	20
ClassDesc *classDescArray[MAX_MOD_OBJECTS];
int classDescCount = 0;

void initClassDescArray(void) {
   if( !classDescCount )
   {
      classDescArray[classDescCount++] = GetPolyTestModDesc();
   }
}

/** public functions **/
BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID lpvReserved) {
   if( fdwReason == DLL_PROCESS_ATTACH )
   {
      MaxSDK::Util::UseLanguagePackLocale();
      hInstance = hinstDLL;
      DisableThreadLibraryCalls(hInstance);
   }
	return(TRUE);
	}


//------------------------------------------------------
// This is the interface to Jaguar:
//------------------------------------------------------

__declspec( dllexport ) const TCHAR * LibDescription() {
	return GetString(IDS_LIBDESCRIPTION);
}

/// MUST CHANGE THIS NUMBER WHEN ADD NEW CLASS
__declspec( dllexport ) int LibNumberClasses() {
   initClassDescArray();

	return classDescCount;
}

// russom - 05/07/01 - changed to use classDescArray
__declspec( dllexport ) ClassDesc* LibClassDesc(int i) {
   initClassDescArray();

   if( i < classDescCount )
		return classDescArray[i];
	else
		return NULL;
}

// Return version so can detect obsolete DLLs
__declspec( dllexport ) ULONG LibVersion() { return VERSION_3DSMAX; }

// Let the plug-in register itself for deferred loading
__declspec( dllexport ) ULONG CanAutoDefer() {
	return 1;
}

INT_PTR CALLBACK DefaultSOTProc (HWND hWnd,
								 UINT msg,WPARAM wParam,LPARAM lParam) {
	IObjParam *ip = DLGetWindowLongPtr<IObjParam*>(hWnd);
	switch (msg) {
	case WM_INITDIALOG:
		DLSetWindowLongPtr(hWnd, lParam);
		break;

	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MOUSEMOVE:
		if (ip) ip->RollupMouseMessage(hWnd,msg,wParam,lParam);
		return FALSE;

	default:
		return FALSE;
	}
	return TRUE;
}

TCHAR *GetString(int id) {
	static TCHAR buf[256];
	if (hInstance) return LoadString(hInstance, id, buf, _countof(buf)) ? buf : NULL;
	return NULL;
}
