// MatchAgentDoc.cpp : CMatchAgentDoc Ŭ������ ����
//

#include "stdafx.h"
#include "MatchAgent.h"
#include "MBMatchAgent.h"
#include "MatchAgentDoc.h"
#include "OutputView.h"
#include "CommandLogView.h"
#include "MainFrm.h"
#include "MAgentConfig.h"
#include "MCrashDump.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMatchAgentDoc

IMPLEMENT_DYNCREATE(CMatchAgentDoc, CDocument)

BEGIN_MESSAGE_MAP(CMatchAgentDoc, CDocument)
END_MESSAGE_MAP()


// CMatchAgentDoc ����/�Ҹ�

CMatchAgentDoc::CMatchAgentDoc()
{
	m_pMatchAgent = new MBMatchAgent();
	m_pMatchAgentView = NULL;
}

CMatchAgentDoc::~CMatchAgentDoc()
{
	delete m_pMatchAgent; m_pMatchAgent=NULL;
}

BOOL CMatchAgentDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	CMatchAgentApp* pApp = (CMatchAgentApp*)AfxGetApp();
	CMainFrame* pMainFrm = (CMainFrame*)pApp->GetMainWnd();

	// Zone-Server View
	m_pMatchAgentView = GetOutputView();
	m_pMatchAgentView->AddString("Agent-Server Log");
	m_pMatchAgentView->GetParentFrame()->GetSystemMenu(FALSE)->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND|MF_GRAYED);

	// Command Log View �߰�
	CFrameWnd* pFrmWndCmdLogView = pApp->m_pDocTemplateCmdLogView->CreateNewFrame(this, NULL);
	ASSERT(pFrmWndCmdLogView!=NULL);
	pApp->m_pDocTemplateCmdLogView->InitialUpdateFrame(pFrmWndCmdLogView, this);
	m_pCmdLogView = (CCommandLogView*)pFrmWndCmdLogView->GetActiveView();
	m_pMatchAgent->m_pCmdLogView = m_pCmdLogView;
	m_pCmdLogView->GetParentFrame()->GetSystemMenu(FALSE)->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND|MF_GRAYED);
	m_pFrmWndCmdLogView = pFrmWndCmdLogView;

	// Set Output View
	m_pMatchAgent->m_pView = m_pMatchAgentView;

	MAgentConfig* pAgentConfig = MAgentConfig::GetInstance();
	m_pMatchAgent->SetIP(pAgentConfig->GetIP());
	m_pMatchAgent->SetTCPPort(pAgentConfig->GetTCPPort());
	m_pMatchAgent->SetUDPPort(pAgentConfig->GetUDPPort());
	m_pMatchAgent->SetMatchServerIP(pAgentConfig->GetMatchServerIP());
	m_pMatchAgent->SetMatchServerTCPPort(pAgentConfig->GetMatchServerTCPPort());
	pAgentConfig->ReleaseInstance();

	// Communicator Initialize
	m_pMatchAgent->Create(0);	// Config Port
	m_pMatchAgent->ConnectToMatchServer(NULL, 0);	// Config Address
	return TRUE;
}

COutputView* CMatchAgentDoc::GetOutputView(void)
{
	POSITION p = GetFirstViewPosition();
	while(1){
		CView* pView = GetNextView(p);
		if(pView==NULL) break;
		if(pView->IsKindOf(RUNTIME_CLASS(COutputView))==TRUE){
			return (COutputView*)pView;
		}
	}
	return NULL;
}

#define COLOR_MESSAGE		RGB(0, 128, 0)
#define COLOR_USERCOMMAND	RGB(0, 0, 0)
#define COLOR_ERRORCOMMAND	RGB(128, 0, 0)

void CMatchAgentDoc::PostCommand(const char* szCommand)
{
	if(szCommand[0]==NULL) return;

	//static char szTemp[512];
	//sprintf(szTemp, "> %s", szCommand);
	//m_pMonitor->OutputMessage(szTemp, MZMOM_USERCOMMAND);

	/*
	char szErrMsg[256];
	if(m_pMonitor->Post(szErrMsg, 256, szCommand)==false){
		m_pMonitor->OutputMessage(szErrMsg, MZMOM_ERROR);
	}
	*/
}

void CMatchAgentDoc::Run(void)
{
	__try
	{
		if (m_pMatchAgent != NULL) m_pMatchAgent->Run();
	}
	__except (CrashExceptionDump(GetExceptionInformation(), "matchagent.dmp"))
	{

	}
}





// CMatchAgentDoc serialization

void CMatchAgentDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	}
	else
	{
		// TODO: ���⿡ �ε� �ڵ带 �߰��մϴ�.
	}
}


// CMatchAgentDoc ����

#ifdef _DEBUG
void CMatchAgentDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMatchAgentDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CMatchAgentDoc ���

BOOL CMatchAgentDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	// TODO:  Add your specialized creation code here

	return TRUE;
}

void CMatchAgentDoc::OnCloseDocument()
{
	// TODO: Add your specialized code here and/or call the base class
	m_pFrmWndCmdLogView->DestroyWindow();

	CDocument::OnCloseDocument();
}
