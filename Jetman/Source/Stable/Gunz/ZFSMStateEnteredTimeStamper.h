#pragma once

// FSM�� ����ϴ� ��ü�� �� state�� ���԰��� ��Ÿ���� Ȯ���ϱ� ���ؼ� �� state�� ������ ���Խð���
// ����ص� �ʿ䰡 �ִ�. �� Ŭ������ �װ��� �����ش�.

class ZFSMState;

class ZFSMStateEnteredTimeStamper
{
	typedef map<ZFSMState*, DWORD>				MapStateLastEnteredTime;
	typedef MapStateLastEnteredTime::iterator	ItorStateLastEnteredTime;

	MapStateLastEnteredTime m_mapStateLastEnteredTime;

public:
	bool CheckCooltime(ZFSMState* pState, DWORD dwCurrTime);
	void SetEnteredTime(ZFSMState* pState, DWORD dwCurrTime);
	DWORD GetEnteredTime(ZFSMState* pState);
};