#include "StdAfx.h"
#include ".\mlocator.h"
#include "MLocatorDBMgr.h"
#include "CustomDefine.h"
#include "MSafeUDP.h"
#include "mmsystem.h"
#include "Msg.h"
#include "MSharedCommandTable.h"
#include "MBlobArray.h"
#include "MLocatorConfig.h"
#include "MLocatorUDP.h"
#include "MCountryFilter.h"
#include "MCommandCommunicator.h"
#include "MErrorTable.h"
#include "MCommandBuilder.h"
#include "MUtil.h"
#include "MLocatorStatistics.h"
#include "MServerStatus.h"
#include "MLogManager.h"

#pragma comment( lib, "winmm.lib" )

MLocator* g_pMainLocator = NULL;
void SetMainLocator( MLocator* pLocator ) { g_pMainLocator = pLocator; }
MLocator* GetMainLocator() { return g_pMainLocator; }


MLocator::MLocator(void)
{
	m_pDBMgr							= 0; 
	m_pSafeUDP							= 0;
	m_dwLastServerStatusUpdatedTime		= timeGetTime();
	m_dwLastUDPManagerUpdateTime		= timeGetTime();
	m_vpServerStatusInfoBlob			= 0;
	m_nLastGetServerStatusCount			= 0;
    m_nServerStatusInfoBlobSize			= 0;
	m_pServerStatusMgr					= 0;
	m_pRecvUDPManager					= 0;
	m_pSendUDPManager					= 0;
	m_pBlockUDPManager					= 0;
	m_nRecvCount						= 0;
	m_nSendCount						= 0;
	m_nDuplicatedCount					= 0;
	m_dwLastLocatorStatusUpdatedTime	= timeGetTime();
	//m_pCountryCodeFilter = 0;
	m_pCountryFilter					= 0;

	m_This = GetLocatorConfig()->GetLocatorUID();

	InitializeCriticalSection( &m_csCommandQueueLock );
}

MLocator::~MLocator(void)
{
	Destroy();

	DeleteCriticalSection( &m_csCommandQueueLock );
}


bool MLocator::Create()
{
	SetMainLocator( this );
	OnRegisterCommand( &m_CommandManager );

	if( !GetLocatorConfig()->IsInitCompleted() )
	{
		if( !GetLocatorConfig()->LoadConfig() )
			return false;
	}
		
	if( !InitDBMgr() )
	{
		mlog( "MLocator::Create - DB�ʱ�ȭ ����.\n" );
		return false;
	}

	if( !InitServerStatusMgr() )
	{
		mlog( "MLocator::Create - ServerStatusMgr ��� �ʱ�ȭ ����.\n" );
		return false;
	}

	if( !InitUDPManager() )
	{
		mlog( "MLocator::Create - UDP Manager ��� �ʱ�ȭ ����.\n" );
		return false;
	}

	if( !InitCountryCodeFilter() )
	{
		mlog( "MLocator::Create - ���� ��� ���� �ڵ� ����Ʈ �ʱ�ȭ ����.\n" );
		return false;
	}

	if( !InitSafeUDP() )
	{
		mlog( "MLocator::Create - SafeUDP�ʱ�ȭ ����.\n" );
		return false;
	}

#ifdef _DEBUG
	InitDebug();
#endif

	mlog( "MLocator create.\n" );

	return true;
}


bool MLocator::InitDBMgr()
{
	if( 0 != m_pDBMgr )
		ReleaseDBMgr();

	m_pDBMgr = new MLocatorDBMgr;
	if( 0 != m_pDBMgr )
	{
		const CString strDSNString = m_pDBMgr->BuildDSNString( GetLocatorConfig()->GetDBDSN(),
															   GetLocatorConfig()->GetDBUserName(), 
															   GetLocatorConfig()->GetDBPassword() );
		const bool bConnect = m_pDBMgr->Connect( strDSNString );
		if( bConnect )
		{
			GetDBServerStatus( timeGetTime(), true );

			return m_pDBMgr->StartUpLocaterStauts( GetLocatorConfig()->GetLocatorID(), 
				GetLocatorConfig()->GetLocatorIP(), 
				GetLocatorConfig()->GetLocatorPort(),
				GetLocatorConfig()->GetMaxElapsedUpdateServerStatusTime() );
		}
	}

	return false;
}


bool MLocator::InitSafeUDP()
{
	if( 0 == m_pSafeUDP )
	{
		m_pSafeUDP = new MSafeUDP;
		if( 0 != m_pSafeUDP )
		{
			if( m_pSafeUDP->Create(true, GetLocatorConfig()->GetLocatorPort()) )
			{
				m_pSafeUDP->SetCustomRecvCallback( UDPSocketRecvEvent );
				return true;
			}
		}
	}

	return false;
}


bool MLocator::InitServerStatusMgr()
{
	if( 0 == m_pServerStatusMgr )
	{
		m_pServerStatusMgr = new MServerStatusMgr;
		if( 0 == m_pServerStatusMgr ) 
			return false;

		if( 0 != GetLocatorDBMgr() )
			GetDBServerStatus( 0, true );
		else
			ASSERT( 0 && "���۽ÿ� DB�� ������ �������°��� ����." );
		
		return true;
	}

	return false;
}

