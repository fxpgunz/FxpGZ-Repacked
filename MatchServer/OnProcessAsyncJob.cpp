#include "stdafx.h"
#include "MBMatchServer.h"
#include "MBMatchAsyncDBJob_NetmarbleLogin.h"
#include "MBMatchAsyncDBJob_NHNLogin.h"
#include "MBMatchAsyncDBJob_GameOnLogin.h"
#include "MSharedCommandTable.h"
#include "MBlobArray.h"
#include "MMatchFormula.h"
#include "MMatchConfig.h"
#include "MAsyncDBJob.h"
#include "MAsyncDBJob_FriendList.h"
#include "MAsyncDBJob_GetLoginInfo.h"
#include "MAsyncDBJob_InsertConnLog.h"
#include "MAsyncDBJob_Event.h"
#include "MBMatchAsyncDBJob_BuyBountyItem.h"
#include "MBMatchAsyncDBJob_SellItem.h"
#include "MBMatchAsyncDBJob_GambleItem.h"
#include "MBMatchAsyncDBJob_GambleQuestItem.h"
#include "MBMatchAsyncDBJob_BringAccountItem.h"
#include "MBMatchAsyncDBJob_BringAccountItemStackable.h"
#include "MBMatchAsyncDBJob_BringBackAccountItem.h"
#include "MBMatchAsyncDBJob_BringBackAccountItemStackable.h"
#include "MBMatchAsyncDBJob_UpdateEquipItem.h"
#include "MBMatchAsyncDBJob_TakeoffItem.h"
#include "RTypes.h"



#ifdef _MONITORING
#include "MMonitorCommand.h"
#include "MBMatchAsyncJob_GetCpuInfo.h"
#endif

#ifdef _GAMEGUARD
#include "MBMatchGGSvrRef.h"
#define SAFE_DELETE(p)  { if(p) { delete (p); (p)=NULL; } }
#endif


void MBMatchServer::OnProcessAsyncJob(MAsyncJob* pJob)
{
	switch (pJob->GetJobID())
	{
	case MASYNCJOB_NETMARBLE_KOR_LOGIN:
	{
		OnAsyncNetmarbleLogin(pJob);
	}
	break;

	case MASYNCJOB_NHNLOGIN:
	{
		OnAsyncNHNLogin(pJob);
	}
	break;

	case MASYNCJOB_GAMEONLOGIN:
	{
		OnAsyncGameOnLogin(pJob);
	}
	break;

	case MASYNCJOB_GETACCOUNTCHARINFO:
	{
		OnAsyncGetAccountCharInfo(pJob);
	}
	break;

	case MASYNCJOB_GETCHARINFO:
	{
		OnAsyncGetCharInfo(pJob);
	}
	break;

	case MASYNCJOB_SETBLOCKHACKING:
	{
		// Æ¯º°È÷ ÇÏ´Â ÀÛ¾÷Àº ¾øÀ½.
	}
	break;

	case MASYNCJOB_RESETACCOUNTHACKINGBLOCK:
	{
		// Æ¯º°È÷ ÇÏ´Â ÀÛ¾÷Àº ¾øÀ½.
	}
	break;

	case MASYNCJOB_UPDATCHARCLANCONTPOINT:
	{
		// Æ¯º°È÷ ÇÏ´Â ÀÛ¾÷Àº ¾øÀ½.
	}
	break;

	case MASYNCJOB_GETSYSTEMINFO:
	{
		OnAsyncGetCpuUsage(pJob);
	}
	break;

	case MASYNCJOB_BUYBOUNTRYITEM:
	{
		OnAsyncBuyBountyItem(pJob);
	}
	break;

	case MASYNCJOB_SELL_ITEM_TO_BOUNTY:
	{
		OnAsyncSelling_Item_ToBounty(pJob);
	}
	break;

	case MASYNCJOB_SELL_SPENDABLEITEM_TO_BOUNTY:
	{
		OnAsyncSelling_SpendableItem_ToBounty(pJob);
	}
	break;

	case MASYNCJOB_GAMBLE_ITEM:
	{
		OnAsyncGambleItem(pJob);
	}
	break;

	case MASYNCJOB_GAMBLE_QUESTITEM:
	{
		OnAsyncGambleQuestItem(pJob);
	}
	break;


	case MASYNCJOB_BRING_ACCOUNTITEM:
	{
		OnAsyncBringAccountItem(pJob);
	}
	break;

	case MASYNCJOB_BRING_ACCOUNTITEM_STACKABLE:
	{
		OnAsyncBringAccountItemStackable(pJob);
	}
	break;

	case MASYNCJOB_BRINGBACK_ACCOUNTITEM:
	{
		OnBringBackAccountItem(pJob);
	}
	break;

	case MASYNCJOB_BRINGBACK_ACCOUNTITEM_STACKABLE:
	{
		OnBringBackAccountItemStackable(pJob);
	}
	break;

	case MASYNCJOB_UPDATECHARLEVEL:
	{
		// ÇÒ°Ô ¾øÀ½.
	}
	break;

	case MASYNCJOB_UPDATEEQUIPITEM:
	{
		OnUpdateEquipItem(pJob);
	}
	break;

	case MASYNCJOB_TAKEOFFITEM:
	{
		OnTakeoffItem(pJob);
	}
	break;

	case MASYNCJOB_UPDATEACCOUNTLASTLOGINTIME:
	{
		// ÇÒÀÏÀÌ ¾øÀ½.
	}
	break;

	case MASYNCJOB_INSERT_CHAR_BUFF_INFO:
	case MASYNCJOB_UPDATE_CHAR_BUFF_INFO:
		break;

	default:
#ifdef _DEBUG
		mlog("invalid async job id : %d\n", pJob->GetJobID());
#endif
		ASSERT(0);
		break;
	}
}


void MBMatchServer::OnAsyncNetmarbleLogin(MAsyncJob* pJobInput)
{
	MBMatchAsyncDBJob_NetmarbleLogin* pJob = (MBMatchAsyncDBJob_NetmarbleLogin*)pJobInput;

	if (pJob->GetResult() != MASYNC_RESULT_SUCCEED)
	{
		// Notify Message ÇÊ¿ä -> ·Î±×ÀÎ °ü·Ã - ÇØ°á(Login Fail ¸Þ¼¼Áö ÀÌ¿ë)
		// Disconnect(pJob->GetCommUID());
		int nErrCode = pJob->GetErrorCode();

		// 0ÀÌ¸é Á¤»ó, -1ÀÌ¸é Default°ª, 1~6ÀÌ¸é ¿¡·¯ ÄÚµå!
		if (nErrCode > 0 && nErrCode < 10) {
			LOG(LOG_PROG, "OnAsyncNetmarbleLogin - Can't Make UserID(ID=%s, AID=%u, Code=%d)\n",
				pJob->GetAccountInfo()->m_szUserID, pJob->GetAID(), nErrCode);

		}

		MCommand* pCmd = CreateCmdMatchResponseLoginFailed(pJob->GetCommUID(), MERR_FAILED_NETMARBLE_KOR_CANT_GET_ACCOUNTINFO);
		Post(pCmd);

		pJob->DeleteMemory();
		return;
	}

	MMatchAccountInfo* pAccountInfo = pJob->GetAccountInfo();
	if (pAccountInfo == 0) return;

	MMatchAccountPenaltyInfo* pAccountPenaltyInfo = pJob->GetAccountPenaltyInfo();
	if (pAccountPenaltyInfo == 0) return;

#ifndef _DEBUG
	// Áßº¹ ·Î±×ÀÎÀÌ¸é ÀÌÀü¿¡ ÀÖ´ø »ç¶÷À» ²÷¾î¹ö¸°´Ù.
	MMatchObject* pCopyObj = GetPlayerByAID(pAccountInfo->m_nAID);
	if (pCopyObj != NULL)
	{
		// Notify Message ÇÊ¿ä -> ·Î±×ÀÎ °ü·Ã - ÇØ°á(Æ¯º°ÇÑ ¸Þ¼¼Áö ÇÊ¿ä ¾øÀ½)
		// Áßº¹ Á¢¼Ó¿¡ °üÇÑ °ÍÀº ÀÌÀü Á¢¼ÓÀÚÀÇ Á¢¼ÓÀ» ÇØÁöÇÏ´Â °ÍÀÌ¹Ç·Î,
		// Æ¯º°ÇÑ ¿À·ù ÆÐÅ¶À» ¸¸µéÁö ¾Ê´Â´Ù.
		Disconnect(pCopyObj->GetUID());
	}
#endif

	// »ç¿ëÁ¤Áö °èÁ¤ÀÎÁö È®ÀÎÇÑ´Ù.
	if ((pAccountInfo->m_nUGrade == MMUG_BLOCKED) || (pAccountInfo->m_nUGrade == MMUG_PENALTY))
	{
		MCommand* pCmd = CreateCmdMatchResponseLoginFailed(pJob->GetCommUID(), MERR_CLIENT_MMUG_BLOCKED);
		Post(pCmd);

		pJob->DeleteMemory();
		return;
	}

	AddObjectOnMatchLogin(pJob->GetCommUID(), pJob->GetAccountInfo(), pJob->GetAccountPenaltyInfo()
		, pJob->IsFreeLoginIP(), pJob->GetCountryCode3(), pJob->GetChecksumPack());
}

