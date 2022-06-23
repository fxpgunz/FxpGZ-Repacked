#include <maxscript/maxscript.h>
#include "resource.h"

extern void tester_init();

HMODULE hInstance = NULL;

BOOL APIENTRY
DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			MaxSDK::Util::UseLanguagePackLocale();
			// Hang on to this DLL's instance handle.
			hInstance = hModule;
			DisableThreadLibraryCalls(hInstance);
			break;
	}
		
	return(TRUE);
}

__declspec( dllexport ) void
LibInit() { 
	// do any setup here
	tester_init();
}


__declspec( dllexport ) const TCHAR *
LibDescription() 
{ 
	static TSTR libDescription (MaxSDK::GetResourceStringAsMSTR(IDS_LIBDESCRIPTION)); 
	return libDescription;
}

__declspec( dllexport ) ULONG
LibVersion() {  return VERSION_3DSMAX; }