bool MLocator::InitUDPManager()
{
	bool bRet = true;
	m_pRecvUDPManager = new MUDPManager;
	if( (0 == m_pRecvUDPManager) && (bRet) ) bRet = false;
	m_pSendUDPManager = new MUDPManager;
	if( (0 == m_pSendUDPManager) && (bRet) ) bRet = false;
	m_pBlockUDPManager = new MUDPManager;
	if( (0 == m_pBlockUDPManager) && (bRet) ) bRet = false;

	if( !bRet )
	{
		delete m_pRecvUDPManager;
		delete m_pSendUDPManager;
		delete m_pBlockUDPManager;

		m_pRecvUDPManager = 0;
		m_pSendUDPManager = 0;
		m_pBlockUDPManager = 0;

		return false;
	}

	InitMemPool( MLocatorUDPInfo );

	return true;
}


bool MLocator::InitCountryCodeFilter()
{
	ASSERT( 0 != GetLocatorDBMgr() );

	m_pCountryFilter = new MCountryFilter;
	if( 0 == m_pCountryFilter ) {
		mlog( "Fail to new MCountryFilter. \n" );
		return false;
	}

	IPtoCountryList			ipcl;
	BlockCountryCodeList	bcl;
	CustomIPList			cil;

	//if( !GetLocatorDBMgr()->GetIPtoCountryList(ipcl) )
	//{
	//	DeleteCountryFilter();
	//	mlog( "Fail to init IPtoCountryList.\n" );
	//	return false;
	//}
	
	if( !GetLocatorDBMgr()->GetBlockCountryCodeList(bcl) )
	{
		DeleteCountryFilter();
		mlog( "Fail to Init BlockCountryCode.\n" );
		return false;
	}
	
	if( !GetLocatorDBMgr()->GetCustomIPList(cil) )
	{
		DeleteCountryFilter();
		mlog( "Fail to Init CustomIPList.\n" );
		return false;
	}

	const DWORD s = timeGetTime();
	if( !GetCountryFilter()->Create(bcl, ipcl, cil) )
	{
		DeleteCountryFilter();
		mlog( "Fail to Create country filter.\n" );
		return false;
	}
	const DWORD e = timeGetTime();

	const float t = (e - s)/1000.0f;

	return true;
}


void MLocator::Destroy()
{
	// MCommandCommunicator::Destroy();

	ReleaseDBMgr();
	ReleaseSafeUDP();
	ReleaseUDPManager();
	ReleaseValidCountryCodeList();
	ReleaseServerStatusMgr();
	ReleaseServerStatusInfoBlob();
}


void MLocator::ReleaseDBMgr()
{
	if( 0 != m_pDBMgr )
	{
		m_pDBMgr->Disconnect();
		delete m_pDBMgr;
		m_pDBMgr = 0;
	}
}


void MLocator::ReleaseSafeUDP()
{
	if( 0 != m_pSafeUDP )
	{
		m_pSafeUDP->Destroy();
		delete m_pSafeUDP;
		m_pSafeUDP = 0;
	}
}


void MLocator::ReleaseServerStatusMgr()
{
	if( 0 != m_pServerStatusMgr )
	{
		delete m_pServerStatusMgr;
		m_pServerStatusMgr = 0;
	}
}


void MLocator::ReleaseServerStatusInfoBlob()
{
	if( 0 != m_vpServerStatusInfoBlob )
	{
		MEraseBlobArray( m_vpServerStatusInfoBlob );
		m_vpServerStatusInfoBlob = 0;
	}
}


void MLocator::ReleaseUDPManager()
{
	if( 0 != m_pRecvUDPManager )
	{
		m_pRecvUDPManager->SafeDestroy();
		delete m_pRecvUDPManager;
		m_pRecvUDPManager = 0;
	}

	if( 0 != m_pSendUDPManager )
	{
		m_pSendUDPManager->SafeDestroy();
		delete m_pSendUDPManager;
		m_pSendUDPManager = 0;
	}

	if( 0 != m_pBlockUDPManager )
	{
		m_pBlockUDPManager->SafeDestroy();
		delete m_pBlockUDPManager;
		m_pBlockUDPManager = 0;
	}

	ReleaseMemPool( MLocatorUDPInfo );

	UninitMemPool( MLocatorUDPInfo );
}


void MLocator::ReleaseValidCountryCodeList()
{
	if( 0 != m_pCountryFilter )
	{
		delete m_pCountryFilter;
		m_pCountryFilter = 0;
	}
}


void MLocator::ReleaseCommand()
{
	while(MCommand* pCmd = GetCommandSafe())
		delete pCmd;
}


