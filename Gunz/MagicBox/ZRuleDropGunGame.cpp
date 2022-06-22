#include "stdafx.h"
#include "ZRuleDropGunGame.h"
#include "../ZWorldItem.h"

/////////////////////////////////////////////////////////////////////////////////////////
//////// GunGame
ZRuleDropGunGame::ZRuleDropGunGame(ZMatch* pMatch) : ZRule(pMatch)
{

}
ZRuleDropGunGame::~ZRuleDropGunGame()
{

}

////////////////////////////////////////////////
ZDropGunGameWorldItem* ZRuleDropGunGame::GetDropGunGameWorldItemByID(int nID)
{
	iterator itor = find(nID);
	if (itor != end())
	{
		return (*itor).second;
	}
	return NULL;
}

////////////////////////////////////////////////
bool ZRuleDropGunGame::AddDropGunGameWorldItem(ZDropGunGameWorldItem* pGunGameWorldItem)
{
	if (GetDropGunGameWorldItemByID(pGunGameWorldItem->GetID()))
		return false;

	insert(value_type(pGunGameWorldItem->GetID(), pGunGameWorldItem));
	return true;
}

////////////////////////////////////////////////
void ZRuleDropGunGame::AddWorldItem(int nID, float possx, float possy, float possz)
{
	try
	{
		ZDropGunGameWorldItem* pGunGameWorldItem = new ZDropGunGameWorldItem;

		pGunGameWorldItem->SetID(nID);
		pGunGameWorldItem->SetAssX(possx);
		pGunGameWorldItem->SetAssY(possy);
		pGunGameWorldItem->SetAssZ(possz);

		if (!AddDropGunGameWorldItem(pGunGameWorldItem))
		{
			mlog("ERROR: Drop GunGame Item Duplication.");
			delete pGunGameWorldItem;
		}
	}
	catch (...)
	{
		mlog("ERROR: Drop GunGame Add Item.");
	}
}