void MBMatchServer::OnAsyncNHNLogin(MAsyncJob* pJob)
{
	if (0 == pJob) return;

	MBMatchAsyncDBJob_NHNLogin* pNHNJob = static_cast< MBMatchAsyncDBJob_NHNLogin* >(pJob);

	if (MASYNC_RESULT_SUCCEED != pNHNJob->GetResult())
	{
		if (0 != pNHNJob->GetAccountInfo()->m_szUserID)
			LOG(LOG_PROG, "NHN USA Login Fail:UserID=%s", pNHNJob->GetAccountInfo()->m_szUserID);

		MCommand* pCmd = CreateCmdMatchResponseLoginFailed(pNHNJob->GetUID(), MERR_FAILED_NHNUSA_INVALID_AUTHSTR);
		Post(pCmd);

		pNHNJob->DeleteMemory();
		return;
	}


	const MMatchAccountInfo* pAccountInfo = pNHNJob->GetAccountInfo();
	if (0 == pAccountInfo) return;

	const MMatchAccountPenaltyInfo* pAccountPenaltyInfo = pNHNJob->GetAccountPenaltyInfo();
	if (0 == pAccountPenaltyInfo) return;


	// Áßº¹ ·Î±ä °Ë»ç.
	MMatchObject* pCopyObj = GetPlayerByAID(pNHNJob->GetAID());
	if (0 != pCopyObj)
	{
		// ÀÌÀü Á¢¼Ó Á¾·á.	
		LOG(LOG_PROG, "Duplicated Login. ID=%s, AID=%u\n",
			pNHNJob->GetAccountInfo()->m_szUserID, pNHNJob->GetAID());

		// Notify Message ÇÊ¿ä -> ·Î±×ÀÎ °ü·Ã - ÇØ°á(Æ¯º°ÇÑ ¸Þ¼¼Áö ÇÊ¿ä ¾øÀ½)
		// Áßº¹ Á¢¼Ó¿¡ °üÇÑ °ÍÀº ÀÌÀü Á¢¼ÓÀÚÀÇ Á¢¼ÓÀ» ÇØÁöÇÏ´Â °ÍÀÌ¹Ç·Î,
		// Æ¯º°ÇÑ ¿À·ù ÆÐÅ¶À» ¸¸µéÁö ¾Ê´Â´Ù.
		Disconnect(pCopyObj->GetUID());
	}

	// ¿ìÀú µî±ÞÀ» È®ÀÎÇÑ´Ù.
	if ((MMUG_BLOCKED == pAccountInfo->m_nUGrade) || (MMUG_PENALTY == pAccountInfo->m_nUGrade))
	{
		MCommand* pCmd = CreateCmdMatchResponseLoginFailed(pNHNJob->GetUID(), MERR_CLIENT_MMUG_BLOCKED);
		Post(pCmd);

		pNHNJob->DeleteMemory();
		return;
	}

#ifdef _GAMEGUARD
	if (MGetServerConfig()->IsUseGamegaurd())
	{
		// ggtest
		MBMatchGameguard* pGameguard = new MBMatchGameguard(GetGlobalClockCount());
		if (0 == pGameguard)
		{
			LOG(LOG_PROG, "GAMEGUARD ERR :MBMatchGameguard memory alloc fail\n");

			// Notify Message ÇÊ¿ä -> ·Î±×ÀÎ °ü·Ã - ÇØ°á(Login Fail ¸Þ¼¼Áö ÀÌ¿ë)
			// Disconnect( pNHNJob->GetUID() );
			// ¼­¹öÂÊÀÇ ¹®Á¦ÀÌ¹Ç·Î, ·Î±×ÀÎ ½ÇÆÐ -> Àç½ÃµµÇÏ¶ó°í ¾Ë·ÁÁÖ´Â Message
			MCommand* pCmd = CreateCmdMatchResponseLoginFailed(pNHNJob->GetUID(), MERR_FAILED_LOGIN_RETRY);
			Post(pCmd);

			pNHNJob->DeleteMemory();
			return;
		}

		if (!pGameguard->Init())
		{
			// Notify Message ÇÊ¿ä -> ·Î±×ÀÎ °ü·Ã - ÇØ°á(Login Fail ¸Þ¼¼Áö ÀÌ¿ë)
			// Disconnect( pNHNJob->GetUID() );
			// ¼­¹öÂÊÀÇ ¹®Á¦ÀÌ¹Ç·Î, ·Î±×ÀÎ ½ÇÆÐ -> Àç½ÃµµÇÏ¶ó°í ¾Ë·ÁÁÖ´Â Message
			MCommand* pCmd = CreateCmdMatchResponseLoginFailed(pNHNJob->GetUID(), MERR_FAILED_LOGIN_RETRY);
			Post(pCmd);

			pNHNJob->DeleteMemory();
			SAFE_DELETE(pGameguard);
			return;
		}

		if (!pGameguard->CreateAuthQuery())
		{
			LOG(LOG_PROG, "GAMEGUARD ERR :create new gameguard data fail in nhn login async job. AID(%u), ErrorCode(%u)\n",
				pAccountInfo->m_nAID, pGameguard->GetLastError());

			// Notify Message ÇÊ¿ä -> ·Î±×ÀÎ °ü·Ã - ÇØ°á(Login Fail ¸Þ¼¼Áö ÀÌ¿ë)
			// Disconnect( pNHNJob->GetUID() );
			// ¼­¹öÂÊÀÇ ¹®Á¦ÀÌ¹Ç·Î, ·Î±×ÀÎ ½ÇÆÐ -> Àç½ÃµµÇÏ¶ó°í ¾Ë·ÁÁÖ´Â Message
			MCommand* pCmd = CreateCmdMatchResponseLoginFailed(pNHNJob->GetUID(), MERR_FAILED_LOGIN_RETRY);
			Post(pCmd);

			pNHNJob->DeleteMemory();
			SAFE_DELETE(pGameguard);
			return;
		}

		const GG_AUTH_DATA& AuthData = pGameguard->GetServerAuth()->GetAuthQuery();
		RequestFirstGameguardAuth(pNHNJob->GetUID()
			, AuthData.dwIndex
			, AuthData.dwValue1
			, AuthData.dwValue2
			, AuthData.dwValue3);

		if (!m_Security.GetManager(MSI_GAMEGUARD)->AddUserSecurityInfo(pNHNJob->GetUID(), pGameguard))
		{
			LOG(LOG_PROG, "GAMEGUARD ERR :add security info fail. AID(%u)\n", pAccountInfo->m_nAID);

			// Notify Message ÇÊ¿ä -> ·Î±×ÀÎ °ü·Ã - ÇØ°á(Login Fail ¸Þ¼¼Áö ÀÌ¿ë)
			// Disconnect( pNHNJob->GetUID() );
			// ¼­¹öÂÊÀÇ ¹®Á¦ÀÌ¹Ç·Î, ·Î±×ÀÎ ½ÇÆÐ -> Àç½ÃµµÇÏ¶ó°í ¾Ë·ÁÁÖ´Â Message
			MCommand* pCmd = CreateCmdMatchResponseLoginFailed(pNHNJob->GetUID(), MERR_FAILED_LOGIN_RETRY);
			Post(pCmd);

			pNHNJob->DeleteMemory();
			SAFE_DELETE(pGameguard);
			return;
		}
	}
#endif

	AddObjectOnMatchLogin(pNHNJob->GetUID(), pNHNJob->GetAccountInfo(), pNHNJob->GetAccountPenaltyInfo()
		, pNHNJob->IsFreeLoginIP(), pNHNJob->GetCountryCode3(), pNHNJob->GetCheckSumPak());

#ifdef _GAMEGUARD
	if (MGetServerConfig()->IsUseGamegaurd())
	{
		// ¹Ýµå½Ã AddObjectOnMatchLoginÇÔ¼ö°¡ ¸ÕÀú È£ÃâÀÌ µÇ¾î¾ß MMatchObjectÆ®°¡ µî·ÏÀÌ µÇ¾î ÀÖ´Ù.
		//  ÀÌÀü¿¡ È£ÃâÀ» ÇÏ¸é GetObject(...)¿¡¼­ NULLÀ» ¹ÝÈ¯ÇÔ.
		MMatchObject* pPlayer = GetObject(pNHNJob->GetUID());
		if (NULL == pPlayer)
		{
			_ASSERT(0);

			m_Security.GetManager(MSI_GAMEGUARD)->Delete(pNHNJob->GetUID());

			// Notify Message ÇÊ¿ä -> ·Î±×ÀÎ °ü·Ã - ÇØ°á(Login Fail ¸Þ¼¼Áö ÀÌ¿ë)
			// Disconnect( pNHNJob->GetUID() );
			// ¼­¹öÂÊÀÇ ¹®Á¦ÀÌ¹Ç·Î, ·Î±×ÀÎ ½ÇÆÐ -> Àç½ÃµµÇÏ¶ó°í ¾Ë·ÁÁÖ´Â Message
			MCommand* pCmd = CreateCmdMatchResponseLoginFailed(pNHNJob->GetUID(), MERR_FAILED_LOGIN_RETRY);
			Post(pCmd);

			pNHNJob->DeleteMemory();
			return;
		}

		pPlayer->FirstGameguardReqeustIsSent();
	}
#endif

	if (pNHNJob->GetAccountInfo())
	{
		pNHNJob->DeleteMemory();
	}
}

