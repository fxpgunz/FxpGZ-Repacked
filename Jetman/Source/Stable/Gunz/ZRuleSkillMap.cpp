#include "stdafx.h"
#include "ZRuleSkillMap.h"
#include "ZScreenEffectManager.h"
#include "ZBmNumLabel.h"
#include "MMatchWorldItemDesc.h"
#include "../CSCommon/SkillMap.h"
ZRuleSkillMap::ZRuleSkillMap(ZMatch* pMatch) : ZRule(pMatch)
{
	m_chestGrabber = MUID(0, 0);
	m_roundStartTime = 15000;
	currRoundTime = 0;
	myBestTime = 0;
}

ZRuleSkillMap::~ZRuleSkillMap()
{

}


bool ZRuleSkillMap::OnCommand(MCommand* command)
{
	switch (command->GetID())
	{
		case MC_SKILLMAP_OBTAIN_ITEM:
		{
			MUID targetPlayer;
			int xp, bp;
			//DWORD chestRespawnTime;
			command->GetParameter(&targetPlayer, 0, MPT_UID);
			command->GetParameter(&xp, 1, MPT_INT);
			command->GetParameter(&bp, 2, MPT_INT);

			int nLevelPct = GetExpPercentFromTransData(xp);
			ZCharacter* pCharacter = (ZCharacter*)ZGetCharacterManager()->Find(targetPlayer);


			if (pCharacter == nullptr)
				break;

			if (pCharacter == ZGetGame()->m_pMyCharacter)
			{
				char szText[256];
				ZGetMyInfo()->SetXP(ZGetMyInfo()->GetXP() + xp);
				ZGetMyInfo()->SetLevelPercent(nLevelPct);
				ZGetScreenEffectManager()->SetGaugeExpFromMyInfo();
				ZGetMyInfo()->SetBP(ZGetMyInfo()->GetBP() + bp);
				ZGetScreenEffectManager()->AddExpEffect(xp);		// È¹µæ °æÇèÄ¡ Ç¥½Ã ¾ø¾Ö±â
				sprintf(szText, "You gain %d XP and %d BP for grabbing the Chest!", xp, bp);
				ZChatOutput(ZCOLOR_CHAT_SYSTEM, szText);
			}
			///TODO: Add losses to other players
			pCharacter->GetStatus().CheckCrc();
			pCharacter->GetStatus().Ref().nExp += xp;
			pCharacter->GetStatus().Ref().nKills += 1;
			pCharacter->GetStatus().MakeCrc();

			for (auto itor = ZGetCharacterManager()->begin(); itor != ZGetCharacterManager()->end(); ++itor)
			{
				ZCharacter* otherPlayer = (ZCharacter*)(*itor).second;
				if (otherPlayer != pCharacter) {
					otherPlayer->GetStatus().CheckCrc();
					otherPlayer->GetStatus().Ref().AddDeaths();
					otherPlayer->GetStatus().MakeCrc();
				}

			}

			m_chestGrabber = pCharacter->GetUID();

		} return true;
		case MC_MATCH_RESPONSE_SKILLMAP_BESTTIME:
		{
			MUID player;
			unsigned int bestTime;
			command->GetParameter(&player, 0, MPT_UID);
			command->GetParameter(&bestTime, 1, MPT_UINT);

			ZCharacter* Object = static_cast<ZCharacter*>(ZGetCharacterManager()->Find(player));
			if (Object == nullptr)
				return false;
			if (player == ZGetMyUID())
			{
				ZGetGame()->m_pMyCharacter->SetSkillMapBestTime(bestTime);
				myBestTime = bestTime;

			}

		} return true;
		default:
			break;
	}
	return false;
}

