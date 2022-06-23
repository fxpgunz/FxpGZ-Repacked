#include "stdafx.h"
#include "MQuestScenario.h"
#include "MQuestFormula.h"
#include "MZFileSystem.h"
#include "stdlib.h"


MQuestScenarioCatalogue::MQuestScenarioCatalogue()
{
	m_nDefaultStandardScenarioID = CalcStandardScenarioID(0, 0);
}

MQuestScenarioCatalogue::~MQuestScenarioCatalogue()
{
	Clear();
}

MQuestScenarioInfo* MQuestScenarioCatalogue::GetInfo(int nScenarioID)
{
	iterator itor = find(nScenarioID);
	if (itor != end())
	{
		return (*itor).second;
	}

	return NULL;
}

void MQuestScenarioCatalogue::Clear()
{
	for (iterator itor = begin(); itor != end(); ++itor)
	{
		delete (*itor).second;
	}

	clear();
}

static int __cdecl _uint_r_sortfunc(const void* a, const void* b)
{
	return (int)(*((unsigned int*)b) - *((unsigned int*)a));
}

void MQuestScenarioCatalogue::Insert(MQuestScenarioInfo* pScenarioInfo)
{
	int nID = pScenarioInfo->nID;

	if ((nID <= 0) || (GetInfo(nID)))
	{
		_ASSERT(0);		// �ó����� ID�� �߸����.
		delete pScenarioInfo;
		return;
	}

	// reward ������ ����
	qsort(pScenarioInfo->nResSacriItemID, MAX_SCENARIO_SACRI_ITEM, sizeof(unsigned int), _uint_r_sortfunc);

	insert(value_type(nID, pScenarioInfo));
}

int MQuestScenarioCatalogue::GetStandardScenarioID(int nMapsetID, int nQL)
{
	if ((nQL < 0) || (nQL > MAX_QL)) return 0;
	//if ((nDice <= 0) || (nDice > SCENARIO_STANDARD_DICE_SIDES)) return 0;

	return CalcStandardScenarioID(nMapsetID, nQL);
	
}


bool MQuestScenarioCatalogue::FindSpecialScenarioID(int nMapsetID, int nPlayerQL, unsigned int* SacriQItemIDs, unsigned int* outScenarioID)
{
	// ������ �׳� ���� �˻�
	for (iterator itor = begin(); itor != end(); ++itor)
	{
		if ((*itor).first >= 10000) return false;

		MQuestScenarioInfo* pScenarioInfo = (*itor).second;
		if ((pScenarioInfo->nMapSet == nMapsetID)) //&& (nPlayerQL >= pScenarioInfo->nQL))
		{
			bool bSame = true;

			for (int i = 0;i < MAX_SCENARIO_SACRI_ITEM; i++)
			{
				if (SacriQItemIDs[i] != pScenarioInfo->nResSacriItemID[i]) 
				{
					bSame = false;
					continue;
				}
			}

			if (bSame)
			{
				*outScenarioID = (unsigned int)pScenarioInfo->nID;
				return true;
			}
		}
	}

	return false;
}



