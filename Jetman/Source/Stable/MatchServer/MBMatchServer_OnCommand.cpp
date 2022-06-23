#include "stdafx.h"
#include "MBMatchServer.h"
#include "MatchServerDoc.h"
#include "OutputView.h"
#include <atltime.h>
#include "MatchServer.h"
#include "MMap.h"
#include "MErrorTable.h"
#include "CommandLogView.h"
#include "MDebug.h"
#include "MMatchRule.h"
#include "MMatchRuleAssassinate.h"
#include "MBMatchAuth.h"
#include "MDebug.h"
#include "MMatchStatus.h"
#include "MMatchSchedule.h"
#include "MSharedCommandTable.h"
#include "MMatchConfig.h"
#include "MBlobArray.h"
#include "MUtil.h"
#include "RTypes.h"
#include "MTypes.h"
#include "MCommandParameter.h"
#include "MMatchUtil.h"
#include "MMatchShop.h"

#ifdef LOCALE_NHNUSA
#include "MBMatchNHNAuth.h"
#endif


bool MBMatchServer::OnCommand(MCommand* pCommand)
{
	// �ּ��� �ϳ��� Ŀ�ǵ尡 ���������� �˱����� ���ú����� ����
	int nCommandID = pCommand->GetID();

	CheckMemoryCorruption();

	if( MMatchServer::OnCommand(pCommand) )
	{
		CheckMemoryCorruption();
		return true;
	}

	switch( pCommand->GetID() )
	{	
	case MC_MATCH_REQUEST_EQUIP_ITEM:
		{
			MUID uidPlayer, uidItem;
			uidPlayer = pCommand->GetSenderUID();
			unsigned long int nEquipmentSlot = 0;

			//pCommand->GetParameter(&uidPlayer, 0, MPT_UID);
			pCommand->GetParameter(&uidItem, 1, MPT_UID);
			pCommand->GetParameter(&nEquipmentSlot, 2, MPT_UINT);

			OnRequestEquipItem(uidPlayer, uidItem, nEquipmentSlot);
		}
		break;

	case MC_MATCH_REQUEST_TAKEOFF_ITEM:
		{
			MUID uidPlayer = pCommand->GetSenderUID();
			unsigned long int nEquipmentSlot = 0;
			//pCommand->GetParameter(&uidPlayer, 0, MPT_UID);
			pCommand->GetParameter(&nEquipmentSlot, 1, MPT_UINT);

			OnRequestTakeoffItem(uidPlayer, nEquipmentSlot);
		}
		break;

	case MC_MATCH_LOGIN_NETMARBLE:
		{
#ifdef LOCALE_KOREA
			char szAuthCookie[4096];
			char szDataCookie[4096];
			char szCPCookie[4096];
			char szSpareParam[4096];
			int nCommandVersion = 0;
			unsigned long nChecksumPack = 0;
			if (pCommand->GetParameter(szAuthCookie, 0, MPT_STR, sizeof(szAuthCookie) )==false) break;
			if (pCommand->GetParameter(szDataCookie, 1, MPT_STR, sizeof(szDataCookie) )==false) break;
			if (pCommand->GetParameter(szCPCookie, 2, MPT_STR, sizeof(szCPCookie) )==false) break;
			if (pCommand->GetParameter(szSpareParam, 3, MPT_STR, sizeof(szSpareParam) )==false) break;
			if (pCommand->GetParameter(&nCommandVersion, 4, MPT_INT)==false) break;
			if (pCommand->GetParameter(&nChecksumPack, 5, MPT_UINT)==false) break;

			//todok mark
			//OnRequestLoginNetmarble(pCommand->GetSenderUID(), szAuthCookie, szDataCookie, szCPCookie, szSpareParam, nCommandVersion, nChecksumPack);
			//OnRequestLoginNetmarble(pCommand->GetSenderUID(), szCPCookie, szSpareParam, nCommandVersion, nChecksumPack);
#endif
		}
		break;

	case MC_MATCH_LOGIN_NHNUSA :  
		{
#ifdef LOCALE_NHNUSA
			char szUserID[ MAX_USERID_STRING_LEN ]	= {0,};
			char szAuthStr[ NHN_AUTH_LENGTH ]		= {0,};
			int  nCommandVersion = 0;
			int  nCheckSumPack = 0;

			pCommand->GetParameter( szUserID, 0, MPT_STR, MAX_USERID_STRING_LEN );
			pCommand->GetParameter( szAuthStr, 1, MPT_STR, NHN_AUTH_LENGTH );
			pCommand->GetParameter( &nCommandVersion, 2, MPT_INT );
			pCommand->GetParameter( &nCheckSumPack, 3, MPT_UINT );
			MCommandParameter* pLoginParam = pCommand->GetParameter(4);
			if (pLoginParam->GetType() != MPT_BLOB) break;
			void *pLoginBlob = pLoginParam->GetPointer();
			if( NULL == pLoginBlob )
				break;

			char *szEncryptMD5Value = (char *)MGetBlobArrayElement(pLoginBlob, 0);

			OnRequestLoginNHNUSA( pCommand->GetSenderUID(), szUserID, szAuthStr, nCommandVersion, nCheckSumPack, szEncryptMD5Value);
#endif
		}
		break;

		case MC_MATCH_LOGIN_GAMEON_JP :  
		{
#ifdef LOCALE_JAPAN
			char szString[ 512 ] = {0,};
			char szStatIndex[ 512 ] = {0,};
			int  nCommandVersion = 0;
			int  nCheckSumPack = 0;

			pCommand->GetParameter( szString, 0, MPT_STR, 512 );
			pCommand->GetParameter( szStatIndex, 1, MPT_STR, 512 );
			pCommand->GetParameter( &nCommandVersion, 2, MPT_INT );
			pCommand->GetParameter( &nCheckSumPack, 3, MPT_UINT );
			MCommandParameter* pLoginParam = pCommand->GetParameter(4);
			if (pLoginParam->GetType() != MPT_BLOB) break;
			void *pLoginBlob = pLoginParam->GetPointer();
			if( NULL == pLoginBlob )
				break;

			char *szEncryptMD5Value = (char *)MGetBlobArrayElement(pLoginBlob, 0);

			OnRequestLoginGameOn( pCommand->GetSenderUID(), szString, szStatIndex, nCommandVersion, nCheckSumPack, szEncryptMD5Value);
#endif
		}
		break;

	case MC_MATCH_SCHEDULE_ANNOUNCE_MAKE :
		{
			// ���� �����췯�� ��ϵ� ������ Ŭ���̾�Ʈ�� ����.

			char szAnnounce[ ANNOUNCE_STRING_LEN ];

			if( MUID(0, 0) != pCommand->GetSenderUID() )
				break;

			pCommand->GetParameter( szAnnounce, 0, MPT_STR, ANNOUNCE_STRING_LEN );

			OnScheduleAnnounce( szAnnounce );
		}
		break;

	case MC_MATCH_SCHEDULE_CLAN_SERVER_SWITCH_DOWN : 
		{
			// Ŭ�� ������ ��� Ŭ������ �Ҽ� ���� �����մϴ�.

			// bool bClanEnabled;
			// pCommand->GetParameter( &bClanEnabled, 0, MPT_BOOL );

			OnScheduleClanServerSwitchDown();		
		}
		break;

	case MC_MATCH_SCHEDULE_CLAN_SERVER_SWITCH_ON :
		{
			OnScheduleClanServerSwitchUp();
		}
		break;

	case MC_REQUEST_KEEPER_CONNECT_MATCHSERVER :
		{
			OnRequestConnectMatchServer( pCommand->GetSenderUID() );
		}
		break;

	case MC_REQUEST_MATCHSERVER_STATUS :
		{
			OnResponseServerStatus( pCommand->GetSenderUID() );
		}
		break;

	case MC_REQUEST_SERVER_HEARBEAT :
		{
			OnRequestServerHearbeat( pCommand->GetSenderUID() );
		}
		break;

	case MC_REQUEST_KEEPER_ANNOUNCE :
		{
			char szAnnounce[ ANNOUNCE_STRING_LEN ];

			pCommand->GetParameter( szAnnounce, 0, MPT_STR, ANNOUNCE_STRING_LEN );

			OnRequestKeeperAnnounce( pCommand->GetSenderUID(), szAnnounce );
		}
		break;

	case MC_REQUEST_ANNOUNCE_STOP_SERVER :
		{
			OnRequestStopServerWithAnnounce( pCommand->GetSenderUID() );
		}
		break;

	case MC_REQUEST_KEEPER_MANAGER_SCHEDULE :
		{
			int nType;
			int nYear;
			int nMonth;
			int nDay;
			int nHour;
			int nMin;
			int nCount;
			int nCommand;
			char szAnnounce[ ANNOUNCE_STRING_LEN ] = {0,};

			pCommand->GetParameter( &nType, 0, MPT_INT );
			pCommand->GetParameter( &nYear, 1, MPT_INT );
			pCommand->GetParameter( &nMonth, 2, MPT_INT );
			pCommand->GetParameter( &nDay, 3, MPT_INT );
			pCommand->GetParameter( &nHour, 4, MPT_INT );
			pCommand->GetParameter( &nMin, 5, MPT_INT );
			pCommand->GetParameter( &nCount, 6, MPT_INT );
			pCommand->GetParameter( &nCommand, 7, MPT_INT );
			pCommand->GetParameter( szAnnounce, 8, MPT_STR, ANNOUNCE_STRING_LEN );

			OnRequestSchedule( pCommand->GetSenderUID(), 
				nType, 
				nYear, 
				nMonth, 
				nDay, 
				nHour, 
				nMin, 
				nCount, 
				nCommand, 
				szAnnounce );
		}
		break;

	case MC_MATCH_SCHEDULE_STOP_SERVER :
		{
			char szAnnounce[ ANNOUNCE_STRING_LEN ] = { 0, };

			pCommand->GetParameter( szAnnounce, 0, MPT_STR, ANNOUNCE_STRING_LEN );

			OnRequestKeeperStopServerSchedule( pCommand->GetSenderUID(), szAnnounce );
		}
		break;

	case MC_REQUEST_RELOAD_CONFIG :
		{
			char szFileList[ 1024 ] = {0,};

			pCommand->GetParameter( szFileList, 0, MPT_STR, 1024 );

			OnRequestReloadServerConfig( pCommand->GetSenderUID(), szFileList );
		}
		break;

	case MC_REQUEST_ADD_HASHMAP :
		{
	//		char szNewHashValue[ 128 ] = {0,};
	//		pCommand->GetParameter( szNewHashValue, 0, MPT_STR, 128 );
	//		OnRequestAddHashMap( pCommand->GetSenderUID(), szNewHashValue );
		}
		break;
	
	case MC_HSHIELD_PING:
		{
		}
		break;

	case MC_HSHIELD_PONG:
		{
			unsigned int nTimeStamp;
			pCommand->GetParameter(&nTimeStamp, 0, MPT_UINT);
			MCommandParameter* pParam = pCommand->GetParameter(1);
			if(pParam->GetType() != MPT_BLOB) 
				break;

			void* pBlob = pParam->GetPointer();
			if( NULL == pBlob )
				break;

			int nCount = MGetBlobArrayCount(pBlob);
			unsigned char* pbyAckMsg = (unsigned char*)MGetBlobArrayElement(pBlob, 0);

			OnHShieldPong(pCommand->GetSenderUID(), nTimeStamp);

			MMatchObject* pObj = GetObject(pCommand->GetSenderUID());
			if (pObj == NULL) return false;

			DWORD dwRet = HShield_AnalyzeAckMsg(pObj->GetHShieldInfo()->m_pCRCInfo, pbyAckMsg, pObj->GetHShieldInfo()->m_pbyReqInfo);

			if(dwRet != ERROR_SUCCESS)
			{
				++pObj->m_dwHShieldCheckCount;

				LOG(LOG_FILE, "@AnalyzeAckMsg - Find Hacker(%s) (Error code = %x) (Ack Msg = %s) (Check count = %u) (CRC Address = %x)", 
					pObj->GetAccountInfo()->m_szUserID, dwRet, pbyAckMsg,
					pObj->m_dwHShieldCheckCount, pObj->GetHShieldInfo()->m_pCRCInfo );

				MCommand* pNewCmd = CreateCommand(MC_MATCH_FIND_HACKING, MUID(0,0));
				RouteToListener(pObj, pNewCmd);

#ifdef _HSHIELD
				if( MGetServerConfig()->IsUseHShield() )
				{
// #ifndef _DEBUG
					// �������� �����̹Ƿ� ĳ���� �����Ҷ� ������ ���´�.
					pObj->SetHacker(true);
//					pObj->SetHShieldHackerDisconnectWaitInfo();
					pObj->DisconnectHacker( MMHT_HSHIELD_HACKER );

					LOG(LOG_FILE, "Hacker(%s) disconnect wait", pObj->GetAccountInfo()->m_szUserID);
// #endif
				}
#endif
			}

			LOG(LOG_DEBUG, "Ping from (%u:%u) = %d", 
				pCommand->GetSenderUID().High, pCommand->GetSenderUID().Low, timeGetTime()-nTimeStamp);
		}
		break;

	//case MC_RESPONSE_GAMEGUARD_AUTH :
	//	{
	//		DWORD dwIndex;
	//		DWORD dwValue1;
	//		DWORD dwValue2;
	//		DWORD dwValue3;

	//		pCommand->GetParameter( &dwIndex, 0, MPT_UINT );
	//		pCommand->GetParameter( &dwValue1, 1, MPT_UINT );
	//		pCommand->GetParameter( &dwValue2, 2, MPT_UINT );
	//		pCommand->GetParameter( &dwValue3, 3, MPT_UINT );

	//		OnResponseGameguardAuth( pCommand->GetSenderUID(), dwIndex, dwValue1, dwValue2, dwValue3 );
	//	}
	//	break;

	//case MC_RESPONSE_FIRST_GAMEGUARD_AUTH :
	//	{
	//		DWORD dwIndex;
	//		DWORD dwValue1;
	//		DWORD dwValue2;
	//		DWORD dwValue3;

	//		pCommand->GetParameter( &dwIndex, 0, MPT_UINT );
	//		pCommand->GetParameter( &dwValue1, 1, MPT_UINT );
	//		pCommand->GetParameter( &dwValue2, 2, MPT_UINT );
	//		pCommand->GetParameter( &dwValue3, 3, MPT_UINT );

	//		mlog( "Recv fist gg auth.\n" );

	//		OnResponseFirstGameguardAuth( pCommand->GetSenderUID(), dwIndex, dwValue1, dwValue2, dwValue3 );
	//	}
	//	break;

	//case MC_RESPONSE_XTRAP_SEEDKEY:
	//	{
	//		MCommandParameter *pParam = pCommand->GetParameter(0);
	//		if(pParam->GetType() != MPT_BLOB) 
	//		{
	//			break;
	//		}
	//		void* pBlob = pParam->GetPointer();
	//		if( NULL == pBlob )
	//			break;

	//		unsigned char *pComBuf = (unsigned char *)MGetBlobArrayElement(pBlob, 0);

	//		OnResponseXTrapSeedKey(pCommand->GetSenderUID(), pComBuf);
	//	}
	//	break;

	case MC_ADMIN_RELOAD_CLIENT_HASH:
		{
			XTrap_OnAdminReloadFileHash(pCommand->GetSenderUID());
		}
		break;

	case MC_MATCH_REQUEST_ACCOUNT_CHARLIST:
		{
			MUID uidPlayer = pCommand->GetSenderUID();
			MCommandParameter* pParam = pCommand->GetParameter(0);
			if (pParam->GetType() != MPT_BLOB)
			{
				break;
			}
			void* pBlob = pParam->GetPointer();
			if( NULL == pBlob )
				break;

			int nCount = MGetBlobArrayCount(pBlob);
			unsigned char* pbyGuidAckMsg = (unsigned char*)MGetBlobArrayElement(pBlob, 0);
			OnRequestAccountCharList(uidPlayer, pbyGuidAckMsg);
		}
		break;
	case MC_MATCH_REQUEST_SHOP_ITEMLIST:
		{
			MUID uidPlayer = pCommand->GetSenderUID();
			int nFirstItemIndex = 0, nItemCount = 0;

			pCommand->GetParameter(&nFirstItemIndex, 1, MPT_INT);
			pCommand->GetParameter(&nItemCount, 2, MPT_INT);

			OnRequestShopItemList(uidPlayer, nFirstItemIndex, nItemCount);
		}
		break;
	case MC_MATCH_REQUEST_GAMBLE:
		{
			MUID uidPlayer = pCommand->GetSenderUID();
			MUID uidItem;
			pCommand->GetParameter(&uidItem, 0, MPT_UID);

			OnRequestGamble(uidPlayer, uidItem);
		}
		break;

	case MC_MATCH_REQUEST_BUY_ITEM:
		{
			MUID uidPlayer = pCommand->GetSenderUID();
			unsigned long int nItemID;
			int	nItemCount;

			pCommand->GetParameter(&nItemID, 1, MPT_UINT);
			pCommand->GetParameter(&nItemCount, 2, MPT_UINT);

			OnRequestBuyItem(uidPlayer, nItemID, nItemCount);
		}
		break;

	case MC_MATCH_REQUEST_CHARACTER_ITEMLIST:
		{
			MUID uidPlayer = pCommand->GetSenderUID();

			OnRequestCharacterItemList(uidPlayer);
		}
		break;

	case MC_MATCH_REQUEST_CHARACTER_ITEMLIST_FORCE:
		{
			MUID uidPlayer = pCommand->GetSenderUID();

			OnRequestCharacterItemListForce(uidPlayer);
		}
		break;

	case MC_MATCH_REQUEST_SELL_ITEM:
		{
			MUID uidPlayer, uidItem;
			int nItemCount = 0;
			uidPlayer = pCommand->GetSenderUID();			

			//pCommand->GetParameter(&uidPlayer, 0, MPT_UID);
			pCommand->GetParameter(&uidItem, 1, MPT_UID);
			pCommand->GetParameter(&nItemCount, 2, MPT_UINT);

			OnRequestSellItem(uidPlayer, uidItem, nItemCount);
		}
		break;

	case MC_MATCH_REQUEST_BRING_ACCOUNTITEM:
		{
			unsigned int nAIID, nItemID, nItemCnt;
			MUID uidPlayer = pCommand->GetSenderUID();

			pCommand->GetParameter(&nAIID, 1, MPT_INT);
			pCommand->GetParameter(&nItemID, 2, MPT_INT);
			pCommand->GetParameter(&nItemCnt, 3, MPT_INT);

			OnRequestBringAccountItem(uidPlayer, nAIID, nItemID, nItemCnt);
		}
		break;

	case MC_MATCH_REQUEST_BRING_BACK_ACCOUNTITEM:
		{
			int nItemCnt;
			MUID uidPlayer, uidItem;

			uidPlayer = pCommand->GetSenderUID();

			pCommand->GetParameter(&uidItem,  1, MPT_UID);
			pCommand->GetParameter(&nItemCnt, 2, MPT_UINT);

			OnRequestBringBackAccountItem(uidPlayer, uidItem, nItemCnt);
		}
		break;

	case MC_ADMIN_RELOAD_GAMBLEITEM :
		{
			MMatchObject* pObj = GetObject( pCommand->GetSenderUID() );
			if( NULL == pObj )
				break;

			if( IsAdminGrade(pObj) )
				InitGambleMachine();
			break;
		}

	case MC_ADMIN_DUMP_GAMBLEITEM_LOG :
		{
			MMatchObject* pObj = GetObject( pCommand->GetSenderUID() );
			if( NULL == pObj )
				break;

			if( IsAdminGrade(pObj) )
			{
				GetGambleMachine().WriteGambleItemInfoToLog();
			}
		}
		break;
	
	case MC_ADMIN_ASSASIN :
		{
			MMatchObject* pObj = GetObject( pCommand->GetSenderUID() );
			if( NULL == pObj )
				break;

			if( IsAdminGrade(pObj) )
			{
				MMatchStage* pStage = FindStage(pObj->GetStageUID());
				if (pStage == NULL)			break;
				if (pStage->GetStageSetting()->GetGameType() != MMATCH_GAMETYPE_ASSASSINATE ) break;

				MMatchRuleAssassinate* pRule = (MMatchRuleAssassinate*)pStage->GetRule();
				pRule->ChooseAdminAsCommander();
			}
		}
		break;

	case MC_MATCH_GAME_KILL:
		{
			MUID uidAttacker, uidVictim;
			pCommand->GetParameter(&uidAttacker, 0, MPT_UID);
			uidVictim = pCommand->GetSenderUID();

			OnGameKill(uidAttacker, uidVictim);
		}
		break;

	case MC_MATCH_REQUEST_STAGE_JOIN:
		{
			MUID uidPlayer, uidStage;

			uidPlayer = pCommand->GetSenderUID();

			//pCommand->GetParameter(&uidPlayer, 0, MPT_UID);
			pCommand->GetParameter(&uidStage, 1, MPT_UID);

			OnStageJoin(uidPlayer, uidStage);
		}
		break;

	case MC_REQUEST_GIVE_ONESELF_UP :
		{
			MMatchObject* pObj = GetObject( pCommand->GetSenderUID() );
			if( NULL == pObj )
			{
				break;
			}

//			pObj->SetDllInjectionDisconnectWaitInfo();
			pObj->DisconnectHacker( MMHT_GIVE_ONESELF_UP_DLLINJECTION);
		}
		break;

	default :
		{
			// ���ǵ��� ���� Ŀ�ǵ�.
		}
		CheckMemoryCorruption();
		return false;
	}

	CheckMemoryCorruption();
	return true;
}


