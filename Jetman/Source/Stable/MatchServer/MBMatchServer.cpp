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
#include "MBMatchAuth.h"
#include "MDebug.h"
#include "MMatchStatus.h"
#include "MMatchSchedule.h"
#include "MSharedCommandTable.h"
#include "MMatchConfig.h"
#include "MMatchEventFactory.h"
#include "MMatchLocale.h"
#include "MBlobArray.h"
#include "MMatchCheckLoopTime.h"




void __cdecl MBRCPLog(const char *pFormat,...)
{
	char szBuf[256];

	va_list args;

	va_start(args,pFormat);
	vsprintf(szBuf, pFormat, args);
	va_end(args);

	int nEnd = (int)(strlen(szBuf)-1);
	if ((nEnd >= 0) && (szBuf[nEnd] == '\n')) {
		szBuf[nEnd] = NULL;
		strcat(szBuf, "\n");
	}
	mlog(szBuf);
}

MBMatchServer::~MBMatchServer() 
{
	OnDestroy();
}

bool MBMatchServer::OnCreate(void)
{
	SetupRCPLog(MBRCPLog);

	CMatchServerApp* pApp = (CMatchServerApp*)AfxGetApp();

	// UDP�ʱ�ȭ���� ���� ������ �Ǿ�� ��.
	// ����͸� ���� ��Ŷ�� ����͸� �ʱ�ȭ ���� ���� �����ϸ� NULL������ ������ �Ͼ.


	if( !MMatchServer::OnCreate() )
		return false;

	if( !m_ConfigReloader.Create() )
		return false;

	if( !InitSecrity() )
	{
		mlog( "init security fail.\n" );
		return false;
	}
	
	if( !LoadFileMD5HashValue() )
		return false; // - by SungE 2007-05-28
	

	if( m_KillTracker.Init(&(MGetServerConfig()->GetKillTrackerConfig())) )
		return false;


	WriteServerInfoLog();

	MGetCheckLoopTimeInstance()->SetInit();

#ifdef _DEBUG
	//m_Tester.Init( this );
	//m_Tester.DoTest( GetGlobalClockCount() );
#endif
	return true;
}


bool MBMatchServer::LoadFileMD5HashValue()
{
#ifndef _DEBUG
	char curDirectory[MAX_PATH] = {0};
	GetCurrentDirectory(MAX_PATH, curDirectory);

	char filePath[MAX_PATH] = {0};
	sprintf(filePath, "%s\\MD5HashValue.txt", curDirectory);

	FILE *ReadFp = fopen(filePath, "rb");
	if( NULL == ReadFp )
	{
		mlog( "Load MD5 fail.\n" );
		return false;
	}

	int readNum = 0;
	for (int i = 0; i < 16; i++)
	{
		fscanf(ReadFp, "%x", &readNum);
		m_szMD5Value[i] = (unsigned char)readNum;
	}
	
	fclose(ReadFp);


	mlog("FILE PATH : %s\nMD5 Hash Value => ", filePath);
	for( int i=0; i<16; i++)
	{
		mlog("%02x ", m_szMD5Value[i]);
	}

	mlog("\n");
#endif

	return true;
}


void MBMatchServer::ReleaseSecurity()
{

}


void MBMatchServer::OnDestroy(void)
{
	ReleaseSecurity();
}

void MBMatchServer::OnPrepareCommand(MCommand* pCommand)
{
	// Ŀ�ǵ� �α� �����
	if(m_pCmdLogView==NULL) return;

	CMatchServerApp* pApp = (CMatchServerApp*)AfxGetApp();
	if (pApp->CheckOutputLog() == false) return;


	CCommandLogView::CCommandType t;
	if(pCommand->m_pCommandDesc->IsFlag(MCDT_LOCAL)==true) t = CCommandLogView::CCT_LOCAL;
	else if(pCommand->m_Sender==m_This) t = CCommandLogView::CCT_SEND;
	else if(pCommand->m_Receiver==m_This) t = CCommandLogView::CCT_RECEIVE;
	else _ASSERT(FALSE);

	m_pCmdLogView->AddCommand(GetGlobalClockCount(), t, pCommand);
/*
#ifdef _DEBUG
#ifndef _DEBUG_PUBLISH
	// Ŀ�ǵ� �α� �����
	if(m_pCmdLogView==NULL) return;

	CMatchServerApp* pApp = (CMatchServerApp*)AfxGetApp();
	if (pApp->CheckOutputLog() == false) return;


	CCommandLogView::CCommandType t;
	if(pCommand->m_pCommandDesc->IsFlag(MCDT_LOCAL)==true) t = CCommandLogView::CCT_LOCAL;
	else if(pCommand->m_Sender==m_This) t = CCommandLogView::CCT_SEND;
	else if(pCommand->m_Receiver==m_This) t = CCommandLogView::CCT_RECEIVE;
	else _ASSERT(FALSE);
	
	m_pCmdLogView->AddCommand(GetGlobalClockCount(), t, pCommand);
#endif
#endif
*/
}