unsigned int MQuestScenarioCatalogue::MakeScenarioID(int nMapsetID, int nPlayerQL, unsigned int* SacriQItemIDs)
{
	unsigned int nSQItems[MAX_SCENARIO_SACRI_ITEM];
	for (int i = 0; i < MAX_SCENARIO_SACRI_ITEM; i++)
	{
		nSQItems[i] = SacriQItemIDs[i];
	}
	
	// ����
	qsort(nSQItems, MAX_SCENARIO_SACRI_ITEM, sizeof(unsigned int), _uint_r_sortfunc);

	unsigned int nOutScenarioID = 0;

	// Ư�� �ó����� �˻�
	if (FindSpecialScenarioID(nMapsetID, nPlayerQL, nSQItems, &nOutScenarioID))
	{
		return nOutScenarioID;
	}

	// �������� QL ���� - ������ �ٻ� ����� �ϵ��ڵ�..-_- by bird
	int nQL = 0;
	if( (nSQItems[0] == 0) && (nSQItems[1] == 0) )
	{
		if( 1 < nPlayerQL )
			nQL = 1;
		else
			nQL = nPlayerQL;
	}
	else if ((nSQItems[0] == 200001) && (nSQItems[1] == 0))
	{
		nQL = 2;
	}
	else if ((nSQItems[0] == 200002) && (nSQItems[1] == 0))
	{
		nQL = 3;
	}
	else if ((nSQItems[0] == 200003) && (nSQItems[1] == 0))
	{
		nQL = 4;
	}
	else if ((nSQItems[0] == 200004) && (nSQItems[1] == 0))
	{
		nQL = 5;
	}
	else if ((nSQItems[0] != 0) || (nSQItems[1] != 0))
	{
		// ���� �������� ������ �ش� �ó������� ���°�
		return 0;
	}

	// �ø� �������� QL�� �´��� �˻�
	if (nQL > nPlayerQL) return 0;

#ifdef _DEBUG_QUEST
	// ���� �ó����� �˻�
	nOutScenarioID= 100;
#else
	// ���� �ó����� �˻�
	nOutScenarioID= GetStandardScenarioID(nMapsetID, nQL);
#endif

	return nOutScenarioID;
}

//////////////////////////////////////////////////////

#define MTOK_SCENARIO					"SCENARIO"
#define MTOK_STANDARD_SCENARIO			"STANDARD_SCENARIO"
#define MTOK_SPECIAL_SCENARIO			"SPECIAL_SCENARIO"


#define MTOK_SCENARIO_ID				"id"
#define MTOK_SCENARIO_TITLE				"title"
#define MTOK_SCENARIO_QL				"QL"
#define MTOK_SCENARIO_DC				"DC"
#define MTOK_SCENARIO_MAPSET			"mapset"
#define MTOK_SCENARIO_KEY_SECTOR		"key_sector"
#define MTOK_SCENARIO_KEY_NPC			"key_npc"
#define MTOK_SCENARIO_BOSS				"boss"
#define MTOK_SCENARIO_NPC				"npc"
#define MTOK_SCENARIO_XP				"XP"
#define MTOK_SCENARIO_BP				"BP"
#define MTOK_SCENARIO_REPEAT			"repeat"				// �����̹� ���� (�ó����� �ݺ�Ƚ��)
#define MTOK_SCENARIO_MAX_SPAWN			"max_spawn"				// �����̹� ���� (NPC ���� Ƚ��)
#define MTOK_SCENARIO_MAX_SPAWN_SAMETIME "max_spawn_sametime"	// �����̹� ���� (NPC ���� ���� ����)
#define MTOK_SCENARIO_SECTOR_XP			"SECTOR_XP"
#define MTOK_SCENARIO_SECTOR_BP			"SECTOR_BP"
#define MTOK_SCENARIO_SACRI_ITEM		"SACRI_ITEM"
#define MTOK_SCENARIO_REWARD_ITEM		"REWARD_ITEM"
#define MTOK_SCENARIO_MAP				"MAP"
#define MTOK_SCENARIO_ITEMID			"itemid"
#define MTOK_SCENARIO_TYPE				"type"
#define MTOK_SCENARIO_RATE				"rate"
#define MTOK_SCENARIO_DICE				"dice"
#define MTOK_SCENARIO_NPCID				"npcid"
#define MTOK_SCENARIO_COUNT				"count"
#define MTOK_SCENARIO_TICK				"tick"
#define MTOK_SCENARIO_MIN_NPC			"min_npc"
#define MTOK_SCENARIO_MAX_NPC			"max_npc"

#define MTOK_SCENARIO_NPCSET_ARRAY		"NPCSET_ARRAY"
#define MTOK_SCENARIO_KEYNPC_ARRAY		"KEYNPC_ARRAY"		// �����̹� ���� (���ͺ� Ű���Ǿ� ����)
#define MTOK_SCENARIO_SECTOR_XP_ARRAY	"SECTOR_XP_ARRAY"	// �����̹� ���� (���ͺ� XP ����)
#define MTOK_SCENARIO_SECTOR_BP_ARRAY	"SECTOR_BP_ARRAY"	// �����̹� ���� (���ͺ� BP ����)
#define MTOK_SCENARIO_JACO				"JACO"
#define MTOK_SCENARIO_NPCSET			"NPCSET"
#define MTOK_SCENARIO_NAME				"name"

