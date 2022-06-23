#include "StdAfx.h"
#include ".\ZGameTypeList.h"


ZGameTypeConfig::ZGameTypeConfig( void)
{
	m_nDefaultRound			= 0;
	m_nDefaultLimitTime		= 0;
	m_nDefaultMaxPlayers	= 16;
}


ZGameTypeConfig::~ZGameTypeConfig( void)
{
	// Delete round list
	while ( !m_Round.empty())
	{
		delete *m_Round.begin();
		m_Round.pop_front();
	}


	// Delete limit time list
	while ( !m_LimitTime.empty())
	{
		delete *m_LimitTime.begin();
		m_LimitTime.pop_front();
	}


	// Delete players list
	while ( !m_MaxPlayers.empty())
	{
		delete *m_MaxPlayers.begin();
		m_MaxPlayers.pop_front();
	}
}




ZGameTypeList::ZGameTypeList( void)
{
}


ZGameTypeList::~ZGameTypeList( void)
{
	Clear();
}


void ZGameTypeList::Clear( void)
{
	while( !m_GameTypeCfg.empty())
	{
		delete m_GameTypeCfg.begin()->second;
		m_GameTypeCfg.erase( m_GameTypeCfg.begin());
	}
}


bool ZGameTypeList::ParseGameTypeList( int nGameTypeNum, rapidxml::xml_node<>* element)
{
	ZGameTypeConfig* pConfig = new ZGameTypeConfig;

	int nRoundCount			= 0,
		nTimeLimitCount		= 0,
		nMaxPlayersCount	= 0;

	for (auto itor = element; itor; itor = itor->next_sibling())
	{
		if (itor->name()[0] == '#')
			continue;
		if (_stricmp(itor->name(), "ROUNDS")==0)
		{
			if (ParseRound(pConfig, itor))
				pConfig->m_nDefaultRound = nRoundCount;
			nRoundCount++;
		}
		if (_stricmp(itor->name(), "LIMITTIME") == 0)
		{
			if (ParseLimitTime(pConfig, itor))
				pConfig->m_nDefaultLimitTime = nTimeLimitCount;

			nTimeLimitCount++;
		}
		if (_stricmp(itor->name(), "MAXPLAYERS") == 0)
		{
			if (ParseMaxPlayers(pConfig, itor))
				pConfig->m_nDefaultMaxPlayers = nMaxPlayersCount;
			nMaxPlayersCount++;
		}
	}

	m_GameTypeCfg.insert( m_GameTypeCfg.end(), MGAMETYPECFG::value_type( nGameTypeNum, pConfig));


	return true;
}


bool ZGameTypeList::ParseRound( ZGameTypeConfig* pConfig, rapidxml::xml_node<>* element)
{
	bool selected = false;

	ZGameTypeConfigData* pCfgData = new ZGameTypeConfigData;
	pCfgData->m_nValue = atoi(element->first_attribute("round")->value());
	strcpy( pCfgData->m_szString,element->first_attribute("str")->value());
	pConfig->m_Round.push_back( pCfgData);
	if (element->first_attribute("default") != nullptr)
		selected = true;

	return selected;		// Is default?
}


bool ZGameTypeList::ParseLimitTime( ZGameTypeConfig* pConfig, rapidxml::xml_node<>* element)
{
	bool selected = false;

	ZGameTypeConfigData* pCfgData = new ZGameTypeConfigData;
	pCfgData->m_nValue = atoi(element->first_attribute("sec")->value());
	strcpy( pCfgData->m_szString, ZGetStringResManager()->GetStringFromXml(element->first_attribute("str")->value()));
	pConfig->m_LimitTime.push_back( pCfgData);

	if (element->first_attribute("default") != nullptr)
		selected = true;

	return selected;		// Is default?
}


bool ZGameTypeList::ParseMaxPlayers( ZGameTypeConfig* pConfig, rapidxml::xml_node<>* element)
{
	bool selected = false;

	ZGameTypeConfigData* pCfgData = new ZGameTypeConfigData;
	pCfgData->m_nValue = atoi(element->first_attribute("player")->value());
	strcpy( pCfgData->m_szString, element->first_attribute("str")->value());
	pConfig->m_MaxPlayers.push_back( pCfgData);

	if (element->first_attribute("default") != nullptr)
		selected = true;
	return selected;		// Is default?
}


ZGameTypeConfig* ZGameTypeList::GetGameTypeCfg( int nGameTypeNum)
{
	ZGameTypeConfig* pGameTypeCfg = NULL;

	MGAMETYPECFG::iterator itr = m_GameTypeCfg.find( nGameTypeNum);
	if ( itr != m_GameTypeCfg.end())
	{
		pGameTypeCfg = (*itr).second;
	}


	return pGameTypeCfg;
}
