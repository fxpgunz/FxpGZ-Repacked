//////////////////////////////////////////////////
//// DROP GUNGAME
//// ADDED: 22/03/2014
//// AUTOR: NICK VLUG (HEROBANANA)
//
#include "stdafx.h"
#include "MDropGunGameManager.h"

///////////////////////////////////////////////////////////////////////////////////
//
//		Drop GunGame Item Set Mgr
//
///////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////
MDropGunGameItemSetMgr::MDropGunGameItemSetMgr()
{

}

////////////////////////////////////////////////
MDropGunGameItemSetMgr::~MDropGunGameItemSetMgr()
{
	Clear();
}

////////////////////////////////////////////////
MDropGunGameItemSet* MDropGunGameItemSetMgr::GetDropGunGameItemSetByListID(int nID)
{
	iterator itor = find(nID);
	if (itor != end())
	{
		return (*itor).second;
	}
	return NULL;
}

////////////////////////////////////////////////
bool MDropGunGameItemSetMgr::AddDropGunGameItemSet(MDropGunGameItemSet* pDropGunGameItem)
{
	if (GetDropGunGameItemSetByListID(pDropGunGameItem->GetID()))
		return false;

	insert(value_type(pDropGunGameItem->GetID(), pDropGunGameItem));
	return true;
}

////////////////////////////////////////////////
void MDropGunGameItemSetMgr::AddItemSet(int nID, int nItemID1, int nItemID2, int nItemID3, int nItemID4, int nItemID5)
{
	try
	{
		MDropGunGameItemSet* pDropGunGameItemSet = new MDropGunGameItemSet;

		pDropGunGameItemSet->SetID(nID);
		pDropGunGameItemSet->SetItemID1(nItemID1);
		pDropGunGameItemSet->SetItemID2(nItemID2);
		pDropGunGameItemSet->SetItemID3(nItemID3);
		pDropGunGameItemSet->SetItemID4(nItemID4);
		pDropGunGameItemSet->SetItemID5(nItemID5);

		if (!AddDropGunGameItemSet(pDropGunGameItemSet))
		{
			mlog("ERROR: Drop GunGame Item Duplication.");
			delete pDropGunGameItemSet;
		}
	}
	catch (...)
	{
		mlog("ERROR: Drop GunGame Add Item.");
	}
}

////////////////////////////////////////////////
void MDropGunGameItemSetMgr::Clear()
{
	for (iterator itor = begin(); itor != end(); ++itor)
	{
		delete (*itor).second;
	}

	clear();
}

///////////////////////////////////////////////////////////////////////////////////
//
//		Drop GunGame WorldItem Mgr
//
///////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////
MDropGunGameWorldItemMgr::MDropGunGameWorldItemMgr()
{

}

////////////////////////////////////////////////
MDropGunGameWorldItemMgr::~MDropGunGameWorldItemMgr()
{
	Clear();
}

////////////////////////////////////////////////
MDropGunGameWorldItem* MDropGunGameWorldItemMgr::GetDropGunGameWorldItemByListID(int nID)
{
	iterator itor = find(nID);
	if (itor != end())
	{
		return (*itor).second;
	}
	return NULL;
}

////////////////////////////////////////////////
bool MDropGunGameWorldItemMgr::AddDropGunGameWorldItem(MDropGunGameWorldItem* pDropGunGameWorldItem)
{
	if (GetDropGunGameWorldItemByListID(pDropGunGameWorldItem->GetID()))
		return false;

	insert(value_type(pDropGunGameWorldItem->GetID(), pDropGunGameWorldItem));
	return true;
}

