#include "stdafx.h"
#include "ZRuleBlitzKrieg.h"
#include "ZActorActionManager.h"
#include "MActorDef.h"
#include "ZBmNumLabel.h"
#include "ZTips.h"
#include <chrono>
#include "../CSCommon/Blitz.h"
#include "ZScreenEffectManager.h"
#include "ZCharacter.h"
#include "MDrawContext.h"
#include "Mint.h"
enum HONOR_SOUND {
	HS_LESSGAIN,
	HS_REGULARGAIN,
	HS_MOREGAIN,

	HS_END
};

ZRuleBlitzKrieg::ZRuleBlitzKrieg(ZMatch* pMatch) : IRuleNewQuest(pMatch)
{
	m_redBarricades = 12;
	m_blueBarricades = 12;
	m_tresureChests = 0;
	m_bossBlueUID = MUID(0, 0);
	m_bossRedUID = MUID(0, 0);
	m_drawHPTopInfo = false;
	m_updateTips = 0;
	m_updateHonorTick = 0;
	m_roundStartTime = 0;
}

ZRuleBlitzKrieg::~ZRuleBlitzKrieg()
{

}

bool ZRuleBlitzKrieg::Init()
{
	if (!IRuleNewQuest::Init())
		return false;

	return true;
}

bool ZRuleBlitzKrieg::OnCommand(MCommand* pCmd)
{
	if (IRuleNewQuest::OnCommand(pCmd))
		return true;

	switch (pCmd->GetID())
	{
		case MC_BLITZ_START_HONOR:
		{
			int startHonor;
			MUID targetPlayer;
			pCmd->GetParameter(&targetPlayer, 0, MPT_UID);
			pCmd->GetParameter(&startHonor, 1, MPT_INT);
			ZCharacter* pCharacter = dynamic_cast<ZCharacter*>(ZGetCharacterManager()->Find(targetPlayer));
			if (pCharacter == nullptr)
				return false;

			pCharacter->AddHonor(startHonor);
		
		}return true;
		case MC_BLITZ_PLAYER_KILL:
		{
			MUID killer, victim;
			pCmd->GetParameter(&killer, 0, MPT_UID);
			pCmd->GetParameter(&victim, 1, MPT_UID);

			AddPlayerKillReward(killer, victim);

		}return true;
		///TODO: handle this!
		case MC_BLITZ_RESPONSE_UPGRADE:
		{
			MUID targetPlayer;
			pCmd->GetParameter(&targetPlayer, 0, MPT_UID);
			ZCharacter* player = dynamic_cast<ZCharacter*>(ZGetCharacterManager()->Find(targetPlayer));
			if (player == nullptr)
				break;

			int keyIndex, honorCost, newHonorLevel = 0;
			pCmd->GetParameter(&keyIndex, 1, MPT_INT);
			pCmd->GetParameter(&honorCost, 2, MPT_INT);
			pCmd->GetParameter(&newHonorLevel, 3, MPT_INT);

			player->m_blitzUpgradeLevel[keyIndex].at(0) = newHonorLevel;
			
			if (player->GetUID() == ZGetMyUID())
				player->ReduceHonor(honorCost);

			UpdateUpgradeUI(keyIndex, newHonorLevel);
		}return true;
		case MC_BLITZ_RESPONSE_CLASS:
		{
			MUID requester;
			MMatchObjectClass classID;
			pCmd->GetParameter(&requester, 0, MPT_UID);
			pCmd->GetParameter(&classID, 1, MPT_INT);

			ZCharacter* player = (ZCharacter*)ZGetCharacterManager()->Find(requester);
			if (player == nullptr)
				return false;

			player->SetBlitzClass(classID);
			///Only update the text one time, that way unnecessary draws are mitigated
			UpdateClassSelectDesc(classID);

		} return true;
		default:
			break;
		///TODO: make buff serversided
		/*		case MC_BLITZ_CLASSBUFF:
		{
			MUID player;

		}*/
	}

	return false;
}