MBMatchServer::MBMatchServer(COutputView* pView)
{
	m_pView = pView;
	m_pCmdLogView = NULL;
	
	SetKeeperUID( MUID(0, 0) );

}

void MBMatchServer::Shutdown()
{
	MMatchServer::Shutdown();
	AfxGetMainWnd()->PostMessage(WM_DESTROY);
//	AfxGetMainWnd()->DestroyWindow();
}

void MBMatchServer::Log(unsigned int nLogLevel, const char* szLog)
{
	MMatchServer::Log(nLogLevel, szLog);	

	if ((nLogLevel & LOG_PROG) == LOG_PROG)
	{
		if(m_pView==NULL) return;

		CTime theTime = CTime::GetCurrentTime();
		CString szTime = theTime.Format( "[%c] " );

		m_pView->AddString(szTime, TIME_COLOR, false);
		m_pView->AddString(szLog, RGB(0,0,0));
	}
}

bool MBMatchServer::InitSubTaskSchedule()
{
	/*  ��Ϲ���� �����ؾ� ��.
		�켱�� �ڵ� �ӿ��ٰ�. */

	// TODO: ���װ� �ִ� �� ���� �ּ�ó�� �س��ҽ��ϴ�. Ŭ������û ����ġ�� ������ �ʾƾ� �� �� �����ϴ�. Ȯ�� ��� -bird
	// ���� ���� - SungE.

	// clan������ ��츸.
	if( MSM_CLAN == MGetServerConfig()->GetServerMode() ){
		if( !AddClanServerSwitchDownSchedule() )
			return false;

		if( !AddClanServerAnnounceSchedule() )
		return false;
	}

	return true;
}

// ���� ���۽� Ŭ���� �����ϰ�� ��ϵǴ� ������.
bool MBMatchServer::AddClanServerSwitchDownSchedule()
{
	int a = 0;

	MCommand* pCmd = CreateCommand( MC_MATCH_SCHEDULE_CLAN_SERVER_SWITCH_DOWN, MUID(0, 0) );
	if( 0 == pCmd )
		return false;

	tm* ptm = MMatchGetLocalTime();

	// �̰��� ������ �ѹ��� ������ ������, �Ŵ� ������ �ϴ°����� ������ �ؾ���.
	// Ŀ�ǵ尡 ������ �ϴ� �������� Ŀ�ǵ带 ������Ʈ�ؼ� �ٽ� �����췯�� ����� �ϴ������� ������ ����.
	// OnCommnad�迭���� Ŭ�� ���� �ٿ� Ŀ�ǵ尡 ������ �ɽÿ� �����ٿ� Ŀ�ǵ带 �����޷� �� ������ �ϰ�,
	//  ������ 1�Ͽ� Ŭ�� ������ �ٽ� �����ϴ� Ŀ�ǵ带 �����Ͽ� �߰��ϴ� ������� ��.
	// Ŭ������ �ٿ� Ŀ�ǵ� ���� -> ������ 1�� Ŭ�� ���� �ٽ� ���� Ŀ�ǵ� ����, ��� -> ������ Ŭ������ �ٿ� Ŀ�ǵ� ����, ���.
	// ���� Ŭ�����ٿ� �ð��߿� ������� ����(���� ����۵�) Ŭ�����ٿ� �ð��� �̹� �������� �� �ð����� �����Ͽ�
	// ��ٷ� Ŭ�����ٿ��� �����ְ� Ŭ���� ���۽ð��� ����, ������ش�.

	MMatchScheduleData* pScheduleData = m_pScheduler->MakeOnceScheduleData( ptm->tm_year - 100, ptm->tm_mon + 1, GetMaxDay(), 23, 50, pCmd );
	if( 0 == pScheduleData ){
		delete pCmd;
		return false;
	}

	if( !m_pScheduler->AddDynamicSchedule(pScheduleData) ){
		delete pCmd;
		delete pScheduleData;
		return false;
	}

	mlog( "MBMatchServer::AddClanServerSwitchDownSchedule - make close clan mode schedule success. next close clan mode time is %d-%d-%d %d:%d\n",
		pScheduleData->GetYear(), pScheduleData->GetMonth(), pScheduleData->GetDay(),
		pScheduleData->GetHour(), pScheduleData->GetMin() );

	return true;
}


