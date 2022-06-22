#include "stdafx.h"
#include "ZShopEquipItem.h"
#include "ZItemIconBitmap.h"
#include "ZCharacterView.h"
#include "ZShop.h"
#include "ZShopEquipInterface.h"
#include "MMatchItemFunction.h"

string GetRestrictionString(int nResSex, int nResLevel, int nWeight)
{
	string str;
	char temp[1024];

	if (nResSex != -1)
	{
		if (nResSex == 0)
			sprintf(temp, "%s\n", ZMsg(MSG_WORD_FORMEN));
		else if (nResSex == 1)
			sprintf(temp, "%s\n", ZMsg(MSG_WORD_FORWOMEN));
		str += temp;
	}

	if (nResLevel != 0)
	{
		if (nResLevel > ZGetMyInfo()->GetLevel())
			sprintf(temp, "%s : ^1%d ^0%s\n", ZMsg(MSG_WORD_LIMITEDLEVEL), nResLevel, ZMsg(MSG_CHARINFO_LEVELMARKER));
		else
			sprintf(temp, "%s : %d %s\n", ZMsg(MSG_WORD_LIMITEDLEVEL), nResLevel, ZMsg(MSG_CHARINFO_LEVELMARKER));
		str += temp;
	}

	if (nWeight != 0)
	{
		sprintf(temp, "%s : %d Wt.", ZMsg(MSG_WORD_WEIGHT), nWeight);
		str += temp;
	}

	return str;
}

string GetRestrictionString(MMatchItemDesc* pItemDesc)
{
	return GetRestrictionString(pItemDesc->m_nResSex.Ref(), pItemDesc->m_nResLevel.Ref(), pItemDesc->m_nWeight.Ref());
}

string GetItemSpecString(MMatchItemDesc* pItemDesc)
{
	string str;
	char temp[1024];

	// ÀÎÃ¾Æ® ¾ÆÀÌÅÛÀÏ °æ¿ì...
	if (pItemDesc->IsEnchantItem())
	{
		// Áö¼Ó½Ã°£ ¼Ò¼öÁ¡ Ç¥Çö
		char DelayTemp[1024];
		if (pItemDesc->m_nDelay.Ref() % 1000 == 0)
			sprintf(DelayTemp, "%s : %d%s\n", ZMsg(MSG_WORD_RUNTIME), pItemDesc->m_nDelay.Ref() / 1000, ZMsg(MSG_CHARINFO_SECOND));
		else if (pItemDesc->m_nDelay.Ref() % 100 == 0)
			sprintf(DelayTemp, "%s : %.1f%s\n", ZMsg(MSG_WORD_RUNTIME), (float)pItemDesc->m_nDelay.Ref() / 1000, ZMsg(MSG_CHARINFO_SECOND));
		else if (pItemDesc->m_nDelay.Ref() % 10 == 0)
			sprintf(DelayTemp, "%s : %.2f%s\n", ZMsg(MSG_WORD_RUNTIME), (float)pItemDesc->m_nDelay.Ref() / 1000, ZMsg(MSG_CHARINFO_SECOND));
		else
			sprintf(DelayTemp, "%s : %.3f%s\n", ZMsg(MSG_WORD_RUNTIME), (float)pItemDesc->m_nDelay.Ref() / 1000, ZMsg(MSG_CHARINFO_SECOND));

		switch (pItemDesc->m_nWeaponType.Ref())
		{
		case MWT_ENCHANT_FIRE:
			sprintf(temp, "<%s>\n", ZMsg(MSG_WORD_ATTRIBUTE_FIRE));
			str += temp;
			str += DelayTemp;
			if (pItemDesc->m_nDamage.Ref())
			{
				sprintf(temp, "%s : %d dmg/%s\n", ZMsg(MSG_WORD_DAMAGE), pItemDesc->m_nDamage.Ref(), ZMsg(MSG_CHARINFO_SECOND));
				str += temp;
			}
			break;

		case MWT_ENCHANT_COLD:
			sprintf(temp, "<%s>\n", ZMsg(MSG_WORD_ATTRIBUTE_COLD));
			str += temp;
			str += DelayTemp;
			if (pItemDesc->m_nLimitSpeed.Ref())
			{
				sprintf(temp, "%s -%d%%\n", ZMsg(MSG_WORD_RUNSPEED), 100 - pItemDesc->m_nLimitSpeed.Ref());
				str += temp;
			}
			sprintf(temp, "%s\n", ZMsg(MSG_WORD_DONOTDASH));
			str += temp;
			sprintf(temp, "%s\n", ZMsg(MSG_WORD_DONOTHANGWALL));
			str += temp;
			break;

		case MWT_ENCHANT_LIGHTNING:
			sprintf(temp, "<%s>\n", ZMsg(MSG_WORD_ATTRIBUTE_LIGHTNING));
			str += temp;
			str += DelayTemp;
			if (pItemDesc->m_nDamage.Ref())
			{
				sprintf(temp, "%s : %d dmg.\n", ZMsg(MSG_WORD_ATTACK), pItemDesc->m_nDamage.Ref());
				str += temp;
			}
			break;

		case MWT_ENCHANT_POISON:
			sprintf(temp, "<%s>\n", ZMsg(MSG_WORD_ATTRIBUTE_POISON));
			str += temp;
			str += DelayTemp;
			if (pItemDesc->m_nDamage.Ref())
			{
				sprintf(temp, "%s : %d dmg/%s\n", ZMsg(MSG_WORD_DAMAGE), pItemDesc->m_nDamage.Ref(), ZMsg(MSG_CHARINFO_SECOND));
				str += temp;
			}
			break;
		}
	}


	// ÀÎÃ¾Æ® ¾ÆÀÌÅÛÀÌ ¾Æ´Ò°æ¿ì...
	else
	{
		if (pItemDesc->m_nMaxBullet.Ref()) {
			sprintf(temp, "%s : %d", ZMsg(MSG_WORD_BULLET), pItemDesc->m_nMagazine.Ref());

			if ((pItemDesc->m_nMaxBullet.Ref() / pItemDesc->m_nMagazine.Ref()) > 1)
				sprintf(temp, "%s x %d", temp, pItemDesc->m_nMaxBullet.Ref() / pItemDesc->m_nMagazine.Ref());

			str += temp;
			str += "\n";
		}

		if (pItemDesc->m_nDamage.Ref()) {
			sprintf(temp, "%s : %d dmg.\n", ZMsg(MSG_WORD_ATTACK), pItemDesc->m_nDamage.Ref());
			str += temp;
		}

		if (pItemDesc->m_nDelay.Ref()) {
			sprintf(temp, "%s : %d\n", ZMsg(MSG_WORD_DELAY), pItemDesc->m_nDelay.Ref());
			str += temp;
		}

		//		if(pItemDesc->m_nReloadTime) {
		//			sprintf(temp,"ÀåÀü½Ã°£ : %d s\n",pItemDesc->m_nReloadTime);
		//			str += temp;
		//		}


		// HP
		if (pItemDesc->m_nHP.Ref() != 0)
		{
			if (pItemDesc->m_nHP.Ref() > 0)
				sprintf(temp, "%s +%d\n", ZMsg(MSG_CHARINFO_HP), pItemDesc->m_nHP.Ref());
			else
				sprintf(temp, "%s %d\n", ZMsg(MSG_CHARINFO_HP), pItemDesc->m_nHP.Ref());

			str += temp;
		}


		// AP
		if (pItemDesc->m_nAP.Ref() != 0)
		{
			if (pItemDesc->m_nAP.Ref() > 0)
				sprintf(temp, "%s +%d\n", ZMsg(MSG_CHARINFO_AP), pItemDesc->m_nAP.Ref());
			else
				sprintf(temp, "%s %d\n", ZMsg(MSG_CHARINFO_AP), pItemDesc->m_nAP.Ref());

			str += temp;
		}


		if (pItemDesc->m_nMaxWT.Ref())
		{
			if (pItemDesc->m_nMaxWT.Ref() > 0)
				sprintf(temp, "%s +%d\n", ZMsg(MSG_WORD_MAXWEIGHT), pItemDesc->m_nMaxWT.Ref());
			else
				sprintf(temp, "%s %d\n", ZMsg(MSG_WORD_MAXWEIGHT), pItemDesc->m_nMaxWT.Ref());

			str += temp;
		}

		if (pItemDesc->m_nSF.Ref()) {
			sprintf(temp, "SF +%d\n", pItemDesc->m_nSF.Ref());
			str += temp;
		}

		if (pItemDesc->m_nFR.Ref()) {
			sprintf(temp, "FR +%d\n", pItemDesc->m_nFR.Ref());
			str += temp;
		}

		if (pItemDesc->m_nCR.Ref()) {
			sprintf(temp, "CR +%d\n", pItemDesc->m_nCR.Ref());
			str += temp;
		}

		if (pItemDesc->m_nPR.Ref()) {
			sprintf(temp, "PR +%d\n", pItemDesc->m_nPR.Ref());
			str += temp;
		}

		if (pItemDesc->m_nLR.Ref()) {
			sprintf(temp, "LR +%d\n", pItemDesc->m_nLR.Ref());
			str += temp;
		}

		str += "\n";

		if (pItemDesc->m_nLimitSpeed.Ref() < 100) {
			sprintf(temp, "%s -%d%%\n", ZMsg(MSG_WORD_RUNSPEED), 100 - pItemDesc->m_nLimitSpeed.Ref());
			str += temp;
		}

		if (pItemDesc->m_nLimitJump.Ref()) {
			sprintf(temp, "%s\n", ZMsg(MSG_WORD_DONOTJUMP));
			str += temp;
		}

		if (pItemDesc->m_nLimitTumble.Ref()) {
			sprintf(temp, "%s\n", ZMsg(MSG_WORD_DONOTDASH));
			str += temp;
		}

		if (pItemDesc->m_nLimitWall.Ref()) {
			sprintf(temp, "%s\n", ZMsg(MSG_WORD_DONOTHANGWALL));
			str += temp;
		}
	}

	return str;
}

