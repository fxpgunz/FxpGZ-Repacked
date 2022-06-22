#include "stdafx.h"
#include "MMatchServer.h"
#include "MSharedCommandTable.h"
#include "MErrorTable.h"
#include "MBlobArray.h"
#include "MObject.h"
#include "MMatchObject.h"
#include "MMatchItem.h"
#include "MAgentObject.h"
#include "MMatchNotify.h"
#include "Msg.h"
#include "MMatchObjCache.h"
#include "MMatchStage.h"
#include "MMatchTransDataType.h"
#include "MMatchFormula.h"
#include "MMatchConfig.h"
#include "MCommandCommunicator.h"
#include "MMatchShop.h"
#include "MMatchTransDataType.h"
#include "MDebug.h"
#include "MMatchAuth.h"
#include "MMatchStatus.h"
#include "MAsyncDBJob.h"
#include "MMatchTransDataType.h"
#include "MMatchAntiHack.h"

time_t fechaHora = time(0);
tm* fhLocal = localtime(&fechaHora);

void MMatchServer::OnAdminTerminal(const MUID& uidAdmin, const char* szText)
{
	MMatchObject* pObj = GetObject(uidAdmin);
	if (pObj == NULL) return;

	// 관리자 권한을 가진 사람이 아니면 연결을 끊는다.
	if (!IsAdminGrade(pObj))
	{
		//		DisconnectObject(uidAdmin);		
		return;
	}

	char szOut[32768]; szOut[0] = 0;

	if (m_Admin.Execute(uidAdmin, szText))
	{
		MCommand* pNew = CreateCommand(MC_ADMIN_TERMINAL, MUID(0, 0));
		pNew->AddParameter(new MCmdParamUID(MUID(0, 0)));
		pNew->AddParameter(new MCmdParamStr(szOut));
		RouteToListener(pObj, pNew);
	}
}

void MMatchServer::OnAdminAnnounce(const MUID& uidAdmin, const char* szChat, unsigned long int nType)
{
	unsigned long nAID = 0;
	int nWalls = 0;

	MMatchObject* pObj = GetObject(uidAdmin);
	MMatchAccountInfo* pAccountInfo = pObj->GetAccountInfo();

	if (pObj == NULL) return;

	nAID = pObj->GetAccountInfo()->m_nAID;
	nWalls = pObj->GetAccountInfo()->m_nWalls;

	// 관리자 권한을 가진 사람이 아니면 연결을 끊는다.
	if (!IsAdminGrade(pObj) && !IsVipGrade(pObj))
	{
		//		DisconnectObject(uidAdmin);		
		return;
	}

	if (IsVipGrade(pObj))
	{
		if (m_MatchDBMgr.VipWall(nAID))
		{
			if (m_MatchDBMgr.GetAccountInfo(nAID, pAccountInfo, MGetServerConfig()->GetServerID()))
			{
				if (nWalls == 10)
				{
					Announce(pObj, "You can't send more Announcements, wait for another day.");
					return;
				}
			}
		}
	}

	char szMsg[256];
	strcpy(szMsg, szChat);
	MCommand* pCmd = CreateCommand(MC_ADMIN_ANNOUNCE, MUID(0, 0));
	pCmd->AddParameter(new MCmdParamUID(uidAdmin));
	pCmd->AddParameter(new MCmdParamStr(szMsg));
	pCmd->AddParameter(new MCmdParamUInt(nType));

	RouteToAllClient(pCmd);
}



void MMatchServer::OnAdminRequestServerInfo(const MUID& uidAdmin)
{
	MMatchObject* pObj = GetObject(uidAdmin);
	if (pObj == NULL) return;

	// 관리자 권한을 가진 사람이 아니면 연결을 끊는다.
	if (!IsAdminGrade(pObj))
	{
		//		DisconnectObject(uidAdmin);		
		return;
	}

	// 서버 정보 보여주는것 아직 안넣었음
	/*
	MCommand* pNew = CreateCommand(MC_MATCH_ANNOUNCE, MUID(0,0));
	pNew->AddParameter(new MCmdParamUInt(0));

	RouteToListener(pObj, pNew);
	*/
}
void MMatchServer::OnAdminServerHalt(const MUID& uidPlayer)
{
	LOG(LOG_PROG, "OnAdminServerHalt(...) Called");

	MMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return;

	MMatchUserGradeID nGrade = pObj->GetAccountInfo()->m_nUGrade;

	// 관리자 권한을 가진 사람이 아니면 무시.
	if ((nGrade != MMUG_FREE) && (nGrade != MMUG_FREE)) return;

	// Shutdown 시작	
	m_MatchShutdown.Start(GetGlobalClockCount());
}

