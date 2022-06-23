#include "stdafx.h"
#include "ZRuleDemolition.h"
#include "../Demolition.h"
#include "ZInput.h"
#include "ZCharacter.h"
#include "ZActionDef.h"
#include "ZActionKey.h"
#include "ZEffectManager.h"

bool isCharWithinBounds(ZCharacter* myChar)
{
	rvector pos = myChar->GetPosition();

	DemoMap map = MGetDemolition()->GetMapIndex(ZGetGameClient()->GetMatchStageSetting()->GetMapName());


	if (myChar->GetTeamID() == MMT_RED) {
		float length = D3DXVec3Length(&(pos - rvector(map.bluePos.x, map.bluePos.y, map.bluePos.z)));
		if(length <= 80.f)
			return true;
	}
	if (myChar->GetTeamID() == MMT_BLUE) {
		float length = D3DXVec3Length(&(pos - rvector(map.redPos.x, map.redPos.y, map.redPos.z)));
		if(length <= 80.f)
			return true;
	}
	return false;
}

ZRuleDemolition::ZRuleDemolition(ZMatch* pMatch) : ZRule(pMatch)
{
	
	for (int i = 0; i < 2; ++i) {
		bombCarrier[i].SetInvalid();
		m_bombPlanted[i] = false;
	}

	DemoMap map = MGetDemolition()->GetMapIndex(ZGetGameClient()->GetMatchStageSetting()->GetMapName());
	m_bombMesh[0] = new RVisualMesh();
	m_bombMesh[0]->SetMesh(ZGetMeshMgr()->Get("bomb_red"));
	m_bombMesh[0]->SetPos(rvector(map.redPos.x, map.redPos.y, map.redPos.z), rvector(0, 1, 0), rvector(0, 0, 1));

	m_bombMesh[1] = new RVisualMesh();
	m_bombMesh[1]->SetMesh(ZGetMeshMgr()->Get("bomb_blue"));
	m_bombMesh[1]->SetPos(rvector(map.bluePos.x, map.bluePos.y, map.bluePos.z), rvector(0, 1, 0), rvector(0, 0, 1));
}

ZRuleDemolition::~ZRuleDemolition()
{
	for (int i = 0; i < 2; ++i)
	{
		if (m_bombMesh[i] != nullptr)
			SAFE_DELETE(m_bombMesh[i]);
	}
}

bool ZRuleDemolition::OnCommand(MCommand* command)
{
	if (ZGetGame() == nullptr)
		return false;

	switch (command->GetID())
	{
		case MC_MATCH_RESPONSE_PLANT_BOMB:
		{
			MMatchTeam teamID;
			bool bombPlanted = false;
			command->GetParameter(&teamID, 0, MPT_INT);
			command->GetParameter(&bombPlanted, 1, MPT_BOOL);

			if (teamID == MMT_RED)
			{
				m_bombPlanted[0] = bombPlanted;
				if (!m_bombPlanted[0])
				{
					bombCarrier[0].SetInvalid();
				}
			}
			else if (teamID == MMT_BLUE)
			{
				m_bombPlanted[1] = bombPlanted;
				if (!m_bombPlanted[1])
				{
					bombCarrier[1].SetInvalid();
				}
			}
		}return true;
		case MC_MATCH_BOMB_EXPLODE:
		{
			MMatchTeam winningTeam;
			command->GetParameter(&winningTeam, 0, MPT_INT);
			AddBombExplodeEffect(winningTeam);
		}
	}
	return false;
}

void ZRuleDemolition::OnDraw(MDrawContext* pDC)
{
	for (auto itor = ZGetCharacterManager()->begin(); itor != ZGetCharacterManager()->end(); ++itor)
	{
		ZCharacter* player = (ZCharacter*)(*itor).second;
		if (player != nullptr && isCharWithinBounds(player) && player->isPlantingBomb())
		{
			rvector pos = player->GetPosition();
			RealSpace2::rboundingbox box;

			if (player->m_pVMesh == NULL) continue;


			box.vmax = pos + rvector(50.f, 50.f, 190.f);
			box.vmin = pos + rvector(-50.f, -50.f, 0.f);

			if (isInViewFrustum(&box, RGetViewFrustum()))
			{

				rvector screen_pos = RGetTransformCoord(player->GetVisualMesh()->GetHeadPosition() + rvector(0, 0, 30.f));

				int x = screen_pos.x - pDC->GetFont()->GetWidth(player->GetCharName()) / 2;

				float nValue = 0.004 * player->currDemoProgress;

				pDC->SetColor(MCOLOR(0x40FF0000));
				pDC->FillRectangleW(x - 100, screen_pos.y - 17, nValue * 700, 7);
			}
		}
	}
}

