// Locator.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
//

#include "stdafx.h"
#include "Locator.h"
#include "MainFrm.h"

#include "LocatorDoc.h"
#include "LocatorView.h"
#include ".\locator.h"
#include "MDebug.h"
#include "MLocatorConfig.h"
#include "MLocatorView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLocatorApp

BEGIN_MESSAGE_MAP(CLocatorApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// ǥ�� ������ ���ʷ� �ϴ� ���� ����Դϴ�.
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// ǥ�� �μ� ���� ����Դϴ�.
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()


// CLocatorApp ����

CLocatorApp::CLocatorApp() : m_dwViewUpdatedTime( timeGetTime() ) 
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	// InitInstance�� ��� �߿��� �ʱ�ȭ �۾��� ��ġ�մϴ�.
}


// ������ CLocatorApp ��ü�Դϴ�.

CLocatorApp theApp;

// CLocatorApp �ʱ�ȭ

BOOL CLocatorApp::InitInstance()
{
	char szName[ 128 ] = {0,};
	SYSTEMTIME st;
	// GetSystemTime(&st);
	GetLocalTime( &st );
	_snprintf( szName, 127, "./Log/LocatorLog_%u-%u-%u_%u-%u.txt",
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute );
	InitLog( MLOGSTYLE_DEBUGSTRING | MLOGSTYLE_FILE, szName );

	// Locator�� ȯ�� ���� ���������� �ε�.
	// �ݵ�� Locator�� �����Ǳ����� ����� ��.
	if( !GetLocatorConfig()->LoadConfig() )
	{
		AfxMessageBox( "Lcator config load fail" );
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
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CLocatorDoc),
		RUNTIME_CLASS(CMainFrame),       // �� SDI ������ â�Դϴ�.
		// RUNTIME_CLASS(CLocatorView));
		RUNTIME_CLASS(MLocatorView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);
	// ǥ�� �� ���, DDE, ���� ���⿡ ���� ������� ���� �м��մϴ�.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	// ����ٿ� ������ ����� ����ġ�մϴ�. ���� ���α׷��� /RegServer, /Register, /Unregserver �Ǵ� /Unregister�� ���۵� ��� FALSE�� ��ȯ�մϴ�.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// â �ϳ��� �ʱ�ȭ�Ǿ����Ƿ� �̸� ǥ���ϰ� ������Ʈ�մϴ�.
	m_pMainWnd->SetWindowPos( &CWnd::wndTop, 200, 200, 520, 250, SWP_SHOWWINDOW );
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// ���̻簡 ���� ��쿡�� DragAcceptFiles�� ȣ���մϴ�.
	// SDI ���� ���α׷������� ProcessShellCommand �Ŀ� �̷��� ȣ���� �߻��ؾ� �մϴ�.

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
void CLocatorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CLocatorApp �޽��� ó����


void CLocatorApp::HeartBeat()
{
	POSITION p = GetFirstDocTemplatePosition(); 
	CDocTemplate* pTemplate = GetNextDocTemplate(p); 
	p = pTemplate->GetFirstDocPosition(); 
	CLocatorDoc* pDoc = (CLocatorDoc*)pTemplate->GetNextDoc(p); 
	if(pDoc!=NULL) 
	{
		pDoc->Run();
		if( GetLocatorConfig()->GetMaxElapsedUpdateServerStatusTime() < (timeGetTime() - m_dwViewUpdatedTime) )
		{
			MLocatorView* pLocatorView = GetLocatorView();
			if( 0 != pLocatorView )
			{
				pLocatorView->GetListCtrl().DeleteAllItems();
				pLocatorView->UpdateView();

				m_dwViewUpdatedTime = timeGetTime();
			}
		}
	}
	Sleep( 1 );
}


int CLocatorApp::Run()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	ASSERT_VALID(this);
	_AFX_THREAD_STATE* pState = AfxGetThreadState();

	// acquire and dispatch messages until a WM_QUIT message is received.
	for (;;)
	{
		// phase1: check to see if we can do idle work
		if (::PeekMessage(&(pState->m_msgCur), NULL, NULL, NULL, PM_NOREMOVE))
		{
			if (!PumpMessage())
				return ExitInstance();
		}

		HeartBeat();

		/*
		if( m_bShutdown == false )
			HeartBeat();
			
		else
			break;
			*/
	}

	return 0;
}
