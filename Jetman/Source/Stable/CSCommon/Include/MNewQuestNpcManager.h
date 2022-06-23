#pragma once

class MNewQuestNpcObject
{
private:
	MUID m_uid;
	//MQUEST_NPC m_nType;
	MUID m_uidController;
	int m_nDropItemId;
	string m_actorDef;

public:
	MNewQuestNpcObject(MUID& uid) : m_uid(uid), m_uidController(0,0), m_nDropItemId(0),m_actorDef("") {}
	~MNewQuestNpcObject() {}

	MUID GetUID() { return m_uid; }

	// 이 npc를 조종할 플레이어
	void AssignControl(MUID& uidPlayer) { m_uidController=uidPlayer; }
	void ReleaseControl() { m_uidController.SetInvalid(); }
	MUID& GetController() { return m_uidController; }

	int GetDropItemId()			{ return m_nDropItemId; }
	void SetDropItemId(int id)	{ m_nDropItemId = id; }
	bool HasDropItem()			{ return m_nDropItemId != 0; }
	void SetActorDef(const char* actorDef) { m_actorDef = actorDef; }
	const char* GetActorDef() { return m_actorDef.c_str(); }
};

class MNewQuestNpcManager
{
	typedef map<MUID, MNewQuestNpcObject*>	MapNpc;
	typedef MapNpc::iterator				ItorNpc;

	MapNpc m_mapNpc;

public:
	void AddNpcObject(MUID uidNpc, MUID uidController, int nDropItemId, const char* szActorDef);
	void DeleteNpcObject(MUID uidNpc);

	int GetNumNpc() { return (int)m_mapNpc.size(); }
	MNewQuestNpcObject* GetNpc(MUID uid);
	bool GetNpcByName(const char* npcName);
	//Custom: latejoin related shit
	MapNpc* GetNpcs() { return &m_mapNpc; }
};