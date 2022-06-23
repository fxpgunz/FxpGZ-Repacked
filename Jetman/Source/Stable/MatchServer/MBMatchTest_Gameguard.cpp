#include "stdafx.h"
#include "MBMatchTest.h"
#include "MMatchObject.h"
#include "MDebug.h"
#include "MBMatchServer.h"
#include "MMatchKillTrackerConfig.h"
#include "MMatchConfig.h"
#include "MErrorTable.h"
#include "MSharedCommandTable.h"
#include "MCommand.h"
#include "MUID.h"


void MBMatchTest::InitGameguardTest()
{
	MUID uid = m_pServer->UseUID();

	_ASSERT( MOK == m_pServer->ObjectAdd(uid) );

	MMatchObject* pObj = m_pServer->GetObject( uid );
	_ASSERT( NULL != pObj);

	AddTestObject( "GGObject", "MMatchObject", (void*)pObj );
}


void MBMatchTest::DoTest_GameguardFirstRequest( const DWORD dwCurTime )
{
	mlog( "// GAMEGUARD TEST /////////////////////////////////////////\n" );
	TESTOBJECT* pTestObj = GetTestObject( "GGObject" );
	_ASSERT( NULL != pTestObj );
	_ASSERT( NULL != pTestObj->pObject );

	MMatchObject* pObj = (MMatchObject*)(pTestObj->pObject);

	MCommand* pCmd = m_pServer->CreateCommand( MC_RESPONSE_FIRST_GAMEGUARD_AUTH, pObj->GetUID() );
	_ASSERT( NULL != pCmd );

	MCommand* pDumCmd = m_pServer->CreateCommand( MC_REQUEST_FIRST_GAMEGUARD_AUTH, pObj->GetUID() );
	_ASSERT( NULL != pDumCmd );

	MUID uid = pObj->GetUID();
	pCmd->SetSenderUID( uid );
	pDumCmd->SetSenderUID( uid );
	
	_ASSERT( false == pObj->IsSendFirstGameguardRequest() );
	_ASSERT( false == pObj->IsRecvFirstGameguardResponse() );
	_ASSERT( false == m_pServer->IsSkeepByGameguardFirstAuth(pCmd) );	///< 아직 요청한 적이 없어서 통과 해야 한다.

    pObj->FirstGameguardReqeustIsSent();
	_ASSERT( true == pObj->IsSendFirstGameguardRequest() );
	_ASSERT( false == pObj->IsRecvFirstGameguardResponse() );
	_ASSERT( true == m_pServer->IsSkeepByGameguardFirstAuth(pDumCmd) );	
	_ASSERT( false == m_pServer->IsSkeepByGameguardFirstAuth(pCmd) );	

	pObj->FirstGameguardResponseIsRecved();
	_ASSERT( true == pObj->IsSendFirstGameguardRequest() );
	_ASSERT( true == pObj->IsRecvFirstGameguardResponse() );
	_ASSERT( false == m_pServer->IsSkeepByGameguardFirstAuth(pDumCmd) );	
	_ASSERT( false == m_pServer->IsSkeepByGameguardFirstAuth(pCmd) );	///< 응답이 왔으니 통과를 해야 한다.

	m_pServer->m_Objects.erase( pObj->GetUID() );

	delete pCmd;
	delete pDumCmd;
	mlog( "// GAMEGUARD TEST END ////////////////////////////////////\n" );
}