string GetPeriodItemString(ZMyItemNode* pRentalNode)
{
	string str = "";
	char temp[1024];

	if (pRentalNode != NULL)
	{
		if ((pRentalNode->GetRentMinutePeriodRemainder() < RENT_MINUTE_PERIOD_UNLIMITED))
		{
			DWORD dwRemaind = pRentalNode->GetRentMinutePeriodRemainder();
			DWORD dwRecievedClock = (timeGetTime() - pRentalNode->GetWhenReceivedClock()) / 60000;

			if (dwRemaind <= dwRecievedClock)
			{
				str = ZMsg(MSG_EXPIRED);
			}
			else
			{
				dwRemaind -= dwRecievedClock;

				if (dwRemaind > 1440)
				{
					sprintf(temp, "%d%s ", dwRemaind / 1440, ZMsg(MSG_CHARINFO_DAY));
					str += temp;
					dwRemaind %= 1440;
				}

				sprintf(temp, "%d%s ", dwRemaind / 60, ZMsg(MSG_CHARINFO_HOUR));
				str += temp;
				dwRemaind %= 60;

				sprintf(temp, "%d%s", dwRemaind, ZMsg(MSG_CHARINFO_MINUTE));
				str += temp;

				str += "  ";
				str += ZMsg(MSG_REMAIND_PERIOD);
			}
		}
	}

	return str;
}


// ============= ZShopEquipItem »óÁ¡/Àåºñ ¾ÆÀÌÅÛ ±âº» Å¬·¡½º================================================

ZShopEquipItem::~ZShopEquipItem()
{
	delete m_pHandlerPurchase;
	delete m_pHandlerSell;
	delete m_pHandlerSendAccount;
	delete m_pHandlerBringAccount;
}

const char* ZShopEquipItem::GetPriceText(char* szBuf)
{
	_ASSERT(!(m_pHandlerPurchase && m_pHandlerSell));	// µÎ°¡Áö°¡ µ¿½Ã¿¡ Á¸ÀçÇÑ´Ù´Â °ÍÀº ¹º°¡ Àß¸øµÇ¾ú´Ù´Â °Í

	if (m_pHandlerPurchase)
		return m_pHandlerPurchase->GetPriceText(szBuf);
	else if (m_pHandlerSell)
		return m_pHandlerSell->GetPriceText(szBuf);

	szBuf[0] = 0;
	return szBuf;
}

int ZShopEquipItem::GetPrice(bool& out_bCash)
{
	_ASSERT(!(m_pHandlerPurchase && m_pHandlerSell));	// µÎ°¡Áö°¡ µ¿½Ã¿¡ Á¸ÀçÇÑ´Ù´Â °ÍÀº ¹º°¡ Àß¸øµÇ¾ú´Ù´Â °Í

	bool bCash(false);
	int nPrice(0), nRentHour(0);
	if (m_pHandlerPurchase)
		m_pHandlerPurchase->GetPrice(bCash, nRentHour, nPrice);
	else if (m_pHandlerSell)
		m_pHandlerSell->GetPrice(nPrice);
	return nPrice;
}

// ============= ZShopEquipItem_Gamble °·ºí ¾ÆÀÌÅÛ ================================================

ZShopEquipItem_Gamble::ZShopEquipItem_Gamble(const ZGambleItemDefine* pDesc) : m_pItemDesc(pDesc)
{
	_ASSERT(m_pItemDesc);

	if (m_pItemDesc->IsCash())
		m_pIconBitmap = MBitmapManager::Get("slot_icon_gamble_cash.tga");

	else
		m_pIconBitmap = MBitmapManager::Get("slot_icon_gamble1.tga");
}

const char* ZShopEquipItem_Gamble::GetName(char* szBuf)
{
	strcpy(szBuf, m_pItemDesc->GetName().c_str());

	char sz[256];
	if (m_pHandlerSell)
	{
		MUID uid = m_pHandlerSell->GetItemUID();
		const ZMyGambleItem* pMyGambleItem = ZGetMyInfo()->GetItemList()->GetGambleItem(uid);
		if (pMyGambleItem && pMyGambleItem->GetItemCount() > 0)
		{
			sprintf(sz, "\n(x%d)", pMyGambleItem->GetItemCount());
			strcat(szBuf, sz);
		}
	}
	else if (m_pHandlerBringAccount && m_pHandlerBringAccount->GetMyItemNode())
	{
		sprintf(sz, "\n(x%d)", m_pHandlerBringAccount->GetMyItemNode()->GetItemCount());
		strcat(szBuf, sz);
	}

	return szBuf;
}

const char* ZShopEquipItem_Gamble::GetLevelResText(char* szBuf)
{
	sprintf(szBuf, "%s-", ZMsg(MSG_CHARINFO_LEVELMARKER));
	return szBuf;
}

//int ZShopEquipItem_Gamble::GetPrice( bool& out_bCash )
//{
//	out_bCash = m_pItemDesc->IsCash();
//	return m_pItemDesc->GetPrice();
//}

int ZShopEquipItem_Gamble::GetLevelRes()
{
	return 0;
}

void ZShopEquipItem_Gamble::FillItemDesc(MTextArea* pTextArea)
{
	if (!pTextArea) return;

	pTextArea->SetTextColor(MCOLOR(255, 255, 255));
	pTextArea->SetText(m_pItemDesc->GetName().c_str());

	pTextArea->AddText("\n\n\n");
	pTextArea->AddText(m_pItemDesc->GetDescription().c_str(), MCOLOR(120, 120, 120));

	pTextArea->AdjustHeightByContent();
}

void ZShopEquipItem_Gamble::UpdateCharInfoText()
{
	bool bCash;
	int nPrice = GetPrice(bCash);
	ZGetGameInterface()->GetShopEquipInterface()->DrawCharInfoText(NULL,
		0,
		ZGetMyInfo()->GetItemList()->GetEquipedTotalWeight(),
		ZGetMyInfo()->GetItemList()->GetMaxWeight(),
		ZGetMyInfo()->GetHP(),
		ZGetMyInfo()->GetAP(),
		bCash ? 0 : nPrice,
		!bCash ? nPrice : 0);	//todok Ä³½¬°¡°Ý Ã³¸®ÇØÁà¾ßÇÔ
}

// ============= ZShopEquipItem_Match Àåºñ ¾ÆÀÌÅÛ ================================================

ZShopEquipItem_Match::ZShopEquipItem_Match(MMatchItemDesc* pDesc) : m_pItemDesc(pDesc)
{
	_ASSERT(m_pItemDesc);
	m_pIconBitmap = GetItemIconBitmap(m_pItemDesc);
}

MMatchItemSlotType ZShopEquipItem_Match::GetSlotType()
{
	return m_pItemDesc->m_nSlot;
}