bool MBMatchServer::AddClanServerSwitchUpSchedule()
{
	// ������ 1�� ��ħ 10�ÿ� Ŭ������ Ŭ���� Ȱ��ȭ.
	MCommand* pCmd = CreateCommand( MC_MATCH_SCHEDULE_CLAN_SERVER_SWITCH_ON, MUID(0, 0) );
	if( 0 == pCmd )
	{
		mlog( "MBMatchServer::AddClanServerSwitchUpSchedule - make clan mode open command fail.\n" );
		return false;
	}

	tm* ptm = MMatchGetLocalTime();

	unsigned char cYear;
	unsigned char cMon;

	// �������� �������� �Ѿ���� �˻���.
	if( 12 >= (ptm->tm_mon + 2) )
	{
		cYear = ptm->tm_year - 100;
		cMon  = ptm->tm_mon + 2;
	}
	else
	{
		// �������� ���� 1���� ���.
		cYear = ptm->tm_year - 99;
		cMon  = 1;
	}

	MMatchScheduleData* pScheduleData = m_pScheduler->MakeOnceScheduleData( cYear, cMon, 1, 9, 0, pCmd );
	if( 0 == pScheduleData )
	{
		delete pCmd;
		return false;
	}

	if( !m_pScheduler->AddDynamicSchedule(pScheduleData) )
	{
		delete pCmd;
		delete pScheduleData;
		return false;
	}

	mlog( "MBMatchServer::AddClanServerSwitchUpSchedule - make open clan mode schedule success. next open time is %d-%d-%d %d:%d\n",
		pScheduleData->GetYear(), pScheduleData->GetMonth(), pScheduleData->GetDay(),
		pScheduleData->GetHour(), pScheduleData->GetMin() );

	return true;
}


// ���� ���۽� ��ϵǾ�� �ϴ� �������� ������.
bool MBMatchServer::AddClanServerAnnounceSchedule()
{
	char szTest[] = "clan mode is closed on 23:50";

	MCommand* pCmd = CreateCommand( MC_MATCH_SCHEDULE_ANNOUNCE_MAKE, MUID(0, 0) );
	if( 0 == pCmd )
		return false;

	MCommandParameterString* pCmdPrmStr = new MCommandParameterString( szTest );
	if( 0 == pCmdPrmStr ){
		delete pCmd;
		return false;
	}
	
	if( !pCmd->AddParameter(pCmdPrmStr) ){
		delete pCmd;
		return false;
	}

	tm* ptm = MMatchGetLocalTime();

	MMatchScheduleData* pScheduleData = m_pScheduler->MakeOnceScheduleData( ptm->tm_year - 100, ptm->tm_mon + 1, GetMaxDay(), 23, 40, pCmd );
	if( 0 == pScheduleData ){
		delete pCmd;
		return false;
	}

	if( !m_pScheduler->AddDynamicSchedule(pScheduleData) ){
		pScheduleData->Release();
		delete pScheduleData;
		return false;
	}
	
	return true;
}



char log_buffer[65535];

void AddStr(const char* pFormat,...)
{
	va_list args;
	char temp[1024];

	va_start(args, pFormat);
	vsprintf(temp, pFormat, args);

	strcat(log_buffer, temp);
	va_end(args);
}

void MBMatchServer::OnViewServerStatus()
{
	MGetServerStatusSingleton()->SaveToLogFile();
}

ULONG MBMatchServer::HShield_MakeGuidReqMsg(unsigned char *pbyGuidReqMsg, unsigned char *pbyGuidReqInfo)
{
#ifdef _HSHIELD
	if( MGetServerConfig()->IsUseHShield() )
		return _AntiCpSvr_MakeGuidReqMsg(pbyGuidReqMsg, pbyGuidReqInfo);
#endif
	return 0L;
}