////////////////////////////////////////////////
void MDropGunGameWorldItemMgr::AddWorldItem(int nID, int nItemID, float fassx, float fassy, float fassz)
{
	try
	{
		MDropGunGameWorldItem* pDropGunGameWorldItem = new MDropGunGameWorldItem;

		pDropGunGameWorldItem->SetID(nID);
		pDropGunGameWorldItem->SetItemID(nItemID);
		pDropGunGameWorldItem->SetAssX(fassx);
		pDropGunGameWorldItem->SetAssY(fassy);
		pDropGunGameWorldItem->SetAssZ(fassz);

		if (!AddDropGunGameWorldItem(pDropGunGameWorldItem))
		{
			mlog("ERROR: Drop GunGame WorldItem Duplication.");
			delete pDropGunGameWorldItem;
		}
	}
	catch (...)
	{
		mlog("ERROR: Drop GunGame Add Item.");
	}
}

////////////////////////////////////////////////
void MDropGunGameWorldItemMgr::Clear()
{
	for (iterator itor = begin(); itor != end(); ++itor)
	{
		delete (*itor).second;
	}

	clear();
}

///////////////////////////////////////////////////////////////////////////////////
//
//		Drop GunGame XML Manager
//
///////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////
MDropGunGameXMLManager::MDropGunGameXMLManager()
{

}

////////////////////////////////////////////////
MDropGunGameXMLManager::~MDropGunGameXMLManager()
{
	m_pItemSetMgr.clear();
	m_pWorldItemMgr.clear();
}

////////////////////////////////////////////////
#define MTOK_DROPGUNGAME					"DROPGUNGAME"
#define MTOK_DROPGUNGAME_WORLDITEM			"WORLDITEM"
#define MTOK_DROPGUNGAME_ITEMSET			"ITEMSET"

#define MTOK_DROPGUNGAME_ATTR_ID			"id"
#define MTOK_DROPGUNGAME_ATTR_ITEMID		"itemid"
#define MTOK_DROPGUNGAME_ATTR_MELEE			"melee"
#define MTOK_DROPGUNGAME_ATTR_PRIMARY		"primary"
#define MTOK_DROPGUNGAME_ATTR_SECONDARY		"secondary"
#define MTOK_DROPGUNGAME_ATTR_CUSTOM1		"custom1"
#define MTOK_DROPGUNGAME_ATTR_CUSTOM2		"custom2"

#define MTOK_DROPGUNGAME_ATTR_XASS			"x"
#define MTOK_DROPGUNGAME_ATTR_YASS			"y"
#define MTOK_DROPGUNGAME_ATTR_ZASS			"z"
////////////////////////////////////////////////
bool MDropGunGameXMLManager::ReadXml(MZFileSystem* pFileSystem, const char* szFileName)
{
	MXmlDocument xmlIniData;
	xmlIniData.Create();

	char *buffer;
	MZFile mzf;

	if (pFileSystem)
	{
		if (!mzf.Open(szFileName, pFileSystem))
		{
			if (!mzf.Open(szFileName))
			{
				mlog("DropGunGame Can't Open File in MZFileSystem (xml/DropGunGameList.xml)\n");
				xmlIniData.Destroy();
				return false;
			}
		}
	}
	else
	{
		if (!mzf.Open(szFileName))
		{
			mlog("DropGunGame Can't Open File (xml/DropGunGameList.xml)\n");
			xmlIniData.Destroy();
			return false;
		}
	}

	buffer = new char[mzf.GetLength() + 1];
	buffer[mzf.GetLength()] = 0;

	mzf.Read(buffer, mzf.GetLength());

	if (!xmlIniData.LoadFromMemory(buffer))
	{
		mlog("DropGunGame Can't Load (xml/DropGunGameList.xml) From Memory\n");
		xmlIniData.Destroy();
		return false;
	}

	delete[] buffer;
	mzf.Close();


	MXmlElement rootElement, chrElement, attrElement;
	char szTagName[256];

	rootElement = xmlIniData.GetDocumentElement();
	int iCount = rootElement.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{

		chrElement = rootElement.GetChildNode(i);
		chrElement.GetTagName(szTagName);

		if (szTagName[0] == '#') continue;

		if (!stricmp(szTagName, MTOK_DROPGUNGAME))
		{
			ParseDropGunGame(chrElement);
		}
		else
		{
			return false;
		}
	}

	xmlIniData.Destroy();

	return true;
}