void MQuestScenarioCatalogue::ParseSpecialScenario(rapidxml::xml_node<>* element)
{
	MQuestScenarioInfo* pScenarioInfo = new MQuestScenarioInfo();
	pScenarioInfo->bSpecialScenario = true;

	if (element->first_attribute(MTOK_SCENARIO_ID))
		pScenarioInfo->nID = atoi(element->first_attribute(MTOK_SCENARIO_ID)->value());

	if (element->first_attribute(MTOK_SCENARIO_TITLE))
		strcpy_s(pScenarioInfo->szTitle, MGetStringResManager()->GetStringFromXml(element->first_attribute(MTOK_SCENARIO_TITLE)->value()));
	
	if (element->first_attribute(MTOK_SCENARIO_QL))
		pScenarioInfo->nQL = atoi(element->first_attribute(MTOK_SCENARIO_QL)->value());

	if (element->first_attribute(MTOK_SCENARIO_DC))
		pScenarioInfo->fDC = strtof(element->first_attribute(MTOK_SCENARIO_DC)->value(),nullptr);

	if (element->first_attribute(MTOK_SCENARIO_MAPSET))
	{
		if (_stricmp(element->first_attribute(MTOK_SCENARIO_MAPSET)->value(), "mansion") == 0)
			pScenarioInfo->nMapSet = 1;
		if (_stricmp(element->first_attribute(MTOK_SCENARIO_MAPSET)->value(), "prison") == 0)
			pScenarioInfo->nMapSet = 2;
		if (_stricmp(element->first_attribute(MTOK_SCENARIO_MAPSET)->value(), "dungeon") == 0)
			pScenarioInfo->nMapSet = 3;
	}
	if (element->first_attribute(MTOK_SCENARIO_XP))
		pScenarioInfo->nXPReward = atoi(element->first_attribute(MTOK_SCENARIO_XP)->value());
	if (element->first_attribute(MTOK_SCENARIO_BP))
		pScenarioInfo->nBPReward = atoi(element->first_attribute(MTOK_SCENARIO_BP)->value());
	if(element->first_attribute(MTOK_SCENARIO_SECTOR_XP))
		pScenarioInfo->nSectorXP = atoi(element->first_attribute(MTOK_SCENARIO_SECTOR_XP)->value());
	if (element->first_attribute(MTOK_SCENARIO_SECTOR_BP))
		pScenarioInfo->nSectorBP = atoi(element->first_attribute(MTOK_SCENARIO_SECTOR_BP)->value());

	rapidxml::xml_node<>* childNode = element->first_node();
	for (auto childNode = element->first_node(); childNode; childNode = childNode->next_sibling())
	{
		if (childNode->name()[0] == '#')
			continue;

		if (_stricmp(childNode->name(), MTOK_SCENARIO_SACRI_ITEM) == 0)
			ParseSacriItem(childNode, pScenarioInfo);

		if (_stricmp(childNode->name(), MTOK_SCENARIO_REWARD_ITEM) == 0)
			ParseRewardItem(childNode, pScenarioInfo);

		if (_stricmp(childNode->name(), MTOK_SCENARIO_MAP) == 0)
			ParseMap(childNode, pScenarioInfo);
	}

	Insert(pScenarioInfo);
}

