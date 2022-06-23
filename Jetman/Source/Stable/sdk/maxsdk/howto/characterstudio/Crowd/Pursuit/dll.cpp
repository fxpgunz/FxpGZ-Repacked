//*********************************************************************
//	Crowd / Unreal Pictures / dll.cpp
//	Copyright (c) 2000, All Rights Reserved.
//*********************************************************************

#include "dll.h"
#include "resource.h"

HINSTANCE hInstance = NULL;
int messagesent = FALSE;
#define MAX_PATH_LENGTH 257
#define MAX_STRING_LENGTH 256
int triedToLoad = FALSE;
;

TCHAR *GetString(int id)
{
	static TCHAR buf[256];

	if (hInstance)
		return LoadString(hInstance, id, buf, _countof(buf)) ? buf : NULL;
	return NULL;
}

/** public functions **/
BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID lpvReserved) {
	switch(fdwReason) {
		case DLL_PROCESS_ATTACH:
			MaxSDK::Util::UseLanguagePackLocale();
			hInstance = hinstDLL;
			DisableThreadLibraryCalls(hInstance);
			break;
		case DLL_PROCESS_DETACH:
			break;
		}
	return(TRUE);
	}


//------------------------------------------------------
// Dll Library Functions
//------------------------------------------------------

__declspec( dllexport ) const TCHAR* LibDescription() 
{ 
   return GetString(IDS_LIBDESC); 
}

__declspec( dllexport ) int LibNumberClasses()
{
	return 1;
}

__declspec( dllexport ) ClassDesc* LibClassDesc(int i)
{
	switch(i) {
		case 0:  return GetPursuitBhvrDesc();
		default: return 0;
	}
}

__declspec( dllexport ) ULONG LibVersion()
{
	return VERSION_3DSMAX;
}
