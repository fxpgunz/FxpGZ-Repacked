#include "stdafx.h"
#include "ZRuleSpyMode.h"
#include "ZScreenEffectManager.h"
#include "../CSCommon/SpyMode.h"
#include "ZMessages.h"
#include "ZBmNumLabel.h"

ZRuleSpyMode::ZRuleSpyMode(ZMatch* pMatch) : ZRule(pMatch)
{
	m_bRevealTime = false;
}

ZRuleSpyMode::~ZRuleSpyMode()
{
	if (Spies.size() > 0)
		Spies.clear();

	if (SpyItems.size() > 0)
		SpyItems.clear();
}

bool ZRuleSpyMode::OnCommand(MCommand* pCmd)
{
	if (!ZGetGame())
		return false;

	switch (pCmd->GetID())
	{
		case MC_MATCH_SPY_TEAMS:
		{
			MCommandParameter* pParam1 = pCmd->GetParameter(0);
			if (pParam1->GetType() != MPT_BLOB)
				break;
			int hpApBonus;
			pCmd->GetParameter(&hpApBonus, 1, MPT_INT);
			m_HpApBonus = hpApBonus;
			if (Spies.size() > 0)
				Spies.clear();


			void* SpyUIDBlob = pParam1->GetPointer();
			int Count = MGetBlobArrayCount(SpyUIDBlob);
			for (int i = 0; i < Count; ++i)
			{
				MUID uidPlayer = *(MUID*)MGetBlobArrayElement(SpyUIDBlob, i);

				ZCharacter* pCharacter = (ZCharacter*)ZGetCharacterManager()->Find(uidPlayer);
				if (pCharacter != nullptr)
				{
					pCharacter->SetTeamID(MMT_RED);
					Spies.push_back(pCharacter->GetUID());
				}
			}
			for (auto itor = ZGetCharacterManager()->begin(); itor != ZGetCharacterManager()->end(); ++itor)
			{
				ZCharacter* player = (ZCharacter*)(*itor).second;
				if (!IsSpy(player->GetUID()))
					player->SetTeamID(MMT_BLUE);
			}

			return true;
		}break;
		case MC_MATCH_SPY_INFO:
		{

			MCommandParameter* pParam2 = pCmd->GetParameter(0);
			if (pParam2->GetType() != MPT_BLOB)
				break;
			if (SpyItems.size() > 0)
				SpyItems.clear();

			void* SpyItemBlob = pParam2->GetPointer();
			int Count = MGetBlobArrayCount(SpyItemBlob);
			for (int i = 0; i < Count; ++i)
			{
				MTD_SpyItems SpyItem = *(MTD_SpyItems*)MGetBlobArrayElement(SpyItemBlob, i);
				SpyItems.push_back(SpyItem);
			}
			///TODO: test this
			std::random_shuffle(SpyItems.begin(), SpyItems.end());
			return true;
		}break;
		case MC_MATCH_SPY_ROUNDREWARD:
		{
			MUID player;
			unsigned char scoreValue;
			pCmd->GetParameter(&player, 0, MPT_UID);
			pCmd->GetParameter(&scoreValue, 1, MPT_UCHAR);
			ZCharacter* playerObj = (ZCharacter*)ZGetCharacterManager()->Find(player);
			if (playerObj)
				ApplyBonus(playerObj, scoreValue);

			return true;
		}break;
	}
	return false;
}

bool ZRuleSpyMode::IsSpy(const MUID& uidPlayer)
{
	if (std::find(Spies.begin(), Spies.end(), uidPlayer) != Spies.end())
		return true;

	return false;
}

bool ZRuleSpyMode::IsSpyOpenTime()
{
	if(ZGetGame()->GetMatch()->GetRoundState() != MMATCH_ROUNDSTATE_PLAY)
		return false;
	DWORD dwTime = ZGetGame()->GetMatch()->GetRemaindTime();
	DWORD dwLimitTime = MGetSpyMode()->GetSpyMap(ZGetGameClient()->GetMatchStageSetting()->GetMapIndex()).spyRevealTime * 1000;

	if ((int)dwTime <= (int)dwLimitTime)
		return true;

	return false;
}