void MQuestScenarioCatalogue::ParseStandardScenario(rapidxml::xml_node<>* element)
{
	MQuestScenarioInfo* pScenarioInfo = new MQuestScenarioInfo();
	pScenarioInfo->bSpecialScenario = false;

	if (element->first_attribute(MTOK_SCENARIO_ID))
		pScenarioInfo->nID = atoi(element->first_attribute(MTOK_SCENARIO_ID)->value());

	if (element->first_attribute(MTOK_SCENARIO_TITLE))
		strcpy_s(pScenarioInfo->szTitle, MGetStringResManager()->GetStringFromXml(element->first_attribute(MTOK_SCENARIO_TITLE)->value()));

	if (element->first_attribute(MTOK_SCENARIO_QL))
		pScenarioInfo->nQL = atoi(element->first_attribute(MTOK_SCENARIO_QL)->value());

	if (element->first_attribute(MTOK_SCENARIO_DC))
		pScenarioInfo->fDC = strtof(element->first_attribute(MTOK_SCENARIO_DC)->value(),nullptr);

	if (element->first_attribute(MTOK_SCENARIO_MAPSET))
	{
		if (_stricmp(element->first_attribute(MTOK_SCENARIO_MAPSET)->value(), "mansion") == 0)
			pScenarioInfo->nMapSet = 1;
		if (_stricmp(element->first_attribute(MTOK_SCENARIO_MAPSET)->value(), "prison") == 0)
			pScenarioInfo->nMapSet = 2;
		if (_stricmp(element->first_attribute(MTOK_SCENARIO_MAPSET)->value(), "dungeon") == 0)
			pScenarioInfo->nMapSet = 3;
	}
	if (element->first_attribute(MTOK_SCENARIO_XP))
		pScenarioInfo->nXPReward = atoi(element->first_attribute(MTOK_SCENARIO_XP)->value());
	if (element->first_attribute(MTOK_SCENARIO_BP))
		pScenarioInfo->nBPReward = atoi(element->first_attribute(MTOK_SCENARIO_BP)->value());
	if (element->first_attribute(MTOK_SCENARIO_SECTOR_XP))
		pScenarioInfo->nSectorXP = atoi(element->first_attribute(MTOK_SCENARIO_SECTOR_XP)->value());
	if (element->first_attribute(MTOK_SCENARIO_SECTOR_BP))
		pScenarioInfo->nSectorBP = atoi(element->first_attribute(MTOK_SCENARIO_SECTOR_BP)->value());
	if (element->first_attribute(MTOK_SCENARIO_REPEAT)) {
		pScenarioInfo->nRepeat = atoi(element->first_attribute(MTOK_SCENARIO_REPEAT)->value());
		_ASSERT(pScenarioInfo->nRepeat > 0);
	}
	if (element->first_attribute(MTOK_SCENARIO_MAX_SPAWN)) {
		pScenarioInfo->nMaxSpawn = atoi(element->first_attribute(MTOK_SCENARIO_MAX_SPAWN)->value());
		_ASSERT(pScenarioInfo->nMaxSpawn > 0);

	}
	if (element->first_attribute(MTOK_SCENARIO_MAX_SPAWN_SAMETIME)) {
		pScenarioInfo->nMaxSpawnSameTime = atoi(element->first_attribute(MTOK_SCENARIO_MAX_SPAWN_SAMETIME)->value());
		_ASSERT(pScenarioInfo->nMaxSpawnSameTime > 0);
	}

	for (auto childNode = element->first_node(); childNode; childNode = childNode->next_sibling())
	{
		if (childNode->name()[0] == '#')
			continue;

		if (_stricmp(childNode->name(), MTOK_SCENARIO_SACRI_ITEM) == 0)
			ParseSacriItem(childNode, pScenarioInfo);

		if (_stricmp(childNode->name(), MTOK_SCENARIO_REWARD_ITEM) == 0)
			ParseRewardItem(childNode, pScenarioInfo);

		if (_stricmp(childNode->name(), MTOK_SCENARIO_MAP) == 0)
			ParseMap(childNode, pScenarioInfo);
	}

	pScenarioInfo->nID = CalcStandardScenarioID(pScenarioInfo->nMapSet, pScenarioInfo->nQL);
	Insert(pScenarioInfo);
}