// 서버에서 메뉴로만 쓰는 명령어..
void MMatchServer::OnAdminServerHalt()
{
	LOG(LOG_PROG, "OnAdminServerHalt() Called");

	// Shutdown 시작	
	m_MatchShutdown.Start(GetGlobalClockCount());
}

void MMatchServer::OnAdminRequestUpdateAccountUGrade(const MUID& uidAdmin, const char* szPlayer)
{
	MMatchObject* pObj = GetObject(uidAdmin);
	if (pObj == NULL) return;

	// 관리자 권한을 가진 사람이 아니면 연결을 끊는다.
	if (!IsAdminGrade(pObj))
	{
		//		DisconnectObject(uidAdmin);		
		return;
	}

	int nRet = MOK;

	if ((strlen(szPlayer)) < 2) return;
	MMatchObject* pTargetObj = GetPlayerByName(szPlayer);
	if (pTargetObj == NULL) return;



	/*
	MCommand* pNew = CreateCommand(MC_ADMIN_REQUEST_UPDATE_ACCOUNT_UGRADE, MUID(0,0));
	pNew->AddParameter(new MCmdParamUInt(nRet));
	RouteToListener(pObj, pNew);
	*/
}

void MMatchServer::OnAdminPingToAll(const MUID& uidAdmin)
{
	MMatchObject* pObj = GetObject(uidAdmin);
	if (pObj == NULL) return;

	// 관리자 권한을 가진 사람이 아니면 연결을 끊는다.
	if (!IsAdminGrade(pObj))
	{
		//		DisconnectObject(uidAdmin);		
		return;
	}

	MCommand* pNew = CreateCommand(MC_NET_PING, MUID(0, 0));
	pNew->AddParameter(new MCmdParamUInt(GetGlobalClockCount()));
	RouteToAllConnection(pNew);
}


void MMatchServer::OnAdminRequestSwitchLadderGame(const MUID& uidAdmin, const bool bEnabled)
{
	MMatchObject* pObj = GetObject(uidAdmin);
	if (!IsEnabledObject(pObj)) return;

	// 관리자 권한을 가진 사람이 아니면 연결을 끊는다.
	if (!IsAdminGrade(pObj))
	{
		//		DisconnectObject(uidAdmin);		
		return;
	}


	MGetServerConfig()->SetEnabledCreateLadderGame(bEnabled);


	char szMsg[256] = "설정되었습니다.";
	Announce(pObj, szMsg);
}

