#include "stdafx.h"
#include "MMatchItem.h"
#include "MZFileSystem.h"
#include "MMatchServer.h"
#include "MBaseStringResManager.h"
#include "MMatchCRC32XORCache.h"
#include "MCRC32.h"

#define DEFAULT_MELEE_WEAPON_RANGE 160

MUID MMatchItemMap::m_uidGenerate = MUID(0,0);
MCriticalSection MMatchItemMap::m_csUIDGenerateLock;

MMatchItemDesc::MMatchItemDesc() : m_nID(0), m_nSlot(MMIST_NONE), m_pEffect(NULL), m_bSlugOutput(0), 
	m_nColor(0xFFFFFFFF), m_nImageID(0), m_nBulletImageID(0), m_nMagazineImageID(0), m_bIsCashItem(false), m_bIsSpendableItem(false)
{
	m_nTotalPoint.Set_MakeCrc(0);
	m_nType.Set_MakeCrc(MMIT_MELEE);
	m_nResSex.Set_MakeCrc(0);
	m_nResLevel.Set_MakeCrc(0);
	m_nWeaponType.Set_MakeCrc(MWT_NONE);
	m_nWeight.Set_MakeCrc(0);
	m_nBountyPrice.Set_MakeCrc(0);	
	m_nDelay.Set_MakeCrc(0);
	m_nEffectId = MMIEI_NONE;
	m_nControllability.Set_MakeCrc(0);
	m_nMagazine.Set_MakeCrc(0);
	m_nGadgetID.Set_MakeCrc(0);
    m_nReloadTime.Set_MakeCrc(0);
	m_nHP.Set_MakeCrc(0);
	m_nAP.Set_MakeCrc(0);
	m_nMaxWT.Set_MakeCrc(0); 
	m_nSF.Set_MakeCrc(0);
	m_nFR.Set_MakeCrc(0); 
	m_nCR.Set_MakeCrc(0); 
	m_nPR.Set_MakeCrc(0); 
	m_nLR.Set_MakeCrc(0);
	m_nMaxBullet.Set_MakeCrc(0);
	m_nLimitSpeed.Set_MakeCrc(100);
	m_nLimitJump.Set_MakeCrc(0);
	m_nLimitTumble.Set_MakeCrc(0);
	m_nLimitWall.Set_MakeCrc(0);
	m_nEffectLevel.Set_MakeCrc(0);
	m_nMaxRentPeriod.Set_MakeCrc(0);

	m_nDamage.Set_MakeCrc(0);
	m_nItemPower.Set_MakeCrc(0);
	m_nDamageType.Set_MakeCrc(MMDT_NORMAL);
	m_nDamageTime.Set_MakeCrc(0);
	m_nLifeTime.Set_MakeCrc(0);

	m_pMItemName = new MProtectValue<MMatchItemName>;
	memset(&m_pMItemName->Ref(), 0, sizeof(MMatchItemName));
	//memset(m_szItemName, 0, sizeof(m_szItemName));
	memset(m_szDesc, 0, sizeof(m_szDesc));
//	memset(m_szMeshName, 0, sizeof(m_szMeshName));
	memset(m_szReloadSndName, 0, sizeof(m_szReloadSndName));
	memset(m_szFireSndName, 0, sizeof(m_szFireSndName));
	memset(m_szDryfireSndName, 0, sizeof(m_szDryfireSndName));
	memset(m_szWeaponByFiber, 0, sizeof(m_szWeaponByFiber));

	m_Bonus.m_fXP_SoloBonus = 0.0f;
	m_Bonus.m_fXP_TeamBonus = 0.0f;
	m_Bonus.m_fXP_QuestBonus = 0.0f;

	m_bIsEnableMoveToAccountItem = false;

	m_TicketType = MMTT_NONE;

	m_nSpendType.Set_MakeCrc(MMCT_NONE);

	m_pAvatarMeshName = new MProtectValue<MMatchAvatarMeshName>;
	memset(&m_pAvatarMeshName->Ref(), 0, sizeof(MMatchAvatarMeshName));
	m_pAvatarMeshName->MakeCrc();
}

MMatchItemDesc::~MMatchItemDesc()
{
	delete m_pMItemName;
	delete m_pAvatarMeshName;
}


void MMatchItemDesc::CacheCRC32( MMatchCRC32XORCache& crc )
{
	crc.CRC32XOR( m_nID );
	crc.CRC32XOR( m_nType.Ref() );
	crc.CRC32XOR( m_nSlot );
	crc.CRC32XOR( m_nWeight.Ref() );	
	crc.CRC32XOR( m_nDelay.Ref() );
	crc.CRC32XOR( m_nControllability.Ref() );
	crc.CRC32XOR( m_nMagazine.Ref() );
	crc.CRC32XOR( m_nMaxBullet.Ref() );
	crc.CRC32XOR( m_nReloadTime.Ref() );
	crc.CRC32XOR( m_nHP.Ref() );
	crc.CRC32XOR( m_nAP.Ref() );
	crc.CRC32XOR( m_nMaxWT.Ref() );
	crc.CRC32XOR( m_nLimitSpeed.Ref() );
	crc.CRC32XOR( m_nLimitJump.Ref() );
	crc.CRC32XOR( m_nLimitTumble.Ref() );
	crc.CRC32XOR( m_nLimitWall.Ref() );
	crc.CRC32XOR( m_nRange.Ref() );
	crc.CRC32XOR( m_nAngle.Ref() );
	crc.CRC32XOR( m_nEffectLevel.Ref() );

	crc.CRC32XOR( m_nDamage.Ref() );
	crc.CRC32XOR( m_nItemPower.Ref() );
	crc.CRC32XOR( m_nDamageType.Ref() );
	crc.CRC32XOR( m_nDamageTime.Ref() );
	crc.CRC32XOR( m_nLifeTime.Ref() );

	// 메쉬명을 바꾸는 메모리핵 때문에 메쉬명도 crc에 포함 (따발공속으로 칼질함)
	const char* szMeshName;
	size_t len;
	DWORD checksum;	

	szMeshName = m_pMItemName->Ref().m_szMeshName;
	len = strlen(szMeshName);
	checksum = 0;
	for (size_t i=0; i<len; ++i) checksum += szMeshName[i] * DWORD(i<<1);
	crc.CRC32XOR( checksum );

	szMeshName = m_pAvatarMeshName->Ref().m_szHeadMeshName;
	len = strlen(szMeshName);
	checksum = 0;
	for (size_t i=0; i<len; ++i) checksum += szMeshName[i] * DWORD(i<<1);
	crc.CRC32XOR( checksum );

	szMeshName = m_pAvatarMeshName->Ref().m_szChestMeshName;
	len = strlen(szMeshName);
	checksum = 0;
	for (size_t i=0; i<len; ++i) checksum += szMeshName[i] * DWORD(i<<1);
	crc.CRC32XOR( checksum );

	szMeshName = m_pAvatarMeshName->Ref().m_szHandMeshName;
	len = strlen(szMeshName);
	checksum = 0;
	for (size_t i=0; i<len; ++i) checksum += szMeshName[i] * DWORD(i<<1);
	crc.CRC32XOR( checksum );

	szMeshName = m_pAvatarMeshName->Ref().m_szLegsMeshName;
	len = strlen(szMeshName);
	checksum = 0;
	for (size_t i=0; i<len; ++i) checksum += szMeshName[i] * DWORD(i<<1);
	crc.CRC32XOR( checksum );

	szMeshName = m_pAvatarMeshName->Ref().m_szFeetMeshName;
	len = strlen(szMeshName);
	checksum = 0;
	for (size_t i=0; i<len; ++i) checksum += szMeshName[i] * DWORD(i<<1);
	crc.CRC32XOR( checksum );	
}

void MMatchItemDesc::ShiftFugitiveValues()
{
	m_pAvatarMeshName->ShiftHeapPos_CheckCrc();

	m_pMItemName->ShiftHeapPos_CheckCrc();
	m_nTotalPoint.ShiftHeapPos_CheckCrc();
	m_nWeaponType.ShiftHeapPos_CheckCrc();
	m_nType.ShiftHeapPos_CheckCrc();
	m_nResSex.ShiftHeapPos_CheckCrc();
	m_nResLevel.ShiftHeapPos_CheckCrc();

	m_nWeight.ShiftHeapPos_CheckCrc();
	m_nBountyPrice.ShiftHeapPos_CheckCrc();	
	m_nDelay.ShiftHeapPos_CheckCrc();
	m_nControllability.ShiftHeapPos_CheckCrc();
	m_nMagazine.ShiftHeapPos_CheckCrc();
	m_nMaxBullet.ShiftHeapPos_CheckCrc();
	m_nReloadTime.ShiftHeapPos_CheckCrc();
	m_nGadgetID.ShiftHeapPos_CheckCrc();
	m_nHP.ShiftHeapPos_CheckCrc();
	m_nAP.ShiftHeapPos_CheckCrc();
	m_nMaxWT.ShiftHeapPos_CheckCrc();
	m_nSF.ShiftHeapPos_CheckCrc();
	m_nFR.ShiftHeapPos_CheckCrc();
	m_nCR.ShiftHeapPos_CheckCrc();
	m_nPR.ShiftHeapPos_CheckCrc();
	m_nLR.ShiftHeapPos_CheckCrc();
	m_nLimitSpeed.ShiftHeapPos_CheckCrc();
	m_nLimitJump.ShiftHeapPos_CheckCrc();
	m_nLimitTumble.ShiftHeapPos_CheckCrc();
	m_nLimitWall.ShiftHeapPos_CheckCrc();
	m_nRange.ShiftHeapPos_CheckCrc();
	m_nAngle.ShiftHeapPos_CheckCrc();
	m_nEffectLevel.ShiftHeapPos_CheckCrc();
	m_nMaxRentPeriod.ShiftHeapPos_CheckCrc();

	m_nDamage.ShiftHeapPos_CheckCrc();
	m_nItemPower.ShiftHeapPos_CheckCrc();
	m_nDamageType.ShiftHeapPos_CheckCrc();
	m_nDamageTime.ShiftHeapPos_CheckCrc();
	m_nLifeTime.ShiftHeapPos_CheckCrc();
}