void MLocator::GetDBServerStatus( const DWORD dwEventTime, const bool bIsWithoutDelayUpdate )
{
	// 30�ʸ��� DB�� ServerStatus���̺������� ������.

	if( IsElapedServerStatusUpdatedTime(dwEventTime) || bIsWithoutDelayUpdate )
	{
		if( (0 != GetLocatorDBMgr()) && (0 != m_pServerStatusMgr) )
		{
			m_pServerStatusMgr->Clear();
			if( GetLocatorDBMgr()->GetServerStatus(m_pServerStatusMgr) )
			{
				m_pServerStatusMgr->CheckDeadServerByLastUpdatedTime( GetLocatorConfig()->GetMarginOfErrorMin(), 
																	  m_pServerStatusMgr->CalcuMaxCmpCustomizeMin() );
				/*
				 * ServerStatusInfo Blob�� List���� ����Ǿ��� ���(Blob�� size�� ����)�� �ٽ� �Ҵ��� �ϰ�,
				 *  �� �ܿ��� �Ҵ�� �޸𸮸� �ٽ� �����.
				 */

				if( m_nLastGetServerStatusCount != m_pServerStatusMgr->GetSize() )
				{
					MEraseBlobArray( m_vpServerStatusInfoBlob );

					m_nLastGetServerStatusCount = m_pServerStatusMgr->GetSize();
					m_vpServerStatusInfoBlob	= MMakeBlobArray( MTD_SERVER_STATUS_INFO_SIZE, m_nLastGetServerStatusCount );
					m_nServerStatusInfoBlobSize = MGetBlobArraySize( m_vpServerStatusInfoBlob );
				}
				if( 0 != m_vpServerStatusInfoBlob )
				{
					MTD_ServerStatusInfo* pMTDss;
					for( int i = 0; i < m_nLastGetServerStatusCount; ++i )
					{
						pMTDss = (MTD_ServerStatusInfo*)MGetBlobArrayElement( m_vpServerStatusInfoBlob, i );

						pMTDss->m_dwIP			= (*m_pServerStatusMgr)[i].GetIP();						
						pMTDss->m_nPort			= (*m_pServerStatusMgr)[i].GetPort();
						pMTDss->m_nServerID		= static_cast<unsigned char>( (*m_pServerStatusMgr)[i].GetID() );
						pMTDss->m_nCurPlayer	= (*m_pServerStatusMgr)[i].GetCurPlayer();
						pMTDss->m_nMaxPlayer	= (*m_pServerStatusMgr)[i].GetMaxPlayer();
						pMTDss->m_nType			= (*m_pServerStatusMgr)[i].GetType();
						pMTDss->m_bIsLive		= (*m_pServerStatusMgr)[i].IsLive();
#if defined(LOCALE_NHNUSA)
						pMTDss->m_dwAgentIP		= (*m_pServerStatusMgr)[i].GetAgentIP();
#endif
						strcpy ( pMTDss->m_szServerName	, (*m_pServerStatusMgr)[i].GetServerName().c_str() );

					}

					UpdateLastServerStatusUpdatedTime( dwEventTime );
				}
				else
				{
					m_nLastGetServerStatusCount = -1;
				}
			}
			else
			{
				mlog( "Fail to GetServerStatus\n" );
				ASSERT( 0 && "GetServerStatus����." );
			}
		}
	}
}


bool MLocator::IsElapedServerStatusUpdatedTime( const DWORD dwEventTime )
{
	return (GetLocatorConfig()->GetMaxElapsedUpdateServerStatusTime() < (dwEventTime - GetUpdatedServerStatusTime()));
}


bool MLocator::IskLIveBlockUDP( const MLocatorUDPInfo* pBlkRecvUDPInfo, const DWORD dwEventTime )
{
	if( 0 == pBlkRecvUDPInfo ) return false;

	if( GetLocatorConfig()->GetBlockTime() > (dwEventTime - pBlkRecvUDPInfo->GetUseStartTime()) )
		return true;

	return false;
}


bool MLocator::IsBlocker( const DWORD dwIPKey, const DWORD dwEventTime )
{
	MUDPManager& rfBlkUDPMgr = GetBlockUDPManager();

	rfBlkUDPMgr.Lock();

	MLocatorUDPInfo* pBlkRecvUDPInfo = rfBlkUDPMgr.Find( dwIPKey );
	if( 0 != pBlkRecvUDPInfo )
	{
		// ���� ������ ��� ����ų�� ����.
		if( !IskLIveBlockUDP(pBlkRecvUDPInfo, dwEventTime) )
		{
			rfBlkUDPMgr.Delete( dwIPKey );
			rfBlkUDPMgr.Unlock();
			return false;
		}

		pBlkRecvUDPInfo->IncreaseUseCount();

		// �ٽ� ���ġ�� �ʰ��ϸ� �� �ð��� ������.
		if( IsOverflowedNormalUseCount(pBlkRecvUDPInfo) )
		{
			SYSTEMTIME st;
			// GetSystemTime(&st);
			GetLocalTime( &st );

			/*
			mlog( "IsBlocker - Reset block time. %u.%u.%u %u:%u:%u, dwIP:%u\n", 
				st.wYear, st.wMonth, st.wDay, st.wHour + 9, st.wMinute, st.wSecond, dwIPKey );
				*/
			
			pBlkRecvUDPInfo->SetUseStartTime( dwEventTime );
			pBlkRecvUDPInfo->SetUseCount( 1 );
		}

#ifdef _DEBUG
		mlog( "MLocator::IsBlocker - Block! time. dwIP:%u, UseCount:%d, LimitUseCount:%d, DbgInfo:%s\n",
			dwIPKey, 
			pBlkRecvUDPInfo->GetUseCount(), 
			GetLocatorConfig()->GetMaxFreeUseCountPerLiveTime(), 
			rfBlkUDPMgr.m_strExDbgInfo.c_str() );
#endif

		rfBlkUDPMgr.Unlock();
		return true;
	}

	rfBlkUDPMgr.Unlock();

	return false;
}// IsBlocker


bool MLocator::IsDuplicatedUDP( const DWORD dwIPKey, MUDPManager& rfCheckUDPManager, const DWORD dwEventTime )
{
	rfCheckUDPManager.Lock();

	MLocatorUDPInfo* pRecvUDPInfo = rfCheckUDPManager.Find( dwIPKey );
	if( 0 != pRecvUDPInfo )
	{
		pRecvUDPInfo->IncreaseUseCount();
		rfCheckUDPManager.Unlock();
		return false;
	}

	rfCheckUDPManager.Unlock();

	return true;
}


