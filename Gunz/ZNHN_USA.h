#ifndef _NHN_USA_AUTH
#define _NHN_USA_AUTH

#ifdef LOCALE_NHNUSA


#include ".\\nhn\\HanAuthForClient.h"

#include "MMatchGlobal.h"
#include "ZBaseAuthInfo.h"
#include <string>
using std::string;




class ZNHN_USAAuthInfo : public ZBaseAuthInfo
{
public :
	ZNHN_USAAuthInfo();
	~ZNHN_USAAuthInfo();


	// bool InitAuth();


	const string	GetUserID()		{ return m_strUserID; }
	const char*		GetAuthStr()	{ return m_szAuthStr; }
	const char*		GetGameStr()	{ return m_szGameStr; }


	void SetUserID( const string& strUserID ) { m_strUserID = strUserID; }
	void SetAuthStr( const char* szAuthStr, const int nLen )
	{
		if( NHN_AUTH_LENGTH < nLen ) return;

		strncpy( m_szAuthStr, szAuthStr, nLen );
		m_szAuthStr[ nLen + 1 ] = '\0';
	}
	void SetGameStr( const char* pszGameStr, const int nLen )
	{
		if( NHN_AUTH_LENGTH < nLen ) return;

		memset( m_szGameStr, 0, NHN_AUTH_LENGTH );
		memcpy( m_szGameStr, pszGameStr, nLen );
	}

	const bool IsAlpha();
	const bool IsReal();


	void ZUpdateGameString();

private :
	string	m_strUserID;
	char	m_szAuthStr[ NHN_AUTH_LENGTH ];
	char	m_szGameStr[ NHN_AUTH_LENGTH ];
};




bool NHN_USA_ParseArgument(	const char* pszArgs );
void WriteNHNAuthLog( const int nValue, const string& strComment );


#endif

#endif