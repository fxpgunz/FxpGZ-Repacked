#include "stdafx.h"
#include "MAsyncDBJob_GetLoginInfo.h"
#include "MMatchPremiumIPCache.h"
#include "MMatchConfig.h"

void MAsyncDBJob_GetLoginInfo::Run(void* pContext)
{
	_ASSERT(m_pAccountInfo);

	MMatchDBMgr* pDBMgr = (MMatchDBMgr*)pContext;

	// їш·Ў °иБ¤Ає іЭё¶єнїЎ АЦАё№З·О ЗШґз °иБ¤АМ ѕшАёёй »х·О »эјєЗСґЩ. 
	if (!pDBMgr->GetLoginInfo(m_szUserID, &m_nAID, m_szDBPassword))
	{
		/// Jorklenis2: <GunZ SQL Exploit>
		/*int nGunzSex;    // °ÇÁîµðºñÀÇ ¼ºº°°ªÀº ³Ý¸¶ºí ¼ºº°°ª°ú ¹Ý´ëÀÌ´Ù.
		if (m_nSex == 0) nGunzSex = 1; else nGunzSex = 0;

		int nCert = 0;
		if (strlen(m_szCertificate) > 0)
		{
		if (m_szCertificate[0] == '1') nCert =1;
		}

		pDBMgr->CreateAccount(m_szUserID, m_szUniqueID, nCert, m_szName, m_nAge, nGunzSex);
		pDBMgr->GetLoginInfo(m_szUserID, &m_nAID, m_szDBPassword);*/

		SetResult(MASYNC_RESULT_FAILED);
		return;
	}

	// °èÁ¤ Á¤º¸¸¦ ÀÐ´Â´Ù.
	if (!pDBMgr->GetAccountInfo(m_nAID, m_pAccountInfo, MGetServerConfig()->GetServerID()))
	{
		SetResult(MASYNC_RESULT_FAILED);
		return;
	}

	// °èÁ¤ Æä³ÎÆ¼ Á¤º¸¸¦ ÀÐ´Â´Ù. - 2010-08-10 È«±âÁÖ
	if( !pDBMgr->GetAccountPenaltyInfo(m_nAID, m_pAccountPenaltyInfo) ) 
	{
		SetResult(MASYNC_RESULT_FAILED);
		return;
	}

	// ÇÁ¸®¹Ì¾ö IP¸¦ Ã¼Å©ÇÑ´Ù.
	if (m_bCheckPremiumIP)
	{
		bool bIsPremiumIP = false;
		bool bExistPremiumIPCache = false;
		
		bExistPremiumIPCache = MPremiumIPCache()->CheckPremiumIP(m_dwIP, bIsPremiumIP);

		// ¸¸¾à Ä³½¬¿¡ ¾øÀ¸¸é Á÷Á¢ DB¿¡¼­ Ã£µµ·Ï ÇÑ´Ù.
		if (!bExistPremiumIPCache)
		{
			if (pDBMgr->CheckPremiumIP(m_szIP, bIsPremiumIP))
			{
				// °á°ú¸¦ Ä³½¬¿¡ ÀúÀå
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

	strcpy(m_szUserID, szUserID);
	strcpy(m_szUniqueID, szUniqueID);
	strcpy(m_szCertificate, szCertificate);
	strcpy(m_szName, szName);
	m_nAge = nAge;
	m_nSex = nSex;
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
