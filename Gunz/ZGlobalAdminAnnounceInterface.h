#ifndef _ZGLOBALADMINANNOUNCEINTERFACE_H
#define _ZGLOBALADMINANNOUNCEINTERFACE_H

#include <string>
using namespace std;

class MDrawContext;

#define TIME_DELAY	5000 // 5sec

class ZGlobalAdminAnnounceInterface {
protected:
	char			m_szGlobalMsg[128];
	DWORD			m_dwTime;
	DWORD			m_dwTimeDelay;

public:
	ZGlobalAdminAnnounceInterface();
	~ZGlobalAdminAnnounceInterface()					{ Clear(); }

	const char* GetGlobalMessage()						{ return m_szGlobalMsg; }
	void SetGlobalMessage(const char* pszMessage)		{ strcpy(m_szGlobalMsg, pszMessage); m_dwTime = timeGetTime(); m_dwTimeDelay = TIME_DELAY; }
	void Clear();

	void DrawGlobalAdminAnnounce(MLabel* pLabel);
	void DrawGlobalAdminAnnounce(MDrawContext* pDC);
};

#endif