void MQuestScenarioCatalogue::ParseNPCSetArray(rapidxml::xml_node<>* element, vector<int>& vec)
{
	char szMaps[256];
	memset(szMaps, 0, sizeof(szMaps));
	MMatchQuest* pQuest = MMatchServer::GetInstance()->GetQuest();


	strcpy_s(szMaps, element->value());

	int nLen = (int)strlen(szMaps);

	for (int i = 0; i < nLen; i += 4)
	{
		char szNPCSet[4];
		memset(szNPCSet, 0, sizeof(szNPCSet));

		for (int j = 0; j < 3; j++)
		{
			szNPCSet[j] = szMaps[i+j];
		}

		// NPCSetCatalogue���� �ش� NPCSet�� ã�´�.
		MQuestNPCSetInfo* pNPCSetInfo = pQuest->GetNPCSetInfo(szNPCSet);
		if (pNPCSetInfo)
		{
			vec.push_back(pNPCSetInfo->nID);
		}
		else
		{
			// �´� NPCSet�� ����.
			_ASSERT(0);
		}

	}
}

void MQuestScenarioCatalogue::ParseKeyNPCArray(rapidxml::xml_node<>* element, vector<int>& vec)
{
	char szMaps[256];
	memset(szMaps, 0, sizeof(szMaps));

	strcpy_s(szMaps, element->value());

	int nLen = (int)strlen(szMaps);

	// ���� �Ľ��� �Ѵ�
	char* sz = strtok(szMaps, "/");
	if (sz) vec.push_back( atoi(sz) );
	while (sz != NULL)
	{
		sz = strtok(NULL, "/");
		if (sz) vec.push_back( atoi(sz) );
	}

	// npc ID�� �ùٸ��� �˻�
	MMatchQuest* pQuest = MMatchServer::GetInstance()->GetQuest();
	_ASSERT(pQuest);
	if (pQuest) 
	{
		for (unsigned int i=0; i<vec.size(); ++i)
		{
			if (!pQuest->GetNPCInfo(MQUEST_NPC(vec[i])))
			{
				vec[i] = NPC_NONE;
				_ASSERT(0);	// �ش� npc�� ����.
			}
		}
	}
}

void MQuestScenarioCatalogue::ParseSectorXpBpArray(rapidxml::xml_node<>* element, vector<int>& vec)
{
	char szMaps[256];
	memset(szMaps, 0, sizeof(szMaps));

	strcpy_s(szMaps, element->value());

	int nLen = (int)strlen(szMaps);

	char* sz = strtok(szMaps, "/");
	if (sz) vec.push_back( atoi(sz) );
	while (sz != NULL)
	{
		sz = strtok(NULL, "/");
		if (sz) vec.push_back( atoi(sz) );
	}
}

void MQuestScenarioCatalogue::ParseJaco(rapidxml::xml_node<>* element, MQuestScenarioInfoMaps* pMap)
{


	MQuestScenarioInfoMapJaco jaco;
	jaco.nNPCID = NPC_NONE;
	jaco.fRate = 0.0f;

	if (element->first_attribute(MTOK_SCENARIO_COUNT))
		pMap->nJacoCount = atoi(element->first_attribute(MTOK_SCENARIO_COUNT)->value());

	if (element->first_attribute(MTOK_SCENARIO_TICK))
		pMap->nJacoSpawnTickTime = atoi(element->first_attribute(MTOK_SCENARIO_TICK)->value()) * 1000;

	if (element->first_attribute(MTOK_SCENARIO_MIN_NPC))
		pMap->nJacoMinNPCCount = atoi(element->first_attribute(MTOK_SCENARIO_MIN_NPC)->value());

	if (element->first_attribute(MTOK_SCENARIO_MAX_NPC))
		pMap->nJacoMaxNPCCount = atoi(element->first_attribute(MTOK_SCENARIO_MAX_NPC)->value());

	for (auto childNode = element->first_node(); childNode; childNode = childNode->next_sibling())
	{
		if (childNode->name()[0] == '#')
			continue;

		if (_stricmp(childNode->name(), MTOK_SCENARIO_NPC) == 0)
		{
			MQuestScenarioInfoMapJaco jaco;
			jaco.nNPCID = NPC_NONE;
			jaco.fRate = 0.0f;

			if (childNode->first_attribute(MTOK_SCENARIO_NPCID))
				jaco.nNPCID = (MQUEST_NPC)atoi(childNode->first_attribute(MTOK_SCENARIO_NPCID)->value());

			if (childNode->first_attribute(MTOK_SCENARIO_RATE))
				jaco.fRate = strtof(childNode->first_attribute(MTOK_SCENARIO_RATE)->value(),nullptr);

			pMap->vecJacoArray.push_back(jaco);

		}
	}
}

