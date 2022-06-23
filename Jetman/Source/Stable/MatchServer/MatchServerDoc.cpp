// MatchServerDoc.cpp : CMatchServerDoc Ŭ������ ����
//

#include "stdafx.h"
#include "MatchServer.h"

#include "MatchServerDoc.h"
#include "OutputView.h"
#include "MainFrm.h"
#include "MObject.h"
#include "MBMatchServer.h"
#include "CommandLogView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


MBMaster::MBMaster(COutputView* pView)
{
	m_pView = pView;
}

void MBMaster::Log(const char* szLog)
{
	if(m_pView==NULL) return;

	CTime theTime = CTime::GetCurrentTime();
	CString szTime = theTime.Format( "[%c] " );
	m_pView->AddString(szTime, TIME_COLOR, false);

	m_pView->AddString(szLog, RGB(0,0,0));
}


// CMatchServerDoc
IMPLEMENT_DYNCREATE(CMatchServerDoc, CDocument)

BEGIN_MESSAGE_MAP(CMatchServerDoc, CDocument)
END_MESSAGE_MAP()


// CMatchServerDoc ����/�Ҹ�
CMatchServerDoc::CMatchServerDoc()
{
	m_pMaster = new MBMaster();
	//MTempSetMaster(m_pMaster);
	

	m_pMatchServer = NULL;

	m_pMasterView = NULL;
	m_pMatchServerView = NULL;
}

CMatchServerDoc::~CMatchServerDoc()
{
//	MTempSetMaster(NULL);
//	m_pMatchServer->DisconnectToMaster();

	if( m_pMatchServer ){ delete m_pMatchServer; m_pMatchServer = NULL;}
	delete m_pMaster;
}

void ReadServerPorts(int* pMasterPort, int* pMatchPort)
{
	FILE* pF = fopen("port.txt", "r");
	if (pF == NULL) return;
	fscanf(pF, "%d %d", pMasterPort, pMatchPort);
	fclose(pF);
}

BOOL CMatchServerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	CMatchServerApp* pApp = (CMatchServerApp*)AfxGetApp();
	CMainFrame* pMainFrm = (CMainFrame*)pApp->GetMainWnd();

	// Zone-Server View
	m_pMatchServerView = GetOutputView();
	m_pMatchServerView->AddString("Match-Server Log");
	m_pMatchServerView->GetParentFrame()->GetSystemMenu(FALSE)->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND|MF_GRAYED);

	// Master-Server View
	CFrameWnd* pFrmWndOutput = pApp->m_pDocTemplateOutput->CreateNewFrame(this, NULL);
	ASSERT(pFrmWndOutput!=NULL);
	pApp->m_pDocTemplateOutput->InitialUpdateFrame(pFrmWndOutput, this);
	pFrmWndOutput->GetSystemMenu(FALSE)->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND|MF_GRAYED);
	m_pMasterView = (COutputView*)pFrmWndOutput->GetActiveView();
	m_pMasterView->AddString("Master-Controller Log");
	m_pFrmWndMasterView = pFrmWndOutput;
	((CMDIFrameWnd*)AfxGetMainWnd())->MDIActivate(pFrmWndOutput);

	// Command Log View �߰�
	CFrameWnd* pFrmWndCmdLogView = pApp->m_pDocTemplateCmdLogView->CreateNewFrame(this, NULL);
	ASSERT(pFrmWndCmdLogView!=NULL);
	pApp->m_pDocTemplateCmdLogView->InitialUpdateFrame(pFrmWndCmdLogView, this);
	m_pCmdLogView = (CCommandLogView*)pFrmWndCmdLogView->GetActiveView();
	m_pCmdLogView->GetParentFrame()->GetSystemMenu(FALSE)->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND|MF_GRAYED);
	m_pFrmWndCmdLogView = pFrmWndCmdLogView;

	// Set Output View
	m_pMaster->m_pView = m_pMasterView;

	int nMasterPort = 5000;
	int nMatchPort = 6000;
	ReadServerPorts(&nMasterPort, &nMatchPort);

	// Communicator Initialize
	m_pMaster->Create(nMasterPort);


	m_pMatchServer = new MBMatchServer;
	m_pMatchServer->m_pCmdLogView = m_pCmdLogView;
	m_pMatchServer->m_pView = m_pMatchServerView;

	if (!m_pMatchServer->Create(nMatchPort))
	{
		AfxMessageBox("MatchServer Failed to initialize.", MB_OK);
		AfxAbort();
		return FALSE;
	}

//	MCommObject* pCommObjMaster = new MCommObject(m_pMatchServer);
//	pCommObjMaster->SetUID( MUID(0,1) );
//	pCommObjMaster->SetAddress("127.0.0.1", nMasterPort);
//	pMaster->pDirectConnection = m_pMaster;
//	m_pMatchServer->ConnectToMaster(pCommObjMaster);

	return TRUE;
}

BOOL CMatchServerDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	// TODO:  Add your specialized creation code here
	return TRUE;
}

void CMatchServerDoc::OnCloseDocument()
{
	if( m_pMatchServer ){ delete m_pMatchServer; m_pMatchServer = NULL;}

	//	m_pFrmWndMasterView->DestroyWindow();
	m_pFrmWndCmdLogView->DestroyWindow();
	CDocument::OnCloseDocument();
}

COutputView* CMatchServerDoc::GetOutputView(void)
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

void CMatchServerDoc::PostCommand(const char* szCommand)
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

void CMatchServerDoc::Run(void)
{
//	if(m_pMaster!=NULL) m_pMaster->Run();
	if(m_pMatchServer!=NULL) m_pMatchServer->Run();
}


// CMatchServerDoc serialization

void CMatchServerDoc::Serialize(CArchive& ar)
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


// CMatchServerDoc ����

#ifdef _DEBUG
void CMatchServerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMatchServerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


