#include "stdafx.h"
#include "MBaseChannelRule.h"
#include "MZFileSystem.h"



void MChannelRuleMapList::Add(string strMapName)
{
	for (int i = 0; i < MMATCH_MAP_MAX; i++)
	{
		if (!_stricmp(strMapName.c_str(), MGetMapDescMgr()->GetMapName(i)))
		{
			Add(i);
			return;
		}
	}
}

bool MChannelRuleMapList::Exist(int nMapID, bool bOnlyDuel)
{
	set<int>::iterator itor = m_Set.find( nMapID);

	if ( itor != m_Set.end())
	{
		int id = (*itor);

		if ( !bOnlyDuel && MGetMapDescMgr()->IsMapOnlyDuel(id))
			return false;

		return true;
	}

	return false;
}


bool MChannelRuleMapList::Exist(const char* pszMapName, bool bOnlyDuel)
{
	for (set<int>::iterator itor = m_Set.begin(); itor != m_Set.end(); ++itor)
	{
		int id = (*itor);

		if ((id >= 0) && (id < MMATCH_MAP_MAX))
		{
			if ( !_stricmp(pszMapName, MGetMapDescMgr()->GetMapName(id)) )
			{
				if ( !bOnlyDuel && MGetMapDescMgr()->IsMapOnlyDuel(id))
					return false;

				return true;
			}
		}
	}

	return false;
}

bool MChannelRuleMapList::ExistAsCTF(int nMapID)
{
	set<int>::iterator itor = m_Set.find( nMapID);

	if ( itor != m_Set.end())
	{
		int id = (*itor);

		if (MGetMapDescMgr()->IsCTFMap(id))
			return true;

		return false;
	}

	return false;
}

bool MChannelRuleMapList::ExistAsCTF(const char* pszMapName)
{
	for (set<int>::iterator itor = m_Set.begin(); itor != m_Set.end(); ++itor)
	{
		int id = (*itor);

if ((id >= 0) && (id < MMATCH_MAP_MAX))
{
	if (!_stricmp(pszMapName, MGetMapDescMgr()->GetMapName(id)))
	{
		if (MGetMapDescMgr()->IsCTFMap(id))
			return true;

		return false;
	}
}
	}

	return false;
}
#define MTOK_CHANNELRULE				"CHANNELRULE"
#define MTOK_CHANNELMAP					"MAP"
#define MTOK_CHANNELRULE_GAMETYPE		"GAMETYPE"
#define MTOK_CHANNELRULE_ATTR_ID		"id"
#define MTOK_CHANNELRULE_ATTR_DEFAULT	"default"
#define MTOK_CHANNELRULE_ATTR_NAME		"name"


MChannelRuleMgr::MChannelRuleMgr()
{

}

MChannelRuleMgr::~MChannelRuleMgr()
{
	Clear();
}


void MChannelRuleMgr::Clear()
{
	m_RuleTypeMap.clear();

	while (!empty())
	{
		MChannelRule* pRule = (*begin()).second;
		delete pRule; pRule = NULL;
		erase(begin());
	}
}

MChannelRule* MChannelRuleMgr::GetRule(const string& strName)
{
	iterator itor = find(strName);
	if (itor != end())
	{
		return (*itor).second;
	}
	return NULL;
}

MChannelRule* MChannelRuleMgr::GetRule(MCHANNEL_RULE nChannelRule)
{
	map<MCHANNEL_RULE, MChannelRule*>::iterator itor = m_RuleTypeMap.find(nChannelRule);
	if (itor != m_RuleTypeMap.end())
	{
		return (*itor).second;
	}
	return NULL;
}

void MChannelRuleMgr::ParseRoot(const char* szTagName, rapidxml::xml_node<>* pElement)
{
	if (!_stricmp(szTagName, MTOK_CHANNELRULE))
	{
		ParseRule(pElement);
	}
}


