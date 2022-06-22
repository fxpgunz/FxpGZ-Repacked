
#include "stdafx.h"
#include "ZNHN_USA_Report.h"
//#include "ZMyInfo.h"
#include "MDebug.h"

#ifdef LOCALE_NHNUSA


#ifndef _DEBUG
#pragma comment( lib, ".\\nhn\\HanReportForClient.lib" )
#else
#pragma comment( lib, ".\\nhn\\HanReportForClientD.lib" )
#endif

#ifndef _DEBUG
#define DEFAULT_SERVICE_TYPE (SERVICE_USA | SERVICE_REAL)
#else
#define DEFAULT_SERVICE_TYPE (SERVICE_USA | SERVICE_ALPHA)
#endif




ZNHNUSA_Report::ZNHNUSA_Report()
{
	memset( m_szUserID, 0, 20 );
}



ZNHNUSA_Report::~ZNHNUSA_Report()
{
}


bool ZNHNUSA_Report::InitReport( const char* szUserID, const char* pszGameStr )
{
	if( 20 < strlen(szUserID) ) return false;
	strcpy( m_szUserID, szUserID );

	if( 0 == HanReportInitGameStringWithDump(const_cast<char*>(pszGameStr)) )
	{
		mlog( "ZNHNUSA_Report::InitReport - success\n" );
		return true;
	}
	else
	{
		ASSERT( 0 );
		return false;
	}

	return true;
}


void ZNHNUSA_Report::ReportStartGame()
{
	char szReport[ MAX_REPORT_LEN ] = {0,};
	sprintf( szReport, "id=%s&value=%d", m_szUserID, 300 );
	if( 0 != HanReportSendPosLog(szReport) )
	{
		ASSERT( 0 );
		mlog( "ZNHNUSA_Report::ReportStartGame - fail(%s)\n", szReport );
	}
}


void ZNHNUSA_Report::ReportInitGameGuard()
{
	char szReport[ MAX_REPORT_LEN ] = {0,};
	sprintf( szReport, "id=%s&value=%d", m_szUserID, 301 );
	if( 0 != HanReportSendPosLog(szReport) )
	{
		ASSERT( 0 );
		mlog( "ZNHNUSA_Report::ReportInitGameGuard - fail(%s)\n", szReport );
	}
}


void ZNHNUSA_Report::ReportInitComplete()
{
	char szReport[ MAX_REPORT_LEN ] = {0,};
	sprintf( szReport, "id=%s&value=%d", m_szUserID, 302 );
	if( 0 != HanReportSendPosLog(szReport) )
	{
		ASSERT( 0 );
		mlog( "ZNHNUSA_Report::ReportInitComplete - fail(%s)\n", szReport );
	}
}


void ZNHNUSA_Report::ReportCreateGameScreen()
{
	char szReport[ MAX_REPORT_LEN ] = {0,};
	sprintf( szReport, "id=%s&value=%d", m_szUserID, 303 );
	if( 0 != HanReportSendPosLog(szReport) )
	{
		ASSERT( 0 );
		mlog( "ZNHNUSA_Report::ReportCreateGameScreen - fail(%s)\n", szReport );
	}
}


void ZNHNUSA_Report::ReportDisplayServerList()
{
	char szReport[ MAX_REPORT_LEN ] = {0,};
	sprintf( szReport, "id=%s&value=%d", m_szUserID, 304 );
	if( 0 != HanReportSendPosLog(szReport) )
	{
		ASSERT( 0 );
		mlog( "ZNHNUSA_Report::ReportDisplayServerList - fail(%s)\n", szReport );
	}
}


void ZNHNUSA_Report::ReportDisplaySelCharacter()
{
	char szReport[ MAX_REPORT_LEN ] = {0,};
	sprintf( szReport, "id=%s&value=%d", m_szUserID, 305 );
	if( 0 != HanReportSendPosLog(szReport) )
	{
		ASSERT( 0 );
		mlog( "ZNHNUSA_Report::ReportDisplaySelCharacter - fail(%s)\n", szReport );
	}
}


void ZNHNUSA_Report::ReportDisplayCreateCharacter()
{
	char szReport[ MAX_REPORT_LEN ] = {0,};
	sprintf( szReport, "id=%s&value=%d", m_szUserID, 306 );
	if( 0 != HanReportSendPosLog(szReport) )
	{
		ASSERT( 0 );
		mlog( "ZNHNUSA_Report::ReportDisplayCreateCharacter - fail(%s)\n", szReport );
	}
}


void ZNHNUSA_Report::ReportSelectCharacter()
{
	char szReport[ MAX_REPORT_LEN ] = {0,};
	sprintf( szReport, "id=%s&value=%d", m_szUserID, 308 );
	if( 0 != HanReportSendPosLog(szReport) )
	{
		ASSERT( 0 );
		mlog( "ZNHNUSA_Report::ReportSelectCharacter - fail(%s)\n", szReport );
	}
}


void ZNHNUSA_Report::ReportJoinChannel()
{
	char szReport[ MAX_REPORT_LEN ] = {0,};
	sprintf( szReport, "id=%s&value=%d", m_szUserID, 310 );
	if( 0 != HanReportSendPosLog(szReport) )
	{
		ASSERT( 0 );
		mlog( "ZNHNUSA_Report::ReportJoinChannel - fail(%s)\n", szReport );
	}
}


void ZNHNUSA_Report::ReportCompleteJoinChannel()
{
	char szReport[ MAX_REPORT_LEN ] = {0,};
	sprintf( szReport, "id=%s&value=%d", m_szUserID, 311 );
	if( 0 != HanReportSendPosLog(szReport) )
	{
		ASSERT( 0 );
		mlog( "ZNHNUSA_Report::ReportCompleteJoinChannel - fail(%s)\n", szReport );
	}
}


void ZNHNUSA_Report::ReportJoinRoom()
{
	char szReport[ MAX_REPORT_LEN ] = {0,};
	sprintf( szReport, "id=%s&value=%d", m_szUserID, 320 );
	if( 0 != HanReportSendPosLog(szReport) )
	{
		ASSERT( 0 );
		mlog( "ZNHNUSA_Report::ReportJoinRoom - fail(%s)\n", szReport );
	}
}


void ZNHNUSA_Report::ReportCompleteGameResult()
{
	char szReport[ MAX_REPORT_LEN ] = {0,};
	sprintf( szReport, "id=%s&value=%d", m_szUserID, 330 );
	if( 0 != HanReportSendPosLog(szReport) )
	{
		ASSERT( 0 );
		mlog( "ZNHNUSA_Report::ReportCompleteGameResult - fail(%s)\n", szReport );
	}
}


void ZNHNUSA_Report::ReportCloseGame()
{
	char szReport[ MAX_REPORT_LEN ] = {0,};
	sprintf( szReport, "id=%s&value=%d", m_szUserID, 900 );
	if( 0 != HanReportSendPosLog(szReport) )
	{
		ASSERT( 0 );
		mlog( "ZNHNUSA_Report::ReportCloseGame - fail(%s)\n", szReport );
	}
}


void ZNHNUSA_Report::ReportCrashedGame()
{
	char szReport[ MAX_REPORT_LEN ] = {0,};
	sprintf( szReport, "id=%s&value=%d", m_szUserID, 901 );
	if( 0 != HanReportSendPosLog(szReport) )
	{
		ASSERT( 0 );
		mlog( "ZNHNUSA_Report::ReportCrashedGame - fail(%s)\n", szReport );
	}
}


ZNHNUSA_Report& GetNHNUSAReport()
{
	return ZNHNUSA_Report::GetInstance();	
}

#endif