void ZRuleBlitzKrieg::OnUpdate(float fDelta)
{
	IRuleNewQuest::OnUpdate(fDelta);

	if (ZGetGame()->GetMatch()->GetRoundState() == MMATCH_ROUNDSTATE_PLAY)
	{
		if (m_updateHonorTick + (MGetBlitz()->GetPlayerHonorInfo().honorIncreaseTime * 1000) < GetElapsedTime())
		{
			ZGetGame()->m_pMyCharacter->AddHonor(MGetBlitz()->GetPlayerHonorInfo().honorIncreaseValue);
			m_updateHonorTick = GetElapsedTime();
		}
	}
	if (ZGetGame()->GetMatch()->GetRoundState() == MMATCH_ROUNDSTATE_COUNTDOWN)
	{
		if (m_roundStartTime > 0)
			m_roundStartTime -= fDelta * 1000;
	}
}

void ZRuleBlitzKrieg::OnSetRoundState(MMATCH_ROUNDSTATE roundState)
{
	//TODO: fill in roundstate info from xml's
	switch (roundState)
	{
		case MMATCH_ROUNDSTATE_PREPARE:
		{
			ZGetCombatInterface()->SetShowBlitzHelp(false);
			ZGetCombatInterface()->SetShowBlitzMap(false);
			ZGetCombatInterface()->SetShowUpgradeList(false);
		}break;
		case MMATCH_ROUNDSTATE_COUNTDOWN:
		{
			m_roundStartTime = 30000;
			///temporary, remove after the interface is done
			ZPostRequestBlitzClass(MOC_HUNTER);
		}break;
		case MMATCH_ROUNDSTATE_PLAY:
		{
			m_drawHPTopInfo = true;
			DrawHPTop(m_drawHPTopInfo);

			ZGetCombatInterface()->SetShowBlitzMap(true);
			ZGetCombatInterface()->SetShowUpgradeList(true);
		}break;
	}
}

ZActorWithFSM* ZRuleBlitzKrieg::SpawnActor(const char* szActorDefName, MUID& uid, int nCustomSpawnTypeIndex, int nSpawnIndex, bool bMyControl)
{
	ZActorWithFSM* pActor =  IRuleNewQuest::SpawnActor(szActorDefName, uid, nCustomSpawnTypeIndex, nSpawnIndex, bMyControl);
	if (!pActor)
		return nullptr;

	ZMapSpawnManager* pMSM = ZGetGame()->GetMapDesc()->GetSpawnManager();
	ZMapSpawnData* pSpawnData;
	if (pMSM)
	{
		if (_stricmp(szActorDefName, "radar_red") == 0)
		{
			pSpawnData = pMSM->GetSpawnData(ZMST_SPAWN_RADAR_RED, nSpawnIndex);
			pActor->SetTeamID(MMT_RED);
			m_bossRedUID = pActor->GetUID();
			pActor->SetStationary(true);
		}
		else if (_stricmp(szActorDefName, "radar_blue") == 0)
		{
			pSpawnData = pMSM->GetSpawnData(ZMST_SPAWN_RADAR_BLUE, nSpawnIndex);
			pActor->SetTeamID(MMT_BLUE);
			m_bossBlueUID = pActor->GetUID();
			pActor->SetStationary(true);
		}
		else if (_stricmp(szActorDefName, "barricade_blue") == 0)
		{
			pSpawnData = pMSM->GetSpawnData(ZMST_SPAWN_BARRICADE_BLUE, nSpawnIndex);
			pActor->SetTeamID(MMT_BLUE);
			pActor->SetStationary(true);
			pActor->SetCollideable(false);
		}
		else if (_stricmp(szActorDefName, "barricade_red") == 0)
		{
			pSpawnData = pMSM->GetSpawnData(ZMST_SPAWN_BARRICADE_RED, nSpawnIndex);
			pActor->SetTeamID(MMT_RED);
			pActor->SetStationary(true);
			pActor->SetCollideable(false);
		}
		else if (_stricmp(szActorDefName, "guardian") == 0)
		{
			pSpawnData = pMSM->GetSpawnData(ZMST_SPAWN_GUARDIAN,nSpawnIndex);
			if (StrStr(pSpawnData->m_szSpawnName, "guardian_r"))
				pActor->SetTeamID(MMT_RED);
			else
				pActor->SetTeamID(MMT_BLUE);

			pActor->SetStationary(true);
		}
		else if (StrStr(szActorDefName, "tresure"))
		{
			///TODO: handle drawing bitmap for treasure spawn
			pSpawnData = pMSM->GetSpawnData(ZMST_SPAWN_TRESURE, nSpawnIndex);
			pActor->SetTeamID(MMT_ALL);
			pActor->SetStationary(true);

			DrawAnnouncement(SA_EVNTMSG6);

			m_tresureChests++;
		}
	}
	pActor->SetPosition(pSpawnData->m_Pos);
	pActor->SetDirection(pSpawnData->m_Dir);
	ApplyActorName(pActor);
	if (_stricmp(pActor->GetActorDef()->GetActorType(), "terminator") == 0)
	{
		if (pActor->GetTeamID() == (ZGetGame() ? ZGetGame()->m_pMyCharacter->GetTeamID() : MMatchTeam::MMT_END))
			DrawAnnouncement(SPECIAL_ANNOUNCEMENT::SA_EVNTMSG15);
		else
			DrawAnnouncement(SPECIAL_ANNOUNCEMENT::SA_EVNTMSG14);
	}
	return pActor;
}

