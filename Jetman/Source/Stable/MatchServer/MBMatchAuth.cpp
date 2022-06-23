#include "stdafx.h"
#include "MBMatchAuth.h"
//#include "Netmarble/CPSSOLib.h"
#include <stdlib.h>
#include "MDebug.h"
#include "MMatchStatus.h"
#include "CommonLog.h"

#ifndef NEW_AUTH_MODULE

bool MBMatchAuthBuilder::ParseAuthInfo(const char* pszData, MMatchAuthInfo** ppoutAutoInfo)
{
	return true;
}

#else

#include "MBMatchAsyncDBJob_NetmarbleLogin.h"

// �� �ݸ��� ���� ���� ���̺귯�� �Ŵ��� Gunz\MatchServer\Netmarble\NMAuthLib.chm ����
MBMatchNetmarbleModule::MBMatchNetmarbleModule()
{
	InitializeCriticalSection(&m_cs);

	DWORD dwThreadId=0;
	m_hThread = CreateThread(NULL, 0, WorkerThread, this, 0, &dwThreadId);
	if (m_hThread == NULL)
	{
		Log("## FAILED TO CREATE NETMARBLE AUTH THREAD ##");
		_ASSERT(0);
	}
}

MBMatchNetmarbleModule::~MBMatchNetmarbleModule()
{
	WaitForSingleObject(m_hThread, INFINITE);

	TerminateThread(m_hThread, 0);
	CloseHandle(m_hThread);

	DestroyModule();
	DeleteCriticalSection(&m_cs);
}

MBMatchNetmarbleModule& MGetNetmarbleModule() 
{
	return MBMatchNetmarbleModule::GetInstance();
}

bool MBMatchNetmarbleModule::InitModule()
{
#ifndef _DEBUG
	ERROR_NMAUTH error;
	error = NMAuthLib::Initialize("gunz");	//�ݸ����� �߱��� ������ gamecode ���ڿ�

	// ���� ���н�..
	if ( error != ERROR_NMAUTH_SUCCESS )
	{
		this->ErrorLog(error, "init failed");
		return false;
	}
	MLog("Netmarble auth server connected.\n");
#endif
	return true;
}

void MBMatchNetmarbleModule::DestroyModule()
{
	NMAuthLib::Destroy();
}


void MBMatchNetmarbleModule::ErrorLog()
{
	// �ݸ��� ���� ��⿡�� �� �����ڵ带 �״�� �α�
	ErrorLog( m_NMAuth.GetLastError(), "");
}

void MBMatchNetmarbleModule::ErrorLog(ERROR_NMAUTH error, const char* szMsg)
{
	// �ݸ��� ���� ����� �� �����ڵ�� �߰� �޽����� �α�
	LPCTSTR szErrorCode;
	szErrorCode = NMAuthLib::ErrorCode2String(error);

	if (szErrorCode)
		mlog("Netmarble Auth Lib Error : %s (%s)\n", szMsg, szErrorCode);
	else
		mlog("Netmarble Auth Lib Error : %s (%d)\n", szMsg, error);
}

void MBMatchNetmarbleModule::RequestAuth(const MUID& commUid, const char* szAuth, const char* szData, const char* szCp,
										 bool bFreeLoginIP,
										 unsigned long nChecksumPack,
										 bool bCheckPremiumIP,
										 const char* szIP,
										 DWORD dwIP,
										 const char* szCountryCode3)
{
	MBMatchNetmarbleAuthQuery* pQuery = new MBMatchNetmarbleAuthQuery(
		commUid, szAuth, szData, szCp,
		bFreeLoginIP,
		nChecksumPack,
		bCheckPremiumIP,
		szIP,
		dwIP,
		szCountryCode3);

	EnterCriticalSection(&m_cs);
		m_queries.push_back(pQuery);
	LeaveCriticalSection(&m_cs);
}

MBMatchNetmarbleAuthQuery* MBMatchNetmarbleModule::GetNextQuery()
{
	MBMatchNetmarbleAuthQuery* pQuery = NULL;

	EnterCriticalSection(&m_cs);
		if (!m_queries.empty()) {
			pQuery = *m_queries.begin();
			m_queries.pop_front();
		}
	LeaveCriticalSection(&m_cs);

	return pQuery;
}