ULONG MBMatchServer::HShield_AnalyzeGuidAckMsg(unsigned char *pbyGuidAckMsg, unsigned char *pbyGuidReqInfo, unsigned long **ppCrcInfo)
{
#ifdef _HSHIELD
	if( MGetServerConfig()->IsUseHShield() )
		return _AntiCpSvr_AnalyzeGuidAckMsg(pbyGuidAckMsg, pbyGuidReqInfo, ppCrcInfo);
#endif
	return 0L;
}

ULONG MBMatchServer::HShield_MakeReqMsg(unsigned long *pCrcInfo, unsigned char *pbyReqMsg, unsigned char *pbyReqInfo, unsigned long ulOption)
{
#ifdef _HSHIELD
	if( MGetServerConfig()->IsUseHShield() )
		return _AntiCpSvr_MakeReqMsg(pCrcInfo, pbyReqMsg, pbyReqInfo, ulOption);
#endif
	return 0L;
}

ULONG MBMatchServer::HShield_AnalyzeAckMsg(unsigned long *pCrcInfo, unsigned char *pbyAckMsg, unsigned char *pbyReqInfo)
{
#ifdef _HSHIELD
	if( MGetServerConfig()->IsUseHShield() )
		return _AntiCpSvr_AnalyzeAckMsg(pCrcInfo, pbyAckMsg, pbyReqInfo);
#endif
	return 0L;
}

bool MBMatchServer::IsKeeper( const MUID& uidKeeper )
{
	MMatchObject* pObj = GetObject( uidKeeper );
	if( 0 == pObj )
		return false;

	if( !MGetServerConfig()->IsKeeperIP(pObj->GetIPString()) )
	{
		mlog( "Keeper hacking. " );
		if( 0 != pObj->GetIPString() )
			mlog( "IP:%s, ", pObj->GetIPString() );

		if( (0 != pObj->GetCharInfo()) && (0 != pObj->GetCharInfo()->m_szName) )
			mlog( "Name:%s", pObj->GetCharInfo()->m_szName );

		mlog( "\n" );

		return false;
	}

	return true;
}


void MBMatchServer::WriteServerInfoLog()
{
	mlog( "\n" );
	mlog( "================================== Server configure info ==================================\n" );

	char szTemp[256];
	sprintf(szTemp, "Release Date : %s", __DATE__);
	Log(LOG_PROG, szTemp);
	
#ifdef _XTRAP
	if( MGetServerConfig()->IsUseXTrap() )
	{
		LOG( LOG_PROG, "X-Trap On" );
		LOG( LOG_PROG, "X-Trap usable state : (true)" );
	}
	else
		LOG( LOG_PROG, "X-Trap Off" );
#endif

#ifdef _HSHIELD
	if( MGetServerConfig()->IsUseHShield() )
	{
		LOG( LOG_PROG, "Hack Shield On" );
		LOG( LOG_PROG, "Hack Shield usable state : (true)" );
	}
	else
		LOG( LOG_PROG, "Hack Shield Off" );
#endif

#ifndef _DEBUG
	// MD5 Ȯ�� �ϴ��� ���� �α׷� �����.
	if (MGetServerConfig()->IsUseMD5())
	{
		LOG( LOG_PROG, "MD5 Check On" );
	}
	else
	{
		LOG( LOG_PROG, "MD5 Check Off" );
	}
#endif

	if (MGetServerConfig()->IsUseLoopLog())
	{
		LOG(LOG_PROG, "Loop Log Save On - Time Gap : (%u)", MGetServerConfig()->GetLoopTimeGap());
	}
	else
	{
		LOG(LOG_PROG, "Loop Log Save Off");
	}

	if( MC_KOREA == MGetLocale()->GetCountry() )
		LOG( LOG_PROG, "Server Country : KOREA" );
	else if( MC_US == MGetLocale()->GetCountry() )
		LOG( LOG_PROG, "Server Country : US" );
	else if( MC_JAPAN == MGetLocale()->GetCountry() )
		LOG( LOG_PROG, "Server Country : JAPAN" );
	else if( MC_BRAZIL == MGetLocale()->GetCountry() )
		LOG( LOG_PROG, "Server Country : BRAZIL" );
	else if( MC_INDIA == MGetLocale()->GetCountry() )
		LOG( LOG_PROG, "Server Country : INDIA" );
	else if( MC_NHNUSA == MGetLocale()->GetCountry() )
		LOG( LOG_PROG, "Server Country : NHNUSA" );
	else
	{
		ASSERT( 0 && "���� ������ ���ּ���." );
		LOG( LOG_PROG, "!!!!!!!!!Not setted country code!!!!!!!!!!!!!" );
	}
	
	LOG( LOG_PROG, "Command version : (%u)", MCOMMAND_VERSION );
	LOG( LOG_PROG, "Event usable state : (%s)", MGetServerConfig()->IsUseEvent() ? "true" : "false" );
	LOG( LOG_PROG, "Load event size : (%u)", MMatchEventFactoryManager::GetInstance().GetLoadEventSize() );
	LOG( LOG_PROG, "FileCRCCheckSum usable state : (%s)", MGetServerConfig()->IsUseFileCrc() ? "true" : "false" );
	LOG( LOG_PROG, "FileCRC size : (%u)", MMatchAntiHack::GetFielCRCSize() );
	LOG( LOG_PROG, "Country Code Filter usalbe state : (%s)", MGetServerConfig()->IsUseFilter() ? "true" : "false" );
	LOG( LOG_PROG, "Accept Invalied IP state : (%s)", MGetServerConfig()->IsAcceptInvalidIP() ? "true" : "false" );
	LOG( LOG_PROG, "Keeper IP : (%s)", MGetServerConfig()->GetKeeperIP().c_str() );
	in_addr ar;
	ar.S_un.S_addr = MGetServerConfig()->GetMonitorUDPIP();
	LOG( LOG_PROG, "Monitor IP : (%s)", inet_ntoa(ar) );
	LOG( LOG_PROG, "Ticket use : %d\n", MGetServerConfig()->IsUseTicket() );

	mlog( "===========================================================================================\n" );
	mlog( "\n" );
}


