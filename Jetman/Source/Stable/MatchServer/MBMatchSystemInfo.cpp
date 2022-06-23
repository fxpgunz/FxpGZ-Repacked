/////////////////////////////////////////////////////////////////////
// MBMatchSystemInfo.cpp
/////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Psapi.h"
#include "MBMatchSystemInfo.h"

MBMatchSystemInfo::MBMatchSystemInfo()
{
	m_dwStartTick = GetTickCount();
}

MBMatchSystemInfo::~MBMatchSystemInfo()
{
}

/////////////////////////////////////////////////////////////////////
// GetProcessRunningTime => ���μ��� �����ð��� ���� ������ ���´�.
/////////////////////////////////////////////////////////////////////
void MBMatchSystemInfo::GetProcessRunningTime(SYSTEMTIME* stRunningTime)
{
#ifdef _MONITORING
	memset(stRunningTime, 0, sizeof(SYSTEMTIME));
	// ��� �ð�(���� �ð��� ������ - ���� �ð��� ������)
	DWORD dwNowTick = GetTickCount();
	DWORD dwRunningTime = dwNowTick - m_dwStartTick;

	DWORD dwTotalSecond = dwRunningTime / 1000;
	DWORD dwDay = dwTotalSecond / 86400;
	DWORD dwRemanderSecond = dwTotalSecond % 86400;
	stRunningTime->wDay = (WORD)dwDay;
	DWORD dwHour = dwRemanderSecond / 3600;
	dwRemanderSecond = dwRemanderSecond % 3600;
	stRunningTime->wHour = (WORD)dwHour;
	DWORD dwMinute = dwRemanderSecond / 60;
	stRunningTime->wMinute = (WORD)dwMinute;
	DWORD dwSecond = dwRemanderSecond % 60;
	stRunningTime->wSecond = (WORD)dwSecond;
#endif
}

/////////////////////////////////////////////////////////////////////
// GetMemoryInfo => �޸𸮿� ���� ������ ���´�. (���� MB)
// dwTotalMemMB : �ý��� ��ü �޸� �뷮
// dwAvailMemMB : ������� �޸� �뷮
// dwVirtualMemMB : ����޸� ��ü �뷮
/////////////////////////////////////////////////////////////////////
void MBMatchSystemInfo::GetMemoryInfo(DWORD* dwTotalMemMB, DWORD* dwAvailMemMB, DWORD* dwVirtualMemMB)
{
#ifdef _MONITORING
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);

	DWORDLONG lMemTotalMB = (statex.ullTotalPhys / (1024 * 1024));
	*dwTotalMemMB = (DWORD)lMemTotalMB;

	DWORDLONG lAvailMemMB = (statex.ullAvailPhys / (1024 * 1024));
	*dwAvailMemMB = (DWORD)lAvailMemMB;

	DWORDLONG lVirtualMemMB = (statex.ullTotalVirtual / (1024 * 1024));
	*dwVirtualMemMB = (DWORD)lVirtualMemMB;
#endif
}

/////////////////////////////////////////////////////////////////////
// GetDiskInfo => ����̺꿡 ���� ������ ���´�. (���� MB)
// szDriveName : ����̺� ��
// dwTotalDriveMB : �ش� ����̺� ��ü �뷮
// dwFreeDriveMB : �ش� ����̺� ���� �뷮(��밡��)
// return value : �����ϸ� true ����, false => szDriveName�� ��ũ ����̺���� �ƴ� ���
/////////////////////////////////////////////////////////////////////
bool MBMatchSystemInfo::GetDiskInfo(char szDriveName, DWORD* dwTotalDriveMB, DWORD* dwFreeDriveMB)
{
#ifdef _MONITORING
	char strDirveName[MAX_PATH] = {0};
	sprintf(strDirveName, "%c:", szDriveName);
	unsigned int uDrvType = GetDriveType(strDirveName);
	if (uDrvType != DRIVE_FIXED)						// ��ũ ����̹��� ��츸
	{
		return false;
	}

	typedef BOOL (WINAPI *P_GDFSE)(LPCTSTR, PULARGE_INTEGER, PULARGE_INTEGER, PULARGE_INTEGER);
    P_GDFSE pGetDiskFreeSpaceEx = (P_GDFSE)GetProcAddress(GetModuleHandle("kernel32.dll"), "GetDiskFreeSpaceExA");

	DWORD dwVolumeSerialNumber = 0;
	DWORD dwMaxNameLength = 0;
	DWORD dwFileSystemFlags = 0;
	char szFileSysName[MAX_PATH] = {0};
	char szLabel[MAX_PATH] = {0};

	// GetVolumeInformation�� �ش� ��ũ�� �Ϲ������� ������ ���� �Լ��̴�.
	GetVolumeInformation(strDirveName, szLabel, 256, &dwVolumeSerialNumber, &dwMaxNameLength, &dwFileSystemFlags, szFileSysName, 256);

	BOOL fResult;
	unsigned __int64 i64FreeBytesToCaller, i64TotalBytes, i64FreeBytes;
	DWORD dwSectPerClust, dwBytesPerSect, dwFreeClusters, dwTotalClusters;
	DWORD dwTotalMBytes, dwFreeMBytes;

	// ���� 95�̻��� ���� �ý��� �ϰ��
	if (pGetDiskFreeSpaceEx)
	{
		// Win98 or NT �� �� �̻��̸� True ���� ��ȯ�Ѵ�.
		fResult = pGetDiskFreeSpaceEx(strDirveName, (PULARGE_INTEGER)&i64FreeBytesToCaller, (PULARGE_INTEGER)&i64TotalBytes, (PULARGE_INTEGER)&i64FreeBytes);
		if (fResult)
		{
			dwTotalMBytes = (DWORD)(i64TotalBytes / (1024 * 1024));
			dwFreeMBytes = (DWORD)(i64FreeBytes / (1024 * 1024));
		}
	}
	// ���� 95������ �ý��� �ϰ��
	else
	{
		fResult = GetDiskFreeSpace(strDirveName, &dwSectPerClust, &dwBytesPerSect, &dwFreeClusters, &dwTotalClusters);
		if (fResult)
		{
			i64TotalBytes = (__int64)dwTotalClusters * dwSectPerClust *	dwBytesPerSect;
			i64FreeBytes = (__int64)dwFreeClusters * dwSectPerClust * dwBytesPerSect;
			dwTotalMBytes = (DWORD)(i64TotalBytes / (1024 * 1024));
			dwFreeMBytes = (DWORD)(i64FreeBytes / (1024 * 1024));
		}
	}

	*dwTotalDriveMB = dwTotalMBytes;
	*dwFreeDriveMB = dwFreeMBytes;
#endif
	return true;
}

/////////////////////////////////////////////////////////////////////
// GetProcessName => �ڱ��ڽ� �������ϸ� ���� ������ ���´�.
// szProcessName : �������ϸ�
/////////////////////////////////////////////////////////////////////
void MBMatchSystemInfo::GetExeFileName(char* szExeFileName)
{
	char szName[MAX_PATH] = {0};
	DWORD cb;
	BOOL bResult;
	HMODULE hModule;
	HANDLE hProcess = GetCurrentProcess();
	if (hProcess)
	{
		bResult = EnumProcessModules(hProcess, &hModule, sizeof(hModule), &cb);
		if (bResult != 0)
		{
			GetModuleBaseName(hProcess, hModule, szName, sizeof(szName));
		}
	}
	memcpy(szExeFileName, szName, MAX_PATH);
}