const char* ZShopEquipItem_Match::GetName(char* szBuf)
{
	sprintf(szBuf, "%s", m_pItemDesc->m_pMItemName->Ref().m_szItemName);

	if (!m_pItemDesc->IsSpendableItem()) return szBuf;

	// ¼ö·® Ç¥½Ã°¡ ÇÊ¿äÇÑ °æ¿ì

	char sz[256] = "";

	if (m_pHandlerPurchase)
	{
		/* Ä³½¬ ¼Ò¸ð¼º ¾ÆÀÌÅÛÀÇ °æ¿ì ¹­À½À¸·Î ÆÈ¸®±â ¶§¹®¿¡ ÀÌ ÄÚµå°¡ ÇÊ¿äÇÏ´Ù
		if (pShopItem && pShopItem->GetCount() > 1)
		sprintf(sz, "\n(x%d)", pShopItem->GetCount());
		*/
	}
	else if (m_pHandlerSell)
	{
		ZMyItemNode* pMyItemNode = ZGetMyInfo()->GetItemList()->GetItem(m_pHandlerSell->GetItemUID());
		if (pMyItemNode)
			sprintf(sz, "\n(x%d)", pMyItemNode->GetItemCount());
	}
	else if (m_pHandlerBringAccount && m_pHandlerBringAccount->GetMyItemNode())
	{
		sprintf(sz, "\n(x%d)", m_pHandlerBringAccount->GetMyItemNode()->GetItemCount());
	}

	strcat(szBuf, sz);
	return szBuf;
}

const char* ZShopEquipItem_Match::GetLevelResText(char* szBuf)
{
	int nResLevel = m_pItemDesc->m_nResLevel.Ref();
	if (nResLevel == 0 || nResLevel == 1)
		sprintf(szBuf, "%s-", ZMsg(MSG_CHARINFO_LEVELMARKER));
	else
		sprintf(szBuf, "%s%d", ZMsg(MSG_CHARINFO_LEVELMARKER), nResLevel);
	return szBuf;
}

int ZShopEquipItem_Match::GetLevelRes()
{
	return m_pItemDesc->m_nResLevel.Ref();
}

void ZShopEquipItem_Match::UpdateCharacterView(ZCharacterView* pCharacterView)
{
	MMatchCharItemParts nCharItemParts = GetSuitableItemParts(m_pItemDesc->m_nSlot);

	pCharacterView->SetSelectMyCharacter();
	pCharacterView->SetParts(nCharItemParts, m_pItemDesc->m_nID);

	if (IsWeaponCharItemParts(nCharItemParts))
		pCharacterView->ChangeVisualWeaponParts(nCharItemParts);
}

void ZShopEquipItem_Match::FillItemDesc(MTextArea* pTextArea)
{
	ZMyItemNode* pRentalNode = NULL;
	if (m_pHandlerSell)
	{
		MUID uidItem = m_pHandlerSell->GetItemUID();
		pRentalNode = ZGetMyInfo()->GetItemList()->GetItem(uidItem);
	}
	else if (m_pHandlerBringAccount)
	{
		int nAIID = m_pHandlerBringAccount->GetAIID();
		pRentalNode = ZGetMyInfo()->GetItemList()->GetAccountItemByAIID(nAIID);
	}

	FillItemDesc(m_pItemDesc, pTextArea, pRentalNode);
}

void ZShopEquipItem_Match::FillItemDesc(MMatchItemDesc* pItemDesc, MTextArea* pTextArea, ZMyItemNode* pRentalNode)
{
	if (!pTextArea) return;

	// Àåºñ ÀÌ¸§
	pTextArea->SetTextColor(MCOLOR(255, 255, 255));
	pTextArea->SetText(pItemDesc->m_pMItemName->Ref().m_szItemName);

	// Á¦¾à»çÇ×
	pTextArea->AddText("\n\n");
	pTextArea->AddText(GetRestrictionString(pItemDesc).c_str(), MCOLOR(170, 170, 170));

	// ±â´É
	pTextArea->AddText("\n");
	pTextArea->AddText(GetItemSpecString(pItemDesc).c_str(), MCOLOR(170, 170, 170));

	// ¼Ò¸ð¼º Ç¥½Ã
	if (pItemDesc->IsSpendableItem())
	{
		pTextArea->AddText("\n");
		pTextArea->AddText(ZMsg(MSG_SHOPEQUIP_SPENDABLE), MCOLOR(200, 0, 0));
	}

	//if (!bBriefly)
	{
		// ±â°£Á¦ Ç¥½Ã
		if (pRentalNode)
		{
			pTextArea->AddText("\n");
			pTextArea->AddText(GetPeriodItemString(pRentalNode).c_str(), MCOLOR(200, 0, 0));
		}

		// ¾ÆÀÌÅÛ ¼³¸í
		pTextArea->AddText("\n");
		pTextArea->AddText(pItemDesc->m_szDesc, MCOLOR(120, 120, 120));
	}

	pTextArea->AdjustHeightByContent();
}

void ZShopEquipItem_Match::UpdateCharInfoText()
{
	static MMatchItemDesc nullItem;
	MMatchItemDesc* pEquipedItemDesc = &nullItem;
	MMatchCharItemParts parts = ZGetGameInterface()->GetShopEquipInterface()->RecommendEquipParts(m_pItemDesc->m_nSlot);
	ZMyItemNode* pMyItemNode = ZGetMyInfo()->GetItemList()->GetEquipedItem(parts);
	if (pMyItemNode)
		pEquipedItemDesc = MGetMatchItemDescMgr()->GetItemDesc(pMyItemNode->GetItemID());

	ZGetGameInterface()->GetShopEquipInterface()->DrawCharInfoText(NULL,
		m_pItemDesc->m_nResLevel.Ref(),
		ZGetMyInfo()->GetItemList()->GetEquipedTotalWeight() - pEquipedItemDesc->m_nWeight.Ref() + m_pItemDesc->m_nWeight.Ref(),
		ZGetMyInfo()->GetItemList()->GetMaxWeight() - pEquipedItemDesc->m_nMaxWT.Ref() + m_pItemDesc->m_nMaxWT.Ref(),
		ZGetMyInfo()->GetHP() - pEquipedItemDesc->m_nHP.Ref() + m_pItemDesc->m_nHP.Ref(),
		ZGetMyInfo()->GetAP() - pEquipedItemDesc->m_nAP.Ref() + m_pItemDesc->m_nAP.Ref(),
		m_pItemDesc->m_nBountyPrice.Ref(),
		100);	//todok Ä³½¬°¡°Ý Ã³¸®ÇØÁà¾ßÇÔ
}


// ============= ZShopEquipItem_Quest Äù½ºÆ® ¾ÆÀÌÅÛ ================================================

ZShopEquipItem_Quest::ZShopEquipItem_Quest(MQuestItemDesc* pDesc) : m_pItemDesc(pDesc)
{
	_ASSERT(m_pItemDesc);
	m_pIconBitmap = ZApplication::GetGameInterface()->GetQuestItemIcon(pDesc->m_nItemID, true);
}

const char* ZShopEquipItem_Quest::GetName(char* szBuf)
{
	strcpy(szBuf, m_pItemDesc->m_szQuestItemName);

	if (m_pHandlerSell)
	{
		ZMyQuestItemNode* pNode = ZGetMyInfo()->GetItemList()->GetQuestItemMap().Find(m_pItemDesc->m_nItemID);
		if (pNode && pNode->GetCount() > 0)
		{
			char sz[256];
			sprintf(sz, "\n(x%d)", pNode->GetCount());
			strcat(szBuf, sz);
		}
	}

	return szBuf;
}

const char* ZShopEquipItem_Quest::GetLevelResText(char* szBuf)
{
	sprintf(szBuf, "%s-", ZMsg(MSG_CHARINFO_LEVELMARKER));
	return szBuf;
}

//int ZShopEquipItem_Quest::GetPrice( bool& out_bCash )
//{
//	return m_pItemDesc->m_nPrice;
//}

int ZShopEquipItem_Quest::GetLevelRes()
{
	return 0;
}

void ZShopEquipItem_Quest::FillItemDesc(MTextArea* pTextArea)
{
	if (!pTextArea) return;

	// Àåºñ ÀÌ¸§
	pTextArea->SetTextColor(MCOLOR(255, 255, 255));
	pTextArea->SetText(m_pItemDesc->m_szQuestItemName);
	pTextArea->AddText("\n");
	char szText[256];
	sprintf(szText, "(%s %s)", ZMsg(MSG_WORD_QUEST), ZMsg(MSG_WORD_ITEM));
	pTextArea->AddText(szText, MCOLOR(170, 170, 170));
	pTextArea->AddText("\n\n");

	// Á¦¾à»çÇ×
	if (m_pItemDesc->m_bSecrifice)
	{
		sprintf(szText, "%s %s", ZMsg(MSG_WORD_SACRIFICE), ZMsg(MSG_WORD_ITEM));
		pTextArea->AddText(szText, MCOLOR(170, 170, 170));
	}
	// ¾ÆÀÌÅÛ ¼³¸í
	pTextArea->AddText(m_pItemDesc->m_szDesc, MCOLOR(120, 120, 120));

	pTextArea->AdjustHeightByContent();
}

