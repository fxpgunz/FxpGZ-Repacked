#ifndef _MMATCHRULE_VAMPIRE_H
#define _MMATCHRULE_VAMPIRE_H


#include "MMatchRuleDeathMatch.h"

///////////////////////////////////////////////////////////////////////////////////////////////
class MMatchRuleVampire : public MMatchRuleSoloDeath  {
public:
	MMatchRuleVampire(MMatchStage* pStage);
	virtual ~MMatchRuleVampire() { }
	virtual MMATCH_GAMETYPE GetGameType() { return MMATCH_GAMETYPE_VAMPIRE; }
};


#endif