bool ZRuleDropGunGame::OnCommand(MCommand* pCommand)
{
	if (!ZGetGame()) return false;

	switch (pCommand->GetID())
	{
		case MC_DROPGUNGAME_RESPONSE_ENTERGAME:
			//P(MPT_UID, "")
			//P(MPT_INT, "")
			//P(MPT_INT, "")
			//P(MPT_INT, "")
			//P(MPT_INT, "")
			//P(MPT_INT, "");
			{
				MUID uidChar;
				int nItemID1;
				int nItemID2;
				int nItemID3;
				int nItemID4;
				int nItemID5;
				bool bTrigger;

				pCommand->GetParameter(&uidChar,  0, MPT_UID);
				pCommand->GetParameter(&nItemID1, 1, MPT_INT);
				pCommand->GetParameter(&nItemID2, 2, MPT_INT);
				pCommand->GetParameter(&nItemID3, 3, MPT_INT);
				pCommand->GetParameter(&nItemID4, 4, MPT_INT);
				pCommand->GetParameter(&nItemID5, 5, MPT_INT);
				pCommand->GetParameter(&bTrigger, 6, MPT_BOOL);

				// Character
				ZCharacter* pChar = (ZCharacter*) ZGetGame()->m_CharacterManager.Find(uidChar);
				if (pChar == NULL)
					break;

				if (pChar->GetUID() == ZGetGame()->m_pMyCharacter->GetUID())
				{
					ZGetGame()->m_pMyCharacter->ChangeWeapon(MMCIP_PRIMARY);
					ZGetGame()->m_pMyCharacter->m_pVMesh->RemoveAllWeapon();
					ZGetGame()->m_pMyCharacter->GetItems()->EquipItem((MMatchCharItemParts) MMCIP_MELEE, nItemID1);
					ZGetGame()->m_pMyCharacter->GetItems()->EquipItem((MMatchCharItemParts) MMCIP_PRIMARY, nItemID2);
					ZGetGame()->m_pMyCharacter->GetItems()->EquipItem((MMatchCharItemParts) MMCIP_SECONDARY, nItemID3);
					ZGetGame()->m_pMyCharacter->GetItems()->EquipItem((MMatchCharItemParts) MMCIP_CUSTOM1, nItemID4);
					ZGetGame()->m_pMyCharacter->GetItems()->EquipItem((MMatchCharItemParts) MMCIP_CUSTOM2, nItemID5);
					ZGetGame()->m_pMyCharacter->ChangeWeapon(MMCIP_MELEE);

					if (nItemID1 == 0 && nItemID2 == 0 && nItemID3 == 0 && nItemID4 == 0 && nItemID5 == 0)
						ZGetGame()->m_pMyCharacter->m_pVMesh->SelectWeaponMotion(eq_wd_item);

					if (bTrigger)
					{
						// Install Screen Effect
						m_nTime[1] = timeGetTime();
						m_show[1] = true;
						m_szText[1] = "Look for magic boxes found on the entire map and collect weapons, be a survivor!";
					}
				}
				else
				{
					pChar->ChangeWeapon(MMCIP_PRIMARY);
					pChar->m_pVMesh->RemoveAllWeapon();
					pChar->GetItems()->EquipItem((MMatchCharItemParts) MMCIP_MELEE, nItemID1);
					pChar->GetItems()->EquipItem((MMatchCharItemParts) MMCIP_PRIMARY, nItemID2);
					pChar->GetItems()->EquipItem((MMatchCharItemParts) MMCIP_SECONDARY, nItemID3);
					pChar->GetItems()->EquipItem((MMatchCharItemParts) MMCIP_CUSTOM1, nItemID4);
					pChar->GetItems()->EquipItem((MMatchCharItemParts) MMCIP_CUSTOM2, nItemID5);
					pChar->ChangeWeapon(MMCIP_MELEE);

					if (nItemID1 == 0 && nItemID2 == 0 && nItemID3 == 0 && nItemID4 == 0 && nItemID5 == 0)
						pChar->m_pVMesh->SelectWeaponMotion(eq_wd_item);
				}
			}
			break;

		case MC_DROPGUNGAME_RESPONSE_ITEM:
			//P(MPT_UID, "")
			//P(MPT_INT, "")
			//P(MPT_INT, "")
			//P(MPT_INT, "")
			//P(MPT_INT, "")
			//P(MPT_INT, "");
			{
				MUID uidChar;
				int nMeleeID;
				int nPrimaryID;
				int nSecondaryID;
				int nCustom1ID;
				int nCustom2ID;

				pCommand->GetParameter(&uidChar, 0, MPT_UID);
				pCommand->GetParameter(&nMeleeID, 1, MPT_INT);
				pCommand->GetParameter(&nPrimaryID, 2, MPT_INT);
				pCommand->GetParameter(&nSecondaryID, 3, MPT_INT);
				pCommand->GetParameter(&nCustom1ID, 4, MPT_INT);
				pCommand->GetParameter(&nCustom2ID, 5, MPT_INT);

				ZCharacter* pChar = (ZCharacter*) ZGetGame()->m_CharacterManager.Find(uidChar);
				if (pChar == NULL) break;

				if (nMeleeID != 0)
				{
					pChar->GetItems()->EquipItem((MMatchCharItemParts) MMCIP_MELEE, nMeleeID);
				}
				else
				{
					pChar->GetItems()->EquipItem((MMatchCharItemParts) MMCIP_MELEE, 0);
				}

				if (nPrimaryID != 0)
				{
					pChar->GetItems()->EquipItem((MMatchCharItemParts) MMCIP_PRIMARY, nPrimaryID);
				}
				else
				{
					pChar->GetItems()->EquipItem((MMatchCharItemParts) MMCIP_PRIMARY, 0);
				}

				if (nSecondaryID != 0)
				{
					pChar->GetItems()->EquipItem((MMatchCharItemParts) MMCIP_SECONDARY, nSecondaryID);
				}
				else
				{
					pChar->GetItems()->EquipItem((MMatchCharItemParts) MMCIP_SECONDARY, 0);
				}

				if (nCustom1ID != 0)
				{
					pChar->GetItems()->EquipItem((MMatchCharItemParts) MMCIP_CUSTOM1, nCustom1ID);
				}
				else
				{
					pChar->GetItems()->EquipItem((MMatchCharItemParts) MMCIP_CUSTOM1, 0);
				}

				if (nCustom2ID != 0)
				{
					pChar->GetItems()->EquipItem((MMatchCharItemParts) MMCIP_CUSTOM2, nCustom2ID);
				}
				else
				{
					pChar->GetItems()->EquipItem((MMatchCharItemParts) MMCIP_CUSTOM2, 0);
				}

				pChar->ChangeWeapon(MMCIP_PRIMARY);
				pChar->ChangeWeapon(MMCIP_MELEE);
				pChar->InitItemBullet();
			}
			break;

		case MC_DROPGUNGAME_RESPONSE_WORLDITEMS:
			//P(MPT_UID, "")
			//P(MPT_INT, "")
			//P(MPT_FLOAT, "")
			//P(MPT_FLOAT, "")
			//P(MPT_FLOAT, "");
			{
				MUID uidChar;
				int itemID;
				float fpossx;
				float fpossy;
				float fpossz;

				pCommand->GetParameter(&uidChar, 0, MPT_UID);
				pCommand->GetParameter(&itemID, 1, MPT_INT);
				pCommand->GetParameter(&fpossx, 2, MPT_FLOAT);
				pCommand->GetParameter(&fpossy, 3, MPT_FLOAT);
				pCommand->GetParameter(&fpossz, 4, MPT_FLOAT);

				if (uidChar == ZGetGame()->m_pMyCharacter->GetUID())
				{
					ZGetWorldItemManager()->AddWorldItem(ZGetWorldItemManager()->GenStandAlondID(), itemID, MTD_Static, rvector(fpossx, fpossy, fpossz));
					AddWorldItem(m_nID++, fpossx, fpossy, fpossz);
				}
			}
			break;

		case MC_MATCH_GAME_DEAD:
			{
				MUID uidAttacker, uidVictim;
				unsigned long int nAttackerArg, nVictimArg;

				pCommand->GetParameter(&uidAttacker, 0, MPT_UID);
				pCommand->GetParameter(&nAttackerArg, 1, MPT_UINT);
				pCommand->GetParameter(&uidVictim, 2, MPT_UID);
				pCommand->GetParameter(&nVictimArg, 3, MPT_UINT);

				ZCharacter* pChar = (ZCharacter*) ZGetGame()->m_CharacterManager.Find(uidVictim);
				if (pChar == NULL)
					break;

				if(uidVictim == ZGetMyUID())
				{
					// Install Screen Effect
					m_nTime[1] = timeGetTime();
					m_show[1] = true;
					m_szText[1] = "Oh no! You have lost all your weapons, collect more weapons and take revenge!";

					// Install Items
					ZGetGame()->m_pMyCharacter->m_pVMesh->RemoveAllWeapon();
					ZGetGame()->m_pMyCharacter->GetItems()->EquipItem((MMatchCharItemParts) MMCIP_MELEE, 0);
					ZGetGame()->m_pMyCharacter->GetItems()->EquipItem((MMatchCharItemParts) MMCIP_PRIMARY, 0);
					ZGetGame()->m_pMyCharacter->GetItems()->EquipItem((MMatchCharItemParts) MMCIP_SECONDARY, 0);
					ZGetGame()->m_pMyCharacter->GetItems()->EquipItem((MMatchCharItemParts) MMCIP_CUSTOM1, 0);
					ZGetGame()->m_pMyCharacter->GetItems()->EquipItem((MMatchCharItemParts) MMCIP_CUSTOM2, 0);
					ZGetGame()->m_pMyCharacter->m_pVMesh->SelectWeaponMotion(eq_wd_item);
				}
				else
				{
					pChar->m_pVMesh->RemoveAllWeapon();
					pChar->GetItems()->EquipItem((MMatchCharItemParts) MMCIP_MELEE, 0);
					pChar->GetItems()->EquipItem((MMatchCharItemParts) MMCIP_PRIMARY, 0);
					pChar->GetItems()->EquipItem((MMatchCharItemParts) MMCIP_SECONDARY, 0);
					pChar->GetItems()->EquipItem((MMatchCharItemParts) MMCIP_CUSTOM1, 0);
					pChar->GetItems()->EquipItem((MMatchCharItemParts) MMCIP_CUSTOM2, 0);
					pChar->m_pVMesh->SelectWeaponMotion(eq_wd_item);
				}
			}
			break;
	}

	return false;
}


