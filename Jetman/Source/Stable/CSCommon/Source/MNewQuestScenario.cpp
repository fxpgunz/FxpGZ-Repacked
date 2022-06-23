#include "stdafx.h"
#include "MNewQuestScenario.h"

bool MNewQuestSector::AddSpawn( MNewQuestSpawn* pSpawn )
{
	if (!pSpawn) { _ASSERT(0); return false; }

	// 이미 존재하는 이름이면 실패
	ItorSpawn it = m_mapSpawn.find(pSpawn->GetPosTag());
	if (it != m_mapSpawn.end())
		return false;

	m_mapSpawn[pSpawn->GetPosTag()] = pSpawn;
	return true;
}

MNewQuestSpawn* MNewQuestSector::GetSpawnByIndex(int index)
{
	if (index < 0 || (int)m_mapSpawn.size() <= index) { _ASSERT(0); return NULL; }

	ItorSpawn it = m_mapSpawn.begin();
	for (int i=0; i<index; ++i)
		++it;

	return it->second;
}


MNewQuestScenarioManager::MNewQuestScenarioManager()
: m_pDropTable(0)
{

}

MNewQuestScenarioManager::~MNewQuestScenarioManager()
{
	Clear();
}

void MNewQuestScenarioManager::Clear()
{
	for (ItorScenario it=m_mapScenario.begin(); it!=m_mapScenario.end(); ++it)
		delete it->second;
	m_mapScenario.clear();
}

bool MNewQuestScenarioManager::AddScenario(MNewQuestScenario* pScenario)
{
	if (!pScenario) { _ASSERT(0); return false; }

	// 이미 존재하는 이름이면 실패
	if (GetScenario(pScenario->GetName()))
		return false;

	m_mapScenario[pScenario->GetName()] = pScenario;
	return true;
}

MNewQuestScenario* MNewQuestScenarioManager::GetScenario(const char* szName)
{
	ItorScenario it = m_mapScenario.find(szName);
	if (it != m_mapScenario.end())
		return it->second;
	return NULL;
}

void MNewQuestScenarioManager::GetScenarioNameList( vector<string>& out )
{
	out.clear();
	for (ItorScenario it=m_mapScenario.begin(); it!=m_mapScenario.end(); ++it)
		out.push_back(it->second->GetName());
}

const char* MNewQuestScenarioManager::GetDefaultScenarioName()
{
	if (m_mapScenario.empty())
	{
		_ASSERT(0);
		return "";
	}

	//todok 가장 낮은 레벨의 시나리오를 반환하도록 수정해야 한다
	return m_mapScenario.begin()->second->GetName();
}

bool MNewQuestScenarioManager::ReadXml(MZFileSystem* pFileSystem, const char* szFileName, MQuestDropTable* pDropTable)
{
	m_pDropTable = pDropTable;

	MZFile mzf;

	if(pFileSystem) {
		if(!mzf.Open(szFileName,pFileSystem))  {
			if(!mzf.Open(szFileName))  {
				return false;
			}
		}
	} 
	else  {

		if(!mzf.Open(szFileName)) {
			return false;
		}
	}

	char* buffer = new char[mzf.GetLength()+1];
	buffer[mzf.GetLength()] = 0;

	mzf.Read(buffer,mzf.GetLength());

	bool bResult = ParseXmlFromStr(buffer);

	delete[] buffer;
	mzf.Close();

	return bResult;
}

bool MNewQuestScenarioManager::ParseXmlFromStr(const char* szXmlContent)
{
	Clear();
	bool bResult = true;


	if (!szXmlContent) return NULL;

	rapidxml::xml_document<> doc;
	doc.parse<0>(const_cast<char*>(szXmlContent));

	rapidxml::xml_node<>* rootNode = doc.first_node();
	for (auto it = rootNode->first_node(); it; it = it->next_sibling())
	{
		if (_stricmp(it->name(), "SCENARIO") == 0)
		{
			MNewQuestScenario* pScenario = ParseScenario(it);
			if (!pScenario)
			{
				bResult = false;
			}


			if (!AddScenario(pScenario))
			{
				mlog("ERROR: xml scenario node name duplicated.");
				delete pScenario;
				bResult = false;
			}
		}
	}
	if (!bResult)
		Clear();

	doc.clear();
	return bResult;
}