DWORD WINAPI MBMatchNetmarbleModule::WorkerThread(LPVOID pJobContext)
{
	MBMatchNetmarbleModule* pAuthModule = (MBMatchNetmarbleModule*)pJobContext;
	_ASSERT(pAuthModule);

	ERROR_NMAUTH error;
	MBMatchNetmarbleAuthQuery* pQuery = NULL;

	while(1)
	{
		Sleep(1);

		pQuery = pAuthModule->GetNextQuery();
		if (pQuery == NULL)
			continue;

		// ���� ������ �ı�.
		pAuthModule->m_NMAuth.Destroy();

		//�ݸ��� ������⿡ ���������� ��û
		error = pAuthModule->m_NMAuth.Init(pQuery->m_strAuth.c_str(), pQuery->m_strData.c_str(), pQuery->m_strCp.c_str());

		if ( error == ERROR_NMAUTH_SUCCESS )
		{
			// Ÿ�Ӿƿ��� �Է����� ������ �⺻������ ó��.
			error = pAuthModule->m_NMAuth.LoadDataFromXML();

			// �����ϸ� 
			if ( error == ERROR_NMAUTH_SUCCESS )
			{
				//todok del
				//MLog("Netmarble login Auth SUCCEED by XML\n");

				// DB�� ���� ������ ����
				MBMatchAsyncDBJob_NetmarbleLogin* pNewJob = new MBMatchAsyncDBJob_NetmarbleLogin(pQuery->m_commUid);
				pNewJob->Input(new MMatchAccountInfo(), 
					new MMatchAccountPenaltyInfo,
					pAuthModule->m_NMAuth.GetData("UserID"), 
					pAuthModule->m_NMAuth.GetData("UniID"), 
					pAuthModule->m_NMAuth.GetData("Certificate"), 
					pAuthModule->m_NMAuth.GetData("Name"), 
					atoi(pAuthModule->m_NMAuth.GetData("Age")), 
					atoi(pAuthModule->m_NMAuth.GetData("Sex")), 
					atoi(pAuthModule->m_NMAuth.GetData("CCode")), 
					pQuery->m_bFreeLoginIP, 
					pQuery->m_nChecksumPack,
					pQuery->m_bCheckPremiumIP,
					pQuery->m_strIP.c_str(),
					pQuery->m_dwIP,
					pQuery->m_strCountryCode3);

				MMatchServer::GetInstance()->PostAsyncJob(pNewJob);
			}
			else
			{
				// XML ������ �ε� ���н� ������ ����ϰ�,
				pAuthModule->ErrorLog(error, "Netmarble Login Auth Failed (by XML)");

				// ��Ű�κ��� �����͸� ���� �����Ѵ�.
				error = pAuthModule->m_NMAuth.LoadDataFromCookie();

				if ( error == ERROR_NMAUTH_SUCCESS )
				{
					//todok del
					//MLog("Netmarble login Auth SUCCEED by CPCOOKIE\n");

					// DB�� ���� ������ ����
					MBMatchAsyncDBJob_NetmarbleLogin* pNewJob = new MBMatchAsyncDBJob_NetmarbleLogin(pQuery->m_commUid);
					pNewJob->Input(new MMatchAccountInfo(), 
						new MMatchAccountPenaltyInfo,
						pAuthModule->m_NMAuth.GetData("UserID"), 
						pAuthModule->m_NMAuth.GetData("UniID"), 
						pAuthModule->m_NMAuth.GetData("Certificate"), 
						pAuthModule->m_NMAuth.GetData("Name"), 
						atoi(pAuthModule->m_NMAuth.GetData("Age")), 
						atoi(pAuthModule->m_NMAuth.GetData("Sex")), 
						atoi(pAuthModule->m_NMAuth.GetData("CCode")), 
						pQuery->m_bFreeLoginIP, 
						pQuery->m_nChecksumPack,
						pQuery->m_bCheckPremiumIP,
						pQuery->m_strIP.c_str(),
						pQuery->m_dwIP,
						pQuery->m_strCountryCode3);

					MMatchServer::GetInstance()->PostAsyncJob(pNewJob);
				}
				else
				{
					// ����ó��
					MGetServerStatusSingleton()->SetRunStatus(5);

					// ������ �̷��� �Ǿ� �־��µ� �����忡 �������� �ʾƼ� �ϴ� ��
					//MCommand* pCmd = MGetMatchServer()->CreateCmdMatchResponseLoginFailed(pQuery->m_commUid, MERR_CLIENT_WRONG_PASSWORD);
					//MGetMatchServer()->Post(pCmd);

					MLog("Netmarble Certification Failed\n");
				}
			}
		}

		delete pQuery;
	}
	return (0);
} 

#endif