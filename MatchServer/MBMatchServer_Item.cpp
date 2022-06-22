#include "stdafx.h"
#include "MMatchServer.h"
#include "MSharedCommandTable.h"
#include "MErrorTable.h"
#include "MBlobArray.h"
#include "MObject.h"
#include "MMatchObject.h"
#include "MMatchItem.h"
#include "MAgentObject.h"
#include "MMatchNotify.h"
#include "Msg.h"
#include "MMatchObjCache.h"
#include "MMatchStage.h"
#include "MMatchTransDataType.h"
#include "MMatchFormula.h"
#include "MMatchConfig.h"
#include "MCommandCommunicator.h"
#include "MMatchShop.h"
#include "MMatchTransDataType.h"
#include "MDebug.h"	
#include "MMatchAuth.h"
#include "MMatchStatus.h"
#include "MAsyncDBJob.h"
#include "MMatchDBGambleItem.h"
#include "MMatchGambleMachine.h"
#include "MBMatchServer.h"
#include "MMatchUtil.h"
#include "MMatchItemFunction.h"
#include "MBMatchAsyncDBJob_BuyBountyItem.h"
#include "MBMatchAsyncDBJob_SellItem.h"
#include "MBMatchAsyncDBJob_GambleItem.h"
#include "MBMatchAsyncDBJob_GambleQuestItem.h"
#include "MBMatchAsyncDBJob_BringAccountItem.h"
#include "MBMatchAsyncDBJob_BringAccountItemStackable.h"
#include "MBMatchAsyncDBJob_BringBackAccountItem.h"
#include "MBMatchAsyncDBJob_BringBackAccountItemStackable.h"
#include "MBMatchAsyncDBJob_UpdateEquipItem.h"
#include "MBMatchAsyncDBJob_TakeoffItem.h"




void MBMatchServer::OnRequestShopItemList(const MUID& uidPlayer, const int nFirstItemIndex, const int nItemCount)
{
	ResponseShopItemList(uidPlayer, nFirstItemIndex, nItemCount);
}

void MBMatchServer::ResponseShopItemList(const MUID& uidPlayer, const int nFirstItemIndex, const int nItemCount)
{
	MMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return;
	if (NULL == pObj->GetCharInfo()) return;

	SendGambleItemList(pObj);

	MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_SHOP_ITEMLIST, uidPlayer);

	// Ä¿¸ÇµåÀÇ ÆÄ¶ó¸ÞÅÍ·Î ºí·°À» Ãß°¡ÇÑ´Ù. //
	pNew->AddParameter(MakeShopGambleItemListBlob());
	pNew->AddParameter(MakeShopItemListBlob(nFirstItemIndex, nItemCount));

	PostSafeQueue(pNew);
}

void MBMatchServer::OnRequestGamble(const MUID& uidPlayer, const MUID& uidItem)
{
	// ÀÚ½ÅÀÇ ÀÎº¥Åä¸®¿¡ Á¸ÀçÇÏ´Â ¾ÆÀÌÅÛÀÎÁö È®ÀÎÇÑ´Ù. //
	MMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) { return; }

	MMatchCharInfo* pCharInfo = pObj->GetCharInfo();
	if (pCharInfo == NULL) { return; }

	// À¯Àú°¡ °¡Áö°í ÀÖ´Â ¾ÆÀÌÅÛ °íÀ¯ ¾ÆÀÌµð·Î ÇØ´ç ¾ÆÀÌÅÛ Ã£±â
	const MMatchCharGambleItem* pGambleItem = pCharInfo->m_GambleItemManager.GetGambleItemByUID(uidItem);
	if (pGambleItem == NULL) {
		mlog("GetGambleItem() failed! [CID : %d]\n", pCharInfo->m_nCID);
		return;
	}

	//if( pGambleItem->GetGambleItemID() >= 1000100)  // quest item À¸·Î ¹Ù²î´Â gamble
	// ÀÌ¹ø¿¡ Ä³½Ã¿ë °×ºí¾ÆÀÌÅÛ Ãß°¡·Î ¼öÁ¤
	// ±âÁ¸ ÀÏº» ¿äÃ»À¸·Î Äù½ºÆ®¿ë °×ºí¾ÆÀÌÅÛÀ» Ãß°¡Çß¾úÀ½ (1000100~1000199»çÀÌ)
	// Äù½ºÆ®¿ë °×ºí¾ÆÀÌÅÛÀº 1000100¿¡¼­ 1000200·Î Á¦ÇÑÇÑ´Ù
	if (1000100 <= pGambleItem->GetGambleItemID() && pGambleItem->GetGambleItemID() < 1000200)	// quest item À¸·Î ¹Ù²î´Â gamble
	{
		// °×ºí ·£´ýÇÔ¼ö¸¦ È£ÃâÇÏ¿© °á°ú ¾ÆÀÌÅÛÀÇ ID¸¦ °¡Á®¿Â´Ù. //
		const MMatchGambleRewardItem* pGambleRewardItem = GetGambleMachine().Gamble(pGambleItem->GetGambleItemID());
		if (pGambleRewardItem == NULL) {
			mlog("Gamble() failed! [ItemID : %d]\n", pGambleItem->GetGambleItemID());
			ASSERT(0);
			return;
		}

		DWORD dwRIID;
		if (MMS_MALE == pCharInfo->m_nSex)			dwRIID = pGambleRewardItem->GetItemIDMale();
		else if (MMS_FEMALE == pCharInfo->m_nSex)	dwRIID = pGambleRewardItem->GetItemIDFemale();

		MBMatchAsyncDBJob_GambleQuestItem *pJob = new MBMatchAsyncDBJob_GambleQuestItem(uidPlayer);
		pJob->Input(uidPlayer, (MUID&)pGambleItem->GetUID(), pCharInfo->m_nCID, pGambleItem->GetCIID(), pGambleRewardItem->GetGambleRewardItemID(), dwRIID);

		pObj->m_DBJobQ.DBJobQ.push_back(pJob);
	}
	else
	{
		// °×ºí ·£´ýÇÔ¼ö¸¦ È£ÃâÇÏ¿© °á°ú ¾ÆÀÌÅÛÀÇ ID¸¦ °¡Á®¿Â´Ù. //
		const MMatchGambleRewardItem* pGRewardItem = GetGambleMachine().Gamble(pGambleItem->GetGambleItemID());
		if (pGRewardItem == NULL) {
			mlog("Gamble() failed! [ItemID : %d]\n", pGambleItem->GetGambleItemID());
			ASSERT(0);
			return;
		}

		DWORD dwRIID;
		DWORD dwItemCnt;

		if (MMS_MALE == pCharInfo->m_nSex)			dwRIID = pGRewardItem->GetItemIDMale();
		else if (MMS_FEMALE == pCharInfo->m_nSex)	dwRIID = pGRewardItem->GetItemIDFemale();

		dwItemCnt = pGRewardItem->GetItemCnt();

		bool bIsSpendable = false;

		MMatchItemDesc* pDesc = MGetMatchItemDescMgr()->GetItemDesc(dwRIID);
		if (pDesc != NULL) {
			bIsSpendable = pDesc->IsSpendableItem();

			if (!bIsSpendable && dwItemCnt > 1) {
				_ASSERT(0);
				dwItemCnt = 1;
			}
		}
		else {
			if (GetGambleMachine().GetGambleItemByGambleItemID(dwRIID) != NULL) {
				bIsSpendable = true;
			}
			else {
				_ASSERT(0);
				return;
			}
		}

		MBMatchAsyncDBJob_GambleItem* pJob = new MBMatchAsyncDBJob_GambleItem(uidPlayer);
		pJob->Input(uidPlayer, pGambleItem->GetUID(), pCharInfo->m_nCID, pGambleItem->GetCIID(), pGambleItem->GetGambleItemID(),
			pGRewardItem->GetGambleRewardItemID(), dwRIID, pGRewardItem->GetItemCnt(), pGRewardItem->GetRentHourPeriod(), pDesc->IsSpendableItem());

		pObj->m_DBJobQ.DBJobQ.push_back(pJob);
	}
}