void MQuestScenarioCatalogue::ParseSacriItem(rapidxml::xml_node<>* element, MQuestScenarioInfo* pScenarioInfo)
{
	int nIndex = pScenarioInfo->nResSacriItemCount;
	pScenarioInfo->nResSacriItemCount++;

	if (element->first_attribute(MTOK_SCENARIO_ITEMID))
		pScenarioInfo->nResSacriItemID[nIndex] = atoi(element->first_attribute(MTOK_SCENARIO_ITEMID)->value());
}

void MQuestScenarioCatalogue::ParseRewardItem(rapidxml::xml_node<>* element, MQuestScenarioInfo* pScenarioInfo)
{
	int nIndex = pScenarioInfo->nRewardItemCount;
	pScenarioInfo->nRewardItemCount++;

	if (element->first_attribute(MTOK_SCENARIO_TYPE)) {

	}
	if (element->first_attribute(MTOK_SCENARIO_ITEMID))
		pScenarioInfo->nRewardItemID[nIndex] = atoi(element->first_attribute(MTOK_SCENARIO_ITEMID)->value());

	if (element->first_attribute(MTOK_SCENARIO_RATE))
		pScenarioInfo->fRewardItemRate[nIndex] = strtof(element->first_attribute(MTOK_SCENARIO_RATE)->value(),nullptr);
}