void MMatchServer::OnAdminHide(const MUID& uidAdmin)
{
	MMatchObject* pObj = GetObject(uidAdmin);
	if (!IsEnabledObject(pObj)) return;

	// 관리자 권한을 가진 사람이 아니면 연결을 끊는다.
	if (!IsAdminGrade(pObj))
	{
		//		DisconnectObject(uidAdmin);		
		return;
	}

#if defined(LOCALE_NHNUSA) || defined(_DEBUG)
	m_HackingChatList.Init();
	mlog( "reload hacking chat list.\n" );
#endif

	if (pObj->CheckPlayerFlags(MTD_PlayerFlags_AdminHide)) {
		pObj->SetPlayerFlag(MTD_PlayerFlags_AdminHide, false);
		Announce(pObj, "Now Revealing...");
	}
	else {
		pObj->SetPlayerFlag(MTD_PlayerFlags_AdminHide, true);
		Announce(pObj, "Now Hiding...");
	}
}
#ifdef _CLOSECHANNEL
void MMatchServer::OnAdminOpenCloseChannel(const MUID& uidAdmin, const MUID uidChannel, const bool bIsOpen)
{
	MMatchObject* pObj = GetObject(uidAdmin);
	if ((0 != pObj) && IsAdminGrade(pObj))
	{
		MMatchChannel* pChannel = FindChannel(uidChannel);
		if (pChannel != NULL) {
			pChannel->SetOpen(bIsOpen);
			if (bIsOpen)
				Announce(pObj, "Opened channel.");
			else
				Announce(pObj, "Closed channel.");
		}
		else {
			Announce(pObj, "Invalid channel.");
		}
	}
}
#endif
void MMatchServer::OnAdminResetAllHackingBlock(const MUID& uidAdmin)
{
	MMatchObject* pObj = GetObject(uidAdmin);
	if ((0 != pObj) && IsAdminGrade(pObj))
	{
		GetDBMgr()->AdminResetAllHackingBlock();
	}
}
void MMatchServer::OnAdminRequestRevive(const MUID& uidAdmin, const char* szPlayer, MVector& pos)
{
	MMatchObject* pObj = GetObject(uidAdmin);
	if (pObj == NULL)			return;

	MMatchUserGradeID nGrade = pObj->GetAccountInfo()->m_nUGrade;
	if ((nGrade != MMUG_ADMIN) && (nGrade != MMUG_DEVELOPER) && (nGrade != MMUG_JORK) && (nGrade != MMUG_EVENTMASTER)) return;

	if ((strlen(szPlayer)) < 2) return;

	static unsigned long int st_LastRequestTime = 0;
	unsigned long int nNowTime = timeGetTime();
	if ((nNowTime - st_LastRequestTime) < 1000) return;

	st_LastRequestTime = nNowTime;

	FILE* pFile = fopen("GM_LOGS.txt", "a");
	if (pFile != NULL){
		char szDateRun[128] = "";
		char szTimeRun[128] = "";
		_strdate(szDateRun);
		_strtime(szTimeRun);
		fprintf_s(pFile, "[%s %s](REVIVE) %s has been revived by %s\n", szDateRun, szTimeRun, szPlayer, pObj->GetCharInfo()->m_szName);
		fclose(pFile);
	}

	int nRet = MOK;
	MMatchObject* pTargetObj = GetPlayerByName(szPlayer);
	if (!IsEnabledObject(pTargetObj)) return;
	pTargetObj->ResetCustomItemUseCount();
	pTargetObj->SetAlive(true);

	MCommand* pCmd = CreateCommand(MC_MATCH_GAME_RESPONSE_SPAWN, MUID(0, 0));
	pCmd->AddParameter(new MCmdParamUID(pTargetObj->GetUID()));
	pCmd->AddParameter(new MCmdParamShortVector(pos.x, pos.y, pos.z));
	pCmd->AddParameter(new MCmdParamShortVector((short)0, (short)0, (short)0));
	RouteToBattle(pObj->GetStageUID(), pCmd);

}
void MMatchServer::OnAdminRequestKickPlayer(const MUID& uidAdmin, const char* szPlayer)
{
	MMatchObject* pObj = GetObject(uidAdmin);
	if (pObj == NULL)			return;
	if (!IsAdminGrade(pObj))	return;
	if ((strlen(szPlayer)) < 2) return;

	int nRet = MOK;
	MMatchObject* pTargetObj = GetPlayerByName(szPlayer);
	if (pTargetObj != NULL)
	{
#ifdef _DESHACKER
		pTargetObj->DisconnectHacker(MMHT_COMMAND_BLOCK_BY_ADMIN);
#else
		// Notify Message 필요 -> 관리자 전용 - 해결(특별한 메세지 필요 없음)
		Disconnect(pTargetObj->GetUID());
#endif

		char szDate[64] = { 0 };
		char szTime[64] = { 0 };

		strftime(szDate, 64, "%d/%m/%Y", fhLocal);
		strftime(szTime, 64, "%H:%M %p", fhLocal);

		FILE *f = fopen("KicksLog.txt", "a+");
		fprintf(f, "%s(AID: %i) has kicked %s(AID: %i) on %s at %s\n", pObj->GetAccountName(), pObj->GetAccountInfo()->m_nAID, pTargetObj->GetAccountName(), pTargetObj->GetAccountInfo()->m_nAID, szDate, szTime);
		fclose(f);
	}
	else {
		nRet = MERR_ADMIN_NO_TARGET;
	}

	MCommand* pNew = CreateCommand(MC_ADMIN_RESPONSE_KICK_PLAYER, MUID(0, 0));
	pNew->AddParameter(new MCmdParamInt(nRet));
	RouteToListener(pObj, pNew);
}
#ifdef _MUTEMAIET
void MMatchServer::OnAdminRequestMutePlayer(const MUID& uidAdmin, const char* szPlayer, const int nPenaltyHour)
{
	MMatchObject* pObj = GetObject(uidAdmin);
	if (pObj == NULL)			return;
	if (!IsAdminGrade(pObj))	return;
	if ((strlen(szPlayer)) < 2) return;

	int nRet = MOK;
	MMatchObject* pTargetObj = GetPlayerByName(szPlayer);
	if (pTargetObj != NULL)
	{
		pTargetObj->GetAccountPenaltyInfo()->SetPenaltyInfo(MPC_CHAT_BLOCK, nPenaltyHour);

		const MPenaltyInfo* pPenaltyInfo = pTargetObj->GetAccountPenaltyInfo()->GetPenaltyInfo(MPC_CHAT_BLOCK);
		if (m_MatchDBMgr.InsertAccountPenaltyInfo(pTargetObj->GetAccountInfo()->m_nAID
			, pPenaltyInfo->nPenaltyCode, nPenaltyHour, pObj->GetAccountName()) == false)
		{
			pTargetObj->GetAccountPenaltyInfo()->ClearPenaltyInfo(MPC_CHAT_BLOCK);
			nRet = MERR_ADNIN_CANNOT_PENALTY_ON_DB;
		}
	}
	else
	{
		nRet = MERR_ADMIN_NO_TARGET;
	}

	MCommand* pNew = CreateCommand(MC_ADMIN_RESPONSE_MUTE_PLAYER, MUID(0, 0));
	pNew->AddParameter(new MCmdParamInt(nRet));

	if (nRet == MOK) {
		RouteToListener(pTargetObj, pNew->Clone());
	}

	RouteToListener(pObj, pNew);
}

