// MatchAgent.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
//

#include "stdafx.h"
#include "MatchAgent.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "MatchAgentDoc.h"
#include "MatchAgentView.h"
#include "OutputView.h"
#include "CommandLogView.h"
#include "MRegistry.h"
#include "MDebug.h"
#include "MSync.h"
#include "Shlwapi.h"
#include "Matchagent.h"
#include "MMatchagent.h"
#include "MDebugUtil.h"
#include "MCrashDump.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define APPLICATION_NAME	"MatchAgent"


// ������ CMatchAgentApp ��ü�Դϴ�.
CMatchAgentApp theApp;
MSingleRunController	g_SingleRunController("AgentServer"); 

// CMatchAgentApp
BEGIN_MESSAGE_MAP(CMatchAgentApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// ǥ�� ������ ���ʷ� �ϴ� ���� ����Դϴ�.
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// ǥ�� �μ� ���� ����Դϴ�.
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
	ON_COMMAND(IDM_CMD_DEBUGPEER, OnCmdDebugpeer)
END_MESSAGE_MAP()


static bool GetRecommandLogFileName(char* pszBuf)
{
	if (PathIsDirectory("Log") == FALSE)
		CreateDirectory("Log", NULL);

	time_t		tClock;
	struct tm*	ptmTime;
	
	time(&tClock);
	ptmTime = localtime(&tClock);

	char szFileName[_MAX_DIR];

	int nFooter = 1;
	while(TRUE) {
		sprintf(szFileName, "Log/AgentLog_%02d-%02d-%02d-%d.txt", 
			ptmTime->tm_year+1900, ptmTime->tm_mon+1, ptmTime->tm_mday, nFooter);

		if (PathFileExists(szFileName) == FALSE)
			break;

		nFooter++;
		if (nFooter > 100) return false;
	}
	strcpy(pszBuf, szFileName);
	return true;
}


// CMatchAgentApp ����
CMatchAgentApp::CMatchAgentApp()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	// InitInstance�� ��� �߿��� �ʱ�ȭ �۾��� ��ġ�մϴ�.
}

// CMatchAgentApp �ʱ�ȭ
BOOL CMatchAgentApp::InitInstance()
{
	if (g_SingleRunController.Create(true) == false)
		return FALSE;

	char szLogFileName[_MAX_DIR];
	if (GetRecommandLogFileName(szLogFileName) == false) 
		return FALSE;

	InitLog(MLOGSTYLE_DEBUGSTRING|MLOGSTYLE_FILE, szLogFileName);

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
	pDocTemplate = new CMultiDocTemplate(IDR_MatchAgentTYPE,
		RUNTIME_CLASS(CMatchAgentDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(COutputView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);
	m_pDocTemplateOutput = pDocTemplate;

	// Template
	m_pDocTemplateCmdLogView = new CMultiDocTemplate(IDR_MatchAgentTYPE,
		RUNTIME_CLASS(CMatchAgentDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CCommandLogView));
//	AddDocTemplate(m_pDocTemplateCmdLogView);

	// �� MDI ������ â�� ����ϴ�.
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
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
	return TRUE;
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
void CMatchAgentApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CMatchAgentApp �޽��� ó����


int CMatchAgentApp::ExitInstance()
{
	// TODO: Add your specialized code here and/or call the base class

	return CWinApp::ExitInstance();
}

void CMatchAgentApp::HeartBeat()
{
	POSITION p = GetFirstDocTemplatePosition(); 
	CDocTemplate* pTemplate = GetNextDocTemplate(p); 
	p = pTemplate->GetFirstDocPosition(); 
	CMatchAgentDoc* pDoc = (CMatchAgentDoc*)pTemplate->GetNextDoc(p); 
	if(pDoc!=NULL) pDoc->Run();
	Sleep(1);
}

double CMatchAgentApp::UpdateFPS()
{
	static DWORD nFrameCount=0;
	nFrameCount++;

	DWORD tmCurrent = timeGetTime();
	static DWORD tmLastUpdate = tmCurrent;

	static double dFPS = 0;

	if (tmCurrent - tmLastUpdate > 1000)	// Update every 1 second
	{
		dFPS = (double)nFrameCount/(double)(tmCurrent-tmLastUpdate)*1000.0f;

		tmLastUpdate = tmCurrent;
		nFrameCount = 0;
	}
	return dFPS;
}

int CMatchAgentApp::Run()
{
	ASSERT_VALID(this);
	_AFX_THREAD_STATE* pState = AfxGetThreadState();

	// acquire and dispatch messages until a WM_QUIT message is received.
	__try
	{
		for (;;)
		{
			m_dFPS = UpdateFPS();

			// phase1: check to see if we can do idle work
			if (::PeekMessage(&(pState->m_msgCur), NULL, NULL, NULL, PM_NOREMOVE))
			{
				if (!PumpMessage())
					return ExitInstance();
			}

			if (m_bShutdown == false)
				HeartBeat();
		}
	}
	__except(CrashExceptionDump(GetExceptionInformation(), "matchagent.dmp"))
	{
		
	}
}

BOOL CMatchAgentApp::PreTranslateMessage(MSG* pMsg)
{
	if(GetKeyState(17)<0)
	{
		if(pMsg->message==WM_KEYDOWN && pMsg->wParam=='C')	// For Crash Test
		{
			return TRUE;
		}
		if(pMsg->message==WM_KEYDOWN && pMsg->wParam=='D') // For UI Debug
		{
			MMatchAgent* pServer = (MMatchAgent*)MMatchAgent::GetInstance();
			pServer->RequestLiveCheck();

			return TRUE;
		}
	}

	return CWinApp::PreTranslateMessage(pMsg);
}

void CMatchAgentApp::OnCmdDebugpeer()
{
	MDebugUtil_PrintStagePeerList();
}
