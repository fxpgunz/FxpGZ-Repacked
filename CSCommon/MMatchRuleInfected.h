class MMatchRuleInfection : public MMatchRule {
	enum InfectionState
	{
		Playing,
		SelectInfected,
		End
	};

	struct InfectS
	{
		bool Zombie, Alive;
		InfectS()
		{
			Zombie = false;
			Alive = false;
		}
	};
private:
	map<MUID, InfectS*> InfectionMap;
	InfectionState CurrentState;
	unsigned long int nLastStateTime;
protected:
	void SetState(InfectionState state, int Seconds) { CurrentState = state; nLastStateTime = timeGetTime() + (Seconds * 1000); }
	void RouteInfected(MUID Player);
	void RouteMessage(MUID Victim, int Type, MUID Attacker = MUID(0, 0));
	void SendQueueInfo(bool ToStage=true, MMatchObject* Peer = NULL);
	bool GetAliveCount(int* pRedAliveCount, int* pBlueAliveCount);
	bool Controller();
	virtual void OnBegin() { };
	virtual void OnEnd();
	virtual void OnEnterBattle(MUID& uidChar);
	virtual void OnLeaveBattle(MUID& uidChar);
	virtual bool OnRun();
	virtual void OnRoundBegin();
	virtual void OnRoundEnd();
	virtual bool OnCheckRoundFinish();
	virtual void OnRoundTimeOut();
	virtual bool RoundCount();
	virtual bool OnCheckEnableBattleCondition();
	virtual void OnGameKill(const MUID& uidAttacker, const MUID& uidVictim);
public:
	MMatchRuleInfection(MMatchStage* pStage);
	virtual ~MMatchRuleInfection()				{}
	virtual void CalcTeamBonus(MMatchObject* pAttacker, MMatchObject* pVictim,
								int nSrcExp, int* poutAttackerExp, int* poutTeamExp);
	virtual MMATCH_GAMETYPE GetGameType() { return MMATCH_GAMETYPE_INFECTED; }
	bool		IsDead(MUID uid);
};