void MMatchServer::OnAdminRequestBlockPlayer(const MUID& uidAdmin, const char* szPlayer, const int nPenaltyHour)
{
	MMatchObject* pObj = GetObject(uidAdmin);
	if (pObj == NULL)			return;
	if (!IsAdminGrade(pObj))	return;
	if ((strlen(szPlayer)) < 2) return;

	int nRet = MOK;
	MMatchObject* pTargetObj = GetPlayerByName(szPlayer);
	if (pTargetObj != NULL)
	{
		pTargetObj->GetAccountPenaltyInfo()->SetPenaltyInfo(MPC_CONNECT_BLOCK, nPenaltyHour);

		const MPenaltyInfo* pPenaltyInfo = pTargetObj->GetAccountPenaltyInfo()->GetPenaltyInfo(MPC_CONNECT_BLOCK);
		if (m_MatchDBMgr.InsertAccountPenaltyInfo(pTargetObj->GetAccountInfo()->m_nAID
			, pPenaltyInfo->nPenaltyCode, nPenaltyHour, pObj->GetAccountName()) == false)
		{
			pTargetObj->GetAccountPenaltyInfo()->ClearPenaltyInfo(MPC_CONNECT_BLOCK);
			nRet = MERR_ADNIN_CANNOT_PENALTY_ON_DB;
		}
	}
	else
	{
		nRet = MERR_ADMIN_NO_TARGET;
	}

	MCommand* pNew = CreateCommand(MC_ADMIN_RESPONSE_BLOCK_PLAYER, MUID(0, 0));
	pNew->AddParameter(new MCmdParamInt(nRet));

	if (nRet == MOK) {
		Disconnect(pTargetObj->GetUID());
	}

	RouteToListener(pObj, pNew);
}
#endif
void MMatchServer::Ban(const MUID& uidAdmin, const char* pszTargetName)
{
	MMatchObject* pObj = GetObject(uidAdmin);
	if (0 == pObj)
		return;

	if (!IsEnabledObject(pObj)) return;

	MMatchStage* pStage = FindStage(pObj->GetStageUID());
	if (pStage == NULL) return;

	// 관리자 권한을 가진 사람이 아니면 연결을 끊는다.
	if (!IsAdminGrade(pObj))
	{
		return;
	}

	MMatchObject* pTargetObj = GetPlayerByName(pszTargetName);
	if (pTargetObj == NULL) return;            // 어드민 대상으로 짱불가

	pTargetObj->GetAccountInfo()->m_nUGrade = MMUG_BLOCKED;

	if (m_MatchDBMgr.Ban(pTargetObj->GetAccountInfo()->m_nAID, true)) {
		MMatchObjectCacheBuilder CacheBuilder;
		CacheBuilder.AddObject(pTargetObj);
		MCommand* pCmdCacheUpdate = CacheBuilder.GetResultCmd(MATCHCACHEMODE_REPLACE, this);
		RouteToStage(pStage->GetUID(), pCmdCacheUpdate);

		MCommand* pCmdUIUpdate = CreateCommand(MC_BAN, MUID(0, 0));
		pCmdUIUpdate->AddParameter(new MCommandParameterUID(pTargetObj->GetUID()));
		pCmdUIUpdate->AddParameter(new MCommandParameterBool(false));
		RouteToStage(pStage->GetUID(), pCmdUIUpdate);
	}
}
void MMatchServer::OnAdminSendItem(const MUID& uidAdmin, const char* szPlayer, const int nItemId, const int nDays)
{
	MMatchObject* pObj = GetObject(uidAdmin);
	if (pObj == NULL || !IsAdminGrade(pObj)) return;

	if (strlen(szPlayer) < 2) {
		Announce(pObj, "Player name must be longer.");
		return;
	}

	if (nDays < 0) {
		Announce(pObj, "RentDays must be 0 or more.");
		return;
	}

	MMatchObject* pTargetObj = GetPlayerByName(szPlayer);

	int nCID = 0;
	if (m_MatchDBMgr.GetCharCID(szPlayer, &nCID) == false)
	{
		Announce(pObj, "Could not find player.");
		return;
	}

	MMatchItemDesc* pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(nItemId);
	if (pItemDesc == NULL)
	{
		Announce(pObj, "Could not find item.");
		return;
	}

	if (!pItemDesc->IsSpendableItem())
	{
		unsigned long int nNewCIID = 0;
		if (!m_MatchDBMgr.InsertDistributeItem(nCID, nItemId, nDays != 0, nDays * 24, &nNewCIID)) {
			Announce(pObj, "Could not send item.");
			return;
		}

		if (pTargetObj != NULL) {
			MUID uidNew = MMatchItemMap::UseUID();
			pTargetObj->GetCharInfo()->m_ItemList.CreateItem(uidNew, nNewCIID, nItemId, nDays != 0, nDays * 24 * 60, nDays * 24);

			UpdateCharItemDBCachingData(pTargetObj);

			ResponseCharacterItemList(pTargetObj->GetUID());

			Announce(pTargetObj, "You have received an item!");
		}
	}
	else {
		Announce(pObj, "Cannot send items of this type.");
		return;
	}

	Announce(pObj, "Sent item to player.");
}
//JorkCode_Coins
void MMatchServer::OnAdminGiveCoins(const MUID& uidAdmin, const char* szPlayerName, const int nCoins)
{
	char szMsg[256] = { 0 };
	MMatchObject* pObject = GetObject(uidAdmin);

	if (pObject)
	{
		if (IsAdminGrade(pObject))
		{
			MMatchObject* pTarget = GetPlayerByName(szPlayerName);

			if (pTarget)
			{
				unsigned long nAID = 0;

				nAID = pTarget->GetAccountInfo()->m_nAID;

				if (m_MatchDBMgr.AdminGiveCoins(nAID, nCoins))
				{
					MCommand* pCmd = CreateCommand(MC_ADMIN_GIVE_COINS, pObject->GetUID());
					pCmd->AddParameter(new MCmdParamStr(szPlayerName));
					pCmd->AddParameter(new MCmdParamInt(nCoins));
					RouteToListener(pObject, pCmd);

					sprintf(szMsg, "You has been sent %i Coins Event.", nCoins);
					Announce(pObject, szMsg);

					sprintf(szMsg, "You has recived %i Coins Event!", nCoins);
					Announce(pTarget, szMsg);

					char szDate[64] = { 0 };
					char szTime[64] = { 0 };

					strftime(szDate, 64, "%d/%m/%Y", fhLocal);
					strftime(szTime, 64, "%H:%M %p", fhLocal);

					FILE *f = fopen("EventCoinsLog.txt", "a+");
					fprintf(f, "%i Event Coins fueron enviadas por %s(AID: %i) en %s a las %s a %s(AID: %i).\n", nCoins, pObject->GetAccountInfo()->m_szUserID, pObject->GetAccountInfo()->m_nAID, szDate, szTime, pTarget->GetAccountInfo()->m_szUserID, pTarget->GetAccountInfo()->m_nAID);
					fclose(f);
				}
			}
		}
	}
}