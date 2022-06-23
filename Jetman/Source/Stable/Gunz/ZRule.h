#ifndef _ZRULE_H
#define _ZRULE_H


class ZMatch;

class ZRule
{
private:
	ZRule()			{ _ASSERT(0); } // �̰� ����ϸ� �ȵ� 

protected:
	ZMatch*		m_pMatch;
	virtual void OnUpdate(float fDelta) {}
public:
	ZRule(ZMatch* pMatch);
	virtual ~ZRule();
	virtual bool Init() { return true; }
	void Update(float fDelta);
	virtual bool OnCommand(MCommand* pCommand);
	virtual void AfterCommandProcessed(MCommand* pCommand);
	virtual void OnResponseRuleInfo(MTD_RuleInfo* pInfo);
	virtual void OnSetRoundState(MMATCH_ROUNDSTATE roundState) {}
	virtual int GetRoundMax() { return 0; }						// �꿡�� �Ѷ��� ���� �ڵ� �����ϰ� ���� ��� �ܿ��� 0�� ��ȯ
	static ZRule* CreateRule(ZMatch* pMatch, MMATCH_GAMETYPE nGameType);


	// �ش� Rule���� ��� �޾Ƽ� ����ؾ� �� ��

	
};




#endif