ZActorWithFSM* ZRuleBlitzKrieg::SpawnActorSummon(const char* szActorName, MUID& uid, int nNum, bool bMyControl, MShortVector vec, MShortVector vec2,
	int route)
{
	ZActorWithFSM* pActor = IRuleNewQuest::SpawnActorSummon(szActorName, uid, nNum,  bMyControl, vec,vec2,route);

	if (StrStr(szActorName, "red"))
	{
		pActor->SetTeamID(MMT_RED);
	}
	else if (StrStr(szActorName, "blue"))
	{
		pActor->SetTeamID(MMT_BLUE);
	}

	ApplyActorName(pActor);


	return pActor;
}

//handles hardcoded event
void ZRuleBlitzKrieg::CheckSpecialEvent(ZActorWithFSM* pDeadActor, MMatchTeam nTeamID)
{
	AddHonorReward(pDeadActor,pDeadActor->GetLastAttacker());

	ZActorWithFSM* pBoss = NULL;
	if (_stricmp(pDeadActor->GetActorDef()->GetActorType(), "barricade") != 0)
	{
		if (!_stricmp(pDeadActor->GetActorDef()->GetActorType(), "honor_item"))
		{
			m_tresureChests--;
			return;
		}
		else
			return;
	}
	if (nTeamID > MMT_BLUE || nTeamID < MMT_RED)
		return;

	int nTeamBarricadeCount = 0;
	if (nTeamID == MMT_RED)
	{
		m_redBarricades--;
		nTeamBarricadeCount = m_redBarricades;
		if (m_redBarricades != 0)
			pBoss = (ZActorWithFSM*)ZGetObjectManager()->GetNPCObject(m_bossRedUID);
		else
			pBoss = (ZActorWithFSM*)ZGetObjectManager()->GetNPCObject(m_bossBlueUID);
	}
	else if (nTeamID == MMT_BLUE)
	{
		m_blueBarricades--;
		nTeamBarricadeCount = m_blueBarricades;
		if (m_blueBarricades != 0)
			pBoss = (ZActorWithFSM*)ZGetObjectManager()->GetNPCObject(m_bossBlueUID);
		else
			pBoss = (ZActorWithFSM*)ZGetObjectManager()->GetNPCObject(m_bossRedUID);
	}
	pBoss->SpawnSpecialNpc(nTeamBarricadeCount);
}

void ZRuleBlitzKrieg::OnDraw(MDrawContext* pDC)
{
	MWidget* pWidget = ZGetGameInterface()->GetIDLResource()->FindWidget("Blitzkrieg");
	if (pWidget)
	{
		pWidget->Show(ZGetCombatInterface()->IsShowUI());
		if(pWidget->IsVisible())
		{
				DrawMap(pDC, ZGetCombatInterface()->GetShowBlitzMap());
				DrawHelpScreen(pDC, ZGetCombatInterface()->GetShowBlitzHelp());
				DrawClassSelect(pDC, ZGetGame()->GetMatch()->GetRoundState() == MMATCH_ROUNDSTATE_COUNTDOWN);
				DrawUpgradeList(pDC, ZGetCombatInterface()->GetShowUpgradeList());
		}
	}
}

void ZRuleBlitzKrieg::DrawHPTop(bool draw)
{
	MPicture* pPicture = (MPicture*)ZGetGameInterface()->GetIDLResource()->FindWidget("BlitrzKriegTopInfo");
	if (pPicture)
	{
		pPicture->Show(draw);
		if (!pPicture->IsVisible())
			return;
	}
}

