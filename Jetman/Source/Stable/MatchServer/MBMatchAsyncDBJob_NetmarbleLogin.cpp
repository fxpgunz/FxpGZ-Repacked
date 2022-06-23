#include "StdAfx.h"
#include "MBMatchAsyncDBJob_NetmarbleLogin.h"
#include "MMatchPremiumIPCache.h"
#include "MMatchConfig.h"

void MBMatchAsyncDBJob_NetmarbleLogin::Run(void* pContext)
{
	_ASSERT(m_pAccountInfo);

	MMatchDBMgr* pDBMgr = (MMatchDBMgr*)pContext;

	// ���� ������ �ݸ��� �����Ƿ� �ش� ������ ������ ���� �����Ѵ�. 
	unsigned int nOutCCode = 0;
	if (!pDBMgr->GetLoginInfo_Netmarble(m_szUserID, &m_nAID, &nOutCCode, m_szUniqueID))
	{
		int nGunzSex;	// �������� �������� �ݸ��� �������� �ݴ��̴�.
		if (m_nSex == 0)	nGunzSex = 1; 
		else				nGunzSex = 0;

		pDBMgr->CreateAccount_Netmarble(m_szUserID, m_szUniqueID, m_nAge, nGunzSex, m_nCCode, &m_nErrCode);
		if( m_nErrCode != 0 ) {
			SetResult(MASYNC_RESULT_FAILED);
			return;
		}

		if( !pDBMgr->GetLoginInfo_Netmarble(m_szUserID, &m_nAID, &nOutCCode, m_szUniqueID) ) {
			SetResult(MASYNC_RESULT_FAILED);
			return;
		}
	}

	if( nOutCCode != m_nCCode ) {
		mlog("CCode is invalid(%s, %d, %d)\n", m_szUserID, nOutCCode, m_nCCode);
		SetResult(MASYNC_RESULT_FAILED);
		return;
	} else {
		m_pAccountInfo->m_nCCode = m_nCCode;
	}

	// ���� ������ �д´�.
	if (!pDBMgr->GetAccountInfo(m_nAID, m_pAccountInfo, MGetServerConfig()->GetServerID()))
	{
		SetResult(MASYNC_RESULT_FAILED);
		return;
	}

	// ���� ���Ƽ ������ �д´�. - 2010-08-10 ȫ����
	if( !pDBMgr->GetAccountPenaltyInfo(m_nAID, m_pAccountPenaltyInfo) ) 
	{
		SetResult(MASYNC_RESULT_FAILED);
		return;
	}


	// �����̾� IP�� üũ�Ѵ�.
	if (m_bCheckPremiumIP) {
		bool bIsPremiumIP = false;
		bool bExistPremiumIPCache = false;

		bExistPremiumIPCache = MPremiumIPCache()->CheckPremiumIP(m_dwIP, bIsPremiumIP);

		// ���� ĳ���� ������ ���� DB���� ã���� �Ѵ�.
		if (!bExistPremiumIPCache) {
			if (pDBMgr->CheckPremiumIP(m_szIP, bIsPremiumIP)) {
				// ����� ĳ���� ����
				MPremiumIPCache()->AddIP(m_dwIP, bIsPremiumIP);
			} else {
				MPremiumIPCache()->OnDBFailed();
			}

		}

		if (bIsPremiumIP) m_pAccountInfo->m_nPGrade = MMPG_PREMIUM_IP;
	}

	SetResult(MASYNC_RESULT_SUCCEED);
}


bool MBMatchAsyncDBJob_NetmarbleLogin::Input(MMatchAccountInfo* pNewAccountInfo,
									 MMatchAccountPenaltyInfo* pNewAccountPenaltyInfo,
									 const char* szUserID, 
									 const char* szUniqueID,
									 const int nAge, 
									 const int nSex,
									 const int nCCode,
									 const bool bFreeLoginIP,
									 unsigned long nChecksumPack,
									 const bool bCheckPremiumIP,
									 const char* szIP,
									 DWORD dwIP,
									 const string& strCountryCode3)
{
	m_pAccountInfo			= pNewAccountInfo;
	m_pAccountPenaltyInfo	= pNewAccountPenaltyInfo;

	strcpy(m_szUserID, szUserID);
	strcpy(m_szUniqueID, szUniqueID);
	m_nAge = nAge;
	m_nSex = nSex;
	m_nCCode = nCCode;
	m_bFreeLoginIP = bFreeLoginIP;
	m_nChecksumPack = nChecksumPack;
	m_bCheckPremiumIP = bCheckPremiumIP;
	strcpy(m_szIP, szIP);
	m_dwIP = dwIP;
	if( 3 == strCountryCode3.length() )
		m_strCountryCode3 = strCountryCode3;
	else
		m_strCountryCode3 = "Err";

	return true;
}