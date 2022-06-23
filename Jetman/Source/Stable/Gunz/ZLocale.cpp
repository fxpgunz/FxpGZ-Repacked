#include "stdafx.h"
#include "ZLocale.h"
#include "ZConfiguration.h"
#include "ZNetmarble.h"			// test
#include "ZApplication.h"
#include "ZGlobal.h"
#include "ZSecurity.h"
#include "ZPost.h"


#ifdef LOCALE_NHNUSA
#include "ZNHN_USA.h"
#include "ZNHN_USA_Report.h"
#endif



ZLocale* ZLocale::GetInstance()
{
	static ZLocale m_stLocale;
	return &m_stLocale;
}

ZLocale::ZLocale() : MBaseLocale(), m_bTeenMode(false), m_pAuthInfo(NULL)
{

}

ZLocale::~ZLocale()
{
	if(m_pAuthInfo) delete m_pAuthInfo;
}

bool ZLocale::IsTeenMode()
{
//#ifdef _DEBUG
//	return true;
//#endif


	if (m_pAuthInfo) return m_pAuthInfo->GetTeenMode();

	return false;
}

void ZLocale::SetTeenMode(bool bTeenMode)
{
	if (m_pAuthInfo) m_pAuthInfo->SetTeenMode(bTeenMode);
	//else _ASSERT(0);
}

bool ZLocale::OnInit()
{
	if( !CreateAuthInfo() ) return false;

	return true;
}

bool ZLocale::ParseArguments(const char* pszArgs)
{
	switch (m_nCountry)
	{
	case MC_KOREA:
		{
			mlog("LOCALE:KOREA \n");
#ifdef LOCALE_KOREA
			if ( NetmarbleParseArguments( pszArgs))
			{
				ZApplication::GetInstance()->SetLaunchMode(ZApplication::ZLAUNCH_MODE_NETMARBLE);
				return true;
			}
			else
			{
				mlog( "Netmarble Certification Failed. Shutdown. \n");
				return false;
			}
#endif // LOCALE_KOREA
		}
		break;

	case MC_JAPAN:
		{
			mlog("LOCALE:JAPAN \n");
#ifdef LOCALE_JAPAN
			if ( GameOnJPParseArguments( pszArgs))
			{
				ZApplication::GetInstance()->SetLaunchMode(ZApplication::ZLAUNCH_MODE_GAMEON);
				return true;
			}
			else
			{
				mlog( "GameOn Certification Failed. Shutdown. \n");
				return false;
			}

#endif // LOCALE_JAPAN
		}
		break;

	case MC_NHNUSA :
		{
#ifdef LOCALE_NHNUSA
			if( NHN_USA_ParseArgument(pszArgs) )
			{
				// ZApplication::GetInstance()->SetLaunchMode( ZApplication::ZLAUNCH_MODE_NHNUSA );
				return true;
			}
			else 
			{
				mlog( "NHN Certification Failed. Shutdown. \n");
				return false;
			}
#endif // LOCALE_NHNUSA
		}
		break;

	default:
		{
			mlog("LOCALE:UNKNOWN \n");
		}
	};

	return true;
}

bool ZLocale::CreateAuthInfo()
{
	if (m_pAuthInfo) delete m_pAuthInfo;
	m_pAuthInfo = NULL;

	switch (m_nCountry)
	{
	case MC_KOREA:
		{
#ifdef LOCALE_KOREA
			m_pAuthInfo = new ZNetmarbleAuthInfo();
#endif // LOCALE_KOREA
		}
		break;
	case MC_JAPAN:
		{
#ifdef LOCALE_JAPAN
			m_pAuthInfo = new ZGameOnJPAuthInfo();
#endif // LOCALE_JAPAN
		}
		break;

	case MC_NHNUSA :
		{
#ifdef LOCALE_NHNUSA
			m_pAuthInfo = new ZNHN_USAAuthInfo();
			if( 0 == m_pAuthInfo ) return false;
			//if( !((ZNHN_USAAuthInfo*)m_pAuthInfo)->InitAuth() )
			//{
			//	delete m_pAuthInfo;
			//	m_pAuthInfo = 0;
			//	return false;
			//}

			return true;
#endif 
		}
		break;
	default:
		{

		}
	};

	return true;
}

void ZLocale::RouteToWebsite()
{
}

void ZLocale::PostLoginViaHomepage(MUID* pAllocUID)
{
	mlog("LoginViaHomepage - Unknown Locale \n");
}