void ZRuleBlitzKrieg::DrawMap(MDrawContext* pDC, bool draw)
{
	MPicture* pPicture = (MPicture*)ZGetGameInterface()->GetIDLResource()->FindWidget("BlitzkriegBarricadeBg");
	if (pPicture)
	{
		pPicture->Show(draw);
		if (!pPicture->IsVisible())
			return;
		

		MLabel* pLabel = (MLabel*)ZGetGameInterface()->GetIDLResource()->FindWidget("BlitzkriegBarricade_Toggle");
		if (pLabel)
		{
			pLabel->SetText("[G] to switch to help screen");
		}
		pLabel = (MLabel*)ZGetGameInterface()->GetIDLResource()->FindWidget("BlitzkriegBarricade_KeyLabel");
		if (pLabel)
			pLabel->Show(draw);

		MBitmap* pBitmapBarricadeMyTeam = MBitmapManager::Get(ZGetGame()->m_pMyCharacter->GetTeamID() == MMT_RED ? "Blitz_Barricade_Red.png" : "Blitz_Barricade_Blue.png");
		if (!pBitmapBarricadeMyTeam)
			return;

		MBitmap* pBitmapBarricadeEnemy = MBitmapManager::Get(ZGetGame()->m_pMyCharacter->GetTeamID() == MMT_RED ? "Blitz_Barricade_Blue.png" : "Blitz_Barricade_Red.png");
		if (!pBitmapBarricadeEnemy)
			return;


		for (int i = 0; i < 12; ++i)
		{


			char szBuff[256];
			sprintf_s(szBuff, sizeof(szBuff), "%s%i", "BlitzkriegBarricade_MyTeam_", i);
			pPicture = static_cast<MPicture*>(ZGetGameInterface()->GetIDLResource()->FindWidget(szBuff));
			if (!pPicture)
				break;

			pPicture->SetBitmap(pBitmapBarricadeMyTeam);

			sprintf_s(szBuff, sizeof(szBuff), "%s%i", "BlitzkriegBarricade_Enemy_", i);
			pPicture = static_cast<MPicture*>(ZGetGameInterface()->GetIDLResource()->FindWidget(szBuff));
			if (!pPicture)
				break;

			pPicture->SetBitmap(pBitmapBarricadeEnemy);
		}
		for (int i = 0; i < m_tresureChests; ++i)
		{
			char sztext[38];
			sprintf_s(sztext, "BlitzkriegBarricade_HonorItem_%d", i);
			pPicture = (MPicture*)ZGetGameInterface()->GetIDLResource()->FindWidget(sztext);
			if (pPicture)
				pPicture->SetBitmap(MBitmapManager::Get("Blitz_HonorItem.png"));
		}
		///TODO: radar bitmaps
	}
}

void ZRuleBlitzKrieg::DrawHelpScreen(MDrawContext* pDC, bool draw)
{
	MPicture* pPicture = (MPicture*)ZGetGameInterface()->GetIDLResource()->FindWidget("BlitzkriegHelp");
	pPicture->Show(draw);
	if (pPicture->IsVisible())
	{
		MLabel* pLabel = (MLabel*)ZGetGameInterface()->GetIDLResource()->FindWidget("BlitzHelp_Toggle");
		if (pLabel)
			pLabel->SetText("[H] to open the Map.");

		pLabel = (MLabel*)ZGetGameInterface()->GetIDLResource()->FindWidget("BlitzHelp_KeyLabel");
		if (pLabel)
			pLabel->SetText("TODO: Key Label");

		MTextArea* pTextArea = (MTextArea*)ZGetGameInterface()->GetIDLResource()->FindWidget("BlitzHelp_TextArea");
		if (pTextArea)
		{
			if (m_updateTips + 12000 < GetElapsedTime())
			{
				pTextArea->Clear();
				pTextArea->SetText(ZGetGameInterface()->GetTips()->GetTips(ZTIPS_CATEGORY_BLITZKRIEG, 1));
				m_updateTips = GetElapsedTime();
			}

		}
	}
}