bool MLocator::UDPSocketRecvEvent( DWORD dwIP, WORD wRawPort, char* pPacket, DWORD dwSize)
{
	if( NULL == GetMainLocator() ) return false;
	if( sizeof(MPacketHeader) > dwSize ) return false;

	// Ư�� �������� ��û�� �����ϰ� ������ ���͸� ��.
	// ���͸��� �����Ǵ� ������Ʈ�� ��ϵǾ� �����ð����� ��û�� ���õ�.

	const DWORD dwRealIP = dwIP;

	const DWORD dwEventTime = timeGetTime(); // Recv�̺�Ʈó���� ����� �ð�.

	MLocator* pLocator = GetMainLocator();
	
	if( pLocator->IsBlocker(dwIP, dwEventTime ) ) 
		return false;
	
#ifdef _LOCATOR_TEST
	if( 400 < GetMainLocator()->GetRecvCount() ) 
		return true;
#endif
	
	if( !pLocator->IsDuplicatedUDP(dwIP, pLocator->GetRecvUDPManager(), dwEventTime) ) 
	{
		GetMainLocator()->IncreaseDuplicatedCount();
		return false;
	}
	
	// ������� ���� ���ο� UDP�̹Ƿ� �߰� �۾��� ��.

	MPacketHeader* pPacketHeader = (MPacketHeader*)pPacket;
	
	if ((dwSize != pPacketHeader->nSize) || 
		((pPacketHeader->nMsg != MSGID_COMMAND) && (pPacketHeader->nMsg != MSGID_RAWCOMMAND)) ) return false;
	
	unsigned int nPort = ntohs(wRawPort);
	GetMainLocator()->ParseUDPPacket( &pPacket[MPACKET_HEADER_SIZE], pPacketHeader, dwIP, nPort );

	GetMainLocator()->IncreaseRecvCount();

	_ASSERT(dwRealIP == dwIP);

	return true;
}// UDPSocketRecvEvent


void MLocator::ParseUDPPacket( char* pData, MPacketHeader* pPacketHeader, DWORD dwIP, unsigned int nPort )
{
	switch (pPacketHeader->nMsg)
	{
	case MSGID_RAWCOMMAND:
		{
			unsigned short nCheckSum = MBuildCheckSum(pPacketHeader, pPacketHeader->nSize);
			if (pPacketHeader->nCheckSum != nCheckSum) {
				static int nLogCount = 0;
				if (nLogCount++ < 100) {	// Log Flooding ����
					mlog("MMatchClient::ParseUDPPacket() -> CHECKSUM ERROR(R=%u/C=%u)\n", 
						pPacketHeader->nCheckSum, nCheckSum);
				}
				return;
			} else {
				MCommand* pCmd = new MCommand();
				if (!pCmd->SetData(pData, &m_CommandManager))
				{
					char szLog[ 128 ] = {0,};
					_snprintf( szLog, 127, "MLocator::ParseUDPPacket -> SetData Error\n" );
					GetLogManager().SafeInsertLog( szLog );
					
					delete pCmd;
					return;
				}

				// ��û�� Ŀ�ǵ尡 Locator���� ó���ϴ� ���� �˻�.
				if( MC_REQUEST_SERVER_LIST_INFO == pCmd->GetID() )
				{
					// ������ �����µ� �ʿ��� DWORD IP�� ť�� ����. - ������� �;� ����� ��.
					if( !GetRecvUDPManager().SafeInsert(dwIP, nPort, timeGetTime()) )
					{
						char szLog[ 1024 ] = {0,};
						_snprintf( szLog, 1023, "fail to insert new IP(%u,%d) Time:%s\n", 
							dwIP, nPort, MGetStrLocalTime().c_str() );
						GetLogManager().SafeInsertLog( szLog );
					}
				}
				else
				{
					ASSERT( 0 && "���� �߰����ǵ� ó��Ŀ�ǵ尡 ����." );

					char szLog[ 1024 ] = {0,};
					_snprintf( szLog, 1023, "invalide command(%u) Time:%s, dwIP:%u\n", 
						pCmd->GetID(), MGetStrLocalTime().c_str(), dwIP );
					GetLogManager().SafeInsertLog( szLog );

					GetBlockUDPManager().SafeInsert( dwIP, nPort, timeGetTime() );
					GetLocatorStatistics().IncreaseBlockCount();
				}

				// ������ ���� �������� ����Ʈ�� ��û�ϴ� Ŀ�ǵ常 ó����.
				// MCommand�� ������� ����.
				delete pCmd;
			}
		}
		break;
	case MSGID_COMMAND:
		{
			ASSERT( 0 && "��ȣȭ ��Ŷ ó���� �ʿ���." );
			char szLog[ 1024 ] = {0,};
			_snprintf( szLog, 1023, "encpypted command. Time:%s, dwIP:%u\n", 
				MGetStrLocalTime().c_str(), dwIP );
			GetLogManager().SafeInsertLog( szLog );

			// ��ӵǴ� IP�α׸� ���ؼ� ������Ʈ�� �߰�.
			GetBlockUDPManager().SafeInsert( dwIP, nPort, timeGetTime() );
			GetLocatorStatistics().IncreaseBlockCount();

			unsigned short nCheckSum = MBuildCheckSum(pPacketHeader, pPacketHeader->nSize);
			if (pPacketHeader->nCheckSum != nCheckSum) {
				static int nLogCount = 0;
				if (nLogCount++ < 100) {	// Log Flooding ����
					mlog("MMatchClient::ParseUDPPacket() -> CHECKSUM ERROR(R=%u/C=%u)\n", 
						pPacketHeader->nCheckSum, nCheckSum);
				}
				return;
			} 
			else {
			}
		}
		break;
	default:
		{
			// MLog("MatchClient: Parse Packet Error");
			ASSERT( 0 && "���ǵ��� ���� Ÿ��." );
		}
		break;
	}
}// MLocator::ParseUDPPacket


