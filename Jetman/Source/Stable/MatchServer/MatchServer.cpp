// MatchServer.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
//

#include "stdafx.h"
#include "MatchServer.h"
#include "MainFrm.h"

#include <shlwapi.h>

#include "ChildFrm.h"
#include "MatchServerDoc.h"
#include "MatchServerView.h"
#include "OutputView.h"
#include "CommandLogView.h"
#include "MRegistry.h"
#include "matchserver.h"
#include "MBMatchServer.h"
#include "MDebug.h"
#include "MSync.h"
#include "MMatchConfig.h"
#include "MTraceMemory.h"
#include "MMatchCheckLoopTime.h"
#include "MMatchStatus.h"

#ifdef _DEBUG
#define new DEBUG_NEW

// �̰��� �׽�Ʈ��..bird
#define _FETCH_112

#endif


#ifdef _FETCH_112
	#include "MInet.h"
	MHttp g_Http;
#endif

#define APPLICATION_NAME	"MatchServer"

// ������ CMatchServerApp ��ü�Դϴ�.
CMatchServerApp			theApp;
MSingleRunController	g_SingleRunController("MatchServer"); 


// CMatchServerApp

BEGIN_MESSAGE_MAP(CMatchServerApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// ǥ�� ������ ���ʷ� �ϴ� ���� ����Դϴ�.
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// ǥ�� �μ� ���� ����Դϴ�.
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
	ON_COMMAND(ID_ViewServerStatus, OnViewServerStatus)
	ON_COMMAND(ID_MESSAGE_EXIT, OnMessageExit)
	ON_COMMAND(ID_SHOW_CMD_LOG, OnShowCmdLog)
	ON_COMMAND(ID_USE_COUNTRY_FILTER, OnSetUseCountryFilter)
	ON_COMMAND(ID_ACCEPT_INVAILD_IP, OnSetAccetpInvalidIP)
	ON_COMMAND(ID_UPDATE_IPtoCOUNTRY, OnUpdateIPtoCountry)
	ON_COMMAND(ID_UPDATE_BLOCK_COUNTRY_CODE, OnUpdateBlockCountryCode)
	ON_COMMAND(ID_UPDATE_CUSTOM_IP, OnUpdateCustomIP)
	ON_UPDATE_COMMAND_UI(ID_SHOW_CMD_LOG, OnUpdateShowCmdLog)
	ON_UPDATE_COMMAND_UI(ID_USE_COUNTRY_FILTER, OnUseCountryFilter)
	ON_UPDATE_COMMAND_UI(ID_ACCEPT_INVAILD_IP, OnAcceptInvalidIP)
	ON_UPDATE_COMMAND_UI(ID_UPDATE_IPtoCOUNTRY, OnEnableUpdateIPtoCountry)
	ON_UPDATE_COMMAND_UI(ID_UPDATE_BLOCK_COUNTRY_CODE, OnEnableUpdateBlockCountryCode)
	ON_UPDATE_COMMAND_UI(ID_UPDATE_CUSTOM_IP, OnEnableUpdateCustomIP)
	ON_COMMAND(ID_TOOL_TEST, OnToolTest)
END_MESSAGE_MAP()



// CMatchServerApp ����
CMatchServerApp::CMatchServerApp()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	// InitInstance�� ��� �߿��� �ʱ�ȭ �۾��� ��ġ�մϴ�.

	m_bTodayRankingRequestDone = false;
}

CMatchServerApp::~CMatchServerApp()
{
#ifdef _MTRACEMEMORY
	MShutdownTraceMemory();
#endif

	if (m_pDocTemplateCmdLogView)
	{
		delete m_pDocTemplateCmdLogView;
	}
}

