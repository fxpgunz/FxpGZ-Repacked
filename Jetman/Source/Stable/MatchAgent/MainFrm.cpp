// MainFrm.cpp : CMainFrame Ŭ������ ����
//

#include "stdafx.h"
#include "MatchAgent.h"

#include "MainFrm.h"
#include "MRegistry.h"
#include "MMatchAgent.h"

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

	ReadPosition();

	// �������¹� Ÿ�̸�
	this->SetTimer(TIMERID_REFRESH_STATUSBAR, 1000, NULL);

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
	CMDIFrameWnd::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
}

void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CMDIFrameWnd::OnActivate(nState, pWndOther, bMinimized);

	// TODO: Add your message handler code here
}

void CMainFrame::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	WritePosition();

	CMatchAgentApp* pApp = (CMatchAgentApp*)AfxGetApp();
	pApp->Shutdown();

	CMDIFrameWnd::OnClose();
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
	MMatchAgent* pServer = MMatchAgent::GetInstance();

	double dFPS = ((CMatchAgentApp*)AfxGetApp())->GetFPS();

	CString strStatus;
	strStatus.Format("C(%u),N(%u),S(%u) || LPS(%.1f)", 
		pServer->GetClientCount(), pServer->GetCommObjCount(), pServer->GetStageCount(), dFPS);

	int nIndex = m_wndStatusBar.CommandToIndex(ID_INDICATOR_SERVERSTATUS);
	m_wndStatusBar.SetPaneInfo(nIndex, ID_INDICATOR_SERVERSTATUS, SBPS_STRETCH, 200);
	m_wndStatusBar.SetPaneText(nIndex, strStatus);
}