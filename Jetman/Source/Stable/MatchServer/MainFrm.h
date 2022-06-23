// MainFrm.h : CMainFrame Ŭ������ �������̽�
//
#include "ConsoleBar.h"


#pragma once

#include "TrayIcon.h"

#define WM_ICON_NOTIFY		(WM_USER+10)

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

protected:
	void ReadPosition();
	void WritePosition();
	void UpdateServerStatusBar();

// �۾�
public:

// ������
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// ����
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // ��Ʈ�� ������ ���Ե� ����Դϴ�.
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

public:
	CConsoleBar		m_wndConsoleBar;
	CTrayIcon		m_TrayIcon;

// �޽��� �� �Լ��� �����߽��ϴ�.
protected:
	LRESULT OnTrayNotification(WPARAM wParam, LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnClose();
	afx_msg void OnMenuTerminate();
	afx_msg void OnMenuShow();
	afx_msg void OnMenuHide();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};