void MMatchItemDesc::DumpBinary(FILE* fp)
{
#ifdef _DEBUG
	fwrite(&m_nID, sizeof(unsigned long int), 1, fp);
	fwrite(&m_pMItemName->Ref(), sizeof(MMatchItemName), 1, fp);
	fwrite(&m_nTotalPoint.Ref(), sizeof(int), 1, fp);
	fwrite(&m_nWeaponType.Ref(), sizeof(MMatchWeaponType), 1, fp);
	fwrite(&m_nType.Ref(), sizeof(MMatchItemType), 1, fp);
	fwrite(&m_nResSex.Ref(), sizeof(int), 1, fp);
	fwrite(&m_nResLevel.Ref(), sizeof(m_nResSex), 1, fp);
	fwrite(&m_nSlot, sizeof(MMatchItemSlotType), 1, fp);

	fwrite(&m_nWeight.Ref(), sizeof(int), 1, fp);
	fwrite(&m_nBountyPrice.Ref(), sizeof(int), 1, fp);
	fwrite(&m_bIsCashItem, sizeof(bool), 1, fp);
	fwrite(&m_nDelay.Ref(), sizeof(int), 1, fp);

	fwrite(&m_nEffectId, sizeof(MMatchItemEffectId), 1, fp);
	fwrite(&m_nControllability.Ref(), sizeof(int), 1, fp);
	fwrite(&m_nMagazine.Ref(), sizeof(int), 1, fp);
	fwrite(&m_nMaxBullet.Ref(), sizeof(int), 1, fp);
	fwrite(&m_nReloadTime.Ref(), sizeof(int), 1, fp);
	
	fwrite(&m_bSlugOutput, sizeof(bool), 1, fp);

	fwrite(&m_nGadgetID.Ref(), sizeof(int), 1, fp);
	fwrite(&m_nHP.Ref(), sizeof(int), 1, fp);
	fwrite(&m_nAP.Ref(), sizeof(int), 1, fp);
	fwrite(&m_nMaxWT.Ref(), sizeof(int), 1, fp);
	fwrite(&m_nSF.Ref(), sizeof(int), 1, fp);
	fwrite(&m_nFR.Ref(), sizeof(int), 1, fp);
	fwrite(&m_nCR.Ref(), sizeof(int), 1, fp);
	fwrite(&m_nPR.Ref(), sizeof(int), 1, fp);
	fwrite(&m_nLR.Ref(), sizeof(int), 1, fp);
	fwrite(&m_nLimitSpeed.Ref(), sizeof(int), 1, fp);
	fwrite(&m_nLimitJump.Ref(), sizeof(int), 1, fp);
	fwrite(&m_nLimitTumble.Ref(), sizeof(int), 1, fp);
	fwrite(&m_nLimitWall.Ref(), sizeof(int), 1, fp);
	fwrite(&m_nRange.Ref(), sizeof(int), 1, fp);
	fwrite(&m_nAngle.Ref(), sizeof(int), 1, fp);
	fwrite(&m_nEffectLevel.Ref(), sizeof(int), 1, fp);

	fwrite(&m_nDamage.Ref(), sizeof(int), 1, fp);
	fwrite(&m_nItemPower.Ref(), sizeof(int), 1, fp);
	fwrite(&m_nDamageTime.Ref(), sizeof(int), 1, fp);
	fwrite(&m_nDamageType.Ref(), sizeof(MMatchDamageType), 1, fp);
	fwrite(&m_nLifeTime.Ref(), sizeof(int), 1, fp);
	
	fwrite(m_szDesc, sizeof(m_szDesc), 1, fp);
	
	fwrite(&m_bIsSpendableItem, sizeof(bool), 1, fp);
	fwrite(&m_nSpendType.Ref(), sizeof(MMatchSpendType), 1, fp);
	
	fwrite(&m_pAvatarMeshName->Ref(), sizeof(MMatchAvatarMeshName), 1, fp);

	fwrite(&m_nColor, sizeof(unsigned long int), 1, fp);
	fwrite(&m_nImageID, sizeof(int), 1, fp);
	fwrite(&m_nBulletImageID, sizeof(int), 1, fp);
	fwrite(&m_nMagazineImageID, sizeof(int), 1, fp);
	fwrite(&m_szReloadSndName, sizeof(m_szReloadSndName), 1, fp);
	fwrite(&m_szFireSndName, sizeof(m_szFireSndName), 1, fp);
	fwrite(&m_szDryfireSndName, sizeof(m_szDryfireSndName), 1, fp);
	fwrite(&m_szWeaponByFiber, sizeof(m_szWeaponByFiber), 1, fp);
	
	fwrite(&m_Bonus, sizeof(MMatchItemBonus), 1, fp);
	fwrite(&m_TicketType, sizeof(MMatchTicketType), 1, fp);
	fwrite(&m_nMaxRentPeriod.Ref(), sizeof(int), 1, fp);

	fwrite(&m_bIsEnableMoveToAccountItem, sizeof(bool), 1, fp);
#endif
}

void MMatchItemDesc::LoadBinary(FILE* fp)
{
#ifdef _DEBUG
	fread(&m_nID, sizeof(unsigned long int), 1, fp);
	fread(&m_pMItemName->Ref(), sizeof(MMatchItemName), 1, fp);		m_pMItemName->MakeCrc();
	fread(&m_nTotalPoint.Ref(), sizeof(int), 1, fp);				m_nTotalPoint.MakeCrc();
	fread(&m_nWeaponType.Ref(), sizeof(MMatchWeaponType), 1, fp);	m_nWeaponType.MakeCrc();
	fread(&m_nType.Ref(), sizeof(MMatchItemType), 1, fp);			m_nType.MakeCrc();
	fread(&m_nResSex.Ref(), sizeof(int), 1, fp);					m_nResSex.MakeCrc();
	fread(&m_nResLevel.Ref(), sizeof(m_nResSex), 1, fp);			m_nResLevel.MakeCrc();
	fread(&m_nSlot, sizeof(MMatchItemSlotType), 1, fp);

	fread(&m_nWeight.Ref(), sizeof(int), 1, fp);					m_nWeight.MakeCrc();
	fread(&m_nBountyPrice.Ref(), sizeof(int), 1, fp);				m_nBountyPrice.MakeCrc();
	fread(&m_bIsCashItem, sizeof(bool), 1, fp);
	fread(&m_nDelay.Ref(), sizeof(int), 1, fp);						m_nDelay.MakeCrc();

	m_pEffect = NULL;

	fread(&m_nEffectId, sizeof(MMatchItemEffectId), 1, fp);
	fread(&m_nControllability.Ref(), sizeof(int), 1, fp);			m_nControllability.MakeCrc();
	fread(&m_nMagazine.Ref(), sizeof(int), 1, fp);					m_nMagazine.MakeCrc();
	fread(&m_nMaxBullet.Ref(), sizeof(int), 1, fp);					m_nMaxBullet.MakeCrc();
	fread(&m_nReloadTime.Ref(), sizeof(int), 1, fp);				m_nReloadTime.MakeCrc();

	fread(&m_bSlugOutput, sizeof(bool), 1, fp);

	fread(&m_nGadgetID.Ref(), sizeof(int), 1, fp);					m_nGadgetID.MakeCrc();
	fread(&m_nHP.Ref(), sizeof(int), 1, fp);						m_nHP.MakeCrc();
	fread(&m_nAP.Ref(), sizeof(int), 1, fp);						m_nAP.MakeCrc();
	fread(&m_nMaxWT.Ref(), sizeof(int), 1, fp);						m_nMaxWT.MakeCrc();
	fread(&m_nSF.Ref(), sizeof(int), 1, fp);						m_nSF.MakeCrc();
	fread(&m_nFR.Ref(), sizeof(int), 1, fp);						m_nFR.MakeCrc();
	fread(&m_nCR.Ref(), sizeof(int), 1, fp);						m_nCR.MakeCrc();
	fread(&m_nPR.Ref(), sizeof(int), 1, fp);						m_nPR.MakeCrc();
	fread(&m_nLR.Ref(), sizeof(int), 1, fp);						m_nLR.MakeCrc();
	fread(&m_nLimitSpeed.Ref(), sizeof(int), 1, fp);				m_nLimitSpeed.MakeCrc();
	fread(&m_nLimitJump.Ref(), sizeof(int), 1, fp);					m_nLimitJump.MakeCrc();
	fread(&m_nLimitTumble.Ref(), sizeof(int), 1, fp);				m_nLimitTumble.MakeCrc();
	fread(&m_nLimitWall.Ref(), sizeof(int), 1, fp);					m_nLimitWall.MakeCrc();
	fread(&m_nRange.Ref(), sizeof(int), 1, fp);						m_nRange.MakeCrc();
	fread(&m_nAngle.Ref(), sizeof(int), 1, fp);						m_nAngle.MakeCrc();
	fread(&m_nEffectLevel.Ref(), sizeof(int), 1, fp);				m_nEffectLevel.MakeCrc();

	fread(&m_nDamage.Ref(), sizeof(int), 1, fp);					m_nDamage.MakeCrc();
	fread(&m_nItemPower.Ref(), sizeof(int), 1, fp);					m_nItemPower.MakeCrc();
	fread(&m_nDamageTime.Ref(), sizeof(int), 1, fp);				m_nDamageTime.MakeCrc();
	fread(&m_nDamageType.Ref(), sizeof(MMatchDamageType), 1, fp);	m_nDamageType.MakeCrc();
	fread(&m_nLifeTime.Ref(), sizeof(int), 1, fp);					m_nLifeTime.MakeCrc();

	fread(m_szDesc, sizeof(m_szDesc), 1, fp);

	fread(&m_bIsSpendableItem, sizeof(bool), 1, fp);
	fread(&m_nSpendType.Ref(), sizeof(MMatchSpendType), 1, fp);		m_nSpendType.MakeCrc();

	fread(&m_pAvatarMeshName->Ref(), sizeof(MMatchAvatarMeshName), 1, fp);	m_pAvatarMeshName->MakeCrc();

	fread(&m_nColor, sizeof(unsigned long int), 1, fp);
	fread(&m_nImageID, sizeof(int), 1, fp);
	fread(&m_nBulletImageID, sizeof(int), 1, fp);
	fread(&m_nMagazineImageID, sizeof(int), 1, fp);
	fread(&m_szReloadSndName, sizeof(m_szReloadSndName), 1, fp);
	fread(&m_szFireSndName, sizeof(m_szFireSndName), 1, fp);
	fread(&m_szDryfireSndName, sizeof(m_szDryfireSndName), 1, fp);
	fread(&m_szWeaponByFiber, sizeof(m_szWeaponByFiber), 1, fp);

	fread(&m_Bonus, sizeof(MMatchItemBonus), 1, fp);
	fread(&m_TicketType, sizeof(MMatchTicketType), 1, fp);
	fread(&m_nMaxRentPeriod.Ref(), sizeof(int), 1, fp);				m_nMaxRentPeriod.MakeCrc();

	fread(&m_bIsEnableMoveToAccountItem, sizeof(bool), 1, fp);
#endif
}

