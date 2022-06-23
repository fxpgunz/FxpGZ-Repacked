/**********************************************************************
 *<
	FILE: DllEntry.cpp

	DESCRIPTION:Contains the Dll Entry stuff

	CREATED BY: 

	HISTORY: 

 *>	Copyright (c) 1997, All Rights Reserved.
 **********************************************************************/
#include "DynPBlock.h"

extern ClassDesc2* GetDynPBlockDesc();
extern ClassDesc2* GetSwatchAttribDesc();
extern ClassDesc2* GetSimpleAttribDesc();
extern ClassDesc2* GetNodeAttribDesc();
extern ClassDesc2* GetMatCustAttribDesc();

HINSTANCE hInstance;

BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID lpvReserved)
{
   if( fdwReason == DLL_PROCESS_ATTACH )
   {
      MaxSDK::Util::UseLanguagePackLocale();
      hInstance = hinstDLL;				// Hang on to this DLL's instance handle.
      DisableThreadLibraryCalls(hInstance);
   }
			
	return (TRUE);
}

__declspec( dllexport ) const TCHAR* LibDescription()
{
	return GetString(IDS_LIBDESCRIPTION);
}

//TODO: Must change this number when adding a new class
__declspec( dllexport ) int LibNumberClasses()
{
	return 5;
}

__declspec( dllexport ) ClassDesc* LibClassDesc(int i)
{
	switch(i) {
		case 0: return GetDynPBlockDesc();
		case 1: return GetMatCustAttribDesc();
		case 2: return GetSimpleAttribDesc();
		case 3: return GetSwatchAttribDesc();
		case 4: return GetNodeAttribDesc();
		default: return 0;
	}
}

__declspec( dllexport ) ULONG LibVersion()
{
	return VERSION_3DSMAX;
}

TCHAR *GetString(int id)
{
	static TCHAR buf[256];

	if (hInstance)
		return LoadString(hInstance, id, buf, _countof(buf)) ? buf : NULL;
	return NULL;
}

