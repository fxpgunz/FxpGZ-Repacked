#include "stdafx.h"
#include "ZRuleQuestChallenge.h"
#include "ZMatch.h"
#include "MActorDef.h"
#include "ZFSMManager.h"
#include "ZActorActionManager.h"
#include "ZActorAction.h"
#include "ZActorWithFSM.h"
#include "ZApplication.h"
#include "ZFSMManager.h"
#include "ZFSMParser.h"
#include "ZFSM.h"
#include "ZScreenEffectManager.h"
#include "ZBmNumLabel.h"

ZRuleQuestChallenge::ZRuleQuestChallenge(ZMatch* pMatch) : IRuleNewQuest(pMatch)
, m_pScenario(NULL)
, m_nCurrSector(0)
, m_nXP(0)
, m_nBP(0)
{
	ZGetScreenEffectManager()->CreateQuestRes();
	m_uidBoss.SetZero();
	m_FadeBGTime = 0;
}

ZRuleQuestChallenge::~ZRuleQuestChallenge()
{
	ZGetScreenEffectManager()->DestroyQuestRes();
}

bool ZRuleQuestChallenge::LoadScenarioMap(const char* szScenarioName)
{
	MNewQuestScenarioManager* pScenarioMgr = ZApplication::GetStageInterface()->GetChallengeQuestScenario();
	if (!pScenarioMgr) { _ASSERT(0); return false; }
	MNewQuestScenario* pScenario = pScenarioMgr->GetScenario(szScenarioName);
	if (!pScenario) { _ASSERT(0); return false; }

	int numSector = pScenario->GetNumSector();
	for (int i=0; i<numSector; ++i)
	{
		MNewQuestSector* pSector = pScenario->GetSector(i);
		if (!pSector)  { _ASSERT(0); return false; }

		const char* szMapName = pSector->GetMapName();
		_ASSERT(0 != strlen(szMapName));
		ZGetWorldManager()->AddWorld(szMapName);
	}

	return true;
}

int ZRuleQuestChallenge::GetRoundMax()
{
	if (!m_pScenario) {
		_ASSERT(0);
		return 1;
	}

	return m_pScenario->GetNumSector();
}


bool ZRuleQuestChallenge::Init()
{
	if (!IRuleNewQuest::Init()) {
		mlog("Error initializing base class\n");
		return false;
	}

	// 이 퀘스트의 시나리오 객체 알아두기
	const char* szScenarioName = ZGetGameClient()->GetMatchStageSetting()->GetMapName();
	
	MNewQuestScenarioManager* pScenarioMgr = ZApplication::GetStageInterface()->GetChallengeQuestScenario();
	if (!pScenarioMgr) { _ASSERT(0); return false; }
	MNewQuestScenario* pScenario = pScenarioMgr->GetScenario(szScenarioName);
	if (!pScenario) { _ASSERT(0); return false; }

	m_pScenario = pScenario;

	m_nCurrSector = 0;

	return true;
}

void ZRuleQuestChallenge::OnUpdate( float fDelta )
{
	IRuleNewQuest::OnUpdate(fDelta);
}

void ZRuleQuestChallenge::OnSetRoundState( MMATCH_ROUNDSTATE roundState )
{
	switch(roundState) 
	{
	case MMATCH_ROUNDSTATE_PREPARE: 
		{
		}
		break;
	case MMATCH_ROUNDSTATE_PRE_COUNTDOWN:
		{
		}
		break;
	case MMATCH_ROUNDSTATE_COUNTDOWN : 
		{
		}
		break;
	case MMATCH_ROUNDSTATE_PLAY:
		{
		}
		break;
	case MMATCH_ROUNDSTATE_FINISH:
		{
		}
		break;
	case MMATCH_ROUNDSTATE_EXIT:
		{
		}
		break;
	case MMATCH_ROUNDSTATE_FREE:
		{
		}
		break;
	};
}

bool ZRuleQuestChallenge::OnCommand( MCommand* pCommand )
{
	if (IRuleNewQuest::OnCommand(pCommand))
		return true;

	switch (pCommand->GetID())
	{


	case MC_NEWQUEST_MOVE_TO_NEXT_SECTOR:
		{
			MoveToNextSector();
			return true;
		}break;

	case MC_QUEST_SECTOR_BONUS:
		{
			ApplyReward(pCommand);
			return true;
		}break;
	case MC_MATCH_LATEJOIN_CQ:
		{
			MUID uidPlayer;
			int currSector;
			UINT currTime;
			pCommand->GetParameter(&uidPlayer, 0, MPT_UID);
			pCommand->GetParameter(&currSector, 1, MPT_INT);
			pCommand->GetParameter(&currTime, 2, MPT_UINT);

			ZCharacter* pCharacter = dynamic_cast<ZCharacter*>(ZGetCharacterManager()->Find(uidPlayer));
			pCharacter->ForceDie();
			if (uidPlayer == ZGetGame()->m_pMyCharacter->GetUID())
			{
				SetElapsedTime(currTime); //1 second time offset?
				MoveToRealSector(currSector);
			}
			return true;
		}break;
	default:
		break;
	}

	return false;
}


void ZRuleQuestChallenge::MoveToNextSector()
{
	ZCharacter *pMyChar = ZGetGame()->m_pMyCharacter;
	pMyChar->InitStatus();
	pMyChar->SetVisible(false);

	ZGetEffectManager()->Clear();
	ZGetGame()->m_WeaponManager.Clear();

	++m_nCurrSector;	
	ZGetWorldManager()->SetCurrent(m_nCurrSector);

}

