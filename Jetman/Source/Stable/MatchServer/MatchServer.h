// MatchServer.h : MatchServer ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // �� ��ȣ
#include "MZFileSystem.h"

// CMatchServerApp:
// �� Ŭ������ ������ ���ؼ��� MatchServer.cpp�� �����Ͻʽÿ�.
//

class CMatchServerApp : public CWinApp
{
private:
	bool				m_bShutdown;
	bool				m_bOutputLog;

	bool				m_bTodayRankingRequestDone;

public:
	MZFileSystem		m_ZFS;

	CMultiDocTemplate* m_pDocTemplateOutput;
	//CMultiDocTemplate* m_pDocTemplateMapView;
	CMultiDocTemplate* m_pDocTemplateCmdLogView;

	BOOL PreTranslateMessage(MSG* pMsg);
public:
	CMatchServerApp();
	virtual ~CMatchServerApp();
	
	bool CheckOutputLog()	{ return m_bOutputLog; }

	bool IsShutdown()	{ return m_bShutdown; }
	void Shutdown()		{ m_bShutdown = true; }
	void HeartBeat();

private:
	void RequestDBRankingList();

// ������
public:
	virtual BOOL InitInstance();

// ����
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
	virtual int Run();
	afx_msg void OnViewServerStatus();
	afx_msg void OnMessageExit();
	afx_msg void OnShowCmdLog();
	afx_msg void OnSetUseCountryFilter();
	afx_msg void OnSetAccetpInvalidIP();
	afx_msg void OnUpdateShowCmdLog(CCmdUI *pCmdUI);
	afx_msg void OnUseCountryFilter(CCmdUI *pCmdUI);
	afx_msg void OnAcceptInvalidIP(CCmdUI* pCmdUI );
	afx_msg void OnUpdateIPtoCountry();
	afx_msg void OnUpdateBlockCountryCode();
	afx_msg void OnUpdateCustomIP();
	afx_msg void OnEnableUpdateIPtoCountry( CCmdUI* pCmdUI );
	afx_msg void OnEnableUpdateBlockCountryCode( CCmdUI* pCmdUI );
	afx_msg void OnEnableUpdateCustomIP( CCmdUI* pCmdUI );
	afx_msg void OnToolTest();
};

extern CMatchServerApp theApp;