void ZShopEquipItem_Quest::UpdateCharInfoText()
{
	bool bCash;
	int nPrice = GetPrice(bCash);
	ZGetGameInterface()->GetShopEquipInterface()->DrawCharInfoText(NULL,
		0,
		ZGetMyInfo()->GetItemList()->GetEquipedTotalWeight(),
		ZGetMyInfo()->GetItemList()->GetMaxWeight(),
		ZGetMyInfo()->GetHP(),
		ZGetMyInfo()->GetAP(),
		bCash ? 0 : nPrice,
		!bCash ? nPrice : 0);	//todok Ä³½¬°¡°Ý Ã³¸®ÇØÁà¾ßÇÔ
}


// ============= ZShopEquipItem_Set ¼¼Æ® ¾ÆÀÌÅÛ ================================================

ZShopEquipItem_Set::ZShopEquipItem_Set(MShopSetItem* pSetItem)
{
	/*	¼¼Æ® ¾ÆÀÌÅÛÀÌ ¿ÀÇÂµÇ¸é ÄÚµå¸¦ ¾Ë¸Â°Ô ¼öÁ¤ÇØ¼­ »ç¿ëÇÏÀÚ

	_ASSERT(pSetItem);
	m_pShopSetItem = pSetItem;

	//¼¼Æ®¸¦ ±¸¼ºÇÏ´Â °¢ ¾ÆÀÌÅÛÀ» ¼øÈ¸ÇØ¼­ °¡Àå ³ôÀº ·¾Á¦¸¦ ¼¼Æ® ÀüÃ¼ÀÇ ·¾Á¦·Î ¼±ÅÃ
	//±×¹Û¿¡ ³ªÁß¿¡ ÇÊ¿äÇÑ ÇÕ»êÄ¡µéÀ» °è»êÇØµÐ´Ù
	int nHighestLevelRestrict = 0;
	bool bSexResMale(false), bSexResFemale(false);
	MMatchItemDesc* pDesc;
	for (int i=0; i<MAX_SET_ITEM_COUNT; ++i)
	{
	pDesc = MGetMatchItemDescMgr()->GetItemDesc( pSetItem->GetItemID(i));
	if (!pDesc) continue;

	if (nHighestLevelRestrict < pDesc->m_nResLevel.Ref())
	nHighestLevelRestrict = pDesc->m_nResLevel.Ref();


	if (pDesc->m_nResSex.Ref() == 0) bSexResMale = true;
	if (pDesc->m_nResSex.Ref() == 1) bSexResFemale = true;

	m_sumSetSpec.m_nWeight.Ref()		+= pDesc->m_nWeight.Ref();

	m_sumSetSpec.m_nMaxBullet.Ref()		+= pDesc->m_nMaxBullet.Ref();
	m_sumSetSpec.m_nMagazine.Ref()		+= pDesc->m_nMagazine.Ref();
	m_sumSetSpec.m_nDamage.Ref()		+= pDesc->m_nDamage.Ref();
	m_sumSetSpec.m_nDelay.Ref()			+= pDesc->m_nDelay.Ref();
	m_sumSetSpec.m_nHP.Ref()			+= pDesc->m_nHP.Ref();
	m_sumSetSpec.m_nAP.Ref()			+= pDesc->m_nAP.Ref();
	m_sumSetSpec.m_nMaxWT.Ref()			+= pDesc->m_nMaxWT.Ref();
	m_sumSetSpec.m_nSF.Ref()			+= pDesc->m_nSF.Ref();
	m_sumSetSpec.m_nFR.Ref()			+= pDesc->m_nFR.Ref();
	m_sumSetSpec.m_nCR.Ref()			+= pDesc->m_nCR.Ref();
	m_sumSetSpec.m_nPR.Ref()			+= pDesc->m_nPR.Ref();
	m_sumSetSpec.m_nLR.Ref()			+= pDesc->m_nLR.Ref();
	m_sumSetSpec.m_nLimitSpeed.Ref()	+= pDesc->m_nLimitSpeed.Ref();
	m_sumSetSpec.m_nLimitJump.Ref()		+= pDesc->m_nLimitJump.Ref();
	m_sumSetSpec.m_nLimitTumble.Ref()	+= pDesc->m_nLimitTumble.Ref();
	m_sumSetSpec.m_nLimitWall.Ref()		+= pDesc->m_nLimitWall.Ref();
	}
	m_nLevelRes = nHighestLevelRestrict;

	_ASSERT(!(bSexResMale && bSexResFemale));	// ºÎÀ§º°·Î ´Ù¸¥ ¼ºº°ÀÇ ¼¼Æ®ÅÛÀº ¾ø°ÚÁÒ;
	if (bSexResMale) m_nSexRes = 0;
	else if (bSexResFemale) m_nSexRes = 1;
	else m_nSexRes = -1;
	*/
}

const char* ZShopEquipItem_Set::GetName(char* szBuf)
{
	/*	¼¼Æ® ¾ÆÀÌÅÛÀÌ ¿ÀÇÂµÇ¸é ÄÚµå¸¦ ¾Ë¸Â°Ô ¼öÁ¤ÇØ¼­ »ç¿ëÇÏÀÚ
	strcpy(szBuf, m_pShopSetItem->GetName());
	*/
	return szBuf;
}

const char* ZShopEquipItem_Set::GetLevelResText(char* szBuf)
{
	int nResLevel = GetLevelRes();
	if (nResLevel == 0 || nResLevel == 1)
		sprintf(szBuf, "%s-", ZMsg(MSG_CHARINFO_LEVELMARKER));
	else
		sprintf(szBuf, "%s%d", ZMsg(MSG_CHARINFO_LEVELMARKER), nResLevel);
	return szBuf;
}

int ZShopEquipItem_Set::GetLevelRes()
{
	return m_nLevelRes;
}

void ZShopEquipItem_Set::UpdateCharacterView(ZCharacterView* pCharacterView)
{
	/*	¼¼Æ® ¾ÆÀÌÅÛÀÌ ¿ÀÇÂµÇ¸é ÄÚµå¸¦ ¾Ë¸Â°Ô ¼öÁ¤ÇØ¼­ »ç¿ëÇÏÀÚ
	pCharacterView->SetSelectMyCharacter();
	MMatchItemDesc* pDesc;
	for (int i=0; i<MAX_SET_ITEM_COUNT; ++i)
	{
	int partsItemId = m_pShopSetItem->GetItemID(i);
	if (partsItemId != 0)
	{
	pDesc = MGetMatchItemDescMgr()->GetItemDesc(partsItemId);
	if (!pDesc) { _ASSERT(0); continue; }

	MMatchCharItemParts nCharItemParts = GetSuitableItemParts(pDesc->m_nSlot);
	pCharacterView->SetParts(nCharItemParts, pDesc->m_nID);
	}
	}
	*/
}

void ZShopEquipItem_Set::UpdateCharInfoText()
{
	/*	¼¼Æ® ¾ÆÀÌÅÛÀÌ ¿ÀÇÂµÇ¸é ÄÚµå¸¦ ¾Ë¸Â°Ô ¼öÁ¤ÇØ¼­ »ç¿ëÇÏÀÚ
	static MMatchItemDesc nullItem;
	MMatchItemDesc* pEquipedItemDesc;
	MMatchItemDesc* pItemDesc;
	ZMyItemNode* pMyItemNode;

	int nNewWT = ZGetMyInfo()->GetItemList()->GetEquipedTotalWeight();
	int nNewMaxWT = ZGetMyInfo()->GetItemList()->GetMaxWeight();
	int nNewHP = ZGetMyInfo()->GetHP();
	int nNewAP = ZGetMyInfo()->GetAP();

	for (int i=0; i<MAX_SET_ITEM_COUNT; ++i)
	{
	pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(m_pShopSetItem->GetItemID(i));
	if (!pItemDesc) continue;

	pEquipedItemDesc = &nullItem;
	pMyItemNode = ZGetMyInfo()->GetItemList()->GetEquipedItem( GetSuitableItemParts( pItemDesc->m_nSlot));
	if (pMyItemNode)
	pEquipedItemDesc = MGetMatchItemDescMgr()->GetItemDesc( pMyItemNode->GetItemID());

	nNewWT		+= - pEquipedItemDesc->m_nWeight.Ref() + pItemDesc->m_nWeight.Ref();
	nNewMaxWT	+= - pEquipedItemDesc->m_nMaxWT.Ref() + pItemDesc->m_nMaxWT.Ref();
	nNewHP		+= - pEquipedItemDesc->m_nHP.Ref() + pItemDesc->m_nHP.Ref();
	nNewAP		+= - pEquipedItemDesc->m_nAP.Ref() + pItemDesc->m_nAP.Ref();
	}

	ZGetGameInterface()->GetShopEquipInterface()->DrawCharInfoText(NULL,
	GetLevelRes(),
	nNewWT,
	nNewMaxWT,
	nNewHP,
	nNewAP,
	0,		//todok ¼¼Æ®ÀÇ ¹Ù¿îÆ¼°¡°Ý ?
	100);	//todok ¼¼Æ®ÀÇ Ä³½¬°¡°Ý Ã³¸®ÇØÁà¾ßÇÔ
	*/
}