///////////////////////////////////////////////////////////////////////////////
// MMatchItemDescMgr //////////////////////////////////////////////////////////
MMatchItemDescMgr::MMatchItemDescMgr() : m_nChecksum(0), m_nextItemIdToMemoryShift(0)
{

}

MMatchItemDescMgr::~MMatchItemDescMgr()
{
	Clear();
}

bool MMatchItemDescMgr::ReadCache()
{
	return false;
}

void MMatchItemDescMgr::WriteCache()
{

}

bool MMatchItemDescMgr::ReadXml(const char* szFileName,MZFileSystem* pFileSystem)
{
	MXmlDocument	xmlIniData;
	xmlIniData.Create();

	//	<-----------------
	char *buffer;
	MZFile mzf;

	if(pFileSystem) 
	{
		if(!mzf.Open(szFileName,pFileSystem)) 
		{
			if(!mzf.Open(szFileName)) 
			{
				xmlIniData.Destroy();
				return false;
			}
		}
	} 
	else 
	{
		if(!mzf.Open(szFileName))
		{
			xmlIniData.Destroy();
			return false;
		}
	}

	buffer = new char[mzf.GetLength()+1];
	buffer[mzf.GetLength()] = 0;
	memset( buffer, 0, mzf.GetLength()+1 );

	mzf.Read(buffer,mzf.GetLength());

	m_nChecksum = MGetMemoryChecksum(buffer,mzf.GetLength());

	if(!xmlIniData.LoadFromMemory(buffer))
	{
		delete[] buffer;
		m_nChecksum = 0;
		xmlIniData.Destroy();
		return false;
	}
	delete[] buffer;
	mzf.Close();
	//	<------------------



	MXmlElement rootElement, chrElement, attrElement;
	char szTagName[256];

	rootElement = xmlIniData.GetDocumentElement();
	int iCount = rootElement.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		chrElement = rootElement.GetChildNode(i);
		chrElement.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;

		if (!_stricmp(szTagName, "ITEM"))
		{
			if( !ParseItem(chrElement) ) {
				Clear();
				return false;
			}
		}
	}

	xmlIniData.Destroy();
	return true;
}
void MMatchItemDescMgr::Clear()
{
	while(!empty())
	{
		MMatchItemDesc* pItemDesc = (*begin()).second;
		delete pItemDesc; pItemDesc = NULL;
		erase(begin());
	}
}
MMatchItemDesc* MMatchItemDescMgr::GetItemDesc(unsigned long int nID)
{
	iterator itor = find(nID);
	if (itor != end())
	{
		return (*itor).second;
	}
	return NULL;
}