void ZRuleDropGunGame::OnDrawDropGunGame(MDrawContext* pDC)
{
	if ((timeGetTime() - m_nTime[1]) < 5000 && m_show[1] == true)
	{
		MCOLOR backgroundcolor = MCOLOR(0, 0, 0, 150);

		pDC->SetColor(backgroundcolor);
		pDC->FillRectangle(MGetWorkspaceWidth() / 2 - 350, MGetWorkspaceHeight() * (200.0f / 800.0f) - 7, 730, 30);

		backgroundcolor = MCOLOR(255, 255, 255, 255);
		pDC->SetColor(backgroundcolor);

		TextRelative(pDC, 0.5f, 200.0f / 800.0f, m_szText[1].c_str(), true);
	}

	for(map<int, ZDropGunGameWorldItem*>::iterator itor = map<int, ZDropGunGameWorldItem*>::begin(); itor != map<int, ZDropGunGameWorldItem*>::end(); ++itor)
	{
		ZDropGunGameWorldItem* pWorldItem = (ZDropGunGameWorldItem*) (*itor).second;
		if (pWorldItem == NULL)
			break;

		rvector diff = ZGetGame()->m_pMyCharacter->GetPosition() + ZGetGame()->m_pMyCharacter->GetDirection()*10.f - rvector(pWorldItem->GetAssX(), pWorldItem->GetAssY(), pWorldItem->GetAssZ());
		diff.z *= .5f;
		float fDist = Magnitude(diff);

		if (fDist < 200.f)
		{
			m_show[1] = false;

			rvector pos = RGetTransformCoord(rvector(pWorldItem->GetAssX(), pWorldItem->GetAssY()+20, pWorldItem->GetAssZ()));

			MCOLOR backgroundcolor = MCOLOR(0, 0, 0, 150);

			pDC->SetColor(backgroundcolor);
			pDC->FillRectangle(MGetWorkspaceWidth() / 2 - 350, MGetWorkspaceHeight() * (200.0f / 800.0f) - 7, 730, 30);

			backgroundcolor = MCOLOR(255, 255, 255, 255);
			pDC->SetColor(backgroundcolor);

			TextRelative(pDC, 0.5f, 200.0f / 800.0f, "Press the 'E' button to open the magic box and hand yourself a quick random weapons kit before they kill you!", true);
		}
	}
}