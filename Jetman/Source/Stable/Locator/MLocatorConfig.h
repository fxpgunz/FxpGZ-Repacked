#pragma once

#include "MMatchTransDataType.h"


#ifdef _DEBUG
	#define LOCATOR_CONFIG "./DbgLocator.ini"
#else 
	#define LOCATOR_CONFIG "./Locator.ini"
#endif


class MLocatorConfig
{
public:
	virtual ~MLocatorConfig(void);

	bool LoadConfig();

	// DB
	const CString& GetDBDSN() const			{ return m_strDBDSN; }
	const CString& GetDBUserName() const	{ return m_strDBUserName; }
	const CString& GetDBPassword() const	{ return m_strDBPassword; }

	// Network
	const string& GetLocatorIP()	const { return m_strLocatorIP; }
	const int GetLocatorPort()		const { return m_nLocatorPort; }

	// Environment
	const DWORD			GetLocatorID() const							{ return m_dwID; }
	inline const MUID&	GetLocatorUID() const							{ return m_uidLocatorUID; }
	inline const DWORD	GetMaxElapsedUpdateServerStatusTime() const		{ return m_dwMaxElapsedUpdateServerStatusTime; }
	inline const DWORD	GetUDPLiveTime() const							{ return m_dwUDPLiveTime; }
	inline const DWORD	GetMaxFreeUseCountPerLiveTime() const			{ return m_dwMaxFreeUseCountPerLiveTime; }
	inline const DWORD	GetBlockTime() const							{ return m_dwBlockTime; }
	inline const DWORD	GetUpdateUDPManagerElapsedTime() const			{ return m_dwUpdateUDPManagerElapsedTime; }
	inline const DWORD	GetMarginOfErrorMin() const						{ return m_dwMarginOfErrorMin; }
	inline const DWORD	GetElapsedTimeUpdateLocatorLog() const			{ return m_dwElapsedTimeUpdateLocatorLog; }

	inline const bool IsUseCountryCodeFilter() const	{ return m_bIsUseCountryCodeFilter; }
	inline const bool IsInitCompleted() const			{ return m_bIsInitCompleted; }
	inline const bool IsAcceptInvalidIP() const			{ return m_bIsAcceptInvaildIP; }
	inline const bool IsTestServerOnly() const			{ return m_bIsTestServerOnly; }

	static MLocatorConfig* GetLocatorConfigInst()
	{
		static MLocatorConfig LocatorConfig;
		return &LocatorConfig;
	}

private :
	MLocatorConfig(void);

	bool LoadDBConfig();
	bool LoadNetConfig();
	bool LoadEnvConfig();

	// DB
	bool SetDBDSN( const char* pszDSN );
	bool SetDBUserName( const char* pszUserName );
	bool SetDBPassword( const char* pszPassword );

	// Network
	void SetLocatorIP( const string& strLocatorIP )		{ m_strLocatorIP = strLocatorIP; }
	void SetLocatorPort( const int nPort )				{ m_nLocatorPort = nPort; }

	// Environment
	void SetLocatorID( const DWORD dwID )								{ m_dwID = dwID; }
	void SetLocatorUID( const MUID& uid )								{ m_uidLocatorUID = uid; }
	void SetMaxElapsedUpdateServerSTatusTime( const DWORD dwTime )		{ m_dwMaxElapsedUpdateServerStatusTime = dwTime; }
	void SetUDPLiveTime( const DWORD dwLiveTime )						{ m_dwUDPLiveTime = dwLiveTime; }
	void SetMaxFreeUseCountPerLiveTime( const DWORD dwCount )			{ m_dwMaxFreeUseCountPerLiveTime = dwCount; }
	void SetBlockTime( const DWORD dwBlockTime )						{ m_dwBlockTime = dwBlockTime; }
	void SetUpdateUDPManagerElapsedTime( const DWORD dwElapsedTime )	{ m_dwUpdateUDPManagerElapsedTime = dwElapsedTime; }
	void SetMarginOfErrorMin( const DWORD dwMin )						{ m_dwMarginOfErrorMin = dwMin; }
	void SetCountryCodeFilterStatus( const bool bIsUse )				{ m_bIsUseCountryCodeFilter = bIsUse; }
	void SetElapsedTimeUpdateLocaorLog( const DWORD dwTime )			{ m_dwElapsedTimeUpdateLocatorLog = dwTime; }
	void SetAcceptInvalidIP( const bool IsAccept )						{ m_bIsAcceptInvaildIP = IsAccept; }
	void SetTestServerOnly(const bool IsTestServerOnly)					{ m_bIsTestServerOnly = IsTestServerOnly; }
	
private :
	// Network
	string	m_strLocatorIP;
	int		m_nLocatorPort;

	// Evironment
	DWORD	m_dwID;									// DB���� Locator�� �����ϴ� ID.
	MUID	m_uidLocatorUID;
	DWORD	m_dwMaxElapsedUpdateServerStatusTime;	// /ms ������ ������ DB���� �������� ����.
	DWORD	m_dwUDPLiveTime;						// /ms UDPť�� ����������� �ִ� �ð�.
	DWORD	m_dwMaxFreeUseCountPerLiveTime;			// ť�� ����� �ִµ��� ������ �ִ� �ִ� ��. �Ѿ�� �����ڷ� ���.
	DWORD	m_dwBlockTime;							// /ms Block����Ʈ�� ��ϵ��ִ� �ð�.
	DWORD	m_dwUpdateUDPManagerElapsedTime;		// UDPť ������Ʈ ����. Recv,Send,Blockť�� ��� ���� ������Ʈ ��.
	DWORD	m_dwMarginOfErrorMin;					// �������� ������ ������Ʈ �ð��� ���� ū���� ���Ͽ� 
													//  �̰��̻� ���̰� ���� ���������� ����.
	DWORD	m_dwGMTDiff;
	bool	m_bIsUseCountryCodeFilter;				// �����ڵ� ���͸� ������� ����.
	DWORD	m_dwElapsedTimeUpdateLocatorLog;

	bool	m_bIsAcceptInvaildIP;
	bool	m_bIsTestServerOnly;					// �׽�Ʈ������ Locating
	
	// DB
	CString m_strDBDSN;
	CString m_strDBUserName;
	CString m_strDBPassword;

	bool	m_bIsInitCompleted;
};


MLocatorConfig* GetLocatorConfig();