void ZShopEquipItem_Set::FillItemDesc(MTextArea* pTextArea)
{
	/*	¼¼Æ® ¾ÆÀÌÅÛÀÌ ¿ÀÇÂµÇ¸é ÄÚµå¸¦ ¾Ë¸Â°Ô ¼öÁ¤ÇØ¼­ »ç¿ëÇÏÀÚ
	if (!pTextArea) return;

	// Àåºñ ÀÌ¸§
	pTextArea->SetTextColor( MCOLOR(255,255,255));
	pTextArea->SetText( m_pShopSetItem->GetName());

	// Á¦¾à»çÇ×
	pTextArea->AddText( "\n\n");
	pTextArea->AddText( GetRestrictionString(m_nSexRes, GetLevelRes(), m_sumSetSpec.m_nWeight.Ref()).c_str(), MCOLOR( 170, 170, 170));

	// ±â´É
	pTextArea->AddText( "\n");
	pTextArea->AddText( GetItemSpecString(&m_sumSetSpec).c_str(), MCOLOR( 170, 170, 170));

	//todok ±¸¸ÅÃ¢¿¡¼­ ¼¼ºÎ ¼³¸í »ý·«ÇÒ ¼ö ÀÖµµ·Ï ÇØÁÖÀÚ
	//if (!bBriefly)
	{
	// ¾ÆÀÌÅÛ ¼³¸í
	pTextArea->AddText( "\n");
	pTextArea->AddText( m_pShopSetItem->GetDescription(), MCOLOR( 120, 120, 120));
	}

	pTextArea->AdjustHeightByContent();
	*/
}


// ============= IShopEquipItemHandle_Purchase ±¸¸Å ÇÚµé·¯ ================================================

const char* IShopEquipItemHandle_Purchase::GetPriceText(char* szBuf)
{
	szBuf[0] = 0;

	bool bCash;
	int nRentHour;
	int nPrice;
	if (!GetPrice(bCash, nRentHour, nPrice))
		return szBuf;

	char sz[256];
	if (nRentHour != 0)
	{
		sprintf(sz, "(%d%s) ", nRentHour / 24, ZMsg(MSG_CHARINFO_DAY));
		strcat(szBuf, sz);
	}
	/// Jorklenis2: <CashShop>
	const char* szCurrency;
	if (ZGetShop()->m_ShopMode == ZSHOP_CASH)
	{
		szCurrency = "Cash";
	}
	else if (ZGetShop()->m_ShopMode == ZSHOP_EVENT)
	{
		szCurrency = "Event";
	}
	else
	{
		szCurrency = "Bounty";
	}

	if (ZGetShop()->m_ShopMode == ZSHOP_CASH)
	{
		sprintf(sz, "%d %s", nPrice, szCurrency);
		strcat(szBuf, sz);
	}
	else if (ZGetShop()->m_ShopMode == ZSHOP_EVENT)
	{
		sprintf(sz, "%d %s", nPrice, szCurrency);
		strcat(szBuf, sz);
	}
	else
	{
		sprintf(sz, "%d %s", nPrice, szCurrency);
		strcat(szBuf, sz);
	}

	return szBuf;
}

void IShopEquipItemHandle_Purchase::OpenSimpleConfirmDlg(ISimpleConfirmDlgDoneHandler* pDoneHandler)
{
	ZSimpleConfirmDlg* pDlg = ZGetGameInterface()->GetShopEquipInterface()->GetSimpleConfirmDlg();
	if (!pDlg) { _ASSERT(0); return; }
	pDlg->Open(ZMsg(MSG_SHOPEQUIP_BUY_CONFIRM), pDoneHandler);
}

void IShopEquipItemHandle_Purchase::OpenCountableConfirmDlg(
	const char* szItemName, MBitmap* pIconBmp, int nPrice, int nMax, IItemCountDlgDoneHandler* pDoneHandler)
{
	ZItemCountDlg* pDlg = ZGetGameInterface()->GetShopEquipInterface()->GetItemCountDlg();
	if (!pDlg) { _ASSERT(0); return; }
	pDlg->Open(ZICD_BUY, szItemName, pIconBmp, nPrice, 0, nMax, pDoneHandler);
}


// ============= ZShopEquipItemHandle_PurchaseGamble ±¸¸Å ÇÚµé·¯ : °·ºí================================================

ZShopEquipItemHandle_PurchaseGamble::ZShopEquipItemHandle_PurchaseGamble(ZShopEquipItem_Gamble* pItem)
{
	m_pItem = pItem;
	_ASSERT(m_pItem);
}

void ZShopEquipItemHandle_PurchaseGamble::Buy()
{
	if (!m_pItem->GetDesc()) { _ASSERT(0); return; }
	if (!m_pItem->GetDesc()->IsCash())
	{
		OpenSimpleConfirmDlg(this);
	}
	else
	{
#ifndef _PUBLISH
		OpenSimpleConfirmDlg(this);//todok ÀÓ½Ã·Î Ä³½¬ÅÛÀ» ¹Ù¿îÆ¼·Î ±¸¸Å °¡´É
#endif
	}
}

void ZShopEquipItemHandle_PurchaseGamble::OnDone(bool bOk)
{
	if (!bOk) return;

	ZPostRequestBuyItem(ZGetMyUID(), m_pItem->GetDesc()->GetGambleItemID(), 1);
	ZPostRequestCharacterItemList(ZGetGameClient()->GetPlayerUID());
}

bool ZShopEquipItemHandle_PurchaseGamble::GetPrice(bool& out_bCash, int& out_nRentalHour, int& out_nPrice)
{
	if (!m_pItem || !m_pItem->GetDesc()) { _ASSERT(0); return false; }
	out_bCash = m_pItem->GetDesc()->IsCash();
	if (out_bCash) return false; // ¾ÆÁ÷Àº »óÁ¡¿¡¼­ Ä³½¬¸¦ ÆÈÁö ¾ÊÀ¸¹Ç·Î Ä³½¬ ¾ÆÀÌÅÛÀÎ °æ¿ì ¹Ù¿îÆ¼·Î ÆÈ¸é Å«ÀÏÀÌ´Ï±î ±×³É ¸®ÅÏ
	out_nRentalHour = 0;
	out_nPrice = m_pItem->GetDesc()->GetPrice();
	return true;
}

// ============= ZShopEquipItemHandle_PurchaseMatch ±¸¸ÅÇÚµé·¯ : Àåºñ ================================================

ZShopEquipItemHandle_PurchaseMatch::ZShopEquipItemHandle_PurchaseMatch(ZShopEquipItem_Match* pItem)
{
	m_pItem = pItem;
	_ASSERT(m_pItem);
}

