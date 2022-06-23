#include "stdafx.h"
#include "MQuestDropTable.h"
#include "MQuestItem.h"
#include "MMath.h"
#include "MZFileSystem.h"

void MQuestDropSet::AddItem(MQuestDropItem* pItem, int nQL, float fRate)
{
	if ((pItem->nDropItemType == QDIT_ZITEM) && (pItem->nRentPeriodHour == 0))
	{
		_ASSERT(0);	// 기간 설정이 안되어 있다.
		return;
	}

	if (fRate > 1.0f) 
	{
		_ASSERT(0);		// 비율이 잘못되어 있다.
		return;
	}

	int nRate = (int)(fRate * MAX_DROPSET_RATE);

	int idx = m_nTop[nQL];
	for (int i=0; i < nRate; i++)
	{
		if (idx < MAX_DROPSET_RATE)
		{
			m_DropItemSet[nQL][idx].Assign(pItem);

			m_nTop[nQL]++;
			idx++;
		}
	}

	if (pItem->nDropItemType == QDIT_QUESTITEM)
	{
		m_QuestItems.insert(pItem->nID);
	}
}


bool MQuestDropSet::Roll(MQuestDropItem& outDropItem, int nQL)
{
	int idx = RandomNumber(0, MAX_DROPSET_RATE-1);
	if (idx >= m_nTop[nQL]) return false;

	outDropItem.Assign(&m_DropItemSet[nQL][idx]);

	return true;
}


//////////////////////////////////////////////////////////////////////////////////

MQuestDropTable::MQuestDropTable()
{

}

MQuestDropTable::~MQuestDropTable()
{
	Clear();
}


//////////////////////////////////////////////////////

#define MTOK_DROPSET						"DROPSET"

// 기본정보
#define MTOK_DROPSET_ITEMSET				"ITEMSET"
#define MTOK_DROPSET_ITEM					"ITEM"
//#define MTOK_DROPSET_ZITEM					"ZITEM"
//#define MTOK_DROPSET_QUESTITEM				"QUESTITEM"
//#define MTOK_DROPSET_GAMEITEM				"GAMEITEM"

#define MTOK_DROPSET_ATTR_ID				"id"
#define MTOK_DROPSET_ATTR_QL				"QL"
#define MTOK_DROPSET_ATTR_NAME				"name"
#define MTOK_DROPSET_ATTR_RATE				"rate"
#define MTOK_DROPSET_ATTR_RENT_PERIOD		"rent_period"

//////////////////////////////////////////////////////

bool MQuestDropTable::ReadXml(const char* szFileName, MZFileSystem* pFileSystem)
{
	rapidxml::xml_document<> doc;

	MZFile mzf;
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

	char* buffer = new char[mzf.GetLength() + 1];
	buffer[mzf.GetLength()] = 0;
	mzf.Read(buffer, mzf.GetLength());
	if (!doc.parse<0>(buffer)) {
		mlog("error parsing droptable.xml");
		delete[] buffer;
		return false;
	}

	rapidxml::xml_node<>* rootNode = doc.first_node();
	for (auto itor = rootNode->first_node(); itor; itor = itor->next_sibling())
	{
		if (itor->name()[0] == '#')
			continue;

		if (_stricmp(itor->name(), MTOK_DROPSET) == 0)
			ParseDropSet(itor);
	}

	delete[] buffer;
	doc.clear();
	mzf.Close();

	return true;

}


void MQuestDropTable::ParseDropSet(rapidxml::xml_node<>* element)
{
	int nQL = -1;

	MQuestDropSet* pDropSet = new MQuestDropSet();

	pDropSet->SetID(atoi(element->first_attribute(MTOK_DROPSET_ATTR_ID)->value()));
	pDropSet->SetName(element->first_attribute(MTOK_DROPSET_ATTR_NAME)->value());

	rapidxml::xml_node<>* childNode = element->first_node();
	for (auto itor = childNode; itor; itor = itor->next_sibling())
	{
		if (itor->name()[0] == '#')
			continue;
		if (_stricmp(itor->name(), MTOK_DROPSET_ITEMSET) == 0)
		{
			if (itor->first_attribute(MTOK_DROPSET_ATTR_QL) != nullptr)
				nQL = atoi(itor->first_attribute(MTOK_DROPSET_ATTR_QL)->value());

			rapidxml::xml_node<>* itemSetChild = itor->first_node();
			for (auto it = itemSetChild; it; it = it->next_sibling())
			{
				if (it->name()[0] == '#')
					continue;
				if (_stricmp(it->name(), MTOK_DROPSET_ITEM) == 0)
				{
					MQuestDropItem item;
					float fRate = 0.0f;
					int nRentPeriodHour = 0;



					if (it->first_attribute(MTOK_DROPSET_ATTR_ID) != nullptr)
						ParseDropItemID(&item, it->first_attribute(MTOK_DROPSET_ATTR_ID)->value());

					if (it->first_attribute(MTOK_DROPSET_ATTR_RATE) != nullptr)
						fRate = strtof(it->first_attribute(MTOK_DROPSET_ATTR_RATE)->value(),nullptr);

					if (it->first_attribute(MTOK_DROPSET_ATTR_RENT_PERIOD) != nullptr)
						nRentPeriodHour = atoi(it->first_attribute(MTOK_DROPSET_ATTR_RENT_PERIOD)->value());

					if (nQL >= 0)
					{
						pDropSet->AddItem(&item, nQL, fRate);
					}
				}
			}
		}
	}
	insert(value_type(pDropSet->GetID(), pDropSet));
}