MCommandParameterBlob* MBMatchServer::MakeShopGambleItemListBlob() const
{
	vector<DWORD>			GambleItemIndexVec;
	MTD_GambleItemNode*		pSendGmableIItem;

	// °×ºí ¾ÆÀÌÅÛ ¸®½ºÆ®¸¦ ´ã¾Æ¼­ º¸³½´Ù.
	const MMatchGambleMachine& GambleMachine = GetGambleMachine();

	// °×ºí ¾ÆÀÌÅÛÀÇ »ç¿ë ½Ã°£À» °Ë»çÇØ¼­ º¸³»¾ß ÇÏ´Â °×ºí ¾ÆÀÌÅÛÀ» ¹éÅÍ¿¡ ´ã¾Æ¿Â´Ù. //
	GambleMachine.GetItemVectorByCheckedItemTime(GambleItemIndexVec, GetGlobalClockCount());

	const DWORD dwGambleItemSize = (DWORD)GambleItemIndexVec.size();

	// ¾ÆÀÌÅÛÀÇ °³¼ö¸¸Å­ ÇÊ¿äÇÑ ¸Þ¸ð¸® ºí·°À» ÇÒ´çÇÑ´Ù. //
	void* pGambleItemArray = MMakeBlobArray(sizeof(MTD_GambleItemNode), dwGambleItemSize);

	for (DWORD j = 0; j < dwGambleItemSize; ++j)
	{
		// º¤ÅÍ¿¡ ´ã±ä °×ºí ¾ÆÀÌÅÛÀÇ ÀÎµ¦½º·Î º¸³»¾ß ÇÏ´Â °×ºí ¾ÆÀÌÅÛÀ» Ã£¾Æ¿Â´Ù. //
		const MMatchGambleItem* pGambleItem = GambleMachine.GetGambleItemByIndex(GambleItemIndexVec[j]);

		// º¸³»¾ß ÇÒ °×ºí ¾ÆÀÌÅÛ ´ã±æ ³»¿ë ¿øÇÏ´Â ¸Þ¸ð¸® ºí·ÏÀ» Ã£¾Æ¼­ ±â·ÏÇÑ´Ù. //
		pSendGmableIItem = (MTD_GambleItemNode*)MGetBlobArrayElement(pGambleItemArray, j);
		if (pSendGmableIItem != NULL)
		{
			pSendGmableIItem->nItemID = pGambleItem->GetGambleItemID();
			pSendGmableIItem->nItemCnt = 1;
		}
		else
		{
			ASSERT(0);
		}
	}

	MCommandParameterBlob* pGItemBlobParam = new MCommandParameterBlob(pGambleItemArray, MGetBlobArraySize(pGambleItemArray));
	MEraseBlobArray(pGambleItemArray);

	return pGItemBlobParam;
}


MCommandParameterBlob* MBMatchServer::MakeShopItemListBlob(const int nFirstItemIndex, const int nItemCount) const
{
	int nRealItemCount = nItemCount;

	if ((nItemCount <= 0) || (nItemCount > MGetMatchShop()->GetCount())) {
		nRealItemCount = MGetMatchShop()->GetCount();
	}

	void* pItemArray = MMakeBlobArray(sizeof(MTD_ShopItemInfo), nRealItemCount);
	int nIndex = 0;

	for (int i = nFirstItemIndex; i < nFirstItemIndex + nRealItemCount; i++)
	{
		MTD_ShopItemInfo *pInfo = (MTD_ShopItemInfo *)MGetBlobArrayElement(pItemArray, nIndex++);
		ShopItemNode* pSINode = MGetMatchShop()->GetSellItemByIndex(i);

		if (pSINode != NULL) {
			pInfo->nItemID = pSINode->nItemID;
			pInfo->nItemCount = pSINode->nItemCount;
		}
		else {
			pInfo->nItemID = 0;
			pInfo->nItemCount = 0;

			ASSERT(0);
		}

	}

	MCommandParameterBlob* pItemListBlob = new MCommandParameterBlob(pItemArray, MGetBlobArraySize(pItemArray));
	MEraseBlobArray(pItemArray);

	return pItemListBlob;
}


void MBMatchServer::OnRequestBuyItem(const MUID& uidPlayer, const unsigned long int nItemID, const int nItemCount)
{
	MMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return;

	MMatchCharInfo* pCharInfo = pObj->GetCharInfo();
	if (NULL == pCharInfo) return;

	DWORD	dwPrice;
	WORD	nRentHourPeriod = RENT_PERIOD_UNLIMITED;
	bool	bIsGambleItem = false;
	bool	bIsSpendableItem = false;
	int		nBuyMode = 0; /// Jorklenis2: <nBuyMode>

	// Shop¿¡¼­ ÆÇ¸ÅÇÏ´ÂÁö ¸ÕÀú °Ë»çÇÏ°í GambleItemÀÎÁö ÇÑ¹ø´õ °Ë»ç ÇÑ´Ù.
	if (MGetMatchShop()->IsSellItem(nItemID))
	{
		MMatchItemDesc* pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(nItemID);
		if (pItemDesc == NULL) return;

		if (!pItemDesc->IsUnLimitItem())
		{
			nRentHourPeriod = pItemDesc->m_nMaxRentPeriod.Ref() * 24;	// ½Ã°£´ÜÀ§·Î º¯È¯(DB ÀúÀå¿ë)
		}

		if (pItemDesc->IsSpendableItem())
		{
			bIsSpendableItem = true;
		}

		/// Jorklenis2: <nBuyMode 1>
		if (pItemDesc->IsCashItem())
		{
			nBuyMode = 1;
		}
		/// Jorklenis2: <nBuyMode 2>
		if (pItemDesc->IsEventItem())
		{
			nBuyMode = 2;
		}

		dwPrice = pItemDesc->m_nBountyPrice.Ref() * nItemCount;
	}
	else
	{
		if (GetGambleMachine().IsItTimeoverEventGambleItem(nItemID, GetGlobalClockCount()))
		{
			MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_BUY_ITEM, pObj->GetUID());
			pNew->AddParameter(new MCmdParamInt(MERR_CANNOT_BUY_ITEM));
			PostSafeQueue(pNew);
			return;
		}

		const MMatchGambleItem* pGItem = GetGambleMachine().GetGambleItemByGambleItemID(nItemID);
		if (NULL == pGItem) return;

		if (pGItem->IsCash()) {
			MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_BUY_ITEM, pObj->GetUID());
			pNew->AddParameter(new MCmdParamInt(MERR_CANNOT_BUY_ITEM));
			PostSafeQueue(pNew);

			return;
		}

		dwPrice = pGItem->GetPrice() * nItemCount;
		bIsGambleItem = true;
		bIsSpendableItem = true;
	}

	if (!CheckUserCanBuyItem(pObj, nItemID, nItemCount, dwPrice, nBuyMode)) /// Jorklenis2: <nBuyMode>
	{
		return;
	}

	ResponseBuyItem(pObj, nItemID, nItemCount, bIsGambleItem, dwPrice, nRentHourPeriod, bIsSpendableItem, nBuyMode); /// Jorklenis2: <nBuyMode>
}

