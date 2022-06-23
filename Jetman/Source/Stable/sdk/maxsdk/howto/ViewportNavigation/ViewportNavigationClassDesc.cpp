/**********************************************************************
 *<
	FILE: ViewportNavigationClassDesc.cpp

	DESCRIPTION:	Appwizard generated plugin

	CREATED BY: 

	HISTORY: 

 *>	Copyright (c) 2003, All Rights Reserved.
 **********************************************************************/

#include "ViewportNavigator.h"


class ViewportNavigatorClassDesc : public ClassDesc2 
{
	public:
		int IsPublic() { return TRUE; }
		void* Create(BOOL loading = FALSE) { return &ViewportNavigator::GetInstance();}
		const TCHAR* ClassName() { return GetString(IDS_CLASS_NAME); }
		SClass_ID SuperClassID() { return UTILITY_CLASS_ID; }
		Class_ID ClassID() { return ViewportNavigator::GetClassID(); }
		const TCHAR* Category() { return GetString(IDS_CATEGORY); }

		// returns fixed parsable name (scripter-visible name)
		const TCHAR* InternalName() { return _T("ViewportNavigator"); }	
		// returns owning module handle
		HINSTANCE HInstance() { return hInstance; }					
};

ClassDesc2* GetViewportNavigatorDesc() 
{ 
	static ViewportNavigatorClassDesc ViewportNavigatorDesc;
	return &ViewportNavigatorDesc; 
}