bool ZRuleSpyMode::IsIntroduceTeam()
{
	if (ZGetGame()->GetMatch()->GetRoundState() != MMATCH_ROUNDSTATE_COUNTDOWN)
		return false;
	DWORD dwTime = ZGetGame()->GetMatch()->GetRemaindTime();
	DWORD dwLimitTime = ZGetGameClient()->GetMatchStageSetting()->GetLimitTime();
	dwLimitTime *= 1000;
	dwTime = (dwLimitTime - dwTime);

	if (dwTime >= dwLimitTime - 3000)
	{
		return true;
	}
	//	ZGetScreenEffectManager()->AddScreenEffect("spy_selection");

	return false;
}

void ZRuleSpyMode::OnSetRoundState(MMATCH_ROUNDSTATE roundState)
{
	switch (roundState)
	{
		case MMATCH_ROUNDSTATE_PREPARE:
		{
			m_bRevealTime = false;
		}break;
		case MMATCH_ROUNDSTATE_PLAY:
		{
			SetTeams();
		}break;
		case MMATCH_ROUNDSTATE_FINISH:
		{
			for (auto itor = ZGetCharacterManager()->begin(); itor != ZGetCharacterManager()->end(); ++itor)
			{
				ZCharacter* player = (ZCharacter*)(*itor).second;
				unsigned long nItemID[MMCIP_END];
				for (int i = 0; i < MMCIP_END; ++i)
				{
					nItemID[i] = player->GetCharInfo()->nEquipedItemDesc[i];
					player->m_Items.EquipItem((MMatchCharItemParts)i, nItemID[i]);
				}
				ZChangeCharParts(player->GetVisualMesh(), player->GetProperty()->nSex, player->GetProperty()->nHair,
					player->GetProperty()->nFace, nItemID);
				//Forces a mesh reset on the melee item

				if (player->m_Items.GetSelectedWeaponType() != MMCIP_MELEE)
					player->ChangeWeapon(MMCIP_MELEE);
				//safety check
				if (player->m_Items.GetItem(MMCIP_MELEE)->GetDesc() != nullptr)
					player->OnChangeWeapon(player->m_Items.GetItem(MMCIP_MELEE)->GetDesc());
			}
		}
	}
}

void ZRuleSpyMode::OnUpdate(float fElapsed)
{
	if (ZGetGame()->GetMatch()->GetRoundState() == MMATCH_ROUNDSTATE_PLAY)
	{
		DWORD dwTime = ZGetGame()->GetMatch()->GetRemaindTime();
		DWORD dwLimitTime = ZGetGameClient()->GetMatchStageSetting()->GetStageSetting()->nLimitTime;
		if (dwLimitTime != 99999)
		{
			dwLimitTime *= 1000;
			if (dwTime <= dwLimitTime)
			{
				dwTime = (dwLimitTime - dwTime);

				if ((dwTime / 60) == 0 && (dwTime % 60) == MGetSpyMode()->GetSpyMap(ZGetGameClient()->GetMatchStageSetting()->GetMapIndex()).spyRevealTime
					&& !m_bRevealTime)
				{
					m_bRevealTime = true;
					ZGetScreenEffectManager()->AddScreenEffect("spy_location");
				}
			}
		}
	}
}