void ZRuleDemolition::OnUpdate(float fElapsed)
{

	if (ZGetGame()->m_pMyCharacter->GetTeamID() == MMT_RED) {
		if (bombCarrier[0].IsValid())
			return;
	}
	else if (ZGetGame()->m_pMyCharacter->GetTeamID() == MMT_BLUE) {
		if (bombCarrier[1].IsValid())
			return;
	}

	if (ZIsActionKeyPressed(ZACTION_PLANTINGBOMB) && isCharWithinBounds(ZGetGame()->m_pMyCharacter) && !ZGetGame()->m_pMyCharacter->IsDie())
	{
		ZGetGame()->m_pMyCharacter->setPlantingBomb(true);
		ZGetGame()->m_pMyCharacter->currDemoProgress += fElapsed * 1000;
		if (ZGetGame()->m_pMyCharacter->currDemoProgress / 1000  >= 5)
		{
			MMatchTeam teamID = ZGetGame()->m_pMyCharacter->GetTeamID();
			if (teamID == MMT_BLUE && m_bombPlanted[1] == false)
			{
				ZPostPlantBomb(ZGetMyUID());
				m_bombPlanted[1] = true;
			}

			else if (teamID == MMT_RED && m_bombPlanted[0] == false)
			{
				ZPostPlantBomb(ZGetMyUID());
				m_bombPlanted[0] = true;
			}
		}
	}
	else
	{
		ZGetGame()->m_pMyCharacter->currDemoProgress = 0;
	}
}

void ZRuleDemolition::OnSetRoundState(MMATCH_ROUNDSTATE roundState)
{
	switch (roundState)
	{
		case MMATCH_ROUNDSTATE_PREPARE:
		{
			for (int i = 0; i < 2; ++i) {
				bombCarrier[i].SetInvalid();
				m_bombPlanted[i] = false;
			}

			for (auto itor = ZGetCharacterManager()->begin(); itor != ZGetCharacterManager()->end(); ++itor)
			{
				ZCharacter* player = (ZCharacter*)(*itor).second;
				player->setBombPlanted(false);
				player->setPlantingBomb(false);
			}
		}break;
	}
}

void ZRuleDemolition::AfterCommandProcessed(MCommand* command)
{
	if (!ZGetGame())
		return;

	switch (command->GetID())
	{
		case MC_MATCH_GAME_DEAD:
		{
			MUID victim;
			command->GetParameter(&victim, 2, MPT_UID);

			ZCharacter* player = (ZCharacter*)ZGetCharacterManager()->Find(victim);
			player->currDemoProgress = 0.0f;
			if (player->GetTeamID() == MMT_RED && victim == bombCarrier[0]) {
				bombCarrier[0].SetInvalid();
				m_bombPlanted[0] = false;
			}
			else if (player->GetTeamID() == MMT_BLUE && victim == bombCarrier[1]) {
				bombCarrier[1].SetInvalid();
				m_bombPlanted[1] = false;
			}
			player->setPlantingBomb(false);
			player->setBombPlanted(false);
		}break;
	}
}

void ZRuleDemolition::AddBombExplodeEffect(MMatchTeam teamID)
{
	DemoMap map = MGetDemolition()->GetMapIndex(ZGetGameClient()->GetMatchStageSetting()->GetMapIndex());

	if (teamID == MMT_RED)
	{
		rvector dir(map.bluePos.x, map.bluePos.y, map.bluePos.z);
		Normalize(dir);

		ZGetEffectManager()->AddWithScale("ef_exgrenade", rvector(map.bluePos.x, map.bluePos.y + 10, map.bluePos.z), dir, MUID(0, 0), 100, 1);
		ZGetSoundEngine()->PlaySound("we_grenade_explosion");
	}
	else if (teamID == MMT_BLUE)
	{
		rvector dir(map.redPos.x, map.redPos.y, map.redPos.z);
		Normalize(dir);

		ZGetEffectManager()->AddWithScale("ef_exgrenade", rvector(map.redPos.x, map.redPos.y + 10, map.redPos.z), dir, MUID(0, 0), 100, 1);
		ZGetSoundEngine()->PlaySound("we_grenade_explosion");
	}
}