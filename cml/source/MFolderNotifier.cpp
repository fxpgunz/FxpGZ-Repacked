#define _WIN32_WINNT 0x400
#include "MFolderNotifier.h"
#define MFOLDERNOTIFY_MAXQUEUE	1000


// Must be LOCKED before call
void MFolderNotifierListener::AddEvent(MFOLDERNOTIFY nEventType, const string& strFileName)
{
	if (m_NotifyQueue.size() > MFOLDERNOTIFY_MAXQUEUE)
	{
		MFolderNotify* pDiscard = m_NotifyQueue.front();
		m_NotifyQueue.pop();
		delete pDiscard;
	}

	// FileChanged Notify는 두번 연달아들어오므로 중복제거
	bool bSkipNotify = false;
	if (m_NotifyQueue.empty() == false)
	{
		MFolderNotify* pRepeat = m_NotifyQueue.back();
		if (pRepeat)
		{
			if ( (pRepeat->GetNotify() == nEventType) &&
				(pRepeat->GetFileName().compare(strFileName) == 0) )
			{
				bSkipNotify = true;
			}
		}
	}

	if (bSkipNotify == false)
	{
		MFolderNotify* pNotify = new MFolderNotify(nEventType, strFileName);
		m_NotifyQueue.push(pNotify);
	}
}

// Must be LOCKED before call
// Delete MFolderNotify after Fetch
MFolderNotify* MFolderNotifierListener::Fetch()
{
	if (m_NotifyQueue.empty())
		return NULL;

	MFolderNotify* pNotify = m_NotifyQueue.front();
	m_NotifyQueue.pop();
	return pNotify;
}

bool MFolderNotifier::Create(const string& strFolderName)
{
	// Initialize FolderNotify
	HANDLE hFolder = CreateFile( strFolderName.c_str(),
							FILE_LIST_DIRECTORY,				// access (read/write) mode
							FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,	// share mode
							NULL,								// security descriptor
							OPEN_EXISTING,						// how to create
							FILE_FLAG_BACKUP_SEMANTICS,			// file attributes
							NULL								// file with attributes to copy
							);
	if (hFolder == INVALID_HANDLE_VALUE)
	{
		DWORD dwError = GetLastError();
		return false;
	} 
	else 
	{
		m_strFolderName = strFolderName;
		m_hFolder = hFolder;
	}

	// Create ReadDirectoryChanges Thread
	m_hThread = CreateThread( 
							NULL,			// default security attributes 
							0,				// use default stack size  
							NotifierThread,	// thread function 
							this,			// argument to thread function 
							0,				// use default creation flags 
							&m_dwThreadID);	// returns the thread identifier 
	
	if (m_hThread == NULL)
	{
		CloseHandle(m_hFolder);
		m_hFolder = NULL;
		return false;
	}

	return true;
}

void MFolderNotifier::Destroy()
{
	if (m_hThread)
	{
		TerminateThread(m_hThread, 0);
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}	

	if (m_hFolder)
	{
		CloseHandle(m_hFolder);
		m_hFolder = NULL;
	}
}

DWORD WINAPI MFolderNotifier::NotifierThread(LPVOID pArg)
{
	MFolderNotifier* pReader = static_cast<MFolderNotifier*>(pArg);
	HANDLE hFolder = pReader->GetFolderHandle();

	// Blocked Loop for Notify read
	while(true)
	{
		BYTE buffer[1024];
		ZeroMemory(buffer,1024);
		DWORD BytesReturned;
		BOOL bResult = ReadDirectoryChangesW(
							hFolder,						// handle to directory
							&buffer,						// read results buffer
							sizeof(buffer),					// length of buffer
							TRUE,							// monitoring option
							//FILE_NOTIFY_CHANGE_SECURITY|
							//FILE_NOTIFY_CHANGE_CREATION|
							//FILE_NOTIFY_CHANGE_LAST_ACCESS|
							//FILE_NOTIFY_CHANGE_SIZE|
							//FILE_NOTIFY_CHANGE_ATTRIBUTES|
							//FILE_NOTIFY_CHANGE_DIR_NAME|
							//FILE_NOTIFY_CHANGE_FILE_NAME|
							FILE_NOTIFY_CHANGE_LAST_WRITE,	// filter conditions
							&BytesReturned,					// bytes returned
							NULL,							// overlapped buffer
							NULL							// completion routine
							);
		if (bResult == FALSE)
			break;

		int nOffset = 0;
		while(true)
		{
			FILE_NOTIFY_INFORMATION* pInfo = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(buffer+nOffset);

			// Log
			char szFileName[_MAX_PATH]="";
			ZeroMemory(szFileName, _MAX_PATH);
			size_t nSize = wcstombs(NULL, pInfo->FileName, _MAX_PATH);
			size_t nSizeN = wcstombs(szFileName, pInfo->FileName, _MAX_PATH);

//			char szBuf[256];
//			wsprintf(szBuf, "FileNotify> (%d) %s \n", pInfo->Action, szFileName);
//			OutputDebugString(szBuf);

			MFolderNotifierListener* pListener = pReader->GetListener();
			if (pListener)
			{
				switch(pInfo->Action)
				{
				case FILE_ACTION_ADDED:
					break;
				case FILE_ACTION_REMOVED:
					break;
				case FILE_ACTION_MODIFIED:
					pListener->Lock();
					pListener->AddEvent(MFOLDERNOTIFY_FILECHANGED, szFileName);
					pListener->Unlock();
					pListener->OnEvent(MFOLDERNOTIFY_FILECHANGED, szFileName);
					break;
				case FILE_ACTION_RENAMED_OLD_NAME:
					break;
				case FILE_ACTION_RENAMED_NEW_NAME:
					break;
				};
			}

			if (pInfo->NextEntryOffset == 0)
				break;

			// Increase offset
			nOffset += pInfo->NextEntryOffset;
		}
	}

	return 0;
}