void MQuestDropTable::ParseDropItemID(MQuestDropItem* pItem, const char* szAttrValue)
{
	// id는 그냥 하드코딩..-_-ㅋ
	if (!_stricmp(szAttrValue, "hp1"))
	{
		pItem->nDropItemType = QDIT_WORLDITEM;
		pItem->nID = 1;
	}
	else if (!_stricmp(szAttrValue, "hp2"))
	{
		pItem->nDropItemType = QDIT_WORLDITEM;
		pItem->nID = 2;
	}
	else if (!_stricmp(szAttrValue, "hp3"))
	{
		pItem->nDropItemType = QDIT_WORLDITEM;
		pItem->nID = 3;
	}
	else if (!_stricmp(szAttrValue, "ap1"))
	{
		pItem->nDropItemType = QDIT_WORLDITEM;
		pItem->nID = 4;
	}
	else if (!_stricmp(szAttrValue, "ap2"))
	{
		pItem->nDropItemType = QDIT_WORLDITEM;
		pItem->nID = 5;
	}
	else if (!_stricmp(szAttrValue, "ap3"))
	{
		pItem->nDropItemType = QDIT_WORLDITEM;
		pItem->nID = 6;
	}
	else if (!_stricmp(szAttrValue, "mag1"))
	{
		pItem->nDropItemType = QDIT_WORLDITEM;
		pItem->nID = 7;
	}
	else if (!_stricmp(szAttrValue, "mag2"))
	{
		pItem->nDropItemType = QDIT_WORLDITEM;
		pItem->nID = 8;
	}
	else
	{
		// 만약 위에꺼가 아니면 아이템
		int nID = atoi(szAttrValue);

		if (IsQuestItemID(nID))
		{
			// 퀘스트 아이템
			MQuestItemDesc* pQuestItemDesc = GetQuestItemDescMgr().FindQItemDesc(nID);
			if (pQuestItemDesc)
			{
				pItem->nDropItemType = QDIT_QUESTITEM;
				pItem->nID = nID;
			}
			else
			{
				_ASSERT(0);
			}
		}
		else
		{
			// 일반 아이템
			MMatchItemDesc* pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(nID);
			if (pItemDesc)
			{
				pItem->nDropItemType = QDIT_ZITEM;
				pItem->nID = nID;
			}
			else
			{
				_ASSERT(0);
			}
		}
	}
}




bool MQuestDropTable::Roll(MQuestDropItem& outDropItem, int nDropTableID, int nQL)
{
	iterator itor = find(nDropTableID);
	if (itor != end())
	{
		MQuestDropSet* pDropSet = (*itor).second;
		return pDropSet->Roll(outDropItem, nQL);
	}

	return false;
}

void MQuestDropTable::Clear()
{
	for (iterator itor = begin(); itor != end(); ++itor)
	{
		delete (*itor).second;
	}

	clear();

}

MQuestDropSet* MQuestDropTable::Find(int nDropTableID)
{
	iterator itor = find(nDropTableID);
	if (itor != end())
	{
		return (*itor).second;
	}

	return NULL;
}

int MQuestDropTable::GetIdByName(const char* szDropTableName)
{
	MQuestDropSet* pDropSet = NULL;
	for (iterator it = begin(); it != end(); ++it)
	{
		pDropSet = it->second;

		if (0 == _stricmp(pDropSet->GetName(), szDropTableName))
			return pDropSet->GetID();
	}
	return -1;
}