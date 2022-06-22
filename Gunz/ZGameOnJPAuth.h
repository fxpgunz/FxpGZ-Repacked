#ifndef _ZGAMEONJP_H
#define _ZGAMEONJP_H


#include "ZBaseAuthInfo.h"
#include <string>
using namespace std;

#define TITLE_PUBLAGENT					"D7CD782E-A5B6-471d-9564-72F87BA18BDF"
#define GET_MSG_HWND_TERMINATE_PUBGAME	"FDAC8B61-2915-4137-81BA-CE1BDC1D50E4"

class ZGameOnJPAuthInfo : public ZBaseAuthInfo
{
private:
	bool	m_bAlpha;
	string	m_strString;
	string	m_strStatIndex;


public:
	ZGameOnJPAuthInfo();
	virtual ~ZGameOnJPAuthInfo()		{}

	bool ParseVersion();

	void SetString( const char* szString);
	const char* GetString();

	void SetStatIndex( const char* szStatIndex);
	const char* GetStatIndex();

	bool IsAlpha()						{ return ( (m_bAlpha == true)  ?  true : false);	}
	bool IsReal()						{ return ( (m_bAlpha == false) ?  true : false);	}

    LRESULT SendMsgToLauncher(LPCSTR lpMsg);
	LRESULT NewLogin(WPARAM wparam, LPARAM lparam); 
	
	static HWND m_hLauncher;
};



#ifdef LOCALE_JAPAN

bool GameOnJPParseArguments(const char* pszArgs);

#endif



#endif