// Locator.h : Locator ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error PCH���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����Ͻʽÿ�.
#endif

#include "resource.h"       // �� ��ȣ


// CLocatorApp:
// �� Ŭ������ ������ ���ؼ��� Locator.cpp�� �����Ͻʽÿ�.
//

class CLocatorApp : public CWinApp
{
public:
	CLocatorApp();

private :
	void HeartBeat();

private :
	DWORD m_dwViewUpdatedTime;

// ������
public:
	virtual BOOL InitInstance();

// ����
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual int Run();
};

extern CLocatorApp theApp;