void ZRuleSpyMode::AfterCommandProcessed(MCommand* pCmd)
{
	if (!ZGetGame())
		return;
	switch (pCmd->GetID())
	{
		case MC_MATCH_GAME_DEAD:
		{
			if (!ZGetScreenEffectManager())
			{
				break;
			}
			MUID uidVictim;
			pCmd->GetParameter(&uidVictim, 2, MPT_UID);
			ZCharacter* pCharacter = (ZCharacter*)ZGetCharacterManager()->Find(uidVictim);
			if (!pCharacter)
				break;
			if (pCharacter->GetTeamID() == MMT_RED)
			{
				int nAliveCount = 0;
				for (auto itor = ZGetCharacterManager()->begin(); itor != ZGetCharacterManager()->end(); ++itor)
				{
					ZCharacter* pAliveChar = (ZCharacter*)(*itor).second;
					if (!pAliveChar->GetInitialized()) continue;
					if (pAliveChar->IsDie()) continue;
					if (pAliveChar->GetTeamID() != MMT_RED) continue;
					nAliveCount++;
				}
				DrawRemainingSpies(nAliveCount);
			}
		}break;
	}
}

void ZRuleSpyMode::DrawRemainingSpies(int SpyCount)
{
	switch (SpyCount)
	{
	case 2:
		ZGetScreenEffectManager()->AddScreenEffect("spy_elimination2Remain"); break;
	case 1:
		ZGetScreenEffectManager()->AddScreenEffect("spy_elimination1Remain"); break;
	case 0:
		ZGetScreenEffectManager()->AddScreenEffect("spy_eliminationAllKill"); break;
	default:
		break;
	}
}