void ZShopEquipItemHandle_PurchaseMatch::Buy()
{
	if (!m_pItem->GetDesc()) { _ASSERT(0); return; }
	bool bCash = m_pItem->GetDesc()->IsCashItem();
	bool bEvent = m_pItem->GetDesc()->IsEventItem(); /// Jorklenis2 <IsEventItem>
	bool bSpendable = m_pItem->GetDesc()->IsSpendableItem();

	/// Jorklenis2: <Comprar artículos de cash y event>
	if (bCash || bEvent)
	{
#if !defined(_PUBLISH) || defined(_MAX_SPENDABLE)

		if (bSpendable)
		{
			int nMaxBuyCount = MAX_SPENDABLE_ITEM_COUNT;

			ZMyItemNode* pMyItemNode = ZGetMyInfo()->GetItemList()->GetItemByDescId(m_pItem->GetDesc()->m_nID);
			if (pMyItemNode)
				nMaxBuyCount -= pMyItemNode->GetItemCount();

			if (nMaxBuyCount == 0) {
				ZApplication::GetGameInterface()->ShowErrorMessage(MERR_TOO_MANY_ITEM);
				return;
			}

			OpenCountableConfirmDlg(
				m_pItem->GetDesc()->m_pMItemName->Ref().m_szItemName,
				m_pItem->GetIcon(),
				m_pItem->GetDesc()->m_nBountyPrice.Ref(),
				nMaxBuyCount, this);
		}
		else
		{
			OpenSimpleConfirmDlg(this);
		}
#endif
	}
	else
	{
		if (bSpendable)
		{
			int nMaxBuyCount = MAX_SPENDABLE_ITEM_COUNT;

			ZMyItemNode* pMyItemNode = ZGetMyInfo()->GetItemList()->GetItemByDescId(m_pItem->GetDesc()->m_nID);
			if (pMyItemNode)
				nMaxBuyCount -= pMyItemNode->GetItemCount();

			if (nMaxBuyCount == 0) {
				ZApplication::GetGameInterface()->ShowErrorMessage(MERR_TOO_MANY_ITEM);
				return;
			}

			OpenCountableConfirmDlg(
				m_pItem->GetDesc()->m_pMItemName->Ref().m_szItemName,
				m_pItem->GetIcon(),
				m_pItem->GetDesc()->m_nBountyPrice.Ref(),
				nMaxBuyCount, this);
		}
		else
		{
			OpenSimpleConfirmDlg(this);
		}
	}
}


void ZShopEquipItemHandle_PurchaseMatch::OnDone(bool bOk)
{
	if (!bOk) return;

	ZPostRequestBuyItem(ZGetMyUID(), m_pItem->GetDesc()->m_nID, 1);
	ZPostRequestCharacterItemList(ZGetGameClient()->GetPlayerUID());
}

void ZShopEquipItemHandle_PurchaseMatch::OnDone(int nCount)
{
	ZPostRequestBuyItem(ZGetMyUID(), m_pItem->GetDesc()->m_nID, nCount);
	ZPostRequestCharacterItemList(ZGetGameClient()->GetPlayerUID());
}

bool ZShopEquipItemHandle_PurchaseMatch::GetPrice(bool& out_bCash, int& out_nRentalHour, int& out_nPrice)
{
	if (!m_pItem || !m_pItem->GetDesc()) { _ASSERT(0); return false; }
	out_bCash = m_pItem->GetDesc()->IsCashItem() || m_pItem->GetDesc()->IsEventItem(); /// Jorklenis2: <IsEventItem>
	//if (out_bCash) return false;
	out_nRentalHour = m_pItem->GetDesc()->m_nMaxRentPeriod.Ref() * 24;
	out_nPrice = m_pItem->GetDesc()->m_nBountyPrice.Ref();
	return true;
}

// ============= ZShopEquipItemHandle_PurchaseQuest ±¸¸Å ÇÚµé·¯ : Äù½ºÆ®================================================

ZShopEquipItemHandle_PurchaseQuest::ZShopEquipItemHandle_PurchaseQuest(ZShopEquipItem_Quest* pItem)
{
	m_pItem = pItem;
	_ASSERT(m_pItem);
}

void ZShopEquipItemHandle_PurchaseQuest::Buy()
{
	if (!m_pItem->GetDesc()) { _ASSERT(0); return; }

	// ¼­¹öÂÊ ¼ÒÁöÇÑ°è °Ë»ç ÄÚµå°¡ count<=MAX_QUEST_ITEM_COUNT°¡ ¾Æ´Ï¶ó count<MAX_QUEST_ITEM_COUNTÀ¸·Î µÅÀÖ¾î¼­ -1
	int nMaxBuyCount = MAX_QUEST_ITEM_COUNT - 1;

	ZMyQuestItemMap& mapQuestItem = ZGetMyInfo()->GetItemList()->GetQuestItemMap();
	ZMyQuestItemNode* pMyItemNode = mapQuestItem.Find(m_pItem->GetDesc()->m_nItemID);

	if (pMyItemNode)
		nMaxBuyCount -= pMyItemNode->GetCount();

	_ASSERT(0 <= nMaxBuyCount && nMaxBuyCount <= MAX_QUEST_ITEM_COUNT - 1);

	if (nMaxBuyCount == 0) {
		ZApplication::GetGameInterface()->ShowErrorMessage(MERR_TOO_MANY_ITEM);
		return;
	}

	OpenCountableConfirmDlg(
		m_pItem->GetDesc()->m_szQuestItemName,
		m_pItem->GetIcon(),
		m_pItem->GetDesc()->m_nPrice,
		nMaxBuyCount, this);
}

void ZShopEquipItemHandle_PurchaseQuest::OnDone(int nCount)
{
	ZPostRequestBuyQuestItem(ZGetMyUID(), m_pItem->GetDesc()->m_nItemID, nCount);
	ZPostRequestCharacterItemList(ZGetGameClient()->GetPlayerUID());
}

bool ZShopEquipItemHandle_PurchaseQuest::GetPrice(bool& out_bCash, int& out_nRentalHour, int& out_nPrice)
{
	if (!m_pItem || !m_pItem->GetDesc()) { _ASSERT(0); return false; }
	out_bCash = false;
	out_nRentalHour = 0;
	out_nPrice = m_pItem->GetDesc()->m_nPrice;
	return true;
}

// ============= ZShopEquipItemHandle_PurchaseSet ±¸¸Å ÇÚµé·¯ : ¼¼Æ® ================================================

ZShopEquipItemHandle_PurchaseSet::ZShopEquipItemHandle_PurchaseSet(ZShopEquipItem_Set* pItem)
{
	m_pItem = pItem;
	_ASSERT(m_pItem);
}

void ZShopEquipItemHandle_PurchaseSet::Buy()
{

}

// ============= IShopEquipItemHandle_Sell ÆÇ¸Å ÇÚµé·¯ ================================================

const char* IShopEquipItemHandle_Sell::GetPriceText(char* szBuf)
{
	szBuf[0] = 0;
	int nPrice = 0;
	if (!GetPrice(nPrice))
		return szBuf;

	sprintf(szBuf, "%d %s", nPrice, ZMsg(MSG_CHARINFO_BOUNTY));
	return szBuf;
}

void IShopEquipItemHandle_Sell::OpenCountableConfirmDlg(const char* szItemName, MBitmap* pIconBmp, int nPrice, int nMax, IItemCountDlgDoneHandler* pDoneHandler)
{
	ZItemCountDlg* pDlg = ZGetGameInterface()->GetShopEquipInterface()->GetItemCountDlg();
	if (!pDlg) { _ASSERT(0); return; }
	pDlg->Open(ZICD_SELL, szItemName, pIconBmp, nPrice, 0, nMax, pDoneHandler);
}

void IShopEquipItemHandle_Sell::OpenSimpleConfirmDlg(ISimpleConfirmDlgDoneHandler* pDoneHandler)
{
	ZSimpleConfirmDlg* pDlg = ZGetGameInterface()->GetShopEquipInterface()->GetSimpleConfirmDlg();
	if (!pDlg) { _ASSERT(0); return; }
	pDlg->Open(ZMsg(MSG_SHOPEQUIP_SELL_CONFIRM), pDoneHandler);
}

void IShopEquipItemHandle_Sell::OpenSellCashItemConfirmDlg(const char* szItemName, MBitmap* pIcon, int price, int count, ISellCashItemConfirmDlgDoneHandler* pHandler)
{
	ZSellCashItemConfirmDlg* pDlg = ZGetGameInterface()->GetShopEquipInterface()->GetSellCashItemConfirmDlg();
	if (!pDlg) { _ASSERT(0); return; }
	pDlg->Open(szItemName, pIcon, price, count, pHandler);
}

// ============= ZShopEquipItemHandle_SellGamble ÆÇ¸Å ÇÚµé·¯ : °·ºí ================================================

ZShopEquipItemHandle_SellGamble::ZShopEquipItemHandle_SellGamble(ZShopEquipItem_Gamble* pItem)
{
	m_pItem = pItem;
	_ASSERT(m_pItem);
}

