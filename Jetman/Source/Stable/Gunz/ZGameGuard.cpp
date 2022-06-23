#include "stdafx.h"
#include "ZGameguard.h"
#include "MDebug.h"
#include "ZPost.h"




#ifdef _DEBUG
// #define NO_GAMEGUARD // 게임가드를 사용하지 않고 디버깅하려면 선언을 해줘야 함. - by sunge
#endif


#include ".\\GameGuard\\NPGameLib.h"


#pragma comment( lib, "ws2_32.lib" )
#pragma comment( lib, "winmm.lib" )
#ifdef LOCALE_JAPAN
#pragma comment( lib, ".\\GameGuard\\JPN\\NPGameLib.lib" )
#else
#pragma comment( lib, ".\\GameGuard\\NPGameLib.Lib" )
#endif



BOOL CALLBACK NPGameMonCallback( DWORD dwMsg, DWORD dwArg )
{
	LPCTSTR lpszMsg = NULL;

	char msg[ MAX_CALLBACKMSG_LEN ] = {0,};

	switch (dwMsg)
	{
	case NPGAMEMON_CHECK_CSAUTH2 :
		{
			GG_AUTH_DATA AuthData;
			memcpy( &AuthData, (void*)dwArg, sizeof(GG_AUTH_DATA) );

			if( ZGameguard::m_IsResponseFirstGameguardAuth )
			{
				ZPsotResponseGameguardAuht( AuthData.dwIndex, AuthData.dwValue1, AuthData.dwValue2, AuthData.dwValue3 );
				// mlog( "Recv ggnp default ok.\n" );
			}
			else
			{
				ZPostResponseFirstGameguardAuth( AuthData.dwIndex, AuthData.dwValue1, AuthData.dwValue2, AuthData.dwValue3 );
				ZGameguard::m_IsResponseFirstGameguardAuth = true;
				// mlog( "Recv ggnp step1 ok.\n" );
			}
		}
		break;

	case NPGAMEMON_COMM_ERROR:
	case NPGAMEMON_COMM_CLOSE:
		return false;

	case NPGAMEMON_INIT_ERROR:
		GetZGameguard().SetExitGame( true );
		wsprintf(msg, "GameGuard initialization error : %lu", dwArg);
		break;

	case NPGAMEMON_SPEEDHACK:
		GetZGameguard().SetExitGame( true );
		wsprintf(msg, "Speedhack detected : %lu", dwArg);
		break;

	case NPGAMEMON_GAMEHACK_KILLED:
		// bAppExit = true; // termination code, close or proceed continuously
		wsprintf(msg, "Gamehack detected : %lu", dwArg);
		break;

	case NPGAMEMON_GAMEHACK_DETECT:
		GetZGameguard().SetExitGame( true );
		wsprintf(msg, "Gamehack detected : %lu", dwArg);
		break;

	case NPGAMEMON_GAMEHACK_DOUBT:
		GetZGameguard().SetExitGame( true );
		wsprintf(msg, "game or GameGuard file corrupted : %lu", dwArg);
		break;
	}

	GetZGameguard().SetCallbackMsg( msg );

	// return ‘false’ only after the game is closed.
	if( GetZGameguard().IsExitGame() )
		return false;
	
	return true;
}

//---------------------------------------------------------------------------------------------------

bool ZGameguard::m_IsResponseFirstGameguardAuth = false;


ZGameguard& GetZGameguard()
{
	return ZGameguard::GetInstance();
}



ZGameguard::ZGameguard()
{
	m_pNPGameLib		= 0;
	m_dwLastCheckTime	= 0;
	m_dwLastHookCheckTime = 0;
	m_bIsExitGame		= false;
}


ZGameguard::~ZGameguard()
{
	Release();
}