bool MMatchItemDescMgr::ParseItem(MXmlElement& element)
{
	MMatchItemDesc* pNewDesc = new MMatchItemDesc;

	// default 값 입력
	pNewDesc->m_bIsCashItem = false;
	pNewDesc->m_nLimitSpeed.Set_MakeCrc(100);
	pNewDesc->m_nRange.Set_MakeCrc(DEFAULT_MELEE_WEAPON_RANGE);
	pNewDesc->m_nColor = 0xFFFFFFFF;
	pNewDesc->m_nAngle.Set_MakeCrc(120);
	pNewDesc->m_nMaxRentPeriod.Set_MakeCrc(RENT_PERIOD_UNLIMITED);

	int n = 0;
	char szAttrValue[256];
	char szAttrName[64];

	int nAttrCount = element.GetAttributeCount();
	for (int i = 0; i < nAttrCount; i++)
	{
		memset( szAttrValue, 0, 256 );
		memset( szAttrName, 0, 64 );

		element.GetAttribute(i, szAttrName, szAttrValue);
		if (!_stricmp(szAttrName, "ID"))
		{
			pNewDesc->m_nID = atoi(szAttrValue);
		}
		else if (!_stricmp(szAttrName, "name"))
		{
			strcpy_s(pNewDesc->m_pMItemName->Ref().m_szItemName, MGetStringResManager()->GetStringFromXml(szAttrValue));
			pNewDesc->m_pMItemName->MakeCrc();
		}
		else if (!_stricmp(szAttrName, "type"))
		{
			if (!_stricmp(szAttrValue, "melee"))			pNewDesc->m_nType.Set(MMIT_MELEE);
			else if (!_stricmp(szAttrValue, "range"))		pNewDesc->m_nType.Set(MMIT_RANGE);
			else if (!_stricmp(szAttrValue, "equip"))		pNewDesc->m_nType.Set(MMIT_EQUIPMENT);
			else if (!_stricmp(szAttrValue, "custom"))		pNewDesc->m_nType.Set(MMIT_CUSTOM);
			else if( !_stricmp(szAttrValue, "ticket") )	pNewDesc->m_nType.Set(MMIT_TICKET);
			else if( !_stricmp(szAttrValue, "avatar") )	pNewDesc->m_nType.Set(MMIT_AVATAR);
			else if( !_stricmp(szAttrValue, "community") )	pNewDesc->m_nType.Set(MMIT_COMMUNITY);
			else if( !_stricmp(szAttrValue, "longbuff") )	pNewDesc->m_nType.Set(MMIT_LONGBUFF);
			else _ASSERT(0);

			pNewDesc->m_nType.MakeCrc();
		}
		else if (!_stricmp(szAttrName, "res_sex"))
		{
			if (!_stricmp(szAttrValue, "m"))		pNewDesc->m_nResSex.Set(0);
			else if (!_stricmp(szAttrValue, "f"))	pNewDesc->m_nResSex.Set(1);
			else if (!_stricmp(szAttrValue, "a"))	pNewDesc->m_nResSex.Set(-1);
			else _ASSERT(0);
			pNewDesc->m_nResSex.MakeCrc();
		}
		else if (!_stricmp(szAttrName, "res_level"))
		{
			pNewDesc->m_nResLevel.Set( atoi(szAttrValue));
			pNewDesc->m_nResLevel.MakeCrc();
		}
		else if (!_stricmp(szAttrName, "slot"))
		{
			// 슬롯 타입
			if (!_stricmp(szAttrValue, "none"))			pNewDesc->m_nSlot = MMIST_NONE;
			else if (!_stricmp(szAttrValue, "melee"))		pNewDesc->m_nSlot = MMIST_MELEE;
			else if (!_stricmp(szAttrValue, "range"))		pNewDesc->m_nSlot = MMIST_RANGE;
			else if (!_stricmp(szAttrValue, "custom"))		pNewDesc->m_nSlot = MMIST_CUSTOM;
			else if (!_stricmp(szAttrValue, "head"))		pNewDesc->m_nSlot = MMIST_HEAD;
			else if (!_stricmp(szAttrValue, "chest"))		pNewDesc->m_nSlot = MMIST_CHEST;
			else if (!_stricmp(szAttrValue, "hands"))		pNewDesc->m_nSlot = MMIST_HANDS;
			else if (!_stricmp(szAttrValue, "legs"))		pNewDesc->m_nSlot = MMIST_LEGS;
			else if (!_stricmp(szAttrValue, "feet"))		pNewDesc->m_nSlot = MMIST_FEET;
			else if (!_stricmp(szAttrValue, "finger"))		pNewDesc->m_nSlot = MMIST_FINGER;
			else if (!_stricmp(szAttrValue, "extra"))		pNewDesc->m_nSlot = MMIST_EXTRA;
			else if (!_stricmp(szAttrValue, "avatar"))		pNewDesc->m_nSlot = MMIST_AVATAR;
			else if (!_stricmp(szAttrValue, "community")) 	pNewDesc->m_nSlot = MMIST_COMMUNITY;
			else if (!_stricmp(szAttrValue, "longbuff")) 	pNewDesc->m_nSlot = MMIST_LONGBUFF;
			///TODO: this is for blitzkrieg, do i want to use it or not?
			else if (!_stricmp(szAttrValue, "skill"))			pNewDesc->m_nSlot = MMIST_NONE;
			else if (!_stricmp(szAttrValue, "dash")) pNewDesc->m_nSlot = MMIST_DASH;
			else
			{
				mlog("Error with equipment slot\n");
				_ASSERT(0);
			}
		}
		else if (!_stricmp(szAttrName, "weapon"))
		{
			// 무기 타입
			if (strlen(szAttrValue) <= 0)								pNewDesc->m_nWeaponType.Set(MWT_NONE);
			else if (!_stricmp(szAttrValue, "dagger"))		pNewDesc->m_nWeaponType.Set(MWT_DAGGER);
			else if (!_stricmp(szAttrValue, "dualdagger"))	pNewDesc->m_nWeaponType.Set(MWT_DUAL_DAGGER);
			else if (!_stricmp(szAttrValue, "katana"))		pNewDesc->m_nWeaponType.Set(MWT_KATANA);
			else if (!_stricmp(szAttrValue, "greatsword"))	pNewDesc->m_nWeaponType.Set(MWT_GREAT_SWORD);
			else if (!_stricmp(szAttrValue, "doublekatana"))	pNewDesc->m_nWeaponType.Set(MWT_DOUBLE_KATANA);
			else if (!_stricmp(szAttrValue, "spycase")) pNewDesc->m_nWeaponType.Set(MWT_SPYCASE);

			else if (!_stricmp(szAttrValue, "pistol"))		pNewDesc->m_nWeaponType.Set(MWT_PISTOL);
			else if (!_stricmp(szAttrValue, "pistolx2"))		pNewDesc->m_nWeaponType.Set(MWT_PISTOLx2);
			else if (!_stricmp(szAttrValue, "revolver"))		pNewDesc->m_nWeaponType.Set(MWT_REVOLVER);
			else if (!_stricmp(szAttrValue, "revolverx2"))	pNewDesc->m_nWeaponType.Set(MWT_REVOLVERx2);
			else if (!_stricmp(szAttrValue, "smg"))			pNewDesc->m_nWeaponType.Set(MWT_SMG);
			else if (!_stricmp(szAttrValue, "smgx2"))		pNewDesc->m_nWeaponType.Set(MWT_SMGx2);
			else if (!_stricmp(szAttrValue, "shotgun"))		pNewDesc->m_nWeaponType.Set(MWT_SHOTGUN);
			else if (!_stricmp(szAttrValue, "sawedshotgun"))	pNewDesc->m_nWeaponType.Set(MWT_SAWED_SHOTGUN);
			else if (!_stricmp(szAttrValue, "rifle"))		pNewDesc->m_nWeaponType.Set(MWT_RIFLE);
			else if (!_stricmp(szAttrValue, "machinegun"))	pNewDesc->m_nWeaponType.Set(MWT_MACHINEGUN);
			else if (!_stricmp(szAttrValue, "rocket"))		pNewDesc->m_nWeaponType.Set(MWT_ROCKET);
			else if (!_stricmp(szAttrValue, "snifer"))		pNewDesc->m_nWeaponType.Set(MWT_SNIFER);

			else if (!_stricmp(szAttrValue, "medkit"))		pNewDesc->m_nWeaponType.Set(MWT_MED_KIT);
			else if (!_stricmp(szAttrValue, "repairkit"))	pNewDesc->m_nWeaponType.Set(MWT_REPAIR_KIT);
			else if (!_stricmp(szAttrValue, "bulletkit"))	pNewDesc->m_nWeaponType.Set(MWT_BULLET_KIT);
			else if (!_stricmp(szAttrValue, "flashbang"))	pNewDesc->m_nWeaponType.Set(MWT_FLASH_BANG);
			else if (!_stricmp(szAttrValue, "frag"))			pNewDesc->m_nWeaponType.Set(MWT_FRAGMENTATION);
			else if (!_stricmp(szAttrValue, "smoke"))		pNewDesc->m_nWeaponType.Set(MWT_SMOKE_GRENADE);
			else if (!_stricmp(szAttrValue, "stungrenade")) pNewDesc->m_nWeaponType.Set(MWT_STUNGRENADE);
			else if (!_stricmp(szAttrValue, "skill"))		pNewDesc->m_nWeaponType.Set(MWT_SKILL);
			else if (!_stricmp(szAttrValue, "food"))			pNewDesc->m_nWeaponType.Set(MWT_FOOD);

			// enchant
			else if (!_stricmp(szAttrValue, "enchant_fire"))			pNewDesc->m_nWeaponType.Set(MWT_ENCHANT_FIRE);
			else if (!_stricmp(szAttrValue, "enchant_cold"))			pNewDesc->m_nWeaponType.Set(MWT_ENCHANT_COLD);
			else if (!_stricmp(szAttrValue, "enchant_lightning"))	pNewDesc->m_nWeaponType.Set(MWT_ENCHANT_LIGHTNING);
			else if (!_stricmp(szAttrValue, "enchant_poison"))		pNewDesc->m_nWeaponType.Set(MWT_ENCHANT_POISON);

			// 소모성
			else if (!_stricmp(szAttrValue, "potion"))			pNewDesc->m_nWeaponType.Set(MWT_POTION);
			else if (!_stricmp(szAttrValue, "trap"))				pNewDesc->m_nWeaponType.Set(MWT_TRAP);
			else if (!_stricmp(szAttrValue, "dynamite"))			pNewDesc->m_nWeaponType.Set(MWT_DYNAMITYE);
			else if (!_stricmp(szAttrValue, "landmine_spy")) pNewDesc->m_nWeaponType.Set(MWT_LANDMINE);
			else _ASSERT(0);

			pNewDesc->m_nWeaponType.MakeCrc();
		}		
		else if (!_stricmp(szAttrName, "spendtype"))
		{
			if (strlen(szAttrValue) <= 0)									pNewDesc->m_nSpendType.Set(MMCT_NONE);
			else if (!_stricmp(szAttrValue, "normal"))		pNewDesc->m_nSpendType.Set(MMCT_NORMAL);
			else if (!_stricmp(szAttrValue, "longbuff"))		pNewDesc->m_nSpendType.Set(MMCT_LONGBUFF);
			else if (!_stricmp(szAttrValue, "shortbuff"))	pNewDesc->m_nSpendType.Set(MMCT_SHORTBUFF);
			else if (!_stricmp(szAttrValue, "community"))	pNewDesc->m_nSpendType.Set(MMCT_COMMUNITY);

			pNewDesc->m_nSpendType.MakeCrc();
		}
		else if (!_stricmp(szAttrName, "effect_level"))
		{
			// 이펙트레벨
			pNewDesc->m_nEffectLevel.Set( atoi(szAttrValue));
			pNewDesc->m_nEffectLevel.MakeCrc();
		}
		else if (!_stricmp(szAttrName, "weight"))
		{
			pNewDesc->m_nWeight.Set( atoi(szAttrValue));
			pNewDesc->m_nWeight.MakeCrc();
		}
		else if (!_stricmp(szAttrName, "bt_price"))
		{
			pNewDesc->m_nBountyPrice.Set( atoi(szAttrValue));
			pNewDesc->m_nBountyPrice.MakeCrc();
		}
		else if (!_stricmp(szAttrName, "damage"))
		{
			pNewDesc->m_nDamage.Set( atoi(szAttrValue));
			pNewDesc->m_nDamage.MakeCrc();
		}
		else if (!_stricmp(szAttrName, "itempower"))
		{
			pNewDesc->m_nItemPower.Set( atoi(szAttrValue));
			pNewDesc->m_nItemPower.MakeCrc();
		}
		else if (!_stricmp(szAttrName, "damagetype"))
		{
			if (strlen(szAttrValue) <= 0)									pNewDesc->m_nDamageType.Set(MMDT_NORMAL);
			else if (!_stricmp(szAttrValue, "fire"))		pNewDesc->m_nDamageType.Set(MMDT_FIRE);
			else if (!_stricmp(szAttrValue, "cold"))		pNewDesc->m_nDamageType.Set(MMDT_COLD);
			else if (!_stricmp(szAttrValue, "posion"))		pNewDesc->m_nDamageType.Set(MMDT_POISION);
			else if (!_stricmp(szAttrValue, "lightning"))	pNewDesc->m_nDamageType.Set(MMDT_LIGHTING);
			else if (!_stricmp(szAttrValue, "heal"))		pNewDesc->m_nDamageType.Set(MMDT_HEAL);
			else if (!_stricmp(szAttrValue, "repair"))		pNewDesc->m_nDamageType.Set(MMDT_REPAIR);
			else if (!_stricmp(szAttrValue, "haste"))		pNewDesc->m_nDamageType.Set(MMDT_HASTE);

			pNewDesc->m_nDamageType.MakeCrc();
		}
		else if (!_stricmp(szAttrName, "damagetime"))
		{
			pNewDesc->m_nDamageTime.Set( atoi(szAttrValue));
			pNewDesc->m_nDamageTime.MakeCrc();
		}
		else if (!_stricmp(szAttrName, "lifetime"))
		{
			pNewDesc->m_nLifeTime.Set( atoi(szAttrValue));
			pNewDesc->m_nLifeTime.MakeCrc();
		}
		else if (!_stricmp(szAttrName, "iscashitem"))
		{
			if (!_stricmp(szAttrValue, "true")) 	pNewDesc->m_bIsCashItem = true;
			else										pNewDesc->m_bIsCashItem = false;
		}
		else if (!_stricmp(szAttrName, "effect_id"))
		{
			pNewDesc->m_nEffectId = (MMatchItemEffectId)atoi(szAttrValue);
		}
		else if (!_stricmp(szAttrName, "delay"))
		{
			pNewDesc->m_nDelay.Set( atoi(szAttrValue));
			pNewDesc->m_nDelay.MakeCrc();
		}
		else if (!_stricmp(szAttrName, "ctrl_ability"))
		{
			pNewDesc->m_nControllability.Set( atoi(szAttrValue));
			pNewDesc->m_nControllability.MakeCrc();
		}
		else if (!_stricmp(szAttrName, "magazine"))
		{
			pNewDesc->m_nMagazine.Set( atoi(szAttrValue));
			pNewDesc->m_nMagazine.MakeCrc();
		}
		else if (!_stricmp(szAttrName, "maxbullet"))
		{
			pNewDesc->m_nMaxBullet.Set( atoi(szAttrValue));
			pNewDesc->m_nMaxBullet.MakeCrc();
		}
		else if (!_stricmp(szAttrName, "reloadtime"))
		{
			pNewDesc->m_nReloadTime.Set( atoi(szAttrValue));
			pNewDesc->m_nReloadTime.MakeCrc();
		}
		else if (!_stricmp(szAttrName, "slug_output"))
		{
			if (!_stricmp(szAttrValue, "true")) pNewDesc->m_bSlugOutput = true;
			else									pNewDesc->m_bSlugOutput = false;
		}
		else if (!_stricmp(szAttrName, "hp"))
		{
			//pNewDesc->m_nHP = atoi(szAttrValue);
			pNewDesc->m_nHP.Set( atoi(szAttrValue));
			pNewDesc->m_nHP.MakeCrc();
		}
		else if (!_stricmp(szAttrName, "ap"))
		{
			pNewDesc->m_nAP.Set( atoi(szAttrValue));
			pNewDesc->m_nAP.MakeCrc();
		}
		else if (!_stricmp(szAttrName, "maxwt"))
		{
			pNewDesc->m_nMaxWT.Set( atoi(szAttrValue));
			pNewDesc->m_nMaxWT.MakeCrc();
		}
		else if (!_stricmp(szAttrName, "sf"))
		{
			pNewDesc->m_nSF.Set( atoi(szAttrValue));
			pNewDesc->m_nSF.MakeCrc();
		}
		else if (!_stricmp(szAttrName, "fr"))
		{
			pNewDesc->m_nFR.Set( atoi(szAttrValue));
			pNewDesc->m_nFR.MakeCrc();
		}
		else if (!_stricmp(szAttrName, "cr"))
		{
			pNewDesc->m_nCR.Set( atoi(szAttrValue));
			pNewDesc->m_nCR.MakeCrc();
		}
		else if (!_stricmp(szAttrName, "pr"))
		{
			pNewDesc->m_nPR.Set( atoi(szAttrValue));
			pNewDesc->m_nPR.MakeCrc();
		}
		else if (!_stricmp(szAttrName, "lr"))
		{
			pNewDesc->m_nLR.Set( atoi(szAttrValue));
			pNewDesc->m_nLR.MakeCrc();
		}
		else if (!_stricmp(szAttrName, "limitspeed"))
		{
			pNewDesc->m_nLimitSpeed.Set( atoi(szAttrValue));
			pNewDesc->m_nLimitSpeed.MakeCrc();
		}
		else if (!_stricmp(szAttrName, "limitjump"))
		{
			pNewDesc->m_nLimitJump.Set( atoi(szAttrValue));
			pNewDesc->m_nLimitJump.MakeCrc();
		}
		else if (!_stricmp(szAttrName, "limittumble"))
		{
			pNewDesc->m_nLimitTumble.Set( atoi(szAttrValue));
			pNewDesc->m_nLimitTumble.MakeCrc();
		}
		else if (!_stricmp(szAttrName, "limitwall"))
		{
			pNewDesc->m_nLimitWall.Set( atoi(szAttrValue));
			pNewDesc->m_nLimitWall.MakeCrc();
		}
		else if (!_stricmp(szAttrName, "range"))
		{
			pNewDesc->m_nRange.Set( atoi(szAttrValue));
			pNewDesc->m_nRange.MakeCrc();
		}
		else if (!_stricmp(szAttrName, "angle"))
		{
			pNewDesc->m_nAngle.Set( atoi(szAttrValue));
			pNewDesc->m_nAngle.MakeCrc();
		}
		else if (!_stricmp(szAttrName, "color"))
		{
			// color 처리해야 함..-_-z
		}
		else if (!_stricmp(szAttrName, "desc"))
		{
		strcpy_s(pNewDesc->m_szDesc, MGetStringResManager()->GetStringFromXml(szAttrValue));
		}
		else if (!_stricmp(szAttrName, "mesh_name"))
		{
		strcpy_s(pNewDesc->m_pMItemName->Ref().m_szMeshName, szAttrValue);
			pNewDesc->m_pMItemName->MakeCrc();
		}
		else if (!_stricmp(szAttrName, "image_id"))
		{
			pNewDesc->m_nImageID = atoi(szAttrValue);
		}
		else if (!_stricmp(szAttrName, "bullet_image_id"))
		{
			pNewDesc->m_nBulletImageID = atoi(szAttrValue);
		}
		else if (!_stricmp(szAttrName, "magazine_image_id"))
		{
			pNewDesc->m_nMagazineImageID = atoi(szAttrValue);
		}
		else if (!_stricmp(szAttrName, "snd_reload"))
		{
		strcpy_s(pNewDesc->m_szReloadSndName, szAttrValue);
		}
		else if (!_stricmp(szAttrName, "snd_fire"))
		{
		strcpy_s(pNewDesc->m_szFireSndName, szAttrValue);
		}
		else if (!_stricmp(szAttrName, "snd_dryfire"))
		{
		strcpy_s(pNewDesc->m_szDryfireSndName, szAttrValue);
		}
		else if (!_stricmp(szAttrName, "snd_weapon_fiber"))
		{
		strcpy_s(pNewDesc->m_szWeaponByFiber, szAttrValue);
		}
		else if (!_stricmp(szAttrName, "gadget_id"))
		{
			pNewDesc->m_nGadgetID.Set( atoi(szAttrValue));
			pNewDesc->m_nGadgetID.MakeCrc();
		}
		else if (!_stricmp(szAttrName, "xp_solo_bonus"))
		{
			pNewDesc->m_Bonus.m_fXP_SoloBonus = (float)(atoi(szAttrValue)) / 100.0f;
		}
		else if (!_stricmp(szAttrName, "xp_team_bonus"))
		{
			pNewDesc->m_Bonus.m_fXP_TeamBonus = (float)(atoi(szAttrValue)) / 100.0f;
		}
		else if (!_stricmp(szAttrName, "xp_quest_bonus"))
		{
			pNewDesc->m_Bonus.m_fXP_QuestBonus = (float)(atoi(szAttrValue)) / 100.0f;
		}
		else if( !_stricmp(szAttrName, "bp_solo_bonus") )
		{
			pNewDesc->m_Bonus.m_fBP_SoloBonus = static_cast< float >( atoi(szAttrValue) ) / 100.0f;
		}
		else if( !_stricmp(szAttrName, "bp_team_bonus") )
		{
			pNewDesc->m_Bonus.m_fBP_TeamBonus = static_cast< float >( atoi(szAttrValue) ) / 100.0f;
		}
		else if( !_stricmp(szAttrName, "bp_quest_bonus") )
		{
			pNewDesc->m_Bonus.m_fBP_QuestBonus = static_cast< float >( atoi(szAttrValue) ) / 100.0f;
		}
		else if( !_stricmp(szAttrName, "spendable") )
		{
			if( 0 == _stricmp("false", szAttrValue) )	pNewDesc->m_bIsSpendableItem = false;
			else											pNewDesc->m_bIsSpendableItem = true;
		}
		else if( !_stricmp(szAttrName, "ticket_type") )
		{
			if( 0 == _stricmp(szAttrValue, "admission") )
				pNewDesc->m_TicketType = MMTT_ADMISSION;
			else if( 0 == _stricmp(szAttrValue, "change_head") )
				pNewDesc->m_TicketType = MMIT_CHANGEHEAD;
			else {
				ASSERT( 0 && "머야? 이 종이는 머야?" );
			}
		}
		else if (!_stricmp(szAttrName, "rent_period"))
		{
			pNewDesc->m_nMaxRentPeriod.Set( atoi(szAttrValue));
			pNewDesc->m_nMaxRentPeriod.MakeCrc();
		}
		// 여기서부터는 아바타 관련된 것들... 추후에는 따로 XML로 관리하고 싶은데-_ㅠ
		else if (!_stricmp(szAttrName, "avatar_head"))
		{
		strcpy_s(pNewDesc->m_pAvatarMeshName->Ref().m_szHeadMeshName
				, MGetStringResManager()->GetStringFromXml(szAttrValue));
			pNewDesc->m_pAvatarMeshName->MakeCrc();
		}
		else if (!_stricmp(szAttrName, "avatar_chest"))
		{
		strcpy_s(pNewDesc->m_pAvatarMeshName->Ref().m_szChestMeshName
				, MGetStringResManager()->GetStringFromXml(szAttrValue));
			pNewDesc->m_pAvatarMeshName->MakeCrc();
		}
		else if (!_stricmp(szAttrName, "avatar_hand"))
		{
		strcpy_s(pNewDesc->m_pAvatarMeshName->Ref().m_szHandMeshName
				, MGetStringResManager()->GetStringFromXml(szAttrValue));
			pNewDesc->m_pAvatarMeshName->MakeCrc();			
		}
		else if (!_stricmp(szAttrName, "avatar_legs"))
		{
		strcpy_s(pNewDesc->m_pAvatarMeshName->Ref().m_szLegsMeshName
				, MGetStringResManager()->GetStringFromXml(szAttrValue));
			pNewDesc->m_pAvatarMeshName->MakeCrc();
		}
		else if (!_stricmp(szAttrName, "avatar_feet"))
		{
		strcpy_s(pNewDesc->m_pAvatarMeshName->Ref().m_szFeetMeshName
				, MGetStringResManager()->GetStringFromXml(szAttrValue));
			pNewDesc->m_pAvatarMeshName->MakeCrc();			
		}
	}

	if( pNewDesc->IsSpendableItem() && pNewDesc->m_nSpendType.Ref() == MMCT_NONE ) {
		_ASSERT(0);
		return false;
	}

	if( pNewDesc->m_nSpendType.Ref() == MMCT_NORMAL ) 
	{
		switch(pNewDesc->m_nWeaponType.Ref())
		{
		case MWT_POTION:
		case MWT_TRAP:
		case MWT_DYNAMITYE:
		case MWT_STUNGRENADE:
			break;
			
		default:
			_ASSERT(0);
			return false;
		}
	}

	iterator tempitor = find(pNewDesc->m_nID);
	if (tempitor != end()) {
		_ASSERT(0);		///< 같은 ID의 아이템이 존재한다.
		return false;
	}

	insert(value_type(pNewDesc->m_nID, pNewDesc));
	return true;
}


