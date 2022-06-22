#include "stdafx.h"
#include "ZRule.h"
#include "ZMatch.h"
#include "ZRuleDeathMatch.h"
#include "ZRuleGladiator.h"
#include "ZRuleAssassinate.h"
#include "ZRuleTraining.h"
#include "ZRuleQuest.h"
#include "ZRuleSurvival.h"
#include "ZRuleBerserker.h"
#include "ZRuleDuel.h"
#include "ZRuleDuelTournament.h"
#include "ZRuleClassic.h"
#include "ZRuleVampire.h"
#include "ZRuleNKS.h"
#include "ZRuleNinja.h"
#include "ZRuleSpy.h"
#include "MagicBox/ZRuleDropGunGame.h"

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
			return (new ZRuleTeamDeathMatch(pMatch));
		}
		break;
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
	case MMATCH_GAMETYPE_INFECTED:
	{
		return (new ZRuleTeamInfected(pMatch));
	}
	break;
	case MMATCH_GAMETYPE_CLASSIC_SOLO:
		{
			return (new ZRuleSoloClassic(pMatch));
		}
		break;
	case MMATCH_GAMETYPE_CLASSIC_TEAM:
		{
			return (new ZRuleTeamClassic(pMatch));
		}
		break;
	case MMATCH_GAMETYPE_NINJA:
		{
			return (new ZRuleNinja(pMatch));
		}
		break;
	case MMATCH_GAMETYPE_NOKSTYLE:
	{
		return (new ZRuleNKS(pMatch));
	}
	break;
	case MMATCH_GAMETYPE_VAMPIRE:
	{
		return (new ZRuleVampire(pMatch));
	}
	break;
	case MMATCH_GAMETYPE_PAINTBALL_SOLO:
	{
		 return (new ZRuleSoloPaintball(pMatch));
	}
		break;
	case MMATCH_GAMETYPE_PAINTBALL_TEAM:
	{
		return (new ZRuleTeamPaintball(pMatch));
	}
		break;
	case MMATCH_GAMETYPE_MODE_STAFF:
	{
		 return (new ZRuleModeStaff(pMatch));
	}
		break;
	case MMATCH_GAMETYPE_SNIPERMODE:
	{
		return (new ZRuleTeamDeathMatch(pMatch)); // No Need for own rule
	}
	break;
#ifdef _GUNGAME
	case MMATCH_GAMETYPE_GUNMODE:
	{
		return (new ZRuleGunMode(pMatch));
	}
	break;
#endif
	case MMATCH_GAMETYPE_SPY:
	{
		return (new ZRuleSpy(pMatch));
	}
	break;
	case MMATCH_GAMETYPE_DROPMAGIC:
	{
		return (new ZRuleDropGunGame(pMatch));
	}
	break;
	default:
		{
			// 게임 타입이 없습니다.
			//_ASSERT(0);
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