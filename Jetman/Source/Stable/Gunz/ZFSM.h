#pragma once

class ZFSMState;

// 유한상태기계
class ZFSM
{
	typedef map<string, ZFSMState*>		MapState;
	typedef MapState::iterator			ItorState;

	MapState m_mapState;

	string m_strName;
	string m_strEntryStateName;

public:
	~ZFSM();

	void AddState(ZFSMState* pState);
	ZFSMState* GetState(const char* szName);

	void SetName(const char* sz) { m_strName = sz; }
	const char* GetName() { return m_strName.c_str(); }

	void SetEntryStateName(const char* sz) { m_strEntryStateName = sz; }
	const char* GetEntryStateName() { return m_strEntryStateName.c_str(); }

	void PrepareTransNextState();
};