void MLocator::Run()
{
	const DWORD dwEventTime = timeGetTime();
	GetDBServerStatus( dwEventTime );
	FlushRecvQueue( dwEventTime );
	UpdateUDPManager( dwEventTime );
	UpdateLocatorStatus( dwEventTime );
	UpdateLocatorLog( dwEventTime );
	UpdateLogManager();
}


void MLocator::ResponseServerStatusInfoList( DWORD dwIP, int nPort )
{
	if( 0 < m_nLastGetServerStatusCount )
	{
		MCommand* pCmd = CreateCommand( MC_RESPONSE_SERVER_LIST_INFO, MUID(0, 0) );
		if( 0 != pCmd )
		{
			// GetDBServerStatus���� ������� ������ ������.
			pCmd->AddParameter( new MCommandParameterBlob(m_vpServerStatusInfoBlob, m_nServerStatusInfoBlobSize) );
			SendCommandByUDP( dwIP, nPort, pCmd );
			delete pCmd;
		}
	}
}


void MLocator::ResponseBlockCountryCodeIP( DWORD dwIP, int nPort, const string& strCountryCode, const string& strRoutingURL )
{
	MCommand* pCmd = CreateCommand( MC_RESPONSE_BLOCK_COUNTRY_CODE_IP, MUID(0, 0) );
	if( 0 != pCmd )
	{
		pCmd->AddParameter( new MCommandParameterString(strCountryCode.c_str()) );
		pCmd->AddParameter( new MCommandParameterString(strRoutingURL.c_str()) );
		SendCommandByUDP( dwIP, nPort, pCmd );
		delete pCmd;
	}
}


bool MLocator::IsLiveUDP( const MLocatorUDPInfo* pRecvUDPInfo, const DWORD dwEventTime )
{
	if( 0 == pRecvUDPInfo ) return false;

	if( GetLocatorConfig()->GetUDPLiveTime() > (dwEventTime - pRecvUDPInfo->GetUseStartTime()) )
		return true;

	return false;
}


bool MLocator::IsOverflowedNormalUseCount( const MLocatorUDPInfo* pRecvUDPInfo )
{
	if( 0 == pRecvUDPInfo ) return false;

	if( GetLocatorConfig()->GetMaxFreeUseCountPerLiveTime() > pRecvUDPInfo->GetTotalUseCount() )
		return false;
	
	return true;
}


const int MLocator::MakeCmdPacket( char* pOutPacket, const int nMaxSize, MCommand* pCmd )
{
	if( (0 == pOutPacket) || (0 > nMaxSize) || (0 == pCmd) ) 
		return -1;

	MCommandMsg* pMsg = reinterpret_cast< MCommandMsg* >( pOutPacket );

	const int nCmdSize = nMaxSize - MPACKET_HEADER_SIZE;

	pMsg->Buffer[ 0 ] = 0;
	pMsg->nCheckSum = 0;

	if( pCmd->m_pCommandDesc->IsFlag(MCCT_NON_ENCRYPTED) )
	{
		pMsg->nMsg = MSGID_RAWCOMMAND;

		const int nGetCmdSize = pCmd->GetData( pMsg->Buffer, nCmdSize );
		if( nGetCmdSize != nCmdSize )
			return -1;

		pMsg->nSize		= static_cast< unsigned int >( MPACKET_HEADER_SIZE ) + nGetCmdSize;
		pMsg->nCheckSum = MBuildCheckSum(pMsg, pMsg->nSize);
	}
	else
	{
		ASSERT( 0 && "��ȣȭ�� Ŀ�ǵ� ó���� ����.\n" );
		return -1;
	}

	return pMsg->nSize;
}


void MLocator::SendCommandByUDP( DWORD dwIP, int nPort, MCommand* pCmd )
{
	const int nPacketSize = CalcPacketSize( pCmd );
	char* pszPacketBuf = new char[ nPacketSize ];
	if( 0 != pszPacketBuf ) 
	{
		const int nMakePacketSize = MakeCmdPacket( pszPacketBuf, nPacketSize, pCmd );
		if( nPacketSize == nMakePacketSize )
		{
			if( !m_pSafeUDP->Send(dwIP, nPort, pszPacketBuf, nMakePacketSize) )
			{
				delete [] pszPacketBuf;
				// mlog( "MLocator::SendCommandByUDP - fail:%u.\n", dwIP );
			}
		}
		else
		{
			delete [] pszPacketBuf;
			ASSERT( 0 && "Packet�� ����µ� ������ ����." );
		}
	}
}


void MLocator::OnRegisterCommand(MCommandManager* pCommandManager)
{
	if( 0 != pCommandManager )
		MAddSharedCommandTable( pCommandManager, 0 );
}


void MLocator::UpdateUDPManager( const DWORD dwEventTime )
{
	if( GetLocatorConfig()->GetUpdateUDPManagerElapsedTime() < (dwEventTime - GetLastUDPManagerUpdateTime()) )
	{
		GetSendUDPManager().SafeClearElapsedLiveTimeUDP( GetLocatorConfig()->GetUpdateUDPManagerElapsedTime(), dwEventTime );
		GetBlockUDPManager().SafeClearElapsedLiveTimeUDP( GetLocatorConfig()->GetBlockTime(), dwEventTime );

		UpdateLastUDPManagerUpdateTime( dwEventTime );
	}
}


