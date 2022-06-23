#include "StdAfx.h"
#include ".\mlocatordbmgr.h"
#include "MDebug.h"
#include "Winsock2.h"
#include "CustomDefine.h"
#include "MLocatorConfig.h"
#include "MServerStatus.h"
#include "ODBCRecordset.h"

#include <algorithm>


#pragma comment( lib, "Ws2_32.lib" )


MLocatorDBMgr::MLocatorDBMgr(void)
{
	SetDB( new MDatabase );
}

MLocatorDBMgr::~MLocatorDBMgr(void)
{
	ReleaseDB();
}


bool MLocatorDBMgr::CheckOpen()
{
	return GetDB()->CheckOpen();
}


CString MLocatorDBMgr::BuildDSNString(const CString& strDSN, const CString& strUserName, const CString& strPassword)
{
	return GetDB()->BuildDSNString( strDSN, strUserName, strPassword );
}


bool MLocatorDBMgr::Connect( const CString& strDSNConnect)
{
	return GetDB()->Connect( strDSNConnect );
}


void MLocatorDBMgr::Disconnect()
{
	if( GetDB()->IsOpen() )
		GetDB()->Disconnect();
}


bool MLocatorDBMgr::GetServerStatus( MServerStatusMgr* pServerStatusMgr )
{
	if( CheckOpen() && (0 != pServerStatusMgr) )
	{
		pServerStatusMgr->Clear();

		/////////////////////////////////
		//CTime tm = CTime::GetCurrentTime();
		//CString strTime = tm.Format("%Y-%m-%d %H:%M:%S");

		//MServerStatus ss;

		//ss.SetID( 999 );
		//ss.SetCurPlayer( 999 );
		//ss.SetMaxPalyer( 1500 );
		//ss.SetLastUpdatedTime( (LPCTSTR)strTime );
		//ss.SetIPString( "200.229.52.13" );
		//ss.SetIP( inet_addr("200.229.52.13") );
		//ss.SetPort( 6000 );
		//ss.SetServerName( "Test" );
		//ss.SetOpenState( 1 );

		//pServerStatusMgr->Insert( ss );
		//return 1;
		/////////////////////////////////

		CODBCRecordset rs(GetDB());

		try
		{
			if (GetLocatorConfig()->IsTestServerOnly() == true) {
				// TEST SERVER ��ϸ� ������.
#if defined(LOCALE_NHNUSA)
				rs.Open("SELECT ServerID, Type, CurrPlayer, MaxPlayer, CONVERT(varchar(20), Time, 20) AS Time, IP, Port, ServerName, Opened, AgentIP FROM ServerStatus(NOLOCK) WHERE (Opened <> 0) AND (Opened IS NOT NULL) AND Type=6", 
					CRecordset::forwardOnly, CRecordset::readOnly);
#else 
				rs.Open("SELECT ServerID, Type, CurrPlayer, MaxPlayer, CONVERT(varchar(20), Time, 20) AS Time, IP, Port, ServerName, Opened FROM ServerStatus(NOLOCK) WHERE (Opened <> 0) AND (Opened IS NOT NULL) AND Type=6", 
					CRecordset::forwardOnly, CRecordset::readOnly);
#endif
			} else {
				// DB���� �����ö� ���͸��ؼ� ������.
#if defined(LOCALE_NHNUSA)
				rs.Open("SELECT ServerID, Type, CurrPlayer, MaxPlayer, CONVERT(varchar(20), Time, 20) AS Time, IP, Port, ServerName, Opened, AgentIP FROM ServerStatus(NOLOCK) WHERE (Opened <> 0) AND (Opened IS NOT NULL) AND Type <> 6", 
					CRecordset::forwardOnly, CRecordset::readOnly);
#else 
				rs.Open("SELECT ServerID, Type, CurrPlayer, MaxPlayer, CONVERT(varchar(20), Time, 20) AS Time, IP, Port, ServerName, Opened FROM ServerStatus(NOLOCK) WHERE (Opened <> 0) AND (Opened IS NOT NULL) AND Type <> 6", 
					CRecordset::forwardOnly, CRecordset::readOnly);
#endif
			}
		}
		catch( CDBException* e )
		{
			MLog( "MLocatorDBMgr::GetServerStatus - %s\n", e->m_strError );
			return false;
		}

		if( rs.IsOpen() )
		{
			if( (long)pServerStatusMgr->Capacity() < rs.GetRecordCount() )
				pServerStatusMgr->Reserve( rs.GetRecordCount() );

			MServerStatus ss;

			for( ; !rs.IsEOF(); rs.MoveNext() )
			{
				ss.SetID( rs.Field( "ServerID" ).AsInt() );
				ss.SetType( rs.Field( "Type" ).AsChar() );
				ss.SetCurPlayer( rs.Field( "CurrPlayer" ).AsInt() );
				ss.SetMaxPalyer( rs.Field( "MaxPlayer" ).AsInt() );
				ss.SetLastUpdatedTime( rs.Field( "Time" ).AsString().GetBuffer() );
				ss.SetIPString( rs.Field( "IP" ).AsString().GetBuffer() );
				ss.SetIP( inet_addr(ss.GetIPString().c_str()) );
				ss.SetPort( rs.Field( "Port" ).AsInt() );
				ss.SetServerName( rs.Field( "ServerName" ).AsString().GetBuffer() );
				ss.SetOpenState( 0 != rs.Field( "Opened" ).AsInt() );
#if defined(LOCALE_NHNUSA)
				ss.SetAgentIPString( rs.Field( "AgentIP" ).AsString().GetBuffer() );
#endif
				ss.SetAgentIP( inet_addr(ss.GetAgentIPString().c_str()) );

				pServerStatusMgr->Insert( ss );
			}

			return ( (long)pServerStatusMgr->GetSize() == rs.GetRecordCount() );
		}
	}

	return false;
}


