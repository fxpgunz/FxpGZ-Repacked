//-----------------------------------------------------------------------------
// Notify.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include <direct.h>
#include "CNotifyApp.h"
#include "notifdlg.h"
#include <maxnet_job.h>
#include <Util\IniUtil.h> // MaxSDK::Util::WritePrivateProfileString

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//-----------------------------------------------------------------------------
// CNotifyApp

BEGIN_MESSAGE_MAP(CNotifyApp, CWinApp)
	//{{AFX_MSG_MAP(CNotifyApp)
	//}}AFX_MSG
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
// CNotifyApp construction

CNotifyApp::CNotifyApp() {
	
	//-- Set where the .INI file is located
	
	TCHAR szCurDir[_MAX_DIR];
	_tgetcwd(szCurDir,_MAX_DIR);
	m_sINIname.Format(_T("%s\\Notify.ini"),szCurDir);

}

//-----------------------------------------------------------------------------
// CNotifyApp destruction

CNotifyApp::~CNotifyApp() {

}

//-----------------------------------------------------------------------------
// The one and only CNotifyApp object

CNotifyApp	theApp;
CNotifyApp* theAppPtr = &theApp;

//-----------------------------------------------------------------------------
// CNotifyApp initialization

BOOL CNotifyApp::InitInstance() {

	AfxEnableControlContainer();

	#ifndef _AFXDLL
	Enable3dControlsStatic();	// Call this when linking to MFC statically
	#endif

	//-------------------------------------------------------------------------
	//-- Load INI Configuration

	TCHAR buffer[_MAX_DIR];
	
	GetPrivateProfileString(iniMAILTARGETS, iniFAILURE,_T(""),
		(TCHAR*)&buffer,_countof(buffer),m_sINIname);
	if (buffer[0])
		failureTarget = buffer;
	GetPrivateProfileString(iniMAILTARGETS, iniPROGRESS,_T(""),
		(TCHAR*)&buffer,_countof(buffer),m_sINIname);
	if (buffer[0])
		progressTarget = buffer;
	GetPrivateProfileString(iniMAILTARGETS, iniCOMPLETION,_T(""),
		(TCHAR*)&buffer,_countof(buffer),m_sINIname);
	if (buffer[0])
		completionTarget = buffer;

	//-------------------------------------------------------------------------
	//-- Check Command Line Arguments

	CString	alertFile;

	if (__argc == 3) {
		
		//-- Get Arguments --------------------------------
		
		alertFile	= __argv[1];
		alertFlags	= (DWORD)(atoi(__argv[2]));
	
		//-- Setup Email Notification ---------------------

		CString notifyType;

		switch (alertFlags) {
			case NOTIFY_FAILURE:
				sndPlaySound(failureTarget,SND_SYNC);
				break;
			case NOTIFY_PROGRESS:
				sndPlaySound(progressTarget,SND_SYNC);
				break;
			case NOTIFY_COMPLETION:
				sndPlaySound(completionTarget,SND_SYNC);
				break;
		}

		return FALSE;
	
	}

	//-------------------------------------------------------------------------
	//-- Standard initialization (Running Interactively)

	CNotifyDlg	dlg;
	m_pMainWnd	= &dlg;
	
	dlg.m_CompletionTarget	= completionTarget;
	dlg.m_ProgressTarget	= progressTarget;
	dlg.m_FailureTarget		= failureTarget;

	if (dlg.DoModal() == IDOK) {

		//-- Save Settings

		MaxSDK::Util::WritePrivateProfileString(iniMAILTARGETS, iniFAILURE,
			dlg.m_FailureTarget, m_sINIname);
		MaxSDK::Util::WritePrivateProfileString(iniMAILTARGETS, iniPROGRESS,
			dlg.m_ProgressTarget, m_sINIname);
		MaxSDK::Util::WritePrivateProfileString(iniMAILTARGETS, iniCOMPLETION,
			dlg.m_CompletionTarget, m_sINIname);
	}

	//-- Since the dialog has been closed, return FALSE so that we exit the
	//-- application, rather than start the application's message pump.
	
	return FALSE;

}