bool ZShopEquipItemHandle_SellGamble::GetPrice(int& out_nPrice)
{
	out_nPrice = 0;

	const ZGambleItemDefine* pDesc = m_pItem->GetDesc();
	if (!pDesc) { _ASSERT(pDesc); return false; }

	if (pDesc->IsCash())
	{
#ifdef _SELL_CASHITEM
		out_nPrice = CalculateCashItemToBounty(CASHITEM_GAMBLE);
		return true;
#else
		return false;
#endif
	}
	else
	{
		out_nPrice = int(pDesc->GetPrice() * 0.25);
		return true;
	}

	return false;
}

void ZShopEquipItemHandle_SellGamble::Sell()
{
	if (!m_pItem->GetDesc()) { _ASSERT(0); return; }

#ifndef _SELL_CASHITEM
	if (m_pItem->GetDesc()->IsCash()) { _ASSERT(0); return; }
#endif

	int nPrice = 0;
	if (!GetPrice(nPrice)) return;

	const ZMyGambleItem *pMyGItem = ZGetMyInfo()->GetItemList()->GetGambleItem(GetItemUID());
	if (!pMyGItem)  { _ASSERT(0); return; }
	int nMax = pMyGItem->GetItemCount();

	OpenCountableConfirmDlg(
		m_pItem->GetDesc()->GetName().c_str(),
		m_pItem->GetIcon(),
		nPrice,
		nMax, this);
}

void ZShopEquipItemHandle_SellGamble::OnDone(int nCount)
{
	ZPostRequestSellItem(ZGetGameClient()->GetPlayerUID(), GetItemUID(), nCount);
}

// ============= ZShopEquipItemHandle_SellMatch ÆÇ¸Å ÇÚµé·¯ : Àåºñ ================================================

ZShopEquipItemHandle_SellMatch::ZShopEquipItemHandle_SellMatch(ZShopEquipItem_Match* pItem)
{
	m_pItem = pItem;
	_ASSERT(m_pItem);
}

bool ZShopEquipItemHandle_SellMatch::GetPrice(int& out_nPrice)
{
	out_nPrice = 0;

	MMatchItemDesc* pDesc = m_pItem->GetDesc();
	if (!pDesc) { _ASSERT(pDesc); return false; }

	ZMyItemNode* pItemNode = ZGetMyInfo()->GetItemList()->GetItem(GetItemUID());
	if (!pItemNode) return false;

	if (!pDesc->IsCashItem() && !pDesc->IsEventItem())/// Jorklenis2: <IsEventItem>
	{
		// ±â°£Á¦ ¹Ù¿îÆ¼ ¾ÆÀÌÅÛÀÌ¸é ³²Àº ±â°£¿¡ »ó´ëÀûÀ¸·Î °¡°ÝÀ» °è»êÇÑ´Ù. 
		if (pItemNode->IsRentItem())
		{
			int nPrice = pDesc->m_nBountyPrice.Ref();
			int nRentHourPeriod = pItemNode->GetRentHourPeriod();
			int nRentMinutePeriodRemainder = pItemNode->GetRentMinutePeriodRemainder();

			out_nPrice = CalculateRentItemSellBounty(nPrice, nRentHourPeriod, nRentMinutePeriodRemainder / 60, pDesc->IsUnLimitItem());
		}
		else
		{
			out_nPrice = pDesc->GetSellBountyValue(1);
		}
		return true;
	}

	// Ä³½¬¾ÆÀÌÅÛÀÎ °æ¿ì
#ifndef _SELL_CASHITEM
	return false;
#else
	if (pDesc->IsSpendableItem())
	{
		out_nPrice = CalculateCashItemToBounty(CASHITEM_SPENDABLE);
	}
	else
	{
		int nResLevel = pDesc->m_nResLevel.Ref();
		int nRentMinuteRemainder = pItemNode->GetRentMinutePeriodRemainder();

		out_nPrice = CalculateCashItemToBounty(CASHITEM_NORMAL, nResLevel, nRentMinuteRemainder, pDesc->m_nSlot);
	}

	return true;
#endif
}

void ZShopEquipItemHandle_SellMatch::Sell()
{
	if (!m_pItem->GetDesc()) { _ASSERT(0); return; }

#ifndef _SELL_CASHITEM
	if (m_pItem->GetDesc()->IsCashItem()) { _ASSERT(0); return; }
#endif
	/// Jorklenis2: <IsEventItem>
#ifndef _SELL_EVENTITEM
	if (m_pItem->GetDesc()->IsEventItem()) { _ASSERT(0); return; }
#endif

	int nPrice = 0;
	if (!GetPrice(nPrice)) return;

	if (m_pItem->GetDesc()->IsSpendableItem())
	{
		ZMyItemNode* pMyItemNode = ZGetMyInfo()->GetItemList()->GetItem(GetItemUID());
		if (!pMyItemNode) { _ASSERT(0); return; }

		int nMax = pMyItemNode->GetItemCount();

		OpenCountableConfirmDlg(
			m_pItem->GetDesc()->m_pMItemName->Ref().m_szItemName,
			m_pItem->GetIcon(),
			nPrice,
			nMax, this);
	}
	else
	{
		if (!m_pItem->GetDesc()->IsCashItem())
			OpenSimpleConfirmDlg(this);
		else
		{
			OpenSellCashItemConfirmDlg(
				m_pItem->GetDesc()->m_pMItemName->Ref().m_szItemName,
				m_pItem->GetIcon(),
				nPrice,
				1, this);
		}
	}
	/// Jorklenis2: <IsEventItem>
	if (!m_pItem->GetDesc()->IsEventItem())
		OpenSimpleConfirmDlg(this);
	else
	{
		OpenSellCashItemConfirmDlg(
			m_pItem->GetDesc()->m_pMItemName->Ref().m_szItemName,
			m_pItem->GetIcon(),
			nPrice,
			1, this);
	}
}

void ZShopEquipItemHandle_SellMatch::OnDone(bool bOk)
{
	if (!bOk) return;
	ZPostRequestSellItem(ZGetGameClient()->GetPlayerUID(), GetItemUID());
}

void ZShopEquipItemHandle_SellMatch::OnDone(int nCount)
{
	ZPostRequestSellItem(ZGetGameClient()->GetPlayerUID(), GetItemUID(), nCount);
}

void ZShopEquipItemHandle_SellMatch::OnConfirmSellCashItem(bool bOk)
{
	if (!bOk) return;
	ZPostRequestSellItem(ZGetGameClient()->GetPlayerUID(), GetItemUID());
}

// ============= ZShopEquipItemHandle_SellQuest ÆÇ¸Å ÇÚµé·¯ : Äù½ºÆ® ================================================

ZShopEquipItemHandle_SellQuest::ZShopEquipItemHandle_SellQuest(ZShopEquipItem_Quest* pItem)
{
	m_pItem = pItem;
	_ASSERT(m_pItem);
}

bool ZShopEquipItemHandle_SellQuest::GetPrice(int& out_nPrice)
{
	out_nPrice = 0;

	MQuestItemDesc* pDesc = m_pItem->GetDesc();
	if (!pDesc) { _ASSERT(pDesc); return false; }

	out_nPrice = pDesc->GetSellBountyValue(1);
	return true;
}

void ZShopEquipItemHandle_SellQuest::Sell()
{
	if (!m_pItem->GetDesc()) { _ASSERT(0); return; }
	int nPrice = 0;
	if (!GetPrice(nPrice)) return;

	ZMyQuestItemNode* pMyQItem = ZGetMyInfo()->GetItemList()->GetQuestItemMap().Find(m_pItem->GetDesc()->m_nItemID);
	if (!pMyQItem) { _ASSERT(0); return; }

	int nMax = pMyQItem->GetCount();

	OpenCountableConfirmDlg(
		m_pItem->GetDesc()->m_szQuestItemName,
		m_pItem->GetIcon(),
		nPrice,
		nMax, this);
}

void ZShopEquipItemHandle_SellQuest::OnDone(int nCount)
{
	if (!m_pItem->GetDesc()) { _ASSERT(0); return; }

	ZPostRequestSellQuestItem(ZGetGameClient()->GetPlayerUID(), m_pItem->GetDesc()->m_nItemID, nCount);
}


// ============= IShopEquipItemHandle_SendAccount ÀºÇà º¸³»±â ÇÚµé·¯ ================================================

void IShopEquipItemHandle_SendAccount::OpenCountableConfirmDlg(const char* szItemName, MBitmap* pIconBmp, int nPrice, int nMax, IItemCountDlgDoneHandler* pDoneHandler)
{
	ZItemCountDlg* pDlg = ZGetGameInterface()->GetShopEquipInterface()->GetItemCountDlg();
	if (!pDlg) { _ASSERT(0); return; }
	pDlg->Open(ZICD_SENDACCOUNT, szItemName, pIconBmp, nPrice, 0, nMax, pDoneHandler);
}

