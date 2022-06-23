#ifndef _LOCATOR_UDP_
#define _LOCATOR_UDP_


#include "mempool.h"


#include <string>
#include <map>
using std::string;
using std::map;

class MLocatorUDPInfo : public CMemPool< MLocatorUDPInfo > 
{
	friend class MLocator;
	friend class MUDPManager;

	enum MAX_SIZE
	{
		USE_COUNT_SIZE = 2100000000,
	};

public :
	MLocatorUDPInfo() : m_nUseCount(0), m_nUsedCount(0) {}
	~MLocatorUDPInfo() {}

	const DWORD			GetIP() const			 { return m_dwIP; }
	const string&		GetStrIP() const		 { return m_strIP; }
	const int			GetPort() const			 { return m_nPort; }
	const DWORD			GetUseStartTime() const	 { return m_dwUseStartTime; }
	const unsigned int	GetUseCount() const		 { return m_nUseCount; }
	const unsigned int	GetUsedCount() const	 { return m_nUsedCount; }
	const unsigned int	GetTotalUseCount() const { return m_nUsedCount + m_nUseCount; }
		
	void IncreaseUseCount( const unsigned int nCount = 1 ) 
	{ 
		if( USE_COUNT_SIZE > (nCount + m_nUseCount) ) // �����÷ο� ����. �̹� �̼�ġ�� ���� ��������.
			m_nUseCount += nCount; 
	} 
	void IncreaseUsedCount( const unsigned int nCount = 1 )
	{
		if( USE_COUNT_SIZE > (nCount + m_nUsedCount) ) // �����÷ο� ����. �̹� �̼�ġ�� ���� ��������.
			m_nUsedCount += nCount; 
	}

	const DWORD CalcuUseElapsedTime();
	const DWORD CalcuUseElapsedTime( const DWORD dwTime ) { return (dwTime - m_dwUseStartTime); }

private :
	void SetIP( const DWORD dwIP )							{ m_dwIP = dwIP; }
	void SetStrIP( const string& strIP )					{ m_strIP = strIP; }
	void SetPort( const int nPort )							{ m_nPort = nPort; }
	void SetUseStartTime( const DWORD dwUseStartTime )		{ m_dwUseStartTime = dwUseStartTime; }
	void SetUseCount( const unsigned int nUseCount )		
	{ 
		if( USE_COUNT_SIZE > nUseCount )
			m_nUseCount = nUseCount;
		else
			m_nUseCount = USE_COUNT_SIZE;
	}
	void SetUsedCount( const unsigned int nUsedCount )
	{
		if( USE_COUNT_SIZE > nUsedCount )
			m_nUsedCount = nUsedCount;
		else
			m_nUsedCount = USE_COUNT_SIZE;
	}
		
	void SetInfo( const DWORD dwIP,
				  const int nPort,
				  const unsigned int nUseCount, 
				  const DWORD dwUseStartTime, 
				  const string& strIP );

private :
	DWORD			m_dwIP;
	string			m_strIP;
	int				m_nPort;
	DWORD			m_dwUseStartTime;
	unsigned int	m_nUseCount;
	unsigned int	m_nUsedCount;
};


template< typename T >
class MDeadUDPFinder
{
public :
	MDeadUDPFinder( const DWORD dwLimitTime, const DWORD dwEventTime ) : 
	   m_dwLimitTime( dwLimitTime ), m_dwEventTime( dwEventTime ) {}
	~MDeadUDPFinder() {}

	bool operator() ( const T& tObj )
	{
		return m_dwLimitTime < (m_dwEventTime - tObj.second->GetUseStartTime());
	}

private :
	MDeadUDPFinder();

	DWORD m_dwLimitTime;
	DWORD m_dwEventTime;
};


class MUDPManager : public map< DWORD, MLocatorUDPInfo* >
{
public :
	MUDPManager();
	~MUDPManager();

	void InitRecvUDPInfoMemPool();
	void ReleaseRecvUDPInfoMemPool();

	bool Insert( const DWORD dwIPKey,const int nPort, const DWORD dwUseStartTime );
	bool Insert( const DWORD dwIPKey, MLocatorUDPInfo* pRecvUDPInfo );
	bool SafeInsert( const DWORD dwIPKey, const int nPort, const DWORD dwUseStartTime );
	bool SafeInsert( const DWORD dwIPKey, MLocatorUDPInfo* pRecvUDPInfo );

	MLocatorUDPInfo* PopFirst();
	MLocatorUDPInfo* SafePopFirst();

	MLocatorUDPInfo* PopByIPKey( const DWORD dwIPKey );
	MLocatorUDPInfo* SafePopByIPKey( const DWORD dwIPKey );

	bool CheckWasInserted( const DWORD dwIPKey );
	bool SafeCheckWasInserted( const DWORD dwIPKey );

	const unsigned char GetUseCount( const DWORD dwIPKey );
	const unsigned char SafeGetUseCount( const DWORD dwIPKey );

	MLocatorUDPInfo* Find( const DWORD dwIPKey );
	MLocatorUDPInfo* SafeFind( const DWORD dwIPKey );

	void Delete( const DWORD dwIPKey );
	void SafeDelete( const DWORD dwIPKey );

	void ClearElapsedLiveTimeUDP( const DWORD dwLiveTime, const DWORD dwEventTime );
	void SafeClearElapsedLiveTimeUDP( const DWORD dwLiveTime, const DWORD dwEventTime );

	void SafeDestroy();

	void Lock()		{ EnterCriticalSection( &m_csLock ); }
	void Unlock()	{ LeaveCriticalSection( &m_csLock ); }

	void DumpStatusInfo();
	void DumpUDPInfo();

	///////////////////////////////////////////////////////////////////////////////////////
	// �ܺο��� ����Ž���� �ʿ��ҽø� ���...
	// �ݵ�� ������� SetBegin()�� �ϰ� GetCurPosUDP()�� ���� ��ġ�� Object�� ������.
	// MoveNext�� ���� Object�� �̵��ϰ�, end()�� �����ϸ� false�� ��ȯ��. 
	// GetCurPosUDP()�� MoveNext()�� ���ô� ��ȯ���� �˻��ؾ� ��.
	void SetBegin();
	MLocatorUDPInfo* GetCurPosUDP();
	bool MoveNext();
	///////////////////////////////////////////////////////////////////////////////////////

private :
	CRITICAL_SECTION m_csLock;

	iterator m_itBegin;
	
#ifdef _DEBUG
public :
	string m_strExDbgInfo;
#endif

};

#endif