bool MBMatchServer::InitHShiled()
{
#ifdef _HSHIELD
	// HSHIELD Init
	if( MGetServerConfig()->IsUseHShield() )
	{
		// HShield Init ������ ������� ����.
		// if(MPacketHShieldCrypter::Init() != ERROR_SUCCESS)
		// 	bResult = false;
	
		GetCurrentDirectory( sizeof( m_strFileCrcDataPath), m_strFileCrcDataPath);
		strcat( m_strFileCrcDataPath, "\\HackShield.crc");
		ULONG dwRet = _AntiCpSvr_Initialize(m_strFileCrcDataPath);

		if( dwRet != ERROR_SUCCESS )
		{
			AfxMessageBox("_AntiCpSvr_Initialize Failed!");
			return false;
		}

		LOG( LOG_PROG, "HackShield Init OK." );
	}
#endif

	return true;
}

#include "MMatchBuff.h"

void MBMatchServer::OnRun(void)
{
	MMatchServer::OnRun();

	const DWORD dwGlobalClock = GetGlobalClockCount();

	MGetCheckLoopTimeInstance()->SetNHNUSAAuthTick();
#ifdef LOCALE_NHNUSA
	if( MGetServerConfig()->IsUseNHNUSAAuth() )
	{
		if( GetNHNRTA().IsElapsed(dwGlobalClock) )
		{
			GetNHNRTA().RTA( GetClientCount(), dwGlobalClock );
		}
	}
#endif

	MGetServerStatusSingleton()->SetRunStatus(113);

	MGetCheckLoopTimeInstance()->SetGameGuardTick();
#ifdef _GAMEGUARD
	OnRun_GameGuard(dwGlobalClock);
#endif

	MGetCheckLoopTimeInstance()->SetXTrapTick();
#ifdef _XTRAP
	OnRun_XTrap(dwGlobalClock);
#endif

	MGetServerStatusSingleton()->SetRunStatus(114);

	MGetCheckLoopTimeInstance()->SetMonitorTick();

	MGetCheckLoopTimeInstance()->SetKillTrackerTick();
	GetKillTracker().Update( dwGlobalClock );
	
	MGetServerStatusSingleton()->SetRunStatus(115);
}

