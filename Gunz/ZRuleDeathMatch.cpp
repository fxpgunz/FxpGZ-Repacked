#include "stdafx.h"
#include "ZRuleDeathMatch.h"
#include "ZScreenEffectManager.h"
#include "MMatchUtil.h"

ZRuleSoloDeathMatch::ZRuleSoloDeathMatch(ZMatch* pMatch) : ZRule(pMatch)
{

}

ZRuleSoloDeathMatch::~ZRuleSoloDeathMatch()
{





}
/////////////////////////////////////////////////////////////////////////////////////////

ZRuleTeamDeathMatch::ZRuleTeamDeathMatch(ZMatch* pMatch) : ZRule(pMatch)
{

}

ZRuleTeamDeathMatch::~ZRuleTeamDeathMatch()
{

}


/////////////////////////////////////////////////////////////////////////////////////////

ZRuleTeamDeathMatch2::ZRuleTeamDeathMatch2(ZMatch* pMatch) : ZRule(pMatch)
{

}

ZRuleTeamDeathMatch2::~ZRuleTeamDeathMatch2()
{

}

bool ZRuleTeamDeathMatch2::OnCommand(MCommand* pCommand)
{
	if (!ZGetGame()) return false;

	switch (pCommand->GetID())
	{

	case MC_MATCH_GAME_DEAD:
		{
			MUID uidAttacker, uidVictim;

			pCommand->GetParameter(&uidAttacker, 0, MPT_UID);
			pCommand->GetParameter(&uidVictim, 2, MPT_UID);

			ZCharacter* pAttacker = ZGetGame()->m_CharacterManager.Find(uidAttacker);
			ZCharacter* pVictim = ZGetGame()->m_CharacterManager.Find(uidVictim);
			
			m_pMatch->AddTeamKills(pVictim->GetTeamID() == MMT_BLUE ? MMT_RED : MMT_BLUE);
		}
		break;
	}

	return false;
}

ZRuleTeamCTF::ZRuleTeamCTF(ZMatch* pMatch) : ZRule(pMatch)
{
	ZeroMemory(m_BlueFlagPos, sizeof(m_BlueFlagPos));
	ZeroMemory(m_RedFlagPos, sizeof(m_RedFlagPos));
}

ZRuleTeamCTF::~ZRuleTeamCTF()
{

}

