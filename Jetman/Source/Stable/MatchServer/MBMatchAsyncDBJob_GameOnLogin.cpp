#include "stdafx.h"
#include "MBMatchAsyncDBJob_GameOnLogin.h"
#include "MBMatchGameOnAuth.h"
#include "MMatchObject.h"
#include "MMatchDBMgr.h"


bool MBMatchAsyncDBJob_GameOnLogin::Input( const MUID& uidCommUID, const string& strUserID, const int nCheckSum,
									   const bool bIsFreeLoginIP, const string strCountryCode3, const int nServerID )
{
	m_CommUID			= uidCommUID;
	m_strUserID			= strUserID;
	m_nCheckSum			= nCheckSum;
	m_bIsFreeLoginIP	= bIsFreeLoginIP;
	m_strCountryCode3	= strCountryCode3;
	m_nServerID			= nServerID;

	m_pAccountInfo = new MMatchAccountInfo;
	if( 0 == m_pAccountInfo ) return false;

	m_pAccountPenaltyInfo = new MMatchAccountPenaltyInfo;
	if( 0 == m_pAccountPenaltyInfo ) return false;

	return true;
}


MBMatchAsyncDBJob_GameOnLogin::~MBMatchAsyncDBJob_GameOnLogin()
{
	DeleteMemory();
}


void MBMatchAsyncDBJob_GameOnLogin::Run( void* pContext )
{
	MMatchDBMgr* pDBMgr = (MMatchDBMgr*)pContext;

	// 계정 정보를 읽는다.
	if (!pDBMgr->GetAccountInfo(m_nAID, m_pAccountInfo, m_nServerID))
	{
		mlog( "MBMatchAsyncDBJob_GameOnLogin - can't find accont.\n" );
		SetResult(MASYNC_RESULT_FAILED);
		return;
	}

	// 계정 페널티 정보를 읽는다. - 2010-08-10 홍기주
	if( !pDBMgr->GetAccountPenaltyInfo(m_nAID, m_pAccountPenaltyInfo) ) 
	{
		SetResult(MASYNC_RESULT_FAILED);
		return;
	}

	SetResult(MASYNC_RESULT_SUCCEED);
}


void MBMatchAsyncDBJob_GameOnLogin::DeleteMemory()
{
	if( NULL != m_pAccountInfo )
	{
		delete m_pAccountInfo;
		m_pAccountInfo = NULL;
	}

	if( NULL != m_pAccountPenaltyInfo )
	{
		delete m_pAccountPenaltyInfo;
		m_pAccountPenaltyInfo = NULL;
	}
}