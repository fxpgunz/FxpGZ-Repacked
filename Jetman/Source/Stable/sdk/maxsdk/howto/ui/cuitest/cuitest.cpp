/*===========================================================================*\
 |    File: CUITest.cpp
 |
 | Purpose: To test the CUI Toolbar APIs.  This program adds a toolbar
 |          to MAX when BeginEditParams() is called and hides it when 
 |          EndEditParams() is called.
 |
 |          Important Notes:
 |          o This file requires four image files be placed in the MAX EXE\UI_LN\ICONS 
 |            directory.  These are:
 |             CUITest_16a.bmp
 |             CUITest_16i.bmp
 |             CUITest_24a.bmp
 |             CUITest_24i.bmp
 |
 |			NOTE: The above files can be found in the CONTENT directory, off the
 |					root directory of this project.
 |
 |
 | History: Mark Meier, 03/05/99, Began.
 |          MM, 03/14/99, Last Change.
 |			Harry Denholm, 03/28/99, Prep'd for SDK inclusion
 |			Neil Hazzard	07/12/00 R4
 |          Scott Morrison  05/18/01 Removed usage of cached ICUIFrame
 |          Joey Gaspe      08/29/11 Specified the language pack directory
 |          Joey Gaspe      11/15/11 Specified the language neutral directory for some subdirectories of UI (to UI_LN)
 |
\*===========================================================================*/

// Please see the following header file for class information
#include "CUITest.h"
#include "GetCOREInterface.h"
#include "MaxIcon.h"

#include "Qt/QmaxMainWindow.h" 
#include "Qt/QmaxToolBar.h"
#include "Qt/QmaxDockingWinHost.h"
#include "Qt/QmaxDockingWinHostDelegate.h"

/*===========================================================================*\
 | Class Descriptor for the CUITest plugin
\*===========================================================================*/

class CUITestClassDesc:public ClassDesc {
	public:
	int 			IsPublic() {return 1;}
	void *			Create(BOOL loading = FALSE) {return &theCUITest;}
	const TCHAR *	ClassName() {return MY_CLASSNAME;}
	SClass_ID		SuperClassID() {return UTILITY_CLASS_ID;}
	Class_ID		ClassID() {return MY_CLASS_ID;}
	const TCHAR* 	Category() {return MY_CATEGORY;}
};
static CUITestClassDesc CUITestDesc;
ClassDesc* GetCUITestDesc() {return &CUITestDesc;}

/*===========================================================================*\
| Class ToolbarDockingDelegate is the custom docking delegate that will
| process the controls we add.
\*===========================================================================*/

class ToolbarDockingDelegate : public MaxSDK::QmaxProxyDockingWinHostDelegate
{
public:
	ToolbarDockingDelegate(MaxSDK::QmaxDockingWinHostDelegate* baseDelegate)
		: QmaxProxyDockingWinHostDelegate(baseDelegate) {}

	bool winEvent(MSG* msg, long* result) override;

private:
	bool processMessage(UINT message, WPARAM wParam, LPARAM lParam);
};

bool ToolbarDockingDelegate::winEvent(MSG* msg, long* result)
{
	return processMessage(msg->message, msg->wParam, msg->lParam);
}

bool ToolbarDockingDelegate::processMessage(UINT message, WPARAM wParam, LPARAM lParam) {
	switch(message) {
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_TB_0: // Dock or Float
		{
			auto tb = toolbar();
			if(tb->isFloating())
			{
				GetCOREInterface()->GetQmaxMainWindow()->addToolBar(Qt::ToolBarArea::LeftToolBarArea, tb);
			} else
			{
				MaxSDK::QmaxMainWindow::makeToolBarFloating(tb, QCursor::pos());
			}
			return true;
		}
		case ID_TB_1: // This is the check button ... ignore it.
			return true;
		case ID_TB_2:
			MessageBox(NULL, _T("Pressed 2"), _T("ID_TB_2"), MB_OK);
			return true;
		default: // ID not recognized -- use default CUI processing
			return false;
		}
	}
	return false;
}


/*===========================================================================*\
 | The Begin/EndEditParams calls, which create and destroy the toolbar
\*===========================================================================*/