void MBMatchServer::OnAsyncGameOnLogin(MAsyncJob* pJob)
{
	if (0 == pJob) return;

	MBMatchAsyncDBJob_GameOnLogin* pGameOnJob = static_cast< MBMatchAsyncDBJob_GameOnLogin* >(pJob);

	if (MASYNC_RESULT_SUCCEED != pGameOnJob->GetResult())
	{
		mlog("GameOn Login fail");
		if (0 != pGameOnJob->GetAccountInfo()->m_szUserID)
			mlog(" : UserID=%s", pGameOnJob->GetAccountInfo()->m_szUserID);
		mlog("\n");
		pGameOnJob->DeleteMemory();
		return;
	}


	const MMatchAccountInfo* pAccountInfo = pGameOnJob->GetAccountInfo();
	if (0 == pAccountInfo) return;

	const MMatchAccountPenaltyInfo* pAccountPenaltyInfo = pGameOnJob->GetAccountPenaltyInfo();
	if (0 == pAccountPenaltyInfo) return;

	// Áßº¹ ·Î±ä °Ë»ç.

	MMatchObject* pCopyObj = GetPlayerByAID(pGameOnJob->GetAID());
	if (0 != pCopyObj)
	{
		// ÀÌÀü Á¢¼Ó Á¾·á.	
		mlog("Duplicated login. AID=%u\n", pGameOnJob->GetAID());
		MCommand* pCmd = CreateCmdMatchResponseLoginFailed(pCopyObj->GetUID(), MERR_MULTIPLE_LOGIN);
		Post(pCmd);
	}

	// ¿ìÀú µî±ÞÀ» È®ÀÎÇÑ´Ù.
	if ((MMUG_BLOCKED == pAccountInfo->m_nUGrade) || (MMUG_PENALTY == pAccountInfo->m_nUGrade))
	{
		MCommand* pCmd = CreateCmdMatchResponseLoginFailed(pGameOnJob->GetUID(), MERR_CLIENT_MMUG_BLOCKED);
		Post(pCmd);

		pGameOnJob->DeleteMemory();

		return;
	}


#ifdef _GAMEGUARD
	if (MGetServerConfig()->IsUseGamegaurd())
	{
		// ggtest
		MBMatchGameguard* pGameguard = new MBMatchGameguard(GetGlobalClockCount());
		if (0 == pGameguard)
		{
			LOG(LOG_PROG, "GAMEGUARD ERR :MBMatchGameguard memory alloc fail\n");

			// Notify Message ÇÊ¿ä -> ·Î±×ÀÎ °ü·Ã - ÇØ°á(Login Fail ¸Þ¼¼Áö ÀÌ¿ë)
			// Disconnect( pGameOnJob->GetUID() );
			// ¼­¹öÂÊÀÇ ¹®Á¦ÀÌ¹Ç·Î, ·Î±×ÀÎ ½ÇÆÐ -> Àç½ÃµµÇÏ¶ó°í ¾Ë·ÁÁÖ´Â Message
			MCommand* pCmd = CreateCmdMatchResponseLoginFailed(pGameOnJob->GetUID(), MERR_FAILED_LOGIN_RETRY);
			Post(pCmd);

			pGameOnJob->DeleteMemory();
			return;
		}

		if (!pGameguard->Init())
		{
			// Notify Message ÇÊ¿ä -> ·Î±×ÀÎ °ü·Ã - ÇØ°á(Login Fail ¸Þ¼¼Áö ÀÌ¿ë)
			// Disconnect( pGameOnJob->GetUID() );
			// ¼­¹öÂÊÀÇ ¹®Á¦ÀÌ¹Ç·Î, ·Î±×ÀÎ ½ÇÆÐ -> Àç½ÃµµÇÏ¶ó°í ¾Ë·ÁÁÖ´Â Message
			MCommand* pCmd = CreateCmdMatchResponseLoginFailed(pGameOnJob->GetUID(), MERR_FAILED_LOGIN_RETRY);
			Post(pCmd);

			pGameOnJob->DeleteMemory();
			SAFE_DELETE(pGameguard);
			return;
		}

		if (!pGameguard->CreateAuthQuery())
		{
			LOG(LOG_PROG, "GAMEGUARD ERR :create new gameguard data fail in nhn login async job. AID(%u), ErrorCode(%u)\n",
				pAccountInfo->m_nAID, pGameguard->GetLastError());

			// Notify Message ÇÊ¿ä -> ·Î±×ÀÎ °ü·Ã - ÇØ°á(Login Fail ¸Þ¼¼Áö ÀÌ¿ë)
			// Disconnect( pGameOnJob->GetUID() );
			// ¼­¹öÂÊÀÇ ¹®Á¦ÀÌ¹Ç·Î, ·Î±×ÀÎ ½ÇÆÐ -> Àç½ÃµµÇÏ¶ó°í ¾Ë·ÁÁÖ´Â Message
			MCommand* pCmd = CreateCmdMatchResponseLoginFailed(pGameOnJob->GetUID(), MERR_FAILED_LOGIN_RETRY);
			Post(pCmd);

			pGameOnJob->DeleteMemory();
			SAFE_DELETE(pGameguard);
			return;
		}

		const GG_AUTH_DATA& AuthData = pGameguard->GetServerAuth()->GetAuthQuery();
		RequestFirstGameguardAuth(pGameOnJob->GetUID()
			, AuthData.dwIndex
			, AuthData.dwValue1
			, AuthData.dwValue2
			, AuthData.dwValue3);

		if (!m_Security.GetManager(MSI_GAMEGUARD)->AddUserSecurityInfo(pGameOnJob->GetUID(), pGameguard))
		{
			LOG(LOG_PROG, "GAMEGUARD ERR :add security info fail. AID(%u)\n", pAccountInfo->m_nAID);

			// Notify Message ÇÊ¿ä -> ·Î±×ÀÎ °ü·Ã(º¸·ù - GameOn)
			// Disconnect( pGameOnJob->GetUID() );
			// ¼­¹öÂÊÀÇ ¹®Á¦ÀÌ¹Ç·Î, ·Î±×ÀÎ ½ÇÆÐ -> Àç½ÃµµÇÏ¶ó°í ¾Ë·ÁÁÖ´Â Message
			MCommand* pCmd = CreateCmdMatchResponseLoginFailed(pGameOnJob->GetUID(), MERR_FAILED_LOGIN_RETRY);
			Post(pCmd);

			pGameOnJob->DeleteMemory();
			SAFE_DELETE(pGameguard);
			return;
		}
	}
#endif


	AddObjectOnMatchLogin(pGameOnJob->GetUID(), pGameOnJob->GetAccountInfo(), pGameOnJob->GetAccountPenaltyInfo()
		, pGameOnJob->IsFreeLoginIP(), pGameOnJob->GetCountryCode3(), pGameOnJob->GetCheckSumPak());

#ifdef _GAMEGUARD
	if (MGetServerConfig()->IsUseGamegaurd())
	{
		// ¹Ýµå½Ã AddObjectOnMatchLoginÇÔ¼ö°¡ ¸ÕÀú È£ÃâÀÌ µÇ¾î¾ß MMatchObjectÆ®°¡ µî·ÏÀÌ µÇ¾î ÀÖ´Ù.
		//  ÀÌÀü¿¡ È£ÃâÀ» ÇÏ¸é GetObject(...)¿¡¼­ NULLÀ» ¹ÝÈ¯ÇÔ.
		MMatchObject* pPlayer = GetObject(pGameOnJob->GetUID());
		if (NULL == pPlayer)
		{
			_ASSERT(0);

			m_Security.GetManager(MSI_GAMEGUARD)->Delete(pGameOnJob->GetUID());

			// Notify Message ÇÊ¿ä -> ·Î±×ÀÎ °ü·Ã - ÇØ°á(Login Fail ¸Þ¼¼Áö ÀÌ¿ë)
			// Disconnect( pGameOnJob->GetUID() );
			// ¼­¹öÂÊÀÇ ¹®Á¦ÀÌ¹Ç·Î, ·Î±×ÀÎ ½ÇÆÐ -> Àç½ÃµµÇÏ¶ó°í ¾Ë·ÁÁÖ´Â Message
			MCommand* pCmd = CreateCmdMatchResponseLoginFailed(pGameOnJob->GetUID(), MERR_FAILED_LOGIN_RETRY);
			Post(pCmd);

			pGameOnJob->DeleteMemory();
			return;
		}

		pPlayer->FirstGameguardReqeustIsSent();
	}
#endif

}