bool MBMatchServer::ResponseBuyItem(MMatchObject* pObj, const unsigned long int nItemID, const int nItemCount
	, const bool bIsGambleItem, const DWORD dwPrice, const WORD wRentHourPeriod, const bool bIsSpendableItem, int nBuyMode) /// Jorklenis2: <nBuyMode>
{
	MMatchCharInfo* pCharInfo = pObj->GetCharInfo();

	/// Jorklenis2: <Añadido GetAccountInfo>
	MMatchAccountInfo* pAccountInfo = pObj->GetAccountInfo();

	MBMatchAsyncDBJob_BuyBountyItem* pBuyBountyItemJob = new MBMatchAsyncDBJob_BuyBountyItem(pObj->GetUID());
	if (NULL == pBuyBountyItemJob) { return false; }

	pBuyBountyItemJob->Input(pObj->GetUID(), pCharInfo->m_nCID, pAccountInfo->m_nAID, nItemID
		, nItemCount, dwPrice, wRentHourPeriod, bIsGambleItem, bIsSpendableItem, nBuyMode); /// Jorklenis2: <nBuyMode>

	pObj->m_DBJobQ.DBJobQ.push_back(pBuyBountyItemJob);

	return true;
}

// TodoH(ÇÏ) - ÃßÈÄ¿¡ ´Ù½Ã Ã¼Å©Ã¼Å©!
const bool MBMatchServer::CheckUserCanBuyItem(MMatchObject* pObj, const int nItemID, const int nItemCnt, const DWORD dwPrice, int nBuyMode) /// Jorklenis2: <nBuyMode>
{
	if (NULL == pObj) return false;

	MMatchCharInfo* pCharInfo = pObj->GetCharInfo();

	/// Jorklenis2: <Añadido GetAccountInfo>
	MMatchAccountInfo* pAccountInfo = pObj->GetAccountInfo();

	if (NULL == pCharInfo) return false;

	/// Jorklenis2: <Añadido pAccountInfo>
	if (NULL == pAccountInfo) return false;

	/// Jorklenis2: <nBuyMode 1>
	bool bIsCashItem = (nBuyMode == 1);

	/// Jorklenis2: <nBuyMode 2>
	bool bIsEventItem = (nBuyMode == 2);

	MMatchItem* pItem = pCharInfo->m_ItemList.GetItemByItemID(nItemID);
	if (pItem != NULL)
	{
		MMatchItemDesc* pDesc = MGetMatchItemDescMgr()->GetItemDesc(nItemID);
		if (pDesc && pDesc->IsSpendableItem())
		{
			if (pItem->GetItemCount() + nItemCnt > MAX_SPENDABLE_ITEM_COUNT)
			{
				MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_BUY_ITEM, pObj->GetUID());
				pNew->AddParameter(new MCmdParamInt(MERR_TOO_MANY_ITEM));
				PostSafeQueue(pNew);

				return false;
			}
		}
	}
	else
	{
		const MMatchCharGambleItem* pGambleItem = pCharInfo->m_GambleItemManager.GetGambleItemByItemID(nItemID);
		if (pGambleItem != NULL)
		{
			if (pGambleItem->GetItemCount() + nItemCnt > MAX_SPENDABLE_ITEM_COUNT)
			{
				MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_BUY_ITEM, pObj->GetUID());
				pNew->AddParameter(new MCmdParamInt(MERR_TOO_MANY_ITEM));
				PostSafeQueue(pNew);

				return false;
			}
		}
	}

	const DWORD dwTotalItemCount = pCharInfo->m_ItemList.GetCount() + pCharInfo->m_GambleItemManager.GetCount();
	if (dwTotalItemCount >= MAX_ITEM_COUNT)
	{
		MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_BUY_ITEM, pObj->GetUID());
		pNew->AddParameter(new MCmdParamInt(MERR_TOO_MANY_ITEM));
		PostSafeQueue(pNew);

		return false;
	}

	if (static_cast<DWORD>(pCharInfo->m_nBP) < dwPrice)
	{
		MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_BUY_ITEM, pObj->GetUID());
		pNew->AddParameter(new MCmdParamInt(MERR_TOO_EXPENSIVE_BOUNTY));
		PostSafeQueue(pNew);

		return false;
	}

	/// Jorklenis2: <Comprar artículos de cash>
	else if ((bIsCashItem) && (static_cast<DWORD>(pAccountInfo->m_nCoins) < dwPrice))
	{
		MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_BUY_ITEM, pObj->GetUID());
		pNew->AddParameter(new MCmdParamInt(MERR_TOO_EXPENSIVE_CASH));
		PostSafeQueue(pNew);

		return false;
	}
	/// Jorklenis2: <Comprar artículos de event>
	else if ((bIsEventItem) && (static_cast<DWORD>(pAccountInfo->m_nECoins) < dwPrice))
	{
		MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_BUY_ITEM, pObj->GetUID());
		pNew->AddParameter(new MCmdParamInt(MERR_TOO_EXPENSIVE_EVENT));
		PostSafeQueue(pNew);

		return false;
	}

	return true;
}

bool MBMatchServer::CheckUserCanDistributeRewardItem(MMatchObject* pObj)
{
	if (NULL == pObj) return false;

	MMatchCharInfo* pCharInfo = pObj->GetCharInfo();
	if (NULL == pCharInfo) return false;

	const DWORD dwTotalItemCount = pCharInfo->m_ItemList.GetCount() + pCharInfo->m_GambleItemManager.GetCount();
	if (dwTotalItemCount >= MAX_QUEST_REWARD_ITEM_COUNT)
	{
		MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_BUY_ITEM, pObj->GetUID());
		pNew->AddParameter(new MCmdParamInt(MERR_TOO_MANY_ITEM));
		PostSafeQueue(pNew);

		return false;
	}

	return true;
}

void MBMatchServer::OnRequestCharacterItemList(const MUID& uidPlayer)
{
	MMatchObject* pObj = GetObject(uidPlayer);
	if (NULL == pObj)
		return;

	if (NULL == pObj->GetCharInfo())
		return;

	if (pObj->GetCharInfo()->m_IsSendMyItemListByRequestClient)
		return;

	if (ResponseCharacterItemList(uidPlayer))
		pObj->GetCharInfo()->m_IsSendMyItemListByRequestClient = true;
}

void MBMatchServer::OnRequestCharacterItemListForce(const MUID& uidPlayer)
{
	MMatchObject* pObj = GetObject(uidPlayer);
	if (NULL == pObj)
		return;

	if (NULL == pObj->GetCharInfo())
		return;

	if (ResponseCharacterItemList(uidPlayer))
		pObj->GetCharInfo()->m_IsSendMyItemListByRequestClient = true;
}