void MLocator::UpdateLocatorLog( const DWORD dwEventTime )
{
	if( GetLocatorConfig()->GetElapsedTimeUpdateLocatorLog() < (dwEventTime - GetLocatorStatistics().GetLastUpdatedTime()) )
	{
		GetLocatorDBMgr()->InsertLocatorLog( GetLocatorConfig()->GetLocatorID(),
			GetLocatorStatistics().GetCountryStatistics() );
		if( 0 != GetServerStatusMgr() )
		{
			GetLocatorStatistics().SetDeadServerCount( GetServerStatusMgr()->GetDeadServerCount() );
			GetLocatorStatistics().SetLiveServerCount( GetServerStatusMgr()->GetLiveServerCount() );
		}
		else
		{
			GetLocatorStatistics().SetDeadServerCount( 0 );
			GetLocatorStatistics().SetLiveServerCount( 0 );
		}
		GetLocatorStatistics().SetLastUpdatedTime( dwEventTime );
		GetLocatorStatistics().Reset();
	}
}


void MLocator::UpdateCountryCodeFilter( const DWORD dwEventTime )
{
	// ����غ�����.
	//if( 0 == GetLocatorDBMgr() ) return;

	//if( GetLocatorConfig()->GetElapsedTimeUpdateCountryCodeFilter() <
	//	(dwEventTime - GetCountryCodeFilter()->GetLastUpdatedTime()) )
	//{
	//	BlockCountryCodeInfoList	bcci;
	//	IPtoCountryList				icl;

	//	if( !GetLocatorDBMgr()->GetBlockCountryCodeList(&bcci) )
	//	{
	//		mlog( "Fail to GetDBBlockCountryCodeList.\n" );
	//		return;
	//	}

	//	if( !GetLocatorDBMgr()->GetIPtoCountryList(&icl) )
	//	{
	//		mlog( "Fail to GetDBIPtoCountryList.\n" );
	//		return;
	//	}

	//	IPtoCountryList::iterator it, end;
	//	end = icl.end();
	//	for( it = icl.begin(); it != end; ++it )
	//		GetLocatorStatistics().InitInsertCountryCode( it->strCountryCode3 ); // ������ ���� �õ�ī��Ʈ�� ���ؼ�, �� �����ڵ带 �����.
	//	
	//	if( !GetCountryCodeFilter()->Update( bcci, icl) )
	//		mlog( "fail to update country code filter.\n" );

	//	GetCountryCodeFilter()->SetLastUpdatedTime( dwEventTime );
	//}
}


void MLocator::UpdateLogManager()
{
	// �ٸ� �����忡�� ����� �α׸� �����.
	GetLogManager().SafeWriteMLog();
	GetLogManager().SafeReset();
}


void MLocator::FlushRecvQueue( const DWORD dwEventTime )
{
	MUDPManager& RecvUDPMgr = GetRecvUDPManager();
	MUDPManager& SendUDPMgr = GetSendUDPManager();

	MLocatorUDPInfo* pRecvUDPInfo;
	MLocatorUDPInfo* pSendUDPInfo;

	string	strCountryCode;
	string	strRoutingURL;
	bool	bIsBlock;
	string	strComment;

    RecvUDPMgr.Lock();
	while( 0 != (pRecvUDPInfo = RecvUDPMgr.SafePopFirst()) )
	{
		if( !SendUDPMgr.Insert(pRecvUDPInfo->GetIP(), pRecvUDPInfo) )
		{
			pSendUDPInfo = SendUDPMgr.Find( pRecvUDPInfo->GetIP() );
			if( 0 != pSendUDPInfo )
				pSendUDPInfo->IncreaseUseCount( pRecvUDPInfo->GetUseCount() );
			delete pRecvUDPInfo;
		}
	}
	RecvUDPMgr.Unlock();

	for( SendUDPMgr.SetBegin(); 0 != (pSendUDPInfo = SendUDPMgr.GetCurPosUDP()); )
	{
		if( 0 < pSendUDPInfo->GetUseCount() )
		{
			if( IsOverflowedNormalUseCount(pSendUDPInfo) )
			{
				// SendQ�� �����ִ� BlockUDP�� ��üUDPManager������Ʈ���� ���� �ð��� ������ �ڵ����� ������.
				// SendUDPMgr.PopByIPKey( pSendUDPInfo->GetIP() );

				if( !IsBlocker(pSendUDPInfo->GetIP(), dwEventTime) )
				{
					pSendUDPInfo->SetUseCount( 0 );

					// ���ο� BlockUDP�� ������ BlockQ�� �����.
					if( !GetBlockUDPManager().SafeInsert(pSendUDPInfo->GetIP(), pSendUDPInfo->GetPort(), dwEventTime) )
					{
						mlog( "fail to block udp(%s) time:%s\n", 
							pSendUDPInfo->GetStrIP().c_str(), MGetStrLocalTime().c_str() );
					}

					GetLocatorStatistics().IncreaseBlockCount();
					mlog( "Block. IP(%s), time:%s\n", pSendUDPInfo->GetStrIP().c_str(), MGetStrLocalTime().c_str() );
				}
				SendUDPMgr.MoveNext();
				continue;
			}

			// ���� �ڵ� ���͸� ����ҽ�, ������ IP�� ���� ������ �������� �˻�.
			if( GetLocatorConfig()->IsUseCountryCodeFilter() )
			{
				// custom ip�˻��� ip country code�˻�.
				if( GetCustomIP(pSendUDPInfo->GetStrIP(), strCountryCode, bIsBlock, strComment) )
				{
					if( !bIsBlock )
					{
						ResponseServerStatusInfoList( pSendUDPInfo->GetIP(), pSendUDPInfo->GetPort() );
					}
					else
					{
						ResponseBlockCountryCodeIP( pSendUDPInfo->GetIP(), 
							pSendUDPInfo->GetPort(), 
							strCountryCode, strComment );
					}
				}
				else if( IsValidCountryCodeIP(pSendUDPInfo->GetStrIP(), strCountryCode, strRoutingURL) )
				{
					ResponseServerStatusInfoList( pSendUDPInfo->GetIP(), pSendUDPInfo->GetPort() );
					GetLocatorStatistics().IncreaseCountryStatistics( strCountryCode );
				}
				else
				{
					ResponseBlockCountryCodeIP( pSendUDPInfo->GetIP(), 
						pSendUDPInfo->GetPort(), 
						strCountryCode, strRoutingURL );

					GetLocatorStatistics().IncreaseBlockCountryCodeHitCount();
					GetLocatorStatistics().IncreaseCountryStatistics( strCountryCode, -1 );
				}
			}
			else
			{
				string CountryCode3;

				ResponseServerStatusInfoList( pSendUDPInfo->GetIP(), pSendUDPInfo->GetPort() );

				GetCountryFilter()->GetIPCountryCode( pSendUDPInfo->GetStrIP(), CountryCode3 );
				GetLocatorStatistics().IncreaseCountryStatistics( CountryCode3 );
			}

			pSendUDPInfo->IncreaseUsedCount( pSendUDPInfo->GetUseCount() );
			pSendUDPInfo->SetUseCount( 0 );
			IncreaseSendCount();

			
		}
		SendUDPMgr.MoveNext();
	}
}


