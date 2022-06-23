/*===========================================================================*\
 |  File: CUITest.h
 |	Header file for the CUITest Utility. Check CUITest.cpp for notes and information.
 |
\*===========================================================================*/

#include "Max.h"
#include "resource.h"
#include "utilapi.h"
#include "ActionTable.h" // This is for the keyboard shortcut APIs


TCHAR *GetString(int id);
extern ClassDesc* GetCUITestDesc();

/*===========================================================================*\
 | Misc defines, etc.
\*===========================================================================*/

#define MY_CATEGORY _T("How To")
#define MY_CLASSNAME _T("CUITester")
#define MY_CLASS_ID Class_ID(0x227d9df3, 0x999895ed)

/*===========================================================================*\
 | Definitions for the toolbar items themselves
\*===========================================================================*/

#define TBITEM(type, pIcon, cmd) \
		ToolButtonItem(type,pIcon,GetCUIFrameMgr()->GetImageSize(),GetCUIFrameMgr()->GetImageSize(),GetCUIFrameMgr()->GetButtonWidth(),GetCUIFrameMgr()->GetButtonHeight(),cmd,0)
#define TBMACRO(md) \
		ToolMacroItem(0, GetCUIFrameMgr()->GetButtonHeight(), md)

// Here a large value is used so it won't conflict with the IDs used
// by MAX.
#define ID_TB_0 10000
#define ID_TB_1 10001
#define ID_TB_2 10002

class ToolbarDockingDelegate;
namespace MaxSDK
{
	class QmaxToolBar;
}

/*===========================================================================*\
 | This is the main plug-in class
\*===========================================================================*/
class CUITest : public UtilityObj {
  public:
	CUITest() {};
	~CUITest() override {};

	void BeginEditParams(Interface *ip, IUtil *iu) override;
	void EndEditParams(Interface *ip, IUtil *iu) override;
	void DeleteThis() override  {}

private:
	ToolbarDockingDelegate* mDockingDelegate = nullptr;
	MaxSDK::QmaxToolBar* mToolBar = nullptr;
};

static CUITest theCUITest;