bool MBMatchServer::ResponseCharacterItemList(const MUID& uidPlayer)
{
	MMatchObject* pObj = GetObject(uidPlayer);
	if ((pObj == NULL) || (pObj->GetCharInfo() == NULL))
	{
		mlog("ResponseCharacterItemList > pObj or pObj->GetCharInfo() IS NULL\n");
		return false;
	}

	// ¸¸¾à DB°¡ ¾÷µ¥ÀÌÆ® µÇ¾ú´Ù¸é »óÁ¡À» ´Ù½Ã ±¸¼ºÇÑ´Ù.
	// ¿ë·®Àº Å©Áö¸¸ ÀÚÁÖ ÀÏ¾î³ª´Â ÀÏÀÌ ¾Æ´Ï´Ù. - by SungE 2007-06-28
	if (SendGambleItemList(pObj)) {
		ResponseShopItemList(uidPlayer, 0, 0);
	}

	// ÀÌÀü¿¡ µðºñ ¾ï¼¼½º¸¦ ¾ÈÇß¾úÀ¸¸é µðºñ¿¡¼­ ¾ÆÀÌÅÛ Á¤º¸¸¦ °¡Á®¿Â´Ù
	if (!pObj->GetCharInfo()->m_ItemList.IsDoneDbAccess())
	{
		mlog("start DB Query(ResponseCharacterItemList > GetCharItemInfo). \n");
		if (!m_MatchDBMgr.GetCharItemInfo(pObj->GetCharInfo()))
		{
			mlog("DB Query(ResponseCharacterItemList > GetCharItemInfo) Failed\n");
			return false;
		}
	}

	if (MSM_CLAN == MGetServerConfig()->GetServerMode())
	{
		if (!pObj->GetCharInfo()->m_QuestItemList.IsDoneDbAccess())
		{
			if (!m_MatchDBMgr.GetCharQuestItemInfo(pObj->GetCharInfo()))
			{
				mlog("ResponseCharacterItemList - GetCharQuestItemInfo fail.\n");
				return false;
			}
		}
	}

	MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_CHARACTER_ITEMLIST, uidPlayer);

	// ¹Ù¿îÆ¼ Àü¼Û
	pNew->AddParameter(new MCommandParameterInt(pObj->GetCharInfo()->m_nBP));
	/// Jorklenis2: <Cash and ECoins>
	pNew->AddParameter(new MCommandParameterInt(pObj->GetAccountInfo()->m_nCoins));
	pNew->AddParameter(new MCommandParameterInt(pObj->GetAccountInfo()->m_nECoins));

	// ÀåºñÇÑ ¾ÆÀÌÅÛ Àü¼Û
	int nIndex = 0;
	int nRealEquipedItemCount = 0;
	void* pEquipItemArray = MMakeBlobArray(sizeof(MUID), MMCIP_END);

	for (int i = 0; i < MMCIP_END; i++) {
		MUID* pUID = (MUID*)MGetBlobArrayElement(pEquipItemArray, nIndex++);

		if (!pObj->GetCharInfo()->m_EquipedItem.IsEmpty(MMatchCharItemParts(i))) {
			MMatchItem* pItem = pObj->GetCharInfo()->m_EquipedItem.GetItem(MMatchCharItemParts(i));
			if (pItem != NULL) {
				*pUID = pItem->GetUID();
				nRealEquipedItemCount++;
			}
			else {
				mlog("Invalid Equiped Item - part(%d), CID(%d)\n", i, pObj->GetCharInfo()->m_nCID);
				*pUID = MUID(0, 0);
			}
		}
		else {
			*pUID = MUID(0, 0);
		}
	}

	pNew->AddParameter(new MCommandParameterBlob(pEquipItemArray, MGetBlobArraySize(pEquipItemArray)));
	MEraseBlobArray(pEquipItemArray);


	// °®°í ÀÖ´Â ¾ÆÀÌÅÛ ¸®½ºÆ® Àü¼Û
	int nItemTotalCount = pObj->GetCharInfo()->m_ItemList.GetCount();

	void*			pItemArray = MMakeBlobArray(sizeof(MTD_ItemNode), nItemTotalCount);
	MMatchItemMap*	pItemList = &pObj->GetCharInfo()->m_ItemList;
	MMatchItem*		pItem = NULL;
	MTD_ItemNode*	pItemNode = NULL;
	int				nItemCount = 0;
	unsigned long	nPassTime = 0;
	int				nPassMinuteTime = 0;
	int				iMaxUseHour = 0;
	int				nRentMinutePeriodRemainder = RENT_MINUTE_PERIOD_UNLIMITED;

	nIndex = 0;
	for (MMatchItemMap::iterator itor = pItemList->begin(); itor != pItemList->end(); ++itor) {
		pItem = (*itor).second;

		if (pItem == NULL) {
			MUID id = (*itor).first;
			LOG(LOG_FILE, "there is unkown error in itemlist. Add:%d Count:%d, No:%d, UID:%d|%d\n"
				, pItemList, pItemList->GetCount(), nIndex, id.High, id.Low);
			continue;
		}

		pItemNode = (MTD_ItemNode*)MGetBlobArrayElement(pItemArray, nIndex++);
		nPassTime = MGetTimeDistance(pItem->GetRentItemRegTime(), GetTickTime());
		nPassMinuteTime = nPassTime / (1000 * 60);
		iMaxUseHour = 0;		// ÃÖ´ë »ç¿ë½Ã°£ º¸³»±â(ÆÇ¸Å°¡°Ý °è»êÀ» À§ÇØ)
		nRentMinutePeriodRemainder = RENT_MINUTE_PERIOD_UNLIMITED;
		nItemCount = pItem->GetItemCount();

		if (nItemCount <= 0) { continue; }

		if (pItem->IsRentItem()) {
			nRentMinutePeriodRemainder = pItem->GetRentMinutePeriodRemainder() - nPassMinuteTime;
			iMaxUseHour = pItem->GetRentHourPeriod();
		}

		Make_MTDItemNode(pItemNode
			, pItem->GetUID()
			, pItem->GetDescID()
			, nRentMinutePeriodRemainder
			, iMaxUseHour
			, nItemCount);
	}

	pNew->AddParameter(new MCommandParameterBlob(pItemArray, MGetBlobArraySize(pItemArray)));
	MEraseBlobArray(pItemArray);


	// °¡Áö°í ÀÖ´Â °×ºí ¾ÆÀÌÅÛ ¸®½ºÆ® Àü¼Û
	int nGambleItemCount = pObj->GetCharInfo()->m_GambleItemManager.GetCount();
	void* pGambleItemArray = MMakeBlobArray(sizeof(MTD_GambleItemNode), nGambleItemCount);

	for (int j = 0; j < nGambleItemCount; ++j) {
		const MMatchCharGambleItem* pGambleItem = pObj->GetCharInfo()->m_GambleItemManager.GetGambleItemByIndex(j);
		MTD_GambleItemNode* pSendGambleItem = (MTD_GambleItemNode*)MGetBlobArrayElement(pGambleItemArray, j);
		if (pSendGambleItem != NULL) {
			pSendGambleItem->uidItem = pGambleItem->GetUID();
			pSendGambleItem->nItemID = pGambleItem->GetGambleItemID();
			pSendGambleItem->nItemCnt = pGambleItem->GetItemCount();
		}
		else {
			ASSERT(0);
		}
	}

	pNew->AddParameter(new MCommandParameterBlob(pGambleItemArray, MGetBlobArraySize(pGambleItemArray)));
	MEraseBlobArray(pGambleItemArray);

	PostSafeQueue(pNew);

	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sell Item °ü·ÃÇÏ¿©...

void MBMatchServer::OnRequestSellItem(const MUID& uidPlayer, const MUID& uidItem, int nItemCount)
{
	MMatchObject* pObj = GetObject(uidPlayer);
	if (NULL == pObj) return;

	MMatchCharInfo* pCharInfo = pObj->GetCharInfo();
	if (NULL == pCharInfo) return;


	if (NULL != pCharInfo->m_ItemList.GetItem(uidItem))
	{
		MMatchItem* pItem = pCharInfo->m_ItemList.GetItem(uidItem);
		if (pItem == NULL) return;
		if (pItem->GetDesc() == NULL) return;

		if (!pItem->GetDesc()->IsSpendableItem()) 	ResponseSellItem(uidPlayer, uidItem);
		else										ResponseSellSpendableItem(uidPlayer, uidItem, nItemCount);
	}
	else if (NULL != pCharInfo->m_GambleItemManager.GetGambleItemByUID(uidItem))
	{
		ResponseSellGItem(uidPlayer, uidItem, nItemCount);	///< Gamble Item
	}

	return;
}

bool MBMatchServer::IsValidSellItem(const MUID& uidPlayer, const MUID& uidItem, int nItemCount)
{
	MMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return false;

	MMatchCharInfo* pCharInfo = pObj->GetCharInfo();
	if (pCharInfo == NULL) return false;

	MMatchItem* pItem = pObj->GetCharInfo()->m_ItemList.GetItem(uidItem);

	if ((pItem == NULL) || (pItem->GetDesc() == NULL))
	{	///< ¾ÆÀÌÅÛÀÌ Á¸ÀçÇÏÁö ¾ÊÀ»¶§
		MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_SELL_ITEM, MUID(0, 0));
		pNew->AddParameter(new MCmdParamInt(MERR_CANNOT_SELL_NONE_ITEM));
		RouteToListener(pObj, pNew);
		return false;
	}

	if (pItem->IsEquiped())
	{	///< ÀåºñÇÏ°í ÀÖÀ¸¸é ÆÈ ¼ö ¾ø´Ù.
		MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_SELL_ITEM, MUID(0, 0));
		pNew->AddParameter(new MCmdParamInt(MERR_CANNOT_SELL_EQUIPED_ITEM));
		RouteToListener(pObj, pNew);
		return false;
	}


	if (!pItem->GetDesc()->IsSpendableItem())
	{
		if (nItemCount > 1)
		{
			MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_SELL_ITEM, MUID(0, 0));
			pNew->AddParameter(new MCmdParamInt(MERR_CANNOT_SELL_NOT_SPENDABLEITEM));
			RouteToListener(pObj, pNew);

			return false;
		}