void MBMatchServer::OnRun_GameGuard(const DWORD dwGlobalClock)
{
#ifdef _GAMEGUARD
	if( MGetServerConfig()->IsUseGamegaurd() )
	{
		static DWORD dwLastGameguardCheckTime = dwGlobalClock;
		if( GAMEGUARD_CHECKTIME	 < (dwGlobalClock - dwLastGameguardCheckTime) )
		{
			MMatchObjectList::iterator itGameguard, endGameguard;
			itGameguard = m_Objects.begin();
			endGameguard = m_Objects.end();
			while( itGameguard != endGameguard )
			{
				CheckGameguard( itGameguard->second, dwGlobalClock );
				++itGameguard;
			}
			dwLastGameguardCheckTime = dwGlobalClock;
		}
	}
#endif
}

void MBMatchServer::OnRun_XTrap(const DWORD dwGlobalClock)
{
#ifdef _XTRAP
	DWORD retVal = 0;
	if (MGetServerConfig()->IsUseXTrap())
	{
		MMatchObjectList::iterator iter;
		for (iter = m_Objects.begin(); iter != m_Objects.end(); iter++)
		{
			MMatchObject *pObj = (MMatchObject*)(iter->second);
			if (pObj == NULL)
			{
				continue;
			}

			MBMatchXTrapCC *pXTrapCC = GetXTrapCC(pObj->GetUID());
			if (pXTrapCC == NULL)
			{
				continue;
			}

			// 20�ʸ��� �ѹ��� ȣ���Ѵ�.(�α����� �Ϸ�� ������, �������� ������(���� ó������ ���� ����))
			DWORD dwTimeGap = dwGlobalClock - pXTrapCC->GetXTrapLastUpdateTime();
			if (pObj->GetDisconnStatusInfo().GetStatus() == MMDS_CONNECTED && pObj->IsLoginCompleted())
			{
				if (dwGlobalClock > pXTrapCC->GetXTrapLastUpdateTime() && dwTimeGap >= MAX_XTRAP_ELAPSEDTIME)
				{
					retVal = pXTrapCC->XTrapGetHashData();
					OnRequestXTrapSeedKey(pObj->GetUID(), pXTrapCC->GetXTrapComBuf());
					pXTrapCC->SetXTrapLastUpdateTime(dwGlobalClock);

					// ���� ó�� (�ش� ������ ������ �����Ѵ�.)
					if (retVal != XTRAP_API_RETURN_OK)
					{
//						pObj->SetXTrapHackerDisconnectWaitInfo();
						pObj->DisconnectHacker( MMHT_XTRAP_HACKER );
						mlog("pXTrapCC->XTrapGetHashData() return value = %d\n", retVal);
						continue;
					}
				}
			}
		}
	}
#endif
}

void MBMatchServer::CheckGameguard( MMatchObject* pObj, const DWORD dwTime )
{
#ifdef _GAMEGUARD
	if( 0 == pObj ) return;

	if( (MMDS_DISCONN_WAIT == pObj->GetDisconnStatusInfo().GetStatus()) || 
		(MMDS_DISCONNECT == pObj->GetDisconnStatusInfo().GetStatus()) )
		return;

	// �������� �����ϴ� ù���� ������ �Ϸ���� �ʾ����� ���� ���������� �ؼ� �ȵȴ�
	if (!pObj->IsRecvFirstGameguardResponse())
		return;

	MBMatchGameguard* pGameguard = GetGameguard( pObj->GetUID() );
	if( 0 == pGameguard )
	{
		if( !pObj->IsLoginCompleted() )
		{
			// ���� �α� �۾��� �Ϸ���� �ʾ����Ƿ� �׳� ������.
			return;
		}

		mlog( "gameguard obj is null point(1). AID(%u)\n", pObj->GetAccountInfo()->m_nAID );

		// ������� ���� �α� �۾��� �Ϸ�� �Ŀ��� ���� ��ü�� �����Ƿ� 
		// ������ ������ �Ǵ�.
//		pObj->SetGameguardHackerDisconnectWaitInfo();
		pObj->DisconnectHacker(MMHT_GAMEGUARD_HACKER);
		return;
	}

	if( MAX_ELAPSEDTIME < (dwTime - pGameguard->GetLastCheckTime()) )
	{
		LOG(LOG_PROG,  "GAMEGUARD ERR :elapsed time(%u). AID(%u), CreateCount(%u)\n"
			, dwTime - pGameguard->GetLastCheckTime()
			, pObj->GetAccountInfo()->m_nAID
			, pGameguard->GetCreateAuthCount() );

//		pObj->SetGameguardHackerDisconnectWaitInfo();
		pObj->DisconnectHacker(MMHT_GAMEGUARD_HACKER);
		return;
	}

	if( !pGameguard->CreateAuthQuery() )
	{
		// ���� Ŭ���̾�Ʈ���� ������ ��ٸ��� ����.
		if( ERROR_GGAUTH_NO_REPLY == pGameguard->GetLastError() )
			return;

		// ���ο� �������� ����µ� �����ϸ� ���� ���� ��Ŵ.
		// ggtest
		LOG(LOG_PROG,  "GAMEGUARD ERR :err make gameguard auth. AID(%u), ErrorCode(%u), CreateAuthCount(%u)\n"
			, pObj->GetAccountInfo()->m_nAID
			, pGameguard->GetLastError()
			, pGameguard->GetCreateAuthCount() );

//		pObj->SetGameguardHackerDisconnectWaitInfo();
		pObj->DisconnectHacker(MMHT_GAMEGUARD_HACKER);

		return;
	}

	MMatchServer* pServer = MMatchServer::GetInstance();
	if( 0 == pServer )
	{
		ASSERT( 0 );
		mlog( "MMatchObject::CheckGameguard - game server address is null...\n" );
		return;
	}

	const GG_AUTH_DATA& AuthData = pGameguard->GetServerAuth()->GetAuthQuery();
	pServer->RequestGameguardAuth( pObj->GetUID()
		, AuthData.dwIndex
		, AuthData.dwValue1
		, AuthData.dwValue2
		, AuthData.dwValue3 );
#endif
}