void MBMatchServer::OnAsyncGetAccountCharInfo(MAsyncJob* pJobResult)
{
	MAsyncDBJob_GetAccountCharInfo* pJob = (MAsyncDBJob_GetAccountCharInfo*)pJobResult;

	if (pJob->GetResult() != MASYNC_RESULT_SUCCEED) {
		char szTime[128] = "";
		_strtime(szTime);

		mlog("[%s] Async DB Query(ResponseAccountCharInfo) Failed\n", szTime);
		return;
	}

	MMatchObject* pObj = GetObject(pJob->GetUID());
	if (pObj == NULL) return;

	MCommand* pNewCmd = CreateCommand(MC_MATCH_RESPONSE_ACCOUNT_CHARINFO, MUID(0, 0));
	pNewCmd->AddParameter(new MCommandParameterChar((char)pJob->GetCharNum()));

	void* pCharArray = MMakeBlobArray(sizeof(MTD_CharInfo), 1);

	MTD_CharInfo* pTransCharInfo = (MTD_CharInfo*)MGetBlobArrayElement(pCharArray, 0);
	memcpy(pTransCharInfo, pJob->GetCharInfo(), sizeof(MTD_CharInfo));

	pNewCmd->AddParameter(new MCommandParameterBlob(pCharArray, MGetBlobArraySize(pCharArray)));
	MEraseBlobArray(pCharArray);

	RouteToListener(pObj, pNewCmd);
}


void MBMatchServer::OnAsyncGetCharInfo(MAsyncJob* pJobResult)
{
	MAsyncDBJob_GetCharInfo* pJob = (MAsyncDBJob_GetCharInfo*)pJobResult;

	if (pJob->GetResult() != MASYNC_RESULT_SUCCEED) {
		mlog("DB Query(OnAsyncGetCharInfo > GetCharInfoByAID) Failed\n");
		return;
	}

	MMatchObject* pObj = GetObject(pJob->GetUID());
	if (pObj == NULL) return;

	if (pObj->GetCharInfo()) {
		// ÀÌÀü¿¡ Ä³¸¯ÀÌ ¼±ÅÃµÇ¾î ÀÖ¾ú´Ù¸é Ä³¸¯³¡³¯¶§ ·Î±× ³²±ä´Ù
		CharFinalize(pObj->GetUID());		// Ä³¸¯³¡³¯¶§ µðºñ ·Î±× µî Ã³¸®
		pObj->FreeCharInfo();
		pObj->FreeDuelTournamentInfo();
		pObj->FreeFriendInfo();
		pObj->FreePlayerWarsInfo();

		pObj->FreeCharBuff();
	}

	if (pJob->GetCharInfo() == NULL)
	{
		mlog("pJob->GetCharInfo() IS NULL\n");
		return;
	}

	pObj->SetCharInfo(pJob->GetCharInfo());		// Save Async Result 
	//	pObj->SetFriendInfo(pJob->GetFriendInfo());	// Save Async Result

	if (CharInitialize(pJob->GetUID()) == false)
	{
		mlog("OnAsyncGetCharInfo > CharInitialize failed");
		return;
	}

	//--------------------------------------------------------------------------------
	SendDuelTournamentCharInfoToPlayer(pJob->GetUID());
	SendDuelTournamentPreviousCharInfoToPlayer(pJob->GetUID());
	//--------------------------------------------------------------------------------
	SendPlayerWarsCharInfoToPlayer(pJob->GetUID());
	SendPlayerWarsSideRankingToPlayer(pJob->GetUID());

	MCommand* pNewCmd = CreateCommand(MC_MATCH_RESPONSE_SELECT_CHAR, MUID(0, 0));
	pNewCmd->AddParameter(new MCommandParameterInt(MOK));		// result

	// ³» ÄÉ¸¯ÅÍÀÇ Á¤º¸
	void* pCharInfoArray = MMakeBlobArray(sizeof(MTD_CharInfo), 1);
	MTD_CharInfo* pTransCharInfo = (MTD_CharInfo*)MGetBlobArrayElement(pCharInfoArray, 0);
	CopyCharInfoForTrans(pTransCharInfo, pJob->GetCharInfo(), pObj);
	pNewCmd->AddParameter(new MCommandParameterBlob(pCharInfoArray, MGetBlobArraySize(pCharInfoArray)));

	//¹öÇÁÁ¤º¸ÀÓ½ÃÁÖ¼® 
	/*// ³» ÄÉ¸¯ÅÍÀÇ ¹öÇÁ Á¤º¸
	void* pCharBuffInfoArray = MMakeBlobArray(sizeof(MTD_CharBuffInfo), 1);
	MTD_CharBuffInfo* pTransCharBuffInfo = (MTD_CharBuffInfo*)MGetBlobArrayElement(pCharBuffInfoArray, 0);
	CopyCharBuffInfoForTrans(pTransCharBuffInfo, pJob->GetCharInfo(), pObj);
	pNewCmd->AddParameter(new MCommandParameterBlob(pCharBuffInfoArray, MGetBlobArraySize(pCharBuffInfoArray)));
	*/

	// ³» Ä³¸¯ÅÍÀÇ Ãß°¡ Á¤º¸
	void* pCharExtraInfoArray = MMakeBlobArray(sizeof(MTD_MyExtraCharInfo), 1);
	MTD_MyExtraCharInfo* pMyExtraInfo = (MTD_MyExtraCharInfo*)MGetBlobArrayElement(pCharExtraInfoArray, 0);
	int nPercent = MMatchFormula::GetLevelPercent(pTransCharInfo->nXP, (int)pTransCharInfo->nLevel);
	pMyExtraInfo->nLevelPercent = (char)nPercent;
	pNewCmd->AddParameter(new MCommandParameterBlob(pCharExtraInfoArray, MGetBlobArraySize(pCharExtraInfoArray)));

	MEraseBlobArray(pCharInfoArray);
	MEraseBlobArray(pCharExtraInfoArray);
	//¹öÇÁÁ¤º¸ÀÓ½ÃÁÖ¼® MEraseBlobArray(pCharBuffInfoArray);

	RouteToListener(pObj, pNewCmd);
}