void CUITest::BeginEditParams(Interface *ip,IUtil *iu) {

	if (mToolBar) {
		// We have the toolbar already, just show it...(EndEditParams() hid it)
		mToolBar->setVisible(true);
	}
	else {
		// Create Qt toolbar
		auto mainWindow = GetCOREInterface()->GetQmaxMainWindow();
		mToolBar = new MaxSDK::QmaxToolBar("CUI Test Toolbar", "CUI Test Toolbar", mainWindow);

		// Create docking win host for hosting the legacy CustToolbar
		MaxSDK::QmaxDockingWinHost* winHost = new MaxSDK::QmaxDockingWinHost(mToolBar);
		mToolBar->setWidget(winHost);

		// -- Now create the toolbar window
		HWND hToolbar = CreateWindow(
				CUSTTOOLBARWINDOWCLASS,
				NULL,
				WS_CHILD | WS_VISIBLE,
				0, 0, 250, 100,
				reinterpret_cast<HWND>(winHost->winId()),
				NULL,
				hInstance,
				NULL);

		// -- Now link the toolbar to the CUI frame
		ICustToolbar *iToolBar = GetICustToolbar(hToolbar);

        // Get the 0th icon from the CUITest icon file.
        // Note that the index is 1-based.
        MaxBmpFileIcon* pIcon = new MaxBmpFileIcon(_T("CUITest"), 0+1);
		// -- Toss in a few controls of various sorts...

		// Add a push button
		// This one docks and undocks the toolbar if clicked...
		iToolBar->AddTool(TBITEM(CTB_PUSHBUTTON, pIcon, ID_TB_0));

		// Add a separator
		iToolBar->AddTool(ToolSeparatorItem(8));

		// Add a check button
		// This one doesn't do anything except change to blue when on...
        pIcon = new MaxBmpFileIcon(_T("CUITest"), 1+1);
		iToolBar->AddTool(TBITEM(CTB_CHECKBUTTON, pIcon, ID_TB_1));
		
		// Add a separator
		iToolBar->AddTool(ToolSeparatorItem(8));

		// Add a keyboard macro command.  In this case the Time Configuration dialog
		// is presented as an example.  In order to get the proper command ID to use
		// we must search through the names in the MAX UI action table and find
		// a match.  The list of names can be reviewed in the MAX UI when setting up
		// a keyboard shortcut.
		TSTR findName = _T("Time Configuration");
        ActionTable *pTable = ip->GetActionManager()->FindTable(kActionMainUI);
        assert(pTable);
        for(int i = 0; i < pTable->Count(); i++) {
            ActionItem* pAction = (*pTable)[i];
            TSTR desc;
            pAction->GetDescriptionText(desc);
			if (_tcscmp(findName.data(), desc.data()) == 0) {
                int cmdID = pAction->GetId();
				MacroButtonData md1(kActionMainUI, cmdID, _T("KBD Cmd"), _T("Key Macro Tooltip"));
				iToolBar->AddTool(TBMACRO(&md1));
				break;
			}
		}

		// Add a separator
		iToolBar->AddTool(ToolSeparatorItem(8));

		// This button is processed by our message handler (ID_TB_2)
        pIcon = new MaxBmpFileIcon(_T("CUITest"), 2+1);
		iToolBar->AddTool(TBITEM(CTB_PUSHBUTTON, pIcon, ID_TB_2));

		//Create a custom docking delegate which wraps the default docking delegate.
		//This custom delegate will process the controls we added.
		mDockingDelegate = new ToolbarDockingDelegate(iToolBar->GetDockingDelegate());
		winHost->setDockingDelegate(mDockingDelegate);
		winHost->setHostedWindow(hToolbar);

		// need to be added once to the mainwindow toolbar layout before we can float it
		mainWindow->addToolBar(Qt::ToolBarArea::TopToolBarArea, mToolBar);
		MaxSDK::QmaxMainWindow::makeToolBarFloating(mToolBar, QCursor::pos());

		// We are done, release the toolbar
		ReleaseICustToolbar(iToolBar);
	}
}
	
void CUITest::EndEditParams(Interface *ip,IUtil *iu) {
	// Hide the toolbar so the user can't execute anything 
	// when we're not active.
    if (mToolBar) {
		mToolBar->setVisible(false);
    }
}

