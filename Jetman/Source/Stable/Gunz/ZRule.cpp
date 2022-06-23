#include "stdafx.h"
#include "ZRule.h"
#include "ZMatch.h"
#include "ZRuleDeathMatch.h"
#include "ZRuleGladiator.h"
#include "ZRuleAssassinate.h"
#include "ZRuleTraining.h"
#include "ZRuleQuest.h"
#include "ZRuleSurvival.h"
#include "ZRuleQuestChallenge.h"
#include "ZRuleBerserker.h"
#include "ZRuleDuel.h"
#include "ZRuleDuelTournament.h"
#include "ZRuleGunGame.h"
#include "ZRuleInfection.h"
#include "ZRuleSpyMode.h"
#include "ZRuleBlitzKrieg.h"
#include "ZRuleSkillMap.h"
#include "ZRuleDemolition.h"

ZRule::ZRule(ZMatch* pMatch)
{
	m_pMatch = pMatch;				// 초기화 =_=
}

ZRule::~ZRule()
{

}

void ZRule::Update(float fDelta)
{
	OnUpdate(fDelta);
}

ZRule* ZRule::CreateRule(ZMatch* pMatch, MMATCH_GAMETYPE nGameType)
{
	switch(nGameType)
	{
	case MMATCH_GAMETYPE_DEATHMATCH_SOLO:
		{
			return (new ZRuleSoloDeathMatch(pMatch));
		}
		break;
	case MMATCH_GAMETYPE_DEATHMATCH_TEAM:
		{
			return (new ZRuleTeamClassic(pMatch));
		}
		break;
	/*case MMATCH_GAMETYPE_CLASSIC_SOLO:
		{
			return (new ZRuleSoloClassic(pMatch));
		}
		break;
	case MMATCH_GAMETYPE_CLASSIC_TEAM:
		{
			return (new ZRuleTeamDeathMatch(pMatch));
		}
		break;*/
	case MMATCH_GAMETYPE_GLADIATOR_SOLO:
		{
			return (new ZRuleSoloGladiator(pMatch));
		}
		break;
	case MMATCH_GAMETYPE_GLADIATOR_TEAM:
		{
			return (new ZRuleTeamGladiator(pMatch));
		}
		break;
	case MMATCH_GAMETYPE_ASSASSINATE:
		{
			return (new ZRuleAssassinate(pMatch));
		}
		break;
	case MMATCH_GAMETYPE_TRAINING:
		{
			return (new ZRuleTraining(pMatch));
		}
		break;
#ifdef _QUEST
	case MMATCH_GAMETYPE_SURVIVAL:
		{
			return (new ZRuleSurvival(pMatch));
		}
		break;
	case MMATCH_GAMETYPE_QUEST:
		{
			return (new ZRuleQuest(pMatch));
		}
		break;
#endif
	case MMATCH_GAMETYPE_QUEST_CHALLENGE:
		{
			return (new ZRuleQuestChallenge(pMatch));
		}
		break;
	case MMATCH_GAMETYPE_BERSERKER:
		{
			return (new ZRuleBerserker(pMatch));
		}
		break;
	case MMATCH_GAMETYPE_DEATHMATCH_TEAM2:
		{
			return (new ZRuleTeamDeathMatch2(pMatch));
		}
		break;
	case MMATCH_GAMETYPE_DUEL:
		{
			return (new ZRuleDuel(pMatch));
		}
		break;

	case MMATCH_GAMETYPE_DUELTOURNAMENT:
		{
			return (new ZRuleDuelTournament(pMatch));
		}
		break;
	case MMATCH_GAMETYPE_CTF:
		{
			return (new ZRuleTeamCTF(pMatch));
		}
		break;
	case MMATCH_GAMETYPE_GUNGAME:
		{
			return (new ZRuleGunGame(pMatch));
		}
		break;
	case MMATCH_GAMETYPE_INFECTION:
	{
		return (new ZRuleInfection(pMatch));
	}break;
	case MMATCH_GAMETYPE_SPYMODE:
	{
		return (new ZRuleSpyMode(pMatch));
	}break;
	case MMATCH_GAMETYPE_BLITZKRIEG:
	{
		return (new ZRuleBlitzKrieg(pMatch));
	}break;
	case MMATCH_GAMETYPE_SKILLMAP:
	{
		return (new ZRuleSkillMap(pMatch));
	}break;
	case MMATCH_GAMETYPE_DEMOLITION:
	{
		return (new ZRuleDemolition(pMatch));
	}break;


	default:
		{
			// 게임 타입이 없습니다.
			_ASSERT(0);
		}
	}
	return NULL;

}

bool ZRule::OnCommand(MCommand* pCommand)
{
	return false;
}

void ZRule::AfterCommandProcessed( MCommand* pCommand )
{

}

void ZRule::OnResponseRuleInfo(MTD_RuleInfo* pInfo)
{

}