void ZRuleBlitzKrieg::DrawClassSelect(MDrawContext* pDC, bool draw)
{
	MPicture* pPicture = (MPicture*)ZGetGameInterface()->GetIDLResource()->FindWidget("BlitzClassSelect");
	if (pPicture)
	{
		pPicture->Show(draw);
		if (pPicture->IsVisible())
		{
			string background = "BlitzKriegClassSelect_Frame_";
			background.append(ZGetGame()->m_pMyCharacter->GetTeamID() == MMT_RED ? "Red.png" : "Blue.png");
			pPicture->SetBitmap(MBitmapManager::Get(background.c_str()));

			background = "Blitz_ClassSelectTime";
			background.append(ZGetGame()->m_pMyCharacter->GetTeamID() == MMT_RED ? "Red" : "Blue");
			ZBmNumLabel* numLabel = dynamic_cast<ZBmNumLabel*>(ZGetGameInterface()->GetIDLResource()->FindWidget(background.c_str()));
			if (numLabel != nullptr)
			{
				numLabel->Show(draw);
				numLabel->SetText(to_string(static_cast<int>((m_roundStartTime / 1000) % 60)).c_str());
			}
		}
/*		MBmButton* bmNum = (MBmButton*)ZGetGameInterface()->GetIDLResource()->FindWidget("Terrorist_Red");
		if (bmNum)
		{
			bmNum->Show(false);
		}*/
	}
}

void ZRuleBlitzKrieg::DrawUpgradeList(MDrawContext* pDC, bool draw)
{
	if (!ZGetCombatInterface()->GetShowUpgradeList())
	{
		MPicture* abilityMainFrame = dynamic_cast<MPicture*>(ZGetGameInterface()->GetIDLResource()->FindWidget("BlitzAbilityList_Close"));
		abilityMainFrame->Show(true);
	}
	else
	{

	}

}

void ZRuleBlitzKrieg::ApplyActorName(ZActorWithFSM* pActor)
{
	if (!_stricmp(pActor->GetActorDef()->GetActorType(), "radar"))
	{
		pActor->SetActorName(MGetStringResManager()->GetStringFromXml("STR:UI_BLITZ_30"));
	}
	else if (!_stricmp(pActor->GetActorDef()->GetActorType(), "barricade"))
	{
		pActor->SetActorName(MGetStringResManager()->GetStringFromXml("STR:UI_BLITZ_31"));
	}
	else if (!_stricmp(pActor->GetActorDef()->GetActorType(), "knifeman"))
	{
		pActor->SetActorName(MGetStringResManager()->GetStringFromXml("STR:UI_BLITZ_35"));
	}
	else if (!_stricmp(pActor->GetActorDef()->GetActorType(), "honor_item"))
	{
		pActor->SetActorName(MGetStringResManager()->GetStringFromXml("STR:UI_BLITZ_32"));
	}
	else if (!_stricmp(pActor->GetActorDef()->GetActorType(), "terminator"))
	{
		pActor->SetActorName(MGetStringResManager()->GetStringFromXml("STR:UI_BLITZ_39"));
	}
	else if (!_stricmp(pActor->GetActorDef()->GetActorType(), "knight"))
	{
		pActor->SetActorName(MGetStringResManager()->GetStringFromXml("STR:UI_BLITZ_34"));;
	}
	
}

///TODO: determine how to handle previous attacker vs current attacker?
void ZRuleBlitzKrieg::AddHonorReward(ZActorWithFSM* pActor, MUID uidPlayer)
{
	ZCharacter* pCharacter = static_cast<ZCharacter*>(ZGetCharacterManager()->Find(uidPlayer));
	if (!pCharacter || pCharacter->IsNPC() || !pCharacter->GetInitialized())
		return;

	const char* actorType = pActor->GetActorDef()->GetActorType();

	if (pCharacter && pCharacter->GetUID() == pActor->GetLastAttacker()) {
		pCharacter->AddHonor(MGetBlitz()->GetHonorKillReward(actorType).singleKillReward);
		ZGetSoundEngine()->PlaySound(const_cast<char*>(MGetBlitz()->GetHonorRewardSound(BE_REGULARGAIN).c_str()));
	}
	for (auto itor = ZGetCharacterManager()->begin(); itor != ZGetCharacterManager()->end(); ++itor)
	{
		ZCharacter* pChar = static_cast<ZCharacter*>((*itor).second);
		if (pChar != nullptr)
			pChar->AddHonor(MGetBlitz()->GetHonorKillReward(actorType).allKillReward);

		ZGetSoundEngine()->PlaySound(const_cast<char*>(MGetBlitz()->GetHonorRewardSound(BE_MOREGAIN).c_str()));

	}
	if (!_stricmp(actorType, "honor_item"))
	{
		ZGetEffectManager()->AddBlitzBuff(pCharacter, BE_HONORITEM);
	}
	else
	{
		ZGetEffectManager()->AddBlitzBuff(pCharacter, BE_REGULARGAIN);
	}
}