void MChannelRuleMgr::ParseRule(rapidxml::xml_node<>* pElement)
{

	MChannelRule* pRule = new MChannelRule;
	pRule->Init(atoi(pElement->first_attribute(MTOK_CHANNELRULE_ATTR_ID)->value()), pElement->first_attribute(MTOK_CHANNELRULE_ATTR_NAME)->value());

	int nDefaultCount = 0;

	rapidxml::xml_node<>* childNode = pElement->first_node();
	for (auto itor = childNode; itor; itor = itor->next_sibling())
	{
		if (itor->name()[0] == '#')
			continue;

		if (_stricmp(itor->name(), MTOK_CHANNELMAP) == 0)
			pRule->AddMap(itor->first_attribute(MTOK_CHANNELRULE_ATTR_NAME)->value());

		if (_stricmp(itor->name(), MTOK_CHANNELRULE_GAMETYPE) == 0)
		{
			if (itor->first_attribute(MTOK_CHANNELRULE_ATTR_ID) != nullptr) {
				pRule->AddGameType(atoi(itor->first_attribute(MTOK_CHANNELRULE_ATTR_ID)->value()));
				nDefaultCount++;
				_ASSERT(pRule->CheckGameType(atoi(itor->first_attribute(MTOK_CHANNELRULE_ATTR_ID)->value())));
			}
			if (itor->first_attribute(MTOK_CHANNELRULE_ATTR_DEFAULT) != nullptr)
				pRule->SetDefault(nDefaultCount - 1);
		}

	}
	AddRule(pRule);
}


void MChannelRuleMgr::AddRule(MChannelRule* pRule)
{
	insert(value_type(pRule->GetName(), pRule));

	if (!_stricmp(MCHANNEL_RULE_NOVICE_STR, pRule->GetName()))
	{
		m_RuleTypeMap.insert(map<MCHANNEL_RULE, MChannelRule*>::value_type(MCHANNEL_RULE_NOVICE, pRule));
	}
	else if (!_stricmp(MCHANNEL_RULE_NEWBIE_STR, pRule->GetName()))
	{
		m_RuleTypeMap.insert(map<MCHANNEL_RULE, MChannelRule*>::value_type(MCHANNEL_RULE_NEWBIE, pRule));
	}
	else if (!_stricmp(MCHANNEL_RULE_ROOKIE_STR, pRule->GetName()))
	{
		m_RuleTypeMap.insert(map<MCHANNEL_RULE, MChannelRule*>::value_type(MCHANNEL_RULE_ROOKIE, pRule));
	}
	else if (!_stricmp(MCHANNEL_RULE_MASTERY_STR, pRule->GetName()))
	{
		m_RuleTypeMap.insert(map<MCHANNEL_RULE, MChannelRule*>::value_type(MCHANNEL_RULE_MASTERY, pRule));
	}
	else if (!_stricmp(MCHANNEL_RULE_ELITE_STR, pRule->GetName()))
	{
		m_RuleTypeMap.insert(map<MCHANNEL_RULE, MChannelRule*>::value_type(MCHANNEL_RULE_ELITE, pRule));
	}
	else if( !_stricmp(MCHANNEL_RULE_CHAMPION_STR, pRule->GetName()) )
	{
		m_RuleTypeMap.insert(map<MCHANNEL_RULE, MChannelRule*>::value_type(MCHANNEL_RULE_CHAMPION, pRule));
	}
	else if( !_stricmp(MCHANNEL_RULE_QUEST_STR, pRule->GetName()) )
	{
		m_RuleTypeMap.insert(map<MCHANNEL_RULE, MChannelRule*>::value_type(MCHANNEL_RULE_QUEST, pRule));
	}
	else if( !_stricmp(MCHANNEL_RULE_DUELTOURNAMENT_STR, pRule->GetName()) )
	{
		m_RuleTypeMap.insert(map<MCHANNEL_RULE, MChannelRule*>::value_type(MCHANNEL_RULE_DUELTOURNAMENT, pRule));
	}
	else if( !_stricmp(MCHANNEL_RULE_SET1_STR, pRule->GetName()) )
	{
		m_RuleTypeMap.insert(map<MCHANNEL_RULE, MChannelRule*>::value_type(MCHANNEL_RULE_SET1, pRule));
	}
	else if( !_stricmp(MCHANNEL_RULE_SET2_STR, pRule->GetName()) )
	{
		m_RuleTypeMap.insert(map<MCHANNEL_RULE, MChannelRule*>::value_type(MCHANNEL_RULE_SET2, pRule));
	}
	else if( !_stricmp(MCHANNEL_RULE_SET3_STR, pRule->GetName()) )
	{
		m_RuleTypeMap.insert(map<MCHANNEL_RULE, MChannelRule*>::value_type(MCHANNEL_RULE_SET3, pRule));
	}
	else if (!_stricmp("event", pRule->GetName()))
	{
		m_RuleTypeMap.insert(map<MCHANNEL_RULE, MChannelRule*>::value_type(MCHANNEL_RULE_EVENT, pRule));
	}
	else if (!_stricmp("classic", pRule->GetName()))
	{
		//unused now
	}
	else
	{
		_ASSERT(0);		// 그런 룰은 존재하지 않음.
	}

}