/*===========================================================================*\
 |    File: SimDesktopMain.cpp
 |
 | Purpose: Demonstrate how to display overlay (e.g. UI) in 3ds max viewport.
 |
 | History: Qilin Ren, Began 2014/09/01.
\*===========================================================================*/

#include "SimDesktop.h"
#include "SimDesktopMain.h"
#include "Resource.h"

HINSTANCE ghInstance;

#define SIM_DESKTOP_CLASS_ID Class_ID(0x12c32f8f, 0xc4ab704e)

class SimDesktopDesc : public ClassDesc2
{
public:
	int 			IsPublic() {return TRUE;}
	void*			Create(BOOL loading)
	{
		loading;
		return new MaxSDK::Graphics::SimDesktop;
	}
	const MCHAR *	ClassName() { return _M("SimDesktop"); }
	SClass_ID		SuperClassID() { return Fragment_CLASS_ID; }
	Class_ID		ClassID() { return SIM_DESKTOP_CLASS_ID;}
	const MCHAR* 	Category() { return _M("Fragment"); }
};

static SimDesktopDesc oneSimDesktopDesc;

ClassDesc2* GetSimDesktopDesc()
{
	return &oneSimDesktopDesc;
}

/** public functions **/
BOOL WINAPI DllMain(
	HINSTANCE hinstDLL, 
	ULONG fdwReason, 
	LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		MaxSDK::Util::UseLanguagePackLocale();
		ghInstance = hinstDLL;
		DisableThreadLibraryCalls(ghInstance);
	}

	return (TRUE);
}

//------------------------------------------------------
// This is the interface to Max:
//------------------------------------------------------

// Return version so can detect obsolete DLLs
__declspec( dllexport ) ULONG LibVersion()
{
	return VERSION_3DSMAX;
}

TCHAR *GetString(int id)
{
	static TCHAR buf[256];
	if (ghInstance != NULL)
	{
		return LoadString(ghInstance, id, buf, _countof(buf)) ? buf : NULL;
	}

	return NULL;
}

__declspec( dllexport ) const TCHAR * LibDescription()
{
	return GetString(IDS_LIBDESCRIPTION);
}

// MUST CHANGE THIS NUMBER WHEN ADD NEW CLASS
__declspec( dllexport ) int LibNumberClasses() 
{
	return 1;
}

__declspec( dllexport ) ClassDesc* LibClassDesc(int i) 
{
	switch(i)
	{
	case 0:
		return GetSimDesktopDesc();
	default:
		return NULL;
	}
}

namespace MaxSDK { namespace Graphics {

	Class_ID SimDesktop::GetClassID() const
	{
		return SIM_DESKTOP_CLASS_ID;
	}

}}