MMatchItemDescMgr* MMatchItemDescMgr::GetInstance()
{
	static MMatchItemDescMgr m_ItemDescMgr;
	return &m_ItemDescMgr;
}

void MMatchItemDescMgr::ShiftMemoryGradually()
{
	// MemoryFugitive는 SetData할때만 위치가 바뀐다. 아이템 속성값은 최초에 한번만 SetData 하므로
	// 결과적으로 위치가 바뀌지 않게 된다. 매프레임마다 아이템 속성치를 수동으로 메모리 위치 변경한다
	// 성능 부하가 염려되어 여러프레임에 걸쳐 아이템 몇개씩 차례로 옮겨준다

	iterator it = find(m_nextItemIdToMemoryShift);
	if (it == end())
		it = begin();

	const int NUM_SHIFT_EACH_FRAME = 8;
	MMatchItemDesc* pItemDesc;

	for (int i=0; i<NUM_SHIFT_EACH_FRAME; ++i)
	{
		pItemDesc = it->second;
		pItemDesc->ShiftFugitiveValues();

		++it;
		if (it == end())
			it = begin();
	}

	m_nextItemIdToMemoryShift = it->first;
}

///////////////////////////////////////////////////////////////////////////////
// MMatchItem /////////////////////////////////////////////////////////////////
MMatchItem::MMatchItem() : MBaseItem(), m_nCIID(0), m_pDesc(NULL), m_bEquiped(false), m_nRentItemRegTime(0)
{
	m_nCountOfNonDestroyItem = 0;
	m_nUseCountOfNonDestroyItem = 0;
	m_nItemID = 0;
}

