#include "stdafx.h"
#include "MBMatchAsyncDBJob_GambleQuestItem.h"

void MBMatchAsyncDBJob_GambleQuestItem::Run( void* pContext )
{
	MMatchDBMgr* pDBMgr = (MMatchDBMgr*)pContext;
	
	// �ڽ��� �κ��丮�� �����ϴ� ���������� Ȯ���Ѵ�. //
	MMatchObject* pObj = MGetMatchServer()->GetObject(m_uidPlayer);
	if (pObj == NULL) { return; }

	MMatchCharInfo* pCharInfo = pObj->GetCharInfo();
	if (pCharInfo == NULL) { return; }

	// ������ ������ �ִ� ������ ���� ���̵�� �ش� ������ ã��
	const MMatchCharGambleItem* pGambleItem = pCharInfo->m_GambleItemManager.GetGambleItemByUID(m_uidGItem);
	if (pGambleItem == NULL) { return; }


	if( !pDBMgr->UpdateQuestItem(pCharInfo->m_nCID, pCharInfo->m_QuestItemList, pCharInfo->m_QMonsterBible)  )
	{
		SetResult( MASYNC_RESULT_FAILED );
		return;
	}

	if( !pDBMgr->UpdateCharGambleItemCount(pGambleItem->GetCIID(), pGambleItem->GetGambleItemID()) )
	{
		SetResult( MASYNC_RESULT_FAILED );
		return;
	}

	SetResult( MASYNC_RESULT_SUCCEED );
}