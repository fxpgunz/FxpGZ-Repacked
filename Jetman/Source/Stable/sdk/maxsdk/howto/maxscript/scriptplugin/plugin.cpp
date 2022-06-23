/*===========================================================================*\
 | 
 |  FILE:	Plugin.cpp
 |			Utility that accesses a scripted plugin's parameters
 |			Demonstrates SDK -> MAX Script plugins techniques
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 7-4-99
 | 
\*===========================================================================*/

#include "Utility.h"



HINSTANCE hInstance;

BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID lpvReserved) {
   if( fdwReason == DLL_PROCESS_ATTACH )
   {
      MaxSDK::Util::UseLanguagePackLocale();
      hInstance = hinstDLL;
      DisableThreadLibraryCalls(hInstance);
   }
	return(TRUE);
}



__declspec( dllexport ) const TCHAR *
LibDescription() 
{
	static TSTR libDescription (MaxSDK::GetResourceStringAsMSTR(IDS_LIBDESC));
	return libDescription;
}


__declspec( dllexport ) int LibNumberClasses() 
{
	return 1;
}


__declspec( dllexport ) ClassDesc* LibClassDesc(int i) 
{
	switch(i) {
		case 0: return GetSCPUtilDesc();
		default: return 0;
	}
}


__declspec( dllexport ) ULONG LibVersion() { return VERSION_3DSMAX; }