bool MBMatchServer::IsSkeepByGameguardFirstAuth( MCommand* pCommand )
{
	/**
	by SungE 2007-09-14
	-- 1. Ŀ�ǵ带 ���� ������ �����ϴ��� �˻��Ѵ�.
	-- 2. �������� FirstGameguardCheck�� ��û�ߴ��� Ȯ��.
	-- 3. FirstGameguardCheck������ �Դ��� Ȯ���Ѵ�.
	-- 4. FirstGameguardCheck��û�� ǥ���Ѵ�.
	-- 5. FirstGameguardCheck������ ǥ���Ѵ�.
	-- 6. FirstGameguardCheck������ �ֱ� ������ ��� Ŀ�ǵ� ��û�� �����Ѵ�.
	-- 7. FirstGameguardCheck������ ���� ĳ���� ����Ʈ�� �����ش�. 
	-- 8. FirstGameguardCheck������ ���� ��� Ŀ�ǵ� ��û�� �����Ѵ�.
	*/

	MMatchObject* pObj = GetObject( pCommand->GetSenderUID() );
	if( NULL == pObj )
		return false;	///< Agent���� ���� MMatchObject�� ���⶧���� MMatchObject�� ���ؼ� 
						///<	Ŀ�ǵ� ó���� �ϴ� ������ �˻��� ��� �Ѵ�.
	
	//if( pObj->IsSendFirstGameguardRequest() && !pObj->IsRecvFirstGameguardResponse() )
	//{
	//	if( MC_RESPONSE_FIRST_GAMEGUARD_AUTH == pCommand->GetID() )
	//	{
	//		DWORD dwIndex;
	//		DWORD dwValue1;
	//		DWORD dwValue2;
	//		DWORD dwValue3;

	//		pCommand->GetParameter( &dwIndex, 0, MPT_UINT );
	//		pCommand->GetParameter( &dwValue1, 1, MPT_UINT );
	//		pCommand->GetParameter( &dwValue2, 2, MPT_UINT );
	//		pCommand->GetParameter( &dwValue3, 3, MPT_UINT );

	//		OnResponseFirstGameguardAuth( pCommand->GetSenderUID(), dwIndex, dwValue1, dwValue2, dwValue3 );

	//		return false;
	//	}
	//	else
	//	{
	//		return true;
	//	}
	//}

	return false;
}