bool MLocator::GetCustomIP( const string& strIP, string& strOutCountryCode, bool& bIsBlock, string& strOutComment )
{
	if( (0 == GetLocatorDBMgr()) || (0 == GetCountryFilter()) )
		return false;

	// CustomIP���̺� �����ϴ��� �˻���.

	if( !GetCountryFilter()->GetCustomIP(strIP, bIsBlock, strOutCountryCode, strOutComment) )
	{
		// �̺κп��� custom ip���̺��� ��ȸ�ؼ� ������Ʈ �ϱ⿡�� DB�� �ʹ� ���� �о�� �ؼ�
		// custom ip�� ���⼭ ������Ʈ���� �ʰ�, CountryCodeIP�˻� �κп��� ������Ʈ ��.
		return false;
	}

	return true;
}


bool MLocator::IsValidCountryCodeIP( const string& strIP, string& strOutCountryCode, string& strOutRoutingURL )
{
	if( (0 == GetLocatorDBMgr()) || (0 == GetCountryFilter()) )
		return false;

#ifdef _LOCATOR_TEST
	DWORD	dwStart;
	DWORD	dwEnd;
	bool	bIsWriteLog = false;

	dwStart =timeGetTime();
#endif

	// MCountryCodeFilter�� �ִ��� �˻��� ������ DB���� �˻�.
	// DB���� �˻��� ������ Filter�� �߰�.
	// �߰��� Key�� ��ġ���� �˻��ؾ� ��.
	if( !GetCountryFilter()->GetIPCountryCode(strIP, strOutCountryCode) )
	{
		// ���Ⱑ�� �������� ���� locator���� �������� �ʴ� ip�̱⿡ CustomIP���̺�� IPtoCountry���̺��� ��ȸ�ؼ� 
		// ã������� ������Ʈ �Ѵ�.
		
		DWORD	dwIPFrom;
		DWORD	dwIPTo;
		bool	bIsBlock;
		string	strCountryCode;
		string	strComment;

		if( GetLocatorDBMgr()->GetCustomIP(strIP, dwIPFrom, dwIPTo, bIsBlock, strCountryCode, strComment) )
		{
			if( GetCountryFilter()->AddCustomIP(dwIPFrom, dwIPTo, bIsBlock, strCountryCode, strComment) )
			{
#ifdef _LOCATOR_TEST
				mlog( "Add new custom ip(%s) from(%u), to(%u), IsBlock(%d), code(%s), comment(%s)\n",
					strIP.c_str(), dwIPFrom, dwIPTo, bIsBlock, strCountryCode.c_str(), strComment.c_str() );
				bIsWriteLog = true;
#endif
			}
			else
				mlog( "Fail to add new custom ip(%s) CountryCode(%s)\n", strIP.c_str(), strCountryCode.c_str() );
		}
		else if( GetLocatorDBMgr()->GetIPContryCode(strIP, dwIPFrom, dwIPTo, strCountryCode) )
		{
			if( GetCountryFilter()->AddIPtoCountry(dwIPFrom, dwIPTo, strCountryCode) )
			{
#ifdef _LOCATOR_TEST
				mlog( "Add new country code ip(%s), dwIPFrom(%u), dwIPTo(%u), code(%s).\n",
					strIP.c_str(), dwIPFrom, dwIPTo, strCountryCode.c_str() );
				bIsWriteLog = true;
#endif
			}
			else
				mlog( "MLocator::IsValidCountryCodeIP - DB���� �о�� ���ο� IPCountryCode������ Filter�� ����ϴµ� ����.\n" );
		}
		else
		{
			GetLocatorStatistics().IncreaseInvalidIPCount();

			return GetLocatorConfig()->IsAcceptInvalidIP();
		}

		strOutCountryCode = strCountryCode;

		GetLocatorStatistics().IncreaseCountryCodeCacheHitMissCount();
	}

#ifdef _LOCATOR_TEST
	dwEnd = timeGetTime();

	const float fInsertTime = (dwEnd - dwStart) / 1000.0f;

	if( bIsWriteLog )
	{
		const IPtoCountryList&	icl = GetCountryFilter()->GetIPtoCountryList();
		const CustomIPList&		cil = GetCountryFilter()->GetCustomIPList();

		mlog( "IPtoCountrySize(%u), CustomIPSize(%u), UpdateElapsedTime(%f) \n",
			icl.size(), cil.size(), fInsertTime );
	}
#endif

	return GetCountryFilter()->IsNotBlockCode( strOutCountryCode, strOutRoutingURL );
}


