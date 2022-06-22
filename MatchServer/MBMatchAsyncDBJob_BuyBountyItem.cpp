#include "stdafx.h"
#include "MBMatchAsyncDBJob_BuyBountyItem.h"

void MBMatchAsyncDBJob_BuyBountyItem::Run(void* pContext)
{
	MMatchDBMgr* pDBMgr = (MMatchDBMgr*)pContext;

	if (!pDBMgr->BuyBountyItem(m_nCID, m_nAID, m_nItemID, m_nItemCount, m_dwPrice, m_wRentHourPeriod, m_bIsSpendableItem, &m_dwCIID, m_nBuyMode)) /// Jorklenis2: <nBuyMode>
	{
		SetResult(MASYNC_RESULT_FAILED);
		return;
	}

	SetResult(MASYNC_RESULT_SUCCEED);
}