bool ZGameguard::Init( const ZGMAEGUARD_MODE GameguardMode, const char* szArg )
{	
	if( ZGGM_ALPHA == GameguardMode )
	{
//        m_pNPGameLib = new CNPGameLib( "GunzUSTest" );
        m_pNPGameLib = new CNPGameLib( szArg);

#ifdef _DEBUG
		mlog( "gameguard load '%s.ini'\n", szArg );
#endif
	}
	else if( ZGGM_REAL == GameguardMode )
	{
//		m_pNPGameLib = new CNPGameLib( "GunzUS" );
		m_pNPGameLib = new CNPGameLib( szArg );

#ifdef _DEBUG
		mlog( "gameguard load '%s.ini'\n", szArg );
#endif
	}
	else
	{
		mlog( "gameguard mode error...\n" );
		ASSERT( 0 );
		return false;
	}

	if( 0 == m_pNPGameLib ) 
		return false;

	const DWORD dwRetGameGuardInit = m_pNPGameLib->Init();
		if( NPGAMEMON_SUCCESS != dwRetGameGuardInit )
	{
		// 초기화 실패 처리.

		InitGameGuardFail( dwRetGameGuardInit );

		return false;
	}

	return true;
}


void ZGameguard::Release()
{
	if( 0 != m_pNPGameLib )
	{
		mlog( "delete gameguard instance...'\n");
		delete m_pNPGameLib;
		m_pNPGameLib = 0;
		mlog( "delete gameguard instance done.\n");
	}
}


void ZGameguard::SetMainWnd( const HWND hMainWnd )
{
	m_pNPGameLib->SetHwnd( hMainWnd );
}


bool ZGameguard::CheckGameGuardRunning( const DWORD dwGlobalTime )
{
#define MAX_ELAPSED_TIME (1000 * 5) // 5초에 한번씩.

	if( MAX_ELAPSED_TIME < (dwGlobalTime - m_dwLastCheckTime) )
	{
#ifdef _DEBUG
		mlog( "Check GameGuard running...\n");
#endif

		DWORD dwRetVal = m_pNPGameLib->Check();
		if( NPGAMEMON_SUCCESS != dwRetVal)
		{
			mlog( "ERROR : GameGuard is not running (errcode = %u)\n", dwRetVal);

			return false;	// exit game.
		}

#ifdef _DEBUG
		mlog( "  >> Return code = %u\n", dwRetVal);
#endif

		m_dwLastCheckTime = dwGlobalTime;
	}

	return true;
}

bool ZGameguard::CheckD3dDllHooking(DWORD dwGlobalTime)
{
#ifdef LOCALE_NHNUSA
	// 1분 주기로 d3d9.dll이 후킹되었는지 검사한다
	const int D3DDLLHOOK_CHECK_TERM = 1000 * 60;
	if (D3DDLLHOOK_CHECK_TERM < (dwGlobalTime - m_dwLastHookCheckTime))
	{
		if (/*m_pNPGameLib->*/CheckD3DDevice((LPVOID)RGetDevice(), "d3d9.dll"))
		{
			// d3d9 변조됨!
			mlog( "ERROR : GameGuard dll check.\n");
			return false;
		}

		m_dwLastHookCheckTime = dwGlobalTime;
	}
#endif

	// 변조 없음
	return true;
}


bool ZGameguard::CheckHackToolUser( const char* szUserID )
{
	if( (0 == szUserID) || (1 > strlen(szUserID)) ) 
		return false;

	if( 1 != m_pNPGameLib->Send(szUserID) )
		return false;

	return true;
}


const char* ZGameguard::GetHackToolInfo()
{
	return m_pNPGameLib->GetInfo();
}


void ZGameguard::SetCallbackMsg( const char* szCallbackMsg )
{
	memset( m_szCallbackMsg, 0, MAX_CALLBACKMSG_LEN );

	if( (0 == szCallbackMsg) || (MAX_CALLBACKMSG_LEN < strlen(szCallbackMsg)) )
	{
		mlog( "GameGuard callback message is overflow (length=%u)\n", strlen(szCallbackMsg));

		return;
	}

	strncpy( m_szCallbackMsg, szCallbackMsg, strlen(szCallbackMsg) );


#ifdef _DEBUG
	mlog( "Set GameGuard callback message... (%s)\n", m_szCallbackMsg);
#endif
}


