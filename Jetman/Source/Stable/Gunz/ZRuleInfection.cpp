#include "stdafx.h"
#include "ZRuleInfection.h"
#include "ZScreenEffectManager.h"

ZRuleInfection::ZRuleInfection(ZMatch* pMatch) : ZRule(pMatch)
{
	ZombieOverlay = new MBitmapR2();
	ZombieOverlay->Create("Zombieview.png", RGetDevice(), "interface/default/combat/Zombieview.png");
	uidFirstZombie.SetInvalid();
}

ZRuleInfection::~ZRuleInfection()
{
	SAFE_DELETE(ZombieOverlay);
}

bool ZRuleInfection::OnCommand(MCommand* pCmd)
{
	if (!ZGetGame())
	{
		_ASSERT(0);
		return false;
	}

	switch (pCmd->GetID())
	{
		case MC_MATCH_INFECT_PLAYER:
		{
			MUID uidPlayer;
			int hp, ap;
			bool firstInfect;
			pCmd->GetParameter(&uidPlayer, 0, MPT_UID);
			pCmd->GetParameter(&hp, 1, MPT_INT);
			pCmd->GetParameter(&ap, 2, MPT_INT);
			pCmd->GetParameter(&firstInfect, 3, MPT_BOOL);
			InfectPlayer(uidPlayer, hp, ap,firstInfect);
			return true;
		}break;

		case MC_MATCH_INFECTION_LATEJOIN:
		{
			MUID targetPlayer, infectedPlayer;
			bool firstInfect;
			pCmd->GetParameter(&targetPlayer, 0, MPT_UID);
			pCmd->GetParameter(&infectedPlayer, 1, MPT_UID);
			pCmd->GetParameter(&firstInfect, 2, MPT_BOOL);

			if (ZGetMyUID() == targetPlayer) {

				if (firstInfect)
					uidFirstZombie = infectedPlayer;
				InfectPlayer(infectedPlayer, 0, 0, firstInfect);
			}
			return true;
		}
	}
	return false;
}

void ZRuleInfection::AfterCommandProcessed(MCommand* pCmd)
{
	switch (pCmd->GetID())
	{
		case MC_MATCH_GAME_DEAD:
		{
			MUID uidPlayer;
			pCmd->GetParameter(&uidPlayer, 2, MPT_UID);
			DrawRemainingPlayers(uidPlayer);
		}break;
	}
}

void ZRuleInfection::InfectPlayer(const MUID& uidPlayer, int hp, int ap, bool firstInfect)
{
	ZCharacter* pChar = (ZCharacter*)ZGetCharacterManager()->Find(uidPlayer);
	if (!pChar)
		return;

	if (!ZGetGame()->IsReplay()) {
		if (firstInfect)
		{
			uidFirstZombie = pChar->GetUID();
		}
	}
	//Change the itemid's to w/e you need or want them to be.
	
	pChar->m_Items.EquipItem(MMCIP_AVATAR, pChar->GetProperty()->nSex == MMS_MALE ? 530002 : 530502);
	pChar->m_Items.EquipItem(MMCIP_MELEE, 1);

	if (pChar->m_Items.GetSelectedWeaponType() != MMCIP_MELEE)
		pChar->ChangeWeapon(MMCIP_MELEE);

	for (int i = MMCIP_PRIMARY; i <= MMCIP_CUSTOM2; ++i)
	{
		pChar->m_Items.EquipItem((MMatchCharItemParts)i, 0);
	}

	unsigned long nItemID[MMCIP_END];
	for (int i = 0; i < MMCIP_END; ++i)
	{
		nItemID[i] = pChar->m_Items.GetItem((MMatchCharItemParts)i)->GetDescID();
	}

	ZChangeCharParts(pChar->GetVisualMesh(), pChar->GetProperty()->nSex, pChar->GetProperty()->nHair, pChar->GetProperty()->nFace, nItemID);

	pChar->Revival();
	ZGetEffectManager()->AddReBirthEffect(pChar->GetPosition());
	pChar->OnChangeWeapon(pChar->m_Items.GetItem(MMCIP_MELEE)->GetDesc());
	pChar->SetTeamID(MMT_RED);
	pChar->SetMaxHP((float)hp);
	pChar->SetHP((float)hp);
	pChar->SetMaxAP((float)ap);
	pChar->SetAP((float)ap);
	pChar->SetInfected(true);

}

