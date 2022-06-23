#include "stdafx.h"
#include "MUtil.h"

#include <windows.h>
#include <mmsystem.h>


const __int64 IN_SEC = 10000000i64;
const __int64 IN_MIN = IN_SEC  * 60;
const __int64 IN_HOUR = IN_MIN  * 60;
const __int64 IN_DAY = IN_HOUR * 24;


const string MakeTimeString( const MDateType DateType, SYSTEMTIME st )
{
	char szBuf[ 128 ] = {0,};

	if( MDT_Y == DateType )
	{
		_snprintf( szBuf, 127, "%u", st.wYear );
	}
	else if( MDT_YM == DateType )
	{
		_snprintf( szBuf, 127, "%u-%u",
			st.wYear, st.wMonth );
	}
	else if( MDT_YMD == DateType )
	{
		_snprintf( szBuf, 127, "%u-%u-%u",
			st.wYear, st.wMonth, st.wDay );
	}
	else if( MDT_YMDH == DateType )
	{
		_snprintf( szBuf, 127, "%u-%u-%u %u",
			st.wYear, st.wMonth, st.wDay, st.wHour );
	}
	else if( MDT_YMDHM == DateType )
	{
		_snprintf( szBuf, 127, "%u-%u-%u %u:%u",
			st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute );
	}

	return string( szBuf );
}

const string MGetStrLocalTime( const MDateType DateType )
{
	SYSTEMTIME st;
	GetLocalTime( &st );

	return MakeTimeString(DateType, st);
}

const string MGetStrLocalTime( const unsigned short wDay, const unsigned short wHour, const unsigned short wMin, const MDateType DateType )
{
	FILETIME ftm;
	LARGE_INTEGER large_int;
	SYSTEMTIME stNow, stResult;

	GetLocalTime( &stNow );
	SystemTimeToFileTime(&stNow, &ftm);   
	memcpy(&large_int, &ftm, sizeof(FILETIME));

	large_int.QuadPart = large_int.QuadPart + (wDay * IN_DAY) + (wHour * IN_HOUR) + (wMin * IN_MIN);

	memcpy(&ftm, &large_int, sizeof(FILETIME));
	FileTimeToSystemTime(&ftm, &stResult);
	
	return MakeTimeString(DateType, stResult);
}


bool SplitStrIP( const string& strIP, vector<BYTE>& vIP )
{
	if( strIP.empty() || (7 > strIP.length()) ) 
		return false;

	size_t a, b, c;
	char szPos1[ 4 ] = {0,};
	char szPos2[ 4 ] = {0,};
	char szPos3[ 4 ] = {0,};
	char szPos4[ 4 ] = {0,};

	a = strIP.find( "." );
	if( string::npos == a ) 
		return false;

	b = strIP.find( ".", a + 1 );
	if( string::npos == b ) 
		return false;

	c = strIP.find( ".", b + 1 );
	if( string::npos == c )
		return false;

	strncpy( szPos1, &strIP[0], a );
	strncpy( szPos2, &strIP[a + 1], b - a - 1 );
	strncpy( szPos3, &strIP[b + 1], c - b - 1 );
	strncpy( szPos4, &strIP[c + 1], strIP.length() - c - 1 );

	vIP.push_back( static_cast<BYTE>(atoi(szPos1)) );
	vIP.push_back( static_cast<BYTE>(atoi(szPos2)) );
	vIP.push_back( static_cast<BYTE>(atoi(szPos3)) );
	vIP.push_back( static_cast<BYTE>(atoi(szPos4)) );

	return true;
}