bool MBMatchServer::InitSecrity()
{
	// not use. - by SungE 2007-05-03
	//if( !InitHShiled() ) 
	//	return false;

	if( !InitNHNAuth() )
		return false;

	if( !InitGameguard() )
		return false;

	if( !InitXtrap() )
		return false;

	if( !InitGameOn() )
		return false;

#ifdef NEW_AUTH_MODULE
	if( !InitNetmarble() )
		return false;
#endif

	mlog( "success init security.\n" );

	return true;
}


bool MBMatchServer::DeleteGameguardUserSecurityInfo( const MUID& uidUser )
{
	return true;
}

void MBMatchServer::OnNetClear( const MUID& CommUID )
{
	// MMatchServer::OnNetClear( CommUID );

    MMatchObject* pObj = GetObject(CommUID);
	if (pObj)
		OnCharClear(pObj->GetUID());

	MAgentObject* pAgent = GetAgent(CommUID);
	if (pAgent)
		AgentRemove(pAgent->GetUID(), NULL);

	MServer::OnNetClear(CommUID);
}

bool MBMatchServer::PreCheckAddObj(const MUID& uidObj)
{
	return true;
}

bool MBMatchServer::DeleteXTrapUserSecurityInfo(const MUID& uidUser)
{
	return true;
}

void MBMatchServer::OnRequestXTrapSeedKey(const MUID &uidChar, unsigned char *pComBuf)
{
	//MCommand* pCmd = CreateCommand(MC_REQUEST_XTRAP_SEEDKEY, uidChar);
	//if (pCmd != 0)
	//{
	//	void *pBlob = MMakeBlobArray(sizeof(unsigned char), XTRAP_CS4_COMSIZE_PACKDATA);
	//	unsigned char* pCmdBlock = (unsigned char*)MGetBlobArrayElement(pBlob, 0);
	//	CopyMemory(pCmdBlock, pComBuf, XTRAP_CS4_COMSIZE_PACKDATA);

	//	if (pCmd->AddParameter(new MCmdParamBlob(pBlob, MGetBlobArraySize(pBlob))))
	//	{
	//		Post(pCmd);
	//	}
	//	MEraseBlobArray(pBlob);
	//}
}

void MBMatchServer::XTrap_OnAdminReloadFileHash(const MUID& uidAdmin)
{
	MMatchObject* pObj = GetObject(uidAdmin);
	if (!IsEnabledObject(pObj)) return;
	if (!IsAdminGrade(pObj)) return;

#ifdef _XTRAP
	if (MGetServerConfig()->IsUseXTrap())
	{
		ReloadXTrapMapFile();
	}
#endif	
}