void ZGameguard::InitGameGuardFail( const DWORD dwResult )
{
	TCHAR msg[256];
	LPCSTR lpszMsg;

	// Display relevant messages referring to the ‘6. Main error code’.
	switch (dwResult)
	{
	case NPGAMEMON_ERROR_EXIST:
		lpszMsg = "GameGuard is already running. Try rebooting first and executing the game again."; 
		break;
	case NPGAMEMON_ERROR_GAME_EXIST:
		lpszMsg = "There are multiple events of game execution or GameGuard is already running. Close the game then try again.";
		break;
	case NPGAMEMON_ERROR_INIT:
		lpszMsg = "GameGuard initialization error. Try rebooting and executing the game or close the program considered to cause a collision.";
		break;
	case NPGAMEMON_ERROR_AUTH_GAMEGUARD:
	case NPGAMEMON_ERROR_NFOUND_GG:
	case NPGAMEMON_ERROR_AUTH_INI:
	case NPGAMEMON_ERROR_NFOUND_INI:
		lpszMsg = "GameGuard file does not exist or is corrupted. Please install the GameGuard setup file.";
		break;
	case NPGAMEMON_ERROR_CRYPTOAPI:
		lpszMsg = "Window system files might be corrupted. Please reinstall the Internet Explorer(IE).";
		break;
	case NPGAMEMON_ERROR_EXECUTE:
		lpszMsg = "Fail to run GameGuard. Please reinstall the GameGuard setup file.";
		break;
	case NPGAMEMON_ERROR_ILLEGAL_PRG:
		lpszMsg = "Illegal program detected. Close all the unnecessary programs before running the game.";
		break;
	case NPGMUP_ERROR_ABORT:
		lpszMsg = "GameGuard update has been aborted. Please check the status of Internet network or personal firewall settings when unable to connect continuously.";
		break;
	case NPGMUP_ERROR_CONNECT:
	case NPGMUP_ERROR_DOWNCFG:
		lpszMsg = "Fail to connect the GameGuard update server. Please try again after a while, or check personal Firewall settings if any.";
		break;
	case NPGMUP_ERROR_AUTH:
		lpszMsg = "Fail to complete GameGuard update. Suspend Anti-Virus program temporarily and try the game, or check the settings of PC management programs if any.";
		break;
	case NPGAMEMON_ERROR_NPSCAN:
		lpszMsg = "Failed to load virus and hacking tool scanning module. Possibly due to lack of memory or virus infection";
		break;
	default:
		// Display relevant messages.
		lpszMsg = " Error occurred while running GameGuard. Please send all *.erl files in GameGuard folder to Game2@inca.co.kr via email.";
			break;
	}
	wsprintf(msg, "GameGuard execute error : %lu\n", dwResult);
	mlog( msg );
	MessageBox(NULL, lpszMsg, msg, MB_OK);
}


void ZGameguard::SendToCallback( const DWORD dwIndex, const DWORD dwValue1, const DWORD dwValue2, const DWORD dwValue3 )
{
	GG_AUTH_DATA AuthData;
	AuthData.dwIndex = dwIndex;
	AuthData.dwValue1 = dwValue1;
	AuthData.dwValue2 = dwValue2;
	AuthData.dwValue3 = dwValue3;

	m_pNPGameLib->Auth2( &AuthData );


#ifdef _DEBUG
	mlog( "Send to GameGuard callback message... (index=%u  value1=%u  value2=%u  value3=%u)\n",  dwIndex, dwValue1, dwValue2, dwValue3);
#endif
}


//---------------------------------------------------------------------------------------------------