void ZRuleBlitzKrieg::AddPlayerKillReward(const MUID& uidAttacker, const MUID& uidVictim)
{
	ZCharacter* pAttacker = dynamic_cast<ZCharacter*>(ZGetCharacterManager()->Find(uidAttacker));
	ZCharacter* pVictim = dynamic_cast<ZCharacter*>(ZGetCharacterManager()->Find(uidVictim));
	if (!pAttacker || !pVictim)
		return;
	//TODO: handle kill/assist delay.

	pAttacker->AddHonor(MGetBlitz()->GetPlayerRewardKill().singleKill);
	//i assume that the assist gets the less/regular gain, while the killer gets the more gain???
	ZGetEffectManager()->AddBlitzBuff(pAttacker, BE_REGULARGAIN);

	ZGetSoundEngine()->PlaySound(const_cast<char*>(MGetBlitz()->GetHonorRewardSound(BE_REGULARGAIN).c_str()));

	///TODO: determine how to handle assists vs main kill
	//if (pAttacker->GetUID() == pVictim->GetLastAttacker()) {
	//	///TODO: blitzkrieg xml for player honor kill
	//	pAttacker->AddHonor(MGetBlitz()->GetPl)
	//}
}

///TODO: Get this working
void ZRuleBlitzKrieg::DrawAnnouncement(SPECIAL_ANNOUNCEMENT announcementType)
{
	MPicture* childWidget = static_cast<MPicture*>(ZGetGameInterface()->GetIDLResource()->FindWidget("BlitzEventMsg1"));
	if (!childWidget)
		return;
	float drawTime = 3000.f;
	switch (announcementType)
	{
		case SA_EVNTMSG6:
		{
			MBitmap* treasureBitmap = MBitmapManager::Get("Blitz_EventMsg_6.png");
			if (treasureBitmap) {
				childWidget->SetBitmap(treasureBitmap);
				while (drawTime > 0)
				{
					childWidget->SetVisible(true);
					childWidget->Show(true);
					--drawTime;
				}
				childWidget->SetVisible(false);
				childWidget->Show(false);
			}
		}
		case SA_EVNTMSG14:
		{
			MBitmap* enemyBitmap = MBitmapManager::Get("Blitz_EventMsg_14.png");
			if (enemyBitmap)
			{
				childWidget->SetBitmap(enemyBitmap);
				childWidget->Show(true);
			}
		}
		case SA_EVNTMSG15:
		{
			MBitmap* teamBitmap = MBitmapManager::Get("Blitz_EventMsg_15.png");
			if (teamBitmap)
			{
				childWidget->SetBitmap(teamBitmap);
				childWidget->Show(true);
			}
		}
	}
}


//Custom: latejoin
ZActorWithFSM* ZRuleBlitzKrieg::SpawnLateJoinNpc(const char* szActorDefName, MUID& uid, int ncustomSpawnTypeIndex, int nspawnIndex, bool bMyControl)
{
	ZActorWithFSM* pActor = IRuleNewQuest::SpawnLateJoinNpc(szActorDefName, uid, ncustomSpawnTypeIndex, nspawnIndex);
	if (!pActor)
		return nullptr;
	//TODO: change the code below to fix latejoin
	///TODO: do i want latejoin? probably not, wont work well b/c no class selection, unless i default the user

	ZMapSpawnManager* pMSM = ZGetGame()->GetMapDesc()->GetSpawnManager();
	ZMapSpawnData* pSpawnData = nullptr;
	if (pMSM)
	{
		if (_stricmp(szActorDefName, "radar_red") == 0)
		{
			pSpawnData = pMSM->GetSpawnData(ZMST_SPAWN_RADAR_RED, nspawnIndex);
			pActor->SetTeamID(MMT_RED);
			m_bossRedUID = pActor->GetUID();
		}
		else if (_stricmp(szActorDefName, "radar_blue") == 0)
		{
			pSpawnData = pMSM->GetSpawnData(ZMST_SPAWN_RADAR_BLUE, nspawnIndex);
			pActor->SetTeamID(MMT_BLUE);
			m_bossBlueUID = pActor->GetUID();
		}
		else if (_stricmp(szActorDefName, "barricade_blue") == 0)
		{
			pSpawnData = pMSM->GetSpawnData(ZMST_SPAWN_BARRICADE_BLUE, nspawnIndex);
			pActor->SetTeamID(MMT_BLUE);
			pActor->SetCollideable(false);
		}
		else if (_stricmp(szActorDefName, "barricade_red") == 0)
		{
			pSpawnData = pMSM->GetSpawnData(ZMST_SPAWN_BARRICADE_RED, nspawnIndex);
			pActor->SetTeamID(MMT_RED);
			pActor->SetCollideable(false);
		}
		else if (_stricmp(szActorDefName, "guardian") == 0)
		{
			pSpawnData = pMSM->GetSpawnData(ZMST_SPAWN_GUARDIAN,nspawnIndex);
			if (StrStr(pSpawnData->m_szSpawnName, "guardian_r"))
				pActor->SetTeamID(MMT_RED);
			else
				pActor->SetTeamID(MMT_BLUE);
		}
		else if (StrStr(szActorDefName, "tresure"))
		{
			pSpawnData = pMSM->GetSpawnData(ZMST_SPAWN_TRESURE, nspawnIndex);
			pActor->SetTeamID(MMT_ALL);
			m_tresureChests++;
		}
	}
	if (pSpawnData)
	{
		pActor->SetPosition(pSpawnData->m_Pos);
		pActor->SetDirection(pSpawnData->m_Dir);
	}
	pActor->SetMyControl(false);
	ApplyActorName(pActor);
	return pActor;
}

