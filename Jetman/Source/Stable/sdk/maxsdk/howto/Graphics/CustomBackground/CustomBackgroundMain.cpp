/*===========================================================================*\
 |    File: CustomBackgroundMain.cpp
 |
 | Purpose: Demonstrate custom background in 3ds max viewport.
 |
 | History: Qilin Ren, Began 2014/09/01.
\*===========================================================================*/

#include "CustomBackground.h"
#include "CustomBackgroundMain.h"
#include "Resource.h"

HINSTANCE ghInstance;

#define CUSTOM_BACKGROUND_CLASS_ID Class_ID(0x6298742b, 0x531e13b3)

class CustomBackgroundDesc : public ClassDesc2
{
public:
	int 			IsPublic() {return TRUE;}
	void*			Create(BOOL loading)
	{
		loading;
		return new MaxSDK::Graphics::CustomBackground;
	}
	const MCHAR *	ClassName() { return _M("CustomBackground"); }
	SClass_ID		SuperClassID() { return Fragment_CLASS_ID; }
	Class_ID		ClassID() { return CUSTOM_BACKGROUND_CLASS_ID;}
	const MCHAR* 	Category() { return _M("Fragment"); }
};

static CustomBackgroundDesc theCustomBackgroundDesc;

ClassDesc2* GetCustomBackgroundDesc()
{
	return &theCustomBackgroundDesc;
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
		return GetCustomBackgroundDesc();
	default:
		return NULL;
	}
}

namespace MaxSDK { namespace Graphics {

	Class_ID CustomBackground::GetClassID() const
	{
		return CUSTOM_BACKGROUND_CLASS_ID;
	}

}}