////////////////////////////////////////////////
void MDropGunGameXMLManager::ParseDropGunGame(MXmlElement& element)
{
	char szAttrValue[256];
	char szAttrName[64];
	char szTagName[128];

	MDropGunGameItemSetMgr* pDropGunGameItemSetMgr = new MDropGunGameItemSetMgr;
	MDropGunGameWorldItemMgr* pDropGunGameWorldItemMgr = new MDropGunGameWorldItemMgr;

	// DropGunGame
	int nAttrCount = element.GetAttributeCount();
	for (int i = 0; i < nAttrCount; i++)
	{
		element.GetAttribute(i, szAttrName, szAttrValue);
		if (!stricmp(szAttrName, MTOK_DROPGUNGAME_ATTR_ID))
		{
			//mlog("DropGunGame Map ID (%i)\n", atoi(szAttrValue)); 
			SetID(atoi(szAttrValue));
		}
	}

	//mlog("DropGunGame Start New Traject\n");
	int nID1 = 0;
	int nID2 = 0;

	int iChildCount = element.GetChildNodeCount();
	MXmlElement chrElement;
	for (int i = 0; i < iChildCount; i++)
	{
		chrElement = element.GetChildNode(i);
		chrElement.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;

		// ITEM
		if (!stricmp(szTagName, MTOK_DROPGUNGAME_WORLDITEM))
		{
			//mlog("Read DropGunGame WorldItem\n");
			nID1 = ++nID1;
			//mlog("DropGunGame Insert ID (%i)\n", nID1);
			int nItemID;
			float fAssx;
			float fAssy;
			float fAssz;

			int nAttrCount = chrElement.GetAttributeCount();
			for (int i = 0; i < nAttrCount; i++)
			{
				chrElement.GetAttribute(i, szAttrName, szAttrValue);

				if (!stricmp(szAttrName, MTOK_DROPGUNGAME_ATTR_ITEMID))
				{
					nItemID = atoi(szAttrValue);
					//mlog("DropGunGame Insert ItemID1 (%i)\n", atoi(szAttrValue));
				}
				else if (!stricmp(szAttrName, MTOK_DROPGUNGAME_ATTR_XASS))
				{
					fAssx = atof(szAttrValue);
					//mlog("DropGunGame Insert x ass (%f)\n", atof(szAttrValue));
				}
				else if (!stricmp(szAttrName, MTOK_DROPGUNGAME_ATTR_YASS))
				{
					fAssy = atof(szAttrValue);
					//mlog("DropGunGame Insert y ass (%f)\n", atof(szAttrValue));
				}
				else if (!stricmp(szAttrName, MTOK_DROPGUNGAME_ATTR_ZASS))
				{
					fAssz = atof(szAttrValue);
					//mlog("DropGunGame Insert z ass (%f)\n", atof(szAttrValue));
				}
			}
			if (nID1 >= 0)
			{
				pDropGunGameWorldItemMgr->AddWorldItem(nID1, nItemID, fAssx, fAssy, fAssz);
			}
		}

		if (!stricmp(szTagName, MTOK_DROPGUNGAME_ITEMSET))
		{
			//mlog("Read DropGunGame ItemSet\n");
			nID2 = ++nID2;
			//mlog("DropGunGame Insert ID (%i)\n", nID2);
			int nMelee;
			int nPrimary;
			int nSecondary;
			int nCustom1;
			int nCustom2;

			int nAttrCount = chrElement.GetAttributeCount();
			for (int i = 0; i < nAttrCount; i++)
			{
				chrElement.GetAttribute(i, szAttrName, szAttrValue);
				if (!stricmp(szAttrName, MTOK_DROPGUNGAME_ATTR_MELEE))
				{
					nMelee = atoi(szAttrValue);
					//mlog("DropGunGame Insert Melee (%i)\n", atoi(szAttrValue));
				}
				else if (!stricmp(szAttrName, MTOK_DROPGUNGAME_ATTR_PRIMARY))
				{
					nPrimary = atoi(szAttrValue);
					//mlog("DropGunGame Insert Primary (%i)\n", atoi(szAttrValue));
				}
				else if (!stricmp(szAttrName, MTOK_DROPGUNGAME_ATTR_SECONDARY))
				{
					nSecondary = atoi(szAttrValue);
					//mlog("DropGunGame Insert Secondary (%i)\n", atoi(szAttrValue));
				}
				else if (!stricmp(szAttrName, MTOK_DROPGUNGAME_ATTR_CUSTOM1))
				{
					nCustom1 = atoi(szAttrValue);
					//mlog("DropGunGame Insert Custom1 (%i)\n", atoi(szAttrValue));
				}
				else if (!stricmp(szAttrName, MTOK_DROPGUNGAME_ATTR_CUSTOM2))
				{
					nCustom2 = atoi(szAttrValue);
					//mlog("DropGunGame Insert Custom2 (%i)\n", atoi(szAttrValue));
				}
			}
			if (nID2 >= 0)
			{
				pDropGunGameItemSetMgr->AddItemSet(nID2, nMelee, nPrimary, nSecondary, nCustom1, nCustom2);
			}
		}
	}

	m_pItemSetMgr.insert(map<int, MDropGunGameItemSetMgr*>::value_type(GetID(), pDropGunGameItemSetMgr));
	m_pWorldItemMgr.insert(map<int, MDropGunGameWorldItemMgr*>::value_type(GetID(), pDropGunGameWorldItemMgr));
}