#ifndef _SELL_CASHITEM
		if (pItem->GetDesc()->IsCashItem() && pItem->GetDesc()->IsEventItem())
		{
			MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_SELL_ITEM, pObj->GetUID());
			pNew->AddParameter(new MCmdParamInt(MERR_CANNOT_SELL_CASHITEM));
			PostSafeQueue(pNew);
			return false;
		}
#endif
	}
	else
	{
		if (pItem->GetItemCount() < nItemCount)
		{	///< ÆÈ°íÀÚ ÇÏ´Â ¾ÆÀÌÅÛÀÇ °³¼ö°¡ ¼ÒÁöÇÏ°í ÀÖ´Â °³¼öº¸´Ù ¸¹À» °æ¿ì, ÆÈ ¼ö ¾ø´Ù.
			MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_SELL_ITEM, MUID(0, 0));
			pNew->AddParameter(new MCmdParamInt(MERR_CANNOT_SELL_TOO_MANY_ITEMCOUNT));
			RouteToListener(pObj, pNew);

			return false;
		}

		if (pItem->GetDesc()->IsCashItem() && pItem->GetDesc()->IsEventItem())
		{	///< ¼Ò¸ð¼º ¾ÆÀÌÅÛÀº Ä³½¬ÅÛÀ¸·Î ÆÈ ¼ö°¡ ¾ø´Ù.
			MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_SELL_ITEM, pObj->GetUID());
			pNew->AddParameter(new MCmdParamInt(MERR_CANNOT_SELL_CASHITEM));
			PostSafeQueue(pNew);
			return false;
		}
	}

	return true;
}

bool MBMatchServer::ResponseSellItem(const MUID& uidPlayer, const MUID& uidItem)
{
	MMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return false;

	MMatchCharInfo* pCharInfo = pObj->GetCharInfo();
	if (pCharInfo == NULL) return false;

	if (IsValidSellItem(uidPlayer, uidItem) == false) return false;

	/////////////////////////////////////////////////////////////////////////////////////////////
	UpdateCharDBCachingData(pObj);		///< XP, BP, KillCount, DeathCount Ä³½³ ¾÷µ¥ÀÌÆ®
	/////////////////////////////////////////////////////////////////////////////////////////////

	MMatchItem* pItem = pObj->GetCharInfo()->m_ItemList.GetItem(uidItem);
	if (pItem == NULL || pItem->GetDesc() == NULL) return false;
	if (!pItem->GetDesc()->IsCashItem() && !pItem->GetDesc()->IsEventItem())
	{
		int nPrice;

		if (!pItem->IsRentItem())
		{	///< ¹«Á¦ÇÑ ¹Ù¿îÆ¼ ¾ÆÀÌÅÛ
			nPrice = pItem->GetDesc()->GetSellBountyValue();
		}
		else
		{	///< ±â°£Á¦ ¹Ù¿îÆ¼ ¾ÆÀÌÅÛ
			unsigned long	nPassTime = MGetTimeDistance(pItem->GetRentItemRegTime(), GetTickTime()); // 1/1000ÃÊ.
			int				nPassMinuteTime = nPassTime / (60000);											// Áö³ª°£ ½Ã°£À» ºÐ´ÜÀ§·Î º¯°æ.

			nPrice = CalculateRentItemSellBounty(pItem->GetDesc()->m_nBountyPrice.Ref(),
				pItem->GetRentHourPeriod(),
				(pItem->GetRentMinutePeriodRemainder() - nPassMinuteTime) / 60,
				RENT_PERIOD_UNLIMITED == pItem->GetDesc()->m_nMaxRentPeriod.Ref());

		}

		MBMatchAsyncDBJob_SellItem* pJob = new MBMatchAsyncDBJob_SellItem(uidPlayer, MASYNCJOB_SELL_ITEM_TO_BOUNTY);
		pJob->Input(uidPlayer, uidItem, pCharInfo->m_nCID, pItem->GetCIID(), pItem->GetDescID(), nPrice, 1, pCharInfo->m_nBP + nPrice);
		pObj->m_DBJobQ.DBJobQ.push_back(pJob);
	}
#ifdef _SELL_CASHITEM
	else
	{
		int nPrice;

		int nResLevel = pItem->GetDesc()->m_nResLevel.Ref();
		int nRentMinuteRemainder = pItem->GetRentMinutePeriodRemainder();
		MMatchItemSlotType slotType = pItem->GetDesc()->m_nSlot;

		nPrice = CalculateCashItemToBounty(CASHITEM_NORMAL, nResLevel, nRentMinuteRemainder, slotType);

		MBMatchAsyncDBJob_SellItem* pJob = new MBMatchAsyncDBJob_SellItem(uidPlayer, MASYNCJOB_SELL_ITEM_TO_BOUNTY);
		pJob->Input(uidPlayer, uidItem, pCharInfo->m_nCID, pItem->GetCIID(), pItem->GetDescID(), nPrice, 1, pCharInfo->m_nBP + nPrice);
		pObj->m_DBJobQ.DBJobQ.push_back(pJob);
	}
#endif

	return true;
}

bool MBMatchServer::ResponseSellSpendableItem(const MUID& uidPlayer, const MUID& uidItem, int nItemCount)
{
	MMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return false;

	MMatchCharInfo* pCharInfo = pObj->GetCharInfo();
	if (pCharInfo == NULL) return false;

	if (IsValidSellItem(uidPlayer, uidItem, nItemCount) == false) return false;

	/////////////////////////////////////////////////////////////////////////////////////////////
	UpdateCharDBCachingData(pObj);		///< XP, BP, KillCount, DeathCount Ä³½³ ¾÷µ¥ÀÌÆ®
	/////////////////////////////////////////////////////////////////////////////////////////////

	MMatchItem* pItem = pObj->GetCharInfo()->m_ItemList.GetItem(uidItem);
	if (pItem == NULL || pItem->GetDesc() == NULL) return false;
	if (!pItem->GetDesc()->IsCashItem() && !pItem->GetDesc()->IsEventItem())
	{
		int nPrice;

		if (!pItem->IsRentItem())
		{	///< ¹«Á¦ÇÑ ¹Ù¿îÆ¼ ¾ÆÀÌÅÛ
			nPrice = pItem->GetDesc()->GetSellBountyValue(nItemCount);
		}
		else
		{	///< ±â°£Á¦ ¹Ù¿îÆ¼ ¾ÆÀÌÅÛ
			unsigned long	nPassTime = MGetTimeDistance(pItem->GetRentItemRegTime(), GetTickTime()); // 1/1000ÃÊ.
			int				nPassMinuteTime = nPassTime / (60000);											// Áö³ª°£ ½Ã°£À» ºÐ´ÜÀ§·Î º¯°æ.

			nPrice = CalculateRentItemSellBounty(pItem->GetDesc()->m_nBountyPrice.Ref() * pItem->GetItemCount(),
				pItem->GetRentHourPeriod(),
				(pItem->GetRentMinutePeriodRemainder() - nPassMinuteTime) / 60,
				RENT_PERIOD_UNLIMITED == pItem->GetDesc()->m_nMaxRentPeriod.Ref());

		}

		MBMatchAsyncDBJob_SellItem* pJob = new MBMatchAsyncDBJob_SellItem(uidPlayer, MASYNCJOB_SELL_SPENDABLEITEM_TO_BOUNTY);
		pJob->Input(uidPlayer, uidItem, pCharInfo->m_nCID, pItem->GetCIID(), pItem->GetDescID(), nPrice, nItemCount, pCharInfo->m_nBP + nPrice);
		pObj->m_DBJobQ.DBJobQ.push_back(pJob);
	}

	return true;
}

