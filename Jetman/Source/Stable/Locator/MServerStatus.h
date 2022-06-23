#ifndef _MSERVER_STATUS
#define _MSERVER_STATUS


#include <vector>
#include <string>
#include <utility>

using std::vector;
using std::string;
using std::pair;


class MServerStatus
{
public :
	MServerStatus() : m_nID( 0 ), m_nMaxPlayer( 0 ), m_nCurPlayer( 0 ), m_dwIP( 0 ), 
		m_nPort( 0 ), m_bIsOpened( false ), m_nType( 0 ), m_bIsLive( false )
	{}

	int				GetID() const				{ return m_nID; }
	int				GetMaxPlayer() const		{ return m_nMaxPlayer; }
	int				GetCurPlayer() const		{ return m_nCurPlayer; }
	const string&	GetLastUpdatedTime() const	{ return m_strLastUpdatedTime; }
	const string&	GetIPString() const			{ return m_strIP; }
	DWORD 			GetIP() const				{ return m_dwIP; }
	const string&	GetAgentIPString() const	{ return m_strAgentIP; }
	DWORD 			GetAgentIP() const			{ return m_dwAgentIP; }
	int				GetPort() const				{ return m_nPort; }
	const string&	GetServerName() const		{ return m_strServerName; }
	bool			IsOpened() const			{ return m_bIsOpened; }
	int				GetType() const				{ return m_nType; }
	bool			IsLive() const				{ return m_bIsLive; }

	void SetID( const int nID )									{ m_nID = nID; }
	void SetMaxPalyer( const int nMaxPlayer )					{ m_nMaxPlayer = nMaxPlayer; }
	void SetCurPlayer( const int nCurPlayer )					{ m_nCurPlayer = nCurPlayer; }
	void SetLastUpdatedTime( const string& strLastUpdatedTime ) { m_strLastUpdatedTime = strLastUpdatedTime; }
	void SetIPString( const string& strIP )						{ m_strIP = strIP; }
	void SetIP( const DWORD dwIP )								{ m_dwIP = dwIP; }
	void SetPort( const int nPort )								{ m_nPort = nPort; }
	void SetServerName( const string& strServerName )			{ m_strServerName = strServerName; }
	void SetOpenState( const bool bOpenState )					{ m_bIsOpened = bOpenState; }
	void SetType( const char nType )							{ m_nType = nType; }
	void SetLiveStatus( const bool bLiveState );
	void SetAgentIPString( const string& strIP )				{ m_strAgentIP = strIP; }
	void SetAgentIP( const DWORD dwIP )							{ m_dwAgentIP = dwIP; }

#ifdef _DEBUG
	const string GetDebugString()
	{
		char szBuf[ 1024 ] = {0,};
		_snprintf( szBuf, 1023, "ID:%d, CurPlayer:%d, MaxPlayer:%d, Time:%s, IP:%s(%u), Port:%d, Name:%s, IsOpened:%d\n", 
			GetID(), GetCurPlayer(), GetMaxPlayer(), GetLastUpdatedTime().c_str(), GetIPString().c_str(), GetIP(),
			GetPort(), GetServerName().c_str(), IsOpened() );

		return string( szBuf );
	}
#endif

private :
	int		m_nID;
	int		m_nMaxPlayer;
	int		m_nCurPlayer;
	string	m_strLastUpdatedTime;
	string	m_strIP;
	DWORD	m_dwIP;
	int		m_nPort;
	string	m_strServerName;
	bool	m_bIsOpened;
	char	m_nType;				// �����1, �Ϲ�2, Ŭ��3, ����Ʈ4, �̺�Ʈ5.
	bool	m_bIsLive;
	string	m_strAgentIP;
	DWORD	m_dwAgentIP;
};


const int MakeCustomizeMin( const string& strTime );
const float GetPlayerRate( const float fCurPlayer, const float fMaxPlayer );


typedef vector< MServerStatus > ServerStatusVec;
typedef vector< int >			ServerIDVec;


class MServerStatusMgr
{
public :
	void		Insert( MServerStatus& ss );
	void		Reserve( const int nSize )			{ m_ServerStatusVec.reserve( nSize ); }
	void		Clear()								{ m_ServerStatusVec.clear(); }
	const int	Capacity() const					{ return static_cast< int >( m_ServerStatusVec.capacity() ); }
	const int	GetSize() const						{ return static_cast< int >( m_ServerStatusVec.size() ); }
	void		CheckDeadServerByLastUpdatedTime( const int nMarginOfErrMin, const int nCmpCustomizeMin );
	const int	CalcuMaxCmpCustomizeMin();
	void		IncreaseLiveServerCount( const int nCount = 1 ) { m_nLiveServerCount += nCount; }
	void		IncreaseDeadServerCount( const int nCount = 1 ) { m_nDeadServerCount += nCount; }
	void		AddDeadServerID( const int nID )				{ m_vDeadServerIDList.push_back( nID ); }
	const int	GetLiveServerCount() const			{ return m_nLiveServerCount; }
	const int	GetDeadServerCount() const			{ return m_nDeadServerCount; }
	const ServerIDVec& GetDeadServerIDList() const	{ return m_vDeadServerIDList; }

	
	const MServerStatus& operator[] ( const int nPos ) const { return m_ServerStatusVec[ nPos ]; }

private :
	void SetLiveServerCount( const int nCount ) { m_nLiveServerCount = 0; }
	void SetDeadServerCount( const int nCount ) { m_nDeadServerCount = 0; }
	void ClearDeadServerIDList()				{ m_vDeadServerIDList.clear(); }

private :
	ServerStatusVec m_ServerStatusVec;
	int				m_nLiveServerCount;
	int				m_nDeadServerCount;
	ServerIDVec		m_vDeadServerIDList;
};


// ������Ʈ ��ȭ�ð��� �ʹ� �� ���������� ã�Ƽ� Opened�� 0���� ������.
// �ð��� ������ ȯ���ؼ� �˻���.
template< typename T >
class MDeadTimeServerChecker
{
public :
	MDeadTimeServerChecker( const int nMarginOfErrorMin, 
						    const int nCmpCustomizeMin,
							MServerStatusMgr* pServerStatusMgr ) :
		m_nMarginOfErrorMin( nMarginOfErrorMin ), m_nCmpCustomizeMin( nCmpCustomizeMin ), m_pServerStatusMgr( pServerStatusMgr )
	{
		ASSERT( 0 != pServerStatusMgr );
	}

	~MDeadTimeServerChecker() {}

	void operator() ( T& tObj )
	{
		if( m_nMarginOfErrorMin > abs(m_nCmpCustomizeMin - MakeCustomizeMin(tObj.GetLastUpdatedTime())) )
		{
			tObj.SetLiveStatus( true );
			m_pServerStatusMgr->IncreaseLiveServerCount();
		}
		else
		{
			tObj.SetLiveStatus( false );
			m_pServerStatusMgr->IncreaseDeadServerCount();
			m_pServerStatusMgr->AddDeadServerID( tObj.GetID() );
		}
	}

private :
	MDeadTimeServerChecker();

	int					m_nMarginOfErrorMin;	// ���� ���ġ.
	int					m_nCmpCustomizeMin;		// �˻��� ������ �Ǵ� �ð�.
	MServerStatusMgr*	m_pServerStatusMgr;		// ȣ����.
};

#endif