MCommand* MLocator::CreateCommand(int nCmdID, const MUID& TargetUID)
{
	return new MCommand(m_CommandManager.GetCommandDescByID(nCmdID), TargetUID, m_This);
}


void MLocator::DumpLocatorStatusInfo()
{
	mlog( "\n======================================================\n" );
	mlog( "Locator Status Info.\n" );

	mlog( "Recv UDP Manager Status Info\n" );
	GetRecvUDPManager().Lock();
	GetRecvUDPManager().DumpStatusInfo();
	GetRecvUDPManager().Unlock();

	mlog( "Send UDP Manager Status Info\n" );
	GetSendUDPManager().Lock();
	GetSendUDPManager().DumpStatusInfo();
	GetSendUDPManager().Unlock();

	mlog( "Block UDP Manager Status Info\n" );
	GetBlockUDPManager().Lock();
	GetBlockUDPManager().DumpStatusInfo();
	GetBlockUDPManager().DumpUDPInfo();
	GetBlockUDPManager().Unlock();
	mlog( "======================================================\n\n" );
}


void MLocator::UpdateLocatorStatus( const DWORD dwEventTime )
{
	if( GetLocatorConfig()->GetMaxElapsedUpdateServerStatusTime() < 
		(dwEventTime - GetLastLocatorStatusUpdatedTime()) )
	{
		if( 0 == m_pDBMgr )
			return;

		if( !m_pDBMgr->UpdateLocaterStatus( GetLocatorConfig()->GetLocatorID(), 
			GetRecvCount(), 
			GetSendCount(), 
			static_cast<DWORD>(GetBlockUDPManager().size()), 
			GetDuplicatedCount() ) )
		{
			mlog( "fail to update locator status.\n" );
		}

		ResetRecvCount();
		ResetSendCount();
		ResetDuplicatedCount();

		UpdateLastLocatorStatusUpdatedTime( dwEventTime );
	}
}


void MLocator::DeleteCountryFilter()
{
	if( 0 != m_pCountryFilter )
	{
		delete m_pCountryFilter;
		m_pCountryFilter = 0;
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG

// �ʱ�ȭ�� �����ϴ��� ���࿡�� ������ ����� ��.
void MLocator::InitDebug()
{
	GetRecvUDPManager().m_strExDbgInfo = "Name:RecvUDPManager";
	GetSendUDPManager().m_strExDbgInfo = "Name:SendUDPManager";
	GetBlockUDPManager().m_strExDbgInfo = "Name:BlockUDPManager";
}


void MLocator::TestDo()
{
	if( 0 != m_pDBMgr )
	{
		if( IsElapedServerStatusUpdatedTime(timeGetTime()) )
		{
			/*
			ServerStatusVec ss;
			if( !m_pDBMgr->GetServerStatus(ss) )
			{
				ASSERT( 0 );
			}

			for( int i = 0; i < ss.size(); ++i )
			{
				OutputDebugString( ss[i].GetDebugString().c_str() );
			}
			
			UpdateLastServerStatusUpdatedTime( timeGetTime() );
			*/
		}
	}
}

void MLocator::DebugOutput( void* vp )
{
	return; 

	MServerStatusMgr ssm = *( reinterpret_cast<MServerStatusMgr*>(vp) );

	int nSize = ssm.GetSize();

	char szBuf[ 1024 ];

	OutputDebugString( "\nStart Debug Output-------------------------------------------------\n" );

	for( int i = 0; i < nSize; ++i )
	{
		_snprintf( szBuf, 1023, "dwIP:%u, Port:%d, ServerID:%d, Time:%s, Live:%d (%d/%d)\n",
			ssm[i].GetIP(), ssm[i].GetPort(), ssm[i].GetID(), ssm[i].GetLastUpdatedTime().c_str(), 
			ssm[i].IsLive(), 
			ssm[i].GetCurPlayer(), ssm[i].GetMaxPlayer() );

		OutputDebugString( szBuf );

		char szVal[ 3 ];
		strncpy( szVal, &ssm[i].GetLastUpdatedTime()[11], 2 );
		const int nHour = atoi( szVal );
		strncpy( szVal, &ssm[i].GetLastUpdatedTime()[14], 2 );
		const int nMin = atoi( szVal );

		_snprintf( szBuf, 1023, "%d:%d\n", nHour, nMin );

		OutputDebugString( szBuf );
	}

	OutputDebugString( "End Debug Output---------------------------------------------------\n\n" );
}
#endif
