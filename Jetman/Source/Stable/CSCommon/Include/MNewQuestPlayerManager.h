#pragma once

class MNewQuestPlayer
{
	MMatchObject* m_pObject;

	int m_numNpcControl;		// 현재 내가 조종 중인 npc 수
	bool m_bEnableNpcControl;	// NPC Control이 가능한지 여부

public:
	MNewQuestPlayer(MMatchObject* pObject)
		: m_pObject(pObject)
		, m_numNpcControl(0)
		, m_bEnableNpcControl(true)
	{
		_ASSERT(m_pObject);
		_ASSERT(m_pObject->GetUID().IsValid());
	}

	MUID GetUid() { return m_pObject->GetUID();}

	int GetNumNpcControl() { return m_numNpcControl; }
	void IncreaseNpcControl() { ++m_numNpcControl; _ASSERT(m_numNpcControl>=0); }
	void DecreaseNpcControl() { --m_numNpcControl; _ASSERT(m_numNpcControl>=0); }
};

class MNewQuestPlayerManager
{
	typedef map<MUID, MNewQuestPlayer*>		MapPlayer;
	typedef MapPlayer::iterator				ItorPlayer;

	MapPlayer m_mapPlayer;

public:

	void AddPlayer(MMatchObject* pObject);
	void RemovePlayer(MUID uid);

	void IncreaseNpcControl(MUID uidController, MUID uidNpc);
	void DecreaseNpcControl(MUID uidController);
	bool FindPlayer(MUID uid);

	MUID FindSuitableController();
};