////////////////////////////////////////////////
MDropGunGameItemSetMgr* MDropGunGameXMLManager::GetDropGunGameItemSetByID(int nId)
{
	map<int, MDropGunGameItemSetMgr*>::iterator itor = m_pItemSetMgr.find(nId);
	if (itor != m_pItemSetMgr.end())
	{
		return (*itor).second;
	}

	return NULL;
}
////////////////////////////////////////////////
MDropGunGameWorldItemMgr* MDropGunGameXMLManager::GetDropGunGameWorldItemByID(int nId)
{
	map<int, MDropGunGameWorldItemMgr*>::iterator itor = m_pWorldItemMgr.find(nId);
	if (itor != m_pWorldItemMgr.end())
	{
		return (*itor).second;
	}

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////////
//
//		Drop GunGame Battle Stage
//
///////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////
MDropGunGameBattleStage::MDropGunGameBattleStage()
{

};

////////////////////////////////////////////////
MDropGunGameBattleStage::~MDropGunGameBattleStage()
{
	Clear();
};

////////////////////////////////////////////////
bool MDropGunGameBattleStage::AddPlayerInStorage(MDropGunGameBattleStageStorage* pGetPlayerByUid)
{
	if (GetPlayerByCID(pGetPlayerByUid->GetCID()))
		return false;

	insert(value_type(pGetPlayerByUid->GetCID(), pGetPlayerByUid));
	return true;
}

////////////////////////////////////////////////
int MDropGunGameBattleStage::GetCount()
{
	return (int) size();
}

////////////////////////////////////////////////
void MDropGunGameBattleStage::Clear()
{
	for (iterator itor = begin(); itor != end(); ++itor)
	{
		delete (*itor).second;
	}
	clear();
};

////////////////////////////////////////////////
MDropGunGameBattleStageStorage* MDropGunGameBattleStage::GetPlayerByCID(int nCID)
{
	iterator itor = find(nCID);
	if (itor != end())
	{
		return (*itor).second;
	}
	return NULL;
}

////////////////////////////////////////////////
void MDropGunGameBattleStage::AddPlayer(MMatchObject* pObj, int CID, int itemid1, int itemid2, int itemid3, int itemid4, int itemid5)
{
	MDropGunGameBattleStageStorage* pBattleStageStorage = new MDropGunGameBattleStageStorage;

	////////////////////////////////////////////
	// check if object is alive
	if (pObj == NULL)
	{
		mlog("MDropGunGameBattleStage: Something went wrong by OBJECT checking, for sure we put the cid (%i).", CID);
		return;
	}
	pBattleStageStorage->SetObj(pObj);

	////////////////////////////////////////////
	// Check if cid is same as object cid
	if (pObj->GetCharInfo()->m_nCID != CID)
	{
		mlog("MDropGunGameBattleStage: Something went wrong, for sure we kicked (%s).", pObj->GetCharInfo()->m_szName);
		MMatchServer::GetInstance()->OnDisconnect(pObj->GetUID());
		return;
	}
	pBattleStageStorage->SetCID(CID);

	// Insert Item IDs
	pBattleStageStorage->SetItemID1(itemid1);
	pBattleStageStorage->SetItemID2(itemid2);
	pBattleStageStorage->SetItemID3(itemid3);
	pBattleStageStorage->SetItemID4(itemid4);
	pBattleStageStorage->SetItemID5(itemid5);

	////////////////////////////////////////////
	// Check if there is a dublication problem
	if (!AddPlayerInStorage(pBattleStageStorage))
	{
		mlog("MDropGunGameBattleStage: Player Dublication found. The Dublicated User was (%s).", pObj->GetCharInfo()->m_szName);
		MMatchServer::GetInstance()->OnDisconnect(pObj->GetUID());
		return;
	}

}

////////////////////////////////////////////////
void MDropGunGameBattleStage::RemovePlayer(MMatchObject* pObj, int CID)
{
	if (pObj == NULL)
		return;

	if (pObj->GetCharInfo()->m_nCID != CID)
		return;

	iterator itor = find(CID);
	if (end() == itor)
		return;

	erase(itor);
};

///////////////////////////////////////////////////////////////////////////////////
//
//		Drop GunGame Battle Stage Manager
//
///////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////
MDropGunGameStageManager::MDropGunGameStageManager()
{

};

////////////////////////////////////////////////
MDropGunGameStageManager::~MDropGunGameStageManager()
{
	Clear();
};

////////////////////////////////////////////////
bool MDropGunGameStageManager::AddStageInStorage(MDropGunGameBattleStage* pGetStorageByStageID)
{
	if (GetStorageByStageID(pGetStorageByStageID->GetID()))
		return false;

	insert(value_type(pGetStorageByStageID->GetID(), pGetStorageByStageID));
	return true;
}

////////////////////////////////////////////////
int MDropGunGameStageManager::GetCount()
{
	return (int) size();
}

////////////////////////////////////////////////
void MDropGunGameStageManager::Clear()
{
	for (iterator itor = begin(); itor != end(); ++itor)
	{
		delete (*itor).second;
	}
	clear();
}

////////////////////////////////////////////////
MDropGunGameBattleStage* MDropGunGameStageManager::GetStorageByStageID(int nID)
{
	iterator itor = find(nID);
	if (itor != end())
	{
		return (*itor).second;
	}
	return NULL;
}

////////////////////////////////////////////////
void MDropGunGameStageManager::AddStage(int nID)
{
	MDropGunGameBattleStage* pBattleStage = new MDropGunGameBattleStage;
	pBattleStage->SetID(nID);

	if (!AddStageInStorage(pBattleStage))
	{
		mlog("BattleStage: Stage Duplication Found!");
		return;
	}
}

////////////////////////////////////////////////
void MDropGunGameStageManager::RemoveStage(int nID)
{
	iterator itor = find(nID);
	if (end() == itor)
		return;

	erase(itor);
}