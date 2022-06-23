// MatchServerDoc.h : CMatchServerDoc Ŭ������ �������̽�
//


#pragma once

#include "MMatchServer.h"
#include "MCommandManager.h"
#include "MMonitor.h" // �������� ����� ����ϰ� ���� �ʾƼ� cscommon/mmonitor �ּ�ó����.MMonitor������Ʈ�� �浹��.
#include "MMaster.h"

class COutputView;
class CMatchServerDoc;
class MCommandProcessor;
class MBMatchServer;
class CCommandLogView;
class MBMatchServer;


#define TIME_COLOR	RGB(128, 128, 128)


class MBMaster : public MMaster{
public:
	COutputView*	m_pView;
public:
	MBMaster(COutputView* pView=NULL);
	virtual void Log(const char* szLog);
};


class CMatchServerDoc : public CDocument
{
protected: // serialization������ ��������ϴ�.
	CMatchServerDoc();
	DECLARE_DYNCREATE(CMatchServerDoc)

// Attributes
public:
	MBMaster*		m_pMaster;
 	MBMatchServer*	m_pMatchServer;
	MMonitor*		m_pMonitor;

protected:
	COutputView*		m_pMasterView;
	COutputView*		m_pMatchServerView;
	CCommandLogView*	m_pCmdLogView;
	CFrameWnd*		m_pFrmWndMasterView;
	CFrameWnd*		m_pFrmWndCmdLogView;

public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

	COutputView* GetOutputView(void);

	void PostCommand(const char* szCommand);
	void Run(void);

protected:
	void ProcessLocalCommand(MCommand* pCommand);

// ����
public:
	virtual ~CMatchServerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// �޽��� �� �Լ��� �����߽��ϴ�.
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();
};


