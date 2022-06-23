// MainFrm.cpp : CMainFrame Ŭ������ ����
//

#include "stdafx.h"
#include "MatchServer.h"

#include "MainFrm.h"
#include "MRegistry.h"
#include "mainfrm.h"
#include "resource.h"

#include "MMatchServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame
IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ACTIVATE()
	ON_WM_CLOSE()
	ON_MESSAGE(WM_ICON_NOTIFY, OnTrayNotification)
	ON_COMMAND(ID_MENU_TERMINATE, OnMenuTerminate)
	ON_COMMAND(ID_MENU_SHOW, OnMenuShow)
	ON_COMMAND(ID_MENU_HIDE, OnMenuHide)
	ON_WM_DESTROY()
	ON_WM_TIMER()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // ���� �� ǥ�ñ�
	ID_INDICATOR_SERVERSTATUS,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

#define TIMERID_REFRESH_STATUSBAR	101


// CMainFrame ����/�Ҹ�

CMainFrame::CMainFrame()
{
	// TODO: ���⿡ ��� �ʱ�ȭ �ڵ带 �߰��մϴ�.
}

CMainFrame::~CMainFrame()
{
	m_TrayIcon.RemoveIcon();
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("���� ������ ������ ���߽��ϴ�.\n");
		return -1;      // ������ ���߽��ϴ�.
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("���� ǥ������ ������ ���߽��ϴ�.\n");
		return -1;      // ������ ���߽��ϴ�.
	}

	// TODO: ���� ������ ��ŷ�� �� ���� �Ϸ��� �� �� ���� �����Ͻʽÿ�.
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	if(!m_wndConsoleBar.Create(this, IDD_DIALOG_CONSOLE, CBRS_BOTTOM|CBRS_TOOLTIPS|CBRS_FLYBY, IDD_DIALOG_CONSOLE)){
		TRACE0("Failed to create ConsoleBar\n");
		return -1;
	}
	//m_wndConsoleBar.ShowWindow(SW_HIDE);

	ReadPosition();

	// Ʈ���� ������ ���
	if (!m_TrayIcon.Create(this, WM_ICON_NOTIFY, _T("MatchServer"), NULL, IDR_MAINFRAME))
	{
		TRACE0("Failed to create tray icon.\n");
		return -1;
	}
	// ������ ����
	HICON icon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_TrayIcon.SetIcon(icon);

	// �������¹� Ÿ�̸�
	this->SetTimer(TIMERID_REFRESH_STATUSBAR, 5000, NULL);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	// Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return TRUE;
}


// CMainFrame ����

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame �޽��� ó����


void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	// Console Resize
	RECT cr;
	GetClientRect(&cr);
	m_wndConsoleBar.m_sizeDefault.cx = cr.right-cr.left;
	RecalcLayout();

	CMDIFrameWnd::OnSize(nType, cx, cy);
	// TODO: Add your message handler code here
}

void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CMDIFrameWnd::OnActivate(nState, pWndOther, bMinimized);

	CMatchServerApp* pApp = (CMatchServerApp*)AfxGetApp();
	if (pApp->IsShutdown() == true) return;

	if(nState==WA_ACTIVE) m_wndConsoleBar.GetDlgItem(IDC_COMBO_COMMAND)->SetFocus();
}

void CMainFrame::OnClose()
{
	if (AfxMessageBox("Exit?", MB_YESNO | MB_ICONQUESTION) != IDYES)
	{
		return;
	}

	// TODO: Add your message handler code here and/or call default
   
	CMDIFrameWnd::OnClose();
}

void CMainFrame::OnDestroy()
{
	CMDIFrameWnd::OnDestroy();

	// TODO: Add your message handler code here
	TRACE("CMainFrame::OnDestroy() \n");

	WritePosition();
	CMatchServerApp* pApp = (CMatchServerApp*)AfxGetApp();
	pApp->Shutdown();
}

void CMainFrame::ReadPosition()
{
	RECT rt;
	DWORD dwSize = sizeof(rt);
	if (MRegistry::ReadBinary(HKEY_CURRENT_USER, "MainPosition", (char*)&rt, &dwSize) == true)
		MoveWindow(&rt);
		
}

void CMainFrame::WritePosition()
{
	RECT rt;
	GetWindowRect(&rt);
	if (rt.left < 0 || rt.top < 0 || rt.right - rt.left < 0 || rt.bottom - rt.top < 0)
		return;

	MRegistry::WriteBinary(HKEY_CURRENT_USER, "MainPosition", (char*)&rt, sizeof(rt));
}


LRESULT CMainFrame::OnTrayNotification(WPARAM wParam, LPARAM lParam)
{
	if (LOWORD(lParam) == WM_RBUTTONUP)
	{
		CMenu menu, *pSubMenu;

		if (!menu.LoadMenu(IDR_POPUPMENU)) return 0;
		if (!(pSubMenu = menu.GetSubMenu(0))) return 0;

		CPoint pos;
		GetCursorPos(&pos);
		SetForegroundWindow();

		pSubMenu->TrackPopupMenu(TPM_RIGHTALIGN, pos.x, pos.y, this);
		menu.DestroyMenu();
	}
	else if (LOWORD(lParam) == WM_LBUTTONDBLCLK)
	{
		if (IsWindowVisible()) ShowWindow(SW_HIDE);
		else ShowWindow(SW_SHOW);
	}
	return 1;
}
void CMainFrame::OnMenuTerminate()
{
	SendMessage(WM_CLOSE, 0, 0);
}

void CMainFrame::OnMenuShow()
{
	ShowWindow(SW_SHOW);
}

void CMainFrame::OnMenuHide()
{
	ShowWindow(SW_HIDE);
}


void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if (nIDEvent == TIMERID_REFRESH_STATUSBAR) {
		UpdateServerStatusBar();
	}

	CMDIFrameWnd::OnTimer(nIDEvent);
}

void CMainFrame::UpdateServerStatusBar()
{
	MMatchServer* pServer = MMatchServer::GetInstance();
	if (!pServer->IsCreated()) return;

	CString strStatus;
	strStatus.Format("C(%d),N(%d),A(%d)", 
		pServer->GetClientCount(), pServer->GetCommObjCount(), pServer->GetAgentCount());

	int nIndex = m_wndStatusBar.CommandToIndex(ID_INDICATOR_SERVERSTATUS);
	m_wndStatusBar.SetPaneInfo(nIndex, ID_INDICATOR_SERVERSTATUS, SBPS_STRETCH, 200);
	m_wndStatusBar.SetPaneText(nIndex, strStatus);
}