MBitmap* ZRuleBlitzKrieg::GetClassBitmap(const int& classID, const MMatchTeam& teamID)
{
	switch (classID)
	{
		case MOC_HUNTER:
		{
			if (teamID == MMT_RED)
				return MBitmapManager::Get("Hunter_Red.png");
			else
				return MBitmapManager::Get("Hunter_blue.png");
		}
		case MOC_SLAUGHTER:
		{
			if (teamID == MMT_RED)
				return MBitmapManager::Get("Slaughter_Red.png");
			else
				return MBitmapManager::Get("Slaughter_blue.png");
		}
		case MOC_TRICKSTER:
		{
			if (teamID == MMT_RED)
				return MBitmapManager::Get("Trickster_Red.png");
			else
				return MBitmapManager::Get("Trickster_blue.png");
		}
		case MOC_GLADIATOR:
		{
			if (teamID == MMT_RED)
				return MBitmapManager::Get("Gladiator_Red.png");
			else
				return MBitmapManager::Get("Gladiator_blue.png");
		}
		case MOC_DUELIST:
		{
			if (teamID == MMT_RED)
				return MBitmapManager::Get("Duelist_Red.png");
			else
				return MBitmapManager::Get("Duelist_blue.png");
		}
		case MOC_INCINERATOR:
		{
			if (teamID == MMT_RED)
				return MBitmapManager::Get("Incinerator_Red.png");
			else
				return MBitmapManager::Get("Incinerator_blue.png");
		}
		case MOC_COMBATOFFICER:
		{
			if (teamID == MMT_RED)
				return MBitmapManager::Get("CombatOfficer_Red.png");
			else
				return MBitmapManager::Get("CombatOfficer_blue.png");
		}
		case MOC_ASSASSIN:
		{
			if (teamID == MMT_RED)
				return MBitmapManager::Get("Assassin_Red.png");
			else
				return MBitmapManager::Get("Assassin_blue.png");
		}
		case MOC_TERRORIST:
		{
			if (teamID == MMT_RED)
				return MBitmapManager::Get("Terrorist_Red.png");
			else
				return MBitmapManager::Get("Terrorist_blue.png");
		}
		default:
			return MBitmapManager::Get("Hunter_Red.png");
			break;
	}
	return nullptr;
}