MMatchItem::~MMatchItem()
{

}

bool MMatchItem::Create( const MUID& uid, MMatchItemDesc* pDesc, const WORD nCountOfNonDesctroyItem, int nCount)
{
	if( NULL == pDesc ) {
		m_pDesc = NULL;
		return false;
	}

	m_uidItem					= uid;
	m_nCount					= nCount;
	m_pDesc						= pDesc;
	m_nCountOfNonDestroyItem	= nCountOfNonDesctroyItem;
	m_nItemID					= pDesc->m_nID;

	m_CharItemCachingData.Reset();

	return true;
}

void MMatchItem::Destroy()
{
	m_pDesc = NULL;
	m_nCount = 0;
	m_uidItem = MUID(0,0);
}

MMatchItemType MMatchItem::GetItemType()
{
//	_ASSERT(m_pDesc != NULL);
	if (m_pDesc == NULL) return MMIT_MELEE;

	return m_pDesc->m_nType.Ref();
}


void MMatchItem::IncreaseUseCountOfNonDestroyItem()
{ 
	if( m_nCountOfNonDestroyItem > m_nUseCountOfNonDestroyItem) 
		++m_nUseCountOfNonDestroyItem; 
}


MMatchItemDesc* MMatchItem::GetDesc() const
{
	if( NULL == m_pDesc )
	{
		// NHN테스트를 하기 우해서. 
		// 이 로그는 클라이언트에도 남기때문에 빌드 후 주석 처리함.
		// mlog( "ItemDesc is null : %d(CIID:%u)\n", m_nItemID, m_nCIID );
		return NULL;
	}

	return m_pDesc;
}

unsigned long int MMatchItem::GetDescID() const							
{ 
	if( NULL == m_pDesc )
	{
		// NHN테스트를 하기 우해서. 
		// 이 로그는 클라이언트에도 남기때문에 빌드 후 주석 처리함.
		// mlog( "ItemDesc is null : %d(CIID:%u)\n", m_nItemID, m_nCIID );
		return 0;
	}

	return m_pDesc->m_nID; 
}

void MMatchItem::IncreaseCount(int nVal)
{
	SetItemCount(m_nCount + nVal);
}

void MMatchItem::DecreaseCount(int nVal)
{
	SetItemCount(m_nCount - nVal);
}

// 소모성 아이템 사용할 때만 사용
void MMatchItem::DecreaseCountWithCaching(int nVal)
{
	_ASSERT(nVal >= 0);
	m_CharItemCachingData.DecCnt(nVal);
	DecreaseCount(nVal);
}

///////////////////////////////////////////////////////////////////////////////
// MMatchItemMap //////////////////////////////////////////////////////////////
MMatchItemMap::MMatchItemMap()
{
	m_bDoneDbAccess = false;	
	m_bHasRentItem = false;
}

MMatchItemMap::~MMatchItemMap()
{
	// Clear();
}

bool MMatchItemMap::CreateItem( const MUID& uid
							   , int nCIID
							   , int nItemDescID
							   , bool bRentItem
							   , DWORD dwRentMinutePeriodRemainder
							   , const WORD wRentHourPeriod
							   , int nCount)
{
	MMatchItemDesc* pDesc = NULL;
	pDesc = MGetMatchItemDescMgr()->GetItemDesc(nItemDescID);
	
	if (pDesc == NULL) {
		_ASSERT(0);
		return false;
	}

	MMatchItem* pNewItem = new MMatchItem();
	if (pNewItem == NULL) {
		_ASSERT(0);
		return false;
	}

	pNewItem->Create( uid, pDesc, pDesc->m_nMagazine.Ref() );
	pNewItem->SetCIID( nCIID );
	pNewItem->SetRentItem( dwRentMinutePeriodRemainder );
	pNewItem->SetRentItemRegTime( MMatchServer::GetInstance()->GetTickTime() );
	pNewItem->SetRentHourPeriod( wRentHourPeriod );
	pNewItem->SetItemCount(nCount);
	
	insert(value_type(uid, pNewItem));

	if (bRentItem) m_bHasRentItem = true;

	return true;
}

int	MMatchItemMap::GetItemCount(const MUID& uidItem)
{
	iterator itor = find(uidItem);
	if (itor != end())
	{
		MMatchItem* pItem = (*itor).second;
		return pItem->GetItemCount();
	}

	return -1;
}

bool MMatchItemMap::RemoveItem(const MUID& uidItem)
{
	iterator itor = find(uidItem);
	if (itor != end())
	{
		MMatchItem* pItem = (*itor).second;
		delete pItem; pItem = NULL;
		erase(itor);
	}
	else
	{
		return false;
	}

	return true;
}