bool ZRuleTeamCTF::OnCommand(MCommand* pCommand)
{
	if (!ZGetGame()) return false;

	switch (pCommand->GetID())
	{
		case MC_MATCH_FLAG_STATE:
		{
			int nItemID;
			MShortVector s_pos;
			int IsGone;
			MUID Carrier;
			pCommand->GetParameter(&nItemID, 0, MPT_INT);
			pCommand->GetParameter(&s_pos, 1, MPT_SVECTOR);
			pCommand->GetParameter(&IsGone, 2, MPT_INT);
			pCommand->GetParameter(&Carrier, 3, MPT_UID);
			switch(nItemID)
			{
			case CTF_RED_ITEM_ID:
				{
				SetRedCarrier(Carrier);
				SetRedFlagState(IsGone);
				SetRedFlagPos(rvector(s_pos.x, s_pos.y, s_pos.z));
				}
				break;
			case CTF_BLUE_ITEM_ID:
				{
				SetBlueCarrier(Carrier);
				SetBlueFlagState(IsGone);
				SetBlueFlagPos(rvector(s_pos.x, s_pos.y, s_pos.z));
				}
				break;
			}
		}
		break;

	case MC_MATCH_FLAG_CAP:
		{
			MMatchTeam nTeam;
			pCommand->GetParameter(&nTeam, 0, MPT_INT);
			m_pMatch->AddTeamKills(nTeam == MMT_BLUE ? MMT_BLUE : MMT_RED);

			if(nTeam == MMT_RED)
			{
				ZGetGameInterface()->PlayVoiceSound( VOICE_RED_TEAM_SCORE, 1600);
				ZGetScreenEffectManager()->AddScreenEffect("ctf_score_r");
				SetBlueFlagState(false);
				SetRedCarrier(MUID(0,0));
			}
			else if(nTeam == MMT_BLUE)
			{
				ZGetGameInterface()->PlayVoiceSound( VOICE_BLUE_TEAM_SCORE, 1600);
				ZGetScreenEffectManager()->AddScreenEffect("ctf_score_b");
				SetBlueCarrier(MUID(0,0));
				SetRedFlagState(true);
			}

			for (ZCharacterManager::iterator itor = ZGetGame()->m_CharacterManager.begin();
			itor != ZGetGame()->m_CharacterManager.end(); ++itor)
			{
				ZCharacter* pCharacter = (*itor).second;
				if(pCharacter)
				{
					if(pCharacter->GetTeamID() == nTeam)
					{
						pCharacter->SetTagger(false);
					}
				}
			}
		}
		break;
	case MC_MATCH_FLAG_EFFECT:
		{
			MUID uidOwner;
			int nTeam;
			pCommand->GetParameter(&uidOwner,		0, MPT_UID);
			pCommand->GetParameter(&nTeam,		1, MPT_INT);

			ZCharacter* pCapper = ZGetGame()->m_CharacterManager.Find(uidOwner);
			if(pCapper)
			{
				if(!pCapper->IsDie())
				{
					AssignFlagEffect(uidOwner, nTeam);
				}
				else
				{
					for (ZCharacterManager::iterator itor = ZGetGame()->m_CharacterManager.begin();
					itor != ZGetGame()->m_CharacterManager.end(); ++itor)
					{
						ZCharacter* pCharacter = (*itor).second;
						if(pCharacter)
						{
							if(pCharacter->GetTeamID() == nTeam)
							{
								pCharacter->SetTagger(false);
							}
						}
					}
					if(nTeam == MMT_RED)
					{
						SetRedCarrier(MUID(0,0));
						SetBlueFlagState(false);
						ZGetScreenEffectManager()->AddScreenEffect("ctf_flagdrop_b");
						ZGetGameInterface()->PlayVoiceSound( VOICE_BLUE_FLAG_RETURN, 1600);
					}
					else if(nTeam == MMT_BLUE)
					{
						SetBlueCarrier(MUID(0,0));
						SetRedFlagState(false);
						ZGetScreenEffectManager()->AddScreenEffect("ctf_flagdrop_r");
						ZGetGameInterface()->PlayVoiceSound( VOICE_RED_FLAG_RETURN, 1600);
					}
				}
			}
			else
			{
					for (ZCharacterManager::iterator itor = ZGetGame()->m_CharacterManager.begin();
					itor != ZGetGame()->m_CharacterManager.end(); ++itor)
					{
						ZCharacter* pCharacter = (*itor).second;
						if(pCharacter)
						{
							if(pCharacter->GetTeamID() == nTeam)
							{
								pCharacter->SetTagger(false);
							}
						}
					}
					if(nTeam == MMT_RED)
					{
					SetRedCarrier(MUID(0,0));
					SetBlueFlagState(false);
					ZGetScreenEffectManager()->AddScreenEffect("ctf_flagdrop_b");
					ZGetGameInterface()->PlayVoiceSound( VOICE_BLUE_FLAG_RETURN, 1600);
					}
					else if(nTeam == MMT_BLUE)
					{
					SetBlueCarrier(MUID(0,0));
					SetRedFlagState(false);
					ZGetScreenEffectManager()->AddScreenEffect("ctf_flagdrop_r");
					ZGetGameInterface()->PlayVoiceSound( VOICE_RED_FLAG_RETURN, 1600);
					}
			}
		}
		break;
	}

	return false;
}