bool MBMatchServer::ResponseSellGItem(const MUID& uidPlayer, const MUID& uidItem, int nItemCount)
{
	MMatchObject*	pObj = GetObject(uidPlayer);
	MMatchCharInfo* pCharInfo = pObj->GetCharInfo();

	const MMatchCharGambleItem* pCharGItem = pCharInfo->m_GambleItemManager.GetGambleItemByUID(uidItem);

	const MMatchGambleItem* pGItem = GetGambleMachine().GetGambleItemByGambleItemID(pCharGItem->GetGambleItemID());
	if (NULL == pGItem) return false;

#ifndef _SELL_CASHITEM
	if (pGItem->IsCash()) {
		MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_SELL_ITEM, pObj->GetUID());
		pNew->AddParameter(new MCmdParamInt(MERR_CANNOT_SELL_CASHITEM));
		PostSafeQueue(pNew);
		return false;
	}
#endif

	if (pCharGItem->GetItemCount() < nItemCount)
	{	///< ÆÈ°íÀÚ ÇÏ´Â ¾ÆÀÌÅÛÀÇ °³¼ö°¡ ¼ÒÁöÇÏ°í ÀÖ´Â °³¼öº¸´Ù ¸¹À» °æ¿ì, ÆÈ ¼ö ¾ø´Ù.
		MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_SELL_ITEM, MUID(0, 0));
		pNew->AddParameter(new MCmdParamInt(MERR_CANNOT_SELL_TOO_MANY_ITEMCOUNT));
		RouteToListener(pObj, pNew);

		return false;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	UpdateCharDBCachingData(pObj);		///< XP, BP, KillCount, DeathCount Ä³½³ ¾÷µ¥ÀÌÆ®
	/////////////////////////////////////////////////////////////////////////////////////////////

	if (!pGItem->IsCash())
	{
		int nSellPrice = (pGItem->GetPrice() / 4) * nItemCount;

		MBMatchAsyncDBJob_SellItem* pJob = new MBMatchAsyncDBJob_SellItem(uidPlayer, MASYNCJOB_SELL_SPENDABLEITEM_TO_BOUNTY);
		pJob->Input(uidPlayer, uidItem, pCharInfo->m_nCID, pCharGItem->GetCIID(),
			pCharGItem->GetGambleItemID(), nSellPrice, nItemCount, pCharInfo->m_nBP + nSellPrice);

		pObj->m_DBJobQ.DBJobQ.push_back(pJob);
	}
#ifdef _SELL_CASHITEM
	else
	{
		int nSellPrice = CalculateCashItemToBounty(CASHITEM_GAMBLE) * nItemCount;

		MBMatchAsyncDBJob_SellItem* pJob = new MBMatchAsyncDBJob_SellItem(uidPlayer, MASYNCJOB_SELL_SPENDABLEITEM_TO_BOUNTY);
		pJob->Input(uidPlayer, uidItem, pCharInfo->m_nCID, pCharGItem->GetCIID(),
			pCharGItem->GetGambleItemID(), nSellPrice, nItemCount, pCharInfo->m_nBP + nSellPrice);

		pObj->m_DBJobQ.DBJobQ.push_back(pJob);
	}
#endif

	return true;
}

void MBMatchServer::OnRequestBringAccountItem(const MUID& uidPlayer, unsigned int nAIID, unsigned int nItemID, unsigned int nItemCnt)
{
	/**
	ÇöÀç Áß¾ÓÀºÇà BRING Á¤Ã¥

	1. ¸ðµç ¹Ù¿îÆ¼ ¾ÆÀÌÅÛ				- ÀÌµ¿ ºÒ°¡´É
	2. Ä³½¬ ÀÏ¹Ý ¾ÆÀÌÅÛ					- ±âÁ¸´ë·Î ÀÌµ¿(ÇÕÃÄÁöÁö ¾ÊÀ½)
	3. Ä³½¬ °×ºí ¾ÆÀÌÅÛ, ¼Ò¸ð¼º ¾ÆÀÌÅÛ 	- ÇÕÃÄÁ®¼­ ÀÌµ¿(¾ÆÀÌÅÛ ºÐ¸®´Â ¾ÆÀÌÅÛÀ» Áß¾ÓÀºÇàÀ¸·Î º¸³»´Â UI¿¡¼­ ¼öÇàÇÏµµ·Ï ÇÑ´Ù)
	*/

	MMatchObject* pObj = GetObject(uidPlayer);
	if (NULL == pObj) return;

	MMatchCharInfo* pCharInfo = pObj->GetCharInfo();
	if (NULL == pCharInfo) return;

	MMatchItemDesc* pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(nItemID);
	if (pItemDesc)
	{
		if (!pItemDesc->IsSpendableItem())
		{
			MBMatchAsyncDBJob_BringAccountItem* pJob = new MBMatchAsyncDBJob_BringAccountItem(pObj->GetUID());
			pJob->Input(pObj->GetAccountInfo()->m_nAID, pObj->GetCharInfo()->m_nCID, nAIID);
			pObj->m_DBJobQ.DBJobQ.push_back(pJob);
		}
		else
		{
			unsigned int nCIID;
			MMatchItem* pCharItem;

			pCharItem = pCharInfo->m_ItemList.GetItemByItemID(nItemID);
			if (pCharItem) { nCIID = pCharItem->GetCIID(); }
			else			{ nCIID = 0; }

			MBMatchAsyncDBJob_BringAccountItemStackable* pJob = new MBMatchAsyncDBJob_BringAccountItemStackable(pObj->GetUID());
			pJob->Input(pObj->GetAccountInfo()->m_nAID, pObj->GetCharInfo()->m_nCID, nAIID, nCIID, nItemCnt);
			pObj->m_DBJobQ.DBJobQ.push_back(pJob);
		}
	}
	else
	{
		const MMatchGambleItem *pGItemDesc = m_GambleMachine.GetGambleItemByGambleItemID(nItemID);
		if (pGItemDesc)
		{
			unsigned int nCIID;
			const MMatchCharGambleItem* pCharGItem;

			pCharGItem = pCharInfo->m_GambleItemManager.GetGambleItemByItemID(nItemID);
			if (pCharGItem)	{ nCIID = pCharGItem->GetCIID(); }
			else				{ nCIID = 0; }

			MBMatchAsyncDBJob_BringAccountItemStackable* pJob = new MBMatchAsyncDBJob_BringAccountItemStackable(pObj->GetUID());
			pJob->Input(pObj->GetAccountInfo()->m_nAID, pObj->GetCharInfo()->m_nCID, nAIID, nCIID, nItemCnt);
			pObj->m_DBJobQ.DBJobQ.push_back(pJob);
		}
	}
}