MMatchItem* MMatchItemMap::GetItem(const MUID& uidItem) const
{
	const_iterator itor = find(uidItem);
	if (itor != end())
	{
		return (*itor).second;
	}

	return NULL;
}

MMatchItem* MMatchItemMap::GetItemByItemID(const DWORD dwItemID) const
{
	const_iterator It, End;
	for( It = begin(), End = end(); It != End; ++It ) {
		if( dwItemID == It->second->GetDescID() )
			return It->second;
	}

	return NULL;
}

MMatchItem* MMatchItemMap::GetItemByCIID(const DWORD dwCIID) const
{
	const_iterator It, End;
	for( It = begin(), End = end(); It != End; ++It ) {
		if( dwCIID == It->second->GetCIID() )
			return It->second;
	}

	return NULL;
}

void MMatchItemMap::Clear()
{	
	m_bDoneDbAccess = false;
	m_bHasRentItem = false;

	MMatchItemMap::const_iterator	itEnd	= end();
	MMatchItem*						pItem	= NULL;

	for( MMatchItemMap::iterator Iter = begin(); itEnd != Iter; ++Iter )
	{
		pItem = Iter->second;
		delete pItem;
		pItem = NULL;
	}

	clear();
}


const bool MMatchItemMap::IsHave( const DWORD dwItemID ) const
{
	const_iterator It, End;
	for( It = begin(), End = end(); It != End; ++It )
	{
		if( dwItemID == It->second->GetDescID() )
			return true;
	}

	return false;
}


///////////////////////////////////////////////////////////////////////////////
// MMatchEquipedItem //////////////////////////////////////////////////////////
//bool MMatchEquipedItem::SetItem(MMatchCharItemParts parts, MMatchItem* pMatchItem)
//{
//	if (m_pParts[parts] != NULL)
//	{
//		m_pParts[parts]->SetEquiped(false);
//	}
//
//	m_pParts[parts] = pMatchItem;
//	pMatchItem->SetEquiped(true);
//
//	return true;
//}


bool MMatchEquipedItem::SetItem( MMatchCharItemParts parts, const MUID& uidItem, MMatchItem* pItem )
{
	if( NULL == pItem ) {
		MGetMatchServer()->Log(7, "Error, item is null");
		return false;
	}

	if( pItem->IsEquiped() ) {
		MGetMatchServer()->LOG(7, "Error, item is somehow already equipped");
		return false;
	}
	
	// parts를 파라미터로 넘겨도 되지만, 코드의 명확성을 위하여!
	MMatchCharItemParts outParts = MMCIP_END;
	if( IsEquipedItem(uidItem, outParts) ) {
		MGetMatchServer()->LOG(7, "Error, item is somehow already equipped");
		return false;
	}

	MMatchItem* pOldItem = GetItem( parts );
	if( NULL != pOldItem ) {
		MGetMatchServer()->LOG(7, "Error, old item was not null");
		pOldItem->SetEquiped( false );
	}

	pItem->SetEquiped( true );
	m_uidParts[ parts ] = uidItem;

	return true;
}


void MMatchEquipedItem::GetTotalWeight(int* poutWeight, int* poutMaxWeight)
{
	int weight = 0;
	int maxwt = 0;
	MMatchItem* pItem = NULL;

	for( int i = 0; i < MMCIP_END; i++)
	{
		pItem = GetItem( MMatchCharItemParts(i) );
		if( NULL == pItem || NULL == pItem->GetDesc() )
		{
			continue;
		}

		weight += pItem->GetDesc()->m_nWeight.Ref();
		maxwt += pItem->GetDesc()->m_nMaxWT.Ref();
	}

	*poutWeight = weight;
	*poutMaxWeight = maxwt;

	//for (int i = 0; i < MMCIP_END; i++)
	//{
	//	if (m_pParts[i] != NULL)
	//	{
	//		weight += (m_pParts[i]->GetDesc()->m_nWeight);
	//		maxwt += (m_pParts[i]->GetDesc()->m_nMaxWT);
	//	}
	//}
	//
	//*poutWeight = weight;
	//*poutMaxWeight = maxwt;
}


//MMatchItem* MMatchEquipedItem::GetMyItem( MMatchCharItemParts parts )
//{
//	if( NULL == m_pOwner )
//	{
//		return NULL;
//	}
//
//	return m_pOwner->m_ItemList.GetItem( m_uidParts[i] );
//}


MMatchItem* MMatchEquipedItem::GetItem(MMatchCharItemParts parts)
{
	if( NULL == m_pOwner )
	{
		return NULL;
	}

	return m_pOwner->m_ItemList.GetItem( m_uidParts[parts] );
}


void MMatchEquipedItem::Remove(MMatchCharItemParts parts)
{
	MMatchItem* pItem = GetItem( parts );
	if( NULL == pItem )
	{
		return;
	}

	pItem->SetEquiped( false );
	m_uidParts[ parts ] = MUID( 0, 0 );



	//if (m_pParts[parts] != NULL) m_pParts[parts]->SetEquiped(false);
	//m_pParts[parts] = NULL;
}

void MMatchEquipedItem::Remove(const MUID& uidParts)
{
	MMatchItem* pItem = NULL;
	for( int i = 0; i < MMCIP_END; ++i ) {
		if( m_uidParts[i] == uidParts ) {
			pItem = GetItem(MMatchCharItemParts(i));

			pItem->SetEquiped(false);
			m_uidParts[i] = MUID( 0, 0 );
			return;
		}
	}

	if( pItem == NULL ) {
		_ASSERT(0);		
	}
}

void MMatchEquipedItem::Clear()
{
	for( int i = 0; i < MMCIP_END; ++i )
	{
		if( MUID(0, 0) != m_uidParts[ i ] )
		{
			MMatchItem* pItem = GetItem( MMatchCharItemParts(i) );
			if( NULL != pItem )
			{
				pItem->SetEquiped( false );
			}
		}

		m_uidParts[ i ] = MUID( 0, 0 );
	}
	


	//for (int i = 0; i < MMCIP_END; i++)
	//{
	//	if (m_pParts[i] != NULL)
	//	{
	//		m_pParts[i]->SetEquiped(false);
	//	}
	//	m_pParts[i] = NULL;
	//}
}


//bool MMatchEquipedItem::IsEquipedItem(MMatchItem* pCheckItem, MMatchCharItemParts& outParts)
//{
//	if (pCheckItem == NULL) return false;
//	
//	for (int i = 0; i < MMCIP_END; i++)
//	{
//		if (m_pParts[i] == pCheckItem)
//		{
//			outParts = MMatchCharItemParts(i);
//			return true;
//		}
//	}
//
//	return false;
//}


bool MMatchEquipedItem::IsEquipedItem(const MUID& uidItem, MMatchCharItemParts& outParts) // 해당 아이템이 장비중인지 체크
{
	for( int i = 0; i < MMCIP_END; ++i )
	{
		if( uidItem == m_uidParts[i] )
		{
			outParts = MMatchCharItemParts( i );
			return true;
		}
	}

	return false;
}


bool MMatchEquipedItem::IsEmpty(MMatchCharItemParts parts) 
{ 
	if( MUID(0, 0) != m_uidParts[parts] )
	{
		return false;
	}
	
	return true;
}


bool IsSuitableItemSlot(MMatchItemSlotType nSlotType, MMatchCharItemParts nParts)
{
	if (nSlotType == MMIST_MELEE)
	{
		if (nParts == MMCIP_MELEE) return true;
		else return false;
	}
	else if (nSlotType == MMIST_RANGE)
	{
		if ((nParts == MMCIP_PRIMARY) || (nParts == MMCIP_SECONDARY)) return true;
		else return false;
	}
	else if (nSlotType == MMIST_CUSTOM)
	{
		if ((nParts == MMCIP_CUSTOM1) || (nParts == MMCIP_CUSTOM2)  || (nParts == MMCIP_CUSTOM3)) return true;
		else return false;
	}
	else if (nSlotType == MMIST_HEAD)
	{
		if (nParts == MMCIP_HEAD) return true;
		else return false;
	}
	else if (nSlotType == MMIST_CHEST)
	{
		if (nParts == MMCIP_CHEST) return true;
		else return false;
	}
	else if (nSlotType == MMIST_HANDS)
	{
		if (nParts == MMCIP_HANDS) return true;
		else return false;
	}
	else if (nSlotType == MMIST_LEGS)
	{
		if (nParts == MMCIP_LEGS) return true;
		else return false;
	}
	else if (nSlotType == MMIST_FEET)
	{
		if (nParts == MMCIP_FEET) return true;
		else return false;
	}
	else if (nSlotType == MMIST_FINGER)
	{
		if ((nParts == MMCIP_FINGERL) || (nParts == MMCIP_FINGERR)) return true;
		else return false;
	}
	else if( nSlotType == MMIST_AVATAR )
	{
		if(nParts == MMCIP_AVATAR) return true;
		else return false;
	}
	else if( nSlotType == MMIST_COMMUNITY )
	{
		if((nParts == MMCIP_COMMUNITY1) || (nParts == MMCIP_COMMUNITY2)) return true;
		else return false;
	}
	else if( nSlotType == MMIST_LONGBUFF )
	{
		if((nParts == MMCIP_LONGBUFF1) || (nParts == MMCIP_LONGBUFF2)) return true;
		else return false;
	}
	else if (nSlotType == MMIST_DASH)
	{
		if (nParts == MMCIP_DASH) return true;
		else
			return false;
	}

	return false;
};