void ZRuleSpyMode::Draw(MDrawContext* pDC)
{
	///TODO: update every so often, or is this fine since it's small?
	string strText;

	MWidget* pWidget = (MWidget*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("CombatSpyInfo");
	if (pWidget)
	{
		pWidget->Show(true);
		int nMargin[16] = { 46,46,46,46,46,46,46,46,46,46,20,20,20 };
		ZBmNumLabel* pBmNumLabel = (ZBmNumLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Spy_CountDownTime");
		if (pBmNumLabel)
		{
			pBmNumLabel->Show(true);
			DWORD dwTime = ZGetGame()->GetMatch()->GetRemaindTime();
			DWORD dwLimitTime = MGetSpyMode()->GetSpyMap(ZGetGameClient()->GetMatchStageSetting()->GetMapIndex()).timeLimit * 1000;

			if (dwTime <= dwLimitTime)
			{
				dwTime = (dwLimitTime - dwTime);
				char szText[256];
				sprintf(szText, "%02d,%02d,%02d", (((int)dwTime / 1000) / 60), (((int)dwTime / 1000) % 60), ((int)dwTime / 10) % 100);
				pBmNumLabel->SetText(szText);
				pBmNumLabel->SetAlignmentMode(MAM_HCENTER);
				pBmNumLabel->SetCharMargin(nMargin);
				pBmNumLabel->Draw(pDC);
			}
		}
		ZBmNumLabel* pBmNumLabelRed = (ZBmNumLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Spy_CountDownTime_Red");
		if (pBmNumLabelRed)
		{
			if (IsSpyOpenTime())
			{
				{
					DWORD dwTime = ZGetGame()->GetMatch()->GetRemaindTime();
					DWORD dwLimitTime = MGetSpyMode()->GetSpyMap(ZGetGameClient()->GetMatchStageSetting()->GetMapIndex()).spyRevealTime * 1000;

					if (dwTime <= dwLimitTime)
					{
						dwTime = (dwLimitTime - dwTime);
						bool bShow = ((dwTime % (300 * 2)) < 300);

						char szText[256];
						sprintf(szText, "%02d,%02d,%02d", (((int)dwTime / 1000) / 60), (((int)dwTime / 1000) % 60), ((int)dwTime / 10) % 100);
						pBmNumLabelRed->SetText(szText);
						pBmNumLabelRed->SetAlignmentMode(MAM_HCENTER);
						pBmNumLabelRed->SetCharMargin(nMargin);
						pBmNumLabelRed->Show(bShow);
						pBmNumLabelRed->Draw(pDC);
					}
				}
			}
			else
				pBmNumLabelRed->Show(false);
		}
		MPicture* pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Spy_TipBG");
		if (pPicture)
		{
			pPicture->Show(true);

			ZCharacter *pTargetCharacter = ZGetGameInterface()->GetCombatInterface()->GetTargetCharacter();
			MMatchCharItemParts pSelectedItem = pTargetCharacter->GetItems()->GetSelectedWeaponParts();
			MTextArea* pText = (MTextArea*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Spy_Tip");
			if (pText)
			{
				if (IsSpy(pTargetCharacter->GetUID()))
				{
					if (pSelectedItem == MMCIP_MELEE)
					{
						pText->SetText(ZMsg(MSG_SPY_SWORD));
					}
					else if (pSelectedItem == MMCIP_CUSTOM1)
					{
						pText->Clear();
						pText->SetText(ZMsg(MSG_SPY_CUSTOM1));
					}
					else if (pSelectedItem == MMCIP_CUSTOM2)
					{
						pText->Clear();
						pText->SetText(ZMsg(MSG_SPY_CUSTOM2));
					}

					else if (pSelectedItem == MMCIP_CUSTOM3)
					{
						pText->Clear();
						pText->SetText(ZMsg(MSG_SPY_CUSTOM3));
					}

					else
					{
						pText->Clear();
					}
				}
				else
				{
					if (IsSpyOpenTime())
					{
						pText->SetText(ZMsg(MSG_SPY_TIME_ALMOST_UP));
					}

					if (pSelectedItem == MMCIP_CUSTOM1)
					{
						pText->Clear();
						pText->SetText(ZMsg(MSG_TRACER_CUSTOM1));
					}
					else if (pSelectedItem == MMCIP_CUSTOM2)
					{
						pText->Clear();
						pText->SetText(ZMsg(MSG_TRACER_CUSTOM2));
					}
					else
					{
						pText->Clear();
					}
				}
			}
			pPicture->SetSize(pText->GetFont()->GetWidth(strText.c_str()) * 1.04f, 20.f / 600.f * MGetWorkspaceWidth());
			pPicture->SetOpacity(80);
			pPicture->Draw(pDC);
		}

		pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Spy_EventMsg");
		if (pPicture)
		{
			if (ZGetCharacterManager()->size() < 4)
				pPicture->Show(false);
			else
			{
				pPicture->Show(true);

				ZCharacter* pCharacter = ZGetCombatInterface()->GetTargetCharacter();
				if (ZGetGame()->GetMatch()->GetRoundState() == MMATCH_ROUNDSTATE_COUNTDOWN)
					pPicture->SetBitmap(MBitmapManager::Get("Spy_EventMsg_ComingSoonOpenSpy.png"));
				else
				{
					if (ZGetGame()->GetMatch()->GetRoundState() == MMATCH_ROUNDSTATE_PLAY)
					{
						if (IsSpy(pCharacter->GetUID()))
							pPicture->SetBitmap(MBitmapManager::Get("Spy_EventMsg_Survive.png"));
						else
							pPicture->SetBitmap(MBitmapManager::Get("Spy_EventMsg_EliminateSpy.png"));
					}
					else
						pPicture->Show(false);
				}
			}
		}

		pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Spy_Notice");
		if (pPicture)
		{
			pPicture->Show(true);
			if (ZGetCharacterManager()->size() < 4)
				pPicture->SetBitmap(MBitmapManager::Get("Spy_Notice_Wait_OtherPlayer.png"));
			else
			{
				if (IsIntroduceTeam())
				{
					ZCharacter* pCharacter = ZGetCombatInterface()->GetTargetCharacter();

					if (IsSpy(pCharacter->GetUID()))
						pPicture->SetBitmap(MBitmapManager::Get("Spy_Notice_Spy.png"));
					else
						pPicture->SetBitmap(MBitmapManager::Get("Spy_Notice_Tracer.png"));
				}
				else
				{
					pPicture->Show(false);
				}
			}
		}
	}
}

void ZRuleSpyMode::SetTeams()
{
	for (auto itor = ZGetCharacterManager()->begin(); itor != ZGetCharacterManager()->end(); ++itor)
	{
		ZCharacter* pCharacter = (ZCharacter*)(*itor).second;
		if (IsSpy(pCharacter->GetUID()))
		{

			for (auto it = SpyItems.begin(); it != SpyItems.end(); ++it)
			{
				if (_stricmp((*it).desc, "BAG") == 0)
					pCharacter->m_Items.EquipItem(MMCIP_MELEE, (*it).id);

				if (_stricmp((*it).desc, "LIGHT") == 0) {
					pCharacter->m_Items.EquipItem(MMCIP_CUSTOM1, (*it).id, (*it).count);
					pCharacter->m_Items.GetItem(MMCIP_CUSTOM1)->SetBulletCurrMagazine((*it).count);
					pCharacter->m_Items.GetItem(MMCIP_CUSTOM1)->SetBulletSpare((*it).count);
				}

				if (_stricmp((*it).desc, "ICE") == 0) {
					pCharacter->m_Items.EquipItem(MMCIP_CUSTOM2, (*it).id, (*it).count);
					pCharacter->m_Items.GetItem(MMCIP_CUSTOM2)->SetBulletCurrMagazine((*it).count);
					pCharacter->m_Items.GetItem(MMCIP_CUSTOM2)->SetBulletSpare((*it).count);
				}
				///TODO: keep this?
				if (_stricmp((*it).desc, "SMOKE") == 0) {
					pCharacter->m_Items.EquipItem(MMCIP_CUSTOM3, (*it).id, (*it).count);
					pCharacter->m_Items.GetItem(MMCIP_CUSTOM3)->SetBulletCurrMagazine((*it).count);
					pCharacter->m_Items.GetItem(MMCIP_CUSTOM3)->SetBulletSpare((*it).count);
				}
				pCharacter->m_Items.EquipItem(MMCIP_PRIMARY, 0);
				pCharacter->m_Items.EquipItem(MMCIP_SECONDARY, 0);
			}
			pCharacter->SetMaxHP(pCharacter->GetHP() + m_HpApBonus);
			pCharacter->SetHP(pCharacter->GetHP() + m_HpApBonus);
			pCharacter->SetMaxAP(pCharacter->GetAP() + m_HpApBonus);
			pCharacter->SetAP(pCharacter->GetAP() + m_HpApBonus);
		}
		else
		{
			for (auto it = SpyItems.begin(); it != SpyItems.end(); ++it)
			{
				if (_stricmp((*it).desc, "STUN") == 0)
				{
					pCharacter->m_Items.EquipItem(MMCIP_CUSTOM1, (*it).id, (*it).count);
					pCharacter->m_Items.GetItem(MMCIP_CUSTOM1)->SetBulletCurrMagazine((*it).count);
					pCharacter->m_Items.GetItem(MMCIP_CUSTOM1)->SetBulletSpare((*it).count);
				}
				else if (_stricmp((*it).desc, "MINE") == 0)
				{
					pCharacter->m_Items.EquipItem(MMCIP_CUSTOM2, (*it).id, (*it).count);
					pCharacter->m_Items.GetItem(MMCIP_CUSTOM2)->SetBulletCurrMagazine((*it).count);
					pCharacter->m_Items.GetItem(MMCIP_CUSTOM2)->SetBulletSpare((*it).count);
				}
			}
		}
		if (pCharacter->GetItems()->GetSelectedWeaponType() != MMCIP_MELEE)
			pCharacter->ChangeWeapon(MMCIP_MELEE);
		//force a mesh change
		if (pCharacter->m_Items.GetItem(MMCIP_MELEE)->GetDesc() != nullptr)
			pCharacter->OnChangeWeapon(pCharacter->m_Items.GetItem(MMCIP_MELEE)->GetDesc());
	}
}

void ZRuleSpyMode::ApplyBonus(ZCharacter* playerObj, unsigned char scoreValue)
{
	playerObj->GetStatus().CheckCrc();
	playerObj->GetStatus().Ref().nScore += static_cast<int>(scoreValue);
	playerObj->GetStatus().MakeCrc();
}