void MBMatchServer::OnRequestBringBackAccountItem(const MUID& uidPlayer, const MUID& uidItem, unsigned int nItemCount)
{
	/**
	ÇöÀç Áß¾ÓÀºÇà BRING BACK Á¤Ã¥

	1. ¸ðµç ¹Ù¿îÆ¼ ¾ÆÀÌÅÛ				- ÀÌµ¿ ºÒ°¡´É
	2. Ä³½¬ ÀÏ¹Ý ¾ÆÀÌÅÛ					- ±âÁ¸´ë·Î ÀÌµ¿(ÇÕÃÄÁöÁö ¾ÊÀ½)
	3. Ä³½¬ °×ºí ¾ÆÀÌÅÛ, °×ºí ¾ÆÀÌÅÛ	- ÇÕÃÄÁ®¼­ ÀÌµ¿(ºÐ¸®´Â Áß¾ÓÀºÇà UI¿¡¼­ ¼öÇàÇÏµµ·Ï ÇÑ´Ù)
	*/

	MMatchObject* pObj = GetObject(uidPlayer);
	if (NULL == pObj) return;
	MMatchCharInfo* pCharInfo = pObj->GetCharInfo();
	if (NULL == pCharInfo) return;

	if (NULL != pCharInfo->m_ItemList.GetItem(uidItem))
	{
		MMatchItem* pItem = pCharInfo->m_ItemList.GetItem(uidItem);

		if (NULL == pItem || pItem->GetDesc() == NULL)
		{	///< ¾ÆÀÌÅÛÀÌ Á¸ÀçÇÏÁö ¾ÊÀ»¶§
			MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_BRING_BACK_ACCOUNTITEM, uidPlayer);
			pNew->AddParameter(new MCmdParamInt(MERR_BRING_BACK_ACCOUNTITEM));
			PostSafeQueue(pNew);
			return;
		}
		else if (nItemCount <= 0 || pItem->GetItemCount() < nItemCount)
		{	///< °¹¼ö ¹üÀ§°¡ Àß¸øµÈ °æ¿ì
			MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_BRING_BACK_ACCOUNTITEM, MUID(0, 0));
			pNew->AddParameter(new MCmdParamInt(MERR_BRING_BACK_ACCOUNTITEM));
			RouteToListener(pObj, pNew);
			return;
		}
		else if (pItem->IsEquiped())
		{	///< ÀåºñÇÏ°í ÀÖÀ¸¸é ¿Å±æ ¼ö ¾ø´Ù
			MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_BRING_BACK_ACCOUNTITEM, MUID(0, 0));
			pNew->AddParameter(new MCmdParamInt(MERR_BRING_BACK_ACCOUNTITEM));
			RouteToListener(pObj, pNew);
			return;
		}
		else if (!pItem->GetDesc()->IsCashItem() && !pItem->GetDesc()->IsEventItem())
		{	///< Ä³½¬¾ÆÀÌÅÛÀÌ ¾Æ´Ï¸é ¿Å±æ ¼ö ¾ø´Ù
			MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_BRING_BACK_ACCOUNTITEM, MUID(0, 0));
			pNew->AddParameter(new MCmdParamInt(MERR_BRING_BACK_ACCOUNTITEM_FOR_CASHITEM));
			RouteToListener(pObj, pNew);
			return;
		}

		if (!pItem->GetDesc()->IsSpendableItem())
		{
			MBMatchAsyncDBJob_BringBackAccountItem* pJob = new MBMatchAsyncDBJob_BringBackAccountItem(uidPlayer);
			pJob->Input(uidItem, pObj->GetAccountInfo()->m_nAID, pObj->GetCharInfo()->m_nCID, pItem->GetCIID());
			pObj->m_DBJobQ.DBJobQ.push_back(pJob);
		}
		else
		{
			MBMatchAsyncDBJob_BringBackAccountItemStackable* pJob = new MBMatchAsyncDBJob_BringBackAccountItemStackable(uidPlayer);
			pJob->Input(uidItem, pObj->GetAccountInfo()->m_nAID, pCharInfo->m_nCID, pItem->GetCIID(), nItemCount);
			pObj->m_DBJobQ.DBJobQ.push_back(pJob);
		}
	}
	else if (NULL != pCharInfo->m_GambleItemManager.GetGambleItemByUID(uidItem))
	{
		const MMatchCharGambleItem* pCharGItem = pCharInfo->m_GambleItemManager.GetGambleItemByUID(uidItem);

		MBMatchAsyncDBJob_BringBackAccountItemStackable* pJob = new MBMatchAsyncDBJob_BringBackAccountItemStackable(uidPlayer);
		pJob->Input(uidItem, pObj->GetAccountInfo()->m_nAID, pCharInfo->m_nCID, pCharGItem->GetCIID(), nItemCount);
		pObj->m_DBJobQ.DBJobQ.push_back(pJob);
	}
}

const bool MBMatchServer::SendGambleItemList(const MMatchObject* pObj)
{
	static const DWORD dwMTD_DBGambleItemNodeSize = sizeof(MTD_DBGambleItmeNode);

	MMatchGambleMachine& GMachine = GetGambleMachine();

	// °×ºí ¾ÆÀÌÅÛÀÌ ¾øÀ¸¸é º¸³»Áö ¾Ê´Â´Ù.
	if (0 == GMachine.GetGambleItemSize())
	{
		return true;
	}

	if (NULL == pObj)
		return false;

	// DBÀÇ GambleItem°¡ À¯Àú°¡ °¡Áö°í ÀÖ´Â°Å º¸´Ù ÃÖ½ÅÀÌ¸é ¾÷µ¥ÀÌÆ® ÇÑ´Ù.
	if (GetGambleMachine().GetLastUpdateTime() <= pObj->GetLastSendGambleItemTime())
		return false;

	vector< DWORD >				vOpenedGItemList;
	vector< DWORD >::iterator	it, end;
	MTD_DBGambleItmeNode*		pSendGItem;
	void*						pGambleItemArray;
	DWORD						dwOpenedGItemSize;

	GMachine.GetOpenedGambleItemList(vOpenedGItemList);

	dwOpenedGItemSize = static_cast< DWORD >(vOpenedGItemList.size());
	pGambleItemArray = MMakeBlobArray(dwMTD_DBGambleItemNodeSize, dwOpenedGItemSize);

	if (NULL == pGambleItemArray)
		return false;

	for (DWORD i = 0; i < dwOpenedGItemSize; ++i)
	{
		const MMatchGambleItem* pGItem = GMachine.GetGambleItemByIndex(vOpenedGItemList[i]);

		pSendGItem = (MTD_DBGambleItmeNode*)MGetBlobArrayElement(pGambleItemArray, i);
		if (NULL == pSendGItem)
		{
			MEraseBlobArray(pGambleItemArray);
#ifdef _GAMBLEITEM_TEST_LOG
			mlog("SendGambleItemList Fail [MTD_DBGambleItmeNode]\n");
#endif
			return false;
		}

		memset(pSendGItem->szName, 0, MAX_GAMBLEITEMNAME_LEN);
		memset(pSendGItem->szDesc, 0, MAX_GAMBLEITEMDESC_LEN);

		// º¹»çÇÏ±âÀü¿¡ ±æÀÌ°¡ Á¤»óÀÎÁö È®ÀÎÇÑ´Ù.
		_ASSERT(MAX_GAMBLEITEMNAME_LEN >= pGItem->GetName().length());
		_ASSERT(MAX_GAMBLEITEMDESC_LEN >= pGItem->GetDescription().length());

		// Á¤»óÀûÀÌÁö ¾ÊÀº µ¥ÀÌÅÍ¸é ÀÛ¾÷À» Áß´Ü ÇÑ´Ù.
		if ((MAX_GAMBLEITEMNAME_LEN < pGItem->GetName().length())
			|| (MAX_GAMBLEITEMDESC_LEN < pGItem->GetDescription().length()))
		{
			MEraseBlobArray(pGambleItemArray);
#ifdef _GAMBLEITEM_TEST_LOG
			mlog("SendGambleItemList Fail [Size:%d, Count:%d, ID:%d, NameLength(Max25):%d, DescLength(Max65):%d]\n",
				dwOpenedGItemSize, i, pGItem->GetGambleItemID(), pGItem->GetName().length(), pGItem->GetDescription().length());
#else
			// °£´ÜÇÏ°Ô¶óµµ ·Î±×´Â ³²±âÀÚ -_-
			mlog("SendGambleItemList error ID:%d, too long name or desc!", pGItem->GetGambleItemID());
#endif
			return false;
		}

		memcpy(pSendGItem->szName
			, pGItem->GetName().c_str()
			, pGItem->GetName().length());

		memcpy(pSendGItem->szDesc
			, pGItem->GetDescription().c_str()
			, pGItem->GetDescription().length());

		pSendGItem->nItemID = pGItem->GetGambleItemID();
		pSendGItem->nBuyPrice = pGItem->GetPrice();
		pSendGItem->bIsCash = pGItem->IsCash();
	}

	MCommand* pNew = CreateCommand(MC_RESPONSE_GAMBLEITEMLIST, pObj->GetUID());
	if (NULL == pNew)
		return false;

	pNew->AddParameter(new MCommandParameterBlob(pGambleItemArray, MGetBlobArraySize(pGambleItemArray)));

	MEraseBlobArray(pGambleItemArray);

	PostSafeQueue(pNew);

	// DBÀÇ GambleItemÁ¤º¸¸¦ ¹Þ¾Ò´Ù´Â Ç¥½Ã¸¦ ÇÑ´Ù.
	const_cast<MMatchObject*>(pObj)->SetLastSendGambleItemTime(GetGlobalClockCount());

	return true;
}