// CPU »ç¿ë·®À» Á¶»çÇÑ´Ù.
void MBMatchServer::OnAsyncGetCpuUsage(MAsyncJob* pJobResult)
{
#ifdef _MONITORING
	MBMatchAsyncJob_GetCpuInfo* pJob = (MBMatchAsyncJob_GetCpuInfo*)pJobResult;

	UINT uiCpuUsage = pJob->GetCpuUsage();

	m_Monitor.OnPostAsyncCpuUsage(pJob->GetProtocolID(), pJob->GetTaskName(), uiCpuUsage);

	pJob->SetResult(MASYNC_RESULT_SUCCEED);
#endif
}

void MBMatchServer::OnAsyncBuyBountyItem(MAsyncJob* pJobResult)
{
	MBMatchAsyncDBJob_BuyBountyItem* pJob = (MBMatchAsyncDBJob_BuyBountyItem*)(pJobResult);

	if (MASYNC_RESULT_SUCCEED != pJob->GetResult()) {
		MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_BUY_ITEM, pJob->GetPlayerUID());
		pNew->AddParameter(new MCmdParamInt(MERR_CANNOT_BUY_ITEM));
		PostSafeQueue(pNew);

		return;
	}

	MMatchObject* pObj = GetObject(pJob->GetPlayerUID());
	if (NULL == pObj) return;

	MMatchCharInfo* pCharInfo = pObj->GetCharInfo();
	if (NULL == pCharInfo) return;

	/// Jorklenis2: <Añadido pAccountInfo>
	MMatchAccountInfo* pAccountInfo = pObj->GetAccountInfo();
	if (NULL == pAccountInfo) return;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ¾ÆÀÌÅÛ Áö±Þ

	const DWORD			dwPrice = pJob->GetPrice();
	const WORD			wRentHourPeriod = pJob->GetRentHourPeriod();
	const bool			bIsGambleItem = pJob->IsGambleItem();
	const DWORD			dwNewCIID = pJob->GetNewCIID();
	const unsigned int	nItemID = pJob->GetItemID();
	const int			nItemCount = pJob->GetItemCount();

	WORD wRentMinPeriod = RENT_PERIOD_UNLIMITED;
	if (RENT_PERIOD_UNLIMITED != wRentHourPeriod) {
		wRentMinPeriod = (wRentHourPeriod * 60) - 1; // »óÁ¡¿¡¼­ »ç°í ÀÖ´Â »çÀÌ¿¡µµ ½Ã°£ÀÌ Áö³ª±â ¶§¹®¿¡.
	}

	if (!bIsGambleItem)
	{
		MMatchItemDesc* pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(nItemID);
		if (pItemDesc == NULL) return;

		MMatchItem* pItem = pCharInfo->m_ItemList.GetItemByCIID(dwNewCIID);
		if (pItem == NULL)
		{
			pCharInfo->m_ItemList.CreateItem(MMatchItemMap::UseUID(), dwNewCIID, nItemID,
				RENT_PERIOD_UNLIMITED != wRentHourPeriod, wRentMinPeriod, wRentHourPeriod, nItemCount);
		}
		else
		{
			if (pItem->GetDescID() == nItemID && pItemDesc->IsSpendableItem())
			{
				pItem->IncreaseCount(nItemCount);
			}
			else
			{
				mlog("OnAsyncBuyBountyItem - FAILED (ItemID(%d, %d), OutCIID(%d))\n", nItemID, pItem->GetDescID(), dwNewCIID);
				_ASSERT(0);
				return;
			}
		}
	}
	else {
		const MMatchCharGambleItem *pGItem = pCharInfo->m_GambleItemManager.GetGambleItemByCIID(dwNewCIID);
		if (pGItem == NULL) {
			pCharInfo->m_GambleItemManager.AddGambleItem(MMatchItemMap::UseUID(), dwNewCIID, nItemID, nItemCount);
		}
		else {
			pCharInfo->m_GambleItemManager.SetGambleItemCount(dwNewCIID, pGItem->GetItemCount() + nItemCount);

		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/// Jorklenis2: <Comprar artículos de cash>
	if (pJob->GetBuyMode() == 1)
		pAccountInfo->m_nCoins -= dwPrice;

	if (pJob->GetBuyMode() == 2)
		pAccountInfo->m_nECoins -= dwPrice;
	else
		pCharInfo->m_nBP -= dwPrice;

	UpdateCharDBCachingData(pObj);		///< XP, BP, KillCount, DeathCount Ä³½³ ¾÷µ¥ÀÌÆ®
	UpdateCharItemDBCachingData(pObj);	///< Character Item¿¡¼­ ¾÷µ¥ÀÌÆ®°¡ ÇÊ¿äÇÑ °Íµé ¾÷µ¥ÀÌÆ®

	MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_BUY_ITEM, pObj->GetUID());
	pNew->AddParameter(new MCmdParamInt(MOK));
	PostSafeQueue(pNew);

	ResponseCharacterItemList(pObj->GetUID());
}

void MBMatchServer::OnAsyncSelling_Item_ToBounty(MAsyncJob* pJobResult)
{
	MBMatchAsyncDBJob_SellItem* pJob = (MBMatchAsyncDBJob_SellItem*)pJobResult;
	if (MASYNC_RESULT_SUCCEED != pJob->GetResult())
	{
		MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_SELL_ITEM, pJob->GetPlayerUID());
		pNew->AddParameter(new MCmdParamInt(MERR_CANNOT_SELL_ITEM));
		PostSafeQueue(pNew);
		return;
	}

	MMatchObject* pObj = GetObject(pJob->GetPlayerUID());
	if (NULL == pObj) return;

	MMatchCharInfo* pCharInfo = pObj->GetCharInfo();
	if (NULL == pCharInfo) return;

	MUID& uidPlayer = pJob->GetPlayerUID();
	MUID& uidItem = pJob->GetItemUID();


	MMatchItem*	pCheckItem = pCharInfo->m_ItemList.GetItem(uidItem);
	if (NULL != pCheckItem)
	{
		// ¸¸¾à ºñµ¿±â ÀÛ¾÷ Áß¿¡ Âø¿ëÇß´Ù¸é... ÇØÁ¦½ÃÅ²´Ù.
		MMatchCharItemParts parts;
		if (pCharInfo->m_EquipedItem.IsEquipedItem(uidItem, parts)) {
			OnRequestTakeoffItem(uidPlayer, parts);
		}

		pObj->GetCharInfo()->m_ItemList.RemoveItem(uidItem);
		pObj->GetCharInfo()->m_nBP += pJob->GetSellPrice();

		MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_SELL_ITEM, uidPlayer);
		pNew->AddParameter(new MCmdParamInt(MOK));
		PostSafeQueue(pNew);

		ResponseCharacterItemList(uidPlayer);
	}


}

