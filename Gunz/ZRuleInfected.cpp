#include "stdafx.h"
#include "ZRuleInfected.h"

/////////////////////////////////////////////////////////////////////////////////////////
// Infected
ZRuleInfected::ZRuleInfected(ZMatch* pMatch) : ZRule(pMatch)
{
	CurrentText = None;
}

ZRuleInfected::~ZRuleInfected()
{
}

bool ZRuleInfected::OnCommand(MCommand* pCommand)
{
	if (!ZGetGame()) return false;

	switch (pCommand->GetID())
	{
		case MC_MATCH_RESPONSE_INFECTEDMESSAGE:
		{
			MTD_InfectedMessage Msg;
			pCommand->GetParameter(&Msg, 0, MPT_BLOB);
			Attacker = Msg.Attacker;
			Victim = Msg.Victim;
			SetMessage(Msg.MessageType);
		}
		break;
		case MC_MATCH_RESPONSE_LIST:
			{
				MTD_InfectedQueueInfo QInfo;
				pCommand->GetParameter(&QInfo, 0, MPT_BLOB);
				for (int i=0; i < QInfo.QueueSize; i++)
				{
					ZCharacter* cha = ZGetCharacterManager()->Find(QInfo.m_WaitQueue[i]);
					if (cha != NULL)
					{
						if(QInfo.IsRoundInfo)
						{
							cha->SetTeamID(MMT_RED);
							ZGetMyInfo()->GetGameInfo()->bForcedChangeTeam = true;					
							cha->IsZombie = false;
						}	
						for (int c = 0; c < MMCIP_END; c++)
							cha->GetItems()->EquipItem((MMatchCharItemParts)c, QInfo.nEquipedItemId[i][c], QInfo.nEquipedItemCount[i][c]);
						cha->InitMeshParts();
					}
				}
			}
			break;
			case MC_MATCH_RESPONSE_FIRST_INFECTED:
			{
				MUID uid;
				int Item1 = 0, Item2 = 0;
				pCommand->GetParameter(&uid, 0, MPT_UID);
				pCommand->GetParameter(&Item1, 1, MPT_INT);
				pCommand->GetParameter(&Item2, 2, MPT_INT);
				ZCharacter* cha = ZGetCharacterManager()->Find(uid);
				if(!cha) return true;
				cha->GetItems()->EquipItem(MMCIP_MELEE, Item2);
				cha->GetItems()->EquipItem(MMCIP_PRIMARY, 0, 0);
				cha->GetItems()->EquipItem(MMCIP_SECONDARY, 0, 0);
				cha->GetItems()->EquipItem(MMCIP_CUSTOM1, 0, 0);
				cha->GetItems()->EquipItem(MMCIP_CUSTOM2, 0, 0);
				cha->ChangeWeapon(MMCIP_MELEE);
				cha->IsZombie = true;
				cha->SetInfectedSpeed(400.f);
				ChangeClothes(cha, Item1);
				if(uid == ZGetMyUID())
				{
					ZMapSpawnData* pSpawnData = ZGetGame()->GetMapDesc()->GetSpawnManager()->GetTeamData(1, 0);
					cha->SetLastAttacker(MUID(0, 0));
					ZPostRequestSpawn(ZGetMyUID(), pSpawnData->m_Pos, pSpawnData->m_Dir);
					ZGetGame()->SetSpawnRequested(true);
				}
				cha->SetTeamID(MMT_BLUE);
			}
			break;
	}
	return false;
}

void ZRuleInfected::ChangeClothes(ZCharacter* cha, int ItemID)
{
	 RVisualMesh* pVMesh = cha->GetVisualMesh();
	 MMatchItemDesc* pDesc = MGetMatchItemDescMgr()->GetItemDesc(ItemID);
	 pVMesh->DestroyCloth();
	 pVMesh->ClearParts();
	 pVMesh->SetParts(eq_parts_head, pDesc->m_pAvatarMeshName->Ref().m_szHeadMeshName);
	 pVMesh->SetParts(eq_parts_chest, pDesc->m_pAvatarMeshName->Ref().m_szChestMeshName);
	 pVMesh->SetParts(eq_parts_hands, pDesc->m_pAvatarMeshName->Ref().m_szHandMeshName);
	 pVMesh->SetParts(eq_parts_legs, pDesc->m_pAvatarMeshName->Ref().m_szLegsMeshName);
	 pVMesh->SetParts(eq_parts_feet, pDesc->m_pAvatarMeshName->Ref().m_szFeetMeshName);
}

void ZRuleInfected::OnDrawInfected(MDrawContext* pDC)
{
	/*///////////////////////////////////////
	//RSetFog(true, 0, 5000, 0xFF12100D);

	///////////////////////////////////////
	if ((ZGetGame()->m_pMyCharacter->GetTeamID() == MMT_RED))
	{
		pDC->SetBitmap(MBitmapManager::Get("screen_infected.png"));
		pDC->Draw(0, 0, RGetScreenWidth(), RGetScreenHeight());
	}*/
	if(CurrentText != None && timeGetTime() < Timer)
	{
		MCOLOR backgroundcolor = MCOLOR(0, 0, 0, 150);
		pDC->SetFont(MFontManager::Get("Default"));
		pDC->SetColor(backgroundcolor);
		pDC->FillRectangle(MGetWorkspaceWidth() / 2 - 170, MGetWorkspaceHeight() * (200.0f / 800.0f) - 3, 340, 20);
		backgroundcolor = MCOLOR(255, 255, 255, 255);
		pDC->SetColor(backgroundcolor);
		char szText[255] = "";
		ZCharacter* pAttacker = ZGetCharacterManager()->Find(Attacker);
		ZCharacter* pVictim = ZGetCharacterManager()->Find(Victim);
		switch (CurrentText)
		{
		case FirstInfected:
			if(!pVictim) return;
			if (Victim == ZGetMyUID())
				sprintf(szText, "You have been Infected!");
			else
				sprintf(szText, "Run!!! Zombie '%s' has been spawned!", pVictim->GetUserNameA());
			break;
		case BeenInfected:
			if (!pAttacker || !pVictim) return;
			if (Victim == ZGetMyUID())
				sprintf(szText, "You have been infected by %s!", pAttacker->GetUserNameA());
			else
				sprintf(szText, "Human '%s' has been infected by %s!", pVictim->GetUserNameA(), pAttacker->GetUserNameA());
				break;
		case KilledInfected:			
			if (!pAttacker || !pVictim) return;
			if (Victim == ZGetMyUID())
				sprintf(szText, "You have been slain by %s!", pAttacker->GetUserNameA());
			else
				sprintf(szText, "Zombie '%s' has been slain by %s!", pVictim->GetUserNameA(), pAttacker->GetUserNameA());	
			break;
		case ZombiesWin:
			sprintf(szText, "The Infected have Won!");
			break;
		case HumansWin:
			sprintf(szText, "The Humans have Won!");
			break;
		}
		TextRelative(pDC, 0.5f, 200.0f / 800.0f, szText, true);
	} else CurrentText = None;
}