void ZRuleQuestChallenge::MoveToRealSector(int currSector)
{
	ZCharacter *pMyChar = ZGetGame()->m_pMyCharacter;
	ZGetCombatInterface()->SetObserverMode(true);
	pMyChar->InitStatus();
	pMyChar->SetVisible(false);
	pMyChar->ForceDie();
	pMyChar->Die();
	m_nCurrSector = currSector;
	ZGetWorldManager()->SetCurrent(currSector);
}

void ZRuleQuestChallenge::ApplyReward(MCommand* pCommand)
{
	MUID uidPlayer;
	unsigned long int nRewardXP = 0;
	unsigned long int nRewardBP = 0;
	pCommand->GetParameter(&uidPlayer, 0, MPT_UID);
	pCommand->GetParameter(&nRewardXP, 1, MPT_UINT);
	pCommand->GetParameter(&nRewardBP, 2, MPT_UINT);

	int nLevelPct = GetExpPercentFromTransData(nRewardXP);

	if (ZGetCharacterManager()->Find(uidPlayer) == ZGetGame()->m_pMyCharacter)
	{
		char szText[256];
		ZGetMyInfo()->SetXP(ZGetMyInfo()->GetXP() + nRewardXP);
		ZGetMyInfo()->SetLevelPercent(nLevelPct);
		ZGetScreenEffectManager()->SetGaugeExpFromMyInfo();
		ZGetMyInfo()->SetBP(ZGetMyInfo()->GetBP() + nRewardBP);
		m_nXP += nRewardXP;
		m_nBP += nRewardBP;
		ZGetScreenEffectManager()->AddExpEffect(nRewardXP);		// 획득 경험치 표시 없애기
		sprintf(szText, "You have won %d XP and %d BP for clearing the round", nRewardXP, nRewardBP);
		ZChatOutput(ZCOLOR_CHAT_SYSTEM, szText);
	}
}


void ZRuleQuestChallenge::OnDraw(MDrawContext* pDC)
{
	char szText[256];

	MWidget* pWidget = (MWidget*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("CombatChallQuestInfo");
	if (pWidget)
	{
		ZBmNumLabel* pBmNumLabel = (ZBmNumLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("CQ_ElapsedTime");
		if (pBmNumLabel)
		{
			sprintf(szText, "%2d,%02d", GetElapsedTime() / 1000 / 60, GetElapsedTime() / 1000 % 60);
			pBmNumLabel->SetText(szText);
		}

		pBmNumLabel = (ZBmNumLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("CQ_RoundProgress");
		if (pBmNumLabel)
		{

			sprintf(szText, "%d/%d", m_nCurrSector + 1, GetRoundMax());
			pBmNumLabel->SetText(szText);
		}
		MPicture* pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("CQ_FadeBG");
		if (pPicture)
		{
			pPicture->Show(false);
			if (ZGetGame()->GetMatch()->GetRoundState() == MMATCH_ROUNDSTATE_FINISH)
			{
				if (m_nCurrSector + 1 < GetRoundMax())
				{
					pPicture->Show(true);
				}
			}

		}

		MLabel* pLabel = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("CQ_ObserverDescription");
		if (pLabel)
		{
			pLabel->Show(false);
			if (ZGetCombatInterface()->GetObserverMode())
			{
				pLabel->Show(true);
				pLabel->SetText("You will be revived after this round.");
			}
			else if (!ZGetCombatInterface()->GetObserverMode() && ZGetGame()->GetMatch()->GetRoundState() == MMATCH_ROUNDSTATE_FINISH &&
				m_nCurrSector + 1 < GetRoundMax())
			{
				pLabel->Show(true);
				pLabel->SetText("You will be redirected to the next stage soon.");
			}
		}
	}
}

ZActorWithFSM* ZRuleQuestChallenge::SpawnActor(const char* szActorDefName, MUID& uid, int nCustomSpawnTypeIndex, int nSpawnIndex, bool bMyControl)
{
	ZActorWithFSM* pActor =  IRuleNewQuest::SpawnActor(szActorDefName, uid, nCustomSpawnTypeIndex, nSpawnIndex, bMyControl);
	if (pActor == nullptr)
	{
		return nullptr;
	}
	if (pActor->GetActorDef()->IsBoss())
	{
		m_uidBoss = pActor->GetUID();
		if (StrStr(pActor->GetActorDef()->GetName(), "tower"))
			pActor->SetStationary(true);
	}
	return pActor;
}

ZActorWithFSM* ZRuleQuestChallenge::SpawnActorSummon(const char* szActorName, MUID& uid, int nNum, bool bMyControl,
	MShortVector vec, MShortVector vec2, int route)
{
	return IRuleNewQuest::SpawnActorSummon(szActorName, uid, nNum, bMyControl, vec, vec2, route);
}

ZActorWithFSM* ZRuleQuestChallenge::SpawnLateJoinNpc(const char* szActorDefName, MUID& uid, int nCustomSpawnTypeIndex, int nSpawnIndex, bool bMyControl)
{
	ZActorWithFSM* lateJoinNPC = IRuleNewQuest::SpawnLateJoinNpc(szActorDefName, uid, nCustomSpawnTypeIndex, nSpawnIndex, bMyControl);
	ZGetEffectManager()->AddReBirthEffect(lateJoinNPC->GetPosition());

	return lateJoinNPC;
}