void MBMatchServer::OnAsyncSelling_SpendableItem_ToBounty(MAsyncJob* pJobResult)
{
	MBMatchAsyncDBJob_SellItem* pJob = (MBMatchAsyncDBJob_SellItem*)pJobResult;
	if (MASYNC_RESULT_SUCCEED != pJob->GetResult())
	{
		MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_SELL_ITEM, pJob->GetPlayerUID());
		pNew->AddParameter(new MCmdParamInt(MERR_CANNOT_SELL_ITEM));
		PostSafeQueue(pNew);
		return;
	}

	MMatchObject* pObj = GetObject(pJob->GetPlayerUID());
	if (NULL == pObj) return;

	MMatchCharInfo* pCharInfo = pObj->GetCharInfo();
	if (NULL == pCharInfo) return;

	MUID& uidPlayer = pJob->GetPlayerUID();
	MUID& uidItem = pJob->GetItemUID();

	MMatchItem*	pCheckItem = pCharInfo->m_ItemList.GetItem(uidItem);
	if (NULL != pCheckItem)
	{
		// ¸¸¾à ºñµ¿±â ÀÛ¾÷ Áß¿¡ Âø¿ëÇß´Ù¸é... ÇØÁ¦½ÃÅ²´Ù.
		MMatchCharItemParts parts;
		if (pCharInfo->m_EquipedItem.IsEquipedItem(uidItem, parts)) {
			OnRequestTakeoffItem(uidPlayer, parts);
		}

		int nSellItemCount = pJob->GetSellItemCount();
		int nItemTotalCount = pObj->GetCharInfo()->m_ItemList.GetItemCount(uidItem);

		if (nItemTotalCount == nSellItemCount) {
			pObj->GetCharInfo()->m_ItemList.RemoveItem(uidItem);
		}
		else if (nItemTotalCount > nSellItemCount) {
			MMatchItem *pItem = pObj->GetCharInfo()->m_ItemList.GetItem(uidItem);
			if (pItem == NULL)	return;
			pItem->DecreaseCount(nSellItemCount);
		}
		else {
			return;
		}

		pObj->GetCharInfo()->m_nBP += pJob->GetSellPrice();

		MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_SELL_ITEM, uidPlayer);
		pNew->AddParameter(new MCmdParamInt(MOK));
		PostSafeQueue(pNew);

		ResponseCharacterItemList(uidPlayer);
	}

	const MMatchCharGambleItem* pCharGItem = pObj->GetCharInfo()->m_GambleItemManager.GetGambleItemByUID(uidItem);
	if (pCharGItem != NULL)
	{
		int nSellItemCount = pJob->GetSellItemCount();
		int nItemTotalCount = pCharGItem->GetItemCount();

		if (nItemTotalCount == nSellItemCount) {
			pObj->GetCharInfo()->m_GambleItemManager.DeleteGambleItem(uidItem);
		}
		else if (nItemTotalCount > nSellItemCount) {
			pObj->GetCharInfo()->m_GambleItemManager.SetGambleItemCount(uidItem, nItemTotalCount - nSellItemCount);
		}
		else {
			return;
		}

		pObj->GetCharInfo()->m_nBP += pJob->GetSellPrice();

		MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_SELL_ITEM, uidPlayer);
		pNew->AddParameter(new MCmdParamInt(MOK));
		PostSafeQueue(pNew);

		ResponseCharacterItemList(uidPlayer);
	}
}

void MBMatchServer::OnAsyncGambleItem(MAsyncJob* pJobResult)
{
	MBMatchAsyncDBJob_GambleItem* pJob = (MBMatchAsyncDBJob_GambleItem*)pJobResult;
	if (MASYNC_RESULT_SUCCEED != pJob->GetResult()) return;

	MMatchObject* pObj = GetObject(pJob->GetPlayerUID());
	if (NULL == pObj) return;

	MMatchCharInfo* pCharInfo = pObj->GetCharInfo();
	if (NULL == pCharInfo) return;


	WORD		wRentMinPeriod = RENT_PERIOD_UNLIMITED;
	const MUID& uidPlayer = pJob->GetPlayerUID();
	const MUID& uidGItem = pJob->GetGItemUID();
	const DWORD dwCIID = pJob->GetRewardItemCIID();
	const DWORD dwRIID = pJob->GetRewardItemID();
	const DWORD dwItemCnt = pJob->GetRewardItemCnt();
	const WORD	wRentHourPeriod = pJob->GetRentHourPeriod();

	if (RENT_PERIOD_UNLIMITED != wRentHourPeriod)
		wRentMinPeriod = (wRentHourPeriod * 60) - 1; // »óÁ¡¿¡¼­ »ç°í ÀÖ´Â »çÀÌ¿¡µµ ½Ã°£ÀÌ Áö³ª±â ¶§¹®¿¡.

	MMatchItem* pItem = pCharInfo->m_ItemList.GetItemByCIID(dwCIID);
	if (pItem == NULL)
	{
		pCharInfo->m_ItemList.CreateItem(MMatchItemMap::UseUID(), dwCIID, dwRIID,
			RENT_PERIOD_UNLIMITED != wRentHourPeriod, wRentMinPeriod, wRentHourPeriod, dwItemCnt);
	}
	else
	{
		MMatchItemDesc* pDesc = MGetMatchItemDescMgr()->GetItemDesc(dwRIID);
		if (pItem->GetDescID() == dwRIID && pDesc->IsSpendableItem())
		{
			pItem->IncreaseCount(dwItemCnt);
		}
		else
		{
			mlog("OnAsyncGambleItem - FAILED (ItemID(%d, %d), OutCIID(%d))\n", dwRIID, pItem->GetDescID(), dwCIID);
			_ASSERT(0);
			return;
		}
	}

	const MMatchCharGambleItem* pGambleItem = pCharInfo->m_GambleItemManager.GetGambleItemByUID(uidGItem);
	if (pGambleItem == NULL) return;

	if (pGambleItem->GetItemCount() > 1) {
		const MMatchCharGambleItem*	pGItem = pCharInfo->m_GambleItemManager.GetGambleItemByUID(uidGItem);
		pCharInfo->m_GambleItemManager.SetGambleItemCount(pGItem->GetUID(), pGItem->GetItemCount() - 1);
	}
	else {
		pCharInfo->m_GambleItemManager.DeleteGambleItem(uidGItem);
	}

	// º¸»ó ¾ÆÀÌÅÛÀÇ ³»¿ëÀ» Å¬¶óÀÌ¾ðÆ®¿¡ Àü¼Û(¾ÆÀÌÅÛ ¾ÆÀÌµð, ±â°£(ºÐ´ÜÀ§))
	MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_GAMBLE, uidPlayer);
	pNew->AddParameter(new MCmdParamUInt(dwRIID));
	pNew->AddParameter(new MCmdParamUInt(dwItemCnt));
	pNew->AddParameter(new MCmdParamUInt(wRentHourPeriod * 60));

	PostSafeQueue(pNew);

	ResponseCharacterItemList(uidPlayer);
}