void ZRuleTeamCTF::AssignFlagEffect(MUID& uidOwner, int nTeam)
{
	if (!ZGetGame()) return;

	for (ZCharacterManager::iterator itor = ZGetGame()->m_CharacterManager.begin();
	itor != ZGetGame()->m_CharacterManager.end(); ++itor)
	{
		ZCharacter* pCharacter = (*itor).second;
		if(pCharacter)
		{
			if(pCharacter->GetTeamID() == nTeam)
			{
				pCharacter->SetTagger(false);
			}
		}
	}

	ZCharacter* pFlagChar = ZGetGame()->m_CharacterManager.Find(uidOwner);
	if (pFlagChar)
	{		
		ZGetEffectManager()->AddBerserkerIcon(pFlagChar);
		pFlagChar->SetTagger(true);
			if(nTeam == MMT_BLUE)
			{
			SetBlueCarrier(uidOwner);
			SetRedFlagState(true);
			ZGetScreenEffectManager()->AddScreenEffect("ctf_taken_r");
			ZGetGameInterface()->PlayVoiceSound( VOICE_BLUE_HAS_FLAG, 1600);
			}
			else if (nTeam == MMT_RED)
			{
			SetRedCarrier(uidOwner);
			SetBlueFlagState(true);
			ZGetScreenEffectManager()->AddScreenEffect("ctf_taken_b");
			ZGetGameInterface()->PlayVoiceSound( VOICE_RED_HAS_FLAG, 1600);
			}
		}
        if(nTeam == MMT_BLUE)
        {
            ZGetEffectManager()->AddBerserkerIcon(pFlagChar);
            ZGetEffectManager()->AddRedFlagIcon(pFlagChar);
            pFlagChar->SetTagger(true);
        }        
        else if(nTeam == MMT_RED)
        {
            ZGetEffectManager()->AddBerserkerIcon(pFlagChar);
            ZGetEffectManager()->AddBlueFlagIcon(pFlagChar);
            pFlagChar->SetTagger(true);
        }  
}
#include "stdafx.h"
#include "ZRulePaint.h"

//PaintMode
ZRuleSoloPaintball::ZRuleSoloPaintball(ZMatch* pMatch) : ZRuleSoloDeathMatch(pMatch)
{

}

ZRuleSoloPaintball::~ZRuleSoloPaintball()
{

}
ZRuleTeamPaintball::ZRuleTeamPaintball(ZMatch* pMatch) : ZRuleTeamDeathMatch(pMatch)
{

}

ZRuleTeamPaintball::~ZRuleTeamPaintball()
{

}
//Mode Staff
ZRuleModeStaff::ZRuleModeStaff(ZMatch* pMatch) : ZRule(pMatch)
{

}
ZRuleModeStaff::~ZRuleModeStaff()
{

}
//Infected Mode
ZRuleTeamInfected::ZRuleTeamInfected(ZMatch* pMatch) : ZRule(pMatch)
{
	m_bLastFive = false;
}

ZRuleTeamInfected::~ZRuleTeamInfected()
{

}

bool ZRuleTeamInfected::OnCommand(MCommand* pCommand)
{
	if (!ZGetGame()) return false;

	switch (pCommand->GetID())
	{
	case MC_MATCH_ASSIGN_COMMANDER:
	{
		MUID uidPlayer;

		pCommand->GetParameter(&uidPlayer, 0, MPT_UID);
		//pCommand->GetParameter(&uidBlueCommander,	1, MPT_UID);

		OnInfected(uidPlayer, true);
	}
	break;

	case MC_MATCH_INFECT:
	{
		MUID uidPlayer;

		pCommand->GetParameter(&uidPlayer, 0, MPT_UID);

		OnInfected(uidPlayer, false);
	}
	break;

	case MC_MATCH_LASTSURVIVOR:
	{
		MUID uidPlayer;
		pCommand->GetParameter(&uidPlayer, 0, MPT_UID);

		if (uidPlayer.IsInvalid())
			break;

		ZCharacter* pChar = (ZCharacter*)ZGetGame()->m_CharacterManager.Find(uidPlayer);
		if (pChar)
		{
			char szInfectedMsg[128];
			memset(szInfectedMsg, 0, sizeof(szInfectedMsg));

			if (pChar == ZGetGame()->m_pMyCharacter && !ZGetGameInterface()->GetCombatInterface()->GetObserverMode())
			{
				sprintf_s(szInfectedMsg, "You are the last survivor.");
				ZGetGameInterface()->GetCombatInterface()->UpdateCTFMsg(szInfectedMsg);
			}
			else
			{
				sprintf_s(szInfectedMsg, "'%s' is the last survivor.", pChar->GetUserName());
				ZGetGameInterface()->GetCombatInterface()->UpdateCTFMsg(szInfectedMsg);
			}
		}

		ZGetSoundEngine()->PlaySound("last_survivor");
	}
	break;
	}

	return false;
}

