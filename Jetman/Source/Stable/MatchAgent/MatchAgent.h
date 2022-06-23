// MatchAgent.h : MatchAgent ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error PCH���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����Ͻʽÿ�.
#endif

#include "resource.h"       // �� ��ȣ
#include "MZFileSystem.h"


class CSeException;


// CMatchAgentApp:
// �� Ŭ������ ������ ���ؼ��� MatchAgent.cpp�� �����Ͻʽÿ�.
//
class CMatchAgentApp : public CWinApp
{
private:
	bool				m_bShutdown;

public:
	double				m_dFPS;
	double				GetFPS()	{ return m_dFPS; }

public:
	MZFileSystem		m_ZFS;

public:
	CMultiDocTemplate* m_pDocTemplateOutput;
	CMultiDocTemplate* m_pDocTemplateCmdLogView;

public:
	CMatchAgentApp();
	void Shutdown() { m_bShutdown = true; }
	void HeartBeat();
	double UpdateFPS();

// ������
public:
	virtual BOOL InitInstance();

// ����
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
	virtual int Run();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnCmdDebugpeer();
};

extern CMatchAgentApp theApp;
