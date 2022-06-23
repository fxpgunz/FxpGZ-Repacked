/**********************************************************************
 *<
	FILE:			ApplyVC.cpp
	DESCRIPTION:	Vertex Color Utility + Modifier
	CREATED BY:		Christer Janson
	HISTORY:		Created Monday, June 02, 1997

 *>	Copyright (c) 1997 Kinetix, All Rights Reserved.
 **********************************************************************/

#include "MeshSplit.h"

HINSTANCE hInstance;

BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID lpvReserved)
{
   if( fdwReason == DLL_PROCESS_ATTACH )
   {
      MaxSDK::Util::UseLanguagePackLocale();
      hInstance = hinstDLL;
      DisableThreadLibraryCalls(hInstance);
   }
			
	return (TRUE);
}


__declspec( dllexport ) const TCHAR* LibDescription()
{
	return GetString(IDS_LIBDESCRIPTION);
}

/// MUST CHANGE THIS NUMBER WHEN ADD NEW CLASS
__declspec( dllexport ) int LibNumberClasses()
{
	return 1;
}


__declspec( dllexport ) ClassDesc* LibClassDesc(int i)
{
	switch(i) {
		case 0: return GetSplitMeshesUtilDesc();
		default: return 0;
	}
}

__declspec( dllexport ) ULONG LibVersion()
{
	return VERSION_3DSMAX;
}

__declspec( dllexport ) ULONG CanAutoDefer()
{
	return 1;
}

TCHAR *GetString(int id)
{
	static TCHAR buf[256];

	if (hInstance)
		return LoadString(hInstance, id, buf, _countof(buf)) ? buf : NULL;
	return NULL;
}