MNewQuestScenario* MNewQuestScenarioManager::ParseScenario(rapidxml::xml_node<>* elem)
{
	MNewQuestScenario* pScenario = new MNewQuestScenario;
	rapidxml::xml_attribute<>* attributes = elem->first_attribute("map_id");
	pScenario->SetMapID(atoi(attributes->value()));
	pScenario->SetName(attributes->next_attribute("name")->value());
	if (strlen(pScenario->GetName()) == 0) {
		delete pScenario;
		mlog("ERROR: xml scenario node has no name attr.\n"); _ASSERT(0); return NULL;
	}
	pScenario->SetRewardItem(atoi(attributes->next_attribute("reward_item")->value()));
	pScenario->SetMaxPlayers(atoi(attributes->next_attribute("players")->value()));
	pScenario->SetLevelLimit(atoi(attributes->next_attribute("level_limit")->value()));
	pScenario->SetDesc(attributes->next_attribute("Description")->value());

	rapidxml::xml_node<>* childNode = elem->first_node();
	for (auto it = childNode; it; it = it->next_sibling())
	{
		if (it->name()[0] == '#')
			continue;

		if (_stricmp(it->name(), "SECTOR") == 0)
		{
			MNewQuestSector* pSector = ParseSector(it);
			if (!pSector)
			{
				delete pScenario;
				return NULL;
			}
			pScenario->AddSector(pSector);
		}
	}

	return pScenario;
}

MNewQuestSector* MNewQuestScenarioManager::ParseSector(rapidxml::xml_node<>* elem)
{
	MNewQuestSector* pSector = new MNewQuestSector;
	rapidxml::xml_attribute<>* attributes = elem->first_attribute("map");
	if (0 == strcmp(attributes->value(),"")) {
		mlog("ERROR: xml scenario sector node has no map attr.\n"); _ASSERT(0); return NULL;
	}
	pSector->SetMap(attributes->value());
	pSector->SetRewardXp(atoi(attributes->next_attribute("xp")->value()));
	pSector->SetRewardBp(atoi(attributes->next_attribute("bp")->value()));

	for (auto itor = elem->first_node(); itor; itor = itor->next_sibling())
	{
		if (itor->name()[0] == '#')
			continue;

		if (_stricmp(itor->name(), "SPAWN") == 0)
		{
			MNewQuestSpawn* pSpawn = ParseSpawn(itor);
			if (!pSpawn) {
				delete pSector;
				return NULL;
			}
			pSector->AddSpawn(pSpawn);
		}
	}

	return pSector;
}

MNewQuestSpawn* MNewQuestScenarioManager::ParseSpawn(rapidxml::xml_node<>* elem)
{
	MNewQuestSpawn* pSpawn = new MNewQuestSpawn;

	rapidxml::xml_attribute<>* attributes = elem->first_attribute("postag");
	if (attributes == nullptr) {
		mlog("ERROR: xml scenario sector spawn node has no postag attr.\n"); _ASSERT(0); return NULL;
	}
	pSpawn->SetPosTag(attributes->value());
	pSpawn->SetNum(atoi(attributes->next_attribute("num")->value()));
	pSpawn->SetActor(attributes->next_attribute("actor")->value());
	char itemName[256];
	strcpy_s(itemName,sizeof(itemName), attributes->next_attribute("drop")->value());
	int dropTableId = -1;
	if (m_pDropTable)
		dropTableId = m_pDropTable->GetIdByName(itemName);
	pSpawn->SetDropTableId(dropTableId);

	return pSpawn;
}