#pragma once

// FSM을 사용하는 객체는 각 state의 진입가능 쿨타임을 확인하기 위해서 각 state의 마지막 진입시각을
// 기억해둘 필요가 있다. 이 클래스가 그것을 도와준다.

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