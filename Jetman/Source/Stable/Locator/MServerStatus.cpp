#include "stdafx.h"
#include "MUID.h"
#include "MMatchGlobal.h"
#include "MLocatorConfig.h"
#include "MLocatorStatistics.h"
#include "MServerStatus.h"


#include <algorithm>

const int MakeCustomizeMin( const string& strTime )
{
	int nHourConvMin;
	int nMin;
	char szVal[ 3 ];

	memset( szVal, 0, 3 );
	strncpy( szVal, &strTime[11], 2 );
	nHourConvMin = atoi( szVal ) * 60;

	memset( szVal, 0, 3 );
	strncpy( szVal, &strTime[14], 2 );
	nMin = atoi( szVal );

	return nHourConvMin + nMin;
}


const float GetPlayerRate( const float fCurPlayer, const float fMaxPlayer )
{
	return fCurPlayer / fMaxPlayer * 100.0f;
}


//////////////////////////////////////////////////////////////////////////////////////////////


void MServerStatus::SetLiveStatus( const bool bLiveState )
{
	if( m_bIsLive && !bLiveState )
		GetLocatorStatistics().IncreaseDeadServerStatistics( GetID() );
	
	m_bIsLive = bLiveState;
}


//////////////////////////////////////////////////////////////////////////////////////////////


void MServerStatusMgr::Insert( MServerStatus& ss )
{
	m_ServerStatusVec.push_back( ss );
}


void MServerStatusMgr::CheckDeadServerByLastUpdatedTime( const int nMarginOfErrMin, const int nCmpCustomizeMin )
{
	SetLiveServerCount( 0 );
	SetDeadServerCount( 0 );
	ClearDeadServerIDList();

	for_each( m_ServerStatusVec.begin(),
			  m_ServerStatusVec.end(),
			  MDeadTimeServerChecker<MServerStatus>(nMarginOfErrMin, nCmpCustomizeMin, this) );
}


const int MServerStatusMgr::CalcuMaxCmpCustomizeMin()
{
	// Locator�� Server�� ���� �ð����� ��ҿ� �ֱ⿡ Locator�� �ð��� ������ ������ ������ ���Ʈ �豳 �ð��� ����.

	SYSTEMTIME st;
	// GetSystemTime( &st );
	GetLocalTime( &st );

	return static_cast< int >( (st.wHour * 60) + st.wMinute );
}