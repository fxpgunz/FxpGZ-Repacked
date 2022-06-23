#include "stdafx.h"
#include "MMatchShutdown.h"
#include "MMatchServer.h"
#include "MSharedCommandTable.h"


MMatchShutdown::~MMatchShutdown()
{
	for (vector<MShutdownNotify*>::iterator i = m_ShutdownNotifyArray.begin(); i != m_ShutdownNotifyArray.end(); ++i)
	{
		delete (*i);
	}

	m_ShutdownNotifyArray.clear();
}

bool MMatchShutdown::LoadXML_ShutdownNotify(const char* pszFileName)
{
	MZFile mzf;

	if (!mzf.Open(pszFileName))
		return false;

	char* buffer = new char[mzf.GetLength() + 1];
	buffer[mzf.GetLength()] = 0;
	if (!mzf.Read(buffer, mzf.GetLength()))
		return false;

	rapidxml::xml_document<> doc;
	if (!doc.parse<0>(buffer)) {
		mlog("error parsing %s", pszFileName);
		delete[] buffer;
		return false;
	}

	rapidxml::xml_node<>* rootNode = doc.first_node();
	for (auto itor = rootNode->first_node(); itor; itor = itor->next_sibling())
	{
		if (itor->name()[0] == '#')
			continue;
		
		if (_stricmp(itor->name(), "SHUTDOWNNOTIFY") == 0)
			m_ShutdownNotifyArray.push_back(new MShutdownNotify(atoi(itor->first_attribute("delay")->value()), itor->value()));
	}

	delete[] buffer;
	doc.clear();
	mzf.Close();
	return true;
}

void MMatchShutdown::Start(unsigned long nClock)
{
	m_bShutdown = true;
	m_nProgressIndex = 0;
	m_nTimeLastProgress = nClock;
}

void MMatchShutdown::SetProgress(int nIndex, unsigned long nClock)
{
	m_nProgressIndex = nIndex;
	m_nTimeLastProgress = nClock;
}

void MMatchShutdown::Notify(int nIndex)
{
	char* pszMsg = m_ShutdownNotifyArray[nIndex]->GetString();
	MMatchServer* pServer = MMatchServer::GetInstance();
	
	MCommand* pCmd = pServer->CreateCommand(MC_ADMIN_ANNOUNCE, MUID(0,0));
	pCmd->AddParameter(new MCmdParamUID(MUID(0,0)));
	pCmd->AddParameter(new MCmdParamStr(pszMsg));
	pCmd->AddParameter(new MCmdParamUInt(ZAAT_CHAT));
	pServer->RouteToAllClient(pCmd);

#ifdef _DEBUG
	mlog( "MMatchShutdown::Notify - Notify : %s\n", pszMsg );
#endif
}

void MMatchShutdown::Terminate()
{
	MMatchServer* pServer = MMatchServer::GetInstance();
	pServer->Shutdown();
}

void MMatchShutdown::OnRun(unsigned long nClock)
{
	if (IsShutdown() == false) return;

	int nIndex = GetProgressIndex();
	if (nIndex < (int)m_ShutdownNotifyArray.size())
	{
		MShutdownNotify* pNotify = m_ShutdownNotifyArray[nIndex];
		if (nClock - GetTimeLastProgress() < pNotify->GetDelay())
			return;
		Notify(nIndex);
		SetProgress(nIndex+1, nClock);
	}
	else if (nIndex == m_ShutdownNotifyArray.size())
	{
		Terminate();
		SetProgress(nIndex+1, nClock);
	}
}