void MBMatchServer::OnAsyncGambleQuestItem(MAsyncJob* pJobResult)
{
	MBMatchAsyncDBJob_GambleQuestItem* pJob = (MBMatchAsyncDBJob_GambleQuestItem*)pJobResult;
	if (MASYNC_RESULT_SUCCEED != pJob->GetResult()) return;

	MMatchObject* pObj = GetObject(pJob->GetPlayerUID());
	if (NULL == pObj) return;

	MMatchCharInfo* pCharInfo = pObj->GetCharInfo();
	if (NULL == pCharInfo) return;


	const MUID& uidPlayer = pJob->GetPlayerUID();
	const MUID& uidGItem = pJob->GetGItemUID();
	const DWORD dwCIID = pJob->GetCIID();
	const DWORD dwRIID = pJob->GetRIID();


	MQuestItemMap::iterator itMyQItem = pCharInfo->m_QuestItemList.find(dwRIID);
	if (pCharInfo->m_QuestItemList.end() != itMyQItem) {
		const int nOver = itMyQItem->second->Increase(1);				//< ±âÁ¸¿¡ °¡Áö°í ÀÖ´ø Äù½ºÆ® ¾ÆÀÌÅÛ. ¼ö·®¸¸ Áõ°¡ ½ÃÄÑÁÖ¸é µÊ.
	}
	else {
		pCharInfo->m_QuestItemList.CreateQuestItem(dwRIID, 1);		///< Ã³À½ È¹µæÇÑ Äù½ºÆ® ¾ÆÀÌÅÛ. »õ·Î Ãß°¡½ÃÄÑ Áà¾ß ÇÔ.
	}

	const MMatchCharGambleItem* pGambleItem = pCharInfo->m_GambleItemManager.GetGambleItemByUID(uidGItem);
	if (pGambleItem == NULL) return;

	if (pGambleItem->GetItemCount() > 1) {
		const MMatchCharGambleItem*	pGItem = pCharInfo->m_GambleItemManager.GetGambleItemByUID(uidGItem);
		pCharInfo->m_GambleItemManager.SetGambleItemCount(pGItem->GetUID(), pGItem->GetItemCount() - 1);
	}
	else {
		pCharInfo->m_GambleItemManager.DeleteGambleItem(uidGItem);
	}

	// º¸»ó ¾ÆÀÌÅÛÀÇ ³»¿ëÀ» Å¬¶óÀÌ¾ðÆ®¿¡ Àü¼Û(¾ÆÀÌÅÛ ¾ÆÀÌµð, ±â°£(ºÐ´ÜÀ§))
	MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_GAMBLE, uidPlayer);
	pNew->AddParameter(new MCmdParamUInt(dwRIID));
	pNew->AddParameter(new MCmdParamUInt(0));
	PostSafeQueue(pNew);

	ResponseCharacterItemList(uidPlayer);
	OnRequestCharQuestItemList(uidPlayer);
}

void MBMatchServer::OnAsyncBringAccountItem(MAsyncJob* pJobResult)
{
	MBMatchAsyncDBJob_BringAccountItem* pJob = (MBMatchAsyncDBJob_BringAccountItem*)pJobResult;

	if (pJob->GetResult() != MASYNC_RESULT_SUCCEED)
	{
		MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_BRING_ACCOUNTITEM, pJob->GetUID());
		pNew->AddParameter(new MCmdParamInt(MERR_UNKNOWN));
		PostSafeQueue(pNew);
	}

	MMatchObject* pObj = GetObject(pJob->GetUID());
	if (!IsEnabledObject(pObj)) return;

	unsigned long int noutCIID = pJob->GetOutCIID();
	unsigned long int noutItemID = pJob->GetOutItemID();
	bool boutIsRentItem = pJob->GetOutIsRentItem();
	int noutRentMinutePeriodRemainder = pJob->GetOutRentMinutePeriodRemainder();
	DWORD woutRentHourPeriod = pJob->GetOutRentHourPeriod();

	MMatchItemDesc* pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(noutItemID);
	if (pItemDesc == NULL) return;

	pObj->GetCharInfo()->m_ItemList.CreateItem(MMatchItemMap::UseUID(), noutCIID, noutItemID,
		boutIsRentItem, noutRentMinutePeriodRemainder, woutRentHourPeriod);

	ResponseCharacterItemList(pJob->GetUID());

	MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_BRING_ACCOUNTITEM, pJob->GetUID());
	pNew->AddParameter(new MCmdParamInt(MOK));
	PostSafeQueue(pNew);
}

void MBMatchServer::OnAsyncBringAccountItemStackable(MAsyncJob* pJobResult)
{
	MBMatchAsyncDBJob_BringAccountItemStackable* pJob = (MBMatchAsyncDBJob_BringAccountItemStackable*)pJobResult;

	MMatchObject* pObj = GetObject(pJob->GetUID());
	if (!IsEnabledObject(pObj)) return;

	if (pJob->GetResult() != MASYNC_RESULT_SUCCEED)
	{
		MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_BRING_ACCOUNTITEM, pJob->GetUID());
		pNew->AddParameter(new MCmdParamInt(MERR_UNKNOWN));
		PostSafeQueue(pNew);
	}

	unsigned long int noutCIID = pJob->GetOutCIID();
	unsigned long int noutItemID = pJob->GetOutItemID();
	unsigned int noutItemCnt = pJob->GetOutItemCnt();
	bool boutIsRentItem = pJob->GetOutIsRentItem();
	int noutRentMinutePeriodRemainder = pJob->GetOutRentMinutePeriodRemainder();
	DWORD woutRentHourPeriod = pJob->GetOutRentHourPeriod();

	MMatchItemDesc* pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(noutItemID);
	if (pItemDesc)
	{
		MMatchItem *pItem = pObj->GetCharInfo()->m_ItemList.GetItemByCIID(noutCIID);
		if (pItem == NULL)
		{
			pObj->GetCharInfo()->m_ItemList.CreateItem(MMatchItemMap::UseUID(), noutCIID, noutItemID,
				boutIsRentItem, noutRentMinutePeriodRemainder, woutRentHourPeriod, noutItemCnt);
		}
		else
		{
			if (pItem->GetDescID() != noutItemID || !pItemDesc->IsSpendableItem())
			{
				mlog("OnAsyncBringAccountItemStackable - FAILED (ItemID(%d, %d), OutCIID(%d))\n", noutItemID, pItem->GetDescID(), noutCIID);
				return;
			}

			pItem->IncreaseCount(noutItemCnt);
		}
	}
	else
	{
		const MMatchGambleItem* pGItemDesc = GetGambleMachine().GetGambleItemByGambleItemID(noutItemID);
		if (pGItemDesc == NULL) return;

		const MMatchCharGambleItem* pItem = pObj->GetCharInfo()->m_GambleItemManager.GetGambleItemByCIID(noutCIID);
		if (pItem == NULL) pObj->GetCharInfo()->m_GambleItemManager.AddGambleItem(MMatchItemMap::UseUID(), noutCIID, noutItemID, noutItemCnt);
		else				pObj->GetCharInfo()->m_GambleItemManager.SetGambleItemCount(noutCIID, pItem->GetItemCount() + noutItemCnt);
	}

	ResponseCharacterItemList(pJob->GetUID());

	MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_BRING_ACCOUNTITEM, pJob->GetUID());
	pNew->AddParameter(new MCmdParamInt(MOK));
	PostSafeQueue(pNew);
}

void MBMatchServer::OnBringBackAccountItem(MAsyncJob* pJobResult)
{
	MBMatchAsyncDBJob_BringBackAccountItem* pJob = (MBMatchAsyncDBJob_BringBackAccountItem*)pJobResult;

	if (MASYNC_RESULT_SUCCEED != pJob->GetResult())
	{
		MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_BRING_BACK_ACCOUNTITEM, pJob->GetOwnerUID());
		pNew->AddParameter(new MCmdParamInt(MERR_BRING_BACK_ACCOUNTITEM));
		PostSafeQueue(pNew);
		return;
	}

	MMatchObject* pObj = GetObject(pJob->GetOwnerUID());
	if (NULL == pObj) return;

	MMatchCharInfo* pCharInfo = pObj->GetCharInfo();
	if (NULL == pCharInfo) return;

	const MUID& uidPlayer = pJob->GetOwnerUID();
	const MUID& uidItem = pJob->GetItemUID();

	MMatchCharItemParts parts;
	if (pCharInfo->m_EquipedItem.IsEquipedItem(uidItem, parts)) {
		OnRequestTakeoffItem(uidPlayer, parts);
	}

	pCharInfo->m_ItemList.RemoveItem(uidItem);

	MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_BRING_BACK_ACCOUNTITEM, uidPlayer);
	pNew->AddParameter(new MCmdParamInt(MOK));
	PostSafeQueue(pNew);

	ResponseCharacterItemList(uidPlayer);
}

