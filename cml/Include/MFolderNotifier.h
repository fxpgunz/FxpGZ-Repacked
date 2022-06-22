//////////////////////////////////////////////////////
// MFolderNotifier
//		지정한 폴더안의 파일에 수정이 생기면 감지한다.
//		NT4.0 이상 사용
//		2007-04-19 RaonHaje 작업

#pragma once

#include <windows.h>
#include <string>
#include <queue>
using namespace std;


enum MFOLDERNOTIFY
{
	MFOLDERNOTIFY_FILECHANGED = 1,
	MFOLDERNOTIFY_END
};

class MFolderNotify
{
private:
	MFOLDERNOTIFY	m_nNotify;
	string			m_strFileName;

public:
	MFolderNotify(MFOLDERNOTIFY	nNotify, string strFileName)
	{
		m_nNotify = nNotify;
		m_strFileName = strFileName;
	}
	virtual ~MFolderNotify() {}
	
	MFOLDERNOTIFY GetNotify()	{ return m_nNotify; }
	const string& GetFileName()	{ return m_strFileName; }
};
typedef queue<MFolderNotify*> MFolderNotifyQueue;


class MFolderNotifierListener
{
private:
	CRITICAL_SECTION	m_cs;

	MFolderNotifyQueue	m_NotifyQueue;

public:
	MFolderNotifierListener()			{ InitializeCriticalSection(&m_cs); }
	virtual ~MFolderNotifierListener()	{ DeleteCriticalSection(&m_cs); }

	void Lock()							{ EnterCriticalSection(&m_cs); }
	void Unlock()						{ LeaveCriticalSection(&m_cs); }
	void AddEvent(MFOLDERNOTIFY nEventType, const string& strFileName);	// Must be LOCKED before call
	MFolderNotify* Fetch();	// Must be LOCKED before call
							// Delete MFolderNotify after Fetch

	// Call from NotifierThread 
	virtual bool OnEvent(int nEventType, const string& strFileName)=0;
};


class MFolderNotifier
{
	HANDLE	m_hThread;
	DWORD	m_dwThreadID;

	HANDLE	m_hFolder;
	string	m_strFolderName;

	MFolderNotifierListener*	m_pFolderNotifierListener;

private:
	const string& GetFolderName()	{ return m_strFolderName; }
	HANDLE GetFolderHandle()		{ return m_hFolder; }

private:
	static DWORD WINAPI NotifierThread(LPVOID pArg);

public:
	MFolderNotifier()
	{
		m_hThread = 0;
		m_dwThreadID = 0;
		m_hFolder = 0;
		m_pFolderNotifierListener = NULL;
	}
	virtual ~MFolderNotifier()	{}

	void SetListener(MFolderNotifierListener* pListener)	{ m_pFolderNotifierListener = pListener; }
	MFolderNotifierListener* GetListener() { return m_pFolderNotifierListener; }

	bool Create(const string& strFolderName);
	void Destroy();
};