void ZRuleTeamInfected::OnInfected(const MUID& uidPlayer, bool bFirst)
{
	if (!ZGetGame()) return;

	if (uidPlayer.IsInvalid()) return;

	char szTest[128];
	memset(szTest, 0, sizeof(szTest));

	//sprintf_s (szTest, "player [%u-%u] - infect call start", uidPlayer.High, uidPlayer.Low);
	//ZChatOutput(szTest, ZChat::CMT_SYSTEM);

	if (bFirst)
	{
		for (ZCharacterManager::iterator itor = ZGetGame()->m_CharacterManager.begin(); itor != ZGetGame()->m_CharacterManager.end(); ++itor)
		{
			ZGetEffectManager()->Clear();
			ZCharacter* pCharacter = (ZCharacter*)(*itor).second;
			pCharacter->m_dwStatusBitPackingValue.Ref().m_bCommander = false;
		}
	}

	ZCharacter* pChar = (ZCharacter*)ZGetGame()->m_CharacterManager.Find(uidPlayer);

	if (pChar)
	{
		if (pChar->m_bInfected)
		{
			//sprintf_s (szTest, "player [%u-%u] - infect FAIL (done)", uidPlayer.High, uidPlayer.Low);
			//ZChatOutput(szTest, ZChat::CMT_SYSTEM);
			return;
		}

		if (!pChar->GetInitialized() || pChar->IsAdminHide())
			return;

		//sprintf_s (szTest, "player [%u-%u] - infect OK", uidPlayer.High, uidPlayer.Low);
		//ZChatOutput(szTest, ZChat::CMT_SYSTEM);

		pChar->SetVisible(true);
		pChar->Revival();
		pChar->InfectCharacter(bFirst);
		pChar->OnScream();

		// because revival() resets the commander flag
		if (bFirst)
		{
			ZGetEffectManager()->AddCommanderIcon(pChar, 0);
			pChar->m_dwStatusBitPackingValue.Ref().m_bCommander = true;
			m_uidPatientZero = pChar->GetUID();
		}
		else
		{
			ZGetSoundEngine()->PlaySound("scored");
			ZCharacter* pMaster = (ZCharacter*)ZGetGame()->m_CharacterManager.Find(m_uidPatientZero);

			if (pMaster && pMaster == ZGetGame()->m_pMyCharacter)
			{
				pMaster->SetHP(500.f); // ZOMBIE_HP
			}
		}

		int nRandId = rand() % 14 + 1;

		char szSndName[128];
		memset(szSndName, 0, sizeof(szSndName));
		sprintf_s(szSndName, "zombie_voice_idle%d", nRandId);

		ZGetSoundEngine()->PlaySound(szSndName/*, pChar->GetPosition(), pChar->IsObserverTarget()*/);

		if ((!ZGetGameInterface()->GetCombatInterface()->GetObserverMode() && pChar == ZGetGame()->m_pMyCharacter) || (ZGetGameInterface()->GetCombatInterface()->GetObserverMode() && pChar == ZGetGameInterface()->GetCombatInterface()->GetObserver()->GetTargetCharacter()))
		{
			// don't release
			if (pChar == ZGetGame()->m_pMyCharacter)
				ZGetGame()->ReleaseObserver();

			ZGetCamera()->Shock(2000.0f, .7f, rvector(0.0f, 0.0f, -1.0f)); // Vibrate camera

			char szInfectedMsg[128];
			memset(szInfectedMsg, 0, sizeof(szInfectedMsg));

			sprintf_s(szInfectedMsg, "YOU are Infected. KILL THEM ALL!");

			ZGetGameInterface()->GetCombatInterface()->UpdateCTFMsg(szInfectedMsg);
			ZGetSoundEngine()->PlaySound("you_infected");
		}
	}
}