// ============= ZShopEquipItemHandle_SendAccountGamble ÀºÇà º¸³»±â ÇÚµé·¯ : °·ºí ================================================

ZShopEquipItemHandle_SendAccountGamble::ZShopEquipItemHandle_SendAccountGamble(ZShopEquipItem_Gamble* pItem)
	: m_pItem(pItem)
{
	_ASSERT(pItem);
}

void ZShopEquipItemHandle_SendAccountGamble::Send()
{
	if (!m_pItem->GetDesc()) { _ASSERT(0); return; }
	if (!m_pItem->CanSendAccount()) return;


	const ZMyGambleItem* pMyGItem = ZGetMyInfo()->GetItemList()->GetGambleItem(GetItemUID());
	int nMax = pMyGItem->GetItemCount();

	OpenCountableConfirmDlg(
		m_pItem->GetDesc()->GetName().c_str(),
		m_pItem->GetIcon(),
		0,//todok Ä³½¬ °·ºí ¾ÆÀÌÅÛ ÆÇ¸Å °¡°Ý °è»ê °ø½ÄÀ» Àû¿ëÇÒ°Í
		nMax, this);
}

void ZShopEquipItemHandle_SendAccountGamble::OnDone(int nCount)
{
	if (!m_pItem->GetDesc()) { _ASSERT(0); return; }
	if (!m_pItem->CanSendAccount()) return;

	if (nCount <= 0) return;

	ZPostRequestSendAccountItem(ZGetMyUID(), GetItemUID(), nCount);
}

// ============= ZShopEquipItemHandle_SendAccountMatch ÀºÇà º¸³»±â ÇÚµé·¯ : Àåºñ ================================================

ZShopEquipItemHandle_SendAccountMatch::ZShopEquipItemHandle_SendAccountMatch(ZShopEquipItem_Match* pItem)
	: m_pItem(pItem)
{
	_ASSERT(pItem);
}

void ZShopEquipItemHandle_SendAccountMatch::Send()
{
	if (!m_pItem->GetDesc()) { _ASSERT(0); return; }
	if (!m_pItem->CanSendAccount()) { _ASSERT(0); return; }

	if (m_pItem->GetDesc()->IsSpendableItem())
	{
		ZMyItemNode* pMyItemNode = ZGetMyInfo()->GetItemList()->GetItem(GetItemUID());
		if (!pMyItemNode) { _ASSERT(0); return; }

		int nMax = pMyItemNode->GetItemCount();

		OpenCountableConfirmDlg(
			m_pItem->GetDesc()->m_pMItemName->Ref().m_szItemName,
			m_pItem->GetIcon(),
			0,
			nMax, this);
	}
	else
	{
		ZPostRequestSendAccountItem(ZGetMyUID(), GetItemUID());
	}
}

void ZShopEquipItemHandle_SendAccountMatch::OnDone(int nCount)
{
	if (!m_pItem->GetDesc()) { _ASSERT(0); return; }
	if (!m_pItem->CanSendAccount()) return;

	if (nCount <= 0) return;

	ZPostRequestSendAccountItem(ZGetMyUID(), GetItemUID(), nCount);
}


// ============= IShopEquipItemHandle_BringAccount ÀºÇà °¡Á®¿À±â ÇÚµé·¯ ================================================

void IShopEquipItemHandle_BringAccount::OpenSimpleConfirmDlg(ISimpleConfirmDlgDoneHandler* pDoneHandler)
{
	ZSimpleConfirmDlg* pDlg = ZGetGameInterface()->GetShopEquipInterface()->GetSimpleConfirmDlg();
	if (!pDlg) { _ASSERT(0); return; }
	pDlg->Open(ZMsg(MSG_SHOPEQUIP_BRING_CONFIRM), pDoneHandler);
}

void IShopEquipItemHandle_BringAccount::OpenCountableConfirmDlg(const char* szItemName, MBitmap* pIconBmp, int nPrice, int nMax, IItemCountDlgDoneHandler* pDoneHandler)
{
	ZItemCountDlg* pDlg = ZGetGameInterface()->GetShopEquipInterface()->GetItemCountDlg();
	if (!pDlg) { _ASSERT(0); return; }
	pDlg->Open(ZICD_BRINGACCOUNT, szItemName, pIconBmp, nPrice, 0, nMax, pDoneHandler);
}

// ============= ZShopEquipItemHandle_BringAccountGamble ÀºÇà °¡Á®¿À±â ÇÚµé·¯ : °·ºí ================================================

ZShopEquipItemHandle_BringAccountGamble::ZShopEquipItemHandle_BringAccountGamble(ZShopEquipItem_Gamble* pItem)
	: m_pItem(pItem)
{
	_ASSERT(pItem);
}

void ZShopEquipItemHandle_BringAccountGamble::Bring()
{
	if (!m_pItem->GetDesc()) { _ASSERT(0); return; }
	if (GetAIID() == 0) return;

	static unsigned long int st_LastRequestTime = 0;	///< ¸· ´©¸£´Â °Í ¹æÁö
	unsigned long int nNowTime = timeGetTime();
	if ((nNowTime - st_LastRequestTime) < 1000) return;

	st_LastRequestTime = nNowTime;

	if (!ZGetGameInterface()->GetShopEquipInterface()->CheckRestrictBringAccountItem())
		return;

	ZMyItemNode* pMyItemNode = ZGetMyInfo()->GetItemList()->GetAccountItemByAIID(GetAIID());
	if (!pMyItemNode) return;

	int nMax = pMyItemNode->GetItemCount();

	OpenCountableConfirmDlg(
		m_pItem->GetDesc()->GetName().c_str(),
		m_pItem->GetIcon(),
		0,//todok Ä³½¬ °·ºí ¾ÆÀÌÅÛ ÆÇ¸Å °¡°Ý °è»ê °ø½ÄÀ» Àû¿ëÇÒ°Í
		nMax, this);
}

void ZShopEquipItemHandle_BringAccountGamble::OnDone(int nCount)
{
	if (nCount <= 0) return;

	ZPostRequestBringAccountItem(ZGetGameClient()->GetPlayerUID(), GetAIID(), m_pItem->GetDesc()->GetGambleItemID(), nCount);
}

// ============= ZShopEquipItemHandle_BringAccountMatch ÀºÇà °¡Á®¿À±â ÇÚµé·¯ : Àåºñ ================================================

ZShopEquipItemHandle_BringAccountMatch::ZShopEquipItemHandle_BringAccountMatch(ZShopEquipItem_Match* pItem)
	: m_pItem(pItem)
{
	_ASSERT(pItem);
}

void ZShopEquipItemHandle_BringAccountMatch::Bring()
{
	if (!m_pItem->GetDesc()) { _ASSERT(0); return; }
	if (GetAIID() == 0) return;

	static unsigned long int st_LastRequestTime = 0;	///< ¸· ´©¸£´Â °Í ¹æÁö
	unsigned long int nNowTime = timeGetTime();
	if ((nNowTime - st_LastRequestTime) < 1000) return;

	st_LastRequestTime = nNowTime;

	if (!ZGetGameInterface()->GetShopEquipInterface()->CheckRestrictBringAccountItem())
		return;

	if (m_pItem->GetDesc()->IsSpendableItem())
	{
		ZMyItemNode* pMyItemNode = ZGetMyInfo()->GetItemList()->GetAccountItemByAIID(GetAIID());
		if (!pMyItemNode) return;

		int nMax = pMyItemNode->GetItemCount();

		OpenCountableConfirmDlg(
			m_pItem->GetDesc()->m_pMItemName->Ref().m_szItemName,
			m_pItem->GetIcon(),
			0,
			nMax, this);
	}
	else
	{
		OpenSimpleConfirmDlg(this);
	}
}

void ZShopEquipItemHandle_BringAccountMatch::OnDone(bool bOk)
{
	if (!bOk) return;

	ZPostRequestBringAccountItem(ZGetGameClient()->GetPlayerUID(), GetAIID(), m_pItem->GetDesc()->m_nID, 1);
}

void ZShopEquipItemHandle_BringAccountMatch::OnDone(int nCount)
{
	if (nCount <= 0) return;

	ZPostRequestBringAccountItem(ZGetGameClient()->GetPlayerUID(), GetAIID(), m_pItem->GetDesc()->m_nID, nCount);
}