void MQuestScenarioCatalogue::ParseMap(rapidxml::xml_node<>* element, MQuestScenarioInfo* pScenarioInfo)
{
	int nDiceNum=0;
	if (element->first_attribute(MTOK_SCENARIO_DICE))
		nDiceNum = atoi(element->first_attribute(MTOK_SCENARIO_DICE)->value());

	if (element->first_attribute(MTOK_SCENARIO_KEY_SECTOR))
		pScenarioInfo->Maps[nDiceNum - 1].nKeySectorID = atoi(element->first_attribute(MTOK_SCENARIO_KEY_SECTOR)->value());

	if (element->first_attribute(MTOK_SCENARIO_KEY_NPC))
	{
		pScenarioInfo->Maps[nDiceNum - 1].nKeyNPCID = atoi(element->first_attribute(MTOK_SCENARIO_KEY_NPC)->value());

		MMatchQuest* pQuest = MMatchServer::GetInstance()->GetQuest();
		if (pQuest)
		{
			if (!pQuest->GetNPCInfo(MQUEST_NPC(pScenarioInfo->Maps[nDiceNum - 1].nKeyNPCID)))
			{
				pScenarioInfo->Maps[nDiceNum - 1].nKeyNPCID = 0;
				_ASSERT(0);	// �ش� npc�� ����.
			}
		}
	}
	if (element->first_attribute(MTOK_SCENARIO_BOSS))
	{
		if (_stricmp(element->first_attribute(MTOK_SCENARIO_BOSS)->value(), "true") == 0)
			pScenarioInfo->Maps[nDiceNum - 1].bKeyNPCIsBoss = true;
		else
			pScenarioInfo->Maps[nDiceNum - 1].bKeyNPCIsBoss = false;
	}

	if ((nDiceNum <= 0) || (nDiceNum > SCENARIO_STANDARD_DICE_SIDES))
	{
		nDiceNum = 1;
		// �ֻ��� ���� �߸��ԷµǾ� �ִ�.
		_ASSERT(0);
	}

	for (auto childNode = element->first_node(); childNode; childNode = childNode->next_sibling())
	{
		if (childNode->name()[0] == '#')
			continue;

		if (_stricmp(childNode->name(), MTOK_SCENARIO_NPCSET_ARRAY) == 0)
			ParseNPCSetArray(childNode, pScenarioInfo->Maps[nDiceNum - 1].vecNPCSetArray);

		if (_stricmp(childNode->name(), MTOK_SCENARIO_JACO) == 0)
			ParseJaco(childNode, &pScenarioInfo->Maps[nDiceNum - 1]);

		if (_stricmp(childNode->name(), MTOK_SCENARIO_KEYNPC_ARRAY) == 0)
			ParseKeyNPCArray(childNode, pScenarioInfo->Maps[nDiceNum - 1].vecKeyNPCArray);

		if (_stricmp(childNode->name(), MTOK_SCENARIO_SECTOR_XP_ARRAY) == 0)
			ParseSectorXpBpArray(childNode, pScenarioInfo->Maps[nDiceNum - 1].vecSectorXpArray);

		if (_stricmp(childNode->name(), MTOK_SCENARIO_SECTOR_XP_ARRAY) == 0)
			ParseSectorXpBpArray(childNode, pScenarioInfo->Maps[nDiceNum - 1].vecSectorBpArray);
	}
	// keyNPCArray�� ���ǵǾ� �ִٸ� �����̹��� �ó��������״� NPCSetArray�� ������ �������� �˻�������
	if (!pScenarioInfo->Maps[nDiceNum-1].vecKeyNPCArray.empty())
	{
		size_t sizeNpcSetAry = pScenarioInfo->Maps[nDiceNum-1].vecNPCSetArray.size();
		size_t sizeKeyNpcAry = pScenarioInfo->Maps[nDiceNum-1].vecKeyNPCArray.size();
		_ASSERT(sizeNpcSetAry == sizeKeyNpcAry);

		// ���ͺ� XP/BP ������ ������ ���ƾ� �Ѵ�
		size_t sizeSectorXpAry = pScenarioInfo->Maps[nDiceNum-1].vecSectorXpArray.size();
		_ASSERT(sizeNpcSetAry == sizeSectorXpAry);

		size_t sizeSectorBpAry = pScenarioInfo->Maps[nDiceNum-1].vecSectorBpArray.size();
		_ASSERT(sizeNpcSetAry == sizeSectorBpAry);
	}
}


bool MQuestScenarioCatalogue::ReadXml(const char* szFileName, MZFileSystem* pFileSystem)
{
	MZFile mzf;
	rapidxml::xml_document<> doc;
	if (pFileSystem) {
		if (!mzf.Open(szFileName, pFileSystem)) {
			if (!mzf.Open(szFileName))
				return false;
		}
	}
	else {
		if (!mzf.Open(szFileName))
			return false;
	}

	char* buffer = new char[mzf.GetLength()+1];
	buffer[mzf.GetLength()] = 0;
	mzf.Read(buffer,mzf.GetLength());

	if (!doc.parse<0>(buffer)) {
		mlog("Error parsing %s\n", szFileName);
		delete[] buffer;
		return false;
	}

	rapidxml::xml_node<>* rootNode = doc.first_node();
	for (auto childNode = rootNode->first_node(); childNode; childNode = childNode->next_sibling())
	{
		if (childNode->name()[0] == '#')
			continue;

		if (_stricmp(childNode->name(), MTOK_SPECIAL_SCENARIO) == 0)
			ParseSpecialScenario(childNode);
		if (_stricmp(childNode->name(), MTOK_STANDARD_SCENARIO) == 0)
			ParseStandardScenario(childNode);
	}

	delete[] buffer;
	doc.clear();
	mzf.Close();
	return true;

}

int MQuestScenarioCatalogue::CalcStandardScenarioID(int nMapsetID, int nQL)
{
	if (nMapsetID <= 0) nMapsetID = 1;
	if ((nQL < 0) || (nQL > MAX_QL)) nQL = 0;

	return (10000 + (nMapsetID * 100) + (nQL));
}