void ZRuleInfection::OnSetRoundState(MMATCH_ROUNDSTATE roundstate)
{
	//this is here to reset the players items and set team to blue on the client side.
	switch (roundstate)
	{
		case MMATCH_ROUNDSTATE_PREPARE:
		{
			for (auto itor = ZGetCharacterManager()->begin(); itor != ZGetCharacterManager()->end(); ++itor)
			{
				ZCharacter* pCharacter = (ZCharacter*)(*itor).second;
				pCharacter->SetTeamID(MMT_BLUE);
				pCharacter->SetInfected(false);
				if (pCharacter->m_Items.GetSelectedWeaponType() != MMCIP_MELEE)
					pCharacter->ChangeWeapon(MMCIP_MELEE);


				unsigned long nItemID[MMCIP_END];
				for (int i = 0; i < MMCIP_END; ++i)
				{
					nItemID[i] = pCharacter->GetCharInfo()->nEquipedItemDesc[i];
					pCharacter->m_Items.EquipItem((MMatchCharItemParts)i, nItemID[i]);
				}
				ZChangeCharParts(pCharacter->GetVisualMesh(), pCharacter->GetProperty()->nSex, pCharacter->GetProperty()->nHair,
					pCharacter->GetProperty()->nFace, nItemID);
				//Forces a mesh reset on the melee item
				pCharacter->OnChangeWeapon(pCharacter->m_Items.GetItem(MMCIP_MELEE)->GetDesc());

			}
		}break;
		case MMATCH_ROUNDSTATE_COUNTDOWN:
		{
			ZGetScreenEffectManager()->AddScreenEffect("infectedArrival");
		}break;
	}
}

void ZRuleInfection::OnDraw(MDrawContext* pDC)
{
	if (ZGetCombatInterface()->GetObserverMode())
	{
		ZCharacter* pCharacter = ZGetCombatInterface()->GetObserver()->GetTargetCharacter();
		if (pCharacter->IsInfected())
		{
			MDrawEffect effect = pDC->GetEffect();
			unsigned char opacity = pDC->GetOpacity();
			pDC->SetEffect(MDE_ADD);
			pDC->SetBitmap(ZombieOverlay);
			pDC->Draw(0, 0, RGetScreenWidth(), RGetScreenHeight());
			//DrawInfectionPanel(pDC);
			pDC->SetBitmap(NULL);
			pDC->SetEffect(effect);
			pDC->SetOpacity(opacity);
		}
	}
	else
	{
		if (ZGetGame()->m_pMyCharacter->IsInfected())
		{
			MDrawEffect effect = pDC->GetEffect();
			unsigned char opacity = pDC->GetOpacity();
			pDC->SetEffect(MDE_ADD);
			pDC->SetBitmap(ZombieOverlay);
			pDC->SetOpacity(255);
			pDC->Draw(0, 0, RGetScreenWidth(), RGetScreenHeight());
			//DrawInfectionPanel(pDC);
			pDC->SetBitmap(NULL);
			pDC->SetEffect(effect);
			pDC->SetOpacity(opacity);
		}
	}
}

void ZRuleInfection::DrawInfectionPanel(MDrawContext* pDC)
{
	ZGetScreenEffectManager()->DrawInfectionPanel(pDC);
}

void ZRuleInfection::DrawRemainingPlayers(const MUID& uidPlayer)
{
	int redCount = 0, blueCount = 0, redTeamCount = 0;

	for (auto itor = ZGetCharacterManager()->begin(); itor != ZGetCharacterManager()->end(); ++itor)
	{
		if ((*itor).second->GetTeamID() == MMT_RED)
			redTeamCount++;
	}
	ZGetGame()->GetMatch()->GetTeamAliveCount(&redCount, &blueCount);
	ZCharacter* pChar = (ZCharacter*)ZGetCharacterManager()->Find(uidPlayer);
	if (!pChar)
		return;

	if (pChar == ZGetGame()->m_pMyCharacter)
		return;

	if (pChar->GetTeamID() == MMT_RED)
	{
		if (redTeamCount > 1)
		{
			if (redCount == 1 && pChar->GetTeamID() == ZGetGame()->m_pMyCharacter->GetTeamID())
			{
				ZGetScreenEffectManager()->AddScreenEffect("lastInfected");

			}
		}
	}
	else if (pChar->GetTeamID() == MMT_BLUE)
	{
		if (blueCount == 1 && pChar->GetTeamID() == ZGetGame()->m_pMyCharacter->GetTeamID())
		{
			ZGetScreenEffectManager()->AddScreenEffect("lastSurvivor");
		}
	}
}