// CMatchServerApp �ʱ�ȭ
BOOL CMatchServerApp::InitInstance()
{
#ifdef _MTRACEMEMORY
	MInitTraceMemory();
#endif
//	_CrtSetBreakAlloc(206319);

	m_bOutputLog = 0;

//	MNewMemories::Init();

#ifdef _FETCH_112
	g_Http.Create();
#endif


	if (g_SingleRunController.Create(true) == false)
		return FALSE;


	MRegistry::szApplicationName=APPLICATION_NAME;

	if(m_ZFS.Create(".")==false){
		AfxMessageBox("MAIET Zip File System Initialize Error");
		return FALSE;
	}

	// ���� ���α׷� �Ŵ��佺Ʈ�� ComCtl32.dll ���� 6 �̻��� ����Ͽ� ���־� ��Ÿ����
	// ����ϵ��� �����ϴ� ���, Windows XP �󿡼� �ݵ�� InitCommonControls()�� �ʿ��մϴ�. 
	// InitCommonControls()�� ������� ������ â�� ���� �� �����ϴ�.
	InitCommonControls();

	CWinApp::InitInstance();

	// OLE ���̺귯���� �ʱ�ȭ�մϴ�.
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// ǥ�� �ʱ�ȭ
	// �̵� ����� ������� �ʰ� ���� ���� ������ ũ�⸦ ���̷���
	// �Ʒ����� �ʿ� ���� Ư�� �ʱ�ȭ ��ƾ�� �����ؾ� �մϴ�.
	// �ش� ������ ����� ������Ʈ�� Ű�� �����Ͻʽÿ�.
	// TODO: �� ���ڿ��� ȸ�� �Ǵ� ������ �̸��� ����
	// ������ �������� �����ؾ� �մϴ�.
	SetRegistryKey(_T("���� ���� ���α׷� �����翡�� ������ ���� ���α׷�"));
	LoadStdProfileSettings(4);  // MRU�� �����Ͽ� ǥ�� INI ���� �ɼ��� �ε��մϴ�.
	// ���� ���α׷��� ���� ���ø��� ����մϴ�. ���� ���ø���
	// ����, ������ â �� �� ������ ���� ������ �մϴ�.
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_MatchServerTYPE,
		RUNTIME_CLASS(CMatchServerDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(COutputView));
	AddDocTemplate(pDocTemplate);

	m_pDocTemplateOutput = pDocTemplate;

	// Template
	m_pDocTemplateOutput = pDocTemplate;
	m_pDocTemplateCmdLogView = new CMultiDocTemplate(IDR_MatchServerTYPE,
		RUNTIME_CLASS(CMatchServerDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CCommandLogView));


	// �� MDI ������ â�� ����ϴ�.
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;
	// ���̻簡 ���� ��쿡�� DragAcceptFiles�� ȣ���մϴ�.
	// MDI ���� ���α׷������� m_pMainWnd�� ������ �� �ٷ� �̷��� ȣ���� �߻��ؾ� �մϴ�.
	// ǥ�� �� ���, DDE, ���� ���⿡ ���� ������� ���� �м��մϴ�.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	// ����ٿ� ������ ����� ����ġ�մϴ�. ���� ���α׷��� /RegServer, /Register, /Unregserver �Ǵ� /Unregister�� ���۵� ��� FALSE�� ��ȯ�մϴ�.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// �� â�� �ʱ�ȭ�Ǿ����Ƿ� �̸� ǥ���ϰ� ������Ʈ�մϴ�.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	pMainFrame->m_wndConsoleBar.GetDlgItem(IDC_COMBO_COMMAND)->SetFocus();
	//pMainFrame->m_wndConsoleBar.ShowWindow(SW_HIDE);


	// ����� ����϶��� �ߴ� â�� �������Ƿ� �Ⱥ��̰� �Ѵ�.
#ifdef _DEBUG
	m_pMainWnd->ShowWindow(SW_HIDE);
#endif

	return TRUE;
}

// CMatchServerApp �޽��� ó����
int CMatchServerApp::ExitInstance()
{
#ifdef _FETCH_112
	g_Http.Destroy();
#endif

	// TODO: Add your specialized code here and/or call the base class
	return CWinApp::ExitInstance();
}

