#include "stdafx.h"
#include "MAsyncDBJob_GetLoginInfo.h"
#include "MMatchPremiumIPCache.h"
#include "MMatchConfig.h"

void MAsyncDBJob_GetLoginInfo::Run(void* pContext)
{
	_ASSERT(m_pAccountInfo);

	MMatchDBMgr* pDBMgr = (MMatchDBMgr*)pContext;

	// 원래 계정은 넷마블에 있으므로 해당 계정이 없으면 새로 생성한다. 
	if (!pDBMgr->GetLoginInfo(m_szUserID, &m_nAID, m_szDBPassword))
	{
		SetResult(MASYNC_RESULT_FAILED);
		return;
	}

	// 계정 정보를 읽는다.
	if (!pDBMgr->GetAccountInfo(m_nAID, m_pAccountInfo, MGetServerConfig()->GetServerID()))
	{
		SetResult(MASYNC_RESULT_FAILED);
		return;
	}

	// 계정 페널티 정보를 읽는다. - 2010-08-10 홍기주
	if( !pDBMgr->GetAccountPenaltyInfo(m_nAID, m_pAccountPenaltyInfo) ) 
	{
		SetResult(MASYNC_RESULT_FAILED);
		return;
	}

	// 프리미엄 IP를 체크한다.
	if (m_bCheckPremiumIP)
	{
		bool bIsPremiumIP = false;
		bool bExistPremiumIPCache = false;
		
		bExistPremiumIPCache = MPremiumIPCache()->CheckPremiumIP(m_dwIP, bIsPremiumIP);

		// 만약 캐쉬에 없으면 직접 DB에서 찾도록 한다.
		if (!bExistPremiumIPCache)
		{
			if (pDBMgr->CheckPremiumIP(m_szIP, bIsPremiumIP))
			{
				// 결과를 캐쉬에 저장
				MPremiumIPCache()->AddIP(m_dwIP, bIsPremiumIP);
			}
			else
			{
				MPremiumIPCache()->OnDBFailed();
			}

		}

		if (bIsPremiumIP) m_pAccountInfo->m_nPGrade = MMPG_PREMIUM_IP;
	}

	SetResult(MASYNC_RESULT_SUCCEED);
}


bool MAsyncDBJob_GetLoginInfo::Input(MMatchAccountInfo* pNewAccountInfo,
			MMatchAccountPenaltyInfo* pNewAccountPenaltyInfo,
			const char* szUserID, 
			const char* szUniqueID, 
			const char* szCertificate, 
			const char* szName, 
			const int nAge, 
			const int nSex,
			const bool bFreeLoginIP,
			unsigned long nChecksumPack,
			const bool bCheckPremiumIP,
			const char* szIP,
            DWORD dwIP,
			const string& strCountryCode3)
{
	m_pAccountInfo = pNewAccountInfo;
	m_pAccountPenaltyInfo = pNewAccountPenaltyInfo;

	strcpy_s(m_szUserID, szUserID);
	strcpy_s(m_szUniqueID, szUniqueID);
	strcpy_s(m_szCertificate, szCertificate);
	strcpy_s(m_szName, szName);
	m_nAge = nAge;
	m_nSex = nSex;
	m_bFreeLoginIP = bFreeLoginIP;
	m_nChecksumPack = nChecksumPack;
	m_bCheckPremiumIP = bCheckPremiumIP;
	strcpy_s(m_szIP, szIP);
	m_dwIP = dwIP;
	if( 3 == strCountryCode3.length() )
		m_strCountryCode3 = strCountryCode3;
	else
		m_strCountryCode3 = "Err";

	return true;
}
