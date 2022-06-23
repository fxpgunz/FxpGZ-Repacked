#include "stdafx.h"
#include "MAgentConfig.h"
#include "MZFileSystem.h"


#define AGENTCONFIGFILE		"AgentConfig.xml"

MAgentConfig* MAgentConfig::m_pInstance = nullptr;


MAgentConfig* MAgentConfig::GetInstance()
{ 
	if (m_pInstance == nullptr) {
		m_pInstance = new MAgentConfig;
		m_pInstance->LoadFromXML(AGENTCONFIGFILE);
	}
	return m_pInstance; 
}

void MAgentConfig::ReleaseInstance()
{
	if (m_pInstance == nullptr) {
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

MAgentConfig::~MAgentConfig()
{
	if (m_pInstance == nullptr) {
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

MAgentConfig::MAgentConfig()
{ 
	ZeroMemory(this, sizeof(MAgentConfig)); 
}

bool MAgentConfig::LoadFromXML(const char* pszFileName)
{

	MZFile			mzFile;
	if (!mzFile.Open(pszFileName))
		return false;

	char* buffer = new char[mzFile.GetLength() + 1];
	buffer[mzFile.GetLength()] = 0;
	mzFile.Read(buffer, mzFile.GetLength());
	rapidxml::xml_document<> doc;
	if (!doc.parse<0>(buffer)) {
		delete[] buffer;
		doc.clear();
		return false;
	}

	rapidxml::xml_node<>* rootNode = doc.first_node();
	for (auto itor = rootNode->first_node(); itor; itor = itor->next_sibling())
	{
		if (itor->name()[0] == '#')
			continue;

		if (stricmp(itor->name(), ATOK_AGENTCONFIG) == 0)
		{
			strcpy_s(m_szName, itor->first_node(ATOK_NAME)->value());
			m_nMaxClient = atoi(itor->first_node(ATOK_MAXCLIENT)->value());
			strcpy_s(m_szIP, itor->first_node(ATOK_ADDRESS)->first_attribute(ATOK_ATTR_IP)->value());
			m_nTCPPort = atoi(itor->first_node(ATOK_ADDRESS)->first_attribute(ATOK_ATTR_TCPPORT)->value());
			m_nUDPPort = atoi(itor->first_node(ATOK_ADDRESS)->first_attribute(ATOK_ATTR_UDPPORT)->value());
			strcpy_s(m_szMatchServerIP, itor->first_node(ATOK_MATCHSERVERADDRESS)->first_attribute(ATOK_ATTR_IP)->value());
			m_nMatchServerTCPPort = atoi(itor->first_node(ATOK_MATCHSERVERADDRESS)->first_attribute(ATOK_ATTR_TCPPORT)->value());
		}
	}

	delete[] buffer;
	doc.clear();
	mzFile.Close();

	return true;
}