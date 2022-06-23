// MatchAgentDoc.h : CMatchAgentDoc Ŭ������ �������̽�
//
#pragma once


#include "MCommandManager.h"
//#include "MMonitor.h"
#include "MMaster.h"

class COutputView;
class CMatchServerDoc;
class MCommandProcessor;
class MBMatchAgent;
class CCommandLogView;

#define TIME_COLOR	RGB(128, 128, 128)


class CMatchAgentDoc : public CDocument
{
protected: // serialization������ ��������ϴ�.
	CMatchAgentDoc();
	DECLARE_DYNCREATE(CMatchAgentDoc)

protected:
	MBMatchAgent*	m_pMatchAgent;

protected:
	COutputView*		m_pMatchAgentView;
	CCommandLogView*	m_pCmdLogView;
	CFrameWnd*			m_pFrmWndCmdLogView;


// Ư��
public:

// �۾�
public:

// ������
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
	virtual ~CMatchAgentDoc();
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


