#ifndef _MBASECHANNELRULE_H
#define _MBASECHANNELRULE_H

#include "MXmlParser.h"
#include <list>
#include <map>
#include <set>
#include <string>
#include <algorithm>
using namespace std;



enum MCHANNEL_RULE {
	MCHANNEL_RULE_NOVICE=0,
	MCHANNEL_RULE_NEWBIE,
	MCHANNEL_RULE_ROOKIE,
	MCHANNEL_RULE_MASTERY,
	MCHANNEL_RULE_ELITE,
	MCHANNEL_RULE_CHAMPION,
	MCHANNEL_RULE_QUEST,
	MCHANNEL_RULE_DUELTOURNAMENT,
	MCHANNEL_RULE_SET1,									// 예비용 채널 1
	MCHANNEL_RULE_SET2,									// 예비용 채널 2
	MCHANNEL_RULE_SET3,									// 예비용 채널 3
	MCHANNEL_RULE_EVENT,

	MCHANNEL_RULE_MAX
};

#define MCHANNEL_RULE_NOVICE_STR		"novice"
#define MCHANNEL_RULE_NEWBIE_STR		"newbie"
#define MCHANNEL_RULE_ROOKIE_STR		"rookie"
#define MCHANNEL_RULE_MASTERY_STR		"mastery"
#define MCHANNEL_RULE_ELITE_STR			"elite"
#define MCHANNEL_RULE_CHAMPION_STR		"champion"
#define MCHANNEL_RULE_QUEST_STR			"quest"
#define MCHANNEL_RULE_DUELTOURNAMENT_STR "dueltournament"
#define MCHANNEL_RULE_SET1_STR			"set1"			// 예비용 채널 1
#define MCHANNEL_RULE_SET2_STR			"set2"			// 예비용 채널 2
#define MCHANNEL_RULE_SET3_STR			"set3"			// 예비용 채널 3



class MChannelRuleMapList : public list<int>
{
private:
	set<int>		m_Set;
public:
	void Add(int nMapID)
	{
		m_Set.insert(nMapID);
		push_back(nMapID);
	}
	void Add(string strMapName);
	bool Exist(int nMapID, bool bOnlyDuel);
	bool Exist(const char* pszMapName, bool bOnlyDuel);
	bool ExistAsCTF(int nMapID);
	bool ExistAsCTF(const char* pszMapName);
};

class MChannelRuleGameTypeList : public list<int>
{
private:
	set<int>		m_Set;
public:
	void Add(int nGameTypeID)
	{
		m_Set.insert(nGameTypeID);
		push_back(nGameTypeID);
	}
	bool Exist(int nGameTypeID)
	{
#ifdef _DEBUG
		set<int>::iterator i = m_Set.begin();
		set<int>::iterator e = m_Set.end();

		for(;i != e; ++i )
		{
			// mlog( "game type : %d.\n", (*i) );
		}

#endif
		if (m_Set.find(nGameTypeID) != m_Set.end()) return true;
		return false;
	}

	const int GetFirstGameType()
	{
		if( empty() )
		{
			return -1;
		}

		return *(begin());
	}
};

class MChannelRule {
protected:
	int							m_nID;
	string						m_Name;
	MChannelRuleMapList			m_MapList;
	MChannelRuleGameTypeList	m_GameTypeList;
	int							m_nDefault;

public:
	MChannelRule()						{}
	virtual ~MChannelRule()				{}
	void Init(int nID, const char* pszName)
	{
		m_nDefault = 0;
		m_nID = nID;
		m_Name = pszName;
	}

	int GetID()							{ return m_nID; }
	const char* GetName()				{ return m_Name.c_str(); }

	void AddMap(string strMapName)		{ m_MapList.Add(strMapName); }
	void AddGameType(int nGameTypeID)	{ m_GameTypeList.Add(nGameTypeID); }
	bool CheckMap(int nMapID, bool bOnlyDuel)
	{
		return m_MapList.Exist(nMapID, bOnlyDuel);
	}
	bool CheckMap(const char* pszMapName, bool bOnlyDuel)
	{
		return m_MapList.Exist(pszMapName, bOnlyDuel);
	}
	bool CheckCTFMap(const char* pszMapName)
	{
		return m_MapList.ExistAsCTF(pszMapName);
	}
	bool CheckCTFMap(int nMapID)
	{
		return m_MapList.ExistAsCTF(nMapID);
	}
	bool CheckGameType(int nGameTypeID)
	{
#ifdef _DEBUG
		//mlog( "Rule name : %s.\n", m_Name.c_str() );
#endif
		return m_GameTypeList.Exist(nGameTypeID);
	}
	MChannelRuleMapList* GetMapList()					{ return &m_MapList; }
	MChannelRuleGameTypeList* GetGameTypeList()			{ return &m_GameTypeList; }
	void SetDefault( int nDefault)						{ m_nDefault = nDefault; }
	int GetDefault()									{ return m_nDefault; }
};



class MChannelRuleMgr : public map<string, MChannelRule*>, public MXmlParser
{
private:
	map<MCHANNEL_RULE, MChannelRule*>		m_RuleTypeMap;
	void AddRule(MChannelRule* pRule);
public:
	MChannelRuleMgr();
	virtual ~MChannelRuleMgr();
	void Clear();	
	MChannelRule* GetRule(const string& strName);
	MChannelRule* GetRule(MCHANNEL_RULE nChannelRule);

protected:
	void ParseRule(rapidxml::xml_node<>* element);
	virtual void ParseRoot(const char* szTagName, rapidxml::xml_node<>* pElement);
};



#endif