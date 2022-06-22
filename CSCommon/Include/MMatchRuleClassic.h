#ifndef _MMATCHRULE_CLASSIC_H
#define _MMATCHRULE_CLASSIC_H


#include "MMatchRuleDeathMatch.h"

///////////////////////////////////////////////////////////////////////////////////////////////
class MMatchRuleSoloClassic : public MMatchRuleSoloDeath  {
public:
	MMatchRuleSoloClassic(MMatchStage* pStage);
	virtual ~MMatchRuleSoloClassic() { }
	virtual MMATCH_GAMETYPE GetGameType() { return MMATCH_GAMETYPE_CLASSIC_SOLO; }
};

///////////////////////////////////////////////////////////////////////////////////////////////
class MMatchRuleTeamClassic : public MMatchRuleTeamDeath {
public:
	MMatchRuleTeamClassic(MMatchStage* pStage);
	virtual ~MMatchRuleTeamClassic()				{}
	virtual MMATCH_GAMETYPE GetGameType() { return MMATCH_GAMETYPE_CLASSIC_TEAM; }
};




#endif