void ZRuleSkillMap::OnDraw(MDrawContext* pDC)
{
	int nMargin[16] = { 46,46,46,46,46,46,46,46,46,46,20,20,20 };
	ZBmNumLabel* bmNumLabel = (ZBmNumLabel*)ZGetGameInterface()->GetIDLResource()->FindWidget("Spy_CountDownTime");
	ZBmNumLabel* bmNumLabelRed = (ZBmNumLabel*)ZGetGameInterface()->GetIDLResource()->FindWidget("Spy_CountDownTime_Red");
	if (m_setDrawStartNotice == true)
	{
		ZBmNumLabel* bmNumLabel = (ZBmNumLabel*)ZGetGameInterface()->GetIDLResource()->FindWidget("Spy_CountDownTime");
		if (bmNumLabel)
		{
			char szText[256];
			sprintf(szText, "%02d,%02d,%02d", (((int)m_roundStartTime / 1000) / 60), (((int)m_roundStartTime / 1000) % 60), ((int)m_roundStartTime / 10) % 100);
			bmNumLabel->SetText(szText);
			bmNumLabel->SetAlignmentMode(MAM_HCENTER);
			bmNumLabel->SetCharMargin(nMargin);
			bmNumLabel->Draw(pDC);
			bmNumLabel->Show(true);
		}
		if (bmNumLabelRed && m_roundStartTime <= 5000)
		{
			bool bShow = ((m_roundStartTime % (300 * 2)) < 300);
			char szText[256];
			sprintf(szText, "%02d,%02d,%02d", (((int)m_roundStartTime / 1000) / 60), (((int)m_roundStartTime / 1000) % 60), ((int)m_roundStartTime / 10) % 100);
			bmNumLabelRed->SetText(szText);
			bmNumLabelRed->SetAlignmentMode(MAM_HCENTER);
			bmNumLabelRed->SetCharMargin(nMargin);
			bmNumLabelRed->Show(bShow);
			bmNumLabelRed->Draw(pDC);
		}
	}
	else {
		char szText[256];
		sprintf(szText, "%02d,%02d,%02d", (((int)currRoundTime / 1000) / 60), (((int)currRoundTime / 1000) % 60), ((int)currRoundTime / 10) % 100);
		bmNumLabel->SetText(szText);
		bmNumLabel->SetAlignmentMode(MAM_HCENTER);
		bmNumLabel->SetCharMargin(nMargin);
		bmNumLabel->Draw(pDC);
		bmNumLabel->Show(true);
		bmNumLabelRed->Show(false);
	}
	if (myBestTime > 0) {
		char bestTime[48];//oversized, but safe
		sprintf_s(bestTime, "My Best - %02d : %02d : %02d", static_cast<int>(myBestTime / 1000 / 60), static_cast<int>(myBestTime / 1000 % 60), ((int)myBestTime / 10) % 100);
		pDC->Text(400.f/800.f, 100.f/600.f, bestTime);
	}
}

void ZRuleSkillMap::OnUpdateUI(const DWORD& newBestTime)
{
	myBestTime = newBestTime;
}

void ZRuleSkillMap::OnUpdate(float fElapsed)
{
	///prevent death by falling
	if (ZGetGame()->m_pMyCharacter->GetPosition().z <= -2400.f)
	{
		ZMapSpawnData* spawnData = ZGetGame()->GetWorld()->GetDesc()->GetSpawnManager()->GetSoloRandomData();
		ZGetGame()->m_pMyCharacter->SetPosition(spawnData->m_Pos);

		ZGetEffectManager()->AddReBirthEffect(ZGetGame()->m_pMyCharacter->GetPosition());
	}
	if (ZGetGame()->GetMatch()->GetRoundState() == MMATCH_ROUNDSTATE_COUNTDOWN)
	{
		if (m_roundStartTime > 0)
			m_roundStartTime -= fElapsed * 1000;
	}
	if (ZGetGame()->GetMatch()->GetRoundState() == MMATCH_ROUNDSTATE_PLAY) {
		currRoundTime += fElapsed * 1000;
	}
}

void ZRuleSkillMap::OnSetRoundState(MMATCH_ROUNDSTATE roundState)
{
	switch (roundState)
	{
		case MMATCH_ROUNDSTATE_PREPARE:
		{
			m_chestGrabber.SetInvalid();
			m_roundStartTime = 15000;
			currRoundTime = 0;
			ZPostRequestSkillMapBestTime(ZGetGame()->m_pMyCharacter->GetUID(), MGetMapDescMgr()->GetMapID(ZGetGame()->GetMatch()->GetMapName()));
		}break;
		case MMATCH_ROUNDSTATE_COUNTDOWN:
		{
			m_setDrawStartNotice = true;
		}break;
		case MMATCH_ROUNDSTATE_PLAY: {
			m_setDrawStartNotice = false;
		}break;
		case MMATCH_ROUNDSTATE_FINISH: 
		{
		}break;
	}
}

void ZRuleSkillMap::AfterCommandProcessed(MCommand* command)
{
	///originally was goinjg to post besttime, but no point,b/c that's done in roundstate_finish
}