void ZRuleBlitzKrieg::UpdateClassSelectDesc(const MMatchObjectClass& classID)
{
	bool isCountDown = ZGetGame()->GetMatch()->GetRoundState() == MMATCH_ROUNDSTATE_COUNTDOWN;
	MPicture* pPicture = (MPicture*)ZGetGameInterface()->GetIDLResource()->FindWidget("BlitzClassSelect");
	if (pPicture)
	{
		pPicture->Show(isCountDown);
		if (pPicture->IsVisible())
		{
			MLabel* classNameLabel = dynamic_cast<MLabel*>(ZGetGameInterface()->GetIDLResource()->FindWidget("SelClassNameLabel"));
			if (classNameLabel)
			{
				classNameLabel->Show(isCountDown);
			}

			MLabel* classSimpleDesc = dynamic_cast<MLabel*>(ZGetGameInterface()->GetIDLResource()->FindWidget("ClassSimpleDescription"));
			if (classSimpleDesc != nullptr)
			{
				classSimpleDesc->Show(isCountDown);
			}

			MTextArea* textArea = dynamic_cast<MTextArea*>(ZGetGameInterface()->GetIDLResource()->FindWidget("ClassDetailDescription"));
			if (textArea != nullptr)
			{
				textArea->Show(isCountDown);
			}
			switch (classID)
			{
				case MOC_HUNTER:
				{
					classNameLabel->SetText(MGetStringResManager()->GetStringFromXml("STR:UI_BLITZ_02"));
					classSimpleDesc->SetText(MGetStringResManager()->GetStringFromXml("STR:UI_BLITZ_11"));
					textArea->SetText(MGetStringResManager()->GetStringFromXml("STR:UI_BLITZ_20"));
				}break;
				case MOC_SLAUGHTER:
				{
					classNameLabel->SetText(MGetStringResManager()->GetStringFromXml("STR:UI_BLITZ_03"));
					classSimpleDesc->SetText(MGetStringResManager()->GetStringFromXml("STR:UI_BLITZ_12"));
					textArea->SetText(MGetStringResManager()->GetStringFromXml("STR:UI_BLITZ_21"));
				}break;
				case MOC_TRICKSTER:
				{
					classNameLabel->SetText(MGetStringResManager()->GetStringFromXml("STR:UI_BLITZ_04"));
					classSimpleDesc->SetText(MGetStringResManager()->GetStringFromXml("STR:UI_BLITZ_13"));
					textArea->SetText(MGetStringResManager()->GetStringFromXml("STR:UI_BLITZ_22"));
				}break;
				case MOC_GLADIATOR:
				{
					classNameLabel->SetText(MGetStringResManager()->GetStringFromXml("STR:UI_BLITZ_05"));
					classSimpleDesc->SetText(MGetStringResManager()->GetStringFromXml("STR:UI_BLITZ_14"));
					textArea->SetText(MGetStringResManager()->GetStringFromXml("STR:UI_BLITZ_23"));
				}break;
				case MOC_DUELIST:
				{
					classNameLabel->SetText(MGetStringResManager()->GetStringFromXml("STR:UI_BLITZ_06"));
					classSimpleDesc->SetText(MGetStringResManager()->GetStringFromXml("STR:UI_BLITZ_15"));
					textArea->SetText(MGetStringResManager()->GetStringFromXml("STR:UI_BLITZ_24"));
				}break;
				case MOC_INCINERATOR:
				{
					classNameLabel->SetText(MGetStringResManager()->GetStringFromXml("STR:UI_BLITZ_07"));
					classSimpleDesc->SetText(MGetStringResManager()->GetStringFromXml("STR:UI_BLITZ_16"));
					textArea->SetText(MGetStringResManager()->GetStringFromXml("STR:UI_BLITZ_25"));
				}break;
				case MOC_COMBATOFFICER:
				{
					classNameLabel->SetText(MGetStringResManager()->GetStringFromXml("STR:UI_BLITZ_08"));
					classSimpleDesc->SetText(MGetStringResManager()->GetStringFromXml("STR:UI_BLITZ_17"));
					textArea->SetText(MGetStringResManager()->GetStringFromXml("STR:UI_BLITZ_26"));
				}break;
				case MOC_ASSASSIN:
				{
					classNameLabel->SetText(MGetStringResManager()->GetStringFromXml("STR:UI_BLITZ_09"));
					classSimpleDesc->SetText(MGetStringResManager()->GetStringFromXml("STR:UI_BLITZ_18"));
					textArea->SetText(MGetStringResManager()->GetStringFromXml("STR:UI_BLITZ_27"));
				}break;
				case MOC_TERRORIST:
				{
					classNameLabel->SetText(MGetStringResManager()->GetStringFromXml("STR:UI_BLITZ_10"));
					classSimpleDesc->SetText(MGetStringResManager()->GetStringFromXml("STR:UI_BLITZ_19"));
					textArea->SetText(MGetStringResManager()->GetStringFromXml("STR:UI_BLITZ_28"));
				}break;
				default:
					break;
			}
		}
	}
	return;
}

void ZRuleBlitzKrieg::UpdateUpgradeUI(int& const index, int& const upgradeLevel)
{

}