MMatchCharItemParts GetSuitableItemParts(MMatchItemSlotType nSlotType)
{
	switch (nSlotType)
	{
	case MMIST_MELEE:		return MMCIP_MELEE;
	case MMIST_RANGE:		return MMCIP_PRIMARY;		
	case MMIST_CUSTOM:		return MMCIP_CUSTOM1;
	case MMIST_HEAD:		return MMCIP_HEAD;
	case MMIST_CHEST:		return MMCIP_CHEST;
	case MMIST_HANDS:		return MMCIP_HANDS;
	case MMIST_LEGS:		return MMCIP_LEGS;
	case MMIST_FEET:		return MMCIP_FEET;
	case MMIST_FINGER:		return MMCIP_FINGERL;
	case MMIST_AVATAR:		return MMCIP_AVATAR;
	case MMIST_COMMUNITY:	return MMCIP_COMMUNITY1;
	case MMIST_LONGBUFF:	return MMCIP_LONGBUFF1;
	case MMIST_DASH:		return MMCIP_DASH;
	case MMIST_NONE :		break;
	default:
		_ASSERT(0);
		break;
	}

	return MMCIP_END;
}

MMatchItemSlotType	GetSuitableItemSlot(MMatchCharItemParts nParts)
{
	switch (nParts)
	{
	case MMCIP_HEAD:		
		return MMIST_HEAD;

	case MMCIP_CHEST:		
		return MMIST_CHEST;

	case MMCIP_HANDS:		
		return MMIST_HANDS;

	case MMCIP_LEGS:		
		return MMIST_LEGS;

	case MMCIP_FEET:		
		return MMIST_FEET;

	case MMCIP_FINGERL:
	case MMCIP_FINGERR:		
		return MMIST_FINGER;

	case MMCIP_AVATAR:
		return MMIST_AVATAR;

	case MMCIP_MELEE:		
		return MMIST_MELEE;

	case MMCIP_PRIMARY:
	case MMCIP_SECONDARY:	
		return MMIST_RANGE;

	case MMCIP_CUSTOM1:
	case MMCIP_CUSTOM2:
	case MMCIP_CUSTOM3:
		return MMIST_CUSTOM;

	case MMCIP_COMMUNITY1:
	case MMCIP_COMMUNITY2:
		return MMIST_COMMUNITY;

	case MMCIP_LONGBUFF1:
	case MMCIP_LONGBUFF2:
		return MMIST_LONGBUFF;
	case MMCIP_DASH:
		return MMIST_DASH;

	default:
		_ASSERT(0);
	}

	return MMIST_END;
}


bool IsWeaponItemSlotType(MMatchItemSlotType nSlotType)
{
	if ((nSlotType == MMIST_MELEE) || (nSlotType == MMIST_RANGE) ||	(nSlotType == MMIST_CUSTOM)) 
		return true;

	return false;
}

bool IsWeaponCharItemParts(MMatchCharItemParts nParts)
{
	if ((nParts == MMCIP_MELEE) || (nParts == MMCIP_PRIMARY) ||	(nParts == MMCIP_SECONDARY) 
		|| (nParts == MMCIP_CUSTOM1) ||	(nParts == MMCIP_CUSTOM2)
		|| (nParts == MMCIP_COMMUNITY1) || (nParts == MMCIP_COMMUNITY2)
		|| (nParts == MMCIP_LONGBUFF1) || (nParts == MMCIP_LONGBUFF2) ||
		(nParts == MMCIP_CUSTOM3))
		return true;

	return false;
}


char* GetItemSlotTypeStr(MMatchItemSlotType nSlotType)			///< 사용되지 않는다?
{
	static char st_SlotTypeStr[MMIST_END][32] = { "없음",		// MMIST_NONE
												"근접무기",		// MMIST_MELEE
												"원거리무기",	// MMIST_RANGE
												"아이템",		// MMIST_CUSTOM
												"머리",			// MMIST_HEAD
												"가슴",			// MMIST_CHEST
												"손",			// MMIST_HANDS
												"다리",			// MMIST_LEGS
												"발",			// MMIST_FEET
												"손가락",		// MMIST_FINGER
												"특별",			// MMIST_EXTRA
												};

	return st_SlotTypeStr[nSlotType];
}

char* GetCharItemPartsStr(MMatchCharItemParts nParts)			///< 사용되지 않는다?
{
	static char st_CharItemPartsStr[MMCIP_END][32] = {  "머리",					// MMCIP_HEAD
														"가슴",					// MMCIP_CHEST
														"손",					// MMCIP_HANDS
														"다리",					// MMCIP_LEGS
														"발",					// MMCIP_FEET
														"왼쪽손가락",			// MMCIP_FINGERL
														"오른쪽손가락",			// MMCIP_FINGERR
														"근접무기",				// MMCIP_MELEE
														"주무기",				// MMCIP_PRIMARY
														"보조무기",				// MMCIP_SECONDARY
														"아이템1",				// MMCIP_CUSTOM1
														"아이템2",				// MMCIP_CUSTOM2
														};

	return st_CharItemPartsStr[nParts];
}


MMatchWeaponType GetWeaponType(MMatchMeleeItemType nMeleeItemType)
{
	switch (nMeleeItemType)
	{
	case MIT_DAGGER:		return MWT_DAGGER; 
	case MIT_DUAL_DAGGER:	return MWT_DUAL_DAGGER;
	case MIT_KATANA:		return MWT_KATANA;
	case MIT_GREAT_SWORD:	return MWT_GREAT_SWORD;
	case MIT_DOUBLE_KATANA:	return MWT_DOUBLE_KATANA;
	default:
			// 없는 타입
			_ASSERT(0);
	}

	return MWT_NONE;
}

MMatchWeaponType GetWeaponType(MMatchRangeItemType nRangeItemType)
{
	switch (nRangeItemType)
	{

	case RIT_PISTOL:		return MWT_PISTOL;
	case RIT_PISTOLx2:		return MWT_PISTOLx2;

	case RIT_REVOLVER:		return MWT_REVOLVER;
	case RIT_REVOLVERx2:	return MWT_REVOLVERx2;

	case RIT_SMG:			return MWT_SMG;
	case RIT_SMGx2:			return MWT_SMGx2;

	case RIT_SHOTGUN:		return MWT_SHOTGUN;
	case RIT_SAWED_SHOTGUN:	return MWT_SAWED_SHOTGUN;

	case RIT_RIFLE:			return MWT_RIFLE;
	case RIT_MACHINEGUN:	return MWT_MACHINEGUN;
	case RIT_ROCKET:		return MWT_ROCKET;
	case RIT_SNIFER:		return MWT_SNIFER;
	default:
			// 없는 타입
			_ASSERT(0);
	}

	return MWT_NONE;
}

MMatchWeaponType GetWeaponType(MMatchCustomItemType nCustomItemType)
{
	switch (nCustomItemType)
	{
	case MMCIT_MED_KIT:				return MWT_MED_KIT;
	case MMCIT_REPAIR_KIT:			return MWT_REPAIR_KIT;
	case MMCIT_BULLET_KIT:			return MWT_BULLET_KIT;
	case MMCIT_FLASH_BANG:			return MWT_FLASH_BANG;
	case MMCIT_FRAGMENTATION:		return MWT_FRAGMENTATION;
	case MMCIT_SMOKE_GRENADE:		return MWT_SMOKE_GRENADE;
	case MMCIT_ENCHANT_FIRE:		return MWT_ENCHANT_FIRE;
	case MMCIT_ENCHANT_COLD:		return MWT_ENCHANT_COLD;
	case MMCIT_ENCHANT_LIGHTNING:	return MWT_ENCHANT_LIGHTNING;
	case MMCIT_ENCHANT_POISON:		return MWT_ENCHANT_POISON;
	case MMCIT_FOOD:				return MWT_FOOD;
	case MMCIT_POTION:				return MWT_POTION;

	default:
		{
			// 없는 타입
			_ASSERT(0);
		}
	}
	return MWT_NONE;
}

bool IsEnchantItem(MMatchItemDesc* pItemDesc)
{
	if (pItemDesc->m_nType.Ref() == MMIT_CUSTOM)
	{
		MMatchWeaponType t = pItemDesc->m_nWeaponType.Ref();
		if ((t == MWT_ENCHANT_FIRE) || 
			(t == MWT_ENCHANT_COLD) || 
			(t == MWT_ENCHANT_LIGHTNING) || 
			(t == MWT_ENCHANT_POISON) )
			return true;
	}

	return false;
}

bool IsExpiredRentItem( const MMatchItem* pMItem, const DWORD dwTick )
{
	if( 0 == pMItem )			return false;
	if( !pMItem->IsRentItem() ) return false;
	
	const DWORD dwPassTime			= MGetTimeDistance( pMItem->GetRentItemRegTime(), dwTick );
	const DWORD dwPassMinuteTime	= dwPassTime / 60000; // (1000 * 60);

	if (static_cast<int>((pMItem->GetRentMinutePeriodRemainder() - dwPassMinuteTime)) > 0)
		return false;

	return true;
}