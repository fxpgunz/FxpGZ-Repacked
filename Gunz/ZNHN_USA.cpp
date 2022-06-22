#include "stdafx.h"
#include "ZNHN_USA.h"
#include "ZLocale.h"

#ifdef LOCALE_NHNUSA

#ifndef _DEBUG
#pragma comment( lib, ".\\nhn\\HanAuthForClient.lib" )
#else 
#pragma comment( lib, ".\\nhn\\HanAuthForClientD.lib" )
#endif


#ifndef _DEBUG
#define DEFAULT_SERVICE_TYPE (SERVICE_USA | SERVICE_REAL)
#else
#define DEFAULT_SERVICE_TYPE (SERVICE_USA | SERVICE_ALPHA)
#endif



ZNHN_USAAuthInfo::ZNHN_USAAuthInfo()
{
	memset(m_szAuthStr, 0, NHN_AUTH_LENGTH );
	strcpy( m_szAuthStr, "init" );
}


ZNHN_USAAuthInfo::~ZNHN_USAAuthInfo()
{
}


const bool ZNHN_USAAuthInfo::IsAlpha()
{
	int nCode = 0; 
	HanAuthGetServiceTypeCode( m_szGameStr, &nCode ); 
	if( SERVICE_ALPHA != nCode )
		return false;
	return true;
}


const bool ZNHN_USAAuthInfo::IsReal()
{
	int nCode = 0; 
	HanAuthGetServiceTypeCode( m_szGameStr, &nCode ); 
	if( SERVICE_REAL != nCode )
		return false;
	return true;
}

void ZNHN_USAAuthInfo::ZUpdateGameString()
{
	char szGameStringNew[ NHN_AUTH_LENGTH ]	= {0,};
	const int nUpdateRet = UpdateGameString( m_szGameStr, szGameStringNew, sizeof(szGameStringNew) );

	strcpy( m_szGameStr, szGameStringNew);

	if( 0 != nUpdateRet )
	{
		WriteNHNAuthLog( nUpdateRet, "UpdateGameString" );
	}
}



/////////////////////////////////////////////////////////////////////////////////////////////////////


bool NHN_USA_ParseArgument( const char* pszArgs )
{
#ifdef _DEBUG_NHN_USA_AUTH
	return true;
#endif

	mlog( "start init nhn auth.\n" );

	const int nInitRet = HanAuthInitGameString( const_cast<char*>(pszArgs) );
	if( 0 != nInitRet )
	{
		WriteNHNAuthLog( nInitRet, "HanAuthInit" );
		return false;
	}

#ifdef _DEBUG
	mlog( pszArgs );
#endif

	const int nAuthStrLen = strlen( pszArgs );
    	
	char szGameStringOld[ NHN_AUTH_LENGTH ]	= {0,};
	char szGameStringNew[ NHN_AUTH_LENGTH ]	= {0,};
	char szAuthString[ NHN_AUTH_LENGTH ]	= {0,};
	char szMemberID[ 25 ]					= {0,};

	strcpy( szGameStringOld, pszArgs );
	
	const int nClientRet = HanAuthForClientGameString( szGameStringOld );
	if( 0 != nClientRet )
	{
		WriteNHNAuthLog( nClientRet, "HanAuthForClient" );
		return false;
	}

#ifdef _DEBUG
	mlog( "\nszGameStringOld : (%s)\n", szGameStringOld );
#endif

	const int nStrRet = GetAuthString( szAuthString, NHN_AUTH_LENGTH );
	if( 0 != nStrRet )
	{
		WriteNHNAuthLog( nStrRet, "GetAuthString" );
		return false;
	}

#ifdef _DEBUG
	mlog( "\nAuthString : (%s)\n", szAuthString );
#endif

	const int nIDRet = HanAuthGetId( szGameStringOld, szMemberID, 25 );
	if( 0 != nIDRet )
	{
		WriteNHNAuthLog( nIDRet, "\nHanAuthGetID" );
		return false;
	}

#ifdef _DEBUG
	mlog( "\nGetID : (%s)\n", szMemberID );
#endif

	const int nRefreshRet = HanAuthRefresh( szMemberID );
	if( 0 != nRefreshRet )
	{
		WriteNHNAuthLog( nRefreshRet, "HanAuthRefresh" );
		return false;
	}

	const int nUpdateRet = UpdateGameString( szGameStringOld, szGameStringNew, sizeof(szGameStringNew) );
	if( 0 != nUpdateRet )
	{
		WriteNHNAuthLog( nUpdateRet, "UpdateGameString" );
		return false;
	}


	((ZNHN_USAAuthInfo*)ZGetLocale()->GetAuthInfo())->SetAuthStr( szAuthString, strlen(szAuthString) );
	((ZNHN_USAAuthInfo*)ZGetLocale()->GetAuthInfo())->SetUserID( szMemberID );
	((ZNHN_USAAuthInfo*)ZGetLocale()->GetAuthInfo())->SetGameStr( pszArgs, nAuthStrLen );

	mlog( "init nhn auth success." );

	return true;
}





//bool ZNHN_USAAuthInfo::InitAuth()
//{
//	// const int nInitRet = HanAuthInit( NHN_GAMEID, DEFAULT_SERVICE_TYPE );
//	const int nInitRet = HanAuthInit( NHN_GAMEID, SERVICE_USA | SERVICE_ALPHA );
//	if( 0 != nInitRet )
//	{
//		WriteNHNAuthLog( nInitRet, "HanAuthInit" );
//		return false;
//	}
//
//	return true;
//}


void WriteNHNAuthLog( const int nValue, const string& strComment )
{
	char szLog[ 256 ]		= {0,};
	char szErrCode[ 64 ]	= {0,};

	switch( nValue )
	{
	case -1 : 
		sprintf( szErrCode, "general fault." );
		break;

	case 1 : 
		sprintf( szErrCode, "parameter of auth is invalid." );
		break;

	case 2 :
		sprintf( szErrCode, "this ip address is invalid." );
		break;

	case 3 : 
		sprintf( szErrCode, "memberID is invalid." );
		break;

	case 4 :
		sprintf( szErrCode, "password incorrect." );
		break;

	case 5 :
		sprintf( szErrCode, "password mismatch( over 3 times )." );
		break;

	case 6 :
		sprintf( szErrCode, "memberID is not HangameID." );
		break;

	case 7 :
		sprintf( szErrCode, "system error." );
		break;

	default :
		ASSERT( 0 && "not defined." );
		break;
	};

	sprintf( szLog, "Init Authentic fail. error code : (%s), comment : (%s)\n", szErrCode, strComment.c_str() );
	mlog( szLog );
}


#endif