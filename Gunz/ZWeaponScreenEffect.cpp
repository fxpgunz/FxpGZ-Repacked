
#include "stdafx.h"
#include "ZWeaponScreenEffect.h"
#include "ZGame.h"
#include "ZMyInfo.h"
#include "ZMyItemList.h"
#include "ZConfiguration.h"

ZWeaponScreenEffect::ZWeaponScreenEffect()
{
	m_nWeaponType = MWT_NONE;
	m_pSniferPanel = NULL;
}

ZWeaponScreenEffect::~ZWeaponScreenEffect()
{

}

void ZWeaponScreenEffect::Draw(MDrawContext* pDC)
{

	switch (m_nWeaponType)
	{
	case MWT_SNIFER:
	{
		if (m_pSniferPanel)
		{
			pDC->SetBitmap(m_pSniferPanel);
			pDC->Draw(0, 0, RGetScreenWidth(), RGetScreenHeight());
		}

	}
	break;
	}
}

void ZWeaponScreenEffect::InitWeapon(MMatchWeaponType nWeaponType)
{

	switch (nWeaponType)
	{
	case MWT_SNIFER:
	{
		PrevMouseSens = Z_MOUSE_SENSITIVITY;
		g_fFOV = (40.0f / 180.0f * pi);
		ZGetConfiguration()->SetMouseSensitivityInFloat(ZGetConfiguration()->GetMouseSensitivityInFloat() / 10.0f);
	}
	break;
	}

}

void ZWeaponScreenEffect::FinalWeapon(MMatchWeaponType nWeaponType)
{

	switch (nWeaponType)
	{
	case MWT_SNIFER:
	{
		g_fFOV = (70.0f / 180.0f * pi);
		ZGetConfiguration()->SetMouseSensitivityInFloat(ZGetConfiguration()->GetMouseSensitivityInFloat() * 10.0f);
	}
	break;
	}
}

void ZWeaponScreenEffect::OnGadget(MMatchWeaponType nWeaponType)
{

	if (m_nWeaponType == nWeaponType) return;
	if (m_nWeaponType != MWT_NONE) OnGadgetOff();

	InitWeapon(nWeaponType);
	m_nWeaponType = nWeaponType;
}

void ZWeaponScreenEffect::OnGadgetOff()
{

	FinalWeapon(m_nWeaponType);
	m_nWeaponType = MWT_NONE;
}

void ZWeaponScreenEffect::Create()
{

	MMatchWeaponType nPrimaryWeaponType = MWT_NONE,
		nSecondaryWeaponType = MWT_NONE,
		nMeleeWeaponType = MWT_NONE;

	unsigned long int nItemID;
	nItemID = ZGetMyInfo()->GetItemList()->GetEquipedItemID(MMCIP_PRIMARY);
	MMatchItemDesc* pItemDesc = NULL;
	pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(nItemID);
	if (pItemDesc)
	{
		nPrimaryWeaponType = pItemDesc->m_nWeaponType.Ref();
	}

	nItemID = ZGetMyInfo()->GetItemList()->GetEquipedItemID(MMCIP_SECONDARY);

	pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(nItemID);
	if (pItemDesc)
	{
		nSecondaryWeaponType = pItemDesc->m_nWeaponType.Ref();
	}
	nItemID = ZGetMyInfo()->GetItemList()->GetEquipedItemID(MMCIP_MELEE);

	pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(nItemID);
	if (pItemDesc)
	{
		nMeleeWeaponType = pItemDesc->m_nWeaponType.Ref();
	}

	if ((nPrimaryWeaponType == MWT_SNIFER) || (nSecondaryWeaponType == MWT_SNIFER) || (ZGetGameClient()->GetMatchStageSetting()->GetGameType() == MMATCH_GAMETYPE_SNIPERMODE))
	{
		if (m_pSniferPanel) delete m_pSniferPanel;
		m_pSniferPanel = new MBitmapR2;
		((MBitmapR2*)m_pSniferPanel)->Create("sniper.png", RGetDevice(), "interface/default/sniper/sniper.png");
	}
}

void ZWeaponScreenEffect::Destroy()
{

	if (m_pSniferPanel)
	{
		delete m_pSniferPanel;
		m_pSniferPanel = NULL;
	}
}