void MBMatchServer::OnBringBackAccountItemStackable(MAsyncJob* pJobResult)
{
	MBMatchAsyncDBJob_BringBackAccountItemStackable* pJob = (MBMatchAsyncDBJob_BringBackAccountItemStackable*)pJobResult;

	if (MASYNC_RESULT_SUCCEED != pJob->GetResult())
	{
		MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_BRING_BACK_ACCOUNTITEM, pJob->GetOwnerUID());
		pNew->AddParameter(new MCmdParamInt(MERR_BRING_BACK_ACCOUNTITEM));
		PostSafeQueue(pNew);
		return;
	}

	MMatchObject* pObj = GetObject(pJob->GetOwnerUID());
	if (NULL == pObj) return;

	MMatchCharInfo* pCharInfo = pObj->GetCharInfo();
	if (NULL == pCharInfo)	return;

	const MUID& uidPlayer = pJob->GetOwnerUID();
	const MUID& uidItem = pJob->GetItemUID();

	MMatchItem *pItem = pCharInfo->m_ItemList.GetItem(uidItem);
	if (pItem)
	{
		unsigned int nRemainCnt = pItem->GetItemCount() - pJob->GetItemCnt();
		if (nRemainCnt > 0)
		{
			pItem->SetItemCount(nRemainCnt);
		}
		else
		{
			MMatchCharItemParts parts;
			if (pCharInfo->m_EquipedItem.IsEquipedItem(uidItem, parts)) {
				OnRequestTakeoffItem(uidPlayer, parts);
			}
			pCharInfo->m_ItemList.RemoveItem(uidItem);
		}
	}
	else
	{
		const MMatchCharGambleItem* pGambleItem = pCharInfo->m_GambleItemManager.GetGambleItemByUID(uidItem);
		if (NULL == pGambleItem) return;

		int nRemainCnt = pGambleItem->GetItemCount() - pJob->GetItemCnt();
		if (nRemainCnt > 0) pCharInfo->m_GambleItemManager.SetGambleItemCount(uidItem, nRemainCnt);
		else				pCharInfo->m_GambleItemManager.DeleteGambleItem(uidItem);
	}

	MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_BRING_BACK_ACCOUNTITEM, uidPlayer);
	pNew->AddParameter(new MCmdParamInt(MOK));
	PostSafeQueue(pNew);

	ResponseCharacterItemList(uidPlayer);
}

void MBMatchServer::OnUpdateEquipItem(MAsyncJob* pJobResult)
{
	//MBMatchAsyncDBJob_UpdateEquipItem* pJob = (MBMatchAsyncDBJob_UpdateEquipItem*)pJobResult;

	//int nResult = MOK;

	//if( MASYNC_RESULT_SUCCEED != pJob->GetResult() || !pJob->GetRet() ) 
	//{
	//	MCommand* pNew = CreateCommand( MC_MATCH_RESPONSE_EQUIP_ITEM, pJob->GetPlayerUID() );
	//	pNew->AddParameter(new MCommandParameterInt(MERR_CANNOT_EQUIP_ITEM));
	//	PostSafeQueue( pNew );

	//	return;
	//}

	//MMatchObject* pObj = GetObject( pJob->GetPlayerUID() );
	//if( NULL == pObj )
	//{
	//	return;
	//}

	//MMatchCharInfo* pCharInfo = pObj->GetCharInfo();
	//if( NULL == pCharInfo )
	//{
	//	return;
	//}

	//const MUID&					uidPlayer	= pJob->GetPlayerUID();
	//const MMatchCharItemParts	parts		= pJob->GetParts();
	//MMatchItem*					pItem		= pCharInfo->m_ItemList.GetItem( pJob->GetItemUID() );

	//MCommand* pNew = CreateCommand( MC_MATCH_RESPONSE_EQUIP_ITEM, uidPlayer );
	//pNew->AddParameter( new MCommandParameterInt(MOK) );
	//PostSafeQueue( pNew );

	//pCharInfo->m_EquipedItem.SetItem(parts, pJob->GetItemUID(), pItem);

	//ResponseCharacterItemList( uidPlayer );

	//if( FindStage(pJob->GetStageUID()) )
	//{
	//	// ¸¸¾à ½ºÅ×ÀÌÁö ¾ÈÀÌ¸é ½ºÅ×ÀÌÁö¿¡ ¾Ë·Á¼­ À¯ÀúÀÇ LookÀ» ¾÷µ¥ÀÌÆ® ÇØÁà¾ß ÇÑ´Ù.
	//	if( FindStage(pJob->GetStageUID()) )
	//	{
	//		MCommand* pEquipInfo = CreateCommand( MC_MATCH_ROUTE_UPDATE_STAGE_EQUIP_LOOK, MUID(0, 0) );
	//		pEquipInfo->AddParameter( new MCmdParamUID(uidPlayer) );
	//		pEquipInfo->AddParameter( new MCmdParamInt(pJob->GetParts()) );
	//		pEquipInfo->AddParameter( new MCmdParamInt(0) );
	//		RouteToStage( pJob->GetStageUID(), pEquipInfo );
	//	}
	//}
}


void MBMatchServer::OnTakeoffItem(MAsyncJob* pJobResult)
{
	//MBMatchAsyncDBJob_TakeoffItem* pJob = (MBMatchAsyncDBJob_TakeoffItem*)pJobResult;

	//if( MASYNC_RESULT_SUCCEED != pJob->GetResult() || !pJob->GetRet() ) 
	//{
	//	MCommand* pNew = CreateCommand( MC_MATCH_RESPONSE_TAKEOFF_ITEM, pJob->GetPlayerUID() );
	//	pNew->AddParameter( new MCommandParameterInt(MERR_CANNOT_TAKEOFF_ITEM) );
	//	PostSafeQueue( pNew );

	//	return;
	//}

	//MMatchObject* pObj = GetObject( pJob->GetPlayerUID() );
	//if( NULL == pObj )
	//{
	//	return;
	//}

	//MMatchCharInfo* pCharInfo = pObj->GetCharInfo();
	//if( NULL == pCharInfo )
	//{
	//	return;
	//}

	//const MUID&					uidPlayer	= pJob->GetPlayerUID();
	//const MMatchCharItemParts	parts		= pJob->GetParts();
	//

	//MCommand* pNew = CreateCommand( MC_MATCH_RESPONSE_TAKEOFF_ITEM, uidPlayer );
	//pNew->AddParameter( new MCommandParameterInt(MOK) );
	//PostSafeQueue( pNew );

	//pCharInfo->m_EquipedItem.Remove( pJob->GetParts() );


	//ResponseCharacterItemList( uidPlayer );


	//if( FindStage(pJob->GetStageUID()) )
	//{
	//	// ¸¸¾à ½ºÅ×ÀÌÁö ¾ÈÀÌ¸é ½ºÅ×ÀÌÁö¿¡ ¾Ë·Á¼­ À¯ÀúÀÇ LookÀ» ¾÷µ¥ÀÌÆ® ÇØÁà¾ß ÇÑ´Ù.
	//	if( FindStage(pJob->GetStageUID()) )
	//	{
	//		MCommand* pEquipInfo = CreateCommand( MC_MATCH_ROUTE_UPDATE_STAGE_EQUIP_LOOK, MUID(0, 0) );
	//		pEquipInfo->AddParameter( new MCmdParamUID(uidPlayer) );
	//		pEquipInfo->AddParameter( new MCmdParamInt(pJob->GetParts()) );
	//		pEquipInfo->AddParameter( new MCmdParamInt(0) );
	//		RouteToStage( pJob->GetStageUID(), pEquipInfo );
	//	}
	//}
}