void CMatchServerApp::HeartBeat()
{
	POSITION p = GetFirstDocTemplatePosition(); 
	CDocTemplate* pTemplate = GetNextDocTemplate(p); 
	p = pTemplate->GetFirstDocPosition(); 
	CMatchServerDoc* pDoc = (CMatchServerDoc*)pTemplate->GetNextDoc(p); 
	if(pDoc!=NULL) pDoc->Run();
	Sleep(1);


#ifdef _FETCH_112
	unsigned long int nNowTime=timeGetTime();
	static unsigned long int nLastTime = 0;
	//if ((nNowTime - nLastTime) >= (1000 * 60 * 5))		// 5�и��� �ѹ��� fetch
	if ((nNowTime - nLastTime) >= (1000 * 60  * 1))		// 5�и��� �ѹ��� fetch
	{
		g_Http.Query("http://192.168.0.31:8080/112.html?mode=fetch");
		nLastTime = nNowTime;
	}
#endif

}

int CMatchServerApp::Run()
{
	ASSERT_VALID(this);
	_AFX_THREAD_STATE* pState = AfxGetThreadState();

	// acquire and dispatch messages until a WM_QUIT message is received.
	for (;;)
	{
		MGetCheckLoopTimeInstance()->SetStartLoop();

		// phase1: check to see if we can do idle work
		if (::PeekMessage(&(pState->m_msgCur), NULL, NULL, NULL, PM_NOREMOVE))
		{
			if (!PumpMessage())
				return ExitInstance();
		}

		if (m_bShutdown == false)
			HeartBeat();
		else
			break;

		MGetCheckLoopTimeInstance()->SetEndLoop();

		RequestDBRankingList();

		if ((MGetServerConfig()->IsUseLoopLog() == true) && (MGetServerConfig()->GetLoopTimeGap() != 0))
		{
			if (MGetCheckLoopTimeInstance()->GetLoopTimeGap() > MGetServerConfig()->GetLoopTimeGap())
			{
				MGetCheckLoopTimeInstance()->SaveLoopLogFile();
			}
		}
	}
	return 0;
}

SYSTEMTIME g_systemTime;
void CMatchServerApp::RequestDBRankingList()
{
	// �Ϸ翡 1ȸ DB���� �����̹� ��ŷ ������ ��û�Ѵ� (DB���� �Ϸ� 1ȸ ��ŷ ������ �ϱ� ����)
	// ��û	�ð�
	int HOUR = MGetServerConfig()->GetSurvivalRankingDalyRequestTimeHour();
	int MIN = MGetServerConfig()->GetSurvivalRankingDalyRequestTimeMinute();

	// '������ ��û �Ϸ�' �÷��׸� �� �ð��� ����� ���� ���� 0�� 0������ �����Ǿ �ȵȴ�
	// 0�� 5������ �����ش�
	if (HOUR == 0 && MIN == 0) {
		MIN = 5;
	}

	::GetLocalTime(&g_systemTime);
	if (g_systemTime.wHour > HOUR || (g_systemTime.wHour==HOUR && g_systemTime.wMinute>=MIN))
	{
		// ������ ������Ʈ �ð��� �ƴٸ�(Ȥ�� �����ٸ�) �ѹ��� DB�� ��û��
		if (!m_bTodayRankingRequestDone)
		{
			mlog("Daily Survival Ranking Request [month%d day%d hour%d min%d]\n", g_systemTime.wMonth, g_systemTime.wDay, g_systemTime.wHour, g_systemTime.wMinute);

			MMatchServer::GetInstance()->OnRequestSurvivalModeGroupRanking();
			m_bTodayRankingRequestDone = true;
		}
	}
	else
	{
		m_bTodayRankingRequestDone = false;
	}
}

#include "MMatchStatus.h"
#include ".\matchserver.h"

void CMatchServerApp::OnViewServerStatus()
{
	// TODO: Add your command handler code here
	MMatchServer* pServer = MMatchServer::GetInstance();
	if (pServer) pServer->Log(MCommandCommunicator::LOG_PROG, "�������º���");

	POSITION p = GetFirstDocTemplatePosition(); 
	CDocTemplate* pTemplate = GetNextDocTemplate(p); 
	p = pTemplate->GetFirstDocPosition(); 
	CMatchServerDoc* pDoc = (CMatchServerDoc*)pTemplate->GetNextDoc(p); 

	if(pDoc!=NULL) 
	{
		pDoc->m_pMatchServer->OnViewServerStatus();
//		MNewMemories::Dump();
		MGetServerStatusSingleton()->Dump();

#ifdef _CMD_PROFILE
		pDoc->m_pMatchServer->m_CommandProfiler.Analysis();
#endif
	}
	
}