bool MLocatorDBMgr::StartUpLocaterStauts( const int nLocatorID, 
										  const string& strIP, 
										  const int nPort, 
										  const int nDuplicatedUpdateTime )
{
	if( !CheckOpen() )
		return false;

	CString strQuery;
	try 
	{
		strQuery.Format("{CALL spStartUpLocatorStatus (%d, '%s', %d, %d)}", 
			nLocatorID, strIP.c_str(), nPort, nDuplicatedUpdateTime );

		GetDB()->ExecuteSQL( strQuery );
	}
	catch( CDBException* e )
	{
		mlog( "MLocatorDBMgr::StartUpLocaterStauts - %s\n", 
			e->m_strError );
		return false;
	}

	return true;
}


bool MLocatorDBMgr::UpdateLocaterStatus( const int nLocatorID, 
										 const DWORD nRecvCount, 
										 const DWORD nSendCount, 
										 const DWORD nBlockCount, 
										 const DWORD nDuplicatedCount )
{
	if( !CheckOpen() )
		return false;

	CString strQuery;
	try
	{
		strQuery.Format( "{CALL spUpdateLocatorStatus (%d, %u, %u, %u, %u)}",
			nLocatorID, nRecvCount, nSendCount, nBlockCount, nDuplicatedCount );
		GetDB()->ExecuteSQL( strQuery );
	}
	catch( CDBException* e )
	{
		mlog( "MLocatorDBMgr::UpdateLocaterStatus - %s\n",
			e->m_strError );
		return false;
	}

	return true;
}


bool MLocatorDBMgr::InsertLocatorLog( const int nLocatorID, const map<string, DWORD>& CountryStatistics )
{
	if( CountryStatistics.empty() ) return true;

	if( !CheckOpen() ) 
		return false;

	CString strQuery;

	try
	{
		map< string, DWORD >::const_iterator it, end;
		end = CountryStatistics.end();
		for( it = CountryStatistics.begin(); it != end; ++it )
		{
			strQuery.Format( "{CALL spInsertLocatorLog (%d, '%s', %u)}",
				nLocatorID, it->first.c_str(), it->second );
			GetDB()->ExecuteSQL( strQuery );
		}
	}
	catch( CDBException* e )
	{
		mlog( "MLocatorDBMgr::InsertLocatorLog - %s\n",
			e->m_strError );
	}

	return true;
}