bool MBMatchServer::InitXtrap()
{
#ifdef _XTRAP
	InitMemPool(MBMatchXTrapCC);

	if (MGetServerConfig()->IsUseXTrap())
	{
		if (!LoadXTrapFile())
		{
			return false;
		}

		MBMatchUserSecurityMgr* pXTrapMgr = new MBMatchUserSecurityMgr(MSI_XTRAP, "XTrapCS4", 0);
		if (pXTrapMgr == 0)
		{
			mlog("Failed! new MBMatchUserSecurityMgr()\n");
			return false;
		}
		m_Security.AddManager(MSI_XTRAP, pXTrapMgr);
		mlog("XTrap Files Load and Init success!\n");
	}
#endif
	return true;
}


bool MBMatchServer::InitNHNAuth()
{

	// move - by SungE 2007-05-04
#ifdef LOCALE_NHNUSA
	if( MGetServerConfig()->IsUseNHNUSAAuth() )
	{
		bool bInitAuth = false;
		if( NSM_REAL == MGetServerConfig()->GetNHNServerMode() )
		{
			if( !GetNHNModule().InitAuthReal() )
			{
				mlog( "init nhn usa auth is fail(real).\n" );
				return false;
			}
		}
		else if( NSM_ALPHA == MGetServerConfig()->GetNHNServerMode() )
		{
			if( !GetNHNModule().InitAuthAlpha() )
			{
				mlog( "init nhn usa auth is fail(alpha).\n" );
				return false;
			}
		}

		mlog( "success init nhn auth.\n" );

		if( NSM_REAL == MGetServerConfig()->GetNHNServerMode() )
		{
			if( !GetNHNRTA().InitRTAReal(MGetServerConfig()->GetServerName()) )
			{
				mlog( "init nhn usa report is fail(real).\n" );
				return false;
			}
		}
		else if( NSM_ALPHA == MGetServerConfig()->GetNHNServerMode() )
		{
			if( !GetNHNRTA().InitRTAAlpha(MGetServerConfig()->GetServerName()) )
			{
				mlog( "init nhn usa report is fail(alpha).\n" );
				return false;
			}
		}

		mlog( "success init nhn usa report.\n" );
	}
#endif

	return true;
}


bool MBMatchServer::InitGameguard()
{
#ifdef _GAMEGUARD
	if( MGetServerConfig()->IsUseGamegaurd() )
	{
		const DWORD dwRet = InitMBMatchGameguardAuth(); 
		if( ERROR_SUCCESS != dwRet )
		{
			mlog( "init gameguard fail. err(%u)\n", dwRet );
			return false;
		}
	
		MBMatchUserSecurityMgr* pGameguardMgr = new MBMatchUserSecurityMgr( MSI_GAMEGUARD, "game guard", MAX_ELAPSEDTIME * 2 );
		if( NULL == pGameguardMgr )
		{
			mlog( "Init gameguard security manager fail!\n" );
			return false;
		}

		m_Security.AddManager( MSI_GAMEGUARD, pGameguardMgr );
	}

	mlog( "init gameguard success.\n" );
#endif

	return true;
}


bool MBMatchServer::InitGameOn()
{
#ifdef LOCALE_JAPAN
	if ( GetGameOnModule().InitModule() == false)
	{
		mlog( "init GameOn fail.\n" );
		return false;
	}
#endif

	mlog( "init GameOn success.\n" );
	return true;
}

#ifdef NEW_AUTH_MODULE
bool MBMatchServer::InitNetmarble()
{
#ifdef LOCALE_KOREA
	if ( MGetNetmarbleModule().InitModule() == false)
	{
		mlog( "init Netmarble failed.\n");
		return false;
	}

	mlog( "init Netmarble success.\n");
#endif

	return true;
}
#endif


void MBMatchServer::SafePushMonitorUDP( const DWORD dwIP, const WORD wPort, const char* pData, const DWORD dwDataSize )
{

}

bool MBMatchServer::SendMonitorUDP(const DWORD dwIP, const USHORT nPort, const string& strMonitorCommand)
{
	char* szMonitorCommand;
	szMonitorCommand = new char[ strMonitorCommand.length() ];
	strncpy( szMonitorCommand, strMonitorCommand.c_str(), strMonitorCommand.length() );
	return m_SafeUDP.Send(dwIP, nPort, szMonitorCommand, static_cast<DWORD>(strMonitorCommand.length()));
}