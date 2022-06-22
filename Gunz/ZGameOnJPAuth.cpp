#include "stdafx.h"
#include "ZGameOnJpAuth.h"
#include "ZLocale.h"
#include "ZConfiguration.h"
#include "ZLocatorList.h"

#define VERSION_CONFIG_FILENAME		"./version.ini"


HWND ZGameOnJPAuthInfo::m_hLauncher;

ZGameOnJPAuthInfo::ZGameOnJPAuthInfo()
{
	m_bAlpha = false;
}


bool ZGameOnJPAuthInfo::ParseVersion()
{
	ZLocatorList* pLocatorList = ZGetConfiguration()->GetLocatorList();
	if ( pLocatorList == NULL)
		return false;


	// Launch mode
	char szTestLaunch[ 64];
	GetPrivateProfileString( "SERVER", "TESTLAUNCH", "FALSE", szTestLaunch, 64, VERSION_CONFIG_FILENAME );

	if ( stricmp( szTestLaunch, "TRUE") == 0)
		m_bAlpha = true;

#ifdef _DEBUG
	if ( m_bAlpha == true)
		mlog( "%s : Launch mode = Alpha\n", __FUNCTION__);
	else
		mlog( "%s : Launch mode = Real\n", __FUNCTION__);
#else
	if ( m_bAlpha == true)
		mlog( "Alpha launch mode\n");
#endif


	// Server list
	int nCount = 0;
	nCount = GetPrivateProfileInt( "SERVER", "COUNT", 0, VERSION_CONFIG_FILENAME);

	if ( nCount == 0)
	{
		mlog( "ERROR : The count of locator is ZERO.\n");
		ASSERT( 0);
		return false;
	}

#ifdef _DEBUG
		mlog( "%s : Server count = %d\n", __FUNCTION__, nCount);
#endif


	for ( int i = 0;  i < nCount;  i++)
	{
		char szKeyName[ 64];
		sprintf( szKeyName, "IP%d", i + 1);

		char szIP[ 64];
		GetPrivateProfileString( "SERVER", szKeyName, "", szIP, 64, VERSION_CONFIG_FILENAME );

		pLocatorList->AddIP( szIP);

#ifdef _DEBUG
		mlog( "%s : Server IP = %s\n", __FUNCTION__, szIP);
#endif
	}


	return true;
}


void ZGameOnJPAuthInfo::SetString( const char* szString)
{
	m_strString = szString;
}


const char* ZGameOnJPAuthInfo::GetString()
{
	return m_strString.c_str();
}


void ZGameOnJPAuthInfo::SetStatIndex( const char* szStatIndex)
{
	m_strStatIndex = szStatIndex;
}


const char* ZGameOnJPAuthInfo::GetStatIndex()
{
	return m_strStatIndex.c_str();
}


LRESULT ZGameOnJPAuthInfo::SendMsgToLauncher(LPCSTR lpMsg)
{
	// TITLE_PUBLAGENT값은 GameChuAgent내에거 바뀔수 있으므로, 게임 기동시 HWND값을 설정해 두어야 됨. 
	// TITLE_PUBLAGENT로 된 Window를 찾음 ... 
	if ( m_hLauncher == NULL )
		return FALSE;

	if ( lpMsg == NULL )
		return FALSE;

	if ( m_hLauncher )
	{
		COPYDATASTRUCT stData;

		stData.dwData= 0;   
		stData.cbData= (DWORD)strlen(lpMsg) + 1;
		stData.lpData= (PVOID)lpMsg;

		return ::SendMessage( m_hLauncher, WM_COPYDATA, (WPARAM)g_hWnd, (LPARAM)(&stData));	
	}

	return FALSE;
}


LRESULT ZGameOnJPAuthInfo::NewLogin(WPARAM wparam, LPARAM lparam)
{
	COPYDATASTRUCT *pData= (COPYDATASTRUCT *)lparam;

	if ( pData )
	{
		// TERMINATE_GAMECLIENT 인경우, 게임 클라이언트를 종료 시킴 ...
		if ( _stricmp( (char*)pData->lpData, "TERMINATE_GAMECLIENT") == 0)
		{
			return TRUE;
		}
	}

	return FALSE;
}



#ifdef LOCALE_JAPAN

bool AnalizeArgument( const char *lpArg)
{
	string strString;
	string strStatIndex;

	char ch;
	bool bSeperate = false;

	while ( *lpArg != 0)
	{
		ch = *lpArg++;

		if (ch == '\"')
			continue;
		else if (ch == ' ')
		{
			if ( bSeperate == true)
				return false;

			bSeperate = true;
		}
		else if ( bSeperate == false)
			strString += ch;
		else
			strStatIndex += ch;
	}


	if ( strString.empty() || strStatIndex.empty())
		return false;


	ZGameOnJPAuthInfo* pAuth = (ZGameOnJPAuthInfo*)ZGetLocale()->GetAuthInfo();

	if ( pAuth->ParseVersion() == false)
		return false;

	pAuth->SetString( strString.c_str());
	pAuth->SetStatIndex( strStatIndex.c_str());


	return true;
}

bool GameOnJPParseArguments( const char* pszArgs)
{
	return AnalizeArgument( pszArgs);
}

#endif