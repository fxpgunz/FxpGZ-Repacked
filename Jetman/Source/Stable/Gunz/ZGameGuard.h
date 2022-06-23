#ifndef _ZGameguard
#define _ZGameguard


class CNPGameLib;


#define MAX_CALLBACKMSG_LEN 256



enum ZGMAEGUARD_MODE
{
	ZGGM_ALPHA = 1,
	ZGGM_REAL,

	ZGGM_END,
};



class ZGameguard
{
public :
	~ZGameguard();


	bool Init( const ZGMAEGUARD_MODE GameguardMode, const char* szArg );
	void Release();

	const char* GetHackToolInfo();
	void SetMainWnd( const HWND hMainWnd );
	void SetCallbackMsg( const char* szCallbackMsg );
	void SetExitGame( const bool isExitGame )				{ m_bIsExitGame = isExitGame; }

	bool CheckGameGuardRunning( const DWORD dwGlobalTime );	
	bool CheckD3dDllHooking(DWORD dwGlobalTime);

	bool CheckHackToolUser( const char* szUserID );
	bool IsExitGame()										{ return m_bIsExitGame; }

	void SendToCallback( const DWORD dwIndex, const DWORD dwValue1, const DWORD dwValue2, const DWORD dwValue3 );


	static ZGameguard& GetInstance()
	{
		static ZGameguard gg;
		return gg;
	}


private :
	ZGameguard();

	void InitGameGuardFail( const DWORD dwResult );
public :
	static bool m_IsResponseFirstGameguardAuth;

private :
	CNPGameLib*		m_pNPGameLib;
	DWORD			m_dwLastCheckTime;
	DWORD			m_dwLastHookCheckTime;
	char			m_szCallbackMsg[ MAX_CALLBACKMSG_LEN ];
	bool			m_bIsExitGame;
};


ZGameguard& GetZGameguard();


#endif