BOOL CMatchServerApp::PreTranslateMessage(MSG* pMsg)
{
	if(GetKeyState(17)<0)
	{
		if(pMsg->message==WM_KEYDOWN && pMsg->wParam=='P')
		{
			OnViewServerStatus();
			return TRUE;
		}
		if(pMsg->message==WM_KEYDOWN && pMsg->wParam=='C')	// For Crash Test
		{
			return TRUE;
		}
		if(pMsg->message==WM_KEYDOWN && pMsg->wParam=='D') // For UI Debug
		{
			MBMatchServer* pServer = (MBMatchServer*)MMatchServer::GetInstance();
			CRichEditCtrl& c = pServer->m_pView->GetRichEditCtrl();

			return TRUE;
		}
	}

	return CWinApp::PreTranslateMessage(pMsg);
}


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ��ȭ ���� ������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ����

// ����
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// ��ȭ ���ڸ� �����ϱ� ���� ���� ���α׷� ����Դϴ�.
void CMatchServerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}
void CMatchServerApp::OnMessageExit()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	MMatchServer* pServer = MMatchServer::GetInstance();
	if(pServer)	{
		pServer->Log(MCommandCommunicator::LOG_PROG, "OnMessageExit - Stop Server");
		pServer->OnAdminServerHalt();		
	}
}

void CMatchServerApp::OnShowCmdLog()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	m_bOutputLog = 1-(int)m_bOutputLog;
}

void CMatchServerApp::OnUpdateShowCmdLog(CCmdUI *pCmdUI)
{
	// TODO: ���⿡ ��� ������Ʈ UI ó���� �ڵ带 �߰��մϴ�.
	pCmdUI->SetCheck(m_bOutputLog);
}

void CMatchServerApp::OnUpdateIPtoCountry()
{
	MMatchServer* pServer = MMatchServer::GetInstance();
	if(pServer)	
		pServer->UpdateIPtoCountryList();
}


void CMatchServerApp::OnUpdateBlockCountryCode()
{
	MMatchServer* pServer = MMatchServer::GetInstance();
	if(pServer)	
		pServer->UpdateBlockCountryCodeLsit();
}


void CMatchServerApp::OnUpdateCustomIP()
{
	MMatchServer* pServer = MMatchServer::GetInstance();
	if(pServer)	
		pServer->UpdateCustomIPList();
}


void CMatchServerApp::OnUseCountryFilter(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( MGetServerConfig()->IsUseFilter() );
}


void CMatchServerApp::OnSetUseCountryFilter()
{
	MMatchServer* pServer = MMatchServer::GetInstance();
	if(pServer)	
		pServer->SetUseCountryFilter();
}


void CMatchServerApp::OnSetAccetpInvalidIP()
{
	MMatchServer* pServer = MMatchServer::GetInstance();
	if(pServer)	
		pServer->SetAccetpInvalidIP();
}


void CMatchServerApp::OnAcceptInvalidIP(CCmdUI* pCmdUI )
{
	// pCmdUI->Enable( MGetServerConfig()->IsUseFilter() );
	pCmdUI->SetCheck( MGetServerConfig()->IsAcceptInvalidIP() );
}


void CMatchServerApp::OnEnableUpdateIPtoCountry( CCmdUI* pCmdUI )
{
	pCmdUI->Enable( MGetServerConfig()->IsUseFilter() );
}


void CMatchServerApp::OnEnableUpdateBlockCountryCode( CCmdUI* pCmdUI )
{
	pCmdUI->Enable( MGetServerConfig()->IsUseFilter() );
}


void CMatchServerApp::OnEnableUpdateCustomIP( CCmdUI* pCmdUI )
{
	pCmdUI->Enable( MGetServerConfig()->IsUseFilter() );
}
void CMatchServerApp::OnToolTest()
{
	MMatchServer* pServer = MMatchServer::GetInstance();
	if(pServer)	{
		//MAgentObject *pObj = pServer->FindFreeAgent();		
		//if( pObj == NULL ) return;
		//pServer->Disconnect(pObj->GetUID());
		//pServer->SetUID(MUID(0, 9741612));
	}
}
