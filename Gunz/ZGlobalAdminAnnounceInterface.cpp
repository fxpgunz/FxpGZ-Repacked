#include "stdafx.h"
#include "ZGlobalAdminAnnounceInterface.h"

ZGlobalAdminAnnounceInterface::ZGlobalAdminAnnounceInterface()
{
	m_dwTime = 0;
	m_dwTimeDelay = 0;
	Clear();
}

void ZGlobalAdminAnnounceInterface::Clear()
{
	m_szGlobalMsg[0] = NULL;
}

void ZGlobalAdminAnnounceInterface::DrawGlobalAdminAnnounce(MLabel* pLabel) // lobby
{
	if (m_dwTimeDelay == 0)
		return;

	if ((timeGetTime() - m_dwTime) < m_dwTimeDelay)
	{
		//if (pLabel->IsVisible() == false)
		{
			char szMsg[512] = { 0 };
			sprintf(szMsg, "%s", GetGlobalMessage());

			if (strlen(szMsg) <= 0)
				return;

			pLabel->SetText(szMsg);
			pLabel->SetVisible(true);
		}
	}
	else
	{
		m_dwTimeDelay = 0;
		pLabel->SetVisible(false);
		Clear();
	}

	/*

	double nTime = (timeGetTime() / 100) % 200;
	char szMsg[512] = { 0 };

	sprintf(szMsg, "%s", GetGlobalMessage());

	pLabel->SetText(szMsg);

	if (nTime == 199)
	{
	ZGetGameClient()->GetGlobalAnnounceInterface()->Clear();
	nTime = (timeGetTime() / 100) % 200;
	}*/
}

void ZGlobalAdminAnnounceInterface::DrawGlobalAdminAnnounce(MDrawContext* pDC) // ingame 
{
	if (m_dwTimeDelay == 0)
		return;

	if ((timeGetTime() - m_dwTime) < m_dwTimeDelay)
	{
		MFont *pFont = MFontManager::Get("FONTa10_O2Wht");
		if (pFont == NULL)
			return;

		char szMsg[512] = { 0 };
		sprintf(szMsg, "%s", GetGlobalMessage());

		if (strlen(szMsg) <= 0)
			return;

		pDC->SetFont(pFont);
		pDC->SetColor(MCOLOR(0xFF00FF00));
		TextRelative(pDC, 350.f / 800.f, 23.f / 600.f, szMsg);
	}
	else
	{
		m_dwTime = 0;
		m_dwTimeDelay = 0;
		Clear();
	}
}