void MBMatchServer::OnRequestEquipItem(const MUID& uidPlayer, const MUID& uidItem, const long int nEquipmentSlot)
{
	if (nEquipmentSlot >= MMCIP_END) return;
	MMatchCharItemParts parts = MMatchCharItemParts(nEquipmentSlot);

	ResponseEquipItem(uidPlayer, uidItem, parts);
}


void MBMatchServer::ResponseEquipItem(const MUID& uidPlayer, const MUID& uidItem, const MMatchCharItemParts parts)
{
	MMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return;

	int nResult = MOK;
	MMatchCharInfo* pCharInfo = pObj->GetCharInfo();
	if (pCharInfo == NULL) return;

	MMatchItem* pItem = pCharInfo->m_ItemList.GetItem(uidItem);

	if ((pItem == NULL) || (!IsEquipmentTypeItem(pItem->GetDesc())) || (!IsSuitableItemSlot(pItem->GetDesc()->m_nSlot, parts)))
	{
		// °á°ú¸¦ ÇÃ·¹ÀÌ¾î¿¡°Ô ÀÀ´äÇÑ´Ù.
		nResult = MERR_CANNOT_EQUIP_ITEM;
		MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_EQUIP_ITEM, MUID(0, 0));
		pNew->AddParameter(new MCommandParameterInt(nResult));
		RouteToListener(pObj, pNew);
		return;
	}

	nResult = ValidateEquipItem(pObj, pItem, parts);
	if (nResult != MOK)
	{
		// °á°ú¸¦ ÇÃ·¹ÀÌ¾î¿¡°Ô ÀÀ´äÇÑ´Ù.
		MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_EQUIP_ITEM, uidPlayer);
		pNew->AddParameter(new MCommandParameterInt(nResult));
		PostSafeQueue(pNew);
		return;
	}

	if (!m_MatchDBMgr.UpdateEquipedItem(pCharInfo->m_nCID, parts, pItem->GetCIID(), pItem->GetDescID()))
	{
		MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_EQUIP_ITEM, uidPlayer);
		pNew->AddParameter(new MCommandParameterInt(MERR_CANNOT_EQUIP_ITEM));
		PostSafeQueue(pNew);

		return;
	}

	if (!pCharInfo->m_EquipedItem.SetItem(parts, uidItem, pItem))
	{
		MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_EQUIP_ITEM, uidPlayer);
		pNew->AddParameter(new MCommandParameterInt(MERR_CANNOT_EQUIP_ITEM));
		PostSafeQueue(pNew);

		return;
	}

	ResponseCharacterItemList(uidPlayer);

	if (FindStage(pObj->GetStageUID()))
	{
		MCommand* pEquipInfo = CreateCommand(MC_MATCH_ROUTE_UPDATE_STAGE_EQUIP_LOOK, MUID(0, 0));
		pEquipInfo->AddParameter(new MCmdParamUID(uidPlayer));
		pEquipInfo->AddParameter(new MCmdParamInt(parts));
		pEquipInfo->AddParameter(new MCmdParamInt(pItem->GetDescID()));
		RouteToStage(pObj->GetStageUID(), pEquipInfo);
	}
}


void MBMatchServer::OnRequestTakeoffItem(const MUID& uidPlayer, const unsigned long int nEquipmentSlot)
{
	if (nEquipmentSlot >= MMCIP_END) return;
	MMatchCharItemParts parts = MMatchCharItemParts(nEquipmentSlot);

	ResponseTakeoffItem(uidPlayer, parts);
}


void MBMatchServer::ResponseTakeoffItem(const MUID& uidPlayer, const MMatchCharItemParts parts)
{
	MMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return;
	int nResult = MOK;

	MMatchCharInfo* pCharInfo = pObj->GetCharInfo();
	if (pCharInfo == NULL) return;
	if (pCharInfo->m_EquipedItem.IsEmpty(parts)) return;

	MMatchItem* pItem = pCharInfo->m_EquipedItem.GetItem(parts);
	if (NULL == pItem) return;

	MMatchItemDesc* pItemDesc = pItem->GetDesc();
	if (pItemDesc == NULL) return;

	// ¹«°Ô Ã¼Å©
	int nWeight = 0, nMaxWeight = 0;
	pCharInfo->m_EquipedItem.GetTotalWeight(&nWeight, &nMaxWeight);
	nMaxWeight = pCharInfo->m_nMaxWeight + nMaxWeight - pItemDesc->m_nMaxWT.Ref();
	nWeight -= pItemDesc->m_nWeight.Ref();

	if (nWeight > nMaxWeight) nResult = MERR_CANNOT_TAKEOFF_ITEM_BY_WEIGHT;

	if (MOK == nResult)
	{
		//bool bRet = false; // DB½ÇÆÐÇÏ´õ¶óµµ ¾ÆÀÌÅÛ »èÁ¦¸¦ ÇÏ±â ¶§¹®¿¡ »ç¿ëÇÒ °÷ÀÌ ¾ø¾îÁ³À½;;
		//  DB¾÷µ¥ÀÌÆ®¸¦ ½ÇÆÐÇÏ´õ¶ó°í »ç¿ëÇÒ ¼ö ¾ø´Â ¾ÆÀÌÅÛÀÌ±â ¶§¹®¿¡ 
		//   ¾ÆÀÌÅÛ ¸®½ºÆ®¿¡¼­ »èÁ¦¸¦ ÇÑ´Ù.
		if (!m_MatchDBMgr.UpdateEquipedItem(pCharInfo->m_nCID, parts, 0, 0))
		{
			mlog("DB Query(ResponseEquipItem > UpdateEquipedItem) Failed\n");
			// nResult = MERR_CANNOT_TAKEOFF_ITEM;
		}

		pCharInfo->m_EquipedItem.Remove(parts);

		MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_TAKEOFF_ITEM, MUID(0, 0));
		pNew->AddParameter(new MCommandParameterInt(nResult));
		RouteToListener(pObj, pNew);

		ResponseCharacterItemList(uidPlayer);

		// ¸¸¾à ½ºÅ×ÀÌÁö ¾ÈÀÌ¸é ½ºÅ×ÀÌÁö¿¡ ¾Ë·Á¼­ À¯ÀúÀÇ LookÀ» ¾÷µ¥ÀÌÆ® ÇØÁà¾ß ÇÑ´Ù.
		if (FindStage(pObj->GetStageUID()))
		{
			MCommand* pEquipInfo = CreateCommand(MC_MATCH_ROUTE_UPDATE_STAGE_EQUIP_LOOK, MUID(0, 0));
			pEquipInfo->AddParameter(new MCmdParamUID(uidPlayer));
			pEquipInfo->AddParameter(new MCmdParamInt(parts));
			pEquipInfo->AddParameter(new MCmdParamInt(0));
			RouteToStage(pObj->GetStageUID(), pEquipInfo);
		}
	}
}
