#pragma once

///////////////////////////////////////////////////////////////////////////
// DROPGUNGAME XML READ
class MDropGunGameItemSet
{
private:
	int m_nID;
	int m_nItemID1;
	int m_nItemID2;
	int m_nItemID3;
	int m_nItemID4;
	int m_nItemID5;
public:
	void SetID(int i) { m_nID = i; }
	void SetItemID1(int i) { m_nItemID1 = i; }
	void SetItemID2(int i) { m_nItemID2 = i; }
	void SetItemID3(int i) { m_nItemID3 = i; }
	void SetItemID4(int i) { m_nItemID4 = i; }
	void SetItemID5(int i) { m_nItemID5 = i; }

	int GetID() { return m_nID; }
	int GetItemID1() { return m_nItemID1; }
	int GetItemID2() { return m_nItemID2; }
	int GetItemID3() { return m_nItemID3; }
	int GetItemID4() { return m_nItemID4; }
	int GetItemID5() { return m_nItemID5; }
};

//////////////////

class MDropGunGameWorldItem
{
private:
	int m_nID;
	int m_nItemID;
	float m_fassx;
	float m_fassy;
	float m_fassz;
public:
	void SetID(int i) { m_nID = i; }
	void SetItemID(int i) { m_nItemID = i; }
	void SetAssX(float f) { m_fassx = f; }
	void SetAssY(float f) { m_fassy = f; }
	void SetAssZ(float f) { m_fassz = f; }

	int GetID() { return m_nID; }
	int GetItemID() { return m_nItemID; }
	float GetAssX() { return m_fassx; }
	float GetAssY() { return m_fassy; }
	float GetAssZ() { return m_fassz; }
};

//////////////////

class MDropGunGameItemSetMgr : public map<int, MDropGunGameItemSet*>
{
public:
	MDropGunGameItemSetMgr();
	~MDropGunGameItemSetMgr();
	void AddItemSet(int nID, int nItemID1, int nItemID2, int nItemID3, int nItemID4, int nItemID5);

	bool AddDropGunGameItemSet(MDropGunGameItemSet* pGunGameItemSet);
	MDropGunGameItemSet* GetDropGunGameItemSetByListID(int nID);
	void Clear();
};

//////////////////

class MDropGunGameWorldItemMgr : public map<int, MDropGunGameWorldItem*>
{

public:
	MDropGunGameWorldItemMgr();
	~MDropGunGameWorldItemMgr();
	void AddWorldItem(int nID, int nItemID, float fassx, float fassy, float fassz);

	bool AddDropGunGameWorldItem(MDropGunGameWorldItem* pGunGameWorldItem);
	MDropGunGameWorldItem* GetDropGunGameWorldItemByListID(int nID);
	void Clear();
};

//////////////////

class MDropGunGameXMLManager
{
private:
	map<int, MDropGunGameItemSetMgr*>	m_pItemSetMgr;
	map<int, MDropGunGameWorldItemMgr*>	m_pWorldItemMgr;
public:
	int m_nId;
public:
	MDropGunGameXMLManager();
	~MDropGunGameXMLManager();
	void SetID(int i) { m_nId = i; }
	int GetID() { return m_nId; }

	bool ReadXml(MZFileSystem* pFileSystem, const char* szFileName);
	void ParseDropGunGame(MXmlElement& element);
	bool AddDropGunGameMap(MDropGunGameItemSetMgr* pGunGameItemSetMgr, MDropGunGameWorldItemMgr* pDropGunGameWorldItemMgr);


	MDropGunGameItemSetMgr* GetDropGunGameItemSetByID(int nID);
	MDropGunGameWorldItemMgr* GetDropGunGameWorldItemByID(int nId);
};


///////////////////////////////////////////////////////////////////////////
// DROPGUNGAME STAGE BRAIN

class MDropGunGameBattleStageStorage
{
protected:
	int m_mCID;
	MMatchObject* m_pObj;
	int m_nItemID1;
	int m_nItemID2;
	int m_nItemID3;
	int m_nItemID4;
	int m_nItemID5;
public:
	void SetCID(int i) { m_mCID = i; };
	void SetObj(MMatchObject* o) { m_pObj = o; };
	void SetItemID1(int i) { m_nItemID1 = i; };
	void SetItemID2(int i) { m_nItemID2 = i; };
	void SetItemID3(int i) { m_nItemID3 = i; };
	void SetItemID4(int i) { m_nItemID4 = i; };
	void SetItemID5(int i) { m_nItemID5 = i; };

	int GetCID() { return m_mCID; };
	MMatchObject* GetObj() { return m_pObj; };
	int GetItemID1() { return m_nItemID1; };
	int GetItemID2() { return m_nItemID2; };
	int GetItemID3() { return m_nItemID3; };
	int GetItemID4() { return m_nItemID4; };
	int GetItemID5() { return m_nItemID5; };
};

//////////////////

class MDropGunGameBattleStage : public map<int, MDropGunGameBattleStageStorage*>
{
private:
	int m_nID;
public:
	void SetID(int i) { m_nID = i; };
	int GetID() { return m_nID; };
protected:
	bool AddPlayerInStorage(MDropGunGameBattleStageStorage* pGetPlayerByUid);
	int GetCount();
	void Clear();
public:
	MDropGunGameBattleStage();
	~MDropGunGameBattleStage();
	MDropGunGameBattleStageStorage* GetPlayerByCID(int nCID);
	void AddPlayer(MMatchObject* pObj, int CID, int itemid1, int itemid2, int itemid3, int itemid4, int itemid5);
	void RemovePlayer(MMatchObject* pObj, int CID);

};

//////////////////

class MDropGunGameStageManager : public map<int, MDropGunGameBattleStage*>
{
protected:
	bool AddStageInStorage(MDropGunGameBattleStage* pGetStorageByStageID);
	int GetCount();
	void Clear();
public:
	MDropGunGameStageManager();
	~MDropGunGameStageManager();

	MDropGunGameBattleStage* GetStorageByStageID(int nID);
	void AddStage(int nID);
	void RemoveStage(int nID);
};