void ZRuleTeamInfected::OnSetRoundState(MMATCH_ROUNDSTATE roundState)
{
	switch (roundState)
	{
	case MMATCH_ROUNDSTATE_PRE_COUNTDOWN:
	{
		ZGetGameInterface()->GetCombatInterface()->UpdateCTFMsg("Zombies will spawn soon. HIDE!");
		m_bLastFive = false;
		m_uidPatientZero = MUID(0, 0);
		//ZChatOutput("HIDE NOW", ZChat::CMT_SYSTEM);
		ZGetSoundEngine()->PlaySound("wait_zombie_player");
	}
	break;
	case MMATCH_ROUNDSTATE_COUNTDOWN:
	{
		//ZChatOutput("", ZChat::CMT_SYSTEM);
	}
	break;
	case MMATCH_ROUNDSTATE_PLAY:
	{
		if (ZGetGame()->IsReplay() && m_uidPatientZero.IsValid() && ZGetGame()->m_pMyCharacter->GetUID() == m_uidPatientZero && !ZGetGame()->m_pMyCharacter->m_bInfected)
			ZGetGame()->m_pMyCharacter->InfectCharacter(true);

		if (ZGetGame()->IsReplay() && m_uidPatientZero.IsValid() && ZGetGame()->m_pMyCharacter->GetTeamID() == MMT_RED && !ZGetGame()->m_pMyCharacter->m_bInfected)
			ZGetGame()->m_pMyCharacter->InfectCharacter(false);

		ZGetGameInterface()->GetCombatInterface()->UpdateCTFMsg("RUN!");
		ZChatOutput("RUN! The zombies come for you, remember to work as a team.", ZChat::CMT_SYSTEM);
		ZGetSoundEngine()->PlaySound("run_for_you_life");
	}
	break;

	case MMATCH_ROUNDSTATE_FREE:
	{
		ZGetGameInterface()->GetCombatInterface()->UpdateCTFMsg("4 or more players are required to start the game.");
		m_bLastFive = false;
		m_uidPatientZero = MUID(0, 0);
		//ZChatOutput("RS -> (Free)", ZChat::CMT_SYSTEM);
		ZGetSoundEngine()->PlaySound("player_4");
	}
	break;
	}
}

void ZRuleTeamInfected::OnResponseRuleInfo(MTD_RuleInfo* pInfo)
{
	MTD_RuleInfo_Assassinate* pAssassinateRule = (MTD_RuleInfo_Assassinate*)pInfo;
	OnInfected(pAssassinateRule->uidRedCommander, true);
}

void ZRuleTeamInfected::OnUpdate(float fDelta)
{
	if (ZGetGame()->GetMatch()->GetRoundState() == MMATCH_ROUNDSTATE_PLAY)
	{
		DWORD dwTime = ZGetGame()->GetMatch()->GetRemaindTime();
		DWORD dwLimitTime = ZGetGameClient()->GetMatchStageSetting()->GetStageSetting()->nLimitTime;
		if (dwLimitTime != 99999)
		{
			dwLimitTime *= 60000;
			if (dwTime <= dwLimitTime)
			{
				dwTime = (dwLimitTime - dwTime) / 1000;

				if ((dwTime / 60) == 0 && (dwTime % 60) == 5 && !m_bLastFive)
				{
					m_bLastFive = true;
					ZGetSoundEngine()->PlaySound("time_tick_last5");
				}
			}
		}
	}

}
#ifdef _GUNGAME
ZRuleGunMode::ZRuleGunMode(ZMatch* pMatch) : ZRule(pMatch)
{

}

ZRuleGunMode::~ZRuleGunMode()
{

}
#endif
